/*
 *    main.c
 *
 *    Written, if that's the word, by Phil Bambridge
 *    with much help from John Honniball.
 *
 *    v1.0.0 - 2012/07/16
 *    v1.0.1 - 2012/07/19 (Changes to signal handling, swapped RS and EN pins for diagram ease)
 *    v1.0.2 - 2012/07/19 (Refactored to move rpi code to rpi.x, lcd code to lcd.x, added timeofday)
 *    v1.0.3 - 2012/07/21 (Rechecks for IP address, allows arbitrary datapins, works on 4 line LCD)
 *    v1.0.4 - 2012/07/22 (Reverted to inline define for CMDxBITS, fixed system load calculation)
 *    v1.0.5 - 2012/07/23 (Added support for multiple LCD modules)
 *    v1.0.5 - 2012/08/21 (Added support for BackLight LED)
 *    v1.0.6 - 2012/10/04 (Added support for temperature reading of DS18B20 from GPIO)
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/ioctl.h> 
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#include <arpa/inet.h>
#include <net/if.h>

#include <sys/sysinfo.h>
#include <sys/vfs.h>

#include <time.h>

#include <unistd.h>

#include <signal.h>

#include "lcd.h"
#include "w1therm.h"

#define SECS_IN_DAY 86400
#define SECS_IN_HOUR 3600
#define SECS_IN_MIN 60

#define DEG 223

typedef char ip_address[15+1];
struct lcdmodule module;

static int get_addr(char * ifname,ip_address theip);
void updateDisplay (struct lcdmodule module, unsigned int displayOption);
void sigintHandler(void);
void sigtermHandler(void);

#define inaddrr(x) (*(struct in_addr *) &ifr->x[sizeof sa.sin_port])
#define IFRSIZE   ((int)(size * sizeof (struct ifreq)))

int main(int argc, char **argv) {
  ip_address	theip;
  double temperature = 0.0;
  unsigned char i=0;
  //struct lcdmodule module2;

  // Here we create and initialise two LCD modules.
  // The arguments are, in order, EN, RS, D4, D5, D6, D7,LED.
                 // BCM pin asignment 
  //module = lcdInit(17, 18, 22, 23, 24, 25);
  // moje funkcni
  //module = lcdInit(8, 0, 23, 24, 25, 4, 18);
  // nove pro adafruit
  module = lcdInit(17, 18, 25, 22, 23, 24, 0);
  //module2 = lcdInit(21, 18, 22, 23, 24, 25);

  //prints(module2, "Ti goes with Pi.");

  // Register signal handlers to clear the screen before
  // shutting down. 
  signal(SIGINT, (__sighandler_t)sigintHandler);
  signal(SIGTERM, (__sighandler_t)sigtermHandler);

  // backlight ON
  backLightLED(module, 1);

  // try to read temperature
  temperature = w1therm();

  int displayOption = 0;
  while(1) {
    gotoXy(module, 0,0);
    prints(module, "                ");
    // TODO: This should be capable of blanking out a line wider than 16 chars.
    gotoXy(module, 0,0);
    // Print eth0's IP address in dotted-quad format
    if (get_addr("eth0",theip) == 0) {
      if (strlen(theip) < 13) {
         prints(module, "IP:");
      }
      prints(module, theip);
    } else if (get_addr("wlan0",theip) == 0) {
      if (strlen(theip) < 13) {
         prints(module, "WiFi:");
      }
      prints(module, theip);
    } else {
      prints(module, "IP unknown!");
    }
    for (i=0; i < 5; i++) 
    {
     updateDisplay(module, displayOption);
     if ( (displayOption == 1) && (temperature > -1000.0) ) {
       // print temperature instead of IP
       temperature = w1therm(); // read temperature
       if ( temperature > -100.0) {
        char T[17];

        sprintf(T,"%.1lf%cC",temperature,DEG);
        gotoXy(module, 0,0);
        prints(module, "                ");
        gotoXy(module, 0,0);
        prints(module, "T = ");
        prints(module, T);
       } else {
        temperature = 0;
       }
     }
     sleep(1);
    }
    displayOption++;
    if (displayOption > 3) { displayOption = 0; }
  }
  return 0;
} // main


void updateDisplay (struct lcdmodule module, unsigned int displayOption) { 
    struct sysinfo systeminfo;
    struct statfs diskinfo;
    struct tm *timestruct;
    time_t timenow;
    char displayBuffer[16+1];
    unsigned int days, hours, minutes;
    unsigned long uptime;

    gotoXy(module, 0,1);
    // Clear the bottom line
    prints(module, "                ");
    gotoXy(module, 0,1);

    switch (displayOption) {
        case 0:
            // Display current uptime
            // Shows days, hours, minutes if up longer than 0 whole days,
            // shows hours, minutes, seconds otherwise
            sysinfo(&systeminfo);
            uptime = systeminfo.uptime;
            
            prints(module, "up: ");

            days = uptime / SECS_IN_DAY;
            uptime -= days * SECS_IN_DAY;
            if (days) { integerToLcd(module, days, 1); prints(module, "d"); }

            hours = uptime / SECS_IN_HOUR;
            uptime -= hours * SECS_IN_HOUR;
            integerToLcd(module, hours, 1); prints(module, "h");

            minutes =  uptime / SECS_IN_MIN;
            integerToLcd(module, minutes, 1); prints(module, "m");

            if(!days) {
              uptime -= minutes * SECS_IN_MIN;
              integerToLcd(module, uptime, 2); prints(module, "s");
            }
            break;
        case 1:
           // System load 1, 5, 15 minute averages.
            sysinfo(&systeminfo);
            snprintf(displayBuffer, 16+1, "%0.2f %0.2f %0.2f", systeminfo.loads[0] / 65536.0, systeminfo.loads[1] / 65536.0, systeminfo.loads[2] / 65536.0);
            prints(module, displayBuffer);
            break;
        case 2:
          // Disk space free on / partition
          if (statfs("/", &diskinfo) == 0) {
            prints(module, "SD / :");
            integerToLcd(module, (int)(((float)(diskinfo.f_bavail) / (float)(diskinfo.f_blocks)) * 100), 1);
            prints(module, "% free");
          } else {
            prints(module, "Can't stat /!");
          }
          break;
        case 3:
          // Show current time of day
          timenow = time(0);
          timestruct = localtime(&timenow);
          // calendar day
          snprintf(displayBuffer, 16+1, "date: %02d.%02d.%04d", timestruct->tm_mday, timestruct->tm_mon + 1, timestruct->tm_year + 1900); 
          gotoXy(module, 0,0);
          // Clear the top line
          prints(module, "                ");
          gotoXy(module, 0,0);
          prints(module, displayBuffer);
          snprintf(displayBuffer, 16+1, "time: %02d:%02d:%02d", timestruct->tm_hour, timestruct->tm_min, timestruct->tm_sec);
          gotoXy(module, 0,1);
          prints(module, displayBuffer);
          break;
    }
  }


static int get_addr(char * ifname,ip_address theip) {

  struct ifreq		*ifr;
  struct ifreq		ifrr;
  struct sockaddr_in	sa;
  struct sockaddr	ifaddr;
  int			sockfd;

  if((sockfd=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP))==-1)
  {
	printf ("Socket for DGRAM cannot be created\n");
	return -1;
  }

  ifr = &ifrr;

  ifrr.ifr_addr.sa_family = AF_INET;

  strncpy(ifrr.ifr_name, ifname, sizeof(ifrr.ifr_name));

  if (ioctl(sockfd, SIOCGIFADDR, ifr) < 0) {
    // printf("No %s interface.\n", ifname);
    close(sockfd);
    return -1;
  }

  ifaddr = ifrr.ifr_addr;
     strncpy(theip,inet_ntoa(inaddrr(ifr_addr.sa_data)),sizeof(ip_address));
  close(sockfd);
  return 0;

}

void sigintHandler(void) {
  clearLcd(module);
  gotoXy(module, 0,0);
  prints(module, "-=Raspberry PI=-");
  gotoXy(module, 0,1);
  prints(module, "LCD info down...");
  exit(0);
}

void sigtermHandler(void) {
  clearLcd(module);
  gotoXy(module, 0,0);
  prints(module, "-=Raspberry PI=-");
  gotoXy(module, 0,1);
  prints(module, "Shutting down...");
  backLightLED(module, 0);
  exit(0);
}

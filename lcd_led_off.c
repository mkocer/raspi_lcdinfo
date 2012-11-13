/*
 *   lcd_led_on.c    
 *
 *   switch LCD_LED ON
 *
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
#include "rpi.h"

#define LED_PIN  0

int main(int argc, char **argv) {
 
  // Set up GPIO pointer for direct register access
  setup_io();
  // set pin as output
  INP_GPIO(LED_PIN);
  OUT_GPIO(LED_PIN); 
  // set the pin OFF
  GPIO_CLR = 1<<LED_PIN;
  return 0;
} // main



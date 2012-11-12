/*
   Reads temperature from 1-Wire bus slave 
   Dallas DS18B20 sensor

   using Raspberry PI GPIO and kernel modules. 
   Be sure the modules are loaded:
       modprobe wire 
       modprobe w1-gpio
       modprobe w1-therm

   Using Occidentalis - Adafruit Educational Linux Distro:
   http://learn.adafruit.com/adafruit-raspberry-pi-educational-linux-distro/occidentalis-v0-dot-1
 */

#include "w1therm.h"

double
w1therm (void)
{
  char c, sens = 0, crc = 0, buff[W1_BUFFMAX + 1], s[2 * W1_BUFFMAX + 1];
  char *pc;
  int i = 0;
  double T = -1000.0;
  FILE *fd;

  if ((fd = fopen (W1_SLAVESFILE, "r")) == NULL)
    {
      fprintf (stderr, "File %s not found!\n", W1_SLAVESFILE);
      return (T);
    }
  while (fgets (buff, W1_BUFFMAX, fd) != NULL)
    {
      //fprintf (stdout, "%s", buff);
      if (strstr (buff, "28-"))
	{
	  sens = 1;		//sensor is present
          *s = '\0';
          strcpy (s, W1_DEVICEBASE);
	  pc = strchr (buff, '\n');
	  if (pc)
	    {
	      *pc = '\0';
	    }
	  strcat (s, buff);
	}
    }
  fclose (fd);
  if (sens)
    {
      strcat (s, W1_TEMPFILE);
      i = 0;
      while ((T <= -1000.0) && i <= 3)
	{
	  /* open temperature file */
	  if ((fd = fopen (s, "r")) == NULL)
	    {
	      fprintf (stderr, "File %s not found!\n", s);
	      return (T);
	    }
	  while (fgets (buff, W1_BUFFMAX, fd) != NULL)
	    {
	      // fprintf (stdout, "%s", buff);
	      if ((pc = strstr (buff, "t=")) && (crc == 1))
		{
		  // temperature OK checked against CRC as well
		  T = (double) atoi (pc + 2) / 1000.0;
		}
	      if (strstr (buff, "crc=") && (strstr (buff, "YES")))
		{
		  // CRC OK
		  crc = 1;
		}
	      else
		{
		  // wrong CRC or other kind of line
		  crc = 0;
		}
	    }
	  fclose (fd);
	  i++;
	}			// while
    }
  return (T);
}

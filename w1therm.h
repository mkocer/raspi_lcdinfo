/*
   1-Wire bus slave routines for reading Dallas DS18B20 sensor 
   on pin GPIO 4 using Adafruit Educational Linux Distro.

   Asure that wire, w1-gpio and w1-therm kernel modules are loaded

 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include <math.h>
#include <errno.h>

#define W1_SLAVESFILE "/sys/bus/w1/devices/w1 bus master/w1_master_slaves"
#define W1_DEVICEBASE "/sys/bus/w1/devices/"
#define W1_TEMPFILE   "/w1_slave"

#define W1_BUFFMAX 512

/* returns the temperature on 1-Wire bus  DS18B20 sensor in Celsius */
/*    returns -1000.0 when the temperature was not read */
double w1therm(void);

/*
 *    lcd.c
 *
 *    Ported to Raspberry Pi, refactored and extended
 *    by Phil Bambridge and John Honniball
 *    Original MSP430 code: Gaurav www.circuitvalley.com
 */

#include "lcd.h"
#include "rpi.h"
#include <unistd.h>

void lcdCmd(struct lcdmodule module, unsigned char cmd) {

  GPIO_CLR = 1<<module.rs; //because sending command
  GPIO_CLR = 1<<module.en;

  GPIO_CLR = CMDLOWBITS(0xF);

  // Setting the pins for the high 4 bits of the byte of command
  GPIO_SET = CMDHIGHBITS(cmd);
  // Pulse EN to say we have pins to read
  GPIO_SET = 1<<module.en;
  waitLcd(1);
  GPIO_CLR = 1<<module.en;
  waitLcd(41);

  GPIO_CLR = CMDLOWBITS(0xF);
  // Setting the pins for the low 4 bits of the byte of command
  GPIO_SET = CMDLOWBITS(cmd);
  // Pulse EN to say we have pins to read
  GPIO_SET = 1<<module.en;
  waitLcd(1);
  GPIO_CLR = 1<<module.en;
  waitLcd(41);
}


void lcdData(struct lcdmodule module, unsigned char data) {

  GPIO_SET = 1<<module.rs;  //because sending data
  GPIO_CLR = 1<<module.en;

  GPIO_CLR = CMDLOWBITS(0xF);
  // Setting the pins for the high 4 bits of the byte of data
  GPIO_SET = CMDHIGHBITS(data);
  // Pulse EN to say we have pins to read
  GPIO_SET = 1<<module.en;
  waitLcd(1);
  GPIO_CLR = 1<<module.en;
  waitLcd(41);

  GPIO_CLR = CMDLOWBITS(0xF);
  // Setting the pins for the low 4 bits of the byte of data
  GPIO_SET = CMDLOWBITS(data);
  // Pulse EN to say we have pins to read
  GPIO_SET = 1<<module.en;
  waitLcd(1);
  GPIO_CLR = 1<<module.en;
  waitLcd(41);
}

struct lcdmodule lcdInit(unsigned char en, unsigned char rs, unsigned char d4, unsigned char d5, unsigned char d6, unsigned char d7, unsigned char led) {

  struct lcdmodule module;

  module.rs = rs;
  module.en = en;
  module.datapins[0] = d4; module.datapins[1] = d5; module.datapins[2] = d6; module.datapins[3] = d7;
  module.led = led;

  // Set up gpi pointer for direct register access
  setup_io();

  // Set our required pins as outputs
  INP_GPIO(module.rs); // must use INP_GPIO before we can use OUT_GPIO
  INP_GPIO(module.en);
  OUT_GPIO(module.rs);
  OUT_GPIO(module.en);
  if (module.led < 255) {
    INP_GPIO(module.led);
    OUT_GPIO(module.led);
  }

  // Data lines for 4-bit operation are D4 to D7 inclusive on
  // the LCD module. They are assigned to 4 GPIO pins in datapins[]
  // where datapins[0] is D4, and datapins[3] is D7.
  // Which GPIO pins these are is set in lcd.h
  int g;
  for (g = 0; g <= 3 ; g++) {
    INP_GPIO(module.datapins[g]);
    OUT_GPIO(module.datapins[g]);
  }

  GPIO_CLR = 1<<module.rs; // Send command flag
  GPIO_CLR = 1<<module.en;

  // Datasheet says we have to set this 3 times, with an initial gap
  // of 4.1ms, then 100us.
  GPIO_CLR = CMDLOWBITS(0xF);
  GPIO_SET = CMDHIGHBITS(LCD_FUNCTIONSET | LCD_8BITMODE);
  GPIO_SET = 1<<module.en;
  waitLcd(1);
  GPIO_CLR = 1<<module.en;
  waitLcd(4200);
  GPIO_SET = 1<<module.en;
  waitLcd(1);
  GPIO_CLR = 1<<module.en;
  waitLcd(150);
  GPIO_SET = 1<<module.en;
  waitLcd(1);
  GPIO_CLR = 1<<module.en;
  waitLcd(150);

  // Now we tell it to go into 4 bit mode.
  // From then on, all commands and data are sent as two 4 bit chunks, so we
  // can use our in-built lcdCmd() function.
  GPIO_CLR = CMDLOWBITS(0xF);
  GPIO_SET = CMDHIGHBITS(LCD_FUNCTIONSET | LCD_4BITMODE);
  GPIO_SET = 1<<module.en;
  waitLcd(1);
  GPIO_CLR = 1<<module.en;
  waitLcd(41);

  lcdCmd(module, LCD_FUNCTIONSET | LCD_2LINE);

  lcdCmd(module, LCD_DISPLAYCONTROL | LCD_DISPLAYON | LCD_BLINKOFF);

  lcdCmd(module, LCD_CLEARDISPLAY);
  waitLcd(1600); // This command can take a lot longer to run

  lcdCmd(module, LCD_ENTRYMODESET | LCD_ENTRYLEFT);

  lcdCmd(module, LCD_SETDDRAMADDR | 0x0);

  return module;
}

void waitLcd(volatile unsigned int usec) {
  usleep(usec);
}

void prints(struct lcdmodule module, char *s) {

  while (*s) {
    lcdData(module, *s);
    s++;
  }
}

void gotoXy(struct lcdmodule module, unsigned char x, unsigned char y) {
  if(x < 40) {
    int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
    if (y >= 4 ) {
      y = 3;
    }
    y = (x + row_offsets[y]);

    lcdCmd(module, LCD_SETDDRAMADDR | y);
  }
}

void integerToLcd(struct lcdmodule module, unsigned int intnum, unsigned char zeroes) {
  unsigned char pos, started, digit;
  unsigned int x;

  pos = 10;
  started = 0;
  for (x=1000000000; x >= 1; x /= 10) {
    // The implicit cast to integer means we lose the remainder,
    // just leaving us with the number of whole billions.
    // Next iteration, we'll count the hundreds of millions, etc.
    digit = intnum / x;
    intnum -= digit * x;
    // Here we skip any unwanted leading zeroes, but a zero in the middle
    // of the number has to be displayed! Also, make sure we display
    // "zeroes" number of leading zeroes.
    if (digit || started || (pos <= zeroes)) {
       lcdData(module, digit + '0');
       started = 1;
    }
    pos--;
  }
}

void clearLcd(struct lcdmodule module) {
  lcdCmd(module, LCD_CLEARDISPLAY);
  waitLcd(1600); // This command can take a lot longer to run
}

void backLightLED(struct lcdmodule module, unsigned char state) 
{
  if (module.led < 255) {
     if (state)  {
       GPIO_SET = 1<<module.led;        
     }else {
       GPIO_CLR = 1<<module.led;        
     }
  }
}

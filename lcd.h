/*
 *    lcd.h
 *
 *    Ported to Raspberry Pi, refactored and extended
 *    by Phil Bambridge and John Honniball
 *    Original MSP430 code: Gaurav www.circuitvalley.com
 */

#define CMDHIGHBITS(cmd) (((1<<0 & ((cmd)>>4 & 0x0F)) >> 0) << module.datapins[0]) | (((1<<1 & ((cmd)>>4 & 0x0F)) >> 1) << module.datapins[1]) | (((1<<2 & ((cmd)>>4 & 0x0F)) >> 2) << module.datapins[2]) | (((1<<3 & ((cmd)>>4 & 0x0F)) >> 3) << module.datapins[3])
#define CMDLOWBITS(cmd) (((1<<0 & ((cmd) & 0x0F)) >> 0) << module.datapins[0]) | (((1<<1 & ((cmd) & 0x0F)) >> 1) << module.datapins[1]) | (((1<<2 & ((cmd) & 0x0F)) >> 2) << module.datapins[2]) | (((1<<3 & ((cmd) & 0x0F)) >> 3) << module.datapins[3])

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

struct lcdmodule {
  unsigned char rs; // Which GPIO pin is Register Selectt on the LCD module?
  unsigned char en; // Which GPIO pin is Enable?
  unsigned char datapins[4]; // List of GPIO pins corressponding to D4, D5, D6, D7.
  unsigned char led; // Which GPIO pin is backlight LED anode (cathode should be grounded)
                     // 255 if disabled
};

struct lcdmodule lcdInit(unsigned char en, unsigned char rs, unsigned char d4, unsigned char d5, unsigned char d6, unsigned char d7, unsigned char led);
void integerToLcd(struct lcdmodule module, unsigned int integer, unsigned char zeroes);
void lcdData(struct lcdmodule module, unsigned char data);
void lcdCmd(struct lcdmodule module, unsigned char cmd);
void prints(struct lcdmodule module, char *s);
void gotoXy(struct lcdmodule module, unsigned char  x,unsigned char y);
void waitLcd(volatile unsigned int usec);
void clearLcd(struct lcdmodule module);
void backLightLED(struct lcdmodule module, unsigned char state);

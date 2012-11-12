all: lcdinfo

lcdinfo: main.c lcd.c rpi.c w1therm.c w1therm.h lcd.h rpi.h
	gcc -O2 -o lcdinfo main.c lcd.c rpi.c w1therm.c

lcd_led_on: lcd_led_on.c
	gcc -O2 -o lcd_led_on lcd_led_on.c lcd.c rpi.c
lcd_led_off: lcd_led_off.c
	gcc -O2 -o lcd_led_off lcd_led_off.c lcd.c rpi.c

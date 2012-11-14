Prints some information of Raspberry Pi on LCD display
======================================================

LCD display
-----------
*LCD display is supposed to be Hitachi HD44780 compatiple*

The display is connected to GPIO -  using "Dom and Gert van Loo" C-code rpi.c/rpi.h

Original Project
----------------

Project based on lcdinfo from Phil's blog: http://pbambridge.brunelweb.net/blog/?p=64
The scheme picture Pi_LCD2_bb.png is copied from that blog.  But we use other pins 
for connecting the display, see the function *lcdInit()* in *main.c*.

Raspberry Pi distro used
------------------------

We are using Occidentalis - _Adafruit Educational Linux Distro_:
http://learn.adafruit.com/adafruit-raspberry-pi-educational-linux-distro/
with "One wire support on GPIO #4 when loaded"
https://github.com/FrankBuss/linux-1/commit/71871509238d3e7bce4a74cdf616c3f12542acaa

One wire bus here is  used for DS18B20 temp sensor.
The Pi does not have 'hardware' 1-wire support but it can bitbang it with some success. 
Connect a DS18B20 with VCC to 3V, ground to ground and Data to GPIO #4. 
Then connect a 4.7K resistor from Data to VCC. (Adafruit)

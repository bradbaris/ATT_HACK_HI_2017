# ATTHACK2017
AT&amp;T Hackathon Honolulu 2017 Entry — **Won Most Technically Challenging Implementation!**

![](/art/demo.gif)  
RFID reader in action (via [@aisis](https://www.github.com/aisis))  

![](/art/slack_screenshot.png)
![](/art/dashboard_concept.png)  
Arduino output in Slack, and dashboard art  

# Description

Standard assembly of an Arduino Uno (WiFi, this case), Adafruit PN532 RFID/NFC reader/writer shield, and an Adafruit RGB LCD shield. Shields for convenience and speed, because we are noob. We used [totally-insecure](https://en.wikipedia.org/wiki/MIFARE#Security_of_MIFARE_Classic.2C_MIFARE_DESFire_and_MIFARE_Ultralight) MIFARE 1K cards to play with. Much of the basic structure of MIFARE 1K cards can be learned via the [`mifareclassic_memdump`](https://github.com/adafruit/Adafruit-PN532/blob/master/examples/mifareclassic_memdump/mifareclassic_memdump.pde) sketch in the Adafruit PN532 library, and the [MIFARE byte layout diagram](https://en.wikipedia.org/wiki/MIFARE#/media/File:MiFare_Byte_Layout.png).

Setup:

1. Put together the Arduino module  
   *  Align the pinouts of the PN532 to the Arduino Uno, and connect it with a set of stacking headers. Later sauter these headers to the PN532 shield.
   *  Assemble and sauter together the RGB LCD shield: resistors, buttons, potentiometer, MCP23017, LCD display.
   *  Align the LCD shield to the stacked headers of the PN532 shield (sans 2 pins), and sauter a row of male headers to the LCD shield.
   *  Connect the PN532 shield on top of the Arduino Uno, then connect the RGB LCD shield on top of the PN532 shield.  
2. Install the [Arduino IDE](https://www.arduino.cc/en/main/software)  
3. Burn sketches to the Arduino  

**See the sibling repo:** https://github.com/bradbaris/ATT_HACK_HI_2017_RPi2

Notes:

[1] https://github.com/adafruit/Adafruit-PN532/blob/master/examples/readMifare/readMifare.pde  
[2] https://github.com/adafruit/Adafruit-RGB-LCD-Shield-Library/blob/master/examples/HelloWorld/HelloWorld.ino  
[3] https://github.com/adafruit/Adafruit-PN532/blob/master/examples/mifareclassic_memdump/mifareclassic_memdump.pde  
[4] https://en.wikipedia.org/wiki/MIFARE#/media/File:MiFare_Byte_Layout.png  

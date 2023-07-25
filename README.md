# EEPROM-PROGRAMMER

-Based off Ben eater EEPROM Programmer
https://www.hobbytronics.co.uk/arduino-serial-buffer-size#:~:text=The%20Arduino%20core%20code%20contains,only%2064%20bytes%20in%20size.

I originaly began working on this because I didn't know how one could upload 6502 assembly onto an EEPROM without the TL866- and you best believe im not paying that $60+! What this EEPROM programmer does is use the pySerial api to communicate via USB to the Arduino Nano. THrough this it can send 6502 assembly to the EEPROM. 


If you want something more reliable that will likkely work better for you and for a wider arrange of EEPROMs for the love of god just use the TommyPROM. I only realized its existence while making this EEPROM programmer, but you know what they say: you got to see it through.

Commands:
```
Write [file]
Write address [address:data]
Read address [address]
Read EEPROM
Read Block [address (HEX):address (HEX)]
Hexdump [file]
Erase EEPROM
Print [file]
help
```
Example command call in the terminal:
`python Arduino.py Read EEPROM`

# Configuring the programmer
You should put the python script in the same directory that you assemble your 6502 scripts-or where the a.out file is produced. You should also make sure to go into the Arduino.py file and change the the com port according to what arduino com you are using. Also make sure to go into the EEPROM_programmer file and change the EEPROM_Size according to what EEPROM you are using( I have the at28c64b). Lastly you should connect an LED to the Indicator light as it indicates itf the Arduino is still writing/ reading a program, or if it received the message at all. Debugging tip: unplugging and replugging/reprogrmaing in the arduino can fix the problem sometimes-likely due to the horrible way I managed the heap storage while making this.




If you actually plan to use this and need help you can email me here if you have any trouble: nurpsbob@gmail.com

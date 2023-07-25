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
You should put the python script in the same directory that you assemble your 6502 scripts-or where the a.out file is produced.

Example command call in the terminal:
`python Arduino.py Read EEPROM`



If you actually plan to use this and need help you can email me here if you have any trouble: nurpsbob@gmail.com

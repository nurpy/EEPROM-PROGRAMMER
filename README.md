# EEPROM-PROGRAMMER

-Based off Ben eater EEPROM Programmer
https://www.hobbytronics.co.uk/arduino-serial-buffer-size#:~:text=The%20Arduino%20core%20code%20contains,only%2064%20bytes%20in%20size.

I originaly began working on this because I didn't know how one could upload 6502 assembly onto an EEPROM without the TL866- and you best believe im not paying that $60+! 

If you want something more reliable that will likkely work better for you and for a wider arrange of EEPROMs for the love of god just use the TommyPROM. I only realized its existence while making this EEPROM programmer, but you know what they say: you got to see it through.



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

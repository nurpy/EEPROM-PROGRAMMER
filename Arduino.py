import serial
import time
from hexdump import hexdump
import sys


arduino = serial.Serial()
arduino.port = "COM4" ##com port for arduino
arduino.baudrate = 9600
arduino.timeout = 1
arduino.setDTR(False)
arduino.open()



def writeToArduino(x):
    arduino.write(x.encode())
    time.sleep(0.05)
    

def getAllContents():
    exitcase=True
    data=""
    response=""
    while(exitcase):
        data = arduino.read(128)
        data=data.decode()
        if data=="":
            exitcase=False
        
        response= response+data
    return response
def getAddress(data):
    data=data.replace("[","")
    data=data.replace("]","")
    data=data.replace("0x","")
    data=str(data)
    writeToArduino("readAddress")
    bufout=getAllContents()
    
   
    
    writeToArduino(data)
    bufout= getAllContents()
    print(f"value in address {hex(int(data))} (HEX) {hex(int(bufout))}")
    
def eraseEEPROM():
    writeToArduino("EraseEEPROM")
    bufout=getAllContents()
    print("filled EEPROM with ea's")
    
def writeAddress(data):
    data=data.replace("[","")
    data=data.replace("]","")
    data=data.replace("0x","")

    writeToArduino("writeAddress")
    bufout=getAllContents()
    writeToArduino(data)
    bufout=getAllContents()
    print("wrote value to EEPROM")


def writeEntireEEPROM(FileName):
    writeToArduino("TOTALWRITE")
    FileName=FileName.replace("[","")
    FileName=FileName.replace("]","")
    bufout=getAllContents()
    sendMessage(FileName)
    bufout=getAllContents()

    

def viewEEProm(data):
    data=data.replace("[","")
    data=data.replace("]","")
    data=data.replace("0x","")
    writeToArduino("printEEPROM")
    bufout=getAllContents()
    writeToArduino(data)
    bufout=getAllContents()
    print(bufout)
 

    
def ViewFile(FileName): #reads a.out
    file = open("a.out", "rb")
    print("These are the contents of a.out binary file")
    print(hexdump(file.read(),0x00000000))
    file.close
    
def CreateHexFile(HexTextFileName):
    file = open("a.out", "rb")
    s=(hexdump(file.read(),0x00000000))
    print(s)
    print(f"Wrote contents to {HexTextFileName}")
    print(s, file=open(HexTextFileName,'w'))
    file.close()
    
def formatMessage(FileName):
    messages=[]
    HexTextFileName=FileName
    CreateHexFile(HexTextFileName)
    file=open(HexTextFileName,'r')
    hexarr=[]
    check=False
    startadress=0
    message=""
    separations=[]
    for line in file:
        line=line.strip()
        hexarr.append(line)
    file.close()
    
    file=open(HexTextFileName,'r')
    iter=0
    for line in file:
        if line.find("*") ==0:
            hexstart = hexarr[iter-1].split()
            first_word= "0x" +hexstart[0]
            first_word=int(first_word, 16)
            
            hexend = hexarr[iter+1].split()
            end_word= "0x" + hexend[0]
            end_word=int(end_word, 16)
            
            #print(hex(end_word-first_word,16))
            val=int(((end_word-first_word)))
            val=val-16
            
            messages.append(f"*{val}*")
            
            message=f"{message} *{val}*"
            separations.append((val-16))
            hexlevel=first_word
            (firstWord, rest) = hexarr[iter-1].split(maxsplit=1)
            
        else:
            line=line.strip()
            line=line.split()
            line.pop(0)
            if len(line)==0:
                continue
            mline=" ".join(line)
            mline=mline[0:mline.find("|")-1]
            messages.append(mline)
            
        iter=iter+1        
            
    file.close()
    return messages

def sendMessage(FileName):
    messages=formatMessage(FileName)
    arduino_parsing_commands = {"special continue":"SCON",
                                "continue":"CONT",
                                "finished":"DONE"}
    Omessages=[]
    try:
        longMessage=""
        iter=0
        while(1 != None): 
            hexcode=messages.pop(0)
            
            if len(messages) ==0:#send message with end type
                longMessage= f"{longMessage} {hexcode}"
                Omessages.append(f"{longMessage} {arduino_parsing_commands['finished']}")
                continue

            
            
            if hexcode.find("*") >-1:#send what we have with cont(spec) type then send astericks with cont type
         
                Omessages.append(f"{longMessage} {arduino_parsing_commands['special continue']}")
                hexcode=hexcode.replace("*","")
                Omessages.append(f"{longMessage[-2:]} {hexcode} {arduino_parsing_commands['continue']}")
                
                
                iter=0
                longMessage=""
                continue
            
                

            if(iter ==6):#send message with cont type ##
                Omessages.append(f"{longMessage} {arduino_parsing_commands['special continue']}")

                iter=0
                longMessage=""
            iter+=1
            longMessage= f"{longMessage} {hexcode}"
            

        
    except IndexError:
        
        for message in Omessages:
            writeToArduino(message)
            (getAllContents())
        print("Wait for led to turn off This indicates that Writing is finished")
            
    
    

def eraseEEPROM():
    writeToArduino("EraseEEPROM")
    bufout=getAllContents()
    print("filled EEPROM with ea")


            
def parseCommand():###Simpole command reader
    command=sys.argv
    

    if(command[0] == "help"):
        print("callable commands are:")
        print('''
                Write [file]
                Write address [address:data]
                Read address [address]
                Read EEPROM
                Read Block [address (HEX):address (HEX)]
                Hexdump [file]
                Erase EEPROM
                Print [file]
                help''')
    else:
        if(command[0] =="Write"):
            if command[1] == "address":
                writeAddress(command[2])
            else:
                writeEntireEEPROM(command[1])
            
            
                    
        elif(command[0] =="Read"):
            
            if command[1] == "address":
                getAddress(command[2])
                
            elif command[1] == "Block":
                viewEEProm(command[2])
                
            elif command[1] == "EEPROM":
                viewEEProm("ENTIRE")
            

            else:
                raise Exception("Read ----- ------ Command Not Understood")

        elif(command[0] =="Print"):
            ViewFile(command[1])
        elif(command[0] =="Hexdump"):
            CreateHexFile(command[1])
        elif(command[0] == "Erase"):
            eraseEEPROM()
          
        else:
            raise Exception("Command Not Found -- use command: (python [python file name] help) to get a list of commands")

    print("Done with command")
    
def main():
    sys.argv.pop(0)
    parseCommand()
    

if __name__ == "__main__":
    main()


arduino.close()
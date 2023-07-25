
#define SHIFT_DATA 2
#define SHIFT_CLK 3
#define SHIFT_LATCH 4
#define EEPROM_D0 5
#define EEPROM_D7 12
#define WRITE_EN 13

#define EEPROM_SIZE 8192
#define dataBuf 96
#define INDICATOR_LIGHT A3
int data[dataBuf];
String endCommand="CONT";


/*
 * Output the address bits and outputEnable signal using shift registers.
 */
void setAddress(int address, bool outputEnable) {
  shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, (address >> 8) | (outputEnable ? 0x00 : 0x80));
  shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, address);

  digitalWrite(SHIFT_LATCH, LOW);
  digitalWrite(SHIFT_LATCH, HIGH);
  digitalWrite(SHIFT_LATCH, LOW);
}


/*
 * Read a byte from the EEPROM at the specified address.
 */
byte readEEPROM(int address) {
  for (int pin = EEPROM_D0; pin <= EEPROM_D7; pin += 1) {
    pinMode(pin, INPUT);
  }
  setAddress(address, /*outputEnable*/ true);

  byte data = 0;
  for (int pin = EEPROM_D7; pin >= EEPROM_D0; pin -= 1) {
    data = (data << 1) + digitalRead(pin);
  }
  return data;
}


/*
 * Write a byte to the EEPROM at the specified address.
 */
void writeEEPROM(int address, byte data) {
  setAddress(address, /*outputEnable*/ false);
  for (int pin = EEPROM_D0; pin <= EEPROM_D7; pin += 1) {
    pinMode(pin, OUTPUT);
  }

  for (int pin = EEPROM_D0; pin <= EEPROM_D7; pin += 1) {
    digitalWrite(pin, data & 1);
    data = data >> 1;
  }
  digitalWrite(WRITE_EN, LOW);
  delayMicroseconds(1);
  digitalWrite(WRITE_EN, HIGH);
  delay(10);
}

void printContents(int start, int end) {// start and end must be less than 256
  int digitsAwayFromBase=start%16; // just for fromat
  for (int base = start-digitsAwayFromBase; base <= end; base += 16) {
    byte data[16];
    for (int offset = 0; offset <= 15; offset += 1) {
      if(base+offset<start || base+offset>end){data[offset]=218; continue;}// DA did not ask
      data[offset] = readEEPROM(base + offset);
    }

    char buf[80];
    sprintf(buf, "%03x:  %02x %02x %02x %02x %02x %02x %02x %02x   %02x %02x %02x %02x %02x %02x %02x %02x",
            base, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7],
            data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15]);

    Serial.println(buf);
  }
}
int power(int number,int exponent){
int val=1;
for(int i=0;i<exponent;i++){
  val*=number;

}
return val;
}
int convertToHex(String s){
int val=0;
int charval=0;
int exponenet=s.length()-1;
//String test=" abcdef";
 for(byte r=0; r<s.length(); r++){
     
     charval=s[r]-'0';
     if (charval> 10){ //added a space so first index of letter is 1
      charval=s[r]-'`';
      charval=(power(16,exponenet) * (charval+9));
      exponenet--;
      val+=charval;
     }
     else{
       charval=(power(16,exponenet) *((s[r])-'0'));
       exponenet--;
       val+=charval;
     }
 }
 return val;
}
void processStringS(String s){
  String temp=s;
  int element=0;
  while(temp.indexOf(' ')>-1){
    temp.remove(temp.indexOf(' '),1);
  }
  endCommand=temp.substring(temp.length()-4,temp.length());
  temp.remove(temp.length()-4,5);
      
  
  String results="00";
  results[0]=temp[0];
  results[1]=temp[1];
  data[0]=(convertToHex(results));

  temp.remove(0,2);

  data[1]=((temp).toInt());
    
  

}

int processString(String s){
  String temp=s;
  int element=0;
  while(temp.indexOf(' ')>-1){
    temp.remove(temp.indexOf(' '),1);
  }
  endCommand=temp.substring(temp.length()-4,temp.length());
  temp.remove(temp.length()-5,5);
      
  for(int i=0;i<temp.length()-1;i+=2){
    String results="00";
    results[0]=temp[i];
    results[1]=temp[i+1];
    data[element]=(convertToHex(results));
    element++;
  }
 
  return element+1;
}
int blockWrite(int numberOfWrites, int curAdress, int prevCommand){
  Serial.println("writing");
  for(int i=0;i<numberOfWrites;i++){
    writeEEPROM(curAdress+i,prevCommand);
  }
  curAdress+=numberOfWrites;
  return curAdress;

}
int blockWrite(int numberOfWrites, int curAdress){
  Serial.println("writing");
  for(int i=0;i<numberOfWrites;i++){
    writeEEPROM(curAdress+i,data[i]);
  }
  curAdress+=numberOfWrites;
  return curAdress;

}


void decideAction(String x){
  digitalWrite(INDICATOR_LIGHT,HIGH);
  Serial.print("gotcha");

  
  if(x=="EraseEEPROM"){
    EraseEEPROM();
    digitalWrite(INDICATOR_LIGHT,LOW);
  }
  if(x=="readAddress"){
    while (!Serial.available());
    String response = Serial.readString();
    digitalWrite(INDICATOR_LIGHT,LOW);
    
    Serial.println(readEEPROM(response.toInt()));
  }
  if(x=="writeAddress"){
    while (!Serial.available());
    digitalWrite(INDICATOR_LIGHT,LOW);
    String response = Serial.readString();
    

    int middle=response.indexOf(":");
    int start=convertToHex((response.substring(0,middle)));
    response=(response.substring(middle+1));
    int end=convertToHex((response));
    
    
    writeEEPROM(start,end);

  }
  if(x=="printEEPROM"){
    while (!Serial.available());
    String response = Serial.readString();
    digitalWrite(INDICATOR_LIGHT,LOW);

    if(response == "ENTIRE"){
      printContents();
    }
    else{
      
      int middle=response.indexOf(":");
      int start=convertToHex((response.substring(0,middle)));
      
      response=(response.substring(middle+1));
     
      int end=convertToHex((response));
      
      while(end-start != 0){
          int jumpAmount=256;
          if(end-start<jumpAmount){
            jumpAmount=end-start;
          }
          int nextblock=start+jumpAmount;
          printContents(start,nextblock);
          start+=jumpAmount;
     
    }
    }
  }
    

  if(x=="TOTALWRITE"){
    
    Serial.println("started");

    int curAdress=0;
    while (!Serial.available());
      String response = Serial.readString();
      
      
      int iterations = processString(response);
      curAdress=blockWrite(iterations,0);
      // write them in
      
      

      while(endCommand != "DONE"){
        Serial.println("going");
        
        if(endCommand == "CONT"){
          while (!Serial.available());
          String response = Serial.readString();
          int iterations = processString(response);
          curAdress=blockWrite(iterations,curAdress);
          
        }
        if(endCommand == "SCON"){
          while (!Serial.available());
          String response = Serial.readString();
          processStringS(response);
          int  previous_command = data[0];
          int repetitions = data[1];
          curAdress=blockWrite(repetitions,curAdress,previous_command);
          

        }

      }
  digitalWrite(INDICATOR_LIGHT,LOW);
  Serial.println("done with total Write");    
// process response    
  }
}
    
  
/*
 * Read the contents of the EEPROM and print them to the serial monitor.
 */
void printContents() {
  for (int base = 0; base <= 8191; base += 16) {
    byte data[16];
    for (int offset = 0; offset <= 15; offset += 1) {
      data[offset] = readEEPROM(base + offset);
    }

    char buf[80];
    sprintf(buf, "%03x:  %02x %02x %02x %02x %02x %02x %02x %02x   %02x %02x %02x %02x %02x %02x %02x %02x",
            base, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7],
            data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15]);

    Serial.println(buf);
  }
}


// 4-bit hex decoder for common anode 7-segment display
byte datahex[] = { 0x81, 0xcf, 0x92, 0x86, 0xcc, 0xa4, 0xa0, 0x8f, 0x80, 0x84, 0x88, 0xe0, 0xb1, 0xc2, 0xb0, 0xb8 };

// 4-bit hex decoder for common cathode 7-segment display
// byte data[] = { 0x7e, 0x30, 0x6d, 0x79, 0x33, 0x5b, 0x5f, 0x70, 0x7f, 0x7b, 0x77, 0x1f, 0x4e, 0x3d, 0x4f, 0x47 };

void EraseEEPROM(){
  for (int address = 0; address <= EEPROM_SIZE; address += 1) {
    writeEEPROM(address, 0xea);

    if (address % 64 == 0) {
      Serial.print(".");
    }
  }
}
void setup() {
  // put your setup code here, to run once:
  pinMode(SHIFT_DATA, OUTPUT);
  pinMode(SHIFT_CLK, OUTPUT);
  pinMode(SHIFT_LATCH, OUTPUT);
  digitalWrite(WRITE_EN, HIGH);
  pinMode(WRITE_EN, OUTPUT);
  Serial.begin(9600);
  Serial.setTimeout(100);
  pinMode(INDICATOR_LIGHT,OUTPUT);
/*

  // Program data bytes
  Serial.print("Programming EEPROM");
  for(int i=0;i<8192;i+=16){
  for (int address = 0; address < sizeof(data); address += 1) {
    writeEEPROM(address+i, data[address]);

    if (address % 64 == 0) {
      Serial.print(".");
    }
  }
  }
  */
  Serial.println(" done");


  // Read and print out the contents of the EERPROM
  Serial.println("Reading EEPROM");
  //printContents();
}


void loop() {
  // put your main code here, to run repeatedly:
  while (!Serial.available());
  String x = Serial.readString();
  decideAction(x);  

}

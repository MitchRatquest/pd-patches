
#include "Manchester.h"
#define RX_PIN 3
uint8_t moo = 1;
#define BUFFER_SIZE 22
uint8_t buffer[BUFFER_SIZE];

byte TURNON[22] = {11, 't', 'u', 'r', 'n', 'o', 'n', 'l', 'a', 'm', 'p'};
byte MORSECODE[22] = {10, 'm', 'o', 'r', 's', 'e', 'c','o','d','e'};
byte TURNOFF[22] = {12, 't','u','r','n','o','f','f','l','a','m','p'};
byte statusflag=0;  //0 = off, 1 = on, 2=morsecode

void setup() {
  Serial.begin(115200);
  man.setupReceive(RX_PIN, MAN_4800);
  man.beginReceiveArray(BUFFER_SIZE, buffer);
  
  pinMode(2, OUTPUT);  //relay
  digitalWrite(2, 0);  //turn low at first
}

void loop() {

  if (man.receiveComplete()) 
  {
    uint8_t receivedSize = 0;

    //do something with the data in 'buffer' here before you start receiving to the same buffer again
    receivedSize = buffer[0];  //first entry is length
    Serial.print(receivedSize);
    Serial.print(": ");
    for(uint8_t i=1; i<receivedSize; i++)
      Serial.write(buffer[i]);
    
    Serial.println();
    if(check(buffer, MORSECODE, 10))
    {
      statusflag=2;
    }
    if(check(buffer, TURNON, 11))
    {
      statusflag=1;
    }
    if(check(buffer, TURNOFF, 12))
    {
      statusflag=0;
    }

    
    man.beginReceiveArray(BUFFER_SIZE, buffer);

  }

 switch (statusflag)
 {
    case 0:
      digitalWrite(2, 0);
      break;
    case 1:
      digitalWrite(2, 1);
      break;
    case 2:
      morseCode();
      break;
      
 }
}

int check(byte input[], byte compare[], int n)
{
  for(int i = 0; i < n - 1; i++)
  {
    if(input[i] != compare[i])
    {
      return 0;  //NOT THE SAME
    }
  }
  return 1; //THE SAME ARRAY
}

void morseCode()
{
  digitalWrite(2, 0);
  delay(200);
  digitalWrite(2,1);
  delay(200);
}


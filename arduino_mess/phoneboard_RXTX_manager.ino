
#include <Manchester.h>

/*
  Manchester Transmitter example
  In this example transmitter will send 10 bytes array  per transmission
  try different speeds using this constants, your maximum possible speed will 
  depend on various factors like transmitter type, distance, microcontroller speed, ...

  MAN_300 0
  MAN_600 1
  MAN_1200 2
  MAN_2400 3
  MAN_4800 4
  MAN_9600 5
  MAN_19200 6
  MAN_38400 7

*/

#define TX_PIN  7 //pin where your transmitter is connected
///RECEIVE SHIT HERE
#define RX_PIN 2
#define BUFFER_SIZE 22
uint8_t buffer[BUFFER_SIZE];


/////////TRANSMIT BELOW HERE
uint8_t data[40] = {11, 'T','U', 'R', 'N', '_', 'O', 'F', 'F', '9','1','2','3','4','5','6','7','8','9'};
uint8_t datalength=0;

uint8_t serialinput[40];
byte gotserial=0;
int serialsize=0;

byte TURNON[22] = {11, 't', 'u', 'r', 'n', 'o', 'n', 'l', 'a', 'm', 'p'};
byte MORSECODE[22] = {10, 'm', 'o', 'r', 's', 'e', 'c','o','d','e'};

byte RESET_BOARD[22] = {14, 'r','e','s','e','t','t','h','e','p','h','o','n','e'};

byte RESETFLAG = 1; //HAS THIS BEEN RESET BY CONTRTOLLER? 1 = GOOD TO GO

void setup() {
  man.setupTransmit(TX_PIN, MAN_4800);
  man.setupReceive(RX_PIN, MAN_4800);
  man.beginReceiveArray(BUFFER_SIZE, buffer);
  Serial.begin(115200);
  Serial.setTimeout(50);  
  pinMode(4, INPUT);
}

void loop() {
  if(digitalRead(4) && RESETFLAG )  //CANNOT SPAM MORSE CODE
  {
    man.stopReceive();
    datalength = MORSECODE[0];
    man.transmitArray(datalength, MORSECODE);
    RESETFLAG =0;
    man.beginReceiveArray(BUFFER_SIZE, buffer);
  }

  if (man.receiveComplete()) 
  {
    uint8_t receivedSize = 0;
    receivedSize = buffer[0];
    if(check(buffer, RESET_BOARD, 14))  //if controller gives u a reset, take it
    {
      RESETFLAG=1;
    }
    man.beginReceiveArray(BUFFER_SIZE, buffer); //erase last array and start scanning for a new one
  }

  
getSerial(); 
parseSerial();

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

void getSerial()
{
  serialsize = 0;        //temp var for sniffing the serial events
  gotserial = 0;         //temp flag for parsing serial later
  while(Serial.available() > 0)
  {
    serialsize = Serial.readBytesUntil('<', serialinput, 40);  //MUST HAVE LINE TERMINATOR jesus christ how i've Learnt
    if(serialsize > 0)gotserial=1;                              //set flag if we caught one pappi
  }
}

void parseSerial()
{
  if(gotserial)
  {
    for(int i = 0; i < serialsize; i++)
    {
      data[i+1] = serialinput[i];
    }
    datalength = serialsize + 1;  //make sure we get the size of the array into a fresh variable or it'll reset AND ADD ONE
    data[0] = datalength;
    man.stopReceive();
    man.transmitArray(datalength, data);
    man.beginReceiveArray(BUFFER_SIZE, buffer);
    gotserial = 0;                    //reset flag
  }
  
}

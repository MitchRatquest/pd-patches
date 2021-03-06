
#include <Manchester.h>
/*  
from hardware/arduino/avr/variants/standard/pins_arduino.h
static const uint8_t A0 = 14;
static const uint8_t A1 = 15;
static const uint8_t A2 = 16;
static const uint8_t A3 = 17;
static const uint8_t A4 = 18;
static const uint8_t A5 = 19;
static const uint8_t A6 = 20;
static const uint8_t A7 = 21;
 */
#define TX_PIN  7 //pin where your transmitter is connected


uint8_t data[40] = {11, 'T','U', 'R', 'N', '_', 'O', 'F', 'F', '9','1','2','3','4','5','6','7','8','9'};
uint8_t datalength=0;

byte RESETFLAG = 1; //HAS THIS BEEN RESET BY CONTRTOLLER? 1 = GOOD TO GO

byte relay0[] = {7, 'r','e','l','a','y','0'};
byte relay1[] = {7, 'r','e','l','a','y','1'};
byte relay2[] = {7, 'r','e','l','a','y','2'};
byte relay3[] = {7, 'r','e','l','a','y','3'};
byte relay4[] = {7, 'r','e','l','a','y','4'};

byte relayReset[5] = {1,1,1,1,1};

const int analogInPin = A3;  // Analog input pin that the potentiometer is attached to
const byte magnetics[] = {14, 15, 16, 17};

int analogReads[] = {0, 0, 0, 0};
int analogCode[] = {735, 650, 450, 370};

const byte RESET_PIN=7;

void setup() {
  man.setupTransmit(TX_PIN, MAN_4800);
  pinMode(A3, INPUT);
  Serial.begin(115200); 
  Serial.setTimeout(50);  
  pinMode(RESET_PIN, INPUT_PULLUP); //pull to ground then
}

void loop() {

  if(digitalRead(RESET_PIN))
  {
    for(byte i=0;i>5;i++)
    {
      relayReset[i] = 1;
    }
  }
                 
}
void checkMagnets()
{
  for(int i =0; i < 5; i++)
  {
    analogReads[i] = analogRead[i];
  }
  if(analogReads[0] >= 720 && analogReads[0] <= 770)
  {
    sendRF(relay0, 0,0);
    relayReset[0] = 0;
  }
  if(analogReads[1] >=580 && analogReads[1] <= 680)
  {
    sendRF(relay1, 1, 1);
    relayReset[1] = 0; 
  }
}

void sendRF(int arrayTX[],  int number, int analogPin)
{
    if(!relayReset[analogPin])return;  //if it hasnt been reset then don't send another relay
    for(int i = 0; i < number-1; i++)
    {
      arrayTX[i] = data[i];
    }
    data[number]=number;
    datalength = number;
    man.transmitArray(datalength, data);
    //relayReset[analogPin] =0;
}

#include <Manchester.h>
#define RX_PIN  7 //pin where your transmitter is connected
#define BUFFER_SIZE 22
byte buffer[BUFFER_SIZE];
byte relay1[] = {7, 'r','e','l','a','y','0'};  //should only need this CANNOT be const or u get error
byte relay2[] = {7, 'r','e','l','a','y','1'};
byte relay3[] = {7, 'r','e','l','a','y','2'};
byte relay4[] = {7, 'r','e','l','a','y','3'};
byte relay5[] = {7, 'r','e','l','a','y','4'};


const byte pinMapping[] = {8,9,10,11,12};  //relay pins
const byte bookPull[] = {2,3,4,5};         //buttons for books, internal pullups, active low

const byte buttonArraySize = 20;
byte buttonArrayAddress = 0;
byte buttonArray[20] = {0}; //unsure of how many pulls, set to unrealistic input

const int codeSize=8;                           //static size of code array
const byte code[codeSize] = {1,2,3,4,1,2,3,4};  //code to unlock bookshelf=



void setup() 
{
  man.setupReceive(RX_PIN, MAN_4800);
  man.beginReceiveArray(BUFFER_SIZE, buffer);
  
  Serial.begin(115200);
  for(int i=0;i<5;i++)
  {
    pinMode(pinMapping[i], OUTPUT);
    if(i!=4)pinMode(bookPull[i], INPUT_PULLUP);
  }
}

void loop() 
{
  /*
  for(int i = 0; i < 5; i++)
  {
    relayToggle(i);
  }
  */
  buttonInput();
  /*
  Serial.print("buttonArray ");
  for(int i = 0; i < buttonArraySize;i++)
  {
    Serial.print(buttonArray[i]);
    Serial.print(" ");
  }
  Serial.println();
  Serial.print("bookpull    ");
  */
  Serial.print(checkBookPull());
  Serial.println();

  if (man.receiveComplete()) 
  {
    uint8_t receivedSize = 0;
    receivedSize = buffer[0];
    if(checkRadio(buffer, relay1, 6))  //if it starts with 'relay' 
    {
      relayToggle(buffer[6]);           //take the letter directly after relay and turn that one on
    }
    man.beginReceiveArray(BUFFER_SIZE, buffer); //erase last array and start scanning for a new one
  }
  
}


byte buttonflag[4]={0,0,0,0};
void buttonInput()
{

  for(int i=0; i <4; i++)
  {
    if(digitalRead(bookPull[i]))  //if button is not pressed, update the button flag 
    {
      buttonflag[i] = 0;
    }
    if(!digitalRead(bookPull[i]) && (!buttonflag[i]))  //buttons are active low, only sample when buttonflag is reset
    {
      //lastread = i;
      buttonArray[buttonArrayAddress] = i+1;     //if pulled, put button NUMBER in slot
      buttonArrayAddress = (buttonArrayAddress + 1) % buttonArraySize;  //incremement address slot, loop around to 0 @ 20
      buttonflag[i] = 1;   //flag high means you have to wait until button is not-pressed to add another slot
    }
  }
}

byte checkBookPull()
{
  byte check =0; 
  /*
   * ok so if u just read from a[0] you're not gonna be set so use 
   * buttonArrayAddress as your last input, and check all
   * previous inputs up to buttonArrayAddress
   */
  for(int i = 0; i < codeSize; i++)  //codeSize is constant
  {
    if(buttonArray[i+((buttonArrayAddress % buttonArraySize ) - codeSize)] == code[i])  //if they match check = 1
    {
      check=1;
    }
    else
    {
      check=0;                     //any mismatch will zero 
    }
    
    //Serial.print(buttonArray[i+((buttonArrayAddress % buttonArraySize ) - codeSize)]);
    //Serial.print(" ");
  }
  //Serial.println();
  return check;
}

void relayToggle(byte relayNumber)
{
  if(relayNumber >5)return;
  digitalWrite(pinMapping[relayNumber], HIGH);
  delay(50);
  digitalWrite(pinMapping[relayNumber], LOW);
}


int checkRadio(byte input[], byte compare[], int n) 
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

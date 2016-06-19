
/*

1    PD1     ---------------  VIN
0    PD0     -|           |-  GND
RST  PC6     -|           |-  RST   PC6
GND          -|           |-  5V
2    PD2     -|  ARDUINO  |-  A7 ONLY
3    PD3     -|    NANO   |-  A6 ONLY
4    PD4     -|           |-  19/A5 PC5
5    PD5     -|           |-  18/A4 PC4
6    PD6     -|           |-  17/A3 PC3
7    PD7     -|           |-  16/A2 PC2
8    PB0     -|           |-  15/A1 PC1
9    PB1     -|           |-  14/A0 PC0
10   PB2     -|           |-  AREF
11   PB4     -|   |USB|   |-  3.3V
12   PB3     ---------------  13    PB5 (ONBOARD LED)

*/

/*
 * | OR  - SET TO 1          PORTA |=  (1<<x)
 * & AND - CHECK IF 1        PORTA &   (1<<x)
 * ~ NOT - CLEAR             PORTA &= ~(1<<x)
 * ^ XOR - FLIP              PORTA ^=  (1<<x)
 * << OR >> BITSHIFT START @ 0
 * PORT = ZERO OR ONE OR PULLUP
 * DDR  = INPUT OR OUTPUT
 * PIN  = CURRENT VALUE
 */
byte EN3 = 6;
byte EN2 = 5;
byte EN1 = 4;
byte STEP = 3;
byte DIR = 2;

byte serialinput[26] = {0};
byte serialsize = 0;
byte gotserial = 0;

int positionarray[400][2]= {};

int MOTOR[400] = {0};
float motorconstantmultiplier=0;
int newMax=0;
int newMin=0;
int zeroCrossing=0;    //impossible zero cross
int halfpoint =0; 


byte offset = 0;
byte rotary1[26] = {3,24,10,14,17,9,20,1,16,23,18,4,25,0,13,15,21,19,7,12,11,2,5,22,8,6};  //statics for rotaries at position 0
byte rotary2[26] = {17,11,3,16,21,5,19,25,15,1,23,10,0,6,13,18,20,2,4,14,22,12,9,8,24,7};
byte rotary3[26] = {9,14,16,4,18,3,6,11,13,19,17,22,23,8,0,24,20,5,12,1,10,2,21,25,15,7};


void setup() 
{
  pinMode(19, INPUT);
  pinMode(18, INPUT);
  digitalWrite(19, LOW);
  digitalWrite(18, LOW);
  
  pinMode(EN3, OUTPUT);//motor enable1
  pinMode(EN2, OUTPUT);//motor enable1
  pinMode(EN1, OUTPUT);//motor enable1
  pinMode(STEP, OUTPUT);//step
  pinMode(DIR, OUTPUT);//DIR
  digitalWrite(EN1, HIGH);
  digitalWrite(EN2, HIGH);
  digitalWrite(EN3, HIGH);
  
  Serial.begin(115200);
  Serial.setTimeout(20);
  pinMode(A7, INPUT);
  pinMode(A6, INPUT);

  //analogReference(EXTERNAL);
}
// SERIAL STUFF BELOW



void getserial()
{
  serialsize = 0;
  gotserial = 0;
  while(Serial.available() > 0)
  {
    serialsize = Serial.readBytesUntil('<', serialinput, 100);
    if(serialsize > 0)gotserial=1;
    
  }
}
void parseserial()
{
  
  int babu = 0;
  int mamu = 0;
  if(gotserial)
  {
    for(int x = 0; x <= serialsize; x++)
    {
      serialinput[x] = serialinput[x]-48;  //ascii offset for numbers
    }
    babu =serialinput[0]*10 + serialinput[1];
    for(int i = 0; i < babu; i++)
    {
    //Serial.print(i);
    //Serial.print(',');
    if(i%5==0)
    {
      motor(14,0, EN1);
    }
    else
    {
      motor(15, 0,EN1);
    }
    sendSerialCSV();   
    }
    digitalWrite(EN1, HIGH);
    gotserial =0;
  }
}


//BADA BING
void loop() 
{

  //for(byte t = 4; t <=6; t++)
  //{
  
    for(byte i = 0; i < 400; i++)
    {
      /*
      Serial.print(i);
      Serial.print(',');
      if(i%3==0 || i == 0 || i == 25)  //every 3rd, and need 2 more to equal 400 steps per rev
      {
        motor(16, 0, EN1);
      }
  
      else
      {
        motor(15, 0, EN1);
      }
      */
      motor(1, 0, EN1);
      sendSerialCSV();
    }
   //byte Decoded = temparray1[temparray2[temparray3[3]]];
    
  //}
  //delay(500);
  
  //*/
  /*
  motor(400, 1, EN1);
  motor(400, 1, EN2);
  motor(400, 1, EN3);
  motor(400, 0, EN1);
  motor(400, 0, EN2);
  motor(400, 0, EN3);
 */
 
  
  /*
  long  temp1;
  long  temp2;
  long result;
  for(int i = 0; i < 27; i++)
  {
    Serial.print(i);
    Serial.print(',');
    if(i%5==0)
    {
      motor(14,0);
    }
    else
    {
      motor(15, 0);
    }
    sendSerialCSV();   
  }
  while(1)
  {
    //motor(400,0);
    //sendSerial();
    //getserial();
    //parseserial();
    calibrate();
    //testing();
    delay(1000);
  }
  */
}

void motor(int steps, byte direct, byte enable)
{
  
  digitalWrite(DIR, direct);
  digitalWrite(enable, LOW);
  //delay(1);
  for(int i = 0; i < steps; i++)
  {
    digitalWrite(STEP, HIGH);
    delayMicroseconds(850);
    digitalWrite(STEP, LOW);
    delayMicroseconds(850);
  }
  digitalWrite(enable, HIGH);

}

void sendSerial()
{
 Serial.print(analogRead(A5));
 Serial.print(" ");
 Serial.print(analogRead(A4));
 Serial.println();
}
void sendSerialCSV()
{
 Serial.print(analogRead(A5));
 //Serial.print(",");
 //Serial.print(analogRead(A4));
 Serial.println();
}



void scanPositions()
{
  
}

void findPosition(int x, int y)
{
  byte found = 0;
  int curx = 0;
  int cury = 0;
  while(!found)
  {
    curx = analogRead(A5);
    cury = analogRead(A6);
    if(x == curx && y ==cury)found=1;
    motor(1,0, EN1);
  }
}

int tempPosition[10] = {0};
void calibrate()
{
    int max1=0;//impossibly small max
    int min1=1024;//impossible large min
    digitalWrite(EN1, LOW); //enable motor
    for(int i = 0; i < 400; i++)//one full rotation
    {
        motor(1, 0, EN1);
        delay(1);
        MOTOR[i] = analogRead(A5);
    }
    ////////////Consolidate lower loop into upper loop here and remove the 400 point array (400x2bytes = too many bytes)

    
    for(int i = 0; i < 400; i++)//maxmin get
    {
        if(MOTOR[i] > max1)
        {
          max1 = MOTOR[i];
        }
        if(MOTOR[i] < min1)
        {
          min1 = MOTOR[i];
        }
    }
    halfpoint = (max1 - min1)/2;
    zeroCrossing = ((max1 - min1)/2)+min1;
    int Max2 = max1;
    //newMax = max1 - zeroCrossing;
    //newMin = min1 - zeroCrossing;

    newMax = max1;
    newMin = min1;
    
    Serial.print("newMax: ");
    Serial.print(newMax);
    Serial.print(' ');
    Serial.print("newMin: ");
    Serial.print(newMin);
    Serial.print(' '); 
    Serial.print("halfpoint: ");
    Serial.print(halfpoint);
    Serial.print(' ');       
        Serial.print("zerocrossing: ");
    Serial.print(zeroCrossing);
    Serial.print(' ');
    Serial.println();
    
    for(int i =0; i < 400; i++)
    {
      /*
      Serial.print(i);
      Serial.print(",");
      Serial.print(MOTOR[i]);
      Serial.println();
      */
      motor(1,0, EN1);
      //int currentposition = analogRead(A5);
      int average[4];
      int currentposition =0;
      for(int i = 0; i < 4; i++)
      {
        
        average[i] = analogRead(A5);
      }
      currentposition = (average[0] + average[1] + average[2] + average[3])/4;

      ///////////EXPERIMENTAL
      if(tempPosition[0] != currentposition)
      {
        for(int i = 0; i < 10; i++)
        {
          tempPosition[i+1] = tempPosition[i];
        }
        tempPosition[0] = currentposition;
      }
      ///////EXPERIMENTAl
      
      float workingtemp = ((float) currentposition - (float) zeroCrossing)/ ((float) newMax - zeroCrossing); //now float between -1 and 1
      
      Serial.print(i);
      Serial.print(",");
      Serial.print(currentposition);
      Serial.print(",");
      Serial.print(workingtemp);
      Serial.print(",");
      Serial.print(newMax);
      float arcsinex = asin(workingtemp); //now a number  between -1.5 and 1.5 (asin = sin^-1(x)
      arcsinex = ((arcsinex + 1.6) * 50);  //134 is 400/3, 400 is motorpositions, 3 is max returned number of asin(x)
      //arcsinex now between 0 and 402
      int tempvar = (int) arcsinex;
      //if(tempPosition[0] < tempPosition[1] && tempPosition[1] < tempPosition[2] || tempPosition[0] < tempPosition[1]-1 && tempPosition[1] < tempPosition[2]   ) //if current position is less than previous, going down on curve
      
      
      if(tempPosition[0] < ((tempPosition[1] + tempPosition[2])/2) )
      { 
        tempvar = tempvar * -1 ;
      }
      
      Serial.print(',');
      Serial.print(tempvar);
      Serial.println();

      

      
    }
    //for(int i = 0; i > 10
}
int getAngle() //from 0 to 1023
{
    int currentposition = analogRead(A5);
    float workingtemp = (currentposition - zeroCrossing)/newMax; //now float between -1 and 1
    float arcsinex = asin(workingtemp); //now a number  between -1.5 and 1.5
    arcsinex = ((arcsinex + 1.5) * 134);  //134 is 400/3, 400 is motorpositions, 3 is max returned number of asin(x)
    //arcsinex now between 0 and 402
    return (int) arcsinex;    
}
void testing()
{
    for(int i = 0; i < 400; i++)
    {
        motor(1, 0, EN1);
        int temp = getAngle();
        Serial.print(i);
        Serial.print(" ");
        Serial.println(temp);
    }
}


void decipher(byte index)
{
  readRotaries(); //read position, compare to last readings, determine absolute position as index from 0 to 25
  byte temparray1[26];
  byte temparray2[26];
  byte temparray3[26];
  for(byte i = 0; i < 26; i++)
  {
    if(i + offset > 25)                        //if you're outside of the array
    {
      temparray1[i] = rotary1[i+offset-26];    //from readRotaries, a number from 0-25
    }
    else
    {
      temparray1[i] = rotary1[i+offset];       //circular buffer implementation
    }
  }
  for(byte i = 0; i < 26; i++)
  {
    if(i + offset > 25)
    {
      temparray2[i] = rotary2[i+offset-26];
    }
    else
    {
      temparray2[i] = rotary2[i+offset];
    }
  }
  for(byte i = 0; i < 26; i++)
  {
    if(i + offset > 25)
    {
      temparray3[i] = rotary3[i+offset-26];
    }
    else
    {
      temparray3[i] = rotary3[i+offset];
    }
  }

  byte Decode = temparray3[temparray2[temparray1[index]]];  //holy fuck that actually compiles
  sendLED(Decode);
  delay(500);
  motor(15, 0; EN1);
}
  






  




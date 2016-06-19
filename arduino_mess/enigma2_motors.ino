
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

byte buttons[26] = {0};

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin 13 as an output.
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(14, OUTPUT);  //has to sink current
  digitalWrite(14, LOW);

  

  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
 Serial.begin(9600);
}


void loop()  //##################MAIN LOOP MOTHERFUCKERS#######################
{

  readKeyboard2();
  fuckmeup(parseKeyboard());
  delay(100);
  keyboardSerial();
int steps = 256;
int delaytime = 150;
for(int i = 0; i < steps; i++)
{
  digitalWrite(13, HIGH);
  delayMicroseconds(delaytime);
  digitalWrite(13, LOW);
  delayMicroseconds(delaytime);
}
  delay(1000);

}

void readKeyboard()
{
  pinMode(14, INPUT);
  for(byte t = 0; t < 3; t++)
  {
    digitalWrite(2, t & 1);
    digitalWrite(3, t & 2);
    digitalWrite(4, t & 4);  //does this even get used? NO
    digitalWrite(5, 0);
    for(byte n = 0; n < 9; n++)
    {
        digitalWrite(6, n & 1);
        digitalWrite(7, n & 2);
        digitalWrite(8, n & 4);
        digitalWrite(9, 0);
        delay(10);                                                                   
      buttons[(t*8)+n] = digitalRead(14);
      //buttons[(t*8)+n] = PORTC & 1;
    }
  }
  ///read 2 outlier buttons
    digitalWrite(2, 0);
    digitalWrite(3, 1);
    digitalWrite(4, 1);
    digitalWrite(5, 0);
    digitalWrite(6, 0);
    digitalWrite(7, 0);
    digitalWrite(8, 0);
    digitalWrite(9, 1);
    buttons[24] = digitalRead(14);

    digitalWrite(2, 0);
    digitalWrite(3, 1);
    digitalWrite(4, 1);
    digitalWrite(5, 0);
    digitalWrite(6, 1);
    digitalWrite(7, 0);
    digitalWrite(8, 0);
    digitalWrite(9, 1);
    buttons[25] = digitalRead(14); //wired the outlier display LEDS wrong here buddy
    digitalWrite(8, 1);
}

void readKeyboard2()
{
  PORTD &= ~252; //all bits 2-8 high, clear em
  PORTB &= ~3;   //LSB 0 and 1 clear
  pinMode(14, INPUT);
  for(byte t = 0; t < 3; t++)
  {
    PORTD &= ~252;
    PORTD |= t<<2;  //push out mux lines for power

    for(byte n = 0; n < 9; n++)
    {

      PORTD &= ~192;     //clear top two bits 
      PORTD |= n<<6;     //set top bits
      PORTB &= ~3;      //clear 2 LSB PORTB
      PORTB |= (n>>2);   //set bottom 2 bits
      buttons[(t*8)+n] = digitalRead(14);
    }
  }
    ///read 2 outlier buttons
    digitalWrite(2, 0);
    digitalWrite(3, 1);
    digitalWrite(4, 1);
    digitalWrite(5, 0);
    digitalWrite(6, 0);
    digitalWrite(7, 0);
    digitalWrite(8, 0);
    digitalWrite(9, 1);
    buttons[24] = digitalRead(14);

    digitalWrite(2, 0);
    digitalWrite(3, 1);
    digitalWrite(4, 1);
    digitalWrite(5, 0);
    digitalWrite(6, 1);
    digitalWrite(7, 0);
    digitalWrite(8, 0);
    digitalWrite(9, 1);
    buttons[25] = digitalRead(14); //wired the outlier display LEDS wrong here buddy
    digitalWrite(8, 1);
}

void keyboardSerial()    //print the keyboard out over the serial port
{
  for(byte x = 0; x < 26; x++)
  {
    Serial.print(buttons[x]);
    Serial.print(' ');
  }
  Serial.println();  
}

byte parseKeyboard()
{
  byte tempvar=26;  //break condition in LED writing function (>=26)
  for(byte x = 0; x < 26; x++)
  {
    if(buttons[x] == 1)
    {
      tempvar = x;
      return tempvar;
    }
  }

}

void strobeLED()   //LSB nibble is power, MSB nibble is individual LED current return
{
  pinMode(14, INPUT);
  for (byte i = 3; i <= 7; i++)
  {
    if(i==6)i=7;
    digitalWrite(2, i & 1);
    digitalWrite(3, i & 2);
    digitalWrite(4, i & 4);
    digitalWrite(5, i & 8);
    
    for (byte j = 0; j < 10; j++)
    {
      
      if (i == 7 && j ==0) 
      {
        digitalWrite(6, 0);
        digitalWrite(7, 0);
        digitalWrite(8, 0);
        digitalWrite(9, 1);
        delay(100);
      }
      
     if (i == 7 && j==1)
      {
        digitalWrite(6, 1);
        digitalWrite(7, 0);
        digitalWrite(8, 0);
        digitalWrite(9, 1);
        delay(100);
        j = 11;           //end of LED chain
      }
      
      if (j >= 8)break; //stop if its not the weird keys
      else
      {
        digitalWrite(6, j & 1);
        digitalWrite(7, j & 2);
        digitalWrite(8, j & 4);
        digitalWrite(9, j & 8);
        delay(100);
      }

    }
  }
}
void fuckmeup(byte towrite)  //correct writeled script
{
  if(towrite >= 26)return;
  pinMode(14, INPUT);
  byte temp = 3;
  temp += towrite >> 3;
  digitalWrite(2, temp & 1);
  digitalWrite(3, temp & 2);
  digitalWrite(4, temp & 4);
  digitalWrite(5, temp & 8);
  digitalWrite(6, towrite & 1);
  digitalWrite(7, towrite & 2);
  digitalWrite(8, towrite & 4);
  digitalWrite(9, 0);  //reset if hit 24 or 25
  //digitalWrite(9, towrite & 8);  //YOU FUCKING IDIOT YOU HAVE A 4 BIT MUX YOU FUCKING ASSHOLE
  if (towrite == 24)
  {
    digitalWrite(2, 1);
    digitalWrite(3, 1);
    digitalWrite(4, 1);
    digitalWrite(5, 0);
    digitalWrite(6, 0);
    digitalWrite(7, 0);
    digitalWrite(8, 0);
    digitalWrite(9, 1);
  }
  if (towrite == 25)
  {
    digitalWrite(2, 1);
    digitalWrite(3, 1);
    digitalWrite(4, 1);
    digitalWrite(5, 0);
    digitalWrite(6, 1);
    digitalWrite(7, 0); 
    digitalWrite(8, 0);
    digitalWrite(9, 1);
  }

}
void writeLED(byte toWrite)  //0 at bottom left, 25 at top right
{
  if (toWrite <= 7)
  {
    digitalWrite(2, 1);
    digitalWrite(3, 1);
    digitalWrite(4, 0);
    digitalWrite(5, 0);
    digitalWrite(6, toWrite & 1);
    digitalWrite(7, toWrite & 2);
    digitalWrite(8, toWrite & 4);
    digitalWrite(9, toWrite & 8);
  }
  else if (toWrite <= 15 && toWrite >= 8)
  {
    digitalWrite(2, 0);///add 1 later
    digitalWrite(3, 0);
    digitalWrite(4, 1);
    digitalWrite(5, 0);
    digitalWrite(6, toWrite & 1);
    digitalWrite(7, toWrite & 2);
    digitalWrite(8, toWrite & 4);
    digitalWrite(9, toWrite & 8);
  }

  else if (toWrite >= 16 && toWrite <= 23)
  {
    digitalWrite(2, 1);///add 1 later
    digitalWrite(3, 0);
    digitalWrite(4, 1);
    digitalWrite(5, 0);
    digitalWrite(6, toWrite & 1);
    digitalWrite(7, toWrite & 2);
    digitalWrite(8, toWrite & 4);
    digitalWrite(9, toWrite & 8);
  }
  else if (toWrite == 24)
  {
    digitalWrite(2, 0);
    digitalWrite(3, 1);
    digitalWrite(4, 1);
    digitalWrite(5, 0);
    digitalWrite(6, 1);
    digitalWrite(7, 0);
    digitalWrite(8, 0);
    digitalWrite(9, 1);
  }
  else if (toWrite == 25)
  {
    digitalWrite(2, 1);
    digitalWrite(3, 1);
    digitalWrite(4, 1);
    digitalWrite(5, 0);
    digitalWrite(6, 0);
    digitalWrite(7, 0);
    digitalWrite(8, 0);
    digitalWrite(9, 1);
  }

}



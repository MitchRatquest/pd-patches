#include <OSCBundle.h>
#include <SLIPEncodedSerial.h>
 SLIPEncodedSerial SLIPSerial(Serial);
/*

1      ---------------  VIN
0      -|           |-  GND
RST    -|           |-  RST
GND    -|           |-  5V
2      -|  ARDUINO  |-  A7 ONLY
3      -|    NANO   |-  A6 ONLY
4      -|           |-  19/A5
5      -|           |-  18/A4
6      -|           |-  17/A3
7      -|           |-  16/A2
8      -|           |-  15/A1
9      -|           |-  14/A0
10     -|           |-  AREF
11     -|   |USB|   |-  3.3V
12     ---------------  13 (ONBOARD LED)

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

const int IO_size=15;
int IO_previous[IO_size] = {0};
int IO_current[IO_size] = {0};

const byte digital_pin_count = 12;
const byte digitalPins[digital_pin_count] = {2,3,4,5,6,7,8,9,10,11,12,13};
const byte analog_pin_count = 3;
const byte analogPins[analog_pin_count] = {19, 20, 21};

void backup_IO()
{
    for(byte i = 0; i < IO_size; i++)  //backup array
    {
        IO_previous[i] = IO_current[i];
    }
}

void readIO_digital()  // fill current_IO
{
    for(byte i = 5; i < digital_pin_count; i++)  //up to max digital pin count
    { 
        digitalWrite(digitalPins[i], HIGH);
        IO_current[i] = digitalRead(digitalPins[i]);
    }
}
void readIO_digital_rotary()
{
    /*
     * combinations, from D2 to D6
     * 11100
     * 11110
     * 01110
     * 00110
     * 00011
     * turn on the 3rd msb and sample the rest
     * with a bitshift to get it into 1 number
     * from bottom up it goes
     * 27
     * 26
     * 18
     * 2
     * 3
     */
    digitalWrite(2, 1);//all other pins high as inputs = pullups
    digitalWrite(3, 1);
    digitalWrite(5, 1);
    digitalWrite(6, 1);
    byte rotary=0;
    pinMode(4, OUTPUT);//sink current through this
    digitalWrite(4, 0);//sinking current
    rotary = (digitalRead(2)<<4) | (digitalRead(3)<<3) | (digitalRead(5)<<1) | (digitalRead(6)<<0); //finally shift them
    digitalWrite(2, 0);
    digitalWrite(3, 0);
    digitalWrite(5, 0);
    digitalWrite(6, 0);
    IO_current[2] = rotary; //feed it to the machine
}
void readIO_analog()
{
    for(byte i = 0; i < analog_pin_count; i++)  //offset for digital pins, up to max IO size
    {


       IO_current[i+digital_pin_count] = analogRead(analogPins[i]);
      
    }
}

void send_OSC(byte pinNumber, int changedData)  //send that data
{
    OSCMessage interface("/interface");
    /*
     * stupidest bug hackaround below
     * will work perfectly on windows
     * on linux will toss the 13 into the 10 as well as ten
     * no documentation on why or how
     * but this nasty hack works
     * god save us all
     */
    if(pinNumber == 13)                 
    {
      interface.add((int32_t)18);
      interface.add((int32_t)changedData);
      SLIPSerial.beginPacket();
      interface.send(SLIPSerial);
      SLIPSerial.endPacket();
    }
    else
    {
    interface.add((int32_t)pinNumber);
    interface.add((int32_t)changedData);
    SLIPSerial.beginPacket();
    interface.send(SLIPSerial);
    SLIPSerial.endPacket();
    }
}

void input_change()  //determine whiche ones actually changed
{
    for(byte i = 0; i < IO_size; i++)
    {
        if(IO_current[i] != IO_previous[i])
        {
            send_OSC(i, IO_current[i]);
        }
    }
}

void setup()
{  
    SLIPSerial.begin(57600);   // set this as high as you can reliably run on your platform
    Serial.begin(57600);
    
    for(byte i = 0; i < digital_pin_count; i++)  //maybe have a function for outputs, too? later when time
    {
        pinMode(digitalPins[i], INPUT_PULLUP);
        digitalWrite(digitalPins[i], LOW);
    }
    for(byte i = 0; i < analog_pin_count; i++)   //maybe have a function for outputs, too? later when time
    {
        pinMode(analogPins[i], INPUT);
    }
}

void loop()
{
    backup_IO();
    readIO_digital();
    readIO_analog();
    readIO_digital_rotary();
    input_change();
}


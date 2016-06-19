byte EN = 4;
byte STEP = 3;
byte DIR = 2;
void setup() 
{
  pinMode(19, INPUT);
  pinMode(18, INPUT);
  digitalWrite(19, LOW);
  digitalWrite(18, LOW);

  pinMode(EN, OUTPUT);//motor enable
  pinMode(STEP, OUTPUT);//step
  pinMode(DIR, OUTPUT);//DIR
  digitalWrite(EN, HIGH);
  
  Serial.begin(115200);
  Serial.setTimeout(20);
}
int tempvar =0;
void loop() 
{
  motor(1,0);
  tempvar = analogRead(A5);
  tempvar = map(tempvar, 370, 650, 0, 1023);
  Serial.println(tempvar);
  //delay(2);

}

void motor(int steps, byte direct)
{
  digitalWrite(DIR, direct);
  digitalWrite(EN, LOW);
  for(int i = 0; i < steps; i++)
  {
    digitalWrite(STEP, HIGH);
    delayMicroseconds(800);
    digitalWrite(STEP, LOW);
    delayMicroseconds(800);
  }
}

const int selA = 5;
const int selB = 6;
const int selC = 7;
   
int batas = 300;
 
void setup() {
  // put your setup code here, to run once:
    Serial.begin(9600);
 
    pinMode(selA,OUTPUT);
    pinMode(selB,OUTPUT);
    pinMode(selC,OUTPUT);
}
 
void loop() {
  
  Serial.print(SensorL(7));
  Serial.print("\t");
 
  Serial.print(SensorL(6));
  Serial.print("\t");
  
  Serial.print(SensorL(5));
  Serial.print("\t");
 
  Serial.print(SensorL(4));
  Serial.print("\t");
 
  Serial.print(SensorL(3));
  Serial.print("\t");
 
  Serial.print(SensorL(2));
  Serial.print("\t");
 
  Serial.print(SensorL(1));
  Serial.print("\t");
 
  Serial.print(SensorL(0));
  Serial.print("\t");
 
  Serial.print(SensorR(7));
  Serial.print("\t\t");
 
  Serial.print(SensorR(6));
  Serial.print("\t");
  
  Serial.print(SensorR(5));
  Serial.print("\t");
 
  Serial.print(SensorR(4));
  Serial.print("\t");
 
  Serial.print(SensorR(3));
  Serial.print("\t");
 
  Serial.print(SensorR(2));
  Serial.print("\t");
 
  Serial.print(SensorR(1));
  Serial.print("\t");
 
  Serial.print(SensorR(0));
  Serial.println("\t");
 
  delay(1);
}
 
void selektor(int value)
{
  if( value == 0)
  {
    digitalWrite(selA,LOW);
    digitalWrite(selB,LOW);
    digitalWrite(selC,LOW);
  }
  else if( value == 1)
  {
    digitalWrite(selA,HIGH);
    digitalWrite(selB,LOW);
    digitalWrite(selC,LOW);
  }
  else if( value == 2)
  {
    digitalWrite(selA,LOW);
    digitalWrite(selB,HIGH);
    digitalWrite(selC,LOW);
  }
  else if( value == 3)
  {
    digitalWrite(selA,HIGH);
    digitalWrite(selB,HIGH);
    digitalWrite(selC,LOW);
  }
  else if( value == 4)
  {
    digitalWrite(selA,LOW);
    digitalWrite(selB,LOW);
    digitalWrite(selC,HIGH);
  }
  else if( value == 5)
  {
    digitalWrite(selA,HIGH);
    digitalWrite(selB,LOW);
    digitalWrite(selC,HIGH);
  }
  else if( value == 6)
  {
    digitalWrite(selA,LOW);
    digitalWrite(selB,HIGH);
    digitalWrite(selC,HIGH);
  }
  else
  {
    digitalWrite(selA,HIGH);
    digitalWrite(selB,HIGH);
    digitalWrite(selC,HIGH);
  }
}
 
int SensorL(int x)
{
  //int nilai;
  selektor(x);
 
  return analogRead(A0);
}
 
int SensorR(int x)
{
  //int nilai;
  selektor(x);
 
  return analogRead(A1);
}


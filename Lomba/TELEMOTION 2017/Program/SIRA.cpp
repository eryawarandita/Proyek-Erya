#include <LiquidCrystal.h>
#include <Keypad.h>
#include <SoftwareSerial.h>
SoftwareSerial BT (15,14);


const byte ROWS = 4; //jumlah baris keypad
const byte COLS = 4; //jumlah kolom keypad

//definisi simbol button keypad
char hexaKeys[ROWS][COLS] = {
  {'1','4','7','*'},
  {'2','5','8','0'},
  {'3','6','9','#'},
  {'A','B','C','D'}
};

byte rowPins[ROWS] = {32, 33, 34, 35}; //pin untuk baris keypad
byte colPins[COLS] = {36, 37, 38, 39}; //pin untuk kolom keypad

//inisialisasi keypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

//inisialisasi lcd
//LiquidCrystal lcd(33,23,25,27,29,31);
LiquidCrystal lcd(48,46,44,45,42,43);
//32-39 keypad
//42-49 lcd
// it's a 16x2 LCD so...
int screenWidth = 16;
int screenHeight = 2;

// the two lines
// line1 = scrolling
String line1 = "SIRA";
// line2 = static
String line2 = "SISTEM PENGUKURAN AIR";

// just some reference flags
int stringStart, stringStop = 0;
int scrollCursor = screenWidth;

//variabel tambahan
int isi=0;
int i=0, j=0,k=0,l,m=0;
char customKey;
char token[15];
String token_str="";
unsigned long token_int;
unsigned long t_current, t_previous=0,interval=0, t_interval, t_airtemp,t_air ;
int s1=0;
byte sensorInterrupt = 0;  // 0 = digital pin 2
byte sensorPin       = 2;

// The hall-effect flow sensor outputs approximately 4.5 pulses per second per
// litre/minute of flow.
float calibrationFactor = 4.5;

volatile byte pulseCount;  

float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;
unsigned long totalLitres;

unsigned long oldTime;
int valve = 53;
int buzzer = 5;
int state_buzz =0;
long int n=0;

// most of the part is pretty basic
void setup() {
  Serial.available();
  lcd.begin(screenWidth,screenHeight);
  Serial.begin(9600);
  BT.begin (9600);
  s1=0;
  pinMode(sensorPin, INPUT);
  digitalWrite(sensorPin, HIGH);
  pinMode(valve,OUTPUT);
  digitalWrite(valve,HIGH);
  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, LOW);

  pulseCount        = 0;
  flowRate          = 0.0;
  flowMilliLitres   = 0;
  totalMilliLitres  = 0;
  oldTime           = 0;

  // The Hall-effect sensor is connected to pin 2 which uses interrupt 0.
  // Configured to trigger on a FALLING state change (transition from HIGH
  // state to LOW state)
  attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
}

void loop() {

	bacaSensor();
  t_current=millis();
  t_interval = interval - (totalMilliLitres/1000);
  t_airtemp =(totalMilliLitres);
  
    customKey = customKeypad.getKey();
    if (customKey)
    {
      lcd.clear();
      s1=1;
      m=9;
    }
  
    if (s1==0)
    {
      lcd.clear();
      lcd.setCursor(scrollCursor, 1);
      lcd.print(line2.substring(stringStart,stringStop));
      lcd.setCursor(6, 0);
      lcd.print(line1);
      delay(500);
      //lcd.clear();
      if(stringStart == 0 && scrollCursor > 0)
      {
        scrollCursor--;
        stringStop++;
      } 
      else if (stringStart == stringStop)
      {
        stringStart = stringStop = 0;
        scrollCursor = screenWidth;
      } 
      else if (stringStop == line2.length() && scrollCursor == 0) 
      {
        stringStart++;
      } 
      else 
      {
        stringStart++;
        stringStop++;
      }
    } 

    if (isi == 0)
    {
        if (j==0)
        {
          Serial.println("Token Habis");
          Serial.println("Silakan masukan token");
          lcd.setCursor(0, 0);
          lcd.print("Token Habis");
          lcd.setCursor(0, 1);
          lcd.print("Masukan token");
          j+=1;            
        }
        if (customKey)
        {      
            if (customKey =='#')
            {
	              t_current=millis();
                token_str=token;
                token_int=token_str.toInt(); 
                Serial.println("Token berhasil diisi");
                Serial.println(token_int);
                lcd.setCursor(0, 0);
                lcd.print("Transaksi sukses");
                delay(1000);
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Token : ");
                lcd.setCursor(0, 1);          
                lcd.print(token_int);
                delay(1000);
                lcd.clear();   
                isi=1;
                customKey='B';
                j=0; 
                i=0;
                k=0;        
	              interval+=(token_int/2000);
                t_interval = interval;
	              t_previous =t_current;
	              for(l=0;l<=15;l++)
	              {
                    token[l]='\0';
                }
                token_str=""; 
                token_str=token;
                s1=0;
                digitalWrite(valve,LOW);
            }
            
            else if ((customKey !='*')&&(customKey !='A')&&(customKey !='B')&&(customKey !='C')&&(customKey !='D'))
            {
                if (k==0)
                {
                  Serial.print("Token : ");
                  k++;
                }
                Serial.print(customKey); 
                token[i]=customKey;
                i++;
                lcd.setCursor(0, 0);
                lcd.print("Token :");
                lcd.setCursor(0, 1);
                lcd.print(token);
            }
            else
            {
                for(l=0;l<=15;l++)
                {
                    token[l]='\0';
                } 
                token_str=token;
                //state=0;
                j=0;
                i=0;
             }
      }   
  } 
  else
  { 
      if (t_interval <= 2)
      {
        n++;
        if (n<=100000)
        {
            if (state_buzz==0)
            {
                digitalWrite(buzzer,HIGH);
                state_buzz=1;
            }
            else
            {
                digitalWrite(buzzer,LOW);
                state_buzz=0;
            }
        }
        else 
        {
          n=0;
        }
      }
	    if ((totalMilliLitres/1000) >= interval)
	    {
        digitalWrite(valve, HIGH);
        totalMilliLitres=0;
        isi=0;
        j=0;
        k=0;
        for(l=0;l<=15;l++)
        {
          token[l]='\0';
        } 
        token_str=token; 
        lcd.clear(); 
        s1=1;
        t_air=t_airtemp;
        t_airtemp=0; 
        n=0;
        state_buzz=0;
        digitalWrite(buzzer,LOW);              
      }
	    else 
	    {
		    lcd.clear();
        //delay(300);
			  if (customKey=='*')
			  {
			      m=9;
            s1=0;        
			  } 
			  else if (customKey=='B')
			  {
          m=0;
			  }
        else if (customKey=='A')
        {
          m=1;
        }
        if (m==0)
        {
			    //lcd.clear();
			    i=0;			  
			    lcd.setCursor(0, 0);
			    lcd.print("Sisa Air :");
			    lcd.setCursor(0, 1);
			    lcd.print(t_interval);
          //BT.print(t_interval);
          //BT.println("L");
          lcd.setCursor(15, 1);
          lcd.print("L");
          delay(300);
          //s1=0;          
			   }
         else if (m==1)
         {
            i=0;
            lcd.setCursor(0, 0);
            lcd.print("Total Penggunaan");
            lcd.setCursor(0, 1);
            lcd.print(t_air+t_airtemp);
            //BT.print("\t\t");
            //BT.print(t_air+t_airtemp);
            //BT.println("mL");
            lcd.setCursor(14, 1);
            lcd.print("mL");
            delay(300);
         }      
	    }
	  }
   BT.print(t_interval);
   BT.print("L");
   delay(300);
   BT.print("\t\t");
   BT.print(t_air+t_airtemp);
   BT.println("mL");
}



/*
Insterrupt Service Routine
 */
void pulseCounter()
{
  // Increment the pulse counter
  pulseCount++;
}

void bacaSensor(){
  if((millis() - oldTime) > 1000)    // Only process counters once per second
  { 
    // Disable the interrupt while calculating flow rate and sending the value to
    // the host
    detachInterrupt(sensorInterrupt);
        
    // Because this loop may not complete in exactly 1 second intervals we calculate
    // the number of milliseconds that have passed since the last execution and use
    // that to scale the output. We also apply the calibrationFactor to scale the output
    // based on the number of pulses per second per units of measure (litres/minute in
    // this case) coming from the sensor.
    flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;
    
    // Note the time this processing pass was executed. Note that because we've
    // disabled interrupts the millis() function won't actually be incrementing right
    // at this point, but it will still return the value it was set to just before
    // interrupts went away.
    oldTime = millis();
    
    // Divide the flow rate in litres/minute by 60 to determine how many litres have
    // passed through the sensor in this 1 second interval, then multiply by 1000 to
    // convert to millilitres.
    flowMilliLitres = (flowRate / 60) * 1000;
    
    // Add the millilitres passed in this second to the cumulative total
    totalMilliLitres += flowMilliLitres;
      
    unsigned int frac;
    
    // Print the flow rate for this second in litres / minute
    Serial.print("Flow rate: ");
    Serial.print(int(flowRate));  // Print the integer part of the variable
    Serial.print("L/min");
    Serial.print("\t");       // Print tab space

    // Print the cumulative total of litres flowed since starting
    Serial.print("Output Liquid Quantity: ");        
    Serial.print(totalMilliLitres);
    Serial.println("mL"); 
    Serial.print("\t");       // Print tab space
    Serial.print(totalMilliLitres/1000);
    Serial.print("L");
    

    // Reset the pulse counter so we can start incrementing again
    pulseCount = 0;
    
    // Enable the interrupt again now that we've finished sending output
    attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
  }
}

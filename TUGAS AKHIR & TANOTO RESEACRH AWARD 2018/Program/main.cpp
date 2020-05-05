
/****************************************************************************/ 
/* TA1617.02.008                                                            */
/* Digital Spirometer using  MPXV 70002DP sensor                            */
/*                                                                          */
/* (c) Erya Warandita Bupuly Tomara /13213063                               */
/*                                                                          */
/*                                                                          */
/*  In cooperation with : QIM-TRONICS                                       */
/****************************************************************************/

//Librari
#include <MovingAverage.h>
#include <SoftwareSerial.h>
MovingAverage average();
SoftwareSerial BT(PA10,PA9); //TX RX HC-05


//Variabel
int i,t;
int sensor = PB0;
int nilaiSensor =0;
float averageInitialValue =0;
float voltage =0 ;
float tekanan_kpa = 0;
float tekanan;
float volumetricFlow = 0;        //debit udara (Q)
float velocityFlow =0;           //kecepatan flow (v)
float debit =0;
float tekanan_pa;

//konstanta
const float rho = 1.225; // konstanta rho udara
const float diameter1 = 0.0155;  // diameter1 dalam meter
const float diameter2 = 0.0105;  // diameter2 dalam meter
const float luas1 = 0.000754385; // luas1 dalam meter^2
const float luas2 = 0.000346185; // luas2 dalam meter^2
const float k = (float) 2/11;

float jumlahData = 0;
float EMAprev, EMAcurr, SMA;

float hitungDebit(float input) {
  voltage =  ((input)*3.3/4096);
  //voltage = input*5/1024;
  //tekanan_kpa = (((voltage/5.0)-0.5)/0.2);
  
  //rumus konversi ke maple menggunakan persamaan garis
  //tekanan_kpa = (1.227*voltage) - 2.049;
  //tekanan_kpa = (0.95*voltage) - 1.142;
  tekanan_kpa = (1.021*voltage) - 1.37;//PUTIH
  tekanan_pa = (tekanan_kpa*1000);
  
  //hitung debit ekshalasi
  volumetricFlow = luas1*(sqrt((2/rho) * (tekanan_pa/(((luas1/luas2)*(luas1/luas2))-1))));  
  
  return(volumetricFlow*1000);
  
}

void setup() {
  // Declare the sensorPin as INPUT_ANALOG:
  pinMode(sensor, INPUT_ANALOG);
  Serial.begin(9600); 
  BT.begin(38400); 
  debit = hitungDebit((float)analogRead(sensor));
  average.reset(debit);
  
}

void loop() 
{
    debit = hitungDebit((float)analogRead(sensor));
    Serial.println( average.update( debit ) );
    Serial.print(" ");
    Serial.println(debit);
    BT.println(debit);
    delay(10);  
}


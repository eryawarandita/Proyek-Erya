/*******************************************************
This program was created by the
CodeWizardAVR V3.12 Advanced
Automatic Program Generator
© Copyright 1998-2014 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com

Project : Tugas Besar EL3014 
Version : 6
Date    : 5/14/2016
Author  : Erya Warandita B.T
          13213063
Company : Institut Teknologi Bandung
Comments: Program Utama Robot Line Follower DIgital


Chip type               : ATmega32
Program type            : Application
AVR Core Clock frequency: 16.000000 MHz
Memory model            : Small
External RAM size       : 0
Data Stack size         : 512
*******************************************************/

#include <mega32.h>
#include <stdio.h>

#include <delay.h>
// Alphanumeric LCD functions
#include <alcd.h>

#include "adc.c"

//define
// Motor
#define MOTOR_ENL PORTB.0
#define MOTOR_ENR PORTB.1
#define PWM_KIRI  OCR0
#define PWM_KANAN OCR2
#define DIR_KIRI  PORTD.4
#define DIR_KANAN PORTD.5
#define MAX_PWM   255

// State Utama
#define GO_INIT 1
#define GO_MAIN 2
// LCD
#define LCD_PATH_ROOT 1
#define LCD_TAMPIL_SENSOR 2
#define LCD_KALIBRASI 3
#define LCD_ADC_SENSOR_1 4
#define LCD_ADC_SENSOR_2 5
#define LCD_PID 6
#define LCD_SET_P 7
#define LCD_SET_I 8
#define LCD_SET_D 9
#define LCD_SET_SPEED 10
#define LCD_SET_ROTSPEED 11
#define LCD_SET_PATH 12

// Switch
#define S1 PINB.4
#define S2 PINB.5
#define S3 PINB.6
#define S4 PINB.7

#define LCD PORTC.3

// Mode navigasi
#define NAV_JALAN_PID 1
#define NAV_KALIBRASI 2
#define NAV_REM 3
#define NAV_ROTASI_KIRI 4
#define NAV_ROTASI_KANAN 5
#define NAV_BERHENTI 0
// Declare your global variables here
unsigned int sensor;
eeprom unsigned char sensor_calibration[12];
unsigned char adc_sensor[12];
void baca_adc_sensor(){
// urutan pembacaan
// 4  2  6  1  7  0  5  3
    PORTA&=0xF8;
    PORTA|=4;
    adc_sensor[6]=read_adc(5);
    PORTA&=0xF8;
    PORTA|=2;
    adc_sensor[7]=read_adc(5);
    PORTA&=0xF8;
    PORTA|=6;
    adc_sensor[0]=read_adc(4);
    adc_sensor[8]=read_adc(5);
    PORTA&=0xF8;
    PORTA|=1;
    adc_sensor[1]=read_adc(4);
    adc_sensor[9]=read_adc(5);
    PORTA&=0xF8;
    PORTA|=7;
    adc_sensor[2]=read_adc(4);
    adc_sensor[10]=read_adc(5);
    PORTA&=0xF8;
    PORTA|=0;
    adc_sensor[3]=read_adc(4);
    adc_sensor[11]=read_adc(5);
    PORTA&=0xF8;
    PORTA|=5;
    adc_sensor[4]=read_adc(4);
    PORTA&=0xF8;
    PORTA|=3;
    adc_sensor[5]=read_adc(4);            
}
void kalibrasi_sensor(){
    static unsigned char max[12]={0,0,0,0,0,0,0,0,0,0,0,0};
    static unsigned char min[12]={255,255,255,255,255,255,255,255,255,255,255,255};
    unsigned char tmp_cal[12];
    unsigned char i;
    for(i=0;i<12;i++){
        if(adc_sensor[i]>max[i]){
            max[i]=adc_sensor[i];
        }
        if(adc_sensor[i]<min[i]){
            min[i]=adc_sensor[i];
        }
        tmp_cal[i]=(max[i]+min[i])/2;
    }
    for(i=0;i<12;i++){
        sensor_calibration[i]=tmp_cal[i];
    }        
}

void motor_setpwm(int input_kiri, int input_kanan){
    int kiri;
    int kanan;
    if(input_kiri<-MAX_PWM){
        kiri=-MAX_PWM;
    }else if(input_kiri>MAX_PWM){
        kiri=MAX_PWM;
    }else{
        kiri=input_kiri;
    }
    if(input_kanan<-MAX_PWM){
        kanan=-MAX_PWM;
    }else if(input_kanan>MAX_PWM){
        kanan=MAX_PWM;
    }else{
        kanan=input_kanan;
    } 
    MOTOR_ENL=1;
    MOTOR_ENR=1;
    if(kiri<0){
        PWM_KIRI=255+kiri;
        DIR_KIRI=0;
    }else{
        PWM_KIRI=kiri;
        DIR_KIRI=1;
    }
    if(kanan<0){
        PWM_KANAN=255+kanan;
        DIR_KANAN=0;
    }else{
        PWM_KANAN=kanan;
        DIR_KANAN=1;
    }
}

unsigned char state_lcd;
void set_lcd(unsigned char s1,unsigned char s2,unsigned char s3,unsigned char s4){
    if(S1==0 && s1!=0xFF){   
        while(S1==0);
        state_lcd=s1;
    }else if(S2==0 && s2!=0xFF){
        while(S2==0);
        state_lcd=s2;
    }else if(S3==0 && s3!=0xFF){
        while(S3==0);
        state_lcd=s3;
    }else if(S4==0 && s4!=0xFF){
        while(S4==0);
        state_lcd=s4;
    }
} 
           
// Timer1 output compare A interrupt service routine
unsigned char mode=0;
eeprom char P_gain;
eeprom char I_gain;
eeprom char D_gain;
eeprom unsigned char throttle;
eeprom unsigned char throttle_r;
eeprom unsigned char checkpoint=0;
eeprom unsigned char pathcust[]="________________";

int PID;
int error;
unsigned char lebar_garis;
unsigned char invert=0;
    
interrupt [TIM1_COMPA] void timer1_compa_isr(void){
// Interupsi setiap 10ms
    unsigned char i;
    static unsigned int counter=0;
    static int d_error=0,prev_error=0;
    static long sum_error=0;
    int sensor_position=0;
    int active_sensor=0;
    
    TCNT1L=0;
    TCNT1H=0;
    
    baca_adc_sensor();
    sensor=0;
    for(i=0;i<12;i++){
        if(adc_sensor[i]>sensor_calibration[i])
        { // sensor mendeteksi garis atau bidang gelap
            sensor|=1<<i;   
            sensor_position+=((i*2)-11);
            active_sensor++;
        }
        else
        { 
        // sensor tidak mendeteksi garis atau mendeteksi bidang terang
            sensor&=~(1<<i);
        }
    }      
    if(invert){
        sensor_position=-sensor_position;
        sensor=(~sensor)&0x0FFF;
        active_sensor=12-active_sensor;
    }
    lebar_garis=active_sensor;
    switch(mode){                                       
        case NAV_KALIBRASI: // mode kalibrasi
            motor_setpwm(-200,200);   // robot berputar di atas garis
            kalibrasi_sensor();
            counter++;
            if(counter==300){         // lakukan selama 3 detik (300 x 10ms)
                motor_setpwm(0,0);
                mode=NAV_BERHENTI;
                counter=0;
            }
            break;
        case NAV_JALAN_PID: // jalan normal
            if(active_sensor>0){
                error=sensor_position/active_sensor;
            }else{
                error=prev_error;
            }
            sum_error+=error;             // integrator
            d_error=error-prev_error;     // differensiator
            if(sum_error<-32000){         // pembatas integrator, hindari overflow
                sum_error=-32000;            
            }else if(sum_error>32000){
                sum_error=32000;
            }
            PID=
                P_gain*error +
                (I_gain*sum_error/100) + // periode sampling 10ms = 1/100 detik
                D_gain*d_error;
            motor_setpwm(throttle-PID,throttle+PID);
            prev_error=error;
            break;
        case NAV_ROTASI_KIRI:
            motor_setpwm(-throttle_r,throttle_r);
            break;
        case NAV_ROTASI_KANAN:
            motor_setpwm(throttle_r,-throttle_r);
            break;
        case NAV_REM:
            motor_setpwm(0,0);
            break; 
        default: // NAV_BERHENTI
            MOTOR_ENL=0;
            MOTOR_ENR=0;
            break;
    }   
}      
void main(void){
unsigned char jalan=0;
unsigned char set=0;
char str2[16+1];  // pada akhir string terdapat sebuah karakter kosong tanda akhir string
char str1[16+1];

// Rencana jalur
unsigned char path0[]="^^><<<>^><<>><<><<<>.";
unsigned char path1[]="><<><<<.";
unsigned char path2[]="<^>>^.";
unsigned char path3[]="^^>><!>!^.";
unsigned char path_custom[16];
unsigned char path_counter=0;
unsigned char *path=NULL;

unsigned char inputpath_counter=0;
unsigned char arrow[]="^<>!.";
unsigned char indexarrow=0;

// Input/Output Ports initialization
// Port A initialization
/*
PORT A
    0 -> Output (Multiplexer select A)
    1 -> Output (Multiplexer select B)
    2 -> Output (Multiplexer select C)
    3 -> (Tidak digunakan)
    4 -> Input (ADC sensor bagian kanan)
    5 -> Input (ADC sensor bagian kiri)
    6 -> Output (Aktivator multoplexer kanan (output 0 = aktif))
    7 -> Output (Aktivator multiplexer kiri)
*/
PORTA=0x00;
DDRA=0b11000111;

// Port B initialization
/*
PORT B
    0 -> Output (Motor kiri enable)
    1 -> Output (Motor kanan enable)
    2 -> (Pin tambahan P1)
    3 -> Output (PWM kiri)
    4 -> Input (Switch 1)
    5 -> Input (Switch 2)
    6 -> Input (Switch 3)
    7 -> Input (Switch 4)
*/
PORTB=0xF0;
DDRB=0b00001011;

// Port C initialization
/*
PORT C: LCD (input/output otomatis)
    0-2 -> (LCD control)
    3   -> Output (LCD backlight)
    4-7 -> (data)
*/
PORTC=0x00;
DDRC=0b00001000;

/*
PORT D
    0 -> Input (Rx (kalau pakai serial))
    1 -> Output (Tx)
    2 -> (Pin tambahan P2)
    3 -> (Pin tambahan P3)
    4 -> Output (Arah motor kiri)
    5 -> Output (Arah motor kanan)
    6 -> (Pin tambahan P4)
    7 -> Output (PWM kanan)
*/
PORTD=0x00;
DDRD=0b10110000;

// Timer 0, untuk PWM kiri
// 250kHz, inverting output
TCCR0=0x7B;
TCNT0=0x00;
OCR0=0x00;

// Timer 1, untuk masalah sampling setiap 10ms
TCCR1A=0x00;
TCCR1B=0x04;
TCNT1H=0x00;
TCNT1L=0x00;
ICR1H=0x00;
ICR1L=0x00;
OCR1AH=0x02;
OCR1AL=0x71;
OCR1BH=0x00;
OCR1BL=0x00;

// Timer 2, untuk PWM kanan
// 250kHz, inverting output
ASSR=0x00;
TCCR2=0x7C;
TCNT2=0x00;
OCR2=0x00;

MCUCR=0x00;
MCUCSR=0x00;

// Aktifkan bit interupsi timer1
TIMSK=0x10;

UCSRB=0x00;

ACSR=0x80;
SFIOR=0x00;

ADMUX=ADC_VREF_TYPE & 0xff;
ADCSRA=0x86;

SPCR=0x00;
TWCR=0x00;

#asm("sei")
// Alphanumeric LCD initialization
// Connections are specified in the
// Project|Configure|C Compiler|Libraries|Alphanumeric LCD menu:
// RS - PORTC Bit 0
// RD - PORTC Bit 1
// EN - PORTC Bit 2
// D4 - PORTC Bit 4
// D5 - PORTC Bit 5
// D6 - PORTC Bit 6
// D7 - PORTC Bit 7
// Characters/line: 16
lcd_init(16);

sprintf(str1,"EL3014 ");
sprintf(str2,"SISMIKBOT");
lcd_clear();
lcd_puts(str1);
lcd_gotoxy(0,1);
lcd_puts(str2);
LCD=1;
delay_ms(1000);

while (1){
    if(jalan==GO_INIT){
       if(checkpoint==0){
            path=path0;    // variabel path dan path0 merupakan pointer
        }else if(checkpoint==1){
            path=path1;  
        }else if(checkpoint==2){
            path=path2;
        }else if(checkpoint==3){
            path=path3;
		}else if(checkpoint==255){
            for(path_counter=0;path_counter<16;path_counter++){
                path_custom[path_counter]=pathcust[path_counter];
            }
            path=path_custom;
        }else{ // mengikuti garis secara normal, tanpa perencanaan jalur
            path=NULL;
        }
        path_counter=0;
        jalan=GO_MAIN;
    }else if(jalan==GO_MAIN){
        // Algoritma utama di sini 
        // Algoritma Perencanaan Belok
               
        
        if(path[path_counter]=='>'){ // belok kanan
            //while (((sensor&0b000001100000) == 0) || ((sensor&0b000000000111) == 0)){
            while ((lebar_garis <= 3) || ((sensor&0b000000001111) == 0)){
                mode=NAV_JALAN_PID;
            }
            mode=NAV_REM;
            delay_ms(150);  
            mode=NAV_ROTASI_KANAN;
            while((sensor&0b000000000110)==0 || (sensor&0b100000000000)!=0);
            mode=NAV_REM;
            delay_ms(50);
            mode=NAV_JALAN_PID;  
            path_counter++; 
        }else if(path[path_counter]=='<')
        { // belok kiri
            //while (((sensor&0b000001100000) == 0) || ((sensor&0b111000000000) == 0)){
            while ((lebar_garis <= 3) || ((sensor&0b110000000000) == 0)){
                mode=NAV_JALAN_PID;
            }
            mode=NAV_REM;
            delay_ms(100);  
            mode=NAV_ROTASI_KIRI;
            while((sensor&0b011000000000)==0 || (sensor&0b000000000001)!=0);
            mode=NAV_REM;
            delay_ms(100);
            mode=NAV_JALAN_PID;   
            path_counter++; 
        }else if(path[path_counter]=='^'){
            while (((sensor&0b000001100000) == 0) || ((sensor&0b100000000001) == 0)){
                mode=NAV_JALAN_PID;
            }
            mode=NAV_REM;
            delay_ms(100);  
            mode=NAV_JALAN_PID;
            mode=NAV_REM;
            delay_ms(50);
            mode=NAV_JALAN_PID;   
            path_counter++; 
        }
        
        else if(path[path_counter]=='!'){ // invert warna garis  
            while ((sensor&0b100000000001) == 0){
                mode=NAV_JALAN_PID;
            }
            invert=!invert; 
            //mode=NAV_REM;
            delay_ms(100);
            mode=NAV_JALAN_PID;   
            path_counter++; 
        }
        
        else if(path[path_counter]=='.'){ // stop
            path_counter=0;
            jalan=0;
            mode=NAV_REM;
            LCD=0;
            delay_ms(100);
            LCD=1;
            delay_ms(100);
            LCD=0;
            delay_ms(100);
            LCD=1;
            mode=NAV_BERHENTI;     
            //path_counter++;        
        }
        
        else{
            mode=NAV_JALAN_PID;
        }
        if(!S3){
            while(!S3);
			mode=NAV_BERHENTI;
			path_counter=0;
            jalan=0;
        }
      }else{     
        #include "tampilanlcd.c"
      }
      }
}

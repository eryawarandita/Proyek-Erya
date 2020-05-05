/*********************************************************************************************/
/** GARUDAGO-ITB (KRAI2016)                                                                 **/
/** #ROADTOBANGKOK!                                                                         **/
/**                                                                                         **/
/** MAIN PROGRAM ROBOT HYBRID SEMI OTOMATIS                                                 **/
/**                                                                                         **/
/**                                                                                         **/
/** Created by :                                                                            **/
/** Rizqi Cahyo Yuwono                                                                      **/
/** M. Nibrosul Umam                                                                        **/
/** Erya Warandita                                                                          **/
/**                                                                                         **/
/**                                                                                         **/
/** Last Update : 19 Desember 2016, 06.10 WIB                                               **/
/*********************************************************************************************/
 
/*********************************************************************************************/
/**     FILE HEADER                                                                         **/
/*********************************************************************************************/
#include "mbed.h"
#include "Motor.h"
#include "NewTextLCD.h"
#include "PS_PAD.h"
#include "PID.h"
#include "millis.h"
 
/*********************************************************************************************/
/**     DEKLARASI INPUT OUTPUT                                                              **/
/*********************************************************************************************/
// serial pc
Serial pc(USBTX,USBRX);
 
// LCD 20x4
TextLCD lcd(PC_5, PB_1, PA_7, PC_4, PA_5, PA_6, TextLCD::LCD20x4); //(rs,e,d4,d5,d6,d7)
 
// joystick PS2
PS_PAD ps2(PB_15,PB_14,PB_13, PB_12); //(mosi, miso, sck, ss)
 
// PID sensor garis 
PID PID(0.992,0.000,0.81,0.001); //(P,I,D, time sampling)
 
// Motor(pwm, fwd, rev)
Motor gripper(PB_6, PB_8, PB_9);
Motor motor3(PC_6, PC_8, PC_9);
Motor motor2(PB_3, PB_5, PB_4); //kanan
Motor motor1(PA_8, PA_9, PC_7); //kiri
 
// Sensor
DigitalIn S1(PC_0);
DigitalIn S2(PC_1);
DigitalIn S3(PC_2);
DigitalIn S4(PC_3);
DigitalIn S5(PA_0);
DigitalIn S6(PA_1);
DigitalIn S7(PA_4);
DigitalIn S8(PB_0);
DigitalIn S9(PB_2);
DigitalIn S10(PB_10);
DigitalIn S11(PA_10);
DigitalIn S12(PA_11);
DigitalIn S13(PA_12);
DigitalOut calibrate(PA_15);
 
DigitalIn button(USER_BUTTON);
 
bool sensor[13];
 
//DigitalIn limit_switch1(A0);
//DigitalIn limit_switch2(A1);
 
 
// Multitasker
Ticker timer;
 
 
/*********************************************************************************************/
/**     DEKLARASI VARIABEL GLOBAL                                                           **/
/*********************************************************************************************/
float gMax_speed=0.7; //nilai maksimum kecepatan motor
float gMin_speed=-0.05;  //nilai minimum kecepatan motor
float gTuning = 0.36;
 
unsigned char gMode=0;  //variabel mode driving (manual = 0 otomatis = 1)
unsigned char gCase=0;  //variabel keadaan proses
 
unsigned char i; // variabel iterasi
int over=0;
 
 
  
/*********************************************************************************************/
/**     DEKLARASI PROSEDUR DAN FUNGSI                                                        **/
/*********************************************************************************************/     
void init_sensor()
{
    if((ps2.read(PS_PAD::PAD_CIRCLE)==1))
    {
        calibrate=0;
    }
    else
    {
            calibrate=1;
    }
     
    sensor[0]=(S1.read()==1);
    sensor[1]=(S2.read()==1);
    sensor[2]=(S3.read()==1);
    sensor[3]=(S4.read()==1);
    sensor[4]=(S5.read()==1);
    sensor[5]=(S6.read()==1);
    sensor[6]=(S7.read()==1);
    sensor[7]=(S8.read()==1);
    sensor[8]=(S9.read()==1);
    sensor[9]=(S10.read()==1);
    sensor[10]=(S11.read()==1);
    sensor[11]=(S12.read()==1);
    sensor[12]=(S13.read()==1);
}
 
 
void PIDrunning()   //menjalankan perintah untuk line follower
{  
    int pv;
 
    float speedR,speedL;
    
    //init_sensor();
    //////////////////logic dari PV (present Value)/////////////////////////
    if(sensor[0]){
        pv = -12;
        over=1;
    }
    else if(sensor[12]){
        pv = 12;
        over=2;
    }
    else if(sensor[0] && sensor[1]){
        pv = -10;
    }
    else if(sensor[11] && sensor[12]){
        pv = 10;
    }
    else if(sensor[1]){
        pv = -9;
    }
    else if(sensor[11]){
        pv = 9;
    }
    else if(sensor[1] && sensor[2]){
        pv = -8;
    }
    else if(sensor[10] && sensor[11]){
        pv = 8;
    }
    else if(sensor[2]){
        pv = -7;
    }
    else if(sensor[10]){
        pv = 7;
    }
    else if(sensor[2] && sensor[3]){
        pv = -6;
    }
    else if(sensor[9] && sensor[10]){
        pv = 6;
    }
    else if(sensor[3]){
        pv = -5;
    }
    else if(sensor[9]){
        pv = 5;
    }
    else if(sensor[3] && sensor[4]){
        pv = -4;
    }
    else if(sensor[8] && sensor[9]){
        pv = 4;  
    }
    else if(sensor[4]){
        pv = -3;
    }
    else if(sensor[8]){
        pv = 3;
    }
    else if(sensor[4] && sensor[5]){
        pv = -2;
    }
    else if(sensor[7] && sensor[8]){
        pv = 2;
    }
    else if(sensor[5]){
        pv = -1;
    }  
     else if(sensor[7]){
        pv = 1;
    }
    else if(sensor[5] && sensor[6]){
        pv = -0.5;    
    }
    else if(sensor[6] && sensor[7]){
        pv = 0.5;
    } 
    else if (sensor[6]){
        pv = 0;
    }
    else
    {
        ///////////////// robot bergerak keluar dari sensor/////////////////////
        if(over==1){
            /*if(speed_ka > 0){
                wait_ms(10);
                motor2.speed(-speed_ka);
                wait_ms(10);
                }
            else{
                motor2.speed(speed_ka);
                }
            wait_ms(10);*/
            
            motor1.brake(1);
            //wait_ms(100);
            
        }
        else if(over==2){
            /*if(speed_ki > 0){
                wait_ms(10);
                motor1.speed(-speed_ki);
                wait_ms(10);
                }
            else{
                wait_ms(10);
                motor1.speed(speed_ki);
                wait_ms(10);
                }
            wait_ms(10);*/
            motor2.brake(1);
            //wait_ms(100);
        }
    } 
    PID.setProcessValue(pv);
    PID.setSetPoint(0);
    
    // memulai perhitungan PID
 
    speedR = gMax_speed - PID.compute();
    if(speedR > gMax_speed){
        speedR = gMax_speed;
        }
    else if(speedR < gMin_speed)
        speedR = gMin_speed;
    motor2.speed(speedR);
 
    speedL = gMax_speed + PID.compute();
    if(speedL > gMax_speed)
        speedL = gMax_speed;
    else if(speedL < gMin_speed)
        speedL = gMin_speed;
    motor1.speed(speedL);
}
 
void showLCD()  //menampilkan user interface pada LCD
{   
    lcd.cls();
    lcd.locate(5,0);
    lcd.printf("GarudaGo !!");
    
    switch(gCase)
    {
        case 0 :
        {
            lcd.locate(2,2);
            lcd.printf("speed = %.4f",gMax_speed);
            break;
        }
        case 1 :
        {   
            if (gMode == 1)
            {
                lcd.locate(3,1);
                lcd.printf("Mode = Otomatis");
            }
            else if (gMode==0)
            {
                lcd.locate(3,1);
                lcd.printf("Mode = Manual");
            }
            break;
        }
    }
     
        lcd.locate(2,3);
        lcd.printf("%d",S1.read());
        lcd.locate(3,3);
        lcd.printf("%d",S2.read());
        lcd.locate(4,3);
        lcd.printf("%d",S3.read());
        lcd.locate(5,3);
        lcd.printf("%d",S4.read());
        lcd.locate(6,3);
        lcd.printf("%d",S5.read());
        
        lcd.locate(7,3);
        lcd.printf("%d",S6.read());
        lcd.locate(8,3);
        lcd.printf("%d",S7.read());
        lcd.locate(9,3);
        lcd.printf("%d",S8.read());
        lcd.locate(10,3);
        lcd.printf("%d",S9.read());
        lcd.locate(11,3);
        lcd.printf("%d",S10.read());
        
        lcd.locate(12,3);
        lcd.printf("%d",S11.read());
        lcd.locate(13,3);
        lcd.printf("%d",S12.read());
        lcd.locate(14,3);
        lcd.printf("%d",S13.read());
}
 
 
void running()  //prosedur utama untuk kendali robot
{
    float speed=gMax_speed;
    float k=1;
    
    switch(gCase)
    {
        case 0 :
        {
            motor1.brake(1);
            motor2.brake(1);
 
            if((ps2.read(PS_PAD::PAD_R2)==1) && (ps2.read(PS_PAD::PAD_L2)==0))
                gMax_speed += 0.00008;
            else if((ps2.read(PS_PAD::PAD_R2)==0) && (ps2.read(PS_PAD::PAD_L2)==1))
                gMax_speed -= 0.00008;
                
            if (gMax_speed > 1.0)
                gMax_speed = 1.0;
            else if(gMax_speed < 0)
                gMax_speed = 0.0;
            
            if((ps2.read(PS_PAD::PAD_START)==1) && (ps2.read(PS_PAD::PAD_R2)==0) && (ps2.read(PS_PAD::PAD_L2)==0))
            {               
                gCase++;
                wait_ms(200);
            }
            break;   
        }
        case 1 : 
        {
            if (gMode == 1)
            {
                if ((ps2.read(PS_PAD::PAD_X)==1))
                {
                    PIDrunning();
                    pc.printf("PID \t %f \t ",PID.compute());
                    
                    for(i=0;i<13;i++)
                    {
                        pc.printf("%i \t",sensor[i]);
                    }
                    pc.printf("\n");
                }
                else if((ps2.read(PS_PAD::PAD_X)==0))
                {
                    motor1.brake(1);
                    motor2.brake(1);
                    pc.printf("PID stop \n");
                }
            }
            else if (gMode == 0)
            {
                if ((ps2.read(PS_PAD::PAD_TOP)==1) && (ps2.read(PS_PAD::PAD_BOTTOM)==0) && (ps2.read(PS_PAD::PAD_R1)==0) && (ps2.read(PS_PAD::PAD_L1)==0)) //maju
                {
                    if (speed < 0.1)
                        speed = 0.1;
                    else
                        speed += 0.0001;
                    
                    if (speed >= gMax_speed)
                        speed = gMax_speed;  
                    
                    motor1.speed(speed*k);
                    motor2.speed(speed*k);
                    pc.printf("maju \n");
                }
                else if ((ps2.read(PS_PAD::PAD_TOP)==0) && (ps2.read(PS_PAD::PAD_BOTTOM)==1) && (ps2.read(PS_PAD::PAD_R1)==0) && (ps2.read(PS_PAD::PAD_L1)==0)) //mundur
                {
                    if (speed < 0.1)
                        speed = 0.1;
                    else
                        speed += 0.0001;
                    
                    if (speed >= gMax_speed)
                        speed = gMax_speed;
                        
                    motor1.speed(-speed*k);
                    motor2.speed(-speed*k);
                    pc.printf("mundur \n");
                }
                else if ((ps2.read(PS_PAD::PAD_TOP)==0) && (ps2.read(PS_PAD::PAD_BOTTOM)==0) && (ps2.read(PS_PAD::PAD_R1)==0) && (ps2.read(PS_PAD::PAD_L1)==1)) //pivot kiri
                {
                    if (speed < 0.1)
                        speed = 0.1;
                    else
                        speed += 0.0001;
                    
                    if (speed >= gMax_speed)
                        speed = gMax_speed;
                        
                    motor1.speed(speed*k*0.7);
                    motor2.speed(-speed*k*0.7);
                    pc.printf("piv kiri \n");
                }
                else if ((ps2.read(PS_PAD::PAD_TOP)==0) && (ps2.read(PS_PAD::PAD_BOTTOM)==0) && (ps2.read(PS_PAD::PAD_R1)==1) && (ps2.read(PS_PAD::PAD_L1)==0)) //pivot kanan
                {
                    if (speed < 0.1)
                        speed = 0.1;
                    else
                        speed += 0.0001;
                    
                    if (speed >= gMax_speed)
                        speed = gMax_speed;
                        
                    motor1.speed(speed*k*0.7-gTuning);
                    motor2.speed(-speed*k*0.7);
                    pc.printf("piv kanan \n");
                }
                else if ((ps2.read(PS_PAD::PAD_TOP)==0) && (ps2.read(PS_PAD::PAD_BOTTOM)==0) && (ps2.read(PS_PAD::PAD_R1)==0) && (ps2.read(PS_PAD::PAD_L1)==0))
                {
                    motor1.brake(0);
                    motor2.brake(0);
                    pc.printf("stop \n");
                }
                
            }
            else
            {
                motor1.brake(1);
                motor2.brake(1);
                pc.printf("no PID no MANUAL \n");
            }
            
            if((ps2.read(PS_PAD::PAD_SELECT)==1) && (ps2.read(PS_PAD::PAD_START)==0))
            {
                if (gMode==1)
                {
                       gMode=0;
                       wait_ms(200);
                }
                else
                {
                    gMode=1;
                    wait_ms(200);
                }
            }
        
            if((ps2.read(PS_PAD::PAD_START)==1) && (ps2.read(PS_PAD::PAD_SELECT)==0))
            {
                    gCase--;
                    wait_ms(200);
            }
                
            break;
        }   
    }
}
 
/*********************************************************************************************/
/*********************************************************************************************/
/**     PROGRAM UTAMA                                                                       **/
/*********************************************************************************************/
/*********************************************************************************************/
int main(void){
    //inisialisasi joystick
    ps2.init();
    
    //inisialisasi PID
    PID.setInputLimits(-15,15);
    PID.setOutputLimits(-1.0,1.0);
    PID.setMode(1.0);
    PID.setBias(0.0);
    PID.reset();
    
    // serial PC
    pc.baud(115200);
    
    //multitasking untuk menampilkan layar
    startMillis();
    uint32_t lastTime = 0;
    int lcd_refresh_rate = 10;
    
    while(1)
    {  
        if(millis() - lastTime >= lcd_refresh_rate)
        {
            showLCD;
        }
        init_sensor();
        ps2.poll();    
        running();
    }
}
            
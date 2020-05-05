/*********************************************************************************************/
/** GARUDAGO-ITB (KRAI2016)                                                                 **/
/** #ROADTOBANGKOK!                                                                         **/
/**                                                                                         **/
/** MAIN PROGRAM FINAL ROBOT HYBRID                                                         **/
/**                                                                                         **/
/**                                                                                         **/
/** Created by :                                                                            **/
/** Rizqi Cahyo Yuwono                                                                      **/
/** M. Nibrosul Umam                                                                        **/
/** Erya Warandita                                                                          **/
/**                                                                                         **/
/**                                                                                         **/
/** Last Update : 10 feb 2017, 10.10 WIB                                                    **/
/*********************************************************************************************/
#include "mbed.h"
#include "motordriver.h"
#include "PS_PAD.h"
#include "esc.h"
#include "Servo.h"
#include "Ping.h"
 
/*********************************************************************************************/
/**     DEKLARASI INPUT OUTPUT                                                              **/
/*********************************************************************************************/
//serial
Serial pc(USBTX,USBRX);      //debug
Serial com1(PA_0,PA_1);      //sensor depan
Serial com2(PC_6, PC_7);     //sensor belakang
 
//joystick PS2
PS_PAD ps2(PB_15,PB_14,PB_13, PC_4); //(mosi, miso, sck, ss) 
 
//motor (PWM, forward, reverse)
Motor motor3(PA_8, PB_0, PC_15, 1); //motor1
Motor motor1(PA_11, PA_6, PA_5, 1); //motor2
Motor motor4(PA_9, PC_2, PC_3, 1);   //motor_slider
Motor motor2(PA_10, PB_5, PB_4, 1);  //motor_griper
Motor motorC1(PB_7, PA_14, PA_15 , 1); //motor4
Motor motorC2(PB_9, PA_12, PC_5, 1);  //motor6
Motor motorS(PB_8, PA_13, PB_1, 1);  //motor5
//Motor motor4(PB_6, PA_7, PB_12);
 
//pnuematik 
DigitalOut pnuematik1(PC_11);
DigitalOut pnuematik2(PC_10);
DigitalOut pnuematik3(PD_2);
DigitalOut pnuematik4(PC_12);
 
//Limit Switch
DigitalIn limit1(PC_13 ,PullUp);
DigitalIn limit2(PC_14 ,PullUp);
DigitalIn limit3(PC_1 ,PullUp);
DigitalIn limit4(PC_0 ,PullUp);
 
DigitalIn field(PB_10 ,PullUp);
 
//laser pointer
DigitalIn IR(PB_3 ,PullUp);
 
//servo(PWM)
Servo servoEDF(PC_8);
 
//EDF(PWM, timer)
ESC edf(PC_9,20);
 
//Timer
Ticker timer;
Timer timer_linetracer;
 
//Sensor Ping
Ping ping(PB_2);
 
/*********************************************************************************************/
/**     DEKLARASI VARIABEL, PROSEDUR, DAN FUNGSI                                                       **/
/*********************************************************************************************/ 
const float gMax_speed = 0.85;
//const float gMin_speed = 0.1;
const float v0 = 0.4;
const float ax = 0.0005;
float vcurr_base = v0;
float vcurr_slider = v0;
 
const float tuning1 = 0.03;
const float tuning2 = 0.06;
const float tuning3 = 0.03;
const float tuning4 = 0.0 ;
 
const float driver1_0 = 1;
const float driver1_1 = 0.91;
const float driver1_2 = 0.78;
const float driver1_3 = 0.55;
 
// inisialisasi pwm awal servo
double pwm = 0.00;
 
// inisialisasi sudut awal
double sudut = -85;
 
// variabel counting
unsigned long int g = 0;
 
//slider auto
int c =0;  
int batas_delay = 340;
int flag_river;
 
//data sensor garis dan line following
int datasensor1[6];
int datasensor2[6];
int over;
int g_flag1;
int g_flag2;
 
 
void aktuator(int f);
void edf_servo();
 
void init_sensor();
void linetracer(float speed);
void flag_sensor();
void putar(float speed);
 
float read_jarak();
 
/*********************************************************************************************/
/*********************************************************************************************/
/**     PROGRAM UTAMA                                                                       **/
/*********************************************************************************************/
/*********************************************************************************************/ 
int main() {
    //inisiasi serial
    pc.baud(115200);
    com1.baud(115200);
    com2.baud(115200);
    
    if(field == 1)      sudut = -85;
    else                sudut = 85;
    
    //inisisasi flag river
    flag_river = 0;
    
    //inisiasi joystick
    ps2.init();
    
    //set inisiasi servo pada posisi 0 
    servoEDF.position(sudut);
 
    //set edf pada posisi bukan kalibrasi, yaitu set edf 0
    edf.setThrottle(0);
    edf.pulse();
     
    //inisiasi pnuematik
    pnuematik1 = 1;
    pnuematik2 = 1;
    pnuematik3 = 1;
    pnuematik4 = 1;
    
    //inisisasi TIMER
    //timer.attach(&flag_sensor,0.0005);
    //timer.detach();
    timer_linetracer.reset();
    
    //kondisi robot
    bool manual=true; 
    bool pool= false;
    
    //running manual
    while(manual){
        
        ps2.poll();
        
        aktuator(field);
        edf_servo();
        
        if(limit4==0)    manual = false;
        
    }
    
    //running otomatis
    timer.attach(&flag_sensor,0.0005);
    timer_linetracer.start();
    vcurr_base = v0;
    
    while(!pool){
        init_sensor();
        
        if(vcurr_base > (float)1)  vcurr_base = (float) 1;
        //else if (vcurr_base < 0.2)   vcurr_base = (float) 0.2;
        
        linetracer(vcurr_base);            
        
 
        if(timer_linetracer.read_ms() >= 2500){
            vcurr_base = 0.4;
        }
        else{
            vcurr_base += 0.002;
        }
        
        for(int i=0; i<6; i++){
            printf("%d  ",datasensor1[i]);
        }
        for(int i=5; i>=0; i--){
            printf("%d  ",datasensor2[i]);
        }
        printf("%d  %d   %lf \n",g_flag1, g_flag2, read_jarak());
        
        if((limit3==0) || (read_jarak() <= (float)5.0))   pool=true;   
    }
    putar(0.2);    
    
    timer.detach();
    timer_linetracer.stop();
    
    motor1.stop(1);
    motor2.stop(1);
    motor3.stop(1);
    motor4.stop(1); 
    
    wait_ms(200);
    pnuematik1=0;
    
    wait_ms(1000);
    
    while(limit4!=0){
        motorC1.speed(1);
        motorC2.speed(-1);
        
     /*   if(g < 600000){
            motor1.speed(-(1-tuning1));
            motor2.speed(-(1-tuning2));
            motor3.speed(-(1-tuning3));
            motor4.speed(-(1-tuning4)); 
            
            g++;
        }
        else{
            motor1.stop(1);
            motor2.stop(1);
            motor3.stop(1);
            motor4.stop(1);
        }*/
    }
    
    motorC1.stop(1);
    motorC2.stop(1);
    
    if(field==1){
        pnuematik3 = 0;
        wait_ms(1300);
        pnuematik2 = 0;
        wait_ms(1500);
        pnuematik3 = 1;
    }
    else{
        pnuematik4 = 0;
        wait_ms(1300);
        pnuematik2 = 0;
        wait_ms(1500);
        pnuematik4 = 1;
    }
    
    return 0;
}
 
/*********************************************************************************************/
/**     ALGORITMA PROSEDUR DAN FUNGSI                                                       **/
/*********************************************************************************************/ 
void aktuator(int f){
    float speed = vcurr_base;
    
    if(vcurr_base >= gMax_speed)    vcurr_base = gMax_speed;
    if(vcurr_slider >= 1)           vcurr_slider = 1; 
    
    if(f == 1){
        if((ps2.read(PS_PAD::PAD_L1)==1) && (ps2.read(PS_PAD::PAD_R1)==0)){
            //pivot kiri                      
            motor2.speed((float)0.5*(speed-tuning2));
            motor4.speed((float)-0.5*(speed-tuning4));
            motor1.speed((float)0.5*(speed-tuning1));
            motor3.speed((float)-0.5*(speed-tuning3));
            pc.printf("pivot kiri \n");
            
            vcurr_base += ax;
        }
        else if((ps2.read(PS_PAD::PAD_R1)==1) && (ps2.read(PS_PAD::PAD_L1)==0)){
            //pivot kanan                      
            motor2.speed((float)-0.5*(speed-tuning2));
            motor4.speed((float)0.5*(speed-tuning4));
            motor1.speed((float)-0.5*(speed-tuning1));
            motor3.speed((float)0.5*(speed-tuning3));
            pc.printf("pivot kanan \n");
            
            vcurr_base += ax;
        }
        else if((ps2.read(PS_PAD::PAD_RIGHT)==1) && (ps2.read(PS_PAD::PAD_TOP)==1)){
            //serong atas kanan                      
            motor2.speed(speed-tuning2);
            motor4.stop(1);
            motor1.stop(1);
            motor3.speed(speed-tuning3);
            pc.printf("serong atas kanan \n");
            
            vcurr_base += ax;
        }
        else if((ps2.read(PS_PAD::PAD_TOP)==1) && (ps2.read(PS_PAD::PAD_LEFT)==1)){
            //serong atas kiri                       
            motor2.stop(1);
            motor4.speed(-(speed-tuning4));
            motor1.speed(-(speed-tuning1));
            motor3.stop(1);
            pc.printf("serong atas kiri \n");
            
            vcurr_base += ax;
        }
        else if((ps2.read(PS_PAD::PAD_LEFT)==1) && (ps2.read(PS_PAD::PAD_BOTTOM)==1)){
            //serong bawah kiri                     
            motor2.speed(-(speed-tuning2));
            motor4.stop(1);
            motor1.stop(1);
            motor3.speed(-(speed-tuning3));
            pc.printf("serong bawah kiri \n");
            
            vcurr_base += ax;
        }
        else if((ps2.read(PS_PAD::PAD_BOTTOM)==1) && (ps2.read(PS_PAD::PAD_RIGHT)==1)){
            //serong bawah kanan                     
            motor2.stop(1);
            motor4.speed(speed-tuning4);
            motor1.speed(speed-tuning1);
            motor3.stop(1);
            pc.printf("serong bawah kanan \n");
            
            vcurr_base += ax;
        }
        else if((ps2.read(PS_PAD::PAD_TOP)==1) && (ps2.read(PS_PAD::PAD_BOTTOM)==0)){
            //maju
            motor1.speed(-(speed-tuning1));
            motor3.speed(speed-tuning3);
            motor2.speed(speed-tuning2);
            motor4.speed(-(speed-tuning4));
            pc.printf("maju \n");
            
            vcurr_base += ax;
        }
        else if((ps2.read(PS_PAD::PAD_BOTTOM)==1) && (ps2.read(PS_PAD::PAD_TOP)==0)){
            //mundur     
            motor1.speed(speed-tuning1);
            motor3.speed(-(speed-tuning3));
            motor2.speed(-(speed-tuning2));
            motor4.speed(speed-tuning4);
            pc.printf("mundur \n");
            
            vcurr_base += ax;
        } 
        else if((ps2.read(PS_PAD::PAD_RIGHT)==1) && (ps2.read(PS_PAD::PAD_LEFT)==0)){
            //kanan                     
            motor2.speed(speed-tuning2);
            motor4.speed(speed-tuning4);
            motor1.speed(speed-tuning1);
            motor3.speed(speed-tuning3);
            pc.printf("kanan \n");
            
            vcurr_base += ax;
        }
        else if((ps2.read(PS_PAD::PAD_RIGHT)==0) && (ps2.read(PS_PAD::PAD_LEFT)==1)){
            //kiri                       
            motor2.speed(-(speed-tuning2));
            motor4.speed(-(speed-tuning4));
            motor1.speed(-(speed-tuning1));
            motor3.speed(-(speed-tuning3));
            pc.printf("kiri \n");
            
            vcurr_base += ax;
        }
        else{
            motor1.stop(1);
            motor3.stop(1);
            motor2.stop(1);
            motor4.stop(1);
            pc.printf("diam \n");
            
            vcurr_base = v0;
        }
    }
    else{
        if((ps2.read(PS_PAD::PAD_L1)==1) && (ps2.read(PS_PAD::PAD_R1)==0)){
            //pivot kiri                      
            motor2.speed((float)0.5*(speed-tuning2));
            motor4.speed((float)-0.5*(speed-tuning4));
            motor1.speed((float)0.5*(speed-tuning1));
            motor3.speed((float)-0.5*(speed-tuning3));
            pc.printf("pivot kiri \n");
            
            vcurr_base += ax;
        }
        else if((ps2.read(PS_PAD::PAD_R1)==1) && (ps2.read(PS_PAD::PAD_L1)==0)){
            //pivot kanan                      
            motor2.speed((float)-0.5*(speed-tuning2));
            motor4.speed((float)0.5*(speed-tuning4));
            motor1.speed((float)-0.5*(speed-tuning1));
            motor3.speed((float)0.5*(speed-tuning3));
            pc.printf("pivot kanan \n");
            
            vcurr_base += ax;
        }
        else if((ps2.read(PS_PAD::PAD_RIGHT)==1) && (ps2.read(PS_PAD::PAD_TOP)==1)){
            //serong atas kanan                                  
            motor2.speed(-(speed-tuning2));
            motor4.stop(1);
            motor1.stop(1);
            motor3.speed(-(speed-tuning3));
            pc.printf("serong atas kanan \n");
            
            vcurr_base += ax;
        }
        else if((ps2.read(PS_PAD::PAD_TOP)==1) && (ps2.read(PS_PAD::PAD_LEFT)==1)){
            //serong atas kiri                                   
            motor2.stop(1);
            motor4.speed(speed-tuning4);
            motor1.speed(speed-tuning1);
            motor3.stop(1);
            pc.printf("serong atas kiri \n");
            
            vcurr_base += ax;
        }
        else if((ps2.read(PS_PAD::PAD_LEFT)==1) && (ps2.read(PS_PAD::PAD_BOTTOM)==1)){
            //serong bawah kiri 
            motor2.speed(speed-tuning2);
            motor4.stop(1);
            motor1.stop(1);
            motor3.speed(speed-tuning3);                    
            pc.printf("serong bawah kiri \n");
            
            vcurr_base += ax;
        }
        else if((ps2.read(PS_PAD::PAD_BOTTOM)==1) && (ps2.read(PS_PAD::PAD_RIGHT)==1)){
            //serong bawah kanan                     
            motor2.stop(1);
            motor4.speed(-(speed-tuning4));
            motor1.speed(-(speed-tuning1));
            motor3.stop(1);
            pc.printf("serong bawah kanan \n");
            
            vcurr_base += ax;
        }
        else if((ps2.read(PS_PAD::PAD_TOP)==1) && (ps2.read(PS_PAD::PAD_BOTTOM)==0)){
            //maju
            motor1.speed(speed-tuning1);
            motor3.speed(-(speed-tuning3));
            motor2.speed(-(speed-tuning2));
            motor4.speed(speed-tuning4);
            pc.printf("maju \n");
            
            vcurr_base += ax;
        }
        else if((ps2.read(PS_PAD::PAD_BOTTOM)==1) && (ps2.read(PS_PAD::PAD_TOP)==0)){
            //mundur     
            motor1.speed(-(speed-tuning1));
            motor3.speed(speed-tuning3);
            motor2.speed(speed-tuning2);
            motor4.speed(-(speed-tuning4));
            pc.printf("mundur \n");
            
            vcurr_base += ax;
        } 
        else if((ps2.read(PS_PAD::PAD_RIGHT)==1) && (ps2.read(PS_PAD::PAD_LEFT)==0)){
            //kanan                     
            motor2.speed(-(speed-tuning2));
            motor4.speed(-(speed-tuning4));
            motor1.speed(-(speed-tuning1));
            motor3.speed(-(speed-tuning3));
            pc.printf("kanan \n");
            
            vcurr_base += ax;
        }
        else if((ps2.read(PS_PAD::PAD_RIGHT)==0) && (ps2.read(PS_PAD::PAD_LEFT)==1)){
            //kiri                                 
            motor2.speed(speed-tuning2);
            motor4.speed(speed-tuning4);
            motor1.speed(speed-tuning1);
            motor3.speed(speed-tuning3);
            pc.printf("kiri \n");
            
            vcurr_base += ax;
        }
        else{
            motor1.stop(1);
            motor3.stop(1);
            motor2.stop(1);
            motor4.stop(1);
            pc.printf("diam \n");
            
            vcurr_base = v0;
        }
    
    }    
        
    if(((ps2.read(PS_PAD::PAD_CIRCLE)==0) && (ps2.read(PS_PAD::PAD_TRIANGLE)==1))|| (IR == 0)){
            //POWER WINDOW ATAS
        motorS.speed(vcurr_slider);
        if (limit1 == 0){
            motorS.stop(1);
        }
            
            
        pc.printf("up \n");
        c++;
        
        vcurr_slider += ax;
    }
    else if((ps2.read(PS_PAD::PAD_CIRCLE)==1) && (ps2.read(PS_PAD::PAD_TRIANGLE)==0)){
            //POWER WINDOW BAWAH        
        motorS.speed(-vcurr_slider);
            
        if (limit2 ==0){
            motorS.stop(1);
        }
            
        pc.printf("down \n");
        c--;     
        
        vcurr_slider += ax;   
    }
    else{
        vcurr_slider = v0;
        motorS.stop(1);
        if ((c <= batas_delay) && (c>=-batas_delay)){
            c=0;
        }
            
        pc.printf("diam \n");
    }           
        
    if((c > batas_delay) && (limit1 == 0)){
        c = 0;
        motorS.stop(1);
    }
    else if((c < -batas_delay) && (limit2 == 0)){
        c = 0;
        motorS.stop(1);
    }
    else if( (c > batas_delay) && (limit1 != 0)){
        motorS.speed(1);
    }
    else if ((c<-batas_delay) && (limit2 != 0)){
        motorS.speed(-0.7);
    }
        
        
    if ((ps2.read(PS_PAD::PAD_SELECT)==1))
    {
            //mekanisme ambil gripper
            pc.printf("mekanisme gripper");
           /* if (g==1){
                pc.printf("ambil 1");
                pnuematik2 = 0;
                g=2;
                wait_ms(400);
            }
            else
            {
                pnuematik2 = 1;
                wait_ms(400);
                g=1;
            }*/
            pc.printf("ambil 1");
            pnuematik2 = !pnuematik2;
                //g=2;
            wait_ms(400);
            
    }
}
 
void edf_servo(){
        if(ps2.read(PS_PAD::PAD_X)==1){
            //PWM ++
            
            if( flag_river == 1){
                pwm += 0.0007;
            }
            else{
                pwm = 0.378;
            }
            
            if( pwm > 0.8)  pwm = 0.8;
            pc.printf(" %f gaspol \n", pwm);
        }
        else if(ps2.read(PS_PAD::PAD_SQUARE)==1){
            //PWM--
            pwm -= 0.0007;
            
            if(pwm < 0)    pwm = 0.0;
            pc.printf("%f  rem ndeng \n", pwm);
        }
        
        if(ps2.read(PS_PAD::PAD_R2)==1){
            //SERVO --
            sudut += 0.5;
            
            if(sudut > 90)  sudut = 90;
            pc.printf("servo max \n");
        }
        else if(ps2.read(PS_PAD::PAD_L2)==1){
            //SERVO ++
            sudut -= 0.5;
            
            if(sudut < -90) sudut = -90;
            pc.printf("servo min \n");
        }
        
        if(ps2.read(PS_PAD::PAD_START)==1){
            
            flag_river = 1;
            sudut = 0;
            pwm = 0.295;
        }
        
        
        servoEDF.position((float)sudut);
        edf.setThrottle((float)pwm);
        edf.pulse();
}
 
/////////////////////////////////////////LINE FOLLOWER/////////////////////////
 
void init_sensor(){
    char data;
    if(com1.readable()){  
        data = com1.getc();
        
        for(int i=0; i<6; i++){
           datasensor1[i] = (data >> i) & 1;
       }
    }
    
    if(com2.readable()){  
        data = com2.getc();
        
        for(int i=0; i<6; i++){
           datasensor2[i] = (data >> i) & 1;
       }
    }
}
 
void linetracer(float speed){    
    float speed1,speed2,speed3,speed4;
    
        //////////////////logic dari PV (present Value)/////////////////////////    
        if(datasensor1[2] && datasensor1[3])    {   speed1 = speed*driver1_0;   speed2 = speed*driver1_0;   speed3 = speed*driver1_0;   speed4 = speed*driver1_0;   }
        else if(datasensor1[2])                 {   speed1 = speed*driver1_1;   speed2 = speed*driver1_0;   speed3 = speed*driver1_0;   speed4 = speed*driver1_1;   }
        else if(datasensor1[3])                 {   speed1 = speed*driver1_0;   speed2 = speed*driver1_1;   speed3 = speed*driver1_1;   speed4 = speed*driver1_0;   }
        else if(datasensor1[1])                 {   speed1 = speed*driver1_2;   speed2 = speed*driver1_0;   speed3 = speed*driver1_0;   speed4 = speed*driver1_2;   }
        else if(datasensor1[4])                 {   speed1 = speed*driver1_0;   speed2 = speed*driver1_2;   speed3 = speed*driver1_2;   speed4 = speed*driver1_0;   }
        else if(datasensor1[0])                 {   speed1 = speed*driver1_3;   speed2 = speed*driver1_0;   speed3 = speed*driver1_0;   speed4 = speed*driver1_3;   }
        else if(datasensor1[5])                 {   speed1 = speed*driver1_0;   speed2 = speed*driver1_3;   speed3 = speed*driver1_3;   speed4 = speed*driver1_0;   }
        else
        {
            if(g_flag1 == 0)        {   speed1 = speed*driver1_0;   speed2 = speed*driver1_0;   speed3 = speed*driver1_0;   speed4 = speed*driver1_0;   }
            else if(g_flag1 == 3)   {   speed1 = speed*driver1_1;   speed2 = speed*driver1_0;   speed3 = speed*driver1_0;   speed4 = speed*driver1_1;   }
            else if(g_flag1 == 4)   {   speed1 = speed*driver1_0;   speed2 = speed*driver1_1;   speed3 = speed*driver1_1;   speed4 = speed*driver1_0;   }
            else if(g_flag1 == 2)   {   speed1 = speed*driver1_2;   speed2 = speed*driver1_0;   speed3 = speed*driver1_0;   speed4 = speed*driver1_2;   }
            else if(g_flag1 == 5)   {   speed1 = speed*driver1_0;   speed2 = speed*driver1_2;   speed3 = speed*driver1_2;   speed4 = speed*driver1_0;   }
            else if(g_flag1 == 1)   {   speed1 = 0;                 speed2 = speed*driver1_2;   speed3 = speed*driver1_2;   speed4 = 0;                 }
            else if(g_flag1 == 6)   {   speed1 = speed*driver1_2;   speed2 = 0;                 speed3 = 0;                 speed4 = speed*driver1_2;  }
        } 
 
        motor1.speed(-(float)(speed1-tuning1-0.04));
        motor2.speed(-(float)(speed2-tuning2-0.04));
        motor3.speed(-(float)(speed3-tuning3));
        motor4.speed(-(float)(speed4-tuning4));
 
}
 
void flag_sensor(){
    if((datasensor1[2] == 1) && (datasensor1[3] == 1))   g_flag1 = 0;
    else if(datasensor1[2] == 1)                         g_flag1 = 3;
    else if(datasensor1[3] == 1)                         g_flag1 = 4;
    else if(datasensor1[1] == 1)                         g_flag1 = 2;
    else if(datasensor1[4] == 1)                         g_flag1 = 5;
    else if(datasensor1[0] == 1)                         g_flag1 = 1;
    else if(datasensor1[5] == 1)                         g_flag1 = 6;
    
    if((datasensor2[2] == 1) && (datasensor2[3] == 1))   g_flag2 = 0;
    else if(datasensor2[2] == 1)                         g_flag2 = 3;
    else if(datasensor2[3] == 1)                         g_flag2 = 4;
    else if(datasensor2[1] == 1)                         g_flag2 = 2;
    else if(datasensor2[4] == 1)                         g_flag2 = 5;
    else if(datasensor2[0] == 1)                         g_flag2 = 1;
    else if(datasensor2[5] == 1)                         g_flag2 = 6;
}
 
 
////////////////////////SENSOR PING///////////////////////////////////////
float read_jarak() {
    float jarak;
    
    ping.Send();
    wait_ms(45);
    jarak = ping.Read_cm()/2;
    return jarak;
}
 
 
void putar(float speed){
    float speed1, speed2, speed3, speed4;
    
    while(!((datasensor2[2] == 1) ||  (datasensor2[3] == 1))){
        init_sensor();
        
        if(datasensor2[2] && datasensor2[3])    {   speed1 = 0;                  speed2 = 0;                  speed3 = 0;                  speed4 = 0;                   }
        else if(datasensor2[3])                 {   speed1 = -speed*driver1_3;   speed2 = -speed*driver1_3;   speed3 = -speed*driver1_3;   speed4 = -speed*driver1_3;    }
        else if(datasensor2[2])                 {   speed1 = speed*driver1_3;   speed2 = speed*driver1_3;   speed3 = speed*driver1_3;   speed4 = speed*driver1_3;    }
        else if(datasensor2[4])                 {   speed1 = -speed*driver1_1;   speed2 = -speed*driver1_1;   speed3 = -speed*driver1_1;   speed4 = -speed*driver1_1;    }
        else if(datasensor2[1])                 {   speed1 = speed*driver1_1;   speed2 = speed*driver1_1;   speed3 = speed*driver1_1;   speed4 = speed*driver1_1;    }
        else if(datasensor2[5])                 {   speed1 = -speed*driver1_0;   speed2 = -speed*driver1_0;   speed3 = -speed*driver1_0;   speed4 = -speed*driver1_0;    }
        else if(datasensor2[0])                 {   speed1 = speed*driver1_0;   speed2 = speed*driver1_0;   speed3 = speed*driver1_0;   speed4 = speed*driver1_0;    }
        else
        {
            if(g_flag2 == 0)        {   speed1 = speed*driver1_3;   speed2 = speed*driver1_3;   speed3 = speed*driver1_3;   speed4 = speed*driver1_3;    }
            else if(g_flag2 == 3)   {   speed1 = -speed*driver1_2;   speed2 = -speed*driver1_2;   speed3 = -speed*driver1_2;   speed4 = -speed*driver1_2;    }
            else if(g_flag2 == 4)   {   speed1 = speed*driver1_2;   speed2 = speed*driver1_2;   speed3 = speed*driver1_2;   speed4 = speed*driver1_2;    }
            else if(g_flag2 == 2)   {   speed1 = -speed*driver1_1;   speed2 = -speed*driver1_1;   speed3 = -speed*driver1_1;   speed4 = -speed*driver1_1;    }
            else if(g_flag2 == 5)   {   speed1 = speed*driver1_1;   speed2 = speed*driver1_1;   speed3 = speed*driver1_1;   speed4 = speed*driver1_1;    }
            else if(g_flag2 == 1)   {   speed1 = -speed*driver1_0;   speed2 = -speed*driver1_0;   speed3 = -speed*driver1_0;   speed4 = -speed*driver1_0;    }
            else if(g_flag2 == 6)   {   speed1 = speed*driver1_0;   speed2 = speed*driver1_0;   speed3 = speed*driver1_0;   speed4 = speed*driver1_0;    }
        }
        
        motor1.speed((float)(speed1));
        motor2.speed((float)(speed2));
        motor3.speed(-(float)(speed3));
        motor4.speed(-(float)(speed4));
    }
    
        motor1.speed(-(0.35-tuning1));
    motor2.speed(-(0.35-tuning2));
    motor3.speed(-(0.35-tuning3));
    motor4.speed(-(0.35-tuning4));
        
        wait_ms(300);
        
    motor1.stop(1);
    motor2.stop(1);
    motor3.stop(1);
    motor4.stop(1);
}
            
Repository toolbox
Import into Compiler
 Export to desktop IDE
 Build repository
 Send Pull Request from here
 Following
 Clone repository to desktop: 
hg clone https://eryawarandita@os.mbed.com/teams/KRAI-2016/code/Ultimate_Hybrid_LF_v4/
Repository details
Type:	 Program
Version:	Mbed OS 2
Created:	10 Feb 2018
Imports:	 1
Forks:	 0
Commits:	 8
Dependents:	 0
Dependencies:	 6
Followers:	 17
Developer
You have been given write permission on this repository by the owner.

 Linkedin  Twitter  Facebook  YouTube  Events  Forum  Blog
Copyright © 2020 Arm Limited (or its affiliates).
Home
UX Research
Website Terms
Pelion Device Management Terms
Privacy
Cookies

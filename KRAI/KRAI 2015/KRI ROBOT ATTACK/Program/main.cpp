/**
Base 4 Nasional
 
Case Gerak
1.  Pivot Kanan
2.  Pivot Kiri
3.  Maju
4.  Mundur
5.  Serong Atas Kanan
6.  Serong Bawah Kanan
7.  Serong Atas Kiri
8.  Serong Bawah Kiri
9.  Kanan
10. Kiri
11. Stop
 
 
Bima Sahbani            EL'12
Fanny Achmad Hindrarta  EL'12
Erya Warandita          EL'13  
M. Fadhil Ginting       EL'13
Fadel Mahadika          EL'13 
**/
#include "mbed.h"
#include "JoystickPS3.h"
#include "Motor.h"
 
#define vmax 1
#define vmaxserong 0.9
#define vmaxpivot 0.7
#define ax 0.005
//#define koefperlambatan 0.8
 
// Deklarasi variabel motor
Motor motor1(PA_15, PA_13, PA_14); // pwm, fwd, rev
Motor motor2(PA_0, PC_14, PC_15); // pwm, fwd, rev
Motor motor3(PA_1, PH_1, PH_0); // pwm, fwd, rev
Motor motor4(PC_6, PC_9, PC_8); // pwm, fwd, rev
 
// Deklarasi Register Pneumatik
DigitalOut pneumatik1(PC_11);
DigitalOut pneumatik2(PD_2);
 
// Deklarasi Timer Pneumatik
Timer timer_pneu;
 
// Inisialisasi  Pin TX-RX Joystik dan PC
joysticknucleo joystick(PA_11,PA_12);
//Serial pc(USBTX,USBRX);
 
//bool perlambatan=0;
char case_ger;
bool maju=false,mundur=false,kiri=false,kanan=false,saka=false,saki=false,sbka=false,sbki=false,pivki=false,pivka=false,cw1=false,ccw1=false,cw2=false,ccw2=false,cw3=false,ccw3=false;
bool stop = true;
bool t1, t2, pneu1, pneu2;
int delay_pneumatik;
double vcurr;
 
 
int case_gerak()
{
    int casegerak;
    if (!joystick.L1 && joystick.R1) {
        // Pivot Kanan
        casegerak = 1;
    } else if (!joystick.R1 && joystick.L1) {
        // Pivot Kiri
        casegerak = 2;
    } else if (((!(joystick.L1&&joystick.R1)) && (joystick.LX>110 && joystick.LX<190) && (joystick.LY<=50)) || ((joystick.atas)&&(!joystick.bawah)&&(!joystick.kanan)&&(!joystick.kiri)) ) {  
        // Maju
        casegerak = 3; 
    } else if (((!(joystick.L1&&joystick.R1)) && (joystick.LX>90 && joystick.LX<190) && (joystick.LY>=200) )|| ((!joystick.atas)&&(joystick.bawah)&&(!joystick.kanan)&&(!joystick.kiri)))  {  
        // Mundur
        casegerak = 4;
    } else if ((!(joystick.L1&&joystick.R1)) && (((joystick.LX>=200)&&(joystick.LY<=50)) || ((joystick.atas)&&(!joystick.bawah)&&(!joystick.kiri)&&(joystick.kanan))))   {   
        // Serong Atas Kanan
        casegerak = 5;
    } else if(((!(joystick.L1&&joystick.R1)) && (((joystick.LX>=200)&&(joystick.LY>=200)) || ((!joystick.atas)&&(joystick.bawah)&&(!joystick.kiri)&&(joystick.kanan))))) {   
        // Serong Bawah Kanan
        casegerak = 6;
    } else if ((!(joystick.L1&&joystick.R1)) && (((joystick.LX<=50)&&(joystick.LY<=50)) || ((joystick.atas)&&(!joystick.bawah)&&(joystick.kiri)&&(!joystick.kanan)))) {   
        // Serong Atas Kiri
        casegerak = 7;
    } else if ((!(joystick.L1&&joystick.R1)) && (((joystick.LX<=50)&&(joystick.LY>=200)) || ((!joystick.atas)&&(joystick.bawah)&&(joystick.kiri)&&(!joystick.kanan)))) {   
        // Serong Bawah Kiri
        casegerak = 8;
    } else if (((!(joystick.L1&&joystick.R1)) && (joystick.LX>=210) && (joystick.LY>80 && joystick.LY<200))|| ((!joystick.atas)&&(!joystick.bawah)&&(joystick.kanan)&&(!joystick.kiri)) )  {   
        // Kanan
        casegerak = 9;
    } else if (((!(joystick.L1&&joystick.R1)) && (joystick.LX<=50) && (joystick.LY>80 && joystick.LY<200))|| ((!joystick.atas)&&(!joystick.bawah)&&(!joystick.kanan)&&(joystick.kiri))) {   
        // Kiri
        casegerak = 10;
    } else {
        // Stop
        casegerak = 11;
    }
 
    if(joystick.silang_click && t1==0 && t2==0) {
        pneu1 = 1;
    } else {
        pneu1 = 0;
    }
    
    if(joystick.kotak_click && t1==0 && t2==0) {
        pneu2 = 1;
    } else {
        pneu2 = 0;
    }
    return(casegerak);
}
 
 
 
/**
 
**  Case 1  : Pivot Kanan
**  Case 2  : Pivot Kiri
**  Case 3  : Maju
**  Case 4  : Mundur
**  Case 5  : Serong Atas Kanan
**  Case 6  : Serong Bawah Kanan
**  Case 7  : Serong Atas Kiri
**  Case 8  : Serong Bawah Kiri
**  Case 9  : Kanan
**  Case 10 : Kiri
**  Case 11 : Break
 
**/
void aktuator()
{
    double koef;
    double s1=0,s2=0,s3=0,s4=0;
    
    // PNEUMATIK
    if(t1==1) {
        if(timer_pneu.read_ms() - delay_pneumatik > 800) {
            pneumatik1 = 1;
            t1=0;
        }
    }
    if(t2==1) {
        if(timer_pneu.read_ms() - delay_pneumatik > 800) {
            pneumatik2 = 1;
            t2=0;
        }
    }
    
    if (pneu1 == 1 || pneu2==1) {
        timer_pneu.reset();
        delay_pneumatik = timer_pneu.read_ms();
        if(pneu1 == 1) {
            pneumatik1 = 0;
            t1 = 1;
            pneu1 = 0;
        } else if(pneu2 == 1) {
            pneumatik2 = 0;
            t2 = 1;
            pneu2 = 0;
        }
        
    }
 
    // MOTOR
    switch (case_ger) 
    {
    case (1): 
        {       
            if (pivka) {
                if(vcurr<0.1) {
                    vcurr=0.1;
                } else {
                    vcurr+=ax;
                }
                //perlambatan=0;
            } else { 
                //perlambatan=1;
            } 
            
            if (vcurr>=vmaxpivot) {
                vcurr=vmaxpivot; 
            }
            
            if(joystick.R2==255 && joystick.L2==0) {
                koef=2;
            } else if (joystick.L2==255 && joystick.R2==0) {
                koef=0.5;
            }
            else { 
                koef=1;
            }
            
            s1 = (float)(-0.5*koef*vcurr);
            s2 = (float)(0.5*koef*vcurr);
            s3 = (float)(-0.5*koef*vcurr);
            s4 = (float)(0.5*koef*vcurr);    
            
            pivka=true;         
            maju=mundur=kiri=kanan=saka=saki=sbka=sbki=pivki=cw1=ccw1=cw2=ccw2=cw3=ccw3=false;
            
            //pc.printf("pivKa\n");
            
            motor1.speed(s1);
            motor2.speed(s2);
            motor3.speed(s3);
            motor4.speed(s4);
 
            break;
        }
    case (2):
           {
            if (pivki){
                if(vcurr<0.1) {
                    vcurr=0.1;
                } else {
                    vcurr+=ax;
                }
                //perlambatan=0;
            } else { 
                //perlambatan=1;
            } 
            
            if (vcurr>=vmaxpivot) {
                vcurr=vmaxpivot; 
            }
            
            if(joystick.R2==255 && joystick.L2==0) {
                koef=2;
            } else if (joystick.L2==255 && joystick.R2==0) {
                koef=0.5;
            } else {
                koef=1;
            }
            
            s1 = (float)(0.5*koef*vcurr);
            s2 = (float)(-0.5*koef*vcurr);
            s3 = (float)(0.5*koef*vcurr); 
            s4 = (float)(-0.5*koef*vcurr);    
            
            pivki=true; 
            maju=mundur=kiri=kanan=saka=saki=sbka=sbki=pivka=cw1=ccw1=cw2=ccw2=cw3=ccw3=false;
            
            //pc.printf("pivKi\n");
            
            motor1.speed(s1);
            motor2.speed(s2);
            motor3.speed(s3);
            motor4.speed(s4);
    
            break;
           }
    case (3):
        {   
            if (maju) {
                if(vcurr<0.1) {
                    vcurr=0.1;
                } else {
                    vcurr+=ax;
                }
                //perlambatan=0;
            } else {
                //perlambatan=1;
            } 
            
            if (vcurr>=vmax) { 
                vcurr=vmax; 
            }
            
            if(joystick.R2==255 && joystick.L2==0) {
                koef=2;
            }  else if (joystick.L2==255 && joystick.R2==0)  { koef=0.5;}
            else {
                koef=1;  
            }
            
            //Case s1 untuk mode L2 lebih lambat
            s1 = (float)(-1*koef*(vcurr+0.005));
            
            s2 = (float)(1.0*koef*vcurr); 
            s3 = (float)(1.0*koef*vcurr); 
            s4 = (float)(-1*koef*vcurr);  
            
            //s1 =-0.8*koef*vcurr;
            //s2 =koef*vcurr; 
            //s3 =-koef*vcurr; 
            //s4 =koef*vcurr;    
            
            maju=true;             
            mundur=kiri=kanan=saka=saki=sbka=sbki=pivka=pivki=cw1=ccw1=cw2=ccw2=cw3=ccw3=false;
 
            //pc.printf("maju\n");
 
            motor1.speed(s1);
            motor2.speed(s2);
            motor3.speed(s3);
            motor4.speed(s4);
    
            break;
        }
    case (4):
        { 
            if (mundur) {
                if(vcurr<0.1) {
                    vcurr=0.1;
                } else {
                    vcurr+=ax;
                }
                //perlambatan=0;
            } else {
                //perlambatan=1;
            } 
            
            if (vcurr>=vmax) {
                vcurr=vmax; 
            }
            
            if(joystick.R2==255 && joystick.L2==0) { 
                koef=2;
            } else if (joystick.L2==255 && joystick.R2==0) { 
                koef=0.5;
            } else { 
                koef=1;  
            }
            //Motor 4  telat mulai
            s1 = (float)(1*koef*(vcurr-0.008));
            s2 = (float)(-1*koef*(vcurr-0.005)); 
            s3 = (float)(-1*koef*(vcurr-0.005)); 
            s4 = (float)(1*koef*(vcurr+0.005));
 
            mundur=true; 
            maju=kiri=kanan=saka=saki=sbka=sbki=pivka=pivki=cw1=ccw1=cw2=ccw2=cw3=ccw3=false;
 
            //pc.printf("mundur\n");
 
            motor1.speed(s1);
            motor2.speed(s2);
            motor3.speed(s3);
            motor4.speed(s4);
    
            break;
        }
    case (5) :
        {   
            if (saka) { 
                if(vcurr<0.1) {
                    vcurr=0.1;
                } else {
                    vcurr+=ax;
                }
                //perlambatan=0;
            } else { 
                //perlambatan=1;
            } 
            
            if (vcurr>=vmax) {
                vcurr=vmax; 
            } if(joystick.R2==255 && joystick.L2==0)  { 
                koef=2;  
            } else if (joystick.L2==255 && joystick.R2==0) { 
                koef=0.5;
            } else { 
                koef=1;  
            }
            
            s1 = (float)(-koef*vcurr);
            s2 = (float)(0);                 //koef*0.1*vcurr;
            s3 = (float)(koef*vcurr); 
            s4 = (float)(0);                 //-koef*0.1*vcurr;                
            
            saka=true; 
            maju=mundur=kiri=kanan=sbka=saki=sbki=pivka=pivki=cw1=ccw1=cw2=ccw2=cw3=ccw3=false;
            
            //pc.printf("saka\n");
            
            motor1.speed(s1);
            motor2.brake(1);
            //motor2.speed(s2);
            motor3.speed(s3);
            motor4.brake(1);
            //motor4.speed(s4);
            
            break;
        }
    case (6) :
        {   
            if (sbka){
                if(vcurr<0.1) {
                    vcurr=0.1;
                } else {
                    vcurr+=ax;
                }
                //perlambatan=0;
            } else {
                //perlambatan=1;
            } 
            
            if (vcurr>=vmaxserong) {
                vcurr=vmaxserong; 
            }
            
            if(joystick.R2==255 && joystick.L2==0) { 
                koef=2;  
            } else if (joystick.L2==255 && joystick.R2==0) { 
                koef=0.5;
            } else { 
                koef=1;  
            }
            
            s1 = (float)(0);                 //koef*0.1*vcurr;
            s2 = (float)(-koef*vcurr); 
            s3 = (float)(0);                 //-koef*0.1*vcurr;
            s4 = (float)(koef*vcurr);
            
            sbka=true; 
            maju=mundur=kiri=kanan=saka=saki=sbki=pivka=pivki=cw1=ccw1=cw2=ccw2=cw3=ccw3=false;
            
            //pc.printf("sbka\n");
 
            //motor1.speed(s1);
            motor1.brake(1);
            motor2.speed(s2);
            //motor3.speed(s3);
            motor3.brake(1);
            motor4.speed(s4);
    
            break;
        }
    case (7) :
        {   
            if (saki) {
                if(vcurr<0.1) {
                    vcurr=0.1;
                } else {
                    vcurr+=ax;
                }
                //perlambatan=0;
            } else {
                //perlambatan=1;
            } 
            
            if (vcurr>=vmaxserong) { 
                vcurr=vmaxserong; 
            }
            
            if(joystick.R2==255 && joystick.L2==0) { 
                koef=2;  
            } else if (joystick.L2==255 && joystick.R2==0)  { 
                koef=0.5;
            } else { 
                koef=1;  
            }
            
            s1 = (float)(0);                     //-koef*0.1*vcurr;
            s2 = (float)(koef*vcurr); 
            s3 = (float)(0);                     //koef*0.1*vcurr; 
            s4 = (float)(-koef*vcurr);  
            
            saki=true; 
            maju=kiri=kanan=saka=mundur=sbka=sbki=pivka=pivki=cw1=ccw1=cw2=ccw2=cw3=ccw3=false;
            
            //pc.printf("saki\n");
            
            //motor1.speed(s1);
            motor1.brake(1);
            motor2.speed(s2);
            //motor3.speed(s3);
            motor3.brake(1);
            motor4.speed(s4);
    
            break;
        }
    case (8) :
        {   
            if (sbki) {
                if(vcurr<0.1) {
                    vcurr=0.1;
                } else {
                    vcurr+=ax;
                }
                //perlambatan=0;
            } else {
                //perlambatan=1;
            } 
            
            if (vcurr>=vmaxserong) { 
                vcurr=vmaxserong; 
            }
            
            if(joystick.R2==255 && joystick.L2==0) { 
                koef=2;  
            } else if (joystick.L2==255 && joystick.R2==0)  { 
                koef=0.5;
            } else { 
                koef=1;  
            }
            
            s1 = (float)(koef*vcurr);
            s2 = (float)(0);                 //-koef*0.1*vcurr;
            s3 = (float)(-koef*vcurr); 
            s4 = (float)(0);                 //koef*0.1*vcurr;
            
            sbki=true; 
            maju=kiri=kanan=saka=saki=sbka=mundur=pivka=pivki=cw1=ccw1=cw2=ccw2=cw3=ccw3=false;
            
            //pc.printf("sbki\n");
            
            motor1.speed(s1);
            //motor2.speed(s2);
            motor2.brake(1);
            motor3.speed(s3);
            //motor4.speed(s4);
            motor4.brake(1);
    
            break;
        }
    case (9) :
        {   
            if (kanan) {
                if(vcurr<0.1) {
                    vcurr=0.1;
                } else {
                    vcurr+=ax;
                }
                //perlambatan=0;
            } else {
                //perlambatan=1;
            } 
            
            if (vcurr>=vmax) { 
                vcurr=vmax; 
            }
            
            if(joystick.R2==255 && joystick.L2==0) { 
                koef=2;  
            } else if (joystick.L2==255 && joystick.R2==0)  { 
                koef=0.5;
            } else { 
                koef=1;  
            }
            
            s1 =(float)(-1*koef*vcurr);
            s2 =(float)(-1.0*koef*vcurr); 
            s3 =(float)(1*koef*vcurr); 
            s4 =(float)(1.0*koef*vcurr);           
            
            kanan=true; 
            maju=kiri=mundur=saka=saki=sbka=sbki=pivka=pivki=cw1=ccw1=cw2=ccw2=cw3=ccw3=false;
            
            //pc.printf("Kanan\n");
            
            motor1.speed(s1);
            motor2.speed(s2);
            motor3.speed(s3);
            motor4.speed(s4);
            break;
        }
    case (10) :
        {   
            if (kiri) { 
                if(vcurr<0.1) {
                    vcurr=0.1;
                } else {
                    vcurr+=ax;
                }
                //perlambatan=1;
            } else {
                //perlambatan=1;
            } 
            
            if (vcurr>=vmax) {
                vcurr=vmax; 
            }
 
            if(joystick.R2==255 && joystick.L2==0)  { 
                koef=2;  
            } else if (joystick.L2==255 && joystick.R2==0)  { 
                koef=0.5;
            } else { 
                koef=1;  
            }
            
            s1 =(float)(1*koef*vcurr);
            s2 =(float)(1*koef*vcurr); 
            s3 =(float)(-1*koef*vcurr); 
            s4 =(float)(-1.0*koef*vcurr);
            
            kiri=true; 
            maju=kanan=mundur=saka=saki=sbka=sbki=pivka=pivki=cw1=ccw1=cw2=ccw2=cw3=ccw3=false;
            
            //pc.printf("Kiri\n");
            
            motor1.speed(s1);
            motor2.speed(s2);
            motor3.speed(s3);
            motor4.speed(s4);
              
            break;
        }
    default :
        {
            //if (mundur||kiri||kanan||saka||saki||sbka||sbki||pivki||pivka||cw1||ccw1||cw2||ccw2||cw3||ccw3) wait_ms(100);
            //if (maju && (vcurr>=0.5)) wait_ms(100); 
            //else if (maju && (vcurr<0.5)) wait_ms(50);  
            /*      
            if(s1>0.2 || s1<-0.2 || s2>0.2 || s2<-0.2) {
                s1 = koefperlambatan * s1;
                s2 = koefperlambatan * s2;
                s3 = koefperlambatan * s3;
                s4 = koefperlambatan * s4;
 
                motor1.speed(s1);
                motor2.speed(s2);
                motor3.speed(s3);
                motor4.speed(s4);   
                
                
            } else {
            */
                motor1.brake(1);
                motor2.brake(1);
                motor3.brake(1);
                motor4.brake(1);
            //}
 
            maju=mundur=kiri=kanan=saka=saki=sbka=sbki=pivki=pivka=cw1=ccw1=cw2=ccw2=cw3=ccw3=false;
            stop = true;
 
            //s1 = 0;s2 =0; s3 =0; s4 =0;
 
            //pc.printf("Stop\n");
        }
    }
}
 
 
int main (void)
{
    // Set baud rate - 115200
    joystick.setup();
    //pc.baud(115200);
    //pc.printf("Ready...\n");
    timer_pneu.start();
    pneumatik1=1;
    pneumatik2=1;
    t1=0;
    t2=0;
    while(1)
    {
        // Interrupt Serial
        joystick.idle();
       if(joystick.readable() ) {
            // Panggil fungsi pembacaan joystik
            joystick.baca_data();
            // Panggil fungsi pengolahan data joystik
            joystick.olah_data();
            //pc.printf("%2x %2x %2x %2x %3d %3d %3d %3d %3d %3d\n\r",joystick.button, joystick.RL, joystick.button_click, joystick.RL_click, joystick.R2, joystick.L2, joystick.RX, joystick.RY, joystick.LX, joystick.LY);
            case_ger = case_gerak();
            aktuator();  
 
        } else {
            joystick.idle();
            
        }                           
    }
}
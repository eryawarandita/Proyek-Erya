switch (state_lcd)
{
        case LCD_PATH_ROOT:
            sprintf(str1,"%d. Atur Path", state_lcd);
            sprintf(str2,"    <Masuk>     ");
            set_lcd(0,LCD_TAMPIL_SENSOR,-1, LCD_SET_PATH);
            break;
        case LCD_TAMPIL_SENSOR:
            sprintf(str1,"%d. Data Sensor",state_lcd);
            sprintf(str2,"  %d%d%d%d%d%d%d%d%d%d%d%d  ",
                (sensor>>11)&1,(sensor>>10)&1,(sensor>>9)&1,(sensor>>8)&1,(sensor>>7)&1,(sensor>>6)&1,
                (sensor>>5)&1,(sensor>>4)&1,(sensor>>3)&1,(sensor>>2)&1,(sensor>>1)&1,(sensor>>0)&1
            );     
            set_lcd(LCD_PATH_ROOT,LCD_KALIBRASI,-1,-1);
            break;
        case LCD_KALIBRASI:
            sprintf(str1,"%d. Kalibrasi",state_lcd);
            sprintf(str2,"     <Auto>     ");
            set_lcd(LCD_TAMPIL_SENSOR,LCD_ADC_SENSOR_1,-1,-1); 
            if(S4==0){
                LCD=0;
                while(S4==0);
                LCD=1;
                delay_ms(100);
                LCD=0;
                delay_ms(100);
                LCD=1;
                delay_ms(100);
                LCD=0;
                delay_ms(100);
                LCD=1;
                mode=NAV_KALIBRASI;
            } 
            break;
        case LCD_ADC_SENSOR_1:
            sprintf(str1,"%d. %3d %3d",state_lcd,adc_sensor[0],adc_sensor[1]);
            sprintf(str2,"%3d %3d %3d %3d",adc_sensor[2],adc_sensor[3],adc_sensor[4],adc_sensor[5]);
            set_lcd(LCD_KALIBRASI,LCD_ADC_SENSOR_2,-1,-1);
            break;
        case LCD_ADC_SENSOR_2:
            sprintf(str1,"%d. %3d %3d",state_lcd,adc_sensor[6],adc_sensor[7]);
            sprintf(str2,"%3d %3d %3d %3d",adc_sensor[8],adc_sensor[9],adc_sensor[10],adc_sensor[11]);
            set_lcd(LCD_ADC_SENSOR_1,LCD_PID,-1,-1);
            break; 
        case LCD_PID:
            sprintf(str1,"Atur Parameter", state_lcd);
            sprintf(str2,"    <Masuk>     ");
            set_lcd(LCD_ADC_SENSOR_2,0,-1,LCD_SET_P);
            break;
        case LCD_SET_PATH:
            if(set){
                sprintf(str1, " Simpangan %d:", inputpath_counter);
                sprintf(str2, "  > '%c'", arrow[indexarrow]);
                if (!S1){
                    while(!S1);
                    indexarrow+=5;
                    indexarrow=(indexarrow-1)%5;
                }
                else if (!S2){
                    while(!S2);
                    indexarrow=(indexarrow+1)%5;
                }
                if (!S4){
                    pathcust[inputpath_counter]=arrow[indexarrow];
                    while(!S4);
                    inputpath_counter=(inputpath_counter+1)%16;
                } 
                if(!S3){
                    while(!S3);
                    set=0;
                    inputpath_counter=0;
                    indexarrow=0;
                }
            }
            else{
                sprintf(str1, " Simpangan %d:", inputpath_counter);
                sprintf(str2, "    '%c'", pathcust[inputpath_counter]);
                set_lcd(-1,-1,LCD_PATH_ROOT,-1);
                if (!S1){
                    while(!S1);
                    inputpath_counter+=16;
                    inputpath_counter=(inputpath_counter-1)%16;
                }
                else if (!S2){
                    while(!S2);
                    inputpath_counter=(inputpath_counter+1)%16;
                }
                if(!S4){
                    while(!S4);
                    if (pathcust[inputpath_counter]=='^'){
                        indexarrow=0;
                    }else if(pathcust[inputpath_counter]=='<'){
                        indexarrow=1;
                    }else if(pathcust[inputpath_counter]=='>'){
                        indexarrow=2;
                    }else if(pathcust[inputpath_counter]=='!'){
                        indexarrow=3;
                    }else {
                        indexarrow=4;
                    }
                    set=1;
                }
            }
            break;
        case LCD_SET_P:
            sprintf(str1," P Gain:");
            if(set){
                sprintf(str2,"  > %d     ",P_gain);
                if(!S2){
                    delay_ms(50);
                    P_gain++;
                }else if(!S1){
                    delay_ms(50);
                    P_gain--;
                }
                if(!S4){
                    while(!S4);
                    set=0;
                }
            }else{
                sprintf(str2,"    %d     ",P_gain);
                set_lcd(LCD_SET_ROTSPEED,LCD_SET_I,LCD_PID,-1);
                if(!S4){
                    while(!S4);
                    set=1;
                }
            }
            break;
        case LCD_SET_I:
            sprintf(str1," I Gain:");
            if(set){
                sprintf(str2,"  > %d     ",I_gain);
                if(!S2){
                    delay_ms(50);
                    I_gain++;
                }else if(!S1){
                    delay_ms(50);
                    I_gain--;
                }
                if(!S4){
                    while(!S4);
                    set=0;
                }
            }else{
                sprintf(str2,"    %d     ",I_gain);
                set_lcd(LCD_SET_P,LCD_SET_D,LCD_PID,-1);
                if(!S4){
                    while(!S4);
                    set=1;
                }
            }
            break;
        case LCD_SET_D:
            sprintf(str1," D Gain:");
            if(set){
                sprintf(str2,"  > %d     ",D_gain);
                if(!S2){
                    delay_ms(50);
                    D_gain++;
                }else if(!S1){
                    delay_ms(50);
                    D_gain--;
                }
                if(!S4){
                    while(!S4);
                    set=0;
                }
            }else{
                sprintf(str2,"    %d     ",D_gain);
                set_lcd(LCD_SET_I,LCD_SET_SPEED,LCD_PID,-1);
                if(!S4){
                    while(!S4);
                    set=1;
                }
            }
            break;     
        case LCD_SET_SPEED:
            sprintf(str1," Kecepatan:");
            if(set){
                sprintf(str2,"  > %d     ",throttle);
                if(!S2){
                    delay_ms(50);
                    throttle++;
                }else if(!S1){
                    delay_ms(50);
                    throttle--;
                }
                if(!S4){
                    while(!S4);
                    set=0;
                }
            }else{
                sprintf(str2,"    %d     ",throttle);
                set_lcd(LCD_SET_D,LCD_SET_ROTSPEED,LCD_PID,-1);
                if(!S4){
                    while(!S4);
                    set=1;
                }
            }
            break; 
        case LCD_SET_ROTSPEED:
            sprintf(str1," Kec pivot:");
            if(set){
                sprintf(str2,"  > %d     ",throttle_r);
                if(!S2){
                    delay_ms(50);
                    throttle_r++;
                }else if(!S1){
                    delay_ms(50);
                    throttle_r--;
                }
                if(!S4){
                    while(!S4);
                    set=0;
                }
            }else{
                sprintf(str2,"    %d     ",throttle_r);
                set_lcd(LCD_SET_SPEED,LCD_SET_P,LCD_PID,-1);
                if(!S4){
                    while(!S4);
                    set=1;
                }
            }
            break;
        default:
            sprintf(str1,"  >> ASFARI <<  ");
            //sprintf(str2,"Menu       Start");
            if(set){
                sprintf(str2," > Rute: %3d",checkpoint);
                if(!S2){
                    while(!S2);
                    checkpoint++;
                }else if(!S1){
                    while(!S1);
                    checkpoint--;
                }
                if(!S4){
                    LCD=0;
                    while(S4==0);
                    LCD=1;
                    delay_ms(70);
                    LCD=0;
                    delay_ms(70);
                    LCD=1;
                    delay_ms(70);
                    LCD=0;
                    delay_ms(70);
                    LCD=1;    
                    jalan=1;
                    set=0;            
                    mode=NAV_JALAN_PID;
                }else if(!S3){
                    while(!S3);
                    set=0;
                }
            }else{
                sprintf(str2,"   Rute: %3d",checkpoint);
                set_lcd(LCD_PID,LCD_PATH_ROOT,-1,-1);
                if(!S3){
                    while(!S3);
                    jalan=0;
                }
                if(!S4){
                    while(!S4);
                    set=1;
                }
            }
            
            break;
      }
      lcd_clear();
      lcd_gotoxy(0,0);      
      lcd_puts(str1);
      lcd_gotoxy(0,1);      
      lcd_puts(str2);
      delay_ms(5);
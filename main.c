#include <LPC21xx.h>

#include "lcd.h"
#include "lcd_defines.h"
#include "delays.h"
#include "rtc.h"
#include "keypad.h"
#include "alarm.h"
#include "password.h"

#include "adc2.h"
// --------------------------------------------------
// SWITCH
// --------------------------------------------------
#define SW1 (1 << 7)
int i,j;
// --------------------------------------------------
// MODES
// --------------------------------------------------
#define MODE_RTC      0
#define MODE_MENU     1
#define MODE_ALARM    2
#define MODE_PWD      3

// --------------------------------------------------
// GLOBAL VARIABLES
// --------------------------------------------------
u8 mode = MODE_RTC;
u8 prev_mode = 255;

u8 key = 0;

//ADC VARIABLES
u16 adc_val = 0;
f32 voltage = 0.0;
f32 current_temp = 0.0;

// RTC VARIABLES
s32 hour, min, sec;
s32 date, month, year;
s32 day;

s32 prev_sec = -1;

// SWITCH VARIABLES
u8 prev_sw = 0;
u8 curr_sw = 0;

// ALARMS
u32 alarm1 = 0;
u32 alarm2 = 0;

// GLOBAL ALARM FLAG
extern u8 g_alarm_active;

//menu variables
u8 menu_start_sec=0;
u8 menu_timer_started=0;

// --------------------------------------------------
// MAIN
// --------------------------------------------------
int main(void)
{
    // ----------------------------------------------
    // SWITCH INPUTS
    // ----------------------------------------------
    IODIR0 &= ~SW1;
    IODIR0 &= ~SW_ALARM;

    // ----------------------------------------------
    // INITIALIZE MODULES
    // ----------------------------------------------
    InitLCD();
    
    //VPBDIV=0X02;

    RTC_Init();

    InitKPM();

    Alarm_Init();

    Password_Init();
    
 
        //PCONP|=(1<<12);
        
        Init_ADC();

    // ----------------------------------------------
    // SPLASH SCREEN
    // ----------------------------------------------
    CmdLCD(CLEAR_LCD);

    CmdLCD(GOTO_LINE1_POS0);
    StrLCD("EnviroTime");

    CmdLCD(GOTO_LINE2_POS0);
    StrLCD("Initializing");
        
        
        CmdLCD(GOTO_LINE2_POS0+13);
        
        for(i=0;i<3;i++)
        {
            for(j=0;j<i;j++)
            {    
                CharLCD('.');
                delay_ms(500);
            }
        }
            

    delay_ms(500);

    CmdLCD(CLEAR_LCD);

   

    // ----------------------------------------------
    // MAIN LOOP
    // ----------------------------------------------
    while(1)
    {
        // ==========================================
        // ALWAYS RUN ALARM TASK
        // ==========================================
        Alarm_Task();

        // ==========================================
        // IF ALARM ACTIVE
        // ==========================================
        if(g_alarm_active)
        {
            continue;
        }
                //prev_mode=255;
        // ==========================================
        // SWITCH EDGE DETECTION
        // ==========================================
        curr_sw = (IOPIN0 & SW1) ? 1 : 0;

        if(curr_sw && !prev_sw)
        {
            delay_ms(20);

            if(IOPIN0 & SW1)
            {
                                //wait for release
                                while(IOPIN0&SW1);
                if(mode == MODE_RTC)
                {
                    mode = MODE_MENU;
                   
                }
                else
                {        menu_timer_started=0;
                    mode = MODE_RTC;
                                        prev_sec=-1;
                    
                }
                                prev_mode=255;
                                CmdLCD(CLEAR_LCD);
            }
        }

        prev_sw = curr_sw;

        // ==========================================
        // MODE : RTC DISPLAY
        // ==========================================
        if(mode == MODE_RTC)
        {
            GetRTCTimeInfo(&hour, &min, &sec);

            GetRTCDay(&day);

            if((sec != prev_sec)||(prev_sec==-1))
                    {
                        prev_sec = sec;

                        // CLEAR BOTH LINES FIRST
                        

                        // TIME
                        DisplayRTCTime(hour,min,sec);

            
                        // -------- TEMPERATURE --------
                        Read_ADC(1, &adc_val, &voltage);

                        current_temp = voltage * 100.0;

                        CmdLCD(0x80 + 9);

                        F32LCD(current_temp,1);

                        CharLCD(0xDF); // degree symbol
                        CharLCD('C');
                                            
                        // DATE
                        GetRTCDateInfo(&date,&month,&year);

                        DisplayRTCDate(date,month,year);

                        // DAY
                        CmdLCD(GOTO_LINE2_POS0 + 12);

                    DisplayRTCDay(day);
}
        }

        // ==========================================
        // MODE : MENU
        // ==========================================
        else if(mode == MODE_MENU)
        {
            if(prev_mode != MODE_MENU)
            {
                            if(menu_timer_started==0)
                            {
                                menu_start_sec=SEC;
                                menu_timer_started=1;
                                }

                                /* 10 sec timeout*/
                                if(((SEC-menu_start_sec+60)%60)>=10)
                                {
                                mode=MODE_RTC;
                                prev_mode=255;
                                menu_timer_started=0;
                                CmdLCD(CLEAR_LCD);
                                prev_sec=-1;
                                continue;
                        }
                CmdLCD(CLEAR_LCD);

                            LCD_Print(GOTO_LINE1_POS0, "1:RTC 2:ALARM");

                            LCD_Print(GOTO_LINE2_POS0, "3:PWD 4:EXIT");

                prev_mode = MODE_MENU;
            }

            key = KeyScan();

            if(key != 0)
            {        menu_start_sec=SEC;
                delay_ms(20);

                while(KeyScan() != 0);
                //{
                  //  Alarm_Task();
                //}

                // RTC EDIT
                if(key == '1')
                {
                    if(Password_Verify())
                    {
                        RTC_Edit();
                                                menu_timer_started=0;
                        mode = MODE_RTC;
                        prev_mode = 255;

                        CmdLCD(CLEAR_LCD);
                    }
                }

                // ALARM MENU
                else if(key == '2')
                {
                    if(Password_Verify())
                    {
                        mode = MODE_ALARM;
                        prev_mode = 255;

                        CmdLCD(CLEAR_LCD);
                    }
                }

                // PASSWORD CHANGE
                else if(key == '3')
                {
                    Password_Change();

                    prev_mode = 255;

                    CmdLCD(CLEAR_LCD);
                }

                // EXIT
                else if(key == '4')
                {        menu_timer_started=0;
                    mode = MODE_RTC;
                    prev_mode = 255;

                    CmdLCD(CLEAR_LCD);
                }
            }
        }

        // ==========================================
        // MODE : ALARM
        // ==========================================
        else if(mode == MODE_ALARM)
        {
            Alarm_Menu(&alarm1, &alarm2);

            mode = MODE_MENU;

            prev_mode = 255;

            CmdLCD(CLEAR_LCD);
        }

        // ==========================================
        // MODE : PASSWORD
        // ==========================================
        else if(mode == MODE_PWD)
        {
            if(prev_mode != MODE_PWD)
            {
                CmdLCD(CLEAR_LCD);

                CmdLCD(GOTO_LINE1_POS0);
                StrLCD("PASSWORD");

                CmdLCD(GOTO_LINE2_POS0);
                StrLCD("SW:BACK");

                prev_mode = MODE_PWD;
            }
        }
    }
}

#include <LPC21xx.h>
#include<stdio.h>
#include "alarm.h"
#include "lcd.h"
#include "rtc.h"
#include "delays.h"
#include "lcd_defines.h"
#include "keypad.h"
#include<string.h>
#define MODE_RTC      0

u8 g_alarm_active = 0;

u8 pos = 0;

u32 snooze_time=0XFFFF;
u8 snooze_active=0;
u8 snooze_count=0;

extern u32 alarm1;
extern u32 alarm2;

extern u8 mode;
extern u8 prev_mode;

extern s32 prev_sec;

void Alarm_Init(void)
{
		PINSEL0&= ~(3<<2);
	
    PINSEL1&=~(3<<18);
    
    IODIR0 |= BUZZER;

    IODIR0 &= ~SW_ALARM;

    IOCLR0 = BUZZER;
}

void Alarm_Menu(u32 *a1, u32 *a2)
{
    char key;
    char line1[17];
                u8 a1h,a1m,a2h,a2m;

    while(1)
    {
        ClearLineLCD(1);
        ClearLineLCD(2);
                a1h=(*a1)/60;
                a1m=(*a1)%60;
                
                a2h=(*a2)/60;
                a2m=(*a2)%60;
                
                sprintf(line1,"A1%02d:%02d A2%02d:%02d",a1h,a1m,a2h,a2m);
                
            
                LCD_Print(GOTO_LINE1_POS0,line1);
            
        LCD_Print(GOTO_LINE2_POS0,"1:A1 2:A2");

        LCD_Print(GOTO_LINE2_POS0+10,"3:BACK");

        key = GetKey();

        if(key == '3')
        {
            CmdLCD(CLEAR_LCD);
            return;
        }

        if(key == '1' || key == '2')
        {
            char sel=key;
            char buf[6];
                        strcpy(buf,"00:00");
                        CmdLCD(CLEAR_LCD);
                     LCD_Print(GOTO_LINE1_POS0,(sel=='1') ? "SET ALARM1     ": "SET ALARM     ");

           LCD_Print(GOTO_LINE2_POS0,buf);
                        CmdLCD(GOTO_LINE2_POS0+pos);
                        CmdLCD(0X0E);
                    
            while(1)
            {
               
                key = GetKey();

                if(key == '+')
                {
                    if(pos < 4)
                    {
                        pos++;

                        if(pos == 2)
                            pos++;
                    }
                                        CmdLCD(GOTO_LINE2_POS0+pos);
                }

                else if(key == '-')
                {
                    if(pos > 0)
                    {
                        pos--;

                        if(pos == 2)
                            pos--;
                    }
                                        CmdLCD(GOTO_LINE2_POS0+pos);
                }

                else if(key == 'C')
                {
                    strcpy(buf,"00:00");
                                        LCD_Print(GOTO_LINE2_POS0,buf);
                                    pos=0;
                                    CmdLCD(GOTO_LINE2_POS0+pos);
                }

                else if(key == '=')
                {
                    u8 hh;
                    u8 mm;

                    hh=((buf[0]-'0')*10)+(buf[1]-'0');

                    mm=((buf[3]-'0')*10)+(buf[4]-'0');

                    if(hh > 23 || mm > 59)
                    {
                        CmdLCD(CLEAR_LCD);
                        LCD_Print(GOTO_LINE1_POS0,
                        "INVALID TIME");

                        delay_ms(1000);
                                                CmdLCD(CLEAR_LCD);
                                                LCD_Print(GOTO_LINE1_POS0,(sel=='1')?"SET ALARM1":"SET ALARM2");
                        LCD_Print(GOTO_LINE2_POS0,buf);
                                                CmdLCD(GOTO_LINE2_POS0+pos);
                                                continue;
                    }

                    if(sel=='1')
                        *a1 = (hh*60)+mm;
                    else
                        *a2 = (hh*60)+mm;

                    CmdLCD(0x0C);

                    CmdLCD(CLEAR_LCD);

                    LCD_Print(GOTO_LINE1_POS0,
                    "ALARM SAVED");

                    delay_ms(1000);

                    break;
                }

                else if(key >= '0' && key <= '9')
                {
                    buf[pos] = key;
                                        buf[5]='\0';
                                        LCD_Print(GOTO_LINE2_POS0,buf);
                                        CmdLCD(GOTO_LINE2_POS0+pos);

                }
            }
        }
    }
}

void Alarm_Task(void)
{
    s32 hr,min,sec;

    static u8 alarm_ack = 0;
        static u8 alarm_screen=0;

    static u32 last_alarm_time = 0xFFFF;

    u32 cur;

    GetRTCTimeInfo(&hr,&min,&sec);

    cur = (hr*60)+min;//works for minutes snooze

    if(cur != last_alarm_time)
    {
        alarm_ack = 0;
        
    }

    if(
    (g_alarm_active == 0)
            &&
            (
            (
        ((cur == alarm1) || (cur == alarm2))
        &&
        (alarm_ack == 0)
    )
    ||
    (
        (cur == snooze_time)
        &&
        (snooze_active)
        &&
        (sec == 0)
        &&
        (alarm_ack == 0)
                &&(g_alarm_active==0)
    )
    )
)
{
    g_alarm_active = 1;
        alarm_screen=0;
        snooze_active = 0;
    last_alarm_time = cur;

}

    if(g_alarm_active)
    {
            
                char abuf[17];
                u32 atime;
                u8 ah,am;
                
                static u8 alarm_counter=0;
                static s32 last_sec=-1;
                
                if(cur==alarm1)
                    atime=alarm1;
                else if(cur==alarm2)
                    atime=alarm2;
                else
                    atime=snooze_time;
                
                ah=atime/60;
                am=atime%60;
        IOSET0 = BUZZER;
                if(alarm_screen==0)
                {
                
                CmdLCD(CLEAR_LCD);
        LCD_Print(GOTO_LINE1_POS0,
        "**** ALARM ****");
                

                sprintf(abuf,"ALARM %02d:%02d",ah,am);
        LCD_Print(GOTO_LINE2_POS0,abuf);
                alarm_screen=1;
                }
                if(last_sec!=sec)
                {
                last_sec=sec;
                alarm_counter++;
                }
                if(alarm_counter>=10)
                {
                g_alarm_active=0;
                alarm_counter=0;
                snooze_time=cur+1;

                if(snooze_time>=1440)
                {
                snooze_time-=1440;
                }
                snooze_active=1;
                snooze_count++;
                alarm_ack=1;
                IOCLR0=BUZZER;
                CmdLCD(CLEAR_LCD);

                LCD_Print(GOTO_LINE1_POS0,"AUTO SNOOZE");
                delay_ms(500);
                CmdLCD(CLEAR_LCD);
                prev_sec=-1;
                prev_mode=255;
                mode=MODE_RTC;
                return;
                }
                
        if(IOPIN0 & SW_ALARM)
        {
            delay_ms(20);

            if(IOPIN0 & SW_ALARM)
            {
                IOCLR0 = BUZZER;

                g_alarm_active = 0;
                                alarm_screen=0;
                            
                alarm_ack = 1;

                

                CmdLCD(CLEAR_LCD);

                prev_sec = -1;
            }
        }
    }
    else
    {
        IOCLR0 = BUZZER;
    }
}

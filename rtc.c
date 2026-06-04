#include "rtc.h"
#include "lcd.h"
#include "lcd_defines.h"
#include "keypad.h"
#include "delays.h"
#include "alarm.h"
#include "input2.h"

// --------------------------------------------------
// RTC MACROS
// --------------------------------------------------
#define RTC_ENABLE (1<<0)
#define RTC_RESET  (1<<1)
//#define RTC_CLKSRC (1<<4)

// --------------------------------------------------
// DAYS
// --------------------------------------------------
char     week[][4] =
{
    "SUN",
    "MON",
    "TUE",
    "WED",
    "THU",
    "FRI",
    "SAT"
};

// --------------------------------------------------
// RTC INIT
// --------------------------------------------------
void RTC_Init(void)
{

        PCONP|=(1<<9);
        
				//If RTC already running don't reset it
				if((CCR&0X01)==0)
				{
					
        CCR=0X02;//RESET RTC
        
				PREINT=0X000001C8;
        PREFRAC=0X000061C0;

        // SET ONLY FIRST TIME
        HOUR  = 01;
        MIN   = 49;
        SEC   = 35;

        DOM   = 04;
        MONTH = 06;
        YEAR  = 2026;

        DOW   = 4;
        CCR=0X01;
    }
	}

// --------------------------------------------------
// GET TIME
// --------------------------------------------------
void GetRTCTimeInfo(s32 *h, s32 *m, s32 *s)
{
    *h = HOUR;
    *m = MIN;
    *s = SEC;
}

// --------------------------------------------------
// DISPLAY TIME
// --------------------------------------------------
void DisplayRTCTime(u32 h, u32 m, u32 s)
{
    CmdLCD(GOTO_LINE1_POS0);

    CharLCD((h/10)+'0');
    CharLCD((h%10)+'0');
    CharLCD(':');

    CharLCD((m/10)+'0');
    CharLCD((m%10)+'0');
    CharLCD(':');

    CharLCD((s/10)+'0');
    CharLCD((s%10)+'0');
}

// --------------------------------------------------
// GET DATE
// --------------------------------------------------
void GetRTCDateInfo(s32 *d, s32 *m, s32 *y)
{
    *d = DOM;
    *m = MONTH;
    *y = YEAR;
}

// --------------------------------------------------
// DISPLAY DATE
// --------------------------------------------------
void DisplayRTCDate(u32 d, u32 m, u32 y)
{
    CmdLCD(GOTO_LINE2_POS0);

    CharLCD((d/10)+'0');
    CharLCD((d%10)+'0');
    CharLCD('/');

    CharLCD((m/10)+'0');
    CharLCD((m%10)+'0');
    CharLCD('/');

    U32LCD(y);
}

// --------------------------------------------------
// SET TIME
// --------------------------------------------------
void SetRTCTimeInfo(u32 h, u32 m, u32 s)
{
    HOUR = h;
    MIN  = m;
    SEC  = s;
}

// --------------------------------------------------
// SET DATE
// --------------------------------------------------
void SetRTCDateInfo(u32 d, u32 m, u32 y)
{
    DOM   = d;
    MONTH = m;
    YEAR  = y;
}

// --------------------------------------------------
// GET DAY
// --------------------------------------------------
void GetRTCDay(s32 *d)
{
    *d = DOW;
}

// --------------------------------------------------
// DISPLAY DAY
// --------------------------------------------------
void DisplayRTCDay(u32 d)
{
    CmdLCD(GOTO_LINE2_POS0 + 11);

    StrLCD(week[d]);
}

// --------------------------------------------------
// SET DAY
// --------------------------------------------------
void SetRTCDay(u32 d)
{
    DOW = d;
}

// --------------------------------------------------
// READ 2 DIGITS
// --------------------------------------------------

// --------------------------------------------------
// RTC EDIT
// --------------------------------------------------
void RTC_Edit(void)
{
    char key;

    char timeStr[9];
    char dateStr[11];
    char dayStr[2];

    u8 positions[] =
    {
        0,1,3,4,6,7,
        8,9,11,12,14,15,16,17,
        19
    };

    u8 index = 0;

    timeStr[0]=(HOUR/10)+'0';
    timeStr[1]=(HOUR%10)+'0';
    timeStr[2]=':';
    timeStr[3]=(MIN/10)+'0';
    timeStr[4]=(MIN%10)+'0';
    timeStr[5]=':';
    timeStr[6]=(SEC/10)+'0';
    timeStr[7]=(SEC%10)+'0';
    timeStr[8]='\0';

    dateStr[0]=(DOM/10)+'0';
    dateStr[1]=(DOM%10)+'0';
    dateStr[2]='/';
    dateStr[3]=(MONTH/10)+'0';
    dateStr[4]=(MONTH%10)+'0';
    dateStr[5]='/';

    dateStr[6]=((YEAR/1000)%10)+'0';
    dateStr[7]=((YEAR/100)%10)+'0';
    dateStr[8]=((YEAR/10)%10)+'0';
    dateStr[9]=(YEAR%10)+'0';

    dateStr[10]='\0';

    dayStr[0]=DOW+'0';
    dayStr[1]='\0';

    CmdLCD(CLEAR_LCD);

    LCD_Print(GOTO_LINE1_POS0,timeStr);

    LCD_Print(GOTO_LINE2_POS0,dateStr);

    LCD_Print(GOTO_LINE2_POS0+12,"D:");

    LCD_Print(GOTO_LINE2_POS0+14,dayStr);

    CmdLCD(0x0F);

    while(1)
    {
        u8 p = positions[index];

        if(p <= 7)
            CmdLCD(GOTO_LINE1_POS0+p);
        else if(p <= 17)
            CmdLCD(GOTO_LINE2_POS0+(p-8));
        else
            CmdLCD(GOTO_LINE2_POS0+14);

        key = GetKey();

        if(key == '+')
        {
            if(index < 14)
                index++;
        }

        else if(key == '-')
        {
            if(index > 0)
                index--;
        }

        else if(key == 'C')
        {
            timeStr[0]='0';
            timeStr[1]='0';
            timeStr[3]='0';
            timeStr[4]='0';
            timeStr[6]='0';
            timeStr[7]='0';

            dateStr[0]='0';
            dateStr[1]='0';
            dateStr[3]='0';
            dateStr[4]='0';
            dateStr[6]='0';
            dateStr[7]='0';
            dateStr[8]='0';
            dateStr[9]='0';

            dayStr[0]='0';

            LCD_Print(GOTO_LINE1_POS0,timeStr);
            LCD_Print(GOTO_LINE2_POS0,dateStr);
            LCD_Print(GOTO_LINE2_POS0+14,dayStr);

            index = 0;
        }

        else if(key == '=')
        {
            u8 hh,mm,ss,dd,mon,day;
            u16 yy;

            hh=((timeStr[0]-'0')*10)+(timeStr[1]-'0');
            mm=((timeStr[3]-'0')*10)+(timeStr[4]-'0');
            ss=((timeStr[6]-'0')*10)+(timeStr[7]-'0');

            dd=((dateStr[0]-'0')*10)+(dateStr[1]-'0');
            mon=((dateStr[3]-'0')*10)+(dateStr[4]-'0');

            yy=((dateStr[6]-'0')*1000)+
               ((dateStr[7]-'0')*100)+
               ((dateStr[8]-'0')*10)+
               (dateStr[9]-'0');

            day=dayStr[0]-'0';

            if((hh>23 || mm>59||ss>59|| (dd<1||dd>31)||(mon<1||mon>12)||(day>6))||(yy<2000 ||yy>2099))
						{
							IOSET0=BUZZER;
							CmdLCD(CLEAR_LCD);
							LCD_Print(GOTO_LINE1_POS0,"INVALID DATA");
							delay_ms(1000);
							IOCLR0=BUZZER;
							CmdLCD(CLEAR_LCD);
							
							LCD_Print(GOTO_LINE1_POS0,timeStr);
							LCD_Print(GOTO_LINE2_POS0,dateStr);
							LCD_Print(GOTO_LINE2_POS0+12,"D:");
							LCD_Print(GOTO_LINE2_POS0+14,dayStr);
							
							continue;
						}

            SetRTCTimeInfo(hh,mm,ss);

            SetRTCDateInfo(dd,mon,yy);

            SetRTCDay(day);

            CmdLCD(0x0C);

            CmdLCD(CLEAR_LCD);

            LCD_Print(GOTO_LINE1_POS0,
            "RTC UPDATED");

            delay_ms(1000);

            return;
        }

        else if(key>='0' && key<='9')
        {
            u8 p = positions[index];

            if(p <= 7)
            {
                timeStr[p]=key;

                //LCD_Print(GOTO_LINE1_POS0,timeStr);
                CmdLCD(GOTO_LINE1_POS0+p);
                CharLCD(key);
            }
            else if(p <= 17)
            {
                dateStr[p-8]=key;

                //LCD_Print(GOTO_LINE2_POS0,dateStr);
                CmdLCD(GOTO_LINE2_POS0+(p-8));
                CharLCD(key);
            }
            else
            {
                
                    dayStr[0]=key;

                    //LCD_Print(GOTO_LINE2_POS0+14,dayStr);
                    CmdLCD(GOTO_LINE2_POS0+14);
                    CharLCD(key);
                
                
            }
        }
    }
}

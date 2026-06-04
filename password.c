#include "input2.h"
#include <LPC21xx.h>

#include "password.h"
#include "lcd.h"
#include "lcd_defines.h"
#include "keypad.h"
#include "delays.h"
#include "alarm.h"

static u8 stored_pass[PASS_LENGTH] = {'1','1','1','1'};

static u8 entered_pass[PASS_LENGTH];

static u8 attempts = 0;

// --------------------------------------------------
void Password_Init(void)
{
    attempts = 0;
}

// --------------------------------------------------
static void GetPassword(u8 *pass)
{
    u8 idx = 0;

    char key;

    CmdLCD(CLEAR_LCD);

    LCD_Print(GOTO_LINE1_POS0,"ENTER PASS");

    CmdLCD(GOTO_LINE2_POS0);

    while(1)
    {
        key = GetKey();

        // DIGIT
        if((key >= '0') && (key <= '9'))
        {
            if(idx < PASS_LENGTH)
            {
                pass[idx++] = key;

                CharLCD('*');
            }
        }

        // CLEAR LAST DIGIT
        else if(key == 'C')
        {
            if(idx > 0)
            {
                idx--;

                CmdLCD(GOTO_LINE2_POS0);

                LCD_ShowStars(idx);

                CharLCD(' ');

                CmdLCD(GOTO_LINE2_POS0 + idx);
            }
        }

        // SUBMIT
        else if(key == '=')
        {
            if(idx == PASS_LENGTH)
            {
                return;
            }
        }
    }
}
// --------------------------------------------------
u8 Password_Verify(void)
{
    u8 i;
    u8 remaining;

    while(attempts < 3)
    {
        GetPassword(entered_pass);

        for(i = 0; i < PASS_LENGTH; i++)
        {
            if(entered_pass[i] != stored_pass[i])
            {
                break;
            }
        }

        // CORRECT PASSWORD
        if(i == PASS_LENGTH)
        {
            attempts = 0;

            CmdLCD(CLEAR_LCD);

            StrLCD("ACCESS GRANTED");

            delay_ms(500);

            return 1;
        }

        // WRONG PASSWORD
        attempts++;

        remaining = 3 - attempts;

        CmdLCD(CLEAR_LCD);

        CmdLCD(GOTO_LINE1_POS0);
        StrLCD("WRONG PASS");

        CmdLCD(GOTO_LINE2_POS0);

        if(remaining > 0)
        {
            StrLCD("LEFT:");

            U32LCD(remaining);
        }

        IOSET0 = BUZZER;

        delay_ms(150);

        IOCLR0 = BUZZER;

        delay_ms(250);
    }

    // LOCK SYSTEM
    {
        u8 sec;

        CmdLCD(CLEAR_LCD);

        CmdLCD(GOTO_LINE1_POS0);
        StrLCD("SYSTEM LOCKED");

        IOSET0 = BUZZER;

        for(sec = 10; sec > 0; sec--)
        {
            CmdLCD(GOTO_LINE2_POS0);

            StrLCD("WAIT ");

            if(sec < 10)
                CharLCD('0');

            U32LCD(sec);

            StrLCD(" SEC ");

            delay_ms(1000);
        }

        IOCLR0 = BUZZER;
    }

    attempts = 0;

    CmdLCD(CLEAR_LCD);

    StrLCD("TRY AGAIN");

    delay_ms(500);

    return 0;
}

// --------------------------------------------------
void Password_Change(void)
{
    u8 new_pass[PASS_LENGTH];
    u8 confirm_pass[PASS_LENGTH];

    u8 i;

    if(!Password_Verify())
    {
        return;
    }

    CmdLCD(CLEAR_LCD);

    CmdLCD(GOTO_LINE1_POS0);
    StrLCD("NEW PASS:");

    CmdLCD(GOTO_LINE2_POS0);

    for(i = 0; i < PASS_LENGTH; i++)
    {
        char key = 0;

        while(key == 0)
        {
            Alarm_Task();

            key = KeyScan();
        }

        delay_ms(20);

        while(KeyScan() != 0)
        {
            Alarm_Task();
        }

        new_pass[i] = key;

        CharLCD('*');
    }

    CmdLCD(CLEAR_LCD);

    CmdLCD(GOTO_LINE1_POS0);
    StrLCD("CONFIRM:");

    CmdLCD(GOTO_LINE2_POS0);

    for(i = 0; i < PASS_LENGTH; i++)
    {
        char key = 0;

        while(key == 0)
        {
            Alarm_Task();

            key = KeyScan();
        }

        delay_ms(20);

        while(KeyScan() != 0)
        {
            Alarm_Task();
        }

        confirm_pass[i] = key;

        CharLCD('*');
    }

    for(i = 0; i < PASS_LENGTH; i++)
    {
        if(new_pass[i] != confirm_pass[i])
        {
            CmdLCD(CLEAR_LCD);

            StrLCD("NOT MATCH");

            IOSET0 = BUZZER;

            delay_ms(400);

            IOCLR0 = BUZZER;

            return;
        }
    }

    for(i = 0; i < PASS_LENGTH; i++)
    {
        stored_pass[i] = new_pass[i];
    }

    CmdLCD(CLEAR_LCD);

    StrLCD("PASS UPDATED");

    delay_ms(700);
}

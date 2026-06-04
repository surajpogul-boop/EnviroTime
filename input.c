#include <LPC21xx.h>

#include "input2.h"
#include "lcd.h"
#include "lcd_defines.h"
#include "keypad.h"
#include "alarm.h"
#include "delays.h"
#include "types.h"

u8 InputDigits(char *msg, u8 *buf, u8 digits)
{
    u8 idx = 0;

    char key;

    while(1)
    {
            u8 i;
        ClearLineLCD(1);
        ClearLineLCD(2);

        LCD_Print(GOTO_LINE1_POS0, msg);

        CmdLCD(GOTO_LINE2_POS0);

        for(i=0;i<idx;i++)
        {
            CharLCD(buf[i]);
        }

        key = GetKey();

        // DIGIT
        if((key >= '0') && (key <= '9'))
                {
            if(idx < digits)
            {
                buf[idx++] = key;
            }
        }

        // CLEAR / BACKSPACE
        else if(key == 'C')
        {
            if(idx > 0)
            {
                idx--;
            }
        }

        // ENTER
        else if(key == '=')
        {
            if(idx == digits)
            {
                buf[idx] = '\0';

                return 1;
            }
        }
    }
}

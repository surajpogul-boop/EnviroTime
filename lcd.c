#include <LPC21xx.h>

#include "lcd.h"
#include "lcd_defines.h"
#include "defines.h"
#include "types.h"
#include "delays.h"

void WriteLCD(u8 byte)
{
    IOCLR0 = (0xFF << LCD_DATA);

    IOSET0 = (byte << LCD_DATA);

    IOSET0 = (1 << LCD_EN);

    delay_us(1);

    IOCLR0 = (1 << LCD_EN);

    delay_ms(2);
}

void CmdLCD(u8 cmd)
{
    IOCLR0 = 1 << LCD_RS;

    WriteLCD(cmd);
}

void InitLCD(void)
{
    WRITEBYTE(IODIR0, LCD_DATA, 0xFF);

    SETBIT(IODIR0, LCD_RS);
    SETBIT(IODIR0, LCD_RW);
    SETBIT(IODIR0, LCD_EN);

    delay_ms(20);

    CmdLCD(0x38);

    CmdLCD(0x0C);

    CmdLCD(0x01);

    CmdLCD(0x06);
}

void CharLCD(u8 asciiVal)
{
    IOSET0 = 1 << LCD_RS;

    WriteLCD(asciiVal);
}

void StrLCD(char *s)
{
    while(*s)
    {
        CharLCD(*s++);
    }
}

void U32LCD(u32 n)
{
    s32 i = 0;

    u8 a[10];

    if(n == 0)
    {
        CharLCD('0');
        return;
    }

    while(n > 0)
    {
        a[i++] = (n % 10) + '0';

        n /= 10;
    }

    for(i = i - 1; i >= 0; i--)
    {
        CharLCD(a[i]);
    }
}

void S32LCD(s32 n)
{
    if(n < 0)
    {
        CharLCD('-');

        n = -n;
    }

    U32LCD(n);
}

void F32LCD(f32 fn,u8 nDP)
{
    u32 n,i;

    if(fn < 0.0)
    {
        CharLCD('-');

        fn = -fn;
    }

    n = fn;

    U32LCD(n);

    CharLCD('.');

    for(i=0;i<nDP;i++)
    {
        fn=(fn-n)*10;

        n=fn;

        CharLCD(n+48);
    }
}

void ClearLineLCD(u8 line)
{
    u8 i;

    if(line == 1)
        CmdLCD(GOTO_LINE1_POS0);
    else
        CmdLCD(GOTO_LINE2_POS0);

    for(i = 0; i < 16; i++)
    {
        CharLCD(' ');
    }
}

void LCD_Print(u8 pos, char *str)
{
    CmdLCD(pos);

    StrLCD(str);
}

void LCD_ShowStars(u8 count)
{
    u8 i;

    for(i=0;i<count;i++)
    {
        CharLCD('*');
    }
}


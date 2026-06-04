#include <LPC21xx.h>
#include "types.h"
#include "delays.h"
#include "alarm.h"

// Row: P1.16–P1.19 (OUTPUT)
// Col: P1.20–P1.23 (INPUT)

#define ROW_MASK (0xF << 16)
#define COL_MASK (0xF << 20)

// Keypad LUT
char kpmLUT[4][4] = {
    {'7','8','9','/'},
    {'4','5','6','*'},
    {'1','2','3','-'},
    {'C','0','=','+'}
};

// Initialize keypad
void InitKPM(void)
{
    // Rows as OUTPUT
    IODIR1 |= ROW_MASK;

    // Columns as INPUT
    IODIR1 &= ~COL_MASK;

    // Set all rows HIGH (idle state)
    IOSET1 = ROW_MASK;
}

// Scan keypad
char KeyScan(void)
{
    u8 row, col;

    for(row = 0; row < 4; row++)
    {
        IOSET1 = ROW_MASK;

        IOCLR1 = (1 << (16 + row));

        for(col = 0; col < 4; col++)
        {
            if(!(IOPIN1 & (1 << (20 + col))))
            {
                delay_ms(20);

                if(!(IOPIN1 & (1 << (20 + col))))
                {
                    return kpmLUT[row][col];
                }
            }
        }
    }

    return 0;
}

char GetKey(void)
{
    char key = 0;

    // WAIT FOR PRESS
    while(key == 0)
    {
        Alarm_Task();

        key = KeyScan();
    }

    delay_ms(20);

    // WAIT FOR RELEASE
    while(KeyScan() != 0)
    {
        Alarm_Task();
    }

    delay_ms(20);

    return key;
}


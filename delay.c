#include "delays.h"

void delay_us(u32 dlyUS)
{
    for(dlyUS *= 12; dlyUS > 0; dlyUS--);
}

void delay_ms(u32 dlyMS)
{
    for(dlyMS *= 12000; dlyMS > 0; dlyMS--);
}

void delay_s(u32 dlyS)
{
    for(dlyS *= 12000000; dlyS > 0; dlyS--);
}


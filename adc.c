#include "types.h"
#include <LPC21xx.h>
#include "adc_defines.h"
#include "delays.h"
#include "adc.h"

// ---------------- ADC INIT ----------------
void Init_ADC(void)
{
    // P0.28 -> AD0.1
    PINSEL1 &= ~(3 << 24);
    PINSEL1 |=  (1 << 24);

    // ADC configuration
    ADCR =
        (1 << PDN_BIT) |
        (CLKDIV << CLKDIV_BITS);
}

// ---------------- ADC READ ----------------
void Read_ADC(u8 chNO, u16 *dVal, f32 *eAR)
{
    // Select channel
    ADCR &= ~(0xFF);
    ADCR |= (1 << chNO);

    // Start conversion
    ADCR |= (1 << 24);

    // Wait till DONE
    while(((ADDR >> 31) & 1) == 0);

    // Read ADC value
    *dVal = (ADDR >> 6) & 0x3FF;

    // Voltage
    *eAR = (*dVal * 3.3f) / 1023.0f;

    // Stop conversion
    ADCR &= ~(7 << 24);
}

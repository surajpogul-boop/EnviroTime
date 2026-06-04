#ifndef _ADC_H_
#define _ADC_H_

#include "types.h"

void Init_ADC(void);

void Read_ADC(u8 chNO, u16 *dVal, f32 *eAR);

#endif

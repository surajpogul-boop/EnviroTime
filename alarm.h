#ifndef _ALARM_H_
#define _ALARM_H_

#include "types.h"

#define BUZZER   (1<<25)
#define SW_ALARM (1<<1)

void Alarm_Init(void);

void Alarm_Task(void);

void Alarm_Menu(u32 *a1, u32 *a2);

#endif

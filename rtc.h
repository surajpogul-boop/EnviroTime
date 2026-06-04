#ifndef _RTC_H_
#define _RTC_H_

#include <lpc21xx.h>
#include "types.h"

// Function declarations
void RTC_Init(void);

void GetRTCTimeInfo(s32 *,s32 *,s32 *);
void DisplayRTCTime(u32,u32,u32);

void GetRTCDateInfo(s32 *,s32 *,s32 *);
void DisplayRTCDate(u32,u32,u32);

void SetRTCTimeInfo(u32,u32,u32);
void SetRTCDateInfo(u32,u32,u32);

void GetRTCDay(s32 *);
void DisplayRTCDay(u32);
void SetRTCDay(u32);
void RTC_Edit(void);
#endif


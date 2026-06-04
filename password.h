// ==================================================
// password.h
// ==================================================

#ifndef _PASSWORD_H_
#define _PASSWORD_H_

#include "types.h"

#define PASS_LENGTH 4

void Password_Init(void);

u8 Password_Verify(void);

void Password_Change(void);

#endif



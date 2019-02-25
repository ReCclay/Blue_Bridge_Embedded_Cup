#ifndef _PWMMODE_H
#define _PWMMODE_H

#include "config.h"

void TIM3_PWMInit(u32 freq, u8 ch1_duty, u8 ch2_duty);

#endif

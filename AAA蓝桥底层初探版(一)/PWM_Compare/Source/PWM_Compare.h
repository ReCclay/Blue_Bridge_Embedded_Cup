#ifndef _PWM_COMPARE_H
#define _PWM_COMPARE_H

#include "config.h"

extern u32 CH1_Val, CH2_Val, CH1_Duty, CH2_Duty;

void TIM4_PWMCompare(u32 ch1_freq, u32 ch2_freq, u32 ch1_duty, u32 ch2_duty);

#endif

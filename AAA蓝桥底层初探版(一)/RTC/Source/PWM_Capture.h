#ifndef _PWM_CAPTURE_H
#define _PWM_CAPTURE_H

#include "config.h"


void TIM3_CaptureInit(void);

extern u8 CAPTURE_MODE;

extern u8 TIM3_CH1_CAPTURE_MODE;
extern u32 TIM3_CH1_CAPTURE_H, TIM3_CH1_CAPTURE_HL;

extern u8 TIM3_CH2_CAPTURE_MODE;
extern u32 TIM3_CH2_CAPTURE_H, TIM3_CH2_CAPTURE_HL;

#endif

#ifndef PWM_OC_H
#define PWM_OC_H

extern u32 TIM2_CH2_VAL;
extern u32 TIM2_CH2_Duty;

void PWM_OC_Init(u32 freq, u32 duty);

#endif

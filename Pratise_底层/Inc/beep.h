#ifndef BEEP_H
#define BEEP_H

#include "stm32f10x.h"

#define BeepOn()  GPIO_ResetBits(GPIOB, GPIO_Pin_4);
#define BeepOff() GPIO_SetBits(GPIOB, GPIO_Pin_4);

void BEEP_Init(void);
void BeepScan(u8 ms);
void Beep(s32 time);

#endif

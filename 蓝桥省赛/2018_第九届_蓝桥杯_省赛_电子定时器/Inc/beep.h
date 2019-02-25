#ifndef _BEEP_H
#define _BEEP_H


#define BeepOff() GPIO_SetBits(GPIOB, GPIO_Pin_4)
#define BeepOn() GPIO_ResetBits(GPIOB, GPIO_Pin_4)

void BeepInit(void);
void BeepScan(u8 ms);
void Beep(s32 time);

#endif

#ifndef BEEP_H
#define BEEP_H

#define BeepOff() GPIO_SetBits(GPIOB, GPIO_Pin_4)
#define BeepOn() GPIO_ResetBits(GPIOB, GPIO_Pin_4)

void BEEP_Init(void);
void Beep(s32 time);
void BeepScan(u8 ms);

#endif

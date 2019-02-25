#ifndef _USART_H
#define _USART_H

#include "stm32f10x.h"

void USART2Init(u32 baud);
void USART2_SendByte(u8 *str);
void USART2_RecByte(void);

extern u8 RxdBuf[50];
extern u8 RxdOverFlag;
extern u8 RxdCnt;

#endif

#ifndef USART2_H
#define USART2_H

#include "stm32f10x.h"

void USART2_Init(void);
void SendString(u8 *str);

#endif

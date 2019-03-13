#include "led.h"

//Ëø´æÆ÷N_LED(PD2)
//Ð¡µÆPC8~PC15
void Led_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;//PC8~PC15
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = 0xFF00;//PC8~PC15
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIOD->ODR |= (1<<2);
	GPIOC->ODR = 0xFFFF;//Ð¡µÆ³õÊ¼×´Ì¬¹Ø±Õ
	GPIOD->ODR &=~(1<<2);
}

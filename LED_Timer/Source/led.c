#include "led.h"

void LEDInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD, ENABLE);//设能PC和PD口
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);//设置573锁存器对应的使能端PD2
	
	GPIO_InitStructure.GPIO_Pin = 0xFF00;//注意看这里
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);//对应PC8-PC15
	
	GPIOD->ODR |= (1<<2);//PD2置1，使能573锁存器
	GPIOC->ODR |= 0xFF00;//PC8-PC15置1，初始状态熄灭小灯
	GPIOD->ODR &= ~(1<<2);//PD2清0，关闭573锁存器
}


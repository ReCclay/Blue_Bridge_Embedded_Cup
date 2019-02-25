#include "stm32f10x.h"

void LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD, ENABLE);//设能PC和PD口
	
	//锁存器IO配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//强推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	//8个LED的IO配置
	GPIO_InitStructure.GPIO_Pin = 0xFF00;//PC8~PC15对应的LED位置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//强推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIOC->ODR |= 0xFF00;//初始状态熄灭小灯
	GPIOD->ODR |= (1<<2);//使能573
	GPIOD->ODR &= ~(1<<2);//失能573
}

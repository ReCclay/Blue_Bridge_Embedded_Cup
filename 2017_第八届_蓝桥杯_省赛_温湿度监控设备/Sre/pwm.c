#include "stm32f10x.h"

void PWM_Init(u32 freq, u8 duty)
{
	u32 arr;
	
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	arr = 1000000 / freq;//计算频率对应的周期值
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//强推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	TIM_TimeBaseStructure.TIM_Prescaler = 72-1;//72分频
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = arr - 1;//周期值
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);//TIM2定时器
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;//极性
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;//极性
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = (arr-1) * duty / 100;//占空比
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);//TIM2_CH2
	
	TIM_Cmd(TIM2, ENABLE);//使能TIM2
}


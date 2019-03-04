#include "stm32f10x.h"

void PWM_IC_Init(void)
{
//	GPIO_InitTypeDef GPIO_InitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;
//	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
//	TIM_ICInitTypeDef  TIM_ICInitStructure;
//	
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
//	
//	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1 | GPIO_Pin_2;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);
//	
//	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
//	
//	//注意仍需要定时器的配置
//	TIM_TimeBaseStructure.TIM_Period = 0xFFFF;
//	TIM_TimeBaseStructure.TIM_Prescaler = 71;
//	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
//	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
//	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);//注意是哪个定时器(从别的地方复制的)
//	
//	
//	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
//	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
//	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
//	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
//	TIM_ICInitStructure.TIM_ICFilter = 0x0;
//	TIM_ICInit(TIM2, &TIM_ICInitStructure);
//	
//	TIM_ICInitStructure.TIM_Channel = TIM_Channel_3;
//	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
//	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
//	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
//	TIM_ICInitStructure.TIM_ICFilter = 0x0;
//	TIM_ICInit(TIM2, &TIM_ICInitStructure);
//	
//	TIM_Cmd(TIM2, ENABLE);
//	TIM_ITConfig(TIM2, TIM_IT_CC2 | TIM_IT_CC3, ENABLE);


	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	TIM_TimeBaseInitStructure.TIM_Period = 0xffff;
	TIM_TimeBaseInitStructure.TIM_Prescaler = 71;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = 0x0;
	TIM_TimeBaseInitStructure.TIM_CounterMode = 0x0;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICFilter = 0x0;
	TIM_ICInit(TIM2, &TIM_ICInitStructure);
	
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_3;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICFilter = 0x0;
	TIM_ICInit(TIM2, &TIM_ICInitStructure);
	
	TIM_Cmd(TIM2, ENABLE);
	TIM_ITConfig(TIM2, TIM_IT_CC3 | TIM_IT_CC2, ENABLE);

}

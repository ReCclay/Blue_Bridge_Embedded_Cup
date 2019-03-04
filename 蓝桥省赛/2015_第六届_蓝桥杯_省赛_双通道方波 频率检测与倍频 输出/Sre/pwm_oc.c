#include "stm32f10x.h"

u32 TIM3_Ch1_Val;
u32 TIM3_Ch2_Val;

void TIM3_OUTPUT_Init(u32 ch2_fre,u32 ch3_fre,u8 status,u8 Ch1_Enable,u8 Ch2_Enable)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
		TIM_OCInitTypeDef TIM_OCInitStructure;
		if(status)
		{
				RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
				RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
			
				GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
				GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
				GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
				GPIO_Init(GPIOA, &GPIO_InitStructure);
			
				TIM_TimeBaseInitStructure.TIM_Period = 0xffff;
				TIM_TimeBaseInitStructure.TIM_Prescaler = 71;
				TIM_TimeBaseInitStructure.TIM_ClockDivision = 0x0;
				TIM_TimeBaseInitStructure.TIM_CounterMode = 0x0;
				TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);
			
				NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
				NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
				NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
				NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
				NVIC_Init(&NVIC_InitStructure);
		}
		TIM3_Ch1_Val = 1000000 / ch2_fre;
		TIM3_Ch2_Val = 1000000 / ch3_fre;
		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Toggle;
		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
		if(Ch1_Enable)
		{
				TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
		}else
		{
				TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;
		}
		TIM_OCInitStructure.TIM_Pulse = TIM3_Ch2_Val;
		TIM_OC1Init(TIM3,&TIM_OCInitStructure);
		
		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Toggle;
		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
		if(Ch2_Enable)
		{
				TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
		}else
		{
				TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;
		}
		TIM_OCInitStructure.TIM_Pulse = TIM3_Ch2_Val;
		TIM_OC2Init(TIM3,&TIM_OCInitStructure);
		
		TIM_SetCompare1(TIM3, 0x0);
		TIM_SetCompare2(TIM3, 0x0);
		TIM_Cmd(TIM3, ENABLE);
		TIM_ITConfig(TIM3, TIM_IT_CC1 | TIM_IT_CC2, ENABLE);
}


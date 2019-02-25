#include "stm32f10x.h"

/*最后一个参数表示的是是否使能输出PWM*/
void PWM_Init(u32 freq, u8 duty, u8 en)
{
	u32 arr;
	
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	arr = 1000000 / freq;//计算频率对应的周期值
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//强推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	TIM_TimeBaseStructure.TIM_Prescaler = 72-1;//72分频
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = arr - 1;//周期值
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);//TIM3定时器
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;//极性
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;//极性
	if(en == 1)
	{
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	}
	else
	{
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;
	}
	TIM_OCInitStructure.TIM_Pulse = (arr-1) * duty / 100;//占空比
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);//TIM3_CH1
	
	TIM_Cmd(TIM3, ENABLE);//使能TIM3
}


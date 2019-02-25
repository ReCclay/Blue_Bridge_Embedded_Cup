#include "stm32f10x.h"

/*sta参数表示是否初始化IO，en参数表示是否使能通道输出*/
void PWM_OC_Init(u8 freq, u8 duty, u8 sta, u8 en)
{
	u16 freq_Buf;
	
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	//别忘了先开启对应时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_TIM1, ENABLE);
	
	if(sta)//由于这个函数需要多次调用，但是并不需要每次都重新初始化IO所以这里来个变量
	{
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//强推挽输出
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//强推挽输出
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
	}
	
	freq_Buf = 1000000 / (freq*1000);//注意这里要进行放大
	
	TIM_TimeBaseStructure.TIM_Period = freq_Buf - 1;//总的值设置为0xFFFF
	TIM_TimeBaseStructure.TIM_Prescaler = 71;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
	
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;//转换模式
	if(en)
	{
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
		TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
	}
	else
	{
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;
		TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
	}
	TIM_OCInitStructure.TIM_Pulse = (freq_Buf - 1) * duty / 100;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;//高级定时器多了这个
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
	TIM_OC2Init(TIM1, &TIM_OCInitStructure);//注意这里是OC2
	
	TIM_CtrlPWMOutputs(TIM1, ENABLE);//高级定时器必须有这个
	TIM_Cmd(TIM1, ENABLE);//开启定时器
	
}



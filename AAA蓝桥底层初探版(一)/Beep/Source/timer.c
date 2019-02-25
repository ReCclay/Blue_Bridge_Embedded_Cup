#include "timer.h"


void NVIC_TIM2Enable(void)
{
	NVIC_InitTypeDef NVIC_initstructure;

    NVIC_initstructure.NVIC_IRQChannel = TIM2_IRQn;           //选择TIM2中断通道
    NVIC_initstructure.NVIC_IRQChannelCmd = ENABLE;           //使能中断通道
    NVIC_initstructure.NVIC_IRQChannelPreemptionPriority = 0; //设定抢占优先级为0
    NVIC_initstructure.NVIC_IRQChannelSubPriority = 0;        //设定响应优先级为0
    NVIC_Init(&NVIC_initstructure);

}

void TIM2Init(u16 period, u16 prescaler)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);//注意TIM2在APB1总线上
	TIM_TimeBaseStructure.TIM_Period = period-1;
	TIM_TimeBaseStructure.TIM_Prescaler = prescaler-1;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	NVIC_TIM2Enable();
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);//严谨起见，这里清一下0
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);//使能中断
	TIM_Cmd(TIM2, ENABLE);//使能定时器
}
	


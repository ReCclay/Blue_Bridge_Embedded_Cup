#include "PWM_Capture.h"

u8 TIM3_CH1_CAPTURE_MODE = 0;
u32 TIM3_CH1_CAPTURE_H = 0, TIM3_CH1_CAPTURE_HL = 0;

u8 TIM3_CH2_CAPTURE_MODE = 0;
u32 TIM3_CH2_CAPTURE_H = 0, TIM3_CH2_CAPTURE_HL = 0;

u8 CAPTURE_MODE;

void TIM3_IOInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//***注意这里是APB2***
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //使能 GPIOA时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7; //TIM3通道1-PA6 TIM3通道2-PA7
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//***输入捕获使用IO模式为上拉输入或者浮空输入
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//输入就所谓输出速度了，这句可省略。
	GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化 GPIO
}

void NVIC_TIM3Enable(void)
{
    NVIC_InitTypeDef NVIC_initstructure;

    NVIC_initstructure.NVIC_IRQChannel = TIM3_IRQn;           //选择TIM4中断通道
    NVIC_initstructure.NVIC_IRQChannelCmd = ENABLE;           //使能中断通道
    NVIC_initstructure.NVIC_IRQChannelPreemptionPriority = 0; //设定抢占优先级为0
    NVIC_initstructure.NVIC_IRQChannelSubPriority = 0;        //设定响应优先级为0
    NVIC_Init(&NVIC_initstructure);
}

void TIM3Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	
	//***注意这里是APB1***
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);  //使能定时器3时钟 
	TIM_TimeBaseStructure.TIM_Period = 0xFFFF; //***设置为0xFFFF
	TIM_TimeBaseStructure.TIM_Prescaler = 72-1; //设置预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM 向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //初始化 TIM3
}

void TIM3_CaptureInit(void)
{
	TIM_ICInitTypeDef TIM_ICInitStructure;

	TIM3_IOInit();//TIM3通道1-PA6 TIM3通道2-PA7配置
	TIM3Init();//TIM3定时器配置
	NVIC_TIM3Enable();//中断向量配置

	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;//选择通道
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;//上升沿触发
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;//必须选择指向TI寄存器
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;//不分频
	TIM_ICInitStructure.TIM_ICFilter = 0;//不滤波
	TIM_ICInit(TIM3, &TIM_ICInitStructure);

	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStructure.TIM_ICFilter = 0;
	TIM_ICInit(TIM3, &TIM_ICInitStructure);
	
	TIM_ITConfig(TIM3, TIM_IT_CC1|TIM_IT_CC2, ENABLE);	
	TIM_Cmd(TIM3, ENABLE);
}



#include "PWM_Compare.h"

void TIM4_IOInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	
	//***注意这里是APB2***
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //使能 GPIOB时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7; //TIM4通道1-PB6 TIM4通道2-PB7
	//***复用推挽输出(PWM模式)***
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure); //初始化 GPIO
}

void NVIC_TIM4Enable(void)
{
    NVIC_InitTypeDef NVIC_initstructure;

    NVIC_initstructure.NVIC_IRQChannel = TIM4_IRQn;           //选择TIM4中断通道
    NVIC_initstructure.NVIC_IRQChannelCmd = ENABLE;           //使能中断通道
    NVIC_initstructure.NVIC_IRQChannelPreemptionPriority = 0; //设定抢占优先级为0
    NVIC_initstructure.NVIC_IRQChannelSubPriority = 0;        //设定响应优先级为0
    NVIC_Init(&NVIC_initstructure);
}

void TIM4Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	
	//***注意这里是APB1***
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);  //使能定时器4时钟 
	//***注意这里设置的是0xFFFF
	TIM_TimeBaseStructure.TIM_Period = 0xFFFF; //设置计数值最大
	TIM_TimeBaseStructure.TIM_Prescaler = 72-1; //设置预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM 向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //初始化 TIM4
	//***别忘了设置这个***
	NVIC_TIM4Enable();
}

u32 CH1_Val,  CH2_Val;
u32  CH1_Duty,  CH2_Duty;

void TIM4_PWMCompare(u32 ch1_freq, u32 ch2_freq, u32 ch1_duty, u32 ch2_duty)
{
//	u32 CH1_Val,  CH2_Val;//FUCK!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	
	TIM_OCInitTypeDef TIM_OCInitStructure;

	CH1_Val = 1000000 / ch1_freq;
	CH2_Val = 1000000 / ch2_freq;
	CH1_Duty = CH1_Val * ch1_duty / 100;
	CH2_Duty = CH2_Val * ch2_duty / 100;
	
	TIM4_IOInit();//TIM4通道1-PB6 TIM4通道2-PB7配置
	TIM4Init();//TIM4定时器配置
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Toggle;//触发模式
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;//输出极性低
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = CH1_Val;
	//TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);//预装载使能位
	TIM_OC1Init( TIM4, &TIM_OCInitStructure);
			
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Toggle;//触发模式
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;//输出极性低
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//比较输出使能
	TIM_OCInitStructure.TIM_Pulse = CH2_Val;
	//TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);//预装载使能位;
	TIM_OC2Init( TIM4, &TIM_OCInitStructure);
	
	TIM_SetCounter(TIM4, 0);//定时器计数值清0
	TIM_SetCompare1(TIM4, 0);//定时器捕获比较1寄存器值清0
	TIM_SetCompare2(TIM4, 0);//定时器捕获比较2寄存器值清0
	
	TIM_ClearITPendingBit(TIM4, TIM_IT_CC1|TIM_IT_CC2);//清除中断标志位 //这个也会影响极性？
	TIM_ITConfig(TIM4, TIM_IT_CC1|TIM_IT_CC2, ENABLE);//使能通道比较中断
	TIM_Cmd(TIM4, ENABLE); //使能 TIM4
}




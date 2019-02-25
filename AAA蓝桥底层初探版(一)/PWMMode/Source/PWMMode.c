#include "PWMMode.h"

void TIM3_IOInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//***注意这里是APB2***
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //使能 GPIOA时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7; //TIM3通道1-PA6 TIM3通道2-PA7
	//***复用推挽输出(PWM模式)***
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化 GPIO
}

void TIM3Init(u32 arr, u8 ch1_duty, u8 ch2_duty)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	
	//***注意这里是APB1***
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);  //使能定时器3时钟 
	TIM_TimeBaseStructure.TIM_Period = arr-1; //设置在自动重装载周期值
	TIM_TimeBaseStructure.TIM_Prescaler = 72-1; //设置预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM 向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //初始化 TIM3
}

void TIM3_PWMInit(u32 freq, u8 ch1_duty, u8 ch2_duty)
{
	u32 arr;
	
	TIM_OCInitTypeDef TIM_OCInitStructure;
	arr = 1000000 / freq; 

	TIM3_IOInit();//TIM3通道1-PA6 TIM3通道2-PA7配置
	TIM3Init(arr, ch1_duty, ch2_duty);//TIM3定时器配置
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//PWM1模式
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;//输出极性高
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = (arr - 1) * ch1_duty / 100;
	//TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);//预装载使能位无所谓，不影响
	TIM_OC1Init( TIM3, &TIM_OCInitStructure);
			
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;//PWM2模式
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;//输出极性低
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//比较输出使能
	//***(arr-1)是总的计数值，而(arr-1)*占空比是电平反转时的计数值，故控制pluse就可以控制占空比***
	TIM_OCInitStructure.TIM_Pulse = (arr - 1) * ch2_duty / 100;
	//TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);//预装载使能位无所谓，不影响
	TIM_OC2Init( TIM3, &TIM_OCInitStructure);
	//***高级定时器必须有这一句，而通用就不必了***
	//TIM_CtrlPWMOutputs(TIM3, ENABLE);//使能PWM输出 
	TIM_Cmd(TIM3, ENABLE); //使能 TIM3
}


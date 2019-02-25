#include "stm32f10x.h"
#include "beep.h"


s32 BeepTimer = 0;//注意是有符号类型

void BeepInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);//注意这里要开启AFIO时钟
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_NoJTRST, ENABLE);//PB4引脚的重映射成普通GPIO
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//强推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	BeepOff();//上电初始化关闭蜂鸣器
}

void BeepScan(u8 ms)//ms对应的定时器扫描时间
{
	if(BeepTimer > 0)
	{
		BeepTimer -= ms;
		if(BeepTimer <= 0)
		{
			BeepOff();
			BeepTimer = 0;
		}
	}
}

void Beep(s32 time)
{
	BeepTimer = time;
	
	if(BeepTimer == 0)
	{
		BeepOff();
	}
	else
	{
		BeepOn();
	}
}

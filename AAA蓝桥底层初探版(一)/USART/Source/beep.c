/*******************************************************************************
* 文件名：main.c
* 描  述：
* 作  者：CLAY
* 版本号：v1.0.0
* 日  期: 2019年1月24日
* 备  注：Beep(x);响x毫秒；Beep(0)；不响 Beep(-1)；一直响
*         
*******************************************************************************
*/

#include "beep.h"

s32 BeepTimer = 0;

void BeepInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);
	//GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); 
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_NoJTRST,ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//推挽模式输出
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	BeepOff();//初始化关闭蜂鸣器
}

void BeepScan(s32 ms)
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


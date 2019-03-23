#include "stm32f10x.h"
#include "beep.h"

s32 BeepTimer = 0;

void BEEP_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_NoJTRST ,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
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

void BeepScan(u8 ms)
{
	if(BeepTimer > 0)
	{
		BeepTimer -= ms;
		if(BeepTimer <= 0)
		{
			BeepOff();
		}
	}
}


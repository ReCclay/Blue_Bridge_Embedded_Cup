#include "stm32f10x.h"
#include "key.h"

extern void KeyAction(u8 code);

u8 KeySta[4] = {1, 1, 1, 1};

void KEY_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void KeyDriver(void)
{
	u8 i;
	static u8 backup[4] = {1, 1, 1, 1};
	
	for(i=0; i<4; i++)
	{
		if(backup[i] != KeySta[i])
		{
			if(backup[i] != 0)
			{
				KeyAction(i+1);
			}
			backup[i] = KeySta[i];
		}
	}
}

void KeyScan(void)
{
	u8 i;
	static u8 keybuff[4] = {0xFF, 0xFF, 0xFF, 0xFF};
	
	keybuff[0] = (keybuff[0] << 1) | KEY1;
	keybuff[1] = (keybuff[1] << 1) | KEY2;
	keybuff[2] = (keybuff[2] << 1) | KEY3;
	keybuff[3] = (keybuff[3] << 1) | KEY4;
	
	for(i=0; i<4; i++)
	{
		if(keybuff[i] == 0x00)
		{
			KeySta[i] = 0;
		}
		else if(keybuff[i] == 0xFF)
		{
			KeySta[i] = 1;
		}
	}
}

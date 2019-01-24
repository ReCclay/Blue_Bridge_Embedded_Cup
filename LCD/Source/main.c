/*******************************************************************************
* 文件名：main.c
* 描  述：
* 作  者：CLAY
* 版本号：v1.0.0
* 日  期: 2019年1月24日
* 备  注：按键B1切换LED1状态并且蜂鸣器鸣叫0.1s
*         按键B2按下发生长鸣;按键B3停止鸣叫
*******************************************************************************
*/

#include "stm32f10x.h"
#include "led.h"
#include "key.h"
#include "timer.h"
#include "beep.h"
#include "lcd.h"
#include "stdio.h"


int main(void)
{
	
	u8 str[25];
	u8 temp = 30;
	float AO = 3.845;
	
	STM3210B_LCD_Init();
	LCD_Clear(Blue);
	LEDInit();
	KeyInit();
	BeepInit();
	TIM2Init(2000, 72);//定时2ms
	
	
	LCD_DisplayStringLine(Line3,(u8*) "qwertyuioplkjhgfdsazxcvb");
	sprintf((char*)str,"temp=%d   A0=%.1f  ",temp, AO);
	LCD_DisplayStringLine(Line8,str);
	
	while(1)
	{	
		KeyDriver();
	}
}

void KeyAction(int code)
{
	if(code == 1)//按下B1，切换灯状态，蜂鸣器鸣叫0.1s
	{
		GPIOC->ODR ^= (1<<8);//PC8不断取反
		GPIOD->ODR |= (1<<2);//PD2置1，使能573锁存器
		GPIOD->ODR &= ~(1<<2);//PD2清0，关闭573锁存器
		Beep(100);
	}
	else if(code == 2)
	{
		Beep(-1);
	}
	else if(code == 3)
	{
		Beep(0);
	}
}



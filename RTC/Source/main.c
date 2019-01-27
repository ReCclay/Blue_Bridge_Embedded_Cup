/*******************************************************************************
* 文件名：main.c
* 描  述：
* 作  者：CLAY
* 版本号：v1.0.0
* 日  期: 2019年1月26日
* 备  注：RTC时钟的时间显示
*         
*******************************************************************************
*/

#include "config.h"
#include "led.h"
#include "key.h"
#include "timer.h"
#include "beep.h"
#include "lcd.h"
#include "stdio.h"
#include "rtc.h"

u8 RTC_Flag = 0;
u8 string[20];

int main(void)
{
	u8 hour,min,sec;
	u32 TimeVal;
	
	STM3210B_LCD_Init();
	LCD_Clear(Blue);
	LCD_SetBackColor(Blue);
	LCD_SetTextColor(White);
	
	LEDInit();
	KeyInit();
	BeepInit();
	TIM2Init(2000, 72);//定时2ms
	
	RTCInit(23,59,55);
	
	while(1)
	{	
		KeyDriver();
		if(RTC_Flag)
		{
			RTC_Flag = 0;
			TimeVal = RTC_GetCounter();
			hour = TimeVal / 3600;
			min = TimeVal % 3600 / 60;
			sec = TimeVal % 3600 % 60;
			sprintf((char*)string,"TIME:%.2d:%.2d:%.2d    ",hour,min,sec);
			LCD_DisplayStringLine(Line3, string);
		}
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
	else if(code == 4)
	{
		
	}
}



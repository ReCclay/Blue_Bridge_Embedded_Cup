/*******************************************************************************
* 文件名：main.c
* 描  述：
* 作  者：CLAY
* 版本号：v1.0.0
* 日  期: 2019年2月18日
* 备  注：修改后的LCD例程
*         RTC显示到LCD上，并注意到24:00:00的特殊处理！
*******************************************************************************
*/

#include "stm32f10x.h"
#include "lcd.h"
#include "e2prom.h"
#include "stdio.h"
#include "i2c.h"
#include "adc.h"
#include "rtc.h"

u32 TimingDelay = 0;

void Delay_Ms(u32 nTime);

u8 RTC_Flag = 0;
u32 time = 0;

u8 hour, min, sec;

//Main Body
int main(void)
{
	u8 str[20];
	
	STM3210B_LCD_Init();
	LCD_Clear(Blue);
	LCD_SetBackColor(Blue);
	LCD_SetTextColor(White);
	
	SysTick_Config(SystemCoreClock/1000);
	
	RTC_Init(23, 59, 55);
	
	while(1)
	{
		if(RTC_Flag)
		{
			RTC_Flag = 0;
			time = RTC_GetCounter();
			RTC_WaitForLastTask();
			
			if(time == 24 * 3600)
			{
				RTC_SetCounter(1);//特殊处理下，不至于到0的时候卡顿。
				RTC_WaitForLastTask();
				LCD_DisplayStringLine(Line5, "     00:00:00   ");
			}
			else
			{
				hour = time / 3600;
				min = time % 3600 / 60;
				sec = time % 3600 % 60;
				sprintf((char *)str, "     %.2d:%.2d:%.2d   ", hour, min, sec);
				LCD_DisplayStringLine(Line5, str);
			}			
		}
	}
}

//
void Delay_Ms(u32 nTime)
{
	TimingDelay = nTime;
	while(TimingDelay != 0);	
}

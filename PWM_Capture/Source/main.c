/*******************************************************************************
* 文件名：main.c
* 描  述：
* 作  者：CLAY
* 版本号：v1.0.0
* 日  期: 2019年1月26日
* 备  注：TIM3_CH1-PA6, TIM3_CH2-PA7配置为输入捕获，
*         测量TIM4的CH1-PB6和PB7的频率以及占空比
*******************************************************************************
*/

#include "config.h"
#include "led.h"
#include "key.h"
#include "timer.h"
#include "beep.h"
#include "lcd.h"
#include "stdio.h"
#include "usart.h"
#include "i2c.h"
#include "eeprom.h"
#include "PWMMode_Advance.h"
#include "PWM_Compare.h"
#include "PWM_Capture.h"


u8 string[20];

int main(void)
{
	STM3210B_LCD_Init();
	LCD_Clear(Blue);
	LCD_SetBackColor(Blue);
	LCD_SetTextColor(White);
	
	LEDInit();
	KeyInit();
	BeepInit();
	TIM2Init(2000, 72);//定时2ms
	
	TIM1_PWMInit(2000, 40, 80);//频率2K CH2占空比40% CH3占空比80%
	TIM4_PWMCompare(2000, 1000, 40, 80);//CH1占空比2K 40% CH2 1K占空比80%
	
	TIM3_CaptureInit();
	
	while(1)
	{	
		KeyDriver();
		
		if(TIM3_CH1_CAPTURE_MODE == 3)
		{
			sprintf((char*)string,"ch1_fre:%d           ",1000000 / TIM3_CH1_CAPTURE_HL);
			LCD_DisplayStringLine(Line3, string);
			sprintf((char*)string,"ch1_duty:%d          ",TIM3_CH1_CAPTURE_H * 100/TIM3_CH1_CAPTURE_HL);
			LCD_DisplayStringLine(Line4, string);
			TIM3_CH1_CAPTURE_MODE = 0;
		}

		if(TIM3_CH2_CAPTURE_MODE == 3)
		{
			sprintf((char*)string,"ch2_fre:%d           ",1000000 / TIM3_CH2_CAPTURE_HL);
			LCD_DisplayStringLine(Line7, string);
			sprintf((char*)string,"ch2_duty:%d          ",TIM3_CH2_CAPTURE_H * 100/TIM3_CH2_CAPTURE_HL);
			LCD_DisplayStringLine(Line8, string);
			TIM3_CH2_CAPTURE_MODE = 0;
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



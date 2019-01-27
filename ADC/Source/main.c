/*******************************************************************************
* 文件名：main.c
* 描  述：
* 作  者：CLAY
* 版本号：v1.0.0
* 日  期: 2019年1月27日
* 备  注：ADC转换实现电压测量
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
#include "adc.h"

u8 ADC_Flag = 0;
u8 string[20];
float adc_val;

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
	
	ADC1Init();
	
	while(1)
	{	
		KeyDriver();
		if(ADC_Flag )
		{
			ADC_Flag = 0;
			adc_val = Get_ADC(8) * 3.3 / 4096;
			sprintf((char*)string,"ADC_VAL : %.2f    ",adc_val);
			LCD_DisplayStringLine(Line2, string);
		
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



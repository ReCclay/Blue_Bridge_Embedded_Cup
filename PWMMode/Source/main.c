/*******************************************************************************
* 文件名：main.c
* 描  述：
* 作  者：CLAY
* 版本号：v1.0.0
* 日  期: 2019年1月25日
* 备  注：通用定时器输出硬件PWM PA6-TIM3_CH1 PA7-TIM3_CH2
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
#include "usart.h"
#include "i2c.h"
#include "eeprom.h"
#include "PWMMode.h"

int main(void)
{
	
	STM3210B_LCD_Init();
	LCD_Clear(Blue);
	LEDInit();
	KeyInit();
	BeepInit();
	TIM2Init(2000, 72);//定时2ms

	TIM3_PWMInit(2000, 40, 80);
	
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



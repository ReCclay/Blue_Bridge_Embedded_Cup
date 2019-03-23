/*******************************************************************************
* 文件名：main.c
* 描  述：
* 作  者：CLAY
* 版本号：v1.0.0
* 日  期: 2019年3月23日
* 备  注：底层第七次默写
*         单独的底层默写最后一遍
*		  祝好运~
*******************************************************************************
*/

#include "stm32f10x.h"
#include "lcd.h"
#include "led.h"
#include "timer.h"
#include "key.h"
#include "beep.h"
#include "i2c.h"
#include "e2prom.h"
#include "stdio.h"
#include "adc.h"
#include "rtc.h"
#include "usart2.h"
#include "pwm_ic.h"
//#include "pwm_oc.h"
#include "pwm.h"


//全局变量
u32 TimingDelay = 0;
u8 str[20];
u8 flag200ms = 0;


//ADC相关
float ADC_VAL = 0;


//RTC相关
u8 HH, MM, SS;
u8 RTC_FLAG;
u32 time;


//USART2相关
u8 RxdOver;
u8 RxdCnt;
u8 RxdBuf[20];


//函数声明
void Delay_Ms(u32 nTime);

//Main Body
int main(void)
{
	u8 x=3, y=4;
	u8 i;
	
	STM3210B_LCD_Init();
	LCD_Clear(Blue);
	LCD_SetBackColor(Blue);
	LCD_SetTextColor(White);
	SysTick_Config(SystemCoreClock/1000);


	//初始化函数
	LED_Init();
	TIM4_Init(2000, 72);
	KEY_Init();
	BEEP_Init();
	i2c_init();
	ADC1_Init();
	RTC_Init(23, 59, 57);
	USART2_Init();
	PWM_IC_Init();
//	PWM_OC_Init(2000, 80);	
	PWM_Init(1000, 20);
	
	E2Write(0x00, x);
	Delay_Ms(5);
	y = E2Read(0x00);
	Delay_Ms(5);
	
	sprintf((char *)str, " E2: %d ", y);
	LCD_DisplayStringLine(Line1, str);
	
	while(1)
	{
		KeyDriver();
		
		if(flag200ms)
		{
			flag200ms = 0;
			
			ADC_VAL = Get_ADC();
			sprintf((char *)str, " ADC: %.2f  ", ADC_VAL * 3.3 / 4096 );
			LCD_DisplayStringLine(Line3, str);
			
			if(TIM3_CH2_MODE == 3)
			{
				sprintf((char *)str, " Freq: %.2d  ", 1000000 / TIM3_CH2_HL );
				LCD_DisplayStringLine(Line8, str);
				
				sprintf((char *)str, " Duty: %.2d  ", TIM3_CH2_H * 100 / TIM3_CH2_HL);
				LCD_DisplayStringLine(Line9, str);
				
				TIM3_CH2_MODE = 0;
			}
		}
		
		if(RTC_FLAG)
		{
			RTC_FLAG = 0;
			
			time = RTC_GetCounter();
			if(time == 23*3600 + 59*60 + 59)
			{
				RTC_SetCounter(0);
			}
			HH = time / 3600;
			MM = time % 3600 / 60;
			SS = time % 3600 % 60;
			
			sprintf((char *)str, " RTC: %.2d:%.2d:%.2d ", HH, MM, SS);
			LCD_DisplayStringLine(Line5, str);
		}
		
		if(RxdOver)
		{
			RxdOver = 0;
			
			USART2_SendStr(RxdBuf);
			LCD_DisplayStringLine(Line7, RxdBuf);
			
			USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
			for(i=0; i<20; i++)
			{
				RxdBuf[i] = 0;
			}
		}
	}
}

void KeyAction(u8 code)
{
	static u8 sta = 0;
	
	if(code == 1)
	{
		if(sta == 0)
		{
			GPIOD->ODR |= (1<<2);
			GPIOC->ODR = 0xFE00;
			GPIOD->ODR &=~(1<<2);
			sta = 1;
		}
		else
		{
			GPIOD->ODR |= (1<<2);
			GPIOC->ODR = 0xFF00;
			GPIOD->ODR &=~(1<<2);
			sta = 0;
		}
	}
	else if(code == 2)
	{
		Beep(100);
	}
	else if(code == 3)
	{
		Beep(-1);
	}
	else if(code == 4)
	{
		Beep(0);
	}
}	


//
void Delay_Ms(u32 nTime)
{
	TimingDelay = nTime;
	while(TimingDelay != 0);	
}

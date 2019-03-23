/*******************************************************************************
* 文件名：main.c
* 描  述：
* 作  者：CLAY
* 版本号：v1.0.0
* 日  期: 2019年3月13日
* 备  注：底层第一次默写
*        
*******************************************************************************
*/

#include "stm32f10x.h"
#include "lcd.h"
#include "timer.h"
#include "led.h"
#include "key.h"
#include "beep.h"
#include "i2c.h"
#include "e2prom.h"
#include "stdio.h"
#include "adc.h"
#include "rtc.h"
#include "usart2.h"
#include "pwm.h"
#include "pwm_oc.h"
#include "pwm_ic.h"

//全局变量
u32 TimingDelay = 0;
u8 str[20];
u8 flag200ms = 0;

//RTC相关
u8 RTC_Flag = 0;
u8 HH, MM, SS;
u32 time;

//串口相关变量
u8 RxdCnt = 0;
u8 RxdBuf[20];
u8 RxdOver = 0;

//捕获相关
extern u8 TIM3_CH2_CAPTURE_MODE;
extern u32 TIM3_CH2_CAPTURE_H;
extern u32 TIM3_CH2_CAPTURE_HL;
u32 TIM3_CH2_Duty = 0;
u32 TIM3_CH2_Freq = 0;

//函数声明
void Delay_Ms(u32 nTime);

//Main Body
int main(void)
{
	u8 i;
	u8 x=5, y=2;
	float ADC_Val = 0;
	
	STM3210B_LCD_Init();
	LCD_Clear(Blue);
	LCD_SetBackColor(Blue);
	LCD_SetTextColor(White);
	
	SysTick_Config(SystemCoreClock/1000);

	TIM4_Init(2000, 72);//72分频，计数值2000 -- 2ms定时
	Led_Init();
	KEY_Init();
	BEEP_Init();
	i2c_init();
	ADC1_Init();
	RTC_Init(23, 59, 57);
	USART2_Init();
	
//	PWM_Init(1000, 40);
	PWM_OC_Init(1000, 40);
	
	PWM_IC_Init();
	
	E2Write(0x00, x);
	Delay_Ms(5);
	y = E2Read(0x00);
	Delay_Ms(5);
	
	sprintf((char*)str, "E2 = %d   ", y);
	LCD_DisplayStringLine(Line4, str);
	
	while(1)
	{
		KeyDriver();
		
		if(RTC_Flag)
		{
			RTC_Flag = 0;
			time = RTC_GetCounter();
			
			if(time == (23*3600 + 59*60 + 59))
			{
				RTC_SetCounter(0);
			}
			
			HH = time / 3600;
			MM = time % 3600 / 60;
			SS = time % 3600 % 60;
			
			sprintf((char*)str, "RTC: %.2d:%.2d:%.2d ", HH, MM, SS);
			LCD_DisplayStringLine(Line6, str);
		}
		if(flag200ms)
		{
			flag200ms = 0;
			ADC_Val = Get_ADC();
			sprintf((char*)str, "  ADC = %.2f   ", ADC_Val * 3.3 / 4096);
			LCD_DisplayStringLine(Line2, str);
			
			if(TIM3_CH2_CAPTURE_MODE == 3)
			{
				TIM3_CH2_Freq = 1000000 / TIM3_CH2_CAPTURE_HL;
				TIM3_CH2_Duty = TIM3_CH2_CAPTURE_H*100 / TIM3_CH2_CAPTURE_HL;
				
				sprintf((char*)str, " Freq: %d Hz", TIM3_CH2_Freq);
				LCD_DisplayStringLine(Line8, str);
				sprintf((char*)str, " Duty: %d %", TIM3_CH2_Duty);
				LCD_DisplayStringLine(Line9, str);
				TIM3_CH2_CAPTURE_MODE = 0;
			}
		}
		if(RxdOver)
		{
			RxdOver = 0;
			LCD_DisplayStringLine(Line8, RxdBuf);
			SendString(RxdBuf);
			for(i=0; i<20; i++)
			{
				RxdBuf[i] = 0;
			}
			USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
		}
	}
}

void KeyAction(int code)
{
	static u8 sta = 0;
	
	if(code == 1)
	{
		if(sta == 0)
		{
			GPIOD->ODR |= (1<<2);
			GPIOC->ODR = 0xFEFF;//小灯初始状态关闭
			GPIOD->ODR &=~(1<<2);
			sta = 1;
			Beep(100);
		}
		else
		{
			GPIOD->ODR |= (1<<2);
			GPIOC->ODR = 0xFFFF;//小灯初始状态关闭
			GPIOD->ODR &=~(1<<2);
			sta = 0;
			Beep(100);
		}
	}
}

//
void Delay_Ms(u32 nTime)
{
	TimingDelay = nTime;
	while(TimingDelay != 0);	
}

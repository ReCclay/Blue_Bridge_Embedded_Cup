/*******************************************************************************
* 文件名：main.c
* 描  述：
* 作  者：CLAY
* 版本号：v1.0.0
* 日  期: 2019年2月19日
* 备  注：修改后的LCD例程
*         PA7 (TIM3_CH2) 输入捕获外来信号频率(两次上升沿)
*******************************************************************************
*/

#include "stm32f10x.h"
#include "lcd.h"
#include "e2prom.h"
#include "stdio.h"
#include "i2c.h"
#include "adc.h"
#include "rtc.h"
#include "usart2.h"
#include "pwm.h"
#include "pwm_oc.h"
#include "pwm_ic.h"

u32 TimingDelay = 0;
u8 RxdCnt = 0;
u8 RxdOver = 0;
u8 RxdBuf[20];
void Delay_Ms(u32 nTime);
u8 RTC_Flag = 0;

extern __IO uint32_t TIM3Freq;

//Main Body
int main(void)
{	
	u8 str[20];
	
	STM3210B_LCD_Init();
	LCD_Clear(Blue);
	LCD_SetBackColor(Blue);
	LCD_SetTextColor(White);
	
	SysTick_Config(SystemCoreClock/1000);
	
	//PWM_Init(500, 60);//500Hz 60%方波
	PWM_OC_Init(500, 60);//500Hz 60%方波
	
	
	PWM_IC_Init();
	while(1)
	{
		sprintf((char *)str, "  Freq = %d     ", TIM3Freq);//注意加点空格，避免残留数据影响
		LCD_DisplayStringLine(Line5, str);
		Delay_Ms(200);
	}
}

//
void Delay_Ms(u32 nTime)
{
	TimingDelay = nTime;
	while(TimingDelay != 0);	
}

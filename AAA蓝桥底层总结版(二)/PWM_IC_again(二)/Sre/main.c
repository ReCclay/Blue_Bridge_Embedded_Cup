/*******************************************************************************
* 文件名：main.c
* 描  述：
* 作  者：CLAY
* 版本号：v1.0.0
* 日  期: 2019年2月23日
* 备  注：修改后的LCD例程
*         PA7 (TIM3_CH2) 输入捕获外来信号频率和占空比
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
#include "timer.h"

u32 TimingDelay = 0;

//全局变量
u8 flag200ms = 0;
u8 str[20];

//串口相关
u8 RxdCnt = 0;
u8 RxdOver = 0;
u8 RxdBuf[20];
//RTC相关
u8 RTC_Flag = 0;

//捕获相关
extern u8 TIM3_CH2_CAPTURE_MODE;
extern u32 TIM3_CH2_CAPTURE_H;
extern u32 TIM3_CH2_CAPTURE_HL;
	
void Delay_Ms(u32 nTime);


//Main Body
int main(void)
{	
	u8 str[20];
	
	STM3210B_LCD_Init();
	LCD_Clear(Blue);
	LCD_SetBackColor(Blue);
	LCD_SetTextColor(White);
	
	SysTick_Config(SystemCoreClock/1000);
	TIM4_Init(2000 ,72);//定时2ms
	PWM_OC_Init(500, 60);//500Hz 60%方波
	PWM_IC_Init();
	
	while(1)
	{
		
		if(flag200ms)
		{
			flag200ms = 0;
			
			if(TIM3_CH2_CAPTURE_MODE == 3)
			{
				sprintf((char*)str, " Freq =  %d Hz   ", 1000000 / TIM3_CH2_CAPTURE_HL);
				LCD_DisplayStringLine(Line3, str);
				
				sprintf((char*)str, " Duty =  %d %%   ",  TIM3_CH2_CAPTURE_H * 100 / TIM3_CH2_CAPTURE_HL);
				LCD_DisplayStringLine(Line5, str);
				
				TIM3_CH2_CAPTURE_MODE = 0;
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

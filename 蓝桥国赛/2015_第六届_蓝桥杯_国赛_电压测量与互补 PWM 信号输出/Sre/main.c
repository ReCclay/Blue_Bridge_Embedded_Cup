/*******************************************************************************
* 文件名：main.c
* 描  述：
* 作  者：CLAY
* 版本号：v1.0.0
* 日  期: 2019年2月23日
* 备  注：修改后的LCD例程
*         2015_第六届_蓝桥杯_国赛_温湿度监控设备电压测量与互补 PWM 信号输出
*		  实现题设所有功能
*		  附加按键蜂鸣器
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
#include "led.h"
#include "key.h"
#include "beep.h"

u32 TimingDelay = 0;
void Delay_Ms(u32 nTime);


//全局变量
u8 flagSet = 0;//参数设置界面和实时显示界面切换
u8 str[20];//字符串公用的数组
u8 flag200ms = 1;//200ms定时到标志

//ADC变量
float ADC_Val;

//PWM输出
u8 OUT_Sta = 0;//0-STOP 1-START
u8 Show_CH2_Duty = 0;//PA9对应占空比
u8 Show_CH2N_Duty = 0;//PB14对应占空比
u8 Freq = 1;//信号对应的频率输出

//LED
u16 LED_MODE = 0XFFFF;

void ShowRealData(void);
void ShowSetDat(void);
void Refresh(void);

//Main Body
int main(void)
{	
	STM3210B_LCD_Init();
	LCD_Clear(Blue);
	LCD_SetBackColor(Blue);
	LCD_SetTextColor(White);
	SysTick_Config(SystemCoreClock/1000);

	/*初始化配置*/
	TIM4_Init(2000, 72);//定时2ms
	KeyInit();
	BeepInit();
	ADC1_Init();
	LED_Init();//LED配置
	i2c_init();
	
	PWM_OC_Init(Freq, Show_CH2_Duty, 1, 1);
	
	Freq = E2Read(0x00);//读取E2中的数据
	Delay_Ms(5);
	
	while(1)
	{
		KeyDriver();
		
		if(flag200ms)
		{
			flag200ms = 0;
			
			Refresh();
			
			if(!flagSet)
			{
				ShowRealData();//实时数据显示界面
			}
			else
			{
				ShowSetDat();//参数设置界面
			}
		}
	}
}

void Refresh(void)//刷新显示界面的相关参数
{
	ADC_Val = Get_ADC() * 3.3 / 4096;//采集ADC值
	Show_CH2_Duty = ADC_Val * 100 / 3.3; //PA9对应的占空比
	Show_CH2N_Duty = 100 - Show_CH2_Duty; //PB14对应的占空比
	
	if(OUT_Sta)
	{
		PWM_OC_Init(Freq, Show_CH2_Duty, 0, 1);
	}
	else
	{
		PWM_OC_Init(Freq, Show_CH2_Duty, 0, 0);
	}
	
	
}

void ShowRealData(void)//显示实时数据界面
{	

	LCD_DisplayStringLine(Line0, (u8 *)"     Real Data     ");
	
	sprintf((char *)str, " ADC_Val:  %.2fV         ", ADC_Val);
	LCD_DisplayStringLine(Line2, str);
	
	if(OUT_Sta == 0)
		LCD_DisplayStringLine(Line4, (u8*)" OUT_Sta:  STOP        ");
	else
		LCD_DisplayStringLine(Line4, (u8*)" OUT_Sta:  START       ");
	
	sprintf((char *)str, " Signal :  PA9 :%d%%       ", Show_CH2_Duty);
	LCD_DisplayStringLine(Line6, str);
	
	sprintf((char *)str, "           PB14:%d%%      ", Show_CH2N_Duty);
	LCD_DisplayStringLine(Line7, str);
	
	sprintf((char *)str, "           %dKHz        ", Freq);
	LCD_DisplayStringLine(Line8, str);
	
	LCD_DisplayStringLine(Line9, (u8*)"                  1  ");
}

void ShowSetDat(void)//参数设置界面
{
	LCD_DisplayStringLine(Line0, (u8 *)"     Set Data      ");
	
	sprintf((char *)str, " Freq:    %dKHz        ", Freq);
	LCD_DisplayStringLine(Line4, str);
	
	LCD_DisplayStringLine(Line9, (u8*)"                  2  ");
}

void KeyAction(int code)
{
	if(code == 1)
	{
		OUT_Sta ^= 1;//改变PWM输出状态
		
		if(OUT_Sta)
		{
			LED_MODE &= ~(1<<8);//开启状态下，点亮LED1
		}
		else
		{
			LED_MODE |= (1<<8);//关闭LED1
		}
		GPIOC-> ODR = LED_MODE;
		GPIOD-> ODR |= (1<<2);
		GPIOD-> ODR &=~(1<<2);
		Beep(100);//伴随滴的一声
	}
	else if(code == 2)//B2
	{
		flagSet ^= 1;//切换界面
		if(flagSet == 1)
			LCD_Clear(Blue);
		else//退出设置界面前向E2中存Freq
		{
			E2Write(0x00, Freq);
			Delay_Ms(5);
		}
		Beep(100);//伴随滴的一声
	}
	else if(code == 3)//加
	{
		if(flagSet)//仅在设置界面下有效
		{
			Freq += 1;
			if(Freq > 10)
				Freq = 1;
			Beep(100);//伴随滴的一声
		}
	}
	else if(code == 4)//减
	{
		
		Beep(100);//伴随滴的一声
	}
}

//
void Delay_Ms(u32 nTime)
{
	TimingDelay = nTime;
	while(TimingDelay != 0);	
}

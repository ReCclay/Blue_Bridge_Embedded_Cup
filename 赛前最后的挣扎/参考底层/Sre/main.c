/*******************************************************************************
* 文件名：main.c
* 描  述：
* 作  者：CLAY
* 版本号：v1.0.0
* 日  期: 2019年2月25日
* 备  注：修改后的LCD例程
*         2016_第七届_蓝桥杯_省赛_电压测量监控设备
*		  实现所有功能
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
u8 flagSet = 0;//0-实时数据界面 1-参数设置界面
u8 str[20];//字符串公用的数组
u8 flag200ms = 1;//200ms定时到标志

//串口相关变量
u8 RxdCnt = 0;
u8 RxdOver = 0;
u8 RxdBuf[20];

//LED
u16 LED_MODE = 0XFFFF;
u8 LED_Sta = 1;//LED报警，0-关， 1-开 默认是开

//RTC相关变量
u8 RTC_Flag = 0;
u32 time = 0;
u8 HH = 23, MM = 59, SS = 55;//实时时间
u32 time_Buf = 0;
u8 HH_Buf = 0, MM_Buf = 0, SS_Buf = 0;//上报时间 
u8 timeIndex = 0;//0-谁都不高亮 1-HH高亮 2-MM高亮 3-SS高亮

//ADC相关
float ADC_Val;
float k_Val = 0;

//函数声明
void UartAction(void);
void Refresh(void);
void ShowRealData(void);
void ShowSetData(void);
u32 TimeToSecs(u8 hour, u8 min, u8 sec);
void SubmitVolt(void);

//Main Body
int main(void)
{	
	STM3210B_LCD_Init();
	LCD_Clear(Blue);
	LCD_SetBackColor(Blue);
	LCD_SetTextColor(White);
	SysTick_Config(SystemCoreClock/1000);

	TIM4_Init(2000, 72);//定时2ms
	KeyInit();
	BeepInit();
	RTC_Init(HH, MM, SS);
	ADC1_Init();
	LED_Init();//LED配置
	USART2_Init();
	i2c_init();
		
	k_Val = E2Read(0x00) / 10.0;
	
	while(1)
	{
		KeyDriver();
		
		if(flag200ms)
		{
			flag200ms = 0;
			Refresh();
			
			if(flagSet == 0)
			{
				ShowRealData();
			}
			else if(flagSet == 1)
			{
				ShowSetData();
			}
		}
		
		if(RxdOver)//接收到以含有\n的字符串
		{
			RxdOver = 0;
			UartAction();//执行相应的串口动作
		}
		
		if(RTC_Flag)
		{
			RTC_Flag = 0;
			SubmitVolt();
		}
	}
}

void SubmitVolt(void)
{	
	if(time == (HH_Buf*3600 + MM_Buf*60 + SS_Buf))
	{
		sprintf((char *)str, "%.2f+%.1f+%.2d%.2d%.2d\n", ADC_Val, k_Val, HH_Buf, MM_Buf, SS_Buf);
		USART2_SendString(str);
	}
}

void ShowLedThre(void)
{
	if(LED_Sta && (ADC_Val > (3.3*k_Val/10) ) )
	{
		LED_MODE ^= (1<<8);
		GPIOC-> ODR = LED_MODE;
		GPIOD-> ODR |= (1<<2);
		GPIOD-> ODR &=~(1<<2);
	}
	else
	{
		LED_MODE |= (1<<8);
		GPIOC-> ODR = LED_MODE;
		GPIOD-> ODR |= (1<<2);
		GPIOD-> ODR &=~(1<<2);
	}
}

void Refresh(void)//实时数据刷新
{
	ADC_Val = Get_ADC() * 3.3 / 4096;
	ShowLedThre();//刷新LED状态
}

void ShowRealData(void)//显示实时数据界面
{	
	sprintf((char *)str, "  V1: %.2f         ", ADC_Val);
	LCD_DisplayStringLine(Line1, str);
	
	sprintf((char *)str, "  k: %.1f          ", k_Val);
	LCD_DisplayStringLine(Line3, str);
	
	if(LED_Sta)
	{
		LCD_DisplayStringLine(Line5, (u8*)"  LED: ON          ");
	}
	else
	{
		LCD_DisplayStringLine(Line5, (u8*)"  LED: OFF          ");
	}
	
	sprintf((char *)str, "  T: %.2d-%.2d-%.2d       ", HH, MM, SS);
	LCD_DisplayStringLine(Line7, str);
	
	
	LCD_DisplayStringLine(Line9, (u8*)"                1 ");
}

void RefreshTime(void)
{
	if(timeIndex == 1)
		LCD_SetTextColor(Green);
	else
		LCD_SetTextColor(White);
	LCD_DisplayChar(Line6, 240, HH_Buf/10 + '0');//对应的数字转化为ASCII
	LCD_DisplayChar(Line6, 225, HH_Buf%10 + '0');
	
	LCD_SetTextColor(White);
	LCD_DisplayChar(Line6, 210, '-');
	
	if(timeIndex == 2)
		LCD_SetTextColor(Green);
	else
		LCD_SetTextColor(White);
	LCD_DisplayChar(Line6, 195, MM_Buf/10 + '0');
	LCD_DisplayChar(Line6, 180, MM_Buf%10 + '0');
	
	LCD_SetTextColor(White);
	LCD_DisplayChar(Line6, 165, '-');
	
	if(timeIndex == 3)
		LCD_SetTextColor(Green);
	else
		LCD_SetTextColor(White);
	LCD_DisplayChar(Line6, 150, SS_Buf/10 + '0');
	LCD_DisplayChar(Line6, 135, SS_Buf%10 + '0');
	
	LCD_SetTextColor(White);	
}

void ShowSetData(void)//参数设置界面
{
	LCD_DisplayStringLine(Line3, (u8*)"     Setting          ");
	
	RefreshTime();
	
	LCD_DisplayStringLine(Line9, (u8*)"                 2 ");
}

void UartAction(void)
{
	u8 i;

	if(RxdBuf[0] == 'k')
	{
		if( ( (RxdBuf[3]-'0') >= 1) && ( (RxdBuf[3]-'0') <= 9) )
		{
			k_Val = (RxdBuf[3] - '0') / 10.0;//注意这里必须10.0，不然得出的结果是截断后的
			E2Write(0x00, RxdBuf[3] - '0');
			USART2_SendString((u8*)"OK\n");
		}
	}
	
	for(i=0; i<20; i++)//清空缓冲区
		RxdBuf[i] = 0;	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//接收的数据处理完毕后打开接收中断
}

void IncTime()
{
	if(timeIndex == 1)
	{
		HH_Buf += 1;
		if(HH_Buf >= 60)
		{
			HH_Buf = 0;
		}
	}
	else if(timeIndex == 2)
	{
		MM_Buf += 1;
		if(MM_Buf >= 60)
		{
			MM_Buf = 0;
		}
	}
	else if(timeIndex == 3)
	{
		SS_Buf += 1;
		if(SS_Buf >= 60)
		{
			SS_Buf = 0;
		}
	}
}

void KeyAction(int code)
{
	if(code == 1)//B1
	{
		LED_Sta ^= 1;
		Beep(100);//伴随滴的一声
	}
	else if(code == 2)//B2
	{
		if(flagSet == 0)
		{
			flagSet = 1;
			
			LCD_Clear(Blue);
			HH_Buf = HH;
			MM_Buf = MM;
			SS_Buf = SS;
			
			timeIndex = 1;
		}
		else if(flagSet == 1)
		{
			flagSet = 0;
			
			LCD_Clear(Blue);
			
			timeIndex = 0;
		}
		Beep(100);//伴随滴的一声
	}
	else if(code == 3)//B3
	{
		if(flagSet)
		{
			timeIndex++;
			if(timeIndex > 3)
			{
				timeIndex = 1;
			}
			Beep(100);//伴随滴的一声
		}
		
	}
	else if(code == 4)//B4
	{
		if(flagSet)
		{
			IncTime();
			Beep(100);//伴随滴的一声
		}
		
	}
	
}

//
void Delay_Ms(u32 nTime)
{
	TimingDelay = nTime;
	while(TimingDelay != 0);	
}

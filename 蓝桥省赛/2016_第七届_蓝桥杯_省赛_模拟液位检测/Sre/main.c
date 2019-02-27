/*******************************************************************************
* 文件名：main.c
* 描  述：
* 作  者：CLAY
* 版本号：v1.0.0
* 日  期: 2019年2月27日
* 备  注：修改后的LCD例程
*         2016_第七届_蓝桥杯_省赛_模拟液位检测
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
u8 flag1s = 0;

//串口相关变量
u8 RxdCnt = 0;
u8 RxdOver = 0;
u8 RxdBuf[20];

//LED
u16 LED_MODE = 0XFFFF;


//ADC相关
float ADC_Val;

//阈值相关
u8 Thre1 = 30;
u8 Thre2 = 50;
u8 Thre3 = 70;
u8 LiquidHeight = 0;//液面高度
u8 LiquidLevel = 0;//液面所处的等级
u8 LiquidLevel_Buf = 0;
u8 selcIndex = 0;// 0-都不选中 1-选中Thre1 2-选中Thre2 3-选中Thre3
u8 flagLED2 = 0;//液面的等级变化
u8 LED2Cnt = 0;//LED2闪烁次数
u8 flagLED3 = 0;//液面的等级变化
u8 LED3Cnt = 0;//LED3闪烁次数

//函数声明
void UartAction(void);
void ShowRealData(void);
void ShowSetData(void);
void RefreshData(void);
void LevelChange(void);


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
	ADC1_Init();
	LED_Init();//LED配置
	USART2_Init();
	i2c_init();
	
	
	//初始上电从E2中读取数据到阈值变量中
	Thre1 = E2Read(0x00);
	Delay_Ms(5);
	Thre2 = E2Read(0x01);
	Delay_Ms(5);
	Thre3 = E2Read(0x02);
	Delay_Ms(5);
	
	
	while(1)
	{
		KeyDriver();
		
		if(flag1s)
		{
			flag1s = 0;
			LED_MODE ^= (1<<8);
			GPIOC-> ODR = LED_MODE;
			GPIOD-> ODR |= (1<<2);
			GPIOD-> ODR &=~(1<<2);
		}
				
		if(flag200ms)
		{
			flag200ms = 0;
			RefreshData();//200ms刷新一次数据
			
			if(flagSet == 0)
			{
				ShowRealData();
			}
			else if(flagSet == 1)
			{
				ShowSetData();
			}
			if(flagLED2)
			{
				LED_MODE ^= (1<<9);
				GPIOC-> ODR = LED_MODE;
				GPIOD-> ODR |= (1<<2);
				GPIOD-> ODR &=~(1<<2);
				LED2Cnt++;
				if(LED2Cnt >= 10)
				{
					LED2Cnt = 0;
					flagLED2 = 0;
				}
			}
			if(flagLED3)
			{
				LED_MODE ^= (1<<10);
				GPIOC-> ODR = LED_MODE;
				GPIOD-> ODR |= (1<<2);
				GPIOD-> ODR &=~(1<<2);
				LED3Cnt++;
				if(LED3Cnt >= 10)
				{
					LED3Cnt = 0;
					flagLED3 = 0;
				}
			}
		}
		
		if(RxdOver)//接收到以含有\n的字符串
		{
			RxdOver = 0;
			UartAction();//执行相应的串口动作
		}
		LevelChange();
	}
}

void LevelChange(void)//液面变化处理
{
	u8 i;
	
	if(LiquidLevel_Buf != LiquidLevel)
	{
		flagLED2 = 1;
		LED2Cnt = 0;
		
		if(LiquidLevel > LiquidLevel_Buf)
		{
			sprintf((char *)str, "A:H%.2d+L%d+U\r\n", LiquidHeight, LiquidLevel);
			USART2_SendString(str);
		}
		else
		{
			sprintf((char *)str, "A:H%.2d+L%d+D\r\n", LiquidHeight, LiquidLevel);
			USART2_SendString(str);
		}
		LiquidLevel_Buf = LiquidLevel;//更新液面高度缓冲区
		
		for(i=0; i<20; i++)//清空缓冲区
			RxdBuf[i] = 0;	
		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//接收的数据处理完毕后打开接收中断
	}
}

void RefreshData(void)
{
	ADC_Val = Get_ADC() * 3.3 / 4096; 
	
	LiquidHeight = ADC_Val * (100/3.3);
	
	if(LiquidHeight > Thre3)
	{
		LiquidLevel = 3;
	}
	else if(LiquidHeight > Thre2)
	{
		LiquidLevel = 2;
	}
	else if(LiquidHeight > Thre1)
	{
		LiquidLevel = 1;
	}
	else
	{
		LiquidLevel = 0;
	}
}

void ShowRealData(void)//显示实时数据界面
{	
	LCD_DisplayStringLine(Line1, (u8*)"    Liquid Level    ");
	
	sprintf((char *)str, "  Height: %d         ", LiquidHeight);
	LCD_DisplayStringLine(Line3, str);
	
	sprintf((char *)str, "  ADC   : %.2f       ", ADC_Val);
	LCD_DisplayStringLine(Line5, str);
	
	sprintf((char *)str, "  Level : %d         ", LiquidLevel);
	LCD_DisplayStringLine(Line7, str);
}

void ShowSetData(void)//参数设置界面
{
	LCD_SetTextColor(White);
	
	LCD_DisplayStringLine(Line1, (u8*)"  Parameter Setup   ");
	
	if(selcIndex == 1)
		LCD_SetTextColor(Green);
	else
		LCD_SetTextColor(White);
	sprintf((char *)str, "  Threshold 1: %d     ", Thre1);
	LCD_DisplayStringLine(Line3, str);
	
	if(selcIndex == 2)
		LCD_SetTextColor(Green);
	else
		LCD_SetTextColor(White);
	sprintf((char *)str, "  Threshold 2: %d     ", Thre2);
	LCD_DisplayStringLine(Line5, str);
	
	if(selcIndex == 3)
		LCD_SetTextColor(Green);
	else
		LCD_SetTextColor(White);
	sprintf((char *)str, "  Threshold 3: %d     ", Thre3);
	LCD_DisplayStringLine(Line7, str);
	
	LCD_SetTextColor(White);
}

void UartAction(void)
{
	u8 i;

	if((RxdBuf[0]=='C') || (RxdBuf[0]=='S'))
	{
		flagLED3 = 1;
		LED3Cnt = 0;
	}
	
	if(RxdBuf[0] == 'C')
	{
		sprintf((char *)str, "C:H%.2d+L%d\r\n", LiquidHeight, LiquidLevel);
		USART2_SendString(str);
	}
	else if(RxdBuf[0] == 'S')
	{
		sprintf((char *)str, "S:TL%.2d+TM%.2d+TH%.2d\r\n", Thre1, Thre2, Thre3);
		USART2_SendString(str);
	}
	
	for(i=0; i<20; i++)//清空缓冲区
		RxdBuf[i] = 0;	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//接收的数据处理完毕后打开接收中断
}

void IncThre(void)
{
	if(selcIndex == 1)
	{
		Thre1++;
		if(Thre1 >= 95)
		{
			Thre1 = 95;
		}
	}
	else if(selcIndex == 2)
	{
		Thre2++;
		if(Thre2 >= 95)
		{
			Thre2 = 95;
		}
	}
	else if(selcIndex == 3)
	{
		Thre3++;
		if(Thre3 >= 95)
		{
			Thre3 = 95;
		}
	}
}

void DecThre(void)
{
	if(selcIndex == 1)
	{
		Thre1--;
		if(Thre1 <= 5)
		{
			Thre1 = 5;
		}
	}
	else if(selcIndex == 2)
	{
		Thre2--;
		if(Thre2 <= 5)
		{
			Thre2 = 5;
		}
	}
	else if(selcIndex == 3)
	{
		Thre3--;
		if(Thre3 <= 5)
		{
			Thre3 = 5;
		}
	}
}

void KeyAction(int code)
{
	if(code == 1)//B1
	{
		if(flagSet == 0)
		{
			LCD_Clear(Blue);
			
			
			selcIndex = 1;//高亮Thre1
			flagSet = 1;//进入设置界面
		}
		else
		{
			LCD_Clear(Blue);
			
			//退出设置界面前保存当前的数据到变量中
			E2Write(0x00, Thre1);
			Delay_Ms(5);
			E2Write(0x01, Thre2);
			Delay_Ms(5);
			E2Write(0x02, Thre3);
			Delay_Ms(5);
			
			flagSet = 0;//退出设置界面
		}
//		Beep(100);//伴随滴的一声
	}
	else if(code == 2)//B2
	{
		if(flagSet == 1)
		{
			selcIndex++;
			if(selcIndex > 3)
			{
				selcIndex = 1;
			}
		}
//		Beep(100);//伴随滴的一声
	}
	else if(code == 3)//B3
	{
		if(flagSet == 1)
		{
			IncThre();
		}
//		Beep(100);//伴随滴的一声
		
	}
	else if(code == 4)//B4
	{
		if(flagSet == 1)
		{
			DecThre();
		}
//		Beep(100);//伴随滴的一声
		
	}
}

//
void Delay_Ms(u32 nTime)
{
	TimingDelay = nTime;
	while(TimingDelay != 0);	
}

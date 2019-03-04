/*******************************************************************************
* 文件名：main.c
* 描  述：
* 作  者：CLAY
* 版本号：v1.0.0
* 日  期: 2019年3月2日
* 备  注：修改后的LCD例程
*         2015_第六届_蓝桥杯_省赛_双通道方波 频率检测与倍频 输出
*		  PWM输出有瑕疵，具体原因待找...
*		  输入通道 PA1-TIM2_CH2 对应输出 PA6-TIM3_CH1
*		  输入通道 PA2-TIM2_CH3 对应输出 PA7_TIM3_CH2
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
u8 str[20];//字符串公用的数组
u8 flag200ms = 1;//200ms定时到标志
u8 setMode = 0;//设置模式: 0-本地设定 1-串口设定
u8 selChannel = 0; //选择输入通道 0-输入通道1 2-输入通道1
u8 flag1s = 0;

//串口相关变量
u8 RxdCnt = 0;
u8 RxdOver = 0;
u8 RxdBuf[20];

//LED
u16 LED_MODE = 0XFFFF;

//信号捕获相关变量
extern u32 TIM2_CH2_CAPTURE_HL;
extern u8 TIM2_CH2_CAPTURE_MODE;
extern u32 TIM2_CH3_CAPTURE_HL;
extern u8 TIM2_CH3_CAPTURE_MODE;

//信号输出相关变量
u8 Channal_1_Enable = 1;
u8 Channal_2_Enable = 1;
u32 Channal_1_Frequency = 0;
u32 Channal_2_Frequency = 0;

u8 CH1_N = 2;//通道1倍频数
u8 CH2_N = 2;//通道2倍频数
u8 CH1_NBuf = 2;
u8 CH2_NBuf = 2;

//函数声明
void UartAction(void);
void ShowRealData(void);
void ShowSetData(void);
void RefreshLed(void);
void RefreshN(void);

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
	
	PWM_IC_Init();//PWM输入捕获
	TIM3_OUTPUT_Init(1000, 500, 1, 1, 1);//PWM输出
	
	CH1_N = E2Read(0x00);
	Delay_Ms(5);
	CH2_N = E2Read(0x01);
	Delay_Ms(5);
	
	while(1)
	{
		KeyDriver();
		
		
		if(TIM2_CH2_CAPTURE_MODE == 2)
		{
			Channal_1_Frequency = 1000000 / TIM2_CH2_CAPTURE_HL;
			if((Channal_1_Frequency < 50) || (Channal_1_Frequency > 50000))
			{
					Channal_1_Enable = 0;
			}else
			{
					Channal_1_Enable = 1;
			}
			TIM2_CH2_CAPTURE_MODE = 0;
		}
		if(TIM2_CH3_CAPTURE_MODE == 2)
		{
				Channal_2_Frequency = 1000000 / TIM2_CH3_CAPTURE_HL;
				if((Channal_2_Frequency < 50) || (Channal_2_Frequency > 50000))
				{
						Channal_2_Enable = 0;
				}else
				{
						Channal_2_Enable = 1;
				}
				TIM2_CH3_CAPTURE_MODE = 0;
		}
		
		if(flag200ms)
		{
			flag200ms = 0;
			RefreshLed();
			ShowRealData();

			RefreshN();//刷新倍频数
		}
		if(flag1s)
		{
			flag1s = 0;
			TIM3_OUTPUT_Init(Channal_1_Frequency*CH1_N, Channal_2_Frequency*CH2_N, 0, Channal_1_Enable, Channal_2_Enable);
		}
		
		if(setMode == 1)
		{
			if(RxdOver)//接收到以含有\n的字符串
			{
				RxdOver = 0;
				UartAction();//执行相应的串口动作
			}
		}
	}
}

void RefreshN(void)
{
	if(CH1_N != CH1_NBuf)
	{
		CH1_NBuf = CH1_N;
		E2Write(0x00, CH1_NBuf);
		Delay_Ms(5);
	}
	if(CH2_N != CH2_NBuf)
	{
		CH2_NBuf = CH2_N;
		E2Write(0x01, CH2_NBuf);
		Delay_Ms(5);
	}
}

void RefreshLed(void)//设置相应的LED状态
{
	if(Channal_1_Enable) 
	{
		LED_MODE &= ~(1<<8); 
	}
	else 
	{
		LED_MODE |= (1<<8); 
	}
	
	if(Channal_2_Enable)
	{
		LED_MODE &= ~(1<<9); 
	}
	else
	{
		LED_MODE |= (1<<9); 
	}
	
	if(setMode == 1)
	{
		LED_MODE &= ~(1<<10); 
	}
	else
	{
		LED_MODE |= (1<<10); 
	}
	
	GPIOC->ODR = LED_MODE;
	GPIOD->ODR |= (1<<2);
	GPIOD->ODR &=~(1<<2);	
}

void ShowRealData(void)//显示实时数据界面
{	
	
	sprintf((char *)str, " Channel(1): %d   ", Channal_1_Frequency);
	LCD_DisplayStringLine(Line1, str);

	sprintf((char *)str, " N(1)      : %d   ", CH1_N);
	LCD_DisplayStringLine(Line3, str);
	
	sprintf((char *)str, " Channel(2): %d   ", Channal_2_Frequency);
	LCD_DisplayStringLine(Line5, str);
	
	sprintf((char *)str, " N(2)      : %d   ", CH2_N);
	LCD_DisplayStringLine(Line7, str);
	
	sprintf((char *)str, "                 %d  ", selChannel+1);
	LCD_DisplayStringLine(Line9, str);
	
}

void UartAction(void)
{
	u8 i;
	
	if(RxdBuf[4] == '0')
	{
		if(RxdBuf[6]=='1' && RxdBuf[7]=='0')
		{
			CH1_N = 10;
		}
		else
		{
			CH1_N = RxdBuf[6] - '0';
		}
	}
	else if(RxdBuf[4] == '1')
	{
		if(RxdBuf[6]=='1' && RxdBuf[7]=='0')
		{
			CH2_N = 10;
		}
		else
		{
			CH2_N = RxdBuf[6] - '0';
		}
	}
	
	for(i=0; i<20; i++)//清空缓冲区
		RxdBuf[i] = 0;	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//接收的数据处理完毕后打开接收中断
}


void KeyAction(int code)
{
	if(code == 1)//B1
	{
		setMode ^= 1;
		Beep(100);//伴随滴的一声
	}
	else if(code == 2)//B2
	{
		if(setMode == 0)//除以本地设定模式才可以进行切换通道
		{
			selChannel ^= 1;//切换输入通道
		}
		Beep(100);//伴随滴的一声
	}
	else if(code == 3)//B3
	{
		if(setMode == 0)//处于本地设定模式
		{
			if(selChannel == 0)//选择的是通道1
			{
				CH1_N--;
				if(CH1_N < 1)
				{
					CH1_N = 1;
				}
			}
			else if(selChannel == 1)
			{
				CH2_N--;
				if(CH2_N < 1)
				{
					CH2_N = 1;
				}
			}
		}
		Beep(100);//伴随滴的一声
		
	}
	else if(code == 4)//B4
	{
		if(setMode == 0)//处于本地设定模式
		{
			if(selChannel == 0)//选择的是通道1
			{
				CH1_N++;
				if(CH1_N >= 10)
				{
					CH1_N = 10;
				}
			}
			if(selChannel == 1)
			{
				CH2_N++;
				if(CH2_N >= 10)
				{
					CH2_N = 10;
				}
			}
		}
		Beep(100);//伴随滴的一声
		
	}
}

//
void Delay_Ms(u32 nTime)
{
	TimingDelay = nTime;
	while(TimingDelay != 0);	
}

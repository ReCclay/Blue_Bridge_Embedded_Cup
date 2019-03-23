/*******************************************************************************
* 文件名：main.c
* 描  述：
* 作  者：CLAY
* 版本号：v1.0.0
* 日  期: 2019年3月23日
* 备  注：第九届赛题再做
*         
*		 
*******************************************************************************
*/

#include "stm32f10x.h"
#include "lcd.h"
#include "led.h"
#include "timer.h"
#include "key.h"
#include "i2c.h"
#include "e2prom.h"
#include "stdio.h"
#include "pwm.h"


//全局变量
u32 TimingDelay = 0;
u8 str[20];
u8 flag200ms = 0;
u8 flag1s = 0;
u8 flag500ms = 0;


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

//存储状态相关
u8 saveIndex = 0;//5个存储位置(0-4)
u8 hour=1, min=4, sec=2; //储存的5个定时时间,这个用来实时显示
u8 hourBuf=1, minBuf=2, secBuf=3;//这个用来存储
u8 timeIndex = 0;//时分秒索引高亮 1-时 2-分 3-秒 0-都不高亮
u8 saveSta = 0;//电子定时器的4种工作状态 0-停止Standby 1-设置Setting 2-运行Running 3-暂停Pause


//函数声明
void Delay_Ms(u32 nTime);
void Show(void);
void ReadTimeData(void);
void RefreshTime(void);
void RefreshLed(u8 sta);


//Main Body
int main(void)
{
	
	STM3210B_LCD_Init();
	LCD_Clear(Blue);
	LCD_SetBackColor(Blue);
	LCD_SetTextColor(White);
	SysTick_Config(SystemCoreClock/1000);


	//初始化函数
	LED_Init();
	TIM4_Init(2000, 72);
	KEY_Init();
	i2c_init();
	PWM_Init(0);

	ReadTimeData();//读取E2中的定时时间
	hour = hourBuf;
	min = minBuf;
	sec = secBuf;

	while(1)
	{
		KeyDriver();
		
		if(flag200ms)
		{
			flag200ms = 0;
			Show();
			
			if(saveSta == 2)//处于运行状态
			{
				PWM_Init(1);//输出PWM
				RefreshLed(1);//0.5s闪烁LED
			}
			else
			{
				PWM_Init(0);//停止输出PWM
				RefreshLed(0);//关闭LED
			}
			
		}
		if(flag1s)
		{
			flag1s = 0;
			if(saveSta == 2)
			{
				RefreshTime();
			}
		}
	}
}

void RefreshLed(u8 sta)
{
	static u8 ledSta = 0;
	
	if(sta)
	{
		if(flag500ms)
		{
			flag500ms = 0;
			
			if(ledSta)
			{
				GPIOD->ODR |= (1<<2);
				GPIOC->ODR = 0xFF00;
				GPIOD->ODR &=~(1<<2);
				ledSta = 0;
			}
			else
			{
				GPIOD->ODR |= (1<<2);
				GPIOC->ODR = 0xFE00;
				GPIOD->ODR &=~(1<<2);
				ledSta = 1;
			}
		}
	}
	else
	{
		GPIOD->ODR |= (1<<2);
		GPIOC->ODR = 0xFF00;
		GPIOD->ODR &=~(1<<2);
	}
}

void RefreshTime(void)
{
	int time;
	
	time = hour * 3600 + min * 60 + sec;
	
	time--;
	if(time == 0)
	{
		time = 0;
	}
	
	hour = time / 3600;
	min  = time % 3600 / 60;
	sec  = time % 3600 % 60;
}

void Show(void)
{
	//索引位置
	sprintf((char *)str, "  No %d   ", saveIndex);
	LCD_DisplayStringLine(Line2, str);
	
	//定时器时间设置
	if(timeIndex == 1)
	{
		LCD_SetTextColor(Green);
	}
	else
	{
		LCD_SetTextColor(White);
	}	
	LCD_DisplayChar(Line5, 250, hour/10+'0');
	LCD_DisplayChar(Line5, 235, hour%10+'0');
	
	LCD_SetTextColor(White);
	LCD_DisplayChar(Line5, 220, ':');
	
	
	if(timeIndex == 2)
	{
		LCD_SetTextColor(Green);
	}
	else
	{
		LCD_SetTextColor(White);
	}
	LCD_DisplayChar(Line5, 195, min/10+'0');
	LCD_DisplayChar(Line5, 180, min%10+'0');
	
	LCD_SetTextColor(White);
	LCD_DisplayChar(Line5, 165, ':');
	
	if(timeIndex == 3)
	{
		LCD_SetTextColor(Green);
	}
	else
	{
		LCD_SetTextColor(White);
	}
	LCD_DisplayChar(Line5, 140, sec/10+'0');
	LCD_DisplayChar(Line5, 125, sec%10+'0');
	
	LCD_SetTextColor(White);
	
	
	//系统状态
	if(saveSta == 0)
	{
		LCD_DisplayStringLine(Line8, (u8*)"     Standby  ");
	}
	else if(saveSta == 1)
	{
		LCD_DisplayStringLine(Line8, (u8*)"     Setting  ");
	}
	else if(saveSta == 2)
	{
		LCD_DisplayStringLine(Line8, (u8*)"     Running  ");
	}
	else if(saveSta == 3)
	{
		LCD_DisplayStringLine(Line8, (u8*)"     Pause  ");
	}
}

//保存时间到E2中
void WriteTimeData(void)
{
	E2Write(saveIndex * 3, hourBuf);
	Delay_Ms(5);
	E2Write(saveIndex * 3 + 1, minBuf);
	Delay_Ms(5);
	E2Write(saveIndex * 3 + 2, secBuf);
	Delay_Ms(5);
}

void ReadTimeData(void)
{
	hourBuf = E2Read(saveIndex * 3);
	Delay_Ms(5);
	minBuf = E2Read(saveIndex * 3 + 1);
	Delay_Ms(5);
	secBuf = E2Read(saveIndex * 3 + 2);
	Delay_Ms(5);
}

void IncTime(void)
{
	if(timeIndex == 1)
	{
		hour++;
		if(hour >= 24)
		{
			hour = 0;
		}
	}
	else if(timeIndex == 2)
	{
		min++;
		if(min >= 59)
		{
			min = 0;
		}
	}
	else if(timeIndex == 3)
	{
		sec++;
		if(sec >= 59)
		{
			sec = 0;
		}
	}
}

//sta为0表示短按 1-表示长按
void KeyAction(u8 code, u8 sta)
{
	if(sta == 0)//短按
	{
		if(code == 1)
		{
			if(saveSta == 0)//在停止模式下切换索引
			{
				saveIndex++;
				if(saveIndex > 4)
				{
					saveIndex = 0;
				}
				
				ReadTimeData();
				hour = hourBuf;
				min = minBuf;
				sec = secBuf;
				
			}
			
		}
		else if(code == 2)//短按B2进入设置状态
		{
			saveSta = 1;//进入设置状态
			timeIndex++;//高亮时->分->秒的顺序
			if(timeIndex > 3)
			{
				timeIndex = 1;
			}
			
		}
		else if(code == 3)
		{
			if(saveSta == 1)//处在设置状态
			{
				IncTime();
			}
		}
		else if(code == 4)
		{
			if(saveSta == 0)//处在停止模式下
			{
				saveSta = 2;//设置为运行模式
			}
			else if(saveSta == 2)//处在运行模式下
			{
				saveSta = 3;//暂停定时器
			}
			else if(saveSta == 3)//处在暂停模式下
			{
				saveSta = 2;//恢复运行模式
			}
			else if(saveSta == 1)//处于设置模式下(临时设置时间)
			{
				saveSta = 2;//临时设置完毕后，直接切换到运行模式，但是不保存到E2中
				timeIndex = 0;//取消高亮
			}
		}
	}
	else if(sta == 1)//长按
	{
		if(code == 2)
		{
			hourBuf = hour;//退出设置模式前，更新参数
			minBuf = min;
			secBuf = sec;
			
			saveSta = 0;//进入停止模式
			timeIndex = 0;//去除高亮
			
			WriteTimeData();//写入设置时间到E2中
		}
		else if(code == 3)
		{
			if(saveSta == 1)//处在设置模式下
			{
				IncTime();
			}
		}
		else if(code == 4)
		{
			if((saveSta==2) || (saveSta==3))//处在运行模式和暂停模式
			{
				saveSta = 0;//变为停止模式
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

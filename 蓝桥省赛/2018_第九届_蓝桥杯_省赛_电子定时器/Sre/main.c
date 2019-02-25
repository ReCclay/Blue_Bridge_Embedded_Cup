/*******************************************************************************
* 文件名：main.c
* 描  述：
* 作  者：CLAY
* 版本号：v1.0.0
* 日  期: 2019年2月24日
* 备  注：修改后的LCD例程
*         2018_第九届_蓝桥杯_省赛_电子定时器
*		  实现所有功能
*		  长按动作伴随有蜂鸣器100ms的声响
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
u8 flag500ms = 0;
u8 flag200ms = 0;//200ms定时到标志
u8 flag1s = 0; //用来定时器时间的倒计时
u8 flagMode = 0;//设置模式 0-停止模式(Standby) 1-设置模式(Setting) 2-运行模式(Running) 3-暂停(Pause)
u8 str[20];//字符串公用的数组
u8 saveIndex = 0; //存储数据索引 共5个数据(0-4)
u8 timeIndex = 0; //高亮某一位索引 0-没有高亮 1-时 2-分 3-秒

//LED
u16 LED_MODE = 0XFFFF;

//倒计时相关变量
int secs=0;//时间换算成的秒数
u8 hour=0, min=2, sec=1;//时间对应的时分秒

//函数声明
void Show(void);
void ReadE2Time(void);
void TimeToSecs(void);
void SecsToTime(void);


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
	LED_Init();//LED配置
	i2c_init();//E2PROM必须先初始化I2C
	
	//初始化倒计时参数
	ReadE2Time();//读取E2中存储的位置0的时间参数
	
	while(1)
	{
		KeyDriver();
		
		if(flag200ms)
		{
			flag200ms = 0;
			Show();
		}
		if(flag1s)
		{
			flag1s = 0;
			if(flagMode == 2)//处于运行模式
			{
				TimeToSecs();
				secs--;
				if(secs <= 0)
					secs = 0;
				SecsToTime();
			}
		}
	}
}

/*把时间转换成秒的形式，方便倒计时*/
void TimeToSecs(void)
{
	secs = hour*3600 + min*60 + sec;
}

/*把秒转换成时间的形式，方便显示和存储*/
void SecsToTime(void)
{
	hour = secs / 3600;
	min  = (secs % 3600) / 60;
	sec  = (secs % 3600) % 60;
}

void RefreshTime(void)
{
	if(timeIndex == 1)
		LCD_SetTextColor(Green);
	else
		LCD_SetTextColor(White);
	LCD_DisplayChar(Line4, 240, hour/10 + '0');//对应的数字转化为ASCII
	LCD_DisplayChar(Line4, 225, hour%10 + '0');
	
	LCD_SetTextColor(White);
	LCD_DisplayChar(Line4, 210, ':');
	
	if(timeIndex == 2)
		LCD_SetTextColor(Green);
	else
		LCD_SetTextColor(White);
	LCD_DisplayChar(Line4, 195, min/10 + '0');
	LCD_DisplayChar(Line4, 180, min%10 + '0');
	
	LCD_SetTextColor(White);
	LCD_DisplayChar(Line4, 165, ':');
	
	if(timeIndex == 3)
		LCD_SetTextColor(Green);
	else
		LCD_SetTextColor(White);
	LCD_DisplayChar(Line4, 150, sec/10 + '0');
	LCD_DisplayChar(Line4, 135, sec%10 + '0');
	
	LCD_SetTextColor(White);
	
}

void Show(void)
{
	sprintf((char*)str, "  No %d       ", saveIndex);
	LCD_DisplayStringLine(Line1, str);
	
	RefreshTime();
	
	switch(flagMode)
	{
		case 0: LCD_DisplayStringLine(Line7, (u8*)"     Standby     "); break;
		case 1: LCD_DisplayStringLine(Line7, (u8*)"     Setting     "); break;
		case 2: LCD_DisplayStringLine(Line7, (u8*)"     Running     "); break;
		case 3: LCD_DisplayStringLine(Line7, (u8*)"     Pause       "); break;
		default: break;
	}
	
	if(flagMode == 2)//处于运行模式输出PWM和闪烁LED1
	{
		/*PA6输出1K 80%的PWM信号*/
		PWM_Init(1000, 80, 1); //允许使能PWM波
		
		if(flag500ms == 1)
		{
			flag500ms = 0;
			
			LED_MODE ^= (1<<8);
			GPIOC-> ODR = LED_MODE;
			GPIOD-> ODR |= (1<<2);
			GPIOD-> ODR &=~(1<<2);
		}
	}
	else
	{
		/*PA6输出1K 80%的PWM信号*/
		PWM_Init(1000, 80, 0);//禁止使能PWM波
		LED_MODE |= (1<<8);
		GPIOC-> ODR = LED_MODE;
		GPIOD-> ODR |= (1<<2);
		GPIOD-> ODR &=~(1<<2);
	}
}


void ReadE2Time()
{
	switch(saveIndex)
	{
		case 0: hour = E2Read(0x00); Delay_Ms(5);
				min  = E2Read(0x01); Delay_Ms(5);
				sec  = E2Read(0x02); Delay_Ms(5);
				break;
		case 1: hour = E2Read(0x03); Delay_Ms(5);
			    min  = E2Read(0x04); Delay_Ms(5);
		        sec  = E2Read(0x05); Delay_Ms(5);
		        break;
		case 2: hour = E2Read(0x06); Delay_Ms(5);
			    min  = E2Read(0x07); Delay_Ms(5);
		        sec  = E2Read(0x08); Delay_Ms(5);
		        break;
		case 3: hour = E2Read(0x09); Delay_Ms(5);
			    min  = E2Read(0x0A); Delay_Ms(5);
		        sec  = E2Read(0x0B); Delay_Ms(5);
		        break;
		case 4: hour = E2Read(0x0C); Delay_Ms(5);
			    min  = E2Read(0x0D); Delay_Ms(5);
		        sec  = E2Read(0x0E); Delay_Ms(5);
		        break;
		default: break;
	}
}

void WriteE2Time()
{
	switch(saveIndex)
	{
		case 0: E2Write(0x00, hour); Delay_Ms(5);
				E2Write(0x01, min ); Delay_Ms(5);
				E2Write(0x02, sec ); Delay_Ms(5);
				break; 
		case 1: E2Write(0x03, hour); Delay_Ms(5);
			    E2Write(0x04, min ); Delay_Ms(5);
		        E2Write(0x05, sec ); Delay_Ms(5);
		        break; 
		case 2: E2Write(0x06, hour); Delay_Ms(5);
			    E2Write(0x07, min ); Delay_Ms(5);
		        E2Write(0x08, sec ); Delay_Ms(5);
		        break; 
		case 3: E2Write(0x09, hour); Delay_Ms(5);
			    E2Write(0x0A, min ); Delay_Ms(5);
		        E2Write(0x0B, sec ); Delay_Ms(5);
		        break; 
		case 4: E2Write(0x0C, hour); Delay_Ms(5);
			    E2Write(0x0D, min ); Delay_Ms(5);
		        E2Write(0x0E, sec ); Delay_Ms(5);
		        break;
		default: break;
	}
}

void IncTime()
{
	if(timeIndex == 1)
	{
		hour += 1;
		if(hour >= 60)
		{
			hour = 0;
		}
	}
	else if(timeIndex == 2)
	{
		min += 1;
		if(min >= 60)
		{
			min = 0;
		}
	}
	else if(timeIndex == 3)
	{
		sec += 1;
		if(sec >= 60)
		{
			sec = 0;
		}
	}
}

void KeyAction(int code, u8 sta)
{
	/*短按对应的动作*/
	if(sta == 0)
	{
		if(code == 1)//按下B1
		{
			if(flagMode == 0)//处于停止模式才能切换存储位置
			{
				saveIndex++;
				if(saveIndex > 5)
				{
					saveIndex = 0;
				}
				ReadE2Time();
			}
		}
		else if(code == 2)//B2
		{
			if(flagMode == 0)//是停止模式
			{
				flagMode = 1;//按下后切换到设置模式
				timeIndex = 1;//把时间高亮索引放到"时"上
			}
			else if(flagMode == 1)//是设置模式
			{
				timeIndex++;
				if(timeIndex > 3)
				{
					timeIndex = 1;
				}
			}
			
		}
		else if(code == 3)//B3
		{
			if(flagMode == 1)
			{
				IncTime();
			}
		}
		else if(code == 4)//B4
		{
			if(flagMode == 0)//处于停止模式短按,立即启动定时器
			{
				flagMode = 2;
			}
			else if(flagMode == 1)//处于设置模式按下启动，不存储数据直接启动
			{
				flagMode = 2;//进入运行模式
				timeIndex = 0;//去除所有高亮
			}
			else if(flagMode == 2)//运行期间短按
			{
				flagMode = 3;//进入暂停模式
			}
			else if(flagMode == 3)//处于暂停模式短按，恢复定时器倒计时
			{
				flagMode = 2;
			}
		}
	}
	/*长按对应的动作*/
	else if(sta == 1)
	{
		if(code == 2)//长按B2
		{
			if(flagMode == 1)//处于设置模式
			{
				WriteE2Time();//退出前保存设置的时间数据到E2中
				flagMode = 0;//进入停止模式
				timeIndex = 0;//去除所有高亮
				Beep(100);//伴随滴的一声
			}
		}
		else if(code == 3)//B3
		{
			if(flagMode == 1)
			{
				IncTime();
				Beep(100);//伴随滴的一声
			}
		}
		else if(code == 4)//B4
		{
			if((flagMode==2) || (flagMode==3))//处于运行或者暂停模式长按则进入停止模式
			{
				flagMode = 0;
				Beep(100);//伴随滴的一声
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

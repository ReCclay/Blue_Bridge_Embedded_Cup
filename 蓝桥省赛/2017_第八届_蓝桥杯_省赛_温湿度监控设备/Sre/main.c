/*******************************************************************************
* 文件名：main.c
* 描  述：
* 作  者：CLAY
* 版本号：v1.0.0
* 日  期: 2019年2月20日-2019年2月22日
* 备  注：修改后的LCD例程
*         2017_第八届_蓝桥杯_省赛_温湿度监控设备
*		  实现题设所有功能
*		  附加：加入按键时伴随的蜂鸣器声音
*		  		加入长按键可连续加减
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

//串口相关变量
u8 RxdCnt = 0;
u8 RxdOver = 0;
u8 RxdBuf[20];

//LED
u16 LED_MODE = 0XFFFF;

//参数设置界面相关变量
int TempThre = 40;//温度阈值 40℃ //注意温度是有负值的
u8 HumiThre = 80;//湿度阈值 80%
u8 SampleTime= 1; //采样间隔 1S
float TestFreq = 1.5;//测试信号频率 1.5KHz
//缓冲区变量
int TempThre_Buf = 40;
u8 HumiThre_Buf = 80;
u8 SampleTime_Buf = 1; 
float TestFreq_Buf = 1.5;

//实时显示界面相关变量
float Temp, Humi;
float Temp_Buf, Humi_Buf;
u8 SelIndex = 0;//设置参数界面行索引(用来突出显示) 0-温度 1-湿度 2-采样 3-频率

//RTC相关变量
u32 time;
u8 HH, MM, SS;
u8 HH_Buf, MM_Buf, SS_Buf;
u8 RTC_Flag = 1;

//采样相关变量
int Record[70][5];
u16 RecordCnt = 0;
u8 flagSampleTime = 0;

//输入捕获PA1频率
extern __IO uint32_t TIM3Freq;

void RefreshTempHumi(void);//200ms刷新一次温度和湿度
void ShowRealData(void);
void ShowSetDat(void);
void ShowLedThre(void);
void UartAction(void);
void RecordAction(void);
void ReadE2Data(void);


//Main Body
int main(void)
{	
	STM3210B_LCD_Init();
	LCD_Clear(Blue);
	LCD_SetBackColor(Blue);
	LCD_SetTextColor(White);
	SysTick_Config(SystemCoreClock/1000);
	PWM_IC_Init();//PA7 输入捕获
	TIM4_Init(2000, 72);//定时2ms
	KeyInit();
	BeepInit();
	RTC_Init(9, 40, 0);
	ADC1_Init();
	LED_Init();//LED配置
	USART2_Init();
	i2c_init();
	
	
	ReadE2Data();//读取E2中原本存取的数据，存到相应的缓冲区
	
	//把缓冲区的数据更新给原值
	//注意别忘了这里也需要再赋值
	TempThre = TempThre_Buf;
	HumiThre = HumiThre_Buf;
	SampleTime = SampleTime_Buf;
	TestFreq = TestFreq_Buf;
	//产生对应频率的PWM
	PWM_OC_Init(TestFreq_Buf*1000,50);//注意这里要乘以1000
	
	
	while(1)
	{
		KeyDriver();
		
		if(flag200ms)
		{
			flag200ms = 0;
			RefreshTempHumi();//刷新温度和湿度
			if(!flagSet)
			{
				ShowRealData();//实时数据显示界面
			}
			else
			{
				ShowSetDat();//参数设置界面
			}
			ShowLedThre();//LED1和LED3对温度和湿度的阈值检测 要注意在设置界面下，不要让设置的阈值实时生效，也就是说实时值是和Buf的值进行比较的。
		}
		
		if(RxdOver)//接收到以含有\n的字符串
		{
			RxdOver = 0;
			UartAction();//执行相应的串口动作
		}
		
		if(flagSampleTime)//每SampleTime记录一次
		{
			flagSampleTime = 0;
			RecordAction();//记录数据的动作
		}
	}
}
/*务必注意连续读E2中的数据要记得延时5ms*/
void ReadE2Data(void)
{
	TempThre_Buf = E2Read(0x01);
	Delay_Ms(5);
	if(E2Read(0x00))
	{
		Delay_Ms(5);
		TempThre_Buf = -TempThre_Buf;
	}
	HumiThre_Buf = E2Read(0x02);
	Delay_Ms(5);
	SampleTime_Buf = E2Read(0x03);
	Delay_Ms(5);
	TestFreq_Buf = (float)E2Read(0x04) / 10;
	Delay_Ms(5);
}

/*务必注意连续写E2中的数据要记得延时5ms*/
void WriteE2Dta(void)
{
	//注意往E2PROM里面写的是缓冲区值，原值是用来实时显示到LCD上的
	if(TempThre_Buf < 0)
	{
		E2Write(0x00, 1);//0x00存储温度的符号-负值为1，正值为0
		Delay_Ms(5);
		E2Write(0x01, (u8)(-TempThre_Buf));//0x01存温度的设置值
	}
	else
	{
		E2Write(0x00, 0);
		Delay_Ms(5);
		E2Write(0x01, (u8)TempThre_Buf);//0x01存温度的设置值
	}
	Delay_Ms(5);
	E2Write(0x02, HumiThre_Buf);//0x02存湿度的设置值
	Delay_Ms(5);
	E2Write(0x03, SampleTime_Buf);//0x03存采样间隔的设置值
	Delay_Ms(5);
	E2Write(0x04, (u8)(TestFreq_Buf*10));//0x04存测试频率的设置值 注意放大了10倍
	Delay_Ms(5);
}

void ShowLedThre(void)//LED在温度和湿度超过阈值后的相应状态 200ms扫一次
{
	if(Temp > TempThre_Buf)//L1 注意是与Buf值比较
	{
		LED_MODE ^= (1<<8);
	}
	else//保证小于的时候是熄灭状态
	{
		LED_MODE |= (1<<8);
	}
	
	if(Humi > HumiThre_Buf)//L2 注意是与Buf值比较
	{
		LED_MODE ^= (1<<9);
	}
	else//保证小于的时候是熄灭状态
	{
		LED_MODE |= (1<<9);
	}
	GPIOC-> ODR = LED_MODE;
	GPIOD-> ODR |= (1<<2);
	GPIOD-> ODR &=~(1<<2);
}

void RefreshTempHumi(void)//更新温度和湿度
{
	float ADC_Val;//必须定义成float
	
	//更新温度
	ADC_Val = Get_ADC() * 3.3 / 4096;
	Temp = (80 * ADC_Val / 3.3) - 20;
	
	//更新湿度
	Humi = (80 * TIM3Freq / 1000 + 10) / 9;//注意分析题设，有必要的用PC的计算器演算一下，除以1000别忘了。
}

void ShowRealData(void)//显示实时数据界面
{	
	LCD_SetTextColor(White);//注意这里要加上该界面下的字体显示颜色，不然从第二个界面
	
	LCD_DisplayStringLine(Line1, (u8 *)"     Real Data     ");
	
	sprintf((char *)str, "  Temp:  %.0fC         ", Temp);//还要注意浮点数，必须指明小数点后几位！
	LCD_DisplayStringLine(Line3, str);
	
	sprintf((char *)str, "  Humi:  %.0f%%        ", Humi);//注意百分号怎么打出来%%
	LCD_DisplayStringLine(Line5, str);
	
	sprintf((char *)str, "  Time: %.2d-%.2d-%.2d       ", HH, MM, SS);//这里加上格式控制符.2d是最好的！
	LCD_DisplayStringLine(Line7, str);
	
	sprintf((char *)str, "      RecordCnt: %d     ", RecordCnt);
	LCD_DisplayStringLine(Line9, str);
}

void ShowSetDat(void)//参数设置界面
{
	LCD_SetTextColor(White);//注意这里要加上第0行显示的颜色，不然选中最后一行的时候，第0行也会高亮
	
	LCD_DisplayStringLine(Line1, (u8 *)"     Set Data     ");
	
	if(SelIndex == 0)
		LCD_SetTextColor(Green);
	else
		LCD_SetTextColor(White);
	sprintf((char *)str, "  TempThre: %dC ", TempThre);
	LCD_DisplayStringLine(Line3, str);
	
	if(SelIndex == 1)
		LCD_SetTextColor(Green);
	else
		LCD_SetTextColor(White);
	sprintf((char *)str, "  HumiThre: %d%%  ", HumiThre);
	LCD_DisplayStringLine(Line5, str);
	
	if(SelIndex == 2)
		LCD_SetTextColor(Green);
	else
		LCD_SetTextColor(White);
	sprintf((char *)str, "  SampleTime: %dS  ", SampleTime);
	LCD_DisplayStringLine(Line7, str);
	
	if(SelIndex == 3)
		LCD_SetTextColor(Green);
	else
		LCD_SetTextColor(White);
	sprintf((char *)str, "  TestFreq: %.1fKHz  ", TestFreq);
	LCD_DisplayStringLine(Line9, str);
}

void RecordAction(void)
{
	Record[RecordCnt][0] = Temp;
	Record[RecordCnt][1] = Humi;
	Record[RecordCnt][2] = HH;
	Record[RecordCnt][3] = MM;
	Record[RecordCnt][4] = SS;
	
	RecordCnt++;
	if(RecordCnt >= 65)//一共65组数据
		RecordCnt = 0;
	
	LED_MODE ^= (1<<10);//LED3在数据记录中亮灭交替
	GPIOC-> ODR = LED_MODE;
	GPIOD-> ODR |= (1<<2);
	GPIOD-> ODR &=~(1<<2);
}

void UartAction(void)
{
	u8 i;
	u8 UART_Str[50];
	
	if(RxdBuf[0] == 'C')
	{
		//获取最新时间存至Buf中
		HH_Buf = Record[RecordCnt-1][2];//注意是RecordCnt-1
		MM_Buf = Record[RecordCnt-1][3];
		SS_Buf = Record[RecordCnt-1][4];
		
		if(TempThre < 0)
		{		
			//最后加个回车换行会更好
			sprintf((char *)UART_Str, " TempThre:-%d,  HumiThre:%d  Time:%2d:%2d:%2d\r\n", -TempThre_Buf,HumiThre_Buf,HH_Buf,MM_Buf,SS_Buf);
		}
		else
		{
			sprintf((char *)UART_Str, " TempThre:%d,  HumiThre:%d  Time:%2d:%2d:%2d\r\n", TempThre_Buf,HumiThre_Buf,HH_Buf,MM_Buf,SS_Buf);
		}
		USART2_SendString(UART_Str);
	}
	else if(RxdBuf[0] == 'T')
	{
		for(i=0; i<RecordCnt; i++)
		{
			Temp_Buf = Record[i][0];
			Humi_Buf = Record[i][1];
			HH_Buf = Record[i][2];
			MM_Buf = Record[i][3];
			SS_Buf = Record[i][4];
			
			if(Temp_Buf < 0)
			{
				sprintf((char *)UART_Str, " Cnt:%2d Temp:-%.0f,  Humi:%.0f  Time:%2d:%2d:%2d\r\n", i, -Temp_Buf,Humi_Buf,HH_Buf,MM_Buf,SS_Buf);
			}
			else
			{
				sprintf((char *)UART_Str, " Cnt:%2d Temp:%.0f,  Humi:%.0f  Time:%2d:%2d:%2d\r\n", i, Temp_Buf,Humi_Buf,HH_Buf,MM_Buf,SS_Buf);
			}
			USART2_SendString(UART_Str);
		}
	}
	//别忘了这两个
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//接收的数据处理完毕后打开接收中断
	for(i=0; i<20; i++)//清空缓冲区
		RxdBuf[i] = 0;
}

void KeyAction(int code)
{
	if(code == 1)//按下B1，LED1状态改变
	{
		if(flagSet == 0)
		{
			flagSet = 1;
		}
		else if(flagSet == 1)
		{
			//设置完毕退出前，把原值给缓冲区值。
			//往EEPROM里面写的时候用缓冲区，在LCD上实时显示用的是原值
			TempThre_Buf = TempThre;
			HumiThre_Buf = HumiThre;
			SampleTime_Buf = SampleTime;
			TestFreq_Buf = TestFreq;
			//产生对应频率的PWM
			PWM_OC_Init(TestFreq_Buf*1000,50);//注意这里要乘以1000
			
			WriteE2Dta();//退出前把缓冲区的数据写入到E2中
			
			flagSet = 0;//标志位清零
		}
		Beep(100);//伴随滴的一声
	}
	else if(code == 2)//B2
	{
		if(flagSet == 1)//在参数设置界面才实现切换高亮
		{
			SelIndex++;
			SelIndex &= 0x03;//这个算法很不错吧,哈哈
		}
		Beep(100);//伴随滴的一声
	}
	else if(code == 3)//加
	{
		if(flagSet == 1)//在参数设置界面才实现切换高亮
		{
			if(SelIndex == 0)
			{
				TempThre += 1;
				if(TempThre >= 60)
					TempThre = 60;
			}
			else if(SelIndex == 1)
			{
				HumiThre += 5;
				if(HumiThre >= 90)
					HumiThre = 90;
			}
			else if(SelIndex == 2)
			{
				SampleTime += 1;
				if(SampleTime >= 5)
					SampleTime = 5;
			}
			else if(SelIndex == 3)
			{
				TestFreq += 0.5;
				if(TestFreq >= 10)
					TestFreq = 10;
			}
		}
		Beep(100);//伴随滴的一声
	}
	else if(code == 4)//减
	{
		if(flagSet == 1)//在参数设置界面才实现切换高亮
		{
			if(SelIndex == 0)
			{
				TempThre -= 1;
				if(TempThre <= -20)
					TempThre = -20;
			}
			else if(SelIndex == 1)
			{
				HumiThre -= 5;
				if(HumiThre <= 10)
					HumiThre = 10;
			}
			else if(SelIndex == 2)
			{
				SampleTime -= 1;
				if(SampleTime <= 1)
					SampleTime = 1;
			}
			else if(SelIndex == 3)
			{
				TestFreq -= 0.5;
				if(TestFreq <= 1)
					TestFreq = 1;
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

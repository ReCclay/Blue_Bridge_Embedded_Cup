#include "rtc.h"

void NVIC_RTCEnable(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);  //使能RTC中断
}

void RTCInit(u8 HH,u8 MM,u8 SS)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP | RCC_APB1Periph_PWR,ENABLE);
	PWR_BackupAccessCmd(ENABLE);
	BKP_DeInit();
	
	NVIC_RTCEnable();
	
	RCC_LSICmd(ENABLE); //使能LSI时钟
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI); //选择LSI作为时钟源 
	RCC_RTCCLKCmd(ENABLE); //使能RTC时钟
	
	RTC_WaitForSynchro();//APB1时钟与RTC时钟同步
	RTC_WaitForLastTask();//等待操作完成
	
	//以下对RTC寄存器配置
	RTC_SetPrescaler(40000 - 1);  //设置分频
	RTC_WaitForLastTask(); //等待操作完成
	
	RTC_SetCounter(3600 * HH + 60 * MM + SS); //设置初始时间
	RTC_WaitForLastTask();
	
	RTC_ITConfig(RTC_IT_SEC,ENABLE);//秒中断使能
	RTC_WaitForLastTask();
}


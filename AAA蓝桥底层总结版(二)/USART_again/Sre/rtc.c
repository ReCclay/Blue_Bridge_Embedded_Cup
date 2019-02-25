#include "stm32f10x.h"

void RTC_Init(u8 HH, u8 MM, u8 SS)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//RTC中断配置
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);//使能PWR和BKP时钟
	PWR_BackupAccessCmd(ENABLE);//使能BKP
	BKP_DeInit();//BKP复位
	
	RCC_LSICmd(ENABLE);//使能LSI时钟
	while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);//等待时钟使能完毕
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);//选择LSI作为RTC时钟源
	RCC_RTCCLKCmd(ENABLE);//使能RTC时钟
	
	RTC_WaitForSynchro();//等待APB1时钟和RTC时钟同步
	RTC_WaitForLastTask();
	
	RTC_ITConfig(RTC_IT_SEC, ENABLE);//使能秒中断
	RTC_WaitForLastTask();
	
	RTC_SetPrescaler(40000-1);//LSI时钟40K
	RTC_WaitForLastTask();
	
	RTC_SetCounter(HH * 3600 + MM * 60 + SS);//设置RTC时间
	RTC_WaitForLastTask();
	
	
}

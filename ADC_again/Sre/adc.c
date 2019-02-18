#include "stm32f10x.h"

void ADC1_Init(void)
{
	ADC_InitTypeDef ADC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOB, ENABLE);//开启GPIO时钟和ADC1时钟
	RCC_ADCCLKConfig(RCC_PCLK2_Div6); //ADC时钟最大14M，至少进行6分频!!!
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;//选中PB0引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;//模拟输入
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;//ADC独立模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;//单通道
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;//单次转换
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//软件触发
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 1;//通道数目1
	ADC_Init(ADC1, &ADC_InitStructure);
	
	ADC_Cmd(ADC1, ENABLE);//使能ADC
	
	//ADC校准(校准前必须先使能ADC)
	ADC_ResetCalibration(ADC1);//使能复位校准
	while(ADC_GetResetCalibrationStatus(ADC1));//等待复位校准结束
	ADC_StartCalibration(ADC1);//开启AD校准
	while(ADC_GetCalibrationStatus(ADC1));//等待校准结束
}

u16 Get_ADC(void)
{
	u16 tmp;
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_239Cycles5);//转换时间239.5个周期
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);//软件触发
	while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC) == 0);//等待转换完成
	tmp = ADC_GetConversionValue(ADC1);//获取转换结果(读取转换结果会自动清0-EOC标志位)
	ADC_SoftwareStartConvCmd(ADC1, DISABLE);//启动下一次软件转换
	
	return tmp;
}


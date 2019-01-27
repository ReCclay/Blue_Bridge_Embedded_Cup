#include "adc.h"

void ADC1_IOInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//使能PB口时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;//选中PB0引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;//模拟输入
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化IO
}

void ADC_Cali(void)
{
	ADC_ResetCalibration(ADC1);//使能复位校准
	while(ADC_GetResetCalibrationStatus( ADC1));//等待复位校准结束
	ADC_StartCalibration( ADC1);//开启AD校准
	while(ADC_GetCalibrationStatus(ADC1));//等待校准结束
}

void ADC1Init(void)
{
	ADC_InitTypeDef ADC_InitStructure;
	
	ADC1_IOInit();//ADC的GPIO配置
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);//使能ADC时钟,APB2总线
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);//ADC最大时钟不超过14M，这里尽心了对APB2时钟进行6分频=12M
	ADC_DeInit(ADC1);//复位ADC
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;//ADC独立模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;//通道模式选择单通道
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;//转换模式选择单次转换模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//规则组触发转换方式选择转换由软件触发启动
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//数据格式选择右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 1;//通道数目为1
	ADC_Init(ADC1,&ADC_InitStructure);//初始化结构体
	
	ADC_Cmd(ADC1, ENABLE);//使能ADC
	ADC_Cali();//ADC校准
}

u16 Get_ADC(u8 channel)
{
	u16 temp;
	
	ADC_RegularChannelConfig(ADC1, channel, 1, ADC_SampleTime_239Cycles5);//ADC采样时间239.5周期 总转换时间=采样时间+12.5周期
	ADC_SoftwareStartConvCmd( ADC1,ENABLE);//使能ADC1软件触发转换，触发一次ADC转换
	while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC) == 0);//等待ADC转换完毕
	temp = ADC_GetConversionValue( ADC1);//获取转换结果，注意此时已经自动清除EOC位
	ADC_SoftwareStartConvCmd( ADC1,DISABLE);//失能ADC转换
	
	return temp;
}


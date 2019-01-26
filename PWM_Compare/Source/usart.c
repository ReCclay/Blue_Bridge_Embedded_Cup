/*******************************************************************************
* 文件名：usart.c
* 描  述：
* 作  者：CLAY
* 版本号：v1.0.0
* 日  期: 2019年1月25日
* 备  注：简单的串口通信，发送采用主函数查询的方式，接收从采用中断方式
*         接收缓冲区考虑到一般要显示到显示屏的缘故，所以限制了个数20个
*******************************************************************************
*/

#include "usart.h"

u8 RxdBuf[50];//接收缓冲区
u8 RxdOverFlag = 0;//接收完成标志
u8 RxdCnt = 0;//接收变量计数

void USART2_IOInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//PA口时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;//PA2 - TXD
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA3 - RXD
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void NVIC_USART2Enable(void)
{
	NVIC_InitTypeDef NVIC_initstructure;

    NVIC_initstructure.NVIC_IRQChannel = USART2_IRQn;           //选择TIM2中断通道
    NVIC_initstructure.NVIC_IRQChannelCmd = ENABLE;           //使能中断通道
    NVIC_initstructure.NVIC_IRQChannelPreemptionPriority = 0; //设定抢占优先级为0
    NVIC_initstructure.NVIC_IRQChannelSubPriority = 0;        //设定响应优先级为0
    NVIC_Init(&NVIC_initstructure);
}

void USART2Init(u32 baud)
{
	USART_InitTypeDef USART_InitStructure;
	
	USART2_IOInit();
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	USART_InitStructure.USART_BaudRate = baud; //波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; //8位数据位
	USART_InitStructure.USART_StopBits = USART_StopBits_1; //1位停止位
	USART_InitStructure.USART_Parity = USART_Parity_No; //无校验位
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2,&USART_InitStructure); //初始化USART2
	NVIC_USART2Enable();//设置中断优先级
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启串口接收中断
	USART_Cmd(USART2,ENABLE);//使能USART2
}

void USART2_SendByte(u8 *str)
{
	u8 index = 0;
	
	while(str[index] != 0)
	{
		USART_SendData(USART2, str[index]);
		while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == 0);//=1表示发送完成
		index++;
	}
}

void USART2_RecByte(void)
{
	u8 tmp;
	tmp =  USART_ReceiveData(USART2);
	if(tmp != '\n')//结尾标志符号'\n'并且一次发送的数据不能超过20(液晶一行显示20个)
	{
		RxdBuf[RxdCnt++] = tmp;
		if(RxdCnt >= 20)
			RxdCnt = 0;
	}
	else
	{
		RxdCnt = 0;
		RxdOverFlag = 1;
		USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);//关闭接收中断，等待处理完毕再打开
	}
	
}




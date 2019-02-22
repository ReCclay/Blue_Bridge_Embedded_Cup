/**
  ******************************************************************************
  * @file    I2S/SPI_I2S_Switch/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/

#include "stm32f10x.h"
#include "stm32f10x_it.h"
#include "key.h"
#include "beep.h"

extern u32 TimingDelay;

/** @addtogroup STM32F10x_StdPeriph_Examples
  * @{
  */

/** @addtogroup I2S_SPI_I2S_Switch
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles PendSV_Handler exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	TimingDelay--;
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_Switch_IRQHandler(void)
{
}*/

/**
  * @}
  */ 

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

extern u32 time;
extern u8 HH, MM, SS;

void RTC_IRQHandler(void)
{
	if(RTC_GetITStatus(RTC_IT_SEC) == 1)
	{
		RTC_ClearITPendingBit(RTC_IT_SEC);
		time = RTC_GetCounter();
		if(time == 24 * 3600)
		{
			RTC_SetCounter(1);
		}
		HH = time / 3600;
		MM = time % 3600 / 60;
		SS = time % 3600 % 60;
	}
}

extern u8 RxdCnt;
extern u8 RxdOver;
extern u8 RxdBuf[20];
void USART2_IRQHandler(void)
{
	u16 tmp;
	
	if(USART_GetITStatus(USART2, USART_IT_RXNE) == 1)
	{
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);//中断用IT
		tmp = USART_ReceiveData(USART2);
		if(tmp == '\n')
		{
			RxdBuf[RxdCnt-1] = 0;//避免\r显示在LCD发生的乱码
			RxdCnt = 0;
			RxdOver = 1;
			USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);//接收完毕后关闭，防止处理过程发生干扰。
		}
		else
		{
			RxdBuf[RxdCnt++] = tmp;
		}
	}
}

extern u32 CH2_Val;
extern u32 CH2_Duty;

void TIM2_IRQHandler(void)
{
	u16 capture = 0;
	static u8 flag = 0;
	
	if(TIM_GetITStatus(TIM2, TIM_IT_CC2) == 1)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);
		
		capture = TIM_GetCapture2(TIM2);//获取通道2的当前的比较值
		if(flag)
		{
			TIM_SetCompare2(TIM2, capture+CH2_Duty);//设置通道2的值为：当前比较值+高电平
		}
		else
		{
			TIM_SetCompare2(TIM2, capture+(CH2_Val-CH2_Duty));//设置通道2的值为：当前比较值+低电平
		}
		flag ^= 1;//flag在0和1直接变化
	}
}

__IO uint16_t IC3ReadValue1 = 0, IC3ReadValue2 = 0;
__IO uint16_t CaptureNumber = 0;
__IO uint32_t Capture = 0;
__IO uint32_t TIM3Freq = 0;

void TIM3_IRQHandler(void)
{ 
	if(TIM_GetITStatus(TIM3, TIM_IT_CC2) == SET) 
	{
		/* Clear TIM3 Capture compare interrupt pending bit */
		TIM_ClearITPendingBit(TIM3, TIM_IT_CC2);
		if(CaptureNumber == 0)
		{
			/* Get the Input Capture value */
			IC3ReadValue1 = TIM_GetCapture2(TIM3);
			CaptureNumber = 1;
		}
		else if(CaptureNumber == 1)
		{
			/* Get the Input Capture value */
			IC3ReadValue2 = TIM_GetCapture2(TIM3); 

			/* Capture computation */
			if (IC3ReadValue2 > IC3ReadValue1)
			{
				Capture = (IC3ReadValue2 - IC3ReadValue1); 
			}
			else
			{
				Capture = ((0xFFFF - IC3ReadValue1) + IC3ReadValue2); 
			}
			/* Frequency computation */ 
			TIM3Freq = 1000000 / Capture;
			CaptureNumber = 0;
		}
	}
}

extern u8 flagSampleTime;
extern u8 SampleTime;
extern u8 flag200ms;
void TIM4_IRQHandler(void)
{
	static u16 tmr1s = 0;
	static u16 tmr200ms = 0;
	static u16 flag1s = 0;
	
	if(TIM_GetITStatus(TIM4, TIM_IT_Update) == SET) 
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
		KeyScan();
		BeepScan(2);
		
		tmr200ms ++;
		tmr1s++;
		if(tmr200ms >= 100) 
		{
			tmr200ms = 0;
			flag200ms = 1;
		}
		if(tmr1s >= 500)
		{
			tmr1s = 0;
			flag1s++;
			if(flag1s >= SampleTime)
			{
				flag1s = 0;
				flagSampleTime = 1;
			}
		}
	}
}



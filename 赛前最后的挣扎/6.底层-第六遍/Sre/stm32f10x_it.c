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
#include "stm32f10x_it.h"
#include "key.h"
#include "beep.h"
#include "pwm_oc.h"
#include "pwm_ic.h"


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

//void TIM2_IRQHandler(void)
//{
//	u16 capture;
//	static u8 sta = 0;
//	
//	if(TIM_GetITStatus(TIM2, TIM_IT_CC2))
//	{
//		TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);
//		
//		capture = TIM_GetCapture2(TIM2);
//		
//		if(sta)
//		{
//			TIM_SetCompare2(TIM2, capture + TIM2_CH2_Duty);
//		}
//		else
//		{
//			TIM_SetCompare2(TIM2, capture + TIM2_CH2_VAL - TIM2_CH2_Duty);
//		}
//		sta ^= 1;
//	}
//}

void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3, TIM_IT_CC2))
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_CC2);
		
		switch(TIM3_CH2_MODE)
		{
			case 0:
				TIM3_CH2_MODE = 1;
				TIM3_CH2_H = 0;
				TIM3_CH2_HL = 0;
				TIM_SetCounter(TIM3, 0);
				TIM_OC2PolarityConfig(TIM3, TIM_ICPolarity_Falling);
				break;
			case 1:
				TIM3_CH2_MODE = 2;
				TIM3_CH2_H = TIM_GetCapture2(TIM3);
				TIM_OC2PolarityConfig(TIM3, TIM_ICPolarity_Rising);
				break;
			case 2:
				TIM3_CH2_MODE = 3;
				TIM3_CH2_HL = TIM_GetCapture2(TIM3);
				break;
		}
	}
}

extern u8 flag200ms;
void TIM4_IRQHandler(void)
{
	
	static u16 tmr200ms = 0;
	
	if(TIM_GetITStatus(TIM4, TIM_IT_Update))
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
		
		KeyScan();
		BeepScan(2);
		
		
		tmr200ms++;
		if(tmr200ms >= 100)
		{
			tmr200ms = 0;
			flag200ms = 1;
		}
	}
}

extern u8 RxdOver;
extern u8 RxdCnt;
extern u8 RxdBuf[20];
void USART2_IRQHandler(void)
{
	u8 tmp;
	
	if(USART_GetITStatus(USART2, USART_IT_RXNE))
	{
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
		
		tmp = USART_ReceiveData(USART2);
		if(tmp == 'n')
		{
			RxdOver = 1;
			RxdCnt = 0;
			USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
		}
		else
		{
			RxdBuf[RxdCnt++] = tmp;
		}
	}
}

extern u8 RTC_FLAG;
void RTC_IRQHandler(void)
{
	if(RTC_GetITStatus(RTC_IT_SEC))
	{
		RTC_ClearITPendingBit(RTC_IT_SEC);
		
		RTC_FLAG = 1;
	}

}


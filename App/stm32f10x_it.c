/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
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
#include "timer.h"
#include "bsp_rs485.h"
#include "cmd_queue.h"
#include "bsp_tim6.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern uint8_t RxData[10];
extern uint8_t RxLen;
extern uint8_t TxData[10];
extern uint8_t TxLen;

extern uint16_t PhData;
extern uint8_t PhdataArr[10];
extern uint16_t TempData;
extern uint8_t TempdataArr[10];

extern uint32_t screen_time;
extern uint32_t sensor_time;

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
  {
  }
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
  {
  }
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
  {
  }
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
  {
  }
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
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
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
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/


/************************************************
函数名称 ： USART1_IRQHandler
功    能 ： USART1中断
参    数 ： 无
返 回 值 ： 无
作    者 ： strongerHuang
*************************************************/
void USART1_IRQHandler(void)
{
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        uint8_t data = USART_ReceiveData(USART1);
        queue_push(data);
    }
}

/************************************************
函数名称 ： USART3_IRQHandler
功    能 ： USART3中断
参    数 ： 无
返 回 值 ： 无
作    者 ： strongerHuang
*************************************************/
void USART3_IRQHandler(void)
{
	
  if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
  {
    //USART1_SendByte(USART_ReceiveData(USART1));  //发送接收到的字符数据
		RxData[RxLen++] = USART_ReceiveData(USART3);
		
		if(RxLen == 7)
		{
			RxLen = 0;
			
			PhData = RxData[3] * 256 + RxData[4];
			
			if(PhData<100)
			{
				PhdataArr[0] = '0';
				PhdataArr[1] = '.';
				PhdataArr[2] = PhData/10+48;
				PhdataArr[3] = PhData%10+48;
			}
			else if(PhData>=100 && PhData<1000)
			{
				PhdataArr[0] = PhData/100+48;
				PhdataArr[1] = '.';
				PhdataArr[2] = PhData%100/10+48;
				PhdataArr[3] = PhData%100%10+48;
			}
			else
			{
				PhdataArr[0] = PhData/1000+48;
				PhdataArr[1] = PhData%1000/100+48;
				PhdataArr[2] = '.';
				PhdataArr[3] = PhData%1000%100/10+48;
				PhdataArr[4] = PhData%1000%100%10+48;
			}
			
			TempData = RxData[4];
			
			if(TempData<100)
			{
				TempdataArr[0] = TempData/10+48;
				TempdataArr[1] = '.';
				TempdataArr[0] = TempData%10+48;
			}
			else
			{
				TempdataArr[0] = TempData/100+48;
				TempdataArr[1] = TempData%100/10+48;
				TempdataArr[2] = '.';
				TempdataArr[3] = TempData%100%10+48;
			}
		}
  }
	
	if(USART_GetITStatus(USART3, USART_IT_TXE) != RESET)
	{
		if(TxLen>0)
		{
			USART_SendData(USART3, TxData[8-TxLen]);
		}
		else
		{
			while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
			USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
			RS485E_RXEN;
			
		}
		
		if(TxLen == 0)
		{
			TxLen = 9;
		}
		
		TxLen--;
	}
}

/************************************************
函数名称 ： TIM4_IRQHandler
功    能 ： TIM6中断
参    数 ： 无
返 回 值 ： 无
作    者 ： xuyifei
*************************************************/
void  TIM4_IRQHandler (void)
{
	if ( TIM_GetITStatus( TIM4, TIM_IT_Update) != RESET ) 
	{	
		sensor_time++;
		TIM_ClearITPendingBit(TIM4 , TIM_FLAG_Update);  		 
	}		 	
}

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

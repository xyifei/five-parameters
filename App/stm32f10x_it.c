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
#include "hmi_driver.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern uint8_t RxData[32];
extern uint8_t RxLen;
extern uint8_t TxData[10];
extern uint8_t TxLen;

extern uint32_t PhData;
extern uint8_t PhdataArr[10];
extern uint32_t TempData;
extern uint8_t TempdataArr[10];
extern float ConData;
extern uint8_t CondataArr[10];
extern float DoData;
extern uint8_t DodataArr[10];


extern uint32_t screen_time;
extern uint32_t sensor_time;
extern uint32 ph_connect_time;
extern uint32 con_connect_time;
extern uint32 do_connect_time;
extern uint32 tur_connect_time;

long a = 0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*! 
*  \brief  将float类型的书转换成16进制
*  \return 返回一个long 32位的数据
*  \param  HEX 带转换的float类型的值 
*/
long FloatToHex(float HEX)
{
	return *(long *)&HEX;
}

/*! 
*  \brief  将16进制的数转换成float类型的数
*  \return 返回一个float类型数据
*  \param  Byte 待转换的16进制的值 
*/
float HexToDecimal(uint8_t *Byte)
{
	return *((float *)Byte);
}

/*! 
*  \brief  将数字转换成字符存在数组里，以展示在屏幕上
*  \param  Arr  存放的数组
*  \param  data 整型数据 
*/
void NumToArr(uint8_t *Arr, uint32_t data)
{	
	if(data<100)
	{
		Arr[0] = '0';
		Arr[1] = '.';
		Arr[2] = data/10+48;
		Arr[3] = data%10+48;
		Arr[4] = ' ';
		Arr[5] = ' ';
	}
	else if(data>=100 && data<1000)
	{
		Arr[0] = data/100+48;
		Arr[1] = '.';
		Arr[2] = data%100/10+48;
		Arr[3] = data%100%10+48;
		Arr[4] = ' ';
		Arr[5] = ' ';
	}
	else if(data>=1000 && data<10000)
	{
		Arr[0] = data/1000+48;
		Arr[1] = data%1000/100+48;
		Arr[2] = '.';
		Arr[3] = data%1000%100/10+48;
		Arr[4] = data%1000%100%10+48;
		Arr[5] = ' ';
	}
	else if(data>=10000 && data<100000)
	{
		Arr[0] = data/10000+48;
		Arr[1] = data%10000/1000+48;
		Arr[2] = data%1000/100+48;
		Arr[3] = '.';
		Arr[4] = data%100/10+48;
		Arr[5] = data%100%10+48;
	}
}

/*! 
*  \brief    处理PH传感器的数据
*/
void PhSensor(void)
{
	PhData = RxData[3] * 256 + RxData[4];
	TempData = RxData[20]*10;
	
	NumToArr(PhdataArr, PhData);
	NumToArr(TempdataArr, TempData);
}

/*! 
*  \brief    处理电导率传感器的数据
*/
void ConductivitySensor(void)
{
	uint8_t buf[4];
	
	buf[0] = RxData[4];
	buf[1] = RxData[3];
	buf[2] = RxData[6];
	buf[3] = RxData[5];
	ConData = HexToDecimal(buf);
	
	NumToArr(CondataArr, ConData*100);
}

/*! 
*  \brief    处理溶解氧传感器的数据
*/
void DoSensor(void)
{
	uint8_t buf[4];
	buf[0] = RxData[4];
	buf[1] = RxData[3];
	buf[2] = RxData[6];
	buf[3] = RxData[5];
	DoData = HexToDecimal(buf);
	
	NumToArr(DodataArr, DoData*100);
}

/*! 
*  \brief    处理浊度传感器的数据
*/
void TurbiditySensor(void)
{
	a = FloatToHex(2.4);
}

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
	static uint8 i = 0;
	static uint8_t RecFlag = 0;
	
  if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
  {
    //USART1_SendByte(USART_ReceiveData(USART1));  //发送接收到的字符数据
		RxData[RxLen] = USART_ReceiveData(USART3);
		
		if(RxData[0]==0x01 || RxData[0]==0x02 || RxData[0]==0x03 || RxData[0]==0x0A)
		{
			RecFlag = 1;
		}
		
		if(RecFlag == 1)
		{
			RxLen++;
		}
		
		if(RxLen == 25)
		{
			RxLen = 0;
			RecFlag = 0;
			
			switch(RxData[0])
			{
				case 0x01:
					ph_connect_time = 0;
					PhSensor();
					break;
				case 0x02:
					con_connect_time = 0;
					ConductivitySensor();
					break;
				case 0x03:
					do_connect_time = 0;
					DoSensor();
					break;
				case 0x0A:
					tur_connect_time = 0;
					TurbiditySensor();
					break;
			}				
		}
  }
	
	if(USART_GetITStatus(USART3, USART_IT_TXE) != RESET)
	{
		if(i<TxLen)
		{
			USART_SendData(USART3, TxData[i]);
			i++;
		}
		else
		{
			while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
			USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
			RS485E_RXEN;	
			i=0;
		}
		
//		if(TxLen>0)
//		{
//			USART_SendData(USART3, TxData[8-TxLen]);
//		}
//		else
//		{
//			while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
//			USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
//			RS485E_RXEN;	
//		}
//		
//		if(TxLen == 0)
//		{
//			TxLen = 9;
//		}
//		
//		TxLen--;
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
		screen_time++;
		ph_connect_time++;
		con_connect_time++;
		do_connect_time++;
		tur_connect_time++;
		TIM_ClearITPendingBit(TIM4 , TIM_FLAG_Update);  		 
	}		 	
}

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

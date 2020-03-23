/**
  ********************************  STM32F10x  *********************************
  * @文件名     ： main.c
  * @作者       ： strongerHuang
  * @库版本     ： V3.5.0
  * @文件版本   ： V1.0.0
  * @日期       ： 2016年10月18日
  * @摘要       ： 主函数 - Demo软件工程
                   增强版：增加USART串口、TIM定时器
  ******************************************************************************/
/*----------------------------------------------------------------------------
  更新日志:
  2016-10-18 V1.0.0:初始版本
  ----------------------------------------------------------------------------*/
/* 包含的头文件 --------------------------------------------------------------*/
#include "timer.h"
#include "usart.h"
#include "bsp_rs485.h"
#include "bsp_key.h"
#include "hmi_driver.h"
#include "cmd_queue.h"
#include "cmd_process.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t RxData[7];
uint8_t RxLen = 0;         
uint8_t TxData[8];
uint8_t TxLen = 8;   
uint8_t KeyFlag = 0;

uint16_t PhData = 0;
uint8_t PhdataArr[10];
uint16_t TempData = 0;
uint8_t TempdataArr[10];

uint8  cmd_buffer[CMD_MAX_SIZE];  

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/************************************************
函数名称 ： main
功    能 ： 主函数入口
参    数 ： 无
返 回 值 ： int
作    者 ： strongerHuang
*************************************************/
int main(void)
{
	//配置定时器
	TIMER_Initializes();
	//配置485通信（STM32-传感器）
	rs485_Init();
	//配置串口（STM32-屏幕）
	USART_Initializes();
	//按键初始化
	GPIO_Key_Configuration();
	
	//清空串口接收缓冲区                                                          
  queue_reset();
	
	//延时等待串口屏初始化完毕,必须等待300ms                                      
  TIMDelay_Nms(300);
	
  while(1)
  { 
		RS485E_TXEN;
		TxData[0] = 0x01;
		TxData[1] = 0x04;
		TxData[2] = 0x00;
		TxData[3] = 0x00;
		TxData[4] = 0x00;
		TxData[5] = 0x01;
		TxData[6] = 0x31;
		TxData[7] = 0xCA;
		USART_ITConfig(USART3, USART_IT_TXE, ENABLE); 
		TIMDelay_Nms(500);
		SetTextValue(0,7, PhdataArr);
		
		TIMDelay_Nms(500);

		RS485E_TXEN;
		TxData[0] = 0x01;
		TxData[1] = 0x04;
		TxData[2] = 0x00;
		TxData[3] = 0x08;
		TxData[4] = 0x00;
		TxData[5] = 0x01;
		TxData[6] = 0xB0;
		TxData[7] = 0x08;
		USART_ITConfig(USART3, USART_IT_TXE, ENABLE); 
		TIMDelay_Nms(500);
		SetTextValue(0,12, TempdataArr);

//		if(KeyFlag == 0)
//		{
//			Key_Scan();
//		}
//		
//		
//		if(KeyFlag)
//		{
//			KeyFlag = 0;
//			
//			RS485E_TXEN;
//			
//			TxData[0] = 0x01;
//			TxData[1] = 0x04;
//			TxData[2] = 0x00;
//			TxData[3] = 0x00;
//			TxData[4] = 0x00;
//			TxData[5] = 0x01;
//			TxData[6] = 0x31;
//			TxData[7] = 0xCA;
//			
//			USART_ITConfig(USART3, USART_IT_TXE, ENABLE); 
//			
//			SetTextValue(0,7, PhdataArr);
//		}
	}

}


/**** Copyright (C)2016 strongerHuang. All Rights Reserved **** END OF FILE ****/

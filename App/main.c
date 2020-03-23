/**
  ********************************  STM32F10x  *********************************
  * @�ļ���     �� main.c
  * @����       �� strongerHuang
  * @��汾     �� V3.5.0
  * @�ļ��汾   �� V1.0.0
  * @����       �� 2016��10��18��
  * @ժҪ       �� ������ - Demo�������
                   ��ǿ�棺����USART���ڡ�TIM��ʱ��
  ******************************************************************************/
/*----------------------------------------------------------------------------
  ������־:
  2016-10-18 V1.0.0:��ʼ�汾
  ----------------------------------------------------------------------------*/
/* ������ͷ�ļ� --------------------------------------------------------------*/
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
�������� �� main
��    �� �� ���������
��    �� �� ��
�� �� ֵ �� int
��    �� �� strongerHuang
*************************************************/
int main(void)
{
	//���ö�ʱ��
	TIMER_Initializes();
	//����485ͨ�ţ�STM32-��������
	rs485_Init();
	//���ô��ڣ�STM32-��Ļ��
	USART_Initializes();
	//������ʼ��
	GPIO_Key_Configuration();
	
	//��մ��ڽ��ջ�����                                                          
  queue_reset();
	
	//��ʱ�ȴ���������ʼ�����,����ȴ�300ms                                      
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

#include "bsp_rs485.h"
#include "timer.h"

static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void rs485_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	RCC_APB2PeriphClockCmd(RS485COM_PORTRCC | RS485E_PORTRCC , ENABLE); 
	RCC_APB1PeriphClockCmd(RS485COM_RCC, ENABLE);
	
	/*  USART1_Tx */
	GPIO_InitStructure.GPIO_Pin = RS485COMTX_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_AF_PP;		//推挽输出-TX
	GPIO_Init(RS485COM_PORT, &GPIO_InitStructure);

	/*  USART1_Rx  */
	GPIO_InitStructure.GPIO_Pin = RS485COMRX_PIN;
	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_IPU;      //浮空输入-RX
	GPIO_Init(RS485COM_PORT, &GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	
	USART_InitStructure.USART_Parity =USART_Parity_No;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_ITConfig(RS485COM_NAME, USART_IT_PE, DISABLE);//关闭奇偶校验错中断
	
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(RS485COM_NAME, &USART_InitStructure);
  
	// 串口中断优先级配置
	NVIC_Configuration();
	
	// 使能串口接收中断
	USART_ITConfig(RS485COM_NAME, USART_IT_RXNE, ENABLE);	
	
	// ENABLE the USARTx 
	USART_Cmd(RS485COM_NAME, ENABLE);     
	
	//配置RS485芯片发送和接收模式的控制引脚
	GPIO_InitStructure.GPIO_Pin = RS485E_PIN; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(RS485E_PORT, &GPIO_InitStructure); 
	
	RS485E_RXEN;
}

void UartASendStr (u8 *pucStr, u8 ulNum) 
{ 
    u8 i; 
    for(i = 0;i<ulNum;i++) 
    { 
			while ((USART3->SR & USART_FLAG_TC) == (uint16_t)RESET);
			USART_SendData(USART3,*pucStr++); 
    }  
} 

//void RS485_Send(u8 *pucStr, u8 ulNum) 
//{
//	RS485E_TXEN;
//	Delay_us(10);
//  UartASendStr(pucStr,ulNum);
//  while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
//	Delay_us(10);
//  RS485E_RXEN; 
//}


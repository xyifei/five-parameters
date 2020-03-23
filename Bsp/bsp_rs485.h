#ifndef __RS485_H
#define __RS485_H


#include "stm32f10x.h"

#define RS485E_PORT                         GPIOB  			                            //配置485发送接收引脚 
#define RS485E_PIN                          GPIO_Pin_12
#define RS485E_PORTRCC                      RCC_APB2Periph_GPIOB
#define RS485E_TXEN                         GPIO_SetBits(RS485E_PORT,RS485E_PIN)	  //高电平为发送模式
#define RS485E_RXEN													GPIO_ResetBits(RS485E_PORT,RS485E_PIN)

#define RS485COM_NAME                       USART3 	                                //改串口配置  引脚 时钟        
#define RS485COM_PORT                       GPIOB  
#define RS485COMTX_PIN                      GPIO_Pin_10
#define RS485COMRX_PIN                      GPIO_Pin_11
#define RS485COM_PORTRCC                    RCC_APB2Periph_GPIOB
#define RS485COM_RCC                        RCC_APB1Periph_USART3
#define RS485COM_IRQHandler                 USART3_IRQHandler 

void rs485_Init(void);
void UartASendStr (u8 *pucStr, u8 ulNum);
void RS485_Send(u8 *pucStr, u8 ulNum);

#endif

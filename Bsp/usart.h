/**
  ********************************  STM32F10x  *********************************
  * @文件名     ： usart.h
  * @作者       ： strongerHuang
  * @库版本     ： V3.5.0
  * @文件版本   ： V1.0.0
  * @日期       ： 2016年10月18日
  * @摘要       ： USART头文件
  ******************************************************************************/

/* 定义防止递归包含 ----------------------------------------------------------*/
#ifndef _USART_H
#define _USART_H

/* 包含的头文件 --------------------------------------------------------------*/
#include "stm32f10x.h"
#include <stdio.h>

#define uchar    unsigned char
#define uint8    unsigned char
#define uint16   unsigned short int
#define uint32   unsigned long
#define int16    short int
#define int32    long

/* 函数申明 ------------------------------------------------------------------*/
void USART_Initializes(void);
void SendChar(uint8_t t);
//void USART1_SendNByte(uint8_t *pData, uint16_t Length);
//void USART1_Printf(uint8_t *String);


#endif /* _USART_H */

/**** Copyright (C)2016 strongerHuang. All Rights Reserved **** END OF FILE ****/

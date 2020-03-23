/**
  ********************************  STM32F10x  *********************************
  * @�ļ���     �� usart.h
  * @����       �� strongerHuang
  * @��汾     �� V3.5.0
  * @�ļ��汾   �� V1.0.0
  * @����       �� 2016��10��18��
  * @ժҪ       �� USARTͷ�ļ�
  ******************************************************************************/

/* �����ֹ�ݹ���� ----------------------------------------------------------*/
#ifndef _USART_H
#define _USART_H

/* ������ͷ�ļ� --------------------------------------------------------------*/
#include "stm32f10x.h"
#include <stdio.h>

#define uchar    unsigned char
#define uint8    unsigned char
#define uint16   unsigned short int
#define uint32   unsigned long
#define int16    short int
#define int32    long

/* �������� ------------------------------------------------------------------*/
void USART_Initializes(void);
void SendChar(uint8_t t);
//void USART1_SendNByte(uint8_t *pData, uint16_t Length);
//void USART1_Printf(uint8_t *String);


#endif /* _USART_H */

/**** Copyright (C)2016 strongerHuang. All Rights Reserved **** END OF FILE ****/

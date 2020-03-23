/**
  ********************************  STM32F10x  *********************************
  * @文件名     ： timer.h
  * @作者       ： strongerHuang
  * @库版本     ： V3.5.0
  * @文件版本   ： V1.0.0
  * @日期       ： 2016年10月18日
  * @摘要       ： 定时器头文件
  ******************************************************************************/

/* 定义防止递归包含 ----------------------------------------------------------*/
#ifndef _TIMER_H
#define _TIMER_H


/* 包含的头文件 --------------------------------------------------------------*/
#include "stm32f10x.h"


/* 宏定义 --------------------------------------------------------------------*/
#define TIM3_COUNTER_CLOCK        1000000                  //计数时钟(1M次/秒)
                                                           //预分频值
#define TIM3_PRESCALER_VALUE      (SystemCoreClock/TIM3_COUNTER_CLOCK - 1)
#define TIM3_PERIOD_TIMING        (10 - 1)                 //定时周期（相对于计数时钟:1周期 = 1计数时钟）

/* 函数申明 ------------------------------------------------------------------*/
void TIMER_Initializes(void);
void TIMDelay_N10us(uint16_t Times);
void TIMDelay_Nms(uint16_t Times);


#endif /* _TIMER_H */

/**** Copyright (C)2016 strongerHuang. All Rights Reserved **** END OF FILE ****/

/**
  ********************************  STM32F10x  *********************************
  * @�ļ���     �� timer.c
  * @����       �� strongerHuang
  * @��汾     �� V3.5.0
  * @�ļ��汾   �� V1.0.0
  * @����       �� 2016��10��18��
  * @ժҪ       �� ��ʱ��Դ�ļ�
  ******************************************************************************/
/*----------------------------------------------------------------------------
  ������־:
  2016-10-18 V1.0.0:��ʼ�汾
  ----------------------------------------------------------------------------*/
/* ������ͷ�ļ� --------------------------------------------------------------*/
#include "timer.h"


/************************************************
�������� �� TIMER_Initializes
��    �� �� TIMER��ʼ��
��    �� �� ��
�� �� ֵ �� ��
��    �� �� strongerHuang
*************************************************/
void TIMER_Initializes(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

  /* RCCʱ������ */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

  /* TIM3ʱ����Ԫ���� */
  TIM_TimeBaseStructure.TIM_Prescaler = TIM3_PRESCALER_VALUE;        //Ԥ��Ƶֵ
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;        //���ϼ���ģʽ
  TIM_TimeBaseStructure.TIM_Period = TIM3_PERIOD_TIMING;             //��ʱ����
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;            //ʱ�ӷ�Ƶ����
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  /* ʹ��Ԥ��Ƶֵ */
  TIM_ARRPreloadConfig(TIM3, ENABLE);                                //ʹ������ֵ
}

/************************************************
�������� �� TIMDelay_N10us
��    �� �� ��ʱ����ʱN��10us
��    �� �� Times --- Nֵ
�� �� ֵ �� ��
��    �� �� strongerHuang
*************************************************/
void TIMDelay_N10us(uint16_t Times)
{
  TIM_Cmd(TIM3, ENABLE);                                             //������ʱ��
  while(Times--)
  {
    while(RESET == TIM_GetFlagStatus(TIM3, TIM_FLAG_Update));
    TIM_ClearFlag(TIM3, TIM_FLAG_Update);                            //�����־
  }
  TIM_Cmd(TIM3, DISABLE);                                            //�رն�ʱ��
}

/************************************************
�������� �� TIMDelay_Nms
��    �� �� ��ʱ����ʱNms
��    �� �� Times --- Nֵ
�� �� ֵ �� ��
��    �� �� strongerHuang
*************************************************/
void TIMDelay_Nms(uint16_t Times)
{
  while(Times--)
  {
    TIMDelay_N10us(100);
  }
}


/**** Copyright (C)2016 strongerHuang. All Rights Reserved **** END OF FILE ****/

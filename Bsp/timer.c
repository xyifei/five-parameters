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

// �ж����ȼ�����
static void TIM_NVIC_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    // �����ж���Ϊ0
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		
		// �����ж���Դ
    NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;	
		// ���������ȼ�Ϊ 0
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	 
	  // ������ռ���ȼ�Ϊ3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

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
	
	TIM_NVIC_Config();

  /* RCCʱ������ */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  /* TIM3ʱ����Ԫ���� */
  TIM_TimeBaseStructure.TIM_Prescaler = TIM3_Prescaler;        //Ԥ��Ƶֵ
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;        //���ϼ���ģʽ
  TIM_TimeBaseStructure.TIM_Period = TIM3_Period;             //��ʱ����
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;            //ʱ�ӷ�Ƶ����
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
  /* ��ʼ����ʱ�� */
  TIM_ARRPreloadConfig(TIM3, ENABLE);                                //ʹ������ֵ
	
	
	/* RCCʱ������ */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	/* TIM4ʱ����Ԫ���� */
  TIM_TimeBaseStructure.TIM_Prescaler = TIM4_Prescaler;        //Ԥ��Ƶֵ
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;        //���ϼ���ģʽ
  TIM_TimeBaseStructure.TIM_Period = TIM4_Period;             //��ʱ����
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;            //ʱ�ӷ�Ƶ����
  /* ��ʼ����ʱ�� */
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	/* ����������жϱ�־λ */
	TIM_ClearFlag(TIM4, TIM_FLAG_Update);
	/* �����������ж� */
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);
	/* ʹ�ܶ�ʱ�� */
	TIM_Cmd(TIM4, ENABLE);	
}

/************************************************
�������� �� delay_10us
��    �� �� ��ʱ����ʱN��10us
��    �� �� Times --- Nֵ
�� �� ֵ �� ��
��    �� �� strongerHuang
*************************************************/
void delay_10us(uint16_t Times)
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
�������� �� delay_ms
��    �� �� ��ʱ����ʱNms
��    �� �� Times --- Nֵ
�� �� ֵ �� ��
��    �� �� strongerHuang
*************************************************/
void delay_ms(uint16_t Times)
{
  while(Times--)
  {
    delay_10us(100);
  }
}


/**** Copyright (C)2016 strongerHuang. All Rights Reserved **** END OF FILE ****/

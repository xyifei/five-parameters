/**
  ********************************  STM32F10x  *********************************
  * @�ļ���     �� bsp_key.c
  * @����       �� xuyifei
  * @��汾     �� V3.5.0
  * @�ļ��汾   �� V1.0.0
  * @����       �� 2020��3��20��
  * @ժҪ       �� USARTԴ�ļ�
  ******************************************************************************/
/*----------------------------------------------------------------------------
  ������־:
  2020-3-20 V1.0.0:��ʼ�汾
  ----------------------------------------------------------------------------*/
/* ������ͷ�ļ� --------------------------------------------------------------*/
#include "bsp_key.h"
#include "timer.h"

extern uint8_t KeyFlag;


/**
  * @brief  ���ð����õ���I/O��
  * @param  ��
  * @retval ��
  */
void GPIO_Key_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOA,ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	  //��������
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
	GPIO_SetBits(GPIOA, GPIO_Pin_0);
}

/*
 * ��������Key_Scan
 * ����  ������Ƿ��а�������
 * ����  ��GPIOx��x ������ A��B��C��D���� E
 *		     GPIO_Pin������ȡ�Ķ˿�λ 	
 * ���  ��KEY_OFF(û���°���)��KEY_ON�����°�����
 */
void Key_Scan(void)
{			
	if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_6) == 0)  
	{	 
		TIMDelay_Nms(500);
		while(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_6) == 1); 		
		KeyFlag = 1;	 
	}
	else
	{
		KeyFlag = 0;
	}
}


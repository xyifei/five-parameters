/**
  ********************************  STM32F10x  *********************************
  * @文件名     ： bsp_key.c
  * @作者       ： xuyifei
  * @库版本     ： V3.5.0
  * @文件版本   ： V1.0.0
  * @日期       ： 2020年3月20日
  * @摘要       ： USART源文件
  ******************************************************************************/
/*----------------------------------------------------------------------------
  更新日志:
  2020-3-20 V1.0.0:初始版本
  ----------------------------------------------------------------------------*/
/* 包含的头文件 --------------------------------------------------------------*/
#include "bsp_key.h"
#include "timer.h"

extern uint8_t KeyFlag;


/**
  * @brief  配置按键用到的I/O口
  * @param  无
  * @retval 无
  */
void GPIO_Key_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOA,ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	  //上拉输入
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
	GPIO_SetBits(GPIOA, GPIO_Pin_0);
}

/*
 * 函数名：Key_Scan
 * 描述  ：检测是否有按键按下
 * 输入  ：GPIOx：x 可以是 A，B，C，D或者 E
 *		     GPIO_Pin：待读取的端口位 	
 * 输出  ：KEY_OFF(没按下按键)、KEY_ON（按下按键）
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


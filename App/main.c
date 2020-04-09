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
#include "stm32f10x_it.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/*                            STM32-����������                                */
uint8_t RxData[32];																																
uint8_t RxLen = 0;         
uint8_t TxData[13];
uint8_t TxLen = 8;   
uint8_t KeyFlag = 0;


/*                              ���������ֵ                                   */
uint32_t PhData = 0;                                                                //PHֵ
uint8_t PhdataArr[10];																															//���PHֵ������
uint32_t TempData = 0;																															//�¶�ֵ
uint8_t TempdataArr[10];																														//����¶�ֵ������
float ConData = 0;																																	//�絼��ֵ
uint8_t CondataArr[10];																															//��ŵ絼��ֵ������
float DoData = 0;
uint8_t DodataArr[10];
 
 
/*                              ��Ļ��ص�����                                  */                                                              
volatile uint32 screen_time = 0;                                                     //����Ļ�ϴ����ݵ�ʱ��
volatile uint32 sensor_time = 0;                                                     //��ȡ��������ʱ��
volatile uint32 ph_connect_time = 5000;                                                 //���PH�������Ƿ���������
volatile uint32 con_connect_time = 5000;                                                //���絼�ʴ������Ƿ���������
volatile uint32 do_connect_time = 5000;                                                 //����ܽ����������Ƿ���������
volatile uint32 tur_connect_time = 5000;                                                //����Ƕȴ������Ƿ���������

uint8  cmd_buffer[CMD_MAX_SIZE];                                                     //ָ���
static uint16 current_screen_id = 0;                                                 //��ǰ����ID
static float PhCal = 0;                                                              //PHУ׼                                                                
//static float ConCal = 0;                                                             //�絼��У׼Һ��ֵ
//static float TurCal = 0;                                                              //PHУ׼Һ��ֵ
long b;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void GetSensorData(void);
float ArrToFloat(uint8 *str);

/*! 
*  \brief  ���������
*/
int main(void)
{
	qsize  size = 0; 
	
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
  delay_ms(300);
	
  while(1)
  { 
		size = queue_find_cmd(cmd_buffer,CMD_MAX_SIZE);

		if(size>0&&cmd_buffer[1]!=0x07)                                              //���յ�ָ�� �����ж��Ƿ�Ϊ������ʾ
		{                                                                           
				ProcessMessage((PCTRL_MSG)cmd_buffer, size);                             //ָ���  
		}                                                                           
		else if(size>0&&cmd_buffer[1]==0x07)                                         //���Ϊָ��0x07��������STM32  
		{                                                                           
				__disable_fault_irq();                                                   
				NVIC_SystemReset();                                                                                                                                          
		}                             
		
		GetSensorData();
		
		if(screen_time >= 500)
		{
			screen_time = 0;
			UpdateUI();
		}
	}

}

void ProcessMessage( PCTRL_MSG msg, uint16 size )
{
    uint8 cmd_type = msg->cmd_type;                                                  //ָ������
    uint8 ctrl_msg = msg->ctrl_msg;                                                  //��Ϣ������
    uint8 control_type = msg->control_type;                                          //�ؼ�����
    uint16 screen_id = PTR2U16(&msg->screen_id);                                     //����ID
    uint16 control_id = PTR2U16(&msg->control_id);                                   //�ؼ�ID
    uint32 value = PTR2U32(msg->param);                                              //��ֵ


    switch(cmd_type)
    {  
    case NOTIFY_TOUCH_PRESS:                                                        //����������
    case NOTIFY_TOUCH_RELEASE:                                                      //�������ɿ�
        NotifyTouchXY(cmd_buffer[1],PTR2U16(cmd_buffer+2),PTR2U16(cmd_buffer+4)); 
        break;                                                                    
    case NOTIFY_WRITE_FLASH_OK:                                                     //дFLASH�ɹ�
        NotifyWriteFlash(1);                                                      
        break;                                                                    
    case NOTIFY_WRITE_FLASH_FAILD:                                                  //дFLASHʧ��
        NotifyWriteFlash(0);                                                      
        break;                                                                    
    case NOTIFY_READ_FLASH_OK:                                                      //��ȡFLASH�ɹ�
        NotifyReadFlash(1,cmd_buffer+2,size-6);                                     //ȥ��֡ͷ֡β
        break;                                                                    
    case NOTIFY_READ_FLASH_FAILD:                                                   //��ȡFLASHʧ��
        NotifyReadFlash(0,0,0);                                                   
        break;                                                                    
    case NOTIFY_READ_RTC:                                                           //��ȡRTCʱ��
        NotifyReadRTC(cmd_buffer[2],cmd_buffer[3],cmd_buffer[4],cmd_buffer[5],cmd_buffer[6],cmd_buffer[7],cmd_buffer[8]);
        break;
    case NOTIFY_CONTROL:
        {
            if(ctrl_msg==MSG_GET_CURRENT_SCREEN)                                    //����ID�仯֪ͨ
            {
                NotifyScreen(screen_id);                                            //�����л������ĺ���
            }
            else
            {
                switch(control_type)
                {
                case kCtrlButton:                                                   //��ť�ؼ�
                    NotifyButton(screen_id,control_id,msg->param[1]);                  
                    break;                                                             
                case kCtrlText:                                                     //�ı��ؼ�
                    NotifyText(screen_id,control_id,msg->param);                       
                    break;                                                             
                case kCtrlProgress:                                                 //�������ؼ�
                    NotifyProgress(screen_id,control_id,value);                        
                    break;                                                             
                case kCtrlSlider:                                                   //�������ؼ�
                    NotifySlider(screen_id,control_id,value);                          
                    break;                                                             
                case kCtrlMeter:                                                    //�Ǳ�ؼ�
                    NotifyMeter(screen_id,control_id,value);                           
                    break;                                                             
                case kCtrlMenu:                                                     //�˵��ؼ�
                    NotifyMenu(screen_id,control_id,msg->param[0],msg->param[1]);      
                    break;                                                              
                case kCtrlSelector:                                                 //ѡ��ؼ�
                    NotifySelector(screen_id,control_id,msg->param[0]);                
                    break;                                                              
                case kCtrlRTC:                                                      //����ʱ�ؼ�
                    NotifyTimer(screen_id,control_id);
                    break;
                default:
                    break;
                }
            } 
            break;  
        } 
    case NOTIFY_HandShake:                                                          //����֪ͨ                                                     
        NOTIFYHandShake();
        break;
    default:
        break;
    }
}

/*! 
*  \brief  ����֪ͨ
*/
void NOTIFYHandShake()
{
   SetButtonValue(3,2,1);
}

/*! 
*  \brief  �����л�֪ͨ
*  \details  ��ǰ����ı�ʱ(�����GetScreen)��ִ�д˺���
*  \param screen_id ��ǰ����ID
*/
void NotifyScreen(uint16 screen_id)
{
    //TODO: ����û�����
    current_screen_id = screen_id;                                                   //�ڹ��������п��������л�֪ͨ����¼��ǰ����ID

}

/*! 
*  \brief  ���������¼���Ӧ
*  \param press 1���´�������3�ɿ�������
*  \param x x����
*  \param y y����
*/
void NotifyTouchXY(uint8 press,uint16 x,uint16 y)
{ 
    //TODO: ����û�����
}


/*! 
*  \brief  ��������
*/ 
void UpdateUI()
{
    //�ı����ú���ʾ  ��ʱ500msˢ��һ��
    if(current_screen_id==0)                                              
    {
			//ÿ500ms���ɼ����Ĵ�����������ʾ����Ļ��
			BatchBegin(0);
			BatchSetText(7, PhdataArr);                                               //����PH�ı�ֵ
			BatchSetText(12, CondataArr);                                             //���õ絼�ʱ�ֵ
			BatchSetText(13, DodataArr);                                              //�����ܽ����ı�ֵ
			BatchSetText(15, TempdataArr);                                            //�����¶ȱ�ֵ	
			
			if(ph_connect_time < 5000)
			{
				BatchSetText(22,"��������");
			}
			else
			{
				BatchSetText(22,"δ����");
			}
			
			if(con_connect_time < 5000)
			{
				BatchSetText(25,"��������");
			}
			else
			{
				BatchSetText(25,"δ����");
			}
			
			if(do_connect_time < 5000)
			{
				BatchSetText(27,"��������");
			}
			else
			{
				BatchSetText(27,"δ����");
			}
			
			if(tur_connect_time < 5000)
			{
				BatchSetText(29,"��������");
			}
			else
			{
				BatchSetText(29,"δ����");
			}

			BatchEnd();
    }                                                                            
                                                                                                                                                        
}

/*! 
*  \brief  ��ť�ؼ�֪ͨ
*  \details  ����ť״̬�ı�(�����GetControlValue)ʱ��ִ�д˺���
*  \param screen_id ����ID
*  \param control_id �ؼ�ID
*  \param state ��ť״̬��0����1����
*/
void NotifyButton(uint16 screen_id, uint16 control_id, uint8  state)
{ 
	//����У׼PH
	if(screen_id == 3)
	{
		if(control_id==1 && state==1)                                                       //����У׼ģʽ                                                       
		{
			TxLen = 8;
			RS485E_TXEN;
			TxData[0] = 0x01;
			TxData[1] = 0x06;
			TxData[2] = 0x00;
			TxData[3] = 0x40;
			TxData[4] = 0x00;
			TxData[5] = 0x60;
			TxData[6] = 0x88;
			TxData[7] = 0x36;
			USART_ITConfig(USART3, USART_IT_TXE, ENABLE); 
		}
		
		if(control_id==10 && state==1)                                                      //У׼���Ա�׼Һ
		{
			TxLen = 8;
			RS485E_TXEN;
			TxData[0] = 0x01;
			TxData[1] = 0x06;
			TxData[2] = 0x00;
			TxData[3] = 0x41;
			TxData[4] = 0x00;
			TxData[5] = 0x02;
			TxData[6] = 0x58;
			TxData[7] = 0x1F;
			USART_ITConfig(USART3, USART_IT_TXE, ENABLE);                           
		}
		
		if(control_id==9 && state==1)                                                       //У׼���Ա�׼Һ
		{
			TxLen = 8;
			RS485E_TXEN;
			TxData[0] = 0x01;
			TxData[1] = 0x06;
			TxData[2] = 0x00;
			TxData[3] = 0x41;
			TxData[4] = 0x00;
			TxData[5] = 0x04;
			TxData[6] = 0xD8;
			TxData[7] = 0x1D;
			USART_ITConfig(USART3, USART_IT_TXE, ENABLE); 
		}
		
		if(control_id==11 && state==1)                                                       //У׼���Ա�׼Һ
		{
			TxLen = 8;
			RS485E_TXEN;
			TxData[0] = 0x01;
			TxData[1] = 0x06;
			TxData[2] = 0x00;
			TxData[3] = 0x41;
			TxData[4] = 0x00;
			TxData[5] = 0x08;
			TxData[6] = 0xD8;
			TxData[7] = 0x18;
			USART_ITConfig(USART3, USART_IT_TXE, ENABLE); 
		}		
	}
	
	//У׼�絼��
	if(screen_id == 8)
	{
		if(control_id==10 && state==1)                                                        //����У׼ģʽ
		{
			TxLen = 8;
			RS485E_TXEN;
			TxData[0] = 0x01;
			TxData[1] = 0x06;
			TxData[2] = 0x00;
			TxData[3] = 0x40;
			TxData[4] = 0x00;
			TxData[5] = 0x60;
			TxData[6] = 0x88;
			TxData[7] = 0x05;
			USART_ITConfig(USART3, USART_IT_TXE, ENABLE); 
		}	
	}
	
	if(screen_id == 12)
	{
		if(control_id==9 && state==1)
		{
			TxLen = 13;
			RS485E_TXEN;
			TxData[0] = 0x03;
			TxData[1] = 0x10;
			TxData[2] = 0x00;
			TxData[3] = 0x06;
			TxData[4] = 0x00;
			TxData[5] = 0x02;
			TxData[6] = 0x04;
			TxData[7] = FloatToHex(1.027853)>>8 & 0xFF;
			TxData[8] = FloatToHex(1.027853) & 0xFF;
			TxData[9] = FloatToHex(1.027853)>>24 & 0xFF;
			TxData[10] = FloatToHex(1.027853)>>16 & 0xFF;
			TxData[11] = usMBCRC16(TxData,11) & 0xFF;
			TxData[12] = usMBCRC16(TxData,11)>>8 & 0xFF;		
			USART_ITConfig(USART3, USART_IT_TXE, ENABLE); 
		}
	}
		
}

/*! 
*  \brief  �ı��ؼ�֪ͨ
*  \details  ���ı�ͨ�����̸���(�����GetControlValue)ʱ��ִ�д˺���
*  \details  �ı��ؼ����������ַ�����ʽ�·���MCU������ı��ؼ������Ǹ���ֵ��
*  \details  ����Ҫ�ڴ˺����н��·��ַ�������ת�ظ���ֵ��
*  \param screen_id ����ID
*  \param control_id �ؼ�ID
*  \param str �ı��ؼ�����
*/
void NotifyText(uint16 screen_id, uint16 control_id, uint8 *str)
{
	if(screen_id==3)                                                                 
	{                                                                            
		if(control_id == 8)                                                            
		{                                                                                                
			PhCal = ArrToFloat(str);
			PhCal = PhCal/ (((float)PhData)/100); 
			
//			RS485E_TXEN;
//			TxData[0] = 0x01;
//			TxData[1] = 0x10;
//			TxData[2] = 0x00;
//			TxData[3] = 0x04;
//			TxData[4] = 0x00;
//			TxData[5] = 0x02;
//			TxData[6] = 0x04;
//			TxData[7] = FloatToHex(PhCal)>>8 & 0xFF;
//			TxData[8] = FloatToHex(PhCal) & 0xFF;
//			TxData[9] = FloatToHex(PhCal)>>24 & 0xFF;
//			TxData[10] = FloatToHex(PhCal)>>16 & 0xFF;
//			TxData[11] = usMBCRC16(TxData,11) & 0xFF;
//			TxData[12] = usMBCRC16(TxData,11)>>8 & 0xFF;		
//			USART_ITConfig(USART3, USART_IT_TXE, ENABLE); 
		}                                                                         
	}     

	if(screen_id == 8)
	{
		if(control_id == 8)
		{
			
		}
	}
}                                                                                

/*!                                                                              
*  \brief  �������ؼ�֪ͨ                                                       
*  \details  ����GetControlValueʱ��ִ�д˺���                                  
*  \param screen_id ����ID                                                      
*  \param control_id �ؼ�ID                                                     
*  \param value ֵ                                                              
*/                                                                              
void NotifyProgress(uint16 screen_id, uint16 control_id, uint32 value)           
{  

}                                                                                

/*!                                                                              
*  \brief  �������ؼ�֪ͨ                                                       
*  \details  ���������ı�(�����GetControlValue)ʱ��ִ�д˺���                  
*  \param screen_id ����ID                                                      
*  \param control_id �ؼ�ID                                                     
*  \param value ֵ                                                              
*/                                                                              
void NotifySlider(uint16 screen_id, uint16 control_id, uint32 value)             
{                                                             

}

/*! 
*  \brief  �Ǳ�ؼ�֪ͨ
*  \details  ����GetControlValueʱ��ִ�д˺���
*  \param screen_id ����ID
*  \param control_id �ؼ�ID
*  \param value ֵ
*/
void NotifyMeter(uint16 screen_id, uint16 control_id, uint32 value)
{
    //TODO: ����û�����
}

/*! 
*  \brief  �˵��ؼ�֪ͨ
*  \details  ���˵���»��ɿ�ʱ��ִ�д˺���
*  \param screen_id ����ID
*  \param control_id �ؼ�ID
*  \param item �˵�������
*  \param state ��ť״̬��0�ɿ���1����
*/
void NotifyMenu(uint16 screen_id, uint16 control_id, uint8 item, uint8 state)
{
    //TODO: ����û�����
}

/*! 
*  \brief  ѡ��ؼ�֪ͨ
*  \details  ��ѡ��ؼ��仯ʱ��ִ�д˺���
*  \param screen_id ����ID
*  \param control_id �ؼ�ID
*  \param item ��ǰѡ��
*/
void NotifySelector(uint16 screen_id, uint16 control_id, uint8  item)
{

}

/*! 
*  \brief  ��ʱ����ʱ֪ͨ����
*  \param screen_id ����ID
*  \param control_id �ؼ�ID
*/
void NotifyTimer(uint16 screen_id, uint16 control_id)
{

}

/*! 
*  \brief  ��ȡ�û�FLASH״̬����
*  \param status 0ʧ�ܣ�1�ɹ�
*  \param _data ��������
*  \param length ���ݳ���
*/
void NotifyReadFlash(uint8 status,uint8 *_data,uint16 length)
{
    //TODO: ����û�����
}

/*! 
*  \brief  д�û�FLASH״̬����
*  \param status 0ʧ�ܣ�1�ɹ�
*/
void NotifyWriteFlash(uint8 status)
{
    //TODO: ����û�����
}

/*! 
*  \brief  ��ȡRTCʱ�䣬ע�ⷵ�ص���BCD��
*  \param year �꣨BCD��
*  \param month �£�BCD��
*  \param week ���ڣ�BCD��
*  \param day �գ�BCD��
*  \param hour ʱ��BCD��
*  \param minute �֣�BCD��
*  \param second �루BCD��
*/
void NotifyReadRTC(uint8 year,uint8 month,uint8 week,uint8 day,uint8 hour,uint8 minute,uint8 second)
{

}

/*! 
*  \brief  ����Ļ���������ı��ؼ�����ת��Ϊfloat
*/
float ArrToFloat(uint8 *str)
{
	uint8 i = 0;
	uint8 Len=0; 
	uint8 Point=0;
	float IntValue = 0;
	float DecValue = 0;
	float value = 0;
	
	while(*(str+i) != NULL)
	{
		i++;
	}
	Len = i;
	
	for(i=0;i<Len;i++)
	{
		if(*(str+i) == '.')
		{
			Point = i;
		}
		
		if(i==Len-1 && Point==0)
		{
			Point = i+1;
		}
	}
	
	for(i=0;i<Point;i++)
	{
		IntValue = IntValue*10 + (*(str+i)-48);
	}
	
	for(i=Len-1;i>Point;i--)
	{
		DecValue = DecValue*0.1 + (*(str+i)-48)*0.1;
	}
	value = IntValue + DecValue;
	
	return value;
}

/*! 
*  \brief  �򴫸������Ͷ�ȡָ��
*/
void GetSensorData(void)
{
	if(sensor_time == 250)
	{
		TxLen = 8;
		RS485E_TXEN;
		TxData[0] = 0x01;
		TxData[1] = 0x04;
		TxData[2] = 0x00;
		TxData[3] = 0x00;
		TxData[4] = 0x00;
		TxData[5] = 0x0A;
		TxData[6] = 0x70;
		TxData[7] = 0x0D;
		USART_ITConfig(USART3, USART_IT_TXE, ENABLE); 
	}
	else if(sensor_time == 500)
	{
		TxLen = 8;
		RS485E_TXEN;
		TxData[0] = 0x02;
		TxData[1] = 0x03;
		TxData[2] = 0x00;
		TxData[3] = 0x00;
		TxData[4] = 0x00;
		TxData[5] = 0x0A;
		TxData[6] = 0xC5;
		TxData[7] = 0xFE;
		USART_ITConfig(USART3, USART_IT_TXE, ENABLE); 
	}
	else if(sensor_time == 750)
	{
		TxLen = 8;
		RS485E_TXEN;
		TxData[0] = 0x03;
		TxData[1] = 0x03;
		TxData[2] = 0x00;
		TxData[3] = 0x00;
		TxData[4] = 0x00;
		TxData[5] = 0x0A;
		TxData[6] = 0xC4;
		TxData[7] = 0x2F;
		USART_ITConfig(USART3, USART_IT_TXE, ENABLE); 
	}
	else if(sensor_time >= 1000)
	{
		sensor_time = 0;
//		RS485E_TXEN;
//		TxData[0] = 0x0A;
//		TxData[1] = 0x03;
//		TxData[2] = 0x00;
//		TxData[3] = 0x00;
//		TxData[4] = 0x00;
//		TxData[5] = 0x0A;
//		TxData[6] = 0xC4;
//		TxData[7] = 0xB6;
//		USART_ITConfig(USART3, USART_IT_TXE, ENABLE); 
		
	}
		
}

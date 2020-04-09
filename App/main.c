/**
  ********************************  STM32F10x  *********************************
  * @文件名     ： main.c
  * @作者       ： strongerHuang
  * @库版本     ： V3.5.0
  * @文件版本   ： V1.0.0
  * @日期       ： 2016年10月18日
  * @摘要       ： 主函数 - Demo软件工程
                   增强版：增加USART串口、TIM定时器
  ******************************************************************************/
/*----------------------------------------------------------------------------
  更新日志:
  2016-10-18 V1.0.0:初始版本
  ----------------------------------------------------------------------------*/
/* 包含的头文件 --------------------------------------------------------------*/
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
/*                            STM32-传感器数据                                */
uint8_t RxData[32];																																
uint8_t RxLen = 0;         
uint8_t TxData[13];
uint8_t TxLen = 8;   
uint8_t KeyFlag = 0;


/*                              所测得物理值                                   */
uint32_t PhData = 0;                                                                //PH值
uint8_t PhdataArr[10];																															//存放PH值的数组
uint32_t TempData = 0;																															//温度值
uint8_t TempdataArr[10];																														//存放温度值的数组
float ConData = 0;																																	//电导率值
uint8_t CondataArr[10];																															//存放电导率值得数组
float DoData = 0;
uint8_t DodataArr[10];
 
 
/*                              屏幕相关的数据                                  */                                                              
volatile uint32 screen_time = 0;                                                     //向屏幕上传数据的时间
volatile uint32 sensor_time = 0;                                                     //读取传感器的时间
volatile uint32 ph_connect_time = 5000;                                                 //检测PH传感器是否正常连接
volatile uint32 con_connect_time = 5000;                                                //检测电导率传感器是否正常连接
volatile uint32 do_connect_time = 5000;                                                 //检测溶解氧传感器是否正常连接
volatile uint32 tur_connect_time = 5000;                                                //检测浊度传感器是否正常连接

uint8  cmd_buffer[CMD_MAX_SIZE];                                                     //指令缓存
static uint16 current_screen_id = 0;                                                 //当前画面ID
static float PhCal = 0;                                                              //PH校准                                                                
//static float ConCal = 0;                                                             //电导率校准液的值
//static float TurCal = 0;                                                              //PH校准液的值
long b;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void GetSensorData(void);
float ArrToFloat(uint8 *str);

/*! 
*  \brief  主函数入口
*/
int main(void)
{
	qsize  size = 0; 
	
	//配置定时器
	TIMER_Initializes();
	//配置485通信（STM32-传感器）
	rs485_Init();
	//配置串口（STM32-屏幕）
	USART_Initializes();
	//按键初始化
	GPIO_Key_Configuration();
	
	//清空串口接收缓冲区                                                          
  queue_reset();
	
	//延时等待串口屏初始化完毕,必须等待300ms                                      
  delay_ms(300);
	
  while(1)
  { 
		size = queue_find_cmd(cmd_buffer,CMD_MAX_SIZE);

		if(size>0&&cmd_buffer[1]!=0x07)                                              //接收到指令 ，及判断是否为开机提示
		{                                                                           
				ProcessMessage((PCTRL_MSG)cmd_buffer, size);                             //指令处理  
		}                                                                           
		else if(size>0&&cmd_buffer[1]==0x07)                                         //如果为指令0x07就软重置STM32  
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
    uint8 cmd_type = msg->cmd_type;                                                  //指令类型
    uint8 ctrl_msg = msg->ctrl_msg;                                                  //消息的类型
    uint8 control_type = msg->control_type;                                          //控件类型
    uint16 screen_id = PTR2U16(&msg->screen_id);                                     //画面ID
    uint16 control_id = PTR2U16(&msg->control_id);                                   //控件ID
    uint32 value = PTR2U32(msg->param);                                              //数值


    switch(cmd_type)
    {  
    case NOTIFY_TOUCH_PRESS:                                                        //触摸屏按下
    case NOTIFY_TOUCH_RELEASE:                                                      //触摸屏松开
        NotifyTouchXY(cmd_buffer[1],PTR2U16(cmd_buffer+2),PTR2U16(cmd_buffer+4)); 
        break;                                                                    
    case NOTIFY_WRITE_FLASH_OK:                                                     //写FLASH成功
        NotifyWriteFlash(1);                                                      
        break;                                                                    
    case NOTIFY_WRITE_FLASH_FAILD:                                                  //写FLASH失败
        NotifyWriteFlash(0);                                                      
        break;                                                                    
    case NOTIFY_READ_FLASH_OK:                                                      //读取FLASH成功
        NotifyReadFlash(1,cmd_buffer+2,size-6);                                     //去除帧头帧尾
        break;                                                                    
    case NOTIFY_READ_FLASH_FAILD:                                                   //读取FLASH失败
        NotifyReadFlash(0,0,0);                                                   
        break;                                                                    
    case NOTIFY_READ_RTC:                                                           //读取RTC时间
        NotifyReadRTC(cmd_buffer[2],cmd_buffer[3],cmd_buffer[4],cmd_buffer[5],cmd_buffer[6],cmd_buffer[7],cmd_buffer[8]);
        break;
    case NOTIFY_CONTROL:
        {
            if(ctrl_msg==MSG_GET_CURRENT_SCREEN)                                    //画面ID变化通知
            {
                NotifyScreen(screen_id);                                            //画面切换调动的函数
            }
            else
            {
                switch(control_type)
                {
                case kCtrlButton:                                                   //按钮控件
                    NotifyButton(screen_id,control_id,msg->param[1]);                  
                    break;                                                             
                case kCtrlText:                                                     //文本控件
                    NotifyText(screen_id,control_id,msg->param);                       
                    break;                                                             
                case kCtrlProgress:                                                 //进度条控件
                    NotifyProgress(screen_id,control_id,value);                        
                    break;                                                             
                case kCtrlSlider:                                                   //滑动条控件
                    NotifySlider(screen_id,control_id,value);                          
                    break;                                                             
                case kCtrlMeter:                                                    //仪表控件
                    NotifyMeter(screen_id,control_id,value);                           
                    break;                                                             
                case kCtrlMenu:                                                     //菜单控件
                    NotifyMenu(screen_id,control_id,msg->param[0],msg->param[1]);      
                    break;                                                              
                case kCtrlSelector:                                                 //选择控件
                    NotifySelector(screen_id,control_id,msg->param[0]);                
                    break;                                                              
                case kCtrlRTC:                                                      //倒计时控件
                    NotifyTimer(screen_id,control_id);
                    break;
                default:
                    break;
                }
            } 
            break;  
        } 
    case NOTIFY_HandShake:                                                          //握手通知                                                     
        NOTIFYHandShake();
        break;
    default:
        break;
    }
}

/*! 
*  \brief  握手通知
*/
void NOTIFYHandShake()
{
   SetButtonValue(3,2,1);
}

/*! 
*  \brief  画面切换通知
*  \details  当前画面改变时(或调用GetScreen)，执行此函数
*  \param screen_id 当前画面ID
*/
void NotifyScreen(uint16 screen_id)
{
    //TODO: 添加用户代码
    current_screen_id = screen_id;                                                   //在工程配置中开启画面切换通知，记录当前画面ID

}

/*! 
*  \brief  触摸坐标事件响应
*  \param press 1按下触摸屏，3松开触摸屏
*  \param x x坐标
*  \param y y坐标
*/
void NotifyTouchXY(uint8 press,uint16 x,uint16 y)
{ 
    //TODO: 添加用户代码
}


/*! 
*  \brief  更新数据
*/ 
void UpdateUI()
{
    //文本设置和显示  定时500ms刷新一次
    if(current_screen_id==0)                                              
    {
			//每500ms将采集到的传感器数据显示在屏幕上
			BatchBegin(0);
			BatchSetText(7, PhdataArr);                                               //设置PH文本值
			BatchSetText(12, CondataArr);                                             //设置电导率本值
			BatchSetText(13, DodataArr);                                              //设置溶解氧文本值
			BatchSetText(15, TempdataArr);                                            //设置温度本值	
			
			if(ph_connect_time < 5000)
			{
				BatchSetText(22,"连接正常");
			}
			else
			{
				BatchSetText(22,"未连接");
			}
			
			if(con_connect_time < 5000)
			{
				BatchSetText(25,"连接正常");
			}
			else
			{
				BatchSetText(25,"未连接");
			}
			
			if(do_connect_time < 5000)
			{
				BatchSetText(27,"连接正常");
			}
			else
			{
				BatchSetText(27,"未连接");
			}
			
			if(tur_connect_time < 5000)
			{
				BatchSetText(29,"连接正常");
			}
			else
			{
				BatchSetText(29,"未连接");
			}

			BatchEnd();
    }                                                                            
                                                                                                                                                        
}

/*! 
*  \brief  按钮控件通知
*  \details  当按钮状态改变(或调用GetControlValue)时，执行此函数
*  \param screen_id 画面ID
*  \param control_id 控件ID
*  \param state 按钮状态：0弹起，1按下
*/
void NotifyButton(uint16 screen_id, uint16 control_id, uint8  state)
{ 
	//按键校准PH
	if(screen_id == 3)
	{
		if(control_id==1 && state==1)                                                       //进入校准模式                                                       
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
		
		if(control_id==10 && state==1)                                                      //校准酸性标准液
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
		
		if(control_id==9 && state==1)                                                       //校准中性标准液
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
		
		if(control_id==11 && state==1)                                                       //校准碱性标准液
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
	
	//校准电导率
	if(screen_id == 8)
	{
		if(control_id==10 && state==1)                                                        //进入校准模式
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
*  \brief  文本控件通知
*  \details  当文本通过键盘更新(或调用GetControlValue)时，执行此函数
*  \details  文本控件的内容以字符串形式下发到MCU，如果文本控件内容是浮点值，
*  \details  则需要在此函数中将下发字符串重新转回浮点值。
*  \param screen_id 画面ID
*  \param control_id 控件ID
*  \param str 文本控件内容
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
*  \brief  进度条控件通知                                                       
*  \details  调用GetControlValue时，执行此函数                                  
*  \param screen_id 画面ID                                                      
*  \param control_id 控件ID                                                     
*  \param value 值                                                              
*/                                                                              
void NotifyProgress(uint16 screen_id, uint16 control_id, uint32 value)           
{  

}                                                                                

/*!                                                                              
*  \brief  滑动条控件通知                                                       
*  \details  当滑动条改变(或调用GetControlValue)时，执行此函数                  
*  \param screen_id 画面ID                                                      
*  \param control_id 控件ID                                                     
*  \param value 值                                                              
*/                                                                              
void NotifySlider(uint16 screen_id, uint16 control_id, uint32 value)             
{                                                             

}

/*! 
*  \brief  仪表控件通知
*  \details  调用GetControlValue时，执行此函数
*  \param screen_id 画面ID
*  \param control_id 控件ID
*  \param value 值
*/
void NotifyMeter(uint16 screen_id, uint16 control_id, uint32 value)
{
    //TODO: 添加用户代码
}

/*! 
*  \brief  菜单控件通知
*  \details  当菜单项按下或松开时，执行此函数
*  \param screen_id 画面ID
*  \param control_id 控件ID
*  \param item 菜单项索引
*  \param state 按钮状态：0松开，1按下
*/
void NotifyMenu(uint16 screen_id, uint16 control_id, uint8 item, uint8 state)
{
    //TODO: 添加用户代码
}

/*! 
*  \brief  选择控件通知
*  \details  当选择控件变化时，执行此函数
*  \param screen_id 画面ID
*  \param control_id 控件ID
*  \param item 当前选项
*/
void NotifySelector(uint16 screen_id, uint16 control_id, uint8  item)
{

}

/*! 
*  \brief  定时器超时通知处理
*  \param screen_id 画面ID
*  \param control_id 控件ID
*/
void NotifyTimer(uint16 screen_id, uint16 control_id)
{

}

/*! 
*  \brief  读取用户FLASH状态返回
*  \param status 0失败，1成功
*  \param _data 返回数据
*  \param length 数据长度
*/
void NotifyReadFlash(uint8 status,uint8 *_data,uint16 length)
{
    //TODO: 添加用户代码
}

/*! 
*  \brief  写用户FLASH状态返回
*  \param status 0失败，1成功
*/
void NotifyWriteFlash(uint8 status)
{
    //TODO: 添加用户代码
}

/*! 
*  \brief  读取RTC时间，注意返回的是BCD码
*  \param year 年（BCD）
*  \param month 月（BCD）
*  \param week 星期（BCD）
*  \param day 日（BCD）
*  \param hour 时（BCD）
*  \param minute 分（BCD）
*  \param second 秒（BCD）
*/
void NotifyReadRTC(uint8 year,uint8 month,uint8 week,uint8 day,uint8 hour,uint8 minute,uint8 second)
{

}

/*! 
*  \brief  将屏幕发送来的文本控件数组转化为float
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
*  \brief  向传感器发送读取指令
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

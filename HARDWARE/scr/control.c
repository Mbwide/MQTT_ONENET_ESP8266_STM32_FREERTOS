/*-------------------------------------------------*/
/*                                                 */
/* 		发送控制设备数据，存放至发送缓冲区         */
/*                                                 */
/*-------------------------------------------------*/

//LED控制数据发送给服务器

#include "stm32f10x.h"
#include "control.h"
#include "usart1.h"
#include "mqtt.h"
   
#include "FreeRTOS.h"
#include "task.h"

/*---------------------------------------------------------------*/
/*函数名：send_data(const char *laber, char *flag)               */
/*功  能：处理待发送的控制设备数据（固定）						 */
/*		  1.处理待发送的控制设备数据，移入MQTT数据发送缓冲区      */
/*参  数：1.const char *laber :设备标签，发送给ONENET的数据流名称 */
/*        2.char *flag :设备状态，即发送给ONENET的数据流具体数值  */
/*返回值：无                                       			     */
/*---------------------------------------------------------------*/
void send_data(const char *laber, char *flag)
{	
	char data_buffer[255] = {0}; //数据包缓存区,初始化为0
	int data_len = 0;			 //数据包总长度，初始化为0
	int data_msg_len = 0;		 //消息体长度，初始化为0
	
	/*  数据包解析（数据类型3）
	 *	0:0x03 固定报头（数据点类型，此处为3）
	 *	1:	   消息体长度高字节（消息体长度不超过255个字节则为0x00）
	 *	2:	   消息体长度低字节（消息体长度，即数据包长度，自己设置）
	 *	3-结尾 消息体
	 *  消息体：
	 *  {
	 *		"data_1":"value_1",
	 *		"data_2":"value_2"
	 *	}
	 *	消息体示例：
	 *	{"led1_flag":"LED1ON"}
	 */	
	sprintf(data_buffer + 3, "{\"%s\":\"%s\"}", laber, flag);//构建报文
	
	data_msg_len = strlen(data_buffer + 3);			//消息体长度计算
	data_buffer[0] = 0x03;							//固定报头
	data_buffer[1] = data_msg_len >> 8 ; 			//消息体长度高字节
	data_buffer[2] = data_msg_len & 0xFF;			//消息体长度低字节
	data_len = data_msg_len + 3;   		 	    	//数据包总长度
	
	printf("%s\r\n", data_buffer + 3);		  		//消息体通过串口回显
	
	taskENTER_CRITICAL(); //进入临界区，防止打断
	MQTT_PublishQs0(DATA_TOPIC_NAME, data_buffer, data_len);//添加数据，存放至发送缓冲区
	taskEXIT_CRITICAL();  //退出临界区
}


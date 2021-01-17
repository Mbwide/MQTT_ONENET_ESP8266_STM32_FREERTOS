/*-------------------------------------------------*/
/*                                                 */
/* 		���Ϳ����豸���ݣ���������ͻ�����         */
/*                                                 */
/*-------------------------------------------------*/

//LED�������ݷ��͸�������

#include "stm32f10x.h"
#include "control.h"
#include "usart1.h"
#include "mqtt.h"
   
#include "FreeRTOS.h"
#include "task.h"

/*---------------------------------------------------------------*/
/*��������send_data(const char *laber, char *flag)               */
/*��  �ܣ���������͵Ŀ����豸���ݣ��̶���						 */
/*		  1.��������͵Ŀ����豸���ݣ�����MQTT���ݷ��ͻ�����      */
/*��  ����1.const char *laber :�豸��ǩ�����͸�ONENET������������ */
/*        2.char *flag :�豸״̬�������͸�ONENET��������������ֵ  */
/*����ֵ����                                       			     */
/*---------------------------------------------------------------*/
void send_data(const char *laber, char *flag)
{	
	char data_buffer[255] = {0}; //���ݰ�������,��ʼ��Ϊ0
	int data_len = 0;			 //���ݰ��ܳ��ȣ���ʼ��Ϊ0
	int data_msg_len = 0;		 //��Ϣ�峤�ȣ���ʼ��Ϊ0
	
	/*  ���ݰ���������������3��
	 *	0:0x03 �̶���ͷ�����ݵ����ͣ��˴�Ϊ3��
	 *	1:	   ��Ϣ�峤�ȸ��ֽڣ���Ϣ�峤�Ȳ�����255���ֽ���Ϊ0x00��
	 *	2:	   ��Ϣ�峤�ȵ��ֽڣ���Ϣ�峤�ȣ������ݰ����ȣ��Լ����ã�
	 *	3-��β ��Ϣ��
	 *  ��Ϣ�壺
	 *  {
	 *		"data_1":"value_1",
	 *		"data_2":"value_2"
	 *	}
	 *	��Ϣ��ʾ����
	 *	{"led1_flag":"LED1ON"}
	 */	
	sprintf(data_buffer + 3, "{\"%s\":\"%s\"}", laber, flag);//��������
	
	data_msg_len = strlen(data_buffer + 3);			//��Ϣ�峤�ȼ���
	data_buffer[0] = 0x03;							//�̶���ͷ
	data_buffer[1] = data_msg_len >> 8 ; 			//��Ϣ�峤�ȸ��ֽ�
	data_buffer[2] = data_msg_len & 0xFF;			//��Ϣ�峤�ȵ��ֽ�
	data_len = data_msg_len + 3;   		 	    	//���ݰ��ܳ���
	
	printf("%s\r\n", data_buffer + 3);		  		//��Ϣ��ͨ�����ڻ���
	
	taskENTER_CRITICAL(); //�����ٽ�������ֹ���
	MQTT_PublishQs0(DATA_TOPIC_NAME, data_buffer, data_len);//������ݣ���������ͻ�����
	taskEXIT_CRITICAL();  //�˳��ٽ���
}


/*-------------------------------------------------*/
/*                                                 */
/*            	   �жϷ�����          	   	   */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32f10x.h"     //������Ҫ��ͷ�ļ�
#include "FreeRTOS.h"	   //FreeRTOSʹ��		  
#include "task.h"
#include "event_groups.h"
#include "stm32f10x_it.h"  //������Ҫ��ͷ�ļ�
#include "usart1.h"        //������Ҫ��ͷ�ļ�
#include "usart2.h"        //������Ҫ��ͷ�ļ�
#include "timer3.h"        //������Ҫ��ͷ�ļ�
#include "mqtt.h"          //������Ҫ��ͷ�ļ�
#include "dht11.h"         //������Ҫ��ͷ�ļ�                  

extern void xPortSysTickHandler(void);
extern TaskHandle_t WIFI_Task_Handler;
extern  EventGroupHandle_t Event_Handle;
extern const int PING_MODE;

/*---------------------------------------------------------------*/
/*��������void USART2_IRQHandler(void) 			      			 */
/*��  �ܣ�����2�жϴ�����										 */
/*		  1.��esp8266ͨ�ţ��Ѿ����ӷ��������ƶ�ʱ��4��δ���ӷ����� */
/*			�����ƶ�ʱ��4��ͨ���¼���־���λ0 WIFI_CONECT�жϣ�	 */
/*��  ������                                       				 */
/*����ֵ����                                     				 */
/*---------------------------------------------------------------*/
void USART2_IRQHandler(void)   
{   
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)         //���USART_IT_RXNE��־��λ����ʾ�����ݵ��ˣ�����if��֧	   
	{   
		if ((xEventGroupGetBitsFromISR(Event_Handle) & 0x01) == 0)//��ȡ�¼���־�����ݣ�����0˵��δ���ӷ���������������ʱ��4��MQTT�������ݴ�����ʱ��
		{
			if(USART2->DR)                                        //����ָ������״̬ʱ������ֵ�ű��浽������	
			{                                     			 
				Usart2_RxBuff[Usart2_RxCounter] = USART2->DR;	  //���浽������	
				Usart2_RxCounter++; 						      //ÿ����1���ֽڵ����ݣ�Usart2_RxCounter��1����ʾ���յ���������+1 
			}		
		}
		else
		{
			Usart2_RxBuff[Usart2_RxCounter] = USART2->DR;//�ѽ��յ������ݱ��浽Usart2_RxBuff��
				
			if(Usart2_RxCounter == 0)				     //���Usart2_RxCounter����0����ʾ�ǽ��յĵ�1�����ݣ�����if��֧	
			{    								
				TIM_Cmd(TIM4, ENABLE); 					 //ʹ�ܶ�ʱ��4
			}
			else										 //else��֧����ʾ��Usart2_RxCounter������0�����ǽ��յĵ�һ������
			{                        									    
				TIM_SetCounter(TIM4, 0);  				 //��λ��ʱ��4
			}	
			Usart2_RxCounter++;         				 //ÿ����1���ֽڵ����ݣ�Usart2_RxCounter��1����ʾ���յ���������+1 				
		}	
	} 
}

/*---------------------------------------------------------------*/
/*��������void TIM4_IRQHandler(void)				      			 */
/*��  �ܣ���ʱ��4�жϴ�����									 */
/*		  1.������2���յ���MQTT���ݣ������ڽ��ջ��帴�Ƶ�MQTT����*/
/*			����        										 */
/*��  ������                                       				 */
/*����ֵ����                                     				 */
/*---------------------------------------------------------------*/
void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)//���TIM_IT_Update��λ����ʾTIM4����жϣ�����if	
	{                	
		memcpy(&MQTT_RxDataInPtr[2], Usart2_RxBuff, Usart2_RxCounter);  //�������ݵ����ջ�����
		MQTT_RxDataInPtr[0] = Usart2_RxCounter/256;                  	//��¼���ݳ��ȸ��ֽ�
		MQTT_RxDataInPtr[1] = Usart2_RxCounter%256;					 	//��¼���ݳ��ȵ��ֽ�
		MQTT_RxDataInPtr += RBUFF_UNIT;                                	//ָ������
		if(MQTT_RxDataInPtr == MQTT_RxDataEndPtr)                     	//���ָ�뵽������β����
			MQTT_RxDataInPtr = MQTT_RxDataBuf[0];                    	//ָ���λ����������ͷ
		Usart2_RxCounter = 0;                                        	//����2������������������
		TIM_SetCounter(TIM3, 0);                                     	//���㶨ʱ��3�����������¼�ʱping������ʱ��
		TIM_Cmd(TIM4, DISABLE);                        				 	//�ر�TIM4��ʱ��
		TIM_SetCounter(TIM4, 0);                        			 	//���㶨ʱ��4������
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);     			 	//���TIM4����жϱ�־ 	
	}
}

/*---------------------------------------------------------------*/
/*��������void TIM3_IRQHandler(void)				      			 */
/*��  �ܣ���ʱ��3�жϴ�����									 */
/*		  1.����ping�������ķ���									 */
/*��  ������                                       				 */
/*����ֵ����                                     				 */
/*��  ������ο��ٷ��ͣ�2s��5�Σ�û�з�Ӧ��wifi�����ɹ���̬->����̬*/
/*---------------------------------------------------------------*/
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)//���TIM_IT_Update��λ����ʾTIM3����жϣ�����if	
	{  
		switch(pingFlag) 					//�ж�pingFlag��״̬
		{                               
			case 0:							//���pingFlag����0����ʾ����״̬������Ping����  
					MQTT_PingREQ(); 		//���Ping���ĵ����ͻ�����  
					break;
			case 1:							//���pingFlag����1��˵����һ�η��͵���ping���ģ�û���յ��������ظ�������1û�б����Ϊ0�������������쳣������Ҫ��������pingģʽ
					TIM3_ENABLE_2S(); 	    //���ǽ���ʱ��6����Ϊ2s��ʱ,���ٷ���Ping����
					xEventGroupClearBitsFromISR(Event_Handle, PING_MODE);//�رշ���PING���Ķ�ʱ��3�������¼���־λ
					MQTT_PingREQ();			//���Ping���ĵ����ͻ�����  
					break;
			case 2:							//���pingFlag����2��˵����û���յ��������ظ�
			case 3:				            //���pingFlag����3��˵����û���յ��������ظ�
			case 4:				            //���pingFlag����4��˵����û���յ��������ظ�	
					MQTT_PingREQ();  		//���Ping���ĵ����ͻ����� 
					break;
			case 5:							//���pingFlag����5��˵�����Ƿ����˶��ping�����޻ظ���Ӧ�������������⣬������������
					xTaskResumeFromISR(WIFI_Task_Handler);        //����״̬��0����ʾ�Ͽ���û���Ϸ�����
					TIM_Cmd(TIM3, DISABLE); //��TIM3 				
					break;			
		}
		pingFlag++;           		   		//pingFlag����1����ʾ�ַ�����һ��ping���ڴ��������Ļظ�
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update); //���TIM3����жϱ�־ 	
	}
}



/*-------------------------------------------------*/
/*�����������������жϴ�����                     */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void NMI_Handler(void)
{

}

/*-------------------------------------------------*/
/*��������Ӳ������������жϴ�����             */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void HardFault_Handler(void)
{

}

/*-------------------------------------------------*/
/*���������ڴ�����жϴ�����                     */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void MemManage_Handler(void)
{

}

/*-------------------------------------------------*/
/*��������Ԥȡָʧ�ܣ��洢������ʧ���жϴ�����   */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void BusFault_Handler(void)
{

}

/*-------------------------------------------------*/
/*��������δ�����ָ���Ƿ�״̬������           */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void UsageFault_Handler(void)
{

}

/*-------------------------------------------------*/
/*�����������жϣ�SWI ָ����õĴ�����           */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
//void SVC_Handler(void)
//{
//	
//}

/*-------------------------------------------------*/
/*�����������Լ����������                       */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void DebugMon_Handler(void)
{
	
}

/*-------------------------------------------------*/
/*���������ɹ����ϵͳ��������                 */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
//void PendSV_Handler(void)
//{
//}

/*-------------------------------------------------*/
/*��������SysTicϵͳ��શ�ʱ��������             */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void SysTick_Handler(void)
{
	if(xTaskGetSchedulerState()!=taskSCHEDULER_NOT_STARTED)//ϵͳ�Ѿ�����
    {
        xPortSysTickHandler();	
    }
}

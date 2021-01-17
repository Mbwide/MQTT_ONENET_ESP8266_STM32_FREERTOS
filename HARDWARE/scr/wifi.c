
/*-------------------------------------------------*/
/*                                                 */
/*          	 WIFI��ESP8266��Դ�ļ�             */
/*                                                 */
/*-------------------------------------------------*/

// Ӳ�����ӣ�
// PA2 RX
// PA3 TX
// PA4 ��λ

#include "stm32f10x.h"  //������Ҫ��ͷ�ļ�
#include "wifi.h"	    //������Ҫ��ͷ�ļ�
#include "delay.h"	    //������Ҫ��ͷ�ļ�
#include "usart1.h"	    //������Ҫ��ͷ�ļ�

	
/*-------------------------------------------------*/
/*����������ʼ��WiFi�ĸ�λIO                       */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void wifi_reset_io_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;                    //����һ������IO�˿ڲ����Ľṹ��
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA , ENABLE); //ʹ��PA�˿�ʱ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;               //׼������PA4
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       //����50Mhz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   	    //���������ʽ
	GPIO_Init(GPIOA, &GPIO_InitStructure);            	    //����PA4
	RESET_IO(1);                                            //��λIO���ߵ�ƽ
}
/*-------------------------------------------------*/
/*��������WiFi��������ָ��                         */
/*��  ����cmd��ָ��                                */
/*��  ����timeout����ʱʱ�䣨100ms�ı�����         */
/*����ֵ��0����ȷ   ����������                     */
/*-------------------------------------------------*/
char WiFi_SendCmd(char *cmd, int timeout)
{
	WiFi_RxCounter = 0;                           	//WiFi������������������                        
	memset(WiFi_RX_BUF, 0, WiFi_RXBUFF_SIZE);     	//���WiFi���ջ����� 
	WiFi_printf("%s\r\n", cmd);                  	//����ָ��
	while(timeout--)								//�ȴ���ʱʱ�䵽0
	{                           			
		delay_ms(100);                              //��ʱ100ms
		if(strstr(WiFi_RX_BUF, "OK"))              	//������յ�OK��ʾָ��ɹ�
			break;       							//��������whileѭ��
		printf("%d ", timeout);                 	//����������ڵĳ�ʱʱ��
	}			
	printf("\r\n");                          			   
	if(timeout <= 0)return 1;                       //���timeout<=0��˵����ʱʱ�䵽�ˣ�Ҳû���յ�OK������1
	else return 0;		         					//��֮����ʾ��ȷ��˵���յ�OK��ͨ��break��������while
}
/*-------------------------------------------------*/
/*��������WiFi��λ                                 */
/*��  ����timeout����ʱʱ�䣨100ms�ı�����         */
/*����ֵ��0����ȷ   ����������                     */
/*-------------------------------------------------*/
char WiFi_Reset(int timeout)
{
	RESET_IO(0);                                    //��λIO���͵�ƽ
	delay_ms(500);                                  //��ʱ500ms
	RESET_IO(1);                                   	//��λIO���ߵ�ƽ	
	while(timeout--)								//�ȴ���ʱʱ�䵽0 
	{                              		  
		delay_ms(100);                              //��ʱ100ms
		if(strstr(WiFi_RX_BUF, "ready"))            //������յ�ready��ʾ��λ�ɹ�
			break;       						    //��������whileѭ��
		printf("%d ", timeout);                     //����������ڵĳ�ʱʱ��
	}
	printf("\r\n");                              
	if(timeout <= 0)return 1;                       //���timeout<=0��˵����ʱʱ�䵽�ˣ�Ҳû���յ�ready������1
	else return 0;		         				   	//��֮����ʾ��ȷ��˵���յ�ready��ͨ��break��������while
}
/*-------------------------------------------------*/
/*��������WiFi����·����ָ��                       */
/*��  ����timeout����ʱʱ�䣨1s�ı�����            */
/*����ֵ��0����ȷ   ����������                     */
/*-------------------------------------------------*/
char WiFi_JoinAP(int timeout)
{		
	WiFi_RxCounter = 0;                                    //WiFi������������������                        
	memset(WiFi_RX_BUF, 0, WiFi_RXBUFF_SIZE);              //���WiFi���ջ����� 
	WiFi_printf("AT+CWJAP=\"%s\",\"%s\"\r\n", SSID, PASS); //����ָ��	
	while(timeout--)									   //�ȴ���ʱʱ�䵽0
	{                                   
		delay_ms(1000);                             	   //��ʱ1s
		if(strstr(WiFi_RX_BUF, "WIFI GOT IP\r\n\r\nOK"))   //������յ�WIFI GOT IP��ʾ�ɹ�
			break;       						           //��������whileѭ��
		printf("%d ", timeout);                            //����������ڵĳ�ʱʱ��
	}
	printf("\r\n");                             	       //���������Ϣ
	if(timeout <= 0)return 1;                              //���timeout<=0��˵����ʱʱ�䵽�ˣ�Ҳû���յ�WIFI GOT IP������1
	return 0;                                              //��ȷ������0
}
/*-------------------------------------------------*/
/*������������TCP��������������͸��ģʽ            */
/*��  ����timeout�� ��ʱʱ�䣨100ms�ı�����        */
/*����ֵ��0����ȷ  ����������                      */
/*-------------------------------------------------*/
char WiFi_Connect_Server(int timeout)
{	
	WiFi_RxCounter = 0;                               //WiFi������������������                        
	memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);           //���WiFi���ջ�����   
	WiFi_printf("AT+CIPSTART=\"TCP\",\"%s\",%d\r\n", ServerIP, ServerPort);//�������ӷ�����ָ��
	while(timeout--)								  //�ȴ���ʱ���
	{                           
		delay_ms(100);                             	  //��ʱ100ms	
		if(strstr(WiFi_RX_BUF, "CONNECT"))            //������ܵ�CONNECT��ʾ���ӳɹ�
			break;                                    //����whileѭ��
		if(strstr(WiFi_RX_BUF, "CLOSED"))             //������ܵ�CLOSED��ʾ������δ����
			return 1;                                 //������δ��������1
		if(strstr(WiFi_RX_BUF, "ALREADY CONNECTED"))  //������ܵ�ALREADY CONNECTED�Ѿ���������
			return 2;                                 //�Ѿ��������ӷ���2
		printf("%d ", timeout);                       //����������ڵĳ�ʱʱ��  
	}
	printf("\r\n");                                   
	if(timeout <= 0)return 3;                         //��ʱ���󣬷���3
	else                                              //���ӳɹ���׼������͸��
	{
		printf("���ӷ������ɹ���׼������͸��\r\n");   
		WiFi_RxCounter = 0;                           //WiFi������������������                        
		memset(WiFi_RX_BUF, 0, WiFi_RXBUFF_SIZE);     //���WiFi���ջ�����     
		WiFi_printf("AT+CIPSEND\r\n");                //���ͽ���͸��ָ��
		while(timeout--)							  //�ȴ���ʱ���
		{                            
			delay_ms(100);                            //��ʱ100ms	
			if(strstr(WiFi_RX_BUF, "\r\nOK\r\n\r\n>"))//���������ʾ����͸���ɹ�
				break;                          	  //����whileѭ��
			printf("%d ", timeout);                   //����������ڵĳ�ʱʱ��  
		}
		if(timeout <= 0)return 4;                     //͸����ʱ���󣬷���4	
	}
	return 0;	                                      //�ɹ�����0	
}
/*-------------------------------------------------*/
/*��������WiFi_Smartconfig                         */
/*��  ����timeout����ʱʱ�䣨1s�ı�����            */
/*����ֵ��0����ȷ   ����������                     */
/*-------------------------------------------------*/
char WiFi_Smartconfig(int timeout)
{
	
	WiFi_RxCounter = 0;                           		//WiFi������������������                        
	memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);     		//���WiFi���ջ�����     
	while(timeout--)									//�ȴ���ʱʱ�䵽0
	{                           		
		delay_ms(1000);                         		//��ʱ1s
		if(strstr(WiFi_RX_BUF, "connected"))    	 	//������ڽ��ܵ�connected��ʾ�ɹ�
			break;                                  	//����whileѭ��  
		printf("%d ", timeout);                 		//����������ڵĳ�ʱʱ��  
	}	
	printf("\r\n");                          			
	if(timeout <= 0)return 1;                     		//��ʱ���󣬷���1
	return 0;                                   		//��ȷ����0
}
/*-------------------------------------------------*/
/*���������ȴ�����·����                           */
/*��  ����timeout����ʱʱ�䣨1s�ı�����            */
/*����ֵ��0����ȷ   ����������                     */
/*-------------------------------------------------*/
char WiFi_WaitAP(int timeout)
{		
	while(timeout--){                               //�ȴ���ʱʱ�䵽0
		delay_ms(1000);                             //��ʱ1s
		if(strstr(WiFi_RX_BUF, "WIFI GOT IP"))      //������յ�WIFI GOT IP��ʾ�ɹ�
			break;       						 
		printf("%d ", timeout);                     //����������ڵĳ�ʱʱ��
	}
	printf("\r\n");                             	//���������Ϣ
	if(timeout <= 0)return 1;                       //���timeout<=0��˵����ʱʱ�䵽�ˣ�Ҳû���յ�WIFI GOT IP������1
	return 0;                                       //��ȷ������0
}
/*-------------------------------------------------*/
/*��������WiFi���ӷ�����                           */
/*��  ������                                       */
/*����ֵ��0����ȷ   ����������                     */
/*-------------------------------------------------*/
char WiFi_Connect_IoTServer(void)
{	
	printf("׼����λģ��\r\n");                   
	if(WiFi_Reset(50))							  //��λ��100ms��ʱ��λ���ܼ�5s��ʱʱ��
	{                             
		printf("��λʧ�ܣ�׼������\r\n");	      //���ط�0ֵ������if
		return 1;                                 //����1
	}else printf("��λ�ɹ�\r\n");                 
	
	printf("׼������STAģʽ\r\n");                
	if(WiFi_SendCmd("AT+CWMODE=1",50))			  //����STAģʽ��100ms��ʱ��λ���ܼ�5s��ʱʱ��
	{             
		printf("����STAģʽʧ�ܣ�׼������\r\n");  //���ط�0ֵ������if
		return 2;                                 //����2
	}else printf("����STAģʽ�ɹ�\r\n");          
	                            
	printf("׼��ȡ���Զ�����\r\n");            	  
	if(WiFi_SendCmd("AT+CWAUTOCONN=0",50))		  //ȡ���Զ����ӣ�100ms��ʱ��λ���ܼ�5s��ʱʱ��
	{       
		printf("ȡ���Զ�����ʧ�ܣ�׼������\r\n"); //���ط�0ֵ������if
		return 3;                                 //����3
	}else printf("ȡ���Զ����ӳɹ�\r\n");         
			
	printf("׼������·����\r\n");                 	
	if(WiFi_JoinAP(30))							  //����·����,1s��ʱ��λ���ܼ�30s��ʱʱ��
	{                          
		printf("����·����ʧ�ܣ�׼������\r\n");   //���ط�0ֵ������if
		return 4;                                 //����4	
	}else printf("����·�����ɹ�\r\n");       		

	printf("׼������͸��\r\n");                    
	if(WiFi_SendCmd("AT+CIPMODE=1",50)) 		  //����͸����100ms��ʱ��λ���ܼ�5s��ʱʱ��
	{           
		printf("����͸��ʧ�ܣ�׼������\r\n");     //���ط�0ֵ������if
		return 8;                                 //����8
	}else printf("����͸���ɹ�\r\n");              
	
	printf("׼���رն�·����\r\n");               
	if(WiFi_SendCmd("AT+CIPMUX=0",50)) 		      //�رն�·���ӣ�100ms��ʱ��λ���ܼ�5s��ʱʱ��
	{            
		printf("�رն�·����ʧ�ܣ�׼������\r\n"); //���ط�0ֵ������if
		return 9;                                 //����9
	}else printf("�رն�·���ӳɹ�\r\n");         
	 
	printf("׼�����ӷ�����\r\n");                 
	if(WiFi_Connect_Server(100))      			  //���ӷ�������100ms��ʱ��λ���ܼ�10s��ʱʱ��
	{            
		printf("���ӷ�����ʧ�ܣ�׼������\r\n");   //���ط�0ֵ������if
		return 10;                                //����10
	}else printf("���ӷ������ɹ�\r\n");           
	return 0;                                     //��ȷ����0
}
	


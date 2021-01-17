
/*-------------------------------------------------*/
/*                                                 */
/*            ʵ��MQTTЭ�鹦�ܵ�Դ�ļ�             */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32f10x.h"    //������Ҫ��ͷ�ļ�
#include "mqtt.h"         //������Ҫ��ͷ�ļ�
#include "string.h"       //������Ҫ��ͷ�ļ�
#include "stdio.h"        //������Ҫ��ͷ�ļ�
#include "usart1.h"       //������Ҫ��ͷ�ļ�
#include "wifi.h"         //������Ҫ��ͷ�ļ�

unsigned char  MQTT_RxDataBuf[R_NUM][RBUFF_UNIT];  //���ݵĽ��ջ�����,���з��������������ݣ�����ڸû�����,��������һ���ֽڴ�����ݳ���
unsigned char *MQTT_RxDataInPtr;                   //ָ����ջ�����������ݵ�λ��
unsigned char *MQTT_RxDataOutPtr;                  //ָ����ջ�������ȡ���ݵ�λ��
unsigned char *MQTT_RxDataEndPtr;                  //ָ����ջ�����������λ��

unsigned char  MQTT_TxDataBuf[T_NUM][TBUFF_UNIT];  //���ݵķ��ͻ�����,���з��������������ݣ�����ڸû�����,��������һ���ֽڴ�����ݳ���
unsigned char *MQTT_TxDataInPtr;                   //ָ���ͻ�����������ݵ�λ��
unsigned char *MQTT_TxDataOutPtr;                  //ָ���ͻ�������ȡ���ݵ�λ��
unsigned char *MQTT_TxDataEndPtr;                  //ָ���ͻ�����������λ��

unsigned char  MQTT_CMDBuf[C_NUM][CBUFF_UNIT];     //�������ݵĽ��ջ�����
unsigned char *MQTT_CMDInPtr;                      //ָ���������������ݵ�λ��
unsigned char *MQTT_CMDOutPtr;                     //ָ�����������ȡ���ݵ�λ��
unsigned char *MQTT_CMDEndPtr;                     //ָ���������������λ��

char ClientID[128];                                //��ſͻ���ID�Ļ�����
int  ClientID_len;                                 //��ſͻ���ID�ĳ���

char Username[128];                                //����û����Ļ�����
int  Username_len;								   //����û����ĳ���

char Passward[128];                                //�������Ļ�����
int  Passward_len;								   //�������ĳ���

char ServerIP[128];                                //��ŷ�����IP��������
int  ServerPort;                                   //��ŷ������Ķ˿ں�

int   Fixed_len;                       			   //�̶���ͷ����
int   Variable_len;                     		   //�ɱ䱨ͷ����
int   Payload_len;                       		   //��Ч���ɳ���
unsigned char  temp_buff[TBUFF_UNIT];			   //��ʱ������������������

char pingFlag = 0;       //ping����״̬       0������״̬���ȴ���ʱʱ�䵽������Ping����
                         //ping����״̬       1��Ping�����ѷ��ͣ����յ� �������ظ����ĵĺ� ��1��Ϊ0

/*----------------------------------------------------------*/
/*����������ʼ������,����,�������ݵ� ������ �Լ���״̬����  */
/*��  ������                                                */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void MQTT_Buff_Init(void)
{	
	MQTT_RxDataInPtr=MQTT_RxDataBuf[0]; 				 //ָ���ͻ�����������ݵ�ָ���λ
	MQTT_RxDataOutPtr=MQTT_RxDataInPtr; 				 //ָ���ͻ�������ȡ���ݵ�ָ���λ
	MQTT_RxDataEndPtr=MQTT_RxDataBuf[R_NUM-1];		     //ָ���ͻ�����������ָ���λ
	
	MQTT_TxDataInPtr=MQTT_TxDataBuf[0];					 //ָ���ͻ�����������ݵ�ָ���λ
	MQTT_TxDataOutPtr=MQTT_TxDataInPtr;				     //ָ���ͻ�������ȡ���ݵ�ָ���λ
	MQTT_TxDataEndPtr=MQTT_TxDataBuf[T_NUM-1];           //ָ���ͻ�����������ָ���λ
	
	MQTT_CMDInPtr=MQTT_CMDBuf[0];                        //ָ���������������ݵ�ָ���λ
	MQTT_CMDOutPtr=MQTT_CMDInPtr;                        //ָ�����������ȡ���ݵ�ָ���λ
	MQTT_CMDEndPtr=MQTT_CMDBuf[C_NUM-1];              	 //ָ���������������ָ���λ

	MQTT_ConectPack();                                	 //���ͻ�����������ӱ���
	
	pingFlag = 0;   //ping����ֵ����
}

/*----------------------------------------------------------*/
/*���������Ƴ�ʼ���������õ��ͻ���ID���û���������          */
/*��  ������                                                */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void IoT_parameter_init(void)
{	
	memset(ClientID,0,128);                              //�ͻ���ID�Ļ�����ȫ������
	sprintf(ClientID,"%s",DEVICEID);                     //�����ͻ���ID�������뻺����
	ClientID_len = strlen(ClientID);                     //����ͻ���ID�ĳ���
	
	memset(Username,0,128);                              //�û����Ļ�����ȫ������
	sprintf(Username,"%s",PRODUCTID);                    //�����û����������뻺����
	Username_len = strlen(Username);                     //�����û����ĳ���
	
	memset(Passward,0,128);                              //�û����Ļ�����ȫ������
	sprintf(Passward,"%s",AUTHENTICATION);               //�������룬�����뻺����
	Passward_len = strlen(Passward);                     //��������ĳ���
	
	memset(ServerIP,0,128);  
	sprintf(ServerIP,"%s",SERVER_IP);              		//��������������
	ServerPort = SERVER_PORT;                           //�������˿ں�
	
	printf("�� �� ����%s:%d\r\n",ServerIP,ServerPort);  
	printf("�ͻ���ID��%s\r\n",ClientID);                
	printf("�� �� ����%s\r\n",Username);                 
	printf("��    �룺%s\r\n",Passward);                 
}

/*----------------------------------------------------------*/
/*�����������ӷ���������                                    */
/*��  ������                                                */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void MQTT_ConectPack(void)
{	
	int temp,Remaining_len;
	
	Fixed_len = 1;                                                        //���ӱ����У��̶���ͷ������ʱ��=1
	Variable_len = 10;                                                    //���ӱ����У��ɱ䱨ͷ����=10
	Payload_len = 2 + ClientID_len + 2 + Username_len + 2 + Passward_len; //���ӱ����У����س���      
	Remaining_len = Variable_len + Payload_len;                           //ʣ�೤��=�ɱ䱨ͷ����+���س���
	
	temp_buff[0]=0x10;                    //�̶���ͷ��1���ֽ� ���̶�0x01		
	do{                                   //ѭ������̶���ͷ�е�ʣ�೤���ֽڣ��ֽ�������ʣ���ֽڵ���ʵ���ȱ仯
		temp = Remaining_len%128;         //ʣ�೤��ȡ��128
		Remaining_len = Remaining_len/128;//ʣ�೤��ȡ��128
		if(Remaining_len>0)               	
			temp |= 0x80;                 //��Э��Ҫ��λ7��λ          
		temp_buff[Fixed_len] = temp;      //ʣ�೤���ֽڼ�¼һ������
		Fixed_len++;	                  //�̶���ͷ�ܳ���+1    
	}while(Remaining_len > 0);            //���Remaining_len>0�Ļ����ٴν���ѭ��
	
	temp_buff[Fixed_len + 0] = 0x00;      //�ɱ䱨ͷ��1���ֽ� ���̶�0x00	            
	temp_buff[Fixed_len + 1] = 0x04;      //�ɱ䱨ͷ��2���ֽ� ���̶�0x04
	temp_buff[Fixed_len + 2] = 0x4D;	  //�ɱ䱨ͷ��3���ֽ� ���̶�0x4D
	temp_buff[Fixed_len + 3] = 0x51;	  //�ɱ䱨ͷ��4���ֽ� ���̶�0x51
	temp_buff[Fixed_len + 4] = 0x54;	  //�ɱ䱨ͷ��5���ֽ� ���̶�0x54
	temp_buff[Fixed_len + 5] = 0x54;      //�ɱ䱨ͷ��6���ֽ� ���̶�0x54
	temp_buff[Fixed_len + 6] = 0x04;	  //�ɱ䱨ͷ��7���ֽ� ���̶�0x04
	temp_buff[Fixed_len + 7] = 0xC2;	  //�ɱ䱨ͷ��8���ֽ� ��ʹ���û���������У�飬��ʹ���������������Ự
	temp_buff[Fixed_len + 8] = 0x00; 	  //�ɱ䱨ͷ��9���ֽ� ������ʱ����ֽ� 0x00
	temp_buff[Fixed_len + 9] = 0x64;	  //�ɱ䱨ͷ��10���ֽڣ�����ʱ����ֽ� 0x64   100s
	
	/*     CLIENT_ID      */
	temp_buff[Fixed_len+10] = ClientID_len/256;                			  	//�ͻ���ID���ȸ��ֽ�
	temp_buff[Fixed_len+11] = ClientID_len%256;               			  	//�ͻ���ID���ȵ��ֽ�
	memcpy(&temp_buff[Fixed_len+12],ClientID,ClientID_len);                 //���ƹ����ͻ���ID�ִ�	
	/*     �û���        */
	temp_buff[Fixed_len+12+ClientID_len] = Username_len/256; 				//�û������ȸ��ֽ�
	temp_buff[Fixed_len+13+ClientID_len] = Username_len%256; 				//�û������ȵ��ֽ�
	memcpy(&temp_buff[Fixed_len+14+ClientID_len],Username,Username_len);    //���ƹ����û����ִ�	
	/*      ����        */
	temp_buff[Fixed_len+14+ClientID_len+Username_len] = Passward_len/256;	//���볤�ȸ��ֽ�
	temp_buff[Fixed_len+15+ClientID_len+Username_len] = Passward_len%256;	//���볤�ȵ��ֽ�
	memcpy(&temp_buff[Fixed_len+16+ClientID_len+Username_len],Passward,Passward_len); //���ƹ��������ִ�

	TxDataBuf_Deal(temp_buff, Fixed_len + Variable_len + Payload_len);      //���뷢�����ݻ�����
}

/*----------------------------------------------------------*/
/*��������SUBSCRIBE����topic����                            */
/*��  ����QoS�����ĵȼ�                                     */
/*��  ����topic_name������topic��������                     */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void MQTT_Subscribe(char *topic_name, int QoS)
{	
	Fixed_len = 2;                              		   //SUBSCRIBE�����У��̶���ͷ����=2
	Variable_len = 2;                          			   //SUBSCRIBE�����У��ɱ䱨ͷ����=2	
	Payload_len = 2 + strlen(topic_name) + 1;   		   //������Ч���ɳ��� = 2�ֽ�(topic_name����)+ topic_name�ַ����ĳ��� + 1�ֽڷ���ȼ�
	
	temp_buff[0] = 0x82;                                   //��1���ֽ� ���̶�0x82                      
	temp_buff[1] = Variable_len + Payload_len;             //��2���ֽ� ���ɱ䱨ͷ+��Ч���ɵĳ���	
	temp_buff[2] = 0x00;                                   //��3���ֽ� �����ı�ʶ�����ֽڣ��̶�ʹ��0x00
	temp_buff[3] = 0x01;		                           //��4���ֽ� �����ı�ʶ�����ֽڣ��̶�ʹ��0x01
	temp_buff[4] = strlen(topic_name)/256;                 //��5���ֽ� ��topic_name���ȸ��ֽ�
	temp_buff[5] = strlen(topic_name)%256;		           //��6���ֽ� ��topic_name���ȵ��ֽ�
	memcpy(&temp_buff[6], topic_name, strlen(topic_name)); //��7���ֽڿ�ʼ �����ƹ���topic_name�ִ�		
	temp_buff[6 + strlen(topic_name)] = QoS;               //���1���ֽڣ����ĵȼ�
	
	TxDataBuf_Deal(temp_buff, Fixed_len + Variable_len + Payload_len);  //���뷢�����ݻ�����
}

/*----------------------------------------------------------*/
/*��������PING���ģ�������                                   */
/*��  ������                                                */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void MQTT_PingREQ(void)
{
	temp_buff[0] = 0xC0;              //��1���ֽ� ���̶�0xC0                      
	temp_buff[1] = 0x00;              //��2���ֽ� ���̶�0x00 

	TxDataBuf_Deal(temp_buff, 2);     //�������ݵ�������
}

/*----------------------------------------------------------*/
/*���������ȼ�0 ������Ϣ����                                */
/*��  ����topic_name��topic����                             */
/*��  ����data������                                        */ 
/*��  ����data_len�����ݳ���                                */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void MQTT_PublishQs0(const char *topic, char *data, int data_len)
{	
	int temp, Remaining_len;
	
	Fixed_len = 1;                              //�̶���ͷ������ʱ�ȵ��ڣ�1�ֽ�
	Variable_len = 2 + strlen(topic);           //�ɱ䱨ͷ���ȣ�2�ֽ�(topic����)+ topic�ַ����ĳ���
	Payload_len = data_len;                     //��Ч���ɳ��ȣ�����data_len
	Remaining_len = Variable_len + Payload_len; //ʣ�೤��=�ɱ䱨ͷ����+���س���
	
	temp_buff[0] = 0x30;                      	//�̶���ͷ��1���ֽ� ���̶�0x30   	
	do{                                         //ѭ������̶���ͷ�е�ʣ�೤���ֽڣ��ֽ�������ʣ���ֽڵ���ʵ���ȱ仯
		temp = Remaining_len%128;           	//ʣ�೤��ȡ��128
		Remaining_len = Remaining_len/128;      //ʣ�೤��ȡ��128
		if(Remaining_len>0)               	
			temp |= 0x80;                    	//��Э��Ҫ��λ7��λ          
		temp_buff[Fixed_len] = temp;            //ʣ�೤���ֽڼ�¼һ������
		Fixed_len++;	                     	//�̶���ͷ�ܳ���+1    
	}while(Remaining_len>0);                    //���Remaining_len>0�Ļ����ٴν���ѭ��
		             
	temp_buff[Fixed_len+0] = strlen(topic)/256;                       //�ɱ䱨ͷ��1���ֽ�     ��topic���ȸ��ֽ�
	temp_buff[Fixed_len+1] = strlen(topic)%256;		                  //�ɱ䱨ͷ��2���ֽ�     ��topic���ȵ��ֽ�
	memcpy(&temp_buff[Fixed_len+2], topic,strlen(topic));             //�ɱ䱨ͷ��3���ֽڿ�ʼ ������topic�ַ���	
	memcpy(&temp_buff[Fixed_len + 2 + strlen(topic)], data, data_len);//��Ч���ɣ�����data����
	
	TxDataBuf_Deal(temp_buff, Fixed_len + Variable_len + Payload_len);//���뷢�����ݻ�����	
}

/*----------------------------------------------------------*/
/*����������������������ĵȼ�0������                       */
/*��  ����redata�����յ�����                                */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void MQTT_DealPushdata_Qs0(unsigned char *redata)
{
	int  re_len;               	           //����һ����������Ž��յ������ܳ���
	int  pack_num;                         //����һ�����������������һ�����ʱ���������͵ĸ���
    int  temp,temp_len;                    //����һ���������ݴ�����
    int  totle_len;                        //����һ������������Ѿ�ͳ�Ƶ����͵���������
	int  topic_len;              	       //����һ���������������������ĳ���
	int  cmd_len;                          //����һ����������������а������������ݵĳ���
	int  cmd_loca;                         //����һ����������������а������������ʼλ��
	int  i;                                //����һ������������forѭ��
	int  local,multiplier;
	unsigned char tempbuff[RBUFF_UNIT];	   //��ʱ������
	unsigned char *data;                   //redata������ʱ�򣬵�һ���ֽ�������������data����ָ��redata�ĵ�2���ֽڣ����������ݿ�ʼ�ĵط�
		
	re_len = redata[0] * 256 + redata[1];                 //��ȡ���յ������ܳ���		
	data = &redata[2];                                    //dataָ��redata�ĵ�2���ֽڣ����������ݿ�ʼ�� 
	pack_num = temp_len = totle_len = temp = 0;           //������������
	local = 1;
	multiplier = 1;
	do{
		pack_num++;                                       //��ʼѭ��ͳ�����͵ĸ�����ÿ��ѭ�����͵ĸ���+1	
		do{
			temp = data[totle_len + local];   
			temp_len += (temp & 127) * multiplier;
			multiplier *= 128;
			local++;
		}while ((temp & 128) != 0);
		totle_len += (temp_len + local);                  //�ۼ�ͳ�Ƶ��ܵ����͵����ݳ���
		re_len -= (temp_len + local) ;                    //���յ������ܳ��� ��ȥ ����ͳ�Ƶ����͵��ܳ���      
		local = 1;
		multiplier = 1;
		temp_len = 0;
	}while(re_len!=0);                                    //������յ������ܳ��ȵ���0�ˣ�˵��ͳ�������
	printf("���ν�����%d����������\r\n", pack_num);		
	temp_len = totle_len = 0;                		      //������������
	local = 1;
	multiplier = 1;
	for(i = 0; i < pack_num; i++)						  //�Ѿ�ͳ�Ƶ��˽��յ����͸�������ʼforѭ����ȡ��ÿ�����͵����� 
	{                                		
		do{
			temp = data[totle_len + local];   
			temp_len += (temp & 127) * multiplier;
			multiplier *= 128;
			local++;
		} while ((temp & 128) != 0);				
		topic_len = data[local + totle_len]*256 + data[local + 1 + totle_len] + 2; //���㱾����������������ռ�õ�������
		cmd_len = temp_len - topic_len;                              			   //���㱾��������������������ռ�õ�������
		cmd_loca = totle_len + local +  topic_len;                  			   //���㱾�������������������ݿ�ʼ��λ��
		memcpy(tempbuff, &data[cmd_loca], cmd_len);                   			   //�������ݿ�������		                 
		CMDBuf_Deal(tempbuff, cmd_len);                             			   //�������������
		totle_len += (temp_len + local);                              			   //�ۼ��Ѿ�ͳ�Ƶ����͵����ݳ���
		local = 1;
		multiplier = 1;
		temp_len = 0;
	}	
}

/*----------------------------------------------------------*/
/*�������������ͻ�����                                    */
/*��  ����data������                                        */
/*��  ����size�����ݳ���								    */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void TxDataBuf_Deal(unsigned char *data, int size)
{
	memcpy(&MQTT_TxDataInPtr[2], data, size);     //�������ݵ����ͻ�����	
	MQTT_TxDataInPtr[0] = size/256;               //��¼���ݳ���
	MQTT_TxDataInPtr[1] = size%256;               //��¼���ݳ���
	MQTT_TxDataInPtr += TBUFF_UNIT;               //ָ������
	if(MQTT_TxDataInPtr == MQTT_TxDataEndPtr)     //���ָ�뵽������β����
		MQTT_TxDataInPtr = MQTT_TxDataBuf[0];     //ָ���λ����������ͷ
}

/*----------------------------------------------------------*/
/*�������������������									*/
/*��  ����data������                                        */
/*��  ����size�����ݳ���                                    */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void CMDBuf_Deal(unsigned char *data, int size)
{
	memcpy(&MQTT_CMDInPtr[2], data,size);         //�������ݵ��������
	MQTT_CMDInPtr[0] = size/256;              	  //��¼���ݳ���
	MQTT_CMDInPtr[1] = size%256;                  //��¼���ݳ���
	MQTT_CMDInPtr[size+2] = '\0';                 //�����ַ���������
	MQTT_CMDInPtr += CBUFF_UNIT;               	  //ָ������
	if(MQTT_CMDInPtr == MQTT_CMDEndPtr)           //���ָ�뵽������β����
		MQTT_CMDInPtr = MQTT_CMDBuf[0];        	  //ָ���λ����������ͷ
}

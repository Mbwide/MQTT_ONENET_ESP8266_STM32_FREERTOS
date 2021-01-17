
/*-------------------------------------------------*/
/*                                                 */
/*          		  ����1                    	   */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32f10x.h"  
#include "usart1.h"     

/*-------------------------------------------------*/
/*��������printf�ض���      				           */
/*��  ����                                         */
/*����ֵ��                                         */
/*-------------------------------------------------*/  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40) == 0);//ѭ������,ֱ���������   
    USART1->DR = (u8)ch;      
	return ch;
}
#endif 


/*-------------------------------------------------*/
/*����������ʼ������1���͹���                       */
/*��  ����bound��������                            */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void usart1_init(unsigned int bound)
{  	 	
    GPIO_InitTypeDef GPIO_InitStructure;     //����һ������GPIO���ܵı���
	USART_InitTypeDef USART_InitStructure;   //����һ�����ô��ڹ��ܵı���
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); //ʹ�ܴ���1ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  //ʹ��GPIOAʱ��
	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;              //׼������PA9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;      //IO����50M
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	       //����������������ڴ���1�ķ���
    GPIO_Init(GPIOA, &GPIO_InitStructure);                 //����PA9
   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;             //׼������PA10 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //�������룬���ڴ���1�Ľ���
    GPIO_Init(GPIOA, &GPIO_InitStructure);                 //����PA10
	
	USART_InitStructure.USART_BaudRate = bound;                                    //����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;                    //8������λ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;                         //1��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;                            //����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
                                                                                   //�����ʹ�ܽ���ģʽ
	USART_InitStructure.USART_Mode = USART_Mode_Tx ;	                           //ֻ��ģʽ        
    USART_Init(USART1, &USART_InitStructure);                                      //���ô���1	
	
	USART_Cmd(USART1, ENABLE);                            						   //ʹ�ܴ���1
}

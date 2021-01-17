 
/*-------------------------------------------------*/
/*         	                                       */
/*           		 LED����           	 		   */
/*                                                 */
/*-------------------------------------------------*/

// Ӳ�����ӣ�
// PD2  LED1
// PA8  LED2
#include "stm32f10x.h"  //������Ҫ��ͷ�ļ�
#include "led.h"        //������Ҫ��ͷ�ļ�

/*-------------------------------------------------*/
/*����������ʼ��LED1��LED2����                     */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void led_init(void)
{    	 
	GPIO_InitTypeDef GPIO_InitStructure;                     
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);    

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;     
	GPIO_Init(GPIOD, &GPIO_InitStructure); 
	GPIO_SetBits(GPIOD, GPIO_Pin_2); 				//PD2 ����� 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;     
	GPIO_Init(GPIOA, &GPIO_InitStructure); 
	GPIO_SetBits(GPIOA, GPIO_Pin_8); 				//PA8 ����� 
}

/*-------------------------------------------------*/
/*��������LED1����                                  */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void led1_on(void)
{			
	GPIO_ResetBits(GPIOD, GPIO_Pin_2); 			    //PD2 �����
} 

/*-------------------------------------------------*/
/*��������LED1�ر�                                  */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void led1_off(void)
{		
	GPIO_SetBits(GPIOD, GPIO_Pin_2); 				//PD2 �����
}

/*-------------------------------------------------*/
/*��������LED2����                                  */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void led2_on(void)
{			
	GPIO_ResetBits(GPIOA, GPIO_Pin_8); 			    //PA8 �����
} 
/*-------------------------------------------------*/
/*��������LED2�ر�                                  */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void led2_off(void)
{		
	GPIO_SetBits(GPIOA, GPIO_Pin_8); 				//PA8 �����
}


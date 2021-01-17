
/*-------------------------------------------------*/
/*                                                 */
/*          		 ��ʱ��4                       */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32f10x.h"  //������Ҫ��ͷ�ļ�

char timer4_flag = 0;   //1:��ʱ��4�жϣ������ݵ�

/*-------------------------------------------------*/
/*����������ʱ��4��ʼ��                            */
/*��  ����arr���Զ���װֵ   0~65535                */
/*��  ����psc��ʱ��Ԥ��Ƶ�� 0~65535                */
/*����ֵ����                                       */
/*˵  ������ʱʱ�䣺arr*psc*1000/72000000  ��λms  */
/*-------------------------------------------------*/
void tim4_init(unsigned short int arr, unsigned short int psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;              //����һ�����ö�ʱ���ı���
	NVIC_InitTypeDef NVIC_InitStructure;                            //����һ�������жϵı���
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);            //ʹ��TIM4ʱ��	
    TIM_TimeBaseInitStructure.TIM_Period = arr; 	                //�����Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc;                  //���ö�ʱ��Ԥ��Ƶ��
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;     //1��Ƶ
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStructure);             //����TIM4
	
	TIM_ClearITPendingBit(TIM4, TIM_IT_Update);                     //�������жϱ�־λ
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);                      //ʹ��TIM4����ж�    
	TIM_Cmd(TIM4, DISABLE);                                         //�ȹر�TIM4                          
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;                 //����TIM4�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;       //��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;              //�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                 //�ж�ͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);                                 //�����ж�
}



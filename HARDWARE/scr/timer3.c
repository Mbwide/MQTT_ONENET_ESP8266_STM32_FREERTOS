
/*-------------------------------------------------*/
/*                                                 */
/*          		 ��ʱ��3                       */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32f10x.h"  //������Ҫ��ͷ�ļ�

/*-------------------------------------------------*/
/*����������ʱ��3ʹ��30s��ʱ                       */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void TIM3_ENABLE_30S(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;             //����һ�����ö�ʱ���ı���
	NVIC_InitTypeDef NVIC_InitStructure;                           //����һ�������жϵı���
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);           //ʹ��TIM3ʱ��	
	TIM_DeInit(TIM3);                                              //��ʱ��3�Ĵ����ָ�Ĭ��ֵ	
	TIM_TimeBaseInitStructure.TIM_Period = 60000-1; 	           //�����Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler = 36000-1;             //���ö�ʱ��Ԥ��Ƶ��
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;//���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;    //1��Ƶ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);            //����TIM3
	
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);                    //�������жϱ�־λ
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);                     //ʹ��TIM3����ж�    
	TIM_Cmd(TIM3, ENABLE);                                         //��TIM3                          
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;                //����TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;      //��ռ���ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;             //�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                //�ж�ͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);                                //�����ж�
}
/*-------------------------------------------------*/
/*����������ʱ��3ʹ��2s��ʱ                        */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void TIM3_ENABLE_2S(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;             //����һ�����ö�ʱ���ı���
	NVIC_InitTypeDef NVIC_InitStructure;                           //����һ�������жϵı���
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);           //ʹ��TIM3ʱ��
	TIM_DeInit(TIM3);                                              //��ʱ��3�Ĵ����ָ�Ĭ��ֵ	
	TIM_TimeBaseInitStructure.TIM_Period = 20000-1; 	           //�����Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler = 7200-1;              //���ö�ʱ��Ԥ��Ƶ��
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;//���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;    //1��Ƶ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);            //����TIM3
	
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);                    //�������жϱ�־λ
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);                     //ʹ��TIM3����ж�    
	TIM_Cmd(TIM3, ENABLE);                                         //��TIM3                          
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;                //����TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;      //��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;             //�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                //�ж�ͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);                                //�����ж�
}


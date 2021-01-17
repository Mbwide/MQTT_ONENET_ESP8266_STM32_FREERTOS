
/*-----------------------------------------------------*/
/*                                                     */
/*         		  	  ��ʱ����                         */
/*                                                     */
/*-----------------------------------------------------*/


/*
 *	delay_us()��us����ʱ������delay_ms��delay_xms()����ms������ʱ����,delay_us()��
 *	delay_xms()���ᵼ�������л���delay_ms()��ʵ���Ƕ� FreeRTOS�е���ʱ����vTaskDelay()��
 *	�򵥷�װ��������ʹ��delay_ms()��ʱ��ͻᵼ�������л��� 
 */

#include "delay.h"
#include "FreeRTOS.h"					//FreeRTOSʹ��		  
#include "task.h"
static u8  fac_us = 0;					//us��ʱ������			   
static u16 how_ms_per_rate = 0;			//ms��ʱ������

/*-------------------------------------------------*/
/*����������ʱ��ʼ��                    	       */
/*��  ������                                       */
/*����ֵ����                                       */
/*-------------------------------------------------*/
void delay_init()
{
	u32 reload;
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);//ѡ���ⲿʱ��  HCLK
	fac_us = SystemCoreClock/1000000;				//�����Ƿ�ʹ��OS,fac_us����Ҫʹ�� 
	reload = SystemCoreClock/configTICK_RATE_HZ;	//����configTICK_RATE_HZ�趨���ʱ��
													//reloadΪ24λ�Ĵ���,���ֵ:16777216,��72M��,Լ��0.233s����	
	how_ms_per_rate = 1000/configTICK_RATE_HZ;		//����OS������ʱ�����ٵ�λ	   

	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;   	//����SYSTICK�ж�
	SysTick->LOAD = reload; 						//ÿ1/configTICK_RATE_HZ���ж�һ��	
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;   	//����SYSTICK 					
}	 

/*-------------------------------------------------*/
/*��������us��ʱ                    			    */
/*��  ��  us                                       */
/*����ֵ����                                        */
/*-------------------------------------------------*/
void delay_us(u32 nus)
{		
	u32 ticks;
	u32 told,tnow,tcnt = 0;
	u32 reload = SysTick->LOAD;					//LOAD��ֵ	    	 
	ticks = nus * fac_us; 						//��Ҫ�Ľ����� 
	told = SysTick->VAL;        				//�ս���ʱ�ļ�����ֵ
	while(1)
	{
		tnow = SysTick->VAL;	
		if(tnow != told)
		{	    
			if(tnow < told)tcnt += told - tnow;	//����ע��һ��SYSTICK��һ���ݼ��ļ������Ϳ�����.
			else tcnt += reload - tnow + told;	    
			told = tnow;
			if(tcnt >= ticks)break;				//ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳�.
		}  
	};										    
} 

/*-------------------------------------------------*/
/*��������ms��ʱ (FreeRTOS vTaskDelay()��װ)        */
/*��  ��  ms                                       */
/*����ֵ����                                        */
/*-------------------------------------------------*/
void delay_ms(u32 nms)
{	 		  	  
	if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
	{
		if (nms >= how_ms_per_rate) 
		{
			vTaskDelay(nms/how_ms_per_rate);
		}
		nms %= how_ms_per_rate;				//OS�Ѿ��޷��ṩ��ôС����ʱ��,������ͨ��ʽ��ʱ  		
	}
	delay_us((u32)(nms * 1000));		    //��ͨ��ʽ��ʱ
} 


//��ʱnms,���������������
//nms:Ҫ��ʱ��ms��
void delay_xms(u32 nms)
{
	u32 i;
	for(i = 0; i < nms; i++) delay_us(1000);
}








































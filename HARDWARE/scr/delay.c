
/*-----------------------------------------------------*/
/*                                                     */
/*         		  	  延时函数                         */
/*                                                     */
/*-----------------------------------------------------*/


/*
 *	delay_us()是us级延时函数，delay_ms和delay_xms()都是ms级的延时函数,delay_us()和
 *	delay_xms()不会导致任务切换。delay_ms()其实就是对 FreeRTOS中的延时函数vTaskDelay()的
 *	简单封装，所以在使用delay_ms()的时候就会导致任务切换。 
 */

#include "delay.h"
#include "FreeRTOS.h"					//FreeRTOS使用		  
#include "task.h"
static u8  fac_us = 0;					//us延时倍乘数			   
static u16 how_ms_per_rate = 0;			//ms延时倍乘数

/*-------------------------------------------------*/
/*函数名：延时初始化                    	       */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void delay_init()
{
	u32 reload;
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);//选择外部时钟  HCLK
	fac_us = SystemCoreClock/1000000;				//不论是否使用OS,fac_us都需要使用 
	reload = SystemCoreClock/configTICK_RATE_HZ;	//根据configTICK_RATE_HZ设定溢出时间
													//reload为24位寄存器,最大值:16777216,在72M下,约合0.233s左右	
	how_ms_per_rate = 1000/configTICK_RATE_HZ;		//代表OS可以延时的最少单位	   

	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;   	//开启SYSTICK中断
	SysTick->LOAD = reload; 						//每1/configTICK_RATE_HZ秒中断一次	
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;   	//开启SYSTICK 					
}	 

/*-------------------------------------------------*/
/*函数名：us延时                    			    */
/*参  数  us                                       */
/*返回值：无                                        */
/*-------------------------------------------------*/
void delay_us(u32 nus)
{		
	u32 ticks;
	u32 told,tnow,tcnt = 0;
	u32 reload = SysTick->LOAD;					//LOAD的值	    	 
	ticks = nus * fac_us; 						//需要的节拍数 
	told = SysTick->VAL;        				//刚进入时的计数器值
	while(1)
	{
		tnow = SysTick->VAL;	
		if(tnow != told)
		{	    
			if(tnow < told)tcnt += told - tnow;	//这里注意一下SYSTICK是一个递减的计数器就可以了.
			else tcnt += reload - tnow + told;	    
			told = tnow;
			if(tcnt >= ticks)break;				//时间超过/等于要延迟的时间,则退出.
		}  
	};										    
} 

/*-------------------------------------------------*/
/*函数名：ms延时 (FreeRTOS vTaskDelay()封装)        */
/*参  数  ms                                       */
/*返回值：无                                        */
/*-------------------------------------------------*/
void delay_ms(u32 nms)
{	 		  	  
	if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
	{
		if (nms >= how_ms_per_rate) 
		{
			vTaskDelay(nms/how_ms_per_rate);
		}
		nms %= how_ms_per_rate;				//OS已经无法提供这么小的延时了,采用普通方式延时  		
	}
	delay_us((u32)(nms * 1000));		    //普通方式延时
} 


//延时nms,不会引起任务调度
//nms:要延时的ms数
void delay_xms(u32 nms)
{
	u32 i;
	for(i = 0; i < nms; i++) delay_us(1000);
}








































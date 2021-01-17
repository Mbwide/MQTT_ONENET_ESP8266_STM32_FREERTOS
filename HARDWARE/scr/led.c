 
/*-------------------------------------------------*/
/*         	                                       */
/*           		 LED控制           	 		   */
/*                                                 */
/*-------------------------------------------------*/

// 硬件连接：
// PD2  LED1
// PA8  LED2
#include "stm32f10x.h"  //包含需要的头文件
#include "led.h"        //包含需要的头文件

/*-------------------------------------------------*/
/*函数名：初始化LED1和LED2函数                     */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void led_init(void)
{    	 
	GPIO_InitTypeDef GPIO_InitStructure;                     
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);    

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;     
	GPIO_Init(GPIOD, &GPIO_InitStructure); 
	GPIO_SetBits(GPIOD, GPIO_Pin_2); 				//PD2 输出高 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;     
	GPIO_Init(GPIOA, &GPIO_InitStructure); 
	GPIO_SetBits(GPIOA, GPIO_Pin_8); 				//PA8 输出高 
}

/*-------------------------------------------------*/
/*函数名：LED1开启                                  */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void led1_on(void)
{			
	GPIO_ResetBits(GPIOD, GPIO_Pin_2); 			    //PD2 输出低
} 

/*-------------------------------------------------*/
/*函数名：LED1关闭                                  */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void led1_off(void)
{		
	GPIO_SetBits(GPIOD, GPIO_Pin_2); 				//PD2 输出高
}

/*-------------------------------------------------*/
/*函数名：LED2开启                                  */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void led2_on(void)
{			
	GPIO_ResetBits(GPIOA, GPIO_Pin_8); 			    //PA8 输出低
} 
/*-------------------------------------------------*/
/*函数名：LED2关闭                                  */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void led2_off(void)
{		
	GPIO_SetBits(GPIOA, GPIO_Pin_8); 				//PA8 输出高
}


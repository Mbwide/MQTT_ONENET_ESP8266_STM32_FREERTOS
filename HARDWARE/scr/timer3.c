
/*-------------------------------------------------*/
/*                                                 */
/*          		 定时器3                       */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32f10x.h"  //包含需要的头文件

/*-------------------------------------------------*/
/*函数名：定时器3使能30s定时                       */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void TIM3_ENABLE_30S(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;             //定义一个设置定时器的变量
	NVIC_InitTypeDef NVIC_InitStructure;                           //定义一个设置中断的变量
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);           //使能TIM3时钟	
	TIM_DeInit(TIM3);                                              //定时器3寄存器恢复默认值	
	TIM_TimeBaseInitStructure.TIM_Period = 60000-1; 	           //设置自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler = 36000-1;             //设置定时器预分频数
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;//向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;    //1分频
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);            //设置TIM3
	
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);                    //清除溢出中断标志位
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);                     //使能TIM3溢出中断    
	TIM_Cmd(TIM3, ENABLE);                                         //开TIM3                          
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;                //设置TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;      //抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;             //子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                //中断通道使能
	NVIC_Init(&NVIC_InitStructure);                                //设置中断
}
/*-------------------------------------------------*/
/*函数名：定时器3使能2s定时                        */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void TIM3_ENABLE_2S(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;             //定义一个设置定时器的变量
	NVIC_InitTypeDef NVIC_InitStructure;                           //定义一个设置中断的变量
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);           //使能TIM3时钟
	TIM_DeInit(TIM3);                                              //定时器3寄存器恢复默认值	
	TIM_TimeBaseInitStructure.TIM_Period = 20000-1; 	           //设置自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler = 7200-1;              //设置定时器预分频数
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;//向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;    //1分频
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);            //设置TIM3
	
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);                    //清除溢出中断标志位
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);                     //使能TIM3溢出中断    
	TIM_Cmd(TIM3, ENABLE);                                         //开TIM3                          
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;                //设置TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;      //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;             //子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                //中断通道使能
	NVIC_Init(&NVIC_InitStructure);                                //设置中断
}



/*-------------------------------------------------*/
/*                                                 */
/*          		 定时器4                       */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32f10x.h"  //包含需要的头文件

char timer4_flag = 0;   //1:定时器4中断，有数据到

/*-------------------------------------------------*/
/*函数名：定时器4初始化                            */
/*参  数：arr：自动重装值   0~65535                */
/*参  数：psc：时钟预分频数 0~65535                */
/*返回值：无                                       */
/*说  明：定时时间：arr*psc*1000/72000000  单位ms  */
/*-------------------------------------------------*/
void tim4_init(unsigned short int arr, unsigned short int psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;              //定义一个设置定时器的变量
	NVIC_InitTypeDef NVIC_InitStructure;                            //定义一个设置中断的变量
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);            //使能TIM4时钟	
    TIM_TimeBaseInitStructure.TIM_Period = arr; 	                //设置自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc;                  //设置定时器预分频数
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;     //1分频
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStructure);             //设置TIM4
	
	TIM_ClearITPendingBit(TIM4, TIM_IT_Update);                     //清除溢出中断标志位
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);                      //使能TIM4溢出中断    
	TIM_Cmd(TIM4, DISABLE);                                         //先关闭TIM4                          
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;                 //设置TIM4中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;       //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;              //子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                 //中断通道使能
	NVIC_Init(&NVIC_InitStructure);                                 //设置中断
}



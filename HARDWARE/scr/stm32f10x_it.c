/*-------------------------------------------------*/
/*                                                 */
/*            	   中断服务函数          	   	   */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32f10x.h"     //包含需要的头文件
#include "FreeRTOS.h"	   //FreeRTOS使用		  
#include "task.h"
#include "event_groups.h"
#include "stm32f10x_it.h"  //包含需要的头文件
#include "usart1.h"        //包含需要的头文件
#include "usart2.h"        //包含需要的头文件
#include "timer3.h"        //包含需要的头文件
#include "mqtt.h"          //包含需要的头文件
#include "dht11.h"         //包含需要的头文件                  

extern void xPortSysTickHandler(void);
extern TaskHandle_t WIFI_Task_Handler;
extern  EventGroupHandle_t Event_Handle;
extern const int PING_MODE;

/*---------------------------------------------------------------*/
/*函数名：void USART2_IRQHandler(void) 			      			 */
/*功  能：串口2中断处理函数										 */
/*		  1.与esp8266通信，已经连接服务器控制定时器4，未连接服务器 */
/*			不控制定时器4（通过事件标志组的位0 WIFI_CONECT判断）	 */
/*参  数：无                                       				 */
/*返回值：无                                     				 */
/*---------------------------------------------------------------*/
void USART2_IRQHandler(void)   
{   
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)         //如果USART_IT_RXNE标志置位，表示有数据到了，进入if分支	   
	{   
		if ((xEventGroupGetBitsFromISR(Event_Handle) & 0x01) == 0)//获取事件标志组数据，等于0说明未连接服务器，不开启定时器4（MQTT接收数据处理）定时器
		{
			if(USART2->DR)                                        //处于指令配置状态时，非零值才保存到缓冲区	
			{                                     			 
				Usart2_RxBuff[Usart2_RxCounter] = USART2->DR;	  //保存到缓冲区	
				Usart2_RxCounter++; 						      //每接收1个字节的数据，Usart2_RxCounter加1，表示接收的数据总量+1 
			}		
		}
		else
		{
			Usart2_RxBuff[Usart2_RxCounter] = USART2->DR;//把接收到的数据保存到Usart2_RxBuff中
				
			if(Usart2_RxCounter == 0)				     //如果Usart2_RxCounter等于0，表示是接收的第1个数据，进入if分支	
			{    								
				TIM_Cmd(TIM4, ENABLE); 					 //使能定时器4
			}
			else										 //else分支，表示果Usart2_RxCounter不等于0，不是接收的第一个数据
			{                        									    
				TIM_SetCounter(TIM4, 0);  				 //置位定时器4
			}	
			Usart2_RxCounter++;         				 //每接收1个字节的数据，Usart2_RxCounter加1，表示接收的数据总量+1 				
		}	
	} 
}

/*---------------------------------------------------------------*/
/*函数名：void TIM4_IRQHandler(void)				      			 */
/*功  能：定时器4中断处理函数									 */
/*		  1.处理串口2接收到的MQTT数据，将串口接收缓冲复制到MQTT接收*/
/*			缓冲        										 */
/*参  数：无                                       				 */
/*返回值：无                                     				 */
/*---------------------------------------------------------------*/
void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)//如果TIM_IT_Update置位，表示TIM4溢出中断，进入if	
	{                	
		memcpy(&MQTT_RxDataInPtr[2], Usart2_RxBuff, Usart2_RxCounter);  //拷贝数据到接收缓冲区
		MQTT_RxDataInPtr[0] = Usart2_RxCounter/256;                  	//记录数据长度高字节
		MQTT_RxDataInPtr[1] = Usart2_RxCounter%256;					 	//记录数据长度低字节
		MQTT_RxDataInPtr += RBUFF_UNIT;                                	//指针下移
		if(MQTT_RxDataInPtr == MQTT_RxDataEndPtr)                     	//如果指针到缓冲区尾部了
			MQTT_RxDataInPtr = MQTT_RxDataBuf[0];                    	//指针归位到缓冲区开头
		Usart2_RxCounter = 0;                                        	//串口2接收数据量变量清零
		TIM_SetCounter(TIM3, 0);                                     	//清零定时器3计数器，重新计时ping包发送时间
		TIM_Cmd(TIM4, DISABLE);                        				 	//关闭TIM4定时器
		TIM_SetCounter(TIM4, 0);                        			 	//清零定时器4计数器
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);     			 	//清除TIM4溢出中断标志 	
	}
}

/*---------------------------------------------------------------*/
/*函数名：void TIM3_IRQHandler(void)				      			 */
/*功  能：定时器3中断处理函数									 */
/*		  1.控制ping心跳包的发送									 */
/*参  数：无                                       				 */
/*返回值：无                                     				 */
/*其  他：多次快速发送（2s，5次）没有反应，wifi任务由挂起态->就绪态*/
/*---------------------------------------------------------------*/
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)//如果TIM_IT_Update置位，表示TIM3溢出中断，进入if	
	{  
		switch(pingFlag) 					//判断pingFlag的状态
		{                               
			case 0:							//如果pingFlag等于0，表示正常状态，发送Ping报文  
					MQTT_PingREQ(); 		//添加Ping报文到发送缓冲区  
					break;
			case 1:							//如果pingFlag等于1，说明上一次发送到的ping报文，没有收到服务器回复，所以1没有被清除为0，可能是连接异常，我们要启动快速ping模式
					TIM3_ENABLE_2S(); 	    //我们将定时器6设置为2s定时,快速发送Ping报文
					xEventGroupClearBitsFromISR(Event_Handle, PING_MODE);//关闭发送PING包的定时器3，设置事件标志位
					MQTT_PingREQ();			//添加Ping报文到发送缓冲区  
					break;
			case 2:							//如果pingFlag等于2，说明还没有收到服务器回复
			case 3:				            //如果pingFlag等于3，说明还没有收到服务器回复
			case 4:				            //如果pingFlag等于4，说明还没有收到服务器回复	
					MQTT_PingREQ();  		//添加Ping报文到发送缓冲区 
					break;
			case 5:							//如果pingFlag等于5，说明我们发送了多次ping，均无回复，应该是连接有问题，我们重启连接
					xTaskResumeFromISR(WIFI_Task_Handler);        //连接状态置0，表示断开，没连上服务器
					TIM_Cmd(TIM3, DISABLE); //关TIM3 				
					break;			
		}
		pingFlag++;           		   		//pingFlag自增1，表示又发送了一次ping，期待服务器的回复
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update); //清除TIM3溢出中断标志 	
	}
}



/*-------------------------------------------------*/
/*函数名：不可屏蔽中断处理函数                     */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void NMI_Handler(void)
{

}

/*-------------------------------------------------*/
/*函数名：硬件出错后进入的中断处理函数             */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void HardFault_Handler(void)
{

}

/*-------------------------------------------------*/
/*函数名：内存管理中断处理函数                     */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void MemManage_Handler(void)
{

}

/*-------------------------------------------------*/
/*函数名：预取指失败，存储器访问失败中断处理函数   */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void BusFault_Handler(void)
{

}

/*-------------------------------------------------*/
/*函数名：未定义的指令或非法状态处理函数           */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void UsageFault_Handler(void)
{

}

/*-------------------------------------------------*/
/*函数名：软中断，SWI 指令调用的处理函数           */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
//void SVC_Handler(void)
//{
//	
//}

/*-------------------------------------------------*/
/*函数名：调试监控器处理函数                       */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void DebugMon_Handler(void)
{
	
}

/*-------------------------------------------------*/
/*函数名：可挂起的系统服务处理函数                 */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
//void PendSV_Handler(void)
//{
//}

/*-------------------------------------------------*/
/*函数名：SysTic系统嘀嗒定时器处理函数             */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void SysTick_Handler(void)
{
	if(xTaskGetSchedulerState()!=taskSCHEDULER_NOT_STARTED)//系统已经运行
    {
        xPortSysTickHandler();	
    }
}

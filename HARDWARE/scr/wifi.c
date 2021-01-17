
/*-------------------------------------------------*/
/*                                                 */
/*          	 WIFI（ESP8266）源文件             */
/*                                                 */
/*-------------------------------------------------*/

// 硬件连接：
// PA2 RX
// PA3 TX
// PA4 复位

#include "stm32f10x.h"  //包含需要的头文件
#include "wifi.h"	    //包含需要的头文件
#include "delay.h"	    //包含需要的头文件
#include "usart1.h"	    //包含需要的头文件

	
/*-------------------------------------------------*/
/*函数名：初始化WiFi的复位IO                       */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void wifi_reset_io_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;                    //定义一个设置IO端口参数的结构体
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA , ENABLE); //使能PA端口时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;               //准备设置PA4
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       //速率50Mhz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   	    //推免输出方式
	GPIO_Init(GPIOA, &GPIO_InitStructure);            	    //设置PA4
	RESET_IO(1);                                            //复位IO拉高电平
}
/*-------------------------------------------------*/
/*函数名：WiFi发送设置指令                         */
/*参  数：cmd：指令                                */
/*参  数：timeout：超时时间（100ms的倍数）         */
/*返回值：0：正确   其他：错误                     */
/*-------------------------------------------------*/
char WiFi_SendCmd(char *cmd, int timeout)
{
	WiFi_RxCounter = 0;                           	//WiFi接收数据量变量清零                        
	memset(WiFi_RX_BUF, 0, WiFi_RXBUFF_SIZE);     	//清空WiFi接收缓冲区 
	WiFi_printf("%s\r\n", cmd);                  	//发送指令
	while(timeout--)								//等待超时时间到0
	{                           			
		delay_ms(100);                              //延时100ms
		if(strstr(WiFi_RX_BUF, "OK"))              	//如果接收到OK表示指令成功
			break;       							//主动跳出while循环
		printf("%d ", timeout);                 	//串口输出现在的超时时间
	}			
	printf("\r\n");                          			   
	if(timeout <= 0)return 1;                       //如果timeout<=0，说明超时时间到了，也没能收到OK，返回1
	else return 0;		         					//反之，表示正确，说明收到OK，通过break主动跳出while
}
/*-------------------------------------------------*/
/*函数名：WiFi复位                                 */
/*参  数：timeout：超时时间（100ms的倍数）         */
/*返回值：0：正确   其他：错误                     */
/*-------------------------------------------------*/
char WiFi_Reset(int timeout)
{
	RESET_IO(0);                                    //复位IO拉低电平
	delay_ms(500);                                  //延时500ms
	RESET_IO(1);                                   	//复位IO拉高电平	
	while(timeout--)								//等待超时时间到0 
	{                              		  
		delay_ms(100);                              //延时100ms
		if(strstr(WiFi_RX_BUF, "ready"))            //如果接收到ready表示复位成功
			break;       						    //主动跳出while循环
		printf("%d ", timeout);                     //串口输出现在的超时时间
	}
	printf("\r\n");                              
	if(timeout <= 0)return 1;                       //如果timeout<=0，说明超时时间到了，也没能收到ready，返回1
	else return 0;		         				   	//反之，表示正确，说明收到ready，通过break主动跳出while
}
/*-------------------------------------------------*/
/*函数名：WiFi加入路由器指令                       */
/*参  数：timeout：超时时间（1s的倍数）            */
/*返回值：0：正确   其他：错误                     */
/*-------------------------------------------------*/
char WiFi_JoinAP(int timeout)
{		
	WiFi_RxCounter = 0;                                    //WiFi接收数据量变量清零                        
	memset(WiFi_RX_BUF, 0, WiFi_RXBUFF_SIZE);              //清空WiFi接收缓冲区 
	WiFi_printf("AT+CWJAP=\"%s\",\"%s\"\r\n", SSID, PASS); //发送指令	
	while(timeout--)									   //等待超时时间到0
	{                                   
		delay_ms(1000);                             	   //延时1s
		if(strstr(WiFi_RX_BUF, "WIFI GOT IP\r\n\r\nOK"))   //如果接收到WIFI GOT IP表示成功
			break;       						           //主动跳出while循环
		printf("%d ", timeout);                            //串口输出现在的超时时间
	}
	printf("\r\n");                             	       //串口输出信息
	if(timeout <= 0)return 1;                              //如果timeout<=0，说明超时时间到了，也没能收到WIFI GOT IP，返回1
	return 0;                                              //正确，返回0
}
/*-------------------------------------------------*/
/*函数名：连接TCP服务器，并进入透传模式            */
/*参  数：timeout： 超时时间（100ms的倍数）        */
/*返回值：0：正确  其他：错误                      */
/*-------------------------------------------------*/
char WiFi_Connect_Server(int timeout)
{	
	WiFi_RxCounter = 0;                               //WiFi接收数据量变量清零                        
	memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);           //清空WiFi接收缓冲区   
	WiFi_printf("AT+CIPSTART=\"TCP\",\"%s\",%d\r\n", ServerIP, ServerPort);//发送连接服务器指令
	while(timeout--)								  //等待超时与否
	{                           
		delay_ms(100);                             	  //延时100ms	
		if(strstr(WiFi_RX_BUF, "CONNECT"))            //如果接受到CONNECT表示连接成功
			break;                                    //跳出while循环
		if(strstr(WiFi_RX_BUF, "CLOSED"))             //如果接受到CLOSED表示服务器未开启
			return 1;                                 //服务器未开启返回1
		if(strstr(WiFi_RX_BUF, "ALREADY CONNECTED"))  //如果接受到ALREADY CONNECTED已经建立连接
			return 2;                                 //已经建立连接返回2
		printf("%d ", timeout);                       //串口输出现在的超时时间  
	}
	printf("\r\n");                                   
	if(timeout <= 0)return 3;                         //超时错误，返回3
	else                                              //连接成功，准备进入透传
	{
		printf("连接服务器成功，准备进入透传\r\n");   
		WiFi_RxCounter = 0;                           //WiFi接收数据量变量清零                        
		memset(WiFi_RX_BUF, 0, WiFi_RXBUFF_SIZE);     //清空WiFi接收缓冲区     
		WiFi_printf("AT+CIPSEND\r\n");                //发送进入透传指令
		while(timeout--)							  //等待超时与否
		{                            
			delay_ms(100);                            //延时100ms	
			if(strstr(WiFi_RX_BUF, "\r\nOK\r\n\r\n>"))//如果成立表示进入透传成功
				break;                          	  //跳出while循环
			printf("%d ", timeout);                   //串口输出现在的超时时间  
		}
		if(timeout <= 0)return 4;                     //透传超时错误，返回4	
	}
	return 0;	                                      //成功返回0	
}
/*-------------------------------------------------*/
/*函数名：WiFi_Smartconfig                         */
/*参  数：timeout：超时时间（1s的倍数）            */
/*返回值：0：正确   其他：错误                     */
/*-------------------------------------------------*/
char WiFi_Smartconfig(int timeout)
{
	
	WiFi_RxCounter = 0;                           		//WiFi接收数据量变量清零                        
	memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);     		//清空WiFi接收缓冲区     
	while(timeout--)									//等待超时时间到0
	{                           		
		delay_ms(1000);                         		//延时1s
		if(strstr(WiFi_RX_BUF, "connected"))    	 	//如果串口接受到connected表示成功
			break;                                  	//跳出while循环  
		printf("%d ", timeout);                 		//串口输出现在的超时时间  
	}	
	printf("\r\n");                          			
	if(timeout <= 0)return 1;                     		//超时错误，返回1
	return 0;                                   		//正确返回0
}
/*-------------------------------------------------*/
/*函数名：等待加入路由器                           */
/*参  数：timeout：超时时间（1s的倍数）            */
/*返回值：0：正确   其他：错误                     */
/*-------------------------------------------------*/
char WiFi_WaitAP(int timeout)
{		
	while(timeout--){                               //等待超时时间到0
		delay_ms(1000);                             //延时1s
		if(strstr(WiFi_RX_BUF, "WIFI GOT IP"))      //如果接收到WIFI GOT IP表示成功
			break;       						 
		printf("%d ", timeout);                     //串口输出现在的超时时间
	}
	printf("\r\n");                             	//串口输出信息
	if(timeout <= 0)return 1;                       //如果timeout<=0，说明超时时间到了，也没能收到WIFI GOT IP，返回1
	return 0;                                       //正确，返回0
}
/*-------------------------------------------------*/
/*函数名：WiFi连接服务器                           */
/*参  数：无                                       */
/*返回值：0：正确   其他：错误                     */
/*-------------------------------------------------*/
char WiFi_Connect_IoTServer(void)
{	
	printf("准备复位模块\r\n");                   
	if(WiFi_Reset(50))							  //复位，100ms超时单位，总计5s超时时间
	{                             
		printf("复位失败，准备重启\r\n");	      //返回非0值，进入if
		return 1;                                 //返回1
	}else printf("复位成功\r\n");                 
	
	printf("准备设置STA模式\r\n");                
	if(WiFi_SendCmd("AT+CWMODE=1",50))			  //设置STA模式，100ms超时单位，总计5s超时时间
	{             
		printf("设置STA模式失败，准备重启\r\n");  //返回非0值，进入if
		return 2;                                 //返回2
	}else printf("设置STA模式成功\r\n");          
	                            
	printf("准备取消自动连接\r\n");            	  
	if(WiFi_SendCmd("AT+CWAUTOCONN=0",50))		  //取消自动连接，100ms超时单位，总计5s超时时间
	{       
		printf("取消自动连接失败，准备重启\r\n"); //返回非0值，进入if
		return 3;                                 //返回3
	}else printf("取消自动连接成功\r\n");         
			
	printf("准备连接路由器\r\n");                 	
	if(WiFi_JoinAP(30))							  //连接路由器,1s超时单位，总计30s超时时间
	{                          
		printf("连接路由器失败，准备重启\r\n");   //返回非0值，进入if
		return 4;                                 //返回4	
	}else printf("连接路由器成功\r\n");       		

	printf("准备设置透传\r\n");                    
	if(WiFi_SendCmd("AT+CIPMODE=1",50)) 		  //设置透传，100ms超时单位，总计5s超时时间
	{           
		printf("设置透传失败，准备重启\r\n");     //返回非0值，进入if
		return 8;                                 //返回8
	}else printf("设置透传成功\r\n");              
	
	printf("准备关闭多路连接\r\n");               
	if(WiFi_SendCmd("AT+CIPMUX=0",50)) 		      //关闭多路连接，100ms超时单位，总计5s超时时间
	{            
		printf("关闭多路连接失败，准备重启\r\n"); //返回非0值，进入if
		return 9;                                 //返回9
	}else printf("关闭多路连接成功\r\n");         
	 
	printf("准备连接服务器\r\n");                 
	if(WiFi_Connect_Server(100))      			  //连接服务器，100ms超时单位，总计10s超时时间
	{            
		printf("连接服务器失败，准备重启\r\n");   //返回非0值，进入if
		return 10;                                //返回10
	}else printf("连接服务器成功\r\n");           
	return 0;                                     //正确返回0
}
	


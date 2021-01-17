
#ifndef __USART2_H
#define __USART2_H

#include "stdio.h"      
#include "stdarg.h"		 
#include "string.h"    

#define USART2_TXBUFF_SIZE   1024   		   //定义串口2 发送缓冲区大小 1024字节
#define USART2_RXBUFF_SIZE   1024              //定义串口2 接收缓冲区大小 1024字节


extern unsigned int Usart2_RxCounter;          //外部声明，其他文件可以调用该变量
extern char Usart2_RxBuff[USART2_RXBUFF_SIZE]; //外部声明，其他文件可以调用该变量

void usart2_init(unsigned int);       
void u2_printf(char*,...) ;          
void u2_TxData(unsigned char *data);

#endif



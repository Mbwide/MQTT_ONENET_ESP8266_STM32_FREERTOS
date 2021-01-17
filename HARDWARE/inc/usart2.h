
#ifndef __USART2_H
#define __USART2_H

#include "stdio.h"      
#include "stdarg.h"		 
#include "string.h"    

#define USART2_TXBUFF_SIZE   1024   		   //���崮��2 ���ͻ�������С 1024�ֽ�
#define USART2_RXBUFF_SIZE   1024              //���崮��2 ���ջ�������С 1024�ֽ�


extern unsigned int Usart2_RxCounter;          //�ⲿ�����������ļ����Ե��øñ���
extern char Usart2_RxBuff[USART2_RXBUFF_SIZE]; //�ⲿ�����������ļ����Ե��øñ���

void usart2_init(unsigned int);       
void u2_printf(char*,...) ;          
void u2_TxData(unsigned char *data);

#endif



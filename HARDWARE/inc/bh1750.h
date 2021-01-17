#ifndef  __BH1750_H__
#define  __BH1750_H__

#include  "sys.h"
#include  "delay.h"
#include  "usart1.h"
 
//IIC总线地址接口定义
#define IIC_SCL PCout(12)
#define IIC_SDA_OUT PCout(11)
#define IIC_SDA_IN PCin(11)
 
#define IIC_INPUT_MODE_SET()  {GPIOC->CRH&=0XFFFF0FFF;GPIOC->CRH|=8<<12;}
#define IIC_OUTPUT_MODE_SET() {GPIOC->CRH&=0XFFFF0FFF;GPIOC->CRH|=3<<12;}
 
#define  IIC_WRITE      0     // 写
#define  IIC_READ       1     // 读
#define	 SlaveAddress   0x46  //BH1750地址
extern u8 BUF[3];

//函数声明
void iic_by30_init(void);
void BH1750_Start(void);
void BH1750_Stop(void);
void BH1750_SendACK(char ack);
char BH1750_RecvACK(void);
void BH1750_SendByte(u8 data);
void Single_Write_BH1750(u8 REG_Address);
void Multiple_Read_BH1750(void);
int get_sunlight_value(void);
u8 BH1750_RecvByte(void);
 
#endif




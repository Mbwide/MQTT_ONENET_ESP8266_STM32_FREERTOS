
#ifndef __DHT11_H
#define __DHT11_H 
 
#define DHT11_IO_IN()  {GPIOA->CRL &= 0XF0FFFFFF;GPIOA->CRL |= 8<<4*6;}       //PA8控制DHT11，设置为输出
#define DHT11_IO_OUT() {GPIOA->CRL &= 0XF0FFFFFF;GPIOA->CRL |= 3<<4*6;}       //PA8控制DHT11，设置为输入
										   
#define DHT11_OUT(x)    GPIO_WriteBit(GPIOA, GPIO_Pin_6, (BitAction)x)  //PA8控制DHT11
#define DHT11_DQ_IN     GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6)        //PA8控制DHT11	
   
void DHT11_Rst(void);
char DHT11_Check(void);
char DHT11_Read_Bit(void);
char DHT11_Read_Byte(void);
char dht11_read_data(char *temp, char *humi);
char dht11_init(void);

#endif
















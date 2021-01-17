
/*-------------------------------------------------*/
/*                                                 */
/*            	    光照强度			   		   	   */
/*                                                 */
/*-------------------------------------------------*/

// 硬件连接：
// SCL-PC12
// SDA-PC11
// ARR-GND
// VCC-5V
// GND-GND
#include "bh1750.h"

u8  BUF[3];

/*-------------------------------------------------*/
/*函数名：BY30（光照传感器）初始化        		   */
/*参  数：                                         */
/*返回值：                                         */
/*-------------------------------------------------*/
void iic_by30_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOC, ENABLE );	
	   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	IIC_SCL = 1;
	IIC_SDA_OUT = 1;
}
 
/*-------------------------------------------------*/
/*函数名：起始信号                     			   */
/*参  数：                                         */
/*返回值：                                         */
/*-------------------------------------------------*/
void BH1750_Start()
{
	IIC_OUTPUT_MODE_SET(); //配置输出模式
	IIC_SDA_OUT = 1;       //拉高数据线
	IIC_SCL = 1;           //拉高时钟线
	delay_us(5);           //延时
	IIC_SDA_OUT = 0;       //产生下降沿
	delay_us(5);           //延时
	IIC_SCL = 0;           //拉低时钟线
}
 
/*-------------------------------------------------*/
/*函数名：停止信号                     			   */
/*参  数：                                         */
/*返回值：                                         */
/*-------------------------------------------------*/
void BH1750_Stop()
{
	IIC_OUTPUT_MODE_SET(); 	//配置输出模式
	IIC_SDA_OUT = 0;        //拉低数据线
	IIC_SCL = 1;            //拉高时钟线
	delay_us(5);            //延时
	IIC_SDA_OUT = 1;        //产生上升沿
	delay_us(5);            //延时
}
 
/*-------------------------------------------------*/
/*函数名：发送应答信号                  		       */
/*参  数：ack 应答信号,1或0                        */
/*返回值：                                         */
/*-------------------------------------------------*/
void BH1750_SendACK(char ack)
{
	IIC_OUTPUT_MODE_SET(); 	  //配置输出模式
	IIC_SCL = 0;              //拉低时钟线
	if(ack) IIC_SDA_OUT = 1; 
	else IIC_SDA_OUT = 0;     //写应答信号
	delay_us(2);              //延时
	IIC_SCL = 1;              //拉高时钟线
	delay_us(5);              //延时
	IIC_SCL = 0;              //拉低时钟线
}
 
/*-------------------------------------------------*/
/*函数名：接收应答信号                  		       */
/*参  数：										   */
/*返回值：                                         */
/*-------------------------------------------------*/
char BH1750_RecvACK()
{
	u8 cnt = 0;
	IIC_INPUT_MODE_SET(); //输入模式
	IIC_SDA_OUT = 1; 
	delay_us(1);
	IIC_SCL = 1;
	delay_us(1);
	while(IIC_SDA_IN)	  //等待应答
	{
		cnt++;
		delay_us(1);
		if(cnt >= 250)	  //等待时间过长，产生停止信号，返回1，表示接收应答失败
		{
			BH1750_Stop();
			return 1;
		}
	}
	IIC_SCL = 0;		  //应答成功，则SCL变低
	return 0;
}
 
/*-------------------------------------------------*/
/*函数名：向IIC发送一个字节数据         		       */
/*参  数：data:发送的数据				           */
/*返回值：                                         */
/*-------------------------------------------------*/
void BH1750_SendByte(u8 data)
{
	u8 i;
	IIC_OUTPUT_MODE_SET();
	IIC_SCL = 0;				      //拉低时钟线, 数据准备发送
	delay_us(2);					  //延时
	for (i = 0;i < 8; i++)			  //从高位开始一位一位地传送
	{
		if(data&0x80) IIC_SDA_OUT = 1;//送数据口
		else IIC_SDA_OUT = 0;
		data <<= 1;				      //移出数据的最高位
		IIC_SCL = 1;				  //拉高时钟线,发送数据
		delay_us(2);				  //延时
		IIC_SCL = 0;			      //拉低时钟线，数据发送完毕
		delay_us(2);				  //延时
	}
}

/*-------------------------------------------------*/
/*函数名：从IIC总线接收一个数据         		       */
/*参  数：			                  			   */
/*返回值：                                         */
/*-------------------------------------------------*/
u8 BH1750_RecvByte()
{
	u8 i;
	u8 data = 0;
	IIC_INPUT_MODE_SET();		    //使能内部上拉,准备读取数据,
	for (i = 0; i < 8; i++)   		//8位计数器
	{
		IIC_SCL = 0;				//置时钟线为低，准备接收数据位
		delay_us(2);			    //时钟低电平周期大于4.7μs
		IIC_SCL = 1;			   	//置时钟线为高使数据线上数据有效，主机开始读数据，从机不能再改变数据了，即改变SDA的电平
		data <<= 1; 
		IIC_SCL = 1;			 	//拉高时钟线
		if(IIC_SDA_IN) data |= 0x01;//读数据               
		delay_us(1);				 				
	}
	IIC_SCL = 0;
	return data;
}

/*-------------------------------------------------*/
/*函数名：向BY30内部寄存器地址写入数据         	   */
/*参  数：REG_Address：BY30内部寄存器地址    	   */
/*返回值：                                         */
/*-------------------------------------------------*/
void Single_Write_BH1750(u8 REG_Address)
{
	BH1750_Start();					//起始信号
	BH1750_SendByte(SlaveAddress);	//发送设备地址+写信号
	BH1750_RecvACK();
	BH1750_SendByte(REG_Address); 	//内部寄存器地址，请参考中文pdf22页 
	BH1750_RecvACK();
	BH1750_Stop();					//发送停止信号
}
/*-------------------------------------------------*/
/*函数名：连续读出BH1750数据					       */
/*参  数：										   */
/*返回值：                                         */
/*-------------------------------------------------*/
void Multiple_Read_BH1750()
{   
  BH1750_Start();				  //起始信号
  BH1750_SendByte(SlaveAddress+1);//发送设备地址+读信号
	BH1750_RecvACK();
	BUF[0] = BH1750_RecvByte();	  //BUF[0]存储0x32地址中的数据
	BH1750_SendACK(0);
	BUF[1] = BH1750_RecvByte();   //BUF[0]存储0x32地址中的数据
	BH1750_SendACK(1);
	BH1750_Stop();
	delay_ms(5);
}
 
/*-------------------------------------------------*/
/*函数名：读出数据并合成光照强度                    */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
int get_sunlight_value()
{
	int dis_data = 0;
	int temp;
	u8 i = 0;
	Single_Write_BH1750(0x01);    //power on
	Single_Write_BH1750(0x10);	  //H- resolution mode
	delay_ms(180);				  //延时180ms
	Multiple_Read_BH1750();		  //连续读出数据，存储在BUF中  
	for(i = 0; i < 3; i++)
	dis_data = BUF[0];
	dis_data = (dis_data << 8) + BUF[1];//合成数据 
	temp=(int)dis_data* 10/12;
	//printf("Sunlight = %d\r\n",temp);
	return temp;
}



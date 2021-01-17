
#ifndef __MQTT_H
#define __MQTT_H

#define  R_NUM               5     //接收缓冲区个数
#define  RBUFF_UNIT          300   //接收缓冲区长度

#define  T_NUM               5     //发送缓冲区个数  
#define  TBUFF_UNIT          300   //发送缓冲区长度

#define  C_NUM               5     //命令缓冲区个数
#define  CBUFF_UNIT          300   //命令缓冲区长度

#define  MQTT_TxData(x)       u2_TxData(x)  //串口2负责数据发送

extern const char PRODUCTID[];      //产品ID，在main.c中定义
extern const char DEVICEID [];      //设备ID，在main.c中定义 
extern const char AUTHENTICATION[]; //鉴权信息，在main.c中定义 
extern const char DATA_TOPIC_NAME[];//topic，Onenet数据点上传topic，在main.c中定义
extern const char SERVER_IP[];		//存放服务器IP或域名，在main.c中定义
extern const int  SERVER_PORT;		//存放服务器的端口号，在main.c中定义

extern unsigned char  MQTT_RxDataBuf[R_NUM][RBUFF_UNIT];//外部变量声明，数据的接收缓冲区,所有服务器发来的数据，存放在该缓冲区,缓冲区第一个字节存放数据长度
extern unsigned char *MQTT_RxDataInPtr;                 //外部变量声明，指向缓冲区存放数据的位置
extern unsigned char *MQTT_RxDataOutPtr;                //外部变量声明，指向缓冲区读取数据的位置
extern unsigned char *MQTT_RxDataEndPtr;                //外部变量声明，指向缓冲区结束的位置
extern unsigned char  MQTT_TxDataBuf[T_NUM][TBUFF_UNIT];//外部变量声明，数据的发送缓冲区,所有发往服务器的数据，存放在该缓冲区,缓冲区第一个字节存放数据长度
extern unsigned char *MQTT_TxDataInPtr;                 //外部变量声明，指向缓冲区存放数据的位置
extern unsigned char *MQTT_TxDataOutPtr;                //外部变量声明，指向缓冲区读取数据的位置
extern unsigned char *MQTT_TxDataEndPtr;                //外部变量声明，指向缓冲区结束的位置
extern unsigned char  MQTT_CMDBuf[C_NUM][CBUFF_UNIT];   //外部变量声明，命令数据的接收缓冲区
extern unsigned char *MQTT_CMDInPtr;                    //外部变量声明，指向缓冲区存放数据的位置
extern unsigned char *MQTT_CMDOutPtr;                   //外部变量声明，指向缓冲区读取数据的位置
extern unsigned char *MQTT_CMDEndPtr;                   //外部变量声明，指向缓冲区结束的位置

extern char ClientID[128];     //外部变量声明，存放客户端ID的缓冲区
extern int  ClientID_len;      //外部变量声明，存放客户端ID的长度
extern char Username[128];     //外部变量声明，存放用户名的缓冲区
extern int  Username_len;	   //外部变量声明，存放用户名的长度
extern char Passward[128];     //外部变量声明，存放密码的缓冲区
extern int  Passward_len;	   //外部变量声明，存放密码的长度
extern char ServerIP[128];     //外部变量声明，存放服务器IP或是域名
extern int  ServerPort;        //外部变量声明，存放服务器的端口号

extern char pingFlag;          //外部变量声明，ping报文状态      0：正常状态，等待计时时间到，发送Ping报文


void MQTT_Buff_Init(void);
void IoT_parameter_init(void);
void MQTT_ConectPack(void);
void MQTT_Subscribe(char *, int);
void MQTT_PingREQ(void);
void MQTT_PublishQs0(const char *, char *, int);
void MQTT_DealPushdata_Qs0(unsigned char *);	
void TxDataBuf_Deal(unsigned char *, int);
void CMDBuf_Deal(unsigned char *, int);

#endif

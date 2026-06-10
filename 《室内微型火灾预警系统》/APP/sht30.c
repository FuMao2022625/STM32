#include "SHT30.h"
 
#define write 0
#define read  1
//IIC总线地址接口定义
//#define SCL PAout(4)
//#define SDA_OUT PAout(5)
//#define SDA_IN PAin(5)
//#define IIC_INPUT_MODE_SET()  {GPIOA->CRL&=0xFF0FFFFF;GPIOA->CRL|=0x00800000;}
//#define IIC_OUTPUT_MODE_SET() {GPIOA->CRL&=0xFF0FFFFF;GPIOA->CRL|=0x00300000;}
// 原 SCL 使用 PA4，现改为 PA7
#define SCL PAout(7)          // PA7 输出作为 SCL
#define SDA_OUT PAout(5)      // PA5 输出作为 SDA（写）
#define SDA_IN PAin(5)        // PA5 输入作为 SDA（读）

// IIC 输入/输出模式设置宏（只针对 SDA 引脚 PA5，保持不变）
#define IIC_INPUT_MODE_SET()  {GPIOA->CRL &= 0xFF0FFFFF; GPIOA->CRL |= 0x00800000;}
#define IIC_OUTPUT_MODE_SET() {GPIOA->CRL &= 0xFF0FFFFF; GPIOA->CRL |= 0x00300000;}
float humiture[4];
u8 Refresh_SHT30_Data=0;
u8 humiture_buff1[20];
u8 humiture_buff2[20];
 
u8 Temperature_L=15;
u8 Humidity_L=50;
u8 Temperature_H=30;
u8 Humidity_H=80;

 
//void SHT30_Init(void)
//{
//  /*1.开时钟*/
//  RCC->APB2ENR |= 1<<2;
//  /*2.设置GPIO模式*/
//  GPIOA->CRL &= 0x0000FFFF;
//  GPIOA->CRL |= 0x33330000;
//  /*3.设置GPIO空闲电平*/
//  GPIOA->ODR |= 0xF<<4;	
//  printf("SHT30_Init OK！\n");
//}

void SHT30_Init(void)
{
    /* 1. 开启 GPIOA 时钟 */
    RCC->APB2ENR |= 1 << 2;   // 使能 GPIOA 时钟

    /* 2. 设置 GPIO 模式：PA5 (SDA) 和 PA7 (SCL) 为推挽输出，50MHz */
    GPIOA->CRL &= ~((0xF << 20) | (0xF << 28)); // 清除 PA5 和 PA7 的配置位
    GPIOA->CRL |= (0x3 << 20) | (0x3 << 28);    // 设置 PA5 和 PA7 为推挽输出 50MHz

    /* 3. 初始电平拉高：SDA 和 SCL 空闲时为高 */
    GPIOA->ODR |= (1 << 5) | (1 << 7);          // PA5 和 PA7 输出高电平

    //printf("SHT30_Init OK！\n");
}
 /*主机发送ACK*/
void IIC_ACK(void)
{
  IIC_OUTPUT_MODE_SET();
  SCL=0;
  SDA_OUT=0;
  Delay_us(2);     
  SCL=1;
  Delay_us(2);                  
  SCL=0;                     
  Delay_us(1);  

}
 /*主机不发送ACK*/
void IIC_NACK(void)
{
  IIC_OUTPUT_MODE_SET();
  SCL=0;
  SDA_OUT=1;
  Delay_us(2);      
  SCL=1;
  Delay_us(2);                   
  SCL=0;                     
  Delay_us(1);    
}
 /*主机等待从机的ACK*/
u8 IIC_wait_ACK(void)
{
    u8 t = 200;
    IIC_OUTPUT_MODE_SET();
    SDA_OUT=1;//8位发送完后释放数据线，准备接收应答位 
    Delay_us(1);
    SCL=1;
    Delay_us(1); 
    IIC_INPUT_MODE_SET();
    Delay_us(1); 
    while(SDA_IN)//等待SHT30应答
    {
	    t--;
	    Delay_us(1); 
	    if(t==0)
	    {
	          SCL=0;
	          return 1;
	    }
	    Delay_us(1); 
    }
    SCL=0;             
    Delay_us(1);
    return 0;	
}
/*******************************************************************
功能:启动I2C总线,即发送I2C起始条件.  
********************************************************************/
void IIC_Start(void)
{
  IIC_OUTPUT_MODE_SET();
  SCL=1;
  SDA_OUT=1;
  Delay_us(2);	
  SDA_OUT=0;
  Delay_us(2); 
}
 
/*******************************************************************
功能:结束I2C总线,即发送I2C结束条件.  
********************************************************************/
void IIC_Stop(void)
{
	IIC_OUTPUT_MODE_SET();
	SCL=1;
	SDA_OUT=0;  
	Delay_us(2);	
	SDA_OUT=1;
	Delay_us(2);
}
 
/*******************************************************************
字节数据发送函数               
函数原型: void  SendByte(UCHAR c);
功能:将数据c发送出去,可以是地址,也可以是数据
********************************************************************/
void  IIC_SendByte(u8 byte)
{
	u8  BitCnt;
	IIC_OUTPUT_MODE_SET();
	SCL=0;
	for(BitCnt=0;BitCnt<8;BitCnt++)//要传送的数据长度为8位
	{
		if(byte&0x80) SDA_OUT=1;//判断发送位
		else SDA_OUT=0; 
		byte<<=1;
		Delay_us(2); 
		SCL=1;
		Delay_us(2);
		SCL=0;
		Delay_us(2);
        
	}
}
/*******************************************************************
 字节数据接收函数               
函数原型: UCHAR  RcvByte();
功能: 用来接收从器件传来的数据  
********************************************************************/    
u8 IIC_RcvByte(void)
{
  u8 retc;
  u8 BitCnt;
  retc=0; 
  IIC_INPUT_MODE_SET();//置数据线为输入方式
  SCL=0;//置时钟线为低，准备接收数据位
  Delay_us(2);                    
  for(BitCnt=0;BitCnt<8;BitCnt++)
  {  
             
	SCL=1;//置时钟线为高使数据线上数据有效                
	retc=retc<<1;
	if(SDA_IN) retc |=1;//读数据位,接收的数据位放入retc中 
	Delay_us(2);
    SCL=0;
    Delay_us(2);
  }   
  return(retc);
}
/*******************************************************************
 温湿度获取函数               
函数原型: SHT30_read_result(u8 addr);
功能: 用来接收从器件采集并合成温湿度
********************************************************************/ 
SHT30_Data SHT30_read_result(u8 addr)
{
	u16 tem,hum;
	u16 buff[6];
	float Temperature=0;
	float Humidity=0;
	SHT30_Data data = {0, 0};
	
	IIC_Start();
	IIC_SendByte(addr<<1 | write);//写7位I2C设备地址加0作为写取位,1为读取位
	IIC_wait_ACK();
	IIC_SendByte(0x2C);
	IIC_wait_ACK();
	IIC_SendByte(0x06);
	IIC_wait_ACK();
	IIC_Stop();
	Delayms(20);//写完操作指令后，起码要等20ms(使用SHT30测试得到可以按照实际情况加长延时)，SHT3x传感器才能把温湿度采集好！！！
	IIC_Start();
	IIC_SendByte(addr<<1 | read);//写7位I2C设备地址加0作为写取位,1为读取位
	if(IIC_wait_ACK()==0)
	{
		buff[0]=IIC_RcvByte();
		IIC_ACK();
		buff[1]=IIC_RcvByte();
		IIC_ACK();
		buff[2]=IIC_RcvByte();
		IIC_ACK();
		buff[3]=IIC_RcvByte();
		IIC_ACK();
		buff[4]=IIC_RcvByte();
		IIC_ACK();
		buff[5]=IIC_RcvByte();
		IIC_NACK();
		IIC_Stop();
	}
	
	tem = ((buff[0]<<8) | buff[1]);//温度拼接
	hum = ((buff[3]<<8) | buff[4]);//湿度拼接
	
	/*转换实际温度*/
	Temperature= (175.0*(float)tem/65535.0-45.0) ;// T = -45 + 175 * tem / (2^16-1)
	Humidity= (100.0*(float)hum/65535.0);// RH = hum*100 / (2^16-1)

	if((Temperature>=-20)&&(Temperature<=125)&&(Humidity>=0)&&(Humidity<=100))//过滤错误数据
	{
		humiture[0]=Temperature;
		humiture[2]=Humidity;
		sprintf(humiture_buff1,"%6.2f*C %6.2f%%",Temperature,Humidity);//111.01*C 100.01%（保留2位小数）
		//sprintf((char *)humiture_buff1, "%6.2f*C %6.2f%%", Temperature, Humidity);
		data.Temperature = Temperature;
		data.Humidity = Humidity;
	}
	//printf("温湿度：%s\n",humiture_buff1);
	hum=0;
	tem=0;
	
	return data;
}

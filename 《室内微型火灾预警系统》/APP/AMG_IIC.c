#include "AMG_IIC.h"
 
void AMG8833_IIC_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	AMG_SDA_RCC, ENABLE );	
	GPIO_InitStructure.GPIO_Pin = AMG_SDA_PIN;               //端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;       //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       //50Mhz速度
	GPIO_Init(AMG_SDA_IOx, &GPIO_InitStructure);
 
	RCC_APB2PeriphClockCmd(	AMG_SCL_RCC, ENABLE );	
	GPIO_InitStructure.GPIO_Pin = AMG_SCL_PIN;               //端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;       //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;       //50Mhz速度
	GPIO_Init(AMG_SCL_IOx, &GPIO_InitStructure);
	
	GPIO_SetBits(AMG_SDA_IOx,AMG_SDA_PIN);//SDA输出高	
	GPIO_SetBits(AMG_SCL_IOx,AMG_SCL_PIN);//SCL输出高	
}
 
void AMG_IIC_Start(void)
{
	AMG_SDA_OUT();//sda线输出
	AMG_IIC_SDA=1;	  	  
	AMG_IIC_SCL=1;
	Delay_us(4);
 	AMG_IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	Delay_us(4);
	AMG_IIC_SCL=0;//钳住I2C总线，准备发送或接收数据 
}
 
//产生IIC停止信号
void AMG_IIC_Stop(void)
{
	AMG_SDA_OUT();//sda线输出
	AMG_IIC_SCL=0;
	AMG_IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	Delay_us(4);
	AMG_IIC_SCL=1; 
	AMG_IIC_SDA=1;//发送I2C总线结束信号
	Delay_us(4);							   	
}
 
u8 AMG_IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	AMG_SDA_IN();  //SDA设置为输入  
	AMG_IIC_SDA=1;Delay_us(1);	   
	AMG_IIC_SCL=1;Delay_us(1);	 
	while(AMG_READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			AMG_IIC_Stop();
			return 1;
		}
	}
	AMG_IIC_SCL=0;//时钟输出0 	   
	return 0;  
}
 
//产生ACK应答
void AMG_IIC_Ack(void)
{
	AMG_IIC_SCL=0;
	AMG_SDA_OUT();
	AMG_IIC_SDA=0;
	Delay_us(2);
	AMG_IIC_SCL=1;
	Delay_us(2);
	AMG_IIC_SCL=0;
}
 
//不产生ACK应答		    
void AMG_IIC_NAck(void)
{
	AMG_IIC_SCL=0;
	AMG_SDA_OUT();
	AMG_IIC_SDA=1;
	Delay_us(2);
	AMG_IIC_SCL=1;
	Delay_us(2);
	AMG_IIC_SCL=0;
}
 
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void AMG_IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	AMG_SDA_OUT(); 	    
    AMG_IIC_SCL=0;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
		if((txd&0x80)>>7)
			AMG_IIC_SDA=1;
		else
			AMG_IIC_SDA=0;
		txd<<=1; 	  
		Delay_us(2);  
		AMG_IIC_SCL=1;
		Delay_us(2); 
		AMG_IIC_SCL=0;	
		Delay_us(2);
    }	 
} 
 
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 AMG_IIC_Read_Byte(void)
{
	unsigned char i,receive=0;
	AMG_SDA_IN();          //SDA设置为输入
	AMG_IIC_SDA = 1;
	Delay_us(4);
	for(i=0;i<8;i++ )
	{
		receive<<=1;
		AMG_IIC_SCL=0; 
		Delay_us(4);
	    AMG_IIC_SCL=1;
		Delay_us(4);
		if(AMG_READ_SDA)
			receive |= 0x01;   
	    Delay_us(4); //1
	}	
    AMG_IIC_SCL = 0;	
	return receive;
}
 
//IIC写一个字节数据
u8 AMG_IIC_Write_1Byte(u8 SlaveAddress, u8 REG_Address,u8 REG_data)
{
	AMG_IIC_Start();
	AMG_IIC_Send_Byte(SlaveAddress);
	if(AMG_IIC_Wait_Ack())
	{
		AMG_IIC_Stop();//释放总线
		return 1;//没应答则退出
 
	}
	AMG_IIC_Send_Byte(REG_Address);
	AMG_IIC_Wait_Ack();	
	Delay_us(5);
	AMG_IIC_Send_Byte(REG_data);
	AMG_IIC_Wait_Ack();	
	AMG_IIC_Stop();
 
	return 0;
}
 
//IIC读一个字节数据
u8 AMG_IIC_Read_1Byte(u8 SlaveAddress, u8 REG_Address,u8 *REG_data) 
{
	AMG_IIC_Start();
	AMG_IIC_Send_Byte(SlaveAddress);//发写命令
	if(AMG_IIC_Wait_Ack())
	{
		 AMG_IIC_Stop();//释放总线
		 return 1;//没应答则退出
	}		
	AMG_IIC_Send_Byte(REG_Address);
	AMG_IIC_Wait_Ack();
	Delay_us(5);
	AMG_IIC_Start(); 
	AMG_IIC_Send_Byte(SlaveAddress|0x01);//发读命令
	AMG_IIC_Wait_Ack();
	*REG_data = AMG_IIC_Read_Byte();
	AMG_IIC_Stop();
 
	return 0;
}
 
//I2C读多个字节数据
uint8_t AMG_I2C_Read_nByte(uint8_t SlaveAddress, uint8_t REG_Address, uint8_t *buf, uint16_t len)
{
	AMG_IIC_Start();
	AMG_IIC_Send_Byte(SlaveAddress);//发写命令
	if(AMG_IIC_Wait_Ack()) 
	{
		AMG_IIC_Stop();//释放总线
		return 1;//没应答则退出
	}
	AMG_IIC_Send_Byte(REG_Address);
	AMG_IIC_Wait_Ack();
	Delay_us(5);
	AMG_IIC_Start(); 
	AMG_IIC_Send_Byte(SlaveAddress|0x01);//发读命令
	AMG_IIC_Wait_Ack();
	while(len)
	{
		*buf = AMG_IIC_Read_Byte();
		if(1 == len)
		{
			AMG_IIC_NAck();
		}
		else
		{
			AMG_IIC_Ack();
		}
		buf++;
		len--;
	}
	AMG_IIC_Stop();
 
	return STATUS_OK;
}
 
//I2C写多个字节数据
uint8_t AMG_I2C_Write_nByte(uint8_t SlaveAddress, uint8_t REG_Address, uint8_t *buf, uint16_t len)
{
	AMG_IIC_Start();
	AMG_IIC_Send_Byte(SlaveAddress);//发写命令
	if(AMG_IIC_Wait_Ack()) 
	{
		AMG_IIC_Stop();//释放总线
		return 1;//没应答则退出
	}
	AMG_IIC_Send_Byte(REG_Address);
	AMG_IIC_Wait_Ack();
	while(len--)
	{
		AMG_IIC_Send_Byte(*buf++);
		AMG_IIC_Wait_Ack();
	}
	AMG_IIC_Stop();
 
	return STATUS_OK;
}

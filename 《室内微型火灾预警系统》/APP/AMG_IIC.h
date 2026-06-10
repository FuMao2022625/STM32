#ifndef __AMG_I2C_H
#define __AMG_I2C_H
 
#include "stdint.h"
#include "sys.h"
#include "interface.h"
 
//#define AMG_SDA_RCC  RCC_APB2Periph_GPIOB
//#define AMG_SDA_PIN  GPIO_Pin_7
//#define AMG_SDA_IOx  GPIOB  
//#define AMG_SCL_RCC  RCC_APB2Periph_GPIOB
//#define AMG_SCL_PIN  GPIO_Pin_6
//#define AMG_SCL_IOx  GPIOB  
// 
//#define AMG_SDA_IN()  {GPIOB->CRL&=0x0FFFFFFF;GPIOB->CRL|=8<<28;}
//#define AMG_SDA_OUT() {GPIOB->CRL&=0x0FFFFFFF;GPIOB->CRL|=3<<28;}
//#define AMG_IIC_SCL    PBout(6)      //SCL
//#define AMG_IIC_SDA    PBout(7)      //SDA	 
//#define AMG_READ_SDA   PBin(7) 		 //输入SDA 
// ========== AMG8833 I2C 引脚定义（PA1=SDA, PA3=SCL）==========
#define AMG_SDA_RCC  RCC_APB2Periph_GPIOA
#define AMG_SDA_PIN  GPIO_Pin_1
#define AMG_SDA_IOx  GPIOA  
#define AMG_SCL_RCC  RCC_APB2Periph_GPIOA
#define AMG_SCL_PIN  GPIO_Pin_3
#define AMG_SCL_IOx  GPIOA  

// 方向控制（操作 CRL 寄存器）
// PA1 在 CRL 的偏移：1*4 = 4 位，PA3 偏移：3*4 = 12 位
// 输入模式：浮空输入 (CNF=01, MODE=00) → 0x4
// 输出模式：推挽输出 50MHz (CNF=00, MODE=11) → 0x3
// 注意：这里只对 SDA 进行方向切换，SCL 始终为输出
#define AMG_SDA_IN()  { GPIOA->CRL &= ~(0xF << (1*4)); GPIOA->CRL |= (0x4 << (1*4)); }
#define AMG_SDA_OUT() { GPIOA->CRL &= ~(0xF << (1*4)); GPIOA->CRL |= (0x3 << (1*4)); }

// 引脚电平操作
#define AMG_IIC_SCL    PAout(3)      // SCL 输出 (PA3)
#define AMG_IIC_SDA    PAout(1)      // SDA 输出 (PA1)
#define AMG_READ_SDA   PAin(1)       // 输入 SDA (PA1)
 
#define AMG88xx_ADR    0xD0          //5脚拉低时的设备地址
 
//Status
#define STATUS_OK       0x00
#define STATUS_FAIL     0x01
 
void AMG8833_IIC_Init(void);
u8 AMG_IIC_Write_1Byte(u8 SlaveAddress, u8 REG_Address,u8 REG_data);
u8 AMG_IIC_Read_1Byte(u8 SlaveAddress, u8 REG_Address,u8 *REG_data);
uint8_t AMG_I2C_Read_nByte(uint8_t SlaveAddress, uint8_t REG_Address, uint8_t *buf, uint16_t len);
 
#endif 
 
 

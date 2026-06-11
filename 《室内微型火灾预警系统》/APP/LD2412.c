#include "LCD1602.h"
#include "interface.h"
#include "stm32f10x.h"
#include "LD2412.h"
#define LD2412_DEBOUNCE_SAMPLES  5

// LD2412初始化
void LD2412_Init(void)
{		
	GPIO_InitTypeDef GPIO_InitStructure;  // 定义串口对应管脚的结构体
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOF,ENABLE);  //使能PORTA,PORTE时钟
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_11;  // PF11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入,默认为低电平  有人为高电平，无人为低电平
 	GPIO_Init(GPIOF, &GPIO_InitStructure);  //初始化GPIOF11
}


/**
 * @brief 读取人体存在状态（带消抖）
 * @retval 1 有人，0 无人
 */
uint8_t LD2412_Read(void)
{
    uint8_t sample = 0;
    uint8_t i;
    // 连续采样多次，统计高电平次数
    for (i = 0; i < LD2412_DEBOUNCE_SAMPLES; i++) {
        if (FLAG_PEOPLE == Bit_SET) {
            sample++;
        }
        // 可添加微小延时，但通常引脚电平变化很快，可省略
        // delay_us(10);
    }
    // 如果采样中高电平次数超过一半，则认为有人
    return (sample > (LD2412_DEBOUNCE_SAMPLES / 2)) ? 1 : 0;
}


#ifndef __BEEP_H
#define __BEEP_H

#include "stm32f10x.h"

/* 定义蜂鸣器（喇叭）连接的GPIO端口，PB0 */
#define BEEP_GPIO_PORT    	GPIOB
#define BEEP_GPIO_CLK 	    RCC_APB2Periph_GPIOB
#define BEEP_GPIO_PIN		GPIO_Pin_0

/* 报警声控制宏 */
#define BEEP_ON()   BEEP_StartAlarm()
#define BEEP_OFF()  BEEP_StopAlarm()

void BEEP_GPIO_Config(void);      // 初始化GPIO
void BEEP_StartAlarm(void);       // 启动报警
void BEEP_StopAlarm(void);        // 停止报警
void BEEP_IRQHandler(void);       // 中断处理函数（由 stm32f10x_it.c 调用）

#endif

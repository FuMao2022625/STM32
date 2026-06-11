#ifndef __BSP_BEEP_H
#define __BSP_BEEP_H

#include "stm32f1xx_hal.h"

#define BEEP_PIN GPIO_PIN_12
#define BEEP_PORT GPIOB

void BEEP_Init(void);
void BEEP_On(void);
void BEEP_Off(void);
void BEEP_Alarm(uint16_t duration_ms);

#endif
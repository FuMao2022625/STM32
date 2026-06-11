#ifndef __BSP_MQ2_H
#define __BSP_MQ2_H

#include "stm32f1xx_hal.h"

typedef struct {
    float ppm;
    float voltage;
} MQ2_Data;

void MQ2_Init(ADC_HandleTypeDef *hadc);
uint8_t MQ2_ReadData(ADC_HandleTypeDef *hadc, MQ2_Data *data);

#endif
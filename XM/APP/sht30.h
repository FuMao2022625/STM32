#ifndef __SHT30_H
#define __SHT30_H

#include "stm32f1xx_hal.h"

#define SHT30_ADDR 0x44 << 1

typedef struct {
    float temperature;
    float humidity;
} SHT30_Data;

uint8_t SHT30_Init(I2C_HandleTypeDef *hi2c);
uint8_t SHT30_ReadData(I2C_HandleTypeDef *hi2c, SHT30_Data *data);

#endif
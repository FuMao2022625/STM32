#ifndef __AMG8833_H
#define __AMG8833_H

#include "stm32f1xx_hal.h"

#define AMG8833_ADDR 0x69 << 1

#define AMG8833_PIXELS 64

typedef struct {
    float pixels[AMG8833_PIXELS];
    float max_temp;
    float min_temp;
} AMG8833_Data;

uint8_t AMG8833_Init(I2C_HandleTypeDef *hi2c);
uint8_t AMG8833_ReadData(I2C_HandleTypeDef *hi2c, AMG8833_Data *data);

#endif
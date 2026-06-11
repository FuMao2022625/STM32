#ifndef __LD2412_H
#define __LD2412_H

#include "stm32f1xx_hal.h"

#define LD2412_BUFFER_SIZE 64

typedef struct {
    uint8_t has_target;
    uint16_t distance;
    uint8_t strength;
} LD2412_Data;

void LD2412_Init(UART_HandleTypeDef *huart);
uint8_t LD2412_ReadData(UART_HandleTypeDef *huart, LD2412_Data *data);

#endif
#ifndef __INTERFACE_H
#define __INTERFACE_H

#include "stm32f1xx_hal.h"
#include "function.h"

void System_Init(I2C_HandleTypeDef *hi2c, ADC_HandleTypeDef *hadc, UART_HandleTypeDef *huart_ld2412);
void System_MainLoop(SensorData *data, I2C_HandleTypeDef *hi2c, ADC_HandleTypeDef *hadc, 
                     UART_HandleTypeDef *huart_debug, UART_HandleTypeDef *huart_ld2412, UART_HandleTypeDef *huart_esp8266);

#endif
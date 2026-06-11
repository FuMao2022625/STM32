#ifndef __LCD1602_H
#define __LCD1602_H

#include "stm32f1xx_hal.h"

#define LCD_RS_PIN    GPIO_PIN_13
#define LCD_RS_PORT   GPIOC
#define LCD_EN_PIN    GPIO_PIN_14
#define LCD_EN_PORT   GPIOC
#define LCD_DATA_PORT GPIOF

#define LCD_RS_SET    HAL_GPIO_WritePin(LCD_RS_PORT, LCD_RS_PIN, GPIO_PIN_SET)
#define LCD_RS_RESET  HAL_GPIO_WritePin(LCD_RS_PORT, LCD_RS_PIN, GPIO_PIN_RESET)
#define LCD_EN_SET    HAL_GPIO_WritePin(LCD_EN_PORT, LCD_EN_PIN, GPIO_PIN_SET)
#define LCD_EN_RESET  HAL_GPIO_WritePin(LCD_EN_PORT, LCD_EN_PIN, GPIO_PIN_RESET)

void LCD1602_Init(void);
void LCD1602_Clear(void);
void LCD1602_SetCursor(uint8_t row, uint8_t col);
void LCD1602_PrintString(const char* str);
void LCD1602_PrintFloat(float value, uint8_t decimals);
void LCD1602_PrintInt(int value);
void LCD1602_DisplaySensorData(float temp, float humi, float smoke, float max_temp, uint8_t alarm_type);

#endif
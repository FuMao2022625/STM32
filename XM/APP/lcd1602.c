#include "lcd1602.h"
#include <stdio.h>
#include <string.h>

static void LCD1602_WriteData(uint8_t data)
{
    HAL_GPIO_WritePin(LCD_DATA_PORT, 0xFF, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_DATA_PORT, data & 0xFF, GPIO_PIN_SET);
    LCD_EN_SET;
    HAL_Delay(1);
    LCD_EN_RESET;
    HAL_Delay(1);
}

static void LCD1602_WriteCommand(uint8_t cmd)
{
    LCD_RS_RESET;
    HAL_GPIO_WritePin(LCD_DATA_PORT, 0xFF, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_DATA_PORT, cmd & 0xFF, GPIO_PIN_SET);
    LCD_EN_SET;
    HAL_Delay(1);
    LCD_EN_RESET;
    HAL_Delay(1);
}

void LCD1602_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    
    GPIO_InitStruct.Pin = LCD_RS_PIN | LCD_EN_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LCD_RS_PORT, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 |
                         GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LCD_DATA_PORT, &GPIO_InitStruct);
    
    HAL_Delay(20);
    LCD1602_WriteCommand(0x38);
    HAL_Delay(5);
    LCD1602_WriteCommand(0x38);
    HAL_Delay(5);
    LCD1602_WriteCommand(0x38);
    HAL_Delay(5);
    
    LCD1602_WriteCommand(0x0C);
    HAL_Delay(5);
    LCD1602_WriteCommand(0x06);
    HAL_Delay(5);
    LCD1602_WriteCommand(0x01);
    HAL_Delay(5);
}

void LCD1602_Clear(void)
{
    LCD1602_WriteCommand(0x01);
    HAL_Delay(5);
}

void LCD1602_SetCursor(uint8_t row, uint8_t col)
{
    uint8_t address;
    if (row == 0) {
        address = 0x80 + col;
    } else {
        address = 0xC0 + col;
    }
    LCD1602_WriteCommand(address);
}

void LCD1602_PrintString(const char* str)
{
    LCD_RS_SET;
    while (*str) {
        LCD1602_WriteData(*str++);
    }
}

void LCD1602_PrintFloat(float value, uint8_t decimals)
{
    char buffer[20];
    snprintf(buffer, sizeof(buffer), "%.*f", decimals, value);
    LCD1602_PrintString(buffer);
}

void LCD1602_PrintInt(int value)
{
    char buffer[20];
    snprintf(buffer, sizeof(buffer), "%d", value);
    LCD1602_PrintString(buffer);
}

void LCD1602_DisplaySensorData(float temp, float humi, float smoke, float max_temp, uint8_t alarm_type)
{
    LCD1602_Clear();
    
    LCD1602_SetCursor(0, 0);
    LCD1602_PrintString("Temp:");
    LCD1602_PrintFloat(temp, 1);
    LCD1602_PrintString("C");
    
    LCD1602_SetCursor(0, 10);
    LCD1602_PrintString("Hum:");
    LCD1602_PrintFloat(humi, 1);
    LCD1602_PrintString("%");
    
    LCD1602_SetCursor(1, 0);
    LCD1602_PrintString("Smoke:");
    LCD1602_PrintFloat(smoke, 0);
    LCD1602_PrintString("ppm");
    
    LCD1602_SetCursor(1, 10);
    LCD1602_PrintString("IR:");
    LCD1602_PrintFloat(max_temp, 1);
    LCD1602_PrintString("C");
    
    if (alarm_type != 0) {
        LCD1602_Clear();
        LCD1602_SetCursor(0, 0);
        switch (alarm_type) {
            case 1:
                LCD1602_PrintString("ALARM: No Person");
                break;
            case 2:
                LCD1602_PrintString("ALARM: Critical!");
                break;
            case 3:
                LCD1602_PrintString("ALARM: Smoke!");
                break;
        }
        LCD1602_SetCursor(1, 0);
        LCD1602_PrintString("Fire Warning!");
    }
}
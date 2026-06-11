#include "AMG8833.h"

static uint8_t AMG8833_WriteReg(I2C_HandleTypeDef *hi2c, uint8_t reg, uint8_t value)
{
    uint8_t data[2] = {reg, value};
    return HAL_I2C_Master_Transmit(hi2c, AMG8833_ADDR, data, 2, 100);
}

static uint8_t AMG8833_ReadReg(I2C_HandleTypeDef *hi2c, uint8_t reg, uint8_t *data, uint8_t len)
{
    if (HAL_I2C_Master_Transmit(hi2c, AMG8833_ADDR, &reg, 1, 100) != HAL_OK)
        return 1;
    return HAL_I2C_Master_Receive(hi2c, AMG8833_ADDR, data, len, 100);
}

uint8_t AMG8833_Init(I2C_HandleTypeDef *hi2c)
{
    if (HAL_I2C_IsDeviceReady(hi2c, AMG8833_ADDR, 3, 100) != HAL_OK)
        return 1;
    
    AMG8833_WriteReg(hi2c, 0x00, 0x00);
    AMG8833_WriteReg(hi2c, 0x01, 0x00);
    
    return 0;
}

uint8_t AMG8833_ReadData(I2C_HandleTypeDef *hi2c, AMG8833_Data *data)
{
    uint8_t raw_data[AMG8833_PIXELS * 2];
    uint8_t i;
    
    if (AMG8833_ReadReg(hi2c, 0x80, raw_data, AMG8833_PIXELS * 2) != HAL_OK)
        return 1;
    
    data->max_temp = -100.0f;
    data->min_temp = 100.0f;
    
    for (i = 0; i < AMG8833_PIXELS; i++) {
        int16_t temp_raw = (raw_data[i * 2 + 1] << 8) | raw_data[i * 2];
        temp_raw = (temp_raw >> 2) & 0x3FFF;
        
        if (temp_raw & 0x2000)
            data->pixels[i] = (float)(temp_raw - 0x4000) * 0.25f;
        else
            data->pixels[i] = (float)temp_raw * 0.25f;
        
        if (data->pixels[i] > data->max_temp)
            data->max_temp = data->pixels[i];
        if (data->pixels[i] < data->min_temp)
            data->min_temp = data->pixels[i];
    }
    
    return 0;
}
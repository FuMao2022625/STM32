#include "sht30.h"

static uint8_t SHT30_WriteCommand(I2C_HandleTypeDef *hi2c, uint16_t cmd)
{
    uint8_t data[2];
    data[0] = (cmd >> 8) & 0xFF;
    data[1] = cmd & 0xFF;
    return HAL_I2C_Master_Transmit(hi2c, SHT30_ADDR, data, 2, 100);
}

uint8_t SHT30_Init(I2C_HandleTypeDef *hi2c)
{
    if (HAL_I2C_IsDeviceReady(hi2c, SHT30_ADDR, 3, 100) != HAL_OK)
        return 1;
    return SHT30_WriteCommand(hi2c, 0x30A2);
}

uint8_t SHT30_ReadData(I2C_HandleTypeDef *hi2c, SHT30_Data *data)
{
    uint8_t raw_data[6];
    
    if (SHT30_WriteCommand(hi2c, 0x2400) != HAL_OK)
        return 1;
    
    HAL_Delay(15);
    
    if (HAL_I2C_Master_Receive(hi2c, SHT30_ADDR, raw_data, 6, 100) != HAL_OK)
        return 1;
    
    uint16_t temp_raw = (raw_data[0] << 8) | raw_data[1];
    uint16_t hum_raw = (raw_data[3] << 8) | raw_data[4];
    
    data->temperature = -45 + 175 * ((float)temp_raw / 65535.0f);
    data->humidity = 100 * ((float)hum_raw / 65535.0f);
    
    return 0;
}
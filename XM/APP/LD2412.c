#include "LD2412.h"

uint8_t ld2412_buffer[LD2412_BUFFER_SIZE];
uint8_t ld2412_index = 0;

void LD2412_Init(UART_HandleTypeDef *huart)
{
    HAL_UART_Receive_IT(huart, &ld2412_buffer[ld2412_index], 1);
}

void LD2412_IRQHandler(UART_HandleTypeDef *huart)
{
    ld2412_index++;
    if (ld2412_index >= LD2412_BUFFER_SIZE)
        ld2412_index = 0;
    HAL_UART_Receive_IT(huart, &ld2412_buffer[ld2412_index], 1);
}

uint8_t LD2412_ReadData(UART_HandleTypeDef *huart, LD2412_Data *data)
{
    HAL_UART_Receive(huart, ld2412_buffer, 9, 500);
    
    if (ld2412_buffer[0] != 0xFD || ld2412_buffer[1] != 0xFC)
        return 1;
    
    data->has_target = ld2412_buffer[5];
    data->distance = (ld2412_buffer[6] << 8) | ld2412_buffer[7];
    data->strength = ld2412_buffer[8];
    
    return 0;
}
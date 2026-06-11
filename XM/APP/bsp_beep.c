#include "bsp_beep.h"

void BEEP_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    __HAL_RCC_GPIOB_CLK_ENABLE();
    
    GPIO_InitStruct.Pin = BEEP_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(BEEP_PORT, &GPIO_InitStruct);
    
    HAL_GPIO_WritePin(BEEP_PORT, BEEP_PIN, GPIO_PIN_RESET);
}

void BEEP_On(void)
{
    HAL_GPIO_WritePin(BEEP_PORT, BEEP_PIN, GPIO_PIN_SET);
}

void BEEP_Off(void)
{
    HAL_GPIO_WritePin(BEEP_PORT, BEEP_PIN, GPIO_PIN_RESET);
}

void BEEP_Alarm(uint16_t duration_ms)
{
    BEEP_On();
    HAL_Delay(duration_ms);
    BEEP_Off();
}
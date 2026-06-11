#include "bsp_mq2.h"
#include <math.h>

#define MQ2_RL 10.0f
#define MQ2_RO 10.0f

void MQ2_Init(ADC_HandleTypeDef *hadc)
{
    HAL_ADC_Start(hadc);
}

uint8_t MQ2_ReadData(ADC_HandleTypeDef *hadc, MQ2_Data *data)
{
    HAL_ADC_PollForConversion(hadc, 100);
    
    if (HAL_ADC_GetState(hadc) != HAL_ADC_STATE_REG_EOC)
        return 1;
    
    uint32_t adc_value = HAL_ADC_GetValue(hadc);
    
    data->voltage = (float)adc_value * 3.3f / 4095.0f;
    
    float rs = (3.3f - data->voltage) * MQ2_RL / data->voltage;
    float ratio = rs / MQ2_RO;
    
    if (ratio < 0.1f) ratio = 0.1f;
    
    data->ppm = 100.0f * powf(ratio, -1.4f);
    
    return 0;
}
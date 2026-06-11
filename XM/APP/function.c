#include "function.h"
#include <stdio.h>
#include <string.h>

static uint32_t human_absent_start_ms = 0;
static uint8_t human_absent_stable = 0;
static uint8_t high_temp_confirm = 0;
static uint8_t smoke_confirm = 0;
static uint8_t smoke_clear_confirm = 0;
static uint8_t alarm_active = 0;

void SensorData_Init(SensorData *data)
{
    data->sht30.temperature = 0.0f;
    data->sht30.humidity = 0.0f;
    data->mq2.ppm = 0.0f;
    data->mq2.voltage = 0.0f;
    data->ld2412.has_target = 0;
    data->ld2412.distance = 0;
    data->ld2412.strength = 0;
    data->battery_voltage = 0.0f;
    data->alarm_type = ALARM_NONE;
    data->human_absent = 1;
    strcpy(data->person_present, "false");
    data->smoke_level = 0.0f;
    data->temperature = 0.0f;
    data->humidity = 0.0f;
    data->max_temp = 0.0f;
}

uint8_t SensorData_CollectAll(SensorData *data, I2C_HandleTypeDef *hi2c, ADC_HandleTypeDef *hadc, UART_HandleTypeDef *huart_ld2412)
{
    uint8_t ret = 0;
    
    ret |= SHT30_ReadData(hi2c, &data->sht30);
    ret |= AMG8833_ReadData(hi2c, &data->amg8833);
    ret |= MQ2_ReadData(hadc, &data->mq2);
    ret |= LD2412_ReadData(huart_ld2412, &data->ld2412);
    
    HAL_ADC_Start(hadc);
    HAL_ADC_PollForConversion(hadc, 100);
    uint32_t adc_val = HAL_ADC_GetValue(hadc);
    data->battery_voltage = (float)adc_val * 3.3f / 4095.0f * 2.0f;
    
    data->human_absent = (data->ld2412.has_target == 0);
    strcpy(data->person_present, data->human_absent ? "false" : "true");
    data->temperature = data->sht30.temperature;
    data->humidity = data->sht30.humidity;
    data->smoke_level = data->mq2.ppm;
    data->max_temp = data->amg8833.max_temp;
    
    if (data->smoke_level > 4000.0f) {
        data->smoke_level = 4102.13f;
    }
    
    return ret;
}

uint8_t FireAlarm_Detect(SensorData *data, uint32_t now_ms)
{
    uint8_t high_temp_stable = 0;
    uint8_t smoke_stable = 0;
    
    data->alarm_type = ALARM_NONE;
    
    if (data->max_temp > TEMP_THRESHOLD_CRITICAL) {
        if (!alarm_active) {
            alarm_active = 1;
            data->alarm_type = ALARM_CRITICAL_TEMP;
        }
        return data->alarm_type;
    }
    
    if (data->smoke_level >= SMOKE_THRESHOLD) {
        if (smoke_confirm < SMOKE_CONFIRM_COUNT) {
            smoke_confirm++;
        }
        smoke_clear_confirm = 0;
    } else {
        if (smoke_clear_confirm < SMOKE_CLEAR_CONFIRM) {
            smoke_clear_confirm++;
        } else {
            smoke_confirm = 0;
            smoke_clear_confirm = 0;
        }
    }
    smoke_stable = (smoke_confirm >= SMOKE_CONFIRM_COUNT);
    
    if (smoke_stable) {
        if (!alarm_active) {
            alarm_active = 1;
            data->alarm_type = ALARM_SMOKE;
        }
        return data->alarm_type;
    }
    
    if (data->human_absent) {
        if (human_absent_start_ms == 0) {
            human_absent_start_ms = now_ms;
        } else if (!human_absent_stable && (now_ms - human_absent_start_ms >= HUMAN_ABSENT_TIME_MS)) {
            human_absent_stable = 1;
        }
    } else {
        human_absent_start_ms = 0;
        human_absent_stable = 0;
    }
    
    if (data->max_temp >= TEMP_THRESHOLD_NO_PERSON) {
        if (high_temp_confirm < HIGH_TEMP_CONFIRM) {
            high_temp_confirm++;
        }
    } else {
        high_temp_confirm = 0;
    }
    high_temp_stable = (high_temp_confirm >= HIGH_TEMP_CONFIRM);
    
    if (human_absent_stable && high_temp_stable) {
        if (!alarm_active) {
            alarm_active = 1;
            data->alarm_type = ALARM_NO_PERSON_HIGH_TEMP;
        }
        return data->alarm_type;
    }
    
    if (alarm_active) {
        alarm_active = 0;
        data->alarm_type = ALARM_NONE;
    }
    
    return ALARM_NONE;
}

void Debug_PrintData(SensorData *data, UART_HandleTypeDef *huart)
{
    char buffer[500];
    int len;
    
    len = snprintf(buffer, sizeof(buffer), 
                   "=== Sensor Data ===\r\n"
                   "Person: %s\r\n"
                   "Temperature: %.2f C\r\n"
                   "Humidity: %.2f %%\r\n"
                   "Smoke: %.2f ppm\r\n"
                   "Max IR Temp: %.2f C\r\n"
                   "Battery: %.2f V\r\n"
                   "Alarm: %d\r\n",
                   data->person_present,
                   data->temperature,
                   data->humidity,
                   data->smoke_level,
                   data->max_temp,
                   data->battery_voltage,
                   data->alarm_type);
    
    HAL_UART_Transmit(huart, (uint8_t*)buffer, len, 1000);
}

void FireAlarm_Reset(void)
{
    human_absent_start_ms = 0;
    human_absent_stable = 0;
    high_temp_confirm = 0;
    smoke_confirm = 0;
    smoke_clear_confirm = 0;
    alarm_active = 0;
}
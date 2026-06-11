#ifndef __FUNCTION_H
#define __FUNCTION_H

#include "stm32f1xx_hal.h"
#include "sht30.h"
#include "AMG8833.h"
#include "bsp_mq2.h"
#include "LD2412.h"

typedef struct {
    SHT30_Data sht30;
    AMG8833_Data amg8833;
    MQ2_Data mq2;
    LD2412_Data ld2412;
    float battery_voltage;
    uint8_t alarm_type;
    uint8_t human_absent;
    char person_present[6];
    float smoke_level;
    float temperature;
    float humidity;
    float max_temp;
} SensorData;

#define ALARM_NONE 0
#define ALARM_NO_PERSON_HIGH_TEMP 1
#define ALARM_CRITICAL_TEMP 2
#define ALARM_SMOKE 3

#define TEMP_THRESHOLD_NO_PERSON 45.0f
#define TEMP_THRESHOLD_CRITICAL 60.0f
#define SMOKE_THRESHOLD 150.0f
#define STABLE_COUNT_THRESHOLD 3

#define HUMAN_ABSENT_TIME_MS (5 * 60 * 1000)
#define HIGH_TEMP_CONFIRM 3
#define SMOKE_CONFIRM_COUNT 3
#define SMOKE_CLEAR_CONFIRM 10

void SensorData_Init(SensorData *data);
uint8_t SensorData_CollectAll(SensorData *data, I2C_HandleTypeDef *hi2c, ADC_HandleTypeDef *hadc, UART_HandleTypeDef *huart_ld2412);
uint8_t FireAlarm_Detect(SensorData *data, uint32_t now_ms);
void Debug_PrintData(SensorData *data, UART_HandleTypeDef *huart);
void FireAlarm_Reset(void);

#endif
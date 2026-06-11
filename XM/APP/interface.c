#include "interface.h"
#include "sht30.h"
#include "AMG8833.h"
#include "bsp_mq2.h"
#include "LD2412.h"
#include "bsp_esp8266.h"
#include "bsp_beep.h"
#include "lcd1602.h"

#define DEBUG_INTERVAL 3000
#define UPLOAD_INTERVAL_DEBUG 1500
#define UPLOAD_INTERVAL_FAST 260
#define SENSOR_INTERVAL_MS 200

void System_Init(I2C_HandleTypeDef *hi2c, ADC_HandleTypeDef *hadc, UART_HandleTypeDef *huart_ld2412)
{
    SHT30_Init(hi2c);
    AMG8833_Init(hi2c);
    MQ2_Init(hadc);
    LD2412_Init(huart_ld2412);
    BEEP_Init();
    LCD1602_Init();
    
    LCD1602_SetCursor(0, 0);
    LCD1602_PrintString("Fire Warning Sys");
    LCD1602_SetCursor(1, 0);
    LCD1602_PrintString("Initializing...");
    HAL_Delay(1000);
}

void System_MainLoop(SensorData *data, I2C_HandleTypeDef *hi2c, ADC_HandleTypeDef *hadc, 
                     UART_HandleTypeDef *huart_debug, UART_HandleTypeDef *huart_ld2412, UART_HandleTypeDef *huart_esp8266)
{
    static uint32_t last_debug_time = 0;
    static uint32_t last_upload_time = 0;
    static uint32_t last_sensor_time = 0;
    static uint32_t last_lcd_time = 0;
    static uint8_t upload_mode = 0;
    
    uint32_t now = HAL_GetTick();
    
    if (now - last_sensor_time >= SENSOR_INTERVAL_MS) {
        last_sensor_time = now;
        
        SensorData_CollectAll(data, hi2c, hadc, huart_ld2412);
        
        uint8_t alarm = FireAlarm_Detect(data, now);
        
        if (alarm != ALARM_NONE) {
            BEEP_On();
            upload_mode = 1;
        } else {
            BEEP_Off();
            upload_mode = 0;
        }
    }
    
    if ((now - last_debug_time) >= DEBUG_INTERVAL) {
        last_debug_time = now;
        Debug_PrintData(data, huart_debug);
    }
    
    if ((now - last_lcd_time) >= 500) {
        last_lcd_time = now;
        LCD1602_DisplaySensorData(data->temperature, data->humidity, 
                                  data->smoke_level, data->max_temp, data->alarm_type);
    }
    
    uint32_t upload_interval = upload_mode ? UPLOAD_INTERVAL_FAST : UPLOAD_INTERVAL_DEBUG;
    
    if ((now - last_upload_time) >= upload_interval) {
        last_upload_time = now;
        ESP8266_UploadData(huart_esp8266, 
                           data->temperature, 
                           data->humidity,
                           data->smoke_level,
                           data->max_temp,
                           !data->human_absent,
                           data->alarm_type);
    }
}
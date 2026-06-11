#ifndef __BSP_ESP8266_H
#define __BSP_ESP8266_H

#include "stm32f1xx_hal.h"

#define ESP8266_BUFFER_SIZE 256
#define ESP8266_TIMEOUT 3000

typedef enum {
    ESP8266_OK = 0,
    ESP8266_TIMEOUT_ERROR,
    ESP8266_RESPONSE_ERROR
} ESP8266_Status;

uint8_t ESP8266_Init(UART_HandleTypeDef *huart);
uint8_t ESP8266_SendData(UART_HandleTypeDef *huart, const char *data, uint16_t len);
uint8_t ESP8266_WaitResponse(UART_HandleTypeDef *huart, const char *expected, uint32_t timeout);
uint8_t ESP8266_UploadData(UART_HandleTypeDef *huart, float temp, float humi, float smoke, float max_temp, uint8_t has_person, uint8_t alarm_type);
uint8_t ESP8266_EnterTransparentMode(UART_HandleTypeDef *huart);
uint8_t ESP8266_TransparentSend(UART_HandleTypeDef *huart, const char *data);
uint8_t ESP8266_ConnectTcpServer(UART_HandleTypeDef *huart, const char *ip, uint16_t port);

#endif
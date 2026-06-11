#include "bsp_esp8266.h"
#include <string.h>
#include <stdio.h>

char esp8266_buffer[ESP8266_BUFFER_SIZE];
uint8_t esp8266_index = 0;
uint8_t esp8266_transparent_mode = 0;

uint8_t ESP8266_SendData(UART_HandleTypeDef *huart, const char *data, uint16_t len)
{
    return HAL_UART_Transmit(huart, (uint8_t*)data, len, 1000);
}

uint8_t ESP8266_WaitResponse(UART_HandleTypeDef *huart, const char *expected, uint32_t timeout)
{
    uint32_t start = HAL_GetTick();
    esp8266_index = 0;
    memset(esp8266_buffer, 0, ESP8266_BUFFER_SIZE);
    
    while ((HAL_GetTick() - start) < timeout) {
        HAL_UART_Receive(huart, (uint8_t*)&esp8266_buffer[esp8266_index], 1, 10);
        
        if (strstr(esp8266_buffer, expected) != NULL) {
            return ESP8266_OK;
        }
        
        esp8266_index++;
        if (esp8266_index >= ESP8266_BUFFER_SIZE) {
            esp8266_index = 0;
            memset(esp8266_buffer, 0, ESP8266_BUFFER_SIZE);
        }
    }
    
    return ESP8266_TIMEOUT_ERROR;
}

uint8_t ESP8266_Init(UART_HandleTypeDef *huart)
{
    HAL_Delay(1000);
    
    ESP8266_SendData(huart, "AT\r\n", 4);
    if (ESP8266_WaitResponse(huart, "OK", 2000) != ESP8266_OK)
        return 1;
    
    ESP8266_SendData(huart, "AT+CWMODE=1\r\n", 12);
    if (ESP8266_WaitResponse(huart, "OK", 2000) != ESP8266_OK)
        return 1;
    
    ESP8266_SendData(huart, "AT+CWJAP=\"WiFiSSID\",\"password\"\r\n", 32);
    if (ESP8266_WaitResponse(huart, "OK", 5000) != ESP8266_OK)
        return 1;
    
    ESP8266_SendData(huart, "AT+CIPMUX=0\r\n", 11);
    if (ESP8266_WaitResponse(huart, "OK", 2000) != ESP8266_OK)
        return 1;
    
    return 0;
}

uint8_t ESP8266_ConnectTcpServer(UART_HandleTypeDef *huart, const char *ip, uint16_t port)
{
    char cmd[50];
    snprintf(cmd, sizeof(cmd), "AT+CIPSTART=\"TCP\",\"%s\",%d\r\n", ip, port);
    ESP8266_SendData(huart, cmd, strlen(cmd));
    return ESP8266_WaitResponse(huart, "CONNECT", 3000);
}

uint8_t ESP8266_EnterTransparentMode(UART_HandleTypeDef *huart)
{
    ESP8266_SendData(huart, "AT+CIPMODE=1\r\n", 14);
    if (ESP8266_WaitResponse(huart, "OK", 2000) != ESP8266_OK)
        return 1;
    
    esp8266_transparent_mode = 1;
    return 0;
}

uint8_t ESP8266_TransparentSend(UART_HandleTypeDef *huart, const char *data)
{
    uint16_t len = strlen(data);
    return HAL_UART_Transmit(huart, (uint8_t*)data, len, 1000);
}

uint8_t ESP8266_UploadData(UART_HandleTypeDef *huart, float temp, float humi, float smoke, float max_temp, uint8_t has_person, uint8_t alarm_type)
{
    if (esp8266_transparent_mode) {
        char json_buffer[200];
        snprintf(json_buffer, sizeof(json_buffer), 
                 "{\"type\":\"sensor_data\",\"temperature\":%.2f,\"humidity\":%.2f,"
                 "\"smoke_level\":%.2f,\"max_temp\":%.2f,\"human_detected\":%s,"
                 "\"fire_risk\":%d}\r\n",
                 temp, humi, smoke, max_temp, has_person ? "true" : "false", alarm_type);
        return ESP8266_TransparentSend(huart, json_buffer);
    }
    
    char json_buffer[200];
    snprintf(json_buffer, sizeof(json_buffer), 
             "{\"temp\":%.1f,\"humi\":%.1f,\"smoke\":%.1f,\"max_temp\":%.1f,\"has_person\":%d,\"alarm\":%d}",
             temp, humi, smoke, max_temp, has_person, alarm_type);
    
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "AT+CIPSTART=\"TCP\",\"192.168.1.100\",8080\r\n");
    ESP8266_SendData(huart, cmd, strlen(cmd));
    if (ESP8266_WaitResponse(huart, "CONNECT", 3000) != ESP8266_OK)
        return 1;
    
    snprintf(cmd, sizeof(cmd), "GET /upload?data=%s HTTP/1.1\r\nHost: 192.168.1.100\r\n\r\n", json_buffer);
    ESP8266_SendData(huart, cmd, strlen(cmd));
    if (ESP8266_WaitResponse(huart, "200 OK", 3000) != ESP8266_OK)
        return 1;
    
    ESP8266_SendData(huart, "AT+CIPCLOSE\r\n", 12);
    ESP8266_WaitResponse(huart, "OK", 2000);
    
    return 0;
}
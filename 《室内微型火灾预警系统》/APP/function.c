#include "function.h"
#include "usart.h"
#include "main.h"
#include "motor.h"

void uart_debug(void)
{


	if (sensor_data.alarm_type == 2)
        printf("火灾风险：超高！！！\r\n");
	else if (sensor_data.alarm_type == 1)
        printf("火灾风险：高！\n");
	else if (sensor_data.alarm_type == 0)
        printf("火灾风险：低\n");
    printf("电池电量：%.1f%%\r\n", sensor_data.battery_voltage);
	if (sensor_data.alarm_type == 2)
        printf("警告：环境内取暖设备温度过高！\n");
	else if (sensor_data.alarm_type == 1)
        printf("警告：取暖设备未断电，无人状态已持续5分钟！\n");
	else if (sensor_data.alarm_type == 3)
        printf("警告：检测到烟雾浓度超标！\n");
	else if (sensor_data.alarm_type == 1)
        printf("警告：取暖设备未断电，无人状态已持续5分钟！\n");
	printf("================================\r\n");
}

void car_go(unsigned char time_5ms, unsigned char time_200ms)
{
		time_5ms = 0;
		time_200ms++;
		if (time_200ms >= 40)
		{
				time_200ms = 0;
				LEDToggle(LED_PIN);
		}
		SearchRun();
		if (ctrl_comm_last != ctrl_comm) // 指令发生变化
		{
				ctrl_comm_last = ctrl_comm;
				switch (ctrl_comm)
				{
						case COMM_UP:    CarGo(); break;
						case COMM_DOWN:  CarBack(); break;
						case COMM_LEFT:  CarLeft(); break;
						case COMM_RIGHT: CarRight(); break;
						case COMM_STOP:  CarStop(); break;
						default: break;
				}
				Delayms(10); 
		}
}

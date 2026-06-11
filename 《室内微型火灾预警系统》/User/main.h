#ifndef __MAIN_H_
#define __MAIN_H_
#include "stdint.h"	
#include "interface.h"

extern unsigned int speed_count;
extern char front_right_speed_duty;
extern char behind_left_speed_duty;
extern unsigned char tick_5ms;
extern unsigned char tick_1ms;
extern unsigned char tick_200ms;
extern char ctrl_comm;
extern char ctrl_comm_last;
extern unsigned char continue_time;
extern unsigned char bt_rec_flag;


typedef struct{
    uint16_t adc_value;
    float 			smoke_level;
    float 			temperature;
    float 			humidity;
		uint8_t  		human_absent;
    char 				person_present[6];   // "false" + '\0' 需要5字节，这里给6安全
    uint16_t 		high_temp_detected;
		float 			battery_voltage;
		float 			max_temp;
		uint8_t  		alarm_type;  		// 报警类型0/1/2：无异常/无人未关取暖设备/无论是否有人，温度达到70℃
} SensorData_t;
void SearchRun(void);

extern SensorData_t sensor_data;
extern char json_buffer[1024];

#endif

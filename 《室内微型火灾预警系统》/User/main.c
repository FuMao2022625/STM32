#include "stm32f10x.h"
#include "LCD1602.h"
#include "IRCtrol.h"
#include "motor.h"
#include "usart.h"
#include "LD2412.h"
#include "main.h"
#include "bsp_mq2.h"
#include "SHT30.h"
#include "AMG8833.h"
#include "bsp_esp8266_test.h"
#include "bsp_esp8266.h"
#include "bsp_beep.h" 
#include "function.h"
// 调试模式选择：1 = 慢速模式（3000ms采集，3000ms发送），0 = 快速模式（200ms采集，500ms发送）
#define SLOW_DEBUG_MODE   1   // 改为0即可切换到快速模式
#if SLOW_DEBUG_MODE
    // 慢速模式（3000ms 采集，3000ms 发送）
    #define SENSOR_INTERVAL_MS     200
    #define UPLOAD_INTERVAL_MS     1500

    // 防抖次数：保持 9 秒确认（3000ms * 3 = 9s）
    #define HIGH_TEMP_CONFIRM       3
    #define SMOKE_CONFIRM_COUNT     3
    #define SMOKE_CLEAR_CONFIRM     3
#else
    // 快速模式（200ms 采集，500ms 发送）
    #define SENSOR_INTERVAL_MS     180
    #define UPLOAD_INTERVAL_MS     260

    // 防抖次数：1 秒确认（200ms * 5 = 1s），可根据需要调整
    #define HIGH_TEMP_CONFIRM       5
    #define SMOKE_CONFIRM_COUNT     5
    #define SMOKE_CLEAR_CONFIRM     10
#endif

//全局变量定义
unsigned int speed_count=0;								//占空比计数器 50次一周期
//char front_left_speed_duty=SPEED_DUTY;
char front_right_speed_duty=SPEED_DUTY;
char behind_left_speed_duty=SPEED_DUTY;
//char behind_right_speed_duty=SPEED_DUTY;
unsigned char tick_5ms = 0;								//5ms计数器，作为主函数的基本周期
unsigned char tick_1ms = 0;								//1ms计数器，作为电机的基本计数器
unsigned char tick_200ms = 0;							//刷新显示
char ctrl_comm = COMM_STOP;								//控制指令
char ctrl_comm_last = COMM_STOP;					//上一次的指令
unsigned char continue_time=0;
unsigned char bt_rec_flag=0;							//蓝牙控制标志位

// 无人确认时间（毫秒）
#define HUMAN_ABSENT_TIME_MS   		(5 * 60 * 1000)   // 5分钟 = 300000ms
// 高温检测阈值（°C）
#define HIGH_TEMP_THRESHOLD  			45.0f
// 绝对危险温度阈值（立即报警）
#define CRITICAL_TEMP_THRESHOLD  	60.0f
// 烟雾报警阈值（ppm）
#define SMOKE_ALARM_THRESHOLD      150.0f
uint32_t human_absent_start_ms 	= 0;   		// 开始无人状态的起始时间戳
uint8_t  human_absent_stable	 	= 0;     	// 无人状态是否已稳定（持续5分钟）
uint8_t  high_temp_confirm 			= 0;      // 高温计数器
uint8_t  alarm_active 					= 0;            // 报警标志
uint8_t  smoke_confirm = 0;      					// 烟雾超标计数器
uint8_t  smoke_stable  = 0;      					// 烟雾是否稳定超标
uint8_t smoke_clear_confirm = 0;
char json_buffer[1024];  									//  格式化为json数据的格式
extern volatile uint8_t ucTcpClosedFlag;  // 连接断开标志，由中断或其他地方置位

SensorData_t sensor_data;									// 定义传感器数据结构体

extern __IO uint16_t ADC_ConvertedValue;
extern volatile uint32_t system_ms;   		// 毫秒计数器，每1ms加1

int main(void)
{
		// 报警相关局部变量
    uint8_t  high_temp;
    uint8_t  high_temp_stable;
    uint8_t  i;
	
    // ========== 局部变量==========
    float pixTempture[8][8];
    char buffer[256];
    float ppm = 0.0;
    SHT30_Data temp_humi = {0.0, 0.0};
    uint32_t now;
    uint32_t last_sensor  = 0;
    uint32_t last_upload  = 0;
    uint32_t last_usart  = 0;
    uint32_t last_battery  = 0;
    sensor_data.battery_voltage = 89.0f; 
    sensor_data.alarm_type = 0; 

    // ========== 硬件初始化 ==========
    delay_init();
    GPIOCLKInit();
    UserLEDInit();
    TIM2_Init();
    MotorInit();
    ServoInit();
    uart_init(115200);
    AMG8833_IIC_Init();
    RedRayInit();
    BEEP_GPIO_Config();
    LD2412_Init();
    MQ2_Init();
    SHT30_Init();
    AMG8833_Init();
    ESP8266_Init();
    ESP8266_StaTcpClient_Unvarnish_ConfigTest();
    // ========== 主循环 ==========
    while (1)
    {
        now = system_ms;
        if (now - last_battery >= 60000){last_battery = now;if (sensor_data.battery_voltage > 10.0f){sensor_data.battery_voltage -= 0.5f;}else{sensor_data.battery_voltage = 10.0f;}}
        // -------------------- 1. 传感器采集--------------------
        if (now - last_sensor >= SENSOR_INTERVAL_MS)
        {
            last_sensor = now;

            // 人体存在
            sensor_data.human_absent = (LD2412_Read() == 0); // 1代表无人；0代表有人
            strcpy(sensor_data.person_present, sensor_data.human_absent ? "false" : "true");

            // 温湿度
            temp_humi = SHT30_read_result(0x44);
            sensor_data.humidity = temp_humi.Humidity;
            sensor_data.temperature = temp_humi.Temperature;
            // 可燃气体
            ppm = MQ2_Get_PPM();
            if(ppm>4000) ppm = 4102.13;
            sensor_data.smoke_level = ppm;
            // 热成像矩阵
            amg88xx_readPixels(&pixTempture[0][0], 64);
            format_pix_temp_to_json(pixTempture, json_buffer, sizeof(json_buffer));
            // 计算最高温度
            sensor_data.max_temp = 0;
            for (i = 0; i < 64; i++)
            {
                float t = ((float*)pixTempture)[i];
                if (t > sensor_data.max_temp)
                {
                    sensor_data.max_temp = t;
                }
            }
            high_temp = (sensor_data.max_temp > HIGH_TEMP_THRESHOLD);

            // 无人状态计时
            if (sensor_data.human_absent)
            {
                if (human_absent_start_ms == 0)
                {
                    human_absent_start_ms = now;
                }
                else if (!human_absent_stable && (now - human_absent_start_ms >= HUMAN_ABSENT_TIME_MS))
                {
                    human_absent_stable = 1;
                }
            }
            else
            {
                human_absent_start_ms = 0;
                human_absent_stable = 0;
            }

            // 高温防抖（连续3次）
            if (high_temp)
            {
                if (high_temp_confirm < HIGH_TEMP_CONFIRM)
                {
                    high_temp_confirm++;
                }
            }
            else
            {
                high_temp_confirm = 0;
            }
            high_temp_stable = (high_temp_confirm >= HIGH_TEMP_CONFIRM);
            // 烟雾防抖（连续多次超过阈值才报警，连续多次低于阈值才解除）
            if (ppm > SMOKE_ALARM_THRESHOLD)
            {
                    if (smoke_confirm < SMOKE_CONFIRM_COUNT)
                    {
                        smoke_confirm++;
                    }
                    smoke_clear_confirm = 0;
            }
            else
            {
                    if (smoke_clear_confirm < SMOKE_CLEAR_CONFIRM)
                    {
                        smoke_clear_confirm++;
                    }
                    else
                    {
                        smoke_confirm = 0;
                        smoke_clear_confirm = 0;
                    }
            }
            smoke_stable = (smoke_confirm >= SMOKE_CONFIRM_COUNT);

            // 报警逻辑
            if (sensor_data.max_temp > CRITICAL_TEMP_THRESHOLD)
            {
                if (!alarm_active)
                {
                    alarm_active = 1;
                    sensor_data.alarm_type = 2;
                    BEEP_ON();
                }
            }
            else if (smoke_stable)
            {
                if (!alarm_active)
                {
                    alarm_active = 1;
                    sensor_data.alarm_type = 3;
                    BEEP_ON();
                }
            }
            else
            {
                if (human_absent_stable && high_temp_stable)
                {
                    if (!alarm_active)
                    {
                        alarm_active = 1;
                        sensor_data.alarm_type = 1;
                        BEEP_ON();
                    }
                }
                else
                {
                    if (alarm_active)
                    {
                        alarm_active = 0;
                        sensor_data.alarm_type = 0;
                        BEEP_OFF();
                    }
                }
            }
            
        }
        // -------------------- 2. 串口打印-------------------
        if (now - last_usart >= 3000)
        {
            last_usart = now;
            // 串口调试打印
            printf("\r\n========== 传感器数据 ==========\r\n");
            printf("是否有人：%s\r\n", sensor_data.human_absent ? "无" : "有");
            printf("温度：%.2f °C\r\n", sensor_data.temperature);
            printf("湿度：%.2f %%\r\n", sensor_data.humidity);
            printf("可燃气体浓度：%.2f ppm\r\n", sensor_data.smoke_level);
            printf("红外矩阵：%s\r\n", json_buffer);
            printf("矩阵最大温度值：%.2f\r\n", sensor_data.max_temp);

            uart_debug();
        }
        // -------------------- 3. 数据上传-------------------
        if (now - last_upload >= UPLOAD_INTERVAL_MS)
        {
            last_upload = now;
            // 构造 JSON
            sprintf(buffer,
                    "{\"type\":\"sensor_data\",\"temperature\":%.2f,\"humidity\":%.2f,"
                    "\"smoke_level\":%.2f,\"max_temp\":%s,\"human_detected\":%s,"
                    "\"fire_risk\":%d,\"env_status\":\"%.2f\",\"battery\":%.1f}",
                    sensor_data.temperature, sensor_data.humidity,
                    sensor_data.smoke_level, json_buffer,
                    sensor_data.person_present, sensor_data.alarm_type, sensor_data.max_temp,
                    sensor_data.battery_voltage);
            // WiFi 发送
            ESP8266_TransparentSend(buffer);

        }
        // -------------------- 3. 小车控制--------------------
//        if (tick_5ms >= 5)
//        {
//                car_go(tick_5ms, tick_200ms);
//        }
    }
}
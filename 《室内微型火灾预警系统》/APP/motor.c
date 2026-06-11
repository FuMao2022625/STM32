#include "motor.h"
#include "interface.h"
#include "stm32f10x.h"
#include "main.h"

//GPIO配置函数
void MotorGPIO_Configuration(void)
{		
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = FRONT_LEFT_F_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 	
	GPIO_Init(FRONT_LEFT_F_GPIO, &GPIO_InitStructure);    
	
	GPIO_InitStructure.GPIO_Pin = FRONT_LEFT_B_PIN;	
	GPIO_Init(FRONT_LEFT_B_GPIO, &GPIO_InitStructure); 
	
	GPIO_InitStructure.GPIO_Pin = FRONT_RIGHT_F_PIN;	
	GPIO_Init(FRONT_RIGHT_F_GPIO, &GPIO_InitStructure); 
	
	GPIO_InitStructure.GPIO_Pin = FRONT_RIGHT_B_PIN;	
	GPIO_Init(FRONT_RIGHT_B_GPIO, &GPIO_InitStructure); 
	
	GPIO_InitStructure.GPIO_Pin = BEHIND_LEFT_F_PIN;	
	GPIO_Init(BEHIND_LEFT_F_GPIO, &GPIO_InitStructure);  
	
	GPIO_InitStructure.GPIO_Pin = BEHIND_LEFT_B_PIN;	
	GPIO_Init(BEHIND_LEFT_B_GPIO, &GPIO_InitStructure);  
	
	GPIO_InitStructure.GPIO_Pin = BEHIND_RIGHT_F_PIN;	
	GPIO_Init(BEHIND_RIGHT_F_GPIO, &GPIO_InitStructure);  
	
	GPIO_InitStructure.GPIO_Pin = BEHIND_RIGHT_B_PIN;	
	GPIO_Init(BEHIND_RIGHT_B_GPIO, &GPIO_InitStructure);  
	
}

//根据占空比驱动电机转动
void CarMove(void)
{   
	
	 BEHIND_RIGHT_EN;
	
		//右轮
	if(front_right_speed_duty > 0)//向前
	{
		if(speed_count < front_right_speed_duty)
		{
			FRONT_RIGHT_GO;
		}else                //停止
		{
			FRONT_RIGHT_STOP;
		}
	}
	else if(front_right_speed_duty < 0)//向后
	{
		if(speed_count < (-1)*front_right_speed_duty)
		{
			FRONT_RIGHT_BACK;
		}else                //停止
		{
			FRONT_RIGHT_STOP;
		}
	}
	else                //停止
	{
		FRONT_RIGHT_STOP;
	}
	
	//左轮
	if(behind_left_speed_duty > 0)//向前
	{
		if(speed_count < behind_left_speed_duty)
		{
			BEHIND_LEFT_GO;
		}	else                //停止
		{
			BEHIND_LEFT_STOP;
		}
	}
	else if(behind_left_speed_duty < 0)//向后
	{
		if(speed_count < (-1)*behind_left_speed_duty)
		{
			BEHIND_LEFT_BACK;
		}	else                //停止
		{
			BEHIND_LEFT_STOP;
		}
	}
	else                //停止
	{
		BEHIND_LEFT_STOP;
	}
	
}

//向前
void CarGo(void)
{
	front_right_speed_duty=15;  //右边前进，改变电机PWM值可实现速度调节
	behind_left_speed_duty=15; //左边前进，改变电机PWM值可实现速度调节
}

//后退
void CarBack(void)
{
	front_right_speed_duty=-SPEED_DUTY; //右边后退，全速
	behind_left_speed_duty=-SPEED_DUTY; //左边后退，全速
//	front_right_speed_duty=-10; //右边后退，全速
//	behind_left_speed_duty=-10; //左边后退，全速
}

//向左
void CarLeft(void)
{
	front_right_speed_duty=10;   //右边前进，实现左转
	behind_left_speed_duty=-20;  //左边后退
}

//向右
void CarRight(void)
{
	front_right_speed_duty=-20; //右边后退
	behind_left_speed_duty=10;//左前进，实现右转
}

//停止
void CarStop(void)
{
	front_right_speed_duty=0;
	behind_left_speed_duty=0;
}

void MotorInit(void)
{
	MotorGPIO_Configuration();
	CarStop();
}

//循迹
void SearchRun(void)
{
	//三路都检测到 //黑线
	if(SEARCH_M_IO == BLACK_AREA && SEARCH_L_IO == BLACK_AREA && SEARCH_R_IO == BLACK_AREA)
	{
		ctrl_comm = COMM_UP;
		return;
	}
	
	if(SEARCH_R_IO == BLACK_AREA)//右边黑线，说明小车偏左，需要调用右转函数
	{
		ctrl_comm = COMM_RIGHT;  //小车右转
	}
	else if(SEARCH_L_IO == BLACK_AREA)//左边黑线，说明小车偏右，需要调用左转函数
	{
		ctrl_comm = COMM_LEFT;
	}
	else if(SEARCH_M_IO == BLACK_AREA)//中间黑线，调用小车前进函数
	{
		ctrl_comm = COMM_UP;
	}
}

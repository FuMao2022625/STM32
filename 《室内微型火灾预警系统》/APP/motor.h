#ifndef __MOTOR_H_
#define __MOTOR_H_

extern unsigned int speed_count;//占空比计数器 50次一周期
extern char front_left_speed_duty;
extern char front_right_speed_duty;
extern char behind_left_speed_duty;
extern char behind_right_speed_duty;

void CarMove(void);
void CarGo(void);
void CarBack(void);
void CarLeft(void);
void CarRight(void);
void CarStop(void);
void MotorInit(void);
void SearchRun(void);
#endif


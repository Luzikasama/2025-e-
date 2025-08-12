#ifndef __TRACKING_CAR_H
#define __TRACKING_CAR_H

#include "motor.h"
#include "stdbool.h"

#define RAMP_START_DIS 8.0f
#define WAIT_TO_TURN_DIS 5.0f
extern motor_pid_t tracking_pid;
extern uint8_t circle_set;
extern bool car_start_flag;
extern bool car_start_flag_1;
extern float g_car_speed;
extern bool turn_flag;
extern bool loop_flag;
void tracking_car_init(void);
void speed_task(void);
void car_circle_task(void);
void car_circle_task_1(void);
void motor_turn_task(void);
void master_control_task(void);
#endif
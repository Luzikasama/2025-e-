#ifndef PTZ_H
#define PTZ_H

#include <stdint.h>
#include "stepper.h"
#include "stdbool.h"
#include "motor.h"
#include "pid.h"
#define VISUAL_STABLE_ERROR_THRESHOLD 3
extern StepperMotor stepper_x;
extern StepperMotor stepper_y;

extern float angle_x;
extern float angle_y;
extern uint16_t point_x;
extern uint16_t point_y;
extern pid_t s_visual_pid;
extern motor_pid_t visal_pid;
extern bool ptz_start_flag;
extern bool ptz_sweep_flag;
extern uint8_t s_next_start_direction;
void ptz_manager_init(void);
void ptz_scan_task(void);
void pid_complete_corounite(void);
void ptz_ctrl_task(void);
#endif // PTZ_H
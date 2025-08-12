#ifndef __MOTOR_H_
#define __MOTOR_H_
#include "stdlib.h"
#include "ti_msp_dl_config.h"
#include "uart.h"
#include <math.h>

#define MOTOR_SPEED_RERATIO 28    
#define PULSE_PRE_ROUND 13 
#define ENCODER_MULTIPLE 1.0  
#define PULSE_PER_CYCLE  (MOTOR_SPEED_RERATIO*PULSE_PRE_ROUND*ENCODER_MULTIPLE)//364
#define RADIUS_OF_TYRE 3.3 
#define LINE_SPEED_C  RADIUS_OF_TYRE * 2 * 3.14  
#define SPEED_RECORD_NUM 20
#define WHEEL_DIAMETER_MM   65.0f   // 您小车的轮子直径 (毫米)
#define WHEELBASE_M         0.12f   // 您小车左右驱动轮之间的距离 (轮距)，单位：米

typedef struct {
int32_t lastcount;
float speed;
uint8_t dierct;
int32_t   countnum;
int32_t distance_start_ticks;
float speed_Record[20];
}  encoder_t;


typedef  struct{
float kp,ki,kd;
float err,last_err;
float integral,max_integral;
float output,maxoutput;
float dead_zone;
float Ts;
} motor_pid_t ;



typedef struct {
float speed;   //rpm
float pos ;   //target position   cm
int32_t pos_pulse;//
float pwmDuty;
uint8_t dir;
uint8_t dirLogic;
uint8_t timer_index;
GPIO_Regs *forward_GPIO_PORT;
GPIO_Regs *reverse_GPIO_PORT;
int32_t forward_GPIO_PIN;
int32_t reverse_GPIO_PIN;
encoder_t encoder;
motor_pid_t pos_pid;
motor_pid_t speed_pid;

} motor_t;

extern motor_t motor_l;
extern motor_t motor_r;
/*motor part*/
void motor_Init(void);
void Motor_Set_Duty(motor_t *motor);
void Motor_Set_Speed(motor_t *motor);
void Motor_Set_Speed_Mps(motor_t *motor, float mps);
void Motor_Set_Position(motor_t *motor);
void Motor_Stop(motor_t *motor);
void Motor_Update_Speed(motor_t *motor);
/*encoder part*/
float Speed_Low_Filter(float new_Spe,encoder_t *encoder);
/*pid part*/
float PID_Realize(motor_pid_t* pid,float target,float feedback);
void Odometer_Reset(motor_t* motor) ;
float Odometer_GetDistanceCm(motor_t* motor) ;
void Odometer_SetDistanceCm(motor_t* motor, float distance_cm);
float Kinematics_GetRotationSpeed(void);
#endif
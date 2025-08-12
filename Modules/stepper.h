#ifndef __STEPPER_MOTOR_H__
#define __STEPPER_MOTOR_H__


#include "ti/devices/msp/peripherals/hw_gptimer.h"
#include "ti/driverlib/dl_gpio.h"
#include "ti_msp_dl_config.h"
#include <stdint.h>
#include <math.h>

#define DEFAULT_STEP_ANGLE      1.8f    // 电机步距角，单位：度
#define DEFAULT_MICROSTEPS      64      // 细分数
#define TIM_RCC 40000000
#define TIM_PSC 0
#define TIM_ARR 10000-1
#define TIM_FREQ TIM_RCC/(TIM_PSC+1)/(TIM_ARR+1)
#define DOUBLE_EDGE 0

typedef enum {
    STEPPER_DIR_CW = 0,  // 顺时针
    STEPPER_DIR_CCW       // 逆时针
} StepperDir;

typedef struct {
    GPIO_Regs* STP_Port;
    int32_t      STP_Pin;
    GPIO_Regs* DIR_Port;
    int32_t      DIR_Pin;
	GPIO_Regs* EN_Port;    // 使能引脚端口
    int32_t      EN_Pin;      // 使能引脚
    GPTIMER_Regs* htim;

    float        speed_dps;        // 速度，单位：度/秒
    float        target_angle;     // 目标绝对角度（°）
    float        current_angle;    // 当前角度（°），根据总脉冲计算
    StepperDir   direction;        // 当前方向

    int32_t      target_pulses;    // 目标脉冲数（绝对值）
    int32_t      remaining_steps;  // 剩余需要走的步数
    int32_t      total_count;      // 总脉冲数，顺时针加，逆时针减
    uint8_t      running;          // 电机是否运行
    float        max_speed_dps;
	
	float         limit_min_angle;    // 角度下限
    float         limit_max_angle;    // 角度上限
    uint8_t       limits_enabled;     // 是否启用限位功能 (0:禁用, 1:启用)
} StepperMotor;

// 接口函数声明
void Stepper_Init(StepperMotor* motor);
void Stepper_SetDirection(StepperMotor* motor, StepperDir dir);
void Stepper_SetSpeed(StepperMotor* motor, float dps);
void Stepper_SetMaxSpeed(StepperMotor* motor, float max_dps);
void Stepper_Speed_loop(StepperMotor* motor, float speed_dps);
void Stepper_SetAngleLimits(StepperMotor* motor, float min_angle, float max_angle, uint8_t enable);
void Stepper_MoveToAngle(StepperMotor* motor, float angle_deg);
void Stepper_MoveAngle(StepperMotor* motor, float angle_deg);
void Stepper_Stop(StepperMotor* motor);
void Stepper_TimerCallback(StepperMotor* motor);
void process_stepper_command(StepperMotor* motor, const char *cmd);
#endif

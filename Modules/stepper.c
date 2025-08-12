#include "stepper.h"
#include "ti/driverlib/dl_gpio.h"
#include <string.h>
#include <stdlib.h>
#include "log.h"
// 初始化步进电机结构体，启动定时器中断等
void Stepper_Init(StepperMotor* motor)
{
	if (motor==NULL) {
		//log_error("stepper is NULL!\r\n");
		return;
	}
    motor->current_angle = 0;
    motor->total_count = 0;
    motor->remaining_steps = 0;
    motor->running = 0;
    motor->target_angle = 0;
    motor->limit_min_angle = 0;
    motor->limit_max_angle = 0;
    motor->limits_enabled = 0;
    motor->max_speed_dps = 0; 
    DL_Timer_startCounter(motor->htim);
    DL_GPIO_clearPins(motor->STP_Port, motor->STP_Pin);
    Stepper_SetDirection(motor, motor->direction);
  	DL_GPIO_writePins(motor->EN_Port, motor->EN_Pin); 
}

// 设置方向引脚电平及结构体方向状态
void Stepper_SetDirection(StepperMotor* motor, StepperDir dir)
{
    motor->direction = dir;
    if (dir == STEPPER_DIR_CW){DL_GPIO_writePins(motor->DIR_Port, motor->DIR_Pin);}
    else{DL_GPIO_clearPins(motor->DIR_Port, motor->DIR_Pin);}

}

// 设置电机转速（单位度/秒），通过调整定时器ARR实现脉冲频率
void Stepper_SetSpeed(StepperMotor* motor, float dps)
{
		if (motor->htim==NULL) {
		return;
	}
    if(dps <= 0) return;  // 非法速度不处理

    DL_Timer_stopCounter(motor->htim);

    // 计算ARR，根据公式：
    // ARR = TimerClock / (Prescaler+1) / PulseFrequency - 1
    // 脉冲频率 = 转速（度/秒） / 每步角度
    // 细分后每步角度 = DEFAULT_STEP_ANGLE / DEFAULT_MICROSTEPS
#if DOUBLE_EDGE
    float pulses_per_second = dps / (DEFAULT_STEP_ANGLE / DEFAULT_MICROSTEPS);
    uint32_t arr = (uint32_t)(TIM_RCC / (TIM_PSC + 1) / pulses_per_second) - 1;
#else
	    float pulses_per_second = dps / (DEFAULT_STEP_ANGLE / DEFAULT_MICROSTEPS)*2;
    uint32_t arr = (uint32_t)(TIM_RCC / (TIM_PSC + 1) / pulses_per_second) - 1;
#endif

if (arr>60000) {arr=60000;}
//log_debug("%d\r\n",arr);
    DL_Timer_setLoadValue(motor->htim, arr+1);
    DL_Timer_startCounter(motor->htim);

    motor->speed_dps = dps;
}
/**
 * @brief 速度环控制函数 - 使电机以指定速度和方向持续转动
 * @param motor     电机结构体指针
 * @param speed_dps 目标速度 (单位: 度/秒)，应为正数
 */
void Stepper_Speed_loop(StepperMotor* motor, float speed_dps)
{
    if (motor == NULL) {
        return;
    }

    if (fabsf(speed_dps) < 0.01f) {
        Stepper_Stop(motor);
        return;
    }

    StepperDir target_dir = (speed_dps > 0) ? STEPPER_DIR_CW : STEPPER_DIR_CCW;
    float absolute_speed = fabsf(speed_dps);

    // --- 【关键修改】在这里应用速度限制 ---
    // 检查是否设置了限速 (max_speed_dps > 0)
    if (motor->max_speed_dps > 0) {
        // 如果目标速度超过了最大限制
        if (absolute_speed > motor->max_speed_dps) {
            // 将速度强制限制在最大值
            absolute_speed = motor->max_speed_dps;
            // (可选) 可以在这里打印一条警告日志
            // log_warn("Speed limited to max: %.2f dps", absolute_speed);
        }
    }
    // ------------------------------------

    // --- 执行控制 ---
    Stepper_SetDirection(motor, target_dir);
    motor->remaining_steps = -1; // -1 表示无限运行
    motor->running = 1;
    Stepper_SetSpeed(motor, absolute_speed); // 使用被限制过的速度值
}

void Stepper_SetMaxSpeed(StepperMotor* motor, float max_dps) {
    if (!motor) return;
    
    motor->max_speed_dps = fabsf(max_dps); // 确保存储的是一个正数
}
/**
 * @brief 设置步进电机的角度限位
 * @param motor   电机实例指针
 * @param min_angle 角度下限
 * @param max_angle 角度上限
 * @param enable  是否启用限位功能 (1:启用, 0:禁用)
 */
void Stepper_SetAngleLimits(StepperMotor* motor, float min_angle, float max_angle, uint8_t enable) {
    if (!motor) return;
    
    // 确保min < max
    if (min_angle >= max_angle) {
        // 可以增加一个log_error来提示错误
        return;
    }

    motor->limit_min_angle = min_angle;
    motor->limit_max_angle = max_angle;
    motor->limits_enabled = enable;
}

// 绝对角度控制函数
// 将电机转到angle_deg指定的绝对角度位置（0度为起点）
// 支持中途更改目标角度
void Stepper_MoveToAngle(StepperMotor* motor, float angle_deg)
{
			if (motor==NULL) {
		//log_error("MoveToAngle Failed ,stepper is NULL!\r\n");
		return;
	}
    float target_angle_limited = angle_deg;
    if (motor->limits_enabled) {
        if (target_angle_limited < motor->limit_min_angle) {
            target_angle_limited = motor->limit_min_angle;
             //log_warn("Target angle below min limit. Clamped to %.2f", target_angle_limited);
        } 
        else if (target_angle_limited > motor->limit_max_angle) {
            target_angle_limited = motor->limit_max_angle;
             //log_warn("Target angle above max limit. Clamped to %.2f", target_angle_limited);
        }
    }
    float delta_angle = target_angle_limited - motor->current_angle;
    if (fabsf(delta_angle) < (DEFAULT_STEP_ANGLE / DEFAULT_MICROSTEPS)) {
        Stepper_Stop(motor);
        return;
    }

    if (delta_angle > 0) {
        Stepper_SetDirection(motor, STEPPER_DIR_CW); // 正向转动
    } else {
        Stepper_SetDirection(motor,STEPPER_DIR_CCW); // 反向转动
        delta_angle = -delta_angle; // 将差值转为正数，用于计算步数
    }

    #if DOUBLE_EDGE
        motor->remaining_steps = (int32_t)(delta_angle / (DEFAULT_STEP_ANGLE / DEFAULT_MICROSTEPS));
    #else
        motor->remaining_steps = (int32_t)(delta_angle / (DEFAULT_STEP_ANGLE / DEFAULT_MICROSTEPS) * 2);
    #endif
    motor->target_angle = target_angle_limited; // 保存最终要到达的目标
    motor->running = 1;                         // 标记为运行状态
}

// 相对角度控制函数
// 在当前角度基础上转动angle_deg度
void Stepper_MoveAngle(StepperMotor* motor, float angle_deg)
{
    float new_target = motor->current_angle + angle_deg;
    Stepper_MoveToAngle(motor, new_target);
}

// 停止电机运行
void Stepper_Stop(StepperMotor* motor)
{
    motor->running = 0;
    motor->remaining_steps = 0;
}

// 定时器中断回调，产生步进脉冲
void Stepper_TimerCallback(StepperMotor* motor)
{
    if (!motor->running) return;

    DL_GPIO_togglePins(motor->STP_Port, motor->STP_Pin); // 输出脉冲

#if DOUBLE_EDGE
    static uint8_t toggle_state = 0;
    toggle_state = !toggle_state;
    if (!toggle_state) return; // 只处理一个边缘
#endif

    // 更新角度和总步数（这部分逻辑对于两种模式是通用的）
    if (motor->direction == STEPPER_DIR_CW) {
        motor->total_count++;
#if DOUBLE_EDGE
        motor->current_angle += DEFAULT_STEP_ANGLE / DEFAULT_MICROSTEPS;
#else
        motor->current_angle += DEFAULT_STEP_ANGLE / DEFAULT_MICROSTEPS / 2;
#endif
    } else {
        motor->total_count--;
#if DOUBLE_EDGE
        motor->current_angle -= DEFAULT_STEP_ANGLE / DEFAULT_MICROSTEPS;
#else
        motor->current_angle -= DEFAULT_STEP_ANGLE / DEFAULT_MICROSTEPS / 2;
#endif
    }
 // --- 2. 【新增】检查角度是否达到限位 ---
    if (motor->limits_enabled) {
        // a. 如果是顺时针(CW)运动，检查是否超过上限
        if (motor->direction == STEPPER_DIR_CW && motor->current_angle >= motor->limit_max_angle) {
            //log_warn("Angle limit reached at max: %.2f deg. Stopping.", motor->limit_max_angle);
            Stepper_Stop(motor);
            return; // 立即停止并退出
        }
        // b. 如果是逆时针(CCW)运动，检查是否低于下限
        else if (motor->direction == STEPPER_DIR_CCW && motor->current_angle <= motor->limit_min_angle) {
           // log_warn("Angle limit reached at min: %.2f deg. Stopping.", motor->limit_min_angle);
            Stepper_Stop(motor);
            return; // 立即停止并退出
        }
    }
    // ================== 【模式判断逻辑】 ==================
    // 只有在位置模式下 (remaining_steps > 0)，才需要减小步数并判断停止
    if (motor->remaining_steps > 0) {
        motor->remaining_steps--;

        if (motor->remaining_steps <= 0) {
            // 已到达目标位置，停止电机
            Stepper_Stop(motor); 
        }
    }
    // 如果是速度模式 (remaining_steps = -1)，则跳过上面的判断，电机将持续运行
    // ====================================================
}


#include "motor.h"
#include "ti_msp_dl_config.h"

 motor_t motor_l;
 motor_t motor_r;


void motor_Init(){
DL_TimerG_startCounter(PWM_0_INST);
motor_r.dirLogic=0;
motor_r.pwmDuty=0;
motor_r.speed=0;
motor_r.timer_index=DL_TIMER_CC_0_INDEX;
motor_r.forward_GPIO_PORT=GPIO_MOTOR_AIN2_PORT;
motor_r.reverse_GPIO_PORT=GPIO_MOTOR_AIN1_PORT;
motor_r.forward_GPIO_PIN=GPIO_MOTOR_AIN2_PIN;
motor_r.reverse_GPIO_PIN=GPIO_MOTOR_AIN1_PIN;
motor_r.speed_pid.kp=2.4;
motor_r.speed_pid.ki=0.18;
motor_r.speed_pid.kd=0.3;
motor_r.speed_pid.max_integral=1000;
motor_r.speed_pid.maxoutput=800;
motor_r.speed_pid.dead_zone=0.3;
/*motor_r init*/
motor_l.dirLogic=1;
motor_l.pwmDuty=0;
motor_l.speed=0;
motor_l.timer_index=DL_TIMER_CC_1_INDEX;
motor_l.forward_GPIO_PORT=GPIO_MOTOR_BIN1_PORT;
motor_l.reverse_GPIO_PORT=GPIO_MOTOR_BIN2_PORT;
motor_l.forward_GPIO_PIN=GPIO_MOTOR_BIN1_PIN;
motor_l.reverse_GPIO_PIN=GPIO_MOTOR_BIN2_PIN;
motor_l.speed_pid.kp=2.7;
motor_l.speed_pid.ki=0.15;
motor_l.speed_pid.kd=0.1;
motor_l.speed_pid.max_integral=1000;
motor_l.speed_pid.maxoutput=800;
motor_l.speed_pid.dead_zone=0.3;
}
/*motor part*/
void Motor_Set_Duty(motor_t *motor){
    if (motor->pwmDuty>=0){
DL_TimerG_setCaptureCompareValue(PWM_0_INST,motor->pwmDuty,motor->timer_index);
DL_GPIO_setPins(motor->forward_GPIO_PORT, motor->forward_GPIO_PIN);
DL_GPIO_clearPins(motor->reverse_GPIO_PORT, motor->reverse_GPIO_PIN);
    }
    else if(motor->pwmDuty<0){
DL_TimerG_setCaptureCompareValue(PWM_0_INST,-motor->pwmDuty,motor->timer_index);
DL_GPIO_setPins(motor->reverse_GPIO_PORT, motor->reverse_GPIO_PIN);
DL_GPIO_clearPins(motor->forward_GPIO_PORT, motor->forward_GPIO_PIN);

    }
}

void Motor_Set_Speed(motor_t *motor){
Motor_Update_Speed(motor);
motor->pwmDuty=PID_Realize(&motor->speed_pid,motor->speed,motor->encoder.speed);
Motor_Set_Duty(motor);
}

void Motor_Set_Speed_Mps(motor_t *motor, float mps) {
    const float circumference_m = (RADIUS_OF_TYRE / 100.0f) * 3.14f;
    float target_rps = mps / circumference_m;
    float target_rpm = target_rps * 60.0f;
    motor->speed = target_rpm;
}

void Motor_Set_Position(motor_t *motor){
motor->pos_pulse=(int)((motor->pos*PULSE_PER_CYCLE)/(LINE_SPEED_C));
motor->speed=PID_Realize(&motor->pos_pid, motor->pos_pulse, motor->encoder.countnum);
Motor_Set_Speed(motor);
}

void Motor_Stop(motor_t *motor){
    motor->speed = 0;
    motor->speed_pid.integral = 0;
    motor->pwmDuty = 0;
    Motor_Set_Duty(motor);
}

void Motor_Update_Speed(motor_t *motor){
    motor->encoder.speed = (float)(motor->encoder.countnum - motor->encoder.lastcount) * 6000 / PULSE_PER_CYCLE;

    // 2. 对速度值进行低通滤波，防止突变
    motor->encoder.speed = Speed_Low_Filter(motor->encoder.speed, &motor->encoder);

    // 3. 更新“上次脉冲计数值”，为下个周期计算做准备
    motor->encoder.lastcount = motor->encoder.countnum;

}
/*encoder part*/

float Speed_Low_Filter(float new_Spe,encoder_t *encoder)
{
    float sum = 0.0f;
    uint32_t test_Speed = new_Spe;
    for(uint8_t i=SPEED_RECORD_NUM-1;i>0;i--)//
    {
        encoder->speed_Record[i] = encoder->speed_Record[i-1];
        sum += encoder->speed_Record[i-1];
    }
    encoder->speed_Record[0] = new_Spe;//
    sum += new_Spe;
    test_Speed = sum/SPEED_RECORD_NUM;
    return sum/SPEED_RECORD_NUM;//
}

void TIMA0_IRQHandler(void) {
  switch (DL_TimerA_getPendingInterrupt(ENCODER1A_INST)) {
  case DL_TIMERA_IIDX_CC0_DN:
   motor_r.encoder.dierct = DL_GPIO_readPins(GPIO_ENCODER_PORT, GPIO_ENCODER_ENCODER1B_PIN);//读取IO电平获取电机旋转方向
   motor_r.encoder.countnum=
       motor_r.encoder.dierct? (motor_r.encoder.countnum + 1) : (motor_r.encoder.countnum - 1);//通过判断旋转方向来决定encoder_cnt增加还是减少
    break;
  default:
    break;
  }
}

void TIMG6_IRQHandler(void) {
  switch (DL_TimerG_getPendingInterrupt(ENCODER2A_INST)) {
  case DL_TIMERG_IIDX_CC0_DN:
    motor_l.encoder.dierct= DL_GPIO_readPins(GPIO_ENCODER_PORT, GPIO_ENCODER_ENCODER2B_PIN);//读取IO电平获取电机旋转方向
   motor_l.encoder.countnum =
        motor_l.encoder.dierct ? ( motor_l.encoder.countnum + 1) : ( motor_l.encoder.countnum- 1);//通过判断旋转方向来决定encoder_cnt增加还是减少
    break;
  default:
    break;
  }
}




/*pid part*/

float PID_Realize(motor_pid_t* pid,float target,float feedback)//一次PID计算
{
    pid->err = target - feedback;
    if(pid->err < pid->dead_zone && pid->err > -pid->dead_zone) pid->err = 0;//pid死区
    pid->integral += pid->err;
    
    if(pid->ki * pid->integral < -pid->max_integral) pid->integral = -pid->max_integral / pid->ki;//积分限幅
    else if(pid->ki * pid->integral > pid->max_integral) pid->integral = pid->max_integral / pid->ki;

    if(target == 0) pid->integral = 0; // 刹车时清空i


    pid->output = (pid->kp * pid->err) + (pid->ki * pid->integral) + (pid->kd * (pid->err - pid->last_err));//全量式PID

    //输出限幅
 if(pid->output > pid->maxoutput) pid->output = pid->maxoutput;
 else  if(pid->output < -pid->maxoutput) pid->output = -pid->maxoutput;
    pid->last_err = pid->err;
    return pid->output;
}


/**
 * @brief 复位（或称“清零”）指定电机的里程计。
 * @brief 它通过记录当前的脉冲总数作为新的起点来实现。
 * @param motor 指向要操作的电机结构体的指针
 */
void Odometer_Reset(motor_t* motor) {
    if (motor == NULL) return;
    motor->encoder.distance_start_ticks = motor->encoder.countnum;
}

/**
 * @brief 获取指定电机从上一次复位以来行驶的距离。
 * @param motor 指向要操作的电机结构体的指针
 * @return float 行驶的距离，单位为厘米 (cm)
 */
float Odometer_GetDistanceCm(motor_t* motor) {
    if (motor == NULL) return 0.0f;
    int32_t ticks_since_reset = motor->encoder.countnum - motor->encoder.distance_start_ticks;
    float distance_cm = ((float)ticks_since_reset / PULSE_PER_CYCLE) * LINE_SPEED_C;
    return distance_cm;
}
/**
 * @brief 【新增】将指定电机的里程计设定为一个特定的距离值。
 * @brief 它通过反向计算并设置起点脉冲数来实现。
 * @param motor         指向要操作的电机结构体的指针
 * @param distance_cm   您希望设定的新的里程值 (单位: cm)
 */
void Odometer_SetDistanceCm(motor_t* motor, float distance_cm)
{
    if (motor == NULL) return;

    // 1. 计算1厘米对应的脉冲数
    const float ticks_per_cm = PULSE_PER_CYCLE / 20.724f;

    // 2. 将您期望的目标距离 (cm) 转换为对应的脉冲数
    int32_t target_ticks_offset = (int32_t)(distance_cm * ticks_per_cm);

    // 3. 【核心】反向计算出新的“起点脉冲数”
    //    公式: 新起点 = 当前总脉冲 - 目标距离脉冲
    //    这里的 motor->encoder.countnum 是持续累加的总脉冲数
    motor->encoder.distance_start_ticks = motor->encoder.countnum - target_ticks_offset;

}

static float convert_rpm_to_mps(float rpm) {
    // 1. 计算轮子周长 (米)
    const float circumference_m = (WHEEL_DIAMETER_MM / 1000.0f) * M_PI;
    
    // 2. 将 rpm 转换为 rps (转/秒)
    float rps = rpm / 60.0f;
    
    // 3. 返回线速度
    return rps * circumference_m;
}


/**
 * @brief 【公开API】根据左右轮速度，计算车体自转角速度
 */
float Kinematics_GetRotationSpeed(void) {
    // 1. 获取左右轮的当前转速 (rpm)
    //    我们假设 motor->Current_speed 是由您的100Hz任务实时更新的
    float left_rpm = motor_l.encoder.speed;
    float right_rpm = motor_r.encoder.speed;

    // 2. 将转速(rpm)分别转换为线速度(m/s)
    float left_mps = convert_rpm_to_mps(left_rpm);
    float right_mps = convert_rpm_to_mps(right_rpm);
    
    // 3. 应用车辆运动学公式，计算角速度 (弧度/秒)
    //    角速度 = (右轮速度 - 左轮速度) / 轮距
    //    如果轮距为0，则无法计算，直接返回0
    if (WHEELBASE_M < 0.001f) {
        return 0.0f;
    }
    float angular_velocity_rad_s = (right_mps - left_mps) / WHEELBASE_M;

    // 4. 将单位从 弧度/秒 转换为 度/秒，以便于PID使用
    float angular_velocity_deg_s = angular_velocity_rad_s * (180.0f / M_PI);
    
    return angular_velocity_deg_s;
}
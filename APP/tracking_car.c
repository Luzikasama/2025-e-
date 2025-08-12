#include "tracking_car.h"
#include "motor.h"
#include "log.h"
#include "gray_sensor.h"
#include "scheduler.h"
#include "stepper.h"
#include "uart.h"
#include "ptz.h"

float gray=0;
float dis;
motor_pid_t tracking_pid;
uint16_t speed_cnt;
uint8_t circle=0;
uint8_t circle_set=1;
uint16_t v_err_cnt=0;
int16_t weight;
bool car_start_flag=false;
bool car_start_flag_1=false;
bool turn_flag=false;
bool loop_flag=false;
float angle_adjustment;
float current_turn_speed ;
static float s_lost_target_angle = 0.0f; // 记录丢失目标时的角度
static bool  s_is_searching_to_max = true; // 记录扫描方向
float g_car_speed=0.21;
float n_turn_dis;
uint16_t turn_cnt=0;
void tracking_car_init(void){
motor_Init();
tracking_pid.kp=1.3f;
tracking_pid.ki=0;
tracking_pid.kd=10;
tracking_pid.dead_zone=2;
tracking_pid.Ts=0.01;
tracking_pid.maxoutput=30;
tracking_pid.max_integral=1000;
//Add_SchTask(car_circle_task,100.0f,ENABLE);
//Add_SchTask(car_circle_task_first, 100.0f, ENABLE);
//Add_SchTask(motor_turn_task, 100.0f, ENABLE);
//Add_Corontine(turn_corounite, 1, 1, 0, 0);
Add_SchTask(master_control_task,100.0f, ENABLE);
}
motor_pid_t ramp_pid={

    .kp=0.5,
    .ki=0,
    .kd=0,
    .Ts=0.01,
    .maxoutput=10,
    .max_integral=1000,
};
motor_pid_t visual_pid={

    .kp=3.2f,//2.8
    .ki=0,
    .kd=10,
    .maxoutput=280,
    .max_integral=1000,
    .dead_zone=0,
    .Ts=0.01,
};

typedef enum {
    Car_STATE_IDLE,
    Car_STATE_START,
    Car_STATE_TRACKING,
    Car_STATE_WAIT_TO_TURN,
    Car_STATE_TURNRAMP_START,
    Car_STATE_TURNRAMP_STOP,
    Car_STATE_RAMP_START,
} CarState_t;

typedef enum {
    PTZ_STATE_IDLE,         // 云台空闲
    PTZ_STATE_VISUAL_LOCK,  // 视觉闭环锁定
    PTZ_STATE_SEARCHING     // 丢失目标后，来回扫描
} PtzState_t;
PtzState_t s_ptz_state = PTZ_STATE_IDLE;
CarState_t Carstate;
CarState_t s_car_state=Car_STATE_IDLE;
void car_ctrl_task(void){
static float speed_adjust=0;
gray=Gray_Senor_Get_State();
log_debug("%.2f",gray);
speed_adjust=PID_Realize(&tracking_pid, 0.0f, gray);
motor_l.speed=30;
motor_r.speed=30;
motor_l.speed-=speed_adjust;
motor_r.speed+=speed_adjust;
uartprintf_DMA("%.2f,%.2f\r\n",gray,speed_adjust);
Motor_Set_Speed(&motor_r);
Motor_Set_Speed(&motor_l);

//uartprintf_DMA("%.2f,%.2f\r\n",motor_l.speed,motor_r.speed);

}


void car_circle_task_1(void){

const float MAX_TURN_SPEED_MPS = 0.05f;  // 转弯时的最大轮速
const float MIN_TURN_SPEED_MPS = 0.01f; // 转弯时的最低轮速（防止完全停止）
const float TOTAL_TURN_ARC_CM = 5.4f; // 90度转弯的总弧长 (基于12cm轮距)
const float RAMP_DISTANCE_CM = TOTAL_TURN_ARC_CM / 2.0f; // 前半段加速，后半段减速
static float speed_adjust=0;
float turn_dis=0;
if (!car_start_flag) return;
switch (Carstate){
case Car_STATE_IDLE:
//log_debug("idle");
Motor_Stop(&motor_l);
Motor_Stop(&motor_r);
if (car_start_flag){
Odometer_SetDistanceCm(&motor_l,90.0f);
Odometer_SetDistanceCm(&motor_r,90.0f);
 g_car_speed=0.21;
car_start_flag=false;
Carstate=Car_STATE_START;
}
break;
case Car_STATE_START:
gray=Gray_Senor_Get_State();
 weight=Gray_Senor_Get_Turn();
 dis=Odometer_GetDistanceCm(&motor_l);
if (((Odometer_GetDistanceCm(&motor_r)+Odometer_GetDistanceCm(&motor_l))/2)>=80.0f){
if (weight<-40){
Carstate=Car_STATE_WAIT_TO_TURN;
Odometer_Reset(&motor_r);
Odometer_Reset(&motor_l);
    }
}
speed_adjust=PID_Realize(&tracking_pid, 0.0f, gray);
Motor_Set_Speed_Mps(&motor_l, 0.12);
Motor_Set_Speed_Mps(&motor_r, 0.12);
motor_l.speed-=speed_adjust;
motor_r.speed+=speed_adjust;
break;
case Car_STATE_TRACKING:
gray=Gray_Senor_Get_State();
 weight=Gray_Senor_Get_Turn();
 dis=Odometer_GetDistanceCm(&motor_l);
if (((Odometer_GetDistanceCm(&motor_r)+Odometer_GetDistanceCm(&motor_l))/2)>=80.0f){
if (weight<-40){
    if (circle>=4*circle_set){
        Carstate=Car_STATE_IDLE;
        circle=0;
        car_start_flag=false;
        loop_flag=false;
        Stepper_Stop(&stepper_x);
        DL_GPIO_clearPins(GPIO_RELAY_PORT, GPIO_RELAY_RELAY_PIN);
    }else{
//Run_Corontine(wait_to_turn_coroutine);
Carstate=Car_STATE_WAIT_TO_TURN;
Odometer_Reset(&motor_r);
Odometer_Reset(&motor_l);
    }
}
}
speed_adjust=PID_Realize(&tracking_pid, 0.0f, gray);
Motor_Set_Speed_Mps(&motor_l, g_car_speed);
Motor_Set_Speed_Mps(&motor_r, g_car_speed);
motor_l.speed-=speed_adjust;
motor_r.speed+=speed_adjust;
//uartprintf_DMA("%.2f,%.2f\r\n",gray,speed_adjust);
//log_debug("cir%d",circle);
break;
case Car_STATE_WAIT_TO_TURN:{
//log_debug("wait");
float distance_wait=Odometer_GetDistanceCm(&motor_l);
if (distance_wait>=WAIT_TO_TURN_DIS){Carstate=Car_STATE_TURNRAMP_START;
//Run_Corontine(turn_corounite);
Odometer_Reset(&motor_r);
Odometer_Reset(&motor_l);
}
Motor_Set_Speed_Mps(&motor_l,0.02+(g_car_speed-0.04)*distance_wait/WAIT_TO_TURN_DIS);
Motor_Set_Speed_Mps(&motor_r,0.02+(g_car_speed-0.04)*distance_wait/WAIT_TO_TURN_DIS);}
break;
 case Car_STATE_TURNRAMP_START: {
float turn_distance = fabsf(Odometer_GetDistanceCm(&motor_l));
float progress = turn_distance / RAMP_DISTANCE_CM;
if (progress > 1.0f) progress = 1.0f;
float current_turn_speed = MIN_TURN_SPEED_MPS + (MAX_TURN_SPEED_MPS - MIN_TURN_SPEED_MPS) * progress;
Motor_Set_Speed_Mps(&motor_l, -current_turn_speed);
Motor_Set_Speed_Mps(&motor_r,  current_turn_speed);
if (turn_distance >= RAMP_DISTANCE_CM) {
Carstate = Car_STATE_TURNRAMP_STOP;
}
break;
}
case Car_STATE_TURNRAMP_STOP: {
float turn_distance = fabsf(Odometer_GetDistanceCm(&motor_l));
float progress = (turn_distance - RAMP_DISTANCE_CM) / RAMP_DISTANCE_CM;
if (progress > 1.0f) progress = 1.0f;
if (circle%4==1||circle%4==2){ current_turn_speed = MAX_TURN_SPEED_MPS - (MAX_TURN_SPEED_MPS - 0.008) * progress;
}else {current_turn_speed = MAX_TURN_SPEED_MPS - (MAX_TURN_SPEED_MPS - MIN_TURN_SPEED_MPS) * progress;}
Motor_Set_Speed_Mps(&motor_l, -current_turn_speed);
Motor_Set_Speed_Mps(&motor_r,  current_turn_speed);
if (circle%4==1||circle%4==2){n_turn_dis=TOTAL_TURN_ARC_CM+0.2;}
else {n_turn_dis=TOTAL_TURN_ARC_CM+0.1;}
if (turn_distance >= n_turn_dis) {
Motor_Stop(&motor_l);
Motor_Stop(&motor_r);
Odometer_Reset(&motor_l);
Odometer_Reset(&motor_r);
circle++;
Carstate = Car_STATE_RAMP_START; // 切换到巡线缓启动
    }
// }else {speed_cnt=0;}
}
break;

case Car_STATE_RAMP_START:
gray=Gray_Senor_Get_State();
float distance=Odometer_GetDistanceCm(&motor_l);
if (distance>=RAMP_START_DIS){Carstate=Car_STATE_TRACKING;}
speed_adjust=PID_Realize(&ramp_pid, 0.0f, gray);
//log_debug("%.2f",0.05+g_car_speed*distance/RAMP_START_DIS);
Motor_Set_Speed_Mps(&motor_l,0.02+ g_car_speed*distance/RAMP_START_DIS);
Motor_Set_Speed_Mps(&motor_r, 0.02+g_car_speed*distance/RAMP_START_DIS);
motor_l.speed-=speed_adjust;
motor_r.speed+=speed_adjust;
break;
default:
break;
}
Motor_Set_Speed(&motor_r);
Motor_Set_Speed(&motor_l);

}




void master_control_task(void) {
    if (!loop_flag) return; 
        
        switch (s_ptz_state) {
            case PTZ_STATE_IDLE:
                s_ptz_state = PTZ_STATE_VISUAL_LOCK;
                break;
            case PTZ_STATE_VISUAL_LOCK:
                if (point_x != 0) {
                     angle_adjustment = PID_Realize(&visual_pid, 400.0f, (float)point_x);
                  //  if (s_car_state ){//== Car_STATE_TURNRAMP_START || s_car_state == Car_STATE_TURNRAMP_STOP) {
                        const float K_ff =-3.8f; 
                        float car_rotation_speed = Kinematics_GetRotationSpeed(); 
                        angle_adjustment += K_ff * car_rotation_speed;
                        log_debug("%.2f",angle_adjustment);
                  //  }                    
            float pid_error = fabsf(399.0f - (float)point_x);
            if (pid_error < VISUAL_STABLE_ERROR_THRESHOLD) {
            v_err_cnt++;
            if (v_err_cnt>10){
                DL_GPIO_writePins(GPIO_RELAY_PORT, GPIO_RELAY_RELAY_PIN);
            }
            }else{
                v_err_cnt=0;
            }
                    Stepper_Speed_loop(&stepper_x, angle_adjustment);
                } else {
                  //  log_warn("PTZ: Target lost! Engaging search mode.");
                    s_lost_target_angle = stepper_x.current_angle; // 记录丢失时的角度
                    s_is_searching_to_max = true; // 初始扫描方向
                    s_ptz_state = PTZ_STATE_SEARCHING;
                }
                break;
            case PTZ_STATE_SEARCHING:
                if (point_x != 0) {
                 //   log_info("PTZ: Target re-acquired! Resuming visual lock.");
                    s_ptz_state = PTZ_STATE_VISUAL_LOCK; // 切回视觉闭环
                } else {
                    if (!stepper_x.running) {
                        if (s_is_searching_to_max) {
                            Stepper_MoveToAngle(&stepper_x, s_lost_target_angle - 720.0f);
                            s_is_searching_to_max = false; // 下一次去下限
                        } else {
                            Stepper_MoveToAngle(&stepper_x, s_lost_target_angle + 720.0f);
                            s_is_searching_to_max = true; // 下一次去上限
                        }
                    }
                }
                break;
        }
    

}


void car_circle_task(void){
     const float MAX_TURN_SPEED_MPS = 0.08f;  // 转弯时的最大轮速
    const float MIN_TURN_SPEED_MPS = 0.02f; // 转弯时的最低轮速（防止完全停止）
    const float TOTAL_TURN_ARC_CM = 5.6f; // 90度转弯的总弧长 (基于12cm轮距)
    const float RAMP_DISTANCE_CM = TOTAL_TURN_ARC_CM / 2.0f; // 前半段加速，后半段减速
static float speed_adjust=0;
float turn_dis=0;
switch (Carstate){
case Car_STATE_IDLE:
//log_debug("idle");
Motor_Stop(&motor_l);
Motor_Stop(&motor_r);
if (car_start_flag){
Odometer_SetDistanceCm(&motor_l,90.0f);
Odometer_SetDistanceCm(&motor_r,90.0f);
car_start_flag=false;
Carstate=Car_STATE_TRACKING;
}
break;
case Car_STATE_TRACKING:
gray=Gray_Senor_Get_State();
int16_t weight=Gray_Senor_Get_Turn();
float dis=Odometer_GetDistanceCm(&motor_l);
if (((Odometer_GetDistanceCm(&motor_r)+Odometer_GetDistanceCm(&motor_l))/2)>=80.0f){
if (weight<-40){
    if (circle>=4*circle_set){
        Carstate=Car_STATE_IDLE;
        circle=0;
        car_start_flag=false;
    }else{
//Run_Corontine(wait_to_turn_coroutine);
Carstate=Car_STATE_WAIT_TO_TURN;
Odometer_Reset(&motor_r);
Odometer_Reset(&motor_l);
    }
}
}
speed_adjust=PID_Realize(&tracking_pid, 0.0f, gray);
Motor_Set_Speed_Mps(&motor_l, g_car_speed);
Motor_Set_Speed_Mps(&motor_r, g_car_speed);
motor_l.speed-=speed_adjust;
motor_r.speed+=speed_adjust;
uartprintf_DMA("%.2f,%.2f\r\n",gray,speed_adjust);
//log_debug("cir%d",circle);
break;
case Car_STATE_WAIT_TO_TURN:{
//log_debug("wait");
float distance_wait=Odometer_GetDistanceCm(&motor_l);
if (distance_wait>=WAIT_TO_TURN_DIS){Carstate=Car_STATE_TURNRAMP_START;
//Run_Corontine(turn_corounite);
Odometer_Reset(&motor_r);
Odometer_Reset(&motor_l);
}
Motor_Set_Speed_Mps(&motor_l,0.02+(g_car_speed-0.02)*distance_wait/WAIT_TO_TURN_DIS);
Motor_Set_Speed_Mps(&motor_r,0.02+(g_car_speed-0.02)*distance_wait/WAIT_TO_TURN_DIS);}
break;
 case Car_STATE_TURNRAMP_START: {
float turn_distance = fabsf(Odometer_GetDistanceCm(&motor_l));

// a. 计算加速进度 (0.0 -> 1.0)
float progress = turn_distance / RAMP_DISTANCE_CM;
if (progress > 1.0f) progress = 1.0f;

// b. 速度从最低线性增加到最大
float current_turn_speed = MIN_TURN_SPEED_MPS + (MAX_TURN_SPEED_MPS - MIN_TURN_SPEED_MPS) * progress;

// c. 将计算出的转弯速度应用到电机差速上
Motor_Set_Speed_Mps(&motor_l, -current_turn_speed);
Motor_Set_Speed_Mps(&motor_r,  current_turn_speed);

// d. 检查是否完成缓启动，进入缓刹车阶段
if (turn_distance >= RAMP_DISTANCE_CM) {
Carstate = Car_STATE_TURNRAMP_STOP;
}
break;
}

case Car_STATE_TURNRAMP_STOP: {
float turn_distance = fabsf(Odometer_GetDistanceCm(&motor_l));

// a. 计算刹车进度 (0.0 -> 1.0)
float progress = (turn_distance - RAMP_DISTANCE_CM) / RAMP_DISTANCE_CM;
if (progress > 1.0f) progress = 1.0f;

// b. 速度从最大线性减小到最低
float current_turn_speed = MAX_TURN_SPEED_MPS - (MAX_TURN_SPEED_MPS - MIN_TURN_SPEED_MPS) * progress;

// c. 应用到电机
Motor_Set_Speed_Mps(&motor_l, -current_turn_speed);
Motor_Set_Speed_Mps(&motor_r,  current_turn_speed);

// d. 【关键】检查是否完成整个转弯
if (turn_distance >= TOTAL_TURN_ARC_CM) {
log_info("Smooth turn complete. Ramping up for tracking...");

// 转弯完成，停止电机并准备进入下一个状态
Motor_Stop(&motor_l);
Motor_Stop(&motor_r);

// 复位里程计，为接下来的巡线距离计算做准备
Odometer_Reset(&motor_l);
Odometer_Reset(&motor_r);
circle++;
Carstate = Car_STATE_RAMP_START; // 切换到巡线缓启动
}
break;
}
case Car_STATE_RAMP_START:
gray=Gray_Senor_Get_State();
float distance=Odometer_GetDistanceCm(&motor_l);
if (distance>=RAMP_START_DIS){Carstate=Car_STATE_TRACKING;}
speed_adjust=PID_Realize(&ramp_pid, 0.0f, gray);
//log_debug("%.2f",0.05+g_car_speed*distance/RAMP_START_DIS);
Motor_Set_Speed_Mps(&motor_l,0.02+ g_car_speed*distance/RAMP_START_DIS);
Motor_Set_Speed_Mps(&motor_r, 0.02+g_car_speed*distance/RAMP_START_DIS);
motor_l.speed-=speed_adjust;
motor_r.speed+=speed_adjust;
break;
default:
break;
}
Motor_Set_Speed(&motor_r);
Motor_Set_Speed(&motor_l);
  //  uartprintf_DMA("%.2f,%.2f\r\n",motor_l.encoder.speed,motor_r.encoder.speed);

}


#include "ptz.h"
#include "log.h"
#include "motor.h"
#include "pid.h"
#include "stdbool.h"
#include <string.h> 
#include <stdlib.h>
#include "scheduler.h"
#include "ti/driverlib/dl_gpio.h"
#include "ti_msp_dl_config.h"
#include <math.h>      
#include "stepper.h"
#include "scheduler.h"
#include "uart.h"


#define IMAGE_CENTER_X 400
bool ptz_start_flag;
bool relay_complete=false;
bool ptz_sweep_flag=false;
uint16_t delay_t;
uint8_t s_next_start_direction;
uint8_t s_ptz_state_before_lock;
uint16_t err_cnt;
typedef enum {
    PTZ_STATE_IDLE,          // 0. 空闲状态，任务未激活
    PTZ_STATE_START,         // 1. 任务启动，进行初始化
    PTZ_STATE_SWEEP_CW,      // 2. 顺时针扫描，寻找目标
    PTZ_STATE_SWEEP_CCW,     // 3. 逆时针扫描，寻找目标
    PTZ_STATE_VISUAL_LOCK,   // 4. 视觉闭环，锁定目标
    PTZ_STATE_COMPLETE       // 5. 任务完成，执行最终动作
} ptz_state_t;
static ptz_state_t s_ptz_current_state = PTZ_STATE_IDLE;

StepperMotor stepper_y={
.STP_Port=GPIO_STEPPER_STEPPER_Y_STP_PORT,
.STP_Pin=GPIO_STEPPER_STEPPER_Y_STP_PIN,
.DIR_Port=GPIO_STEPPER_STEPPER_Y_DIR_PORT,
.DIR_Pin=GPIO_STEPPER_STEPPER_Y_DIR_PIN,
.EN_Port=GPIO_STEPPER_STEPPER_Y_EN_PORT,
.EN_Pin=GPIO_STEPPER_STEPPER_Y_EN_PIN,
.htim=TIMER_STEPPER_Y_INST,
};
StepperMotor stepper_x={
.STP_Port=GPIO_STEPPER_STEPPER_X_STP_PORT,
.STP_Pin=GPIO_STEPPER_STEPPER_X_STP_PIN,
.DIR_Port=GPIO_STEPPER_STEPPER_X_DIR_PORT,
.DIR_Pin=GPIO_STEPPER_STEPPER_X_DIR_PIN,
.EN_Port=GPIO_STEPPER_STEPPER_X_EN_PORT,
.EN_Pin=GPIO_STEPPER_STEPPER_X_EN_PIN,
.htim=TIMER_STEPPER_X_INST
};


float angle_x;
float angle_y;
uint16_t point_x;
uint16_t point_y;
pid_t s_visual_pid=
{
 .setPoint=400,
 .proportion=0.8,
 .integral=0,
 .derivative=0,
 .Ts=0.01,

 .maxOutput=50,
 .minOutput=-50,
 .deadBand=8,
 .integMode=0,
 .iModeK1=20,

};

motor_pid_t visal_pid={

    .kp=2.8,
    .ki=0,
    .kd=14,
    .maxoutput=280,
    .max_integral=1000,
    .dead_zone=0,
    .Ts=0.01,
};
uint16_t pid_error;
void ptz_manager_init(void){
		Stepper_Init(&stepper_y);
		Stepper_SetSpeed(&stepper_y,10);
		Stepper_SetDirection(&stepper_y,STEPPER_DIR_CCW);
		Stepper_Init(&stepper_x);
		Stepper_SetDirection(&stepper_x,STEPPER_DIR_CW);
		Stepper_SetSpeed(&stepper_x,420);
		//Stepper_SetAngleLimits(&stepper_x,-2880,2880,1);
        //Stepper_SetAngleLimits(&stepper_y,-20,20,1);
       Add_SchTask(ptz_ctrl_task, 100.0f, ENABLE);
       Add_SchTask(ptz_scan_task, 100.0f, ENABLE);
}

void TIMG7_IRQHandler(void) {
  switch (DL_TimerG_getPendingInterrupt(TIMER_STEPPER_X_INST)) {
  case DL_TIMER_IIDX_ZERO:
  DL_TimerG_clearInterruptStatus(TIMER_STEPPER_X_INST,DL_TIMER_IIDX_ZERO);
Stepper_TimerCallback(&stepper_x);
    break;
  default:
    break;
  }
}

void TIMA1_IRQHandler(void) {
  switch (DL_TimerA_getPendingInterrupt(TIMER_STEPPER_Y_INST)) {
  case DL_TIMER_IIDX_ZERO:
  DL_TimerG_clearInterruptStatus(TIMER_STEPPER_Y_INST,DL_TIMER_IIDX_ZERO);
Stepper_TimerCallback(&stepper_y);
    break;
  default:
    break;
  }
}


void ptz_scan_task(void){//第三题
    if (!ptz_sweep_flag && s_ptz_current_state != PTZ_STATE_IDLE) {
        Stepper_Stop(&stepper_x);
        s_ptz_current_state = PTZ_STATE_IDLE;
        return;
    }
    if (s_ptz_current_state == PTZ_STATE_COMPLETE) {
        return;
    }
    switch (s_ptz_current_state) {
        case PTZ_STATE_IDLE:
            if (ptz_sweep_flag) {
                log_info("PTZ Scan Task: Activated.");
                s_ptz_current_state = PTZ_STATE_START;
            }
            break;
        case PTZ_STATE_START:
        Stepper_SetSpeed(&stepper_x,280);
            if (s_next_start_direction == 1) {
                log_info("PTZ Scan: Starting initial CW sweep (+1440 deg)...");
                Stepper_MoveToAngle(&stepper_x, stepper_x.current_angle + 1440.0f);
                s_ptz_current_state = PTZ_STATE_SWEEP_CW;
            } else { // SCAN_START_CCW
                log_info("PTZ Scan: Starting initial CCW sweep (-1440 deg)...");
                Stepper_MoveToAngle(&stepper_x, stepper_x.current_angle - 1440.0f);
                s_ptz_current_state = PTZ_STATE_SWEEP_CCW;
            }
            break;
        case PTZ_STATE_SWEEP_CW:
        case PTZ_STATE_SWEEP_CCW:
        log_debug("%.2f",stepper_x.speed_dps);
            if (point_x != 0) {
                log_info("PTZ Scan: Target detected! Engaging visual lock.");
                Stepper_Stop(&stepper_x);
                s_ptz_state_before_lock = s_ptz_current_state; // 记住被打断前的状态
                s_ptz_current_state = PTZ_STATE_VISUAL_LOCK;
            } 
            else if (stepper_x.running == 0) {
                // 扫描完成，开始反向扫描
                if (s_ptz_current_state == PTZ_STATE_SWEEP_CW) {
                    Stepper_MoveToAngle(&stepper_x, stepper_x.current_angle - 1440.0f);
                    s_ptz_current_state = PTZ_STATE_SWEEP_CCW;
                } else {
                    Stepper_MoveToAngle(&stepper_x, stepper_x.current_angle + 1440.0f);
                    s_ptz_current_state = PTZ_STATE_SWEEP_CW;
                }
            }
            break;

        case PTZ_STATE_VISUAL_LOCK: {
            // if (point_x == 0) {
            //     log_warn("PTZ Lock: Target lost! Resuming previous sweep.");
            //     Stepper_Stop(&stepper_x);
            //     s_ptz_current_state = s_ptz_state_before_lock; // 恢复到被打断前的状态
            //     break; // 退出本次switch
            // }
            if(point_x !=0){

            
            float desired_speed_dps = PID_Realize(&visal_pid, 399.0f, (float)point_x);
            Stepper_Speed_loop(&stepper_x, desired_speed_dps);
            uartprintf_DMA("%.2f,%.2f\r\n", 399.0f - (float)point_x, desired_speed_dps);
            float pid_error = fabsf(399.0f - (float)point_x);
            if (pid_error < VISUAL_STABLE_ERROR_THRESHOLD) {
            err_cnt++;
            if (err_cnt>1){
                Run_Corontine(pid_complete_corounite);
            }
            }else{
                err_cnt=0;
            }
            }
            break;
          
        }      
        case PTZ_STATE_COMPLETE:
            break;
    }
}

void pid_complete_corounite(void){
    TH_BEGIN();
  if (pid_error < VISUAL_STABLE_ERROR_THRESHOLD){
    ptz_start_flag=false;
    ptz_sweep_flag=false;
     s_ptz_current_state = PTZ_STATE_COMPLETE;
     DL_GPIO_writePins(GPIO_RELAY_PORT, GPIO_RELAY_RELAY_PIN);
    TH_DELAY(500);
    DL_GPIO_clearPins(GPIO_RELAY_PORT, GPIO_RELAY_RELAY_PIN);

  }
}


void ptz_ctrl_task(void){//第二题
    float angle_adjustment;
if (!ptz_start_flag) {
    return;}
else{
angle_adjustment=PID_Realize(&visal_pid, 400.0f, (float)point_x);
Stepper_Speed_loop(&stepper_x,angle_adjustment);
//uartprintf_DMA("%.2f,%d\r\n",angle_adjustment,point_x);
            pid_error = 400 -point_x;
            if (pid_error< VISUAL_STABLE_ERROR_THRESHOLD) {
                
                // 如果误差已经足够小，认为任务完成，切换到“完成”状态
                Run_Corontine(pid_complete_corounite);
            }
            
}
}


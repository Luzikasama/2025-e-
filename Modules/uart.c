#include "uart.h"
#include "stdio.h"
#include "stepper.h"
#include "string.h"
#include "ti_msp_dl_config.h"
#include <ctype.h>
#include "log.h"
#include "ptz.h"
#include "motor.h"
#include "tracking_car.h"
#include "scheduler.h"
uint8_t gEchoData[20];
uint8_t Uart_0_data[20];
uint8_t Rxflag;
uint8_t uart_0_cnt;

uint8_t gEchoData_1[20];
uint8_t Uart_1_data[20];
uint8_t Rxflag_1;
uint8_t uart_1_cnt;

uint8_t gEchoData_2[20];
uint8_t Uart_2_data[20];
uint8_t Rxflag_2;
uint8_t uart_2_cnt;

uint8_t gEchoData_3[20];
uint8_t Uart_3_data[20];
uint8_t Rxflag_3;
uint8_t uart_3_cnt;

float Yaw_angle;
 volatile bool g_uart_dma_tx_done=true;


void uart_irq_init(void){

        NVIC_ClearPendingIRQ(UART_DEBUG_INST_INT_IRQN);
        NVIC_EnableIRQ(UART_DEBUG_INST_INT_IRQN);
        DL_UART_enableInterrupt(UART_DEBUG_INST, DL_UART_INTERRUPT_RX);
        NVIC_ClearPendingIRQ(UART_HMI_INST_INT_IRQN);
        NVIC_EnableIRQ(UART_HMI_INST_INT_IRQN);
        DL_UART_enableInterrupt(UART_HMI_INST, DL_UART_INTERRUPT_RX);
        NVIC_ClearPendingIRQ(UART_RASPBERRYPI_INST_INT_IRQN);
        NVIC_EnableIRQ(UART_RASPBERRYPI_INST_INT_IRQN);
        DL_UART_enableInterrupt(UART_RASPBERRYPI_INST, DL_UART_INTERRUPT_RX);
        // NVIC_ClearPendingIRQ(UART_JY901S_INST_INT_IRQN);
        // NVIC_EnableIRQ(UART_JY901S_INST_INT_IRQN);
        // DL_UART_enableInterrupt(UART_JY901S_INST, DL_UART_INTERRUPT_RX);
}
/*IRQHAander*/
void UART_DEBUG_INST_IRQHandler(void){
static int16_t i;
switch (DL_UART_Main_getPendingInterrupt(UART_DEBUG_INST)) {
case DL_UART_MAIN_IIDX_RX:
gEchoData[i++]=DL_UART_Main_receiveData(UART_DEBUG_INST);

if (gEchoData[0]!='a'){
    i=0;
}

if (gEchoData[i-1]=='z') {
    uart_0_cnt=i;
    i=0;
    Rxflag=1;
    memcpy(Uart_0_data, gEchoData, uart_0_cnt);
    memset(gEchoData,0,sizeof(gEchoData));
    }
break;
    case DL_UART_MAIN_IIDX_DMA_DONE_TX:
    g_uart_dma_tx_done = true;
default:
break;
}

}



void UART_HMI_INST_IRQHandler(void){
static uint16_t k;
switch (DL_UART_Main_getPendingInterrupt(UART_HMI_INST)){
case DL_UART_MAIN_IIDX_RX:
gEchoData_1[k++]=DL_UART_Main_receiveData(UART_HMI_INST);
if (gEchoData_1[0]!=0xAA){
    k=0;
}

if (gEchoData_1[k-1]==0x55) {
    uart_1_cnt=k;
    k=0;
    Rxflag_1=1;
    memcpy(Uart_1_data, gEchoData_1, uart_1_cnt);
    memset(gEchoData_1,0,sizeof(gEchoData_1));
    }
break;
default:
break;
}

}


void UART_RASPBERRYPI_INST_IRQHandler(void){
static uint16_t k;
switch (DL_UART_Main_getPendingInterrupt(UART_RASPBERRYPI_INST)){
case DL_UART_MAIN_IIDX_RX:
gEchoData_2[k++]=DL_UART_Main_receiveData(UART_RASPBERRYPI_INST);
if (gEchoData_2[0]!=0xAA){
    k=0;
}

if (gEchoData_2[k-1]==0X55) {
    uart_2_cnt=k;
    k=0;
    Rxflag_2=1;
    memcpy(Uart_2_data, gEchoData_2, 20);
    memset(gEchoData_2,0,sizeof(gEchoData_2));
    }
break;
default:
break;
}

}


// void UART_JY901S_INST_IRQHandler(void){
// static uint32_t j;
// switch(DL_UART_Main_getPendingInterrupt(UART_JY901S_INST)){
// case DL_UART_MAIN_IIDX_RX:
// gEchoData_3[j++]=DL_UART_Main_receiveData(UART_JY901S_INST);

// if (gEchoData_3[0]!=0x55){
//     j=0;
// }
// if (gEchoData_3[0]==0x55&&gEchoData_3[1]!=0x53&&j>=2){
//     j=0;
// }
// if (j==11){

//     Rxflag_3=1;
//     j=0;
//     memcpy(Uart_3_data, gEchoData_3, 11);
//     memset(gEchoData_3,0,sizeof(gEchoData_3));
// }

// break;
// default:
// break;
// }
// }

/*uart receive handle*/

void Uart_DEBUG_check(){
if (Rxflag){
Rxflag=0;
Uart_0_data[uart_0_cnt] = '\0';
switch(Uart_0_data[1])
{
    case 'S':
        // 将接收到的数据从第二个字节开始转换成浮点数, 并赋值给目标变量
     log_debug("s:%.2f",atof((const char*)&Uart_0_data[2]));
  //  motor_r.speed=atof((const char*)&Uart_0_data[2]);
   // motor_l.speed=atof((const char*)&Uart_0_data[2]);
   Stepper_Speed_loop(&stepper_x,atof((const char*)&Uart_0_data[2]));

        break;

    case 'P':
       // P_value = atof((const char*)&Uart_0_data[1]);
       //log_debug("p:%.2f",atof((const char*)&Uart_0_data[2]));
//motor_r.speed_pid.kp=atof((const char*)&Uart_0_data[2]);
s_visual_pid.proportion=atof((const char*)&Uart_0_data[2]);
visal_pid.kp=atof((const char*)&Uart_0_data[2]);
        break;

    case 'I':
      //  I_value = atof((const char*)&Uart_0_data[1]);
     // log_debug("i:%.2f",atof((const char*)&Uart_0_data[2]));
      // motor_r.speed_pid.ki=atof((const char*)&Uart_0_data[2]);
      s_visual_pid.integral=atof((const char*)&Uart_0_data[2]);
visal_pid.ki=atof((const char*)&Uart_0_data[2]);
        break;

    case 'D':
      //  D_value = atof((const char*)&Uart_0_data[1]);
//motor_r.speed_pid.kd=atof((const char*)&Uart_0_data[2]);
s_visual_pid.derivative=atof((const char*)&Uart_0_data[2]);
visal_pid.kd=atof((const char*)&Uart_0_data[2]);
 //     log_debug("d:%.2f",atof((const char*)&Uart_0_data[2]));
        break;
  case 'O':
DL_GPIO_writePins(GPIO_RELAY_PORT, GPIO_RELAY_RELAY_PIN);
  break;
  case 'F':
DL_GPIO_clearPins(GPIO_RELAY_PORT, GPIO_RELAY_RELAY_PIN);
  break;
  case 'X':
Stepper_MoveToAngle(&stepper_x, atof((const char*)&Uart_0_data[2]));
  break;
  case 'Y':
Stepper_MoveToAngle(&stepper_y, atof((const char*)&Uart_0_data[2]));
  break;
    default:
        // 可选：在这里处理无法识别的命令
        break;
}


}
}



void Uart_HMI_check() {
    if (Rxflag_1) {
        Rxflag_1 = 0;
        // 2. 正确地处理字符串：将帧尾0x55替换为字符串结束符'\0'
        Uart_1_data[uart_1_cnt - 1] = '\0';

        // 3. 创建一个指向有效载荷(payload)的指针，这是修正的核心！
        // payload 指向 0xAA 之后的数据，所有的字符串操作都应基于它
        char *payload = (char *)&Uart_1_data[1];

        // 4. 基于 payload 的第一个字符进行 switch 判断
        switch (payload[0]) {
            case 's':
                // 检查是否为完整的 "start" 命令
                // 现在比较的是 "start" 和 payload，而不是整个 Uart_1_data
                if (strcmp(payload, "start") == 0) {
                    log_debug("SYSTEM CMD: Start\n");
                    Add_SchTask(car_circle_task,100.0f,ENABLE);
                    car_start_flag = true;
                }
                break;
                case 't':
                if (strcmp(payload, "task") == 0) {
                    log_debug("SYSTEM CMD: task\n");
                    Add_SchTask(car_circle_task_1,100.0f,ENABLE);
                    car_start_flag = true;
                }
               break;
            case 'c':
                // 检查是否为 "car" 命令前缀
                if (strncmp(payload, "car", 3) == 0) {
                    const char* sub_command = payload + 3; // sub_command 指向 "car" 后面的部分
                    int circle_val;
                    float speed_val;
                    if (sscanf(sub_command, " circle %d", &circle_val) == 1) {
                        circle_set = circle_val;
                        log_debug("CAR CMD: Circle, Laps: %d\n", circle_val);
                    } else if (sscanf(sub_command, " speed %f", &speed_val) == 1) {
                        g_car_speed = speed_val;
                        log_debug("CAR CMD: Speed, Value: %f\n", speed_val);
                    } else if (strstr(sub_command, "turn") != NULL) {
                        log_debug("CAR CMD: Turn\n");
                        turn_flag = true;
                    } 
                }
                break;
            case 'p':
                // 检查是否为 "ptz" 命令前缀
                if (strncmp(payload, "ptz", 3) == 0) {
                    const char* sub_command = payload + 3;
                    float x_val, y_val;
                    if (sscanf(sub_command, " x%f y%f", &x_val, &y_val) == 2) {
                        log_debug("PTZ CMD: Position, X: %f, Y: %f\n", x_val, y_val);
                        Stepper_MoveToAngle(&stepper_x, x_val - 360);
                        Stepper_MoveToAngle(&stepper_y, y_val);
                    } else if (strstr(sub_command, "home") != NULL) {
                        log_debug("PTZ CMD: Home\n");
                        Stepper_SetSpeed(&stepper_x,90);
                        Stepper_MoveToAngle(&stepper_x, 0);
                        Stepper_MoveToAngle(&stepper_y, 0);
                    } else if (strstr(sub_command, "scan") != NULL) {
                        log_debug("PTZ CMD: Scan\n");
                        ptz_start_flag = true;
                    } else if (strstr(sub_command, "stop") != NULL) {
                        log_debug("PTZ CMD: stop\n");
                        ptz_start_flag = false;
                        ptz_sweep_flag=false;
                        loop_flag=false;
                        Stepper_Stop(&stepper_x);
                        DL_GPIO_clearPins(GPIO_RELAY_PORT, GPIO_RELAY_RELAY_PIN);
                    }else if (strstr(sub_command, "cw") != NULL) {
                        log_debug("PTZ CMD: cw\n");
                        ptz_sweep_flag=true;
                        s_next_start_direction=1;
                    }else if (strstr(sub_command, "wc") != NULL) {
                        log_debug("PTZ CMD: ccw\n");
                        ptz_sweep_flag=true;
                        s_next_start_direction=0;
                    }else if (strstr(sub_command, "loop") != NULL) {
                        log_debug("PTZ CMD: loop\n");
                        loop_flag=true;
                        
                    }else if (strstr(sub_command, "light") != NULL) {
                        log_debug("PTZ CMD: light\n");
                        DL_GPIO_clearPins(GPIO_RELAY_PORT, GPIO_RELAY_RELAY_PIN);
                    }
                }
                break;
            default:
                break;
        }
    }
}

void Uart_RASPBERRYPI_check(){
if (Rxflag_2){
Rxflag_2=0;
Uart_2_data[uart_2_cnt] = '\0';
sscanf((const char*)&Uart_2_data[1], "x%huy%hu", &point_x, &point_y);
//log_debug("%d,%d",point_x,point_y);
}

}

void Uart_JY901S_check(){

if (Rxflag_3){
    Rxflag_3=0;
    if (Uart_3_data[8]==0xcc&&Uart_3_data[9]==0x46){
    Yaw_angle=(float)((Uart_3_data[7]<<8)|Uart_3_data[6])/32768*180;
    }
}
}


void Uart_check(){
    Uart_DEBUG_check();
    Uart_HMI_check();
    Uart_RASPBERRYPI_check();
    Uart_JY901S_check();
}

/*printf*/

void Serial_SendByte(uint8_t Byte,UART_Regs * const uart)
{
	DL_UART_Main_transmitDataBlocking(uart  ,Byte);	//将字节数据写入数据寄存器，写入后USART自动生成时序波形
}
 

void Serial_SendArray(uint8_t *Array, uint16_t Length,UART_Regs * const uart)
{
	uint16_t i;
	for (i = 0; i < Length; i ++)		//遍历数组
	{
		Serial_SendByte(Array[i],uart);		//依次调用Serial_SendByte发送每个字节数据
	}
}
/* printf*/
void uartprintf(UART_Regs * const uart ,const char *format, ...) {
  char buffer[UART_BUFFER_SIZE];
  va_list args;
  va_start(args, format);
  vsnprintf(buffer, UART_BUFFER_SIZE, format, args);
  va_end(args);

  unsigned int length = strlen(buffer);
  for (int i = 0; i < length; i++) {
    DL_UART_Extend_transmitDataBlocking(uart, (uint8_t)buffer[i]);
  }
}


void uartprintf_DMA( const char *format, ...)
{
    if (g_uart_dma_tx_done == false) {
        return ; 
    }
    static char buffer[256];
    
    va_list args;
    va_start(args, format);
    int length = vsnprintf(buffer, 256, format, args);
    va_end(args);

    if (length <= 0) {
        return;
    }

    DL_DMA_setSrcAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t)&buffer[0]);
    DL_DMA_setDestAddr(DMA, DMA_CH0_CHAN_ID, (uint32_t)(&UART_DEBUG_INST->TXDATA));
    DL_DMA_setTransferSize(DMA, DMA_CH0_CHAN_ID, length);
    g_uart_dma_tx_done = false;
    DL_DMA_enableChannel(DMA, DMA_CH0_CHAN_ID);
    
}


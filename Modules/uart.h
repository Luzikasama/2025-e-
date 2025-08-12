#ifndef __UART_H_
#define __UART_H_

#include "stdlib.h"
#include "ti_msp_dl_config.h"
#include "motor.h"
#include "stdio.h"
#include "uart.h"

extern float Yaw_angle;

extern uint8_t gEchoData[20];
extern uint8_t Uart_0_data[20];
extern uint8_t Rxflag;
extern uint8_t uart_0_cnt;

extern uint8_t gEchoData_1[20];
extern uint8_t Uart_1_data[20];
extern uint8_t Rxflag_1;
extern uint8_t uart_1_cnt;

extern uint8_t gEchoData_2[20];
extern uint8_t Uart_2_data[20];
extern uint8_t Rxflag_2;
extern uint8_t uart_2_cnt;

extern uint8_t gEchoData_3[20];
extern uint8_t Uart_3_data[20];
extern uint8_t Rxflag_3;
extern uint8_t uart_3_cnt;

#define UART_BUFFER_SIZE 128

typedef struct{
    uint8_t gEchoData[UART_BUFFER_SIZE];
    uint8_t Uart_data[UART_BUFFER_SIZE];
    uint8_t Rxflag;
    uint8_t uart_cnt;
    uint32_t UART_IRQN;
    UART_Regs * const uart_regs;
} uart_t;



void uart_irq_init(void);
void Uart_0_check();
void Uart_3_check();
void Uart_HMI_check();
void Uart_check();
void Serial_SendByte(uint8_t Byte,UART_Regs * const uart);
void Serial_SendArray(uint8_t *Array, uint16_t Length,UART_Regs * const uart);
void uartprintf(UART_Regs * const uart ,const char *format, ...) ;
void uartprintf_DMA(const char *format, ...);
#endif
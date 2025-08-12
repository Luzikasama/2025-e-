#ifndef  GRAY_SENSOR_H_
#define  GRAY_SENSOR_H_

#include "ti_msp_dl_config.h"

// 定义传感器输入
#define R4 (DL_GPIO_readPins(GPIO_GW_SENSOR_R4_PORT, GPIO_GW_SENSOR_R4_PIN))
#define R3 (DL_GPIO_readPins(GPIO_GW_SENSOR_R3_PORT, GPIO_GW_SENSOR_R3_PIN))
#define R2 (DL_GPIO_readPins(GPIO_GW_SENSOR_R2_PORT, GPIO_GW_SENSOR_R2_PIN))
#define R1 (DL_GPIO_readPins(GPIO_GW_SENSOR_R1_PORT, GPIO_GW_SENSOR_R1_PIN))
#define L1 (DL_GPIO_readPins(GPIO_GW_SENSOR_L1_PORT, GPIO_GW_SENSOR_L1_PIN))
#define L2 (DL_GPIO_readPins(GPIO_GW_SENSOR_L2_PORT, GPIO_GW_SENSOR_L2_PIN))
#define L3 (DL_GPIO_readPins(GPIO_GW_SENSOR_L3_PORT, GPIO_GW_SENSOR_L3_PIN))
#define L4 (DL_GPIO_readPins(GPIO_GW_SENSOR_L4_PORT, GPIO_GW_SENSOR_L4_PIN))

float Gray_Senor_Get_State();
int16_t Gray_Senor_Get_Turn();
#endif // GRAY_SENSOR8_H_

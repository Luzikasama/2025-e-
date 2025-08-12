#include "gray_sensor.h"
#include "math.h"

uint8_t sensor_val[8] = {0}; 
float sensor_offset = 0.0f;         // 计算出的偏移值（左负右正）
uint8_t sensor_active_count = 0;    // 有多少个传感器检测到黑线

// 权重表：从L4~R4，左负右正
const int8_t sensor_weight_table[8] = {-14, -12, -10, -8, 8, 10, 12, 14};

float Gray_Senor_Get_State(void)
{
    uint8_t State = 0;
    int16_t weight_sum = 0;
    sensor_active_count = 0;
  
        sensor_val[0] = L4 ? 1 : 0;
        sensor_val[1] = L3 ? 1 : 0;
        sensor_val[2] = L2 ? 1 : 0;
        sensor_val[3] = L1 ? 1 : 0;
        sensor_val[4] = R1 ? 1 : 0;
        sensor_val[5] = R2 ? 1 : 0;
        sensor_val[6] = R3 ? 1 : 0;
        sensor_val[7] = R4 ? 1 : 0;

    for (uint8_t i = 0; i < 8; i++) {
        if (sensor_val[i]) {
            State |= (1 << i); // 拼接状态位
            weight_sum += sensor_weight_table[i];
            sensor_active_count++;
        }
    }

    // 计算偏移量（平均权重）
    if (sensor_active_count > 0) {
        sensor_offset = (float)weight_sum / sensor_active_count;
    } else {
        sensor_offset = 0.0f; // 全白线时保持默认值或进入保护
    }

    return sensor_offset;
}


int16_t Gray_Senor_Get_Turn(){
  uint8_t State = 0;
  int16_t  sum;
    sensor_active_count = 0;
  
        sensor_val[0] = L4 ? 1 : 0;
        sensor_val[1] = L3 ? 1 : 0;
        sensor_val[2] = L2 ? 1 : 0;
        sensor_val[3] = L1 ? 1 : 0;

    for (uint8_t i = 0; i < 4; i++) {
        if (sensor_val[i]) {
            State |= (1 << i); // 拼接状态位
            sum += sensor_weight_table[i];
        }
    }

    return sum;

}
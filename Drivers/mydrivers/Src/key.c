#include "key.h"
uint8_t key_scan(void)
{
    static uint32_t last_state = 1; // 上次按键状态
    static uint32_t last_time = 0; // 上次按键时间
    uint8_t current_state = HAL_GPIO_ReadPin(key_GPIO_Port, key_Pin); // 当前按键状态
    uint32_t current_time = HAL_GetTick(); // 当前时间
    if (last_state == 1 && current_state == 0) // 按键按下
    {
        if (current_time - last_time > 200) // 去抖动
        {
            last_time = current_time;
            last_state = 0;
            return 1;
        }
    }
    else if (current_state == 1) // 按键松开
    {
        last_state = 1;
    }
    return 0;
}
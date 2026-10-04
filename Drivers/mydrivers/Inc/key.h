#ifndef __KEY_H
#define __KEY_H
#include "main.h"

/* PA9：专门用于 OLED 页面切换（外部轻触开关，另一端接 GND） */
#define KEY2_Pin        GPIO_PIN_9
#define KEY2_GPIO_Port  GPIOA

 uint8_t key_scan(void);
 uint8_t key2_scan(void);
#endif
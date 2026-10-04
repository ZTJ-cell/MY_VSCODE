#ifndef __ENCODER_H
#define __ENCODER_H

#include "main.h"

/* 引脚宏用 CubeMX 生成的（main.h 里）：
 *   Encoder_A_Pin   -> PA0   A 相, EXTI0
 *   Encoder_B_Pin   -> PA1   B 相, EXTI1
 *   Encoder_KEY_Pin -> PA2   按键, EXTI2
 * （CubeMX 里把标签改成 Encoder_A / Encoder_B / Encoder_KEY）
 */

extern volatile uint16_t encoder_cw_count;    /* 顺时针 */
extern volatile uint16_t encoder_ccw_count;   /* 逆时针 */

void Encoder_Init(void);
void Encoder_EXTI_Handler(uint16_t GPIO_Pin);

#endif

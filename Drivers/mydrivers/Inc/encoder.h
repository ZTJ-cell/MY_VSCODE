#ifndef __ENCODER_H
#define __ENCODER_H

#include "main.h"

/* EC11：A -> PA0(EXTI0)  B -> PA1(EXTI1)  C(按键) -> PA2(EXTI2) */
#define ENCODER_A_Pin         GPIO_PIN_0
#define ENCODER_A_GPIO_Port   GPIOA
#define ENCODER_B_Pin         GPIO_PIN_1
#define ENCODER_B_GPIO_Port   GPIOA
#define ENCODER_KEY_Pin       GPIO_PIN_2
#define ENCODER_KEY_GPIO_Port GPIOA

extern volatile uint16_t encoder_cw_count;    /* 顺时针 */
extern volatile uint16_t encoder_ccw_count;   /* 逆时针 */

void Encoder_Init(void);
void Encoder_EXTI_Handler(uint16_t GPIO_Pin);

#endif

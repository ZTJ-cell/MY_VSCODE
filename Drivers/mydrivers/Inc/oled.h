#ifndef __OLED_H
#define __OLED_H
#include "main.h"
#define SCL_H() HAL_GPIO_WritePin(SCL_GPIO_Port, SCL_Pin, GPIO_PIN_SET)
#define SCL_L() HAL_GPIO_WritePin(SCL_GPIO_Port, SCL_Pin, GPIO_PIN_RESET)
#define SDA_H() HAL_GPIO_WritePin(SDA_GPIO_Port, SDA_Pin, GPIO_PIN_SET)
#define SDA_L() HAL_GPIO_WritePin(SDA_GPIO_Port, SDA_Pin, GPIO_PIN_RESET)
#define SDA_READ() HAL_GPIO_ReadPin(SDA_GPIO_Port, SDA_Pin)
void OLED_Init(void);
void OLED_Clear(void);
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr);
void OLED_ShowChinese(uint8_t x, uint8_t y, const uint8_t *zh);
void OLED_ShowString(uint8_t x, uint8_t y, const char *str);
void delay_us(uint32_t us);
void OLED_SetCursor(uint8_t x, uint8_t y);
void OLED_WriteData(uint8_t data);
uint8_t I2C_WaitAck(void);
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len);
#endif
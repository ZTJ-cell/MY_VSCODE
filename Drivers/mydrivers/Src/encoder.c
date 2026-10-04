#include "encoder.h"

volatile uint16_t encoder_cw_count  = 0;
volatile uint16_t encoder_ccw_count = 0;

static volatile uint8_t  enc_last = 3;   /* 上次 AB：(A<<1)|B，上拉静止 = 11 */
static volatile uint8_t  enc_cur  = 3;
static volatile int8_t   enc_step = 0;   /* 攒够 ±4 才算一格 */
static volatile uint32_t key_tick = 0;

/* 查表：(上次状态<<2)|当前状态 -> 方向 */
static const int8_t quad_table[16] = {
     0, -1,  1,  0,
     1,  0,  0, -1,
    -1,  0,  0,  1,
     0,  1, -1,  0
};

void Encoder_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStruct.Pin  = ENCODER_A_Pin | ENCODER_B_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(ENCODER_A_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin  = ENCODER_KEY_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(ENCODER_KEY_GPIO_Port, &GPIO_InitStruct);

    HAL_NVIC_SetPriority(EXTI0_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);
    HAL_NVIC_SetPriority(EXTI1_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(EXTI1_IRQn);
    HAL_NVIC_SetPriority(EXTI2_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(EXTI2_IRQn);

    enc_cur  = (uint8_t)((HAL_GPIO_ReadPin(ENCODER_A_GPIO_Port, ENCODER_A_Pin) << 1)
                       |  HAL_GPIO_ReadPin(ENCODER_B_GPIO_Port, ENCODER_B_Pin));
    enc_last = enc_cur;
}

void Encoder_EXTI_Handler(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == ENCODER_A_Pin || GPIO_Pin == ENCODER_B_Pin)
    {
        uint8_t a = HAL_GPIO_ReadPin(ENCODER_A_GPIO_Port, ENCODER_A_Pin);
        uint8_t b = HAL_GPIO_ReadPin(ENCODER_B_GPIO_Port, ENCODER_B_Pin);

        enc_last = enc_cur;
        enc_cur  = (uint8_t)((a << 1) | b);

        int8_t dir = quad_table[(enc_last << 2) | enc_cur];

        if (dir > 0)
        {
            if (++enc_step == 4)  { enc_step = 0; encoder_cw_count++;  }
        }
        else if (dir < 0)
        {
            if (--enc_step == -4) { enc_step = 0; encoder_ccw_count++; }
        }
    }
    else if (GPIO_Pin == ENCODER_KEY_Pin)
    {
        if (HAL_GetTick() - key_tick > 200)
        {
            key_tick          = HAL_GetTick();
            encoder_cw_count  = 0;
            encoder_ccw_count = 0;
            enc_step          = 0;
        }
    }
}

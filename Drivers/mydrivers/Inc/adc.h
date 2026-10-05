#ifndef __ADC_H
#define __ADC_H
#include "main.h"

/* 参考电压：Blue Pill 的 VDDA 接 3.3V */
#define VREF_MV   3300UL
/* 12 位 ADC 满量程 */
#define ADC_FULL  4095UL
/* ADC 原始值 -> 真实电压(mV) */
#define ADC_RAW_TO_MV(r)   ((uint32_t)(r) * VREF_MV / ADC_FULL)

void     ADC_AppInit(void);      /* 上电校准，放在 USER CODE 2 */
uint16_t ADC_ReadRawAvg(void);   /* 连读 8 次取平均，抑制抖动 */

#endif

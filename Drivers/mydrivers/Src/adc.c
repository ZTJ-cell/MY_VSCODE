#include "adc.h"

extern ADC_HandleTypeDef hadc1;

/* 上电校准：F1 的 ADC 每次上电必须校准一次，否则读数偏差大 */
void ADC_AppInit(void)
{
  HAL_ADCEx_Calibration_Start(&hadc1);
}

/* 单次转换轮询读取，平均 8 次 */
uint16_t ADC_ReadRawAvg(void)
{
  uint32_t sum = 0;

  for (uint8_t i = 0; i < 8; i++)
  {
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, 10);
    sum += HAL_ADC_GetValue(&hadc1);
    HAL_ADC_Stop(&hadc1);
  }

  return (uint16_t)(sum / 8);
}

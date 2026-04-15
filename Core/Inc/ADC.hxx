#ifndef ADC_HXX
#define ADC_HXX
#include "main.h"
#include "stm32h7xx_hal_adc.h"
class ADC_c
{
  public:
    ADC_c( ADC_HandleTypeDef );
    ~ADC_c();
};
#endif
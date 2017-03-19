#include "nrf.h"

int16_t analogRead( int pin )
{
  int16_t value;
  uint32_t adcReference = ADC_CONFIG_REFSEL_VBG;

  // Twiz only have AIN0, AIN1 and AIN2 easily accessible (AIN7 could also work with IMU INT pin)
  if (pin >= 0 && pin <= 2) {
      pin += ADC_CONFIG_PSEL_AnalogInput0;
  } else {
      return 0;
  }

  NRF_ADC->ENABLE = 1;

  uint32_t config_reg = 0;
  config_reg |= ((uint32_t)ADC_CONFIG_RES_10bit << ADC_CONFIG_RES_Pos) & ADC_CONFIG_RES_Msk;
  config_reg |= ((uint32_t)ADC_CONFIG_RES_10bit << ADC_CONFIG_INPSEL_Pos) & ADC_CONFIG_INPSEL_Msk;
  config_reg |= ((uint32_t)adcReference << ADC_CONFIG_REFSEL_Pos) & ADC_CONFIG_REFSEL_Msk;

  if (adcReference & ADC_CONFIG_EXTREFSEL_Msk)
  {
      config_reg |= adcReference & ADC_CONFIG_EXTREFSEL_Msk;
  }

  NRF_ADC->CONFIG = ((uint32_t)pin << ADC_CONFIG_PSEL_Pos) | (NRF_ADC->CONFIG & ~ADC_CONFIG_PSEL_Msk);
  NRF_ADC->CONFIG = config_reg | (NRF_ADC->CONFIG & ADC_CONFIG_PSEL_Msk);

  NRF_ADC->TASKS_START = 1;

  while(!NRF_ADC->EVENTS_END);
  NRF_ADC->EVENTS_END = 0;

  value = (int16_t)NRF_ADC->RESULT;

  NRF_ADC->TASKS_STOP = 1;
  NRF_ADC->ENABLE = 0;

  return value;
}


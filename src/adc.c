#include "nrf.h"

int16_t analogRead( int pin )
{
  int16_t value;

#define USE_EXT_REF
#ifdef USE_EXT_REF
  // Use AREF0 (AREF1 is available on IMU INT pin):
  uint32_t adcReference = ADC_CONFIG_REFSEL_External | (ADC_CONFIG_EXTREFSEL_AnalogReference0 << ADC_CONFIG_EXTREFSEL_Pos);
#else
  uint32_t adcReference = ADC_CONFIG_REFSEL_VBG;
#endif

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

#ifdef USE_EXT_REF
  // Turn on "supply GPIO" - (can be used for the reference too - TODO: check sensor consumption)
  const int supplyPin = 1;
//  NRF_GPIO->OUTSET = (1UL << supplyPin);
#endif

  NRF_ADC->TASKS_START = 1;

  while(!NRF_ADC->EVENTS_END);
  NRF_ADC->EVENTS_END = 0;

  // Normalize from unsigned 10 bits to signed on 16 bits
  // The maximum on 10 bits: 11 1111 1111
  // ...becomes:           0111 1111 1110 0000
  value = (int16_t)NRF_ADC->RESULT;

#ifdef USE_EXT_REF
  // Turn off "supply GPIO"
  NRF_GPIO->OUTCLR = (1UL << supplyPin);
#endif

  NRF_ADC->TASKS_STOP = 1;
  NRF_ADC->ENABLE = 0;

  return value;
}


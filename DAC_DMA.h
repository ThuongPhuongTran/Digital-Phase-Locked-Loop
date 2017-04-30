/**
 *******************************************************************************
  * @file       ADC_DMA.h
  * @author     Anonymous
  * @Version    V1.0
  * date        2015/11/11
  * brief       ADC configuration for DMA
**/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DAC_DMA_H
#define __DAC_DMA_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

#include "main.h"
#include "time.h"
#include "misc.h"
#include "stm32f4xx_dac.h"
   
void Gpio_DAC_Config(void);
void DAC_Config(void);
void TDMA(void);

#ifdef __cplusplus
}
#endif
#endif /*__ mx_pinoutConfig_H */

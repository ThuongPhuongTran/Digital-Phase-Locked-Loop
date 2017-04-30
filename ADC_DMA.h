/**
 *******************************************************************************
  * @file       ADC_DMA.h
  * @author     Anonymous
  * @Version    V1.0
  * date        2015/11/11
  * brief       ADC configuration for DMA
**/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ADC_DMA_H
#define __ADC_DMA_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

#include "main.h"
   
void GPIO_Configuration(void);
void ADC1_Configuration(void);

#ifdef __cplusplus
}
#endif
#endif /*__ mx_pinoutConfig_H */

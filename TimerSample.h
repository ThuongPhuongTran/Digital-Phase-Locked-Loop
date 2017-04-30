/**
 *******************************************************************************
  * @file       TimerSample.h
  * @author     Anonymous
  * @Version    V1.0
  * date        2015/11/18
  * brief       ADC configuration for DMA
**/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TIMERSAMPLE_H
#define __TIMERSAMPLE_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

#include "main.h"
#include "time.h"
#include "misc.h"

void Timer_us(uint16_t sample);
void TIM5_Configuration(void);
#ifdef __cplusplus
}
#endif
#endif /*__ mx_pinoutConfig_H */
/**
 *******************************************************************************
  * @file       algorithm.h
  * @author     Anonymous
  * @Version    V1.0
  * date        2015/11/18
  * brief       ADC configuration for DMA
**/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ALGORITHM_H
#define __ALGORITHM_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

#include "main.h"
#include "complex.h"
#include <stdio.h>
#include <string.h>
#include "math.h"


void priori_adc(void);
void pre_filter(void);
void phase_shift_rov(void);
void snd_filter(void);
void fst_filter(void);
void pll(void);
void sigturn(void);
void multiturn(void);
float sinLUT(float value);
float cosLUT(float value);
#ifdef __cplusplus
}
#endif
#endif /*__ mx_pinoutConfig_H */
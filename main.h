/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : MAIN.H
* Author             : Anomymous
* Version            : V1.0.0
* Date               : 12/17/2008
* Description        : Main header file
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <intrinsics.h>
#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <arm_math.h>



#include "stm32f4xx_dac.h"

#include "arm_comm.h"
#include "iar_stm32f407zg_sk.h"

#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"

#include "algorithm.h"
#include "TimerSample.h"
#include "ADC_DMA.h"
#include "DAC_DMA.h"
#include "stm32f4xx_exti.h"

#include "GPIO.h"
#include "systick.h"
#include "interrupt.h"
#include "calib.h"

#endif /* __MAIN_H */

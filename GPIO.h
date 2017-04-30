/**
 *******************************************************************************
  * @file       GPIO.h
  * @author     Anonymous
  * @Version    V1.0
  * date        2015/11/11
  * brief       This file includes all of GPIO config
**/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GPIO_H
#define __GPIO_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
   
   void GPIOA_config(void);
   void GPIOB_config(void);
   void GPIOC_config(void);
   void GPIOD_config(void);
   
#ifdef __cplusplus
}
#endif
#endif /*__ GIPO_H */

/**
 *******************************************************************************
  * @file       AADPLL.h
  * @author     Anonymous
  * @Version    V1.0
  * date        2016/01/11
  
**/
/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __AADPLL_H
#define __AADPLL_H
#ifdef __cplusplus
 extern "C" {
#endif
   
#include "main.h"
#include "complex.h"
#include <stdio.h>
#include <string.h>
#include "math.h"
   
void pre_filter(void);
void pre_fuzzy(void);
void FuzzyPLL(void);

#ifdef __cplusplus
}
#endif
#endif /*__ mx_pinoutConfig_H */
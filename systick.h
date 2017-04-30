/**
  ******************************************************************************
  * @file    systick.h 
  * @author  Robot Club BK HCM
  * @version
  * @date
  * @brief
  ******************************************************************************
  */
	
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SYSTICK_H
#define __SYSTICK_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "main.h"

/* Private define ------------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
extern uint16_t cnt0, cnt1, cnt2, cnt3, cnt4, cnt_pad,cnt_step;
extern float cnt_vd;
/* Private function prototypes -----------------------------------------------*/
void SysTick_Init(uint32_t time);
void SysTick_Handler(void);
//void delay_ms(uint16_t time);
//void delay_us(uint32_t time);
void Delay(__IO uint32_t nTime);
void TimingDelay_Decrement(void);
void getTxBuff(void);
#endif

/******************* (C) COPYRIGHT 2011 STMicroelectronics - 2013 Robot Club BKHCM *****END OF FILE****/


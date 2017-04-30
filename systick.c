/**
  ******************************************************************************
  * @file    systick.c 
  * @author  
  * @version
  * @date
  * @brief	Main systick body
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "systick.h"
#include "main.h"

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t cb_tick = 0;
uint16_t imu_tick = 0;
/* Private functions ---------------------------------------------------------*/
static __IO uint32_t TimingDelay;
/*******************************************************************************
Function name: SysTick_Handler
Decription: This function handles SysTick Handler.
Input: None
Output: None
*******************************************************************************/
void SysTick_Handler(void)
{
    TimingDelay_Decrement();
    imu_tick++;
    if (imu_tick==9)
    {
//      priori_adc();
//      pre_filter();
//      phase_shift_rov();
//      snd_filter();
//      fst_filter();
//      pll();
//      imu_tick=0;
    }
	
}

/*******************************************************************************
Function name: SysTick_Init
Decription: This function initialize SysTick Timebase.
Input: None
Output: None
*******************************************************************************/
void SysTick_Init(uint32_t time)
{
if (SysTick_Config(SystemCoreClock / time))
  { 
    /* Capture error */ 
    while (1);
  }
}

/*******************************************************************************
Function name: Delay
Decription: Delay us
Input: None
Output: None
*******************************************************************************/
void Delay(__IO uint32_t nTime)
{ 
  TimingDelay = nTime;

  while(TimingDelay != 0);
}

void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
  }
}

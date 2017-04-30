/**
 *******************************************************************************
  * @file       GPIO.c
  * @author     Anonymous
  * @Version    V1.0
  * date        2015/11/11
  * brief       This file includes all of algorithm
**/

#include "math.h"
#include "main.h"


//---------------------------Variables Section--------------------------------//

//Vars for ADC-Reading
volatile float adc_now, adc_temp;
volatile float adc_sin[3], adc_cos[3];
volatile float adc_temps = 0, adc_tempc = 0;
int16_t init = 0;

void delay_clk(uint16_t time);

float32_t f_input_cmsis_dsp = 2;
float32_t f_result_cmsis_dsp;
  
 float f_input = 2;
 float f_result;
  
int main()
{
  //GPIOC_config();
  //GPIO_Configuration();
  ADC1_Configuration();
  DAC_Config();
 // SysTick_Init(10000);
  //TDMA();
  delay_clk(10000);
  pre_fuzzy();
  TIM5_Configuration();
  Timer_us(10);
  
  while(1)  
  {
  }
}

 //Simple delay
 void delay_clk(uint16_t time)
 {
   while(time > 0)
   {
     time--;
   }
 }

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
   
  }
}
#endif

/**
  * @}
  */ 

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

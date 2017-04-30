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
//Vars for function selection
int state = 0;

//Vars for ADC-Reading
volatile float adc_now, adc_temp;
volatile float adc_sin[3], adc_cos[3];
volatile float adc_temps = 0, adc_tempc = 0;
int16_t init = 0;

// Normalized & Phase shifts removing
volatile float Vs,Vc; 	
volatile float Xm,Xc; 	

//Vars for 2nd filter
volatile float sinfil[3] = {0,0,0};     //Initiazile the values of sin/cos inputs
volatile float x_sin[2] = {0,0};
volatile float cosfil[3] = {0,0,0};
volatile float x_cos[2] = {0,0};

//Vars for 1st filter
volatile float Xmfil[2] = {1,1};
volatile float x_Xm = 1;
volatile float Xcfil[2] = {0,0};
volatile float x_Xc = 0;

//Vars for API PLL
volatile float sinout = 0;
volatile float cosout = 1;

volatile float error[3]={0,0,0};
volatile float Fiout[3] = {0,0,0};

//Vars for adaptive coefficient //initial wn = 14000, epsilon = 0.8
volatile float Aek1 = 0.1169;
volatile float Aek2 = 0.0098;
volatile float Aek3 = -0.1071;

volatile float dev_error;

//Vars for timer debug
uint16_t count = 0;
int32_t nowtime,lasttime,debug;
volatile float Debug1 = 20.055555;
volatile float Debug2 = 160.015;
volatile float Debug3 = 20000;


double test1 =2.9562558;
double test2 =6.2545452;
float floattest=0;
double doubletest=0;
volatile float result =0;
volatile float result1 =0;
float resultangle;
//For debug only

//const uint16_t multitable[2][3][4]={1,2,3,4, 5,6
//,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24};
//Prototype
//extern const uint16_t multitable[8][27][29];
void delay_clk(uint16_t time);

float32_t f_input_cmsis_dsp = 2;
float32_t f_result_cmsis_dsp;
  
float f_input = 2;
float f_result;
  
int main()
{
  GPIOC_config();       //Config GPIOC for LED
  ADC1_Configuration(); //Config ADC 1,2,3,6,7,8
  DAC_Config();         //DAC PA4, PA5
  delay_clk(100);       //Delay 100 clock cycle
  EXT0_Configuration(); //Config button
  delay_clk(100);       //Delay 100 clock cycle
  //SysTick_Init(10000);
  //TDMA();
  TIM5_Configuration(); //TIM5 for DAC 
  Timer_us(10);         //TIM2 for algorithm        
  
  while(1)  
  {
    priori_adc();
    pre_filter();
    phase_shift_rov();
    snd_filter();
    fst_filter();
    pll();
    count=1;
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

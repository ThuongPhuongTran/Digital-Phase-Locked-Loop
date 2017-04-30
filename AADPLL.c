/**
 *******************************************************************************
  * @file       AADPLL_final.c
  * @author     Anonymous
  * @Version    V1.0
  * date        2016/01/16
  * brief       Algorithm
  * detail description:
  *******************************************************************************
**/

#include "AADPLL.h"

const float TwoPI = 6.28318;
const float Pi = 3.14159;

//Var for ADC_Reading from peripheral 
extern uint16_t ADCConvertedValues[24];

//Vars for ADC-Reading
extern float adc_now, adc_temp;
extern float adc_sin[3], adc_cos[3];
extern float adc_temps, adc_tempc;
extern int16_t init;

// Var for this function
float sinADC, cosADC;
float sinADC_old = 0;
float cosADC_old = 0;
float temp, tempa, temp_new, Fioutemp, temp_xsum;
float sinin, cosin;
//Sampling time Ts
float Ts = 0.00001;
// Var for 2nd lowpass filter
float FB00 = 0.000039;
float FB01 = 0.000078;
float FB02 = 0.000039;
float FA01 = -1.981171;
float FA02 = 0.981327;
// PLL coefficients
float Fiout = 0;          // Fi estimate
float ue[3] = {0};        // Error of PD
float duefil = 0;         // Derivative of error after 2nd lowpass filter
float uefil[3] = {0};     // Error after 2nd lowpass filter
float bw = 1000;          // Initial value of bw = 1000 rad
float bwinc = 0;          // Bandwidth increasing output from fuzzy
float K1 = 0;             // Bandwidth coefficient for PLL K1 & K2
float K2 = 0;
float zeta = 0.707;
float protion = 0;
float integral = 0;
float sum[2]={0};
float cosFiout = 1; 
float sinFiout = 0;
// Load fuzzy array
extern float bw_saved[21][101];
// Fuzzy look up
int row = 0;
int column = 0;
//Var for pre_PLL
float pre_numK1  = 0;
float pre_denK11 = 0;
float pre_denK12 = 0;
float pre_denK13 = 0;
float pre_numK2  = 0;
float numK1, denK1, numK2, denK2;
float temp;
// Calibration coefficient
float gradient_J[5] = {0}; // Gradient value
float X[5] = {1,0,1,0,0};
float rk1 = 0; 
float rk2 = 0;
float gamma = 0.002;
float cosAlpha = 1;
float sinAlpha = 0;

//Debug only
extern uint32_t ui32DACdebug1;
extern uint32_t ui32DACdebug2;

/**
  * @brief  Prefilter ADC
  * @param  None
  * @retval None
  * @detail: 
  * 1. Read ADC value from memmory
  * 2. Compare current ADC with previous ADC
  * 3. If |current ADC - previous ADC| > 400: current ADC = (current ADC + previous ADC)/2
  * 4. ADC_value = Average of (ADC[0]+ADC[0]+ADC[0]+ADC_temp)/4
  */
void pre_filter(void)
{
  if (init <= 10)
  {
    adc_temps += ADCConvertedValues[0];
    adc_tempc += ADCConvertedValues[1];
    init ++;
    if(init == 10)
    {
      adc_sin[0] = adc_temps/10;
      adc_cos[0] = adc_tempc/10;
      
      adc_sin[1] = adc_sin[0];
      adc_sin[2] = adc_sin[0];    
      adc_cos[1] = adc_cos[0];
      adc_cos[2] = adc_cos[0];
    }
  }
  else
  {
    //Read sine ADC signal
    adc_now = (ADCConvertedValues[0] + ADCConvertedValues[6] + ADCConvertedValues[12] + ADCConvertedValues[18])/4.0f;
    adc_temp = adc_now - adc_sin[0];
    
    if((adc_temp > 400) || (adc_temp < -400))
      adc_now = (adc_sin[0] + adc_now)/2.0f;
    
    //adc_temp = adc_now + adc_sin[0] + adc_sin[1] + adc_sin[2];
    adc_sin[2] = adc_sin[1];
    adc_sin[1] = adc_sin[0];
    //adc_sin[0] = adc_temp/4;
    adc_sin[0] = adc_now;

    
    //sinADC = adc_temp/4; //Shoud be divided to 4
    sinADC = adc_now;
    
    //Read cossine ADC signal
    adc_now = (ADCConvertedValues[1] + ADCConvertedValues[7] + ADCConvertedValues[13] + ADCConvertedValues[19])/4.0f;
    adc_temp = adc_now - adc_cos[0];
    
    if((adc_temp > 400) || (adc_temp < -400))
      adc_now = (adc_cos[0] + adc_now)/2.0f;
    
    adc_temp = adc_now + adc_cos[0] + adc_cos[1] + adc_cos[2];
    adc_cos[2] = adc_cos[1];
    adc_cos[1] = adc_cos[0];
    adc_cos[0] = adc_now;
    
    cosADC = adc_now; 
  }
}

void pre_fuzzy(void)
{
  pre_numK1  = 64*zeta*zeta*Ts;
  pre_denK11 = 4*(4*zeta*zeta+1)*(4*zeta*zeta+1);
  pre_denK12 = 32*zeta*zeta*Ts*(1+4*zeta*zeta);
  pre_denK13 = 64*(zeta*Ts)*(zeta*Ts);
  pre_numK2  = 256*(zeta*Ts)*(zeta*Ts);
}

void FuzzyPLL(void)
{
  // Fuzzy PLL
  // Phase detector
  // ue[0] = sinADC*cosFiout - cosADC*sinFiout;
  ue[0] = sinAlpha*cosFiout - cosAlpha*sinFiout;
  // Using 2nd lowpass filter after PD, uefil is an input of fuzzy
  uefil[0] = FB00*ue[0]+FB01*ue[1]+FB02*ue[2]-FA01*uefil[1]-FA02*uefil[2];
  duefil = (uefil[0] - uefil[1])/Ts;
  // Look up to find bandwidth
  if (uefil[0] < 0) temp = -uefil[0]; // Get absolute value of uefil
  else temp = uefil[0];
  if (temp > 0.006) temp = 0.006;
  if (duefil < -6) duefil = -6;
  if (duefil > 15) duefil = 15;
  // Look up index
  row = (int)temp*10000/3; // In MATLAB array begin from 1  - dont use it in C
  column = (int)(duefil+6)*100/21;
  // Look up bw
  bwinc = bw_saved[row][column];
  // Update bandwidth
  bw = bw + bwinc;
  // Limit bandwidth
  if (bw>10000) bw = 10000;
  if (bw<300 ) bw= 300;
  // Calculate K1 & K2;
  numK1 = pre_numK1*bw;
  denK1 = pre_denK11 + pre_denK12*bw + pre_denK13*bw*bw;
  numK2 = pre_numK2*bw*bw;
  denK2 = denK1;
  K1 = numK1/denK1;
  K2 = numK2/denK2;
  // PLL calculate
  protion = K1*ue[0];
  integral = K2*ue[0]+integral;
  sum[0] = protion + integral;
  Fiout = Fiout + sum[1];
  sum[1] = sum[0];
  // Limmit Fiout
  if (Fiout>TwoPI) Fiout -= TwoPI;
  if (Fiout<0) Fiout += TwoPI;
  // Saved filter for next cycle
  uefil[2] = uefil[1];
  uefil[1] = uefil[0];
  ue[2] = ue[1];
  ue[1] = ue[0];
  // Calibration adjust
  // Using temp to reduce calculating time of sine/cosine calculating
  cosFiout = cos(Fiout);
  sinFiout = sin(Fiout);
  // Calculate usin&ucos alpha
  cosAlpha = cosADC*X[0] + X[1];
  sinAlpha = sinADC*X[2] + X[3] + X[4]*cosADC;
  // Calculate error
  rk1 = cosAlpha - cosFiout;
  rk2 = sinAlpha - sinFiout;
  // Gradient calculate
  gradient_J[0] = cosADC_old*rk1;
  gradient_J[1] = rk1;
  gradient_J[2] = sinADC_old*rk2;
  gradient_J[3] = rk2;
  gradient_J[4] = cosADC_old*rk2;
  // Update X
  X[0] = X[0] - gamma*gradient_J[0];
  X[1] = X[1] - gamma*gradient_J[1];
  X[2] = X[2] - gamma*gradient_J[2];
  X[3] = X[3] - gamma*gradient_J[3];
  X[4] = X[4] - gamma*gradient_J[4];
  //Save ADC value for next time
  sinADC_old = sinADC;
  cosADC_old = cosADC;
}

/**
 *******************************************************************************
  * @file       algorithm.c
  * @author     Anonymous
  * @Version    V1.0
  * date        2016/02/12
  * brief       Algorithm
  * this code use phase shift remove method like a journal paper of Hung 
  * detail description:
  *******************************************************************************
**/

#include "algorithm.h"
#include "Kalman.h"
#include "multitable.h"
#include "sinetable.h"

const float TwoPI = 6.28318;
const float TwoPI28 = 6.28318;
//const float PI = 3.14159;
// Filter1: 2nd coeffficients - 1.5kHz, Ts = 10us
const float F2_A02 = 0.002078;
//#define F2_A1 0.004156
const float F2_B1 = -1.867;
const float F2_B2 = 0.8753;

// Filter2: 1st coefficients - 300Hz, Ts = 10us
const float F1_A01 = 0.009337;
const float F1_B1 = -0.9813;

const float damping = 2;//0.777;//2;//0.5//0.777
const float Ts = 0.00001;

//Calibrating values - Need to run auto-calibration first
//const float Ac = 0.795237;
//const float As = 0.8146;
//const float Bc = -0.0229;
//const float Bs = 0.0192;
//const float Beta = -0.0109;
float Ac = 0.7905;
float As = 0.8147;
float Bc = -0.0178;
float Bs = 0.0224;
float Beta = -0.0070;

//Need to calibration before performance
const float Off_Sin = 64.9;     //66.5//33//58.35//67.136//58.35//67.5//49.5
const float Amp_Sin = 54.34;    //45.5//29.65//41.5//45.014//41.5//34.5
const float Off_Cos = -0.1094;  //3.5//34.5//2.39//2.485//2.39//1//-0.44
const float Amp_Cos = 48.86;    //45//27.5//31.5//44.535//31.5

const float sqrt2 = 1.41;


//Var for ADC_Reading from peripheral 
extern uint16_t ADCConvertedValues[24];

//Vars for ADC-Reading
extern float adc_now, adc_temp;
extern float adc_sin[3], adc_cos[3];
extern float adc_temps, adc_tempc;
extern int16_t init; 


//Normalized & Phase shifts removing
extern float Vs,Vc; 	
extern float Xm,Xc; 	

//Var for 2nd filter
extern float sinfil[3];
extern float x_sin[2];
extern float cosfil[3];
extern float x_cos[2];

//Vars for 1st filter
extern float Xmfil[2];
extern float x_Xm;
extern float Xcfil[2];
extern float x_Xc;

//Vars for API PLL
extern float sinout;
extern float cosout;

extern float error[3];
extern float Fiout[3];

//Vars for adaptive coefficient //initial wn = 14000, epsilon = 0.8
extern float Aek1;
extern float Aek2;
extern float Aek3;

extern float dev_error;

//Var for this function
float sinADC, cosADC;
float sinADC1, cosADC1;

float temp, tempa, temp1,temp_HA1;
float sinin, cosin;

//DAC debug variable
extern volatile uint32_t ui32DACdebug1;
extern volatile uint32_t ui32DACdebug2;

//For Debug calib
float maxs = 0;
float mins= 90;
float amps = 0;
float offs = 0;
float ranges = 0;
float maxc = 0;
float minc= 30;
float ampc = 0;
float offc = 0;
float rangec = 0;
float KalOut = 0;
// Debug Fout
float FioutTemp = 0;

// multi-turn 
const uint16_t k=2;  //k is an integer and k>=1
const uint16_t N=4;  //N is an integer and N >= 2
const uint16_t a=4;  //a is an integer and a >= 2
const uint16_t M=32;   //a*k*N; 
const uint16_t R=28;  //M-a;
const uint16_t q=27;   //R-1;
const uint16_t S=29;   //R+1;
const uint16_t u=4096; // unit
uint16_t Fi_multi[4]={0};
long Fi_sig=0;  // angle resolution of main 
uint16_t pole=4;  //pole single
uint16_t cycle=0;
//LUT
uint16_t P[3]={0};
uint16_t m[3]={0};
uint32_t rotation=0;
//end multi-turn

int count_debug = 1;

/**
  * @brief  Priori using ADC
  * @param  None
  * @retval None
  * @detail: 
  * 1. Read and get average value of 32 first ADC values for stablity
  */
void priori_adc(void)
{
  while (init < 32)
  {
    adc_temps += ADCConvertedValues[0];
    adc_tempc += ADCConvertedValues[1];
    init ++;
    if(init == 32)
    {
      adc_sin[0] = adc_temps/32;
      adc_cos[0] = adc_tempc/32;
      
      adc_sin[1] = adc_sin[0];
      adc_sin[2] = adc_sin[0];    
      adc_cos[1] = adc_cos[0];
      adc_cos[2] = adc_cos[0];
    }
  }
 //DAC debug
//  ui32DACdebug1 = (uint32_t)(ADCConvertedValues[0]);
}               

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
  //Read sine ADC signal
  adc_now = (ADCConvertedValues[0] + ADCConvertedValues[6] + ADCConvertedValues[12] + ADCConvertedValues[18])/4.0f;
  adc_temp = adc_now - adc_sin[0];
  
  if((adc_temp > 400) || (adc_temp < -400))
    adc_now = (adc_sin[0] + adc_now)/2;
  
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
    adc_now = (adc_cos[0] + adc_now)/2;
  
  adc_temp = adc_now + adc_cos[0] + adc_cos[1] + adc_cos[2];
  adc_cos[2] = adc_cos[1];
  adc_cos[1] = adc_cos[0];
  //adc_cos[0] = adc_temp/4;
  adc_cos[0] = adc_now;
  
  //cosADC = adc_temp/4; //Should be divided to 4
  cosADC = adc_now;
  
//  ui32DACdebug2 = (uint32_t)(sinADC);
  
}

/**
  * @brief  Phase shift remove
  * @param  None
  * @retval None
  * @detail: 
  * 1. Using Eq.5 in the paper to remove phase shift
  * 2. Using Eq.3 to remove offset and different amplitude
  * @Note   This function just the same with Hung's source code, it is not really 
            like the method in the paper 
  */
void phase_shift_rov(void)
{
//  temp = sinADC;
//  sinADC = sinADC + cosADC;
//  cosADC = cosADC - temp;
//  
//  //sin ADC & cosADC are divied to 64 for in range [-64,64]
//  Vs = sinADC/64.0f; //ADC 12 bit
//  Vc = cosADC/64.0f;
//  
//  sinin = (Vs - Off_Sin)/Amp_Sin;
//  cosin = (Vc - Off_Cos)/Amp_Cos;
//    
//   

  //Normalize in [-1;1]
  sinADC = sinADC/2047.5f - 1;
  cosADC = cosADC/2047.5f - 1;
  //Remove offset and different Amplitude
  sinADC = (sinADC - Bs)/As;
  cosADC = (cosADC - Bc)/Ac;
  //Remove phase shift
  temp = sinADC;
  sinin = (sinADC + cosADC)/sqrt2;
  cosin = (cosADC - temp)/sqrt2;
  
  if(sinin >1) sinin = 1;
  if(sinin < -1) sinin = -1;
  if(cosin >1) cosin = 1;
  if(cosin < -1) cosin = -1;
  
//  ui32DACdebug1 = (uint32_t)(sinADC1*4095/2+4095/2);
//  ui32DACdebug2 = (uint32_t)(cosADC1*4095/2+4095/2);  
  ui32DACdebug2 = (uint32_t)(cosin*4095/2+4095/2);
}

/**
  * @brief  Code section of 2nd Filter
  * @param  None
  * @retval None
  * @detail: 
  * The transfer function of 2nd filter is
  *  F2_A02.(z^-2 + 2z^-1 + 1)
  * ---------------------------
  * F2_B2.z^-2 + F2_B1.z^-1 + 1
  * sinin = sinin[0]
  * x_sin[0] = sinin[1].z^-1
  * x_sin[1] = sinin[2].z^-2
  * @Note    
  */
void snd_filter(void)
{
  temp = x_sin[1] + 2*x_sin[0] + sinin;
  sinfil[0] = temp*F2_A02 - sinfil[1]*F2_B1 - sinfil[2]*F2_B2;
  sinfil[2] = sinfil[1];
  sinfil[1] = sinfil[0];
  x_sin[1]  = x_sin[0];
  x_sin[0]  = sinin;
  
  temp = x_cos[1] + 2*x_cos[0] + cosin;
  cosfil[0] = temp*F2_A02 - cosfil[1]*F2_B1 - cosfil[2]*F2_B2;
  cosfil[2] = cosfil[1];
  cosfil[1] = cosfil[0];
  x_cos[1]  = x_cos[0];
  x_cos[0]  = cosin;
  
  // DAC debug  
  // ui32DACdebug2 = (uint32_t)(sinfil[0]*4097/2+4097/2);
  // ui32DACdebug2 = (uint32_t)(cosfil[0]*4097/2+4097/2);
}

/**
  * @brief  Code section of 1st Filter
  * @param  None
  * @retval None
  * @detail: 
  * The transfer function of 1st filter is
  *  F1_A01.(z^-1 + 1)
  * ------------------
  *   F1_B1.z^-1 + 1
  * To filter Ua and Ut corresponding to the paper
  * @Note    
  */
void fst_filter(void)
{
  Xm = sinin*sinout + cosin*cosout;
  Xc = sinin*cosfil[0] - cosin*sinfil[0];
  
  Xmfil[0] = (Xm + x_Xm)*F1_A01 - Xmfil[1]*F1_B1;
  Xmfil[1] = Xmfil[0];
  x_Xm = Xm;
  
  Xcfil[0] = (Xc + x_Xc)*F1_A01 - Xcfil[1]*F1_B1;
  Xcfil[1] = Xcfil[0];
  x_Xc = Xc;
  
  // DAC debug  
//  ui32DACdebug1 = (uint32_t)(Xcfil[0]*4097/2+4097/2);
//  ui32DACdebug2 = (uint32_t)(Xmfil[0]*4097/2+4097/2);
}

//Correct signal loop
void pll(void)
{
  temp = sinfil[0]*cosout - cosfil[0]*sinout;   //U0
  error[0] = Xcfil[0] + Xmfil[0]*temp;          //Ue, this equation is different form the paper
  
  // ------ API Calculate wn - new method for calculating only with IQ24-----
  //dev_error = (error[2] - 4*error[1] + 3*error[0])/0.5*10^5; // 0.5*10^5 = 195.3125*2^8
  
  temp = error[2] - 4*error[1] + 3*error[0] ;
  
 // dev_error = 2.5*cabs(temp); //Corresponding to Hung's code, not adjust anything??
  
  if (temp<0) temp = -temp; //replace cabs
  dev_error = 2.5f*temp; 
  
  //D = Wn*Ts, K1 = 18000?, K2 = 2....using adaptive equation
  //temp = 0.35*cabs(error[0]) + dev_error;       //D = Wn*Ts, K1 = 18000, K2 = 2
  temp = error[0];
  if (temp<0) temp = -temp;  //replace cabs
//  temp_HA1 = 0.35*cabs(error[0]) + dev_error;
  temp_HA1 = 0.35f*temp + dev_error;
  //Limit
//  if (temp>0.1885)
//    temp = 0.1885;
//  else if (temp < 0.00031416)
//    temp = 0.00031416;
  temp1 = temp_HA1;
    if (temp_HA1>0.1885)
    temp_HA1 = 0.1885;
  else if (temp_HA1 < 0.00031416)
    temp_HA1 = 0.00031416;
  
  //
  tempa = temp_HA1*temp_HA1;    //D^2
  tempa = tempa/4.0f;      //(D^2)/4
  temp_HA1 = damping*temp_HA1;  //epsilon*D, epsilon is not adaptive as the paper
  
  Aek1 = tempa + temp_HA1;  //Eq.25 , Ts = 10^-5 is neglected
  Aek2 = tempa*2;
  Aek3 = tempa - temp_HA1;
  //End of API
  
  //Fiout estimation
  temp = 2*Fiout[1];
  
  //Fiout = 2.Fiout[1] - Fiout[2] + P1*u_e(k_1) + P2*u_e(k_2) + P3*u_e(k_3)
  Fiout[0] = temp - Fiout[2] + Aek1*error[0] + Aek2*error[1] + Aek3*error[2];

  sinout = sinf(Fiout[0]);
  cosout = cosf(Fiout[0]);
    
//  sinout = arm_sin_f32(Fiout[0]);
//  cosout = arm_cos_f32(Fiout[0]);
  
  /***************************************
//  Limit Fiout: 0-2PI
//  This limitation can only use in incremental encoder
//  In absolute encoder it will make the absolute angle be wrong
//  
//  if (Fiout[0] >= TwoPI ||Fiout[0] <= -TwoPI)
//  {
//    Fiout[0] = Fiout[0] - Fiout[1];
//    Fiout[1] = 0;
//  }
  **************************************/
  
/*  
  if (Fiout[0] >= 2*TwoPI)
  {
    Fiout[0] = Fiout[0] - 2*TwoPI;
    Fiout[1] = Fiout[1] - 2*TwoPI;
    Fiout[2] = Fiout[2] - 2*TwoPI;
  }
  
  if (Fiout[0] <= -2*TwoPI)
  {
    Fiout[0] = Fiout[0] + 2*TwoPI;
    Fiout[1] = Fiout[1] + 2*TwoPI;
    Fiout[2] = Fiout[2] + 2*TwoPI;
  }
*/ 
  if (Fiout[0] >= TwoPI || Fiout[0] <= -TwoPI)
  {
    Fiout[1] = Fiout[1] - Fiout[0];
    Fiout[0] = 0;
  }
  
//  sinout = sinf(Fiout[0]);
//  cosout = cosf(Fiout[0]);
  
  // Fout debug  
  FioutTemp = Fiout[0];
  if(FioutTemp <0) FioutTemp = FioutTemp + TwoPI;
  //Update
  Fiout[2] = Fiout[1];	// update
  Fiout[1] = Fiout[0];	// update
  error[2] = error[1];	// update
  error[1] = error[0];	// update
  //End of update
  // DAC debug  
//  sinout = sin(Fiout[0]);
  ui32DACdebug1 = (uint32_t)(FioutTemp*4095/TwoPI);
  //ui32DACdebug2 = (uint32_t)(sinout*4095/2+4095/2);
//  ui32DACdebug2 = (uint32_t)(cosout*4097/2+4097/2);
}

void sigturn(void)
{
  Fi_multi[0]=ADCConvertedValues[2];
  cycle =Fi_multi[0]/1024;   //(4096/pole)
  Fi_sig=(int)(Fiout[0]*4095/TwoPI)+cycle*4095;
}  

void multiturn(void)
{
  Fi_multi[1]=ADCConvertedValues[3];
  Fi_multi[2]=ADCConvertedValues[4];
  Fi_multi[3]=ADCConvertedValues[5];
  
  P[0]=Fi_multi[0]%4095+Fi_multi[1]%4095;
  P[1]=Fi_multi[0]%4095+Fi_multi[2]%4095;
  P[2]=Fi_multi[0]%4095+Fi_multi[3]%4095;
  
  // Multiturn calculate
  m[0]=P[0]/4095*M/a;
  m[1]=P[1]/4095*q;
  m[2]=P[2]/4095*S;
  
  rotation=multitable[m[0]][m[1]][m[2]];
}
float sinLUT(float value)
{
  uint16_t num=0;
  float result=0;
  
  if (value>=0)
    {
      if(value>TwoPI)
        {
          value=value-TwoPI;
        }
      //value = fmodf(value,TwoPI);
      if (value<=3.14159f) 
        {
          num=(uint16_t)((65535*value)/3.14159f);
          result=sintable[num];
        }
      else 
        {
          value=value-3.14159f;
          num=(uint16_t)((65535*value)/3.14159f);
          result=-sintable[num];
        }  
    }
  else
    {
      value=-value;
     // value =fmodf(value,TwoPI);
      if (value<=3.14159f) 
        {
          num=(uint16_t)((65535*value)/3.14159f);
          result=-sintable[num];
        }
      else 
        {
          value=value-3.14159f;
          num=(uint16_t)((65535*value)/3.14159f);
          result=sintable[num];
        }
    }
  return result;
}
float cosLUT(float value)
{
  value=value+1.570795f;
  return sinLUT(value);
}
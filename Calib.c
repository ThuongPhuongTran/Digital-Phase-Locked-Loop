/**
 *******************************************************************************
  * @file       Calib.h
  * @author     Anonymous
  * @Version    V1.0
  * date        2016/02/04
**/

#include "calib.h"

//Var for ADC_Reading from peripheral 
extern uint16_t ADCConvertedValues[24];

//Vars for ADC-Reading
extern float adc_now, adc_temp;
extern float adc_sin[3], adc_cos[3];
extern float adc_temps, adc_tempc;
extern int16_t init; 

// Debug Fout
extern float FioutTemp;

const float FA01_calib = -0.063;
const float FA00_calib = 0.071;
const float FB01_calib = -2;
const float FB02_calib = 1;

float estimate_calib[3] = {0,0};
float error_calib[2] = {0,0};

float lamda = 0.1;
float gamma = 0.001;

float gradient[5] = {0};
float usin[2] = {0,0};
float ucos[2] = {0,0};

float X_calib[5] = {1,0,1,0,0};

float M1_calib[5] = {1,1,0,0,0};
float M2_calib[5] = {0,0,1,1,1};

float rk_calib[2] = {0,0};
float ek_calib = 0;
float temp1_calib = 0;
float temp2_calib = 0;

extern float Ac;
extern float Bc;
extern float Beta;
extern float As;
extern float Bs;
/**
* param:        None
* output:       None
* brief:        Online gradient method
**/
void calib(void)
{
  //Normalize signals form [-1;1]
  usin[0] = adc_sin[0]/2047.5f - 1;
  ucos[0] = adc_cos[0]/2047.5f - 1;
  
  //Estimate phase
  error_calib[0] = (usin[1]*X_calib[2] + X_calib[3] + X_calib[4]*ucos[1])*cosf(estimate_calib[1]) - (ucos[1]*X_calib[0] + X_calib[1])*sinf(estimate_calib[1]);
  estimate_calib[0] = FA01_calib*error_calib[1] + FA00_calib*error_calib[0] - FB02_calib*estimate_calib[2] - FB01_calib*estimate_calib[1];
  
  //Matrix M
  M1_calib[0] = ucos[0];
  M2_calib[2] = usin[0];
  M2_calib[4] = ucos[0];
  
  // Calculate rk + (sin(alpha)-cos(alpha))*error
  rk_calib[0] = ucos[0]*X_calib[0] + X_calib[0] ;
  rk_calib[1] = usin[0]*X_calib[2] + X_calib[3] + ucos[0]*X_calib[4];
  ek_calib = (usin[0]*X_calib[2] + X_calib[3] + X_calib[4]*ucos[0])*cosf(estimate_calib[0]) - (ucos[0]*X_calib[0] + X_calib[1])*sinf(estimate_calib[0]);

  temp1_calib = rk_calib[0] + sinf(estimate_calib[0])*ek_calib;
  temp2_calib = rk_calib[1] - cosf(estimate_calib[0])*ek_calib;
  
  //Gradient
  gradient[0] = M1_calib[0]*temp1_calib + M2_calib[0]*temp2_calib;
  gradient[1] = M1_calib[1]*temp1_calib + M2_calib[1]*temp2_calib;
  gradient[2] = M1_calib[2]*temp1_calib + M2_calib[2]*temp2_calib;
  gradient[3] = M1_calib[3]*temp1_calib + M2_calib[3]*temp2_calib;
  gradient[4] = M1_calib[4]*temp1_calib + M2_calib[4]*temp2_calib;
  
  //Update calib value
  X_calib[0] = X_calib[0] - gamma*gradient[0];
  X_calib[1] = X_calib[1] - gamma*gradient[0];
  X_calib[2] = X_calib[2] - gamma*gradient[0];
  X_calib[3] = X_calib[3] - gamma*gradient[0];
  X_calib[4] = X_calib[4] - gamma*gradient[0];
  
  //Make sine and cosine in [-2*pi; 2*pi] zone
  if(estimate_calib[0] > 6.283185f)
  {
    estimate_calib[0] = estimate_calib[0] - 6.283185f;
    estimate_calib[1] = estimate_calib[1] - 6.283185f;
    estimate_calib[2] = estimate_calib[2] - 6.283185f;
  }
  if(estimate_calib[0] < -6.283185f)
  {
    estimate_calib[0] = estimate_calib[0] + 6.283185f;
    estimate_calib[1] = estimate_calib[1] + 6.283185f;
    estimate_calib[2] = estimate_calib[2] + 6.283185f;
  }
  
  //Saving old data
  usin[1] = usin[0];
  
  ucos[1] = ucos[0];
  error_calib[1] = error_calib[0];
  estimate_calib[2] = estimate_calib[1];
  estimate_calib[1] = estimate_calib[0];
  
//  //Calculate offset, gain mismatch and phase shift remove
//  Ac = 1/X_calib[0];
//  Bc = -X_calib[1]*Ac;
//  Beta = atanf(X_calib[4]*Ac);
//  As = 1/X_calib[2]/cosf(Beta);
//  Bs = -As*cos(Beta)*X_calib[3] + Bc*tanf(Beta)/Ac;
}

void get_values(void)
{
  //Calculate offset, gain mismatch and phase shift remove
  Ac = 1/X_calib[0];
  Bc = -X_calib[1]*Ac;
  Beta = atanf(X_calib[4]*Ac);
  As = 1/X_calib[2]/cosf(Beta);
  Bs = -As*cos(Beta)*X_calib[3] + Bc*tanf(Beta)/Ac;
}

/*-----------The End----------------------------------------------------------*/














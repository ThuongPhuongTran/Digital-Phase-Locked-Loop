/**
 *******************************************************************************
  * @file       ADC_DMA.c
  * @author     Anonymous
  * @Version    V1.0
  * date        2015/11/11
  * brief       ADC configuration for DMA
**/

#include "ADC_DMA.h"
volatile uint16_t ADCConvertedValues[24];

//ADC Pin configuration 
//Congfig PIN PA0, PA1, PA2, PA3, PA6, PA7 as analog pin
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  
  /* Configure ADC1 Channel3 & 5 pins as analog input ******************************/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1| GPIO_Pin_2| GPIO_Pin_3| GPIO_Pin_6| GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}
  
//******************************************************************************
//ADC 1 config  
void ADC1_Configuration(void)
{
  //Declare temp variable
  GPIO_InitTypeDef GPIO_InitStructure;
  ADC_InitTypeDef       ADC_InitStructure;
  ADC_CommonInitTypeDef ADC_CommonInitStructure;
  DMA_InitTypeDef       DMA_InitStructure;
  
  /*Configure GPIO  ****************************************************************/
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  
  /* Configure ADC1 Channel3 & 5 pins as analog input ******************************/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1| GPIO_Pin_2| GPIO_Pin_3| GPIO_Pin_6| GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
  /* Enable peripheral clocks *************************************************/
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  
  /* DMA2_Stream0 channel0 configuration **************************************/
  DMA_DeInit(DMA2_Stream0);
  DMA_InitStructure.DMA_Channel = DMA_Channel_0;                                //DMA channel 0
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;               //Peripheral addr
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&ADCConvertedValues[0];     //Memmory addr
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;                       //Peripheral to memmory (unidirection)
  DMA_InitStructure.DMA_BufferSize = 24;                                         //Buffer Size = 6, relevant to 6 ADC
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;              //Peripheral Inc is disable
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                       //Incremental mode is enabled
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;   //Data size is 2 bytes (half-words)
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;           //Data size of memmory corresponds to Peripheral 
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                               //Data is automatically reloaded with value programmed during the tream configuration phase
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;                           //Priority (8 stream is managed this priority)
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;   //dis                     //
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;             //
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;                   //
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;           //
  DMA_Init(DMA2_Stream0, &DMA_InitStructure);
  /* DMA2_Stream0 enable */
  DMA_Cmd(DMA2_Stream0, ENABLE);
  
  /* ADC Common Init **********************************************************/
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
  ADC_CommonInit(&ADC_CommonInitStructure);
  
  /* ADC1 Init ****************************************************************/
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion = 6;
  ADC_Init(ADC1, &ADC_InitStructure);
  
  /* ADC1 regular channel configuration ******************************/
  ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_3Cycles);//480Cycles); // PA0
  ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_3Cycles);//480Cycles); // PA1
  ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 3, ADC_SampleTime_3Cycles);//480Cycles); // PA2
  ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 4, ADC_SampleTime_3Cycles);//480Cycles); // PA3
  ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 5, ADC_SampleTime_3Cycles);//480Cycles); // PA6
  ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 6, ADC_SampleTime_3Cycles);//480Cycles); // PA7
  
  /* Enable DMA request after last transfer (Single-ADC mode) */
  ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);
  
  /* Enable ADC1 DMA */
  ADC_DMACmd(ADC1, ENABLE);
  
  /* Enable ADC1 **************************************************************/
  ADC_Cmd(ADC1, ENABLE);
  
  /* Start ADC1 Software Conversion */
  ADC_SoftwareStartConv(ADC1);
}


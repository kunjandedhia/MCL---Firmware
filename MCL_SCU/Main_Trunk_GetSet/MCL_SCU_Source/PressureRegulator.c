/******************************************************************************
**@file     :  PressureRegulator.c
* 
**@brief    :  This File contains the functions  for pressure regulator.
*******************************************************************************/    

/* ----------------------------------------------------------------------------
*                           Includes
* ----------------------------------------------------------------------------
*/
#include "Config.h"
/* ----------------------------------------------------------------------------
*                           MACROS
* ----------------------------------------------------------------------------
*/
/* ----------------------------------------------------------------------------
*                           CONSTANTS
* ----------------------------------------------------------------------------
*/
/* ----------------------------------------------------------------------------
*                           GLOBAL VARIABLES
* ----------------------------------------------------------------------------
*/
/* ----------------------------------------------------------------------------
*                           EXTERNAL VARIABLES
* ----------------------------------------------------------------------------
*/
/* ----------------------------------------------------------------------------
*                           EXTERNAL FUNCTIONS
* ----------------------------------------------------------------------------
*/
/* ----------------------------------------------------------------------------
*                           FUNCTIONS DECLARATION
* ----------------------------------------------------------------------------
*/
/*****************************************************************************
**@Function 		  : 	fnDAC_Config
**@Descriptions	: 	Configures the DAC Peripheral.
**@parameters		: 	None
**@return				: 	None
*****************************************************************************/
void fnDAC_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  DAC_InitTypeDef DAC_InitStructure;	
  
  /* Peripheral Clock Enable */	
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE); 
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  
  /* Once the DAC channel is enabled, the corresponding GPIO pin is
  automatically connected to the DAC converter. In order to avoid 
  parasitic consumption, GPIO pin should be configured in analog */
  
  
  /* GPIO Configuration */	
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN; 
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 
  GPIO_Init(GPIOA, &GPIO_InitStructure);  
  
  /* DAC Configuration */	
  DAC_InitStructure.DAC_Trigger = DAC_Trigger_None; 
  DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
  DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;  
  DAC_Init(DAC_Channel_1, &DAC_InitStructure); 
  
  /* Enable DAC Channel1: Once the DAC channel1 is enabled, PA.04 is  
  automatically connected to the DAC converter. */
  
  DAC_Cmd(DAC_Channel_1, ENABLE); 
}


/*************************************************************************
**@Function 	  : fnADC_Config
**@Description : ADC Initialization
**@Parameters	: None
**@Return			: None
*************************************************************************/
void fnADC_Config(void)
{
  ADC_InitTypeDef       ADC_InitStructure;
  ADC_CommonInitTypeDef ADC_CommonInitStructure;
  GPIO_InitTypeDef      GPIO_InitStructure;
  
  /* Enable ADC3, DMA2 and GPIO clocks */
  RCC_AHB1PeriphClockCmd(ADC_GPIO_CLOCK, ENABLE);
  RCC_APB2PeriphClockCmd(ADC_CLOCK, ENABLE);
  
  /* Configure ADC3 Channel 0 pin as analog input */
  GPIO_InitStructure.GPIO_Pin = PR_ADC_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(PR_ADC_PORT, &GPIO_InitStructure);
  
  ADC_DeInit();
  
  /* ADC Common Init */
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
  ADC_CommonInit(&ADC_CommonInitStructure);
  
  /* ADC3 Configuration */
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  /*ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;	*/
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion = 1;
  ADC_Init(ADC_PERIPHERAL, &ADC_InitStructure);
  
  ADC_ClearFlag(ADC_PERIPHERAL,ADC_FLAG_EOC);
  
  /* Enable ADC3 */
  ADC_Cmd(ADC_PERIPHERAL, ENABLE); 
}
/*****************************************************************************  
**@Function 				: fnADC3ChannelValue_Get  
**@Description			: get the conversion value from ADC
**@Parameters			: channel :  channel number for the  selected ADC
**@Return					: conversion value of ADC 
****************************************************************************/   
uint16_t fnADC3ChannelValue_Get(uint8_t chanel)   
{
  /* Configure channel   */
  ADC_RegularChannelConfig(ADC_PERIPHERAL, chanel, 1, ADC_SampleTime_3Cycles);   
  
  /* Start the conversion  */
  ADC_SoftwareStartConv(ADC_PERIPHERAL);   
  
  /* Wait until conversion completion   */
  
  //  fnDelay_ms(1);
  
  /* Get the conversion value   */
  return ADC_GetConversionValue(ADC_PERIPHERAL);   
} 

/*************************************************************************  
**@Function  	 : fnADCValue_Read  
**@Description  : this function Converts the value on given configured channel
to voltage
**@Parameters   : channel number for the  selected ADC  
**@Return       : converted value according to ADC   
*************************************************************************/  
float fnADCValue_Read(uint8_t chanel)
{	
  return ((float)(fnADC3ChannelValue_Get(chanel)*CONVR_NUMERATOR)/CONVR_DENOMINATOR);
}

/*****************************************************************************
**@Function 			:  fnDACValue_Set
**@Descriptions	:  convert mA Value into a Digital Value Then Generate a Analog Value
**@parameters		:  Passing a Milli Ampere (mA) Value.
**@return				:  None
*****************************************************************************/
void fnDACValue_Set(uint8_t ucmAValue)
{
  uint16_t uiConvertedValue;
  uint32_t uilLoop = 0;
  
#ifdef DBG_PRESSURE_REGULATOR
  printf("\r\n ucmAValue=%d \r\n",ucmAValue);
#endif
  uiConvertedValue = (uint16_t)((MULTIPLICATION_VALUE * ucmAValue) / DIVISION_VALUE);
  
#ifdef DBG_PRESSURE_REGULATOR
  printf("\r\n uiConvertedValue=%d \r\n",uiConvertedValue);
#endif
  
  DAC_SetChannel1Data(DAC_Align_12b_R, uiConvertedValue);
  for(uilLoop = 0 ; uilLoop < 100000 ; uilLoop++);
}



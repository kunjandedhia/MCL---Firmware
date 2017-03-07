/******************************************************************************
  * @file    Adc.c
  * 
  * @brief   This file contains the ADC functions
  ****************************************************************************
  */
/* ----------------------------------------------------------------------------
 *                           Includes
 * ---------------------------------------------------------------------------
 */
 #include "Includes.h"
/* ----------------------------------------------------------------------------
 *                           MACROS
 * ---------------------------------------------------------------------------
 */

/* ----------------------------------------------------------------------------
 *                           CONSTANTS
 * ---------------------------------------------------------------------------
 */

/* ----------------------------------------------------------------------------
 *                           GLOBAL VARIABLES
 * ---------------------------------------------------------------------------
 */

/* ----------------------------------------------------------------------------
 *                           EXTERNAL VARIABLES
 * ---------------------------------------------------------------------------
 */
 
 /* ---------------------------------------------------------------------------
 *                           EXTERNAL FUNCTIONS
 * ---------------------------------------------------------------------------
 */

/* ----------------------------------------------------------------------------
 *                           FUNCTIONS DECLARITION
 * ---------------------------------------------------------------------------
 */
/******************************************************************************
 **@Function 			: ADCInitialize  
 **@Description		: ADC3 Initialisation for the Reading of Pressure gauge and 
										Linear Transducer 
 **@Parameters		: None  
 **@Return				: None  
 ****************************************************************************/  
void fnADC_Init(void)
{
	GPIO_InitTypeDef 	stGPIO_InitStructure;   
	ADC_InitTypeDef   stADC_InitStructure;   
      
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);
	
	/* ADC Deinit 	*/
	ADC_DeInit(ADC3); 
   
	/*Analog input for Linear Transducer and Pressure gauge */
  stGPIO_InitStructure.GPIO_Pin 		= PG_GPIO_PINS;   
  stGPIO_InitStructure.GPIO_Speed 	= (GPIOSpeed_TypeDef)0;   
  stGPIO_InitStructure.GPIO_Mode 		= GPIO_Mode_AIN;   
  GPIO_Init(PG_GPIO_PORT, &stGPIO_InitStructure); 
	
	stGPIO_InitStructure.GPIO_Pin 		= LT_GPIO_PINS ;
  stGPIO_InitStructure.GPIO_Speed 	= (GPIOSpeed_TypeDef)0;   
  stGPIO_InitStructure.GPIO_Mode 		= GPIO_Mode_AIN;   
  GPIO_Init(LT_GPIO_PORT, &stGPIO_InitStructure);
	  
	/*ADC3 for Linear Transducer*/
	/* ADC Structure Initialization   */
  ADC_StructInit(&stADC_InitStructure);   
   
  stADC_InitStructure.ADC_Mode 								= ADC_Mode_Independent;   
  stADC_InitStructure.ADC_ScanConvMode 				= DISABLE;   
  stADC_InitStructure.ADC_ContinuousConvMode 	= DISABLE;   
  stADC_InitStructure.ADC_ExternalTrigConv 		= ADC_ExternalTrigConv_None;   
  stADC_InitStructure.ADC_DataAlign 					= ADC_DataAlign_Right;   
  stADC_InitStructure.ADC_NbrOfChannel 				= NUM_OF_ADC_CHANNEL;   
 
	ADC_Init(ADC3, &stADC_InitStructure);   
   
  /* Enable the ADC   */
  ADC_Cmd(ADC3, ENABLE);   
   
  /* ADC calibration   */
  ADC_StartCalibration(ADC3);   
  while(SET == ADC_GetCalibrationStatus(ADC3));
}
/******************************************************************************  
 **@Function 		: GetADC1Channel  
 **@Description	: get the conversion value from ADC
 **@Parameters	: channel :  channel number for the  selected ADC
 **@Return			: conversion value of ADC 
 ****************************************************************************/   
uint16_t fnGetADCChannel(uint8_t ucChannel)   
{
  /* Configure channel   */
  ADC_RegularChannelConfig(ADC3, ucChannel, 1, ADC_SampleTime_55Cycles5);   
   
  /* Start the conversion   */
  ADC_SoftwareStartConvCmd(ADC3, ENABLE);   
   
  /* Wait until conversion completion   */
  while(ADC_GetFlagStatus(ADC3, ADC_FLAG_EOC) == RESET);   
   
  /* Get the conversion value   */
  return ADC_GetConversionValue(ADC3);   
}
/******************************************************************************  
 **@Function 		 : fnADC_value  
 **@Description  : this function Converts the value on given configured channel
									 to voltage
 **@Parameters   : channel number for the  selected ADC  
 **@Return       : converted value according to ADC   
 ****************************************************************************/  
float fnADC_value(uint8_t ucChannel)
{	
	return ((float)(fnGetADCChannel(ucChannel) * CONVR_NUMERATOR) /
									CONVR_DENOMINATOR);
}


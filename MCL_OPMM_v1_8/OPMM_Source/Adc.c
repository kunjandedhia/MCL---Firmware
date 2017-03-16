/******************************************************************************
  **@file    Adc.c
  * 
  **@brief   This file contains the ADC functions
  ****************************************************************************
  */
/* ----------------------------------------------------------------------------
 *                           Includes
 * ---------------------------------------------------------------------------
 */
 #include "Config.h"
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
 *                           FUNCTIONS DECLARATION
 * ---------------------------------------------------------------------------
 */
uint16_t fnGetADCChannel(ADC_TypeDef *, uint8_t);
/******************************************************************************  
 **@Function 		: ADCInitialize  
 **@Description	: ADC1 Initialisation  
 **@Parameters	: None  
 **@Return			: None    
 *************************************************************************/  
void fnADC_Config(void)
{
	GPIO_InitTypeDef stGPIO_InitStructure;   
	ADC_InitTypeDef  stADC_InitStructure;   
	
  /*ADC3 INITIALIZATION*/
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);  
	
	/*ADC Deinit*/
  ADC_DeInit(ADC3);   

	/*Analog input Temperature sensors configuration */
  stGPIO_InitStructure.GPIO_Pin = TEMP_SENSOR1 | TEMP_SENSOR2 |
																	TEMP_SENSOR3 | TEMP_SENSOR4;									
  stGPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   
  stGPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;   
  GPIO_Init(TEMP_SENSOR_PORT, &stGPIO_InitStructure); 
		  
	stGPIO_InitStructure.GPIO_Pin = VBAT_STATUS;									
  stGPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   
  stGPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;   
  GPIO_Init(VBAT_SUPPLY_PORT, &stGPIO_InitStructure); 
	
  /*ADC Structure Initialization*/
  ADC_StructInit(&stADC_InitStructure);   
   
  stADC_InitStructure.ADC_Mode = ADC_Mode_Independent;   
  stADC_InitStructure.ADC_ScanConvMode = DISABLE;   
  stADC_InitStructure.ADC_ContinuousConvMode = DISABLE;   
  stADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;   
  stADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;   
  stADC_InitStructure.ADC_NbrOfChannel = 5;   
  ADC_Init(ADC3, &stADC_InitStructure);   
   
  /*Enable the ADC*/
  ADC_Cmd(ADC3, ENABLE);   
   
  /*ADC calibration*/
  ADC_StartCalibration(ADC3); 
  
  while(ADC_GetCalibrationStatus(ADC3) == SET);

	/*ADC1 INITIALIZATION*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);  
	
	/*ADC Deinit*/
  ADC_DeInit(ADC1); 
  
	/*Analog input Temperature sensors configuration */
  stGPIO_InitStructure.GPIO_Pin =  SUPPLY_24V_STATUS;   
  stGPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   
  stGPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;   
  GPIO_Init(MAIN_SUPPLY_24V_PORT, &stGPIO_InitStructure); 
	  
  /*ADC Structure Initialization*/
  ADC_StructInit(&stADC_InitStructure);   
   
  stADC_InitStructure.ADC_Mode = ADC_Mode_Independent;   
  stADC_InitStructure.ADC_ScanConvMode = DISABLE;   
  stADC_InitStructure.ADC_ContinuousConvMode = DISABLE;   
  stADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;   
  stADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; 
	stADC_InitStructure.ADC_NbrOfChannel = 1;  
  ADC_Init(ADC1, &stADC_InitStructure);   
   
  /*Enable the ADC*/
  ADC_Cmd(ADC1, ENABLE);   
   
  /*ADC calibration*/
  ADC_StartCalibration(ADC1); 
  
  while(ADC_GetCalibrationStatus(ADC1) == SET);
}

/*****************************************************************************  
 **@Function 		: GetADCChannel  
 **@Description	: get the conversion value from ADC
 **@Parameters	: ucChannel :  channel number for the  selected ADC
 **@Return			: uint16_t  
 ***************************************************************************/   
uint16_t fnGetADCChannel(ADC_TypeDef *ADCx, uint8_t ucChannel)   
{   
  /* Configure channel */
  ADC_RegularChannelConfig(ADCx, ucChannel, 1, ADC_SampleTime_55Cycles5);   
   
  /* Start the conversion */
  ADC_SoftwareStartConvCmd(ADCx, ENABLE);   
   
  /* Wait until conversion completion */
  while(RESET == ADC_GetFlagStatus(ADCx, ADC_FLAG_EOC));   
   
  /* Get the conversion value */
  return ADC_GetConversionValue(ADCx);   
} 

/****************************************************************************** 
 **@Function     : fnADC_Read  
 **@Description  : This function Converts the value on given configured
									 channel to voltage
 **@Parameters   : ADCx: ADC numer (x =  1, 2, 3)	
								   ucChannel:channel number for the  selected ADC  
 **@Return       : None  
 ****************************************************************************/  
float fnADC_Read(ADC_TypeDef * ADCx, uint8_t ucChannel)
{
	return ((fnGetADCChannel(ADCx, ucChannel) *  CONVR_NUMERATOR) /
					CONVR_DENOMINATOR);  
}

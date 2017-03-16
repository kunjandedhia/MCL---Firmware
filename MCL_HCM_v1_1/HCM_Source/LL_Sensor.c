/******************************************************************************
* @file    LL_Sensor.c
* 
* @brief   This file contains Functioin definations for the Liquid Level Sensor
******************************************************************************
*/    
/* ----------------------------------------------------------------------------
*                           Includes
* ----------------------------------------------------------------------------
*/
#include "Includes.h"
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
uint8_t ucLLWindowSize 						= 1; 
uint16_t uiTimCountLL 						= 0;
LL_t stLL[NO_OF_LL_SENSOR];

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
*/uint16_t fnGetADC1Channel(uint8_t);
/******************************************************************************  
 **@Function 		: ADCInitialize  
 **@Description	: ADC1 Initialisation  
 **@Parameters	: None  
 **@Return			: None    
 ****************************************************************************/  
void fnADC_Init(void)
{
	GPIO_InitTypeDef stGPIO_InitStructure;
	ADC_InitTypeDef stADC_InitStructure;   
   
  RCC_APB2PeriphClockCmd(RCC_APB2Periph, ENABLE);  
	/* ADC Deinit 	*/
  ADC_DeInit(ADC_NUM); 

	stGPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AIN;
  stGPIO_InitStructure.GPIO_Pin   = LIQUID_SENSOR_PIN;            
  GPIO_Init(LIQUID_SENSOR_PORT, &stGPIO_InitStructure);  
      
  /* ADC Structure Initialization   */
  ADC_StructInit(&stADC_InitStructure);   
   
  stADC_InitStructure.ADC_Mode 								= ADC_Mode_Independent;   
  stADC_InitStructure.ADC_ScanConvMode 				= DISABLE;   
  stADC_InitStructure.ADC_ContinuousConvMode 	= DISABLE;   
  stADC_InitStructure.ADC_ExternalTrigConv 		= ADC_ExternalTrigConv_None;   
  stADC_InitStructure.ADC_DataAlign 					= ADC_DataAlign_Right;   
  stADC_InitStructure.ADC_NbrOfChannel 				= NUMBER_OF_ADC_CHANNEL;   
  ADC_Init(ADC_NUM, &stADC_InitStructure);   
   
  /* Enable the ADC   */
  ADC_Cmd(ADC_NUM, ENABLE);   
   
  /* ADC calibration   */
  ADC_StartCalibration(ADC_NUM);   
  while(ADC_GetCalibrationStatus(ADC_NUM) == SET);
}
/******************************************************************************  
 **@Function 	  : fnGetADC1Channel  
 **@Description : get the conversion value from ADC
 **@Parameters	: ucChannel :  channel number for the  selected ADC
 **@Return			: uint16_t: returns the 16 bit converted integer value 
 ****************************************************************************/   
uint16_t fnGetADC1Channel(uint8_t ucChannel)   
{   
  /*Configure channel*/
  ADC_RegularChannelConfig(ADC_NUM, ucChannel, 1, ADC_SampleTime_55Cycles5);   
   
  /* Start the conversion   */
  ADC_SoftwareStartConvCmd(ADC_NUM, ENABLE);   
   
  /* Wait until conversion completion   */
  while(RESET == ADC_GetFlagStatus(ADC_NUM, ADC_FLAG_EOC));   
   
  /* Get the conversion value   */
  return ADC_GetConversionValue(ADC_NUM);   
}

/******************************************************************************  
 **@Function 		 : fnADC_Read  
 **@Description  : Read and return the voltage from the ADC
 **@Parameters   : ucChannel :  channel number for the  selected ADC
 **@Return       : float: Voltage value 
 ****************************************************************************/   
float fnADC_Read(uint8_t ucChannel)
{
	return ((float)fnGetADC1Channel(ucChannel)*(ADCRESOLUTION));
}
				
/******************************************************************************
 **@Function 		: fnLiquidLevel_Check
 **@Description	: this funtion will read the Liquid Level and Transmit to 
									SCU and HMI and also check for any error
 **@Parameters	: ucLLNum: this will indicate the sensor number/type
 **@Return			: None
 ****************************************************************************/
void fnLiquidLevel_Check(uint8_t ucLLNum)
{
	float fReadValue = 0.0f;
	#ifdef GENERAL_DBG_LL	
		char rgcTemp[200];
	#endif
		
	if(LL_NOT_CONNECTED != stLL[ucLLNum].ucADCChannel)
	{
		/*Reading liquid level in voltage*/
		fReadValue = fnADC_Read(stLL[ucLLNum].ucADCChannel);
		#ifdef GENERAL_DBG_LL
//				sprintf(rgcTemp, "\nLiquid Level voltage = %0.2f\r\n", fReadValue);	
//				DEBUG_PRINTF("%s", rgcTemp ); 
		#endif
		
		/*converting liquid level from Voltage to mm; 
			(0.6V corresponds to 200mm & 3.0V corresponds to 2000mm)*/
		fReadValue = (((fReadValue - LOW_LIQUID_LEVEL_V) * 
									LIQUID_LEVEL_SLOPE) + LOW_LIQUID_LEVEL);
		#ifdef GENERAL_DBG_LL
//			sprintf(rgcTemp, "Read Liquid Level %d = %0.2f\r\n", ucLLNum, fReadValue);	
//			DEBUG_PRINTF("%s", rgcTemp ); 
		#endif
	}
	else
	{
		fReadValue = INVALID_FLOAT_VALUE;
	}
	/*Validation*/
	if((MIN_LIQ_READING_RANGE <= fabs(fReadValue)) && 
		 (MAX_LIQ_READING_RANGE >= fabs(fReadValue))) 
	{
		stLL[ucLLNum].fLiquidLevel = stLL[ucLLNum].fLiquidLevelTankHeight - 
															 fReadValue;
		#ifdef GENERAL_DBG_LL
//			memset(rgcTemp, 0 ,200);
//			sprintf(rgcTemp, "ACTUAL Liquid Height = %0.2f\r\n", stLL[ucLLNum].fLiquidLevel);	
//			DEBUG_PRINTF("%s",rgcTemp ); 
		#endif 
		if((0 <= stLL[ucLLNum].fLiquidLevel) &&
			 (stLL[ucLLNum].fLiquidLevelTankHeight >= stLL[ucLLNum].fLiquidLevel))
		{
			stLL[ucLLNum].uiTimCountLLSample++;
			/*Minus the older value which is out of the window*/
			stLL[ucLLNum].fSumOfLL = stLL[ucLLNum].fSumOfLL - 
			stLL[ucLLNum].rgfLiquidLevelBuff[stLL[ucLLNum].ucWindowtraverse];
			
			stLL[ucLLNum].rgfLiquidLevelBuff[stLL[ucLLNum].ucWindowtraverse] = 
										stLL[ucLLNum].fLiquidLevel;
			
			/*summing all the value within the Window*/
			stLL[ucLLNum].fSumOfLL = stLL[ucLLNum].fSumOfLL + 
			stLL[ucLLNum].rgfLiquidLevelBuff[stLL[ucLLNum].ucWindowtraverse];
			#ifdef GENERAL_DBG
				memset(rgcTemp, 0 ,200);
				sprintf(rgcTemp, "Current LIQUID LEVEL = %0.2f\r\n", 
								stLL[ucLLNum].rgfLiquidLevelBuff[stLL[ucLLNum].ucWindowtraverse]);	
				DEBUG_PRINTF("%s", rgcTemp); 
			#endif 

			/*Enable to again send the Liquid level sensor fault if occurs*/
			stLL[ucLLNum].bLLSensorErrorSend = RESET;
			stLL[ucLLNum].bLLSensorError = RESET;
			
			stLL[ucLLNum].ucWindowtraverse++;
			/*check for Window Overflow or not, also for RESET traversing variable*/
			if((stLL[ucLLNum].ucWindowtraverse >= ucLLWindowSize) && 
				 (0 != ucLLWindowSize))
			{
				#ifdef GENERAL_DBG_LL
					//DEBUG_PRINTF("traverse Check\r\n"); 
				#endif
				stLL[ucLLNum].bSlideWindowFull = SET;
				stLL[ucLLNum].ucWindowtraverse = 0;
			}
		}
		else
		{
			stLL[ucLLNum].bLLSensorError = SET;	
		}
	}
	else
	{
		stLL[ucLLNum].bLLSensorError = SET;
	}
	
	/*Liquid Level Sensor Fault*/
	if((SET == stLL[ucLLNum].bLLSensorError) && 
		 (RESET == stLL[ucLLNum].bLLSensorErrorSend))
	{
		stLL[ucLLNum].bLLSensorErrorSend = SET;
		/*Tx Current liquid level to SCU and HMI */
		fnDataMsg_Tx(INVALID_FLOAT_VALUE, LIQUID_LEVEL, ucLLNum); 
		
		#ifdef GENERAL_DBG_LL
			DEBUG_PRINTF("Send Liquid level sensor fault\r\n"); 
		#endif
	
		stLL[ucLLNum].bLLLowTHFaultFlag = RESET;
		stLL[ucLLNum].bLLHighTHFaultFlag = RESET;
		stLL[ucLLNum].bLLInRangeFlag = RESET;
		
		/*Liquid Level Led ON, OFF*/
		uiIOExpander2 &= ~LL_LED_VALUE(ucLLNum, RED);
		uiIOExpander2 &= ~LL_LED_VALUE(ucLLNum, BLUE);
		uiIOExpander2 &= ~LL_LED_VALUE(ucLLNum, GREEN);
		
		stLL[ucLLNum].ucLLInRangeCnt = 0;
		stLL[ucLLNum].ucLLHighFaultCnt = 0;
		stLL[ucLLNum].ucLLLowFaultCnt = 0;
		return;
	}
	else if((SET == stLL[ucLLNum].bLLSensorError) && 
					(SET == stLL[ucLLNum].bLLSensorErrorSend))
	{
		return;
	}
		
	/*send Liquid Level data and check for any fault*/
	if((SET == stLL[ucLLNum].bSlideWindowFull) && 
		 (ucLLWindowSize <= stLL[ucLLNum].uiTimCountLLSample))
	{		
		#ifdef GENERAL_DBG_LL
			//DEBUG_PRINTF("Valid Liquid level\r\n"); 
		#endif
		
		stLL[ucLLNum].uiTimCountLLSample = 0;
		/*Average*/
		stLL[ucLLNum].fLiquidLevel = (stLL[ucLLNum].fSumOfLL / ucLLWindowSize);

		/*Tx Current liquid level to SCU and HMI */
		fnDataMsg_Tx(stLL[ucLLNum].fLiquidLevel, LIQUID_LEVEL, ucLLNum); 
		#ifdef GENERAL_DBG_LL	
			memset(rgcTemp, 0 ,200);
			sprintf(rgcTemp, "Current LIQUID LEVEL %d= %0.2f\r\n", ucLLNum, 
							stLL[ucLLNum].fLiquidLevel);
			DEBUG_PRINTF("%s", rgcTemp);
		#endif		
	
		/*Low thershold Fault*/
		if(stLL[ucLLNum].fLiquidLevel <= stLL[ucLLNum].fLiquidLevelLowTH)
		{		
			if(stLL[ucLLNum].ucLLLowFaultCnt < LL_LOW_TH_FAULT_COUNT) 
			{
				stLL[ucLLNum].ucLLLowFaultCnt++;
			}
			stLL[ucLLNum].ucLLInRangeCnt = 0;
			stLL[ucLLNum].ucLLHighFaultCnt = 0;
			if((LL_LOW_TH_FAULT_COUNT == stLL[ucLLNum].ucLLLowFaultCnt) && 
				 (RESET == stLL[ucLLNum].bLLLowTHFaultFlag))
			{
				stLL[ucLLNum].ucLLLowFaultCnt = 0;
				stLL[ucLLNum].bLLLowTHFaultFlag = SET;
				stLL[ucLLNum].bLLHighTHFaultFlag = RESET;
			  stLL[ucLLNum].bLLInRangeFlag = RESET;

				StatusFlag.stStatusFlag.bWarnningFlag = SET;
				/* Fault tx to SCU and HMI */
				fnFaultMsg_Tx(LOW_THERSHOLD,LIQUID_LEVEL,ucLLNum);
				
				
				/*Liquid Level Led ON, OFF*/
				uiIOExpander2 &= ~LL_LED_VALUE(ucLLNum, RED);
				uiIOExpander2 |= LL_LED_VALUE(ucLLNum, BLUE);
				uiIOExpander2 &= ~LL_LED_VALUE(ucLLNum, GREEN);
				
				#ifdef GENERAL_DBG
					DEBUG_PRINTF("Low Liquid level Threshold Fault\r\n"); 
				#endif
			}
		}
		/*In Range*/
		else if((fabs(stLL[ucLLNum].fLiquidLevel) > stLL[ucLLNum].fLiquidLevelLowTH) &&
						(fabs(stLL[ucLLNum].fLiquidLevel) < stLL[ucLLNum].fLiquidLevelHighTH))
		{
			if(stLL[ucLLNum].ucLLInRangeCnt < LL_LOW_TH_FAULT_COUNT)
			{
				stLL[ucLLNum].ucLLInRangeCnt++;
			}
			stLL[ucLLNum].ucLLLowFaultCnt = 0;
			if((LL_LOW_TH_FAULT_COUNT == stLL[ucLLNum].ucLLInRangeCnt) && 
				 (RESET == stLL[ucLLNum].bLLInRangeFlag))
			{
				stLL[ucLLNum].ucLLInRangeCnt = 0;
				stLL[ucLLNum].bLLLowTHFaultFlag = RESET;
				stLL[ucLLNum].bLLHighTHFaultFlag = RESET;
			  stLL[ucLLNum].bLLInRangeFlag = SET;
				
				/*Liquid Level Led ON, OFF*/
				uiIOExpander2 &= ~LL_LED_VALUE(ucLLNum, RED);
				uiIOExpander2 &= ~LL_LED_VALUE(ucLLNum, BLUE);
				uiIOExpander2 |= LL_LED_VALUE(ucLLNum, GREEN);
				
				#ifdef GENERAL_DBG
					DEBUG_PRINTF("Liquid level In Range\r\n"); 
				#endif
			}
		}
		/*High Thershold Fault*/
		else if(fabs(stLL[ucLLNum].fLiquidLevel) >= stLL[ucLLNum].fLiquidLevelHighTH)
		{
			if(stLL[ucLLNum].ucLLHighFaultCnt < LL_HIGH_TH_FAULT_COUNT)
			{
				stLL[ucLLNum].ucLLHighFaultCnt++;
			}
			stLL[ucLLNum].ucLLInRangeCnt = 0;
			stLL[ucLLNum].ucLLLowFaultCnt = 0;
			if((LL_HIGH_TH_FAULT_COUNT == stLL[ucLLNum].ucLLHighFaultCnt) && 
				 (RESET == stLL[ucLLNum].bLLHighTHFaultFlag))
			{
				stLL[ucLLNum].bLLHighTHFaultFlag = SET;
				stLL[ucLLNum].bLLLowTHFaultFlag = RESET;
			  stLL[ucLLNum].bLLInRangeFlag = RESET;

				StatusFlag.stStatusFlag.bWarnningFlag = SET;

				stLL[ucLLNum].ucLLHighFaultCnt = 0;
				/*High thershold fault */
				fnFaultMsg_Tx(HIGH_THERSHOLD,LIQUID_LEVEL, ucLLNum);
				
				/*Liquid Level Led ON, OFF*/
				uiIOExpander2 |= LL_LED_VALUE(ucLLNum, RED);
				uiIOExpander2 &= ~LL_LED_VALUE(ucLLNum, BLUE);
				uiIOExpander2 &= ~LL_LED_VALUE(ucLLNum, GREEN);
				#ifdef GENERAL_DBG
					DEBUG_PRINTF("High Liquid level Threshold Fault\r\n"); 
				#endif
			}
		}
	}
}



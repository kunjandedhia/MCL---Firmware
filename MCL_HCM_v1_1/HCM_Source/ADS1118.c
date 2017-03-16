/******************************************************************************
* @file    ADS1118.c
* 
* @brief   This file contains the funtions related to Thercouple IC(ADS1118).
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
uint8_t ucCReading	= 0; 		/*for currently reading thermocouple temp*/
uint8_t ucSwitchRd 	= 0; 		/*for switching between TS and ADC reading of ADS1118*/
uint16_t uiConfigWr = DEFAULT_SETTING; /*Configuration Register of the ADS1118*/
uint16_t uiConfigRd = 0;		/*Use for Read ADC data From the ADS1118*/
/*NIST -  National Institute of Standards and Technology (NIST)*/
float fTcj 					= 0.0f;	/*Read Cold Junction Temp Value from the ADS1118*/
float fVcj 					= 0.0f;	/*Converted voltage Value from fTcj using NIST table*/
float fVtc 					= 0.0f;	/*Read Thermocouple Voltage Value from the ADS1118*/
float fVtotal 			= 0.0f; /*Added voltage values of Thermocouple and Cold Junction*/

/*Look Up table according to NIST table for K-type Thermocouple*/
/*Coloumn 1 ==> Temp and Coloumn(in °C) 2 ==> volts in mV*/
float rgfLookUp[][2] = { {-160.0f, -5.141f},
												 {-120.0f, -4.138f},	
												 {-100.0f, -3.554f},	
												 {-90.0f, -3.243f},
												 {-80.0f, -2.920f},
												 {-70.0f, -2.587f},	
												 {-60.0f, -2.243f},	
												 {-50.0f, -1.889f},
												 {-40.0f, -1.527f},
												 {-30.0f, -1.156f},	
												 {-20.0f, -0.778f},	
												 {-10.0f, -0.392f},	
												 {0.0f, 0.0f},	
												 {10.0f, 0.397f},	
											   {20.0f, 0.798f},	
												 {30.0f, 1.203f},	
												 {40.0f, 1.612f},	
											 	 {50.0f, 2.023f},	
												 {60.0f, 2.436f},	
												 {70.0f, 2.851f},	
												 {80.0f, 3.267f},	
												 {90.0f, 3.682f},	
												 {100.0f, 4.096f},	
												 {110.0f, 4.509f},	
												 {120.0f, 4.920f},	
												 {130.0f, 5.328f},	
												 {140.0f, 5.735f},	
												 {150.0f, 6.138f},	
												 {180.0f, 7.340f},	
												 {200.0f, 8.138f},	
												 {220.0f, 8.940f},	
												 {250.0f, 10.153f},	
												 {280.0f, 10.153f},	
												 {300.0f, 12.209f},	
												 {350.0f, 14.293f},	
												 {400.0f, 16.397f},	
												 {450.0f, 18.516f},	
												 {500.0f, 20.644f},	
												 {550.0f, 22.776f},	
												 {600.0f, 24.905f},	
												 {650.0f, 27.025f},	
											 	 {700.0f, 29.129f},	
												 {800.0f, 33.275f},	
												 {900.0f, 37.326f},	
												 {1000.0f, 41.276f},
												 {1050.0f, 43.211f},
												 {1100.0f, 45.119f},
												 {1200.0f, 48.838f},
												 {1300.0f, 52.410f},
												 {1350.0f, 54.138f},
												 {1372.0f, 54.886f},							 
											 };
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
/******************************************************************************
 **@Function 		: fnReadTemp
 **@Description	: This function read the cold Junction Temp and Delta V
 **@Parameters	: None
 **@Return			: None
 *****************************************************************************/
void fnReadTemp(void)
{
	#ifdef TC_READ
		uint8_t rgucTemp[150];
	#endif 

	if(0 != stTC.stRead[ucCReading].ucThermoPosition)
	{
		if(0 == ucSwitchRd)
		{ 
			ucSwitchRd = 1;	/*Use for switching between TS and ADC reading of ADS1118*/
			
			uiConfigWr &= ~TS_MODE;  /* Setting  TS = 0 for ADC read in next cycle */
			uiConfigRd = fnADS1118_ReadData(uiConfigWr);/*COLD_JUCNTION*/

			uiConfigRd = ((uiConfigRd >> 2) & 0x3FFF);

			/*voltage calculation*/
			if(uiConfigRd & 0x2000)
			{
				uiConfigRd = ((~(uiConfigRd) + 1) & 0x3FFF);
				fTcj = -(float)uiConfigRd * COLDJ_RESOLUTION;
			}
			else
			{
				fTcj = (float)uiConfigRd * COLDJ_RESOLUTION;
			}				
		}
		else
		{
			ucSwitchRd = 0; 
			#ifdef TC_READ
				DEBUG_PRINTF("\nCONG Reg : 0x%04x\n",uiConfigWr);
				sprintf((char *)rgucTemp, "Tcj = %0.04f C  \r\n", fTcj);
				DEBUG_PRINTF((const char *)rgucTemp);
			#endif
			/*Conversion temp to voltage according to NIST Table for K-type TC*/
			fVcj = fnTempToVoltage(fTcj);
			
			#ifdef TC_READ
				sprintf((char *)rgucTemp, "Vcj = %0.04f mV\r\n", fVcj);
				DEBUG_PRINTF((const char *)rgucTemp);		
			#endif
			
			/*Use for switching between TS and ADC reading of ADS1118*/

			uiConfigWr |= TS_MODE; /*Setting  TS = 1 for Cold Junction Temp*/
			
			uiConfigRd = fnADS1118_ReadData(uiConfigWr);/*Delta V ADC*/
			
			if(uiConfigRd & 0x8000)
			{
				uiConfigRd = (~(uiConfigRd) + 1);
				fVtc = -(((float)uiConfigRd * RES_NUM) / RES_DEN ) * 1000.0f;
			}
			else
			{
				fVtc = (((float)uiConfigRd * RES_NUM) / RES_DEN ) * 1000.0f;
			}
					
			#ifdef TC_READ
				sprintf((char *)rgucTemp, "Vtc = %0.04f mV\r\n", fVtc);
				DEBUG_PRINTF((const char *)rgucTemp);
			#endif
			/*addition of both voltages*/
			fVtotal = fVtc + fVcj;
			
			#ifdef TC_READ
				sprintf((char *)rgucTemp, "Voltage = %0.04f mV\r\n", fVtotal);
				DEBUG_PRINTF((const char *)rgucTemp);
			#endif
			
			/*Convert voltage to Temp usin NIST and Current Temp stored in structure*/
			stTC.stRead[ucCReading].fCurrentTemp = (float)fnVoltageToTemp(fVtotal);
			
			#ifdef TC_READ
				sprintf((char *)rgucTemp, "Temperature %d = %0.04f C \r\n", ucCReading,
												stTC.stRead[ucCReading].fCurrentTemp);
				DEBUG_PRINTF((const char *)rgucTemp);
			#endif
				
//				sprintf((char *)rgucTemp, "Temperature in read temp function %d = %0.04f C \r\n", ucCReading,
//												stTC.stRead[ucCReading].fCurrentTemp);
//				DEBUG_PRINTF((const char *)rgucTemp);
			/************************* Temp reading completed ***********************/
			
			/*this check for thermocouple not connected or faulty*/
			if(fVtotal >= 55.0f)
			{
				stTC.stRead[ucCReading].fCurrentTemp = INVALID_FLOAT_VALUE;
				#ifdef TC_READ
					DEBUG_PRINTF("Fault Detected\r\n");
				#endif
			}
			ucCReading++;
			if(NO_OF_TC_SENSOR <= ucCReading)
			{
				ucCReading = 0;
				StatusFlag.stStatusFlag.bTCProcessed = SET;
			}		
		}
	}
	else
	{
		stTC.stRead[ucCReading].fCurrentTemp = INVALID_FLOAT_VALUE;
		#ifdef TC_READ
			DEBUG_PRINTF("not configured by user or not connected ucCReading : %d\r\n", 
										ucCReading);
		#endif
		ucCReading++;
		if(NO_OF_TC_SENSOR <= ucCReading)
		{
			ucCReading = 0;
			StatusFlag.stStatusFlag.bTCProcessed = SET;
		}	
	}
	
	
}
/*******************************************************************************
 **@Function 			: fnADS1118_ReadData
 **@Descriptions	: This function is used to Read 16 bit data from ADS1118 
 **@Parameters		: uiData: 16 bit Command Data
 **@Return				:	data read from ADS1118
 *****************************************************************************/
uint16_t fnADS1118_ReadData(uint16_t uiData)   
{ 
	uint16_t uiReadData = 0;
	ADS1118_SELECT(stTC.stRead[ucCReading].ucThermoPosition);  /*CS asserts low*/
	uiReadData = fnSPI_ReadWrite(TC_SPI, uiData);	
	ADS1118_UNSELECT(stTC.stRead[ucCReading].ucThermoPosition);/*CS asserts High*/
	return 	uiReadData;
}
/*******************************************************************************
 **@Function 			: fnTempToVoltage
 **@Descriptions	: convert given Temperature into voltage according to NIST
 **@Parameters		: fTempValue: Temp value to convert into Voltage 
 **@Return				:	converted voltage 
 *****************************************************************************/
float fnTempToVoltage(float fTempValue)
{
	float fRetVoltage = 0.0f;
	uint8_t ucRow = 0;
	int uiLookUpTableSize = (sizeof(rgfLookUp)/sizeof(rgfLookUp[0]));
	for(ucRow = 0; ucRow < uiLookUpTableSize; ucRow++)
	{
		if(rgfLookUp[ucRow][T_COL] == fTempValue)
		{
			fRetVoltage = rgfLookUp[ucRow][V_COL];
		}
		else if((rgfLookUp[ucRow][T_COL] < fTempValue) && 
			      (rgfLookUp[ucRow + 1][T_COL] > fTempValue))
		{
			/*here n = ucRow +1 and n-1 = ucRow*/
			/*Interpolation equation*/
			/*V[n-1] + ((V[n] - V[n-1]) * ((Value - T[n-1]) / (T[n] - T[n-1])))*/
			fRetVoltage = (rgfLookUp[ucRow][V_COL] + 
										((rgfLookUp[ucRow + 1][V_COL] - rgfLookUp[ucRow][V_COL]) *
										 ((fTempValue - rgfLookUp[ucRow][T_COL]) / 
										 (rgfLookUp[ucRow + 1][T_COL] - rgfLookUp[ucRow][T_COL]))));
		}
	}
	return (fRetVoltage);
}
/******************************************************************************
 **@Function 			: fnVoltageToTemp
 **@Descriptions	: Convert Given Voltage to Temperature according to NIST
 **@Parameters		: fTempValue: voltage value to convert into Tempreture
 **@Return				:	converted temp in float 
 *****************************************************************************/
float fnVoltageToTemp(float fVoltage)
{
	float fRetTempValue = 0.0f;
	uint8_t ucRow = 0;
	
	int uiLookUpTableSize = (sizeof(rgfLookUp)/sizeof(rgfLookUp[0]));
	
	for(ucRow = 0; ucRow < uiLookUpTableSize; ucRow++)
	{
		if(rgfLookUp[ucRow][V_COL] == fVoltage)
		{
			fRetTempValue = rgfLookUp[ucRow][T_COL];
		}
		else if((rgfLookUp[ucRow][V_COL] < fVoltage) && 
						(rgfLookUp[ucRow + 1][V_COL] > fVoltage))
		{
			/*here n = ucRow +1 and n-1 = ucRow*/
			/*Interpolation equation*/
			/*T[n-1] + ((T[n] - T[n-1]) * ((Value - V[n-1]) / (V[n] - V[n-1])))*/
			fRetTempValue = (rgfLookUp[ucRow][T_COL] + /*Tn-1*/
											((rgfLookUp[ucRow + 1][T_COL] - rgfLookUp[ucRow][T_COL]) * 
											 ((fVoltage - rgfLookUp[ucRow][V_COL]) / 
											 (rgfLookUp[ucRow + 1][V_COL] - rgfLookUp[ucRow][V_COL]))));
		}
	}
	return fRetTempValue;
}
/******************************************************************************
 **@Function 	 	: fnTemprature_Process
 **@Description	: this funtion will read the TC and take the action
 **@Parameters	: ucTCNum: processing TC no. between 0 to 3 only
 **@Return			: None
 ****************************************************************************/
void fnTemprature_Process(uint8_t ucTCNum)
{
#ifdef TEST_DBG
	char rgcTemp[150];
#endif
	
	/*subtract the previous value from the sum and add new temp to it*/
	if((fabs(stTC.stRead[ucTCNum].fCurrentTemp) <= MAX_TEMP_READING_RANGE) && 
		 (fabs(stTC.stRead[ucTCNum].fCurrentTemp) >= MIN_TEMP_READING_RANGE))
	{
		stTC.stRead[ucTCNum].fSum -= 
					stTC.stRead[ucTCNum].rgfTempBuff[stTC.stRead[ucTCNum].ucTraverse];
		stTC.stRead[ucTCNum].rgfTempBuff[stTC.stRead[ucTCNum].ucTraverse] = 
																						stTC.stRead[ucTCNum].fCurrentTemp;
		stTC.stRead[ucTCNum].fSum += stTC.stRead[ucTCNum].fCurrentTemp;
		stTC.stRead[ucTCNum].ucTraverse++;
		
		if(TC_WINDOW_SIZE <= stTC.stRead[ucTCNum].ucTraverse)
		{
			stTC.stRead[ucTCNum].ucTraverse = 0;
			stTC.stRead[ucTCNum].bWindowOverflow = SET;
		}
		
		stTC.stRead[ucTCNum].ucTCFault = 0;	
	}
	else /* Thermocouple fault */
	{
		if(MAX_TC_FAULT_COUNT > stTC.stRead[ucTCNum].ucTCFault)
		{
			stTC.stRead[ucTCNum].ucTCFault++;
		}
	}
		
	
	/*Averaging*/
	if(SET == stTC.stRead[ucTCNum].bWindowOverflow)
	{
		stTC.stRead[ucTCNum].fCurrentTemp = stTC.stRead[ucTCNum].fSum /
				TC_WINDOW_SIZE;			
	}
	else
	{
		stTC.stRead[ucTCNum].fCurrentTemp = stTC.stRead[ucTCNum].fSum / 
			stTC.stRead[ucTCNum].ucTraverse;	
	}
	#ifdef OPER_MODE_DBG
		sprintf(rgcTemp, "Current Temp[%d] Send = %0.4f", ucTCNum, 
			stTC.stRead[ucTCNum].fCurrentTemp); 
		DEBUG_PRINTF("%s\r\n", rgcTemp); 
	#endif
		
	/*Transmit data over CAN*/
	if(MAX_TC_FAULT_COUNT <= stTC.stRead[ucTCNum].ucTCFault)
	{
		/* Invalid data to HMI and SCU*/
		fnDataMsg_Tx(INVALID_FLOAT_VALUE, TEMPERATURE, ucTCNum);
		
		/*RESET Window again for New data*/
		stTC.stRead[ucTCNum].ucTraverse = 0;
		stTC.stRead[ucTCNum].fSum = 0.0f;
		memset(stTC.stRead[ucTCNum].rgfTempBuff, 0, 4*TC_WINDOW_SIZE);
		stTC.stRead[ucTCNum].bWindowOverflow = RESET;
		stTC.stRead[ucTCNum].ucTraverse = 0;
	}
	else 
	{		
		/* Tx Current temp to the Respective Module*/
		fnDataMsg_Tx(stTC.stRead[ucTCNum].fCurrentTemp, TEMPERATURE, ucTCNum);
	}
	
	#ifdef TEST_DBG
		memset(rgcTemp, '\0', sizeof(rgcTemp));
		sprintf(rgcTemp, "HEATER TEMP = %0.2f", stTC.stRead[ucTCNum].fCurrentTemp);
		DEBUG_PRINTF("%s\r\n", rgcTemp); 
	#endif
	
	/***************************** Thershold Temp Check **********************/		
	if(RETURN_LINE != ucTCNum)
	{
		/*Open circuit action*/
		if((RESET == stTC.stProcess[ucTCNum].bTCFaultFlag) && 
			 (INVALID_FLOAT_VALUE == stTC.stRead[ucTCNum].fCurrentTemp))
		{
			stTC.stProcess[ucTCNum].bTempHighTHFaultFlag = RESET;
			stTC.stProcess[ucTCNum].bTempLowTHFaultFlag = RESET;
			stTC.stProcess[ucTCNum].bTempInRangeFlag = RESET;
			stTC.stProcess[ucTCNum].bTCFaultFlag = SET; 
			
			/*ALL LED OFF*/
			uiIOExpander2 &= ~TC_LED_VALUE(ucTCNum, RED);
			uiIOExpander2 &= ~TC_LED_VALUE(ucTCNum, BLUE);
			uiIOExpander2 &= ~TC_LED_VALUE(ucTCNum, GREEN);
			
			stTC.stProcess[ucTCNum].ucNormalTempCnt = 0;
			stTC.stProcess[ucTCNum].ucLowTHFaultCnt = 0;
			stTC.stProcess[ucTCNum].ucHighTHFaultCnt = 0;
		}
		/* High Thershold fault */
		else if(stTC.stRead[ucTCNum].fCurrentTemp >= 
						stTC.stProcess[ucTCNum].fTempHighTH)
		{
			stTC.stProcess[ucTCNum].ucNormalTempCnt = 0;
			stTC.stProcess[ucTCNum].ucLowTHFaultCnt = 0;
			if(stTC.stProcess[ucTCNum].ucHighTHFaultCnt < HIGH_TH_FAULT_COUNT)
			{
				stTC.stProcess[ucTCNum].ucHighTHFaultCnt++;
			}
			
			if((HIGH_TH_FAULT_COUNT == stTC.stProcess[ucTCNum].ucHighTHFaultCnt) && 
				 (RESET == stTC.stProcess[ucTCNum].bTempHighTHFaultFlag))
			{
				stTC.stProcess[ucTCNum].ucHighTHFaultCnt = 0;
				stTC.stProcess[ucTCNum].bTempHighTHFaultFlag = SET;
				stTC.stProcess[ucTCNum].bTempLowTHFaultFlag = RESET;
				stTC.stProcess[ucTCNum].bTempInRangeFlag = RESET;
				fnFaultMsg_Tx(HIGH_THERSHOLD, TEMPERATURE, ucTCNum);
				
				/*RED Led ON*/
				uiIOExpander2 |=  TC_LED_VALUE(ucTCNum, RED);
				uiIOExpander2 &= ~TC_LED_VALUE(ucTCNum, BLUE);
				uiIOExpander2 &= ~TC_LED_VALUE(ucTCNum, GREEN);
				#ifdef HEATER_DBG
					DEBUG_PRINTF("in high Tempthershold fault: %d\r\n", ucFaultType);
				#endif
			}			
		}
		/*In range*/
		else if((stTC.stRead[ucTCNum].fCurrentTemp < 
						stTC.stProcess[ucTCNum].fTempHighTH) &&
						(stTC.stRead[ucTCNum].fCurrentTemp >
						stTC.stProcess[ucTCNum].fTempLowTH))
		{
			if(stTC.stProcess[ucTCNum].ucNormalTempCnt < IN_RANGE_CHECK_COUNT)
			{
				stTC.stProcess[ucTCNum].ucNormalTempCnt++;
			}
			stTC.stProcess[ucTCNum].ucHighTHFaultCnt = 0;
			stTC.stProcess[ucTCNum].ucLowTHFaultCnt = 0;
			
			if((IN_RANGE_CHECK_COUNT == stTC.stProcess[ucTCNum].ucNormalTempCnt) && 
				 (RESET == stTC.stProcess[ucTCNum].bTempInRangeFlag))
			{
				stTC.stProcess[ucTCNum].ucNormalTempCnt = 0;
				stTC.stProcess[ucTCNum].bTempHighTHFaultFlag = RESET;
				stTC.stProcess[ucTCNum].bTempLowTHFaultFlag = RESET;
				stTC.stProcess[ucTCNum].bTempInRangeFlag = SET;
				uiIOExpander2 &= ~TC_LED_VALUE(ucTCNum, RED);
				uiIOExpander2 &= ~TC_LED_VALUE(ucTCNum, BLUE);
				uiIOExpander2 |= TC_LED_VALUE(ucTCNum, GREEN);
				#ifdef HEATER_DBG
					DEBUG_PRINTF("Temp is in range\r\n");
				#endif
			}
		}
		/* low thershold fault */
		else if(fabs(stTC.stRead[ucTCNum].fCurrentTemp) <= 
						stTC.stProcess[ucTCNum].fTempLowTH) 
		{
			if(stTC.stProcess[ucTCNum].ucLowTHFaultCnt < LOW_TH_FAULT_COUNT)
			{
				stTC.stProcess[ucTCNum].ucLowTHFaultCnt++;
			}
			stTC.stProcess[ucTCNum].ucNormalTempCnt = 0;
			stTC.stProcess[ucTCNum].ucHighTHFaultCnt = 0;
			if((LOW_TH_FAULT_COUNT == stTC.stProcess[ucTCNum].ucLowTHFaultCnt) &&
				 (RESET == stTC.stProcess[ucTCNum].bTempLowTHFaultFlag))
			{
				stTC.stProcess[ucTCNum].ucLowTHFaultCnt = 0;
				stTC.stProcess[ucTCNum].bTempLowTHFaultFlag = SET;
				stTC.stProcess[ucTCNum].bTempHighTHFaultFlag = RESET;
				stTC.stProcess[ucTCNum].bTempInRangeFlag = RESET;
				StatusFlag.stStatusFlag.bWarnningFlag = SET;/*Warnning*/
				/* Fault tx to SCU and HMI */
				fnFaultMsg_Tx(LOW_THERSHOLD, TEMPERATURE, ucTCNum);
				/*LED ON, OFF*/
				uiIOExpander2 &= ~TC_LED_VALUE(ucTCNum, RED);
				uiIOExpander2 |= TC_LED_VALUE(ucTCNum, BLUE);
				uiIOExpander2 &= ~TC_LED_VALUE(ucTCNum, GREEN);
				
				#ifdef HEATER_DBG
					DEBUG_PRINTF("Temp Low thershold fault\r\n");
				#endif
			}
		}
		/* condition for temp increases more than set temp */
		if(fabs(stTC.stRead[ucTCNum].fCurrentTemp) > 
			(stTC.stProcess[ucTCNum].fSetTemp + stTC.stProcess[ucTCNum].fTempHystPos))
		{
			fnHeater_ONOFF(ucTCNum, HEATER_OFF);
			 
			#ifdef HEATER_DBG
				DEBUG_PRINTF("Heater OFF\r\n"); 
			#endif
		}
		/* condition for temp decreases less than set temp */	
		else if(fabs(stTC.stRead[ucTCNum].fCurrentTemp) < 
			(stTC.stProcess[ucTCNum].fSetTemp - stTC.stProcess[ucTCNum].fTempHystNeg))
		{
			fnHeater_ONOFF(ucTCNum, HEATER_ON);

			#ifdef HEATER_DBG
					DEBUG_PRINTF("Heater ON\r\n"); 
			#endif
		}
	}
}


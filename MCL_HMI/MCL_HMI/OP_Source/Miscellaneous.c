/******************************************************************************
**@file    Miscellaneous.c
* 
**@brief   This file contains all the miscellaneous functions
******************************************************************************
*/    
/* ----------------------------------------------------------------------------
*                           Includes
* ----------------------------------------------------------------------------
*/
#include "System_Header.h"
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
	uint8_t ucIOExpader1Low = 0;								/*Variable for Port 0 of I/O Expande 1*/
	uint8_t ucIOExpader1High = 0;								/*Variable for Port 1 of I/O Expande 1*/
	uint16_t uiSytemDateTime = 0;
	uint8_t   ucTimeZoneOffset=0;
	uint16_t uiLetterX = 0;
	uint16_t uiLetterY = 0;
	uint16_t uiLetterStartPos = 0;

	uint8_t ucpDisplayLetter = 0;
	uint8_t ulirgStoreLetter[30];
	uint8_t *ucAlphaNum[3][13] = {	{"Q","W","E","R","T","Y","U","I","O","P","7","8","9"},
																	{"A","S","D","F","G","H","J","K","L","-","4","5","6"},
																	{"Z","X","C","V","B","N","M","<",".","0","1","2","3"}	};
	uint8_t ucLetterPos = 0;			
	uint8_t ucScreenPatternNumber = 0;
	uint8_t ucPatternNumberCount = 0;
	uint8_t ucSwitchPressFlag = 0;
		
	uint8_t ucPresSetVariableCounter = 1;
	uint8_t ucTempSetVariableCounter = 1;
	uint8_t ucPumpSetVariableCounter = 1;
	uint8_t ucJobSetVariableCounter = 1;
	uint8_t ucTimerSetVariableCounter = 1;
	uint8_t ucStallSetVariableCounter = 1;
	uint8_t ucOtherSetVariableCounter = 1;
	/*New Changes*/
																	
	uint8_t ucLiquidLSetVariableCounter1 = 1;
	uint8_t ucHystSetVariableCounter = 1;
																	
	uint8_t ucVCMPeripheralSetCounter = 1;
	uint8_t ucHCMPeripheralSetCounter = 1;
	uint8_t ucOPMMPeripheralSetCounter = 1;
	uint8_t ucSCUPeripheralSetCounter = 1;
																	
	uint8_t ucResetVariableCounter = 1;
	uint8_t ucDiagModuleVariable = 1;

	uint8_t ucRedColor = 0;
	uint8_t ucGreenColor = 0;
	uint8_t ucBlueColor = 0;
	uint16_t uiLetter = 0;

	uint8_t ucLastProcessDone = 0;
	uint8_t ucMaterialDataReceived = 0;
	uint8_t ucrgStoredStatusMessage[24][75];
	uint8_t ucStatusCountLimit = 0;
	uint8_t ucrgStoredAlarmMessage[24][75];
	uint8_t rgucModule[24];
	uint8_t rgucAlarmTpe[24];

	uint8_t ucAlarmCountLimit = 0;
	uint8_t ucStatusMessageCounter = 0;
	uint8_t ucAlarmMessageCounter = 0;
/***********change4***********************/
	uint8_t ucrgStoredMemoryAlarmMessage[24][75];
	uint8_t ucMemoryAlarmCountLimit = 0;
	uint8_t ucMemoryAlarmMessageCounter = 0;
	/***************************************/
	uint8_t rgucSupervisorName[22];
	uint8_t rgucCoatingMaterial[22];
	uint8_t rgucBaseMatBatchNumber[22];
	uint8_t rgucHardMatBatchNumber[22];
	uint8_t rgucJobName[22];
	/*change7*/
	uint8_t rgucJobId[22];
	/*change3*/
	uint8_t rgucSSID[22];
	uint8_t rgucPass[22];
	uint8_t ucETHIPSetVariableCounter = 1;
	uint8_t ucWIFIIPSetVariableCounter = 1;
	float fUserInput = 0.0f;
	int16_t iUserInput = 0;
	uint8_t ucUserInput = 0;
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
**@Function		 	:  fnAlarmMessage_Clear()
**@Descriptions	:	 Function to clear the alarm message
**@parameters		:  none
**@return				:  none
*****************************************************************************/
/*change4*/
void fnMemory_Alarm_Message_Clear(void)
{
	uint8_t ucCnt = 0;

	for(ucCnt = 0; ucCnt < ucMemoryAlarmCountLimit; ucCnt++)
	{
		memset(ucrgStoredMemoryAlarmMessage[ucCnt],'\0',strlen((const char *)ucrgStoredMemoryAlarmMessage[ucCnt]));
	}
	ucMemoryAlarmCountLimit = 0;
		}
	

/*****************************************************************************
**@Function		 	:  fnMemory_Alarm_Message_Store()
**@Descriptions	:	 Function to Store status message
**@parameters		:  ucrgStr : string passed to store in buffer
**@return				:  none
*****************************************************************************/
/*change4*/
void fnMemory_Alarm_Message_Store(uint8_t ucrgStr[50])
{
	uint8_t ucCnt = 0;
	
	/*change5*/	
	uint8_t ucrgStr1[75];
	memset(ucrgStr1,'\0',sizeof(ucrgStr1));
	strcat((char *)ucrgStr1,(const char *)stDisplayDateTime.ucDD);
	strcat((char *)ucrgStr1,(const char *)SLASH);		
	strcat((char *)ucrgStr1,(const char *)stDisplayDateTime.ucMM);
	strcat((char *)ucrgStr1,(const char *)SLASH);	
	strcat((char *)ucrgStr1,(const char *)stDisplayDateTime.ucYY);
	strcat((char *)ucrgStr1,(const char *)SPAC);	
	strcat((char *)ucrgStr1,(const char *)stDisplayDateTime.ucHR);
	strcat((char *)ucrgStr1,(const char *)COLON);		
	strcat((char *)ucrgStr1,(const char *)stDisplayDateTime.ucMIN);
	strcat((char *)ucrgStr1,(const char *)COLON);	
	strcat((char *)ucrgStr1,(const char *)stDisplayDateTime.ucSEC);
	strcat((char *)ucrgStr1,(const char *)SPAC);
	strcat((char *)ucrgStr1,(const char *)ucrgStr);
	
	for(ucCnt = ucMemoryAlarmCountLimit; ucCnt > 0; ucCnt--)
	{
		memmove(ucrgStoredMemoryAlarmMessage[ucCnt],ucrgStoredMemoryAlarmMessage[ucCnt - 1],strlen((const char *)ucrgStoredMemoryAlarmMessage[ucCnt - 1])+1);
	}
	//memcpy(ucrgStoredStatusMessage[0],ucrgStr,strlen((const char *)ucrgStr)+1);
	/*change5*/
	memcpy(ucrgStoredMemoryAlarmMessage[0],ucrgStr1,strlen((const char *)ucrgStr1)+1);
	if ( 23 > ucMemoryAlarmCountLimit )
		ucMemoryAlarmCountLimit++;
}

/*****************************************************************************
**@Function		 	:  fnStatusMessage_Store()
**@Descriptions	:	 Function to Store status message
**@parameters		:  ucrgStr : string passed to store in buffer
**@return				:  none
*****************************************************************************/
void fnStatusMessage_Store(uint8_t ucrgStr[50])
{
	uint8_t ucCnt = 0;
	
	/*change5*/	
	uint8_t ucrgStr1[75];
	memset(ucrgStr1,'\0',sizeof(ucrgStr1));
	strcat((char *)ucrgStr1,(const char *)stDisplayDateTime.ucDD);
	strcat((char *)ucrgStr1,(const char *)SLASH);		
	strcat((char *)ucrgStr1,(const char *)stDisplayDateTime.ucMM);
	strcat((char *)ucrgStr1,(const char *)SLASH);	
	strcat((char *)ucrgStr1,(const char *)stDisplayDateTime.ucYY);
	strcat((char *)ucrgStr1,(const char *)SPAC);	
	strcat((char *)ucrgStr1,(const char *)stDisplayDateTime.ucHR);
	strcat((char *)ucrgStr1,(const char *)COLON);		
	strcat((char *)ucrgStr1,(const char *)stDisplayDateTime.ucMIN);
	strcat((char *)ucrgStr1,(const char *)COLON);	
	strcat((char *)ucrgStr1,(const char *)stDisplayDateTime.ucSEC);
	strcat((char *)ucrgStr1,(const char *)SPAC);
	strcat((char *)ucrgStr1,(const char *)ucrgStr);
	
	for(ucCnt = ucStatusCountLimit; ucCnt > 0; ucCnt--)
	{
		memmove(ucrgStoredStatusMessage[ucCnt],ucrgStoredStatusMessage[ucCnt - 1],strlen((const char *)ucrgStoredStatusMessage[ucCnt - 1])+1);
	}
	//memcpy(ucrgStoredStatusMessage[0],ucrgStr,strlen((const char *)ucrgStr)+1);
	/*change5*/
	memcpy(ucrgStoredStatusMessage[0],ucrgStr1,strlen((const char *)ucrgStr1)+1);
	if ( 23 > ucStatusCountLimit )
		ucStatusCountLimit++;
}

/*****************************************************************************
**@Function		 	:  fnAlarmMessage_Store()
**@Descriptions	:	 Function to Store Alarm message
**@parameters		:  ucrgStr : string passed to store in buffer
									 ucModuleNum : Module number for which the Fault message is stored
**@return				:  none
*****************************************************************************/
void fnAlarmMessage_Store(uint8_t ucrgStr[50],uint8_t ucModuleNum,uint8_t ucAlarmtype)
{
	uint8_t ucCnt = 0;
	/*change5*/	
	uint8_t ucrgStr1[75];
	memset(ucrgStr1,'\0',sizeof(ucrgStr1));
	strcat((char *)ucrgStr1,(const char *)stDisplayDateTime.ucDD);
	strcat((char *)ucrgStr1,(const char *)SLASH);		
	strcat((char *)ucrgStr1,(const char *)stDisplayDateTime.ucMM);
	strcat((char *)ucrgStr1,(const char *)SLASH);	
	strcat((char *)ucrgStr1,(const char *)stDisplayDateTime.ucYY);
	strcat((char *)ucrgStr1,(const char *)SPAC);	
	strcat((char *)ucrgStr1,(const char *)stDisplayDateTime.ucHR);
	strcat((char *)ucrgStr1,(const char *)COLON);		
	strcat((char *)ucrgStr1,(const char *)stDisplayDateTime.ucMIN);
	strcat((char *)ucrgStr1,(const char *)COLON);	
	strcat((char *)ucrgStr1,(const char *)stDisplayDateTime.ucSEC);
	strcat((char *)ucrgStr1,(const char *)SPAC);
	strcat((char *)ucrgStr1,(const char *)ucrgStr);	
	for(ucCnt = ucAlarmCountLimit; ucCnt > 0; ucCnt--)
	{
		memmove(ucrgStoredAlarmMessage[ucCnt],ucrgStoredAlarmMessage[ucCnt - 1],strlen((const char *)ucrgStoredAlarmMessage[ucCnt - 1])+1);
		rgucModule[ucCnt] = rgucModule[ucCnt -1];
		rgucAlarmTpe[ucCnt] = rgucAlarmTpe[ucCnt - 1];
	}
	memset(ucrgStoredAlarmMessage[0],'\0',strlen((const char *)ucrgStr)+1);
	memcpy(ucrgStoredAlarmMessage[0],ucrgStr1,strlen((const char *)ucrgStr1)+1);
	rgucModule[0] = ucModuleNum;
	rgucAlarmTpe[0] = ucAlarmtype;
	
	if ( 23 > ucAlarmCountLimit )
			ucAlarmCountLimit++;
}

/*****************************************************************************
**@Function		 	:  fnAlarmMessage_Clear()
**@Descriptions	:	 Function to clear the alarm message
**@parameters		:  ucModuleNum : Module number for which the Fault message is stored
**@return				:  none
*****************************************************************************/
void fnAlarmMessage_Clear(uint8_t ucModuleNum,uint8_t ucAlarmType)
{
	uint8_t ucCnt,ucCnt1 = 0;

	for(ucCnt = 0; ucCnt < ucAlarmCountLimit; ucCnt++)
	{
		if ( rgucModule[ucCnt] == ucModuleNum )
		{
			if ( rgucAlarmTpe[ucCnt] == ucAlarmType )
			{
				for(ucCnt1 = ucCnt ; ucCnt1 < ucAlarmCountLimit; ucCnt1++)
				{
					memmove(ucrgStoredAlarmMessage[ucCnt1], ucrgStoredAlarmMessage[ucCnt1 + 1], strlen((const char *)ucrgStoredAlarmMessage[ucCnt1 + 1])+1);
					rgucModule[ucCnt1] = rgucModule[ucCnt1 + 1];
					rgucAlarmTpe[ucCnt1] = rgucAlarmTpe[ucCnt1 + 1];
				}
				memset(ucrgStoredAlarmMessage[ucCnt1],'\0',strlen((const char *)ucrgStoredAlarmMessage[ucCnt1])+1);
				
				ucAlarmCountLimit--;	
				if ( 0 == ucAlarmCountLimit )
				{
					memset(ucrgStoredAlarmMessage[0],'\0',strlen((const char *)ucrgStoredAlarmMessage[0])+1);
				}
				ucCnt--;
			}
		}
	}	
}

/*****************************************************************************
**@Function		 	:  fnSelfDiagnostics_Test()
**@Descriptions	:	 Function For Self Diagnostic
**@parameters		:  none
**@return				:  none
*****************************************************************************/
void fnSelfDiagnostics_Test(void)
{
	uint8_t ucPin;
	uint16_t uiSw = 0;
	
	uint8_t ucNVSRAMData = 0;
	uint8_t ucvalue = 'F';
	
	uint32_t uliNofSw;

	uint8_t ucLoop = 0;
	/* Diagnostic of swithces */
	memset(stTx1Message.Data,0,sizeof(stTx1Message.Data));
	stTx1Message.DLC = 2; 
	
	for (uliNofSw = 0 ; uliNofSw < NUMBER_OF_SWITCHES ; uliNofSw++)
	{
		ucPin = GPIO_ReadInputDataBit(switch_map[uliNofSw].GPIOx, switch_map[uliNofSw].GPIO_Pin);
		uiSw = (uiSw << 1) | (uint16_t)((ucPin ^ 1) & 1);		
	}
	
	if(ALL_SW_AT_DEFAULT != uiSw)
	{
		StatusFlag.stStatusFlag.bDiagnostic_Status = SET; /* this indicate that Selfdiagnostic of OP failed*/
		/*change2*/
		StatusFlag.stStatusFlag.bDiagnostic_SW_Status = SET; /* this indicate that Selfdiagnostic of OP failed*/
		stTx1Message.Data[1] |= SW_DIAGNOSTIC_FAIL;
		#ifdef GENERAL_DBG
			DEBUG_PRINTF("SW Diag FAIL\r\n");
		#endif
	}
	
	/* NVSRAM Diagnostic*/
	 NV_Write(NVSRAM_TEST_ADDRESS, &ucvalue, NVSRAM_BYTE_COUNT);

	 NV_Read(NVSRAM_TEST_ADDRESS, &ucNVSRAMData, NVSRAM_BYTE_COUNT);

	if(ucvalue != ucNVSRAMData)
	{
		StatusFlag.stStatusFlag.bDiagnostic_Status = SET; /* this indicate that Self diagnostic of OP failed*/
		/*change2*/
		StatusFlag.stStatusFlag.bDiagnostic_NVSRAM_Status = SET; /* this indicate that Selfdiagnostic of OP failed*/
		stTx1Message.Data[1] |= NVSRAM_DIAGNOSTIC_FAIL;
		#ifdef GENERAL_DBG
			DEBUG_PRINTF("NVSRAM Diagnostic Fail\r\n");
		#endif
	}
	else 
	{
		#ifdef GENERAL_DBG
			DEBUG_PRINTF("NVSRAM Diagnostic Pass\r\n");
		#endif
	}	
	
	if(SET == StatusFlag.stStatusFlag.bDiagnostic_Status)
	{
		/* Diagnostic Fails*/
		stTx1Message.Data[0] = DIAGNOSTIC_FAIL;
		#ifdef GENERAL_DBG
			DEBUG_PRINTF("OP Diag Fail\r\n");
		#endif
		
		stDiagStatus.bHMI_DiagStatus = FALSE;
		//fnStatusMessage_Store(HMI_DIAGNOSTIC_FAIL);       
	//	ucStatusMessageCounter++;
		
		/* OFF Board Status LED */
		fnLED_TurnONOFF (PORT_LED_ERR_GREEN, Bit_RESET,PIN_LED_ERR_GREEN);
	}
	else
	{
		/* Diagnostic Pass*/
		stTx1Message.Data[0] = DIAGNOSTIC_PASS;
		
		#ifdef GENERAL_DBG
			DEBUG_PRINTF("OP Diag Pass\r\n");
		#endif
		
		stDiagStatus.bHMI_DiagStatus = TRUE;
		//fnStatusMessage_Store(HMI_DIAGNOSTIC_PASS);
	//	ucStatusMessageCounter++;
		
		/* ON Board Status LED GREEN*/
		fnLED_TurnONOFF (PORT_LED_ERR_GREEN, Bit_SET,PIN_LED_ERR_GREEN);
		/* off Board Status LED RED*/
		fnLED_TurnONOFF (PORT_LED_ERR_RED, Bit_RESET,PIN_LED_ERR_RED);
		
		/* OFF CAN Status LED RED*/
		fnLED_TurnONOFF (PORT_LED_COM_RED, Bit_RESET,PIN_LED_COM_RED);
		/* ON CAN Status LED RED*/
		fnLED_TurnONOFF (PORT_LED_COM_GREEN, Bit_SET,PIN_LED_COM_GREEN);
	}
	stDiagStatus.bHMI_DiagStatusSent = TRUE;
	/*Send Diagnostic status**/
	fnCANMessage_Send(STATUS_HMI_DIAGNOSTIC, MSG_TYPE_STATUS, SCU_ID);
	
	/*11thMarch-Sending code version after diagnostic completed*/
	memset(stTx1Message.Data,0,sizeof(stTx1Message.Data));
	stTx1Message.DLC = 5;
	
	stTx1Message.Data[0] = MODULE_CODE;
	SplitByte.fFloat = CODE_VERSION;
	for(ucLoop = 1; ucLoop < stTx1Message.DLC; ucLoop++)
	{
		stTx1Message.Data[ucLoop] = SplitByte.ucChar[ucLoop - 1];
	}
	
	fnCANMessage_Send(STATUS_SOFTWARE_VERSION, MSG_TYPE_STATUS, SCU_ID);
}

/*****************************************************************************
**@Function		 	:  fnEStop_Check()
**@Descriptions	:	 EStop Function
**@parameters		:  none
**@return				:  none
*****************************************************************************/
void fnEStop_Check(void)
{
//	uint8_t ucEStopPinStatus = 0;
//	
//	ucEStopPinStatus = GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_7);
//	ucEStopPinStatus = (ucEStopPinStatus << 1 ) | GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_6);
//	
//	if (( ESTOP_PRESSED == ucEStopPinStatus ) && ( FALSE == StatusFlag.stStatusFlag.bEstopPressed ))
//	{
//		#ifdef GENERAL_DBG
//			DEBUG_PRINTF("ESTOP Pressed\r\n");
//		#endif
//		
//		GPIO_WriteBit(GPIOE, GPIO_Pin_4, Bit_SET);
//		
//		ucSwitchPressFlag = TRUE;
//		fnStatusMessage_Store(ESTOP_PRESSED_STRING);
//		ucStatusMessageCounter++;
//		
//		if ( FALSE == StatusFlag.stStatusFlag.bEstopPressed )
//		{
//			StatusFlag.stStatusFlag.bSystemErrorFlag = TRUE;
//			
//			StatusFlag.stStatusFlag.bEstopPressed = TRUE;
//			memset(stTx1Message.Data, 0 , sizeof(stTx1Message.Data));
//			stTx1Message.DLC = 0;

//			fnCANMessage_Send(BCAST_ESTOP, MSG_TYPE_BROADCAST, ALL_SLAVE_DEVICE);
//		}
//	}
//	else if (( ESTOP_RELEASED == ucEStopPinStatus ) && ( TRUE == StatusFlag.stStatusFlag.bEstopPressed ))
//	{
//		ucSwitchPressFlag = TRUE;
//		StatusFlag.stStatusFlag.bEstopPressed = FALSE;
//		fnStatusMessage_Store(ESTOP_RELEASED_STRING);
//		ucStatusMessageCounter++;
//		
//		fnClearWarnings();
//		fnClearErrors();
//		#ifdef GENERAL_DBG
//			DEBUG_PRINTF("ESTOP Released\r\n");
//		#endif
//	}
}

/*****************************************************************************
**@Function		 	:  fnMetricToImperial_Conv()
**@Descriptions	:	 Metric to Imperial uint conversion
**@parameters		:  fCurrentData : data to be converted
									 ucTypeOfData : Pressure/Temperaure 
**@return				:  converted value
*****************************************************************************/
float fnMetricToImperial_Conv(float fCurrentData,uint8_t ucTypeOfData)
{
	if ( PRESSURE_VALUE == ucTypeOfData )
	{
		return (float)BAR_TO_PSI(fCurrentData);
	}
	else if ( TEMPERATURE_VALUE == ucTypeOfData )
	{
		return (float)CELSIUS_TO_FAHRENHEIT(fCurrentData);
		//(fCurrentData * (DIGIT_9/DIGIT_5))+DIGIT_32;
	}
	else if ( MM_TO_INCH == ucTypeOfData )
	{
		return (float)(CONVERT_MM_TO_INCH(fCurrentData));
	}
	else if ( CM_TO_INCH == ucTypeOfData )
	{
		return (float)(CONVERT_CM_TO_INCH(fCurrentData));
	}
	else if (ML_TO_OZ == ucTypeOfData )
	{
		return (float)(CONVERT_ML_TO_OZ(fCurrentData));
	}
	else if (MM2_TO_INCH2 == ucTypeOfData )
	{
		return (float)(CONVERT_MM2_TO_INCH2(fCurrentData));
	}
	else if (MM3_TO_OZ == ucTypeOfData )
	{
		return (float)(CONVERT_MM3_TO_OZ(fCurrentData));
	}
	return 0.0f;
}

/*****************************************************************************
**@Function		 	:  fnImperialToMetric_Conv()
**@Descriptions	:	 Imperial to Metric  uint conversion
**@parameters		:  fCurrentData : data to be converted
									 ucTypeOfData : Pressure/Temperaure 
**@return				:  converted value
*****************************************************************************/
float fnImperialToMetric_Conv(float fCurrentData,uint8_t ucTypeOfData)
{
	if ( PRESSURE_VALUE == ucTypeOfData )
	{
		return (float)PSI_TO_BAR(fCurrentData);
	}
	else if ( TEMPERATURE_VALUE == ucTypeOfData )
	{
		return (float)FAHRENHEIT_TO_CELSIUS(fCurrentData);
		//((fCurrentData - DIGIT_32)*(DIGIT_5/DIGIT_9));
	}
	else if ( INCH_TO_MM == ucTypeOfData )
	{
		return (float)(CONVERT_INCH_TO_MM(fCurrentData));
	}
	else if ( INCH_TO_CM == ucTypeOfData )
	{
		return (float)(CONVERT_INCH_TO_CM(fCurrentData));
	}
	else if ( INCH2_TO_MM2 == ucTypeOfData )
	{
		return (float)(CONVERT_INCH2_TO_MM2(fCurrentData));
	}
	return 0.0f;
}

/*****************************************************************************
**@Function		 	:  fnBoardStatusRedLed_Blink
**@Descriptions	:  Blink the RED LED if CAN Fault Occurs
**@parameters		:  none
**@return				:  none
*****************************************************************************/
void fnBoardStatusRedLed_Blink(void)
{
	if(SET == StatusFlag.stStatusFlag.bAnyFault)	 
	{
		/* OFF Board Status  Led GREEN */
		fnLED_TurnONOFF (PORT_LED_ERR_GREEN, Bit_RESET,PIN_LED_ERR_GREEN);

		if ( STATUS_LED_BLINK_COUNT > uiAlarm1LEDBlinkCount )
		{
			/* ON Board Status RED LED*/
			fnLED_TurnONOFF (PORT_LED_ERR_RED, Bit_SET,PIN_LED_ERR_RED);
		}
		else if ( STATUS_LED_BLINK_COUNT < uiAlarm1LEDBlinkCount )
		{
			/* OFF Board status RED LED*/
			fnLED_TurnONOFF (PORT_LED_ERR_RED, Bit_RESET,PIN_LED_ERR_RED); 
			
			/* RESET The Blinking LED COUNTER if greatere than 200msec */
			if( (2 * STATUS_LED_BLINK_COUNT) <= uiAlarm1LEDBlinkCount )
			{
				uiAlarm1LEDBlinkCount = 0;
			}
		}
	}

}

/*****************************************************************************
**@Function		 	:  fnAlarm1StatusRedLed_Blink
**@Descriptions	:  Blink the RED LED if any Fault Occurs
**@parameters		:  none
**@return				:  none
*****************************************************************************/
void fnAlarm1StatusRedLed_Blink(void)
{
	if(SET == StatusFlag.stStatusFlag.bCANFaultFlag)	 
	{
		/* OFF CAN Status LED GREEN*/
		fnLED_TurnONOFF (PORT_LED_COM_GREEN, Bit_RESET,PIN_LED_COM_GREEN);
				
		if ( STATUS_LED_BLINK_COUNT > ucLEDBlinkCount )
		{
			/* ON CAN status RED LED*/
			fnLED_TurnONOFF (PORT_LED_COM_RED, Bit_SET,PIN_LED_COM_RED);
		}
		else if ( STATUS_LED_BLINK_COUNT < ucLEDBlinkCount )
		{
			/* OFF CAN status RED LED*/
			fnLED_TurnONOFF (PORT_LED_COM_RED, Bit_RESET,PIN_LED_COM_RED);
			
			/* RESET The Blinking LED COUNTER if greatere than 200msec */
			if( (2 * STATUS_LED_BLINK_COUNT) <= ucLEDBlinkCount )
			{
				ucLEDBlinkCount = 0;
			}
		}
	}
}

/*****************************************************************************
**@Function		 	:  fnLED_TurnONOFF
**@Descriptions	:	 Psi to Bar conversion
**@parameters		:  none
**@return				:  none
*****************************************************************************/
void fnLED_TurnONOFF (GPIO_TypeDef *GPIOx,BitAction bLEDStatus,uint16_t bPinNumber)
{
	GPIO_WriteBit(GPIOx, bPinNumber, bLEDStatus);	
}

/*****************************************************************************
**@Function		 	:  fnWDT_Check()
**@Descriptions	:	 WDT check
**@parameters		:  none
**@return				:  none
*****************************************************************************/
void fnWDT_Check(void)
{
	if ( WDT_TIME_CHECK < uiWDTCount )
	{
		#ifdef GENERAL_DBG
			DEBUG_PRINTF("WDT reset Given\r\n");
		#endif
		uiWDTCount = 0;
		GPIO_WriteBit(WDTI_PORT,WDTI_PIN,Bit_SET);
		GPIO_WriteBit(WDTI_PORT,WDTI_PIN,Bit_RESET);
	}
}

/*****************************************************************************
**@Function		 	:  fnProcessFlags_Clear()
**@Descriptions	:	 clear process flags
**@parameters		:  none
**@return				:  none
*****************************************************************************/
void fnProcessFlags_Clear(void)
{
	StatusFlag.stStatusFlag.bFlushDone = FALSE;
	StatusFlag.stStatusFlag.bFlushStatusReceived = FALSE;
	StatusFlag.stStatusFlag.bFlushStatus = FALSE;
	StatusFlag.stStatusFlag.bInitFlushProcess = FALSE;							
	StatusFlag.stStatusFlag.bAutoFlush = FALSE;
	StatusFlag.stStatusFlag.bManualFlush = FALSE;
	
	StatusFlag.stStatusFlag.bPrimeDone = FALSE;
	StatusFlag.stStatusFlag.bPrimeStatusReceived = FALSE;
	StatusFlag.stStatusFlag.bPrimeStatus = FALSE;
	StatusFlag.stStatusFlag.bInitPrimeProcess = FALSE;							
	StatusFlag.stStatusFlag.bAutoPrime = FALSE;
	StatusFlag.stStatusFlag.bManualPrime = FALSE;
	
	StatusFlag.stStatusFlag.bAutoSpray = FALSE;
	StatusFlag.stStatusFlag.bSprayDone = FALSE;
	StatusFlag.stStatusFlag.bSprayStatus = FALSE;
	StatusFlag.stStatusFlag.bInitSprayProcess = FALSE;
	StatusFlag.stStatusFlag.bSprayStatusReceived = FALSE;
}
/*****************************************************************************
**@Function		 	:  fnCursorPosition_Setting()
**@Descriptions	:	 change cursor position
**@parameters		:  none
**@return				:  none
*****************************************************************************/
void fnCursorPosition_Setting(void)
{
	#ifdef GENERAL_DBG
		uint8_t  rgcTempCAN[100];
	#endif
	switch(uiCurrentScreen)
	{
		case SCREEN_KEYPAD:
		{
			switch(ucCurrentSwitch)
			{
				case SWITCH11:
				{
					ucCurrentSwitch = 0;
					
					if ( 149 != uiCursorXPosKP )
					{
						fnGr_FillRectangle(uiCursorXPosKP, uiCursorYPosKP, 26,28,RGB(255,255,255));
						uiCursorXPosKP -= 30;
								
								fnDisplay_Reg_Write(0x007, 0x0000);   /* Display Control1 */
								fnGr_FillRectangle(uiCursorXPosKP, uiCursorYPosKP, 26,28,RGB(106,255,102));
								fnGR_SCREEN1(SCREEN_PATTERN_11);
								fnDisplay_Reg_Write(0x007, 0x0173);
								uiCurrentScreen = SCREEN_KEYPAD;
					}
				}break;
				
				case SWITCH12:
				{
					ucCurrentSwitch = 0;
					
					if ( 209 != uiCursorXPosKP )
					{
						fnGr_FillRectangle(uiCursorXPosKP, uiCursorYPosKP, 26,28,RGB(255,255,255));
						uiCursorXPosKP += 30;
											
									fnDisplay_Reg_Write(0x007, 0x0000);   //Display Control1 
									fnGr_FillRectangle(uiCursorXPosKP, uiCursorYPosKP, 26,28,RGB(106,255,102));
									fnGR_SCREEN1(SCREEN_PATTERN_11);
									fnDisplay_Reg_Write(0x007, 0x0173);
									uiCurrentScreen = SCREEN_KEYPAD;
					}
				}break;
				
				case SWITCH13:
				{
					ucCurrentSwitch = 0;
					
					if ( 366 != uiCursorYPosKP )
					{
						fnGr_FillRectangle(uiCursorXPosKP, uiCursorYPosKP, 26,28,RGB(255,255,255));
						uiCursorYPosKP += 30;
											
									fnDisplay_Reg_Write(0x007, 0x0000);   //Display Control1 
									fnGr_FillRectangle(uiCursorXPosKP, uiCursorYPosKP, 26,28,RGB(106,255,102));
									fnGR_SCREEN1(SCREEN_PATTERN_11);
									fnDisplay_Reg_Write(0x007, 0x0173);
									uiCurrentScreen = SCREEN_KEYPAD;
					}
				}break;
				
				case SWITCH14:
				{
					ucCurrentSwitch = 0;
					
					if ( 6 != uiCursorYPosKP )
					{
						fnGr_FillRectangle(uiCursorXPosKP, uiCursorYPosKP, 26,28,RGB(255,255,255));
						uiCursorYPosKP -= 30;					
								
						fnDisplay_Reg_Write(0x007, 0x0000);							
						fnGr_FillRectangle(uiCursorXPosKP, uiCursorYPosKP, 26,28,RGB(106,255,102));
						fnGR_SCREEN1(SCREEN_PATTERN_11);
						fnDisplay_Reg_Write(0x007, 0x0173);
						uiCurrentScreen = SCREEN_KEYPAD;
					}
				}break;			
			}
		}break;

		case SCREEN_PRESSURE_SETTING_1:
		case SCREEN_PRESSURE_SETTING_2:
		{
			if (( TRUE == StatusFlag.stStatusFlag.bParameterSet ) && ( '\0' != ulirgStoreLetter[0] ))
			{
				ucSwitchPressFlag = TRUE;
				StatusFlag.stStatusFlag.bParameterSet = FALSE;
				sscanf((char *)ulirgStoreLetter,"%f",&fUserInput);
				if (( PRESSURE_LOW_LIMIT_VALIDATION <= fUserInput ) && ( PRESSURE_HIGH_LIMIT_VALIDATION >= fUserInput ))
				{
					if ( SCREEN_PRESSURE_SETTING_1 == uiCurrentScreen )
					{
						if ( 1 == ucPresSetVariableCounter )
						{
							stConfig_Value.fConfigPressureGauge1LTH = fUserInput;
							fnOPConfig_Send(CONFIG_VCM_PGAUGE1_TH,LOW_TH);
						}
						else if ( 2 == ucPresSetVariableCounter )
						{
							stConfig_Value.fConfigPressureGauge1HTH = fUserInput;
							fnOPConfig_Send(CONFIG_VCM_PGAUGE1_TH,HIGH_TH);
						}
						else	if ( 3 == ucPresSetVariableCounter )
						{
							stConfig_Value.fConfigPressureGauge2LTH = fUserInput;
							fnOPConfig_Send(CONFIG_VCM_PGAUGE2_TH,LOW_TH);
						}
						else if ( 4 == ucPresSetVariableCounter )
						{
							stConfig_Value.fConfigPressureGauge2HTH = fUserInput;
							fnOPConfig_Send(CONFIG_VCM_PGAUGE2_TH,HIGH_TH);
						}
						else	if ( 5 == ucPresSetVariableCounter )
						{
							stConfig_Value.fConfigPressureGauge3LTH = fUserInput;
							fnOPConfig_Send(CONFIG_VCM_PGAUGE3_TH,LOW_TH);
						}
						else if ( 6 == ucPresSetVariableCounter )
						{
							stConfig_Value.fConfigPressureGauge3HTH = fUserInput;
							fnOPConfig_Send(CONFIG_VCM_PGAUGE3_TH,HIGH_TH);
						}
					}else if ( SCREEN_PRESSURE_SETTING_2 == uiCurrentScreen )
					{
						if ( 1 == ucPresSetVariableCounter )
						{
							stConfig_Value.fConfigSprayPressureLTH = fUserInput;
							fnOPConfig_Send(CONFIG_SPRAY_PRESSURE,(LOW_TH << 1));
						}
						else if ( 2 == ucPresSetVariableCounter )
						{
							stConfig_Value.fConfigSprayPressure = fUserInput;
							fnOPConfig_Send(CONFIG_SPRAY_PRESSURE,SET_VALUE);
						}
						else if ( 3 == ucPresSetVariableCounter )
						{
							stConfig_Value.fConfigSprayPressureHTH = fUserInput;
							fnOPConfig_Send(CONFIG_SPRAY_PRESSURE,(HIGH_TH << 1));
						}
						else if ( 4 == ucPresSetVariableCounter )
						{
							stConfig_Value.fConfigRecirPressureLTH = fUserInput;
							fnOPConfig_Send(CONFIG_RECIRC_PRESSURE,(LOW_TH << 1));
						}
						else if ( 5 == ucPresSetVariableCounter )
						{
							stConfig_Value.fConfigRecirPressure = fUserInput;
							fnOPConfig_Send(CONFIG_RECIRC_PRESSURE,SET_VALUE);
						}
						else if ( 6 == ucPresSetVariableCounter )
						{
							stConfig_Value.fConfigRecirPressureHTH = fUserInput;
							fnOPConfig_Send(CONFIG_RECIRC_PRESSURE,(HIGH_TH << 1));
						}
					}
				}
				fUserInput = 0;
				memset(ulirgStoreLetter,'\0',sizeof(ulirgStoreLetter));
				uiLetterStartPos = 340;
				uiLetter = 0;
				ucLetterPos = 0;
			}
					
			switch(ucCurrentSwitch)
			{ 	
				case SWITCH12:
				{
					ucCurrentSwitch = 0;
					fnGr_FillRectangle((uiCursorXPos - 25), uiCursorYPos, 26,30,WHITE_CLR);
					
					switch(uiCurrentScreen)
					{
						case SCREEN_PRESSURE_SETTING_1:
						case SCREEN_PRESSURE_SETTING_2:
						{
							ucPresSetVariableCounter++;
							uiCursorXPos += 34;
							if ( 270 == uiCursorXPos )
							{
								ucPresSetVariableCounter--;
								uiCursorXPos -= 34;	
							}
						}break;
					}
				}break;
				
				case SWITCH11:
				{
					ucCurrentSwitch = 0;
					fnGr_FillRectangle((uiCursorXPos - 25), uiCursorYPos, 26,30,WHITE_CLR);
					
					switch(uiCurrentScreen)
					{
						case SCREEN_PRESSURE_SETTING_1:
						case SCREEN_PRESSURE_SETTING_2:
						{
							ucPresSetVariableCounter--;
							uiCursorXPos -= 34;
							if ( 32 == uiCursorXPos )
							{
								uiCursorXPos += 34;
								ucPresSetVariableCounter++;
							}
						}break;
					}	
				}break;
			}
		}break;
		
		case SCREEN_TEMP_SETTING_1:
		case SCREEN_TEMP_SETTING_2:
		{
			if (( TRUE == StatusFlag.stStatusFlag.bParameterSet ) && ( '\0' != ulirgStoreLetter[0] ))
			{
				ucSwitchPressFlag = TRUE;
				StatusFlag.stStatusFlag.bParameterSet = FALSE;
				sscanf((char *)ulirgStoreLetter,"%f",&fUserInput);
				if (( TEMPERATURE_LOW_LIMIT_VALIDATION <= fUserInput ) && ( TEMPERATURE_HIGH_LIMIT_VALIDATION >= fUserInput ))
				{				
					if ( SCREEN_TEMP_SETTING_1 == uiCurrentScreen )
					{
						if ( 1 == ucTempSetVariableCounter )
						{
							stConfig_Value.fConfigAntiFrzTempLTH = fUserInput;
							fnOPConfig_Send(CONFIG_BASE_TEMP_SETTING,(LOW_TH <<1));
						}
						else if ( 2 == ucTempSetVariableCounter )
						{
							stConfig_Value.fConfigAntiFrzTempSet = fUserInput;
							fnOPConfig_Send(CONFIG_BASE_TEMP_SETTING,SET_VALUE);
						}
						else	if ( 3 == ucTempSetVariableCounter )
						{
							stConfig_Value.fConfigAntiFrzTempHTH = fUserInput;
							fnOPConfig_Send(CONFIG_BASE_TEMP_SETTING,(HIGH_TH << 1));
						}
						else if ( 4 == ucTempSetVariableCounter )
						{
							stConfig_Value.fConfigInlineBaseTempLTH = fUserInput;
							fnOPConfig_Send(CONFIG_INLINE_BASE_TEMP_SETTING,(LOW_TH << 1));
						}
						else	if ( 5 == ucTempSetVariableCounter )
						{
							stConfig_Value.fConfigInlineBaseTempSet = fUserInput;
							fnOPConfig_Send(CONFIG_INLINE_BASE_TEMP_SETTING,SET_VALUE);
						}
						else if ( 6 == ucTempSetVariableCounter )
						{
							stConfig_Value.fConfigInlineBaseTempHTH = fUserInput;
							fnOPConfig_Send(CONFIG_INLINE_BASE_TEMP_SETTING,(HIGH_TH << 1));
						}
					}
						
					if ( SCREEN_TEMP_SETTING_2 == uiCurrentScreen )
					{
						if ( 1 == ucTempSetVariableCounter )
						{
							stConfig_Value.fConfigInlineHardTempLTH = fUserInput;
							fnOPConfig_Send(CONFIG_INLINE_HARD_TEMP_SETTING,(LOW_TH << 1));
						}
						else	if ( 2 == ucTempSetVariableCounter )
						{
							stConfig_Value.fConfigInlineHardTempSet = fUserInput;
							fnOPConfig_Send(CONFIG_INLINE_HARD_TEMP_SETTING,SET_VALUE);
						}
						else if ( 3 == ucTempSetVariableCounter )
						{
							stConfig_Value.fConfigInlineHardTempHTH = fUserInput;
							fnOPConfig_Send(CONFIG_INLINE_HARD_TEMP_SETTING,(HIGH_TH << 1));
						}if ( 4 == ucTempSetVariableCounter )
						{
							stConfig_Value.fConfigPipeTempLTH = fUserInput;
							fnOPConfig_Send(CONFIG_PIPE_TEMP_TH,LOW_TH);
						}
						else if ( 5 == ucTempSetVariableCounter )
						{
							stConfig_Value.fConfigPipeTempHTH = fUserInput;
							fnOPConfig_Send(CONFIG_PIPE_TEMP_TH,HIGH_TH);
						}
					}
				}
					fUserInput = 0;
					memset(ulirgStoreLetter,'\0',sizeof(ulirgStoreLetter));
					uiLetterStartPos = 340;
					uiLetter = 0;
					ucLetterPos = 0;
			}
						
			switch(ucCurrentSwitch)
			{
				case SWITCH12:
				{
					ucCurrentSwitch = 0;
					fnGr_FillRectangle((uiCursorXPos - 25), uiCursorYPos, 26,30,WHITE_CLR);
					
					switch(uiCurrentScreen)
					{	
						case SCREEN_TEMP_SETTING_1:
						{
							ucTempSetVariableCounter++;
							uiCursorXPos += 34;
							if ( 270 == uiCursorXPos )
							{
								ucTempSetVariableCounter--;
								uiCursorXPos -= 34;	
							}	
						}break;
						
						case SCREEN_TEMP_SETTING_2:
						{
							ucTempSetVariableCounter++;
							uiCursorXPos += 34;
							if ( 236 == uiCursorXPos )
							{
								ucTempSetVariableCounter--;
								uiCursorXPos -= 34;	
							}	
						}break;
					}
				}break;
				
				case SWITCH11:
				{
					ucCurrentSwitch = 0;
					fnGr_FillRectangle((uiCursorXPos - 25), uiCursorYPos, 26,30,WHITE_CLR);
					
					switch(uiCurrentScreen)
					{
						case SCREEN_TEMP_SETTING_1:
						case SCREEN_TEMP_SETTING_2:
						{
							ucTempSetVariableCounter--;
							uiCursorXPos -= 34;
							if ( 32 == uiCursorXPos )
							{
								uiCursorXPos += 34;
								ucTempSetVariableCounter++;
							}
						}break;
					}
				}break;
			}
		}break;
		case SCREEN_PUMP_SETTING:
		{
			if (( TRUE == StatusFlag.stStatusFlag.bParameterSet ) && ( '\0' != ulirgStoreLetter[0] ))
			{
				ucSwitchPressFlag = TRUE;
				StatusFlag.stStatusFlag.bParameterSet = FALSE;
				if ( 1 == ucPumpSetVariableCounter )
				{
					sscanf((const char *)ulirgStoreLetter,"%hd",&iUserInput);
					if( (0 < iUserInput) && (100 > iUserInput))
					{
						stConfig_Value.uiConfigCylinderCount = iUserInput;
						fnOPConfig_Send(CONFIG_NUMBER_OF_CYLINDER,0);
					}
				}
				else if ( 2 == ucPumpSetVariableCounter )
				{
					sscanf((const char *)ulirgStoreLetter,"%f",&stConfig_Value.fConfigAreaofCylinderA);
					fnOPConfig_Send(CONFIG_CYLINDER_SURFACE_AREA,LEG_A);
				}
				else	if ( 3 == ucPumpSetVariableCounter )
				{
					sscanf((const char *)ulirgStoreLetter,"%f",&stConfig_Value.fConfigAreaofCylinderB);
					fnOPConfig_Send(CONFIG_CYLINDER_SURFACE_AREA,LEG_B);
				}
				else if ( 4 == ucPumpSetVariableCounter )
				{
					sscanf((const char *)ulirgStoreLetter,"%f",&stConfig_Value.fConfigAreaofCylinderC);
					fnOPConfig_Send(CONFIG_CYLINDER_SURFACE_AREA,LEG_C);
				}
				
				memset(ulirgStoreLetter,'\0',sizeof(ulirgStoreLetter));
				uiLetterStartPos = 340;
				uiLetter = 0;
				ucLetterPos = 0;
			}
					
			switch(ucCurrentSwitch)
			{ 	
				case SWITCH12:
				{
					ucCurrentSwitch = 0;
					fnGr_FillRectangle((uiCursorXPos - 25), uiCursorYPos, 26,30,WHITE_CLR);
					
					switch(uiCurrentScreen)
					{
						case SCREEN_PUMP_SETTING:
						{
							ucPumpSetVariableCounter++;
							uiCursorXPos += 34;
							if ( 202 == uiCursorXPos )
							{
								ucPumpSetVariableCounter--;
								uiCursorXPos -= 34;	
							}
						}break;
					}
				}break;
					
				
				case SWITCH11:
				{
					ucCurrentSwitch = 0;
					fnGr_FillRectangle((uiCursorXPos - 25), uiCursorYPos, 26,30,WHITE_CLR);
					
					switch(uiCurrentScreen)
					{
						case SCREEN_PUMP_SETTING:
						{
							ucPumpSetVariableCounter--;
							uiCursorXPos -= 34;
							if ( 32 == uiCursorXPos )
							{
								uiCursorXPos += 34;
								ucPumpSetVariableCounter++;
							}
						}
					}	
					
				}break;
				
			}
		}break;
		case SCREEN_JOB_SETTING_1:
		case SCREEN_JOB_SETTING_2:
		{
			if ( TRUE == StatusFlag.stStatusFlag.bParameterSet ) 
			{
				ucSwitchPressFlag = TRUE;
				StatusFlag.stStatusFlag.bParameterSet = FALSE;
				if ( '\0' != ulirgStoreLetter[0] )
				{
					switch(uiCurrentScreen)
					{
						case SCREEN_JOB_SETTING_1:
						{
							if ( 1 == ucJobSetVariableCounter )
							{
								memset(rgucSupervisorName,'\0',sizeof(rgucSupervisorName));
								
								strncpy((char *)stConfig_Value.rgucConfigSupervisorName1,(const char *)ulirgStoreLetter,7);
								stConfig_Value.rgucConfigSupervisorName1[7] = '\0';
								strcat((char *)rgucSupervisorName,(const char *)stConfig_Value.rgucConfigSupervisorName1);
								
								strncpy((char *)stConfig_Value.rgucConfigSupervisorName2,(const char *)ulirgStoreLetter + 7,7);
								stConfig_Value.rgucConfigSupervisorName2[7] = '\0';
								strcat((char *)rgucSupervisorName,(const char *)stConfig_Value.rgucConfigSupervisorName2);
								
								strncpy((char *)stConfig_Value.rgucConfigSupervisorName3,(const char *)ulirgStoreLetter + 14,7);
								stConfig_Value.rgucConfigSupervisorName3[7] = '\0';
								strcat((char *)rgucSupervisorName,(const char *)stConfig_Value.rgucConfigSupervisorName3);
								
								fnOPConfig_Send(CONFIG_SUPERVISOR_NAME,0);
							}
							else if ( 2 == ucJobSetVariableCounter )
							{
								memset(rgucCoatingMaterial,'\0',sizeof(rgucCoatingMaterial));
								
								strncpy((char *)stConfig_Value.rgucConfigCoatingMatName1,(const char *)ulirgStoreLetter,7);
								stConfig_Value.rgucConfigCoatingMatName1[7] = '\0';
								strcat((char *)rgucCoatingMaterial,(const char *)stConfig_Value.rgucConfigCoatingMatName1);
								
								strncpy((char *)stConfig_Value.rgucConfigCoatingMatName2,(const char *)ulirgStoreLetter + 7,7);
								stConfig_Value.rgucConfigCoatingMatName2[7] = '\0';
								strcat((char *)rgucCoatingMaterial,(const char *)stConfig_Value.rgucConfigCoatingMatName2);
								
								strncpy((char *)stConfig_Value.rgucConfigCoatingMatName3,(const char *)ulirgStoreLetter + 14,7);
								stConfig_Value.rgucConfigCoatingMatName3[7] = '\0';
								strcat((char *)rgucCoatingMaterial,(const char *)stConfig_Value.rgucConfigCoatingMatName3);
								
								fnOPConfig_Send(CONFIG_COATING_MATERIAL,0);
							}
							else	if ( 3 == ucJobSetVariableCounter )
							{
								memset(rgucBaseMatBatchNumber,'\0',sizeof(rgucBaseMatBatchNumber));
								
								strncpy((char *)stConfig_Value.rgucConfigBaseMatBatchNo1,(const char *)ulirgStoreLetter,7);
								stConfig_Value.rgucConfigBaseMatBatchNo1[7] = '\0';
								strcat((char *)rgucBaseMatBatchNumber,(const char *)stConfig_Value.rgucConfigBaseMatBatchNo1);
								
								strncpy((char *)stConfig_Value.rgucConfigBaseMatBatchNo2,(const char *)ulirgStoreLetter + 7,7);
								stConfig_Value.rgucConfigBaseMatBatchNo2[7] = '\0';
								strcat((char *)rgucBaseMatBatchNumber,(const char *)stConfig_Value.rgucConfigBaseMatBatchNo2);
								
								strncpy((char *)stConfig_Value.rgucConfigBaseMatBatchNo3,(const char *)ulirgStoreLetter + 14,7);
								stConfig_Value.rgucConfigBaseMatBatchNo3[7] = '\0';
								strcat((char *)rgucBaseMatBatchNumber,(const char *)stConfig_Value.rgucConfigBaseMatBatchNo3);

								fnOPConfig_Send(CONFIG_BASE_MAT_BATCH_NO,0);
							}
						}break;
						case SCREEN_JOB_SETTING_2:
						{
							if ( 1 == ucJobSetVariableCounter )
							{
								memset(rgucHardMatBatchNumber,'\0',sizeof(rgucHardMatBatchNumber));
								
								strncpy((char *)stConfig_Value.rgucConfigHardMatBatchNo1,(const char *)ulirgStoreLetter,7);
								stConfig_Value.rgucConfigHardMatBatchNo1[7] = '\0';
								strcat((char *)rgucHardMatBatchNumber,(const char *)stConfig_Value.rgucConfigHardMatBatchNo1);
								
								strncpy((char *)stConfig_Value.rgucConfigHardMatBatchNo2,(const char *)ulirgStoreLetter + 7,7);
								stConfig_Value.rgucConfigHardMatBatchNo2[7] = '\0';
								strcat((char *)rgucHardMatBatchNumber,(const char *)stConfig_Value.rgucConfigHardMatBatchNo2);
								
								strncpy((char *)stConfig_Value.rgucConfigHardMatBatchNo3,(const char *)ulirgStoreLetter + 14,7);
								stConfig_Value.rgucConfigHardMatBatchNo3[7] = '\0';
								strcat((char *)rgucHardMatBatchNumber,(const char *)stConfig_Value.rgucConfigHardMatBatchNo3);
								
								fnOPConfig_Send(CONFIG_HARD_MAT_BATCH_NO,0);
							}
							else if ( 2 == ucJobSetVariableCounter )
							{
								memset(rgucJobName,'\0',sizeof(rgucJobName));
								
								strncpy((char *)stConfig_Value.rgucJobName1,(const char *)ulirgStoreLetter,7);
								stConfig_Value.rgucJobName1[7] = '\0';
								strcat((char *)rgucJobName,(const char *)stConfig_Value.rgucJobName1);
								
								strncpy((char *)stConfig_Value.rgucJobName2,(const char *)ulirgStoreLetter + 7,7);
								stConfig_Value.rgucJobName2[7] = '\0';
								strcat((char *)rgucJobName,(const char *)stConfig_Value.rgucJobName2);
								
								strncpy((char *)stConfig_Value.rgucJobName3,(const char *)ulirgStoreLetter + 14,7);
								stConfig_Value.rgucJobName3[7] = '\0';
								strcat((char *)rgucJobName,(const char *)stConfig_Value.rgucJobName3);
								
								fnOPConfig_Send(CONFIG_JOB_NAME,0);
							}
							/*change7*/
							else if ( 3 == ucJobSetVariableCounter )
							{
								memset(rgucJobId,'\0',sizeof(rgucJobId));
								
								strncpy((char *)stConfig_Value.rgucJobId1,(const char *)ulirgStoreLetter,7);
								stConfig_Value.rgucJobId1[7] = '\0';
								strcat((char *)rgucJobId,(const char *)stConfig_Value.rgucJobId1);
								
								strncpy((char *)stConfig_Value.rgucJobId2,(const char *)ulirgStoreLetter + 7,7);
								stConfig_Value.rgucJobId2[7] = '\0';
								strcat((char *)rgucJobId,(const char *)stConfig_Value.rgucJobId2);
								
								strncpy((char *)stConfig_Value.rgucJobId3,(const char *)ulirgStoreLetter + 14,7);
								stConfig_Value.rgucJobId3[7] = '\0';
								strcat((char *)rgucJobId,(const char *)stConfig_Value.rgucJobId3);
								
								fnOPConfig_Send(CONFIG_JOB_ID,0);
							}
						}break;
					
					}
				}
				
				memset(ulirgStoreLetter,'\0',sizeof(ulirgStoreLetter));
				uiLetterStartPos = 340;
				uiLetter = 0;
				ucLetterPos = 0;
			}
					
			switch(ucCurrentSwitch)
			{
				case SWITCH12:
				{
					ucCurrentSwitch = 0;
					fnGr_FillRectangle((uiCursorXPos - 25), uiCursorYPos, 26,30,WHITE_CLR);
					
					switch(uiCurrentScreen)
					{
						case SCREEN_JOB_SETTING_1:
						{
							ucJobSetVariableCounter++;
							uiCursorXPos += 68;
							if ( 270 == uiCursorXPos )
							{
								ucJobSetVariableCounter--;
								uiCursorXPos -= 68;	
							}	
						}break;
						case SCREEN_JOB_SETTING_2:
						{
							ucJobSetVariableCounter++;
							uiCursorXPos += 68;
							if ( 270 == uiCursorXPos )
							{
								ucJobSetVariableCounter--;
								uiCursorXPos -= 68;	
							}	
						}break;
					}
				}break;
					
				
				case SWITCH11:
				{
					ucCurrentSwitch = 0;
					fnGr_FillRectangle((uiCursorXPos - 25), uiCursorYPos, 26,30,WHITE_CLR);
					
					switch(uiCurrentScreen)
					{
						case SCREEN_JOB_SETTING_1:
						case SCREEN_JOB_SETTING_2:
						{
							ucJobSetVariableCounter--;
							uiCursorXPos -= 68;
							if ( 0 == ucJobSetVariableCounter )
							{
								uiCursorXPos = 66;
								ucJobSetVariableCounter++;
							}
							
						}break;
					}	
					
				}break;
				
			}
		}break;
		case SCREEN_TIMER_SETTING_1:
		case SCREEN_TIMER_SETTING_2:
		case SCREEN_TIMER_SETTING_3:
		case SCREEN_TIMER_SETTING_4:
		{
			if (( TRUE == StatusFlag.stStatusFlag.bParameterSet ) && ( '\0' != ulirgStoreLetter[0] ))
			{
				ucSwitchPressFlag = TRUE;
				StatusFlag.stStatusFlag.bParameterSet = FALSE;
				if ( '\0' != ulirgStoreLetter[0] )
				{
					switch(uiCurrentScreen)
					{
						case SCREEN_TIMER_SETTING_1:
						{
							sscanf((char *)ulirgStoreLetter,"%f",&fUserInput);
							if (( TIMER_LOW_LIMIT_VALIDATION <= fUserInput ) && ( TIMER_HIGH_LIMIT_VALIDATION > fUserInput ))
							{
								if ( 1 == ucTimerSetVariableCounter )
								{
									stConfig_Value.fConfigPrimeTime = fUserInput;
									fnOPConfig_Send(CONFIG_PRIME_TIME,0);
								}
								else if ( 2 == ucTimerSetVariableCounter )
								{
									stConfig_Value.fConfigFlushTime = fUserInput;
									fnOPConfig_Send(CONFIG_OPMM_FLUSH_TIME,0);
								}
								else	if ( 3 == ucTimerSetVariableCounter )
								{
									stConfig_Value.fConfigGunDelayTime = fUserInput;
									fnOPConfig_Send(CONFIG_OPMM_GUN_DELAY,0);
								}
								else if ( 4 == ucTimerSetVariableCounter )
								{
									stConfig_Value.fConfigReverseDelayTime = fUserInput;
									fnOPConfig_Send(CONFIG_OPMM_REVERSE_DELAY,0);
								}
								else	if ( 5 == ucTimerSetVariableCounter )
								{
									stConfig_Value.fConfigSprayOverlapTime = fUserInput;
									fnOPConfig_Send(CONFIG_OPMM_SPRAY_OVERLAP_TIME,0);
								}
								else if ( 6 == ucTimerSetVariableCounter )
								{
									stConfig_Value.fConfigDataLoggingPeriod = fUserInput;
									fnOPConfig_Send(CONFIG_DATA_LOG_PERIOD,0);
								}
							}
						}break;
						
						case SCREEN_TIMER_SETTING_2:
						{
							sscanf((char *)ulirgStoreLetter,"%f",&fUserInput);
							if (( TIMER_LOW_LIMIT_VALIDATION <= fUserInput ) && ( TIMER_HIGH_LIMIT_VALIDATION > fUserInput ))
							{
								if ( 1 == ucTimerSetVariableCounter )
								{
									stConfig_Value.fConfigTimeBeforeStall = fUserInput;
									fnOPConfig_Send(CONFIG_TIME_BEFORE_STALL,0);
								}
								else if ( 2 == ucTimerSetVariableCounter )
								{
									stConfig_Value.fConfigTimeCompareStall = fUserInput;
									fnOPConfig_Send(CONFIG_TIME_BEFORE_COMPARE_STALL,0);
								}
								else	if ( 3 == ucTimerSetVariableCounter )
								{
									stConfig_Value.fConfigDCLSHomeTime = fUserInput;
									fnOPConfig_Send(CONFIG_DCLS_HOME_TIME,DCLS_TIMER);
								}
								else if ( 4 == ucTimerSetVariableCounter )
								{
									stConfig_Value.fConfigHomeDCLSTime = fUserInput;
									fnOPConfig_Send(CONFIG_DCLS_HOME_TIME,HLS_TIMER);
								}
								else	if ( 5 == ucTimerSetVariableCounter )
								{
									stConfig_Value.fGunPosSwTime = fUserInput;
									fnOPConfig_Send(CONFIG_GUN_POSITION_CHANGE_TIME,0);
								}
								else if ( 6 == ucTimerSetVariableCounter )
								{/*change25*/
									//if(15 <= fUserInput)
									//{
									stConfig_Value.fConfigTimebeforeONSprayValve = fUserInput;
									fnOPConfig_Send(CONFIG_SPRAY_VALVE_OPEN_TIME_VBAT,0);
									//}
								}
							}
						}break;
						
						case SCREEN_TIMER_SETTING_3:
						{
							sscanf((char *)ulirgStoreLetter,"%f",&fUserInput);
							if (( TIMER_LOW_LIMIT_VALIDATION <= fUserInput ) && ( TIMER_HIGH_LIMIT_VALIDATION > fUserInput ))
							{
								if ( 1 == ucTimerSetVariableCounter )
								{
									stConfig_Value.fConfigFlushWarnTime = fUserInput;
									fnOPConfig_Send(CONFIG_FLUSH_WARNING_TIME,0);
								}
								else if ( 2 == ucTimerSetVariableCounter )
								{
									stConfig_Value.fTimerStartStallTest = fUserInput;
									fnOPConfig_Send(CONFIG_TIME_TO_START_STALL,0);
								}
								else	if ( 3 == ucTimerSetVariableCounter )
								{
									stConfig_Value.fTimerSprayLastRotStop = fUserInput;
									fnOPConfig_Send(CONFIG_STOP_MOTOR_ROTATION_TIME,0);
								}
								else	if ( 4 == ucTimerSetVariableCounter )
								{
									stConfig_Value.fTimerIdealPeriodic = fUserInput;
									fnOPConfig_Send(CONFIG_IDEAL_PERIODIC_TIME,0);
								}
								
							}
							/*change1*/
							if ( 5 == ucTimerSetVariableCounter )
								{
									stConfig_Value.ucConfig_TimeOffsetSign = (ulirgStoreLetter[0]);
									if(45 == stConfig_Value.ucConfig_TimeOffsetSign)
											{
											
											stConfig_Value.ucConfig_TimeOffsetSign	= 0x01;
											ulirgStoreLetter[0]=' ';
											}
											else if(55 == stConfig_Value.ucConfig_TimeOffsetSign)
											{
											stConfig_Value.ucConfig_TimeOffsetSign	= 0x00;
											}
											else
											{
													stConfig_Value.ucConfig_TimeOffsetSign	= 0x00;
											}
											memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));	
											stConfig_Value.ucConfig_TimeOffsetHour = atoi(strtok((char *)ulirgStoreLetter,"."));
											if(24 > stConfig_Value.ucConfig_TimeOffsetHour)
											{
											}
											else
											{
													stConfig_Value.ucConfig_TimeOffsetHour = 0;
											}
											memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
											stConfig_Value.ucConfig_TimeOffsetMin  = atoi(strtok(NULL,"."));
											if(60 > stConfig_Value.ucConfig_TimeOffsetMin)
											{
										
											}
											else
											{
													stConfig_Value.ucConfig_TimeOffsetMin = 0;
											}
											
											stConfig_Value.ucConfig_TimeOffsetSec = 0;
											fnOPConfig_Send(CONFIG_UTC_TIME_OFFSET,0);
								}
						}break;
						
						case SCREEN_TIMER_SETTING_4:
						{
							uiSytemDateTime = atoi((const char *)ulirgStoreLetter);
							if ( 1 == ucTimerSetVariableCounter )
							{
								/*date*/
								if( (0 < uiSytemDateTime) && (31 >= uiSytemDateTime) )
								{
									stConfig_Value.ucConfigSystemTime[3] = (uint8_t)uiSytemDateTime;		
									strcpy((char *)stDisplayDateTime.ucUTCDD , (const char *)ulirgStoreLetter);
								}
							}
							else if ( 2 == ucTimerSetVariableCounter )
							{
								/*month*/
								if( (0 < uiSytemDateTime) && (12 >= uiSytemDateTime) )
								{
									stConfig_Value.ucConfigSystemTime[4] = (uint8_t)uiSytemDateTime;		
									strcpy((char *)stDisplayDateTime.ucUTCMM , (const char *)ulirgStoreLetter);		
								}									
							}
							else	if ( 3 == ucTimerSetVariableCounter )
							{
								/*year*/
								stConfig_Value.ucConfigSystemTime[5] = (uint8_t)(uiSytemDateTime / 100);
								stConfig_Value.ucConfigSystemTime[6] = (uint8_t)(uiSytemDateTime % 2000);
								
								strcpy((char *)stDisplayDateTime.ucUTCYY , (const char *)ulirgStoreLetter);
														
							}
							else if ( 4 == ucTimerSetVariableCounter )
							{
								/*hour*/
								if(23 >= uiSytemDateTime)
								{
									stConfig_Value.ucConfigSystemTime[0] = (uint8_t)uiSytemDateTime;		
									strcpy((char *)stDisplayDateTime.ucUTCHR , (const char *)ulirgStoreLetter);
								}
							}
							else	if ( 5 == ucTimerSetVariableCounter )
							{
								/*minute*/
								if (60 > uiSytemDateTime)
								{
									stConfig_Value.ucConfigSystemTime[1] = (uint8_t)uiSytemDateTime;
									strcpy((char *)stDisplayDateTime.ucUTCMIN , (const char *)ulirgStoreLetter);							
								}
							}
							else if ( 6 == ucTimerSetVariableCounter )
							{
								/*second*/
								if (60 > uiSytemDateTime) 
								{
									stConfig_Value.ucConfigSystemTime[2] = (uint8_t)uiSytemDateTime;
									strcpy((char *)stDisplayDateTime.ucUTCSEC , (const char *)ulirgStoreLetter);
								}
							}	
						}break;
					}
				}
				
				memset(ulirgStoreLetter,'\0',sizeof(ulirgStoreLetter));
				uiLetterStartPos = 340;
				uiLetter = 0;
				ucLetterPos = 0;
				fUserInput = 0;
			}
					
			switch(ucCurrentSwitch)
			{
				case SWITCH12:
				{
					ucCurrentSwitch = 0;
					fnGr_FillRectangle((uiCursorXPos - 25), uiCursorYPos, 26,30,WHITE_CLR);
					switch(uiCurrentScreen)
					{
						case SCREEN_TIMER_SETTING_1:
						case SCREEN_TIMER_SETTING_2:
						case SCREEN_TIMER_SETTING_4:
						{
							ucTimerSetVariableCounter++;
							uiCursorXPos += 34;
							if ( 270 == uiCursorXPos )
							{
								ucTimerSetVariableCounter--;
								uiCursorXPos -= 34;	
							}	
						}break;
						case SCREEN_TIMER_SETTING_3:
						{
							ucTimerSetVariableCounter++;
							uiCursorXPos += 34;
							if ( 236 == uiCursorXPos )  /*change1*/
							{
								ucTimerSetVariableCounter--;
								uiCursorXPos -= 34;	
							}	
						}break;
					}
				}break;
					
				
				case SWITCH11:
				{
					ucCurrentSwitch = 0;
					fnGr_FillRectangle((uiCursorXPos - 25), uiCursorYPos, 26,30,WHITE_CLR);
					
					switch(uiCurrentScreen)
					{
						case SCREEN_TIMER_SETTING_1:
						case SCREEN_TIMER_SETTING_2:
						case SCREEN_TIMER_SETTING_3:
						case SCREEN_TIMER_SETTING_4:
						{
							ucTimerSetVariableCounter--;
							uiCursorXPos -= 34;
							if ( 32 == uiCursorXPos )
							{
								uiCursorXPos += 34;
								ucTimerSetVariableCounter++;
							}
						}break;
					}	
				}break;
				
			}
		}break;
		case SCREEN_STALL_SETTING:
		case SCREEN_STALL_SETTING_1:
		{
			if (( TRUE == StatusFlag.stStatusFlag.bParameterSet ) && ( '\0' != ulirgStoreLetter[0] ))
			{
				ucSwitchPressFlag = TRUE;
				StatusFlag.stStatusFlag.bParameterSet = FALSE;
				
				sscanf((const char *)ulirgStoreLetter,"%hd",&iUserInput);
				
				if ( SCREEN_STALL_SETTING == uiCurrentScreen )
				{
					if ( 1 == ucStallSetVariableCounter )
					{
						if (( STALL_NEGATIVE_TH_VALIDATION_MIN < iUserInput ) && ( STALL_NEGATIVE_TH_VALIDATION_MAX > iUserInput ))
						{
							stConfig_Value.iConfigDownStrokeTH = iUserInput;
							fnOPConfig_Send(CONFIG_VCM_STALL_TH,DOWNSTROKE_TH);
						}
					}
					else if ( 2 == ucStallSetVariableCounter )
					{
						if (( STALL_POSITIVE_TH_VALIDATION_MIN < iUserInput ) && ( STALL_POSITIVE_TH_VALIDATION_MAX > iUserInput ))
						{ 
							stConfig_Value.iConfigUpStrokeTH = iUserInput; 
							fnOPConfig_Send(CONFIG_VCM_STALL_TH,UPSTROKE_TH);
						}
					}
					else if ( 3 == ucStallSetVariableCounter )
					{
						if (( 0 < iUserInput ) && ( iUserInput < 150 ))
						{
							stConfig_Value.fUpstrokeLimit = iUserInput; 
							fnOPConfig_Send(CONFIG_STALL_TEST_LT_RANGES,UPSTROKE_LIMIT);
						}
					}
					else if ( 4 == ucStallSetVariableCounter )
					{
						if (( 0 < iUserInput ) && ( iUserInput < 150 ))
						{
							stConfig_Value.fUpstrokeMin = iUserInput; 
							fnOPConfig_Send(CONFIG_STALL_TEST_LT_RANGES,UPSTROKE_MIN);
						}
					}
					else if ( 5 == ucStallSetVariableCounter )
					{
						if (( 0 < iUserInput ) && ( iUserInput < 150 ))
						{
							stConfig_Value.fUpstrokeMax = iUserInput; 
							fnOPConfig_Send(CONFIG_STALL_TEST_LT_RANGES,UPSTROKE_MAX);
						}
					}
					else if ( 6 == ucStallSetVariableCounter )
					{
						if (( 0 < iUserInput ) && ( iUserInput < 150 ))
						{
							stConfig_Value.fDownstrokeLimit = iUserInput; 
							fnOPConfig_Send(CONFIG_STALL_TEST_LT_RANGES,DOWNSTROKE_LIMIT);
						}
					}
				}
				
				if ( SCREEN_STALL_SETTING_1 == uiCurrentScreen )
				{
					if ( 1 == ucStallSetVariableCounter )
					{
						if (( 0 < iUserInput ) && ( iUserInput < 150 ))
						{
							stConfig_Value.fDownstrokeMin = iUserInput; 
							fnOPConfig_Send(CONFIG_STALL_TEST_LT_RANGES,DOWNSTROKE_MIN);
						}
					}
					else if ( 2 == ucStallSetVariableCounter )
					{
						if (( 0 < iUserInput ) && ( iUserInput < 150 ))
						{
							stConfig_Value.fDownstrokeMax = iUserInput; 
							fnOPConfig_Send(CONFIG_STALL_TEST_LT_RANGES,DOWNSTROKE_MAX);
						}
					}
				}
				iUserInput = 0;
				memset(ulirgStoreLetter,'\0',sizeof(ulirgStoreLetter));
				uiLetterStartPos = 340;
				uiLetter = 0;
				ucLetterPos = 0;
			}
					
			switch(ucCurrentSwitch)
			{ 
				case SWITCH12:
				{
					ucCurrentSwitch = 0;
					fnGr_FillRectangle((uiCursorXPos - 25), uiCursorYPos, 26,30,WHITE_CLR);
					
					switch(uiCurrentScreen)
					{
						case SCREEN_STALL_SETTING:
						{
							ucStallSetVariableCounter++;
							uiCursorXPos += 34;
							if ( 270 == uiCursorXPos )
							{
								ucStallSetVariableCounter--;
								uiCursorXPos -= 34;	
							}	
						}break;
						
						case SCREEN_STALL_SETTING_1:
						{
							ucStallSetVariableCounter++;
							uiCursorXPos += 34;
							if ( 134 == uiCursorXPos )
							{
								ucStallSetVariableCounter--;
								uiCursorXPos -= 34;	
							}	
						}break;
					}					
					}break;
				
				case SWITCH11:
				{
					ucCurrentSwitch = 0;
					fnGr_FillRectangle((uiCursorXPos - 25), uiCursorYPos, 26,30,WHITE_CLR);
					
					switch(uiCurrentScreen)
					{
						case SCREEN_STALL_SETTING:
						case SCREEN_STALL_SETTING_1:
						{
							ucStallSetVariableCounter--;
							uiCursorXPos -= 34;
							if ( 32 == uiCursorXPos )
							{
								uiCursorXPos += 34;
								ucStallSetVariableCounter++;
							}	
						}break;
					}	
					
				}break;
				
			}
		}break;
		case SCREEN_OTHER_SETTING:
		case SCREEN_OTHER_SETTING_1:
		{
			if (( TRUE == StatusFlag.stStatusFlag.bParameterSet ) && ( '\0' != ulirgStoreLetter[0] ))
			{
				ucSwitchPressFlag = TRUE;
				StatusFlag.stStatusFlag.bParameterSet = FALSE;
				if ( '\0' != ulirgStoreLetter[0] )
				{
					switch(uiCurrentScreen)
					{
						case SCREEN_OTHER_SETTING:
						{
							if ( 1 == ucOtherSetVariableCounter )
							{
								sscanf((const char *)ulirgStoreLetter,"%f",&fUserInput);
								if(( 0.0f <= fUserInput ) && ( 9999.0f >=  fUserInput ))
								{
									stConfig_Value.fConfigJointNum = fUserInput;
									fnOPConfig_Send(CONFIG_JOINT_NUMBER,0);
								}
							}
							if ( 2 == ucOtherSetVariableCounter )
							{
								sscanf((const char *)ulirgStoreLetter,"%hd",&iUserInput);
								if (( 0 < iUserInput ) && ( 200 > iUserInput ))
								{
									stConfig_Value.uiConfigSprayCount = iUserInput;
									fnOPConfig_Send(CONFIG_SPRAY_COUNT,0);
								}
							}
						}break;
						case SCREEN_OTHER_SETTING_1:
						{
							if( 1 == ucOtherSetVariableCounter)
							{
								sscanf((const char *)ulirgStoreLetter,"%hd",&iUserInput);
								if(( 1000 <= iUserInput ) && ( 9999 >=  iUserInput ))
								{
									stConfig_Value.uiSignature = iUserInput;
									fnOPConfig_Send(CONFIG_SIGNATURE_BYTE,0);
								}
							}
							/*3rdMarch*/
							if( 2 == ucOtherSetVariableCounter)
							{
								sscanf((char *)ulirgStoreLetter,"%f",&fUserInput);
								if (( 0.0f <= fUserInput ) && ( 40.0 >= fUserInput ))
								{
									stConfig_Value.fOPMMBatKneeVoltage = fUserInput;
									fnOPConfig_Send(CONFIG_BAT_KNEE_VOLTAGE,0);
								}
							}
							
							/*21July2016*/
							if( 3 == ucOtherSetVariableCounter)
							{
								sscanf((char *)ulirgStoreLetter,"%f",&fUserInput);
								if (( 0.0f <= fUserInput ) && ( 5000.0 >= fUserInput ))
								{
									stConfig_Value.fPipeDiameter = fUserInput;
									fnOPConfig_Send(CONFIG_PIPE_DIAMETER,0);
								}
							}
						}break;
					}
				}
				
				fUserInput = 0;
				memset(ulirgStoreLetter,'\0',sizeof(ulirgStoreLetter));
				uiLetterStartPos = 340;
				uiLetter = 0;
				ucLetterPos = 0;
			}
					
			switch(ucCurrentSwitch)
			{
				case SWITCH12:
				{
					ucCurrentSwitch = 0;
					fnGr_FillRectangle((uiCursorXPos - 25), uiCursorYPos, 26,30,WHITE_CLR);
					
					switch(uiCurrentScreen)
					{
						case SCREEN_OTHER_SETTING:
						{
							ucOtherSetVariableCounter++;
							uiCursorXPos += 34;
							if ( 236 == uiCursorXPos )
							{
								ucOtherSetVariableCounter--;
								uiCursorXPos -= 34;	
							}
						}break;
						case SCREEN_OTHER_SETTING_1:
						{
							ucOtherSetVariableCounter++;
							uiCursorXPos += 34;
							if ( 168 == uiCursorXPos )
							{
								ucOtherSetVariableCounter--;
								uiCursorXPos -= 34;	
							}
						}break;
					}									
				}break;		
				case SWITCH11:
				{
					ucCurrentSwitch = 0;
					fnGr_FillRectangle((uiCursorXPos - 25), uiCursorYPos, 26,30,WHITE_CLR);
					
					switch(uiCurrentScreen)
					{
						case SCREEN_OTHER_SETTING:
						case SCREEN_OTHER_SETTING_1:
						{
							ucOtherSetVariableCounter--;
							uiCursorXPos -= 34;
							if ( 32 == uiCursorXPos )
							{
								uiCursorXPos += 34;
								ucOtherSetVariableCounter++;
							}	
						}
					}	
					
				}break;
				
			}
		}break;
		case SCREEN_ADVANCED_RESET:
		{
			/*switch(ucCurrentSwitch)
			{ 
				case SWITCH12:
				{
					ucCurrentSwitch = 0;
					fnGr_FillRectangle((uiCursorXPos - 25), uiCursorYPos, 26,30,WHITE_CLR);
					
					switch(uiCurrentScreen)
					{
						case SCREEN_ADVANCED_RESET:
						{
							ucResetVariableCounter++;
							uiCursorXPos += 34;
							if ( 168 == uiCursorXPos )
							{
								ucResetVariableCounter--;
								uiCursorXPos -= 34;	
							}							
						}break;
					}				
				}break;
				
				case SWITCH11:
				{
					ucCurrentSwitch = 0;
					fnGr_FillRectangle((uiCursorXPos - 25), uiCursorYPos, 26,30,WHITE_CLR);
					
					switch(uiCurrentScreen)
					{
						case SCREEN_ADVANCED_RESET:
						{
							ucResetVariableCounter--;
							uiCursorXPos -= 34;
							if ( 32 == uiCursorXPos )
							{
								uiCursorXPos += 34;
								ucResetVariableCounter++;
							}							
						}
					}	
					
				}break;
				
			}*/
		}break;
		case SCREEN_DIAGNOSTIC_STATUS:
		{
			switch(ucCurrentSwitch)
			{
				case SWITCH14:
				{
					ucCurrentSwitch = 0;
					fnGr_FillRectangle(uiCursorXPos, uiCursorYPos, 3,26,RGB(255,255,255));
					
					switch(uiCurrentScreen)
					{
						case SCREEN_DIAGNOSTIC_STATUS:
						{
							ucDiagModuleVariable++;
							uiCursorYPos -= 80;
							
							if ( 25 == uiCursorYPos )
							{
								uiCursorYPos += 240;
								uiCursorXPos += 26;
								if ( 152 == uiCursorXPos )
								{
									ucDiagModuleVariable--;
									uiCursorYPos = 105;
									uiCursorXPos -= 26;
									break;
								}								
								break;
							}	
						}break;
					}
				}break;
					
				
				case SWITCH13:
				{
					ucCurrentSwitch = 0;
					fnGr_FillRectangle(uiCursorXPos, uiCursorYPos, 3,26,RGB(255,255,255));
					
					switch(uiCurrentScreen)
					{
						case SCREEN_DIAGNOSTIC_STATUS:
						{
							ucDiagModuleVariable--;						
							uiCursorYPos += 80;
							
							if ( 345 == uiCursorYPos )
							{
								uiCursorYPos -= 240;
								uiCursorXPos -= 26;
								if ( 22 == uiCursorXPos )
								{
									ucDiagModuleVariable++;
									uiCursorYPos = 265;
									uiCursorXPos += 26;
									break;
								}								
								break;
							}
						}break;
					}					
				}break;
			}
		}break;
		case SCREEN_HYSTERESIS_1:
		case SCREEN_HYSTERESIS_2:
		{
			if (( TRUE == StatusFlag.stStatusFlag.bParameterSet ) && ( '\0' != ulirgStoreLetter[0] ))
			{
				ucSwitchPressFlag = TRUE;
				StatusFlag.stStatusFlag.bParameterSet = FALSE;
				sscanf((const char *)ulirgStoreLetter,"%f",&fUserInput);
				if (( TEMPERATURE_LOW_LIMIT_VALIDATION <= fUserInput ) && ( TEMPERATURE_HIGH_LIMIT_VALIDATION >= fUserInput ))
				{
					if ( SCREEN_HYSTERESIS_1 == uiCurrentScreen )
					{
						if ( 1 == ucHystSetVariableCounter )
						{
							stConfig_Value.fConfigHysInlineBasePos = fUserInput;
							fnOPConfig_Send(CONFIG_INLINE_BASE_TEMP_SETTING,BASE_HYST_POS);
						}
						else if ( 2 == ucHystSetVariableCounter )
						{
							stConfig_Value.fConfigHysInlineBaseNeg = fUserInput;
							fnOPConfig_Send(CONFIG_INLINE_BASE_TEMP_SETTING,BASE_HYST_NEG);
						}
						else	if ( 3 == ucHystSetVariableCounter )
						{
							stConfig_Value.fConfigInlineHysHardPos = fUserInput;
							fnOPConfig_Send(CONFIG_INLINE_HARD_TEMP_SETTING,HARD_HYST_POS);
						}
						else if ( 4 == ucHystSetVariableCounter )
						{
							stConfig_Value.fConfigInlineHysHardNeg = fUserInput;
							fnOPConfig_Send(CONFIG_INLINE_HARD_TEMP_SETTING,HARD_HYST_NEG);
						}
						else	if ( 5 == ucHystSetVariableCounter )
						{
							stConfig_Value.fConfigHysAntiFrzPos = fUserInput;
							fnOPConfig_Send(CONFIG_BASE_TEMP_SETTING,ANTIFRZ_HYST_POS);
						}
						else if ( 6 == ucHystSetVariableCounter )
						{
							stConfig_Value.fConfigHysAntiFrzNeg = fUserInput;
							fnOPConfig_Send(CONFIG_BASE_TEMP_SETTING,ANTIFRZ_HYST_NEG);
						}
					}
					else if ( SCREEN_HYSTERESIS_2 == uiCurrentScreen )
					{
						if ( 1 == ucHystSetVariableCounter )
						{
							stConfig_Value.fConfigHysRecircPresPos = fUserInput;
							fnOPConfig_Send(CONFIG_RECIRC_PRESSURE,RECIRC_PRES_HYST_POS);
						}
						else if ( 2 == ucHystSetVariableCounter )
						{
							stConfig_Value.fConfigHysRecircPresNeg = fUserInput;
							fnOPConfig_Send(CONFIG_RECIRC_PRESSURE,RECIRC_PRES_HYST_NEG);
						}
						else	if ( 3 == ucHystSetVariableCounter )
						{
							stConfig_Value.fConfigHysSprayPresPos = fUserInput;
							fnOPConfig_Send(CONFIG_SPRAY_PRESSURE,SPRAY_PRES_HYST_POS);
						}
						else if ( 4 == ucHystSetVariableCounter )
						{
							stConfig_Value.fConfigHysSprayPresNeg = fUserInput;
							fnOPConfig_Send(CONFIG_SPRAY_PRESSURE,SPRAY_PRES_HYST_NEG);
						}
					}
				}
				memset(ulirgStoreLetter,'\0',sizeof(ulirgStoreLetter));
				uiLetterStartPos = 340;
				uiLetter = 0;
				ucLetterPos = 0;
			}
						
			switch(ucCurrentSwitch)
			{
				case SWITCH12:
				{
					ucCurrentSwitch = 0;
					fnGr_FillRectangle((uiCursorXPos - 25), uiCursorYPos, 26,30,WHITE_CLR);
					
					switch(uiCurrentScreen)
					{
						case SCREEN_HYSTERESIS_1:
						{
							ucHystSetVariableCounter++;
							uiCursorXPos += 34;
							if ( 270 == uiCursorXPos )
							{
								ucHystSetVariableCounter--;
								uiCursorXPos -= 34;	
							}	
						}break;
						case SCREEN_HYSTERESIS_2:
						{
							ucHystSetVariableCounter++;
							uiCursorXPos += 34;
							if ( 202 == uiCursorXPos )
							{
								ucHystSetVariableCounter--;
								uiCursorXPos -= 34;	
							}	
						}break;
					}
				}break;
				case SWITCH11:
				{
					ucCurrentSwitch = 0;
					fnGr_FillRectangle((uiCursorXPos - 25), uiCursorYPos, 26,30,WHITE_CLR);
					
					switch(uiCurrentScreen)
					{
						case SCREEN_HYSTERESIS_1:
						case SCREEN_HYSTERESIS_2:
						{
							ucHystSetVariableCounter--;
							uiCursorXPos -= 34;
							if ( 32 == uiCursorXPos )
							{
								uiCursorXPos += 34;
								ucHystSetVariableCounter++;
							}
						}break;
					}	
				}break;
			}
		}break;
		case SCREEN_LIQUID_LEVEL_1:
		{
			if (( TRUE == StatusFlag.stStatusFlag.bParameterSet ) && ( '\0' != ulirgStoreLetter[0] ))
			{
				ucSwitchPressFlag = TRUE;
				StatusFlag.stStatusFlag.bParameterSet = FALSE;
				
					if ( '\0' != ulirgStoreLetter[0] )
					{
						sscanf((const char *)ulirgStoreLetter,"%f",&fUserInput);
						if (( LL_LOW_LIMIT_VALIDATION <= fUserInput ) && 
								( LL_HIGH_LIMIT_VALIDATION >= fUserInput ))
						{
							if ( 1 == ucLiquidLSetVariableCounter1 )
							{
								stConfig_Value.fConfigBaseTankHeight = fUserInput;
								fnOPConfig_Send(CONFIG_TANK_HEIGHT,BASE_MAT_TANK_HEIGHT);
							}
							else if ( 2 == ucLiquidLSetVariableCounter1 )
							{
								stConfig_Value.fConfigHardTankHeight = fUserInput;
								fnOPConfig_Send(CONFIG_TANK_HEIGHT,HARD_MAT_TANK_HEIGHT);
							}
							else if ( 3 == ucLiquidLSetVariableCounter1 )
							{
								stConfig_Value.fConfigHardLiquidLevelHTH = fUserInput;
								fnOPConfig_Send(CONFIG_HARD_LEVEL_SETTING,HIGH_TH);
							}
							else if ( 4 == ucLiquidLSetVariableCounter1 )
							{
								stConfig_Value.fConfigHardLiquidLevelLTH = fUserInput;
								fnOPConfig_Send(CONFIG_HARD_LEVEL_SETTING,LOW_TH);
							}
							else if ( 5 == ucLiquidLSetVariableCounter1 )
							{
								stConfig_Value.fConfigBaseLiquidLevelHTH = fUserInput;
								fnOPConfig_Send(CONFIG_BASE_LEVEL_SETTING,HIGH_TH);
							}
							else if ( 6 == ucLiquidLSetVariableCounter1 )
							{
								stConfig_Value.fConfigBaseLiquidLevelLTH = fUserInput;
								fnOPConfig_Send(CONFIG_BASE_LEVEL_SETTING,LOW_TH);
							}
						}
					}
				
					ucUserInput = 0;
					fUserInput = 0;
					memset(ulirgStoreLetter,'\0',sizeof(ulirgStoreLetter));
					uiLetterStartPos = 340;
					uiLetter = 0;
					ucLetterPos = 0;
			}
			
			switch(ucCurrentSwitch)
			{
				case SWITCH12:
				{
					ucCurrentSwitch = 0;
					fnGr_FillRectangle((uiCursorXPos - 25), uiCursorYPos, 26,30,WHITE_CLR);
					
					switch(uiCurrentScreen)
					{
						case SCREEN_LIQUID_LEVEL_1:
						{
							ucLiquidLSetVariableCounter1++;
							uiCursorXPos += 34;
							if ( 270 == uiCursorXPos )
							{
								ucLiquidLSetVariableCounter1--;
								uiCursorXPos -= 34;	
								break;
							}	
						}break;
					}
				}break;
					
				
				case SWITCH11:
				{	
					ucCurrentSwitch = 0;
					fnGr_FillRectangle((uiCursorXPos - 25), uiCursorYPos, 26,30,RGB(255,255,255));
					
					switch(uiCurrentScreen)
					{
						case SCREEN_LIQUID_LEVEL_1:
						{
							ucLiquidLSetVariableCounter1--;
							uiCursorXPos -= 34;
							if ( 32 == uiCursorXPos )
							{
								uiCursorXPos += 34;
								ucLiquidLSetVariableCounter1++;
								break;
							}
							
						}break;
					}	
				}break;
			}
		}break;
		case SCREEN_LIQUID_LEVEL_2:
		{
			if (( TRUE == StatusFlag.stStatusFlag.bParameterSet ) && ( '\0' != ulirgStoreLetter[0] ))
			{
				ucSwitchPressFlag = TRUE;
				StatusFlag.stStatusFlag.bParameterSet = FALSE;
				
				if ( '\0' != ulirgStoreLetter[0] )
				{
					if ( 1 == ucLiquidLSetVariableCounter1 )
						{
							sscanf((const char *)ulirgStoreLetter,"%hd",&iUserInput);
							if (( 0 < iUserInput ) && ( 9999 >= iUserInput ))
							{
								stConfig_Value.uiConfigLLWindowSize = iUserInput;							
								fnOPConfig_Send(CONFIG_LL_SAMPLE_RATE,0);
							}
						}
				}
				ucUserInput = 0;
				iUserInput = 0;
				memset(ulirgStoreLetter,'\0',sizeof(ulirgStoreLetter));
				uiLetterStartPos = 340;
				uiLetter = 0;
				ucLetterPos = 0;
			}
			
		}break;
		
		case SCREEN_VCM_PP_SETTING_1:
		case SCREEN_VCM_PP_SETTING_2:
		{
			if (( TRUE == StatusFlag.stStatusFlag.bParameterSet ) && ( '\0' != ulirgStoreLetter[0] ))
			{
				ucSwitchPressFlag = TRUE;
				StatusFlag.stStatusFlag.bParameterSet = FALSE;
				if ( '\0' != ulirgStoreLetter[0] )
				{
					sscanf((const char *)ulirgStoreLetter,"%hd",&iUserInput);
					
					switch(uiCurrentScreen)
					{
						case SCREEN_VCM_PP_SETTING_1:
						{
							if ( 1 == ucVCMPeripheralSetCounter )
							{
								if ( ucAlphaNum[1][9][0] == ulirgStoreLetter[0] )
								{
									stConfig_Value.ucConfigConnPressureGuage1 = 0;
								}
								else if(( 0 < iUserInput) && ( 4 >= iUserInput ) && 
										( iUserInput != stConfig_Value.ucConfigConnPressureGuage2) &&
										( iUserInput != stConfig_Value.ucConfigConnPressureGuage3))
								{
									stConfig_Value.ucConfigConnPressureGuage1 = iUserInput;
								}
							}
							else if ( 2 == ucVCMPeripheralSetCounter )
							{
								if ( ucAlphaNum[1][9][0] == ulirgStoreLetter[0] )
								{
									stConfig_Value.ucConfigConnPressureGuage2 = 0;
								}
								else if(( 0 < iUserInput) && ( 4 >= iUserInput ) && 
										( iUserInput != stConfig_Value.ucConfigConnPressureGuage1) &&
										( iUserInput != stConfig_Value.ucConfigConnPressureGuage3))
								{
									stConfig_Value.ucConfigConnPressureGuage2 = iUserInput;
								}
							}
							else	if ( 3 == ucVCMPeripheralSetCounter )
							{
								if ( ucAlphaNum[1][9][0] == ulirgStoreLetter[0] )
								{
									stConfig_Value.ucConfigConnPressureGuage3 = 0;
								}
								else if(( 0 < iUserInput) && ( 4 >= iUserInput ) && 
										( iUserInput != stConfig_Value.ucConfigConnPressureGuage2) &&
										( iUserInput != stConfig_Value.ucConfigConnPressureGuage1))
								{
									stConfig_Value.ucConfigConnPressureGuage3 = iUserInput;
								}
							}
							else if ( 4 == ucVCMPeripheralSetCounter )
							{
								if ( ucAlphaNum[1][9][0] == ulirgStoreLetter[0] )
								{
									stConfig_Value.ucConfigConnInsideEE07 = 0;
								}
								else if(( 0 < iUserInput) && ( 3 >= iUserInput ) && 
										( iUserInput != stConfig_Value.ucConfigConnOutsideEE07 ))
								{
									stConfig_Value.ucConfigConnInsideEE07 = iUserInput;
								}
							}
							else if ( 5 == ucVCMPeripheralSetCounter )
							{
								if ( ucAlphaNum[1][9][0] == ulirgStoreLetter[0] )
								{
									stConfig_Value.ucConfigConnOutsideEE07 = 0;
								}
								else if(( 0 < iUserInput) && ( 3 >= iUserInput ) && 
										( iUserInput != stConfig_Value.ucConfigConnInsideEE07 ))
								{
									stConfig_Value.ucConfigConnOutsideEE07 = iUserInput;
								}
							}
						}break;
						case SCREEN_VCM_PP_SETTING_2: 
						{
							if ( 1 == ucVCMPeripheralSetCounter )
							{
								if ( ucAlphaNum[1][9][0] == ulirgStoreLetter[0] )
								{
									stConfig_Value.ucConfigConnStallvalve = 0;
								}
								else if(( 0 < iUserInput) && ( 2 >= iUserInput )) /*change15*/
								{
									stConfig_Value.ucConfigConnStallvalve = iUserInput;
								}
							}
							else if ( 2 == ucVCMPeripheralSetCounter )
							{
								if ( ucAlphaNum[1][9][0] == ulirgStoreLetter[0] )
								{
									stConfig_Value.ucConfigConnLinearTransducer = 0;
								}
								else if(( 0 < iUserInput) && ( 2 >= iUserInput ))
								{
									stConfig_Value.ucConfigConnLinearTransducer = iUserInput;
								}
							}
								/*change15*/
							else if ( 4 == ucVCMPeripheralSetCounter )
							{
								if ( ucAlphaNum[1][9][0] == ulirgStoreLetter[0] )
								{
									stConfig_Value.ucConfigConnRecircvalve = 0;
								}
								else if(( 2 < iUserInput) && ( 4 >= iUserInput ))
								{
									stConfig_Value.ucConfigConnRecircvalve = iUserInput;
								}	
							}
						}break;
						default:
							break;
					}
					/*change15*/
					if( stConfig_Value.ucConfigConnPressureGuage1 	== 0 || stConfig_Value.ucConfigConnPressureGuage2 	== 0 ||
							stConfig_Value.ucConfigConnPressureGuage3 == 0 || stConfig_Value.ucConfigConnLinearTransducer == 0 ||
							stConfig_Value.ucConfigConnInsideEE07 		== 0 || stConfig_Value.ucConfigConnOutsideEE07 			== 0 ||
							stConfig_Value.ucConfigConnStallvalve 		== 0 || stConfig_Value.ucConfigConnRecircvalve 			== 0 
						)
					{
						StatusFlag.stStatusFlag.bVCMPortSetFail = TRUE;
					}
					else
					{
						StatusFlag.stStatusFlag.bVCMPortSetFail = FALSE;
					}
				}
				
				
				memset(ulirgStoreLetter,'\0',sizeof(ulirgStoreLetter));
				uiLetterStartPos = 340;
				uiLetter = 0;
				ucLetterPos = 0;
				iUserInput = 0;
			}
			
			switch(ucCurrentSwitch)
			{
				case SWITCH12:
				{
					ucCurrentSwitch = 0;
					fnGr_FillRectangle((uiCursorXPos - 25), uiCursorYPos, 26,30,WHITE_CLR);
					
					switch(uiCurrentScreen)
					{
						case SCREEN_VCM_PP_SETTING_1:
						{
							ucVCMPeripheralSetCounter++;
							uiCursorXPos += 34;
							if ( 270 == uiCursorXPos )
							{
								ucVCMPeripheralSetCounter--;
								uiCursorXPos -= 34;	
								break;
							}	
						}break;
						case SCREEN_VCM_PP_SETTING_2:
						{
							ucVCMPeripheralSetCounter++;
							uiCursorXPos += 34;
							if ( 236 == uiCursorXPos )
							{
								ucVCMPeripheralSetCounter--;
								uiCursorXPos -= 34;	
								break;
							}	
						}break;
					}
				}break;
					
				
				case SWITCH11:
				{	
					ucCurrentSwitch = 0;
					fnGr_FillRectangle((uiCursorXPos - 25), uiCursorYPos, 26,30,RGB(255,255,255));
					
					switch(uiCurrentScreen)
					{
						case SCREEN_VCM_PP_SETTING_1:
						case SCREEN_VCM_PP_SETTING_2:
						{
							ucVCMPeripheralSetCounter--;
							uiCursorXPos -= 34;
							if ( 66 == uiCursorXPos )
							{
								uiCursorXPos += 34;
								ucVCMPeripheralSetCounter++;
								break;
							}
							
						}break;
					}	
				}break;
			}
		}break;
		
		case SCREEN_HCM_PP_SETTING_1:
		case SCREEN_HCM_PP_SETTING_2:
		{
			if (( TRUE == StatusFlag.stStatusFlag.bParameterSet ) && ( '\0' != ulirgStoreLetter[0] ))
			{
				StatusFlag.stStatusFlag.bParameterSet = FALSE;
				if ( '\0' != ulirgStoreLetter[0] )
				{
					ucSwitchPressFlag = TRUE;
					sscanf((const char *)ulirgStoreLetter,"%hd",&iUserInput);
					switch(uiCurrentScreen)
					{
						case SCREEN_HCM_PP_SETTING_1:
						{
							if ( 1 == ucHCMPeripheralSetCounter )
							{
								if ( ucAlphaNum[1][9][0] == ulirgStoreLetter[0] )
								{
									stConfig_Value.ucConfig_ConnBaseinlineHeater = 0;
								}
								else if(( 0 < iUserInput) && ( 8 >= iUserInput ) && 
										( iUserInput != stConfig_Value.ucConfig_ConnHardinlineHeater ) &&
										( iUserInput != stConfig_Value.ucConfig_ConnAntifreezeHeater ) &&
										( iUserInput != stConfig_Value.ucConfig_ConnHardHeater ))
								{
									stConfig_Value.ucConfig_ConnBaseinlineHeater = iUserInput;
								}
							}
							else if ( 2 == ucHCMPeripheralSetCounter )
							{
								if ( ucAlphaNum[1][9][0] == ulirgStoreLetter[0] )
								{
									stConfig_Value.ucConfig_ConnHardinlineHeater = 0;
								}
								else if(( 0 < iUserInput) && ( 8 >= iUserInput ) && 
										( iUserInput != stConfig_Value.ucConfig_ConnBaseinlineHeater ) &&
										( iUserInput != stConfig_Value.ucConfig_ConnAntifreezeHeater ) &&
										( iUserInput != stConfig_Value.ucConfig_ConnHardHeater ))
								{
									stConfig_Value.ucConfig_ConnHardinlineHeater = iUserInput;
								}
							}
							else	if ( 3 == ucHCMPeripheralSetCounter )
							{
								if ( ucAlphaNum[1][9][0] == ulirgStoreLetter[0] )
								{
									stConfig_Value.ucConfig_ConnAntifreezeHeater = 0;
								}
								else if(( 0 < iUserInput) && ( 8 >= iUserInput ) && 
										( iUserInput != stConfig_Value.ucConfig_ConnBaseinlineHeater ) &&
										( iUserInput != stConfig_Value.ucConfig_ConnHardinlineHeater ) &&
										( iUserInput != stConfig_Value.ucConfig_ConnHardHeater ))
								{
									stConfig_Value.ucConfig_ConnAntifreezeHeater = iUserInput;
								}
							}
							else if ( 4 == ucHCMPeripheralSetCounter )
							{
								if ( ucAlphaNum[1][9][0] == ulirgStoreLetter[0] )
								{
									stConfig_Value.ucConfig_ConnHardHeater = 0;
								}
								else if(( 0 < iUserInput) && ( 8 >= iUserInput ) && 
										( iUserInput != stConfig_Value.ucConfig_ConnBaseinlineHeater ) &&
										( iUserInput != stConfig_Value.ucConfig_ConnHardinlineHeater ) &&
										( iUserInput != stConfig_Value.ucConfig_ConnAntifreezeHeater ))
								{
									stConfig_Value.ucConfig_ConnHardHeater = iUserInput;
								}
							}
							else if ( 5 == ucHCMPeripheralSetCounter )
							{
								if ( ucAlphaNum[1][9][0] == ulirgStoreLetter[0] )
								{
									stConfig_Value.ucConfig_ConnBaseInlineTher = 0;
								}
								else if(( 0 < iUserInput) && ( 6 >= iUserInput ) && 
										( iUserInput != stConfig_Value.ucConfig_ConnHardInlineTher ) &&
										( iUserInput != stConfig_Value.ucConfig_ConnAntifreezeTher ) &&
										( iUserInput != stConfig_Value.ucConfig_ConnBasereturnlineTher ))
								{
									stConfig_Value.ucConfig_ConnBaseInlineTher = iUserInput;
								}
							}
						}break;
						case SCREEN_HCM_PP_SETTING_2: 
						{
							if ( 1 == ucHCMPeripheralSetCounter )
							{
								if ( ucAlphaNum[1][9][0] == ulirgStoreLetter[0] )
								{
									stConfig_Value.ucConfig_ConnHardInlineTher = 0;
								}
								else if(( 0 < iUserInput) && ( 6 >= iUserInput ) && 
										( iUserInput != stConfig_Value.ucConfig_ConnBaseInlineTher ) &&
										( iUserInput != stConfig_Value.ucConfig_ConnAntifreezeTher ) &&
										( iUserInput != stConfig_Value.ucConfig_ConnBasereturnlineTher ))
								{
									stConfig_Value.ucConfig_ConnHardInlineTher = iUserInput;
								}
							}
							else if ( 2 == ucHCMPeripheralSetCounter )
							{
								if ( ucAlphaNum[1][9][0] == ulirgStoreLetter[0] )
								{
									stConfig_Value.ucConfig_ConnAntifreezeTher = 0;
								}
								else if(( 0 < iUserInput) && ( 6 >= iUserInput ) && 
										( iUserInput != stConfig_Value.ucConfig_ConnBaseInlineTher ) &&
										( iUserInput != stConfig_Value.ucConfig_ConnHardInlineTher ) &&
										( iUserInput != stConfig_Value.ucConfig_ConnBasereturnlineTher ))
								{
									stConfig_Value.ucConfig_ConnAntifreezeTher = iUserInput;
								}
							}
							else	if ( 3 == ucHCMPeripheralSetCounter )
							{
								if ( ucAlphaNum[1][9][0] == ulirgStoreLetter[0] )
								{
									stConfig_Value.ucConfig_ConnBasereturnlineTher = 0;
								}
								else if(( 0 < iUserInput) && ( 6 >= iUserInput ) && 
										( iUserInput != stConfig_Value.ucConfig_ConnBaseInlineTher ) &&
										( iUserInput != stConfig_Value.ucConfig_ConnHardInlineTher ) &&
										( iUserInput != stConfig_Value.ucConfig_ConnAntifreezeTher ))
								{
									stConfig_Value.ucConfig_ConnBasereturnlineTher = iUserInput;
								}
							}
							else	if ( 4 == ucHCMPeripheralSetCounter )
							{
								if ( ucAlphaNum[1][9][0] == ulirgStoreLetter[0] )
								{
									stConfig_Value.ucConfig_ConnBaseTankLL = 0;
								}
								else if(( 0 < iUserInput) && ( 3 >= iUserInput ) && 
										( iUserInput != stConfig_Value.ucConfig_ConnHardTankLL ))
								{
									stConfig_Value.ucConfig_ConnBaseTankLL = iUserInput;
								}
							}
							else	if ( 5 == ucHCMPeripheralSetCounter )
							{
								if ( ucAlphaNum[1][9][0] == ulirgStoreLetter[0] )
								{
									stConfig_Value.ucConfig_ConnHardTankLL = 0;
								}
								else if(( 0 < iUserInput) && ( 3 >= iUserInput ) && 
										( iUserInput != stConfig_Value.ucConfig_ConnBaseTankLL ))
								{
									stConfig_Value.ucConfig_ConnHardTankLL = iUserInput;
								}
							}
						}break;
						default:
							break;
					}
				}
				
				if(	stConfig_Value.ucConfig_ConnBaseInlineTherRev 		== 0	|| stConfig_Value.ucConfig_ConnHardinlineHeaterRev	== 0 ||
						stConfig_Value.ucConfig_ConnHardInlineTherRev 		== 0 	|| stConfig_Value.ucConfig_ConnAntifreezeHeaterRev	== 0 ||
						stConfig_Value.ucConfig_ConnAntifreezeTherRev 		== 0 	|| stConfig_Value.ucConfig_ConnHardHeaterRev 				== 0 ||
						stConfig_Value.ucConfig_ConnBasereturnlineTherRev == 0	|| stConfig_Value.ucConfig_ConnBaseTankLLRev 	 			== 0 ||
						stConfig_Value.ucConfig_ConnBaseinlineHeaterRev 	== 0 	|| stConfig_Value.ucConfig_ConnHardTankLLRev				== 0 )
					{
						StatusFlag.stStatusFlag.bHCMPortSetFail = TRUE;
					}
					else
					{
						StatusFlag.stStatusFlag.bHCMPortSetFail = FALSE;
					}
				memset(ulirgStoreLetter,'\0',sizeof(ulirgStoreLetter));
				uiLetterStartPos = 340;
				uiLetter = 0;
				ucLetterPos = 0;
				iUserInput = 0;
			}
			
			switch(ucCurrentSwitch)
			{
				case SWITCH12:
				{
					ucCurrentSwitch = 0;
					fnGr_FillRectangle((uiCursorXPos - 25), uiCursorYPos, 26,30,WHITE_CLR);
					
					switch(uiCurrentScreen)
					{
						case SCREEN_HCM_PP_SETTING_1:
						case SCREEN_HCM_PP_SETTING_2:
						{
							ucHCMPeripheralSetCounter++;
							uiCursorXPos += 34;
							if ( 270 == uiCursorXPos )
							{
								ucHCMPeripheralSetCounter--;
								uiCursorXPos -= 34;	
								break;
							}	
						}break;
					}
				}break;
					
				
				case SWITCH11:
				{	
					ucCurrentSwitch = 0;
					fnGr_FillRectangle((uiCursorXPos - 25), uiCursorYPos, 26,30,RGB(255,255,255));
					
					switch(uiCurrentScreen)
					{
						case SCREEN_HCM_PP_SETTING_1:
						case SCREEN_HCM_PP_SETTING_2:
						{
							ucHCMPeripheralSetCounter--;
							uiCursorXPos -= 34;
							if ( 66 == uiCursorXPos )
							{
								uiCursorXPos += 34;
								ucHCMPeripheralSetCounter++;
								break;
							}
							
						}break;
					}	
				}break;
			}
		}break;
		
		case SCREEN_OPMM_PP_SETTING_1:
		case SCREEN_OPMM_PP_SETTING_2:
		case SCREEN_OPMM_PP_SETTING_3:
		{
			if (( TRUE == StatusFlag.stStatusFlag.bParameterSet ) && ( '\0' != ulirgStoreLetter[0] ))
			{
				ucSwitchPressFlag = TRUE;
				StatusFlag.stStatusFlag.bParameterSet = FALSE;
				sscanf((const char *)ulirgStoreLetter,"%hd",&iUserInput);
				if ( '\0' != ulirgStoreLetter[0] )
				{
					switch(uiCurrentScreen)
					{
						case SCREEN_OPMM_PP_SETTING_1:
						{
							if ( 1 == ucOPMMPeripheralSetCounter )
							{
								if(( 0 <= iUserInput) && ( 8 >= iUserInput ) && 
										( iUserInput != stConfig_Value.ucConfigConnGunposition ) &&
										( iUserInput != stConfig_Value.ucConfigConnABvalve ) &&
										( iUserInput != stConfig_Value.ucConfigConnSolventvalve ) &&
										( iUserInput != stConfig_Value.ucConfigConnForwardvalve ) &&
										( iUserInput != stConfig_Value.ucConfigConnBackwardvalve ))
								{
									stConfig_Value.ucConfigConnSprayvalve = iUserInput;
								}
							}
							else if ( 2 == ucOPMMPeripheralSetCounter )
							{
								if(( 0 <= iUserInput) && ( 8 >= iUserInput ) && 
										( iUserInput != stConfig_Value.ucConfigConnSprayvalve ) &&
										( iUserInput != stConfig_Value.ucConfigConnABvalve ) &&
										( iUserInput != stConfig_Value.ucConfigConnSolventvalve ) &&
										( iUserInput != stConfig_Value.ucConfigConnForwardvalve ) &&
										( iUserInput != stConfig_Value.ucConfigConnBackwardvalve ))
								{
									stConfig_Value.ucConfigConnGunposition = iUserInput;
								}
							}
							else	if ( 3 == ucOPMMPeripheralSetCounter )
							{
								if(( 0 <= iUserInput) && ( 8 >= iUserInput ) && 
										( iUserInput != stConfig_Value.ucConfigConnSprayvalve ) &&
										( iUserInput != stConfig_Value.ucConfigConnGunposition ) &&
										( iUserInput != stConfig_Value.ucConfigConnSolventvalve ) &&
										( iUserInput != stConfig_Value.ucConfigConnForwardvalve ) &&
										( iUserInput != stConfig_Value.ucConfigConnBackwardvalve ))
								{
									stConfig_Value.ucConfigConnABvalve = iUserInput;
								}
							}
							else if ( 4 == ucOPMMPeripheralSetCounter )
							{
								if(( 0 <= iUserInput) && ( 8 >= iUserInput ) && 
										( iUserInput != stConfig_Value.ucConfigConnSprayvalve ) &&
										( iUserInput != stConfig_Value.ucConfigConnGunposition ) &&
										( iUserInput != stConfig_Value.ucConfigConnABvalve ) &&
										( iUserInput != stConfig_Value.ucConfigConnForwardvalve ) &&
										( iUserInput != stConfig_Value.ucConfigConnBackwardvalve ))
								{
									stConfig_Value.ucConfigConnSolventvalve = iUserInput;
								}
							}
							else if ( 5 == ucOPMMPeripheralSetCounter )
							{
								if(( 0 <= iUserInput) && ( 8 >= iUserInput ) && 
										( iUserInput != stConfig_Value.ucConfigConnSprayvalve ) &&
										( iUserInput != stConfig_Value.ucConfigConnGunposition ) &&
										( iUserInput != stConfig_Value.ucConfigConnABvalve ) &&
										( iUserInput != stConfig_Value.ucConfigConnSolventvalve ) &&
										( iUserInput != stConfig_Value.ucConfigConnBackwardvalve ))
								{
									stConfig_Value.ucConfigConnForwardvalve = iUserInput;
								}
							}
						}break;
						case SCREEN_OPMM_PP_SETTING_2: 
						{
							if ( 1 == ucOPMMPeripheralSetCounter )
							{
								if(( 0 <= iUserInput) && ( 8 >= iUserInput ) && 
										( iUserInput != stConfig_Value.ucConfigConnSprayvalve ) &&
										( iUserInput != stConfig_Value.ucConfigConnGunposition ) &&
										( iUserInput != stConfig_Value.ucConfigConnABvalve ) &&
										( iUserInput != stConfig_Value.ucConfigConnSolventvalve ) &&
										( iUserInput != stConfig_Value.ucConfigConnForwardvalve ))
								{
									stConfig_Value.ucConfigConnBackwardvalve = iUserInput;
								}
							}
							else if ( 2 == ucOPMMPeripheralSetCounter )
							{
								if(( 0 <= iUserInput) && ( 4 >= iUserInput ) && 
										( iUserInput != stConfig_Value.ucConfigNCTempSensor2 ) &&
										( iUserInput != stConfig_Value.ucConfigNCTempSensor3 ) &&
										( iUserInput != stConfig_Value.ucConfigNCTempSensor4 ))
								{
									stConfig_Value.ucConfigNCTempSensor1 = iUserInput;
								}
							}
							else	if ( 3 == ucOPMMPeripheralSetCounter )
							{
								if(( 0 <= iUserInput) && ( 4 >= iUserInput ) && 
										( iUserInput != stConfig_Value.ucConfigNCTempSensor1 ) &&
										( iUserInput != stConfig_Value.ucConfigNCTempSensor3 ) &&
										( iUserInput != stConfig_Value.ucConfigNCTempSensor4 ))
								{
									stConfig_Value.ucConfigNCTempSensor2 = iUserInput;
								}
							}
							else	if ( 4 == ucOPMMPeripheralSetCounter )
							{
								if(( 0 <= iUserInput) && ( 4 >= iUserInput ) && 
										( iUserInput != stConfig_Value.ucConfigNCTempSensor1 ) &&
										( iUserInput != stConfig_Value.ucConfigNCTempSensor2 ) &&
										( iUserInput != stConfig_Value.ucConfigNCTempSensor4 ))
								{
									stConfig_Value.ucConfigNCTempSensor3 = iUserInput;
								}
							}
							else	if ( 5 == ucOPMMPeripheralSetCounter )
							{
								if(( 0 <= iUserInput) && ( 4 >= iUserInput ) && 
										( iUserInput != stConfig_Value.ucConfigNCTempSensor1 ) &&
										( iUserInput != stConfig_Value.ucConfigNCTempSensor2 ) &&
										( iUserInput != stConfig_Value.ucConfigNCTempSensor3 ))
								{
									stConfig_Value.ucConfigNCTempSensor4 = iUserInput;
								}
							}
						}break;
						default:
							break;
					}
				}
				
				
				memset(ulirgStoreLetter,'\0',sizeof(ulirgStoreLetter));
				uiLetterStartPos = 340;
				uiLetter = 0;
				ucLetterPos = 0;
			}
			
			switch(ucCurrentSwitch)
			{
				case SWITCH12:
				{
					ucCurrentSwitch = 0;
					fnGr_FillRectangle((uiCursorXPos - 25), uiCursorYPos, 26,30,WHITE_CLR);
					
					switch(uiCurrentScreen)
					{
						case SCREEN_OPMM_PP_SETTING_1:
						case SCREEN_OPMM_PP_SETTING_2:
						{
							ucOPMMPeripheralSetCounter++;
							uiCursorXPos += 34;
							if ( 270 == uiCursorXPos )
							{
								ucOPMMPeripheralSetCounter--;
								uiCursorXPos -= 34;	
								break;
							}	
						}break;
						case SCREEN_OPMM_PP_SETTING_3:
						{
							ucOPMMPeripheralSetCounter++;
							uiCursorXPos += 34;
							if ( 168 == uiCursorXPos )
							{
								ucOPMMPeripheralSetCounter--;
								uiCursorXPos -= 34;	
								break;
							}	
						}break;
					}
				}break;
					
				
				case SWITCH11:
				{	
					ucCurrentSwitch = 0;
					fnGr_FillRectangle((uiCursorXPos - 25), uiCursorYPos, 26,30,RGB(255,255,255));
					
					switch(uiCurrentScreen)
					{
						case SCREEN_OPMM_PP_SETTING_1:
						case SCREEN_OPMM_PP_SETTING_2:
						case SCREEN_OPMM_PP_SETTING_3:
						{
							ucOPMMPeripheralSetCounter--;
							uiCursorXPos -= 34;
							if ( 66 == uiCursorXPos )
							{
								uiCursorXPos += 34;
								ucOPMMPeripheralSetCounter++;
								break;
							}
							
						}break;
					}	
				}break;
			}
		}break;
		case SCREEN_SCU_PP_SETTING:
		{
			switch(ucCurrentSwitch)
			{
				case SWITCH12:
				{
					ucCurrentSwitch = 0;
					fnGr_FillRectangle((uiCursorXPos - 25), uiCursorYPos, 26,30,WHITE_CLR);
					
					switch(uiCurrentScreen)
					{
						case SCREEN_SCU_PP_SETTING:
						{
							ucSCUPeripheralSetCounter++;
							uiCursorXPos += 34;
							if ( 270 == uiCursorXPos )
							{
								ucSCUPeripheralSetCounter--;
								uiCursorXPos -= 34;	
								break;
							}	
						}break;
					}
				}break;
					
				
				case SWITCH11:
				{	
					ucCurrentSwitch = 0;
					fnGr_FillRectangle((uiCursorXPos - 25), uiCursorYPos, 26,30,RGB(255,255,255));
					
					switch(uiCurrentScreen)
					{
						case SCREEN_SCU_PP_SETTING:
						{
							ucSCUPeripheralSetCounter--;
							uiCursorXPos -= 34;
							if ( 66 == uiCursorXPos )
							{
								uiCursorXPos += 34;
								ucSCUPeripheralSetCounter++;
								break;
							}
							
						}break;
					}	
				}break;
			}
		}break;
		
		/*change3*/
			case SCREEN_ETH_IP_DETAILS:
		{
			if (( TRUE == StatusFlag.stStatusFlag.bParameterSet ) && ( '\0' != ulirgStoreLetter[0] ))
			{
				ucSwitchPressFlag = TRUE;
				StatusFlag.stStatusFlag.bParameterSet = FALSE;
				DEBUG_PRINTF("%s \r\n",ulirgStoreLetter);
				if ( 1 == ucETHIPSetVariableCounter )
				{
					DEBUG_PRINTF(">>>>%s \r\n",ulirgStoreLetter);
					stCurrent_Value.ucEthIPSlot1 = atoi(strtok((char *)ulirgStoreLetter,"."));
					stCurrent_Value.ucEthIPSlot2 = atoi(strtok((char *)NULL,"."));
					stCurrent_Value.ucEthIPSlot3 = atoi(strtok((char *)NULL,"."));
					stCurrent_Value.ucEthIPSlot4 = atoi(strtok((char *)NULL,"."));
//					sscanf((const char *)ulirgStoreLetter,"%hd",&iUserInput);
//					if( (0 < iUserInput) && (100 > iUserInput))
//					{
//						stConfig_Value.uiConfigCylinderCount = iUserInput;
						fnOPConfig_Send(CONFIG_ETH_WIFI_IP_SN_GW_PN,ETHERNETIP);
//					}
				}
				else if ( 2 == ucETHIPSetVariableCounter )
				{
					stCurrent_Value.ucEthSubnetSlot1 = atoi(strtok((char *)ulirgStoreLetter,"."));
					stCurrent_Value.ucEthSubnetSlot2 = atoi(strtok((char *)NULL,"."));
					stCurrent_Value.ucEthSubnetSlot3 = atoi(strtok((char *)NULL,"."));
					stCurrent_Value.ucEthSubnetSlot4 = atoi(strtok((char *)NULL,"."));
//					sscanf((const char *)ulirgStoreLetter,"%f",&stConfig_Value.fConfigAreaofCylinderA);
					
				fnOPConfig_Send(CONFIG_ETH_WIFI_IP_SN_GW_PN,ETHERNETSUBNET);
				}
				else	if ( 3 == ucETHIPSetVariableCounter )
				{
					stCurrent_Value.ucEthGWSlot1 = atoi(strtok((char *)ulirgStoreLetter,"."));
					stCurrent_Value.ucEthGWSlot2 = atoi(strtok((char *)NULL,"."));
					stCurrent_Value.ucEthGWSlot3 = atoi(strtok((char *)NULL,"."));
					stCurrent_Value.ucEthGWSlot4 = atoi(strtok((char *)NULL,"."));
//					sscanf((const char *)ulirgStoreLetter,"%f",&stConfig_Value.fConfigAreaofCylinderB);
			fnOPConfig_Send(CONFIG_ETH_WIFI_IP_SN_GW_PN,ETHERNETGW);
				}
				else if ( 4 == ucETHIPSetVariableCounter )
				{
					sscanf((const char *)ulirgStoreLetter,"%hd",&iUserInput);
//					if( (0 < iUserInput) && (100 > iUserInput))
//					{
						stCurrent_Value.iEthPort= iUserInput;
						fnOPConfig_Send(CONFIG_ETH_WIFI_IP_SN_GW_PN,ETHERNETPORTNUMBER);
//					}
//					sscanf((const char *)ulirgStoreLetter,"%f",&stConfig_Value.fConfigAreaofCylinderC);
//					fnOPConfig_Send(CONFIG_CYLINDER_SURFACE_AREA,LEG_C);
				}
				
				memset(ulirgStoreLetter,'\0',sizeof(ulirgStoreLetter));
				uiLetterStartPos = 340;
				uiLetter = 0;
				ucLetterPos = 0;
			}
					
			switch(ucCurrentSwitch)
			{ 	
				case SWITCH12:
				{
					ucCurrentSwitch = 0;
					fnGr_FillRectangle((uiCursorXPos - 25), uiCursorYPos, 26,30,WHITE_CLR);
					
					switch(uiCurrentScreen)
					{
						case SCREEN_ETH_IP_DETAILS:
						{
							ucETHIPSetVariableCounter++;
							uiCursorXPos += 34;
							if ( 202 == uiCursorXPos )
							{
								ucETHIPSetVariableCounter--;
								uiCursorXPos -= 34;	
							}
						}break;
					}
				}break;
					
				
				case SWITCH11:
				{
					ucCurrentSwitch = 0;
					fnGr_FillRectangle((uiCursorXPos - 25), uiCursorYPos, 26,30,WHITE_CLR);
					
					switch(uiCurrentScreen)
					{
						case SCREEN_ETH_IP_DETAILS:
						{
							ucETHIPSetVariableCounter--;
							uiCursorXPos -= 34;
							if ( 32 == uiCursorXPos )
							{
								uiCursorXPos += 34;
								ucETHIPSetVariableCounter++;
							}
						}
					}	
					
				}break;
				
			}
		}break;
		
				/*change3*/
			case SCREEN_WIFI_IP_DETAILS:
		{
			if (( TRUE == StatusFlag.stStatusFlag.bParameterSet ) && ( '\0' != ulirgStoreLetter[0] ))
			{
				ucSwitchPressFlag = TRUE;
				StatusFlag.stStatusFlag.bParameterSet = FALSE;
				DEBUG_PRINTF("%s \r\n",ulirgStoreLetter);
				DEBUG_PRINTF("ucWIFIIPSetVariableCounter  : %d \r\n",ucWIFIIPSetVariableCounter);
				if ( 1 == ucWIFIIPSetVariableCounter )
				{
					DEBUG_PRINTF(">>>>%s \r\n",ulirgStoreLetter);
					stCurrent_Value.ucWiFiIPSlot1 = atoi(strtok((char *)ulirgStoreLetter,"."));
					stCurrent_Value.ucWiFiIPSlot2 = atoi(strtok((char *)NULL,"."));
					stCurrent_Value.ucWiFiIPSlot3 = atoi(strtok((char *)NULL,"."));
					stCurrent_Value.ucWiFiIPSlot4 = atoi(strtok((char *)NULL,"."));
//					sscanf((const char *)ulirgStoreLetter,"%hd",&iUserInput);
//					if( (0 < iUserInput) && (100 > iUserInput))
//					{
//						stConfig_Value.uiConfigCylinderCount = iUserInput;
						fnOPConfig_Send(CONFIG_ETH_WIFI_IP_SN_GW_PN,WIFIIP);
//					}
				}
				else if ( 2 == ucWIFIIPSetVariableCounter )
				{
					stCurrent_Value.ucWiFiSubnetSlot1 = atoi(strtok((char *)ulirgStoreLetter,"."));
					stCurrent_Value.ucWiFiSubnetSlot2 = atoi(strtok((char *)NULL,"."));
					stCurrent_Value.ucWiFiSubnetSlot3 = atoi(strtok((char *)NULL,"."));
					stCurrent_Value.ucWiFiSubnetSlot4 = atoi(strtok((char *)NULL,"."));
//					sscanf((const char *)ulirgStoreLetter,"%f",&stConfig_Value.fConfigAreaofCylinderA);
					
				fnOPConfig_Send(CONFIG_ETH_WIFI_IP_SN_GW_PN,WIFISUBNET);
				}
				else	if ( 3 == ucWIFIIPSetVariableCounter )
				{
					stCurrent_Value.ucWiFiGWSlot1 = atoi(strtok((char *)ulirgStoreLetter,"."));
					stCurrent_Value.ucWiFiGWSlot2 = atoi(strtok((char *)NULL,"."));
					stCurrent_Value.ucWiFiGWSlot3 = atoi(strtok((char *)NULL,"."));
					stCurrent_Value.ucWiFiGWSlot4 = atoi(strtok((char *)NULL,"."));
//					sscanf((const char *)ulirgStoreLetter,"%f",&stConfig_Value.fConfigAreaofCylinderB);
			fnOPConfig_Send(CONFIG_ETH_WIFI_IP_SN_GW_PN,WIFIGW);
				}
				else if ( 4 == ucWIFIIPSetVariableCounter )
				{
					sscanf((const char *)ulirgStoreLetter,"%hd",&iUserInput);
//					if( (0 < iUserInput) && (100 > iUserInput))
//					{
						stCurrent_Value.iWiFiPort= iUserInput;
						fnOPConfig_Send(CONFIG_ETH_WIFI_IP_SN_GW_PN,WIFIPORTNUMBER);
//					}
//					sscanf((const char *)ulirgStoreLetter,"%f",&stConfig_Value.fConfigAreaofCylinderC);
//					fnOPConfig_Send(CONFIG_CYLINDER_SURFACE_AREA,LEG_C);
				}
				else if ( 5 == ucWIFIIPSetVariableCounter )
				{
							memset(rgucSSID,'\0',sizeof(rgucSSID));
								
								strncpy((char *)stConfig_Value.rgucSSID1,(const char *)ulirgStoreLetter,7);
								stConfig_Value.rgucSSID1[7] = '\0';
								strcat((char *)rgucSSID,(const char *)stConfig_Value.rgucSSID1);
								
								strncpy((char *)stConfig_Value.rgucSSID2,(const char *)ulirgStoreLetter + 7,7);
								stConfig_Value.rgucSSID2[7] = '\0';
								strcat((char *)rgucSSID,(const char *)stConfig_Value.rgucSSID2);
								DEBUG_PRINTF("%s \r\n",rgucSSID);																
								fnOPConfig_Send(CONFIG_WIFI_SSID,0);
//					
				}
				else if ( 6 == ucWIFIIPSetVariableCounter )
				{
								memset(rgucPass,'\0',sizeof(rgucPass));
								
								strncpy((char *)stConfig_Value.rgucPass1,(const char *)ulirgStoreLetter,7);
								stConfig_Value.rgucPass1[7] = '\0';
								strcat((char *)rgucPass,(const char *)stConfig_Value.rgucPass1);
								
								strncpy((char *)stConfig_Value.rgucPass2,(const char *)ulirgStoreLetter + 7,7);
								stConfig_Value.rgucPass2[7] = '\0';
								strcat((char *)rgucPass,(const char *)stConfig_Value.rgucPass2);
									DEBUG_PRINTF("%s \r\n",rgucPass);																
								fnOPConfig_Send(CONFIG_WIFI_PASS,0);
				}
				memset(ulirgStoreLetter,'\0',sizeof(ulirgStoreLetter));
				uiLetterStartPos = 340;
				uiLetter = 0;
				ucLetterPos = 0;
			}
					
			switch(ucCurrentSwitch)
			{ 	
				case SWITCH12:
				{
					ucCurrentSwitch = 0;
					fnGr_FillRectangle((uiCursorXPos - 25), uiCursorYPos, 26,30,WHITE_CLR);
					
					switch(uiCurrentScreen)
					{
						case SCREEN_WIFI_IP_DETAILS:
						{
							ucWIFIIPSetVariableCounter++;
							uiCursorXPos += 34;
							if ( 270 == uiCursorXPos )
							{
								ucWIFIIPSetVariableCounter--;
								uiCursorXPos -= 34;	
							}
						}break;
					}
				}break;
					
				
				case SWITCH11:
				{
					ucCurrentSwitch = 0;
					fnGr_FillRectangle((uiCursorXPos - 25), uiCursorYPos, 26,30,WHITE_CLR);
					
					switch(uiCurrentScreen)
					{
						case SCREEN_WIFI_IP_DETAILS:
						{
							ucWIFIIPSetVariableCounter--;
							uiCursorXPos -= 34;
							if ( 32 == uiCursorXPos )
							{
								uiCursorXPos += 34;
								ucWIFIIPSetVariableCounter++;
							}
						}
					}	
					
				}break;
				
			}
		}break;
		default:
			break;
	}
}
/*****************************************************************************
**@Function		 	:  fnFlushWarning_Check()
**@Descriptions	:	 Flush warning check after prime and spray
**@parameters		:  none
**@return				:  none
*****************************************************************************/
void fnFlushWarning_Check(void)
{
	if((stConfig_Value.fConfigFlushWarnTime * 1000.0) < uliFlushWarningCnt)
	{
		StatusFlag.stStatusFlag.bFlushWarnCntAchieved = TRUE;
		GPIO_WriteBit(GPIOE, GPIO_Pin_4, Bit_SET);
	}
}

/*****************************************************************************
**@Function		 	:  fnConvertConfigToMatric()
**@Descriptions	:	 Convert all configuration parameters to Metric
**@parameters		:  none
**@return				:  none
*****************************************************************************/
void fnConvertConfigToMatric(void)
{
	stConfig_Value.fConfigSprayPressure = fnImperialToMetric_Conv( stConfig_Value.fConfigSprayPressure, PRESSURE_VALUE); 
	stConfig_Value.fConfigSprayPressureLTH = fnImperialToMetric_Conv( stConfig_Value.fConfigSprayPressureLTH, PRESSURE_VALUE);
	stConfig_Value.fConfigSprayPressureHTH = fnImperialToMetric_Conv( stConfig_Value.fConfigSprayPressureHTH, PRESSURE_VALUE);
	
	stConfig_Value.fConfigRecirPressure = fnImperialToMetric_Conv( stConfig_Value.fConfigRecirPressure, PRESSURE_VALUE);	
	stConfig_Value.fConfigRecirPressureLTH = fnImperialToMetric_Conv( stConfig_Value.fConfigRecirPressureLTH, PRESSURE_VALUE);
	stConfig_Value.fConfigRecirPressureHTH = fnImperialToMetric_Conv( stConfig_Value.fConfigRecirPressureHTH, PRESSURE_VALUE);

	stConfig_Value.fConfigPressureGauge1LTH = fnImperialToMetric_Conv( stConfig_Value.fConfigPressureGauge1LTH, PRESSURE_VALUE);
	stConfig_Value.fConfigPressureGauge1HTH = fnImperialToMetric_Conv( stConfig_Value.fConfigPressureGauge1HTH, PRESSURE_VALUE);
	stConfig_Value.fConfigPressureGauge2LTH = fnImperialToMetric_Conv( stConfig_Value.fConfigPressureGauge2LTH, PRESSURE_VALUE);
	stConfig_Value.fConfigPressureGauge2HTH = fnImperialToMetric_Conv( stConfig_Value.fConfigPressureGauge2HTH, PRESSURE_VALUE);
	stConfig_Value.fConfigPressureGauge3LTH = fnImperialToMetric_Conv( stConfig_Value.fConfigPressureGauge3LTH, PRESSURE_VALUE);
	stConfig_Value.fConfigPressureGauge3HTH = fnImperialToMetric_Conv( stConfig_Value.fConfigPressureGauge3HTH, PRESSURE_VALUE);


	
	stConfig_Value.fConfigAntiFrzTempSet = fnImperialToMetric_Conv( stConfig_Value.fConfigAntiFrzTempSet, TEMPERATURE_VALUE);
	stConfig_Value.fConfigAntiFrzTempLTH = fnImperialToMetric_Conv( stConfig_Value.fConfigAntiFrzTempLTH, TEMPERATURE_VALUE);
	stConfig_Value.fConfigAntiFrzTempHTH = fnImperialToMetric_Conv( stConfig_Value.fConfigAntiFrzTempHTH, TEMPERATURE_VALUE);

	stConfig_Value.fConfigInlineBaseTempSet = fnImperialToMetric_Conv( stConfig_Value.fConfigInlineBaseTempSet, TEMPERATURE_VALUE);
	stConfig_Value.fConfigInlineBaseTempLTH = fnImperialToMetric_Conv( stConfig_Value.fConfigInlineBaseTempLTH, TEMPERATURE_VALUE);
	stConfig_Value.fConfigInlineBaseTempHTH = fnImperialToMetric_Conv( stConfig_Value.fConfigInlineBaseTempHTH, TEMPERATURE_VALUE);


	stConfig_Value.fConfigInlineHardTempSet = fnImperialToMetric_Conv( stConfig_Value.fConfigInlineHardTempSet, TEMPERATURE_VALUE);
	stConfig_Value.fConfigInlineHardTempLTH = fnImperialToMetric_Conv( stConfig_Value.fConfigInlineHardTempLTH, TEMPERATURE_VALUE);
	stConfig_Value.fConfigInlineHardTempHTH = fnImperialToMetric_Conv( stConfig_Value.fConfigInlineHardTempHTH, TEMPERATURE_VALUE);
	
	
	stConfig_Value.fConfigPipeTempLTH = fnImperialToMetric_Conv( stConfig_Value.fConfigPipeTempLTH, TEMPERATURE_VALUE);
	stConfig_Value.fConfigPipeTempHTH = fnImperialToMetric_Conv( stConfig_Value.fConfigPipeTempHTH, TEMPERATURE_VALUE);

/*change21*/
	stConfig_Value.fConfigAreaofCylinderA = fnImperialToMetric_Conv( stConfig_Value.fConfigAreaofCylinderA, INCH2_TO_MM2);
	stConfig_Value.fConfigAreaofCylinderB = fnImperialToMetric_Conv( stConfig_Value.fConfigAreaofCylinderB, INCH2_TO_MM2);
	stConfig_Value.fConfigAreaofCylinderC = fnImperialToMetric_Conv( stConfig_Value.fConfigAreaofCylinderC, INCH2_TO_MM2);

	stConfig_Value.fConfigBaseLiquidLevelLTH = fnImperialToMetric_Conv( stConfig_Value.fConfigBaseLiquidLevelLTH, INCH_TO_MM);
	stConfig_Value.fConfigBaseLiquidLevelHTH = fnImperialToMetric_Conv( stConfig_Value.fConfigBaseLiquidLevelHTH, INCH_TO_MM);
	stConfig_Value.fConfigHardLiquidLevelLTH = fnImperialToMetric_Conv( stConfig_Value.fConfigHardLiquidLevelLTH, INCH_TO_MM);
	stConfig_Value.fConfigHardLiquidLevelHTH = fnImperialToMetric_Conv( stConfig_Value.fConfigHardLiquidLevelHTH , INCH_TO_MM);
	stConfig_Value.fConfigBaseTankHeight = fnImperialToMetric_Conv( stConfig_Value.fConfigBaseTankHeight, INCH_TO_MM);
	stConfig_Value.fConfigHardTankHeight = fnImperialToMetric_Conv( stConfig_Value.fConfigHardTankHeight, INCH_TO_MM);

	stConfig_Value.fUpstrokeLimit = fnImperialToMetric_Conv( stConfig_Value.fUpstrokeLimit, INCH_TO_MM);
	stConfig_Value.fUpstrokeMin = fnImperialToMetric_Conv( stConfig_Value.fUpstrokeMin, INCH_TO_MM);
	stConfig_Value.fUpstrokeMax = fnImperialToMetric_Conv( stConfig_Value.fUpstrokeMax, INCH_TO_MM);
	stConfig_Value.fDownstrokeLimit = fnImperialToMetric_Conv( stConfig_Value.fDownstrokeLimit, INCH_TO_MM);
	stConfig_Value.fDownstrokeMin = fnImperialToMetric_Conv( stConfig_Value.fDownstrokeMin, INCH_TO_MM);
	stConfig_Value.fDownstrokeMax = fnImperialToMetric_Conv( stConfig_Value.fDownstrokeMax, INCH_TO_MM);
	
	stConfig_Value.fPipeDiameter = fnImperialToMetric_Conv( stConfig_Value.fPipeDiameter, INCH_TO_MM);
	
	stConfig_Value.fConfigInlineHysHardPos = fnImperialToMetric_Conv( stConfig_Value.fConfigInlineHysHardPos, TEMPERATURE_VALUE);
	stConfig_Value.fConfigInlineHysHardNeg = fnImperialToMetric_Conv( stConfig_Value.fConfigInlineHysHardNeg, TEMPERATURE_VALUE);
	stConfig_Value.fConfigHysInlineBasePos = fnImperialToMetric_Conv( stConfig_Value.fConfigHysInlineBasePos, TEMPERATURE_VALUE);
	stConfig_Value.fConfigHysInlineBaseNeg = fnImperialToMetric_Conv( stConfig_Value.fConfigHysInlineBaseNeg, TEMPERATURE_VALUE);
	stConfig_Value.fConfigHysAntiFrzPos = fnImperialToMetric_Conv( stConfig_Value.fConfigHysAntiFrzPos, TEMPERATURE_VALUE);
	stConfig_Value.fConfigHysAntiFrzNeg = fnImperialToMetric_Conv( stConfig_Value.fConfigHysAntiFrzNeg, TEMPERATURE_VALUE);
	
	stConfig_Value.fConfigHysRecircPresPos = fnImperialToMetric_Conv( stConfig_Value.fConfigHysRecircPresPos, PRESSURE_VALUE);
	stConfig_Value.fConfigHysRecircPresNeg = fnImperialToMetric_Conv( stConfig_Value.fConfigHysRecircPresNeg, PRESSURE_VALUE);
	stConfig_Value.fConfigHysSprayPresPos = fnImperialToMetric_Conv( stConfig_Value.fConfigHysSprayPresPos, PRESSURE_VALUE);
	stConfig_Value.fConfigHysSprayPresNeg = fnImperialToMetric_Conv( stConfig_Value.fConfigHysSprayPresNeg, PRESSURE_VALUE);


	/*change6*/
		stCurrent_Value.fHardMatTotalJoint = (float)(CONVERT_OZ_TO_ML(stCurrent_Value.fHardMatTotalJoint));
	
		
		stCurrent_Value.fBaseMatTotalJoint = (float)(CONVERT_OZ_TO_ML(stCurrent_Value.fBaseMatTotalJoint ));
	
		stCurrent_Value.fHardMatPrimeAcc = (float)(CONVERT_OZ_TO_ML(stCurrent_Value.fHardMatPrimeAcc ));
		stCurrent_Value.fHardMatPrimeAcc = (float)(CONVERT_ML_TO_L(stCurrent_Value.fHardMatPrimeAcc ));
		
		stCurrent_Value.fBaseMatPrimeAcc = (float)(CONVERT_OZ_TO_ML(stCurrent_Value.fBaseMatPrimeAcc ));
		stCurrent_Value.fBaseMatPrimeAcc = (float)(CONVERT_ML_TO_L(stCurrent_Value.fBaseMatPrimeAcc ));
		
		stCurrent_Value.fHardMatSprayAcc = (float)(CONVERT_OZ_TO_ML(stCurrent_Value.fHardMatSprayAcc ));
		stCurrent_Value.fHardMatSprayAcc = (float)(CONVERT_ML_TO_L(stCurrent_Value.fHardMatSprayAcc ));
		
		stCurrent_Value.fBaseMatSprayAcc = (float)(CONVERT_OZ_TO_ML(stCurrent_Value.fBaseMatSprayAcc ));
		stCurrent_Value.fBaseMatSprayAcc = (float)(CONVERT_ML_TO_L(stCurrent_Value.fBaseMatSprayAcc ));
		
		stCurrent_Value.fHardMatTotalAcc = (float)(CONVERT_OZ_TO_ML(stCurrent_Value.fHardMatTotalAcc ));
		stCurrent_Value.fHardMatTotalAcc = (float)(CONVERT_ML_TO_L(stCurrent_Value.fHardMatTotalAcc ));
		
		stCurrent_Value.fBaseMatTotalAcc = (float)(CONVERT_OZ_TO_ML(stCurrent_Value.fBaseMatTotalAcc ));								
		stCurrent_Value.fBaseMatTotalAcc = (float)(CONVERT_ML_TO_L(stCurrent_Value.fBaseMatTotalAcc ));	



}

/*****************************************************************************
**@Function		 	:  fnConvertConfigToMatric()
**@Descriptions	:	 Convert all configuration parameters to Imperial
**@parameters		:  none
**@return				:  none
*****************************************************************************/
void fnConvertConfigToImperial(void)
{
	stConfig_Value.fConfigSprayPressure = fnMetricToImperial_Conv( stConfig_Value.fConfigSprayPressure, PRESSURE_VALUE); 
	stConfig_Value.fConfigSprayPressureLTH = fnMetricToImperial_Conv( stConfig_Value.fConfigSprayPressureLTH, PRESSURE_VALUE);
	stConfig_Value.fConfigSprayPressureHTH = fnMetricToImperial_Conv( stConfig_Value.fConfigSprayPressureHTH, PRESSURE_VALUE);
	
	stConfig_Value.fConfigRecirPressure = fnMetricToImperial_Conv( stConfig_Value.fConfigRecirPressure, PRESSURE_VALUE);	
	stConfig_Value.fConfigRecirPressureLTH = fnMetricToImperial_Conv( stConfig_Value.fConfigRecirPressureLTH, PRESSURE_VALUE);
	stConfig_Value.fConfigRecirPressureHTH = fnMetricToImperial_Conv( stConfig_Value.fConfigRecirPressureHTH, PRESSURE_VALUE);

	stConfig_Value.fConfigPressureGauge1LTH = fnMetricToImperial_Conv( stConfig_Value.fConfigPressureGauge1LTH, PRESSURE_VALUE);
	stConfig_Value.fConfigPressureGauge1HTH = fnMetricToImperial_Conv( stConfig_Value.fConfigPressureGauge1HTH, PRESSURE_VALUE);
	stConfig_Value.fConfigPressureGauge2LTH = fnMetricToImperial_Conv( stConfig_Value.fConfigPressureGauge2LTH, PRESSURE_VALUE);
	stConfig_Value.fConfigPressureGauge2HTH = fnMetricToImperial_Conv( stConfig_Value.fConfigPressureGauge2HTH, PRESSURE_VALUE);
	stConfig_Value.fConfigPressureGauge3LTH = fnMetricToImperial_Conv( stConfig_Value.fConfigPressureGauge3LTH, PRESSURE_VALUE);
	stConfig_Value.fConfigPressureGauge3HTH = fnMetricToImperial_Conv( stConfig_Value.fConfigPressureGauge3HTH, PRESSURE_VALUE);

	stConfig_Value.fConfigAntiFrzTempSet = fnMetricToImperial_Conv( stConfig_Value.fConfigAntiFrzTempSet, TEMPERATURE_VALUE);
	stConfig_Value.fConfigAntiFrzTempLTH = fnMetricToImperial_Conv( stConfig_Value.fConfigAntiFrzTempLTH, TEMPERATURE_VALUE);
	stConfig_Value.fConfigAntiFrzTempHTH = fnMetricToImperial_Conv( stConfig_Value.fConfigAntiFrzTempHTH, TEMPERATURE_VALUE);

	stConfig_Value.fConfigInlineBaseTempSet = fnMetricToImperial_Conv( stConfig_Value.fConfigInlineBaseTempSet, TEMPERATURE_VALUE);
	stConfig_Value.fConfigInlineBaseTempLTH = fnMetricToImperial_Conv( stConfig_Value.fConfigInlineBaseTempLTH, TEMPERATURE_VALUE);
	stConfig_Value.fConfigInlineBaseTempHTH = fnMetricToImperial_Conv( stConfig_Value.fConfigInlineBaseTempHTH, TEMPERATURE_VALUE);


	stConfig_Value.fConfigInlineHardTempSet = fnMetricToImperial_Conv( stConfig_Value.fConfigInlineHardTempSet, TEMPERATURE_VALUE);
	stConfig_Value.fConfigInlineHardTempLTH = fnMetricToImperial_Conv( stConfig_Value.fConfigInlineHardTempLTH, TEMPERATURE_VALUE);
	stConfig_Value.fConfigInlineHardTempHTH = fnMetricToImperial_Conv( stConfig_Value.fConfigInlineHardTempHTH, TEMPERATURE_VALUE);
	
	
	stConfig_Value.fConfigPipeTempLTH = fnMetricToImperial_Conv( stConfig_Value.fConfigPipeTempLTH, TEMPERATURE_VALUE);
	stConfig_Value.fConfigPipeTempHTH = fnMetricToImperial_Conv( stConfig_Value.fConfigPipeTempHTH, TEMPERATURE_VALUE);


	stConfig_Value.fConfigAreaofCylinderA = fnMetricToImperial_Conv( stConfig_Value.fConfigAreaofCylinderA, MM2_TO_INCH2);
	stConfig_Value.fConfigAreaofCylinderB = fnMetricToImperial_Conv( stConfig_Value.fConfigAreaofCylinderB, MM2_TO_INCH2);
	stConfig_Value.fConfigAreaofCylinderC = fnMetricToImperial_Conv( stConfig_Value.fConfigAreaofCylinderC, MM2_TO_INCH2);

	stConfig_Value.fConfigBaseLiquidLevelLTH = fnMetricToImperial_Conv( stConfig_Value.fConfigBaseLiquidLevelLTH, MM_TO_INCH);
	stConfig_Value.fConfigBaseLiquidLevelHTH = fnMetricToImperial_Conv( stConfig_Value.fConfigBaseLiquidLevelHTH, MM_TO_INCH);
	stConfig_Value.fConfigHardLiquidLevelLTH = fnMetricToImperial_Conv( stConfig_Value.fConfigHardLiquidLevelLTH, MM_TO_INCH);
	stConfig_Value.fConfigHardLiquidLevelHTH = fnMetricToImperial_Conv( stConfig_Value.fConfigHardLiquidLevelHTH , MM_TO_INCH);
	stConfig_Value.fConfigBaseTankHeight = fnMetricToImperial_Conv( stConfig_Value.fConfigBaseTankHeight, MM_TO_INCH);
	stConfig_Value.fConfigHardTankHeight = fnMetricToImperial_Conv( stConfig_Value.fConfigHardTankHeight, MM_TO_INCH);


	stConfig_Value.fUpstrokeLimit = fnMetricToImperial_Conv( stConfig_Value.fUpstrokeLimit, MM_TO_INCH);
	stConfig_Value.fUpstrokeMin = fnMetricToImperial_Conv( stConfig_Value.fUpstrokeMin, MM_TO_INCH);
	stConfig_Value.fUpstrokeMax = fnMetricToImperial_Conv( stConfig_Value.fUpstrokeMax, MM_TO_INCH);
	stConfig_Value.fDownstrokeLimit = fnMetricToImperial_Conv( stConfig_Value.fDownstrokeLimit, MM_TO_INCH);
	stConfig_Value.fDownstrokeMin = fnMetricToImperial_Conv( stConfig_Value.fDownstrokeMin, MM_TO_INCH);
	stConfig_Value.fDownstrokeMax = fnMetricToImperial_Conv( stConfig_Value.fDownstrokeMax, MM_TO_INCH);

	stConfig_Value.fPipeDiameter = fnMetricToImperial_Conv( stConfig_Value.fPipeDiameter, MM_TO_INCH);

	stConfig_Value.fConfigInlineHysHardPos = fnMetricToImperial_Conv( stConfig_Value.fConfigInlineHysHardPos, TEMPERATURE_VALUE);
	stConfig_Value.fConfigInlineHysHardNeg = fnMetricToImperial_Conv( stConfig_Value.fConfigInlineHysHardNeg, TEMPERATURE_VALUE);
	stConfig_Value.fConfigHysInlineBasePos = fnMetricToImperial_Conv( stConfig_Value.fConfigHysInlineBasePos, TEMPERATURE_VALUE);
	stConfig_Value.fConfigHysInlineBaseNeg = fnMetricToImperial_Conv( stConfig_Value.fConfigHysInlineBaseNeg, TEMPERATURE_VALUE);
	stConfig_Value.fConfigHysAntiFrzPos = fnMetricToImperial_Conv( stConfig_Value.fConfigHysAntiFrzPos, TEMPERATURE_VALUE);
	stConfig_Value.fConfigHysAntiFrzNeg = fnMetricToImperial_Conv( stConfig_Value.fConfigHysAntiFrzNeg, TEMPERATURE_VALUE);
	
	stConfig_Value.fConfigHysRecircPresPos = fnMetricToImperial_Conv( stConfig_Value.fConfigHysRecircPresPos, PRESSURE_VALUE);
	stConfig_Value.fConfigHysRecircPresNeg = fnMetricToImperial_Conv( stConfig_Value.fConfigHysRecircPresNeg, PRESSURE_VALUE);
	stConfig_Value.fConfigHysSprayPresPos = fnMetricToImperial_Conv( stConfig_Value.fConfigHysSprayPresPos, PRESSURE_VALUE);
	stConfig_Value.fConfigHysSprayPresNeg = fnMetricToImperial_Conv( stConfig_Value.fConfigHysSprayPresNeg, PRESSURE_VALUE);

/*change6*/
		stCurrent_Value.fHardMatTotalJoint = fnMetricToImperial_Conv(stCurrent_Value.fHardMatTotalJoint, ML_TO_OZ);
		
		stCurrent_Value.fBaseMatTotalJoint = fnMetricToImperial_Conv(stCurrent_Value.fBaseMatTotalJoint, ML_TO_OZ);
		
		
		stCurrent_Value.fHardMatPrimeAcc = (float)(CONVERT_L_TO_ML(stCurrent_Value.fHardMatPrimeAcc ));
		stCurrent_Value.fHardMatPrimeAcc = fnMetricToImperial_Conv(stCurrent_Value.fHardMatPrimeAcc, ML_TO_OZ);
		
		stCurrent_Value.fBaseMatPrimeAcc = (float)(CONVERT_L_TO_ML(stCurrent_Value.fBaseMatPrimeAcc ));
		stCurrent_Value.fBaseMatPrimeAcc = fnMetricToImperial_Conv(stCurrent_Value.fBaseMatPrimeAcc, ML_TO_OZ);
		
		stCurrent_Value.fHardMatSprayAcc = (float)(CONVERT_L_TO_ML(stCurrent_Value.fHardMatSprayAcc ));
		stCurrent_Value.fHardMatSprayAcc = fnMetricToImperial_Conv(stCurrent_Value.fHardMatSprayAcc, ML_TO_OZ);
		
		stCurrent_Value.fBaseMatSprayAcc = (float)(CONVERT_L_TO_ML(stCurrent_Value.fBaseMatSprayAcc ));
		stCurrent_Value.fBaseMatSprayAcc = fnMetricToImperial_Conv(stCurrent_Value.fBaseMatSprayAcc, ML_TO_OZ);
		
		stCurrent_Value.fHardMatTotalAcc = (float)(CONVERT_L_TO_ML(stCurrent_Value.fHardMatTotalAcc ));
		stCurrent_Value.fHardMatTotalAcc = fnMetricToImperial_Conv(stCurrent_Value.fHardMatTotalAcc, ML_TO_OZ);
		
		stCurrent_Value.fBaseMatTotalAcc = (float)(CONVERT_L_TO_ML(stCurrent_Value.fBaseMatTotalAcc ));	
		stCurrent_Value.fBaseMatTotalAcc = fnMetricToImperial_Conv(stCurrent_Value.fBaseMatTotalAcc, ML_TO_OZ);


}


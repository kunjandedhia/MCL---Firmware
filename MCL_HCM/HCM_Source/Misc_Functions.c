/******************************************************************************
**@file    MiscFunctions.c
* 
**@brief   This File contain Misc Funtions Definition
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

uint8_t ucMode 									= 0;	/*Variable for Mode switching*/
uint16_t uiBypass 							= 0;	/*Variable for Bypass peripherals*/
StatusFlags_t StatusFlag;				/*Status Flag Union Variables*/
SplitByte_t SplitByte; 					/*union to split data as float and char*/
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
 **@Function 			: fnDefault_Setting
 **@Descriptions	: Reset the System flag and LED Status to RESET Values
 **@Parameters		: None
 **@Return				: None
 *****************************************************************************/
void fnDefault_Setting(void)
{	
	uint16_t uiTDataRd = 0, uiTDataWr = DEFAULT_SETTING;
	uint8_t ucLoop = 0;
	ucMode = DEFAULT_MODE;
	/*RESET All the Status Flags*/
	StatusFlag.uliStatusFlagReset = 0;
	//StatusFlag.uliStatusFlagReset[1] = 0;
	
	/* Enable the external CAN tranceiver pins for CAN communication*/
	GPIO_WriteBit(CAN_ENABLE_GPIO_PORT,CAN_ENABLE_GPIO_PIN,Bit_SET);
	GPIO_WriteBit(CAN_STROBE_GPIO_PORT,CAN_STROBE_GPIO_PIN,Bit_SET);
	/*Setting Thermocouple board at the Default Condition and Set TS = 1 
		So that it Can read the Data of Cold Junction temp */
	for(ucLoop = 0; ucLoop < NO_OF_TC_SENSOR + NO_OF_SPARE_TC_SENSOR; ucLoop++)
	{
		ADS1118_SELECT(ucLoop); /*CS asserts low*/
		
		uiTDataWr |= TS_MODE;  /*Setting  TS = 1*/
		uiTDataRd = fnSPI_ReadWrite(TC_SPI, uiTDataWr);
		uiTDataRd = fnSPI_ReadWrite(TC_SPI, uiTDataWr);
			
		ADS1118_UNSELECT(ucLoop);  /*CS High*/
		if(uiTDataWr == uiTDataRd)
		{
			#ifdef GENERAL_DBG
				DEBUG_PRINTF("Includes Reg[%d] : 0x%04x\r\n", ucLoop, uiTDataRd);
			#endif
		}
	}
	
	stLL[BASE].ucADCChannel 		= LL_NOT_CONNECTED;
	stLL[HARDNER].ucADCChannel 	= LL_NOT_CONNECTED;
}
/*******************************************************************************
 **@Function 			: fnSelfDiagnostic_Test
 **@Descriptions	: this funtion will do self diagostic test and sends the 
										Diagnostics Status to SCU and HMI
 **@Parameters		: None
 **@Return				: None
 ******************************************************************************/
void fnSelfDiagnostic_Test(void)
{
	float fReadValue = 0.0f;
	uint8_t ucLoop = 0;
	CanTxMsg stTxMsgDiag;
	
	memset(stTxMsgDiag.Data, 0, sizeof(stTxMsgDiag.Data));
	stTxMsgDiag.DLC = 3;
	stTxMsgDiag.ExtId = MSG_TYPE_STATUS | RESERVED | STATUS_HCM_DIAGNOSTIC |
											SOURCE_HCM ;	
	
  /********** Heater SSR Diagnostics Tets (for all HCB Module) **********/	
	for(ucLoop = 0; ucLoop < NO_OF_SSR; ucLoop++)
	{	
		fnHeater_ONOFF(ucLoop, HEATER_ON);
	}
	fnDelay_ms(1);
	for(ucLoop = 0; ucLoop < NO_OF_SSR; ucLoop++)
	{
		if(HARDNER_HEATER != ucLoop)
		{
			if(GPIO_ReadInputDataBit(HEATER_SSR_FEEDBAK_PORT,
			   HEATER_FEEDBACK_PIN(stTC.stProcess[ucLoop].ucSSRPosition)))	
			{
				#ifdef DIAG_DBG
					DEBUG_PRINTF("SSR Diagnostics Pass  POSITION = %d\r\n",
												stTC.stProcess[ucLoop].ucSSRPosition);
				#endif
				uiIOExpander1 &= ~SSR_LED_VALUE(ucLoop, RED);
				uiIOExpander1 |= SSR_LED_VALUE(ucLoop, GREEN);
			}
			else
			{
				/* if Bypass then don't send the Diagnostic fault to SCU AND HMI*/
				if(RESET == ((uiBypass >> ucLoop) & 0x01))
				{
					stTxMsgDiag.Data[1] |= HEATER_SSR_DIGNOSTICS_FAIL(ucLoop); 
				}
				
				uiIOExpander1 |= SSR_LED_VALUE(ucLoop, RED);
				uiIOExpander1 &= ~SSR_LED_VALUE(ucLoop, GREEN);
				/* Heater SSR status-LED OFF - SSR Faulty*/
				#ifdef DIAG_DBG
					DEBUG_PRINTF("SSR Diagnostics Fail POSITION = %d\r\n",
												stTC.stProcess[ucLoop].ucSSRPosition);
				#endif
			}
		}
		else if(HARDNER_HEATER == ucLoop)
		{
			if(GPIO_ReadInputDataBit(HEATER_SSR_FEEDBAK_PORT,
			   HEATER_FEEDBACK_PIN(ucHardSSRPosition)))	
			{
				#ifdef DIAG_DBG
					DEBUG_PRINTF("Hardner SSR Diagnostics Pass POSITION = %d\r\n",ucHardSSRPosition);
				#endif
				uiIOExpander1 &= ~SSR_LED_VALUE(ucLoop, RED);
				uiIOExpander1 |= SSR_LED_VALUE(ucLoop, GREEN);
			}
			else
			{
				/* if Bypass then don't send the Diagnostic fautl to SCU AND HMI*/
				if(RESET == ((uiBypass >> ucLoop) & 0x01))
				{
					stTxMsgDiag.Data[1] |= HEATER_SSR_DIGNOSTICS_FAIL(ucLoop); 
				}
				
				uiIOExpander1 |= SSR_LED_VALUE(ucLoop, RED);
				uiIOExpander1 &= ~SSR_LED_VALUE(ucLoop, GREEN);
				/* Heater SSR status-LED OFF - SSR Faulty*/
				#ifdef DIAG_DBG
					DEBUG_PRINTF("Hardner SSR Diagnostics Fail POSITION = %d\r\n", ucHardSSRPosition);
				#endif
			}
		}		
	}
	for(ucLoop = 0; ucLoop < NO_OF_SSR; ucLoop++)
	{
		fnHeater_ONOFF(ucLoop, HEATER_OFF);
	}			
	/*********** Thermocouple Diagnostics Test **********/
	for(ucLoop = 0; ucLoop < NO_OF_TC_SENSOR; ucLoop++)
	{
		if(INVALID_FLOAT_VALUE == fabs(stTC.stRead[ucLoop].fCurrentTemp)) 
		{
			if(RESET == ((uiBypass >> (ucLoop + 4)) & 0x01))
			{
				/*SET Diagnostic status Bits*/
				stTxMsgDiag.Data[1] |= THERMOCOUPLE_DIGNOSTICS_FAIL(ucLoop); 
			}		
			if(RETURN_LINE != ucLoop)
			{
				uiIOExpander2 |= TC_LED_VALUE(ucLoop, RED);
				uiIOExpander2 &= ~TC_LED_VALUE(ucLoop, BLUE);
				uiIOExpander2 &= ~TC_LED_VALUE(ucLoop, GREEN);
			}
			else
			{
				uiIOExpander1 |= TC_LED_VALUE(ucLoop, RED);
				uiIOExpander1 &= ~TC_LED_VALUE(ucLoop, BLUE);
				uiIOExpander1 &= ~TC_LED_VALUE(ucLoop, GREEN);
			}
				
			#ifdef DIAG_DBG
				DEBUG_PRINTF("Thermocouple Diagnostics Fail %d\r\n", ucLoop);
			#endif
		}
		else
		{
			#ifdef DIAG_DBG
				DEBUG_PRINTF("Thermocouple Diagnostics Pass %d\r\n", ucLoop);
			#endif
			
			if(RETURN_LINE != ucLoop)
			{
				uiIOExpander2 &= ~TC_LED_VALUE(ucLoop, RED);
			uiIOExpander2 &= ~TC_LED_VALUE(ucLoop, BLUE);
			uiIOExpander2 |= TC_LED_VALUE(ucLoop, GREEN);
			}
			else
			{
				uiIOExpander1 &= ~TC_LED_VALUE(ucLoop, RED);
				uiIOExpander1 &= ~TC_LED_VALUE(ucLoop, BLUE);
				uiIOExpander1 |= TC_LED_VALUE(ucLoop, GREEN);
			}
			
		}
	}
	/********************** LL Diagnostics ****************************/
	/*Reading liquid level in voltage*/
	for(ucLoop = 0; ucLoop < NO_OF_LL_SENSOR; ucLoop++)
	{ 
		if(LL_NOT_CONNECTED != stLL[ucLoop].ucADCChannel)
		{
			fReadValue = fnADC_Read(stLL[ucLoop].ucADCChannel);
		}
		else
		{
			fReadValue = INVALID_FLOAT_VALUE;
		}
		/*Validation*/
		if((LOW_LIQUID_LEVEL_V <= fabs(fReadValue)) && 
			 (HIGH_LIQUID_LEVEL_V >= fabs(fReadValue))) 
		{
			#ifdef DIAG_DBG
				DEBUG_PRINTF("LL Diagnostics Pass\r\n");
			#endif
			uiIOExpander2 &= ~LL_LED_VALUE(ucLoop, RED);
			uiIOExpander2 &= ~LL_LED_VALUE(ucLoop, BLUE);
			uiIOExpander2 |= LL_LED_VALUE(ucLoop, GREEN);
		}
		else
		{
			if(RESET == ((uiBypass >> (ucLoop + 8)) & 0x01))
			{
				stTxMsgDiag.Data[2] |= LIQUID_LEVEL_DIGNOSTICS_FAIL(ucLoop); 
			}
			uiIOExpander2 |= LL_LED_VALUE(ucLoop, RED);
			uiIOExpander2 &= ~LL_LED_VALUE(ucLoop, BLUE);
			uiIOExpander2 &= ~LL_LED_VALUE(ucLoop, GREEN);
			stLL[ucLoop].bLLSensorError = SET;
			#ifdef DIAG_DBG
				DEBUG_PRINTF("LL Diagnostics Fail\r\n");
			#endif
		}
	}		 
	
	
	/********************************Diagnostics Over***************/
	/********************@Diagnostics @Pass* ******************/
	if((0x00 == stTxMsgDiag.Data[1]) && (0x00 == stTxMsgDiag.Data[2]))
	{
		StatusFlag.stStatusFlag.bDiagnosticsPass = SET;
		#ifdef DIAG_DBG
			DEBUG_PRINTF("Diagnostics Pass\r\n");
		#endif
		stTxMsgDiag.Data[0] = DIAGNOSTICS_PASS;/*diagnostics fail status */
	}
	else /************************@diagnostics @Fail****************/
	{
		StatusFlag.stStatusFlag.bDiagnosticsPass = RESET;
		#ifdef DIAG_DBG
			DEBUG_PRINTF("Diagnostics Fail\r\n");
		#endif
		stTxMsgDiag.Data[0] = DIAGNOSTICS_FAIL ;
	}
	fnCANMessage_Send(&stTxMsgDiag);
	
	/*11thMarch-Sending code version after diagnostic completed*/
	memset(stTxMsgDiag.Data, 0, sizeof(stTxMsgDiag.Data));
	stTxMsgDiag.DLC = 5;
	stTxMsgDiag.ExtId = MSG_TYPE_STATUS | RESERVED | STATUS_SOFTWARE_VERSION |
											SOURCE_HCM ;
	
	stTxMsgDiag.Data[0] = MODULE_CODE;
	SplitByte.fData = CODE_VERSION;
	for(ucLoop = 1; ucLoop < stTxMsgDiag.DLC; ucLoop++)
	{
		stTxMsgDiag.Data[ucLoop] = SplitByte.rgucData[ucLoop - 1];
	}
	fnCANMessage_Send(&stTxMsgDiag);
}
/********************************************************************************
 **@Function		 	:  fnLED_Indication
 **@Descriptions	:  Update LED status if Any Changes occure on the IO_Expanders
										 Port variables
 **@Parameters		:  None
 **@Return				:  None
 ******************************************************************************/
void fnLED_Indication(void)
{	
	if(RESET == StatusFlag.stStatusFlag.bFirstHrtBtMsgFlag)
	{			
		if(ucPostLedBlinkCnt < POST_LED_BLINK_COUNT)
		{
			/* ON Board status RED LED*/
			uiIOExpander1 =	IO1_ALL_RED_LED;
			uiIOExpander2 =	IO1_ALL_RED_LED;
		}	
		else if(ucPostLedBlinkCnt > POST_LED_BLINK_COUNT)
		{
			/* OFF Board status RED LED*/
			uiIOExpander1 =	IO2_ALL_BLUE_LED;
			uiIOExpander2 =	IO2_ALL_BLUE_LED;
			
			/* RESET The Blinking LED COUNTER if greatere than 200msec */
			if(ucPostLedBlinkCnt >= (2 * POST_LED_BLINK_COUNT))
			{
				ucPostLedBlinkCnt = 0;
			}
		}
	}	
	
	if((uiIOExpander1 != uiPreIOExpander1) && 
		 (RESET == StatusFlag.stStatusFlag.bI2CBusyFlag))
	{
		if(!I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))
		{
			uiI2CWriteData =  ~uiIOExpander1;
			
			StatusFlag.stStatusFlag.bI2CBusyFlag = SET;
			ucI2C1Seq = 0;
			/* Slave Address with Write instruction */
			ucIOExpSlaveAddr = IOEXP_1_SLAVE_ADDR & I2C_SLAVE_WRITE; 
			/* Send I2C1 START condition */
			I2C_GenerateSTART(I2C_IO_EXP, ENABLE);
			
			uiPreIOExpander1 = uiIOExpander1;
			#ifdef IOEXP_DBG
			DEBUG_PRINTF("IOEXP_1_SLAVE_ADDR\r\n");
			#endif
		}
	}
	if((uiIOExpander2 != uiPreIOExpander2) &&
  	 (RESET == StatusFlag.stStatusFlag.bI2CBusyFlag))
	{
		if(!I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))
		{
			uiI2CWriteData =  ~uiIOExpander2;
			StatusFlag.stStatusFlag.bI2CBusyFlag = SET;
			ucI2C1Seq = 0;
			/* Slave Address with Write instruction */
			ucIOExpSlaveAddr = IOEXP_2_SLAVE_ADDR & I2C_SLAVE_WRITE; 
			/* Send I2C1 START condition */
			I2C_GenerateSTART(I2C_IO_EXP, ENABLE);
			
			uiPreIOExpander2 = uiIOExpander2;
			#ifdef IOEXP_DBG
			DEBUG_PRINTF("IOEXP_2_SLAVE_ADDR\r\n");
			#endif
		}
	}
}
/********************************************************************************
 **@Function		 	:  fnClearAll_Warnings
 **@Descriptions	:  This funtion clear all the Warnning flags
 **@Parameters		:  None
 **@Return				:  None
 ******************************************************************************/
void fnClearAll_Warnings(void)
{
	uint8_t ucLoop;
	StatusFlag.stStatusFlag.bWarnningFlag 				= RESET;
	StatusFlag.stStatusFlag.bHardHeaterFaultFlag	= RESET;
	StatusFlag.stStatusFlag.bDiagnosticsPass 			= RESET;
	for(ucLoop = 0; ucLoop < NO_OF_TC_SENSOR; ucLoop++)
	{
		stTC.stProcess[ucLoop].bHeaterFaultFlag 		= RESET;
		stTC.stProcess[ucLoop].bTempLowTHFaultFlag 	= RESET;
		
		/*SSR Fault LED OFF */
		uiIOExpander1 &= ~SSR_LED_VALUE(ucLoop, RED); /*RED/Fault Led OFF*/
	}
	/*Clear LL Flags*/
	for(ucLoop = 0; ucLoop < NO_OF_LL_SENSOR; ucLoop++)
	{
		stLL[ucLoop].bLLLowTHFaultFlag 	= RESET;
		stLL[ucLoop].bLLSensorErrorSend = RESET;
		stLL[ucLoop].bLLSensorError 		= RESET;
	}	
}
/********************************************************************************
 **@Function		 	:  fnClearAll_Faults
 **@Descriptions	:  This function Claer all the faults flags
 **@Parameters		:  None
 **@Return				:  None
 ******************************************************************************/
void fnClearAll_Faults(void)
{	
	uint8_t ucLoop = 0;
	/*SSR Fault LED OFF*/
	for(ucLoop = 0; ucLoop < NO_OF_SSR; ucLoop++)
	{
		uiIOExpander1 &= ~SSR_LED_VALUE(ucLoop, RED);
		uiIOExpander1 &= ~SSR_LED_VALUE(ucLoop, GREEN);
		uiIOExpander1 &= ~SSR_LED_VALUE(ucLoop, BLUE);
		if(HARDNER_HEATER != ucLoop)
		{
			stTC.stProcess[ucLoop].bHeaterFaultFlag = RESET;
		}
		else
		{
			StatusFlag.stStatusFlag.bHardHeaterFaultFlag = RESET;
		}
	}
		
	StatusFlag.stStatusFlag.bCANFaultSCU 	= RESET;
	StatusFlag.stStatusFlag.bCANFaultHMI 	= RESET;
	StatusFlag.stStatusFlag.bEStopFlag 		= RESET;
	StatusFlag.stStatusFlag.bHBNotRcvFlag = RESET;

	
	for(ucLoop = 0; ucLoop < NO_OF_TC_SENSOR - 1; ucLoop++)
	{
		stTC.stProcess[ucLoop].bTempHighTHFaultFlag = RESET;
	}
	for(ucLoop = 0; ucLoop < NO_OF_LL_SENSOR; ucLoop++)
	{
		stLL[ucLoop].bLLHighTHFaultFlag  = RESET;
	}
	StatusFlag.stStatusFlag.bErrorFlag = RESET;
}
/********************************************************************************
 **@Function 		: fnDefault_Mode
 **@Description	: this function Contains Default Mode Fucntionality
									Default Mode: Enable Timer if First Heartbeat rcv
									Init HCB after Device Func CMD Rcv & Diagnostics Test
									After Diagnostics Pass Swtich the Mode To Normal Mode														
 **@Parameters	: None
 **@Return			: None
 ******************************************************************************/
void fnDefault_Mode(void)
{
	if(DEFAULT_MODE == ucMode)
	{
		#ifdef DEFAULT_MODE_DBG
			DEBUG_PRINTF("uliConfigParaByte = 0%02x\r\n",uliConfigParaByte);
		#endif
		
		/* It will check for first heartbeat only*/
		if((SET == StatusFlag.stStatusFlag.bHeartBeatMsgFlag) && 
			 (RESET == StatusFlag.stStatusFlag.bFirstHrtBtMsgFlag)) 
		{
			/* Setting the first heart beat flag */
			StatusFlag.stStatusFlag.bFirstHrtBtMsgFlag = SET;
			StatusFlag.stStatusFlag.bHeartBeatMsgFlag = RESET;
			
			uiIOExpander1 = 0;
			uiIOExpander2 = 0;
			ucPostLedBlinkCnt = 0;
			/* TIM1 enable counter */
			TIM_Cmd(TIM1, ENABLE);
			#ifdef DEFAULT_MODE_DBG
				DEBUG_PRINTF("First Heartbeat Received\r\n");
			#endif
		}
		/*Read thermocouple in interval of 50msec after first Hearbeat(Timer enable)*/
		if((TEMP_READ_TIME <= uiTempReadCnt) &&
			 (RESET == StatusFlag.stStatusFlag.bTCProcessed) &&
			 (CONFIG_PARA_BYTE_HCM == uliConfigParaByte))
		{
			uiTempReadCnt = 0;
			fnReadTemp();	
		}
		
		if((SET == StatusFlag.stStatusFlag.bFirstHrtBtMsgFlag) && 
			 (RESET == StatusFlag.stStatusFlag.bDiagnosticsFlag) &&
			 (CONFIG_PARA_BYTE_HCM == uliConfigParaByte))
		{
			/* If Self Diagnost Pass then send Pass status and
				 Includes CMD ,if fail then send Fail Status */
			if(SET == StatusFlag.stStatusFlag.bTCProcessed)
			{
				
				fnSelfDiagnostic_Test();
				StatusFlag.stStatusFlag.bDiagnosticsFlag = SET;
				StatusFlag.stStatusFlag.bTCProcessed = RESET;
			}
			#ifdef DEFAULT_MODE_DBG
				DEBUG_PRINTF("fnSelfDiagnostic_Test\r\n");
			#endif
		}
		/* after Diagnostic pass and Includes parameters received, 
			Switch from Default Mode to Normal Mode */
		if(SET == StatusFlag.stStatusFlag.bDiagnosticsPass)
		{		
			ucMode = OPERATION_MODE; 
			uiTempReadCount = 0;
			uiTimCountLL = 0;
			uiTempReadCnt = 0;
			StatusFlag.stStatusFlag.bTCProcessed = RESET;
			#ifdef DEFAULT_MODE_DBG
				DEBUG_PRINTF("Mode Switch to Normal Mode ulidevi\r\n");
			#endif
		}
	}
}
/********************************************************************************
 **@Function 		: fnOperation_Mode
 **@Description : This function Contains the Normal Mode functionality
									Operation Mode: Periodically Read Data and Monitor 
									Temperature(i.e heater), Liquid Level, Check For Heater Faults.
 **@Parameters	: None
 **@Return			: None
 ******************************************************************************/
void fnOperation_Mode(void)
{
	uint8_t ucLoop = 0;
//	uint8_t rgucTemp[150];
	if(OPERATION_MODE == ucMode)
	{	
		/*Read thermocouple in interval of 50msec*/
		if((TEMP_READ_TIME <= uiTempReadCnt) && 
			 (RESET == StatusFlag.stStatusFlag.bTCProcessed))
		{
			uiTempReadCnt = 0;
			fnReadTemp();
			
//			sprintf((char *)rgucTemp, "Temperature out read temp function %d = %0.04f C \r\n", (ucCReading-1),
//												stTC.stRead[ucCReading-1].fCurrentTemp);
//				DEBUG_PRINTF((const char *)rgucTemp);
		}
		/*condition check for Data Read Time */
		if((DATA_READ_TIME <= uiTempReadCount) && 
			 (SET == StatusFlag.stStatusFlag.bTCProcessed))
		{
			uiTempReadCount = 0;
			
			for(ucLoop = 0; ucLoop < NO_OF_TC_SENSOR; ucLoop++)
			{
				fnTemprature_Process(ucLoop);
			}
			/*RESET for data reading from thermocouple*/
			StatusFlag.stStatusFlag.bTCProcessed = RESET;
			ucCReading = 0;
		}
		
		if(DATA_READ_TIME_LL <= uiTimCountLL )
		{
			uiTimCountLL = 0;			
			fnLiquidLevel_Check(BASE);
			fnLiquidLevel_Check(HARDNER);
		}
		
		/*Check SSR Status and Fault and take action accordingly*/
		fnHeater_Check();
	}
}	
/********************************************************************************
 **@Function 		: fnHearBeat_Check
 **@Description	: this function will Check the HeartBeat 
 **@Parameters	: None
 **@Return			: None
 *******************************************************************************/
void fnHearBeat_Check(void)
{
	if(HEARTBEAT_TIMEOUT <= uiHBTimeCounter)
	{
		uiHBTimeCounter = 0;		
		StatusFlag.stStatusFlag.bCANFaultSCU = SET;
		if(RESET == StatusFlag.stStatusFlag.bHBNotRcvFlag)
		{
			/* this is used to send the fault only once*/
			StatusFlag.stStatusFlag.bHBNotRcvFlag = SET;
		}
	}
}
/********************************************************************************
 **@Function		 	:  fnEstop_Check()
 **@Descriptions	:  Estop message check
 **@Parameters		:  None
 **@Return				:  None
 *******************************************************************************/
void fnEstop_Check(void)
{
	if(SET == StatusFlag.stStatusFlag.bEStopFlag)
	{
		StatusFlag.stStatusFlag.bEStopFlag = RESET;
	}
}
/********************************************************************************
 **@Function		 	:  fnDelay_ms
 **@Descriptions	:  this funtion will give the delay approx 1 miliseconds when 
										 uiDly = 1, for clock Frequency 72Mhz
 **@Parameters		:  uiDly: will give the No. of milisecond delay e.g 1 
										 for Approx 1milisec 
 **@Return				:  None
 *******************************************************************************/
void fnDelay_ms(unsigned int uiDly)
{
	uint16_t uiLoop;
	for(; uiDly > 0; uiDly--)
	{
		for(uiLoop = 24000; uiLoop > 0; uiLoop--);
	}
}



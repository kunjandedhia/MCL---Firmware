/******************************************************************************
  * @file    MiscFunctions.c
  * 
  * @brief   This file contains the miscelaneous functions
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
	FLAGS_Union_t	StatusFlag;			/*union variable of status flags*/
	uint8_t ucStoreBuffCnt 			= 0;
	uint8_t ucTxBuffCnt 				= 0;
	uint8_t ucMode 							= 0;
	uint8_t	ucBypass 						= 0;
	uint8_t	ucStallValvePos 		= 0;
	uint8_t	ucOutsideRemap			= 0;
	uint8_t	ucInsideRemap 			= 0; 
	uint16_t uiIOExpander1 			= 0;
	uint16_t uiPreIOExpander1 	= 0;
	uint16_t uiIOExpander2 			= 0;
	uint16_t uiPreIOExpander2 	= 0;
	uint8_t ucAPSConnStatus			=	0;
	uint8_t ucPreAPSConnStatus	=	0;
	uint8_t ucPOSTLEDBlinkCnt	= 0; /*for LED Blinking on Power on Reset till no HB Rcv*/

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
 **@Function 			: fnDefault_Config
 **@Descriptions	: This function will set the Default state for the system
 **@Parameters		: None 
 **@Return				: None
 ****************************************************************************/
void fnDefault_Config(void)
{
	uint8_t ucLoop = 0;
	
	/*Valve to default state*/
	fnValve_ONOFF(STALL_VALVE, STALL_VALVE_OFF);
	#ifdef REC_VAL_DBG
		DEBUG_PRINTF("Default Config Stall valve off\r\n");
	#endif
	/*Set CAN enable pins to default state*/
	GPIO_WriteBit(CAN_ENABLE_PORT, CAN_ENABLE_PIN ,Bit_SET);
	GPIO_WriteBit(CAN_STROBE_PORT, CAN_STROBE_PIN ,Bit_SET);
	
	/*RESET All Status Flags*/
	StatusFlag.uliFlagReset = 0;
	
	for(ucLoop = 0; ucLoop < NUM_OF_PG; ucLoop++)
	{
		stPG[ucLoop].bHTHFaultFlag = RESET;	
		stPG[ucLoop].bLTHFaultFlag = RESET;	
	}

	stPG[PG1].bLegStatus = RESET;
	stPG[PG2].bLegStatus = RESET;
	stPG[PG3].bLegStatus = RESET;	

	stStallFlag.bTBeforeStall = RESET;
	stStallFlag.bTBeforeStallPresCompr = RESET;		
	stStallFlag.bPumpOnDownStroke = RESET;
	stStallFlag.bPumpOnUpStroke = RESET;
	stStallFlag.bCompareLegs = RESET;
	stStallFlag.bDownStrokeDone = RESET;
	stStallFlag.bUpStrokeDone = RESET;
	stStallFlag.bStartDownStroke = RESET;
	stStallFlag.bStartUpStroke = RESET;
	stStallFlag.bUpStrokeLegCmpr = RESET;
	stStallFlag.bDownStrokeLegCmpr = RESET;
	stStallFlag.bUpStrokePass = RESET;
	stStallFlag.bDownStrokePass = RESET;
	
	/*Reset All status LEDs*/
	uiIOExpander2 = IO1_ALL_RED_LED; 
	uiIOExpander1 = IO2_ALL_RED_LED;
}

/******************************************************************************
 **@Function		 	:  fnEstop_Check
 **@Descriptions	:  Estop message check
 **@Parameters		:  None
 **@Return				:  None
 ****************************************************************************/
void fnEstop_Check(void)
{
	if((SET == StatusFlag.stStatusFlag.bEstop) && (RESET == StatusFlag.stStatusFlag.bStallTestStart))
	{
		/*turn OFF all the valves*/
    fnValve_ONOFF(STALL_VALVE, STALL_VALVE_OFF);
		#ifdef REC_VAL_DBG
			DEBUG_PRINTF("EStop Stall valve OFF\r\n");
		#endif
//		/*Reset status LED for VALVES*/
//		uiIOExpander1 &= ~STALL_VALVE_LED_VALUE(RED);
//		uiIOExpander1 &= ~STALL_VALVE_LED_VALUE(GREEN);
//		uiIOExpander1 &= ~STALL_VALVE_LED_VALUE(BLUE);

//		/*Reset status LED for Pressure gauges*/
//		uiIOExpander1 &= ~PG_LED_VALUE(PG1, RED);
//		uiIOExpander1 &= ~PG_LED_VALUE(PG1, GREEN);
//		uiIOExpander1 &= ~PG_LED_VALUE(PG1, BLUE); 
//		
//		uiIOExpander1 &= ~PG_LED_VALUE(PG2, RED);z
//		uiIOExpander1 &= ~PG_LED_VALUE(PG2, GREEN);
//		uiIOExpander1 &= ~PG_LED_VALUE(PG2, BLUE); 
//		
//		uiIOExpander2 &= ~PG_LED_VALUE(PG3, RED);
//		uiIOExpander2 &= ~PG_LED_VALUE(PG3, GREEN);
//		uiIOExpander2 &= ~PG_LED_VALUE(PG3, BLUE); 
	}
}
/******************************************************************************
 **@Function		 	:  fnDefault_Mode()
 **@Descriptions	:  Check for the first heartbeat (Received after Power ON)
 **@Parameters		:  None
 **@Return				:  None
 ****************************************************************************/
void fnDefault_Mode(void)
{
	if(DEFAULT_MODE == ucMode)
	{
		#ifdef DEFAULT_MODE_DBG
			DEBUG_PRINTF("uliConfigParaByte = 0%02x\r\n",uliConfigParaByte);
		#endif
		/* it will check for first heartbeat only*/
		if((SET == StatusFlag.stStatusFlag.bHeartBeatMsgFlag) && 
			 (RESET == StatusFlag.stStatusFlag.bFirstHrtBtMsgFlag)) 
		{
			/* Setting the first heart beat flag */
			StatusFlag.stStatusFlag.bFirstHrtBtMsgFlag = SET;
			StatusFlag.stStatusFlag.bHeartBeatMsgFlag 	= RESET;
			
			/* TIM1 enable counter */
			TIM_Cmd(TIM1, ENABLE);
			
			ucPOSTLEDBlinkCnt = 0;
			uiIOExpander1 = 0;
			uiIOExpander2 = 0;
			
			#ifdef DEFAULT_MODE_DBG
				DEBUG_PRINTF("First Heartbeat Received\r\n");
			#endif
		}
		/*Read thermocouple in interval of 50msec after first Hearbeat*/
		if((EE07_READ_TIME <= uiAmbiantReadCnt) &&
			 (RESET == StatusFlag.stStatusFlag.bAmbiantReadDone) &&
			 (CONFIG_PARA_BYTE_VCM == uliConfigParaByte))
		{
			fnEE07_Process();	
		}
		if((SET == StatusFlag.stStatusFlag.bFirstHrtBtMsgFlag) && 
			 (RESET == StatusFlag.stStatusFlag.bDiagnosticsFlag) &&
			 (CONFIG_PARA_BYTE_VCM == uliConfigParaByte))
		{
			/* Self Diagnost and if Pass then send Pass status and
				 Config CMD if fail then send Fail Status */
			if(SET == StatusFlag.stStatusFlag.bAmbiantReadDone)
			{
				fnSelfDiagnostic_Test();
				StatusFlag.stStatusFlag.bDiagnosticsFlag = SET;
				#ifdef DEFAULT_MODE_DBG
					DEBUG_PRINTF("fnSelfDiagnostic_Test\r\n");
				#endif
			}
		}
		/* after Diagnostic pass and Config parameters received, 
			Switch from Default Mode to Normal Mode */
		if(SET == StatusFlag.stStatusFlag.bDiagnosticsPass)
		{
			ucMode = OPERATION_MODE; 
			uiAmbiantReadCnt = 0;
			StatusFlag.stStatusFlag.bAmbiantReadDone = RESET;
			#ifdef DEFAULT_MODE_DBG
				DEBUG_PRINTF("Mode Switch to Normal Mode ulidevi\r\n");
			#endif
		}
	}
}
/******************************************************************************
 **@Function		 	:  fnLED_Indication
 **@Descriptions	:  Blibk the LED if CAN fault occurs
 **@Parameters		:  None
 **@Return				:  None
 ****************************************************************************/
void fnLED_Indication(void)
{	
	if(RESET == StatusFlag.stStatusFlag.bFirstHrtBtMsgFlag)
	{			
		if(ucPOSTLEDBlinkCnt < POST_LED_BLINK_COUNT)
		{
			/* ON Board status RED LED*/
			uiIOExpander1 =	IO1_ALL_RED_LED;
			uiIOExpander2 =	IO1_ALL_RED_LED;
		}	
		else if(ucPOSTLEDBlinkCnt > POST_LED_BLINK_COUNT)
		{
			/* OFF Board status RED LED*/
			uiIOExpander1 =	IO2_ALL_BLUE_LED;
			uiIOExpander2 =	IO2_ALL_BLUE_LED;
			
			/* RESET The Blinking LED COUNTER if greatere than 200msec */
			if(ucPOSTLEDBlinkCnt >= (2 * POST_LED_BLINK_COUNT))
			{
				ucPOSTLEDBlinkCnt = 0;
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
/******************************************************************************
 **@Function		 	:  fnSelfDiagnostic_Test
 **@Descriptions	:  Diagnostic taste
 **@Parameters		:  None
 **@Return				:  None
 ****************************************************************************/
void fnSelfDiagnostic_Test(void)
{
  uint8_t ucStatus1 = 0, ucStatus2 = 0, uctemp = 0;
	uint8_t ucLoop = 0;
  float fLinearTransRead = 0.0f;
	
	/*Getting Status of Stall valve connected*/
	for(ucLoop = 0; ucLoop < VALVE_USED; ucLoop++)
	{
		/*Getting Status of valve connected*/
		ucStatus1 |= (uint8_t)(GPIO_ReadInputDataBit(VALVE_CONN_STAT_PORT,
											VALVE_CONN_STAT_PIN(stValve[ucLoop].ucPosition)) << ucLoop);
	}
	
	
	/* invert the read connected status of valves*/
	ucStatus1 = ((~ucStatus1) & 0x03);

	#ifdef	DIAG_DBG
		DEBUG_PRINTF("Stall valve connected :%x\r\n",ucStatus1);
	#endif
	
	/*Check for Valves diagnostic*/
	{
		
		/*turn OFF Stall valves*/
		fnValve_ONOFF(STALL_VALVE, STALL_VALVE_OFF);
		fnValve_ONOFF(RECIRCULATION_VALVE, STALL_VALVE_OFF);
		
		fnDelay_ms(10);
		/*Add Bypass Logic here*/
		for(ucLoop = 0; ucLoop < VALVE_USED; ucLoop++)
		{
			ucStatus2 |= (GPIO_ReadInputDataBit(VALVE_FEEDBACK_PORT,
										VALVE_FEEDBACK_PIN(stValve[ucLoop].ucPosition)) << ucLoop);
		}
		
		#ifdef	DIAG_DBG
			DEBUG_PRINTF("\r\n Valve Status :%x\r\n", ucStatus2);
		#endif
	}
	//else/*Stall valve not connected*/
	if(0 != ucStatus1)
	{
		//DEBUG_PRINTF("Valve disconnected\r\n");
		/*if Stall valve is not connected to the system or configured port*/
		if(0x01 == (ucStatus1 & 0x01))
		{
//			ucStatus2 |= 0x01;
			/*if stall valve not connected then set it one as fault*/
			uiIOExpander1 &= ~STALL_VALVE_LED_VALUE(GREEN);		/*GREEN LED OFF*/
			uiIOExpander1 |= STALL_VALVE_LED_VALUE(RED);		/*FAULT/RED LED ON*/
		}
	}

	
	/*Backup the ucStatus1 before modifying it*/
	uctemp = ucStatus1;
	
	/*Bypassing Valve1 diagnostic as per the configration sent by SCU*/
	if((ucBypass >> 4) & 0x01)
	{
		ucStatus1 = 0; /*Pass the diagnostic of the Stall valve if its Bypassed*/
	}
	else
	{
		/*Replace the bits of the Status of the Stall valve from the status2 to status 1*/
		/*first bit of ucStatus2 is placed in the second bit of ucStatus1*/
		ucStatus1 = ((ucStatus1 & (~0x02)) | ((ucStatus2 & 0x01) << 1));
	}
	
	/*Bypassing Valve2 diagnostic as per the configration sent by SCU*/
	if((ucBypass >> 7) & 0x01)
	{
		ucStatus2 = 0;
	}
	else
	{
		/*second bit of ucStatus1 is placed in the First bit of ucStatus2*/
		ucStatus2 = ((ucStatus2 & (~0x01)) | ((uctemp & 0x02) >> 1));
	}
	
	#ifdef	DIAG_DBG
		DEBUG_PRINTF("After changing the bits :%x %x\r\n",ucStatus1, ucStatus2);
	#endif
	
	/*Pressure gauge Diagnostic*/
	fnPGSensor_Read(DIAGNOS_CALL);
	for(ucLoop = 0; ucLoop < NUM_OF_PG; ucLoop++)
	{
		/*PG Diagnostic Fail this is change for testing*/
		if(((MIN_PRESSURE > CURRENT_PG_DATA(ucLoop)) || 
			 (MAX_PRESSURE < CURRENT_PG_DATA(ucLoop))) && 
		  (RESET == ((ucBypass >> ucLoop + 1) & 0x01)))
		{
			/*If PG is Bypass then Pass the Diagnostic of PG's*/
			ucStatus1 |=  (0x01 << (ucLoop + 2));
			
			if(PG3 == ucLoop)
			{
				uiIOExpander2 |= PG_LED_VALUE(ucLoop, RED);
				uiIOExpander2 &= ~PG_LED_VALUE(ucLoop, GREEN);
			}
			else
			{
				uiIOExpander1 |= PG_LED_VALUE(ucLoop, RED);
				uiIOExpander1 &= ~PG_LED_VALUE(ucLoop, GREEN);
			}
			
			#ifdef DIAG_DBG
				DEBUG_PRINTF("Diagnostic Fail PG %d\r\n",ucLoop);
			#endif
		}
		else /*PG Diagnostic Pass*/
		{
			if(PG3 == ucLoop)
			{
				uiIOExpander2 &= ~PG_LED_VALUE(ucLoop, RED);
				uiIOExpander2 |= PG_LED_VALUE(ucLoop, GREEN);
			}
			else
			{
				uiIOExpander1 &= ~PG_LED_VALUE(ucLoop, RED);
				uiIOExpander1 |= PG_LED_VALUE(ucLoop, GREEN);
			}
			//DEBUG_PRINTF("Diagnostic PASS PG %d\r\n",ucLoop);
		}
	}
	/*Linear transducer Diagnostic*/
	fLinearTransRead = fnLinearPosition();
	if(( 0.0f > fLinearTransRead) || ( 150.0f < fLinearTransRead))
	{
		if(RESET == (ucBypass & 0x01))/*Diagnostic Bypass for LT*/
		{
			ucStatus1 |=  (0x01 << 5);
			
			uiIOExpander1 |= LT_LED_VALUE(RED);
			uiIOExpander1 &= ~LT_LED_VALUE(GREEN);
		}
		//DEBUG_PRINTF("Diagnostic Fail LT\r\n");
	}
	else
	{
		uiIOExpander1 &= ~LT_LED_VALUE(RED);
		uiIOExpander1 |= LT_LED_VALUE(GREEN);
		//DEBUG_PRINTF("Diagnostic Pass LT\r\n");
	}
	
	if(0 != ucInsideStatus)
	{
		if(RESET == ((ucBypass >> 5) & 0x01))
		{
			ucStatus1 |=  (0x01 << 6);
			//DEBUG_PRINTF("EE07 Diagnostic Fail ucInsideStatus = %d\r\n", ucInsideStatus);
		}
	}
	else
	{
		//DEBUG_PRINTF("EE07 IN Diagnostic PASS\r\n");
	}
	
	if(0 != ucOutsideStatus)
	{
		if(RESET == ((ucBypass >> 6) & 0x01))
		{
			ucStatus1 |=  (0x01 << 7);
			//DEBUG_PRINTF("EE07 Diagnostic Fail ucOutsideStatus = %d\r\n", ucOutsideStatus);
		}
	}
	else
	{
		//DEBUG_PRINTF("EE07 OUT Diagnostic PASS\r\n");
	}
	
	stTxMsg.DLC 	= 3;
  if((0 == ucStatus1) && (0 == ucStatus2))
  {
		/*Diagnostic Pass*/
    stTxMsg.Data[0] = DIAGNOSTICS_PASS;
    stTxMsg.Data[1] = ucStatus1;
		stTxMsg.Data[2] = ucStatus2;
		StatusFlag.stStatusFlag.bDiagnosticsPass = SET;
		#ifdef DIAG_DBG
			DEBUG_PRINTF("Diagnostic Pass data :%x %x\r\n", ucStatus1, ucStatus2);
		#endif
  }
	else 
  {
		/*Diagnostic Fails*/
    stTxMsg.Data[0] = DIAGNOSTICS_FAIL;
    stTxMsg.Data[1] = ucStatus1;
		stTxMsg.Data[2] = ucStatus2;
		StatusFlag.stStatusFlag.bDiagnosticsPass = RESET;
		#ifdef DIAG_DBG
			DEBUG_PRINTF("Diagnostic Fail  data : %x %x\r\n", ucStatus1, ucStatus2);
		#endif
  }
	fnStatus_Send(STATUS_VCM_DIAGNOSTIC);
	
	/*11thMarch-Sending code version after diagnostic completed*/
	stTxMsg.DLC 	= 5;
	stTxMsg.Data[0] = MODULE_CODE;
	SplitByte.fData = CODE_VERSION;
	for(ucLoop = 1; ucLoop < stTxMsg.DLC; ucLoop++)
	{
		stTxMsg.Data[ucLoop] = SplitByte.rgucData[ucLoop - 1];
	}
	fnStatus_Send(STATUS_SOFTWARE_VERSION);
	
//	uiIOExpander2 |= APS_LED_VALUE(RED);
//	uiIOExpander2 &= ~APS_LED_VALUE(GREEN);
}
/******************************************************************************
 **@Function		 	:  fnOperation_Mode
 **@Descriptions	:  Operation mode of the VCM
 **@Parameters		:  None
 **@Return				:  None
 ****************************************************************************/
void fnOperation_Mode(void)
{
	#ifdef PG_DBG
		char rgcTemp[150];
	#endif
	if(OPERATION_MODE == ucMode) 
	{
		/************************* PRESSURE GAUGE ************************/
		if(PG_READ_TIME < uiPressureReadCounter)
    {
			
			fnPG_Process();
			uiPressureReadCounter = 0;
		}
		/************************* VALVE ************************/
		fnValve_Process();
		
		/************************* STALL TEST ************************/
		if(SET == StatusFlag.stStatusFlag.bStallTestStart )
		{
			/*Stall test function*/
			fnStall_Test();
			if((SET == stStallFlag.bDownStrokeDone)&&(SET == stStallFlag.bUpStrokeDone))
			{
				DEBUG_PRINTF("STALL TEST DONE\r\n");
				stStallFlag.bDownStrokeDone = RESET;
				stStallFlag.bUpStrokeDone = RESET;
				stStallFlag.bStartDownStroke 				= RESET;
				stStallFlag.bStartUpStroke 					= RESET;
				StatusFlag.stStatusFlag.bStallTestStart = RESET;
				ucStrokeType 						= NONE;
				ucPreStrokeType 				= NONE;	
				
			}
		}
		//else
	//	{
			/* adf 2/3/2017 this just to avoid reading of EE07 while Stall is performing*/
			/*******************   AMBIANT TEMP AND HUMIDITY *****************/
			fnEE07_Process();
	//	}
		
		fnLT_Process();
		
		
		
		ucAPSConnStatus =  GPIO_ReadInputDataBit(APS_GPIO_PORT, APS_GPIO_PIN1);
		if(ucPreAPSConnStatus != ucAPSConnStatus)
		{
			stTxMsg.DLC = 1;
			if(0 == ucAPSConnStatus)
			{
				stTxMsg.Data[0] = 0x02; /*This state that Air present or Connected*/
				uiIOExpander2 &= ~APS_LED_VALUE(RED);
				uiIOExpander2 |= APS_LED_VALUE(GREEN);
			}
			else
			{
				stTxMsg.Data[0] = 0x01; /*this state that No Air or disconnected*/
				uiIOExpander2 |= APS_LED_VALUE(RED);
				uiIOExpander2 &= ~APS_LED_VALUE(GREEN);
			}
			
			fnStatus_Send(STATUS_AIR_SENSOR_CONN);
			ucPreAPSConnStatus = ucAPSConnStatus;
		}
	}
}

/*****************************************************************************
 **@Function		 	:  fnAllWarnnings_Clear
 **@Descriptions	:  This funtion clear all the Warnning flags
 **@Parameters		:  None
 **@Return				:  None
 ****************************************************************************/
void fnAllWarnnings_Clear(void)
{
	#ifdef GENERAL_DBG  
		DEBUG_PRINTF("VCM Clear Warnings\r\n");
	#endif
	/*clear all Warnings*/
	stPG[PG1].bLTHFaultFlag = RESET;	
	stPG[PG2].bLTHFaultFlag = RESET;	
	stPG[PG3].bLTHFaultFlag = RESET;	
	stValve[STALL_VALVE].bFaultFlag = RESET;
	ucValveFaultHistory = 0; /*Clear all Valve fault to enable the LED Toggle*/
	ucPrevValveFault    = 0;
	ucValveFault				=	0;
}
/******************************************************************************
 **@Function		 	:  fnAllFaults_Clear
 **@Descriptions	:  this function Claer all the faults flags
 **@Parameters		:  None
 **@Return				:  None
 ****************************************************************************/
void fnAllFaults_Clear(void)
{
	#ifdef GENERAL_DBG  
		DEBUG_PRINTF("VCM Clear faults\r\n");
	#endif
	StatusFlag.stStatusFlag.bEstop = RESET;
	
	/*Clear all faults*/
	StatusFlag.stStatusFlag.bCANFaultSCU = RESET;
	StatusFlag.stStatusFlag.bCANFaultHMI = RESET;
	StatusFlag.stStatusFlag.bHBNotRecv 	= RESET;
	
	stPG[PG1].bHTHFaultFlag = RESET;	
	stPG[PG2].bHTHFaultFlag = RESET;	
	stPG[PG3].bHTHFaultFlag = RESET;	
	
	StatusFlag.stStatusFlag.bLTRangeCheck = RESET;
	
	StatusFlag.stStatusFlag.bEstop = RESET;
	
	/*Turn OFF all the valves*/
	fnValve_ONOFF(STALL_VALVE, STALL_VALVE_OFF);
	
	#ifdef REC_VAL_DBG
		DEBUG_PRINTF("STALL_VALVE OFF, Clear Fault\r\n");
	#endif
  ucValveFaultHistory = 0; /*Clear all Valve fault to enable the LED Toggle*/
	ucPrevValveFault    = 0;
	ucValveFault				=	0;
	/*Need to add all the Flags whichever is needed*/
	
	stValve[STALL_VALVE].bFaultFlag = RESET;
}
/******************************************************************************
 **@Function		 	:  fnDelay_ms
 **@Descriptions	:  this funtion will give the delay approx 1 miliseconds when 
										 uiDly = 1, for clock Frequency 72Mhz
 **@Parameters		:  uiDly: will give the No. of milisecond delay e.g 1 
										 for Approx 1milisec 
 **@Return				:  None
 ****************************************************************************/
void fnDelay_ms(unsigned int uiDly)
{
	uint16_t uiLoop;
	for(; uiDly > 0; uiDly--)
	{
		for(uiLoop = 36000; uiLoop > 0; uiLoop--);
	}
}

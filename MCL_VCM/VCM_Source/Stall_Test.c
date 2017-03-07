/******************************************************************************
  * @file    StallTest.c
  * 
  * @brief   This file contains the stall test functions
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
	StallTest_t stStallFlag;
	float fValue1 									= 0.0f;
	float fValue2 									= 0.0f;
	float fLinearPosition 					= 0.0f;
	float fTimeBeforeStall 					= 0.0f;
	float fTimBfrCompStallPressure 	= 0.0f;
	
	float fPreviousPG1 = 0.0f;
	float fPreviousPG2 = 0.0f;
	float fPreviousPG3 = 0.0f;
	
	/*4thMarch2016*/
	uint8_t ucUpstrokeLimit = 0;
	uint8_t ucUpstrokeMin = 0;
	uint8_t ucUpstrokeMax = 0;
	uint8_t ucDownstrokeLimit = 0;
	uint8_t ucDownstrokeMin = 0;
	uint8_t ucDownstrokeMax = 0;
	
	/*variables to store the upstroke and downstroke threshold in percentage*/
	int16_t iStallPositiveTh 				= 10;
	int16_t iStallNegativeTh 				= -10;
	
	uint16_t uiTBeforeStallCnt 			= 0;
	
	uint8_t ucStokeType 						= 0;
	uint8_t ucStrokeType 						= NONE;
	uint8_t ucPreStrokeType 				= NONE;
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
uint8_t fnTypeOfStroke(void);
void fnStall_Logic(void);

/******************************************************************************
 **@Function 	 	: fnStall_Test
 **@Descriptions: Stall test operation 
 **@Parameters	: None
 **@Return			: None
 ****************************************************************************/
void fnStall_Test(void)
{
	if(NONE == ucStrokeType) //&& (RESET == stStallFlag.bStartDownStroke))
	{
		ucStrokeType = fnTypeOfStroke();	
		if(ucPreStrokeType == ucStrokeType )
		{
			ucStrokeType = NONE;
	//		DEBUG_PRINTF("!!!! \r\n");
		}
		else if((NONE != ucStrokeType ) && (RESET == stStallFlag.bStartDownStroke) && (RESET == stStallFlag.bStartUpStroke))
		{
			ucPreStrokeType = ucStrokeType;
			
		}
		if( DOWNSTROKE == ucStrokeType)
		{
			#ifdef	STALL_DBG
				DEBUG_PRINTF("Downstroke GOT!!!! \r\n");
			#endif
		}
		if( UPSTROKE == ucStrokeType)
		{
			#ifdef	STALL_DBG
				DEBUG_PRINTF("Upstroke GOT!!!! \r\n");
			#endif
		}
	}
	/*STEP 1*/
	if((DOWNSTROKE == ucStrokeType) && (RESET == stStallFlag.bStartDownStroke))
	{
		fLinearPosition = fnLinearPosition();
		if(fLinearPosition < ((float)ucDownstrokeLimit))
		{
			stStallFlag.bPumpOnDownStroke = SET;
		}
		
	}
	if((( ((float)ucDownstrokeMin) < fLinearPosition ) && 
			(((float)ucDownstrokeMax) > fLinearPosition )) && 
	    (SET == stStallFlag.bPumpOnDownStroke))			 
	{
		
		#ifdef	STALL_DBG
			DEBUG_PRINTF("Step 1 \r\n");
			DEBUG_PRINTF("Downstroke Start \r\n");
		#endif	
		stStallFlag.bStartDownStroke = SET;
		stStallFlag.bPumpOnDownStroke = RESET;
		stStallFlag.bTBeforeStall = SET;
				
		/*OFF Valve recirculation closed*/
		fnValve_ONOFF(STALL_VALVE, STALL_VALVE_ON);
	}
	/*STEP 2*/		
	if((uiTBeforeStallCnt > fTimeBeforeStall) && 
		 (SET == stStallFlag.bTBeforeStall))
	{
		uiTBeforeStallCnt = 0;
		stStallFlag.bTBeforeStall = RESET;
		stStallFlag.bCompareLegs = SET;
		
		if(DOWNSTROKE == ucStrokeType)
		{
			stStallFlag.bDownStrokeLegCmpr = SET;
		}
		else if(UPSTROKE == ucStrokeType)
		{
			stStallFlag.bUpStrokeLegCmpr = SET;
		}
		fnPGSensor_Read(STALL_CALL);
		
		fPreviousPG1 = fCurrentPG1; 
		fPreviousPG2 = fCurrentPG2;
		fPreviousPG3 = fCurrentPG3;
		 
		#ifdef	STALL_DBG
			DEBUG_PRINTF("Step 2 \r\n");
		#endif	
	}
	/*STEP3*/
	//if((SET == stStallFlag.bCompareLegs ) && (SET == stStallFlag.bCompareLegs ))
	if((SET == stStallFlag.bCompareLegs ) && (SET == stStallFlag.bDownStrokeLegCmpr )) /*change1*/
	{
		stStallFlag.bTBeforeStallPresCompr = SET;
		fnStall_Logic();		
	}
	/*STEP4*/
//	if(SET == stStallFlag.bDownStrokeDone )
//	{
//		ucStrokeType = fnTypeOfStroke();	
//		if((UPSTROKE == ucStrokeType ) && (RESET == stStallFlag.bStartUpStroke))
//		{
//			stStallFlag.bDownStrokeDone = RESET;
//			stStallFlag.bStartUpStroke = SET;
//			#ifdef	STALL_DBG
//				DEBUG_PRINTF("Step 4 \r\n");
//				DEBUG_PRINTF("UpStroke GOT!!!! \r\n");
//			#endif	
//		}
//	}
	/*Upstroke start*/
	if((UPSTROKE == ucStrokeType ) && (RESET == stStallFlag.bStartUpStroke ))
	{
		fLinearPosition = fnLinearPosition();
		if( ((float)ucUpstrokeLimit) < fLinearPosition )
		{
			stStallFlag.bPumpOnUpStroke = SET;
		}
	}
	if((( ((float)ucUpstrokeMin) < fLinearPosition) && 
		  (((float)ucUpstrokeMax) > fLinearPosition )) && 
	    (SET == stStallFlag.bPumpOnUpStroke ))			 
	{
		#ifdef	STALL_DBG
			DEBUG_PRINTF("10 to 70 POSITION GOT \r\n");
		#endif
		
		/*OFF Valve*/
		fnValve_ONOFF(STALL_VALVE, STALL_VALVE_ON);
		stStallFlag.bStartUpStroke 		= SET;
		stStallFlag.bPumpOnUpStroke 	= RESET;
		stStallFlag.bPumpOnDownStroke = RESET;
	//	stStallFlag.bCompareLegs 			= SET;
	//	stStallFlag.bUpStrokeLegCmpr 	= SET;
		stStallFlag.bTBeforeStall 		= SET;
	}
	/*STEP 5*/
	if((SET == stStallFlag.bCompareLegs ) && 
		 ( SET == stStallFlag.bUpStrokeLegCmpr))
	{
		#ifdef	STALL_DBG
//			DEBUG_PRINTF("Step 5 Stall Logic \r\n");
		#endif
		stStallFlag.bTBeforeStallPresCompr = SET;
		fnStall_Logic();	
		
	}
	if(( SET == stStallFlag.bUpStrokePass)  ||
		 (SET == stStallFlag.bDownStrokePass ))
	{
		stTxMsg.DLC = 2;
		
		/*ON ReValve*/
		/*fnValve_ONOFF(STALL_VALVE, STALL_VALVE_OFF);*/
		
		if(SET == stStallFlag.bDownStrokePass)
		{
			stTxMsg.Data[0] = DOWNSTROKE_PASS;
			stStallFlag.bDownStrokePass = RESET;
	#ifdef	STALL_DBG
			DEBUG_PRINTF("Stall Test DOWNSTROKE_PASS!\r\n");
		#endif
			ucStrokeType = NONE;
		}
		else
		{
			stTxMsg.Data[0] = UPSTROKE_PASS;
			stStallFlag.bUpStrokePass = RESET;
		#ifdef	STALL_DBG
			DEBUG_PRINTF("Stall Test UPSTROKE_PASS!\r\n");
		#endif
			
		//	stStallFlag.bDownStrokePass 	= RESET;
			stStallFlag.bUpStrokePass 		= RESET;
	//		StatusFlag.stStatusFlag.bStallTestStart 	= RESET;
			
		//	stStallFlag.bStartDownStroke 	= RESET;
				
			stPG[PG1].bLegStatus 								= RESET;
			stPG[PG2].bLegStatus 								= RESET;
			stPG[PG3].bLegStatus 								= RESET;	
			stStallFlag.bTBeforeStall 					= RESET;
			stStallFlag.bTBeforeStallPresCompr 	= RESET;	
			stStallFlag.bPumpOnDownStroke 			= RESET;
			stStallFlag.bPumpOnUpStroke 				= RESET;
			stStallFlag.bCompareLegs 						= RESET;
//			stStallFlag.bDownStrokeDone 				= RESET;
//			stStallFlag.bStartDownStroke 				= RESET;
//			stStallFlag.bStartUpStroke 					= RESET;
			stStallFlag.bUpStrokeLegCmpr 				= RESET;
			stStallFlag.bDownStrokeLegCmpr 			= RESET;

			ucStrokeType = NONE;
		}
		stTxMsg.Data[1] = ALL_PRESSURE_LEGS;
		fnStatus_Send(STATUS_STALL_TEST_RESULT);
			
	}
	else
	{
	#ifdef	STALL_DBG
//			DEBUG_PRINTF("Both stall flag not set!\r\n");
		#endif	
	}
}


///******************************************************************************
// **@Function 	 	: fnStall_Test
// **@Descriptions: Stall test operation 
// **@Parameters	: None
// **@Return			: None
// ****************************************************************************/
//void fnStall_Test(void)
//{
//	if((NONE == ucStrokeType) && (RESET == stStallFlag.bStartDownStroke))
//	{
//		ucStrokeType = fnTypeOfStroke();	
//		if(DOWNSTROKE != ucStrokeType )
//		{
//			ucStrokeType = NONE;
//		}
//		if( DOWNSTROKE == ucStrokeType)
//		{
//			#ifdef	STALL_DBG
//				DEBUG_PRINTF("Downstroke GOT!!!! \r\n");
//			#endif
//		}
//	}
//	/*STEP 1*/
//	if((DOWNSTROKE == ucStrokeType) && (RESET == stStallFlag.bStartDownStroke))
//	{
//		fLinearPosition = fnLinearPosition();
//		if(fLinearPosition < ((float)ucDownstrokeLimit))
//		{
//			stStallFlag.bPumpOnDownStroke = SET;
//		}
//		#ifdef	STALL_DBG
//			DEBUG_PRINTF("Step 1 \r\n");
//			DEBUG_PRINTF("Downstroke Start \r\n");
//		#endif	
//	}
//	if((( ((float)ucDownstrokeMin) < fLinearPosition ) && 
//			(((float)ucDownstrokeMax) > fLinearPosition )) && 
//	    (SET == stStallFlag.bPumpOnDownStroke))			 
//	{
//		stStallFlag.bStartDownStroke = SET;
//		stStallFlag.bPumpOnDownStroke = RESET;
//		stStallFlag.bTBeforeStall = SET;
//				
//		/*OFF Valve recirculation closed*/
//		fnValve_ONOFF(STALL_VALVE, STALL_VALVE_ON);
//	}
//	/*STEP 2*/		
//	if((uiTBeforeStallCnt > fTimeBeforeStall) && 
//		 (SET == stStallFlag.bTBeforeStall))
//	{
//		uiTBeforeStallCnt = 0;
//		stStallFlag.bTBeforeStall = RESET;
//		stStallFlag.bCompareLegs = SET;
//		
//		if(DOWNSTROKE == ucStrokeType)
//		{
//			stStallFlag.bDownStrokeLegCmpr = SET;
//		}
//		else if(UPSTROKE == ucStrokeType)
//		{
//			stStallFlag.bUpStrokeLegCmpr = SET;
//		}
//		fnPGSensor_Read(STALL_CALL);
//		
//		fPreviousPG1 = fCurrentPG1; 
//		fPreviousPG2 = fCurrentPG2;
//		fPreviousPG3 = fCurrentPG3;
//		 
//		#ifdef	STALL_DBG
//			DEBUG_PRINTF("Step 2 \r\n");
//		#endif	
//	}
//	/*STEP3*/
//	//if((SET == stStallFlag.bCompareLegs ) && (SET == stStallFlag.bCompareLegs ))
//	if((SET == stStallFlag.bCompareLegs ) && (SET == stStallFlag.bDownStrokeLegCmpr )) /*change1*/
//	{
//		stStallFlag.bTBeforeStallPresCompr = SET;
//		fnStall_Logic();		
//	}
//	/*STEP4*/
//	if(SET == stStallFlag.bDownStrokeDone )
//	{
//		ucStrokeType = fnTypeOfStroke();	
//		if(UPSTROKE == ucStrokeType )
//		{
//			stStallFlag.bDownStrokeDone = RESET;
//			stStallFlag.bStartUpStroke = SET;
//			#ifdef	STALL_DBG
//				DEBUG_PRINTF("Step 4 \r\n");
//				DEBUG_PRINTF("UpStroke GOT!!!! \r\n");
//			#endif	
//		}
//	}
//	/*Upstroke start*/
//	if((UPSTROKE == ucStrokeType ) && (SET == stStallFlag.bStartUpStroke ))
//	{
//		fLinearPosition = fnLinearPosition();
//		if( ((float)ucUpstrokeLimit) < fLinearPosition )
//		{
//			stStallFlag.bPumpOnUpStroke = SET;
//		}
//	}
//	if((( ((float)ucUpstrokeMin) < fLinearPosition) && 
//		  (((float)ucUpstrokeMax) > fLinearPosition )) && 
//	    (SET == stStallFlag.bPumpOnUpStroke ))			 
//	{
//		#ifdef	STALL_DBG
//			DEBUG_PRINTF("10 to 70 POSITION GOT \r\n");
//		#endif
//		
//		/*OFF Valve*/
//		fnValve_ONOFF(STALL_VALVE, STALL_VALVE_ON);
//		stStallFlag.bStartUpStroke 		= RESET;
//		stStallFlag.bPumpOnUpStroke 	= RESET;
//		stStallFlag.bPumpOnDownStroke = RESET;
//		stStallFlag.bCompareLegs 			= SET;
//		stStallFlag.bUpStrokeLegCmpr 	= SET;
//		stStallFlag.bTBeforeStall 		= SET;
//	}
//	/*STEP 5*/
//	if((SET == stStallFlag.bCompareLegs ) && 
//		 ( SET == stStallFlag.bUpStrokeLegCmpr))
//	{
//		#ifdef	STALL_DBG
//			DEBUG_PRINTF("Step 5 Stall Logic \r\n");
//		#endif
//		stStallFlag.bTBeforeStallPresCompr = SET;
//		fnStall_Logic();	
//	}
//	if(( SET == stStallFlag.bUpStrokePass)  ||
//		 (SET == stStallFlag.bDownStrokePass ))
//	{
//		stTxMsg.DLC = 2;
//		
//		/*ON ReValve*/
//		/*fnValve_ONOFF(STALL_VALVE, STALL_VALVE_OFF);*/
//		
//		if(SET == stStallFlag.bDownStrokePass)
//		{
//			stTxMsg.Data[0] = DOWNSTROKE_PASS;
//			stStallFlag.bDownStrokePass = RESET;
//		#ifdef	STALL_DBG
//			DEBUG_PRINTF("Stall Test DOWNSTROKE_PASS!\r\n");
//		#endif
//		}
//		else
//		{
//			stTxMsg.Data[0] = UPSTROKE_PASS;
//			stStallFlag.bUpStrokePass = RESET;
//		#ifdef	STALL_DBG
//			DEBUG_PRINTF("Stall Test UPSTROKE_PASS!\r\n");
//		#endif
//			
//			stStallFlag.bDownStrokePass 	= RESET;
//			stStallFlag.bUpStrokePass 		= RESET;
//			StatusFlag.stStatusFlag.bStallTestStart 	= RESET;
//			
//			stStallFlag.bStartDownStroke 	= RESET;
//				
//			stPG[PG1].bLegStatus 								= RESET;
//			stPG[PG2].bLegStatus 								= RESET;
//			stPG[PG3].bLegStatus 								= RESET;	
//			stStallFlag.bTBeforeStall 					= RESET;
//			stStallFlag.bTBeforeStallPresCompr 	= RESET;	
//			stStallFlag.bPumpOnDownStroke 			= RESET;
//			stStallFlag.bPumpOnUpStroke 				= RESET;
//			stStallFlag.bCompareLegs 						= RESET;
//			stStallFlag.bDownStrokeDone 				= RESET;
//			stStallFlag.bStartDownStroke 				= RESET;
//			stStallFlag.bStartUpStroke 					= RESET;
//			stStallFlag.bUpStrokeLegCmpr 				= RESET;
//			stStallFlag.bDownStrokeLegCmpr 			= RESET;

//			ucStrokeType = NONE;
//		}
//		stTxMsg.Data[1] = ALL_PRESSURE_LEGS;
//		fnStatus_Send(STATUS_STALL_TEST_RESULT);
//			
//	}
//	else
//	{
//		#ifdef	STALL_DBG
//			DEBUG_PRINTF("Both stall flag not set!\r\n");
//		#endif	
//	}
//}
/******************************************************************************
 **@Function 			: fnStall_Logic
 **@Descriptions	: Stall test logic implemented in STEP 3 according 
										to flow chart
 **@Parameters		: None
 **@Return				: None
 ****************************************************************************/
void fnStall_Logic(void)
{
	uint8_t ucLoop = 0;
	uint8_t ucFaultByte = 0x00;
	#ifdef STALL_DBG
		char rgcTemp[500];
	#endif
	/*Timing before comparing stall pressures*/
	if(uiTBeforeStallPresCompr > fTimBfrCompStallPressure)
	{
		fnPGSensor_Read(STALL_CALL);
			
		/*OFF Valve*/
		fnValve_ONOFF(STALL_VALVE, STALL_VALVE_OFF);
		
		for(ucLoop = 0; ucLoop < NUM_OF_PG; ucLoop++)
		{
			if( 0 == ucLoop )
			{
				stPG[ucLoop].iPercentage = (int16_t)(((fCurrentPG1 - 
								fPreviousPG1) / fPreviousPG1) * 100);
				
				/*Reading pressure gauge current value*/	
			if((1 < fPreviousPG1 ) && ( 1 < fCurrentPG1) && 
				 ((iStallNegativeTh < stPG[ucLoop].iPercentage) && 
					(iStallPositiveTh > stPG[ucLoop].iPercentage)))
				{
					stPG[ucLoop].bLegStatus = SET;
					#ifdef	STALL_DBG
						DEBUG_PRINTF("Leg %d pass\r\n", ucLoop + 1);
					#endif
				}
				else
				{
					#ifdef	STALL_DBG
						DEBUG_PRINTF("Leg %d fail\r\n", ucLoop + 1);
					#endif

					stPG[ucLoop].bLegStatus = RESET;
					ucFaultByte |= 0x01 << ucLoop; 
				}
			}else if( 1 == ucLoop )
			{
				stPG[ucLoop].iPercentage = (int16_t)(((fCurrentPG2 - 
								fPreviousPG2) / fPreviousPG2) * 100);
				
				/*Reading pressure gauge current value*/	
			if((1 < fPreviousPG2 ) && ( 1 < fCurrentPG2 ) && 
				 ((iStallNegativeTh < stPG[ucLoop].iPercentage) && 
					(iStallPositiveTh > stPG[ucLoop].iPercentage)))
				{
					stPG[ucLoop].bLegStatus = SET;
					#ifdef	STALL_DBG
						DEBUG_PRINTF("Leg %d pass\r\n", ucLoop + 1);
					#endif
				}
				else
				{
					#ifdef	STALL_DBG
						DEBUG_PRINTF("Leg %d fail\r\n", ucLoop + 1);
					#endif

					stPG[ucLoop].bLegStatus = RESET;
					ucFaultByte |= 0x01 << ucLoop; 
				}
			}else if( 2 == ucLoop )
			{
				stPG[ucLoop].iPercentage = (int16_t)(((fCurrentPG3 - 
								fPreviousPG3) / fPreviousPG3) * 100);
				/*Reading pressure gauge current value*/	
			if((1 < fPreviousPG3 ) && ( 1 < fCurrentPG3 ) && 
				 ((iStallNegativeTh < stPG[ucLoop].iPercentage) && 
					(iStallPositiveTh > stPG[ucLoop].iPercentage)))
				{
					stPG[ucLoop].bLegStatus = SET;
					#ifdef	STALL_DBG
						DEBUG_PRINTF("Leg %d pass\r\n", ucLoop + 1);
					#endif
				}
				else
				{
					#ifdef	STALL_DBG
						DEBUG_PRINTF("Leg %d fail\r\n", ucLoop + 1);
					#endif

					stPG[ucLoop].bLegStatus = RESET;
					ucFaultByte |= 0x01 << ucLoop; 
				}
			}
			#ifdef	STALL_DBG
				DEBUG_PRINTF("iPercentage1 :%d \r\n", stPG[ucLoop].iPercentage);
			#endif
			
			
			SplitByte.iData = stPG[ucLoop].iPercentage;
			stTxMsg.Data[(2 * ucLoop) + 1] = SplitByte.rgcData[0];
			stTxMsg.Data[(2 * ucLoop) + 2] = SplitByte.rgcData[1];
		}
		stStallFlag.bCompareLegs 						= RESET;
		stStallFlag.bTBeforeStallPresCompr 	= RESET;	
		stStallFlag.bUpStrokeLegCmpr 				= RESET;
		stStallFlag.bDownStrokeLegCmpr 			= RESET;
		uiTBeforeStallPresCompr = 0;

		/*Send Stall test Data to MCU and OP*/
		stTxMsg.DLC = 7;
		if( DOWNSTROKE == ucStrokeType)
		{
			stTxMsg.Data[0] = DOWNSTROKE;
		}
		else if(UPSTROKE == ucStrokeType)
		{
			stTxMsg.Data[0] = UPSTROKE;
		}

		fnStallData_Send();
		/*Send Status stall test for upstroke and downstroke*/
		if((RESET == stPG[PG1].bLegStatus) || (RESET == stPG[PG2].bLegStatus ) ||
			(RESET == stPG[PG3].bLegStatus ))
		{
			stTxMsg.DLC = 2;
			if(UPSTROKE == ucStrokeType )
			{
				/*ON Valve*/
				fnValve_ONOFF(STALL_VALVE, STALL_VALVE_OFF);
				
				stTxMsg.Data[0] |= UPSTROKE_FAIL;
//				#ifdef	STALL_DBG
					DEBUG_PRINTF("UpStroke fail\n\r");
//				#endif
				stStallFlag.bUpStrokeDone = SET;
			}
			if( DOWNSTROKE == ucStrokeType )
			{
				/*ON Valve*/
				fnValve_ONOFF(STALL_VALVE, STALL_VALVE_OFF);
				
				stTxMsg.Data[0] = DOWNSTROKE_FAIL;
//				#ifdef	STALL_DBG
					DEBUG_PRINTF("DownStroke fail\r\n");
//				#endif
				stStallFlag.bDownStrokeDone = SET;
			}
			stTxMsg.Data[1] = ucFaultByte;
			
			fnStatus_Send(STATUS_STALL_TEST_RESULT);
			
		//	StatusFlag.stStatusFlag.bStallTestStart = RESET;
			stPG[PG1].bLegStatus 								= RESET;
			stPG[PG2].bLegStatus 								= RESET;
			stPG[PG3].bLegStatus 								= RESET;	
			stStallFlag.bTBeforeStall 					= RESET;
			stStallFlag.bTBeforeStallPresCompr 	= RESET;	
			stStallFlag.bPumpOnDownStroke 			= RESET;
			stStallFlag.bPumpOnUpStroke 				= RESET;
			stStallFlag.bCompareLegs 						= RESET;
		//	stStallFlag.bDownStrokeDone 				= SET;
		//	stStallFlag.bStartDownStroke 				= RESET;
		//	stStallFlag.bStartUpStroke 					= RESET;
			stStallFlag.bUpStrokeLegCmpr 				= RESET;
			stStallFlag.bDownStrokeLegCmpr 			= RESET;
			
			ucStrokeType = NONE;
		}
		else
		{
			if(DOWNSTROKE == ucStrokeType)
			{
				stStallFlag.bDownStrokeDone = SET;
				stStallFlag.bDownStrokePass = SET;
//				#ifdef	STALL_DBG
					DEBUG_PRINTF("DownStroke pass\r\n");
//				#endif
			}
			
			if(UPSTROKE == ucStrokeType )
			{
				stStallFlag.bUpStrokeDone = SET;
				stStallFlag.bUpStrokePass = SET;
//				#ifdef	STALL_DBG
					DEBUG_PRINTF("UpStroke pass\r\n");
//				#endif
			}
		}			
	}
}

///******************************************************************************
// **@Function 			: fnStall_Logic
// **@Descriptions	: Stall test logic implemented in STEP 3 according 
//										to flow chart
// **@Parameters		: None
// **@Return				: None
// ****************************************************************************/
//void fnStall_Logic(void)
//{
//	uint8_t ucLoop = 0;
//	uint8_t ucFaultByte = 0x00;
//	#ifdef STALL_DBG
//		char rgcTemp[500];
//	#endif
//	/*Timing before comparing stall pressures*/
//	if(uiTBeforeStallPresCompr > fTimBfrCompStallPressure)
//	{
//		fnPGSensor_Read(STALL_CALL);
//			
//		/*OFF Valve*/
//		fnValve_ONOFF(STALL_VALVE, STALL_VALVE_OFF);
//		
//		for(ucLoop = 0; ucLoop < NUM_OF_PG; ucLoop++)
//		{
//			if( 0 == ucLoop )
//			{
//				stPG[ucLoop].iPercentage = (int16_t)(((fCurrentPG1 - 
//								fPreviousPG1) / fPreviousPG1) * 100);
//				
//				/*Reading pressure gauge current value*/	
//			if((1 < fPreviousPG1 ) && ( 1 < fCurrentPG1) && 
//				 ((iStallNegativeTh < stPG[ucLoop].iPercentage) && 
//					(iStallPositiveTh > stPG[ucLoop].iPercentage)))
//				{
//					stPG[ucLoop].bLegStatus = SET;
//					#ifdef	STALL_DBG
//						DEBUG_PRINTF("Leg %d pass\r\n", ucLoop + 1);
//					#endif
//				}
//				else
//				{
//					#ifdef	STALL_DBG
//						DEBUG_PRINTF("Leg %d fail\r\n", ucLoop + 1);
//					#endif

//					stPG[ucLoop].bLegStatus = RESET;
//					ucFaultByte |= 0x01 << ucLoop; 
//				}
//			}else if( 1 == ucLoop )
//			{
//				stPG[ucLoop].iPercentage = (int16_t)(((fCurrentPG2 - 
//								fPreviousPG2) / fPreviousPG2) * 100);
//				
//				/*Reading pressure gauge current value*/	
//			if((1 < fPreviousPG2 ) && ( 1 < fCurrentPG2 ) && 
//				 ((iStallNegativeTh < stPG[ucLoop].iPercentage) && 
//					(iStallPositiveTh > stPG[ucLoop].iPercentage)))
//				{
//					stPG[ucLoop].bLegStatus = SET;
//					#ifdef	STALL_DBG
//						DEBUG_PRINTF("Leg %d pass\r\n", ucLoop + 1);
//					#endif
//				}
//				else
//				{
//					#ifdef	STALL_DBG
//						DEBUG_PRINTF("Leg %d fail\r\n", ucLoop + 1);
//					#endif

//					stPG[ucLoop].bLegStatus = RESET;
//					ucFaultByte |= 0x01 << ucLoop; 
//				}
//			}else if( 2 == ucLoop )
//			{
//				stPG[ucLoop].iPercentage = (int16_t)(((fCurrentPG3 - 
//								fPreviousPG3) / fPreviousPG3) * 100);
//				/*Reading pressure gauge current value*/	
//			if((1 < fPreviousPG3 ) && ( 1 < fCurrentPG3 ) && 
//				 ((iStallNegativeTh < stPG[ucLoop].iPercentage) && 
//					(iStallPositiveTh > stPG[ucLoop].iPercentage)))
//				{
//					stPG[ucLoop].bLegStatus = SET;
//					#ifdef	STALL_DBG
//						DEBUG_PRINTF("Leg %d pass\r\n", ucLoop + 1);
//					#endif
//				}
//				else
//				{
//					#ifdef	STALL_DBG
//						DEBUG_PRINTF("Leg %d fail\r\n", ucLoop + 1);
//					#endif

//					stPG[ucLoop].bLegStatus = RESET;
//					ucFaultByte |= 0x01 << ucLoop; 
//				}
//			}
//			#ifdef	STALL_DBG
//				DEBUG_PRINTF("iPercentage1 :%d \r\n", stPG[ucLoop].iPercentage);
//			#endif
//			
//			
//			SplitByte.iData = stPG[ucLoop].iPercentage;
//			stTxMsg.Data[(2 * ucLoop) + 1] = SplitByte.rgcData[0];
//			stTxMsg.Data[(2 * ucLoop) + 2] = SplitByte.rgcData[1];
//		}
//		stStallFlag.bCompareLegs 						= RESET;
//		stStallFlag.bTBeforeStallPresCompr 	= RESET;	
//		stStallFlag.bUpStrokeLegCmpr 				= RESET;
//		stStallFlag.bDownStrokeLegCmpr 			= RESET;
//		uiTBeforeStallPresCompr = 0;

//		/*Send Stall test Data to MCU and OP*/
//		stTxMsg.DLC = 7;
//		if( DOWNSTROKE == ucStrokeType)
//		{
//			stTxMsg.Data[0] = DOWNSTROKE;
//		}
//		else if(UPSTROKE == ucStrokeType)
//		{
//			stTxMsg.Data[0] = UPSTROKE;
//		}

//		fnStallData_Send();
//		/*Send Status stall test for upstroke and downstroke*/
//		if((RESET == stPG[PG1].bLegStatus) || (RESET == stPG[PG2].bLegStatus ) ||
//			(RESET == stPG[PG3].bLegStatus ))
//		{
//			stTxMsg.DLC = 2;
//			if(UPSTROKE == ucStrokeType )
//			{
//				/*ON Valve*/
//				fnValve_ONOFF(STALL_VALVE, STALL_VALVE_OFF);
//				
//				stTxMsg.Data[0] |= UPSTROKE_FAIL;
//				#ifdef	STALL_DBG
//					DEBUG_PRINTF("UpStroke fail\r\n");
//				#endif
//			}
//			if( DOWNSTROKE == ucStrokeType )
//			{
//				/*ON Valve*/
//				fnValve_ONOFF(STALL_VALVE, STALL_VALVE_OFF);
//				
//				stTxMsg.Data[0] = DOWNSTROKE_FAIL;
//				#ifdef	STALL_DBG
//					DEBUG_PRINTF("DownStroke fail\r\n");
//				#endif
//				stStallFlag.bDownStrokeDone = RESET;
//			}
//			stTxMsg.Data[1] = ucFaultByte;
//			
//			fnStatus_Send(STATUS_STALL_TEST_RESULT);
//			
//			StatusFlag.stStatusFlag.bStallTestStart = RESET;
//			stPG[PG1].bLegStatus 								= RESET;
//			stPG[PG2].bLegStatus 								= RESET;
//			stPG[PG3].bLegStatus 								= RESET;	
//			stStallFlag.bTBeforeStall 					= RESET;
//			stStallFlag.bTBeforeStallPresCompr 	= RESET;	
//			stStallFlag.bPumpOnDownStroke 			= RESET;
//			stStallFlag.bPumpOnUpStroke 				= RESET;
//			stStallFlag.bCompareLegs 						= RESET;
//			stStallFlag.bDownStrokeDone 				= RESET;
//			stStallFlag.bStartDownStroke 				= RESET;
//			stStallFlag.bStartUpStroke 					= RESET;
//			stStallFlag.bUpStrokeLegCmpr 				= RESET;
//			stStallFlag.bDownStrokeLegCmpr 			= RESET;
//			
//			ucStrokeType = NONE;
//		}
//		else
//		{
//			if(DOWNSTROKE == ucStrokeType)
//			{
//				stStallFlag.bDownStrokeDone = SET;
//				stStallFlag.bDownStrokePass = SET;
//				#ifdef	STALL_DBG
//					DEBUG_PRINTF("DownStroke pass\r\n");
//				#endif
//			}
//			
//			if(UPSTROKE == ucStrokeType )
//			{
//				
//				stStallFlag.bUpStrokePass = SET;
//				#ifdef	STALL_DBG
//					DEBUG_PRINTF("UpStroke pass\r\n");
//				#endif
//			}
//		}			
//	}
//}
/******************************************************************************
 **@Function 			: fnTypeOfStroke
 **@Descriptions	: function to find the type of stroke
 **@Parameters		: None
 **@Return				: type of stroke i.e Upstroke or downstroke. Until stroke
										type is not recognized it will return NONE.
 ****************************************************************************/
uint8_t fnTypeOfStroke(void)
{
	if(RESET == StatusFlag.stStatusFlag.bReadValue1 )
	{
		fValue1 = fnLinearPosition();
		StatusFlag.stStatusFlag.bReadValue1 = SET;
	}
	if(( STALL_COUNTER < uiStallCounter) && (RESET == StatusFlag.stStatusFlag.bReadValue2 ))
	{
		uiStallCounter = 0;
		fValue2 = fnLinearPosition();
		StatusFlag.stStatusFlag.bReadValue2 = SET;
	}
	if(SET == StatusFlag.stStatusFlag.bReadValue2 )
	{
		if(fValue1 < fValue2)
		{
			/*Upstroke*/
			if(fValue1 < ((float)ucUpstrokeLimit)) 
			{
				ucStokeType = UPSTROKE;
			}else
			{
				ucStokeType = NONE;
			}
		}
		if(fValue1 > fValue2)
		{
			/*Downstroke*/
			if(fValue1 > ((float)ucDownstrokeLimit))
			{
				ucStokeType = DOWNSTROKE;
			}else
			{
				ucStokeType = NONE;
			}
		}
		StatusFlag.stStatusFlag.bReadValue1 = RESET;
		StatusFlag.stStatusFlag.bReadValue2 = RESET;

		return ucStokeType;
	}
	return NONE;
}


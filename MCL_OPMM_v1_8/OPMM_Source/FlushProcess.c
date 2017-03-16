/******************************************************************************
**@file    FlushProcess.c
* 
**@brief   This file Contain Flush process functions.
******************************************************************************
*/    
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
/******************************************************************************
 **@Function   	: fnFlush_Process
 **@Description	: This function is for flush process
 **@Parameters	: None
 **@Return			: None
 ****************************************************************************/
void fnFlush_Process(void)
{
	uint8_t ucCnt;
	if(SET == StatusFlag.stStatusFlag.bFlushSelector)
  {
		/*Turn ON Spray Gun head valve*/
		fnValve_ONOFF(SPRAY_HEAD, VALVE_ON);
		
		if(uiGunPositionCounter < uiGunPosSwTime )
		{
			if(SET == StatusFlag.stStatusFlag.bAwayPipePosition)
			{
				DEBUG_PRINTF("===========================\n\r");
				uiGunPositionCounter = 0;
				
				StatusFlag.stStatusFlag.bFlushSelector = RESET;
				
				StatusFlag.stStatusFlag.bVbatFlushGunAwayFromPipe = SET;
					
			
				if((RESET == StatusFlag.stStatusFlag.bAutoFlushOnError) && 
					 (RESET == StatusFlag.stStatusFlag.bAutoFlushByTime))
				{
					/*Send Gun Position to SCU and OP*/
					stTxMsg.DLC = 1;
					stTxMsg.Data[0] = PRIME_POSITION;
					
					
					
					fnCANMessage_Send(STATUS_GUN_POSITION, MSG_TYPE_STATUS);
						#ifdef FLUSH_DBG
					DEBUG_PRINTF("Flush selector received and gun positoin\
												Status sent\r\n");
				#endif
				}
				
				/*Turn ON Flush/solvent Valves*/
				fnValve_ONOFF(AB_MAT, VALVE_OFF);
			
				
//				/*Turn ON Flush/solvent Valves*/
//				fnValve_ONOFF(FLUSH_MAT, VALVE_ON );
			
			}
		}
		else
    		{
			StatusFlag.stStatusFlag.bInitFlushProcess = RESET;
		  /* Send Spray gun motion error to OP and SCU if time 
				taken to reach the Flush position exceeds*/
			StatusFlag.stStatusFlag.bFlushFault = SET;
			uiGunPositionCounter = 0;
			StatusFlag.stStatusFlag.bFlushSelector = RESET;
			
			if((RESET == StatusFlag.stStatusFlag.bAutoFlushOnError) &&
				 (RESET == StatusFlag.stStatusFlag.bAutoFlushByTime))
			{
					memset(stTxMsg.Data, 0 , sizeof(stTxMsg.Data));
					stTxMsg.DLC = 0;
					
					/*send Fault on CAN*/
					fnCANMessage_Send(FAULT_SPRAY_GUN_MOTION, MSG_TYPE_FAULT);
			}
			GPIO_WriteBit(LIMIT_SW_STATUS_LED_PORT, GUN_AWAY_LSW_LED, LED_OFF);

			#ifdef FLUSH_DBG
				DEBUG_PRINTF("Flush selector received and Flush failed\r\n");
			#endif
		}
	}
	
	/*check for Automatic or Manual Flush command*/
	/*Manual*/
	if((SET == StatusFlag.stStatusFlag.bManualFlush) && /*Manual Flush Started*/
		 (SET == StatusFlag.stStatusFlag.bAwayPipePosition))
	{
		/*Turn ON Spray Gun Valves*/
		fnValve_ONOFF(SPRAY_GUN, VALVE_ON);

		/*Turn ON Flush/solvent Valves*//*added 2/3/2016*/
		fnValve_ONOFF(FLUSH_MAT, VALVE_ON );
		
		if((uiProcessTimeCount >= uiMaxFlushTime) || 
			 (SET == StatusFlag.stStatusFlag.bOneTimeAutoProcess))
		{
			if(uiProcessTimeCount >= uiMaxFlushTime)
			{
				StatusFlag.stStatusFlag.bTimeAchievStatusFlag = SET;
			}
			/* wait for Manual Flush Done command or the max time
  			to Manual Flush*/
			if((SET == StatusFlag.stStatusFlag.bManualFlushDone) ||
				 (MANUAL_FLUSH_MAX_TIME <= uiProcessTimeCount))
			{
				#ifdef FLUSH_DBG
					DEBUG_PRINTF("FLush Time : %d\r\n", uiProcessTimeCount);
				#endif
				StatusFlag.stStatusFlag.bOneTimeAutoProcess = SET;
				StatusFlag.stStatusFlag.bManualFlushDone = RESET;
				StatusFlag.stStatusFlag.bFlushDone = SET;	
				StatusFlag.stStatusFlag.bManualFlush = RESET;
			}
		}
	}
	
	
	
	if((SET == StatusFlag.stStatusFlag.bTimeAchievStatusFlag) && 
		 (RESET == StatusFlag.stStatusFlag.bPrimeTimeAchiveTx))
	{
		StatusFlag.stStatusFlag.bPrimeTimeAchiveTx = SET;
		#ifdef PRIME_DBG
			DEBUG_PRINTF("Flush Time achieved Pass\r\n");
		#endif
		/*These checks is use only when auto flush is not perform*/
		if(RESET == StatusFlag.stStatusFlag.bVbatSwichFlag)
		{
			if((RESET == StatusFlag.stStatusFlag.bAutoFlushOnError) && 
				 (RESET == StatusFlag.stStatusFlag.bAutoFlushByTime))
			{
				memset(stTxMsg.Data, 0 , sizeof(stTxMsg.Data));
				stTxMsg.DLC = 2;
				stTxMsg.Data[0] = FLUSH_TIME_ACHIEVED;
				stTxMsg.Data[1] = PRIME_FLUSH_TIME_ACHIEVED_PASS;	
				fnCANMessage_Send(STATUS_PRIME_TIME_ACHIEVED, MSG_TYPE_STATUS);
				
			}
		}
		if(RESET == StatusFlag.stStatusFlag.bFlushDone)
		{
			/*Buzzer ON when Flush Time Achieved*/
			BUZZER_ON();
		}
	}
	
	/*Automatic*/
	if(SET == StatusFlag.stStatusFlag.bAutoFlush)
	{
		/*Turn ON SPray Gun Valves*/
		fnValve_ONOFF(SPRAY_GUN, VALVE_ON);
		
		/*Turn ON Flush/solvent Valves*//*added 2/3/2016*/
		fnValve_ONOFF(FLUSH_MAT, VALVE_ON );
		
	  /*Wait for the Max Flush Time Count*/
		#ifdef FLUSH_DBG
	//		DEBUG_PRINTF("%d\r\n", uiProcessTimeCount);
	//		DEBUG_PRINTF("%d\r\n", uiMaxFlushTime);
		#endif
		if(uiProcessTimeCount >= uiMaxFlushTime)
		{
			StatusFlag.stStatusFlag.bFlushDone = SET;
			StatusFlag.stStatusFlag.bAutoFlush = RESET;
		}
	}
	
	if(SET == StatusFlag.stStatusFlag.bFlushDone)
	{
		BUZZER_OFF();
		#ifdef FLUSH_DBG
///			DEBUG_PRINTF("BUZZER_OFF bFlushDone\r\n");
		#endif
		if(SET == StatusFlag.stStatusFlag.bVbatSwichFlag)
		{
			StatusFlag.stStatusFlag.bInitFlushProcess = RESET;
			StatusFlag.stStatusFlag.bVBatFlushStartFlag = RESET;
		}
		StatusFlag.stStatusFlag.bFlushDone = RESET;
		/*To Stop the Timer counter for the FLUSH process */
		StatusFlag.stStatusFlag.bProcessTimeFlag = RESET; 
		StatusFlag.stStatusFlag.bVBatPrimeStartFlag = RESET; 
		StatusFlag.stStatusFlag.bVBatSprayStartFlag = RESET;
		
		/* To indicate Flush completed */
		StatusFlag.stStatusFlag.bVBatPrimeStartFlag = RESET; 
		StatusFlag.stStatusFlag.bVBatFlushStarted = RESET;
		
		/*Turn OFF SPray Gun Valves*/
		fnValve_ONOFF(SPRAY_GUN, VALVE_OFF);
		
		/*Turn ON Flush/solvent Valves*//*added 2/3/2016*/
		fnValve_ONOFF(FLUSH_MAT, VALVE_OFF );
		
		if(RESET == StatusFlag.stStatusFlag.bVbatSwichFlag)
		{			
//			if((RESET == StatusFlag.stStatusFlag.bAutoFlushOnError) && 
//				 (RESET == StatusFlag.stStatusFlag.bAutoFlushByTime))
//				{
					/**************** sending Status of Flush*************/
					memset(stTxMsg.Data, 0 , sizeof(stTxMsg.Data));
					stTxMsg.DLC = 1;
					stTxMsg.Data[0] = PROCESS_PASS;
				
					#ifdef FLUSH_DBG
						DEBUG_PRINTF("Flush process PASS!!\r\n");
					#endif
					/*send status on CAN*/
					fnCANMessage_Send(STATUS_FLUSH_PROCESS, MSG_TYPE_STATUS);
//				}
				
				
		  /******************* sending Data*****************/
//				if((RESET == StatusFlag.stStatusFlag.bAutoFlushOnError) &&
//					 (RESET == StatusFlag.stStatusFlag.bAutoFlushByTime))
//				{
					memset(stTxMsg.Data, 0 , sizeof(stTxMsg.Data));
					stTxMsg.DLC = 5;
					stTxMsg.Data[0] = FLUSH_PROCESS;
					SplitByte.fData = (float)(uiProcessTimeCount) / 
																		 TIME_CONVERT_FACTOR_10MS;
					for(ucCnt = 1; ucCnt < stTxMsg.DLC; ucCnt++)
					{
						stTxMsg.Data[ucCnt] =	SplitByte.rgucData[ucCnt - 1];
					}
					fnCANMessage_Send(DATA_PROCESS_TIME, MSG_TYPE_DATA);	
//				}
			}
				
				StatusFlag.stStatusFlag.bAutoFlushOnError = RESET;
				StatusFlag.stStatusFlag.bAutoFlushByTime = RESET;

				
			StatusFlag.stStatusFlag.bProcessTimeFlag = RESET;
			uiProcessTimeCount = 0;
			StatusFlag.stStatusFlag.bTimeAchievStatusFlag = RESET;
			StatusFlag.stStatusFlag.bPrimeTimeAchiveTx = RESET;

			/*19thFeb2016 -- reset this flag as auto or manual flush get done*/
			StatusFlag.stStatusFlag.bHLSReachedOnError = RESET;
			/*3rdFeb2017 - as auto flush get done*/
			StatusFlag.stStatusFlag.bMtoHomeAutoFlush = RESET;
			StatusFlag.stStatusFlag.bVbatFlushGunAwayFromPipe = RESET;
	}
	
	/*if any fault occures during the flush process while opening 
		or closing the valve */
	if(SET == StatusFlag.stStatusFlag.bFlushFault)
	{
		BUZZER_OFF();
		#ifdef FLUSH_DBG
//			DEBUG_PRINTF("BUZZER_OFF bFlushFault\r\n");
		#endif
		#ifdef FLUSH_DBG
			DEBUG_PRINTF("Flush process Fail\r\n");
		#endif
		if(SET == StatusFlag.stStatusFlag.bVbatSwichFlag)
		{
			StatusFlag.stStatusFlag.bInitFlushProcess = RESET;
			StatusFlag.stStatusFlag.bVBatFlushStartFlag = RESET;
		}
		
		StatusFlag.stStatusFlag.bVBatPrimeStartFlag = RESET; 
		StatusFlag.stStatusFlag.bVBatSprayStartFlag = RESET;
		StatusFlag.stStatusFlag.bVBatFlushStarted = RESET;
		StatusFlag.stStatusFlag.bManualFlush = RESET;
		StatusFlag.stStatusFlag.bAutoFlush = RESET;
		StatusFlag.stStatusFlag.bFlushFault = RESET;
		/*Turn OFF SPray Gun and Flush Valves*/
		fnValve_ONOFF(SPRAY_GUN, VALVE_OFF);
		fnValve_ONOFF(FLUSH_MAT, VALVE_OFF);
		
		
//		if((RESET == StatusFlag.stStatusFlag.bAutoFlushOnError) &&
//  		 (RESET == StatusFlag.stStatusFlag.bAutoFlushByTime))
//		{
			/*send the fault status of Flush process*/
			memset(stTxMsg.Data, 0 , sizeof(stTxMsg.Data));
			stTxMsg.DLC = 1;
			stTxMsg.Data[0] = PROCESS_FAIL;
			
			#ifdef FLUSH_DBG
				DEBUG_PRINTF("Flush process Fail!!\r\n");
			#endif
			/*send status on CAN*/
			fnCANMessage_Send(STATUS_FLUSH_PROCESS, MSG_TYPE_STATUS);
//		}
				
		if(SET == StatusFlag.stStatusFlag.bProcessTimeFlag)
		{
			/* To Stop the Timer counter for the FLUSH process */
			StatusFlag.stStatusFlag.bProcessTimeFlag = RESET;		 
			
//			if((RESET == StatusFlag.stStatusFlag.bAutoFlushOnError) &&
//				(RESET == StatusFlag.stStatusFlag.bAutoFlushByTime))
//				{
					memset(stTxMsg.Data, 0 , sizeof(stTxMsg.Data));
					stTxMsg.DLC = 5;
					stTxMsg.Data[0] = FLUSH_PROCESS;
					SplitByte.fData = (float)(uiProcessTimeCount) / TIME_CONVERT_FACTOR_10MS;
					for(ucCnt = 1; ucCnt < stTxMsg.DLC; ucCnt++)
					{
						stTxMsg.Data[ucCnt] =	SplitByte.rgucData[ucCnt - 1];
					}
					/*send Data on CAN to SCU and OP*/
					fnCANMessage_Send(DATA_PROCESS_TIME, MSG_TYPE_DATA);	
//				}
		}
		
		StatusFlag.stStatusFlag.bAutoFlushOnError = RESET;
		StatusFlag.stStatusFlag.bAutoFlushByTime = RESET;
		
		StatusFlag.stStatusFlag.bProcessTimeFlag = RESET;
				uiProcessTimeCount = 0;
		StatusFlag.stStatusFlag.bTimeAchievStatusFlag = RESET; 
		StatusFlag.stStatusFlag.bPrimeTimeAchiveTx = RESET;
		
		
		/*19thFeb2016 -- reset this flag as auto or manual flush get done or failed*/
			StatusFlag.stStatusFlag.bHLSReachedOnError = RESET;
		/*3rdFeb2017 -- reset this flag as auto or manual flush get done*/
			StatusFlag.stStatusFlag.bMtoHomeAutoFlush = RESET;
		StatusFlag.stStatusFlag.bVbatFlushGunAwayFromPipe = RESET;
	}
}

/******************************************************************************
 **@Function   	: fnAutoFlush
 **@Description	: This function is for Automatic Flush process
 **@Parameters	: None
 **@Return			: None
 ****************************************************************************/
void fnAutoFlush(void)
{
	/*if prime and spray process is initiated or started and Flush not done yet*/
	if((SET == StatusFlag.stStatusFlag.bVBatSprayStartFlag) || 
		 (SET == StatusFlag.stStatusFlag.bVBatPrimeStartFlag) || 
		 (SET == StatusFlag.stStatusFlag.bVBatFlushStartFlag))
	{
		if(RESET == StatusFlag.stStatusFlag.bVBatFlushStarted)
		{
			/*Turn OFF backward rotation valves*/
			fnValve_ONOFF(MBACKWARD, VALVE_OFF);
		
			/*Turn OFF SPray Gun Valves*/
			fnValve_ONOFF(SPRAY_GUN, VALVE_OFF);
			
			StatusFlag.stStatusFlag.bVBatFlushStarted = SET;
			StatusFlag.stStatusFlag.bHomePosition = RESET;
			StatusFlag.stStatusFlag.bInitFlushProcess = SET;
			StatusFlag.stStatusFlag.bFlushSelector = SET;
DEBUG_PRINTF("bAutoFlushOnError------Autoflush\n\r");

			StatusFlag.stStatusFlag.bAutoFlushOnError = SET;
			
			StatusFlag.stStatusFlag.bManualPrime = RESET;
			StatusFlag.stStatusFlag.bInitPrimeProcess = RESET;
			StatusFlag.stStatusFlag.bTimeAchievStatusFlag = RESET;
			StatusFlag.stStatusFlag.bPrimeTimeAchiveTx = RESET;
			StatusFlag.stStatusFlag.bManualPrime = RESET;
		}
		
		if((SET == StatusFlag.stStatusFlag.bVBatPrimeStartFlag) &&
			(RESET == StatusFlag.stStatusFlag.bVBatFlushStarted))
		{
			StatusFlag.stStatusFlag.bVBatFlushStarted = SET;
			
			StatusFlag.stStatusFlag.bInitFlushProcess = SET;
			StatusFlag.stStatusFlag.bFlushSelector = SET;
		}
		/* wait till Gun Away from the Pipe */
		if(SET == StatusFlag.stStatusFlag.bVbatFlushGunAwayFromPipe)
		{
			StatusFlag.stStatusFlag.bGunDelayCountFlag = SET;
			/* wait user to rach at gun with bucket to collect the material during
				 flush process*/
			if(uiGunDelayCnt >= uiSprayGunOpenTime)
			{
				uiGunDelayCnt = 0;
				uiProcessTimeCount = 0;	
				
				/*StatusFlag.stStatusFlag.bAutoFlushFlag = RESET;
				uiAutoFlushCnt = 0;*/
				
				StatusFlag.stStatusFlag.bProcessTimeFlag = SET;
				StatusFlag.stStatusFlag.bVbatFlushGunAwayFromPipe = RESET;
				StatusFlag.stStatusFlag.bAutoFlush = SET;
				
				StatusFlag.stStatusFlag.bProcessTimeFlag = SET;
				uiProcessTimeCount = 0;
				StatusFlag.stStatusFlag.bHLSReachedOnError = RESET;
				/*3rdFeb2017 -- reset this flag as auto or manual flush get done*/
			   StatusFlag.stStatusFlag.bMtoHomeAutoFlush = RESET;
				//DEBUG_PRINTF("RESET bHLSReachedOnError------Autoflush\n\r");
			}
		}					
	}
}

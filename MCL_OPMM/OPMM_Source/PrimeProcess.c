/******************************************************************************
**@file    PrimeProcess.c
* 
**@brief   Prime process functions.
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
uint16_t uiGunPosSwTime = 0;
uint16_t uiSprayGunOpenTime = 0;
uint16_t uiFlushWarnTime = 0;
uint16_t uiFlushWarnCnt = 0; /*adf 21/10/2016*/
uint16_t uiBypass = 0;
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
 **@Function 			: fnPrime_Process
 **@Description		: This function intilize and complete the prime process
 **@Parameters		: None
 **@Return				: None
 *****************************************************************************/
void fnPrime_Process(void)
{
	uint8_t ucCnt;
	if(SET == StatusFlag.stStatusFlag.bPrimeSelector)
	{
		/*Turn ON Spray Gun head valve*/
		fnValve_ONOFF(SPRAY_HEAD, VALVE_ON);
		/*Spray head switch Time*/
		if(uiGunPositionCounter < uiGunPosSwTime )
		{
			/*before the delay spray head switch time Gun Position 
				should be away from the Pipe*/
			if(SET == StatusFlag.stStatusFlag.bAwayPipePosition)
			{
				uiGunPositionCounter = 0;
				StatusFlag.stStatusFlag.bPrimeSelector = RESET;
				
				#ifdef PRIME_DBG
					DEBUG_PRINTF("Prime selector received and gun \
												 positoin command sent\r\n");
				#endif
				
				/*Send Gun Position to SCU and OP*/
				stTxMsg.DLC = 1;
				stTxMsg.Data[0] = PRIME_POSITION;
				fnCANMessage_Send(STATUS_GUN_POSITION, MSG_TYPE_STATUS);
				
				/* to indicate that Prime is initiated*/
	      StatusFlag.stStatusFlag.bVBatPrimeStartFlag = SET;
				
				/*Turn ON material Valves*/
				fnValve_ONOFF(AB_MAT, VALVE_ON);
			}
		}
		/*Spray head switch timer overlapse and Gun not reached to
			Away position then send Spray head switch fault*/
		else
		{
		  /*Send Spray gun motion error to HMI and SCU if time taken to reach
			  the prime position exceeds*/
			StatusFlag.stStatusFlag.bManualPrime = RESET;
			StatusFlag.stStatusFlag.bPrimeFault = SET;
			uiGunPositionCounter = 0;
			StatusFlag.stStatusFlag.bPrimeSelector = RESET;
			StatusFlag.stStatusFlag.bInitPrimeProcess = RESET;
			
			memset(stTxMsg.Data, 0 , sizeof(stTxMsg.Data));
			stTxMsg.DLC = 0;
			
			/*send status on CAN*/
			fnCANMessage_Send(FAULT_SPRAY_GUN_MOTION, MSG_TYPE_FAULT);
				
			#ifdef PRIME_DBG
				DEBUG_PRINTF("Prime selector received and prime failed\r\n");
			#endif
		}
	}
	
	/*Manual Prime Proccess*/
	if((SET == StatusFlag.stStatusFlag.bManualPrime) && /*Prime Start Command Rcv*/
		 (SET == StatusFlag.stStatusFlag.bAwayPipePosition)) /*AND Gun Away From Pipe*/
	{
		/*Turn ON Spray Gun Valves*/
		fnValve_ONOFF(SPRAY_GUN, VALVE_ON);
		
		if((uiProcessTimeCount >= uiMaxPrimeTime) || 
			 (SET == StatusFlag.stStatusFlag.bOneTimeAutoProcess))
		{
			if(uiProcessTimeCount >= uiMaxPrimeTime)
			{
				StatusFlag.stStatusFlag.bTimeAchievStatusFlag = SET;			
			}
			if((SET == StatusFlag.stStatusFlag.bManualPrimeDone) ||/*Prime Process Stop*/
				 (MANUAL_PRIME_MAX_TIME <= uiProcessTimeCount))/*OR Max Prime Time Overlape*/
			{
				#ifdef PRIME_DBG
					DEBUG_PRINTF("Prime Time : %d\r\n", uiProcessTimeCount);
				#endif
				/*Indicate One Time Prime Process Done*/				
				StatusFlag.stStatusFlag.bOneTimeAutoProcess = SET;
				StatusFlag.stStatusFlag.bManualPrimeDone = RESET;
				StatusFlag.stStatusFlag.bPrimeDone = SET;
				StatusFlag.stStatusFlag.bManualPrime = RESET;
			}
		}
	}
   	
	/*As Prime Time Achieved Status should be Sent to the SCU*/
	if((SET == StatusFlag.stStatusFlag.bTimeAchievStatusFlag) && 
		 (RESET == StatusFlag.stStatusFlag.bPrimeTimeAchiveTx))
	{
		StatusFlag.stStatusFlag.bPrimeTimeAchiveTx = SET;
		#ifdef PRIME_DBG
			DEBUG_PRINTF("Prime Time achieved Pass\r\n");
		#endif
		memset(stTxMsg.Data, 0 , sizeof(stTxMsg.Data));
		stTxMsg.DLC = 2;
		stTxMsg.Data[0] = PRIME_TIME_ACHIEVED;
		stTxMsg.Data[1] = PRIME_FLUSH_TIME_ACHIEVED_PASS;	
		fnCANMessage_Send(STATUS_PRIME_TIME_ACHIEVED, MSG_TYPE_STATUS);
		/*Buzzer ON for the Indication of Prime Time Achieved*/
		if(RESET == StatusFlag.stStatusFlag.bPrimeDone)
		{
			BUZZER_ON();
		}
	}

	/*Prime Process Done Successfully*/
	if(SET == StatusFlag.stStatusFlag.bPrimeDone)
	{
		BUZZER_OFF();
		#ifdef PRIME_DBG
			DEBUG_PRINTF("BUZZER_OFF bPrimeDone\r\n");
		#endif
		StatusFlag.stStatusFlag.bManualPrime = RESET;
		/*StatusFlag.stStatusFlag.bAutoFlushFlag = SET;
		uiAutoFlushCnt = 0;*/
		
		StatusFlag.stStatusFlag.bPrimeDone = RESET;
		/* To Stop the Timer counter for the PRIME process */
		StatusFlag.stStatusFlag.bProcessTimeFlag = RESET; 
				
		/*Turn OFF SPray Gun Valves*/
		fnValve_ONOFF(SPRAY_GUN, VALVE_OFF);
		
		/*Prime time Achieved status*/
		memset(stTxMsg.Data, 0 , sizeof(stTxMsg.Data));
		stTxMsg.DLC = 1;
		stTxMsg.Data[0] = PROCESS_PASS;

		#ifdef PRIME_DBG
		  DEBUG_PRINTF("Prime process status PASS!!!!\r\n");
		#endif
		/*send status on CAN*/
		fnCANMessage_Send(STATUS_PRIME_PROCESS, MSG_TYPE_STATUS);
		
		memset(stTxMsg.Data, 0 , sizeof(stTxMsg.Data));
		stTxMsg.DLC = 5;
		stTxMsg.Data[0] = PRIME_PROCESS;
		
		SplitByte.fData = fabs((float)(uiProcessTimeCount) /
														TIME_CONVERT_FACTOR_10MS);
		
		for(ucCnt = 1; ucCnt < stTxMsg.DLC; ucCnt++)
		{
			stTxMsg.Data[ucCnt] =	SplitByte.rgucData[ucCnt - 1];
		}
		/*Send the Prime Process time to SCU */
		fnCANMessage_Send(DATA_PROCESS_TIME, MSG_TYPE_DATA);	
		
		StatusFlag.stStatusFlag.bProcessTimeFlag = RESET;
		uiProcessTimeCount = 0;
		
		StatusFlag.stStatusFlag.bTimeAchievStatusFlag = RESET;
		StatusFlag.stStatusFlag.bPrimeTimeAchiveTx = RESET;
	}
	
	/*if any fault occures during the prime process while opening
		or closing the valve */
	if(SET == StatusFlag.stStatusFlag.bPrimeFault)
	{
		BUZZER_OFF();
		#ifdef PRIME_DBG
			DEBUG_PRINTF("BUZZER_OFF bPrimeFault\r\n");
		#endif
		StatusFlag.stStatusFlag.bManualPrime = RESET;

		/*StatusFlag.stStatusFlag.bAutoFlushFlag = SET;
		uiAutoFlushCnt = 0;*/
		
		StatusFlag.stStatusFlag.bPrimeFault = RESET;
		/*send the fault status of prime process*/
		stTxMsg.DLC = 1;
		stTxMsg.Data[0] = PROCESS_FAIL;

		#ifdef PRIME_DBG
		  DEBUG_PRINTF("Prime process status as FAIL!!!!\r\n");
		#endif
		
		/*Turn OFF SPray Gun Valves*/
		fnValve_ONOFF(SPRAY_GUN, VALVE_OFF);
				
		/*send status on CAN*/
		fnCANMessage_Send(STATUS_PRIME_PROCESS, MSG_TYPE_STATUS);
		
		if(SET == StatusFlag.stStatusFlag.bProcessTimeFlag)
		{
			/* To Stop the Timer counter for the PRIME process */
			StatusFlag.stStatusFlag.bProcessTimeFlag = RESET;			
					
			memset(stTxMsg.Data, 0 , sizeof(stTxMsg.Data));
			stTxMsg.DLC = 5;
			stTxMsg.Data[0] = PRIME_PROCESS;
			
			SplitByte.fData = (float)(uiProcessTimeCount) /
															TIME_CONVERT_FACTOR_10MS;
			
			for(ucCnt = 1; ucCnt < stTxMsg.DLC; ucCnt++)
			{
				stTxMsg.Data[ucCnt] =	SplitByte.rgucData[ucCnt - 1];
			}
			/* Send the Prime Process time to SCU and OP */
			fnCANMessage_Send(DATA_PROCESS_TIME, MSG_TYPE_DATA);
		}	
		
		StatusFlag.stStatusFlag.bTimeAchievStatusFlag = RESET; 
		StatusFlag.stStatusFlag.bPrimeTimeAchiveTx = RESET;
		uiProcessTimeCount = 0;
	}
}

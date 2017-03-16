/******************************************************************************
**@file    SprayProcess.c
* 
**@brief   Spray process functions.
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
uint16_t uiDCLSCounter 				= 0;
uint16_t uiHLSCounter					= 0;
uint16_t uiDCLSTime 					= 0;
uint16_t uiHLSTime 						= 0;
uint8_t ucRotationCounter			= 0;
uint16_t uiStopMotorRotTimer  = 0;
uint8_t ucCurrentLimitSwitch  = 0;

/*4thMarch*/
uint8_t ucRotationCntBackup 	= 0;
uint8_t ucDCLSHitCntToHome		= 0;
uint8_t ucHLSHitCntToHome		= 0;/*adf 23/02/2017*/
#ifdef GENERAL_DBG
	char rgucTemp[60];
#endif

uint8_t ucSprayFaultStatus;
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
 **@Function 		: fnSpray_Process
 **@Description	: This function intilize and complete the spray process
 **@Parameters	: None
 **@Return			: None
 ****************************************************************************/
void fnSpray_Process(void)
{
	#ifdef SPRAY_DBG
	char rgucTemp[200];
	#endif
	uint8_t ucCnt = 0;
		
	if(SET == StatusFlag.stStatusFlag.bSpraySelector)
	{
		StatusFlag.stStatusFlag.bSprayGoingOn = SET;
		
		/*to indicate that Spray is initiated for flush
   		process when Vbat enable*/
		StatusFlag.stStatusFlag.bVBatSprayStartFlag = SET;
		/*Turn Spray Gun head valve Towards Pipe(OFF the Valve head)*/
		fnValve_ONOFF(SPRAY_HEAD, VALVE_OFF);
			
		if(uiGunPositionCounter < uiGunPosSwTime )
		{
			if(SET == StatusFlag.stStatusFlag.bTowardPipePosition)
			{
				#ifdef SPRAY_DBG
					DEBUG_PRINTF("Spray Selector GUN TOWARDS PIPE\r\n");
				#endif
				
				uiGunPositionCounter = 0;
				StatusFlag.stStatusFlag.bSpraySelector = RESET;
				
				/*Send Gun Position to SCU and OP*/
				stTxMsg.DLC = 1;
				stTxMsg.Data[0] = SPRAY_POSITION;
				fnCANMessage_Send(STATUS_GUN_POSITION, MSG_TYPE_STATUS);
				
				/*Turn ON material Valves*/
				fnValve_ONOFF(AB_MAT, VALVE_ON);
								
				StatusFlag.stStatusFlag.bInitiateSpray = SET;
				StatusFlag.stStatusFlag.bForwardRotationInit = SET;
			}
		}
		else
		{
			#ifdef SPRAY_DBG
				DEBUG_PRINTF("Spray Selector FAULT\r\n");
			#endif
			StatusFlag.stStatusFlag.bInitSprayProcess = RESET;
			
			/*Send Spray gun motion error to OP and SCU if time taken 
				to reach the prime position exceeds*/
			StatusFlag.stStatusFlag.bSprayFault = SET;
			uiGunPositionCounter = 0;
			StatusFlag.stStatusFlag.bSpraySelector = RESET;
			
			memset(stTxMsg.Data, 0 , sizeof(stTxMsg.Data));
			stTxMsg.DLC = 0;
		
			/*send status on CAN*/
			fnCANMessage_Send(FAULT_SPRAY_GUN_MOTION, MSG_TYPE_FAULT);
			ucSprayFaultStatus |= SPRAY_GUN_MOTION_ERROR;
			StatusFlag.stStatusFlag.bInitiateSpray = SET;
			StatusFlag.stStatusFlag.bForwardRotationInit = SET;
		}
	}/*Spray Selector*/
	
	if(SET == StatusFlag.stStatusFlag.bStartSpray) /*SET In Pasrser*/
	{
		/*StatusFlag.stStatusFlag.bAutoFlushFlag = RESET;
		uiAutoFlushCnt = 0;*/
		 
		/*when head positioin towards pipe set, or when fail to reach towards Pipe, or when the rotion switch to forward*/
		if(SET == StatusFlag.stStatusFlag.bForwardRotationInit)
		{
			
			/*this flag set at 1. Gun position 2. fail to reach gun position towards pipe, 
				3. after the switching of the rotaiton i.e from forward to backward and vise versa*/
			if(SET == StatusFlag.stStatusFlag.bInitiateSpray)/*Initiating spraying loop*/
			{
				
				#ifdef SPRAY_DBG
					DEBUG_PRINTF("FORWARD SPRAY INIT PRESS SWITCH  3\r\n");
				#endif
				
				StatusFlag.stStatusFlag.bInitiateSpray = RESET;
				
				/*Turn OFF rotation Backward Valves*/
				fnValve_ONOFF(MBACKWARD, VALVE_OFF);
								
				/*Turn ON rotation forward Valves*/
				fnValve_ONOFF(MFORWARD, VALVE_ON);
								
				StatusFlag.stStatusFlag.bDCLSForward = SET;
				
				/* in First pass Flag set */
				if(0 == ucRotationCounter)
				{
					StatusFlag.stStatusFlag.bFirstRotationFlag = SET;
					StatusFlag.stStatusFlag.bNoLimitSwitchPressFlag = SET;
				}
			}
			
			/*Check for DCLS position*/		
			if((SET == StatusFlag.stStatusFlag.bDCLSPosition) && 
				 (SET == StatusFlag.stStatusFlag.bDCLSForward))
			{
				#ifdef SPRAY_DBG
					DEBUG_PRINTF("FORWARD DCLS REACHED 2\r\n");
				#endif
				
				StatusFlag.stStatusFlag.bDCLSForward = RESET;
				StatusFlag.stStatusFlag.bDCLSPosition = RESET;
				StatusFlag.stStatusFlag.bGunDelayTimerFlag = SET;
				
				StatusFlag.stStatusFlag.bNoLimitSwitchPressFlag = RESET;
				uiNoLimitSwitchCnt = 0;
			}
			
			/*check for Gun delay timer*/
			if(uiGunDelayCounter >= uiMaxGunDelayTime)
			{
				#ifdef SPRAY_DBG
					DEBUG_PRINTF("FORWARD GUN DELAY TIMER ACHIEVED 3\r\n");
				#endif
				uiGunDelayCounter = 0;
				StatusFlag.stStatusFlag.bGunDelayTimerFlag = RESET;
				StatusFlag.stStatusFlag.bGunDelayTimeAchieved = SET;
			}
			
			/*After Gun delay time Achieved*/
			if(SET == StatusFlag.stStatusFlag.bGunDelayTimeAchieved)
			{
				#ifdef SPRAY_DBG
					DEBUG_PRINTF("FORWARD ACTUAL SPRAYING STARTED \
												FORWARD-4-PRESS SWITCH 4\r\n");
				#endif
				StatusFlag.stStatusFlag.bGunDelayTimeAchieved = RESET;
				
				/*Turn ON Spray Gun Valve Valves*/
				fnValve_ONOFF(SPRAY_GUN, VALVE_ON);
				
				/*Changes for CRC Testing*/
				StatusFlag.stStatusFlag.bIgnoreHLSInit = SET;				
			}
			
			/*check for the Home limit Switch*/
			if(uiDCLSCounter < uiDCLSTime) 
			{
				if(SET == StatusFlag.stStatusFlag.bIgnoreHLSInit)
				{
					if(SET == StatusFlag.stStatusFlag.bHomePosition)
					{
						#ifdef SPRAY_DBG
							DEBUG_PRINTF("FORWARD HOME POSITION REACHED\
														5-PRESS SWITCH 3\r\n");
						#endif
						StatusFlag.stStatusFlag.bHomePosition = RESET;
						
						StatusFlag.stStatusFlag.bDCLSFlag = RESET;
						uiDCLSCounter = 0;
						
						StatusFlag.stStatusFlag.bHLSFlag = SET;					
						uiHLSCounter = 0;
						uiDclsToHomeFirstCnt = 0;
						StatusFlag.stStatusFlag.bFirstRotationFlag = RESET;
						
					}
					else if(0 == ucRotationCounter)
					{
						/* fail to reach Home in first pass */
						if(uiDclsToHomeFirstCnt > (uint16_t)
							(uiDCLSTime * TIME_PERCENT_FACTOR))
						{
							#ifdef SPRAY_DBG
								DEBUG_PRINTF("FAULT FLAG SET&\r\n");
							#endif
							ucSprayFaultStatus |= DCLS_HLS_REACH_FAULT;
							
							uiDclsToHomeFirstCnt = 0;
							StatusFlag.stStatusFlag.bFirstRotationFlag = RESET;
							StatusFlag.stStatusFlag.bSprayFault = SET;
							StatusFlag.stStatusFlag.bDCLSRotationFault = SET;
							
							#ifdef SPRAY_FAULT_DEBUG
								memset(stTxMsg.Data, 0 , sizeof(stTxMsg.Data));
								stTxMsg.DLC = 1;
								
								stTxMsg.Data[0] = 1;
								/*send status on CAN*/
								fnCANMessage_Send(FAULT_DEBUG, MSG_TYPE_FAULT);
							#endif
						}
					}
				}
			}
			else 
			{
				#ifdef SPRAY_DBG
					DEBUG_PRINTF("FAULT FLAG SET$\r\n");
				#endif
				StatusFlag.stStatusFlag.bSprayFault = SET;
				StatusFlag.stStatusFlag.bDCLSRotationFault = SET;
				ucSprayFaultStatus |= DCLS_HLS_REACH_FAULT;/*adf 21/10/2016*/
				#ifdef SPRAY_FAULT_DEBUG
					memset(stTxMsg.Data, 0 , sizeof(stTxMsg.Data));
					stTxMsg.DLC = 1;
					
					stTxMsg.Data[0] = 2;
					/*send status on CAN*/
					fnCANMessage_Send(FAULT_DEBUG, MSG_TYPE_FAULT);
				#endif
			}
			
			/*If NO Limit Switch get hit after starting 
				forward rotation -SET FAULT*/
			if(uiNoLimitSwitchCnt > (uint16_t)(uiDCLSTime * 0.2f))
			{
				#ifdef SPRAY_DBG
					DEBUG_PRINTF("FAULT FLAG SET&-\r\n");
				#endif
				uiNoLimitSwitchCnt = 0;
				StatusFlag.stStatusFlag.bNoLimitSwitchPressFlag = RESET;
				StatusFlag.stStatusFlag.bFirstRotationFlag = RESET;
				StatusFlag.stStatusFlag.bSprayFault = SET;
				StatusFlag.stStatusFlag.bDCLSRotationFault = SET;
				ucSprayFaultStatus |= NO_LMSW_DETECTED_FAULT;/*adf 21/10/2016*/
				#ifdef SPRAY_FAULT_DEBUG
					memset(stTxMsg.Data, 0 , sizeof(stTxMsg.Data));
					stTxMsg.DLC = 1;
					
					stTxMsg.Data[0] = 3;
					/*send status on CAN*/
					fnCANMessage_Send(FAULT_DEBUG, MSG_TYPE_FAULT);
				#endif
			}
			
			if(uiHLSCounter > uiHLSTime)
			{
				#ifdef SPRAY_DBG
					DEBUG_PRINTF("FORWARD HLS COUNTER FAULT OCCURS_+\r\n");
				#endif
				StatusFlag.stStatusFlag.bSprayFault = SET;
				StatusFlag.stStatusFlag.bHLSRotationFault = SET;
				ucSprayFaultStatus |= DCLS_HLS_REACH_FAULT;/*adf 21/10/2016*/
				#ifdef SPRAY_FAULT_DEBUG
					memset(stTxMsg.Data, 0 , sizeof(stTxMsg.Data));
					stTxMsg.DLC = 1;
					
					stTxMsg.Data[0] = 11;
					/*send status on CAN*/
					fnCANMessage_Send(FAULT_DEBUG, MSG_TYPE_FAULT);
				#endif
			}
						
			/*Check for DCLS limit switch position*/
			if((SET == StatusFlag.stStatusFlag.bHLSFlag) && 
				 (SET == StatusFlag.stStatusFlag.bDCLSPosition))
			{
				StatusFlag.stStatusFlag.bSprayOverlapTime = SET;
				if(uiSprayOverlapCounter >= uiSprayOverlapTime)
				{
					#ifdef SPRAY_DBG
						DEBUG_PRINTF("FORWARD DCLS POSITION REACHED-6\r\n");
					#endif
					uiSprayOverlapCounter = 0;
					StatusFlag.stStatusFlag.bDCLSPosition = RESET;
					
					StatusFlag.stStatusFlag.bSprayOverlapTime = RESET;
					
					#ifdef SPRAY_DBG
						DEBUG_PRINTF("FORWARD TURN OFF SPRAY GUN VALVE-7\r\n");
					#endif
					
					/*Turn OFF Spray Gun Valve Valves*/
					fnValve_ONOFF(SPRAY_GUN, VALVE_OFF);
									
					StatusFlag.stStatusFlag.bReverseDelayTime = SET;		
					//uiReverseDelayCounter = 0;	

					/*here we need to stop the forward motor rotation*//*29/2/2016*/
					/*7thMarch-For Testing Purpose added this part to overcome the sequence fault
						  as HLS and DCLS switches are much closer*/
						//if(ucRotationCounter != ucMaxSprayCount) 
						//{
							/*Turn OFF rotation Backward Valves*/
							fnValve_ONOFF(MFORWARD, VALVE_OFF);
						//}
				}
			}
			
			/*Check for the reverse delay timer*/
			if(uiReverseDelayCounter >= uiReverseDelayTime)
			{
				#ifdef SPRAY_DBG
					DEBUG_PRINTF("FORWARD REVERSE DELAY TIMER ACHIVED-8\r\n");
				#endif
				
				uiReverseDelayCounter = 0;
				StatusFlag.stStatusFlag.bReverseDelayTime = RESET;
				StatusFlag.stStatusFlag.bBackwardRotationInit = SET;
				StatusFlag.stStatusFlag.bForwardRotationInit = RESET;
				StatusFlag.stStatusFlag.bInitiateSpray = SET;
			}
		}
		
	  /*Initiate Backward Rotation*/
		if(SET == StatusFlag.stStatusFlag.bBackwardRotationInit)
		{
			
			if(uiDCLSCounter > uiDCLSTime) 
			{
				#ifdef SPRAY_DBG
					DEBUG_PRINTF("BACKWARD DCLS COUNTER FAULT FLAG SET-%\r\n");
				#endif
				StatusFlag.stStatusFlag.bSprayFault = SET;
				StatusFlag.stStatusFlag.bDCLSRotationFault = SET;
				ucSprayFaultStatus |= DCLS_HLS_REACH_FAULT;/*adf 21/10/2016*/
				
				#ifdef SPRAY_FAULT_DEBUG
					memset(stTxMsg.Data, 0 , sizeof(stTxMsg.Data));
					stTxMsg.DLC = 1;
					
					stTxMsg.Data[0] = 12;
					/*send status on CAN*/
					fnCANMessage_Send(FAULT_DEBUG, MSG_TYPE_FAULT);
				#endif
			}
			
			if(SET == StatusFlag.stStatusFlag.bInitiateSpray)
			{
				#ifdef SPRAY_DBG
					DEBUG_PRINTF("BACKWARD ROTATION INIT 9 PRESS SWITCH 3\r\n");
				#endif
				
				StatusFlag.stStatusFlag.bInitiateSpray = RESET;
				/*Stop forward rotation*/
				/*Turn OFF rotation forward Valves*/
				fnValve_ONOFF(MFORWARD, VALVE_OFF);
				
				/*Start Backward rotation*/
				/*Turn ON rotation Backward Valves*/
				fnValve_ONOFF(MBACKWARD, VALVE_ON);
				
				StatusFlag.stStatusFlag.bDCLSBackward = SET;
				
			}
			
			/*Check for the DClS Limit Switch*/
			if((SET == StatusFlag.stStatusFlag.bDCLSPosition) && 
				 (SET == StatusFlag.stStatusFlag.bDCLSBackward))
			{
				StatusFlag.stStatusFlag.bDCLSPosition = RESET;
				StatusFlag.stStatusFlag.bDCLSBackward = RESET;
				
				/*Set flag for Gun delay timer*/
				StatusFlag.stStatusFlag.bGunDelayTimerFlag = SET;
			}
			
			/*check for Gun delay timer*/
			if(uiGunDelayCounter >= uiMaxGunDelayTime)
			{
				#ifdef SPRAY_DBG
					DEBUG_PRINTF("BACKWARD GUN DELAY TIMER ACHIEVED-10\r\n");
				#endif
				
				uiGunDelayCounter = 0;
				StatusFlag.stStatusFlag.bGunDelayTimerFlag = RESET;
				StatusFlag.stStatusFlag.bGunDelayTimeAchieved = SET;
			}
			
			/*After Gun delay time Achieved*/
			if(SET == StatusFlag.stStatusFlag.bGunDelayTimeAchieved)
			{
				#ifdef SPRAY_DBG
					DEBUG_PRINTF("BACKWARD SPRAY STARTED 11 PRESS SWITCH 4\r\n");
				#endif
				
				StatusFlag.stStatusFlag.bGunDelayTimeAchieved = RESET;
				
				/*Turn ON Spray Gun Valve Valves*/
				fnValve_ONOFF(SPRAY_GUN, VALVE_ON);
				
			}
			
			/*check for the Home limit Switch*/
			if(uiHLSCounter < uiHLSTime)
			{
				if(SET == StatusFlag.stStatusFlag.bHomePosition)
				{
					#ifdef SPRAY_DBG
						DEBUG_PRINTF("BACKWARD HOME POS REACHED 12 PRESS SW 3\r\n");
					#endif
					
					StatusFlag.stStatusFlag.bHomePosition = RESET;
					
					StatusFlag.stStatusFlag.bHLSFlag = RESET;
					uiHLSCounter = 0;
					
					StatusFlag.stStatusFlag.bDCLSFlag = SET;
					uiDCLSCounter = 0;
				}
			}
			else
			{
				#ifdef SPRAY_DBG
					DEBUG_PRINTF("HLS FAULT OCCURS_*\r\n");
				#endif
				StatusFlag.stStatusFlag.bSprayFault = SET;
				StatusFlag.stStatusFlag.bHLSRotationFault = SET;
				ucSprayFaultStatus |= DCLS_HLS_REACH_FAULT;/*adf 21/10/2016*/
				#ifdef SPRAY_FAULT_DEBUG
					memset(stTxMsg.Data, 0 , sizeof(stTxMsg.Data));
					stTxMsg.DLC = 1;
					
					stTxMsg.Data[0] = 4;
					/*send status on CAN*/
					fnCANMessage_Send(FAULT_DEBUG, MSG_TYPE_FAULT);
				#endif
			}
			
			if(uiDCLSCounter < (uiDCLSTime / 2)) 
			{
				/*Check for DCLS position limit switch*/
				if(SET == StatusFlag.stStatusFlag.bDCLSPosition)
				{
					StatusFlag.stStatusFlag.bSprayOverlapTime = SET;
					if(uiSprayOverlapCounter >= uiSprayOverlapTime)
					{
						#ifdef SPRAY_DBG
							DEBUG_PRINTF("BACKWARD DCLS POSITION REACHED---\
												and SPRAY OVERLAP TIMER ACHIEVED---13\r\n");
						#endif
						
						uiSprayOverlapCounter = 0;
						StatusFlag.stStatusFlag.bDCLSPosition = RESET;
						
						StatusFlag.stStatusFlag.bSprayOverlapTime = RESET;
						
						/*Turn OFF Spray Gun Valve Valves*/
						fnValve_ONOFF(SPRAY_GUN, VALVE_OFF);				
								
						StatusFlag.stStatusFlag.bBackwardRotationInit = SET;
						StatusFlag.stStatusFlag.bForwardRotationInit = RESET;
						
						/*Changes done according to CRC testing*/
						/*Reverse delay timer starts if the rotation counter is achieved*/
						ucRotationCounter++;

						/*7thMarch-For Testing Purpose added this part to overcome the sequence fault
						  as HLS and DCLS switches are much closer*/
						if(ucRotationCounter != ucMaxSprayCount) 
						{
							/*Turn OFF rotation Backward Valves*/
							fnValve_ONOFF(MBACKWARD, VALVE_OFF);
						}
						
						#ifdef SPRAY_DBG
							sprintf(rgucTemp,"NUmber OF ROTATIONS COMPLETED : %d\r\n",
											ucRotationCounter);
							DEBUG_PRINTF("%s\r\n",rgucTemp);
						#endif
						
						memset(stTxMsg.Data, 0 , sizeof(stTxMsg.Data));
						stTxMsg.DLC = 1;
						stTxMsg.Data[0] = ucRotationCounter;
						fnCANMessage_Send(DATA_ROTATIONS_COMPLETED, MSG_TYPE_DATA);
						
						ucCurrentLimitSwitch = 1;
						if((ucRotationCounter == ucMaxSprayCount) ||
							 (SET == StatusFlag.stStatusFlag.bSystemErrorFlag ) ||
							 (SET == StatusFlag.stStatusFlag.bEXtraSprayCoat))
						{
							StatusFlag.stStatusFlag.bEXtraSprayCoat = RESET;
							
							ucRotationCounter = ucMaxSprayCount;
							ucCurrentLimitSwitch = 7;
														
							uiReverseDelayCounter = 0;
							StatusFlag.stStatusFlag.bReverseDelayTime = RESET;
							StatusFlag.stStatusFlag.bBackwardRotationInit = RESET;
							
							StatusFlag.stStatusFlag.bForwardRotationInit = RESET;
							StatusFlag.stStatusFlag.bInitiateSpray = RESET;
						}
						else
						{
							StatusFlag.stStatusFlag.bReverseDelayTime = SET;
						}
					}
				}
			}
			else
			{
				#ifdef SPRAY_DBG
						DEBUG_PRINTF("FAULT FLAG SET# in Backward Rotation\r\n");
				#endif
				StatusFlag.stStatusFlag.bSprayFault = SET;
				StatusFlag.stStatusFlag.bDCLSRotationFault = SET;
				ucSprayFaultStatus |= DCLS_HLS_REACH_FAULT;/*adf 21/10/2016*/
				#ifdef SPRAY_FAULT_DEBUG
					memset(stTxMsg.Data, 0 , sizeof(stTxMsg.Data));
					stTxMsg.DLC = 1;
					
					stTxMsg.Data[0] = 5;
					/*send status on CAN*/
					fnCANMessage_Send(FAULT_DEBUG, MSG_TYPE_FAULT);
				#endif
			}
			
			if(uiReverseDelayCounter >= uiReverseDelayTime)
			{
				#ifdef SPRAY_DBG
					DEBUG_PRINTF("BACKWARD REVERSE DELAY TIMER ACHIEVED-14\r\n");
				#endif
				
				uiReverseDelayCounter = 0;
				StatusFlag.stStatusFlag.bReverseDelayTime = RESET;
				StatusFlag.stStatusFlag.bBackwardRotationInit = RESET;
				StatusFlag.stStatusFlag.bForwardRotationInit = SET;
				StatusFlag.stStatusFlag.bInitiateSpray = SET;
			}
		}
		
		/*Rotation complete checks*/
		if(ucRotationCounter == ucMaxSprayCount)
		{
			/*Changes For CRC Testing*/
			StatusFlag.stStatusFlag.bIgnoreHLSInit = RESET;
			StatusFlag.stStatusFlag.bNoLimitSwitchPressFlag = RESET;
					
			StatusFlag.stStatusFlag.bBackwardRotationInit = RESET;
			StatusFlag.stStatusFlag.bForwardRotationInit = RESET;
			 /* To Stop the Timer counter for the Spray process */
			StatusFlag.stStatusFlag.bProcessTimeFlag = RESET;
			StatusFlag.stStatusFlag.bFirstRotationFlag = RESET;
			StatusFlag.stStatusFlag.bInitiateSpray = RESET;
			
			uiDclsToHomeFirstCnt = 0;
			#ifdef SPRAY_DBG
				//DEBUG_PRINTF("SPRAY PROCESS DONE_COUNT ACHIEVED PRESS SWITCH 4\r\n");
			#endif
			#ifdef SPRAY_DBG
					DEBUG_PRINTF("uiBackHlsCounter = %d, uiStopMotorRotTimer = %d \r\n",uiBackHlsCounter, uiStopMotorRotTimer);
			#endif
			/*Set flag for Back To HLS Timer*/
			StatusFlag.stStatusFlag.bBackHlsTimer = SET;
//			if(uiBackHlsCounter >= uiStopMotorRotTimer)
//			{
//				if(SET == StatusFlag.stStatusFlag.bHomePosition)
//				{
					/*StatusFlag.stStatusFlag.bAutoFlushFlag = SET;
					uiAutoFlushCnt = 0;*/
					#ifdef SPRAY_DBG
							DEBUG_PRINTF("uiBackHlsCounter = %d, uiStopMotorRotTimer = %d \r\n",uiBackHlsCounter, uiStopMotorRotTimer);
					#endif
					//uiBackHlsCounter = 0;
					StatusFlag.stStatusFlag.bStartSpray = RESET;
					StatusFlag.stStatusFlag.bInitSprayProcess = RESET;
					//StatusFlag.stStatusFlag.bBackHlsTimer = RESET;
					//StatusFlag.stStatusFlag.bHomePosition = RESET;
					
					StatusFlag.stStatusFlag.bHLSFlag = RESET;
					uiHLSCounter = 0;
					
					StatusFlag.stStatusFlag.bDCLSFlag = RESET;
					uiDCLSCounter = 0;
					
					ucRotationCounter = 0;
					
					/*Turn OFF material Valves and Backward rotation valve*/
					fnValve_ONOFF(AB_MAT, VALVE_OFF);
					//fnValve_ONOFF(MBACKWARD, VALVE_OFF);
										
//					/*Take Spray Head Away from Pipe*/
//					fnValve_ONOFF(SPRAY_HEAD, VALVE_ON);
										
//					if(SET == StatusFlag.stStatusFlag.bSystemErrorFlag)
//					{
//						StatusFlag.stStatusFlag.bHLSReachedOnError = SET;
//					}
					
					StatusFlag.stStatusFlag.bSprayGoingOn = RESET;
					
					if(RESET == StatusFlag.stStatusFlag.bSystemErrorFlag)
					{
						memset(stTxMsg.Data, 0 , sizeof(stTxMsg.Data));
						stTxMsg.DLC = 2;
						stTxMsg.Data[0] = PROCESS_PASS;/* Spray process Pass */
						stTxMsg.Data[1] = 0;
						fnCANMessage_Send(STATUS_SPRAY_PROCESS, MSG_TYPE_STATUS);
						ucSprayFaultStatus = 0;
						memset(stTxMsg.Data, 0 , sizeof(stTxMsg.Data));
						stTxMsg.DLC = 5;
						stTxMsg.Data[0] = SPRAY_PROCESS;
						SplitByte.fData = fabs((float)(uiProcessTimeCount) / 
																			TIME_CONVERT_FACTOR_10MS);
						for(ucCnt = 1; ucCnt < stTxMsg.DLC; ucCnt++)
						{
							stTxMsg.Data[ucCnt] =	SplitByte.rgucData[ucCnt - 1];
						}
						fnCANMessage_Send(DATA_PROCESS_TIME, MSG_TYPE_DATA);	

						#ifdef SPRAY_DBG
							DEBUG_PRINTF("SPRAY DONE\r\n");
						#endif
						
						StatusFlag.stStatusFlag.bProcessTimeFlag = RESET;
						uiProcessTimeCount = 0;
					}
					else
					{
						
						ucSprayFaultStatus |= SYSTEM_ERROR_OCCUR;
						//StatusFlag.stStatusFlag.bHLSReachedOnError = SET;
						/*send the fault status of spray process*/
						memset(stTxMsg.Data, 0 , sizeof(stTxMsg.Data));
						stTxMsg.DLC = 2;
						stTxMsg.Data[0] = PROCESS_FAIL;/* Spray process Fail */
						stTxMsg.Data[1] = ucSprayFaultStatus;
						fnCANMessage_Send(STATUS_SPRAY_PROCESS, MSG_TYPE_STATUS);
						ucSprayFaultStatus = 0;
						StatusFlag.stStatusFlag.bProcessTimeFlag = RESET;
						uiProcessTimeCount = 0;
						
						#ifdef SPRAY_FAULT_DEBUG
							memset(stTxMsg.Data, 0 , sizeof(stTxMsg.Data));
							stTxMsg.DLC = 1;
							
							stTxMsg.Data[0] = 6;
							/*send status on CAN*/
							fnCANMessage_Send(FAULT_DEBUG, MSG_TYPE_FAULT);
						#endif
					}
//				}
//			}
//			else
//			{
//				#ifdef SPRAY_DBG
//				DEBUG_PRINTF("FINAL DESTINATION FAULT OCCURS_\r\n");
//				#endif
//				StatusFlag.stStatusFlag.bSprayFault = SET;
//				StatusFlag.stStatusFlag.bHLSRotationFault = SET;
//				ucSprayFaultStatus |= DCLS_HLS_REACH_FAULT;/*adf 21/10/2016*/
//				uiBackHlsCounter = 0.0f;
//				StatusFlag.stStatusFlag.bBackHlsTimer = RESET;
//				
//				StatusFlag.stStatusFlag.bProcessTimeFlag = RESET;
//				uiProcessTimeCount = 0;
//				
//				#ifdef SPRAY_FAULT_DEBUG
//					memset(stTxMsg.Data, 0 , sizeof(stTxMsg.Data));
//					stTxMsg.DLC = 1;
//					
//					stTxMsg.Data[0] = 7;
//					/*send status on CAN*/
//					fnCANMessage_Send(FAULT_DEBUG, MSG_TYPE_FAULT);
//				#endif
//			}
			
		}
 	}
	
	/*if any fault occures during the spray process while opening 
		or closing the valve */
	if(SET == StatusFlag.stStatusFlag.bSprayFault)
	{
		StatusFlag.stStatusFlag.bSprayGoingOn = RESET;
		
		/*StatusFlag.stStatusFlag.bAutoFlushFlag = SET;
		uiAutoFlushCnt = 0;*/
		
		#ifdef SPRAY_DBG
			DEBUG_PRINTF("SPRAY FAULT OCCURS\r\n");
		#endif
		
		ucRotationCounter = 0;
		uiGunPositionCounter = 0;
		
		StatusFlag.stStatusFlag.bHLSFlag = RESET;
		uiHLSCounter = 0;
		
		StatusFlag.stStatusFlag.bDCLSFlag = RESET;
		uiDCLSCounter = 0;
		
		StatusFlag.stStatusFlag.bHomePosition = RESET;
		
		/*Changes For CRC Testing*/
		StatusFlag.stStatusFlag.bIgnoreHLSInit = RESET;
		
		StatusFlag.stStatusFlag.bInitSprayProcess = RESET;
		StatusFlag.stStatusFlag.bStartSpray = RESET;
		StatusFlag.stStatusFlag.bFirstRotationFlag = RESET;
		uiDclsToHomeFirstCnt = 0;
		
		StatusFlag.stStatusFlag.bBackwardRotationInit = RESET;
		StatusFlag.stStatusFlag.bForwardRotationInit = RESET;
		
		StatusFlag.stStatusFlag.bSprayFault = RESET;
		
		StatusFlag.stStatusFlag.bInitiateSpray = RESET;
		/*send the fault status of rotation fault*/
		if((SET == StatusFlag.stStatusFlag.bHLSRotationFault) || 
			 (SET == StatusFlag.stStatusFlag.bDCLSRotationFault))
		{
			memset(stTxMsg.Data, 0 , sizeof(stTxMsg.Data));
			stTxMsg.DLC = 1;
			if(SET == StatusFlag.stStatusFlag.bHLSRotationFault)
			{
				stTxMsg.Data[0] = HLS_TIMER;
			}
			else 
			{
				stTxMsg.Data[0] = DCLS_TIMER;
			}
			fnCANMessage_Send(FAULT_OPMM_ROTATION, MSG_TYPE_FAULT);
		}
		
		/*send the fault status of spray process*/
		memset(stTxMsg.Data, 0 , sizeof(stTxMsg.Data));
		stTxMsg.DLC = 2;
		stTxMsg.Data[0] = PROCESS_FAIL;/* Spray process Fail */
    stTxMsg.Data[1] = ucSprayFaultStatus;
		fnCANMessage_Send(STATUS_SPRAY_PROCESS, MSG_TYPE_STATUS);
		ucSprayFaultStatus = 0;
		if(SET == StatusFlag.stStatusFlag.bProcessTimeFlag)
		{
			/* To Stop the Timer counter for the SPRAY process */
			StatusFlag.stStatusFlag.bProcessTimeFlag = RESET;		
			memset(stTxMsg.Data, 0 , sizeof(stTxMsg.Data));
			stTxMsg.DLC = 5;
			stTxMsg.Data[0] = SPRAY_PROCESS;
			SplitByte.fData = (float)(uiProcessTimeCount) / TIME_CONVERT_FACTOR_10MS;
			
			for(ucCnt = 1; ucCnt < stTxMsg.DLC; ucCnt++)
			{
				stTxMsg.Data[ucCnt] =	SplitByte.rgucData[ucCnt - 1];
			}
			fnCANMessage_Send(DATA_PROCESS_TIME, MSG_TYPE_DATA);
		}
		
		/*Turn OFF SPray Gun Valves*/
		fnValve_ONOFF(SPRAY_GUN, VALVE_OFF);
		fnValve_ONOFF(SPRAY_HEAD, VALVE_OFF);
		fnValve_ONOFF(AB_MAT, VALVE_OFF);
		fnValve_ONOFF(MFORWARD, VALVE_OFF);
		fnValve_ONOFF(MBACKWARD, VALVE_OFF);
				
		if(RESET == StatusFlag.stStatusFlag.bStopSprayProcessbutton)
		{
			/*Take Spray Head Away from Pipe*/
			fnValve_ONOFF(SPRAY_HEAD, VALVE_ON);
		}	
		uiDCLSCounter = 0;
		uiHLSCounter = 0;
		ucRotationCounter = 0.0f;
		uiGunDelayCounter = 0;
		uiDclsToHomeFirstCnt = 0;
		uiSprayOverlapCounter = 0;
		uiReverseDelayCounter = 0;
		uiBackHlsCounter = 0.0f;
		uiProcessTimeCount = 0;

		StatusFlag.stStatusFlag.bInitiateSpray = RESET;
		StatusFlag.stStatusFlag.bDCLSForward = RESET;
		StatusFlag.stStatusFlag.bDCLSPosition = RESET;
		StatusFlag.stStatusFlag.bGunDelayTimerFlag = RESET;
		StatusFlag.stStatusFlag.bGunDelayTimeAchieved = RESET;
		StatusFlag.stStatusFlag.bHomePosition = RESET;
		StatusFlag.stStatusFlag.bDCLSFlag = RESET;
		StatusFlag.stStatusFlag.bFirstRotationFlag = RESET;
		StatusFlag.stStatusFlag.bSprayOverlapTime = RESET;
		StatusFlag.stStatusFlag.bReverseDelayTime = RESET;
		StatusFlag.stStatusFlag.bForwardRotationInit = RESET;
		StatusFlag.stStatusFlag.bDCLSBackward = RESET;
		StatusFlag.stStatusFlag.bHLSFlag = RESET;
		StatusFlag.stStatusFlag.bBackwardRotationInit = RESET;
		StatusFlag.stStatusFlag.bIgnoreHLSInit = RESET;
		StatusFlag.stStatusFlag.bProcessTimeFlag = RESET;
		StatusFlag.stStatusFlag.bStartSpray = RESET;
		StatusFlag.stStatusFlag.bInitSprayProcess = RESET;
		StatusFlag.stStatusFlag.bBackHlsTimer = RESET;
		StatusFlag.stStatusFlag.bNoLimitSwitchPressFlag = RESET;
		StatusFlag.stStatusFlag.bEXtraSprayCoat = RESET;
		StatusFlag.stStatusFlag.bFlushButtonStopSpray = RESET;
		StatusFlag.stStatusFlag.bStopSprayProcessbutton = RESET;
		
		#ifdef SPRAY_FAULT_DEBUG
			memset(stTxMsg.Data, 0 , sizeof(stTxMsg.Data));
			stTxMsg.DLC = 1;
			
			stTxMsg.Data[0] = 8;
			/*send status on CAN*/
			fnCANMessage_Send(FAULT_DEBUG, MSG_TYPE_FAULT);
		#endif
		
		if(SET == StatusFlag.stStatusFlag.bLSWSequenceErrorDCLS)
		{
			StatusFlag.stStatusFlag.bLSWSequenceErrorDCLS = RESET;
			stTxMsg.DLC = 2;
			stTxMsg.Data[0] = 0x01;
			stTxMsg.Data[1] = ucCurrentLimitSwitch;
			fnCANMessage_Send(FAULT_LSW_SEQ_MISSED, MSG_TYPE_FAULT);
		}
		else if(SET == StatusFlag.stStatusFlag.bLSWSequenceErrorHLS)
		{
			StatusFlag.stStatusFlag.bLSWSequenceErrorHLS = RESET;
			stTxMsg.DLC = 2;
			stTxMsg.Data[0] = 0x02;
			stTxMsg.Data[1] = ucCurrentLimitSwitch;
			fnCANMessage_Send(FAULT_LSW_SEQ_MISSED, MSG_TYPE_FAULT);
		}
	}
}



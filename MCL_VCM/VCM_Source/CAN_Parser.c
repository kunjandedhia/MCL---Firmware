/******************************************************************************
* @file    CanComm.c
* 
* @brief   This file contains CAN communication functions defination
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
void fnCANMsg_Process(CanRxMsg *pstCANrxmsg);

/******************************************************************************
 **@Function 			: fnCANMsg_Validate
 **@Descriptions	: Validate received can message as per the Device ID
 **@Parameters		:	pstCANrxmsg: pointer to a structure receive frame which
										contains CAN Id, CAN DLC, CAN data and FMI number.
 **@Return				: None
 ****************************************************************************/
void fnCANMsg_Validate(CanRxMsg *pstCANrxmsg)
{
  uint32_t uliCanID = pstCANrxmsg->ExtId;
	#ifdef DBG_CAN			 
		DEBUG_PRINTF("In fnCANMsg_Process \n\r");
	#endif	
  
	#ifdef DBG_CAN			 
		DEBUG_PRINTF("Rx CanID = 0x%x \n\r", uliCanID);
	#endif
  /************Destination Addr*************/
  switch  (uliCanID & DEST_ADDR_MASK)
  {
		case DEST_ALL:
    {
			#ifdef DBG_CAN      
				DEBUG_PRINTF("Broad Cast Message..\r\n");
			#endif	
      /*message is for MFM so precess it*/
      fnCANMsg_Process(pstCANrxmsg); 
    }
		break;

    case DEST_VCM:
		{
			#ifdef DBG_CAN      
				DEBUG_PRINTF("Msg for VCM..\r\n");
			#endif	
			/*message is for MFM so precess it*/
			fnCANMsg_Process(pstCANrxmsg); 
		}
		break;
		
		default:
    {   
			#ifdef DBG_CAN      
					DEBUG_PRINTF("Invalid Destination..\r\n");
			#endif
    }		
  }
}
/******************************************************************************
 **@Function 			: fnCANMsg_Process
 **@Descriptions	: Process received can message
 **@Parameters		: pstCANrxmsg: pointer to a structure receive frame which 
										contains CAN Id, CAN DLC, CAN data and FMI number.
 **@Return				: None
 ****************************************************************************/
void fnCANMsg_Process(CanRxMsg *pstCANrxmsg)
{
	#ifdef DBG_CAN
		char rgcTemp[200];
	#endif
	uint8_t ucCnt 		= 0;
  uint32_t uliCanID = pstCANrxmsg->ExtId;
  
	#ifdef DBG_CAN
		DEBUG_PRINTF("In fnCANMsg_Process..\r\n");
	#endif
	if((pstCANrxmsg->ExtId & ACKNOWLEDGEMENT) != ACKNOWLEDGEMENT)
	{
		fnAck_Send(pstCANrxmsg);
	}
  /************Type of message*************/
  switch (uliCanID & TYPE_MASK)
  {
		case MSG_TYPE_BROADCAST:
    {
			#ifdef DBG_CAN      
				DEBUG_PRINTF("Msg Type: MSG_TYPE_BROADCAST \r\n");
			#endif
      switch (uliCanID & MSG_MASK) 
      {
				case BCAST_ESTOP:
        {
					#ifdef DBG_CAN      
						DEBUG_PRINTF("Msg ID: CMD_ESTOP \r\n");
					#endif				
          StatusFlag.stStatusFlag.bEstop = SET;
        }
				break;
				case BCAST_HEARTBEAT:
        {
					#ifdef DBG_CAN      
						DEBUG_PRINTF("Msg ID: CMD_HEARTBEAT \r\n");
					#endif								
					StatusFlag.stStatusFlag.bHeartBeatMsgFlag = SET;
					uiHBTimeCounter = 0;
					/*4thAugust2016 - Android app and SCU get config para 
						requires more time than assumed*/
					StatusFlag.stStatusFlag.bCANFaultSCU = RESET;	
        }
				break;
				default:
				{
					#ifdef DBG_CAN      
						DEBUG_PRINTF("Invalid Command Msg Id \r\n");
					#endif					
				}				
      }
    }
		break;
		case MSG_TYPE_COMMAND:
    {
			#ifdef DBG_CAN      
				DEBUG_PRINTF("Msg Type: MSG_TYPE_COMMAND \r\n");
			#endif
      
      switch (uliCanID & MSG_MASK)
      {
				case CMD_STALL_TEST:
				{
					#ifdef CMD_DBG      
						DEBUG_PRINTF("Command to start Stall Test\r\n");
					#endif	
					StatusFlag.stStatusFlag.bStallTestStart = SET;
				}
				break;
				
				case CMD_STALL_VALVE:
        {
					
          if(OPERATION_MODE == ucMode)
          {
						#ifdef CMD_DBG      
							DEBUG_PRINTF("\r\n Turn ON/OFF Valve \r\n");
						#endif	
						ucCnt = 0;
						/*For Position */
						while(((pstCANrxmsg->Data[0] >> ucCnt) & 0x01) != 0x01)
						{
							ucCnt++;
						}
						#ifdef CMD_DBG
							DEBUG_PRINTF("CMD_STALL_VALVE %d\r\n", ucCnt);
						#endif
						fnValve_ONOFF(ucCnt, (BitAction)((pstCANrxmsg->Data[1] >> 1) & 0x01));
						
					}
        }
				break;
				case CMD_LINEAR_TRANS_MONITOR:
				{
					if(START == pstCANrxmsg->Data[0] )
					{
						#ifdef CMD_DBG
							DEBUG_PRINTF("Command to Start Monitoring Linear Transducer\r\n");
						#endif
						StatusFlag.stStatusFlag.bStartMonitorLT = SET;
						
						fLTFirstRead = fnLinearPosition();
					}
					else	if(STOP == pstCANrxmsg->Data[0])
					{
						fLTLastRead = fnLinearPosition();
						#ifdef CMD_DBG
							DEBUG_PRINTF("Command to Stop Monitoring Linear Transducer\r\n");
						#endif
						StatusFlag.stStatusFlag.bStartMonitorLT = RESET;
						StatusFlag.stStatusFlag.bStopMonitorLT = SET;
					}
        }
				break;
				case CMD_CLEAR_ERROR:
        {
					#ifdef CMD_DBG      
						DEBUG_PRINTF("Msg ID: Clear Fault Command received\r\n");
					#endif	
					fnAllFaults_Clear();
        }
				break;
				case CMD_CLEAR_WARNING:
        {
					#ifdef CMD_DBG      
						DEBUG_PRINTF("Msg ID: Clear Warning Command received\r\n");
					#endif	
					fnAllWarnnings_Clear();
					
        }
				break;
				case ACKNOWLEDGEMENT:
        {
					#ifdef CMD_DBG      
						DEBUG_PRINTF("Msg ID: ACK received\r\n");
					#endif	
          StatusFlag.stStatusFlag.bACK_Received = SET;
          
        }
				break;
				default:
				{
					#ifdef CMD_DBG      
						DEBUG_PRINTF("In Valid Cmd Msg\r\n");
					#endif	
				}
      }
    }
		break;
		case MSG_TYPE_FAULT:
    {
			#ifdef DBG_CAN      
				DEBUG_PRINTF("Msg Type: MSG_TYPE_FAULT \r\n");
			#endif			
      if(ACKNOWLEDGEMENT == (uliCanID & MSG_MASK))
      {
				#ifdef DBG_CAN      
					DEBUG_PRINTF("Msg ID: Fault_MSG_ACK received\r\n");
				#endif	
        StatusFlag.stStatusFlag.bACK_Received = SET;
      }
			else			
			{
				#ifdef DBG_CAN      
					DEBUG_PRINTF("Invalid Fault Msg Id\r\n");
				#endif	
			}
    }
		break;	
		case MSG_TYPE_STATUS:
    {
			#ifdef DBG_CAN      
				DEBUG_PRINTF("Msg Type: MSG_TYPE_STATUS \r\n");
			#endif			
      if(ACKNOWLEDGEMENT == (uliCanID & MSG_MASK))
      {
				#ifdef DBG_CAN      
					DEBUG_PRINTF("Msg ID: Status_MSG_ACK received\r\n");
				#endif	
				StatusFlag.stStatusFlag.bACK_Received = SET;
			}
			else if (STATUS_OPMM_VALVE == (uliCanID & MSG_MASK))
			{
				#ifdef REC_VAL_DBG
					DEBUG_PRINTF("REC_VALVE on Status Rx\r\n");
				#endif
				fnValve_ONOFF(RECIRCULATION_VALVE, (BitAction)((pstCANrxmsg->Data[0] >> 2) & 0x01));
			}
			else
			{
				#ifdef DBG_CAN      
					DEBUG_PRINTF("Invalid Status Msg Id\r\n");
				#endif	
			}
    }
		break;
		case MSG_TYPE_DATA:
    {
			#ifdef DBG_CAN      
				DEBUG_PRINTF("Msg Type: MSG_TYPE_DATA \r\n");
			#endif			
      if(ACKNOWLEDGEMENT == (uliCanID & MSG_MASK))
      {
     		#ifdef DBG_CAN      
					DEBUG_PRINTF("Msg ID: Data_MSG_ACK received\r\n");
				#endif	
        StatusFlag.stStatusFlag.bACK_Received = SET;
      }
			else 
			{
				#ifdef DBG_CAN      
					DEBUG_PRINTF("Invalid Data Msg Id\r\n");
				#endif	
			}
    }
		break;
		case MSG_TYPE_CONFIG:
    {
			#ifdef CONFIG_DBG      
				DEBUG_PRINTF("Msg Type: MSG_TYPE_CONFIG \r\n");
			#endif			
      switch (uliCanID & MSG_MASK)
      {
				case CONFIG_VCM_STALL_TH :
        {
					#ifdef CONFIG_DBG      
						DEBUG_PRINTF("Msg ID: Stall test threshold in %\r\n");
					#endif	         
					
					/* splitting the data received from the CAN */
					for(ucCnt = 1; ucCnt < pstCANrxmsg->DLC; ucCnt++)
					{
						SplitByte.rgcData[ucCnt - 1] = pstCANrxmsg->Data[ucCnt];
					}	
					if(DOWNSTROKE == pstCANrxmsg->Data[0])
					{
						iStallNegativeTh = (SplitByte.iData * (-1));
						/*Stall test Downstroke threshold received bit*/
						uliConfigParaByte |= CONFIG_PARA_STALL_TH_DOWN;
						
						#ifdef CONFIG_DBG  
							DEBUG_PRINTF("Stall threshold DOWN: %d\r\n",iStallNegativeTh);
						#endif
					}
					if(UPSTROKE == pstCANrxmsg->Data[0])
					{
						iStallPositiveTh = SplitByte.iData;
						/*Stall test Upstroke threshold received bit*/
						uliConfigParaByte |= CONFIG_PARA_STALL_TH_UP;
						#ifdef CONFIG_DBG  
							DEBUG_PRINTF("Stall threshold UP: %d\r\n",iStallPositiveTh);
						#endif
					}          
        }
				break;
				case CONFIG_VCM_PGAUGE1_TH :
        {
					#ifdef CONFIG_DBG      
						DEBUG_PRINTF("Pressure Gauge 1 threshold values\r\n");
					#endif	
          /* splitting the data received from the CAN */
					for(ucCnt = 1; ucCnt < pstCANrxmsg->DLC; ucCnt++)
					{
						SplitByte.rgucData[ucCnt - 1] = pstCANrxmsg->Data[ucCnt];
					}						
          
					if(LOW_THERSHOLD == pstCANrxmsg->Data[0])
					{		
						/*set Pressure gauge 1 Low threshold received bit*/						
						uliConfigParaByte |= CONFIG_PARA_PG1_LTH;
						fPG1_LowTH = SplitByte.fData;
						#ifdef CONFIG_DBG     
							memset(rgcTemp, '\0', sizeof(rgcTemp));
							sprintf(rgcTemp,"Msg ID: CONFIG_PGAUGE1 LOW_TH : %0.2f",
											fPG1_LowTH);	
							DEBUG_PRINTF("%s\r\n",rgcTemp);						
						#endif		
					}
					if(HIGH_THERSHOLD == pstCANrxmsg->Data[0])
					{							
						/*set Pressure gauge 1 High threshold received bit*/
						uliConfigParaByte |= CONFIG_PARA_PG1_HTH;
						fPG1_HighTH = SplitByte.fData;
						#ifdef CONFIG_DBG      
							memset(rgcTemp, '\0', sizeof(rgcTemp));
							sprintf(rgcTemp,"CONFIG PGAUGE1_TH HIGH : %0.2f\r\n",
											fPG1_HighTH);
							DEBUG_PRINTF("%s\r\n",rgcTemp);
						#endif		
					}
        }
				break;
				case CONFIG_VCM_PGAUGE2_TH :
        {
					#ifdef CONFIG_DBG      
						DEBUG_PRINTF("Pressure Gauge 2 threshold values\r\n");
					#endif	
          /* splitting the data received from the CAN */
					for(ucCnt = 1; ucCnt < pstCANrxmsg->DLC; ucCnt++)
					{
						SplitByte.rgucData[ucCnt - 1] = pstCANrxmsg->Data[ucCnt];
					}						
          
					if(LOW_THERSHOLD == pstCANrxmsg->Data[0])
					{			
						/*set Pressure gauge 2 Low threshold received bit*/
						uliConfigParaByte |= CONFIG_PARA_PG2_LTH;
						fPG2_LowTH = SplitByte.fData;
					#ifdef CONFIG_DBG      
							memset(rgcTemp, '\0', sizeof(rgcTemp));
							sprintf(rgcTemp,"CONFIG PGAUGE2_TH LOW: %0.2f",
											fPG2_LowTH);	
							DEBUG_PRINTF("%s\r\n",rgcTemp);	
						#endif		
					}
					if(HIGH_THERSHOLD == pstCANrxmsg->Data[0])
					{		
						/*set Pressure gauge 2 High threshold received bit*/
						uliConfigParaByte |= CONFIG_PARA_PG2_HTH;	
						fPG2_HighTH = SplitByte.fData;
						#ifdef CONFIG_DBG      
								memset(rgcTemp, '\0', sizeof(rgcTemp));
								sprintf(rgcTemp,"CONFIG PGAUGE2_TH HIGH : %0.2f \r\n",
								fPG2_HighTH);
								DEBUG_PRINTF("%s\r\n",rgcTemp);
						#endif		
					}          
        }
				break;
				case CONFIG_VCM_PGAUGE3_TH :
        {
					#ifdef CONFIG_DBG      
						DEBUG_PRINTF("Pressure Gauge 3 threshold values\r\n");
					#endif	
          /* splitting the data received from the CAN */
					for(ucCnt = 1; ucCnt < pstCANrxmsg->DLC; ucCnt++)
					{
						SplitByte.rgucData[ucCnt - 1] = pstCANrxmsg->Data[ucCnt];
					}						
          
					if(LOW_THERSHOLD == pstCANrxmsg->Data[0])
					{		
						uliConfigParaByte |= CONFIG_PARA_PG3_LTH;	/*set Pressure gauge 3 Low threshold received bit*/
						fPG3_LowTH = SplitByte.fData;
						#ifdef CONFIG_DBG      
							memset(rgcTemp, '\0', sizeof(rgcTemp));
							sprintf(rgcTemp,"CONFIG PGAUGE3_TH LOW : %0.2f",
											fPG3_LowTH);	
							DEBUG_PRINTF("%s\r\n",rgcTemp);
						#endif		
					}
					if(HIGH_THERSHOLD == pstCANrxmsg->Data[0])
					{
						uliConfigParaByte |= CONFIG_PARA_PG3_HTH;	/*set Pressure gauge 3 High threshold received bit*/
						fPG3_HighTH = SplitByte.fData;
						#ifdef CONFIG_DBG      
							memset(rgcTemp, '\0', sizeof(rgcTemp));
							sprintf(rgcTemp,"CONFIG PGAUGE3_TH HIGH : %0.2f \r\n",
											fPG3_HighTH);
							DEBUG_PRINTF("%s\r\n",rgcTemp);
						#endif		
					}          
        }
				break;
				case CONFIG_CYLINDER_SURFACE_AREA :
        {
					#ifdef CONFIG_DBG      
						DEBUG_PRINTF("Surface area of cylinder\r\n");
					#endif	
					/* splitting the data received from the CAN */
					for(ucCnt = 1; ucCnt < pstCANrxmsg->DLC; ucCnt++)
					{
						SplitByte.rgucData[ucCnt - 1] = pstCANrxmsg->Data[ucCnt];
					}
					if(CYLINDER_A == pstCANrxmsg->Data[0])
					{
						fCylinderAreaA = SplitByte.fData;
						#ifdef CONFIG_DBG      
							memset(rgcTemp, '\0', sizeof(rgcTemp));
							sprintf(rgcTemp,"CONFIG_CYLINDER_AREA A: %0.2f\r\n",
											fCylinderAreaA);
							DEBUG_PRINTF("%s\r\n",rgcTemp);
						#endif
						/*Surface area received bit*/
						uliConfigParaByte |= CONFIG_PARA_CSA_A;	
          }
					else if(CYLINDER_B == pstCANrxmsg->Data[0])
					{
						fCylinderAreaB = SplitByte.fData;
						#ifdef CONFIG_DBG      
							memset(rgcTemp, '\0', sizeof(rgcTemp));
							sprintf(rgcTemp,"CONFIG_CYLINDER_AREA B: %0.2f\r\n",
											fCylinderAreaB);
							DEBUG_PRINTF("%s\r\n",rgcTemp);
						#endif
						/*Surface area received bit*/
						uliConfigParaByte |= CONFIG_PARA_CSA_B;	
          }
					else if(CYLINDER_C == pstCANrxmsg->Data[0])
					{
						fCylinderAreaC = SplitByte.fData;
						#ifdef CONFIG_DBG      
							memset(rgcTemp, '\0', sizeof(rgcTemp));
							sprintf(rgcTemp,"Msg ID: CONFIG_CYLINDER_AREA C: %0.2f \r\n",
											fCylinderAreaC);
							DEBUG_PRINTF("%s\r\n",rgcTemp);
						#endif
						/*Surface area received bit*/
						uliConfigParaByte |= CONFIG_PARA_CSA_C;	
          }
        }
				break;
				case CONFIG_NUMBER_OF_CYLINDER :
        {
					ucCylinderCount = pstCANrxmsg->Data[0];
					#ifdef CONFIG_DBG      
						DEBUG_PRINTF("Number of Cylinders :%d\r\n",ucCylinderCount);
					#endif
					/*number of cylinders received bit*/
          uliConfigParaByte |= CONFIG_PARA_NBM_CYLINDERS;	
        }
				break;
				case CONFIG_TIME_BEFORE_STALL:
				{
					/* splitting the data received from the CAN */
					for(ucCnt = 0; ucCnt < pstCANrxmsg->DLC; ucCnt++)
					{
						SplitByte.rgucData[ucCnt] = pstCANrxmsg->Data[ucCnt];
					}
					fTimeBeforeStall = SplitByte.fData * 100;
					
					
					#ifdef CONFIG_DBG      
						memset(rgcTemp, '\0', sizeof(rgcTemp));
						sprintf(rgcTemp,"CONFIG_TIME_BEFORE_STALL: %0.2f\r\n", 	
						fTimeBeforeStall);
						DEBUG_PRINTF("%s\r\n",rgcTemp);
					#endif
					uliConfigParaByte |= CONFIG_PARA_TB_STALL;
				}
				break;
				case CONFIG_TIME_BEFORE_COMPARE_STALL:
				{
					/* splitting the data received from the CAN */
					for(ucCnt = 0; ucCnt < pstCANrxmsg->DLC; ucCnt++)
					{
						SplitByte.rgucData[ucCnt] = pstCANrxmsg->Data[ucCnt];
					}
					fTimBfrCompStallPressure = SplitByte.fData * 100;
					#ifdef CONFIG_DBG      
						memset(rgcTemp, '\0', sizeof(rgcTemp));
						sprintf(rgcTemp,"CONFIG_TIME_BEFORE_COMPARE_STALL: %0.2f\r\n",
										fTimBfrCompStallPressure);
						DEBUG_PRINTF("%s\r\n",rgcTemp);
					#endif
					uliConfigParaByte |= CONFIG_PARA_TB_COMPARE_STALL;
				}
				break;
				case CONFIG_PGAUGE_REMAP:
				{
					ucPositionPG1 = pstCANrxmsg->Data[0]	;
					#ifdef CONFIG_DBG  
						DEBUG_PRINTF("PG1 Position: %d\r\n",pstCANrxmsg->Data[0]);	
					#endif
				
					ucPositionPG2 = pstCANrxmsg->Data[1];
					#ifdef CONFIG_DBG  
						DEBUG_PRINTF("PG2 Position: %d\r\n",pstCANrxmsg->Data[1]);	
					#endif
				
					ucPositionPG3 = pstCANrxmsg->Data[2];
					#ifdef CONFIG_DBG  
						DEBUG_PRINTF("PG3 Position: %d\r\n",pstCANrxmsg->Data[2]);	
					#endif
					
					uliConfigParaByte |= CONFIG_PARA_PGAUGE_REMAP;
				}					
				break;
				case CONFIG_LT_REMAP:
				{
					ucLTPostion = pstCANrxmsg->Data[0];
					#ifdef CONFIG_DBG  
						DEBUG_PRINTF("CONFIG_LT_REMAP, Position : %d\r\n",
						pstCANrxmsg->Data[0]);	
					#endif
					uliConfigParaByte |= CONFIG_PARA_LT_REMAP;
				}					
				break;
				case CONFIG_EE07_REMAP:
				{
					ucInsideRemap = pstCANrxmsg->Data[0];
					#ifdef CONFIG_DBG  
						DEBUG_PRINTF("Inside EE07 REMAP, Pos: %d\r\n", ucInsideRemap);	
					#endif
				
					ucOutsideRemap = pstCANrxmsg->Data[1];
					#ifdef CONFIG_DBG  
						DEBUG_PRINTF("Outside EE07_REMAP, Pos: %d\r\n", ucOutsideRemap);	
					#endif
					uliConfigParaByte |= CONFIG_PARA_EE07_REMAP;
				}					
				break;
				case CONFIG_STALL_VALVE_REMAP:
				{
					stValve[STALL_VALVE].ucPosition = pstCANrxmsg->Data[0];
					stValve[RECIRCULATION_VALVE].ucPosition = pstCANrxmsg->Data[1];
					
					#ifdef CONFIG_DBG  
						DEBUG_PRINTF("CONFIG_STALL_VALVE_REMAP, Position : %d %d\r\n",
													stValve[STALL_VALVE].ucPosition,stValve[RECIRCULATION_VALVE].ucPosition);	
					#endif
					
					uliConfigParaByte |= CONFIG_PARA_STALL_VALVE_REMAP;
				}
				break;
				case CONFIG_VCM_PERIPHRAL_BYPASS:
				{
				
					ucBypass = pstCANrxmsg->Data[0];
					
			
					uliConfigParaByte |= CONFIG_PARA_VCM_PERIPH_BYPASS;
					#ifdef CONFIG_DBG  
						DEBUG_PRINTF("CONFIG_VCM_PERIPHRAL_BYPASS : %02x\r\n", ucBypass);	
					#endif
				}			
				break;
				case CONFIG_STALL_TEST_LT_RANGES:
				{
					#ifdef DBG_CAN      
					 DEBUG_PRINTF("CONFIG_STALL_TEST_LT_RANGES\r\n");
					#endif

					if(UPSTROKE_LIMIT == pstCANrxmsg->Data[0])
					{
						ucUpstrokeLimit = pstCANrxmsg->Data[1];
						#ifdef CONFIG_DBG
							DEBUG_PRINTF("ucUpstrokeLimit : %d\r\n", ucUpstrokeLimit);	
						#endif
					}
					else if(UPSTROKE_MIN == pstCANrxmsg->Data[0])
					{
						ucUpstrokeMin = pstCANrxmsg->Data[1];
						#ifdef CONFIG_DBG
							DEBUG_PRINTF("ucUpstrokeMin : %d\r\n", ucUpstrokeMin);
						#endif
					}
					else if(UPSTROKE_MAX == pstCANrxmsg->Data[0])
					{
						ucUpstrokeMax = pstCANrxmsg->Data[1];
						#ifdef CONFIG_DBG
							DEBUG_PRINTF("ucUpstrokeMax : %d\r\n", ucUpstrokeMax);
						#endif
					}
					else if(DOWNSTROKE_LIMIT == pstCANrxmsg->Data[0])
					{
						ucDownstrokeLimit = pstCANrxmsg->Data[1];
						#ifdef CONFIG_DBG
							DEBUG_PRINTF("ucDownstrokeLimit : %d\r\n", ucDownstrokeLimit);
						#endif
					}
					else if(DOWNSTROKE_MIN == pstCANrxmsg->Data[0])
					{
						ucDownstrokeMin = pstCANrxmsg->Data[1];
						#ifdef CONFIG_DBG
							DEBUG_PRINTF("ucDownstrokeMin : %d\r\n", ucDownstrokeMin);
						#endif
					}
					else if(DOWNSTROKE_MAX == pstCANrxmsg->Data[0])
					{
						ucDownstrokeMax = pstCANrxmsg->Data[1];
						#ifdef CONFIG_DBG
							DEBUG_PRINTF("ucDownstrokeMax : %d\r\n", ucDownstrokeMax);
						#endif
					}
				}
				break;
				default :
				{
					#ifdef CONFIG_DBG  
						DEBUG_PRINTF("Invalid Config Msg Id\r\n");	
					#endif
				}
      }
    }
		break;	
		default:
    {
			#ifdef CONFIG_DBG      
				DEBUG_PRINTF("Default Frame..\r\n");
			#endif	
    }		
  }			
}

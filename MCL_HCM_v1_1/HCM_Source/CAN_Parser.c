/******************************************************************************
* @file    CAN_Parser.c
* 
* @brief   This file contains CAN Parser Definition
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
/*******************************************************************************
 **@Function 			:  fnCANMsg_Validate
 **@Descriptions	:  Validate received can message as per the Device ID
 **@Parameters		:  pstCANrxmsg: Pointer to Received CAN Message
 **@Return				:  None
 *****************************************************************************/
void fnCANMsg_Validate(CanRxMsg *pstCANrxmsg)
{
	/*Destined HCM*/
	if(DEST_HCM == (pstCANrxmsg->ExtId & DEST_ADDR_MASK))	 
	{
		#ifdef DBG_CAN 
			DEBUG_PRINTF("Destined HCM\r\n");
		#endif
		/*message is for SCU so precess it*/
		fnCANMsg_Process(pstCANrxmsg);
	}	
	/*Broardcast message*/
	else if(DEST_ALL == (pstCANrxmsg->ExtId & DEST_ADDR_MASK)) 
	{
		#ifdef DBG_CAN  
			DEBUG_PRINTF("Broadcast Message\r\n");
		#endif
		/*message is for SCU so precess it*/
		fnCANMsg_Process(pstCANrxmsg);
	}
	else 
	{
		#ifdef DBG_CAN  
			DEBUG_PRINTF("Invalid Destination\r\n");
		#endif
	}
}
/*******************************************************************************
 **@Function 			:	 fnCANMsg_Process
 **@Descriptions	:  Process received can message
 **@Parameters		:	 pstCANrxmsg: Pointer to Received CAN Message
 **@Return				:  None
 *****************************************************************************/
void fnCANMsg_Process(CanRxMsg *pstCANrxmsg)
{
	uint8_t ucCount = 0;
	#if defined DBG_CAN || defined USART_DBG
		char rgcTemp[150];
	#endif
	
	uint32_t uliCanID = pstCANrxmsg->ExtId;
	#ifdef DBG_CAN      
		DEBUG_PRINTF("In fnCANMsg_Process..\r\n");
	#endif	
	/*send acknowledgement msg for all CAN msgs except Ack Msgs*/
	if(ACKNOWLEDGEMENT != (pstCANrxmsg->ExtId & MSG_MASK))
	{
		fnACK_Send(pstCANrxmsg);
	}
	/************MESSAGE TYPE*************/
	switch (uliCanID & TYPE_MASK)
	{
		/************BROADCAST MESSAGE*************/
		case MSG_TYPE_BROADCAST:
		{
			#ifdef DBG_CAN      
				DEBUG_PRINTF("MSG_TYPE_BROADCAST \r\n");
			#endif
			switch (uliCanID & MSG_MASK) 
			{
				case BCAST_ESTOP:
				{
					StatusFlag.stStatusFlag.bEStopFlag = SET;
					
					#ifdef DBG_CAN      
						DEBUG_PRINTF("BCAST_ESTOP \r\n");
					#endif
				}
				break;	
				case BCAST_HEARTBEAT:
				{
					uiHBTimeCounter = 0;/* Reset the Heartbeat Counter */
					StatusFlag.stStatusFlag.bHeartBeatMsgFlag = SET;
					#ifdef DBG_CAN      
						DEBUG_PRINTF("Broadcast HEARTBEAT Rx\r\n");
					#endif	
					/*4thAugust2016 - Android app and SCU get config para 
						requires more time than assumed*/
					StatusFlag.stStatusFlag.bCANFaultSCU = RESET;				
				}	
				break;
				default :
				{
					#ifdef DBG_CAN      
						DEBUG_PRINTF("Invalid Broadcast msg ID\r\n");
					#endif
				}
			}
		}
		break;
		/************COMMAND MESSAGE*************/
		case MSG_TYPE_COMMAND:
		{
			#ifdef DBG_CAN      
				DEBUG_PRINTF("MSG_TYPE_COMMAND\r\n");
			#endif
			switch(uliCanID & MSG_MASK)
			{
				case ACKNOWLEDGEMENT:
				{					
					StatusFlag.stStatusFlag.bACKRxFlag = SET;
					#ifdef DBG_CAN      
						DEBUG_PRINTF("ACKNOWLEDGE FOR COMMAND MESSAGE..\r\n");
					#endif	
				}
				break;
				case CMD_CLEAR_ERROR: /* Command from SCU for Clear all Faults */
				{
					fnClearAll_Faults();
					#ifdef DBG_CAN      
						DEBUG_PRINTF(" CMD_CLEAR_ERROR\r\n");
					#endif	
				}
				break;
				
				case CMD_CLEAR_WARNING: /* Command From SCU for Clear all Warnings */
				{
					fnClearAll_Warnings();
					#ifdef DBG_CAN      
						DEBUG_PRINTF(" CMD_CLEAR_WARNING\r\n");
					#endif			
				}
				break;
				
				case CMD_TURN_ON_OFF_HEATER: /* Command for Heater ON/OFF */
				{
					if(pstCANrxmsg->Data[0] & 0x0F)
					{
						ucCount = 0;
						/*For Position */
						while(((pstCANrxmsg->Data[0] >> ucCount) & 0x01) != 0x01)
						{
							ucCount++;
						}
						fnHeater_ONOFF(ucCount, 
						          ((pstCANrxmsg->Data[1] >> 1) & 0x01)? Bit_SET : Bit_RESET);
					}
				}
				break;
			}	
		}
		break;
		/************FAULT MESSAGE*************/
		case MSG_TYPE_FAULT:
		{
			#ifdef DBG_CAN      
				DEBUG_PRINTF("MSG_TYPE_FAULT \r\n");
			#endif			
			if(ACKNOWLEDGEMENT == (uliCanID & MSG_MASK))
			{
				StatusFlag.stStatusFlag.bACKRxFlag = SET;
				/******************************/
				#ifdef DBG_CAN      
					DEBUG_PRINTF("ACKNOWLEDGE FOR FAULT MESSAGE..\r\n");
				#endif	
			}
		}
		break;
		/************DATA MESSAGE*************/
		case MSG_TYPE_DATA:
		{
			#ifdef DBG_CAN      
				DEBUG_PRINTF("MSG_TYPE_DATA \r\n");
			#endif	
			if(ACKNOWLEDGEMENT == (uliCanID & MSG_MASK))
			{
				StatusFlag.stStatusFlag.bACKRxFlag = SET;
				/******************************/
				#ifdef DBG_CAN      
					DEBUG_PRINTF("ACKNOWLEDGE FOR DATA MESSAGE..\r\n");
				#endif	
			}
		}
		break;
		/************STATUS MESSAGE*************/	
		case MSG_TYPE_STATUS:
		{
			#ifdef DBG_CAN      
			DEBUG_PRINTF("MSG_TYPE_STATUS \r\n");
			#endif	
			if(ACKNOWLEDGEMENT == (uliCanID & MSG_MASK))
			{
				StatusFlag.stStatusFlag.bACKRxFlag = SET;
				/******************************/
				#ifdef DBG_CAN      
				DEBUG_PRINTF("ACKNOWLEDGE FOR STATUS MESSAGE..\r\n");
				#endif	
			}
		}
		break;
		/************CONFIG MESSAGE*************/		
		case MSG_TYPE_CONFIG:
		{
			#ifdef CONFIG_DBG      
			DEBUG_PRINTF("MSG_TYPE_CONFIG\r\n");
			#endif	
			switch (uliCanID & MSG_MASK)
			{
				case ACKNOWLEDGEMENT:
				{		
					StatusFlag.stStatusFlag.bACKRxFlag = SET;
					/******************************/					
					#ifdef CONFIG_DBG      
					DEBUG_PRINTF("ACKNOWLEDGE FOR CONFIG MESSAGE..\r\n");
					#endif	
				}
				break;	
				case CONFIG_BASE_TEMP_SETTING:
				{
					for(ucCount = 1; ucCount < pstCANrxmsg->DLC; ucCount++)
					{
						SplitByte.rgucData[ucCount - 1] = pstCANrxmsg->Data[ucCount];
					}
					
					if(TEMP_SET == pstCANrxmsg->Data[0])
					{
						stTC.stProcess[ANTIFREEZ].fSetTemp = SplitByte.fData;	
						#ifdef CONFIG_DBG      
							DEBUG_PRINTF("ANTIFREEZ  TEMP_SET\r\n");
						#endif							
						uliConfigParaByte |= CONFIG_BTEMP_SET;
					}
					else if(TEMP_LOW_THERSHOLD == pstCANrxmsg->Data[0])
					{
						stTC.stProcess[ANTIFREEZ].fTempLowTH = SplitByte.fData;
						#ifdef CONFIG_DBG      
							DEBUG_PRINTF("ANTIFREEZ  TEMP_LOW_THERSHOLD\r\n");
						#endif	
						uliConfigParaByte |= CONFIG_BTEMP_LOW_TH;						
					}
					else if(TEMP_HIGH_THERSHOLD == pstCANrxmsg->Data[0])
					{						
						stTC.stProcess[ANTIFREEZ].fTempHighTH = SplitByte.fData;
						#ifdef CONFIG_DBG      
							DEBUG_PRINTF("ANTIFREEZ  TEMP_HIGH_THERSHOLD\r\n");
						#endif	
						uliConfigParaByte |= CONFIG_BTEMP_HIGH_TH;												
					}
					else if(TEMP_HYS_POS == pstCANrxmsg->Data[0])
					{
						stTC.stProcess[ANTIFREEZ].fTempHystPos = SplitByte.fData;
						#ifdef CONFIG_DBG      
							DEBUG_PRINTF("ANTIFREEZ  TEMP_HYS_POS\r\n");
						#endif	
						uliConfigParaByte |= CONFIG_BTEMP_HYS_POS;
					}
					else if(TEMP_HYS_NEG == pstCANrxmsg->Data[0])
					{
						stTC.stProcess[ANTIFREEZ].fTempHystNeg = SplitByte.fData;
						#ifdef CONFIG_DBG      
							DEBUG_PRINTF("ANTIFREEZ  TEMP_HYS_NEG\r\n");
						#endif	
						uliConfigParaByte |= CONFIG_BTEMP_HYS_NEG;												
					}
					else
					{
						#ifdef CONFIG_DBG      
							DEBUG_PRINTF("INVALID CONFIG MSG\r\n");
						#endif
					}
					#ifdef CONFIG_DBG  
						sprintf(rgcTemp,"DATA %0.2f\r\n",SplitByte.fData);
						DEBUG_PRINTF("%s\r\n",rgcTemp);
					#endif	
				}
				break;	
				case CONFIG_INLINE_BASE_TEMP_SETTING:
				{
					for(ucCount = 1; ucCount < pstCANrxmsg->DLC; ucCount++)
					{
						SplitByte.rgucData[ucCount - 1] = pstCANrxmsg->Data[ucCount];
					}
					if(TEMP_SET == pstCANrxmsg->Data[0])
					{
						stTC.stProcess[INLINE_BASE].fSetTemp = SplitByte.fData;			
						#ifdef CONFIG_DBG      
							DEBUG_PRINTF("INLINE_BASE  TEMP_SET\r\n");
						#endif	
						uliConfigParaByte |= CONFIG_ILBTEMP_SET;
					}
					else if(TEMP_LOW_THERSHOLD == pstCANrxmsg->Data[0])
					{
						stTC.stProcess[INLINE_BASE].fTempLowTH = SplitByte.fData;
						#ifdef CONFIG_DBG      
							DEBUG_PRINTF("INLINE_BASE  TEMP_LOW_THERSHOLD\r\n");
						#endif	
						uliConfigParaByte |= CONFIG_ILBTEMP_LOW_TH;						
					}
					else if(TEMP_HIGH_THERSHOLD == pstCANrxmsg->Data[0])
					{
						stTC.stProcess[INLINE_BASE].fTempHighTH = SplitByte.fData;
						#ifdef CONFIG_DBG      
							DEBUG_PRINTF("INLINE_BASE  TEMP_HIGH_THERSHOLD\r\n");
						#endif
						uliConfigParaByte |= CONFIG_ILBTEMP_HIGH_TH;						
					}
					else if(TEMP_HYS_POS == pstCANrxmsg->Data[0])
					{
						stTC.stProcess[INLINE_BASE].fTempHystPos = SplitByte.fData;
						#ifdef CONFIG_DBG      
							DEBUG_PRINTF("INLINE_BASE  TEMP_HYS_POS\r\n");
						#endif
						uliConfigParaByte |= CONFIG_ILBTEMP_HYS_POS;
					}
					else if(TEMP_HYS_NEG == pstCANrxmsg->Data[0])
					{
						stTC.stProcess[INLINE_BASE].fTempHystNeg = SplitByte.fData;
						#ifdef CONFIG_DBG      
							DEBUG_PRINTF("INLINE_BASE  TEMP_HYS_NEG\r\n");
						#endif
						uliConfigParaByte |= CONFIG_ILBTEMP_HYS_NEG;
					}
					else
					{
						#ifdef CONFIG_DBG      
							DEBUG_PRINTF("INVALID CONFIG MSG\r\n");
						#endif
					}
					#ifdef CONFIG_DBG  
						sprintf(rgcTemp,"DATA %0.2f PARA: %d\r\n",SplitByte.fData, uliConfigParaByte);
						DEBUG_PRINTF("%s\r\n",rgcTemp);
					#endif	
				}
				break;	
				case CONFIG_INLINE_HARD_TEMP_SETTING:
				{
					for(ucCount = 1; ucCount < pstCANrxmsg->DLC; ucCount++)
					{
						SplitByte.rgucData[ucCount - 1] = pstCANrxmsg->Data[ucCount];
					}
					
					if(TEMP_SET == pstCANrxmsg->Data[0])
					{
						stTC.stProcess[INLINE_HARD].fSetTemp = SplitByte.fData;		
						#ifdef CONFIG_DBG      
							DEBUG_PRINTF("INLINE_HARD  TEMP_SET\r\n");
						#endif	
						uliConfigParaByte |= CONFIG_ILHTEMP_SET;						
					}
					else if(TEMP_LOW_THERSHOLD == pstCANrxmsg->Data[0])
					{
						stTC.stProcess[INLINE_HARD].fTempLowTH = SplitByte.fData;
						#ifdef CONFIG_DBG      
							DEBUG_PRINTF("INLINE_HARD TEMP_LOW_THERSHOLD\r\n");
						#endif
						uliConfigParaByte |= CONFIG_ILHTEMP_LOW_TH;
					}
					else if(TEMP_HIGH_THERSHOLD == pstCANrxmsg->Data[0])
					{
						stTC.stProcess[INLINE_HARD].fTempHighTH = SplitByte.fData;
						#ifdef CONFIG_DBG      
							DEBUG_PRINTF("INLINE_HARD TEMP_HIGH_THERSHOLD\r\n");
						#endif
						uliConfigParaByte |= CONFIG_ILHTEMP_HIGH_TH;
					}
					else if(TEMP_HYS_POS == pstCANrxmsg->Data[0])
					{
						stTC.stProcess[INLINE_HARD].fTempHystPos = SplitByte.fData;
						#ifdef CONFIG_DBG      
							DEBUG_PRINTF("INLINE_HARD TEMP_HYS_POS\r\n");
						#endif
						uliConfigParaByte |= CONFIG_ILHTEMP_HYS_POS;
					}
					else if(TEMP_HYS_NEG == pstCANrxmsg->Data[0])
					{
						stTC.stProcess[INLINE_HARD].fTempHystNeg = SplitByte.fData;
						#ifdef CONFIG_DBG      
							DEBUG_PRINTF("INLINE_HARD TEMP_HYS_NEG\r\n");
						#endif
						uliConfigParaByte |= CONFIG_ILHTEMP_HYS_NEG;
					}
					else
					{
						#ifdef CONFIG_DBG      
							DEBUG_PRINTF("INVALID CONFIG MSG\r\n");
						#endif
					}
					#ifdef CONFIG_DBG  
					sprintf(rgcTemp,"DATA %0.2f PARA :%d\r\n",SplitByte.fData,uliConfigParaByte);
						DEBUG_PRINTF("%s\r\n",rgcTemp);
					#endif	
				}
				break;	
				case CONFIG_BASE_LEVEL_SETTING:
				{
					for(ucCount = 1; ucCount < pstCANrxmsg->DLC; ucCount++)
					{
						SplitByte.rgucData[ucCount - 1] = pstCANrxmsg->Data[ucCount];
					}
					
					if(LOW_THERSHOLD == pstCANrxmsg->Data[0])
					{
						stLL[BASE].fLiquidLevelLowTH = SplitByte.fData;
						#ifdef CONFIG_DBG   
							DEBUG_PRINTF("BASE LL Low \r\n");
						#endif
						uliConfigParaByte |= CONFIG_LL_BASE_LOW_TH;	
					}
					else if(HIGH_THERSHOLD == pstCANrxmsg->Data[0])
					{
						stLL[BASE].fLiquidLevelHighTH = SplitByte.fData;
						#ifdef CONFIG_DBG   
						DEBUG_PRINTF("BASE LL High \r\n");
						#endif 
						uliConfigParaByte |= CONFIG_LL_BASE_HIGH_TH;
					}
					else
					{
						#ifdef CONFIG_DBG      
							DEBUG_PRINTF("INVALID MSG\r\n");
						#endif
					}
					#ifdef CONFIG_DBG  
						sprintf(rgcTemp,"Thershold %0.2f PARA: %d\r\n",SplitByte.fData,uliConfigParaByte);
						DEBUG_PRINTF("%s\r\n",rgcTemp);
					#endif
				}
				break;	
				case CONFIG_HARD_LEVEL_SETTING:
				{
					for(ucCount = 1; ucCount < pstCANrxmsg->DLC; ucCount++)
					{
						SplitByte.rgucData[ucCount - 1] = pstCANrxmsg->Data[ucCount];
					}
					if(LOW_THERSHOLD == pstCANrxmsg->Data[0])
					{
						stLL[HARDNER].fLiquidLevelLowTH = SplitByte.fData;
						#ifdef CONFIG_DBG   
							DEBUG_PRINTF("HARDNER LL Low \r\n");
						#endif
						uliConfigParaByte |= CONFIG_LL_HARD_LOW_TH;
					}
					else if(HIGH_THERSHOLD == pstCANrxmsg->Data[0])
					{
						stLL[HARDNER].fLiquidLevelHighTH = SplitByte.fData;
						#ifdef CONFIG_DBG   
							DEBUG_PRINTF("HARDNER LL High\r\n");
						#endif
						uliConfigParaByte |= CONFIG_LL_HARD_HIGH_TH;
					}
					else
					{
						#ifdef CONFIG_DBG      
							DEBUG_PRINTF("INVALID MSG\r\n");
						#endif
					}
					#ifdef CONFIG_DBG  
						sprintf(rgcTemp,"Thershold %0.2f\r\n",SplitByte.fData);
						DEBUG_PRINTF("%s\r\n",rgcTemp);
					#endif	
				}
				break;	
				case CONFIG_TANK_HEIGHT:
				{
					for(ucCount = 1; ucCount < pstCANrxmsg->DLC; ucCount++)
					{
						SplitByte.rgucData[ucCount - 1] = pstCANrxmsg->Data[ucCount];
					}
						
					if(BASE_TANK == pstCANrxmsg->Data[0])
					{	
						stLL[BASE].fLiquidLevelTankHeight = SplitByte.fData;
						#ifdef CONFIG_DBG      
							DEBUG_PRINTF("BASE \r\n");
						#endif
						uliConfigParaByte |= CONFIG_BASE_TANK_HEIGHT;
					}
					else if(HARD_TANK == pstCANrxmsg->Data[0])
					{
						stLL[HARDNER].fLiquidLevelTankHeight = SplitByte.fData;
						#ifdef CONFIG_DBG      
							DEBUG_PRINTF("HARD\r\n");
						#endif
						uliConfigParaByte |= CONFIG_HARD_TANK_HEIGHT;
					}
					else
					{
						#ifdef CONFIG_DBG      
							DEBUG_PRINTF("INVALID MSG\r\n");
						#endif
					}
					#ifdef CONFIG_DBG  
						sprintf(rgcTemp,"TANK HEIGHT %0.2f PARA: %d\r\n",SplitByte.fData, uliConfigParaByte);
						DEBUG_PRINTF("%s\r\n",rgcTemp);
					#endif	
				}
				break;
				case CONFIG_LL_SAMPLE_RATE:
				{			
					/*Sampling data is Rx by Dividing 100 of the actual value shown on the Tab or HMI*/
					ucLLWindowSize = pstCANrxmsg->Data[0];
					uliConfigParaByte |= CONFIG_LL_SAMPLING_RATE;
					
					if(ucLLWindowSize >= LL_WINDOW_SIZE)
					{
						ucLLWindowSize = LL_WINDOW_SIZE - 1;
					}
					
					/*RESET all flags and variable for both liquid level sensor reading*/
					stLL[BASE].bSlideWindowFull = RESET;					
					memset(stLL[BASE].rgfLiquidLevelBuff, 0, 4 * LL_WINDOW_SIZE);
					stLL[BASE].fSumOfLL = 0.0f;
					stLL[BASE].fLiquidLevel = 0.0f;
					stLL[BASE].ucWindowtraverse = 0;
					
					stLL[HARDNER].bSlideWindowFull = RESET;					
					memset(stLL[HARDNER].rgfLiquidLevelBuff, 0, 4 * LL_WINDOW_SIZE);
					stLL[HARDNER].fSumOfLL = 0.0f;
					stLL[HARDNER].fLiquidLevel = 0.0f;
					stLL[HARDNER].ucWindowtraverse = 0;
					#ifdef CONFIG_DBG      
						DEBUG_PRINTF("LIQUID LEVEL SAMPLE RATE CONFIG: %d\r\n",
												 ucLLWindowSize);
					#endif	
				}
				break;	
				case CONFIG_THERMOCOUPLE_REMAP:
				{
					uliConfigParaByte |= CONFIG_TC_REMAP_RCV;

					stTC.stRead[INLINE_BASE].ucThermoPosition = pstCANrxmsg->Data[0];
					#ifdef CONFIG_DBG      
						DEBUG_PRINTF("INLINE_BASE Thermo REMAP POS: %d PARA: %d\r\n",
												 pstCANrxmsg->Data[0],uliConfigParaByte);
					#endif	
					
					stTC.stRead[INLINE_HARD].ucThermoPosition  = pstCANrxmsg->Data[1];
					#ifdef CONFIG_DBG      
						DEBUG_PRINTF("INLINE_HARD Thermo REMAP POS: %d\r\n",
												 pstCANrxmsg->Data[1]);
					#endif	
					
					stTC.stRead[ANTIFREEZ].ucThermoPosition  = pstCANrxmsg->Data[2];
					#ifdef CONFIG_DBG      
						DEBUG_PRINTF("ANTIFREEZ Thermo REMAP POS: %d\r\n",
												 pstCANrxmsg->Data[2]);
					#endif	
					
					stTC.stRead[RETURN_LINE].ucThermoPosition  = pstCANrxmsg->Data[3];
					#ifdef CONFIG_DBG      
					  DEBUG_PRINTF("RETURN_LINE Thermo REMAP POS: %d PARA: %d\r\n",
						pstCANrxmsg->Data[3], uliConfigParaByte);
					#endif	
				}
				break;
				case CONFIG_SSR_REMAP:
				{
					uliConfigParaByte |= CONFIG_SSR_REMAP_RCV;
					
					stTC.stProcess[INLINE_BASE].ucSSRPosition = pstCANrxmsg->Data[0];
					#ifdef CONFIG_DBG      
						DEBUG_PRINTF("INLINE_BASE SSR REMAP POS: %d\r\n",
											   pstCANrxmsg->Data[0]);
					#endif	
					stTC.stProcess[INLINE_HARD].ucSSRPosition  = pstCANrxmsg->Data[1];
					#ifdef CONFIG_DBG      
						DEBUG_PRINTF("INLINE_HARD SSR REMAP POS: %d\r\n",
												 pstCANrxmsg->Data[1]);
					#endif	
					stTC.stProcess[ANTIFREEZ].ucSSRPosition  = pstCANrxmsg->Data[2];
					#ifdef CONFIG_DBG      
						DEBUG_PRINTF("ANTIFREEZ SSR REMAP POS: %d\r\n",
												 pstCANrxmsg->Data[2]);
					#endif	
					ucHardSSRPosition  = pstCANrxmsg->Data[3];
					#ifdef CONFIG_DBG      
						DEBUG_PRINTF("HARD SSR REMAP POS: %d PARA : %d\r\n",ucHardSSRPosition,
						uliConfigParaByte);
					#endif	
				}
				break;
				case CONFIG_LL_SENSOR_REMAP:
				{
					uliConfigParaByte |= CONFIG_LL_REMAP_RCV;
					stLL[BASE].ucADCChannel = LIQUID_LEVEL_CH(pstCANrxmsg->Data[0]);
					#ifdef CONFIG_DBG      
					DEBUG_PRINTF("BASE LL REMAP POS: %d, ADC Ch : %d\r\n",pstCANrxmsg->Data[0], stLL[BASE].ucADCChannel);
					#endif	
					stLL[HARDNER].ucADCChannel = LIQUID_LEVEL_CH(pstCANrxmsg->Data[1]);
					#ifdef CONFIG_DBG      
						DEBUG_PRINTF("HARD LL REMAP POS: %d, ADC Ch : %d PARA :%x\r\n",pstCANrxmsg->Data[1],
						stLL[HARDNER].ucADCChannel,uliConfigParaByte);
					#endif	
				}
				break;
				case CONFIG_HCM_PERIPHRAL_BYPASS:
				{
					uliConfigParaByte |= CONFIG_BYPASS_RCV;
					
					SplitByte.rgucData[0] = pstCANrxmsg->Data[0];
					SplitByte.rgucData[1] = pstCANrxmsg->Data[1];
					uiBypass = SplitByte.uiData;
					
					#ifdef CONFIG_DBG      
						DEBUG_PRINTF("BYPASS RCV: 0%02x PARA : %d\r\n",uiBypass,uliConfigParaByte);
					#endif	
				}
				break;
			}
		}
		break;
	}
}




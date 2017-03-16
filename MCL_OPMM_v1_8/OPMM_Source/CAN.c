/******************************************************************************
  **@file    CAN.c
  * 
  **@brief   this file contain all the CAN funtions
  ****************************************************************************
  */  
/* ----------------------------------------------------------------------------
 *                           Includes
 * ---------------------------------------------------------------------------
 */
#include "Config.h"
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

CanTxMsg stTxMsg;  /*CAN1 Tx msg */
CanTxMsg rgstTxCANBuff[TX_CAN_BUF_SIZE] = {0};
CanRxMsg rgstRxCANBuff[RX_CAN_BUF_SIZE];

uint8_t ucStoreRxCANBuffCnt 	= 0;
uint8_t ucRxCANProcessCnt 		= 0;
uint8_t ucStoreBuffCnt 				= 0;
uint8_t ucTxBuffCnt 					= 0;
uint16_t uiMaxPrimeTime 			= 0;
uint16_t uiMaxFlushTime 			= 0;
uint16_t uiMaxGunDelayTime 		= 0;
uint16_t uiSprayOverlapTime 	= 0;
uint16_t uiReverseDelayTime 	= 0;
uint8_t ucMaxSprayCount 			= 0;
uint16_t uiProcessTimeCount 	= 0;
uint16_t uiCANCount 					= 0;
uint8_t ucSendAckMBNo 				= 0;
uint8_t ucBuzzerVolume 				= 0;
/* ----------------------------------------------------------------------------
 *                           EXTERNAL VARIABLES
 * ---------------------------------------------------------------------------
 */
/* ----------------------------------------------------------------------------
 *                           EXTERNAL FUNCTIONS
 * ---------------------------------------------------------------------------
 */
/* ----------------------------------------------------------------------------
 *                           FUNCTIONS DECLARATION
 * ---------------------------------------------------------------------------
 */
/******************************************************************************
 **@Function 			: fnCAN_Config
 **@Descriptions	: Configures the CAN bus
 **@Parameters		: None
 **@Return				: None
*****************************************************************************/
void fnCAN_Config(void)
{
	GPIO_InitTypeDef stGPIO_InitStructure;
  NVIC_InitTypeDef stNVIC_InitStructure;
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APBPERIPH_GPIO, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, DISABLE);
  
  /* CAN1 Periph clock enable */
  RCC_APB1PeriphResetCmd(RCC_APB1Periph_CAN1, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
  
  /* Configure CAN pin: RX */
  stGPIO_InitStructure.GPIO_Pin = CAN_RX;
  stGPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  stGPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(CAN_PORT, &stGPIO_InitStructure);
  
  /* Configure CAN pin: TX */
  stGPIO_InitStructure.GPIO_Pin = CAN_TX;
  stGPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  stGPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(CAN_PORT, &stGPIO_InitStructure);
  	
  /* Enable CAN1 RX0 interrupt IRQ channel */
  stNVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
  stNVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  stNVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
  stNVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&stNVIC_InitStructure);
}
/******************************************************************************
 **@Function	 		: fnCANMsg_Init
 **@Descriptions	: CAN Message initialization And CAN Filter Configuration
 **@Parameters		: None
 **@Return				: None
 *****************************************************************************/
void fnCANMsg_Init(void)
{
  CAN_InitTypeDef        stCAN_InitStructure;
  CAN_FilterInitTypeDef  stCAN_FilterInitStructure;
  
  /* CAN register init */
  CAN_DeInit(CAN1);
  CAN_StructInit(&stCAN_InitStructure);
  
  /* CAN cell init */
  stCAN_InitStructure.CAN_TTCM = DISABLE;
  stCAN_InitStructure.CAN_ABOM = DISABLE;
  stCAN_InitStructure.CAN_AWUM = DISABLE;
  stCAN_InitStructure.CAN_NART = DISABLE;
  stCAN_InitStructure.CAN_RFLM = ENABLE;
  stCAN_InitStructure.CAN_TXFP = DISABLE;
  stCAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
  
  /*Baudrate setting 125000bps*/
  stCAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
  stCAN_InitStructure.CAN_BS1 = CAN_BS1_10tq;
  stCAN_InitStructure.CAN_BS2 = CAN_BS2_7tq;
  stCAN_InitStructure.CAN_Prescaler = 16;
	 
  CAN_Init(CAN1, &stCAN_InitStructure);
  
  /* CAN filter init */ /*For filter no=0*/
  stCAN_FilterInitStructure.CAN_FilterNumber = 0;
  stCAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
  stCAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
  stCAN_FilterInitStructure.CAN_FilterIdHigh = OPMM_CAN_FILTERIDHIGH;
  stCAN_FilterInitStructure.CAN_FilterIdLow = OPMM_CAN_FILTERIDLOW_ALL; 
  stCAN_FilterInitStructure.CAN_FilterMaskIdHigh = CAN_FILTERMASKIDHIGH;
  stCAN_FilterInitStructure.CAN_FilterMaskIdLow = CAN_FILTERMASKIDLOW;
  stCAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FIFO0;
  stCAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
  CAN_FilterInit(&stCAN_FilterInitStructure);


	/* CAN filter init, filter for Destination as OPMM*/
	stCAN_FilterInitStructure.CAN_FilterNumber = 1;	
  stCAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	stCAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;	
	stCAN_FilterInitStructure.CAN_FilterIdLow = OPMM_CAN_FILTERIDLOW;
	stCAN_FilterInitStructure.CAN_FilterIdHigh = OPMM_CAN_FILTERIDHIGH;
	stCAN_FilterInitStructure.CAN_FilterMaskIdHigh = CAN_FILTERMASKIDHIGH;
	stCAN_FilterInitStructure.CAN_FilterMaskIdLow = CAN_FILTERMASKIDLOW;
	stCAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FIFO0;
	stCAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&stCAN_FilterInitStructure);
  
  
  /* CAN FIFO0 message pending interrupt enable */
  CAN_ITConfig(CAN1,CAN_IT_FMP0, ENABLE);
  
  CAN_ITConfig(CAN1,CAN_IT_FF0, ENABLE);
  CAN_ITConfig(CAN1,CAN_IT_FOV0, ENABLE);
  CAN_ITConfig(CAN1,CAN_IT_ERR, DISABLE);
  CAN_ITConfig(CAN1,CAN_IT_TME, ENABLE);
}
/******************************************************************************
 **@Function 			: fnCAN_Transmit
 **@Descriptions	: Sends Response frame on CAN bus
 **@Parameters		: pstCANTxMsg: Pointer to  CAN Transmit Structure
 **@Return				: None
 ****************************************************************************/
void fnCAN_Transmit(CanTxMsg *pstCANTxMsg)
{
	CanTxMsg stTx1Msg;
	static uint8_t sucMailBoxNo = 0;
	uint8_t ucCnt; /*Count variable to fill Transmit Message Frame*/
	/*Configure CAN Transmitt Message tructure*/
	stTx1Msg.StdId = pstCANTxMsg->StdId;
	stTx1Msg.ExtId = pstCANTxMsg->ExtId; 
	stTx1Msg.IDE = CAN_ID_EXT;
	stTx1Msg.RTR = CAN_RTR_DATA;
	
	/* Reset Data Bufer */
	memset(stTx1Msg.Data,0, sizeof(stTx1Msg.Data)); 
	
	/*Update Length of CAN Data Frame*/
	stTx1Msg.DLC = pstCANTxMsg->DLC;
	
	/*Update CAN Data Frame into Transmitt Structure*/
	for(ucCnt=0; ucCnt < stTx1Msg.DLC; ucCnt++)
	{
		stTx1Msg.Data[ucCnt] = pstCANTxMsg->Data[ucCnt];
	}
		
	/*Transmitt CAN message onto CAN bus*/
	if(SET == StatusFlag.stStatusFlag.bCAN1ActiveFlag) 
	{
		if(CANTXOK == CAN_TransmitStatus(CAN1,ucSendAckMBNo))
		{
			CAN_CancelTransmit(CAN1, sucMailBoxNo);
		}
		/******/
		
		sucMailBoxNo = CAN_Transmit(CAN1, &stTx1Msg);
		
		if(stTx1Msg.ExtId == (STATUS_GUN_POSITION | MSG_TYPE_STATUS | 	DEST_SCU | SOURCE_OPMM))
		{
			//DEBUG_PRINTF("in FnCANTX data = %d mailbox Num =%d \r\n",stTx1Msg.Data[0],sucMailBoxNo);
		}
//		#ifdef GENERAL_DBG	
//			DEBUG_PRINTF("Main TxMt = %x \r\n",sucMailBoxNo);
		//DEBUG_PRINTF("in CAN1 TX = 0x%08x \r\n",stTx1Msg.ExtId);
		
//		#endif	
	}
	if(SET == StatusFlag.stStatusFlag.bMCPCAN2ActiveFlag)
	{
		fnMCPCAN2_Transmit(&stTx1Msg);
		//#ifdef GENERAL_DBG	
//			DEBUG_PRINTF("in CAN2 TX = 0x%08x \r\n", stTx1Msg.ExtId);
		//#endif	
	}
	StatusFlag.stStatusFlag.bTxFlag = SET;
	
}
/******************************************************************************
 **@Function 			: fnSendACK
 **@Descriptions	: Sends acknowledgement frame on CAN bus
 **@Parameters		: pstCANrxmsg: Pointer to Structure receive CAN MSG
 **@Return				: None
 ****************************************************************************/
void fnSendACK(CanRxMsg *pstCANrxmsg)
{
	CanTxMsg stTxAckCan;
	SplitByte.uliData = pstCANrxmsg->ExtId;
	
	/*Configure CAN Transmitt Message tructure*/
	stTxAckCan.IDE = CAN_ID_EXT;
	stTxAckCan.RTR = CAN_RTR_DATA;
	
	/* Reset Data Bufer */
	memset(stTxAckCan.Data,0, sizeof(stTxAckCan.Data)); 
	stTxAckCan.Data[0] = 0;
	
	/*Only for First heart beat acknowldge*/
	if((RESET == StatusFlag.stStatusFlag.bFirstBootUp) && 
		 (0x2FF02 == pstCANrxmsg->ExtId))
	{
		StatusFlag.stStatusFlag.bFirstBootUp = SET;
		stTxAckCan.Data[0] = 0x02;
	}
	
	/*Update Length of CAN Data Frame*/
	stTxAckCan.DLC = 1;
	SplitByte.rgucData[2] = 0xE8; /*acknowledge*/	
	SplitByte.rgucData[1] = SplitByte.rgucData[0];
	SplitByte.rgucData[0] = 0x03;
	
	stTxAckCan.ExtId = SplitByte.uliData;
	
	
		
	/*Transmitt CAN message onto CAN bus*/
	if(SET == StatusFlag.stStatusFlag.bCAN1ActiveFlag) 
	{
		ucSendAckMBNo = CAN_Transmit(CAN1, &stTxAckCan);
	}
	if(SET == StatusFlag.stStatusFlag.bMCPCAN2ActiveFlag)
	{
		fnMCPCAN2_Transmit(&stTxAckCan);
		//DEBUG_PRINTF("in ACK CAN2 TX = 0x%08x \r\n",stTxAckCan.ExtId);
	}
}
/******************************************************************************
 **@Function 			:  fnCANMsg_Validate
 **@Descriptions	:  Validate received can message as per the Device ID
 **@Parameters		:  None
 **@Return				:  None
 ****************************************************************************/
void fnCANMsg_Validate(CanRxMsg *pstCANrxmsg)
{
	uint32_t uliCanID = pstCANrxmsg->ExtId;
	
	#ifdef DBG_CAN			 
		DEBUG_PRINTF("ID = 0x%x\r\n", pstCANrxmsg->ExtId);
	#endif
	
/************Destination Addr*************/
  switch(uliCanID & DEST_ADDR_MASK)
	{
		case DEST_ALL:
		{
			#ifdef DBG_CAN      
				DEBUG_PRINTF("Broad Cast Message..\r\n");
			#endif	
			/*message is for ALL so precess it*/
			fnCANMsg_Process(pstCANrxmsg); 
		}
		break;
		case DEST_OPMM:
		{
			#ifdef DBG_CAN      
				DEBUG_PRINTF("Message For OPMM..\r\n");
			#endif	
			/*message is for OPMM so process it*/
			fnCANMsg_Process(pstCANrxmsg); 
		}
		break;
		default:
		{		
			#ifdef DBG_CAN      
				DEBUG_PRINTF("Invalid Msg Not for OPMM..\r\n");
			#endif
		}
	}	
}

/******************************************************************************
 **@Function 			:	 fnCANMsg_Process
 **@Descriptions	:  Process received can message
 **@Parameters		:	 pstCANrxmsg: Pointer to Structure of Received CAN Message
 **@Return				:  None
 ****************************************************************************/
void fnCANMsg_Process(CanRxMsg *pstCANrxmsg)
{
	uint8_t ucCnt = 0;
	uint32_t uliCanID = pstCANrxmsg->ExtId;
	#ifdef CONFIG_DBG
	uint8_t rgucTemp[150];
	#endif
	
	#ifdef DBG_CAN  
	
		DEBUG_PRINTF("In fnCANMsg_Process.. RX ID : 0x%08x\r\n",uliCanID);
	#endif	
	
	if(ACKNOWLEDGEMENT != (pstCANrxmsg->ExtId & ACKNOWLEDGEMENT))
	{
		fnSendACK(pstCANrxmsg); 
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
					StatusFlag.stStatusFlag.bEStopFlag = SET;

					

					
					fnValve_ONOFF(SPRAY_GUN, VALVE_OFF);
					fnValve_ONOFF(AB_MAT, VALVE_OFF);
					fnValve_ONOFF(FLUSH_MAT, VALVE_OFF);
					fnValve_ONOFF(MFORWARD, VALVE_OFF);
					fnValve_ONOFF(MBACKWARD, VALVE_OFF);
					
					/*Gun should be away from pipe when estop pressed*/
					fnValve_ONOFF(SPRAY_HEAD, VALVE_ON);

				}
				break;	
				case BCAST_HEARTBEAT:
				{
					uiHBTimeCounter = 0;
					StatusFlag.stStatusFlag.bCANSwitchFlag = RESET;
					StatusFlag.stStatusFlag.bHeartBeatMsgFlag = SET;
					
					/*4thAugust2016 - Android app and SCU get config para 
						requires more time than assumed*/
					StatusFlag.stStatusFlag.bCANFaultSCU = RESET;	
				}
				break;	
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
				case ACKNOWLEDGEMENT:
				{	
					StatusFlag.stStatusFlag.bACKRxFlag = SET;
					#ifdef DBG_CAN      
						DEBUG_PRINTF("CMD Ack Rcv..\r\n");
					#endif										
				}
				break;
				case CMD_CLEAR_ERROR: /* Command for Clear all Faults */
				{
					fnAllFaults_Clear();
					#ifdef DBG_CAN      
						DEBUG_PRINTF("CMD to Clear Faults.....\r\n");
					#endif
				}
				break;
				case CMD_CLEAR_WARNING:/*Command From SCU for Clear all Warnings*/
				{						
					fnAllWarnnings_Clear();
					#ifdef DBG_CAN      
						DEBUG_PRINTF("CMD to Clear Warnings.....\r\n");
					#endif
				}
				break;
				case CMD_OPMM_VALVE: 
				{			
					if(OPERATION_MODE == ucMode)
					{						
						if(pstCANrxmsg->Data[0] & 0x3F)
						{	
							ucCnt = 0;
							/*For Position */
							while(((pstCANrxmsg->Data[0] >> ucCnt) & 0x01) != 0x01)
							{
								ucCnt++;
							}
						
							fnValve_ONOFF(ucCnt,pstCANrxmsg->Data[1] >> 1);
							#ifdef DBG_CAN      
								DEBUG_PRINTF("CMD to Turn ON/OFF the Valves.....%d\r\n",ucCnt);
							#endif
						}
						
					}
				}
				break;
				case CMD_LASER_BEAM:
				{
					if(LASER_BEAM_ON == pstCANrxmsg->Data[0])
					{
						/*Turn ON the Laser Beam*/
						GPIO_WriteBit(LASER_BEAM_GPIO_PORT, LASER_BEAM_GPIO_PIN, Bit_SET);
						#ifdef DBG_CAN      
							DEBUG_PRINTF("CMD to Turn ON Laser Beam.....\r\n");
						#endif
					}
					else if(LASER_BEAM_OFF == pstCANrxmsg->Data[0])
					{
						/*Turn OFF the Laser Beam*/
						GPIO_WriteBit(LASER_BEAM_GPIO_PORT, LASER_BEAM_GPIO_PIN, Bit_RESET);
						#ifdef DBG_CAN      
							DEBUG_PRINTF("CMD to Turn OFF Laser Beam.....\r\n");
						#endif
					}
				}
				break;
				
				
				case CMD_PRIME_SELECTOR_START:
				{	
					ucReadNCTSCmd = 0; /* RESET the Read Non-Contact temp sensor command*/
					if(PRIME_SELECTOR == pstCANrxmsg->Data[0])
					{
						if((RESET == StatusFlag.stStatusFlag.bSystemErrorFlag) && 
							 (RESET == StatusFlag.stStatusFlag.bEStopReceived))
						{
							uiGunPositionCounter = 0;
							StatusFlag.stStatusFlag.bInitFlushProcess = RESET;/*to Stop/RESET Flush*/
							/*to Start Prime Process*/
							StatusFlag.stStatusFlag.bInitPrimeProcess = SET; 
							/*For Prime Selector Action*/
							StatusFlag.stStatusFlag.bPrimeSelector = SET;
							
							
							/*StatusFlag.stStatusFlag.bAutoFlushFlag = SET;
							uiAutoFlushCnt = 0;*/
							
							/*Turn OFF SPray Gun Valves*/
							fnValve_ONOFF(FLUSH_MAT, VALVE_OFF);
							/*Indication for Prime Done Once till Prime Time set by user*/												
							StatusFlag.stStatusFlag.bOneTimeAutoProcess = RESET;
							
							/*this flag is used to flush if flush not completed or going on*/
							StatusFlag.stStatusFlag.bVBatFlushStartFlag = RESET;
						}
						#ifdef PROCESS_DBG      
							DEBUG_PRINTF("CMD Prime Selector....\r\n");
						#endif
					}						
					else if(PRIME_PROCESS_START == pstCANrxmsg->Data[0])
					{
						if((RESET == StatusFlag.stStatusFlag.bSystemErrorFlag) && 
							 (RESET == StatusFlag.stStatusFlag.bEStopReceived))
						{
							StatusFlag.stStatusFlag.bProcessTimeFlag = SET;
							uiProcessTimeCount = 0;		
							
							if(AUTOMATIC == pstCANrxmsg->Data[0])
							{
								StatusFlag.stStatusFlag.bAutoPrime = SET;
							}
							if(MANUAL == pstCANrxmsg->Data[0])
							{
								StatusFlag.stStatusFlag.bManualPrime = SET;
								/*RESET the Flags */
								StatusFlag.stStatusFlag.bPrimeTimeAchiveTx = RESET;
								StatusFlag.stStatusFlag.bTimeAchievStatusFlag = RESET;
							}
							
							
							/*StatusFlag.stStatusFlag.bAutoFlushFlag = RESET;
							uiAutoFlushCnt = 0;*/
							
							/*Turn OFF SPray Gun Valves*/
							fnValve_ONOFF(FLUSH_MAT, VALVE_OFF);
							
							
							StatusFlag.stStatusFlag.bInitFlushProcess = RESET;
							/*adf 21/10/2016*/
//							StatusFlag.stStatusFlag.bFlushWarnTime = SET;
//							uiFlushWarnCnt = 0;
							
						}
						#ifdef PROCESS_DBG      
							DEBUG_PRINTF("CMD to Prime Process Start...\r\n");
						#endif
					}
					else if(PRIME_PROCESS_STOP == pstCANrxmsg->Data[0])
					{
						BUZZER_OFF();
						#ifdef PROCESS_DBG
							//DEBUG_PRINTF("BUZZER_OFF PRIME_PROCESS_STOP CAN PARSER\r\n");
						#endif
						StatusFlag.stStatusFlag.bManualPrimeDone = SET;
						#ifdef PROCESS_DBG      
							DEBUG_PRINTF("CMD to Prime Process STOP...\r\n");
						#endif
					}
				}
				break;
				case CMD_SPRAY_SELECTOR_START:
				{	
					ucReadNCTSCmd = 0; /* RESET the Read Non-Contact temp sensor command*/

					/*Single coat or Auto coat spray selector command*/
					if(SPRAY_SELECTOR == pstCANrxmsg->Data[0])
					{
						if((RESET == StatusFlag.stStatusFlag.bSystemErrorFlag) &&
						   (RESET == StatusFlag.stStatusFlag.bEStopReceived))
						{
							uiGunPositionCounter = 0;
							StatusFlag.stStatusFlag.bInitPrimeProcess = RESET;
							StatusFlag.stStatusFlag.bInitFlushProcess = RESET;
							
							StatusFlag.stStatusFlag.bInitSprayProcess = SET;
							StatusFlag.stStatusFlag.bSpraySelector = SET;
										
							/*StatusFlag.stStatusFlag.bAutoFlushFlag = SET;
							uiAutoFlushCnt = 0;*/		
							
							/*Turn OFF Flush valve Gun Valves*/
							fnValve_ONOFF(FLUSH_MAT, VALVE_OFF);
														
							StatusFlag.stStatusFlag.bInitFlushProcess = RESET;
							
							StatusFlag.stStatusFlag.bOneTimeAutoProcess = RESET;
							/*this is used to flush if flush is not done or on going*/
							StatusFlag.stStatusFlag.bVBatFlushStartFlag = RESET;
						}
						#ifdef PROCESS_DBG      
							DEBUG_PRINTF("CMD to Spray Selector....\r\n");
						#endif
					}
					/*SPRAY START command from SCU*/
					else if((AUTO_COAT == pstCANrxmsg->Data[0]) || 
									(SINGLE_COAT == pstCANrxmsg->Data[0]))
					{
						if((RESET == StatusFlag.stStatusFlag.bSystemErrorFlag) && 
							 (RESET == StatusFlag.stStatusFlag.bEStopReceived))
						{
//							StatusFlag.stStatusFlag.bFlushWarnTime = SET; /*adf 21/10/2016*/
							if(SINGLE_COAT == pstCANrxmsg->Data[0])
							{
								StatusFlag.stStatusFlag.bEXtraSprayCoat = SET;
								#ifdef PROCESS_DBG      
									DEBUG_PRINTF("SINGLE_COAT....\r\n");
								#endif
							}
							else
							{
								#ifdef PROCESS_DBG      
									DEBUG_PRINTF("AUTO_COAT\r\n");
								#endif
							}
							StatusFlag.stStatusFlag.bStartSpray = SET;
							StatusFlag.stStatusFlag.bProcessTimeFlag = SET;
							uiProcessTimeCount = 0;
							
							ucCurrentLimitSwitch = 0;
							StatusFlag.stStatusFlag.bDCLSPosition = RESET;
							StatusFlag.stStatusFlag.bHomePosition = RESET;
							
							/*StatusFlag.stStatusFlag.bAutoFlushFlag = RESET;
							uiAutoFlushCnt = 0;*/
							
							/*Turn OFF SPray Gun Valves*/
							fnValve_ONOFF(FLUSH_MAT, VALVE_OFF);
							
							StatusFlag.stStatusFlag.bInitFlushProcess = RESET;
						}
						#ifdef DBG_CAN      
							DEBUG_PRINTF("CMD to SPRAY_START.....\r\n");
						#endif
					}
				}
				break;
				case CMD_FLUSH_SELECTOR_START:
				{
					ucReadNCTSCmd = 0; /*RESET the Read Non-Contact temp sensor command*/					
					if(FLUSH_SELECTOR == pstCANrxmsg->Data[0])
					{
						uiGunPositionCounter = 0;	
						StatusFlag.stStatusFlag.bVBatFlushStartFlag = SET;
						
						StatusFlag.stStatusFlag.bInitPrimeProcess = RESET;
					
						StatusFlag.stStatusFlag.bInitFlushProcess = SET;
						StatusFlag.stStatusFlag.bFlushSelector = SET;
									
						#ifdef PROCESS_DBG
							DEBUG_PRINTF("CMD to Flush Selector....\r\n");
						#endif
					
						/*StatusFlag.stStatusFlag.bAutoFlushFlag = RESET;
						uiAutoFlushCnt = 0;*/
						/*Indication for Flush Done Once till Flush Time set by user*/	
						StatusFlag.stStatusFlag.bOneTimeAutoProcess = RESET;
					}
					/*Flush Start*/
					else if(FLUSH_PROCESS_START == pstCANrxmsg->Data[0])
					{
						StatusFlag.stStatusFlag.bProcessTimeFlag = SET;
						/*adf 21/10/2016*/
//						if(RESET ==	StatusFlag.stStatusFlag.bFlushWarnTime)
//						{
//							StatusFlag.stStatusFlag.bFlushWarnTime = RESET;
//							uiFlushWarnCnt = 0;
//							BUZZER_OFF();
//						}
						/*****************************/
						
						uiProcessTimeCount = 0;							
						if(MANUAL == pstCANrxmsg->Data[0])
						{
							StatusFlag.stStatusFlag.bManualFlush = SET;
							StatusFlag.stStatusFlag.bTimeAchievStatusFlag = RESET; 
							StatusFlag.stStatusFlag.bPrimeTimeAchiveTx = RESET;
						}
						#ifdef PROCESS_DBG      
							DEBUG_PRINTF("CMD to FLUSH_PROCESS_START.....\r\n");
						#endif
					}
					else if(FLUSH_PROCESS_STOP == pstCANrxmsg->Data[0])
					{
						BUZZER_OFF();
						#ifdef PROCESS_DBG
//							DEBUG_PRINTF("BUZZER_OFF FLUSH_PROCESS_STOP CAN PARSER\r\n");
						#endif
						StatusFlag.stStatusFlag.bManualFlushDone = SET;
						#ifdef PROCESS_DBG      
							DEBUG_PRINTF("CMD to FLUSH_PROCESS_STOP.....\r\n");
						#endif
					}
					/*Flush complete or JOINT/PROCESS COMPLETED*/
					else if(FLUSH_PROCESS_DONE == pstCANrxmsg->Data[0])
					{
						BUZZER_OFF();
						#ifdef PROCESS_DBG
//							DEBUG_PRINTF("BUZZER_OFF FLUSH_PROCESS_DONE CAN PARSER\r\n");
						#endif
						/*Turn OFF SPray Gun Valves*/
						fnValve_ONOFF(FLUSH_MAT, VALVE_OFF);
						fnValve_ONOFF(SPRAY_HEAD, VALVE_OFF);
						fnValve_ONOFF(SPRAY_GUN, VALVE_OFF);
													 
						StatusFlag.stStatusFlag.bInitFlushProcess = RESET;
						StatusFlag.stStatusFlag.bVBatFlushStartFlag = RESET;
					
						#ifdef PROCESS_DBG      
							DEBUG_PRINTF("CMD Process is DONE.....\r\n");
						#endif
					}
					else if(FLUSH_BUTTON_STOP_SPRAY == pstCANrxmsg->Data[0])
					{
						/*Stop the Spray when Stop button of remote is pressed*/
						if(SET == StatusFlag.stStatusFlag.bStartSpray) 
						{
							StatusFlag.stStatusFlag.bSprayFault = SET;
							ucSprayFaultStatus |= FLUSH_RECEIVED_FAULT;/*adf 21/10/2016*/
							StatusFlag.stStatusFlag.bFlushButtonStopSpray = SET;
							ucRotationCntBackup = ucCurrentLimitSwitch;
						}
						#ifdef PROCESS_DBG
							DEBUG_PRINTF("CMD to FLUSH_BUTTON_STOP_SPRAY.\r\n");
						#endif
					}
				}
				break;

				case CMD_READ_NON_CONTACT_TEMP:
				{	
					
					#ifdef PROCESS_DBG
							DEBUG_PRINTF("CMD_READ_NON_CONTACT_TEMP\r\n");
						#endif
					if(0 != ucNCTSConnected) /*one sensor must be connected*/
					{
						StatusFlag.stStatusFlag.bOpmmSetOnPipe = SET;
						if(0 == ucReadNCTSCmd)
						{
							if(0x0F == ucNCTSConnected)
							{
								ucNCTSRead = ucNCTSConnected;
							}
							else 
							{
								ucNCTSRead = 0x01; 
							}
						}
						else
						{
							if(0 != ucNCTSRead)
							{								
								ucNCTSRead <<= 1;
								if(ucNCTSRead > 0x0F)
								{
									ucNCTSRead = 0x01;
								}
							}
						}
						ucReadNCTSCmd++;
						#ifdef PROCESS_DBG
						DEBUG_PRINTF("CMD READ count = %d, Read : %d, Connected: %d\r\n", ucReadNCTSCmd, ucNCTSRead,ucNCTSConnected);
						#endif
					}
					else
					{
						#ifdef PROCESS_DBG
							DEBUG_PRINTF("NCTS Read CMD!! No Sensor Connected\r\n");
						#endif
					}
				}
				break;
				
				case CMD_MOVE_OPMM_TO_HOME:
				{
					
					/*need to discuss and impliment this to move the OPMM at Home Position*/
					#ifdef PROCESS_DBG      
						DEBUG_PRINTF("CMD_MOVE_OPMM_TO_HOME..\r\n");
					#endif
					/*Resetting these flag since it was not coming at home
					  Since the flag is set after hitting flush during spray,
						and when hit home button it count 1 for HLS/DCCLS, that's 
					  why it doesn't move the machine to home. adf 1/3/2017
					*/
					StatusFlag.stStatusFlag.bDCLSPosMTH = RESET;
					StatusFlag.stStatusFlag.bHLSPosMTH = RESET;
					StatusFlag.stStatusFlag.bDCLSPosition = RESET;
				  ucDCLSHitCntToHome = 0;
					StatusFlag.stStatusFlag.bHomePosition = RESET;
					ucHLSHitCntToHome = 0;
					StatusFlag.stStatusFlag.bMoveToHomePos = SET;
					ucRotationCntBackup = ucCurrentLimitSwitch;
				}
				break;
				
				case CMD_STOP_SPRAY:
				{
					/*Stop the Spray when Stop button of remote is pressed*/
					if(SET == StatusFlag.stStatusFlag.bStartSpray) 
					{
						StatusFlag.stStatusFlag.bSprayFault = SET;
						ucSprayFaultStatus |= STOP_CMD_FAULT;/*adf 21/10/2016*/ 
						StatusFlag.stStatusFlag.bStopSprayProcessbutton = SET;
					}

					#ifdef PROCESS_DBG      
						DEBUG_PRINTF("CMD_STOP_SPRAY...\r\n");
					#endif
				}
				break;
				default:
				{
					#ifdef DBG_CAN      
						DEBUG_PRINTF("Invalid Command Msg ID...\r\n");
					#endif
				}
				break;
			}
		}
		break;	
		case MSG_TYPE_FAULT:
		{
			#ifdef DBG_CAN      
				DEBUG_PRINTF("Msg Type: MSG_TYPE_FAULT \r\n");
			#endif			
			switch (uliCanID & MSG_MASK)
			{
				case ACKNOWLEDGEMENT:
				{
					StatusFlag.stStatusFlag.bACKRxFlag = SET;
					#ifdef DBG_CAN      
						DEBUG_PRINTF("Fault Ack Rcv..\r\n");
					#endif	
				}
				break;
				
				case FAULT_ERR_WARN_OCCUR:
				{
					if(SYSTEM_WARNING == pstCANrxmsg->Data[0])
					{
						
					}
					else if(SYSTEM_ERROR == pstCANrxmsg->Data[0])
					{
						/*System Error*/
						StatusFlag.stStatusFlag.bSystemErrorFlag = SET;
						//DEBUG_PRINTF("ERROR WARNNING OCCURS ---------...CAN\r\n");
					}
				}
				break;
				default:
				{
					#ifdef DBG_CAN      
						DEBUG_PRINTF("Invalid Fault Msg ID...\r\n");
					#endif
				}
				break;
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
				StatusFlag.stStatusFlag.bACKRxFlag = SET;
				#ifdef DBG_CAN      
					DEBUG_PRINTF("Data Ack Rcv..\r\n");
				#endif	
			}
			else
			{
				#ifdef DBG_CAN      
					DEBUG_PRINTF("Invalid Data Msg ID...\r\n");
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
				StatusFlag.stStatusFlag.bACKRxFlag = SET;
				#ifdef DBG_CAN      
					DEBUG_PRINTF("Status Ack Rcv from SCU..\r\n");
				#endif
			}
			else
			{
				#ifdef DBG_CAN      
					DEBUG_PRINTF("Invalid Status Msg ID...\r\n");
				#endif
			}
		}
		break;	
		case MSG_TYPE_CONFIG:
		{
			#ifdef DBG_CAN      
				DEBUG_PRINTF("Msg Type: MSG_TYPE_CONFIG \r\n");
			#endif
			switch (uliCanID & MSG_MASK)
			{					
				case CONFIG_SPRAY_COUNT:
				{
					ucMaxSprayCount = pstCANrxmsg->Data[0];
					uliConfigParaByte |= CONFIG_PARA_SPRAY_COUNT;
					#ifdef CONFIG_DBG      
				   	DEBUG_PRINTF("CONFIG_SPRAY_COUNT = %d parabyte = %04x \r\n",
													ucMaxSprayCount, uliConfigParaByte);
					#endif						
				}
				break;			
				case CONFIG_PRIME_TIME:
				{	
					/*splitting the data received from the CAN */
					for(ucCnt = 0; ucCnt < pstCANrxmsg->DLC; ucCnt++)
					{
						SplitByte.rgucData[ucCnt] = pstCANrxmsg->Data[ucCnt];
					}	
					
					uiMaxPrimeTime = (uint16_t)(SplitByte.fData * TIME_CONVERT_FACTOR_10MS);
					uliConfigParaByte |= CONFIG_PARA_PRIME_TIME;
					#ifdef CONFIG_DBG      
				   	DEBUG_PRINTF("CONFIG Prime time = %d parabyte = %04x \r\n",
													uiMaxPrimeTime, uliConfigParaByte);
					#endif	
				}
				break;
				case CONFIG_OPMM_GUN_DELAY:
				{
					/*splitting the data received from the CAN */
					for(ucCnt = 0; ucCnt < pstCANrxmsg->DLC; ucCnt++)
					{
						SplitByte.rgucData[ucCnt] = pstCANrxmsg->Data[ucCnt];
					}
					
					uiMaxGunDelayTime = (uint16_t)(SplitByte.fData * TIME_CONVERT_FACTOR_1MS);
					/* ADF 20/10/2016*/
					if(0 == uiMaxGunDelayTime)
					{
						uiMaxGunDelayTime = 1;
					}
					uliConfigParaByte |= CONFIG_PARA_GUNDELAY_TIME;
					//#ifdef CONFIG_DBG      
						DEBUG_PRINTF("CONFIG Gun Delay time = %d parabyte = %04x \r\n",
													uiMaxGunDelayTime, uliConfigParaByte);
					//#endif
				}
				break;
				case CONFIG_OPMM_SPRAY_OVERLAP_TIME:
				{
					/*splitting the data received from the CAN */
					for(ucCnt = 0; ucCnt < pstCANrxmsg->DLC; ucCnt++)
					{
						SplitByte.rgucData[ucCnt] = pstCANrxmsg->Data[ucCnt];
					}
					
					uiSprayOverlapTime = (uint16_t)(SplitByte.fData * TIME_CONVERT_FACTOR_1MS);
					/* ADF 20/10/2016*/
					if(0 == uiSprayOverlapTime)
					{
						uiSprayOverlapTime = 1;
					}
					uliConfigParaByte |= CONFIG_PARA_SPRAY_OVERLAP_TIME;
					//#ifdef CONFIG_DBG      
						DEBUG_PRINTF("CONFIG Spray overlap time = %d parabyte = %04x \r\n",
													uiSprayOverlapTime, uliConfigParaByte);
					//#endif
				}
				break;
				case CONFIG_OPMM_REVERSE_DELAY:
				{
					/*splitting the data received from the CAN */
					for(ucCnt = 0; ucCnt < pstCANrxmsg->DLC; ucCnt++)
					{
						SplitByte.rgucData[ucCnt] = pstCANrxmsg->Data[ucCnt];
					}
					/* this time is calc as  count = time(sec) X factor in Msec 1000 */
					uiReverseDelayTime = (uint16_t)(SplitByte.fData * TIME_CONVERT_FACTOR_1MS);
					uliConfigParaByte |= CONFIG_PARA_REVERSE_DELAY_TIME;
					/* ADF 20/10/2016*/
					if(0 == uiReverseDelayTime)
					{
						uiReverseDelayTime = 1;
					}
					
					//#ifdef CONFIG_DBG      
						DEBUG_PRINTF("CONFIG Reverse Delay time  = %d parabyte = %04x \r\n",
													uiReverseDelayTime, uliConfigParaByte);
					//#endif
				}
				break;
				case CONFIG_OPMM_FLUSH_TIME:
				{
					/*splitting the data received from the CAN */
					for(ucCnt = 0; ucCnt < pstCANrxmsg->DLC; ucCnt++)
					{
						SplitByte.rgucData[ucCnt] = pstCANrxmsg->Data[ucCnt];
					}
					
					uiMaxFlushTime = (uint16_t)(SplitByte.fData * TIME_CONVERT_FACTOR_10MS);
					uliConfigParaByte |= CONFIG_PARA_FLUSH_TIME;
					#ifdef CONFIG_DBG      
						DEBUG_PRINTF("CONFIG Flush time = %d parabyte = %04x\r\n",
													uiMaxFlushTime, uliConfigParaByte);
					#endif	
				}
				break;
				case CONFIG_DCLS_HOME_TIME:
				{
					/*splitting the data received from the CAN */
					for(ucCnt = 1; ucCnt < pstCANrxmsg->DLC; ucCnt++)
					{
						SplitByte.rgucData[ucCnt - 1] = pstCANrxmsg->Data[ucCnt];
					}
					
					if(DCLS_TIMER == pstCANrxmsg->Data[0])
					{
						uiDCLSTime = (uint16_t)(SplitByte.fData * TIME_CONVERT_FACTOR_10MS);
						uliConfigParaByte |= CONFIG_PARA_DCLS_HCLS_TIME;
						#ifdef CONFIG_DBG      
							DEBUG_PRINTF("DCLS-HOME reach time= %d parabyte = %04x \r\n",
														uiDCLSTime, uliConfigParaByte);
					  #endif
					}
					if(HLS_TIMER == pstCANrxmsg->Data[0])
					{
 						uiHLSTime = (uint16_t)(SplitByte.fData * TIME_CONVERT_FACTOR_10MS);
						uliConfigParaByte |= CONFIG_PARA_HCLS_DCLS_TIME;
						#ifdef DBG_CAN      
							DEBUG_PRINTF("HOME-DCLS reach time = %d parabyte = %04x \r\n", 
								uiHLSTime, uliConfigParaByte);
					  #endif
					}
				}
				break;
				case CONFIG_GUN_POSITION_CHANGE_TIME:
				{
					/*splitting the data received from the CAN */
					for(ucCnt=0; ucCnt < pstCANrxmsg->DLC; ucCnt++)
					{
						SplitByte.rgucData[ucCnt] = pstCANrxmsg->Data[ucCnt];
					}
				
					uiGunPosSwTime = (uint16_t)(SplitByte.fData * TIME_CONVERT_FACTOR_10MS);
				  uliConfigParaByte |= CONFIG_PARA_GUN_POS_SW_TIME;
					#ifdef CONFIG_DBG      
						DEBUG_PRINTF("Gun Pos switching Time = %d parabyte = %04x \r\n",
												uiGunPosSwTime, uliConfigParaByte);
					#endif
				}
				break;
				case CONFIG_SPRAY_VALVE_OPEN_TIME_VBAT:
				{
					/*splitting the data received from the CAN */
					for(ucCnt=0; ucCnt < pstCANrxmsg->DLC; ucCnt++)
					{
						SplitByte.rgucData[ucCnt] = pstCANrxmsg->Data[ucCnt];
					}
				
					uiSprayGunOpenTime = (uint16_t)(SplitByte.fData * TIME_CONVERT_FACTOR_10MS);
				  uliConfigParaByte |= CONFIG_PARA_SPRAY_GUN_OPEN_TIME;
					#ifdef CONFIG_DBG      
						DEBUG_PRINTF("Spray Gun Open Time = %d parabyte = %04x \r\n",
													uiSprayGunOpenTime, uliConfigParaByte);
					#endif
				}
				break;
				case CONFIG_FLUSH_WARNING_TIME:
				{
					/*splitting the data received from the CAN */
					for(ucCnt = 0; ucCnt < pstCANrxmsg->DLC; ucCnt++)
					{
						SplitByte.rgucData[ucCnt] = pstCANrxmsg->Data[ucCnt];
					}
				
					uiFlushWarnTime = (uint16_t)(SplitByte.fData * TIME_CONVERT_FACTOR_10MS);
				  uliConfigParaByte |=  CONFIG_PARA_FLUSH_WARNNING_TIME;
					#ifdef CONFIG_DBG      
						DEBUG_PRINTF("CONFIG_FLUSH_WARNNING_TIME = %d parabyte = %04x \r\n",
													uiFlushWarnTime, uliConfigParaByte);
					#endif
				}
				break;
				
				case CONFIG_OPMM_VALVE_REMAP:
				{
					/*splitting the data received from the CAN */
					for(ucCnt = 0; ucCnt < pstCANrxmsg->DLC; ucCnt++)
					{
						if(0 != pstCANrxmsg->Data[ucCnt])/*If Position is not 0*/
						{
							stValve[ucCnt].ucPosition = pstCANrxmsg->Data[ucCnt];
						#ifdef CONFIG_DBG
							DEBUG_PRINTF("VALVE %d REMAP: 0x%04x\r\n", ucCnt,
														stValve[ucCnt].ucPosition);
							#endif
						}
					}
									
				  uliConfigParaByte |= CONFIG_PARA_VALVE_REMAP;
					#ifdef DBG_CAN      
					DEBUG_PRINTF("OPMM_VALVE_REMAP: 0x%04x\r\n", uliConfigParaByte);
					#endif
				}
				break;
				case CONFIG_OPMM_PERIPHRAL_BYPASS:
				{
					/*splitting the data received from the CAN */
					for(ucCnt = 0; ucCnt < pstCANrxmsg->DLC; ucCnt++)
					{
						SplitByte.rgucData[ucCnt] = pstCANrxmsg->Data[ucCnt];
					}
				
					uiBypass = SplitByte.uiData;
					
					//ucNCTSConnected = (uint8_t)((uiBypass >> 8) & 0x0F);
				  uliConfigParaByte |= CONFIG_PARA_PERIPHRAL_BYPASS;
					#ifdef CONFIG_DBG      
						DEBUG_PRINTF("OPMM_PERIPHRAL_BYPASS = 0x%04x parabyte = %04x \r\n",
													uiBypass, uliConfigParaByte);
					#endif
				}
				break;
				case CONFIG_NON_CONTACT_TEMP_REMAP:
				{
					ucNCTSConnected = 0;
					/*splitting the data received from the CAN */
					for(ucCnt = 0; ucCnt < pstCANrxmsg->DLC; ucCnt++)
					{
						if(0 != pstCANrxmsg->Data[ucCnt])
						{
							stNCTS[ucCnt].ucPosition = pstCANrxmsg->Data[ucCnt];
							ucNCTSConnected |= 0x01 << ucCnt;
						}
					}
				  uliConfigParaByte |= CONFIG_PARA_NCTS_REMAP;
					#ifdef CONFIG_DBG      
						DEBUG_PRINTF("NON_CONTACT_TEMP_REMAP = 0x%02x parabyte = %04x \r\n",
													ucNCTSConnected, uliConfigParaByte);
					#endif
				}
				break;
				
				case CONFIG_OPMM_BUZZER_VOLUME:
				{				
					ucBuzzerVolume = pstCANrxmsg->Data[0];

//					/*setting Duty Cycle*/
//					TIM_SetCompare3(TIM1, ucBuzzerVolume);
					
				  uliConfigParaByte |= CONFIG_PARA_BUZZER_VOLUME;
					#ifdef CONF
						DEBUG_PRINTF("CONFIG_PARA_BUZZER_VOLUME = 0x%02x parabyte = %04x \r\n",
													ucBuzzerVolume, uliConfigParaByte);IG_DBG      
					#endif
				}
				break;
				
				
				
				case CONFIG_BAT_KNEE_VOLTAGE:
				{				
					/*splitting the data received from the CAN */
					for(ucCnt = 0; ucCnt < pstCANrxmsg->DLC; ucCnt++)
					{
						SplitByte.rgucData[ucCnt] = pstCANrxmsg->Data[ucCnt];
					}
					
					fBatKneeVoltage = SplitByte.fData;
					
				  uliConfigParaByte |= CONFIG_PARA_KNEE_VOLTAGE;
					#ifdef CONFIG_DBG   
						sprintf((char *)rgucTemp, "Knee voltage =  %f V,  parabyte = 0x%08x \r\n", fBatKneeVoltage, uliConfigParaByte);

						DEBUG_PRINTF((const char *)rgucTemp);
					#endif
				}
				break;
				
				
				case CONFIG_STOP_MOTOR_ROTATION_TIME:
				{				
					/*splitting the data received from the CAN */
					for(ucCnt = 0; ucCnt < pstCANrxmsg->DLC; ucCnt++)
					{
						SplitByte.rgucData[ucCnt] = pstCANrxmsg->Data[ucCnt];
					}
					
					uiStopMotorRotTimer = (uint16_t)(SplitByte.fData * TIME_CONVERT_FACTOR_10MS);
					
					
				  uliConfigParaByte |= CONFIG_PARA_STOP_MOTOR_ROTATION;
					#ifdef CONFIG_DBG      
						DEBUG_PRINTF("CONFIG_STOP_MOTOR_ROTATION_TIME = %d parabyte = %08x \r\n",
													uiStopMotorRotTimer, uliConfigParaByte);
					#endif
				}
				break;
				
				default:
				{
					#ifdef CONFIG_DBG      
						DEBUG_PRINTF("Invalid  Config Msg ID...\r\n");
					#endif	
				}
				break;	
			}
		}
		break;
		
		
		default:
		{
			#ifdef DBG_CAN      
				DEBUG_PRINTF("Invalid Msg Type...\r\n");
			#endif	
		}
		break;	
	}
}

/******************************************************************************
 **@Function		 	:  fnBuffer_Store
 **@Descriptions	:  Stores the CAN Structure in the Buffer
 **@Parameters		:  pstTxMsg: pointer to CAN Transmit structure
 **@Return				:  None
 *****************************************************************************/
void fnBuffer_Store(CanTxMsg *pstTxMsg)
{	
	#ifdef GENERAL_DBG
		DEBUG_PRINTF("In fnTxStoreBuff = %d \r\nId = 0x%x\r\n", 
									ucStoreBuffCnt, pstTxMsg->ExtId);
	#endif
	memmove(&rgstTxCANBuff[ucStoreBuffCnt++], pstTxMsg, sizeof(CanTxMsg));
	if(TX_CAN_BUF_SIZE == ucStoreBuffCnt)
	{
		ucStoreBuffCnt = 0;
		StatusFlag.stStatusFlag.bDataInCANTxBuffFlag = SET;
	}
}

/******************************************************************************
 **@Function		 	:  fnTxBuff_Shift
 **@Descriptions	:  Shift the Buffer when ack received or msg is discarded 
										 and transmit the next frame in the buffer
 **@Parameters		:  None
 **@Return				:  None
*****************************************************************************/
void fnTxBuff_Shift(void)
{
	memset(&rgstTxCANBuff[ucTxBuffCnt++], 0 , sizeof(CanTxMsg));
	if(TX_CAN_BUF_SIZE <= ucTxBuffCnt)
	{
		ucTxBuffCnt = 0;
		StatusFlag.stStatusFlag.bDataInCANTxBuffFlag = RESET;
	}
	#ifdef GENERAL_DBG
		DEBUG_PRINTF("In fnTxBuff_Shift TxCNT = %d and = %d\r\n",
								 ucTxBuffCnt,ucStoreBuffCnt);
	#endif
	if((ucTxBuffCnt != ucStoreBuffCnt) || 
		(SET == StatusFlag.stStatusFlag.bDataInCANTxBuffFlag))
	{
		for(; ((ucTxBuffCnt < ucStoreBuffCnt) || 
				(SET == StatusFlag.stStatusFlag.bDataInCANTxBuffFlag)) ; ucTxBuffCnt++)
		{
			/*Empty the CAN Transmit Buffer if the respective Destination CAN
				Fault occurs*/
			if(((DEST_HMI == (rgstTxCANBuff[ucTxBuffCnt].ExtId & DEST_ADDR_MASK)) && 
					(SET == StatusFlag.stStatusFlag.bCANFaultHMI)) || 
				 (((DEST_SCU == (rgstTxCANBuff[ucTxBuffCnt].ExtId & DEST_ADDR_MASK)) &&
					(SET == StatusFlag.stStatusFlag.bCANFaultSCU))))	
			{
				memset(&rgstTxCANBuff[ucTxBuffCnt], 0 , sizeof(CanTxMsg));
				#ifdef GENERAL_DBG
					DEBUG_PRINTF("emptying CAN MSG of OP, Txcnt = %d storebuff = %d \r\n",
												ucTxBuffCnt, ucStoreBuffCnt);
				#endif
			}
			else
			{
				break;
			}
		}
		if((ucTxBuffCnt != ucStoreBuffCnt) || 
			 (SET == StatusFlag.stStatusFlag.bDataInCANTxBuffFlag))
		{
			fnCAN_Transmit(&rgstTxCANBuff[ucTxBuffCnt]);
		}
	}
}

/******************************************************************************
 **@Function 			: fnCANMessage_Send
 **@Descriptions	: Sends status message on CAN
 **@Parameters		: uliMessageId: message id.
										uliMsgType:	Message type.
 **@Return				: None
 *****************************************************************************/
void fnCANMessage_Send(uint32_t uliMessageId, uint32_t uliMsgType)
{
	uint8_t ucCnt = 0;
	CanTxMsg stSendCANMsg;
	stSendCANMsg.DLC = stTxMsg.DLC;
	for(ucCnt = 0; ucCnt < stSendCANMsg.DLC; ucCnt++)
	{
		stSendCANMsg.Data[ucCnt] = stTxMsg.Data[ucCnt];
	}
	
	/*send message to SCU*/
	stSendCANMsg.ExtId = (uliMsgType  | RESERVED  | uliMessageId |
												DEST_SCU | SOURCE_OPMM);
	if((ucTxBuffCnt == ucStoreBuffCnt) && 
		 (RESET == StatusFlag.stStatusFlag.bDataInCANTxBuffFlag))
	{
		fnCAN_Transmit(&stSendCANMsg);
	}
	fnBuffer_Store(&stSendCANMsg);			
}

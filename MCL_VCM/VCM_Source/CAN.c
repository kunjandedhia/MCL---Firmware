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
	CAN_InitTypeDef        CAN_InitStructure;
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
	CanTxMsg stTxMsg;  									/*CAN1 Tx msg */
	SplitByte_t SplitByte;				/* union to split the data as float and char */
	CanRxMsg rgstRxCANBuff[RX_CAN_BUF_SIZE] = {0};
	CanTxMsg rgstTxCANBuff[TX_CAN_BUF_SIZE] = {0};				/*Buffer to store trasmitting CAN message*/
	uint8_t ucResendCount 									= 0;					/*CAN message resend counter*/
	uint32_t uliConfigParaByte 							= 0;					/*Config parameter received check variable*/
	uint32_t uliFaultId 										= 0x00000000;	/*To set the fault ID*/
	uint8_t ucCylinderCount 								= 0;					/*Number of cylinder*/
	uint8_t ucCAN1ProcessCnt 								= 0;
	uint8_t ucStoreRxCANBuffCnt 						= 0;
	uint8_t ucSendAckMBNo 									= 0;
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

void fnShiftBuff(void); 
/******************************************************************************
 **@Function 			: CAN_Config
 **@Descriptions	: Configures the CAN bus
 **@Parameters		: None
 **@Return				: None
 ****************************************************************************/
void CAN_Config(void)
{
  GPIO_InitTypeDef stGPIO_InitStructure;
  NVIC_InitTypeDef stNVIC_InitStructure;
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APBPERIPH_GPIO, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, DISABLE);
  
  /* CAN1 Periph clock enable */
  RCC_APB1PeriphResetCmd(RCC_APB1Periph_CAN1, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
  
  /* Configure CAN pin: RX */
  stGPIO_InitStructure.GPIO_Pin 		= CAN_RX_PIN;
  stGPIO_InitStructure.GPIO_Mode 		= GPIO_Mode_IPU;
  stGPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
  GPIO_Init(CAN_PORT, &stGPIO_InitStructure);
  
  /* Configure CAN pin: TX */
  stGPIO_InitStructure.GPIO_Pin 		= CAN_TX_PIN;
  stGPIO_InitStructure.GPIO_Mode 		= GPIO_Mode_AF_PP;
  stGPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
  GPIO_Init(CAN_PORT, &stGPIO_InitStructure);
  
  /* Enable CAN1 RX0 interrupt IRQ channel */
  stNVIC_InitStructure.NVIC_IRQChannel 										= USB_LP_CAN1_RX0_IRQn;
  stNVIC_InitStructure.NVIC_IRQChannelPreemptionPriority 	= 0;
  stNVIC_InitStructure.NVIC_IRQChannelSubPriority 				= 0;
  stNVIC_InitStructure.NVIC_IRQChannelCmd 								= ENABLE;
  NVIC_Init(&stNVIC_InitStructure); 
}
/******************************************************************************
 **@Function 			: CANMsg_Init
 **@Descriptions	: Config the CAN, transmit and receive using interrupt.
 										And CAN Filter Configuration
 **@Parameters		: None
 **@Return				: None
 ****************************************************************************/
void CANMsg_Init(void)
{
  CAN_InitTypeDef        stCAN_InitStructure;
  CAN_FilterInitTypeDef  stCAN_FilterInitStructure;
  
  /* CAN register init */
  CAN_DeInit(CAN1);
  CAN_StructInit(&stCAN_InitStructure);
  
  /* CAN cell init */
  stCAN_InitStructure.CAN_TTCM 		= DISABLE;
  stCAN_InitStructure.CAN_ABOM 		= DISABLE;
  stCAN_InitStructure.CAN_AWUM 		= DISABLE;
  stCAN_InitStructure.CAN_NART 		= DISABLE;
  stCAN_InitStructure.CAN_RFLM		= ENABLE;
  stCAN_InitStructure.CAN_TXFP 		= DISABLE;//ENABLE;
	#ifdef CAN_SILENT_LBK
		stCAN_InitStructure.CAN_Mode 	= CAN_Mode_Silent_LoopBack;
	#else
		stCAN_InitStructure.CAN_Mode 	= CAN_Mode_Normal;
	#endif
  
  /*Baudrate setting 125000bps*/
  stCAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
  stCAN_InitStructure.CAN_BS1 = CAN_BS1_10tq;
  stCAN_InitStructure.CAN_BS2 = CAN_BS2_7tq;
  stCAN_InitStructure.CAN_Prescaler = 16;/* As APB1 clock Changed to 18Mhz*/

  CAN_Init(CAN1, &stCAN_InitStructure);
  #ifdef CAN_SILENT_LBK
		/* CAN filter init, filter no=0 for ALL ID in Silent Loopback mode*/
		stCAN_FilterInitStructure.CAN_FilterNumber 					= 0;
		stCAN_FilterInitStructure.CAN_FilterMode 						= CAN_FilterMode_IdMask;
		stCAN_FilterInitStructure.CAN_FilterScale 					= CAN_FilterScale_32bit;
		stCAN_FilterInitStructure.CAN_FilterIdHigh 					= 0x0000;
		stCAN_FilterInitStructure.CAN_FilterIdLow 					= 0x0000; 
		stCAN_FilterInitStructure.CAN_FilterMaskIdHigh 			= 0x0000;
		stCAN_FilterInitStructure.CAN_FilterMaskIdLow 			= 0x0000;
		stCAN_FilterInitStructure.CAN_FilterFIFOAssignment 	= CAN_FIFO0;
		stCAN_FilterInitStructure.CAN_FilterActivation 			= ENABLE;
		CAN_FilterInit(&stCAN_FilterInitStructure);
	#else
		/* CAN filter init */ /*For filter no=0*/
		stCAN_FilterInitStructure.CAN_FilterNumber 					= 0;
		stCAN_FilterInitStructure.CAN_FilterMode 						= CAN_FilterMode_IdMask;
		stCAN_FilterInitStructure.CAN_FilterScale 					= CAN_FilterScale_32bit;
		stCAN_FilterInitStructure.CAN_FilterIdHigh 					= VCM_CAN_FILTERIDHIGH_0;
		stCAN_FilterInitStructure.CAN_FilterIdLow 					= VCM_CAN_FILTERIDLOW_0; 
		stCAN_FilterInitStructure.CAN_FilterMaskIdHigh 			= CAN_FILTERMASKIDHIGH_0;
		stCAN_FilterInitStructure.CAN_FilterMaskIdLow 			= CAN_FILTERMASKIDLOW_0;
		stCAN_FilterInitStructure.CAN_FilterFIFOAssignment 	= CAN_FIFO0;
		stCAN_FilterInitStructure.CAN_FilterActivation 			= ENABLE;
		CAN_FilterInit(&stCAN_FilterInitStructure);
		
		/* CAN filter init */ /*For filter no=1*/
		stCAN_FilterInitStructure.CAN_FilterNumber 					= 1;
		stCAN_FilterInitStructure.CAN_FilterMode 						= CAN_FilterMode_IdMask;
		stCAN_FilterInitStructure.CAN_FilterScale 					= CAN_FilterScale_32bit;
		stCAN_FilterInitStructure.CAN_FilterIdHigh 					= VCM_CAN_FILTERIDHIGH_1;
		stCAN_FilterInitStructure.CAN_FilterIdLow 					= VCM_CAN_FILTERIDLOW_1; 
		stCAN_FilterInitStructure.CAN_FilterMaskIdHigh 			= CAN_FILTERMASKIDHIGH_1;
		stCAN_FilterInitStructure.CAN_FilterMaskIdLow 			= CAN_FILTERMASKIDLOW_1;
		stCAN_FilterInitStructure.CAN_FilterFIFOAssignment 	= CAN_FIFO0;
		stCAN_FilterInitStructure.CAN_FilterActivation 			= ENABLE;
		CAN_FilterInit(&stCAN_FilterInitStructure);
  #endif
  /* CAN FIFO0 message pending interrupt enable */
  CAN_ITConfig(CAN1,CAN_IT_FMP0, ENABLE);
  
  CAN_ITConfig(CAN1,CAN_IT_FF0, ENABLE);
  CAN_ITConfig(CAN1,CAN_IT_FOV0, ENABLE);
  CAN_ITConfig(CAN1,CAN_IT_ERR, ENABLE);
  CAN_ITConfig(CAN1,CAN_IT_TME, ENABLE);
}
/******************************************************************************
 **@Function 			: fnCAN_Transmit
 **@Descriptions	: Sends frame on CAN bus
 **@Parameters		: pstCANrxmsg: pointer to a structure receive frame which 
										contains CAN Id, CAN DLC, CAN data and FMI number.
										CANx: where x can be 1 or 2 to select the CAN peripheral.
 **@Return				: None
 *****************************************************************************/
void fnCAN_Transmit(CanTxMsg *pstCANtxmsg)
{	
  CanTxMsg stTx1Msg;
  uint8_t ucCnt;             /*Count variable to fill Transmit Message Frame*/
  uint8_t sucMailBoxNumber;
	#ifdef	GENERAL_DBG
		uint8_t ucStatusCAN;
	#endif
  
  /*Configure CAN Transmitt Message structure*/
  stTx1Msg.StdId 	= pstCANtxmsg->StdId;
  stTx1Msg.ExtId 	= pstCANtxmsg->ExtId;
  stTx1Msg.IDE 		= CAN_ID_EXT;
  stTx1Msg.RTR 		= CAN_RTR_DATA;
  
  /* Reset Data Bufer */
  memset(stTx1Msg.Data, 0, sizeof(stTx1Msg.Data)); 
  
  /*Update Length of CAN Data Frame*/
  stTx1Msg.DLC = pstCANtxmsg->DLC;
  
  /*Update CAN Data Frame into Transmitt Structure*/
  for(ucCnt = 0; ucCnt < stTx1Msg.DLC; ucCnt++)
  {   
    stTx1Msg.Data[ucCnt] = pstCANtxmsg->Data[ucCnt];	
  }
  
	/*New Modification for Testing At CRC*/
	if(0x01 == CAN_TransmitStatus(CAN1, ucSendAckMBNo))
	{
		CAN_CancelTransmit(CAN1, sucMailBoxNumber);
	}
  /*Transmitt CAN message onto CAN bus*/
  sucMailBoxNumber = CAN_Transmit(CAN1, &stTx1Msg);
  
  StatusFlag.stStatusFlag.bTransmittingCAN = SET;
		
	#ifdef	GENERAL_DBG
		ucStatusCAN =	CAN_TransmitStatus(CAN1,sucMailBoxNumber);
		DEBUG_PRINTF("Transmit status = %d \r\n",ucStatusCAN);
  #endif
	
	#ifdef GENERAL_DBG	
		DEBUG_PRINTF("stTxMsg.ExtId = 0x%x \r\n",stTx1Msg.ExtId);
		DEBUG_PRINTF(" Tx\n\r");
	#endif		
  
}

/******************************************************************************
 **@Function 			: fnAck_Send
 **@Descriptions	: Sends acknowledgement frame on CAN bus
 **@Parameters		: pstCANrxmsg: pointer to a CAN structure, receive frame which
										contains CAN Id, CAN DLC, CAN data and FMI number.
 **@Return				: None
 ****************************************************************************/
void fnAck_Send(CanRxMsg *pstCANrxmsg)
{
  CanTxMsg stTxMsgACK;
  SplitByte.uliData = pstCANrxmsg->ExtId;
  /*Configure CAN Transmitt Message tructure*/
  stTxMsgACK.IDE = CAN_ID_EXT;
  stTxMsgACK.RTR = CAN_RTR_DATA;
	
  /* Reset Data Bufer */
  memset(stTxMsgACK.Data,0, sizeof(stTxMsgACK.Data)); 
  /*Update Length of CAN Data Frame*/
  stTxMsgACK.DLC = 1;
	if((RESET == StatusFlag.stStatusFlag.bFirstBootUp) && (HB_MSG == pstCANrxmsg->ExtId))
	{
		StatusFlag.stStatusFlag.bFirstBootUp = SET;
		stTxMsgACK.Data[0] = 0x02;		/* First hearbeat */
	}
	else
	{
		stTxMsgACK.Data[0] = 0x00;		/* Positive ACk */
	}
  SplitByte.rgucData[1] = SplitByte.rgucData[0];
  SplitByte.rgucData[0] = SOURCE_VCM;
  SplitByte.rgucData[2] = 0xE8;	
  stTxMsgACK.ExtId = SplitByte.uliData;
  
  /*Transmitt CAN message onto CAN bus*/
  ucSendAckMBNo = CAN_Transmit(CAN1, &stTxMsgACK);
}

/******************************************************************************
 **@Function 			: fnCheck_ACK
 **@Descriptions	: Sends acknowledgement frame on CAN bus
 **@Parameters		: pstCANrxmsg: pointer to a structure receive frame which
										contains CAN Id, CAN DLC, CAN data and FMI number.
 **@Return				: None
 ****************************************************************************/
void fnCheck_ACK(void)
{
  if(SET == StatusFlag.stStatusFlag.bTransmittingCAN)
  {
    if(RESET == StatusFlag.stStatusFlag.bACK_Received )
    {
      if(CAN_MSG_RESEND_TIME <= uiTxCounter)
      { 
				if(ucResendCount < CAN_MSG_RESEND_ATTEMPT)
        {
					uiTxCounter = 0;
          fnCAN_Transmit(&rgstTxCANBuff[ucTxBuffCnt]);
          ucResendCount++;
          
					#ifdef FIFO_DBG	
						DEBUG_PRINTF("Transmit again \n\r");
					#endif
        }
        else
        {
          StatusFlag.stStatusFlag.bTransmittingCAN = RESET;
          /*StatusFlag.stStatusFlag.bACK_Received = RESET;*/
          /* setting fault flag if MCU CAN link goes down*/
          if(DEST_SCU == (rgstTxCANBuff[ucTxBuffCnt].ExtId & DEST_ADDR_MASK))
          {
//						StatusFlag.stStatusFlag.bCANFaultSCU = SET;/* SCU CAN FAULT */
						#ifdef FIFO_DBG
							DEBUG_PRINTF("MCU  MESSAGE SEND FAULT");
						#endif
          }
					/* Setting Fault flag if OP CAN link goes down*/
          if(DEST_HMI == (rgstTxCANBuff[ucTxBuffCnt].ExtId & DEST_ADDR_MASK))
          {
//						StatusFlag.stStatusFlag.bCANFaultHMI = SET;/* SCU CAN FAULT */
						#ifdef FIFO_DBG
							DEBUG_PRINTF("OP  MESSAGE SEND FAULT");
						#endif
          }
          if((ucTxBuffCnt != ucStoreBuffCnt) || 
						 (SET == StatusFlag.stStatusFlag.bDataInCANTxBuffFlag))
          {
            fnShiftBuff();
          }
					
					/*ucLEDBlinkCount = 0;					*/
          ucResendCount = 0;
					#ifdef FIFO_DBG	
						DEBUG_PRINTF("Transmit Fail \n\r");
					#endif
        }
      }
    }
    else
    {
      StatusFlag.stStatusFlag.bTransmittingCAN = RESET;
      StatusFlag.stStatusFlag.bACK_Received 		= RESET;
      if((ucTxBuffCnt != ucStoreBuffCnt) ||
 				(SET ==StatusFlag.stStatusFlag.bDataInCANTxBuffFlag))
      {
        fnShiftBuff();
      }
      /*reset the counters*/
      uiTxCounter 	= 0;
      ucResendCount = 0;      
			#ifdef FIFO_DBG	
				DEBUG_PRINTF("Transmit Success \n\r");
			#endif
    }
  }
}
/******************************************************************************
 **@Function 			: fnCheck_HeartBeat
 **@Descriptions	: check flag for heartbeat message and SET/RESET respective
										flags
 **@Parameters		: None   
 **@Return				: None
 ****************************************************************************/
void fnCheck_HeartBeat(void)
{
  /*Heartbeat Fault check*/
  if(HEARTBEAT_TIMEOUT <= uiHBTimeCounter )
  {
    uiHBTimeCounter = 0;
		#ifdef GENERAL_DBG      
			DEBUG_PRINTF("HEartBEAt NOT received !!!!\r\n");
		#endif
		/*ucLEDBlinkCount = 0;*/
    StatusFlag.stStatusFlag.bCANFaultSCU = SET;
    if(RESET == StatusFlag.stStatusFlag.bHBNotRecv )
    {
      StatusFlag.stStatusFlag.bHBNotRecv = SET;
    }    
  }
}
/******************************************************************************
 **@Function		 	:  fnBuffer_Store
 **@Descriptions	:  Stores the CAN Structure in the Buffer
 **@Parameters		:  pstTxMsg: pointer to CAN Transmit structure
 **@Return				:  None
 ****************************************************************************/
void fnBuffer_Store(CanTxMsg *pstTxMsg)
{		
  memmove(&rgstTxCANBuff[ucStoreBuffCnt++], pstTxMsg, sizeof(CanTxMsg));
  if(TX_CAN_BUF_SIZE <= ucStoreBuffCnt )
  {
    ucStoreBuffCnt = 0;
		StatusFlag.stStatusFlag.bDataInCANTxBuffFlag = SET;
  }
	#ifdef FIFO_DBG
		DEBUG_PRINTF("in fnStoreBuff = %d\r\n",ucStoreBuffCnt);
	#endif
}
/******************************************************************************
 **@Function		 	:  fnShiftBuff
 **@Descriptions	:  Stores the CAN Structure in the Buffer
 **@Parameters		:  None
 **@Return				:  None
 ****************************************************************************/
void fnShiftBuff(void)
{
  memset(&rgstTxCANBuff[ucTxBuffCnt++], 0 , sizeof(CanTxMsg));
  if(ucTxBuffCnt >= TX_CAN_BUF_SIZE)
  {
    ucTxBuffCnt = 0;
		StatusFlag.stStatusFlag.bDataInCANTxBuffFlag = RESET;
  }
  if((ucTxBuffCnt != ucStoreBuffCnt) || 
		 (SET == StatusFlag.stStatusFlag.bDataInCANTxBuffFlag))
  {
		for(; ucTxBuffCnt <= ucStoreBuffCnt ; ucTxBuffCnt++)
		{
			if((((rgstTxCANBuff[ucTxBuffCnt].ExtId & DEST_ADDR_MASK) == DEST_SCU) &&
    			(StatusFlag.stStatusFlag.bCANFaultSCU == SET)) ||
				 (((rgstTxCANBuff[ucTxBuffCnt].ExtId & DEST_ADDR_MASK) == DEST_HMI) &&
     			(StatusFlag.stStatusFlag.bCANFaultHMI == SET)))
			{
				#ifdef	FIFO_DBG
					DEBUG_PRINTF("Message Cleared At = %d \r\r %x\r\n",
												ucTxBuffCnt,rgstTxCANBuff[ucTxBuffCnt].ExtId);
				#endif
				memset(&rgstTxCANBuff[ucTxBuffCnt], 0 , sizeof(CanTxMsg));
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
	#ifdef FIFO_DBG
		DEBUG_PRINTF("in fnShiftBuff = %d\r\n", ucTxBuffCnt);
	#endif
}
/******************************************************************************
 **@Function		 	:  fnValveFault_Send
 **@Descriptions	:  Send fault on CAN
 **@Parameters		:  None
 **@Return				:  None
 ****************************************************************************/
void fnValveFault_Send(void)
{
  stTxMsg.Data[0] = 0x01;
  stTxMsg.Data[1] = ucValveFault;
  stTxMsg.DLC 		= 2;
  if(RESET == StatusFlag.stStatusFlag.bCANFaultSCU )
  {
    /*send message to MCU*/
    stTxMsg.ExtId = MSG_TYPE_FAULT  | RESERVED | FAULT_STALL_VALVE |
										DEST_HMI | SOURCE_VCM;
    if((ucTxBuffCnt == ucStoreBuffCnt) && 
			 (RESET == StatusFlag.stStatusFlag.bDataInCANTxBuffFlag))
    {
      fnCAN_Transmit(&stTxMsg);
    }
    fnBuffer_Store(&stTxMsg);		
  }	
  if(RESET == StatusFlag.stStatusFlag.bCANFaultHMI)
  {
    /*send message to OP*/
    stTxMsg.ExtId = MSG_TYPE_FAULT  | RESERVED | FAULT_STALL_VALVE |
										DEST_HMI | SOURCE_VCM;
    
    if((ucTxBuffCnt == ucStoreBuffCnt) &&
			 (RESET ==StatusFlag.stStatusFlag.bDataInCANTxBuffFlag))
    {
      fnCAN_Transmit(&stTxMsg);
    }
    fnBuffer_Store(&stTxMsg);	
  }
  ucValveFault = 0x00;
}
/******************************************************************************
 **@Function 			: fnData_Send
 **@Descriptions	: Sends Data on CAN
 **@parameters		: ucDevice: it is device from which we want to send data.
 **@Return				: None
 ****************************************************************************/
void fnData_Send(uint32_t uliDataId)
{
	if(StatusFlag.stStatusFlag.bCANFaultSCU == RESET)
	{
		/*send message to MCU*/
		stTxMsg.ExtId = (MSG_TYPE_DATA | RESERVED | uliDataId | DEST_SCU |
										 SOURCE_VCM);
		if((ucTxBuffCnt == ucStoreBuffCnt) &&
			 (RESET ==StatusFlag.stStatusFlag.bDataInCANTxBuffFlag))
		{
			fnCAN_Transmit(&stTxMsg);
		}
		fnBuffer_Store(&stTxMsg);		
	}
	if(StatusFlag.stStatusFlag.bCANFaultHMI == RESET)
	{
		/*send message to OP*/
		stTxMsg.ExtId = (MSG_TYPE_DATA | RESERVED | uliDataId | DEST_HMI |
										 SOURCE_VCM);
		
		if((ucTxBuffCnt == ucStoreBuffCnt) &&
			 (RESET ==StatusFlag.stStatusFlag.bDataInCANTxBuffFlag))
		{
			fnCAN_Transmit(&stTxMsg);
		}
		fnBuffer_Store(&stTxMsg);	
	}
}
/******************************************************************************
 **@Function 			: fnFault_Send
 **@Descriptions	: Sends fault on CAN
 **@parameters		: ucFaultId: Its a fault message id.
 **@Return				: None
 ****************************************************************************/
void fnFault_Send(uint32_t uliFaultId)
{
	if(RESET == StatusFlag.stStatusFlag.bCANFaultSCU )
	{
		/*send message to MCU*/
		stTxMsg.ExtId = (MSG_TYPE_FAULT | RESERVED | uliFaultId| DEST_SCU |
										 SOURCE_VCM);
		if((ucTxBuffCnt == ucStoreBuffCnt) && 
			 (RESET == StatusFlag.stStatusFlag.bDataInCANTxBuffFlag))
		{
			fnCAN_Transmit(&stTxMsg);
		}
		fnBuffer_Store(&stTxMsg);		
	}
	if( RESET == StatusFlag.stStatusFlag.bCANFaultHMI)
	{
		/*send message to OP*/
		stTxMsg.ExtId = (MSG_TYPE_FAULT | RESERVED | uliFaultId| DEST_HMI |
										 SOURCE_VCM);
		if((ucTxBuffCnt == ucStoreBuffCnt) &&
			 (RESET ==StatusFlag.stStatusFlag.bDataInCANTxBuffFlag))
		{
			fnCAN_Transmit(&stTxMsg);
		}
		fnBuffer_Store(&stTxMsg);	
	}
}
/******************************************************************************
 **@Function 			: fnStatus_Send
 **@Descriptions	: Sends status message on CAN
 **@parameters		: ucStatusId: Status message id.
 **@Return				: None
 ****************************************************************************/
void fnStatus_Send(uint32_t uliStatusId)
{
	if(RESET == StatusFlag.stStatusFlag.bCANFaultSCU )
	{
		/*send message to MCU*/
		stTxMsg.ExtId = (MSG_TYPE_STATUS | uliStatusId | RESERVED | DEST_SCU |
										 SOURCE_VCM);
		
		if((ucTxBuffCnt == ucStoreBuffCnt) && 
			 (RESET == StatusFlag.stStatusFlag.bDataInCANTxBuffFlag))
		{
			fnCAN_Transmit(&stTxMsg);
		}
		fnBuffer_Store(&stTxMsg);		
	}
	if(RESET == StatusFlag.stStatusFlag.bCANFaultHMI )
	{
		/*send message to OP*/
		stTxMsg.ExtId = (MSG_TYPE_STATUS | uliStatusId | RESERVED | DEST_HMI |
										 SOURCE_VCM);
		
		if((ucTxBuffCnt == ucStoreBuffCnt) &&
			 (RESET == StatusFlag.stStatusFlag.bDataInCANTxBuffFlag))
		{
			fnCAN_Transmit(&stTxMsg);
		}
		fnBuffer_Store(&stTxMsg);	
	}
}
/******************************************************************************
 **@Function 			: fnStallData_Send 
 **@Descriptions	: Sends data for the stall test CAN
 **@parameters		: None.
 **@Return				: None
 ****************************************************************************/
void fnStallData_Send(void)
{	
	if(RESET == StatusFlag.stStatusFlag.bCANFaultSCU )
	{
		/*send message to MCU*/
		stTxMsg.ExtId = (MSG_TYPE_DATA | RESERVED | DATA_STALL_PRESSURE_CHANGE |
										 DEST_SCU | SOURCE_VCM);
		if((ucTxBuffCnt == ucStoreBuffCnt) && 
			 (RESET == StatusFlag.stStatusFlag.bDataInCANTxBuffFlag))
		{
			fnCAN_Transmit(&stTxMsg);
		}
		fnBuffer_Store(&stTxMsg);		
	}
	if( RESET == StatusFlag.stStatusFlag.bCANFaultHMI)
	{
		/*send message to OP*/
		stTxMsg.ExtId = (MSG_TYPE_DATA |  RESERVED | DATA_STALL_PRESSURE_CHANGE |
										 DEST_HMI | SOURCE_VCM);
		if((ucTxBuffCnt == ucStoreBuffCnt) && 
			 (RESET ==StatusFlag.stStatusFlag.bDataInCANTxBuffFlag))
		{
			fnCAN_Transmit(&stTxMsg);
		}
		fnBuffer_Store(&stTxMsg);	
	}
}


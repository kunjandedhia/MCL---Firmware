/********************************************************************************
  * @file    CAN.c
  * 
  * @brief   This file contain all the CAN Related Funtions such as CAN Transmit 
						 Recieved and Ack etc.
  ******************************************************************************
  */  
/* -----------------------------------------------------------------------------
 *                           Includes
 * ----------------------------------------------------------------------------
 */
#include "Includes.h"
/* -----------------------------------------------------------------------------
 *                           MACROS
 * ----------------------------------------------------------------------------
 */
/* -----------------------------------------------------------------------------
 *                           CONSTANTS
 * ----------------------------------------------------------------------------
 */
/* -----------------------------------------------------------------------------
 *                           GLOBAL VARIABLES
 * ----------------------------------------------------------------------------
 */
CanTxMsg stTxMsg;  /*CAN1 Tx msg */
CanTxMsg rgstTxCANBuff[TX_CAN_BUF_SIZE] = {0};
CanRxMsg rgstRxCANBuff[RX_CAN_BUF_SIZE] = {0};
uint8_t ucStoreRxCANBuffCnt 						= 0;
uint8_t ucRxCANProcessCnt 							= 0;
uint8_t ucSendAckMBNo 									= 0;
uint8_t ucTxBuffCnt											= 0;
uint8_t ucStoreBuffCnt									= 0;
uint16_t uiHBTimeCounter								=  0;
uint32_t uliConfigParaByte							= 0;
uint16_t uiTxCounter										= 0;
#ifdef TEST_DBG
uint32_t uliCANTxCount									= 0;
uint32_t uliCANRxCount									= 0;
#endif

/* -----------------------------------------------------------------------------
 *                           EXTERNAL VARIABLES
 * ----------------------------------------------------------------------------
 */
/* -----------------------------------------------------------------------------
 *                           EXTERNAL FUNCTIONS
 * ----------------------------------------------------------------------------
 */
/* -----------------------------------------------------------------------------
 *                           FUNCTIONS DECLARATION
 * ----------------------------------------------------------------------------
 */
/*******************************************************************************
 **@Function 			:  CAN_Configuration
 **@Descriptions	:  Configures the CAN bus
 **@Parameters		:  None
 **@Return				:  None
 *****************************************************************************/
void fnCAN_Config(void)
{
	GPIO_InitTypeDef stGPIO_InitStructure;
	NVIC_InitTypeDef stNVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2PERIPH_GPIO, ENABLE);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, DISABLE);

	/* CAN1 Periph clock enable */
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_CAN1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

	/* Configure CAN pin: RX */
	stGPIO_InitStructure.GPIO_Pin = CAN_RX;
	stGPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	stGPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(CAN_GPIO, &stGPIO_InitStructure);

	/* Configure CAN pin: TX */
	stGPIO_InitStructure.GPIO_Pin = CAN_TX;
	stGPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	stGPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(CAN_GPIO, &stGPIO_InitStructure);

	/* Enable CAN1 RX0 interrupt IRQ channel */
	/* CAN1_RX0_IRQn; */
	stNVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
	stNVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	stNVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	stNVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&stNVIC_InitStructure);
}
/*******************************************************************************
 **@Function 			: fnCAN_MsgInit
 **@Descriptions	: Configures the CAN msg receive filter and interrupt.
 **@Parameters		: None
 **@Return				: None
 *****************************************************************************/
void fnCAN_MsgInit(void)
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
	#ifdef CAN_SILENT_LBK
		stCAN_InitStructure.CAN_Mode = CAN_Mode_Silent_LoopBack;
	#else
		stCAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
	#endif
	/* Baudrate setting 125000bps */
	stCAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
	stCAN_InitStructure.CAN_BS1 = CAN_BS1_10tq;
	stCAN_InitStructure.CAN_BS2 = CAN_BS2_7tq;
	stCAN_InitStructure.CAN_Prescaler = 16;
	
	/* initializes the CAN */
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
	
		/* CAN filter init, filter for HCM Destination */
		stCAN_FilterInitStructure.CAN_FilterNumber 					= 0;
		stCAN_FilterInitStructure.CAN_FilterMode 						= CAN_FilterMode_IdMask;
		stCAN_FilterInitStructure.CAN_FilterScale 					= CAN_FilterScale_32bit;
		stCAN_FilterInitStructure.CAN_FilterIdHigh 					= HCM_CAN_FILTERIDHIGH_0;
		stCAN_FilterInitStructure.CAN_FilterIdLow 					= HCM_CAN_FILTERIDLOW_0; 
		stCAN_FilterInitStructure.CAN_FilterMaskIdHigh 			= CAN_FILTERMASKIDHIGH_0;
		stCAN_FilterInitStructure.CAN_FilterMaskIdLow 			= CAN_FILTERMASKIDLOW_0;
		stCAN_FilterInitStructure.CAN_FilterFIFOAssignment 	= CAN_FIFO0;
		stCAN_FilterInitStructure.CAN_FilterActivation 			= ENABLE;
		CAN_FilterInit(&stCAN_FilterInitStructure);
		
		/* CAN filter init, filter no = 1 for BooadCast message*/
		stCAN_FilterInitStructure.CAN_FilterNumber 					= 1;
		stCAN_FilterInitStructure.CAN_FilterMode 						= CAN_FilterMode_IdMask;
		stCAN_FilterInitStructure.CAN_FilterScale 					= CAN_FilterScale_32bit;
		stCAN_FilterInitStructure.CAN_FilterIdHigh 					= HCM_CAN_FILTERIDHIGH_1;
		stCAN_FilterInitStructure.CAN_FilterIdLow 					= HCM_CAN_FILTERIDLOW_1; 
		stCAN_FilterInitStructure.CAN_FilterMaskIdHigh 			= CAN_FILTERMASKIDHIGH_1;
		stCAN_FilterInitStructure.CAN_FilterMaskIdLow 			= CAN_FILTERMASKIDLOW_1;
		stCAN_FilterInitStructure.CAN_FilterFIFOAssignment 	= CAN_FIFO0;
		stCAN_FilterInitStructure.CAN_FilterActivation 			= ENABLE;
		CAN_FilterInit(&stCAN_FilterInitStructure);
		
	#endif
	/* CAN FIFO0 message pending interrupt enable */
	CAN_ITConfig(CAN1,CAN_IT_FMP0, ENABLE);
	
	/* CAN_ITConfig(CAN1,CAN_IT_FMP1, ENABLE); */
	CAN_ITConfig(CAN1,CAN_IT_FF0,  ENABLE);
	CAN_ITConfig(CAN1,CAN_IT_FOV0, ENABLE);
	CAN_ITConfig(CAN1,CAN_IT_ERR,  ENABLE);
	CAN_ITConfig(CAN1,CAN_IT_TME,  ENABLE);
}
/*******************************************************************************
 **@Function 			: fnCAN_Transmit
 **@Descriptions	: Sends Response frame on CAN bus
 **@Parameters		: pstCANrxmsg: pointer to Receive CAN structure.
										CANx: where x can be 1 or 2 to select the CAN peripheral.
 **@Return				: None
******************************************************************************/
void fnCAN_Transmit(CanTxMsg *pstCANTxmsg, CAN_TypeDef* CANx)
{
	CanTxMsg stCANTransmitMsg;
	static uint8_t sucMailBoxNo = 0;
	uint8_t ucCount;   /*Count variable to fill Transmit Message Frame*/
	
	/*Configure CAN Transmitt Message tructure*/
	stCANTransmitMsg.StdId = pstCANTxmsg->StdId;
	stCANTransmitMsg.ExtId = pstCANTxmsg->ExtId; 
	stCANTransmitMsg.IDE = CAN_ID_EXT;
	stCANTransmitMsg.RTR = CAN_RTR_DATA;
	
	/* Reset Data Bufer */
	memset(stCANTransmitMsg.Data,0, sizeof(stCANTransmitMsg.Data)); 
	
	/*Update Length of CAN Data Frame*/
	stCANTransmitMsg.DLC = pstCANTxmsg->DLC;
	
	/*Update CAN Data Frame into Transmitt Structure*/
	for(ucCount = 0; ucCount < stCANTransmitMsg.DLC; ucCount++)
	{
		stCANTransmitMsg.Data[ucCount] = pstCANTxmsg->Data[ucCount];
	}
	/*Cancel Msg only if its not an ack msg which is pending*/
	/*if ACK msg is under procces then dont cancel Mailbox*/
	if(CANTXOK == CAN_TransmitStatus(CAN1,ucSendAckMBNo))
	{
		/*Transmitt CAN message onto CAN bus*/
		CAN_CancelTransmit(CAN1, sucMailBoxNo);
	}
	sucMailBoxNo = CAN_Transmit(CAN1, &stCANTransmitMsg);
	StatusFlag.stStatusFlag.bTxFlag = SET;
	/*******************/
	#ifdef TEST_DBG	
		uliCANTxCount++;
		DEBUG_PRINTF("in CAN TX = 0x%08x Tx Cnt: %d\r\n",stCANTransmitMsg.ExtId,
								 uliCANTxCount);
	#endif		
}
/*******************************************************************************
 **@Function 			: 	fnACK_Send
 **@Descriptions	:  	Sends acknowledgement frame on CAN bus
 **@Parameters		:   pstCANrxmsg: pointer to a Receive CAN structure. 
 **@Return				:   None
 *****************************************************************************/
void fnACK_Send(CanRxMsg *pstCANrxmsg)
{
	CanTxMsg stTxAckCan;
	SplitByte.uliData = pstCANrxmsg->ExtId;
	
	/*Configure CAN Transmitt Message tructure*/
	stTxAckCan.IDE = CAN_ID_EXT;
	stTxAckCan.RTR = CAN_RTR_DATA;
	/* Reset Data Bufer */
	memset(stTxAckCan.Data,0, sizeof(stTxAckCan.Data)); 
	
	if((RESET == StatusFlag.stStatusFlag.bFirstBootup) && 
		 (0x0002FF02 == pstCANrxmsg->ExtId)) 
	{
		stTxAckCan.Data[0] = FIRST_BOOTUP ; 
		/*it will stop sending the First bootup acknowledge to SCU*/
		StatusFlag.stStatusFlag.bFirstBootup = SET; 
	}
	else 	
	{
		stTxAckCan.Data[0] = POSITIVE_ACK;
	}
	/*Update Length of CAN Data Frame*/
	stTxAckCan.DLC = 1;
	SplitByte.rgucData[2] = 0xE8; /* acknowledge */	
	SplitByte.rgucData[1] = SplitByte.rgucData[0];
	SplitByte.rgucData[0] = SOURCE_HCM;
	stTxAckCan.ExtId = SplitByte.uliData;
	#ifdef GENERAL_DBG
		DEBUG_PRINTF("ACK MESSAGE: %x \n\r",stTxAckCan.ExtId);
	#endif
	/*Transmitt CAN message onto CAN bus*/
	ucSendAckMBNo = CAN_Transmit(CAN1, &stTxAckCan);
}
/*******************************************************************************
 **@Function 			:	 fnDataMsg_Tx
 **@Descriptions	:  Forward the current Data Msgs to SCU and HMI
 **@Parameters		:	 fData : Temperature or Liquid level data (in float) 
										 ucDataType: type of data i.e liquid level or Temperature
										 ucDataId: Data Id i.e BASE, HARDNER, INLINE_BASE,
															 INLINE_HARD or ANTIFREEZ
 **@Return				:  None
 *****************************************************************************/
void fnDataMsg_Tx(float fData, uint8_t ucDataType, uint8_t ucDataId)
{
	uint8_t ucLoop;
	memset(stTxMsg.Data, 0, sizeof(stTxMsg.Data));
	stTxMsg.DLC = 4;
	stTxMsg.ExtId = 0;
	SplitByte.fData = fData;
	
	for(ucLoop = 0; ucLoop < stTxMsg.DLC; ucLoop++)
	{
		 stTxMsg.Data[ucLoop] = SplitByte.rgucData[ucLoop];
	}
	stTxMsg.ExtId = RESERVED | MSG_TYPE_DATA | SOURCE_HCM;
	if(LIQUID_LEVEL == ucDataType)
	{
		if(BASE == ucDataId)
		{
			stTxMsg.ExtId |= DATA_BASE_MATERIAL_LVL;
		}
		if(HARDNER == ucDataId)
		{
			stTxMsg.ExtId |= DATA_HARD_MATERIAL_LVL;
		}
	}
	else if(TEMPERATURE == ucDataType)
	{
		if(INLINE_BASE == ucDataId)
		{
			stTxMsg.ExtId |= DATA_INLINE_BASE_TEMP;
		}
		else if(INLINE_HARD == ucDataId)
		{
			stTxMsg.ExtId |= DATA_INLINE_HARD_TEMP;
		}
		else if(ANTIFREEZ == ucDataId)
		{
			stTxMsg.ExtId |= DATA_ANTIFREEZ_TEMP;
		}
		else if(RETURN_LINE == ucDataId)
		{
			stTxMsg.ExtId |= DATA_RETURN_BASE_TEMP;
		}
	}
	fnCANMessage_Send(&stTxMsg);
}
/*******************************************************************************
 **@Function 			:	 fnFaultMsg_Tx
 **@Descriptions	:  Sends the fault messages to the SCU and HMI 
 **@Parameters		:	 ucFault: above or below thershold fault 
										 ucFaultType: liquid level or Tempmerature fault
										 ucDataId: Data Id i.e BASE, HARDNER, INLINE_BASE,
															 INLINE_HARD or ANTIFREEZ 
 **@Return				:  None
 *****************************************************************************/
void fnFaultMsg_Tx(uint8_t ucFault, uint8_t ucFaultType, uint8_t ucDataId)
{
	memset(stTxMsg.Data,0,sizeof(stTxMsg.Data));
	stTxMsg.DLC = 2;	
	stTxMsg.Data[1] = ucFault;

	stTxMsg.ExtId = RESERVED | MSG_TYPE_FAULT | SOURCE_HCM;
	if(LIQUID_LEVEL == ucFaultType)
	{
		stTxMsg.ExtId |= FAULT_LQ_LV_BEYOND_TH;
		
		if(BASE == ucDataId)
		{
			stTxMsg.Data[0] = BASE_TANK;
		}
		if(HARDNER == ucDataId)
		{
			stTxMsg.Data[0] = HARD_TANK;
		}
	}
	else if(TEMPERATURE == ucFaultType)
	{
		stTxMsg.ExtId |= FAULT_TEMP_BEYOND_TH; 
		
		if(INLINE_BASE == ucDataId)
		{
			stTxMsg.Data[0] = INLINE_BASE_FAULT;
		}
		else if(INLINE_HARD == ucDataId)
		{
			stTxMsg.Data[0] = INLINE_HARD_FAULT;
		}
		else if(ANTIFREEZ == ucDataId)
		{
			stTxMsg.Data[0] = ANTIFREEZ_FAULT;
		}
	}
	fnCANMessage_Send(&stTxMsg);
}
/*******************************************************************************
 **@Function		 	:  fnBuffer_Store
 **@Descriptions	:  Stores the CAN Structure in the Buffer
 **@Parameters		:  pstTxMsg: pointer to CAN Transmit structure
 **@Return				:  None
 *****************************************************************************/
void fnBuffer_Store(CanTxMsg *pstTxMsg)
{	
	#ifdef GENERAL_DBG
		DEBUG_PRINTF("in fnStoreBuff = %d 0x%x\r\n", ucStoreBuffCnt,
								 pstTxMsg->ExtId);
	#endif
	memmove(&rgstTxCANBuff[ucStoreBuffCnt++], pstTxMsg, sizeof(CanTxMsg));
	if(ucStoreBuffCnt > TX_CAN_BUF_SIZE)
	{
		ucStoreBuffCnt = 0;
		StatusFlag.stStatusFlag.bDataInCANTxBuffFlag = SET;
	}
}
/*******************************************************************************
 **@Function		 	:  fnBuffer_Shift
 **@Descriptions	:  Shift the Buffer when the ack received or the msg is
										 discarded and transmit the next frame in the buffer
 **@Parameters		:  None
 **@Return				:  None
 *****************************************************************************/
void fnBuffer_Shift(void)
{
	memset(&rgstTxCANBuff[ucTxBuffCnt++], 0 , sizeof(CanTxMsg));
	
	if(ucTxBuffCnt > TX_CAN_BUF_SIZE)
	{
		ucTxBuffCnt = 0;
		StatusFlag.stStatusFlag.bDataInCANTxBuffFlag = RESET;
	}
	#ifdef GENERAL_DBG
		DEBUG_PRINTF("in fnBuffer_Shift TxCNT = %d\r\n", ucTxBuffCnt);
	#endif
	if((ucTxBuffCnt != ucStoreBuffCnt ) || 
		(SET == StatusFlag.stStatusFlag.bDataInCANTxBuffFlag))
	{
		for(; ucTxBuffCnt <= ucStoreBuffCnt ; ucTxBuffCnt++)
		{
			/* Empty the CAN Transmit Buffer if the respective 
				 Destination CAN Fault occurs*/
			if(((DEST_HMI == (rgstTxCANBuff[ucTxBuffCnt].ExtId & DEST_ADDR_MASK)) &&
				 (SET == StatusFlag.stStatusFlag.bCANFaultHMI)) ||  
				 ((DEST_SCU == (rgstTxCANBuff[ucTxBuffCnt].ExtId & DEST_ADDR_MASK)) && 
				  (SET == StatusFlag.stStatusFlag.bCANFaultSCU)))
			{
				memset(&rgstTxCANBuff[ucTxBuffCnt], 0 , sizeof(CanTxMsg));
				#ifdef GENERAL_DBG
					DEBUG_PRINTF("in emptying Buff, Txcnt = %d storebuff = %d \r\n", 
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
			fnCAN_Transmit(&rgstTxCANBuff[ucTxBuffCnt], CAN1);
		}
	}
}
/*******************************************************************************
 **@Function 			: fnACK_Check
 **@Descriptions	: checks the acknowledgement and resend the previously send 
										Frame again if no ACK received, only 3 time
 **@Parameters		: None
 **@Return				: None
 *****************************************************************************/
void fnACK_Check(void)
{
	static uint8_t sucResendCount = 0;
	
	if(SET == StatusFlag.stStatusFlag.bTxFlag)
	{
		/*No ACK RCV*/
		if(RESET == StatusFlag.stStatusFlag.bACKRxFlag) 
		{
			if(uiTxCounter >= CAN_MSG_RESEND_TIME)/* resend time check */
			{
				if(sucResendCount < CAN_MSG_RESEND_ATTEMPT)/* resend attempt check */
				{
					uiTxCounter = 0;
					sucResendCount++;
					
					fnCAN_Transmit(&rgstTxCANBuff[ucTxBuffCnt], CAN1);
					#ifdef ACK_CHECK_DBG	
						DEBUG_PRINTF("Transmit again\r\n");
					#endif
				}
				else
				{
					StatusFlag.stStatusFlag.bTxFlag = RESET;	
					/*StatusFlag.stStatusFlag.bErrorFlag = SET;*/ /* Error */
					
					uiTxCounter = 0;
					sucResendCount = 0;
					
					if(DEST_SCU == (rgstTxCANBuff[ucTxBuffCnt].ExtId & DEST_ADDR_MASK))
					{
						StatusFlag.stStatusFlag.bCANFaultSCU = SET;/* SCU CAN FAULT */
					}
					
					else if(DEST_HMI == (rgstTxCANBuff[ucTxBuffCnt].ExtId & DEST_ADDR_MASK))
					{
						StatusFlag.stStatusFlag.bCANFaultHMI = SET;/* HMI CAN FAULT */
					}

					if((ucTxBuffCnt != ucStoreBuffCnt ) || 
						(SET == StatusFlag.stStatusFlag.bDataInCANTxBuffFlag))
					{
						fnBuffer_Shift();	
					}
					#ifdef ACK_CHECK_DBG	
						DEBUG_PRINTF("Transmit Fail\r\n");
					#endif
				}
			}
		}
		else/* ACK Recieved*/
		{
			StatusFlag.stStatusFlag.bTxFlag = RESET;
			StatusFlag.stStatusFlag.bACKRxFlag = RESET;	
			uiTxCounter = 0;
			sucResendCount = 0;
			if((ucTxBuffCnt != ucStoreBuffCnt ) || 
				(SET == StatusFlag.stStatusFlag.bDataInCANTxBuffFlag))
			{
				fnBuffer_Shift();
			}	
			#ifdef ACK_CHECK_DBG	
				DEBUG_PRINTF(" Transmit success\r\n");
			#endif
		}
	}
}
/*******************************************************************************
 **@Function 			: fnCANMessage_Send
 **@Descriptions	: Send CAN Message to SCU AND HMI both or Store in FIFO
 **@Parameters		: pstCANTxmsg: Pointer to CAN Transmit Structure
 **@Return				: None
 *****************************************************************************/
void fnCANMessage_Send(CanTxMsg *pstCANTxmsg)
{
	if(RESET == StatusFlag.stStatusFlag.bCANFaultSCU)/* if no MCU CAN fault */
	{
		pstCANTxmsg->ExtId |= DEST_SCU;
		if((ucTxBuffCnt == ucStoreBuffCnt) && 
			 (RESET == StatusFlag.stStatusFlag.bDataInCANTxBuffFlag))
		{
			 fnCAN_Transmit(pstCANTxmsg, CAN1);	
		}
		fnBuffer_Store(pstCANTxmsg);/* Store the Tx structure in the Buffer */
	}
	
	if(RESET == StatusFlag.stStatusFlag.bCANFaultHMI)	/* if no OP CAN fault */
	{			
		pstCANTxmsg->ExtId =  ((pstCANTxmsg->ExtId & DEST_ADDR_CLEAR_MASK) | DEST_HMI)  ;
		if((ucTxBuffCnt == ucStoreBuffCnt) && 
			 (RESET == StatusFlag.stStatusFlag.bDataInCANTxBuffFlag))
		{
			fnCAN_Transmit(pstCANTxmsg, CAN1);
		}
		fnBuffer_Store(pstCANTxmsg);/* Store the Tx structure in the Buffer */
	}
}


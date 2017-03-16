/******************************************************************************
**@file     :   CAN.c
*
**@brief    :   This file contains the CAN functions 
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
CAN_InitTypeDef        stCAN_InitStructure;
CAN_FilterInitTypeDef  stCAN_FilterInitStructure;




uint8_t 	ucHBCntr;
uint16_t 	uiHBACKfail;
uint16_t	uiHBBakACKfail =0;
uint8_t 	rgucHBCntr[TOTAL_HB_DEVICE] = {0};
uint8_t 	rgucHBFail[2] = {0};
uint16_t 	uiHBFailSend;
uint8_t 	ucLoopCnt;
uint8_t 	ucCANLinkFaultCnt;
uint8_t 	ucUpstrokeStallTestResult=0;
uint8_t 	ucDownstrokeStallTestResult=0;
uint8_t 	ucMaterialDataReceived=0;

uint8_t 	ucUpstrokeStallTestReceived=0;  /*change71*/
uint8_t 	ucDownstrokeStallTestReceived=0; /*change71*/
uint8_t 	ucStallTestCount=0;  /*change72*/
CanRxMsg rgstRxCANBuff[RX_CAN_BUF_SIZE];
uint16_t uiStoreRxCANBuffCnt;
uint16_t uiCANProcessCnt;
/*Union Variable Used for Byte splitting or conversion*/
SplitByte_TypeDef SplitByte;
/*Structure Variable for Current Data*/
Typedef_CurrData stCurrentData;
/*Structure Variable for configuration Parameter*/
TypeDef_Config stConfigData;

TypeDef_Status StatusFlag;



//CAN_TypeDef* OPMM_ACTIVE_CAN;/*CAN bus to which current msg is to be tranmitted*/
CanTxMsg stTx1Message;

/*OP Device related Flags */
uint8_t ucMsgTxCnt_HMI;      /*msg transmit counter*/
uint32_t uliTimeoutCnt_HMI;  /*250ms timeout counter*/
uint8_t ucTimerEn_HMI;       /*Timeout Counter enable*/
uint8_t ucDevDead_HMI;       /*Device dead signal*/
stTxMsgFlags stFifo_HMI;    /*FIFO Structure*/
CanTxMsg stCurMsgTrans_HMI; /*MsgTrans - current message under transmission*/

/*OPMM Device related Flags */
uint8_t ucMsgTxCnt_OPMM;      /*msg transmit counter*/
uint32_t uliTimeoutCnt_OPMM;  /*250ms timeout counter*/
uint8_t ucTimerEn_OPMM;       /*Timeout Counter enable*/
uint8_t ucDevDead_OPMM;       /*Device dead signal*/
stTxMsgFlags stFifo_OPMM;    /*FIFO Structure*/
CanTxMsg stCurMsgTrans_OPMM; /*MsgTrans - current message under transmission*/

/*MFM Device related Flags */
uint8_t ucMsgTxCnt_VCM;      /*msg transmit counter*/
uint32_t uliTimeoutCnt_VCM;  /*250ms timeout counter*/
uint8_t ucTimerEn_VCM;       /*Timeout Counter enable*/
uint8_t ucDevDead_VCM;       /*Device dead signal*/
stTxMsgFlags stFifo_VCM;    /*FIFO Structure*/
CanTxMsg stCurMsgTrans_VCM; /*MsgTrans - current message under transmission*/


/*HCM Device related Flags */
uint8_t ucMsgTxCnt_HCM;      /*msg transmit counter*/
uint32_t uliTimeoutCnt_HCM;  /*250ms timeout counter*/
uint8_t ucTimerEn_HCM;       /*Timeout Counter enable*/
uint8_t ucDevDead_HCM;       /*Device dead signal*/
stTxMsgFlags stFifo_HCM;    /*FIFO Structure*/
CanTxMsg stCurMsgTrans_HCM; /*MsgTrans - current message under transmission*/



/*****************************************************************************
**@Function      : fnCAN_Config
**@Descriptions  : Configures the CAN Peripheral.
**@parameters    : CANx: where x can be 1 or 2 to select the CAN peripheral.
**@Return        : None
*****************************************************************************/
void fnCAN_Config(CAN_TypeDef* CANx)
{
  GPIO_InitTypeDef  stGPIO_InitStructure;
  NVIC_InitTypeDef  stNVIC_InitStructure;
  /******************* CAN GPIOs configuration ******************/
  /* Enable GPIO clock And Error Pin Config*/
  RCC_AHB1PeriphClockCmd(	RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB |
                         RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD |
                           RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOF |
                             RCC_AHB1Periph_GPIOI | RCC_AHB1Periph_GPIOH | RCC_AHB1Periph_GPIOG,
                             ENABLE);
  /* Configure PH12 output mode for CAN1 Error pin*/
  stGPIO_InitStructure.GPIO_Pin 	= CAN1_ERROR_PIN;
  stGPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_IN;
  stGPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_NOPULL;
  stGPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(CAN1_ERROR_PORT, &stGPIO_InitStructure);
  
  /* Configure PE2 output mode for CAN2 Error pin*/
  stGPIO_InitStructure.GPIO_Pin 	= CAN2_ERROR_PIN;
  GPIO_Init(CAN2_ERROR_PORT, &stGPIO_InitStructure);
  
  /* Configure STROBE output for CAN1 & 2 */
  stGPIO_InitStructure.GPIO_Pin 	= CAN1_STROBE_PIN;
  stGPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_OUT;
  stGPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_NOPULL;
  stGPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(CAN1_STROBE_PORT, &stGPIO_InitStructure);
  
  stGPIO_InitStructure.GPIO_Pin 	= CAN2_STROBE_PIN;
  GPIO_Init(CAN2_STROBE_PORT, &stGPIO_InitStructure);
  
  /* Configure ENABLE output for CAN1 & 2 */
  stGPIO_InitStructure.GPIO_Pin 	= CAN1_ENABLE_PIN;
  stGPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_OUT;
  stGPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_NOPULL;
  stGPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(CAN1_ENABLE_PORT, &stGPIO_InitStructure);
  
  stGPIO_InitStructure.GPIO_Pin 	= CAN2_ENABLE_PIN;
  GPIO_Init(CAN2_ENABLE_PORT, &stGPIO_InitStructure);
  
  /*Initialize pins for CAN1 */
  if(CANx == CAN1)
  {
    /* Enable CAN 1clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
    
    /*PI9 as CAN1_RX & PH13 as CAN1_TX */
    GPIO_PinAFConfig(GPIOI, GPIO_PinSource9, 	GPIO_AF_CAN1);
    GPIO_PinAFConfig(GPIOH, GPIO_PinSource13, GPIO_AF_CAN1);
    
    /* Configure CAN1 RX */
    stGPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_9 ;
    stGPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF;
    stGPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    stGPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    stGPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(GPIOI, &stGPIO_InitStructure);
    
    /* Configure CAN1 TX */
    stGPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_13;
    GPIO_Init(GPIOH, &stGPIO_InitStructure);
  }
  /*Initialize pins for CAN2 */
  else if(CANx == CAN2)
  {
    /* Enable CAN2 clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);
    
    /*PB5 as CAN2_RX & PB6 as CAN2_TX */
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_CAN2);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_CAN2);
    
    /* Configure CAN RX and TX pins */
    stGPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_5 | GPIO_Pin_6;
    stGPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF;
    stGPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    stGPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    stGPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &stGPIO_InitStructure);
  }
  
  /* CAN register init */
  CAN_DeInit(CANx);
  
  /* CAN cell init */
  stCAN_InitStructure.CAN_TTCM = DISABLE;
  stCAN_InitStructure.CAN_ABOM = DISABLE;//DISABLE;
  stCAN_InitStructure.CAN_AWUM = DISABLE;
  stCAN_InitStructure.CAN_NART = DISABLE;
  stCAN_InitStructure.CAN_RFLM = ENABLE; //DISABLE;
  stCAN_InitStructure.CAN_TXFP = DISABLE;
  stCAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
  
  /* CAN Baudrate = 125Kbps (CAN clocked at 42 MHz) */
  stCAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
  stCAN_InitStructure.CAN_BS1 = CAN_BS1_12tq;
  stCAN_InitStructure.CAN_BS2 = CAN_BS2_8tq;
  stCAN_InitStructure.CAN_Prescaler = 16;
  CAN_Init(CANx, &stCAN_InitStructure);
  
  if(CANx == CAN1)
  {
    /* CAN filter init : No filter */
    stCAN_FilterInitStructure.CAN_FilterNumber = 0;
    stCAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
    stCAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
    stCAN_FilterInitStructure.CAN_FilterIdHigh = 0;
    stCAN_FilterInitStructure.CAN_FilterIdLow =0x0200;
    stCAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
    stCAN_FilterInitStructure.CAN_FilterMaskIdLow = 0xFF00;
    stCAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
    stCAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
    CAN_FilterInit(&stCAN_FilterInitStructure);
    
    /* CAN filter init : No filter */
    stCAN_FilterInitStructure.CAN_FilterNumber = 1;
    stCAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
    stCAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
    stCAN_FilterInitStructure.CAN_FilterIdHigh = 0;
    stCAN_FilterInitStructure.CAN_FilterIdLow =0xFF00;
    stCAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
    stCAN_FilterInitStructure.CAN_FilterMaskIdLow = 0xFF00;
    stCAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
    stCAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
    CAN_FilterInit(&stCAN_FilterInitStructure);
  }
  else if(CANx == CAN2)
  {
    CAN_SlaveStartBank(15);
    /* CAN filter init : No filter */
    stCAN_FilterInitStructure.CAN_FilterNumber = 16;
    stCAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
    stCAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
    stCAN_FilterInitStructure.CAN_FilterIdHigh = 0;
    stCAN_FilterInitStructure.CAN_FilterIdLow = 0x0200;
    stCAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
    stCAN_FilterInitStructure.CAN_FilterMaskIdLow = 0xFF00;
    stCAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
    stCAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
    CAN_FilterInit(&stCAN_FilterInitStructure);
    
    /* CAN filter init : No filter */
    stCAN_FilterInitStructure.CAN_FilterNumber = 17;
    stCAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
    stCAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
    stCAN_FilterInitStructure.CAN_FilterIdHigh = 0;
    stCAN_FilterInitStructure.CAN_FilterIdLow = 0xFF00;
    stCAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
    stCAN_FilterInitStructure.CAN_FilterMaskIdLow = 0xFF00;
    stCAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
    stCAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
    CAN_FilterInit(&stCAN_FilterInitStructure);
  }
  
  if(CANx == CAN1)
  {
    stNVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
    stNVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
    stNVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
  }
  else if(CANx == CAN2)
  {
    stNVIC_InitStructure.NVIC_IRQChannel = CAN2_RX0_IRQn;
    stNVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;
    stNVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
  }
  
  stNVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&stNVIC_InitStructure);
  stNVIC_InitStructure.NVIC_IRQChannel = CAN1_SCE_IRQn;
  stNVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  stNVIC_InitStructure.NVIC_IRQChannel = CAN2_SCE_IRQn;
  stNVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  
  NVIC_Init(&stNVIC_InitStructure);
  /* Enable FIFO 0 message pending Interrupt */
  CAN_ClearITPendingBit(CANx, RESET);
  CAN_ITConfig(CANx, CAN_IT_FMP0 | CAN_IT_FF0 | CAN_IT_FOV0 | CAN_IT_BOF |CAN_IT_EPV | CAN_IT_ERR | CAN_IT_LEC, ENABLE);
  
  /* Enable STROBE AND CAN */
  GPIO_SetBits(CAN1_ENABLE_PORT,CAN1_ENABLE_PIN);
  GPIO_SetBits(CAN1_STROBE_PORT,CAN1_STROBE_PIN);
  GPIO_SetBits(CAN2_ENABLE_PORT,CAN2_ENABLE_PIN);
  GPIO_SetBits(CAN2_STROBE_PORT,CAN2_STROBE_PIN);
  
  
  
}

/*****************************************************************************
**@Function      : fnFifo_Reset
**@Descriptions  : Resets Tx FIFO
**@parameters    : pointer to Tx FIFO
**@Return        : None
*****************************************************************************/
void fnFifo_Reset(stTxMsgFlags *stFifo)
{
  stFifo->bFifoEmpty = 1;
  stFifo->bFifoFull = 0;
  stFifo->bFiEmpFlt = 0;
  stFifo->bFiFullFlt = 0;
  stFifo->ucReadCnt = 0;
  stFifo->ucWriteCnt = 0;
}




/*****************************************************************************
**@Function      : fnCANMsg_Validate
**@Descriptions  : Validate received can message as per the Device ID
**@parameters    : None
**@Return        : None
*****************************************************************************/
void fnCANMsg_Validate(CanRxMsg *pstCANrxmsg)
{
  uint32_t uliCanID = pstCANrxmsg->ExtId;
  
  /************Destination Addr*************/
  switch  (uliCanID & DEST_ADDR_MASK)
  {
  case DEST_ALL:   //Broardcast message
    {
#ifdef DBG_CAN_PARSER  
      printf("Broadcast Message\r\n");
#endif
      /*message is for MCU so precess it*/
      fnCANMsg_Process(pstCANrxmsg);
    }
    break;
    
  case DEST_SCU:    //MSG for SCU
    {
#ifdef DBG_CAN_PARSER 
      printf("Destined for SCU\r\n");
#endif
      /*message is for MCU so precess it*/
      fnCANMsg_Process(pstCANrxmsg);
    }
    break;
    
  default:
    {
#ifdef DBG_CAN_PARSER 
      printf("Error Frame..\r\n");
#endif
    }
    break;
  }
}

/*****************************************************************************
**@Function 			:	 fnProcessCANMsg
**@Descriptions	:  Process received can message
**@parameters		:	 pstCANrxmsg:  Received CAN Message
**@returnval			:  none
*****************************************************************************/
void fnCANMsg_Process(CanRxMsg *pstCANrxmsg)
{
  //			uint8_t ucCnt = 0;
  
  uint8_t ucRec_Data[MAX_DATA_LENGTH];
  uint32_t uliCanID = pstCANrxmsg->ExtId;
  stTx1Message.DLC = pstCANrxmsg->DLC;
  /* will decide*/
  for(ucLoopCnt = 0; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
  {
    /*ucRec_Data is array for data*/
    ucRec_Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
  }
  if(SOURCE_TABLET != (uliCanID & SOURCE_ADDR_MASK))
  {
    if((pstCANrxmsg->ExtId & ACKNOWLEDGEMENT) != ACKNOWLEDGEMENT)
    {
      fnSendACK(pstCANrxmsg); 
    }	
  }
#ifdef DBG_CAN_PARSER      
  printf("In fnProcessCANMsg..\r\n");
#endif	
#ifdef DBG_CAN_HB		
  printf("uliCanID = %x \r\n",uliCanID);
#endif	
  /*change32*/
  switch (uliCanID & SOURCE_ADDR_MASK)
  {
    /******************FORWARD MESSAGE TO HMI**********************/
  case SOURCE_TABLET: 
    {
      /*change39*/
			switch (uliCanID & TYPE_MASK)
			{
				case MSG_TYPE_CONFIG : 
					{
						#ifdef DBG_CAN_PARSER_CONFIG 
										printf("uliCanID = %x \r\n",uliCanID);
										printf("Forward to HMI\n\r"); 
						#endif	
									stTx1Message.DLC = pstCANrxmsg->DLC; 
									for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
									{
										stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
									}
									
									/****************************ABK*****************************/
									pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                              (DEST_HMI	 | SOURCE_SCU));
										stTx1Message.ExtId = pstCANrxmsg->ExtId; 
										/*Send message to HMI*/
										
										fnCANMsg_Transmit(&stTx1Message,CAN1);
							}
					break;
			/*change47*/
				case MSG_TYPE_COMMAND :
						{
						switch (uliCanID & MSG_MASK)
								{
									case CMD_CLEAR_ERROR:
											 {
											#ifdef DBG_CAN_PARSER_CONFIG 
														printf("uliCanID = %x \r\n",uliCanID);
														printf("Forward to HMI\n\r"); 
											#endif	
												stTx1Message.DLC = pstCANrxmsg->DLC; 
												for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
												{
													stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
												}
									
												/****************************ABK*****************************/
												pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
																				(DEST_HMI	 | SOURCE_SCU));
												stTx1Message.ExtId = pstCANrxmsg->ExtId; 
												/*Send message to HMI*/
												fnCANMsg_Transmit(&stTx1Message,CAN1);
											}
											break;
									case CMD_CLEAR_WARNING:
											 {
											#ifdef DBG_CAN_PARSER_CONFIG 
														printf("uliCanID = %x \r\n",uliCanID);
														printf("Forward to HMI\n\r"); 
											#endif	
													stTx1Message.DLC = pstCANrxmsg->DLC; 
													for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
													{
														stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
													}
										
													/****************************ABK*****************************/
													pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
																					(DEST_HMI	 | SOURCE_SCU));
													stTx1Message.ExtId = pstCANrxmsg->ExtId; 
													/*Send message to HMI*/
													fnCANMsg_Transmit(&stTx1Message,CAN1);
												}
											 break;
												case COMMAND_RESET_MATERIAL: 
															{
          
														#ifdef DBG_CAN_PARSER_COMMAND      
															printf(" COMMAND_RESET_MATERIAL\r\n");
																#endif	
          /*change38*/
          stCurrentData.fCurr_BaseMaterialPrimedAccumulate = RESET;
          stCurrentData.fCurr_HardMaterialPrimedAccumulate= RESET;
          stCurrentData.fCurr_BaseMaterialAppliedAccumulate= RESET;
          stCurrentData.fCurr_HardMaterialAppliedAccumulate= RESET;
          stCurrentData.fCurr_BaseTotalMaterialUsedAccumulate= RESET;
          stCurrentData.fCurr_HardTotalMaterialUsedAccumulate= RESET;
					stCurrentData.fCurr_BaseTotalMaterialUsed= RESET;
					stCurrentData.fCurr_HardTotalMaterialUsed= RESET;
					
					/*change43 CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_120;
																
																	stTx1Message.DLC = pstCANrxmsg->DLC; 
													for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
													{
														stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
													}
										
													/****************************ABK*****************************/
													pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
																					(DEST_HMI	 | SOURCE_SCU));
													stTx1Message.ExtId = pstCANrxmsg->ExtId; 
													/*Send message to HMI*/
													fnCANMsg_Transmit(&stTx1Message,CAN1);

         
        }
        break;
			
						
									}
    

						}
						break;
						
						case MSG_TYPE_FAULT :
						{
						switch (uliCanID & MSG_MASK)
								{
									case FAULT_LOG_MEMORY_FULL_ERROR:
											 {
											#ifdef DBG_CAN_PARSER_CONFIG 
														printf("uliCanID = %x \r\n",uliCanID);
														printf("Forward to HMI\n\r"); 
											#endif	
												 
												 					stTx1Message.DLC = pstCANrxmsg->DLC; 
													for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
													{
														stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
													}
										
													/****************************ABK*****************************/
													pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
																					(DEST_HMI	 | SOURCE_SCU));
													stTx1Message.ExtId = pstCANrxmsg->ExtId; 
													/*Send message to HMI*/
													fnCANMsg_Transmit(&stTx1Message,CAN1);

         
										}
										break;
									}
								}
						break;
			}
		}
		break;
	}	
  
  /************MESSAGE TYPE*************/
  switch (uliCanID & TYPE_MASK)
  {
	/************BROADCAST MESSAGE*************/
  case MSG_TYPE_BROADCAST:
    {
#ifdef DBG_CAN_PARSER      
      printf("MSG_TYPE_BROADCAST \r\n");
#endif
      switch (uliCanID & MSG_MASK) 
      {
      case BCAST_ESTOP:
        {
          
          /* Check if CAN2 for OPMM is Active send same message on CAN2*/
          /* SEND ACKNOWLEDGEMENT */
#ifdef DBG_CAN_PARSER_BROADCAST      
          printf("BCAST_ESTOP \r\n");
#endif
        }
        break;	
      case ACKNOWLEDGEMENT:
        {
          /* RECIEVED ACKNOWLEDGEMENT */							
#ifdef DBG_CAN_PARSER_BROADCAST      
          printf("ACKNOWLEDGE FOR HEARTBEAT RECIEVED \r\n");
#endif		
          switch (uliCanID & SOURCE_ADDR_MASK)
          {
          case SOURCE_HMI:
            {
#ifdef DBG_CAN_PARSER_BROADCAST
              printf("ACK from SOURCE_HMI \r\n");
#endif
              if((POSITIVE_ACK == pstCANrxmsg->Data[0]) && rgucHBCntr[HMI] > 0)
              {
                /*Decreament the counter by 1 to indicate that the
                HB ACK rcvd successfully*/
                rgucHBCntr[HMI] = 0; 
                
                uiHBACKfail &= ~(1 << HMI); /*Clear HB ACK fault  if any*/
                
                uiHBFailSend &= ~(1 << HMI); /*RESET the HB Fail send VAriable*/
                
                ucDevDead_HMI = 0;
                
              }
              else if(FIRST_BOOTUP_ACK == pstCANrxmsg->Data[0])
              {
                
                ucDevDead_HMI = 0;
                
              }
            }
            break;
            
          case SOURCE_OPMM:
            {
#ifdef DBG_CAN_PARSER_BROADCAST
              printf("HB from SOURCE_OPMM \r\n");
#endif
              
              if((POSITIVE_ACK == pstCANrxmsg->Data[0]) && rgucHBCntr[OPMM] > 0)
              {
                /*Decreament the counter by 1 to indicate that 
                the HB ACK rcvd successfully */
                rgucHBCntr[OPMM] = 0; 
                uiHBACKfail &= ~(1 << OPMM);  /*Clear HB ACK fault  if any*/
                uiHBFailSend &= ~(1 << OPMM); /*RESET the HB Fail send VAriable*/
                
                ucDevDead_OPMM = 0;
              }
              if(FIRST_BOOTUP_ACK == pstCANrxmsg->Data[0])
              {
                ucDevDead_OPMM = 0;
                fnSendOPMM_Config();
              }
            }
            break;
            
          case SOURCE_VCM:
            {
#ifdef DBG_CAN_PARSER_BROADCAST
              printf("HB from SOURCE_VCM \r\n");
#endif
              if((POSITIVE_ACK == pstCANrxmsg->Data[0]) && rgucHBCntr[VCM] > 0)
              {
                /*Decreament the counter by 1 to indicate that
                the HB ACK rcvd successfully */
                rgucHBCntr[VCM] =0;  
                uiHBACKfail &= ~(1 << VCM);  /* Clear HB ACK fault  if any*/
                uiHBFailSend &= ~(1 << VCM); /*RESET the HB Fail send VAriable*/
                ucDevDead_VCM = 0;
                
              }
              if(FIRST_BOOTUP_ACK == pstCANrxmsg->Data[0])
              {
                ucDevDead_VCM = 0; 
                fnSendVCM_Config();
              }
            }
            break;
            
          case SOURCE_HCM:
            {
#ifdef DBG_CAN_PARSER_BROADCAST
              printf("HB from SOURCE_HCM \r\n");
#endif
              if((POSITIVE_ACK == pstCANrxmsg->Data[0]) && rgucHBCntr[HCM] > 0)
              {
                /* Decreament the counter by 1 to indicate that 
                the HB ACK rcvd successfully */
                rgucHBCntr[HCM]= 0; 
                uiHBACKfail &= ~(1 << HCM);  /* Clear HB fault  if any*/
                uiHBFailSend &= ~(1 << HCM); /*RESET the HB Fail send VAriable*/
                ucDevDead_HCM = 0;
              }
              if(FIRST_BOOTUP_ACK == pstCANrxmsg->Data[0])
              {
                ucDevDead_HCM = 0;
                fnSendHCM_Config();
              }
            }
            //		break; commented for Warning
            
          }	
          //		break;
        }
        //		break;
      }
      //	break;
    }
    break;
    /************COMMAND MESSAGE*************/
  case MSG_TYPE_COMMAND:
    {
#ifdef DBG_CAN_PARSER_COMMAND      
      printf("MSG_TYPE_COMMAND \r\n");
#endif
      
      switch (uliCanID & MSG_MASK)
      {
        /* Acknowledge recieved for Command message */
      case ACKNOWLEDGEMENT:
        {	
#ifdef DBG_CAN_PARSER_COMMAND      
          printf(" ACKNOWLEDGEMENT FOR COMMAND MESSAGE..\r\n");
#endif						
          fnCANPost_Ack(pstCANrxmsg,CAN1);
          
        }
        break;
        /* Command recieved from HMI To switch to operation or warmup mode */
      case CMD_SET_SYSTEM_MODE:
        {
          /*if in warmup change to operation mode and vice-versa*/
          if(CAN_MSG_WARM_UP_MODE ==(ucRec_Data[DATA_BYTE_ZERO]))
          {
            /* Change to Warm-Up-Mode */
            
            
          }
          else if(CAN_MSG_OPERATION_MODE ==(ucRec_Data[DATA_BYTE_ZERO]))
          {
            /* Change to Operation mode */
          }
#ifdef DBG_CAN      
          printf("CMD_SET_SYSTEM_MODE \r\n");
#endif	
        }
        break;
        /* Command recieved from HMI to turn on/off heater of HCM for testing */
      case CMD_TURN_ON_OFF_HEATER:
        {
          /* send this same message to HCM*/
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_HCM | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CAN_PARSER_COMMAND      
          printf("CMD_TURN_ON_OFF_HEATER\r\n");
#endif	
        }
        break;
        /* Command recieved from HMI to turn on/off stall valve of VCM for testing */
      case CMD_STALL_VALVE:
        {
          /* send this same message to VCM*/
          pstCANrxmsg->ExtId 	= ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                   (DEST_VCM | SOURCE_SCU));
          stTx1Message.ExtId 	= pstCANrxmsg->ExtId; 
          stTx1Message.DLC 		= pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CAN_PARSER_COMMAND      
          printf("CMD_STALL_VALVE\r\n");
#endif	
        }
        break;
        /* Command recieved from HMI to turn on/off Particular Valve of OPMM for testing */
      case CMD_OPMM_VALVE:
        {
          /* send this same message to OPMM*/
          pstCANrxmsg->ExtId 	= ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                   (DEST_OPMM | SOURCE_SCU));
          stTx1Message.ExtId 	= pstCANrxmsg->ExtId; 
          stTx1Message.DLC 		= pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CAN_PARSER_COMMAND      
          printf("CMD_OPMM_VALVE\r\n");
#endif	
        }
        break;
        /* Command from HMI to Clear all Error */
      case CMD_CLEAR_ERROR: 
        {
          StatusFlag.stStatusFlag.bClearFault = 1;
					StatusFlag.stStatusFlag.bStop_Flush_flag = RESET;
          fnAllERROR_Clear((uliCanID & SOURCE_ADDR_MASK));
#ifdef DBG_CAN_PARSER_COMMAND      
          printf(" CMD_CLEAR_ERROR\r\n");
#endif	
        }
        break;
        /* Command from HMI to Clear all Warning */
      case CMD_CLEAR_WARNING: 
        {
          StatusFlag.stStatusFlag.bClearWarning = 1;
          fnAllWarnnings_Clear(pstCANrxmsg->Data[0],(uliCanID & SOURCE_ADDR_MASK));
#ifdef DBG_CAN_PARSER_COMMAND      
          printf(" CMD_CLEAR_WARNING\r\n");
#endif			
        }
        break;
        /* Command recieved from HMI to turn on/off Laser Beam of OPMM  */
      case CMD_LASER_BEAM: 
        {
          pstCANrxmsg->ExtId 	= ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                   (DEST_OPMM | SOURCE_SCU));
          stTx1Message.ExtId 	= pstCANrxmsg->ExtId; 
          stTx1Message.DLC 		= pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          fnCANMsg_Transmit(&stTx1Message, CAN1);	
#ifdef DBG_CAN_PARSER_COMMAND      
          printf(" CMD_LASER_BEAM\r\n");
#endif			
        }
        break;
        /* Command recieved from HMI to start Stall test manually */
      case CMD_STALL_TEST: 
        {
					/*change51*/
					if(FLUSHPROCESS == ucPreviousProcess)
							{
          /* this is use for manual stall test set flags for manual stall test */
          pstCANrxmsg->ExtId 	= ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                   (DEST_VCM | SOURCE_SCU));
          stTx1Message.ExtId 	= pstCANrxmsg->ExtId; 
          stTx1Message.DLC 		= pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          fnCANMsg_Transmit(&stTx1Message, CAN1);	
          ucPRDSpray = PRDSTALL; //change15
					StatusFlag.stStatusFlag.bStall_In_Progress = SET; /*change70*/
#ifdef DBG_CAN_PARSER_COMMAND      
          printf(" CMD_STALL_TEST\r\n");
#endif			
				}
        }
        break;
        /* Command recieved from HMI to turn on/off Laser Beam of OPMM  */
      case COMMAND_RESET_MATERIAL: 
        {
          
#ifdef DBG_CAN_PARSER_COMMAND      
          printf(" COMMAND_RESET_MATERIAL\r\n");
#endif	
          /*change38*/
          stCurrentData.fCurr_BaseMaterialPrimedAccumulate = RESET;
          stCurrentData.fCurr_HardMaterialPrimedAccumulate= RESET;
          stCurrentData.fCurr_BaseMaterialAppliedAccumulate= RESET;
          stCurrentData.fCurr_HardMaterialAppliedAccumulate= RESET;
          stCurrentData.fCurr_BaseTotalMaterialUsedAccumulate= RESET;
          stCurrentData.fCurr_HardTotalMaterialUsedAccumulate= RESET;
					stCurrentData.fCurr_BaseTotalMaterialUsed= RESET;
					stCurrentData.fCurr_HardTotalMaterialUsed= RESET;
					
					/*change43 CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_120;
          /*change32*/
          if(SOURCE_HMI == (uliCanID & SOURCE_ADDR_MASK))
          {
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            /****************************ABK*****************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_TABLET	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
//            printf("pstCANrxmsg->ExtId = %x \r\n",pstCANrxmsg->ExtId);
            ////						stWifi.ucSendFlag = SET;
            ////						stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
          }		
        }
        break;
				
				  /* Command recieved from HMI to turn on/off Laser Beam of OPMM  */
      case COMMAND_APPLY_WIFI_SETTING: 
        {
          
#ifdef DBG_CAN_PARSER_COMMAND      
          printf(" COMMAND_APPLY_WIFI_SETTING\r\n");
#endif	
          
					
					
          /*change32*/
          if(SOURCE_HMI == (uliCanID & SOURCE_ADDR_MASK))
          {
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            /****************************ABK*****************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_TABLET	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
//            printf("pstCANrxmsg->ExtId = %x \r\n",pstCANrxmsg->ExtId);
            ////						stWifi.ucSendFlag = SET;
            ////						stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
					}	
								/****************************************************************/
		/*change17*/
		
			if((strcmp((char *)stWifiRxIP.rgucIP_Addr,(char *)stWifiIP.rgucIP_Addr)) ||
				strcmp((char *)stWifiRxIP.rgucSubnet_Addr,(char *)stWifiIP.rgucSubnet_Addr) ||
					strcmp((char *)stWifiRxIP.rgucGateway_Addr,(char *)stWifiIP.rgucGateway_Addr) ||
						strcmp((char *)stWifiRxIP.rgucSSID,(char *)stWifiIP.rgucSSID) ||
							strcmp((char *)stWifiRxIP.rgucPASWD,(char *)stWifiIP.rgucPASWD) ||
								(stWifiRxIP.uiPort != stWifiIP.uiPort))
							{
//								printf(" WIFI_SETTING change\r\nIP %s %s\n\rSM %s %s\n\rGW %s %s\n\rPort %d %d\n\rSSID %s %s\n\rPass %s %s",
//								(char *)stWifiRxIP.rgucIP_Addr,(char *)stWifiIP.rgucIP_Addr,
//									(char *)stWifiRxIP.rgucSubnet_Addr,(char *)stWifiIP.rgucSubnet_Addr,
//										(char *)stWifiRxIP.rgucGateway_Addr,(char *)stWifiIP.rgucGateway_Addr,
//											stWifiRxIP.uiPort,stWifiIP.uiPort,
//										(char *)stWifiRxIP.rgucSSID,(char *)stWifiIP.rgucSSID,
//											(char *)stWifiRxIP.rgucPASWD,(char *)stWifiIP.rgucPASWD);
								uint8_t rgucConfig[100];
								fnWifiCommandMode();
								strcpy((char *)stWifiRxIP.rgucIP_Addr,(char *)stWifiIP.rgucIP_Addr);
								sprintf((char *)rgucConfig,"set ip addr %s",(char *)stWifiIP.rgucIP_Addr);
								fnWifiCommand(rgucConfig);
								
								strcpy((char *)stWifiRxIP.rgucSubnet_Addr,(char *)stWifiIP.rgucSubnet_Addr);
								sprintf((char *)rgucConfig,"set ip net %s",(char *)stWifiIP.rgucSubnet_Addr);
								fnWifiCommand(rgucConfig);
								
								strcpy((char *)stWifiRxIP.rgucGateway_Addr,(char *)stWifiIP.rgucGateway_Addr);
								sprintf((char *)rgucConfig,"set ip gateway %s",(char *)stWifiIP.rgucGateway_Addr);
								fnWifiCommand(rgucConfig);
								
								stWifiRxIP.uiPort=stWifiIP.uiPort;
								sprintf((char *)rgucConfig,"set ip localport %d",stWifiIP.uiPort);
								fnWifiCommand(rgucConfig);
								
								strcpy((char *)stWifiRxIP.rgucSSID,(char *)stWifiIP.rgucSSID);
								sprintf((char *)rgucConfig,"set apmode ssid %s",(char *)stWifiIP.rgucSSID);
								fnWifiCommand(rgucConfig);
								
								strcpy((char *)stWifiRxIP.rgucPASWD,(char *)stWifiIP.rgucPASWD);
								sprintf((char *)rgucConfig,"set apmode passphrase %s",(char *)stWifiIP.rgucPASWD);
								fnWifiCommand(rgucConfig);
								
								fnWifiCommand("save");
								
								fnWifiCommand("reboot");
          }		
				}
        break;
				     /*change64 Command recieved from HMI to turn on/off Particular Valve of OPMM for testing */
      case CMD_REMOTE_BUTTON_PRESS:
        {
         
#ifdef DBG_CAN_PARSER_COMMAND      
          printf("CMD_REMOTE_BUTTON_PRESS\r\n");
#endif	
					 /************************************************************/
          StatusFlag.stStatusFlag.bCanRemote = SET;
          if(BIT0_SET==(ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK))
          {
          
            	if(RESET == stProcessInput.stProcessFlag.bPrimeSelect)
					{
						// stProcessInput.stProcessFlag.bPrimeSelect = SET ;
            /* Send Status message to display Remote switch detected */
					/*change63*/
					stTx1Message.DLC = ONE_BYTES;
					stTx1Message.Data[DATA_BYTE_ZERO] = BIT0_SET  ;
							stTx1Message.ExtId = 	MSG_TYPE_STATUS | STATUS_REMOTE_SW |
																DEST_HMI | SOURCE_SCU;
					stTx1Message.IDE = CAN_ID_EXT;
					stTx1Message.RTR = CAN_RTR_DATA;
					fnCANMsg_Transmit(&stTx1Message, CAN1);
					/****************************ABK*****************************/
						stTx1Message.DLC = ONE_BYTES;
					stTx1Message.Data[DATA_BYTE_ZERO] = BIT0_SET  ;
					stTx1Message.ExtId = ((stTx1Message.ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
															 (DEST_TABLET	 | SOURCE_SCU));
					fnXML_FIFO(&stTx1Message);
						
						stProcessInput.stProcessFlag.bPrimeSelect = SET ;
					}						

#ifdef DBG_CAN_PARSER_COMMAND      
            printf("PRIME BUTTON PRESSED \r\n");
#endif	
            
          }
          if(BIT1_SET==(ucRec_Data[DATA_BYTE_ZERO] & BIT1_MASK))
          {
           	if(RESET == stProcessInput.stProcessFlag.bFlushSelect)
					{
         // stProcessInput.stProcessFlag.bFlushSelect = SET ;
             /* Send Status message to display Remote switch detected */
					/*change63*/
					stTx1Message.DLC = ONE_BYTES;
					stTx1Message.Data[DATA_BYTE_ZERO] = BIT1_SET  ;
						stTx1Message.ExtId = 	MSG_TYPE_STATUS | STATUS_REMOTE_SW |
																DEST_HMI | SOURCE_SCU;
					stTx1Message.IDE = CAN_ID_EXT;
					stTx1Message.RTR = CAN_RTR_DATA;
					fnCANMsg_Transmit(&stTx1Message, CAN1);
					/****************************ABK*****************************/
							stTx1Message.DLC = ONE_BYTES;
					stTx1Message.Data[DATA_BYTE_ZERO] = BIT1_SET  ;
					stTx1Message.ExtId = ((stTx1Message.ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
															 (DEST_TABLET	 | SOURCE_SCU));
					fnXML_FIFO(&stTx1Message);	
						
						 stProcessInput.stProcessFlag.bFlushSelect = SET ;
					}

#ifdef DBG_CAN_PARSER_COMMAND      
            printf("FLUSH BUTTON PRESSED \r\n");
#endif	
          }
          if(BIT2_SET==(ucRec_Data[DATA_BYTE_ZERO] & BIT2_MASK))
          {
           	if(RESET == stProcessInput.stProcessFlag.bSpraySelect)
					{
//          stProcessInput.stProcessFlag.bSpraySelect = SET ;
             /* Send Status message to display Remote switch detected */
					/*change63*/
					stTx1Message.DLC = ONE_BYTES;
					stTx1Message.Data[DATA_BYTE_ZERO] = BIT2_SET  ;
						stTx1Message.ExtId = 	MSG_TYPE_STATUS | STATUS_REMOTE_SW |
																DEST_HMI | SOURCE_SCU;
					stTx1Message.IDE = CAN_ID_EXT;
					stTx1Message.RTR = CAN_RTR_DATA;
					fnCANMsg_Transmit(&stTx1Message, CAN1);
					/****************************ABK*****************************/
						stTx1Message.DLC = ONE_BYTES;
					stTx1Message.Data[DATA_BYTE_ZERO] = BIT2_SET;  
					stTx1Message.ExtId = ((stTx1Message.ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
															 (DEST_TABLET	 | SOURCE_SCU));
					fnXML_FIFO(&stTx1Message);
    stProcessInput.stProcessFlag.bSpraySelect = SET ;						
					}

#ifdef DBG_CAN_PARSER_COMMAND      
            printf("SPRAY BUTTON PRESSED \r\n");
#endif
          }
          if(BIT3_SET==(ucRec_Data[DATA_BYTE_ZERO] & BIT3_MASK))
          {
							if(RESET == stProcessInput.stProcessFlag.bReadPipeTempComm)
					{
          stProcessInput.stProcessFlag.bReadPipeTempComm = SET ;
               /* Send Status message to display Remote switch detected */
					/*change63*/
					stTx1Message.DLC = ONE_BYTES;
					stTx1Message.Data[DATA_BYTE_ZERO] = BIT3_SET  ;
						stTx1Message.ExtId = 	MSG_TYPE_STATUS | STATUS_REMOTE_SW |
																DEST_HMI | SOURCE_SCU;
					stTx1Message.IDE = CAN_ID_EXT;
					stTx1Message.RTR = CAN_RTR_DATA;
					fnCANMsg_Transmit(&stTx1Message, CAN1);
					/****************************ABK*****************************/
						stTx1Message.DLC = ONE_BYTES;
					stTx1Message.Data[DATA_BYTE_ZERO] = BIT3_SET  ;
					stTx1Message.ExtId = ((stTx1Message.ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
															 (DEST_TABLET	 | SOURCE_SCU));
					fnXML_FIFO(&stTx1Message);	
						
						stProcessInput.stProcessFlag.bReadPipeTempComm = SET ;
					}
#ifdef DBG_CAN_PARSER_COMMAND      
            printf("PIPE TEMPERATURE BUTTON PRESSED \r\n");
#endif
         
            
          }
          if(BIT4_SET==(ucRec_Data[DATA_BYTE_ZERO] & BIT4_MASK))
          {
							if(RESET == stProcessInput.stProcessFlag.bStartProcess)
					{
//          stProcessInput.stProcessFlag.bStartProcess = SET ;
               /* Send Status message to display Remote switch detected */
					/*change63*/
					stTx1Message.DLC = ONE_BYTES;
					stTx1Message.Data[DATA_BYTE_ZERO] = BIT4_SET  ;
						stTx1Message.ExtId = 	MSG_TYPE_STATUS | STATUS_REMOTE_SW |
																DEST_HMI | SOURCE_SCU;
					stTx1Message.IDE = CAN_ID_EXT;
					stTx1Message.RTR = CAN_RTR_DATA;
					fnCANMsg_Transmit(&stTx1Message, CAN1);
					/****************************ABK*****************************/
						stTx1Message.DLC = ONE_BYTES;
					stTx1Message.Data[DATA_BYTE_ZERO] = BIT4_SET  ;
					stTx1Message.ExtId = ((stTx1Message.ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
															 (DEST_TABLET	 | SOURCE_SCU));
					fnXML_FIFO(&stTx1Message);	
						
					 stProcessInput.stProcessFlag.bStartProcess = SET ;	
					}

#ifdef DBG_CAN_PARSER_COMMAND      
            printf("START/STOP BUTTON PRESSED \r\n");
#endif
          
            
          }
          if(BIT5_SET==(ucRec_Data[DATA_BYTE_ZERO] & BIT5_MASK))
          {
						
							if(RESET == stProcessInput.stProcessFlag.bFlushComplete)
					{
//          stProcessInput.stProcessFlag.bFlushComplete = SET ;
               /* Send Status message to display Remote switch detected */
					/*change63*/
					stTx1Message.DLC = ONE_BYTES;
					stTx1Message.Data[DATA_BYTE_ZERO] = BIT5_SET  ;
						stTx1Message.ExtId = 	MSG_TYPE_STATUS | STATUS_REMOTE_SW |
																DEST_HMI | SOURCE_SCU;
					stTx1Message.IDE = CAN_ID_EXT;
					stTx1Message.RTR = CAN_RTR_DATA;
					fnCANMsg_Transmit(&stTx1Message, CAN1);
					/****************************ABK*****************************/
						stTx1Message.DLC = ONE_BYTES;
					stTx1Message.Data[DATA_BYTE_ZERO] = BIT5_SET  ;
					stTx1Message.ExtId = ((stTx1Message.ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
															 (DEST_TABLET	 | SOURCE_SCU));
					fnXML_FIFO(&stTx1Message);	
						
						stProcessInput.stProcessFlag.bFlushComplete = SET ;
					}

#ifdef DBG_CAN_PARSER_COMMAND      
            printf("FLUSH COMPLETE BUTTON PRESSED \r\n");
#endif
 
            
          }
         
					
        }
        break;
      }	
    }
    break;
    /************FAULT MESSAGE*************/
  case MSG_TYPE_FAULT:
    {
#ifdef DBG_CAN_PARSER_FAULT      
      printf("MSG_TYPE_FAULT \r\n");
#endif			
      switch (uliCanID & MSG_MASK)
      {
        /* Acknowledge recieved for Fault message */
      case ACKNOWLEDGEMENT:
        {
          fnCANPost_Ack(pstCANrxmsg,CAN1);
#ifdef DBG_CAN_PARSER_FAULT      
          printf("ACKNOWLEDGE FOR FAULT MESSAGE..\r\n");
#endif	
        }
        break;
        /* Fault meassge recieved from OPMM if any fault occurs in any Valve of OPMM
        have to forward this message to TABLET also*/
      case FAULT_OPMM_VALVE:
        {
          /******************FORWARD MESSAGE TO TABLET & HMI**********************/	
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_HMI | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          fnCANMsg_Transmit(&stTx1Message, CAN1);
          
          /****************************ABK*****************************/
         /*change46*/
				//fault messages are sent without source address change
					//    pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
           //                     (DEST_TABLET	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          //						stWifi.ucSendFlag = SET;
          //						stEthernet.ucSendFlag = SET;
          fnXML_FIFO(&stTx1Message);
          /************************************************************/
          
          if(BIT0_SET==(ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK))
          {
            StatusFlag.stStatusFlag.bErrorFlag = SET;/*Error SET*/	
            StatusFlag.stStatusFlag.bErrorSentFlag = SET;
            Fault_Log.stFault_Log_Flag.bFault_Log_SVF = SET;
            
#ifdef DBG_CAN_PARSER_FAULT      
            printf("SPRAY VALVE ERROR \r\n");
#endif	
            
          }
          if(BIT1_SET==(ucRec_Data[DATA_BYTE_ZERO] & BIT1_MASK))
          {
            StatusFlag.stStatusFlag.bErrorFlag = SET;/*Error SET*/	
            StatusFlag.stStatusFlag.bErrorSentFlag = SET;
            Fault_Log.stFault_Log_Flag.bFault_Log_GPVF = SET;
#ifdef DBG_CAN_PARSER_FAULT      
            printf("GUN POSITION VALVE ERROR \r\n");
#endif	
          }
          if(BIT2_SET==(ucRec_Data[DATA_BYTE_ZERO] & BIT2_MASK))
          {
            StatusFlag.stStatusFlag.bErrorFlag = SET;/*Error SET*/	
            StatusFlag.stStatusFlag.bErrorSentFlag = SET;
            Fault_Log.stFault_Log_Flag.bFault_Log_ABVF = SET;
#ifdef DBG_CAN_PARSER_FAULT      
            printf("AB VALVE ERROR \r\n");
#endif
          }
          if(BIT3_SET==(ucRec_Data[DATA_BYTE_ZERO] & BIT3_MASK))
          {
#ifdef DBG_CAN_PARSER_FAULT      
            printf("SOLVENT VALVE ERROR \r\n");
#endif
            StatusFlag.stStatusFlag.bErrorFlag = SET;/*Error SET*/	
            StatusFlag.stStatusFlag.bErrorSentFlag = SET;
            Fault_Log.stFault_Log_Flag.bFault_Log_FVF = SET;
            
          }
          if(BIT4_SET==(ucRec_Data[DATA_BYTE_ZERO] & BIT4_MASK))
          {
#ifdef DBG_CAN_PARSER_FAULT      
            printf("FORWARD MOTOR VALVE ERROR \r\n");
#endif
            StatusFlag.stStatusFlag.bWarningFlag = SET;/*Warnning SET*/
            StatusFlag.stStatusFlag.bErrorSentFlag = SET;
            Fault_Log.stFault_Log_Flag.bFault_Log_MFVF = SET;
            
          }
          if(BIT5_SET==(ucRec_Data[DATA_BYTE_ZERO] & BIT5_MASK))
          {
#ifdef DBG_CAN_PARSER_FAULT      
            printf("BACKWARD MOTOR VALVE ERROR \r\n");
#endif
            StatusFlag.stStatusFlag.bWarningFlag = SET;/*Warnning SET*/
            StatusFlag.stStatusFlag.bErrorSentFlag = SET;
            Fault_Log.stFault_Log_Flag.bFault_Log_MBVF = SET;
            
          }
          
          /**************************Write fault LOG********************/
          
         	if((RESET == stEthernetLog.ucSendFlag)||(RESET == stWifiLog.ucSendFlag))  /*change65*/
			{
          fnFault_Log();
			}
#ifdef DBG_CAN_PARSER_FAULT      
          printf("FAULT_OPMM_VALVE \r\n");
#endif	
          
        }
        break;
        /* Fault meassge recieved from VCM if any fault occurs in Stall Valve of VCM
        have to forward this message to TABLET also*/
      case FAULT_STALL_VALVE:
        {
          /******************FORWARD MESSAGE TO TABLET**********************/	
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          
          /****************************ABK*****************************/
          /*change46*/
				//fault messages are sent without source address change
					//    pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
           //                     (DEST_TABLET	 | SOURCE_SCU));
          //						stWifi.ucSendFlag = SET;
          //						stEthernet.ucSendFlag = SET;
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          fnXML_FIFO(&stTx1Message);
//          /************************************************************/
//          StatusFlag.stStatusFlag.bErrorFlag = SET;/*Error SET*/	
//          StatusFlag.stStatusFlag.bErrorSentFlag = SET;
          /**************************Write fault LOG********************/
					/*change53*/
					 if(BIT0_SET==(ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK))
          {
            StatusFlag.stStatusFlag.bErrorFlag = SET;/*Error SET*/	
            StatusFlag.stStatusFlag.bErrorSentFlag = SET;
            Fault_Log.stFault_Log_Flag.bFault_Log_STVF = SET;	
            
#ifdef DBG_CAN_PARSER_FAULT      
            printf("STALL VALVE ERROR \r\n");
#endif	
						
            
          }
          if(BIT1_SET==(ucRec_Data[DATA_BYTE_ZERO] & BIT1_MASK))
          {
            StatusFlag.stStatusFlag.bErrorFlag = SET;/*Error SET*/	
            StatusFlag.stStatusFlag.bErrorSentFlag = SET;
            Fault_Log.stFault_Log_Flag.bFault_Log_RVF = SET; /*change54*/
#ifdef DBG_CAN_PARSER_FAULT      
            printf("RECIRCULATION VALVE ERROR \r\n");
#endif	
          }
					
          	if((RESET == stEthernetLog.ucSendFlag)||(RESET == stWifiLog.ucSendFlag))  /*change65*/
			{
           fnFault_Log();
			}
          
#ifdef DBG_CAN_PARSER_FAULT      
          printf("FAULT_STALL_VALVE \r\n");
#endif	
        }
        break;
        /* Fault meassge recieved from HCM if Temperature goes out of range of material
        have to forward this message to TABLET also*/
      case FAULT_TEMP_BEYOND_TH:
        {
          /******************FORWARD MESSAGE TO TABLET**********************/	
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          
          /****************************ABK*****************************/
          /*change46*/
				//fault messages are sent without source address change
					//    pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
           //                     (DEST_TABLET	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          //						stWifi.ucSendFlag = SET;
          //						stEthernet.ucSendFlag = SET;
          fnXML_FIFO(&stTx1Message);
          /************************************************************/
          if(BIT0_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK))
          {
            
            
#ifdef DBG_CAN_PARSER_FAULT      
            printf("HCM INLINE BASE TEMP FAULT \r\n");
#endif	
            if(BIT0_SET == (ucRec_Data[DATA_BYTE_ONE] & BIT0_MASK))
            {
              Fault_Log.stFault_Log_Flag.bFault_Log_IBLTF = SET;	
              StatusFlag.stStatusFlag.bErrorSentFlag = SET;
              StatusFlag.stStatusFlag.bWarningFlag = SET;/*Warnning SET*/
#ifdef DBG_CAN_PARSER_FAULT      
              printf("INLINE BASE TEMPERATURE LOW THRESHOLD\r\n");
#endif
            }
            if(BIT1_SET == (ucRec_Data[DATA_BYTE_ONE] & BIT1_MASK))
            {
              Fault_Log.stFault_Log_Flag.bFault_Log_IBHTF = SET;	
              StatusFlag.stStatusFlag.bErrorSentFlag = SET;
              StatusFlag.stStatusFlag.bErrorFlag = SET;/*Error SET*/	
#ifdef DBG_CAN_PARSER_FAULT      
              printf("INLINE BASE TEMPERATURE HIGH THRESHOLD\r\n");
#endif
            }
            
          }
          if(BIT1_SET==(ucRec_Data[DATA_BYTE_ZERO] & BIT1_MASK))
          {
            
#ifdef DBG_CAN_PARSER_FAULT      
            printf("HCM INLINE HARD TEMP FAULT \r\n");
#endif	
            if(BIT0_SET == (ucRec_Data[DATA_BYTE_ONE] & BIT0_MASK))
            {
              Fault_Log.stFault_Log_Flag.bFault_Log_IHLTF = SET;	
              StatusFlag.stStatusFlag.bErrorSentFlag = SET;
              StatusFlag.stStatusFlag.bWarningFlag = SET;/*Warnning SET*/
#ifdef DBG_CAN_PARSER_FAULT      
              printf("INLINE HARD TEMPERATURE LOW THRESHOLD\r\n");
#endif
            }
            if(BIT1_SET == (ucRec_Data[DATA_BYTE_ONE] & BIT1_MASK))
            {
              Fault_Log.stFault_Log_Flag.bFault_Log_IHHTF = SET;	
              StatusFlag.stStatusFlag.bErrorSentFlag = SET;
              StatusFlag.stStatusFlag.bErrorFlag = SET;/*Error SET*/	
#ifdef DBG_CAN_PARSER_FAULT      
              printf("INLINE HARD TEMPERATURE HIGH THRESHOLD\r\n");
#endif
            }
          }
          if(BIT2_SET==(ucRec_Data[DATA_BYTE_ZERO] & BIT2_MASK))
          {
            
#ifdef DBG_CAN_PARSER_FAULT      
            printf("HCM ANTIFREEZE TEMP FAULT \r\n");
#endif	
            if(BIT0_SET == (ucRec_Data[DATA_BYTE_ONE] & BIT0_MASK))
            {
              Fault_Log.stFault_Log_Flag.bFault_Log_ALTF = SET;
              StatusFlag.stStatusFlag.bErrorSentFlag = SET;
              StatusFlag.stStatusFlag.bWarningFlag = SET;/*Warnning SET*/
#ifdef DBG_CAN_PARSER_FAULT      
              printf(" ANTIFREEZE TEMP LOW THRESHOLD\r\n");
#endif
            }
            if(BIT1_SET == (ucRec_Data[DATA_BYTE_ONE] & BIT1_MASK))
            {
              Fault_Log.stFault_Log_Flag.bFault_Log_AHTF = SET;
              StatusFlag.stStatusFlag.bErrorSentFlag = SET;
              StatusFlag.stStatusFlag.bErrorFlag = SET;/*Error SET*/	
#ifdef DBG_CAN_PARSER_FAULT      
              printf("ANTIFREEZE TEMP HIGH THRESHOLD\r\n");
#endif
            }
          }
          if(BIT3_SET==(ucRec_Data[DATA_BYTE_ZERO] & BIT3_MASK))
          {
            
#ifdef DBG_CAN_PARSER_FAULT      
            printf("HCM BASE RETURN LINE TEMP FAULT \r\n");
#endif	
            if(BIT0_SET == (ucRec_Data[DATA_BYTE_ONE] & BIT0_MASK))
            {
              Fault_Log.stFault_Log_Flag.bFault_Log_BRLLTHF = SET;
              StatusFlag.stStatusFlag.bErrorSentFlag = SET;
              StatusFlag.stStatusFlag.bWarningFlag = SET;/*Warnning SET*/
#ifdef DBG_CAN_PARSER_FAULT      
              printf("BASE RETURN LINE TEMP LOW THRESHOLD\r\n");
#endif
            }
            if(BIT1_SET == (ucRec_Data[DATA_BYTE_ONE] & BIT1_MASK))
            {
              Fault_Log.stFault_Log_Flag.bFault_Log_BRLHTHF = SET;
              StatusFlag.stStatusFlag.bErrorSentFlag = SET;
              StatusFlag.stStatusFlag.bErrorFlag = SET;/*Error SET*/	
#ifdef DBG_CAN_PARSER_FAULT      
              printf("BASE RETURN LINE TEMP HIGH THRESHOLD\r\n");
#endif
            }
          }
          	if((RESET == stEthernetLog.ucSendFlag)||(RESET == stWifiLog.ucSendFlag))  /*change65*/
			{
          fnFault_Log();
			}
#ifdef DBG_CAN_PARSER_FAULT      
          printf("FAULT_TEMP_BEYOND_TH \r\n");
#endif	
        }
        break;
        /* Fault meassge recieved from VCM if Pressure goes out of range of material
        have to forward this message to TABLET also*/
      case FAULT_PRESSURE_BEYOND_TH:
        {
          /******************FORWARD MESSAGE TO TABLET**********************/	
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          
          /****************************ABK*****************************/
         /*change46*/
				//fault messages are sent without source address change
					//    pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
           //                     (DEST_TABLET	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          //						stWifi.ucSendFlag = SET;
          //						stEthernet.ucSendFlag = SET;
          fnXML_FIFO(&stTx1Message);
          /************************************************************/
          if(BIT0_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK))
          {
            
#ifdef DBG_CAN_PARSER_FAULT      
            printf("VCM PRESSURE GAUGE1 FAULT \r\n");
#endif	
            if(BIT0_SET == (ucRec_Data[DATA_BYTE_ONE] & BIT0_MASK))
            {
              Fault_Log.stFault_Log_Flag.bFault_Log_PG1LTF = SET;
              StatusFlag.stStatusFlag.bErrorSentFlag = SET;
              StatusFlag.stStatusFlag.bWarningFlag = SET;/*Warnning SET*/
#ifdef DBG_CAN_PARSER_FAULT      
              printf("PRESSURE GAUGE1 LOW THRESHOLD\r\n");
#endif
            }
            if(BIT1_SET == (ucRec_Data[DATA_BYTE_ONE] & BIT1_MASK))
            {
              Fault_Log.stFault_Log_Flag.bFault_Log_PG1HTF = SET;
              StatusFlag.stStatusFlag.bErrorSentFlag = SET;
              StatusFlag.stStatusFlag.bErrorFlag = SET;/*Error SET*/	
#ifdef DBG_CAN_PARSER_FAULT      
              printf("PRESSURE GAUGE1 HIGH THRESHOLD\r\n");
#endif
            }
            
          }
          if(BIT1_SET==(ucRec_Data[DATA_BYTE_ZERO] & BIT1_MASK))
          {
            
#ifdef DBG_CAN_PARSER_FAULT      
            printf("VCM PRESSURE GAUGE2 FAULT \r\n");
#endif	
            if(BIT0_SET == (ucRec_Data[DATA_BYTE_ONE] & BIT0_MASK))
            {
              Fault_Log.stFault_Log_Flag.bFault_Log_PG2LTF = SET;
              StatusFlag.stStatusFlag.bErrorSentFlag = SET;
              StatusFlag.stStatusFlag.bWarningFlag = SET;/*Warnning SET*/
#ifdef DBG_CAN_PARSER_FAULT      
              printf("PRESSURE GAUGE2 LOW THRESHOLD\r\n");
#endif
            }
            if(BIT1_SET == (ucRec_Data[DATA_BYTE_ONE] & BIT1_MASK))
            {
              Fault_Log.stFault_Log_Flag.bFault_Log_PG2HTF = SET;
              StatusFlag.stStatusFlag.bErrorSentFlag = SET;
              StatusFlag.stStatusFlag.bErrorFlag = SET;/*Error SET*/	
#ifdef DBG_CAN_PARSER_FAULT      
              printf("PRESSURE GAUGE2 HIGH THRESHOLD\r\n");
#endif
            }
          }
          if(BIT2_SET==(ucRec_Data[DATA_BYTE_ZERO] & BIT2_MASK))
          {
            
#ifdef DBG_CAN_PARSER_FAULT      
            printf("VCM PRESSURE GAUGE3 FAULT \r\n");
#endif	
            if(BIT0_SET == (ucRec_Data[DATA_BYTE_ONE] & BIT0_MASK))
            {
              Fault_Log.stFault_Log_Flag.bFault_Log_PG3LTF = SET;
              StatusFlag.stStatusFlag.bErrorSentFlag = SET;
              StatusFlag.stStatusFlag.bWarningFlag = SET;/*Warnning SET*/
#ifdef DBG_CAN_PARSER_FAULT      
              printf("PRESSURE GAUGE3 LOW THRESHOLD\r\n");
#endif
            }
            if(BIT1_SET == (ucRec_Data[DATA_BYTE_ONE] & BIT1_MASK))
            {
              Fault_Log.stFault_Log_Flag.bFault_Log_PG3HTF = SET;
              StatusFlag.stStatusFlag.bErrorSentFlag = SET;
              StatusFlag.stStatusFlag.bErrorFlag = SET;/*Error SET*/	
#ifdef DBG_CAN_PARSER_FAULT      
              printf("PRESSURE GAUGE3 HIGH THRESHOLD\r\n");
#endif
            }
          }
         	if((RESET == stEthernetLog.ucSendFlag)||(RESET == stWifiLog.ucSendFlag))  /*change65*/
			{
          fnFault_Log();
			}
#ifdef DBG_CAN_PARSER_FAULT      
          printf("FAULT_PRESSURE_BEYOND_TH \r\n");
#endif	
        }
        break;
        /* Fault meassge recieved from HCM if Liquid Level goes out of range of material
        have to forward this message to TABLET also*/
      case FAULT_LQ_LV_BEYOND_TH:
        {		
          /******************FORWARD MESSAGE TO TABLET**********************/	
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          
          /****************************ABK*****************************/
          /*change46*/
				//fault messages are sent without source address change
					//    pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
           //                     (DEST_TABLET	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          //						stWifi.ucSendFlag = SET;
          //						stEthernet.ucSendFlag = SET;
          fnXML_FIFO(&stTx1Message);
          /************************************************************/
          if(BIT0_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK))
          {
            
#ifdef DBG_CAN_PARSER_FAULT      
            printf("HCM BASE TANK LL FAULT \r\n");
#endif	
            if(BIT0_SET == (ucRec_Data[DATA_BYTE_ONE] & BIT0_MASK))
            {
              Fault_Log.stFault_Log_Flag.bFault_Log_LLBLTF = SET;
              StatusFlag.stStatusFlag.bErrorSentFlag = SET;
              StatusFlag.stStatusFlag.bWarningFlag = SET;/*Warnning SET*/
#ifdef DBG_CAN_PARSER_FAULT      
              printf("BASE TANK LL LOW THRESHOLD\r\n");
#endif
            }
            if(BIT1_SET == (ucRec_Data[DATA_BYTE_ONE] & BIT1_MASK))
            {
              Fault_Log.stFault_Log_Flag.bFault_Log_LLBHTF = SET;
              StatusFlag.stStatusFlag.bErrorSentFlag = SET;
              StatusFlag.stStatusFlag.bWarningFlag = SET;/*Warnning SET*/	
#ifdef DBG_CAN_PARSER_FAULT      
              printf("BASE TANK LL HIGH THRESHOLD\r\n");
#endif
            }
            
          }
          if(BIT1_SET==(ucRec_Data[DATA_BYTE_ZERO] & BIT1_MASK))
          {
            
#ifdef DBG_CAN_PARSER_FAULT      
            printf("HCM HARD TANK LL FAULT \r\n");
#endif	
            if(BIT0_SET == (ucRec_Data[DATA_BYTE_ONE] & BIT0_MASK))
            {
              Fault_Log.stFault_Log_Flag.bFault_Log_LLHLTF = SET;
              StatusFlag.stStatusFlag.bErrorSentFlag = SET;
              StatusFlag.stStatusFlag.bWarningFlag = SET;/*Warnning SET*/
#ifdef DBG_CAN_PARSER_FAULT      
              printf("HARD TANK LL LOW THRESHOLD\r\n");
#endif
            }
            if(BIT1_SET == (ucRec_Data[DATA_BYTE_ONE] & BIT1_MASK))
            {
              Fault_Log.stFault_Log_Flag.bFault_Log_LLHHTF = SET;
              StatusFlag.stStatusFlag.bErrorSentFlag = SET;
              StatusFlag.stStatusFlag.bWarningFlag = SET;/*Warnning SET*/
#ifdef DBG_CAN_PARSER_FAULT      
              printf("HARD TANK LL HIGH THRESHOLD\r\n");
#endif
            }
          }
          	if((RESET == stEthernetLog.ucSendFlag)||(RESET == stWifiLog.ucSendFlag))  /*change65*/
			{
          fnFault_Log();
			}
#ifdef DBG_CAN_PARSER_FAULT      
          printf("FAULT_LQ_LV_BEYOND_TH \r\n");
#endif	
        }
        break;
        /* Fault message recieved from HCM if Heater fault occurs 
        have to forward this message to TABLET also*/
      case FAULT_HEATER:
        {
          /******************FORWARD MESSAGE TO TABLET**********************/	
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          /****************************ABK*****************************/
          /*change46*/
				//fault messages are sent without source address change
					//    pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
           //                     (DEST_TABLET	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          //						stWifi.ucSendFlag = SET;
          //						stEthernet.ucSendFlag = SET;
          fnXML_FIFO(&stTx1Message);
          /************************************************************/
          
          if(BIT0_SET==(ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK))
          {
            Fault_Log.stFault_Log_Flag.bFault_Log_IBHF = SET;
#ifdef DBG_CAN_PARSER_FAULT      
            printf("INLINE BASE HEATER FAULT\r\n");
#endif	
            
          }
          if(BIT1_SET==(ucRec_Data[DATA_BYTE_ZERO] & BIT1_MASK))
          {
            Fault_Log.stFault_Log_Flag.bFault_Log_IHHF = SET;
#ifdef DBG_CAN_PARSER_FAULT      
            printf("INLINE HARD HEATER FAULT \r\n");
#endif	
          }
          if(BIT2_SET==(ucRec_Data[DATA_BYTE_ZERO] & BIT2_MASK))
          {
            Fault_Log.stFault_Log_Flag.bFault_Log_AHF = SET;
#ifdef DBG_CAN_PARSER_FAULT      
            printf("ANTIFREEZE HEATER FAULT\r\n");
#endif
          }
          if(BIT3_SET==(ucRec_Data[DATA_BYTE_ZERO] & BIT3_MASK))
          {
            Fault_Log.stFault_Log_Flag.bFault_Log_HHF = SET;
#ifdef DBG_CAN_PARSER_FAULT      
            printf("HARDENER BAND HEATER FAULT  \r\n");
#endif
            
          }
          
          StatusFlag.stStatusFlag.bWarningFlag = SET;/*Warnning SET*/
          StatusFlag.stStatusFlag.bErrorSentFlag = SET;	
					if((RESET == stEthernetLog.ucSendFlag)||(RESET == stWifiLog.ucSendFlag))  /*change65*/
			{
          fnFault_Log();
			}
#ifdef DBG_CAN_PARSER_FAULT      
          printf("FAULT_HEATER \r\n");
#endif	
        }
        break;
        /* Fault message recieved from OPMM if Spray Gun required more time than 
        set time to go from spray position to prime position or vice versa  
        have to forward this message to TABLET also*/
      case FAULT_SPRAY_GUN_MOTION:
        {
          /******************FORWARD MESSAGE TO HMI FROM OPMM**********************/	
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          /****************************ABK*****************************/
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_HMI	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          fnCANMsg_Transmit(&stTx1Message, CAN1);
          /************************************************************/
          /******************FORWARD MESSAGE TO TABLET**********************/	
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          /****************************ABK*****************************/
           /*change46*/
				//fault messages are sent without source address change
					//    pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
           //                     (DEST_TABLET	 | SOURCE_SCU));
          //						stWifi.ucSendFlag = SET;
          //						stEthernet.ucSendFlag = SET;
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          fnXML_FIFO(&stTx1Message);
          /************************************************************/
          StatusFlag.stStatusFlag.bWarningFlag = SET;/*Warnning SET*/
          StatusFlag.stStatusFlag.bErrorSentFlag = SET;
          Fault_Log.stFault_Log_Flag.bFault_Log_SGMF = SET;
          StatusFlag.stStatusFlag.bPrimeSelector = RESET;
          StatusFlag.stStatusFlag.bSpraySelector = RESET;
          StatusFlag.stStatusFlag.bFlushSelector = RESET;
          StatusFlag.stStatusFlag.bInProgress		=	RESET;
					if((RESET == stEthernetLog.ucSendFlag)||(RESET == stWifiLog.ucSendFlag))  /*change65*/
			{
          fnFault_Log();
					}						
#ifdef DBG_CAN_PARSER_FAULT      
          printf("FAULT_SPRAY_GUN_MOTION \r\n");
#endif	
        }
        break;
        /* Fault message recieved from VCM if Linear Transducer reading goes out of range 
        have to forward this message to TABLET also*/
      case FAULT_LT_OUT_OF_RANGE:
        {
          /******************FORWARD MESSAGE TO TABLET**********************/	
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          /****************************ABK*****************************/
          /*change46*/
				//fault messages are sent without source address change
					//    pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
           //                     (DEST_TABLET	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          //						stWifi.ucSendFlag = SET;
          //						stEthernet.ucSendFlag = SET;
          fnXML_FIFO(&stTx1Message);
          /************************************************************/
          StatusFlag.stStatusFlag.bErrorSentFlag = SET;
          StatusFlag.stStatusFlag.bErrorFlag = SET;/*Error SET*/	
          Fault_Log.stFault_Log_Flag.bFault_Log_LTF = SET;
					if((RESET == stEthernetLog.ucSendFlag)||(RESET == stWifiLog.ucSendFlag))  /*change65*/
			{
          fnFault_Log();	
					}						
#ifdef DBG_CAN_PARSER_FAULT      
          printf("FAULT_LT_OUT_OF_RANGE \r\n");
#endif	
        }
        break;
        /* Fault message recieved from OPMM if OPMM required more time than 
        set time to go from HLS to DCLS or vice versa  
        have to forward this message to TABLET  & HMI also*/
      case FAULT_OPMM_ROTATION:
        {
          /******************FORWARD MESSAGE TO HMI FROM OPMM**********************/	
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          /****************************ABK*****************************/
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_HMI	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          fnCANMsg_Transmit(&stTx1Message, CAN1);
          /******************FORWARD MESSAGE TO TABLET**********************/	
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          /****************************ABK*****************************/
           /*change46*/
				//fault messages are sent without source address change
					//    pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
           //                     (DEST_TABLET	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          //						stWifi.ucSendFlag = SET;
          //						stEthernet.ucSendFlag = SET;
          fnXML_FIFO(&stTx1Message);
          /************************************************************/
          
          if(BIT0_SET==(ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK))
          {
            Fault_Log.stFault_Log_Flag.bFault_Log_DCLSTF = SET;
#ifdef DBG_CAN_PARSER_FAULT      
            printf("DCLS TIMER FAULT \r\n");
#endif	
            
          }
          if(BIT1_SET==(ucRec_Data[DATA_BYTE_ZERO] & BIT1_MASK))
          {
            Fault_Log.stFault_Log_Flag.bFault_Log_HLSTF = SET;
#ifdef DBG_CAN_PARSER_FAULT      
            printf("HLS TIMER FAULT \r\n");
#endif	
          }
          StatusFlag.stStatusFlag.bErrorFlag = SET;/*Error SET*/	
          StatusFlag.stStatusFlag.bErrorSentFlag = SET;
					if((RESET == stEthernetLog.ucSendFlag)||(RESET == stWifiLog.ucSendFlag))  /*change65*/
			{
          fnFault_Log();	
					}						
#ifdef DBG_CAN_PARSER_FAULT      
          printf("FAULT_OPMM_ROTATION \r\n");
#endif	
        }
        break;
        /* Fault message recieved from OPMM if OPMM required more time than 
        set time to go from HLS to DCLS or vice versa  
        have to forward this message to TABLET  & HMI also*/
      case FAULT_SPRAY_SEQUENCE_ERROR:
        {
          /******************FORWARD MESSAGE TO HMI FROM OPMM**********************/	
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          /****************************ABK*****************************/
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_HMI	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          fnCANMsg_Transmit(&stTx1Message, CAN1);
          /******************FORWARD MESSAGE TO TABLET**********************/	
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          /****************************ABK*****************************/
          /*change46*/
				//fault messages are sent without source address change
					//    pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
           //                     (DEST_TABLET	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          //						stWifi.ucSendFlag = SET;
          //						stEthernet.ucSendFlag = SET;
          fnXML_FIFO(&stTx1Message);
          /************************************************************/
#ifdef DBG_CAN_PARSER_FAULT      
          printf("FAULT_SPRAY_SEQUENCE_ERROR \r\n");
#endif	
          
        }
        break;
        /* Fault message recieved from OPMM  
        have to forward this message to TABLET  & HMI also*/
        
      case FAULT_SPRAY_DEBUG_ERROR:
        {
          /******************FORWARD MESSAGE TO HMI FROM OPMM**********************/	
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          /****************************ABK*****************************/
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_HMI	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          fnCANMsg_Transmit(&stTx1Message, CAN1);
          /******************FORWARD MESSAGE TO TABLET**********************/	
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          /****************************ABK*****************************/
          /*change46*/
				//fault messages are sent without source address change
					//    pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
           //                     (DEST_TABLET	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          //						stWifi.ucSendFlag = SET;
          //						stEthernet.ucSendFlag = SET;
          fnXML_FIFO(&stTx1Message);
          /************************************************************/
#ifdef DBG_CAN_PARSER_FAULT      
          printf("FAULT_SPRAY_DEBUG_ERROR \r\n");
#endif	
          
        }
        break;	
        /*change33*/
      case FAULT_LOG_MEMORY_FULL_ERROR:
        {
          
				
          /************************************************************/
          if(BIT4_SET==(ucRec_Data[DATA_BYTE_ZERO] & BIT4_MASK))
          {
            stWarnLog.stWarnLogFlag.bDiagWarn = RESET;
            stWarnLog.stWarnLogFlag.bDiagWarnTimerStart = RESET;
            stWarningLog.uiWarnLog_DiagTimerCounter = RESET;
#ifdef DBG_CAN_PARSER_FAULT      
            printf("DIAGNOSTIC LOG MEMORY FULL ERROR ACKNOWLEDGE \r\n");
#endif	
            
          }
          if(BIT5_SET==(ucRec_Data[DATA_BYTE_ZERO] & BIT5_MASK))
          {
            stWarnLog.stWarnLogFlag.bFaultWarn = RESET;
            stWarnLog.stWarnLogFlag.bFaultWarnTimerStart = RESET;
            stWarningLog.uiWarnLog_FaultTimerCounter = RESET;
#ifdef DBG_CAN_PARSER_FAULT      
            printf("FAULT LOG MEMORY FULL ERROR ACKNOWLEDGE \r\n");
#endif	
          }
          if(BIT6_SET==(ucRec_Data[DATA_BYTE_ZERO] & BIT6_MASK))
          {
            stWarnLog.stWarnLogFlag.bStatusWarn = RESET;
            stWarnLog.stWarnLogFlag.bStatusWarnTimerStart = RESET;
            stWarningLog.uiWarnLog_StatusTimerCounter = RESET;
#ifdef DBG_CAN_PARSER_FAULT      
            printf("STATUS LOG MEMORY FULL ERROR ACKNOWLEDGE \r\n");
#endif	
          }
          if(BIT7_SET==(ucRec_Data[DATA_BYTE_ZERO] & BIT7_MASK))
          {
            stWarnLog.stWarnLogFlag.bPeriodicWarn = RESET;
            stWarnLog.stWarnLogFlag.bPeriodicWarnTimerStart = RESET;
            stWarningLog.uiWarnLog_PeriodicTimerCounter = RESET;
#ifdef DBG_CAN_PARSER_FAULT      
            printf("PERIODIC LOG MEMORY FULL ERROR ACKNOWLEDGE \r\n");
#endif	
          }
          
      /*change46*/
      stTx1Message.DLC = ONE_BYTES; 
      stTx1Message.Data[DATA_BYTE_ZERO] = ACK_FOR_MEMORY_FULL;
      			
      /****************************ABK*****************************/
      stTx1Message.ExtId = (MSG_TYPE_FAULT | FAULT_LOG_MEMORY_FULL_ERROR | 
                            (DEST_TABLET	 | SOURCE_SCU));
      
      ////						stWifi.ucSendFlag = SET;
      ////						stEthernet.ucSendFlag = SET;
      fnXML_FIFO(&stTx1Message);
        
#ifdef DBG_CAN_PARSER_FAULT      
          printf("FAULT_LOG_MEMORY_FULL_ERROR \r\n");
#endif	
        }
        break;
      }
    }
    break;
    /************STATUS MESSAGE*************/
  case MSG_TYPE_STATUS:
    {
#ifdef DBG_CAN_PARSER_STATUS      
      printf("MSG_TYPE_STATUS \r\n");
#endif	
      switch (uliCanID & MSG_MASK)
      {
        /* Acknowledge recieved for Status message */
      case ACKNOWLEDGEMENT:
        {
          fnCANPost_Ack(pstCANrxmsg,CAN1);
#ifdef DBG_CAN_PARSER_STATUS      
          printf("ACKNOWLEDGE FOR STATUS MESSAGE..\r\n");
#endif	
        }
        break;
        /* Status message recieved from VCM about the stall result whether pass or fail
        have to forward this message to TABLET also*/
      case STATUS_STALL_TEST_RESULT:
        {
#ifdef DBG_CAN_PARSER_STATUS      
          printf("STATUS_STALL_TEST_RESULT \r\n");
#endif	
					
          /******************FORWARD MESSAGE TO TABLET**********************/	
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          
          /****************************ABK*****************************/
           /*change46*/
				//fault messages are sent without source address change
					//    pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
           //                     (DEST_TABLET	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          //						stWifi.ucSendFlag = SET;
          //						stEthernet.ucSendFlag = SET;
          fnXML_FIFO(&stTx1Message);
          /************************************************************/
          /***********************LEGA************************/
          if(BIT0_SET == (ucRec_Data[DATA_BYTE_ONE] & BIT0_MASK))
          {
            if(BIT0_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK))
            {  
							ucUpstrokeStallTestReceived |= BIT0_SET; /*change71*/
              ucUpstrokeStallTestResult = RESET;
#ifdef DBG_CAN_PARSER_STATUS      
              printf("UPSTROKE LEGA STALL TEST FAIL  \r\n");
#endif	
              ucPRDSpray = DATA_BYTE_ZERO ;	//change15
            }
            if(BIT1_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT1_MASK))
            {
							ucUpstrokeStallTestReceived |= BIT0_SET; /*change71*/
              ucUpstrokeStallTestResult = SET;
#ifdef DBG_CAN_PARSER_STATUS      
              printf("UPSTROKE LEGA STALL TEST PASS  \r\n");
#endif	
              ucPRDSpray = DATA_BYTE_ZERO ;	//change15
            }
            if(BIT2_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT2_MASK))
            {
							ucDownstrokeStallTestReceived |= BIT0_SET; /*change71*/
              ucDownstrokeStallTestResult = RESET;
#ifdef DBG_CAN_PARSER_STATUS      
              printf("DOWNSTROKE LEGA STALL TEST FAIL  \r\n");
#endif	
              ucPRDSpray = DATA_BYTE_ZERO ;	//change15
            }
            if(BIT3_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT3_MASK))
            {
							ucDownstrokeStallTestReceived |= BIT0_SET; /*change71*/
              ucDownstrokeStallTestResult = SET;
#ifdef DBG_CAN_PARSER_STATUS      
              printf("DOWNSTROKE LEGA STALL TEST PASS  \r\n");
#endif	
            }
          }
          /***********************LEGB************************/
          if(BIT1_SET == (ucRec_Data[DATA_BYTE_ONE] & BIT1_MASK))
          {
            if(BIT0_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK))
            {
							ucUpstrokeStallTestReceived |= BIT1_SET; /*change71*/
              ucUpstrokeStallTestResult = RESET;
#ifdef DBG_CAN_PARSER_STATUS      
              printf("UPSTROKE LEGB STALL TEST FAIL  \r\n");
#endif	
              ucPRDSpray = DATA_BYTE_ZERO ;	//change15
            }
            if(BIT1_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT1_MASK))
            {
							ucUpstrokeStallTestReceived |= BIT1_SET; /*change71*/
              ucUpstrokeStallTestResult = SET;
#ifdef DBG_CAN_PARSER_STATUS      
              printf("UPSTROKE LEGB STALL TEST PASS  \r\n");
#endif	
              ucPRDSpray = DATA_BYTE_ZERO ;	//change15
            }
            if(BIT2_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT2_MASK))
            {
							ucDownstrokeStallTestReceived |= BIT1_SET; /*change71*/
              ucDownstrokeStallTestResult = RESET;
#ifdef DBG_CAN_PARSER_STATUS      
              printf("DOWNSTROKE LEGB STALL TEST FAIL  \r\n");
#endif	
              ucPRDSpray = DATA_BYTE_ZERO ;	//change15
            }
            if(BIT3_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT3_MASK))
            {
							ucDownstrokeStallTestReceived |= BIT1_SET; /*change71*/
              ucDownstrokeStallTestResult = SET;
#ifdef DBG_CAN_PARSER_STATUS      
              printf("DOWNSTROKE LEGB STALL TEST PASS  \r\n");
#endif	
            }
          }
          /***********************LEGC************************/
          if(BIT2_SET == (ucRec_Data[DATA_BYTE_ONE] & BIT2_MASK))
          {
            if(BIT0_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK))
            {
							ucUpstrokeStallTestReceived |= BIT2_SET; /*change71*/
              ucUpstrokeStallTestResult = RESET;
#ifdef DBG_CAN_PARSER_STATUS      
              printf("UPSTROKE LEGC STALL TEST FAIL  \r\n");
#endif	
              ucPRDSpray = DATA_BYTE_ZERO; //change15
            }
            if(BIT1_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT1_MASK))
            {
							ucUpstrokeStallTestReceived |= BIT2_SET; /*change71*/
              ucUpstrokeStallTestResult = SET;
#ifdef DBG_CAN_PARSER_STATUS      
              printf("UPSTROKE LEGC STALL TEST PASS  \r\n");
#endif	
              ucPRDSpray = DATA_BYTE_ZERO ;	//change15
              
            }
            if(BIT2_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT2_MASK))
            {
							ucDownstrokeStallTestReceived |= BIT2_SET; /*change71*/
              ucDownstrokeStallTestResult = RESET;
#ifdef DBG_CAN_PARSER_STATUS      
              printf("DOWNSTROKE LEGC STALL TEST FAIL  \r\n");
#endif	
              ucPRDSpray = DATA_BYTE_ZERO ;	//change15
              
            }
            if(BIT3_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT3_MASK))
            {
							ucDownstrokeStallTestReceived |= BIT2_SET; /*change71*/
              ucDownstrokeStallTestResult = SET;
#ifdef DBG_CAN_PARSER_STATUS      
              printf("DOWNSTROKE LEGC STALL TEST PASS  \r\n");
#endif	
              
            }
          }
          
          /*********************FOR STATUS LOG ********************/
          /*change71*/
//					if(RESET == ucDownstrokeStallTestResult)
//					{
//						ucUpstrokeStallTestReceived |= STALL_RESULT_RECIEVED;
//						ucUpstrokeStallTestResult = RESET;
//					}
					if((STALL_RESULT_RECIEVED == ucUpstrokeStallTestReceived) && 
						(STALL_RESULT_RECIEVED == ucDownstrokeStallTestReceived))
					{
						/*change71*/
						ucUpstrokeStallTestReceived = RESET;
						ucDownstrokeStallTestReceived = RESET;
          rgucStatusLog[BYTE_FOURTYNINE] 	= 	ucUpstrokeStallTestResult ;
          rgucStatusLog[BYTE_FIFTY] = ucDownstrokeStallTestResult ;
						 StatusFlag.stStatusFlag.bStall_In_Progress = RESET; /*change70*/
          /*change62*/
					if((SET == StatusFlag.stStatusFlag.bStatusLogTake) ||
						(SET == StatusFlag.stStatusFlag.bAuto_Stall_Fail))
					{
					StatusFlag.stStatusFlag.bAuto_Stall_Fail = RESET;
					StatusFlag.stStatusFlag.bStatusLogTake = RESET;
						#ifdef DBG_CAN_PARSER_STATUS 
						printf("Writing Status Log  \r\n");
						#endif
						if(3 > ucStallTestCount) /*change72*/
						{
							fnStatus_Log();
						}	
							/*change71*/
						if((RESET == ucUpstrokeStallTestResult) || 
							(RESET == ucDownstrokeStallTestResult))
						{
							ucStallTestCount++; /*change72*/
							#ifdef DBG_CAN_PARSER_STATUS 
								printf("Failed  \r\n");
							#endif
							 StatusFlag.stStatusFlag.bAuto_Stall_Fail = SET;
						}
						else
						{
							ucStallTestCount = RESET; /*change72*/
							#ifdef DBG_CAN_PARSER_STATUS 
							printf("Passed  \r\n");
							#endif
							StatusFlag.stStatusFlag.bAuto_Stall_Fail = RESET;
						}
					}
				}
         
        }
        break;	
        /* Status message recieved from OPMM about the spray process pass or fail
        have to forward this message to TABLET also*/
      case STATUS_SPRAY_PROCESS:
        {		
#ifdef DBG_CAN_PARSER_STATUS      
          printf("STATUS_SPRAY_PROCESS \r\n");
#endif	
          /******************FORWARD MESSAGE TO HMI FROM OPMM**********************/	
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          /****************************ABK*****************************/
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_HMI	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          fnCANMsg_Transmit(&stTx1Message, CAN1);
          /******************FORWARD MESSAGE TO TABLET**********************/	
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          
          /****************************ABK*****************************/
          /*change46*/
				//fault messages are sent without source address change
					   pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                               (DEST_TABLET	 | SOURCE_OPMM));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          //						stWifi.ucSendFlag = SET;
          //						stEthernet.ucSendFlag = SET;
          fnXML_FIFO(&stTx1Message);
          /************************************************************/
          if(BIT0_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK))
          {
            StatusFlag.stStatusFlag.bSprayProcessDone = SET ;
            StatusFlag.stStatusFlag.bInProgress = RESET ;
            StatusFlag.stStatusFlag.bSprayPosition = RESET ;
            StatusFlag.stStatusFlag.bPrimeSelector = RESET;
            StatusFlag.stStatusFlag.bSpraySelector = RESET;
            StatusFlag.stStatusFlag.bFlushSelector = RESET;
            StatusFlag.stStatusFlag.bSprayProcessStarted = RESET;  //change9
            
            /*************change7*********************/
            StatusFlag.stStatusFlag.bStart_Stall_Flag = SET ;
           /*change74*/
					StatusFlag.stStatusFlag.bErrorFlag = SET;/*Error SET*/	
          StatusFlag.stStatusFlag.bErrorSentFlag = SET;	
					StatusFlag.stStatusFlag.bStop_Flush_flag = SET; //change11	
#ifdef DBG_CAN_PARSER_STATUS      
            printf("SPRAY PROCESS FAIL  \r\n");
#endif	
          }
          if(BIT1_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT1_MASK))
          {
            StatusFlag.stStatusFlag.bSprayProcessDone = SET ;
            StatusFlag.stStatusFlag.bSprayProcessStarted = RESET; //change9
            StatusFlag.stStatusFlag.bInProgress = RESET ;
            StatusFlag.stStatusFlag.bSprayPosition = RESET ;
            /*************change7*********************/
            StatusFlag.stStatusFlag.bStart_Stall_Flag = SET ;
            
#ifdef DBG_CAN_PARSER_STATUS      
            printf("SPRAY PROCESS PASS  \r\n");
#endif	
          }
          /**********************change13 start monitoring of Material************************/	
          /* Send Stop Linear Monitoring to VCM */
          stTx1Message.DLC = ONE_BYTES;
          stTx1Message.Data[DATA_BYTE_ZERO] = BIT0_SET  ;
          stTx1Message.ExtId = 	MSG_TYPE_COMMAND | CMD_LINEAR_TRANS_MONITOR |
            DEST_VCM | SOURCE_SCU;
          fnCANMsg_Transmit(&stTx1Message, CAN1);	
          /********************************************************************************/
          ucPRDSpray = DATA_BYTE_ZERO; //change15
        }
        break;	
        /* Status message recieved from OPMM about the prime process pass or fail
        have to forward this message to TABLET also*/
      case STATUS_PRIME_PROCESS:
        {
#ifdef DBG_CAN_PARSER_STATUS      
          printf("STATUS_PRIME_PROCESS \r\n");
#endif	
          /******************FORWARD MESSAGE TO HMI FROM OPMM**********************/	
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          /****************************ABK*****************************/
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_HMI	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          fnCANMsg_Transmit(&stTx1Message, CAN1);
          /******************FORWARD MESSAGE TO TABLET**********************/	
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          
          /****************************ABK*****************************/
           /*change46*/
				//fault messages are sent without source address change
					//    pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
           //                     (DEST_TABLET	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          //						stWifi.ucSendFlag = SET;
          //						stEthernet.ucSendFlag = SET;
          fnXML_FIFO(&stTx1Message);
          /************************************************************/
          if(BIT0_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK))
          {
            StatusFlag.stStatusFlag.bInProgress = RESET ;
            //	StatusFlag.stStatusFlag.bPrimeSelector = RESET;
            StatusFlag.stStatusFlag.bSpraySelector = RESET;
            StatusFlag.stStatusFlag.bFlushSelector = RESET;
            StatusFlag.stStatusFlag.bPrimeProcessStarted = RESET ;
#ifdef DBG_CAN_PARSER_STATUS      
            printf("PRIME PROCESS FAIL  \r\n");
#endif	
          }
          if(BIT1_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT1_MASK))
          {
            StatusFlag.stStatusFlag.bInProgress = RESET ;
            StatusFlag.stStatusFlag.bPrimeProcessStarted = RESET ;
#ifdef DBG_CAN_PARSER_STATUS      
            printf("PRIME PROCESS PASS  \r\n");
#endif	
          }
          /**********************change13 start monitoring of Material************************/	
          /* Send Stop Linear Monitoring to VCM */
          stTx1Message.DLC = ONE_BYTES;
          stTx1Message.Data[DATA_BYTE_ZERO] = BIT0_SET  ;
          stTx1Message.ExtId = 	MSG_TYPE_COMMAND | CMD_LINEAR_TRANS_MONITOR |
            DEST_VCM | SOURCE_SCU;
          fnCANMsg_Transmit(&stTx1Message, CAN1);	
          /********************************************************************************/
          
        }
        break;	
        /* Status message recieved from OPMM about the flush process pass or fail
        have to forward this message to TABLET also*/
      case STATUS_FLUSH_PROCESS:
        {
#ifdef DBG_CAN_PARSER_STATUS      
          printf("STATUS_FLUSH_PROCESS \r\n");
#endif	
          /******************FORWARD MESSAGE TO HMI FROM OPMM**********************/	
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          /****************************ABK*****************************/
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_HMI	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          fnCANMsg_Transmit(&stTx1Message, CAN1);
          /******************FORWARD MESSAGE TO TABLET**********************/	
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          
          /****************************ABK*****************************/
          /*change46*/
				//fault messages are sent without source address change
					//    pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
           //                     (DEST_TABLET	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          //						stWifi.ucSendFlag = SET;
          //						stEthernet.ucSendFlag = SET;
          fnXML_FIFO(&stTx1Message);
          /************************************************************/
          if(BIT0_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK))
          {
            StatusFlag.stStatusFlag.bInProgress = RESET ;
            StatusFlag.stStatusFlag.bPrimeSelector = RESET;
            StatusFlag.stStatusFlag.bSpraySelector = RESET;
            //	StatusFlag.stStatusFlag.bFlushSelector = RESET;
            StatusFlag.stStatusFlag.bFlushProcessStarted = RESET;
#ifdef DBG_CAN_PARSER_STATUS      
            printf("FLUSH PROCESS FAIL  \r\n");
#endif	
          }
          if(BIT1_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT1_MASK))
          {
            StatusFlag.stStatusFlag.bInProgress = RESET ;
            StatusFlag.stStatusFlag.bFlushProcessStarted = RESET;
            if(SET	== StatusFlag.stStatusFlag.bEStop_Flag )
            {
              ucPreviousProcess = FLUSHPROCESS;    //change12
            }
						
						 
#ifdef DBG_CAN_PARSER_STATUS      
            printf("FLUSH PROCESS PASS  \r\n");
#endif	
          }
					/*change77*/
					 if(SET == StatusFlag.stStatusFlag.bAuto_Flush_flag)
						 {
							 StatusFlag.stStatusFlag.bAuto_Flush_flag = RESET ;  /*change77*/
							 StatusFlag.stStatusFlag.bInProgress = RESET ;
            StatusFlag.stStatusFlag.bPrimeSelector = RESET;
            StatusFlag.stStatusFlag.bSpraySelector = RESET;
          StatusFlag.stStatusFlag.bFlushSelector = RESET;
            StatusFlag.stStatusFlag.bFlushProcessStarted = RESET;
							 ucPreviousProcess = FLUSHPROCESS;
						 }
						 
//          /*************change7*********************/
//          if(SET == StatusFlag.stStatusFlag.bStart_Stall_Flag)
//          {
//            StatusFlag.stStatusFlag.bStart_Stall_Flag = RESET;
//            StatusFlag.stStatusFlag.bStall_Timer_Flag = SET ;
//          }
        }
        break;	
        /* Status message recieved from OPMM about the time required to perform prime process is 
        achieved or not
        have to forward this message to TABLET also*/
      case STATUS_PRIME_TIME_ACHIEVED:
        {
#ifdef DBG_CAN_PARSER_STATUS      
          printf("STATUS_PRIME_TIME_ACHIEVED \r\n");
#endif	
          /******************FORWARD MESSAGE TO HMI FROM OPMM**********************/	
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          /****************************ABK*****************************/
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_HMI	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          fnCANMsg_Transmit(&stTx1Message, CAN1);
          /******************FORWARD MESSAGE TO TABLET**********************/	
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          
          /****************************ABK*****************************/
           /*change46*/
				//fault messages are sent without source address change
					//    pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
           //                     (DEST_TABLET	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          //						stWifi.ucSendFlag = SET;
          //						stEthernet.ucSendFlag = SET;
          fnXML_FIFO(&stTx1Message);
          /************************************************************/
          if(BIT0_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK))
          {
            if(BIT0_SET == (ucRec_Data[DATA_BYTE_ONE] & BIT0_MASK))
            {
              StatusFlag.stStatusFlag.bPrimeTimeAchieve = RESET;
#ifdef DBG_CAN_PARSER_STATUS      
              printf("PRIME TIME FAIL \r\n");
#endif	
            }
            if(BIT1_SET == (ucRec_Data[DATA_BYTE_ONE] & BIT1_MASK))
            {
              StatusFlag.stStatusFlag.bPrimeTimeAchieve = SET;
#ifdef DBG_CAN_PARSER_STATUS      
              printf("PRIME TIME PASS \r\n");
#endif	
            }
          }
          if(BIT1_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT1_MASK))
          {
            if(BIT0_SET == (ucRec_Data[DATA_BYTE_ONE] & BIT0_MASK))
            {
              StatusFlag.stStatusFlag.bFlushTimeAchieve = RESET;
#ifdef DBG_CAN_PARSER_STATUS      
              printf("FLUSH TIME FAIL \r\n");
#endif	
            }
            if(BIT1_SET == (ucRec_Data[DATA_BYTE_ONE] & BIT1_MASK))
            {
              StatusFlag.stStatusFlag.bFlushTimeAchieve = SET;
#ifdef DBG_CAN_PARSER_STATUS      
              printf("FLUSH TIME PASS \r\n");
#endif	
            }
          }
        }
        break;	
        /* Status message recieved from VCM whether stall valve is on or off 
        have to forward this message to TABLET also*/
      case STATUS_STALL_VALVE:
        {
#ifdef DBG_CAN_PARSER_STATUS      
          printf("STATUS_STALL_VALVE \r\n");
#endif	
          /******************FORWARD MESSAGE TO TABLET**********************/	
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          
          /****************************ABK*****************************/
           /*change46*/
				//fault messages are sent without source address change
					//    pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
           //                     (DEST_TABLET	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          //						stWifi.ucSendFlag = SET;
          //						stEthernet.ucSendFlag = SET;
          fnXML_FIFO(&stTx1Message);
          /************************************************************/
          if(BIT0_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK))
          {
            
#ifdef DBG_CAN_PARSER_STATUS      
            printf("STALL VALVE ON \r\n");
#endif
          }	
          else
          {
            
#ifdef DBG_CAN_PARSER_STATUS      
            printf("STALL VALVE OFF \r\n");
#endif
          }		
					/*change53*/
           if(BIT1_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT1_MASK))
          {
            	/* Send Command to OPMM to Turn ON AB Valve   */
						/*change59*/
						if(RESET == StatusFlag.stStatusFlag.bABONCmdFlag)
						{
							StatusFlag.stStatusFlag.bABONCmdFlag = SET;
							stTx1Message.DLC = TWO_BYTES;
							stTx1Message.Data[DATA_BYTE_ZERO] = BIT2_SET  ;
							stTx1Message.Data[DATA_BYTE_ONE] 	= BIT1_SET  ;
							stTx1Message.ExtId = 	MSG_TYPE_COMMAND | CMD_OPMM_VALVE |
								DEST_OPMM | SOURCE_SCU;
							stTx1Message.IDE = CAN_ID_EXT;
							stTx1Message.RTR = CAN_RTR_DATA;
							fnCANMsg_Transmit(&stTx1Message, CAN1);
						}

#ifdef DBG_CAN_PARSER_STATUS      
            printf("RECIRCULATION VALVE ON \r\n");
#endif
          }	
          else
          {
            	/* Send Command to OPMM to Turn OFF AB Valve   */
						/*change59*/
						if(SET == StatusFlag.stStatusFlag.bABONCmdFlag)
						{
							StatusFlag.stStatusFlag.bABONCmdFlag = RESET;
							stTx1Message.DLC = TWO_BYTES;
							stTx1Message.Data[DATA_BYTE_ZERO] = BIT2_SET  ;
							stTx1Message.Data[DATA_BYTE_ONE] 	= BIT0_SET  ;
							stTx1Message.ExtId = 	MSG_TYPE_COMMAND | CMD_OPMM_VALVE |
								DEST_OPMM | SOURCE_SCU;
							stTx1Message.IDE = CAN_ID_EXT;
							stTx1Message.RTR = CAN_RTR_DATA;
							fnCANMsg_Transmit(&stTx1Message, CAN1);
						}
#ifdef DBG_CAN_PARSER_STATUS      
            printf("RECIRCULATION VALVE OFF \r\n");
#endif
          }		
          
        }
        break;	
        /* Status message recieved from OPMM about on/off status of valves 
        have to forward this message to TABLET also*/
      case STATUS_OPMM_VALVE:
        {
#ifdef DBG_CAN_PARSER_STATUS      
          printf("STATUS_OPMM_VALVE \r\n");
#endif	
          /******************FORWARD MESSAGE TO HMI FROM OPMM**********************/	
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          /****************************ABK*****************************/
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_HMI	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          fnCANMsg_Transmit(&stTx1Message, CAN1);
          /******************FORWARD MESSAGE TO TABLET**********************/	
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          
          /****************************ABK*****************************/
           /*change46*/
				//fault messages are sent without source address change
					//    pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
           //                     (DEST_TABLET	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          //						stWifi.ucSendFlag = SET;
          //						stEthernet.ucSendFlag = SET;
          fnXML_FIFO(&stTx1Message);
          /************************************************************/
          if(BIT0_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK))
          {
            
#ifdef DBG_CAN_PARSER_STATUS      
            printf("SPRAY VALVE ON \r\n");
#endif
          }
          else
          {
#ifdef DBG_CAN_PARSER_STATUS      
            printf("SPRAY VALVE OFF \r\n");
#endif
          }	
          /*******************************************/										
          if(BIT1_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT1_MASK))
          {
            
#ifdef DBG_CAN_PARSER_STATUS      
            printf("GUN POSITION VALVE ON \r\n");
#endif
          }		
          else
          {
#ifdef DBG_CAN_PARSER_STATUS      
            printf("GUN POSITION VALVE OFF \r\n");
#endif
          }
          /******************************************************/
          if(BIT2_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT2_MASK))
          {
						/* Send Command to VCM to Turn ON Recirc Valve   */
						/*change53*/
						if(RESET == StatusFlag.stStatusFlag.bRecirculationValveONCmdFlag)
						{
							StatusFlag.stStatusFlag.bRecirculationValveONCmdFlag = SET;
							stTx1Message.DLC = TWO_BYTES;
							stTx1Message.Data[DATA_BYTE_ZERO] = BIT1_SET  ;
							stTx1Message.Data[DATA_BYTE_ONE] 	= BIT1_SET  ;
							stTx1Message.ExtId = 	MSG_TYPE_COMMAND | CMD_STALL_VALVE |
								DEST_VCM | SOURCE_SCU;
							stTx1Message.IDE = CAN_ID_EXT;
							stTx1Message.RTR = CAN_RTR_DATA;
							fnCANMsg_Transmit(&stTx1Message, CAN1);
						}
							#ifdef DBG_CAN_PARSER_STATUS      
													printf("AB VALVE ON \r\n");
							#endif
          }	
          else
          {
					/*change53*/
							if(SET == StatusFlag.stStatusFlag.bRecirculationValveONCmdFlag)
								{
								StatusFlag.stStatusFlag.bRecirculationValveONCmdFlag = RESET;
								stTx1Message.DLC = TWO_BYTES;
								stTx1Message.Data[DATA_BYTE_ZERO] = BIT1_SET  ;
								stTx1Message.Data[DATA_BYTE_ONE] 	= BIT0_SET  ;
								stTx1Message.ExtId = 	MSG_TYPE_COMMAND | CMD_STALL_VALVE |
								DEST_VCM | SOURCE_SCU;
								stTx1Message.IDE = CAN_ID_EXT;
								stTx1Message.RTR = CAN_RTR_DATA;
								fnCANMsg_Transmit(&stTx1Message, CAN1); 
								}
#ifdef DBG_CAN_PARSER_STATUS      
            printf("AB VALVE OFF \r\n");
#endif
          }
          /******************************************************/	
          
          if(BIT3_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT3_MASK))
          {
#ifdef DBG_CAN_PARSER_STATUS      
            printf("SOLVENT VALVE ON \r\n");
#endif
          }
          else
          {
#ifdef DBG_CAN_PARSER_STATUS      
            printf("SOLVENT VALVE OFF \r\n");
#endif
          }		
          /******************************************************/	
          if(BIT4_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT4_MASK))
          {
#ifdef DBG_CAN_PARSER_STATUS      
            printf("FORWARD VALVE ON \r\n");
#endif
          }
          else
          {
#ifdef DBG_CAN_PARSER_STATUS      
            printf("FORWARD VALVE OFF \r\n");
#endif
          }		
          /******************************************************/	
          if(BIT5_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT5_MASK))
          {
#ifdef DBG_CAN_PARSER_STATUS      
            printf("BACKWARD VALVE ON \r\n");
#endif
          }
          else
          {
#ifdef DBG_CAN_PARSER_STATUS      
            printf("BACKWARD VALVE OFF \r\n");
#endif
          }		
          /******************************************************/	
          
        }
        break;	
        /* Status message recieved from HCM about on/off status of SSR 
        have to forward this message to TABLET also*/
      case STATUS_HCM_HEATER:
        {
#ifdef DBG_CAN_PARSER_STATUS      
          printf("STATUS_HCM_HEATER \r\n");
#endif
          /******************FORWARD MESSAGE TO TABLET**********************/	
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          
          /****************************ABK*****************************/
          /*change46*/
				//fault messages are sent without source address change
					//    pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
           //                     (DEST_TABLET	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId;
          //						stWifi.ucSendFlag = SET;
          //						stEthernet.ucSendFlag = SET;						
          fnXML_FIFO(&stTx1Message);
          /************************************************************/
          /******************************************************/	
          if(BIT0_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK))
          {
#ifdef DBG_CAN_PARSER_STATUS      
            printf("INLINE BASE HEATER ON \r\n");
#endif
          }
          else
          {
#ifdef DBG_CAN_PARSER_STATUS      
            printf("INLINE BASE HEATER OFF \r\n");
#endif
          }		
          /******************************************************/	
          
          if(BIT1_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT1_MASK))
          {
#ifdef DBG_CAN_PARSER_STATUS      
            printf("INLINE HARD HEATER ON \r\n");
#endif
          }
          else
          {
#ifdef DBG_CAN_PARSER_STATUS      
            printf("INLINE HARD HEATER OFF \r\n");
#endif
          }		
          /******************************************************/	
          if(BIT2_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT2_MASK))
          {
#ifdef DBG_CAN_PARSER_STATUS      
            printf("ANTIFREEZE HEATER ON \r\n");
#endif
          }
          else
          {
#ifdef DBG_CAN_PARSER_STATUS      
            printf("ANTIFREEZE HEATER OFF \r\n");
#endif
          }		
          /******************************************************/	
          if(BIT3_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT3_MASK))
          {
#ifdef DBG_CAN_PARSER_STATUS      
            printf("HARD TANK HEATER ON \r\n");
#endif
          }
          else
          {
#ifdef DBG_CAN_PARSER_STATUS      
            printf("HARD TANK HEATER OFF \r\n");
#endif
          }		
          /******************************************************/	
          
          
        }
        break;	
        /* Status message recieved from OPMM about position of GUN whether Prime or Spray 
        have to forward this message to TABLET also*/
      case STATUS_GUN_POSITION:
        {
#ifdef DBG_CAN_PARSER_STATUS      
          printf("STATUS_GUN_POSITION \r\n");
#endif	
          /******************FORWARD MESSAGE TO HMI FROM OPMM**********************/	
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          /****************************ABK*****************************/
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_HMI	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          fnCANMsg_Transmit(&stTx1Message, CAN1);
          /******************FORWARD MESSAGE TO TABLET**********************/	
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          
          /****************************ABK*****************************/
          /*change46*/
				//fault messages are sent without source address change
					//    pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
           //                     (DEST_TABLET	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          //						stWifi.ucSendFlag = SET;
          //						stEthernet.ucSendFlag = SET;
          fnXML_FIFO(&stTx1Message);
          /************************************************************/
          if(BIT0_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK))
          {
            StatusFlag.stStatusFlag.bPrimePosition = SET ;
            StatusFlag.stStatusFlag.bFlushPosition = SET ;
            StatusFlag.stStatusFlag.bSprayPosition = RESET ;
         //   printf("PRIME/FLUSH POSITION \r\n");
#ifdef DBG_CAN_PARSER_STATUS      
            printf("PRIME POSITION \r\n");
#endif
          }	
          if(BIT1_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT1_MASK))
          {
            StatusFlag.stStatusFlag.bPrimePosition = RESET ;
            StatusFlag.stStatusFlag.bFlushPosition = RESET ;
            StatusFlag.stStatusFlag.bSprayPosition = SET ;
#ifdef DBG_CAN_PARSER_STATUS      
            printf("SPRAY POSITION \r\n");
#endif
          }	
        }
        break;	
        /* Status message recieved from VCM about Diagnostic result of peripherals connected to VCM
        have to forward this message to TABLET also*/
      case STATUS_VCM_DIAGNOSTIC:
        {
#ifdef DBG_CAN_PARSER_STATUS      
          printf("STATUS_VCM_DIAGNOSTIC \r\n");
#endif	
          /******************FORWARD MESSAGE TO TABLET**********************/	
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          
          /****************************ABK*****************************/
          /*change46*/
				//fault messages are sent without source address change
					//    pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
           //                     (DEST_TABLET	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          //						stWifi.ucSendFlag = SET;
          //						stEthernet.ucSendFlag = SET;
          fnXML_FIFO(&stTx1Message);
          /************************************************************/
          if(BIT0_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK))
          {
            StatusFlag.stStatusFlag.bErrorFlag = SET;
            ucDeviceDiagStatus &= ~(1 << VCM);
						/*change61*/
						 ucDeviceDiagRx |= (1 << VCM);
            Diag_Log.stDiag_Log_Flag.bDiag_Log_VCM = SET ;
#ifdef DBG_CAN_PARSER_STATUS      
            printf("VCM DIAGNOSTIC FAIL \r\n");
#endif
            if(BIT0_SET == (ucRec_Data[DATA_BYTE_ONE] & BIT0_MASK))
            {
              StatusFlag.stStatusFlag.bErrorFlag = SET;
              Diag_Log.stDiag_Log_Flag.bDiag_Log_SVC = SET ;
#ifdef DBG_CAN_PARSER_STATUS      
              printf("STALL VALVE CONNECTION ERROR \r\n");
#endif
            }	
            if(BIT1_SET == (ucRec_Data[DATA_BYTE_ONE] & BIT1_MASK))
            {StatusFlag.stStatusFlag.bErrorFlag = SET;
            Diag_Log.stDiag_Log_Flag.bDiag_Log_SV = SET ;
#ifdef DBG_CAN_PARSER_STATUS      
            printf("STALL VALVE STATUS ERROR \r\n");
#endif
            }		
            if(BIT2_SET == (ucRec_Data[DATA_BYTE_ONE] & BIT2_MASK))
            {StatusFlag.stStatusFlag.bErrorFlag = SET;
            Diag_Log.stDiag_Log_Flag.bDiag_Log_PG1 = SET ;
#ifdef DBG_CAN_PARSER_STATUS      
            printf("PRESSURE GAUGE 1 ERROR \r\n");
#endif
            }	
            if(BIT3_SET == (ucRec_Data[DATA_BYTE_ONE] & BIT3_MASK))
            {StatusFlag.stStatusFlag.bErrorFlag = SET;
            Diag_Log.stDiag_Log_Flag.bDiag_Log_PG2 = SET ;
#ifdef DBG_CAN_PARSER_STATUS      
            printf("PRESSURE GAUGE 2 ERROR \r\n");
#endif
            }		
            if(BIT4_SET == (ucRec_Data[DATA_BYTE_ONE] & BIT4_MASK))
            {StatusFlag.stStatusFlag.bErrorFlag = SET;
            Diag_Log.stDiag_Log_Flag.bDiag_Log_PG3 = SET ;
#ifdef DBG_CAN_PARSER_STATUS      
            printf("PRESSURE GAUGE 3 ERROR \r\n");
#endif
            }		
            if(BIT5_SET == (ucRec_Data[DATA_BYTE_ONE] & BIT5_MASK))
            {StatusFlag.stStatusFlag.bErrorFlag = SET;
            Diag_Log.stDiag_Log_Flag.bDiag_Log_LT = SET ;
#ifdef DBG_CAN_PARSER_STATUS      
            printf("LINEAR TRANSDUCER ERROR \r\n");
#endif
            }		
            if(BIT6_SET == (ucRec_Data[DATA_BYTE_ONE] & BIT6_MASK))
            {StatusFlag.stStatusFlag.bErrorFlag = SET;
            Diag_Log.stDiag_Log_Flag.bDiag_Log_ASI = SET ;
#ifdef DBG_CAN_PARSER_STATUS      
            printf("INSIDE AMBIENT SENSOR ERROR\r\n");
#endif
            }
            if(BIT7_SET == (ucRec_Data[DATA_BYTE_ONE] & BIT7_MASK))
            {StatusFlag.stStatusFlag.bErrorFlag = SET;
            Diag_Log.stDiag_Log_Flag.bDiag_Log_ASO = SET ;
#ifdef DBG_CAN_PARSER_STATUS      
            printf("OUTSIDE AMBIENT SENSOR ERROR\r\n");
#endif
            }		
            if(BIT0_SET == (ucRec_Data[DATA_BYTE_TWO] & BIT0_MASK))
            {
              StatusFlag.stStatusFlag.bErrorFlag = SET;
              Diag_Log.stDiag_Log_Flag.bDiag_Log_RVC = SET ;
#ifdef DBG_CAN_PARSER_STATUS      
              printf("RECIRCULATION VALVE CONNECTION ERROR \r\n");
#endif
            }	
						/*change54*/
						/*change53*/
            if(BIT1_SET == (ucRec_Data[DATA_BYTE_TWO] & BIT1_MASK))
            {
						StatusFlag.stStatusFlag.bErrorFlag = SET;
            Diag_Log.stDiag_Log_Flag.bDiag_Log_RV = SET ;
#ifdef DBG_CAN_PARSER_STATUS      
            printf("RECIRCULATION VALVE STATUS ERROR \r\n");
#endif
            }							
          }
          if(BIT1_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT1_MASK))
          {
            ucDeviceDiagStatus |= (1 << VCM);
						/*change61*/
						 ucDeviceDiagRx |= (1 << VCM);
            Diag_Log.stDiag_Log_Flag.bDiag_Log_VCM = RESET ;
#ifdef DBG_CAN_PARSER_STATUS      
            printf("VCM DIAGNOSTIC PASS \r\n");
#endif
          }	
        }
        break;	
        /* Status message recieved from HCM about Diagnostic result of peripherals connected to HCM
        have to forward this message to TABLET also*/
      case STATUS_HCM_DIAGNOSTIC:
        {
#ifdef DBG_CAN_PARSER_STATUS      
          printf("STATUS_HCM_DIAGNOSTIC \r\n");
#endif	
          /******************FORWARD MESSAGE TO TABLET**********************/	
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          
          /****************************ABK*****************************/
         /*change46*/
				//fault messages are sent without source address change
					//    pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
           //                     (DEST_TABLET	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          //						stWifi.ucSendFlag = SET;
          //						stEthernet.ucSendFlag = SET;
          fnXML_FIFO(&stTx1Message);
          /************************************************************/
          if(BIT0_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK))
          {StatusFlag.stStatusFlag.bErrorFlag = SET;
          ucDeviceDiagStatus &= ~(1 << HCM);
						/*change61*/
						 ucDeviceDiagRx |= (1 << HCM);
          Diag_Log.stDiag_Log_Flag.bDiag_Log_HCM = SET ;
#ifdef DBG_CAN_PARSER_STATUS      
          printf("HCM DIAGNOSTIC FAIL \r\n");
#endif
          if(BIT0_SET == (ucRec_Data[DATA_BYTE_ONE] & BIT0_MASK))
          {StatusFlag.stStatusFlag.bErrorFlag = SET;
          Diag_Log.stDiag_Log_Flag.bDiag_Log_IBH = SET ;
#ifdef DBG_CAN_PARSER_STATUS      
          printf("INLINE BASE HEATER ERROR \r\n");
#endif
          }	
          if(BIT1_SET == (ucRec_Data[DATA_BYTE_ONE] & BIT1_MASK))
          {StatusFlag.stStatusFlag.bErrorFlag = SET;
          Diag_Log.stDiag_Log_Flag.bDiag_Log_IHH = SET ;
#ifdef DBG_CAN_PARSER_STATUS      
          printf("INLINE HARDENER HEATER ERROR \r\n");
#endif
          }		
          if(BIT2_SET == (ucRec_Data[DATA_BYTE_ONE] & BIT2_MASK))
          {StatusFlag.stStatusFlag.bErrorFlag = SET;
          Diag_Log.stDiag_Log_Flag.bDiag_Log_BH = SET ;
#ifdef DBG_CAN_PARSER_STATUS      
          printf("ANTIFREEZE HEATER ERROR \r\n");
#endif
          }	
          if(BIT3_SET == (ucRec_Data[DATA_BYTE_ONE] & BIT3_MASK))
          {StatusFlag.stStatusFlag.bErrorFlag = SET;
          Diag_Log.stDiag_Log_Flag.bDiag_Log_HH = SET ;
#ifdef DBG_CAN_PARSER_STATUS      
          printf("HARD TANK HEATER ERROR \r\n");
#endif
          }		
          if(BIT4_SET == (ucRec_Data[DATA_BYTE_ONE] & BIT4_MASK))
          {StatusFlag.stStatusFlag.bErrorFlag = SET;
          Diag_Log.stDiag_Log_Flag.bDiag_Log_IBT = SET ;
#ifdef DBG_CAN_PARSER_STATUS      
          printf("INLINE BASE THERMOCOUPLE ERROR \r\n");
#endif
          }		
          if(BIT5_SET == (ucRec_Data[DATA_BYTE_ONE] & BIT5_MASK))
          {StatusFlag.stStatusFlag.bErrorFlag = SET;
          Diag_Log.stDiag_Log_Flag.bDiag_Log_IHT = SET ;
#ifdef DBG_CAN_PARSER_STATUS      
          printf("INLINE HARDENER THERMOCOUPLE ERROR\r\n");
#endif
          }			
          if(BIT6_SET == (ucRec_Data[DATA_BYTE_ONE] & BIT6_MASK))
          {StatusFlag.stStatusFlag.bErrorFlag = SET;
          Diag_Log.stDiag_Log_Flag.bDiag_Log_AT = SET ;
#ifdef DBG_CAN_PARSER_STATUS      
          printf("ANTIFREEZE TEMPERATURE THERMOCOUPLE ERROR\r\n");
#endif
          }	
          if(BIT7_SET == (ucRec_Data[DATA_BYTE_ONE] & BIT7_MASK))
          {StatusFlag.stStatusFlag.bErrorFlag = SET;
          Diag_Log.stDiag_Log_Flag.bDiag_Log_BRT = SET ;
#ifdef DBG_CAN_PARSER_STATUS      
          printf("BASE RETURN LINE TEMPERATURE THERMOCOUPLE ERROR\r\n");
#endif
          }			
          if(BIT0_SET == (ucRec_Data[DATA_BYTE_TWO] & BIT0_MASK))
          {StatusFlag.stStatusFlag.bErrorFlag = SET;
          Diag_Log.stDiag_Log_Flag.bDiag_Log_BLL = SET ;
#ifdef DBG_CAN_PARSER_STATUS      
          printf("BASE LIQUID LEVEL ERROR\r\n");
#endif
          }		
          if(BIT1_SET == (ucRec_Data[DATA_BYTE_TWO] & BIT1_MASK))
          {StatusFlag.stStatusFlag.bErrorFlag = SET;
          Diag_Log.stDiag_Log_Flag.bDiag_Log_HLL = SET ;
#ifdef DBG_CAN_PARSER_STATUS      
          printf("HARD LIQUID LEVEL ERROR\r\n");
#endif
          }		
          
          }
          if(BIT1_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT1_MASK))
          {
            ucDeviceDiagStatus |= (1 << HCM);
							/*change61*/
						 ucDeviceDiagRx |= (1 << HCM);
            Diag_Log.stDiag_Log_Flag.bDiag_Log_HCM = RESET ;
#ifdef DBG_CAN_PARSER_STATUS      
            printf("HCM DIAGNOSTIC PASS \r\n");
#endif
          }	
          
        }
        break;	
        /* Status message recieved from OPMM about Diagnostic result of peripherals connected to OPMM
        have to forward this message to TABLET also*/
      case STATUS_OPMM_DIAGNOSTIC:
        {
#ifdef DBG_CAN_PARSER_STATUS      
          printf("STATUS_OPMM_DIAGNOSTIC \r\n");
#endif	
          /******************FORWARD MESSAGE TO TABLET**********************/	
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          
          /****************************ABK*****************************/
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_HMI	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          
          fnCANMsg_Transmit(&stTx1Message, CAN1);
          /****************************ABK*****************************/
          /*change46*/
				//fault messages are sent without source address change
					//    pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
           //                     (DEST_TABLET	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          //						stWifi.ucSendFlag = SET;
          //						stEthernet.ucSendFlag = SET;
          fnXML_FIFO(&stTx1Message);
          /************************************************************/
          if(BIT0_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK))
          {StatusFlag.stStatusFlag.bErrorFlag = SET;
#ifdef DBG_CAN_PARSER_STATUS      
          printf("OPMM DIAGNOSTIC FAIL \r\n");
#endif
          ucDeviceDiagStatus &= ~(1 << OPMM);
							/*change61*/
						 ucDeviceDiagRx |= (1 << OPMM);
          Diag_Log.stDiag_Log_Flag.bDiag_Log_OPMM = SET ;
          if(BIT0_SET == (ucRec_Data[DATA_BYTE_ONE] & BIT0_MASK))
          {
            StatusFlag.stStatusFlag.bErrorFlag = SET;
#ifdef DBG_CAN_PARSER_STATUS      
            printf("SPRAY VALVE CONNECTION ERROR \r\n");
#endif
            Diag_Log.stDiag_Log_Flag.bDiag_Log_SGVC = SET ;
          }	
          if(BIT1_SET == (ucRec_Data[DATA_BYTE_ONE] & BIT1_MASK))
          {
            StatusFlag.stStatusFlag.bErrorFlag = SET;
#ifdef DBG_CAN_PARSER_STATUS      
            printf("GUN POSITION VALVE CONNECTION ERROR \r\n");
#endif
            Diag_Log.stDiag_Log_Flag.bDiag_Log_SHVC = SET ;
          }		
          if(BIT2_SET == (ucRec_Data[DATA_BYTE_ONE] & BIT2_MASK))
          {StatusFlag.stStatusFlag.bErrorFlag = SET;
          
#ifdef DBG_CAN_PARSER_STATUS      
          printf("AB VALVE CONNECTION ERROR \r\n");
#endif
          Diag_Log.stDiag_Log_Flag.bDiag_Log_ABVC = SET ;
          }	
          if(BIT3_SET == (ucRec_Data[DATA_BYTE_ONE] & BIT3_MASK))
          {
            StatusFlag.stStatusFlag.bErrorFlag = SET;
#ifdef DBG_CAN_PARSER_STATUS      
            printf("SOLVENT VALVE CONNECTION ERROR \r\n");
#endif
            Diag_Log.stDiag_Log_Flag.bDiag_Log_SFVC = SET ;
          }		
          if(BIT4_SET == (ucRec_Data[DATA_BYTE_ONE] & BIT4_MASK))
          {StatusFlag.stStatusFlag.bErrorFlag = SET;
          Diag_Log.stDiag_Log_Flag.bDiag_Log_MFVC = SET ;
#ifdef DBG_CAN_PARSER_STATUS      
          printf("FORWARD VALVE CONNECTION ERROR \r\n");
#endif
          }		
          if(BIT5_SET == (ucRec_Data[DATA_BYTE_ONE] & BIT5_MASK))
          {StatusFlag.stStatusFlag.bErrorFlag = SET;
          Diag_Log.stDiag_Log_Flag.bDiag_Log_MBVC = SET ;
#ifdef DBG_CAN_PARSER_STATUS      
          printf("BACKWARD VALVE CONNECTION ERROR \r\n");
#endif
          }		
          
          
          if(BIT0_SET == (ucRec_Data[DATA_BYTE_TWO] & BIT0_MASK))
          {StatusFlag.stStatusFlag.bErrorFlag = SET;
          Diag_Log.stDiag_Log_Flag.bDiag_Log_SGV = SET ;
#ifdef DBG_CAN_PARSER_STATUS      
          printf("SPRAY VALVE STATUS ERROR\r\n");
#endif
          }	
          if(BIT1_SET == (ucRec_Data[DATA_BYTE_TWO] & BIT1_MASK))
          {StatusFlag.stStatusFlag.bErrorFlag = SET;
          Diag_Log.stDiag_Log_Flag.bDiag_Log_SHV = SET ;
#ifdef DBG_CAN_PARSER_STATUS      
          printf("GUN POSITION VALVE STATUS ERROR\r\n");
#endif
          }			
          if(BIT2_SET == (ucRec_Data[DATA_BYTE_TWO] & BIT2_MASK))
          {StatusFlag.stStatusFlag.bErrorFlag = SET;
          Diag_Log.stDiag_Log_Flag.bDiag_Log_ABV = SET ;
#ifdef DBG_CAN_PARSER_STATUS      
          printf("AB VALVE STATUS ERROR\r\n");
#endif
          }		
          if(BIT3_SET == (ucRec_Data[DATA_BYTE_TWO] & BIT3_MASK))
          {StatusFlag.stStatusFlag.bErrorFlag = SET;
          Diag_Log.stDiag_Log_Flag.bDiag_Log_SFV = SET ;
#ifdef DBG_CAN_PARSER_STATUS      
          printf("SOLVENT VALVE STATUS ERROR\r\n");
#endif
          }		
          if(BIT4_SET == (ucRec_Data[DATA_BYTE_TWO] & BIT4_MASK))
          {StatusFlag.stStatusFlag.bErrorFlag = SET;
          Diag_Log.stDiag_Log_Flag.bDiag_Log_MFV = SET ;
#ifdef DBG_CAN_PARSER_STATUS      
          printf("FORWARD VALVE STATUS ERROR\r\n");
#endif
          }
          if(BIT5_SET == (ucRec_Data[DATA_BYTE_TWO] & BIT5_MASK))
          {StatusFlag.stStatusFlag.bErrorFlag = SET;
          Diag_Log.stDiag_Log_Flag.bDiag_Log_MBV = SET ;
#ifdef DBG_CAN_PARSER_STATUS      
          printf("BACKWARD VALVE STATUS ERROR\r\n");
#endif
          }
          
          
          if(BIT0_SET == (ucRec_Data[DATA_BYTE_THREE] & BIT0_MASK))
          {StatusFlag.stStatusFlag.bErrorFlag = SET;
          Diag_Log.stDiag_Log_Flag.bDiag_Log_PT12 = SET ;
#ifdef DBG_CAN_PARSER_STATUS      
          printf("PIPE TEMP SENSOR AT 12 ERROR\r\n");
#endif
          }
          if(BIT1_SET == (ucRec_Data[DATA_BYTE_THREE] & BIT1_MASK))
          {StatusFlag.stStatusFlag.bErrorFlag = SET;
          Diag_Log.stDiag_Log_Flag.bDiag_Log_PT3 = SET ;
#ifdef DBG_CAN_PARSER_STATUS      
          printf("PIPE TEMP SENSOR AT 3 ERROR\r\n");
#endif
          }	
          if(BIT2_SET == (ucRec_Data[DATA_BYTE_THREE] & BIT2_MASK))
          {StatusFlag.stStatusFlag.bErrorFlag = SET;
          Diag_Log.stDiag_Log_Flag.bDiag_Log_PT6 = SET ;
#ifdef DBG_CAN_PARSER_STATUS      
          printf("PIPE TEMP SENSOR AT 6 ERROR\r\n");
#endif
          }
          if(BIT3_SET == (ucRec_Data[DATA_BYTE_THREE] & BIT3_MASK))
          {StatusFlag.stStatusFlag.bErrorFlag = SET;
          Diag_Log.stDiag_Log_Flag.bDiag_Log_PT9 = SET ;
#ifdef DBG_CAN_PARSER_STATUS      
          printf("PIPE TEMP SENSOR AT 9 ERROR\r\n");
#endif
          }
          if(BIT4_SET == (ucRec_Data[DATA_BYTE_THREE] & BIT4_MASK))
          {StatusFlag.stStatusFlag.bErrorFlag = SET;
          Diag_Log.stDiag_Log_Flag.bDiag_Log_OBT = SET ;
#ifdef DBG_CAN_PARSER_STATUS      
          printf("OPMM BASE THERMOCOUPLE ERROR\r\n");
#endif
          }
          if(BIT5_SET == (ucRec_Data[DATA_BYTE_THREE] & BIT5_MASK))
          {StatusFlag.stStatusFlag.bErrorFlag = SET;
          Diag_Log.stDiag_Log_Flag.bDiag_Log_OHT = SET ;
#ifdef DBG_CAN_PARSER_STATUS      
          printf("OPMM HARD THERMOCOUPLE ERROR\r\n");
#endif
          }
          }
          if(BIT1_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT1_MASK))
          {
            ucDeviceDiagStatus |= (1 << OPMM);
						/*change61*/
						 ucDeviceDiagRx |= (1 << OPMM);
            Diag_Log.stDiag_Log_Flag.bDiag_Log_OPMM = RESET ;
#ifdef DBG_CAN_PARSER_STATUS      
            printf("OPMM DIAGNOSTIC PASS \r\n");
#endif
          }
          
        }
        break;	
        /* Status message recieved from HMI about Diagnostic result of peripherals connected to HMI
        have to forward this message to TABLET also*/
      case STATUS_HMI_DIAGNOSTIC:
        {
#ifdef DBG_CAN_PARSER_STATUS      
          printf("STATUS_HMI_DIAGNOSTIC \r\n");
#endif	
          
          /******************FORWARD MESSAGE TO TABLET**********************/	
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          
          /****************************ABK*****************************/
          /*change46*/
				//fault messages are sent without source address change
					//    pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
           //                     (DEST_TABLET	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          //						stWifi.ucSendFlag = SET;
          //						stEthernet.ucSendFlag = SET;
          fnXML_FIFO(&stTx1Message);
          /************************************************************/
          if(BIT0_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK))
          {			StatusFlag.stStatusFlag.bErrorFlag = SET;
#ifdef DBG_CAN_PARSER_STATUS      
          printf("HMI DIAGNOSTIC FAIL \r\n");
#endif
          ucDeviceDiagStatus &= ~(1 << HMI);
						/*change61*/
						 ucDeviceDiagRx |= (1 << HMI);
          Diag_Log.stDiag_Log_Flag.bDiag_Log_HMI = SET ;
          
          if(BIT0_SET == (ucRec_Data[DATA_BYTE_ONE] & BIT0_MASK))
          {				StatusFlag.stStatusFlag.bErrorFlag = SET;		
#ifdef DBG_CAN_PARSER_STATUS      
          printf("NVSRAM ERROR \r\n");
#endif
          Diag_Log.stDiag_Log_Flag.bDiag_Log_SRAM = SET ;
          
          }	
          if(BIT1_SET == (ucRec_Data[DATA_BYTE_ONE] & BIT1_MASK))
          {
            StatusFlag.stStatusFlag.bErrorFlag = SET;
#ifdef DBG_CAN_PARSER_STATUS      
            printf("PUSH TO ON SWITCH ERROR \r\n");
#endif
            Diag_Log.stDiag_Log_Flag.bDiag_Log_SW = SET ;
          }		
          }
          if(BIT1_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT1_MASK))
          {
#ifdef DBG_CAN_PARSER_STATUS      
            printf("HMI DIAGNOSTIC PASS \r\n");
#endif
            ucDeviceDiagStatus |= (1 << HMI);
						/*change61*/
						 ucDeviceDiagRx |= (1 << HMI);
            Diag_Log.stDiag_Log_Flag.bDiag_Log_HMI = RESET ;
          }
        }
        break;	
				
				case STATUS_AIR_CONNECTED:
        {
#ifdef DBG_CAN_PARSER_STATUS      
          printf("STATUS_AIR_CONNECTED \r\n");
#endif	
          /******************FORWARD MESSAGE TO TABLET**********************/	
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          
          /****************************ABK*****************************/
           /*change46*/
				//fault messages are sent without source address change
					//    pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
           //                     (DEST_TABLET	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          //						stWifi.ucSendFlag = SET;
          //						stEthernet.ucSendFlag = SET;
          fnXML_FIFO(&stTx1Message);
          /************************************************************/
          if(BIT0_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK))
          {
            
#ifdef DBG_CAN_PARSER_STATUS      
            printf("AIR NOT CONNECTED\r\n");
#endif
          }	
          else if(BIT1_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT1_MASK))
          {
            
#ifdef DBG_CAN_PARSER_STATUS      
            printf("AIR CONNECTED \r\n");
#endif
          }		
          
          
        }
        break;	
      
      case STATUS_OPMM_LIMITSW:
        {
#ifdef DBG_CAN_PARSER_STATUS      
          printf("STATUS OPMM LIMIT SW\r\n");
#endif
          /******************FORWARD MESSAGE TO TABLET**********************/	
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          
          /****************************ABK*****************************/
          /*change46*/
				//fault messages are sent without source address change
					//    pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
           //                     (DEST_TABLET	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId;
          //						stWifi.ucSendFlag = SET;
          //						stEthernet.ucSendFlag = SET;						
          fnXML_FIFO(&stTx1Message);
          /************************************************************/
          /******************************************************/	
          if(BIT0_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK))
          {
#ifdef DBG_CAN_PARSER_STATUS      
            printf("GUN POSITION AWAY FROM PIPE LIMIT SWITCH 1 PRESSED\r\n");
#endif
          }
          /******************************************************/	
          
          if(BIT1_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT1_MASK))
          {
#ifdef DBG_CAN_PARSER_STATUS      
            printf("GUN POSITION TOWARDS PIPE LIMIT SWITCH 2 PRESSED\r\n");
#endif
          }
          
          /******************************************************/	
          if(BIT2_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT2_MASK))
          {
#ifdef DBG_CAN_PARSER_STATUS      
            printf("DCLS LIMIT SWITCH 3 PRESSED\r\n");
#endif
            
          }
          
          /******************************************************/	
          if(BIT3_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT3_MASK))
          {
#ifdef DBG_CAN_PARSER_STATUS      
            printf("HLS LIMIT SWITCH 4 PRESSED\r\n");
#endif
          }
          
          /******************************************************/	
          
        }
        break;	
        /**************************************Change14***************************/	
        
      case STATUS_FIRMWARE_VERSION_NUMBER:
        {
#ifdef DBG_CAN_PARSER_STATUS      
          printf("STATUS_FIRMWARE_VERSION_NUMBER \r\n");
#endif	
          /******************FORWARD MESSAGE TO TABLET & HMI **********************/	
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          /****************************ABK*****************************/
          /*change46*/
				//fault messages are sent without source address change
					//    pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
           //                     (DEST_TABLET	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          //						stWifi.ucSendFlag = SET;
          //						stEthernet.ucSendFlag = SET;
          fnXML_FIFO(&stTx1Message);
          /************************************************************/
          for(ucLoopCnt=1; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
          {
            SplitByte.rgucData[ucLoopCnt-1] = pstCANrxmsg->Data[ucLoopCnt];
          }
          
          if(ID_HMI == (ucRec_Data[DATA_BYTE_ZERO]))
          {
            
            stCurrentData.fCurr_HMI_Firmware_Ver = SplitByte.fData;
            
#ifdef DBG_CAN_PARSER_STATUS
            
            printf("fCurr_HMI_Firmware_Ver %0.2f",
                   stCurrentData.fCurr_HMI_Firmware_Ver);
            
#endif
          }
          if(ID_OPMM == (ucRec_Data[DATA_BYTE_ZERO]))
          {
            
            stCurrentData.fCurr_OPMM_Firmware_Ver = SplitByte.fData;
            /******************FORWARD MESSAGE TO TABLET & HMI **********************/	
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_HMI	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            fnCANMsg_Transmit(&stTx1Message, CAN1);			
            
#ifdef DBG_CAN_PARSER_STATUS
            
            printf("fCurr_OPMM_Firmware_Ver %0.2f",
                   stCurrentData.fCurr_OPMM_Firmware_Ver);
            
#endif
          }
          if(ID_HCM == (ucRec_Data[DATA_BYTE_ZERO]))
          {
            
            stCurrentData.fCurr_HCM_Firmware_Ver = SplitByte.fData;
#ifdef DBG_CAN_PARSER_STATUS
            
            printf("fCurr_HCM_Firmware_Ver %0.2f",
                   stCurrentData.fCurr_HCM_Firmware_Ver);
            
#endif
          }
          if(ID_VCM == (ucRec_Data[DATA_BYTE_ZERO]))
          {
            
            stCurrentData.fCurr_VCM_Firmware_Ver = SplitByte.fData;
           	#ifdef DBG_CAN_PARSER_STATUS
            
            printf("fCurr_VCM_Firmware_Ver %0.2f",
                   stCurrentData.fCurr_VCM_Firmware_Ver);
            
            	#endif
          }
        }
        break;
      }
    }
    break;
    /************DATA MESSAGE*************/	
  case MSG_TYPE_DATA:
    {
#ifdef DBG_CAN_PARSER_DATA      
      printf("MSG_TYPE_DATA \r\n");
#endif	
      switch (uliCanID & MSG_MASK)
      {
        /* Acknowledge recieved for Data message */
      case ACKNOWLEDGEMENT:
        {
          fnCANPost_Ack(pstCANrxmsg,CAN1);
#ifdef DBG_CAN_PARSER_DATA      
          printf("ACKNOWLEDGE FOR DATA MESSAGE..\r\n");
#endif	
        }
        break;
        /* DATA message recieved from HCM For inline base temperature
        have to forward this message to TABLET also*/
      case DATA_INLINE_BASE_TEMP:
        {
#ifdef DBG_CAN_PARSER_DATA      
          printf("DATA_INLINE_BASE_TEMP \r\n");
#endif	
          /******************FORWARD MESSAGE TO TABLET**********************/	
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          
          /****************************ABK*****************************/
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_TABLET	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          //						stWifi.ucSendFlag = SET;
          //						stEthernet.ucSendFlag = SET;
          fnXML_FIFO(&stTx1Message);
          /************************************************************/
          for(ucLoopCnt=0; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
          {
            SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          
          stCurrentData.fCurr_InlineBaseMatTemp = SplitByte.fData;
#ifdef DBG_CAN_PARSER_DATA
          printf("DATA_INLINE_BASE_TEMP %0.2f",
                 stCurrentData.fCurr_InlineBaseMatTemp);
          
#endif
          /*********************FOR STATUS LOG ********************/
          rgucStatusLog[BYTE_SEVENTEEN] 			= SplitByte.rgucData[BYTE_ZERO] ;
          rgucStatusLog[BYTE_EIGHTEEN] 			= SplitByte.rgucData[BYTE_ONE] ;
          rgucStatusLog[BYTE_NINETEEN] 			= SplitByte.rgucData[BYTE_TWO] ;
          rgucStatusLog[BYTE_TWENTY] 		= SplitByte.rgucData[BYTE_THREE] ;
          
          /*********************FOR PERIODIC LOG*******************/
          rgucPeriodicLog[BYTE_EIGHTEEN] 		= SplitByte.rgucData[BYTE_ZERO] ;
          rgucPeriodicLog[BYTE_NINETEEN] 	= SplitByte.rgucData[BYTE_ONE] ;
          rgucPeriodicLog[BYTE_TWENTY] 		= SplitByte.rgucData[BYTE_TWO] ;
          rgucPeriodicLog[BYTE_TWENTYONE] 		= SplitByte.rgucData[BYTE_THREE] ;
        }
        break;
        /* DATA message recieved from HCM For inline hard temperature
        have to forward this message to TABLET also*/
      case DATA_INLINE_HARD_TEMP:
        {
#ifdef DBG_CAN_PARSER_DATA      
          printf("DATA_INLINE_HARD_TEMP \r\n");
#endif	
          /******************FORWARD MESSAGE TO TABLET**********************/	
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          
          /****************************ABK*****************************/
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_TABLET	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          ////						stWifi.ucSendFlag = SET;
          ////						stEthernet.ucSendFlag = SET;
          fnXML_FIFO(&stTx1Message);
          /************************************************************/
          for(ucLoopCnt=0; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
          {
            SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          
          stCurrentData.fCurr_InlineHardMatTemp = SplitByte.fData;
#ifdef DBG_CAN_PARSER_DATA
          
          printf("DATA_INLINE_HARD_TEMP %0.2f",
                 stCurrentData.fCurr_InlineHardMatTemp);
          
#endif
          /*********************FOR STATUS LOG ********************/
          rgucStatusLog[BYTE_TWENTYONE] 			= SplitByte.rgucData[BYTE_ZERO] ;
          rgucStatusLog[BYTE_TWENTYTWO] 			= SplitByte.rgucData[BYTE_ONE] ;
          rgucStatusLog[BYTE_TWENTYTHREE] 				= SplitByte.rgucData[BYTE_TWO] ;
          rgucStatusLog[BYTE_TWENTYFOUR] 		= SplitByte.rgucData[BYTE_THREE] ;
          
          /*********************FOR PERIODIC LOG*******************/
          rgucPeriodicLog[BYTE_TWENTYTWO] 			= SplitByte.rgucData[BYTE_ZERO] ;
          rgucPeriodicLog[BYTE_TWENTYTHREE] 	= SplitByte.rgucData[BYTE_ONE] ;
          rgucPeriodicLog[BYTE_TWENTYFOUR] 	= SplitByte.rgucData[BYTE_TWO] ;
          rgucPeriodicLog[BYTE_TWENTYFIVE] = SplitByte.rgucData[BYTE_THREE] ;
          
          
        }
        break;
        /* DATA message recieved from HCM For Base retun line temperature
        have to forward this message to TABLET also*/
      case DATA_RETURN_BASE_TEMP:
        {
#ifdef DBG_CAN_PARSER_DATA      
          printf("DATA_RETURN_BASE_TEMP \r\n");
#endif	
          /******************FORWARD MESSAGE TO TABLET**********************/	
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          
          /****************************ABK*****************************/
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_TABLET	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          //						stWifi.ucSendFlag = SET;
          //						stEthernet.ucSendFlag = SET;
          fnXML_FIFO(&stTx1Message);
          /************************************************************/
          for(ucLoopCnt=0; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
          {
            SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          
          stCurrentData.fCurr_ReturnLineTemp = SplitByte.fData;
#ifdef DBG_CAN_PARSER_DATA
          
          printf("DATA_RETURN_BASE_TEMP %0.2f",
                 stCurrentData.fCurr_ReturnLineTemp);
          
#endif
          /*********************FOR PERIODIC LOG*******************/
          rgucPeriodicLog[BYTE_FOURTYTWO] 			= SplitByte.rgucData[BYTE_ZERO] ;
          rgucPeriodicLog[BYTE_FOURTYTHREE] 	= SplitByte.rgucData[BYTE_ONE] ;
          rgucPeriodicLog[BYTE_FOURTYFOUR] 	= SplitByte.rgucData[BYTE_TWO] ;
          rgucPeriodicLog[BYTE_FOURTYFIVE] = SplitByte.rgucData[BYTE_THREE] ;
          
        }
        break;
        /* DATA message recieved from HCM For Antifreeze temperature
        have to forward this message to TABLET also*/
      case DATA_ANTIFREEZ_TEMP:
        {
#ifdef DBG_CAN_PARSER_DATA      
          printf("DATA_ANTIFREEZ_TEMP \r\n");
#endif	
          /******************FORWARD MESSAGE TO TABLET**********************/	
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          
          /****************************ABK*****************************/
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_TABLET	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          //						stWifi.ucSendFlag = SET;
          //						stEthernet.ucSendFlag = SET;
          fnXML_FIFO(&stTx1Message);
          /************************************************************/
          for(ucLoopCnt=0; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
          {
            SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          
          stCurrentData.fCurr_AntifreezeLiquidTemp = SplitByte.fData;
#ifdef DBG_CAN_PARSER_DATA
          
          printf("DATA_ANTIFREEZ_TEMP %0.2f",
                 stCurrentData.fCurr_AntifreezeLiquidTemp);
          
#endif
          /*********************FOR STATUS LOG ********************/
          rgucStatusLog[BYTE_THIRTEEN] 			= SplitByte.rgucData[BYTE_ZERO] ;
          rgucStatusLog[BYTE_FOURTEEN] 		= SplitByte.rgucData[BYTE_ONE] ;
          rgucStatusLog[BYTE_FIFTEEN] 		= SplitByte.rgucData[BYTE_TWO] ;
          rgucStatusLog[BYTE_SIXTEEN] 	= SplitByte.rgucData[BYTE_THREE] ;
          
          /*********************FOR PERIODIC LOG*******************/
          rgucPeriodicLog[BYTE_FOURTEEN] 			= SplitByte.rgucData[BYTE_ZERO] ;
          rgucPeriodicLog[BYTE_FIFTEEN] 	= SplitByte.rgucData[BYTE_ONE] ;
          rgucPeriodicLog[BYTE_SIXTEEN] 	= SplitByte.rgucData[BYTE_TWO] ;
          rgucPeriodicLog[BYTE_SEVENTEEN] = SplitByte.rgucData[BYTE_THREE] ;
          
        }
        break;
        /* DATA message recieved from HCM For Base material level
        have to forward this message to TABLET also*/
      case DATA_BASE_MATERIAL_LVL:
        {
#ifdef DBG_CAN_PARSER_DATA      
          printf("DATA_BASE_MATERIAL_LVL \r\n");
#endif	
          /******************FORWARD MESSAGE TO TABLET**********************/	
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          
          /****************************ABK*****************************/
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_TABLET	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          //						stWifi.ucSendFlag = SET;
          //						stEthernet.ucSendFlag = SET;
          fnXML_FIFO(&stTx1Message);
          /************************************************************/
          for(ucLoopCnt=0; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
          {
            SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          
          stCurrentData.fCurr_LiquidLevelBase = SplitByte.fData;
#ifdef DBG_CAN_PARSER_DATA
          
          printf("DATA_BASE_MATERIAL_LVL %0.2f",
                 stCurrentData.fCurr_LiquidLevelBase);
          
#endif
          /*********************FOR PERIODIC LOG*******************/
          rgucPeriodicLog[BYTE_FIFTYFOUR] 		= SplitByte.rgucData[BYTE_ZERO] ;
          rgucPeriodicLog[BYTE_FIFTYFIVE] 		= SplitByte.rgucData[BYTE_ONE] ;
          rgucPeriodicLog[BYTE_FIFTYSIX] 		= SplitByte.rgucData[BYTE_TWO] ;
          rgucPeriodicLog[BYTE_FIFTYSEVEN] 	= SplitByte.rgucData[BYTE_THREE] ;
          
        }
        break;
        /* DATA message recieved from HCM For Hard material level
        have to forward this message to TABLET also*/
      case DATA_HARD_MATERIAL_LVL:
        {
#ifdef DBG_CAN_PARSER_DATA      
          printf("DATA_HARD_MATERIAL_LVL \r\n");
#endif	
          /******************FORWARD MESSAGE TO TABLET**********************/	
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          
          /****************************ABK*****************************/
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_TABLET	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          //						stWifi.ucSendFlag = SET;
          //						stEthernet.ucSendFlag = SET;
          fnXML_FIFO(&stTx1Message);
          /************************************************************/
          for(ucLoopCnt=0; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
          {
            SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          
          stCurrentData.fCurr_LiquidLevelHard = SplitByte.fData;
#ifdef DBG_CAN_PARSER_DATA
          
          printf("DATA_HARD_MATERIAL_LVL %0.2f",
                 stCurrentData.fCurr_LiquidLevelHard);
          
#endif
          /*********************FOR PERIODIC LOG*******************/
          rgucPeriodicLog[BYTE_FIFTYEIGHT] 		= SplitByte.rgucData[BYTE_ZERO] ;
          rgucPeriodicLog[BYTE_FIFTYNINE] 	= SplitByte.rgucData[BYTE_ONE] ;
          rgucPeriodicLog[BYTE_SIXTY] 	= SplitByte.rgucData[BYTE_TWO] ;
          rgucPeriodicLog[BYTE_SIXTYONE] 	= SplitByte.rgucData[BYTE_THREE] ;
          
        }
        break;
        /* DATA message recieved from VCM For current Dew point 
        have to forward this message to TABLET also*/
      case DATA_DEW_POINT:
        {
#ifdef DBG_CAN_PARSER_DATA      
          printf("DATA_DEW_POINT \r\n");
#endif	
          /******************FORWARD MESSAGE TO TABLET**********************/	
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          
          /****************************ABK*****************************/
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_TABLET	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          //						stWifi.ucSendFlag = SET;
          //						stEthernet.ucSendFlag = SET;
          fnXML_FIFO(&stTx1Message);
          /************************************************************/
          for(ucLoopCnt=0; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
          {
            SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          
          stCurrentData.fCurr_DewPoint = SplitByte.fData;
#ifdef DBG_CAN_PARSER_DATA
          
          printf("DATA_DEW_POINT %0.2f",
                 stCurrentData.fCurr_DewPoint);
          
#endif
          /*********************FOR STATUS LOG ********************/
          rgucStatusLog[BYTE_FOURTYFIVE] 	= SplitByte.rgucData[BYTE_ZERO] ;
          rgucStatusLog[BYTE_FOURTYSIX] = SplitByte.rgucData[BYTE_ONE] ;
          rgucStatusLog[BYTE_FOURTYSEVEN] 	= SplitByte.rgucData[BYTE_TWO] ;
          rgucStatusLog[BYTE_FOURTYEIGHT] 	= SplitByte.rgucData[BYTE_THREE] ;
          
          /*********************FOR PERIODIC LOG*******************/
          rgucPeriodicLog[BYTE_FIFTY] 	= SplitByte.rgucData[BYTE_ZERO] ;
          rgucPeriodicLog[BYTE_FIFTYONE] 		= SplitByte.rgucData[BYTE_ONE] ;
          rgucPeriodicLog[BYTE_FIFTYTWO] 				= SplitByte.rgucData[BYTE_TWO] ;
          rgucPeriodicLog[BYTE_FIFTYTHREE] 			= SplitByte.rgucData[BYTE_THREE] ;
          
        }
        break;
        /* DATA message recieved from VCM For current Ambient Temperature inside container 
        have to forward this message to TABLET also*/
      case DATA_AMB_TEMP_INSIDE:
        {
#ifdef DBG_CAN_PARSER_DATA      
          printf("DATA_AMB_TEMP_INSIDE \r\n");
#endif	
          /******************FORWARD MESSAGE TO TABLET**********************/	
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          
          /****************************ABK*****************************/
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_TABLET	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          //						stWifi.ucSendFlag = SET;
          //						stEthernet.ucSendFlag = SET;
          fnXML_FIFO(&stTx1Message);
          /************************************************************/
          for(ucLoopCnt=0; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
          {
            SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          
          stCurrentData.fCurr_InsideAmbientTemp = SplitByte.fData;
#ifdef DBG_CAN_PARSER_DATA
          
          printf("DATA_AMB_TEMP_INSIDE %0.2f",
                 stCurrentData.fCurr_InsideAmbientTemp);
          
#endif
          
          /*********************FOR STATUS LOG ********************/
          rgucStatusLog[BYTE_THIRTYTHREE] 					= SplitByte.rgucData[BYTE_ZERO] ;
          rgucStatusLog[BYTE_THIRTYFOUR] 			= SplitByte.rgucData[BYTE_ONE] ;
          rgucStatusLog[BYTE_THIRTYFIVE] 			= SplitByte.rgucData[BYTE_TWO] ;
          rgucStatusLog[BYTE_THIRTYSIX] 		= SplitByte.rgucData[BYTE_THREE] ;
          
          /*********************FOR PERIODIC LOG*******************/
          rgucPeriodicLog[BYTE_THIRTYFOUR] 		= SplitByte.rgucData[BYTE_ZERO] ;
          rgucPeriodicLog[BYTE_THIRTYFIVE] 	= SplitByte.rgucData[BYTE_ONE] ;
          rgucPeriodicLog[BYTE_THIRTYSIX] 		= SplitByte.rgucData[BYTE_TWO] ;
          rgucPeriodicLog[BYTE_THIRTYSEVEN] 		= SplitByte.rgucData[BYTE_THREE] ;
          
        }
        break;
        /* DATA message recieved from VCM For current Ambient Temperature outside container 
        have to forward this message to TABLET also*/
      case DATA_AMB_TEMP_OUTSIDE:
        {
#ifdef DBG_CAN_PARSER_DATA      
          printf("DATA_AMB_TEMP_OUTSIDE \r\n");
#endif	
          /******************FORWARD MESSAGE TO TABLET**********************/	
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          
          /****************************ABK*****************************/
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_TABLET	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          //						stWifi.ucSendFlag = SET;
          //						stEthernet.ucSendFlag = SET;
          fnXML_FIFO(&stTx1Message);
          /************************************************************/
          for(ucLoopCnt=0; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
          {
            SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          
          stCurrentData.fCurr_OutsideAmbientTemp = SplitByte.fData;
#ifdef DBG_CAN_PARSER_DATA
          
          printf("DATA_AMB_TEMP_OUTSIDE %0.2f",
                 stCurrentData.fCurr_OutsideAmbientTemp);
          
#endif
          /*********************FOR STATUS LOG ********************/
          rgucStatusLog[BYTE_THIRTYSEVEN] 			= SplitByte.rgucData[BYTE_ZERO] ;
          rgucStatusLog[BYTE_THIRTYEIGHT] 			= SplitByte.rgucData[BYTE_ONE] ;
          rgucStatusLog[BYTE_THIRTYNINE] 			= SplitByte.rgucData[BYTE_TWO] ;
          rgucStatusLog[BYTE_FOURTY] 		= SplitByte.rgucData[BYTE_THREE] ;
          
          /*********************FOR PERIODIC LOG*******************/
          rgucPeriodicLog[BYTE_THIRTYEIGHT] 		= SplitByte.rgucData[BYTE_ZERO] ;
          rgucPeriodicLog[BYTE_THIRTYNINE] 	= SplitByte.rgucData[BYTE_ONE] ;
          rgucPeriodicLog[BYTE_FOURTY] 	= SplitByte.rgucData[BYTE_TWO] ;
          rgucPeriodicLog[BYTE_FOURTYONE] 		= SplitByte.rgucData[BYTE_THREE] ;
          
        }
        break;
        /* DATA message recieved from VCM For current Ambient Humidity 
        have to forward this message to TABLET also*/
      case DATA_MCU_AMB_HUMIDITY:
        {
#ifdef DBG_CAN_PARSER_DATA      
          printf("DATA_MCU_AMB_HUMIDITY \r\n");
#endif	
          /******************FORWARD MESSAGE TO TABLET**********************/	
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          
          /****************************ABK*****************************/
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_TABLET	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          //						stWifi.ucSendFlag = SET;
          //						stEthernet.ucSendFlag = SET;
          fnXML_FIFO(&stTx1Message);
          /************************************************************/
          for(ucLoopCnt=0; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
          {
            SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          
          stCurrentData.fCurr_OutsideAmbientHumidity = SplitByte.fData;
#ifdef DBG_CAN_PARSER_DATA
          
          printf("DATA_MCU_AMB_HUMIDITY %0.2f",
                 stCurrentData.fCurr_OutsideAmbientHumidity);
          
#endif
          /*********************FOR STATUS LOG ********************/
          rgucStatusLog[BYTE_FOURTYONE] 	= SplitByte.rgucData[BYTE_ZERO] ;
          rgucStatusLog[BYTE_FOURTYTWO] 		= SplitByte.rgucData[BYTE_ONE] ;
          rgucStatusLog[BYTE_FOURTYTHREE] 				= SplitByte.rgucData[BYTE_TWO] ;
          rgucStatusLog[BYTE_FOURTYFOUR] 		= SplitByte.rgucData[BYTE_THREE] ;
          
          /*********************FOR PERIODIC LOG*******************/
          rgucPeriodicLog[BYTE_FOURTYSIX] 	= SplitByte.rgucData[BYTE_ZERO] ;
          rgucPeriodicLog[BYTE_FOURTYSEVEN] = SplitByte.rgucData[BYTE_ONE] ;
          rgucPeriodicLog[BYTE_FOURTYEIGHT] 	= SplitByte.rgucData[BYTE_TWO] ;
          rgucPeriodicLog[BYTE_FOURTYNINE] 	= SplitByte.rgucData[BYTE_THREE] ;
          
        }
        break;
        /* DATA message recieved from VCM For Material used 
        have to forward this message to TABLET also*/
      case DATA_MATERIAL_USED:
        {
#ifdef DBG_CAN_PARSER_DATA      
          printf("DATA_MATERIAL_USED \r\n");
#endif	
          //					/******************FORWARD MESSAGE TO TABLET**********************/	
          //						stTx1Message.DLC = pstCANrxmsg->DLC; 
          //						for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          //						{
          //							stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          //						}
          //										
          //						/****************************ABK*****************************/
          //						pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
          //																	(DEST_TABLET	 | SOURCE_SCU));
          //						stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          ////						stWifi.ucSendFlag = SET;
          ////						stEthernet.ucSendFlag = SET;
          //						fnXML_FIFO(&stTx1Message);
          /************************************************************/
          /************************LEGA***************************/
          if(BIT0_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK))
          {
            for(ucLoopCnt=1; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              SplitByte.rgucData[ucLoopCnt-1] = pstCANrxmsg->Data[ucLoopCnt];
            }
            ucMaterialDataReceived |= LEG_A_DATA;
            stCurrentData.fCurr_LegAMaterialUsed = SplitByte.fData;
#ifdef DBG_CAN_PARSER_DATA
            
            printf("LEGA DATA_MATERIAL_USED %0.2f",
                   stCurrentData.fCurr_LegAMaterialUsed);
            
#endif
          }	
          /************************LEGB***************************/
          if(BIT1_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT1_MASK))
          {
            for(ucLoopCnt=1; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              SplitByte.rgucData[ucLoopCnt-1] = pstCANrxmsg->Data[ucLoopCnt];
            }
            ucMaterialDataReceived |= LEG_B_DATA;
            stCurrentData.fCurr_LegBMaterialUsed = SplitByte.fData;
#ifdef DBG_CAN_PARSER_DATA
            
            printf("LEGB DATA_MATERIAL_USED %0.2f",
                   stCurrentData.fCurr_LegBMaterialUsed);
            
#endif
          }	
          /************************LEGC***************************/
          if(BIT2_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT2_MASK))
          {
            for(ucLoopCnt=1; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              SplitByte.rgucData[ucLoopCnt-1] = pstCANrxmsg->Data[ucLoopCnt];
            }
            ucMaterialDataReceived |= LEG_C_DATA;
            stCurrentData.fCurr_LegCMaterialUsed = SplitByte.fData;
#ifdef DBG_CAN_PARSER_DATA
            
            printf("LEGC DATA_MATERIAL_USED %0.2f",
                   stCurrentData.fCurr_LegCMaterialUsed);
            
#endif
          }	
          if ( MATERIAL_DATA_RECEIVED == ucMaterialDataReceived )
          {
            ucMaterialDataReceived = 0;
            if ( PRIMEPROCESS == ucPreviousProcess )
            {
              /*Base material use for priming for current joint*/
              stCurrentData.fCurr_BaseMaterialPrimed = (stCurrentData.fCurr_LegAMaterialUsed + stCurrentData.fCurr_LegCMaterialUsed);
						//		 printf("\n\rstCurrentData.fCurr_BaseMaterialPrimed = %0.2f\r\n",  stCurrentData.fCurr_BaseMaterialPrimed);
							/*change36*/
              /*Total Base material use for priming from power on of MCL system*/
              /*Send value to HMI & TAB*/
              stCurrentData.fCurr_BaseMaterialPrimedAccumulate += stCurrentData.fCurr_BaseMaterialPrimed;
              
							/***********/	
              /**************************SEND CAN MESSAGE*******************/
              stTx1Message.DLC = FIVE_BYTES;
              SplitByte.fData = stCurrentData.fCurr_BaseMaterialPrimedAccumulate;
              stTx1Message.Data[DATA_BYTE_ZERO] = BIT2_SET ;
              /**************************DATA***********************************/
              for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
              {
                stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
              }
              /****************************************************************/
              stTx1Message.ExtId = MSG_TYPE_DATA | DATA_ACCUMULATED_MATERIAL_USED |
                DEST_HMI | SOURCE_SCU;
              fnCANMsg_Transmit(&stTx1Message, CAN1);
							
						
#ifdef DBG_CAN_PARSER_DATA
              printf("\n\DATA_ACCUMULATED_MATERIAL_USED = %x\r\n", stTx1Message.ExtId);
#endif
              stTx1Message.ExtId = MSG_TYPE_DATA | DATA_ACCUMULATED_MATERIAL_USED |
                DEST_TABLET | SOURCE_SCU;	
              //	stWifi.ucSendFlag = SET;
              //	stEthernet.ucSendFlag = SET;
              fnXML_FIFO(&stTx1Message);
              /************************************************************/	
              /*Hard material use for priming for current joint*/
              stCurrentData.fCurr_HardMaterialPrimed = stCurrentData.fCurr_LegBMaterialUsed;
							//	 printf("\n\rstCurrentData.fCurr_HardMaterialPrimed = %0.2f\r\n",  stCurrentData.fCurr_HardMaterialPrimed);
              /*change36*/
              /*Total Hard material use for priming from power on of MCL system*/
              /*Send value to HMI & TAB*/
              stCurrentData.fCurr_HardMaterialPrimedAccumulate += stCurrentData.fCurr_HardMaterialPrimed;
             
						 /***********/	
              /**************************SEND CAN MESSAGE*******************/
              stTx1Message.DLC = FIVE_BYTES;
              SplitByte.fData = stCurrentData.fCurr_HardMaterialPrimedAccumulate;
              stTx1Message.Data[DATA_BYTE_ZERO] = BIT3_SET ;
              /**************************DATA***********************************/
              for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
              {
                stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
              }
              /****************************************************************/
              stTx1Message.ExtId = MSG_TYPE_DATA | DATA_ACCUMULATED_MATERIAL_USED |
                DEST_HMI | SOURCE_SCU;
              fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CAN_PARSER_DATA
              printf("\n\DATA_ACCUMULATED_MATERIAL_USED = %x\r\n", stTx1Message.ExtId);
#endif
              stTx1Message.ExtId = MSG_TYPE_DATA | DATA_ACCUMULATED_MATERIAL_USED |
                DEST_TABLET | SOURCE_SCU;	
              //	stWifi.ucSendFlag = SET;
              //	stEthernet.ucSendFlag = SET;
              fnXML_FIFO(&stTx1Message);
              /************************************************************/	
              /*change68*/
							  stCurrentData.fCurr_BaseTotalMaterialUsedAccumulate += stCurrentData.fCurr_BaseMaterialPrimed;
              /***********/
              /**************************SEND CAN MESSAGE*******************/
              stTx1Message.DLC = FIVE_BYTES;
              SplitByte.fData = stCurrentData.fCurr_BaseTotalMaterialUsedAccumulate;
              stTx1Message.Data[DATA_BYTE_ZERO] = BIT6_SET ;
              /**************************DATA***********************************/
              for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
              {
                stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
              }
              /****************************************************************/
              stTx1Message.ExtId = MSG_TYPE_DATA | DATA_ACCUMULATED_MATERIAL_USED |
                DEST_HMI | SOURCE_SCU;
              fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CAN_PARSER_DATA
              printf("\n\DATA_ACCUMULATED_MATERIAL_USED = %x\r\n", stTx1Message.ExtId);
#endif
              stTx1Message.ExtId = MSG_TYPE_DATA | DATA_ACCUMULATED_MATERIAL_USED |
                DEST_TABLET | SOURCE_SCU;	
              //	stWifi.ucSendFlag = SET;
              //	stEthernet.ucSendFlag = SET;
              fnXML_FIFO(&stTx1Message);
              /************************************************************/		
              /*Total Hard material used for priming and spraying of current joint*/
            //  stCurrentData.fCurr_HardTotalMaterialUsed = stCurrentData.fCurr_HardMaterialApplied + stCurrentData.fCurr_HardMaterialPrimed;
              /*change36*/
              /*Total Hard material use for priming & Spraying from power on of MCL system*/
              /*Send value to HMI & TAB*/
              stCurrentData.fCurr_HardTotalMaterialUsedAccumulate += stCurrentData.fCurr_HardMaterialPrimed;
              /***********/	
              /**************************SEND CAN MESSAGE*******************/
              stTx1Message.DLC = FIVE_BYTES;
              SplitByte.fData = stCurrentData.fCurr_HardTotalMaterialUsedAccumulate;
              stTx1Message.Data[DATA_BYTE_ZERO] = BIT7_SET ;
              /**************************DATA***********************************/
              for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
              {
                stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
              }
              /****************************************************************/
              stTx1Message.ExtId = MSG_TYPE_DATA | DATA_ACCUMULATED_MATERIAL_USED |
                DEST_HMI | SOURCE_SCU;
              fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CAN_PARSER_DATA
              printf("\n\DATA_ACCUMULATED_MATERIAL_USED = %x\r\n", stTx1Message.ExtId);
#endif
              stTx1Message.ExtId = MSG_TYPE_DATA | DATA_ACCUMULATED_MATERIAL_USED |
                DEST_TABLET | SOURCE_SCU;	
              //	stWifi.ucSendFlag = SET;
              //	stEthernet.ucSendFlag = SET;
              fnXML_FIFO(&stTx1Message);
              /************************************************************/		

              
            }
            
            if ( SPRAYPROCESS == ucPreviousProcess )
            {
              /*Base material use for Spraying for current joint*/
              stCurrentData.fCurr_BaseMaterialApplied = (stCurrentData.fCurr_LegAMaterialUsed + stCurrentData.fCurr_LegCMaterialUsed);
//								 printf("\n\rstCurrentData.fCurr_BaseMaterialApplied = %0.2f\r\n",  stCurrentData.fCurr_BaseMaterialApplied);
              /*change36*/
              /*Total Base material use for Spraying from power on of MCL system*/
              /*Send value to HMI & TAB*/
              stCurrentData.fCurr_BaseMaterialAppliedAccumulate += stCurrentData.fCurr_BaseMaterialApplied;
              /***********/	
              /**************************SEND CAN MESSAGE*******************/
              stTx1Message.DLC = FIVE_BYTES;
              SplitByte.fData = stCurrentData.fCurr_BaseMaterialAppliedAccumulate;
              stTx1Message.Data[DATA_BYTE_ZERO] = BIT4_SET ;
              /**************************DATA***********************************/
              for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
              {
                stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
              }
              /****************************************************************/
              stTx1Message.ExtId = MSG_TYPE_DATA | DATA_ACCUMULATED_MATERIAL_USED |
                DEST_HMI | SOURCE_SCU;
              fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CAN_PARSER_DATA
              printf("\n\DATA_ACCUMULATED_MATERIAL_USED = %x\r\n", stTx1Message.ExtId);
#endif
              stTx1Message.ExtId = MSG_TYPE_DATA | DATA_ACCUMULATED_MATERIAL_USED |
                DEST_TABLET | SOURCE_SCU;	
              //	stWifi.ucSendFlag = SET;
              //	stEthernet.ucSendFlag = SET;
              fnXML_FIFO(&stTx1Message);
              /************************************************************/	
              /*Hard material use for Spraying for current joint*/
              stCurrentData.fCurr_HardMaterialApplied = stCurrentData.fCurr_LegBMaterialUsed;
//							 printf("\n\rstCurrentData.fCurr_HardMaterialApplied = %0.2f\r\n",  stCurrentData.fCurr_HardMaterialApplied);
              /*change36*/
              /*Total Hard material use for Spraying from power on of MCL system*/
              /*Send value to HMI & TAB*/
              stCurrentData.fCurr_HardMaterialAppliedAccumulate += stCurrentData.fCurr_HardMaterialApplied;
              /***********/	
              /**************************SEND CAN MESSAGE*******************/
              stTx1Message.DLC = FIVE_BYTES;
              SplitByte.fData = stCurrentData.fCurr_HardMaterialAppliedAccumulate;
              stTx1Message.Data[DATA_BYTE_ZERO] = BIT5_SET ;
              /**************************DATA***********************************/
              for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
              {
                stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
              }
              /****************************************************************/
              stTx1Message.ExtId = MSG_TYPE_DATA | DATA_ACCUMULATED_MATERIAL_USED |
                DEST_HMI | SOURCE_SCU;
              fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CAN_PARSER_DATA
              printf("\n\DATA_ACCUMULATED_MATERIAL_USED = %x\r\n", stTx1Message.ExtId);
#endif
              stTx1Message.ExtId = MSG_TYPE_DATA | DATA_ACCUMULATED_MATERIAL_USED |
                DEST_TABLET | SOURCE_SCU;	
              //	stWifi.ucSendFlag = SET;
              //	stEthernet.ucSendFlag = SET;
              fnXML_FIFO(&stTx1Message);
              /************************************************************/	
              
              /*Total Base material used for priming and spraying of current joint*/
							/*change68*/
             stCurrentData.fCurr_BaseTotalMaterialUsed = stCurrentData.fCurr_BaseMaterialApplied + stCurrentData.fCurr_BaseMaterialPrimed;
              /*change36*/
              /*Total Base material use for priming & Spraying from power on of MCL system*/
              /*Send value to HMI & TAB*/
           //   stCurrentData.fCurr_BaseTotalMaterialUsedAccumulate += stCurrentData.fCurr_BaseTotalMaterialUsed;
							 stCurrentData.fCurr_BaseTotalMaterialUsedAccumulate += stCurrentData.fCurr_BaseMaterialApplied;
              /***********/
              /**************************SEND CAN MESSAGE*******************/
              stTx1Message.DLC = FIVE_BYTES;
              SplitByte.fData = stCurrentData.fCurr_BaseTotalMaterialUsedAccumulate;
              stTx1Message.Data[DATA_BYTE_ZERO] = BIT6_SET ;
              /**************************DATA***********************************/
              for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
              {
                stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
              }
              /****************************************************************/
              stTx1Message.ExtId = MSG_TYPE_DATA | DATA_ACCUMULATED_MATERIAL_USED |
                DEST_HMI | SOURCE_SCU;
              fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CAN_PARSER_DATA
              printf("\n\DATA_ACCUMULATED_MATERIAL_USED = %x\r\n", stTx1Message.ExtId);
#endif
              stTx1Message.ExtId = MSG_TYPE_DATA | DATA_ACCUMULATED_MATERIAL_USED |
                DEST_TABLET | SOURCE_SCU;	
              //	stWifi.ucSendFlag = SET;
              //	stEthernet.ucSendFlag = SET;
              fnXML_FIFO(&stTx1Message);
              /************************************************************/		
              /*Total Hard material used for priming and spraying of current joint*/
              stCurrentData.fCurr_HardTotalMaterialUsed = stCurrentData.fCurr_HardMaterialApplied + stCurrentData.fCurr_HardMaterialPrimed;
              /*change36*/
              /*Total Hard material use for priming & Spraying from power on of MCL system*/
              /*Send value to HMI & TAB*/
           //   stCurrentData.fCurr_HardTotalMaterialUsedAccumulate += stCurrentData.fCurr_HardTotalMaterialUsed;
							 stCurrentData.fCurr_HardTotalMaterialUsedAccumulate += stCurrentData.fCurr_HardMaterialApplied; /*change68*/
              /***********/	
              /**************************SEND CAN MESSAGE*******************/
              stTx1Message.DLC = FIVE_BYTES;
              SplitByte.fData = stCurrentData.fCurr_HardTotalMaterialUsedAccumulate;
              stTx1Message.Data[DATA_BYTE_ZERO] = BIT7_SET ;
              /**************************DATA***********************************/
              for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
              {
                stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
              }
              /****************************************************************/
              stTx1Message.ExtId = MSG_TYPE_DATA | DATA_ACCUMULATED_MATERIAL_USED |
                DEST_HMI | SOURCE_SCU;
              fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CAN_PARSER_DATA
              printf("\n\DATA_ACCUMULATED_MATERIAL_USED = %x\r\n", stTx1Message.ExtId);
#endif
              stTx1Message.ExtId = MSG_TYPE_DATA | DATA_ACCUMULATED_MATERIAL_USED |
                DEST_TABLET | SOURCE_SCU;	
              //	stWifi.ucSendFlag = SET;
              //	stEthernet.ucSendFlag = SET;
              fnXML_FIFO(&stTx1Message);
              /************************************************************/		
            }
            /*Total Base material used for current joint*/
            /*Send value to HMI & TAB*/
					//	 printf("\n\rstCurrentData.fCurr_BaseTotalMaterialUsed = %0.2f\r\n",  stCurrentData.fCurr_BaseTotalMaterialUsed);
            SplitByte.fData = stCurrentData.fCurr_BaseTotalMaterialUsed;
            /*********************FOR STATUS LOG ********************/
						SplitByte.fData = (float)(CONVERT_MM3_TO_L(SplitByte.fData));
						/*change66*/
            rgucStatusLog[BYTE_FIFTYONE] 	= SplitByte.rgucData[BYTE_ZERO] ;
            rgucStatusLog[BYTE_FIFTYTWO] 		= SplitByte.rgucData[BYTE_ONE] ;
            rgucStatusLog[BYTE_FIFTYTHREE] 				= SplitByte.rgucData[BYTE_TWO] ;
            rgucStatusLog[BYTE_FIFTYFOUR] 			= SplitByte.rgucData[BYTE_THREE] ;
            /**************************SEND CAN MESSAGE*******************/
            stTx1Message.DLC = FIVE_BYTES;
            SplitByte.fData = stCurrentData.fCurr_BaseTotalMaterialUsed;
            stTx1Message.Data[DATA_BYTE_ZERO] = BIT0_SET ;
            /**************************DATA***********************************/
            for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
            }
            /****************************************************************/
            stTx1Message.ExtId = MSG_TYPE_DATA | DATA_ACCUMULATED_MATERIAL_USED |
              DEST_HMI | SOURCE_SCU;
            fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CAN_PARSER_DATA
            printf("\n\DATA_ACCUMULATED_MATERIAL_USED = %x\r\n", stTx1Message.ExtId);
#endif
            stTx1Message.ExtId = MSG_TYPE_DATA | DATA_ACCUMULATED_MATERIAL_USED |
              DEST_TABLET | SOURCE_SCU;	
            //	stWifi.ucSendFlag = SET;
            //	stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
            /************************************************************/		
            
            /*Total Hard material used for current joint*/
            /*Send value to HMI & TAB*/
	//					printf("\n\rstCurrentData.fCurr_HardTotalMaterialUsed = %0.2f\r\n",  stCurrentData.fCurr_HardTotalMaterialUsed);
            SplitByte.fData = stCurrentData.fCurr_HardTotalMaterialUsed;	
            /*********************FOR STATUS LOG ********************/
						/*change66*/
						SplitByte.fData = (float)(CONVERT_MM3_TO_L(SplitByte.fData));
            rgucStatusLog[BYTE_FIFTYFIVE] 			= SplitByte.rgucData[BYTE_ZERO] ;
            rgucStatusLog[BYTE_FIFTYSIX] 		= SplitByte.rgucData[BYTE_ONE] ;
            rgucStatusLog[BYTE_FIFTYSEVEN] 		= SplitByte.rgucData[BYTE_TWO] ;
            rgucStatusLog[BYTE_FIFTYEIGHT] 		= SplitByte.rgucData[BYTE_THREE] ;
            /**************************SEND CAN MESSAGE*******************/
            stTx1Message.DLC = FIVE_BYTES;
            SplitByte.fData = stCurrentData.fCurr_HardTotalMaterialUsed;
            stTx1Message.Data[DATA_BYTE_ZERO] = BIT1_SET ;
            /**************************DATA***********************************/
            for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
            }
            /****************************************************************/
            stTx1Message.ExtId = MSG_TYPE_DATA | DATA_ACCUMULATED_MATERIAL_USED |
              DEST_HMI | SOURCE_SCU;
            fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CAN_PARSER_DATA
            printf("\n\DATA_ACCUMULATED_MATERIAL_USED = %x\r\n", stTx1Message.ExtId);
#endif
            stTx1Message.ExtId = MSG_TYPE_DATA | DATA_ACCUMULATED_MATERIAL_USED |
              DEST_TABLET | SOURCE_SCU;	
            //	stWifi.ucSendFlag = SET;
            //	stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
            /************************************************************/		
            /*Total Material used for current joint*/
						
            SplitByte.fData = stCurrentData.fCurr_HardTotalMaterialUsed + 
              stCurrentData.fCurr_BaseTotalMaterialUsed;	
					//	printf("\n\rSplitByte.fData= %0.2f\r\n",  SplitByte.fData);

            /*********************FOR STATUS LOG ********************/
						/*change66*/
						SplitByte.fData = (float)(CONVERT_MM3_TO_L(SplitByte.fData));
            rgucStatusLog[BYTE_FIFTYNINE] 			= SplitByte.rgucData[BYTE_ZERO] ;
            rgucStatusLog[BYTE_SIXTY] 		= SplitByte.rgucData[BYTE_ONE] ;
            rgucStatusLog[BYTE_SIXTYONE] 		= SplitByte.rgucData[BYTE_TWO] ;
            rgucStatusLog[BYTE_SIXTYTWO] 		= SplitByte.rgucData[BYTE_THREE] ;				
             /*change43 CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_120;
          }
          
        }
        break;
        /* DATA message recieved from VCM For Stall test pressure change 
        have to forward this message to TABLET also*/
      case DATA_STALL_PRESSURE_CHANGE:
        {
#ifdef DBG_CAN_PARSER_DATA      
          printf("DATA_STALL_PRESSURE_CHANGE \r\n");
#endif
          /******************FORWARD MESSAGE TO TABLET**********************/	
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          
          /****************************ABK*****************************/
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_TABLET	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          //						stWifi.ucSendFlag = SET;
          //						stEthernet.ucSendFlag = SET;
          fnXML_FIFO(&stTx1Message);
          /************************************************************/
          /************************UPSTROKE***************************/
          if(BIT0_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK))
          {
            SplitByte.rgucData[0] = pstCANrxmsg->Data[1];
            SplitByte.rgucData[1] = pstCANrxmsg->Data[2];
            stCurrentData.iCurr_PressureLegAUp = 	SplitByte.iData;
#ifdef DBG_CAN_PARSER_DATA
            printf("iCurr_PressureLegAUp: %d\r\n", 
                   stCurrentData.iCurr_PressureLegAUp );
#endif
            
            SplitByte.rgucData[0] = pstCANrxmsg->Data[3];
            SplitByte.rgucData[1] = pstCANrxmsg->Data[4];
            stCurrentData.iCurr_PressureLegBUp = SplitByte.iData;
#ifdef DBG_CAN_PARSER_DATA
            printf("iCurr_PressureLegBUp: %d\r\n",
                   stCurrentData.iCurr_PressureLegBUp);
#endif
            
            SplitByte.rgucData[0] = pstCANrxmsg->Data[5];
            SplitByte.rgucData[1] = pstCANrxmsg->Data[6];
            stCurrentData.iCurr_PressureLegCUp = SplitByte.iData;
#ifdef DBG_CAN_PARSER_DATA
            printf("iCurr_PressureLegCUp: %d\r\n", 
                   stCurrentData.iCurr_PressureLegCUp );
#endif
          }	
          /************************DOWNSTROKE***************************/
          if(BIT1_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT1_MASK))
          {
            SplitByte.rgucData[0] = pstCANrxmsg->Data[1];
            SplitByte.rgucData[1] = pstCANrxmsg->Data[2];
            stCurrentData.iCurr_PressureLegADown = 	SplitByte.iData;
#ifdef DBG_CAN_PARSER_DATA
            printf("iCurr_PressureLegADown: %d\r\n", 
                   stCurrentData.iCurr_PressureLegADown );
#endif
            
            SplitByte.rgucData[0] = pstCANrxmsg->Data[3];
            SplitByte.rgucData[1] = pstCANrxmsg->Data[4];
            stCurrentData.iCurr_PressureLegBDown = SplitByte.iData;
#ifdef DBG_CAN_PARSER_DATA
            printf("iCurr_PressureLegBDown: %d\r\n",
                   stCurrentData.iCurr_PressureLegBDown);
#endif
            
            SplitByte.rgucData[0] = pstCANrxmsg->Data[5];
            SplitByte.rgucData[1] = pstCANrxmsg->Data[6];
            stCurrentData.iCurr_PressureLegCDown = SplitByte.iData;
#ifdef DBG_CAN_PARSER_DATA
            printf("iCurr_PressureLegCDown: %d\r\n", 
                   stCurrentData.iCurr_PressureLegCDown );
#endif
          }					
          
        }
        break;
        /* DATA message recieved from VCM For Current pressure reading from pressure gauge1 
        have to forward this message to TABLET also*/
      case DATA_PGAUGE1_VALUE:
        {
#ifdef DBG_CAN_PARSER_DATA      
          printf("DATA_PGAUGE1_VALUE \r\n");
#endif
          /******************FORWARD MESSAGE TO TABLET**********************/	
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          
          /****************************ABK*****************************/
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_TABLET	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          //						stWifi.ucSendFlag = SET;
          //						stEthernet.ucSendFlag = SET;
          fnXML_FIFO(&stTx1Message);
          /************************************************************/
          for(ucLoopCnt=0; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
          {
            SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          
          
          stCurrentData.fCurr_PressureGauge1 = SplitByte.fData;
#ifdef DBG_CAN_PARSER_DATA
          
          printf("fCurr_PressureGauge1 %0.2f",
                 stCurrentData.fCurr_PressureGauge1);
          
#endif
          /*********************FOR PERIODIC LOG*******************/
          rgucPeriodicLog[BYTE_SIXTYTWO] 		= SplitByte.rgucData[BYTE_ZERO] ;
          rgucPeriodicLog[BYTE_SIXTYTHREE] 		= SplitByte.rgucData[BYTE_ONE] ;
          rgucPeriodicLog[BYTE_SIXTYFOUR] 		= SplitByte.rgucData[BYTE_TWO] ;
          rgucPeriodicLog[BYTE_SIXTYFIVE] 	= SplitByte.rgucData[BYTE_THREE] ;
          
        }
        break;
        /* DATA message recieved from VCM For Current pressure reading from pressure gauge2 
        have to forward this message to TABLET also*/
      case DATA_PGAUGE2_VALUE:
        {
#ifdef DBG_CAN_PARSER_DATA      
          printf("DATA_PGAUGE2_VALUE \r\n");
#endif	
          /******************FORWARD MESSAGE TO TABLET**********************/	
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          
          /****************************ABK*****************************/
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_TABLET	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          //						stWifi.ucSendFlag = SET;
          //						stEthernet.ucSendFlag = SET;
          fnXML_FIFO(&stTx1Message);
          /************************************************************/
          for(ucLoopCnt=0; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
          {
            SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          
          
          stCurrentData.fCurr_PressureGauge2 = SplitByte.fData;
#ifdef DBG_CAN_PARSER_DATA
          
          printf("fCurr_PressureGauge2 %0.2f",
                 stCurrentData.fCurr_PressureGauge2);
          
#endif
          /*********************FOR PERIODIC LOG*******************/
          rgucPeriodicLog[BYTE_SIXTYSIX] 		= SplitByte.rgucData[BYTE_ZERO] ;
          rgucPeriodicLog[BYTE_SIXTYSEVEN] 		= SplitByte.rgucData[BYTE_ONE] ;
          rgucPeriodicLog[BYTE_SIXTYEIGHT] 		= SplitByte.rgucData[BYTE_TWO] ;
          rgucPeriodicLog[BYTE_SIXTYNINE] 	= SplitByte.rgucData[BYTE_THREE] ;
          
        }
        break;
        /* DATA message recieved from VCM For Current pressure reading from pressure gauge3 
        have to forward this message to TABLET also*/
      case DATA_PGAUGE3_VALUE:
        {
#ifdef DBG_CAN_PARSER_DATA      
          printf("DATA_PGAUGE3_VALUE \r\n");
#endif	
          /******************FORWARD MESSAGE TO TABLET**********************/	
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          
          /****************************ABK*****************************/
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_TABLET	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          //						stWifi.ucSendFlag = SET;
          //						stEthernet.ucSendFlag = SET;
          fnXML_FIFO(&stTx1Message);
          /************************************************************/
          for(ucLoopCnt=0; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
          {
            SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          
          
          stCurrentData.fCurr_PressureGauge3 = SplitByte.fData;
#ifdef DBG_CAN_PARSER_DATA
          
          printf("fCurr_PressureGauge3 %0.2f",
                 stCurrentData.fCurr_PressureGauge3);
          
#endif
          /*********************FOR PERIODIC LOG*******************/
          rgucPeriodicLog[BYTE_SEVENTY] 		= SplitByte.rgucData[BYTE_ZERO] ;
          rgucPeriodicLog[BYTE_SEVENTYONE] 		= SplitByte.rgucData[BYTE_ONE] ;
          rgucPeriodicLog[BYTE_SEVENTYTWO] 		= SplitByte.rgucData[BYTE_TWO] ;
          rgucPeriodicLog[BYTE_SEVENTYTHREE] 	= SplitByte.rgucData[BYTE_THREE] ;
          
        }
        break;
        /* DATA message recieved from VCM For Current LT reading from Linear Transducer 
        have to forward this message to TABLET also*/
      case DATA_LT_READ:
        {
#ifdef DBG_CAN_PARSER_DATA      
          printf("DATA_LT_READ \r\n");
#endif	
          /******************FORWARD MESSAGE TO TABLET**********************/	
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          
          /****************************ABK*****************************/
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_TABLET	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          //						stWifi.ucSendFlag = SET;
          //						stEthernet.ucSendFlag = SET;
          fnXML_FIFO(&stTx1Message);
          /************************************************************/
          for(ucLoopCnt=0; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
          {
            SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          
          
          stCurrentData.fCurr_LinearTransducer = SplitByte.fData;
#ifdef DBG_CAN_PARSER_DATA
          
          printf("fCurr_LinearTransducer %0.2f",
                 stCurrentData.fCurr_LinearTransducer);
          
#endif
        }
        break;
        /* DATA message recieved from OPMM For Current Pipe Temperature reading from Non Contact Temp Sensor 
        have to forward this message to TABLET also*/
      case DATA_OPMM_PIPE_TEMP:
        {
#ifdef DBG_CAN_PARSER_DATA      
          printf("DATA_OPMM_PIPE_TEMP \r\n");
#endif	
          /******************FORWARD MESSAGE TO TABLET & HMI **********************/	
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_HMI	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          fnCANMsg_Transmit(&stTx1Message, CAN1);			
          /****************************ABK*****************************/
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_TABLET	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          //						stWifi.ucSendFlag = SET;
          //						stEthernet.ucSendFlag = SET;
          fnXML_FIFO(&stTx1Message);
          /************************************************************/
          for(ucLoopCnt=1; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
          {
            SplitByte.rgucData[ucLoopCnt-1] = pstCANrxmsg->Data[ucLoopCnt];
          }
          
          if(BIT0_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK))
          {
            
            stCurrentData.fCurr_PipeTempPos12 = SplitByte.fData;
#ifdef DBG_CAN_PARSER_DATA
            
            printf("fCurr_PipeTempPos12 %0.2f",
                   stCurrentData.fCurr_PipeTempPos12);
            
#endif
          }
          if(BIT1_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT1_MASK))
          {
            
            stCurrentData.fCurr_PipeTempPos3 = SplitByte.fData;
#ifdef DBG_CAN_PARSER_DATA
            
            printf("fCurr_PipeTempPos3 %0.2f",
                   stCurrentData.fCurr_PipeTempPos3);
            
#endif
          }
          if(BIT2_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT2_MASK))
          {
            
            stCurrentData.fCurr_PipeTempPos6 = SplitByte.fData;
#ifdef DBG_CAN_PARSER_DATA
            
            printf("fCurr_PipeTempPos6 %0.2f",
                   stCurrentData.fCurr_PipeTempPos6);
            
#endif
          }
          if(BIT3_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT3_MASK))
          {
            
            stCurrentData.fCurr_PipeTempPos9 = SplitByte.fData;
#ifdef DBG_CAN_PARSER_DATA
            
            printf("fCurr_PipeTempPos9 %0.2f",
                   stCurrentData.fCurr_PipeTempPos9);
            
#endif
          }
          if(BIT4_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT4_MASK))
          {
            
            stCurrentData.fCurr_PipeTempAvg = SplitByte.fData;
#ifdef DBG_CAN_PARSER_DATA
            
            printf("fCurr_PipeTempAvg %0.2f",
                   stCurrentData.fCurr_PipeTempAvg);
            
#endif
          }
          stProcessInput.stProcessFlag.bReadPipeTempRecieveData = RESET;
        }
        break;
        /* DATA message recieved from OPMM For Current Battery status 
        have to forward this message to TABLET also*/
      case DATA_OPMM_BATTERY_STATUS:
        {
#ifdef DBG_CAN_PARSER_DATA      
          printf("DATA_OPMM_BATTERY_STATUS \r\n");
#endif	
          /******************FORWARD MESSAGE TO TABLET & HMI **********************/	
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_HMI	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          fnCANMsg_Transmit(&stTx1Message, CAN1);			
          /****************************ABK*****************************/
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_TABLET	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          //						stWifi.ucSendFlag = SET;
          //						stEthernet.ucSendFlag = SET;
          fnXML_FIFO(&stTx1Message);
          /************************************************************/
          for(ucLoopCnt=0; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
          {
            SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          
          
          stCurrentData.fCurr_BatteryStatus = SplitByte.fData;
#ifdef DBG_CAN_PARSER_DATA
          
          printf("fCurr_BatteryStatus %0.2f\n\r",
                 stCurrentData.fCurr_BatteryStatus);
          
#endif
        }
        break;
        /* DATA message recieved from OPMM For Process time for prime Spray & Flush 
        have to forward this message to TABLET also*/
      case DATA_PROCESS_TIME:
        {
#ifdef DBG_CAN_PARSER_DATA      
          printf("DATA_PROCESS_TIME \r\n");
#endif	
					
//          /******************FORWARD MESSAGE TO TABLET & HMI **********************/	
//          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
//                                (DEST_HMI	 | SOURCE_SCU));
//          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
//          stTx1Message.DLC = pstCANrxmsg->DLC; 
//          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
//          {
//            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
//          }
//          fnCANMsg_Transmit(&stTx1Message, CAN1);		
//          /****************************ABK*****************************/
//          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
//                                (DEST_TABLET	 | SOURCE_SCU));
//          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
//          //						stWifi.ucSendFlag = SET;
//          //						stEthernet.ucSendFlag = SET;
//          fnXML_FIFO(&stTx1Message);
//          /************************************************************/
          for(ucLoopCnt=1; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
          {
            SplitByte.rgucData[ucLoopCnt-1] = pstCANrxmsg->Data[ucLoopCnt];
          }
          
          if(BIT0_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK))
          {
						/*change67*/
						stCurrentData.fCurr_SprayTime = 0;
						stCurrentData.fCurr_FlushTime = 0;
						
            //printf("Rx Prime time: %0.2f\n\r", SplitByte.fData);
						
            stCurrentData.fCurr_PrimeTime += SplitByte.fData;
						SplitByte.fData = stCurrentData.fCurr_PrimeTime;
#ifdef DBG_CAN_PARSER_DATA
            
            printf("fCurr_PrimeTime %0.2f\n\r",
                   stCurrentData.fCurr_PrimeTime);
            
#endif
						/*change62*/
						rgucStatusLog[BYTE_SIXTYTHREE] 			= SplitByte.rgucData[BYTE_ZERO] ;
            rgucStatusLog[BYTE_SIXTYFOUR] 		= SplitByte.rgucData[BYTE_ONE] ;
            rgucStatusLog[BYTE_SIXTYFIVE] 		= SplitByte.rgucData[BYTE_TWO] ;
            rgucStatusLog[BYTE_SIXTYSIX] 		= SplitByte.rgucData[BYTE_THREE] ;
          }
          if(BIT1_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT1_MASK))
          {
						/*change67*/
            stCurrentData.fCurr_PrimeTime = 0;
						stCurrentData.fCurr_FlushTime = 0;
            stCurrentData.fCurr_SprayTime += SplitByte.fData;
						SplitByte.fData = stCurrentData.fCurr_SprayTime;
#ifdef DBG_CAN_PARSER_DATA
            
            printf("fCurr_SprayTime %0.2f\n\r",
                   stCurrentData.fCurr_SprayTime);
            
#endif
						/*change62*/
						rgucStatusLog[BYTE_SEVENTYONE] 			= SplitByte.rgucData[BYTE_ZERO] ;
            rgucStatusLog[BYTE_SEVENTYTWO] 		= SplitByte.rgucData[BYTE_ONE] ;
            rgucStatusLog[BYTE_SEVENTYTHREE] 		= SplitByte.rgucData[BYTE_TWO] ;
            rgucStatusLog[BYTE_SEVENTYFOUR] 		= SplitByte.rgucData[BYTE_THREE] ;
          }
          if(BIT2_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT2_MASK))
          {
						/*change67*/
            stCurrentData.fCurr_PrimeTime = 0;
						stCurrentData.fCurr_SprayTime = 0;
            stCurrentData.fCurr_FlushTime += SplitByte.fData;
						SplitByte.fData = stCurrentData.fCurr_FlushTime;
#ifdef DBG_CAN_PARSER_DATA
            
            printf("fCurr_FlushTime %0.2f\n\r",
                   stCurrentData.fCurr_FlushTime);
            
#endif
						/*change62*/
						rgucStatusLog[BYTE_SIXTYSEVEN] 			= SplitByte.rgucData[BYTE_ZERO] ;
            rgucStatusLog[BYTE_SIXTYEIGHT] 		= SplitByte.rgucData[BYTE_ONE] ;
            rgucStatusLog[BYTE_SIXTYNINE] 		= SplitByte.rgucData[BYTE_TWO] ;
            rgucStatusLog[BYTE_SEVENTY] 		= SplitByte.rgucData[BYTE_THREE] ;
          }
					/*change69*/
					          /******************FORWARD MESSAGE TO TABLET & HMI **********************/	
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_HMI	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          stTx1Message.DLC = pstCANrxmsg->DLC; 
					stTx1Message.Data[0] = pstCANrxmsg->Data[0];
          for(ucLoopCnt=1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt - 1];
          }
          fnCANMsg_Transmit(&stTx1Message, CAN1);		
          /****************************ABK*****************************/
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_TABLET	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          //						stWifi.ucSendFlag = SET;
          //						stEthernet.ucSendFlag = SET;
          fnXML_FIFO(&stTx1Message);
          /************************************************************/

					
        }
        break;
        /* DATA message recieved from OPMM Number of rotation completed 
        have to forward this message to TABLET also*/
      case DATA_ROTATIONS_COMPLETED:
        {
#ifdef DBG_CAN_PARSER_DATA      
          printf("DATA_ROTATIONS_COMPLETED \r\n");
#endif	
          /******************FORWARD MESSAGE TO TABLET & HMI **********************/	
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_HMI	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          fnCANMsg_Transmit(&stTx1Message, CAN1);		
          /****************************ABK*****************************/
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_TABLET	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          //						stWifi.ucSendFlag = SET;
          //						stEthernet.ucSendFlag = SET;
          fnXML_FIFO(&stTx1Message);
          /************************************************************/
          stCurrentData.ucCurr_NumberofRotation = pstCANrxmsg->Data[0];
#ifdef DBG_CAN_PARSER_DATA
          printf("ucCurr_NumberofRotation: %d", 
                 stCurrentData.ucCurr_NumberofRotation);
#endif
        }
        break;
        /* DATA message recieved from OPMM current base temp on opmm 
        have to forward this message to TABLET also*/
      case DATA_OPMM_BASE_TEMP:
        {
#ifdef DBG_CAN_PARSER_DATA      
          printf("DATA_OPMM_BASE_TEMP \r\n");
#endif	
          /******************FORWARD MESSAGE TO TABLET & HMI **********************/	
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_HMI	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          fnCANMsg_Transmit(&stTx1Message, CAN1);			
          /****************************ABK*****************************/
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_TABLET	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          //						stWifi.ucSendFlag = SET;
          //						stEthernet.ucSendFlag = SET;
          fnXML_FIFO(&stTx1Message);
          /************************************************************/
          for(ucLoopCnt=0; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
          {
            SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          
          
          stCurrentData.fCurr_OPMMBaseTemp = SplitByte.fData;
#ifdef DBG_CAN_PARSER_DATA
          
          printf("fCurr_OPMMBaseTemp %0.2f",
                 stCurrentData.fCurr_OPMMBaseTemp);
          
#endif
          /*********************FOR STATUS LOG ********************/
          rgucStatusLog[BYTE_TWENTYFIVE] 			= SplitByte.rgucData[BYTE_ZERO] ;
          rgucStatusLog[BYTE_TWENTYSIX] 			= SplitByte.rgucData[BYTE_ONE] ;
          rgucStatusLog[BYTE_TWENTYSEVEN] 				= SplitByte.rgucData[BYTE_TWO] ;
          rgucStatusLog[BYTE_TWENTYEIGHT] 		= SplitByte.rgucData[BYTE_THREE] ;
          
          /*********************FOR PERIODIC LOG*******************/
          rgucPeriodicLog[BYTE_TWENTYSIX] 		= SplitByte.rgucData[BYTE_ZERO] ;
          rgucPeriodicLog[BYTE_TWENTYSEVEN] 		= SplitByte.rgucData[BYTE_ONE] ;
          rgucPeriodicLog[BYTE_TWENTYEIGHT] 		= SplitByte.rgucData[BYTE_TWO] ;
          rgucPeriodicLog[BYTE_TWENTYNINE] 	= SplitByte.rgucData[BYTE_THREE] ;
          
          
        }
        break;
        /* DATA message recieved from OPMM current hard temp on opmm 
        have to forward this message to TABLET also*/
      case DATA_OPMM_HARD_TEMP:
        {
#ifdef DBG_CAN_PARSER_DATA      
          printf("DATA_OPMM_HARD_TEMP \r\n");
#endif	
          /******************FORWARD MESSAGE TO TABLET & HMI **********************/	
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_HMI	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          fnCANMsg_Transmit(&stTx1Message, CAN1);			
          /****************************ABK*****************************/
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_TABLET	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          //						stWifi.ucSendFlag = SET;
          //						stEthernet.ucSendFlag = SET;
          fnXML_FIFO(&stTx1Message);
          /************************************************************/
          for(ucLoopCnt=0; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
          {
            SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          
          
          
          stCurrentData.fCurr_OPMMHardTemp = SplitByte.fData;
#ifdef DBG_CAN_PARSER_DATA
          
          printf("fCurr_OPMMHardTemp %0.2f",
                 stCurrentData.fCurr_OPMMHardTemp);
          
#endif
          /*********************FOR STATUS LOG ********************/
          rgucStatusLog[BYTE_TWENTYNINE] 				= SplitByte.rgucData[BYTE_ZERO] ;
          rgucStatusLog[BYTE_THIRTY] 			= SplitByte.rgucData[BYTE_ONE] ;
          rgucStatusLog[BYTE_THIRTYONE] 			= SplitByte.rgucData[BYTE_TWO] ;
          rgucStatusLog[BYTE_THIRTYTWO] 				= SplitByte.rgucData[BYTE_THREE] ;
          
          /*********************FOR PERIODIC LOG*******************/
          rgucPeriodicLog[BYTE_THIRTY] 		= SplitByte.rgucData[BYTE_ZERO] ;
          rgucPeriodicLog[BYTE_THIRTYONE] 			= SplitByte.rgucData[BYTE_ONE] ;
          rgucPeriodicLog[BYTE_THIRTYTWO] 					= SplitByte.rgucData[BYTE_TWO] ;
          rgucPeriodicLog[BYTE_THIRTYTHREE] 			= SplitByte.rgucData[BYTE_THREE] ;
          
        }
        break;
				/*change76*/
				case DATA_DCLS_HIT_COUNTER:
        {
#ifdef DBG_CAN_PARSER_DATA      
          printf("DATA_DCLS_HIT_COUNTER \r\n");
#endif	
        //stCurrentData.ucCurr_NumberofRotation = pstCANrxmsg->Data[0];
#ifdef DBG_CAN_PARSER_DATA
          printf("DCLS HIT COUNT: %d\n\r", 
                 pstCANrxmsg->Data[0]);
					printf("HLS HIT COUNT: %d\n\r", 
                 pstCANrxmsg->Data[1]);
					printf("CURRENT CASE: %d\n\r", 
                 pstCANrxmsg->Data[2]);
#endif
        }
        break;

        /* DATA message recieved from OPMM Link fault count for CAN 
        have to forward this message to TABLET also change20*/
        //				case DATA_OPMM_LINK_FAULT_CNT:
        //				{
        //					#ifdef DBG_CAN_PARSER_DATA      
        //					printf("DATA_OPMM_LINK_FAULT_CNT \r\n");
        //					#endif
        //						/******************FORWARD MESSAGE TO TABLET & HMI **********************/	
        //						pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
        //																	(DEST_HMI	 | SOURCE_SCU));
        //						stTx1Message.ExtId = pstCANrxmsg->ExtId; 
        //						stTx1Message.DLC = pstCANrxmsg->DLC; 
        //						for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
        //						{
        //							stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
        //						}
        //						fnCANMsg_Transmit(&stTx1Message,CAN1);				
        //						/****************************ABK*****************************/
        //						pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
        //																	(DEST_TABLET	 | SOURCE_SCU));
        //						stTx1Message.ExtId = pstCANrxmsg->ExtId; 
        ////						stWifi.ucSendFlag = SET;
        ////						stEthernet.ucSendFlag = SET;
        //						fnXML_FIFO(&stTx1Message);
        //						/************************************************************/
        //					for(ucLoopCnt=1; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
        //						{
        //								SplitByte.rgucData[ucLoopCnt-1] = pstCANrxmsg->Data[ucLoopCnt];
        //						}
        
        //						if(BIT0_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK))
        //						{
        //							
        //						stCurrentData.fCurr_CAN1ALinkFault = SplitByte.fData;
        //						#ifdef DBG_CAN_PARSER_DATA
        //							
        //							printf("fCurr_CAN1ALinkFault %0.2f",
        //											stCurrentData.fCurr_CAN1ALinkFault);
        //							
        //						#endif
        //						}
        //						if(BIT1_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT1_MASK))
        //						{
        //							
        //						stCurrentData.fCurr_CAN1BLinkFault = SplitByte.fData;
        //						#ifdef DBG_CAN_PARSER_DATA
        //						
        //							printf("fCurr_CAN1BLinkFault %0.2f",
        //											stCurrentData.fCurr_CAN1BLinkFault);
        //							
        //						#endif
        //						}
        //					
        //				}
        //				break;
        /* DATA message recieved from VCM For Air Flow 
        have to forward this message to TABLET also change20*/
        //				case DATA_VCM_AIR_FLOW:
        //				{
        //					#ifdef DBG_CAN_PARSER_DATA      
        //					printf("DATA_VCM_AIR_FLOW \r\n");
        //					#endif	
        //					/******************FORWARD MESSAGE TO TABLET & OPMM **********************/	
        //						stTx1Message.DLC = pstCANrxmsg->DLC; 
        //						for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
        //						{
        //							stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
        //						}
        //						
        //						/****************************ABK*****************************/
        //						pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
        //																	(DEST_TABLET	 | SOURCE_SCU));
        //						stTx1Message.ExtId = pstCANrxmsg->ExtId; 
        ////						stWifi.ucSendFlag = SET;
        ////						stEthernet.ucSendFlag = SET;
        //						fnXML_FIFO(&stTx1Message);
        //						/************************************************************/
        //					for(ucLoopCnt=0; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
        //						{
        //								SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
        //						}
        
        //						
        //							
        //						stCurrentData.fCurr_AIRFlow = SplitByte.fData;
        //						#ifdef DBG_CAN_PARSER_DATA
        //							
        //							printf("fCurr_AIRFlow %0.2f",
        //											stCurrentData.fCurr_AIRFlow);
        //							
        //						#endif
        //						
        //				}
        //				break;
      }
    }
    break;
    /************CONFIG MESSAGE*************/		
  case MSG_TYPE_CONFIG:
    {
#ifdef DBG_CAN_PARSER_CONFIG      
      printf("MSG_TYPE_CONFIG\r\n");
#endif	
      
      switch (uliCanID & MSG_MASK)
      {
      case ACKNOWLEDGEMENT:
        {
          fnCANPost_Ack(pstCANrxmsg,CAN1);
#ifdef DBG_CAN_PARSER_CONFIG      
          printf("ACKNOWLEDGE FOR CONFIG MESSAGE..\r\n");
#endif	
        }
        break;	
        /* Recieve Configuration parameters for spray pressure from HMI */
      case CONFIG_SPRAY_PRESSURE:
        {
#ifdef DBG_CAN_PARSER_CONFIG      
          printf("CONFIG_SPRAY_PRESSURE\r\n");
#endif
          
          /**************************DATA***********************************/
          for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            SplitByte.rgucData[ucLoopCnt - 1] = pstCANrxmsg->Data[ucLoopCnt];
            
          }
          /****************************************************************/		
          if(BIT0_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK))
          {
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_59;
            /*****************************/
            stConfigData.fConfig_SprayPressure = SplitByte.fData;
#ifdef DBG_CAN_PARSER_CONFIG
            
            printf(" SET fConfig_SprayPressure %f",
                   stConfigData.fConfig_SprayPressure);
#endif
          }
          if(BIT1_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT1_MASK))
          {
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_60;
            /*****************************/
            
            stConfigData.fConfig_SprayPressureLTH = SplitByte.fData;
#ifdef DBG_CAN_PARSER_CONFIG
            
            printf("LOW fConfig_SprayPressureLTH %0.2f",
                   stConfigData.fConfig_SprayPressureLTH);
            
#endif
          }
          if(BIT2_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT2_MASK))
          {
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_61;
            /*****************************/
            
            stConfigData.fConfig_SprayPressureHTH = SplitByte.fData;
#ifdef DBG_CAN_PARSER_CONFIG
            
            printf("HIGH fConfig_SprayPressureHTH %0.2f",
                   stConfigData.fConfig_SprayPressureHTH);
            
#endif
          }
          if(BIT3_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT3_MASK))
          {
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_62;
            /*****************************/
            
            stConfigData.fConfig_HysSprayPresPos = SplitByte.fData;
#ifdef DBG_CAN_PARSER_CONFIG
            
            printf("POS fConfig_HysSprayPresPos %0.2f",
                   stConfigData.fConfig_HysSprayPresPos);
            
#endif
          }
          if(BIT4_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT4_MASK))
          {
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_63;
            /*****************************/
            
            stConfigData.fConfig_HysSprayPresNeg = SplitByte.fData;
#ifdef DBG_CAN_PARSER_CONFIG
            
            printf("NEG fConfig_HysSprayPresNeg %0.2f",
                   stConfigData.fConfig_HysSprayPresNeg);
            
#endif
          }
          /*change32*/
          if(SOURCE_HMI == (uliCanID & SOURCE_ADDR_MASK))
          {
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            /****************************ABK*****************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_TABLET	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
//            printf("pstCANrxmsg->ExtId = %x \r\n",pstCANrxmsg->ExtId);
            ////						stWifi.ucSendFlag = SET;
            ////						stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
          }				
        }
        break;
        /* Recieve Configuration parameters for Ricirculation pressure from HMI */
      case CONFIG_RECIRC_PRESSURE:
        {
          
#ifdef DBG_CAN_PARSER_CONFIG      
          printf("CONFIG_RECIRC_PRESSURE\r\n");
#endif	
          /**************************DATA***********************************/
          for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            SplitByte.rgucData[ucLoopCnt - 1] = pstCANrxmsg->Data[ucLoopCnt];
          }
          /****************************************************************/		
          if(BIT0_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK))
          {
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_64;
            /*****************************/
            
            stConfigData.fConfig_RecirPressure = SplitByte.fData;
#ifdef DBG_CAN_PARSER_CONFIG
            
            printf(" SET fConfig_RecirPressure %0.2f",
                   stConfigData.fConfig_RecirPressure);
#endif
          }
          if(BIT1_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT1_MASK))
          {
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_65;
            /*****************************/
            
            stConfigData.fConfig_RecirPressureLTH = SplitByte.fData;
#ifdef DBG_CAN_PARSER_CONFIG
            
            printf("LOW fConfig_RecirPressureLTH %0.2f",
                   stConfigData.fConfig_RecirPressureLTH);
            
#endif
          }
          if(BIT2_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT2_MASK))
          {
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_66;
            /*****************************/
            
            stConfigData.fConfig_RecirPressureHTH = SplitByte.fData;
#ifdef DBG_CAN_PARSER_CONFIG
            
            printf("HIGH fConfig_RecirPressureHTH %0.2f",
                   stConfigData.fConfig_RecirPressureHTH);
            
#endif
          }
          if(BIT3_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT3_MASK))
          {
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_67;
            /*****************************/
            
            stConfigData.fConfig_HysRecircPresPos = SplitByte.fData;
#ifdef DBG_CAN_PARSER_CONFIG
            
            printf("POS fConfig_HysRecircPresPos %0.2f",
                   stConfigData.fConfig_HysRecircPresPos);
            
#endif
          }
          if(BIT4_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT4_MASK))
          {
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_68;
            /*****************************/
            
            stConfigData.fConfig_HysRecircPresNeg = SplitByte.fData;
#ifdef DBG_CAN_PARSER_CONFIG
            
            printf("NEG fConfig_HysRecircPresNeg %0.2f",
                   stConfigData.fConfig_HysRecircPresNeg);
            
#endif
          }
          /*change32*/
          if(SOURCE_HMI == (uliCanID & SOURCE_ADDR_MASK))
          {
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            /****************************ABK*****************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_TABLET	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
            printf("pstCANrxmsg->ExtId = %x \r\n",pstCANrxmsg->ExtId);
            ////						stWifi.ucSendFlag = SET;
            ////						stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
          }				
          
        }
        break;
        /* Recieve Configuration parameters for Joint Number from HMI */
      case CONFIG_JOINT_NUMBER:
        {
          
#ifdef DBG_CAN_PARSER_CONFIG      
          printf("CONFIG_JOINT_NUMBER\r\n");
#endif	
          /**************************DATA***********************************/
          for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          /****************************************************************/		
          /* CONFIG PARAMETER NO */
          ucEEPROMCounter = CONFIG_PARA_69;
          /*****************************/
          
          stConfigData.fConfig_JointNum = SplitByte.fData;
#ifdef DBG_CAN_PARSER_CONFIG
          
          printf(" SET fConfig_JointNum %0.2f\n\r",
                 stConfigData.fConfig_JointNum);
        printf("uliCanID = %x \r\n",uliCanID);  
#endif
          /*********************FOR STATUS LOG ********************/
          rgucStatusLog[BYTE_NINE] 			= SplitByte.rgucData[BYTE_ZERO] ;
          rgucStatusLog[BYTE_TEN] 		= SplitByte.rgucData[BYTE_ONE] ;
          rgucStatusLog[BYTE_ELEVEN] 		= SplitByte.rgucData[BYTE_TWO] ;
          rgucStatusLog[BYTE_TWELVE] 			= SplitByte.rgucData[BYTE_THREE] ;
          
          /*********************FOR PERIODIC LOG*******************/
          rgucPeriodicLog[BYTE_TEN] 	= SplitByte.rgucData[BYTE_ZERO] ;
          rgucPeriodicLog[BYTE_ELEVEN] 		= SplitByte.rgucData[BYTE_ONE] ;
          rgucPeriodicLog[BYTE_TWELVE] 		= SplitByte.rgucData[BYTE_TWO] ;
          rgucPeriodicLog[BYTE_THIRTEEN] 	= SplitByte.rgucData[BYTE_THREE] ;
          
          /*change32*/
          if(SOURCE_HMI == (uliCanID & SOURCE_ADDR_MASK))
          {
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            /****************************ABK*****************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_TABLET	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
           #ifdef DBG_CAN_PARSER_CONFIG
          		printf("pstCANrxmsg->ExtId = %x \r\n",pstCANrxmsg->ExtId);
						#endif 
            ////						stWifi.ucSendFlag = SET;
            ////						stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
          }				
          
        }
        break;
        /* Recieve Configuration parameters for data logging parameter from HMI */
      case CONFIG_DATA_LOG_PERIOD:
        {
          
#ifdef DBG_CAN_PARSER_CONFIG      
          printf("CONFIG_DATA_LOG_PERIOD\r\n");
#endif
          /**************************DATA***********************************/
          for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          /****************************************************************/		
          /* CONFIG PARAMETER NO */
          ucEEPROMCounter = CONFIG_PARA_70;
          /*****************************/
          
          stConfigData.fConfig_DataLoggingPeriod = SplitByte.fData;
#ifdef DBG_CAN_PARSER_CONFIG
          
          printf(" SET fConfig_DataLoggingPeriod %0.2f",
                 stConfigData.fConfig_DataLoggingPeriod);
          
#endif			
          /*change32*/
          if(SOURCE_HMI == (uliCanID & SOURCE_ADDR_MASK))
          {
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            /****************************ABK*****************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_TABLET	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
//            printf("pstCANrxmsg->ExtId = %x \r\n",pstCANrxmsg->ExtId);
            ////						stWifi.ucSendFlag = SET;
            ////						stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
          }										
        }
        break;
        /* Recieve Configuration parameters for system date and time parameter from HMI */
      case CONFIG_SYS_DATE_TIME:
        {
#ifdef DBG_CAN_PARSER_CONFIG      
          printf("CONFIG_SYS_DATE_TIME\r\n");
#endif	
          /**************************DATA***********************************/
          for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stConfigData.ucConfig_SystemTime[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          /****************************************************************/		
          /* CONFIG PARAMETER NO */
          ucEEPROMCounter = CONFIG_PARA_71;
          /*****************************/
          
          /*** WRITE TO RTC REGISTERS ALSO***/
          /* Updating the Time in the RTC registers*/
          stRTC_TimeStructure.RTC_Hours 	= stConfigData.ucConfig_SystemTime[0];
          stRTC_TimeStructure.RTC_Minutes = stConfigData.ucConfig_SystemTime[1];
          stRTC_TimeStructure.RTC_Seconds = stConfigData.ucConfig_SystemTime[2];
          stRTC_TimeStructure.RTC_H12 		= RTC_H12_AM;
          
          stRTC_DateStructure.RTC_Date = stConfigData.ucConfig_SystemTime[3];
          stRTC_DateStructure.RTC_Month = stConfigData.ucConfig_SystemTime[4];
          stRTC_DateStructure.RTC_Year = stConfigData.ucConfig_SystemTime[6];/*Year Low Byte*/
          
          RTC_SetDate(RTC_Format_BIN, &stRTC_DateStructure);
          RTC_SetTime(RTC_Format_BIN, &stRTC_TimeStructure);
          /*change32*/
          if(SOURCE_HMI == (uliCanID & SOURCE_ADDR_MASK))
          {
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            /****************************ABK*****************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_TABLET	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
//            printf("pstCANrxmsg->ExtId = %x \r\n",pstCANrxmsg->ExtId);
            ////						stWifi.ucSendFlag = SET;
            ////						stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
          }				
        }
        break;
        /* Recieve Configuration parameters for Supervisor name parameter from HMI */
      case CONFIG_SUPERVISOR_NAME:
        {
          
#ifdef DBG_CAN_PARSER_CONFIG      
          printf("CONFIG_SUPERVISOR_NAME\r\n");
#endif	
          if(FRAME_1 == pstCANrxmsg->Data[0])
          {
            /*change25*/
            /*before updating the name save previous to flash*/
         /*   memset(supervisorname,'\0',FLASH_RX_TX_BUFFER_SIZE);
            strcat((char *)supervisorname,(char *)stConfigData.rgucConfig_SupervisorName1);
            strcat((char *)supervisorname,(char *)stConfigData.rgucConfig_SupervisorName2);
            strcat((char *)supervisorname,(char *)stConfigData.rgucConfig_SupervisorName3);	
            fnStoreSupervisorName(ucSupervisorNameID,&supervisorname[0]);	*/
						
						/*change78*/						
						memset(supervisornamepre,'\0',FLASH_RX_TX_BUFFER_SIZE);
            strcat((char *)supervisornamepre,(char *)stConfigData.rgucConfig_SupervisorName1);
            strcat((char *)supervisornamepre,(char *)stConfigData.rgucConfig_SupervisorName2);
            strcat((char *)supervisornamepre,(char *)stConfigData.rgucConfig_SupervisorName3);	
						StatusFlag.stStatusFlag.bConfigSupervisorName = SET;
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_72;
            /*****************************/
            
            for(ucLoopCnt = 1; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              stConfigData.rgucConfig_SupervisorName1[ucLoopCnt - 1] = 
                pstCANrxmsg->Data[ucLoopCnt];
            }
            stConfigData.rgucConfig_SupervisorName1[ucLoopCnt - 1] = NULL;
            
#ifdef DBG_CAN_PARSER_CONFIG
            printf("%s\r\n",stConfigData.rgucConfig_SupervisorName1);
#endif
            /*change23*/
         //   ucSupervisorNameID++;
            /*change26*/ 
            /* Check for Supervisor ID and Job ID Range to reset Flash*/
           // fnCheck_ID();
#ifdef DBG_CAN_PARSER_CONFIG
            printf("%d\r\n",ucSupervisorNameID);
#endif
          }
          if(FRAME_2 == pstCANrxmsg->Data[0])
          {
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_73;
            /*****************************/
            
            
            for(ucLoopCnt = 1; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              stConfigData.rgucConfig_SupervisorName2[ucLoopCnt - 1] = 
                pstCANrxmsg->Data[ucLoopCnt];
            }
            stConfigData.rgucConfig_SupervisorName2[ucLoopCnt - 1] = NULL;
#ifdef DBG_CAN_PARSER_CONFIG
            printf("%s\r\n", stConfigData.rgucConfig_SupervisorName2);
#endif
          }
          if(FRAME_3 == pstCANrxmsg->Data[0])
          {
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_74;
            /*****************************/
            
            for(ucLoopCnt = 1; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              stConfigData.rgucConfig_SupervisorName3[ucLoopCnt - 1] = 
                pstCANrxmsg->Data[ucLoopCnt];
            }
            stConfigData.rgucConfig_SupervisorName3[ucLoopCnt - 1] = NULL;
#ifdef DBG_CAN_PARSER_CONFIG
            printf("%s\r\n",stConfigData.rgucConfig_SupervisorName3);
#endif
            
          }
          /*change32*/
          if(SOURCE_HMI == (uliCanID & SOURCE_ADDR_MASK))
          {
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            /****************************ABK*****************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_TABLET	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
//            printf("pstCANrxmsg->ExtId = %x \r\n",pstCANrxmsg->ExtId);
            ////						stWifi.ucSendFlag = SET;
            ////						stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
          }				
        }
        break;
        /* Recieve Configuration parameters for coating material parameter from HMI */
      case CONFIG_COATING_MATERIAL:
        {
          
#ifdef DBG_CAN_PARSER_CONFIG      
          printf("CONFIG_COATING_MATERIAL\r\n");
#endif
          if(FRAME_1 == pstCANrxmsg->Data[0])
          {
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_75;
            /*****************************/
            
            for(ucLoopCnt = 1; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              stConfigData.rgucConfig_CoatingMatName1[ucLoopCnt - 1] = 
                pstCANrxmsg->Data[ucLoopCnt];
            }
            stConfigData.rgucConfig_CoatingMatName1[ucLoopCnt - 1] = NULL;
#ifdef DBG_CAN_PARSER_CONFIG
            printf("%s\r\n",stConfigData.rgucConfig_CoatingMatName1);
#endif
          }
          if(FRAME_2 == pstCANrxmsg->Data[0])
          {
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_76;
            /*****************************/
            
            for(ucLoopCnt = 1; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              stConfigData.rgucConfig_CoatingMatName2[ucLoopCnt - 1] = 
                pstCANrxmsg->Data[ucLoopCnt];
            }
            stConfigData.rgucConfig_CoatingMatName2[ucLoopCnt - 1] = NULL;
#ifdef DBG_CAN_PARSER_CONFIG
            printf("%s\r\n", stConfigData.rgucConfig_CoatingMatName2);
#endif
          }
          if(FRAME_3 == pstCANrxmsg->Data[0])
          {
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_77;
            /*****************************/
            
            for(ucLoopCnt = 1; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              stConfigData.rgucConfig_CoatingMatName3[ucLoopCnt - 1] = 
                pstCANrxmsg->Data[ucLoopCnt];
            }
            stConfigData.rgucConfig_CoatingMatName3[ucLoopCnt - 1] = NULL;
#ifdef DBG_CAN_PARSER_CONFIG
            printf("%s\r\n",stConfigData.rgucConfig_CoatingMatName3);
#endif
          }
          
          /*change32*/
          if(SOURCE_HMI == (uliCanID & SOURCE_ADDR_MASK))
          {
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            /****************************ABK*****************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_TABLET	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
//            printf("pstCANrxmsg->ExtId = %x \r\n",pstCANrxmsg->ExtId);
            ////						stWifi.ucSendFlag = SET;
            ////						stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
          }				
        }
        break;
        /* Recieve Configuration parameters for base material  batch number parameter from HMI */
      case CONFIG_BASE_MAT_BATCH_NO:
        {
          
#ifdef DBG_CAN_PARSER_CONFIG      
          printf("CONFIG_BASE_MAT_BATCH_NO\r\n");
#endif	
          if(FRAME_1 == pstCANrxmsg->Data[0])
          {
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_78;
            /*****************************/
            
            for(ucLoopCnt = 1; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              stConfigData.rgucConfig_BaseMatBatchNo1[ucLoopCnt - 1] = 
                pstCANrxmsg->Data[ucLoopCnt];
            }
            stConfigData.rgucConfig_BaseMatBatchNo1[ucLoopCnt - 1] = NULL;
#ifdef DBG_CAN_PARSER_CONFIG
            printf("%s\r\n",stConfigData.rgucConfig_BaseMatBatchNo1);
#endif
          }
          if(FRAME_2 == pstCANrxmsg->Data[0])
          {
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_79;
            /*****************************/
            
            for(ucLoopCnt = 1; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              stConfigData.rgucConfig_BaseMatBatchNo2[ucLoopCnt - 1] = 
                pstCANrxmsg->Data[ucLoopCnt];
            }
            stConfigData.rgucConfig_BaseMatBatchNo2[ucLoopCnt - 1] = NULL;
#ifdef DBG_CAN_PARSER_CONFIG
            printf("%s\r\n", stConfigData.rgucConfig_BaseMatBatchNo2);
#endif
          }
          if(FRAME_3 == pstCANrxmsg->Data[0])
          {
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_80;
            /*****************************/
            
            for(ucLoopCnt = 1; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              stConfigData.rgucConfig_BaseMatBatchNo3[ucLoopCnt - 1] = 
                pstCANrxmsg->Data[ucLoopCnt];
            }
            stConfigData.rgucConfig_BaseMatBatchNo3[ucLoopCnt - 1] = NULL;
#ifdef DBG_CAN_PARSER_CONFIG
            printf("%s\r\n",stConfigData.rgucConfig_BaseMatBatchNo3);
#endif
          }
          /*change32*/
          if(SOURCE_HMI == (uliCanID & SOURCE_ADDR_MASK))
          {
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            /****************************ABK*****************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_TABLET	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
//            printf("pstCANrxmsg->ExtId = %x \r\n",pstCANrxmsg->ExtId);
            ////						stWifi.ucSendFlag = SET;
            ////						stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
          }				
        }
        break;
        /* Recieve Configuration parameters for hard material  batch number parameter from HMI */
      case CONFIG_HARD_MAT_BATCH_NO:
        {
          
#ifdef DBG_CAN_PARSER_CONFIG      
          printf("CONFIG_HARD_MAT_BATCH_NO\r\n");
#endif	
          if(FRAME_1 == pstCANrxmsg->Data[0])
          {
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_81;
            /*****************************/
            
            for(ucLoopCnt = 1; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              stConfigData.rgucConfig_HardMatBatchNo1[ucLoopCnt - 1] = 
                pstCANrxmsg->Data[ucLoopCnt];
            }
            stConfigData.rgucConfig_HardMatBatchNo1[ucLoopCnt - 1] = NULL;
#ifdef DBG_CAN_PARSER_CONFIG
            printf("%s\r\n",stConfigData.rgucConfig_HardMatBatchNo1);
#endif
          }
          if(FRAME_2 == pstCANrxmsg->Data[0])
          {
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_82;
            /*****************************/
            
            for(ucLoopCnt = 1; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              stConfigData.rgucConfig_HardMatBatchNo2[ucLoopCnt - 1] = 
                pstCANrxmsg->Data[ucLoopCnt];
            }
            stConfigData.rgucConfig_HardMatBatchNo2[ucLoopCnt - 1] = NULL;
#ifdef DBG_CAN_PARSER_CONFIG
            printf("%s\r\n", stConfigData.rgucConfig_HardMatBatchNo2);
#endif
          }
          if(FRAME_3 == pstCANrxmsg->Data[0])
          {
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_83;
            /*****************************/
            
            for(ucLoopCnt = 1; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              stConfigData.rgucConfig_HardMatBatchNo3[ucLoopCnt - 1] = 
                pstCANrxmsg->Data[ucLoopCnt];
            }
            stConfigData.rgucConfig_HardMatBatchNo3[ucLoopCnt - 1] = NULL;
#ifdef DBG_CAN_PARSER_CONFIG
            printf("%s\r\n",stConfigData.rgucConfig_HardMatBatchNo3);
#endif
          }
          /*change32*/
          if(SOURCE_HMI == (uliCanID & SOURCE_ADDR_MASK))
          {
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            /****************************ABK*****************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_TABLET	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
//            printf("pstCANrxmsg->ExtId = %x \r\n",pstCANrxmsg->ExtId);
            ////						stWifi.ucSendFlag = SET;
            ////						stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
          }				
        }
        break;
        /* Recieve Configuration parameters for Type of units parameter from HMI */
      case CONFIG_TYPE_OF_UNIT:
        {
          
#ifdef DBG_CAN_PARSER_CONFIG      
          printf("CONFIG_TYPE_OF_UNIT\r\n");
#endif	
          /* CONFIG PARAMETER NO */
          ucEEPROMCounter = CONFIG_PARA_84;
          /*****************************/
          
          stConfigData.ucConfig_TypeofUnit = pstCANrxmsg->Data[0];
#ifdef DBG_CAN_PARSER_CONFIG
          printf("CONFIG_TYPE_OF_UNIT: %d\r\n",
                 stConfigData.ucConfig_TypeofUnit);
#endif
          
          /*change32*/
          if(SOURCE_HMI == (uliCanID & SOURCE_ADDR_MASK))
          {
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            /****************************ABK*****************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_TABLET	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
//            printf("pstCANrxmsg->ExtId = %x \r\n",pstCANrxmsg->ExtId);
            ////						stWifi.ucSendFlag = SET;
            ////						stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
          }				
        }
        break;
        /* Recieve Configuration parameters for Pipe temperature parameter from HMI */
      case CONFIG_PIPE_TEMP_TH:
        {
#ifdef DBG_CAN_PARSER_CONFIG      
          printf("CONFIG_PIPE_TEMP_TH\r\n");
#endif	
          for(ucLoopCnt = 1; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
          {
            SplitByte.rgucData[ucLoopCnt - 1] = pstCANrxmsg->Data[ucLoopCnt];
          }
          if(BIT0_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK))
          {
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_86;
            /*****************************/
            
            stConfigData.fConfig_PipeTempLTH = SplitByte.fData;
#ifdef DBG_CAN_PARSER_CONFIG
            
            printf("fConfig_PipeTempLTH LOW: %0.2f", 
                   stConfigData.fConfig_PipeTempLTH);
            
#endif
          }
          if(BIT1_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT1_MASK))
          {
            
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_87;
            /*****************************/
            
            stConfigData.fConfig_PipeTempHTH= SplitByte.fData;
#ifdef DBG_CAN_PARSER_CONFIG
            
            printf("fConfig_PipeTempHTH HIGH: %0.2f", 
                   stConfigData.fConfig_PipeTempHTH);
            
#endif
          }
          /*change32*/
          if(SOURCE_HMI == (uliCanID & SOURCE_ADDR_MASK))
          {
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            /****************************ABK*****************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_TABLET	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
//            printf("pstCANrxmsg->ExtId = %x \r\n",pstCANrxmsg->ExtId);
            ////						stWifi.ucSendFlag = SET;
            ////						stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
          }				
        }
        break;
        
        /* Recieve Configuration parameters for Spraying count from HMI */
      case CONFIG_SPRAY_COUNT:
        {
#ifdef DBG_CAN_PARSER_CONFIG      
          printf("CONFIG_SPRAY_COUNT\r\n");
#endif	
          /* CONFIG PARAMETER NO */
          ucEEPROMCounter = CONFIG_PARA_46;
          /*****************************/
          stConfigData.ucConfig_SprayCount = pstCANrxmsg->Data[0];
#ifdef DBG_CAN_PARSER_CONFIG
          printf("ucConfig_SprayCount: %d\r\n", 
                 stConfigData.ucConfig_SprayCount );
#endif
          
          /******************FORWARD MESSAGE TO OPMM**********************/	
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_OPMM	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          fnCANMsg_Transmit(&stTx1Message, CAN1);
          
          /*change32*/
          if(SOURCE_HMI == (uliCanID & SOURCE_ADDR_MASK))
          {
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            /****************************ABK*****************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_TABLET	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
//            printf("pstCANrxmsg->ExtId = %x \r\n",pstCANrxmsg->ExtId);
            ////						stWifi.ucSendFlag = SET;
            ////						stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
          }				
          
        }
        break;
        /* Recieve Configuration parameters for Prime time config from HMI */
      case CONFIG_PRIME_TIME:
        {
#ifdef DBG_CAN_PARSER_CONFIG      
          printf("CONFIG_PRIME_TIME\r\n");
#endif	
          /**************************DATA***********************************/
          for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          /****************************************************************/		
          /* CONFIG PARAMETER NO */
          ucEEPROMCounter = CONFIG_PARA_47;
          /*****************************/
          
          stConfigData.fConfig_PrimeTime = SplitByte.fData;
#ifdef DBG_CAN_PARSER_CONFIG
          printf(" SET fConfig_PrimeTime %0.2f",
                 stConfigData.fConfig_PrimeTime);
#endif					
          /******************FORWARD MESSAGE TO OPMM**********************/	
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_OPMM	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          fnCANMsg_Transmit(&stTx1Message, CAN1);
          
          /*change32*/
          if(SOURCE_HMI == (uliCanID & SOURCE_ADDR_MASK))
          {
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            /****************************ABK*****************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_TABLET	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
//            printf("pstCANrxmsg->ExtId = %x \r\n",pstCANrxmsg->ExtId);
            ////						stWifi.ucSendFlag = SET;
            ////						stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
          }				
          
        }
        break;
        /* Recieve Configuration parameters for OPMM Gun Delay time config from HMI */
      case CONFIG_OPMM_GUN_DELAY:
        {
          
#ifdef DBG_CAN_PARSER_CONFIG      
          printf("CONFIG_OPMM_GUN_DELAY\r\n");
#endif	
          
          /**************************DATA***********************************/
          for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          /****************************************************************/		
          /* CONFIG PARAMETER NO */
          ucEEPROMCounter = CONFIG_PARA_48;
          /*****************************/
          
          stConfigData.fConfig_GunDelayTime = SplitByte.fData;
#ifdef DBG_CAN_PARSER_CONFIG
          printf(" SET fConfig_GunDelayTime %0.2f",
                 stConfigData.fConfig_GunDelayTime);
#endif					
          /******************FORWARD MESSAGE TO OPMM**********************/	
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_OPMM	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          fnCANMsg_Transmit(&stTx1Message, CAN1);
          
          /*change32*/
          if(SOURCE_HMI == (uliCanID & SOURCE_ADDR_MASK))
          {
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            /****************************ABK*****************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_TABLET	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
//            printf("pstCANrxmsg->ExtId = %x \r\n",pstCANrxmsg->ExtId);
            ////						stWifi.ucSendFlag = SET;
            ////						stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
          }				
        }
        break;
        /* Recieve Configuration parameters for OPMM spray overlap time config from HMI */
      case CONFIG_OPMM_SPRAY_OVERLAP_TIME:
        {
#ifdef DBG_CAN_PARSER_CONFIG      
          printf("CONFIG_OPMM_SPRAY_OVERLAP_TIME\r\n");
#endif
          /**************************DATA***********************************/
          for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          /****************************************************************/		
          /* CONFIG PARAMETER NO */
          ucEEPROMCounter = CONFIG_PARA_49;
          /*****************************/
          
          stConfigData.fConfig_SprayOverlapTime = SplitByte.fData;
#ifdef DBG_CAN_PARSER_CONFIG
          printf(" SET fConfig_SprayOverlapTime %0.2f",
                 stConfigData.fConfig_SprayOverlapTime);
#endif					
          /******************FORWARD MESSAGE TO OPMM**********************/	
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_OPMM	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          fnCANMsg_Transmit(&stTx1Message, CAN1);				
          
          /*change32*/
          if(SOURCE_HMI == (uliCanID & SOURCE_ADDR_MASK))
          {
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            /****************************ABK*****************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_TABLET	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
//            printf("pstCANrxmsg->ExtId = %x \r\n",pstCANrxmsg->ExtId);
            ////						stWifi.ucSendFlag = SET;
            ////						stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
          }										
        }
        break;
        /* Recieve Configuration parameters for OPMM reverse delay time config from HMI */
      case CONFIG_OPMM_REVERSE_DELAY:
        {					
#ifdef DBG_CAN_PARSER_CONFIG      
          printf("CONFIG_OPMM_REVERSE_DELAY\r\n");
#endif	
          /**************************DATA***********************************/
          for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          /****************************************************************/		
          /* CONFIG PARAMETER NO */
          ucEEPROMCounter = CONFIG_PARA_50;
          /*****************************/
          
          stConfigData.fConfig_ReverseDelayTime = SplitByte.fData;
#ifdef DBG_CAN_PARSER_CONFIG
          printf(" SET fConfig_ReverseDelayTime %0.2f",
                 stConfigData.fConfig_ReverseDelayTime);
#endif					
          /******************FORWARD MESSAGE TO OPMM**********************/	
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_OPMM	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          fnCANMsg_Transmit(&stTx1Message, CAN1);			
          
          /*change32*/
          if(SOURCE_HMI == (uliCanID & SOURCE_ADDR_MASK))
          {
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            /****************************ABK*****************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_TABLET	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
//            printf("pstCANrxmsg->ExtId = %x \r\n",pstCANrxmsg->ExtId);
            ////						stWifi.ucSendFlag = SET;
            ////						stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
          }										
        }
        break;
        /* Recieve Configuration parameters for OPMM Flush time config from HMI */
      case CONFIG_OPMM_FLUSH_TIME:
        {
          
#ifdef DBG_CAN_PARSER_CONFIG      
          printf("CONFIG_OPMM_FLUSH_TIME\r\n");
#endif	
          /**************************DATA***********************************/
          for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          /****************************************************************/		
          /* CONFIG PARAMETER NO */
          ucEEPROMCounter = CONFIG_PARA_51;
          /*****************************/
          
          stConfigData.fConfig_FlushTime = SplitByte.fData;
#ifdef DBG_CAN_PARSER_CONFIG
          printf(" SET fConfig_FlushTime %0.2f",
                 stConfigData.fConfig_FlushTime);
#endif					
          /******************FORWARD MESSAGE TO OPMM**********************/	
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_OPMM	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          fnCANMsg_Transmit(&stTx1Message, CAN1);		
          
          
          /*change32*/
          if(SOURCE_HMI == (uliCanID & SOURCE_ADDR_MASK))
          {
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            /****************************ABK*****************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_TABLET	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
//            printf("pstCANrxmsg->ExtId = %x \r\n",pstCANrxmsg->ExtId);
            ////						stWifi.ucSendFlag = SET;
            ////						stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
          }				
        }
        break;
        /* Recieve Configuration parameters for DCLS HLS time config from HMI */
      case CONFIG_DCLS_HOME_TIME:
        {
          
#ifdef DBG_CAN_PARSER_CONFIG      
          printf("CONFIG_DCLS_HOME_TIME\r\n");
#endif	
          /**************************DATA***********************************/
          for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            SplitByte.rgucData[ucLoopCnt-1] = pstCANrxmsg->Data[ucLoopCnt];
          }
          /****************************************************************/
          if(BIT0_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK))
          {					
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_52;
            /*****************************/
            stConfigData.fConfig_DCLSTime = SplitByte.fData;
#ifdef DBG_CAN_PARSER_CONFIG
            printf(" SET fConfig_DCLSTime %0.2f",
                   stConfigData.fConfig_DCLSTime);
#endif	
          }
          if(BIT1_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT1_MASK))
          {					
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_53;
            /*****************************/
            stConfigData.fConfig_HOMETime = SplitByte.fData;
#ifdef DBG_CAN_PARSER_CONFIG
            printf(" SET fConfig_HOMETime %0.2f",
                   stConfigData.fConfig_HOMETime);
#endif	
          }
          /******************FORWARD MESSAGE TO OPMM**********************/	
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_OPMM	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          fnCANMsg_Transmit(&stTx1Message, CAN1);	
          
          /*change32*/
          if(SOURCE_HMI == (uliCanID & SOURCE_ADDR_MASK))
          {
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            /****************************ABK*****************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_TABLET	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
//            printf("pstCANrxmsg->ExtId = %x \r\n",pstCANrxmsg->ExtId);
            ////						stWifi.ucSendFlag = SET;
            ////						stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
          }										
        }
        break;
        /* Recieve Configuration parameters for Gun Position Change time from HMI */
      case CONFIG_GUN_POSITION_CHANGE_TIME:
        {
#ifdef DBG_CAN_PARSER_CONFIG      
          printf("CONFIG_GUN_POSITION_CHANGE_TIME\r\n");
#endif	
          /**************************DATA***********************************/
          for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          /****************************************************************/		
          /* CONFIG PARAMETER NO */
          ucEEPROMCounter = CONFIG_PARA_54;
          /*****************************/
          
          stConfigData.fConfig_GunPosChangeTime = SplitByte.fData;
#ifdef DBG_CAN_PARSER_CONFIG
          printf(" SET fConfig_GunPosChangeTime %0.2f",
                 stConfigData.fConfig_GunPosChangeTime);
#endif					
          /******************FORWARD MESSAGE TO OPMM**********************/	
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_OPMM	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          fnCANMsg_Transmit(&stTx1Message, CAN1);		
          
          /*change32*/
          if(SOURCE_HMI == (uliCanID & SOURCE_ADDR_MASK))
          {
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            /****************************ABK*****************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_TABLET	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
//            printf("pstCANrxmsg->ExtId = %x \r\n",pstCANrxmsg->ExtId);
            ////						stWifi.ucSendFlag = SET;
            ////						stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
          }				
        }
        break;
        /* Recieve Configuration parameters for Spray Valve open time on Battery */
      case CONFIG_SPRAY_VALVE_OPEN_TIME_VBAT:
        {
          
#ifdef DBG_CAN_PARSER_CONFIG      
          printf("CONFIG_SPRAY_VALVE_OPEN_TIME_VBAT\r\n");
#endif	
          
          /**************************DATA***********************************/
          for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          /****************************************************************/		
          /* CONFIG PARAMETER NO */
          ucEEPROMCounter = CONFIG_PARA_55;
          /*****************************/
          
          stConfigData.fConfig_TimebeforeONSprayValve = SplitByte.fData;
#ifdef DBG_CAN_PARSER_CONFIG
          printf(" SET fConfig_TimebeforeONSprayValve %0.2f",
                 stConfigData.fConfig_TimebeforeONSprayValve);
#endif					
          /******************FORWARD MESSAGE TO OPMM**********************/	
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_OPMM	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          fnCANMsg_Transmit(&stTx1Message, CAN1);		
          
          /*change32*/
          if(SOURCE_HMI == (uliCanID & SOURCE_ADDR_MASK))
          {
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            /****************************ABK*****************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_TABLET	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
//            printf("pstCANrxmsg->ExtId = %x \r\n",pstCANrxmsg->ExtId);
            ////						stWifi.ucSendFlag = SET;
            ////						stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
          }				
        }
        break;
        /* Recieve Configuration parameters for Flush warning time from HMI */
      case CONFIG_FLUSH_WARNNING_TIME:
        {
          
#ifdef DBG_CAN_PARSER_CONFIG      
          printf("CONFIG_FLUSH_WARNNING_TIME\r\n");
#endif	
          /**************************DATA***********************************/
          for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          /****************************************************************/		
          /* CONFIG PARAMETER NO */
          ucEEPROMCounter = CONFIG_PARA_56;
          /*****************************/
          
          stConfigData.fConfig_FlushWarnTime = SplitByte.fData;
#ifdef DBG_CAN_PARSER_CONFIG
          printf(" SET fConfig_FlushWarnTime %0.2f",
                 stConfigData.fConfig_FlushWarnTime);
#endif					
          /******************FORWARD MESSAGE TO OPMM**********************/	
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_OPMM	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          fnCANMsg_Transmit(&stTx1Message, CAN1);		
          
          
          /*change32*/
          if(SOURCE_HMI == (uliCanID & SOURCE_ADDR_MASK))
          {
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            /****************************ABK*****************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_TABLET	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
//            printf("pstCANrxmsg->ExtId = %x \r\n",pstCANrxmsg->ExtId);
            ////						stWifi.ucSendFlag = SET;
            ////						stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
          }				
        }
        break;
        /* Recieve Configuration parameters for VCM Stall threshold in %  from HMI */
      case CONFIG_VCM_STALL_TH:
        {
#ifdef DBG_CAN_PARSER_CONFIG      
          printf("CONFIG_VCM_STALL_TH\r\n");
#endif	
          /****************************************************************/
          if(BIT0_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK))
          {	
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_1;
            /*****************************/
            SplitByte.rgucData[0]	= pstCANrxmsg->Data[1];
            SplitByte.rgucData[1]	= pstCANrxmsg->Data[2];
            stConfigData.iConfig_UpStrokeTH	= SplitByte.iData;							
#ifdef DBG_CAN_PARSER_CONFIG
            printf("%d\r\n", stConfigData.iConfig_UpStrokeTH);
#endif
          }
          if(BIT1_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT1_MASK))
          {			
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_2;
            /*****************************/
            SplitByte.rgucData[0]	= pstCANrxmsg->Data[1];
            SplitByte.rgucData[1]	= pstCANrxmsg->Data[2];
            stConfigData.iConfig_DownStrokeTH	= SplitByte.iData;							
#ifdef DBG_CAN_PARSER_CONFIG
            printf("%d\r\n", stConfigData.iConfig_DownStrokeTH);
#endif	
          }
          /******************FORWARD MESSAGE TO VCM**********************/	
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_VCM	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          fnCANMsg_Transmit(&stTx1Message, CAN1);		
          
          
          /*change32*/
          if(SOURCE_HMI == (uliCanID & SOURCE_ADDR_MASK))
          {
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            /****************************ABK*****************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_TABLET	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
            printf("pstCANrxmsg->ExtId = %x \r\n",pstCANrxmsg->ExtId);
            ////						stWifi.ucSendFlag = SET;
            ////						stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
          }				
        }
        break;
        /* Recieve Configuration parameters for VCM Pressure Guage1 thresholdfrom HMI */
      case CONFIG_VCM_PGAUGE1_TH:
        {
#ifdef DBG_CAN_PARSER_CONFIG      
          printf("CONFIG_VCM_PGAUGE1_TH\r\n");
#endif	
          /**************************DATA***********************************/
          for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            SplitByte.rgucData[ucLoopCnt-1] = pstCANrxmsg->Data[ucLoopCnt];
          }
          /****************************************************************/
          if(BIT0_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK))
          {					
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_3;
            /*****************************/
            stConfigData.fConfig_PressureGauge1LTH = SplitByte.fData;
#ifdef DBG_CAN_PARSER_CONFIG
            printf(" SET fConfig_PressureGauge1LTH %0.2f",
                   stConfigData.fConfig_PressureGauge1LTH);
#endif	
          }
          if(BIT1_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT1_MASK))
          {					
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_4;
            /*****************************/
            stConfigData.fConfig_PressureGauge1HTH = SplitByte.fData;
#ifdef DBG_CAN_PARSER_CONFIG
            printf(" SET fConfig_PressureGauge1HTH %0.2f",
                   stConfigData.fConfig_PressureGauge1HTH);
#endif	
          }
          /******************FORWARD MESSAGE TO VCM**********************/	
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_VCM	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          fnCANMsg_Transmit(&stTx1Message, CAN1);		
          
          /*change32*/
          if(SOURCE_HMI == (uliCanID & SOURCE_ADDR_MASK))
          {
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            /****************************ABK*****************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_TABLET	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
//            printf("pstCANrxmsg->ExtId = %x \r\n",pstCANrxmsg->ExtId);
            ////						stWifi.ucSendFlag = SET;
            ////						stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
          }				
          
        }
        break;
        /* Recieve Configuration parameters for VCM Pressure Guage2 thresholdfrom HMI */
      case CONFIG_VCM_PGAUGE2_TH:
        {
          
#ifdef DBG_CAN_PARSER_CONFIG      
          printf("CONFIG_VCM_PGAUGE2_TH\r\n");
#endif	
          
          /**************************DATA***********************************/
          for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            SplitByte.rgucData[ucLoopCnt-1] = pstCANrxmsg->Data[ucLoopCnt];
          }
          /****************************************************************/
          if(BIT0_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK))
          {					
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_5;
            /*****************************/
            stConfigData.fConfig_PressureGauge2LTH = SplitByte.fData;
#ifdef DBG_CAN_PARSER_CONFIG
            printf(" SET fConfig_PressureGauge2LTH %0.2f",
                   stConfigData.fConfig_PressureGauge2LTH);
#endif	
          }
          if(BIT1_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT1_MASK))
          {					
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_6;
            /*****************************/
            stConfigData.fConfig_PressureGauge2HTH = SplitByte.fData;
#ifdef DBG_CAN_PARSER_CONFIG
            printf(" SET fConfig_PressureGauge2HTH %0.2f",
                   stConfigData.fConfig_PressureGauge2HTH);
#endif	
          }
          /******************FORWARD MESSAGE TO VCM**********************/	
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_VCM	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          fnCANMsg_Transmit(&stTx1Message, CAN1);		
          
          /*change32*/
          if(SOURCE_HMI == (uliCanID & SOURCE_ADDR_MASK))
          {
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            /****************************ABK*****************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_TABLET	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
//            printf("pstCANrxmsg->ExtId = %x \r\n",pstCANrxmsg->ExtId);
            ////						stWifi.ucSendFlag = SET;
            ////						stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
          }				
        }
        break;
        /* Recieve Configuration parameters for VCM Pressure Guage3 thresholdfrom HMI */
      case CONFIG_VCM_PGAUGE3_TH:
        {
          
#ifdef DBG_CAN_PARSER_CONFIG      
          printf("CONFIG_VCM_PGAUGE3_TH\r\n");
#endif	
          /**************************DATA***********************************/
          for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            SplitByte.rgucData[ucLoopCnt-1] = pstCANrxmsg->Data[ucLoopCnt];
          }
          /****************************************************************/
          if(BIT0_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK))
          {					
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_7;
            /*****************************/
            stConfigData.fConfig_PressureGauge3LTH = SplitByte.fData;
#ifdef DBG_CAN_PARSER_CONFIG
            printf(" SET fConfig_PressureGauge3LTH %0.2f",
                   stConfigData.fConfig_PressureGauge3LTH);
#endif	
          }
          if(BIT1_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT1_MASK))
          {					
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_8;
            /*****************************/
            stConfigData.fConfig_PressureGauge3HTH = SplitByte.fData;
#ifdef DBG_CAN_PARSER_CONFIG
            printf(" SET fConfig_PressureGauge3HTH %0.2f",
                   stConfigData.fConfig_PressureGauge3HTH);
#endif	
          }
          /******************FORWARD MESSAGE TO VCM**********************/	
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_VCM	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          fnCANMsg_Transmit(&stTx1Message, CAN1);		
          
          
          /*change32*/
          if(SOURCE_HMI == (uliCanID & SOURCE_ADDR_MASK))
          {
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            /****************************ABK*****************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_TABLET	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
//            printf("pstCANrxmsg->ExtId = %x \r\n",pstCANrxmsg->ExtId);
            ////						stWifi.ucSendFlag = SET;
            ////						stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
          }				
        }
        break;
        /* Recieve Configuration parameters for  VCM Surface area of cylinder from HMI */
      case CONFIG_CYLINDER_SURFACE_AREA:
        {
#ifdef DBG_CAN_PARSER_CONFIG      
          printf("CONFIG_CYLINDER_SURFACE_AREA\r\n");
#endif
          /**************************DATA***********************************/
          for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            SplitByte.rgucData[ucLoopCnt-1] = pstCANrxmsg->Data[ucLoopCnt];
          }
          /****************************************************************/
          if(BIT0_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK))
          {					
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_9;
            /*****************************/
            stConfigData.fConfig_AreaofCylinderA = SplitByte.fData;
#ifdef DBG_CAN_PARSER_CONFIG
            
            printf(" SET fConfig_AreaofCylinderA %0.2f",
                   stConfigData.fConfig_AreaofCylinderA);
            
#endif	
          }
          if(BIT1_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT1_MASK))
          {					
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_10;
            /*****************************/
            stConfigData.fConfig_AreaofCylinderB = SplitByte.fData;
#ifdef DBG_CAN_PARSER_CONFIG
            printf(" SET fConfig_AreaofCylinderB %0.2f",
                   stConfigData.fConfig_AreaofCylinderB);
#endif	
          }
          if(BIT2_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT2_MASK))
          {					
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_11;
            /*****************************/
            stConfigData.fConfig_AreaofCylinderC = SplitByte.fData;
#ifdef DBG_CAN_PARSER_CONFIG
            
            printf(" SET fConfig_AreaofCylinderC %0.2f",
                   stConfigData.fConfig_AreaofCylinderC);
            
#endif	
          }
          /******************FORWARD MESSAGE TO VCM**********************/	
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_VCM	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          fnCANMsg_Transmit(&stTx1Message, CAN1);		
          
          /*change32*/
          if(SOURCE_HMI == (uliCanID & SOURCE_ADDR_MASK))
          {
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            /****************************ABK*****************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_TABLET	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
//            printf("pstCANrxmsg->ExtId = %x \r\n",pstCANrxmsg->ExtId);
            ////						stWifi.ucSendFlag = SET;
            ////						stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
          }				
          
        }
        break;
        /* Recieve Configuration parameters  VCM Number of cylinder from HMI */
      case CONFIG_NUMBER_OF_CYLINDER:
        {					
#ifdef DBG_CAN_PARSER_CONFIG      
          printf("CONFIG_NUMBER_OF_CYLINDER\r\n");
#endif	
          /* CONFIG PARAMETER NO */
          ucEEPROMCounter = CONFIG_PARA_12;
          /*****************************/
          stConfigData.ucConfig_CylinderCount = pstCANrxmsg->Data[0];
#ifdef DBG_CAN_PARSER_CONFIG
          printf("CONFIG_NUMBER_OF_CYLINDER: %d", 
                 stConfigData.ucConfig_CylinderCount );
#endif
          /******************FORWARD MESSAGE TO VCM**********************/	
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_VCM	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          fnCANMsg_Transmit(&stTx1Message, CAN1);		
          
          /*change32*/
          if(SOURCE_HMI == (uliCanID & SOURCE_ADDR_MASK))
          {
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            /****************************ABK*****************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_TABLET	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
//            printf("pstCANrxmsg->ExtId = %x \r\n",pstCANrxmsg->ExtId);
            ////						stWifi.ucSendFlag = SET;
            ////						stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
          }				
        }
        break;
        /* Recieve Configuration parameters for VCM time before stall valve to close in stall test from HMI */
      case CONFIG_TIME_BEFORE_STALL:
        {
#ifdef DBG_CAN_PARSER_CONFIG      
          printf("CONFIG_TIME_BEFORE_STALL\r\n");
#endif	
          /**************************DATA***********************************/
          for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          /****************************************************************/		
          /* CONFIG PARAMETER NO */
          ucEEPROMCounter = CONFIG_PARA_18;
          /*****************************/
          
          stConfigData.fConfig_TimeBeforeStall = SplitByte.fData;
#ifdef DBG_CAN_PARSER_CONFIG
          printf(" SET fConfig_TimeBeforeStall %0.2f",
                 stConfigData.fConfig_TimeBeforeStall);
#endif					
          /******************FORWARD MESSAGE TO VCM**********************/	
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_VCM	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          fnCANMsg_Transmit(&stTx1Message, CAN1);		
          
          /*change32*/
          if(SOURCE_HMI == (uliCanID & SOURCE_ADDR_MASK))
          {
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            /****************************ABK*****************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_TABLET	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
//            printf("pstCANrxmsg->ExtId = %x \r\n",pstCANrxmsg->ExtId);
            ////						stWifi.ucSendFlag = SET;
            ////						stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
          }				
        }
        break;
        /* Recieve Configuration parameters for VCM time before stall valve to open in stall test from HMI */
      case CONFIG_TIME_BEFORE_COMPARE_STALL:
        {
#ifdef DBG_CAN_PARSER_CONFIG      
          printf("CONFIG_TIME_BEFORE_COMPARE_STALL\r\n");
#endif	
          /**************************DATA***********************************/
          for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          /****************************************************************/		
          /* CONFIG PARAMETER NO */
          ucEEPROMCounter = CONFIG_PARA_19;
          /*****************************/
          
          stConfigData.fConfig_TimeCompareStall = SplitByte.fData;
#ifdef DBG_CAN_PARSER_CONFIG
          printf(" SET fConfig_TimeCompareStall %0.2f",
                 stConfigData.fConfig_TimeCompareStall);
#endif					
          /******************FORWARD MESSAGE TO VCM**********************/	
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_VCM	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          fnCANMsg_Transmit(&stTx1Message, CAN1);		
          
          /*change32*/
          if(SOURCE_HMI == (uliCanID & SOURCE_ADDR_MASK))
          {
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            /****************************ABK*****************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_TABLET	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
//            printf("pstCANrxmsg->ExtId = %x \r\n",pstCANrxmsg->ExtId);
            ////						stWifi.ucSendFlag = SET;
            ////						stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
          }				
        }
        break;
        /* Recieve Configuration parameters for HCM base temperature setting from HMI */
      case CONFIG_BASE_TEMP_SETTING:
        {
#ifdef DBG_CAN_PARSER_CONFIG      
          printf("CONFIG_BASE_TEMP_SETTING\r\n");
#endif	
          /**************************DATA***********************************/
          for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            SplitByte.rgucData[ucLoopCnt-1] = pstCANrxmsg->Data[ucLoopCnt];
          }
          /****************************************************************/		
          if(BIT0_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK))
          {					
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_20;
            /*****************************/
            stConfigData.fConfig_AntiFrzTempSet = SplitByte.fData;
#ifdef DBG_CAN_PARSER_CONFIG
            printf(" SET fConfig_AntiFrzTempSet %0.2f",
                   stConfigData.fConfig_AntiFrzTempSet);
            
#endif	
          }
          if(BIT1_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT1_MASK))
          {					
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_21;
            /*****************************/
            stConfigData.fConfig_AntiFrzTempLTH = SplitByte.fData;
#ifdef DBG_CAN_PARSER_CONFIG
            
            printf(" SET fConfig_AntiFrzTempLTH %0.2f",
                   stConfigData.fConfig_AntiFrzTempLTH);
            
#endif	
          }
          if(BIT2_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT2_MASK))
          {					
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_22;
            /*****************************/
            stConfigData.fConfig_AntiFrzTempHTH = SplitByte.fData;
#ifdef DBG_CAN_PARSER_CONFIG
            printf(" SET fConfig_AntiFrzTempHTH %0.2f",
                   stConfigData.fConfig_AntiFrzTempHTH);
#endif	
          }
          if(BIT3_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT3_MASK))
          {					
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_23;
            /*****************************/
            stConfigData.fConfig_HysAntiFrzPos = SplitByte.fData;
#ifdef DBG_CAN_PARSER_CONFIG
            printf(" SET fConfig_HysAntiFrzPos %0.2f",
                   stConfigData.fConfig_HysAntiFrzPos);
            
#endif	
          }
          if(BIT4_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT4_MASK))
          {					
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_24;
            /*****************************/
            stConfigData.fConfig_HysAntiFrzNeg = SplitByte.fData;
#ifdef DBG_CAN_PARSER_CONFIG
            printf(" SET fConfig_HysAntiFrzNeg %0.2f",
                   stConfigData.fConfig_HysAntiFrzNeg);
            
#endif	
          }
          /******************FORWARD MESSAGE TO HCM**********************/	
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_HCM	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          fnCANMsg_Transmit(&stTx1Message, CAN1);		
          
          
          /*change32*/
          if(SOURCE_HMI == (uliCanID & SOURCE_ADDR_MASK))
          {
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            /****************************ABK*****************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_TABLET	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
//            printf("pstCANrxmsg->ExtId = %x \r\n",pstCANrxmsg->ExtId);
            ////						stWifi.ucSendFlag = SET;
            ////						stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
          }				
          
        }
        break;
        /* Recieve Configuration parameters for HCM inline base temperature setting from HMI */
      case CONFIG_INLINE_BASE_TEMP_SETTING:
        {
          
#ifdef DBG_CAN_PARSER_CONFIG      
          printf("CONFIG_INLINE_BASE_TEMP_SETTING\r\n");
#endif	
          
          /**************************DATA***********************************/
          for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            SplitByte.rgucData[ucLoopCnt-1] = pstCANrxmsg->Data[ucLoopCnt];
          }
          /****************************************************************/		
          if(BIT0_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK))
          {					
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_25;
            /*****************************/
            stConfigData.fConfig_InlineBaseTempSet = SplitByte.fData;
#ifdef DBG_CAN_PARSER_CONFIG
            
            printf(" SET fConfig_InlineBaseTempSet %0.2f",
                   stConfigData.fConfig_InlineBaseTempSet);
            
#endif	
          }
          if(BIT1_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT1_MASK))
          {					
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_26;
            /*****************************/
            stConfigData.fConfig_InlineBaseTempLTH = SplitByte.fData;
#ifdef DBG_CAN_PARSER_CONFIG
            printf(" SET fConfig_InlineBaseTempLTH %0.2f",
                   stConfigData.fConfig_InlineBaseTempLTH);
#endif	
          }
          if(BIT2_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT2_MASK))
          {					
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_27;
            /*****************************/
            stConfigData.fConfig_InlineBaseTempHTH = SplitByte.fData;
#ifdef DBG_CAN_PARSER_CONFIG
            
            printf(" SET fConfig_InlineBaseTempHTH %0.2f",
                   stConfigData.fConfig_InlineBaseTempHTH);
            
#endif	
          }
          if(BIT3_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT3_MASK))
          {					
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_28;
            /*****************************/
            stConfigData.fConfig_HysInlineBasePos = SplitByte.fData;
#ifdef DBG_CAN_PARSER_CONFIG
            
            printf(" SET fConfig_HysInlineBasePos %0.2f",
                   stConfigData.fConfig_HysInlineBasePos);
            
#endif	
          }
          if(BIT4_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT4_MASK))
          {					
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_29;
            /*****************************/
            stConfigData.fConfig_HysInlineBaseNeg = SplitByte.fData;
#ifdef DBG_CAN_PARSER_CONFIG
            
            printf(" SET fConfig_HysInlineBaseNeg %0.2f",
                   stConfigData.fConfig_HysInlineBaseNeg);
            
#endif	
          }
          /******************FORWARD MESSAGE TO HCM**********************/	
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_HCM	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          fnCANMsg_Transmit(&stTx1Message, CAN1);		
          
          
          /*change32*/
          if(SOURCE_HMI == (uliCanID & SOURCE_ADDR_MASK))
          {
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            /****************************ABK*****************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_TABLET	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
//            printf("pstCANrxmsg->ExtId = %x \r\n",pstCANrxmsg->ExtId);
            ////						stWifi.ucSendFlag = SET;
            ////						stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
          }				
        }
        break;
        /* Recieve Configuration parameters for HCM inline hard temperature setting from HMI */
      case CONFIG_INLINE_HARD_TEMP_SETTING:
        {
          
#ifdef DBG_CAN_PARSER_CONFIG      
          printf("CONFIG_INLINE_HARD_TEMP_SETTING\r\n");
#endif	
          /**************************DATA***********************************/
          for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            SplitByte.rgucData[ucLoopCnt-1] = pstCANrxmsg->Data[ucLoopCnt];
          }
          /****************************************************************/		
          if(BIT0_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK))
          {					
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_30;
            /*****************************/
            stConfigData.fConfig_InlineHardTempSet = SplitByte.fData;
#ifdef DBG_CAN_PARSER_CONFIG
            
            printf(" SET fConfig_InlineHardTempSet %0.2f",
                   stConfigData.fConfig_InlineHardTempSet);
            
#endif	
          }
          if(BIT1_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT1_MASK))
          {					
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_31;
            /*****************************/
            stConfigData.fConfig_InlineHardTempLTH = SplitByte.fData;
#ifdef DBG_CAN_PARSER_CONFIG
            
            printf(" SET fConfig_InlineHardTempLTH %0.2f",
                   stConfigData.fConfig_InlineHardTempLTH);
            
#endif	
          }
          if(BIT2_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT2_MASK))
          {					
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_32;
            /*****************************/
            stConfigData.fConfig_InlineHardTempHTH = SplitByte.fData;
#ifdef DBG_CAN_PARSER_CONFIG
            
            printf(" SET fConfig_InlineHardTempHTH %0.2f",
                   stConfigData.fConfig_InlineHardTempHTH);
            
#endif	
          }
          if(BIT3_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT3_MASK))
          {					
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_33;
            /*****************************/
            stConfigData.fConfig_InlineHysHardPos = SplitByte.fData;
#ifdef DBG_CAN_PARSER_CONFIG
            
            printf(" SET fConfig_InlineHysHardPos %0.2f",
                   stConfigData.fConfig_InlineHysHardPos);
            
#endif	
          }
          if(BIT4_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT4_MASK))
          {					
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_34;
            /*****************************/
            stConfigData.fConfig_InlineHysHardNeg = SplitByte.fData;
#ifdef DBG_CAN_PARSER_CONFIG
            
            printf(" SET fConfig_InlineHysHardNeg %0.2f",
                   stConfigData.fConfig_InlineHysHardNeg);
            
#endif	
          }
          /******************FORWARD MESSAGE TO HCM**********************/	
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_HCM	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          fnCANMsg_Transmit(&stTx1Message, CAN1);		
          
          
          
          /*change32*/
          if(SOURCE_HMI == (uliCanID & SOURCE_ADDR_MASK))
          {
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            /****************************ABK*****************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_TABLET	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
            printf("pstCANrxmsg->ExtId = %x \r\n",pstCANrxmsg->ExtId);
            ////						stWifi.ucSendFlag = SET;
            ////						stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
          }				
        }
        break;
        /* Recieve Configuration parameters for HCM base level setting from HMI */
      case CONFIG_BASE_LEVEL_SETTING:
        {
          
#ifdef DBG_CAN_PARSER_CONFIG      
          printf("CONFIG_BASE_LEVEL_SETTING\r\n");
#endif	
          /**************************DATA***********************************/
          for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            SplitByte.rgucData[ucLoopCnt-1] = pstCANrxmsg->Data[ucLoopCnt];
          }
          /****************************************************************/		
          if(BIT0_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK))
          {					
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_35;
            /*****************************/
            stConfigData.fConfig_BaseLiquidLevelLTH = SplitByte.fData;
#ifdef DBG_CAN_PARSER_CONFIG
            
            printf(" SET fConfig_BaseLiquidLevelLTH %0.2f",
                   stConfigData.fConfig_BaseLiquidLevelLTH);
            
#endif	
          }
          if(BIT1_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT1_MASK))
          {					
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_36;
            /*****************************/
            stConfigData.fConfig_BaseLiquidLevelHTH = SplitByte.fData;
#ifdef DBG_CAN_PARSER_CONFIG
            
            printf(" SET fConfig_BaseLiquidLevelHTH %0.2f",
                   stConfigData.fConfig_BaseLiquidLevelHTH);
            
#endif	
          }
          
          
          /******************FORWARD MESSAGE TO HCM**********************/	
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_HCM	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          fnCANMsg_Transmit(&stTx1Message, CAN1);		
          
          
          /*change32*/
          if(SOURCE_HMI == (uliCanID & SOURCE_ADDR_MASK))
          {
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            /****************************ABK*****************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_TABLET	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
//            printf("pstCANrxmsg->ExtId = %x \r\n",pstCANrxmsg->ExtId);
            ////						stWifi.ucSendFlag = SET;
            ////						stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
          }				
        }
        break;
        //* Recieve Configuration parameters for HCM hard level setting from HMI */
      case CONFIG_HARD_LEVEL_SETTING:
        {
          
#ifdef DBG_CAN_PARSER_CONFIG      
          printf("CONFIG_HARD_LEVEL_SETTING\r\n");
#endif
          /**************************DATA***********************************/
          for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            SplitByte.rgucData[ucLoopCnt-1] = pstCANrxmsg->Data[ucLoopCnt];
          }
          /****************************************************************/		
          if(BIT0_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK))
          {					
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_37;
            /*****************************/
            stConfigData.fConfig_HardLiquidLevelLTH = SplitByte.fData;
#ifdef DBG_CAN_PARSER_CONFIG
            
            printf(" SET fConfig_HardLiquidLevelLTH %0.2f",
                   stConfigData.fConfig_HardLiquidLevelLTH);
            
#endif	
          }
          if(BIT1_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT1_MASK))
          {					
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_38;
            /*****************************/
            stConfigData.fConfig_HardLiquidLevelHTH = SplitByte.fData;
#ifdef DBG_CAN_PARSER_CONFIG
            
            printf(" SET fConfig_HardLiquidLevelHTH %0.2f",
                   stConfigData.fConfig_HardLiquidLevelHTH);
            
#endif	
          }
          
          
          /******************FORWARD MESSAGE TO HCM**********************/	
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_HCM	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          fnCANMsg_Transmit(&stTx1Message, CAN1);		
          
          
          /*change32*/
          if(SOURCE_HMI == (uliCanID & SOURCE_ADDR_MASK))
          {
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            /****************************ABK*****************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_TABLET	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
//            printf("pstCANrxmsg->ExtId = %x \r\n",pstCANrxmsg->ExtId);
            ////						stWifi.ucSendFlag = SET;
            ////						stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
          }				
        }
        break;
        /* Recieve Configuration parameters for HCM Tank height from HMI */
      case CONFIG_TANK_HEIGHT:
        {
          
#ifdef DBG_CAN_PARSER_CONFIG      
          printf("CONFIG_TANK_HEIGHT\r\n");
#endif	
          /**************************DATA***********************************/
          for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            SplitByte.rgucData[ucLoopCnt-1] = pstCANrxmsg->Data[ucLoopCnt];
          }
          /****************************************************************/		
          if(BIT0_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK))
          {					
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_39;
            /*****************************/
            stConfigData.fConfig_BaseTankHeight = SplitByte.fData;
#ifdef DBG_CAN_PARSER_CONFIG
            
            printf(" SET fConfig_BaseTankHeight %0.2f",
                   stConfigData.fConfig_BaseTankHeight);
            
#endif	
          }
          if(BIT1_SET == (ucRec_Data[DATA_BYTE_ZERO] & BIT1_MASK))
          {					
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_40;
            /*****************************/
            stConfigData.fConfig_HardTankHeight = SplitByte.fData;
#ifdef DBG_CAN_PARSER_CONFIG
            
            printf(" SET fConfig_HardTankHeight %0.2f",
                   stConfigData.fConfig_HardTankHeight);
            
#endif	
          }
          
          
          /******************FORWARD MESSAGE TO HCM**********************/	
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_HCM	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          fnCANMsg_Transmit(&stTx1Message, CAN1);		
          
          
          /*change32*/
          if(SOURCE_HMI == (uliCanID & SOURCE_ADDR_MASK))
          {
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            /****************************ABK*****************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_TABLET	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
//            printf("pstCANrxmsg->ExtId = %x \r\n",pstCANrxmsg->ExtId);
            ////						stWifi.ucSendFlag = SET;
            ////						stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
          }				
        }
        break;
        /* Recieve Configuration parameters for HCM Liquid level sampling rate from HMI */
      case CONFIG_LL_SAMPLE_RATE:
        {
#ifdef DBG_CAN_PARSER_CONFIG      
          printf("CONFIG_LL_SAMPLE_RATE\r\n");
#endif	
          /**************************DATA***********************************/
          for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          /****************************************************************/		
          /* CONFIG PARAMETER NO */
          ucEEPROMCounter = CONFIG_PARA_41;
          /*****************************/
          
          stConfigData.uiConfig_LLSamplingRate = SplitByte.uiData;
#ifdef DBG_CAN_PARSER_CONFIG
          
          printf(" SET ucConfig_LLSamplingRate %d",
                 stConfigData.uiConfig_LLSamplingRate);
          
#endif	
          /******************FORWARD MESSAGE TO HCM**********************/	
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_HCM	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          fnCANMsg_Transmit(&stTx1Message, CAN1);		
          
          /*change32*/
          if(SOURCE_HMI == (uliCanID & SOURCE_ADDR_MASK))
          {
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            /****************************ABK*****************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_TABLET	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
//            printf("pstCANrxmsg->ExtId = %x \r\n",pstCANrxmsg->ExtId);
            ////						stWifi.ucSendFlag = SET;
            ////						stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
          }				
        }
        break;
        /* Recieve Configuration parameters for VCM pressure guage remapping from HMI */
      case CONFIG_PGAUGE_REMAP:
        {
#ifdef DBG_CAN_PARSER_CONFIG      
          printf("CONFIG_PGAUGE_REMAP\r\n");
#endif	
          /**************************DATA***********************************/
          /* CONFIG PARAMETER NO */
          ucEEPROMCounter = CONFIG_PARA_13;
          /*****************************/
          
          stConfigData.ucConfig_ConnPressureGuage1 = pstCANrxmsg->Data[DATA_BYTE_ZERO];
#ifdef DBG_CAN_PARSER_CONFIG
          printf(" SET ucConfig_ConnPressureGuage1 %d",
                 stConfigData.ucConfig_ConnPressureGuage1);
          
#endif	
          
          stConfigData.ucConfig_ConnPressureGuage2 = pstCANrxmsg->Data[DATA_BYTE_ONE];
#ifdef DBG_CAN_PARSER_CONFIG
          printf(" SET ucConfig_ConnPressureGuage2 %d",
                 stConfigData.ucConfig_ConnPressureGuage2);
          
#endif	
          
          
          stConfigData.ucConfig_ConnPressureGuage3 = pstCANrxmsg->Data[DATA_BYTE_TWO];
#ifdef DBG_CAN_PARSER_CONFIG
          
          printf(" SET ucConfig_ConnPressureGuage3 %d",
                 stConfigData.ucConfig_ConnPressureGuage3);
          
#endif	
          
          /****************************************************************/		
          
          
          
          /******************FORWARD MESSAGE TO VCM**********************/	
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_VCM	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          fnCANMsg_Transmit(&stTx1Message, CAN1);		
          
          /*change32*/
          if(SOURCE_HMI == (uliCanID & SOURCE_ADDR_MASK))
          {
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            /****************************ABK*****************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_TABLET	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
//            printf("pstCANrxmsg->ExtId = %x \r\n",pstCANrxmsg->ExtId);
            ////						stWifi.ucSendFlag = SET;
            ////						stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
          }				
        }
        break;
        /* Recieve Configuration parameters for VCM LT remapping from HMI */
      case CONFIG_LT_REMAP:
        {
          
#ifdef DBG_CAN_PARSER_CONFIG      
          printf("CONFIG_LT_REMAP\r\n");
#endif	
          /**************************DATA***********************************/
          /* CONFIG PARAMETER NO */
          ucEEPROMCounter = CONFIG_PARA_14;
          /*****************************/
          
          stConfigData.ucConfig_ConnLinearTransducer = pstCANrxmsg->Data[DATA_BYTE_ZERO];
#ifdef DBG_CAN_PARSER_CONFIG
          
          printf(" SET ucConfig_ConnLinearTransducer %d",
                 stConfigData.ucConfig_ConnLinearTransducer);
          
#endif	
          
          /******************FORWARD MESSAGE TO VCM**********************/	
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_VCM	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          fnCANMsg_Transmit(&stTx1Message, CAN1);	
          
          /*change32*/
          if(SOURCE_HMI == (uliCanID & SOURCE_ADDR_MASK))
          {
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            /****************************ABK*****************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_TABLET	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
//            printf("pstCANrxmsg->ExtId = %x \r\n",pstCANrxmsg->ExtId);
            ////						stWifi.ucSendFlag = SET;
            ////						stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
          }										
        }
        break;
        /* Recieve Configuration parameters for VCM EE07 remapping from HMI */
      case CONFIG_EE07_REMAP:
        {
#ifdef DBG_CAN_PARSER_CONFIG      
          printf("CONFIG_EE07_REMAP\r\n");
#endif	
          
          /**************************DATA***********************************/
          /* CONFIG PARAMETER NO */
          ucEEPROMCounter = CONFIG_PARA_15;
          /*****************************/
          
          stConfigData.ucConfig_ConnInsideEE07 = pstCANrxmsg->Data[DATA_BYTE_ZERO];
#ifdef DBG_CAN_PARSER_CONFIG
          
          printf(" SET ucConfig_ConnInsideEE07 %d",
                 stConfigData.ucConfig_ConnInsideEE07);
          
#endif	
          
          
          stConfigData.ucConfig_ConnOutsideEE07 = pstCANrxmsg->Data[DATA_BYTE_ONE];
#ifdef DBG_CAN_PARSER_CONFIG
          
          printf(" SET ucConfig_ConnOutsideEE07 %d",
                 stConfigData.ucConfig_ConnOutsideEE07);
          
#endif	
          
          
          /****************************************************************/		
          
          
          
          /******************FORWARD MESSAGE TO VCM**********************/	
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_VCM	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          fnCANMsg_Transmit(&stTx1Message, CAN1);		
          /*change32*/
          if(SOURCE_HMI == (uliCanID & SOURCE_ADDR_MASK))
          {
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            /****************************ABK*****************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_TABLET	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
//            printf("pstCANrxmsg->ExtId = %x \r\n",pstCANrxmsg->ExtId);
            ////						stWifi.ucSendFlag = SET;
            ////						stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
          }				
        }
        break;
        /* Recieve Configuration parameters for VCM Stall valve remapping from HMI */
      case CONFIG_STALL_VALVE_REMAP:
        {
          
#ifdef DBG_CAN_PARSER_CONFIG      
          printf("CONFIG_STALL_VALVE_REMAP\r\n");
#endif	
          /**************************DATA***********************************/
          /* CONFIG PARAMETER NO */
          ucEEPROMCounter = CONFIG_PARA_16;
          /*****************************/
          
          stConfigData.ucConfig_ConnStallvalve = pstCANrxmsg->Data[DATA_BYTE_ZERO];
#ifdef DBG_CAN_PARSER_CONFIG
          
          printf(" SET ucConfig_ConnStallvalve %d",
                 stConfigData.ucConfig_ConnStallvalve);
          
#endif	
					/*change53*/
          stConfigData.ucConfig_ConnRecirculationvalve = pstCANrxmsg->Data[DATA_BYTE_ONE];
#ifdef DBG_CAN_PARSER_CONFIG
          
          printf(" SET ucConfig_ConnRecirculationvalve %d",
                 stConfigData.ucConfig_ConnRecirculationvalve);
          
#endif	
          /******************FORWARD MESSAGE TO VCM**********************/	
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_VCM	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          fnCANMsg_Transmit(&stTx1Message, CAN1);		
          /*change32*/
          if(SOURCE_HMI == (uliCanID & SOURCE_ADDR_MASK))
          {
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            /****************************ABK*****************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_TABLET	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
//            printf("pstCANrxmsg->ExtId = %x \r\n",pstCANrxmsg->ExtId);
            ////						stWifi.ucSendFlag = SET;
            ////						stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
          }				
        }
        break;
        /* Recieve Configuration parameters for HCM Thermocouple remapping from HMI */
      case CONFIG_THERMOCOUPLE_REMAP:
        {
#ifdef DBG_CAN_PARSER_CONFIG      
          printf("CONFIG_THERMOCOUPLE_REMAP\r\n");
#endif	
          /**************************DATA***********************************/
          /* CONFIG PARAMETER NO */
          ucEEPROMCounter = CONFIG_PARA_42;
          /*****************************/
          
          stConfigData.ucConfig_ConnBaseinlinether = pstCANrxmsg->Data[DATA_BYTE_ZERO];
#ifdef DBG_CAN_PARSER_CONFIG
          printf(" SET ucConfig_ConnBaseinlinether %d",
                 stConfigData.ucConfig_ConnBaseinlinether);
          
#endif	
          
          
          stConfigData.ucConfig_ConnHardinlinether = pstCANrxmsg->Data[DATA_BYTE_ONE];
#ifdef DBG_CAN_PARSER_CONFIG
          
          printf(" SET ucConfig_ConnHardinlinether %d",
                 stConfigData.ucConfig_ConnHardinlinether);
          
#endif	
          
          
          stConfigData.ucConfig_ConnAntifreezether = pstCANrxmsg->Data[DATA_BYTE_TWO];
#ifdef DBG_CAN_PARSER_CONFIG
          printf(" SET ucConfig_ConnAntifreezether %d",
                 stConfigData.ucConfig_ConnAntifreezether);
          
#endif	
          
          stConfigData.ucConfig_ConnBasereturnlinether = pstCANrxmsg->Data[DATA_BYTE_THREE];
#ifdef DBG_CAN_PARSER_CONFIG
          
          printf(" SET ucConfig_ConnBasereturnlinether %d",
                 stConfigData.ucConfig_ConnBasereturnlinether);
          
#endif	
          /****************************************************************/		
          
          
          
          /******************FORWARD MESSAGE TO HCM**********************/	
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_HCM	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          fnCANMsg_Transmit(&stTx1Message, CAN1);		
          
          /*change32*/
          if(SOURCE_HMI == (uliCanID & SOURCE_ADDR_MASK))
          {
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            /****************************ABK*****************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_TABLET	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
//            printf("pstCANrxmsg->ExtId = %x \r\n",pstCANrxmsg->ExtId);
            ////						stWifi.ucSendFlag = SET;
            ////						stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
          }				
        }
        break;
        /* Recieve Configuration parameters for HCM SSR remapping from HMI */
      case CONFIG_SSR_REMAP:
        {
          
#ifdef DBG_CAN_PARSER_CONFIG      
          printf("CONFIG_SSR_REMAP\r\n");
#endif	
          
          /**************************DATA***********************************/
          /* CONFIG PARAMETER NO */
          ucEEPROMCounter = CONFIG_PARA_43;
          /*****************************/
          
          stConfigData.ucConfig_ConnBaseinlineheater = pstCANrxmsg->Data[DATA_BYTE_ZERO];
#ifdef DBG_CAN_PARSER_CONFIG
          
          printf(" SET ucConfig_ConnBaseinlineheater %d",
                 stConfigData.ucConfig_ConnBaseinlineheater);
          
#endif	
          
          
          stConfigData.ucConfig_ConnHardinlineheater = pstCANrxmsg->Data[DATA_BYTE_ONE];
#ifdef DBG_CAN_PARSER_CONFIG
          
          printf(" SET ucConfig_ConnHardinlineheater %d",
                 stConfigData.ucConfig_ConnHardinlineheater);
          
#endif	
          
          //					/* CONFIG PARAMETER NO */
          //								ucEEPROMCounter = CONFIG_PARA_1;
          //					/*****************************/
          
          stConfigData.ucConfig_ConnAntifreezeheater = pstCANrxmsg->Data[DATA_BYTE_TWO];
#ifdef DBG_CAN_PARSER_CONFIG
          printf(" SET ucConfig_ConnAntifreezeheater %d",
                 stConfigData.ucConfig_ConnAntifreezeheater);
          
#endif	
          //					/* CONFIG PARAMETER NO */
          //								ucEEPROMCounter = CONFIG_PARA_1;
          //					/*****************************/
          
          stConfigData.ucConfig_ConnHardheater = pstCANrxmsg->Data[DATA_BYTE_THREE];
#ifdef DBG_CAN_PARSER_CONFIG
          printf(" SET ucConfig_ConnHardheater %d",
                 stConfigData.ucConfig_ConnHardheater);
          
#endif	
          /****************************************************************/		
          
          
          
          /******************FORWARD MESSAGE TO HCM**********************/	
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_HCM	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          fnCANMsg_Transmit(&stTx1Message, CAN1);		
          /*change32*/
          if(SOURCE_HMI == (uliCanID & SOURCE_ADDR_MASK))
          {
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            /****************************ABK*****************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_TABLET	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
//            printf("pstCANrxmsg->ExtId = %x \r\n",pstCANrxmsg->ExtId);
            ////						stWifi.ucSendFlag = SET;
            ////						stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
          }				
        }
        break;
        /* Recieve Configuration parameters for HCM Liquid level remapping from HMI */
      case CONFIG_LL_SENSOR_REMAP:
        {
          
#ifdef DBG_CAN_PARSER_CONFIG      
          printf("CONFIG_LL_SENSOR_REMAP\r\n");
#endif	
          /**************************DATA***********************************/
          /* CONFIG PARAMETER NO */
          ucEEPROMCounter = CONFIG_PARA_44;
          /*****************************/
          
          stConfigData.ucConfig_ConnBasetankLL = pstCANrxmsg->Data[DATA_BYTE_ZERO];
#ifdef DBG_CAN_PARSER_CONFIG
          
          printf(" SET ucConfig_ConnBasetankLL %d",
                 stConfigData.ucConfig_ConnBasetankLL);
          
#endif	
          
          
          stConfigData.ucConfig_ConnHardtankLL = pstCANrxmsg->Data[DATA_BYTE_ONE];
#ifdef DBG_CAN_PARSER_CONFIG
          
          printf(" SET ucConfig_ConnHardtankLL %d",
                 stConfigData.ucConfig_ConnHardtankLL);
          
#endif	
          
          
          
          /****************************************************************/		
          
          
          
          /******************FORWARD MESSAGE TO HCM**********************/	
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_HCM	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          fnCANMsg_Transmit(&stTx1Message, CAN1);	
          /*change32*/
          if(SOURCE_HMI == (uliCanID & SOURCE_ADDR_MASK))
          {
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            /****************************ABK*****************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_TABLET	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
//            printf("pstCANrxmsg->ExtId = %x \r\n",pstCANrxmsg->ExtId);
            ////						stWifi.ucSendFlag = SET;
            ////						stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
          }				
          
        }
        break;
        /* Recieve Configuration parameters for OPMM Valves remapping from HMI */
      case CONFIG_OPMM_VALVE_REMAP:
        {
#ifdef DBG_CAN_PARSER_CONFIG      
          printf("CONFIG_OPMM_VALVE_REMAP\r\n");
#endif	
          /**************************DATA***********************************/
          /* CONFIG PARAMETER NO */
          ucEEPROMCounter = CONFIG_PARA_57;
          /*****************************/
          
          stConfigData.ucConfig_ConnSprayvalve = pstCANrxmsg->Data[DATA_BYTE_ZERO];
#ifdef DBG_CAN_PARSER_CONFIG
          
          printf(" SET ucConfig_ConnSprayvalve %d",
                 stConfigData.ucConfig_ConnSprayvalve);
          
#endif	
          
          
          stConfigData.ucConfig_ConnGunposition = pstCANrxmsg->Data[DATA_BYTE_ONE];
#ifdef DBG_CAN_PARSER_CONFIG
          
          printf(" SET ucConfig_ConnGunposition %d",
                 stConfigData.ucConfig_ConnGunposition);
          
#endif	
          
          
          stConfigData.ucConfig_ConnABvalve = pstCANrxmsg->Data[DATA_BYTE_TWO];
#ifdef DBG_CAN_PARSER_CONFIG
          
          printf(" SET ucConfig_ConnABvalve %d",
                 stConfigData.ucConfig_ConnABvalve);
          
#endif	
          
          //					/* CONFIG PARAMETER NO */
          //								ucEEPROMCounter = CONFIG_PARA_1;
          //					/*****************************/
          
          stConfigData.ucConfig_ConnSolventvalve = pstCANrxmsg->Data[DATA_BYTE_THREE];
#ifdef DBG_CAN_PARSER_CONFIG
          
          printf(" SET ucConfig_ConnSolventvalve %d",
                 stConfigData.ucConfig_ConnSolventvalve);
          
#endif	
          
          //					/* CONFIG PARAMETER NO */
          //								ucEEPROMCounter = CONFIG_PARA_1;
          //					/*****************************/
          //						
          stConfigData.ucConfig_ConnForwardvalve = pstCANrxmsg->Data[DATA_BYTE_FOUR];
#ifdef DBG_CAN_PARSER_CONFIG
          
          printf(" SET ucConfig_ConnForwardvalve %d",
                 stConfigData.ucConfig_ConnForwardvalve);
          
#endif	
          //					/* CONFIG PARAMETER NO */
          //								ucEEPROMCounter = CONFIG_PARA_1;
          //					/*****************************/
          //						
          stConfigData.ucConfig_ConnBackwardvalve = pstCANrxmsg->Data[DATA_BYTE_FIVE];
#ifdef DBG_CAN_PARSER_CONFIG
          
          printf(" SET ucConfig_ConnBackwardvalve %d",
                 stConfigData.ucConfig_ConnBackwardvalve);
          
#endif	
          /****************************************************************/		
          
          
          
          /******************FORWARD MESSAGE TO OPMM**********************/	
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_OPMM	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          fnCANMsg_Transmit(&stTx1Message, CAN1);		
          
          /*change32*/
          if(SOURCE_HMI == (uliCanID & SOURCE_ADDR_MASK))
          {
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            /****************************ABK*****************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_TABLET	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
//            printf("pstCANrxmsg->ExtId = %x \r\n",pstCANrxmsg->ExtId);
            ////						stWifi.ucSendFlag = SET;
            ////						stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
          }				
        }
        break;
        /* Recieve Configuration parameters for VCM to bypass peripherals from HMI */
      case CONFIG_VCM_PERIPHRAL_BYPASS:
        {
          
#ifdef DBG_CAN_PARSER_CONFIG      
          printf("CONFIG_VCM_PERIPHRAL_BYPASS\r\n");
#endif	
          /*************************************************************/		
          
          /* CONFIG PARAMETER NO */
          ucEEPROMCounter = CONFIG_PARA_17;
          /*****************************/
          stConfigData.ucConfig_VCMBypass = pstCANrxmsg->Data[DATA_BYTE_ZERO];;
#ifdef DBG_CAN_PARSER_CONFIG
          printf(" SET ucConfig_VCMBypass %d",
                 stConfigData.ucConfig_VCMBypass);
          
#endif	
          /******************FORWARD MESSAGE TO VCM**********************/	
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_VCM	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          fnCANMsg_Transmit(&stTx1Message, CAN1);	
          
          /*change32*/
          if(SOURCE_HMI == (uliCanID & SOURCE_ADDR_MASK))
          {
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            /****************************ABK*****************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_TABLET	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
//            printf("pstCANrxmsg->ExtId = %x \r\n",pstCANrxmsg->ExtId);
            ////						stWifi.ucSendFlag = SET;
            ////						stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
          }						
          
        }
        break;
        /* Recieve Configuration parameters for HCM to bypass peripherals from HMI */
      case CONFIG_HCM_PERIPHRAL_BYPASS:
        {
          	#ifdef DBG_CAN_PARSER_CONFIG      
          printf("CONFIG_HCM_PERIPHRAL_BYPASS\r\n");
          	#endif	
          
          /**************************DATA***********************************/
          for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          /****************************************************************/		
          /* CONFIG PARAMETER NO */
          ucEEPROMCounter = CONFIG_PARA_45;
          /*****************************/
          
          stConfigData.uiConfig_HCMBypass = SplitByte.uiData;
#ifdef DBG_CAN_PARSER_CONFIG
          printf(" SET uiConfig_HCMBypass %d",
                 stConfigData.uiConfig_HCMBypass);
          
#endif	
          /******************FORWARD MESSAGE TO HCM*********************/	
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_HCM	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          fnCANMsg_Transmit(&stTx1Message, CAN1);		
//          printf("uliCanID = %x \r\n",uliCanID);
          /*change32*/
          if(SOURCE_HMI == (uliCanID & SOURCE_ADDR_MASK))
          {
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            /****************************ABK*****************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_TABLET	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
//            printf("pstCANrxmsg->ExtId = %x \r\n",pstCANrxmsg->ExtId);
            ////						stWifi.ucSendFlag = SET;
            ////						stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
          }			
        }
        break;
        /* Recieve Configuration parameters for SCU to bypass peripherals from HMI */
      case CONFIG_SCU_PERIPHRAL_BYPASS:
        {
          
#ifdef DBG_CAN_PARSER_CONFIG      
          printf("CONFIG_SCU_PERIPHRAL_BYPASS\r\n");
#endif	
          /* CONFIG PARAMETER NO */
          ucEEPROMCounter = CONFIG_PARA_85;
          /*****************************/
          stConfigData.ucConfig_SCUBypass = pstCANrxmsg->Data[DATA_BYTE_ZERO];
#ifdef DBG_CAN_PARSER_CONFIG
          
          printf(" SET stConfigData.ucConfig_SCUBypass %d",
                 stConfigData.ucConfig_SCUBypass);
          
#endif	
          /*change32*/
          if(SOURCE_HMI == (uliCanID & SOURCE_ADDR_MASK))
          {
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            /****************************ABK*****************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_TABLET	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
//            printf("pstCANrxmsg->ExtId = %x \r\n",pstCANrxmsg->ExtId);
            ////						stWifi.ucSendFlag = SET;
            ////						stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
          }			
        }
        break;
        /* Recieve Configuration parameters for OPMM to bypass peripherals from HMI */
      case CONFIG_OPMM_PERIPHRAL_BYPASS:
        {
          
#ifdef DBG_CAN_PARSER_CONFIG      
          printf("CONFIG_OPMM_PERIPHRAL_BYPASS\r\n");
#endif	
          /**************************DATA***********************************/
          for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          /****************************************************************/		
          
          /* CONFIG PARAMETER NO */
          ucEEPROMCounter = CONFIG_PARA_58;
          /*****************************/
          stConfigData.uiConfig_OPMMBypass = SplitByte.uiData;
#ifdef DBG_CAN_PARSER_CONFIG
          
          printf(" SET uiConfig_OPMMBypass %d",
                 stConfigData.uiConfig_OPMMBypass);
          
#endif	
          /******************FORWARD MESSAGE TO OPMM**********************/	
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_OPMM	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          fnCANMsg_Transmit(&stTx1Message, CAN1);		
          /*change32*/
          if(SOURCE_HMI == (uliCanID & SOURCE_ADDR_MASK))
          {
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            /****************************ABK*****************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_TABLET	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
//            printf("pstCANrxmsg->ExtId = %x \r\n",pstCANrxmsg->ExtId);
            ////						stWifi.ucSendFlag = SET;
            ////						stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
          }			
        }
        break;
        /* Recieve Configuration parameters for OPMM Valves remapping from HMI */
      case CONFIG_OPMM_NON_CONTACT_REMAP:
        {
#ifdef DBG_CAN_PARSER_CONFIG      
          printf("CONFIG_OPMM_NON_CONTACT_REMAP\r\n");
#endif	
          /**************************DATA***********************************/
          /* CONFIG PARAMETER NO */
          ucEEPROMCounter = CONFIG_PARA_89;
          /*****************************/
          
          stConfigData.ucConfig_ConnNonContactat12 = pstCANrxmsg->Data[DATA_BYTE_ZERO];
#ifdef DBG_CAN_PARSER_CONFIG
          
          printf(" SET ucConfig_ConnNonContactat12 %d",
                 stConfigData.ucConfig_ConnNonContactat12);
          
#endif	
          
          
          stConfigData.ucConfig_ConnNonContactat3 = pstCANrxmsg->Data[DATA_BYTE_ONE];
#ifdef DBG_CAN_PARSER_CONFIG
          
          printf(" SET ucConfig_ConnNonContactat3 %d",
                 stConfigData.ucConfig_ConnNonContactat3);
          
#endif	
          
          
          stConfigData.ucConfig_ConnNonContactat6 = pstCANrxmsg->Data[DATA_BYTE_TWO];
#ifdef DBG_CAN_PARSER_CONFIG
          
          printf(" SET ucConfig_ConnNonContactat6 %d",
                 stConfigData.ucConfig_ConnNonContactat6);
          
#endif	
          
          //					/* CONFIG PARAMETER NO */
          //								ucEEPROMCounter = CONFIG_PARA_1;
          //					/*****************************/
          
          stConfigData.ucConfig_ConnNonContactat9 = pstCANrxmsg->Data[DATA_BYTE_THREE];
#ifdef DBG_CAN_PARSER_CONFIG
          
          printf(" SET ucConfig_ConnNonContactat9 %d",
                 stConfigData.ucConfig_ConnNonContactat9);
          
#endif	
          
          /****************************************************************/		
          
          
          
          /******************FORWARD MESSAGE TO OPMM**********************/	
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_OPMM	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          fnCANMsg_Transmit(&stTx1Message, CAN1);		
          /*change32*/
          if(SOURCE_HMI == (uliCanID & SOURCE_ADDR_MASK))
          {
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            /****************************ABK*****************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_TABLET	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
            printf("pstCANrxmsg->ExtId = %x \r\n",pstCANrxmsg->ExtId);
            ////						stWifi.ucSendFlag = SET;
            ////						stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
          }			
        }
        break;
        /* Recieve Configuration parameters for OPMM to bypass peripherals from HMI */
      case CONFIG_TEMP_PRESS_ACHIEVE_BYPASS:
        {
          
#ifdef DBG_CAN_PARSER_CONFIG      
          printf("CONFIG_TEMP_PRESS_ACHIEVE_BYPASS\r\n");
#endif	
          
          /* CONFIG PARAMETER NO */
          ucEEPROMCounter = CONFIG_PARA_88;
          /*****************************/
          stConfigData.ucConfig_Temp_Press_Achieve_Bypass = pstCANrxmsg->Data[DATA_BYTE_ZERO];
#ifdef DBG_CAN_PARSER_CONFIG
          
          printf(" SET ucConfig_Temp_Press_Achieve_Bypass %d",
                 stConfigData.ucConfig_Temp_Press_Achieve_Bypass);
          
#endif	
          /*change32*/
          if(SOURCE_HMI == (uliCanID & SOURCE_ADDR_MASK))
          {
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            /****************************ABK*****************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_TABLET	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
//            printf("pstCANrxmsg->ExtId = %x \r\n",pstCANrxmsg->ExtId);
            ////						stWifi.ucSendFlag = SET;
            ////						stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
          }			
          
        }
        break;
      case CONFIG_OPMM_BUZZER_VOLUME:
        {
          
#ifdef DBG_CAN_PARSER_CONFIG      
          printf("CONFIG_OPMM_BUZZER_VOLUME\r\n");
#endif	
          /* CONFIG PARAMETER NO */
          ucEEPROMCounter = CONFIG_PARA_94;
          /*****************************/
          
          stConfigData.ucConfig_BuzzerVolume = pstCANrxmsg->Data[0];
#ifdef DBG_CAN_PARSER_CONFIG
          printf("CONFIG_OPMM_BUZZER_VOLUME: %d\r\n",
                 stConfigData.ucConfig_BuzzerVolume);
#endif
          /******************FORWARD MESSAGE TO OPMM**********************/	
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_OPMM	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          fnCANMsg_Transmit(&stTx1Message, CAN1);		
          /*change32*/
          if(SOURCE_HMI == (uliCanID & SOURCE_ADDR_MASK))
          {
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            /****************************ABK*****************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_TABLET	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
//            printf("pstCANrxmsg->ExtId = %x \r\n",pstCANrxmsg->ExtId);
            ////						stWifi.ucSendFlag = SET;
            ////						stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
          }			
        }
        break;
      case CONFIG_SIGNATURE_BYTE:
        {
          
#ifdef DBG_CAN_PARSER_CONFIG      
          printf("CONFIG_SIGNATURE_BYTE\r\n");
#endif	
          /* CONFIG PARAMETER NO */
          ucEEPROMCounter = CONFIG_PARA_95;
          
          /**************************DATA***********************************/
          for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          /****************************************************************/		
          stConfigData.uiConfig_Signature = SplitByte.uiData;
#ifdef DBG_CAN_PARSER_CONFIG
          
          printf(" SET uiSignature %d",
                 stConfigData.uiConfig_Signature);
          
#endif	
          /*change32*/
          if(SOURCE_HMI == (uliCanID & SOURCE_ADDR_MASK))
          {
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            /****************************ABK*****************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_TABLET	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
            printf("pstCANrxmsg->ExtId = %x \r\n",pstCANrxmsg->ExtId);
            ////						stWifi.ucSendFlag = SET;
            ////						stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
          }				
          
        }
        break;
        /********************************change8***********************************/
        /* Recieve Configuration parameters for Prime time config from HMI */
      case CONFIG_OPMM_BATT_KNEE_VOLT:
        {
#ifdef DBG_CAN_PARSER_CONFIG      
          printf("CONFIG_OPMM_BATT_KNEE_VOLT\r\n");
#endif	
          /**************************DATA***********************************/
          for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          /****************************************************************/		
          /* CONFIG PARAMETER NO */
          ucEEPROMCounter = CONFIG_PARA_98;
          /*****************************/
          
          stConfigData.fConfig_BatteryKneeVoltage = SplitByte.fData;
#ifdef DBG_CAN_PARSER_CONFIG
          printf(" SET fConfig_BatteryKneeVoltage %0.2f",
                 stConfigData.fConfig_BatteryKneeVoltage);
#endif					
          /******************FORWARD MESSAGE TO OPMM**********************/	
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_OPMM	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          fnCANMsg_Transmit(&stTx1Message, CAN1);
          
          /*change32*/
          if(SOURCE_HMI == (uliCanID & SOURCE_ADDR_MASK))
          {
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            /****************************ABK*****************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_TABLET	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
//            printf("pstCANrxmsg->ExtId = %x \r\n",pstCANrxmsg->ExtId);
            ////						stWifi.ucSendFlag = SET;
            ////						stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
          }				
          
        }
        break;
        /* Recieve Configuration parameters for Prime time config from HMI */
      case CONFIG_SCU_STALL_COMMAND_TIME:
        {
#ifdef DBG_CAN_PARSER_CONFIG      
          printf("CONFIG_SCU_STALL_COMMAND_TIME\r\n");
#endif	
          /**************************DATA***********************************/
          for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          /****************************************************************/		
          /* CONFIG PARAMETER NO */
          ucEEPROMCounter = CONFIG_PARA_96;
          /*****************************/
          
          stConfigData.fConfig_TimeBeforeSendingStallcommand = SplitByte.fData;
#ifdef DBG_CAN_PARSER_CONFIG
          printf(" SET fConfig_TimeBeforeSendingStallcommand %0.2f",
                 stConfigData.fConfig_TimeBeforeSendingStallcommand);
#endif					
          /*change32*/
          if(SOURCE_HMI == (uliCanID & SOURCE_ADDR_MASK))
          {
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            /****************************ABK*****************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_TABLET	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
//            printf("pstCANrxmsg->ExtId = %x \r\n",pstCANrxmsg->ExtId);
            ////						stWifi.ucSendFlag = SET;
            ////						stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
          }				
        }
        break;
        /* Recieve Configuration parameters for Prime time config from HMI */
      case CONFIG_OPMM_STOP_HLS_TIME:
        {
#ifdef DBG_CAN_PARSER_CONFIG      
          printf("CONFIG_OPMM_STOP_HLS_TIME\r\n");
#endif	
          /**************************DATA***********************************/
          for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          /****************************************************************/		
          /* CONFIG PARAMETER NO */
          ucEEPROMCounter = CONFIG_PARA_97;
          /*****************************/
          
          stConfigData.fConfig_TimeStopOPMMcommand = SplitByte.fData;
#ifdef DBG_CAN_PARSER_CONFIG
          printf(" SET fConfig_TimeStopOPMMcommand %0.2f",
                 stConfigData.fConfig_TimeStopOPMMcommand);
#endif					
          /******************FORWARD MESSAGE TO OPMM**********************/	
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_OPMM	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          fnCANMsg_Transmit(&stTx1Message, CAN1);
          /*change32*/
          if(SOURCE_HMI == (uliCanID & SOURCE_ADDR_MASK))
          {
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            /****************************ABK*****************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_TABLET	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
//            printf("pstCANrxmsg->ExtId = %x \r\n",pstCANrxmsg->ExtId);
            ////						stWifi.ucSendFlag = SET;
            ////						stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
          }				
        }
        break;
        /*******************************change10*************************************/
        /* Recieve Configuration parameters for DCLS HLS time config from HMI */
      case CONFIG_VCM_STALL_RANGE:
        {
          
#ifdef DBG_CAN_PARSER_CONFIG      
          printf("CONFIG_VCM_STALL_RANGE\r\n");
#endif	
          if(BIT0_SET == (pstCANrxmsg->Data[DATA_BYTE_ZERO] & BIT0_MASK))
          {					
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_99;
            /*****************************/
            stConfigData.ucConfig_UpstrokeUpLimit = pstCANrxmsg->Data[DATA_BYTE_ONE];
#ifdef DBG_CAN_PARSER_CONFIG
            printf(" SET ucConfig_UpstrokeUpLimit %0.2f",
                   stConfigData.ucConfig_UpstrokeUpLimit);
#endif	
          }
          if(BIT1_SET == (pstCANrxmsg->Data[DATA_BYTE_ZERO] & BIT1_MASK))
          {					
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_100;
            /*****************************/
            stConfigData.ucConfig_UpstrokeMinRange = pstCANrxmsg->Data[DATA_BYTE_ONE];
#ifdef DBG_CAN_PARSER_CONFIG
            printf(" SET ucConfig_UpstrokeMinRange %0.2f",
                   stConfigData.ucConfig_UpstrokeMinRange);
#endif	
          }
          if(BIT2_SET == (pstCANrxmsg->Data[DATA_BYTE_ZERO] & BIT2_MASK))
          {					
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_101;
            /*****************************/
            stConfigData.ucConfig_UpstrokeMaxRange = pstCANrxmsg->Data[DATA_BYTE_ONE];
#ifdef DBG_CAN_PARSER_CONFIG
            printf(" SET ucConfig_UpstrokeMaxRange %0.2f",
                   stConfigData.ucConfig_UpstrokeMaxRange);
#endif	
          }
          if(BIT3_SET == (pstCANrxmsg->Data[DATA_BYTE_ZERO] & BIT3_MASK))
          {					
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_102;
            /*****************************/
            stConfigData.ucConfig_DownstrokeUpLimit = pstCANrxmsg->Data[DATA_BYTE_ONE];
#ifdef DBG_CAN_PARSER_CONFIG
            printf(" SET ucConfig_DownstrokeUpLimit %0.2f",
                   stConfigData.ucConfig_DownstrokeUpLimit);
#endif	
          }
          if(BIT4_SET == (pstCANrxmsg->Data[DATA_BYTE_ZERO] & BIT4_MASK))
          {					
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_103;
            /*****************************/
            stConfigData.ucConfig_DownstrokeMinRange = pstCANrxmsg->Data[DATA_BYTE_ONE];
#ifdef DBG_CAN_PARSER_CONFIG
            printf(" SET ucConfig_DownstrokeMinRange %0.2f",
                   stConfigData.ucConfig_DownstrokeMinRange);
#endif	
          }
          if(BIT5_SET == (pstCANrxmsg->Data[DATA_BYTE_ZERO] & BIT5_MASK))
          {					
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_104;
            /*****************************/
            stConfigData.ucConfig_DownstrokeMaxRange = pstCANrxmsg->Data[DATA_BYTE_ONE];
#ifdef DBG_CAN_PARSER_CONFIG
            printf(" SET ucConfig_DownstrokeMaxRange %0.2f",
                   stConfigData.ucConfig_DownstrokeMaxRange);
#endif	
          }
          
          /******************FORWARD MESSAGE TO OPMM**********************/	
          pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                (DEST_VCM	 | SOURCE_SCU));
          stTx1Message.ExtId = pstCANrxmsg->ExtId; 
          stTx1Message.DLC = pstCANrxmsg->DLC; 
          for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          fnCANMsg_Transmit(&stTx1Message, CAN1);		
          /*change32*/
          if(SOURCE_HMI == (uliCanID & SOURCE_ADDR_MASK))
          {
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            /****************************ABK*****************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_TABLET	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
//            printf("pstCANrxmsg->ExtId = %x \r\n",pstCANrxmsg->ExtId);
            ////						stWifi.ucSendFlag = SET;
            ////						stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
          }				
        }
        break;
        /*change21*/
        /* Recieve Configuration parameters for data logging parameter from HMI */
      case CONFIG_IDLE_DATA_LOG_PERIOD:
        {
          
#ifdef DBG_CAN_PARSER_CONFIG      
          printf("CONFIG_IDLE_DATA_LOG_PERIOD\r\n");
#endif
          /**************************DATA***********************************/
          for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          /****************************************************************/		
          /* CONFIG PARAMETER NO */
          ucEEPROMCounter = CONFIG_PARA_114;
          /*****************************/
          
          stConfigData.fConfig_IdleTimePeriodicLog = SplitByte.fData;
#ifdef DBG_CAN_PARSER_CONFIG
          
          printf(" SET fConfig_IdleTimePeriodicLog %0.2f",
                 stConfigData.fConfig_IdleTimePeriodicLog);
          
#endif			
          
          /*change32*/
          if(SOURCE_HMI == (uliCanID & SOURCE_ADDR_MASK))
          {
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            /****************************ABK*****************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_TABLET	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
//            printf("pstCANrxmsg->ExtId = %x \r\n",pstCANrxmsg->ExtId);
            ////						stWifi.ucSendFlag = SET;
            ////						stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
          }										
        }
        break;
        /* Recieve Configuration parameters for Job name parameter from HMI */
      case CONFIG_JOB_NAME:
        {
          
          #ifdef DBG_CAN_PARSER_CONFIG      
          printf("CONFIG_JOB_NAME\r\n");
         	#endif	
          if(FRAME_1 == pstCANrxmsg->Data[0])
          {
            /*change25*/
            /*before updating the name save previous to flash*/
           /* memset(jobname,'\0',FLASH_RX_TX_BUFFER_SIZE);
            strcat((char *)jobname,(char *)stConfigData.rgucConfig_JobName1);
            strcat((char *)jobname,(char *)stConfigData.rgucConfig_JobName2);
            strcat((char *)jobname,(char *)stConfigData.rgucConfig_JobName3);	
            fnStoreJobName(ucJobNameID,&jobname[0]);*/
						/*change78*/						
						memset(jobnamepre,'\0',FLASH_RX_TX_BUFFER_SIZE);
            strcat((char *)jobnamepre,(char *)stConfigData.rgucConfig_JobName1);
            strcat((char *)jobnamepre,(char *)stConfigData.rgucConfig_JobName2);
            strcat((char *)jobnamepre,(char *)stConfigData.rgucConfig_JobName3);	
						StatusFlag.stStatusFlag.bConfigJobName = SET;
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_111;
            /*****************************/
            
            for(ucLoopCnt = 1; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              stConfigData.rgucConfig_JobName1[ucLoopCnt - 1] = 
                pstCANrxmsg->Data[ucLoopCnt];
            }
            stConfigData.rgucConfig_JobName1[ucLoopCnt - 1] = NULL;
            
#ifdef DBG_CAN_PARSER_CONFIG
            printf("%s\r\n",stConfigData.rgucConfig_JobName1);
#endif
            
            /*change23*/
        //    ucJobNameID++;
            /*change26*/ 
            /* Check for Supervisor ID and Job ID Range to reset Flash*/
          //  fnCheck_ID();
#ifdef DBG_CAN_PARSER_CONFIG
            printf("%d\r\n",ucJobNameID);
#endif
          }
          if(FRAME_2 == pstCANrxmsg->Data[0])
          {
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_112;
            /*****************************/
            
            
            for(ucLoopCnt = 1; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              stConfigData.rgucConfig_JobName2[ucLoopCnt - 1] = 
                pstCANrxmsg->Data[ucLoopCnt];
            }
            stConfigData.rgucConfig_JobName2[ucLoopCnt - 1] = NULL;
#ifdef DBG_CAN_PARSER_CONFIG
            printf("%s\r\n", stConfigData.rgucConfig_JobName2);
#endif
          }
          if(FRAME_3 == pstCANrxmsg->Data[0])
          {
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_113;
            /*****************************/
            
            for(ucLoopCnt = 1; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              stConfigData.rgucConfig_JobName3[ucLoopCnt - 1] = 
                pstCANrxmsg->Data[ucLoopCnt];
            }
            stConfigData.rgucConfig_JobName3[ucLoopCnt - 1] = NULL;
#ifdef DBG_CAN_PARSER_CONFIG
            printf("%s\r\n",stConfigData.rgucConfig_JobName3);
#endif
            
          }
          /*change32*/
          if(SOURCE_HMI == (uliCanID & SOURCE_ADDR_MASK))
          {
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            /****************************ABK*****************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_TABLET	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
//            printf("pstCANrxmsg->ExtId = %x \r\n",pstCANrxmsg->ExtId);
            ////						stWifi.ucSendFlag = SET;
            ////						stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
          }				
        }
        break;
        /* Recieve Configuration parameters for data logging parameter from HMI */
      case CONFIG_PIPE_DIAMETER:
        {
          
#ifdef DBG_CAN_PARSER_CONFIG      
          printf("CONFIG_PIPE_DIAMETER\r\n");
#endif
          /**************************DATA***********************************/
          for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
          {
            SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
          }
          /****************************************************************/		
          /* CONFIG PARAMETER NO */
          ucEEPROMCounter = CONFIG_PARA_115;
          /*****************************/
          
          stConfigData.fConfig_PipeDiameter = SplitByte.fData;
#ifdef DBG_CAN_PARSER_CONFIG
          
          printf(" SET fConfig_PipeDiameter %0.2f",
                 stConfigData.fConfig_PipeDiameter);
          
#endif			
          
          /*change32*/
          if(SOURCE_HMI == (uliCanID & SOURCE_ADDR_MASK))
          {
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            /****************************ABK*****************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_TABLET	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
//            printf("pstCANrxmsg->ExtId = %x \r\n",pstCANrxmsg->ExtId);
            ////						stWifi.ucSendFlag = SET;
            ////						stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
          }										
        }
        break;
        /*change28*/
        /* Recieve Configuration parameters ETH WIFI IP SUBNET MASK GATEWAY AND PORT NUMBER from HMI */
      case CONFIG_ETH_WIFI_IP_SN_GW_PN:
        {
          
#ifdef DBG_CAN_PARSER_CONFIG      
          printf("CONFIG_ETH_WIFI_IP_SN_GW_PN\r\n");
#endif
          
          /************************ETHERNET CONFIG PARAMETERS****************************/		
          if(BIT0_SET == (pstCANrxmsg->Data[DATA_BYTE_ZERO] & BIT0_MASK))
          {	
            if(BIT0_SET == (pstCANrxmsg->Data[DATA_BYTE_ONE] & BIT0_MASK))
            {	
#ifdef DBG_CAN_PARSER_CONFIG      
              printf("CONFIG_ETH_IP_ADDRESS\r\n");
#endif
              sprintf((char *)stEthernetIP.rgucIP_Addr,"%d.%d.%d.%d",pstCANrxmsg->Data[DATA_BYTE_TWO],\
                pstCANrxmsg->Data[DATA_BYTE_THREE],pstCANrxmsg->Data[DATA_BYTE_FOUR],\
                  pstCANrxmsg->Data[DATA_BYTE_FIVE]);
              
              /* CONFIG PARAMETER NO */
              ucEEPROMCounter = CONFIG_PARA_90;
              /*****************************/
            }
            else if(BIT1_SET == (pstCANrxmsg->Data[DATA_BYTE_ONE] & BIT1_MASK))
            {	
#ifdef DBG_CAN_PARSER_CONFIG      
              printf("CONFIG_ETH_SUBNET_MASK_ADDRESS\r\n");
#endif
              
              sprintf((char *)stEthernetIP.rgucSubnet_Addr,"%d.%d.%d.%d",pstCANrxmsg->Data[DATA_BYTE_TWO],\
                pstCANrxmsg->Data[DATA_BYTE_THREE],pstCANrxmsg->Data[DATA_BYTE_FOUR],\
                  pstCANrxmsg->Data[DATA_BYTE_FIVE]);
              /* CONFIG PARAMETER NO */
              ucEEPROMCounter = CONFIG_PARA_91;
              /*****************************/
            }
            else if(BIT2_SET == (pstCANrxmsg->Data[DATA_BYTE_ONE] & BIT2_MASK))
            {	
#ifdef DBG_CAN_PARSER_CONFIG      
              printf("CONFIG_ETH_GATEWAY_ADDRESS\r\n");
#endif
              sprintf((char *)stEthernetIP.rgucGateway_Addr,"%d.%d.%d.%d",pstCANrxmsg->Data[DATA_BYTE_TWO],\
                pstCANrxmsg->Data[DATA_BYTE_THREE],pstCANrxmsg->Data[DATA_BYTE_FOUR],\
                  pstCANrxmsg->Data[DATA_BYTE_FIVE]);
              /* CONFIG PARAMETER NO */
              ucEEPROMCounter = CONFIG_PARA_92;
              /*****************************/
            }
            else if(BIT3_SET == (pstCANrxmsg->Data[DATA_BYTE_ONE] & BIT3_MASK))
            {	
#ifdef DBG_CAN_PARSER_CONFIG      
              printf("CONFIG_ETH_PORT_NUMBER\r\n");
#endif
              SplitByte.rgucData[DATA_BYTE_ZERO]= pstCANrxmsg->Data[DATA_BYTE_SIX];
              SplitByte.rgucData[DATA_BYTE_ONE] = pstCANrxmsg->Data[DATA_BYTE_SEVEN];
              stEthernetIP.uiPort = SplitByte.uiData;
              /* CONFIG PARAMETER NO */
              ucEEPROMCounter = CONFIG_PARA_93;
              /*****************************/
            }
          }
          /************************WIFI CONFIG PARAMETERS****************************/		
          else if(BIT1_SET == (pstCANrxmsg->Data[DATA_BYTE_ZERO] & BIT1_MASK))
          {	
            if(BIT0_SET == (pstCANrxmsg->Data[DATA_BYTE_ONE] & BIT0_MASK))
            {	
#ifdef DBG_CAN_PARSER_CONFIG      
              printf("CONFIG_WIFI_IP_ADDRESS\r\n");
#endif
              
              sprintf((char *)stWifiIP.rgucIP_Addr,"%d.%d.%d.%d",pstCANrxmsg->Data[DATA_BYTE_TWO],\
                pstCANrxmsg->Data[DATA_BYTE_THREE],pstCANrxmsg->Data[DATA_BYTE_FOUR],\
                  pstCANrxmsg->Data[DATA_BYTE_FIVE]);
             		
							
              /* CONFIG PARAMETER NO */
              ucEEPROMCounter = CONFIG_PARA_105;
              /*****************************/
            }
            else if(BIT1_SET == (pstCANrxmsg->Data[DATA_BYTE_ONE] & BIT1_MASK))
            {	
#ifdef DBG_CAN_PARSER_CONFIG      
              printf("CONFIG_WIFI_SUBNET_MASK_ADDRESS\r\n");
#endif
              
              sprintf((char *)stWifiIP.rgucSubnet_Addr,"%d.%d.%d.%d",pstCANrxmsg->Data[DATA_BYTE_TWO],\
                pstCANrxmsg->Data[DATA_BYTE_THREE],pstCANrxmsg->Data[DATA_BYTE_FOUR],\
                  pstCANrxmsg->Data[DATA_BYTE_FIVE]);
              
								
							
              /* CONFIG PARAMETER NO */
              ucEEPROMCounter = CONFIG_PARA_106;
              /*****************************/
            }
            else if(BIT2_SET == (pstCANrxmsg->Data[DATA_BYTE_ONE] & BIT2_MASK))
            {	
#ifdef DBG_CAN_PARSER_CONFIG      
              printf("CONFIG_WIFI_GATEWAY_ADDRESS\r\n");
#endif
              
              sprintf((char *)stWifiIP.rgucGateway_Addr,"%d.%d.%d.%d",pstCANrxmsg->Data[DATA_BYTE_TWO],\
                pstCANrxmsg->Data[DATA_BYTE_THREE],pstCANrxmsg->Data[DATA_BYTE_FOUR],\
                  pstCANrxmsg->Data[DATA_BYTE_FIVE]);
              
              
              /* CONFIG PARAMETER NO */
              ucEEPROMCounter = CONFIG_PARA_107;
              /*****************************/
            }
            else if(BIT3_SET == (pstCANrxmsg->Data[DATA_BYTE_ONE] & BIT3_MASK))
            {	
#ifdef DBG_CAN_PARSER_CONFIG      
              printf("CONFIG_WIFI_PORT_NUMBER\r\n");
#endif
              
              SplitByte.rgucData[DATA_BYTE_ZERO]= pstCANrxmsg->Data[DATA_BYTE_SIX];
              SplitByte.rgucData[DATA_BYTE_ONE] = pstCANrxmsg->Data[DATA_BYTE_SEVEN];
              stWifiIP.uiPort = SplitByte.uiData;
              /* CONFIG PARAMETER NO */
              ucEEPROMCounter = CONFIG_PARA_108;
							
              /*****************************/
            }
          }			
          /*change32*/
          if(SOURCE_HMI == (uliCanID & SOURCE_ADDR_MASK))
          {
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            /****************************ABK*****************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_TABLET	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
//            printf("pstCANrxmsg->ExtId = %x \r\n",pstCANrxmsg->ExtId);
            ////						stWifi.ucSendFlag = SET;
            ////						stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
          }				
        }
        break;
        /*change29*/
        /* Recieve Configuration parameters for WIFI SSID from HMI */
      case CONFIG_WIFI_SSID:
        {
          
#ifdef DBG_CAN_PARSER_CONFIG      
          printf("CONFIG_WIFI_SSID\r\n");
#endif
          if(FRAME_1 == pstCANrxmsg->Data[0])
          {
            
            for(ucLoopCnt = 1; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              stConfigData.rgucConfig_WifiSSID1[ucLoopCnt - 1] = 
                pstCANrxmsg->Data[ucLoopCnt];
            }
            stConfigData.rgucConfig_WifiSSID1[ucLoopCnt - 1] = NULL;
#ifdef DBG_CAN_PARSER_CONFIG
            printf("%s\r\n",stConfigData.rgucConfig_WifiSSID1);
#endif
          }
          if(FRAME_2 == pstCANrxmsg->Data[0])
          {
            for(ucLoopCnt = 1; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              stConfigData.rgucConfig_WifiSSID2[ucLoopCnt - 1] = 
                pstCANrxmsg->Data[ucLoopCnt];
            }
            stConfigData.rgucConfig_WifiSSID2[ucLoopCnt - 1] = NULL;
#ifdef DBG_CAN_PARSER_CONFIG
            printf("%s\r\n", stConfigData.rgucConfig_WifiSSID2);
#endif
						memset(stWifiIP.rgucSSID,'\0',16);
          strcat((char *)stWifiIP.rgucSSID,(char *)stConfigData.rgucConfig_WifiSSID1);
          strcat((char *)stWifiIP.rgucSSID,(char *)stConfigData.rgucConfig_WifiSSID2);
						
          }
          memset(stWifiIP.rgucSSID,'\0',16);
          strcat((char *)stWifiIP.rgucSSID,(char *)stConfigData.rgucConfig_WifiSSID1);
          strcat((char *)stWifiIP.rgucSSID,(char *)stConfigData.rgucConfig_WifiSSID2);
          /* CONFIG PARAMETER NO */
          ucEEPROMCounter = CONFIG_PARA_109;
          /*****************************/
             
          /*change32*/
          if(SOURCE_HMI == (uliCanID & SOURCE_ADDR_MASK))
          {
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            /****************************ABK*****************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_TABLET	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
//            printf("pstCANrxmsg->ExtId = %x \r\n",pstCANrxmsg->ExtId);
            ////						stWifi.ucSendFlag = SET;
            ////						stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
          }				
        }
        break;
        /*change29*/
        /* Recieve Configuration parameters for WIFI SSID from HMI */
      case CONFIG_WIFI_PASS:
        {
          
#ifdef DBG_CAN_PARSER_CONFIG      
          printf("CONFIG_WIFI_PASS\r\n");
#endif
          if(FRAME_1 == pstCANrxmsg->Data[0])
          {
            
            for(ucLoopCnt = 1; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              stConfigData.rgucConfig_WifiPASS1[ucLoopCnt - 1] = 
                pstCANrxmsg->Data[ucLoopCnt];
            }
            stConfigData.rgucConfig_WifiPASS1[ucLoopCnt - 1] = NULL;
#ifdef DBG_CAN_PARSER_CONFIG
            printf("%s\r\n",stConfigData.rgucConfig_WifiPASS1);
#endif
          }
          if(FRAME_2 == pstCANrxmsg->Data[0])
          {
            for(ucLoopCnt = 1; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              stConfigData.rgucConfig_WifiPASS2[ucLoopCnt - 1] = 
                pstCANrxmsg->Data[ucLoopCnt];
            }
            stConfigData.rgucConfig_WifiPASS2[ucLoopCnt - 1] = NULL;
#ifdef DBG_CAN_PARSER_CONFIG
            printf("%s\r\n", stConfigData.rgucConfig_WifiPASS2);
#endif
					memset(stWifiIP.rgucPASWD,'\0',16);
          strcat((char *)stWifiIP.rgucPASWD,(char *)stConfigData.rgucConfig_WifiPASS1);
          strcat((char *)stWifiIP.rgucPASWD,(char *)stConfigData.rgucConfig_WifiPASS2);
					
          }
          memset(stWifiIP.rgucPASWD,'\0',16);
          strcat((char *)stWifiIP.rgucPASWD,(char *)stConfigData.rgucConfig_WifiPASS1);
          strcat((char *)stWifiIP.rgucPASWD,(char *)stConfigData.rgucConfig_WifiPASS2);
          /* CONFIG PARAMETER NO */
          ucEEPROMCounter = CONFIG_PARA_110;
          /*****************************/
          
          /*change32*/
          if(SOURCE_HMI == (uliCanID & SOURCE_ADDR_MASK))
          {
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            /****************************ABK*****************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_TABLET	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
//            printf("pstCANrxmsg->ExtId = %x \r\n",pstCANrxmsg->ExtId);
            ////						stWifi.ucSendFlag = SET;
            ////						stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
          }				
        }
        break;
        /*change37*/				
        /* change37 Recieve Configuration parameters for Spraying count from HMI */
      case CONFIG_TIME_ZONE_OFFSET:
        {
#ifdef DBG_CAN_PARSER_CONFIG      
          printf("CONFIG_TIME_ZONE_OFFSET\r\n");
#endif	
          /* CONFIG PARAMETER NO */
          ucEEPROMCounter = CONFIG_PARA_116;
          /*****************************/
          stConfigData.ucConfig_TimeOffsetSign = pstCANrxmsg->Data[0];
#ifdef DBG_CAN_PARSER_CONFIG
          printf("ucConfig_TimeOffsetSign: %d\r\n", 
                 stConfigData.ucConfig_TimeOffsetSign );
#endif
          
          /*****************************/
          stConfigData.ucConfig_TimeOffsetHour = pstCANrxmsg->Data[1];
#ifdef DBG_CAN_PARSER_CONFIG
          printf("ucConfig_TimeOffsetHour: %d\r\n", 
                 stConfigData.ucConfig_TimeOffsetHour );
#endif
          /*****************************/
          stConfigData.ucConfig_TimeOffsetMin = pstCANrxmsg->Data[2];
#ifdef DBG_CAN_PARSER_CONFIG
          printf("ucConfig_TimeOffsetMin: %d\r\n", 
                 stConfigData.ucConfig_TimeOffsetMin );
#endif
          /*****************************/
          stConfigData.ucConfig_TimeOffsetSec = pstCANrxmsg->Data[3];
#ifdef DBG_CAN_PARSER_CONFIG
          printf("ucConfig_TimeOffsetSec: %d\r\n", 
                 stConfigData.ucConfig_TimeOffsetSec );
#endif		
          /*change32*/
          if(SOURCE_HMI == (uliCanID & SOURCE_ADDR_MASK))
          {
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            /****************************ABK*****************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_TABLET	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
            //printf("pstCANrxmsg->ExtId = %x \r\n",pstCANrxmsg->ExtId);
            ////						stWifi.ucSendFlag = SET;
            ////						stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
          }				
          
        }
        break;
/*change42*/      
				/* Recieve Configuration parameters for Supervisor name parameter from HMI */
      case CONFIG_JOB_ID:
        {
          
#ifdef DBG_CAN_PARSER_CONFIG      
          printf("CONFIG_JOB_ID\r\n");
#endif	
          if(FRAME_1 == pstCANrxmsg->Data[0])
          {
            /*change25*/
            /*before updating the name save previous to flash*/
           /* memset(jobid,'\0',FLASH_RX_TX_BUFFER_SIZE);
            strcat((char *)jobid,(char *)stConfigData.rgucConfig_JobId1);
            strcat((char *)jobid,(char *)stConfigData.rgucConfig_JobId2);
            strcat((char *)jobid,(char *)stConfigData.rgucConfig_JobId3);*/	
						/*change48*/
           // fnStoreJobID(ucJobID,&jobid[0]);	
						/*change78*/						
						memset(jobidpre,'\0',FLASH_RX_TX_BUFFER_SIZE);
            strcat((char *)jobidpre,(char *)stConfigData.rgucConfig_JobId1);
            strcat((char *)jobidpre,(char *)stConfigData.rgucConfig_JobId2);
            strcat((char *)jobidpre,(char *)stConfigData.rgucConfig_JobId3);	
						StatusFlag.stStatusFlag.bConfigJobID = SET;
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_117;
            /*****************************/
            
            for(ucLoopCnt = 1; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              stConfigData.rgucConfig_JobId1[ucLoopCnt - 1] = 
                pstCANrxmsg->Data[ucLoopCnt];
            }
            stConfigData.rgucConfig_JobId1[ucLoopCnt - 1] = NULL;
            
#ifdef DBG_CAN_PARSER_CONFIG
            printf("%s\r\n",stConfigData.rgucConfig_JobId1);
#endif
           /*change48*/
          //  ucJobID++;
            /*change26*/ 
            /* Check for Supervisor ID and Job ID & Job ID Range to reset Flash*/
        //    fnCheck_ID();
#ifdef DBG_CAN_PARSER_CONFIG
           printf("%d\r\n",ucJobID);
#endif
          }
          if(FRAME_2 == pstCANrxmsg->Data[0])
          {
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_118;
            /*****************************/
            
            
            for(ucLoopCnt = 1; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              stConfigData.rgucConfig_JobId2[ucLoopCnt - 1] = 
                pstCANrxmsg->Data[ucLoopCnt];
            }
            stConfigData.rgucConfig_JobId2[ucLoopCnt - 1] = NULL;
#ifdef DBG_CAN_PARSER_CONFIG
            printf("%s\r\n", stConfigData.rgucConfig_JobId2);
#endif
          }
          if(FRAME_3 == pstCANrxmsg->Data[0])
          {
            /* CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_119;
            /*****************************/
            
            for(ucLoopCnt = 1; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              stConfigData.rgucConfig_JobId3[ucLoopCnt - 1] = 
                pstCANrxmsg->Data[ucLoopCnt];
            }
            stConfigData.rgucConfig_JobId3[ucLoopCnt - 1] = NULL;
#ifdef DBG_CAN_PARSER_CONFIG
            printf("%s\r\n",stConfigData.rgucConfig_JobId3);
#endif
            
          }
          /*change32*/
          if(SOURCE_HMI == (uliCanID & SOURCE_ADDR_MASK))
          {
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            /****************************ABK*****************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                                  (DEST_TABLET	 | SOURCE_SCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
            printf("pstCANrxmsg->ExtId = %x \r\n",pstCANrxmsg->ExtId);
            ////						stWifi.ucSendFlag = SET;
            ////						stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
          }				
        }
        break;

      }
    }
    break;
  }
}

/*******************************************************************************
**@Function 			: fnAllERROR_Clear
**@Descriptions	: This function willsend clear all errors to all modules
**@parameters		: None
**@return				: None
*******************************************************************************/
void fnAllERROR_Clear(uint8_t ucSour)
{
  if(RESET	== StatusFlag.stStatusFlag.bEStop_Flag )
  {
	/*Clear all error to HCM */
	stTx1Message.DLC = 0;
	stTx1Message.IDE = CAN_ID_EXT;
	stTx1Message.RTR = CAN_RTR_DATA;
	stTx1Message.ExtId = 	MSG_TYPE_COMMAND | CMD_CLEAR_ERROR |
      DEST_HCM | SOURCE_SCU;
	fnCANMsg_Transmit(&stTx1Message, CAN1);
	/*Clear all error to VCM */
	stTx1Message.ExtId = 	MSG_TYPE_COMMAND | CMD_CLEAR_ERROR |
      DEST_VCM | SOURCE_SCU;
	fnCANMsg_Transmit(&stTx1Message, CAN1);
	/*Clear all error to OPMMM */
	stTx1Message.ExtId = 	MSG_TYPE_COMMAND | CMD_CLEAR_ERROR |
      DEST_OPMM | SOURCE_SCU;
	fnCANMsg_Transmit(&stTx1Message, CAN1);	
    /*clear error flags */
    StatusFlag.stStatusFlag.bErrorFlag = RESET;/*Error SET*/	
    StatusFlag.stStatusFlag.bErrorSentFlag = RESET;
    
	/*HB FAILURE FLAGS clear */	
	uiHBACKfail = RESET ;
	uiHBBakACKfail = RESET;
	uiHBTxCounter = RESET;
	
	/* Clear all Flags, Related to PROCESS */
    StatusFlag.stStatusFlag.bPrimeSelector = RESET;
    StatusFlag.stStatusFlag.bSpraySelector = RESET;
    StatusFlag.stStatusFlag.bFlushSelector = RESET;
    StatusFlag.stStatusFlag.bInProgress		=	RESET;
	//			ucPreviousProcess = FLUSHPROCESS;    //change12    
	/*change34*/
    /*change35 clear all error bits and take log*/
	Fault_Log.stFault_Log_Flag.bFault_Log_SVF= RESET;				/* Spray valve fault error*/
	Fault_Log.stFault_Log_Flag.bFault_Log_GPVF= RESET;			/* Gun Position valve fault error*/
	Fault_Log.stFault_Log_Flag.bFault_Log_ABVF= RESET;			/* AB valve fault error*/		
	Fault_Log.stFault_Log_Flag.bFault_Log_FVF= RESET;				/* Flush valve fault error*/
	Fault_Log.stFault_Log_Flag.bFault_Log_STVF= RESET;			/* Stall valve fault error */
	Fault_Log.stFault_Log_Flag.bFault_Log_IBHTF= RESET;			/* Inline Base temperature High threshold fault error change35 */
	Fault_Log.stFault_Log_Flag.bFault_Log_IHHTF= RESET;			/* Inline Hard temperature high threshold fault error change35*/
	Fault_Log.stFault_Log_Flag.bFault_Log_AHTF= RESET;			/* Antifreeze temperature high threshold fault error change35 */
	Fault_Log.stFault_Log_Flag.bFault_Log_BRLHTHF= RESET;		/* Base Return Line temperature high threshold fault error change35 */
	Fault_Log.stFault_Log_Flag.bFault_Log_PG1HTF= RESET;		/* Pressure Gauge1 high threshold fault error change35 */
	Fault_Log.stFault_Log_Flag.bFault_Log_PG2HTF= RESET;		/* Pressure Gauge2 high threshold fault  error change35*/
	Fault_Log.stFault_Log_Flag.bFault_Log_PG3HTF= RESET;		/* Pressure Gauge3 high threshold fault error change35*/
	Fault_Log.stFault_Log_Flag.bFault_Log_LTF= RESET;				/* Linear Transducer fault error */
	Fault_Log.stFault_Log_Flag.bFault_Log_DCLSTF= RESET;		/* DCLS time fault error */
	Fault_Log.stFault_Log_Flag.bFault_Log_HLSTF= RESET;			/* HLS time fault error */
	Fault_Log.stFault_Log_Flag.bFault_Log_HMIHAF= RESET;	/* HMI heartbeat ack fault error*/
	Fault_Log.stFault_Log_Flag.bFault_Log_OPMMHAF= RESET;		/* OPMM heartbeat ack fault  error*/
	Fault_Log.stFault_Log_Flag.bFault_Log_VCMHAF= RESET;		/* VCM heartbeat ack fault error*/
	Fault_Log.stFault_Log_Flag.bFault_Log_HCMHAF= RESET;		/* HCM heartbeat ack fault error */
	Fault_Log.stFault_Log_Flag.bFault_Log_RVF = RESET; /*change54*/
	if((RESET == stEthernetLog.ucSendFlag)||(RESET == stWifiLog.ucSendFlag))  /*change65*/
			{
fnFault_Log();  /*take log after clearing error change34 change35*/
			}
	         
     if(SOURCE_HMI == ucSour)
		{	     
			stTx1Message.ExtId = 	MSG_TYPE_COMMAND | CMD_CLEAR_ERROR |
															DEST_TABLET | SOURCE_SCU;
          
          //						stWifi.ucSendFlag = SET;
          //						stEthernet.ucSendFlag = SET;
          fnXML_FIFO(&stTx1Message);
		}
  }
}
/*******************************************************************************
**@Function 			: fnAllWarnnings_Clear
**@Descriptions	: This function willsend clear Warnings of particular module
**@parameters		: None
**@return				: None
*******************************************************************************/
void fnAllWarnnings_Clear(uint8_t ucDest,uint8_t ucSour)
{
  if(RESET	== StatusFlag.stStatusFlag.bEStop_Flag )
  {
	/*Clear all warning to Particular module */
	stTx1Message.DLC = ONE_BYTES;
	stTx1Message.Data[DATA_BYTE_ZERO] = ucDest  ;
	stTx1Message.IDE = CAN_ID_EXT;
	stTx1Message.RTR = CAN_RTR_DATA;
	stTx1Message.ExtId = 	MSG_TYPE_COMMAND | CMD_CLEAR_WARNING |
      ((uint32_t)(ucDest<<8)) | SOURCE_SCU;
	fnCANMsg_Transmit(&stTx1Message, CAN1);
		if(SOURCE_HMI == ucSour)
		{			
	stTx1Message.ExtId = 	MSG_TYPE_COMMAND | CMD_CLEAR_WARNING |
															DEST_TABLET | SOURCE_SCU;
          
          //						stWifi.ucSendFlag = SET;
          //						stEthernet.ucSendFlag = SET;
          fnXML_FIFO(&stTx1Message);	
		}
	/* Clear all Flags, Related to PROCESS */
    StatusFlag.stStatusFlag.bPrimeSelector = RESET;
    StatusFlag.stStatusFlag.bSpraySelector = RESET;
    StatusFlag.stStatusFlag.bFlushSelector = RESET;
    StatusFlag.stStatusFlag.bInProgress		=	RESET;
    //	if(OPMM_ACTIVE_CAN == CAN2)
    //	{
    //		fnCANMsg_Transmit(&stTx1Message, OPMM_ACTIVE_CAN);
    //	}
	/* clear Warning flags */
    StatusFlag.stStatusFlag.bWarningFlag = RESET;/*Error SET*/	
    StatusFlag.stStatusFlag.bErrorSentFlag = RESET;
    /* change35 Clear warnings bits of particular module*/
    switch(ucDest)
    {	/* clear OPMM warning flags */
    case ID_OPMM:
      {
        Fault_Log.stFault_Log_Flag.bFault_Log_MFVF= RESET;			/* Motor Forward valve fault warning*/
        Fault_Log.stFault_Log_Flag.bFault_Log_MBVF= RESET;			/* Motor Backward valve fault warning */
        Fault_Log.stFault_Log_Flag.bFault_Log_IBLTF= RESET;			/* Inline Base temperature low threshold fault warning change35 */
        Fault_Log.stFault_Log_Flag.bFault_Log_SGMF= RESET;			/* Spray Gun Motion fault warning */
      }
      break;
    case ID_HCM:
      {
        Fault_Log.stFault_Log_Flag.bFault_Log_IHLTF= RESET;			/* Inline Hard temperature low threshold fault  warning change35*/	
        Fault_Log.stFault_Log_Flag.bFault_Log_ALTF= RESET;			/* Antifreeze temperature low threshold fault warning change35 */
        Fault_Log.stFault_Log_Flag.bFault_Log_BRLLTHF= RESET;		/* Base Return Line temperature low threshold fault  warning change35 */
        Fault_Log.stFault_Log_Flag.bFault_Log_LLBLTF= RESET;		/* Base Liquid low Level threshold fault warning change35*/
        Fault_Log.stFault_Log_Flag.bFault_Log_LLBHTF= RESET;		/* Base Liquid high Level threshold fault warning change35*/
        Fault_Log.stFault_Log_Flag.bFault_Log_LLHLTF= RESET;		/* Hard Liquid low Level threshold fault warning change35*/
        Fault_Log.stFault_Log_Flag.bFault_Log_LLHHTF= RESET;		/* Hard Liquid high Level threshold fault warning change35*/
        Fault_Log.stFault_Log_Flag.bFault_Log_IBHF= RESET;			/* Inline Base Heater fault warning */
        Fault_Log.stFault_Log_Flag.bFault_Log_IHHF= RESET;			/* Inline Hard Heater fault warning */
        Fault_Log.stFault_Log_Flag.bFault_Log_AHF= RESET;				/* Antifreeze Heater fault warning*/
        Fault_Log.stFault_Log_Flag.bFault_Log_HHF= RESET;				/* Hardener Heater fault warning*/
      }
      break;
    case ID_VCM:
      {
        Fault_Log.stFault_Log_Flag.bFault_Log_PG1LTF= RESET;		/* Pressure Gauge1 Low threshold fault warning change35 */
        Fault_Log.stFault_Log_Flag.bFault_Log_PG2LTF= RESET;		/* Pressure Gauge2 Low threshold fault  warning change35*/
        Fault_Log.stFault_Log_Flag.bFault_Log_PG3LTF= RESET;		/* Pressure Gauge3 Low threshold fault  warning change35*/
        
      }
      break;
    case ID_SCU:
      {
        Fault_Log.stFault_Log_Flag.bFault_Log_PTF= RESET;				/* Non-Contact Pipe temperature out of range fault warning */
      }
      break;
    }
		if((RESET == stEthernetLog.ucSendFlag)||(RESET == stWifiLog.ucSendFlag))  /*change65*/
			{
    fnFault_Log();  /*take log after clearing error change34 change35*/
			}
  }
}
/*******************************************************************************
**@Function 			: fnPeriodicHeartbeat
**@Descriptions	: This function will send the periodic HB to all the slave
**@parameters		: None
**@return				: None
*******************************************************************************/
void fnPeriodicHeartbeat(void)
{
  
  CanTxMsg stHBmsg;
  if(HB_SND_TIME <= uiHBTxCounter)
  {
    memset(stHBmsg.Data,'\0', strlen((char *)stHBmsg.Data));
    stHBmsg.IDE 	= CAN_ID_EXT;
    stHBmsg.RTR 	= CAN_RTR_DATA;
    stHBmsg.DLC 	= 0;
    stHBmsg.StdId = 0;
    stHBmsg.ExtId = MSG_TYPE_BROADCAST | RESERVED |
      BCAST_HEARTBEAT | DEST_ALL | SOURCE_SCU;
    
    
    /*Transmitt CAN message onto CAN1 bus*/
    CAN_Transmit(CAN1, &stHBmsg);	
    //		CAN_Transmit(CAN2, &stHBmsg);	
    if(SET	== StatusFlag.stStatusFlag.bCAN2ActiveFlag)
    {
      //#ifdef DBG_CAN_HB
      //printf("\n CAN2 = 0x%x\r\n", stHBmsg.ExtId);
      //#endif
      
      CAN_Transmit(CAN2, &stHBmsg);
    }
#ifdef DBG_CAN_HB
    printf("\n CANID = 0x%x\r\n", stHBmsg.ExtId);
#endif
#ifdef DBG_CAN_HB		
    printf("uiHBTxCounter = %d \r\n",uiHBTxCounter);
#endif		
    uiHBTxCounter = 0;
    /* increment heartbeat counter */
	for(ucHBCntr = 0; ucHBCntr < TOTAL_HB_DEVICE; ucHBCntr++)
    {					
      if(rgucHBCntr[ucHBCntr] >= HB_RESND_CNT)
      {
        uiHBACKfail |= (1 << ucHBCntr);
      }
      else
        rgucHBCntr[ucHBCntr] += 1;
    }
    uiHBACKfail &= 0x000F;
  }
  
  if(uiHBACKfail != 0)
  {
    rgucHBFail[0] = uiHBACKfail & 0xff;
    for(ucHBCntr = 0; ucHBCntr < TOTAL_HB_DEVICE; ucHBCntr++)
    {
      if((!((uiHBFailSend >> ucHBCntr) & 0x01)) && 
         ((uiHBACKfail >> ucHBCntr) & 0x01))
      {
        uiHBFailSend |= (1 << ucHBCntr);
      }
    }
    //		if(SET == StatusFlag.stStatusFlag.bHBAckNotRcvFaultSndFlag)
    //		{
    if(0x0F == (uiHBACKfail & 0x0F))
    {
      StatusFlag.stStatusFlag.bCAN1ActiveFlag = RESET;
      StatusFlag.stStatusFlag.bCAN2ActiveFlag = RESET;
      StatusFlag.stStatusFlag.bCANFaultFlag = SET;	
      StatusFlag.stStatusFlag.bErrorFlag = SET;
      StatusFlag.stStatusFlag.bErrorSentFlag = SET;
      ////				#ifdef DBG_CAN_HB		
      //				printf("HEARTBEAT ACKNOWLEDGE FAILED uiHBACKfail =0x%x\n\r",uiHBACKfail);
      //				#endif	
      
    }
    else 
    {
      if((uiHBACKfail>>HMI) & 0x01)
      {
        Fault_Log.stFault_Log_Flag.bFault_Log_HMIHAF = SET;	
#ifdef DBG_CAN_HB		
        printf("HMI ACKNOWLEDGEMENT FAILED\n\r");
#endif	
      }
      if((uiHBACKfail>>OPMM) & 0x01)
      {
        ucCANLinkFaultCnt++;
        if(CAN_FAULT_COUNT <= ucCANLinkFaultCnt) 
        {
          ucCANLinkFaultCnt = 0;
          StatusFlag.stStatusFlag.bCANFaultFlag = SET;	
          StatusFlag.stStatusFlag.bErrorFlag = SET;
          Fault_Log.stFault_Log_Flag.bFault_Log_OPMMHAF = SET;
        }
        else if(SET	== StatusFlag.stStatusFlag.bCAN1ActiveFlag)
        {
          StatusFlag.stStatusFlag.bCAN2ActiveFlag = SET;
          StatusFlag.stStatusFlag.bCAN1ActiveFlag =RESET;
          ClearBit(uiIOExpData , CAN2_STATUS_RED_LED);
          ClearBit(uiIOExpData , CAN2_STATUS_BLUE_LED);
          SetBit(uiIOExpData , CAN2_STATUS_GREEN_LED);
          
#ifdef DBG_CAN_HB		
          printf("OPMM_ACTIVE_CAN2\n\r");
#endif
          rgucHBCntr[OPMM] = 0; 
          uiHBACKfail &= ~(1 << OPMM);  /*Clear HB ACK fault  if any*/
          uiHBFailSend &= ~(1 << OPMM); /*RESET the HB Fail send VAriable*/
          ucDevDead_OPMM = 0;
        }
        
#ifdef DBG_CAN_HB		
        printf("OPMM ACKNOWLEDGEMENT FAILED\n\r");
#endif	
      }
      if((uiHBACKfail>>VCM) & 0x01)
      {
        Fault_Log.stFault_Log_Flag.bFault_Log_VCMHAF = SET;	
#ifdef DBG_CAN_HB		
        printf("VCM ACKNOWLEDGEMENT FAILED\n\r");
#endif	
      }
      if((uiHBACKfail>>HCM) & 0x01)
      {
        Fault_Log.stFault_Log_Flag.bFault_Log_HCMHAF = SET;	
#ifdef DBG_CAN_HB		
        printf("HCM ACKNOWLEDGEMENT FAILED\n\r");
#endif	
      }
    }
    //				/*OP HB not Rcv then OP CAN fault Declaired*/
    //				if(uiHBACKfail & 0x01)
    //				{
    //					/*OP CAN FAULT*/
    //					StatusFlag.stStatusFlag.bOPCANFaultFlag  = SET;
    //					/*ONLY WHEN Both CAN Fault Occurs*/		
    //					StatusFlag.stStatusFlag.bErrorFlag = SET;
	//		StatusFlag.stStatusFlag.bErrorSentFlag = SET;
    //				}
    //				/*Else any other module CAN Fault Declaire*/
    //				if(uiHBACKfail & (~0x01))
    //				{
    //					/*if any other CAN fault except OP and OPMM*/
    //					if(uiHBACKfail & (~0x02))
    //					{
    //						/* Any Other CAN Fault*/
    //						StatusFlag.stStatusFlag.bAnyCANFaultFlag = SET;	
    //						/*ONLY WHEN Both CAN Fault Occurs*/	
    //						StatusFlag.stStatusFlag.bErrorFlag = SET;
	//		StatusFlag.stStatusFlag.bErrorSentFlag = SET;
    //					}									
    //				}
    //			}*/
    //			StatusFlag.stStatusFlag.bHBAckNotRcvFaultSndFlag = RESET;
    if(uiHBBakACKfail != uiHBACKfail)
    {
      fnHBAckFail_Send(((uiHBBakACKfail ^ uiHBACKfail)&uiHBACKfail));	
      uiHBBakACKfail = uiHBACKfail;
			if((RESET == stEthernetLog.ucSendFlag)||(RESET == stWifiLog.ucSendFlag))  /*change65*/
			{
      fnFault_Log();/*Fault Log for the heartbeat not received*/
			}
    }
  }	
  uiHBBakACKfail |= uiHBACKfail;
  uiHBACKfail = 0;	
}
//}

/*****************************************************************************
**@Function 				: fnFifoAckTimeout_Check
**@Descriptions		: this is use to check the Time out of Acknowledge 
received from each module 
**@parameters			: None 
**@return					: None
*****************************************************************************/
void fnFifoAckTimeout_Check(void)
{
  if(SET == StatusFlag.stStatusFlag.bFifoAckTimeoutCheckFlag)
  {
    StatusFlag.stStatusFlag.bFifoAckTimeoutCheckFlag = RESET;
    if(ucTimerEn_HMI)
    {
      if(uliTimeoutCnt_HMI > ACK_TIMEOUT_DELAY)
      {
        uliTimeoutCnt_HMI = 0;
        ucTimerEn_HMI = 0;
        fnCANPost_TimeOut(DEST_HMI);
      } 		
      else
      {
        uliTimeoutCnt_HMI++;
      }			
    }
    
    if(ucTimerEn_OPMM)
    {
      if(uliTimeoutCnt_OPMM > ACK_TIMEOUT_DELAY)
      {
        uliTimeoutCnt_OPMM = 0;
        ucTimerEn_OPMM = 0;
        fnCANPost_TimeOut(DEST_OPMM); 
      } 		
      else
      {
        uliTimeoutCnt_OPMM++;
      }			
    }
    
    if(ucTimerEn_VCM)
    {
      if(uliTimeoutCnt_VCM > ACK_TIMEOUT_DELAY)
      {
        uliTimeoutCnt_VCM = 0;
        ucTimerEn_VCM = 0;
        fnCANPost_TimeOut(DEST_VCM); 
      } 		
      else
      {
        uliTimeoutCnt_VCM++;
      }			
    }
    
    if(ucTimerEn_HCM)
    {
      if(uliTimeoutCnt_HCM > ACK_TIMEOUT_DELAY)
      {
        uliTimeoutCnt_HCM = 0;
        ucTimerEn_HCM = 0;
        fnCANPost_TimeOut(DEST_HCM); 
      } 		
      else
      {
        uliTimeoutCnt_HCM++;
      }			
    }
    
    
    
  }
}



/*****************************************************************************
**@Function 			: fnHBAckFail_Send
**@Descriptions	: Send command to OP to indicate HB ack is not received
**@parameters		: None
**@return				: None
*****************************************************************************/
void fnHBAckFail_Send(uint16_t uiHBACKFail)
{
  CanTxMsg stHBmsg;
  
  /* Reset Data Bufer */
  memset(stHBmsg.Data,'\0', strlen((char *)stHBmsg.Data));
  stHBmsg.StdId = 0;
  stHBmsg.ExtId = MSG_TYPE_FAULT | RESERVED |
    FAULT_HB_ACK_NOT_RECEIVED | DEST_HMI | SOURCE_SCU ;
  stHBmsg.IDE = CAN_ID_EXT;
  stHBmsg.RTR = CAN_RTR_DATA;
  stHBmsg.DLC = 1;
  
  // stHBmsg.Data[0] = (uint8_t)(uiHBACKfail & 0xFF) ;
  stHBmsg.Data[0] = (uint8_t)(uiHBACKFail & 0xFF) ;
  
  /*Transmitt CAN message onto CAN bus*/
  fnCANMsg_Transmit(&stHBmsg, CAN1);
	/*change55*/
	 /****************************ABK*****************************/
  stTx1Message.ExtId = stHBmsg.ExtId;    
	stTx1Message.ExtId = ((stTx1Message.ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                            (DEST_TABLET	 | SOURCE_SCU));
	stTx1Message.DLC = 1;													
   stTx1Message.Data[0] = stHBmsg.Data[0];   
      ////						stWifi.ucSendFlag = SET;
      ////						stEthernet.ucSendFlag = SET;
      fnXML_FIFO(&stTx1Message);

}

/*****************************************************************************
**@Function      :  fnFifo_Write
**@Descriptions  :  Store msg frame into Tx FIFO
**@parameters    :  txMsg points to msg frame from MPU
stFifo points to FIFO stucture
**@Return        :  None
*****************************************************************************/
uint8_t fnFifo_Write(CanTxMsg *stTx1Message, stTxMsgFlags *stFifo)
{
  uint8_t ucWriteStatus = 0;
  uint8_t ucWriteCnt = 0;
  CanTxMsg *stTempCANMsg;
  
  ucWriteCnt = stFifo->ucWriteCnt;
  
  /*When fifo wr is called, fifo full goes low */
  stFifo->bFifoEmpty = 0;
  
  /*When FIFO is full and write happens, data lost and fault signal is set*/
  if(stFifo->bFifoFull == 1)
  {
    stFifo->bFiFullFlt = 1;
    ucWriteStatus = 0;
    return (ucWriteStatus);
  }
  else
  {
    /* Storing data into FIFO location */
    stTempCANMsg = &(stFifo->stMsg.rgFifo[ucWriteCnt]);
    
    /*Copy by value into the FIFO we have created*/
    stTempCANMsg->DLC = stTx1Message->DLC;
    stTempCANMsg->ExtId = stTx1Message->ExtId;
    stTempCANMsg->IDE = stTx1Message->IDE;
    stTempCANMsg->RTR = stTx1Message->RTR;
    stTempCANMsg->StdId = stTx1Message->StdId;
    
    for(ucLoopCnt = 0; ucLoopCnt < stTempCANMsg->DLC; ucLoopCnt++)
    {
      stTempCANMsg->Data[ucLoopCnt] = stTx1Message->Data[ucLoopCnt];
    }
    /* Write pointer incremented */
    stFifo->ucWriteCnt++;
    
    /* Data successfully written flag is set */
    ucWriteStatus = 1;
    
    /* Write pointer wrapping is done */
    if(stFifo->ucWriteCnt == FIFO_DEPTH)
    {
      stFifo->ucWriteCnt = 0;
    }
    if (stFifo->ucWriteCnt == stFifo->ucReadCnt)
      stFifo->bFifoFull = 1;
    return (ucWriteStatus);
  }
}




/*****************************************************************************
**@Function 			: fnFifo_Read
**@Descriptions	: Reads msg frame from Tx FIFO
**@parameters		: pointer to structure Fifo
**@return				: msg frame to transmit on can bus
*****************************************************************************/
void fnFifo_Read(stTxMsgFlags *pstFifo, CanTxMsg *pstCurMsgTrans)
{
  pstFifo->bFifoFull = 0;
#ifdef FIFO_DBG
  printf("FIFO RD POINTER = %d\r\n", pstFifo->ucReadCnt);
#endif
  /*When FIFO is empty and read happens, 
  NULL pointer is written and flag is set*/
  if (1 == pstFifo->bFifoEmpty)
  {
    pstFifo->bFiEmpFlt = 1;
#ifdef FIFO_DBG
    printf("FIFO EMPTY FAULT = %d\r\n", pstFifo->bFiEmpFlt);
#endif
  }
  else
  {
    pstFifo->bFiFullFlt = 0; /*When fifo is read, fifo full fault is reset*/
    
    memmove(pstCurMsgTrans, &pstFifo->stMsg.rgFifo[pstFifo->ucReadCnt],
            sizeof(CanTxMsg));
    /* Read pointer is incremented */
    pstFifo->ucReadCnt++;
    
    /* Read pointer wrapping is done */
    if(FIFO_DEPTH == pstFifo->ucReadCnt)
    {
      pstFifo->ucReadCnt = 0;
    }
    
    if (pstFifo->ucReadCnt == pstFifo->ucWriteCnt)
    {
      pstFifo->bFifoEmpty = 1;
    }
    else
    {
      pstFifo->bFifoEmpty = 0;
    }
  }
}


/*****************************************************************************
**@Function      : fnCANMsg_Transmit
**@Descriptions  : Tx msg is stored in FIFO and intiates frame transmission
and intiates ACK ptotocol.
**@parameters    : pstCANTxmsg: Pointer to CAN Tx Struct
CANx: 	CAN Peripheral x : 1/2
**@Return        : Either msg frame written in FIFO/msg lost as FIFO is full
*****************************************************************************/
uint8_t fnCANMsg_Transmit(CanTxMsg *pstCANTxmsg, CAN_TypeDef* CANx)
{
  uint32_t uliCANId = pstCANTxmsg->ExtId;
  uint8_t ucMsgTxStat;
  uint8_t ucFifoWrStatus;
  pstCANTxmsg->IDE = CAN_ID_EXT;
  pstCANTxmsg->RTR = CAN_RTR_DATA;
  
#ifdef DBG_CAN
  printf("\n CANID = 0x%x\r\n", pstCANTxmsg->ExtId);
#endif
  switch (uliCANId & DEST_ADDR_MASK)
  {
  case DEST_HMI:
    {
      if((uiHBACKfail >> HMI) == NEGATIVE_ACK)
      {
        /*No Heart Beat ACK is received from OP, so delcaring it to dead*/
        ucDevDead_HMI = 1;
#ifdef FIFO_DBG
        printf("IN HMI DEAD\n\r");
#endif
        ucMsgTxStat = DEVICE_DEAD;
      }
      else
      {
        if(ucMsgTxCnt_HMI)/*Msg frame is being transmitted and*/
        {
          /*non zero value*/
          ucFifoWrStatus = fnFifo_Write(pstCANTxmsg, &stFifo_HMI);
          
          if(ucFifoWrStatus)
          {
            /*msf frame is stored in Tx fifo*/
            ucMsgTxStat = MSG_STORE_IN_FIFO;
          }
          else
          {
            /*Msg not written in fifo as it is full*/
            ucMsgTxStat = FIFO_FULL_MSG_DISCARDED;
          }
        }
        else
        { 
          /*zero value*/
          /*When nothing is transmitted on line, transmit msg frame*/
          CAN_Transmit(CANx, pstCANTxmsg);
          
          ucMsgTxCnt_HMI++; /*increment msg transmit counter*/
          
          ucTimerEn_HMI = 1;	/*Timeout  counter enable set*/
          
          memmove(&stCurMsgTrans_HMI, pstCANTxmsg, sizeof(CanTxMsg));
          ucMsgTxStat = MSG_TRANSMITTING;/*currently data is under TX*/
        }
      }
    }
    break;
  case DEST_OPMM:
    {
      if((uiHBACKfail >> OPMM) == NEGATIVE_ACK)
      {
        if(CAN_FAULT_COUNT <= ucCANLinkFaultCnt) 
        {
          /*No Heart Beat ACK is received from OPMM, so delcaring it to dead*/
          ucDevDead_OPMM = 1;
          ucMsgTxStat = DEVICE_DEAD;
        }
      }
      else
      {
        if(ucMsgTxCnt_OPMM)/*Msg frame is being transmitted and*/
        {  
          /*non zero value*/							
          ucFifoWrStatus = fnFifo_Write(pstCANTxmsg, &stFifo_OPMM);
          if(ucFifoWrStatus)
          {
            /*msf frame is stored in Tx fifo*/
            ucMsgTxStat = MSG_STORE_IN_FIFO;
          }
          else
          {
            /*Msg not written in fifo as it is full*/
            ucMsgTxStat = FIFO_FULL_MSG_DISCARDED;
          }
        }
        else
        {
          /*zero value*/
          /*Check for CAN1 Active or CAN2 for OPMM*/
          //	CAN_Transmit(OPMM_ACTIVE_CAN, pstCANTxmsg);
          if(SET == StatusFlag.stStatusFlag.bCAN1ActiveFlag )
          {
            /*When nothing is transmitted on line, transmit msg frame*/
            CAN_Transmit(CAN1, pstCANTxmsg);
          }
          else
          {
            
            /*When nothing is transmitted on line, transmit msg frame*/
            CAN_Transmit(CAN2, pstCANTxmsg);
          }
          
          ucMsgTxCnt_OPMM++; /*increment msg transmit counter*/
          ucTimerEn_OPMM = 1;	/* Timeout  counter enable set*/
          
          memmove(&stCurMsgTrans_OPMM, pstCANTxmsg, sizeof(CanTxMsg));
          ucMsgTxStat = MSG_TRANSMITTING;/*currently data is under TX*/
        }
      }
    }
    break;
  case DEST_VCM:
    {
      if((uiHBACKfail >> VCM) == NEGATIVE_ACK)
      {
        /*No Heart Beat ACK is received from MFM, so delcaring it to dead*/
        ucDevDead_VCM = 1;
        ucMsgTxStat = DEVICE_DEAD;
      }
      else
      {
        if(ucMsgTxCnt_VCM)/*Msg frame is being transmitted and*/
        {  
          /*non zero value*/
          ucFifoWrStatus = fnFifo_Write(pstCANTxmsg, &stFifo_VCM);
          
          if(ucFifoWrStatus == 1)
          {
            /*msf frame is stored in Tx fifo*/
            ucMsgTxStat = MSG_STORE_IN_FIFO;
          }
          else
          {
            /*Msg not written in fifo as it is full*/
            ucMsgTxStat = FIFO_FULL_MSG_DISCARDED;
          }
        }
        else
        {   
          /*zero value*/
          /*When nothing is transmitted on line, transmit msg frame*/
          CAN_Transmit(CANx, pstCANTxmsg);
          ucMsgTxCnt_VCM++; /*increment msg transmit counter*/
          
          ucTimerEn_VCM = 1;	/* Timeout  counter enable set*/
          
          memmove(&stCurMsgTrans_VCM, pstCANTxmsg, sizeof(CanTxMsg));
          ucMsgTxStat = MSG_TRANSMITTING;/*currently data is under TX*/
        }
      }
    }
    break;
  case DEST_HCM:
    {
      if((uiHBACKfail >> HCM) == NEGATIVE_ACK)
      {
#ifdef FIFO_DBG
        printf("\n11111111111\n\r");
#endif
        /*No Heart Beat ACK is received from BHCM, so delcaring it to dead*/
        ucDevDead_HCM = 1;
        ucMsgTxStat = DEVICE_DEAD;
      }
      else
      {
        if(ucMsgTxCnt_HCM)/*Msg frame is being transmitted and*/
        {  
#ifdef FIFO_DBG
          printf("\n22222222222\n\r");
#endif
          /*non zero value*/
          ucFifoWrStatus = fnFifo_Write(pstCANTxmsg, &stFifo_HCM);
          if(ucFifoWrStatus)
          {
            /*msf frame is stored in Tx fifo*/
            ucMsgTxStat = MSG_STORE_IN_FIFO;
          }
          else
          {
            /*Msg not written in fifo as it is full*/
            ucMsgTxStat = FIFO_FULL_MSG_DISCARDED;
          }
        }
        else
        { 
          /*zero value*/
#ifdef FIFO_DBG
          printf("\n33333333333\n\r");
#endif
          /*When nothing is transmitted on line, transmit msg frame*/
          CAN_Transmit(CANx, pstCANTxmsg);
          
          ucMsgTxCnt_HCM++; /*increment msg transmit counter*/
          
          ucTimerEn_HCM = 1;	/* Timeout  counter enable set*/
          
          memmove(&stCurMsgTrans_HCM, pstCANTxmsg, sizeof(CanTxMsg));
          ucMsgTxStat = MSG_TRANSMITTING;/*currently data is under TX*/
        }
      }
    }
    break;
    
  default:
    {
      ucMsgTxStat =  WRONG_DESTINATION;
    }
    break;
  }
  return ucMsgTxStat;
}


/*****************************************************************************
**@Function 			:  fnCANPost_TimeOut
**@Descriptions	:	 Checks the ACK protocol implementation after timeout.
**@parameters		:  Can device Id, whose interupt routine is to select
**@return				:	 None
*****************************************************************************/
void fnCANPost_TimeOut(uint32_t uliDevId)
{
  uint8_t ucCANTxMsgStatus = 0;
  switch(uliDevId)
  {
  case DEST_HMI:
    {
      /*msg transmit counter 3, means we have done 3 msg tx*/
      if(3 == ucMsgTxCnt_HMI)
      {
        /*Since ACK is not received, we are setting device dead flag*/
        ucDevDead_HMI = 1;
      }
      else if (ucMsgTxCnt_HMI < 3)
      {
        ucCANTxMsgStatus = CAN_Transmit(CAN1, &stCurMsgTrans_HMI);
        if(CAN_TxStatus_NoMailBox != ucCANTxMsgStatus)
        {
          ucMsgTxCnt_HMI++;  
        }
        else
        {
          uliTimeoutCnt_HMI = 25; /*Set to 25 for retry this within 10ms*/
#ifdef FIFO_DBG
          //				printf("\n HMI BUFFER NOT EMPTY\r\n");
#endif
        }
        ucTimerEn_HMI = 1;/*Enable timer*/
      }
    }
    break;
  case DEST_OPMM:
    {
      /*msg transmit counter 3, means we have done 3 msg tx*/
      if(3 == ucMsgTxCnt_OPMM)
      {
        if(CAN_FAULT_COUNT <= ucCANLinkFaultCnt) 
        {
          /*No Heart Beat ACK is received from OPMM, so delcaring it to dead*/
          ucDevDead_OPMM = 1;
        }	
      }
      else if (ucMsgTxCnt_OPMM < 3)
      {
        if(SET == StatusFlag.stStatusFlag.bCAN2ActiveFlag)
        {
          
          ucCANTxMsgStatus = CAN_Transmit(CAN2, &stCurMsgTrans_OPMM);
        }
        else if(SET == StatusFlag.stStatusFlag.bCAN1ActiveFlag)
        {
          ucCANTxMsgStatus = CAN_Transmit(CAN1, &stCurMsgTrans_OPMM);
        }				
        
        if(CAN_TxStatus_NoMailBox != ucCANTxMsgStatus)
        {
          ucMsgTxCnt_OPMM++;		
        }
        else
        {
          uliTimeoutCnt_OPMM = 25; /*Set to 25 for retry this within 10ms*/
#ifdef FIFO_DBG
          printf("\n OPMM BUFFER NOT EMPTY\r\n");
#endif
        }
        ucTimerEn_OPMM = 1;			
      }
    }
    break;
  case DEST_VCM:
    {
      /*msg transmit counter 3, means we have done 3 msg tx*/
      if(3 == ucMsgTxCnt_VCM)
      {
        /*Since ACK is not received, we are setting device dead flag*/
        ucDevDead_VCM = 1;
      }
      else if (ucMsgTxCnt_VCM < 3)
      {
        ucCANTxMsgStatus = CAN_Transmit(CAN1, &stCurMsgTrans_VCM);
        if(CAN_TxStatus_NoMailBox != ucCANTxMsgStatus)
        {
          ucMsgTxCnt_VCM++;		
        }
        else
        {
          uliTimeoutCnt_VCM = 25; /*Set to 25 for retry this within 10ms*/
#ifdef FIFO_DBG
          printf("\n VCM BUFFER NOT EMPTY\r\n");
#endif
        }
        ucTimerEn_VCM = 1;
      }
    }
    break;
  case DEST_HCM:
    {
      /*msg transmit counter 3, means we have done 3 msg tx*/
      if(3 == ucMsgTxCnt_HCM)
      {
        /*Since ACK is not received, we are setting device dead flag*/
        ucDevDead_HCM = 1;
      }
      else if (ucMsgTxCnt_HCM < 3)
      {
#ifdef FIFO_DBG
        printf("\nstCurMsgTrans_HCM\r\n");
#endif
        ucCANTxMsgStatus = CAN_Transmit(CAN1, &stCurMsgTrans_HCM);
        if(CAN_TxStatus_NoMailBox != ucCANTxMsgStatus)
        {
          ucMsgTxCnt_HCM++;	
        }
        else
        {
          uliTimeoutCnt_HCM = 50; //25; /*Set to 25 for retry this within 10ms*/
#ifdef FIFO_DBG
          printf("\n HCM BUFFER NOT EMPTY\r\n");
#endif
        }
        ucTimerEn_HCM = 1;
      }
    }
    break;
  }
}


/*****************************************************************************
**@Function 		  : fnSendACK
**@Descriptions  : Sends acknowledgement frame on CAN bus
**@parameters	  : pstCANrxmsg: pointer to a structure receive frame 
which contains CAN Id,CAN DLC, CAN data and FMI number.
**@return			  : None
*****************************************************************************/
void  fnSendACK(CanRxMsg *pstCANrxmsg)
{
  CanTxMsg stTxMsgACK;
#ifdef DBG_CAN_PARSER
  printf("SEND ACK FOR MESSAGE= 0x%x\r\n",pstCANrxmsg->ExtId);
#endif
  SplitByte.uliData = pstCANrxmsg->ExtId;
  
  /*Configure CAN Transmitt Message tructure*/
  stTxMsgACK.IDE = CAN_ID_EXT;
  stTxMsgACK.RTR = CAN_RTR_DATA;
  
  /* Reset Data Bufer */
  memset(stTxMsgACK.Data,'\0', strlen((char *)stTxMsgACK.Data));
  
  /*Update Length of CAN Data Frame*/
  stTxMsgACK.DLC = 1;
  
  stTxMsgACK.Data[0] = 0x00;		/* Positive ACk */
  
  SplitByte.rgucData[1] = SplitByte.rgucData[0];
  SplitByte.rgucData[0] = SOURCE_SCU;
  SplitByte.rgucData[2] = 0xE8;
  stTxMsgACK.ExtId = SplitByte.uliData;
  
  /*Transmitt CAN message onto CAN bus*/
  if((SET == StatusFlag.stStatusFlag.bCAN2ActiveFlag) &&
     (ID_OPMM == SplitByte.rgucData[1]))
  {
    CAN_Transmit(CAN2, &stTxMsgACK);
#ifdef GENERAL_DBG
    printf("OPMM ACKS ON CAN2 link\r\n");
#endif
  }
  else 
  {
    CAN_Transmit(CAN1, &stTxMsgACK);
  }
}



/*****************************************************************************
**@Function 		 : fnCANPost_Ack
**@Descriptions : Checks the ACK protocol implementation after ACK received
**@parameters	 : pointer to struct message which is received
**@return			 : 0: No Msg in Fifo, 1: msg in FIFO and new msg send
*****************************************************************************/
uint8_t fnCANPost_Ack(CanRxMsg *pstCANRxmsg, CAN_TypeDef* CANx)
{
  uint32_t uliCANId = pstCANRxmsg->ExtId;
  
  if (uliCANId & ACKNOWLEDGEMENT)
  {
    switch(uliCANId & SOURCE_ADDR_MASK)/*CHeck the destination address*/
    {
    case SOURCE_HMI:
      {
        if(ucDevDead_HMI)
        {
#ifdef FIFO_DBG
          printf("\n ACK RECEIVED AFTER 3 MSG TRANSMISSIONS\r\n");
#endif						
        }
        else
        {
          ucTimerEn_HMI = 0;	 /*Reset device timer enable*/
          uliTimeoutCnt_HMI = 0;/*Reset device timeout counter*/
          ucMsgTxCnt_HMI = 0; /*Reset msg transmit counter*/
          ucDevDead_HMI = 0;  /*Set device dead-alive flag to alive*/
#ifdef FIFO_DBG
          printf("\n ACK RECEIVED SO MSG TRANSMISSION\n\rCOUNTER = %x\r\n"
                 ,ucMsgTxCnt_HMI);
#endif
          if (stFifo_HMI.bFifoEmpty)
          {  
            /*If FIFO is empty*/
#ifdef FIFO_DBG
            printf("\n FIFO IS EMPTY HMIr\n\n\n");
#endif
            
            return (NULL);
          }
          else
          { 
            /* If FIFO is not empty*/					
            /*Read one message from fifo into buffer*/
            fnFifo_Read(&stFifo_HMI, &stCurMsgTrans_HMI);
            
            CAN_Transmit(CAN1, &stCurMsgTrans_HMI);
            
            ucMsgTxCnt_HMI++;
            ucTimerEn_HMI = 1;
#ifdef FIFO_DBG
            printf("\n FIFO IS NOT EMPTY HMI\r\n");
#endif
          }
        }
      }
      break;
    case SOURCE_OPMM:
      {
        if(ucDevDead_OPMM)
        {
#ifdef FIFO_DBG
          printf("\n ACK RECEIVED AFTER 3 MSG TRANSMISSIONS\r\n");
#endif						
        }
        else
        {
          ucTimerEn_OPMM = 0;	 /*Reset device timer enable*/
          ucMsgTxCnt_OPMM = 0; /*Reset msg transmit counter*/
          ucDevDead_OPMM = 0;  /*Set device dead-alive flag to alive*/
          uliTimeoutCnt_OPMM = 0;/*Reset device timeout counter*/
#ifdef FIFO_DBG
          printf("\n ACK RECEIVED SO MSG TRANSMISSION COUNTER = %x\r\n",
                 ucMsgTxCnt_OPMM);
#endif
          
          if (stFifo_OPMM.bFifoEmpty)
          {   
            /*If FIFO is empty*/
#ifdef FIFO_DBG
            printf("\n FIFO IS EMPTY OPMM\r\n\n\n");
#endif
            return (NULL);
          }
          else
          {   
            /* If FIFO is not empty*/
            /*Read one message from fifo into buffer*/						
            fnFifo_Read(&stFifo_OPMM, &stCurMsgTrans_OPMM);
#ifdef FIFO_DBG
            printf("OPMM FIFO RD DATA = 0x%08x",
                   stCurMsgTrans_OPMM.ExtId);
#endif
            /*Check for CAN1 Active or CAN2 for OPMM*/
            //			CAN_Transmit(OPMM_ACTIVE_CAN, &stCurMsgTrans_OPMM);
            if(SET == StatusFlag.stStatusFlag.bCAN2ActiveFlag)
            {
              CAN_Transmit(CAN2, &stCurMsgTrans_OPMM);
            }
            else if(SET == StatusFlag.stStatusFlag.bCAN1ActiveFlag)
            {
              CAN_Transmit(CAN1, &stCurMsgTrans_OPMM);
            }
            ucMsgTxCnt_OPMM++;
            ucTimerEn_OPMM = 1;
#ifdef FIFO_DBG
            printf("\n FIFO IS NOT EMPTY OPMM\r\n");
#endif
          }
        }
      }
      break;
    case SOURCE_VCM:
      {
        if(ucDevDead_VCM)
        {
#ifdef FIFO_DBG
          printf("\n ACK RECEIVED AFTER 3 MSG TRANSMISSIONS\r\n");
#endif						
        }
        else
        {
          ucTimerEn_VCM = 0;  /*Reset device timer enable*/
          ucMsgTxCnt_VCM = 0; /*Reset msg transmit counter*/
          ucDevDead_VCM = 0;  /*Set device dead-alive flag to alive*/
          uliTimeoutCnt_VCM = 0;/*Reset device timeout counter*/
          if (stFifo_VCM.bFifoEmpty)
          {  
            /*If FIFO is empty*/
#ifdef FIFO_DBG
            printf("\n FIFO IS EMPTY VCM\r\n\n\n");
#endif
            return (NULL);
          }
          else
          {   
            /* If FIFO is not empty*/
            /*Read one message from fifo into buffer*/
            fnFifo_Read(&stFifo_VCM, &stCurMsgTrans_VCM);
#ifdef FIFO_DBG
            printf("VCM FIFO RD DATA = 0x%08x",
                   stCurMsgTrans_VCM.ExtId);
#endif
            
            CAN_Transmit(CAN1, &stCurMsgTrans_VCM);
            
            ucMsgTxCnt_VCM++;
            ucTimerEn_VCM = 1;
          }
        }
      }
      break;
    case SOURCE_HCM:
      {
        if(ucDevDead_HCM)
        {
#ifdef FIFO_DBG
          printf("\n ACK RECEIVED AFTER 3 MSG TRANSMISSIONS\r\n");
#endif						
        }
        else
        {
          
          ucMsgTxCnt_HCM = 0;  /*Reset msg transmit counter*/
          ucDevDead_HCM = 0;   /*Set device dead-alive flag to alive*/
          ucTimerEn_HCM = 0;	  /*Reset device timer enable*/
          uliTimeoutCnt_HCM = 0;/*Reset device timeout counter*/
#ifdef FIFO_DBG
          printf("\n ACK RECEIVED SO MSG TRANSMISSION COUNTER   HCM= %x \r\n", ucMsgTxCnt_HCM);
#endif
          
          if (stFifo_HCM.bFifoEmpty)
          {
            //	memset(&stCurMsgTrans_HCM,0, sizeof(CanTxMsg));
            /*If FIFO is empty*/
#ifdef FIFO_DBG
            printf("\n FIFO IS EMPTY\r\n\n\n");
#endif
            
            
            return (NULL);
          }
          else
          {   
            /* If FIFO is not empty*/													
            /*Read one message from fifo into buffer*/
            fnFifo_Read(&stFifo_HCM, &stCurMsgTrans_HCM);
#ifdef FIFO_DBG
            printf("HCM FIFO RD DATA = 0x%08x", 
                   stCurMsgTrans_HCM.ExtId);
#endif
            
            CAN_Transmit(CAN1, &stCurMsgTrans_HCM);
            
            ucMsgTxCnt_HCM++;
            ucTimerEn_HCM = 1;
#ifdef FIFO_DBG
            printf("\nHCM FIFO IS NOT EMPTY\r\n");
#endif
          }
        }		
      }
      break;
    default:
      {
#ifdef FIFO_DBG
        printf("\nAck Recieved from Unknown Device\r\n");
#endif
      }
      break;
    }
  }
  return 1;
}




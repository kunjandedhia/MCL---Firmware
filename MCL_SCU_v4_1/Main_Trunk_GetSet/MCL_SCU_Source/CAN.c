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
CAN_InitTypeDef        CAN_InitStructure;
CAN_FilterInitTypeDef  CAN_FilterInitStructure;

CanRxMsg rgstRxCANBuff[RX_CAN_BUF_SIZE];

uint16_t uiStoreRxCANBuffCnt = 0;
uint16_t uiCANProcessCnt = 0;


uint8_t ucMsgTxCnt_OP = 0;    /* OP msg transmit counter*/
uint32_t uliTimeoutCnt_OP; 		/* 250ms OP timeout counter*/
uint8_t ucTimerEn_OP;					/* Op timeout counter enable*/
uint8_t ucDevDead_OP; 				/*OP dead signal*/
stTxMsgFlags pstFifo_OP; 			/*CanTxDataFlags pstFifo_OP; name for Fifo struct*/
CanTxMsg pstCurMsgTrans_OP;   /*pstCurMsgTrans_OP-current msg under transmission*/

uint8_t ucMsgTxCnt_OPMM = 0;  /*OPMM msg transmit counter*/
uint32_t uliTimeoutCnt_OPMM;  /*250ms OPMM timeout counter*/
uint8_t ucTimerEn_OPMM;				/* OPMM timeout counter enable*/
uint8_t ucDevDead_OPMM; 			/*OPMM dead signal*/
stTxMsgFlags pstFifo_OPMM; 		/*CanTxDataFlags pstFifo_OPMM; name for Fifo structure*/
CanTxMsg pstCurMsgTrans_OPMM; /*pstCurMsgTrans_OPMM - current msg under transmission*/

uint8_t ucMsgTxCnt_MFM = 0;   /*MFM msg transmit counter*/
uint32_t uliTimeoutCnt_MFM;   /*250ms MFM timeout counter*/
uint8_t ucTimerEn_MFM;			  /*MFM timeout counter enable*/
uint8_t ucDevDead_MFM; 		 	  /*MFM dead signal */
stTxMsgFlags pstFifo_MFM;		  /*CanTxDataFlags pstFifo_MFM; name for Fifo structure*/
CanTxMsg pstCurMsgTrans_MFM;  /*pstCurMsgTrans_MFM - current msg under transmission*/

uint8_t ucMsgTxCnt_BHCM = 0;  /*BHCM msg transmit counter*/
uint32_t uliTimeoutCnt_BHCM;  /*250ms BHCM timeout counter*/
uint8_t ucTimerEn_BHCM;       /* BHCM timeout counter enable*/
uint8_t ucDevDead_BHCM;       /*BHCM dead signal*/
stTxMsgFlags pstFifo_BHCM;    /*CanTxDataFlags pstFifo_BHCM; name for Fifo structure*/
CanTxMsg pstCurMsgTrans_BHCM; /*pstCurMsgTrans_BHCM - current msg under transmission*/

uint8_t ucMsgTxCnt_HHCM = 0;  /*HHCM msg transmit counter*/
uint32_t uliTimeoutCnt_HHCM;  /*250ms HHCM timeout counter*/
uint8_t ucTimerEn_HHCM;       /* HHCM timeout counter enable*/
uint8_t ucDevDead_HHCM;       /*HHCM dead signal*/
stTxMsgFlags pstFifo_HHCM;    /*CanTxDataFlags pstFifo_HHCM; name for Fifo structure*/
CanTxMsg pstCurMsgTrans_HHCM; /*pstCurMsgTrans_HHCM - current msg under transmission*/

uint8_t ucMsgTxCnt_IHCM1 = 0; /*IHCM1 msg transmit counter*/
uint32_t uliTimeoutCnt_IHCM1; /*250ms IHCM1 timeout counter*/
uint8_t ucTimerEn_IHCM1;      /* IHCM1 timeout counter enable*/
uint8_t ucDevDead_IHCM1;      /*IHCM1 dead signal*/
stTxMsgFlags pstFifo_IHCM1;   /*CanTxDataFlags pstFifo_IHCM1; name for Fifo structure*/
CanTxMsg pstCurMsgTrans_IHCM1;/*pstCurMsgTrans_IHCM1 - current msg under transmission*/

/*IHCM4_HM Device related Flags */
uint8_t ucMsgTxCnt_IHCM4_HM = 0;  /*msg transmit counter*/
uint32_t uliTimeoutCnt_IHCM4_HM;  /*250ms timeout counter*/
uint8_t ucTimerEn_IHCM4_HM;       /*Timeout Counter enable*/
uint8_t ucDevDead_IHCM4_HM;       /*Device dead signal*/
stTxMsgFlags pstFifo_IHCM4_HM;    /*FIFO Structure*/
CanTxMsg pstCurMsgTrans_IHCM4_HM; /*MsgTrans - current message under transmission*/


/* DATA_LOGGING*/
uint8_t ucStatusByte; 			/*Status Log Byte contains bitwise logs*/
uint8_t ucFaultByte0;			  /*FAULT Log Byte Zero contains bitwise logs*/
uint8_t ucFaultByte1; 			/*FAULT Log Byte One contains bitwise logs*/
uint8_t ucFaultByte2; 			/*FAULT Log Byte Two contains bitwise logs*/
uint8_t ucFaultByte3; 			/*FAULT Log Byte Three contains bitwise logs*/
uint8_t rgucStatusLog[81];  /*Global Array For DATA LOGGING of STATUS LOG*/
uint8_t rgucFaultLog[2];    /*Local Array For DATA LOGGING of FAULT LOG*/
uint8_t rgucPeriodicLog[89];/*Global Array For DATA LOGGING of PERIODIC LOG*/

SplitByte_TypeDef SplitByte; /*Union Variable Used for Byte splitting or conversion*/

CurrentData_Typedef stCurrentData;/*Structure Variable for Current data*/
ConfigData_Typedef stConfigData;  /*Structure Variable for Config data*/

#ifdef DBG_CAN
uint32_t uiAckCount = 0; 
uint32_t uliTxHbCount = 0;
#endif
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

/*****************************************************************************
**@Function      : fnCAN_Config
**@Descriptions  : Configures the CAN Peripheral.
**@parameters    : CANx: where x can be 1 or 2 to select the CAN peripheral.
**@Return        : None
*****************************************************************************/
void fnCAN_Config(CAN_TypeDef* CANx)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  NVIC_InitTypeDef  NVIC_InitStructure;
  /* CAN GPIOs configuration **************************************************/
  /* Enable GPIO clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB |
                         RCC_AHB1Periph_GPIOH |
                           RCC_AHB1Periph_GPIOI,
                           ENABLE);
  
  /*Initialize pins for CAN1 */
  if(CANx == CAN1)
  {
    /* Enable CAN 1clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
    
    /*PI9 as CAN1_RX & PH13 as CAN1_TX */
    GPIO_PinAFConfig(GPIOI, GPIO_PinSource9, GPIO_AF_CAN1);
    GPIO_PinAFConfig(GPIOH, GPIO_PinSource13, GPIO_AF_CAN1);
    
    /* Configure CAN1 RX */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(GPIOI, &GPIO_InitStructure);
    
    /* Configure CAN1 TX */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_Init(GPIOH, &GPIO_InitStructure);
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
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
  }
  /* CAN configuration ********************************************************/
  
  /* CAN register init */
  CAN_DeInit(CANx);
  
  /* CAN cell init */
  CAN_InitStructure.CAN_TTCM = DISABLE;
  CAN_InitStructure.CAN_ABOM = DISABLE;
  CAN_InitStructure.CAN_AWUM = DISABLE;
  CAN_InitStructure.CAN_NART = DISABLE;
  CAN_InitStructure.CAN_RFLM = DISABLE;
  CAN_InitStructure.CAN_TXFP = DISABLE;
  CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
  
  /* CAN Baudrate = 125Kbps (CAN clocked at 42 MHz) */
  CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
  CAN_InitStructure.CAN_BS1 = CAN_BS1_12tq;
  CAN_InitStructure.CAN_BS2 = CAN_BS2_8tq;
  CAN_InitStructure.CAN_Prescaler = 16;
  CAN_Init(CANx, &CAN_InitStructure);
  
  if(CANx == CAN1)
  {
    /* CAN filter init : No filter */
    CAN_FilterInitStructure.CAN_FilterNumber = 0;
    CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
    CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
    CAN_FilterInitStructure.CAN_FilterIdHigh = 0;
    CAN_FilterInitStructure.CAN_FilterIdLow =0x0200;
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0xFF00;
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
    CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
    CAN_FilterInit(&CAN_FilterInitStructure);
    /* CAN filter init : No filter */
    CAN_FilterInitStructure.CAN_FilterNumber = 1;
    CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
    CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
    CAN_FilterInitStructure.CAN_FilterIdHigh = 0;
    CAN_FilterInitStructure.CAN_FilterIdLow =0xFF00;
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0xFF00;
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
    CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
    CAN_FilterInit(&CAN_FilterInitStructure);
  }
  else if(CANx == CAN2)
  {
    CAN_SlaveStartBank(15);
    /* CAN filter init : No filter */
    CAN_FilterInitStructure.CAN_FilterNumber = 16;
    CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
    CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
    CAN_FilterInitStructure.CAN_FilterIdHigh = 0;
    CAN_FilterInitStructure.CAN_FilterIdLow = 0x0200;
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0xFF00;
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
    CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
    CAN_FilterInit(&CAN_FilterInitStructure);
    
    /* CAN filter init : No filter */
    CAN_FilterInitStructure.CAN_FilterNumber = 17;
    CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
    CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
    CAN_FilterInitStructure.CAN_FilterIdHigh = 0;
    CAN_FilterInitStructure.CAN_FilterIdLow = 0xFF00;
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0xFF00;
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
    CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
    CAN_FilterInit(&CAN_FilterInitStructure);
  }
  
  if(CANx == CAN1)
  {
    NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
  }
  else if(CANx == CAN2)
  {
    NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
  }
  
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* Enable FIFO 0 message pending Interrupt */
  CAN_ITConfig(CANx, CAN_IT_FMP0 | CAN_IT_FF0 | CAN_IT_FOV0, ENABLE);
}


/*****************************************************************************
**@Function      : fnCANMsg_Validate
**@Descriptions  : Validate received can message as per the Device ID
**@parameters    : None
**@Return        : None
*****************************************************************************/
void fnCANMsg_Validate(CanRxMsg *pstCANrxmsg)
{
#ifdef DBG_CAN
  DEBUG_PRINTF("In fnCANMsg_Process \n\r");
#endif
  uint32_t uliCanID = pstCANrxmsg->ExtId;
  
#ifdef DBG_CAN
  DEBUG_PRINTF("uliCanID = 0x%x \n\r", uliCanID);
#endif
  
  /************Destination Addr*************/
  switch  (uliCanID & DEST_ADDR_MASK)
  {
  case DEST_ALL:
    {
#ifdef DBG_CAN
      DEBUG_PRINTF("Broad Cast Message..\r\n");
#endif
      /*message is for MCU so precess it*/
      fnCANMsg_Process(pstCANrxmsg, PROCESS_CAN_MSG);
    }
    break;
    
  case DEST_MCU:
    {
#ifdef DBG_CAN
      DEBUG_PRINTF("Msg for MCU..\r\n");
#endif
      /*message is for MCU so precess it*/
      fnCANMsg_Process(pstCANrxmsg, PROCESS_CAN_MSG);
    }
    break;
    
  default:
    {
#ifdef DBG_CAN
      DEBUG_PRINTF("Error Frame..\r\n");
#endif
    }
    break;
  }
}



/*****************************************************************************
**@Function 			: fnCANMsg_Process
**@Descriptions  : Process or farward received can message
**@parameters    : ucProcess: 0: message is for MCU  ; so process it
1: <reserved> message is not for MCU so forward it to other CAN bus
**@Return        : None
*****************************************************************************/
void fnCANMsg_Process(CanRxMsg *pstCANrxmsg, uint8_t ucProcess)
{
#ifdef GENERAL_DBG
  DEBUG_PRINTF("In fnCANMsg_Process ExtId : 0x%08x..\r\n", pstCANrxmsg->ExtId);
#endif
  
#ifdef DBG_CAN
  char  rgcTemp[150];
#endif
  
  uint8_t ucRec_Data[MAX_DATA_LENGTH];
  
  uint32_t uliCanID = pstCANrxmsg->ExtId;
  
  for(ucLoopCnt = 0; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
  {
    /*ucRec_Data is same as of ucRec_Byte of OP module*/
    ucRec_Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
  }
  
  if(ucProcess == PROCESS_CAN_MSG)
  {
    if((pstCANrxmsg->ExtId & ACKNOWLEDGEMENT) != ACKNOWLEDGEMENT)
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
#ifdef DBG_CAN
            DEBUG_PRINTF("Msg ID: CMD_ESTOP \r\n");
#endif
            StatusFlag.stStatusFlag.bEStop_Flag = SET;
            /**************************/
            /* Emergency Stop Process */
            /**************************/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & 0xFFFFFF00) | SOURCE_MCU);
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            if(SET == StatusFlag.stStatusFlag.bCAN2ActiveFlag)
            {
              fnCANMsg_Transmit(&stTx1Message, CAN2);
            }
#ifdef DBG_CAN
            DEBUG_PRINTF("Msg ID: BCAST_ESTOP From OP\r\n");
#endif
          }
          break;
          
        case ACKNOWLEDGEMENT:
          {
            /* Check For Heartbeat acknowledgement */
#ifdef DBG_CAN
            DEBUG_PRINTF("ACKNOWLEDGEMENT From IHCM1 COUNT: %d \r\n", ++uiAckCount);
#endif
            switch (uliCanID & SOURCE_ADDR_MASK)
            {
            case SOURCE_OP:
              {
#ifdef DBG_CAN
                DEBUG_PRINTF("HB from SOURCE_OP \r\n");
#endif
                if((POSITIVE_ACK == pstCANrxmsg->Data[0]) && rgucHBCntr[OP] > 0)
                {
                  /*Decreament the counter by 1 to indicate that the
                  HB ACK rcvd successfully*/
                  rgucHBCntr[OP] = 0; 
                  
                  uiHBACKfail &= ~(1 << OP); /*Clear HB ACK fault  if any*/
                  
                  uiHBFailSend &= ~(1 << OP); /*RESET the HB Fail send VAriable*/
                  ucDevDead_OP = 0;
                }
                else if(FIRST_BOOTUP_ACK == pstCANrxmsg->Data[0])
                {
                  uiDeviceFuncSend |= (1 << OP);
                  ucDevDead_OP = 0;
                  fnDeviceFunctionality(DEST_OP); 
                }
              }
              break;
              
            case SOURCE_OPMM:
              {
#ifdef DBG_CAN
                DEBUG_PRINTF("HB from SOURCE_OPMM \r\n");
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
                  uiDeviceFuncSend |= (1 << OPMM);
                  ucDevDead_OPMM = 0;
                  fnDeviceFunctionality(DEST_OPMM); 
                }
              }
              break;
              
            case SOURCE_MFM:
              {
#ifdef DBG_CAN
                DEBUG_PRINTF("HB from SOURCE_MFM \r\n");
#endif
                
                if((POSITIVE_ACK == pstCANrxmsg->Data[0]) && rgucHBCntr[MFM] > 0)
                {
                  /*Decreament the counter by 1 to indicate that
                  the HB ACK rcvd successfully */
                  rgucHBCntr[MFM] =0;  
                  uiHBACKfail &= ~(1 << MFM);  /* Clear HB ACK fault  if any*/
                  uiHBFailSend &= ~(1 << MFM); /*RESET the HB Fail send VAriable*/
                  ucDevDead_MFM = 0;
                }
                
                if(FIRST_BOOTUP_ACK == pstCANrxmsg->Data[0])
                {
                  uiDeviceFuncSend |= (1 << MFM);
                  ucDevDead_MFM = 0; 
                  fnDeviceFunctionality(DEST_MFM); 
                }
              }
              break;
              
            case SOURCE_BHCM:
              {
#ifdef DBG_CAN
                DEBUG_PRINTF("HB from SOURCE_BHCM \r\n");
#endif
                
                if((POSITIVE_ACK == pstCANrxmsg->Data[0]) && rgucHBCntr[BHCM] > 0)
                {
                  /* Decreament the counter by 1 to indicate that 
                  the HB ACK rcvd successfully */
                  rgucHBCntr[BHCM]= 0; 
                  uiHBACKfail &= ~(1 << BHCM);  /* Clear HB fault  if any*/
                  uiHBFailSend &= ~(1 << BHCM); /*RESET the HB Fail send VAriable*/
                }
                
                if(FIRST_BOOTUP_ACK == pstCANrxmsg->Data[0])
                {
                  uiDeviceFuncSend |= (1 << BHCM);
                  fnDeviceFunctionality(DEST_BHCM); 
                }
              }
              break;
              
            case SOURCE_HHCM:
              {
#ifdef DBG_CAN
                DEBUG_PRINTF("HB from SOURCE_HHCM \r\n");
#endif
                if((POSITIVE_ACK == pstCANrxmsg->Data[0]) && (rgucHBCntr[HHCM] > 0))
                {
                  /*-= 1;   Decreament the counter by 1 to indicate that 
                  the HB ACK rcvd successfully */
                  rgucHBCntr[HHCM] = 0; 
                  uiHBACKfail &= ~(1 << HHCM);  /* Clear HB fault  if any*/
                  uiHBFailSend &= ~(1 << HHCM); /*RESET the HB Fail send VAriable*/
                }
                
                if(FIRST_BOOTUP_ACK == pstCANrxmsg->Data[0])
                {
                  uiDeviceFuncSend |= (1 << HHCM);
                  fnDeviceFunctionality(DEST_HHCM); 
                }
              }
              break;
              
            case SOURCE_IHCM1:
              {
#ifdef DBG_CAN
                DEBUG_PRINTF("HB from SOURCE_IHCM1 \r\n");
#endif
                if((POSITIVE_ACK == pstCANrxmsg->Data[0]) && rgucHBCntr[IHCM1] > 0)
                {
                  /*Decreament the counter by 1 to indicate that 
                  the HB ACK rcvd successfully */
                  rgucHBCntr[IHCM1] = 0; 
                  uiHBACKfail &= ~(1 << IHCM1);  /* Clear HB fault  if any*/
                  uiHBFailSend &= ~(1 << IHCM1); /*RESET the HB Fail send VAriable*/
                }
                
                if(FIRST_BOOTUP_ACK == pstCANrxmsg->Data[0])
                {
                  uiDeviceFuncSend |= (1 << IHCM1);
                  fnDeviceFunctionality(DEST_IHCM1); 
                }
              }
              break;
              
            case SOURCE_IHCM4_HM:
              {
#ifdef DBG_CAN
                DEBUG_PRINTF("HB from SOURCE_IHCM4_HM \r\n");
#endif
                
                if((POSITIVE_ACK == pstCANrxmsg->Data[0]) && rgucHBCntr[IHCM4_HM] > 0)
                {
                  /* Decreament the counter by 1 to indicate that 
                  the HB ACK rcvd successfully */
                  rgucHBCntr[IHCM4_HM] = 0; 
                  uiHBACKfail &= ~(1 << IHCM4_HM);  /* Clear HB fault  if any*/
                  uiHBFailSend &= ~(1 << IHCM4_HM); /*RESET the HB Fail send VAriable*/
                }
                
                if(FIRST_BOOTUP_ACK == pstCANrxmsg->Data[0])
                {
                  uiDeviceFuncSend |= (1 << IHCM4_HM);
                  fnDeviceFunctionality(DEST_IHCM4_HM);
                }
              }
              break;
              
            default:
              {
#ifdef DBG_CAN
                DEBUG_PRINTF("HB Ack from Unknown Device\r\n");
#endif
              }
              break;
            }
          }
          break;
          
        default:
          {
            
          }break;
        }
      }
      break;/*Briadcast Msgs Case ends*/
      
    case MSG_TYPE_COMMAND:
      {
#ifdef DBG_CAN
        DEBUG_PRINTF("Msg Type: MSG_TYPE_COMMAND \r\n");
#endif
        
        switch (uliCanID & MSG_MASK)
        {
        case ACKNOWLEDGEMENT:
          {
            fnCANPost_Ack(pstCANrxmsg,CAN1);
          }
          break;
          
        case CMD_SET_SYSTEM_MODE:
          {
#ifdef DBG_CAN
            DEBUG_PRINTF("Msg ID: CMD_SET_SYSTEM_MODE \r\n");
#endif
            
            if((ucRec_Data[DATA_BYTE_ZERO]) == CAN_MSG_WARM_UP_MODE)
            {
              StatusFlag.stStatusFlag.bWarm_Mod = 0;
              if(MCU_OPERATION_MODE == ucMCUMode)
              {
                StatusFlag.stStatusFlag.bWarmUpOverFlag = RESET;
                StatusFlag.stStatusFlag.bWarmUpModeCmdFlag = RESET;
                ucMCUMode = MCU_WARMUP_MODE;
                stTx1Message.ExtId = MSG_TYPE_STATUS | STATUS_SYSTEM_MODE |
                  DEST_OP | SOURCE_MCU;
                stTx1Message.DLC = 0x01;
                stTx1Message.Data[0] = 0x00; /*System Operation Mode Over*/
                fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef PROCESS_DBG
                DEBUG_PRINTF("Msg ID: SYSTEM_WARM_UP_MODE_SET \r\n");
#endif
              }		
            }
            else if (CAN_MSG_OPERATION_MODE == ucRec_Data[DATA_BYTE_ZERO])
            {
              StatusFlag.stStatusFlag.bOp_Mod = SET;
              if(SET == StatusFlag.stStatusFlag.bWarmUpOverFlag)
              {
                StatusFlag.stStatusFlag.bWarmUpOverFlag = RESET;/* Re-Initialze */
                StatusFlag.stStatusFlag.bWarmUpModeCmdFlag = RESET;
                ucMCUMode = MCU_OPERATION_MODE;
#ifdef PROCESS_DBG
                DEBUG_PRINTF("Msg ID: SYSTEM_OPERATION_MODE_SET \r\n");
#endif
              }
            }
            else if (CAN_MSG_PAUSE_MODE == ucRec_Data[DATA_BYTE_ZERO])
            {
              StatusFlag.stStatusFlag.bPause_Mod = 1;
              ucMCUMode = MCU_PAUSE_MODE;
#ifdef PROCESS_DBG
              DEBUG_PRINTF("Msg ID: SYSTEM_PAUSE_MODE_SET \r\n");
#endif
            }
            else if ((ucRec_Data[DATA_BYTE_ZERO] & BIT1_MASK) == BIT1_RESET)
            {
              StatusFlag.stStatusFlag.bPause_Mod = 0;
#ifdef PROCESS_DBG
              DEBUG_PRINTF("Msg ID: SYSTEM_PAUSE_MODE_RESET \r\n");
#endif
            }
            else
            {
#ifdef PROCESS_DBG
              DEBUG_PRINTF("Msg ID: SYSTEM_ERROR_MODE \r\n");
#endif
            }
          }
          break;
          
        case CMD_START_PAUSE:
          {
#ifdef DBG_CAN
            DEBUG_PRINTF("Msg ID: CMD_START_PAUSE \r\n");
#endif
            if((ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK) == CMD_START_OPERATION)
            {
              StatusFlag.stStatusFlag.bStart = 1;
#ifdef DBG_CAN
              DEBUG_PRINTF("Msg ID: CMD_START_OPERATION \r\n");
#endif
            }
            else if ((ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK) == CMD_PAUSE_OPERATION)
            {
              StatusFlag.stStatusFlag.bStart = 0;
#ifdef DBG_CAN
              DEBUG_PRINTF("Msg ID: CMD_PAUSE_OPERATION \r\n");
#endif
            }
            else
            {
#ifdef DBG_CAN
              DEBUG_PRINTF("Msg ID: CMD_START_PAUSE_ERROR \r\n");
#endif
            }
          }
          break;
          
        case CMD_SPRAY_SELECTOR:
          {
            if(SET == StatusFlag.stStatusFlag.bPrimeSelector)
            {
              /*Send MFM Command to STOP Monitoring the Material used*/ 
              stTx1Message.ExtId = MSG_TYPE_COMMAND | CMD_LINEAR_TRANS_MONITOR |
                DEST_MFM | SOURCE_MCU;
              stTx1Message.DLC = 0x01;
              stTx1Message.Data[0] = 0x02; /*Stop Monitoring the Material Used*/
            }
            /*RESET PRIME PROCESS FLAGS*/
            StatusFlag.stStatusFlag.bPrimePressureAchieve = RESET;
            StatusFlag.stStatusFlag.bRecirculationValveOFFCmdFlag = RESET;
            
            /*RESET FLUSH PROCESS FLAGS*/	
            StatusFlag.stStatusFlag.bTemp3Flag = SET;
            StatusFlag.stStatusFlag.bTemp2Flag = SET;
            
            /*Process Flags*/
            StatusFlag.stStatusFlag.bSpraySelector = SET;
            StatusFlag.stStatusFlag.bFlushSelector = RESET;
            StatusFlag.stStatusFlag.bPrimeSelector = RESET;
            
            if(BIT1_SET == ucRec_Data[DATA_BYTE_ZERO])
            {
              StatusFlag.stStatusFlag.bSingleCoatFlag = SET;
#ifdef PROCESS_DBG
              DEBUG_PRINTF("Msg ID: CMD_SPRAY_SELECTOR Single COAT\r\n");
#endif
            }
            else if(BIT0_SET == ucRec_Data[DATA_BYTE_ZERO])
            {
              StatusFlag.stStatusFlag.bSingleCoatFlag = RESET;
#ifdef PROCESS_DBG
              DEBUG_PRINTF("Msg ID: CMD_SPRAY_SELECTOR AUTO COAT\r\n");
#endif
            }
          }
          break;
          
        case CMD_SPRAY_START:
          {
#ifdef PROCESS_DBG
            DEBUG_PRINTF("Msg ID: CMD_SPRAY_START \r\n");
#endif
            if(SET == StatusFlag.stStatusFlag.bSprayPosition) /*SET in Parser*/
            {
#ifdef PROCESS_DBG
              DEBUG_PRINTF("Gun Position at Spray send Spray Start cmd\r\n");
#endif
              StatusFlag.stStatusFlag.bSprayStartCmd = SET;							
            }	
          }
          break;
          
        case CMD_PRIME_SELECTOR:
          {
            /*RESET FLUSH PROCESS FLAGS*/	
            StatusFlag.stStatusFlag.bTemp3Flag = RESET;/*To Avoid Multiple time sending of these commands*/
            StatusFlag.stStatusFlag.bTemp2Flag = RESET;							
            
            /*RESET PRIME PROCESS FLAGS*/
            StatusFlag.stStatusFlag.bPrimePressureAchieve = RESET;
            StatusFlag.stStatusFlag.bRecirculationValveOFFCmdFlag = RESET;
            
            /*PROCESS Init Flags*/
            StatusFlag.stStatusFlag.bPrimeSelector = SET;
            StatusFlag.stStatusFlag.bFlushSelector = RESET;
            StatusFlag.stStatusFlag.bOperationInProcess = SET; /*This is used for Operation time out error*/
            uiOperationTimeOut = 0;
#ifdef PROCESS_DBG
            DEBUG_PRINTF("Msg ID: CMD_PRIME_SELECTOR \r\n");
#endif
          }
          break;
          
        case CMD_PRIME_START:
          {
#ifdef PROCESS_DBG
            DEBUG_PRINTF("Msg ID: CMD_PRIME_START \r\n");
#endif
            
            if(BIT0_SET == ucRec_Data[DATA_BYTE_ZERO])
            {
              StatusFlag.stStatusFlag.bAutoPrimeStart = SET;
              StatusFlag.stStatusFlag.bManualPrimeStart = RESET;
#ifdef PROCESS_DBG
              DEBUG_PRINTF("Msg ID: CMD_PRIME_START_AUTO_SET \r\n");
#endif
              
            }
            
            
            
            if (BIT1_SET == ucRec_Data[DATA_BYTE_ZERO])
            {
              StatusFlag.stStatusFlag.bManualPrimeStart = SET;
              StatusFlag.stStatusFlag.bAutoPrimeStart = RESET;
#ifdef PROCESS_DBG
              DEBUG_PRINTF("Msg ID: CMD_PRIME_START_MANUAL_SET \r\n");
#endif
            }
            
            
          }
          break;
          
        case CMD_FLUSH_SELECTOR:
          {
            if(SET == StatusFlag.stStatusFlag.bPrimeSelector)
            {
              /*Send MFM Command to start Monitoring the Material used*/ 
              stTx1Message.ExtId = MSG_TYPE_COMMAND | CMD_LINEAR_TRANS_MONITOR | 
                DEST_MFM | SOURCE_MCU;
              stTx1Message.DLC = 0x01;
              stTx1Message.Data[0] = 0x02; /*Stop Monitoring the Material Used*/
            }
            
            /*RESET FLUSH PROCESS FLAGS*/	
            StatusFlag.stStatusFlag.bTemp3Flag = RESET;
            StatusFlag.stStatusFlag.bTemp2Flag = RESET;	
            
            /*RESET PRIME PROCESS FLAGS*/
            StatusFlag.stStatusFlag.bPrimePressureAchieve = RESET;
            StatusFlag.stStatusFlag.bRecirculationValveOFFCmdFlag = RESET;
            
            /*Process Flags*/
            StatusFlag.stStatusFlag.bFlushSelector = SET;
            StatusFlag.stStatusFlag.bPrimeSelector = RESET;
#ifdef PROCESS_DBG
            DEBUG_PRINTF("Msg ID: CMD_FLUSH_SELECTOR \r\n");
#endif
            
          }
          break;
          
        case CMD_FLUSH_START:
          {
            
#ifdef PROCESS_DBG
            DEBUG_PRINTF("Msg ID: CMD_FLUSH_START \r\n");
#endif
            
            if((ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK) == BIT0_SET)
            {
              StatusFlag.stStatusFlag.bStartFlush_Auto = 1;
#ifdef PROCESS_DBG
              DEBUG_PRINTF("Msg ID: CMD_FLUSH_START_AUTO_SET \r\n");
#endif
            }
            else if((ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK) == BIT0_RESET)
            {
              StatusFlag.stStatusFlag.bStartFlush_Auto = 0;
#ifdef PROCESS_DBG
              DEBUG_PRINTF("Msg ID: CMD_FLUSH_START_AUTO_RESET \r\n");
#endif
              
            }
            if ((ucRec_Data[DATA_BYTE_ZERO] & BIT1_MASK) == BIT1_SET)
            {
              StatusFlag.stStatusFlag.bStartFlush_Manu = 1;
#ifdef PROCESS_DBG
              DEBUG_PRINTF("Msg ID: CMD_FLUSH_START_MANUAL_SET \r\n");
#endif
              
            }
            else if ((ucRec_Data[DATA_BYTE_ZERO] & BIT1_MASK) == BIT1_RESET)
            {
              StatusFlag.stStatusFlag.bStartFlush_Manu = 0;
#ifdef PROCESS_DBG
              DEBUG_PRINTF("Msg ID: CMD_FLUSH_START_MANUAL_RESET \r\n");
#endif
            }
          }
          break;
          
        case CMD_STALL_TEST:
          {
            StatusFlag.stStatusFlag.bTemp5Flag = RESET;
#ifdef PROCESS_DBG
            DEBUG_PRINTF("Msg ID: CMD_STALL_TEST \r\n");
#endif
            if(BIT0_SET == ucRec_Data[DATA_BYTE_ZERO])
            {
              StatusFlag.stStatusFlag.bStallTestAuto = SET;
              /*This is used for Operation time out error*/
              StatusFlag.stStatusFlag.bOperationInProcess = RESET; 
              
#ifdef PROCESS_DBG
              DEBUG_PRINTF("Msg ID: CMD_STALL_AUTOMATIC_TEST_SET \r\n");
#endif
            }
            if(BIT1_SET == ucRec_Data[DATA_BYTE_ZERO])
            {
              StatusFlag.stStatusFlag.bStStallTestManual = SET;
#ifdef PROCESS_DBG
              DEBUG_PRINTF("Msg ID: CMD_STALL_MANUAL_TEST_SET \r\n");
#endif
            }
            StatusFlag.stStatusFlag.bFlushSelector = RESET;
            StatusFlag.stStatusFlag.bPrimeSelector = RESET;
          }
          break;
          
          
        case CMD_LASER_BEAM:
          {
#ifdef DBG_CAN
            DEBUG_PRINTF("Msg ID: CMD_LASER_BEAM \r\n");
#endif
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & 0xFFFF0000) | 
                                  (DEST_OPMM | SOURCE_MCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            fnCANMsg_Transmit(&stTx1Message, CAN1);
          }
          break;
          
          
        case CMD_VALVE_OPMM:
          {
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & 0xFFFF0000) | 
                                  (DEST_OPMM | SOURCE_MCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CAN
            DEBUG_PRINTF("Msg ID: CMD_VALVE_OPMM Frome OP\r\n");
#endif
          }
          break;
          
        case CMD_OPMM_CONFIG:
          {
            uiDeviceConfigSent |= (1 << OPMM);
            fnSendOPMMConfig();
#ifdef DBG_CAN
            DEBUG_PRINTF("Msg ID: CMD_OPMM_CONFIG \r\n");
#endif
          }
          break;
          
        case CMD_MFM_CONFIG:
          {
            fnSendMFMConfig();
            
#ifdef DBG_CAN
            DEBUG_PRINTF("Msg ID: CMD_MFM_CONFIG \r\n");
#endif
          }
          break;
          
        case CMD_BHCM_CONFIG:
          {				
            uiDeviceConfigSent |= (1 << BHCM);
            fnSendBHCMConfig();
#ifdef DBG_CAN
            DEBUG_PRINTF("Msg ID: CMD_BHCM_CONFIG \r\n");
#endif
          }
          break;
          
        case CMD_IHCM_CONFIG:
          {
#ifdef DBG_CAN
            DEBUG_PRINTF("Msg ID: CMD_IHCM1_CONFIG \r\n");
#endif
            uiDeviceConfigSent |= (1 << IHCM1);
            fnSendIHCM1Config();
          }
          break;
          
        case CMD_HHCM_CONFIG:
          {
#ifdef DBG_CAN
            DEBUG_PRINTF("Msg ID: CMD_HHCM_CONFIG \r\n");
#endif
            if(SOURCE_IHCM4_HM == (uliCanID & SOURCE_ADDR_MASK))
            {
              fnSendIHCM4HMConfig();
              uiDeviceConfigSent |= (1 << IHCM4_HM);
#ifdef DBG_CAN
              DEBUG_PRINTF("Msg ID: CMD_HARDENER_IHCM4_CONFIG \r\n");
#endif
            }
            else if(SOURCE_HHCM == (uliCanID & SOURCE_ADDR_MASK))
            {
              uiDeviceConfigSent |= (1 << HHCM);
              fnSendHHCMConfig();
#ifdef DBG_CAN
              DEBUG_PRINTF("Msg ID: CMD_HHCM_CONFIG \r\n");
#endif
            }
          }
          break;
          
        case CMD_CLEAR_FAULT:
          {
#ifdef DBG_CAN
            DEBUG_PRINTF("Msg ID: CMD_CLEAR_FAULT \r\n");
#endif
            StatusFlag.stStatusFlag.bClearFault = 1;
            fnAllFaults_Clear();
          }
          break;
          
        case CMD_CLEAR_WARNING:
          {
#ifdef DBG_CAN
            DEBUG_PRINTF("Msg ID: CMD_CLEAR_WARNING \r\n");
#endif
            StatusFlag.stStatusFlag.bClearWarning = 1;
            fnAllWarnnings_Clear(pstCANrxmsg->Data[0]);
          }
          break;
          
        case CMD_MANUAL_PROCESS_DONE:
          {
#ifdef PROCESS_DBG
            DEBUG_PRINTF("Msg ID: CMD_MANUAL_PROCESS_DONE \r\n");
#endif							
            stTx1Message.DLC = pstCANrxmsg->DLC;
            stTx1Message.Data[0] = pstCANrxmsg->Data[0];
            SplitByte.uliData = pstCANrxmsg->ExtId;
            SplitByte.rgucData[0] = (uint8_t)(SOURCE_MCU & 0xff);
            SplitByte.rgucData[1] = (uint8_t)(DEST_OPMM >> 8);
            stTx1Message.ExtId = SplitByte.uliData;
            fnCANMsg_Transmit(&stTx1Message, CAN1);
            /*FLUSH COMPLETE COMMAND*/
            if(FLUSH_COMPLETE == pstCANrxmsg->Data[0])
            {
              /*This is used for Operation time out error*/
              StatusFlag.stStatusFlag.bOperationInProcess = RESET;
              
              /*This Flag bypass the auto stall and System is ready for Operation*/
              if(MCU_OPERATION_MODE == ucMCUMode)
              {
                StatusFlag.stStatusFlag.bOperationModeOver = SET;
              }
            }
          }
          break;
          
        case CMD_STOP_RUNTIME:
          {
            StatusFlag.stStatusFlag.bStopRuntimeReceived = SET;
          }
          break;
        }
      }
      break;/*Command Msgs Case ends*/
      
    case MSG_TYPE_FAULT:
      {
        switch (uliCanID & MSG_MASK)
        {
        case ACKNOWLEDGEMENT:
          {
            fnCANPost_Ack(pstCANrxmsg,CAN1);
          }
          break;
          
        case FAULT_OPMM_VALVE:
          {
#ifdef DBG_CAN
            DEBUG_PRINTF("Msg ID: FAULT_OPMM_VALVE \r\n");
#endif
            if((ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK) == BIT0_SET)
            {
#ifdef DBG_CAN
              DEBUG_PRINTF("Msg ID:  FAULT_OPMM_VALVE_ON\r_\n");
#endif
            }
            else if((ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK) == BIT0_RESET)
            {
#ifdef DBG_CAN
              DEBUG_PRINTF("Msg ID:  FAULT_OPMM_VALVE_OFF\r_\n");
#endif
            }
            if(!(((ucRec_Data[DATA_BYTE_ONE] & 0x80) == 0x80) ||
                 ((ucRec_Data[DATA_BYTE_ONE] & 0x40) == 0x40)))
            {
              StatusFlag.stStatusFlag.bWarnningFlag = SET; /*Warnning SET*/
            }
            
            /*Send to OP*/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & 0xFFFF0000) | 
                                  (DEST_OP | SOURCE_MCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            fnCANMsg_Transmit(&stTx1Message, CAN1);
            /* FAULT BYTE UPDATED and WRITTEN in NVSRAM */
            ucFaultByte0 = (ucFaultByte0 | BIT1_SET);  
            fnFault_Log();
          }
          break;
          
        case FAULT_MFM_VALVE:
          {
#ifdef DBG_CAN
            DEBUG_PRINTF("Msg ID: FAULT_MFM_VALVE \r\n");
#endif
            StatusFlag.stStatusFlag.bWarnningFlag = SET;/*Warnning SET*/
            if((ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK) == BIT0_SET)
            {
#ifdef DBG_CAN
              DEBUG_PRINTF("Msg ID:  FAULT_MFM_VALVE_ON\r_\n");
#endif
              
            }
            else if ((ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK) == BIT0_RESET)
            {
#ifdef DBG_CAN
              DEBUG_PRINTF("Msg ID:  FAULT_MFM_VALVE_OFF\r\n");
#endif
            }
            /* FAULT BYTE UPDATED and WRITTEN in NVSRAM */
            ucFaultByte0 = (ucFaultByte0 | BIT2_SET);  
            fnFault_Log();
          }
          break;
          
        case FAULT_BHCM_TEMP_TH:
          {
#ifdef DBG_CAN
            DEBUG_PRINTF("Msg ID: FAULT_BHCM_TEMP_TH \r\n");
#endif
            
            if ((ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK) == BIT0_SET)
            {
              StatusFlag.stStatusFlag.bErrorFlag = SET;/*Error SET*/
#ifdef DBG_CAN
              DEBUG_PRINTF("Msg ID:  FAULT_BHCM_TEMP_TH_HIGH\r\n");
#endif
            }
            else if ((ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK) == BIT0_RESET)
            {
              StatusFlag.stStatusFlag.bWarnningFlag = SET;/*Warnning SET*/
#ifdef DBG_CAN
              DEBUG_PRINTF("Msg ID:  FAULT_BHCM_TEMP_TH_LOW\r_\n");
#endif
            }
            /* FAULT BYTE UPDATED and WRITTEN in NVSRAM */
            ucFaultByte0 = (ucFaultByte0 | BIT4_SET);  
            fnFault_Log();
          }
          break;
          
        case FAULT_BHCM_LIQUID_LEVEL:
          {
#ifdef DBG_CAN
            DEBUG_PRINTF("Msg ID: FAULT_BHCM_LIQUID_LEVEL \r\n");
#endif
            if ((ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK) == BIT0_SET)
            {
#ifdef DBG_CAN
              DEBUG_PRINTF("Msg ID:  FAULT_BHCM_LIQUID_LEVEL_HIGH\r\n");
#endif
            }
            else if ((ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK) == BIT0_RESET)
            {
#ifdef DBG_CAN
              DEBUG_PRINTF("Msg ID:  FAULT_BHCM_LIQUID_LEVEL_LOW\r_\n");
#endif
            }
            /* FAULT BYTE UPDATED and WRITTEN in NVSRAM */
            ucFaultByte0 = (ucFaultByte0 | BIT5_SET);  
            fnFault_Log();
          }
          break;
          
        case FAULT_IHCM1_IBASE_TEMP_TH:
          {
#ifdef DBG_CAN
            DEBUG_PRINTF("Msg ID: FAULT_IHCM1_IBASE_TEMP_TH \r\n");
#endif
            
            if((ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK) == BIT0_SET)
            {
              StatusFlag.stStatusFlag.bErrorFlag = SET;/*Warnning SET*/
              
#ifdef DBG_CAN
              DEBUG_PRINTF("Msg ID:  FAULT_IHCM1_IBASE_TEMP_TH_HIGH\r\n");
#endif
            }
            else if ((ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK) == BIT0_RESET)
            {
              StatusFlag.stStatusFlag.bWarnningFlag = SET;/*Warnning SET*/
              
#ifdef DBG_CAN
              DEBUG_PRINTF("Msg ID:  FAULT_IHCM1_IBASE_TEMP_TH_LOW\r_\n");
#endif
            }
            /* FAULT BYTE UPDATED and WRITTEN in NVSRAM */
            ucFaultByte0 = (ucFaultByte0 | BIT6_SET); 
            fnFault_Log();
          }
          break;
          
        case FAULT_IHCM4_IHARD_TEMP_TH:
          {
#ifdef DBG_CAN
            DEBUG_PRINTF("Msg ID: FAULT_IHCM4_IHARD_TEMP_ABOVE_TH \r\n");
#endif
            
            if ((ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK) == BIT0_SET)
            {
              StatusFlag.stStatusFlag.bErrorFlag = SET;/*Error SET*/
#ifdef DBG_CAN
              DEBUG_PRINTF("Msg ID:  FAULT_IHCM4_IHARD_TEMP_TH_HIGH\r\n");
#endif
            }
            else if ((ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK) == BIT0_RESET)
            {
              StatusFlag.stStatusFlag.bWarnningFlag = SET;/*Warnning SET*/
#ifdef DBG_CAN
              DEBUG_PRINTF("Msg ID:  FAULT_IHCM4_IHARD_TEMP_TH_LOW\r_\n");
#endif
            }
            /* FAULT BYTE UPDATED and WRITTEN in NVSRAM */
            ucFaultByte1 = (ucFaultByte1 | BIT0_SET);  
            fnFault_Log();
          }
          break;
          
        case FAULT_PGAUGE1_ABOVE_TH:
          {
#ifdef DBG_CAN
            DEBUG_PRINTF("Msg ID: FAULT_PGAUGE1_ABOVE_TH \r\n");
#endif
            if ((ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK) == BIT0_SET)
            {
              StatusFlag.stStatusFlag.bErrorFlag = SET;/*Wrror SET*/
              
#ifdef DBG_CAN
              DEBUG_PRINTF("Msg ID:  FAULT_PGAUGE1_ABOVE_TH_HIGH\r\n");
#endif
            }
            else if ((ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK) == BIT0_RESET)
            {
              StatusFlag.stStatusFlag.bWarnningFlag = SET;/*Warnning SET*/
              
#ifdef DBG_CAN
              DEBUG_PRINTF("Msg ID:  FAULT_PGAUGE1_ABOVE_TH_LOW\r_\n");
#endif
            }
            /* FAULT BYTE UPDATED and WRITTEN in NVSRAM */
            ucFaultByte1 = (ucFaultByte1 | BIT2_SET);
            fnFault_Log();
          }
          break;
          
        case FAULT_PGAUGE2_ABOVE_TH:
          {
#ifdef DBG_CAN
            DEBUG_PRINTF("Msg ID: FAULT_PGAUGE2_ABOVE_TH \r\n");
#endif
            if ((ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK) == BIT0_SET)
            {
              
              StatusFlag.stStatusFlag.bErrorFlag = SET;/*Error SET*/
              
#ifdef DBG_CAN
              DEBUG_PRINTF("Msg ID:  FAULT_PGAUGE2_ABOVE_TH_HIGH\r\n");
#endif
            }
            else if ((ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK) == BIT0_RESET)
            {
              
              StatusFlag.stStatusFlag.bWarnningFlag = SET;/*Warnning SET*/
              
#ifdef DBG_CAN
              DEBUG_PRINTF("Msg ID:  FAULT_PGAUGE2_ABOVE_TH_LOW\r_\n");
#endif
            }
            /* FAULT BYTE UPDATED and WRITTEN in NVSRAM */
            ucFaultByte1 = (ucFaultByte1 | BIT3_SET); 
            fnFault_Log();
          }
          break;
          
        case FAULT_PGAUGE3_ABOVE_TH:
          {
#ifdef DBG_CAN
            DEBUG_PRINTF("Msg ID: FAULT_PGAUGE3_ABOVE_TH \r\n");
#endif
            if ((ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK) == BIT0_SET)
            {
              StatusFlag.stStatusFlag.bErrorFlag = SET;/*Error SET*/
#ifdef DBG_CAN
              DEBUG_PRINTF("Msg ID:  FAULT_PGAUGE3_ABOVE_TH_HIGH\r\n");
#endif
            }
            else if ((ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK) == BIT0_RESET)
            {
              StatusFlag.stStatusFlag.bWarnningFlag = SET;/*Warnning SET*/
#ifdef DBG_CAN
              DEBUG_PRINTF("Msg ID:  FAULT_PGAUGE3_ABOVE_TH_LOW\r_\n");
#endif
            }
            /* FAULT BYTE UPDATED and WRITTEN in NVSRAM */
            ucFaultByte1 = (ucFaultByte1 | BIT4_SET); 
            fnFault_Log();
          }
          break;
          
        case FAULT_DEFECTIVE_HEATER:
          {
#ifdef DBG_CAN
            DEBUG_PRINTF("Msg ID: FAULT_DEFECTIVE_HEATER \r\n");
#endif
            switch (uliCanID & SOURCE_ADDR_MASK)
            {
            case SOURCE_BHCM:
              {
                StatusFlag.stStatusFlag.bWarnningFlag = SET;/*Warnning SET*/
                
#ifdef DBG_CAN
                DEBUG_PRINTF("\r\nBHCM HEATER FAULT \r\n");
#endif
                /* FAULT BYTE UPDATED and WRITTEN in NVSRAM */
                ucFaultByte2 = (ucFaultByte2 | BIT1_SET);
                fnFault_Log();
              }
              break;
              
            case SOURCE_HHCM:
              {
#ifdef DBG_CAN
                DEBUG_PRINTF("\r\nHHCM HEATER FAULT \r\n");
#endif
                StatusFlag.stStatusFlag.bWarnningFlag = SET;/*Warnning SET*/
                /* FAULT BYTE UPDATED and WRITTEN in NVSRAM */
                ucFaultByte2 = (ucFaultByte2 | BIT2_SET);
                fnFault_Log();
              }
              break;
              
            case SOURCE_IHCM1:
              {
#ifdef DBG_CAN
                DEBUG_PRINTF("\r\nIHCM1 HEATER FAULT \r\n");
#endif
                
                StatusFlag.stStatusFlag.bWarnningFlag = SET;/*Warnning SET*/
                /* FAULT BYTE UPDATED and WRITTEN in NVSRAM */
                ucFaultByte2 = (ucFaultByte2 | BIT3_SET);  
                fnFault_Log();
              }
              break;
              
            case SOURCE_IHCM4_HM:
              {
#ifdef DBG_CAN
                DEBUG_PRINTF("\r\nIHCM4 HARDENER MATERIAL HEATER FAULT \r\n");
#endif
                StatusFlag.stStatusFlag.bWarnningFlag = SET;/*Warnning SET*/
                /* FAULT BYTE UPDATED and WRITTEN in NVSRAM */
                ucFaultByte2 = (ucFaultByte2 | BIT7_SET); 
                fnFault_Log();
              }
              break;
            }//heater switch ends
          }
          break;
          
        case FAULT_ROTARY_SWITCH:
          {
#ifdef DBG_CAN
            DEBUG_PRINTF("\r\n FAULT_SPRAY_GUN_MOTION FAULT \r\n");
#endif
            StatusFlag.stStatusFlag.bErrorFlag = SET;/*Error SET*/
            /*FAULT BYTE UPDATED and WRITTEN in NVSRAM*/
            ucFaultByte3 = (ucFaultByte3 | BIT0_SET);  
            fnFault_Log();
          }
          break;
          
        case FAULT_SPRAY_GUN_MOTION:
          {
#ifdef DBG_CAN
            DEBUG_PRINTF("\r\n FAULT_SPRAY_GUN_MOTION FAULT \r\n");
#endif
            StatusFlag.stStatusFlag.bWarnningFlag = SET;/*Warnning SET*/
            
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & 0xFFFF0000) |
                                  (DEST_OP | SOURCE_MCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            fnCANMsg_Transmit(&stTx1Message, CAN1);
            /* FAULT BYTE UPDATED and WRITTEN in NVSRAM */
            ucFaultByte3 = (ucFaultByte3 | BIT1_SET); 
            fnFault_Log();
          }
          break;
          
        case FAULT_LT_OUT_OF_RANGE:
          {
#ifdef DBG_CAN
            DEBUG_PRINTF("\r\n FAULT_Linear Transducer_OUT_OF_RANGE \r\n");
#endif
            StatusFlag.stStatusFlag.bErrorFlag = SET;/*Error SET*/
            /* FAULT BYTE UPDATED and WRITTEN in NVSRAM */
            ucFaultByte3 = (ucFaultByte3 | BIT2_SET); 
            fnFault_Log();
          }
          break;
          
        case FAULT_OPMM_ROTATION:
          {
            StatusFlag.stStatusFlag.bErrorFlag = SET;/*Error SET*/
            if(DCLS_TIMER_FAULT == ucRec_Data[DATA_BYTE_ZERO])
            {
#ifdef DBG_CAN
              DEBUG_PRINTF("\r\n FAULT_OPMM_ROTATION_DCLS_TIMER \r\n");
#endif
              /* FAULT BYTE UPDATED and WRITTEN in NVSRAM */
              ucFaultByte3 = (ucFaultByte3 | BIT3_SET);  
              fnFault_Log();
            }
            else if(HLS_TIMER_FAULT == ucRec_Data[DATA_BYTE_ZERO])
            {
#ifdef DBG_CAN
              DEBUG_PRINTF("\r\n FAULT_OPMM_ROTATION_HLS_TIMER \r\n");
#endif
              /* FAULT BYTE UPDATED and WRITTEN in NVSRAM */
              ucFaultByte3 = (ucFaultByte3 | BIT4_SET);  
              fnFault_Log();
            }
            /*Send to OP*/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & 0xFFFF0000) | 
                                  (DEST_OP | SOURCE_MCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            fnCANMsg_Transmit(&stTx1Message, CAN1);
            
          }
          break;
          
        case FAULT_HHCM_LIQUID_LEVEL:
          {
#ifdef DBG_CAN
            DEBUG_PRINTF("Msg ID: FAULT_BHCM_LIQUID_LEVEL \r\n");
#endif
            if ((ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK) == BIT0_SET)
            {
#ifdef DBG_CAN
              DEBUG_PRINTF("Msg ID:  FAULT_BHCM_LIQUID_LEVEL_HIGH\r\n");
#endif
            }
            else if ((ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK) == BIT0_RESET)
            {
#ifdef DBG_CAN
              DEBUG_PRINTF("Msg ID:  FAULT_BHCM_LIQUID_LEVEL_LOW\r_\n");
#endif
            }
            /* FAULT BYTE UPDATED and WRITTEN in NVSRAM */
            ucFaultByte3 = (ucFaultByte3 | BIT5_SET);  
            fnFault_Log();
          }
          break;
        }
      }
      break;/*Fault Msgs Case ends*/
      
    case MSG_TYPE_STATUS:
      {
#ifdef DBG_CAN
        DEBUG_PRINTF("Msg Type: MSG_TYPE_STATUS \r\n");
#endif
        switch (uliCanID & MSG_MASK)
        {
        case ACKNOWLEDGEMENT:
          {
            fnCANPost_Ack(pstCANrxmsg,CAN1);
          }
          break;
          
        case STATUS_STALL_TEST_RESULT:
          {
#ifdef DBG_CAN
            DEBUG_PRINTF("Msg ID: STATUS_STALL_TEST_RESULT \r\n");
#endif
            StatusFlag.stStatusFlag.bStallTestStatus = SET;
            
            /*LEG A*/
            if((ucRec_Data[DATA_BYTE_ONE] & BIT0_MASK)  == BIT0_SET)
            {
              if((ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK) == BIT0_SET)
              {
                StatusFlag.stStatusFlag.bLAUpstrokeStallTestPass = RESET; /*Fail*/
#ifdef DBG_CAN
                DEBUG_PRINTF("\r\nLEG A UPSTROKE STALL TEST FAIL\r\n");
#endif
              }
              else if((ucRec_Data[DATA_BYTE_ZERO] & BIT1_MASK) == BIT1_SET)
              {
                StatusFlag.stStatusFlag.bLAUpstrokeStallTestPass = SET; /*Pass*/
#ifdef DBG_CAN
                DEBUG_PRINTF("\r\nLEG A UPSTROKE STALL TEST PASS\r\n");
#endif
              }
              if((ucRec_Data[DATA_BYTE_ZERO] & BIT2_MASK) == BIT2_SET)
              {
                StatusFlag.stStatusFlag.bLADnstrokeStallTestPass = RESET;
#ifdef DBG_CAN
                DEBUG_PRINTF("\r\nLEG A DOWNSTROKE STALL TEST FAIL\r\n");
#endif
              }
              else if((ucRec_Data[DATA_BYTE_ZERO] & BIT3_MASK) == BIT3_SET)
              {
                StatusFlag.stStatusFlag.bLADnstrokeStallTestPass = SET;
#ifdef DBG_CAN
                DEBUG_PRINTF("\r\nLEG A DOWNSTROKE STALL TEST PASS\r\n");
#endif
              }
            }
            
            /*LEG B*/
            if((ucRec_Data[DATA_BYTE_ONE] & BIT1_MASK) == BIT1_SET)
            {
              if((ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK) == BIT0_SET)
              {
                StatusFlag.stStatusFlag.bLBUpstrokeStallTestPass = RESET;
#ifdef DBG_CAN
                DEBUG_PRINTF("\r\nLEG B UPSTROKE STALL TEST FAIL\r\n");
#endif
              }
              else if((ucRec_Data[DATA_BYTE_ZERO] & BIT1_MASK) == BIT1_SET)
              {
                StatusFlag.stStatusFlag.bLBUpstrokeStallTestPass = SET;
#ifdef DBG_CAN
                DEBUG_PRINTF("\r\nLEG B UPSTROKE STALL TEST PASS\r\n");
#endif
              }
              if((ucRec_Data[DATA_BYTE_ZERO] & BIT2_MASK) == BIT2_SET)
              {
                StatusFlag.stStatusFlag.bLBDnstrokeStallTestPass = 0;
#ifdef DBG_CAN
                DEBUG_PRINTF("\r\nLEG B DOWNSTROKE STALL TEST FAIL\r\n");
#endif
              }
              else if((ucRec_Data[DATA_BYTE_ZERO] & BIT3_MASK) == BIT3_SET)
              {
                StatusFlag.stStatusFlag.bLBDnstrokeStallTestPass = 1;
#ifdef DBG_CAN
                DEBUG_PRINTF("\r\nLEG B DOWNSTROKE STALL TEST PASS\r\n");
#endif
              }
            }
            
            /*LEG C*/
            if((ucRec_Data[DATA_BYTE_ONE] & BIT2_MASK) == BIT2_SET)
            {
              if((ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK) == BIT0_SET)
              {
                StatusFlag.stStatusFlag.bLCUpstrokeStallTestPass = RESET;
#ifdef DBG_CAN
                DEBUG_PRINTF("\r\nLEG C UPSTROKE STALL TEST FAIL\r\n");
#endif
              }
              else if((ucRec_Data[DATA_BYTE_ZERO] & BIT1_MASK) == BIT1_SET)
              {
                StatusFlag.stStatusFlag.bLCUpstrokeStallTestPass = SET;
#ifdef DBG_CAN
                DEBUG_PRINTF("\r\nLEG C UPSTROKE STALL TEST PASS\r\n");
#endif
              }
              if((ucRec_Data[DATA_BYTE_ZERO] & BIT2_MASK) == BIT2_SET)
              {
                StatusFlag.stStatusFlag.bLCDnstrokeStallTestPass = RESET;
#ifdef DBG_CAN
                DEBUG_PRINTF("\r\nLEG C DOWNSTROKE STALL TEST FAIL\r\n");
#endif
              }
              else if((ucRec_Data[DATA_BYTE_ZERO] & BIT3_MASK) == BIT3_SET)
              {
                StatusFlag.stStatusFlag.bLCDnstrokeStallTestPass = SET;
#ifdef DBG_CAN
                DEBUG_PRINTF("\r\nLEG C DOWNSTROKE STALL TEST PASS\r\n");
#endif
              }
            }	
            /*Down stroke status*/
            if((RESET == StatusFlag.stStatusFlag.bLADnstrokeStallTestPass) || 
               (RESET == StatusFlag.stStatusFlag.bLBDnstrokeStallTestPass) || 
                 (RESET == StatusFlag.stStatusFlag.bLCDnstrokeStallTestPass))
            {
              ucStatusByte &=  DOWNSTROKE_STALL_FAIL ;
            }
            else 
            {
              ucStatusByte |=  DOWNSTROKE_STALL_PASS ;
            }
            /*Up stroke status*/
            if((RESET == StatusFlag.stStatusFlag.bLAUpstrokeStallTestPass) || 
               (RESET == StatusFlag.stStatusFlag.bLBUpstrokeStallTestPass) || 
                 (RESET == StatusFlag.stStatusFlag.bLCUpstrokeStallTestPass))
            {
              ucStatusByte &=  UPSTROKE_STALL_PASS ;
            }
            else 
            {
              ucStatusByte |=  UPSTROKE_STALL_PASS ;
            }						
          }
          break;
          
        case STATUS_SPRAY_PROCESS:
          {
#ifdef DBG_CAN
            DEBUG_PRINTF("Msg ID: STATUS_SPRAY_PROCESS \r\n");
#endif
            /*This indicates Spray status arrived*/
            StatusFlag.stStatusFlag.bSprayProcessStatus = SET;
            if((ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK) == BIT0_SET)
            {
              /*This indicates Spray process Pass*/
              StatusFlag.stStatusFlag.bSprayProcPass = SET;
              /*This indicates Spray status arrived*/
              StatusFlag.stStatusFlag.bSprayProcessStatus = SET;
#ifdef DBG_CAN
              DEBUG_PRINTF("\r\n STATUS SPRAY PROCESS PASS\r\n");
#endif
            }
            else if((ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK) == BIT0_RESET)
            {
              /*This indicates Spray process Fail*/	
              StatusFlag.stStatusFlag.bSprayProcPass = 0; 
#ifdef DBG_CAN
              DEBUG_PRINTF("\r\nSTATUS SPRAY PROCESS FAIL \r\n");
#endif
            }
            
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & 0xFFFF0000) | 
                                  (DEST_OP | SOURCE_MCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            fnCANMsg_Transmit(&stTx1Message, CAN1);
          }
          break;
          
        case STATUS_PRIME_PROCESS:
          {
#ifdef DBG_CAN
            DEBUG_PRINTF("Msg ID: STATUS_PRIME_PROCESS \r\n");
#endif
            /*Status Rcv indication flag*/
            StatusFlag.stStatusFlag.bPrimeProccesStatus = SET;
            if((ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK) == BIT0_SET)
            {
              StatusFlag.stStatusFlag.bPrimeProcPass = SET;
#ifdef DBG_CAN
              DEBUG_PRINTF("\r\n STATUS PRIME PROCESS PASS\r\n");
#endif
            }
            else if((ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK) == BIT0_RESET)
            {
              StatusFlag.stStatusFlag.bPrimeProcPass = RESET;
#ifdef DBG_CAN
              DEBUG_PRINTF("\r\nSTATUS PRIME PROCESS FAIL \r\n");
#endif
            }
            /*System Primed for Status Log*/
            ucStatusByte |= SYSTEM_PRIMED;
            /*Send to OP*/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & 0xFFFF0000) | 
                                  (DEST_OP | SOURCE_MCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            fnCANMsg_Transmit(&stTx1Message, CAN1);
          }
          break;
          
        case STATUS_FLUSH_PROCESS:
          {
#ifdef DBG_CAN
            DEBUG_PRINTF("Msg ID: STATUS_FLUSH_PROCESS \r\n");
#endif
            /*Status Rcv indication flag*/
            StatusFlag.stStatusFlag.bFlushProccessStatus = SET;
            if(ucRec_Data[DATA_BYTE_ZERO] == BIT0_SET)
            {
              StatusFlag.stStatusFlag.bFlushProcPass = SET;
#ifdef DBG_CAN
              DEBUG_PRINTF("\r\n STATUS FLUSH PROCESS PASS\r\n");
#endif
            }
            else if(ucRec_Data[DATA_BYTE_ZERO]  == BIT0_RESET)
            {
              StatusFlag.stStatusFlag.bFlushProcPass = RESET;
#ifdef DBG_CAN
              DEBUG_PRINTF("\r\nSTATUS FLUSH PROCESS FAIL \r\n");
#endif
            }
            /*System Flush for Status Data log*/
            ucStatusByte |= SYSTEM_FLUSH;
            /*Send to OP*/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & 0xFFFF0000) | 
                                  (DEST_OP | SOURCE_MCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            fnCANMsg_Transmit(&stTx1Message, CAN1);
          }
          break;
          
          
        case STATUS_OPMM_ON_PIPE:
          {
#ifdef DBG_CAN
            DEBUG_PRINTF("Msg ID: STATUS_OPMM_ON_PIPE \r\n");
#endif
            if((ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK) == BIT0_SET)
            {
              StatusFlag.stStatusFlag.bOPMMSetOnPipe = SET;
#ifdef DBG_CAN
              DEBUG_PRINTF("\r\nOPMM SET ON PIPE\r\n");
#endif
            }
            else if((ucRec_Data[DATA_BYTE_ZERO] & BIT0_MASK) == BIT0_RESET)
            {
              StatusFlag.stStatusFlag.bOPMMSetOnPipe = RESET;
#ifdef DBG_CAN
              DEBUG_PRINTF("\r\n OPMM NOT SET ON PIPE \r\n");
#endif
            }
            /*Send to OP*/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & 0xFFFF0000) | 
                                  (DEST_OPMM | SOURCE_MCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            fnCANMsg_Transmit(&stTx1Message, CAN1);
          }
          break;
          
        case STATUS_PRIME_TIME_ACHIEVED:
          {
#ifdef DBG_CAN
            DEBUG_PRINTF("Msg ID: STATUS_PRIME/FLUSH_TIME_ACHIEVED \r\n");
#endif
            if(ucRec_Data[DATA_BYTE_ZERO]  == BIT0_SET )	
            {
              if(ucRec_Data[DATA_BYTE_ONE]  == BIT1_SET)/*Pass*/
              {
                
#ifdef DBG_CAN
                DEBUG_PRINTF("\r\n PRIME PROCESS TIME ACHIEVED Pass\r\n");
#endif
              }
              else if(ucRec_Data[DATA_BYTE_ONE]  == BIT0_SET)/*Fail*/
              {
#ifdef DBG_CAN
                DEBUG_PRINTF("\r\n PRIME PROCESS TIME ACHIEVED FAIL\r\n");
#endif
              }
            }
            /*Flush Time achive status*/
            else if(ucRec_Data[DATA_BYTE_ZERO] == BIT1_SET)
            {
              if(ucRec_Data[DATA_BYTE_ONE]  == BIT1_SET)/*Pass*/
              {
#ifdef DBG_CAN
                DEBUG_PRINTF("\r\n FLUSH PROCESS TIME ACHIEVED PASS \r\n");
#endif
              }
              else if(ucRec_Data[DATA_BYTE_ONE]  == BIT0_SET) /*Fail*/
              {
#ifdef DBG_CAN
                DEBUG_PRINTF("\r\n FLUSH PROCESS TIME ACHIEVED FAIL \r\n");
#endif 
              }
            }
            /*Send to OP*/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & 0xFFFF0000) | 
                                  (DEST_OP | SOURCE_MCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            fnCANMsg_Transmit(&stTx1Message, CAN1);
          }
          break;
          
        case STATUS_OP_DIAGNOSTIC:
          {
#ifdef DBG_CAN
            DEBUG_PRINTF("Msg ID: STATUS_OP_DIAGNOSTIC \r\n");
#endif
            if(ucRec_Data[DATA_BYTE_ZERO])	
            { 
              /*Overall OP diagnostic status is in byte 0*/
              /*op diagnostic pass*/
              uiDeviceDiagStatus |= (1 << OP);
#ifdef DBG_CAN
              DEBUG_PRINTF("\r\n OP Diagnostic Pass Status\r\n");
#endif
            }
            else 
            {
              /*op diagnostics fail*/
              StatusFlag.stStatusFlag.bWarnningFlag = SET;/*Warnning SET*/
              uiDeviceDiagStatus &= ~(1 << OP);
#ifdef DBG_CAN
              DEBUG_PRINTF("\r\n OPMM Diagnostic Fail Status\r\n");
#endif
            }
#ifdef DBG_CAN
            DEBUG_PRINTF("Diagnostic Data Log id = %x",pstCANrxmsg->ExtId);
#endif
            fnNVSRAM1MultiData_Write( 2, (uliBaseAddr+BYTE_NINE), ucRec_Data);
          }
          break;
          
        case STATUS_OPMM_DIAGNOSTIC:
          {
#ifdef DBG_CAN
            DEBUG_PRINTF("Msg ID: STATUS_OPMM_DIAGNOSTIC \r\n");
#endif
            if(ucRec_Data[DATA_BYTE_ZERO])	
            { 
              uiDeviceDiagStatus |= (1 << OPMM);
#ifdef DBG_CAN
              DEBUG_PRINTF("\r\n OPMM Diagnostic Pass Status\r\n");
#endif
            }
            else 
            {
              uiDeviceDiagStatus &= ~(1 << OPMM);
#ifdef DBG_CAN
              DEBUG_PRINTF("\r\n OPMM Diagnostic failed Status\r\n");
#endif
              StatusFlag.stStatusFlag.bWarnningFlag = SET;/*Warnning SET*/
            }
            /*Send to OP*/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & 0xFFFF0000) | 
                                  (DEST_OP | SOURCE_MCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CAN
            DEBUG_PRINTF("Diagnostic Data Log id = %x",pstCANrxmsg->ExtId);
#endif
            fnNVSRAM1MultiData_Write(3, (uliBaseAddr+BYTE_ELEVEN), ucRec_Data);
          }
          break;
          
        case STATUS_MFM_DIAGNOSTIC:
          {
#ifdef DBG_CAN
            DEBUG_PRINTF("Msg ID: STATUS_MFM_DIAGNOSTIC \r\n");
#endif
            if(ucRec_Data[DATA_BYTE_ZERO])	
            { 
              uiDeviceDiagStatus |= (1 << MFM);
#ifdef DBG_CAN
              DEBUG_PRINTF("\r\nMFM Diagnostic Pass Status\r\n");
#endif
            }
            else 
            {
              uiDeviceDiagStatus &= ~(1 << MFM);
#ifdef DBG_CAN
              DEBUG_PRINTF("\r\n OPMM Diagnostic failed Status\r\n");
#endif
              StatusFlag.stStatusFlag.bWarnningFlag = SET;/*Warnning SET*/
            }
#ifdef DBG_CAN
            DEBUG_PRINTF("Diagnostic Data Log id = %x",pstCANrxmsg->ExtId);
#endif
            fnNVSRAM1MultiData_Write( 3, (uliBaseAddr+BYTE_FOURTEEN), ucRec_Data);
          }
          break;
          
        case STATUS_BHCM_DIAGNOSTIC:
          {
#ifdef DBG_CAN
            DEBUG_PRINTF("Msg ID: STATUS_BHCM_DIAGNOSTIC \r\n");
#endif
            if(ucRec_Data[DATA_BYTE_ZERO])	
            { 
              uiDeviceDiagStatus |= (1 << BHCM);
#ifdef DBG_CAN
              DEBUG_PRINTF("\r\nBHCM Diagnostic Pass Status\r\n");
#endif									
            }
            else 
            {
              uiDeviceDiagStatus &= ~(1 << BHCM);
              StatusFlag.stStatusFlag.bWarnningFlag = SET;/*Warnning SET*/
#ifdef DBG_CAN
              DEBUG_PRINTF("\r\nBHCM Diagnostic failed Status\r\n");
#endif
            }
#ifdef DBG_CAN
            DEBUG_PRINTF("Diagnostic Data Log id = %x",pstCANrxmsg->ExtId);
#endif
            fnNVSRAM1MultiData_Write( 2, (uliBaseAddr+BYTE_TWENTY), ucRec_Data);
          }
          break;
          
        case STATUS_HHCM_DIAGNOSTIC:
          {
#ifdef DBG_CAN
            DEBUG_PRINTF("Msg ID: STATUS_HHCM_DIAGNOSTIC \r\n");
#endif
            
            if(ucRec_Data[DATA_BYTE_ZERO])	
            { 
              uiDeviceDiagStatus |= (1 << HHCM);
#ifdef DBG_CAN
              DEBUG_PRINTF("\r\nHHCM Diagnostic Pass Status\r\n");
#endif
            }
            else 
            {
              uiDeviceDiagStatus &= ~(1 << HHCM);
              StatusFlag.stStatusFlag.bWarnningFlag = SET;/*Warnning SET*/
#ifdef DBG_CAN
              DEBUG_PRINTF("\r\nHHCM Diagnostic failed Status\r\n");
#endif
            }
#ifdef DBG_CAN
            DEBUG_PRINTF("Diagnostic Data Log id = %x",pstCANrxmsg->ExtId);
#endif
            fnNVSRAM1MultiData_Write( 2, (uliBaseAddr+BYTE_TWENTYTWO), ucRec_Data);  
          }
          break;
          
        case STATUS_IHCM1_DIAGNOSTIC:
          {
#ifdef DBG_CAN
            DEBUG_PRINTF("Msg ID: STATUS_IHCM1_DIAGNOSTIC \r\n");
#endif
            
            if(ucRec_Data[DATA_BYTE_ZERO])	
            { 
              uiDeviceDiagStatus |= (1 << IHCM1);
#ifdef DBG_CAN
              DEBUG_PRINTF("\r\nIHCM1 Diagnostic Pass Status\r\n");
#endif
            }
            else 
            {
              uiDeviceDiagStatus &= ~(1 << IHCM1);
              StatusFlag.stStatusFlag.bWarnningFlag = SET;/*Warnning SET*/
#ifdef DBG_CAN
              DEBUG_PRINTF("\r\n IHCM1 Diagnostic failed Status\r\n");
#endif
            }
#ifdef DBG_CAN
            DEBUG_PRINTF("Diagnostic Data Log id = %x",pstCANrxmsg->ExtId);
#endif
            fnNVSRAM1MultiData_Write( 2, (uliBaseAddr+BYTE_TWENTYFOUR), ucRec_Data);
          }
          break;
          
        case STATUS_IHCM4_DIAGNOSTIC_HM:
          {
#ifdef DBG_CAN
            DEBUG_PRINTF("Msg ID: STATUS_IHCM4_DIAGNOSTIC_HM \r\n");
#endif
            
            if(ucRec_Data[DATA_BYTE_ZERO])	
            { 
              uiDeviceDiagStatus |= (1 << IHCM4_HM);
#ifdef DBG_CAN
              DEBUG_PRINTF("\r\nIHCM4_HM Diagnostic Pass Status\r\n");
#endif
            }
            else 
            {
              uiDeviceDiagStatus &= ~(1 << IHCM4_HM);
              StatusFlag.stStatusFlag.bWarnningFlag = SET;/*Warnning SET*/
#ifdef DBG_CAN
              DEBUG_PRINTF("\r\n IHCM4_HM Diagnostic failed Status\r\n");
#endif	
            }
#ifdef DBG_CAN
            DEBUG_PRINTF("Diagnostic Data Log id = %x",pstCANrxmsg->ExtId);
#endif
            fnNVSRAM1MultiData_Write( 2, (uliBaseAddr+BYTE_THIRTYTWO), ucRec_Data);
          }
          break;
          
        case STATUS_GUN_POSITION:
          {
#ifdef DBG_CAN
            DEBUG_PRINTF("Msg ID: STATUS_GUN_POSITION \r\n");
#endif
            if(BIT0_SET == ucRec_Data[DATA_BYTE_ZERO]) 
            {
              StatusFlag.stStatusFlag.bPrimePosition = SET;
              StatusFlag.stStatusFlag.bSprayPosition = RESET;
#ifdef DBG_CAN
              DEBUG_PRINTF("\r\n PRIME POSITION \r\n");
#endif
            }	
            else if(BIT1_SET == ucRec_Data[DATA_BYTE_ZERO]) 
            {
              StatusFlag.stStatusFlag.bSprayPosition = SET;
              StatusFlag.stStatusFlag.bPrimePosition = RESET;
#ifdef DBG_CAN
              DEBUG_PRINTF("\r\n SPRAY POSITION \r\n");
#endif
            }
            /*Send to OP*/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & 0xFFFF0000) | 
                                  (DEST_OP | SOURCE_MCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            fnCANMsg_Transmit(&stTx1Message, CAN1);
          }
          break;
          
        case  STATUS_VALVES_ON_OFF	:								
          {
#ifdef DBG_CAN
            DEBUG_PRINTF("Msg ID: STATUS_VALVES_ON_OFF \r\n");
#endif
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & 0xFFFF0000) |
                                  (DEST_OP | SOURCE_MCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt=0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            fnCANMsg_Transmit(&stTx1Message, CAN1);
          }
          break;
        }
      }
      break;/*STATUS Msgs Case ends*/
      
    case MSG_TYPE_DATA:
      {
#ifdef DBG_CAN
        DEBUG_PRINTF("Msg Type: MSG_TYPE_DATA \r\n");
#endif
        switch (uliCanID & MSG_MASK)
        {
        case ACKNOWLEDGEMENT:
          {
            fnCANPost_Ack(pstCANrxmsg,CAN1);
          }break;
          
          /*OPMM pipe temp non-contact*/
        case DATA_OPMM_PIPE_TEMP:
          {
#ifdef DBG_CAN
            DEBUG_PRINTF("Msg ID: DATA_OPMM_PIPE_TEMP \r\n");
#endif
            for(ucLoopCnt=0; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt+1];
            }
            if(PIPE_POSITION_12 == pstCANrxmsg->Data[0])
            {
              stCurrentData.fCurrPipeTempPos12 = SplitByte.fData;
#ifdef DBG_CAN
              memset(rgcTemp, NULL , sizeof(rgcTemp));
              sprintf(rgcTemp,"POS12_PIPE_TEMP_CURRENT: %0.2f", 
                      stCurrentData.fCurrPipeTempPos12);
              DEBUG_PRINTF("%s\r\n",rgcTemp);
#endif
            }
            else if(PIPE_POSITION_3 == pstCANrxmsg->Data[0])
            {
              stCurrentData.fCurrPipeTempPos3 = SplitByte.fData;
#ifdef DBG_CAN
              memset(rgcTemp, NULL , sizeof(rgcTemp));
              sprintf(rgcTemp,"POS3_PIPE_TEMP_CURRENT: %0.2f", 
                      stCurrentData.fCurrPipeTempPos3);
              DEBUG_PRINTF("%s\r\n",rgcTemp);
#endif
            }
            else if(PIPE_POSITION_6 == pstCANrxmsg->Data[0])
            {
              stCurrentData.fCurrPipeTempPos6 = SplitByte.fData;
#ifdef DBG_CAN
              memset(rgcTemp, NULL , sizeof(rgcTemp));
              sprintf(rgcTemp,"POS6_PIPE_TEMP_CURRENT: %0.2f", 
                      stCurrentData.fCurrPipeTempPos6);
              DEBUG_PRINTF("%s\r\n",rgcTemp);
#endif
            }
            else if(PIPE_POSITION_9 == pstCANrxmsg->Data[0])
            {
              stCurrentData.fCurrPipeTempPos9 = SplitByte.fData;
#ifdef DBG_CAN
              memset(rgcTemp, NULL , sizeof(rgcTemp));
              sprintf(rgcTemp,"POS9_PIPE_TEMP_CURRENT: %0.2f", 
                      stCurrentData.fCurrPipeTempPos9);
              DEBUG_PRINTF("%s\r\n",rgcTemp);
#endif
            }
            else if(AVG_TEMP == pstCANrxmsg->Data[0])
            {
              stCurrentData.fCurrPipeTempAvg = SplitByte.fData;
#ifdef DBG_CAN
              memset(rgcTemp, NULL , sizeof(rgcTemp));
              sprintf(rgcTemp,"AVG_PIPE_TEMP_CURRENT: %0.2f", 
                      stCurrentData.fCurrPipeTempAvg );
              DEBUG_PRINTF("%s\r\n",rgcTemp);
#endif
            }
            /*Send to OP*/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & 0xFFFF0000) | 
                                  (DEST_OP | SOURCE_MCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            fnCANMsg_Transmit(&stTx1Message, CAN1);
          }
          break;
          
          /*MFM volume of material used*/
        case DATA_MFM_MATERIAL_USED:
          {
#ifdef DBG_CAN
            DEBUG_PRINTF("Msg ID: DATA_MFM_MATERIAL_USED \r\n");
#endif
            for(ucLoopCnt=0; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt+1];
            }
            if(LEGA == pstCANrxmsg->Data[0]) 
            {
              stCurrentData.fLegAMaterialUsed = SplitByte.fData;
#ifdef DBG_CAN
              memset(rgcTemp, NULL , sizeof(rgcTemp));
              sprintf(rgcTemp,"lega Material used: %0.2f", 
                      stCurrentData.fLegAMaterialUsed );
              DEBUG_PRINTF("%s\r\n",rgcTemp);
#endif
            }
            else if(LEGB == pstCANrxmsg->Data[0])
            {
              stCurrentData.fLegBMaterialUsed = SplitByte.fData;
#ifdef DBG_CAN
              memset(rgcTemp, NULL , sizeof(rgcTemp));
              sprintf(rgcTemp,"leg B Material used: %0.2f", 
                      stCurrentData.fLegBMaterialUsed );
              DEBUG_PRINTF("%s\r\n",rgcTemp);
#endif	
            }else if(LEGC == pstCANrxmsg->Data[0])
            {	
              stCurrentData.fLegCMaterialUsed = SplitByte.fData;
#ifdef DBG_CAN
              memset(rgcTemp, NULL , sizeof(rgcTemp));
              sprintf(rgcTemp,"legc Material used: %0.2f",
                      stCurrentData.fLegCMaterialUsed );
              DEBUG_PRINTF("%s\r\n",rgcTemp);
#endif
            }
          }
          break;
          
          /*MFM value of pressure gauge1*/
        case DATA_PGAUGE1_VALUE:
          {
#ifdef DBG_CAN
            DEBUG_PRINTF("Msg ID: DATA_PGAUGE1_VALUE \r\n");
#endif
            for(ucLoopCnt=0; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            stCurrentData.fCurrPressureGauge1 = SplitByte.fData;
#ifdef DBG_CAN
            memset(rgcTemp, NULL , sizeof(rgcTemp));
            sprintf(rgcTemp,"Pressure_Gauge1_Curr %0.2f", 
                    stCurrentData.fCurrPressureGauge1);
            DEBUG_PRINTF("%s\r\n",rgcTemp);
#endif
            /* Array Updation For DATA LOGGING of PERIODIC LOG */
            rgucPeriodicLog[BYTE_FOURTYSEVEN] = ucRec_Data[DATA_BYTE_ZERO]; 
            rgucPeriodicLog[BYTE_FOURTYEIGHT] = ucRec_Data[DATA_BYTE_ONE]; 
            rgucPeriodicLog[BYTE_FOURTYNINE] = ucRec_Data[DATA_BYTE_TWO]; 
            rgucPeriodicLog[BYTE_FIFTY] = ucRec_Data[DATA_BYTE_THREE];
          }
          break;
          
          /*MFM value of pressure gauge2*/
        case DATA_PGAUGE2_VALUE:
          {
#ifdef DBG_CAN
            DEBUG_PRINTF("Msg ID: DATA_PGAUGE2_VALUE \r\n");
#endif
            for(ucLoopCnt = 0; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            stCurrentData.fCurrPressureGauge2 = SplitByte.fData;
#ifdef DBG_CAN
            memset(rgcTemp, NULL , sizeof(rgcTemp));
            sprintf(rgcTemp,"fdPressure_Gauge2_Curr %0.2f", 
                    `					stCurrentData.fCurrPressureGauge2);
            DEBUG_PRINTF("%s\r\n",rgcTemp);
#endif
            /* Array Updation For DATA LOGGING of PERIODIC LOG */
            rgucPeriodicLog[BYTE_FIFTYONE] = ucRec_Data[DATA_BYTE_ZERO]; 
            rgucPeriodicLog[BYTE_FIFTYTWO] = ucRec_Data[DATA_BYTE_ONE]; 
            rgucPeriodicLog[BYTE_FIFTYTHREE] = ucRec_Data[DATA_BYTE_TWO];
            rgucPeriodicLog[BYTE_FIFTYFOUR] = ucRec_Data[DATA_BYTE_THREE];
          }
          break;
          
          /*MFM value of pressure gauge3*/
        case DATA_PGAUGE3_VALUE:
          {
#ifdef DBG_CAN
            DEBUG_PRINTF("Msg ID: DATA_PGAUGE3_VALUE \r\n");
#endif
            for(ucLoopCnt=0; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            
            stCurrentData.fCurrPressureGauge3 = SplitByte.fData;
#ifdef DBG_CAN
            memset(rgcTemp, NULL , sizeof(rgcTemp));
            sprintf(rgcTemp,"fdPressure_Gauge3_Curr %0.2f",
                    stCurrentData.fCurrPressureGauge3);
            DEBUG_PRINTF("%s\r\n",rgcTemp);
#endif
            /* Array Updation For DATA LOGGING of PERIODIC LOG */
            rgucPeriodicLog[BYTE_FIFTYFIVE] = ucRec_Data[DATA_BYTE_ZERO]; 
            rgucPeriodicLog[BYTE_FIFTYSIX] = ucRec_Data[DATA_BYTE_ONE]; 
            rgucPeriodicLog[BYTE_FIFTYSEVEN] = ucRec_Data[DATA_BYTE_TWO]; 
            rgucPeriodicLog[BYTE_FIFTYEIGHT] = ucRec_Data[DATA_BYTE_THREE]; 
          }
          break;
          
          /*BHCM -  liquid level of base material*/
        case DATA_BHCM_LIQUID_LEVEL:
          {
#ifdef DBG_CAN
            DEBUG_PRINTF("Msg ID: DATA_BHCM_LIQUID_LEVEL \r\n");
#endif
            for(ucLoopCnt = 0; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            stCurrentData.fCurrLiquidLevelBase = SplitByte.fData;
#ifdef DBG_CAN
            memset(rgcTemp, NULL , sizeof(rgcTemp));
            sprintf(rgcTemp,"fdLiquid_Base_Level_Curr %0.2f",
                    stCurrentData.fCurrLiquidLevelBase);
            DEBUG_PRINTF("%s\r\n",rgcTemp);
#endif
            /* Array Updation For DATA LOGGING of PERIODIC LOG */
            rgucPeriodicLog[BYTE_FOURTYTHREE] = ucRec_Data[DATA_BYTE_ZERO]; 
            rgucPeriodicLog[BYTE_FOURTYFOUR] = ucRec_Data[DATA_BYTE_ONE]; 
            rgucPeriodicLog[BYTE_FOURTYFIVE] = ucRec_Data[DATA_BYTE_TWO];
            rgucPeriodicLog[BYTE_FOURTYSIX] = ucRec_Data[DATA_BYTE_THREE];
          }
          break;
          
          /*BHCM - Current temp of antifreeze liquid*/
        case DATA_BHCM_LIQUID_CURR_TEMP:
          {
#ifdef DBG_CAN
            DEBUG_PRINTF("Msg ID: DATA_BHCM_LIQUID_CURR_TEMP \r\n");
#endif							 
            for(ucLoopCnt = 0; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            stCurrentData.fCurrAntifreezeLiquidTemp = SplitByte.fData;
#ifdef DBG_CAN
            memset(rgcTemp, NULL , sizeof(rgcTemp));
            sprintf(rgcTemp,"fdLiquid_Antifreeze_TEMP_Curr %0.2f",
                    stCurrentData.fCurrAntifreezeLiquidTemp);
            DEBUG_PRINTF("%s\r\n",rgcTemp);
#endif
            /* Array Updation For DATA LOGGING of STATUS LOG */
            rgucStatusLog[BYTE_ELEVEN] = SplitByte.rgucData[0]; 
            rgucStatusLog[BYTE_TWELVE] = SplitByte.rgucData[1]; 
            rgucStatusLog[BYTE_THIRTEEN] = SplitByte.rgucData[2];
            rgucStatusLog[BYTE_FOURTEEN] = SplitByte.rgucData[3];
            /* Array Updation For DATA LOGGING of PERIODIC LOG */
            rgucPeriodicLog[BYTE_ELEVEN] = SplitByte.rgucData[0]; 
            rgucPeriodicLog[BYTE_TWELVE] = SplitByte.rgucData[1]; 
            rgucPeriodicLog[BYTE_THIRTEEN] = SplitByte.rgucData[2];
            rgucPeriodicLog[BYTE_FOURTEEN] = SplitByte.rgucData[3];
          }
          break;
          
          /*IHCM1 - current temp of inline base material*/
        case DATA_IHCM1_MATERIAL_CURR_TEMP:
          {
#ifdef DBG_CAN
            DEBUG_PRINTF("Msg ID: DATA_IHCM1_MATERIAL_CURR_TEMP \r\n");
#endif
            for(ucLoopCnt = 0; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            stCurrentData.fCurrInlineBaseMatTemp = SplitByte.fData;
#ifdef DBG_CAN
            memset(rgcTemp, NULL , sizeof(rgcTemp));
            sprintf(rgcTemp,"fdInline_Basemat_TEMP_Curr %0.2f",
                    stCurrentData.fCurrInlineBaseMatTemp);
            DEBUG_PRINTF("%s\r\n",rgcTemp);
#endif
            
            /* Array Updation For DATA LOGGING of STATUS LOG */
            rgucStatusLog[BYTE_FIFTEEN] = SplitByte.rgucData[0];
            rgucStatusLog[BYTE_SIXTEEN] = SplitByte.rgucData[1];
            rgucStatusLog[BYTE_SEVENTEEN] = SplitByte.rgucData[2]; 
            rgucStatusLog[BYTE_EIGHTEEN] = SplitByte.rgucData[3]; 
            /* Array Updation For DATA LOGGING of PERIODIC LOG */
            rgucPeriodicLog[BYTE_FIFTEEN] = SplitByte.rgucData[0];
            rgucPeriodicLog[BYTE_SIXTEEN] = SplitByte.rgucData[1];
            rgucPeriodicLog[BYTE_SEVENTEEN] = SplitByte.rgucData[2];
            rgucPeriodicLog[BYTE_EIGHTEEN] = SplitByte.rgucData[3]; 
          }
          break;
          
          /*IHCM4 current temp of inline hardener material*/
        case DATA_IHCM4_MATERIAL_CURR_TEMP:
          {
#ifdef DBG_CAN
            DEBUG_PRINTF("Msg ID: DATA_IHCM4_MATERIAL_CURR_TEMP \r\n");
#endif
            for(ucLoopCnt = 0; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            stCurrentData.fCurrInlineHardMatTemp = SplitByte.fData;
#ifdef DBG_CAN
            memset(rgcTemp, NULL , sizeof(rgcTemp));
            sprintf(rgcTemp,"fdInline_Hardmat_TEMP_Curr %0.2f",
                    stCurrentData.fCurrInlineHardMatTemp);
            DEBUG_PRINTF("%s\r\n",rgcTemp);
#endif
            
            /* Array Updation For DATA LOGGING of Status LOG */
            rgucStatusLog[BYTE_NINETEEN] = SplitByte.rgucData[0];
            rgucStatusLog[BYTE_TWENTY] = SplitByte.rgucData[1]; 
            rgucStatusLog[BYTE_TWENTYONE] = SplitByte.rgucData[2]; 
            rgucStatusLog[BYTE_TWENTYTWO] = SplitByte.rgucData[3]; 
            /* Array Updation For DATA LOGGING of Periodic LOG */	
            rgucPeriodicLog[BYTE_NINETEEN] = ucRec_Data[DATA_BYTE_ZERO]; 
            rgucPeriodicLog[BYTE_TWENTY] = ucRec_Data[DATA_BYTE_ONE]; 
            rgucPeriodicLog[BYTE_TWENTYONE] = ucRec_Data[DATA_BYTE_TWO]; 
            rgucPeriodicLog[BYTE_TWENTYTWO] = ucRec_Data[DATA_BYTE_THREE]; 
          }
          break;
          
          /*Stall test pressure change (in %)*/
        case DATA_STALL_PRESSURE_CHANGE:
          {
#ifdef DBG_CAN
            DEBUG_PRINTF("Msg ID: DATA_STALL_PRESSURE_CHANGE \r\n");
#endif
            
            
            
            if(DOWNSTROKE == ucRec_Data[DATA_BYTE_ZERO])
            {
              SplitByte.rgucData[0] = pstCANrxmsg->Data[1];
              SplitByte.rgucData[1] = pstCANrxmsg->Data[2];
              stCurrentData.iCurrPressureLegADown = 	SplitByte.iData;
#ifdef DBG_CAN
              DEBUG_PRINTF("iCurrPressureLegADown: %d\r\n", 
                           stCurrentData.iCurrPressureLegADown );
#endif
              
              SplitByte.rgucData[0] = pstCANrxmsg->Data[3];
              SplitByte.rgucData[1] = pstCANrxmsg->Data[4];
              stCurrentData.iCurrPressureLegBDown = SplitByte.iData;
#ifdef DBG_CAN
              DEBUG_PRINTF("iCurrPressureLegBDown: %d\r\n",
                           stCurrentData.iCurrPressureLegBDown);
#endif
              
              SplitByte.rgucData[0] = pstCANrxmsg->Data[5];
              SplitByte.rgucData[1] = pstCANrxmsg->Data[6];
              stCurrentData.iCurrPressureLegCDown = SplitByte.iData;
#ifdef DBG_CAN
              DEBUG_PRINTF("iCurrPressureLegCDown: %d\r\n", 
                           stCurrentData.iCurrPressureLegCDown );
#endif
            }
            if(UPSTROKE == pstCANrxmsg->Data[0])
            {
              SplitByte.rgucData[0] = pstCANrxmsg->Data[1];
              SplitByte.rgucData[1] = pstCANrxmsg->Data[2];
              stCurrentData.iCurrPressureLegAUp = SplitByte.iData;
#ifdef DBG_CAN
              DEBUG_PRINTF("iCurrPressureLegAUp: %d\r\n",
                           stCurrentData.iCurrPressureLegAUp);
#endif
              
              SplitByte.rgucData[0] = pstCANrxmsg->Data[3];
              SplitByte.rgucData[1] = pstCANrxmsg->Data[4];
              stCurrentData.iCurrPressureLegBUp = SplitByte.iData;
#ifdef DBG_CAN
              DEBUG_PRINTF("iCurrPressureLegBUp: %d\r\n", 
                           stCurrentData.iCurrPressureLegBUp);
#endif
              
              SplitByte.rgucData[0] = pstCANrxmsg->Data[5];
              SplitByte.rgucData[1] = pstCANrxmsg->Data[6];
              stCurrentData.iCurrPressureLegCUp = SplitByte.iData;
#ifdef DBG_CAN
              DEBUG_PRINTF("iCurrPressureLegCUp: %d\r\n", 
                           stCurrentData.iCurrPressureLegCUp);
#endif
            }									
          }
          break;
          
          /*Prime/Spray/Flush process time*/
        case DATA_PROCESS_TIME:
          {
#ifdef DBG_CAN
            DEBUG_PRINTF("Msg ID: DATA_PROCESS_TIME \r\n");
#endif
            for(ucLoopCnt = 0; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt+1];
            }
            
            if(PRIME_PROCESS == pstCANrxmsg->Data[0])
            {
              stCurrentData.fCurrPrimeTime = SplitByte.fData;
#ifdef DBG_CAN
              memset(rgcTemp, NULL , sizeof(rgcTemp));
              sprintf(rgcTemp,"fdTime_Prime_Curr : %0.2f",
                      stCurrentData.fCurrPrimeTime );
              DEBUG_PRINTF("%s\r\n",rgcTemp);
#endif
            }
            else if(SPRAY_PROCESS == pstCANrxmsg->Data[0])
            {
              stCurrentData.fCurrSprayTime = SplitByte.fData;
#ifdef DBG_CAN
              memset(rgcTemp, NULL , sizeof(rgcTemp));
              sprintf(rgcTemp,"fdTime_Spray_Curr : %0.2f",
                      stCurrentData.fCurrSprayTime );
              DEBUG_PRINTF("%s\r\n",rgcTemp);
#endif
            }
            else if(FLUSH_PROCESS == pstCANrxmsg->Data[0])
            {
              stCurrentData.fCurrFlushTime = SplitByte.fData;
#ifdef DBG_CAN
              memset(rgcTemp, NULL , sizeof(rgcTemp));
              sprintf(rgcTemp,"fdTime_Flush_Curr : %0.2f",
                      stCurrentData.fCurrFlushTime );
              DEBUG_PRINTF("%s\r\n",rgcTemp);
#endif
            }
            /*Send to OP*/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & 0xFFFF0000) | 
                                  (DEST_OP | SOURCE_MCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            fnCANMsg_Transmit(&stTx1Message, CAN1);
          }
          break;
          
          /*OPMM no of rotations completed*/
        case DATA_ROTATIONS_COMPLETED:
          {
#ifdef DBG_CAN
            DEBUG_PRINTF("Msg ID: DATA_ROTATIONS_COMPLETED \r\n");
#endif
            
            stCurrentData.ucCurrNumberofRotation = pstCANrxmsg->Data[0];
#ifdef DBG_CAN
            DEBUG_PRINTF("Num_Rotation_Curr: %d", 
                         stCurrentData.ucCurrNumberofRotation);
#endif
            /*Send to OP*/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & 0xFFFF0000) | 
                                  (DEST_OP | SOURCE_MCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            stTx1Message.Data[0] = pstCANrxmsg->Data[0];
            fnCANMsg_Transmit(&stTx1Message, CAN1);
          }
          break;
          
          /*OPMM battery status in %*/
        case DATA_OPMM_BATTERY_STATUS:
          {
#ifdef DBG_CAN
            DEBUG_PRINTF("Msg ID: DATA_OPMM_BATTERY_STATUS \r\n");
#endif
            for(ucLoopCnt=0; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            stCurrentData.fCurrBateryStatus = SplitByte.fData;
#ifdef DBG_CAN
            memset(rgcTemp, NULL , sizeof(rgcTemp));
            sprintf(rgcTemp,"fdBat_Status_Curr: %0.2f", 
                    stCurrentData.fCurrBateryStatus );
            DEBUG_PRINTF("%s\r\n",rgcTemp);
#endif
            /*Send to OP*/
            pstCANrxmsg->ExtId = ((pstCANrxmsg->ExtId & 0xFFFF0000) | 
                                  (DEST_OP | SOURCE_MCU));
            stTx1Message.ExtId = pstCANrxmsg->ExtId; 
            stTx1Message.DLC = pstCANrxmsg->DLC; 
            for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            fnCANMsg_Transmit(&stTx1Message, CAN1);
          }
          break;
        }
      }
      break;/*DATA Msgs Case ends*/
      
    case MSG_TYPE_CONFIG:
      {
#ifdef DBG_CAN
        DEBUG_PRINTF("Msg Type: MSG_TYPE_CONFIG \r\n");
#endif
        switch (uliCanID & MSG_MASK)
        {
        case ACKNOWLEDGEMENT:
          {
            fnCANPost_Ack(pstCANrxmsg,CAN1);
          }break;
          
          /*Spray pressure value*/
        case CONFIG_SPRAY_PRESSURE:
          {
#ifdef DBG_CAN_EE
            DEBUG_PRINTF("Msg ID: CONFIG_SPRAY_PRESSURE \r\n");
#endif
            
            for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            stConfigData.fConfigSprayPressure = SplitByte.fData;
            ucEEPROMCounter = CONFIG_PARA_1;
#ifdef DBG_CAN_EE
            memset(rgcTemp, NULL , sizeof(rgcTemp));
            sprintf(rgcTemp,"fdBat_Status_Curr: %0.2f",
                    stConfigData.fConfigSprayPressure);
            DEBUG_PRINTF("%s\r\n",rgcTemp);
#endif
          }
          break;
          
          /*Re-circulation pressure value*/
        case CONFIG_RECIRC_PRESSURE:
          {
#ifdef DBG_CAN_EE
            DEBUG_PRINTF("Msg ID: CONFIG_RECIRC_PRESSURE \r\n");
#endif
            
            for(ucLoopCnt=0; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            stConfigData.fConfigRecirPressure = SplitByte.fData;
            ucEEPROMCounter = CONFIG_PARA_2;
#ifdef DBG_CAN_EE
            memset(rgcTemp, NULL , sizeof(rgcTemp));
            sprintf(rgcTemp,"CONFIG_RECIRC_PRESSURE: %0.2f", 
                    stConfigData.fConfigRecirPressure);
            DEBUG_PRINTF("%s\r\n",rgcTemp);
#endif
          }
          break;
          
          /*OPMM pipe temp (non-contact)*/
        case CONFIG_PIPE_TEMP_TH:
          {
#ifdef DBG_CAN_EE
            DEBUG_PRINTF("Msg ID: CONFIG_PIPE_TEMP_TH \r\n");
#endif
            for(ucLoopCnt = 1; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              SplitByte.rgucData[ucLoopCnt - 1] = pstCANrxmsg->Data[ucLoopCnt];
            }
            if(LOW_TH == pstCANrxmsg->Data[0] )
            {
              ucEEPROMCounter = CONFIG_PARA_3;
              stConfigData.fConfigPipeTempLTH = SplitByte.fData;
#ifdef DBG_CAN_EE
              memset(rgcTemp, NULL , sizeof(rgcTemp));
              sprintf(rgcTemp,"CONFIG_PIPE_TEMP_TH LOW: %0.2f", 
                      stConfigData.fConfigPipeTempLTH);
              DEBUG_PRINTF("%s\r\n",rgcTemp);
#endif
            }
            else if(HIGH_TH == pstCANrxmsg->Data[0])
            {
              ucEEPROMCounter = CONFIG_PARA_4;
              stConfigData.fConfigPipeTempHTH= SplitByte.fData;
#ifdef DBG_CAN_EE
              memset(rgcTemp, NULL , sizeof(rgcTemp));
              sprintf(rgcTemp,"CONFIG_PIPE_TEMP_TH HIGH: %0.2f", 
                      stConfigData.fConfigPipeTempHTH);
              DEBUG_PRINTF("%s\r\n", rgcTemp);
#endif
            }
          }
          break;
          
          /*OPMM rotation counter for spraying process*/
        case CONFIG_SPRAY_COUNT:
          {
#ifdef DBG_CAN_EE
            DEBUG_PRINTF("Msg ID: CONFIG_SPRAY_COUNT \r\n");
#endif
            ucEEPROMCounter = CONFIG_PARA_5;
            stConfigData.ucConfigSprayCount = pstCANrxmsg->Data[0];
#ifdef DBG_CAN_EE
            DEBUG_PRINTF("CAN PARSER CONFIG_SPRAY_COUNT: %d\r\n", 
                         stConfigData.ucConfigSprayCount );
#endif
            
            stTx1Message.DLC = pstCANrxmsg->DLC;
            for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            SplitByte.uliData = pstCANrxmsg->ExtId;
            SplitByte.rgucData[0] = (uint8_t)(SOURCE_MCU & 0xff);
            SplitByte.rgucData[1] = (uint8_t)(DEST_OPMM >> 8);
            stTx1Message.ExtId = SplitByte.uliData;
            fnCANMsg_Transmit(&stTx1Message, CAN1);
          }
          break;
          
          /*OPMM timer value for prime process*/
        case CONFIG_PRIME_TIME:
          {
#ifdef DBG_CAN_EE
            DEBUG_PRINTF("Msg ID: CONFIG_PRIME_TIME \r\n");
#endif
            
            for(ucLoopCnt=0; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            ucEEPROMCounter = CONFIG_PARA_6;
            stConfigData.fConfigPrimeTime = SplitByte.fData;						
#ifdef DBG_CAN_EE
            memset(rgcTemp, NULL , sizeof(rgcTemp));
            sprintf(rgcTemp,"CONFIG_PRIME_TIME: %0.2f", 
                    stConfigData.fConfigPrimeTime );
            DEBUG_PRINTF("%s\r\n",rgcTemp);
#endif
            stTx1Message.DLC = pstCANrxmsg->DLC;
            for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            SplitByte.uliData = pstCANrxmsg->ExtId;
            SplitByte.rgucData[0] = (uint8_t)(SOURCE_MCU & 0xff);
            SplitByte.rgucData[1] = (uint8_t)(DEST_OPMM >> 8);
            stTx1Message.ExtId = SplitByte.uliData;
            fnCANMsg_Transmit(&stTx1Message, CAN1);
          }
          break;
          
          /*Gun delay time - OPMM*/
        case CONFIG_OPMM_GUN_DELAY:
          {
#ifdef DBG_CAN_EE
            DEBUG_PRINTF("Msg ID: CONFIG_OPMM_GUN_DELAY \r\n");
#endif
            
            for(ucLoopCnt=0; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            ucEEPROMCounter = CONFIG_PARA_7;
            stConfigData.fConfigGunDelayTime = SplitByte.fData;
#ifdef DBG_CAN_EE
            memset(rgcTemp, NULL , sizeof(rgcTemp));
            sprintf(rgcTemp,"CONFIG_OPMM_GUN_DELAY: %0.2f", 
                    stConfigData.fConfigGunDelayTime );
            DEBUG_PRINTF("%s\r\n",rgcTemp);
#endif
            stTx1Message.DLC = pstCANrxmsg->DLC;
            for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            SplitByte.uliData = pstCANrxmsg->ExtId;
            SplitByte.rgucData[0] = (uint8_t)(SOURCE_MCU & 0xff);
            SplitByte.rgucData[1] = (uint8_t)(DEST_OPMM >> 8);
            stTx1Message.ExtId = SplitByte.uliData;
            fnCANMsg_Transmit(&stTx1Message, CAN1);
          }
          break;
          
          /*OPMM - spray overlap time*/
        case CONFIG_OPMM_SPRAY_OVERLAP_TIME:
          {
#ifdef DBG_CAN_EE
            DEBUG_PRINTF("Msg ID: CONFIG_OPMM_SPRAY_OVERLAP_TIME \r\n");
#endif
            for(ucLoopCnt = 0; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            ucEEPROMCounter = CONFIG_PARA_8;
            stConfigData.fConfigSprayOverlapTime = SplitByte.fData;
#ifdef DBG_CAN_EE
            memset(rgcTemp, NULL , sizeof(rgcTemp));
            sprintf(rgcTemp,"CONFIG_OPMM_SPRAY_OVERLAP_TIME: %0.2f", 
                    stConfigData.fConfigSprayOverlapTime );
            DEBUG_PRINTF("%s\r\n",rgcTemp);
#endif
            stTx1Message.DLC = pstCANrxmsg->DLC;
            for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            SplitByte.uliData = pstCANrxmsg->ExtId;
            SplitByte.rgucData[0] = (uint8_t)(SOURCE_MCU & 0xff);
            SplitByte.rgucData[1] = (uint8_t)(DEST_OPMM >> 8);
            stTx1Message.ExtId = SplitByte.uliData;
            fnCANMsg_Transmit(&stTx1Message, CAN1);
          }
          break;
          
          /*Reverse delay time - OPMM*/
        case CONFIG_OPMM_REVERSE_DELAY:
          {
#ifdef DBG_CAN_EE
            DEBUG_PRINTF("Msg ID: CONFIG_OPMM_REVERSE_DELAY \r\n");
#endif
            for(ucLoopCnt=0; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            ucEEPROMCounter = CONFIG_PARA_9;
            stConfigData.fConfigReverseDelayTime = SplitByte.fData;
#ifdef DBG_CAN_EE
            memset(rgcTemp, NULL , sizeof(rgcTemp));
            sprintf(rgcTemp,"CONFIG_OPMM_REVERSE_DELAY: %0.2f",
                    stConfigData.fConfigReverseDelayTime);
            DEBUG_PRINTF("%s\r\n",rgcTemp);
#endif
            
            stTx1Message.DLC = pstCANrxmsg->DLC;
            for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            SplitByte.uliData = pstCANrxmsg->ExtId;
            SplitByte.rgucData[0] = (uint8_t)(SOURCE_MCU & 0xff);
            SplitByte.rgucData[1] = (uint8_t)(DEST_OPMM >> 8);
            stTx1Message.ExtId = SplitByte.uliData;
            fnCANMsg_Transmit(&stTx1Message, CAN1);
          }
          break;
          
          /*Timer value for flush process*/
        case CONFIG_OPMM_FLUSH_TIME:
          {
#ifdef DBG_CAN_EE
            DEBUG_PRINTF("Msg ID: CONFIG_OPMM_FLUSH_TIME \r\n");
#endif
            
            for(ucLoopCnt=0; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            ucEEPROMCounter = CONFIG_PARA_10;
            stConfigData.fConfigFlushTime = SplitByte.fData;
#ifdef DBG_CAN_EE
            memset(rgcTemp, NULL , sizeof(rgcTemp));
            sprintf(rgcTemp,"CONFIG_OPMM_FLUSH_TIME: %0.2f", 
                    stConfigData.fConfigFlushTime );
            DEBUG_PRINTF("%s\r\n",rgcTemp);
#endif
            stTx1Message.DLC = pstCANrxmsg->DLC;
            for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            SplitByte.uliData = pstCANrxmsg->ExtId;
            SplitByte.rgucData[0] = (uint8_t)(SOURCE_MCU & 0xff);
            SplitByte.rgucData[1] = (uint8_t)(DEST_OPMM >> 8);
            stTx1Message.ExtId = SplitByte.uliData;
            fnCANMsg_Transmit(&stTx1Message, CAN1);
          }
          break;
          
          /*To validate the stall test result with low and high value - MFM*/
        case CONFIG_MFM_STALL_TH:
          {
#ifdef DBG_CAN_EE
            DEBUG_PRINTF("Msg ID: CONFIG_MFM_STALL_TH \r\n");
#endif
            if(DOWNSTROKE_TH == pstCANrxmsg->Data[0])
            {
              ucEEPROMCounter = CONFIG_PARA_12;
              SplitByte.rgucData[0]	= pstCANrxmsg->Data[1];
              SplitByte.rgucData[1]	= pstCANrxmsg->Data[2];
              stConfigData.iConfigDownStrokeTH	= SplitByte.iData;							
#ifdef DBG_CAN_EE
              DEBUG_PRINTF("%d\r\n", stConfigData.iConfigDownStrokeTH);
#endif
            }
            
            if(UPSTROKE_TH == pstCANrxmsg->Data[0])
            {
              ucEEPROMCounter = CONFIG_PARA_11;
              SplitByte.rgucData[0]	= pstCANrxmsg->Data[1];
              SplitByte.rgucData[1]	= pstCANrxmsg->Data[2];
              stConfigData.iConfigUpStrokeTH = SplitByte.iData;
#ifdef DBG_CAN_EE
              DEBUG_PRINTF("%d\r\n", stConfigData.iConfigUpStrokeTH);
#endif
            }
            
            /*Transmit to MFM*/
            stTx1Message.DLC = pstCANrxmsg->DLC;
            for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            SplitByte.uliData = pstCANrxmsg->ExtId;
            SplitByte.rgucData[0] = (uint8_t)(SOURCE_MCU & 0xff);
            SplitByte.rgucData[1] = (uint8_t)(DEST_MFM >> 8);
            stTx1Message.ExtId = SplitByte.uliData;
            fnCANMsg_Transmit(&stTx1Message, CAN1);
          }
          break;
          
          /*MFM pressure gauge1 threshold*/
        case CONFIG_MFM_PGAUGE1_TH:
          {
#ifdef DBG_CAN_EE
            DEBUG_PRINTF("Msg ID: CONFIG_MFM_PGAUGE1_TH \r\n");
#endif
            for(ucLoopCnt = 1; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              SplitByte.rgucData[ucLoopCnt - 1] = pstCANrxmsg->Data[ucLoopCnt];
            }
            if(LOW_TH == pstCANrxmsg->Data[0])
            {
              ucEEPROMCounter = CONFIG_PARA_13;
              stConfigData.fConfigPressureGauge1LTH = SplitByte.fData;
#ifdef DBG_CAN_EE
              memset(rgcTemp, NULL , sizeof(rgcTemp));
              sprintf(rgcTemp,"CONFIG_MFM_PGAUGE1_TH LOW: %0.2f",
                      stConfigData.fConfigPressureGauge1LTH );
              DEBUG_PRINTF("%s\r\n",rgcTemp);
#endif
            }
            if(HIGH_TH == pstCANrxmsg->Data[0])
            {
              ucEEPROMCounter = CONFIG_PARA_14;
              stConfigData.fConfigPressureGauge1HTH= SplitByte.fData;
#ifdef DBG_CAN_EE
              memset(rgcTemp, NULL , sizeof(rgcTemp));
              sprintf(rgcTemp,"CONFIG_MFM_PGAUGE1_TH HIGH: %0.2f",
                      stConfigData.fConfigPressureGauge1HTH );
              DEBUG_PRINTF("%s\r\n",rgcTemp);
#endif
            }
            stTx1Message.DLC = pstCANrxmsg->DLC;
            for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            SplitByte.uliData = pstCANrxmsg->ExtId;
            SplitByte.rgucData[0] = (uint8_t)(SOURCE_MCU & 0xff);
            SplitByte.rgucData[1] = (uint8_t)(DEST_MFM >> 8);
            stTx1Message.ExtId = SplitByte.uliData;
            fnCANMsg_Transmit(&stTx1Message, CAN1);
          }
          break;
          
          /*MFM pressure gauge2 threshold*/
        case CONFIG_MFM_PGAUGE2_TH:
          {
#ifdef DBG_CAN_EE
            DEBUG_PRINTF("Msg ID: CONFIG_MFM_PGAUGE2_TH \r\n");
#endif
            for(ucLoopCnt = 1; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              SplitByte.rgucData[ucLoopCnt - 1] = pstCANrxmsg->Data[ucLoopCnt];
            }
            if(LOW_TH == pstCANrxmsg->Data[0])
            {
              ucEEPROMCounter = CONFIG_PARA_15;
              stConfigData.fConfigPressureGauge2LTH = SplitByte.fData;
#ifdef DBG_CAN_EE
              memset(rgcTemp, NULL , sizeof(rgcTemp));
              sprintf(rgcTemp,"CONFIG_MFM_PGAUGE2_TH LOW: %0.2f",
                      stConfigData.fConfigPressureGauge2LTH );
              DEBUG_PRINTF("%s\r\n",rgcTemp);
#endif
            }
            if(HIGH_TH == pstCANrxmsg->Data[0])
            {
              ucEEPROMCounter = CONFIG_PARA_16;
              stConfigData.fConfigPressureGauge2HTH= SplitByte.fData;
#ifdef DBG_CAN_EE
              memset(rgcTemp, NULL , sizeof(rgcTemp));
              sprintf(rgcTemp,"CONFIG_MFM_PGAUGE2_TH HIGH: %0.2f",
                      stConfigData.fConfigPressureGauge2HTH );
              DEBUG_PRINTF("%s\r\n",rgcTemp);
#endif
            }
            stTx1Message.DLC = pstCANrxmsg->DLC;
            for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            SplitByte.uliData = pstCANrxmsg->ExtId;
            SplitByte.rgucData[0] = (uint8_t)(SOURCE_MCU & 0xff);
            SplitByte.rgucData[1] = (uint8_t)(DEST_MFM >> 8);
            stTx1Message.ExtId = SplitByte.uliData;
            fnCANMsg_Transmit(&stTx1Message, CAN1);
          }
          break;
          
          
          /*MFM pressure gauge3 threshold*/
        case CONFIG_MFM_PGAUGE3_TH:
          {
#ifdef DBG_CAN_EE
            DEBUG_PRINTF("Msg ID: CONFIG_MFM_PGAUGE3_TH \r\n");
#endif
            for(ucLoopCnt = 1; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              SplitByte.rgucData[ucLoopCnt - 1] = pstCANrxmsg->Data[ucLoopCnt];
            }
            if(LOW_TH == pstCANrxmsg->Data[0])
            {
              ucEEPROMCounter = CONFIG_PARA_17;
              stConfigData.fConfigPressureGauge3LTH = SplitByte.fData;
#ifdef DBG_CAN_EE
              memset(rgcTemp, NULL , sizeof(rgcTemp));
              sprintf(rgcTemp,"CONFIG_MFM_PGAUGE3_TH LOW: %0.2f",
                      stConfigData.fConfigPressureGauge3LTH );
              DEBUG_PRINTF("%s\r\n",rgcTemp);
#endif
            }
            if(HIGH_TH == pstCANrxmsg->Data[0])
            {
              ucEEPROMCounter = CONFIG_PARA_18;
              stConfigData.fConfigPressureGauge3HTH= SplitByte.fData;
#ifdef DBG_CAN_EE
              memset(rgcTemp, NULL , sizeof(rgcTemp));
              sprintf(rgcTemp,"CONFIG_MFM_PGAUGE3_TH HIGH: %0.2f",
                      stConfigData.fConfigPressureGauge3HTH);
              DEBUG_PRINTF("%s\r\n",rgcTemp);
#endif
            }
            stTx1Message.DLC = pstCANrxmsg->DLC;
            for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            SplitByte.uliData = pstCANrxmsg->ExtId;
            SplitByte.rgucData[0] = (uint8_t)(SOURCE_MCU & 0xff);
            SplitByte.rgucData[1] = (uint8_t)(DEST_MFM >> 8);
            stTx1Message.ExtId = SplitByte.uliData;
            fnCANMsg_Transmit(&stTx1Message, CAN1);
          }
          break;
          
          /*BHCM base tank temp setting*/
        case CONFIG_BHCM_BTANK_SET_TEMP:
          {
#ifdef DBG_CAN_EE
            DEBUG_PRINTF("Msg ID: CONFIG_BHCM_BTANK_SET_TEMP \r\n");
#endif
            
            for(ucLoopCnt = 0; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            ucEEPROMCounter = CONFIG_PARA_19;
            stConfigData.fConfigBaseTempSet = SplitByte.fData;
#ifdef DBG_CAN_EE
            memset(rgcTemp, NULL , sizeof(rgcTemp));
            sprintf(rgcTemp,"CONFIG_BHCM_BTANK_SET_TEMP: %0.2f",
                    stConfigData.fConfigBaseTempSet );
            DEBUG_PRINTF("%s\r\n",rgcTemp);
#endif
            stTx1Message.DLC = pstCANrxmsg->DLC;
            for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            SplitByte.uliData = pstCANrxmsg->ExtId;
            SplitByte.rgucData[0] = (uint8_t)(SOURCE_MCU & 0xff);
            SplitByte.rgucData[1] = (uint8_t)(DEST_BHCM >> 8);
            stTx1Message.ExtId = SplitByte.uliData;
            fnCANMsg_Transmit(&stTx1Message, CAN1);
          }
          break;
          
          /*BHCM base tank temp threshold*/
        case CONFIG_BHCM_BTANK_TEMP_TH:
          {
#ifdef DBG_CAN
            DEBUG_PRINTF("Msg ID: CONFIG_BHCM_BTANK_TEMP_TH \r\n");
#endif
            for(ucLoopCnt = 1; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              SplitByte.rgucData[ucLoopCnt - 1] = pstCANrxmsg->Data[ucLoopCnt];
            }
            if(LOW_TH == pstCANrxmsg->Data[0])
            {
              ucEEPROMCounter = CONFIG_PARA_20;
              stConfigData.fConfigBaseTempLTH = SplitByte.fData;
#ifdef DBG_CAN_EE
              memset(rgcTemp, NULL , sizeof(rgcTemp));
              sprintf(rgcTemp,"CONFIG_BHCM_BTANK_TEMP_TH LOW: %0.2f",
                      stConfigData.fConfigBaseTempLTH);
              DEBUG_PRINTF("%s\r\n",rgcTemp);
#endif
            }
            
            if(HIGH_TH == pstCANrxmsg->Data[0])
            {
              ucEEPROMCounter = CONFIG_PARA_21;
              stConfigData.fConfigBaseTempHTH= SplitByte.fData;
#ifdef DBG_CAN_EE
              memset(rgcTemp, NULL , sizeof(rgcTemp));
              sprintf(rgcTemp,"CONFIG_BHCM_BTANK_TEMP_TH HIGH: %0.2f",
                      stConfigData.fConfigBaseTempHTH );
              DEBUG_PRINTF("%s\r\n",rgcTemp);
#endif
            }
            stTx1Message.DLC = pstCANrxmsg->DLC;
            for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            SplitByte.uliData = pstCANrxmsg->ExtId;
            SplitByte.rgucData[0] = (uint8_t)(SOURCE_MCU & 0xff);
            SplitByte.rgucData[1] = (uint8_t)(DEST_BHCM >> 8);
            stTx1Message.ExtId = SplitByte.uliData;
            fnCANMsg_Transmit(&stTx1Message, CAN1);
          }
          break;
          
          /*BHCM base tank liquid level threshold*/
        case CONFIG_BHCM_BTANK_LIQUID_TH:
          {
#ifdef DBG_CAN
            DEBUG_PRINTF("Msg ID: CONFIG_BHCM_BTANK_LIQUID_TH \r\n");
#endif
            
            for(ucLoopCnt = 1; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              SplitByte.rgucData[ucLoopCnt - 1] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            
            if(LOW_TH == pstCANrxmsg->Data[0])
            {
              ucEEPROMCounter = CONFIG_PARA_22;
              stConfigData.fConfigBaseLiquidLevelLTH= SplitByte.fData;
#ifdef DBG_CAN_EE
              memset(rgcTemp, NULL , sizeof(rgcTemp));
              sprintf(rgcTemp,"CONFIG_BHCM_BTANK_LIQUID_TH LOW: %0.2f", 
                      stConfigData.fConfigBaseLiquidLevelLTH);
              DEBUG_PRINTF("%s\r\n",rgcTemp);
#endif
            }
            
            if(HIGH_TH == pstCANrxmsg->Data[0])
            {
              ucEEPROMCounter = CONFIG_PARA_23;
              stConfigData.fConfigBaseLiquidLevelHTH = SplitByte.fData;
#ifdef DBG_CAN_EE
              memset(rgcTemp, NULL , sizeof(rgcTemp));
              sprintf(rgcTemp,"CONFIG_BHCM_BTANK_LIQUID_TH HIGH: %0.2f",
                      stConfigData.fConfigBaseLiquidLevelHTH );
              DEBUG_PRINTF("%s\r\n",rgcTemp);
#endif
            }
            stTx1Message.DLC = pstCANrxmsg->DLC;
            for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            SplitByte.uliData = pstCANrxmsg->ExtId;
            SplitByte.rgucData[0] = (uint8_t)(SOURCE_MCU & 0xff);
            SplitByte.rgucData[1] = (uint8_t)(DEST_BHCM >> 8);
            stTx1Message.ExtId = SplitByte.uliData;
            fnCANMsg_Transmit(&stTx1Message, CAN1);
          }
          break;
          
          /*Set temp of inline base material*/
        case CONFIG_IHCM_BLINE_SET_TEMP:
          {
#ifdef DBG_CAN
            DEBUG_PRINTF("Msg ID: CONFIG_IHCM_BLINE_SET_TEMP \r\n");
#endif
            for(ucLoopCnt=0; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            ucEEPROMCounter = CONFIG_PARA_24;
            stConfigData.fConfigInlineBaseTempSet = SplitByte.fData;
#ifdef DBG_CAN_EE
            memset(rgcTemp, NULL , sizeof(rgcTemp));
            sprintf(rgcTemp,"CONFIG_IHCM_BLINE_SET_TEMP: %0.2f",
                    stConfigData.fConfigInlineBaseTempSet);
            DEBUG_PRINTF("%s\r\n",rgcTemp);
#endif
            stTx1Message.DLC = pstCANrxmsg->DLC;
            for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            SplitByte.uliData = pstCANrxmsg->ExtId;
            SplitByte.rgucData[0] = (uint8_t)(SOURCE_MCU & 0xff);
            SplitByte.rgucData[1] = (uint8_t)(DEST_IHCM1 >> 8);
            stTx1Message.ExtId = SplitByte.uliData;
            fnCANMsg_Transmit(&stTx1Message, CAN1);
          }
          break;
          
          /*Set temp threshold of inline base material*/
        case CONFIG_IHCM_BLINE_TEMP_TH:
          {
#ifdef DBG_CAN_EE
            DEBUG_PRINTF("Msg ID: CONFIG_IHCM_BLINE_TEMP_TH \r\n");
#endif
            for(ucLoopCnt = 1; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              SplitByte.rgucData[ucLoopCnt - 1] = pstCANrxmsg->Data[ucLoopCnt];
            }
            if(LOW_TH == pstCANrxmsg->Data[0])
            {
              ucEEPROMCounter = CONFIG_PARA_25;
              stConfigData.fConfigInlineBaseTempLTH = SplitByte.fData;
#ifdef DBG_CAN_EE
              memset(rgcTemp, NULL , sizeof(rgcTemp));
              sprintf(rgcTemp,"CONFIG_IHCM_BLINE_TEMP_TH LOW: %0.2f",
                      stConfigData.fConfigInlineBaseTempLTH );
              DEBUG_PRINTF("%s\r\n", rgcTemp);
#endif
            }
            if(HIGH_TH == pstCANrxmsg->Data[0])
            {
              ucEEPROMCounter = CONFIG_PARA_26;
              stConfigData.fConfigInlineBaseTempHTH = SplitByte.fData;
#ifdef DBG_CAN_EE
              memset(rgcTemp, NULL , sizeof(rgcTemp));
              sprintf(rgcTemp,"CONFIG_IHCM_BLINE_TEMP_TH HIGH: %0.2f",
                      stConfigData.fConfigInlineBaseTempHTH);
              DEBUG_PRINTF("%s\r\n",rgcTemp);
#endif
            }
            stTx1Message.DLC = pstCANrxmsg->DLC;
            for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            SplitByte.uliData = pstCANrxmsg->ExtId;
            SplitByte.rgucData[0] = (uint8_t)(SOURCE_MCU & 0xff);
            SplitByte.rgucData[1] = (uint8_t)(DEST_IHCM1 >> 8);
            stTx1Message.ExtId = SplitByte.uliData;
            fnCANMsg_Transmit(&stTx1Message, CAN1);
          }
          break;
          
          /*IHCM4 hardener inline temp setting*/
        case CONFIG_IHCM4_HLINE_SET_TEMP:
          {
#ifdef DBG_CAN
            DEBUG_PRINTF("Msg ID: CONFIG_IHCM4_HLINE_SET_TEMP \r\n");
#endif
            
            for(ucLoopCnt=0; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            ucEEPROMCounter = CONFIG_PARA_27;
            stConfigData.fConfigInlineHardTempSet= SplitByte.fData;
#ifdef DBG_CAN_EE
            memset(rgcTemp, NULL , sizeof(rgcTemp));
            sprintf(rgcTemp,"CONFIG_IHCM4_HLINE_SET_TEMP: %0.2f", 
                    stConfigData.fConfigInlineHardTempSet );
            DEBUG_PRINTF("%s\r\n",rgcTemp);
#endif
            /*Send config data to IHCM4*/
            stTx1Message.DLC = pstCANrxmsg->DLC;
            for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            SplitByte.uliData = pstCANrxmsg->ExtId;
            SplitByte.rgucData[0] = (uint8_t)(SOURCE_MCU & 0xff);
            SplitByte.rgucData[1] = (uint8_t)(DEST_IHCM4_HM >> 8);
            stTx1Message.ExtId = SplitByte.uliData;
            fnCANMsg_Transmit(&stTx1Message, CAN1);
            
          }
          break;
          
          /*IHCM4 hardener inline temp threshold*/
        case CONFIG_IHCM4_HLINE_TEMP_TH:
          {
#ifdef DBG_CAN_EE
            DEBUG_PRINTF("Msg ID: CONFIG_IHCM4_HLINE_TEMP_TH \r\n");
#endif
            for(ucLoopCnt = 1; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              SplitByte.rgucData[ucLoopCnt - 1] = pstCANrxmsg->Data[ucLoopCnt];
            }
            if(LOW_TH == pstCANrxmsg->Data[0])
            {
              ucEEPROMCounter = CONFIG_PARA_28;
              stConfigData.fConfigInlineHardTempLTH = SplitByte.fData;
#ifdef DBG_CAN_EE
              memset(rgcTemp, NULL , sizeof(rgcTemp));
              sprintf(rgcTemp,"CONFIG_IHCM4_HLINE_TEMP_TH LOW: %0.2f",
                      stConfigData.fConfigInlineHardTempLTH );
              DEBUG_PRINTF("%s\r\n",rgcTemp);
#endif
            }
            if(HIGH_TH == pstCANrxmsg->Data[0])
            {
              ucEEPROMCounter = CONFIG_PARA_29;
              stConfigData.fConfigInlineHardTempHTH = SplitByte.fData;
#ifdef DBG_CAN_EE
              memset(rgcTemp, NULL , sizeof(rgcTemp));
              sprintf(rgcTemp,"CONFIG_IHCM4_HLINE_TEMP_TH HIGH: %0.2f", 
                      stConfigData.fConfigInlineHardTempHTH );
              DEBUG_PRINTF("%s\r\n",rgcTemp);
#endif
            }
            /*Sendig config to IHCM4*/
            stTx1Message.DLC = pstCANrxmsg->DLC;
            for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            SplitByte.uliData = pstCANrxmsg->ExtId;
            SplitByte.rgucData[0] = (uint8_t)(SOURCE_MCU & 0xff);
            SplitByte.rgucData[1] = (uint8_t)(DEST_IHCM4_HM >> 8);
            stTx1Message.ExtId = SplitByte.uliData;
            fnCANMsg_Transmit(&stTx1Message, CAN1);
            
          }
          break;
          
          /*Operation time*/
        case CONFIG_OPERATION_TIME:
          {
#ifdef DBG_CAN
            DEBUG_PRINTF("Msg ID: CONFIG_OPERATION_TIME \r\n");
#endif
            for(ucLoopCnt = 0; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            ucEEPROMCounter = CONFIG_PARA_30;
            stConfigData.fConfigOperationTime = SplitByte.fData;
#ifdef DBG_CAN_EE
            memset(rgcTemp, NULL , sizeof(rgcTemp));
            sprintf(rgcTemp,"CONFIG_OPERATION_TIME: %0.2f",
                    stConfigData.fConfigOperationTime );
            DEBUG_PRINTF("%s\r\n", rgcTemp);
#endif
          }
          break;
          
          /*Joint number*/
        case CONFIG_JOINT_NUMBER:
          {
#ifdef DBG_CAN
            DEBUG_PRINTF("Msg ID: CONFIG_JOINT_NUMBER \r\n");
#endif
            
            for(ucLoopCnt = 0; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            ucEEPROMCounter = CONFIG_PARA_31;
            stConfigData.fConfigJointNum = SplitByte.fData;
#ifdef DBG_CAN_EE
            memset(rgcTemp, NULL , sizeof(rgcTemp));
            sprintf(rgcTemp,"CONFIG_JOINT_NUMBER : %0.2f", 
                    stConfigData.fConfigJointNum );
            DEBUG_PRINTF("%s\r\n", rgcTemp );
#endif
            /* Array Updation For DATA LOGGING of Status LOG */
            rgucStatusLog[BYTE_SEVEN] = SplitByte.rgucData[0];
            rgucStatusLog[BYTE_EIGHT] = SplitByte.rgucData[1];
            rgucStatusLog[BYTE_NINE] = SplitByte.rgucData[2]; 
            rgucStatusLog[BYTE_TEN] = SplitByte.rgucData[3];  
            /* Array Updation For DATA LOGGING of Periodic LOG */
            rgucPeriodicLog[BYTE_SEVEN] = SplitByte.rgucData[0]; 
            rgucPeriodicLog[BYTE_EIGHT] = SplitByte.rgucData[1];
            rgucPeriodicLog[BYTE_NINE] = SplitByte.rgucData[2];
            rgucPeriodicLog[BYTE_TEN] = SplitByte.rgucData[3];
          }
          break;
          
          /*Period of data logging*/
        case CONFIG_DATA_LOG_PERIOD:
          {
#ifdef DBG_CAN
            DEBUG_PRINTF("Msg ID: CONFIG_DATA_LOG_PERIOD \r\n");
#endif
            for(ucLoopCnt=0; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            ucEEPROMCounter = CONFIG_PARA_32;
            stConfigData.fConfigDataLoggingPeriod = SplitByte.fData;
#ifdef DBG_CAN_EE
            memset(rgcTemp, NULL , sizeof(rgcTemp));
            sprintf(rgcTemp,"CONFIG_DATA_LOG_PERIOD: %0.2f", 
                    stConfigData.fConfigDataLoggingPeriod );
            DEBUG_PRINTF("%s\r\n",rgcTemp);
#endif
          }
          break;
          
          /*System date and time*/
        case CONFIG_SYS_DATE_TIME:
          {
#ifdef DBG_CAN
            DEBUG_PRINTF("Msg ID: CONFIG_SYS_DATE_TIME \r\n");
#endif
            
            for(ucLoopCnt = 0; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              stConfigData.ucConfigSystemTime[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            fnSPI_EEPROM_WriteData(SYS_TIME, 7,
                                   stConfigData.ucConfigSystemTime);/*EEPROM Write*/
            /* Updating the Time in the RTC registers*/
            stRTC_TimeStructure.RTC_Hours = stConfigData.ucConfigSystemTime[0];
            stRTC_TimeStructure.RTC_Minutes = stConfigData.ucConfigSystemTime[1];
            stRTC_TimeStructure.RTC_Seconds = stConfigData.ucConfigSystemTime[2];
            stRTC_TimeStructure.RTC_H12 = RTC_H12_AM;
            
            stRTC_DateStructure.RTC_Date = stConfigData.ucConfigSystemTime[3];
            stRTC_DateStructure.RTC_Month = stConfigData.ucConfigSystemTime[4];
            stRTC_DateStructure.RTC_Year = stConfigData.ucConfigSystemTime[6];/*Year Low Byte*/
            
            RTC_SetDate(RTC_Format_BIN, &stRTC_DateStructure);
            RTC_SetTime(RTC_Format_BIN, &stRTC_TimeStructure);
          }
          break;
          
        case CONFIG_CYLINDER_SURFACE_AREA :
          {
#ifdef DBG_CAN      
            DEBUG_PRINTF("Surface area of cylinder\r\n");
#endif	
            /* splitting the data received from the CAN */
            for(ucLoopCnt = 1; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              SplitByte.rgucData[ucLoopCnt - 1] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            if(BIT0_SET == pstCANrxmsg->Data[0])
            {
              ucEEPROMCounter = CONFIG_PARA_34;
              stConfigData.fConfigAreaofCylinderA = SplitByte.fData;								
            }else if(BIT1_SET == pstCANrxmsg->Data[0])
            {
              ucEEPROMCounter = CONFIG_PARA_35;
              stConfigData.fConfigAreaofCylinderB = SplitByte.fData;
            }
            else if(BIT2_SET == pstCANrxmsg->Data[0])
            {
              ucEEPROMCounter = CONFIG_PARA_36;
              stConfigData.fConfigAreaofCylinderC = SplitByte.fData;
            }
            
            stTx1Message.DLC = pstCANrxmsg->DLC;
            for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            SplitByte.uliData = pstCANrxmsg->ExtId;
            SplitByte.rgucData[0] = (uint8_t)(SOURCE_MCU & 0xff);
            SplitByte.rgucData[1] = (uint8_t)(DEST_MFM >> 8);
            stTx1Message.ExtId = SplitByte.uliData;
            fnCANMsg_Transmit(&stTx1Message, CAN1);
          }break;
          
          /*Number of cylinder*/
          
        case CONFIG_NUMBER_OF_CYLINDER:
          {
#ifdef DBG_CAN
            DEBUG_PRINTF("Msg ID: CONFIG_NUMBER_OF_CYLINDER \r\n");
#endif
            ucEEPROMCounter = CONFIG_PARA_37;
            stConfigData.ucConfigCylinderCount = pstCANrxmsg->Data[0];
#ifdef DBG_CAN_EE
            memset(rgcTemp, NULL , sizeof(rgcTemp));
            sprintf(rgcTemp,"CONFIG_NUMBER_OF_CYLINDER: %d", 
                    stConfigData.ucConfigCylinderCount );
            DEBUG_PRINTF("%s\r\n",rgcTemp);
#endif
            
            stTx1Message.DLC = pstCANrxmsg->DLC;
            for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            SplitByte.uliData = pstCANrxmsg->ExtId;
            SplitByte.rgucData[0] = (uint8_t)(SOURCE_MCU & 0xff);
            SplitByte.rgucData[1] = (uint8_t)(DEST_MFM >> 8);
            stTx1Message.ExtId = SplitByte.uliData;
            fnCANMsg_Transmit(&stTx1Message, CAN1);
          }
          break;
          
          /* Supervisor name*/
        case CONFIG_SUPERVISOR_NAME:
          {
#ifdef DBG_CAN
            DEBUG_PRINTF("Msg ID: CONFIG_SUPERVISOR_NAME \r\n");
#endif
            if(FRAME_1 == pstCANrxmsg->Data[0])
            {
              for(ucLoopCnt = 1; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
              {
                stConfigData.rgucConfigSupervisorName1[ucLoopCnt - 1] = 
                  pstCANrxmsg->Data[ucLoopCnt];
              }
              stConfigData.rgucConfigSupervisorName1[ucLoopCnt - 1] = NULL;
              ucEEPROMCounter = CONFIG_PARA_38;
#ifdef DBG_CAN_EE
              DEBUG_PRINTF("%s\r\n",stConfigData.rgucConfigSupervisorName1);
#endif
            }
            if(FRAME_2 == pstCANrxmsg->Data[0])
            {
              for(ucLoopCnt = 1; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
              {
                stConfigData.rgucConfigSupervisorName2[ucLoopCnt - 1] = 
                  pstCANrxmsg->Data[ucLoopCnt];
              }
              stConfigData.rgucConfigSupervisorName2[ucLoopCnt - 1] = NULL;
              ucEEPROMCounter = CONFIG_PARA_39;
#ifdef DBG_CAN_EE
              DEBUG_PRINTF("%s\r\n", stConfigData.rgucConfigSupervisorName2);
#endif
            }
            if(FRAME_3 == pstCANrxmsg->Data[0])
            {
              for(ucLoopCnt = 1; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
              {
                stConfigData.rgucConfigSupervisorName3[ucLoopCnt - 1] = 
                  pstCANrxmsg->Data[ucLoopCnt];
              }
              stConfigData.rgucConfigSupervisorName3[ucLoopCnt - 1] = NULL;
              ucEEPROMCounter = CONFIG_PARA_40;
#ifdef DBG_CAN_EE
              DEBUG_PRINTF("%s\r\n",stConfigData.rgucConfigSupervisorName3);
#endif
            }
          }
          break;
          
          /*Coating Material*/
        case CONFIG_COATING_MATERIAL:
          {
#ifdef DBG_CAN_EE
            DEBUG_PRINTF("Msg ID: CONFIG_COATING_MATERIAL \r\n");
#endif
            
            if(FRAME_1 == pstCANrxmsg->Data[0])
            {
              for(ucLoopCnt = 1; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
              {
                stConfigData.rgucConfigCoatingMatName1[ucLoopCnt - 1] = 
                  pstCANrxmsg->Data[ucLoopCnt];
              }
              ucEEPROMCounter = CONFIG_PARA_41;
              stConfigData.rgucConfigCoatingMatName1[ucLoopCnt - 1] = NULL;
#ifdef DBG_CAN_EE
              DEBUG_PRINTF("%s\r\n", stConfigData.rgucConfigCoatingMatName1);
#endif
            }
            if(FRAME_2 == pstCANrxmsg->Data[0])
            {
              for(ucLoopCnt = 1; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
              {
                stConfigData.rgucConfigCoatingMatName2[ucLoopCnt - 1] =
                  pstCANrxmsg->Data[ucLoopCnt];
              }
              ucEEPROMCounter = CONFIG_PARA_42;
              stConfigData.rgucConfigCoatingMatName2[ucLoopCnt - 1] = NULL;
#ifdef DBG_CAN_EE
              DEBUG_PRINTF("%s\r\n", stConfigData.rgucConfigCoatingMatName2);
#endif
            }
            if(FRAME_3 == pstCANrxmsg->Data[0])
            {
              for(ucLoopCnt = 1; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
              {
                stConfigData.rgucConfigCoatingMatName3[ucLoopCnt - 1] = 
                  pstCANrxmsg->Data[ucLoopCnt];
              }
              stConfigData.rgucConfigCoatingMatName3[ucLoopCnt - 1] = NULL;
              ucEEPROMCounter = CONFIG_PARA_43;
#ifdef DBG_CAN_EE
              DEBUG_PRINTF("%s\r\n",stConfigData.rgucConfigCoatingMatName3);
#endif
            }
          }
          break;
          
          /*Base material batch no*/
        case CONFIG_BASE_MAT_BATCH_NO:
          {
#ifdef DBG_CAN_EE
            DEBUG_PRINTF("Msg ID: CONFIG_BASE_MAT_BATCH_NO \r\n");
#endif
            
            if(FRAME_1 == pstCANrxmsg->Data[0])
            {
              for(ucLoopCnt = 1; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
              {
                stConfigData.rgucConfigBaseMatBatchNo1[ucLoopCnt - 1] = 
                  pstCANrxmsg->Data[ucLoopCnt];
              }
              stConfigData.rgucConfigBaseMatBatchNo1[ucLoopCnt - 1] = NULL;
              ucEEPROMCounter = CONFIG_PARA_44;
#ifdef DBG_CAN_EE
              DEBUG_PRINTF("%s\r\n",stConfigData.rgucConfigBaseMatBatchNo1);
#endif
            }
            if(FRAME_2 == pstCANrxmsg->Data[0])
            {
              for(ucLoopCnt = 1; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
              {
                stConfigData.rgucConfigBaseMatBatchNo2[ucLoopCnt - 1] = 
                  pstCANrxmsg->Data[ucLoopCnt];
              }
              ucEEPROMCounter = CONFIG_PARA_45;
              stConfigData.rgucConfigBaseMatBatchNo2[ucLoopCnt - 1] = NULL;
#ifdef DBG_CAN_EE
              DEBUG_PRINTF("%s\r\n",stConfigData.rgucConfigBaseMatBatchNo2);
#endif
            }
            if(FRAME_3 == pstCANrxmsg->Data[0])
            {
              for(ucLoopCnt = 1; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
              {
                stConfigData.rgucConfigBaseMatBatchNo3[ucLoopCnt - 1] = 
                  pstCANrxmsg->Data[ucLoopCnt];
              }
              stConfigData.rgucConfigBaseMatBatchNo3[ucLoopCnt - 1] = NULL;
              ucEEPROMCounter = CONFIG_PARA_46;
#ifdef DBG_CAN_EE
              DEBUG_PRINTF("%s\r\n",stConfigData.rgucConfigBaseMatBatchNo3);
#endif
            }
          }
          break;
          
          /*Hardener material batch no*/
        case CONFIG_HARD_MAT_BATCH_NO:
          {
#ifdef DBG_CAN_EE
            DEBUG_PRINTF("Msg ID: CONFIG_HARD_MAT_BATCH_NO \r\n");
#endif
            if(FRAME_1 == pstCANrxmsg->Data[0])
            {
              for(ucLoopCnt = 1; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
              {
                stConfigData.rgucConfigHardMatBatchNo1[ucLoopCnt - 1] =
                  pstCANrxmsg->Data[ucLoopCnt];
              }
              stConfigData.rgucConfigHardMatBatchNo1[ucLoopCnt - 1] = NULL;
              ucEEPROMCounter = CONFIG_PARA_47;
#ifdef DBG_CAN_EE
              DEBUG_PRINTF("%s\r\n",stConfigData.rgucConfigHardMatBatchNo1);
#endif
            }
            if(FRAME_2 == pstCANrxmsg->Data[0])
            {
              for(ucLoopCnt = 1; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
              {
                stConfigData.rgucConfigHardMatBatchNo2[ucLoopCnt - 1] =
                  pstCANrxmsg->Data[ucLoopCnt];
              }
              stConfigData.rgucConfigHardMatBatchNo2[ucLoopCnt - 1] = NULL;
              ucEEPROMCounter = CONFIG_PARA_48;
#ifdef DBG_CAN_EE
              DEBUG_PRINTF("%s\r\n",stConfigData.rgucConfigHardMatBatchNo2);
#endif
            }
            if(FRAME_3 == pstCANrxmsg->Data[0])
            {
              for(ucLoopCnt = 1; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
              {
                stConfigData.rgucConfigHardMatBatchNo3[ucLoopCnt - 1] =
                  pstCANrxmsg->Data[ucLoopCnt];
              }
              stConfigData.rgucConfigHardMatBatchNo3[ucLoopCnt - 1] = NULL;
              ucEEPROMCounter = CONFIG_PARA_49;
#ifdef DBG_CAN_EE
              DEBUG_PRINTF("%c\r\n",stConfigData.rgucConfigHardMatBatchNo3);
#endif
            }
          }
          break;
          
          /*Time before stall*/
        case CONFIG_TIME_BEFORE_STALL:
          {
#ifdef DBG_CAN_EE
            DEBUG_PRINTF("Msg ID: CONFIG_TIME_BEFORE_STALL \r\n");
#endif
            
            for(ucLoopCnt = 0; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            ucEEPROMCounter = CONFIG_PARA_50;
            stConfigData.fConfigTimeBeforeStall = SplitByte.fData;
#ifdef DBG_CAN_EE
            memset(rgcTemp, NULL , sizeof(rgcTemp));
            sprintf(rgcTemp,"CONFIG_TIME_BEFORE_STALL: %0.2f",
                    stConfigData.fConfigTimeBeforeStall );
            DEBUG_PRINTF("%s\r\n",rgcTemp);
#endif
            stTx1Message.DLC = pstCANrxmsg->DLC;
            for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            SplitByte.uliData = pstCANrxmsg->ExtId;
            SplitByte.rgucData[0] = (uint8_t)(SOURCE_MCU & 0xff);
            SplitByte.rgucData[1] = (uint8_t)(DEST_MFM >> 8);
            stTx1Message.ExtId = SplitByte.uliData;
            fnCANMsg_Transmit(&stTx1Message, CAN1);
          }
          break;
          
          /*Time before comparing stall pressure*/
        case CONFIG_TIME_BEFORE_COMPARE_STALL:
          {
#ifdef DBG_CAN_EE
            DEBUG_PRINTF("Msg ID: CONFIG_TIME_BEFORE_COMPARE_STALL \r\n");
#endif
            
            for(ucLoopCnt = 0; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            ucEEPROMCounter = CONFIG_PARA_51;
            stConfigData.fConfigTimeCompareStall = SplitByte.fData;
#ifdef DBG_CAN_EE
            memset(rgcTemp, NULL , sizeof(rgcTemp));
            sprintf(rgcTemp,"CONFIG_TIME_BEFORE_COMPARE_STALL: %0.2f",
                    stConfigData.fConfigTimeCompareStall);
            DEBUG_PRINTF("%s\r\n",rgcTemp);
#endif
            stTx1Message.DLC = pstCANrxmsg->DLC;
            for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            SplitByte.uliData = pstCANrxmsg->ExtId;
            SplitByte.rgucData[0] = (uint8_t)(SOURCE_MCU & 0xff);
            SplitByte.rgucData[1] = (uint8_t)(DEST_MFM >> 8);
            stTx1Message.ExtId = SplitByte.uliData;
            fnCANMsg_Transmit(&stTx1Message, CAN1);
          }
          break;
          
          /*DCLS-HOME reach timer*/
        case CONFIG_DCLS_HOME_TIME:
          {
#ifdef DBG_CAN_EE
            DEBUG_PRINTF("Msg ID: CONFIG_DCLS_HOME_TIME \r\n");
#endif
            
            for(ucLoopCnt = 0; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt+1];
            }
            
            if(DCLS_TO_HOME_TIME == pstCANrxmsg->Data[0])
            {
              ucEEPROMCounter = CONFIG_PARA_52;
              stConfigData.fConfigDCLSHomeTime = SplitByte.fData;
#ifdef DBG_CAN_EE
              memset(rgcTemp, NULL , sizeof(rgcTemp));
              sprintf(rgcTemp,"CONFIG_DCLS_HOME_TIME : %0.2f", 
                      stConfigData.fConfigDCLSHomeTime );
              DEBUG_PRINTF("%s\r\n",rgcTemp);
#endif
            }
            if(HOME_TO_DCLS_TIME == pstCANrxmsg->Data[0])
            {
              ucEEPROMCounter = CONFIG_PARA_53;
              stConfigData.fConfigHomeDCLSTime = SplitByte.fData;
#ifdef DBG_CAN_EE
              memset(rgcTemp, NULL , sizeof(rgcTemp));
              sprintf(rgcTemp,"CONFIG_DCLS_HOME_TIME: %0.2f",
                      stConfigData.fConfigHomeDCLSTime );
              DEBUG_PRINTF("%s\r\n",rgcTemp);
#endif
            }
            stTx1Message.DLC = pstCANrxmsg->DLC;
            for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            SplitByte.uliData = pstCANrxmsg->ExtId;
            SplitByte.rgucData[0] = (uint8_t)(SOURCE_MCU & 0xff);
            SplitByte.rgucData[1] = (uint8_t)(DEST_OPMM >> 8);
            stTx1Message.ExtId = SplitByte.uliData;
            fnCANMsg_Transmit(&stTx1Message, CAN1);
          }
          break;
          
          /*Gun position change time*/
        case CONFIG_GUN_POSITION_CHANGE_TIME:
          {
#ifdef DBG_CAN_EE
            DEBUG_PRINTF("Msg ID: CONFIG_GUN_POSITION_CHANGE_TIME \r\n");
#endif
            for(ucLoopCnt = 0; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            ucEEPROMCounter = CONFIG_PARA_54;
            stConfigData.fConfigGunPosChangeTime = SplitByte.fData;
#ifdef DBG_CAN_EE
            memset(rgcTemp, NULL , sizeof(rgcTemp));
            sprintf(rgcTemp,"CONFIG_GUN_POSITION_CHANE_TIME: %0.2f",
                    stConfigData.fConfigGunPosChangeTime);
            DEBUG_PRINTF("%s\r\n",rgcTemp);
#endif
            stTx1Message.DLC = pstCANrxmsg->DLC;
            for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            SplitByte.uliData = pstCANrxmsg->ExtId;
            SplitByte.rgucData[0] = (uint8_t)(SOURCE_MCU & 0xff);
            SplitByte.rgucData[1] = (uint8_t)(DEST_OPMM >> 8);
            stTx1Message.ExtId = SplitByte.uliData;
            fnCANMsg_Transmit(&stTx1Message, CAN1);
          }
          break;
          
        case CONFIG_SPRAY_VALVE_OPEN_TIME_VBAT:
          {
#ifdef DBG_CAN_EE
            DEBUG_PRINTF("Msg ID: CONFIG_SPRAY_VALVE_OPEN_TIME_VBAT \r\n");
#endif
            
            for(ucLoopCnt = 0; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            ucEEPROMCounter = CONFIG_PARA_55;
            stConfigData.fConfigTimebeforeONSprayValve = SplitByte.fData;
#ifdef DBG_CAN_EE
            memset(rgcTemp, NULL , sizeof(rgcTemp));
            sprintf(rgcTemp,"fConfigTimebeforeONSprayValve: %0.2f",
                    stConfigData.fConfigTimebeforeONSprayValve);
            DEBUG_PRINTF("%s\r\n",rgcTemp);
#endif
            stTx1Message.DLC = pstCANrxmsg->DLC;
            for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            SplitByte.uliData = pstCANrxmsg->ExtId;
            SplitByte.rgucData[0] = (uint8_t)(SOURCE_MCU & 0xff);
            SplitByte.rgucData[1] = (uint8_t)(DEST_OPMM >> 8);
            stTx1Message.ExtId = SplitByte.uliData;
            fnCANMsg_Transmit(&stTx1Message, CAN1);
          }
          break;
          
        case CONFIG_TYPE_OF_UNIT:
          {
            ucEEPROMCounter = CONFIG_PARA_71;
            stConfigData.ucTypeofUnit = pstCANrxmsg->Data[0];
#ifdef DBG_CAN_EE
            DEBUG_PRINTF("CONFIG_TYPE_OF_UNIT: %d\r\n",
                         stConfigData.ucTypeofUnit);
#endif
          }
          break;
          
        case CONFIG_TEMP_HYSTERESIS:
          {
#ifdef DBG_CAN_EE
            DEBUG_PRINTF("Msg ID: CONFIG_TEMP_HYSTERESIS\r\n");
#endif
            for(ucLoopCnt = 1; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              SplitByte.rgucData[ucLoopCnt - 1] = pstCANrxmsg->Data[ucLoopCnt];
            }
            stTx1Message.DLC = pstCANrxmsg->DLC;
            for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            if(BIT0_SET == pstCANrxmsg->Data[0])
            {
              ucEEPROMCounter = CONFIG_PARA_56;
              stConfigData.fConfigHysBasePos = SplitByte.fData;
#ifdef DBG_CAN_EE
              memset(rgcTemp, NULL , sizeof(rgcTemp));
              sprintf(rgcTemp,"fConfigHysBasePos: %0.2f", 
                      stConfigData.fConfigHysBasePos);
              DEBUG_PRINTF("%s\r\n",rgcTemp);
#endif
              
              SplitByte.uliData = pstCANrxmsg->ExtId;
              SplitByte.rgucData[0] = (uint8_t)(SOURCE_MCU & 0xff);
              
              SplitByte.rgucData[1] = (uint8_t)(DEST_IHCM1 >> 8);
              stTx1Message.ExtId = SplitByte.uliData;
              fnCANMsg_Transmit(&stTx1Message, CAN1);
              
            }
            else if(BIT1_SET == pstCANrxmsg->Data[0])
            {
              ucEEPROMCounter = CONFIG_PARA_57;
              stConfigData.fConfigHysBaseNeg = SplitByte.fData;
#ifdef DBG_CAN_EE
              memset(rgcTemp, NULL , sizeof(rgcTemp));
              sprintf(rgcTemp,"fConfigHysBaseNeg: %0.2f",
                      stConfigData.fConfigHysBaseNeg);
              DEBUG_PRINTF("%s\r\n",rgcTemp);
#endif
              SplitByte.uliData = pstCANrxmsg->ExtId;
              SplitByte.rgucData[0] = (uint8_t)(SOURCE_MCU & 0xff);
              
              SplitByte.rgucData[1] = (uint8_t)(DEST_IHCM1 >> 8);
              stTx1Message.ExtId = SplitByte.uliData;
              fnCANMsg_Transmit(&stTx1Message, CAN1);
            }
            else if(BIT2_SET == pstCANrxmsg->Data[0])
            {
              ucEEPROMCounter = CONFIG_PARA_58;
              stConfigData.fConfigHysHardPos = SplitByte.fData;
#ifdef DBG_CAN_EE
              memset(rgcTemp, NULL , sizeof(rgcTemp));
              sprintf(rgcTemp,"fConfigHysHardPos: %0.2f",
                      stConfigData.fConfigHysHardPos);
              DEBUG_PRINTF("%s\r\n",rgcTemp);
#endif
              SplitByte.uliData = pstCANrxmsg->ExtId;
              SplitByte.rgucData[0] = (uint8_t)(SOURCE_MCU & 0xff);
              
              SplitByte.rgucData[1] = (uint8_t)(DEST_IHCM4_HM >> 8);
              stTx1Message.ExtId = SplitByte.uliData;
              fnCANMsg_Transmit(&stTx1Message, CAN1);
            }
            else if(BIT3_SET == pstCANrxmsg->Data[0])
            {
              ucEEPROMCounter = CONFIG_PARA_59;
              stConfigData.fConfigHysHardNeg = SplitByte.fData;
#ifdef DBG_CAN_EE
              memset(rgcTemp, NULL , sizeof(rgcTemp));
              sprintf(rgcTemp,"fConfigHysHardNeg: %0.2f",
                      stConfigData.fConfigHysHardNeg);
              DEBUG_PRINTF("%s\r\n",rgcTemp);
#endif
              SplitByte.uliData = pstCANrxmsg->ExtId;
              SplitByte.rgucData[0] = (uint8_t)(SOURCE_MCU & 0xff);
              
              SplitByte.rgucData[1] = (uint8_t)(DEST_IHCM4_HM >> 8);
              stTx1Message.ExtId = SplitByte.uliData;
              fnCANMsg_Transmit(&stTx1Message, CAN1);
            }
            else if(BIT4_SET == pstCANrxmsg->Data[0])
            {
              ucEEPROMCounter = CONFIG_PARA_60;
              stConfigData.fConfigHysAntiFrzPos = SplitByte.fData;
#ifdef DBG_CAN_EE
              memset(rgcTemp, NULL , sizeof(rgcTemp));
              sprintf(rgcTemp,"CfConfigHysAntiFrzPos: %0.2f",
                      stConfigData.fConfigHysAntiFrzPos);
              DEBUG_PRINTF("%s\r\n",rgcTemp);
#endif
              SplitByte.uliData = pstCANrxmsg->ExtId;
              SplitByte.rgucData[0] = (uint8_t)(SOURCE_MCU & 0xff);
              
              SplitByte.rgucData[1] = (uint8_t)(DEST_BHCM >> 8);
              stTx1Message.ExtId = SplitByte.uliData;
              fnCANMsg_Transmit(&stTx1Message, CAN1);
            }
            else if(BIT5_SET == pstCANrxmsg->Data[0])
            {
              ucEEPROMCounter = CONFIG_PARA_61;
              stConfigData.fConfigHysAntiFrzNeg = SplitByte.fData;
#ifdef DBG_CAN_EE
              memset(rgcTemp, NULL , sizeof(rgcTemp));
              sprintf(rgcTemp,"fConfigHysAntiFrzNeg: %0.2f",
                      stConfigData.fConfigHysAntiFrzNeg);
              DEBUG_PRINTF("%s\r\n",rgcTemp);
#endif
              SplitByte.uliData = pstCANrxmsg->ExtId;
              SplitByte.rgucData[0] = (uint8_t)(SOURCE_MCU & 0xff);
              
              SplitByte.rgucData[1] = (uint8_t)(DEST_BHCM >> 8);
              stTx1Message.ExtId = SplitByte.uliData;
              fnCANMsg_Transmit(&stTx1Message, CAN1);
            }
          }
          break;
          
        case CONFIG_PRESSURE_HYSTERESIS:
          {
#ifdef DBG_CAN_EE
            DEBUG_PRINTF("Msg ID: CONFIG_PRESSURE HYSTERESIS \r\n");
#endif
            
            for(ucLoopCnt = 1; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              SplitByte.rgucData[ucLoopCnt - 1] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            if(BIT0_SET == pstCANrxmsg->Data[0])
            {
              ucEEPROMCounter = CONFIG_PARA_62;
              stConfigData.fConfigHysRecircPresPos = SplitByte.fData;
#ifdef DBG_CAN_EE
              memset(rgcTemp, NULL , sizeof(rgcTemp));
              sprintf(rgcTemp,"fConfigHysRecircPresPos: %0.2f",
                      stConfigData.fConfigHysRecircPresPos);
              DEBUG_PRINTF("%s\r\n",rgcTemp);
#endif
            }
            else if(BIT1_SET == pstCANrxmsg->Data[0])
            {
              ucEEPROMCounter = CONFIG_PARA_63;
              stConfigData.fConfigHysRecircPresNeg = SplitByte.fData;
#ifdef DBG_CAN_EE
              memset(rgcTemp, NULL , sizeof(rgcTemp));
              sprintf(rgcTemp,"fConfigHysRecircPresNeg: %0.2f",
                      stConfigData.fConfigHysRecircPresNeg);
              DEBUG_PRINTF("%s\r\n",rgcTemp);
#endif
            }
            else if(BIT2_SET == pstCANrxmsg->Data[0])
            {
              ucEEPROMCounter = CONFIG_PARA_64;
              stConfigData.fConfigHysSprayPresPos = SplitByte.fData;
#ifdef DBG_CAN_EE
              memset(rgcTemp, NULL , sizeof(rgcTemp));
              sprintf(rgcTemp,"fConfigHysSprayPresPos: %0.2f",
                      stConfigData.fConfigHysSprayPresPos);
              DEBUG_PRINTF("%s\r\n",rgcTemp);
#endif
              
            }
            else if(BIT3_SET == pstCANrxmsg->Data[0])
            {
              ucEEPROMCounter = CONFIG_PARA_65;
              stConfigData.fConfigHysSprayPresNeg = SplitByte.fData;
#ifdef DBG_CAN_EE
              memset(rgcTemp, NULL , sizeof(rgcTemp));
              sprintf(rgcTemp,"fConfigHysSprayPresNeg: %0.2f",
                      stConfigData.fConfigHysSprayPresNeg);
              DEBUG_PRINTF("%s\r\n",rgcTemp);
#endif
            }
          }
          break;
          
          /*HHCM base tank liquid level threshold*/
        case CONFIG_HHCM_LIQUID_LEVEL_TH:
          {
#ifdef DBG_CAN
            DEBUG_PRINTF("Msg ID: CONFIG_HHCM_BTANK_LIQUID_TH \r\n");
#endif
            
            for(ucLoopCnt = 1; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              SplitByte.rgucData[ucLoopCnt - 1] = pstCANrxmsg->Data[ucLoopCnt];
            }
            if(LOW_TH == pstCANrxmsg->Data[0])
            {
              ucEEPROMCounter = CONFIG_PARA_67;
              stConfigData.fConfigHardLiquidLevelLTH = SplitByte.fData;
#ifdef DBG_CAN_EE
              memset(rgcTemp, NULL , sizeof(rgcTemp));
              sprintf(rgcTemp,"CONFIG_HHCM_BTANK_LIQUID_TH LOW: %0.2f",
                      stConfigData.fConfigHardLiquidLevelLTH);
              DEBUG_PRINTF("%s\r\n",rgcTemp);
#endif
            }
            if(HIGH_TH == pstCANrxmsg->Data[0])
            {
              ucEEPROMCounter = CONFIG_PARA_66;
              stConfigData.fConfigHardLiquidLevelHTH = SplitByte.fData;
#ifdef DBG_CAN_EE
              memset(rgcTemp, NULL , sizeof(rgcTemp));
              sprintf(rgcTemp,"CONFIG_HHCM_BTANK_LIQUID_TH HIGH: %0.2f",
                      stConfigData.fConfigHardLiquidLevelHTH );
              DEBUG_PRINTF("%s\r\n",rgcTemp);
#endif
            }
            stTx1Message.DLC = pstCANrxmsg->DLC;
            for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            SplitByte.uliData = pstCANrxmsg->ExtId;
            SplitByte.rgucData[0] = (uint8_t)(SOURCE_MCU & 0xff);
            SplitByte.rgucData[1] = (uint8_t)(DEST_HHCM >> 8);
            stTx1Message.ExtId = SplitByte.uliData;
            fnCANMsg_Transmit(&stTx1Message, CAN1);
          }
          break;
          
          
        case CONFIG_HCM_LIQUID_TANK_HEIGHT:
          {
#ifdef DBG_CAN
            DEBUG_PRINTF("Msg ID: CONFIG_HCM_LIQUID_TANK_HEIGHT \r\n");
#endif
            
            for(ucLoopCnt = 1; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              SplitByte.rgucData[ucLoopCnt - 1] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            stTx1Message.DLC = pstCANrxmsg->DLC;
            for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            
            if(BHCM_TANK_HEIHGT == pstCANrxmsg->Data[0])
            {
              ucEEPROMCounter = CONFIG_PARA_68;
              stConfigData.fConfigBaseTankHeight = SplitByte.fData;
#ifdef DBG_CAN_EE
              memset(rgcTemp, NULL , sizeof(rgcTemp));
              sprintf(rgcTemp,"CONFIG_BHCM_BTANK_LIQUID_height : %0.2f",
                      stConfigData.fConfigBaseTankHeight);
              DEBUG_PRINTF("%s\r\n",rgcTemp);
#endif
              SplitByte.uliData = pstCANrxmsg->ExtId;
              SplitByte.rgucData[0] = (uint8_t)(SOURCE_MCU & 0xff);
              SplitByte.rgucData[1] = (uint8_t)(DEST_BHCM >> 8);
              stTx1Message.ExtId = SplitByte.uliData;
              fnCANMsg_Transmit(&stTx1Message, CAN1);
            }
            
            if(HHCM_TANK_HEIHGT == pstCANrxmsg->Data[0])
            {
              ucEEPROMCounter = CONFIG_PARA_69;
              stConfigData.fConfigHardTankHeight = SplitByte.fData;
#ifdef DBG_CAN_EE
              memset(rgcTemp, NULL , sizeof(rgcTemp));
              sprintf(rgcTemp,"CONFIG_HHCM_BTANK_LIQUID_HEIGHT: %0.2f",
                      stConfigData.fConfigHardTankHeight );
              DEBUG_PRINTF("%s\r\n",rgcTemp);
#endif
              SplitByte.uliData = pstCANrxmsg->ExtId;
              SplitByte.rgucData[0] = (uint8_t)(SOURCE_MCU & 0xff);
              SplitByte.rgucData[1] = (uint8_t)(DEST_HHCM >> 8);
              stTx1Message.ExtId = SplitByte.uliData;
              fnCANMsg_Transmit(&stTx1Message, CAN1);
            }
          }
          break;
        case CONFIG_LL_WINDOW_SIZE:
          {
#ifdef DBG_CAN
            DEBUG_PRINTF("Msg ID: CONFIG_LL_WINDOW_SIZE \r\n");
#endif
            ucEEPROMCounter = CONFIG_PARA_70;
            stConfigData.ucConfigLLWindowSize = pstCANrxmsg->Data[0];
#ifdef DBG_CAN_EE
            DEBUG_PRINTF("fConfigLLWindowSize: %d\r\n",
                         stConfigData.ucConfigLLWindowSize );
#endif
            
            stTx1Message.DLC = pstCANrxmsg->DLC;
            stTx1Message.Data[0] = pstCANrxmsg->Data[0];
            SplitByte.uliData = pstCANrxmsg->ExtId;
            /*To BHCM*/
            SplitByte.rgucData[0] = (uint8_t)(SOURCE_MCU & 0xff);
            SplitByte.rgucData[1] = (uint8_t)(DEST_BHCM >> 8);
            stTx1Message.ExtId = SplitByte.uliData;
            fnCANMsg_Transmit(&stTx1Message, CAN1);
            /*to HHCM*/
            SplitByte.rgucData[1] = (uint8_t)(DEST_HHCM >> 8);
            stTx1Message.ExtId = SplitByte.uliData;
            fnCANMsg_Transmit(&stTx1Message, CAN1);	
          }
          break;
          
        case CONFIG_FLUSH_WARNNING_TIME:
          {
#ifdef DBG_CAN
            DEBUG_PRINTF("Msg ID: CONFIG_FLUSH_WARNNING_TIME \r\n");
#endif
            ucEEPROMCounter = CONFIG_PARA_72;
            for(ucLoopCnt = 0; ucLoopCnt < pstCANrxmsg->DLC; ucLoopCnt++)
            {
              SplitByte.rgucData[ucLoopCnt] = pstCANrxmsg->Data[ucLoopCnt];
            }
            stConfigData.fConfigFlushWarnTime = SplitByte.fData;
#ifdef DBG_CAN_EE
            DEBUG_PRINTF("fConfigFlushWarnTime: %d\r\n",
                         stConfigData.fConfigFlushWarnTime);
#endif							
          }
          break;
        }
      }
      break;/*Config Msgs Case ends*/
    }//end of First Switch
  }//end of If case Proccess 
}// end of Proceess CAN



/*****************************************************************************
**@Function 			: fnHBAckFail_Send
**@Descriptions	: Send command to OP to indicate HB ack is not received
**@parameters		: None
**@return				: None
*****************************************************************************/
void fnHBAckFail_Send(void)
{
  CanTxMsg stHBmsg;
  
  /* Reset Data Bufer */
  memset(stHBmsg.Data,0, sizeof(stHBmsg.Data));
  stHBmsg.StdId = 0;
  stHBmsg.ExtId = MSG_TYPE_FAULT | RESERVED |
    FAULT_HB_ACK_NOT_RECEIVED | DEST_OP | SOURCE_MCU ;
  stHBmsg.IDE = CAN_ID_EXT;
  stHBmsg.RTR = CAN_RTR_DATA;
  stHBmsg.DLC = 2;
  
  stHBmsg.Data[0] = (uint8_t)(uiHBACKfail & 0xFF) ;
  stHBmsg.Data[1] =  (uint8_t)((uiHBACKfail >> 8 )& 0xFF);
  
  /*Transmitt CAN message onto CAN bus*/
  fnCANMsg_Transmit(&stHBmsg, CAN1);
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
**@Function 			: fnFifo_Read
**@Descriptions	: Reads msg frame from Tx FIFO
**@parameters		: pointer to structure Fifo
**@return				: msg frame to transmit on can bus
*****************************************************************************/
void fnFifo_Read(stTxMsgFlags *pstFifo, CanTxMsg *pstCurMsgTrans)
{
  pstFifo->bFifoFull = 0;
#ifdef FIFO_DBG
  DEBUG_PRINTF("FIFO RD POINTER = %d\r\n", pstFifo->ucReadCnt);
#endif
  /*When FIFO is empty and read happens, 
  NULL pointer is written and flag is set*/
  if (1 == pstFifo->bFifoEmpty)
  {
    pstFifo->bFiEmpFlt = 1;
#ifdef FIFO_DBG
    DEBUG_PRINTF("FIFO EMPTY FAULT = %d\r\n", pstFifo->bFiEmpFlt);
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
  
#ifdef DBG
  DEBUG_PRINTF("\n CANID = 0x%x\r\n", pstCANTxmsg->ExtId);
#endif
  switch (uliCANId & DEST_ADDR_MASK)
  {
  case DEST_OP:
    {
      if((uiHBACKfail >> OP) == NEGATIVE_ACK)
      {
        /*No Heart Beat ACK is received from OP, so delcaring it to dead*/
        ucDevDead_OP = 1;
#ifdef FIFO_DBG
        DEBUG_PRINTF("IN OP DEAD\n\r");
#endif
        ucMsgTxStat = DEVICE_DEAD;
      }
      else
      {
        if(ucMsgTxCnt_OP)/*Msg frame is being transmitted and*/
        {
          /*non zero value*/
          ucFifoWrStatus = fnFifo_Write(pstCANTxmsg, &pstFifo_OP);
          
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
          
          ucMsgTxCnt_OP++; /*increment msg transmit counter*/
          
          ucTimerEn_OP = 1;	/*Timeout  counter enable set*/
          
          memmove(&pstCurMsgTrans_OP, pstCANTxmsg, sizeof(CanTxMsg));
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
          ucFifoWrStatus = fnFifo_Write(pstCANTxmsg, &pstFifo_OPMM);
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
          
          memmove(&pstCurMsgTrans_OPMM, pstCANTxmsg, sizeof(CanTxMsg));
          ucMsgTxStat = MSG_TRANSMITTING;/*currently data is under TX*/
        }
      }
    }
    break;
  case DEST_MFM:
    {
      if((uiHBACKfail >> MFM) == NEGATIVE_ACK)
      {
        /*No Heart Beat ACK is received from MFM, so delcaring it to dead*/
        ucDevDead_MFM = 1;
        ucMsgTxStat = DEVICE_DEAD;
      }
      else
      {
        if(ucMsgTxCnt_MFM)/*Msg frame is being transmitted and*/
        {  
          /*non zero value*/
          ucFifoWrStatus = fnFifo_Write(pstCANTxmsg, &pstFifo_MFM);
          
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
          ucMsgTxCnt_MFM++; /*increment msg transmit counter*/
          
          ucTimerEn_MFM = 1;	/* Timeout  counter enable set*/
          
          memmove(&pstCurMsgTrans_MFM, pstCANTxmsg, sizeof(CanTxMsg));
          ucMsgTxStat = MSG_TRANSMITTING;/*currently data is under TX*/
        }
      }
    }
    break;
  case DEST_BHCM:
    {
      if((uiHBACKfail >> BHCM) == NEGATIVE_ACK)
      {
        /*No Heart Beat ACK is received from BHCM, so delcaring it to dead*/
        ucDevDead_BHCM = 1;
        ucMsgTxStat = DEVICE_DEAD;
      }
      else
      {
        if(ucMsgTxCnt_BHCM)/*Msg frame is being transmitted and*/
        {  
          /*non zero value*/
          ucFifoWrStatus = fnFifo_Write(pstCANTxmsg, &pstFifo_BHCM);
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
          
          ucMsgTxCnt_BHCM++; /*increment msg transmit counter*/
          
          ucTimerEn_BHCM = 1;	/* Timeout  counter enable set*/
          
          memmove(&pstCurMsgTrans_BHCM, pstCANTxmsg, sizeof(CanTxMsg));
          ucMsgTxStat = MSG_TRANSMITTING;/*currently data is under TX*/
        }
      }
    }
    break;
  case DEST_HHCM:
    {
      if((uiHBACKfail >> HHCM) == NEGATIVE_ACK)
      {
        /* No Heart Beat ACK is received from HHCM, so delcaring it to dead*/
        ucDevDead_HHCM = 1;
        ucMsgTxStat = DEVICE_DEAD;
      }
      else
      {
        if(ucMsgTxCnt_HHCM)/*Msg frame is being transmitted and*/
        {
          /*non zero value*/
          ucFifoWrStatus = fnFifo_Write(pstCANTxmsg, &pstFifo_HHCM);
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
          
          ucMsgTxCnt_HHCM++; /*increment msg transmit counter*/
          
          ucTimerEn_HHCM = 1;	/* Timeout  counter enable set*/
          
          memmove(&pstCurMsgTrans_HHCM, pstCANTxmsg, sizeof(CanTxMsg));
          /*currently data is under TX*/
          ucMsgTxStat = MSG_TRANSMITTING;
        }
      }
    }
    break;
  case DEST_IHCM1:
    {
      if((uiHBACKfail >> IHCM1) == NEGATIVE_ACK)
      {
        /*No Heart Beat ACK is received from IHCM1, so delcaring it to dead*/
        ucDevDead_IHCM1 = 1;
        ucMsgTxStat = DEVICE_DEAD;
      }
      else
      {
        
        if(ucMsgTxCnt_IHCM1)/*Msg frame is being transmitted and*/
        {  
          /*non zero value*/
          ucFifoWrStatus = fnFifo_Write(pstCANTxmsg, &pstFifo_IHCM1);
          
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
          
          ucMsgTxCnt_IHCM1++; /*increment msg transmit counter*/
          
          ucTimerEn_IHCM1 = 1;	/* Timeout  counter enable set*/
          
          memmove(&pstCurMsgTrans_IHCM1, pstCANTxmsg, sizeof(CanTxMsg));
          ucMsgTxStat = MSG_TRANSMITTING;/*currently data is under TX*/
        }
      }
    }
    break;
  case DEST_IHCM4_HM:
    {
      if((uiHBACKfail >> IHCM4_HM) == NEGATIVE_ACK)
      {
        /*No Heart Beat ACK is received from IHCM4_HM, so delcaring it to dead*/
        ucDevDead_IHCM4_HM = 1;
        ucMsgTxStat = DEVICE_DEAD;
      }
      else
      {
        if(ucMsgTxCnt_IHCM4_HM)/*Msg frame is being transmitted and*/
        {   
          /*non zero value*/
          ucFifoWrStatus = fnFifo_Write(pstCANTxmsg, &pstFifo_IHCM4_HM);
          
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
          ucMsgTxCnt_IHCM4_HM++; /*increment msg transmit counter*/
          ucTimerEn_IHCM4_HM = 1;	/* Timeout  counter enable set*/
          
          memmove(&pstCurMsgTrans_IHCM4_HM, pstCANTxmsg, sizeof(CanTxMsg));
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
  case DEST_OP:
    {
      /*msg transmit counter 3, means we have done 3 msg tx*/
      if(3 == ucMsgTxCnt_OP)
      {
        /*Since ACK is not received, we are setting device dead flag*/
        ucDevDead_OP = 1;
      }
      else if (ucMsgTxCnt_OP < 3)
      {
        ucCANTxMsgStatus = CAN_Transmit(CAN1, &pstCurMsgTrans_OP);
        if(CAN_TxStatus_NoMailBox != ucCANTxMsgStatus)
        {
          ucMsgTxCnt_OP++;  
        }
        else
        {
          uliTimeoutCnt_OP = 25; /*Set to 25 for retry this within 10ms*/
#ifdef FIFO_DBG
          DEBUG_PRINTF("\n OP BUFFER NOT EMPTY\r\n");
#endif
        }
        ucTimerEn_OP = 1;/*Enable timer*/
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
          ucCANTxMsgStatus = CAN_Transmit(CAN2, &pstCurMsgTrans_OPMM);
        }
        else if(StatusFlag.stStatusFlag.bCAN1ActiveFlag)
        {
          ucCANTxMsgStatus = CAN_Transmit(CAN1, &pstCurMsgTrans_OPMM);
        }				
        
        if(CAN_TxStatus_NoMailBox != ucCANTxMsgStatus)
        {
          ucMsgTxCnt_OPMM++;		
        }
        else
        {
          uliTimeoutCnt_OPMM = 25; /*Set to 25 for retry this within 10ms*/
#ifdef GENERAL_DBG
          DEBUG_PRINTF("\n OPMM BUFFER NOT EMPTY\r\n");
#endif
        }
        ucTimerEn_OPMM = 1;			
      }
    }
    break;
  case DEST_MFM:
    {
      /*msg transmit counter 3, means we have done 3 msg tx*/
      if(3 == ucMsgTxCnt_MFM)
      {
        /*Since ACK is not received, we are setting device dead flag*/
        ucDevDead_MFM = 1;
      }
      else if (ucMsgTxCnt_MFM < 3)
      {
        ucCANTxMsgStatus = CAN_Transmit(CAN1, &pstCurMsgTrans_MFM);
        if(CAN_TxStatus_NoMailBox != ucCANTxMsgStatus)
        {
          ucMsgTxCnt_MFM++;		
        }
        else
        {
          uliTimeoutCnt_MFM = 25; /*Set to 25 for retry this within 10ms*/
#ifdef GENERAL_DBG
          DEBUG_PRINTF("\n MFM BUFFER NOT EMPTY\r\n");
#endif
        }
        ucTimerEn_MFM = 1;
      }
    }
    break;
  case DEST_BHCM:
    {
      /*msg transmit counter 3, means we have done 3 msg tx*/
      if(3 == ucMsgTxCnt_BHCM)
      {
        /*Since ACK is not received, we are setting device dead flag*/
        ucDevDead_BHCM = 1;
      }
      else if (ucMsgTxCnt_BHCM < 3)
      {
        ucCANTxMsgStatus = CAN_Transmit(CAN1, &pstCurMsgTrans_BHCM);
        if(CAN_TxStatus_NoMailBox != ucCANTxMsgStatus)
        {
          ucMsgTxCnt_BHCM++;	
        }
        else
        {
          uliTimeoutCnt_BHCM = 25; /*Set to 25 for retry this within 10ms*/
#ifdef FIFO_DBG
          DEBUG_PRINTF("\n BHCM BUFFER NOT EMPTY\r\n");
#endif
        }
        ucTimerEn_BHCM = 1;
      }
    }
    break;
  case DEST_HHCM:
    {
      /*msg transmit counter 3, means we have done 3 msg tx*/
      if(3 == ucMsgTxCnt_HHCM)
      {
        /*Since ACK is not received, we are setting device dead flag*/
        ucDevDead_HHCM = 1;	
      }
      else if (ucMsgTxCnt_HHCM < 3)
      {
        ucCANTxMsgStatus = CAN_Transmit(CAN1, &pstCurMsgTrans_HHCM);
        
        if(CAN_TxStatus_NoMailBox != ucCANTxMsgStatus)
        {
          ucMsgTxCnt_HHCM++;	
        }
        else
        {
          uliTimeoutCnt_HHCM = 25; /*Set to 25 for retry this within 10ms*/
#ifdef FIFO_DBG
          DEBUG_PRINTF("\n HHCM BUFFER NOT EMPTY\r\n");
#endif
        }
        ucTimerEn_HHCM = 1;
      }
    }
    break;
  case DEST_IHCM1:
    {
      /*msg transmit counter 3, means we have done 3 msg tx*/
      if(3 == ucMsgTxCnt_IHCM1)
      {
        /*Since ACK is not received, we are setting device dead flag*/
        ucDevDead_IHCM1 = 1;
      }
      else if (ucMsgTxCnt_IHCM1 < 3)
      {             
        ucCANTxMsgStatus = CAN_Transmit(CAN1, &pstCurMsgTrans_IHCM1);
        if(CAN_TxStatus_NoMailBox != ucCANTxMsgStatus)
        {
          ucMsgTxCnt_IHCM1++;		
        }
        else
        {
          uliTimeoutCnt_IHCM1 = 25; /*Set to 25 for retry this within 10ms*/
#ifdef FIFO_DBG
          DEBUG_PRINTF("\n IHCM1 BUFFER NOT EMPTY\r\n");
#endif
        }
        ucTimerEn_IHCM1 = 1;
      }
    }
    break;
    
  case DEST_IHCM4_HM:
    {
      /*msg transmit counter 3, means we have done 3 msg tx*/
      if(3 == ucMsgTxCnt_IHCM4_HM)
      {
        /*Since ACK is not received, we are setting device dead flag*/
        ucDevDead_IHCM4_HM = 1;
      }
      else if (ucMsgTxCnt_IHCM4_HM < 3)
      {			
        ucCANTxMsgStatus = CAN_Transmit(CAN1, &pstCurMsgTrans_IHCM4_HM);
        if(CAN_TxStatus_NoMailBox != ucCANTxMsgStatus)
        {
          ucMsgTxCnt_IHCM4_HM++;
          ucTimerEn_IHCM4_HM = 1;
        }
        else
        {
          ucTimerEn_IHCM4_HM = 1;/*Enable timer*/
          uliTimeoutCnt_IHCM4_HM = 25; /*Set to 25 for retry this within 10ms*/
#ifdef FIFO_DBG
          DEBUG_PRINTF("\n IHCM4 BUFFER NOT EMPTY\r\n");
#endif
        }
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
  
  SplitByte.uliData = pstCANrxmsg->ExtId;
  
  /*Configure CAN Transmitt Message tructure*/
  stTxMsgACK.IDE = CAN_ID_EXT;
  stTxMsgACK.RTR = CAN_RTR_DATA;
  
  /* Reset Data Bufer */
  memset(stTxMsgACK.Data,0, sizeof(stTxMsgACK.Data));
  
  /*Update Length of CAN Data Frame*/
  stTxMsgACK.DLC = 1;
  
  stTxMsgACK.Data[0] = 0x00;		/* Positive ACk */
  
  SplitByte.rgucData[1] = SplitByte.rgucData[0];
  SplitByte.rgucData[0] = SOURCE_MCU;
  SplitByte.rgucData[2] = 0xE8;
  stTxMsgACK.ExtId = SplitByte.uliData;
  
  /*Transmitt CAN message onto CAN bus*/
  if((SET == StatusFlag.stStatusFlag.bCAN2ActiveFlag) &&
     (ID_OPMM == SplitByte.rgucData[1]))
  {
    CAN_Transmit(CAN2, &stTxMsgACK);
#ifdef GENERAL_DBG
    DEBUG_PRINTF("OPMM ACKS ON CAN2 link\r\n");
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
    case SOURCE_OP:
      {
        if(ucDevDead_OP)
        {
#ifdef FIFO_DBG
          DEBUG_PRINTF("\n ACK RECEIVED AFTER 3 MSG TRANSMISSIONS\r\n");
#endif						
        }
        else
        {
          ucTimerEn_OP = 0;	 /*Reset device timeout counter*/
          ucMsgTxCnt_OP = 0; /*Reset msg transmit counter*/
          ucDevDead_OP = 0;  /*Set device dead-alive flag to alive*/
#ifdef FIFO_DBG
          DEBUG_PRINTF("\n ACK RECEIVED SO MSG TRANSMISSION\
                       COUNTER = %x\r\n",ucMsgTxCnt_OP);
#endif
                         if (pstFifo_OP.bFifoEmpty)
                         {  
                           /*If FIFO is empty*/
#ifdef FIFO_DBG
                           DEBUG_PRINTF("\n FIFO IS EMPTY\r\n\n\n");
#endif
                           return (NULL);
                         }
                         else
                         { 
                           /* If FIFO is not empty*/					
                           /*Read one message from fifo into buffer*/
                           fnFifo_Read(&pstFifo_OP, &pstCurMsgTrans_OP);
                           
                           CAN_Transmit(CAN1, &pstCurMsgTrans_OP);
                           
                           ucMsgTxCnt_OP++;
                           ucTimerEn_OP = 1;
#ifdef FIFO_DBG
                           DEBUG_PRINTF("\n FIFO IS NOT EMPTY\r\n");
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
          DEBUG_PRINTF("\n ACK RECEIVED AFTER 3 MSG TRANSMISSIONS\r\n");
#endif						
        }
        else
        {
          ucTimerEn_OPMM = 0;	 /*Reset device timeout counter*/
          ucMsgTxCnt_OPMM = 0; /*Reset msg transmit counter*/
          ucDevDead_OPMM = 0;  /*Set device dead-alive flag to alive*/
#ifdef FIFO_DBG
          DEBUG_PRINTF("\n ACK RECEIVED SO MSG TRANSMISSION COUNTER = %x\r\n",
                       ucMsgTxCnt_OPMM);
#endif
          
          if (pstFifo_OPMM.bFifoEmpty)
          {   
            /*If FIFO is empty*/
#ifdef FIFO_DBG
            DEBUG_PRINTF("\n FIFO IS EMPTY\r\n\n\n");
#endif
            return (NULL);
          }
          else
          {   
            /* If FIFO is not empty*/
            /*Read one message from fifo into buffer*/						
            fnFifo_Read(&pstFifo_OPMM, &pstCurMsgTrans_OPMM);
#ifdef FIFO_DBG
            DEBUG_PRINTF("OPMM FIFO RD DATA = 0x%08x",
                         pstCurMsgTrans_OPMM.ExtId);
#endif
			
            if(SET == StatusFlag.stStatusFlag.bCAN2ActiveFlag)
            {
              CAN_Transmit(CAN2, &pstCurMsgTrans_OPMM);
            }
            else if(SET == StatusFlag.stStatusFlag.bCAN1ActiveFlag)
            {
              CAN_Transmit(CAN1, &pstCurMsgTrans_OPMM);
            }
            ucMsgTxCnt_OPMM++;
            ucTimerEn_OPMM = 1;
#ifdef FIFO_DBG
            DEBUG_PRINTF("\n FIFO IS NOT EMPTY\r\n");
#endif
          }
        }
      }
      break;
    case SOURCE_MFM:
      {
        if(ucDevDead_MFM)
        {
#ifdef FIFO_DBG
          DEBUG_PRINTF("\n ACK RECEIVED AFTER 3 MSG TRANSMISSIONS\r\n");
#endif						
        }
        else
        {
          ucTimerEn_MFM = 0;  /*Reset device timeout counter*/
          ucMsgTxCnt_MFM = 0; /*Reset msg transmit counter*/
          ucDevDead_MFM = 0;  /*Set device dead-alive flag to alive*/
          
          if (pstFifo_MFM.bFifoEmpty)
          {  
            /*If FIFO is empty*/
#ifdef FIFO_DBG
            DEBUG_PRINTF("\n FIFO IS EMPTY\r\n\n\n");
#endif
            return (NULL);
          }
          else
          {   
            /* If FIFO is not empty*/
            /*Read one message from fifo into buffer*/
            fnFifo_Read(&pstFifo_MFM, &pstCurMsgTrans_MFM);
#ifdef FIFO_DBG
            DEBUG_PRINTF("MFM FIFO RD DATA = 0x%08x",
                         pstCurMsgTrans_MFM.ExtId);
#endif
            
            CAN_Transmit(CAN1, &pstCurMsgTrans_MFM);
            
            ucMsgTxCnt_MFM++;
            ucTimerEn_MFM = 1;
          }
        }
      }
      break;
    case SOURCE_BHCM:
      {
        if(ucDevDead_BHCM)
        {
#ifdef FIFO_DBG
          DEBUG_PRINTF("\n ACK RECEIVED AFTER 3 MSG TRANSMISSIONS\r\n");
#endif						
        }
        else
        {
          ucTimerEn_BHCM = 0;	  /*Reset device timeout counter*/
          ucMsgTxCnt_BHCM = 0;  /*Reset msg transmit counter*/
          ucDevDead_BHCM = 0;   /*Set device dead-alive flag to alive*/
#ifdef FIFO_DBG
          DEBUG_PRINTF("\n ACK RECEIVED SO MSG TRANSMISSION\
                       COUNTER = %x\r\n", ucMsgTxCnt_BHCM);
#endif
                         
                         if (pstFifo_BHCM.bFifoEmpty)
                         {
                           /*If FIFO is empty*/
#ifdef FIFO_DBG
                           DEBUG_PRINTF("\n FIFO IS EMPTY\r\n\n\n");
#endif
                           return (NULL);
                         }
                         else
                         {   
                           /* If FIFO is not empty*/													
                           /*Read one message from fifo into buffer*/
                           fnFifo_Read(&pstFifo_BHCM, &pstCurMsgTrans_BHCM);
#ifdef FIFO_DBG
                           DEBUG_PRINTF("BHCM FIFO RD DATA = 0x%08x", 
                                        pstCurMsgTrans_BHCM.ExtId);
#endif
                           
                           CAN_Transmit(CAN1, &pstCurMsgTrans_BHCM);
                           
                           ucMsgTxCnt_BHCM++;
                           ucTimerEn_BHCM = 1;
#ifdef FIFO_DBG
                           DEBUG_PRINTF("\nBHCM FIFO IS NOT EMPTY\r\n");
#endif
                         }
        }		
      }
                       break;
    case SOURCE_IHCM1:
      {
        if(ucDevDead_IHCM1)
        {
#ifdef FIFO_DBG
          DEBUG_PRINTF("\n ACK RECEIVED AFTER 3 MSG TRANSMISSIONS\r\n");
#endif						
        }
        else
        {
          ucTimerEn_IHCM1 = 0;	/*Reset device timeout counter*/
          ucMsgTxCnt_IHCM1 = 0; /*Reset msg transmit counter*/
          ucDevDead_IHCM1 = 0;  /*Set device dead-alive flag to alive*/
          
          
          if (pstFifo_IHCM1.bFifoEmpty)
          {  
            /*If FIFO is empty*/
#ifdef FIFO_DBG
            DEBUG_PRINTF("\n FIFO IS EMPTY\r\n\n\n");
#endif
            return (NULL);
          }
          else
          {   
            /* If FIFO is not empty*/
            /*Read one message from fifo into buffer*/
            fnFifo_Read(&pstFifo_IHCM1, &pstCurMsgTrans_IHCM1);
            
#ifdef FIFO_DBG
            DEBUG_PRINTF("SOURCE_IHCM1 FIFO RD DATA = 0x%08x",
                         pstCurMsgTrans_IHCM1.ExtId);
#endif
            
            CAN_Transmit(CAN1, &pstCurMsgTrans_IHCM1);
            
            ucMsgTxCnt_IHCM1++;
            ucTimerEn_IHCM1 = 1;
#ifdef FIFO_DBG
            DEBUG_PRINTF("\n FIFO IS NOT EMPTY\r\n");
#endif
          }
        }		
      }
      break;
      
    case SOURCE_IHCM4_HM:
      {
        if(ucDevDead_IHCM4_HM)
        {
#ifdef FIFO_DBG
          DEBUG_PRINTF("\n ACK RECEIVED AFTER 3 MSG TRANSMISSIONS\r\n");
#endif						
        }
        else
        {
          ucTimerEn_IHCM4_HM = 0;	 /*Reset device timeout counter*/
          ucMsgTxCnt_IHCM4_HM = 0; /*Reset msg transmit counter*/
          ucDevDead_IHCM4_HM = 0;  /*Set device dead-alive flag to alive*/
          
          if (pstFifo_IHCM4_HM.bFifoEmpty)
          {
            /*If FIFO is empty*/
#ifdef FIFO_DBG
            DEBUG_PRINTF("\n FIFO IS EMPTY\r\n\n\n");
#endif
            return (NULL);
          }
          else
          {   
            /* If FIFO is not empty*/
            /*Read one message from fifo into buffer*/
            fnFifo_Read(&pstFifo_IHCM4_HM, &pstCurMsgTrans_IHCM4_HM);
            
#ifdef FIFO_DBG
            DEBUG_PRINTF("SOURCE_IHCM4HM FIFO RD DATA = 0x%08x", 
                         pstCurMsgTrans_IHCM4_HM.ExtId);
#endif
            
            CAN_Transmit(CAN1, &pstCurMsgTrans_IHCM4_HM);
            
            ucMsgTxCnt_IHCM4_HM++;
            ucTimerEn_IHCM4_HM = 1;
#ifdef FIFO_DBG
            DEBUG_PRINTF("\n FIFO IS NOT EMPTY\r\n");
#endif
          }
        }		
      }
      break;
    case SOURCE_HHCM:
      {
        if(ucDevDead_HHCM)
        {
#ifdef FIFO_DBG
          DEBUG_PRINTF("\n ACK RECEIVED AFTER 3 MSG TRANSMISSIONS\r\n");
#endif
        }
        else
        {
          ucTimerEn_HHCM = 0;	 /*Reset device timeout counter*/
          ucMsgTxCnt_HHCM = 0; /*Reset msg transmit counter*/
          ucDevDead_HHCM = 0;  /*Set device dead-alive flag to alive*/
          
          if (pstFifo_HHCM.bFifoEmpty)
          { 
            /*If FIFO is empty*/		
#ifdef FIFO_DBG
            DEBUG_PRINTF("\n HHCM FIFO IS EMPTY\r\n\n\n");
#endif
            return (NULL);
          }
          else
          {   
            /* If FIFO is not empty*/
            /*Read one message from fifo into buffer*/
            fnFifo_Read(&pstFifo_HHCM, &pstCurMsgTrans_HHCM);
            
            
#ifdef FIFO_DBG
            DEBUG_PRINTF("HHCM FIFO RD DATA = 0x%08x\r\n",
                         pstCurMsgTrans_HHCM.ExtId);
#endif
            
            CAN_Transmit(CAN1, &pstCurMsgTrans_HHCM);
            
            ucMsgTxCnt_HHCM++;
            ucTimerEn_HHCM = 1;
#ifdef FIFO_DBG
            DEBUG_PRINTF("\n HHCM FIFO IS NOT EMPTY\r\n");
#endif
          }
        }		
      }
      break;
    default:
      {
#ifdef FIFO_DBG
        DEBUG_PRINTF("\nAck Recieved from Unknown Device\r\n");
#endif
      }
      break;
    }
  }
                       return 1;
}
                       
                       
/******************************************************************************
**@file     :  Interrupt.c
* 
* @brief    :  This file contains interrupt service routines.
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
uint16_t uiHBTxCounter = 0;
uint8_t rgucRecString[100];
uint8_t ucloopRecStr = 0;
uint8_t	ucI2C1Seq=0;
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

/*******************************************************************************
**@Function 	  : MemManage_Handler
**@Description : This function handles System Hard Fault.
**@parameters  : None
**@Return      : None
*******************************************************************************/
void MemManage_Handler(void)
{
  printf("C");
}
/*******************************************************************************
**@Function 	  : UsageFault_Handler
**@Description : This function handles System Hard Fault.
**@parameters  : None
**@Return      : None
*******************************************************************************/
void UsageFault_Handler(void)
{
  printf("B");
}
/*******************************************************************************
**@Function 	  : HardFault_Handler
**@Description : This function handles System Hard Fault.
**@parameters  : None
**@Return      : None
*******************************************************************************/
void HardFault_Handler(void)
{
  printf("H");
}
/*******************************************************************************
**@Function 	  : SysTick_Handler
**@Description : This function handles SysTick Handler.
**@parameters  : None
**@Return      : None
*******************************************************************************/
void SysTick_Handler(void)
{
  /* Update the LocalTime by adding SYSTEMTICK_PERIOD_MS each SysTick interrupt */
  fnTimeUpdate();
  
}
/*******************************************************************************
**@Function 	  : USART1_IRQHandler
**@Description : This function handles USART1 interrupt request.
**@parameters  : None
**@Return      : None
*******************************************************************************/
void USART1_IRQHandler(void)
{ 
  fnWifiReceivedData();
}

/*******************************************************************************
**@Function 	  : USART6_IRQHandler
**@Description : This function handles USART6 interrupt request.
**@parameters  : None
**@Return      : None
*******************************************************************************/
void USART6_IRQHandler(void)
{
  fnDebugReceivedData();
}
/*******************************************************************************
**@Function 	  : ETH_IRQHandler
**@Description : Handles all the received frames
**@parameters  : None
**@Return      : None
*******************************************************************************/
void ETH_IRQHandler(void)
{
  /* Handles all the received frames */
  if(ETH_CheckFrameReceived()) 
  {		
    fnLwIP_Pkt_Handle();
  }
  /* Clear the Eth DMA Rx IT pending bits */
  ETH_DMAClearITPendingBit(ETH_DMA_IT_R);
  ETH_DMAClearITPendingBit(ETH_DMA_IT_NIS);
}
/*******************************************************************************
**@Function 	  : CAN1_RX0_IRQHandler
**@Description : This function handles CAN1 global interrupt request.
**@parameters  : None
**@Return      : None
*******************************************************************************/
void CAN1_RX0_IRQHandler(void)
{		
  CAN_Receive(CAN1, CAN_FIFO0, &rgstRxCANBuff[uiStoreRxCANBuffCnt++]);
  
  if(uiStoreRxCANBuffCnt >= RX_CAN_BUF_SIZE)
  {
    uiStoreRxCANBuffCnt = 0;
    StatusFlag.stStatusFlag.bDataInCANRx_BuffFlag = SET;
  }
  if(((rgstRxCANBuff[uiStoreRxCANBuffCnt - 1].ExtId) & 
      SOURCE_ADDR_MASK) == SOURCE_OPMM)
  {
    ucCANLinkFaultCnt = 0;
    StatusFlag.stStatusFlag.bCAN1ActiveFlag = SET;
    StatusFlag.stStatusFlag.bCAN2ActiveFlag =RESET;
    StatusFlag.stStatusFlag.bCANFaultFlag = RESET;
  }
#ifdef DBG_CAN
  printf("CAN1 ISR \r\n");
#endif
}

/*******************************************************************************
**@Function 	  : CAN2_RX0_IRQHandler
**@Description : This function handles CAN2 global interrupt request.
**@parameters  : None
**@Return      : None
*******************************************************************************/
void CAN2_RX0_IRQHandler (void)
{
  
  CAN_Receive(CAN2, CAN_FIFO0, &rgstRxCANBuff[uiStoreRxCANBuffCnt++]);
  
  if(uiStoreRxCANBuffCnt >= RX_CAN_BUF_SIZE)
  {
    uiStoreRxCANBuffCnt = 0;
    StatusFlag.stStatusFlag.bDataInCANRx_BuffFlag = SET;
  }	
#ifdef DBG_CAN
  printf("CAN2 ISR \r\n");
#endif
  
  StatusFlag.stStatusFlag.bCAN1ActiveFlag = RESET;
  StatusFlag.stStatusFlag.bCAN2ActiveFlag = SET;
  StatusFlag.stStatusFlag.bCANFaultFlag = RESET;
  ucCANLinkFaultCnt = 0;
  
  
}


/*******************************************************************************
**@Function 	  : TIM2_IRQHandler
**@Description : Timer 2 interrupt handler (1ms Time out)
**@parameters  : None
**@Return      : None
*******************************************************************************/
void TIM2_IRQHandler(void)
{
  /* Clear update interrupt bit*/
  TIM_ClearITPendingBit(TIM2,TIM_FLAG_Update);
  
  //	/* Process the received CAN2 data if available */
  //		fnReciever_Scanner();
  
}

/*******************************************************************************
**@Function 	  : TIM1_UP_IRQHandler
**@Description : Timer 1 interrupt handler (10ms Time out)
**@parameters  : None
**@Return      : None
*******************************************************************************/
void TIM1_UP_TIM10_IRQHandler (void)
{
  
  /* Clear update interrupt bit*/
  TIM_ClearITPendingBit(TIM1, TIM_FLAG_Update);
  
  if((SET == stWifi.ucConnectedStatus) && (RESET == stWifiLog.ucSendFlag))
  {
		/*Wifi Received Hearbeat Check timer*/
		stWifi.uiReceiveTimer++;
  }
  /*Start sending the RTC time to HMI*/ 
  if(SET == StatusFlag.stStatusFlag.bStartPeriodicHB)
  {
    uiHBTxCounter++;
    uiRTCReadCnt++;
  }
  
  /*for Timeout of ack*/
  StatusFlag.stStatusFlag.bFifoAckTimeoutCheckFlag = SET;
  
  if(SCU_OPERATION_MODE == ucSCUMode)
  {
    /*periodic log Read Count(Only when MCU is in Operation mode)*/
    uiPeriodicDataLogCnt++;
    /*Pressure Regulator Read Time*/
    uiPressureRegReadCnt++;
  }	
  /*Counter for Stall command send delay change7*/
  if(SET == StatusFlag.stStatusFlag.bStall_Timer_Flag)
  {
    uiStallcommandsendCnt++;
  }
  
  /*Counter for Diagnostic delay*/
  if(SET == StatusFlag.stStatusFlag.bStartDiagTimer)
  {
    uiDiagnosticTimer++;
  }
  
  if((SET == StatusFlag.stStatusFlag.bCANFaultFlag ))
  {
    uiCANFaultLEDBlinkCnt++;/*counter for LED Blinking On CAN fault*/
  }
  
  if((SET == StatusFlag.stStatusFlag.bDefinedState ))
  {
    uiDefinedStateCnt++;/*counter for LED Blinking On CAN fault*/
  }
  /* Process the received CAN2 data if available */
	stWifi.ucBusyFlag = SET;
  fnReciever_Scanner();
	stWifi.ucBusyFlag = RESET;
  
  /*change27*/
  if((SET == stWarnLog.stWarnLogFlag.bDiagWarnTimerStart))
  {
    /*counter for sending warnig for 80% full memory*/
    stWarningLog.uiWarnLog_DiagTimerCounter++;
  }
  
  if((SET == stWarnLog.stWarnLogFlag.bStatusWarnTimerStart))
  {
    /*counter for sending warnig for 80% full memory*/
    stWarningLog.uiWarnLog_StatusTimerCounter++;
  }
  
  if((SET == stWarnLog.stWarnLogFlag.bFaultWarnTimerStart))
  {
    /*counter for sending warnig for 80% full memory*/
    stWarningLog.uiWarnLog_FaultTimerCounter++;
  }
  
  if((SET == stWarnLog.stWarnLogFlag.bPeriodicWarnTimerStart))
  {
    /*counter for sending warnig for 80% full memory*/
    stWarningLog.uiWarnLog_PeriodicTimerCounter++;
  }
	
	/*change78*/
if(SET ==	StatusFlag.stStatusFlag.bConfigJobName)
{
	uiJobnameCnt++;
}
if(SET ==	StatusFlag.stStatusFlag.bConfigJobID)
{
	uiJobIDCnt++;
}
if(SET ==	StatusFlag.stStatusFlag.bConfigSupervisorName)
{
	uiSupervisornameCnt++;
}
}

/******************************************************************************
**@Function 	  : I2C1_EV_IRQHandler
**@Description : I2C1 Event Handler
**@Parameters  : None
**@Return      : None
****************************************************************************/
void I2C1_EV_IRQHandler(void)
{
  switch(I2C_GetLastEvent(I2C_IO_EXP))
  {
  case I2C_EVENT_MASTER_MODE_SELECT:
    {
      I2C_Send7bitAddress(I2C_IO_EXP, ucIOExpSlaveAddr, I2C_Direction_Transmitter);
    }
    break;
    /*********** Transmitting Data(Channel Configuration)****************/
  case I2C_EVENT_MASTER_BYTE_TRANSMITTING:
  case I2C_EVENT_MASTER_BYTE_TRANSMITTED:
    {
      if(0 == ucI2C1Seq)
      {
        ucI2C1Seq = 1;
        I2C_SendData(I2C_IO_EXP, (uiI2CWriteData & 0xFF));
      }
      else if(1 == ucI2C1Seq)
      {
        I2C_SendData(I2C_IO_EXP, ((uiI2CWriteData >> 8) & 0xFF));
        ucI2C1Seq = 2;
      }
      else
      {
        /* Send I2Cx STOP Condition after last byte has been transmitted */
        I2C_GenerateSTOP(I2C_IO_EXP, ENABLE);
        ucI2C1Seq = 0;
        StatusFlag.stStatusFlag.bI2CBusyFlag = RESET;
      }
    }
    break;
    
  default:
    break;
  }
#ifdef GENERAL_DBG
  DEBUG_PRINTF("I2C1 IRQ\r\n");
#endif
}


/******************************************************************************
**@Function 	  : I2C1_ER_IRQHandler
**@Description : I2C1 Error Handler for Expander
**@Parameters  : None
**@Return      : None
****************************************************************************/
void I2C1_ER_IRQHandler(void)
{
  if((I2C_ReadRegister(I2C_IO_EXP, I2C_Register_SR1) & 0xFF00) != 0x00)
  {
    I2C_IO_EXP->SR1 &= 0x00FF; 
    I2C_AcknowledgeConfig(I2C_IO_EXP, DISABLE);
    I2C_GenerateSTOP(I2C_IO_EXP, ENABLE); 
    StatusFlag.stStatusFlag.bI2CBusyFlag = RESET;
    ucI2C1Seq = 0;
    uiI2CWriteData = 0;
#ifdef GENERAL_DBG
    DEBUG_PRINTF("I2C1 Error!!\r\n");
#endif
  }
}
/******************************************************************************
**@Function 	  : EXTI0_IRQHandler
**@Description : Eternal interrupt for Start Signal
**@Parameters  : None
**@Return      : None
****************************************************************************/
void EXTI0_IRQHandler(void)
{
  /* Make sure that interrupt flag is set */
  if (EXTI_GetITStatus(EXTI_Line0) != RESET) 
  {
    /* Do your stuff when PI0 is changed */
    StatusFlag.stStatusFlag.bStartSignal = SET;
    /* Clear interrupt flag */
    EXTI_ClearITPendingBit(EXTI_Line0);
  }
  
}
/******************************************************************************
**@Function 	  : EXTI4_IRQHandler
**@Description : Eternal interrupt for ESTOP Signal
**@Parameters  : None
**@Return      : None
****************************************************************************/
void EXTI4_IRQHandler(void)
{
  /* Make sure that interrupt flag is set */
  if (EXTI_GetITStatus(EXTI_Line4) != RESET) 
  {
    /* Do your stuff when PI0 is changed */
    if(RESET == GPIO_ReadInputDataBit(ESTOP_SIGNAL_SELECT_PORT, ESTOP_SIGNAL_SELECT_PIN))
    {
      StatusFlag.stStatusFlag.bEStop_Flag = SET;
      ClearBit(uiIOExpData,ESTOP_GREEN_LED);
      SetBit(uiIOExpData,ESTOP_RED_LED);
#ifdef DBG_RECIEVER
      printf("ESTOP RECIEVED\r\n");
#endif
      if( SET == StatusFlag.stStatusFlag.bStartSignal)
      {
        StatusFlag.stStatusFlag.bStartupFault =SET;
      }
    }
    else
    {
      StatusFlag.stStatusFlag.bEStop_Flag = RESET;
      StatusFlag.stStatusFlag.bEStop_Sent_Flag = RESET;
      ClearBit(uiIOExpData,ESTOP_RED_LED);
      SetBit(uiIOExpData,ESTOP_GREEN_LED);
      if( SET == StatusFlag.stStatusFlag.bStartupFault)
      {
				/*change56*/
				StatusFlag.stStatusFlag.bSendEtopClear =SET;
        //fnAllERROR_Clear(SOURCE_HMI);
//        stTx1Message.DLC = 0;
//        stTx1Message.IDE = CAN_ID_EXT;
//        stTx1Message.RTR = CAN_RTR_DATA;
//        stTx1Message.ExtId = 	MSG_TYPE_COMMAND | CMD_CLEAR_ERROR |
//          DEST_HMI | SOURCE_SCU;
//        fnCANMsg_Transmit(&stTx1Message, CAN1);
			}
#ifdef DBG_RECIEVER
      printf("ESTOP RELEASED \r\n");
#endif
    }
    /* Clear interrupt flag */
    EXTI_ClearITPendingBit(EXTI_Line4);
  }
}

/*******************************************************************************
* Function Name  : CAN1_SCE_IRQHandler
* Description    : This function handles CAN SCE interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CAN1_SCE_IRQHandler(void)
{

  StatusFlag.stStatusFlag.bCANFaultFlag = SET;	
  StatusFlag.stStatusFlag.bErrorFlag = SET;
  StatusFlag.stStatusFlag.bErrorSentFlag = SET;
  if ( CAN_GetITStatus(CAN1,CAN_IT_EPV) == SET )
  {
    CAN_ClearITPendingBit(CAN1,CAN_IT_EPV);
#ifdef DBG
		  static uint16_t testcount=0;
    	printf("\n\rCAN ERROR=%d\r\n",testcount);
#endif
  }
  if ( CAN_GetITStatus(CAN1,CAN_IT_BOF) == SET )
  {
    CAN_ClearITPendingBit(CAN1,CAN_IT_BOF);
#ifdef DBG
	printf("\n\rCAN BUS OFF\r\n");
#endif
  }
}

///*******************************************************************************
//* Function Name  : CAN1_SCE_IRQHandler
//* Description    : This function handles CAN SCE interrupt request.
//* Input          : None
//* Output         : None
//* Return         : None
//*******************************************************************************/
void CAN2_SCE_IRQHandler(void)
{
  StatusFlag.stStatusFlag.bCANFaultFlag = SET;	
  StatusFlag.stStatusFlag.bErrorFlag = SET;
  StatusFlag.stStatusFlag.bErrorSentFlag = SET;
  if ( CAN_GetITStatus(CAN2,CAN_IT_EPV) == SET )
  {
    CAN_ClearITPendingBit(CAN2,CAN_IT_EPV);
#ifdef DBG
		static uint16_t testcount=0;
    	printf("\n\rCAN ERROR=%d\r\n",testcount);
#endif
  }
  if ( CAN_GetITStatus(CAN2,CAN_IT_BOF) == SET )
  {
    CAN_ClearITPendingBit(CAN2,CAN_IT_BOF);
#ifdef DBG
	printf("\n\rCAN BUS OFF\r\n");
#endif
  }
}


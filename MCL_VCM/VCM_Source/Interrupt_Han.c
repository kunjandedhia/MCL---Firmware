/******************************************************************************
  * @file    Interrupt.c
  * 
  * @brief   This file contains the functons to handle interrupts
  ****************************************************************************
  */
/* ----------------------------------------------------------------------------
 *                           Includes
 * ---------------------------------------------------------------------------
 */
 #include "Includes.h"
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
	uint8_t ucLEDBlinkCount 					= 0;		/*variable for period of blinking LED*/
	uint16_t uiTxCounter 							= 0;		/*CAN msg Resend counter*/
	uint16_t uiHBTimeCounter 					= 0;	
	uint16_t uiStallCounter 					= 0;
	uint16_t uiTBeforeStallPresCompr 	= 0;
	uint16_t uiPressureReadCounter 		= 0;	
	uint16_t uiPumpCount 							= 0;
	uint16_t uiLTCheckCounter 				= 0;
	uint16_t uiMaterialUsageCnt 			= 0;
	uint16_t uiLinearTransCnt 				= 0;
/* ----------------------------------------------------------------------------
 *                           EXTERNAL VARIABLES
 * ---------------------------------------------------------------------------
 */
 
 /* ----------------------------------------------------------------------------
 *                           EXTERNAL FUNCTIONS
 * ---------------------------------------------------------------------------
 */

/* ----------------------------------------------------------------------------
 *                           FUNCTIONS DECLARITION
 * ---------------------------------------------------------------------------
 */
void TIM1_UP_IRQHandler (void);
/******************************************************************************
 * @Function 			: Timer1IntrHandler
 * @Description		: Timer 1 interrupt handler
 * @Parameters		: None
 * @Return				: None
 ****************************************************************************/
void TIM1_UP_IRQHandler (void)
{
	if(SET == StatusFlag.stStatusFlag.bFirstHrtBtMsgFlag)
	{
		if((SET == StatusFlag.stStatusFlag.bCANFaultSCU) || 
			 (SET == StatusFlag.stStatusFlag.bCANFaultHMI))
		{
			/*For blinking LED  to state CAN fault*/
			ucLEDBlinkCount++;
		}
		
		uiAmbiantReadCnt++;

		if(SET == StatusFlag.stStatusFlag.bTransmittingCAN )
		{
			/*CAN message resend time counter*/
			uiTxCounter++;
		}
		if(SET == StatusFlag.stStatusFlag.bReadValue1)
		{
			/*counter to cheke readings for Upstroke and downstroke*/
			uiStallCounter++;
		}
		
		if(SET == stStallFlag.bTBeforeStall )
		{
			/*counter to set the time before the stall*/
			uiTBeforeStallCnt++;
		}
		if(SET == stStallFlag.bTBeforeStallPresCompr)
		{
			/*counter to check the time before stall pressure compare*/
			uiTBeforeStallPresCompr++;
		}
		/*counter to check the heaet beat received time*/
		uiHBTimeCounter++;
		/*counter to read the pressure gauge*/
		uiPressureReadCounter++;
		/*counter to read the valve status*/
		if(SET == StatusFlag.stStatusFlag.bValveReadFlag)
		{
			uiValveReadCounter++;
		}
		if(SET == StatusFlag.stStatusFlag.bStartMonitorLT)
		{
			uiMaterialUsageCnt++;
		}
		/*Linear transducer range Check time counter*/
		uiLTCheckCounter++;
		uiLinearTransCnt++;
	}
	else
	{
		ucPOSTLEDBlinkCnt++;
	}
	 /* Clear update interrupt bit*/
  TIM_ClearITPendingBit(TIM1,TIM_FLAG_Update);
}
/******************************************************************************
 **@Function Name  : USB_LP_CAN1_RX0_IRQHandler
 **@Description    : This function handles CAN1 global interrupt request.
 **@parameters     : None
 **@Return         : None
 ****************************************************************************/
void USB_LP_CAN1_RX0_IRQHandler(void)
{
	CAN_Receive(CAN1, CAN_FIFO0, &rgstRxCANBuff[ucStoreRxCANBuffCnt++]);
	if(ucStoreRxCANBuffCnt >= RX_CAN_BUF_SIZE)
	{
		ucStoreRxCANBuffCnt = 0;
		StatusFlag.stStatusFlag.bDataInCANRxBuffFlag =  SET;
	}
	#ifdef DBG_CAN    	
		DEBUG_PRINTF("CAN1 ISR \r\n");
	#endif	
}
/******************************************************************************  
 **@Function 			: EXTI9-5_IRQHandler  
 **@Description		: EXTI Interrupt Handler to measure the pump count 
 **@Parameters		: None  
 **@Return				: None  
 ****************************************************************************/ 
void EXTI9_5_IRQHandler(void)
{
	
	#ifdef GENERAL_DBG  
		char rgcTemp[60];
	#endif
	
	if(RESET != EXTI_GetITStatus(LT1_EXTI_LINE_NO)) 			
  {
		if(RESET == StatusFlag.stStatusFlag.bExtInt)  //change2
		{	
			StatusFlag.stStatusFlag.bExtInt	= SET;		//change2
		//	DEBUG_PRINTF("Pump count:%d \r\n",uiPumpCount);
			if(SET == StatusFlag.stStatusFlag.bStartMonitorLT)
			{
				uiPumpCount++;
				#ifdef GENERAL_DBG    	
					DEBUG_PRINTF("Pump count 9:%d \r\n",uiPumpCount);
				#endif
			}
		/*change3*/
		//if(RESET == StatusFlag.stStatusFlag.bLTFindLimit)
//			if(SET == StatusFlag.stStatusFlag.bLTFindLimit)
//		{
//			uiPumpCount++;
//		
//			if(11 < uiPumpCount)
//			{
//				uiPumpCount = 0;
//				StatusFlag.stStatusFlag.bLTFindLimit = SET;
//				#ifdef GENERAL_DBG  
//					sprintf(rgcTemp,"fLinearTransMin  = %0.2f\r\n", fLinearTransMin);	
//					DEBUG_PRINTF("%s",rgcTemp );
//				#endif
//				#ifdef GENERAL_DBG  
//					sprintf(rgcTemp,"fLinearTransMax  = %0.2f\r\n", fLinearTransMax);	
//					DEBUG_PRINTF("%s",rgcTemp );
//				#endif
//			}
//		}
	}
  EXTI_ClearITPendingBit(LT1_EXTI_LINE_NO); /*change2*/
  }
}

/******************************************************************************  
 **@Function 			: EXTI15-10_IRQHandler  
 **@Description		: EXTI Interrupt Handler to measure the pump count 
 **@Parameters		: None  
 **@Return				: None  
 ****************************************************************************/ 
void EXTI15_10_IRQHandler(void)
{
	#ifdef GENERAL_DBG  
		char rgcTemp[60];
	#endif
	
	if(RESET != EXTI_GetITStatus(LT2_EXTI_LINE_NO)) 			
  {
		if(RESET == StatusFlag.stStatusFlag.bExtInt)  //change2
		{	
		StatusFlag.stStatusFlag.bExtInt	= SET;		//change2
		if(SET == StatusFlag.stStatusFlag.bStartMonitorLT)
		{
			uiPumpCount++;
			#ifdef GENERAL_DBG    	
				DEBUG_PRINTF("Pump count 10:%d \r\n",uiPumpCount);
			#endif
		}
		/*change3*/
		//if(RESET == StatusFlag.stStatusFlag.bLTFindLimit)
//			if(SET == StatusFlag.stStatusFlag.bLTFindLimit)
//		{
//			uiPumpCount++;
//		//DEBUG_PRINTF("Pump countEXTI15_10_IRQHandler :%d \r\n",uiPumpCount);
//			if(11 < uiPumpCount)
//			{
//				uiPumpCount = 0;
//				StatusFlag.stStatusFlag.bLTFindLimit = SET;
//				#ifdef GENERAL_DBG  
//					sprintf(rgcTemp,"fLinearTransMin  = %0.2f\r\n", fLinearTransMin);	
//					DEBUG_PRINTF("%s",rgcTemp );
//				#endif
//				#ifdef GENERAL_DBG  
//					sprintf(rgcTemp,"fLinearTransMax  = %0.2f\r\n", fLinearTransMax);	
//					DEBUG_PRINTF("%s",rgcTemp );
//				#endif
//			}
		}
		
    EXTI_ClearITPendingBit(LT2_EXTI_LINE_NO);
  }
}
/******************************************************************************
 **@Function 	  : I2C2_EV_IRQHandler
 **@Description : I2C2 Event Handler
 **@Parameters  : None
 **@Return      : None
 ****************************************************************************/
void I2C2_EV_IRQHandler(void)
{
	switch(I2C_GetLastEvent(I2C_EE07))
	{
		case I2C_EVENT_MASTER_MODE_SELECT:
		{
			I2C_Send7bitAddress(I2C_EE07, ucEE07SlaveAddr, ucI2CDirection);
		}
		break;
		/*********** Transmitting Data(Channel Configuration)****************/
		case I2C_EVENT_MASTER_BYTE_TRANSMITTING:
		{
			I2C_SendData(I2C_EE07, ucMuxChannelNum);
			/* Send I2Cx STOP Condition after last byte has been transmitted */
			I2C_GenerateSTOP(I2C_EE07, ENABLE);
		}
		break;
		case I2C_EVENT_MASTER_BYTE_TRANSMITTED:
		{
		}
		break;
		/****************Recieving Part*********************/
		case I2C_EVENT_SLAVE_RECEIVER_ADDRESS_MATCHED:
		{
		}
		break;
		case I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED:
		{
			/* Enable I2C1 acknowledgement */
			I2C_AcknowledgeConfig(I2C_EE07, ENABLE);
		}
		break;
		case I2C_EVENT_MASTER_BYTE_RECEIVED:
		{
			/* Store I2C1 received data */
			if(ucI2CRxCnt == 0)
			{
				rgucI2CRxBuff[ucI2CRxCnt++] = I2C_ReceiveData (I2C_EE07);
				I2C_AcknowledgeConfig(I2C_EE07, DISABLE);	
				I2C_GenerateSTOP(I2C_EE07, ENABLE);
			}
			else if(ucI2CRxCnt == 1)
			{
				rgucI2CRxBuff[ucI2CRxCnt++] = I2C_ReceiveData (I2C_EE07);				
				
				/*flag set for data Recieved indiacation*/
				StatusFlag.stStatusFlag.bI2CReadDone = SET;
			}
		}
	  break;
		default:
			break;
	}
	#ifdef GENERAL_DBG
		DEBUG_PRINTF("I2C2 IRQ\r\n");
	#endif
}
/******************************************************************************
 **@Function 	  : I2C2_ER_IRQHandler
 **@Description : I2C2 Error Handler for inside EE07 sensor
 **@Parameters  : None
 **@Return      : None
 ****************************************************************************/
void I2C2_ER_IRQHandler(void)
{
	if((I2C_ReadRegister(I2C_EE07, I2C_Register_SR1) & 0xFF00) != 0x00)
	{
		
		I2C_AcknowledgeConfig(I2C_EE07, DISABLE);
		I2C_GenerateSTOP(I2C_EE07, ENABLE); 
		uiAmbiantReadCnt = 0;
		ucI2CRxCnt = 0;
		if(CHANNEL_IN >= ucI2CSequence)
		{
			ucI2CSequence = CHANNEL_IN;
			/*this indicates that reading of OutSide Ambiant Humidity and Temp Not Correct*/
			ucOutsideStatus = 0x03; 
		}
		else if(CHANNEL_IN < ucI2CSequence) 				
		{
			ucI2CSequence = CHANNEL_OUT;
			/*this indicates that reading of inside Ambiant Temp Not Correct*/
			ucInsideStatus = 0x01;
			StatusFlag.stStatusFlag.bAmbiantReadDone = SET;
		}
		StatusFlag.stStatusFlag.bI2CReadDone = RESET;
		I2C_EE07->SR1 &= 0x00FF; 
		#ifdef GENERAL_DBG
			DEBUG_PRINTF("I2C2 Error!!\r\n");
		#endif
	}
}
/******************************************************************************
**@Function		  :	USART2_IRQHandler
**@Description	: USART2 Interrupt hander 
**@parameter  	: none
**@retval       : none
*****************************************************************************/
void UART5_IRQHandler(void)
{
	uint8_t ucData;
	if(SET == USART_GetFlagStatus(UPS_UART, USART_FLAG_RXNE))
	{
		ucData = USART_ReceiveData(UPS_UART);	
//		USART_SendData(UPS_UART, ucData);
//		while(!(USART_GetFlagStatus(UPS_UART, USART_FLAG_TXE)));
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
			I2C_Send7bitAddress(I2C_IO_EXP, ucIOExpSlaveAddr,
													I2C_Direction_Transmitter);
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
		
		I2C_AcknowledgeConfig(I2C_IO_EXP, DISABLE);
		I2C_GenerateSTOP(I2C_IO_EXP, ENABLE); 
		StatusFlag.stStatusFlag.bI2CBusyFlag = RESET;
		ucI2C1Seq = 0;
		uiI2CWriteData = 0;
		I2C_IO_EXP->SR1 &= 0x00FF; 
		#ifdef GENERAL_DBG
			DEBUG_PRINTF("I2C1 Error!!\r\n");
		#endif
	}
}

void HardFault_Handler(void)
{
	DEBUG_PRINTF("H");
}

void BusFault_Handler(void)
{
	DEBUG_PRINTF("BusFault_Handler\r\n");
}

void MemManage_Handler(void)
{
	DEBUG_PRINTF("MemManage_Handler\r\n");
}


void UsageFault_Handler(void)
{
	DEBUG_PRINTF("UsageFault_Handler\r\n");
}



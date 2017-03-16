#include "System_Header.h"


uint32_t uliFlashTimeCounter = 0;
uint8_t Direction = I2C_Direction_Transmitter;

uint32_t uliFlushWarningCnt = 0;
uint32_t uliHmScreenRefreshCnt = 0;

uint16_t uiPostLedBlinkCnt = 0;
/*****************************************************************************
**@Function		 	:  TIM1_UP_IRQHandler
**@Descriptions	:	 Interrupt Service routine for Timer1
**@parameters		:  none
**@return				:  none
*****************************************************************************/
void TIM1_UP_IRQHandler(void)
{
	// Clear the TIM1 Update pending bit
	TIM_ClearITPendingBit(TIM1, TIM_IT_Update);

	if ( SET == StatusFlag.stStatusFlag.bTxFlag )
	{
		uiTxCounter++;
	}
	if ( SET == StatusFlag.stStatusFlag.bCANFaultFlag )
	{
		ucLEDBlinkCount++;
	}
	if ( SET == StatusFlag.stStatusFlag.bAnyFault )
	{
		uiAlarm1LEDBlinkCount++;
	}
	
	if( TRUE == stDiagStatus.bSCU_DiagStatusReceived)
	{
			/*counter to check the heaet beat received time*/
			if (TRUE == StatusFlag.stStatusFlag.bHeartBeatMsg )
			{
				uiHBTimeCounter++;
			}

			fnSw_Foreground_Service();

			uliFlashTimeCounter++;
			
			/*if ( SET == StatusFlag.stStatusFlag.bCANFaultFlag )
			{
				ucLEDBlinkCount++;
			}
			if ( SET == StatusFlag.stStatusFlag.bAnyFault )
			{
				uiAlarm1LEDBlinkCount++;
			}
			if ( SET == StatusFlag.stStatusFlag.bTxFlag )
			{
				uiTxCounter++;
			}
			*/
				uiWDTCount++;

			if ( TRUE == StatusFlag.stStatusFlag.bFlushWarningFlag )
			{
				uliFlushWarningCnt++;
			}
			
			uliHmScreenRefreshCnt++;
			if(uliHmScreenRefreshCnt == 4999)
			{
				ucSwitchPressFlag = TRUE;
			}
			
			/* Blink the Red Led if HeartBeat Not Received */
			fnBoardStatusRedLed_Blink();

			/* Blink the Alarm 1 Red Led if any fault Received */
			fnAlarm1StatusRedLed_Blink();
	}
	else
	{
		uiPostLedBlinkCnt++;
	}
	
}
/*****************************************************************************
**@Function		 	:  TIM2_IRQHandler
**@Descriptions	:	 Interrupt Service routinr for Timer2
**@parameters		:  none
**@return				:  none
*****************************************************************************/
void TIM2_IRQHandler(void)
{
	// Clear the TIM1 Update pending bit
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	if (( uiCAN1_processCnt < uiStoreCAN1_BuffCnt ) || ( StatusFlag.stStatusFlag.bDataInCAN1_BuffFlag == TRUE ))
	{
		#ifdef INTERRUPT_DBG   	
			DEBUG_PRINTF("TIMER2ISR \r\n"); 
		#endif	
		fnValidateCANMsg(&stCANRx1_Rcvbuff[uiCAN1_processCnt++]);
		
		if ( CAN1_BUF_SIZE <= uiCAN1_processCnt )
		{
			uiCAN1_processCnt = 0;
			StatusFlag.stStatusFlag.bDataInCAN1_BuffFlag = FALSE;//FLAG_RESET;
		}
	}
}
/*****************************************************************************
**@Function		 	:  USART1_IRQHandler
**@Descriptions	:	 USART Interrupt Service Routine
**@parameters		:  none
**@return				:  none
*****************************************************************************/
/*void USART1_IRQHandler(void)
{
	if ( USART_GetFlagStatus(USART2,USART_FLAG_RXNE) == SET )
	{

	}
}
*/


/*****************************************************************************
**@Function		 	:  USB_LP_CAN1_RX0_IRQHandler
**@Descriptions	:	 CAN1 Recieve Interrupt Service Routine
**@parameters		:  none
**@return				:  none
*****************************************************************************/
void USB_LP_CAN1_RX0_IRQHandler(void)
{
		
    
	CAN_Receive(CAN1, CAN_FIFO0, &stCANRx1_Rcvbuff[uiStoreCAN1_BuffCnt++]);
	if ( CAN1_BUF_SIZE <= uiStoreCAN1_BuffCnt )
	{ 
		uiStoreCAN1_BuffCnt = 0;
		StatusFlag.stStatusFlag.bDataInCAN1_BuffFlag =  TRUE;
	}
	
	#ifdef INTERRUPT_DBG  	
		DEBUG_PRINTF("CAN1 ISR :%x\r\n",stCANRx1_Rcvbuff[uiStoreCAN1_BuffCnt - 1].ExtId);
	#endif
	
	/*
	if ( ACKNOWLEDGEMENT != (stCANRx1_Rcvbuff[uiStoreCAN1_BuffCnt - 1].ExtId & ACKNOWLEDGEMENT) )
	{
		fnSendACK(&stCANRx1_Rcvbuff[uiStoreCAN1_BuffCnt - 1]); 
	}
	*/
}

/*****************************************************************************
**@Function		 	:  I2C1_EV_IRQHandler
**@Descriptions	:	 I2C1 event interrupt service routine
**@parameters		:  none
**@return				:  none
*****************************************************************************/
void I2C1_EV_IRQHandler(void)
{
	switch(I2C_GetLastEvent(I2C1))
	{
		case I2C_EVENT_MASTER_MODE_SELECT:
		{
			if( TRANSMITTER == Direction )
      {
        /* Master Transmitter ----------------------------------------------*/
        /* Send slave Address for write */
        I2C_Send7bitAddress(I2C1, (IOEXP_1_SLAVE_ADDR & I2C_SLAVE_WRITE), I2C_Direction_Transmitter);
      }
      else
      {
        /* Master Receiver -------------------------------------------------*/
        /* Send slave Address for read */
        I2C_Send7bitAddress(I2C1, (IOEXP_1_SLAVE_ADDR | I2C_SLAVE_READ), I2C_Direction_Receiver);
      }

		}break;
		
		case I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED:
		{
			I2C_SendData(I2C1, ucIOExpader1Low);
			
		}break;
		
		case I2C_EVENT_MASTER_BYTE_TRANSMITTING:
		{
			/*
			if ( FALSE == StatusFlag.stStatusFlag.ucI2CInterruptFlag )
			{
				StatusFlag.stStatusFlag.ucI2CInterruptFlag = TRUE;
				I2C_SendData(I2C1, ucIOExpader1High);
			}
			else
			{
				StatusFlag.stStatusFlag.ucI2CInterruptFlag = FALSE;
				I2C_TransmitPEC(I2C1, ENABLE);
        I2C_ITConfig(I2C1, I2C_IT_BUF, DISABLE);
			}
			*/
		}break;
		
		case I2C_EVENT_MASTER_BYTE_TRANSMITTED:
		{
			if ( FALSE == StatusFlag.stStatusFlag.ucI2CInterruptFlag )
			{
				StatusFlag.stStatusFlag.ucI2CInterruptFlag = TRUE;
				I2C_SendData(I2C1, ucIOExpader1High);
			}
			else
			{
				StatusFlag.stStatusFlag.ucI2CInterruptFlag = FALSE;
				I2C_TransmitPEC(I2C1, ENABLE);
        I2C_ITConfig(I2C1, I2C_IT_BUF, DISABLE);
				//I2C_ITConfig(I2C1, I2C_IT_BUF, ENABLE);
				
				/* I2C1 Re-START Condition */
				I2C_GenerateSTOP(I2C1, ENABLE);
				//Direction = I2C_Direction_Receiver;

				fnDelayMsec(1);
				//I2C_Cmd(I2C1, DISABLE);
				// enable FSMC
				RCC->AHBENR |= RCC_AHBENR_FSMCEN;
			}
			
		}break;
		
	}	
}

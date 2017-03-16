/******************************************************************************
**@file   Interrupt_Han.c 
* 
**@brief  This file contains all the Interrupt handler
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
/******************************************************************************
 **@Function		 :	TIM1_UP_IRQHandler
 **@Description  :	Timer1 interrupt  handler 
 **@Parameter 	 :	None
 **@Return       :	None
 *****************************************************************************/
void TIM1_UP_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM1,TIM_FLAG_Update))
	{
		/* Clear update interrupt bit */
		TIM_ClearITPendingBit(TIM1, TIM_FLAG_Update);
		if(SET == StatusFlag.stStatusFlag.bFirstHrtBtMsgFlag)
		{			
			uiTempReadCount++;
			uiTimCountLL++;
			uiTempReadCnt++;
			
			if(SET == StatusFlag.stStatusFlag.bFirstHrtBtMsgFlag)
			{
				uiHBTimeCounter++;
			}
			if(SET == StatusFlag.stStatusFlag.bTxFlag) 
			{
				uiTxCounter++;
			}	
		}
		else
		{
			ucPostLedBlinkCnt++;
		}
	}
}
#ifdef GENERAL_DBG
/*******************************************************************************
 **@Function		:	USART1_IRQHandler
 **@Description	: USART1 Interrupt hander 
 **@Parameter  	: None
 **@Return      : None
 *****************************************************************************/
void USART1_IRQHandler(void)
{
	#ifdef TEST_DBG
	uint8_t ucData;
	#endif
	if(SET == USART_GetFlagStatus(USART1, USART_FLAG_RXNE))
	{
		#ifdef TEST_DBG
			ucData = USART_ReceiveData(USART1);
			rgucRecString[ucloopC++] = ucData;
			if('$' == ucData)
			{
				StatusFlag.stStatusFlag.bUartFlag = SET;
				rgucRecString[ucloopC] = 0;
			}
		#endif
	}
}	
#endif	
/******************************************************************************
 **@Function 		: USB_LP_CAN1_RX_IRQHandler
 **@Description	:	CAN 1 received interrupt handler 
 **@Parameters	: None
 **@Return			: None
 ****************************************************************************/
void USB_LP_CAN1_RX0_IRQHandler (void) 
{	
	CAN_Receive(CAN1, CAN_FIFO0, &rgstRxCANBuff[ucStoreRxCANBuffCnt++]);
	
	if(RX_CAN_BUF_SIZE <= ucStoreRxCANBuffCnt)
	{
		ucStoreRxCANBuffCnt = 0;
		StatusFlag.stStatusFlag.bDataInCANRxBuffFlag =  SET;
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


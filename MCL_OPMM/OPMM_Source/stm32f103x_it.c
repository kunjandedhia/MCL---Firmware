/******************************************************************************
**@file    Interrupt.c
* 
**@brief   This file contains the Interrupt handlers
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
uint8_t ucLSW1_2BouncingCnt = 0;
uint8_t ucLSW1_2DeBouncingCnt = 0;
uint8_t ucLSW3_4BouncingCnt = 0;
uint8_t ucLSW3_4DeBouncingCnt = 0;
uint8_t ucLSWReadVar = 0;
uint8_t ucLSW1_2ReadBackup = 0;
uint8_t ucLSW3_4ReadBackup = 0;
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
 **@Function    : TIM2_IRQHandler
 **@Description	: Timer 2 interrupt handler for 2 msec for LMSW scanning 
 **@Parameters	: None
 **@Return			: None
 ****************************************************************************/
void TIM2_IRQHandler(void)
{
//static uint8_t ucTest = 0;
//	
//	if((ucTest % 2) == 0)
//	{
//		GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_SET);
//	}
//	else
//	{
//		GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_RESET);
//	}
//		ucTest++;
	
	ucLSWReadVar = ((uint8_t)GPIO_ReadInputData(LIMIT_SW_PORT) & 
									 LIMIT_SW_BITS);
	
	if((RESET == StatusFlag.stStatusFlag.bLSW1_2PressFlag) && 
			(LIMIT_SW_BITS1_2 != (ucLSWReadVar & LIMIT_SW_BITS1_2)))
	{
		ucLSW1_2BouncingCnt = 1;
		ucLSW1_2DeBouncingCnt = 0;
		StatusFlag.stStatusFlag.bLSW1_2PressFlag = SET;
		ucLSW1_2ReadBackup = ucLSWReadVar;
	}
	else if((SET == StatusFlag.stStatusFlag.bLSW1_2PressFlag) && 
					(0 == ucLSW1_2DeBouncingCnt) && 
					(LIMIT_SW_BITS1_2 == (ucLSWReadVar & LIMIT_SW_BITS1_2)))
	{
		ucLSW1_2DeBouncingCnt = 1;
	}
 
	if((RESET == StatusFlag.stStatusFlag.bLSW3_4PressFlag) &&	
			(LIMIT_SW_BITS3_4 != (ucLSWReadVar & LIMIT_SW_BITS3_4)))
	{
		ucLSW3_4BouncingCnt = 1;
		ucLSW3_4DeBouncingCnt = 0;
		StatusFlag.stStatusFlag.bLSW3_4PressFlag = SET;
		ucLSW3_4ReadBackup = ucLSWReadVar;
	}
	else if((SET == StatusFlag.stStatusFlag.bLSW3_4PressFlag) && 
					(0 == ucLSW3_4DeBouncingCnt) && 
					(LIMIT_SW_BITS3_4 == (ucLSWReadVar & LIMIT_SW_BITS3_4)))
	{
		ucLSW3_4DeBouncingCnt = 1;
	}
	
	
	
	if((SET == StatusFlag.stStatusFlag.bLSW1_2PressFlag) && 
		 (0 == ucLSW1_2DeBouncingCnt))
	{
		if(1 <= ucLSW1_2BouncingCnt)
		{
			ucLSW1_2BouncingCnt++;
			if(LSW_BOUNCING_CNT <= ucLSW1_2BouncingCnt)
			{
				if(ucLSW1_2ReadBackup == ucLSWReadVar)
				{
					if(0 == (ucLSWReadVar & LIMIT_SW_BIT_1))
					{
						StatusFlag.stStatusFlag.bAwayPipePosition = SET;
						StatusFlag.stStatusFlag.bTowardPipePosition = RESET;
						GPIO_WriteBit(LIMIT_SW_STATUS_LED_PORT,
													GUN_AWAY_LIMIT, LED_ON);
					  #ifdef LT_SW_DBG    	
							DEBUG_PRINTF("LSW1\r\n");
            #endif 
						StatusFlag.stStatusFlag.bLSWPressed = SET;
						ucLSWDetect = (ucLSWDetect & 0x0C)|0x01;
					}
					if(0 == (ucLSWReadVar & LIMIT_SW_BIT_2))
					{
						StatusFlag.stStatusFlag.bTowardPipePosition = SET;
						StatusFlag.stStatusFlag.bAwayPipePosition = RESET;
						GPIO_WriteBit(LIMIT_SW_STATUS_LED_PORT,
													GUN_TOWARDS_LSW_LED, LED_ON);
						#ifdef LT_SW_DBG    	
							DEBUG_PRINTF("LSW2\r\n");
            #endif 
						StatusFlag.stStatusFlag.bLSWPressed = SET;
						ucLSWDetect = (ucLSWDetect & 0x0C)|0x02;
					}
				}
				ucLSW1_2BouncingCnt = 0;
			}
		}
	}
	else if(1 <= ucLSW1_2DeBouncingCnt)
	{
		ucLSW1_2DeBouncingCnt++;
		if(LSW_DEBOUNCING_CNT <= ucLSW1_2DeBouncingCnt)
		{
			if(LIMIT_SW_BITS1_2 == (ucLSWReadVar & LIMIT_SW_BITS1_2))
			{
				ucLSW1_2DeBouncingCnt = 0;
				StatusFlag.stStatusFlag.bLSW1_2PressFlag = RESET;
				GPIO_WriteBit(LIMIT_SW_STATUS_LED_PORT,
											GUN_AWAY_LIMIT, LED_OFF);
				GPIO_WriteBit(LIMIT_SW_STATUS_LED_PORT,
											GUN_TOWARDS_LSW_LED, LED_OFF);
			}
		}	
	}
		
	if((SET == StatusFlag.stStatusFlag.bLSW3_4PressFlag) && 
		 (0 == ucLSW3_4DeBouncingCnt))
	{
		
		if(1 <= ucLSW3_4BouncingCnt)
		{
			ucLSW3_4BouncingCnt++;
			if(LSW_BOUNCING_CNT <= ucLSW3_4BouncingCnt)
			{
				if(ucLSW3_4ReadBackup == ucLSWReadVar)
				{
					ucCurrentLimitSwitch++;
					if(0 == (ucLSWReadVar & LIMIT_SW_BIT_3))
					{
						if(1 == ucCurrentLimitSwitch)
						{
							ucCurrentLimitSwitch++;
						}
						StatusFlag.stStatusFlag.bDCLSPosition = SET;						
						StatusFlag.stStatusFlag.bHomePosition = RESET;
						
						/*this is added to move machine to home*/
						StatusFlag.stStatusFlag.bDCLSPosMTH = SET;
						StatusFlag.stStatusFlag.bHLSPosMTH = RESET;
						GPIO_WriteBit(LIMIT_SW_STATUS_LED_PORT, 
													DCLS_POSITION_LSW_LED, LED_ON);
						#ifdef LT_SW_DBG    	
							DEBUG_PRINTF("LSW3\r\n");
            #endif 
						StatusFlag.stStatusFlag.bLSWPressed = SET;
						ucLSWDetect = (ucLSWDetect & 0x03)|0x04;
					}
				  if(0 == (ucLSWReadVar & LIMIT_SW_BIT_4))
					{
						StatusFlag.stStatusFlag.bHomePosition = SET;
						StatusFlag.stStatusFlag.bDCLSPosition = RESET;
						
						/*this is added to move machine to home*/
						StatusFlag.stStatusFlag.bDCLSPosMTH = RESET;
						StatusFlag.stStatusFlag.bHLSPosMTH = SET;
						
						GPIO_WriteBit(LIMIT_SW_STATUS_LED_PORT, 
													HOME_POSITION_LSW_LED, LED_ON);
						#ifdef LT_SW_DBG    	
							DEBUG_PRINTF("LSW4\r\n");
            #endif 
						StatusFlag.stStatusFlag.bLSWPressed = SET;
						ucLSWDetect = (ucLSWDetect & 0x03)|0x08;
					}
					
					if(SET == StatusFlag.stStatusFlag.bStartSpray)
					{
						switch(ucCurrentLimitSwitch)
						{
							case 1:
							case 3:
							case 6:
							case 8:
							{
								if(SET != StatusFlag.stStatusFlag.bHomePosition)
								{
									ucSprayFaultStatus |= SEQUENCE_ERROR;
									StatusFlag.stStatusFlag.bSprayFault = SET;
									StatusFlag.stStatusFlag.bDCLSPosition = RESET;
									StatusFlag.stStatusFlag.bHomePosition = RESET;
									StatusFlag.stStatusFlag.bLSWSequenceErrorHLS = SET;									
									#ifdef LT_SW_DBG
										DEBUG_PRINTF("HLS SWITCH DETECTION FAULT\r\n");
									#endif
								}					
							}
							break;
							case 2:
							case 4:
							case 5:
							case 7:	
							{
								if(SET != StatusFlag.stStatusFlag.bDCLSPosition)
								{
									ucSprayFaultStatus |= SEQUENCE_ERROR;
									StatusFlag.stStatusFlag.bSprayFault = SET;
									StatusFlag.stStatusFlag.bHomePosition = RESET;
									StatusFlag.stStatusFlag.bDCLSPosition = RESET;
									
									StatusFlag.stStatusFlag.bLSWSequenceErrorDCLS = SET;
									
									
									#ifdef LT_SW_DBG
										DEBUG_PRINTF("DCLS SWITCH DETECTION FAULT\r\n");
									#endif
								}	
							}								
							break;				
						}
					}
				}
				ucLSW3_4BouncingCnt = 0;
			}
		}
	}
	else if(1 <= ucLSW3_4DeBouncingCnt)
	{
		ucLSW3_4DeBouncingCnt++;
		if(LSW_DEBOUNCING_CNT <= ucLSW3_4DeBouncingCnt)
		{
			if(LIMIT_SW_BITS3_4 == ((ucLSWReadVar) & LIMIT_SW_BITS3_4))
			{
				ucLSW3_4DeBouncingCnt = 0;
				StatusFlag.stStatusFlag.bLSW3_4PressFlag = RESET;
				GPIO_WriteBit(LIMIT_SW_STATUS_LED_PORT, 
											DCLS_POSITION_LSW_LED, LED_OFF);
				GPIO_WriteBit(LIMIT_SW_STATUS_LED_PORT,
											HOME_POSITION_LSW_LED, LED_OFF);
			}
		}	
	}
	
	/* Clear update interrupt bit*/
  TIM_ClearITPendingBit(TIM2, TIM_FLAG_Update);
}
/*****************************************************************************
 **@Function    : TIM1_UP_IRQHandler
 **@Description	: Timer 1 interrupt handler for 10 msec
 **@Parameters	: None
 **@Return			: None
 ***************************************************************************/
void TIM1_UP_IRQHandler(void)
{
	/* Clear update interrupt bit*/
  TIM_ClearITPendingBit(TIM1,TIM_FLAG_Update);
}

/*************************************************************************
 **@Function    : TIM3_IRQHandler
 **@Description	: Timer 3 interrupt handler for the Delay of 1 msec
 **@Parameters	: None
 **@Return			: None
 *************************************************************************/
void TIM3_IRQHandler(void)
{
//	static char uifreecount = 0;
//	uifreecount++;
//		if(uifreecount == 1)
//		{
//			GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_SET);
//		}
//		else
//		{
//			uifreecount = 0;
//			GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_RESET);
//		}
				
	if(SET == StatusFlag.stStatusFlag.bFirstHrtBtMsgFlag)
	{
		if(SET == StatusFlag.stStatusFlag.bSprayOverlapTime)
		{
			uiSprayOverlapCounter++;
		}
		if(SET == StatusFlag.stStatusFlag.bReverseDelayTime)
		{
			uiReverseDelayCounter++;
		}
		if(SET == StatusFlag.stStatusFlag.bGunDelayTimerFlag)
		{
			uiGunDelayCounter++;
		}
		

		uc10msCnt++;
		if(uc10msCnt >= 10)
		{
			uc10msCnt = 0;
			/*Counter to read the valve status*/
			uiValveReadCounter++;
				
			/*WDT counter*/
			uiWDTCount++;

			/*Heartbeat Counter*/
			uiHBTimeCounter++;
			uiTimCount++;
			uiTempReadCnt++;
			/*Automatic flush after 30 sec*/
			if(SET == StatusFlag.stStatusFlag.bAutoFlushFlag)
			{
				uiAutoFlushCnt++;
			}
			if(SET == StatusFlag.stStatusFlag.bCANFaultFlag)
			{
				ucLEDBlinkCount++;
			}
			if(SET == StatusFlag.stStatusFlag.bFirstHrtBtMsgFlag)
			{
				uiBatteryReadCount++;
			}
			if(SET == StatusFlag.stStatusFlag.bTxFlag)
			{
				uiTxCounter++;
			}
			
			if	((SET == StatusFlag.stStatusFlag.bPrimeSelector) ||
					(SET == StatusFlag.stStatusFlag.bFlushSelector) ||
					(SET == StatusFlag.stStatusFlag.bSpraySelector))
			{
				uiGunPositionCounter++;
			}
				
			if(SET == StatusFlag.stStatusFlag.bDCLSFlag)
			{
				uiDCLSCounter++;
			}

			if(SET == StatusFlag.stStatusFlag.bFirstRotationFlag)
			{
				uiDclsToHomeFirstCnt++;
			}
			
			/******/
			if(SET == StatusFlag.stStatusFlag.bNoLimitSwitchPressFlag)
			{
				uiNoLimitSwitchCnt++;
			}
			/***********/
			
			if(SET == StatusFlag.stStatusFlag.bHLSFlag)
			{
				uiHLSCounter++;
			}



			if(SET == StatusFlag.stStatusFlag.bBackHlsTimer)
			{
				uiBackHlsCounter++;
			}
			
			if(SET == StatusFlag.stStatusFlag.bProcessTimeFlag)
			{
				uiProcessTimeCount++;
			}
			
			
			/*this timer is used for flushing when OPMM switches on Battery*/
			if(((SET == StatusFlag.stStatusFlag.bVbatSwichFlag) ||
					(StatusFlag.stStatusFlag.bAutoFlushOnError == SET)) && 
				 (SET == StatusFlag.stStatusFlag.bGunDelayCountFlag))
			{
				uiGunDelayCnt++;
			}
			
			/*adf 21/10/2016*/
//			if(SET == StatusFlag.stStatusFlag.bFlushWarnTime)
//			{
//				 uiFlushWarnCnt++;
//			}

			
		}
	}
	else
	{
		uc10msCnt++;
		if(uc10msCnt >= 10)
		{
			uc10msCnt = 0;
			ucPOSTLEDBlinkCnt++;
		}
	}
	/* Clear update interrupt bit*/
  TIM_ClearITPendingBit(TIM3, TIM_FLAG_Update);
}
/******************************************************************************
 **@Function    : USB_LP_CAN1_RX0_IRQHandler
 **@Description : This Function handles CAN1 Interrupt Request.
 **@Parameters  : None 
 **@Return      : None
 ****************************************************************************/
void USB_LP_CAN1_RX0_IRQHandler(void)
{
	CAN_Receive(CAN1, CAN_FIFO0, &rgstRxCANBuff[ucStoreRxCANBuffCnt++]);
	
	if(RX_CAN_BUF_SIZE <= ucStoreRxCANBuffCnt)
	{
		ucStoreRxCANBuffCnt = 0;
		StatusFlag.stStatusFlag.bDataInCANRxBuffFlag =  SET;
	}
	StatusFlag.stStatusFlag.bMCPCAN2ActiveFlag = RESET;
	StatusFlag.stStatusFlag.bCAN1ActiveFlag = SET;
//	//#ifdef CAN_DBG
	//DEBUG_PRINTF("CAN1 ISR\r\n");
//	//#endif
}

/*****************************************************************************  
 **@Function 		: EXTI9_5_IRQHandler  
 **@Description	: EXTI Interrupt 9 to 5 Handler for MCP CAN and 
									ESTOP Monitoring IRQ Handler
 **@Parameters	: None  
 **@Return			: None  
 ***************************************************************************/ 
void EXTI9_5_IRQHandler(void)
{
	if(SET == EXTI_GetITStatus(MCP_CAN2_EXTI_LINE)) 
	{
		//StatusFlag.stStatusFlag.bCAN2MsgRcvFlag = RESET;
		fnMCPCANMsg_Process();
		//StatusFlag.stStatusFlag.bCAN2MsgRcvFlag 		= SET;
		StatusFlag.stStatusFlag.bMCPCAN2ActiveFlag 	= SET;
		StatusFlag.stStatusFlag.bCAN1ActiveFlag 		= RESET;
		EXTI_ClearITPendingBit(MCP_CAN2_EXTI_LINE);
	
		#ifdef CAN_DBG
			DEBUG_PRINTF("@\r\n");
		#endif
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
			I2C_Send7bitAddress(I2C_IO_EXP, ucIOExpSlaveAddr, I2C_DIRECTION_TRANSMITTER);
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
				StatusFlag.stStatusFlag.bI2CReading = RESET;
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
		StatusFlag.stStatusFlag.bI2CReading = RESET;
		ucI2C1Seq = 0;
		uiI2CWriteData = 0;
		#ifdef GENERAL_DBG
			DEBUG_PRINTF("I2C1 Error!!\r\n");
		#endif
	}
}
void HardFault_Handler(void)
{
	//DEBUG_PRINTF("HardFault_Handler\r\n");
}

void BusFault_Handler(void)
{
	//DEBUG_PRINTF("BusFault_Handler\r\n");
}

void MemManage_Handler(void)
{
	//DEBUG_PRINTF("MemManage_Handler\r\n");
}


void UsageFault_Handler(void)
{
	//DEBUG_PRINTF("UsageFault_Handler\r\n");
}





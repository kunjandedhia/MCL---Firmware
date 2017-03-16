/******************************************************************************
* @file    Temperature_Control.c
* 
* @brief   This file contains all the Functions definitions which required for 
					 Sensing, Proccessing and Actuating/contorling heaters and 
					 thermocouple.
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
uint16_t uiTempReadCnt 		= 0;
uint16_t uiTempReadCount 	= 0;
uint8_t ucHardSSRPosition = 0;
uint8_t ucPreSSRStatus		=	0;
uint8_t ucSSRStatus				=	0;

TC_t stTC;

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
/********************************************************************************
 **@Function 		: fnHeater_Check
 **@Description	: Check the Heater/SSR ON/OFF status and fault as well
 **@Parameters	: None
 **@Return			: None
 ******************************************************************************/
void fnHeater_Check(void)
{
	uint8_t ucLoop = 0;
	
	/*if Heater ON/OFF Status changes then send Status to the SCU AND HMI*/
	if(ucPreSSRStatus != ucSSRStatus)
	{
			memset(stTxMsg.Data, 0, sizeof(stTxMsg.Data));
			stTxMsg.DLC = 1;
			stTxMsg.Data[0] = ucSSRStatus;
			stTxMsg.ExtId   = MSG_TYPE_STATUS | RESERVED | STATUS_HCM_HEATER |
												SOURCE_HCM;
		
		
		#ifdef OPER_MODE_DBG
			DEBUG_PRINTF("HEATER Status Send = %d\r\n", ucSSRStatus);
		#endif
		fnCANMessage_Send(&stTxMsg);
		
		ucPreSSRStatus = ucSSRStatus;
	}
	/*Check for Heater faults*/
	for(ucLoop = 0; ucLoop < NO_OF_SSR - 1; ucLoop++)
	{
		if(stTC.stProcess[ucLoop].bHeaterStatusFlag != 
			 GPIO_ReadInputDataBit(HEATER_SSR_FEEDBAK_PORT,
											 HEATER_FEEDBACK_PIN(stTC.stProcess[ucLoop].ucSSRPosition)))
		{
			/*SSR Fault LED ON*/
			if(RESET == stTC.stProcess[ucLoop].bHeaterFaultFlag)
			{
				/*SEND Heater fault to SCU and HMI*/
				stTC.stProcess[ucLoop].bHeaterFaultFlag = SET;
				
				uiIOExpander1 &= ~SSR_LED_VALUE(ucLoop, GREEN);/*Green/Good Led OFF*/
				uiIOExpander1 |= SSR_LED_VALUE(ucLoop, RED); /*RED/Fault Led ON*/
				
				 /* heater OFF ***/
				fnHeater_ONOFF(ucLoop, HEATER_OFF);
				
				memset(stTxMsg.Data,0, sizeof(stTxMsg.Data));
				stTxMsg.DLC = 1;
				stTxMsg.Data[0] = 0x01 << ucLoop;
				stTxMsg.ExtId = MSG_TYPE_FAULT | RESERVED | FAULT_HEATER |
												SOURCE_HCM;
	
				fnCANMessage_Send(&stTxMsg);
				#ifdef OPER_MODE_DBG
					DEBUG_PRINTF("HEATER FAULT Of %d\r\n",ucLoop);
				#endif
			}
		}
	}
	/*Check for the Hard Heater Fault*/	
	if(StatusFlag.stStatusFlag.bHardHeaterStatusFlag != 
			 GPIO_ReadInputDataBit(HEATER_SSR_FEEDBAK_PORT,
														 HEATER_FEEDBACK_PIN(ucHardSSRPosition)))
	{
		if(RESET == StatusFlag.stStatusFlag.bHardHeaterFaultFlag)
		{
			StatusFlag.stStatusFlag.bHardHeaterFaultFlag = SET;
			
			uiIOExpander1 &= ~SSR_LED_VALUE(HARDNER_HEATER, GREEN);
			uiIOExpander1 |= SSR_LED_VALUE(HARDNER_HEATER, RED); 
			
			fnHeater_ONOFF(HARDNER_HEATER, HEATER_OFF);
			
			memset(stTxMsg.Data,0, sizeof(stTxMsg.Data));
			stTxMsg.DLC = 1;
			stTxMsg.Data[0] = 0x01 << HARDNER_HEATER;
			stTxMsg.ExtId = MSG_TYPE_FAULT | RESERVED | FAULT_HEATER |
													SOURCE_HCM;;

			fnCANMessage_Send(&stTxMsg);
			#ifdef OPER_MODE_DBG
				DEBUG_PRINTF("HARD HEATER FAULT\r\n");
			#endif
		}		
	}
}

/********************************************************************************
 **@Function		 	:  fnHeater_ONOFF
 **@Descriptions	:  This function is used to ON/OFF the SSR and Update LED's 
										 accordingly
 **@Parameters		:  None
 **@Return				:  None
 ******************************************************************************/
void fnHeater_ONOFF(uint8_t ucStructVal, BitAction bState)
{
	if(HARDNER_HEATER == ucStructVal)
	{
		if((RESET == StatusFlag.stStatusFlag.bHardHeaterFaultFlag) || 
			 (Bit_RESET == bState))
		{
			/* Indication for heater ON/OFF */
			StatusFlag.stStatusFlag.bHardHeaterStatusFlag = bState; 
					
			GPIO_WriteBit(HEATER_SSR_PORT, HEATER_SSR_PIN(ucHardSSRPosition), bState); 
			ucSSRStatus &= ~(0x01 << ucStructVal); /*Clear the Pre Status Bit*/
			ucSSRStatus |= (uint8_t)((uint8_t)bState << ucStructVal); /*Update the New Status Bit*/
		}
	}
	else
	{
		if((RESET == stTC.stProcess[ucStructVal].bHeaterFaultFlag) || 
			 (Bit_RESET == bState))
		{
			/* indication for heater ON/OFF */				
			stTC.stProcess[ucStructVal].bHeaterStatusFlag = bState;
			
			GPIO_WriteBit(HEATER_SSR_PORT,
			HEATER_SSR_PIN(stTC.stProcess[ucStructVal].ucSSRPosition), bState);
			/*Heater Status Updation*/
			ucSSRStatus &= ~(0x01 << ucStructVal); /*Clear the Pre Status Bit*/
			ucSSRStatus |= (uint8_t)((uint8_t)bState << ucStructVal); /*Update the New Status Bit*/
		}
	}
	/*Updata SSR LED Only when No Fault*/
	if(((HARDNER_HEATER == ucStructVal) && 
			(RESET == StatusFlag.stStatusFlag.bHardHeaterFaultFlag)) || 
		 ((HARDNER_HEATER != ucStructVal) && 
			(RESET == stTC.stProcess[ucStructVal].bHeaterFaultFlag)))
	{
		/*Update Led status*/
		if(Bit_SET == bState)
		{
			uiIOExpander1  |= SSR_LED_VALUE(ucStructVal, GREEN);
		}
		else
		{
			uiIOExpander1 &= ~SSR_LED_VALUE(ucStructVal, GREEN);
		}
	}
	#ifdef OPER_MODE_DBG
	  DEBUG_PRINTF("Struct : %d , Heater ON: %d\r\n", ucStructVal, bState);
	#endif
}




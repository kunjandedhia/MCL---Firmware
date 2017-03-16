/******************************************************************************
* @file    Main.c
* 
* @brief   Main program body.
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
 **@Function 	  : main 
 **@Description : main
 **@Parameters	: None
 **@Return			: None
 *****************************************************************************/
int main(void)
{  
  fnSystem_Init();
	 
//	/*Set all Flags to default value*/
	fnAllFlags_Reset();		

#ifdef GENERAL_DBG
	DEBUG_PRINTF("Start OPMM \r\n");
#endif

	/*setting Duty Cycle*/
	TIM_SetCompare3(TIM1, 0);
	TIM_Cmd(TIM3, ENABLE);
	BUZZER_OFF();
	
//	/*Initialize WDT*/
//	fnWDT_Initialize();

	while(1) 
  {
		/*SET the duty cycle for 50%*/
		/*Process the received CAN1 data if available*/
		if((ucRxCANProcessCnt < ucStoreRxCANBuffCnt) || 
			 (SET == StatusFlag.stStatusFlag.bDataInCANRxBuffFlag))
		{
			fnCANMsg_Validate(&rgstRxCANBuff[ucRxCANProcessCnt++]);
			#ifdef GENERAL_DBG
				DEBUG_PRINTF("In CAN Msg RX, Process cnt = %d\r\nStore Cnt = %d\r\n",
											ucRxCANProcessCnt,ucStoreRxCANBuffCnt);
			#endif 

			if(RX_CAN_BUF_SIZE <= ucRxCANProcessCnt)
			{
				ucRxCANProcessCnt = 0;
				StatusFlag.stStatusFlag.bDataInCANRxBuffFlag = RESET;
			}
		}
		
//		if(SET == StatusFlag.stStatusFlag.bCAN2MsgRcvFlag)
//		{	
//			StatusFlag.stStatusFlag.bCAN2MsgRcvFlag = RESET;
//			fnMCPCANMsg_Process();
//		}
//		/*WDT Check*/
//		fnWDT_Check();
//			
		/*Check for the ESTOP message*/
		fnEstop_Check();

		/*Check for the Acknowledgement for the previous data transmitted */
		fnACK_Check();
				
		/*Default Mode*/
		fnDefault_Mode();

		/*Operation Mode*/
		fnOperation_Mode();
		
		/*Heartbeat check*/
		fnHearBeat_Check();
	
		fnLED_Status();

		/*Battery Check Function*/
		fnBattery_Status();
		
		
  }  
}








//		if((127 < fnMCP2515Register_Read(MCP2515_TEC)) ||
//  		 (127 < fnMCP2515Register_Read(MCP2515_REC)))
//		{
//			StatusFlag.stStatusFlag.bCAN1ActiveFlag = SET;
//			StatusFlag.stStatusFlag.bMCPCAN2ActiveFlag = RESET;
//			DEBUG_PRINTF("CAN2 Fault Occurs REC: %d, TEC: %d\r\n", fnMCP2515Register_Read(MCP2515_REC),fnMCP2515Register_Read(MCP2515_TEC));
//		}
//		if((127 < CAN_GetLSBTransmitErrorCounter(CAN1)) ||
//  		 (127 < CAN_GetReceiveErrorCounter(CAN1)))
//		{
//			StatusFlag.stStatusFlag.bCAN1ActiveFlag = RESET;
//			StatusFlag.stStatusFlag.bMCPCAN2ActiveFlag = SET;
//			DEBUG_PRINTF("CAN1 Fault Occurs \r\n");
//		}
		
/*		
		if(uiAutoFlushCnt > 3000)
		{
			StatusFlag.stStatusFlag.bAutoFlushByTime = SET;
			fnAutoFlush();
		}		
*/

/*
if(1 == rgstRxCANBuff[ucRxCANProcessCnt - 1].Data[0])
			{
				//ON OFF the Status  
				uiIOExpader1 =  IOEX_ALL_BLUE;//IOEX_ALL_RED;//IOEX_ALL_GREEN ;
				uiIOExpader2 =  IOEX_ALL_BLUE;//IOEX_ALL_RED;//IOEX_ALL_GREEN ;	
			}
			else if(2 == rgstRxCANBuff[ucRxCANProcessCnt - 1].Data[0])
			{
				uiIOExpader1 =  IOEX_ALL_RED;//IOEX_ALL_GREEN ;
				uiIOExpader2 =  IOEX_ALL_RED;//IOEX_ALL_GREEN ;	
			}
			else if(3 == rgstRxCANBuff[ucRxCANProcessCnt - 1].Data[0])
			{
				uiIOExpader1 =  IOEX_ALL_GREEN ;
				uiIOExpader2 =	IOEX_ALL_GREEN ;	
			}
			else if(4 == rgstRxCANBuff[ucRxCANProcessCnt - 1].Data[0])
			{
				uiIOExpader1 =  IOEX_ALL_BLUE | IOEX_ALL_RED | IOEX_ALL_GREEN ;
				uiIOExpader2 =  IOEX_ALL_BLUE | IOEX_ALL_RED | IOEX_ALL_GREEN ;	
			}
			else
			{
				uiIOExpader1 = 0;
				uiIOExpader2 = 0;	
			}
*/


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
 **@Function 		: main
 **@Description	: Main Function
 **@Parameters	: None
 **@Return			: None
 ****************************************************************************/
int main(void)
{		
	/* Initializing system periperals */
	fnSystem_Init();
	
	#ifdef GENERAL_DBG
		DEBUG_PRINTF("Successfully init\r\n");
	#endif
	TIM_Cmd(TIM1, ENABLE);
//  ucMode = OPERATION_MODE;
	while(1)
	{
		if((ucRxCANProcessCnt < ucStoreRxCANBuffCnt) || 
			 (StatusFlag.stStatusFlag.bDataInCANRxBuffFlag == SET))
		{
			fnCANMsg_Validate(&rgstRxCANBuff[ucRxCANProcessCnt++]);
			#ifdef GENERAL_DBG
				DEBUG_PRINTF("PROCESS COUNT = %d STORE COUNT = %d\r\n",
											ucRxCANProcessCnt,ucStoreRxCANBuffCnt);
			#endif 
			if(RX_CAN_BUF_SIZE <= ucRxCANProcessCnt)
			{
				ucRxCANProcessCnt = 0;
				StatusFlag.stStatusFlag.bDataInCANRxBuffFlag = RESET;
			}
		}
		/************Check for the ESTOP message*****************/
		fnEstop_Check();
		
		/* Check for the Acknowledgement for the previous data transmitted */
		fnACK_Check();
			
		/*******Default Mode***********/
		fnDefault_Mode();
		
		/************* Operation MODE Only After Diagnostics Pass*************/	
		fnOperation_Mode();
				
		/* heartbeat check*/
		fnHearBeat_Check();

		/* blink the Red Led if HeartBeat Not Received */
		fnLED_Indication();
//		DEBUG_PRINTF("SSR Diagnostics Pass POSITION = %d\r\n",ucHardSSRPosition);
	}	
}
	

////			if(0 == ch)
////			{
////				uiIOExpander1 	= IO1_ALL_GREEN_LED; /*Variable for I/O Expander 1*/
////				uiIOExpander2 	= IO1_ALL_GREEN_LED; /*Variable for I/O Expander 2*/
////				ch++;
////			}
////			else if(1 == ch)
////			{
////				uiIOExpander1 	= IO1_ALL_BLUE_LED; /*Variable for I/O Expander 1*/
////				uiIOExpander2 	= IO1_ALL_BLUE_LED; /*Variable for I/O Expander 2*/
////				ch++;
////			}
////			else if(2 == ch)
////			{
////				uiIOExpander1 	= IO2_ALL_RED_LED; /*Variable for I/O Expander 1*/
////				uiIOExpander2 	= IO2_ALL_RED_LED; /*Variable for I/O Expander 2*/
////					ch++;
////			}
////			else if(3 == ch)
////			{
////				uiIOExpander1 	= IO1_ALL_BLUE_LED; /*Variable for I/O Expander 1*/
////				uiIOExpander2 	= IO1_ALL_BLUE_LED; /*Variable for I/O Expander 2*/
////				ch++;
////			}
////			else if(4 == ch)
////			{
////				uiIOExpander1 	= IO1_ALL_BLUE_LED; /*Variable for I/O Expander 1*/
////				uiIOExpander2 	= IO1_ALL_BLUE_LED; /*Variable for I/O Expander 2*/
////				ch++;
////			}
////			else if(5 == ch)
////			{
////				uiIOExpander1 	= IO1_ALL_BLUE_LED; /*Variable for I/O Expander 1*/
////				uiIOExpander2 	= IO1_ALL_BLUE_LED; /*Variable for I/O Expander 2*/
////				ch++;
////			}


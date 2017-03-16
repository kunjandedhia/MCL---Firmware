/******************************************************************************
**@file    Main.c
* 
**@brief   This file contains main function
******************************************************************************
*/    
/* ----------------------------------------------------------------------------
*                           Includes
* ----------------------------------------------------------------------------
*/
#include "System_Header.h"
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
/*****************************************************************************
**@Function		 	:  main()
**@Descriptions	:  Main Loop
**@parameters		:  none
**@return				:  none
*****************************************************************************/
int main(void)
{
	fnSystem_Init();
	//#ifdef	GENERAL_DBG
		DEBUG_PRINTF("Start OP \r\n");
	//#endif
	//TimeConvert();
//	stConfig_Value.fOPMMBatKneeVoltage = 10.5;
//	/*VCM*/
//	stConfig_Value.ucConfigConnPressureGuage1Rev 		= stConfig_Value.ucConfigConnPressureGuage1;
//	stConfig_Value.ucConfigConnPressureGuage2Rev 		= stConfig_Value.ucConfigConnPressureGuage2;
//	stConfig_Value.ucConfigConnPressureGuage3Rev 		= stConfig_Value.ucConfigConnPressureGuage3;
//	stConfig_Value.ucConfigConnLinearTransducerRev 	= stConfig_Value.ucConfigConnLinearTransducer;
//	stConfig_Value.ucConfigConnInsideEE07Rev 				= stConfig_Value.ucConfigConnInsideEE07;
//	stConfig_Value.ucConfigConnOutsideEE07Rev 			= stConfig_Value.ucConfigConnOutsideEE07;
//	stConfig_Value.ucConfigConnStallvalveRev 				= stConfig_Value.ucConfigConnStallvalve;
//	
//	/*store current value in revert variable*/
//	/*HCM*/
//	stConfig_Value.ucConfig_ConnBaseInlineTherRev 		= stConfig_Value.ucConfig_ConnBaseInlineTher;
//	stConfig_Value.ucConfig_ConnHardInlineTherRev 		= stConfig_Value.ucConfig_ConnHardInlineTher;
//	stConfig_Value.ucConfig_ConnAntifreezeTherRev 		= stConfig_Value.ucConfig_ConnAntifreezeTher;
//	stConfig_Value.ucConfig_ConnBasereturnlineTherRev = stConfig_Value.ucConfig_ConnBasereturnlineTher;
//	stConfig_Value.ucConfig_ConnBaseinlineHeaterRev 	= stConfig_Value.ucConfig_ConnBaseinlineHeater;
//	stConfig_Value.ucConfig_ConnHardinlineHeaterRev 	= stConfig_Value.ucConfig_ConnHardinlineHeater;
//	stConfig_Value.ucConfig_ConnAntifreezeHeaterRev 	= stConfig_Value.ucConfig_ConnAntifreezeHeater;
//	stConfig_Value.ucConfig_ConnHardHeaterRev 				= stConfig_Value.ucConfig_ConnHardHeater;
//	stConfig_Value.ucConfig_ConnBaseTankLLRev 				= stConfig_Value.ucConfig_ConnBaseTankLL;
//	stConfig_Value.ucConfig_ConnHardTankLLRev 				= stConfig_Value.ucConfig_ConnHardTankLL;
//				

//	/*OPMM*/
//	stConfig_Value.ucConfigConnSprayvalveRev 		= stConfig_Value.ucConfigConnSprayvalve;
//	stConfig_Value.ucConfigConnGunpositionRev 	= stConfig_Value.ucConfigConnGunposition;
//	stConfig_Value.ucConfigConnABvalveRev 			= stConfig_Value.ucConfigConnABvalve;
//	stConfig_Value.ucConfigConnSolventvalveRev 	= stConfig_Value.ucConfigConnSolventvalve;
//	stConfig_Value.ucConfigConnForwardvalveRev 	= stConfig_Value.ucConfigConnForwardvalve;
//	stConfig_Value.ucConfigConnBackwardvalveRev = stConfig_Value.ucConfigConnBackwardvalve;
//	stConfig_Value.ucConfigNCTempSensor1Rev 		= stConfig_Value.ucConfigNCTempSensor1;
//	stConfig_Value.ucConfigNCTempSensor2Rev 		= stConfig_Value.ucConfigNCTempSensor2;
//	stConfig_Value.ucConfigNCTempSensor3Rev 		= stConfig_Value.ucConfigNCTempSensor3;
//	stConfig_Value.ucConfigNCTempSensor4Rev 		= stConfig_Value.ucConfigNCTempSensor4;

																			
//	stDiagStatus.bSCU_DiagStatusReceived = TRUE;
//	stDiagStatus.bHMI_DiagStatusSent = FALSE;
//				
	while(1)
	{
		if( (TRUE == stDiagStatus.bSCU_DiagStatusReceived) && 
				(FALSE == stDiagStatus.bHMI_DiagStatusSent) )
		{
			fnSelfDiagnostics_Test();
			
			fnLED_TurnONOFF (PORT_LED_ERR_RED, Bit_RESET,PIN_LED_ERR_RED);
			fnLED_TurnONOFF (PORT_LED_COM_RED, Bit_RESET,PIN_LED_COM_RED);
			fnLED_TurnONOFF (PORT_LED_ERR_BLUE, Bit_RESET,PIN_LED_ERR_BLUE);
			fnLED_TurnONOFF (PORT_LED_COM_BLUE, Bit_RESET,PIN_LED_COM_BLUE);
		}
		else if( (FALSE == stDiagStatus.bSCU_DiagStatusReceived) && 
						 (FALSE == stDiagStatus.bHMI_DiagStatusSent) )
		{
			/*Blick LEDs*/
			if(200 > uiPostLedBlinkCnt)
			{
				fnLED_TurnONOFF (PORT_LED_ERR_RED, Bit_SET,PIN_LED_ERR_RED);
				fnLED_TurnONOFF (PORT_LED_ERR_BLUE, Bit_RESET,PIN_LED_ERR_BLUE);
				
				fnLED_TurnONOFF (PORT_LED_COM_RED, Bit_SET,PIN_LED_COM_RED);
				fnLED_TurnONOFF (PORT_LED_COM_BLUE, Bit_RESET,PIN_LED_COM_BLUE);
			}
			else if(200 < uiPostLedBlinkCnt)
			{
				fnLED_TurnONOFF (PORT_LED_ERR_RED, Bit_RESET,PIN_LED_ERR_RED);
				fnLED_TurnONOFF (PORT_LED_ERR_BLUE, Bit_SET,PIN_LED_ERR_BLUE);
				
				fnLED_TurnONOFF (PORT_LED_COM_RED, Bit_RESET,PIN_LED_COM_RED);
				fnLED_TurnONOFF (PORT_LED_COM_BLUE, Bit_SET,PIN_LED_COM_BLUE);
				if(400 < uiPostLedBlinkCnt)
				{
					uiPostLedBlinkCnt = 0;
				}
			}
		}
		
		
		/* External WDT Check */
		//fnWDT_Check();
		
		/* ESTOP Check */
//		fnEStop_Check();
		
		/* check the ACK received or not */
		fnACK_Check();

		/* check heartbeat received or not */
		fnHeartBeat_Check();

		/* Flush Warning check */
		fnFlushWarning_Check();

		/* change Cursor Position */
		fnCursorPosition_Setting();

		/* check current switch press */
		fnSwitchPress_Check(ucCurrentSwitch);

		/* Current screen pattern */
		fnCurrentScreen_Pattern();

		/* Show Current Screen */
		fnCurrentScreen_Display(uiCurrentScreen);
	}
}

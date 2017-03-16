/******************************************************************************
**@file    I2C_GPIO_Expander.c
* 
**@brief   This file contains all the functions related to GPIO expander and I2C
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
	uint16_t uiSwBack_Up = 0;
	uint8_t ucBouncingCounter = 0;
	uint8_t ucDebouncing = 0;
	bool bKeyFlag = FALSE;
	uint8_t ucCurrentSwitch = 0x00;
	
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
**@Function		 	:  fnSw_Initialize
**@Descriptions	:	 initialization for switches
**@parameters		:  none
**@return				:  none
*****************************************************************************/
void fnSw_Initialize(void)
{
	GPIO_InitTypeDef stGPIO_InitStructure_SW;
	uint32_t uliNofSw;

	for ( uliNofSw = 0 ; uliNofSw < NUMBER_OF_SWITCHES ; uliNofSw++ )
	{
		memset(&stGPIO_InitStructure_SW, 0, sizeof(stGPIO_InitStructure_SW));
		stGPIO_InitStructure_SW.GPIO_Speed = GPIO_Speed_10MHz;
		stGPIO_InitStructure_SW.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		stGPIO_InitStructure_SW.GPIO_Pin = switch_map[uliNofSw].GPIO_Pin;
		GPIO_Init(switch_map[uliNofSw].GPIOx, &stGPIO_InitStructure_SW);
	}
}
/*****************************************************************************
**@Function		 	:  fnSw_Foreground_Service
**@Descriptions	:	 switch scanning
**@parameters		:  none
**@return				:  none
*****************************************************************************/
void fnSw_Foreground_Service(void)
{
	uint8_t ucPin;
	uint16_t uiSw = 0;

	uint32_t uliNofSw;
	/*******************************SWITCH READING******************************/
	for ( uliNofSw = 0 ; uliNofSw < NUMBER_OF_SWITCHES ; uliNofSw++ )
	{
		ucPin = GPIO_ReadInputDataBit(switch_map[uliNofSw].GPIOx, switch_map[uliNofSw].GPIO_Pin);
		uiSw = (uiSw << 1) | (uint16_t)((ucPin ^ 1) & 1);		
	}
	/************************IF DETECTED THAN START BOUNCING LOOP***************/
	if (0 != uiSw)
	{
		if (0 == ucBouncingCounter)
		{
			ucBouncingCounter++;
			uiSwBack_Up = uiSw;
		}
		else if (FALSE == bKeyFlag)
		{			
			ucBouncingCounter++;
			if (30 <= ucBouncingCounter)
			{
				if (uiSwBack_Up == uiSw)
					{
						if (0x2000 == uiSw)
						{
							#ifdef SWITCH_DBG
								DEBUG_PRINTF("\nSWITCH1 PRESSED\r\n");
							#endif
							ucCurrentSwitch = SWITCH1;
						}
						else if (0x1000 == uiSw)
						{
							#ifdef SWITCH_DBG
								DEBUG_PRINTF("\nSWITCH2 PRESSED\r\n");
							#endif
							ucCurrentSwitch = SWITCH2;
						}
						else if (0x0800 == uiSw)
						{	
							#ifdef SWITCH_DBG
								DEBUG_PRINTF("\nSWITCH3 PRESSED\r\n");
							#endif
							ucCurrentSwitch = SWITCH3;
						}
						else if (0x0400 == uiSw)
						{	
							#ifdef SWITCH_DBG
								DEBUG_PRINTF("\nSWITCH4 PRESSED\r\n");
							#endif
							ucCurrentSwitch = SWITCH4;
						}
						else if (0x0200 == uiSw)
						{	
							#ifdef SWITCH_DBG
								DEBUG_PRINTF("\nSWITCH5 PRESSED\r\n");
							#endif
							ucCurrentSwitch = SWITCH5;
						}
						else if (0x0100 == uiSw)
						{	
							#ifdef SWITCH_DBG
								DEBUG_PRINTF("\nSWITCH6 PRESSED\r\n");
							#endif
							ucCurrentSwitch = SWITCH6;
						}
						else if (0x0080 == uiSw)
						{	
							#ifdef SWITCH_DBG
								DEBUG_PRINTF("\nSWITCH7 PRESSED\r\n");
							#endif
							ucCurrentSwitch = SWITCH7;
						}
						else if (0x0040 == uiSw)
						{	
							#ifdef SWITCH_DBG
								DEBUG_PRINTF("\nSWITCH8 PRESSED\r\n");
							#endif
							ucCurrentSwitch = SWITCH8;
						}
						else if (0x0020 == uiSw)
						{	
							#ifdef SWITCH_DBG
								DEBUG_PRINTF("\nSWITCH9 PRESSED\r\n");
							#endif
							ucCurrentSwitch = SWITCH9;
						}
						else if (0x0010 == uiSw)
						{	
							#ifdef SWITCH_DBG
								DEBUG_PRINTF("\nSWITCH10 PRESSED\r\n");
							#endif
							ucCurrentSwitch = SWITCH10;
						}
						else if (0x0008 == uiSw)
						{	
							#ifdef SWITCH_DBG
								DEBUG_PRINTF("\nSWITCH11 PRESSED\r\n");
							#endif
							ucCurrentSwitch = SWITCH11;
						}
						else if (0x0004 == uiSw)
						{	
							#ifdef SWITCH_DBG
								DEBUG_PRINTF("\nSWITCH12 PRESSED\r\n");
							#endif
							ucCurrentSwitch = SWITCH12;
						}
						else if (0x0002 == uiSw)
						{	
							#ifdef SWITCH_DBG
								DEBUG_PRINTF("\nSWITCH13 PRESSED\r\n");
							#endif
							ucCurrentSwitch = SWITCH13;
						}
						else if (0x0001 == uiSw)
						{	
							#ifdef SWITCH_DBG
								DEBUG_PRINTF("\nSWITCH14 PRESSED\r\n");
							#endif
							ucCurrentSwitch = SWITCH14;
						}
					ucBouncingCounter = 0;
					bKeyFlag= TRUE;
					ucDebouncing = 1;
					}
			}
		}
}
	else
	{
		if (TRUE == bKeyFlag)
		{
			ucDebouncing++;
			if (30 <= ucDebouncing)
			{
				ucDebouncing=0;
				bKeyFlag = FALSE;
			}	
		}
		else
		{
				ucDebouncing=0;
				bKeyFlag= FALSE;
				ucBouncingCounter = 0;
		}
	}
}
/*****************************************************************************
**@Function		 	:  fnSwitchPress_Check()
**@Descriptions	:  Function to detect Current Switch pressed and perform respective operation
**@parameters		:  ucCurrentSwitch : current Switch number
**@return				:  none
*****************************************************************************/
void fnSwitchPress_Check(uint8_t ucCurrentSwitchNum)
{
	if( 0 != ucCurrentSwitchNum )
	{
		switch( ucCurrentSwitchNum )
		{
			case SWITCH1:
				{
					switch(uiCurrentScreen)
					{
						case SCREEN_ALARMS_1:
						case SCREEN_ALARMS_2:
						case SCREEN_ALARMS_3:
						case SCREEN_ALARMS_4:
						{
							ucCurrentSwitchNum = 0;					

							ucScreenPatternNumber = SCREEN_PATTERN_7;
							ucSwitchPressFlag = TRUE;
							
							uiCurrentScreen = SCREEN_CLEAR_FAULTS;
							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_CLEAR_FAULTS;
							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
						}break;
						
						default:
						{
							switch(uiCurrentScreen)
							{
								case SCREEN_VCM_PP_SETTING_1:
								case SCREEN_VCM_PP_SETTING_2:
								{
									if(FALSE == StatusFlag.stStatusFlag.bPortValuesSet)
									{
										StatusFlag.stStatusFlag.bVCMPortSetFail = FALSE;
										
										stConfig_Value.ucConfigConnPressureGuage1 	= stConfig_Value.ucConfigConnPressureGuage1Rev;
										stConfig_Value.ucConfigConnPressureGuage2 	= stConfig_Value.ucConfigConnPressureGuage2Rev;
										stConfig_Value.ucConfigConnPressureGuage3 	= stConfig_Value.ucConfigConnPressureGuage3Rev;
										stConfig_Value.ucConfigConnLinearTransducer = stConfig_Value.ucConfigConnLinearTransducerRev;
										stConfig_Value.ucConfigConnInsideEE07 			= stConfig_Value.ucConfigConnInsideEE07Rev;
										stConfig_Value.ucConfigConnOutsideEE07 			= stConfig_Value.ucConfigConnOutsideEE07Rev;
										stConfig_Value.ucConfigConnStallvalve 			= stConfig_Value.ucConfigConnStallvalveRev;
										/*change15*/
										stConfig_Value.ucConfigConnRecircvalve 			= stConfig_Value.ucConfigConnRecircvalveRev;
										stConfig_Value.uiConfigVCMBypass = stConfig_Value.uiConfigVCMBypassRev;
									}
								}break;
								case SCREEN_HCM_PP_SETTING_1:
								case SCREEN_HCM_PP_SETTING_2:
								{
									if(FALSE == StatusFlag.stStatusFlag.bPortValuesSet)
									{
										StatusFlag.stStatusFlag.bHCMPortSetFail = FALSE;
								
										stConfig_Value.ucConfig_ConnBaseInlineTher 			= stConfig_Value.ucConfig_ConnBaseInlineTherRev;
										stConfig_Value.ucConfig_ConnHardInlineTher 			= stConfig_Value.ucConfig_ConnHardInlineTherRev;
										stConfig_Value.ucConfig_ConnAntifreezeTher 			= stConfig_Value.ucConfig_ConnAntifreezeTherRev;
										stConfig_Value.ucConfig_ConnBasereturnlineTher	= stConfig_Value.ucConfig_ConnBasereturnlineTherRev;
										stConfig_Value.ucConfig_ConnBaseinlineHeater 		= stConfig_Value.ucConfig_ConnBaseinlineHeaterRev;
										stConfig_Value.ucConfig_ConnHardinlineHeater 		= stConfig_Value.ucConfig_ConnHardinlineHeaterRev;
										stConfig_Value.ucConfig_ConnAntifreezeHeater 		= stConfig_Value.ucConfig_ConnAntifreezeHeaterRev;
										stConfig_Value.ucConfig_ConnHardHeater 					= stConfig_Value.ucConfig_ConnHardHeaterRev;
										stConfig_Value.ucConfig_ConnBaseTankLL 					= stConfig_Value.ucConfig_ConnBaseTankLLRev;
										stConfig_Value.ucConfig_ConnHardTankLL 					= stConfig_Value.ucConfig_ConnHardTankLLRev;
										
										stConfig_Value.uiConfigHCMBypass = stConfig_Value.uiConfigHCMBypassRev;
									}
								}break;
								case SCREEN_OPMM_PP_SETTING_1:
								case SCREEN_OPMM_PP_SETTING_2:
								case SCREEN_OPMM_PP_SETTING_3:
								{
									if(FALSE == StatusFlag.stStatusFlag.bPortValuesSet)
									{
										StatusFlag.stStatusFlag.bOPMMPortSetFail = FALSE;
										
										stConfig_Value.ucConfigConnSprayvalve 		= stConfig_Value.ucConfigConnSprayvalveRev;
										stConfig_Value.ucConfigConnGunposition 		= stConfig_Value.ucConfigConnGunpositionRev;
										stConfig_Value.ucConfigConnABvalve 				= stConfig_Value.ucConfigConnABvalveRev;
										stConfig_Value.ucConfigConnSolventvalve 	= stConfig_Value.ucConfigConnSolventvalveRev;
										stConfig_Value.ucConfigConnForwardvalve 	= stConfig_Value.ucConfigConnForwardvalveRev;
										stConfig_Value.ucConfigConnBackwardvalve	= stConfig_Value.ucConfigConnBackwardvalveRev;
										stConfig_Value.ucConfigNCTempSensor1 			= stConfig_Value.ucConfigNCTempSensor1Rev;
										stConfig_Value.ucConfigNCTempSensor2 			= stConfig_Value.ucConfigNCTempSensor2Rev;
										stConfig_Value.ucConfigNCTempSensor3 			= stConfig_Value.ucConfigNCTempSensor3Rev;
										stConfig_Value.ucConfigNCTempSensor4 			= stConfig_Value.ucConfigNCTempSensor4Rev;
										
										stConfig_Value.uiConfigOPMMBypass = stConfig_Value.uiConfigOPMMBypassRev;
									}
								}break;
							}
							ucCurrentSwitchNum = 0;					
							/*MENU option*/

							ucScreenPatternNumber = SCREEN_PATTERN_2;
							ucSwitchPressFlag = TRUE;
							
							uiCurrentScreen = SCREEN_MENU_1;
							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_MENU_1;
							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
						}break;
					}
				}break;
			
			case SWITCH2:
				{
					switch(uiCurrentScreen)
					{
						case SCREEN_MENU_1:
						{				
							/*MENU option*/

							ucScreenPatternNumber = SCREEN_PATTERN_2;
							ucSwitchPressFlag = TRUE;
							
							uiCurrentScreen = SCREEN_PARAMETER_SET_1;
							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_PARAMETER_SET_1;
							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
						}break;
						
						case SCREEN_MENU_2:
						{				
							/*MENU option*/

							ucScreenPatternNumber = SCREEN_PATTERN_4;
							ucSwitchPressFlag = TRUE;
							
							uiCurrentScreen = SCREEN_STALL;
							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_STALL;
							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
						}break;
						
						case SCREEN_CURRENT_PARA:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_1;
							ucSwitchPressFlag = TRUE;
							
							uiCurrentScreen = SCREEN_PARAM_TEMP_1;
							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_PARAM_TEMP_1;
							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;	
						}break;
						
						case SCREEN_ADVANCED:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_1;
							ucSwitchPressFlag = TRUE;
							
							uiCurrentScreen = SCREEN_ADVANCED_AUTOCOAT;
							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_ADVANCED_AUTOCOAT;
							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
						}break;
						
						case SCREEN_STATUS:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_1;
							ucSwitchPressFlag = TRUE;
							
							uiCurrentScreen = SCREEN_MATERIAL_USAGE;
							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_MATERIAL_USAGE;
							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;	
						}break;
						
						case SCREEN_DIAGNOSTIC_STATUS:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_1;
							ucSwitchPressFlag = TRUE;
							
							uiCurrentScreen = SCREEN_SCU_DIAGNOSTIC;
							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_SCU_DIAGNOSTIC;
							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;	
						}break;
						
						case SCREEN_PARAMETER_SET_1:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_3;
							ucSwitchPressFlag = TRUE;
							
							uiCursorXPos = 66;
							uiCursorYPos = 15;
							
							ucLiquidLSetVariableCounter1 = 1;
							
							uiCurrentScreen = SCREEN_LIQUID_LEVEL_1;
							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_LIQUID_LEVEL_1  ;
							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
						}break;
						
						case SCREEN_PARAMETER_SET_2:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_3;
							ucSwitchPressFlag = TRUE;
							
							uiCursorXPos = 66;
							uiCursorYPos = 35;
							
							
							ucJobSetVariableCounter = 1;
							
							uiCurrentScreen = SCREEN_JOB_SETTING_1;
							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_JOB_SETTING_1;
							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
						}break;
						
						case SCREEN_PERIPHERAL_SETTING:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_5;
							ucSwitchPressFlag = TRUE;
							
							uiCursorXPos = 100;
								uiCursorYPos = 35;
						
							
							ucVCMPeripheralSetCounter = 1;
							
							uiCurrentScreen = SCREEN_VCM_PP_SETTING_1;
							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_VCM_PP_SETTING_1;
							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;

							StatusFlag.stStatusFlag.bPortValuesSet = FALSE;
						}break;
						case SCREEN_MANAGE_PERIPHERAL_1:
						{
							if (FALSE == StatusFlag.stStatusFlag.OPMM_VL_SprayHead_status)
							{
								/*Turn Spray head valve ON*/
								memset(stTx1Message.Data, 0 , sizeof(stTx1Message.Data));
								stTx1Message.DLC = 2;
								stTx1Message.Data[0] = OPMM_VL_SPRAY_HEAD;
								stTx1Message.Data[1] = ON_COMMAND;

								fnCANMessage_Send(CMD_OPMM_VALVE, MSG_TYPE_COMMAND, SCU_ID);
							}
							else if (TRUE == StatusFlag.stStatusFlag.OPMM_VL_SprayHead_status)
							{
								/*Turn Spray Head  valve OFF*/
								memset(stTx1Message.Data, 0 , sizeof(stTx1Message.Data));
								stTx1Message.DLC = 2;
								stTx1Message.Data[0] = OPMM_VL_SPRAY_HEAD;
								stTx1Message.Data[1] = OFF_COMMAND;

								fnCANMessage_Send(CMD_OPMM_VALVE, MSG_TYPE_COMMAND, SCU_ID);
							}
						}break;
						case SCREEN_MANAGE_PERIPHERAL_2:
						{
							if (FALSE == StatusFlag.stStatusFlag.VCM_VL_Stall)
							{
								/*Turn stall valve ON*/
								memset(stTx1Message.Data, 0 , sizeof(stTx1Message.Data));
								stTx1Message.DLC = 2;
								stTx1Message.Data[0] = BIT_0;
								stTx1Message.Data[1] = ON_COMMAND;

								fnCANMessage_Send(CMD_STALL_VALVE, MSG_TYPE_COMMAND, SCU_ID);
							}
							else if (TRUE == StatusFlag.stStatusFlag.VCM_VL_Stall)
							{
								/*Turn stall  valve OFF*/
								memset(stTx1Message.Data, 0 , sizeof(stTx1Message.Data));
								stTx1Message.DLC = 2;
								stTx1Message.Data[0] = BIT_0;
								stTx1Message.Data[1] = OFF_COMMAND;

								fnCANMessage_Send(CMD_STALL_VALVE, MSG_TYPE_COMMAND, SCU_ID);
							}
						}break;
						/*change14*/
						case SCREEN_MANAGE_PERIPHERAL_3:
						{
							if (FALSE == StatusFlag.stStatusFlag.VCM_VL_Recirc)
							{
								/*Turn Recirc valve ON*/
								memset(stTx1Message.Data, 0 , sizeof(stTx1Message.Data));
								stTx1Message.DLC = 2;
								stTx1Message.Data[0] = BIT_1;
								stTx1Message.Data[1] = ON_COMMAND;

								fnCANMessage_Send(CMD_STALL_VALVE, MSG_TYPE_COMMAND, SCU_ID);
							}
							else if (TRUE == StatusFlag.stStatusFlag.VCM_VL_Recirc)
							{
								/*Turn Recirc  valve OFF*/
								memset(stTx1Message.Data, 0 , sizeof(stTx1Message.Data));
								stTx1Message.DLC = 2;
								stTx1Message.Data[0] = BIT_1;
								stTx1Message.Data[1] = OFF_COMMAND;

								fnCANMessage_Send(CMD_STALL_VALVE, MSG_TYPE_COMMAND, SCU_ID);
							}
						}break;
						
						case SCREEN_REMOTE_REPLICA:
						{
							/*change20*/
							memset(stTx1Message.Data, 0 , sizeof(stTx1Message.Data));
							stTx1Message.DLC = 1;
							stTx1Message.Data[0] = BIT_0;  // prime
							fnCANMessage_Send(CMD_REMOTE_BUTTON_PRESS, MSG_TYPE_COMMAND, SCU_ID);
						}break;
						/********/
					}
					ucCurrentSwitchNum = 0;
				}break;
			
			case SWITCH3:
				{
					switch(uiCurrentScreen)
					{
						case SCREEN_MENU_1:
						{
							ucCurrentSwitchNum = 0;					
							/*MENU option*/

							ucScreenPatternNumber = SCREEN_PATTERN_2;
							ucSwitchPressFlag = TRUE;
							
							uiCurrentScreen = SCREEN_CURRENT_PARA;
							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_CURRENT_PARA;
							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
						}break;
						
						case SCREEN_CURRENT_PARA:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_1;
							ucSwitchPressFlag = TRUE;
							
							uiCurrentScreen = SCREEN_PARAM_TIME;
							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_PARAM_TIME;
							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;	
						}break;
						
						case SCREEN_ADVANCED:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_2;
							ucSwitchPressFlag = TRUE;
							
							uiCurrentScreen = SCREEN_MANAGE_PERIPHERAL_1;
							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_MANAGE_PERIPHERAL_1;
							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
						}break;
						
						case SCREEN_STATUS:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_1;
							ucSwitchPressFlag = TRUE;
							
							uiCurrentScreen = SCREEN_CYCLE_STATUS;
							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_CYCLE_STATUS;
							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;	
						}break;
						
						case SCREEN_DIAGNOSTIC_STATUS:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_1;
							ucSwitchPressFlag = TRUE;
							
							uiCurrentScreen = SCREEN_VCM_DIAGNOSTIC;
							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_VCM_DIAGNOSTIC;
							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;	
						}break;
						
						case SCREEN_PARAMETER_SET_1:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_3;
							ucSwitchPressFlag = TRUE;
							
							uiCursorXPos = 66;
							uiCursorYPos = 15;
							
							ucTimerSetVariableCounter = 1;
							
							uiCurrentScreen = SCREEN_TIMER_SETTING_1;
							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_TIMER_SETTING_1  ;
							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
						}break;
						
						case SCREEN_PARAMETER_SET_2:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_3;
							ucSwitchPressFlag = TRUE;
							
							uiCursorXPos = 66;
								uiCursorYPos = 35;
							
							
							ucPresSetVariableCounter = 1;
							
							uiCurrentScreen = SCREEN_PRESSURE_SETTING_1;
							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_PRESSURE_SETTING_1;
							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
						}break;
						
						case SCREEN_PERIPHERAL_SETTING:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_5;
							ucSwitchPressFlag = TRUE;
							
							uiCursorXPos = 100;
								uiCursorYPos = 35;
							
							
							ucOPMMPeripheralSetCounter = 1;
							
							uiCurrentScreen = SCREEN_OPMM_PP_SETTING_1;
							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_OPMM_PP_SETTING_1;
							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
							
							StatusFlag.stStatusFlag.bPortValuesSet = FALSE;
						}break;
						case SCREEN_MANAGE_PERIPHERAL_1:
						{
							if (FALSE == StatusFlag.stStatusFlag.OPMM_VL_Forward_status)
							{
								/*Turn OPMM Forward valve ON*/
								memset(stTx1Message.Data, 0 , sizeof(stTx1Message.Data));
								stTx1Message.DLC = 2;
								stTx1Message.Data[0] = OPMM_VL_FORWARD;
								stTx1Message.Data[1] = ON_COMMAND;

								fnCANMessage_Send(CMD_OPMM_VALVE, MSG_TYPE_COMMAND, SCU_ID);
							}
							else if (TRUE == StatusFlag.stStatusFlag.OPMM_VL_Forward_status)
							{
								/*Turn OPMM Forward  valve OFF*/
								memset(stTx1Message.Data, 0 , sizeof(stTx1Message.Data));
								stTx1Message.DLC = 2;
								stTx1Message.Data[0] = OPMM_VL_FORWARD;
								stTx1Message.Data[1] = OFF_COMMAND;

								fnCANMessage_Send(CMD_OPMM_VALVE, MSG_TYPE_COMMAND, SCU_ID);
							}
						}break;
						case SCREEN_MANAGE_PERIPHERAL_2:
						{
							if (FALSE == StatusFlag.stStatusFlag.bHCM_Antfrz_Heater_Status)
							{
								#ifdef SWITCH_DBG
									DEBUG_PRINTF("Turn Antifreez Heater ON\n\r");
								#endif
								/*Turn ON Antifreez Heater*/
								memset(stTx1Message.Data, 0 , sizeof(stTx1Message.Data));
								stTx1Message.DLC = 2;
								stTx1Message.Data[0] = HCM_ANTIFREEZ_HEATER;
								stTx1Message.Data[1] = ON_COMMAND;

								fnCANMessage_Send(CMD_TURN_ON_OFF_HEATER, MSG_TYPE_COMMAND, SCU_ID);
							}
							else if (TRUE == StatusFlag.stStatusFlag.bHCM_Antfrz_Heater_Status)
							{
								#ifdef SWITCH_DBG
									DEBUG_PRINTF("Turn Antifreez Heater OFF\n\r");
								#endif
								/*Turn OFF Antifreez Heater*/
								memset(stTx1Message.Data, 0 , sizeof(stTx1Message.Data));
								stTx1Message.DLC = 2;
								stTx1Message.Data[0] = HCM_ANTIFREEZ_HEATER;
								stTx1Message.Data[1] = OFF_COMMAND;

								fnCANMessage_Send(CMD_TURN_ON_OFF_HEATER, MSG_TYPE_COMMAND, SCU_ID);
							}
						}break;
						
						case SCREEN_REMOTE_REPLICA:
						{/*change20*/
							memset(stTx1Message.Data, 0 , sizeof(stTx1Message.Data));
							stTx1Message.DLC = 1;
							stTx1Message.Data[0] = BIT_2;  //Spray

							fnCANMessage_Send(CMD_REMOTE_BUTTON_PRESS, MSG_TYPE_COMMAND, SCU_ID);
						}break;
						
						case SCREEN_MENU_2:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_3;
							ucSwitchPressFlag = TRUE;
							
							uiCurrentScreen = SCREEN_FIRMWARE_DETAILS;
							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_FIRMWARE_DETAILS;
							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
						}break;
						/*********/
					}
					ucCurrentSwitchNum = 0;
				}break;
			
			case SWITCH4:
				{
					switch(uiCurrentScreen)
					{
						case SCREEN_MENU_1:
						{
							ucCurrentSwitchNum = 0;					
							/*MENU option*/

							ucScreenPatternNumber = SCREEN_PATTERN_2;
							ucSwitchPressFlag = TRUE;
							
							uiCurrentScreen = SCREEN_ADVANCED;
							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_ADVANCED;
							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
						}break;
						
						case SCREEN_PARAMETER_SET_1:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_3;
							ucSwitchPressFlag = TRUE;
							
							uiCursorXPos = 66;
								uiCursorYPos = 35;
							
							
							ucOtherSetVariableCounter = 1;
							
							uiCurrentScreen = SCREEN_OTHER_SETTING;
							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_OTHER_SETTING;
							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
						}break;
						case SCREEN_MANAGE_PERIPHERAL_1:
						{
							if (FALSE == StatusFlag.stStatusFlag.OPMM_VL_SprayGun_status)
							{
								/*Turn Spray Gun valve ON*/
								memset(stTx1Message.Data, 0 , sizeof(stTx1Message.Data));
								stTx1Message.DLC = 2;
								stTx1Message.Data[0] = OPMM_VL_SPRAY_GUN ;
								stTx1Message.Data[1] = ON_COMMAND;

								fnCANMessage_Send(CMD_OPMM_VALVE, MSG_TYPE_COMMAND, SCU_ID);
							}
							else if (TRUE == StatusFlag.stStatusFlag.OPMM_VL_SprayGun_status)
							{
								/*Turn Spray Gun  valve OFF*/
								memset(stTx1Message.Data, 0 , sizeof(stTx1Message.Data));
								stTx1Message.DLC = 2;
								stTx1Message.Data[0] = OPMM_VL_SPRAY_GUN;
								stTx1Message.Data[1] = OFF_COMMAND;

								fnCANMessage_Send(CMD_OPMM_VALVE, MSG_TYPE_COMMAND, SCU_ID);
							}
						}break;
						case SCREEN_MANAGE_PERIPHERAL_2:
						{
							if (FALSE == StatusFlag.stStatusFlag.bHCM_Hard_Heater_Status)
							{
								#ifdef SWITCH_DBG
									DEBUG_PRINTF("Turn Inline Hardner Heater ON\n\r");
								#endif
								/*Turn ON Inline Hardner Heater*/
								memset(stTx1Message.Data, 0 , sizeof(stTx1Message.Data));
								stTx1Message.DLC = 2;
								stTx1Message.Data[0] = HCM_INLINE_HARD_HEATER;
								stTx1Message.Data[1] = ON_COMMAND;

								fnCANMessage_Send(CMD_TURN_ON_OFF_HEATER, MSG_TYPE_COMMAND, SCU_ID);
							}
							else if (TRUE == StatusFlag.stStatusFlag.bHCM_Hard_Heater_Status)
							{
								#ifdef SWITCH_DBG
									DEBUG_PRINTF("Turn Inline Hardner Heater OFF\n\r");
								#endif
								/*Turn OFF Inline Hardner Heater*/
								memset(stTx1Message.Data, 0 , sizeof(stTx1Message.Data));
								stTx1Message.DLC = 2;
								stTx1Message.Data[0] = HCM_INLINE_HARD_HEATER;
								stTx1Message.Data[1] = OFF_COMMAND;

								fnCANMessage_Send(CMD_TURN_ON_OFF_HEATER, MSG_TYPE_COMMAND, SCU_ID);
							}
						}break;
						case SCREEN_OTHER_SETTING:
						{
							if( 5 == ucOtherSetVariableCounter )
							{
								if(0 < stConfig_Value.ucBuzzerVolume)
								{
									stConfig_Value.ucBuzzerVolume -= 1;
									fnOPConfig_Send(CONFIG_OPMM_BUZZER_VOLUME,0);
									StatusFlag.stStatusFlag.bBuzzerVolDecrease = TRUE;
								}
							}
						}break;
						
						case SCREEN_REMOTE_REPLICA:
						{/*change20*/
							memset(stTx1Message.Data, 0 , sizeof(stTx1Message.Data));
							stTx1Message.DLC = 1;
							stTx1Message.Data[0] = BIT_4;  //Start/Stop

							fnCANMessage_Send(CMD_REMOTE_BUTTON_PRESS, MSG_TYPE_COMMAND, SCU_ID);
						}break;
						/********/
						case SCREEN_DIAGNOSTIC_STATUS:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_1;
							ucSwitchPressFlag = TRUE;
							
							uiCurrentScreen = SCREEN_HMI_DIAGNOSTIC;
							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_HMI_DIAGNOSTIC;
							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;	
						}break;
					/*change3*/
						case SCREEN_MENU_2:
						{
							ucCurrentSwitchNum = 0;					
							/*MENU option*/

							ucScreenPatternNumber = SCREEN_PATTERN_1;
							ucSwitchPressFlag = TRUE;
							/*change3*/
							uiCursorXPos = 66;
							uiCursorYPos = 15;
							ucWIFIIPSetVariableCounter = 1;
							uiCurrentScreen = SCREEN_WIFI_IP_DETAILS;
							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_WIFI_IP_DETAILS;
							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
						}break;
					}	
					ucCurrentSwitchNum = 0;
				}break;
			
			case SWITCH5:
				{
					switch(uiCurrentScreen)
					{
						case SCREEN_VCM_PP_SETTING_1:
						case SCREEN_HOME:
						case SCREEN_MENU_1:
						{
							if( (TRUE == StatusFlag.stStatusFlag.bVCMPortSetFail) || (FALSE == StatusFlag.stStatusFlag.bPortValuesSet) )
							{
								StatusFlag.stStatusFlag.bVCMPortSetFail = FALSE;
								//StatusFlag.stStatusFlag.bPortValuesSet = TRUE;
								
								stConfig_Value.ucConfigConnPressureGuage1 	= stConfig_Value.ucConfigConnPressureGuage1Rev;
								stConfig_Value.ucConfigConnPressureGuage2 	= stConfig_Value.ucConfigConnPressureGuage2Rev;
								stConfig_Value.ucConfigConnPressureGuage3 	= stConfig_Value.ucConfigConnPressureGuage3Rev;
								stConfig_Value.ucConfigConnLinearTransducer = stConfig_Value.ucConfigConnLinearTransducerRev;
								stConfig_Value.ucConfigConnInsideEE07 			= stConfig_Value.ucConfigConnInsideEE07Rev;
								stConfig_Value.ucConfigConnOutsideEE07 			= stConfig_Value.ucConfigConnOutsideEE07Rev;
								stConfig_Value.ucConfigConnStallvalve 			= stConfig_Value.ucConfigConnStallvalveRev;
								/*change15*/
								stConfig_Value.ucConfigConnRecircvalve 			= stConfig_Value.ucConfigConnRecircvalveRev;
								stConfig_Value.uiConfigVCMBypass = stConfig_Value.uiConfigVCMBypassRev;
							}
						}break;
						case SCREEN_HCM_PP_SETTING_1:
						{
							if( (TRUE == StatusFlag.stStatusFlag.bHCMPortSetFail) || (FALSE == StatusFlag.stStatusFlag.bPortValuesSet) )
							{
								StatusFlag.stStatusFlag.bHCMPortSetFail = FALSE;
								
								stConfig_Value.ucConfig_ConnBaseInlineTher 			= stConfig_Value.ucConfig_ConnBaseInlineTherRev;
								stConfig_Value.ucConfig_ConnHardInlineTher 			= stConfig_Value.ucConfig_ConnHardInlineTherRev;
								stConfig_Value.ucConfig_ConnAntifreezeTher 			= stConfig_Value.ucConfig_ConnAntifreezeTherRev;
								stConfig_Value.ucConfig_ConnBasereturnlineTher	= stConfig_Value.ucConfig_ConnBasereturnlineTherRev;
								stConfig_Value.ucConfig_ConnBaseinlineHeater 		= stConfig_Value.ucConfig_ConnBaseinlineHeaterRev;
								stConfig_Value.ucConfig_ConnHardinlineHeater 		= stConfig_Value.ucConfig_ConnHardinlineHeaterRev;
								stConfig_Value.ucConfig_ConnAntifreezeHeater 		= stConfig_Value.ucConfig_ConnAntifreezeHeaterRev;
								stConfig_Value.ucConfig_ConnHardHeater 					= stConfig_Value.ucConfig_ConnHardHeaterRev;
								stConfig_Value.ucConfig_ConnBaseTankLL 					= stConfig_Value.ucConfig_ConnBaseTankLLRev;
								stConfig_Value.ucConfig_ConnHardTankLL 					= stConfig_Value.ucConfig_ConnHardTankLLRev;
								
								stConfig_Value.uiConfigHCMBypass = stConfig_Value.uiConfigHCMBypassRev;
							}
						}break;
						case SCREEN_OPMM_PP_SETTING_1:
						case SCREEN_OPMM_PP_SETTING_2:
						case SCREEN_OPMM_PP_SETTING_3:
						{
							if( (TRUE == StatusFlag.stStatusFlag.bOPMMPortSetFail) || (FALSE == StatusFlag.stStatusFlag.bPortValuesSet) )
							{
								StatusFlag.stStatusFlag.bOPMMPortSetFail = FALSE;
								
								stConfig_Value.ucConfigConnSprayvalve 		= stConfig_Value.ucConfigConnSprayvalveRev;
								stConfig_Value.ucConfigConnGunposition 		= stConfig_Value.ucConfigConnGunpositionRev;
								stConfig_Value.ucConfigConnABvalve 				= stConfig_Value.ucConfigConnABvalveRev;
								stConfig_Value.ucConfigConnSolventvalve 	= stConfig_Value.ucConfigConnSolventvalveRev;
								stConfig_Value.ucConfigConnForwardvalve 	= stConfig_Value.ucConfigConnForwardvalveRev;
								stConfig_Value.ucConfigConnBackwardvalve	= stConfig_Value.ucConfigConnBackwardvalveRev;
								stConfig_Value.ucConfigNCTempSensor1 			= stConfig_Value.ucConfigNCTempSensor1Rev;
								stConfig_Value.ucConfigNCTempSensor2 			= stConfig_Value.ucConfigNCTempSensor2Rev;
								stConfig_Value.ucConfigNCTempSensor3 			= stConfig_Value.ucConfigNCTempSensor3Rev;
								stConfig_Value.ucConfigNCTempSensor4 			= stConfig_Value.ucConfigNCTempSensor4Rev;
								
								stConfig_Value.uiConfigOPMMBypass = stConfig_Value.uiConfigOPMMBypassRev;
							}
						}
						default: 
							break;
					}
					/*Back Button*/
					ucSwitchPressFlag = TRUE;
					if( ( 0 < ucCurrentScreenNumber ) && ( 0 < ucPatternNumberCount ) )
					{
						if ( 0 != ucCurrentScreenNumber )
						{
							uiCurrentScreen = uirgStoreCurrentScreen[--ucCurrentScreenNumber];
							ucScreenPatternNumber = ucrgStoreCurrentScreenPattern[--ucPatternNumberCount];
							
							switch(uiCurrentScreen)
							{
								case SCREEN_VCM_PP_SETTING_1:
								case SCREEN_OPMM_PP_SETTING_1:
								case SCREEN_SCU_PP_SETTING:
								case SCREEN_HCM_PP_SETTING_1:
										/*change3*/
								//case SCREEN_ETH_IP_DETAILS:
						//		uiCursorXPos = 100;
									break;
								default:
						//			uiCursorXPos = 66;
									break;
							}
							switch(uiCurrentScreen)
							{
								case	SCREEN_PARAMETER_SET_1:
								case	SCREEN_PARAMETER_SET_2:
								case	SCREEN_TIMER_SETTING_4:
								case	SCREEN_TIMER_SETTING_2:
								case	SCREEN_STALL_SETTING_1:
								case	SCREEN_LIQUID_LEVEL_1:
								case	SCREEN_TIMER_SETTING_1:
								case	SCREEN_TIMER_SETTING_3:
								case	SCREEN_OTHER_SETTING:
								case	SCREEN_STALL_SETTING:
										/*change3*/
								case SCREEN_ETH_IP_DETAILS:
								case SCREEN_WIFI_IP_DETAILS:
								case SCREEN_PUMP_SETTING: 	
									uiCursorYPos = 15;
									break;
								default:
									uiCursorYPos = 35;
									break;
							}
						}
					}
					ucCurrentSwitchNum = 0;					
				}break;
			
			case SWITCH6:
				{
				switch(uiCurrentScreen)
				{
					case	SCREEN_KEYPAD:
					{
					ucpDisplayLetter = ucAlphaNum[LETTER_POS_X(uiCursorXPosKP)][LETTER_POS_Y(uiCursorYPosKP)][0];
						if ( ucpDisplayLetter == ucAlphaNum[2][7][0] )
						{
							uiLetter -= LETTER_WIDTH;
							
							uiLetterStartPos = DISP_LETTER_START_POS - uiLetter;
							if ( uiLetterStartPos > DISP_LETTER_START_POS )
							{
								uiLetter = 0;
								uiLetterStartPos = DISP_LETTER_START_POS;
							}
							ulirgStoreLetter[--ucLetterPos] = '\0';
							
							fnGr_PutString(86,320 , "                             ", RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);		
							fnGr_PutString(85,320 , ulirgStoreLetter, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
							#ifdef JOYSTICK_DBG
								DEBUG_PRINTF("Current Stored String After Deleting :%s \r\n",ulirgStoreLetter);								
							#endif
						}
						else
						{
							uiLetterStartPos = DISP_LETTER_START_POS - uiLetter;
							if(uiLetterStartPos < 25)
							{
								uiLetterStartPos = 10;
							}else
							{								
								if ( ucpDisplayLetter == ucAlphaNum[0][1][0] )
								{
									uiLetter += (LETTER_WIDTH + 6);
								}else if ( ucpDisplayLetter == ucAlphaNum[0][7][0] )
								{
									uiLetter += (LETTER_WIDTH - 5);
								}
								else
								{
									uiLetter += LETTER_WIDTH;
								}
								ulirgStoreLetter[ucLetterPos++] = ucpDisplayLetter;
								fnGr_PutString(85,320 , ulirgStoreLetter, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
								#ifdef JOYSTICK_DBG
									DEBUG_PRINTF("Current Stored String :%s \r\n",ulirgStoreLetter);
								#endif
								}
						}
					}break;
				
					case SCREEN_LIQUID_LEVEL_1:
					case SCREEN_LIQUID_LEVEL_2:
					case SCREEN_TEMP_SETTING_1:
					case SCREEN_TEMP_SETTING_2:
					case SCREEN_TIMER_SETTING_1:
					case SCREEN_TIMER_SETTING_2:
					case SCREEN_TIMER_SETTING_3:
					case SCREEN_TIMER_SETTING_4:
					case SCREEN_PUMP_SETTING:
					case SCREEN_HYSTERESIS_1:
					case SCREEN_HYSTERESIS_2:
					case SCREEN_STALL_SETTING:
					case SCREEN_STALL_SETTING_1:
					case SCREEN_JOB_SETTING_1:
					case SCREEN_JOB_SETTING_2:
					case SCREEN_PRESSURE_SETTING_1:
					case SCREEN_PRESSURE_SETTING_2:
					case SCREEN_VCM_PP_SETTING_1:
					case SCREEN_VCM_PP_SETTING_2:
					case SCREEN_HCM_PP_SETTING_1:
					case SCREEN_HCM_PP_SETTING_2:
					case SCREEN_OPMM_PP_SETTING_1:
					case SCREEN_OPMM_PP_SETTING_2:
						/*change3*/
					case SCREEN_ETH_IP_DETAILS:	
					case SCREEN_WIFI_IP_DETAILS:
					{
						if (( 3 == ucVCMPeripheralSetCounter ) && ( SCREEN_VCM_PP_SETTING_2 == uiCurrentScreen))
						{
							/*don not nevigate to keypad screen*/
						}else
						{
							StatusFlag.stStatusFlag.bParameterSet = TRUE;

							ucScreenPatternNumber = SCREEN_PATTERN_11;
							ucSwitchPressFlag = TRUE;
							
							uiCurrentScreen = SCREEN_KEYPAD;
								
							if ( SCREEN_KEYPAD != uirgStoreCurrentScreen[ucCurrentScreenNumber - 1] )
							{
								uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_KEYPAD;
								ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
							}
						}
					}break;
					case SCREEN_ADVANCED_RESET:
					{
						if( 1 == ucResetVariableCounter )
						{
							/*Reset no of joints*/
							#ifdef JOYSTICK_DBG
								DEBUG_PRINTF("Reset Number of joints \n\r");
							#endif
							
							stConfig_Value.fConfigJointNum = 0;
							fnOPConfig_Send(CONFIG_JOINT_NUMBER,0);
						}
						else if ( 2 == ucResetVariableCounter )
						{
							/*reset base volume*/
							#ifdef JOYSTICK_DBG
								DEBUG_PRINTF("Reset Base Volume \n\r");
							#endif
							stCurrent_Value.fBaseTotalMaterialUsed = 0.0f;
							
							stCurrent_Value.fBaseMaterialPrimed = 0.0f;
							
							stCurrent_Value.fBaseMaterialApplied = 0.0f;
							
						}
						else if ( 3 == ucResetVariableCounter )
						{
							/*reset hardner volume*/
							#ifdef JOYSTICK_DBG
								DEBUG_PRINTF("Reset Hardner Volume \n\r");
							#endif
							stCurrent_Value.fHardTotalMaterialUsed = 0;
							
							stCurrent_Value.fHardMaterialPrimed = 0.0f;
							stCurrent_Value.fHardMaterialApplied = 0.0f;
						}
					}break;
					case SCREEN_OTHER_SETTING:
					{
						if ( (1 != ucOtherSetVariableCounter ) && ( 2 != ucOtherSetVariableCounter ) )
						{
			
							if ( 3 == ucOtherSetVariableCounter )
							{
								if(IMPERIAL == stConfig_Value.ucTypeofUnit)
								{
								stConfig_Value.ucTypeofUnit = METRIC;
								fnOPConfig_Send(CONFIG_TYPE_OF_UNIT,METRIC);
								fnConvertConfigToMatric();
								}
							}
							else if( 4 == ucOtherSetVariableCounter )
							{
								if(METRIC == stConfig_Value.ucTypeofUnit)
								{
								stConfig_Value.ucTypeofUnit = IMPERIAL;
								fnOPConfig_Send(CONFIG_TYPE_OF_UNIT,IMPERIAL);
								fnConvertConfigToImperial();
								}
							}
							break;
						}
						
						if ( ( 1 == ucOtherSetVariableCounter ) || ( 2 == ucOtherSetVariableCounter ))
						{
							ucScreenPatternNumber = SCREEN_PATTERN_11;
							ucSwitchPressFlag = TRUE;
							StatusFlag.stStatusFlag.bParameterSet = TRUE;
							
							uiCurrentScreen = SCREEN_KEYPAD;
								
							if ( SCREEN_KEYPAD != uirgStoreCurrentScreen[ucCurrentScreenNumber - 1] )
							{
								uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_KEYPAD;
								ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
							}
						}
					}break;
					case SCREEN_OTHER_SETTING_1:
					{
						ucScreenPatternNumber = SCREEN_PATTERN_11;
						ucSwitchPressFlag = TRUE;
						StatusFlag.stStatusFlag.bParameterSet = TRUE;
						
						uiCurrentScreen = SCREEN_KEYPAD;
							
						if ( SCREEN_KEYPAD != uirgStoreCurrentScreen[ucCurrentScreenNumber - 1] )
						{
							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_KEYPAD;
							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
						}
					}break;
					default:
					{
					}break;
				}					
					ucCurrentSwitchNum = 0;
				}break;
			
			case SWITCH7:
				{
					switch(uiCurrentScreen)
					{
						case SCREEN_PARAMETER_SET_1:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_3;
							ucSwitchPressFlag = TRUE;
							
							uiCursorXPos = 66;
								uiCursorYPos = 35;
						
							
							ucHystSetVariableCounter = 1;
							
							uiCurrentScreen = SCREEN_HYSTERESIS_1;
							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_HYSTERESIS_1;
							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
						}break;
						case SCREEN_VCM_PP_SETTING_1:
						case SCREEN_VCM_PP_SETTING_2:
						{
							if( FALSE == StatusFlag.stStatusFlag.bVCMPortSetFail)
							{
								StatusFlag.stStatusFlag.bPortValuesSet = TRUE;
								
								fnOPConfig_Send(CONFIG_VCM_PERIPHRAL_BYPASS, 0);
								fnOPConfig_Send(CONFIG_PGAUGE_REMAP, 0);
								fnOPConfig_Send(CONFIG_LT_REMAP, 0);
								fnOPConfig_Send(CONFIG_EE07_REMAP, 0);
								fnOPConfig_Send(CONFIG_STALL_VALVE_REMAP, 0);
								
								stConfig_Value.ucConfigConnPressureGuage1Rev 		= stConfig_Value.ucConfigConnPressureGuage1;
								stConfig_Value.ucConfigConnPressureGuage2Rev 		= stConfig_Value.ucConfigConnPressureGuage2;
								stConfig_Value.ucConfigConnPressureGuage3Rev 		= stConfig_Value.ucConfigConnPressureGuage3;
								stConfig_Value.ucConfigConnLinearTransducerRev 	= stConfig_Value.ucConfigConnLinearTransducer;
								stConfig_Value.ucConfigConnInsideEE07Rev 				= stConfig_Value.ucConfigConnInsideEE07;
								stConfig_Value.ucConfigConnOutsideEE07Rev 			= stConfig_Value.ucConfigConnOutsideEE07;
								stConfig_Value.ucConfigConnStallvalveRev 				= stConfig_Value.ucConfigConnStallvalve;
								/*change12*/
								stConfig_Value.ucConfigConnRecircvalveRev 				= stConfig_Value.ucConfigConnRecircvalve;
								stConfig_Value.uiConfigVCMBypassRev = stConfig_Value.uiConfigVCMBypass;
							}
						}break;
						case SCREEN_HCM_PP_SETTING_1:
						case SCREEN_HCM_PP_SETTING_2:
						{
							if( FALSE == StatusFlag.stStatusFlag.bHCMPortSetFail)
							{
								StatusFlag.stStatusFlag.bPortValuesSet = TRUE;
								
								fnOPConfig_Send(CONFIG_HCM_PERIPHRAL_BYPASS, 0);
								fnOPConfig_Send(CONFIG_THERMOCOUPLE_REMAP, 0);
								fnOPConfig_Send(CONFIG_SSR_REMAP, 0);
								fnOPConfig_Send(CONFIG_LL_SENSOR_REMAP, 0);
								
								stConfig_Value.ucConfig_ConnBaseInlineTherRev 		= stConfig_Value.ucConfig_ConnBaseInlineTher;
								stConfig_Value.ucConfig_ConnHardInlineTherRev 		= stConfig_Value.ucConfig_ConnHardInlineTher;
								stConfig_Value.ucConfig_ConnAntifreezeTherRev 		= stConfig_Value.ucConfig_ConnAntifreezeTher;
								stConfig_Value.ucConfig_ConnBasereturnlineTherRev = stConfig_Value.ucConfig_ConnBasereturnlineTher;
								stConfig_Value.ucConfig_ConnBaseinlineHeaterRev 	= stConfig_Value.ucConfig_ConnBaseinlineHeater;
								stConfig_Value.ucConfig_ConnHardinlineHeaterRev 	= stConfig_Value.ucConfig_ConnHardinlineHeater;
								stConfig_Value.ucConfig_ConnAntifreezeHeaterRev 	= stConfig_Value.ucConfig_ConnAntifreezeHeater;
								stConfig_Value.ucConfig_ConnHardHeaterRev 				= stConfig_Value.ucConfig_ConnHardHeater;
								stConfig_Value.ucConfig_ConnBaseTankLLRev 				= stConfig_Value.ucConfig_ConnBaseTankLL;
								stConfig_Value.ucConfig_ConnHardTankLLRev 				= stConfig_Value.ucConfig_ConnHardTankLL;
								
								stConfig_Value.uiConfigHCMBypassRev = stConfig_Value.uiConfigHCMBypass;
							}
						}break;
						case SCREEN_OPMM_PP_SETTING_1:
						case SCREEN_OPMM_PP_SETTING_2:
						case SCREEN_OPMM_PP_SETTING_3:	
						{
							if( FALSE == StatusFlag.stStatusFlag.bOPMMPortSetFail)
							{
								StatusFlag.stStatusFlag.bPortValuesSet = TRUE;
								
								fnOPConfig_Send(CONFIG_OPMM_PERIPHRAL_BYPASS, 0);
								fnOPConfig_Send(CONFIG_OPMM_VALVE_REMAP, 0);
								fnOPConfig_Send(CONFIG_NON_CONTACT_TEMP_REMAP, 0);
								
								stConfig_Value.ucConfigConnSprayvalveRev 		= stConfig_Value.ucConfigConnSprayvalve;
								stConfig_Value.ucConfigConnGunpositionRev 	= stConfig_Value.ucConfigConnGunposition;
								stConfig_Value.ucConfigConnABvalveRev 			= stConfig_Value.ucConfigConnABvalve;
								stConfig_Value.ucConfigConnSolventvalveRev 	= stConfig_Value.ucConfigConnSolventvalve;
								stConfig_Value.ucConfigConnForwardvalveRev 	= stConfig_Value.ucConfigConnForwardvalve;
								stConfig_Value.ucConfigConnBackwardvalveRev = stConfig_Value.ucConfigConnBackwardvalve;
								stConfig_Value.ucConfigNCTempSensor1Rev 		= stConfig_Value.ucConfigNCTempSensor1;
								stConfig_Value.ucConfigNCTempSensor2Rev 		= stConfig_Value.ucConfigNCTempSensor2;
								stConfig_Value.ucConfigNCTempSensor3Rev 		= stConfig_Value.ucConfigNCTempSensor3;
								stConfig_Value.ucConfigNCTempSensor4Rev 		= stConfig_Value.ucConfigNCTempSensor4;
								
								stConfig_Value.uiConfigOPMMBypassRev = stConfig_Value.uiConfigOPMMBypass;
							}
						}break;
						case SCREEN_CLEAR_FAULTS:
						{
							/*clear warning function call*/
							fnClearWarnings();
							
							ucScreenPatternNumber = SCREEN_PATTERN_7;
							ucSwitchPressFlag = TRUE;
							
							uiCurrentScreen = SCREEN_CLEAR_FAULTS;
							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_CLEAR_FAULTS;
							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
						}break;
						case SCREEN_MANAGE_PERIPHERAL_1:
						{
							if (FALSE == StatusFlag.stStatusFlag.OPMM_VL_Flush_status)
							{
								/*Turn Flush Valve valve ON*/
								memset(stTx1Message.Data, 0 , sizeof(stTx1Message.Data));
								stTx1Message.DLC = 2;
								stTx1Message.Data[0] = OPMM_VL_FLUSH;
								stTx1Message.Data[1] = ON_COMMAND;

								fnCANMessage_Send(CMD_OPMM_VALVE, MSG_TYPE_COMMAND, SCU_ID);
							}
							else if (TRUE == StatusFlag.stStatusFlag.OPMM_VL_Flush_status)
							{
								/*Turn Flush Valve  valve OFF*/
								memset(stTx1Message.Data, 0 , sizeof(stTx1Message.Data));
								stTx1Message.DLC = 2;
								stTx1Message.Data[0] = OPMM_VL_FLUSH;
								stTx1Message.Data[1] = OFF_COMMAND;

								fnCANMessage_Send(CMD_OPMM_VALVE, MSG_TYPE_COMMAND, SCU_ID);
							}
						}break;
						case SCREEN_MANAGE_PERIPHERAL_2:
						{
							if (FALSE == StatusFlag.stStatusFlag.bHCM_Hard_Tank_Heater_Status)
							{
								#ifdef SWITCH_DBG
									DEBUG_PRINTF("Turn Hard Tank Heater ON\n\r");
								#endif
								/*Turn ON Hard Tankv Heater*/
								memset(stTx1Message.Data, 0 , sizeof(stTx1Message.Data));
								stTx1Message.DLC = 2;
								stTx1Message.Data[0] = HCM_HARD_TANK_HEATER;
								stTx1Message.Data[1] = ON_COMMAND;

								fnCANMessage_Send(CMD_TURN_ON_OFF_HEATER, MSG_TYPE_COMMAND, SCU_ID);
							}
							else if (TRUE == StatusFlag.stStatusFlag.bHCM_Hard_Tank_Heater_Status)
							{
								#ifdef SWITCH_DBG
									DEBUG_PRINTF("Turn Hard Tank Heater OFF\n\r");
								#endif
								/*Turn OFF Hard Tank Heater*/
								memset(stTx1Message.Data, 0 , sizeof(stTx1Message.Data));
								stTx1Message.DLC = 2;
								stTx1Message.Data[0] = HCM_HARD_TANK_HEATER;
								stTx1Message.Data[1] = OFF_COMMAND;

								fnCANMessage_Send(CMD_TURN_ON_OFF_HEATER, MSG_TYPE_COMMAND, SCU_ID);
							}
						}break;
						case SCREEN_SCU_PP_SETTING:
						{
							fnOPConfig_Send(CONFIG_SCU_PERIPHRAL_BYPASS, 0);
							fnOPConfig_Send(CONFIG_TEMP_PRES_ACHIEVE_BYPASS, 0);
						}break;
						case SCREEN_TIMER_SETTING_4:
						{
							fnOPConfig_Send(CONFIG_SYS_DATE_TIME, 0);
						}break;
						case SCREEN_MENU_1:
						{
							ucCurrentSwitchNum = 0;					
							/*MENU option*/

							ucScreenPatternNumber = SCREEN_PATTERN_6;
							ucSwitchPressFlag = TRUE;
							
							uiCurrentScreen = SCREEN_NOTIFICATION_1;
							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_NOTIFICATION_1;
							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
						}break;
						case SCREEN_OTHER_SETTING:
						{
							if( 5 == ucOtherSetVariableCounter )
							{
								if(10 > stConfig_Value.ucBuzzerVolume)
								{
									stConfig_Value.ucBuzzerVolume += 1;
									fnOPConfig_Send(CONFIG_OPMM_BUZZER_VOLUME,0);
								}
							}
						}break;
						
						case SCREEN_REMOTE_REPLICA:
						{  /*change20*/
							memset(stTx1Message.Data, 0 , sizeof(stTx1Message.Data));
							stTx1Message.DLC = 1;
							stTx1Message.Data[0] = BIT_5; // Flush Complete

							fnCANMessage_Send(CMD_REMOTE_BUTTON_PRESS, MSG_TYPE_COMMAND, SCU_ID);
						}break;
						
						case SCREEN_ADVANCED_RESET:
						{
							stCurrent_Value.fBaseMatTotalJoint = RESET;
							
						stCurrent_Value.fHardMatTotalJoint = RESET;
					
						stCurrent_Value.fBaseMatPrimeAcc = RESET;
					
						stCurrent_Value.fHardMatPrimeAcc = RESET;
					
						stCurrent_Value.fBaseMatSprayAcc = RESET;
					
						stCurrent_Value.fHardMatSprayAcc = RESET;
					
						stCurrent_Value.fBaseMatTotalAcc = RESET;
					
					
						stCurrent_Value.fHardMatTotalAcc = RESET;
					
							stConfig_Value.fConfigJointNum = 0;
							fnOPConfig_Send(CONFIG_JOINT_NUMBER,0);
							
							stTx1Message.DLC = 0;
							/*Send the CONFIG_OPMM_GUN_DELAY to MCU and OP */
							fnCANMessage_Send(CMD_JOINT_MATERIAL_RESET, MSG_TYPE_COMMAND, SCU_ID);
						}break;
						/*change8*/
						case SCREEN_WIFI_IP_DETAILS:
						{
							stTx1Message.DLC = 0;
							/*Send the CONFIG_OPMM_GUN_DELAY to MCU and OP */
							fnCANMessage_Send(COMMAND_APPLY_WIFI_SETTING, MSG_TYPE_COMMAND, SCU_ID);
						}break;
						
						/*change4*/
						case SCREEN_HOME:
						{
							ucCurrentSwitchNum = 0;					
							/*MENU option*/

							ucScreenPatternNumber = SCREEN_PATTERN_6;
							ucSwitchPressFlag = TRUE;
							
							uiCurrentScreen = SCREEN_MEMORY_ALARM_1;
							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_MEMORY_ALARM_1;
							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
						}break;
						/*change4*/
						case SCREEN_MEMORY_ALARM_1:
						case SCREEN_MEMORY_ALARM_2:
						case SCREEN_MEMORY_ALARM_3:
						case SCREEN_MEMORY_ALARM_4:
								{
									fnMemory_Alarm_Message_Clear();
									if(RESET == ucAlarmCountLimit)
									{
										StatusFlag.stStatusFlag.bAnyFault = FALSE;
										/* ON Alarm 1 Led GREEN */
		fnLED_TurnONOFF (PORT_LED_ERR_GREEN, Bit_SET,PIN_LED_ERR_GREEN);
		fnLED_TurnONOFF (PORT_LED_ERR_RED, Bit_RESET,PIN_LED_ERR_RED);
		
		fnLED_TurnONOFF (PORT_LED_COM_GREEN, Bit_SET,PIN_LED_COM_GREEN);
		fnLED_TurnONOFF (PORT_LED_COM_RED, Bit_RESET,PIN_LED_COM_RED);
									}
									ucSwitchPressFlag = TRUE;
								stTx1Message.DLC = 1;
							stTx1Message.Data[0] = ACK_FOR_MEMORY_FULL;
							/*Send the CONFIG_OPMM_GUN_DELAY to MCU and OP */
							fnCANMessage_Send(FAULT_LOG_MEMORY_FULL_ERROR, MSG_TYPE_FAULT, SCU_ID);
						}break;
					}					
					ucCurrentSwitchNum = 0;
				}break;
			
			case SWITCH8:
				{
					switch(uiCurrentScreen)
					{
						case SCREEN_CURRENT_PARA:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_1;
							ucSwitchPressFlag = TRUE;
							
							uiCurrentScreen = SCREEN_PARAM_OTHERS;
							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_PARAM_OTHERS;
							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;	
						}break;
						
						case SCREEN_STATUS:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_2;
							ucSwitchPressFlag = TRUE;
							
							uiCurrentScreen = SCREEN_DIAGNOSTIC_STATUS;
							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_DIAGNOSTIC_STATUS;
							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;	
						}break;
						
						case SCREEN_DIAGNOSTIC_STATUS:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_1;
							ucSwitchPressFlag = TRUE;
							
							uiCurrentScreen = SCREEN_HCM_DIAGNOSTIC;
							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_HCM_DIAGNOSTIC;
							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;	
						}break;
						case SCREEN_PARAMETER_SET_1:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_3;
							ucSwitchPressFlag = TRUE;
							
							uiCursorXPos = 66;
							uiCursorYPos = 15;
							
							ucPumpSetVariableCounter = 1;
							
							uiCurrentScreen = SCREEN_PUMP_SETTING;
							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_PUMP_SETTING;
							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
						}break;
						case SCREEN_VCM_PP_SETTING_1:
						{
							if ( 1 == ucVCMPeripheralSetCounter )
							{
								stConfig_Value.uiConfigVCMBypass |= BIT_1;
							}
							else if ( 2 == ucVCMPeripheralSetCounter )
							{
								stConfig_Value.uiConfigVCMBypass |= BIT_2;
							}
							else	if ( 3 == ucVCMPeripheralSetCounter )
							{
								stConfig_Value.uiConfigVCMBypass |= BIT_3;
							}
							else if ( 4 == ucVCMPeripheralSetCounter )
							{
								stConfig_Value.uiConfigVCMBypass |= BIT_5;
							}
							else if ( 5 == ucVCMPeripheralSetCounter )
							{
								stConfig_Value.uiConfigVCMBypass |= BIT_6;
							}
						}break;
						case SCREEN_VCM_PP_SETTING_2:
						{
							if ( 1 == ucVCMPeripheralSetCounter )
							{
								stConfig_Value.uiConfigVCMBypass |= BIT_4;
							}
							else if ( 2 == ucVCMPeripheralSetCounter )
							{
								stConfig_Value.uiConfigVCMBypass |= BIT_0;
							}
							else	if ( 4 == ucVCMPeripheralSetCounter )
							{
								stConfig_Value.uiConfigVCMBypass |= BIT_7;
							}
						}break;
						case SCREEN_HCM_PP_SETTING_1:
						{
							if ( 1 == ucHCMPeripheralSetCounter )
							{
								stConfig_Value.uiConfigHCMBypass |= BIT_0;
							}
							else if ( 2 == ucHCMPeripheralSetCounter )
							{
								stConfig_Value.uiConfigHCMBypass |= BIT_1;
							}
							else	if ( 3 == ucHCMPeripheralSetCounter )
							{
								stConfig_Value.uiConfigHCMBypass |= BIT_2;
							}
							else if ( 4 == ucHCMPeripheralSetCounter )
							{
								stConfig_Value.uiConfigHCMBypass |= BIT_3;
							}
							else if ( 5 == ucHCMPeripheralSetCounter )
							{
								stConfig_Value.uiConfigHCMBypass |= BIT_4;
							}
						}break;
						case SCREEN_HCM_PP_SETTING_2:
						{
							if ( 1 == ucHCMPeripheralSetCounter )
							{
								stConfig_Value.uiConfigHCMBypass |= BIT_5;
							}
							else if ( 2 == ucHCMPeripheralSetCounter )
							{
								stConfig_Value.uiConfigHCMBypass |= BIT_6;
							}
							else	if ( 3 == ucHCMPeripheralSetCounter )
							{
								stConfig_Value.uiConfigHCMBypass |= BIT_7;
							}
							else	if ( 4 == ucHCMPeripheralSetCounter )
							{
								stConfig_Value.uiConfigHCMBypass |= BIT_8;
							}
							else	if ( 5 == ucHCMPeripheralSetCounter )
							{
								stConfig_Value.uiConfigHCMBypass |= BIT_9;
							}
						}break;
						case SCREEN_OPMM_PP_SETTING_1:
						{
							if ( 1 == ucOPMMPeripheralSetCounter )
							{
								stConfig_Value.uiConfigOPMMBypass |= BIT_0;
							}
							else if ( 2 == ucOPMMPeripheralSetCounter )
							{
								stConfig_Value.uiConfigOPMMBypass |= BIT_1;
							}
							else	if ( 3 == ucOPMMPeripheralSetCounter )
							{
								stConfig_Value.uiConfigOPMMBypass |= BIT_2;
							}
							else if ( 4 == ucOPMMPeripheralSetCounter )
							{
								stConfig_Value.uiConfigOPMMBypass |= BIT_3;
							}
							else if ( 5 == ucOPMMPeripheralSetCounter )
							{
								stConfig_Value.uiConfigOPMMBypass |= BIT_4;
							}
						}break;
						case SCREEN_OPMM_PP_SETTING_2:
						{
							if ( 1 == ucOPMMPeripheralSetCounter )
							{
								stConfig_Value.uiConfigOPMMBypass |= BIT_5;
							}
							else if ( 2 == ucOPMMPeripheralSetCounter )
							{
								stConfig_Value.uiConfigOPMMBypass |= BIT_8;
							}
							else	if ( 3 == ucOPMMPeripheralSetCounter )
							{
								stConfig_Value.uiConfigOPMMBypass |= BIT_9;
							}
							else	if ( 4 == ucOPMMPeripheralSetCounter )
							{
								stConfig_Value.uiConfigOPMMBypass |= BIT_10;
							}
							else	if ( 5 == ucOPMMPeripheralSetCounter )
							{
								stConfig_Value.uiConfigOPMMBypass |= BIT_11;
							}
						}break;
						case SCREEN_OPMM_PP_SETTING_3:
						{
							if ( 1 == ucOPMMPeripheralSetCounter )
							{
								stConfig_Value.uiConfigOPMMBypass |= BIT_12;
							}
							else if ( 2 == ucOPMMPeripheralSetCounter )
							{
								stConfig_Value.uiConfigOPMMBypass |= BIT_13;
							}
						}break;
						case SCREEN_SCU_PP_SETTING:
						{
							if ( 1 == ucSCUPeripheralSetCounter )
							{
								stConfig_Value.uiConfigSCUBypass |= BIT_0;
							}
							else if ( 2 == ucSCUPeripheralSetCounter )
							{
								stConfig_Value.uiConfigSCUBypass |= BIT_1;
							}
							else	if ( 3 == ucSCUPeripheralSetCounter )
							{
								stConfig_Value.uiConfigSCUBypass |= BIT_2;
							}
							else if ( 4 == ucSCUPeripheralSetCounter )
							{
								StatusFlag.stStatusFlag.bSCU_Temp_Bypass = TRUE;
							}
							else if ( 5 == ucSCUPeripheralSetCounter )
							{
								StatusFlag.stStatusFlag.bSCU_Pressure_Bypass = TRUE;
							}
						}break;
						case SCREEN_HOME:
						{
							ucCurrentSwitchNum = 0;					
							/*MENU option*/

							ucScreenPatternNumber = SCREEN_PATTERN_6;
							ucSwitchPressFlag = TRUE;
							
							uiCurrentScreen = SCREEN_ALARMS_1;
							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_ALARMS_1;
							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
						}break;
						case SCREEN_MENU_1:
						{
							ucCurrentSwitchNum = 0;					
							/*MENU option*/

							ucScreenPatternNumber = SCREEN_PATTERN_6;
							ucSwitchPressFlag = TRUE;
							
							uiCurrentScreen = SCREEN_ALARMS_1;
							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_ALARMS_1;
							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
						}break;
						case SCREEN_CLEAR_FAULTS:
						{
							/*clear error function call*/
							fnClearErrors();
							
							ucScreenPatternNumber = SCREEN_PATTERN_7;
							ucSwitchPressFlag = TRUE;
							
							uiCurrentScreen = SCREEN_CLEAR_FAULTS;
							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_CLEAR_FAULTS;
							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
						}break;
						case SCREEN_MANAGE_PERIPHERAL_1:
						{
							if (FALSE == StatusFlag.stStatusFlag.OPMM_VL_Backward_status)
							{
								/*Turn Backward valve ON*/
								memset(stTx1Message.Data, 0 , sizeof(stTx1Message.Data));
								stTx1Message.DLC = 2;
								stTx1Message.Data[0] = OPMM_VL_BACKWARD;
								stTx1Message.Data[1] = ON_COMMAND;

								fnCANMessage_Send(CMD_OPMM_VALVE, MSG_TYPE_COMMAND, SCU_ID);
							}
							else if (TRUE == StatusFlag.stStatusFlag.OPMM_VL_Backward_status)
							{
								/*Turn Backward  valve OFF*/
								memset(stTx1Message.Data, 0 , sizeof(stTx1Message.Data));
								stTx1Message.DLC = 2;
								stTx1Message.Data[0] = OPMM_VL_BACKWARD;
								stTx1Message.Data[1] = OFF_COMMAND;

								fnCANMessage_Send(CMD_OPMM_VALVE, MSG_TYPE_COMMAND, SCU_ID);
							}
						}break;
						case SCREEN_MANAGE_PERIPHERAL_2:
						{
							if (FALSE == StatusFlag.stStatusFlag.bHCM_Base_Heater_Status)
							{
								#ifdef SWITCH_DBG
									DEBUG_PRINTF("Turn Hard Tank Heater ON\n\r");
								#endif
								/*Turn ON Hard Tankv Heater*/
								memset(stTx1Message.Data, 0 , sizeof(stTx1Message.Data));
								stTx1Message.DLC = 2;
								stTx1Message.Data[0] = HCM_INLINE_BASE_HEATER;
								stTx1Message.Data[1] = ON_COMMAND;

								fnCANMessage_Send(CMD_TURN_ON_OFF_HEATER, MSG_TYPE_COMMAND, SCU_ID);
							}
							else if (TRUE == StatusFlag.stStatusFlag.bHCM_Base_Heater_Status)
							{
								#ifdef SWITCH_DBG
									DEBUG_PRINTF("Turn Hard Tank Heater OFF\n\r");
								#endif
								/*Turn OFF Hard Tank Heater*/
								memset(stTx1Message.Data, 0 , sizeof(stTx1Message.Data));
								stTx1Message.DLC = 2;
								stTx1Message.Data[0] = HCM_INLINE_BASE_HEATER;
								stTx1Message.Data[1] = OFF_COMMAND;

								fnCANMessage_Send(CMD_TURN_ON_OFF_HEATER, MSG_TYPE_COMMAND, SCU_ID);
							}
						}break;
						case SCREEN_PERIPHERAL_SETTING:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_5;
							ucSwitchPressFlag = TRUE;
							
							uiCursorXPos = 100;
						uiCursorYPos = 35;
						
							
							ucSCUPeripheralSetCounter = 1;
							
							uiCurrentScreen = SCREEN_SCU_PP_SETTING;
							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_SCU_PP_SETTING;
							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
						}break;
						
						case SCREEN_ADVANCED:
						{
							ucCurrentSwitchNum = 0;					
							/*MENU option*/

							ucScreenPatternNumber = SCREEN_PATTERN_2;
							ucSwitchPressFlag = TRUE;
							
							uiCurrentScreen = SCREEN_REMOTE_REPLICA;
							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_REMOTE_REPLICA;
							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
						}break;
						
						case SCREEN_REMOTE_REPLICA:
						{/*change20*/
							memset(stTx1Message.Data, 0 , sizeof(stTx1Message.Data));
							stTx1Message.DLC = 1;
							stTx1Message.Data[0] = BIT_3; //Pipe temperature

							fnCANMessage_Send(CMD_REMOTE_BUTTON_PRESS, MSG_TYPE_COMMAND, SCU_ID);
						}break;
						case SCREEN_MENU_2:
						{
							ucCurrentSwitchNum = 0;					
							/*MENU option*/

							ucScreenPatternNumber = SCREEN_PATTERN_1;
							ucSwitchPressFlag = TRUE;
							
							/*change3*/
							uiCursorXPos = 66;
							uiCursorYPos = 15;
							ucETHIPSetVariableCounter = 1;
							uiCurrentScreen = SCREEN_ETH_IP_DETAILS;
							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_ETH_IP_DETAILS;
							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
						}break;
						/****/
					}
					ucCurrentSwitchNum = 0;
				}break;
			
			case SWITCH9:
				{
					switch(uiCurrentScreen)
					{
						case SCREEN_MENU_1:
						{
							ucCurrentSwitchNum = 0;					
							/*MENU option*/

							ucScreenPatternNumber = SCREEN_PATTERN_2;
							ucSwitchPressFlag = TRUE;
							
							uiCurrentScreen = SCREEN_STATUS;
							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_STATUS;
							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
						}break;
						
						case SCREEN_CURRENT_PARA:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_1;
							ucSwitchPressFlag = TRUE;
							
							uiCurrentScreen = SCREEN_PARAM_PRESSURE;
							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_PARAM_PRESSURE;
							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;	
						}break;
						
						case SCREEN_ADVANCED:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_1;
							ucSwitchPressFlag = TRUE;
							
							uiCursorXPos = 66;
									uiCursorYPos = 35;
							
							
							ucResetVariableCounter = 1;
							
							uiCurrentScreen = SCREEN_ADVANCED_RESET;
							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_ADVANCED_RESET;
							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
						}break;
						
						case SCREEN_STATUS:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_1;
							ucSwitchPressFlag = TRUE;
							
							uiCurrentScreen = SCREEN_VALVE_STATUS_1;
							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_VALVE_STATUS_1;
							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;	
						}break;
						
						case SCREEN_DIAGNOSTIC_STATUS:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_1;
							ucSwitchPressFlag = TRUE;
							
							uiCurrentScreen = SCREEN_OPMM_DIAGNOSTIC_1;
							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_OPMM_DIAGNOSTIC_1;
							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;	
						}break;
						case SCREEN_PARAMETER_SET_1:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_3;
							ucSwitchPressFlag = TRUE;
							
							uiCursorXPos = 66;
									uiCursorYPos = 35;
							
							
							ucTempSetVariableCounter = 1;
							
							uiCurrentScreen = SCREEN_TEMP_SETTING_1;
							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_TEMP_SETTING_1  ;
							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
						}break;
						
						case SCREEN_PARAMETER_SET_2:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_3;
							ucSwitchPressFlag = TRUE;
							
							uiCursorXPos = 66;
							uiCursorYPos = 15;
							
							ucStallSetVariableCounter = 1;
							
							uiCurrentScreen = SCREEN_STALL_SETTING;
							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_STALL_SETTING;
							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
						}break;
						
						case SCREEN_MENU_2:
						{				
							/*MENU option*/

							ucScreenPatternNumber = SCREEN_PATTERN_2;
							ucSwitchPressFlag = TRUE;
							
							uiCurrentScreen = SCREEN_PERIPHERAL_SETTING;
							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_PERIPHERAL_SETTING;
							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
						}break;
						
						case SCREEN_PERIPHERAL_SETTING:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_5;
							ucSwitchPressFlag = TRUE;
							
							uiCursorXPos = 100;
							uiCursorYPos = 35;
							
							
							ucHCMPeripheralSetCounter = 1;
							
							uiCurrentScreen = SCREEN_HCM_PP_SETTING_1;
							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_HCM_PP_SETTING_1;
							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
							
							StatusFlag.stStatusFlag.bPortValuesSet = FALSE;
						}break;
						case SCREEN_VCM_PP_SETTING_1:
						{
							if ( 1 == ucVCMPeripheralSetCounter )
							{
								stConfig_Value.uiConfigVCMBypass &= ~BIT_1;
							}
							else if ( 2 == ucVCMPeripheralSetCounter )
							{
								stConfig_Value.uiConfigVCMBypass &= ~BIT_2;
							}
							else	if ( 3 == ucVCMPeripheralSetCounter )
							{
								stConfig_Value.uiConfigVCMBypass &= ~BIT_3;
							}
							else if ( 4 == ucVCMPeripheralSetCounter )
							{
								stConfig_Value.uiConfigVCMBypass &= ~BIT_5;
							}
							else if ( 5 == ucVCMPeripheralSetCounter )
							{
								stConfig_Value.uiConfigVCMBypass &= ~BIT_6;
							}
						}break;
						case SCREEN_VCM_PP_SETTING_2:
						{
							if ( 1 == ucVCMPeripheralSetCounter )
							{
								stConfig_Value.uiConfigVCMBypass &= ~BIT_4;
							}
							else if ( 2 == ucVCMPeripheralSetCounter )
							{
								stConfig_Value.uiConfigVCMBypass &= ~BIT_0;
							}
							else	if ( 4 == ucVCMPeripheralSetCounter )
							{
								stConfig_Value.uiConfigVCMBypass &= ~BIT_7;
							}
						}break;
						case SCREEN_HCM_PP_SETTING_1:
						{
							if ( 1 == ucHCMPeripheralSetCounter )
							{
								stConfig_Value.uiConfigHCMBypass &= ~BIT_0;
							}
							else if ( 2 == ucHCMPeripheralSetCounter )
							{
								stConfig_Value.uiConfigHCMBypass &= ~BIT_1;
							}
							else	if ( 3 == ucHCMPeripheralSetCounter )
							{
								stConfig_Value.uiConfigHCMBypass &= ~BIT_2;
							}
							else if ( 4 == ucHCMPeripheralSetCounter )
							{
								stConfig_Value.uiConfigHCMBypass &= ~BIT_3;
							}
							else if ( 5 == ucHCMPeripheralSetCounter )
							{
								stConfig_Value.uiConfigHCMBypass &= ~BIT_4;
							}
						}break;
						case SCREEN_HCM_PP_SETTING_2:
						{
							if ( 1 == ucHCMPeripheralSetCounter )
							{
								stConfig_Value.uiConfigHCMBypass &= ~BIT_5;
							}
							else if ( 2 == ucHCMPeripheralSetCounter )
							{
								stConfig_Value.uiConfigHCMBypass &= ~BIT_6;
							}
							else	if ( 3 == ucHCMPeripheralSetCounter )
							{
								stConfig_Value.uiConfigHCMBypass &= ~BIT_7;
							}
							else	if ( 4 == ucHCMPeripheralSetCounter )
							{
								stConfig_Value.uiConfigHCMBypass &= ~BIT_8;
							}
							else	if ( 5 == ucHCMPeripheralSetCounter )
							{
								stConfig_Value.uiConfigHCMBypass &= ~BIT_9;
							}
						}break;
						case SCREEN_OPMM_PP_SETTING_1:
						{
							if ( 1 == ucOPMMPeripheralSetCounter )
							{
								stConfig_Value.uiConfigOPMMBypass &= ~BIT_0;
							}
							else if ( 2 == ucOPMMPeripheralSetCounter )
							{
								stConfig_Value.uiConfigOPMMBypass &= ~BIT_1;
							}
							else	if ( 3 == ucOPMMPeripheralSetCounter )
							{
								stConfig_Value.uiConfigOPMMBypass &= ~BIT_2;
							}
							else if ( 4 == ucOPMMPeripheralSetCounter )
							{
								stConfig_Value.uiConfigOPMMBypass &= ~BIT_3;
							}
							else if ( 5 == ucOPMMPeripheralSetCounter )
							{
								stConfig_Value.uiConfigOPMMBypass &= ~BIT_4;
							}
						}break;
						case SCREEN_OPMM_PP_SETTING_2:
						{
							if ( 1 == ucOPMMPeripheralSetCounter )
							{
								stConfig_Value.uiConfigOPMMBypass &= ~BIT_5;
							}
							else if ( 2 == ucOPMMPeripheralSetCounter )
							{
								stConfig_Value.uiConfigOPMMBypass &= ~BIT_8;
							}
							else	if ( 3 == ucOPMMPeripheralSetCounter )
							{
								stConfig_Value.uiConfigOPMMBypass &= ~BIT_9;
							}
							else	if ( 4 == ucOPMMPeripheralSetCounter )
							{
								stConfig_Value.uiConfigOPMMBypass &= ~BIT_10;
							}
							else	if ( 5 == ucOPMMPeripheralSetCounter )
							{
								stConfig_Value.uiConfigOPMMBypass &= ~BIT_11;
							}
						}break;
						case SCREEN_OPMM_PP_SETTING_3:
						{
							if ( 1 == ucOPMMPeripheralSetCounter )
							{
								stConfig_Value.uiConfigOPMMBypass &= ~BIT_12;
							}
							else if ( 2 == ucOPMMPeripheralSetCounter )
							{
								stConfig_Value.uiConfigOPMMBypass &= ~BIT_13;
							}
						}break;
						case SCREEN_SCU_PP_SETTING:
						{
							if ( 1 == ucSCUPeripheralSetCounter )
							{
								stConfig_Value.uiConfigSCUBypass &= ~BIT_0;
							}
							else if ( 2 == ucSCUPeripheralSetCounter )
							{
								stConfig_Value.uiConfigSCUBypass &= ~BIT_1;
							}
							else	if ( 3 == ucSCUPeripheralSetCounter )
							{
								stConfig_Value.uiConfigSCUBypass &= ~BIT_2;
							}
							else if ( 4 == ucSCUPeripheralSetCounter )
							{
								StatusFlag.stStatusFlag.bSCU_Temp_Bypass = FALSE;
							}
							else if ( 5 == ucSCUPeripheralSetCounter )
							{
								StatusFlag.stStatusFlag.bSCU_Pressure_Bypass = FALSE;
							}
						}break;
						case SCREEN_HOME:
						{
							ucCurrentSwitchNum = 0;					
							/*MENU option*/

							ucScreenPatternNumber = SCREEN_PATTERN_6;
							ucSwitchPressFlag = TRUE;
							
							uiCurrentScreen = SCREEN_NOTIFICATION_1;
							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_NOTIFICATION_1;
							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
						}break;
						case SCREEN_MANAGE_PERIPHERAL_1:
						{
							if (FALSE == StatusFlag.stStatusFlag.OPMM_VL_AB_status)
							{
								/*Turn AB valve ON*/
								memset(stTx1Message.Data, 0 , sizeof(stTx1Message.Data));
								stTx1Message.DLC = 2;
								stTx1Message.Data[0] = OPMM_VL_AB;
								stTx1Message.Data[1] = ON_COMMAND;

								fnCANMessage_Send(CMD_OPMM_VALVE, MSG_TYPE_COMMAND, SCU_ID);
							}
							else if (TRUE == StatusFlag.stStatusFlag.OPMM_VL_AB_status)
							{
								/*Turn AB  valve OFF*/
								memset(stTx1Message.Data, 0 , sizeof(stTx1Message.Data));
								stTx1Message.DLC = 2;
								stTx1Message.Data[0] = OPMM_VL_AB;
								stTx1Message.Data[1] = OFF_COMMAND;

								fnCANMessage_Send(CMD_OPMM_VALVE, MSG_TYPE_COMMAND, SCU_ID);
							}
						}break;
						case SCREEN_MANAGE_PERIPHERAL_2:
						{
							/*TURN ON/OFF laser Pointer*/
							memset(stTx1Message.Data, 0 , sizeof(stTx1Message.Data));
							stTx1Message.DLC = 1;
							if (FALSE == StatusFlag.stStatusFlag.bLaserBeamStatus)
							{
								stTx1Message.Data[0] = ON_COMMAND;
								StatusFlag.stStatusFlag.bLaserBeamStatus = TRUE;
							}
							else
							{
								stTx1Message.Data[0] = OFF_COMMAND;
								StatusFlag.stStatusFlag.bLaserBeamStatus = FALSE;
							}

							fnCANMessage_Send(CMD_LASER_BEAM, MSG_TYPE_COMMAND, SCU_ID);
						}break;
						case SCREEN_STALL:
						{
							if( FALSE == StatusFlag.stStatusFlag.bStallTestStatusReceived)
							{
								ucSwitchPressFlag = TRUE;
								StatusFlag.stStatusFlag.bStallTestStatusReceived = TRUE;
								StatusFlag.stStatusFlag.bUpstrokeStatusReceived = FALSE;
								StatusFlag.stStatusFlag.bUpStrokeStatus = FALSE;
								StatusFlag.stStatusFlag.bDownstrokeStatusReceived = FALSE;
								StatusFlag.stStatusFlag.bDownStrokeStatus = FALSE;
								
								stCurrent_Value.iCurrPressureLegADown = 0;
								stCurrent_Value.iCurrPressureLegBDown = 0;
								stCurrent_Value.iCurrPressureLegCDown = 0;
								stCurrent_Value.iCurrPressureLegAUp = 0;
								stCurrent_Value.iCurrPressureLegBUp = 0;
								stCurrent_Value.iCurrPressureLegCUp = 0;
								
								/*send manual stall test command*/
								memset(stTx1Message.Data, 0 , sizeof(stTx1Message.Data));
								stTx1Message.DLC = 1;

								stTx1Message.Data[0] = MANUAL_PROCESS;
								
								fnCANMessage_Send(CMD_STALL_TEST, MSG_TYPE_COMMAND, SCU_ID);
							}
						}break;
						case SCREEN_REMOTE_REPLICA:
						{/*change20*/
							memset(stTx1Message.Data, 0 , sizeof(stTx1Message.Data));
							stTx1Message.DLC = 1;
							stTx1Message.Data[0] = BIT_1;  //Flush

							fnCANMessage_Send(CMD_REMOTE_BUTTON_PRESS, MSG_TYPE_COMMAND, SCU_ID);
						}break;
					}					
					ucCurrentSwitchNum = 0;
				}break;
				
			case SWITCH10:
				{
					switch(uiCurrentScreen)
					{
						case SCREEN_VCM_PP_SETTING_1:
						case SCREEN_VCM_PP_SETTING_2:
						{
							if(FALSE == StatusFlag.stStatusFlag.bPortValuesSet)
							{
								StatusFlag.stStatusFlag.bVCMPortSetFail = FALSE;
								
								stConfig_Value.ucConfigConnPressureGuage1 	= stConfig_Value.ucConfigConnPressureGuage1Rev;
								stConfig_Value.ucConfigConnPressureGuage2 	= stConfig_Value.ucConfigConnPressureGuage2Rev;
								stConfig_Value.ucConfigConnPressureGuage3 	= stConfig_Value.ucConfigConnPressureGuage3Rev;
								stConfig_Value.ucConfigConnLinearTransducer = stConfig_Value.ucConfigConnLinearTransducerRev;
								stConfig_Value.ucConfigConnInsideEE07 			= stConfig_Value.ucConfigConnInsideEE07Rev;
								stConfig_Value.ucConfigConnOutsideEE07 			= stConfig_Value.ucConfigConnOutsideEE07Rev;
								stConfig_Value.ucConfigConnStallvalve 			= stConfig_Value.ucConfigConnStallvalveRev;
								/*change15*/
								stConfig_Value.ucConfigConnRecircvalve 			= stConfig_Value.ucConfigConnRecircvalveRev;
								stConfig_Value.uiConfigVCMBypass = stConfig_Value.uiConfigVCMBypassRev;
							}
						}break;
						case SCREEN_HCM_PP_SETTING_1:
						case SCREEN_HCM_PP_SETTING_2:
						{
							if(FALSE == StatusFlag.stStatusFlag.bPortValuesSet)
							{
								StatusFlag.stStatusFlag.bHCMPortSetFail = FALSE;
								
								stConfig_Value.ucConfig_ConnBaseInlineTher 			= stConfig_Value.ucConfig_ConnBaseInlineTherRev;
								stConfig_Value.ucConfig_ConnHardInlineTher 			= stConfig_Value.ucConfig_ConnHardInlineTherRev;
								stConfig_Value.ucConfig_ConnAntifreezeTher 			= stConfig_Value.ucConfig_ConnAntifreezeTherRev;
								stConfig_Value.ucConfig_ConnBasereturnlineTher	= stConfig_Value.ucConfig_ConnBasereturnlineTherRev;
								stConfig_Value.ucConfig_ConnBaseinlineHeater 		= stConfig_Value.ucConfig_ConnBaseinlineHeaterRev;
								stConfig_Value.ucConfig_ConnHardinlineHeater 		= stConfig_Value.ucConfig_ConnHardinlineHeaterRev;
								stConfig_Value.ucConfig_ConnAntifreezeHeater 		= stConfig_Value.ucConfig_ConnAntifreezeHeaterRev;
								stConfig_Value.ucConfig_ConnHardHeater 					= stConfig_Value.ucConfig_ConnHardHeaterRev;
								stConfig_Value.ucConfig_ConnBaseTankLL 					= stConfig_Value.ucConfig_ConnBaseTankLLRev;
								stConfig_Value.ucConfig_ConnHardTankLL 					= stConfig_Value.ucConfig_ConnHardTankLLRev;
								
								stConfig_Value.uiConfigHCMBypass = stConfig_Value.uiConfigHCMBypassRev;
							}
						}break;
						case SCREEN_OPMM_PP_SETTING_1:
						case SCREEN_OPMM_PP_SETTING_2:
						case SCREEN_OPMM_PP_SETTING_3:
						{
							if(FALSE == StatusFlag.stStatusFlag.bPortValuesSet)
							{
								StatusFlag.stStatusFlag.bOPMMPortSetFail = FALSE;
								
								stConfig_Value.ucConfigConnSprayvalve 		= stConfig_Value.ucConfigConnSprayvalveRev;
								stConfig_Value.ucConfigConnGunposition 		= stConfig_Value.ucConfigConnGunpositionRev;
								stConfig_Value.ucConfigConnABvalve 				= stConfig_Value.ucConfigConnABvalveRev;
								stConfig_Value.ucConfigConnSolventvalve 	= stConfig_Value.ucConfigConnSolventvalveRev;
								stConfig_Value.ucConfigConnForwardvalve 	= stConfig_Value.ucConfigConnForwardvalveRev;
								stConfig_Value.ucConfigConnBackwardvalve	= stConfig_Value.ucConfigConnBackwardvalveRev;
								stConfig_Value.ucConfigNCTempSensor1 			= stConfig_Value.ucConfigNCTempSensor1Rev;
								stConfig_Value.ucConfigNCTempSensor2 			= stConfig_Value.ucConfigNCTempSensor2Rev;
								stConfig_Value.ucConfigNCTempSensor3 			= stConfig_Value.ucConfigNCTempSensor3Rev;
								stConfig_Value.ucConfigNCTempSensor4 			= stConfig_Value.ucConfigNCTempSensor4Rev;
								
								stConfig_Value.uiConfigOPMMBypass = stConfig_Value.uiConfigOPMMBypassRev;
							}
						}break;
						default:
							break;
					}
						
					ucCurrentSwitchNum = 0;					
					/*HOME option*/

					ucScreenPatternNumber = SCREEN_PATTERN_1;
					ucSwitchPressFlag = TRUE;
					
					uiCurrentScreen = SCREEN_HOME;
					uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_HOME;
					ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
				}break;
			
			case SWITCH11:
				{
					switch(uiCurrentScreen)
					{
						
					}
					ucCurrentSwitchNum = 0;
				}break;
				
			case SWITCH12:
				{
					switch(uiCurrentScreen)
					{
						
						
						
					}
					ucCurrentSwitchNum = 0;
				}break;	

			case SWITCH13:
				{
					switch(uiCurrentScreen)
					{
						case SCREEN_HOME:
						{
							ucSwitchPressFlag = TRUE;
							uliHmScreenRefreshCnt = 0;
						}break;
						case SCREEN_VCM_PP_SETTING_2:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_5;
							ucSwitchPressFlag = TRUE;
							
							uiCursorXPos = 100;
								uiCursorYPos = 35;
							
							
							ucVCMPeripheralSetCounter = 1;
							
							uiCurrentScreen = SCREEN_VCM_PP_SETTING_1;
							if( SCREEN_VCM_PP_SETTING_1 == uirgStoreCurrentScreen[ucCurrentScreenNumber - 1] )
							{
								--ucCurrentScreenNumber;
								--ucPatternNumberCount;
							}
//							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_VCM_PP_SETTING_1;
//							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
						}break;
						
						case SCREEN_HCM_PP_SETTING_2:
						{				
							ucScreenPatternNumber = SCREEN_PATTERN_5;
							ucSwitchPressFlag = TRUE;
							
							uiCursorXPos = 100;
								uiCursorYPos = 35;
							
							
							ucHCMPeripheralSetCounter = 1;
							
							uiCurrentScreen = SCREEN_HCM_PP_SETTING_1;
							if( SCREEN_HCM_PP_SETTING_1 == uirgStoreCurrentScreen[ucCurrentScreenNumber - 1] )
							{
								--ucCurrentScreenNumber;
								--ucPatternNumberCount;
							}
//							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_HCM_PP_SETTING_1;
//							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
						}break;
						
						case SCREEN_OPMM_PP_SETTING_2:
						{				
							ucScreenPatternNumber = SCREEN_PATTERN_5;
							ucSwitchPressFlag = TRUE;
							
							uiCursorXPos = 100;
								uiCursorYPos = 35;
							
							
							ucOPMMPeripheralSetCounter = 1;
							
							uiCurrentScreen = SCREEN_OPMM_PP_SETTING_1;
							if( SCREEN_OPMM_PP_SETTING_1 == uirgStoreCurrentScreen[ucCurrentScreenNumber - 1] )
							{
								--ucCurrentScreenNumber;
								--ucPatternNumberCount;
							}
//							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_OPMM_PP_SETTING_1;
//							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
						}break;
						
						case SCREEN_OPMM_PP_SETTING_3:
						{				
							ucScreenPatternNumber = SCREEN_PATTERN_5;
							ucSwitchPressFlag = TRUE;
							
							uiCursorXPos = 100;
							uiCursorYPos = 35;
							
							
							ucOPMMPeripheralSetCounter = 1;
							
							uiCurrentScreen = SCREEN_OPMM_PP_SETTING_2;
							if( SCREEN_OPMM_PP_SETTING_2 == uirgStoreCurrentScreen[ucCurrentScreenNumber - 1] )
							{
								--ucCurrentScreenNumber;
								--ucPatternNumberCount;
							}
//							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_OPMM_PP_SETTING_2;
//							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
					}break;
						case	SCREEN_MENU_2:						
						{
							ucScreenPatternNumber = SCREEN_PATTERN_2;
							ucSwitchPressFlag = TRUE;
							
							uiCurrentScreen = SCREEN_MENU_1;
							if( SCREEN_MENU_1 == uirgStoreCurrentScreen[ucCurrentScreenNumber - 1] )
							{
								--ucCurrentScreenNumber;
								--ucPatternNumberCount;
							}
//							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_MENU_1;
//							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
						}break;
						
						case SCREEN_MANAGE_PERIPHERAL_2:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_2;
							ucSwitchPressFlag = TRUE;
							
							uiCurrentScreen = SCREEN_MANAGE_PERIPHERAL_1;
							if( SCREEN_MANAGE_PERIPHERAL_1 == uirgStoreCurrentScreen[ucCurrentScreenNumber - 1] )
							{
								--ucCurrentScreenNumber;
								--ucPatternNumberCount;
							}
//							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_MANAGE_PERIPHERAL_1;
//							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
						}break;
						/*change13*/
						case SCREEN_MANAGE_PERIPHERAL_3:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_2;
							ucSwitchPressFlag = TRUE;
							
							uiCurrentScreen = SCREEN_MANAGE_PERIPHERAL_2;
							if( SCREEN_MANAGE_PERIPHERAL_2 == uirgStoreCurrentScreen[ucCurrentScreenNumber - 1] )
							{
								--ucCurrentScreenNumber;
								--ucPatternNumberCount;
							}
//							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_MANAGE_PERIPHERAL_1;
//							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
						}break;
						case SCREEN_PARAM_TEMP_2:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_1;
							ucSwitchPressFlag = TRUE;
							
							uiCurrentScreen = SCREEN_PARAM_TEMP_1;
							if( SCREEN_PARAM_TEMP_1 == uirgStoreCurrentScreen[ucCurrentScreenNumber - 1] )
							{
								--ucCurrentScreenNumber;
								--ucPatternNumberCount;
							}
//							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_PARAM_TEMP_1;
//							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;	
						}break;
						
						case SCREEN_PARAM_TEMP_3:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_1;
							ucSwitchPressFlag = TRUE;
							
							uiCurrentScreen = SCREEN_PARAM_TEMP_2;
							if( SCREEN_PARAM_TEMP_2 == uirgStoreCurrentScreen[ucCurrentScreenNumber - 1] )
							{
								--ucCurrentScreenNumber;
								--ucPatternNumberCount;
							}
//							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_PARAM_TEMP_2;
//							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;	
						}break;
						
						case SCREEN_VALVE_STATUS_2:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_1;
							ucSwitchPressFlag = TRUE;
							
							uiCurrentScreen = SCREEN_VALVE_STATUS_1;
							if( SCREEN_VALVE_STATUS_1 == uirgStoreCurrentScreen[ucCurrentScreenNumber - 1] )
							{
								--ucCurrentScreenNumber;
								--ucPatternNumberCount;
							}
//							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_VALVE_STATUS_1;
//							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;	
						}break;
						
						case SCREEN_PARAMETER_SET_2:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_2;
							ucSwitchPressFlag = TRUE;
							
							uiCurrentScreen = SCREEN_PARAMETER_SET_1;
							if( SCREEN_PARAMETER_SET_1 == uirgStoreCurrentScreen[ucCurrentScreenNumber - 1] )
							{
								--ucCurrentScreenNumber;
								--ucPatternNumberCount;
							}
//							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_PARAMETER_SET_1  ;
//							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
						}break;
						
						case SCREEN_LIQUID_LEVEL_2:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_3;
							ucSwitchPressFlag = TRUE;
							
							uiCurrentScreen = SCREEN_LIQUID_LEVEL_1;
							if( SCREEN_LIQUID_LEVEL_1 == uirgStoreCurrentScreen[ucCurrentScreenNumber - 1] )
							{
								--ucCurrentScreenNumber;
								--ucPatternNumberCount;
							}
//							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_LIQUID_LEVEL_1;
//							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
						}break;
						
						case SCREEN_TEMP_SETTING_2:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_3;
							ucSwitchPressFlag = TRUE;
							
							uiCursorXPos = 66;
							uiCursorYPos = 35;
							
							
							ucTempSetVariableCounter = 1;
							
							uiCurrentScreen = SCREEN_TEMP_SETTING_1;
							if( SCREEN_TEMP_SETTING_1 == uirgStoreCurrentScreen[ucCurrentScreenNumber - 1] )
							{
								--ucCurrentScreenNumber;
								--ucPatternNumberCount;
							}
//							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_TEMP_SETTING_1;
//							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
						}break;
						
						case SCREEN_TIMER_SETTING_4:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_3;
							ucSwitchPressFlag = TRUE;
							
							uiCursorXPos = 66;
							uiCursorYPos = 15;
							
							ucTimerSetVariableCounter = 1;
							
							uiCurrentScreen = SCREEN_TIMER_SETTING_3;
							if( SCREEN_TIMER_SETTING_3 == uirgStoreCurrentScreen[ucCurrentScreenNumber - 1] )
							{
								--ucCurrentScreenNumber;
								--ucPatternNumberCount;
							}
//							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_TIMER_SETTING_3;
//							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
						}break;
						
						case SCREEN_TIMER_SETTING_3:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_3;
							ucSwitchPressFlag = TRUE;
							
							uiCursorXPos = 66;
							uiCursorYPos = 15;
							
							ucTimerSetVariableCounter = 1;
							
							uiCurrentScreen = SCREEN_TIMER_SETTING_2;
							if( SCREEN_TIMER_SETTING_2 == uirgStoreCurrentScreen[ucCurrentScreenNumber - 1] )
							{
								--ucCurrentScreenNumber;
								--ucPatternNumberCount;
							}
//							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_TIMER_SETTING_2;
//							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
						}break;
						
						case SCREEN_TIMER_SETTING_2:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_3;
							ucSwitchPressFlag = TRUE;
							
							uiCursorXPos = 66;
							uiCursorYPos = 15;
							
							ucTimerSetVariableCounter = 1;
							
							uiCurrentScreen = SCREEN_TIMER_SETTING_1;
							if( SCREEN_TIMER_SETTING_1 == uirgStoreCurrentScreen[ucCurrentScreenNumber - 1] )
							{
								--ucCurrentScreenNumber;
								--ucPatternNumberCount;
							}
//							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_TIMER_SETTING_1;
//							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
						}break;
						
						case SCREEN_HYSTERESIS_2:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_3;
							ucSwitchPressFlag = TRUE;
							
							uiCursorXPos = 66;
									uiCursorYPos = 35;
						
							
							ucHystSetVariableCounter = 1;
							
							uiCurrentScreen = SCREEN_HYSTERESIS_1;
							if( SCREEN_HYSTERESIS_1 == uirgStoreCurrentScreen[ucCurrentScreenNumber - 1] )
							{
								--ucCurrentScreenNumber;
								--ucPatternNumberCount;
							}
//							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_HYSTERESIS_1;
//							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
						}break;
						
						case SCREEN_PRESSURE_SETTING_2:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_3;
							ucSwitchPressFlag = TRUE;
							
							uiCursorXPos = 66;
								uiCursorYPos = 35;
							
							
							ucPresSetVariableCounter = 1;
							
							uiCurrentScreen = SCREEN_PRESSURE_SETTING_1;
							if( SCREEN_PRESSURE_SETTING_1 == uirgStoreCurrentScreen[ucCurrentScreenNumber - 1] )
							{
								--ucCurrentScreenNumber;
								--ucPatternNumberCount;
							}
//							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_PRESSURE_SETTING_1;
//							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
						}break;
						case SCREEN_ALARMS_4:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_6;
							ucSwitchPressFlag = TRUE;
							
							uiCurrentScreen = SCREEN_ALARMS_3;
							if( SCREEN_ALARMS_3 == uirgStoreCurrentScreen[ucCurrentScreenNumber - 1] )
							{
								--ucCurrentScreenNumber;
								--ucPatternNumberCount;
							}
//							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_ALARMS_3;
//							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
						}break;
						case SCREEN_ALARMS_3:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_6;
							ucSwitchPressFlag = TRUE;
							
							uiCurrentScreen = SCREEN_ALARMS_2;
							if( SCREEN_ALARMS_2 == uirgStoreCurrentScreen[ucCurrentScreenNumber - 1] )
							{
								--ucCurrentScreenNumber;
								--ucPatternNumberCount;
							}
//							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_ALARMS_2;
//							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
						}break;
						case SCREEN_ALARMS_2:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_6;
							ucSwitchPressFlag = TRUE;
							
							uiCurrentScreen = SCREEN_ALARMS_1;
							if( SCREEN_ALARMS_1 == uirgStoreCurrentScreen[ucCurrentScreenNumber - 1] )
							{
								--ucCurrentScreenNumber;
								--ucPatternNumberCount;
							}
//							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_ALARMS_1;
//							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
						}break;
						case SCREEN_NOTIFICATION_4:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_6;
							ucSwitchPressFlag = TRUE;
							
							uiCurrentScreen = SCREEN_NOTIFICATION_3;
							if( SCREEN_NOTIFICATION_3 == uirgStoreCurrentScreen[ucCurrentScreenNumber - 1] )
							{
								--ucCurrentScreenNumber;
								--ucPatternNumberCount;
							}
//							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_NOTIFICATION_3;
//							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
						}break;
						case SCREEN_NOTIFICATION_3:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_6;
							ucSwitchPressFlag = TRUE;
							
							uiCurrentScreen = SCREEN_NOTIFICATION_2;
							if( SCREEN_NOTIFICATION_2 == uirgStoreCurrentScreen[ucCurrentScreenNumber - 1] )
							{
								--ucCurrentScreenNumber;
								--ucPatternNumberCount;
							}
//							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_NOTIFICATION_2;
//							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
						}break;
						case SCREEN_NOTIFICATION_2:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_6;
							ucSwitchPressFlag = TRUE;
							
							uiCurrentScreen = SCREEN_NOTIFICATION_1;
							if( SCREEN_NOTIFICATION_1 == uirgStoreCurrentScreen[ucCurrentScreenNumber - 1] )
							{
								--ucCurrentScreenNumber;
								--ucPatternNumberCount;
							}
//							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_NOTIFICATION_1;
//							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
						}break;
						case SCREEN_JOB_SETTING_2:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_3;
							ucSwitchPressFlag = TRUE;
							
							uiCursorXPos = 66;
								uiCursorYPos = 35;
							
							
							ucJobSetVariableCounter = 1;
							
							uiCurrentScreen = SCREEN_JOB_SETTING_1;
							if( SCREEN_JOB_SETTING_1 == uirgStoreCurrentScreen[ucCurrentScreenNumber - 1] )
							{
								--ucCurrentScreenNumber;
								--ucPatternNumberCount;
							}
//							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_JOB_SETTING_1;
//							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
						}break;
						case SCREEN_OTHER_SETTING_1:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_3;
							ucSwitchPressFlag = TRUE;
							
							uiCursorXPos = 66;
								uiCursorYPos = 35;
						
							
							ucOtherSetVariableCounter = 1;
							
							uiCurrentScreen = SCREEN_OTHER_SETTING;
							if( SCREEN_OTHER_SETTING == uirgStoreCurrentScreen[ucCurrentScreenNumber - 1] )
							{
								--ucCurrentScreenNumber;
								--ucPatternNumberCount;
							}
						}break;
						/*********/
						/*4thMarch2016*/
						case SCREEN_STALL_SETTING_1:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_3;
							ucSwitchPressFlag = TRUE;
							
							uiCursorXPos = 66;
							uiCursorYPos = 15;
							
							ucStallSetVariableCounter = 1;
							
							uiCurrentScreen = SCREEN_STALL_SETTING;
							if( SCREEN_STALL_SETTING == uirgStoreCurrentScreen[ucCurrentScreenNumber - 1] )
							{
								--ucCurrentScreenNumber;
								--ucPatternNumberCount;
							}
						}break;
						/*change4*/
						case SCREEN_MEMORY_ALARM_4:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_6;
							ucSwitchPressFlag = TRUE;
							
							uiCurrentScreen = SCREEN_MEMORY_ALARM_3;
							if( SCREEN_MEMORY_ALARM_3 == uirgStoreCurrentScreen[ucCurrentScreenNumber - 1] )
							{
								--ucCurrentScreenNumber;
								--ucPatternNumberCount;
							}
//							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_NOTIFICATION_3;
//							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
						}break;
						case SCREEN_MEMORY_ALARM_3:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_6;
							ucSwitchPressFlag = TRUE;
							
							uiCurrentScreen = SCREEN_MEMORY_ALARM_2;
							if( SCREEN_MEMORY_ALARM_2 == uirgStoreCurrentScreen[ucCurrentScreenNumber - 1] )
							{
								--ucCurrentScreenNumber;
								--ucPatternNumberCount;
							}
//							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_NOTIFICATION_2;
//							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
						}break;
						case SCREEN_MEMORY_ALARM_2:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_6;
							ucSwitchPressFlag = TRUE;
							
							uiCurrentScreen = SCREEN_MEMORY_ALARM_1;
							if( SCREEN_MEMORY_ALARM_1 == uirgStoreCurrentScreen[ucCurrentScreenNumber - 1] )
							{
								--ucCurrentScreenNumber;
								--ucPatternNumberCount;
							}
//							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_NOTIFICATION_1;
//							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
						}break;
				}
				ucCurrentSwitchNum = 0;
			}break;
				
			case SWITCH14:
				{
					switch(uiCurrentScreen)
					{
						case SCREEN_HOME:
						{
							ucSwitchPressFlag = TRUE;
							uliHmScreenRefreshCnt = 5000;
						}break;
						case SCREEN_VCM_PP_SETTING_1:
						{				
							ucScreenPatternNumber = SCREEN_PATTERN_5;
							ucSwitchPressFlag = TRUE;
							
							uiCursorXPos = 100;
							uiCursorYPos = 35;
							
							
							ucVCMPeripheralSetCounter = 1;
							
							uiCurrentScreen = SCREEN_VCM_PP_SETTING_2;
							if(SCREEN_VCM_PP_SETTING_2 != uirgStoreCurrentScreen[ucCurrentScreenNumber - 1] )
							{
								uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_VCM_PP_SETTING_2;
								ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
							}
						}break;
						
						case SCREEN_HCM_PP_SETTING_1:
						{				
							ucScreenPatternNumber = SCREEN_PATTERN_5;
							ucSwitchPressFlag = TRUE;
							
							uiCursorXPos = 100;
							uiCursorYPos = 35;
							
							
							ucHCMPeripheralSetCounter = 1;
							
							uiCurrentScreen = SCREEN_HCM_PP_SETTING_2;
							if(SCREEN_HCM_PP_SETTING_2 != uirgStoreCurrentScreen[ucCurrentScreenNumber - 1] )
							{
								uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_HCM_PP_SETTING_2;
								ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
							}
						}break;
						
						case SCREEN_OPMM_PP_SETTING_1:
						{				
							ucScreenPatternNumber = SCREEN_PATTERN_5;
							ucSwitchPressFlag = TRUE;
							
							uiCursorXPos = 100;
								uiCursorYPos = 35;
							
							ucOPMMPeripheralSetCounter = 1;
							
							uiCurrentScreen = SCREEN_OPMM_PP_SETTING_2;
							if(SCREEN_OPMM_PP_SETTING_2 != uirgStoreCurrentScreen[ucCurrentScreenNumber - 1] )
							{
								uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_OPMM_PP_SETTING_2;
								ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
							}
						}break;
						
						case SCREEN_OPMM_PP_SETTING_2:
						{				
							ucScreenPatternNumber = SCREEN_PATTERN_5;
							ucSwitchPressFlag = TRUE;
							
							uiCursorXPos = 100;
								uiCursorYPos = 35;
							
							
							ucOPMMPeripheralSetCounter = 1;
							
							uiCurrentScreen = SCREEN_OPMM_PP_SETTING_3;
							if(SCREEN_OPMM_PP_SETTING_3 != uirgStoreCurrentScreen[ucCurrentScreenNumber - 1] )
							{
								uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_OPMM_PP_SETTING_3;
								ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
							}
						}break;
						case	SCREEN_MENU_1:						
						{							
							ucScreenPatternNumber = SCREEN_PATTERN_2;
							ucSwitchPressFlag = TRUE;
							
							uiCurrentScreen = SCREEN_MENU_2;
							if(SCREEN_MENU_2 != uirgStoreCurrentScreen[ucCurrentScreenNumber - 1] )
							{
								uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_MENU_2;
								ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
							}
						}break;
						
						case SCREEN_MANAGE_PERIPHERAL_1:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_2;
							ucSwitchPressFlag = TRUE;
							
							uiCurrentScreen = SCREEN_MANAGE_PERIPHERAL_2;
							if(SCREEN_MANAGE_PERIPHERAL_2 != uirgStoreCurrentScreen[ucCurrentScreenNumber - 1] )
							{
								uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_MANAGE_PERIPHERAL_2;
								ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
							}
						}break;
						/*change13*/
						case SCREEN_MANAGE_PERIPHERAL_2:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_2;
							ucSwitchPressFlag = TRUE;
							
							uiCurrentScreen = SCREEN_MANAGE_PERIPHERAL_3;
							if(SCREEN_MANAGE_PERIPHERAL_3 != uirgStoreCurrentScreen[ucCurrentScreenNumber - 1] )
							{
								uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_MANAGE_PERIPHERAL_3;
								ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
							}
						}break;
						case SCREEN_PARAMETER_SET_1:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_2;
							ucSwitchPressFlag = TRUE;
							
							uiCurrentScreen = SCREEN_PARAMETER_SET_2;
							if(SCREEN_PARAMETER_SET_2 != uirgStoreCurrentScreen[ucCurrentScreenNumber - 1] )
							{
								uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_PARAMETER_SET_2;
								ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
							}
						}break;
						
						case SCREEN_PARAM_TEMP_1:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_1;
							ucSwitchPressFlag = TRUE;
							
							uiCurrentScreen = SCREEN_PARAM_TEMP_2;
							if(SCREEN_PARAM_TEMP_2 != uirgStoreCurrentScreen[ucCurrentScreenNumber - 1] )
							{
								uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_PARAM_TEMP_2;
								ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;	
							}
						}break;
						
						case SCREEN_PARAM_TEMP_2:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_1;
							ucSwitchPressFlag = TRUE;
							
							uiCurrentScreen = SCREEN_PARAM_TEMP_3;
							if(SCREEN_PARAM_TEMP_3 != uirgStoreCurrentScreen[ucCurrentScreenNumber - 1] )
							{
								uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_PARAM_TEMP_3;
								ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;	
							}
						}break;
						
						case SCREEN_VALVE_STATUS_1:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_1;
							ucSwitchPressFlag = TRUE;
							
							uiCurrentScreen = SCREEN_VALVE_STATUS_2;
							if(SCREEN_VALVE_STATUS_2 != uirgStoreCurrentScreen[ucCurrentScreenNumber - 1] )
							{
								uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_VALVE_STATUS_2;
								ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;	
							}
						}break;
						
						case SCREEN_LIQUID_LEVEL_1:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_3;
							ucSwitchPressFlag = TRUE;
							
							uiCursorXPos = 66;
							uiCursorYPos = 15;
							
							ucLiquidLSetVariableCounter1 = 1;
							
							uiCurrentScreen = SCREEN_LIQUID_LEVEL_2;
							if(SCREEN_LIQUID_LEVEL_2 != uirgStoreCurrentScreen[ucCurrentScreenNumber - 1] )
							{
								uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_LIQUID_LEVEL_2;
								ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
							}
						}break;
						
						case SCREEN_TEMP_SETTING_1:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_3;
							ucSwitchPressFlag = TRUE;
							
							uiCursorXPos = 66;
						uiCursorYPos = 35;
							
							
							ucTempSetVariableCounter = 1;
							
							uiCurrentScreen = SCREEN_TEMP_SETTING_2;
							if(SCREEN_TEMP_SETTING_2 != uirgStoreCurrentScreen[ucCurrentScreenNumber - 1] )
							{
								uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_TEMP_SETTING_2;
								ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
							}
						}break;
						
						case SCREEN_TIMER_SETTING_1:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_3;
							ucSwitchPressFlag = TRUE;
							
							uiCursorXPos = 66;
							uiCursorYPos = 15;
							
							ucTimerSetVariableCounter = 1;
							
							uiCurrentScreen = SCREEN_TIMER_SETTING_2;
							if(SCREEN_TIMER_SETTING_2 != uirgStoreCurrentScreen[ucCurrentScreenNumber - 1] )
							{
								uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_TIMER_SETTING_2;
								ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
							}
						}break;
						
						case SCREEN_TIMER_SETTING_2:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_3;
							ucSwitchPressFlag = TRUE;
							
							uiCursorXPos = 66;
							uiCursorYPos = 15;
							
							ucTimerSetVariableCounter = 1;
							
							uiCurrentScreen = SCREEN_TIMER_SETTING_3;
							if(SCREEN_TIMER_SETTING_3 != uirgStoreCurrentScreen[ucCurrentScreenNumber - 1] )
							{
								uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_TIMER_SETTING_3;
								ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
							}
						}break;
						
						case SCREEN_TIMER_SETTING_3:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_3;
							ucSwitchPressFlag = TRUE;
							
							uiCursorXPos = 66;
								uiCursorYPos = 35;
							
							
							ucTimerSetVariableCounter = 1;
							
							uiCurrentScreen = SCREEN_TIMER_SETTING_4;
							if(SCREEN_TIMER_SETTING_4 != uirgStoreCurrentScreen[ucCurrentScreenNumber - 1] )
							{
								uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_TIMER_SETTING_4;
								ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
							}
						}break;
						
						case SCREEN_HYSTERESIS_1:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_3;
							ucSwitchPressFlag = TRUE;
							
							uiCursorXPos = 66;
								uiCursorYPos = 35;
							
							
							ucHystSetVariableCounter = 1;
							
							uiCurrentScreen = SCREEN_HYSTERESIS_2;
							if(SCREEN_HYSTERESIS_2 != uirgStoreCurrentScreen[ucCurrentScreenNumber - 1] )
							{
								uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_HYSTERESIS_2;
								ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
							}
						}break;
						
						case SCREEN_PRESSURE_SETTING_1:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_3;
							ucSwitchPressFlag = TRUE;
							
							uiCursorXPos = 66;
							uiCursorYPos = 35;
							
							
							ucPresSetVariableCounter = 1;
							
							uiCurrentScreen = SCREEN_PRESSURE_SETTING_2;
							if(SCREEN_PRESSURE_SETTING_2 != uirgStoreCurrentScreen[ucCurrentScreenNumber - 1] )
							{
								uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_PRESSURE_SETTING_2;
								ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
							}
						}break;
						case SCREEN_ALARMS_1:
						{
							if (6 < ucAlarmCountLimit)
							{
								ucScreenPatternNumber = SCREEN_PATTERN_6;
								ucSwitchPressFlag = TRUE;
								
								uiCurrentScreen = SCREEN_ALARMS_2;
								if(SCREEN_ALARMS_2 != uirgStoreCurrentScreen[ucCurrentScreenNumber - 1] )
								{
									uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_ALARMS_2;
									ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
								}
							}
						}break;
						
						case SCREEN_ALARMS_2:
						{
							if (12 < ucAlarmCountLimit)
							{
								ucScreenPatternNumber = SCREEN_PATTERN_6;
								ucSwitchPressFlag = TRUE;
								
								uiCurrentScreen = SCREEN_ALARMS_3;
								if(SCREEN_ALARMS_3 != uirgStoreCurrentScreen[ucCurrentScreenNumber - 1] )
								{
									uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_ALARMS_3;
									ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
								}
							}
						}break;
						case SCREEN_ALARMS_3:
						{
							if (18 < ucAlarmCountLimit)
							{
								ucScreenPatternNumber = SCREEN_PATTERN_6;
								ucSwitchPressFlag = TRUE;
								
								uiCurrentScreen = SCREEN_ALARMS_4;
								if(SCREEN_ALARMS_4 != uirgStoreCurrentScreen[ucCurrentScreenNumber - 1] )
								{
									uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_ALARMS_4;
									ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
								}
							}
						}break;
						case SCREEN_NOTIFICATION_1:
						{
							if ( 6 < ucStatusCountLimit)
							{
								ucScreenPatternNumber = SCREEN_PATTERN_6;
								ucSwitchPressFlag = TRUE;
								
								uiCurrentScreen = SCREEN_NOTIFICATION_2;
								if(SCREEN_NOTIFICATION_2 != uirgStoreCurrentScreen[ucCurrentScreenNumber - 1] )
								{
									uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_NOTIFICATION_2;
									ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
								}
							}
						}break;
						
						case SCREEN_NOTIFICATION_2:
						{
							if (12 < ucStatusCountLimit)
							{
								ucScreenPatternNumber = SCREEN_PATTERN_6;
								ucSwitchPressFlag = TRUE;
								
								uiCurrentScreen = SCREEN_NOTIFICATION_3;
								if(SCREEN_NOTIFICATION_3 != uirgStoreCurrentScreen[ucCurrentScreenNumber - 1] )
								{
									uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_NOTIFICATION_3;
									ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
								}
							}
						}break;
						case SCREEN_NOTIFICATION_3:
						{
							if (18 < ucStatusCountLimit)
							{
								ucScreenPatternNumber = SCREEN_PATTERN_6;
								ucSwitchPressFlag = TRUE;
								
								uiCurrentScreen = SCREEN_NOTIFICATION_4;
								if(SCREEN_NOTIFICATION_4 != uirgStoreCurrentScreen[ucCurrentScreenNumber - 1] )
								{
									uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_NOTIFICATION_4;
									ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
								}
							}
						}break;
						case SCREEN_JOB_SETTING_1:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_3;
							ucSwitchPressFlag = TRUE;
							
							uiCursorXPos = 66;
					uiCursorYPos = 35;
							
							
							ucJobSetVariableCounter = 1;
							
							uiCurrentScreen = SCREEN_JOB_SETTING_2;
							if(SCREEN_JOB_SETTING_2 != uirgStoreCurrentScreen[ucCurrentScreenNumber - 1] )
							{
								uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_JOB_SETTING_2;
								ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
							}
						}break;
						case SCREEN_OTHER_SETTING:
						{
							ucScreenPatternNumber = SCREEN_PATTERN_3;
							ucSwitchPressFlag = TRUE;
							
							uiCursorXPos = 66;
							uiCursorYPos = 15;
							
							ucOtherSetVariableCounter = 1;
							
							uiCurrentScreen = SCREEN_OTHER_SETTING_1;
							if(SCREEN_OTHER_SETTING_1 != uirgStoreCurrentScreen[ucCurrentScreenNumber - 1] )
							{
								uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_OTHER_SETTING_1;
								ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
							}
						}break;
						
						case SCREEN_STALL_SETTING:
						{/*******/
						/*4thMarch2016*/
							ucScreenPatternNumber = SCREEN_PATTERN_3;
							ucSwitchPressFlag = TRUE;
							
							uiCursorXPos = 66;
							uiCursorYPos = 15;
							
							ucStallSetVariableCounter = 1;
							
							uiCurrentScreen = SCREEN_STALL_SETTING_1;
							if(SCREEN_STALL_SETTING_1 != uirgStoreCurrentScreen[ucCurrentScreenNumber - 1] )
							{
								uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_STALL_SETTING_1;
								ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
							}
						}break;
						/*change4*/
					case SCREEN_MEMORY_ALARM_1:
						{
							if ( 6 < ucMemoryAlarmCountLimit)
							{
								ucScreenPatternNumber = SCREEN_PATTERN_6;
								ucSwitchPressFlag = TRUE;
								
								uiCurrentScreen = SCREEN_MEMORY_ALARM_2;
								if(SCREEN_MEMORY_ALARM_2 != uirgStoreCurrentScreen[ucCurrentScreenNumber - 1] )
								{
									uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_MEMORY_ALARM_2;
									ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
								}
							}
						}break;
						
						case SCREEN_MEMORY_ALARM_2:
						{
							if (12 < ucMemoryAlarmCountLimit)
							{
								ucScreenPatternNumber = SCREEN_PATTERN_6;
								ucSwitchPressFlag = TRUE;
								
								uiCurrentScreen = SCREEN_MEMORY_ALARM_3;
								if(SCREEN_MEMORY_ALARM_3 != uirgStoreCurrentScreen[ucCurrentScreenNumber - 1] )
								{
									uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_MEMORY_ALARM_3;
									ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
								}
							}
						}break;
						case SCREEN_MEMORY_ALARM_3:
						{
							if (18 < ucMemoryAlarmCountLimit)
							{
								ucScreenPatternNumber = SCREEN_PATTERN_6;
								ucSwitchPressFlag = TRUE;
								
								uiCurrentScreen = SCREEN_MEMORY_ALARM_4;
								if(SCREEN_MEMORY_ALARM_4 != uirgStoreCurrentScreen[ucCurrentScreenNumber - 1] )
								{
									uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_MEMORY_ALARM_4;
									ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
								}
							}
						}break;

						
					}
					ucCurrentSwitchNum = 0;
				}break;
				
		}
	}
	ucCurrentSwitch = 0;
}

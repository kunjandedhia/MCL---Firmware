/******************************************************************************
**@file    MiscFunctions.c
* 
**@brief   This file contains the Misc functions
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
uint16_t uiTxCounter = 0;						/*CAN message transmit counter*/
FLAGS_TypeDef_Union	StatusFlag;			/*union variable of status flags*/
/*Structure variables for Valve Status Flags*/
TypeDef_ValveState	stValve[TOTAL_VALVES];							
/*union varible for Split bytes(i.e float data and uliData into char Array)*/
SplitByte_TypeDef SplitByte;				
uint16_t uiHBTimeCounter 				= 0;				/*Heart beat time counter variable*/
uint8_t ucLEDBlinkCount 				= 0;				/*LED blick tim counter*/
uint8_t ucMode 									= 0;		    					/*varible to store the current mode of system*/
uint32_t uliConfigParaByte 			= 0;			/*varible to check the all configuration parameters received or not*/
uint8_t ucValveFault 						= 0x00;				/*variable to store valve fault*/
uint8_t ucPrevValveFault 				= 0;				/*variable to store Previous valve fault*/
uint8_t ucValveFaultHistory 		= 0;		/*variable to use for Valve fault send only once*/
uint16_t uiIOExpader1 					= 0;					/* Current Variable for I/O Expander 1*/
uint16_t uiIOExpader2 					= 0;					/* Current Variable for I/O Expander 2*/
uint16_t uiPreIOExpader1 				= 0;				/* Previous Variable for I/O Expander 1*/
uint16_t uiPreIOExpader2 				= 0;				/* Previous Variable for I/O Expander 2 */
uint16_t uiI2CWriteData 				= 0;				/* Previous Variable for I/O Expander 2 */
uint8_t ucI2C1Seq								=	0;
uint16_t uiBatteryReadCount 		= 0;		/*Counter to read Battery status*/
uint16_t uiGunDelayCnt 					= 0;					/*Gun delay count while OPMM operating on Battery*/

uint16_t uiValveReadCounter 		= 0;		/*Counter to read valve status*/
uint16_t uiGunPositionCounter   = 0;	/*Counter to check the gun position*/
uint16_t uiGunDelayCounter 			= 0;			/*Gun Delay counter*/
		
uint16_t uiSprayOverlapCounter 	= 0;	/*counter to check the spray overlap*/
uint16_t uiReverseDelayCounter	= 0;	/*couner to check the reverse delay*/
uint16_t uiBackHlsCounter 			= 0;			/*back to HOME position counter*/
uint8_t ucCurrOnOff 						= 0;						/*current ON/OFF status of valve*/
uint8_t ucPrevOnOff							= 0;						/*previous status of valve*/
uint8_t ucValveFBState					= 0;							/*current Feedback status of valve*/
uint8_t ucValvePrevFBState			= 0;					/*Previos Feedback status of valve*/

uint16_t uiDclsToHomeFirstCnt 	= 0;	/*DCLS to HOME first rotation counter to check fault*/
/*1st rotation No limit switch fault*/
uint16_t uiNoLimitSwitchCnt 		= 0;
uint16_t uiAutoFlushCnt 				= 0;
uint8_t ucPOSTLEDBlinkCnt				= 0;
uint16_t uiWDTCount							= 0;
TypeDef_NC_TS stNCTS[NBR_OF_NCTS];
uint8_t ucNCTSConnected 				= 0;				/*indication for the Connected Temp sensor*/
uint8_t ucNCTSRead 							= 0;							/*this will tell which sensor should be read*/
uint8_t ucReadNCTSCmd 					= 0;					/*NC TS read command Count*/
uint16_t uiTempReadCnt 					= 0; 				/*Counter for reading Temp of Material A and B*/
uint16_t uiTimCount 						= 0; 						/*Counter to Send Current data to SCU*/
uint8_t ucIOExpSlaveAddr 				= 0;
uint8_t ucLSWDetect 						= 0;						/*store the limit switch pressed bit*/
float fBatKneeVoltage   				= 0.0f;				/*Battery Knee voltage to Warn on Buzzer and LED as well*/
uint8_t uc10msCnt								= 0;

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
 **@Function 			: fnValve_ONOFF
 **@Descriptions	: turn on/off LED for valve
 **@Parameters		: bState : ON = Bit_SET , OFF = Bit_RESET
										ucStructVal : Structure array Position/Number
 **@Return				: None
 *****************************************************************************/
void fnValve_ONOFF(uint8_t ucStructVal, BitAction bState)
{
	/*Only Valve OFF or no Valve fault*/
	if((Bit_RESET == bState) || !(ucValveFaultHistory & (0x01 << ucStructVal)))
	{
		GPIO_WriteBit(VALVE_DRIVE_PORT, 
									VALVE_DRIVE_PIN(stValve[ucStructVal].ucPosition), bState);

		/*Update valve status in the Variable*/
		ucCurrOnOff &= ~(0x01 << ucStructVal);
		ucCurrOnOff |= (uint8_t)(bState<< ucStructVal);
		#ifdef VALVE_DBG
		DEBUG_PRINTF("Struct: %d, ON/OFF: %d\r\n",ucStructVal, bState);
		#endif
		/*LED Update for the given Valve only if no Fault*/
		if(!(ucValveFaultHistory & (0x01 << ucStructVal)))
		{
			if(MBACKWARD == ucStructVal)
			{
				uiIOExpader2 &= ~VALVE_LED_VALUE(ucStructVal, RED);/*FAULT/RED LED OFF*/
				if(SET == bState)/*GREEN LED ON*/
				{
					uiIOExpader2 |= VALVE_LED_VALUE(ucStructVal, GREEN); 
				}
				else/*GREEN LED OFF*/
				{
					uiIOExpader2 &= ~VALVE_LED_VALUE(ucStructVal, GREEN);
				}
			}
			else
			{
				uiIOExpader1 &= ~VALVE_LED_VALUE(ucStructVal, RED);/*FAULT/RED LED OFF*/
				if(SET == bState)/*GREEN LED ON*/
				{
					uiIOExpader1 |= VALVE_LED_VALUE(ucStructVal, GREEN); 
				}
				else/*GREEN LED OFF*/
				{
					uiIOExpader1 &= ~VALVE_LED_VALUE(ucStructVal, GREEN);
				}
			}
		}
	}
	/*for reading the feedback only when its status changes*/
	StatusFlag.stStatusFlag.bValveReadFlag = SET;
	uiValveReadCounter = 0;
}

/*****************************************************************************
 **@Function 			: fnDefault_Config
 **@Descriptions	: Reset all GPIOS to default state (Tricolor status LEDs, 
										valves and GPIOs) 
 **@Parameters		: None 
 **@Return				: None
 *****************************************************************************/
void fnDefault_Config(void)
{
	uint16_t uiTDataRd = 0;
	uint16_t uiTDataWr = DEFAULT_SETTING;
	uint8_t ucLoop = 0;
	
	/* MCP CAN2 ENABLE SET*/
  GPIO_WriteBit(MCP_CAN2_GPIO_PORT, MCP_CAN2_ENABLE, Bit_SET); 	
	/* MCP CAN2 STROBE SET*/
	GPIO_WriteBit(MCP_CAN2_GPIO_PORT, MCP_CAN2_STROBE, Bit_SET); 	

	/* CAN1 ENABLE High */
	GPIO_WriteBit(CAN1_ENABLE_PORT, CAN1_ENABLE, Bit_SET); 	
	/*CAN1 STROBE High */
	GPIO_WriteBit(CAN1_STROBE_PORT, CAN1_STROBE, Bit_SET); 	

	/*VBAT_ENABLE pin high to enable the Vbat*/
  GPIO_WriteBit(VBAT_ENABLE_PORT, VBAT_ENABLE_PIN, Bit_SET);	
	
	/* Set the Reset pin it is Active Low*/ 
	GPIO_SetBits(MCP_CAN2_RESET_PORT, MCP_RESET_PIN); 
	
	uiIOExpader1 = IOEX1_ALL_RED;
	uiIOExpader2 = IOEX2_ALL_RED;
		
	for(ucLoop = 0; ucLoop < NO_OF_TC_SENSOR; ucLoop++)
	{	
		ADS1118_SELECT(ucLoop); /*CS asserts low*/
		
		uiTDataWr |= TS_MODE; //Setting  TS = 1 for ADC
		uiTDataRd = fnTCSPI_ReadWrite(uiTDataWr);
		uiTDataRd = fnTCSPI_ReadWrite(uiTDataWr);
		
		ADS1118_UNSELECT(ucLoop);  /*CS High*/
		if(uiTDataRd == uiTDataWr)
		{
			#ifdef GENERAL_DBG
				DEBUG_PRINTF("Config Reg[%d] : 0x%08x, 0x%04x\r\n", ucLoop
				uiTDataRd, DEFAULT_SETTING);
			#endif
		}
	}

}

/******************************************************************************
 **@Function 			: fnAllFlags_Reset
 **@Descriptions	: Reset all flags to the default value
 **@Parameters		: None 
 **@Return				: None
 ****************************************************************************/
void fnAllFlags_Reset(void)
{
	uint8_t  ucLoop = 0;
	for(ucLoop = 0; ucLoop < 4; ucLoop++)
	{
		/*RESET All Status Flags*/
		StatusFlag.rguliData[ucLoop] = 0;
	}
	
	/* MCP CAN2 Deactivated */
	StatusFlag.stStatusFlag.bMCPCAN2ActiveFlag = SET; 
	
	/* CAN1 Activated */
	StatusFlag.stStatusFlag.bCAN1ActiveFlag = RESET;
}

/******************************************************************************
 **@Function 			: fnTempSensor_Read
 **@Descriptions	: Function to get Tempreture value from temperature sensors
 **@Parameters		: None
 **@Return				: None
 ****************************************************************************/
void fnTempSensor_Read(void)
{
	uint8_t ucLoop = 0;
	float fReadData = 0.0f;
	#ifdef NCTS_DBG
		char rgucTemp[60];
	#endif
	
	for(ucLoop = 0; ucLoop < NBR_OF_NCTS; ucLoop++)
	{
		if((0x1 << ucLoop) & ucNCTSConnected)
		{
			/*Value of the Temperature sensor 1*/
			/*Temperature sensor at 3 position*/
			fReadData = fnADC_Read(TEMP_ADC_NUMBER, 
						NCTS_ADC_CH(stNCTS[ucLoop].ucPosition));
			
			
			#ifdef NCTS_DBG
				sprintf(rgucTemp,"Read voltage %d = %0.2f\r\n",ucLoop, fReadData);	
				DEBUG_PRINTF("%s", rgucTemp); 
			#endif
			
			/*converting temperature from Voltage to C and 0.6 voltage corresponds
				to 0 C and 3.0V corresponds to 250 C*/
			fReadData = (((fReadData - LOW_TEMPERATURE_V) * 
												TEMPERATURE_SLOPE) + LOW_TEMPERATURE_LEVEL);
			
			#ifdef NCTS_DBG
					sprintf(rgucTemp, "Temperature = %0.2f \r\n", fReadData);	
					DEBUG_PRINTF("%s", rgucTemp ); 
			#endif 
		}
		if((0x1 << ucLoop) & ucNCTSRead)
		{
			//DEBUG_PRINTF("Stored in = %d\r\n", ucLoop);
			stNCTS[ucLoop].fTemperature = fReadData;
		}
	}		
}

/*****************************************************************************
 **@Function 		: fnDefault_Mode
 **@Description	: this function Contains Default Mode Fucntionality
									Default Mode: Enable Timer if First Heartbeat rcv,
									Diagnostics Test, After Diagnostics Pass Swtich the 
									Mode To Operation Mode														
 **@Parameters	: None
 **@Return			: None
 ***************************************************************************/
void fnDefault_Mode(void)
{
	if(DEFAULT_MODE == ucMode)
	{
		#ifdef DBG_CAN      
			DEBUG_PRINTF(" parabyte = %04x \r\n", uliConfigParaByte);
		#endif
		
		/* it will check for first heartbeat only*/
		if((SET == StatusFlag.stStatusFlag.bHeartBeatMsgFlag) && 
				(RESET == StatusFlag.stStatusFlag.bFirstHrtBtMsgFlag)) 
		{
			StatusFlag.stStatusFlag.bHeartBeatMsgFlag = RESET;
			
			/* Setting the first heart beat flag */
			StatusFlag.stStatusFlag.bFirstHrtBtMsgFlag = SET;
			
			/*Timer 2 Enable for Limit Switch Scanning*/
			TIM_Cmd(TIM1, ENABLE);
			TIM_Cmd(TIM2, ENABLE);

			
			#ifdef GENERAL_DBG
				DEBUG_PRINTF("First Heartbeat Received \r\n");
			#endif
		}
		
		/*Read thermocouple in interval of 50msec after first Hearbeat*/
		if((TEMP_READ_TIME <= uiTempReadCnt) && 
			 (RESET == StatusFlag.stStatusFlag.bTCProcessed))
		{
			uiTempReadCnt = 0;
			fnReadTemp();
		}
		/* it will check for Device Functionality Command*/
		if((RESET == StatusFlag.stStatusFlag.bDiagnosticsFlag) &&
			 (CONFIG_PARA_CNT == uliConfigParaByte) && 
			 (SET == StatusFlag.stStatusFlag.bFirstHrtBtMsgFlag))
		{	  
			/* Self Diagnost and if Pass then send Pass status and Config
				CMD if fail then send Fail Status */
			if(SET == StatusFlag.stStatusFlag.bTCProcessed)
			{
				uiIOExpader1 = 0;
				uiIOExpader2 = 0;
				ucPOSTLEDBlinkCnt = 0;
				fnSelf_Diagnostic();
				StatusFlag.stStatusFlag.bDiagnosticsFlag = SET;
			}
		}
	
		/*after Diagnostic pass and Config parameters received, Switch
			from Default Mode to Normal Mode */
		if((SET == StatusFlag.stStatusFlag.bDiagnosticsPassFlag))  
		{		
			StatusFlag.stStatusFlag.bTCProcessed = RESET;
			ucMode = OPERATION_MODE; 
					
			uiTempReadCnt = 0;

			#ifdef GENERAL_DBG
				DEBUG_PRINTF("!!!!Switching from default to Operation mode\r\n");
			#endif
		}
	}
}


/******************************************************************************
 **@Function 		: fnOperation_Mode
 **@Descriptions: This function contain Operation mode funtionality and 
									all Processes and valve status
 **@arameters		: None
 **@Return			: None
 *****************************************************************************/
void fnOperation_Mode(void)
{
	uint8_t ucLoop = 0;

	if(OPERATION_MODE == ucMode)
	{
		
		if((TEMP_READ_TIME <= uiTempReadCnt) && 
			 (RESET == StatusFlag.stStatusFlag.bTCProcessed))
		{
			uiTempReadCnt = 0;
			fnReadTemp();
		}
		if(DATA_READ_TIME <= uiTimCount) /*condition check for Data Read Time */
		{
			#ifdef GENERAL_DBG
				DEBUG_PRINTF("OP\r\n");
			#endif
			uiTimCount = 0;
			for(ucLoop = 0; ucLoop < NO_OF_TC_SENSOR; ucLoop++)
			{
				fnTemprature_Process(ucLoop);
			}
			/*RESET for data reading from thermocouple*/
			StatusFlag.stStatusFlag.bTCProcessed = RESET;
			ucCReading = 0;			
		}
		/************************** VALVE ACTIONS *************************/
		if(RESET == StatusFlag.stStatusFlag.bEStopFlag)
		{
			if(VALVE_READ_TIME <= uiValveReadCounter)
			{
	//			DEBUG_PRINTF("RV\r\n");
				/*Read the status of each valve after VALVE_READ_TIME*/
				ucValveFBState = 0;
				for(ucLoop = 0; ucLoop < TOTAL_VALVES; ucLoop++)
				{
					ucValveFBState |= (GPIO_ReadInputDataBit(
										VALVE_FEEDBACK_PORT(stValve[ucLoop].ucPosition),
										VALVE_FEEDBACK_PIN(stValve[ucLoop].ucPosition)) << ucLoop);
					
				}
				
				/*ON/OFF and Feedback status of valve should be same if not then fault*/
				ucValveFault = ucCurrOnOff ^ ucValveFBState;
				/*Check for Valve Fault*/
				/*and delay between the valve ON/OFF and Reading status should be elapse*/
				/*Cond1=> if any fault
					Cond2=> previous fault and current fault should be differ
					Cond3=> current fault should be first time only*/
				if((0 != ucValveFault) && (ucValveFault != ucPrevValveFault) &&
					 (ucValveFault ^ ucValveFaultHistory))
				{
					/*Take action on Valve fault when ESTOP Not Pressed*/
					if(RESET == StatusFlag.stStatusFlag.bEStopFlag)
					{
							
						#ifdef VALVE_DBG
						DEBUG_PRINTF("ucValveFault: %d, ucCurrOnOff: %d, ucValveFBState: %d \r\n",
													ucValveFault, ucCurrOnOff, ucValveFBState );
						#endif
						/*this is used to store Valve Fault History*/
						ucValveFaultHistory |= ucValveFault;
						
						memset(stTxMsg.Data, 0 , sizeof(stTxMsg.Data));
						stTxMsg.DLC = 1;	
						stTxMsg.Data[0] = ucValveFault;
						/*************VALVE Fault Action*************/
						/*Spray Gun Valve Off if any fault occur*/
						fnValve_ONOFF(SPRAY_GUN, VALVE_OFF);
						
						fnCANMessage_Send(FAULT_OPMM_VALVE, MSG_TYPE_FAULT);

						ucPrevValveFault = ucValveFault;
						#ifdef VALVE_DBG
							DEBUG_PRINTF("FAULT ACTION\r\n");
						#endif
						/*Fault LED Action for Faulty Valve*/
						for(ucLoop = 0; ucLoop < TOTAL_VALVES; ucLoop++)
						{
							if((0x01 << ucLoop) & ucValveFault)
							{
								/* LED */
								if(MBACKWARD == ucLoop)
								{
									uiIOExpader2 |= VALVE_LED_VALUE(ucLoop, RED); /*FAULT/RED LED ON*/
									uiIOExpader2 &= ~VALVE_LED_VALUE(ucLoop, GREEN);/*GREEN LED OFF*/
								}
								else
								{
									uiIOExpader1 |= VALVE_LED_VALUE(ucLoop, RED); /*FAULT/RED LED ON*/
									uiIOExpader1 &= ~VALVE_LED_VALUE(ucLoop, GREEN); /*GREEN LED OFF*/
								}
							}
						}
					}
					/*RESET the Counter for next reading of Feedback status and fault check*/
					uiValveReadCounter = 0;			
				}
			}
		}
		/*If ESTOP is pressed then check for ALL valves used in FLUSH are ON or not if yes then send flush done 
		  status to inform that Manual flush has been take place*/
		else if(RESET == StatusFlag.stStatusFlag.bSendFlushStatusOnEstop)
		{
			ucValveFBState = 0;
			for(ucLoop = 0; ucLoop < TOTAL_VALVES; ucLoop++)
			{
				ucValveFBState |= (GPIO_ReadInputDataBit(
									VALVE_FEEDBACK_PORT(stValve[ucLoop].ucPosition),
									VALVE_FEEDBACK_PIN(stValve[ucLoop].ucPosition)) << ucLoop);
			}
			
				if(((1 << SPRAY_GUN) | (1 << FLUSH_MAT) | (1 << SPRAY_HEAD)) == 
					(ucValveFBState &  (((1 << SPRAY_GUN) | (1 << FLUSH_MAT) | (1 << SPRAY_HEAD)))))
				{
					/**************** Send Flush Status as pass to MCU if ESTOP is Pressed*************/
					memset(stTxMsg.Data, 0 , sizeof(stTxMsg.Data));
					stTxMsg.DLC = 1;
					stTxMsg.Data[0] = PROCESS_PASS;
				
					#ifdef FLUSH_DBG
						DEBUG_PRINTF("Flush process ON ESTOP!!\r\n");
					#endif
					/*send status on CAN*/
					fnCANMessage_Send(STATUS_FLUSH_PROCESS, MSG_TYPE_STATUS);
					StatusFlag.stStatusFlag.bSendFlushStatusOnEstop = SET;
				}
			
		}

		/*Send status if any valve altered*/
		if(ucCurrOnOff != ucPrevOnOff)
		{
			#ifdef VALVE_DBG
				DEBUG_PRINTF("Status send : %x\r\n",ucCurrOnOff);
			#endif
			/*Send valve status to SCU and HMI*/
			stTxMsg.DLC = 1;
			stTxMsg.Data[0] = ucCurrOnOff;
			fnCANMessage_Send(STATUS_OPMM_VALVE, MSG_TYPE_STATUS);
			ucPrevOnOff = ucCurrOnOff;
			
		}
		/*check for the OPMM set on Pipe or Not*/
		if(SET ==  StatusFlag.stStatusFlag.bOpmmSetOnPipe) 
		{
			StatusFlag.stStatusFlag.bOpmmSetOnPipe = RESET;
			fnTempSensor_Read();
			fnTempStatus_Send();
		}
	
		/*Prime Process*/
		if(SET == StatusFlag.stStatusFlag.bInitPrimeProcess)
		{
			fnPrime_Process();
		}
		
		/*Spray Process*/
		if(SET == StatusFlag.stStatusFlag.bInitSprayProcess)
		{
			fnSpray_Process();
		}
		
		/*Flush*/
		if(SET == StatusFlag.stStatusFlag.bInitFlushProcess)
		{
			fnFlush_Process();
		}	
		
		if((SET == StatusFlag.stStatusFlag.bSystemErrorFlag) && 
			 (SET == StatusFlag.stStatusFlag.bHLSReachedOnError) && 
			 (SET == StatusFlag.stStatusFlag.bMtoHomeAutoFlush))
		{
			fnAutoFlush();
		}
		
		if((SET == StatusFlag.stStatusFlag.bSystemErrorFlag) && 
			 (SET == StatusFlag.stStatusFlag.bVBatPrimeStartFlag) && 
			 (RESET == StatusFlag.stStatusFlag.bSprayGoingOn))
		{
			StatusFlag.stStatusFlag.bHLSReachedOnError = SET;/*Prime is started is set and system error*/
			
			//DEBUG_PRINTF("Prime is started and system error\n\r");
		}
		
	
		if((RESET == StatusFlag.stStatusFlag.bCAN1ActiveFlag) && 
			 (RESET == StatusFlag.stStatusFlag.bMCPCAN2ActiveFlag))
		{
			StatusFlag.stStatusFlag.bSystemErrorFlag = SET;/*both can is fail*/
		}
		
		/*Limit Switch Pressed Status to SCU*/
		if(SET == StatusFlag.stStatusFlag.bLSWPressed)
		{
			StatusFlag.stStatusFlag.bLSWPressed = RESET;
			
			memset(stTxMsg.Data, 0 , sizeof(stTxMsg.Data));
			stTxMsg.DLC = 1;
			stTxMsg.Data[0] = ucLSWDetect;
			
			fnCANMessage_Send(STATUS_LSW_PRESSED, MSG_TYPE_STATUS);
		}
		
		if(uiBackHlsCounter >= uiStopMotorRotTimer)
		{
			uiBackHlsCounter = 0;
			StatusFlag.stStatusFlag.bBackHlsTimer = RESET;
			fnValve_ONOFF(MBACKWARD, VALVE_OFF);
			
			/*Take Spray Head Away from Pipe*/
			fnValve_ONOFF(SPRAY_HEAD, VALVE_ON);
			
			/*Do we need AutoFlush when Stop and Flush Button pressed?*/
			if(RESET == StatusFlag.stStatusFlag.bMoveToHomePos)
			{
				if(SET == StatusFlag.stStatusFlag.bSystemErrorFlag)
				{
					/*SystemError and stop and flush is pressed during spray*/
					StatusFlag.stStatusFlag.bHLSReachedOnError = SET;
				}
			}
		}
		
		
		if((SET == StatusFlag.stStatusFlag.bMoveToHomePos) && (RESET == StatusFlag.stStatusFlag.bEStopFlag))
		{
			if(SET == StatusFlag.stStatusFlag.bDCLSPosMTH)
			{
				StatusFlag.stStatusFlag.bDCLSPosMTH = RESET;
				ucDCLSHitCntToHome++;
//				DEBUG_PRINTF("ucDCLSHitCntToHome.. %d\r\n",ucDCLSHitCntToHome);
				/*Send the DCLS hit counter to the MCU*/
				stTxMsg.DLC = 3;
				stTxMsg.Data[0] = ucDCLSHitCntToHome;
				stTxMsg.Data[1] = ucHLSHitCntToHome;
				stTxMsg.Data[2] = ucRotationCntBackup;
				fnCANMessage_Send(DATA_DCLS_HIT_COUNTER, MSG_TYPE_DATA);
			}
			else if(SET == StatusFlag.stStatusFlag.bHLSPosMTH)
			{
				
				StatusFlag.stStatusFlag.bHLSPosMTH = RESET;
				ucHLSHitCntToHome++;
				if(1 == ucHLSHitCntToHome)
				{
					ucDCLSHitCntToHome = 0;
				}
	//			DEBUG_PRINTF("ucHLSHitCntToHome.. %d\r\n",ucHLSHitCntToHome);
				/*Send the DCLS hit counter to the MCU*/
				stTxMsg.DLC = 3;
				stTxMsg.Data[0] = ucDCLSHitCntToHome;
				stTxMsg.Data[1] = ucHLSHitCntToHome;
				stTxMsg.Data[2] = ucRotationCntBackup;
				fnCANMessage_Send(DATA_DCLS_HIT_COUNTER, MSG_TYPE_DATA);
			}
			switch(ucRotationCntBackup)
			{
				
				case 3:
				{
					fnValve_ONOFF(MFORWARD, VALVE_OFF);
					fnValve_ONOFF(MBACKWARD, VALVE_ON);
					if( (1 == ucDCLSHitCntToHome) && (1 == ucHLSHitCntToHome))
					{
						ucDCLSHitCntToHome = 0;
						ucHLSHitCntToHome = 0;
						StatusFlag.stStatusFlag.bMoveToHomePos = RESET;
						StatusFlag.stStatusFlag.bMtoHomeAutoFlush = SET;/*Adf*/
						StatusFlag.stStatusFlag.bBackHlsTimer = SET;
							 
							//DEBUG_PRINTF("bMtoHomeAutoFlush.. set\r\n");
						
					}
				}					
				break;
				case 5:
				case 2:				
				case 6:
					{
						fnValve_ONOFF(MFORWARD, VALVE_OFF);
						fnValve_ONOFF(MBACKWARD, VALVE_ON);
						if(1 == ucDCLSHitCntToHome)
						{
							ucDCLSHitCntToHome = 0;
							ucHLSHitCntToHome = 0;
							StatusFlag.stStatusFlag.bMoveToHomePos = RESET;
							StatusFlag.stStatusFlag.bMtoHomeAutoFlush = SET;/*Adf*/
							StatusFlag.stStatusFlag.bBackHlsTimer = SET;
						     
								//DEBUG_PRINTF("bMtoHomeAutoFlush.. set\r\n");
							
						}
					}
					break;
				case 4:
					{
						fnValve_ONOFF(MFORWARD, VALVE_OFF);
						fnValve_ONOFF(MBACKWARD, VALVE_ON);
						if((1 == ucDCLSHitCntToHome) && (1 == ucHLSHitCntToHome))
						{
							ucDCLSHitCntToHome = 0;
							ucHLSHitCntToHome = 0;
							StatusFlag.stStatusFlag.bMoveToHomePos = RESET;
							StatusFlag.stStatusFlag.bMtoHomeAutoFlush = SET;
							StatusFlag.stStatusFlag.bBackHlsTimer = SET;
							    
								//DEBUG_PRINTF("bMtoHomeAutoFlush set..\r\n");
					
						}
					}
					break;
			}
			
			/*for safty if any of the DCLS or HLS hit counter become more than 2 then stop this process*/
			if((1 < ucDCLSHitCntToHome) || (1 < ucHLSHitCntToHome))
			{
				ucDCLSHitCntToHome = 0;
				ucHLSHitCntToHome = 0;
				StatusFlag.stStatusFlag.bMoveToHomePos = RESET;
				StatusFlag.stStatusFlag.bMtoHomeAutoFlush = SET;
				StatusFlag.stStatusFlag.bBackHlsTimer = SET;
						
					//DEBUG_PRINTF("bMtoHomeAutoFlush set..\r\n");
			}
			
		}/*Move to home position*/
		/*adf 21/10/2016*/
//		if((uiFlushWarnCnt >= uiFlushWarnTime) && (SET == StatusFlag.stStatusFlag.bFlushWarnTime))
//		{
//			uiFlushWarnCnt = 0;
//			BUZZER_ON();
//		}
	}
}

/******************************************************************************
 **@Function		 	:  fnAllWarnnings_Clear
 **@Descriptions	:  this funtion clear all the Warnning flags
 **@Parameters		:  None
 **@Return				:  None
 *****************************************************************************/
void fnAllWarnnings_Clear(void)
{
	ucValveFaultHistory = 0; /*Clear all Valve fault to enable the LED Toggle*/
	ucPrevValveFault    = 0;
	ucValveFault				=	0;
	/*Turn OFF all the valves*/
	fnValve_ONOFF(SPRAY_GUN, VALVE_OFF);
	fnValve_ONOFF(SPRAY_HEAD, VALVE_OFF);
	fnValve_ONOFF(AB_MAT, VALVE_OFF);
	fnValve_ONOFF(FLUSH_MAT, VALVE_OFF);
	fnValve_ONOFF(MFORWARD, VALVE_OFF);
	fnValve_ONOFF(MBACKWARD, VALVE_OFF);
	
}
	
/******************************************************************************
 **@Function		 	:  fnAllFaults_Clear
 **@Descriptions	:  this function Claer all the faults flags
 **@Parameters		:  None
 **@Return				:  None
 *****************************************************************************/
void fnAllFaults_Clear(void)
{
	uiCANCount = 0;
	StatusFlag.stStatusFlag.bSystemErrorFlag = RESET;
	StatusFlag.stStatusFlag.bEStopReceived = RESET;
	
	StatusFlag.stStatusFlag.bCANFaultSCU = RESET;
	StatusFlag.stStatusFlag.bCANFaultHMI = RESET;
	StatusFlag.stStatusFlag.bCANFaultFlag = RESET;
	StatusFlag.stStatusFlag.bHBNotRcvFlag = RESET;
	StatusFlag.stStatusFlag.bHLSRotationFault = RESET;
	StatusFlag.stStatusFlag.bDCLSRotationFault = RESET;
	StatusFlag.stStatusFlag.bPrimeFault = RESET;
	StatusFlag.stStatusFlag.bSprayFault = RESET;
	StatusFlag.stStatusFlag.bFlushFault = RESET;
	StatusFlag.stStatusFlag.bAutoFlushOnError = RESET;
	StatusFlag.stStatusFlag.bAutoFlushByTime 		= RESET;
	ucValveFaultHistory = 0; /*Clear all Valve fault to enable the LED Toggle*/
	ucPrevValveFault    = 0;
	ucValveFault				=	0;
	/*Turn OFF all the valves*/
	fnValve_ONOFF(SPRAY_GUN, VALVE_OFF);
	fnValve_ONOFF(SPRAY_HEAD, VALVE_OFF);
	fnValve_ONOFF(AB_MAT, VALVE_OFF);
	fnValve_ONOFF(FLUSH_MAT, VALVE_OFF);
	fnValve_ONOFF(MFORWARD, VALVE_OFF);
	fnValve_ONOFF(MBACKWARD, VALVE_OFF);
						
						
	//StatusFlag.stStatusFlag.bTowardPipePosition = RESET;
	//StatusFlag.stStatusFlag.bAwayPipePosition = RESET;
	
	StatusFlag.stStatusFlag.bDCLSPosition = RESET;
	StatusFlag.stStatusFlag.bHomePosition = RESET;

	uiIOExpader2 &= (~BOARD_STATUS_RED_LED);
	uiIOExpader2 &= (~BOARD_STATUS_BLUE_LED);
	uiIOExpader2 |= BOARD_STATUS_GREEN_LED;
	
	StatusFlag.stStatusFlag.bCAN1ActiveFlag = RESET;
	StatusFlag.stStatusFlag.bMCPCAN2ActiveFlag = SET;
	
	
	StatusFlag.stStatusFlag.bEStopFlag = RESET;
	StatusFlag.stStatusFlag.bSendFlushStatusOnEstop = RESET; /*RESET this flag on Estop release*/ 

	/*Need to add all the Flags whichever is needed*/
	StatusFlag.stStatusFlag.bMoveToHomePos = RESET;
	StatusFlag.stStatusFlag.bMtoHomeAutoFlush = RESET;/*Adf*/
	StatusFlag.stStatusFlag.bBackHlsTimer = RESET;
	StatusFlag.stStatusFlag.bHomePosition = RESET;
	ucHLSHitCntToHome = 0;
	ucDCLSHitCntToHome = 0;
	
		
}

/*****************************************************************************
 **@Function		 	:  fnEstop_Check
 **@Descriptions	:  Estop message check
 **@Parameters		:  None
 **@Return				:  None
 ****************************************************************************/
void fnEstop_Check(void)
{
	if(SET == StatusFlag.stStatusFlag.bEStopFlag)
	{
		//StatusFlag.stStatusFlag.bEStopFlag = RESET;
/*	StatusFlag.stStatusFlag.bEStopReceived = SET;*/
		
		if(SET == StatusFlag.stStatusFlag.bInitPrimeProcess)
		{
			StatusFlag.stStatusFlag.bPrimeFault = SET;
		}
		if(SET == StatusFlag.stStatusFlag.bInitSprayProcess)
		{
			StatusFlag.stStatusFlag.bSprayFault = SET;
			ucSprayFaultStatus |= ESTOP_PRESSED_FAULT;/*adf 21/10/2016*/
			//DEBUG_PRINTF("ucSprayFaultStatus %d\r\n",ucSprayFaultStatus);
		}

		/* EStop Functionality */
		#ifdef GENERAL_DBG
			DEBUG_PRINTF("ESTOP Received \r\n");
		#endif
		
		
		/*Turn OFF material Valve*/
		fnValve_ONOFF(AB_MAT, VALVE_OFF);
	}
}


/******************************************************************************
 **@Function 	 : fnHearBeat_Check
 **@Description: This function will Check HeartBeat Reception
 **@Parameters : None
 **@Return		 : None
 ****************************************************************************/
void fnHearBeat_Check(void)
{
//	if(HEARTBEAT_TIMEOUT <= uiHBTimeCounter)
//	{
//		uiHBTimeCounter = 0;
//		uiCANCount++;
//		if(CAN_FAULT_COUNT <= uiCANCount)
//		{
//			StatusFlag.stStatusFlag.bCANFaultFlag = SET;
//			StatusFlag.stStatusFlag.bCANFaultSCU = SET;
//			StatusFlag.stStatusFlag.bMCPCAN2ActiveFlag = RESET;
//			StatusFlag.stStatusFlag.bCAN1ActiveFlag = RESET;
//					
//			#ifdef GENERAL_DBG	
//				DEBUG_PRINTF("CHECK HB,BOTH LINK FAIL SET COUNT: %d\r\n", uiCANCount);
//			#endif
//		}
//		if(RESET == StatusFlag.stStatusFlag.bHBNotRcvFlag)
//		{		
//			/* this is used to send the fault only once*/
//			StatusFlag.stStatusFlag.bHBNotRcvFlag = SET;
//			stTxMsg.DLC = 0;
//			memset(stTxMsg.Data, 0, sizeof(stTxMsg.Data));
////			fnCANMessage_Send(FAULT_HB_NOT_RECEIVED, MSG_TYPE_FAULT, OP);
//		}
//	}
}

/******************************************************************************
 **@Function 			: fnSelf_Diagnostic
 **@Descriptions	: This funtion will do self diagostic test and sends the 
										Diagnostics Status to SCU and OP
 **@Parameters		: None
 **@Return				: None
 *****************************************************************************/
void fnSelf_Diagnostic(void)
{
	uint8_t ucLoop = 0;
	uint8_t ucStatus1 = 0x00;			/*valve connected status*/
	uint8_t ucStatus2 = 0x00;			/*valve functionality status*/
	uint8_t ucStatus3 = 0x00;			/*temp sensor status*/
	
	/*OPMM Diagnostic Test*/
	for(ucLoop = 0; ucLoop < TOTAL_VALVES; ucLoop++)
	{
		/*Getting Status of valve connected*/
		ucStatus1 |= (uint8_t)(GPIO_ReadInputDataBit(VALVE_CONN_STAT_PORT,
											VALVE_CONN_STAT_PIN(stValve[ucLoop].ucPosition)) << ucLoop);
	}
	/* invert the read connected status of valves*/
	ucStatus1 = ((~ucStatus1) & 0x3F);
	#ifdef DIAG_DBG
		DEBUG_PRINTF("******* :%x\r\n\r\n", ucStatus1);
	#endif
	//if(0x00 == ucStatus1)
	{
		/*Turn OFF all the valves*/
		for(ucLoop = 0; ucLoop < TOTAL_VALVES; ucLoop++)
		{
			fnValve_ONOFF(ucLoop, VALVE_OFF);
		}
		
		
		fnDelay_ms(10);
		
		/*READ Valve Status and send it in ucStatus2*/
		ucStatus2 = 0;
		
		/*Add Bypass Logic here*/
		for(ucLoop = 0; ucLoop < TOTAL_VALVES; ucLoop++)
		{
			ucStatus2 |= (GPIO_ReadInputDataBit(
										VALVE_FEEDBACK_PORT(stValve[ucLoop].ucPosition),
										VALVE_FEEDBACK_PIN(stValve[ucLoop].ucPosition)) << ucLoop);
		}
		
		#ifdef DIAG_DBG
			DEBUG_PRINTF(" Valve Status :%x\r\n", ucStatus2);
		#endif
	}
	if(0x00 != ucStatus1)
	{
		for(ucLoop = 0; ucLoop < TOTAL_VALVES; ucLoop++)
		{
			if(0x01 == ((ucStatus1 >> ucLoop) & 0x01))
			{
				if(MBACKWARD == ucLoop)
				{
					uiIOExpader2 |= VALVE_LED_VALUE(ucLoop, RED);/*FAULT/RED LED OFF*/
					uiIOExpader2 &= ~VALVE_LED_VALUE(ucLoop, BLUE);/*FAULT/RED LED OFF*/
					uiIOExpader2 &= ~VALVE_LED_VALUE(ucLoop, GREEN);/*FAULT/RED LED OFF*/
				}
				else
				{
					uiIOExpader1 |= VALVE_LED_VALUE(ucLoop, RED);/*FAULT/RED LED OFF*/
					uiIOExpader2 &= ~VALVE_LED_VALUE(ucLoop, BLUE);/*FAULT/RED LED OFF*/
					uiIOExpader2 &= ~VALVE_LED_VALUE(ucLoop, GREEN);/*FAULT/RED LED OFF*/
				}
			}
		}
	}
//	#ifdef DIAG_DBG
//		DEBUG_PRINTF("BYPASSED Valve Status1 :%x Status2 :%x uiBypass: %x\r\n", ucStatus1, ucStatus2,uiBypass);
//	#endif
	/*Bypassing Valve diagnostic as per the configration sent by SCU*/
	ucStatus1 = ~((uint8_t)uiBypass & 0x3F) & ucStatus1;
	ucStatus2 = ~((uint8_t)uiBypass & 0x3F) & ucStatus2;
	#ifdef DIAG_DBG
		DEBUG_PRINTF("BYPASSED Valve Status1 :%x Status2 :%x uiBypass: %x\r\n", ucStatus1, ucStatus2,uiBypass);
	#endif
	/*Thermocouple Diagnostic*/
	for(ucLoop = 0; ucLoop < NO_OF_TC_SENSOR; ucLoop++)
	{					
		if(INVALID_FLOAT_VALUE == stTCRead[ucLoop].fCurrentTemp) 
		{
			if(RESET == ((uiBypass >> (ucLoop + 12)) & 0x01))
			{
				/*SET Diagnostic status Bits*/
				ucStatus3 |= THERMOCOUPLE_DIAG_FAIL(ucLoop); 
			}		
			
			#ifdef DIAG_DBG
				DEBUG_PRINTF("Thermocouple %d Diagnostics Fail\r\n", ucLoop);
			#endif
		}
		else
		{
			#ifdef DIAG_DBG
				DEBUG_PRINTF("Thermocouple Diagnostics Pass %d\r\n", ucLoop);
			#endif
		}
	}
	/*To read all sensor values*/
	ucNCTSRead = 0x0f;
	/*Read Non-contact Temp Sensor*/
	fnTempSensor_Read();
	ucNCTSRead = 0;
	/*Non contact temp sensor diagnostic*/
	for(ucLoop = 0; ucLoop < NUMBER_OF_TEMP_SENSOR; ucLoop++)
	{
		if((stNCTS[ucLoop].fTemperature < LOW_TEMPERATURE_LEVEL) ||
       (stNCTS[ucLoop].fTemperature > HIGH_TEMPERATURE_LEVEL_REQ))					
		{																												           
			if(RESET == ((uiBypass >> (ucLoop + 8)) & 0x01))
			{
				/*SET Diagnostic status Bits*/
				ucStatus3 |= NON_CONTACT_TEMP_DIAG_FAIL(ucLoop); 
//				DEBUG_PRINTF("NCTS Fail : %d\r\n", ucLoop);
			}			
		}
		else
		{
//			DEBUG_PRINTF("NCTS PASS : %d\r\n", ucLoop);
		}
	} 
	
	/*Bypass Temp diagnostic as per the configration sent by SCU*/
	//ucStatus3 = ~((uint8_t)uiBypass & 0xFF) & ucStatus3;

	
	#ifdef DIAG_DBG
		DEBUG_PRINTF(" Temp Diagnostic Status :%x\r\n", ucStatus3);
	#endif
	
	/*Send Diagnostic Status*/
  fnDiagnosticStatus_Send(ucStatus1, ucStatus2, ucStatus3);
}


/******************************************************************************
 **@Function 		:	 fnTempStatus_Send
 **@Descriptions:  Check the temperature status of non contact temperature 
									 sensors and Transmit on CAN
 **@Parameters	:	 None
 **@Return			:  None
 *****************************************************************************/
void	fnTempStatus_Send(void)
{
	#ifdef NCTS_DBG
		char rgucTemp[200];
	#endif
	float fAvgTemp = 0.0f;						/*varible to store average temperature*/
	uint8_t ucLoop = 0,  ucLoop1 = 0;
	uint8_t ucTempSensorCnt = 0;


	for(ucLoop = 0; ucLoop < NUMBER_OF_TEMP_SENSOR; ucLoop++)
	{
		if((stNCTS[ucLoop].fTemperature > LOW_TEMPERATURE_LEVEL) &&
			 (stNCTS[ucLoop].fTemperature < HIGH_TEMPERATURE_LEVEL_REQ))					
		{																												           
			ucTempSensorCnt++;
			fAvgTemp += stNCTS[ucLoop].fTemperature;
		}
	} 
	
	
	if(0 != ucTempSensorCnt)
	{
		fAvgTemp = fAvgTemp / (float)ucTempSensorCnt;
	}
	else
	{
		fAvgTemp = INVALID_DATA;
	}
	
	#ifdef NCTS_DBG
		sprintf(rgucTemp,"\nAverage Temp = %0.2f\r\n", fAvgTemp);	
		DEBUG_PRINTF("%s",rgucTemp ); 
	#endif
	
	
	for(ucLoop = 0; ucLoop < 5; ucLoop++)
	{
		/*send temperature data to SCU and OP*/
		memset(stTxMsg.Data, 0 , sizeof(stTxMsg.Data));
		stTxMsg.DLC = 5;
		stTxMsg.Data[0] = TEMP_READINGS(ucLoop);
		
		if(4 > ucLoop)
		{
			if(!((stNCTS[ucLoop].fTemperature > LOW_TEMPERATURE_LEVEL) &&
					 (stNCTS[ucLoop].fTemperature < HIGH_TEMPERATURE_LEVEL_REQ)))
			{
				SplitByte.fData = INVALID_DATA;
			}
			else
			{			
				SplitByte.fData = stNCTS[ucLoop].fTemperature;
			}
		}
		else
		{
			SplitByte.fData = fAvgTemp;
		}
		/*Send average Temp data or the */
		if((4 <= ucLoop) || ((0x01 << ucLoop) & ucNCTSRead))
		{
			for(ucLoop1 = 1; ucLoop1 < stTxMsg.DLC; ucLoop1++)
			{
				stTxMsg.Data[ucLoop1] = SplitByte.rgucData[ucLoop1 - 1];
			}
			//DEBUG_PRINTF("Send Pipe Temp %d data %d \r\n", ucLoop, stTxMsg.Data[0]);
			fnCANMessage_Send(DATA_OPMM_PIPE_TEMP, MSG_TYPE_DATA);
		}
	}		
	
	fAvgTemp = 0.0f;
	stNCTS[0].fTemperature 	= 0.0f;
	stNCTS[1].fTemperature	=	0.0f;
	stNCTS[2].fTemperature	=	0.0f;
	stNCTS[3].fTemperature	= 0.0f;
}



/*****************************************************************************
 **@Function 			:	 fnBattery_Status
 **@Descriptions	:  Send Battery Status to SCU and OP
 **@Parameters		:	 None
 **@Return				:  None
 *****************************************************************************/
void fnBattery_Status(void)
{
	float fAdcReadBattery, fAdcReadMainSupply;
	uint8_t ucLoop;
	#ifdef BATTERY_DBG
		char rgucTemp[150];
	#endif
	if(SET == StatusFlag.stStatusFlag.bFirstHrtBtMsgFlag)
	{
		if(BATTERY_READ_COUNT <= uiBatteryReadCount)
		{
			uiBatteryReadCount = 0;
					
			fAdcReadBattery = fnADC_Read(TEMP_ADC_NUMBER, VBAT_STATUS_CH);
			fAdcReadMainSupply = fnADC_Read(ADC_SUPPLY_NUMBER, MAIN_SUPPLY_CH);
			//fBatStatus = fnBatteryStatus_Read(fAdcReadBattery);

			fAdcReadBattery /= VBAT_MUL_FACT; /*Convert the 3V to the 24V voltage readings*/
			
			#ifdef BATTERY_DBG    
				memset(rgucTemp, '\0', sizeof(rgucTemp));
				sprintf(rgucTemp, "Btry Voltage = %f MAIN Supp : %f\r\n", fAdcReadBattery, fAdcReadMainSupply);
				DEBUG_PRINTF("%s", rgucTemp);
			#endif  	
			
			stTxMsg.DLC = 4; /*Only battery data to be send*/
			SplitByte.fData = fAdcReadBattery;/* fBatStatus; //this is commented to send only read volt*/
			/*Send Battery Percentage value*/		
			for(ucLoop = 0; ucLoop < stTxMsg.DLC; ucLoop++)
			{
				stTxMsg.Data[ucLoop] = SplitByte.rgucData[ucLoop];
			}
			
			/*send the battry Status data to the SCU and OP*/
			fnCANMessage_Send(DATA_OPMM_BATTERY_STATUS, MSG_TYPE_DATA);
			
			if((SWITCHING_VBAT_VOLT > fAdcReadMainSupply) ||
				(SET == StatusFlag.stStatusFlag.bVbatSwichFlag))
			{
				if(RESET == StatusFlag.stStatusFlag.bVbatSwichFlag)
				{
					StatusFlag.stStatusFlag.bVbatSwichFlag = SET;
				}	
				/*This will indicate on the Battery status Led that Vbat
					Switching occurs*/
				uiIOExpader2 &= (~BAT_STATUS_RED_LED); /*Red LED OFF*/
				uiIOExpader2 |= BAT_STATUS_GREEN_LED;	/*Green LED ON*/
					
				/*if prime and spray process is initiated or started and
					Flush not done yet*/
				if((SET == StatusFlag.stStatusFlag.bVBatSprayStartFlag) ||
					 (SET == StatusFlag.stStatusFlag.bVBatPrimeStartFlag) ||
					 (SET == StatusFlag.stStatusFlag.bVBatFlushStartFlag))
				{
					if((SET == StatusFlag.stStatusFlag.bVBatFlushStartFlag) &&
						 (RESET == StatusFlag.stStatusFlag.bTempFlag))
					{
						StatusFlag.stStatusFlag.bTempFlag = SET;
						StatusFlag.stStatusFlag.bFlushSelector = RESET;
						StatusFlag.stStatusFlag.bInitFlushProcess = RESET;
						
						StatusFlag.stStatusFlag.bProcessTimeFlag = SET;
						uiProcessTimeCount = 0;
						
						StatusFlag.stStatusFlag.bManualPrime = RESET;
						
						StatusFlag.stStatusFlag.bManualFlush = RESET;
						StatusFlag.stStatusFlag.bAutoFlush = RESET;
						StatusFlag.stStatusFlag.bHLSReachedOnError = SET;/*in Vbat switching*/
						StatusFlag.stStatusFlag.bMtoHomeAutoFlush = SET;
						//DEBUG_PRINTF("SET bHLSReachedOnError------BAterry\n\r");
						/*Turn OFF SPray Gun Valves*/
						fnValve_ONOFF(SPRAY_GUN, VALVE_OFF );
						
					}
					
					StatusFlag.stStatusFlag.bSystemErrorFlag = SET;/*in Vbat switching*/
					//DEBUG_PRINTF("ERROR WARNNING OCCURS ---------...BAT\r\n");
				}
				else
				{
					/*VBAT_Disable pin to disbale the Vbat*/
					GPIO_WriteBit(VBAT_ENABLE_PORT, VBAT_ENABLE_PIN, Bit_RESET);
					
					StatusFlag.stStatusFlag.bVbatSwichFlag = RESET;
					/*Indication for OPMM still ON after switching on vbat*/
					StatusFlag.stStatusFlag.bGlobalVbatSW = SET;
				}	
			}
			else 
			{
				/*Vbat switching Didn't occur*/
				uiIOExpader2 &= (~BAT_STATUS_RED_LED);/*Red LED OFF*/
				uiIOExpader2 &= (~BAT_STATUS_GREEN_LED);/*Green LED OFF*/	
				/*Vbat to main Swithcing*/
				/*if OPMM is SW to Battery and then Power is back 
				  then OPMM Should Start from Default Mode */
				if(SET == StatusFlag.stStatusFlag.bGlobalVbatSW)
				{
					/*RESET WatchDog*/
					while(1);
				}						
			}
			/*This will indicate Low battery voltage on RED LED */
			/*if(VBAT_LOW_PERCENT >= fBatStatus) */
			if(fBatKneeVoltage >= fAdcReadBattery) 
			{
				uiIOExpader2 |= BAT_STATUS_RED_LED;/*Red LED ON*/
				uiIOExpader2 &= (~BAT_STATUS_GREEN_LED);/*Green LED OFF*/
				
				if(RESET == StatusFlag.stStatusFlag.bTimeAchievStatusFlag)
				{
					if(SET == StatusFlag.stStatusFlag.bBatLowBuzzer)
					{
						StatusFlag.stStatusFlag.bBatLowBuzzer = RESET;
						TIM_SetCompare3(TIM1, 30);
					}
					else
					{
						StatusFlag.stStatusFlag.bBatLowBuzzer = SET;
						TIM_SetCompare3(TIM1, 0);
					}					
				}
			}
			else
			{
				if(RESET == StatusFlag.stStatusFlag.bTimeAchievStatusFlag)
				{
					TIM_SetCompare3(TIM1, 0);
				}
			}
		}	
	}
}

/******************************************************************************
 **@Function 		:	fnBatteryStatus_Read
 **@Discription : this funtion will return the Status of the battery in %                      
 **@param				:	fData : battery voltage read        
 **@Return			: battery status in %
 *****************************************************************************/
float fnBatteryStatus_Read(float fData)
{
	float fHighVolt = 0.0f, fLowVolt = 0.0f, fHighPercent = 0.0f; 
	float fLowPercent = 0.0f,range = 0.0f;
	float fReturnVal = 0.0f;
	
	#ifdef BATTERY_DBG    
		char rgucTemp[40];
		memset(rgucTemp, '\0', sizeof(rgucTemp));
		sprintf(rgucTemp, "\nBtry Status voltage = %f\r\n", fData);
		DEBUG_PRINTF("%s", rgucTemp);
	#endif  
	
	if(fabs(fData) < MINIMUM_VOLT )
	{
		fReturnVal = 0.0f;
	}
	else if((fabs(fData) >  MINIMUM_VOLT) && 
					(fabs(fData) <= VOLT_AT_10_PER))
	{
		/* for 1% to 10 % battry status*/
		fHighVolt = VOLT_AT_10_PER;
		fLowVolt = MINIMUM_VOLT;
		fHighPercent = PERCENTAGE_10;
		fLowPercent =  PERCENTAGE_1;
		
		/*creating fomula for lookuptable*/
		fHighPercent -= fLowPercent;  
		fHighVolt -= fLowVolt;
		range = fHighPercent / fHighVolt;
		
		/* finding % */
		fData = fData - fLowVolt;
		fData *= range;
		fData += fLowPercent;
		
		fReturnVal = fData;
		return fReturnVal;
	}
	else if((fabs(fData) > VOLT_AT_10_PER) && 
					(fabs(fData) <= VOLT_AT_90_PER )) 
	{
		/* for 11% to 90 % battry status */
		fHighVolt = VOLT_AT_90_PER;
		fLowVolt = VOLT_AT_10_PER;
		fHighPercent = PERCENTAGE_90;
		fLowPercent =  PERCENTAGE_11;
		
		/* creating fomula for lookuptable */
		fHighPercent -= fLowPercent; 
		fHighVolt -= fLowVolt;
		range = fHighPercent / fHighVolt ;                    
		
		/* finding % */
		fData = fData - fLowVolt;
		fData *= range;
		fData += fLowPercent;
		
		fReturnVal = fData;
		return fReturnVal;
	}
	else if((fabs(fData) > VOLT_AT_90_PER ) && 
					(fabs(fData) <= VOLT_AT_99_PER))
	{
		/* for 91% to 99 % battry status */
		fHighVolt = VOLT_AT_99_PER;
		fLowVolt = VOLT_AT_90_PER;
		fHighPercent = PERCENTAGE_99;
		fLowPercent =  PERCENTAGE_91;
		
		/* creating fomula for lookuptable */
		fHighPercent -= fLowPercent; 
		fHighVolt -= fLowVolt;
		range = fHighPercent / fHighVolt ;                      
		
		/* finding % */
		fData = fData - fLowVolt;
		fData *= range;
		fData += fLowPercent;
		
		fReturnVal = fData;
		return fReturnVal;
	}
	else if(fabs(fData) > VOLT_AT_99_PER)
	{
	 return PERCENTAGE_100;
	}
	
	return fReturnVal;   
}


/*****************************************************************************
 **@Function 			: fnACK_Check
 **@Descriptions	: checks the acknowledgement and resend the previously send 
										frame again if no ACK received, only 3 time
 **@Parameters		: None
 **@Return				: None
 *****************************************************************************/
void fnACK_Check(void)
{
	static uint8_t sucResendCount = 0;
	
	if(SET == StatusFlag.stStatusFlag.bTxFlag)
	{
		if(RESET == StatusFlag.stStatusFlag.bACKRxFlag)
		{
			/*Resend the Can Message if failed to receive the acknowledge */
			if(CAN_MSG_RESEND_TIME <= uiTxCounter)
			{
				/* resend time check max 3 time */
				if(CAN_MSG_RESEND_ATTEMPT > sucResendCount)
				{
					uiTxCounter = 0;
					sucResendCount++;
					fnCAN_Transmit(&rgstTxCANBuff[ucTxBuffCnt]);
					
					#ifdef GENERAL_DBG	
						DEBUG_PRINTF("Transmit again\r\n");
					#endif
				}
				else /*3 time send fails*/
				{
					StatusFlag.stStatusFlag.bTxFlag = RESET;					
					uiTxCounter = 0;
					sucResendCount = 0;
					uiHBTimeCounter = 0;
					if(DEST_HMI == (rgstTxCANBuff[ucTxBuffCnt].ExtId & DEST_ADDR_MASK))
					{	
//						StatusFlag.stStatusFlag.bCANFaultHMI = SET;/* OP CAN FAULT */
//						StatusFlag.stStatusFlag.bCANFaultFlag = SET;
					}
					
					if((ucTxBuffCnt != ucStoreBuffCnt) || 
						 (SET == StatusFlag.stStatusFlag.bDataInCANTxBuffFlag))
					{
						fnTxBuff_Shift();	
					}
					#ifdef GENERAL_DBG
						DEBUG_PRINTF("Transmit Fail\r\n");
					#endif
				}
			}
		}
		else
		{
			StatusFlag.stStatusFlag.bTxFlag = RESET;
			StatusFlag.stStatusFlag.bACKRxFlag = RESET;	
			
			uiTxCounter = 0;
			sucResendCount = 0;
			
			if((ucTxBuffCnt != ucStoreBuffCnt) || 
				 (SET == StatusFlag.stStatusFlag.bDataInCANTxBuffFlag))
			{
				fnTxBuff_Shift();
			}	
			#ifdef GENERAL_DBG	
				DEBUG_PRINTF(" Transmit success\r\n");
			#endif
		}
	}
}
/******************************************************************************
 **@Function		 	:  fnDiagnosticStatus_Send
 **@Descriptions	:  Send diagnostic status on CAN
 **@Parameters		:  ucStatus1 : status of Valve connected,
										 ucStatus2 : status of valve Operation
										 ucStatus3 : Status of NCTS and Thermocouple 
 **@Return				:  None
 ****************************************************************************/
void fnDiagnosticStatus_Send(uint8_t ucStatus1,uint8_t ucStatus2,
														 uint8_t ucStatus3)
{
	uint8_t ucLoop = 0;
  stTxMsg.DLC = 4;
	
	stTxMsg.Data[1] = ucStatus1;
	stTxMsg.Data[2] = ucStatus2;
	stTxMsg.Data[3] = ucStatus3;
  if((0 == ucStatus1) && (0 == ucStatus2) && (0 == ucStatus3))
  {
		//#ifdef GENERAL_DBG	
//			DEBUG_PRINTF("Diagnostic Pass\r\n");
		//#endif
		/*Diagnostic Pass*/
		stTxMsg.Data[0] = DIAGNOSTICS_PASS;
		
		/*SET the diagnostic flag*/
		StatusFlag.stStatusFlag.bDiagnosticsPassFlag = SET;
		
		uiIOExpader2 |=	(BOARD_STATUS_GREEN_LED); /* ON Green Led*/
		uiIOExpader2 &=	(~BOARD_STATUS_RED_LED); /* OFF Red Status Led*/
		/* ON the Board status RED LED and OFF Green LED*/
  }
	else if((0 != ucStatus1) || (0 != ucStatus2) || (0 != ucStatus3))
  {
		/*RESET the diagnostic flag*/
		StatusFlag.stStatusFlag.bDiagnosticsPassFlag = RESET;
		
		/*Diagnostic Fail*/
		stTxMsg.Data[0] = DIAGNOSTICS_FAIL;
		
		
		//#ifdef GENERAL_DBG	
//			DEBUG_PRINTF("Diagnostic Fail\r\n");
		//#endif
						
		/* the Board status Led GREEN bit Reset*/
		uiIOExpader2 &=	(~BOARD_STATUS_GREEN_LED);
		/* Set the Red Led bit to ON*/
		uiIOExpader2 |=	BOARD_STATUS_RED_LED; 
		/* ON the Board status Led RED and OFF Green*/
	}
  
	fnCANMessage_Send(STATUS_OPMM_DIAGNOSTIC, MSG_TYPE_STATUS);
	
	/*11thMarch-Sending code version after diagnostic completed*/
	memset(stTxMsg.Data, 0, sizeof(stTxMsg.Data));
	stTxMsg.DLC = 5;

	stTxMsg.Data[0] = MODULE_CODE;
	SplitByte.fData = CODE_VERSION;
	for(ucLoop = 1; ucLoop < stTxMsg.DLC; ucLoop++)
	{
		stTxMsg.Data[ucLoop] = SplitByte.rgucData[ucLoop - 1];
	}
	fnCANMessage_Send(STATUS_SOFTWARE_VERSION, MSG_TYPE_STATUS);
}

/*****************************************************************************
 **@Function		 	:  fnDelay_ms
 **@Descriptions	:  this function use for generating delays in mili seconds
 **@Parameters		:  uiDly: count of delay
 **@Return				:  None
 *****************************************************************************/
void fnDelay_ms(uint16_t uiDly)
{
	uint16_t uiLoop;
	for(; uiDly > 0; uiDly--)
	{
		for(uiLoop = 60000; uiLoop > 0; uiLoop--);
		for(uiLoop = 12000; uiLoop > 0; uiLoop--);
	}
}

/******************************************************************************
 **@Function		 	:  fnLED_Status
 **@Descriptions	:  This function is use to display the Status on the Led
 **@Parameters		:  None
 **@Return				:  None
 ****************************************************************************/
void fnLED_Status(void)
{
	if(RESET == StatusFlag.stStatusFlag.bFirstHrtBtMsgFlag)
	{			
		if(ucPOSTLEDBlinkCnt < POST_LED_BLINK_COUNT)
		{
			/* ON Board status RED LED*/
			uiIOExpader1 =	IOEX_ALL_RED;
			uiIOExpader2 =	IOEX_ALL_RED;
		}	
		else if(ucPOSTLEDBlinkCnt > POST_LED_BLINK_COUNT)
		{
			/* OFF Board status RED LED*/
			uiIOExpader1 =	IOEX_ALL_BLUE;
			uiIOExpader2 =	IOEX_ALL_BLUE;
			
			/* RESET The Blinking LED COUNTER if greatere than 200msec */
			if(ucPOSTLEDBlinkCnt >= (2 * POST_LED_BLINK_COUNT))
			{
				ucPOSTLEDBlinkCnt = 0;
			}
		}
	}
		
	if(SET == StatusFlag.stStatusFlag.bCANFaultFlag)	 
	{
		/* OFF Board status Led GREEN */
		uiIOExpader2 &=	(~BOARD_STATUS_GREEN_LED); 	
		if(SET == StatusFlag.stStatusFlag.bCANFaultSCU)
		{
			if(ucLEDBlinkCount < STATUS_LED_BLINK_COUNT)
			{
				/* ON Board status RED LED*/
				uiIOExpader2 |=	BOARD_STATUS_RED_LED; 
			}	
			else if(ucLEDBlinkCount > STATUS_LED_BLINK_COUNT)
			{
				/* OFF Board status RED LED*/
				uiIOExpader2 &=	(~BOARD_STATUS_RED_LED);
				
				/* RESET The Blinking LED COUNTER if greatere than 200msec */
				if(ucLEDBlinkCount >= (2 * STATUS_LED_BLINK_COUNT))
				{
					ucLEDBlinkCount = 0;
				}
			}
		}
		if(SET == StatusFlag.stStatusFlag.bCANFaultHMI)
		{		
			if(ucLEDBlinkCount < STATUS_LED_BLINK_COUNT)
			{
				/* ON Board status BLUE LED*/
				uiIOExpader2 |=	BOARD_STATUS_BLUE_LED; 
			}	
			else if(ucLEDBlinkCount > STATUS_LED_BLINK_COUNT)
			{
				/* OFF Board status BLUE LED*/
				uiIOExpader2 &=	(~BOARD_STATUS_BLUE_LED);
				
				/* RESET The Blinking LED COUNTER if greatere than 200msec */
				if(ucLEDBlinkCount >= (2 * STATUS_LED_BLINK_COUNT))
				{
					ucLEDBlinkCount = 0;
				}
			}
		}
	}
	if(SET == StatusFlag.stStatusFlag.bCAN1ActiveFlag)
	{
		uiIOExpader1 |= CAN1_STATUS_LED;
		uiIOExpader2 &= (~MCP_CAN2_STATUS_LED);
	}
	else if(SET == StatusFlag.stStatusFlag.bMCPCAN2ActiveFlag)
	{
		uiIOExpader1 &= (~CAN1_STATUS_LED);
		uiIOExpader2 |= MCP_CAN2_STATUS_LED;
	}
	else
	{
		uiIOExpader1 &= (~CAN1_STATUS_LED);
		uiIOExpader2 &= (~MCP_CAN2_STATUS_LED);
		#ifdef GENERAL_DBG
			DEBUG_PRINTF("CAN Fault! Both link down\r\n");
		#endif
	}
	
	if((uiIOExpader1 != uiPreIOExpader1) && (RESET == StatusFlag.stStatusFlag.bI2CReading))
	{
		if(!I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))
		{
			uiI2CWriteData =  ~uiIOExpader1;/*Data to Write on the Expander ports*/
			
			StatusFlag.stStatusFlag.bI2CReading = SET;
			ucI2C1Seq = 0;
			/* Slave Address with Write instruction */
			ucIOExpSlaveAddr = IOEXP_1_SLAVE_ADDR & I2C_SLAVE_WRITE; 
			/* Send I2C1 START condition */
			I2C_GenerateSTART(I2C_IO_EXP, ENABLE);
			
			uiPreIOExpader1 = uiIOExpader1;
//			DEBUG_PRINTF("IOEXP_1_SLAVE_ADDR\r\n");
		}
	}
	if((uiIOExpader2 != uiPreIOExpader2) && (RESET == StatusFlag.stStatusFlag.bI2CReading))
	{
		if(!I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))
		{
			uiI2CWriteData =  ~uiIOExpader2;
			StatusFlag.stStatusFlag.bI2CReading = SET;
			ucI2C1Seq = 0;
			/* Slave Address with Write instruction */
			ucIOExpSlaveAddr = IOEXP_2_SLAVE_ADDR & I2C_SLAVE_WRITE; 
			/* Send I2C1 START condition */
			I2C_GenerateSTART(I2C_IO_EXP, ENABLE);
			
			uiPreIOExpader2 = uiIOExpader2;
//			DEBUG_PRINTF("IOEXP_2_SLAVE_ADDR\r\n");
		}
	}
}

/******************************************************************************
 **@Function 			:	 fnDataMsg_Tx
 **@Descriptions	:  Forward the current Liquid level Data to SCU and HMI
 **@Parameters		:	 fData : Temperature or Liquid level data (in float) 
										 ucDataType: type of data i.e liquid level or Temperature
										 ucDataId: Data Id as per the CAN MSG ID 
 **@returnval			:  None
 *****************************************************************************/
void fnDataMsg_Tx(float fData, uint8_t ucDataId)
{
	uint8_t ucLoop;
	memset(stTxMsg.Data, 0, sizeof(stTxMsg.Data));
	stTxMsg.DLC = 4;
	stTxMsg.ExtId = 0;
	SplitByte.fData = fData;
	
	for(ucLoop = 0; ucLoop < stTxMsg.DLC; ucLoop++)
	{
		 stTxMsg.Data[ucLoop] = SplitByte.rgucData[ucLoop];
	}

	stTxMsg.ExtId = RESERVED | MSG_TYPE_DATA | SOURCE_OPMM;
	
	if(BASE == ucDataId)
	{
		stTxMsg.ExtId |= DATA_OPMM_BASE_TEMP;
	}
	if(HARDNER == ucDataId)
	{
		stTxMsg.ExtId |= DATA_OPMM_HARD_TEMP;
	}
	fnCAN_PreTx(&stTxMsg);
}

/******************************************************************************
 **@Function 		: 	fnACK_Check
 **@Descriptions	:   Send CAN Message to SCU AND HMI both or Store in FIFO
 **@Parameters		:   pstCANTxmsg: Pointer to CAN Transmit Structure
 **@Return				:   None
 *****************************************************************************/
void fnCAN_PreTx(CanTxMsg *pstCANTxmsg)
{
	if(RESET == StatusFlag.stStatusFlag.bCANFaultSCU)/* if no MCU CAN fault */
	{
		pstCANTxmsg->ExtId |= DEST_SCU;
		if((ucTxBuffCnt == ucStoreBuffCnt) && 
			 (RESET == StatusFlag.stStatusFlag.bDataInCANTxBuffFlag))
		{
			 fnCAN_Transmit(pstCANTxmsg);	
		}
		fnBuffer_Store(pstCANTxmsg);/* Store the Tx structure in the Buffer */
	}
	
	if(RESET == StatusFlag.stStatusFlag.bCANFaultHMI)	/* if no OP CAN fault */
	{			
		pstCANTxmsg->ExtId = ((pstCANTxmsg->ExtId & DEST_ADDR_CLEAR_MASK) |
													 DEST_HMI)  ;
		if((ucTxBuffCnt == ucStoreBuffCnt) && 
			 (RESET == StatusFlag.stStatusFlag.bDataInCANTxBuffFlag))
		{
			fnCAN_Transmit(pstCANTxmsg);
		}
		fnBuffer_Store(pstCANTxmsg);/* Store the Tx structure in the Buffer */
	}

}



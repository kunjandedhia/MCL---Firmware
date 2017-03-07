/******************************************************************************
  * @file    SolenoidValve.c
  * 
  * @brief   This file contains the functions related to solenoid valve
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
	ValveState_t	stValve[VALVE_USED];
	uint16_t uiValveReadCounter = 0;
	uint8_t ucValveFBState 			= 0;
	uint8_t ucValveFault 				= 0;/*variable to store valve fault*/
	uint8_t ucPrevValveFault 		= 0;/*variable to store Previous valve fault*/
	uint8_t ucValveFaultHistory = 0;/*variable to use for Valve fault history*/
	uint8_t ucCurrOnOff 				= 0;
	uint8_t ucPrevOnOff 				= 0;
	uint8_t ucValvePrevFBState 	= 0;	
/* ----------------------------------------------------------------------------
 *                           EXTERNAL VARIABLES
 * ---------------------------------------------------------------------------
 */
 /* ---------------------------------------------------------------------------
 *                           EXTERNAL FUNCTIONS
 * ---------------------------------------------------------------------------
 */
/* ----------------------------------------------------------------------------
 *                           FUNCTIONS DECLARITION
 * ---------------------------------------------------------------------------
 */
 
 /*****************************************************************************
 **@Function 			: fnValve_ONOFF
 **@Descriptions	: Turn on/off Valves and LEDs
 **@Parameters		: bState : ON=1 , OFF=0 (boolean)
										GPIO_X : GPIO Port for the Valve
										GPIO_Pin_Number :	GPIO pin number for the Valve
 **@Return				: None
 ****************************************************************************/
void fnValve_ONOFF(uint8_t ucStructVal, BitAction bState)
{
	/*if Valve is faulty then Valve OFF allowed else ON/OFF both*/
	if((Bit_RESET == bState) || !(ucValveFaultHistory & (0x01 << ucStructVal)))
	{
		GPIO_WriteBit(VALVE_DRIVE_PORT,
									VALVE_DRIVE_PIN(stValve[ucStructVal].ucPosition), bState);
		/*Update valve status in the Variable*/
		ucCurrOnOff &= ~(0x01 << ucStructVal);
		ucCurrOnOff |= (uint8_t)(bState << ucStructVal);
		
		#ifdef VALVE_DBG
			DEBUG_PRINTF("Struct: %d, ON/OFF: %d\r\n", ucStructVal, bState);
		#endif
	}
	if((RESET == stValve[ucStructVal].bFaultFlag) && (STALL_VALVE == ucStructVal))
	{
		/*LED Update for the given Valve*/
		uiIOExpander1 &= ~STALL_VALVE_LED_VALUE(RED);		/*FAULT/RED LED OFF*/
		if(SET == bState)/*GREEN LED ON*/
		{
			uiIOExpander1 |= STALL_VALVE_LED_VALUE(GREEN); 
		}
		else/*GREEN LED OFF*/
		{
			uiIOExpander1 &= ~STALL_VALVE_LED_VALUE(GREEN);
		}
		
		//DEBUG_PRINTF("LED Update: %x\r\n", uiIOExpander1);
	}
	/*for reading the feedback only when its status changes*/
	StatusFlag.stStatusFlag.bValveReadFlag = SET;
	uiValveReadCounter = 0;
}

/******************************************************************************
 **@Function		 	: fnValve_Process
 **@Descriptions	: This Function is used to Process the Valves, Check for
										Valve	status and faults of Valve and send status(if changed)
										and	valve faults(if any) to SCU and HMI
 **@Parameters		: None
 **@Return				: None
 ****************************************************************************/
void fnValve_Process(void)
{
	uint8_t ucLoop = 0;
	/*Check for Valve Fault*/
	if(VALVE_READ_TIME <= uiValveReadCounter)
	{
		/*Read the status of each valve after VALVE_READ_TIME*/
		ucValveFBState = 0;
		for(ucLoop = 0; ucLoop < VALVE_USED; ucLoop++)
		{
			ucValveFBState |= (GPIO_ReadInputDataBit(VALVE_FEEDBACK_PORT,
								VALVE_FEEDBACK_PIN(stValve[ucLoop].ucPosition)) << ucLoop);
		}
		#ifdef VALVE_DBG 
				//DEBUG_PRINTF("ucCurrOnOff: %d, ucValveFBState: %d \r\n", ucCurrOnOff, ucValveFBState );
		#endif
		
		/*ON/OFF and Feedback status of valve should be same if not
			then fault*/
		ucValveFault = ucCurrOnOff ^ ucValveFBState;

		/*Check for Valve Fault*/
		/*and delay between the valve ON/OFF and Reading status should
			be elapse*/
		/*Cond1=> if any fault
			Cond2=> previous fault and current fault should be differ
			Cond3=> current fault should be first time only*/
		if((0 != ucValveFault) && (ucValveFault != ucPrevValveFault) &&
			 (ucValveFault ^ ucValveFaultHistory))
		{
			#ifdef VALVE_DBG 
			DEBUG_PRINTF("Valve Fault Occure------------------- \r\n");
				DEBUG_PRINTF("VALVE FAULT ucValveFault: %d, ucCurrOnOff: %d, \
				 ucValveFBState: %d \r\n", ucValveFault, ucCurrOnOff, ucValveFBState );
			#endif
			/*this is used to store Valve Fault History*/
			ucValveFaultHistory |= ucValveFault;

			memset(stTxMsg.Data, 0 , sizeof(stTxMsg.Data));
			stTxMsg.DLC = 0;
			stTxMsg.DLC = 1;	
			stTxMsg.Data[0] = ucValveFault;			
			fnFault_Send(FAULT_STALL_VALVE);
			
			ucPrevValveFault = ucValveFault;
			#ifdef VALVE_DBG 
//				DEBUG_PRINTF("FAULT ACTION\r\n");
			#endif
			/*Fault LED Action for Faulty Valve*/
			if(ucValveFault & 0X01)
			{
				uiIOExpander1 |= STALL_VALVE_LED_VALUE(RED);/*FAULT/RED LED ON*/
				uiIOExpander1 &= ~STALL_VALVE_LED_VALUE(GREEN);/*GREEN LED OFF*/
				
				/*Valve Fault Indication*/
				stValve[STALL_VALVE].bFaultFlag = SET;

				/*************VALVE Fault Action*************/
				fnValve_ONOFF(STALL_VALVE, STALL_VALVE_ON);
				#ifdef REC_VAL_DBG
					DEBUG_PRINTF("CMD_STALL_VALVE\r\n");
				#endif
			}
		}
		/*RESET the Counter for next reading of Feedback status and fault check*/
		uiValveReadCounter = 0;
	}
	/*Send status if any valve altered*/
	if(ucCurrOnOff != ucPrevOnOff)
	{
		#ifdef VALVE_DBG 
//			DEBUG_PRINTF("Status send : %d\r\n", ucCurrOnOff);
		#endif
		/*Send valve status to SCU and HMI*/
		stTxMsg.DLC 		= 1;
		stTxMsg.Data[0] = ucCurrOnOff;
		fnStatus_Send(STATUS_STALL_VALVE);
		ucPrevOnOff = ucCurrOnOff;
	}
}

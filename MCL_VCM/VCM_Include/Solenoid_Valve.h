/******************************************************************************
  * @file    SolenoidValve.h
  * 
  * @brief   This file contains included files, Macros, Global Variables,
						 External Variables, External Functions
  ****************************************************************************
  */
	#ifndef __SolenoidValve_h
	#define __SolenoidValve_h
/* ----------------------------------------------------------------------------
 *                           Includes
 * ---------------------------------------------------------------------------
 */
	#include "Structures.h"
/* ----------------------------------------------------------------------------
 *                           MACROS
 * ---------------------------------------------------------------------------
 */
 /*********GPIO pins for Valves********/
	#define VALVE1_DRIVE														GPIO_Pin_2
	#define VALVE2_DRIVE														GPIO_Pin_3
	#define VALVE3_DRIVE														GPIO_Pin_4
	#define VALVE4_DRIVE														GPIO_Pin_5
	
	#define VALVE1_FEEDBACK													GPIO_Pin_2	
	#define VALVE2_FEEDBACK													GPIO_Pin_3
	#define VALVE3_FEEDBACK													GPIO_Pin_4
	#define VALVE4_FEEDBACK													GPIO_Pin_5
	
	#define VALVE1_CONN_STAT												GPIO_Pin_12
	#define VALVE2_CONN_STAT												GPIO_Pin_13
	#define VALVE3_CONN_STAT												GPIO_Pin_14
	#define VALVE4_CONN_STAT												GPIO_Pin_15
	
	#define VALVES_DRIVE_PIN											VALVE1_DRIVE | VALVE2_DRIVE | VALVE3_DRIVE | VALVE4_DRIVE			
	#define VALVES_FEEDBACK_PIN										VALVE1_FEEDBACK | VALVE2_FEEDBACK | VALVE3_FEEDBACK | VALVE4_FEEDBACK					
	#define VALVES_CONN_STAT_PIN									VALVE1_CONN_STAT | VALVE2_CONN_STAT | VALVE3_CONN_STAT | VALVE4_CONN_STAT							
	/*Structure Position for each Valves*/
	#define STALL_VALVE															(uint8_t)0
	#define RECIRCULATION_VALVE											(uint8_t)1
	#define SPARE_VALVE2														(uint8_t)2
	#define SPARE_VALVE3														(uint8_t)3
	
	#define TOTAL_VALVES													  (uint8_t)4
	#define VALVE_USED													 		(uint8_t)2

	/********* GPIO pins Ports for Valves ********/
	#define VALVE_DRIVE_PORT														GPIOE
	#define VALVE_CONN_STAT_PORT												GPIOD
	#define VALVE_FEEDBACK_PORT			  								  GPIOG																		 

	#define VALVE_DRIVE_PIN(x)						((1 == x)? VALVE1_DRIVE: \
																				 (2 == x)? VALVE2_DRIVE: \
																				 (3 == x)? VALVE3_DRIVE: \
																				 (4 == x)? VALVE4_DRIVE:0)
																									 
  #define VALVE_FEEDBACK_PIN(x)					((1 == x)? VALVE1_FEEDBACK: \
																				 (2 == x)? VALVE2_FEEDBACK: \
																				 (3 == x)? VALVE3_FEEDBACK: \
																				 (4 == x)? VALVE4_FEEDBACK : 0)
																									 
  #define VALVE_CONN_STAT_PIN(x)				((1 == x)? VALVE1_CONN_STAT: \
																				 (2 == x)? VALVE2_CONN_STAT: \
																				 (3 == x)? VALVE3_CONN_STAT: \
																				 (4 == x)? VALVE4_CONN_STAT: 0)
  #define STALL_VALVE_ON									Bit_SET																									 
  #define STALL_VALVE_OFF									Bit_RESET		
	
/* ----------------------------------------------------------------------------
 *                           CONSTANTS
 * ---------------------------------------------------------------------------
 */
/* ----------------------------------------------------------------------------
 *                           GLOBAL VARIABLES
 * ---------------------------------------------------------------------------
 */
/* ----------------------------------------------------------------------------
 *                           EXTERNAL VARIABLES
 * ---------------------------------------------------------------------------
 */
	extern ValveState_t	stValve[VALVE_USED];
	extern uint16_t uiValveReadCounter;
	extern uint8_t ucCurrOnOff;
	extern uint8_t ucPrevOnOff;
	extern uint8_t ucValveFBState;								
	extern uint8_t ucValvePrevFBState;
	extern uint8_t ucValveFault;								/*variable to store valve fault*/
	extern uint8_t ucPrevValveFault;							/*variable to store Previous valve fault*/
	extern uint8_t ucValveFaultHistory;						/*variable to use for Valve fault send only once*/
 /* ---------------------------------------------------------------------------
 *                           EXTERNAL FUNCTIONS
 * ---------------------------------------------------------------------------
 */
 extern void fnValve_ONOFF(uint8_t, BitAction);
 extern void fnValve_Process(void);
/* ----------------------------------------------------------------------------
 *                           FUNCTIONS DECLARITION
 * ---------------------------------------------------------------------------
 */
	#endif

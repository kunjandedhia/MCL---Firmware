/******************************************************************************
**@file     Misc_Function.h
* 
**@brief    This Header File contains declaration of all the Misc external 
						Functions, Macros and External variables.
******************************************************************************
*/    
#ifndef __Misc_Function_h
#define __Misc_Function_h

/* ----------------------------------------------------------------------------
*                           Includes
* ----------------------------------------------------------------------------
*/
#include "Structures.h"

/* ----------------------------------------------------------------------------
*                           MACROS
* ----------------------------------------------------------------------------
*/


#define BASE																	(uint8_t)0
#define HARDNER																(uint8_t)1

#define TEMPERATURE														(uint8_t)1
#define LIQUID_LEVEL													(uint8_t)2

#define DEFAULT_MODE 													(uint8_t)0x00
#define OPERATION_MODE				 								(uint8_t)0x01
#define DIAGNOSTICS_FAIL											(uint8_t)0x01
#define DIAGNOSTICS_PASS											(uint8_t)0x02

#define HEATER_SSR_DIGNOSTICS_FAIL(x)					((0 == x)? 0x01: \
																							 (1 == x)? 0x02: \
																							 (2 == x)? 0x04: 0x08)	
																						 
#define THERMOCOUPLE_DIGNOSTICS_FAIL(x) 		  ((0 == x)? 0x10: \
																							 (1 == x)? 0x20: \
																							 (2 == x)? 0x40: 0x80)
																							 
#define LIQUID_LEVEL_DIGNOSTICS_FAIL(x)		  	((0 == x)? 0x01: 0x02) 


#define HIGH_TH_FAULT_COUNT										(uint8_t)4  /* number of count to check reading before declaring the High TH fault */
#define LOW_TH_FAULT_COUNT										(uint8_t)4	/* number of count to check reading before declaring the Low TH fault */
#define IN_RANGE_CHECK_COUNT									(uint8_t)4	
#define LL_HIGH_TH_FAULT_COUNT								(uint8_t)4
#define LL_LOW_TH_FAULT_COUNT									(uint8_t)4

#define INVALID_FLOAT_VALUE										2999.0f

#define FIRST_BOOTUP 													(uint8_t)0x02
#define POSITIVE_ACK													(uint8_t)0x00 

#ifdef TEST_DBG
	#define TEST_SET_TMP												40.0f   /*Set temperature to 50°C for testing*/
	#define TEST_HI_TH_TMP											45.0f   /*Set high threshold temperature to 60°C for testing*/		
	#define TEST_LOW_TH_TMP											35.0f   /*Set low threshold temperature to 40°C for testing*/	
#endif


	#define MODULE_CODE														(uint8_t)0x04
	#define CODE_VERSION													1.1f
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
extern uint8_t ucMode;
extern uint16_t uiBypass;
extern SplitByte_t SplitByte;
extern StatusFlags_t StatusFlag;

/* ----------------------------------------------------------------------------
*                           EXTERNAL FUNCTIONS
* ----------------------------------------------------------------------------
*/
extern void fnSystem_Init(void);
extern void fnDefault_Setting(void);
extern void fnSelfDiagnostic_Test(void);
extern void fnDefault_Mode(void);
extern void fnOperation_Mode(void);
extern void fnHearBeat_Check(void);
extern void fnEstop_Check(void);
extern void fnDelay_ms(unsigned int);
extern void fnClearAll_Warnings(void);
extern void fnClearAll_Faults(void);

/* ----------------------------------------------------------------------------
*                           FUNCTIONS DECLARATION
* ----------------------------------------------------------------------------
*/
#endif


/******************************************************************************
  * @file    MIsc_Functions.h
  * 
  * @brief   This file contains included files, Macros, Global Variables,
						 External Variables, External Functions
  ****************************************************************************
  */
	#ifndef __Misc_Functions_h
	#define __Misc_Functions_h
/* ----------------------------------------------------------------------------
 *                           Includes
 * ---------------------------------------------------------------------------
 */
 #include "Structures.h"
/* ----------------------------------------------------------------------------
 *                           MACROS
 * ---------------------------------------------------------------------------
 */
	#define DEVICE_FUN_MFM							(uint8_t)0x10

	#define DEFAULT_MODE								(uint8_t)0
	#define OPERATION_MODE							(uint8_t)1
  #define INVALID_DATA								2999.0f

	/****************************************/ 
	#define CONVR_NUMERATOR            	 	3.3f 
	#define CONVR_DENOMINATOR          		4096.0f 
	#define VALUE_PRINT_TIME						(uint8_t)1
	#define INTEGER_2										(uint8_t)2
 
	/****************************************/ 
	#define LOW_THERSHOLD		       			(uint8_t)0x01	
	#define HIGH_THERSHOLD					    (uint8_t)0x02
	
	#define CYLINDER_A									(uint8_t)0x01
	#define CYLINDER_B									(uint8_t)0x02
	#define CYLINDER_C									(uint8_t)0x04
	
	#define START												(uint8_t)0x02
	#define STOP												(uint8_t)0x01
	
	/****************************************/ 
	#define PG_READ_TIME								(uint16_t)200//100		/*Valve Status read time = 100 * 10ms = 1 second */
	#define VALVE_READ_TIME							(uint16_t)5			/*Valve Status read time = 50 * 10ms = 500 ms */	
	#define FAULT_COUNT									(uint8_t)4
	#define LT_CHECKCOUNTER							(uint16_t)100		/*Linear transducer range check time = 50 * 10ms = 0.5 seconds */
	#define STALL_COUNTER								(uint8_t)10     /* 100ms */
	
	#define DIAGNOSTICS_FAIL											(uint8_t)0x01
	#define DIAGNOSTICS_PASS											(uint8_t)0x02
	
	#define TIM_PERIOD 														100
	#define TIM_PRESCALER 												7200
	
	#define MODULE_CODE														(uint8_t)0x05
	#define CODE_VERSION													2.3f
	
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
	extern SplitByte_t SplitByte;
	extern FLAGS_Union_t	StatusFlag;
	extern uint8_t ucMode;
	extern float fCylinderAreaA;
	extern float fCylinderAreaB;
	extern float fCylinderAreaC;
	extern uint16_t uiPumpCount;
	extern uint8_t ucCylinderCount;
	extern uint8_t ucBypass;
	extern uint8_t ucStallValvePos;
	extern uint8_t ucOutsideRemap;
	extern uint8_t ucInsideRemap; 
	extern uint16_t uiIOExpander1;
	extern uint16_t uiPreIOExpander1;
	extern uint16_t uiIOExpander2; 
	extern uint16_t uiPreIOExpander2;
	extern uint8_t ucI2CDirection;
	extern uint8_t ucMuxChannelNum;
	
	extern uint16_t uiHBTimeCounter;
	extern uint8_t ucLEDBlinkCount;
	extern uint16_t uiAmbiantReadCnt;
	extern uint16_t uiTxCounter;
	extern uint16_t uiStallCounter;
	extern uint16_t uiTBeforeStallPresCompr;
	extern uint16_t uiPressureReadCounter;
	extern uint16_t uiMaterialUsageCnt;
	extern uint16_t uiLTCheckCounter;
	extern uint16_t uiLinearTransCnt;
	extern uint8_t ucPOSTLEDBlinkCnt;
	extern uint8_t ucStrokeType;
	extern uint8_t ucPreStrokeType;

 /* ---------------------------------------------------------------------------
 *                           EXTERNAL FUNCTIONS
 * ---------------------------------------------------------------------------
 */
	extern void fnDefault_Config(void);
	extern void fnEstop_Check(void);
	extern void fnDefault_Mode(void);
	extern void fnLED_Indication(void);
	extern void fnOperation_Mode(void);
	extern void fnSelfDiagnostic_Test(void);
	extern void fnAllWarnnings_Clear(void);
	extern void fnAllFaults_Clear(void);
	extern void fnTIM1_Config(void);
	extern void fnSysInit(void);
/* ----------------------------------------------------------------------------
 *                           FUNCTIONS DECLARITION
 * ---------------------------------------------------------------------------
 */
	#endif


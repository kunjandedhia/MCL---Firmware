/******************************************************************************
  * @file    Stall_Test.h
  * 
  * @brief   This file contains included files, Macros, Global Variables,
						 External Variables, External Functions
  ****************************************************************************
  */
	#ifndef __StallTest_h
	#define __StallTest_h
/* ----------------------------------------------------------------------------
 *                           Includes
 * ---------------------------------------------------------------------------
 */
	#include "Structures.h"
/* ----------------------------------------------------------------------------
 *                           MACROS
 * ---------------------------------------------------------------------------
 */
#define UPSTROKE										(uint8_t)0x01
#define DOWNSTROKE									(uint8_t)0x02
#define NONE												(uint8_t)0x03


//	#define DOWNSTROKE_RANGE_LOW				(float)60.0f
//	#define DOWNSTROKE_RANGE_HIGH				(float)110.0f
//	#define UPSTROKE_RANGE_LOW					(float)30.0f
//	#define UPSTROKE_RANGE_HIGH					(float)70.0f

	#define UPSTROKE_PASS								(uint8_t)0x02
	#define UPSTROKE_FAIL								(uint8_t)0x01
	#define DOWNSTROKE_PASS							(uint8_t)0x08
	#define DOWNSTROKE_FAIL							(uint8_t)0x04
	#define STALL_TEST_PASS							(uint8_t)0x0A
	#define ALL_PRESSURE_LEGS						(uint8_t)0x07
	
	/*4th March2016*/
	#define UPSTROKE_LIMIT									(uint8_t)0x01
	#define UPSTROKE_MIN										(uint8_t)0x02
	#define UPSTROKE_MAX										(uint8_t)0x04
	#define DOWNSTROKE_LIMIT								(uint8_t)0x08
	#define DOWNSTROKE_MIN									(uint8_t)0x10
	#define DOWNSTROKE_MAX									(uint8_t)0x20
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
	extern StallTest_t stStallFlag;
	/*Timer counters*/
	extern uint16_t uiTBeforeStallCnt;
	
	/*Variables related to Stall test*/
	
	extern float fTimBfrCompStallPressure;
	extern float fTimeBeforeStall;
	extern float fValue1;
	extern float fValue2;
	extern int16_t iStallNegativeTh;
	extern int16_t iStallPositiveTh;
	
	/*4thMarch2016*/
	extern uint8_t ucUpstrokeLimit;
	extern uint8_t ucUpstrokeMin;
	extern uint8_t ucUpstrokeMax;
	extern uint8_t ucDownstrokeLimit;
	extern uint8_t ucDownstrokeMin;
	extern uint8_t ucDownstrokeMax;
 /* ---------------------------------------------------------------------------
 *                           EXTERNAL FUNCTIONS
 * ---------------------------------------------------------------------------
 */
	extern  void fnStall_Test(void);
/* ----------------------------------------------------------------------------
 *                           FUNCTIONS DECLARITION
 * ---------------------------------------------------------------------------
 */
	#endif

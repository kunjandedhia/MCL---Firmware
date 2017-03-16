/******************************************************************************
**@file    	Timer.h
* 
**@brief    This Header File contains declaration of all the external function,
						Macros and External variables related Timer Peripheral.
******************************************************************************
*/    
#ifndef __Timer_h
#define __Timer_h
/* ----------------------------------------------------------------------------
*                           Includes
* ----------------------------------------------------------------------------
*/
/* ----------------------------------------------------------------------------
*                           MACROS
* ----------------------------------------------------------------------------
*/
#define TIM1_PRESCALER												(uint16_t)7200
#define	TIM1_PERIOD														(uint16_t)100

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
extern void fnTIM1_Init(void);
/* ----------------------------------------------------------------------------
*                           FUNCTIONS DECLARATION
* ----------------------------------------------------------------------------
*/
#endif



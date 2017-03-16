/******************************************************************************
**@file    	I2C.h
* 
**@brief    This Header File contains declaration of all the external function,
						Macros and External variables related IO_Expander on I2C.
******************************************************************************
*/    
#ifndef __I2C_h
#define __I2C_h

/* ----------------------------------------------------------------------------
*                           Includes
* ----------------------------------------------------------------------------
*/
/* ----------------------------------------------------------------------------
*                           MACROS
* ----------------------------------------------------------------------------
*/

/******** I2C for IOExpander (LED Board) *********/
#define I2C_SCL																GPIO_Pin_6					
#define I2C_SDA																GPIO_Pin_7					
#define I2C_PORT															GPIOB
#define I2C_CLOCK_SPEED												(uint32_t)100000	/*100kHz*/
#define I2C_IO_EXP														I2C1

#define I2C_SLAVE_WRITE												(uint8_t)0xFE		
#define I2C_SLAVE_READ												(uint8_t)0x01

#define IOEXP_1_SLAVE_ADDR										(uint8_t)0x42					
#define IOEXP_2_SLAVE_ADDR										(uint8_t)0x44							

/****** Expander 1 ******* SLAVE 1 *****/
#define SSR_1_RED_LED													(uint16_t)0x0001
#define SSR_1_BLUE_LED												(uint16_t)0x0002
#define SSR_1_GREEN_LED												(uint16_t)0x0004

#define SSR_2_RED_LED													(uint16_t)0x0008
#define SSR_2_BLUE_LED												(uint16_t)0x0010
#define SSR_2_GREEN_LED												(uint16_t)0x0020

#define SSR_3_RED_LED													(uint16_t)0x0040
#define SSR_3_BLUE_LED												(uint16_t)0x0080
#define SSR_3_GREEN_LED												(uint16_t)0x0100

#define SSR_4_RED_LED													(uint16_t)0x0200
#define SSR_4_BLUE_LED												(uint16_t)0x0400
#define SSR_4_GREEN_LED												(uint16_t)0x0800

#define RETRUNLIN_TEMP_RED_LED								(uint16_t)0x1000	
#define RETRUNLIN_TEMP_BLUE_LED								(uint16_t)0x2000
#define RETRUNLIN_TEMP_GREEN_LED							(uint16_t)0x4000


					/****** Expander 2 ******* SLAVE 2 *****/
#define IL_BASE_TC_RED_LED									 	(uint16_t)0x0001
#define IL_BASE_TC_BLUE_LED						  			(uint16_t)0x0002
#define IL_BASE_TC_GREEN_LED									(uint16_t)0x0004

#define IL_HARD_TC_RED_LED									 	(uint16_t)0x0008
#define IL_HARD_TC_BLUE_LED						  			(uint16_t)0x0010
#define IL_HARD_TC_GREEN_LED									(uint16_t)0x0020

#define IL_ANTIFREEZ_TC_RED_LED								(uint16_t)0x0040
#define IL_ANTIFREEZ_TC_BLUE_LED							(uint16_t)0x0080
#define IL_ANTIFREEZ_TC_GREEN_LED							(uint16_t)0x0100

#define LL_BASE_RED_LED												(uint16_t)0x0200	
#define LL_BASE_BLUE_LED											(uint16_t)0x0400
#define LL_BASE_GREEN_LED											(uint16_t)0x0800

#define LL_HARD_RED_LED												(uint16_t)0x1000	
#define LL_HARD_BLUE_LED											(uint16_t)0x2000
#define LL_HARD_GREEN_LED											(uint16_t)0x4000

#define IO1_ALL_RED_LED												(uint16_t)0x1249
#define IO2_ALL_RED_LED												(uint16_t)0x1249
#define IO1_ALL_BLUE_LED											(uint16_t)0x2492
#define IO2_ALL_BLUE_LED											(uint16_t)0x2492
#define IO1_ALL_GREEN_LED											(uint16_t)0xC924
#define IO2_ALL_GREEN_LED											(uint16_t)0xC924

#define POST_LED_BLINK_COUNT									(uint8_t)20	
#define RED																		(uint16_t)1	
#define BLUE																	(uint16_t)2
#define GREEN																	(uint16_t)3

#define SSR_LED_VALUE(x , y)									((0 == x)? ((RED == y)? SSR_1_RED_LED: (BLUE == y)? SSR_1_BLUE_LED: SSR_1_GREEN_LED): \
																							 (1 == x)? ((RED == y)? SSR_2_RED_LED: (BLUE == y)? SSR_2_BLUE_LED: SSR_2_GREEN_LED): \
																							 (2 == x)? ((RED == y)? SSR_3_RED_LED: (BLUE == y)? SSR_3_BLUE_LED: SSR_3_GREEN_LED): \
																							   			 ((RED == y)? SSR_4_RED_LED: (BLUE == y)? SSR_4_BLUE_LED: SSR_4_GREEN_LED))			
																										 
#define TC_LED_VALUE(x , y)							 	 		((0 == x)? ((RED == y)? IL_BASE_TC_RED_LED: (BLUE == y)? IL_BASE_TC_BLUE_LED: IL_BASE_TC_GREEN_LED): \
																							 (1 == x)? ((RED == y)? IL_HARD_TC_RED_LED: (BLUE == y)? IL_HARD_TC_BLUE_LED: IL_HARD_TC_GREEN_LED): \
																							 (2 == x)? ((RED == y)? IL_ANTIFREEZ_TC_RED_LED: (BLUE == y)? IL_ANTIFREEZ_TC_BLUE_LED: IL_ANTIFREEZ_TC_GREEN_LED) :\
																							 (3 == x)? ((RED == y)? RETRUNLIN_TEMP_RED_LED: (BLUE == y)? RETRUNLIN_TEMP_BLUE_LED: RETRUNLIN_TEMP_GREEN_LED):0)
															
#define LL_LED_VALUE(x , y)							 		 ((0 == x)? ((RED == y)? LL_BASE_RED_LED: (BLUE == y)? LL_BASE_BLUE_LED: LL_BASE_GREEN_LED): \
																												((RED == y)? LL_HARD_RED_LED: (BLUE == y)? LL_HARD_BLUE_LED: LL_HARD_GREEN_LED))																					           																																			

#define LED_ON																Bit_SET
#define LED_OFF																Bit_RESET

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
extern uint16_t uiIOExpander1;
extern uint16_t uiIOExpander2;
extern uint16_t uiPreIOExpander1;	
extern uint16_t uiPreIOExpander2;
extern uint16_t uiI2CWriteData;
extern uint8_t ucI2C1Seq;
extern uint8_t ucIOExpSlaveAddr;
extern uint8_t ucPostLedBlinkCnt;
/* ----------------------------------------------------------------------------
*                           EXTERNAL FUNCTIONS
* ----------------------------------------------------------------------------
*/
extern void fnI2C_Config(void);
extern void fnLED_Indication(void);

/* ----------------------------------------------------------------------------
*                           FUNCTIONS DECLARATION
* ----------------------------------------------------------------------------
*/

#endif


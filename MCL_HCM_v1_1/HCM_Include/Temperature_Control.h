/******************************************************************************
**@file    	Temperature_Control.h
* 
**@brief   	This Header File contains declaration of all the external function,
						Macros and External variables related Temperature controlling(i.e SSR and Thermocouple).
******************************************************************************
*/    
#ifndef __Temperature_Control_h
#define __Temperature_Control_h
/* ----------------------------------------------------------------------------
*                           Includes
* ----------------------------------------------------------------------------
*/
/* ----------------------------------------------------------------------------
*                           MACROS
* ----------------------------------------------------------------------------
*/
#define TEMP_READ_TIME 												(uint8_t)10			/*Temp Data Read time = 5 * 10ms = 50 mSecond*/
#define DATA_READ_TIME												(uint16_t)100		/*Temp Data Send Time = 100 * 10ms = 1 Second*/

/*Sequence of structure number*/
#define INLINE_BASE 													(uint8_t)0
#define INLINE_HARD 													(uint8_t)1
#define ANTIFREEZ															(uint8_t)2
#define RETURN_LINE 													(uint8_t)3
#define HARDNER_HEATER 										 	  (uint8_t)3


#define LOW_THERSHOLD		       								(uint8_t)0x01	
#define HIGH_THERSHOLD					    					(uint8_t)0x02

#define TEMP_SET					    								(uint8_t)0x01
#define TEMP_LOW_THERSHOLD		 								(uint8_t)0x02
#define TEMP_HIGH_THERSHOLD		 								(uint8_t)0x04
#define TEMP_HYS_POS					    						(uint8_t)0x08
#define TEMP_HYS_NEG					    						(uint8_t)0x10

#define INLINE_BASE_SSR 											(uint8_t)0x01
#define INLINE_HARD_SSR 											(uint8_t)0x02
#define ANTIFREEZ_SSR													(uint8_t)0x04
#define HARDNER_SSR														(uint8_t)0x08

#define INLINE_BASE_FAULT											(uint8_t)0x01
#define INLINE_HARD_FAULT 										(uint8_t)0x02
#define ANTIFREEZ_FAULT												(uint8_t)0x04


/********************** SSR MACRO's ****************************/	 
	#define SSR_1_PIN														GPIO_Pin_2
	#define SSR_2_PIN														GPIO_Pin_3
	#define SSR_3_PIN														GPIO_Pin_4
	#define SSR_4_PIN														GPIO_Pin_5
	#define SSR_5_PIN														GPIO_Pin_6
	#define SSR_6_PIN														GPIO_Pin_7
	#define SSR_7_PIN														GPIO_Pin_8
	#define SSR_8_PIN														GPIO_Pin_9

	#define SSR_1_FEEDBACK_PIN									GPIO_Pin_8
	#define SSR_2_FEEDBACK_PIN									GPIO_Pin_9
	#define SSR_3_FEEDBACK_PIN									GPIO_Pin_10
	#define SSR_4_FEEDBACK_PIN									GPIO_Pin_11
	#define SSR_5_FEEDBACK_PIN									GPIO_Pin_12
	#define SSR_6_FEEDBACK_PIN									GPIO_Pin_13
	#define SSR_7_FEEDBACK_PIN									GPIO_Pin_14
	#define SSR_8_FEEDBACK_PIN									GPIO_Pin_15

	#define HEATER_SSR_PORT					  					GPIOG
	#define HEATER_SSR_FEEDBAK_PORT							GPIOD


#define HEATER_SSR_PIN(x)											((1 == x)? SSR_1_PIN: \
																							 (2 == x)? SSR_2_PIN: \
																							 (3 == x)? SSR_3_PIN: \
																							 (4 == x)? SSR_4_PIN: \
																							 (5 == x)? SSR_5_PIN: \
																							 (6 == x)? SSR_6_PIN: \
																							 (7 == x)? SSR_7_PIN: \
																							 (8 == x)? SSR_8_PIN: 0)
														 
#define HEATER_FEEDBACK_PIN(x)								((1 == x)? SSR_1_FEEDBACK_PIN: \
																							 (2 == x)? SSR_2_FEEDBACK_PIN: \
																							 (3 == x)? SSR_3_FEEDBACK_PIN: \
																							 (4 == x)? SSR_4_FEEDBACK_PIN: \
																							 (5 == x)? SSR_5_FEEDBACK_PIN: \
																							 (6 == x)? SSR_6_FEEDBACK_PIN: \
																							 (7 == x)? SSR_7_FEEDBACK_PIN: \
																							 (8 == x)? SSR_8_FEEDBACK_PIN: 0)


#define HEATER_OFF_COMMAND							  		(uint8_t)0x01
#define HEATER_ON_COMMAND								  		(uint8_t)0x02

#define HEATER_OFF							  						Bit_RESET
#define HEATER_ON								  						Bit_SET

#define MIN_TEMP_READING_RANGE								(float)-45.0
#define MAX_TEMP_READING_RANGE								(float)250.0
	
#define MAX_TC_FAULT_COUNT													(uint8_t)5 

#define NO_OF_SSR												(uint8_t)4
#define NO_OF_SPARE_TC_SENSOR						(uint8_t)2
#define NO_OF_SPARE_SSR									(uint8_t)4

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
extern uint16_t uiTempReadCnt; 
extern uint16_t uiTempReadCount;
extern uint8_t ucHardSSRPosition;
extern uint8_t ucCReading;
extern uint8_t ucPreSSRStatus;
extern uint8_t ucSSRStatus;
extern TC_t stTC;

/* ----------------------------------------------------------------------------
*                           EXTERNAL FUNCTIONS
* ----------------------------------------------------------------------------
*/
/* ----------------------------------------------------------------------------
*                           FUNCTIONS DECLARATION
* ----------------------------------------------------------------------------
*/

extern void fnTemprature_Process(uint8_t);
extern void fnHeater_Check(void);
extern void fnHeater_ONOFF(uint8_t, BitAction);
#endif

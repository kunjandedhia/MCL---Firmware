/******************************************************************************
  * @file    Pressure_Sensors.h
  * 
  * @brief   This file contains included files, Macros, Global Variables,
						 External Variables, External Functions
  ****************************************************************************
  */
	#ifndef __Pressure_Sensors_h
	#define __Pressure_Sensors_h
/* ----------------------------------------------------------------------------
 *                           Includes
 * ---------------------------------------------------------------------------
 */
/* ----------------------------------------------------------------------------
 *                           MACROS
 * ---------------------------------------------------------------------------
 */
	#define PG_GPIO_PINS		 						GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4
	
	#define PG_GPIO_PORT 								GPIOC	 /*Pressure Gauge*/

	#define PG_ADC_CHANNLE(x)					  ((1 == x)?	ADC_Channel_10: \
																			 (2 == x)?	ADC_Channel_11: \
																			 (3 == x)?	ADC_Channel_12: \
																			 (4 == x)?	ADC_Channel_13: INVALID_CHANNEL)
																			 
	#define PG_LOW_TH(x)								((0 == x)? fPG1_LowTH:\
																			 (1 == x)? fPG2_LowTH:fPG3_LowTH)
																				 
	#define PG_HIGH_TH(x)								((0 == x)? fPG1_HighTH:\
																			 (1 == x)? fPG2_HighTH:fPG3_HighTH)
																			 
	#define PG_POSITION(x)							((0 == x)? ucPositionPG1:\
																			 (1 == x)? ucPositionPG2:ucPositionPG3)
										 
	/********* AIR PRESSURE SENSOR INTERFACE ************/		
	#define APS_GPIO_PORT 							GPIOA	 /*Air Pressure Sensor*/
	#define APS_GPIO_PIN1		 						GPIO_Pin_2 
	#define APS_GPIO_PIN2		 						GPIO_Pin_3 
	
	/******GPIO PORTS for pressure gauge status tri color LED********/
	#define	PGAUGE3_LED_PORT						GPIOE
	#define	PGAUGE2_LED_PORT						GPIOF
	#define	PGAUGE1_LED_PORT						GPIOB
	
	#define MIN_PRESSURE								(float)0.0
	#define MAX_PRESSURE								(float)400.0
	
	#define PG_CAN_MSG_ID(x)						((PG1 == x)?	DATA_PGAUGE1_VALUE: \
																			 (PG2 == x)?	DATA_PGAUGE2_VALUE: DATA_PGAUGE3_VALUE)
																			 
	#define LOW_PRESSURE_LEVEL						0.0f   			 /* Pressure value in Bar*/
	#define HIGH_PRESSURE_LEVEL						400.0f
	#define LOW_PRESSURE_V								0.6f				/* Voltage corresponds to Low pressure value*/
	#define HIGH_PRESSURE_V								3.0f				/* Voltage corresponds to High pressure value*/
	#define PRESSURE_GAUGE_SLOPE 				((HIGH_PRESSURE_LEVEL - LOW_PRESSURE_LEVEL)/(HIGH_PRESSURE_V - LOW_PRESSURE_V))
	
	#define NUM_OF_PG												(uint8_t)3
	#define PG1															0
	#define PG2															1
	#define PG3															2
	
	#define PG_ROCESS												1
	#define STALL_CALL											2
	#define DIAGNOS_CALL											2
	#define CURRENT_PG_DATA(x)							((0 == x)? fCurrentPG1 : \
																					 (1 == x)? fCurrentPG2 : fCurrentPG3)
																					 
	
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
	extern PG_t stPG[NUM_OF_PG];
	
	extern float fCurrentPG1;
	extern float fCurrentPG2;
	extern float fCurrentPG3;
	
	extern float fPG1_LowTH;
	extern float fPG1_HighTH;
	 
	extern float fPG2_LowTH;
	extern float fPG2_HighTH;
	 
	extern float fPG3_LowTH;
	extern float fPG3_HighTH;
	
extern uint8_t ucPositionPG1;		  						/*Connected position on the Board connector*/
extern uint8_t ucPositionPG2;		  						/*Connected position on the Board connector*/
extern uint8_t ucPositionPG3;		  						/*Connected position on the Board connector*/
 /* ---------------------------------------------------------------------------
 *                           EXTERNAL FUNCTIONS
 * ---------------------------------------------------------------------------
 */
 extern void fnPG_Process(void);
 extern void fnPGSensor_Read(uint8_t);
/* ----------------------------------------------------------------------------
 *                           FUNCTIONS DECLARITION
 * ---------------------------------------------------------------------------
 */
	#endif


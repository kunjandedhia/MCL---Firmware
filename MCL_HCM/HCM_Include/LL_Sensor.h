
/******************************************************************************
* @file   LL_Sensor.h 
* 
* @brief  This Header File contains declaration of all the external function,
					Macros and External variables related Liquid Level Sensor.
******************************************************************************
*/    
#ifndef __LL_Sensor_h
#define __LL_Sensor_h

/* ----------------------------------------------------------------------------
*                           Includes
* ----------------------------------------------------------------------------
*/
#include "Structures.h"
/* ----------------------------------------------------------------------------
*                           MACROS
* ----------------------------------------------------------------------------
*/

#define LL_NOT_CONNECTED 				(uint8_t)0xAA
#define LIQUID_SENSOR_PIN        		GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8
#define LIQUID_LEVEL_CH(x) 				((1 == x)? ADC_Channel_4 : \
										 (2 == x)? ADC_Channel_5 : \
										 (3 == x)? ADC_Channel_6 : LL_NOT_CONNECTED) 
#define LIQUID_SENSOR_PORT				GPIOF
#define ADC_NUM							ADC3
#define RCC_APB2Periph					RCC_APB2Periph_ADC3
#define NUMBER_OF_ADC_CHANNEL			3

/*(3.3V/2^12) ;3.3V is maximum input voltage; 4095:12 bit ADC value */
#define ADCRESOLUTION					(3.3f/4095.0f) 
#define LOW_LIQUID_LEVEL				200.0f    /* Liquid level in mm*/
#define HIGH_LIQUID_LEVEL				2000.0f
#define LOW_LIQUID_LEVEL_V				0.6f			/* Voltage corresponds to Low liquid level in Volts*/
#define HIGH_LIQUID_LEVEL_V				3.0				/* Voltage corresponds to High liquid level in Volts*/

/* to calculate liqid level from read voltage*/
#define LIQUID_LEVEL_SLOPE 				((HIGH_LIQUID_LEVEL - LOW_LIQUID_LEVEL)/ \
																	 (HIGH_LIQUID_LEVEL_V - LOW_LIQUID_LEVEL_V)) 
#define DATA_READ_TIME_LL						(uint16_t)10		/*Liquid Level Data Read Time = 10 * 10ms = 100 mSecond*/

#define BASE_TANK															(uint8_t)0x01
#define HARD_TANK															(uint8_t)0x02

#define MIN_LIQ_READING_RANGE								 	200.0f
#define MAX_LIQ_READING_RANGE									2000.0f
#define NO_OF_LL_SENSOR									(uint8_t)2

 
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
extern uint8_t ucLLWindowSize; 
extern uint16_t uiTimCountLL;
extern LL_t stLL[NO_OF_LL_SENSOR];

/* ----------------------------------------------------------------------------
*                           EXTERNAL FUNCTIONS
* ----------------------------------------------------------------------------
*/
extern void fnADC_Init(void);
extern float fnADC_Read(uint8_t);
extern void fnLiquidLevel_Check(uint8_t);

/* ----------------------------------------------------------------------------
*                           FUNCTIONS DECLARATION
* ----------------------------------------------------------------------------
*/
#endif


/******************************************************************************
  **@file     :    Linear_Transducer.h
  * 
  * @brief  	:		 This file contains included files, Macros, Global Variables,
									 External Variables, External Functions
  ******************************************************************************
  */	
	#ifndef __Linear_Transducer_h
	#define __Linear_Transducer_h
/* ----------------------------------------------------------------------------
*                           Includes
* ----------------------------------------------------------------------------
*/
/* ----------------------------------------------------------------------------
*                           MACROS
* ----------------------------------------------------------------------------
*/
	#define INVALID_CHANNEL							(uint8_t)0xAA
	
	/*********GPIOs For the ADC channels************/
	#define LT_GPIO_PINS 								GPIO_Pin_6 | GPIO_Pin_7
	
	/*********GPIO PORTS For the ADC channels************/
	#define LT_GPIO_PORT 								GPIOF	 /*Linear Transducer*/	
	
	/********* ADC channels ************/		
	#define LINEAR_TRANSDUCER_CH(x)			((1 == x)?	ADC_Channel_4: \
	  																	 (2 == x)?	ADC_Channel_5: INVALID_CHANNEL)
																			 
	#define NUM_OF_ADC_CHANNEL					(uint8_t)6
	
	/****Macros for linear transducer position-1 interrupt*****/
	/*LT1*/
	#define LT1_INTR_POSITION_1_PIN			GPIO_Pin_8
	#define LT_POSITION_PORT						GPIOD
	#define LT1_EXTI_LINE_NO						EXTI_Line8
	#define LT_RCC_APBPERIPH_GPIO				RCC_APB2Periph_GPIOD
	#define LT_GPIO_PORTSOURCE					GPIO_PortSourceGPIOD
	#define LT1_GPIO_PINSOURCE					GPIO_PinSource8
	#define LT1_EXTI_IRQN								EXTI9_5_IRQn
	
	/*LT2*/
	#define LT2_INTR_POSITION_1_PIN			GPIO_Pin_10
	#define LT2_EXTI_LINE_NO						EXTI_Line10
	#define LT2_GPIO_PINSOURCE					GPIO_PinSource10
	#define LT2_EXTI_IRQN								EXTI15_10_IRQn
	
	
	
	#define MATERIAL_USAGE_CHECKCOUNTER		(uint16_t)300
		
	#define LOW_LT_LEVEL									0.0f   			 /* Linear Transducer value in mm(millimeter)*/
	#define HIGH_LT_LEVEL									150.0f
	#define LOW_LT_V											0.6f				/* Voltage corresponds to Low reading of Linear transducer*/
	#define HIGH_LT_V											3.0f				/* Voltage corresponds to High reading of Linear transducer*/
	#define LT_SLOPE 										((HIGH_LT_LEVEL - LOW_LT_LEVEL)/(HIGH_LT_V - LOW_LT_V))
	
	#define LT_EXT_INT_VALUE							27.5f
	#define FIRST_LENGTH									1
	#define LAST_LENGTH										2
	
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
	extern float fLinearTrns;
	extern float fMaterialUsedA;
	extern float fMaterialUsedB;
	extern float fMaterialUsedC;
	extern float fLinearTransMin;
	extern float fLinearTransMax;
	extern float fLinearTransCurrRead;
	extern float fLTFirstRead;
	extern float fLTLastRead;
	extern uint8_t ucStrokeOfType;
	extern uint8_t ucStrokeOfType;
	extern uint8_t ucLTPostion;
/* ----------------------------------------------------------------------------
*                           EXTERNAL FUNCTIONS
* ----------------------------------------------------------------------------
*/
	extern void fnADC_Init(void);
	extern uint16_t fnGetADCChannel(uint8_t);
	extern float fnADC_value(uint8_t);
	extern float fnLinearPosition(void);
	extern void fnExtIntr_Config(void);
	extern void fnLinearTransLimit(void);
	extern uint8_t fnStrokeType(void);
	extern float fnLTFirstLastLength_Read(float, uint8_t, uint8_t);
	extern void fnMaterial_Used(void);
	extern void fnLT_Process(void);
/* ----------------------------------------------------------------------------
*                           FUNCTIONS DECLARATION
* ----------------------------------------------------------------------------
*/
	#endif



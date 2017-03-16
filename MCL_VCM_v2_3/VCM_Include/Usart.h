/******************************************************************************
* @file    Usart.h
* 
* @brief   This file contains included files, Macros, Global Variables,
					 External Variables, External Functions
******************************************************************************
*/
	#ifndef __usart_h
	#define __usart_h
/* ----------------------------------------------------------------------------
*                           Includes
* ----------------------------------------------------------------------------
*/
	#include "Macro.h"
/* ----------------------------------------------------------------------------
*                           MACROS
* ----------------------------------------------------------------------------
*/
	/**************USART*******************/
	#ifdef USART_DBG
		#define RCC_APB2PERIPH_GPIO					RCC_APB2Periph_GPIOA
		#define RCC_APBPERIPH_USART					RCC_APB2Periph_USART1
		#define USART_TX_PIN								GPIO_Pin_9
		#define USART_RX_PIN								GPIO_Pin_10
		#define USART_GPIO_PORT							GPIOA
		#define USART_BAUD_RATE							(uint32_t)115200
		#define USART												USART1
		#define USART_IRQN									USART1_IRQn
	#endif	
	/**************UPS USART*******************/
	#define RCC_APB2PERIPH_GPIO_UPS		RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD
	#define RCC_APBPERIPH_UART_UPS		RCC_APB1Periph_UART5
	#define UPS_TX_PIN								GPIO_Pin_12
	#define UPS_RX_PIN								GPIO_Pin_2
	#define UPS_TX_GPIO_PORT					GPIOC
	#define UPS_RX_GPIO_PORT					GPIOD
	#define UPS_BAUD_RATE							(uint32_t)115200
	#define UPS_UART									UART5
	#define UPS_UART_IRQN							UART5_IRQn
	#define UPS_INPUT_PIN							GPIO_Pin_0	
	#define UPS_OUTPUT_PIN						GPIO_Pin_1		
	#define UPS_GPIO_PORT							GPIOD		
	
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
	#ifdef USART_DBG
		extern void fnUSARTDBG_Config(void);
	#endif

	extern void fnUPS_Config(void);
	extern int UARTgets(char *pcBuf, unsigned long ulLen);
	extern void UARTprintf(const char *pcString, ...);
	extern int UARTwrite(const char *pcBuf, unsigned long ulLen);
/* ----------------------------------------------------------------------------
*                           FUNCTIONS DECLARITION
* ----------------------------------------------------------------------------
*/
 #endif





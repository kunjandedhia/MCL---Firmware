/******************************************************************************
* @file    Usart.c
* 
* @brief   This file contains the UART Funtions
******************************************************************************
*/    
/* ----------------------------------------------------------------------------
*                           Includes
* ----------------------------------------------------------------------------
*/
#include "Includes.h"
/* ----------------------------------------------------------------------------
*                           MACROS
* ----------------------------------------------------------------------------
*/
/* ----------------------------------------------------------------------------
*                           CONSTANTS
* ----------------------------------------------------------------------------
*/
/* ----------------------------------------------------------------------------
*                           GLOBAL VARIABLES
* ----------------------------------------------------------------------------
*/
#ifdef TEST_DBG 
	char rgucRecString[100];
	uint8_t ucloopC;
#endif
/* ----------------------------------------------------------------------------
*                           EXTERNAL VARIABLES
* ----------------------------------------------------------------------------
*/

/* ----------------------------------------------------------------------------
*                           EXTERNAL FUNCTIONS
* ----------------------------------------------------------------------------
*/
/* ----------------------------------------------------------------------------
*                           FUNCTIONS DECLARATION
* ----------------------------------------------------------------------------
*/
/******************************************************************************
 **@function 		:	fnUSART_init
 **@Description	: Usart2_Init funtion will initialize usart2 port.
 **@Parameter		: None
 **@Return			: None
 *****************************************************************************/
#ifdef USART_DBG
void fnUSART_init(void)
{
	USART_InitTypeDef stUSART_InitStructure;
	GPIO_InitTypeDef stGPIO_InitStructure;
	NVIC_InitTypeDef stNVIC_InitStructure;

	/* Clock configuration */
	/* Configure the GPIO ports( USART2 Transmit and Receive Lines) */
	/* Configure the USART2_Tx as Alternate function Push-Pull */
	/* Configure USART2_Tx as alternate function push-pull */

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO ,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	/* Configure USART2_Tx as input floating                */
	stGPIO_InitStructure.GPIO_Pin		= GPIO_Pin_9;
	stGPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	stGPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &stGPIO_InitStructure);

	/* Configure USART2_Rx as input floating                */
	stGPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_10;
	stGPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &stGPIO_InitStructure);

	/* USART2 configured as follow:
	- BaudRate = 115200 baud
	- Word Length = 8 Bits
	- One Stop Bit
	- No parity
	- Hardware flow control disabled (RTS and CTS signals)
	- Receive and transmit enabled
	*/

	stUSART_InitStructure.USART_BaudRate	 					= 115200;
	stUSART_InitStructure.USART_WordLength 					= USART_WordLength_8b;
	stUSART_InitStructure.USART_StopBits 	 					= USART_StopBits_1;
	stUSART_InitStructure.USART_Parity 							= USART_Parity_No;
	stUSART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	stUSART_InitStructure.USART_Mode 								= USART_Mode_Rx | USART_Mode_Tx;

	/* Configure the USART2 */
	USART_Init(USART1, &stUSART_InitStructure);
	stNVIC_InitStructure.NVIC_IRQChannel 										= USART1_IRQn;
	stNVIC_InitStructure.NVIC_IRQChannelPreemptionPriority 	= 1;
	stNVIC_InitStructure.NVIC_IRQChannelSubPriority 				= 0;
	stNVIC_InitStructure.NVIC_IRQChannelCmd 								= ENABLE;
	NVIC_Init(&stNVIC_InitStructure);

	USART_ClearFlag(USART1, USART_FLAG_CTS | USART_FLAG_LBD  | USART_FLAG_TXE  |
									USART_FLAG_TC  | USART_FLAG_RXNE | USART_FLAG_IDLE |
									USART_FLAG_ORE | USART_FLAG_NE   | USART_FLAG_FE |
									USART_FLAG_PE);

	/* Enable USART2 interrupt*/
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);

	/* Enable the USART2 */
	USART_Cmd(USART1,ENABLE);
}
#endif







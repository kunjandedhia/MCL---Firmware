/******************************************************************************
  * @file    Uart.c
  * 
  * @brief   This file contains the USART Initialization functions
  ****************************************************************************
  */
/* ----------------------------------------------------------------------------
 *                           Includes
 * ---------------------------------------------------------------------------
 */
 #include "Includes.h"
/* ----------------------------------------------------------------------------
 *                           MACROS
 * ---------------------------------------------------------------------------
 */
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

 /* ----------------------------------------------------------------------------
 *                           EXTERNAL FUNCTIONS
 * ----------------------------------------------------------------------------
 */
/* ----------------------------------------------------------------------------
 *                           FUNCTIONS DECLARITION
 * ---------------------------------------------------------------------------
 */
	#ifdef USART_DBG
		void fnUSARTDBG_Config(void);
	#endif
/******************************************************************************
 **@Function 		: USART_Configuration
 **@Descriptions: Initialize USART
 **@Parameters	: None
 **@Return			: None
 ****************************************************************************/
#ifdef USART_DBG
void fnUSARTDBG_Config(void)
{
	USART_InitTypeDef stUSART_InitStructure;
	GPIO_InitTypeDef stGPIO_InitStructure;
	NVIC_InitTypeDef stNVIC_InitStructure;

	/* Clock configuration ---------------------------------------------*/
	/* Configure the GPIO ports( USART Transmit and Receive Lines) */
	/* Configure the USART_Tx as Alternate function Push-Pull */
	/* Configure USART_Rx as IN_FLOATING */
	RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIO_UPS | RCC_APB2Periph_AFIO ,ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APBPERIPH_USART, ENABLE);

	/* Configure USART2_Tx as input floating                */
	stGPIO_InitStructure.GPIO_Pin 		= USART_TX_PIN;
	stGPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
	stGPIO_InitStructure.GPIO_Mode 		= GPIO_Mode_AF_PP;
	GPIO_Init(USART_GPIO_PORT, &stGPIO_InitStructure);

	/* Configure USART2_Rx as input floating                */
	stGPIO_InitStructure.GPIO_Pin 		= USART_RX_PIN;
	stGPIO_InitStructure.GPIO_Mode 		= GPIO_Mode_IN_FLOATING;
	GPIO_Init(USART_GPIO_PORT, &stGPIO_InitStructure);

	/* USART configured as follow:
	- Word Length = 8 Bits
	- One Stop Bit
	- No parity
	- Hardware flow control disabled (RTS and CTS signals)
	- Receive and transmit enabled
	*/

	stUSART_InitStructure.USART_BaudRate 						= USART_BAUD_RATE;
	stUSART_InitStructure.USART_WordLength 					= USART_WordLength_8b;
	stUSART_InitStructure.USART_StopBits 						= USART_StopBits_1;
	stUSART_InitStructure.USART_Parity 							= USART_Parity_No;
	stUSART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	stUSART_InitStructure.USART_Mode 								= USART_Mode_Rx | USART_Mode_Tx;

	/* Configure the USART2 */
	USART_Init(USART, &stUSART_InitStructure);
	stNVIC_InitStructure.NVIC_IRQChannel 										= USART_IRQN;
	stNVIC_InitStructure.NVIC_IRQChannelPreemptionPriority 	= 7;
	stNVIC_InitStructure.NVIC_IRQChannelSubPriority 				= 4;
	stNVIC_InitStructure.NVIC_IRQChannelCmd 								= ENABLE;
	NVIC_Init(&stNVIC_InitStructure);

	USART_ClearFlag(USART, USART_FLAG_CTS | USART_FLAG_LBD  | USART_FLAG_TXE  |
									USART_FLAG_TC  | USART_FLAG_RXNE | USART_FLAG_IDLE |
									USART_FLAG_ORE | USART_FLAG_NE   | USART_FLAG_FE |
									USART_FLAG_PE);

   /*Enable USART interrupt */
   USART_ITConfig(USART,USART_IT_RXNE,ENABLE);

	/*Enable the USART */
	USART_Cmd(USART,ENABLE);
}
#endif

/******************************************************************************
 **@Function 		: fnUPS_Config
 **@Descriptions: Initialize UART5 for the UPS 
 **@Parameters	: None
 **@Return			: None
 ****************************************************************************/
void fnUPS_Config(void)
{
	USART_InitTypeDef stUSART_InitStructure;
	GPIO_InitTypeDef stGPIO_InitStructure;
	NVIC_InitTypeDef stNVIC_InitStructure;

	/* Clock configuration ---------------------------------------------*/
	/* Configure the GPIO ports( USART Transmit and Receive Lines) */
	/* Configure the USART_Tx as Alternate function Push-Pull */
	/* Configure USART_Rx as IN_FLOATING */

	RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIO_UPS | RCC_APB2Periph_AFIO ,ENABLE);

	RCC_APB1PeriphClockCmd(RCC_APBPERIPH_UART_UPS, ENABLE);

	/* Configure USART2_Tx as input floating                */
	stGPIO_InitStructure.GPIO_Pin 		= UPS_TX_PIN;
	stGPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
	stGPIO_InitStructure.GPIO_Mode 		= GPIO_Mode_AF_PP;
	GPIO_Init(UPS_TX_GPIO_PORT, &stGPIO_InitStructure);

	/* Configure USART2_Rx as input floating                */
	stGPIO_InitStructure.GPIO_Pin 		= UPS_RX_PIN;
	stGPIO_InitStructure.GPIO_Mode 		= GPIO_Mode_IN_FLOATING;
	GPIO_Init(UPS_RX_GPIO_PORT, &stGPIO_InitStructure);

	/* USART configured as follow:
	- Word Length = 8 Bits
	- One Stop Bit
	- No parity
	- Hardware flow control disabled (RTS and CTS signals)
	- Receive and transmit enabled
	*/
	stUSART_InitStructure.USART_BaudRate 						= UPS_BAUD_RATE;
	stUSART_InitStructure.USART_WordLength 					= USART_WordLength_8b;
	stUSART_InitStructure.USART_StopBits 						= USART_StopBits_1;
	stUSART_InitStructure.USART_Parity 							= USART_Parity_No;
	stUSART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	stUSART_InitStructure.USART_Mode 								= USART_Mode_Rx | USART_Mode_Tx;
	/* Configure the USART2 */
	USART_Init(UPS_UART, &stUSART_InitStructure);
	
	stNVIC_InitStructure.NVIC_IRQChannel 										= UPS_UART_IRQN;
	stNVIC_InitStructure.NVIC_IRQChannelPreemptionPriority 	= 7;
	stNVIC_InitStructure.NVIC_IRQChannelSubPriority 				= 1;
	stNVIC_InitStructure.NVIC_IRQChannelCmd 								= ENABLE;
	NVIC_Init(&stNVIC_InitStructure);

	USART_ClearFlag(UPS_UART, USART_FLAG_CTS | USART_FLAG_LBD  | USART_FLAG_TXE  |
									USART_FLAG_TC  | USART_FLAG_RXNE | USART_FLAG_IDLE |
									USART_FLAG_ORE | USART_FLAG_NE   | USART_FLAG_FE |
									USART_FLAG_PE);

	/*Enable USART interrupt */
	USART_ITConfig(UPS_UART, USART_IT_RXNE, ENABLE);

	/*Enable the USART */
	USART_Cmd(UPS_UART,ENABLE);
}



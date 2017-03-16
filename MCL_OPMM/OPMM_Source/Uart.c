/******************************************************************************
**@file    Uart.c
* 
**@brief   This file contains USART configuration function
******************************************************************************
*/
/* ----------------------------------------------------------------------------
*                           Includes
* ----------------------------------------------------------------------------
*/
#include "Config.h"
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
 **@Function 			: USART_Configuration
 **@Descriptions	: Initialize USART
 **@Parameters		: None
 **@Return				: None
 *****************************************************************************/
#ifdef UART_DBG
void fnUSART_Config(void)
{
  USART_InitTypeDef stUSART_InitStructure;
	GPIO_InitTypeDef stGPIO_InitStructure;
	NVIC_InitTypeDef stNVIC_InitStructure;

	/* Clock configuration ----------------------------------------*/
	/* Configure the GPIO ports( USART Transmit and Receive Lines) */
	/* Configure the USART_Tx as Alternate function Push-Pull */
	/* Configure USART_Rx as IN_FLOATING */

	RCC_APB2PeriphClockCmd(RCC_APB2PERIPH_GPIO | RCC_APB2Periph_AFIO ,ENABLE);
    
		
	RCC_APB2PeriphClockCmd(RCC_APBPERIPH_USART, ENABLE);
	
		
   /* Configure USART_Tx as input floating*/
   stGPIO_InitStructure.GPIO_Pin = USART_TX_PIN;
   stGPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   stGPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
   GPIO_Init(USART_GPIO_PORT, &stGPIO_InitStructure);

   /* Configure USART_Rx as input floating*/
   stGPIO_InitStructure.GPIO_Pin = USART_RX_PIN;
   stGPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
   GPIO_Init(USART_GPIO_PORT, &stGPIO_InitStructure);

   /* USART configured as follow:
   - Word Length = 8 Bits
   - One Stop Bit
   - No parity
   - Hardware flow control disabled (RTS and CTS signals)
   - Receive and transmit enabled
   */

   stUSART_InitStructure.USART_BaudRate = USART_BAUD_RATE;
   stUSART_InitStructure.USART_WordLength = USART_WordLength_8b;
   stUSART_InitStructure.USART_StopBits = USART_StopBits_1;
   stUSART_InitStructure.USART_Parity = USART_Parity_No;
   stUSART_InitStructure.USART_HardwareFlowControl = 
																		USART_HardwareFlowControl_None;
   stUSART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	 
   /* Configure the USART */
   USART_Init(USART, &stUSART_InitStructure);
   stNVIC_InitStructure.NVIC_IRQChannel = USART_IRQN;
   stNVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
   stNVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
	 stNVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   NVIC_Init(&stNVIC_InitStructure);

   USART_ClearFlag(USART, USART_FLAG_CTS | USART_FLAG_LBD | USART_FLAG_TXE |
                    USART_FLAG_TC | USART_FLAG_RXNE | USART_FLAG_IDLE |
                    USART_FLAG_ORE | USART_FLAG_NE  | USART_FLAG_FE |
                    USART_FLAG_PE);

   /*Enable USART interrupt */
  // USART_ITConfig(USART,USART_IT_RXNE,ENABLE);
	
   /*Enable the USART */
   USART_Cmd(USART,ENABLE);
}
#endif

/******************************************************************************
* @file    System_Init.c 
* 
* @brief   This file contains system initialization funtions definition
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
void fnGPIO_Init(void);
/******************************************************************************
 **@Function			: 	fnSystem_Init
 **@Descriptions	: 	This function initialization All the System Peripherals
 **@Parameters		:  	None
 **@Return		  	:  	None
 ****************************************************************************/
void fnSystem_Init(void)
{
	/* Set the Vector Table base location*/
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
	#ifdef USART_DBG
		/* Initializes the USART2 for Debugging purpose */
		fnUSART_init();
	#endif
	/* GPIO Initialization*/
	fnGPIO_Init();
		
	/*CAN Initialization */
	fnCAN_Config();
	fnCAN_MsgInit();
	
	#ifdef GENERAL_DBG
		DEBUG_PRINTF("CAN INIT !\n");
	#endif	
	
	/* Tim1 initialization for 10msec timeout */
	fnTIM1_Init();
	
	/* Initialization for Temperature sensor*/
	fnSPI_Init(); 

  /*Initialization for liquid level sensor*/
	fnADC_Init();
	
	/*Initialization of I2C for IO Expander(for LED Indication)*/
	fnI2C_Config();
	
	fnDefault_Setting();
}
/******************************************************************************
 **@Function			: fnGPIO_Init
 **@Descriptions	: Initialize the GPIO 
 **@Parameters		: None
 **@Return				: None
 ****************************************************************************/
void fnGPIO_Init(void)
{ 
	uint8_t ucLoop;
	GPIO_InitTypeDef stGPIO_InitStructure;  

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | 
												 RCC_APB2Periph_GPIOC	| RCC_APB2Periph_GPIOD | 
												 RCC_APB2Periph_GPIOE	| RCC_APB2Periph_GPIOF | 
												 RCC_APB2Periph_GPIOG, ENABLE);
	
  RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
												 RCC_APB2Periph_GPIOC	| RCC_APB2Periph_GPIOD |
												 RCC_APB2Periph_GPIOE	| RCC_APB2Periph_GPIOF | 
												 RCC_APB2Periph_GPIOG, DISABLE);
	
	/**********************ON BOARD STATUS LED********************/
	
  /**********     HEATER GPIO    ******************/
	/*   Heater Switch Pin initialization         ***/
	stGPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  stGPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	stGPIO_InitStructure.GPIO_Pin = SSR_1_PIN | SSR_2_PIN |
																	SSR_3_PIN | SSR_4_PIN | 
																	SSR_5_PIN | SSR_6_PIN | 
																	SSR_7_PIN | SSR_8_PIN;
	
	GPIO_Init(HEATER_SSR_PORT, &stGPIO_InitStructure);
	/*Off all the SSR GPIO's*/
	for(ucLoop = 0; ucLoop < 8; ucLoop++)
	{
		fnHeater_ONOFF(ucLoop, HEATER_OFF);
	}
	
	/****************Heater SSR Feedback Pin initialization************/
	stGPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  stGPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	stGPIO_InitStructure.GPIO_Pin = SSR_1_FEEDBACK_PIN | SSR_2_FEEDBACK_PIN |
																	SSR_3_FEEDBACK_PIN | SSR_4_FEEDBACK_PIN |
																	SSR_5_FEEDBACK_PIN | SSR_6_FEEDBACK_PIN |
																	SSR_7_FEEDBACK_PIN | SSR_8_FEEDBACK_PIN;
	
	GPIO_Init(HEATER_SSR_FEEDBAK_PORT, &stGPIO_InitStructure);

  /************* CAN GPIO *****************/
	/***************    CAN ENABLE GPIO PIN init  ***************/
	stGPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  stGPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	stGPIO_InitStructure.GPIO_Pin = CAN_ENABLE_GPIO_PIN;
	GPIO_Init(CAN_ENABLE_GPIO_PORT, &stGPIO_InitStructure);
	
  /*****************  CAN STROBE GPIO PIN initialization  **************/
	stGPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  stGPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	stGPIO_InitStructure.GPIO_Pin = CAN_STROBE_GPIO_PIN;
	
	GPIO_Init(CAN_STROBE_GPIO_PORT, &stGPIO_InitStructure);
	
  /****** CAN ERROR GPIO PIN Initialization *************/
	stGPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  stGPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	stGPIO_InitStructure.GPIO_Pin = CAN_ERROR_GPIO_PIN;
	
	GPIO_Init(CAN_ERROR_GPIO_PORT, &stGPIO_InitStructure);
}

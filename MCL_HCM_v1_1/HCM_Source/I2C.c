/******************************************************************************
**@file    I2C_GPIO_Expander.c
* 
**@brief   This file contains all the functions related to GPIO expander and I2C
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
uint16_t uiIOExpander1 		= IO1_ALL_RED_LED; /*Variable for I/O Expander 1*/
uint16_t uiIOExpander2 		= IO2_ALL_RED_LED; /*Variable for I/O Expander 2*/
uint16_t uiPreIOExpander1 = 0;	/*Previous value of I/O Expander 1*/
uint16_t uiPreIOExpander2 = 0;	/*Previous value of I/O Expander 2*/
uint16_t uiI2CWriteData   = 0;
uint8_t  ucI2C1Seq				=	0;
uint8_t  ucIOExpSlaveAddr	=	0;
uint8_t ucPostLedBlinkCnt = 0;

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
/*****************************************************************************
 **@Function 			: fnI2C_Config
 **@Descriptions	: Initialise the I2C peripheral
 **@Parameters		: None
 **@Return				: None
 *****************************************************************************/
void fnI2C_Config(void)
{
	GPIO_InitTypeDef stGPIOinit;
	I2C_InitTypeDef stI2Cinit;
	NVIC_InitTypeDef stNVIC_InitStructure;

	/* enable APB1 peripheral clock for I2C1 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

	/* enable clock for SCL and SDA pins */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	
	/* setup SCL and SDA pins PB 6 and PB 7 */
	stGPIOinit.GPIO_Pin 							= I2C_SCL | I2C_SDA; 	
	stGPIOinit.GPIO_Mode 							= GPIO_Mode_AF_OD;		
	stGPIOinit.GPIO_Speed 						= GPIO_Speed_50MHz;	
	GPIO_Init(I2C_PORT, &stGPIOinit);					

	/* configure I2C1 */
	stI2Cinit.I2C_ClockSpeed 					= I2C_CLOCK_SPEED; 	/*	100kHz */
	stI2Cinit.I2C_Mode 								= I2C_Mode_I2C;			/* I2C mode */
	stI2Cinit.I2C_DutyCycle 					= I2C_DutyCycle_2;	/* 50% duty cycle --> standard */
	stI2Cinit.I2C_OwnAddress1 				= 0x00;							/* own address, not relevant in master mode */
	stI2Cinit.I2C_Ack 								= I2C_Ack_Enable;		/* enable acknowledge */
	/* set address length to 7 bit addresses */
	stI2Cinit.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; 
	I2C_Init(I2C_IO_EXP, &stI2Cinit);													
	
	/* Enable I2C1 */
	I2C_Cmd(I2C_IO_EXP, ENABLE);
	
	/* Configure the I2C event priority */
	stNVIC_InitStructure.NVIC_IRQChannel                   = I2C1_EV_IRQn;
	stNVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	stNVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0x01;
	stNVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
	NVIC_Init(&stNVIC_InitStructure);
	
	/* Configure the I2C error condition */
	stNVIC_InitStructure.NVIC_IRQChannel                   = I2C1_ER_IRQn;
	stNVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	stNVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0x03;
	stNVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
	NVIC_Init(&stNVIC_InitStructure);
	
	I2C_ITConfig(I2C_IO_EXP , I2C_IT_EVT | I2C_IT_BUF | I2C_IT_ERR, ENABLE);
	I2C_Cmd(I2C_IO_EXP, ENABLE);
}

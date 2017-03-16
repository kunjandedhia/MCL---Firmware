/******************************************************************************
**@file    I2C_Func.c
* 
**@brief   This file contains all the functions related to GPIO expander and I2C
******************************************************************************
*/    
/* ----------------------------------------------------------------------------
*                           Includes
* ----------------------------------------------------------------------------
*/
#include "System_Header.h"

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
/*****************************************************************************
**@Function 		: fnI2C_Configuration
**@parameters		: none
**@return				: none
**@Descriptions	: Initialise the I2C1 peripheral
*****************************************************************************/
void fnI2C_Configuration(void)
{
		GPIO_InitTypeDef stGPIOinit;
		I2C_InitTypeDef stI2Cinit;
		NVIC_InitTypeDef stNVIC_InitStructure;
	
		/* enable APB1 peripheral clock for I2C1 */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	
		/* enable clock for SCL and SDA pins */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
		
	
		/* setup SCL and SDA pins PB 6 and PB 7 */
		stGPIOinit.GPIO_Pin = I2C_SCL | I2C_SDA; 			/* we are going to use SCL and SDA*/
		stGPIOinit.GPIO_Mode = GPIO_Mode_AF_OD;				/* set pins to alternate function open drain */
		stGPIOinit.GPIO_Speed = GPIO_Speed_50MHz;			/* set GPIO speed */
		GPIO_Init(I2C_PORT, &stGPIOinit);							/* init GPIOB */

		/* configure I2C1 */
		stI2Cinit.I2C_ClockSpeed = I2C_SLOCK_SPEED; 	/*	100kHz */
		stI2Cinit.I2C_Mode = I2C_Mode_I2C;						/* I2C mode */
		stI2Cinit.I2C_DutyCycle = I2C_DutyCycle_2;		/* 50% duty cycle --> standard */
		stI2Cinit.I2C_OwnAddress1 = 0x00;							/* own address, not relevant in master mode */
		stI2Cinit.I2C_Ack = I2C_Ack_Enable;						/* enable acknowledge */
		stI2Cinit.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; /* set address length to 7 bit addresses */
		I2C_Init(I2C1, &stI2Cinit);													
		
		/* Configure the I2C event priority */
		stNVIC_InitStructure.NVIC_IRQChannel                   = I2C1_EV_IRQn;
		stNVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
		stNVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
		stNVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
		NVIC_Init(&stNVIC_InitStructure);
		
		I2C_ITConfig(I2C1, I2C_IT_EVT | I2C_IT_BUF, ENABLE);
    /* Enable I2C1 */
		I2C_Cmd(I2C1, ENABLE);
}

/******************************************************************************
**@file    I2C_GPIO_Expander.c
* 
**@brief   This file contains all functions related to GPIO expander and I2C
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
 **@Function 			: fnI2C_Config
 **@Parameters		: None
 **@Return				: None
 **@Descriptions	: Initialise the I2C1 peripheral
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
	stGPIOinit.GPIO_Pin 		= I2C_SCL | I2C_SDA;/* we are going to use SCL and SDA*/
	stGPIOinit.GPIO_Mode 		= GPIO_Mode_AF_OD;
	stGPIOinit.GPIO_Speed 	= GPIO_Speed_50MHz;/* set GPIO speed */
	GPIO_Init(I2C_PORT, &stGPIOinit);/* init GPIOB */

	/* configure I2C1 */
	stI2Cinit.I2C_ClockSpeed 					= I2C_SLOCK_SPEED; 	/*	100kHz */
	stI2Cinit.I2C_Mode 								= I2C_Mode_I2C;			/* I2C mode */
	stI2Cinit.I2C_DutyCycle 					= I2C_DutyCycle_2;	/* 50% duty cycle --> standard */
	stI2Cinit.I2C_OwnAddress1 				= 0x00;
	stI2Cinit.I2C_Ack 								= I2C_Ack_Enable;		/* enable acknowledge */
	/* set address length to 7 bit addresses */
	stI2Cinit.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; 
	I2C_Init(I2C1, &stI2Cinit);													
	
	/* Enable I2C1 */
	I2C_Cmd(I2C1, ENABLE);
	
	/* Configure the I2C event priority */
	stNVIC_InitStructure.NVIC_IRQChannel                   = I2C1_EV_IRQn;
	stNVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	stNVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0x01;
	stNVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
	NVIC_Init(&stNVIC_InitStructure);
	
	/* Configure the I2C error condition */
	stNVIC_InitStructure.NVIC_IRQChannel                   = I2C1_ER_IRQn;
	stNVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	stNVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0x02;
	stNVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
	NVIC_Init(&stNVIC_InitStructure);
	
	I2C_ITConfig(I2C1, I2C_IT_EVT | I2C_IT_BUF | I2C_IT_ERR, ENABLE);
	I2C_Cmd(I2C1, ENABLE);
}

/******************************************************************************
 **@Function 			: I2C_start
 **@Descriptions	: This function issues a start condition and transmits slave 
										address + R/W bit
 **@Parameters		:
										* I2Cx --> the I2C peripheral e.g. I2C1
										* ucAddress --> the 7 bit slave Address
										* direction --> the transmission direction can be:
										* I2C_Direction_Tranmitter for Master transmitter mode
										* I2C_DIRECTION_RECEIVER for Master receiver
 **@Return				: None
 *****************************************************************************/
void fnI2C_Start(I2C_TypeDef* I2Cx, uint8_t ucAddress, uint8_t ucDirection)
{
	/* wait until I2C1 is not busy any more */
	while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY));
	
	/* wait for I2C1 EV5 --> Slave has acknowledged start condition */
	while(!(I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT)));
	/* Send slave Address for write */
	I2C_Send7bitAddress(I2Cx, ucAddress, ucDirection);

	/* wait for I2Cx EV6, check if either Slave has acknowledged Master 
			transmitter or Master receiver */
	if(I2C_DIRECTION_TRANSMITTER == ucDirection)
	{
		while(!(I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)));
	}
	else if(I2C_DIRECTION_RECEIVER == ucDirection)
	{
		while(!(I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)));
	}
}

/******************************************************************************
 **@Function 			: I2C_stop
 **@Descriptions	: This function issues a stop condition and hence releases bus
 **@Parameters		: I2Cx --> the I2C peripheral e.g. I2C1
 **@Return				: None
 *****************************************************************************/
void fnI2C_Stop(I2C_TypeDef* I2Cx)
{
	  /* wait for I2Cx EV8_2 --> byte has been transmitted */
		while(!(I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)));

	  /* Send I2Cx STOP Condition after last byte has been transmitted */
		I2C_GenerateSTOP(I2Cx, ENABLE);
}

/*****************************************************************************
 **@Function 			: fnI2C_Write
 **@Parameters		: * I2Cx --> the I2C peripheral e.g. I2C1
										* data --> the data byte to be transmitted
 **@Return				: None
 **@Descriptions	: This function transmits one byte to the slave device
 ****************************************************************************/
void fnI2C_Write(I2C_TypeDef* I2Cx, uint8_t ucdata)
{
	/* wait for I2C1 EV8 -->last byte is still being transmitted, next 
		 byte can already be written */
	while(!(I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTING)));
	I2C_SendData(I2Cx, ucdata);
}

/******************************************************************************
 **@Function 			: fnI2C_ReadAck					
 **@Descriptions	: This function reads one byte from the slave device and 
										acknowledges the byte (requests another byte)
 **@Parameters		: I2Cx --> the I2C peripheral e.g. I2C1
 **@Return				: data byte read
 *****************************************************************************/
uint8_t fnI2C_ReadAck(I2C_TypeDef* I2Cx)
{
		uint8_t ucdata;

	/* wait until one byte has been received */
	  while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED));
	
	/* read data from I2C data register and return data byte */
		ucdata = I2C_ReceiveData(I2Cx);
		
	 return ucdata;
}

/*******************************************************************************
 **@Function 			: fnI2C_ReadNack		
 **@Descriptions	: This function reads one byte from slave device and DOESNT
										acknowledges the byte
 **@Parameters		: I2Cx --> the I2C peripheral e.g. I2C1
 **@Return				: data byte read
 *****************************************************************************/
uint8_t fnI2C_ReadNack(I2C_TypeDef* I2Cx)
{
		uint8_t ucdata;
	
		/* disable acknowledge of received data */
		I2C_AcknowledgeConfig(I2Cx, DISABLE);
	
	  /* nack also generates stop condition after last byte received */
		I2C_GenerateSTOP(I2Cx, ENABLE); 
     
		/* wait until one byte has been received */
		while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED) );
	
		/* read data from I2C data register and return data byte */
	 	ucdata = I2C_ReceiveData(I2Cx);

	  return ucdata;
}	

/******************************************************************************
 **@Function 			: fnIOExpander_Write	
 **@Descriptions	: This funtion will write 16 bit data on both Port of Slave 
 **@Parameters		: ucSlaveAddr --> Slave Adress to write the data
										uiPortData: Two byte Data to write on both port of the slave
 **@Return				: None
 *****************************************************************************/
void fnIOExpander_Write(uint8_t ucSlaveAddr, uint16_t  uiPortData)
{
	
}

/******************************************************************************
 **@Function 			: fnGPIOExpander_Read	
 **@Descriptions	: this funtion will write the 16 bit data on both the Port of the Slave 
 **@Parameters		: ucSlaveAddr: Slave Adress to write the data
 **@Return				: uint16_t : retutn the read data
 ****************************************************************************/
uint16_t fnGPIOExpander_Read(uint8_t ucSlaveAddr)
{
	uint16_t uiReadData;
	uint8_t ucHighByte, ucLowByte;
  /* Slave Address with Read Instruction*/
	ucSlaveAddr = (ucSlaveAddr | I2C_SLAVE_READ) ; 
	I2C_AcknowledgeConfig(I2C1, ENABLE); 
	/* start a transmission in Master receiver mode */
	fnI2C_Start(I2C1, ucSlaveAddr, I2C_DIRECTION_RECEIVER); 
	ucLowByte = fnI2C_ReadAck(I2C1); /* Read the lower byte */
	ucHighByte = fnI2C_ReadNack(I2C1); /*	Read the Upper byte and stop I2Cx*/
	
	uiReadData	= (((uint16_t)ucHighByte << 8) | ((uint16_t)(ucLowByte)));	
	return uiReadData;
}


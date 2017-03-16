/******************************************************************************
**@file     :  I2C.c
* 
**@brief    :  This File contains All I2C funtions definations
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
/***************************************************************************
**@Function 			: fnI2C_Configuration
**@Descriptions	: This Funtion Initializes the I2Cs
**@parameters		:	none
**@return				: none
***************************************************************************/
void fnI2C_Configuration(void)
{
  GPIO_InitTypeDef stGPIOinit;
  I2C_InitTypeDef stI2Cinit;
  NVIC_InitTypeDef stNVIC_InitStructure;
  
  /* enable APB1 peripheral clock for I2C1 */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
  /* enable clock for SCL and SDA pins */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  /*Deinit and reset the I2C to avoid it locking up*/
  I2C_DeInit(I2C_IO_EXP);  
  I2C_SoftwareResetCmd(I2C_IO_EXP, ENABLE);
  I2C_SoftwareResetCmd(I2C_IO_EXP, DISABLE);
  
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_I2C1); 
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_I2C1);
  
  /* setup SDA and SCL pins PB9 and PB8 */
  stGPIOinit.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_8; 
  stGPIOinit.GPIO_Mode = GPIO_Mode_AF;	
  stGPIOinit.GPIO_OType = GPIO_OType_OD;	
  stGPIOinit.GPIO_Speed = GPIO_Speed_50MHz;
  stGPIOinit.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOB, &stGPIOinit);
  
  /* configure I2C1 */
  stI2Cinit.I2C_ClockSpeed = 100000; 					/*	100kHz */
  stI2Cinit.I2C_Mode = I2C_Mode_I2C;					/* I2C mode */
  stI2Cinit.I2C_DutyCycle = I2C_DutyCycle_2;	/* 50% duty cycle --> standard */
  /* own address, not relevant in master mode */
  stI2Cinit.I2C_OwnAddress1 = 0;
  /* disable acknowledge when reading */
  stI2Cinit.I2C_Ack = I2C_Ack_Enable;
  /* set address length to 7 bit addresses */
  stI2Cinit.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; 
  I2C_Init(I2C_IO_EXP, &stI2Cinit);			/* initialise I2C2 */	
  
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
  
  I2C_ITConfig(I2C_IO_EXP , I2C_IT_EVT | I2C_IT_BUF | I2C_IT_ERR, ENABLE);
  I2C_Cmd(I2C_IO_EXP, ENABLE);
  
  
  
  
}

/****************************************************************************
**@Function 			: fnI2C_Start
**@Descriptions	: This function issues a start condition and transmits 
the slave address + R/W bit
**@parameters		:	* I2Cx --> the I2C peripheral e.g. I2C1
* address --> the 7 bit slave address
* direction --> the transmission direction can be:
* I2C_Direction_Tranmitter for Master transmitter mode
* I2C_DIRECTION_RECEIVER for Master receiver
**@return				: None
****************************************************************************/
void fnI2C_Start(I2C_TypeDef* I2Cx, uint8_t address, uint8_t direction)
{
  /* wait until I2C1 is not busy any more */
  while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY));
  
  /* Send I2C1 START condition */
  I2C_GenerateSTART(I2Cx, ENABLE);
  
  /* wait for I2C1 EV5 --> Slave has acknowledged start condition */
  while(!(I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT)));
  
  /* Send slave Address for write */
  I2C_Send7bitAddress(I2Cx, address, direction);
  
  /* wait for I2Cx EV6, check if either Slave has acknowledged 
  Master transmitter or Master receiver */
  if(I2C_Direction_Transmitter == direction)
  {
    while(!(I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)));
  }
  else if(I2C_Direction_Receiver == direction)
  {
    while(!(I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)));
  }
}

/***************************************************************************
**@Function 			: fnI2C_Stop
**@Descriptions	: This function issues a stop condition and releases the bus
**@parameters		: I2Cx --> the I2C peripheral e.g. I2C1
**@return				: None
****************************************************************************/
void fnI2C_Stop(I2C_TypeDef* I2Cx)
{
  /*wait for I2Cx EV8_2 --> byte has been transmitted */
  while(!(I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED)));
  
  /* Send I2Cx STOP Condition after last byte has been transmitted */
  I2C_GenerateSTOP(I2Cx, ENABLE);
}

/*****************************************************************************
**@Function 			: fnI2C_Write
**@parameters		: * I2Cx --> the I2C peripheral e.g. I2C1
* data --> the data byte to be transmitted
**@return				: None
**@Descriptions	: This function transmits one byte to the slave device
*****************************************************************************/
void fnI2C_Write(I2C_TypeDef* I2Cx, uint8_t ucdata)
{
  /* wait for I2C1 EV8 -->last byte is still being transmitted, 
  next byte can already be written */
  while(!(I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTING)));
  I2C_SendData(I2Cx, ucdata);
}

/*****************************************************************************
**@Function 			: fnI2C_ReadAck			
**@Descriptions	: This function reads one byte from the slave device and 
acknowledges the byte (requests another byte)
**@parameters		: I2Cx --> the I2C peripheral e.g. I2C1
**@return				: data byte read
*******************************************************************************/
uint8_t fnI2C_ReadAck(I2C_TypeDef* I2Cx)
{
  uint8_t ucdata;
  /* disable acknowledge of received data */
  I2C_AcknowledgeConfig(I2Cx, ENABLE);
  
  /* wait until one byte has been received */
  while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED));
  
  /* read data from I2C data register and return data byte */
  ucdata = I2C_ReceiveData(I2Cx);
  
  return ucdata;
}

/*******************************************************************************
**@Function 			: fnI2C_ReadNack	
**@Descriptions	: This function reads one byte from the slave device 
and DOESNT acknowledges the byte
**@parameters		: I2Cx --> the I2C peripheral e.g. I2C1
**@return				: data byte read
********************************************************************************/
uint8_t fnI2C_ReadNack(I2C_TypeDef* I2Cx)
{
  uint8_t ucdata;
  
  /* disable acknowledge of received data */
  I2C_AcknowledgeConfig(I2Cx, DISABLE);
  
  /* wait until one byte has been received */
  while( !I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED) );
  
  /* read data from I2C data register and return data byte */
  ucdata = I2C_ReceiveData(I2Cx);
  
  /* nack also generates stop condition after last byte received */
  I2C_GenerateSTOP(I2Cx, ENABLE); 
  
  return ucdata;
}	

/*********************************************************************************
**@Function 			: fnIOExpander_Write	
**@Descriptions	: this will write the 16 bit data on both the Port of the Slave 
**@parameters		: ucSlaveAddr --> Slave Adress to write the data
ucHighPort: Higher byte Data to write on port1 of the slave
ucLowPort : Lower byte Data to write on port0 of the slave
**@return				: None
***********************************************************************************/
void fnIOExpander_Write(uint8_t ucSlaveAddr, uint8_t ucLowPort, uint8_t  ucHighPort)
{
  ucSlaveAddr = ucSlaveAddr & 0xFE;/*Slave Address with Write instruction*/
  /* start transmission in Master transmitter mode */
  fnI2C_Start(I2C1, ucSlaveAddr, I2C_Direction_Transmitter);
  fnI2C_Write(I2C1,ucLowPort); 	/* write Lower byte to the slave */
  fnI2C_Write(I2C1,ucHighPort); /* write the Higher byte to the slave */
  fnI2C_Stop(I2C1); 						/* stop the transmission */
}

/************************************************************************************** 
**@Function 			: fnGPIOExpander_Read	
**@Descriptions	: this funtion will write the 16 bit data on both the Port of the Slave  
**@parameters		: ucSlaveAddr: Slave Adress to write the data
**@return				: uint16_t retutn the read data
**************************************************************************************/
uint16_t fnGPIOExpander_Read(uint8_t ucSlaveAddr)
{
  uint16_t uiReadData;
  uint8_t ucHighByte, ucLowByte;
  
  ucSlaveAddr = (ucSlaveAddr | 0x01) ; 	/* Slave Address with Read Instruction*/
  I2C_AcknowledgeConfig(I2C2, ENABLE); 
  /* start a transmission in Master receiver mode */
  fnI2C_Start(I2C2, ucSlaveAddr, I2C_Direction_Receiver); 
  ucLowByte = fnI2C_ReadAck(I2C2); 			/* Read the lower byte */
  ucHighByte = fnI2C_ReadNack(I2C2); 		/*	Read the Upper byte and stop I2Cx*/
  
  uiReadData	= (((uint16_t)ucHighByte << 8) | ((uint16_t)(ucLowByte)));	
  return uiReadData;
}


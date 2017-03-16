/******************************************************************************
**@file    I2C_MACROS.h
* 
**@brief   This file contains macros define for i2c.
******************************************************************************
*/    
/* ----------------------------------------------------------------------------
*                           Includes
* ----------------------------------------------------------------------------
*/
/* ----------------------------------------------------------------------------
*                           MACROS
* ----------------------------------------------------------------------------
*/
/*Pins for I2C*/
	#define I2C_SCL																		GPIO_Pin_6					
	#define I2C_SDA																		GPIO_Pin_7					
	#define I2C_PORT																	GPIOB
	#define I2C_SLOCK_SPEED													  (uint32_t)100000

	#define I2C_SLAVE_WRITE														(uint8_t)0xFE		
	#define I2C_SLAVE_READ														(uint8_t)0x01

	#define IOEXP_1_SLAVE_ADDR												(uint8_t)0x42
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
extern void fnI2C_Configuration(void);
/* ----------------------------------------------------------------------------
*                           FUNCTIONS DECLARATION
* ----------------------------------------------------------------------------
*/




	





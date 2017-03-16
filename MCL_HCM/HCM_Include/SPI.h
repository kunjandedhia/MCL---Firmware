/******************************************************************************
**@file    SPI.h
* 
**@brief   This Header File contains declaration of all the external function,
					 Macros and External variables related SPI Peripheral.
******************************************************************************
*/    
#ifndef __SPI_h
#define __SPI_h

/* ----------------------------------------------------------------------------
*                           Includes
* ----------------------------------------------------------------------------
*/
/* ----------------------------------------------------------------------------
*                           MACROS
* ----------------------------------------------------------------------------
*/
/******** SPI2 for temperature Sensor TC(thermocouple) *********/
#define SPI_SCK 														GPIO_Pin_13
#define SPI_MISO 														GPIO_Pin_14
#define SPI_MOSI 														GPIO_Pin_15
#define SPI_PORT														GPIOB
#define TC_SPI															SPI2

#define SPI_CS_PORT												  GPIOD		

#define SPI_CS_1_PIN												GPIO_Pin_0
#define SPI_CS_2_PIN												GPIO_Pin_1
#define SPI_CS_3_PIN												GPIO_Pin_2
#define SPI_CS_4_PIN												GPIO_Pin_3
#define SPI_CS_5_PIN												GPIO_Pin_4
#define SPI_CS_6_PIN												GPIO_Pin_5

#define SPI_CS_PIN												  SPI_CS_1_PIN | SPI_CS_2_PIN | SPI_CS_3_PIN |\
																						SPI_CS_4_PIN | SPI_CS_5_PIN | SPI_CS_6_PIN	
																							
#define ADS1118_SELECT(x)    							 	((1 == x)? GPIO_WriteBit(SPI_CS_PORT, SPI_CS_1_PIN, Bit_RESET) : \
																						 (2 == x)? GPIO_WriteBit(SPI_CS_PORT, SPI_CS_2_PIN, Bit_RESET) : \
																						 (3 == x)? GPIO_WriteBit(SPI_CS_PORT, SPI_CS_3_PIN, Bit_RESET) : \
																						 (4 == x)? GPIO_WriteBit(SPI_CS_PORT, SPI_CS_4_PIN, Bit_RESET) : \
																						 (5 == x)? GPIO_WriteBit(SPI_CS_PORT, SPI_CS_5_PIN, Bit_RESET) : \
																											 GPIO_WriteBit(SPI_CS_PORT, SPI_CS_6_PIN, Bit_RESET))
																				
#define ADS1118_UNSELECT(x)  								((1 == x)? GPIO_WriteBit(SPI_CS_PORT, SPI_CS_1_PIN, Bit_SET) : \
																						 (2 == x)? GPIO_WriteBit(SPI_CS_PORT, SPI_CS_2_PIN, Bit_SET) : \
																						 (3 == x)? GPIO_WriteBit(SPI_CS_PORT, SPI_CS_3_PIN, Bit_SET) : \
																						 (4 == x)? GPIO_WriteBit(SPI_CS_PORT, SPI_CS_4_PIN, Bit_SET) : \
																						 (5 == x)? GPIO_WriteBit(SPI_CS_PORT, SPI_CS_5_PIN, Bit_SET) : \
																											 GPIO_WriteBit(SPI_CS_PORT, SPI_CS_6_PIN, Bit_SET))


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
extern void fnSPI_Init(void);
extern uint16_t fnSPI_ReadWrite(SPI_TypeDef*, uint16_t);
/* ----------------------------------------------------------------------------
*                           FUNCTIONS DECLARATION
* ----------------------------------------------------------------------------
*/
#endif


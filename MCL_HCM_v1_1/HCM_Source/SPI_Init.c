/******************************************************************************
* @file    SPI_Init.c
* 
* @brief   This file contains SPI function Definition
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
/******************************************************************************
 **@Function	 	 : 	fnSPI_Init()
 **@Description  : 	Initialization of SPI for Thermocouple 
 **@parameters	 :  None 
 **@return 		 	 :  None
 ****************************************************************************/
void fnSPI_Init(void)
{
	uint8_t ucLoop = 0;
	SPI_InitTypeDef stSPI_Config;
  GPIO_InitTypeDef   stGPIO_InitStructure;
 	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOD, ENABLE);
	
	stGPIO_InitStructure.GPIO_Pin = SPI_CS_PIN;
  stGPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	stGPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(SPI_CS_PORT, &stGPIO_InitStructure);
 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

  stGPIO_InitStructure.GPIO_Pin = SPI_SCK | SPI_MOSI;  /*** clk and MOSI ***/
  stGPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  stGPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(SPI_PORT, &stGPIO_InitStructure);
	
	stGPIO_InitStructure.GPIO_Pin = SPI_MISO; /** MISO ***/
  stGPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  stGPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(SPI_PORT, &stGPIO_InitStructure);  
  
  stSPI_Config.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  stSPI_Config.SPI_Mode = SPI_Mode_Master;
  stSPI_Config.SPI_DataSize = SPI_DataSize_16b;
  stSPI_Config.SPI_CPOL = SPI_CPOL_Low;
  stSPI_Config.SPI_CPHA = SPI_CPHA_2Edge;
  stSPI_Config.SPI_NSS = SPI_NSS_Hard;
  stSPI_Config.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32 ;
  stSPI_Config.SPI_FirstBit = SPI_FirstBit_MSB;
  stSPI_Config.SPI_CRCPolynomial = 7;

  SPI_Init(TC_SPI , &stSPI_Config);

  SPI_SSOutputCmd(TC_SPI, ENABLE);

  SPI_Cmd(TC_SPI , ENABLE);

  /** make chip select High for all 6 CS pins  for Thermocouple board ****/
	for(ucLoop = 0; ucLoop < NO_OF_SPARE_SSR + NO_OF_SSR; ucLoop++)
	{
		ADS1118_UNSELECT(ucLoop);
	}
}
/******************************************************************************
 **@Function 			: fnSPI_ReadWrite
 **@Descriptions	: write command to MCP2515.
 **@parameters		: SPIx : SPI Peripheral
										ucData: command or data to write
 **@return				:	data read from MCP2515
 ****************************************************************************/
uint16_t fnSPI_ReadWrite(SPI_TypeDef* SPIX, uint16_t uiData)   
{    
	  uint16_t uiRet = 0;
    SPI_I2S_SendData(SPIX, uiData);  

		while(SPI_I2S_GetFlagStatus(SPIX, SPI_I2S_FLAG_TXE) != SET);
    while(SPI_I2S_GetFlagStatus(SPIX, SPI_I2S_FLAG_RXNE) != SET);
	
    uiRet = SPI_I2S_ReceiveData(SPIX);
    return uiRet;
}  


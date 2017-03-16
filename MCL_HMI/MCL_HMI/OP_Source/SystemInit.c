/******************************************************************************
**@file    SystemInit.c
* 
**@brief   system initilization
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
**@Function		 	:  fnSystem_Init()
**@Descriptions	:	 Initialize all System peripherals
**@parameters		:  none
**@return				:  none
*****************************************************************************/
void fnSystem_Init(void)
{
	fnResetAllFlags();
	fnGR_GPIO_Configuration();
	#ifdef GENERAL_DBG
		fnUSART_Configuration();
	#endif
	DEBUG_PRINTF("Start OP  1111 \r\n");
	fnCAN_Configuration();
	fnCAN_MsgInit();
	fnI2C_Configuration();
	fnGR_Memory_Configuration();

	fnDisplay_Initialize();
	fnDisplay_Clear_Screen(RGB(255,255,255));
	fnGr_SelectFont("Franklin24", 1);
	fnDisplay_Reg_Write(0x007, 0x0000);   /*Display Control1*/
	fnDisplay_Clear_Screen(RGB(255,255,255));
	fnGr_SelectFont("Franklin24", 1);
	fnDisplay_Reg_Write(0x007, 0x0173);   /*Display Control1*/

	fnTIM1_Configuration();
	fnTIM2_Configuration();
	fnSw_Initialize();

	fnResetAllData();	
	fnResetAllConfig();
	//fnWDT_Initialize();					/*Intialize External WDT */
	
	fnDefaultConfig();
}
/*****************************************************************************
**@Function		 	:  fnWDT_Initialize()
**@Descriptions	:	 WDT check
**@parameters		:  none
**@return				:  none
*****************************************************************************/
void fnWDT_Initialize(void)
{
	GPIO_InitTypeDef stGPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOD , ENABLE);
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOD , DISABLE);

	stGPIO_InitStructure.GPIO_Pin = WDT_ENABLE_PIN;
	stGPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	stGPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(WDT_ENABLE_PORT, &stGPIO_InitStructure);
	
	stGPIO_InitStructure.GPIO_Pin = WDTI_PIN;
	stGPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	stGPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(WDTI_PORT, &stGPIO_InitStructure);
	
	GPIO_WriteBit(WDT_ENABLE_PORT,WDT_ENABLE_PIN,Bit_SET);
	GPIO_WriteBit(WDTI_PORT,WDTI_PIN,Bit_RESET);
}


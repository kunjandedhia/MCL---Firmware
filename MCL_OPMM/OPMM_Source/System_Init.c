/******************************************************************************
**@file    System_Init.c
* 
**@brief   Handles System Initalization.
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
GPIO_InitTypeDef stGPIO_InitStructure;
NVIC_InitTypeDef stNVIC_InitStructure;
EXTI_InitTypeDef stEXTI_InitStructure;
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
void fnGPIO_Configuration(void);
/******************************************************************************
 **@Function 			: fnSystem_Init
 **@Descriptions	: Initialize all System peripherals
 **@Parameters		: None
 **@Return				: None
 *****************************************************************************/
void fnSystem_Init(void)
{
	SystemInit();
	
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
  fnGPIO_Configuration();
	
  fnTIM1_Config(); /*Timer 1 PWM config for Buzzer*/
  fnTIM2_Config(); /*Timer 2 config for 2 mili second for LM Switch*/
	fnTIM3_Config(); /*Timer 3 config for 10 mili second for General Delay*/

#ifdef UART_DBG
  fnUSART_Config();
#endif
  fnADC_Config();
	
	/*CAN Initialization */
	fnCAN_Config();
	
	fnCANMsg_Init();
	/****** MCP CAN2 initialization***********/
  fnMCP_SPI_Config();
	
	fnSPI_Init();
	
	/*****ESTOP monitoring EXTI Configure*********/
	
	fnMCPEXTICAN_Config();

	fnI2C_Config();
	
	/*Set all Tricolor LEDs to the default state*/
  fnDefault_Config();	
	
	/*******CAN2 Speed Config and MCP Set to Normal Mode********/
	fnMCP2515_Init();

	fnMCP2515CANCTRLMode_Set(MCP2515_MODE_NORMAL);
	
	
}
/******************************************************************************
 **@Function 	  	: fnGPIO_Config
 **@Descriptions	: Initialize  GPIO's 
 **@Parameters		: None
 **@Return				: None
 *****************************************************************************/
void fnGPIO_Configuration(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |
				RCC_APB2Periph_GPIOB |  
				 RCC_APB2Periph_GPIOC	| 
					 RCC_APB2Periph_GPIOD | 
						 RCC_APB2Periph_GPIOE	| 
							 RCC_APB2Periph_GPIOF |
									RCC_APB2Periph_GPIOG,
								 ENABLE);
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOA | 
				RCC_APB2Periph_GPIOB | 
				 RCC_APB2Periph_GPIOC	| 
					 RCC_APB2Periph_GPIOD | 
						 RCC_APB2Periph_GPIOE	| 
							 RCC_APB2Periph_GPIOF |
									RCC_APB2Periph_GPIOG,
								 DISABLE);

	/* GPIO for Valve_In pin configuration */
	stGPIO_InitStructure.GPIO_Pin = VALVE1_DRIVE | VALVE2_DRIVE	| 
																	VALVE3_DRIVE | VALVE4_DRIVE | 
																	VALVE5_DRIVE | VALVE6_DRIVE | 
																	VALVE7_DRIVE | VALVE8_DRIVE;
	stGPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	stGPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(VALVE_DRIVE_PORT, &stGPIO_InitStructure);
	
//	/*Test Point 14 as Output */
	stGPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_Init(GPIOB, &stGPIO_InitStructure);
	
	
	
	/* GPIO for Valve_Status pin configuration for valve 1 to 4 */
	stGPIO_InitStructure.GPIO_Pin = VALVE1_FEEDBACK | VALVE2_FEEDBACK |
																	VALVE3_FEEDBACK | VALVE4_FEEDBACK 
																	;
	stGPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	stGPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(VALVE_FEEDBACK_PORT(1), &stGPIO_InitStructure);

	/*GPIO for Valve_Status pin configuration for valve 5 to 8*/
	stGPIO_InitStructure.GPIO_Pin = VALVE5_FEEDBACK | VALVE6_FEEDBACK |
																	VALVE7_FEEDBACK | VALVE8_FEEDBACK  ;
	stGPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	stGPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(VALVE_FEEDBACK_PORT(7), &stGPIO_InitStructure);
	
	
	stGPIO_InitStructure.GPIO_Pin = MCP_CAN2_ERROR;
	GPIO_Init(MCP_CAN2_GPIO_PORT, &stGPIO_InitStructure);
	
	stGPIO_InitStructure.GPIO_Pin = CAN1_ERROR;
	GPIO_Init(CAN1_ERROR_PORT, &stGPIO_InitStructure);

	/*GPIO for Limit Switch*/
	stGPIO_InitStructure.GPIO_Pin = HOME_POSITION_LIMIT | 
																  DCLS_POSITION_LIMIT | 
																	GUN_TOWARDS_LIMIT |
																	GUN_AWAY_LIMIT ;
	stGPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	stGPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(LIMIT_SW_PORT, &stGPIO_InitStructure);

	/***************** GPIOS for Limit switch LEDs ***************/
	/****GPIO for Limit Switch LEDs*****/
	stGPIO_InitStructure.GPIO_Pin = HOME_POSITION_LSW_LED	|
																	DCLS_POSITION_LSW_LED	| 
																	GUN_TOWARDS_LSW_LED | 
																	GUN_AWAY_LSW_LED ;
	stGPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	stGPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(LIMIT_SW_STATUS_LED_PORT, &stGPIO_InitStructure);

	/******************GPIO for the Laser Beam**********/
	stGPIO_InitStructure.GPIO_Pin = LASER_BEAM_GPIO_PIN;
	GPIO_Init(LASER_BEAM_GPIO_PORT, &stGPIO_InitStructure);
	
	/***************CAN GPIO INIT************************/
	stGPIO_InitStructure.GPIO_Pin = CAN1_ENABLE;
	GPIO_Init(CAN1_ENABLE_PORT, &stGPIO_InitStructure);
	
	stGPIO_InitStructure.GPIO_Pin = CAN1_STROBE;
	GPIO_Init(CAN1_STROBE_PORT, &stGPIO_InitStructure);
		
	stGPIO_InitStructure.GPIO_Pin = MCP_CAN2_ENABLE;
	GPIO_Init(MCP_CAN2_GPIO_PORT, &stGPIO_InitStructure);
	
	stGPIO_InitStructure.GPIO_Pin = MCP_CAN2_STROBE;
	GPIO_Init(MCP_CAN2_GPIO_PORT, &stGPIO_InitStructure);
	
	stGPIO_InitStructure.GPIO_Pin = VBAT_ENABLE_PIN;
	GPIO_Init(VBAT_ENABLE_PORT, &stGPIO_InitStructure);

	/*Hard reset pin for External CAN MCP2515*/
	stGPIO_InitStructure.GPIO_Pin = MCP_RESET_PIN;
	GPIO_Init(MCP_CAN2_RESET_PORT, &stGPIO_InitStructure);

	/*Configure pin as floating input for GUN_AWAY_LIMIT switch*/
	stGPIO_InitStructure.GPIO_Pin = GUN_AWAY_LIMIT;
	stGPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	stGPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LIMIT_SW_PORT, &stGPIO_InitStructure);
	
	/*Configure pin as floating input for GUN_TOWARDS_LIMIT switch*/
	stGPIO_InitStructure.GPIO_Pin = GUN_TOWARDS_LIMIT;
	stGPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	stGPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LIMIT_SW_PORT, &stGPIO_InitStructure);
	
	/*Configure pin as floating input for DCLS_POSITION_LIMIT switch*/
	stGPIO_InitStructure.GPIO_Pin = DCLS_POSITION_LIMIT;
	stGPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	stGPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LIMIT_SW_PORT, &stGPIO_InitStructure);
	
	/*Configure pin as floating input for HOME_POSITION_LIMIT switch*/
	stGPIO_InitStructure.GPIO_Pin = HOME_POSITION_LIMIT;
	stGPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	stGPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LIMIT_SW_PORT, &stGPIO_InitStructure);
	
	/********************** Estop Monitoring**********************/
	stGPIO_InitStructure.GPIO_Pin = ESTOP_RELAY_MONIT_PIN | SAFTY_RELAY_MONIT1_PIN | SAFTY_RELAY_MONIT2_PIN;
	stGPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	stGPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ESTOP_MONIT_PORT, &stGPIO_InitStructure);
}

/******************************************************************************
 **@Function 			: fnMCPEXTICAN_Config
 **@Descriptions  : Configures the External Interrupt for MCP2515 SPI 
										interrupt for CAN2 Link.
 **@Parameters 		: None
 **@Return			  : None
 ****************************************************************************/
void fnMCPEXTICAN_Config(void)
{
	RCC_APB2PeriphClockCmd(MCAP_CAN_RCC_APBPERIPH_GPIO, ENABLE);
	/**********/
////	RCC_APB2PeriphClockCmd(ESTOP_RCC_APBPERIPH_GPIO, ENABLE);
////	
////	/* Configure pin as floating input , max speed 10 MHz */
////	stGPIO_InitStructure.GPIO_Pin 	= ESTOP_MONITOR_PIN;
////	stGPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_IN_FLOATING;
////	stGPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
////	GPIO_Init(ESTOP_MONITOR_PORT, &stGPIO_InitStructure);
	/**********/
	
	/* Configure pin as floating input , max speed 10 MHz */
	stGPIO_InitStructure.GPIO_Pin = MCP_CAN2_EXTI_GPIO_PIN;
	stGPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	stGPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(MCP_CAN2_EXTI_PORT, &stGPIO_InitStructure);
	
	GPIO_EXTILineConfig(MCP_CAN2_GPIO_PORTSOURCEGPIO , 
											MCP_CAN2_GPIO_PINSOURCE);   
	
	EXTI_ClearITPendingBit(MCP_CAN2_EXTI_LINE);
		
	stEXTI_InitStructure.EXTI_Line = MCP_CAN2_EXTI_LINE;
	stEXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	stEXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	stEXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&stEXTI_InitStructure);  
	
	stNVIC_InitStructure.NVIC_IRQChannel = MCP_CAN2_EXTI_IRQN;
	stNVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	stNVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	stNVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&stNVIC_InitStructure); 
	#ifdef MCP_CAN2_DBG    	
		DEBUG_PRINTF("MCP Ext INTR Init\n\r");
	#endif
}

/******************************************************************************
 **@Function		 	:  fnWDT_Initialize
 **@Descriptions	:	 WDT check
 **@Parameters		:  None
 **@Return				:  None
 ****************************************************************************/
void fnWDT_Initialize(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOD ,
												 ENABLE);
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOD ,
												 DISABLE);

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
	
	
////	GPIO_WriteBit(WDTI_PORT,WDTI_PIN,Bit_SET);

////	GPIO_WriteBit(WDTI_PORT,WDTI_PIN,Bit_RESET);
}

/******************************************************************************
 **@Function		 	:  fnWDT_Check
 **@Descriptions	:	 WDT check
 **@Parameters		:  None
 **@Return				:  None
 ****************************************************************************/
void fnWDT_Check(void)
{
	if(uiWDTCount > WDT_TIME_CHECK)
	{
		#ifdef GENERAL_DBG
			DEBUG_PRINTF("WDT reset Given\r\n");
		#endif
		uiWDTCount = 0;
		GPIO_WriteBit(WDTI_PORT,WDTI_PIN,Bit_SET);
		GPIO_WriteBit(WDTI_PORT,WDTI_PIN,Bit_RESET);
	}
}
///******************************************************************************
// **@Function 			: fnESTOP_EXTI_Config
// **@Descriptions  : Configures the External Interrupt for ESTOP Monitoring.
// **@Parameters 		: None
// **@Return			  : None
// ****************************************************************************/
//void fnESTOP_EXTI_Config(void)
//{
////	RCC_APB2PeriphClockCmd(ESTOP_RCC_APBPERIPH_GPIO, ENABLE);

////	/* Configure pin as floating input , max speed 10 MHz */
////	stGPIO_InitStructure.GPIO_Pin 	= ESTOP_MONITOR_PIN;
////	stGPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_IN_FLOATING;
////	stGPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
////	GPIO_Init(ESTOP_MONITOR_PORT, &stGPIO_InitStructure);
////	
////	GPIO_EXTILineConfig(ESTOP_GPIO_PORTSOURCEGPIO, 
////											ESTOP_GPIO_PINSOURCE);   
////	
////	EXTI_ClearITPendingBit(ESTOP_EXTI_LINE);
////	
////	stEXTI_InitStructure.EXTI_Line 		= ESTOP_EXTI_LINE;
////	stEXTI_InitStructure.EXTI_Mode 		= EXTI_Mode_Interrupt;
////	stEXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
////	stEXTI_InitStructure.EXTI_LineCmd = ENABLE;
////	EXTI_Init(&stEXTI_InitStructure);
////	
////	stNVIC_InitStructure.NVIC_IRQChannel 										= ESTOP_EXTI_IRQN;
////	stNVIC_InitStructure.NVIC_IRQChannelPreemptionPriority 	= 2;
////	stNVIC_InitStructure.NVIC_IRQChannelSubPriority 				= 0;
////	stNVIC_InitStructure.NVIC_IRQChannelCmd 								= ENABLE;
////	NVIC_Init(&stNVIC_InitStructure); 
////	#ifdef ESTOP_MON_DBG    	
////		DEBUG_PRINTF("ESTOP Ext INTR Init\n\r");
////	#endif
//}

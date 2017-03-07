/******************************************************************************
  * @file    System_Init.c
  * 
  * @brief   Handles System Initalization.
  ****************************************************************************
  */
/* ----------------------------------------------------------------------------
 *                           Includes
 * ---------------------------------------------------------------------------
 */
 #include "Includes.h"
/* ----------------------------------------------------------------------------
 *                           MACROS
 * ---------------------------------------------------------------------------
 */
/* ----------------------------------------------------------------------------
 *                           CONSTANTS
 * ---------------------------------------------------------------------------
 */

/* ----------------------------------------------------------------------------
 *                           GLOBAL VARIABLES
 * ---------------------------------------------------------------------------
 */
	EXTI_InitTypeDef stEXTI_InitStructure;
	GPIO_InitTypeDef stGPIO_InitStructure;
	NVIC_InitTypeDef stNVIC_InitStructure;
/* ----------------------------------------------------------------------------
 *                           EXTERNAL VARIABLES
 * ---------------------------------------------------------------------------
 */
 /* ---------------------------------------------------------------------------
 *                           EXTERNAL FUNCTIONS
 * ---------------------------------------------------------------------------
 */
/* ----------------------------------------------------------------------------
 *                           FUNCTIONS DECLARITION
 * ---------------------------------------------------------------------------
 */
void fnGPIO_Config(void);
void fnNVIC_SetVector(void);
/******************************************************************************
 **@Function 			: fnSysInit
 **@Descriptions	: Initialize all Sytem peripherals
 **@Parameters		: None
 **@Return				: None
 ****************************************************************************/
void fnSysInit(void)
{
	fnNVIC_SetVector();	
	fnGPIO_Config();
	fnTIM1_Config();		/*Initialize timer 1 with period 10ms*/

	#ifdef USART_DBG
		fnUSARTDBG_Config();
	#endif
	/*Init of UART5 for UPS Communication*/
	fnUPS_Config();
	
	/*Initialize ADC for Linear Transducer and pressure Gauge*/
	fnADC_Init();
	
	/*Initialize External Intterupt	for Linear transducer*/
	fnExtIntr_Config();
	
	/*Initialize CAN*/
	CAN_Config();
	CANMsg_Init();
	
	/*Initialize I2C*/
	fnI2C_Config();

	/*Default Settings*/
	fnDefault_Config();	
}

/******************************************************************************
 **@Function 			: fnGPIO_Config
 **@Descriptions	: Initialize GPIO's for pins
 **@Parameters		: None
 **@Return				: None
 ****************************************************************************/
void fnGPIO_Config(void)
{
	/*Enables or disables the High Speed APB (APB2) peripheral 
	clock for GPIO ports*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |                         
												 RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD |
												 RCC_APB2Periph_GPIOE	| RCC_APB2Periph_GPIOF |
												 RCC_APB2Periph_GPIOG,
                         ENABLE);
	/*Forces or releases High Speed APB (APB2) peripheral reset for GPIO ports*/
  RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |                         
												 RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD |
												 RCC_APB2Periph_GPIOE	| RCC_APB2Periph_GPIOF |
												 RCC_APB2Periph_GPIOG,
                         DISABLE);
  /****GPIO for Valve_Drive  pin configuration*****/
	stGPIO_InitStructure.GPIO_Pin 		= VALVES_DRIVE_PIN;
  stGPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
  stGPIO_InitStructure.GPIO_Mode 		= GPIO_Mode_Out_PP;
	GPIO_Init(VALVE_DRIVE_PORT, &stGPIO_InitStructure);
	/****GPIO for Valve_Status pin configuration*****/
	stGPIO_InitStructure.GPIO_Pin 		= VALVES_FEEDBACK_PIN;
	stGPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
	stGPIO_InitStructure.GPIO_Mode 		= GPIO_Mode_IN_FLOATING;
	GPIO_Init(VALVE_FEEDBACK_PORT, &stGPIO_InitStructure);
	
	/****GPIO for Valve_Connected_Status pin configuration*****/
	stGPIO_InitStructure.GPIO_Pin 		=  VALVES_CONN_STAT_PIN;
	stGPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
	stGPIO_InitStructure.GPIO_Mode 		= GPIO_Mode_IN_FLOATING;
	GPIO_Init(VALVE_CONN_STAT_PORT, &stGPIO_InitStructure);

	/****GPIO for CAN Enable pins configuration*****/
	stGPIO_InitStructure.GPIO_Pin 		= CAN_ENABLE_PIN;
  stGPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
  stGPIO_InitStructure.GPIO_Mode 		= GPIO_Mode_Out_PP;
	GPIO_Init(CAN_ENABLE_PORT, &stGPIO_InitStructure);
	
	stGPIO_InitStructure.GPIO_Pin 		= CAN_STROBE_PIN;
  stGPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
  stGPIO_InitStructure.GPIO_Mode 		= GPIO_Mode_Out_PP;
	GPIO_Init(CAN_STROBE_PORT, &stGPIO_InitStructure);

	stGPIO_InitStructure.GPIO_Pin 		= CAN_ERROR_PIN;
	stGPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
	stGPIO_InitStructure.GPIO_Mode 		= GPIO_Mode_IN_FLOATING;
	GPIO_Init(CAN_ERROR_PORT, &stGPIO_InitStructure);
	
	/******** AIR PRESSURE SENSOR INTERFACE ********/
	stGPIO_InitStructure.GPIO_Pin 		= APS_GPIO_PIN1 | APS_GPIO_PIN2;
	stGPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
	stGPIO_InitStructure.GPIO_Mode 		= GPIO_Mode_IN_FLOATING;
	GPIO_Init(APS_GPIO_PORT, &stGPIO_InitStructure);

	stGPIO_InitStructure.GPIO_Pin 		= UPS_INPUT_PIN;
	stGPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
	stGPIO_InitStructure.GPIO_Mode 		= GPIO_Mode_IN_FLOATING;
	GPIO_Init(UPS_GPIO_PORT, &stGPIO_InitStructure);
	
	stGPIO_InitStructure.GPIO_Pin 		= UPS_OUTPUT_PIN;
  stGPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;
  stGPIO_InitStructure.GPIO_Mode 		= GPIO_Mode_Out_PP;
	GPIO_Init(UPS_GPIO_PORT, &stGPIO_InitStructure);
}

/******************************************************************************
 **@Function 			: fnNVIC_SetVector
 **@Descriptions	: Nested Vectored interrupt control settings
 **@Parameters		: None
 **@Return				: None
 ****************************************************************************/
void fnNVIC_SetVector(void)
{
  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);   
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); 
}

/******************************************************************************
 **@Function 			: fnExtIntr_Config
 **@Descriptions	: Configures External Interrupt to measure pump count.
 **@Parameters		: None
 **@Return				: None
 ****************************************************************************/
void fnExtIntr_Config(void)
{  
	RCC_APB2PeriphClockCmd(LT_RCC_APBPERIPH_GPIO, ENABLE);

	/*Configure pin as floating input Intr, max speed 10 MHz*/
	stGPIO_InitStructure.GPIO_Pin 		= LT1_INTR_POSITION_1_PIN | LT2_INTR_POSITION_1_PIN;
	stGPIO_InitStructure.GPIO_Mode 		= GPIO_Mode_IN_FLOATING;
	stGPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_10MHz;
	GPIO_Init(LT_POSITION_PORT, &stGPIO_InitStructure);
	
	GPIO_EXTILineConfig(LT_GPIO_PORTSOURCE, LT1_GPIO_PINSOURCE); 
	GPIO_EXTILineConfig(LT_GPIO_PORTSOURCE, LT2_GPIO_PINSOURCE);   
	
	EXTI_ClearITPendingBit(LT1_EXTI_LINE_NO);
	
	stEXTI_InitStructure.EXTI_Line 		= LT1_EXTI_LINE_NO; 
	stEXTI_InitStructure.EXTI_Mode 		= EXTI_Mode_Interrupt;
	stEXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	stEXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&stEXTI_InitStructure);
   
	stNVIC_InitStructure.NVIC_IRQChannel 										= LT1_EXTI_IRQN;
	stNVIC_InitStructure.NVIC_IRQChannelPreemptionPriority 	= 6;
	stNVIC_InitStructure.NVIC_IRQChannelSubPriority 				= 0;
	stNVIC_InitStructure.NVIC_IRQChannelCmd 								= ENABLE;
	NVIC_Init(&stNVIC_InitStructure);
	
	/*LT2*/
	EXTI_ClearITPendingBit(LT2_EXTI_LINE_NO);
	stEXTI_InitStructure.EXTI_Line 		= LT2_EXTI_LINE_NO;
	stEXTI_InitStructure.EXTI_Mode 		= EXTI_Mode_Interrupt;
	stEXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	stEXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&stEXTI_InitStructure);
   
	stNVIC_InitStructure.NVIC_IRQChannel 										= LT2_EXTI_IRQN;
	stNVIC_InitStructure.NVIC_IRQChannelPreemptionPriority 	= 6;
	stNVIC_InitStructure.NVIC_IRQChannelSubPriority 				= 1;
	stNVIC_InitStructure.NVIC_IRQChannelCmd 								= ENABLE;
	NVIC_Init(&stNVIC_InitStructure);
}

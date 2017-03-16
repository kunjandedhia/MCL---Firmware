/******************************************************************************
  * @file    Timer.c
  * 
  * @brief   This file contains the Timer configurations
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
void fnTIM1_Config(void);

/******************************************************************************  
 **@Function 		: fnTIM1_Config  
 **@Description	: Timer Initialisation  
 **@Parameters	: None  
 **@Return			: None   
 *****************************************************************************/ 
void fnTIM1_Config(void)
{
	NVIC_InitTypeDef stNVIC_InitStructure;
	TIM_TimeBaseInitTypeDef stTIM1_TimeBaseInitStruct;

	/* Timer1 Init*/
  /* Enable Timer1 clock and release reset*/
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
  RCC_APB2PeriphResetCmd(RCC_APB2Periph_TIM1,DISABLE);

  /* Set timer period 10ms*/
  stTIM1_TimeBaseInitStruct.TIM_Prescaler 					= TIM_PRESCALER; 
  stTIM1_TimeBaseInitStruct.TIM_CounterMode 				= TIM_CounterMode_Up;
  stTIM1_TimeBaseInitStruct.TIM_Period 							= TIM_PERIOD;  
  stTIM1_TimeBaseInitStruct.TIM_ClockDivision 			= TIM_CKD_DIV1;
  stTIM1_TimeBaseInitStruct.TIM_RepetitionCounter 	= 0;
  TIM_TimeBaseInit(TIM1,&stTIM1_TimeBaseInitStruct);

  /* Clear update interrupt bit*/
  TIM_ClearITPendingBit(TIM1,TIM_FLAG_Update);
  /* Enable update interrupt*/
  TIM_ITConfig(TIM1,TIM_FLAG_Update,ENABLE);
	
	stNVIC_InitStructure.NVIC_IRQChannel 										= TIM1_UP_IRQn;
  stNVIC_InitStructure.NVIC_IRQChannelPreemptionPriority 	= 7;
  stNVIC_InitStructure.NVIC_IRQChannelSubPriority 				= 0;
  stNVIC_InitStructure.NVIC_IRQChannelCmd 								= ENABLE;
  NVIC_Init(&stNVIC_InitStructure);
}

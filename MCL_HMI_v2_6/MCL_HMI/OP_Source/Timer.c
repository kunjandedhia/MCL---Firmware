/******************************************************************************
**@file    Timer.c
* 
**@brief   
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
**@Function		 	:  fnTIM1_Configuration
**@Descriptions	:	 Timer 1 initialization
**@parameters		:  none
**@return				:  none
*****************************************************************************/
void fnTIM1_Configuration(void)
{
	NVIC_InitTypeDef stNVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  stTIM_TimeBaseStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

	stNVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;
	stNVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	stNVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	stNVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&stNVIC_InitStructure);

	// Interrupt at a 1-millisecond rate
	stTIM_TimeBaseStructure.TIM_Prescaler = 720 - 1;
	stTIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	stTIM_TimeBaseStructure.TIM_Period = 100 - 1;
	stTIM_TimeBaseStructure.TIM_ClockDivision = 0x0;
	stTIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0;
	TIM_TimeBaseInit(TIM1, &stTIM_TimeBaseStructure);

	TIM_ClearITPendingBit(TIM1,TIM_IT_Update);

	// Enable the TIM1 Update Interrupt
	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
	// TIM1 counter enable
	TIM_Cmd(TIM1, ENABLE);
}

/*****************************************************************************
**@Function		 	:  fnTIM2_Configuration
**@Descriptions	:	 Timer 1 initialization
**@parameters		:  none
**@return				:  none
*****************************************************************************/
void fnTIM2_Configuration(void)
{
	NVIC_InitTypeDef stNVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  stTIM_TimeBaseStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	stNVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	stNVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	stNVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	stNVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&stNVIC_InitStructure);

	// Interrupt at a 5-millisecond rate
	stTIM_TimeBaseStructure.TIM_Prescaler = 1800;
	stTIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	stTIM_TimeBaseStructure.TIM_Period = 100;
	stTIM_TimeBaseStructure.TIM_ClockDivision = 0x0;
	stTIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0;
	TIM_TimeBaseInit(TIM2, &stTIM_TimeBaseStructure);

	TIM_ClearITPendingBit(TIM2,TIM_IT_Update);

	/* Enable the TIM2 Update Interrupt */
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	/* TIM2 counter enable */
	TIM_Cmd(TIM2, ENABLE);
}


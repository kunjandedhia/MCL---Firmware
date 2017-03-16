/******************************************************************************
* @file    Init_timer.c
* 
* @brief   This file contain Timer funtions 
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
 **@function		:	fnTIM1_Init
 **@Description :	Initilizes the timer1 and set timer period of 10msec
 **@parameter 	: None
 **@retval      : None 
 ***************************************************************************/
void fnTIM1_Init(void)
{
	TIM_TimeBaseInitTypeDef stTIM1_TimeBaseInitStruct; 
	NVIC_InitTypeDef stNVIC_InitStructure;
	
  /* Enable Timer1 clock and release reset ***/
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
  RCC_APB2PeriphResetCmd(RCC_APB2Periph_TIM1, DISABLE);
	
	/* timer Configuration for 10msec */
  stTIM1_TimeBaseInitStruct.TIM_Prescaler = TIM1_PRESCALER;  
  stTIM1_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
  stTIM1_TimeBaseInitStruct.TIM_Period = TIM1_PERIOD;  /* 10msec*/
  stTIM1_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
  stTIM1_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
  
	TIM_TimeBaseInit(TIM1, &stTIM1_TimeBaseInitStruct);

	stNVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;
  stNVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
  stNVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  stNVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&stNVIC_InitStructure);
	
	/* Clear update interrupt bit */
   TIM_ClearITPendingBit(TIM1,TIM_FLAG_Update);
  /* Enable update interrupt */
  TIM_ITConfig(TIM1, TIM_FLAG_Update, ENABLE);
}


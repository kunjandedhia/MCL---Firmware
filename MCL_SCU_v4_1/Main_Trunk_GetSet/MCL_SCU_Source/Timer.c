/******************************************************************************
**@file     :    Timer.c
* 
**@brief    :   This file contains the Timer configuration function
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
/* ----------------------------------------------------------------------------
*                           FUNCTIONS DECLARITION
* ----------------------------------------------------------------------------
*/
void fnTIM_Configuration(void);
/*************************************************************************  
**@Function  		: TIM1_Configuration 
**@Description		: Timer Initialisation   
**@Parameters		: None  
**@Return				: None  
*************************************************************************/ 
void fnTIM_Configuration(void)
{
  NVIC_InitTypeDef stNVIC_InitStructure;
  TIM_TimeBaseInitTypeDef stTIM1_TimeBaseInitStruct;
  
  /* Timer1 Init*/
  /* Enable Timer1 clock and release reset*/
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
  RCC_APB2PeriphResetCmd(RCC_APB2Periph_TIM1,DISABLE);
  
  /* Set timer period 10ms*/
  stTIM1_TimeBaseInitStruct.TIM_Prescaler = TIM1_PRESCALER; 
  stTIM1_TimeBaseInitStruct.TIM_Period = TIM1_PERIOD; 
  stTIM1_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
  stTIM1_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
  stTIM1_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM1,&stTIM1_TimeBaseInitStruct);
  
  /* Clear update interrupt bit*/
  TIM_ClearITPendingBit(TIM1,TIM_FLAG_Update);
  /* Enable update interrupt*/
  TIM_ITConfig(TIM1,TIM_FLAG_Update,ENABLE);
  
  stNVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_TIM10_IRQn;
  stNVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  stNVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  stNVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&stNVIC_InitStructure);
  
  /******************************Timer 2 Initialization***********************/
  /* Timer2 Init*/
  /* Enable Timer2 clock and release reset*/
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
  RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM2,DISABLE);
  
  /* Set timer period 1ms*/
  stTIM1_TimeBaseInitStruct.TIM_Prescaler = TIM2_PRESCALER; /*840; */
  stTIM1_TimeBaseInitStruct.TIM_Period = TIM2_PERIOD;   /*100;  */
  stTIM1_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
  stTIM1_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
  stTIM1_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM2, &stTIM1_TimeBaseInitStruct);
  
  /* Clear update interrupt bit*/
  TIM_ClearITPendingBit(TIM2,TIM_FLAG_Update);
  /* Enable update interrupt*/
  TIM_ITConfig(TIM2, TIM_FLAG_Update,ENABLE);
  
  stNVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  stNVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  stNVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  stNVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&stNVIC_InitStructure);
  
}


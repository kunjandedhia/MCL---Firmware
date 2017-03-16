/******************************************************************************
* @file    Timer.c
* 
* @brief   This file contains the Timer configuration function
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
*                           FUNCTIONS DECLARATION
* ----------------------------------------------------------------------------
*/
/*****************************************************************************  
 **@Function 			: TIM1_Configuration 
 **@Description		: Timer Initialisation and PWM for Buzzer 
 **@Parameters		: None  
 **@Return				: None  
 **************************************************************************/ 
void fnTIM1_Config(void)
{
	TIM_TimeBaseInitTypeDef stTIM1_TimeBaseInitStruct;//timer struct varible 
  TIM_OCInitTypeDef stTIM_OCInitStructure;
	GPIO_InitTypeDef stGPIO_InitStructure;
 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

	/* initialize PE13 for PWM output (Buzzer Output)*/
	stGPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	stGPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	stGPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOE, &stGPIO_InitStructure);
	
	/*Remap Tim1, Channel 3 to PE13*/
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM1, ENABLE);

    
	// Set timer period 10 msec
  stTIM1_TimeBaseInitStruct.TIM_Prescaler = 7200;  
  stTIM1_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
  stTIM1_TimeBaseInitStruct.TIM_Period = 100;  
  stTIM1_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
  stTIM1_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1, &stTIM1_TimeBaseInitStruct);
	

 /* PWM1 Mode configuration: Channel3 */
	stTIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	stTIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	stTIM_OCInitStructure.TIM_Pulse = 0;
	stTIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	
	TIM_OC3Init(TIM1, &stTIM_OCInitStructure);
	
	TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);
	
	TIM_ARRPreloadConfig(TIM1, ENABLE);
	
	TIM_CtrlPWMOutputs(TIM1,ENABLE);
	
	TIM_Cmd(TIM1, ENABLE);
	
	/*setting 0% Duty Cycle*/
	TIM_SetCompare3(TIM1,0);
}

/*****************************************************************************  
 **@Function 			: TIM2_Configuration 
 **@Description		: Timer 2 Initialisation  for 2 msec delay to scan limit switches
 **@Parameters		: None  
 **@Return				: None  
 **************************************************************************/ 
void fnTIM2_Config(void)
{
	NVIC_InitTypeDef stNVIC_InitStructure;
  TIM_TimeBaseInitTypeDef stTIM2_TimeBaseInitStruct;
  
  /* Timer1 Init*/
  /* Enable Timer1 clock and release reset*/
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
  RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM2,DISABLE);
  
  /* Set timer period 4 ms*/
  stTIM2_TimeBaseInitStruct.TIM_Prescaler = TIM2_PRESCALER; 
  stTIM2_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
  stTIM2_TimeBaseInitStruct.TIM_Period = TIM2_PERIOD;
  stTIM2_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
  stTIM2_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM2,&stTIM2_TimeBaseInitStruct);
  
  /* Clear update interrupt bit*/
  TIM_ClearITPendingBit(TIM2,TIM_FLAG_Update);
  /* Enable update interrupt*/
  TIM_ITConfig(TIM2,TIM_FLAG_Update,ENABLE);
  
  stNVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  stNVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
  stNVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  stNVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&stNVIC_InitStructure);
}

/*****************************************************************************  
 **@Function 			: fnTIM3_Config 
 **@Description		: Timer 3 Initialisation for General Delay of 10 milisec
 **@Parameters		: None  
 **@Return				: None  
 ***************************************************************************/ 
void fnTIM3_Config(void)
{
	NVIC_InitTypeDef stNVIC_InitStructure;
  TIM_TimeBaseInitTypeDef stTIM3_TimeBaseInitStruct;
  
  /* Timer1 Init*/
  /* Enable Timer1 clock and release reset*/
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
  RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM3,DISABLE);
  
  /* Set timer period 1 ms*/
  stTIM3_TimeBaseInitStruct.TIM_Prescaler = TIM3_PRESCALER; 
  stTIM3_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
  stTIM3_TimeBaseInitStruct.TIM_Period = TIM3_PERIOD;  
  stTIM3_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
  stTIM3_TimeBaseInitStruct.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM3,&stTIM3_TimeBaseInitStruct);
  
  /* Clear update interrupt bit*/
  TIM_ClearITPendingBit(TIM3,TIM_FLAG_Update);
  /* Enable update interrupt*/
  TIM_ITConfig(TIM3,TIM_FLAG_Update,ENABLE);
  
  stNVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  stNVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
  stNVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
  stNVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&stNVIC_InitStructure);
	
	/* TIM3 enable counter */
	TIM_Cmd(TIM3, ENABLE);
}

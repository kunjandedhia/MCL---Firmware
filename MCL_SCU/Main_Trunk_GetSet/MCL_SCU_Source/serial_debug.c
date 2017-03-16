/**
******************************************************************************
* @file    serial_debug.c
* @author  MCD Application Team
* @version V1.1.0
* @date    31-July-2013
* @brief   This file provide functions to retarget the C library printf function
*          to the USART. 
******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
*
* Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
* You may not use this file except in compliance with the License.
* You may obtain a copy of the License at:
*
*        http://www.st.com/software_license_agreement_liberty_v2
*
* Unless required by applicable law or agreed to in writing, software 
* distributed under the License is distributed on an "AS IS" BASIS, 
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************************
*/
/* ----------------------------------------------------------------------------
*                           Includes
* ----------------------------------------------------------------------------
*/
#include "stm32f4xx.h"
#include "Config.h"
/* ----------------------------------------------------------------------------
*                           MACROS
* ----------------------------------------------------------------------------
*/
#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
/* ----------------------------------------------------------------------------
*                           CONSTANTS
* ----------------------------------------------------------------------------
*/
/* ----------------------------------------------------------------------------
*                           GLOBAL VARIABLES
* ----------------------------------------------------------------------------
*/
RECEIVER_t stDebug; 
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
**@Function 		  : 	printf
**@Descriptions	: 	Retargets the C library printf function to the USART.
**@parameters		: 	None
**@return				: 	None
*****************************************************************************/ 
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  while (USART_GetFlagStatus(USART6, USART_FLAG_TC) == RESET);
  
  USART_SendData(USART6, (uint8_t) ch);
  
  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(USART6, USART_FLAG_TC) == RESET);
  
  return ch;
}

/*****************************************************************************
**@Function 		  : 	fnDebugReceivedData
**@Descriptions	: 	Received Data through Wifi
**@parameters		: 	None
**@return				: 	None
*****************************************************************************/ 
void fnDebugReceivedData(void) 
{
  uint8_t ucData;
  while((USART_GetFlagStatus(USART6,USART_FLAG_RXNE)==SET))
  {
    ucData = USART_ReceiveData(USART6);
    USART_SendData(USART1,ucData);
    while(!(USART_GetFlagStatus(USART1, USART_FLAG_TXE)));
    if( stDebug.uiReceivePtr < RECEIVE_BUFFER )
    {
      if(ucData == '$')
      { 
        stDebug.ucReceivedFlag = SET;
      }
      stDebug.rgucReceiveBuff[stDebug.uiReceivePtr++] = ucData;
      stDebug.rgucReceiveBuff[stDebug.uiReceivePtr] = '\0';
    }
    else
    {
      stDebug.uiReceivePtr = 0;
      memset(stDebug.rgucReceiveBuff,'\0',RECEIVE_BUFFER);
    }
  }
}	
/*****************************************************************************
**@Function      : DebugComPort_Init
**@Descriptions  : Configures the USART Peripheral for Debug.
**@parameters    : None
**@Return        : None
*****************************************************************************/
void DebugComPort_Init(void)
{
  USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* USART6 For Debug */
  /* Enable GPIO clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC , ENABLE);
  /* Enable USART clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);
  
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_USART6);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_USART6);
  
  /* Configure USART Tx and Rx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART6, &USART_InitStructure);
  
  USART_ITConfig(USART6,USART_IT_RXNE,ENABLE);  
  /* NVIC configuration */
  /* Configure the Priority Group */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
  /* Enable the USARTx Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* Enable USART */
  USART_Cmd(USART6, ENABLE);
}

/******************************************************************************
**@file     :   RTC.h
* 
* @brief   	:		this file contains all neccessory MACROS and external 
								variable and funtion decleration of RTC
******************************************************************************
*/    
/* ----------------------------------------------------------------------------
*                           Includes
* ----------------------------------------------------------------------------
*/
#include "stm32f4xx_rtc.h"
/* ----------------------------------------------------------------------------
*                           MACROS
* ----------------------------------------------------------------------------
*/
#define RTC_SIGNATURE												(uint16_t)0x1234
/* Uncomment the corresponding line to select the RTC Clock source */
#define RTC_CLOCK_SOURCE_LSE            /* LSE used as RTC source clock */
/* #define RTC_CLOCK_SOURCE_LSI */      /* LSI used as RTC source clock. The RTC Clock
                                           may varies due to LSI frequency dispersion. */ 

                                     
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
extern RTC_TimeTypeDef stRTC_TimeStructure;
extern RTC_DateTypeDef stRTC_DateStructure;
extern uint8_t rgcWeekDay[7][12];
extern RTC_InitTypeDef RTC_InitStructure;

/* ----------------------------------------------------------------------------
*                           EXTERNAL FUNCTIONS
* ----------------------------------------------------------------------------
*/

/*** RTC functions*****/
extern void fnRTC_Init(void);
static void fnRTC_Config(void);
extern void RTC_TimeRegulate(void);
extern void fnBCDtoDecDate(RTC_DateTypeDef*);
extern void fnBCDtoDecTime(RTC_TimeTypeDef*);
extern void fnRTCTime_Increament(RTC_TimeTypeDef *RTC_TimeStructure);
extern void fnRTCAlarm_Configuration(void);
extern void fnRTCCurrentTime_Read(void);
/* ----------------------------------------------------------------------------
*                           FUNCTIONS DECLARATION
* ----------------------------------------------------------------------------
*/


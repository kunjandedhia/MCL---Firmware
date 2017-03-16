/******************************************************************************
**@file     :   RTC.c
* 
**@brief    :   this file contains all neccessory functionality for on-Board RTC  
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
uint32_t uliAsynchPrediv = 0;
uint32_t uliSynchPrediv = 0;
/*Array for the Days for RTC*/
uint8_t rgcWeekDay[7][12]= {"Monday","Tuesday","Wednesday","Thursday",
"Friday","Saturday","Sunday"};
/*Time Sturcture Variable*/
RTC_TimeTypeDef stRTC_TimeStructure;
/*Date Sturcture Variable*/
RTC_DateTypeDef stRTC_DateStructure;
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
/****************************************************************
**@function 		:	fnRTC_Init
**@Discription : Initializes the RTC
**@param    		:	None
**@retval  		: None
***********************************************************/
void fnRTC_Init(void)
{
  if (RTC_SIGNATURE != RTC_ReadBackupRegister(RTC_BKP_DR0))
  {
    /* RTC configuration  */
    fnRTC_Config();
    
    /* Configure the time&date register */
    RTC_TimeRegulate(); 
  }
  else
  {
    /* Check if the Power On Reset flag is set */
    if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
    {
#ifdef RTC_DBG  
      printf("Power On Reset occurred\n" );
#endif
    }
    /* Check if the Pin Reset flag is set */
    else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
    {
#ifdef RTC_DBG   
      printf( "External Reset occurred\n" );
#endif
    }
#ifdef RTC_DBG    
    printf("No need to configure RTC\n" );
#endif
    
    /* Enable the PWR clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    
    /* Allow access to RTC */
    PWR_BackupAccessCmd(ENABLE);
    
    /* Wait for RTC APB registers synchronisation */
    RTC_WaitForSynchro();
  }
}

/*********************************************************************************
**@function		:	fnRTC_Config
**@Discription :	Configure the RTC peripheral by selecting the clock source.
**@param   		:	None
**@retval  		:	None
*************************************************************/
static void fnRTC_Config(void)
{
  RTC_InitTypeDef RTC_InitStructure;
  
  /* Enable the PWR clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
  
  /* Allow access to RTC */
  PWR_BackupAccessCmd(ENABLE);
  
#if defined (RTC_CLOCK_SOURCE_LSI)  /* LSI used as RTC source clock*/
  /* The RTC Clock may varies due to LSI frequency dispersion. */
  /* Enable the LSI OSC */ 
  RCC_LSICmd(ENABLE);
  
  /* Wait till LSI is ready */  
  while(RESET == RCC_GetFlagStatus(RCC_FLAG_LSIRDY))
  {
  }
  /* Select the RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
  
  /* ck_spre(1Hz) = RTCCLK(LSI) /(uliAsynchPrediv + 1)*(uliSynchPrediv + 1)*/
  uliSynchPrediv = 0xFF;
  uliAsynchPrediv = 0x7F;
  
#elif defined (RTC_CLOCK_SOURCE_LSE) /* LSE used as RTC source clock */
  /* Enable the LSE OSC */
  RCC_LSEConfig(RCC_LSE_ON);
  
  /* Wait till LSE is ready */  
  while(RESET == RCC_GetFlagStatus(RCC_FLAG_LSERDY))
  {
  }
  /* Select the RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
  /* ck_spre(1Hz) = RTCCLK(LSE) /(uliAsynchPrediv + 1)*(uliSynchPrediv + 1)*/
  uliSynchPrediv = 0xFF;
  uliAsynchPrediv = 0x7F;
  
#else
#error Please select the RTC Clock source inside the main.c file
#endif /* RTC_CLOCK_SOURCE_LSI */
  
  /* Configure the RTC data register and RTC prescaler */
  RTC_InitStructure.RTC_AsynchPrediv = uliAsynchPrediv;
  RTC_InitStructure.RTC_SynchPrediv = uliSynchPrediv;
  RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
  
  /* Check on RTC init */
  if (RTC_Init(&RTC_InitStructure) == ERROR)
  {
#ifdef GENERAL_DBG 
    printf("RTC Prescaler Config failed\n" );
#endif
  }
  
  /* Enable the RTC Clock */
  RCC_RTCCLKCmd(ENABLE);
  
  /* Wait for RTC APB registers synchronization */
  RTC_WaitForSynchro();  
}
/**********************************************************************
**@function		:	fnBCDtoDecTime
**@Discription	:	This funtion convert the BCD time to Decimal time.
**@parameter		:	None
**@retval 			:	None
***********************************************************************/
void fnBCDtoDecTime(RTC_TimeTypeDef* RTC_TimeStruct)
{
  /** bcd to decimal conversion **/ 
  stRTC_TimeStructure.RTC_Hours = (stRTC_TimeStructure.RTC_Hours & 0x0f) + 
    ((stRTC_TimeStructure.RTC_Hours >> 4) * 10);
  stRTC_TimeStructure.RTC_Minutes = (stRTC_TimeStructure.RTC_Minutes & 0x0f) +
    ((stRTC_TimeStructure.RTC_Minutes >> 4)* 10);
  stRTC_TimeStructure.RTC_Seconds = (stRTC_TimeStructure.RTC_Seconds & 0x0f) +
    ((stRTC_TimeStructure.RTC_Seconds >> 4)* 10);
}

/*************************************************************************
**@function		:	fnBCDtoDecDate
**@Discription	:	It converts the BCD date into the Decimal date
**@parameter	  :	None 
**@retval 	  	:	None
************************************************************************/
void fnBCDtoDecDate(RTC_DateTypeDef* RTC_DateStruct)
{
  /*** bcd to decimal conversion ***/
  stRTC_DateStructure.RTC_Date = (stRTC_DateStructure.RTC_Date & 0x0f) +
    ((stRTC_DateStructure.RTC_Date >> 4)* 10);
  stRTC_DateStructure.RTC_Month = (stRTC_DateStructure.RTC_Month & 0x0f) +
    ((stRTC_DateStructure.RTC_Month >> 4)* 10);
  stRTC_DateStructure.RTC_Year=	(stRTC_DateStructure.RTC_Year & 0x0f) + 
    ((stRTC_DateStructure.RTC_Year >> 4)* 10);
}

/*******************************************************************
**@func				:	 RTC_TimeRegulate 
**@Discription :	 set the new time and date on the RTC
**@param 			:	 None
**@retval			:	 None
******************************************************************/
void RTC_TimeRegulate(void)
{
  /* Set Time hh:mm:ss */
  stRTC_TimeStructure.RTC_H12     = RTC_H12_AM;
  stRTC_TimeStructure.RTC_Hours   = 17;  
  stRTC_TimeStructure.RTC_Minutes = 55;
  stRTC_TimeStructure.RTC_Seconds = 0;
  RTC_SetTime(RTC_Format_BIN, &stRTC_TimeStructure);
  
  /* Set Date Week/Date/Month/Year */
  stRTC_DateStructure.RTC_WeekDay = 3;
  stRTC_DateStructure.RTC_Date = 4;
  stRTC_DateStructure.RTC_Month = 02;
  stRTC_DateStructure.RTC_Year = 15;
  RTC_SetDate(RTC_Format_BIN, &stRTC_DateStructure);
  
  /* Write BkUp DR0 */
  RTC_WriteBackupRegister(RTC_BKP_DR0, RTC_SIGNATURE);
}

/*******************************************************************
**@Function		: fnTime_Configuration
**@Discription :	this funtion set the time for the alarm A. this is will
change after every 30 sec.
**@para 	    	:	None
**@retval    	:	None
********************************************************************/
void fnTime_Configuration(void)
{
  RTC_AlarmTypeDef stRTC_AlarmStructure;
  /* disable before setting or cann't write */
  RTC_AlarmCmd(RTC_Alarm_A, DISABLE);   
  RTC_GetTime(RTC_Format_BIN, &stRTC_TimeStructure);	
  fnRTCTime_Increament(&stRTC_TimeStructure);
  
  /* set alarm time 8:30:0 everyday */
  stRTC_AlarmStructure.RTC_AlarmTime.RTC_H12     = RTC_H12_AM;
  stRTC_AlarmStructure.RTC_AlarmTime.RTC_Hours   = stRTC_TimeStructure.RTC_Hours;
  stRTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes = stRTC_TimeStructure.RTC_Minutes;
  stRTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds = stRTC_TimeStructure.RTC_Seconds;
  stRTC_AlarmStructure.RTC_AlarmDateWeekDay = 0x31; // Nonspecific
  stRTC_AlarmStructure.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date;
  stRTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay; // Everyday 
  RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &stRTC_AlarmStructure);
  
  /* Enable Alarm */
  RTC_ITConfig(RTC_IT_ALRA, ENABLE);
  RTC_AlarmCmd(RTC_Alarm_A, ENABLE);
  RTC_ClearFlag(RTC_FLAG_ALRAF);	
}

/*******************************************************************************
**@Function 		   : fnRTCAlarm_Configuration
**@Description    : This function Configure the Alarm A.
**@parameters     : None
**@Return         : None
*******************************************************************************/
void fnRTCAlarm_Configuration(void)
{
  EXTI_InitTypeDef stEXTI_InitStructure;
  RTC_AlarmTypeDef stRTC_AlarmStructure;
  NVIC_InitTypeDef stNVIC_InitStructure;
  
  /* EXTI configuration */
  EXTI_ClearITPendingBit(EXTI_Line17);
  stEXTI_InitStructure.EXTI_Line = EXTI_Line17;
  stEXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  stEXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  stEXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&stEXTI_InitStructure);
  
  /* Enable the RTC Alarm Interrupt */
  stNVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn;
  stNVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  stNVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  stNVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&stNVIC_InitStructure);
  
  /* Set the alarmA Masks */
  stRTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_All;
  RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &stRTC_AlarmStructure);
  
  /* Set AlarmA subseconds and enable SubSec Alarm : generate 8 interripts per Second */
  RTC_AlarmSubSecondConfig(RTC_Alarm_A, 0xFF, RTC_AlarmSubSecondMask_SS14_5);
  /* Enable AlarmA interrupt */
  RTC_ITConfig(RTC_IT_ALRA, ENABLE);
}

/*******************************************************************************
**@Function 		: fnRTCTime_Increament
**@Discription : This function increase the RTC_Seconds member by 30 sec
in the time structure.
**@parameters  : *stRTC_TimeStructure: POinter to the time structure 
**@Return      : None
*******************************************************************************/
void fnRTCTime_Increament(RTC_TimeTypeDef *stRTC_TimeStructure)
{
  stRTC_TimeStructure->RTC_Seconds += (uint8_t)30;
  if((uint8_t)59 < stRTC_TimeStructure->RTC_Seconds)/* if seconds overflows */
  {
    stRTC_TimeStructure->RTC_Seconds-=(uint8_t)60;
    stRTC_TimeStructure->RTC_Minutes++;
    if((uint8_t)59 < stRTC_TimeStructure->RTC_Minutes) /* if the minute overflow*/
    {
      stRTC_TimeStructure->RTC_Minutes = stRTC_TimeStructure->RTC_Minutes - (uint8_t)60;
      stRTC_TimeStructure->RTC_Hours++;
      if((uint8_t)23 < stRTC_TimeStructure->RTC_Hours)/*if Hour Overflows */
      {
        stRTC_TimeStructure->RTC_Hours -= (uint8_t)24;
      }
    }
  }
}

/*************************************************************************
**@function	  : Init_Wakeup
**@Discription	:	this will initialize auto wakeup events 
**@parameter	  :	None
**@retval 		  :	None
**************************************************************************/
void Init_Wakeup(void)
{
  EXTI_InitTypeDef  EXTI_InitStructure;
  NVIC_InitTypeDef  NVIC_InitStructure;
  
  /* Allow access to RTC  */
  PWR->CR |= (1UL << 8); 
  /* Disable the RTC registers Write protection   */
  RTC_WriteProtectionCmd(DISABLE);
  
  /* Disable Wakeup Counter */
  RTC_WakeUpCmd(DISABLE);
  
  /* Ensure access to Wakeup auto-reload 
  counter and bits WUCKSEL[2:0] is allowed.
  Poll WUTWF until it is set in RTC_ISR (takes 2 RTCCLK cycles)   */
  while((RTC->ISR & (1UL << 2)) == 0)
  {
  }
  
  RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);
  RTC_SetWakeUpCounter(0x000F);
  /*RTC_SetWakeUpCounter(0x1FFF);*/
  
  /* Enable WUTIE wake up interrupt enabled */ 
  /*RTC->CR |= (1UL << 14);*/
  
  /* Enable the Wakeup Interrupt */
  RTC_ITConfig(RTC_IT_WUT, ENABLE);
  /* Connect EXTI_Line22 to the RTC Wakeup event */
  EXTI_ClearITPendingBit(EXTI_Line22);
  EXTI_InitStructure.EXTI_Line = EXTI_Line22;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  
  /* Enable the RTC Wakeup Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* Enable the RTC registers write protection  */
  RTC_WriteProtectionCmd(DISABLE);
  
  /* Enable Wakeup Counter */
  RTC_WakeUpCmd(ENABLE);
}

/******************************************************************************
**@function		 : fnRTCCurrentTime_Read
**@Discription  : This funtion will display the Current Date and Time on Display 
on the USART.
**@parameter		 : None
**@retval			 : None
*******************************************************************************/
void fnRTCCurrentTime_Read(void)
{
  /*Get the current Time and Date */
  RTC_GetTime(RTC_Format_BIN, &stRTC_TimeStructure);
  /* Display the curent time on the USART1 */
#ifdef RTC_DBG
  printf("%02d:%02d:%02d\t",stRTC_TimeStructure.RTC_Hours,
         stRTC_TimeStructure.RTC_Minutes,stRTC_TimeStructure.RTC_Seconds);
#endif
  /* Get the current Date */
  
  RTC_GetDate(RTC_Format_BIN, &stRTC_DateStructure);
  /* Display the curent date on the USART1 */
#ifdef RTC_DBG  
  printf("%s\t %02d/%02d/20%02d\n",rgcWeekDay[stRTC_DateStructure.RTC_WeekDay-1],
         stRTC_DateStructure.RTC_Date, stRTC_DateStructure.RTC_Month,
         stRTC_DateStructure.RTC_Year);  
#endif
}



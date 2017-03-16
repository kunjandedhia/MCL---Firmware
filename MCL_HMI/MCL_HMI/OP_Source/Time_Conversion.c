/******************************************************************************
**@file    Time_Conversion.c
* 
**@brief   This file contains calender time conversion
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
**@Function 		: Time_Conversion
**@Descriptions	: calender time conversion
**@parameters		: none
**@return				: none
*****************************************************************************/
// Variable declaration:
float TimeZoneHour ;
float TimeZoneMin ;
int DSTbegin[] = { //DST 2013 - 2025 in Canada and US
  310, 309, 308, 313, 312, 311, 310, 308, 314, 313, 312, 310, 309};
int DSTend[] = { //DST 2013 - 2025 in Canada and US
  1103, 1102, 1101, 1106, 1105, 1104, 1103, 1101, 1107, 1106, 1105, 1103, 1102};
int DaysAMonth[] = { //number of days a month
  31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
int gpsYear;
int gpsMonth;
int gpsDay;
float gpsHour;
float gpsMin;
char gpsSec;
float totalgpsMin = 0;
/*****************************************************************************
**@Function		 	:  TimeConvert()
**@Descriptions	:  Function to convert UTC time to offset
**@parameters		:  none
**@return				:  none
*****************************************************************************/	
void TimeConvert()
{
	TimeZoneHour =(float)stConfig_Value.ucConfig_TimeOffsetHour;
	TimeZoneMin	=(float)stConfig_Value.ucConfig_TimeOffsetMin;
	gpsYear = ((100*stConfig_Value.ucConfigSystemTime[5])+stConfig_Value.ucConfigSystemTime[6]);
	gpsMonth = stConfig_Value.ucConfigSystemTime[4];
	gpsDay  = stConfig_Value.ucConfigSystemTime[3];
	gpsHour = stConfig_Value.ucConfigSystemTime[0]; 
	gpsMin = stConfig_Value.ucConfigSystemTime[1]; 
	gpsSec = stConfig_Value.ucConfigSystemTime[2];
	
	if(0x01 == stConfig_Value.ucConfig_TimeOffsetSign)
	{
		TimeZoneHour = ((-1)*(TimeZoneHour));
	}
         if (gpsYear%4 == 0) DaysAMonth[1] = 29; //leap year check
       // printf("%d",gpsYear);
       //  printf("   ");
      //Time zone adjustment
      if(TimeZoneMin)
      {
      totalgpsMin = (gpsHour * 60) + gpsMin + ((TimeZoneHour *60) + TimeZoneMin);
    //  
    //totalgpsMin = abs(totalgpsMin);
 //   printf("%f  :",totalgpsMin);
    if(TimeZoneHour < 0)
    {
     gpsHour = (totalgpsMin  /  60) - 1;
    }
    else
    {
        gpsHour = (totalgpsMin  /  60);
    }
    
     gpsMin  = fmod(totalgpsMin, 60);
  //     printf("%f  :",gpsHour);
   //    printf("%f  :",gpsMin);
      //DST adjustment
      if(gpsMin < 0)
    gpsMin = -1* (gpsMin);
      }
  //    if (gpsMonth*100+gpsDay >= DSTbegin[gpsYear-13] &&
   //     gpsMonth*100+gpsDay < DSTend[gpsYear-13]) gpsHour += 1;
   else{
   gpsHour += TimeZoneHour;
   }
      if (gpsHour < 0)
      {
        gpsHour += 24;
        gpsDay -= 1;
        if (gpsDay < 1)
        {
          if (gpsMonth == 1)
          {
            gpsMonth = 12;
            gpsYear -= 1;
          }
          else
          {
            gpsMonth -= 1;
          }
          gpsDay = DaysAMonth[gpsMonth-1];
        }
      }
      if (gpsHour >= 24)
      {
        gpsHour -= 24;
        gpsDay += 1;
        if (gpsDay > DaysAMonth[gpsMonth-1])
        {
          gpsDay = 1;
          gpsMonth += 1;
          
          if (gpsMonth > 12)
          {
              gpsYear += 1;
          
              gpsMonth = 1;
          }
        }
      }

			
						sprintf((char *)stDisplayDateTime.ucHR,"%02d",(int)gpsHour);
						
						
						sprintf((char *)stDisplayDateTime.ucMIN,"%02d",(int)gpsMin);
					
							
						sprintf((char *)stDisplayDateTime.ucSEC,"%02d",(int)gpsSec);
						
							
						sprintf((char *)stDisplayDateTime.ucDD,"%02d",(int)gpsDay);
						
							
						sprintf((char *)stDisplayDateTime.ucMM ,"%02d",(int)gpsMonth);
						
							
						sprintf((char *)stDisplayDateTime.ucYY,"%d",(int)gpsYear);

      
    }


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
#define     DESTZONE    "TZ=America/Cayman"       // Our destination time zone
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
void TimeConvert(void)
{
 struct tm   i;
    time_t      stamp;                              // Can be negative, so works before 1970
    int currentmonth = 8;
    int currentyear =2016;
    //putenv("TZ=UTC");                               		// Begin work in Greenwich …

    i.tm_year = currentyear-1900;                          	// Populate struct members with
    i.tm_mon  = currentmonth-1;                                // the UTC time details, we use
    i.tm_mday = 29;                                 		// 29th August, 2009 12:34:56
    i.tm_hour = 01;                                 		// in this example
    i.tm_min  = 00;
    i.tm_sec  = 00;

    stamp = mktime(&i);                            		 // Convert the struct to a Unix timestamp

    //putenv(DESTZONE);                               		// Switch to destination time zone
DEBUG_PRINTF("Start Time Conversion \r\n");
	DEBUG_PRINTF("UTC  : %s\r\n",asctime(gmtime(&stamp)));
	DEBUG_PRINTF("Local: %s\r\n",asctime(localtime(&stamp)));
   

    //return 0;                                       // That’s it, folks.
	}

	
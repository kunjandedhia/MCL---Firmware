/******************************************************************************
**@file     :   Wifi.h
* 
* @brief  This File Contain all the MACROS and extern and global Funtion and Variable Decleration 
******************************************************************************
*/   
#ifndef __WIFI_H__
#define __WIFI_H__
/* ----------------------------------------------------------------------------
*                           Includes
* ----------------------------------------------------------------------------
*/
/* ----------------------------------------------------------------------------
*                           MACROS
* ----------------------------------------------------------------------------
*/
#define WIFIHEARTBEAT 	(uint8_t)20
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
 extern RECEIVER_t stWifi;
/* ----------------------------------------------------------------------------
*                           EXTERNAL FUNCTIONS
* ----------------------------------------------------------------------------
*/
 extern void fnWifiDefaultConfig(void);
 extern uint32_t fnCheckWifiBaud(void);
 extern void fnWifiInit(uint32_t uiBaudrate);
 extern void fnWifiReceivedData(void) ;
 extern void fnWifiSendData(uint8_t *pucSendData);
 extern void fnWifiProcessData(void);
 extern void fnWifiCommand(uint8_t *pucCommand);
 extern void fnWifiCommandMode(void);
 extern void fnWifiApplysSetting(void);
 
 
#endif /* __WIFI_H__ */ 

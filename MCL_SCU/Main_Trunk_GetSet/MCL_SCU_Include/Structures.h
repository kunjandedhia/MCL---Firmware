/******************************************************************************
**@file     :   Structures.h
* 
* @brief  This File Contain all the Structure Variable Decleration 
******************************************************************************
*/    
#ifndef __STRUCTURES_H__
#define __STRUCTURES_H__
/* ----------------------------------------------------------------------------
*                           Includes
* ----------------------------------------------------------------------------
*/
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

 typedef struct {
	uint8_t rgucReceiveBuff[RECEIVE_BUFFER];
	uint16_t uiReceivePtr;
	uint8_t ucReceivedFlag;
	uint8_t ucConnectedStatus;
	uint8_t ucLinkStatus;
	uint8_t ucSendFlag;
	uint8_t ucBusyFlag;
	uint16_t uiReceiveTimer;
 }RECEIVER_t;
 

 typedef struct {
	uint32_t ExtId;
  uint8_t DLC;
	uint8_t rgucData[8];
 }XML_t;
 
 typedef struct
{
  uint8_t ucSentIndex;
  uint8_t ucStoreIndex;
  //uint8_t rgucXMLtxBuffer[QUEUE][TRANSMIT_BUFFER];
  CanTxMsg stCanTx_XML[QUEUE];
	uint8_t ucTimeStamp[QUEUE][7];
}XML_TX_t;

 typedef struct {
	 uint8_t rgucIP_Addr[16];
	 uint8_t rgucSubnet_Addr[16];
	 uint8_t rgucGateway_Addr[16];
	 uint16_t uiPort;
	 uint8_t rgucSSID[16];
		uint8_t rgucPASWD[16];
 }IPconfig_t;
 
 
// typedef struct {
// 	 uint8_t ucETHSendFlag;
//	 uint8_t ucWifiSendFlag;
//	 uint8_t ucETH_LogType;
//	 uint8_t ucWifi_LogType;
//	 uint32_t uliETHStartAddress;
//	 uint32_t uliETHStopAddress;
//	 uint32_t uliWifiStartAddress;
//	 uint32_t uliWifiStopAddress;
//	 uint32_t uliStartAddress;
//	 uint32_t uliStopAddress;
//	 uint8_t ucNextLogFlag;
//	 uint8_t ucETHDetailFlag;
//	 uint8_t ucWifiDetailFlag;
//	 uint8_t ucETHStartLogTimeFlag;
//	 uint8_t ucWifiStartLogTimeFlag;
//	 uint8_t ucETHStopLogTimeFlag;
//	 uint8_t ucWifiStopLogTimeFlag;
//	 
//	 uint8_t bETHGetConfigFlag;
//	 uint8_t ucETHGetConfigCount;
//	 uint8_t bWifiGetConfigFlag;
//	 uint8_t ucWifiGetConfigCount;
// }DATALOG_t;
 
   typedef struct {
	uint8_t ucDate;
	uint8_t ucMonth;
	uint8_t ucYear;
	uint8_t ucHour;
	uint8_t ucMin;
	uint8_t ucSec;
	uint32_t uliTotalSec;

		 
 }DateTime_t;
	 
  typedef struct {
		char cJobName[22];
		char cJobID[22];
		char cProcess[22];
 	 uint8_t ucSendFlag;
	 uint8_t ucLogType;
	 uint8_t ucNextLogFlag;
	 uint8_t ucDetailFlag;
	 uint8_t ucStartLogTimeFlag;
	 uint8_t ucStopLogTimeFlag;
	 uint8_t ucGetJobID;
	 uint8_t ucStoredJobID;
	 uint8_t ucGetJobName;
	 uint8_t ucStoredJobName;
			 uint8_t ucGetProcessID;
	 uint8_t ucStoredProcessID;
	uint8_t ucAddrFlag;
	 uint32_t uliStartAddress;
	 uint32_t uliStopAddress;
		
		DateTime_t stStartLogDateTime;
	 DateTime_t stEndLogDateTime;

 }DATALOG_TX_t;
	

// 	 uint8_t ucETHSendFlag;
//	 uint8_t ucWifiSendFlag;
//	 uint8_t ucETH_LogType;
//	 uint8_t ucWifi_LogType;
//	 uint32_t uliETHStartAddress;
//	 uint32_t uliETHStopAddress;
//	 uint32_t uliWifiStartAddress;
//	 uint32_t uliWifiStopAddress;
//	 uint32_t uliStartAddress;
//	 uint32_t uliStopAddress;
//	 uint8_t ucNextLogFlag;
//	 uint8_t ucETHDetailFlag;
//	 uint8_t ucWifiDetailFlag;
//	 uint8_t ucETHStartLogTimeFlag;
//	 uint8_t ucWifiStartLogTimeFlag;
//	 uint8_t ucETHStopLogTimeFlag;
//	 uint8_t ucWifiStopLogTimeFlag;
/* ----------------------------------------------------------------------------
*                           EXTERNAL VARIABLES
* ----------------------------------------------------------------------------
*/
/* ----------------------------------------------------------------------------
*                           EXTERNAL FUNCTIONS
* ----------------------------------------------------------------------------
*/

#endif /* __STRUCTURES_H__ */


/******************************************************************************
**@file     :   Ethernet_Server.h
* 
* @brief  This File Contain all the MACROS and extern and global Funtion and Variable Decleration 
******************************************************************************
*/    
#ifndef __ETHERNET_SERVER_H__
#define __ETHERNET_SERVER_H__
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
/* ----------------------------------------------------------------------------
*                           EXTERNAL VARIABLES
* ----------------------------------------------------------------------------
*/ 
 extern struct tcp_pcb *pstConnectPCB;
 extern IPconfig_t stEthernetIP;
 extern IPconfig_t stWifiIP;
 extern RECEIVER_t stEthernet;
/* ----------------------------------------------------------------------------
*                           EXTERNAL FUNCTIONS
* ----------------------------------------------------------------------------
*/

 extern void fnEthernetSend(uint8_t *pucTxData);
 extern void fnTcpServerInit(void);
 extern void fnEthernetProcessData(void);
 extern void fnEthernetConfig(void);


#endif /* __ETHERNET_SERVER_H__ */

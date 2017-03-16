/******************************************************************************
**@file     :   Macros.h
* 
* @brief  This File Contain all the MACROS Decleration 
******************************************************************************
*/
#ifndef __MACROS_H__
#define __MACROS_H__
/* ----------------------------------------------------------------------------
*                           Includes
* ----------------------------------------------------------------------------
*/
/* ----------------------------------------------------------------------------
*                           MACROS
* ----------------------------------------------------------------------------
*/
#define RECEIVE_BUFFER       					15000
#define TRANSMIT_BUFFER       				4000
#define DATALOG_TIMEOUT								10
#define RESPONSE_TIMEOUT_PERIOD 			100
/* MAC ADDRESS: MAC_ADDR0:MAC_ADDR1:MAC_ADDR2:MAC_ADDR3:MAC_ADDR4:MAC_ADDR5 */
#define MAC_ADDR0  										2
#define MAC_ADDR1   									0
#define MAC_ADDR2  										0
#define MAC_ADDR3   									0
#define MAC_ADDR4   									0
#define MAC_ADDR5   									0
/* Ethernet Default IP Address */
#define ETH_IP_ADDRESS  							"192.168.1.100"
/* Ethernet Default Subnet Mask */
#define ETH_SUBNETMASK   							"255.255.255.0"
/* Ethernet Default Gateway */
#define ETH_GATEWAY   								"192.168.1.1"
/* Ethernet Default Port */
#define ETH_PORT   										992

/* WIFI Default IP Address */
#define WIFI_IP_ADDRESS  							"192.168.1.101"
/* WIFI Default Subnet Mask */
#define WIFI_SUBNETMASK   						"255.255.255.0"
/* WIFI Default Gateway */
#define WIFI_GATEWAY   								"192.168.1.1"
/* WIFI Default Port */
#define WIFI_PORT   									992
/* WIFI Default SSID */
#define WIFI_SSID   									"CRC-EVANS_MCL_1"
/* WIFI Default Password */
#define WIFI_PASSWORD   							"12345678"


#define QUEUE    											50
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
#endif /* __MACROS_H__ */

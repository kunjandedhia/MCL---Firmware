/**
  ******************************************************************************
  * @file    ETH_Config.h
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    31-July-2013 
  * @brief   Header for ETH_Config.c file.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ETH_CONFIG_H
#define __ETH_CONFIG_H

#ifdef __cplusplus
 extern "C" {
#endif

/* ----------------------------------------------------------------------------
*                           Includes
* ----------------------------------------------------------------------------
*/
#include "netif.h"
/* ----------------------------------------------------------------------------
*                           MACROS
* ----------------------------------------------------------------------------
*/
/* ----------------------------------------------------------------------------
*                           CONSTANTS
* ----------------------------------------------------------------------------
*/
#define KSZ8081_PHY_ADDR          0x00 /* Relative to STM324xG-EVAL Board */

/* Ethernet Flags for EthStatus variable */
#define ETH_INIT_FLAG           0x01 /* Ethernet Init Flag */
#define ETH_LINK_FLAG           0x10 /* Ethernet Link Flag */
/* ----------------------------------------------------------------------------
*                           GLOBAL VARIABLES
* ----------------------------------------------------------------------------
*/
/* ----------------------------------------------------------------------------
*                           EXTERNAL VARIABLES
* ----------------------------------------------------------------------------
*/
extern __IO uint32_t LocalTime; /* this variable is used to create a time reference incremented by 10ms */
extern uint16_t uiResponseTime;
/* ----------------------------------------------------------------------------
*                           EXTERNAL FUNCTIONS
* ----------------------------------------------------------------------------
*/
void fnETH_Init(void);
void fnEth_Link_Handler(uint16_t PHYAddress);
void fnETH_link_callback(struct netif *netif);
void fnSysTick_Init(void);
void fnTimeUpdate(void);
void fnDelay_ms(uint32_t nCount);

#ifdef __cplusplus
}
#endif

#endif /* __ETH_CONFIG_H */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

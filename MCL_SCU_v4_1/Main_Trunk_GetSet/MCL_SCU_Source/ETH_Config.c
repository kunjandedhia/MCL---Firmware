/**
******************************************************************************
* @file    ETH_Config.c
* @author  MCD Application Team
* @version V1.1.0
* @date    31-July-2013 
* @brief   STM32F4x7 Ethernet hardware configuration.
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
#include "lwip/opt.h"
#include "stm32f4xx_eth.h"
#include "ETH_Config.h"
#include "Config.h"
#include "netif.h"
#include "Netconfig.h"
#include "lwip/dhcp.h"
/* ----------------------------------------------------------------------------
*                           MACROS
* ----------------------------------------------------------------------------
*/
#define SYSTEMTICK_PERIOD_MS  10
/* ----------------------------------------------------------------------------
*                           CONSTANTS
* ----------------------------------------------------------------------------
*/
/* ----------------------------------------------------------------------------
*                           GLOBAL VARIABLES
* ----------------------------------------------------------------------------
*/
__IO uint32_t LocalTime = 0; /* this variable is used to create a time reference incremented by 10ms */
uint32_t timingdelay;
uint8_t ucTimeDelay=0;
ETH_InitTypeDef ETH_InitStructure;
__IO uint32_t  EthStatus = 0;
/* ----------------------------------------------------------------------------
*                           EXTERNAL VARIABLES
* ----------------------------------------------------------------------------
*/
extern struct netif gnetif;
#ifdef USE_DHCP
extern __IO uint8_t DHCP_state;
#endif /* LWIP_DHCP */
/* ----------------------------------------------------------------------------
*                           EXTERNAL FUNCTIONS
* ----------------------------------------------------------------------------
*/
/* ----------------------------------------------------------------------------
*                           FUNCTIONS DECLARATION
* ----------------------------------------------------------------------------
*/
static void fnETH_GPIO_Config(void);
static void fnETH_MACDMA_Config(void);

/*****************************************************************************
**@Function 		  : 	fnSysTick_Init
**@Descriptions	: 	This function Initilize Sys Tick.
**@parameters		: 	None
**@return				: 	None
*****************************************************************************/
void fnSysTick_Init(void)
{ 
  RCC_ClocksTypeDef RCC_Clocks;
  
  /***************************************************************************
  NOTE: 
  When using Systick to manage the delay in Ethernet driver, the Systick
  must be configured before Ethernet initialization and, the interrupt 
  priority should be the highest one.
  *****************************************************************************/
  
  /* Configure Systick clock source as HCLK */
  SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
  
  /* SystTick configuration: an interrupt every 10ms */
  RCC_GetClocksFreq(&RCC_Clocks);
  SysTick_Config(RCC_Clocks.HCLK_Frequency / 100);
  
  /* Set Systick interrupt priority to 0*/
  NVIC_SetPriority (SysTick_IRQn, 0);
}
/*****************************************************************************
**@Function 		  : 	fnDelay_ms
**@Descriptions	: 	Inserts a delay time.
**@parameters		: 	nCount: number of 10ms periods to wait for.
**@return				: 	None
*****************************************************************************/ 
void fnDelay_ms(uint32_t nCount)
{
  /* Capture the current local time */
  timingdelay = LocalTime + nCount;  
  /* wait until the desired delay finish */  
  while(timingdelay > LocalTime)
  {     
  }
}
/*****************************************************************************
**@Function 		  : 	fnTimeUpdate
**@Descriptions	: 	Updates the system local time
**@parameters		: 	None
**@return				: 	None
*****************************************************************************/ 
void fnTimeUpdate(void)
{
  LocalTime += SYSTEMTICK_PERIOD_MS;
  uiResponseTime++;
  ucTimeDelay++;
  if(ucTimeDelay > DATALOG_TIMEOUT)
  {
    ucTimeDelay=0;
    stEthernetLog.ucNextLogFlag = SET;
  }
}
/*****************************************************************************
**@Function 		  : 	fnETH_Init
**@Descriptions	: 	Initilize Ethernet GPIO, DMA, MAC
**@parameters		: 	None
**@return				: 	None
*****************************************************************************/ 
void fnETH_Init(void)
{
  
  /* Configure the GPIO ports for ethernet pins */
  fnETH_GPIO_Config();
  
  /* Configure the Ethernet MAC/DMA */
  fnETH_MACDMA_Config();
  
  /* Get Ethernet link status*/
  if(ETH_ReadPHYRegister(KSZ8081_PHY_ADDR, PHY_SR) & 1)
  {
    EthStatus |= ETH_LINK_FLAG;
  }
  
}
/*****************************************************************************
**@Function 		  : 	fnETH_MACDMA_Config
**@Descriptions	: 	Configures the Ethernet Interface
**@parameters		: 	None
**@return				: 	None
*****************************************************************************/ 
static void fnETH_MACDMA_Config(void)
{  
  /* Enable ETHERNET clock  */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_ETH_MAC | RCC_AHB1Periph_ETH_MAC_Tx |
                         RCC_AHB1Periph_ETH_MAC_Rx, ENABLE);
  
  NVIC_InitTypeDef   NVIC_InitStructure;
  /* Reset ETHERNET on AHB Bus */
  ETH_DeInit();
  
  /* Software reset */
  ETH_SoftwareReset();
  
  /* Wait for software reset */
  while (ETH_GetSoftwareResetStatus() == SET);
  
  /* ETHERNET Configuration --------------------------------------------------*/
  /* Call ETH_StructInit if you don't like to configure all ETH_InitStructure parameter */
  ETH_StructInit(&ETH_InitStructure);
  
  /* Fill ETH_InitStructure parametrs */
  /*------------------------   MAC   -----------------------------------*/
  ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Enable;
  //  ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Disable;
  //  ETH_InitStructure.ETH_Speed = ETH_Speed_10M;
  //  ETH_InitStructure.ETH_Mode = ETH_Mode_FullDuplex;
  
  ETH_InitStructure.ETH_LoopbackMode = ETH_LoopbackMode_Disable;
  ETH_InitStructure.ETH_RetryTransmission = ETH_RetryTransmission_Disable;
  ETH_InitStructure.ETH_AutomaticPadCRCStrip = ETH_AutomaticPadCRCStrip_Disable;
  ETH_InitStructure.ETH_ReceiveAll = ETH_ReceiveAll_Disable;
  ETH_InitStructure.ETH_BroadcastFramesReception = ETH_BroadcastFramesReception_Enable;
  ETH_InitStructure.ETH_PromiscuousMode = ETH_PromiscuousMode_Disable;
  ETH_InitStructure.ETH_MulticastFramesFilter = ETH_MulticastFramesFilter_Perfect;
  ETH_InitStructure.ETH_UnicastFramesFilter = ETH_UnicastFramesFilter_Perfect;
#ifdef CHECKSUM_BY_HARDWARE
  ETH_InitStructure.ETH_ChecksumOffload = ETH_ChecksumOffload_Enable;
#endif
  
  /*------------------------   DMA   -----------------------------------*/  
  
  /* When we use the Checksum offload feature, we need to enable the Store and Forward mode: 
  the store and forward guarantee that a whole frame is stored in the FIFO, so the MAC can insert/verify the checksum, 
  if the checksum is OK the DMA can handle the frame otherwise the frame is dropped */
  ETH_InitStructure.ETH_DropTCPIPChecksumErrorFrame = ETH_DropTCPIPChecksumErrorFrame_Enable;
  ETH_InitStructure.ETH_ReceiveStoreForward = ETH_ReceiveStoreForward_Enable;
  ETH_InitStructure.ETH_TransmitStoreForward = ETH_TransmitStoreForward_Enable;
  
  ETH_InitStructure.ETH_ForwardErrorFrames = ETH_ForwardErrorFrames_Disable;
  ETH_InitStructure.ETH_ForwardUndersizedGoodFrames = ETH_ForwardUndersizedGoodFrames_Disable;
  ETH_InitStructure.ETH_SecondFrameOperate = ETH_SecondFrameOperate_Enable;
  ETH_InitStructure.ETH_AddressAlignedBeats = ETH_AddressAlignedBeats_Enable;
  ETH_InitStructure.ETH_FixedBurst = ETH_FixedBurst_Enable;
  ETH_InitStructure.ETH_RxDMABurstLength = ETH_RxDMABurstLength_32Beat;
  ETH_InitStructure.ETH_TxDMABurstLength = ETH_TxDMABurstLength_32Beat;
  ETH_InitStructure.ETH_DMAArbitration = ETH_DMAArbitration_RoundRobin_RxTx_2_1;
  
  /* Configure Ethernet */
  EthStatus = ETH_Init(&ETH_InitStructure, KSZ8081_PHY_ADDR);
  
  ETH_DMAITConfig(ETH_DMA_IT_NIS | ETH_DMA_IT_R, ENABLE);
  
  /* Enable the Ethernet global Interrupt  */
  NVIC_InitStructure.NVIC_IRQChannel = ETH_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);   
  
}
/*****************************************************************************
**@Function 		  : 	fnETH_GPIO_Config
**@Descriptions	: 	Configures the different GPIO ports
**@parameters		: 	None
**@return				: 	None
*****************************************************************************/ 
void fnETH_GPIO_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* Enable GPIOs clocks */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB |
                         RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOI |
                           RCC_AHB1Periph_GPIOG | RCC_AHB1Periph_GPIOH |
                             RCC_AHB1Periph_GPIOF, ENABLE);
  
  /* Enable SYSCFG clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  
  /* Configure MCO (PA8) */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  /* MII/RMII Media interface selection --------------------------------------*/
  
  RCC_MCO1Config(RCC_MCO1Source_HSE, RCC_MCO1Div_1);
  
  SYSCFG_ETH_MediaInterfaceConfig(SYSCFG_ETH_MediaInterface_RMII);
  
  /* Ethernet pins configuration ************************************************/
  /*
  ETH_MDIO -------------------------> PA2
  ETH_MDC --------------------------> PC1
  ETH_PPS_OUT ----------------------> PB5
  ETH_MII_CRS ----------------------> PH2
  ETH_MII_COL ----------------------> PH3
  ETH_MII_RX_ER --------------------> PI10
  ETH_MII_RXD2 ---------------------> PH6
  ETH_MII_RXD3 ---------------------> PH7
  ETH_MII_TX_CLK -------------------> PC3
  ETH_MII_TXD2 ---------------------> PC2
  ETH_MII_TXD3 ---------------------> PB8
  ETH_MII_RX_CLK/ETH_RMII_REF_CLK---> PA1
  ETH_MII_RX_DV/ETH_RMII_CRS_DV ----> PA7
  ETH_MII_RXD0/ETH_RMII_RXD0 -------> PC4
  ETH_MII_RXD1/ETH_RMII_RXD1 -------> PC5
  ETH_MII_TX_EN/ETH_RMII_TX_EN -----> PG11
  ETH_MII_TXD0/ETH_RMII_TXD0 -------> PG13
  ETH_MII_TXD1/ETH_RMII_TXD1 -------> PG14
  */
  
  /* Configure PA1, PA2 and PA7 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_7;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_ETH);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_ETH);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_ETH);
  
  /* Configure PB5 */
  //  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  //  GPIO_Init(GPIOB, &GPIO_InitStructure);
  //  GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_ETH);
  
  /* Configure PC1, PC4 and PC5 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource1, GPIO_AF_ETH);  
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource4, GPIO_AF_ETH);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource5, GPIO_AF_ETH);
  
  /* Configure PG11, PG14 and PG13 */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_11 | GPIO_Pin_13 | GPIO_Pin_14;
  GPIO_Init(GPIOG, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource11, GPIO_AF_ETH);
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource13, GPIO_AF_ETH);
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource14, GPIO_AF_ETH);  
  
}

/*****************************************************************************
**@Function 		  : 	fnEth_Link_Handler
**@Descriptions	: 	This function handles Ethernet link status.
**@parameters		: 	None
**@return				: 	None
*****************************************************************************/
void fnEth_Link_Handler(uint16_t PHYAddress)
{
  /* Check whether the link Down has occurred or not */
  if((ETH_ReadPHYRegister(PHYAddress, PHY_SR) & PHY_LINK_STATUS))
  {
    if(stEthernet.ucLinkStatus == RESET)
    {
#ifdef DBG_ETH
      printf("\n\rEthernet Link  Up");
#endif
      stEthernet.ucLinkStatus = SET;
      netif_set_link_up(&gnetif);
    }
  }
  else
  {
    if(stEthernet.ucLinkStatus == SET)
    {
      /* Reset Ethernet Link Flag */
      stEthernet.ucConnectedStatus = RESET;	
#ifdef DBG_ETH
      printf("\n\rEthernet Link  Down");
#endif
      stEthernet.ucLinkStatus = RESET;
      netif_set_link_down(&gnetif);
    }
  }
}

/*****************************************************************************
**@Function 		  : 	fnETH_link_callback
**@Descriptions	: 	Link callback function, this function is called on change of link status.
**@parameters		: 	The network interface
**@return				: 	None
*****************************************************************************/
void fnETH_link_callback(struct netif *netif)
{
  __IO uint32_t timeout = 0;
  uint32_t tmpreg,RegValue;
  struct ip_addr ipaddr;
  struct ip_addr netmask;
  struct ip_addr gw;
#ifndef USE_DHCP
  //  uint8_t iptab[4] = {0};
  //  uint8_t iptxt[20];
#endif /* USE_DHCP */
  
  
  if(netif_is_link_up(netif))
  {
    /* Restart the autonegotiation */
    if(ETH_InitStructure.ETH_AutoNegotiation != ETH_AutoNegotiation_Disable)
    {
      /* Reset Timeout counter */
      timeout = 0;
      
      /* Enable Auto-Negotiation */
      ETH_WritePHYRegister(KSZ8081_PHY_ADDR, PHY_BCR, PHY_AutoNegotiation);
      
      /* Wait until the auto-negotiation will be completed */
      do
      {
        timeout++;
      } while (!(ETH_ReadPHYRegister(KSZ8081_PHY_ADDR, PHY_BSR) & PHY_AutoNego_Complete) && (timeout < (uint32_t)PHY_READ_TO));
      
      /* Reset Timeout counter */
      timeout = 0;
      
      /* Read the result of the auto-negotiation */
      RegValue = ETH_ReadPHYRegister(KSZ8081_PHY_ADDR, PHY_SR);
      
      /* Configure the MAC with the Duplex Mode fixed by the auto-negotiation process */
      if((RegValue & PHY_DUPLEX_STATUS) != (uint32_t)RESET)
      {
        /* Set Ethernet duplex mode to Full-duplex following the auto-negotiation */
        ETH_InitStructure.ETH_Mode = ETH_Mode_FullDuplex;  
      }
      else
      {
        /* Set Ethernet duplex mode to Half-duplex following the auto-negotiation */
        ETH_InitStructure.ETH_Mode = ETH_Mode_HalfDuplex;
      }
      /* Configure the MAC with the speed fixed by the auto-negotiation process */
      if(RegValue & PHY_SPEED_STATUS)
      {
        /* Set Ethernet speed to 10M following the auto-negotiation */
        ETH_InitStructure.ETH_Speed = ETH_Speed_10M; 
      }
      else
      {
        /* Set Ethernet speed to 100M following the auto-negotiation */ 
        ETH_InitStructure.ETH_Speed = ETH_Speed_100M;    
      }
      
      /*------------------------ ETHERNET MACCR Re-Configuration --------------------*/
      /* Get the ETHERNET MACCR value */  
      tmpreg = ETH->MACCR;
      
      /* Set the FES bit according to ETH_Speed value */ 
      /* Set the DM bit according to ETH_Mode value */ 
      tmpreg |= (uint32_t)(ETH_InitStructure.ETH_Speed | ETH_InitStructure.ETH_Mode);
      
      /* Write to ETHERNET MACCR */
      ETH->MACCR = (uint32_t)tmpreg;
      
      _eth_delay_(ETH_REG_WRITE_DELAY);
      tmpreg = ETH->MACCR;
      ETH->MACCR = tmpreg;
    }
    
    /* Restart MAC interface */
    ETH_Start();
    
#ifdef USE_DHCP
    ipaddr.addr = 0;
    netmask.addr = 0;
    gw.addr = 0;
    DHCP_state = DHCP_START;
#else
    uint8_t ucIP1=0,ucIP2=0,ucIP3=0,ucIP4=0,rgucIPBuff[15];
    
    strcpy((char *)rgucIPBuff,(char *)stEthernetIP.rgucIP_Addr);
    ucIP1 = atoi(strtok((char *)rgucIPBuff,"."));
    ucIP2 = atoi(strtok(NULL,"."));
    ucIP3 = atoi(strtok(NULL,"."));
    ucIP4 = atoi(strtok(NULL,"."));	
    IP4_ADDR(&ipaddr, ucIP1, ucIP2, ucIP3, ucIP4);
    
    strcpy((char *)rgucIPBuff,(char *)stEthernetIP.rgucSubnet_Addr);
    ucIP1 = atoi(strtok((char *)rgucIPBuff,"."));
    ucIP2 = atoi(strtok(NULL,"."));
    ucIP3 = atoi(strtok(NULL,"."));
    ucIP4 = atoi(strtok(NULL,"."));	
    IP4_ADDR(&netmask, ucIP1, ucIP2, ucIP3, ucIP4);
    
    strcpy((char *)rgucIPBuff,(char *)stEthernetIP.rgucGateway_Addr);
    ucIP1 = atoi(strtok((char *)rgucIPBuff,"."));
    ucIP2 = atoi(strtok(NULL,"."));
    ucIP3 = atoi(strtok(NULL,"."));
    ucIP4 = atoi(strtok(NULL,"."));	
    IP4_ADDR(&gw, ucIP1, ucIP2, ucIP3, ucIP4);
    //    IP4_ADDR(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
    //    IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1 , NETMASK_ADDR2, NETMASK_ADDR3);
    //    IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
#endif /* USE_DHCP */
    
    netif_set_addr(&gnetif, &ipaddr , &netmask, &gw);
    
    /* When the netif is fully configured this function must be called.*/
    netif_set_up(&gnetif);   
  }
  else
  {
    ETH_Stop();
#ifdef USE_DHCP
    DHCP_state = DHCP_LINK_DOWN;
    dhcp_stop(netif);
#endif /* USE_DHCP */
    
    /*  When the netif link is down this function must be called.*/
    netif_set_down(&gnetif);
    
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

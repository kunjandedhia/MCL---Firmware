/******************************************************************************
**@file     :  Wifi.c
* 
**@brief    :  This File contains the functions  for handling wifi communication.
*******************************************************************************/    

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
RECEIVER_t stWifi;
uint16_t uiResponseTime=0;
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
**@Function 		  : 	fnWifiSendCommand
**@Descriptions	: 	Send Command from USART to Wifi
**@parameters		: 	pucCommand : pointer to sending Command string
**@return				: 	None
*****************************************************************************/ 
void fnWifiSendCommand(uint8_t *pucCommand)
{
  uint16_t uiLoop=0;
  /*Send data over usart*/
  for(uiLoop=0; NULL !=  pucCommand[uiLoop];uiLoop++)
  {
    USART_SendData(USART1,pucCommand[uiLoop]);
    while(!(USART_GetFlagStatus(USART1, USART_FLAG_TXE)));
  }
  /*Send new line*/
  USART_SendData(USART1,'\n');
  while(!(USART_GetFlagStatus(USART1, USART_FLAG_TXE)));
  /*send Cartriage return*/
  USART_SendData(USART1,'\r');
  while(!(USART_GetFlagStatus(USART1, USART_FLAG_TXE)));
}
/*****************************************************************************
**@Function 		  : 	fnWifiCommand
**@Descriptions	: 	Send Command to wifi and check response and return or else with timeout
**@parameters		: 	pucCommand : pointer to Command
**@return				: 	None
*****************************************************************************/ 
void fnWifiCommand(uint8_t *pucCommand)
{
  uint8_t rgucResponse[10];
  /*Empty Response Buffer*/
  memset(rgucResponse,'\0',10);
  if(strcmp((char *)pucCommand,"save") == 0)
  {
    /*Check for Response "Storing"*/
    strcpy((char *)rgucResponse,"Storing");
  }
  else if(strcmp((char *)pucCommand,"reboot") == 0)
  {
    /*Check for Response "Reboot"*/
    strcpy((char *)rgucResponse,"Reboot");
  }
  else
  {
    /*Check for Response "OK"*/
    strcpy((char *)rgucResponse,"OK");
  }
  /*Empty Wifi Receiving Buffer*/
  memset(stWifi.rgucReceiveBuff,'\0',RECEIVE_BUFFER);
  /*Move Received Buffer pointer to Zero position*/
  stWifi.uiReceivePtr = 0;
  /*Send command over wifi*/
  fnWifiSendCommand(pucCommand);
  /*Reset and Start Response Timer*/
  uiResponseTime=0;
  /*Wait for the valid response till timeout*/
  while(uiResponseTime <= RESPONSE_TIMEOUT_PERIOD)
  {
    /*Response Received the break while loop*/
    if(strstr((char *)stWifi.rgucReceiveBuff,(char *)rgucResponse) != NULL)
    {
#ifdef DBG_WIFI
      printf("\n\rResponse");
#endif
      break;
    }
  }
}
/*****************************************************************************
**@Function 		  : 	fnWifiCommandMode
**@Descriptions	: 	Enter Wifi in configuratuon mode
**@parameters		: 	None
**@return				: 	None
*****************************************************************************/ 
void fnWifiCommandMode(void)
{
  /*Empty Wifi Receiving Buffer*/
  memset(stWifi.rgucReceiveBuff,'\0',RECEIVE_BUFFER);
  /*Move Received Buffer pointer to Zero position*/
  stWifi.uiReceivePtr = 0;
  /*Reset and Start Response Timer*/
  uiResponseTime=0;
  /*Enter wifi in command mode by sending $$$*/
  USART_SendData(USART1,'$');
  while(!(USART_GetFlagStatus(USART1, USART_FLAG_TXE)));
  USART_SendData(USART1,'$');
  while(!(USART_GetFlagStatus(USART1, USART_FLAG_TXE)));
  USART_SendData(USART1,'$');
  while(!(USART_GetFlagStatus(USART1, USART_FLAG_TXE)));
  /*Wait for the valid response till timeout*/
  while(uiResponseTime <= RESPONSE_TIMEOUT_PERIOD)
  {
    /*Response Received the break while loop*/
    if(strstr((char *)stWifi.rgucReceiveBuff,"CMD") != NULL)
    {
#ifdef DBG_WIFI
      printf("\n\rCMD mode");
#endif
      break;
    }
  }
}
/*****************************************************************************
**@Function 		  : 	fnWifiSendData
**@Descriptions	: 	Send Data from USART to Wifi
**@parameters		: 	pucSendData : pointer to sending data string
**@return				: 	None
*****************************************************************************/ 
void fnWifiSendData(uint8_t *pucSendData)
{
  /*Check status if wifi connected*/
  if(SET == stWifi.ucConnectedStatus)
  {
    /*Send Data over Wifi USART*/
    uint16_t uiLoop=0;
    for(uiLoop=0; NULL != pucSendData[uiLoop] ;uiLoop++)
    {
      USART_SendData(USART1,pucSendData[uiLoop]);
      while(!(USART_GetFlagStatus(USART1, USART_FLAG_TXE)));
    }
		 // printf("\n\rTx %s",pucSendData);
  }
}

/*****************************************************************************
**@Function 		  : 	fnWifiReceivedData
**@Descriptions	: 	Received Data through Wifi
**@parameters		: 	None
**@return				: 	None
*****************************************************************************/ 
void fnWifiReceivedData(void) 
{
  uint8_t ucData;
  while((USART_GetFlagStatus(USART1,USART_FLAG_RXNE)==SET))
  {
    ucData = USART_ReceiveData(USART1);
    /*Check if wifi receive pointer is less than the max buffer size*/
    if( stWifi.uiReceivePtr < RECEIVE_BUFFER )
    {
      /*Store Byte by byte data in Wifi Receive Buffer*/
      stWifi.rgucReceiveBuff[stWifi.uiReceivePtr++] = ucData;
      stWifi.rgucReceiveBuff[stWifi.uiReceivePtr] = '\0';
      /*Check For wifi received data*/
      if(ucData == '#')
      {
        /*Wifi Received flag set to process data*/
        stWifi.ucReceivedFlag = SET;
        /*Wifi Received Timer Reset*/
        stWifi.uiReceiveTimer = RESET;
      }
      /*Check For wifi connected or disconnected*/
      else if(ucData == '*')
      {
        /*Wifi Connected if received *OPEN**/
        if(strstr((char *)stWifi.rgucReceiveBuff,"OPEN"))
        {
          stWifi.ucConnectedStatus = SET;
#ifdef DBG_WIFI
          printf("\n\rWifi Connected");
#endif
          /*Empty Wifi Receiving Buffer*/
          memset(stWifi.rgucReceiveBuff,'\0',RECEIVE_BUFFER);
          /*Move Received Buffer pointer to Zero position*/
          stWifi.uiReceivePtr = 0;
        }
        /*Wifi Disconnected if received *CLOS**/
        else if(strstr((char *)stWifi.rgucReceiveBuff,"CLOS"))
        {
          stWifi.ucConnectedStatus = RESET;
#ifdef DBG_WIFI
          printf("\n\rWifi Disconnected");
#endif
          /*Empty Wifi Receiving Buffer*/
          memset(stWifi.rgucReceiveBuff,'\0',RECEIVE_BUFFER);
          /*Move Received Buffer pointer to Zero position*/
          stWifi.uiReceivePtr = 0;
        }
      }
    }
    /*If wifi received pointer size exceed max buffer size empty buffer*/
    else
    {
      /*Empty Wifi Receiving Buffer*/
      memset(stWifi.rgucReceiveBuff,'\0',RECEIVE_BUFFER);
      /*Move Received Buffer pointer to Zero position*/
      stWifi.uiReceivePtr = 0;
    }
  }
}	

/*****************************************************************************
**@Function 		  : 	fnWifiInit
**@Descriptions	: 	Initilize Wifi USART 1.
**@parameters		: 	uiBaudrate : Baudrate
**@return				: 	None
*****************************************************************************/
void fnWifiInit(uint32_t uiBaudrate)
{
  /* USARTx configured as follow:
  - BaudRate = uiBaudrate 
  - Word Length = 8 Bits
  - One Stop Bit
  - No parity
  - Hardware flow control disabled (RTS and CTS signals)
  - Receive and transmit enabled
  */
  USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* Enable GPIO clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA , ENABLE);
  /* Enable USART clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);//RX
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);//TX
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_USART1);//CTS
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_USART1);//RTS
  
  /* Configure USART Tx and Rx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  USART_InitStructure.USART_BaudRate = uiBaudrate;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_RTS_CTS;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART1, &USART_InitStructure);
  
  USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);  
  /* NVIC configuration */
  /* Configure the Priority Group */
  //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
  /* Enable the USARTx Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* Enable USART */
  USART_Cmd(USART1, ENABLE);
  
  
  /*GPIO Initialization Structure variable*/
  GPIO_InitTypeDef  stGPIO_InitStructure; 
  /* GPIO Peripheral clock enable */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
  
  stGPIO_InitStructure.GPIO_Pin 	= WIFI_RESET_PIN;
  stGPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  stGPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_OUT;
  stGPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_NOPULL;
  stGPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(WIFI_RESET_PORT, &stGPIO_InitStructure);
  GPIO_SetBits(WIFI_RESET_PORT, WIFI_RESET_PIN);
  
  /* GPIO Peripheral clock enable */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
  
  stGPIO_InitStructure.GPIO_Pin 	= WIFI_FORCE_ON_PIN;
  stGPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  stGPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_OUT;
  stGPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_NOPULL;
  stGPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(WIFI_FORCE_ON_PORT, &stGPIO_InitStructure);
  GPIO_SetBits(WIFI_FORCE_ON_PORT, WIFI_FORCE_ON_PIN);
  
}

/*****************************************************************************
**@Function 		  : 	fnWifiDefaultConfig
**@Descriptions	: 	Configure Wifi for AP Mode.
**@parameters		: 	None
**@return				: 	None
*****************************************************************************/
void fnWifiDefaultConfig(void)
{
  
  char rgcTempBuff[100];
  /*Check WIFI current Baudrate and init USART wit that baudrate*/
  fnWifiInit(fnCheckWifiBaud());
  /*Enter Wifi in Command Mode*/
  fnWifiCommandMode();
  
  fnWifiCommand("factory RESET");
  
  fnWifiCommand("set wlan fmon 0");
  
  fnWifiCommand("set wlan linkmon 0");
  
  fnWifiCommand("set uart flow 1");
  
  //fnWifiCommand("set comm match #");
  
  fnWifiCommand("set ap link 0");
  /*Set wifi in HOST mode*/
  fnWifiCommand("set wlan join 7");
  /*Set wifi channel 0*/
  fnWifiCommand("set wlan channel 0 ");
  /*Set wifi SSID*/
  memset(rgcTempBuff,'\0',100);
  sprintf(rgcTempBuff,"set apmode ssid %s",WIFI_SSID);
  fnWifiCommand((uint8_t*)rgcTempBuff);
  /*Set wifi Passord*/
  memset(rgcTempBuff,'\0',100);
  sprintf(rgcTempBuff,"set apmode passphrase %s",WIFI_PASSWORD);
  fnWifiCommand((uint8_t*)rgcTempBuff);
  /*Set wifi in DHCP mode*/
  fnWifiCommand("set ip dhcp 4");
  /*Set wifi HOST IP Address*/
  memset(rgcTempBuff,'\0',100);
  sprintf(rgcTempBuff,"set ip addr %s",WIFI_IP_ADDRESS);
  fnWifiCommand((uint8_t*)rgcTempBuff);
  /*Set wifi HOST Subnet mask Address*/
  memset(rgcTempBuff,'\0',100);
  sprintf(rgcTempBuff,"set ip net %s",WIFI_SUBNETMASK);
  fnWifiCommand((uint8_t*)rgcTempBuff);
  /*Set wifi HOST Gateway Address*/
  memset(rgcTempBuff,'\0',100);
  sprintf(rgcTempBuff,"set ip gateway %s",WIFI_GATEWAY);
  fnWifiCommand((uint8_t*)rgcTempBuff);
  /*Set wifi HOST Port number*/
  memset(rgcTempBuff,'\0',100);
  sprintf(rgcTempBuff,"set ip localport %d",WIFI_PORT);
  fnWifiCommand((uint8_t*)rgcTempBuff);
  /*Set wifi Baudrate to 115200*/
  fnWifiCommand("set uart baudrate 115200");
  /*Save wifi configuration in wifi memory*/
  fnWifiCommand("save");
  /*Reboot Wifi*/
  fnWifiCommand("reboot");
  /*Change usart Baudrate to 115200*/
  fnDelay_ms(100);
  fnWifiInit(115200);
#ifdef DBG_WIFI
  printf("\n\rWifi Init Complete");
#endif
  
  /*WIFI PARAMETERS change18*/
  strcpy((char *)stWifiIP.rgucIP_Addr,WIFI_IP_ADDRESS);
  strcpy((char *)stWifiIP.rgucSubnet_Addr,WIFI_SUBNETMASK);
  strcpy((char *)stWifiIP.rgucGateway_Addr,WIFI_GATEWAY);
  strcpy((char *)stWifiIP.rgucSSID,WIFI_SSID);
  strcpy((char *)stWifiIP.rgucPASWD,WIFI_PASSWORD);			
  stWifiIP.uiPort = WIFI_PORT;
  
  fnSPI_EEPROM_WriteData(SCU_WIFI_IPADDRESS, 15, stWifiIP.rgucIP_Addr);
  fnSPI_EEPROM_WriteData(SCU_WIFI_SMADDRESS, 15, stWifiIP.rgucSubnet_Addr);
  fnSPI_EEPROM_WriteData(SCU_WIFI_GWADDRESS, 15, stWifiIP.rgucGateway_Addr);
  fnSPI_EEPROM_WriteData(SCU_WIFI_SSIDADDRESS, 15,stWifiIP.rgucSSID);
  fnSPI_EEPROM_WriteData(SCU_WIFI_PASSADDRESS, 15,stWifiIP.rgucPASWD);
  SplitByte.uiData = stWifiIP.uiPort;
  fnSPI_EEPROM_WriteData(SCU_WIFI_PORTADDRESS, 2, SplitByte.rgucData);	
}


/*****************************************************************************
**@Function 		  : 	fnCheckWifiBaud
**@Descriptions	: 	Check Wifi Baudrate.
**@parameters		: 	None
**@return				: 	uiBaudrate : Baudrate
*****************************************************************************/
uint32_t fnCheckWifiBaud(void)
{
  /*Empty Wifi Receiving Buffer*/
  memset(stWifi.rgucReceiveBuff,'\0',RECEIVE_BUFFER);
  /*Move Received Buffer pointer to Zero position*/
  stWifi.uiReceivePtr = 0;
#ifdef DBG_WIFI
  printf("\n\rRESET");
#endif
  /*Set WIFI USART baud rate to 9600*/
  fnWifiInit(9600);
  fnDelay_ms(100);
  /*Send Reset pulse to Wifi*/
  GPIO_ResetBits(WIFI_RESET_PORT, WIFI_RESET_PIN);
  fnDelay_ms(100);
  GPIO_SetBits(WIFI_RESET_PORT, WIFI_RESET_PIN);
  /*Reset and Start Response Timer*/
  uiResponseTime=0;
  /*Wait for the valid response till timeout*/
  while(uiResponseTime <= RESPONSE_TIMEOUT_PERIOD)
  {
    /*On reset wifi send ready string*/
    if(strstr((char *)stWifi.rgucReceiveBuff,"READY") != NULL)
    {
#ifdef DBG_WIFI
      printf("\n\rBaud 9600");
#endif
      return 9600;
    }
  }
  /*Empty Wifi Receiving Buffer*/
  memset(stWifi.rgucReceiveBuff,'\0',RECEIVE_BUFFER);
  /*Move Received Buffer pointer to Zero position*/
  stWifi.uiReceivePtr = 0;
#ifdef DBG_WIFI
  printf("\n\rRESET");
#endif
  /*Set WIFI USART baud rate to 115200*/
  fnWifiInit(115200);
  fnDelay_ms(100);
  /*Send Reset pulse to Wifi*/
  GPIO_ResetBits(WIFI_RESET_PORT, WIFI_RESET_PIN);
  fnDelay_ms(100);
  GPIO_SetBits(WIFI_RESET_PORT, WIFI_RESET_PIN);
  /*Reset and Start Response Timer*/
  uiResponseTime=0;
  /*Wait for the valid response till timeout*/
  while(uiResponseTime <= RESPONSE_TIMEOUT_PERIOD)
  {
    /*On reset wifi send ready string*/
    if(strstr((char *)stWifi.rgucReceiveBuff,"READY") != NULL)
    {
#ifdef DBG_WIFI
      printf("\n\rBaud 115200");
#endif
      return 115200;
    }
  }
#ifdef DBG_WIFI
  printf("\n\rBaud 0");
#endif
  return 0;
}

/*****************************************************************************
**@Function 		  : 	fnWifiProcessData
**@Descriptions	: 	Process Received Data through Wifi
**@parameters		: 	None
**@return				: 	None
*****************************************************************************/ 
void fnWifiProcessData(void) 
{
  /*Process Received Buffer*/
  if(SET == stWifi.ucReceivedFlag)
  {
#ifdef DBG_WIFI
		static uint32_t count=0;
    printf("\n\rWifi Rx %d : %s",count++,stWifi.rgucReceiveBuff);
#endif
    stWifi.ucReceivedFlag = RESET;
    /*Check for valid start and stop string*/
    if(strstr((char *)stWifi.rgucReceiveBuff,"</CRC-Evans_Data>"))
    {
      if(strstr((char *)stWifi.rgucReceiveBuff,"<CRC-Evans_Data"))
      {
        /*Set Configuration if received Command*/
#ifdef DBG_WIFI
          printf("\n\rSet_Config_Parameters ");
#endif
          fnSetConfigParameter(stWifi.rgucReceiveBuff);
      }
			/*Empty Wifi Receiving Buffer*/
			memset(stWifi.rgucReceiveBuff,'\0',RECEIVE_BUFFER);
			/*Move Received Buffer pointer to Zero position*/
			stWifi.uiReceivePtr = 0;
    }
    /*Check for valid start and stop string*/
    if(strstr((char *)stWifi.rgucReceiveBuff,"</CRC-Evans_MCL>"))
    {
      if(strstr((char *)stWifi.rgucReceiveBuff,"<CRC-Evans_MCL>"))
      {
        /*Send Datalog if received Command*/
        if(strstr((char *)stWifi.rgucReceiveBuff,"Command"))
        {
#ifdef DBG_WIFI
          printf("\n\rWifi Data Logging");
#endif
          fnDataLogging(stWifi.rgucReceiveBuff,&stWifiLog);
        }
        /*send data for XML processing*/
        else if(strstr((char *)stWifi.rgucReceiveBuff,"Ext_ID"))
        {
#ifdef DBG_WIFI
          printf("\n\rXML process");
#endif
          fnXMLCmdProcess(stWifi.rgucReceiveBuff);
        }
				/*Empty Wifi Receiving Buffer*/
				memset(stWifi.rgucReceiveBuff,'\0',RECEIVE_BUFFER);
				/*Move Received Buffer pointer to Zero position*/
				stWifi.uiReceivePtr = 0;
      }
    }
		stWifi.uiReceiveTimer = RESET;
	}
	if(((WIFIHEARTBEAT * 100) <= stWifi.uiReceiveTimer) && (SET == stWifi.ucConnectedStatus))
	{
#ifdef DBG_WIFI
		printf("\n\rWifi Reset %d",stWifi.uiReceiveTimer);
#endif
		stWifi.ucConnectedStatus = RESET;
		stWifi.uiReceiveTimer = RESET;
		/*Empty Wifi Receiving Buffer*/
		memset(stWifi.rgucReceiveBuff,'\0',RECEIVE_BUFFER);
		/*Move Received Buffer pointer to Zero position*/
		stWifi.uiReceivePtr = 0;
		/*Send Reset pulse to Wifi*/
		GPIO_ResetBits(WIFI_RESET_PORT, WIFI_RESET_PIN);
		fnDelay_ms(100);
		GPIO_SetBits(WIFI_RESET_PORT, WIFI_RESET_PIN);
		/*Enter Wifi in Command Mode*/
		fnWifiCommandMode();
		/*Reboot Wifi*/
		fnWifiCommand("reboot");
	}
}

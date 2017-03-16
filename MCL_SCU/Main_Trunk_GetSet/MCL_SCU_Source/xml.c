/******************************************************************************
**@file     :  XML.c
* 
**@brief    :  This File contains the functions  for formatting XML.
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
uint8_t rgucXMLtxBuffer[TRANSMIT_BUFFER];
IPconfig_t stWifiIP;
IPconfig_t stWifiRxIP;
IPconfig_t stEthernetIP;
uint8_t ucConfigDataCount=0;
char rgcFiledata[100];
XML_TX_t stXMLtx;
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
**@Function 		  : 	fnWriteStartElement
**@Descriptions	: 	Formatting start element frame for XML format.
**@parameters		: 	pucElement : Start element
**@return				: 	None
*****************************************************************************/
void fnWriteStartElement(uint8_t *pucElement)
{
  uint8_t rgucTempBuff[200];
  memset(rgucTempBuff,'\0',200);
  sprintf((char *)rgucTempBuff,"<%s>",(char*)pucElement);
  strcat((char *)rgucXMLtxBuffer,(char *)rgucTempBuff);
  
}

/*****************************************************************************
**@Function 		  : 	fnWriteEndElement
**@Descriptions	: 	Formatting end element frame for XML format.
**@parameters		: 	pucElement : end element
**@return				: 	None
*****************************************************************************/
void fnWriteEndElement(uint8_t *pucElement)
{
  uint8_t rgucTempBuff[200];
  memset(rgucTempBuff,'\0',200);
  sprintf((char *)rgucTempBuff,"</%s>",(char *)pucElement);
  strcat((char *)rgucXMLtxBuffer,(char *)rgucTempBuff);
  
}

/*****************************************************************************
**@Function 		  : 	fnWriteData
**@Descriptions	: 	Formatting string between element frame for XML format.
**@parameters		: 	pucString : 8 byte data string
**@return				: 	None
*****************************************************************************/
void fnWriteData(uint8_t *pucString)
{
  uint8_t rgucTempBuff[200]={'\0'};
  memset(rgucTempBuff,'\0',200);
  sprintf((char *)rgucTempBuff,"%s",pucString);
  strcat((char *)rgucXMLtxBuffer,(char *)rgucTempBuff);
  
}
/*****************************************************************************
**@Function 		  : 	fnWriteMessageID
**@Descriptions	: 	Formatting 32bit data between element frame for XML format.
**@parameters		: 	uliMessageID : 32bit Integer
**@return				: 	None
*****************************************************************************/
void fnWriteMessageID(uint32_t uliMessageID)
{
  uint8_t rgucTempBuff[10];
  memset(rgucTempBuff,'\0',10);
  sprintf((char *)rgucTempBuff,"%08x",uliMessageID);
  strcat((char *)rgucXMLtxBuffer,(char *)rgucTempBuff);
}
/*****************************************************************************
**@Function 		  : 	fnWriteLength
**@Descriptions	: 	Formatting 8bit data between element frame for XML format.
**@parameters		: 	ucDLC : 8bit Integer
**@return				: 	None
*****************************************************************************/
void fnWriteLength(uint8_t ucLength)
{
  uint8_t rgucTempBuff[10];
  memset(rgucTempBuff,'\0',10);
  sprintf((char *)rgucTempBuff,"%02x",ucLength);
  strcat((char *)rgucXMLtxBuffer,(char *)rgucTempBuff);
}
/*****************************************************************************
**@Function 		  : 	fnGetDataElement
**@Descriptions	: 	Formatting start element frame for XML format.
**@parameters		: 	pucElement : Start element
**@return				: 	None
*****************************************************************************/
char* fnGetDataElement(const uint8_t* XMLstring,const char* InternalName)
{
  char *pTemp1,*pTemp2,cLoop=0;
  char rgucStartElement[50],rgucEndElement[50];
  memset(rgucStartElement,'\0',50);
  sprintf((char *)rgucStartElement,"<%s>",(char *)InternalName);
  memset(rgucEndElement,'\0',50);
  sprintf((char *)rgucEndElement,"</%s>",(char *)InternalName);
  
  if((pTemp1 = strstr((char*)XMLstring,rgucStartElement)) != NULL)
  {
    if((pTemp2 = strstr(pTemp1,rgucEndElement)) != NULL)
    {
      //memset(rgcFiledata,'\0',strlen(rgcFiledata));
			for(cLoop=0;cLoop<100;cLoop++)
				rgcFiledata[cLoop]='\0';
      strncpy(rgcFiledata,(pTemp1+strlen(rgucStartElement)),(pTemp2-(pTemp1+strlen(rgucStartElement))));//change rgucStartElement
			rgcFiledata[(pTemp2-(pTemp1+strlen(rgucStartElement)))] = 0x00;
      if(rgcFiledata[0] == NULL)
        return NULL;
      //printf("\n\rData %s %d\n\r",rgcFiledata,(pTemp2-(pTemp1+strlen(rgucStartElement))));
      return rgcFiledata;
    }
  }
  return NULL;
}
/*****************************************************************************
**@Function      : fnXMLCmdAck
**@Descriptions  : Acknowledge the received XML frame
**@parameters    : pstCANrxmsg: Pointer to CAN Rx Struct
**@Return        : None
*****************************************************************************/
void fnXMLCmdAck(CanRxMsg *pstCANrxmsg)
{
  CanTxMsg stTxMsgACK;
  uint8_t rgucTempBuff[500]={'\0'};
#ifdef DBG_XML
  printf("XML Ack Message ID= 0x%x\r\n",pstCANrxmsg->ExtId);
#endif
  SplitByte.uliData = pstCANrxmsg->ExtId;
  
  /* Reset Data Bufer */
  memset(stTxMsgACK.Data,'\0', strlen((char *)stTxMsgACK.Data));
  
  /*Update Length of CAN Data Frame*/
  stTxMsgACK.DLC = 1;
  
  stTxMsgACK.Data[0] = 0x00;		/* Positive ACk */
  
  SplitByte.rgucData[1] = SplitByte.rgucData[0];
  SplitByte.rgucData[0] = SOURCE_SCU;
  SplitByte.rgucData[2] = 0xE8;
  SplitByte.rgucData[3] = 0x14;
  stTxMsgACK.ExtId = SplitByte.uliData;
  
  
  memset(rgucTempBuff,'\0',500);
  sprintf((char *)rgucTempBuff,"%02x%02x%02x%02x%02x%02x%02x%02x",stTxMsgACK.Data[7],
          stTxMsgACK.Data[6],stTxMsgACK.Data[5],stTxMsgACK.Data[4],stTxMsgACK.Data[3],
          stTxMsgACK.Data[2],stTxMsgACK.Data[1],stTxMsgACK.Data[0]);
  
  memset(rgucXMLtxBuffer,'\0',TRANSMIT_BUFFER);
  fnWriteStartElement("CRC-Evans_MCL");
  fnWriteStartElement("Ext_ID");
  fnWriteMessageID(stTxMsgACK.ExtId);
  fnWriteEndElement("Ext_ID");
  fnWriteStartElement("DLC");
  fnWriteLength(stTxMsgACK.DLC);
  fnWriteEndElement("DLC");
  fnWriteStartElement("Data");
  fnWriteData(rgucTempBuff);
  fnWriteEndElement("Data");
  fnWriteEndElement("CRC-Evans_MCL");
  
  /*Transmitt CAN message in XML format over Wifi*/
  fnWifiSendData(rgucXMLtxBuffer);
  //printf("\n\r ACK %s",rgucXMLtxBuffer);
}
/*****************************************************************************
**@Function      : fnXMLCmdNack
**@Descriptions  : N Acknowledge the received XML frame
**@parameters    : pstCANrxmsg: Pointer to CAN Rx Struct
**@Return        : None
*****************************************************************************/
void fnXMLCmdNack(CanRxMsg *pstCANrxmsg)
{
  CanTxMsg stTxMsgACK;
  uint8_t rgucTempBuff[500]={'\0'};
#ifdef DBG_XML
  printf("XML Ack Message ID= 0x%x\r\n",pstCANrxmsg->ExtId);
#endif
  SplitByte.uliData = pstCANrxmsg->ExtId;
  
  /* Reset Data Bufer */
  memset(stTxMsgACK.Data,'\0', strlen((char *)stTxMsgACK.Data));
  
  /*Update Length of CAN Data Frame*/
  stTxMsgACK.DLC = 1;
  
  stTxMsgACK.Data[0] = 0x00;		/* Positive ACk */
  
  SplitByte.rgucData[1] = SplitByte.rgucData[0];
  SplitByte.rgucData[0] = SOURCE_SCU;
  SplitByte.rgucData[2] = 0xEE;
  SplitByte.rgucData[3] = 0x14;
  stTxMsgACK.ExtId = SplitByte.uliData;
  
  
  memset(rgucTempBuff,'\0',500);
  sprintf((char *)rgucTempBuff,"%02x%02x%02x%02x%02x%02x%02x%02x",stTxMsgACK.Data[7],
          stTxMsgACK.Data[6],stTxMsgACK.Data[5],stTxMsgACK.Data[4],stTxMsgACK.Data[3],
          stTxMsgACK.Data[2],stTxMsgACK.Data[1],stTxMsgACK.Data[0]);
  
  memset(rgucXMLtxBuffer,'\0',TRANSMIT_BUFFER);
  fnWriteStartElement("CRC-Evans_MCL");
  fnWriteStartElement("Ext_ID");
  fnWriteMessageID(stTxMsgACK.ExtId);
  fnWriteEndElement("Ext_ID");
  fnWriteStartElement("DLC");
  fnWriteLength(stTxMsgACK.DLC);
  fnWriteEndElement("DLC");
  fnWriteStartElement("Data");
  fnWriteData(rgucTempBuff);
  fnWriteEndElement("Data");
  fnWriteEndElement("CRC-Evans_MCL");
  
  /*Transmitt CAN message in XML format over Wifi*/
  fnWifiSendData(rgucXMLtxBuffer);
}
/*****************************************************************************
**@Function 		  : 	fnXMLCmdProcess
**@Descriptions	: 	Formatting string for XML format.
**@parameters		: 	None
**@return				: 	None
*****************************************************************************/
void fnXMLCmdProcess(uint8_t *pucString)
{
  uint8_t *temp ,*token, ucFrameCheck=0;
  char ucData[2];
  uint8_t msg[RECEIVE_BUFFER];
  XML_t stXML;
  CanRxMsg stTempCanRx;
#ifdef DBG_XML
 // printf("\n\rProcess %s",pucString);
#endif
  
  /* Get the Id */
  strcpy((char *)msg,(char *)pucString);
  if((temp = (uint8_t *)strstr((char *)msg,"<Ext_ID>")) != NULL)
  {
	token = (uint8_t *)strtok((char *)temp+strlen("<Ext_ID>"),"</");
	stXML.ExtId=strtol((char *)token,NULL,16);
    ucFrameCheck++;
  }
  
  /* Get the DLC */
  strcpy((char *)msg,(char *)pucString);
  if((temp = (uint8_t *)strstr((char *)msg,"<DLC>")) != NULL)
  {
	token = (uint8_t *)strtok((char *)temp+strlen("<DLC>"),"</");
	stXML.DLC=atoi((char *)token);
    ucFrameCheck++;
  }
  
  /* Get the data field */
  strcpy((char *)msg,(char *)pucString);
  if((temp = (uint8_t *)strstr((char *)msg,"<Data>")) != NULL)
  {
	token = (uint8_t *)strtok((char *)temp+strlen("<Data>"),"</");
	
	ucData[0]=token[0];
	ucData[1]=token[1];
	stXML.rgucData[7]=(uint8_t)strtol(ucData,NULL,16);
	
	ucData[0]=token[2];
	ucData[1]=token[3];
	stXML.rgucData[6]=(uint8_t)strtol(ucData,NULL,16);
	
	ucData[0]=token[4];
	ucData[1]=token[5];
	stXML.rgucData[5]=(uint8_t)strtol(ucData,NULL,16);
	
	ucData[0]=token[6];
	ucData[1]=token[7];
	stXML.rgucData[4]=(uint8_t)strtol(ucData,NULL,16);
	
	ucData[0]=token[8];
	ucData[1]=token[9];
	stXML.rgucData[3]=(uint8_t)strtol(ucData,NULL,16);
	
	ucData[0]=token[10];
	ucData[1]=token[11];
	stXML.rgucData[2]=(uint8_t)strtol(ucData,NULL,16);
	
	ucData[0]=token[12];
	ucData[1]=token[13];
	stXML.rgucData[1]=(uint8_t)strtol(ucData,NULL,16);
	
	ucData[0]=token[14];
	ucData[1]=token[15];
	stXML.rgucData[0]=(uint8_t)strtol(ucData,NULL,16);
	ucFrameCheck++;
  }
#ifdef DBG_XML      
 // printf("uliCanID = %x \r\n",stXML.ExtId);
#endif	
  if((stXML.ExtId & DEST_ADDR_MASK) != DEST_ADDR_MASK)
  {
    
	/************MESSAGE TYPE*************/
	switch (stXML.ExtId & MSG_MASK) 
	{
      /************BROADCAST MESSAGE*************/
	case ACKNOWLEDGEMENT:
      {
#ifdef DBG_XML
        printf("\n\rACK XML RECEIVED");
#endif 
        //			stWifi.ucSendFlag = RESET;
        //			if(stXMLtx.ucSentIndex < QUEUE)
        //			{
        //				memset(&stXMLtx.stCanTx_XML[stXMLtx.ucSentIndex],'\0',sizeof(CanTxMsg));
        //				stXMLtx.ucSentIndex++;
        //			}
        //			else
        //			{
        //				stXMLtx.ucSentIndex = RESET;
        //			}
      }
      break;
	default:
      {
        if(3 == ucFrameCheck)
        {
          stTempCanRx.ExtId = (uint32_t)0x1FFFFFFF & stXML.ExtId;
          
          stTempCanRx.DLC = (uint8_t)0x0F & stXML.DLC;
          
          stTempCanRx.Data[7] = (uint8_t)0xFF & stXML.rgucData[7];
          
          stTempCanRx.Data[6] = (uint8_t)0xFF & stXML.rgucData[6];
          
          stTempCanRx.Data[5] = (uint8_t)0xFF & stXML.rgucData[5];
          
          stTempCanRx.Data[4] = (uint8_t)0xFF & stXML.rgucData[4];
          
          stTempCanRx.Data[3] = (uint8_t)0xFF & stXML.rgucData[3];
          
          stTempCanRx.Data[2] = (uint8_t)0xFF & stXML.rgucData[2];
          
          stTempCanRx.Data[1] = (uint8_t)0xFF & stXML.rgucData[1];
          
          stTempCanRx.Data[0] = (uint8_t)0xFF & stXML.rgucData[0];
          
          memcpy(&rgstRxCANBuff[uiStoreRxCANBuffCnt],&stTempCanRx,sizeof(CanRxMsg));
          uiStoreRxCANBuffCnt++;
          if(uiStoreRxCANBuffCnt >= RX_CAN_BUF_SIZE)
          {
            uiStoreRxCANBuffCnt = 0;
            StatusFlag.stStatusFlag.bDataInCANRx_BuffFlag = SET;
          }	
          //			#ifdef DBG_XML
          //			printf("\n\rID %x",rgstRxCANBuff[uiStoreRxCANBuffCnt-1].ExtId);
          //			printf("\n\rID %x",(uint32_t)0x1FFFFFFF & stXML.ExtId);
          //			printf("\n\rDLC %d",rgstRxCANBuff[uiStoreRxCANBuffCnt-1].DLC);
          //			printf("\n\rDLC %d",(uint8_t)0x0F & stXML.DLC);
          //			printf("\n\rData %s",((char *)rgstRxCANBuff[uiStoreRxCANBuffCnt-1].Data));
          //			printf("\n\rData %s",((char *)stXML.rgucData));
          //			#endif
          fnXMLCmdAck(&stTempCanRx);//Acknowledge the received frame			
		}
        else
        {
          //Nacknowledge the received frame
          fnXMLCmdNack(&stTempCanRx);
        }
      }
      break;
	}
  }
}
/*****************************************************************************
**@Function      : fnXML_FIFO
**@Descriptions  : Tx msg is stored in FIFO and intiates frame transmission
**@parameters    : pstCANTxmsg: Pointer to CAN Tx Struct
**@Return        : None
*****************************************************************************/
void fnXML_FIFO(CanTxMsg *pstCANTxmsg)
{
  char rgucTempBuff[200]={'\0'};
  //uint8_t ucDLC=pstCANTxmsg->DLC;
  memset(rgucTempBuff,'\0',200);
  /************MESSAGE TYPE*************/
  switch (pstCANTxmsg->ExtId & TYPE_MASK) 
  {
    /************BROADCAST MESSAGE*************/
  case MSG_TYPE_STATUS:
  case MSG_TYPE_FAULT:
    {
     if((RESET == stWifi.ucConnectedStatus) || (SET == stWifiLog.ucSendFlag) || (SET == stWifi.ucBusyFlag))
      {
		
        uint8_t ucLoop;
        if(stXMLtx.ucStoreIndex < QUEUE)
        {
          memcpy(&stXMLtx.stCanTx_XML[stXMLtx.ucStoreIndex], pstCANTxmsg, sizeof(CanTxMsg));
          
          stXMLtx.ucTimeStamp[stXMLtx.ucStoreIndex][0] = stConfigData.ucConfig_SystemTime[0];
          stXMLtx.ucTimeStamp[stXMLtx.ucStoreIndex][1] = stConfigData.ucConfig_SystemTime[1];
          stXMLtx.ucTimeStamp[stXMLtx.ucStoreIndex][2] = stConfigData.ucConfig_SystemTime[2];
          
          stXMLtx.ucTimeStamp[stXMLtx.ucStoreIndex][3] = stConfigData.ucConfig_SystemTime[3];
          stXMLtx.ucTimeStamp[stXMLtx.ucStoreIndex][4] = stConfigData.ucConfig_SystemTime[4];
          stXMLtx.ucTimeStamp[stXMLtx.ucStoreIndex][5] = stConfigData.ucConfig_SystemTime[5];
          stXMLtx.ucTimeStamp[stXMLtx.ucStoreIndex][6] = stConfigData.ucConfig_SystemTime[6];
          
#ifdef DBG_XML
         // printf("Index %d\n\r",stXMLtx.ucStoreIndex);
#endif
          stXMLtx.ucStoreIndex++;//increment storing index
        }
        else
        {
          stXMLtx.ucStoreIndex = RESET;//clear storing index
          for(ucLoop=0;ucLoop < QUEUE;ucLoop++)
          {
            memset(&stXMLtx.stCanTx_XML[ucLoop],'\0',sizeof(CanTxMsg));
            memset(&stXMLtx.ucTimeStamp[ucLoop],'\0',7);
          }
          memcpy(&stXMLtx.stCanTx_XML[stXMLtx.ucStoreIndex], pstCANTxmsg, sizeof(CanTxMsg));
          stXMLtx.ucTimeStamp[stXMLtx.ucStoreIndex][0] = stConfigData.ucConfig_SystemTime[0];
          stXMLtx.ucTimeStamp[stXMLtx.ucStoreIndex][1] = stConfigData.ucConfig_SystemTime[1];
          stXMLtx.ucTimeStamp[stXMLtx.ucStoreIndex][2] = stConfigData.ucConfig_SystemTime[2];
          
          stXMLtx.ucTimeStamp[stXMLtx.ucStoreIndex][3] = stConfigData.ucConfig_SystemTime[3];
          stXMLtx.ucTimeStamp[stXMLtx.ucStoreIndex][4] = stConfigData.ucConfig_SystemTime[4];
          stXMLtx.ucTimeStamp[stXMLtx.ucStoreIndex][5] = stConfigData.ucConfig_SystemTime[5];
          stXMLtx.ucTimeStamp[stXMLtx.ucStoreIndex][6] = stConfigData.ucConfig_SystemTime[6];
#ifdef DBG_XML
          //printf("Index %d\n\r",stXMLtx.ucStoreIndex);
#endif
          stXMLtx.ucStoreIndex++;//increment storing index
        }
		
      }
      else if((SET == stWifi.ucConnectedStatus) && (RESET == stWifiLog.ucSendFlag) && (RESET == stWifi.ucBusyFlag))
      {
#ifdef DBG_XML
      //  printf("\n\rELSE");
#endif
        //						char rgucTempBuff[500]={'\0'};
        //		//uint8_t ucDLC=pstCANTxmsg->DLC;
        //		memset(rgucTempBuff,'\0',strlen((char *)rgucTempBuff));
		sprintf((char *)rgucTempBuff,"%02x%02x%02x%02x%02x%02x%02x%02x",pstCANTxmsg->Data[7],
                pstCANTxmsg->Data[6],pstCANTxmsg->Data[5],pstCANTxmsg->Data[4],pstCANTxmsg->Data[3],
                pstCANTxmsg->Data[2],pstCANTxmsg->Data[1],pstCANTxmsg->Data[0]);
		
		memset(rgucXMLtxBuffer,'\0',TRANSMIT_BUFFER);
		fnWriteStartElement("CRC-Evans_MCL");
		fnWriteStartElement("Ext_ID");
		fnWriteMessageID(pstCANTxmsg->ExtId);
		fnWriteEndElement("Ext_ID");
		fnWriteStartElement("DLC");
		fnWriteLength(pstCANTxmsg->DLC);
		fnWriteEndElement("DLC");
		fnWriteStartElement("Data");			
		fnWriteData((uint8_t *)rgucTempBuff);
		fnWriteEndElement("Data");
		fnWriteStartElement("Timestamp");
		memset(rgucTempBuff,'\0',200);
		sprintf((char *)rgucTempBuff,"%02d/%02d/20%02d %02d:%02d:%02d",
                stConfigData.ucConfig_SystemTime[3],	stConfigData.ucConfig_SystemTime[4],
                stConfigData.ucConfig_SystemTime[6], stConfigData.ucConfig_SystemTime[0],
                stConfigData.ucConfig_SystemTime[1],	stConfigData.ucConfig_SystemTime[2]);
		fnWriteData((uint8_t *)rgucTempBuff);
		fnWriteEndElement("Timestamp");
		fnWriteEndElement("CRC-Evans_MCL");
		
		if(/*(SET == stWifi.ucSendFlag) &&*/ (RESET == stWifiLog.ucSendFlag))
          fnWifiSendData(rgucXMLtxBuffer);
		//memset(pstCANTxmsg,'\0',sizeof(CanTxMsg));
		//	if((SET == stEthernet.ucSendFlag) && (RESET == stEthernetLog.ucSendFlag))
		//		fnEthernetSend(rgucXMLtxBuffer);
		//	stWifi.ucSendFlag = RESET;
		//	stEthernet.ucSendFlag = RESET;
#ifdef DBG_XML
        //printf("\n\r111111");
#endif
      }
      
	}
    break;
  default:
    {
      //						char rgucTempBuff[500]={'\0'};
      //		uint8_t ucDLC=pstCANTxmsg->DLC;
      //		memset(rgucTempBuff,'\0',strlen((char *)rgucTempBuff));
      sprintf((char *)rgucTempBuff,"%02x%02x%02x%02x%02x%02x%02x%02x",pstCANTxmsg->Data[7],
              pstCANTxmsg->Data[6],pstCANTxmsg->Data[5],pstCANTxmsg->Data[4],pstCANTxmsg->Data[3],
              pstCANTxmsg->Data[2],pstCANTxmsg->Data[1],pstCANTxmsg->Data[0]);
      
      memset(rgucXMLtxBuffer,'\0',TRANSMIT_BUFFER);
      fnWriteStartElement("CRC-Evans_MCL");
      fnWriteStartElement("Ext_ID");
      fnWriteMessageID(pstCANTxmsg->ExtId);
      fnWriteEndElement("Ext_ID");
      fnWriteStartElement("DLC");
      fnWriteLength(pstCANTxmsg->DLC);
      fnWriteEndElement("DLC");
      fnWriteStartElement("Data");			
      fnWriteData((uint8_t *)rgucTempBuff);
      fnWriteEndElement("Data");
      fnWriteEndElement("CRC-Evans_MCL");
      if(/*(SET == stWifi.ucSendFlag) &&*/ (RESET == stWifiLog.ucSendFlag))
        fnWifiSendData(rgucXMLtxBuffer);
      //memset(pstCANTxmsg,'\0',sizeof(CanTxMsg));
      //	if((SET == stEthernet.ucSendFlag) && (RESET == stEthernetLog.ucSendFlag))
      //		fnEthernetSend(rgucXMLtxBuffer);
      //	stWifi.ucSendFlag = RESET;
      //	stEthernet.ucSendFlag = RESET;
    }
    break;
  }
  
  memset(pstCANTxmsg,'\0',sizeof(CanTxMsg));
}

/*****************************************************************************
**@Function      : fnXML_DataTransmit
**@Descriptions  : Transmit Frame from FIFO.
**@parameters    : None
**@Return        : None
*****************************************************************************/
void fnXML_DataTransmit(void)
{
  if(/*(RESET == stWifi.ucSendFlag) &&*/ (RESET == stWifiLog.ucSendFlag) && (SET == stWifi.ucConnectedStatus))
  {
    if(stXMLtx.ucStoreIndex > 0)//search if ther any string in the buffer
    {
      //printf("DATA WIFI SENDING\n\r");
      uint8_t rgucTempBuff[200]={'\0'};
      //uint8_t ucDLC=stXMLtx.stCanTx_XML[stXMLtx.ucSentIndex].DLC;
      memset(rgucTempBuff,'\0',200);
      sprintf((char *)rgucTempBuff,"%02x%02x%02x%02x%02x%02x%02x%02x",stXMLtx.stCanTx_XML[stXMLtx.ucSentIndex].Data[7],
              stXMLtx.stCanTx_XML[stXMLtx.ucSentIndex].Data[6],stXMLtx.stCanTx_XML[stXMLtx.ucSentIndex].Data[5],stXMLtx.stCanTx_XML[stXMLtx.ucSentIndex].Data[4],stXMLtx.stCanTx_XML[stXMLtx.ucSentIndex].Data[3],
              stXMLtx.stCanTx_XML[stXMLtx.ucSentIndex].Data[2],stXMLtx.stCanTx_XML[stXMLtx.ucSentIndex].Data[1],stXMLtx.stCanTx_XML[stXMLtx.ucSentIndex].Data[0]);
      
      memset(rgucXMLtxBuffer,'\0',TRANSMIT_BUFFER);
      fnWriteStartElement("CRC-Evans_MCL");
      fnWriteStartElement("Ext_ID");
      fnWriteMessageID(stXMLtx.stCanTx_XML[stXMLtx.ucSentIndex].ExtId);
      fnWriteEndElement("Ext_ID");
      fnWriteStartElement("DLC");
      fnWriteLength(stXMLtx.stCanTx_XML[stXMLtx.ucSentIndex].DLC);
      fnWriteEndElement("DLC");
      fnWriteStartElement("Data");			
      fnWriteData(rgucTempBuff);
      fnWriteEndElement("Data");
      fnWriteStartElement("Timestamp");
      memset(rgucTempBuff,'\0',200);
      sprintf((char *)rgucTempBuff,"%02d/%02d/20%02d %02d:%02d:%02d",
              stXMLtx.ucTimeStamp[stXMLtx.ucSentIndex][3],	stXMLtx.ucTimeStamp[stXMLtx.ucSentIndex][4],
              stXMLtx.ucTimeStamp[stXMLtx.ucSentIndex][6], stXMLtx.ucTimeStamp[stXMLtx.ucSentIndex][0],
              stXMLtx.ucTimeStamp[stXMLtx.ucSentIndex][1],	stXMLtx.ucTimeStamp[stXMLtx.ucSentIndex][2]);
      fnWriteData((uint8_t *)rgucTempBuff);
      fnWriteEndElement("Timestamp");
      fnWriteEndElement("CRC-Evans_MCL");
      
      if(/*(SET == stWifi.ucSendFlag) &&*/ (RESET == stWifiLog.ucSendFlag))
        fnWifiSendData(rgucXMLtxBuffer);
      //  if((SET == stEthernet.ucSendFlag) && (RESET == stEthernetLog.ucSendFlag))
      //    fnEthernetSend(rgucXMLtxBuffer);
      //stWifi.ucSendFlag = SET;
      //  stEthernet.ucSendFlag = RESET;
      if(stXMLtx.ucStoreIndex > stXMLtx.ucSentIndex)
      {
        stXMLtx.ucSentIndex++;
      }
      else 
      {
        stXMLtx.ucSentIndex=0;//clear SMS sending index
        stXMLtx.ucStoreIndex=0;//clear SMS storing index
      }
    }
    //		else 
    //		{
    //			stXMLtx.ucSentIndex=0;//clear SMS sending index
    //			stXMLtx.ucStoreIndex=0;//clear SMS storing index
    //		}
  }
}
/*****************************************************************************
**@Function 		  : 	fnSetConfigParameter
**@Descriptions	: 	Formatting start element frame for XML format.
**@parameters		: 	pucElement : Start element
**@return				: 	None
*****************************************************************************/
void fnSetConfigParameter(const uint8_t* XMLstring)
{
  char *pTemp;
  uint8_t ucETH_Changes = RESET;
  
  /****************************************************************/
  /*change17*/
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"TypeOfUnits")))
  {
    if(strstr(pTemp,"Metric"))
      stConfigData.ucConfig_TypeofUnit =  1;
    else if(strstr(pTemp,"Imperial"))
      stConfigData.ucConfig_TypeofUnit = 0;
    
    ucConfigDataCount++;
   //fnSPI_EEPROM_WriteData(SCU_UNIT_TYPE, 1, &stConfigData.ucConfig_TypeofUnit);
    //printf("ucConfig_TypeofUnit = %d\r\n", stConfigData.ucConfig_TypeofUnit);
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"BaseTankHeight")))
  {
    stConfigData.fConfig_BaseTankHeight = atof(pTemp);
    //printf("\n\rstConfigData.fConfig_BaseTankHeight %f",stConfigData.fConfig_BaseTankHeight);
  //SplitByte.fData = stConfigData.fConfig_BaseTankHeight;
   //fnSPI_EEPROM_WriteData(HCM_BASE_TANK_HEIGHT, 4, SplitByte.rgucData);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"HardnerTankHeight")))
  {
    stConfigData.fConfig_HardTankHeight = atof(pTemp);
    //printf("\n\rstConfigData.fConfig_HardTankHeight %f",stConfigData.fConfig_HardTankHeight);
  //SplitByte.fData = stConfigData.fConfig_HardTankHeight;
   //fnSPI_EEPROM_WriteData(HCM_HARD_TANK_HEIGHT, 4, SplitByte.rgucData);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"HardnerLiqLowTh")))
  {
    stConfigData.fConfig_HardLiquidLevelLTH = atof(pTemp);
    //printf("\n\rstConfigData.fConfig_HardLiquidLevelLTH %f",stConfigData.fConfig_HardLiquidLevelLTH);
  //SplitByte.fData = stConfigData.fConfig_HardLiquidLevelLTH;
   //fnSPI_EEPROM_WriteData(HCM_HARD_LIQUID_THRESH_LEVEL_LOW, 4, SplitByte.rgucData);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"HardnerLiqHighTh")))
  {
    stConfigData.fConfig_HardLiquidLevelHTH = atof(pTemp);
    //printf("\n\rstConfigData.fConfig_HardLiquidLevelHTH %f",stConfigData.fConfig_HardLiquidLevelHTH);
  //SplitByte.fData = stConfigData.fConfig_HardLiquidLevelHTH;
   //fnSPI_EEPROM_WriteData(HCM_HARD_LIQUID_THRESH_LEVEL_HIGH, 4, SplitByte.rgucData);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"BaseLiqLowTh")))
  {
    stConfigData.fConfig_BaseLiquidLevelLTH = atof(pTemp);
    //printf("\n\rstConfigData.fConfig_BaseLiquidLevelLTH %f",stConfigData.fConfig_BaseLiquidLevelLTH);
  //SplitByte.fData = stConfigData.fConfig_BaseLiquidLevelLTH;
   //fnSPI_EEPROM_WriteData(HCM_BASE_LIQUID_THRESH_LEVEL_LOW, 4, SplitByte.rgucData);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"BaseLiqHighTh")))
  {
    stConfigData.fConfig_BaseLiquidLevelHTH = atof(pTemp);
    //printf("\n\rstConfigData.fConfig_BaseLiquidLevelHTH %f",stConfigData.fConfig_BaseLiquidLevelHTH);
  //SplitByte.fData = stConfigData.fConfig_BaseLiquidLevelHTH;
   //fnSPI_EEPROM_WriteData(HCM_BASE_LIQUID_THRESH_LEVEL_HIGH, 4, SplitByte.rgucData);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"LiqLvlSamplingRate")))
  {
    stConfigData.uiConfig_LLSamplingRate = atoi(pTemp);
    //printf("\n\rstConfigData.uiConfig_LLSamplingRate %d",stConfigData.uiConfig_LLSamplingRate);
  //SplitByte.iData = stConfigData.uiConfig_LLSamplingRate;
   //fnSPI_EEPROM_WriteData(HCM_LIQUID_LEVEL_SAMPLING_RATE, 2, SplitByte.rgucData);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"AntifreezLiqSET")))
  {
    stConfigData.fConfig_AntiFrzTempSet = atof(pTemp);
    //printf("\n\rstConfigData.fConfig_AntiFrzTempSet %f",stConfigData.fConfig_AntiFrzTempSet);
  //SplitByte.fData = stConfigData.fConfig_AntiFrzTempSet;
   //fnSPI_EEPROM_WriteData(HCM_ANTI_SET_TEMP, 4, SplitByte.rgucData);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"AntifreezLiqMIN")))
  {
    stConfigData.fConfig_AntiFrzTempLTH = atof(pTemp);
    //printf("\n\rstConfigData.fConfig_AntiFrzTempLTH %f",stConfigData.fConfig_AntiFrzTempLTH);
  //SplitByte.fData = stConfigData.fConfig_AntiFrzTempLTH;
   //fnSPI_EEPROM_WriteData(HCM_ANTI_THRESH_TEMP_LOW, 4, SplitByte.rgucData);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"AntifreezLiqMAX")))
  {
    stConfigData.fConfig_AntiFrzTempHTH = atof(pTemp);
    //printf("\n\rstConfigData.fConfig_AntiFrzTempHTH %f",stConfigData.fConfig_AntiFrzTempHTH);
  //SplitByte.fData = stConfigData.fConfig_AntiFrzTempHTH;
   //fnSPI_EEPROM_WriteData(HCM_ANTI_THRESH_TEMP_HIGH, 4, SplitByte.rgucData);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"HardMaterialSET")))
  {
    stConfigData.fConfig_InlineHardTempSet = atof(pTemp);
    //printf("\n\rstConfigData.fConfig_InlineHardTempSet %f",stConfigData.fConfig_InlineHardTempSet);
  //SplitByte.fData = stConfigData.fConfig_InlineHardTempSet;
   //fnSPI_EEPROM_WriteData(HCM_INLINE_HARD_SET_TEMP, 4, SplitByte.rgucData);		
    ucConfigDataCount++;	
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"HardMaterialMIN")))
  {
    stConfigData.fConfig_InlineHardTempLTH = atof(pTemp);
    //printf("\n\rstConfigData.fConfig_InlineHardTempLTH %f",stConfigData.fConfig_InlineHardTempLTH);
  //SplitByte.fData = stConfigData.fConfig_InlineHardTempLTH;
   //fnSPI_EEPROM_WriteData(HCM_INLINE_HARD_THRESH_TEMP_LOW, 4, SplitByte.rgucData);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"HardMaterialMAX")))
  {
    stConfigData.fConfig_InlineHardTempHTH = atof(pTemp);
    //printf("\n\rstConfigData.fConfig_InlineHardTempHTH %f",stConfigData.fConfig_InlineHardTempHTH);
  //SplitByte.fData = stConfigData.fConfig_InlineHardTempHTH;
   //fnSPI_EEPROM_WriteData(HCM_INLINE_HARD_THRESH_TEMP_HIGH, 4, SplitByte.rgucData);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"BaseMaterialSET")))
  {
    stConfigData.fConfig_InlineBaseTempSet = atof(pTemp);
    //printf("\n\rstConfigData.fConfig_InlineBaseTempSet %f",stConfigData.fConfig_InlineBaseTempSet);
  //SplitByte.fData = stConfigData.fConfig_InlineBaseTempSet;
   //fnSPI_EEPROM_WriteData(HCM_INLINE_BASE_SET_TEMP, 4, SplitByte.rgucData);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"BaseMaterialMIN")))
  {
    stConfigData.fConfig_InlineBaseTempLTH = atof(pTemp);
    //printf("\n\rstConfigData.fConfig_InlineBaseTempLTH %f",stConfigData.fConfig_InlineBaseTempLTH);
  //SplitByte.fData = stConfigData.fConfig_InlineBaseTempLTH;
   //fnSPI_EEPROM_WriteData(HCM_INLINE_BASE_THRESH_TEMP_LOW, 4, SplitByte.rgucData);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"BaseMaterialMAX")))
  {
    stConfigData.fConfig_InlineBaseTempHTH = atof(pTemp);
    //printf("\n\rstConfigData.fConfig_InlineBaseTempHTH %f",stConfigData.fConfig_InlineBaseTempHTH);
  //SplitByte.fData = stConfigData.fConfig_InlineBaseTempHTH;
   //fnSPI_EEPROM_WriteData(HCM_INLINE_BASE_THRESH_TEMP_HIGH, 4, SplitByte.rgucData);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"PipeTempMIN")))
  {
    stConfigData.fConfig_PipeTempLTH = atof(pTemp);
  //SplitByte.fData = stConfigData.fConfig_PipeTempLTH ;
    //printf("\n\rstConfigData.fConfig_PipeTempLTH %f",stConfigData.fConfig_PipeTempLTH);
   //fnSPI_EEPROM_WriteData(OPMM_PIPE_TEMP_THRES_LOW, 4, SplitByte.rgucData);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"PipeTempMAX")))
  {
    stConfigData.fConfig_PipeTempHTH = atof(pTemp);
  //SplitByte.fData = stConfigData.fConfig_PipeTempHTH ;
    //printf("\n\rstConfigData.fConfig_PipeTempHTH %f",stConfigData.fConfig_PipeTempHTH);
   //fnSPI_EEPROM_WriteData(OPMM_PIPE_TEMP_THRES_HIGH, 4, SplitByte.rgucData);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"DCLSTime")))
  {
    stConfigData.fConfig_DCLSTime = atof(pTemp);
  //SplitByte.fData = stConfigData.fConfig_DCLSTime;
    //printf("\n\rstConfigData.fConfig_DCLSTime %f",stConfigData.fConfig_DCLSTime);
   //fnSPI_EEPROM_WriteData(OPMM_DCLS_HOME_REACH_TIME, 4, SplitByte.rgucData);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"HLSTime")))
  {
    stConfigData.fConfig_HOMETime = atof(pTemp);
  //SplitByte.fData = stConfigData.fConfig_HOMETime;
    //printf("\n\rstConfigData.fConfig_HOMETime %f",stConfigData.fConfig_HOMETime);
   //fnSPI_EEPROM_WriteData(OPMM_HOME_DCLS_REACH_TIME, 4, SplitByte.rgucData);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"TimerToStopBackwardMotor")))
  {
    stConfigData.fConfig_TimeStopOPMMcommand = atof(pTemp);
  //SplitByte.fData = stConfigData.fConfig_TimeStopOPMMcommand;
    //printf("\n\rstConfigData.fConfig_TimeStopOPMMcommand %f",stConfigData.fConfig_TimeStopOPMMcommand);
   //fnSPI_EEPROM_WriteData(OPMM_STOP_HLS_TIMER, 4, SplitByte.rgucData);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"SystemDateTime")))
  {
    char *pTemp1,*pTemp2;
    
    pTemp1 = strtok(pTemp,"/");
    stConfigData.ucConfig_SystemTime[5] = atoi(pTemp1)/100;
    stConfigData.ucConfig_SystemTime[6] = atoi(pTemp1)%2000;
    pTemp1 = strstr(pTemp,"/");
    pTemp2 = strtok(pTemp1,"/");
    stConfigData.ucConfig_SystemTime[4] = atoi(pTemp2);
    pTemp1 = strstr(pTemp,"/");
    pTemp2 = strtok(pTemp1," ");
    stConfigData.ucConfig_SystemTime[3] = atoi(pTemp2);
    pTemp1 = strstr(pTemp," ");
    pTemp2 = strtok(pTemp1,":");
    stConfigData.ucConfig_SystemTime[0] = atoi(pTemp2);
    pTemp1 = strstr(pTemp,":");
    pTemp2 = strtok(pTemp1,":");
    stConfigData.ucConfig_SystemTime[1] = atoi(pTemp2);
    pTemp1 = strstr(pTemp,":");
    pTemp2 = strtok(pTemp1,"\0");
    stConfigData.ucConfig_SystemTime[2] = atoi(pTemp2);
    
    //printf("Hours = %d\r\n", stConfigData.ucConfig_SystemTime[0]);
    
    //printf("Minute = %d\r\n", stConfigData.ucConfig_SystemTime[1]);
    
    //printf("Second = %d\r\n", stConfigData.ucConfig_SystemTime[2]);
	
    //printf("Date = %d\r\n", stConfigData.ucConfig_SystemTime[3]);
    
    //printf("Month = %d\r\n", stConfigData.ucConfig_SystemTime[4]);
    
    //printf("Year = %d\r\n", stConfigData.ucConfig_SystemTime[6]);
    
   //fnSPI_EEPROM_WriteData(SYS_TIME, 7, stConfigData.ucConfig_SystemTime);
    ucConfigDataCount++;
  }
	
	  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"TimeOffset")))
  {
    char *pTemp1,*pTemp2;
		if('-' == pTemp[0])
		{
			//printf("\n\rNegative Offset");
			stConfigData.ucConfig_TimeOffsetSign = 0x01;
		}
		else if('+' == pTemp[0])
    {
			//printf("\n\rPositive Offset");
			stConfigData.ucConfig_TimeOffsetSign = 0x00;
		}
    pTemp2 = strtok(&pTemp[1],":");
    stConfigData.ucConfig_TimeOffsetHour = atoi(pTemp2);
    pTemp1 = strstr(pTemp,":");
    pTemp2 = strtok(pTemp1,"\0");
    stConfigData.ucConfig_TimeOffsetMin = atoi(pTemp2);
		
		//printf("\n\rOffset %d %02d:%02d",stConfigData.ucConfig_TimeOffsetSign,stConfigData.ucConfig_TimeOffsetHour
		//,stConfigData.ucConfig_TimeOffsetMin);

    ucConfigDataCount++;
  }
	
	
	
  /****************************************************************/
  /*change17*/
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"DataLoggingPeriod")))
  {
    stConfigData.fConfig_DataLoggingPeriod = atof(pTemp);
  //SplitByte.fData = stConfigData.fConfig_DataLoggingPeriod;
    //printf("\n\rstConfigData.fConfig_DataLoggingPeriod %f",stConfigData.fConfig_DataLoggingPeriod);
   //fnSPI_EEPROM_WriteData(TIME_DATA_LOGGING, 4, SplitByte.rgucData);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"TimeBeforeStall")))
  {
    stConfigData.fConfig_TimeBeforeStall = atof(pTemp);
    //printf("\n\rstConfigData.fConfig_TimeBeforeStall %f",stConfigData.fConfig_TimeBeforeStall);
  //SplitByte.fData = stConfigData.fConfig_TimeBeforeStall;
   //fnSPI_EEPROM_WriteData(VCM_BEFORE_STALL, 4, SplitByte.rgucData);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"TimeBeforeComparingStall")))
  {
    stConfigData.fConfig_TimeCompareStall = atof(pTemp);
    //printf("\n\rstConfigData.fConfig_TimeCompareStall %f",stConfigData.fConfig_TimeCompareStall);
  //SplitByte.fData = stConfigData.fConfig_TimeCompareStall;
   //fnSPI_EEPROM_WriteData(VCM_COMPARE_STALL, 4, SplitByte.rgucData);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"GunSwitching")))
  {
    stConfigData.fConfig_GunPosChangeTime = atof(pTemp);
  //SplitByte.fData = stConfigData.fConfig_GunPosChangeTime;
    //printf("\n\rstConfigData.fConfig_GunPosChangeTime %f",stConfigData.fConfig_GunPosChangeTime);
   //fnSPI_EEPROM_WriteData(OPMM_GUN_POSITION_CHANGE_TIME, 4, SplitByte.rgucData);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"VBATSprayDelay")))
  {
    stConfigData.fConfig_TimebeforeONSprayValve = atof(pTemp);
  //SplitByte.fData = stConfigData.fConfig_TimebeforeONSprayValve;
    //printf("\n\rstConfigData.fConfig_TimebeforeONSprayValve %f",stConfigData.fConfig_TimebeforeONSprayValve);
   //fnSPI_EEPROM_WriteData(OPMM_DELAY_SPRAY_VALVE_FLUSH_VBAT_TIME, 4, SplitByte.rgucData);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"FlushWarningTime")))
  {
    stConfigData.fConfig_FlushWarnTime = atof(pTemp);
  //SplitByte.fData = stConfigData.fConfig_FlushWarnTime;
    //printf("\n\rstConfigData.fConfig_FlushWarnTime %f",stConfigData.fConfig_FlushWarnTime);
   //fnSPI_EEPROM_WriteData(OPMM_FLUSH_WARNNING_TIME, 4, SplitByte.rgucData);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"TimerToStartStallTest")))
  {
    stConfigData.fConfig_TimeBeforeSendingStallcommand = atof(pTemp);
  //SplitByte.fData = stConfigData.fConfig_TimeBeforeSendingStallcommand;
    //printf("\n\rstConfigData.fConfig_TimeBeforeSendingStallcommand %f",stConfigData.fConfig_TimeBeforeSendingStallcommand);
   //fnSPI_EEPROM_WriteData(SCU_TIME_SEND_STALL_COMMAND, 4, SplitByte.rgucData);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"PrimeTime")))
  {
    stConfigData.fConfig_PrimeTime = atof(pTemp);
  //SplitByte.fData = stConfigData.fConfig_PrimeTime ;
    //printf("\n\rstConfigData.fConfig_PrimeTime %f",stConfigData.fConfig_PrimeTime);
   //fnSPI_EEPROM_WriteData(OPMM_PRIME_TIME, 4, SplitByte.rgucData);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"GunDelayTime")))
  {
    stConfigData.fConfig_GunDelayTime = atof(pTemp);
  //SplitByte.fData = stConfigData.fConfig_GunDelayTime;
    //printf("\n\rstConfigData.fConfig_GunDelayTime %f",stConfigData.fConfig_GunDelayTime);
   //fnSPI_EEPROM_WriteData(OPMM_GUN_DELAY_TIME, 4, SplitByte.rgucData);	
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"SprayOverlapTime")))
  {
    stConfigData.fConfig_SprayOverlapTime = atof(pTemp);
  //SplitByte.fData = stConfigData.fConfig_SprayOverlapTime;
    //printf("\n\rstConfigData.fConfig_SprayOverlapTime %f",stConfigData.fConfig_SprayOverlapTime);
   //fnSPI_EEPROM_WriteData(OPMM_SPRAY_OVERLAPTIME, 4, SplitByte.rgucData);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"ReverseDelayTime")))
  {
    stConfigData.fConfig_ReverseDelayTime = atof(pTemp);
  //SplitByte.fData = stConfigData.fConfig_ReverseDelayTime;
    //printf("\n\rstConfigData.fConfig_ReverseDelayTime %f",stConfigData.fConfig_ReverseDelayTime);
   //fnSPI_EEPROM_WriteData(OPMM_REVERSE_DELAYTIME, 4, SplitByte.rgucData);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"FlushTime")))
  {
    stConfigData.fConfig_FlushTime = atof(pTemp);
  //SplitByte.fData = stConfigData.fConfig_FlushTime;
    //printf("\n\rstConfigData.fConfig_FlushTime %f",stConfigData.fConfig_FlushTime);
   //fnSPI_EEPROM_WriteData(OPMM_FLUSH_TIME, 4, SplitByte.rgucData);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"IdealTimeforPeriodicLog")))
  {
    stConfigData.fConfig_IdleTimePeriodicLog = atof(pTemp);
  //SplitByte.fData = stConfigData.fConfig_IdleTimePeriodicLog;
    //printf("\n\rstConfigData.fConfig_IdleTimePeriodicLog %f",stConfigData.fConfig_IdleTimePeriodicLog);
   //fnSPI_EEPROM_WriteData(IDLE_DATA_LOGGING, 4, SplitByte.rgucData);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"AreaofCylinderA")))
  {
    stConfigData.fConfig_AreaofCylinderA = atof(pTemp);
    //printf("\n\rstConfigData.fConfig_AreaofCylinderA %f",stConfigData.fConfig_AreaofCylinderA);
  //SplitByte.fData = stConfigData.fConfig_AreaofCylinderA;
   //fnSPI_EEPROM_WriteData(VCM_CYLINDER_AREA1, 4, SplitByte.rgucData);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"AreaofCylinderB")))
  {
    stConfigData.fConfig_AreaofCylinderB = atof(pTemp);
    //printf("\n\rstConfigData.fConfig_AreaofCylinderB %f",stConfigData.fConfig_AreaofCylinderB);
  //SplitByte.fData = stConfigData.fConfig_AreaofCylinderB ;
   //fnSPI_EEPROM_WriteData(VCM_CYLINDER_AREA2, 4, SplitByte.rgucData);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"AreaofCylinderC")))
  {
    stConfigData.fConfig_AreaofCylinderC = atof(pTemp);
    //printf("\n\rstConfigData.fConfig_AreaofCylinderC %f",stConfigData.fConfig_AreaofCylinderC);
  //SplitByte.fData = stConfigData.fConfig_AreaofCylinderC;
   //fnSPI_EEPROM_WriteData(VCM_CYLINDER_AREA3, 4, SplitByte.rgucData);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"CylinderCount")))
  {
    stConfigData.ucConfig_CylinderCount = atoi(pTemp);
    //printf("\n\rstConfigData.ucConfig_CylinderCount %d",stConfigData.ucConfig_CylinderCount);
   //fnSPI_EEPROM_WriteData(VCM_CYLINDER_COUNT, 1, &stConfigData.ucConfig_CylinderCount);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"SprayingCount")))
  {
    stConfigData.ucConfig_SprayCount = atoi(pTemp);
    //printf("\n\rstConfigData.ucConfig_SprayCount %d",stConfigData.ucConfig_SprayCount);
   //fnSPI_EEPROM_WriteData(OPMM_SPRAY_COUNT, 1, &stConfigData.ucConfig_SprayCount);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"BuzzerVolume")))
  {
    stConfigData.ucConfig_BuzzerVolume = atoi(pTemp);
    //printf("\n\rstConfigData.ucConfig_BuzzerVolume %d",stConfigData.ucConfig_BuzzerVolume);
   //fnSPI_EEPROM_WriteData(OPMM_BUZZER_VOLUME, 1, &stConfigData.ucConfig_BuzzerVolume);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"SignatureByte")))
  {
    uiBakSignature = stConfigData.uiConfig_Signature = atoi(pTemp);
    //printf("\n\rstConfigData.uiConfig_Signature %d",stConfigData.uiConfig_Signature);
  //SplitByte.uiData = stConfigData.uiConfig_Signature;
   //fnSPI_EEPROM_WriteData(SIGNATURE_START, SIGNATUREVALUE_SIZE, SplitByte.rgucData);	
  //SplitByte.uiData = uiBakSignature;
   //fnSPI_EEPROM_WriteData(SIGNATURE_BACKUP, SIGNATUREVALUE_SIZE, SplitByte.rgucData);	
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"BatteryKneeVoltage")))
  {
    stConfigData.fConfig_BatteryKneeVoltage = atof(pTemp);
  //SplitByte.fData = stConfigData.fConfig_BatteryKneeVoltage;
    //printf("\n\rstConfigData.fConfig_BatteryKneeVoltage %f",stConfigData.fConfig_BatteryKneeVoltage);
   //fnSPI_EEPROM_WriteData(OPMM_BATT_KNEE_VOLT, 4, SplitByte.rgucData);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"PipeDiameter")))
  {
    stConfigData.fConfig_PipeDiameter = atof(pTemp);
  //SplitByte.fData = stConfigData.fConfig_PipeDiameter;
    //printf("\n\rstConfigData.fConfig_PipeDiameter %f",stConfigData.fConfig_PipeDiameter);
   //fnSPI_EEPROM_WriteData(PIPE_DIAMETER, 4, SplitByte.rgucData);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"KP")))
  {
    stConfigData.fConfig_JointNum = atof(pTemp);
  //SplitByte.fData = stConfigData.fConfig_JointNum;
    //printf("\n\rstConfigData.fConfig_JointNum %f",stConfigData.fConfig_JointNum);
   //fnSPI_EEPROM_WriteData(COUNT_JOINT, 4, SplitByte.rgucData);
    ucConfigDataCount++;
  }
  
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"WIFI_SSID")))
  {
    uint8_t ucLoop = 0;
		memset( stWifiIP.rgucSSID,'\0',16);
    for(ucLoop=0;ucLoop<15;ucLoop++)
      stWifiIP.rgucSSID[ucLoop] = pTemp[ucLoop];
    stWifiIP.rgucSSID[ucLoop] = NULL;
    /******************************************** SCU_WIFI_SSIDADDRESS***************************************/
   //fnSPI_EEPROM_WriteData(SCU_WIFI_SSIDADDRESS, 15, stWifiIP.rgucSSID);
    //printf("SCU_WIFI_SSIDADDRESS = %s\r\n", stWifiIP.rgucSSID);
    ucConfigDataCount++;
//    ucWifiChanges = SET;
  }
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"WIFI_Key")))
  {
    uint8_t ucLoop = 0;
		memset( stWifiIP.rgucPASWD,'\0',16);
    for(ucLoop=0;ucLoop<15;ucLoop++)
      stWifiIP.rgucPASWD[ucLoop] = pTemp[ucLoop];
    stWifiIP.rgucPASWD[ucLoop] = NULL;
    /******************************************** SCU_WIFI_PASSADDRESS***************************************/
   //fnSPI_EEPROM_WriteData(SCU_WIFI_PASSADDRESS, 15, stWifiIP.rgucPASWD);
    //printf("SCU_WIFI_PASSADDRESS = %s\r\n", stWifiIP.rgucPASWD);
    //ucWifiChanges = SET;
    ucConfigDataCount++;
  }
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"WIFI_IP")))
  {
    uint8_t ucLoop = 0;
		memset( stWifiIP.rgucIP_Addr,'\0',16);
    for(ucLoop=0;ucLoop<15;ucLoop++)
      stWifiIP.rgucIP_Addr[ucLoop] = pTemp[ucLoop];
    stWifiIP.rgucIP_Addr[ucLoop] = NULL;
    /******************************************** SCU_WIFI_IPADDRESS***************************************/
   //fnSPI_EEPROM_WriteData(SCU_WIFI_IPADDRESS, 15, stWifiIP.rgucIP_Addr);
    //printf("SCU_WIFI_IPADDRESS = %s\r\n", stWifiIP.rgucIP_Addr);
//    ucWifiChanges = SET;
    ucConfigDataCount++;
  }
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"WIFI_SubnetMask")))
  {
    uint8_t ucLoop = 0;
		memset( stWifiIP.rgucSubnet_Addr,'\0',16);
    for(ucLoop=0;ucLoop<15;ucLoop++)
      stWifiIP.rgucSubnet_Addr[ucLoop] = pTemp[ucLoop];
    stWifiIP.rgucSubnet_Addr[ucLoop] = NULL;
    /******************************************** SCU_WIFI_SMADDRESS***************************************/
   //fnSPI_EEPROM_WriteData(SCU_WIFI_SMADDRESS, 15, stWifiIP.rgucSubnet_Addr);
    //printf("SCU_WIFI_SMADDRESS = %s\r\n", stWifiIP.rgucSubnet_Addr);
//    ucWifiChanges = SET;
    ucConfigDataCount++;
  }
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"WIFI_Gateway")))
  {
    uint8_t ucLoop = 0;
		memset( stWifiIP.rgucGateway_Addr,'\0',16);
    for(ucLoop=0;ucLoop<15;ucLoop++)
      stWifiIP.rgucGateway_Addr[ucLoop] = pTemp[ucLoop];
    stWifiIP.rgucGateway_Addr[ucLoop] = NULL;
    /******************************************** SCU_WIFI_GWADDRESS***************************************/
   //fnSPI_EEPROM_WriteData(SCU_WIFI_GWADDRESS, 15, stWifiIP.rgucGateway_Addr);
    //printf("SCU_WIFI_GWADDRESS = %s\r\n", stWifiIP.rgucGateway_Addr);
//    ucWifiChanges = SET;
    ucConfigDataCount++;
  }
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"WIFI_Port")))
  {
    stWifiIP.uiPort= atoi(pTemp);
  //SplitByte.uiData = stWifiIP.uiPort;
    /******************************************** SCU_WIFI_PORTADDRESS***************************************/
   //fnSPI_EEPROM_WriteData(SCU_WIFI_PORTADDRESS, 2, SplitByte.rgucData);
    //printf("SCU_WIFI_PORTADDRESS = %d\r\n", stWifiIP.uiPort);
//    ucWifiChanges = SET;
    ucConfigDataCount++;
  }
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"Ethernet_IP")))
  {
    uint8_t ucLoop = 0;
		memset( stEthernetIP.rgucIP_Addr,'\0',16);
    for(ucLoop=0;ucLoop<15;ucLoop++)
      stEthernetIP.rgucIP_Addr[ucLoop] = pTemp[ucLoop];
    stEthernetIP.rgucIP_Addr[ucLoop] = NULL;
    /******************************************** SCU_ETH_IPADDRESS***************************************/
   //fnSPI_EEPROM_WriteData(SCU_ETH_IPADDRESS, 15, stEthernetIP.rgucIP_Addr);
    //printf("SCU_ETH_IPADDRESS = %s\r\n", stEthernetIP.rgucIP_Addr);
    ucETH_Changes = SET;
    ucConfigDataCount++;
  }
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"Ethernet_SubnetMask")))
  {
    uint8_t ucLoop = 0;
		memset( stEthernetIP.rgucSubnet_Addr,'\0',16);
    for(ucLoop=0;ucLoop<15;ucLoop++)
      stEthernetIP.rgucSubnet_Addr[ucLoop] = pTemp[ucLoop];
    stEthernetIP.rgucSubnet_Addr[ucLoop] = NULL;
    /******************************************** SCU_ETH_SMADDRESS***************************************/
   //fnSPI_EEPROM_WriteData(SCU_ETH_SMADDRESS, 15, stEthernetIP.rgucSubnet_Addr);
    //printf("SCU_ETH_SMADDRESS = %s\r\n", stEthernetIP.rgucSubnet_Addr);
    ucETH_Changes = SET;
    ucConfigDataCount++;
  }
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"Ethernet_Gateway")))
  {
    uint8_t ucLoop = 0;
		memset( stEthernetIP.rgucGateway_Addr,'\0',16);
    for(ucLoop=0;ucLoop<15;ucLoop++)
      stEthernetIP.rgucGateway_Addr[ucLoop] = pTemp[ucLoop];
    stEthernetIP.rgucGateway_Addr[ucLoop] = NULL;
    /******************************************** SCU_ETH_GWADDRESS***************************************/
   //fnSPI_EEPROM_WriteData(SCU_ETH_GWADDRESS, 15, stEthernetIP.rgucGateway_Addr);
    //printf("SCU_ETH_GWADDRESS = %s\r\n", stEthernetIP.rgucGateway_Addr);
    ucETH_Changes = SET;
    ucConfigDataCount++;
  }
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"Ethernet_Port")))
  {
    stEthernetIP.uiPort= atoi(pTemp);
  //SplitByte.uiData = stEthernetIP.uiPort;
    /******************************************** SCU_ETH_PORTADDRESS***************************************/
   //fnSPI_EEPROM_WriteData(SCU_ETH_PORTADDRESS, 2, SplitByte.rgucData);
    //printf("SCU_ETH_PORTADDRESS = %d\r\n", stEthernetIP.uiPort);
    ucETH_Changes = SET;
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"AntifreezeHystPOSITIVE")))
  {
    stConfigData.fConfig_HysAntiFrzPos = atof(pTemp);
    //printf("\n\rstConfigData.fConfig_HysAntiFrzPos %f",stConfigData.fConfig_HysAntiFrzPos);
  //SplitByte.fData = stConfigData.fConfig_HysAntiFrzPos;
   //fnSPI_EEPROM_WriteData(HCM_ANTI_HYST_TEMP_POS, 4, SplitByte.rgucData);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"AntifreezeHystNEGATIVE")))
  {
    stConfigData.fConfig_HysAntiFrzNeg = atof(pTemp);
    //printf("\n\rstConfigData.fConfig_HysAntiFrzNeg %f",stConfigData.fConfig_HysAntiFrzNeg);
  //SplitByte.fData = stConfigData.fConfig_HysAntiFrzNeg;
   //fnSPI_EEPROM_WriteData(HCM_ANTI_HYST_TEMP_NEG, 4, SplitByte.rgucData);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"HardHysteresisPOSITIVE")))
  {
    stConfigData.fConfig_InlineHysHardPos = atof(pTemp);
    //printf("\n\rstConfigData.fConfig_InlineHysHardPos %f",stConfigData.fConfig_InlineHysHardPos);
  //SplitByte.fData = stConfigData.fConfig_InlineHysHardPos;
   //fnSPI_EEPROM_WriteData(HCM_INLINE_HARD_HYST_TEMP_POS, 4, SplitByte.rgucData);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"HardHysteresisNEGATIVE")))
  {
    stConfigData.fConfig_InlineHysHardNeg = atof(pTemp);
    //printf("\n\rstConfigData.fConfig_InlineHysHardNeg %f",stConfigData.fConfig_InlineHysHardNeg);
  //SplitByte.fData = stConfigData.fConfig_InlineHysHardNeg;
   //fnSPI_EEPROM_WriteData(HCM_INLINE_HARD_HYST_TEMP_NEG, 4, SplitByte.rgucData);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"BaseHysteresisPOSITIVE")))
  {
    stConfigData.fConfig_HysInlineBasePos = atof(pTemp);
    //printf("\n\rstConfigData.fConfig_HysInlineBasePos %f",stConfigData.fConfig_HysInlineBasePos);
  //SplitByte.fData = stConfigData.fConfig_HysInlineBasePos;
   //fnSPI_EEPROM_WriteData(HCM_INLINE_BASE_HYST_TEMP_POS, 4, SplitByte.rgucData);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"BaseHysteresisNEGATIVE")))
  {
    stConfigData.fConfig_HysInlineBaseNeg = atof(pTemp);
    //printf("\n\rstConfigData.fConfig_HysInlineBaseNeg %f",stConfigData.fConfig_HysInlineBaseNeg);
  //SplitByte.fData = stConfigData.fConfig_HysInlineBaseNeg;
   //fnSPI_EEPROM_WriteData(HCM_INLINE_BASE_HYST_TEMP_NEG, 4, SplitByte.rgucData);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"SprayPOSITIVE")))
  {
    stConfigData.fConfig_HysSprayPresPos = atof(pTemp);
  //SplitByte.fData = stConfigData.fConfig_HysSprayPresPos;
    //printf("\n\rstConfigData.fConfig_HysSprayPresPos %f",stConfigData.fConfig_HysSprayPresPos);
   //fnSPI_EEPROM_WriteData(SCU_SPRAY_PRESSURE_HYST_POS, 4, SplitByte.rgucData);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"SprayNEGATIVE")))
  {
    stConfigData.fConfig_HysSprayPresNeg = atof(pTemp);
  //SplitByte.fData = stConfigData.fConfig_HysSprayPresNeg;
    //printf("\n\rstConfigData.fConfig_HysSprayPresNeg %f",stConfigData.fConfig_HysSprayPresNeg);
   //fnSPI_EEPROM_WriteData(SCU_SPRAY_PRESSURE_HYST_NEG, 4, SplitByte.rgucData);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"RecirculationPOSITIVE")))
  {
    stConfigData.fConfig_HysRecircPresPos = atof(pTemp);
  //SplitByte.fData = stConfigData.fConfig_HysRecircPresPos;
    //printf("\n\rstConfigData.fConfig_HysRecircPresPos %f",stConfigData.fConfig_HysRecircPresPos);
   //fnSPI_EEPROM_WriteData(SCU_RECIRC_PRESSURE_HYST_POS, 4, SplitByte.rgucData);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"RecirculationNEGATIVE")))
  {
    stConfigData.fConfig_HysRecircPresNeg = atof(pTemp);
  //SplitByte.fData = stConfigData.fConfig_HysRecircPresNeg;
    //printf("\n\rstConfigData.fConfig_HysRecircPresNeg %f",stConfigData.fConfig_HysRecircPresNeg);
   //fnSPI_EEPROM_WriteData(SCU_RECIRC_PRESSURE_HYST_NEG, 4, SplitByte.rgucData);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  
  /*        VCM RELATED CONFIG VARIABLES   */
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"StallThresholdUp")))
  {
    stConfigData.iConfig_UpStrokeTH = atoi(pTemp);
    //printf("\n\rstConfigData.iConfig_UpStrokeTH %d",stConfigData.iConfig_UpStrokeTH);
    /*change17*/
    
  //SplitByte.iData = stConfigData.iConfig_UpStrokeTH;
   //fnSPI_EEPROM_WriteData(VCM_UPSTROKE_ADD, 2, SplitByte.rgucData);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"StallThresholdDown")))
  {
    stConfigData.iConfig_DownStrokeTH = atoi(pTemp);
    //printf("\n\rstConfigData.iConfig_DownStrokeTH %d",stConfigData.iConfig_DownStrokeTH);
  //SplitByte.iData = stConfigData.iConfig_DownStrokeTH;
   //fnSPI_EEPROM_WriteData(VCM_DOWNSTROKE_ADD, 2, SplitByte.rgucData);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"UpstrokeLimit")))
  {
    stConfigData.ucConfig_UpstrokeUpLimit = atoi(pTemp);
    //printf("\n\rstConfigData.ucConfig_UpstrokeUpLimit %d",stConfigData.ucConfig_UpstrokeUpLimit);
   //fnSPI_EEPROM_WriteData(VCM_STALL_UPSTROKE_UPPER_LIMIT, 1, &stConfigData.ucConfig_UpstrokeUpLimit); 
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"UpstrokeMax")))
  {
    stConfigData.ucConfig_UpstrokeMaxRange = atoi(pTemp);
    //printf("\n\rstConfigData.ucConfig_UpstrokeMaxRange %d",stConfigData.ucConfig_UpstrokeMaxRange);
   //fnSPI_EEPROM_WriteData(VCM_STALL_UPSTROKE_MAX_RANGE, 1, &stConfigData.ucConfig_UpstrokeMaxRange); 
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"UpstrokeMin")))
  {
    stConfigData.ucConfig_UpstrokeMinRange = atoi(pTemp);
    //printf("\n\rstConfigData.ucConfig_UpstrokeMinRange %d",stConfigData.ucConfig_UpstrokeMinRange);
   //fnSPI_EEPROM_WriteData(VCM_STALL_UPSTROKE_MIN_RANGE, 1, &stConfigData.ucConfig_UpstrokeMinRange); 
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"DownstrokeLimit")))
  {
    stConfigData.ucConfig_DownstrokeUpLimit = atoi(pTemp);
    //printf("\n\rstConfigData.ucConfig_DownstrokeUpLimit %d",stConfigData.ucConfig_DownstrokeUpLimit);
   //fnSPI_EEPROM_WriteData(VCM_STALL_DOWNSTROKE_UPPER_LIMIT, 1, &stConfigData.ucConfig_DownstrokeUpLimit); 
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"DownstrokeMax")))
  {
    stConfigData.ucConfig_DownstrokeMaxRange = atoi(pTemp);
    //printf("\n\rstConfigData.ucConfig_DownstrokeMaxRange %d",stConfigData.ucConfig_DownstrokeMaxRange);
   //fnSPI_EEPROM_WriteData(VCM_STALL_DOWNSTROKE_MAX_RANGE, 1, &stConfigData.ucConfig_DownstrokeMaxRange); 
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"DownstrokeMin")))
  {
    stConfigData.ucConfig_DownstrokeMinRange = atoi(pTemp);
    //printf("\n\rstConfigData.ucConfig_DownstrokeMinRange %d",stConfigData.ucConfig_DownstrokeMinRange);
   //fnSPI_EEPROM_WriteData(VCM_STALL_DOWNSTROKE_MIN_RANGE, 1, &stConfigData.ucConfig_DownstrokeMinRange); 
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/
	pTemp = NULL;
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"SupervisorName")))
  {
    uint8_t ucLoop = 0;
								/*change78*/
						memset(supervisornamepre,'\0',FLASH_RX_TX_BUFFER_SIZE);
            strcat((char *)supervisornamepre,(char *)stConfigData.rgucConfig_SupervisorName1);
            strcat((char *)supervisornamepre,(char *)stConfigData.rgucConfig_SupervisorName2);
            strcat((char *)supervisornamepre,(char *)stConfigData.rgucConfig_SupervisorName3);	
						StatusFlag.stStatusFlag.bConfigSupervisorName = SET;	
		//printf("SUPERVISOR_NAME = %s %d\r\n", pTemp,strlen(pTemp));
		memset(stConfigData.rgucConfig_SupervisorName1,'\0',8);
    memset(stConfigData.rgucConfig_SupervisorName2,'\0',8);
		memset(stConfigData.rgucConfig_SupervisorName3,'\0',8);
		for(ucLoop=0;ucLoop<7;ucLoop++)
      stConfigData.rgucConfig_SupervisorName1[ucLoop] = pTemp[ucLoop];
    stConfigData.rgucConfig_SupervisorName1[ucLoop] = NULL;
    for(ucLoop=0;ucLoop<7;ucLoop++)
      stConfigData.rgucConfig_SupervisorName2[ucLoop] = pTemp[ucLoop+7];
    stConfigData.rgucConfig_SupervisorName2[ucLoop] = NULL;
    for(ucLoop=0;ucLoop<7;ucLoop++)
      stConfigData.rgucConfig_SupervisorName3[ucLoop] = pTemp[ucLoop+14];
    stConfigData.rgucConfig_SupervisorName3[ucLoop] = NULL;
    /******************************************** SUPERVISOR ***************************************/
   //fnSPI_EEPROM_WriteData(SUPERVISOR_NAME1, 7, stConfigData.rgucConfig_SupervisorName1);
    //printf("SUPERVISOR_NAME1 = %s\r\n", stConfigData.rgucConfig_SupervisorName1);
   //fnSPI_EEPROM_WriteData(SUPERVISOR_NAME2, 7, stConfigData.rgucConfig_SupervisorName2);
    //printf("SUPERVISOR_NAME2 = %s\r\n", stConfigData.rgucConfig_SupervisorName2);
   //fnSPI_EEPROM_WriteData(SUPERVISOR_NAME3, 7, stConfigData.rgucConfig_SupervisorName3);
    //printf("SUPERVISOR_NAME3 = %s\r\n", stConfigData.rgucConfig_SupervisorName3);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/
	pTemp = NULL;
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"CoatingMaterial")))
  {
    uint8_t ucLoop = 0;
		memset(stConfigData.rgucConfig_CoatingMatName1,'\0',8);
    memset(stConfigData.rgucConfig_CoatingMatName2,'\0',8);
		memset(stConfigData.rgucConfig_CoatingMatName3,'\0',8);
    for(ucLoop=0;ucLoop<7;ucLoop++)
      stConfigData.rgucConfig_CoatingMatName1[ucLoop] = pTemp[ucLoop];
    stConfigData.rgucConfig_CoatingMatName1[ucLoop] = NULL;
    for(ucLoop=0;ucLoop<7;ucLoop++)
      stConfigData.rgucConfig_CoatingMatName2[ucLoop] = pTemp[ucLoop+7];
    stConfigData.rgucConfig_CoatingMatName2[ucLoop] = NULL;
    for(ucLoop=0;ucLoop<7;ucLoop++)
      stConfigData.rgucConfig_CoatingMatName3[ucLoop] = pTemp[ucLoop+14];
    stConfigData.rgucConfig_CoatingMatName3[ucLoop] = NULL;
    /******************************************** COATING_MAT_NAME***************************************/
   //fnSPI_EEPROM_WriteData(COATING_MAT_NAME1, 7, stConfigData.rgucConfig_CoatingMatName1);
    //printf("COATING_MAT_NAME1 = %s\r\n", stConfigData.rgucConfig_CoatingMatName1);
   //fnSPI_EEPROM_WriteData(COATING_MAT_NAME2, 7, stConfigData.rgucConfig_CoatingMatName2);
    //printf("COATING_MAT_NAME2 = %s\r\n", stConfigData.rgucConfig_CoatingMatName2);
   //fnSPI_EEPROM_WriteData(COATING_MAT_NAME3, 7, stConfigData.rgucConfig_CoatingMatName3);
    //printf("COATING_MAT_NAME3 = %s\r\n", stConfigData.rgucConfig_CoatingMatName3);
    ucConfigDataCount++;
  }
	pTemp = NULL;
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"BaseMaterialBatchNumber")))
  {
    uint8_t ucLoop = 0;
		memset(stConfigData.rgucConfig_BaseMatBatchNo1,'\0',8);
    memset(stConfigData.rgucConfig_BaseMatBatchNo2,'\0',8);
		memset(stConfigData.rgucConfig_BaseMatBatchNo3,'\0',8);
    for(ucLoop=0;ucLoop<7;ucLoop++)
      stConfigData.rgucConfig_BaseMatBatchNo1[ucLoop] = pTemp[ucLoop];
    stConfigData.rgucConfig_BaseMatBatchNo1[ucLoop] = NULL;
    for(ucLoop=0;ucLoop<7;ucLoop++)
      stConfigData.rgucConfig_BaseMatBatchNo2[ucLoop] = pTemp[ucLoop+7];
    stConfigData.rgucConfig_BaseMatBatchNo3[ucLoop] = NULL;
    for(ucLoop=0;ucLoop<7;ucLoop++)
      stConfigData.rgucConfig_BaseMatBatchNo3[ucLoop] = pTemp[ucLoop+14];
    stConfigData.rgucConfig_BaseMatBatchNo3[ucLoop] = NULL;
    /******************************************** COATING_MAT_NAME***************************************/
   //fnSPI_EEPROM_WriteData(BASE_MAT_BATCHNO1, 7, stConfigData.rgucConfig_BaseMatBatchNo1);
    //printf("BASE_MAT_BATCHNO1 = %s\r\n", stConfigData.rgucConfig_BaseMatBatchNo1);
   //fnSPI_EEPROM_WriteData(BASE_MAT_BATCHNO2, 7, stConfigData.rgucConfig_BaseMatBatchNo2);
    //printf("BASE_MAT_BATCHNO2 = %s\r\n", stConfigData.rgucConfig_BaseMatBatchNo2);
   //fnSPI_EEPROM_WriteData(BASE_MAT_BATCHNO3, 7, stConfigData.rgucConfig_BaseMatBatchNo3);
    //printf("BASE_MAT_BATCHNO3 = %s\r\n", stConfigData.rgucConfig_BaseMatBatchNo3);
    ucConfigDataCount++;
  }
	pTemp = NULL;
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"HardMaterialBatchNumber")))
  {
    uint8_t ucLoop = 0;
		memset(stConfigData.rgucConfig_HardMatBatchNo1,'\0',8);
    memset(stConfigData.rgucConfig_HardMatBatchNo2,'\0',8);
		memset(stConfigData.rgucConfig_HardMatBatchNo3,'\0',8);
    for(ucLoop=0;ucLoop<7;ucLoop++)
      stConfigData.rgucConfig_HardMatBatchNo1[ucLoop] = pTemp[ucLoop];
    stConfigData.rgucConfig_HardMatBatchNo1[ucLoop] = NULL;
    for(ucLoop=0;ucLoop<7;ucLoop++)
      stConfigData.rgucConfig_HardMatBatchNo2[ucLoop] = pTemp[ucLoop+7];
    stConfigData.rgucConfig_HardMatBatchNo2[ucLoop] = NULL;
    for(ucLoop=0;ucLoop<7;ucLoop++)
      stConfigData.rgucConfig_HardMatBatchNo3[ucLoop] = pTemp[ucLoop+14];
    stConfigData.rgucConfig_HardMatBatchNo3[ucLoop] = NULL;
    /******************************************** HardMaterialBatchNumber***************************************/
   //fnSPI_EEPROM_WriteData(HARD_MAT_BATCHNO1, 7, stConfigData.rgucConfig_HardMatBatchNo1);
    //printf("HARD_MAT_BATCHNO1 = %s\r\n", stConfigData.rgucConfig_HardMatBatchNo1);
   //fnSPI_EEPROM_WriteData(HARD_MAT_BATCHNO2, 7, stConfigData.rgucConfig_HardMatBatchNo2);
    //printf("HARD_MAT_BATCHNO2 = %s\r\n", stConfigData.rgucConfig_HardMatBatchNo2);
   //fnSPI_EEPROM_WriteData(HARD_MAT_BATCHNO3, 7, stConfigData.rgucConfig_HardMatBatchNo3);
    //printf("HARD_MAT_BATCHNO3 = %s\r\n", stConfigData.rgucConfig_HardMatBatchNo3);
    ucConfigDataCount++;
  }
	pTemp = NULL;
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"JobName")))
  {
    uint8_t ucLoop = 0;
			/*change78*/						
						memset(jobnamepre,'\0',FLASH_RX_TX_BUFFER_SIZE);
            strcat((char *)jobnamepre,(char *)stConfigData.rgucConfig_JobName1);
            strcat((char *)jobnamepre,(char *)stConfigData.rgucConfig_JobName2);
            strcat((char *)jobnamepre,(char *)stConfigData.rgucConfig_JobName3);	
						StatusFlag.stStatusFlag.bConfigJobName = SET;
		memset(stConfigData.rgucConfig_JobName1,'\0',8);
    memset(stConfigData.rgucConfig_JobName2,'\0',8);
		memset(stConfigData.rgucConfig_JobName3,'\0',8);
    for(ucLoop=0;ucLoop<7;ucLoop++)
      stConfigData.rgucConfig_JobName1[ucLoop] = pTemp[ucLoop];
    stConfigData.rgucConfig_JobName1[ucLoop] = NULL;
    for(ucLoop=0;ucLoop<7;ucLoop++)
      stConfigData.rgucConfig_JobName2[ucLoop] = pTemp[ucLoop+7];
    stConfigData.rgucConfig_JobName2[ucLoop] = NULL;
    for(ucLoop=0;ucLoop<7;ucLoop++)
      stConfigData.rgucConfig_JobName3[ucLoop] = pTemp[ucLoop+14];
    stConfigData.rgucConfig_JobName3[ucLoop] = NULL;
    /******************************************** JOB_NAME***************************************/
   //fnSPI_EEPROM_WriteData(JOB_NAME1, 7, stConfigData.rgucConfig_JobName1);
    //printf("JOB_NAME1 = %s\r\n", stConfigData.rgucConfig_JobName1);
   //fnSPI_EEPROM_WriteData(JOB_NAME2, 7, stConfigData.rgucConfig_JobName2);
    //printf("JOB_NAME2 = %s\r\n", stConfigData.rgucConfig_JobName2);
   //fnSPI_EEPROM_WriteData(JOB_NAME3, 7, stConfigData.rgucConfig_JobName3);
    //printf("JOB_NAME3 = %s\r\n", stConfigData.rgucConfig_JobName3);
    ucConfigDataCount++;
  }
	pTemp = NULL;
	if(NULL != (pTemp = fnGetDataElement(XMLstring,"JobID")))
  {
    uint8_t ucLoop = 0;
			/*change78*/						
						memset(jobidpre,'\0',FLASH_RX_TX_BUFFER_SIZE);
            strcat((char *)jobidpre,(char *)stConfigData.rgucConfig_JobId1);
            strcat((char *)jobidpre,(char *)stConfigData.rgucConfig_JobId2);
            strcat((char *)jobidpre,(char *)stConfigData.rgucConfig_JobId3);	
						StatusFlag.stStatusFlag.bConfigJobID = SET;
		memset(stConfigData.rgucConfig_JobId1,'\0',8);
    memset(stConfigData.rgucConfig_JobId2,'\0',8);
		memset(stConfigData.rgucConfig_JobId3,'\0',8);
    for(ucLoop=0;ucLoop<7;ucLoop++)
      stConfigData.rgucConfig_JobId1[ucLoop] = pTemp[ucLoop];
    stConfigData.rgucConfig_JobId1[ucLoop] = NULL;
    for(ucLoop=0;ucLoop<7;ucLoop++)
      stConfigData.rgucConfig_JobId2[ucLoop] = pTemp[ucLoop+7];
    stConfigData.rgucConfig_JobId2[ucLoop] = NULL;
    for(ucLoop=0;ucLoop<7;ucLoop++)
      stConfigData.rgucConfig_JobId3[ucLoop] = pTemp[ucLoop+14];
    stConfigData.rgucConfig_JobId3[ucLoop] = NULL;
    /******************************************** JOB_NAME***************************************/
   //fnSPI_EEPROM_WriteData(JOB_ID1, 7, stConfigData.rgucConfig_JobId1);
    //printf("JOB_ID1 = %s\r\n", stConfigData.rgucConfig_JobId1);
   //fnSPI_EEPROM_WriteData(JOB_ID2, 7, stConfigData.rgucConfig_JobId2);
    //printf("JOB_ID2 = %s\r\n", stConfigData.rgucConfig_JobId2);
   //fnSPI_EEPROM_WriteData(JOB_ID3, 7, stConfigData.rgucConfig_JobId3);
    //printf("JOB_ID3 = %s\r\n", stConfigData.rgucConfig_JobId3);
    ucConfigDataCount++;
  }
	
	
  /****************************************************************/
  /*change17*/
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"PressureGauge1LowTh")))
  {
    stConfigData.fConfig_PressureGauge1LTH = atof(pTemp);
    //printf("\n\rstConfigData.fConfig_PressureGauge1LTH %f",stConfigData.fConfig_PressureGauge1LTH);
  //SplitByte.fData = stConfigData.fConfig_PressureGauge1LTH;
   //fnSPI_EEPROM_WriteData(VCM_PRESSURE_LOWTH1, 4, SplitByte.rgucData);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"PressureGauge1HighTh")))
  {
    stConfigData.fConfig_PressureGauge1HTH = atof(pTemp);
    //printf("\n\rstConfigData.fConfig_PressureGauge1HTH %f",stConfigData.fConfig_PressureGauge1HTH);
   //SplitByte.fData = stConfigData.fConfig_PressureGauge1HTH;
   //fnSPI_EEPROM_WriteData(VCM_PRESSURE_HIGHTH1, 4,//SplitByte.rgucData);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"PressureGauge2LowTh")))
  {
    stConfigData.fConfig_PressureGauge2LTH = atof(pTemp);
    //printf("\n\rstConfigData.fConfig_PressureGauge2LTH %f",stConfigData.fConfig_PressureGauge2LTH);
   //SplitByte.fData = stConfigData.fConfig_PressureGauge2LTH;
   //fnSPI_EEPROM_WriteData(VCM_PRESSURE_LOWTH2, 4,//SplitByte.rgucData);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"PressureGauge2HighTh")))
  {
    stConfigData.fConfig_PressureGauge2HTH = atof(pTemp);
    //printf("\n\rstConfigData.fConfig_PressureGauge2HTH %f",stConfigData.fConfig_PressureGauge2HTH);
   //SplitByte.fData = stConfigData.fConfig_PressureGauge2HTH;
   //fnSPI_EEPROM_WriteData(VCM_PRESSURE_HIGHTH2, 4,//SplitByte.rgucData);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"PressureGauge3LowTh")))
  {
    stConfigData.fConfig_PressureGauge3LTH = atof(pTemp);
    //printf("\n\rstConfigData.fConfig_PressureGauge3LTH %f",stConfigData.fConfig_PressureGauge3LTH);
   //SplitByte.fData = stConfigData.fConfig_PressureGauge3LTH ;
   //fnSPI_EEPROM_WriteData(VCM_PRESSURE_LOWTH3, 4,//SplitByte.rgucData);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"PressureGauge3HighTh")))
  {
    stConfigData.fConfig_PressureGauge3HTH = atof(pTemp);
    //printf("\n\rstConfigData.fConfig_PressureGauge3HTH %f",stConfigData.fConfig_PressureGauge3HTH);
   //SplitByte.fData = stConfigData.fConfig_PressureGauge3HTH;
   //fnSPI_EEPROM_WriteData(VCM_PRESSURE_HIGHTH3, 4,//SplitByte.rgucData);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"RecirculationSet")))
  {
    stConfigData.fConfig_RecirPressure = atof(pTemp);
   //SplitByte.fData = stConfigData.fConfig_RecirPressure;
    //printf("\n\rstConfigData.fConfig_RecirPressure %f",stConfigData.fConfig_RecirPressure);
   //fnSPI_EEPROM_WriteData(SCU_RECIRC_PRESSURE_VALUE, 4,//SplitByte.rgucData);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"RecirculationMin")))
  {
    stConfigData.fConfig_RecirPressureLTH = atof(pTemp);
    //SplitByte.fData = stConfigData.fConfig_RecirPressureLTH;
    //printf("\n\rstConfigData.fConfig_RecirPressureLTH %f",stConfigData.fConfig_RecirPressureLTH);
   //fnSPI_EEPROM_WriteData(SCU_RECIRC_PRESSURE_THRESH_LOW, 4, SplitByte.rgucData);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"RecirculationMax")))
  {
    stConfigData.fConfig_RecirPressureHTH = atof(pTemp);
   // SplitByte.fData = stConfigData.fConfig_RecirPressureHTH;
    //printf("\n\rstConfigData.fConfig_RecirPressureHTH %f",stConfigData.fConfig_RecirPressureHTH);
   //fnSPI_EEPROM_WriteData(SCU_RECIRC_PRESSURE_THRESH_HIGH, 4, SplitByte.rgucData);
    ucConfigDataCount++;
  }
  
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"SpraySet")))
  {
    stConfigData.fConfig_SprayPressure = atof(pTemp);
   // SplitByte.fData = stConfigData.fConfig_SprayPressure;
    //printf("\n\rstConfigData.fConfig_SprayPressure %f",stConfigData.fConfig_SprayPressure);
   //fnSPI_EEPROM_WriteData(SCU_SPRAY_PRESSURE_VALUE, 4, SplitByte.rgucData);
    ucConfigDataCount++;
  }
  
  /****************************************************************/
  /*change17*/
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"SprayMin")))
  {
    stConfigData.fConfig_SprayPressureLTH = atof(pTemp);
   // SplitByte.fData = stConfigData.fConfig_SprayPressureLTH;
    //printf("\n\rstConfigData.fConfig_SprayPressureLTH %f",stConfigData.fConfig_SprayPressureLTH);
   //fnSPI_EEPROM_WriteData(SCU_SPRAY_PRESSURE_THRESH_LOW, 4, SplitByte.rgucData);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"SprayMax")))
  {
    stConfigData.fConfig_SprayPressureHTH = atof(pTemp);
    //SplitByte.fData = stConfigData.fConfig_SprayPressureHTH;
    //printf("\n\rstConfigData.fConfig_SprayPressureHTH %f",stConfigData.fConfig_SprayPressureHTH);
   //fnSPI_EEPROM_WriteData(SCU_SPRAY_PRESSURE_THRESH_HIGH, 4, SplitByte.rgucData);
    ucConfigDataCount++;
  }
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"ScuEepromBypass")))
  {
    if(strstr(pTemp,"Yes"))
      stConfigData.ucConfig_SCUBypass |= BIT0_SET;
    else if(strstr(pTemp,"No"))
      stConfigData.ucConfig_SCUBypass |= BIT0_RESET;
    
    /******************************************** SCU_BYPASS***************************************/
   //fnSPI_EEPROM_WriteData(SCU_BYPASS, 1, &stConfigData.ucConfig_SCUBypass);
    //printf("SCU_BYPASS = %d\r\n", stConfigData.ucConfig_SCUBypass);
    ucConfigDataCount++;
  }
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"ScuNvsramBypass")))
  {
    if(strstr(pTemp,"Yes"))
      stConfigData.ucConfig_SCUBypass |= BIT1_SET;
    else if(strstr(pTemp,"No"))
      stConfigData.ucConfig_SCUBypass |= BIT1_RESET;
    
    /******************************************** SCU_BYPASS***************************************/
   //fnSPI_EEPROM_WriteData(SCU_BYPASS, 1, &stConfigData.ucConfig_SCUBypass);
    //printf("SCU_BYPASS = %d\r\n", stConfigData.ucConfig_SCUBypass);
    ucConfigDataCount++;
  }
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"ScuPressureRegulatorBypass")))
  {
    if(strstr(pTemp,"Yes"))
      stConfigData.ucConfig_SCUBypass |= BIT2_SET;
    else if(strstr(pTemp,"No"))
      stConfigData.ucConfig_SCUBypass |= BIT2_RESET;
    
    /******************************************** SCU_BYPASS***************************************/
   //fnSPI_EEPROM_WriteData(SCU_BYPASS, 1, &stConfigData.ucConfig_SCUBypass);
    //printf("SCU_BYPASS = %d\r\n", stConfigData.ucConfig_SCUBypass);
    ucConfigDataCount++;
  }	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"ScuTempCheckBypass")))
  {
    if(strstr(pTemp,"Yes"))
      stConfigData.ucConfig_Temp_Press_Achieve_Bypass |= BIT0_SET;
    else if(strstr(pTemp,"No"))
      stConfigData.ucConfig_Temp_Press_Achieve_Bypass |= BIT0_RESET;
    
    /******************************************** TEMP_PRESS_ACHIEVE_BYPASS***************************************/
   //fnSPI_EEPROM_WriteData(TEMP_PRESS_ACHIEVE_BYPASS, 1, &stConfigData.ucConfig_Temp_Press_Achieve_Bypass);
    //printf("TEMP_PRESS_ACHIEVE_BYPASS = %d\r\n", stConfigData.ucConfig_Temp_Press_Achieve_Bypass);
    ucConfigDataCount++;
  }	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"ScuPressureCheckBypass")))
  {
    if(strstr(pTemp,"Yes"))
      stConfigData.ucConfig_Temp_Press_Achieve_Bypass |= BIT1_SET;
    else if(strstr(pTemp,"No"))
      stConfigData.ucConfig_Temp_Press_Achieve_Bypass |= BIT1_RESET;
    
    /******************************************** TEMP_PRESS_ACHIEVE_BYPASS***************************************/
   //fnSPI_EEPROM_WriteData(TEMP_PRESS_ACHIEVE_BYPASS, 1, &stConfigData.ucConfig_Temp_Press_Achieve_Bypass);
    //printf("TEMP_PRESS_ACHIEVE_BYPASS = %d\r\n", stConfigData.ucConfig_Temp_Press_Achieve_Bypass);
    ucConfigDataCount++;
  }	
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"OpmmSprayValveBypass")))
  {
    if(strstr(pTemp,"Yes"))
      stConfigData.uiConfig_OPMMBypass |= BIT0_SET;
    else if(strstr(pTemp,"No"))
      stConfigData.uiConfig_OPMMBypass |= BIT0_RESET;
    
    /******************************************** OPMM_BYPASS***************************************/
   // SplitByte.iData = stConfigData.uiConfig_OPMMBypass;
   //fnSPI_EEPROM_WriteData(OPMM_BYPASS, 2, SplitByte.rgucData);
    //printf("OPMM_BYPASS = %d\r\n", stConfigData.uiConfig_OPMMBypass);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"OpmmGunPosBypass")))
  {
    if(strstr(pTemp,"Yes"))
      stConfigData.uiConfig_OPMMBypass |= BIT1_SET;
    else if(strstr(pTemp,"No"))
      stConfigData.uiConfig_OPMMBypass |= BIT1_RESET;
    
    /******************************************** OPMM_BYPASS***************************************/
    //SplitByte.iData = stConfigData.uiConfig_OPMMBypass;
   //fnSPI_EEPROM_WriteData(OPMM_BYPASS, 2, SplitByte.rgucData);
    //printf("OPMM_BYPASS = %d\r\n", stConfigData.uiConfig_OPMMBypass);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"OpmmABValveBypass")))
  {
    if(strstr(pTemp,"Yes"))
      stConfigData.uiConfig_OPMMBypass |= BIT2_SET;
    else if(strstr(pTemp,"No"))
      stConfigData.uiConfig_OPMMBypass |= BIT2_RESET;
    
    /******************************************** OPMM_BYPASS***************************************/
   // SplitByte.iData = stConfigData.uiConfig_OPMMBypass;
   //fnSPI_EEPROM_WriteData(OPMM_BYPASS, 2, SplitByte.rgucData);
    //printf("OPMM_BYPASS = %d\r\n", stConfigData.uiConfig_OPMMBypass);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"OpmmSolventValveBypass")))
  {
    if(strstr(pTemp,"Yes"))
      stConfigData.uiConfig_OPMMBypass |= BIT3_SET;
    else if(strstr(pTemp,"No"))
      stConfigData.uiConfig_OPMMBypass |= BIT3_RESET;
    
    /******************************************** OPMM_BYPASS***************************************/
   // SplitByte.iData = stConfigData.uiConfig_OPMMBypass;
   //fnSPI_EEPROM_WriteData(OPMM_BYPASS, 2, SplitByte.rgucData);
    //printf("OPMM_BYPASS = %d\r\n", stConfigData.uiConfig_OPMMBypass);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"OpmmForwardValveBypass")))
  {
    if(strstr(pTemp,"Yes"))
      stConfigData.uiConfig_OPMMBypass |= BIT4_SET;
    else if(strstr(pTemp,"No"))
      stConfigData.uiConfig_OPMMBypass |= BIT4_RESET;
    
    /******************************************** OPMM_BYPASS***************************************/
   // SplitByte.iData = stConfigData.uiConfig_OPMMBypass;
   //fnSPI_EEPROM_WriteData(OPMM_BYPASS, 2, SplitByte.rgucData);
    //printf("OPMM_BYPASS = %d\r\n", stConfigData.uiConfig_OPMMBypass);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"OpmmBackwardValveBypass")))
  {
    if(strstr(pTemp,"Yes"))
      stConfigData.uiConfig_OPMMBypass |= BIT5_SET;
    else if(strstr(pTemp,"No"))
      stConfigData.uiConfig_OPMMBypass |= BIT5_RESET;
    
    /******************************************** OPMM_BYPASS***************************************/
   // SplitByte.iData = stConfigData.uiConfig_OPMMBypass;
   //fnSPI_EEPROM_WriteData(OPMM_BYPASS, 2, SplitByte.rgucData);
    //printf("OPMM_BYPASS = %d\r\n", stConfigData.uiConfig_OPMMBypass);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"OpmmNCtemp3Bypass")))
  {
    if(strstr(pTemp,"Yes"))
      stConfigData.uiConfig_OPMMBypass |= BIT8_SET;
    else if(strstr(pTemp,"No"))
      stConfigData.uiConfig_OPMMBypass |= BIT8_RESET;
    
    /******************************************** OPMM_BYPASS***************************************/
    //SplitByte.iData = stConfigData.uiConfig_OPMMBypass;
   //fnSPI_EEPROM_WriteData(OPMM_BYPASS, 2, SplitByte.rgucData);
    //printf("OPMM_BYPASS = %d\r\n", stConfigData.uiConfig_OPMMBypass);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"OpmmNCtemp6Bypass")))
  {
    if(strstr(pTemp,"Yes"))
      stConfigData.uiConfig_OPMMBypass |= BIT9_SET;
    else if(strstr(pTemp,"No"))
      stConfigData.uiConfig_OPMMBypass |= BIT9_RESET;
    
    /******************************************** OPMM_BYPASS***************************************/
   // SplitByte.iData = stConfigData.uiConfig_OPMMBypass;
   //fnSPI_EEPROM_WriteData(OPMM_BYPASS, 2, SplitByte.rgucData);
    //printf("OPMM_BYPASS = %d\r\n", stConfigData.uiConfig_OPMMBypass);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"OpmmNCtemp9Bypass")))
  {
    if(strstr(pTemp,"Yes"))
      stConfigData.uiConfig_OPMMBypass |= BIT10_SET;
    else if(strstr(pTemp,"No"))
      stConfigData.uiConfig_OPMMBypass |= BIT10_RESET;
    
    /******************************************** OPMM_BYPASS***************************************/
    //SplitByte.iData = stConfigData.uiConfig_OPMMBypass;
   //fnSPI_EEPROM_WriteData(OPMM_BYPASS, 2, SplitByte.rgucData);
    //printf("OPMM_BYPASS = %d\r\n", stConfigData.uiConfig_OPMMBypass);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"OpmmNCtemp12Bypass")))
  {
    if(strstr(pTemp,"Yes"))
      stConfigData.uiConfig_OPMMBypass |= BIT11_SET;
    else if(strstr(pTemp,"No"))
      stConfigData.uiConfig_OPMMBypass |= BIT11_RESET;
    
    /******************************************** OPMM_BYPASS***************************************/
    //SplitByte.iData = stConfigData.uiConfig_OPMMBypass;
   //fnSPI_EEPROM_WriteData(OPMM_BYPASS, 2, SplitByte.rgucData);
    //printf("OPMM_BYPASS = %d\r\n", stConfigData.uiConfig_OPMMBypass);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"OpmmBaseThermoBypass")))
  {
    if(strstr(pTemp,"Yes"))
      stConfigData.uiConfig_OPMMBypass |= BIT12_SET;
    else if(strstr(pTemp,"No"))
      stConfigData.uiConfig_OPMMBypass |= BIT12_RESET;
    
    /******************************************** OPMM_BYPASS***************************************/
    //SplitByte.iData = stConfigData.uiConfig_OPMMBypass;
   //fnSPI_EEPROM_WriteData(OPMM_BYPASS, 2, SplitByte.rgucData);
    //printf("OPMM_BYPASS = %d\r\n", stConfigData.uiConfig_OPMMBypass);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"OpmmHardThermoBypass")))
  {
    if(strstr(pTemp,"Yes"))
      stConfigData.uiConfig_OPMMBypass |= BIT13_SET;
    else if(strstr(pTemp,"No"))
      stConfigData.uiConfig_OPMMBypass |= BIT13_RESET;
    
    /******************************************** OPMM_BYPASS***************************************/
    //SplitByte.iData = stConfigData.uiConfig_OPMMBypass;
   //fnSPI_EEPROM_WriteData(OPMM_BYPASS, 2, SplitByte.rgucData);
    //printf("OPMM_BYPASS = %d\r\n", stConfigData.uiConfig_OPMMBypass);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"VcmLinearTransducerBypass")))
  {
    if(strstr(pTemp,"Yes"))
      stConfigData.ucConfig_VCMBypass |= BIT0_SET;
    else if(strstr(pTemp,"No"))
      stConfigData.ucConfig_VCMBypass |= BIT0_RESET;
    
    /******************************************** VCM_BYPASS***************************************/
   //fnSPI_EEPROM_WriteData(VCM_BYPASS, 1, &stConfigData.ucConfig_VCMBypass);
    //printf("VCM_BYPASS = %d\r\n", stConfigData.ucConfig_VCMBypass);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"VcmPressureGauge1Bypass")))
  {
    if(strstr(pTemp,"Yes"))
      stConfigData.ucConfig_VCMBypass |= BIT1_SET;
    else if(strstr(pTemp,"No"))
      stConfigData.ucConfig_VCMBypass |= BIT1_RESET;
    
    /******************************************** VCM_BYPASS***************************************/
   //fnSPI_EEPROM_WriteData(VCM_BYPASS, 1, &stConfigData.ucConfig_VCMBypass);
    //printf("VCM_BYPASS = %d\r\n", stConfigData.ucConfig_VCMBypass);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"VcmPressureGauge2Bypass")))
  {
    if(strstr(pTemp,"Yes"))
      stConfigData.ucConfig_VCMBypass |= BIT2_SET;
    else if(strstr(pTemp,"No"))
      stConfigData.ucConfig_VCMBypass |= BIT2_RESET;
    
    /******************************************** VCM_BYPASS***************************************/
   //fnSPI_EEPROM_WriteData(VCM_BYPASS, 1, &stConfigData.ucConfig_VCMBypass);
    //printf("VCM_BYPASS = %d\r\n", stConfigData.ucConfig_VCMBypass);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"VcmPressureGauge3Bypass")))
  {
    if(strstr(pTemp,"Yes"))
      stConfigData.ucConfig_VCMBypass |= BIT3_SET;
    else if(strstr(pTemp,"No"))
      stConfigData.ucConfig_VCMBypass |= BIT3_RESET;
    
    /******************************************** VCM_BYPASS***************************************/
   //fnSPI_EEPROM_WriteData(VCM_BYPASS, 1, &stConfigData.ucConfig_VCMBypass);
    //printf("VCM_BYPASS = %d\r\n", stConfigData.ucConfig_VCMBypass);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"VcmStallValveBypass")))
  {
    if(strstr(pTemp,"Yes"))
      stConfigData.ucConfig_VCMBypass |= BIT4_SET;
    else if(strstr(pTemp,"No"))
      stConfigData.ucConfig_VCMBypass |= BIT4_RESET;
    
    /******************************************** VCM_BYPASS***************************************/
   //fnSPI_EEPROM_WriteData(VCM_BYPASS, 1, &stConfigData.ucConfig_VCMBypass);
    //printf("VCM_BYPASS = %d\r\n", stConfigData.ucConfig_VCMBypass);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"VcmAmbientSensorInBypass")))
  {
    if(strstr(pTemp,"Yes"))
      stConfigData.ucConfig_VCMBypass |= BIT5_SET;
    else if(strstr(pTemp,"No"))
      stConfigData.ucConfig_VCMBypass |= BIT5_RESET;
    
    /******************************************** VCM_BYPASS***************************************/
   //fnSPI_EEPROM_WriteData(VCM_BYPASS, 1, &stConfigData.ucConfig_VCMBypass);
    //printf("VCM_BYPASS = %d\r\n", stConfigData.ucConfig_VCMBypass);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"VcmAmbientSensorOutBypass")))
  {
    if(strstr(pTemp,"Yes"))
      stConfigData.ucConfig_VCMBypass |= BIT6_SET;
    else if(strstr(pTemp,"No"))
      stConfigData.ucConfig_VCMBypass |= BIT6_RESET;
    
    /******************************************** VCM_BYPASS***************************************/
   //fnSPI_EEPROM_WriteData(VCM_BYPASS, 1, &stConfigData.ucConfig_VCMBypass);
    //printf("VCM_BYPASS = %d\r\n", stConfigData.ucConfig_VCMBypass);
    ucConfigDataCount++;
  }
	 /****************************************************************/
  /*change54*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"VcmRecirculationValveBypass")))
  {
    if(strstr(pTemp,"Yes"))
      stConfigData.ucConfig_VCMBypass |= BIT7_SET;
    else if(strstr(pTemp,"No"))
      stConfigData.ucConfig_VCMBypass |= BIT7_RESET;
    
    /******************************************** VCM_BYPASS***************************************/
   //fnSPI_EEPROM_WriteData(VCM_BYPASS, 1, &stConfigData.ucConfig_VCMBypass);
    //printf("VCM_BYPASS = %d\r\n", stConfigData.ucConfig_VCMBypass);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"HcmInlineBaseHeaterBypass")))
  {
    if(strstr(pTemp,"Yes"))
      stConfigData.uiConfig_HCMBypass |= BIT0_SET;
    else if(strstr(pTemp,"No"))
      stConfigData.uiConfig_HCMBypass |= BIT0_RESET;
    
    /******************************************** HCM_BYPASS***************************************/
    //SplitByte.iData = stConfigData.uiConfig_HCMBypass;
   //fnSPI_EEPROM_WriteData(HCM_BYPASS, 2, SplitByte.rgucData);
    //printf("HCM_BYPASS = %d\r\n", stConfigData.uiConfig_HCMBypass);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"HcmInlineHardHeaterBypass")))
  {
    if(strstr(pTemp,"Yes"))
      stConfigData.uiConfig_HCMBypass |= BIT1_SET;
    else if(strstr(pTemp,"No"))
      stConfigData.uiConfig_HCMBypass |= BIT1_RESET;
    
    /******************************************** HCM_BYPASS***************************************/
    //SplitByte.iData = stConfigData.uiConfig_HCMBypass;
   //fnSPI_EEPROM_WriteData(HCM_BYPASS, 2, SplitByte.rgucData);
    //printf("HCM_BYPASS = %d\r\n", stConfigData.uiConfig_HCMBypass);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"HcmBaseHeaterBypass")))
  {
    if(strstr(pTemp,"Yes"))
      stConfigData.uiConfig_HCMBypass |= BIT2_SET;
    else if(strstr(pTemp,"No"))
      stConfigData.uiConfig_HCMBypass |= BIT2_RESET;
    
    /******************************************** HCM_BYPASS***************************************/
    //SplitByte.iData = stConfigData.uiConfig_HCMBypass;
   //fnSPI_EEPROM_WriteData(HCM_BYPASS, 2, SplitByte.rgucData);
    //printf("HCM_BYPASS = %d\r\n", stConfigData.uiConfig_HCMBypass);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"HcmHardnerHeaterBypass")))
  {
    if(strstr(pTemp,"Yes"))
      stConfigData.uiConfig_HCMBypass |= BIT3_SET;
    else if(strstr(pTemp,"No"))
      stConfigData.uiConfig_HCMBypass |= BIT3_RESET;
    
    /******************************************** HCM_BYPASS***************************************/
    //SplitByte.iData = stConfigData.uiConfig_HCMBypass;
   //fnSPI_EEPROM_WriteData(HCM_BYPASS, 2, SplitByte.rgucData);
    //printf("HCM_BYPASS = %d\r\n", stConfigData.uiConfig_HCMBypass);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"HcmInlineBaseThermocoupleBypass")))
  {
    if(strstr(pTemp,"Yes"))
      stConfigData.uiConfig_HCMBypass |= BIT4_SET;
    else if(strstr(pTemp,"No"))
      stConfigData.uiConfig_HCMBypass |= BIT4_RESET;
    
    /******************************************** HCM_BYPASS***************************************/
   // SplitByte.iData = stConfigData.uiConfig_HCMBypass;
   //fnSPI_EEPROM_WriteData(HCM_BYPASS, 2, SplitByte.rgucData);
    //printf("HCM_BYPASS = %d\r\n", stConfigData.uiConfig_HCMBypass);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"HcmInlineHardThermocoupleBypass")))
  {
    if(strstr(pTemp,"Yes"))
      stConfigData.uiConfig_HCMBypass |= BIT5_SET;
    else if(strstr(pTemp,"No"))
      stConfigData.uiConfig_HCMBypass |= BIT5_RESET;
    
    /******************************************** HCM_BYPASS***************************************/
    //SplitByte.iData = stConfigData.uiConfig_HCMBypass;
    //fnSPI_EEPROM_WriteData(HCM_BYPASS, 2, SplitByte.rgucData);
    //printf("HCM_BYPASS = %d\r\n", stConfigData.uiConfig_HCMBypass);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"HcmBaseReturnlineThermocoupleBypass")))
  {
    if(strstr(pTemp,"Yes"))
      stConfigData.uiConfig_HCMBypass |= BIT6_SET;
    else if(strstr(pTemp,"No"))
      stConfigData.uiConfig_HCMBypass |= BIT6_RESET;
    
    /******************************************** HCM_BYPASS***************************************/
   // SplitByte.iData = stConfigData.uiConfig_HCMBypass;
   //fnSPI_EEPROM_WriteData(HCM_BYPASS, 2, SplitByte.rgucData);
    //printf("HCM_BYPASS = %d\r\n", stConfigData.uiConfig_HCMBypass);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"HcmBaseThermocoupleBypass")))
  {
    if(strstr(pTemp,"Yes"))
      stConfigData.uiConfig_HCMBypass |= BIT7_SET;
    else if(strstr(pTemp,"No"))
      stConfigData.uiConfig_HCMBypass |= BIT7_RESET;
    
    /******************************************** HCM_BYPASS***************************************/
    //SplitByte.iData = stConfigData.uiConfig_HCMBypass;
    //fnSPI_EEPROM_WriteData(HCM_BYPASS, 2, SplitByte.rgucData);
    //printf("HCM_BYPASS = %d\r\n", stConfigData.uiConfig_HCMBypass);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"HcmLiquidLevelBaseBypass")))
  {
    if(strstr(pTemp,"Yes"))
      stConfigData.uiConfig_HCMBypass |= BIT8_SET;
    else if(strstr(pTemp,"No"))
      stConfigData.uiConfig_HCMBypass |= BIT8_RESET;
    
    /******************************************** HCM_BYPASS***************************************/
    //SplitByte.iData = stConfigData.uiConfig_HCMBypass;
   //fnSPI_EEPROM_WriteData(HCM_BYPASS, 2, SplitByte.rgucData);
    //printf("HCM_BYPASS = %d\r\n", stConfigData.uiConfig_HCMBypass);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"HcmLiquidLevelHardBypass")))
  {
    if(strstr(pTemp,"Yes"))
      stConfigData.uiConfig_HCMBypass |= BIT9_SET;
    else if(strstr(pTemp,"No"))
      stConfigData.uiConfig_HCMBypass |= BIT9_RESET;
    
    /******************************************** HCM_BYPASS***************************************/
   // SplitByte.iData = stConfigData.uiConfig_HCMBypass;
   //fnSPI_EEPROM_WriteData(HCM_BYPASS, 2, SplitByte.rgucData);
    //printf("HCM_BYPASS = %d\r\n", stConfigData.uiConfig_HCMBypass);
    ucConfigDataCount++;
  }
  
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"ConnSprayvalve")))
  {
    stConfigData.ucConfig_ConnSprayvalve = atoi(pTemp);
    //printf("\n\rstConfigData.ucConfig_ConnSprayvalve %d",stConfigData.ucConfig_ConnSprayvalve);
   //fnSPI_EEPROM_WriteData(OPMM_SPRAY_VALVE_CONN, 1, &stConfigData.ucConfig_ConnSprayvalve);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"ConnGunposition")))
  {
    stConfigData.ucConfig_ConnGunposition = atoi(pTemp);
    //printf("\n\rstConfigData.ucConfig_ConnGunposition %d",stConfigData.ucConfig_ConnGunposition);
   //fnSPI_EEPROM_WriteData(OPMM_GUNPOS_VALVE_CONN, 1, &stConfigData.ucConfig_ConnGunposition);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"ConnABvalve")))
  {
    stConfigData.ucConfig_ConnABvalve = atoi(pTemp);
    //printf("\n\rstConfigData.ucConfig_ConnABvalve %d",stConfigData.ucConfig_ConnABvalve);
   //fnSPI_EEPROM_WriteData(OPMM_AB_VALVE_CONN, 1, &stConfigData.ucConfig_ConnABvalve);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"ConnSolventvalve")))
  {
    stConfigData.ucConfig_ConnSolventvalve = atoi(pTemp);
    //printf("\n\rstConfigData.ucConfig_ConnSolventvalve %d",stConfigData.ucConfig_ConnSolventvalve);
   //fnSPI_EEPROM_WriteData(OPMM_SOLVENT_VALVE_CONN, 1, &stConfigData.ucConfig_ConnSolventvalve);
    ucConfigDataCount++;
  }
  
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"ConnForwardvalve")))
  {
    stConfigData.ucConfig_ConnForwardvalve = atoi(pTemp);
    //printf("\n\rstConfigData.ucConfig_ConnForwardvalve %d",stConfigData.ucConfig_ConnForwardvalve);
   //fnSPI_EEPROM_WriteData(OPMM_FORWARD_VALVE_CONN, 1, &stConfigData.ucConfig_ConnForwardvalve);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"ConnBackwardvalve")))
  {
    stConfigData.ucConfig_ConnBackwardvalve = atoi(pTemp);
    //printf("\n\rstConfigData.ucConfig_ConnBackwardvalve %d",stConfigData.ucConfig_ConnBackwardvalve);
   //fnSPI_EEPROM_WriteData(OPMM_BACKWARD_VALVE_CONN, 1, &stConfigData.ucConfig_ConnBackwardvalve);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"ConnNonContactat12")))
  {
    stConfigData.ucConfig_ConnNonContactat12 = atoi(pTemp);
    //printf("\n\rstConfigData.ucConfig_ConnNonContactat12 %d",stConfigData.ucConfig_ConnNonContactat12);
   //fnSPI_EEPROM_WriteData(OPMM_NON_CONTACT12_CONN, 1, &stConfigData.ucConfig_ConnNonContactat12);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/		 
	if(NULL != (pTemp = fnGetDataElement(XMLstring,"ConnNonContactat3")))
  {
    stConfigData.ucConfig_ConnNonContactat3 = atoi(pTemp);
    //printf("\n\rstConfigData.ucConfig_ConnNonContactat3 %d",stConfigData.ucConfig_ConnNonContactat3);
   //fnSPI_EEPROM_WriteData(OPMM_NON_CONTACT3_CONN, 1, &stConfigData.ucConfig_ConnNonContactat3);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"ConnNonContactat6")))
  {
    stConfigData.ucConfig_ConnNonContactat6 = atoi(pTemp);
    //printf("\n\rstConfigData.ucConfig_ConnNonContactat6 %d",stConfigData.ucConfig_ConnNonContactat6);
   //fnSPI_EEPROM_WriteData(OPMM_NON_CONTACT6_CONN, 1, &stConfigData.ucConfig_ConnNonContactat6);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"ConnNonContactat9")))
  {
    stConfigData.ucConfig_ConnNonContactat9 = atoi(pTemp);
    //printf("\n\rstConfigData.ucConfig_ConnNonContactat9 %d",stConfigData.ucConfig_ConnNonContactat9);
   //fnSPI_EEPROM_WriteData(OPMM_NON_CONTACT9_CONN, 1, &stConfigData.ucConfig_ConnNonContactat9);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"ConnPressureGuage1")))
  {
    stConfigData.ucConfig_ConnPressureGuage1 = atoi(pTemp);
    //printf("\n\rstConfigData.ucConfig_ConnPressureGuage1 %d",stConfigData.ucConfig_ConnPressureGuage1);
   //fnSPI_EEPROM_WriteData(VCM_PRESSURE_GAUGE1_CONN, 1, &stConfigData.ucConfig_ConnPressureGuage1); 
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"ConnPressureGuage2")))
  {
    stConfigData.ucConfig_ConnPressureGuage2 = atoi(pTemp);
    //printf("\n\rstConfigData.ucConfig_ConnPressureGuage2 %d",stConfigData.ucConfig_ConnPressureGuage2);
   //fnSPI_EEPROM_WriteData(VCM_PRESSURE_GAUGE2_CONN, 1, &stConfigData.ucConfig_ConnPressureGuage2);
    ucConfigDataCount++; 
  }
  /****************************************************************/
  /*change17*/
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"ConnPressureGuage3")))
  {
    stConfigData.ucConfig_ConnPressureGuage3 = atoi(pTemp);
    //printf("\n\rstConfigData.ucConfig_ConnPressureGuage3 %d",stConfigData.ucConfig_ConnPressureGuage3);
   //fnSPI_EEPROM_WriteData(VCM_PRESSURE_GAUGE3_CONN, 1, &stConfigData.ucConfig_ConnPressureGuage3); 
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"ConnLinearTransducer")))
  {
    stConfigData.ucConfig_ConnLinearTransducer = atoi(pTemp);
    //printf("\n\rstConfigData.ucConfig_ConnLinearTransducer %d",stConfigData.ucConfig_ConnLinearTransducer);
   //fnSPI_EEPROM_WriteData(VCM_LINEAR_TRANSDUCER_CONN, 1, &stConfigData.ucConfig_ConnLinearTransducer); 
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"ConnInsideEE07")))
  {
    stConfigData.ucConfig_ConnInsideEE07 = atoi(pTemp);
    //printf("\n\rstConfigData.ucConfig_ConnInsideEE07 %d",stConfigData.ucConfig_ConnInsideEE07);
   //fnSPI_EEPROM_WriteData(VCM_EE07_INSIDE_CONN, 1, &stConfigData.ucConfig_ConnInsideEE07);
    ucConfigDataCount++; 
  }
  /****************************************************************/
  /*change17*/
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"ConnOutsideEE07")))
  {
    stConfigData.ucConfig_ConnOutsideEE07 = atoi(pTemp);
    //printf("\n\rstConfigData.ucConfig_ConnOutsideEE07 %d",stConfigData.ucConfig_ConnOutsideEE07);
   //fnSPI_EEPROM_WriteData(VCM_EE07_OUTSIDE_CONN, 1, &stConfigData.ucConfig_ConnOutsideEE07); 
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"ConnStallvalve")))
  {
    stConfigData.ucConfig_ConnStallvalve = atoi(pTemp);
    //printf("\n\rstConfigData.ucConfig_ConnStallvalve %d",stConfigData.ucConfig_ConnStallvalve);
   //fnSPI_EEPROM_WriteData(VCM_STALL_VALVE_CONN, 1, &stConfigData.ucConfig_ConnStallvalve); 
    ucConfigDataCount++;
  }
	/****************************************************************/
  /*change54*/
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"ConnRecirculationvalve")))
  {
    stConfigData.ucConfig_ConnRecirculationvalve = atoi(pTemp);
    //printf("\n\rstConfigData.ucConfig_ConnStallvalve %d",stConfigData.ucConfig_ConnStallvalve);
   //fnSPI_EEPROM_WriteData(VCM_STALL_VALVE_CONN, 1, &stConfigData.ucConfig_ConnStallvalve); 
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"ConnBaseinlineTher")))
  {
    stConfigData.ucConfig_ConnBaseinlinether = atoi(pTemp);
    //printf("\n\rstConfigData.ucConfig_ConnBaseinlinether %d",stConfigData.ucConfig_ConnBaseinlinether);
   //fnSPI_EEPROM_WriteData(HCM_INLINE_BASE_THERM_CONN, 1, &stConfigData.ucConfig_ConnBaseinlinether);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"ConnHardinlineTher")))
  {
    stConfigData.ucConfig_ConnHardinlinether = atoi(pTemp);
    //printf("\n\rstConfigData.ucConfig_ConnHardinlinether %d",stConfigData.ucConfig_ConnHardinlinether);
   //fnSPI_EEPROM_WriteData(HCM_INLINE_HARD_THERM_CONN, 1, &stConfigData.ucConfig_ConnHardinlinether);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"ConnAntifreezeTher")))
  {
    stConfigData.ucConfig_ConnAntifreezether = atoi(pTemp);
    //printf("\n\rstConfigData.ucConfig_ConnAntifreezether %d",stConfigData.ucConfig_ConnAntifreezether);
    //fnSPI_EEPROM_WriteData(HCM_INLINE_ANTIFREEZE_THERM_CONN, 1, &stConfigData.ucConfig_ConnAntifreezether);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"ConnBasereturnlineTher")))
  {
    stConfigData.ucConfig_ConnBasereturnlinether = atoi(pTemp);
    //printf("\n\rstConfigData.ucConfig_ConnBasereturnlinether %d",stConfigData.ucConfig_ConnBasereturnlinether);
    //fnSPI_EEPROM_WriteData(HCM_RETURN_BASE_THERM_CONN, 1, &stConfigData.ucConfig_ConnBasereturnlinether);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"ConnBaseinlineHeater")))
  {
    stConfigData.ucConfig_ConnBaseinlineheater = atoi(pTemp);
    //printf("\n\rstConfigData.ucConfig_ConnBaseinlineheater %d",stConfigData.ucConfig_ConnBaseinlineheater);
    ////fnSPI_EEPROM_WriteData(HCM_INLINE_BASE_HEATER_CONN, 1, &stConfigData.ucConfig_ConnBaseinlineheater);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"ConnHardinlineHeater")))
  {
    stConfigData.ucConfig_ConnHardinlineheater = atoi(pTemp);
    //printf("\n\rstConfigData.ucConfig_ConnHardinlineheater %d",stConfigData.ucConfig_ConnHardinlineheater);
    ////fnSPI_EEPROM_WriteData(HCM_INLINE_HARD_HEATER_CONN, 1, &stConfigData.ucConfig_ConnHardinlineheater);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"ConnAntifreezeHeater")))
  {
    stConfigData.ucConfig_ConnAntifreezeheater = atoi(pTemp);
    //printf("\n\rstConfigData.ucConfig_ConnAntifreezeheater %d",stConfigData.ucConfig_ConnAntifreezeheater);
    //////fnSPI_EEPROM_WriteData(HCM_INLINE_ANTIFREEZE_HEATER_CONN, 1, &stConfigData.ucConfig_ConnAntifreezeheater);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"ConnHardHeater")))
  {
    stConfigData.ucConfig_ConnHardheater = atoi(pTemp);
    //printf("\n\rstConfigData.ucConfig_ConnHardheater %d",stConfigData.ucConfig_ConnHardheater);
    ////fnSPI_EEPROM_WriteData(HCM_HARD_HEATER_CONN, 1, &stConfigData.ucConfig_ConnHardheater);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"ConnBaseTankLiqLvl")))
  {
    stConfigData.ucConfig_ConnBasetankLL = atoi(pTemp);
    //printf("\n\rstConfigData.ucConfig_ConnBasetankLL %d",stConfigData.ucConfig_ConnBasetankLL);
    ////fnSPI_EEPROM_WriteData(HCM_BASE_LIQUID_LEVEL_CONN, 1, &stConfigData.ucConfig_ConnBasetankLL);
    ucConfigDataCount++;
  }
  /****************************************************************/
  /*change17*/	
  if(NULL != (pTemp = fnGetDataElement(XMLstring,"ConnHardTankLiqLvl")))
  {
    stConfigData.ucConfig_ConnHardtankLL = atoi(pTemp);
    //printf("\n\rstConfigData.ucConfig_ConnHardtankLL %d",stConfigData.ucConfig_ConnHardtankLL);
    //fnSPI_EEPROM_WriteData(HCM_HARD_LIQUID_LEVEL_CONN, 1, &stConfigData.ucConfig_ConnHardtankLL);
    ucConfigDataCount++;
  }
  
	if(ucConfigDataCount >= 155)//156
	{
		fnSPI_EEPROM_WriteData(SCU_UNIT_TYPE, 1, &stConfigData.ucConfig_TypeofUnit);
    SplitByte.fData = 0.0f;
		SplitByte.fData = stConfigData.fConfig_BaseTankHeight;
    fnSPI_EEPROM_WriteData(HCM_BASE_TANK_HEIGHT, 4, SplitByte.rgucData);
    SplitByte.fData = 0.0f;
		SplitByte.fData = stConfigData.fConfig_HardTankHeight;
    fnSPI_EEPROM_WriteData(HCM_HARD_TANK_HEIGHT, 4, SplitByte.rgucData);
    SplitByte.fData = 0.0f;
		SplitByte.fData = stConfigData.fConfig_HardLiquidLevelLTH;
    fnSPI_EEPROM_WriteData(HCM_HARD_LIQUID_THRESH_LEVEL_LOW, 4, SplitByte.rgucData);
    SplitByte.fData = 0.0f;
		SplitByte.fData = stConfigData.fConfig_HardLiquidLevelHTH;
    fnSPI_EEPROM_WriteData(HCM_HARD_LIQUID_THRESH_LEVEL_HIGH, 4, SplitByte.rgucData);
    SplitByte.fData = 0.0f;
		SplitByte.fData = stConfigData.fConfig_BaseLiquidLevelLTH;
    fnSPI_EEPROM_WriteData(HCM_BASE_LIQUID_THRESH_LEVEL_LOW, 4, SplitByte.rgucData);
    SplitByte.fData = 0.0f;
		SplitByte.fData = stConfigData.fConfig_BaseLiquidLevelHTH;
    fnSPI_EEPROM_WriteData(HCM_BASE_LIQUID_THRESH_LEVEL_HIGH, 4, SplitByte.rgucData);
    SplitByte.iData = 0;
		SplitByte.iData = stConfigData.uiConfig_LLSamplingRate;
    fnSPI_EEPROM_WriteData(HCM_LIQUID_LEVEL_SAMPLING_RATE, 2, SplitByte.rgucData);
    SplitByte.fData = 0.0f;
		SplitByte.fData = stConfigData.fConfig_AntiFrzTempSet;
    fnSPI_EEPROM_WriteData(HCM_ANTI_SET_TEMP, 4, SplitByte.rgucData);
    SplitByte.fData = 0.0f;
		SplitByte.fData = stConfigData.fConfig_AntiFrzTempLTH;
    fnSPI_EEPROM_WriteData(HCM_ANTI_THRESH_TEMP_LOW, 4, SplitByte.rgucData);
    SplitByte.fData = 0.0f;
		SplitByte.fData = stConfigData.fConfig_AntiFrzTempHTH;
    fnSPI_EEPROM_WriteData(HCM_ANTI_THRESH_TEMP_HIGH, 4, SplitByte.rgucData);
    SplitByte.fData = 0.0f;
		SplitByte.fData = stConfigData.fConfig_InlineHardTempSet;
    fnSPI_EEPROM_WriteData(HCM_INLINE_HARD_SET_TEMP, 4, SplitByte.rgucData);		
    SplitByte.fData = 0.0f;
		SplitByte.fData = stConfigData.fConfig_InlineHardTempLTH;
    fnSPI_EEPROM_WriteData(HCM_INLINE_HARD_THRESH_TEMP_LOW, 4, SplitByte.rgucData);
    SplitByte.fData = 0.0f;
		SplitByte.fData = stConfigData.fConfig_InlineHardTempHTH;
    fnSPI_EEPROM_WriteData(HCM_INLINE_HARD_THRESH_TEMP_HIGH, 4, SplitByte.rgucData);
    SplitByte.fData = 0.0f;
		SplitByte.fData = stConfigData.fConfig_InlineBaseTempSet;
    fnSPI_EEPROM_WriteData(HCM_INLINE_BASE_SET_TEMP, 4, SplitByte.rgucData);
    SplitByte.fData = 0.0f;
		SplitByte.fData = stConfigData.fConfig_InlineBaseTempLTH;
    fnSPI_EEPROM_WriteData(HCM_INLINE_BASE_THRESH_TEMP_LOW, 4, SplitByte.rgucData);
    SplitByte.fData = 0.0f;
		SplitByte.fData = stConfigData.fConfig_InlineBaseTempHTH;
    fnSPI_EEPROM_WriteData(HCM_INLINE_BASE_THRESH_TEMP_HIGH, 4, SplitByte.rgucData);
    SplitByte.fData = 0.0f;
		SplitByte.fData = stConfigData.fConfig_PipeTempLTH ;
    fnSPI_EEPROM_WriteData(OPMM_PIPE_TEMP_THRES_LOW, 4, SplitByte.rgucData);
    SplitByte.fData = 0.0f;
		SplitByte.fData = stConfigData.fConfig_PipeTempHTH ;
    fnSPI_EEPROM_WriteData(OPMM_PIPE_TEMP_THRES_HIGH, 4, SplitByte.rgucData);
    SplitByte.fData = 0.0f;
		SplitByte.fData = stConfigData.fConfig_DCLSTime;
    fnSPI_EEPROM_WriteData(OPMM_DCLS_HOME_REACH_TIME, 4, SplitByte.rgucData);
    SplitByte.fData = 0.0f;
		SplitByte.fData = stConfigData.fConfig_HOMETime;
    fnSPI_EEPROM_WriteData(OPMM_HOME_DCLS_REACH_TIME, 4, SplitByte.rgucData);
    SplitByte.fData = 0.0f;
		SplitByte.fData = stConfigData.fConfig_TimeStopOPMMcommand;
    fnSPI_EEPROM_WriteData(OPMM_STOP_HLS_TIMER, 4, SplitByte.rgucData);
    fnSPI_EEPROM_WriteData(SYS_TIME, 7, stConfigData.ucConfig_SystemTime);
    SplitByte.fData = 0.0f;
		SplitByte.fData = stConfigData.fConfig_DataLoggingPeriod;
    fnSPI_EEPROM_WriteData(TIME_DATA_LOGGING, 4, SplitByte.rgucData);
    SplitByte.fData = 0.0f;
		SplitByte.fData = stConfigData.fConfig_TimeBeforeStall;
    fnSPI_EEPROM_WriteData(VCM_BEFORE_STALL, 4, SplitByte.rgucData);
    SplitByte.fData = 0.0f;
		SplitByte.fData = stConfigData.fConfig_TimeCompareStall;
    fnSPI_EEPROM_WriteData(VCM_COMPARE_STALL, 4, SplitByte.rgucData);
    SplitByte.fData = 0.0f;
		SplitByte.fData = stConfigData.fConfig_GunPosChangeTime;
    fnSPI_EEPROM_WriteData(OPMM_GUN_POSITION_CHANGE_TIME, 4, SplitByte.rgucData);
    SplitByte.fData = 0.0f;
		SplitByte.fData = stConfigData.fConfig_TimebeforeONSprayValve;
    fnSPI_EEPROM_WriteData(OPMM_DELAY_SPRAY_VALVE_FLUSH_VBAT_TIME, 4, SplitByte.rgucData);
    SplitByte.fData = 0.0f;
		SplitByte.fData = stConfigData.fConfig_FlushWarnTime;
    fnSPI_EEPROM_WriteData(OPMM_FLUSH_WARNNING_TIME, 4, SplitByte.rgucData);
    SplitByte.fData = 0.0f;
		SplitByte.fData = stConfigData.fConfig_TimeBeforeSendingStallcommand;
    fnSPI_EEPROM_WriteData(SCU_TIME_SEND_STALL_COMMAND, 4, SplitByte.rgucData);
    SplitByte.fData = 0.0f;
		SplitByte.fData = stConfigData.fConfig_PrimeTime ;
    fnSPI_EEPROM_WriteData(OPMM_PRIME_TIME, 4, SplitByte.rgucData);
    SplitByte.fData = 0.0f;
		SplitByte.fData = stConfigData.fConfig_GunDelayTime;
    fnSPI_EEPROM_WriteData(OPMM_GUN_DELAY_TIME, 4, SplitByte.rgucData);	
    SplitByte.fData = 0.0f;
		SplitByte.fData = stConfigData.fConfig_SprayOverlapTime;
    fnSPI_EEPROM_WriteData(OPMM_SPRAY_OVERLAPTIME, 4, SplitByte.rgucData);
    SplitByte.fData = 0.0f;
		SplitByte.fData = stConfigData.fConfig_ReverseDelayTime;
    fnSPI_EEPROM_WriteData(OPMM_REVERSE_DELAYTIME, 4, SplitByte.rgucData);
    SplitByte.fData = 0.0f;
		SplitByte.fData = stConfigData.fConfig_FlushTime;
    fnSPI_EEPROM_WriteData(OPMM_FLUSH_TIME, 4, SplitByte.rgucData);
    SplitByte.fData = 0.0f;
		SplitByte.fData = stConfigData.fConfig_IdleTimePeriodicLog;
    fnSPI_EEPROM_WriteData(IDLE_DATA_LOGGING, 4, SplitByte.rgucData);
    SplitByte.fData = 0.0f;
		SplitByte.fData = stConfigData.fConfig_AreaofCylinderA;
    fnSPI_EEPROM_WriteData(VCM_CYLINDER_AREA1, 4, SplitByte.rgucData);
    SplitByte.fData = 0.0f;
		SplitByte.fData = stConfigData.fConfig_AreaofCylinderB ;
    fnSPI_EEPROM_WriteData(VCM_CYLINDER_AREA2, 4, SplitByte.rgucData);
    SplitByte.fData = 0.0f;
		SplitByte.fData = stConfigData.fConfig_AreaofCylinderC;
    fnSPI_EEPROM_WriteData(VCM_CYLINDER_AREA3, 4, SplitByte.rgucData);
    fnSPI_EEPROM_WriteData(VCM_CYLINDER_COUNT, 1, &stConfigData.ucConfig_CylinderCount);
    fnSPI_EEPROM_WriteData(OPMM_SPRAY_COUNT, 1, &stConfigData.ucConfig_SprayCount);
    fnSPI_EEPROM_WriteData(OPMM_BUZZER_VOLUME, 1, &stConfigData.ucConfig_BuzzerVolume);
    SplitByte.uiData = 0;
		SplitByte.uiData = stConfigData.uiConfig_Signature;
    fnSPI_EEPROM_WriteData(SIGNATURE_START, SIGNATUREVALUE_SIZE, SplitByte.rgucData);	
    SplitByte.uiData = uiBakSignature;
    fnSPI_EEPROM_WriteData(SIGNATURE_BACKUP, SIGNATUREVALUE_SIZE, SplitByte.rgucData);	
    SplitByte.fData = 0.0f;
		SplitByte.fData = stConfigData.fConfig_BatteryKneeVoltage;
    fnSPI_EEPROM_WriteData(OPMM_BATT_KNEE_VOLT, 4, SplitByte.rgucData);
    SplitByte.fData = 0.0f;
		SplitByte.fData = stConfigData.fConfig_PipeDiameter;
    fnSPI_EEPROM_WriteData(PIPE_DIAMETER, 4, SplitByte.rgucData);
    SplitByte.fData = 0.0f;
    SplitByte.fData = stConfigData.fConfig_JointNum;
    fnSPI_EEPROM_WriteData(COUNT_JOINT, 4, SplitByte.rgucData);
    fnSPI_EEPROM_WriteData(SCU_WIFI_SSIDADDRESS, 15, stWifiIP.rgucSSID);
    fnSPI_EEPROM_WriteData(SCU_WIFI_PASSADDRESS, 15, stWifiIP.rgucPASWD);
    fnSPI_EEPROM_WriteData(SCU_WIFI_IPADDRESS, 15, stWifiIP.rgucIP_Addr);
    fnSPI_EEPROM_WriteData(SCU_WIFI_SMADDRESS, 15, stWifiIP.rgucSubnet_Addr);
    fnSPI_EEPROM_WriteData(SCU_WIFI_GWADDRESS, 15, stWifiIP.rgucGateway_Addr);
    SplitByte.uiData = 0;
    SplitByte.uiData = stWifiIP.uiPort;
    fnSPI_EEPROM_WriteData(SCU_WIFI_PORTADDRESS, 2, SplitByte.rgucData);
    fnSPI_EEPROM_WriteData(SCU_ETH_IPADDRESS, 15, stEthernetIP.rgucIP_Addr);
    fnSPI_EEPROM_WriteData(SCU_ETH_SMADDRESS, 15, stEthernetIP.rgucSubnet_Addr);
    fnSPI_EEPROM_WriteData(SCU_ETH_GWADDRESS, 15, stEthernetIP.rgucGateway_Addr);
    SplitByte.uiData = 0.0f;
		SplitByte.uiData = stEthernetIP.uiPort;
    fnSPI_EEPROM_WriteData(SCU_ETH_PORTADDRESS, 2, SplitByte.rgucData);
    SplitByte.fData = 0.0f;
		SplitByte.fData = stConfigData.fConfig_HysAntiFrzPos;
    fnSPI_EEPROM_WriteData(HCM_ANTI_HYST_TEMP_POS, 4, SplitByte.rgucData);
    SplitByte.fData = 0.0f;
		SplitByte.fData = stConfigData.fConfig_HysAntiFrzNeg;
    fnSPI_EEPROM_WriteData(HCM_ANTI_HYST_TEMP_NEG, 4, SplitByte.rgucData);
    SplitByte.fData = 0.0f;
		SplitByte.fData = stConfigData.fConfig_InlineHysHardPos;
    fnSPI_EEPROM_WriteData(HCM_INLINE_HARD_HYST_TEMP_POS, 4, SplitByte.rgucData);
    SplitByte.fData = 0.0f;
		SplitByte.fData = stConfigData.fConfig_InlineHysHardNeg;
    fnSPI_EEPROM_WriteData(HCM_INLINE_HARD_HYST_TEMP_NEG, 4, SplitByte.rgucData);
    SplitByte.fData = 0.0f;
		SplitByte.fData = stConfigData.fConfig_HysInlineBasePos;
    fnSPI_EEPROM_WriteData(HCM_INLINE_BASE_HYST_TEMP_POS, 4, SplitByte.rgucData);
    SplitByte.fData = 0.0f;
		SplitByte.fData = stConfigData.fConfig_HysInlineBaseNeg;
    fnSPI_EEPROM_WriteData(HCM_INLINE_BASE_HYST_TEMP_NEG, 4, SplitByte.rgucData);
    SplitByte.fData = 0.0f;
		SplitByte.fData = stConfigData.fConfig_HysSprayPresPos;
    fnSPI_EEPROM_WriteData(SCU_SPRAY_PRESSURE_HYST_POS, 4, SplitByte.rgucData);
    SplitByte.fData = 0.0f;
		SplitByte.fData = stConfigData.fConfig_HysSprayPresNeg;
    fnSPI_EEPROM_WriteData(SCU_SPRAY_PRESSURE_HYST_NEG, 4, SplitByte.rgucData);
    SplitByte.fData = 0.0f;
		SplitByte.fData = stConfigData.fConfig_HysRecircPresPos;
    fnSPI_EEPROM_WriteData(SCU_RECIRC_PRESSURE_HYST_POS, 4, SplitByte.rgucData);
    SplitByte.fData = 0.0f;
    SplitByte.fData = stConfigData.fConfig_HysRecircPresNeg;
    fnSPI_EEPROM_WriteData(SCU_RECIRC_PRESSURE_HYST_NEG, 4, SplitByte.rgucData);
    SplitByte.iData = 0;
		SplitByte.iData = stConfigData.iConfig_UpStrokeTH;
    fnSPI_EEPROM_WriteData(VCM_UPSTROKE_ADD, 2, SplitByte.rgucData);
    SplitByte.iData = 0;
		SplitByte.iData = stConfigData.iConfig_DownStrokeTH;
    fnSPI_EEPROM_WriteData(VCM_DOWNSTROKE_ADD, 2, SplitByte.rgucData);
    fnSPI_EEPROM_WriteData(VCM_STALL_UPSTROKE_UPPER_LIMIT, 1, &stConfigData.ucConfig_UpstrokeUpLimit); 
    fnSPI_EEPROM_WriteData(VCM_STALL_UPSTROKE_MAX_RANGE, 1, &stConfigData.ucConfig_UpstrokeMaxRange); 
    fnSPI_EEPROM_WriteData(VCM_STALL_UPSTROKE_MIN_RANGE, 1, &stConfigData.ucConfig_UpstrokeMinRange); 
    fnSPI_EEPROM_WriteData(VCM_STALL_DOWNSTROKE_UPPER_LIMIT, 1, &stConfigData.ucConfig_DownstrokeUpLimit); 
    fnSPI_EEPROM_WriteData(VCM_STALL_DOWNSTROKE_MAX_RANGE, 1, &stConfigData.ucConfig_DownstrokeMaxRange); 
    fnSPI_EEPROM_WriteData(VCM_STALL_DOWNSTROKE_MIN_RANGE, 1, &stConfigData.ucConfig_DownstrokeMinRange);
	
    fnSPI_EEPROM_WriteData(SUPERVISOR_NAME1, 7, stConfigData.rgucConfig_SupervisorName1);
    fnSPI_EEPROM_WriteData(SUPERVISOR_NAME2, 7, stConfigData.rgucConfig_SupervisorName2);
    fnSPI_EEPROM_WriteData(SUPERVISOR_NAME3, 7, stConfigData.rgucConfig_SupervisorName3);
   	fnSPI_EEPROM_WriteData(COATING_MAT_NAME1, 7, stConfigData.rgucConfig_CoatingMatName1);
    fnSPI_EEPROM_WriteData(COATING_MAT_NAME2, 7, stConfigData.rgucConfig_CoatingMatName2);
    fnSPI_EEPROM_WriteData(COATING_MAT_NAME3, 7, stConfigData.rgucConfig_CoatingMatName3);
    fnSPI_EEPROM_WriteData(BASE_MAT_BATCHNO1, 7, stConfigData.rgucConfig_BaseMatBatchNo1);
    fnSPI_EEPROM_WriteData(BASE_MAT_BATCHNO2, 7, stConfigData.rgucConfig_BaseMatBatchNo2);
    fnSPI_EEPROM_WriteData(BASE_MAT_BATCHNO3, 7, stConfigData.rgucConfig_BaseMatBatchNo3);
    fnSPI_EEPROM_WriteData(HARD_MAT_BATCHNO1, 7, stConfigData.rgucConfig_HardMatBatchNo1);
    fnSPI_EEPROM_WriteData(HARD_MAT_BATCHNO2, 7, stConfigData.rgucConfig_HardMatBatchNo2);
    fnSPI_EEPROM_WriteData(HARD_MAT_BATCHNO3, 7, stConfigData.rgucConfig_HardMatBatchNo3);
    fnSPI_EEPROM_WriteData(JOB_NAME1, 7, stConfigData.rgucConfig_JobName1);
    fnSPI_EEPROM_WriteData(JOB_NAME2, 7, stConfigData.rgucConfig_JobName2);
    fnSPI_EEPROM_WriteData(JOB_NAME3, 7, stConfigData.rgucConfig_JobName3);
    fnSPI_EEPROM_WriteData(JOB_ID1, 7, stConfigData.rgucConfig_JobId1);
    fnSPI_EEPROM_WriteData(JOB_ID2, 7, stConfigData.rgucConfig_JobId2);
    fnSPI_EEPROM_WriteData(JOB_ID3, 7, stConfigData.rgucConfig_JobId3);
    SplitByte.fData = 0.0f;
		SplitByte.fData = stConfigData.fConfig_PressureGauge1LTH;
    fnSPI_EEPROM_WriteData(VCM_PRESSURE_LOWTH1, 4, SplitByte.rgucData);
    SplitByte.fData = 0.0f;
		SplitByte.fData = stConfigData.fConfig_PressureGauge1HTH;
    fnSPI_EEPROM_WriteData(VCM_PRESSURE_HIGHTH1, 4, SplitByte.rgucData);
    SplitByte.fData = 0.0f;
		SplitByte.fData = stConfigData.fConfig_PressureGauge2LTH;
    fnSPI_EEPROM_WriteData(VCM_PRESSURE_LOWTH2, 4, SplitByte.rgucData);
    SplitByte.fData = 0.0f;
		SplitByte.fData = stConfigData.fConfig_PressureGauge2HTH;
    fnSPI_EEPROM_WriteData(VCM_PRESSURE_HIGHTH2, 4, SplitByte.rgucData);
    SplitByte.fData = 0.0f;
    SplitByte.fData = stConfigData.fConfig_PressureGauge3LTH ;
    fnSPI_EEPROM_WriteData(VCM_PRESSURE_LOWTH3, 4, SplitByte.rgucData);
    SplitByte.fData = 0.0f;
    SplitByte.fData = stConfigData.fConfig_PressureGauge3HTH;
    fnSPI_EEPROM_WriteData(VCM_PRESSURE_HIGHTH3, 4, SplitByte.rgucData);
    SplitByte.fData = 0.0f;
    SplitByte.fData = stConfigData.fConfig_RecirPressure;
    fnSPI_EEPROM_WriteData(SCU_RECIRC_PRESSURE_VALUE, 4, SplitByte.rgucData);
    SplitByte.fData = 0.0f;
    SplitByte.fData = stConfigData.fConfig_RecirPressureLTH;
    fnSPI_EEPROM_WriteData(SCU_RECIRC_PRESSURE_THRESH_LOW, 4, SplitByte.rgucData);
    SplitByte.fData = 0.0f;
    SplitByte.fData = stConfigData.fConfig_RecirPressureHTH;
    fnSPI_EEPROM_WriteData(SCU_RECIRC_PRESSURE_THRESH_HIGH, 4, SplitByte.rgucData);
    SplitByte.fData = 0.0f;
    SplitByte.fData = stConfigData.fConfig_SprayPressure;
    fnSPI_EEPROM_WriteData(SCU_SPRAY_PRESSURE_VALUE, 4, SplitByte.rgucData);
    SplitByte.fData = 0.0f;
    SplitByte.fData = stConfigData.fConfig_SprayPressureLTH;
    fnSPI_EEPROM_WriteData(SCU_SPRAY_PRESSURE_THRESH_LOW, 4, SplitByte.rgucData);
    SplitByte.fData = 0.0f;
    SplitByte.fData = stConfigData.fConfig_SprayPressureHTH;
    fnSPI_EEPROM_WriteData(SCU_SPRAY_PRESSURE_THRESH_HIGH, 4, SplitByte.rgucData);
    fnSPI_EEPROM_WriteData(SCU_BYPASS, 1, &stConfigData.ucConfig_SCUBypass);
    fnSPI_EEPROM_WriteData(TEMP_PRESS_ACHIEVE_BYPASS, 1, &stConfigData.ucConfig_Temp_Press_Achieve_Bypass);
    SplitByte.iData = 0;
		SplitByte.iData = stConfigData.uiConfig_OPMMBypass;
		fnSPI_EEPROM_WriteData(OPMM_BYPASS, 2, SplitByte.rgucData);
    fnSPI_EEPROM_WriteData(VCM_BYPASS, 1, &stConfigData.ucConfig_VCMBypass);
		SplitByte.iData = 0;
		SplitByte.iData = stConfigData.uiConfig_HCMBypass;
    fnSPI_EEPROM_WriteData(HCM_BYPASS, 2, SplitByte.rgucData);
    fnSPI_EEPROM_WriteData(OPMM_SPRAY_VALVE_CONN, 1, &stConfigData.ucConfig_ConnSprayvalve);
    fnSPI_EEPROM_WriteData(OPMM_GUNPOS_VALVE_CONN, 1, &stConfigData.ucConfig_ConnGunposition);
    fnSPI_EEPROM_WriteData(OPMM_AB_VALVE_CONN, 1, &stConfigData.ucConfig_ConnABvalve);
    fnSPI_EEPROM_WriteData(OPMM_SOLVENT_VALVE_CONN, 1, &stConfigData.ucConfig_ConnSolventvalve);
    fnSPI_EEPROM_WriteData(OPMM_FORWARD_VALVE_CONN, 1, &stConfigData.ucConfig_ConnForwardvalve);
    fnSPI_EEPROM_WriteData(OPMM_BACKWARD_VALVE_CONN, 1, &stConfigData.ucConfig_ConnBackwardvalve);
    fnSPI_EEPROM_WriteData(OPMM_NON_CONTACT12_CONN, 1, &stConfigData.ucConfig_ConnNonContactat12);
    fnSPI_EEPROM_WriteData(OPMM_NON_CONTACT3_CONN, 1, &stConfigData.ucConfig_ConnNonContactat3);
    fnSPI_EEPROM_WriteData(OPMM_NON_CONTACT6_CONN, 1, &stConfigData.ucConfig_ConnNonContactat6);
    fnSPI_EEPROM_WriteData(OPMM_NON_CONTACT9_CONN, 1, &stConfigData.ucConfig_ConnNonContactat9);
    fnSPI_EEPROM_WriteData(VCM_PRESSURE_GAUGE1_CONN, 1, &stConfigData.ucConfig_ConnPressureGuage1); 
    fnSPI_EEPROM_WriteData(VCM_PRESSURE_GAUGE2_CONN, 1, &stConfigData.ucConfig_ConnPressureGuage2);
    fnSPI_EEPROM_WriteData(VCM_PRESSURE_GAUGE3_CONN, 1, &stConfigData.ucConfig_ConnPressureGuage3); 
    fnSPI_EEPROM_WriteData(VCM_LINEAR_TRANSDUCER_CONN, 1, &stConfigData.ucConfig_ConnLinearTransducer); 
    fnSPI_EEPROM_WriteData(VCM_EE07_INSIDE_CONN, 1, &stConfigData.ucConfig_ConnInsideEE07);
    fnSPI_EEPROM_WriteData(VCM_EE07_OUTSIDE_CONN, 1, &stConfigData.ucConfig_ConnOutsideEE07); 
    fnSPI_EEPROM_WriteData(VCM_STALL_VALVE_CONN, 1, &stConfigData.ucConfig_ConnStallvalve); 
		fnSPI_EEPROM_WriteData(VCM_RECIRC_VALVE_CONN, 1, &stConfigData.ucConfig_ConnRecirculationvalve);/*change54*/
    fnSPI_EEPROM_WriteData(HCM_INLINE_BASE_THERM_CONN, 1, &stConfigData.ucConfig_ConnBaseinlinether);
    fnSPI_EEPROM_WriteData(HCM_INLINE_HARD_THERM_CONN, 1, &stConfigData.ucConfig_ConnHardinlinether);
    fnSPI_EEPROM_WriteData(HCM_INLINE_ANTIFREEZE_THERM_CONN, 1, &stConfigData.ucConfig_ConnAntifreezether);
    fnSPI_EEPROM_WriteData(HCM_RETURN_BASE_THERM_CONN, 1, &stConfigData.ucConfig_ConnBasereturnlinether);
    fnSPI_EEPROM_WriteData(HCM_INLINE_BASE_HEATER_CONN, 1, &stConfigData.ucConfig_ConnBaseinlineheater);
    fnSPI_EEPROM_WriteData(HCM_INLINE_HARD_HEATER_CONN, 1, &stConfigData.ucConfig_ConnHardinlineheater);
    fnSPI_EEPROM_WriteData(HCM_INLINE_ANTIFREEZE_HEATER_CONN, 1, &stConfigData.ucConfig_ConnAntifreezeheater);
    fnSPI_EEPROM_WriteData(HCM_HARD_HEATER_CONN, 1, &stConfigData.ucConfig_ConnHardheater);
    fnSPI_EEPROM_WriteData(HCM_BASE_LIQUID_LEVEL_CONN, 1, &stConfigData.ucConfig_ConnBasetankLL);
		fnSPI_EEPROM_WriteData(HCM_HARD_LIQUID_LEVEL_CONN, 1, &stConfigData.ucConfig_ConnHardtankLL);

		fnSPI_EEPROM_WriteData(HMI_TIME_OFFSET_SIGN, 1, &stConfigData.ucConfig_TimeOffsetSign); 
		fnSPI_EEPROM_WriteData(HMI_TIME_OFFSET_HOUR, 1, &stConfigData.ucConfig_TimeOffsetHour); 
		fnSPI_EEPROM_WriteData(HMI_TIME_OFFSET_MIN, 1, &stConfigData.ucConfig_TimeOffsetMin); 
		fnSPI_EEPROM_WriteData(HMI_TIME_OFFSET_SEC, 1, &stConfigData.ucConfig_TimeOffsetSec); 

		/* Check repeat index change78*/
		uiJobnameCnt = 300;
		uiJobIDCnt = 300;
		uiSupervisornameCnt =300;
		fnCheck_Repeat_Index();


		/*Read Config Data from EEPROM */
		fnConfigData_Read();
		/*Set flag if command recieved from Wifi*/
		fnSendHMI_Config();
		
		stTx1Message.ExtId = 0x15000000 | ACKNOWLEDGEMENT |
			DEST_TABLET | SOURCE_SCU;	
		/*Update Length of CAN Data Frame*/
		stTx1Message.DLC = 1;
		stTx1Message.Data[0] = 0x00;		/* Positive ACk */
		fnXML_FIFO(&stTx1Message);
		
//		/****************************************************************/
//		/*change17*/
//		if(SET == ucWifiChanges)
//		{
//			if((!strcmp((char *)stWifiRxIP.rgucIP_Addr,(char *)stWifiIP.rgucIP_Addr)) ||
//				!strcmp((char *)stWifiRxIP.rgucSubnet_Addr,(char *)stWifiIP.rgucSubnet_Addr) ||
//					!strcmp((char *)stWifiRxIP.rgucGateway_Addr,(char *)stWifiIP.rgucGateway_Addr) ||
//						!strcmp((char *)stWifiRxIP.rgucSSID,(char *)stWifiIP.rgucSSID) ||
//							!strcmp((char *)stWifiRxIP.rgucPASWD,(char *)stWifiIP.rgucPASWD) ||
//								(stWifiRxIP.uiPort != stWifiIP.uiPort))
//			uint8_t rgucConfig[100];
//			ucWifiChanges = RESET;
//			fnWifiCommandMode();
//			strcpy((char *)stWifiRxIP.rgucIP_Addr,(char *)stWifiIP.rgucIP_Addr);
//			sprintf((char *)rgucConfig,"set ip addr %s",(char *)stWifiIP.rgucIP_Addr);
//			fnWifiCommand(rgucConfig);
//			
//			strcpy((char *)stWifiRxIP.rgucSubnet_Addr,(char *)stWifiIP.rgucSubnet_Addr);
//			sprintf((char *)rgucConfig,"set ip net %s",(char *)stWifiIP.rgucSubnet_Addr);
//			fnWifiCommand(rgucConfig);
//			
//			strcpy((char *)stWifiRxIP.rgucGateway_Addr,(char *)stWifiIP.rgucGateway_Addr);
//			sprintf((char *)rgucConfig,"set ip gateway %s",(char *)stWifiIP.rgucGateway_Addr);
//			fnWifiCommand(rgucConfig);
//			
//			stWifiRxIP.uiPort=stWifiIP.uiPort;
//			sprintf((char *)rgucConfig,"set ip localport %d",stWifiIP.uiPort);
//			fnWifiCommand(rgucConfig);
//			
//			strcpy((char *)stWifiRxIP.rgucSSID,(char *)stWifiIP.rgucSSID);
//			sprintf((char *)rgucConfig,"set apmode ssid %s",(char *)stWifiIP.rgucSSID);
//			fnWifiCommand(rgucConfig);
//			
//			strcpy((char *)stWifiRxIP.rgucPASWD,(char *)stWifiIP.rgucPASWD);
//			sprintf((char *)rgucConfig,"set apmode passphrase %s",(char *)stWifiIP.rgucPASWD);
//			fnWifiCommand(rgucConfig);
//			
//			fnWifiCommand("save");
//			
//			fnWifiCommand("reboot");
//			
//			}
			
			if(SET == ucETH_Changes)
			{
				ucETH_Changes = RESET;
				/*Ethernet configuration*/
				fnEthernetConfig();
			}
	}
	else
	{
			stTx1Message.ExtId = 0x15000000 | 0x00E70000 |
				DEST_TABLET | SOURCE_SCU;	
			/*Update Length of CAN Data Frame*/
			stTx1Message.DLC = 1;
			stTx1Message.Data[0] = 0x00;		/* Positive ACk */
			fnXML_FIFO(&stTx1Message);
	}
	
//	printf("\n\rCOUNT + %d",ucConfigDataCount);
  ucConfigDataCount=0;
	stWifi.uiReceiveTimer = RESET;
}	


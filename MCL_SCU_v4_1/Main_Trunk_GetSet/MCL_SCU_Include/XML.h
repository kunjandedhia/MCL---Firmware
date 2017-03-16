/******************************************************************************
**@file     :   XML.h
* 
* @brief  This File Contain all the MACROS and extern and global Funtion and Variable Decleration 
******************************************************************************
*/
#ifndef __XML_H__
#define __XML_H__
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
 extern XML_t stXML; 
 extern IPconfig_t stWifiRxIP;
 extern IPconfig_t stWifiIP;
 extern uint8_t rgucXMLtxBuffer[TRANSMIT_BUFFER];
/* ----------------------------------------------------------------------------
*                           EXTERNAL FUNCTIONS
* ----------------------------------------------------------------------------
*/
// extern void fnFormatXML(uint8_t *pucElement,uint8_t *pucString);
 extern void fnXMLCmdProcess(uint8_t *pucString);
 extern void fnWriteStartElement(uint8_t *pucElement);
 extern void fnWriteEndElement(uint8_t *pucElement);
 extern void fnWriteData(uint8_t *pucString);
 extern void fnXML_FIFO(CanTxMsg *pstCANTxmsg);
// extern void fnXMLConfigProcess(uint8_t *pucString);
 extern void fnXML_DataTransmit(void);
// extern void fnGetConfigParameter(void);
 extern void fnSetConfigParameter(const uint8_t* XMLstring);
// extern void fnWifiGetConfigParameter(void);
 extern void fnXMLCmdAck(CanRxMsg *pstCANrxmsg);
 extern void fnWriteMessageID(uint32_t uliMessageID);
 extern void fnWriteLength(uint8_t ucLength);

#endif /* __XML_H__ */

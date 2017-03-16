/******************************************************************************
**@file     :  Datalog_Tx.c
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


 DATALOG_TX_t stWifiLog;
 DATALOG_TX_t stEthernetLog;
 /*****************************************************************************
**@Function 		  : 	fnGetDateTimeInSec
**@Descriptions	: 	get start and stop time for log
**@parameters		: 	XML string
**@return				: 	None
*****************************************************************************/
uint32_t fnGetDateTimeInSec(DateTime_t stDateTime)
{
	return((stDateTime.ucYear * 12 * 31 * 24 * 60 * 60) + 
	(stDateTime.ucMonth * 31 * 24 * 60 * 60) +
	(stDateTime.ucDate * 24 * 60 * 60) +
	(stDateTime.ucHour * 60 * 60) +
	(stDateTime.ucMin * 60) +
	(stDateTime.ucSec));
}
/*****************************************************************************
**@Function 		  : 	fnGetLogStartStop
**@Descriptions	: 	get start and stop time for log
**@parameters		: 	XML string
**@return				: 	None
*****************************************************************************/
void fnGetLogStartStop(const uint8_t* XMLstring,DATALOG_TX_t *pstLog)
{
  char *pTemp1,*pTemp2;
  static char rgcFiledata[100];//, rgcAttribute1[500],rgcAttribute2[500];
  if((pTemp1 = strstr((char *)XMLstring,"<Start>")) != NULL)
	{
		if((pTemp2 = strstr(pTemp1,"</Start>")) != NULL)
		{
			memset(rgcFiledata,'\0',strlen(rgcFiledata));
			strncpy(rgcFiledata,(pTemp1+strlen("<Start>")),(pTemp2-(pTemp1+strlen("<Start>"))));
			if(strstr(rgcFiledata,"at Time"))
			{
				if((pTemp1 = strstr((char *)XMLstring,"<Start_Time>")) != NULL)
				{
					if((pTemp2 = strstr(pTemp1,"</Start_Time>")) != NULL)
					{
						memset(rgcFiledata,'\0',strlen(rgcFiledata));
						strncpy(rgcFiledata,(pTemp1+strlen("<Start_Time>")),(pTemp2-(pTemp1+strlen("<Start_Time>"))));
						pTemp1 = strtok(rgcFiledata,"/");
						pstLog->stStartLogDateTime.ucYear = atoi(pTemp1)-2000;
						pTemp1 = strstr(rgcFiledata,"/");
						pTemp2 = strtok(pTemp1,"/");
						pstLog->stStartLogDateTime.ucMonth = atoi(pTemp2);
						pTemp1 = strstr(rgcFiledata,"/");
						pTemp2 = strtok(pTemp1," ");
						pstLog->stStartLogDateTime.ucDate = atoi(pTemp2);
						pTemp1 = strstr(rgcFiledata," ");
						pTemp2 = strtok(pTemp1,":");
						pstLog->stStartLogDateTime.ucHour = atoi(pTemp2);
						pTemp1 = strstr(rgcFiledata,":");
						pTemp2 = strtok(pTemp1,":");
						pstLog->stStartLogDateTime.ucMin = atoi(pTemp2);
						pTemp1 = strstr(rgcFiledata,":");
						pTemp2 = strtok(pTemp1,"\0");
						pstLog->stStartLogDateTime.ucSec = atoi(pTemp2);
						pstLog->stStartLogDateTime.uliTotalSec = fnGetDateTimeInSec(pstLog->stStartLogDateTime);
					}
				}
			}
			else if(strstr(rgcFiledata,"at Beginning of Log"))
			{
				printf("\n\rat Beginning of Log");
				pstLog->stStartLogDateTime.uliTotalSec = 0;
			}
		}
	}
	if((pTemp1 = strstr((char *)XMLstring,"<Stop>")) != NULL)
	{
		if((pTemp2 = strstr(pTemp1,"</Stop>")) != NULL)
		{
			memset(rgcFiledata,'\0',strlen(rgcFiledata));
			strncpy(rgcFiledata,(pTemp1+strlen("<Stop>")),(pTemp2-(pTemp1+strlen("<Stop>"))));
			if(strstr(rgcFiledata,"at Time"))
			{
				if((pTemp1 = strstr((char *)XMLstring,"<Stop_Time>")) != NULL)
				{
					if((pTemp2 = strstr(pTemp1,"</Stop_Time>")) != NULL)
					{
						memset(rgcFiledata,'\0',strlen(rgcFiledata));
						strncpy(rgcFiledata,(pTemp1+strlen("<Stop_Time>")),(pTemp2-(pTemp1+strlen("<Stop_Time>"))));
						pTemp1 = strtok(rgcFiledata,"/");
						pstLog->stEndLogDateTime.ucYear = atoi(pTemp1)-2000;
						pTemp1 = strstr(rgcFiledata,"/");
						pTemp2 = strtok(pTemp1,"/");
						pstLog->stEndLogDateTime.ucMonth = atoi(pTemp2);
						pTemp1 = strstr(rgcFiledata,"/");
						pTemp2 = strtok(pTemp1," ");
						pstLog->stEndLogDateTime.ucDate = atoi(pTemp2);
						pTemp1 = strstr(rgcFiledata," ");
						pTemp2 = strtok(pTemp1,":");
						pstLog->stEndLogDateTime.ucHour = atoi(pTemp2);
						pTemp1 = strstr(rgcFiledata,":");
						pTemp2 = strtok(pTemp1,":");
						pstLog->stEndLogDateTime.ucMin = atoi(pTemp2);
						pTemp1 = strstr(rgcFiledata,":");
						pTemp2 = strtok(pTemp1,"\0");
						pstLog->stEndLogDateTime.ucSec = atoi(pTemp2);
						pstLog->stEndLogDateTime.uliTotalSec = fnGetDateTimeInSec(pstLog->stEndLogDateTime);
					}
				}
			}
			else if(strstr(rgcFiledata,"at End of Log"))
			{
				
				pstLog->stEndLogDateTime.uliTotalSec = 0xFFFFFFFF;
				printf("\n\rat End of Log %x",pstLog->stEndLogDateTime.uliTotalSec);
			}
		}
	}
}
/*****************************************************************************
**@Function 		: fnDatalogStartFrame
**@Descriptions	: Function write datalog start time stamp in XML format
**@Parameters		: None   
**@Return				: None      
*****************************************************************************/
void fnDatalogStartFrame(uint8_t ucUserID,uint8_t ucJobID)
{
  uint8_t rgucTempData[100];
//	uint8_t rgucDetailData[22];
  /*updatint the Date and Time from RTC registers*/
  RTC_GetTime(RTC_Format_BIN, &stRTC_TimeStructure);
  RTC_GetDate(RTC_Format_BIN, &stRTC_DateStructure);
  
  //memset(rgucXMLtxBuffer,'\0',strlen((char *)rgucXMLtxBuffer));
  
	fnWriteStartElement("Record internalName=\"Start\"");
  sprintf((char *)rgucTempData,"20%02d/%02d/%02d %02d:%02d:%02d ", stRTC_DateStructure.RTC_Year, 
          stRTC_DateStructure.RTC_Month, stRTC_DateStructure.RTC_Date, stRTC_TimeStructure.RTC_Hours, 
          stRTC_TimeStructure.RTC_Minutes, stRTC_TimeStructure.RTC_Seconds);
  fnWriteData(rgucTempData);
	
//	/*Send Job Name*/
//	fnRetriveJobName(ucJobID);	
//  memset(rgucTempData,'\0',strlen((char *)rgucTempData));
//  sprintf((char *)rgucTempData,"%s ",(char *)jobname);
//  fnWriteData(rgucTempData);

//	  /*Send Supervisor Name*/
//	fnRetriveSupervisorName(ucUserID);	
//  memset(rgucTempData,'\0',strlen((char *)rgucTempData));
//  sprintf((char *)rgucTempData,"%s ",(char *)supervisorname);
//  fnWriteData(rgucTempData);
  
  fnWriteEndElement("Record");
	
	fnWriteStartElement("Record internalName=\"Version\"");
	
	  /*SCU Firmware Version*/
  memset(rgucTempData,'\0',strlen((char *)rgucTempData));
  sprintf((char *)rgucTempData,"%f ",SCU_FIRMWARE_VERSION);
  fnWriteData(rgucTempData);
  
  /*HCM Firmware Version*/
  memset(rgucTempData,'\0',strlen((char *)rgucTempData));
  sprintf((char *)rgucTempData,"%f ",stCurrentData.fCurr_HCM_Firmware_Ver);
  fnWriteData(rgucTempData);
  
  /*VCM Firmware Version*/
  memset(rgucTempData,'\0',strlen((char *)rgucTempData));
  sprintf((char *)rgucTempData,"%f ",stCurrentData.fCurr_VCM_Firmware_Ver);
  fnWriteData(rgucTempData);
  
  /*HMI Firmware Version*/
  memset(rgucTempData,'\0',strlen((char *)rgucTempData));
  sprintf((char *)rgucTempData,"%f ",stCurrentData.fCurr_HMI_Firmware_Ver);
  fnWriteData(rgucTempData);
  
  /*OPMM Firmware Version*/
  memset(rgucTempData,'\0',strlen((char *)rgucTempData));
  sprintf((char *)rgucTempData,"%f ",stCurrentData.fCurr_OPMM_Firmware_Ver);
  fnWriteData(rgucTempData);
  
  fnWriteEndElement("Record");
}

/*****************************************************************************
**@Function 		: fnDatalogEndFrame
**@Descriptions	: Function write datalog end time stamp in XML format
**@Parameters		: None   
**@Return				: None      
*****************************************************************************/
void fnDatalogEndFrame(void)
{
  uint8_t rgucTempData[100];
  /*updatint the Date and Time from RTC registers*/
  RTC_GetTime(RTC_Format_BIN, &stRTC_TimeStructure);
  RTC_GetDate(RTC_Format_BIN, &stRTC_DateStructure);
  
  //memset(rgucXMLtxBuffer,'\0',strlen((char *)rgucXMLtxBuffer));
  
  fnWriteStartElement("Record internalName=\"End\"");
  sprintf((char *)rgucTempData,"20%02d/%02d/%02d %02d:%02d:%02d ", stRTC_DateStructure.RTC_Year, 
          stRTC_DateStructure.RTC_Month, stRTC_DateStructure.RTC_Date, stRTC_TimeStructure.RTC_Hours, 
          stRTC_TimeStructure.RTC_Minutes, stRTC_TimeStructure.RTC_Seconds);
  fnWriteData(rgucTempData);
  fnWriteEndElement("Record");
}
 /*****************************************************************************
**@Function			: fnDiagLog_Detail
**@Descriptions	: Append Diagnostic log info in XML format 
**@parameters		: None
**@return				: None      
*******************************************************************************/
void fnDiagLog_Detail(uint8_t *ucNextData)
{
  //memset(rgucXMLtxBuffer,'\0',strlen((char *)rgucXMLtxBuffer));
	switch(*ucNextData)
	{
		case 1:
			{
		
    fnWriteStartElement("CRC-Evans_Data");
    fnWriteStartElement("File_Identity displayName=\"Diagnostic Log\"");
    fnWriteData("Diagnostic Log");
    fnWriteEndElement("File_Identity");
    fnWriteStartElement("View");
		
		fnWriteStartElement("Record internalName=\"Version\" displayName=\"Firmware Version\"");
		fnWriteStartElement("Variable internalName=\"SCU_Firmware_Version\" displayName=\"SCU Firmware Version\" type=\"string\"/");
    fnWriteStartElement("Variable internalName=\"HCM_Firmware_Version\" displayName=\"HCM Firmware Version\" type=\"string\"/");
    fnWriteStartElement("Variable internalName=\"VCM_Firmware_Version\" displayName=\"VCM Firmware Version\" type=\"string\"/");
    fnWriteStartElement("Variable internalName=\"HMI_Firmware_Version\" displayName=\"HMI Firmware Version\" type=\"string\"/");
    fnWriteStartElement("Variable internalName=\"OPMM_Firmware_Version\" displayName=\"OPMM Firmware Version\" type=\"string\"/");
		fnWriteEndElement("Record");
		
    fnWriteStartElement("Record internalName=\"Start\" displayName=\"Start\"");
    fnWriteStartElement("Variable internalName=\"StartDate\" displayName=\"Date\" type=\"Date\" /");
    fnWriteStartElement("Variable internalName=\"StartTime\" displayName=\"Time\" type=\"Time\" /");
    fnWriteEndElement("Record");
    fnWriteStartElement("Record internalName=\"Measurement\" displayName=\"Measurement\"");
			*ucNextData = *ucNextData + 1;
			}
		break;
		case 2:
			{
    fnWriteStartElement("Variable internalName=\"MeasurementStartDate\" displayName=\"Date\" type=\"Date\" /");
    fnWriteStartElement("Variable internalName=\"MeasurementStartTime\" displayName=\"Time\" type=\"Time\" /");
    fnWriteStartElement("Variable internalName=\"SOMDiag\" displayName=\"SOM Diag\" type=\"string\" /");
    fnWriteStartElement("Variable internalName=\"EEPROM\" displayName=\"EEPROM\" type=\"string\" /");
    fnWriteStartElement("Variable internalName=\"NVSRAM\" displayName=\"NVSRAM\" type=\"string\" /");
    fnWriteStartElement("Variable internalName=\"PressureRegulator\" displayName=\"Pressure Regulator\" type=\"string\" /");
					*ucNextData = *ucNextData + 1;
			}
		break;
		case 3:
			{
    fnWriteStartElement("Variable internalName=\"HMIDiag\" displayName=\"HMI Diag\" type=\"string\" /");
    fnWriteStartElement("Variable internalName=\"NVSRAM\" displayName=\"NVSRAM\" type=\"string\" /");
    fnWriteStartElement("Variable internalName=\"PushONSwitch\" displayName=\"Push-to-ON Switch\" type=\"string\" /");
    fnWriteStartElement("Variable internalName=\"OPMMDiag\" displayName=\"OPMM Diag\" type=\"string\" /");
    fnWriteStartElement("Variable internalName=\"MotorFwdValveConn\" displayName=\"Motor Forward Valve Connected\" type=\"string\" /");
   				*ucNextData = *ucNextData + 1;
			}
		break;
		case 4:
			{
    fnWriteStartElement("Variable internalName=\"MotorBwdValveConn\" displayName=\"Motor Backward Valve Connected\" type=\"string\" /");
    fnWriteStartElement("Variable internalName=\"SolventFlushValveConn\" displayName=\"Solvent/Flush Valve Connected\" type=\"string\" /");
    fnWriteStartElement("Variable internalName=\"SprayGunConn\" displayName=\"Spray Gun Connected\" type=\"string\" /");
    fnWriteStartElement("Variable internalName=\"SprayHeadConn\" displayName=\"Spray head Connected\" type=\"string\" /");
    fnWriteStartElement("Variable internalName=\"ABMaterialValveConn\" displayName=\"A/B Material Valve Connected\" type=\"string\" /");
			*ucNextData = *ucNextData + 1;
			}
		break;
		case 5:
			{
    fnWriteStartElement("Variable internalName=\"MotorFwdValve\" displayName=\"Motor Forward Valve\" type=\"string\" /");
    fnWriteStartElement("Variable internalName=\"MotorBwdValve\" displayName=\"Motor Backward Valve\" type=\"string\" /");
    fnWriteStartElement("Variable internalName=\"SolventFlushValve\" displayName=\"Solvent/ Flush Valve\" type=\"string\" /");
    fnWriteStartElement("Variable internalName=\"SprayGun\" displayName=\"Spray Gun\" type=\"string\" /");
    fnWriteStartElement("Variable internalName=\"SprayHead\" displayName=\"Spray head\" type=\"string\" /");
			*ucNextData = *ucNextData + 1;
			}
		break;
		case 6:
			{
    fnWriteStartElement("Variable internalName=\"ABMaterialValve\" displayName=\"A/B Material Valve Connected\" type=\"string\" /");	
    fnWriteStartElement("Variable internalName=\"PT12\" displayName=\"Pipe Temp 12\" type=\"string\" /");	
    fnWriteStartElement("Variable internalName=\"PT3\" displayName=\"Pipe Temp 3\" type=\"string\" /");	
    fnWriteStartElement("Variable internalName=\"PT6\" displayName=\"Pipe Temp 6\" type=\"string\" /");	
    fnWriteStartElement("Variable internalName=\"PT9\" displayName=\"Pipe Temp 9\" type=\"string\" /");	
			*ucNextData = *ucNextData + 1;
			}
		break;
		case 7:
			{
    fnWriteStartElement("Variable internalName=\"BaseThermo\" displayName=\"Base Thermocouple\" type=\"string\" /");	
    fnWriteStartElement("Variable internalName=\"HardnerThermo\" displayName=\"Hardner Thermocouple\" type=\"string\" /");
    fnWriteStartElement("Variable internalName=\"VPMCDiag\" displayName=\"VPMC Diag\" type=\"string\" /");
    fnWriteStartElement("Variable internalName=\"LinearTransducer\" displayName=\"Linear Transducer\" type=\"string\" /");
    fnWriteStartElement("Variable internalName=\"PG1\" displayName=\"PG1\" type=\"string\" /");
    fnWriteStartElement("Variable internalName=\"PG2\" displayName=\"PG2\" type=\"string\" /");
			*ucNextData = *ucNextData + 1;
			}
		break;
		case 8:
			{
    fnWriteStartElement("Variable internalName=\"PG3\" displayName=\"PG3\" type=\"string\" /");
    fnWriteStartElement("Variable internalName=\"StallValueConn\" displayName=\"Stall Value Connected\" type=\"string\" /");
    fnWriteStartElement("Variable internalName=\"StallValue\" displayName=\"Stall Value\" type=\"string\" /");
    fnWriteStartElement("Variable internalName=\"TempHumidSensIN\" displayName=\"Temp Humidity Sensor Inside\" type=\"string\" /");
    fnWriteStartElement("Variable internalName=\"TempHumidSensOUT\" displayName=\"Temp Humidity Sensor Outside\" type=\"string\" /");
			*ucNextData = *ucNextData + 1;
			}
		break;
		case 9:
			{
    fnWriteStartElement("Variable internalName=\"HLCMDiag\" displayName=\"HLCM Diag\" type=\"string\" /");
    fnWriteStartElement("Variable internalName=\"BaseHeater\" displayName=\"Base Heater\" type=\"string\" /");
    fnWriteStartElement("Variable internalName=\"HardnerHeater\" displayName=\"Hardner Heater\" type=\"string\" /");
    fnWriteStartElement("Variable internalName=\"InlineBaseHeater\" displayName=\"Inline Base Heater\" type=\"string\" /");
    fnWriteStartElement("Variable internalName=\"InlineHardnerHeater\" displayName=\"Inline Hardner Heater\" type=\"string\" /");
			*ucNextData = *ucNextData + 1;
			}
		break;
		case 10:
			{
    fnWriteStartElement("Variable internalName=\"AntiFreezThermocouple\" displayName=\"Base Heater\" type=\"string\" /");
    fnWriteStartElement("Variable internalName=\"InlineBaseThermocouple\" displayName=\"Inline Base Thermocouple\" type=\"string\" /");
    fnWriteStartElement("Variable internalName=\"InlineHardnerThermocouple\" displayName=\"Inline Hardner Thermocouple\" type=\"string\" /");
    fnWriteStartElement("Variable internalName=\"BaseReturnlineThermocouple\" displayName=\"Base Returnline Thermocouple\" type=\"string\" /");
			*ucNextData = *ucNextData + 1;
			}
		break;
		case 11:
			{
    fnWriteStartElement("Variable internalName=\"LiquidLevelBase\" displayName=\"Liquid Level Base\" type=\"string\" /");
    fnWriteStartElement("Variable internalName=\"LiquidLevelHardner\" displayName=\"Liquid Level HArdner\" type=\"string\" /");
    fnWriteEndElement("Record");
    fnWriteStartElement("Record internalName=\"End\" displayName=\"End\"");
    fnWriteStartElement("Variable internalName=\"EndDate\" displayName=\"Date\" type=\"Date\" /");
    fnWriteStartElement("Variable internalName=\"EndTime\" displayName=\"Time\" type=\"Time\" /");
    fnWriteEndElement("Record");
    fnWriteEndElement("View");
    fnWriteStartElement("Data");
			*ucNextData = *ucNextData + 1;
			}
		break;
		default:
			{
				printf("\n\rDefault");
		*ucNextData = BYTE_ZERO;
  }
	}
}
/*****************************************************************************
**@Function			: fnDiagLog_Read
**@Descriptions	: Read the one Diagnostic data log from 
NVSRAM1 and send it to UART in CSV format.
**@parameters		: None
**@return				: None      
*******************************************************************************/
void fnDiagLog_Read(uint32_t uliStartAddres)
{
  uint8_t rgucTimeStamp[6];
  uint8_t rgucTempData[100];
  uint8_t ucReadData = 0;
  
  memset(rgucXMLtxBuffer,'\0',strlen((char *)rgucXMLtxBuffer));
  fnWriteStartElement("Record internalName=\"Measurement\"");
  memset(rgucTimeStamp,'\0',strlen((char *)rgucTimeStamp)); 
	memset(rgucTempData,'\0',strlen((char *)rgucTempData)); 
  /*Time Stamp read and Display on UART*/
  fnNVSRAM1_MultiDataRead(6, uliStartAddres, rgucTimeStamp);		
	
  sprintf((char *)rgucTempData,"20%02d/%02d/%02d %02d:%02d:%02d ", rgucTimeStamp[BYTE_ZERO], 
          rgucTimeStamp[BYTE_ONE], rgucTimeStamp[BYTE_TWO], rgucTimeStamp[BYTE_THREE], 
          rgucTimeStamp[BYTE_FOUR], rgucTimeStamp[BYTE_FIVE]);
	
	//printf("%s\n\r",rgucTempData);
  uliStartAddres += 6;
  fnWriteData(rgucTempData);
	
	/*ucJobNameID change23*/
  ucReadData = fnNVSRAM1Byte_Read(uliStartAddres++);
	#ifdef DBG_DIAG_WARN_LOGIC
	 sprintf((char *)rgucTempData,"%d ",ucReadData);
	 fnWriteData(rgucTempData);
	#endif
	
	/*ucSupervisorNameID; change23*/
	ucReadData = fnNVSRAM1Byte_Read(uliStartAddres++);
	#ifdef DBG_DIAG_WARN_LOGIC
	 sprintf((char *)rgucTempData,"%d ",ucReadData);
	 fnWriteData(rgucTempData);
	#endif
  
	
  ucReadData = fnNVSRAM1Byte_Read(uliStartAddres++);
  ((ucReadData >> 0) & 0x01)? fnWriteData("FAIL ") : fnWriteData("PASS ");
  ((ucReadData >> 1) & 0x01)? fnWriteData("FAIL ") : fnWriteData("PASS ");
  ((ucReadData >> 2) & 0x01)? fnWriteData("FAIL ") : fnWriteData("PASS ");
  ((ucReadData >> 3) & 0x01)? fnWriteData("FAIL ") : fnWriteData("PASS ");
  ((ucReadData >> 4) & 0x01)? fnWriteData("FAIL ") : fnWriteData("PASS ");
  ((ucReadData >> 5) & 0x01)? fnWriteData("FAIL ") : fnWriteData("PASS ");
  ((ucReadData >> 6) & 0x01)? fnWriteData("FAIL ") : fnWriteData("PASS ");
  ((ucReadData >> 7) & 0x01)? fnWriteData("FAIL ") : fnWriteData("PASS ");
  
  ucReadData = fnNVSRAM1Byte_Read(uliStartAddres++);
  ((ucReadData >> 0) & 0x01)? fnWriteData("FAIL ") : fnWriteData("PASS ");
  ((ucReadData >> 1) & 0x01)? fnWriteData("FAIL ") : fnWriteData("PASS ");
  ((ucReadData >> 2) & 0x01)? fnWriteData("FAIL ") : fnWriteData("PASS ");
  ((ucReadData >> 3) & 0x01)? fnWriteData("FAIL ") : fnWriteData("PASS ");
  ((ucReadData >> 4) & 0x01)? fnWriteData("FAIL ") : fnWriteData("PASS ");
  ((ucReadData >> 5) & 0x01)? fnWriteData("FAIL ") : fnWriteData("PASS ");
  ((ucReadData >> 6) & 0x01)? fnWriteData("FAIL ") : fnWriteData("PASS ");
  ((ucReadData >> 7) & 0x01)? fnWriteData("FAIL ") : fnWriteData("PASS ");
  
  ucReadData = fnNVSRAM1Byte_Read(uliStartAddres++);
  ((ucReadData >> 0) & 0x01)? fnWriteData("FAIL ") : fnWriteData("PASS ");
  ((ucReadData >> 1) & 0x01)? fnWriteData("FAIL ") : fnWriteData("PASS ");
  ((ucReadData >> 2) & 0x01)? fnWriteData("FAIL ") : fnWriteData("PASS ");
  ((ucReadData >> 3) & 0x01)? fnWriteData("FAIL ") : fnWriteData("PASS ");
  ((ucReadData >> 4) & 0x01)? fnWriteData("FAIL ") : fnWriteData("PASS ");
  ((ucReadData >> 5) & 0x01)? fnWriteData("FAIL ") : fnWriteData("PASS ");
  ((ucReadData >> 6) & 0x01)? fnWriteData("FAIL ") : fnWriteData("PASS ");
  ((ucReadData >> 7) & 0x01)? fnWriteData("FAIL ") : fnWriteData("PASS ");
  
  ucReadData = fnNVSRAM1Byte_Read(uliStartAddres++);
  ((ucReadData >> 0) & 0x01)? fnWriteData("FAIL ") : fnWriteData("PASS ");
  ((ucReadData >> 1) & 0x01)? fnWriteData("FAIL ") : fnWriteData("PASS ");
  ((ucReadData >> 2) & 0x01)? fnWriteData("FAIL ") : fnWriteData("PASS ");
  ((ucReadData >> 3) & 0x01)? fnWriteData("FAIL ") : fnWriteData("PASS ");
  ((ucReadData >> 4) & 0x01)? fnWriteData("FAIL ") : fnWriteData("PASS ");
  ((ucReadData >> 5) & 0x01)? fnWriteData("FAIL ") : fnWriteData("PASS ");
  ((ucReadData >> 6) & 0x01)? fnWriteData("FAIL ") : fnWriteData("PASS ");
  ((ucReadData >> 7) & 0x01)? fnWriteData("FAIL ") : fnWriteData("PASS ");
  
  ucReadData = fnNVSRAM1Byte_Read(uliStartAddres++);
  ((ucReadData >> 0) & 0x01)? fnWriteData("FAIL ") : fnWriteData("PASS ");
  ((ucReadData >> 1) & 0x01)? fnWriteData("FAIL ") : fnWriteData("PASS ");
  ((ucReadData >> 2) & 0x01)? fnWriteData("FAIL ") : fnWriteData("PASS ");
  ((ucReadData >> 3) & 0x01)? fnWriteData("FAIL ") : fnWriteData("PASS ");
  ((ucReadData >> 4) & 0x01)? fnWriteData("FAIL ") : fnWriteData("PASS ");
  ((ucReadData >> 5) & 0x01)? fnWriteData("FAIL ") : fnWriteData("PASS ");
  ((ucReadData >> 6) & 0x01)? fnWriteData("FAIL ") : fnWriteData("PASS ");
  ((ucReadData >> 7) & 0x01)? fnWriteData("FAIL ") : fnWriteData("PASS ");
  
  ucReadData = fnNVSRAM1Byte_Read(uliStartAddres++);
  ((ucReadData >> 0) & 0x01)? fnWriteData("FAIL ") : fnWriteData("PASS ");
  ((ucReadData >> 1) & 0x01)? fnWriteData("FAIL ") : fnWriteData("PASS ");
  ((ucReadData >> 2) & 0x01)? fnWriteData("FAIL ") : fnWriteData("PASS ");
  ((ucReadData >> 3) & 0x01)? fnWriteData("FAIL ") : fnWriteData("PASS ");
  ((ucReadData >> 4) & 0x01)? fnWriteData("FAIL ") : fnWriteData("PASS ");
  ((ucReadData >> 5) & 0x01)? fnWriteData("FAIL ") : fnWriteData("PASS ");
  
  fnWriteEndElement("Record");
}
/*****************************************************************************
**@Function			: fnDiagnosticLogScan
**@Descriptions	: Scan Diagnostic log for User ID and Job ID with Date and time
**@parameters		: None
**@return				: None      
*******************************************************************************/
void fnDiagnosticLogScan(DATALOG_TX_t *pstLog)
{
	  uint8_t rgucTimeStamp[6];
  memset(rgucTimeStamp,'\0',strlen((char *)rgucTimeStamp)); 
	DateTime_t stLogDateTime;
  /*Time Stamp read */
  fnNVSRAM1_MultiDataRead(6, pstLog->uliStartAddress, rgucTimeStamp);		

	stLogDateTime.ucYear = rgucTimeStamp[BYTE_ZERO];
	stLogDateTime.ucMonth = rgucTimeStamp[BYTE_ONE];
	stLogDateTime.ucDate = rgucTimeStamp[BYTE_TWO];
	stLogDateTime.ucHour = rgucTimeStamp[BYTE_THREE];
	stLogDateTime.ucMin = rgucTimeStamp[BYTE_FOUR];
	stLogDateTime.ucSec = rgucTimeStamp[BYTE_FIVE];
	printf("\n\rLog Sec %X",fnGetDateTimeInSec(stLogDateTime));
	printf("\n\rStart Sec %X",pstLog->stStartLogDateTime.uliTotalSec);
	printf("\n\rEnd Sec %X",pstLog->stEndLogDateTime.uliTotalSec);
	
	if((pstLog->stStartLogDateTime.uliTotalSec <= fnGetDateTimeInSec(stLogDateTime)) &&
		(pstLog->stEndLogDateTime.uliTotalSec >= fnGetDateTimeInSec(stLogDateTime)))
	{
		if(RESET == pstLog->ucStartLogTimeFlag)
		{
		  pstLog->uliStartAddress += 6;
	
			/*ucJobNameID change23*/
			pstLog->ucNextUserID = pstLog->ucCurrUserID = fnNVSRAM1Byte_Read(pstLog->uliStartAddress++);
			
			/*ucSupervisorNameID; change23*/
			pstLog->ucNextJobID = pstLog->ucCurrJobID = fnNVSRAM1Byte_Read(pstLog->uliStartAddress++);
			fnDatalogStartFrame(pstLog->ucNextUserID,pstLog->ucNextJobID);
			fnWifiSendData(rgucXMLtxBuffer);
			pstLog->uliStartAddress -= 8;
			pstLog->ucStartLogTimeFlag = SET;
			printf("\n\rStart Log");
		}
		else
		{
			pstLog->uliStartAddress += 6;
			/*ucJobNameID change23*/
			pstLog->ucNextUserID = fnNVSRAM1Byte_Read(pstLog->uliStartAddress++);
			
			/*ucSupervisorNameID; change23*/
			pstLog->ucNextJobID = fnNVSRAM1Byte_Read(pstLog->uliStartAddress++);
			
			pstLog->uliStartAddress -= 8;
			if((pstLog->ucCurrUserID == pstLog->ucNextUserID) && 
				(pstLog->ucCurrJobID == pstLog->ucNextJobID))
			{
				fnDiagLog_Read(pstLog->uliStartAddress);
				//fnWifiSendData(rgucXMLtxBuffer);
				pstLog->uliStartAddress += DL_DIAG_PACKET_SIZE;
				printf("\n\rData Log");
			}
			else if((pstLog->ucCurrUserID == pstLog->ucNextUserID) && 
				(pstLog->ucCurrJobID != pstLog->ucNextJobID))
			{
				pstLog->ucStartLogTimeFlag = RESET;
				fnDatalogEndFrame();
				//fnWifiSendData(rgucXMLtxBuffer);
				printf("\n\rEnd1 Log");
				//pstLog->ucStopLogTimeFlag = SET;
			}
			else if(pstLog->ucCurrUserID != pstLog->ucNextUserID)
			{
				pstLog->ucStartLogTimeFlag = RESET;
				fnDatalogEndFrame();
				//fnWifiSendData(rgucXMLtxBuffer);
				printf("\n\rEnd2 Log");
				//pstLog->ucStopLogTimeFlag = SET;
			}
		}
	}
	else
	{
		pstLog->uliStartAddress += DL_DIAG_PACKET_SIZE;
		//pstLog->ucStopLogTimeFlag = SET;
		printf("\n\rEnd Log");
	}
}
 /*****************************************************************************
**@Function			: fnDiagnosticLog
**@Descriptions	: Send Diagnostic log over Wifi
**@parameters		: None
**@return				: None      
*******************************************************************************/
void fnDiagnosticLog(DATALOG_TX_t *pstLog)
{
  if(DIAGNOSTIC_LOG == pstLog->ucLogType)
  {
    if(pstLog->ucDetailFlag > BYTE_ZERO)
    {
      printf("\n\rDatalog Detail %d",pstLog->ucDetailFlag);
      fnDiagLog_Detail(&pstLog->ucDetailFlag);
					pstLog->ucStartLogTimeFlag = RESET;
			//fnWifiSendData(rgucXMLtxBuffer);
    }
    else if(pstLog->uliStartAddress < pstLog->uliStopAddress)
    {
			printf("\n\rLogging 0x%X 0x%X",pstLog->uliStartAddress,pstLog->uliStopAddress);
      fnDiagnosticLogScan(pstLog);
			//pstLog->uliStartAddress += DL_DIAG_PACKET_SIZE;
    }
    else //if(pstLog->ucStopLogTimeFlag == SET)
    {
      pstLog->ucStopLogTimeFlag = RESET;
			memset(rgucXMLtxBuffer,'\0',strlen((char *)rgucXMLtxBuffer));
			fnWriteEndElement("Data");
			fnWriteEndElement("CRC-Evans_Data");
      //fnWifiSendData(rgucXMLtxBuffer);
      pstLog->ucLogType = RESET;
      pstLog->ucSendFlag = RESET;
			/*change33*/
			stWarningLog.ucWarnLog_DiagSendWarnCounter = RESET;
			 stDatalog.ucWifiSendFlag = RESET;
    }
  }
}
/*****************************************************************************
**@Function			: fnPRDLog_Detail
**@Descriptions	: Append Periodic log info in XML format 
**@parameters		: None
**@return				: None      
*******************************************************************************/
void fnPRDLog_Detail(uint8_t *ucNextData)
{
  //memset(rgucXMLtxBuffer,'\0',strlen((char *)rgucXMLtxBuffer));
	switch(*ucNextData)
	{
		case 1:
			{
		
    fnWriteStartElement("CRC-Evans_Data");
    fnWriteStartElement("File_Identity displayName=\"Periodic Log\"");
    fnWriteData("Periodic Log");
    fnWriteEndElement("File_Identity");
    fnWriteStartElement("View");	
		
		fnWriteStartElement("Record internalName=\"Version\" displayName=\"Firmware Version\"");
		fnWriteStartElement("Variable internalName=\"SCU_Firmware_Version\" displayName=\"SCU Firmware Version\" type=\"string\"/");
    fnWriteStartElement("Variable internalName=\"HCM_Firmware_Version\" displayName=\"HCM Firmware Version\" type=\"string\"/");
    fnWriteStartElement("Variable internalName=\"VCM_Firmware_Version\" displayName=\"VCM Firmware Version\" type=\"string\"/");
    fnWriteStartElement("Variable internalName=\"HMI_Firmware_Version\" displayName=\"HMI Firmware Version\" type=\"string\"/");
    fnWriteStartElement("Variable internalName=\"OPMM_Firmware_Version\" displayName=\"OPMM Firmware Version\" type=\"string\"/");
		fnWriteEndElement("Record");
		
    fnWriteStartElement("Record internalName=\"Start\" displayName=\"Start\"");
    fnWriteStartElement("Variable internalName=\"StartDate\" displayName=\"Date\" type=\"Date\" /");
    fnWriteStartElement("Variable internalName=\"StartTime\" displayName=\"Time\" type=\"Time\" /");
		*ucNextData = *ucNextData + 1;
			}
		break;
		case 2:
			{
    fnWriteStartElement("Variable internalName=\"CoatMaterial\" displayName=\"Coating Material\" type=\"string\" /");
    fnWriteStartElement("Variable internalName=\"BaseMaterial\" displayName=\"Base Material\" type=\"string\" /");
    fnWriteStartElement("Variable internalName=\"HardMaterial\" displayName=\"Hardner Material\" type=\"string\" /");
    fnWriteStartElement("Variable internalName=\"Prime\" displayName=\"Prime\" type=\"string\" /");
    fnWriteStartElement("Variable internalName=\"Flush\" displayName=\"Flush\" type=\"string\" /");
    fnWriteEndElement("Record");
    *ucNextData = *ucNextData + 1;
			}
		break;
		case 3:
			{
    fnWriteStartElement("Record internalName=\"Measurement\" displayName=\"Measurement\"");
    fnWriteStartElement("Variable internalName=\"MeasurementStartDate\" displayName=\"Date\" type=\"Date\" /");
    fnWriteStartElement("Variable internalName=\"MeasurementStartTime\" displayName=\"Time\" type=\"Time\" /");
    fnWriteStartElement("Variable internalName=\"JointNum\" displayName=\"Joint Number\" type=\"decimal\" /");
    fnWriteStartElement("Variable internalName=\"AntifreezTemp\" displayName=\"Antifreez Temp\" type=\"float\" units=\"celsius\" /");
    *ucNextData = *ucNextData + 1;
			}
		break;
		case 4:
			{
    fnWriteStartElement("Variable internalName=\"BaseTemp\" displayName=\"Base Temp\" type=\"float\" units=\"celsius\" /");
    fnWriteStartElement("Variable internalName=\"HardTemp\" displayName=\"Hard Temp\" type=\"float\" units=\"celsius\" /");
    fnWriteStartElement("Variable internalName=\"OPMMBaseTemp\" displayName=\"Base Temp\" type=\"float\" units=\"celsius\" /");
    fnWriteStartElement("Variable internalName=\"OPMMHardTemp\" displayName=\"Hard Temp\" type=\"float\" units=\"celsius\" /");
    *ucNextData = *ucNextData + 1;
			}
		break;
		case 5:
			{
    fnWriteStartElement("Variable internalName=\"AmbientTempInContainer\" displayName=\"Ambient Temp Inside container\" type=\"float\" units=\"celsius\" /");
    fnWriteStartElement("Variable internalName=\"AmbientTempOutContainer\" displayName=\"Ambient Temp Outside container\" type=\"float\" units=\"celsius\" /");
    fnWriteStartElement("Variable internalName=\"ReturnLineTemp\" displayName=\"Return line temperature (MCU)\" type=\"float\" units=\"Percent\" /");
    fnWriteStartElement("Variable internalName=\"Humidity\" displayName=\"Humidity\" type=\"float\" units=\"Percent\" /");
    *ucNextData = *ucNextData + 1;
			}
		break;
		case 6:
			{
    fnWriteStartElement("Variable internalName=\"DewPoint\" displayName=\"Dew Point\" type=\"float\" units=\"celsius\" /");
    fnWriteStartElement("Variable internalName=\"BaseMaterialFillLevel\" displayName=\"Base material  Fill level\" type=\"float\" units=\"Percent\" /");
    fnWriteStartElement("Variable internalName=\"HardMaterialFillLevel\" displayName=\"Hard material  Fill level\" type=\"float\" units=\"Percent\" /");	
    fnWriteStartElement("Variable internalName=\"PressureGauge1\" displayName=\"Pressure gauge 1\" type=\"string\" /");
    *ucNextData = *ucNextData + 1;
			}
		break;
		case 7:
			{
    fnWriteStartElement("Variable internalName=\"PressureGauge2\" displayName=\"Pressure gauge 2\" type=\"string\" /");
    fnWriteStartElement("Variable internalName=\"PressureGauge3\" displayName=\"Pressure gauge 3\" type=\"string\" /");
    fnWriteStartElement("Variable internalName=\"State\" displayName=\"State\" type=\"string\" /");
    fnWriteEndElement("Record");
    fnWriteStartElement("Record internalName=\"End\" displayName=\"End\"");
    fnWriteStartElement("Variable internalName=\"EndDate\" displayName=\"Date\" type=\"Date\" /");
    fnWriteStartElement("Variable internalName=\"EndTime\" displayName=\"Time\" type=\"Time\" /");
    fnWriteEndElement("Record");
    fnWriteEndElement("View");
    fnWriteStartElement("Data");
    *ucNextData = *ucNextData + 1;
			}
		break;
		default:
			{
				printf("\n\rDefault");
		*ucNextData = BYTE_ZERO;
  }
	}
}
/*****************************************************************************
**@Function			: fnPRDLog_Read
**@Descriptions	: Read the One Periodic data log from 
NVSRAM and send it to UART in CSV format.
**@parameters		: uliStartAddr: Starting Address 
**@return				: None      
*******************************************************************************/
void fnPRDLog_Read(uint32_t uliStartAddr)
{
  uint8_t rgucTimeStamp[20];
  //char rgucTempData[150];
  uint8_t rgucTempData[100];
  uint8_t ucReadData ;  //change15
  
  memset(rgucXMLtxBuffer,'\0',strlen((char *)rgucXMLtxBuffer));
  fnWriteStartElement("Record internalName=\"Measurement\"");
  
  if(uliStartAddr & PRDADDR_MASK)/*NVSRAM2 Address*/
  {
    /*Time Stamp read and Display on UART*/
    fnNVSRAM2_MultiDataRead(6, uliStartAddr, rgucTimeStamp);		
    
    sprintf((char *)rgucTempData,"20%02d/%02d/%02d %02d:%02d:%02d ", rgucTimeStamp[BYTE_ZERO],
            rgucTimeStamp[BYTE_ONE], rgucTimeStamp[BYTE_TWO], rgucTimeStamp[BYTE_THREE],
            rgucTimeStamp[BYTE_FOUR], rgucTimeStamp[BYTE_FIVE]);
    uliStartAddr += 6;
    fnWriteData(rgucTempData);
    /*change23*/
		ucReadData = fnNVSRAM2Byte_Read(uliStartAddr++);
		#ifdef DBG_PERIODIC_WARN_LOGIC
	 sprintf((char *)rgucTempData,"%d ",ucReadData);
	 fnWriteData(rgucTempData);
	#endif
	
		/*change23*/
		ucReadData = fnNVSRAM2Byte_Read(uliStartAddr++);
		
		#ifdef DBG_PERIODIC_WARN_LOGIC
	 sprintf((char *)rgucTempData,"%d ",ucReadData);
	 fnWriteData(rgucTempData);
	#endif
		
    /*Joint Number Read*/
    fnNVSRAM2_MultiDataRead (4, uliStartAddr, SplitByte.rgucData);
#ifdef DBG_PRD_READ
    printf(" Joint Number Read = %f \n\r", SplitByte.fData);
#endif
    sprintf((char *)rgucTempData,"%0.0f ", SplitByte.fData);
    fnWriteData(rgucTempData);
    uliStartAddr += 4;
    
    /*Antifreez Liquid Temp Read*/
    fnNVSRAM2_MultiDataRead (4, uliStartAddr, SplitByte.rgucData);
#ifdef DBG_PRD_READ
    printf(" Antifreez Liquid Temp Read = %f \n\r", SplitByte.fData);
#endif
    if(INVALID_FLOAT_READ == SplitByte.fData)
    {
      fnWriteData("ERROR ");
    }
    else
    {
      sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
      fnWriteData(rgucTempData);
    }
    uliStartAddr += 4;
    
    /*Base Material Temp Read*/
    fnNVSRAM2_MultiDataRead (4, uliStartAddr, SplitByte.rgucData);
#ifdef DBG_PRD_READ
    printf(" Base Material Temp Read= %f \n\r", SplitByte.fData);
#endif
    if(INVALID_FLOAT_READ == SplitByte.fData)
    {
      fnWriteData("ERROR ");
    }
    else
    {
      sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
      fnWriteData(rgucTempData);
    }
    uliStartAddr += 4;
    
    /*Hardner Material Temp Read*/
    fnNVSRAM2_MultiDataRead (4, uliStartAddr, SplitByte.rgucData);
#ifdef DBG_PRD_READ
    printf(" Hardner Material Temp Read = %f \n\r", SplitByte.fData);
#endif
    if(INVALID_FLOAT_READ == SplitByte.fData)
    {
      fnWriteData("ERROR ");
    }
    else
    {
      sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
      fnWriteData(rgucTempData);
    }
    uliStartAddr += 4;
    
    /*OPMM Base Temp Read*/
    fnNVSRAM2_MultiDataRead (4, uliStartAddr, SplitByte.rgucData);
#ifdef DBG_PRD_READ
    printf(" OPMM Base Temp Read = %f \n\r", SplitByte.fData);
#endif
    if(INVALID_FLOAT_READ == SplitByte.fData)
    {
      fnWriteData("ERROR ");
    }
    else
    {
      sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
      fnWriteData(rgucTempData);
    }
    uliStartAddr += 4;
    
    /*OPMM Hard Temp Read*/
    fnNVSRAM2_MultiDataRead (4, uliStartAddr, SplitByte.rgucData);
#ifdef DBG_PRD_READ
    printf(" OPMM Hard Temp Read = %f \n\r", SplitByte.fData);
#endif
    if(INVALID_FLOAT_READ == SplitByte.fData)
    {
      fnWriteData("ERROR ");
    }
    else
    {
      sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
      fnWriteData(rgucTempData);
    }
    uliStartAddr += 4;
    
    /*Inside container Temp Read*/
    fnNVSRAM2_MultiDataRead (4, uliStartAddr, SplitByte.rgucData);
#ifdef DBG_PRD_READ
    printf(" Inside container Temp Read = %f \n\r", SplitByte.fData);
#endif
    if(INVALID_FLOAT_READ == SplitByte.fData)
    {
      fnWriteData("ERROR ");
    }
    else
    {
      sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
      fnWriteData(rgucTempData);
    }
    uliStartAddr += 4;
    
    /*Outside container Temp Read*/
    fnNVSRAM2_MultiDataRead (4, uliStartAddr, SplitByte.rgucData);
#ifdef DBG_PRD_READ
    printf(" Outside container Temp Read = %f \n\r", SplitByte.fData);
#endif
    if(INVALID_FLOAT_READ == SplitByte.fData)
    {
      fnWriteData("ERROR ");
    }
    else
    {
      sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
      fnWriteData(rgucTempData);
    }
    uliStartAddr += 4;
    
    /*Base Returnline Temp Read SCU*/
    fnNVSRAM2_MultiDataRead (4, uliStartAddr, SplitByte.rgucData);
#ifdef DBG_PRD_READ
    printf(" Base Returnline Temp Read SCU = %f \n\r", SplitByte.fData);
#endif
    if(INVALID_FLOAT_READ == SplitByte.fData)
    {
      fnWriteData("ERROR ");
    }
    else
    {
      sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
      fnWriteData(rgucTempData);
    }
    uliStartAddr += 4;
    
    /*Humidity Read*/
    fnNVSRAM2_MultiDataRead (4, uliStartAddr, SplitByte.rgucData);
#ifdef DBG_PRD_READ
    printf(" Humidity Read = %f \n\r", SplitByte.fData);
#endif
    if(INVALID_FLOAT_READ == SplitByte.fData)
    {
      fnWriteData("ERROR ");
    }
    else
    {
      sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
      fnWriteData(rgucTempData);
    }
    uliStartAddr += 4;
    
    /*Dew Point Read*/
    fnNVSRAM2_MultiDataRead (4, uliStartAddr, SplitByte.rgucData);
#ifdef DBG_PRD_READ
    printf(" Dew Point Read = %f \n\r", SplitByte.fData);
#endif
    if(INVALID_FLOAT_READ == SplitByte.fData)
    {
      fnWriteData("ERROR ");
    }
    else
    {
      sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
      fnWriteData(rgucTempData);
    }
    uliStartAddr += 4;
    
    /*Base Material Fill Level Read*/
    fnNVSRAM2_MultiDataRead (4, uliStartAddr, SplitByte.rgucData);
#ifdef DBG_PRD_READ
    printf(" Base Material Fill Level Read = %f \n\r", SplitByte.fData);
#endif
    if(INVALID_FLOAT_READ == SplitByte.fData)
    {
      fnWriteData("ERROR ");
    }
    else
    {
      sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
      fnWriteData(rgucTempData);
    }
    uliStartAddr += 4;
    
    
    /*Hard Material Fill Level Read*/
    fnNVSRAM2_MultiDataRead (4, uliStartAddr, SplitByte.rgucData);
#ifdef DBG_PRD_READ
    printf(" HHard Material Fill Level Read = %f \n\r", SplitByte.fData);
#endif
    if(INVALID_FLOAT_READ == SplitByte.fData)
    {
      fnWriteData("ERROR ");
    }
    else
    {
      sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
      fnWriteData(rgucTempData);
    }
    uliStartAddr += 4;
    
    /*PG1 Read*/
    fnNVSRAM2_MultiDataRead (4, uliStartAddr, SplitByte.rgucData);
#ifdef DBG_PRD_READ
    printf(" PG1 Read = %f \n\r", SplitByte.fData);
#endif
    if(INVALID_FLOAT_READ == SplitByte.fData)
    {
      fnWriteData("ERROR ");
    }
    else
    {
      sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
      fnWriteData(rgucTempData);
    }
    uliStartAddr += 4;
    
    /*PG2 Read*/
    fnNVSRAM2_MultiDataRead (4, uliStartAddr, SplitByte.rgucData);
#ifdef DBG_PRD_READ
    printf(" PG2 Read = %f \n\r", SplitByte.fData);
#endif
    if(INVALID_FLOAT_READ == SplitByte.fData)
    {
      fnWriteData("ERROR ");
    }
    else
    {
      sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
      fnWriteData(rgucTempData);
    }
    uliStartAddr += 4;
    
    /*PG3 Read*/
    fnNVSRAM2_MultiDataRead (4, uliStartAddr, SplitByte.rgucData);
#ifdef DBG_PRD_READ
    printf(" PG3 Read= %f \n\r", SplitByte.fData);
#endif
    if(INVALID_FLOAT_READ == SplitByte.fData)
    {
      fnWriteData("ERROR ");
    }
    else
    {
      sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
      fnWriteData(rgucTempData);
    }
    uliStartAddr += 4;	
    /*SPRAY OR STALL OR IDLE Data Byte 1 change15*/
    ucReadData = fnNVSRAM2Byte_Read(uliStartAddr++);
    if(ucReadData & 0x01)
    {
      fnWriteData("SPRAY ");
    }
    else if(ucReadData & 0x02)
    {
      fnWriteData("STALL ");
    }
    else
    {
      fnWriteData("IDLE ");
    }
    
  }		
  else /* NVSRAM1 Address*/
  {
    /*Time Stamp read and Display on UART*/
    fnNVSRAM1_MultiDataRead(6, uliStartAddr, rgucTimeStamp);		
    
    sprintf((char *)rgucTempData,"20%02d/%02d/%02d %02d:%02d:%02d ", rgucTimeStamp[BYTE_ZERO], 
            rgucTimeStamp[BYTE_ONE], rgucTimeStamp[BYTE_TWO], rgucTimeStamp[BYTE_THREE],
            rgucTimeStamp[BYTE_FOUR], rgucTimeStamp[BYTE_FIVE]);
    uliStartAddr += 6;
    fnWriteData(rgucTempData);
     /*change23*/
		ucReadData = fnNVSRAM1Byte_Read(uliStartAddr++);
		#ifdef DBG_PERIODIC_WARN_LOGIC
	 sprintf((char *)rgucTempData,"%d ",ucReadData);
	 fnWriteData(rgucTempData);
	#endif
		/*change23*/
		ucReadData = fnNVSRAM1Byte_Read(uliStartAddr++);
		#ifdef DBG_PERIODIC_WARN_LOGIC
	 sprintf((char *)rgucTempData,"%d ",ucReadData);
	 fnWriteData(rgucTempData);
	#endif
		
    /*Joint Number Read*/
    fnNVSRAM1_MultiDataRead (4, uliStartAddr, SplitByte.rgucData);
#ifdef DBG_PRD_READ
    printf(" Joint Number Read = %f \n\r", SplitByte.fData );
#endif
    sprintf((char *)rgucTempData,"%0.0f ", SplitByte.fData);
    fnWriteData(rgucTempData);
    uliStartAddr += 4;
    
    /*Antifreez Liquid Temp Read*/
    fnNVSRAM1_MultiDataRead (4, uliStartAddr, SplitByte.rgucData);
#ifdef DBG_PRD_READ
    printf(" Antifreez Liquid Temp Read = %f \n\r", SplitByte.fData);
#endif
    if(INVALID_FLOAT_READ == SplitByte.fData)
    {
      fnWriteData("ERROR ");
    }
    else
    {
      sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
      fnWriteData(rgucTempData);
    }
    uliStartAddr += 4;
    
    /*Base Material Temp Read*/
    fnNVSRAM1_MultiDataRead (4, uliStartAddr, SplitByte.rgucData);
#ifdef DBG_PRD_READ
    printf(" Base Material Temp Read = %f \n\r", SplitByte.fData);
#endif
    if(INVALID_FLOAT_READ == SplitByte.fData)
    {
      fnWriteData("ERROR ");
    }
    else
    {
      sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
      fnWriteData(rgucTempData);
    }
    uliStartAddr += 4;
    
    /*Hardner Material Temp Read*/
    fnNVSRAM1_MultiDataRead (4, uliStartAddr, SplitByte.rgucData);
#ifdef DBG_PRD_READ
    printf("Hardner Material Temp Read = %f \n\r", SplitByte.fData);
#endif
    if(INVALID_FLOAT_READ == SplitByte.fData)
    {
      fnWriteData("ERROR ");
    }
    else
    {
      sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
      fnWriteData(rgucTempData);
    }
    uliStartAddr += 4;
    
    /*OPMM Base Temp Read*/
    fnNVSRAM1_MultiDataRead (4, uliStartAddr, SplitByte.rgucData);
#ifdef DBG_PRD_READ
    printf(" OPMM Base Temp Read = %f \n\r", SplitByte.fData);
#endif
    if(INVALID_FLOAT_READ == SplitByte.fData)
    {
      fnWriteData("ERROR ");
    }
    else
    {
      sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
      fnWriteData(rgucTempData);
    }
    uliStartAddr += 4;
    
    /*OPMM Hard Temp Read*/
    fnNVSRAM1_MultiDataRead (4, uliStartAddr, SplitByte.rgucData);
#ifdef DBG_PRD_READ
    printf(" OPMM Hard Temp Read = %f \n\r", SplitByte.fData);
#endif
    if(INVALID_FLOAT_READ == SplitByte.fData)
    {
      fnWriteData("ERROR ");
    }
    else
    {
      sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
      fnWriteData(rgucTempData);
    }
    uliStartAddr += 4;
    
    /*Inside container Temp Read*/
    fnNVSRAM1_MultiDataRead (4, uliStartAddr, SplitByte.rgucData);
#ifdef DBG_PRD_READ
    printf(" Inside container Temp Read = %f \n\r", SplitByte.fData);
#endif
    if(INVALID_FLOAT_READ == SplitByte.fData)
    {
      fnWriteData("ERROR ");
    }
    else
    {
      sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
      fnWriteData(rgucTempData);
    }
    uliStartAddr += 4;
    
    /*Outside container Temp Read*/
    fnNVSRAM1_MultiDataRead (4, uliStartAddr, SplitByte.rgucData);
#ifdef DBG_PRD_READ
    printf(" Outside container Temp Read = %f \n\r", SplitByte.fData);
#endif
    if(INVALID_FLOAT_READ == SplitByte.fData)
    {
      fnWriteData("ERROR ");
    }
    else
    {
      sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
      fnWriteData(rgucTempData);
    }
    uliStartAddr += 4;
    
    /*Base Returnline Temp Read SCU*/
    fnNVSRAM1_MultiDataRead (4, uliStartAddr, SplitByte.rgucData);
#ifdef DBG_PRD_READ
    printf(" Base Returnline Temp Read SCU = %f \n\r", SplitByte.fData);
#endif
    if(INVALID_FLOAT_READ == SplitByte.fData)
    {
      fnWriteData("ERROR ");
    }
    else
    {
      sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
      fnWriteData(rgucTempData);
    }
    uliStartAddr += 4;
    
    /*Humidity Read*/
    fnNVSRAM1_MultiDataRead (4, uliStartAddr, SplitByte.rgucData);
#ifdef DBG_PRD_READ
    printf(" Humidity Read = %f \n\r", SplitByte.fData);
#endif
    if(INVALID_FLOAT_READ == SplitByte.fData)
    {
      fnWriteData("ERROR ");
    }
    else
    {
      sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
      fnWriteData(rgucTempData);
    }
    uliStartAddr += 4;
    
    /*Dew Point Read*/
    fnNVSRAM1_MultiDataRead (4, uliStartAddr, SplitByte.rgucData);
#ifdef DBG_PRD_READ
    printf(" Dew Point Read = %f \n\r", SplitByte.fData);
#endif
    if(INVALID_FLOAT_READ == SplitByte.fData)
    {
      fnWriteData("ERROR ");
    }
    else
    {
      sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
      fnWriteData(rgucTempData);
    }
    uliStartAddr += 4;
    
    /*Base Material Fill Level Read*/
    fnNVSRAM1_MultiDataRead (4, uliStartAddr, SplitByte.rgucData);
#ifdef DBG_PRD_READ
    printf(" Base Material Fill Level Read = %f \n\r", SplitByte.fData);
#endif
    if(INVALID_FLOAT_READ == SplitByte.fData)
    {
      fnWriteData("ERROR ");
    }
    else
    {
      sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
      fnWriteData(rgucTempData);
    }
    uliStartAddr += 4;
    
    
    /*Hard Material Fill Level Read*/
    fnNVSRAM1_MultiDataRead (4, uliStartAddr, SplitByte.rgucData);
#ifdef DBG_PRD_READ
    printf(" Hard Material Fill Level Read = %f \n\r", SplitByte.fData);
#endif
    if(INVALID_FLOAT_READ == SplitByte.fData)
    {
      fnWriteData("ERROR ");
    }
    else
    {
      sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
      fnWriteData(rgucTempData);
    }
    uliStartAddr += 4;
    
    /*PG1 Read*/
    fnNVSRAM1_MultiDataRead (4, uliStartAddr, SplitByte.rgucData);
#ifdef DBG_PRD_READ
    printf(" PG1 Read = %f \n\r", SplitByte.fData);
#endif
    if(INVALID_FLOAT_READ == SplitByte.fData)
    {
      fnWriteData("ERROR ");
    }
    else
    {
      sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
      fnWriteData(rgucTempData);
    }
    uliStartAddr += 4;
    
    /*PG2 Read*/
    fnNVSRAM1_MultiDataRead (4, uliStartAddr, SplitByte.rgucData);
#ifdef DBG_PRD_READ
    printf(" PG2 Read = %f \n\r", SplitByte.fData);
#endif
    if(INVALID_FLOAT_READ == SplitByte.fData)
    {
      fnWriteData("ERROR ");
    }
    else
    {
      sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
      fnWriteData(rgucTempData);
    }
    uliStartAddr += 4;
    
    /*PG3 Read*/
    fnNVSRAM1_MultiDataRead (4, uliStartAddr, SplitByte.rgucData);
#ifdef DBG_PRD_READ
    printf(" PG3 Read = %f \n\r", SplitByte.fData);
#endif
    if(INVALID_FLOAT_READ == SplitByte.fData)
    {
      fnWriteData("ERROR ");
    }
    else
    {
      sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
      fnWriteData(rgucTempData);
    }
    uliStartAddr += 4;	
    /*SPRAY OR STALL OR IDLE Data Byte 1 change15*/
    ucReadData = fnNVSRAM1Byte_Read(uliStartAddr++);
    if(ucReadData & 0x01)
    {
      fnWriteData("SPRAY ");
    }
    else if(ucReadData & 0x02)
    {
      fnWriteData("STALL ");
    }
    else
    {
      fnWriteData("IDLE ");
    }
  }
  fnWriteEndElement("Record");
}
/*****************************************************************************
**@Function			: fnPeriodicLogScan
**@Descriptions	: Scan Periodic log for User ID and Job ID with Date and time
**@parameters		: None
**@return				: None      
*******************************************************************************/
void fnPeriodicLogScan(DATALOG_TX_t *pstLog)
{
	  uint8_t rgucTimeStamp[6];
  memset(rgucTimeStamp,'\0',strlen((char *)rgucTimeStamp)); 
	DateTime_t stLogDateTime;
	
  if(pstLog->uliStartAddress & PRDADDR_MASK)/*NVSRAM2 Address*/
  {
    /*Time Stamp */
    fnNVSRAM2_MultiDataRead(6, pstLog->uliStartAddress, rgucTimeStamp);
	}
else		
{
    /*Time Stamp*/
    fnNVSRAM1_MultiDataRead(6, pstLog->uliStartAddress, rgucTimeStamp);		
}

	stLogDateTime.ucYear = rgucTimeStamp[BYTE_ZERO];
	stLogDateTime.ucMonth = rgucTimeStamp[BYTE_ONE];
	stLogDateTime.ucDate = rgucTimeStamp[BYTE_TWO];
	stLogDateTime.ucHour = rgucTimeStamp[BYTE_THREE];
	stLogDateTime.ucMin = rgucTimeStamp[BYTE_FOUR];
	stLogDateTime.ucSec = rgucTimeStamp[BYTE_FIVE];
	printf("\n\rLog Sec %X",fnGetDateTimeInSec(stLogDateTime));
	printf("\n\rStart Sec %X",pstLog->stStartLogDateTime.uliTotalSec);
	printf("\n\rEnd Sec %X",pstLog->stEndLogDateTime.uliTotalSec);
	
	if((pstLog->stStartLogDateTime.uliTotalSec <= fnGetDateTimeInSec(stLogDateTime)) &&
		(pstLog->stEndLogDateTime.uliTotalSec >= fnGetDateTimeInSec(stLogDateTime)))
	{
		if(RESET == pstLog->ucStartLogTimeFlag)
		{
		  pstLog->uliStartAddress += 6;
	  if(pstLog->uliStartAddress & PRDADDR_MASK)/*NVSRAM2 Address*/
  {
			/*ucJobNameID change23*/
			pstLog->ucNextUserID = pstLog->ucCurrUserID = fnNVSRAM2Byte_Read(pstLog->uliStartAddress++);
			
			/*ucSupervisorNameID; change23*/
			pstLog->ucNextJobID = pstLog->ucCurrJobID = fnNVSRAM2Byte_Read(pstLog->uliStartAddress++);
	}
	else
	  {
			/*ucJobNameID change23*/
			pstLog->ucNextUserID = pstLog->ucCurrUserID = fnNVSRAM1Byte_Read(pstLog->uliStartAddress++);
			
			/*ucSupervisorNameID; change23*/
			pstLog->ucNextJobID = pstLog->ucCurrJobID = fnNVSRAM1Byte_Read(pstLog->uliStartAddress++);
	}
		
		fnDatalogStartFrame(pstLog->ucNextUserID,pstLog->ucNextJobID);
			//fnWifiSendData(rgucXMLtxBuffer);
			pstLog->uliStartAddress -= 8;
			pstLog->ucStartLogTimeFlag = SET;
			printf("\n\rStart Log");
		}
		else
		{
			pstLog->uliStartAddress += 6;
				  if(pstLog->uliStartAddress & PRDADDR_MASK)/*NVSRAM2 Address*/
  {
			/*ucJobNameID change23*/
			pstLog->ucNextUserID = pstLog->ucCurrUserID = fnNVSRAM2Byte_Read(pstLog->uliStartAddress++);
			
			/*ucSupervisorNameID; change23*/
			pstLog->ucNextJobID = pstLog->ucCurrJobID = fnNVSRAM2Byte_Read(pstLog->uliStartAddress++);
	}
	else
	  {
			/*ucJobNameID change23*/
			pstLog->ucNextUserID = pstLog->ucCurrUserID = fnNVSRAM1Byte_Read(pstLog->uliStartAddress++);
			
			/*ucSupervisorNameID; change23*/
			pstLog->ucNextJobID = pstLog->ucCurrJobID = fnNVSRAM1Byte_Read(pstLog->uliStartAddress++);
	}
		
			pstLog->uliStartAddress -= 8;
			if((pstLog->ucCurrUserID == pstLog->ucNextUserID) && 
				(pstLog->ucCurrJobID == pstLog->ucNextJobID))
			{
				fnPRDLog_Read(pstLog->uliStartAddress);
				//fnWifiSendData(rgucXMLtxBuffer);
				pstLog->uliStartAddress += DL_PRD_PACKET_SIZE;
				printf("\n\rData Log");
			}
			else if((pstLog->ucCurrUserID == pstLog->ucNextUserID) && 
				(pstLog->ucCurrJobID != pstLog->ucNextJobID))
			{
				pstLog->ucStartLogTimeFlag = RESET;
				fnDatalogEndFrame();
				//fnWifiSendData(rgucXMLtxBuffer);
				printf("\n\rEnd1 Log");
				//pstLog->ucStopLogTimeFlag = SET;
			}
			else if(pstLog->ucCurrUserID != pstLog->ucNextUserID)
			{
				pstLog->ucStartLogTimeFlag = RESET;
				fnDatalogEndFrame();

				printf("\n\rEnd2 Log");
				//pstLog->ucStopLogTimeFlag = SET;
			}
		}
	}
	else
	{
		pstLog->uliStartAddress += DL_PRD_PACKET_SIZE;
		//pstLog->ucStopLogTimeFlag = SET;
		printf("\n\rEnd Log");
	}
}
 /*****************************************************************************
**@Function			: fnPeriodicLog
**@Descriptions	: Send Periodic log
**@parameters		: None
**@return				: None      
*******************************************************************************/
void fnPeriodicLog(DATALOG_TX_t *pstLog)
{
  if(PERIODIC_LOG == pstLog->ucLogType)
  {
    if(pstLog->ucDetailFlag > BYTE_ZERO)
    {
      printf("\n\rDatalog Detail %d",pstLog->ucDetailFlag);
			printf("\n\rStart %X, Stop %X",pstLog->uliStartAddress,pstLog->uliStopAddress);
      fnPRDLog_Detail(&pstLog->ucDetailFlag);
					pstLog->ucStartLogTimeFlag = RESET;
			//fnWifiSendData(rgucXMLtxBuffer);
    }
    else if(pstLog->uliStartAddress < pstLog->uliStopAddress)
    {
			printf("\n\rLogging 0x%X 0x%X",pstLog->uliStartAddress,pstLog->uliStopAddress);
      fnPeriodicLogScan(pstLog);
							// pstLog->uliStartAddress += DL_PRD_PACKET_SIZE;
				      /*to check if NVSRAM1 is completed or not*/
      if((0 == pstLog->ucAddrFlag) && (pstLog->uliStartAddress >= DL_PRD_BOUNDARY))
      {
        pstLog->ucAddrFlag = 1;
        pstLog->uliStartAddress = BASE_ADDR_NVSRAM2;
      }
    }
    else //if(pstLog->ucStopLogTimeFlag == SET)
    {
      pstLog->ucStopLogTimeFlag = RESET;
			memset(rgucXMLtxBuffer,'\0',strlen((char *)rgucXMLtxBuffer));
			fnWriteEndElement("Data");
			fnWriteEndElement("CRC-Evans_Data");
      //fnWifiSendData(rgucXMLtxBuffer);
      pstLog->ucLogType = RESET;
      pstLog->ucSendFlag = RESET;
			/*change33*/
			stWarningLog.ucWarnLog_PeriodicSendWarnCounter = RESET;
			 stDatalog.ucWifiSendFlag = RESET;
    }
  }
}

/*****************************************************************************
**@Function			: fnStatusLog_Detail
**@Descriptions	: Append staus log inf in XML format 
**@parameters		: None
**@return				: None      
*******************************************************************************/
void fnStatusLog_Detail(uint8_t *ucNextData)
{
  //memset(rgucXMLtxBuffer,'\0',strlen((char *)rgucXMLtxBuffer));
  switch(*ucNextData)
	{
		case 1:
			{
		
    fnWriteStartElement("CRC-Evans_Data");
    fnWriteStartElement("File_Identity displayName=\"Status Log\"");
    fnWriteData("Status Log Per Joint");
    fnWriteEndElement("File_Identity");
    fnWriteStartElement("View");
		fnWriteStartElement("Record internalName=\"Version\" displayName=\"Firmware Version\"");
		fnWriteStartElement("Variable internalName=\"SCU_Firmware_Version\" displayName=\"SCU Firmware Version\" type=\"string\"/");
    fnWriteStartElement("Variable internalName=\"HCM_Firmware_Version\" displayName=\"HCM Firmware Version\" type=\"string\"/");
    fnWriteStartElement("Variable internalName=\"VCM_Firmware_Version\" displayName=\"VCM Firmware Version\" type=\"string\"/");
    fnWriteStartElement("Variable internalName=\"HMI_Firmware_Version\" displayName=\"HMI Firmware Version\" type=\"string\"/");
    fnWriteStartElement("Variable internalName=\"OPMM_Firmware_Version\" displayName=\"OPMM Firmware Version\" type=\"string\"/");
		fnWriteEndElement("Record");
    fnWriteStartElement("Record internalName=\"Start\" displayName=\"Start\"");
    fnWriteStartElement("Variable internalName=\"StartDate\" displayName=\"Date\" type=\"Date\" /");
    fnWriteStartElement("Variable internalName=\"StartTime\" displayName=\"Time\" type=\"Time\" /");
    fnWriteStartElement("Variable internalName=\"User\" displayName=\"User\" type=\"string\" /");
    fnWriteStartElement("Variable internalName=\"CoatingMaterial\" displayName=\"Coating Material\" type=\"string\" /");
*ucNextData = *ucNextData + 1;
			}
		break;
		case 2:
			{
    fnWriteStartElement("Variable internalName=\"BaseMaterial\" displayName=\"Base Material\" type=\"string\" /");
    fnWriteStartElement("Variable internalName=\"HardMaterial\" displayName=\"Hardner Material\" type=\"string\" /");
    fnWriteEndElement("Record");
    fnWriteStartElement("Record internalName=\"Measurement\" displayName=\"Measurement\"");
    fnWriteStartElement("Variable internalName=\"MeasurementStartDate\" displayName=\"Date\" type=\"Date\" /");
    fnWriteStartElement("Variable internalName=\"MeasurementStartTime\" displayName=\"Time\" type=\"Time\" /");
    fnWriteStartElement("Variable internalName=\"JointNumber\" displayName=\"Joint Number\" type=\"decimal\" /");
    fnWriteStartElement("Variable internalName=\"AntifreezTemp\" displayName=\"Antifreez Temp\" type=\"float\" units=\"celsius\" /");
    *ucNextData = *ucNextData + 1;
			}
		break;
		case 3:
			{
    fnWriteStartElement("Variable internalName=\"BaseTemp\" displayName=\"Base Temp\" type=\"float\" units=\"celsius\" /");
    fnWriteStartElement("Variable internalName=\"HardTemp\" displayName=\"Hard Temp\" type=\"float\" units=\"celsius\" /");
    fnWriteStartElement("Variable internalName=\"OPBaseTemp\" displayName=\"OPMM Base Temp\" type=\"float\" units=\"celsius\" /");
    fnWriteStartElement("Variable internalName=\"OPHardTemp\" displayName=\"OPMM Hard Temp\" type=\"float\" units=\"celsius\" /");
    fnWriteStartElement("Variable internalName=\"AmbientTempIN\" displayName=\"Ambient Temp Inside\" type=\"float\" units=\"celsius\" /");
    fnWriteStartElement("Variable internalName=\"AmbientTempOUT\" displayName=\"Ambient Temp Outside\" type=\"float\" units=\"celsius\" /");
    *ucNextData = *ucNextData + 1;
			}
		break;
		case 4:
			{
    fnWriteStartElement("Variable internalName=\"Humidity\" displayName=\"Humidity\" type=\"float\" units=\"Percent\" /");
    fnWriteStartElement("Variable internalName=\"DewPoint\" displayName=\"Dew Point\" type=\"float\" units=\"celsius\" /");
    fnWriteStartElement("Variable internalName=\"UpStrokeStall\" displayName=\"Up Stroke Stall Test\" type=\"string\" /");
    fnWriteStartElement("Variable internalName=\"DownStrokeStall\" displayName=\"Down Stroke Stall Test\" type=\"string\" /");
    fnWriteStartElement("Variable internalName=\"BaseMaterial\" displayName=\"Base Material\" type=\"float\" units=\"CC\" /");
    
    fnWriteEndElement("Record");
    *ucNextData = *ucNextData + 1;
			}
		break;
		case 5:
			{
    fnWriteStartElement("Variable internalName=\"HardMaterial\" displayName=\"Hardner Material\" type=\"float\" units=\"CC\" /");
    fnWriteStartElement("Variable internalName=\"MaterialConsume\" displayName=\"Material Consumed\" type=\"float\" units=\"CC\" /");
    fnWriteStartElement("Record internalName=\"End\" displayName=\"End\"");
    fnWriteStartElement("Variable internalName=\"EndDate\" displayName=\"Date\" type=\"Date\" /");
    fnWriteStartElement("Variable internalName=\"EndTime\" displayName=\"Time\" type=\"Time\" /");
    fnWriteEndElement("Record");
    fnWriteEndElement("View");
    fnWriteStartElement("Data");
    *ucNextData = *ucNextData + 1;
			}
		break;
		default:
			{
				printf("\n\rDefault");
		*ucNextData = BYTE_ZERO;
  }
	}
}	
/*****************************************************************************
**@Function			: fnStatusLog_Read
**@Descriptions	: Read the One Status data log from 
NVSRAM1 and send it to UART in CSV format.
**@parameters		: uliStatusStartAddr: Starting Address 
**@return				: None      
*******************************************************************************/
void fnStatusLog_Read(uint32_t uliStatusStartAddr)
{
  uint8_t rgucTimeStamp[6];
  uint8_t rgucTempData[100];
  uint8_t ucReadData = 0;
  
  memset(rgucXMLtxBuffer,'\0',strlen((char *)rgucXMLtxBuffer));
  fnWriteStartElement("Record internalName=\"Measurement\"");
  /*Time Stamp read and Display on UART*/
  fnNVSRAM1_MultiDataRead(6, uliStatusStartAddr,rgucTimeStamp);		
  
  sprintf((char *)rgucTempData,"%02d/%02d/%02d %02d:%02d:%02d ", rgucTimeStamp[BYTE_ZERO],
          rgucTimeStamp[BYTE_ONE], rgucTimeStamp[BYTE_TWO], rgucTimeStamp[BYTE_THREE],
          rgucTimeStamp[BYTE_FOUR], rgucTimeStamp[BYTE_FIVE]);
  uliStatusStartAddr += 6;
  fnWriteData(rgucTempData);
	
    /*change23*/
		ucReadData = fnNVSRAM1Byte_Read(uliStatusStartAddr++);
	
	#ifdef DBG_STATUS_WARN_LOGIC
	 sprintf((char *)rgucTempData,"%d ",ucReadData);
	 fnWriteData(rgucTempData);
	#endif
	
		/*change23*/
		ucReadData = fnNVSRAM1Byte_Read(uliStatusStartAddr++);
	
	#ifdef DBG_STATUS_WARN_LOGIC
	 sprintf((char *)rgucTempData,"%d ",ucReadData);
	 fnWriteData(rgucTempData);
	#endif
  /*Joint Number Read*/
  fnNVSRAM1_MultiDataRead (4, uliStatusStartAddr, SplitByte.rgucData);
  sprintf((char *)rgucTempData,"%0.0f ", SplitByte.fData);
  fnWriteData(rgucTempData);
  
  uliStatusStartAddr += 4;
  
  /*AntiFreez Temp Read*/
  fnNVSRAM1_MultiDataRead (4, uliStatusStartAddr, SplitByte.rgucData);
  if(INVALID_FLOAT_READ == SplitByte.fData)
  {
    fnWriteData("ERROR ");
  }
  else
  {
    sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
    fnWriteData(rgucTempData);
  }
  uliStatusStartAddr += 4;
  
  /* Inline BASE Temp */
  fnNVSRAM1_MultiDataRead (4, uliStatusStartAddr, SplitByte.rgucData);
  if(INVALID_FLOAT_READ == SplitByte.fData)
  {
    fnWriteData("ERROR ");
  }
  else
  {
    sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
    fnWriteData(rgucTempData); 
  }
  uliStatusStartAddr += 4;
  
  /* Inline Hard Temp*/
  fnNVSRAM1_MultiDataRead (4, uliStatusStartAddr, SplitByte.rgucData);
  if(INVALID_FLOAT_READ == SplitByte.fData)
  {
    fnWriteData("ERROR ");
  }
  else
  {
    sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
    fnWriteData(rgucTempData);
  }	
  uliStatusStartAddr += 4;
  
  /*OPMM Base Temp*/
  fnNVSRAM1_MultiDataRead (4, uliStatusStartAddr, SplitByte.rgucData);
  if(INVALID_FLOAT_READ == SplitByte.fData)
  {
    fnWriteData("ERROR ");
  }
  else
  {
    sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
    fnWriteData(rgucTempData);
  }	
  uliStatusStartAddr += 4;
  
  /*OPMM Hard Temp*/
  fnNVSRAM1_MultiDataRead (4, uliStatusStartAddr, SplitByte.rgucData);
  if(INVALID_FLOAT_READ == SplitByte.fData)
  {
    fnWriteData("ERROR ");
  }
  else
  {
    sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
    fnWriteData(rgucTempData);
  }	
  uliStatusStartAddr += 4;
  
  /*Inside Ambiant Temp*/
  fnNVSRAM1_MultiDataRead (4, uliStatusStartAddr, SplitByte.rgucData);
  if(INVALID_FLOAT_READ == SplitByte.fData)
  {
    fnWriteData("ERROR ");
  }
  else
  {
    sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
    fnWriteData(rgucTempData);
  }
  uliStatusStartAddr += 4;
  
  /*Outside Ambiant Temp*/
  fnNVSRAM1_MultiDataRead (4, uliStatusStartAddr, SplitByte.rgucData);
  if(INVALID_FLOAT_READ == SplitByte.fData)
  {
    fnWriteData("ERROR ");
  }
  else
  {
    sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
    fnWriteData(rgucTempData);
  }
  uliStatusStartAddr += 4;
  
  /*Outside Humidity*/
  fnNVSRAM1_MultiDataRead (4, uliStatusStartAddr, SplitByte.rgucData);
  if(INVALID_FLOAT_READ == SplitByte.fData)
  {
    fnWriteData("ERROR ");
  }
  else
  {
    sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
    fnWriteData(rgucTempData);
  }
  uliStatusStartAddr += 4;
  
  /* Dew Point */
  fnNVSRAM1_MultiDataRead (4, uliStatusStartAddr, SplitByte.rgucData);
  if(INVALID_FLOAT_READ == SplitByte.fData)
  {
    fnWriteData("ERROR ");
  }
  else
  {
    sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
    fnWriteData(rgucTempData);
  }
  uliStatusStartAddr += 4;
  
  
  /*Upstroke Stall test Result Data Byte 1*/
  ucReadData = fnNVSRAM1Byte_Read(uliStatusStartAddr++);
  ((ucReadData & 0x01)? fnWriteData("TRUE "):fnWriteData("FALSE "));
  /*Downstroke Stall test Result Data Byte 1*/
  ucReadData = fnNVSRAM1Byte_Read(uliStatusStartAddr++);
  ((ucReadData & 0x01)? fnWriteData("TRUE "):fnWriteData("FALSE "));
  
  /* Base material consumed per joint */
  fnNVSRAM1_MultiDataRead (4, uliStatusStartAddr, SplitByte.rgucData);
  if(INVALID_FLOAT_READ == SplitByte.fData)
  {
    fnWriteData("ERROR ");
  }
  else
  {
    sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
    fnWriteData(rgucTempData);
  }
  uliStatusStartAddr += 4;
  
  /* Hard material consumed per joint */
  fnNVSRAM1_MultiDataRead (4, uliStatusStartAddr, SplitByte.rgucData);
  if(INVALID_FLOAT_READ == SplitByte.fData)
  {
    fnWriteData("ERROR ");
  }
  else
  {
    sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
    fnWriteData(rgucTempData);
  }
  uliStatusStartAddr += 4;
  
  /* Total Material consumed per joint */
  fnNVSRAM1_MultiDataRead (4, uliStatusStartAddr, SplitByte.rgucData);
  if(INVALID_FLOAT_READ == SplitByte.fData)
  {
    fnWriteData("ERROR ");
  }
  else
  {
    sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
    fnWriteData(rgucTempData);
  }
  
  uliStatusStartAddr += 4;
  
  fnWriteEndElement("Record");
}

/*****************************************************************************
**@Function			: fnStatusLogScan
**@Descriptions	: Scan Status log for User ID and Job ID with Date and time
**@parameters		: None
**@return				: None      
*******************************************************************************/
void fnStatusLogScan(DATALOG_TX_t *pstLog)
{
	  uint8_t rgucTimeStamp[6];
  memset(rgucTimeStamp,'\0',strlen((char *)rgucTimeStamp)); 
	DateTime_t stLogDateTime;
  /*Time Stamp read */
  fnNVSRAM1_MultiDataRead(6, pstLog->uliStartAddress, rgucTimeStamp);		

	stLogDateTime.ucYear = rgucTimeStamp[BYTE_ZERO];
	stLogDateTime.ucMonth = rgucTimeStamp[BYTE_ONE];
	stLogDateTime.ucDate = rgucTimeStamp[BYTE_TWO];
	stLogDateTime.ucHour = rgucTimeStamp[BYTE_THREE];
	stLogDateTime.ucMin = rgucTimeStamp[BYTE_FOUR];
	stLogDateTime.ucSec = rgucTimeStamp[BYTE_FIVE];
	printf("\n\rLog Sec %X",fnGetDateTimeInSec(stLogDateTime));
	printf("\n\rStart Sec %X",pstLog->stStartLogDateTime.uliTotalSec);
	printf("\n\rEnd Sec %X",pstLog->stEndLogDateTime.uliTotalSec);
	
	if((pstLog->stStartLogDateTime.uliTotalSec <= fnGetDateTimeInSec(stLogDateTime)) &&
		(pstLog->stEndLogDateTime.uliTotalSec >= fnGetDateTimeInSec(stLogDateTime)))
	{
		if(RESET == pstLog->ucStartLogTimeFlag)
		{
		  pstLog->uliStartAddress += 6;
	
			/*ucJobNameID change23*/
			pstLog->ucNextUserID = pstLog->ucCurrUserID = fnNVSRAM1Byte_Read(pstLog->uliStartAddress++);
			
			/*ucSupervisorNameID; change23*/
			pstLog->ucNextJobID = pstLog->ucCurrJobID = fnNVSRAM1Byte_Read(pstLog->uliStartAddress++);
			fnDatalogStartFrame(pstLog->ucNextUserID,pstLog->ucNextJobID);
			fnWifiSendData(rgucXMLtxBuffer);
			pstLog->uliStartAddress -= 8;
			pstLog->ucStartLogTimeFlag = SET;
			printf("\n\rStart Log");
		}
		else
		{
			pstLog->uliStartAddress += 6;
			/*ucJobNameID change23*/
			pstLog->ucNextUserID = fnNVSRAM1Byte_Read(pstLog->uliStartAddress++);
			
			/*ucSupervisorNameID; change23*/
			pstLog->ucNextJobID = fnNVSRAM1Byte_Read(pstLog->uliStartAddress++);
			
			pstLog->uliStartAddress -= 8;
			if((pstLog->ucCurrUserID == pstLog->ucNextUserID) && 
				(pstLog->ucCurrJobID == pstLog->ucNextJobID))
			{
				fnStatusLog_Read(pstLog->uliStartAddress);
				//fnWifiSendData(rgucXMLtxBuffer);
				pstLog->uliStartAddress += DL_STA_PACKET_SIZE;
				printf("\n\rData Log");
			}
			else if((pstLog->ucCurrUserID == pstLog->ucNextUserID) && 
				(pstLog->ucCurrJobID != pstLog->ucNextJobID))
			{
				pstLog->ucStartLogTimeFlag = RESET;
				fnDatalogEndFrame();
				//fnWifiSendData(rgucXMLtxBuffer);
				printf("\n\rEnd1 Log");
				pstLog->ucStopLogTimeFlag = SET;
			}
			else if(pstLog->ucCurrUserID != pstLog->ucNextUserID)
			{
				pstLog->ucStartLogTimeFlag = RESET;
				fnDatalogEndFrame();
				//fnWifiSendData(rgucXMLtxBuffer);
				printf("\n\rEnd2 Log");
				pstLog->ucStopLogTimeFlag = SET;
			}
		}
	}
	else
	{
		pstLog->uliStartAddress += DL_STA_PACKET_SIZE;
		pstLog->ucStopLogTimeFlag = SET;
		printf("\n\rEnd Log");
	}
}
 /*****************************************************************************
**@Function			: fnStatusLog
**@Descriptions	: Send Status log
**@parameters		: None
**@return				: None      
*******************************************************************************/
void fnStatusLog(DATALOG_TX_t *pstLog)
{
  if(STATUS_LOG == pstLog->ucLogType)
  {
    if(pstLog->ucDetailFlag > BYTE_ZERO)
    {
      printf("\n\rDatalog Detail %d",pstLog->ucDetailFlag);
			printf("\n\rStart %X, Stop %X",pstLog->uliStartAddress,pstLog->uliStopAddress);
      fnStatusLog_Detail(&pstLog->ucDetailFlag);
					pstLog->ucStartLogTimeFlag = RESET;
			//fnWifiSendData(rgucXMLtxBuffer);
    }
    else if(pstLog->uliStartAddress < pstLog->uliStopAddress)
    {
			printf("\n\rLogging 0x%X 0x%X",pstLog->uliStartAddress,pstLog->uliStopAddress);
      fnStatusLogScan(pstLog);
						//	 pstLog->uliStartAddress += DL_STA_PACKET_SIZE;
    }
    else //if(pstLog->ucStopLogTimeFlag == SET)
    {
      //pstLog->ucStopLogTimeFlag = RESET;
			//memset(rgucXMLtxBuffer,'\0',strlen((char *)rgucXMLtxBuffer));
			fnWriteEndElement("Data");
			fnWriteEndElement("CRC-Evans_Data");
      //fnWifiSendData(rgucXMLtxBuffer);
      pstLog->ucLogType = RESET;
      pstLog->ucSendFlag = RESET;
			/*change33*/
			stWarningLog.ucWarnLog_PeriodicSendWarnCounter = RESET;
			 stDatalog.ucWifiSendFlag = RESET;
    }
  }
}
/*****************************************************************************
**@Function			: fnFaultLog_Detail
**@Descriptions	: Append Fault log info in XML format 
**@parameters		: None
**@return				: None      
*******************************************************************************/
void fnFaultLog_Detail(uint8_t *ucNextData)
{
  //memset(rgucXMLtxBuffer,'\0',strlen((char *)rgucXMLtxBuffer));
  switch(*ucNextData)
	{
		case 1:
			{
    fnWriteStartElement("CRC-Evans_Data");
    fnWriteStartElement("File_Identity displayName=\"Fault Log\"");
    fnWriteData("Fault Log");
    fnWriteEndElement("File_Identity");
    fnWriteStartElement("View");
    
		fnWriteStartElement("Record internalName=\"Version\" displayName=\"Firmware Version\"");
		fnWriteStartElement("Variable internalName=\"SCU_Firmware_Version\" displayName=\"SCU Firmware Version\" type=\"string\"/");
    fnWriteStartElement("Variable internalName=\"HCM_Firmware_Version\" displayName=\"HCM Firmware Version\" type=\"string\"/");
    fnWriteStartElement("Variable internalName=\"VCM_Firmware_Version\" displayName=\"VCM Firmware Version\" type=\"string\"/");
    fnWriteStartElement("Variable internalName=\"HMI_Firmware_Version\" displayName=\"HMI Firmware Version\" type=\"string\"/");
    fnWriteStartElement("Variable internalName=\"OPMM_Firmware_Version\" displayName=\"OPMM Firmware Version\" type=\"string\"/");
		fnWriteEndElement("Record");
		
    fnWriteStartElement("Record internalName=\"Start\" displayName=\"Start\"");
    fnWriteStartElement("Variable internalName=\"StartDate\" displayName=\"Date\" type=\"Date\" /");
    fnWriteStartElement("Variable internalName=\"StartTime\" displayName=\"Time\" type=\"Time\" /");
		fnWriteEndElement("Record");
    fnWriteStartElement("Record internalName=\"Measurement\" displayName=\"Measurement\"");
    *ucNextData = *ucNextData + 1;
			}
		break;
		case 2:
			{
    fnWriteStartElement("Variable internalName=\"MeasurementStartDate\" displayName=\"Date\" type=\"Date\" /");
    fnWriteStartElement("Variable internalName=\"MeasurementStartTime\" displayName=\"Time\" type=\"Time\" /");
    fnWriteStartElement("Variable internalName=\"OPMMSprayValve\" displayName=\"OPMM Spray Valve\" type=\"string\" /");
    fnWriteStartElement("Variable internalName=\"OPMMGunPosValve\" displayName=\"OPMM Gun Position Valve\" type=\"string\" /");
    fnWriteStartElement("Variable internalName=\"OPMMABValve\" displayName=\"OPMM AB Valve\" type=\"string\" /");
    *ucNextData = *ucNextData + 1;
			}
		break;
		case 3:
			{
    fnWriteStartElement("Variable internalName=\"OPMMSolventValve\" displayName=\"OPMM Solvent Valve\" type=\"string\" /");
    fnWriteStartElement("Variable internalName=\"OPMMForwardValve\" displayName=\"OPMM Forward Valve\" type=\"string\" /");
    fnWriteStartElement("Variable internalName=\"OPMMBackwardValve\" displayName=\"OPMM Backward Valve\" type=\"string\" /");
    fnWriteStartElement("Variable internalName=\"StallValve\" displayName=\"Stall Valve\" type=\"string\" /");	
    fnWriteStartElement("Variable internalName=\"InlineBaseTempBeyondLowTH\" displayName=\"Inline Base Temp Beyond Low TH\" type=\"string\" /");
		fnWriteStartElement("Variable internalName=\"InlineBaseTempBeyondHighTH\" displayName=\"Inline Base Temp Beyond High TH\" type=\"string\" /");
    *ucNextData = *ucNextData + 1;
			}
		break;
		case 4:
			{
    fnWriteStartElement("Variable internalName=\"InlineHardTempBeyondLowTH\" displayName=\"Inline Hard Temp Beyond Low TH\" type=\"string\" /");
    fnWriteStartElement("Variable internalName=\"InlineHardTempBeyondHighTH\" displayName=\"Inline Hard Temp Beyond High TH\" type=\"string\" /");
    fnWriteStartElement("Variable internalName=\"AntifreezTempBeyondLowTH\" displayName=\"Antifreez Temp Beyond Low TH\" type=\"string\" /");
    fnWriteStartElement("Variable internalName=\"AntifreezTempBeyondHighTH\" displayName=\"Antifreez Temp Beyond High TH\" type=\"string\" /");
    fnWriteStartElement("Variable internalName=\"BaseReturnLineTempBeyondLowTH\" displayName=\"Base Return Line Temp Beyond Low TH\" type=\"string\" /");	
    fnWriteStartElement("Variable internalName=\"BaseReturnLineTempBeyondHighTH\" displayName=\"Base Return Line Temp Beyond High TH\" type=\"string\" /");	
    fnWriteStartElement("Variable internalName=\"PG1BeyondLowTH\" displayName=\"PG1 Beyond Low TH\" type=\"string\" /");
    fnWriteStartElement("Variable internalName=\"PG1BeyondHighTH\" displayName=\"PG1 Beyond High TH\" type=\"string\" /");
		fnWriteStartElement("Variable internalName=\"PG2BeyondLowTH\" displayName=\"PG2 Beyond Low TH\" type=\"string\" /");
    fnWriteStartElement("Variable internalName=\"PG2BeyondHighTH\" displayName=\"PG2 Beyond High TH\" type=\"string\" /");
    *ucNextData = *ucNextData + 1;
			}
		break;
		case 5:
			{
    fnWriteStartElement("Variable internalName=\"PG3BeyondLowTH\" displayName=\"PG3 Beyond Low TH\" type=\"string\" /");
    fnWriteStartElement("Variable internalName=\"PG3BeyondHighTH\" displayName=\"PG3 Beyond High TH\" type=\"string\" /");
    fnWriteStartElement("Variable internalName=\"LiquidLevelLowBase\" displayName=\"Liquid Level Low Base\" type=\"string\" /"); 
		fnWriteStartElement("Variable internalName=\"LiquidLevelHighBase\" displayName=\"Liquid Level High Base\" type=\"string\" /");
    fnWriteStartElement("Variable internalName=\"LiquidLevelLowHardner\" displayName=\"Liquid Level Low Hardner\" type=\"string\" /");
    fnWriteStartElement("Variable internalName=\"LiquidLevelHighHardner\" displayName=\"Liquid Level High Hardner\" type=\"string\" /");
    fnWriteStartElement("Variable internalName=\"PipeTempNotinRange\" displayName=\"Pipe Temp Not in Range\" type=\"string\" /");
    fnWriteStartElement("Variable internalName=\"InlineBaseHeater\" displayName=\"Inline Base Heater\" type=\"string\" /");
    *ucNextData = *ucNextData + 1;
			}
		break;
		case 6:
			{
    fnWriteStartElement("Variable internalName=\"InlineHardnerHeater\" displayName=\"Inline Hardner Heater\" type=\"string\" /");
    fnWriteStartElement("Variable internalName=\"AntifreezHeater\" displayName=\"Antifreez Heater\" type=\"string\" /");
    fnWriteStartElement("Variable internalName=\"HardnerHeater\" displayName=\"HArdner Heater\" type=\"string\" /");
    fnWriteStartElement("Variable internalName=\"SprayGunMotion\" displayName=\"Spray Gun Motion\" type=\"string\" /");
    fnWriteStartElement("Variable internalName=\"LinearTransducer\" displayName=\"Linear Transducer\" type=\"string\" /");
    *ucNextData = *ucNextData + 1;
			}
		break;
		case 7:
			{
    fnWriteStartElement("Variable internalName=\"DCLSTime\" displayName=\"DCLS Time\" type=\"string\" /");
    fnWriteStartElement("Variable internalName=\"HLSTime\" displayName=\"HLS Time\" type=\"string\" /");
    fnWriteStartElement("Variable internalName=\"HMIHBAck\" displayName=\"HMI HB Ack\" type=\"string\" /");
    fnWriteStartElement("Variable internalName=\"OPMMHBAck\" displayName=\"OPMM HB Ack\" type=\"string\" /");
    fnWriteStartElement("Variable internalName=\"VCMHBAck\" displayName=\"VCM HB Ack\" type=\"string\" /");
    *ucNextData = *ucNextData + 1;
			}
		break;
		case 8:
			{
    fnWriteStartElement("Variable internalName=\"HCMHBAck\" displayName=\"HCM HB Ack Fault\" type=\"string\" /");
    fnWriteEndElement("Record");
    fnWriteStartElement("Record internalName=\"End\" displayName=\"End\"");
    fnWriteStartElement("Variable internalName=\"EndDate\" displayName=\"Date\" type=\"Date\" /");
    fnWriteStartElement("Variable internalName=\"EndTime\" displayName=\"Time\" type=\"Time\" /");
    fnWriteEndElement("Record");
    
    fnWriteEndElement("View");
    fnWriteStartElement("Data");
    *ucNextData = *ucNextData + 1;
			}
		break;
		default:
			{
				printf("\n\rDefault");
		*ucNextData = BYTE_ZERO;
  }
	}
}
/*****************************************************************************
**@Function			: fnFaultLog_Read
**@Descriptions	:	Read the One Fault data log from 
NVSRAM and send it to UART in CSV format.
**@parameters		: uliFaultStartAddr: Starting Address 
**@return				: None      
*******************************************************************************/
void fnFaultLog_Read(uint32_t uliFaultStartAddr)
{
  uint8_t rgucTimeStamp[6];
  uint8_t ucReadData = 0;
  uint8_t rgucTempData[100];
  
  memset(rgucXMLtxBuffer,'\0',strlen((char *)rgucXMLtxBuffer));
  fnWriteStartElement("Record internalName=\"Measurement\"");
  
  /*Time Stamp read and Display on UART*/
  fnNVSRAM1_MultiDataRead(6, uliFaultStartAddr, rgucTimeStamp);		
  
  sprintf((char *)rgucTempData,"20%02d/%02d/%02d %02d:%02d:%02d ", rgucTimeStamp[BYTE_ZERO],
          rgucTimeStamp[BYTE_ONE], rgucTimeStamp[BYTE_TWO], rgucTimeStamp[BYTE_THREE],
          rgucTimeStamp[BYTE_FOUR], rgucTimeStamp[BYTE_FIVE]);
  uliFaultStartAddr += 6;
  fnWriteData(rgucTempData);
  /*ucJobNameID change23*/
  ucReadData = fnNVSRAM1Byte_Read(uliFaultStartAddr++);
	
	#ifdef DBG_FAULT_WARN_LOGIC
	 sprintf((char *)rgucTempData,"%d ",ucReadData);
	 fnWriteData(rgucTempData);
	#endif
	
	
	/*ucSupervisorNameID; change23*/
	ucReadData = fnNVSRAM1Byte_Read(uliFaultStartAddr++);
	#ifdef DBG_FAULT_WARN_LOGIC
	 sprintf((char *)rgucTempData,"%d ",ucReadData);
	 fnWriteData(rgucTempData);
	#endif
	
	
  ucReadData = fnNVSRAM1Byte_Read(uliFaultStartAddr++)	;
  ((ucReadData >> 0) & 0x01)? fnWriteData("FAIL ") : fnWriteData("- ");
  ((ucReadData >> 1) & 0x01)? fnWriteData("FAIL ") : fnWriteData("- ");
  ((ucReadData >> 2) & 0x01)? fnWriteData("FAIL ") : fnWriteData("- ");
  ((ucReadData >> 3) & 0x01)? fnWriteData("FAIL ") : fnWriteData("- ");
  ((ucReadData >> 4) & 0x01)? fnWriteData("FAIL ") : fnWriteData("- ");
  ((ucReadData >> 5) & 0x01)? fnWriteData("FAIL ") : fnWriteData("- ");
  ((ucReadData >> 6) & 0x01)? fnWriteData("FAIL ") : fnWriteData("- ");
  ((ucReadData >> 7) & 0x01)? fnWriteData("FAIL ") : fnWriteData("- ");	
  
  
  ucReadData = fnNVSRAM1Byte_Read(uliFaultStartAddr++);
  ((ucReadData >> 0) & 0x01)? fnWriteData("FAIL ") : fnWriteData("- ");
  ((ucReadData >> 1) & 0x01)? fnWriteData("FAIL ") : fnWriteData("- ");
  ((ucReadData >> 2) & 0x01)? fnWriteData("FAIL ") : fnWriteData("- ");
  ((ucReadData >> 3) & 0x01)? fnWriteData("FAIL ") : fnWriteData("- ");
  ((ucReadData >> 4) & 0x01)? fnWriteData("FAIL ") : fnWriteData("- ");
  ((ucReadData >> 5) & 0x01)? fnWriteData("FAIL ") : fnWriteData("- ");
  ((ucReadData >> 6) & 0x01)? fnWriteData("FAIL ") : fnWriteData("- ");
  ((ucReadData >> 7) & 0x01)? fnWriteData("FAIL ") : fnWriteData("- ");		
  
  
  
  ucReadData = fnNVSRAM1Byte_Read(uliFaultStartAddr++);
  ((ucReadData >> 0) & 0x01)? fnWriteData("FAIL ") : fnWriteData("- ");
  ((ucReadData >> 1) & 0x01)? fnWriteData("FAIL ") : fnWriteData("- ");
  ((ucReadData >> 2) & 0x01)? fnWriteData("FAIL ") : fnWriteData("- ");
  ((ucReadData >> 3) & 0x01)? fnWriteData("FAIL ") : fnWriteData("- ");
  ((ucReadData >> 4) & 0x01)? fnWriteData("FAIL ") : fnWriteData("- ");
  ((ucReadData >> 5) & 0x01)? fnWriteData("FAIL ") : fnWriteData("- ");
  ((ucReadData >> 6) & 0x01)? fnWriteData("FAIL ") : fnWriteData("- ");
  ((ucReadData >> 7) & 0x01)? fnWriteData("FAIL ") : fnWriteData("- ");		
  
  
  
  ucReadData = fnNVSRAM1Byte_Read(uliFaultStartAddr++);
  ((ucReadData >> 0) & 0x01)? fnWriteData("FAIL ") : fnWriteData("- ");
  ((ucReadData >> 1) & 0x01)? fnWriteData("FAIL ") : fnWriteData("- ");
  ((ucReadData >> 2) & 0x01)? fnWriteData("FAIL ") : fnWriteData("- ");
  ((ucReadData >> 3) & 0x01)? fnWriteData("FAIL ") : fnWriteData("- ");
  ((ucReadData >> 4) & 0x01)? fnWriteData("FAIL ") : fnWriteData("- ");
 	((ucReadData >> 5) & 0x01)? fnWriteData("FAIL ") : fnWriteData("- ");
  ((ucReadData >> 6) & 0x01)? fnWriteData("FAIL ") : fnWriteData("- ");
	((ucReadData >> 7) & 0x01)? fnWriteData("FAIL ") : fnWriteData("- ");	
  
  ucReadData = fnNVSRAM1Byte_Read(uliFaultStartAddr++);
  ((ucReadData >> 0) & 0x01)? fnWriteData("FAIL ") : fnWriteData("- ");
  ((ucReadData >> 1) & 0x01)? fnWriteData("FAIL ") : fnWriteData("- ");
  ((ucReadData >> 2) & 0x01)? fnWriteData("FAIL ") : fnWriteData("- ");
  ((ucReadData >> 3) & 0x01)? fnWriteData("FAIL ") : fnWriteData("- ");
  ((ucReadData >> 4) & 0x01)? fnWriteData("FAIL ") : fnWriteData("- ");
  ((ucReadData >> 5) & 0x01)? fnWriteData("FAIL ") : fnWriteData("- ");
//  ((ucReadData >> 6) & 0x01)? fnWriteData("FAIL ") : fnWriteData("- ");
//	((ucReadData >> 7) & 0x01)? fnWriteData("FAIL ") : fnWriteData("- ");	
  fnWriteEndElement("Record");
}

/*****************************************************************************
**@Function			: fnFaultLogScan
**@Descriptions	: Scan Fault log for User ID and Job ID with Date and time
**@parameters		: None
**@return				: None      
*******************************************************************************/
void fnFaultLogScan(DATALOG_TX_t *pstLog)
{
	  uint8_t rgucTimeStamp[6];
  memset(rgucTimeStamp,'\0',strlen((char *)rgucTimeStamp)); 
	DateTime_t stLogDateTime;
  /*Time Stamp read */
  fnNVSRAM1_MultiDataRead(6, pstLog->uliStartAddress, rgucTimeStamp);		

	stLogDateTime.ucYear = rgucTimeStamp[BYTE_ZERO];
	stLogDateTime.ucMonth = rgucTimeStamp[BYTE_ONE];
	stLogDateTime.ucDate = rgucTimeStamp[BYTE_TWO];
	stLogDateTime.ucHour = rgucTimeStamp[BYTE_THREE];
	stLogDateTime.ucMin = rgucTimeStamp[BYTE_FOUR];
	stLogDateTime.ucSec = rgucTimeStamp[BYTE_FIVE];
	printf("\n\rLog Sec %X",fnGetDateTimeInSec(stLogDateTime));
	printf("\n\rStart Sec %X",pstLog->stStartLogDateTime.uliTotalSec);
	printf("\n\rEnd Sec %X",pstLog->stEndLogDateTime.uliTotalSec);
	
	if((pstLog->stStartLogDateTime.uliTotalSec <= fnGetDateTimeInSec(stLogDateTime)) &&
		(pstLog->stEndLogDateTime.uliTotalSec >= fnGetDateTimeInSec(stLogDateTime)))
	{
		if(RESET == pstLog->ucStartLogTimeFlag)
		{
		  pstLog->uliStartAddress += 6;
	
			/*ucJobNameID change23*/
			pstLog->ucNextUserID = pstLog->ucCurrUserID = fnNVSRAM1Byte_Read(pstLog->uliStartAddress++);
			
			/*ucSupervisorNameID; change23*/
			pstLog->ucNextJobID = pstLog->ucCurrJobID = fnNVSRAM1Byte_Read(pstLog->uliStartAddress++);
			fnDatalogStartFrame(pstLog->ucNextUserID,pstLog->ucNextJobID);
			fnWifiSendData(rgucXMLtxBuffer);
			pstLog->uliStartAddress -= 8;
			pstLog->ucStartLogTimeFlag = SET;
			printf("\n\rStart Log");
		}
		else
		{
			pstLog->uliStartAddress += 6;
			/*ucJobNameID change23*/
			pstLog->ucNextUserID = fnNVSRAM1Byte_Read(pstLog->uliStartAddress++);
			
			/*ucSupervisorNameID; change23*/
			pstLog->ucNextJobID = fnNVSRAM1Byte_Read(pstLog->uliStartAddress++);
			
			pstLog->uliStartAddress -= 8;
			if((pstLog->ucCurrUserID == pstLog->ucNextUserID) && 
				(pstLog->ucCurrJobID == pstLog->ucNextJobID))
			{
				fnFaultLog_Read(pstLog->uliStartAddress);
				//fnWifiSendData(rgucXMLtxBuffer);
				pstLog->uliStartAddress += DL_FLT_PACKET_SIZE;
				printf("\n\rData Log");
			}
			else if((pstLog->ucCurrUserID == pstLog->ucNextUserID) && 
				(pstLog->ucCurrJobID != pstLog->ucNextJobID))
			{
				pstLog->ucStartLogTimeFlag = RESET;
				fnDatalogEndFrame();
				//fnWifiSendData(rgucXMLtxBuffer);
				printf("\n\rEnd1 Log");
				pstLog->ucStopLogTimeFlag = SET;
			}
			else if(pstLog->ucCurrUserID != pstLog->ucNextUserID)
			{
				pstLog->ucStartLogTimeFlag = RESET;
				fnDatalogEndFrame();
				//fnWifiSendData(rgucXMLtxBuffer);
				printf("\n\rEnd2 Log");
				pstLog->ucStopLogTimeFlag = SET;
			}
		}
	}
	else
	{
		pstLog->uliStartAddress += DL_FLT_PACKET_SIZE;
		pstLog->ucStopLogTimeFlag = SET;
		printf("\n\rEnd Log");
	}
}
 /*****************************************************************************
**@Function			: fnFaultLog
**@Descriptions	: Send Fault log
**@parameters		: None
**@return				: None      
*******************************************************************************/
void fnFaultLog(DATALOG_TX_t *pstLog)
{
  if(FAULT_LOG == pstLog->ucLogType)
  {
    if(pstLog->ucDetailFlag > BYTE_ZERO)
    {
      printf("\n\rDatalog Detail %d",pstLog->ucDetailFlag);
			printf("\n\rStart %X, Stop %X",pstLog->uliStartAddress,pstLog->uliStopAddress);
      fnFaultLog_Detail(&pstLog->ucDetailFlag);
					pstLog->ucStartLogTimeFlag = RESET;
			//fnWifiSendData(rgucXMLtxBuffer);
    }
    else if(pstLog->uliStartAddress < pstLog->uliStopAddress)
    {
			printf("\n\rLogging 0x%X 0x%X",pstLog->uliStartAddress,pstLog->uliStopAddress);
      fnFaultLogScan(pstLog);
						//	 pstLog->uliStartAddress += DL_STA_PACKET_SIZE;
    }
    else //if(pstLog->ucStopLogTimeFlag == SET)
    {
      //pstLog->ucStopLogTimeFlag = RESET;
			//memset(rgucXMLtxBuffer,'\0',strlen((char *)rgucXMLtxBuffer));
			fnWriteEndElement("Data");
			fnWriteEndElement("CRC-Evans_Data");
      //fnWifiSendData(rgucXMLtxBuffer);
      pstLog->ucLogType = RESET;
      pstLog->ucSendFlag = RESET;
			/*change33*/
			stWarningLog.ucWarnLog_PeriodicSendWarnCounter = RESET;
			// stDatalog.ucWifiSendFlag = RESET;
    }
  }
}
 /*****************************************************************************
**@Function		 	: fnDataLoggingWifi
**@Descriptions	: This function is used to get data log 
**@parameters		: None
**@return				: None
*****************************************************************************/
void fnDataLoggingWifi(uint8_t *pucString,DATALOG_TX_t *pstLog)
{
  if(strstr((char *)pucString,"Get_Periodic_Log"))
  {
    if(RESET == pstLog->ucSendFlag)
    {
      /*Set flag if command recieved from WIFI*/
      if(PERIODIC_LOG != pstLog->ucLogType)
      {
				memset(pstLog,'\0',sizeof(DATALOG_TX_t));
        pstLog->ucLogType = PERIODIC_LOG;
        printf("               PERIODIC DATA LOG      ETHERNET       \r\n");
				uint8_t ucLowerdata = fnNVSRAM1Byte_Read(DL_PRD_WR_BASE_L);
				uint8_t ucMiddledata = fnNVSRAM1Byte_Read(DL_PRD_WR_BASE_M);
				uint8_t ucUpperdata = fnNVSRAM1Byte_Read(DL_PRD_WR_BASE_H);
				pstLog->uliStopAddress = (uint32_t)((uint32_t)(ucUpperdata << 16) | 
																							(uint32_t)(ucMiddledata << 8) | (uint32_t)(ucLowerdata));
				/* Resetting the UNWANTED BITS by Masking with 0xFFFFF*/
				pstLog->uliStopAddress = ( pstLog->uliStopAddress & PRD_MASK2 ); 
				pstLog->uliStartAddress = DL_PRD_SEG_START; /*For NVSRAM1*/
        pstLog->ucDetailFlag = SET;
				pstLog->ucSendFlag = SET;
      }
    }
  }
  else if(strstr((char *)pucString,"Get_Diagnostic_Log"))
  {
    if(RESET == pstLog->ucSendFlag)
    {
      /*Set flag if command recieved from Ethernet*/
      if(DIAGNOSTIC_LOG != pstLog->ucLogType)
      {
				memset(pstLog,'\0',sizeof(DATALOG_TX_t));
        fnGetLogStartStop(pucString,&stWifiLog);
        printf("               DIAGNOSTIC DATA LOG      ETHERNET       \r\n");
				uint8_t ucLowerdata = fnNVSRAM1Byte_Read(DL_DIAG_WR_BASE_L);
				uint8_t ucMiddledata = fnNVSRAM1Byte_Read(DL_DIAG_WR_BASE_M);
				uint8_t ucUpperdata = fnNVSRAM1Byte_Read(DL_DIAG_WR_BASE_H);
				pstLog->uliStopAddress = (uint32_t)((ucUpperdata << 16)| (ucMiddledata << 8)| (ucLowerdata));
				/* Resetting the UNWANTED BITS by Masking with 0x7FFF */
				pstLog->uliStopAddress = ( pstLog->uliStopAddress & DIAG_MASK ); 
				pstLog->uliStartAddress = DL_DIAG_SEG_START;  
        pstLog->ucDetailFlag = SET;
				pstLog->ucSendFlag = SET;
				pstLog->ucLogType = DIAGNOSTIC_LOG;
      }
    }
  }
  else if(strstr((char *)pucString,"Get_Status_Log"))
  {
    if(RESET == pstLog->ucSendFlag)
    {
      /*Set flag if command recieved from Wifi*/
      if(STATUS_LOG != pstLog->ucLogType)
      {
        pstLog->ucLogType = STATUS_LOG;
        printf("               STATUS DATA LOG     WIFI        \r\n");
				  /*Copying the BASE_REGISTER into Local variable uliStatusBaseAddr */
				uint8_t ucLowerdata = fnNVSRAM1Byte_Read(DL_STA_WR_BASE_L);
				uint8_t ucMiddledata = fnNVSRAM1Byte_Read(DL_STA_WR_BASE_M);
				uint8_t ucUpperdata = fnNVSRAM1Byte_Read(DL_STA_WR_BASE_H);
				pstLog->uliStopAddress = (uint32_t)((uint32_t)ucUpperdata << 16) | 
					((uint32_t)ucMiddledata << 8) | ((uint32_t)ucLowerdata);
				/*Resetting the UNWANTED BITS by Masking with 0x7FFF*/
				pstLog->uliStopAddress = ( pstLog->uliStopAddress & STA_MASK ); 
				/* Resetting the UNWANTED BITS by Masking with 0x7FFF*/
				pstLog->uliStartAddress = DL_STA_SEG_START; 
        pstLog->ucDetailFlag = SET;
				pstLog->ucSendFlag = SET;
      }
    }
  }
  else if(strstr((char *)pucString,"Get_Fault_Log"))
  {
    if(RESET == pstLog->ucSendFlag)
    {
      /*Set flag if command recieved from Wifi*/
      if(FAULT_LOG != pstLog->ucLogType)
      {
        pstLog->ucLogType = FAULT_LOG;
        printf("               FAULT DATA LOG     WIFI        \r\n");
				  /*Copying the BASE_REGISTER into Local variable uliStatusBaseAddr */
				uint8_t ucLowerdata = fnNVSRAM1Byte_Read(DL_FLT_WR_BASE_L);
				uint8_t ucMiddledata = fnNVSRAM1Byte_Read(DL_FLT_WR_BASE_M);
				uint8_t ucUpperdata = fnNVSRAM1Byte_Read(DL_FLT_WR_BASE_H);
				pstLog->uliStopAddress = (uint32_t)((ucUpperdata << 16)| (ucMiddledata << 8)| (ucLowerdata));
				/* Resetting the UNWANTED BITS by Masking with 0x7FFF*/
				pstLog->uliStopAddress = ( stDatalog.uliStopAddress & FLT_MASK );  
				
				pstLog->uliStartAddress = DL_FLT_SEG_START;  
        pstLog->ucDetailFlag = SET;
				pstLog->ucSendFlag = SET;
      }
    }
  }
  else if(strstr((char *)pucString,"Get_CAN_Config_Parameters"))
  {
    if(RESET == pstLog->ucSendFlag)
    {
      stDatalog.ucWifiSendFlag = RESET;
      printf("              Configuration Parameter     WIFI        \r\n");
      /*Set flag if command recieved from Wifi*/
      fnSendHMI_Config();
			
      stTx1Message.ExtId = MSG_TYPE_CONFIG | ACKNOWLEDGEMENT |
        DEST_TABLET | SOURCE_SCU;	
      /*Update Length of CAN Data Frame*/
      stTx1Message.DLC = 1;
      stTx1Message.Data[0] = 0x00;		/* Positive ACk */
      fnXML_FIFO(&stTx1Message);
    }
    
  }
	 else if(strstr((char *)pucString,"Get_Firmware_Version"))
  {
    if(RESET == pstLog->ucSendFlag)
    {
      stDatalog.ucWifiSendFlag = RESET;
      printf("              Get_Firmware_Version     WIFI        \r\n");
			
      /*SCU Firmware Version*/
      stTx1Message.DLC = FIVE_BYTES;
		stTx1Message.ExtId = STATUS_FIRMWARE_VERSION_NUMBER | MSG_TYPE_STATUS |
												 DEST_TABLET | SOURCE_SCU;	
		stTx1Message.Data[0] = ID_SCU; 
		SplitByte.fData = SCU_FIRMWARE_VERSION;
		/**************************DATA***********************************/
		for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
			{
		stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
			}
      fnXML_FIFO(&stTx1Message);
			
						/*HCM Firmware Version*/
      stTx1Message.DLC = FIVE_BYTES;
		stTx1Message.ExtId = STATUS_FIRMWARE_VERSION_NUMBER | MSG_TYPE_STATUS |
												 DEST_TABLET | SOURCE_SCU;	
		stTx1Message.Data[0] = ID_HCM; 
		SplitByte.fData = stCurrentData.fCurr_HCM_Firmware_Ver;
		/**************************DATA***********************************/
		for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
			{
		stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
			}
      fnXML_FIFO(&stTx1Message);
      
				/*VCM Firmware Version*/
      stTx1Message.DLC = FIVE_BYTES;
		stTx1Message.ExtId = STATUS_FIRMWARE_VERSION_NUMBER | MSG_TYPE_STATUS |
												 DEST_TABLET | SOURCE_SCU;	
		stTx1Message.Data[0] = ID_VCM; 
		SplitByte.fData = stCurrentData.fCurr_VCM_Firmware_Ver;
		/**************************DATA***********************************/
		for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
			{
		stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
			}
      fnXML_FIFO(&stTx1Message);
			
							/*OPMM Firmware Version*/
      stTx1Message.DLC = FIVE_BYTES;
		stTx1Message.ExtId = STATUS_FIRMWARE_VERSION_NUMBER | MSG_TYPE_STATUS |
												 DEST_TABLET | SOURCE_SCU;	
		stTx1Message.Data[0] = ID_OPMM; 
		SplitByte.fData = stCurrentData.fCurr_OPMM_Firmware_Ver;
		/**************************DATA***********************************/
		for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
			{
		stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
			}
      fnXML_FIFO(&stTx1Message);
    }
    
  }
  else if(strstr((char *)pucString,"Reset_Log"))
  {
    printf("                   RESET DATA LOG              \r\n");
    fnDatalogBaseReg_Reset();
  }
}

/*****************************************************************************
**@Function			: fnSend_Datalog
**@Descriptions	: Send Datalog 
**@parameters		: None
**@return				: None      
*******************************************************************************/
void fnSend_Datalog(void)
{
  if((SET == stEthernetLog.ucSendFlag) && (stEthernetLog.ucNextLogFlag == SET))
  {
    stEthernetLog.ucNextLogFlag = RESET;
    if(SET == stEthernet.ucConnectedStatus)
    {
			  memset(rgucXMLtxBuffer,'\0',strlen((char *)rgucXMLtxBuffer));
      //fnETHGetConfigParameter();
			fnDiagnosticLog(&stEthernetLog);
			fnPeriodicLog(&stEthernetLog);
			fnStatusLog(&stEthernetLog);
			fnFaultLog(&stEthernetLog);
//      fnETHSend_PRD_log();
      //fnETHSend_Diag_log();
      //fnETHSend_Status_log();
      //fnETHSend_Fault_log();	
			fnEthernetSend(rgucXMLtxBuffer);
    }
	}
	if(SET == stWifiLog.ucSendFlag)
  {
    if(SET == stWifi.ucConnectedStatus)
    {
			  memset(rgucXMLtxBuffer,'\0',strlen((char *)rgucXMLtxBuffer));
      //fnWifiGetConfigParameter();
			fnDiagnosticLog(&stWifiLog);
			fnPeriodicLog(&stWifiLog);
			fnStatusLog(&stWifiLog);
			fnFaultLog(&stWifiLog);
      //fnWifiSend_Fault_log();
      //fnWifiSend_Status_log();
      //fnWifiSend_Diag_log();
      //fnWifiSend_PRD_log();
			fnWifiSendData(rgucXMLtxBuffer);
    }
  }
}




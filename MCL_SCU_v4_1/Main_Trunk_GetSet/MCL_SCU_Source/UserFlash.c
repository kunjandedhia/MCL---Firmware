/******************************************************************************
**@file     :   UserFlash.c
* 
**@brief    :   Flash Related Functions.
******************************************************************************
*/    
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
/*change24 ADDED UserFlash.c file*/
uint8_t ucData[FLASH_RX_TX_BUFFER_SIZE];/*change24*/
char supervisorname[FLASH_RX_TX_BUFFER_SIZE];/*change24*/
char jobname[FLASH_RX_TX_BUFFER_SIZE];/*change24*/
char jobid[FLASH_RX_TX_BUFFER_SIZE];/*change42*/
//uint8_t rgucDetailData[22];

/*change78*/
char jobnamepre[FLASH_RX_TX_BUFFER_SIZE];
char jobnamebac[FLASH_RX_TX_BUFFER_SIZE];


char supervisornamepre[FLASH_RX_TX_BUFFER_SIZE];
char supervisornamebac[FLASH_RX_TX_BUFFER_SIZE];


char jobidpre[FLASH_RX_TX_BUFFER_SIZE];
char jobidbac[FLASH_RX_TX_BUFFER_SIZE];

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
**@Function 			: fnFlashWrite
**@Descriptions	: Store Data to internal FLSAH memory
**@parameters		: uliAddress : Write location
pucData : Pointer to Data
ucNoOfBytes : No. of bytes to write
**@return				: None
*****************************************************************************/
void fnFlashWrite(uint32_t uliAddress, char *pucData)
{
  /* Clear pending flags (if any) */  
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGAERR | FLASH_FLAG_WRPERR); 
  while (*pucData != '\0')
  {	
    if (FLASH_ProgramByte(uliAddress++, *pucData) == FLASH_COMPLETE)
    {
      pucData++;
      
    }
    else
    { 
      /* Error occurred while writing data in Flash memory. 
      User can add here some code to deal with this error */
      while (1)
      {
        printf("Error occurred while writing data in Flash memory. User can add here some code to deal with this error\n\r");
				break;
			}
    }
  }
  FLASH_ProgramByte(uliAddress, '\0');
}
/*****************************************************************************
**@Function 			: fnFlashRead
**@Descriptions	: Read Data from internal FLSAH memory
**@parameters		: uliAddress : Read location
ucNoOfBytes : No. of bytes to Read
**@return				: pucData : Pointer to Data
*****************************************************************************/
uint8_t* fnFlashRead(uint32_t uliAddress)
{
  
  uint8_t ucPos=0;
  memset(ucData,'\0',FLASH_RX_TX_BUFFER_SIZE);
  ucData[ucPos] = *(__IO uint32_t *)uliAddress;
  uliAddress++;
	//		printf("\n\rData %x",ucData[ucPos]);
  while(ucData[ucPos] != '\0')
  {

    
		if(ucData[ucPos] == 0xff) 
			break;
		ucPos++;
    ucData[ucPos] = *(__IO uint32_t *)uliAddress;
    uliAddress++;
	//			printf("%x",ucData[ucPos]);
  }
  
  return ucData;
}

/*****************************************************************************
**@Function 			: fnResetName
**@Descriptions	: Erase internal flash memory section used for supervisor 
name and job name
**@parameters		: None
**@return				: None
*****************************************************************************/
void fnResetName(uint16_t Sector_No)
{
  
  /* Unlock the Flash to enable the flash control register access *************/ 
  FLASH_Unlock();
  if(FLASH_Sector_6 == Sector_No)
  {
		while(FLASH_EraseSector(FLASH_Sector_6,VoltageRange_3) == FLASH_BUSY)
		{
		}
  }
  else if(FLASH_Sector_7 == Sector_No)
  {
		while(FLASH_EraseSector(FLASH_Sector_7,VoltageRange_3) == FLASH_BUSY)
		{
		}
  }
	else if(FLASH_Sector_8 == Sector_No)
  {
		while(FLASH_EraseSector(FLASH_Sector_8,VoltageRange_3) == FLASH_BUSY)
		{
		}
  }
  /* Lock the Flash to disable the flash control register access (recommended
  to protect the FLASH memory against possible unwanted operation) *********/
  FLASH_Lock(); 
}
/*****************************************************************************
**@Function 			: fnStoreSupervisorName
**@Descriptions	: Store SupervisorName to internal FLSAH memory
**@parameters		: None
**@return				: None
*****************************************************************************/
void fnStoreSupervisorName(uint8_t INDEX_NUMBER,char *pucData)
{
  
  /* Unlock the Flash to enable the flash control register access *************/ 
  FLASH_Unlock();
  fnFlashWrite(FLASH_SUPERVISOR_NAME_ADDR(INDEX_NUMBER),pucData); 
  /* Lock the Flash to disable the flash control register access (recommended
  to protect the FLASH memory against possible unwanted operation) *********/
  FLASH_Lock(); 
}
/*****************************************************************************
**@Function 			: fnRetriveSupervisorName
**@Descriptions	: Retrive SupervisorName from internal FLSAH memory
**@parameters		: None
**@return				: None
*****************************************************************************/
void fnRetriveSupervisorName(uint8_t INDEX_NUMBER)
{
  
  if(INDEX_NUMBER == ucSupervisorNameID)
  {
						memset(supervisorname,'\0',FLASH_RX_TX_BUFFER_SIZE);
            strcat((char *)supervisorname,(char *)stConfigData.rgucConfig_SupervisorName1);
            strcat((char *)supervisorname,(char *)stConfigData.rgucConfig_SupervisorName2);
            strcat((char *)supervisorname,(char *)stConfigData.rgucConfig_SupervisorName3);	
   }
	else if(INDEX_NUMBER < ucSupervisorNameID)
	{
	memset(supervisorname,'\0',FLASH_RX_TX_BUFFER_SIZE);
  strncpy((char*)&supervisorname[0],(char*)fnFlashRead(FLASH_SUPERVISOR_NAME_ADDR(INDEX_NUMBER)),22);
/*change57*/
		if(supervisorname[0] == 0xFF)
	{
		ucSupervisorNameID = DEFAULT_CURRENT_INDEX_SUPERVISOR_NAME;
		ucSupervisorNameLog = DEFAULT_CURRENT_INDEX_SUPERVISOR_NAME;
		 fnSPI_EEPROM_WriteData(CURRENT_INDEX_SUPERVISOR_NAME, 1, &ucSupervisorNameID); /*change78*/
								 fnSPI_EEPROM_WriteData(LOG_INDEX_SUPERVISOR_NAME, 1, &ucSupervisorNameLog); /*change78*/
	}
					
	}
	else
	{
		memset(supervisorname,'\0',FLASH_RX_TX_BUFFER_SIZE);
		strcpy(supervisorname,"ERROR");
	}
}


/*****************************************************************************
**@Function 			: fnStoreJobName
**@Descriptions	: Store JobName to internal FLSAH memory
**@parameters		: None
**@return				: None
*****************************************************************************/
void fnStoreJobName(uint8_t INDEX_NUMBER,char *pucData)
{
  
  /* Unlock the Flash to enable the flash control register access *************/ 
  FLASH_Unlock();
  fnFlashWrite(FLASH_JOB_NAME_ADDR(INDEX_NUMBER),pucData); 
  /* Lock the Flash to disable the flash control register access (recommended
  to protect the FLASH memory against possible unwanted operation) *********/
  FLASH_Lock(); 
}
/*****************************************************************************
**@Function 			: fnRetriveJobName
**@Descriptions	: Retrive JobName from internal FLSAH memory
**@parameters		: None
**@return				: None
*****************************************************************************/
void fnRetriveJobName(uint8_t INDEX_NUMBER)
{
 
  if(INDEX_NUMBER == ucJobNameID)
  {
  		memset(jobname,'\0',FLASH_RX_TX_BUFFER_SIZE);
            strcat((char *)jobname,(char *)stConfigData.rgucConfig_JobName1);
            strcat((char *)jobname,(char *)stConfigData.rgucConfig_JobName2);
            strcat((char *)jobname,(char *)stConfigData.rgucConfig_JobName3);	
	
  
  }
	else if(INDEX_NUMBER < ucJobNameID)
	{
		memset(jobname,'\0',FLASH_RX_TX_BUFFER_SIZE);
    strncpy((char*)&jobname[0],(char*)fnFlashRead(FLASH_JOB_NAME_ADDR(INDEX_NUMBER)),22);
 /*change57*/
		if(jobname[0] == 0xFF)
		{
			ucJobNameID = DEFAULT_CURRENT_INDEX_JOB_NAME;
			ucJobNameLog = DEFAULT_CURRENT_INDEX_JOB_NAME; /*change78*/
			fnSPI_EEPROM_WriteData(CURRENT_INDEX_JOB_NAME, 1, &ucJobNameID);
			fnSPI_EEPROM_WriteData(LOG_INDEX_JOB_NAME, 1, &ucJobNameLog); /*change78*/
		}
	}
	else
	{
		memset(jobname,'\0',FLASH_RX_TX_BUFFER_SIZE);
		strcpy(jobname,"ERROR");
	}
}

/*change48*/
/*****************************************************************************
**@Function 			: fnStoreJobID
**@Descriptions	: Store JobID to internal FLSAH memory
**@parameters		: None
**@return				: None
*****************************************************************************/
void fnStoreJobID(uint8_t INDEX_NUMBER,char *pucData)
{
  
  /* Unlock the Flash to enable the flash control register access *************/ 
  FLASH_Unlock();
  fnFlashWrite(FLASH_JOB_ID_ADDR(INDEX_NUMBER),pucData); 
  /* Lock the Flash to disable the flash control register access (recommended
  to protect the FLASH memory against possible unwanted operation) *********/
  FLASH_Lock(); 
}
/*****************************************************************************
**@Function 			: fnRetriveSupervisorName
**@Descriptions	: Retrive SupervisorName from internal FLSAH memory
**@parameters		: None
**@return				: None
*****************************************************************************/
void fnRetriveJobID(uint8_t INDEX_NUMBER)
{
  
  if(INDEX_NUMBER == ucJobID)
  {
			memset(jobid,'\0',FLASH_RX_TX_BUFFER_SIZE);
            strcat((char *)jobid,(char *)stConfigData.rgucConfig_JobId1);
            strcat((char *)jobid,(char *)stConfigData.rgucConfig_JobId2);
            strcat((char *)jobid,(char *)stConfigData.rgucConfig_JobId3);	
   }
	else if(INDEX_NUMBER < ucJobID)
	{
	memset(jobid,'\0',FLASH_RX_TX_BUFFER_SIZE);
  strncpy((char*)&jobid[0],(char*)fnFlashRead(FLASH_JOB_ID_ADDR(INDEX_NUMBER)),22);
 /*change57*/
		if(jobid[0] == 0xFF)
		{
				ucJobID = DEFAULT_CURRENT_INDEX_JOB_ID;
				ucJobLog = DEFAULT_CURRENT_INDEX_JOB_ID; /*change78*/
			fnSPI_EEPROM_WriteData(CURRENT_INDEX_JOB_ID, 1, &ucJobID); /*change78*/
								 fnSPI_EEPROM_WriteData(LOG_INDEX_JOB_ID, 1, &ucJobLog); /*change78*/
				
		}	
	}
	else
	{
		memset(jobid,'\0',FLASH_RX_TX_BUFFER_SIZE);
		strcpy(jobid,"ERROR");
	}
}


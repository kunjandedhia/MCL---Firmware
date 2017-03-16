/******************************************************************************
**@file     :   Main.c
* 
**@brief    :   Main program body.
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
**@Function      : Main
**@Descriptions	: Main 
**@parameters    : None
**@Return        : None
*****************************************************************************/
int main(void)
{
  /*Main*/
  uint8_t ucProcess = 0;
	
  /*system initialization*/  
  fnSystem_Init();
  
#ifdef DBG
  printf("\n\rSCU INIT\r\n");
#endif
  
  /*Read Config Data from EEPROM */
  fnConfigData_Read();
	fnWifiApplysSetting();
  /*Ethernet configuration*/
  //fnEthernetConfig();
  /*Enable Timers*/
  TIM_Cmd(TIM1, ENABLE);
  StatusFlag.stStatusFlag.bDefinedState = SET;
  //	StatusFlag.stStatusFlag.bStartSignal=SET;
  
  
  
  while(RESET	== StatusFlag.stStatusFlag.bStartSignal)
  {
    if(STARTUP_DELAY_SCU > uiDefinedStateCnt)
    {
      /*Default Active CAN*/
      uiIOExpData = RESET_CONFIG_RED;
      
    }
    else 
    {
      if(STARTUP_DELAY_SCU1 < uiDefinedStateCnt)
      {
        uiDefinedStateCnt= 0;
        /*Default Active CAN*/
        uiIOExpData = RESET_CONFIG_BLUE;
        GPIO_SetBits(HOOTER_SELECT_PORT, HOOTER_SELECT);
        
      }
      /*LED Indication Function*/
      fnLED_Indication();	
	
    }
    /* Process Data recieved Over Wi-Fi */
    //fnWifiProcessData();
    
  }
	
	#ifdef FACTORY_RESET
	fnWifiDefaultConfig();
  	/*change23*/
  			ucSupervisorNameID = DEFAULT_CURRENT_INDEX_SUPERVISOR_NAME;
  				ucSupervisorNameLog = DEFAULT_CURRENT_INDEX_SUPERVISOR_NAME;
		 fnSPI_EEPROM_WriteData(CURRENT_INDEX_SUPERVISOR_NAME, 1, &ucSupervisorNameID); /*change78*/
								 fnSPI_EEPROM_WriteData(LOG_INDEX_SUPERVISOR_NAME, 1, &ucSupervisorNameLog); /*change78*/
  				/*change24*/
 	fnResetName(FLASH_Sector_6);
 		/*change23*/
 				ucJobNameID = DEFAULT_CURRENT_INDEX_JOB_NAME;
 				ucJobNameLog = DEFAULT_CURRENT_INDEX_JOB_NAME; /*change78*/
			fnSPI_EEPROM_WriteData(CURRENT_INDEX_JOB_NAME, 1, &ucJobNameID);
			fnSPI_EEPROM_WriteData(LOG_INDEX_JOB_NAME, 1, &ucJobNameLog); /*change78*/
 				/*change24*/
 				fnResetName(FLASH_Sector_7);	
  	
//  printf("ucSupervisorNameID = %d \n\r",ucSupervisorNameID);
//  printf("supervisorname = %s \n\r",supervisorname);
  
	/* Testing Reset Datalog address*/ 
  fnDatalogBaseReg_Reset();
  stCurrentData.fCurr_BaseMaterialPrimedAccumulate=1000.2;
stCurrentData.fCurr_HardMaterialPrimedAccumulate=2000.8;


stCurrentData.fCurr_BaseMaterialAppliedAccumulate=3000.5;
stCurrentData.fCurr_HardMaterialAppliedAccumulate=4000;
stCurrentData.fCurr_BaseTotalMaterialUsedAccumulate=10000.5;
stCurrentData.fCurr_HardTotalMaterialUsedAccumulate=15000.8;
stCurrentData.fCurr_BaseTotalMaterialUsed=3000;
stCurrentData.fCurr_HardTotalMaterialUsed=1500;
/*change43 CONFIG PARAMETER NO */
            ucEEPROMCounter = CONFIG_PARA_120;
#endif
//fnWifiDefaultConfig();
/*change23*/
// 			ucSupervisorNameID = DEFAULT_CURRENT_INDEX_SUPERVISOR_NAME;
// 				ucSupervisorNameLog = DEFAULT_CURRENT_INDEX_SUPERVISOR_NAME;
//		 fnSPI_EEPROM_WriteData(CURRENT_INDEX_SUPERVISOR_NAME, 1, &ucSupervisorNameID); /*change78*/
//								 fnSPI_EEPROM_WriteData(LOG_INDEX_SUPERVISOR_NAME, 1, &ucSupervisorNameLog); /*change78*/
//  				/*change24*/
// 	fnResetName(FLASH_Sector_6);
// 		/*change23*/
//			ucJobNameID = DEFAULT_CURRENT_INDEX_JOB_NAME;
// 				ucJobNameLog = DEFAULT_CURRENT_INDEX_JOB_NAME; /*change78*/
//			fnSPI_EEPROM_WriteData(CURRENT_INDEX_JOB_NAME, 1, &ucJobNameID);
//		fnSPI_EEPROM_WriteData(LOG_INDEX_JOB_NAME, 1, &ucJobNameLog); /*change78*/
//// 				/*change24*/
//				fnResetName(FLASH_Sector_7);
///*change48*/				
//				 				ucJobID = DEFAULT_CURRENT_INDEX_JOB_ID;
// 					ucJobLog = DEFAULT_CURRENT_INDEX_JOB_ID; /*change78*/
//			fnSPI_EEPROM_WriteData(CURRENT_INDEX_JOB_ID, 1, &ucJobID); /*change78*/
//								 fnSPI_EEPROM_WriteData(LOG_INDEX_JOB_ID, 1, &ucJobLog); /*change78*/
// 				/*change24*/
// 				fnResetName(FLASH_Sector_8);	
//stCurrentData.fCurr_BaseMaterialPrimedAccumulate=1000000;
//stCurrentData.fCurr_HardMaterialPrimedAccumulate=4000000;


//stCurrentData.fCurr_BaseMaterialAppliedAccumulate=3000000;
//stCurrentData.fCurr_HardMaterialAppliedAccumulate=9000000;
//stCurrentData.fCurr_BaseTotalMaterialUsedAccumulate=8000000;
//stCurrentData.fCurr_HardTotalMaterialUsedAccumulate=6000000;
//stCurrentData.fCurr_BaseTotalMaterialUsed=3000;
//stCurrentData.fCurr_HardTotalMaterialUsed=1500;
///*change43 CONFIG PARAMETER NO */
//           ucEEPROMCounter = CONFIG_PARA_120;
/* Testing Reset Datalog address*/ 
// fnDatalogBaseReg_Reset();
 //printf("ucSupervisorNameID = %d \n\r",ucSupervisorNameID);
  //printf("supervisorname = %s \n\r",supervisorname);
	printf("\n\r ucSupervisorNameID = %d \n\r",ucSupervisorNameID);
		printf("ucSupervisorNameLog = %d \n\r",ucSupervisorNameLog);
		memset(supervisorname,'\0',FLASH_RX_TX_BUFFER_SIZE);
    strcat((char *)supervisorname,(char *)stConfigData.rgucConfig_SupervisorName1);
            strcat((char *)supervisorname,(char *)stConfigData.rgucConfig_SupervisorName2);
            strcat((char *)supervisorname,(char *)stConfigData.rgucConfig_SupervisorName3);	
		printf(" EEPROM supervisorname = %s \n\r",supervisorname);
  if(0 < ucSupervisorNameID)
  {
    for(ucProcess=0;ucProcess<ucSupervisorNameID;ucProcess++)
    {
      fnRetriveSupervisorName(ucProcess);	
      printf(" Flash supervisorname(%d) = %s \n\r",ucProcess,supervisorname);
    }
  }	
/*change58*/	
  else
	{
			fnResetName(FLASH_Sector_6);
	}
	
		printf("ucJobNameID = %d \n\r",ucJobNameID);
		printf(" ucJobNameLog = %d \n\r",ucJobNameLog);
		memset(jobname,'\0',FLASH_RX_TX_BUFFER_SIZE);
    strcat((char *)jobname,(char *)stConfigData.rgucConfig_JobName1);
    strcat((char *)jobname,(char *)stConfigData.rgucConfig_JobName2);
    strcat((char *)jobname,(char *)stConfigData.rgucConfig_JobName3);
		printf(" EEPROM jobname = %s \n\r",jobname);
  
  if(0 < ucJobNameID)
  {
    for(ucProcess=0;ucProcess<ucJobNameID;ucProcess++)
    {
      fnRetriveJobName(ucProcess);	
     printf(" FLASH jobname(%d) = %s \n\r",ucProcess,jobname);
    }
  }	
	/*change58*/
	else
				{
					fnResetName(FLASH_Sector_7);
				}	
	
	printf(" ucJobID = %d \n\r",ucJobID);
		printf(" ucJobLog = %d \n\r",ucJobLog);
		memset(jobid,'\0',FLASH_RX_TX_BUFFER_SIZE);
            strcat((char *)jobid,(char *)stConfigData.rgucConfig_JobId1);
            strcat((char *)jobid,(char *)stConfigData.rgucConfig_JobId2);
            strcat((char *)jobid,(char *)stConfigData.rgucConfig_JobId3);	
		printf(" EEPROM jobid = %s \n\r",jobid);
  
  if(0 < ucJobID)
  {
    for(ucProcess=0;ucProcess<ucJobID;ucProcess++)
    {
      fnRetriveJobID(ucProcess);	
      printf("FLASH jobid(%d) = %s \n\r",ucProcess,jobid);
    }
  }
	/*change58*/
		else
				{
					fnResetName(FLASH_Sector_8);
				}	
	
  
  //	fnRetriveSupervisorName(1);		
  //	printf("supervisorname = %s \n\r",supervisorname);
  GPIO_ResetBits(HOOTER_SELECT_PORT, HOOTER_SELECT);
  StatusFlag.stStatusFlag.bDefinedState = RESET ;
  /*Default Configuration*/
  fnReset_Configuration();
  
  //TIM_Cmd(TIM2, ENABLE);
  
  
  /* Take Diagnostic DATA LOG */
  //	fnDatalog_Diag(Diag_Log.rgucDiagLogFlag);
#ifdef DBG
  printf("\n\rWHILE STARTS\r\n");
#endif

  
  
  //	
  //		fnNVSRAM1MultiData_Write(3, DL_PRD_WR_BASE_L, rguPRDBase_Reset);
  //		printf("\n\rNVSRAM DATA at DL_PRD_WR_BASE_L =%x ,%x ,%x , %x \r\n",
  //		rguPRDBase_Reset[0],rguPRDBase_Reset[1],rguPRDBase_Reset[2] , DL_PRD_WR_BASE_L);
  //		
  //		fnNVSRAM1_MultiDataRead(3, DL_PRD_WR_BASE_L, rgucTimeStamp);
  //		
  //		printf("\n\rNVSRAM DATA at DL_PRD_WR_BASE_L =%x ,%x ,%x \r\n",
  //		rgucTimeStamp[0],rgucTimeStamp[1],
  //		rgucTimeStamp[2]);	
  //	fnDelay_ms(1000);
  
  
  /*Send Config Parameters */
  //	fnSendHMI_Config();	
   
  while(1)
  {
  
    /* Process CAN Message */
    for(ucProcess = 0; ucProcess < MSG_PROCESS_CNT; ucProcess++)
    {
      if((uiCANProcessCnt < uiStoreRxCANBuffCnt)||
         (SET == StatusFlag.stStatusFlag.bDataInCANRx_BuffFlag))
      {		
        fnCANMsg_Validate(&rgstRxCANBuff[uiCANProcessCnt++]);
        
#ifdef DBG
        //       	printf("\n\ruiStoreRxCANBuffCnt= %d\r\n", uiStoreRxCANBuffCnt);	
        //				printf("uiCANProcessCnt= %d\r\n", uiCANProcessCnt);
#endif
        if(uiCANProcessCnt >= RX_CAN_BUF_SIZE)
        {					
          uiCANProcessCnt = 0;
          StatusFlag.stStatusFlag.bDataInCANRx_BuffFlag = RESET;
        }
        /* Write Updated Config Parameters */
        fnEEPROMConfig_Write();
				/*change77*/
        if(SET == StatusFlag.stStatusFlag.bCanRemote)
					{
						StatusFlag.stStatusFlag.bCanRemote = RESET;
					fnSCU_Perform_Process();
					}
      }
      //	 printf("uiCANProcessCnt= %d\r\n", uiCANProcessCnt);	
    }	
    
    
    /* SCU Operating Modes */
    switch (ucSCUMode) 
    {
      /* Default Mode */
    case SCU_DEFAULT_MODE:
      //	printf("1\r\n");	
      fnDefault_Mode();
      //	printf("2\r\n");
      break;
      /* Operation Mode */
    case SCU_OPERATION_MODE:
      //	printf("3\r\n");
      fnOperation_Mode();
      //		printf("4\r\n");
      break;
      /* Default Mode */
    default:
      ucSCUMode = SCU_DEFAULT_MODE;
      break;
      
    }
    
    
    
    /*SCU OPERATION*/	
    //		printf("5\r\n");
    fnSCU_Operation();	
	//	printf("6\r\n");
    /*Check for the ESTOP message*/
    
    fnEstop_Check();
	//	printf("7\r\n");
    /* FOR TESTING OF PROCESS */
    //fnSCU_Perform_Process();
    /*Periodic heartbeat after 250msec*/
    if(SET == StatusFlag.stStatusFlag.bStartPeriodicHB)
    {
      fnPeriodicHeartbeat();
    }
	//	printf("8\r\n");
    /*Check for Ack Timeout after 250ms of msg Tx*/
    fnFifoAckTimeout_Check();/*for Post ack checking*/
    /*LED Indication Function*/
    //		printf("9\r\n");
    fnLED_Indication();
    //		printf("10\r\n");
    
    
    
  }/*While(1)*/
}/*Main*/






/******************************************************************************
**@file     :   Miscellaneous.c
* 
**@brief    :   This file contains the Miscellaneous functions  
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
/*Used for Reciever Inputs*/
TypeDef_RecievedCommand stRecieverInput;
/*Used for Process Inputs*/
TypeDef_ProcessedCommand stProcessInput;

uint8_t ucStalFailureCount = 0;
uint16_t uiAmbiantReadCnt = 0;
uint16_t uiRTCReadCnt = 0;
uint16_t uiRLTempReadCnt = 0;
//float stCurrentData.fCurr_PressureRegRead = 0.0f;
float fPressureRegWrite = 0.0f;
/*Test only*/
uint8_t ucTempCountTEST = 0;
/*change61*/
/* Each bit tells to Diagnostic status recieved from respective device*/
uint8_t ucDeviceDiagRx = 0;    
/* Each bit tells to Diagnostic status of respective device*/
uint8_t ucDeviceDiagStatus = 0;    
/* Each bit tells to config parameters sent to respective device*/	
uint8_t ucDeviceConfigSent = 0;
/* Each bit tells to FIRST BOOT ACK Recieved*/	
uint8_t ucDeviceFirstBootUp = 0;


uint16_t uiIOExpData = 0;/*EXP1 lower Port*/
uint8_t ucIOExpSlaveAddr = 0;/*EXP1 higher Port*/
uint8_t ucIOExp2LowerByte = 0;/*EXP2 lower Port*/
uint8_t ucIOExp2HigherByte = 0;/*EXP2 Higher Port*/
uint16_t uiI2CWriteData;
uint16_t uiPreIOExpander;

uint8_t ucSCUMode = SCU_DEFAULT_MODE;
uint16_t uiDiagnosticTimer = 0;
uint16_t uiStartUpDelayCount = 0;

uint16_t uiCANFaultLEDBlinkCnt = 0;
uint16_t uiDefinedStateCnt=0;
uint16_t uiOperationTimeOut = 0;

// uint8_t ucCANLinkFaultCnt = 0;
uint16_t uiPressureRegReadCnt = 0;

uint8_t ucI2CRxCnt = 0;
uint8_t ucTempHighByte = 0;
uint8_t ucTempLowByte = 0;
uint8_t rgucI2CRxBuff[2];
uint8_t ucSlave_Add = 0;
uint8_t ucI2CSequence = 0; 

uint8_t ucDebCounter=0;
uint8_t ucReleaseStartCounter=0;
uint8_t ucBackUpReciever=0;
uint8_t ucPreviousProcess=FLUSHPROCESS;
uint8_t ucCurrentProcess=0;
uint16_t uiStallcommandsendCnt = 0; /*Counter for periodic log*/
uint8_t ucPRDSpray; /*status to take periodic log change15*/

uint8_t rgucIPADDRESSBuff[15];/*change20*/
/* ----------------------------------------------------------------------------
*                           EXTERNAL VARIABLES
* ----------------------------------------------------------------------------
*/

/* ----------------------------------------------------------------------------
*                           EXTERNAL FUNCTIONS
* ----------------------------------------------------------------------------
*/

/* ----------------------------------------------------------------------------
*                           FUNCTIONS DECLARITION
* ----------------------------------------------------------------------------
*/

/*****************************************************************************
**@Function		 	:  fnReset_Configuration
**@Descriptions	:  This function is used to configure the default values 
**@parameters		:  None
**@return				:  None
*****************************************************************************/
void fnReset_Configuration(void)
{
  /*CAN1   GREEN STATUS LED ON */
  ClearBit(uiIOExpData , CAN1_STATUS_RED_LED);
  ClearBit(uiIOExpData , CAN1_STATUS_BLUE_LED);
  SetBit(uiIOExpData , CAN1_STATUS_GREEN_LED);
  /*CAN2   GREEN STATUS LED OFF */
  ClearBit(uiIOExpData , CAN2_STATUS_RED_LED);
  ClearBit(uiIOExpData , CAN2_STATUS_BLUE_LED);
  ClearBit(uiIOExpData , CAN2_STATUS_GREEN_LED);
  ClearBit(uiIOExpData , ESTOP_RED_LED);
  ClearBit(uiIOExpData , ESTOP_BLUE_LED);
  SetBit(uiIOExpData , ESTOP_GREEN_LED);
}



/*****************************************************************************
**@Function		:	 	fnDefault_Mode
**@Descriptions: 	This function is implemented for Default Mode of MCU
**@parameters	:  	None
**@return			:  	None
*****************************************************************************/
void fnDefault_Mode(void)
{
  /* Perform Diagnostic of SCU */
  if(RESET == StatusFlag.stStatusFlag.bDiagnosticDone)
  {
#ifdef DBG_DEFAULT_MODE
	printf("IN DIAGNOSTIC\r\n");
#endif
	fnSCU_Diagnostic();
	
 	 		
  }
  /* Handshaking with HMI/Tablet of SCU */
  if((RESET == StatusFlag.stStatusFlag.bHandshakeDone)&&
     (SET == StatusFlag.stStatusFlag.bDiagnosticDone))
  {
#ifdef DBG_DEFAULT_MODE
	printf("IN HANDSHAKE\r\n");
#endif
    
    fnSCU_Handshake(); 
    
    
  }	
  /* SCU Send Config Parameters to HMI/Tablet */
  /*Handshake flag says SCU Communication between HMI & TABLET 
  has established */
  /*Saved Config parameters not sent to HMI or Tablet*/
  /*Diagnostic of SCU Passed */
  if((SET == StatusFlag.stStatusFlag.bHandshakeDone)&&      
     (RESET == StatusFlag.stStatusFlag.bHMIConfigDone)&&
       (RESET == Diag_Log.stDiag_Log_Flag.bDiag_Log_SCU))
  {
#ifdef DBG_DEFAULT_MODE
    printf("IN HMI CONFIG\r\n");
#endif
    /*Send Config Parameters */
    /* If Diagnostic of HMI Passed */
	if(ucDeviceDiagStatus & 0x01)
    {
      fnSendHMI_Config();
      StatusFlag.stStatusFlag.bHMIConfigDone = SET;
      
    }			
  }
  /*Set Flag to Start Periodic Heartbeat if Config Parameters sent to HMI*/
  if((ucDeviceConfigSent & 0x01)&&
     (RESET == StatusFlag.stStatusFlag.bStartPeriodicHB))
  {
#ifdef DBG_DEFAULT_MODE
    printf("IN START HEARTBEAT\r\n");
#endif
    StatusFlag.stStatusFlag.bStartPeriodicHB = SET ;
  }	
  /*Check if 1st boot up acknowledge recieved from modules*/
  /*Send config parameters to particular module whose 1st 
  HB Ack is recieved */
  /*Check if config Parameters Sent	*/
  if( ALL_SLAVE_DEVICE_CONFIG_SEND <= (ucDeviceConfigSent & 0x0F))
  {
	/*Start Diagnostic Timer */
    StatusFlag.stStatusFlag.bStartDiagTimer = SET;
  }	
  /*Wait until Diagnostic Timer Elasped */
  if((DIAGNOSTIC_TIMER_OVER <= uiDiagnosticTimer)&&
     (SET == StatusFlag.stStatusFlag.bStartDiagTimer))
  {
    StatusFlag.stStatusFlag.bStartDiagTimer = RESET;
    /*Check for diagnostic tests of all slave devices*/
    if(ALL_SLAVE_DEVICE <= ucDeviceDiagStatus) 
    {
      /*If Diagnostic of all modules pass move to operation mode */
      /*Else wait for user*/ 
      ucDeviceConfigSent = 0;
      ucDeviceDiagStatus = 0;
			
      ClearBit(uiIOExpData,BOARD_STATUS_RED_LED);
      SetBit(uiIOExpData,BOARD_STATUS_GREEN_LED);
      ucSCUMode = SCU_OPERATION_MODE;
    }
    else
    {
      /* wait for user decide afterwards */
      /*Start Diagnostic Timer */
      StatusFlag.stStatusFlag.bStartDiagTimer = SET;
      uiDiagnosticTimer = 0;
    }
		/*change61*/
    /* Take Diagnostic DATA LOG */
		 if(ALL_SLAVE_DEVICE <= ucDeviceDiagRx) 
    {	
			ucDeviceDiagRx=0;
			#ifdef DBG_DEFAULT_MODE
			printf("Take Diagnostic Log\r\n");
			#endif
			fnDatalog_Diag(Diag_Log.rgucDiagLogFlag);
		}
  }  
  
}
/*****************************************************************************
**@Function		:	 	fnOperation_Mode
**@Descriptions: 	This function is implemented for Operation Mode of SCU required for process
**@parameters	:  	None
**@return			:  	None
*****************************************************************************/
void fnOperation_Mode(void)
{
  /* Send Command to HCM to Turn ON Band Heater  */
  /* To avoid multiple command sent Flag is Used */
  if(RESET == StatusFlag.stStatusFlag.bHCMCommandSent)
  {
    StatusFlag.stStatusFlag.bHCMCommandSent = SET ;
    fnSCU_HCM_Command();
  }
  /* Check Pressure & Temperature Of Material */
  fnSCU_Press_Temp_Check();  
  /* In Range or Bypassed SCU allowed to perform processess PRIME,SPRAY & FLUSH*/
  if(SET == StatusFlag.stStatusFlag.bTempAchieved)
  {
    fnSCU_Perform_Process();
  }
}  

/*****************************************************************************
**@Function		:	 	fnSCU_Press_Temp_Check
**@Descriptions: 	This function is implemented for To Check Pressure and 
Temperature are in Range 
**@parameters	:  	None
**@return			:  	None
*****************************************************************************/
void fnSCU_Press_Temp_Check(void)
{
  if(BIT0_SET == (stConfigData.ucConfig_Temp_Press_Achieve_Bypass & BIT0_MASK))
  {	
    StatusFlag.stStatusFlag.bTempAchieveBypass = SET ;
    StatusFlag.stStatusFlag.bTempAchieved = SET ;							
  }
  if(BIT1_SET == (stConfigData.ucConfig_Temp_Press_Achieve_Bypass & BIT1_MASK))
  {	
    StatusFlag.stStatusFlag.bPressAchieveBypass = SET ;		
    StatusFlag.stStatusFlag.bPressAchieved = SET ;							
  }	
  if(RESET == StatusFlag.stStatusFlag.bTempAchieveBypass)
  {
    if((stConfigData.fConfig_AntiFrzTempLTH <= stCurrentData.fCurr_AntifreezeLiquidTemp) &&
       (stConfigData.fConfig_AntiFrzTempHTH > stCurrentData.fCurr_AntifreezeLiquidTemp)  &&
         (stConfigData.fConfig_InlineBaseTempLTH <= stCurrentData.fCurr_InlineBaseMatTemp) &&
           (stConfigData.fConfig_InlineBaseTempHTH > stCurrentData.fCurr_InlineBaseMatTemp)  &&
             (stConfigData.fConfig_InlineHardTempLTH <= stCurrentData.fCurr_InlineHardMatTemp) &&
               (stConfigData.fConfig_InlineHardTempHTH > stCurrentData.fCurr_InlineHardMatTemp))
    {						
      StatusFlag.stStatusFlag.bTempAchieved = SET ;
    }
    else
    {
      StatusFlag.stStatusFlag.bTempAchieved = RESET ;
    }
  }
  
  /*Check Pipe Temperature in Range*/
		/*Check Pressure Achieved*/
  /* Fault Log */
  //Fault_Log.stFault_Log_Flag.bFault_Log_PTF = SET;
  //	StatusFlag.stStatusFlag.bErrorSentFlag = SET;
  //		StatusFlag.stStatusFlag.bWarningFlag = SET;
  //		fnFault_Log();
}  

/*****************************************************************************
**@Function		:	 	fnSCU_Perform_Process
**@Descriptions: 	This function is implemented for To Check Pressure and 
Temperature are in Range 
**@parameters	:  	None
**@return			:  	None
*****************************************************************************/
void fnSCU_Perform_Process(void)
{
  
  /*********************PRIME************************************/
  /* IF NO ERROR AND PRIME SWITCH PRESSED*/
  if((SET == stProcessInput.stProcessFlag.bPrimeSelect)
     && (RESET == StatusFlag.stStatusFlag.bErrorFlag))
  {
    
    /* check if previous operation was Spray or any operation is in progress */
    /* if yes don't send Prime selector Command */
    /* else send Prime selector command */
    if((ucPreviousProcess != SPRAYPROCESS) &&
       (RESET == StatusFlag.stStatusFlag.bInProgress) &&
         (RESET == StatusFlag.stStatusFlag.bPrimeSelector))
    {
#ifdef DBG_PROCESS
      printf("\n\rPRIME SELECT PROCESS \r\n");	
#endif
      /* send prime selector command */
      stTx1Message.DLC = ONE_BYTES;
      stTx1Message.Data[DATA_BYTE_ZERO] = BIT0_SET  ;
      stTx1Message.ExtId = 	MSG_TYPE_COMMAND | CMD_PRIME_SELECTOR_START |
        DEST_OPMM | SOURCE_SCU;
      fnCANMsg_Transmit(&stTx1Message, CAN1);
      /* Indicates Prime Selector Command Sent */
      StatusFlag.stStatusFlag.bPrimeSelector = SET;
      /* Clear Flag for Spray & Flush Selector */
      StatusFlag.stStatusFlag.bSpraySelector = RESET;
      StatusFlag.stStatusFlag.bFlushSelector = RESET;
      /*  Clear Reciever Input For Prime Flag  */
      stProcessInput.stProcessFlag.bPrimeSelect = RESET;
      /*  PRIME IN PROGRESS */
      StatusFlag.stStatusFlag.bInProgress = SET;
      /* Set Flag which says Prime Process Started */
      StatusFlag.stStatusFlag.bPrimeProcessStarted	= RESET;
      ucPRDSpray = DATA_BYTE_ZERO; //change15
    }	
    
  }		
  /*********************SPRAY************************************/
  if((SET == stProcessInput.stProcessFlag.bSpraySelect)
     && 		(RESET == StatusFlag.stStatusFlag.bErrorFlag))
  {
    
    /* check if previous operation was flush or any operation is in progress*/
    /* if yes don't send spray selector Command */
    /* else send Spray selector command */
    //if((ucPreviousProcess != FLUSHPROCESS) && change4
    if((ucPreviousProcess == PRIMEPROCESS) && 
       (RESET == StatusFlag.stStatusFlag.bInProgress))
    {
#ifdef DBG_PROCESS
      printf("\n\rSPRAY SELECT PROCESS AUTOCOAT \r\n");	
#endif
      /* send Spray selector command */
      stTx1Message.DLC = ONE_BYTES;
      StatusFlag.stStatusFlag.bAutoCoatSelector = RESET ;
      stTx1Message.Data[DATA_BYTE_ZERO] = BIT0_SET  ;
      stTx1Message.ExtId = 	MSG_TYPE_COMMAND | CMD_SPRAY_SELECTOR_START |
        DEST_OPMM | SOURCE_SCU;
      fnCANMsg_Transmit(&stTx1Message, CAN1);
      /* Indicates Spray Selector Command Sent */
      StatusFlag.stStatusFlag.bSpraySelector = SET;
      /* Clear Flag for Prime & Flush Selector */
      StatusFlag.stStatusFlag.bPrimeSelector = RESET;
      StatusFlag.stStatusFlag.bFlushSelector = RESET;
      /*  Clear Reciever Input For Spray Flag  */
      stProcessInput.stProcessFlag.bSpraySelect = RESET;
      /*  SPRAY IN PROGRESS */
      StatusFlag.stStatusFlag.bInProgress = SET;
    }	
	
    
  }		
  /*********************FLUSH************************************/
  if(SET == stProcessInput.stProcessFlag.bFlushSelect)
  {
		 /********************change9*****************************/
    if(SET == StatusFlag.stStatusFlag.bSprayProcessStarted)
    {
      StatusFlag.stStatusFlag.bSprayProcessStarted = RESET; //change9
      StatusFlag.stStatusFlag.bStop_Flush_flag = SET; //change11
			StatusFlag.stStatusFlag.bStall_Cmd_Stop_Flush_flag = SET; //change75
      /* Send Command to stop spray process and gun away from pipe  */
      stTx1Message.DLC = ONE_BYTES;
      stTx1Message.Data[DATA_BYTE_ZERO] = BIT4_SET  ;
      stTx1Message.ExtId = 	MSG_TYPE_COMMAND | CMD_FLUSH_SELECTOR_START |
        DEST_OPMM | SOURCE_SCU;
      stTx1Message.IDE = CAN_ID_EXT;
      stTx1Message.RTR = CAN_RTR_DATA;
      fnCANMsg_Transmit(&stTx1Message, CAN1);
      //	send Flushstop command
      /* Indicates Flush Selector Command Sent */
      StatusFlag.stStatusFlag.bFlushSelector = SET;
      /* Clear Flag for Spray & Prime Selector */
      StatusFlag.stStatusFlag.bSpraySelector = RESET;
      StatusFlag.stStatusFlag.bPrimeSelector = RESET;
      /*  Clear Reciever Input For Flush Flag  */
      stProcessInput.stProcessFlag.bFlushSelect = RESET;
      /*  FLUSH IN PROGRESS */
    StatusFlag.stStatusFlag.bInProgress = SET;
			
      
    }	

    /*********************change1********************/
    if((ucPreviousProcess == SPRAYPROCESS) && 
       (SET == StatusFlag.stStatusFlag.bSprayProcessDone) &&
         (RESET == StatusFlag.stStatusFlag.bInProgress))
    {
#ifdef DBG_PROCESS
      printf("\n\rFLUSH SELECT PROCESS AFTER SPRAY \r\n");	
#endif
      StatusFlag.stStatusFlag.bInProgress = RESET ;
      StatusFlag.stStatusFlag.bSprayProcessDone = RESET ;
				/*change62*/
			StatusFlag.stStatusFlag.bStatusLogTake = SET ;
			ucStallTestCount = RESET; /*change72*/
      /* Take Status Log*/
			/*change62*/
   //   fnStatus_Log();
			/*************change7*********************/
			/*change73*/
          if(SET == StatusFlag.stStatusFlag.bStart_Stall_Flag)
          {
            StatusFlag.stStatusFlag.bStart_Stall_Flag = RESET;
            StatusFlag.stStatusFlag.bStall_Timer_Flag = SET ;
          }
      
    }
    /* any operation is in progress */
    /* send Flush selector command */
//    if((RESET == StatusFlag.stStatusFlag.bInProgress)
//       &&(RESET == StatusFlag.stStatusFlag.bFlushSelector))
		if((RESET == StatusFlag.stStatusFlag.bFlushSelector)
			&&(RESET == StatusFlag.stStatusFlag.bPrimeProcessStarted))
//		&&(RESET == StatusFlag.stStatusFlag.bStall_Cmd_Stop_Flush_flag)) //change76
    {
#ifdef DBG_PROCESS
      printf("\n\rFLUSH SELECT PROCESS \r\n");	
#endif
      /* send Flush selector command */
      stTx1Message.DLC = ONE_BYTES;
      stTx1Message.Data[DATA_BYTE_ZERO] = BIT0_SET  ;
      stTx1Message.ExtId = 	MSG_TYPE_COMMAND | CMD_FLUSH_SELECTOR_START |
        DEST_OPMM | SOURCE_SCU;
      fnCANMsg_Transmit(&stTx1Message, CAN1);
      /* Indicates Flush Selector Command Sent */
      StatusFlag.stStatusFlag.bFlushSelector = SET;
      /* Clear Flag for Spray & Prime Selector */
      StatusFlag.stStatusFlag.bSpraySelector = RESET;
      StatusFlag.stStatusFlag.bPrimeSelector = RESET;
      /*  Clear Reciever Input For Flush Flag  */
      stProcessInput.stProcessFlag.bFlushSelect = RESET;
      /*  FLUSH IN PROGRESS */
      StatusFlag.stStatusFlag.bInProgress = SET;
    }	
    /********************change9*****************************/
//    if(SET == StatusFlag.stStatusFlag.bSprayProcessStarted)
//    {
//      StatusFlag.stStatusFlag.bSprayProcessStarted = RESET; //change9
//      StatusFlag.stStatusFlag.bStop_Flush_flag = SET; //change11
//      /* Send Command to stop spray process and gun away from pipe  */
//      stTx1Message.DLC = ONE_BYTES;
//      stTx1Message.Data[DATA_BYTE_ZERO] = BIT4_SET  ;
//      stTx1Message.ExtId = 	MSG_TYPE_COMMAND | CMD_FLUSH_SELECTOR_START |
//        DEST_OPMM | SOURCE_SCU;
//      stTx1Message.IDE = CAN_ID_EXT;
//      stTx1Message.RTR = CAN_RTR_DATA;
//      fnCANMsg_Transmit(&stTx1Message, CAN1);
//      //	send Flushstop command
//      /* Indicates Flush Selector Command Sent */
//      StatusFlag.stStatusFlag.bFlushSelector = SET;
//      /* Clear Flag for Spray & Prime Selector */
//      StatusFlag.stStatusFlag.bSpraySelector = RESET;
//      StatusFlag.stStatusFlag.bPrimeSelector = RESET;
//      /*  Clear Reciever Input For Flush Flag  */
//      stProcessInput.stProcessFlag.bFlushSelect = RESET;
//      /*  FLUSH IN PROGRESS */
//    StatusFlag.stStatusFlag.bInProgress = SET;
//      
//    }	
    /**********************change13 start monitoring of Material************************/	
    /* Send Stop Linear Monitoring to VCM */
    /*stTx1Message.DLC = ONE_BYTES;
    stTx1Message.Data[DATA_BYTE_ZERO] = BIT0_SET  ;
    stTx1Message.ExtId = 	MSG_TYPE_COMMAND | CMD_LINEAR_TRANS_MONITOR |
    DEST_VCM | SOURCE_SCU;
    fnCANMsg_Transmit(&stTx1Message, CAN1);	*/
    /********************************************************************************/
    
    /*  Clear Reciever Input For Flush Flag  */
   // stProcessInput.stProcessFlag.bFlushSelect = RESET;  /*change70*/
  }		
  /*********************STALL JOINT ENDS************************************/	
  if(SET == stProcessInput.stProcessFlag.bFlushComplete)
  {
    /********************change9*****************************/
    if(SET == StatusFlag.stStatusFlag.bSprayProcessStarted)
    {
      StatusFlag.stStatusFlag.bStop_Flush_flag = SET; //change11
				StatusFlag.stStatusFlag.bStall_Cmd_Stop_Flush_flag = SET; //change75
      StatusFlag.stStatusFlag.bSprayProcessStarted = RESET; //change9
      /* Send Command to HCM to Turn ON Band Heater  */
      stTx1Message.DLC = 0;
      stTx1Message.ExtId = 	MSG_TYPE_COMMAND | CMD_STOP_OPMM |
        DEST_OPMM | SOURCE_SCU;
      stTx1Message.IDE = CAN_ID_EXT;
      stTx1Message.RTR = CAN_RTR_DATA;
      fnCANMsg_Transmit(&stTx1Message, CAN1);
      
      
      //	send stop command
    }			
    
    //		/* Check any Operation is in Progress and Previous Operation was Flush */
    		if((ucPreviousProcess == FLUSHPROCESS) && (RESET == StatusFlag.stStatusFlag.bInProgress))
    		{
    //			
    //		/* Send Flush COmplete To OPMM */
   			stTx1Message.DLC = ONE_BYTES;
    			stTx1Message.Data[DATA_BYTE_ZERO] = BIT3_SET  ;
   			stTx1Message.ExtId = 	MSG_TYPE_COMMAND | CMD_FLUSH_SELECTOR_START |
    														DEST_OPMM | SOURCE_SCU;
  			fnCANMsg_Transmit(&stTx1Message, CAN1);
    			
    /* Clear all Flags, Related to PROCESS */
    StatusFlag.stStatusFlag.bPrimeSelector = RESET;
    StatusFlag.stStatusFlag.bSpraySelector = RESET;
    StatusFlag.stStatusFlag.bFlushSelector = RESET;
    StatusFlag.stStatusFlag.bInProgress		=	RESET;
    StatusFlag.stStatusFlag.bStop_Flush_flag = RESET;
    /*  Clear Reciever Input For bFlushComplete Flag  */
    stProcessInput.stProcessFlag.bFlushComplete = RESET;
		
		StatusFlag.stStatusFlag.bStall_In_Progress = RESET; /*change70*/
   }
  }		
  
  /**************START*****************/
  if(SET == stProcessInput.stProcessFlag.bStartProcess)
  {
		#ifdef DBG_PROCESS
      printf("\n\rSTART DETECTED \r\n");	
#endif
    /*********************PRIME************************************/
    
    /* 	check the Position of Gun if sent prime selector 
    gun should be in prime position same for Spray,Flush*/
    if((SET ==  StatusFlag.stStatusFlag.bPrimeSelector) && 
       (SET ==  StatusFlag.stStatusFlag.bPrimePosition) &&
         (SET == StatusFlag.stStatusFlag.bInProgress) &&
           (RESET == StatusFlag.stStatusFlag.bPrimeProcessStarted) &&
             (ucPreviousProcess != SPRAYPROCESS))
    {
      /*  Clear Reciever Input For bStartProcess Flag  */
      stProcessInput.stProcessFlag.bStartProcess = RESET;
#ifdef DBG_PROCESS
      printf("\n\rPRIME STARTED \r\n");	
#endif
      /* 	Send Prime Process start command	*/
      stTx1Message.DLC = ONE_BYTES;
      stTx1Message.Data[DATA_BYTE_ZERO] = BIT1_SET  ;
      stTx1Message.ExtId = 	MSG_TYPE_COMMAND | CMD_PRIME_SELECTOR_START |
        DEST_OPMM | SOURCE_SCU;
      fnCANMsg_Transmit(&stTx1Message, CAN1);
      /**********************change13 start monitoring of Material************************/	
      /* Send Start Linear Monitoring to VCM */
      stTx1Message.DLC = ONE_BYTES;
      stTx1Message.Data[DATA_BYTE_ZERO] = BIT1_SET  ;
      stTx1Message.ExtId = 	MSG_TYPE_COMMAND | CMD_LINEAR_TRANS_MONITOR |
        DEST_VCM | SOURCE_SCU;
      fnCANMsg_Transmit(&stTx1Message, CAN1);	
      /********************************************************************************/
      /* Set Flag which says Prime Process Started */
      StatusFlag.stStatusFlag.bPrimeProcessStarted	= SET;
      /*	SET flag for START Key process excecuted successfully */
      StatusFlag.stStatusFlag.bStartKeyPressed= SET;
      ucPreviousProcess = PRIMEPROCESS;
    }
    /*********************PRIME************************************/
    if((RESET == StatusFlag.stStatusFlag.bPrimeProcessStarted) &&
       (SET == StatusFlag.stStatusFlag.bPrimeSelector) &&
         (SET == StatusFlag.stStatusFlag.bPrimePosition) &&
           //				(RESET == StatusFlag.stStatusFlag.bPrimeFirstStart) &&
           //				(SET == StatusFlag.stStatusFlag.bPrimeTimeAchieve) &&
           (ucPreviousProcess == PRIMEPROCESS))
    {
      /* Set Flag which says Prime Process Started */
      StatusFlag.stStatusFlag.bPrimeProcessStarted= SET;
      /*  Clear Reciever Input For bStartProcess Flag  */
      stProcessInput.stProcessFlag.bStartProcess = RESET;
      /* 	Send Prime Process start command	*/
      stTx1Message.DLC = ONE_BYTES;
      stTx1Message.Data[DATA_BYTE_ZERO] = BIT1_SET  ;
      stTx1Message.ExtId = 	MSG_TYPE_COMMAND | CMD_PRIME_SELECTOR_START |
        DEST_OPMM | SOURCE_SCU;
      fnCANMsg_Transmit(&stTx1Message, CAN1);
      /**********************change13 start monitoring of Material************************/	
      /* Send Start Linear Monitoring to VCM */
      stTx1Message.DLC = ONE_BYTES;
      stTx1Message.Data[DATA_BYTE_ZERO] = BIT1_SET  ;
      stTx1Message.ExtId = 	MSG_TYPE_COMMAND | CMD_LINEAR_TRANS_MONITOR |
        DEST_VCM | SOURCE_SCU;
      fnCANMsg_Transmit(&stTx1Message, CAN1);	
      /********************************************************************************/
      /*	SET flag for START Key process excecuted successfully */
      StatusFlag.stStatusFlag.bStartKeyPressed= SET;
      /* Start Hooter */
#ifdef DBG_PROCESS
      printf("\n\rPRIME PRIME STARTED AGAIN \r\n");	
#endif
    }
    /*********************SPRAY************************************/
    /* 	check the Position of Gun if sent prime selector 
    gun should be in prime position same for Spray,Flush*/
    if((SET == StatusFlag.stStatusFlag.bSpraySelector) && 
       (SET == StatusFlag.stStatusFlag.bSprayPosition) &&
         (SET == StatusFlag.stStatusFlag.bInProgress) &&
           (RESET == StatusFlag.stStatusFlag.bAutoCoatSelector) &&
             (ucPreviousProcess != FLUSHPROCESS))
    {
      /*  Clear Reciever Input For bStartProcess Flag  */
      stProcessInput.stProcessFlag.bStartProcess = RESET;
      /* Send Auto Spray Command For 1st Start */
      StatusFlag.stStatusFlag.bAutoCoatSelector = SET;
      /* 	Send start command	*/
      StatusFlag.stStatusFlag.bInProgress= SET;
      /* 	Send Prime Process start command	*/
      stTx1Message.DLC = ONE_BYTES;
      stTx1Message.Data[DATA_BYTE_ZERO] = BIT1_SET  ;
      stTx1Message.ExtId = 	MSG_TYPE_COMMAND | CMD_SPRAY_SELECTOR_START |
        DEST_OPMM | SOURCE_SCU;
      fnCANMsg_Transmit(&stTx1Message, CAN1);
      
      /**********************change13 start monitoring of Material************************/	
      /* Send Start Linear Monitoring to VCM */
      stTx1Message.DLC = ONE_BYTES;
      stTx1Message.Data[DATA_BYTE_ZERO] = BIT1_SET  ;
      stTx1Message.ExtId = 	MSG_TYPE_COMMAND | CMD_LINEAR_TRANS_MONITOR |
        DEST_VCM | SOURCE_SCU;
      fnCANMsg_Transmit(&stTx1Message, CAN1);	
      /********************************************************************************/
      ucPRDSpray = PRDSPRAY; //change15
#ifdef DBG_PROCESS
      printf("\n\rSPRAY START AUTOCOAT \r\n");	
#endif
      
      /* Set Flag which says Spray Process Started */
      StatusFlag.stStatusFlag.bSprayProcessStarted= SET;
      ucPreviousProcess = SPRAYPROCESS;
    }
    if((SET == StatusFlag.stStatusFlag.bSpraySelector) && 
       (SET == StatusFlag.stStatusFlag.bAutoCoatSelector) &&
         (SET == StatusFlag.stStatusFlag.bSprayProcessDone) &&
           (ucPreviousProcess != FLUSHPROCESS))
    {
      
      if(RESET == StatusFlag.stStatusFlag.bInProgress)
      {
        
        /*SEND SPRAY SELECTOR COMMAND */
        stTx1Message.DLC = ONE_BYTES;
        stTx1Message.Data[DATA_BYTE_ZERO] = BIT0_SET  ;
        stTx1Message.ExtId = 	MSG_TYPE_COMMAND | CMD_SPRAY_SELECTOR_START |
          DEST_OPMM | SOURCE_SCU;
        fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_PROCESS
        printf("\n\rSPRAY SELECT PROCESS SINGLE COAT\r\n");	
#endif
        
        /* 	Send single coat start command	*/
        StatusFlag.stStatusFlag.bInProgress = SET;
      }
      if((SET == StatusFlag.stStatusFlag.bSpraySelector) && 
         (SET == StatusFlag.stStatusFlag.bSprayPosition) &&
           (SET == StatusFlag.stStatusFlag.bInProgress) &&
             (SET == StatusFlag.stStatusFlag.bAutoCoatSelector) &&
               (SET == StatusFlag.stStatusFlag.bSprayProcessDone) &&
                 (ucPreviousProcess != FLUSHPROCESS))
      {
        /*  Clear Reciever Input For bStartProcess Flag  */
        stProcessInput.stProcessFlag.bStartProcess = RESET;
        
        /* 	Send single coat start command	*/
        StatusFlag.stStatusFlag.bInProgress = SET;
        /* 	Send Sprayprocess Done reset	*/
        StatusFlag.stStatusFlag.bSprayProcessDone = RESET;
        /* 	Send Single coatSpray Process start command	*/
        stTx1Message.DLC = ONE_BYTES;
        stTx1Message.Data[DATA_BYTE_ZERO] = BIT2_SET  ;
        stTx1Message.ExtId = 	MSG_TYPE_COMMAND | CMD_SPRAY_SELECTOR_START |
          DEST_OPMM | SOURCE_SCU;
        fnCANMsg_Transmit(&stTx1Message, CAN1);
        ucPRDSpray = PRDSPRAY; //change15		
        
        /**********************change13 start monitoring of Material************************/	
        /* Send Start Linear Monitoring to VCM */
        stTx1Message.DLC = ONE_BYTES;
        stTx1Message.Data[DATA_BYTE_ZERO] = BIT1_SET  ;
        stTx1Message.ExtId = 	MSG_TYPE_COMMAND | CMD_LINEAR_TRANS_MONITOR |
          DEST_VCM | SOURCE_SCU;
        fnCANMsg_Transmit(&stTx1Message, CAN1);	
        /********************************************************************************/
        /* Set Flag which says Spray Process Started */
        StatusFlag.stStatusFlag.bSprayProcessStarted= SET;
#ifdef DBG_PROCESS
        printf("\n\rSPRAY PROCESS SINGLE COAT\r\n");	
#endif
        
        ucPreviousProcess = SPRAYPROCESS;
      }
    }
    /*********************FLUSH************************************/
    /* 	check the Position of Gun if sent prime selector 
    gun should be in prime position same for Spray,Flush*/
    if((SET == StatusFlag.stStatusFlag.bFlushSelector) && 
       (SET == StatusFlag.stStatusFlag.bFlushPosition) &&
         (SET == StatusFlag.stStatusFlag.bInProgress) &&
           (RESET == StatusFlag.stStatusFlag.bFlushProcessStarted))
    {
      /*  Clear Reciever Input For bStartProcess Flag  */
      stProcessInput.stProcessFlag.bStartProcess = RESET;
      /* 	Send start command	*/
      StatusFlag.stStatusFlag.bInProgress= SET;
      /* 	Send Flush Process start command	*/
      stTx1Message.DLC = ONE_BYTES;
      stTx1Message.Data[DATA_BYTE_ZERO] = BIT1_SET  ;
      stTx1Message.ExtId = 	MSG_TYPE_COMMAND | CMD_FLUSH_SELECTOR_START |
        DEST_OPMM | SOURCE_SCU;
      fnCANMsg_Transmit(&stTx1Message, CAN1);
      /* Set Flag which says Prime Process Started */
      StatusFlag.stStatusFlag.bFlushProcessStarted= SET;
      /*	SET flag for START Key process excecuted successfully */
      StatusFlag.stStatusFlag.bStartKeyPressed= SET;
      ucPreviousProcess = FLUSHPROCESS;
#ifdef DBG_PROCESS
      printf("\n\rFLUSH STARTED \r\n");	
#endif
    }
    /*********************FLUSH************************************/
    if((SET == StatusFlag.stStatusFlag.bFlushSelector) &&
       (SET == StatusFlag.stStatusFlag.bFlushPosition) &&
         (RESET == StatusFlag.stStatusFlag.bFlushProcessStarted) &&
           (ucPreviousProcess == FLUSHPROCESS))
      //				(RESET == StatusFlag.stStatusFlag.bFlushFirstStart) &&
      //				(SET == StatusFlag.stStatusFlag.bFlushTimeAchieve))
    {
      /* Set Flag which says Prime Process Started */
      StatusFlag.stStatusFlag.bFlushProcessStarted= SET;
      /*  Clear Reciever Input For bStartProcess Flag  */
      stProcessInput.stProcessFlag.bStartProcess = RESET;
      /*	SET flag for START Key process excecuted successfully */
      StatusFlag.stStatusFlag.bStartKeyPressed= SET;
      stTx1Message.DLC = ONE_BYTES;
      stTx1Message.Data[DATA_BYTE_ZERO] = BIT1_SET  ;
      stTx1Message.ExtId = 	MSG_TYPE_COMMAND | CMD_FLUSH_SELECTOR_START |
        DEST_OPMM | SOURCE_SCU;
      fnCANMsg_Transmit(&stTx1Message, CAN1);
      /* Start Hooter */
#ifdef DBG_PROCESS
      printf("\n\rFLUSH STARTED AGAIN\r\n");	
#endif
    }
    
    /*  Clear Reciever Input For bStartProcess Flag  */
    //		stProcessInput.stProcessFlag.bStartProcess = RESET;
  }	
  /*****SENDING STOP AFTER START RELEASED *****/
  if(RESET == stProcessInput.stProcessFlag.bStartProcess)
  {
    
    /* 	Check if Start key process flag SET	*/
    if(SET == StatusFlag.stStatusFlag.bStartKeyPressed)
    {
      
      if(DEBOUNCINGCNT <= ucReleaseStartCounter )
      {
        
        ucReleaseStartCounter = 0;
        /*********************PRIME************************************/
        /* Send Stop Command to stop the current process */
        /*FOR PRIME check prime process started ,
        Process in progress ,prime position, prime selector, 
        firststart, prime process	*/
        
        if((SET == StatusFlag.stStatusFlag.bPrimeProcessStarted) &&
           (SET == StatusFlag.stStatusFlag.bInProgress) &&
             (SET == StatusFlag.stStatusFlag.bPrimeSelector) &&
               (SET == StatusFlag.stStatusFlag.bPrimePosition) &&
                 (RESET == StatusFlag.stStatusFlag.bPrimeFirstStart) &&
                   (ucPreviousProcess == PRIMEPROCESS))
        {
#ifdef DBG_PROCESS
          printf("\n\rPRIME STOP AGAIN\r\n");	
#endif
          /* RESET START KEY PRESSED	*/
          StatusFlag.stStatusFlag.bStartKeyPressed = RESET ;
          /* 	Send Prime Process Stop command	*/
          stTx1Message.DLC = ONE_BYTES;
          stTx1Message.Data[DATA_BYTE_ZERO] = BIT2_SET  ;
          stTx1Message.ExtId = 	MSG_TYPE_COMMAND | CMD_PRIME_SELECTOR_START |
            DEST_OPMM | SOURCE_SCU;
          fnCANMsg_Transmit(&stTx1Message, CAN1);
          StatusFlag.stStatusFlag.bPrimeFirstStart = SET;
          /**********************change13 start monitoring of Material************************/	
          /* Send Stop Linear Monitoring to VCM */
          /*			stTx1Message.DLC = ONE_BYTES;
          stTx1Message.Data[DATA_BYTE_ZERO] = BIT0_SET  ;
          stTx1Message.ExtId = 	MSG_TYPE_COMMAND | CMD_LINEAR_TRANS_MONITOR |
          DEST_VCM | SOURCE_SCU;
          fnCANMsg_Transmit(&stTx1Message, CAN1);	*/
          /********************************************************************************/
          //	StatusFlag.stStatusFlag.bInProgress = RESET ;
        }	
        /*********************PRIME************************************/
        if((SET == StatusFlag.stStatusFlag.bPrimeProcessStarted) &&
           (SET == StatusFlag.stStatusFlag.bPrimeSelector) &&
             (SET == StatusFlag.stStatusFlag.bPrimePosition) &&
               (SET == StatusFlag.stStatusFlag.bPrimeFirstStart) &&
                 //	(SET == StatusFlag.stStatusFlag.bPrimeTimeAchieve) &&
                 (ucPreviousProcess == PRIMEPROCESS))
        {
          /* RESET START KEY PRESSED	*/
          StatusFlag.stStatusFlag.bStartKeyPressed = RESET ;
          /* Send Prime Process Stop command	*/
          stTx1Message.DLC = ONE_BYTES;
          stTx1Message.Data[DATA_BYTE_ZERO] = BIT2_SET  ;
          stTx1Message.ExtId = 	MSG_TYPE_COMMAND | CMD_PRIME_SELECTOR_START |
            DEST_OPMM | SOURCE_SCU;
          fnCANMsg_Transmit(&stTx1Message, CAN1);
          /**********************change13 start monitoring of Material************************/	
          /* Send Stop Linear Monitoring to VCM */
          /*stTx1Message.DLC = ONE_BYTES;
          stTx1Message.Data[DATA_BYTE_ZERO] = BIT0_SET  ;
          stTx1Message.ExtId = 	MSG_TYPE_COMMAND | CMD_LINEAR_TRANS_MONITOR |
          DEST_VCM | SOURCE_SCU;
          fnCANMsg_Transmit(&stTx1Message, CAN1);	*/
          /********************************************************************************/
          
          
          //			StatusFlag.stStatusFlag.bInProgress = RESET ;
          StatusFlag.stStatusFlag.bPrimeFirstStart = SET ;
          /* stop hooter */
#ifdef DBG_PROCESS
          printf("\n\rPRIME STOP \r\n");	
#endif
        }
        
        /*********************FLUSH************************************/
        /* Send Stop Command to stop the current process */
        /*FOR Flush check Flush process started ,
        Process in progress ,Flush position, Flush selector, 
        firststart, Flush process	*/
        if((SET == StatusFlag.stStatusFlag.bFlushProcessStarted) &&
           (SET == StatusFlag.stStatusFlag.bInProgress) &&
             (SET == StatusFlag.stStatusFlag.bFlushSelector) &&
               (SET == StatusFlag.stStatusFlag.bFlushPosition) &&
                 (RESET == StatusFlag.stStatusFlag.bFlushFirstStart) &&
                   (ucPreviousProcess == FLUSHPROCESS))
        {
          /* RESET START KEY PRESSED	*/
          StatusFlag.stStatusFlag.bStartKeyPressed = RESET ;
          /* 	Send Flush Process Stop command	*/
          stTx1Message.DLC = ONE_BYTES;
          stTx1Message.Data[DATA_BYTE_ZERO] = BIT2_SET  ;
          stTx1Message.ExtId = 	MSG_TYPE_COMMAND | CMD_FLUSH_SELECTOR_START |
            DEST_OPMM | SOURCE_SCU;
          fnCANMsg_Transmit(&stTx1Message, CAN1);
          //						StatusFlag.stStatusFlag.bInProgress = RESET ;
          StatusFlag.stStatusFlag.bFlushFirstStart = SET;
#ifdef DBG_PROCESS
          printf("\n\rFLUSH STOP AGAIN \r\n");	
#endif
        }	
        /*********************FLUSH************************************/
        if((SET == StatusFlag.stStatusFlag.bFlushProcessStarted) &&
           (SET == StatusFlag.stStatusFlag.bFlushSelector) &&
             (SET == StatusFlag.stStatusFlag.bFlushPosition) &&
               (SET == StatusFlag.stStatusFlag.bFlushFirstStart) &&
                 //	(SET == StatusFlag.stStatusFlag.bFlushTimeAchieve) &&
                 (ucPreviousProcess == FLUSHPROCESS))
        {
          /* RESET START KEY PRESSED	*/
          StatusFlag.stStatusFlag.bStartKeyPressed = RESET ;
          /* Send Prime Process Stop command	*/
          stTx1Message.DLC = ONE_BYTES;
          stTx1Message.Data[DATA_BYTE_ZERO] = BIT2_SET  ;
          stTx1Message.ExtId = 	MSG_TYPE_COMMAND | CMD_FLUSH_SELECTOR_START |
            DEST_OPMM | SOURCE_SCU;
          fnCANMsg_Transmit(&stTx1Message, CAN1);
          //StatusFlag.stStatusFlag.bInProgress = RESET ;
          StatusFlag.stStatusFlag.bFlushFirstStart = SET ;
          /* stop hooter */
#ifdef DBG_PROCESS
          printf("\n\rFLUSH STOP \r\n");	
#endif
        }
        
      }	
      /*  check if prime or flush is not first time */
	}
    
    /*	if yes check for time achieved status */
    /* 	no send Stop Process command for PRIME & FLUSH */
    
  }		
  /**************************************change11**********************************************************/
  if(SET == stProcessInput.stProcessFlag.bReadPipeTempComm)
  {
    /* any operation is in progress */
    /* Send Command */
    if(SET == StatusFlag.stStatusFlag.bStop_Flush_flag)
    { 
			StatusFlag.stStatusFlag.bAuto_Flush_flag = SET ;  /*change77*/
      StatusFlag.stStatusFlag.bStop_Flush_flag = RESET;
      stTx1Message.DLC = 0;
      stTx1Message.ExtId = 	MSG_TYPE_COMMAND | COMMAND_RTN_HOME |
        DEST_OPMM | SOURCE_SCU;
      stTx1Message.IDE = CAN_ID_EXT;
      stTx1Message.RTR = CAN_RTR_DATA;
      fnCANMsg_Transmit(&stTx1Message, CAN1);
    }
    /* Send Prime Process Stop command	*/
    else if((RESET == StatusFlag.stStatusFlag.bInProgress)&&
            (RESET == stProcessInput.stProcessFlag.bReadPipeTempRecieveData))
    {
      stTx1Message.DLC = 0x00;
      stTx1Message.ExtId = 	MSG_TYPE_COMMAND | CMD_READ_PIPE_TEMP |
        DEST_OPMM | SOURCE_SCU;
      fnCANMsg_Transmit(&stTx1Message, CAN1);
      stProcessInput.stProcessFlag.bReadPipeTempRecieveData =	SET;
    }
    stProcessInput.stProcessFlag.bReadPipeTempComm =	RESET;
  }		
      /*********************change75********************/
    if((ucPreviousProcess == SPRAYPROCESS) && 
       (SET == StatusFlag.stStatusFlag.bSprayProcessDone) &&
         (RESET == StatusFlag.stStatusFlag.bInProgress) && 
		(SET == StatusFlag.stStatusFlag.bStall_Cmd_Stop_Flush_flag))  /*change76*/
    {
#ifdef DBG_PROCESS
      printf("\n\rFLUSH SELECT PROCESS AFTER SPRAY 2 \r\n");	
#endif
		
      StatusFlag.stStatusFlag.bInProgress = RESET ;
      StatusFlag.stStatusFlag.bSprayProcessDone = RESET ;
			StatusFlag.stStatusFlag.bStall_Cmd_Stop_Flush_flag = RESET;
	//      /*  Clear Reciever Input For Flush Flag  */
     // stProcessInput.stProcessFlag.bFlushSelect = RESET;
				/*change62*/
			//StatusFlag.stStatusFlag.bStatusLogTake = SET ;
			ucStallTestCount = RESET; /*change72*/
      /* Take Status Log*/
			/*change62*/
   //   fnStatus_Log();
			/*************change7*********************/
			/*change73*/
          if(SET == StatusFlag.stStatusFlag.bStart_Stall_Flag)
          {
            StatusFlag.stStatusFlag.bStart_Stall_Flag = RESET;
            StatusFlag.stStatusFlag.bStall_Timer_Flag = SET ;
          }
      
    }
}  

/*****************************************************************************
**@Function		:	 	fnReciever_Scanner
**@Descriptions: 	This function is implemented for To Check Pressure and 
Temperature are in Range 
**@parameters	:  	None
**@return			:  	None
*****************************************************************************/
void fnReciever_Scanner(void)
{
  /* 	Read all Input */
  /*	If any key detected start debouncing 
  & after that check if input there Set the flag accordingly*/
  stRecieverInput.stRecieverFlag.bRecievedPrimeSelector 	= GPIO_ReadInputDataBit(PRIME_SELECT_PORT, PRIME_SELECT_PIN);
  stRecieverInput.stRecieverFlag.bRecievedSpraySelector 	= GPIO_ReadInputDataBit(SPRAY_SELECT_PORT, SPRAY_SELECT_PIN);
  stRecieverInput.stRecieverFlag.bRecievedFlushSelector 	= GPIO_ReadInputDataBit(FLUSH_SELECT_PORT, FLUSH_SELECT_PIN);
  stRecieverInput.stRecieverFlag.bRecievedFlushComplete 	= GPIO_ReadInputDataBit(FLUSH_COMPLETE_SELECT_PORT, FLUSH_COMPLETE_SELECT_PIN);
  stRecieverInput.stRecieverFlag.bRecievedStart				 		= GPIO_ReadInputDataBit(START_SELECT_PORT, START_SELECT_PIN);
  stRecieverInput.stRecieverFlag.bRecievedReadPipeTemp  	= GPIO_ReadInputDataBit(READ_PIPE_TEMP_SELECT_PORT, READ_PIPE_TEMP_SELECT_PIN);
  //stRecieverInput.stRecieverFlag.bRecievedEstop			 			= GPIO_ReadInputDataBit(ESTOP_SIGNAL_SELECT_PORT, ESTOP_SIGNAL_SELECT_PIN);
  
  /* No ESTOP Pressed Logic 1 */
  if((RESET != (stRecieverInput.ucRecieveFlag))&&(RESET == StatusFlag.stStatusFlag.bEStop_Flag))
  {
    
    if(RESET == StatusFlag.stStatusFlag.bDebouncing)
    {
      ucBackUpReciever = stRecieverInput.ucRecieveFlag ;
      StatusFlag.stStatusFlag.bDebouncing = SET ;
      ucDebCounter++;
    }	
    else
    {
      ucDebCounter++;
      if(DEBOUNCINGCNT <= ucDebCounter )
      {  ucDebCounter=0;
      StatusFlag.stStatusFlag.bDebouncing = RESET ;
      if(ucBackUpReciever == stRecieverInput.ucRecieveFlag)
      {
        if(BIT0_SET == (stRecieverInput.ucRecieveFlag & BIT0_MASK))
        {
         
					if((RESET == stProcessInput.stProcessFlag.bPrimeSelect) && 
						(RESET == StatusFlag.stStatusFlag.bStall_In_Progress)) /*change70*/
					{
						 stProcessInput.stProcessFlag.bPrimeSelect = SET ;
            /* Send Status message to display Remote switch detected */
					/*change63*/
					stTx1Message.DLC = ONE_BYTES;
					stTx1Message.Data[DATA_BYTE_ZERO] = BIT0_SET  ;
							stTx1Message.ExtId = 	MSG_TYPE_STATUS | STATUS_REMOTE_SW |
																DEST_HMI | SOURCE_SCU;
					stTx1Message.IDE = CAN_ID_EXT;
					stTx1Message.RTR = CAN_RTR_DATA;
					fnCANMsg_Transmit(&stTx1Message, CAN1);
					/****************************ABK*****************************/
					stTx1Message.ExtId = ((stTx1Message.ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
															 (DEST_TABLET	 | SOURCE_SCU));
					fnXML_FIFO(&stTx1Message);
					}						
#ifdef DBG_RECIEVER
          printf("PRIME SELECTOR\r\n");
#endif
        }
        if(BIT1_SET == (stRecieverInput.ucRecieveFlag & BIT1_MASK))
        {
					if((RESET == stProcessInput.stProcessFlag.bSpraySelect)&& 
						(RESET == StatusFlag.stStatusFlag.bStall_In_Progress)) /*change70*/
					{
          stProcessInput.stProcessFlag.bSpraySelect = SET ;
             /* Send Status message to display Remote switch detected */
					/*change63*/
					stTx1Message.DLC = ONE_BYTES;
					stTx1Message.Data[DATA_BYTE_ZERO] = BIT2_SET  ;
						stTx1Message.ExtId = 	MSG_TYPE_STATUS | STATUS_REMOTE_SW |
																DEST_HMI | SOURCE_SCU;
					stTx1Message.IDE = CAN_ID_EXT;
					stTx1Message.RTR = CAN_RTR_DATA;
					fnCANMsg_Transmit(&stTx1Message, CAN1);
					/****************************ABK*****************************/
					stTx1Message.ExtId = ((stTx1Message.ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
															 (DEST_TABLET	 | SOURCE_SCU));
					fnXML_FIFO(&stTx1Message);	
					}
#ifdef DBG_RECIEVER
          printf("SPRAY SELECTOR\r\n");
#endif
        }
        if(BIT2_SET == (stRecieverInput.ucRecieveFlag & BIT2_MASK))
        {
					if(RESET == stProcessInput.stProcessFlag.bFlushSelect)
					{
          stProcessInput.stProcessFlag.bFlushSelect = SET ;
             /* Send Status message to display Remote switch detected */
					/*change63*/
					stTx1Message.DLC = ONE_BYTES;
					stTx1Message.Data[DATA_BYTE_ZERO] = BIT1_SET  ;
						stTx1Message.ExtId = 	MSG_TYPE_STATUS | STATUS_REMOTE_SW |
																DEST_HMI | SOURCE_SCU;
					stTx1Message.IDE = CAN_ID_EXT;
					stTx1Message.RTR = CAN_RTR_DATA;
					fnCANMsg_Transmit(&stTx1Message, CAN1);
					/****************************ABK*****************************/
					stTx1Message.ExtId = ((stTx1Message.ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
															 (DEST_TABLET	 | SOURCE_SCU));
					fnXML_FIFO(&stTx1Message);	
					}
#ifdef DBG_RECIEVER
          printf("FLUSH SELECTOR\r\n");
#endif
        }
        if(BIT3_SET == (stRecieverInput.ucRecieveFlag & BIT3_MASK))
        {
					if(RESET == stProcessInput.stProcessFlag.bFlushComplete)
					{
          stProcessInput.stProcessFlag.bFlushComplete = SET ;
               /* Send Status message to display Remote switch detected */
					/*change63*/
					stTx1Message.DLC = ONE_BYTES;
					stTx1Message.Data[DATA_BYTE_ZERO] = BIT5_SET  ;
						stTx1Message.ExtId = 	MSG_TYPE_STATUS | STATUS_REMOTE_SW |
																DEST_HMI | SOURCE_SCU;
					stTx1Message.IDE = CAN_ID_EXT;
					stTx1Message.RTR = CAN_RTR_DATA;
					fnCANMsg_Transmit(&stTx1Message, CAN1);
					/****************************ABK*****************************/
					stTx1Message.ExtId = ((stTx1Message.ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
															 (DEST_TABLET	 | SOURCE_SCU));
					fnXML_FIFO(&stTx1Message);	
					}
#ifdef DBG_RECIEVER
          printf("FLUSH COMPLETE\r\n");
#endif
        }
        if(BIT4_SET == (stRecieverInput.ucRecieveFlag & BIT4_MASK))
        {
					if(RESET == stProcessInput.stProcessFlag.bStartProcess)
					{
          stProcessInput.stProcessFlag.bStartProcess = SET ;
               /* Send Status message to display Remote switch detected */
					/*change63*/
					stTx1Message.DLC = ONE_BYTES;
					stTx1Message.Data[DATA_BYTE_ZERO] = BIT4_SET  ;
						stTx1Message.ExtId = 	MSG_TYPE_STATUS | STATUS_REMOTE_SW |
																DEST_HMI | SOURCE_SCU;
					stTx1Message.IDE = CAN_ID_EXT;
					stTx1Message.RTR = CAN_RTR_DATA;
					fnCANMsg_Transmit(&stTx1Message, CAN1);
					/****************************ABK*****************************/
					stTx1Message.ExtId = ((stTx1Message.ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
															 (DEST_TABLET	 | SOURCE_SCU));
					fnXML_FIFO(&stTx1Message);	
					}
#ifdef DBG_RECIEVER
          printf("START\r\n");
#endif
        }
        if(BIT5_SET == (stRecieverInput.ucRecieveFlag & BIT5_MASK))
        {
						if(RESET == stProcessInput.stProcessFlag.bReadPipeTempComm)
					{
          stProcessInput.stProcessFlag.bReadPipeTempComm = SET ;
               /* Send Status message to display Remote switch detected */
					/*change63*/
					stTx1Message.DLC = ONE_BYTES;
					stTx1Message.Data[DATA_BYTE_ZERO] = BIT3_SET  ;
						stTx1Message.ExtId = 	MSG_TYPE_STATUS | STATUS_REMOTE_SW |
																DEST_HMI | SOURCE_SCU;
					stTx1Message.IDE = CAN_ID_EXT;
					stTx1Message.RTR = CAN_RTR_DATA;
					fnCANMsg_Transmit(&stTx1Message, CAN1);
					/****************************ABK*****************************/
					stTx1Message.ExtId = ((stTx1Message.ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
															 (DEST_TABLET	 | SOURCE_SCU));
					fnXML_FIFO(&stTx1Message);	
					}
#ifdef DBG_RECIEVER
          printf("READ PIPE TEMPERATURE\r\n");
#endif
        }
        //						if(BIT6_RESET == (stRecieverInput.ucRecieveFlag & BIT6_MASK))
        //						{
        //							stProcessInput.stProcessFlag.bEstopSignal = SET ;
        //							StatusFlag.stStatusFlag.bEStop_Flag = SET;
        //							uiIOExpData |=	((~ESTOP_GREEN_LED) |(ESTOP_RED_LED));
        //							#ifdef DBG_RECIEVER
        //							printf("ESTOP RECIEVED\r\n");
        //							#endif
        //						}
				
			
      }
      }
    }
    
  }
  else
  {
    
    stRecieverInput.ucRecieveFlag = RESET;
    stProcessInput.ucProcessFlag = RESET;
    ucBackUpReciever = RESET;
  }
  if((SET == StatusFlag.stStatusFlag.bStartKeyPressed)&&
     (BIT4_RESET == (stRecieverInput.ucRecieveFlag & BIT4_MASK)))
  {
    ucReleaseStartCounter++;
  }
}
/*****************************************************************************
**@Function		:	 	fnSCU_HCM_Command
**@Descriptions: 	This function is implemented for To Turn ON Band Heater 
Once SCU moves to Operation mode
**@parameters	:  	None
**@return			:  	None
*****************************************************************************/
void fnSCU_HCM_Command(void)
{
  /* Send Command to HCM to Turn ON Band Heater  */
  stTx1Message.DLC = TWO_BYTES;
  stTx1Message.Data[DATA_BYTE_ZERO] = BIT3_SET  ;
  stTx1Message.Data[DATA_BYTE_ONE] 	= BIT1_SET  ;
  stTx1Message.ExtId = 	MSG_TYPE_COMMAND | CMD_TURN_ON_OFF_HEATER |
    DEST_HCM | SOURCE_SCU;
  
  
  stTx1Message.IDE = CAN_ID_EXT;
  stTx1Message.RTR = CAN_RTR_DATA;
  fnCANMsg_Transmit(&stTx1Message, CAN1);
  
} 
/*****************************************************************************
**@Function		:	 	fnSCU_Operation
**@Descriptions: 	This function is implemented for SCU Tasks
**@parameters	:  	None
**@return			:  	None
*****************************************************************************/
void fnSCU_Operation(void)
{
  
  /* Pressure Regulation */
  fnPressure_Regulation();
  /* Periodic Datalogging */
	if((RESET == stEthernetLog.ucSendFlag)||(RESET == stWifiLog.ucSendFlag))  /*change65*/
			{
  fnPeriodic_Log_Process_Based();
			}
  /* Stall Test Command send change7*/
  fnStalltest_Commandsend();
  /*change27*/
  /*Warning generation for 80% memory full*/
	fnWarnlog_Commandsend();
  /* Process Data recieved Over ETHERNET */
  //fnEthernetProcessData();
  /* Process Data recieved Over Wi-Fi */
  fnWifiProcessData();
  /* Send XML Data */
  fnXML_DataTransmit();
  /* Send RTC Reading every second to HMI */
  fnRTC_Read();	
  /* Error And Warning Action */
  fnERROR_WARN_Action();
  /* Send Logged Data */
  fnSend_Datalog();
			/* Check repeat index change78*/
			fnCheck_Repeat_Index();
	/* Send Error clear message when ESTOP Released */
	  if( SET == StatusFlag.stStatusFlag.bSendEtopClear)
      {
				/*change56*/
				StatusFlag.stStatusFlag.bSendEtopClear = RESET;
        fnAllERROR_Clear(SOURCE_HMI);
				   stTx1Message.DLC = 0;
        stTx1Message.IDE = CAN_ID_EXT;
        stTx1Message.RTR = CAN_RTR_DATA;
        stTx1Message.ExtId = 	MSG_TYPE_COMMAND | CMD_CLEAR_ERROR |
          DEST_HMI | SOURCE_SCU;
        fnCANMsg_Transmit(&stTx1Message, CAN1);
     
			}
} 


/*****************************************************************************
**@Function		:	 	fnCheck_Repeat_Index
**@Descriptions: 	This function is implemented to Check for Change Frequency of 
Periodic log depending on different process(Stall,Spray Idle).
**@parameters	:  	None
**@return			:  	None
*****************************************************************************/
/*change78*/ 
void fnCheck_Repeat_Index(void)
{
	uint8_t ucIndex;
	/*************************JOB NAME*********************************************/
	if((300 <= uiJobnameCnt) && (SET == StatusFlag.stStatusFlag.bConfigJobName))
    { 
      uiJobnameCnt = 0;
			StatusFlag.stStatusFlag.bConfigJobName = RESET;
			/*change78*/						
					//	memset(jobname,'\0',FLASH_RX_TX_BUFFER_SIZE);
          //  strcat((char *)jobname,(char *)stConfigData.rgucConfig_JobName1);
          //  strcat((char *)jobname,(char *)stConfigData.rgucConfig_JobName2);
          //  strcat((char *)jobname,(char *)stConfigData.rgucConfig_JobName3);
						memset(jobnamebac,'\0',FLASH_RX_TX_BUFFER_SIZE);
            strcat((char *)jobnamebac,(char *)stConfigData.rgucConfig_JobName1);
            strcat((char *)jobnamebac,(char *)stConfigData.rgucConfig_JobName2);
            strcat((char *)jobnamebac,(char *)stConfigData.rgucConfig_JobName3);
			     
			       if(RESET == ucJobNameID)
						 {
							 if(strcmp(jobnamebac,jobnamepre))
							 {
									fnStoreJobName(ucJobNameID,&jobnamepre[0]); 
									ucJobNameID++;
								  fnCheck_ID();
									ucJobNameLog = ucJobNameID ;
								  fnSPI_EEPROM_WriteData(CURRENT_INDEX_JOB_NAME, 1, &ucJobNameID); /*change78*/
								 fnSPI_EEPROM_WriteData(LOG_INDEX_JOB_NAME, 1, &ucJobNameLog); /*change78*/
								}
						 }
						 else
						 {
							 if(strcmp(jobnamebac,jobnamepre))
							 {
							 for(ucIndex= 0;ucIndex < ucJobNameID;ucIndex++)
									{
										fnRetriveJobName(ucIndex);	
		//printf("\n\r%s  %s",jobname,pcJobName);
										if(RESET == strcmp(jobname,jobnamebac))
												{
												ucJobNameLog = ucIndex ;
												StatusFlag.stStatusFlag.bChangeJobName = SET;
												break;
												}
									}	
									if(RESET == StatusFlag.stStatusFlag.bChangeJobName)
									{
									fnStoreJobName(ucJobNameID,&jobnamepre[0]); 
									ucJobNameID++;
								  fnCheck_ID();
									ucJobNameLog = ucJobNameID ;
								  
									}
									else
									{
										StatusFlag.stStatusFlag.bChangeJobName = RESET;
										if(strcmp(jobnamebac,jobnamepre))
												{
												for(ucIndex= 0;ucIndex < ucJobNameID;ucIndex++)
													{
													fnRetriveJobName(ucIndex);	
													//printf("\n\r%s  %s",jobname,pcJobName);
													if(RESET == strcmp(jobname,jobnamepre))
														{
														StatusFlag.stStatusFlag.bChangeJobName = SET;
														break;
														}
													}	
														if(RESET == StatusFlag.stStatusFlag.bChangeJobName)
															{
															fnStoreJobName(ucJobNameID,&jobnamepre[0]); 
															ucJobNameID++;
															fnCheck_ID();
															}
														}
									
								}
							 
						 
            /*change26*/ 
            /* Check for Supervisor ID and Job ID Range to reset Flash*/
          //  fnCheck_ID();
			
			StatusFlag.stStatusFlag.bChangeJobName = RESET;
									fnSPI_EEPROM_WriteData(CURRENT_INDEX_JOB_NAME, 1, &ucJobNameID); /*change78*/
									fnSPI_EEPROM_WriteData(LOG_INDEX_JOB_NAME, 1, &ucJobNameLog); /*change78*/
		}
	}
}
		/******************************************************************************/
/*************************JOINT ID*********************************************/
	if((300 <= uiJobIDCnt) && (SET == StatusFlag.stStatusFlag.bConfigJobID))
    { 
      uiJobIDCnt = 0;
			StatusFlag.stStatusFlag.bConfigJobID = RESET;
			/*change78*/						
					//	memset(jobname,'\0',FLASH_RX_TX_BUFFER_SIZE);
          //  strcat((char *)jobname,(char *)stConfigData.rgucConfig_JobName1);
          //  strcat((char *)jobname,(char *)stConfigData.rgucConfig_JobName2);
          //  strcat((char *)jobname,(char *)stConfigData.rgucConfig_JobName3);
						memset(jobidbac,'\0',FLASH_RX_TX_BUFFER_SIZE);
            strcat((char *)jobidbac,(char *)stConfigData.rgucConfig_JobId1);
            strcat((char *)jobidbac,(char *)stConfigData.rgucConfig_JobId2);
            strcat((char *)jobidbac,(char *)stConfigData.rgucConfig_JobId3);
			     
			       if(RESET == ucJobID)
						 {
							 if(strcmp(jobidbac,jobidpre))
							 {
									fnStoreJobID(ucJobID,&jobidpre[0]); 
									ucJobID++;
								  fnCheck_ID();
									ucJobLog = ucJobID ;
								  fnSPI_EEPROM_WriteData(CURRENT_INDEX_JOB_ID, 1, &ucJobID); /*change78*/
								 fnSPI_EEPROM_WriteData(LOG_INDEX_JOB_ID, 1, &ucJobLog); /*change78*/
								}
						 }
						 else
						 {
							 if(strcmp(jobidbac,jobidpre))
							 {
							 for(ucIndex = 0;ucIndex < ucJobID;ucIndex++)
									{
										fnRetriveJobID(ucIndex);	
		//printf("\n\r%s  %s",jobname,pcJobName);
										if(RESET == strcmp(jobid,jobidbac))
												{
												ucJobLog = ucIndex ;
												StatusFlag.stStatusFlag.bChangeJobID = SET;
												break;
												}
									}	
									if(RESET == StatusFlag.stStatusFlag.bChangeJobID)
									{
									fnStoreJobID(ucJobLog,&jobidpre[0]); 
									ucJobID++;
								  fnCheck_ID();
									ucJobLog = ucJobID ;
								  
									}
									else
									{
										StatusFlag.stStatusFlag.bChangeJobID = RESET;
										if(strcmp(jobidbac,jobidpre))
												{
												for(ucIndex= 0;ucIndex < ucJobID;ucIndex++)
													{
													fnRetriveJobID(ucIndex);	
													//printf("\n\r%s  %s",jobname,pcJobName);
													if(RESET == strcmp(jobid,jobidpre))
														{
														StatusFlag.stStatusFlag.bChangeJobID = SET;
														break;
														}
													}	
														if(RESET == StatusFlag.stStatusFlag.bChangeJobID)
															{
															fnStoreJobID(ucJobID,&jobidpre[0]); 
															ucJobID++;
															fnCheck_ID();
															}
														}
									
								}
							 
						 
            /*change26*/ 
            /* Check for Supervisor ID and Job ID Range to reset Flash*/
          //  fnCheck_ID();
			
			StatusFlag.stStatusFlag.bChangeJobID = RESET;
									fnSPI_EEPROM_WriteData(CURRENT_INDEX_JOB_ID, 1, &ucJobID); /*change78*/
								 fnSPI_EEPROM_WriteData(LOG_INDEX_JOB_ID, 1, &ucJobLog); /*change78*/
		}
	}
}
		/******************************************************************************/
/*************************SUPERVISOR NAME*********************************************/
	if((300 <= uiSupervisornameCnt) && (SET == StatusFlag.stStatusFlag.bConfigSupervisorName))
    { 
      uiSupervisornameCnt = 0;
			StatusFlag.stStatusFlag.bConfigSupervisorName = RESET;
			/*change78*/						
					//	memset(jobname,'\0',FLASH_RX_TX_BUFFER_SIZE);
          //  strcat((char *)jobname,(char *)stConfigData.rgucConfig_JobName1);
          //  strcat((char *)jobname,(char *)stConfigData.rgucConfig_JobName2);
          //  strcat((char *)jobname,(char *)stConfigData.rgucConfig_JobName3);
						memset(supervisornamebac,'\0',FLASH_RX_TX_BUFFER_SIZE);
            strcat((char *)supervisornamebac,(char *)stConfigData.rgucConfig_SupervisorName1);
            strcat((char *)supervisornamebac,(char *)stConfigData.rgucConfig_SupervisorName2);
            strcat((char *)supervisornamebac,(char *)stConfigData.rgucConfig_SupervisorName3);
			     
			       if(RESET == ucSupervisorNameID)
						 {
							 if(strcmp(supervisornamebac,supervisornamepre))
							 {
									fnStoreSupervisorName(ucSupervisorNameID,&supervisornamepre[0]); 
									ucSupervisorNameID++;
								  fnCheck_ID();
									ucSupervisorNameLog = ucSupervisorNameID ;
								  fnSPI_EEPROM_WriteData(CURRENT_INDEX_SUPERVISOR_NAME, 1, &ucSupervisorNameID); /*change78*/
								 fnSPI_EEPROM_WriteData(LOG_INDEX_SUPERVISOR_NAME, 1, &ucSupervisorNameLog); /*change78*/
								}
						 }
						 else
						 {
							 if(strcmp(supervisornamebac,supervisornamepre))
							 {
							 for(ucIndex= 0;ucIndex < ucSupervisorNameID;ucIndex++)
									{
										fnRetriveSupervisorName(ucIndex);	
		//printf("\n\r%s  %s",jobname,pcJobName);
										if(RESET == strcmp(supervisorname,supervisornamebac))
												{
												ucSupervisorNameLog = ucIndex ;
												StatusFlag.stStatusFlag.bChangeSupervisorName = SET;
												break;
												}
									}	
									if(RESET == StatusFlag.stStatusFlag.bChangeSupervisorName)
									{
									fnStoreSupervisorName(ucSupervisorNameID,&supervisornamepre[0]); 
									ucSupervisorNameID++;
								  fnCheck_ID();
									ucSupervisorNameLog = ucSupervisorNameID ;
								  
									}
									else
									{
										StatusFlag.stStatusFlag.bChangeSupervisorName = RESET;
										if(strcmp(supervisornamebac,supervisornamepre))
												{
												for(ucIndex= 0;ucIndex < ucSupervisorNameID;ucIndex++)
													{
													fnRetriveSupervisorName(ucIndex);	
													//printf("\n\r%s  %s",jobname,pcJobName);
													if(RESET == strcmp(supervisorname,supervisornamepre))
														{
														StatusFlag.stStatusFlag.bChangeSupervisorName = SET;
														break;
														}
													}	
														if(RESET == StatusFlag.stStatusFlag.bChangeSupervisorName)
															{
															fnStoreSupervisorName(ucSupervisorNameID,&supervisornamepre[0]); 
															ucSupervisorNameID++;
															fnCheck_ID();
															}
														}
									
								}
							 
						 
            /*change26*/ 
            /* Check for Supervisor ID and Job ID Range to reset Flash*/
          //  fnCheck_ID();
			
			StatusFlag.stStatusFlag.bChangeSupervisorName = RESET;
								  fnSPI_EEPROM_WriteData(CURRENT_INDEX_SUPERVISOR_NAME, 1, &ucSupervisorNameID); /*change78*/
								 fnSPI_EEPROM_WriteData(LOG_INDEX_SUPERVISOR_NAME, 1, &ucSupervisorNameLog); /*change78*/
								
		}
	}
}
		/******************************************************************************/

}
/*****************************************************************************
**@Function		:	 	fnWarnlog_Commandsend
**@Descriptions: 	This function is implemented to send warnig command for 3 times 
dependent after configured time
**@parameters	:  	None
**@return			:  	None
*****************************************************************************/
/*change27*/ 
void fnWarnlog_Commandsend(void)
{
  /*DIAGNOSTIC LOG WARNING LOGIC */
  if(SET == stWarnLog.stWarnLogFlag.bDiagWarn)
  {
    if(RESET == stWarnLog.stWarnLogFlag.bDiagWarnTimerStart)
    {
      stWarnLog.stWarnLogFlag.bDiagWarnTimerStart = SET;
//      printf("SEND DIAGNOSTIC WARNING MESSAGE\n\r");
      /**************************change33********************************/
      /******************FORWARD Diagnostic warning MESSAGE TO TABLET & HMI **********************/	
      stTx1Message.ExtId = 	(MSG_TYPE_FAULT | FAULT_LOG_MEMORY_FULL_ERROR |
                             DEST_HMI | SOURCE_SCU);
      
      stTx1Message.DLC = ONE_BYTES; 
      stTx1Message.Data[DATA_BYTE_ZERO] = BIT0_SET;
      fnCANMsg_Transmit(&stTx1Message,CAN1);				
      /****************************ABK*****************************/
      stTx1Message.ExtId = ((stTx1Message.ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                            (DEST_TABLET	 | SOURCE_SCU));
      
      ////						stWifi.ucSendFlag = SET;
      ////						stEthernet.ucSendFlag = SET;
      fnXML_FIFO(&stTx1Message);
      
      /***********************************************************/
      stWarningLog.ucWarnLog_DiagSendWarnCounter++;
      stWarningLog.uiWarnLog_DiagTimerCounter=RESET;
    }
    if((WARNTIMEINTERVAL * 100) <= stWarningLog.uiWarnLog_DiagTimerCounter)
    { 
      stWarningLog.uiWarnLog_DiagTimerCounter = RESET;
//      printf("SEND DIAGNOSTIC WARNING MESSAGE\n\r");
      /**************************change33********************************/
      /******************FORWARD Diagnostic warning MESSAGE TO TABLET & HMI **********************/	
      stTx1Message.ExtId = 	(MSG_TYPE_FAULT | FAULT_LOG_MEMORY_FULL_ERROR |
                             DEST_HMI | SOURCE_SCU);
      
      stTx1Message.DLC = ONE_BYTES; 
      stTx1Message.Data[DATA_BYTE_ZERO] = BIT0_SET;  // Diagnostic warning
      fnCANMsg_Transmit(&stTx1Message,CAN1);				
      /****************************ABK*****************************/
      stTx1Message.ExtId = ((stTx1Message.ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                            (DEST_TABLET	 | SOURCE_SCU));
      
      ////						stWifi.ucSendFlag = SET;
      ////						stEthernet.ucSendFlag = SET;
      fnXML_FIFO(&stTx1Message);
      
      /***********************************************************/
      
      
      stWarningLog.ucWarnLog_DiagSendWarnCounter++;
    }
    if(MAXWARNSEND <= stWarningLog.ucWarnLog_DiagSendWarnCounter)
    {
      stWarnLog.stWarnLogFlag.bDiagWarn = RESET;
      stWarnLog.stWarnLogFlag.bDiagWarnTimerStart = RESET;
      stWarningLog.uiWarnLog_DiagTimerCounter = RESET;
    }
  }
  /*STATUS LOG WARNING LOGIC */
  if(SET == stWarnLog.stWarnLogFlag.bStatusWarn)
  {
    if(RESET == stWarnLog.stWarnLogFlag.bStatusWarnTimerStart)
    {
      stWarnLog.stWarnLogFlag.bStatusWarnTimerStart = SET;
//      printf("SEND STATUS WARNING MESSAGE\n\r");
      /**************************change33********************************/
      /******************FORWARD Status Warning MESSAGE TO TABLET & HMI **********************/	
      stTx1Message.ExtId = 	(MSG_TYPE_FAULT | FAULT_LOG_MEMORY_FULL_ERROR |
                             DEST_HMI | SOURCE_SCU);
      
      stTx1Message.DLC = ONE_BYTES; 
      stTx1Message.Data[DATA_BYTE_ZERO] = BIT2_SET;
      fnCANMsg_Transmit(&stTx1Message,CAN1);				
      /****************************ABK*****************************/
      stTx1Message.ExtId = ((stTx1Message.ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                            (DEST_TABLET	 | SOURCE_SCU));
      
      ////						stWifi.ucSendFlag = SET;
      ////						stEthernet.ucSendFlag = SET;
      fnXML_FIFO(&stTx1Message);
      
      /***********************************************************/
      
      
      stWarningLog.ucWarnLog_StatusSendWarnCounter++;
      stWarningLog.uiWarnLog_StatusTimerCounter=RESET;
    }
    if((WARNTIMEINTERVAL * 100) <= stWarningLog.uiWarnLog_StatusTimerCounter)
    { 
      stWarningLog.uiWarnLog_StatusTimerCounter = RESET;
//      printf("SEND STATUS WARNING MESSAGE\n\r");
      
      /**************************change33********************************/
      /******************FORWARD Status Warning MESSAGE TO TABLET & HMI **********************/	
      stTx1Message.ExtId = 	(MSG_TYPE_FAULT | FAULT_LOG_MEMORY_FULL_ERROR |
                             DEST_HMI | SOURCE_SCU);
      
      stTx1Message.DLC = ONE_BYTES; 
      stTx1Message.Data[DATA_BYTE_ZERO] = BIT2_SET;
      fnCANMsg_Transmit(&stTx1Message,CAN1);				
      /****************************ABK*****************************/
      stTx1Message.ExtId = ((stTx1Message.ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                            (DEST_TABLET	 | SOURCE_SCU));
      
      ////						stWifi.ucSendFlag = SET;
      ////						stEthernet.ucSendFlag = SET;
      fnXML_FIFO(&stTx1Message);
      
      /***********************************************************/
      
      stWarningLog.ucWarnLog_StatusSendWarnCounter++;
    }
    if(MAXWARNSEND <= stWarningLog.ucWarnLog_StatusSendWarnCounter)
    {
      stWarnLog.stWarnLogFlag.bStatusWarn = RESET;
      stWarnLog.stWarnLogFlag.bStatusWarnTimerStart = RESET;
      stWarningLog.uiWarnLog_StatusTimerCounter = RESET;
    }
  }
  
  /*FAULT LOG WARNING LOGIC */
  if(SET == stWarnLog.stWarnLogFlag.bFaultWarn)
  {
    if(RESET == stWarnLog.stWarnLogFlag.bFaultWarnTimerStart)
    {
      stWarnLog.stWarnLogFlag.bFaultWarnTimerStart = SET;
//      printf("SEND FAULT WARNING MESSAGE\n\r");
      
      /**************************change33********************************/
      /******************FORWARD Fault Warning MESSAGE TO TABLET & HMI **********************/	
      stTx1Message.ExtId = 	(MSG_TYPE_FAULT | FAULT_LOG_MEMORY_FULL_ERROR |
                             DEST_HMI | SOURCE_SCU);
      
      stTx1Message.DLC = ONE_BYTES; 
      stTx1Message.Data[DATA_BYTE_ZERO] = BIT1_SET;
      fnCANMsg_Transmit(&stTx1Message,CAN1);				
      /****************************ABK*****************************/
      stTx1Message.ExtId = ((stTx1Message.ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                            (DEST_TABLET	 | SOURCE_SCU));
      
      ////						stWifi.ucSendFlag = SET;
      ////						stEthernet.ucSendFlag = SET;
      fnXML_FIFO(&stTx1Message);
      
      /***********************************************************/
      
      stWarningLog.ucWarnLog_FaultSendWarnCounter++;
      stWarningLog.uiWarnLog_FaultTimerCounter=RESET;
    }
    if((WARNTIMEINTERVAL * 100) <= stWarningLog.uiWarnLog_FaultTimerCounter)
    { 
      stWarningLog.uiWarnLog_FaultTimerCounter = RESET;
//      printf("SEND FAULT WARNING MESSAGE\n\r");
      /**************************change33********************************/
      /******************FORWARD Fault Warning MESSAGE TO TABLET & HMI **********************/	
      stTx1Message.ExtId = 	(MSG_TYPE_FAULT | FAULT_LOG_MEMORY_FULL_ERROR |
                             DEST_HMI | SOURCE_SCU);
      
      stTx1Message.DLC = ONE_BYTES; 
      stTx1Message.Data[DATA_BYTE_ZERO] = BIT1_SET;
      fnCANMsg_Transmit(&stTx1Message,CAN1);				
      /****************************ABK*****************************/
      stTx1Message.ExtId = ((stTx1Message.ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                            (DEST_TABLET	 | SOURCE_SCU));
      
      ////						stWifi.ucSendFlag = SET;
      ////						stEthernet.ucSendFlag = SET;
      fnXML_FIFO(&stTx1Message);
      
      /***********************************************************/
      stWarningLog.ucWarnLog_FaultSendWarnCounter++;
    }
    if(MAXWARNSEND <= stWarningLog.ucWarnLog_FaultSendWarnCounter)
    {
      stWarnLog.stWarnLogFlag.bFaultWarn = RESET;
      stWarnLog.stWarnLogFlag.bFaultWarnTimerStart = RESET;
      stWarningLog.uiWarnLog_FaultTimerCounter = RESET;
    }
  }	
  /*PERIODIC LOG WARNING LOGIC */
  if(SET == stWarnLog.stWarnLogFlag.bPeriodicWarn)
  {
    if(RESET == stWarnLog.stWarnLogFlag.bPeriodicWarnTimerStart)
    {
      stWarnLog.stWarnLogFlag.bPeriodicWarnTimerStart = SET;
//      printf("SEND PERIODIC WARNING MESSAGE\n\r");
      /**************************change33********************************/
      /******************FORWARD Periodic Warning MESSAGE TO TABLET & HMI **********************/	
      stTx1Message.ExtId = 	(MSG_TYPE_FAULT | FAULT_LOG_MEMORY_FULL_ERROR |
                             DEST_HMI | SOURCE_SCU);
      
      stTx1Message.DLC = ONE_BYTES; 
      stTx1Message.Data[DATA_BYTE_ZERO] = BIT3_SET;
      fnCANMsg_Transmit(&stTx1Message,CAN1);				
      /****************************ABK*****************************/
      stTx1Message.ExtId = ((stTx1Message.ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                            (DEST_TABLET	 | SOURCE_SCU));
      
      ////						stWifi.ucSendFlag = SET;
      ////						stEthernet.ucSendFlag = SET;
      fnXML_FIFO(&stTx1Message);
      
      /***********************************************************/
      stWarningLog.ucWarnLog_PeriodicSendWarnCounter++;
      stWarningLog.uiWarnLog_PeriodicTimerCounter=RESET;
    }
    if((WARNTIMEINTERVAL * 100) <= stWarningLog.uiWarnLog_PeriodicTimerCounter)
    { 
      stWarningLog.uiWarnLog_PeriodicTimerCounter = RESET;
//      printf("SEND PERIODIC WARNING MESSAGE\n\r");
      /**************************change33********************************/
      /******************FORWARD Periodic Warning MESSAGE TO TABLET & HMI **********************/	
      stTx1Message.ExtId = 	(MSG_TYPE_FAULT | FAULT_LOG_MEMORY_FULL_ERROR |
                             DEST_HMI | SOURCE_SCU);
      
      stTx1Message.DLC = ONE_BYTES; 
      stTx1Message.Data[DATA_BYTE_ZERO] = BIT3_SET;
      fnCANMsg_Transmit(&stTx1Message,CAN1);				
      /****************************ABK*****************************/
      stTx1Message.ExtId = ((stTx1Message.ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                            (DEST_TABLET	 | SOURCE_SCU));
      
      ////						stWifi.ucSendFlag = SET;
      ////						stEthernet.ucSendFlag = SET;
      fnXML_FIFO(&stTx1Message);
      
      /***********************************************************/
      stWarningLog.ucWarnLog_PeriodicSendWarnCounter++;
    }
    if(MAXWARNSEND <= stWarningLog.ucWarnLog_PeriodicSendWarnCounter)
    {
      stWarnLog.stWarnLogFlag.bPeriodicWarn = RESET;
      stWarnLog.stWarnLogFlag.bPeriodicWarnTimerStart = RESET;
      stWarningLog.uiWarnLog_PeriodicTimerCounter = RESET;
    }
  }		
  
  
}
/*****************************************************************************
**@Function		:	 	fnPeriodic_Log_Process_Based
**@Descriptions: 	This function is implemented to Check for Change Frequency of 
Periodic log depending on different process(Stall,Spray Idle).
**@parameters	:  	None
**@return			:  	None
*****************************************************************************/
/*change15*/ 
void fnPeriodic_Log_Process_Based(void)
{
  /*status to take periodic log change15*/
  
  if(PRDSPRAY == ucPRDSpray)
  {
	if((stConfigData.fConfig_DataLoggingPeriod * 100) <= uiPeriodicDataLogCnt)
    { 
      uiPeriodicDataLogCnt = 0;
			/*change60*/
      rgucPeriodicLog[BYTE_NINE] 	= PRDSPRAY ;	//change15	
      fnPeriodic_Log();
      #ifdef DBG_PRIO_LOG_WRITE
      printf("Periodic Data Log ucPRDSpray spray=%d\r\n ",ucPRDSpray);
     	#endif
    }
  }	
  
  /*status to take periodic log change15*/
  else if(PRDSTALL == ucPRDSpray)
  {
	if((stConfigData.fConfig_DataLoggingPeriod * 100) <= uiPeriodicDataLogCnt)
    { 
      uiPeriodicDataLogCnt = 0;
			/*change60*/
      rgucPeriodicLog[BYTE_NINE] 	= PRDSTALL ;	//change15	
			
      fnPeriodic_Log();
			
      
#ifdef DBG_PRIO_LOG_WRITE
    printf("Periodic Data Log ucPRDSpray stall=%d\r\n ",ucPRDSpray);
#endif
    }
  }	
  /*status to take periodic log change15*/
  
  else
  {
	/*change21*/
	if((stConfigData.fConfig_IdleTimePeriodicLog * 100) <= uiPeriodicDataLogCnt)
    { 
      uiPeriodicDataLogCnt = 0;
			/*change60*/
      rgucPeriodicLog[BYTE_NINE] 	= DATA_BYTE_ZERO ;	//change15	
      fnPeriodic_Log();
   // 		printf("Periodic Data Log ucPRDSpray idle=%d\r\n ",ucPRDSpray);
      
#ifdef DBG_DIAG_WARN_LOGIC
      fnDatalog_Diag(Diag_Log.rgucDiagLogFlag);
#endif
      
#ifdef DBG_STATUS_WARN_LOGIC
      fnStatus_Log();
#endif
#ifdef DBG_FAULT_WARN_LOGIC
      fnFault_Log();
#endif
#ifdef DBG_PERIODIC_WARN_LOGIC
      fnPeriodic_Log();
#endif
#ifdef DBG_PRIO_LOG_WRITE
      //			printf("Periodic Data Log Updated\r\n ");
#endif
    }
  }	
  
}	

/*****************************************************************************
**@Function		:	 	fnStalltest_Commandsend
**@Descriptions: 	This function is implemented to send automatic stall test 
command after configured time
**@parameters	:  	None
**@return			:  	None
*****************************************************************************/
/*change7*/ 
void fnStalltest_Commandsend(void)
{
  if((stConfigData.fConfig_TimeBeforeSendingStallcommand * 100) <= uiStallcommandsendCnt)
  { 
    uiStallcommandsendCnt = 0;
    StatusFlag.stStatusFlag.bStall_Timer_Flag = RESET;
		StatusFlag.stStatusFlag.bStall_In_Progress = SET; /*change70*/
	/*send stall command*/
    /* Send Auto Stall Command to VCM */
    stTx1Message.DLC = ONE_BYTES;
    stTx1Message.Data[DATA_BYTE_ZERO] = BIT0_SET  ;
    stTx1Message.ExtId = 	MSG_TYPE_COMMAND | CMD_STALL_TEST |
      DEST_VCM | SOURCE_SCU;
    fnCANMsg_Transmit(&stTx1Message, CAN1);
    ucPRDSpray = PRDSTALL; //change15
//#ifdef DBG_PRIO_LOG_WRITE
			printf("Stall command sent\r\n ");
//#endif
  }
  
}	


/*****************************************************************************
**@Function		:	 	fnCheck_ID
**@Descriptions: 	This function is implemented to Check for Supervisor ID 
and Job ID Range to reset Flash
**@parameters	:  	None
**@return			:  	None
*****************************************************************************/
/*change26*/ 
void fnCheck_ID(void)
{
  if(FLASH_MAX_INDEX == ucSupervisorNameID)
  {
	ucSupervisorNameID = DEFAULT_CURRENT_INDEX_SUPERVISOR_NAME;
	ucSupervisorNameLog = DEFAULT_CURRENT_INDEX_SUPERVISOR_NAME;
		 fnSPI_EEPROM_WriteData(CURRENT_INDEX_SUPERVISOR_NAME, 1, &ucSupervisorNameID); /*change78*/
								 fnSPI_EEPROM_WriteData(LOG_INDEX_SUPERVISOR_NAME, 1, &ucSupervisorNameLog); /*change78*/
	fnResetName(FLASH_Sector_6);
  }
  else if(FLASH_MAX_INDEX == ucJobNameID)
  {
	ucJobNameID = DEFAULT_CURRENT_INDEX_JOB_NAME;
ucJobNameLog = DEFAULT_CURRENT_INDEX_JOB_NAME; /*change78*/
			fnSPI_EEPROM_WriteData(CURRENT_INDEX_JOB_NAME, 1, &ucJobNameID);
			fnSPI_EEPROM_WriteData(LOG_INDEX_JOB_NAME, 1, &ucJobNameLog); /*change78*/
	fnResetName(FLASH_Sector_7);
  }
  else if(FLASH_MAX_INDEX == ucJobID)
  {
	ucJobID = DEFAULT_CURRENT_INDEX_JOB_ID;
		ucJobLog = DEFAULT_CURRENT_INDEX_JOB_ID; /*change78*/
			fnSPI_EEPROM_WriteData(CURRENT_INDEX_JOB_ID, 1, &ucJobID); /*change78*/
								 fnSPI_EEPROM_WriteData(LOG_INDEX_JOB_ID, 1, &ucJobLog); /*change78*/
	fnResetName(FLASH_Sector_8);
  }
  
}	

/*****************************************************************************
**@Function		:	 	fnERROR_WARN_Action
**@Descriptions: 	This function is implemented for Error & Warning Actions
**@parameters	:  	None
**@return			:  	None
*****************************************************************************/
void fnERROR_WARN_Action(void)
{
  /*Send Error Occurences to OPMM */
  if(SET == StatusFlag.stStatusFlag.bErrorSentFlag)
  {
    
	if(SET == StatusFlag.stStatusFlag.bErrorFlag)
	{
      StatusFlag.stStatusFlag.bInProgress = RESET;
      //	StatusFlag.stStatusFlag.bErrorFlag = RESET;
      stTx1Message.DLC = ONE_BYTES;
      stTx1Message.Data[DATA_BYTE_ZERO] = BIT1_SET  ;
      stTx1Message.ExtId = 	MSG_TYPE_FAULT | FAULT_ERROR_WARN_OCCUR |
        DEST_OPMM | SOURCE_SCU;
      fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_PROCESS
      printf("ERROR OCCUR MESSAGE SENT TO OPMM\r\n");
#endif
      
	}
	if(SET == StatusFlag.stStatusFlag.bWarningFlag)
	{
      //	StatusFlag.stStatusFlag.bWarningFlag = RESET;
      stTx1Message.DLC = ONE_BYTES;
      stTx1Message.Data[DATA_BYTE_ZERO] = BIT0_SET  ;
      stTx1Message.ExtId = 	MSG_TYPE_FAULT | FAULT_ERROR_WARN_OCCUR |
        DEST_OPMM | SOURCE_SCU;
      fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_PROCESS
      printf("WARNING OCCUR MESSAGE SENT TO OPMM\r\n");
#endif
	}
  }
  StatusFlag.stStatusFlag.bErrorSentFlag = RESET;
  
}	

/*****************************************************************************
**@Function		:	 	fnEstop_Check
**@Descriptions: 	This function is implemented for Broadcasting ESTOP OCCURENCE
**@parameters	:  	None
**@return			:  	None
*****************************************************************************/
void fnEstop_Check(void)
{
  /*Send Error Occurences to OPMM */
  if((RESET == StatusFlag.stStatusFlag.bEStop_Sent_Flag) && (SET == StatusFlag.stStatusFlag.bEStop_Flag))
  {
    memset(stTx1Message.Data,'\0', strlen((char *)stTx1Message.Data));
    stTx1Message.IDE 	= CAN_ID_EXT;
    stTx1Message.RTR 	= CAN_RTR_DATA;
    stTx1Message.DLC 	= 0;
    stTx1Message.StdId = 0;
    stTx1Message.ExtId = MSG_TYPE_BROADCAST | RESERVED |
      BCAST_ESTOP | DEST_ALL | SOURCE_SCU;
    
    
    if(SET == StatusFlag.stStatusFlag.bCAN2ActiveFlag )
    {
      /*Transmitt CAN message onto CAN2 bus*/
      CAN_Transmit(CAN2,&stTx1Message);	
    }
    
    
    /*Transmitt CAN message onto CAN1 bus*/
    CAN_Transmit(CAN1,&stTx1Message);	
    
    
#ifdef DBG_RECIEVER
    printf("ESTOP SENT\r\n");
#endif		
    StatusFlag.stStatusFlag.bEStop_Sent_Flag = SET;
  }
  
}	
/*****************************************************************************
**@Function		:	 	fnPressure_Regulation
**@Descriptions: 	This function is implemented for Pressure Regulation 
**@parameters	:  	None
**@return			:  	None
*****************************************************************************/
void fnPressure_Regulation(void)
{
  if(PRESSURE_REG_READ_TIME <= uiPressureRegReadCnt)
  {
#ifdef DBG_PRESSURE_REGULATOR
    printf( "IN PRESSURE REGULATION \n\r" );	
#endif
    uiPressureRegReadCnt = 0;
    
    stCurrentData.fCurr_PressureRegRead = fnADCValue_Read(PRESSURE_REG_ADC_CH);
    
#ifdef DBG_PRESSURE_REGULATOR
    printf( "READ stCurrentData.fCurr_PressureRegRead VALUE : %f \n\r",
           stCurrentData.fCurr_PressureRegRead);	
#endif
    /*converting pressure from Voltage to bar and 0.6 voltage
    corresponds to 0.7 psi and 3.0V corresponds to 130.0 psi*/
    stCurrentData.fCurr_PressureRegRead = (((stCurrentData.fCurr_PressureRegRead - LOW_PRESSURE_V) *
                         PRESSURE_REG_SLOPE_ADC) + LOW_PRESSURE_LEVEL);
#ifdef DBG_PRESSURE_REGULATOR
    printf( "READ PSI VALUE : %f \n\r",
           stCurrentData.fCurr_PressureRegRead);	
    
#endif
    
    /*psi to Bar conversion of pressure regulator value*/
    stCurrentData.fCurr_PressureRegRead = PSI_TO_BAR(stCurrentData.fCurr_PressureRegRead);
    
#ifdef DBG_PRESSURE_REGULATOR
    printf("CONVERTED PSI TO BAR VALUE : %f \n\r",
           stCurrentData.fCurr_PressureRegRead);
#endif
 //   stConfigData.fConfig_RecirPressure = stCurrentData.fCurr_PressureRegRead;
 
    /* Converting Recirculation Pressure BAR to PSI */
		/*change50*/
		if(FLUSHPROCESS == ucPreviousProcess)
	 {
	fPressureRegWrite = BAR_TO_PSI(stConfigData.fConfig_RecirPressure);
	 } 
	 else
	 {
	fPressureRegWrite = BAR_TO_PSI(stConfigData.fConfig_SprayPressure);
	 }
	 
#ifdef DBG_PRESSURE_REGULATOR
    printf("CONVERTED BAR TO PSI Config Recir Value : %f \n\r", fPressureRegWrite);
    
#endif
	
	/* converting PSI pressure to the equivalent Current Value */
	fPressureRegWrite = (((fPressureRegWrite - LOW_PRESSURE_LEVEL) * PRESSURE_REG_SLOPE_DAC) + LOW_CURRENT_VALUE);
	
    
#ifdef DBG_PRESSURE_REGULATOR
    printf("CONVERTED mA value SENDING TO DAC :---------------%f \n\r", fPressureRegWrite);
    
#endif
	
	/* Recirculation Pressure Setting*/
	fnDACValue_Set(fPressureRegWrite);
    
    
  }
}  

/*****************************************************************************
**@Function		:	 	fnRTC_Read
**@Descriptions: 	This function is implemented for Reading RTC 
**@parameters	:  	None
**@return			:  	None
*****************************************************************************/
void fnRTC_Read(void)
{
#ifdef GENERAL_DBG
  char rgcTemp[200];
#endif
  if(RTC_DATA_SEND_TIME <= uiRTCReadCnt)
  {
    uiRTCReadCnt = 0;
    
    RTC_GetTime(RTC_Format_BIN, &stRTC_TimeStructure);
    RTC_GetDate(RTC_Format_BIN, &stRTC_DateStructure);
    
    stConfigData.ucConfig_SystemTime[0] = stRTC_TimeStructure.RTC_Hours ;
    stConfigData.ucConfig_SystemTime[1] = stRTC_TimeStructure.RTC_Minutes;
    stConfigData.ucConfig_SystemTime[2] = stRTC_TimeStructure.RTC_Seconds;
    
    stConfigData.ucConfig_SystemTime[3] = stRTC_DateStructure.RTC_Date;
    stConfigData.ucConfig_SystemTime[4] = stRTC_DateStructure.RTC_Month;
    stConfigData.ucConfig_SystemTime[5] = 20;
    stConfigData.ucConfig_SystemTime[6] = stRTC_DateStructure.RTC_Year;
    
#ifdef GENERAL_DBG_RTC
    printf("\n\r%02d/%02d/20%02d %02d:%02d:%02d",
           stConfigData.ucConfig_SystemTime[3],	stConfigData.ucConfig_SystemTime[4],
           stConfigData.ucConfig_SystemTime[6], stConfigData.ucConfig_SystemTime[0],
           stConfigData.ucConfig_SystemTime[1],	stConfigData.ucConfig_SystemTime[2]);
#endif								
    stTx1Message.DLC = SEVEN_BYTES;
    stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_SYS_DATE_TIME |
      DEST_HMI | SOURCE_SCU;
    for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
    {
      stTx1Message.Data[ucLoopCnt] = 
        stConfigData.ucConfig_SystemTime[ucLoopCnt];
    }
    fnCANMsg_Transmit(&stTx1Message, CAN1);
    stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_SYS_DATE_TIME |
      DEST_TABLET | SOURCE_SCU;
    //		stWifi.ucSendFlag = SET;
    //	stEthernet.ucSendFlag = SET;
    fnXML_FIFO(&stTx1Message);
  }
}  


/*****************************************************************************
**@Function		:	 	fnSCU_Diagnostic
**@Descriptions: 	This function is implemented for SCU Diagnostic of Peripherals
**@parameters	:  	None
**@return			:  	None
*****************************************************************************/
void fnSCU_Diagnostic(void)	
{	
  uint16_t uiNVSRAM1;
  uint16_t uiNVSRAM2;
  
  uint8_t ucTestData = 'T';
  uint8_t ucRdTestData = 0;
  
  memset(stTx1Message.Data,'\0',strlen((char *)stTx1Message.Data));
  /*EEPROM Diagnostic Directly Passed of SCU if Bypassed*/
  if(BIT0_SET == (stConfigData.ucConfig_SCUBypass & BIT0_MASK))
  {
#ifdef DBG_DIAGNOSTIC
    printf("EEPROM Diagnostic Pass\r\n");
#endif
    Diag_Log.stDiag_Log_Flag.bDiag_Log_EEPROM = RESET ;
    
  }
  /*EEPROM Diagnostics Test of SCU if not Bypassed*/
  else
  {
    fnSPI_EEPROM_WriteData(EEPROM_TEST_ADD, TEST_BYTES_COUNT, &ucTestData );
    fnSPI_EEPROM_ReadData(EEPROM_TEST_ADD, TEST_BYTES_COUNT, &ucRdTestData);
    
    if(ucRdTestData == ucTestData)
    {
      Diag_Log.stDiag_Log_Flag.bDiag_Log_EEPROM = RESET ;
#ifdef DBG_DIAGNOSTIC
      printf("EEPROM Diagnostic Pass\r\n");
#endif
    }
    else
    {
      Diag_Log.stDiag_Log_Flag.bDiag_Log_EEPROM = SET ;
      Diag_Log.stDiag_Log_Flag.bDiag_Log_SCU    = SET ;
#ifdef DBG_DIAGNOSTIC
      printf("EEPROM Diagnostic Fail\r\n");
#endif
    }
  }
  
  /*NVSRAM Diagnostic Directly Passed of SCU if Bypassed*/
  if(BIT1_SET == (stConfigData.ucConfig_SCUBypass & BIT1_MASK))
  {
#ifdef DBG_DIAGNOSTIC
    printf("NVSRAM1 and NVSRAM2 Diagnostic Pass\r\n");
#endif
    Diag_Log.stDiag_Log_Flag.bDiag_Log_NVSRAM1 = RESET ; /*change54*/
		Diag_Log.stDiag_Log_Flag.bDiag_Log_NVSRAM2 = RESET ; /*change54*/
  }
  /*NVSRAM Diagnostics Test of SCU if not Bypassed*/
  else
  {
#ifdef DBG_DIAGNOSTIC
    printf("fnNVSRAM_Word_Write\r\n");
#endif
    
    fnNVSRAM1Word_Write( NVSRAM1_START, DUMMY1);
    fnNVSRAM2Word_Write( NVSRAM2_START, DUMMY2);
    
    uiNVSRAM1 = fnNVSRAM1Word_Read(NVSRAM1_START);
    uiNVSRAM2 = fnNVSRAM2Word_Read(NVSRAM2_START);
    
    
    if(DUMMY1 == uiNVSRAM1)
    {
#ifdef DBG_DIAGNOSTIC
      printf("NVSRAM1 Diagnostic Pass\r\n");
#endif
      Diag_Log.stDiag_Log_Flag.bDiag_Log_NVSRAM1 = RESET ;  /*change54*/
    }
    else
    {
#ifdef DBG_DIAGNOSTIC
      printf("NVSRAM1  Diagnostic Fail\r\n");
#endif
      Diag_Log.stDiag_Log_Flag.bDiag_Log_NVSRAM1 = SET ; /*change54*/
      Diag_Log.stDiag_Log_Flag.bDiag_Log_SCU    = SET ;
    }
		if(DUMMY2 == uiNVSRAM2)
    {
#ifdef DBG_DIAGNOSTIC
      printf("NVSRAM2 Diagnostic Pass\r\n");
#endif
      Diag_Log.stDiag_Log_Flag.bDiag_Log_NVSRAM2 = RESET ; /*change54*/
    }
    else
    {
#ifdef DBG_DIAGNOSTIC
      printf(" NVSRAM2 Diagnostic Fail\r\n");
#endif
      Diag_Log.stDiag_Log_Flag.bDiag_Log_NVSRAM2 = SET ; /*change54*/
      Diag_Log.stDiag_Log_Flag.bDiag_Log_SCU    = SET ;
    }
  }
  
  /*Pressure Regulator Diagnostic Directly Passed of SCU if Bypassed*/
  if(BIT2_SET == (stConfigData.ucConfig_SCUBypass & BIT2_MASK))
  {
#ifdef DBG_DIAGNOSTIC
    printf("Pressure Regulator Diagnostic Pass\r\n");
#endif
    ClearBit(uiIOExpData,PRESSUREREGULATOR_RED_LED);
    SetBit(uiIOExpData,PRESSUREREGULATOR_GREEN_LED);
    Diag_Log.stDiag_Log_Flag.bDiag_Log_PR = RESET ;
  }
  /*Pressure Regulator Diagnostics Test of SCU if not Bypassed*/
  else
  {
    //stCurrentData.fCurr_PressureRegRead = fnADCValue_Read(PRESSURE_REG_ADC_CH);
    //		if((PRESSURE_REG_LOWER_READ <= stCurrentData.fCurr_PressureRegRead) && 
    //	  	 (PRESSURE_REG_HIGHER_READ >= stCurrentData.fCurr_PressureRegRead))
    //		{
    //			#ifdef GENERAL_DBG
    //				 printf("Pressure Regulator Diagnostic Pass\r\n");
    //			#endif
    //			Diag_Log.stDiag_Log_Flag.bDiag_Log_PR = RESET ;
    //		}
    //		else
    //		{
    //			#ifdef GENERAL_DBG
    //				 printf("Pressure Regulator Diagnostic Fail\r\n");
    //			#endif
    //			Diag_Log.stDiag_Log_Flag.bDiag_Log_PR = SET ;
    //			StatusFlag.stStatusFlag.bDiagnosticStatus = SET;
    //			stTx1Message.Data[1] |= PRESSURE_REG_DIAGNOSTIC_FAIL; 
    //		}
    
  }
  
  /*This flag is use to do diagnostic one time only*/
  StatusFlag.stStatusFlag.bDiagnosticDone = SET;
  
}	


/*****************************************************************************
**@Function		:	 	fnSCU_Handshake
**@Descriptions: 	This function is implemented for SCU HMI/TAB Handshake
**@parameters	:  	None
**@return			:  	None
*****************************************************************************/
void fnSCU_Handshake(void)	
{	
 

	
  /* Send Diagnostic Status of SCU to HMI & TABLET */
  memset(stTx1Message.Data,'\0',strlen((char *)stTx1Message.Data));
  stTx1Message.DLC = TWO_BYTES;
  stTx1Message.ExtId = STATUS_SCU_DIAGNOSTIC | MSG_TYPE_STATUS |
    DEST_HMI | SOURCE_SCU;	
  if(SET == Diag_Log.stDiag_Log_Flag.bDiag_Log_SCU)
  {
    stTx1Message.Data[0] = DIAGNOSTIC_FAIL;
    if(SET == Diag_Log.stDiag_Log_Flag.bDiag_Log_EEPROM)
    {
      stTx1Message.Data[1] |= BIT0_SET;
    }
		/*change54*/
    if(SET == Diag_Log.stDiag_Log_Flag.bDiag_Log_NVSRAM1)
    {
      stTx1Message.Data[1] |= BIT1_SET;
    }
		/*change54*/
		 if(SET == Diag_Log.stDiag_Log_Flag.bDiag_Log_NVSRAM2)
    {
      stTx1Message.Data[1] |= BIT2_SET;
    }
    if(SET == Diag_Log.stDiag_Log_Flag.bDiag_Log_PR)
    {
      stTx1Message.Data[1] |= BIT3_SET;
    }
  }
  else
  {
#ifdef DBG_HANDSHAKE
	printf("IN DIAGNOSTIC PASS SEND\r\n");
#endif
    stTx1Message.Data[0] = DIAGNOSTIC_PASS;
    stTx1Message.Data[1] = RESET;
  }
#ifdef DBG_HANDSHAKE
  printf("IN SEND DIAGNOSTIC STATUS\r\n");
#endif
  /*Send message to HMI*/
  fnCANMsg_Transmit(&stTx1Message,CAN1);
  /*Send message to XML*/
  stTx1Message.ExtId = 	STATUS_SCU_DIAGNOSTIC | MSG_TYPE_STATUS |
    DEST_TABLET | SOURCE_SCU;	
  //		stWifi.ucSendFlag = SET;
  //		stEthernet.ucSendFlag = SET;		
  fnXML_FIFO(&stTx1Message);
  /*******************SEND FIRMWARE VERSION TO HMI change14**********************/
  memset(stTx1Message.Data,'\0',strlen((char *)stTx1Message.Data));
  stTx1Message.DLC = FIVE_BYTES;
  stTx1Message.ExtId = STATUS_FIRMWARE_VERSION_NUMBER | MSG_TYPE_STATUS |
    DEST_HMI | SOURCE_SCU;	
  stTx1Message.Data[0] = ID_SCU; 
  SplitByte.fData = SCU_FIRMWARE_VERSION;
  /**************************DATA***********************************/
  for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
  {
    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
  }
  /****************************************************************/
  
  /*Send message to HMI*/
  fnCANMsg_Transmit(&stTx1Message,CAN1);
  
  /*Send message to XML*/
  stTx1Message.ExtId = STATUS_FIRMWARE_VERSION_NUMBER | MSG_TYPE_STATUS |
    DEST_TABLET | SOURCE_SCU;	
  
  fnXML_FIFO(&stTx1Message);
  StatusFlag.stStatusFlag.bHandshakeDone = SET;
	
	/*change41*/
/*send utc config after diagnostic*/
                    stTx1Message.DLC = FOUR_BYTES;
                    stTx1Message.Data[DATA_BYTE_ZERO] = stConfigData.ucConfig_TimeOffsetSign ;
                    stTx1Message.Data[DATA_BYTE_ONE] = stConfigData.ucConfig_TimeOffsetHour ;
                    stTx1Message.Data[DATA_BYTE_TWO] = stConfigData.ucConfig_TimeOffsetMin ;
                    stTx1Message.Data[DATA_BYTE_THREE] = stConfigData.ucConfig_TimeOffsetSec ;
                    stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_TIME_ZONE_OFFSET |
                      DEST_HMI | SOURCE_SCU;
                    fnCANMsg_Transmit(&stTx1Message, CAN1);
}	


/*****************************************************************************
**@Function		:		fnSendHMI_Config
**@Descriptions: 	This function is implemented for Send configuration parameters to 
TABLET and HMI
**@parameters	: 	None
**@return			: 	None
*****************************************************************************/
void fnSendHMI_Config(void)
{
  /*Sending first since HMI requirement change22*/
  /*SCU_UNIT_TYPE CONFIG_PARA_84*/
  stTx1Message.DLC = ONE_BYTES;
  stTx1Message.Data[DATA_BYTE_ZERO] = stConfigData.ucConfig_TypeofUnit  ;
  
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_TYPE_OF_UNIT |
    DEST_HMI | SOURCE_SCU;
  fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
  printf("\n\r SCU_UNIT_TYPE CONFIG_PARA_84= %x\r\n", stTx1Message.ExtId);
#endif
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_TYPE_OF_UNIT |
    DEST_TABLET | SOURCE_SCU;	
  //	stWifi.ucSendFlag = SET;
  //	stEthernet.ucSendFlag = SET;
  fnXML_FIFO(&stTx1Message);
  
  /*************************************VCM**********************************/
#ifdef VCM
  /*VCM_UPSTROKE CONFIG_PARA_1*/
  stTx1Message.DLC = THREE_BYTES;
  
  SplitByte.iData = stConfigData.iConfig_UpStrokeTH;
  
  stTx1Message.Data[DATA_BYTE_ZERO] = BIT0_SET ;
  stTx1Message.Data[DATA_BYTE_ONE] = SplitByte.rgucData[DATA_BYTE_ZERO];
  stTx1Message.Data[DATA_BYTE_TWO] = SplitByte.rgucData[DATA_BYTE_ONE];
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_VCM_STALL_TH |
    DEST_HMI | SOURCE_SCU;
  fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
  printf("\n\r VCM_UPSTROKE CONFIG_PARA_1 = %x\r\n", stTx1Message.ExtId);
#endif
  
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_VCM_STALL_TH |
    DEST_TABLET | SOURCE_SCU;	
  //	stWifi.ucSendFlag = SET;
  //	stEthernet.ucSendFlag = SET;
  fnXML_FIFO(&stTx1Message);
  /****************************************************************/	
  
  
  /*VCM_DOWNSTROKE CONFIG_PARA_2*/
  stTx1Message.DLC = THREE_BYTES;
  
  SplitByte.iData = stConfigData.iConfig_DownStrokeTH;
  
  stTx1Message.Data[DATA_BYTE_ZERO] = BIT1_SET ;
  stTx1Message.Data[DATA_BYTE_ONE] = SplitByte.rgucData[DATA_BYTE_ZERO];
  stTx1Message.Data[DATA_BYTE_TWO] = SplitByte.rgucData[DATA_BYTE_ONE];
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_VCM_STALL_TH |
    DEST_HMI | SOURCE_SCU;
  fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
  printf("\n\r VCM_DOWNSTROKE CONFIG_PARA_2 = %x\r\n", stTx1Message.ExtId);
#endif
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_VCM_STALL_TH |
    DEST_TABLET | SOURCE_SCU;	
  //	stWifi.ucSendFlag = SET;
  //	stEthernet.ucSendFlag = SET;
  fnXML_FIFO(&stTx1Message);
  /**********************************************************************/
  
  /*VCM_PRESSURE_LOWTH1 CONFIG_PARA_3*/
  stTx1Message.DLC = FIVE_BYTES;
  
  SplitByte.fData = stConfigData.fConfig_PressureGauge1LTH;
  
  stTx1Message.Data[DATA_BYTE_ZERO] = BIT0_SET ;
  
  /**************************DATA***********************************/
  for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
  {
    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
  }
  /****************************************************************/
  
  
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_VCM_PGAUGE1_TH |
    DEST_HMI | SOURCE_SCU;
  fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
  printf("\n\rVCM_PRESSURE_LOWTH1 CONFIG_PARA_3= %x\r\n", stTx1Message.ExtId);
#endif
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_VCM_PGAUGE1_TH |
    DEST_TABLET | SOURCE_SCU;	
  //	stWifi.ucSendFlag = SET;
  //	stEthernet.ucSendFlag = SET;
  fnXML_FIFO(&stTx1Message);
  /************************************************************/	
  
  /*VCM_PRESSURE_HIGHTH1 CONFIG_PARA_4*/
  stTx1Message.DLC = FIVE_BYTES;
  
  SplitByte.fData = stConfigData.fConfig_PressureGauge1HTH;
  
  stTx1Message.Data[DATA_BYTE_ZERO] = BIT1_SET ;
  
  /**************************DATA***********************************/
  for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
  {
    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
  }
  /****************************************************************/
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_VCM_PGAUGE1_TH |
    DEST_HMI | SOURCE_SCU;
  fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
  printf("\n\rVCM_PRESSURE_HIGHTH1 CONFIG_PARA_4= %x\r\n", stTx1Message.ExtId);
#endif
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_VCM_PGAUGE1_TH |
    DEST_TABLET | SOURCE_SCU;	
  //	stWifi.ucSendFlag = SET;
  //	stEthernet.ucSendFlag = SET;
  fnXML_FIFO(&stTx1Message);
  /****************************************************************/
  
  
  /*VCM_PRESSURE_LOWTH2 CONFIG_PARA_5*/
  stTx1Message.DLC = FIVE_BYTES;
  
  SplitByte.fData = stConfigData.fConfig_PressureGauge2LTH;
  
  stTx1Message.Data[DATA_BYTE_ZERO] = BIT0_SET ;
  
  /**************************DATA***********************************/
  for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
  {
    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
  }
  /****************************************************************/
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_VCM_PGAUGE2_TH |
    DEST_HMI | SOURCE_SCU;
  fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
  printf("\n\rVCM_PRESSURE_LOWTH2 CONFIG_PARA_5= %x\r\n", stTx1Message.ExtId);
#endif
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_VCM_PGAUGE2_TH |
    DEST_TABLET | SOURCE_SCU;	
  //	stWifi.ucSendFlag = SET;
  //	stEthernet.ucSendFlag = SET;
  fnXML_FIFO(&stTx1Message);
  /****************************************************************/
  
  
  /*VCM_PRESSURE_HIGHTH2 CONFIG_PARA_6 */
  stTx1Message.DLC = FIVE_BYTES;
  
  SplitByte.fData = stConfigData.fConfig_PressureGauge2HTH;
  
  stTx1Message.Data[DATA_BYTE_ZERO] = BIT1_SET ;
  
  /**************************DATA***********************************/
  for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
  {
    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
  }
  /****************************************************************/
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_VCM_PGAUGE2_TH |
    DEST_HMI | SOURCE_SCU;
  fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
  printf("\n\rVCM_PRESSURE_HIGHTH2 CONFIG_PARA_6= %x\r\n", stTx1Message.ExtId);
#endif
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_VCM_PGAUGE2_TH |
    DEST_TABLET | SOURCE_SCU;	
  //	stWifi.ucSendFlag = SET;
  //	stEthernet.ucSendFlag = SET;
  fnXML_FIFO(&stTx1Message);
  /****************************************************************/	
  
  
  /*VCM_PRESSURE_LOWTH3 CONFIG_PARA_7*/	
  stTx1Message.DLC = FIVE_BYTES;
  
  SplitByte.fData = stConfigData.fConfig_PressureGauge3LTH;
  
  stTx1Message.Data[DATA_BYTE_ZERO] = BIT0_SET ;
  
  /**************************DATA***********************************/
  for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
  {
    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
  }
  /****************************************************************/
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_VCM_PGAUGE3_TH |
    DEST_HMI | SOURCE_SCU;
  fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
  printf("\n\rVCM_PRESSURE_LOWTH3 CONFIG_PARA_7= %x\r\n", stTx1Message.ExtId);
#endif
  
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_VCM_PGAUGE3_TH |
    DEST_TABLET | SOURCE_SCU;	
  //	stWifi.ucSendFlag = SET;
  //	stEthernet.ucSendFlag = SET;
  fnXML_FIFO(&stTx1Message);
  /****************************************************************/	
  
  
  /*VCM_PRESSURE_HIGHTH3 CONFIG_PARA_8*/
  stTx1Message.DLC = FIVE_BYTES;
  
  SplitByte.fData = stConfigData.fConfig_PressureGauge3HTH;
  
  stTx1Message.Data[DATA_BYTE_ZERO] = BIT1_SET ;
  
  /**************************DATA***********************************/
  for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
  {
    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
  }
  /****************************************************************/
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_VCM_PGAUGE3_TH |
    DEST_HMI | SOURCE_SCU;
  fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
  printf("\n\rVCM_PRESSURE_HIGHTH3 CONFIG_PARA_8= %x\r\n", stTx1Message.ExtId);
#endif
  
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_VCM_PGAUGE3_TH |
    DEST_TABLET | SOURCE_SCU;
  //	stWifi.ucSendFlag = SET;
  //	stEthernet.ucSendFlag = SET;
  fnXML_FIFO(&stTx1Message);
  /****************************************************************/	
  
  
  /*VCM_CYLINDER_AREA1 CONFIG_PARA_9*/
  stTx1Message.DLC = FIVE_BYTES;
  
  SplitByte.fData = stConfigData.fConfig_AreaofCylinderA;
  
  stTx1Message.Data[DATA_BYTE_ZERO] = BIT0_SET ;
  
  /**************************DATA***********************************/
  for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
  {
    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
  }
  /****************************************************************/
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_CYLINDER_SURFACE_AREA |
    DEST_HMI | SOURCE_SCU;
  fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
  printf("\n\rVCM_CYLINDER_AREA1 CONFIG_PARA_9= %x\r\n", stTx1Message.ExtId);
#endif
  
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_CYLINDER_SURFACE_AREA |
    DEST_TABLET | SOURCE_SCU;
  //	stWifi.ucSendFlag = SET;
  //	stEthernet.ucSendFlag = SET;
  fnXML_FIFO(&stTx1Message);
  /****************************************************************/	
  
  
  /*VCM_CYLINDER_AREA2 CONFIG_PARA_10*/
  stTx1Message.DLC = FIVE_BYTES;
  
  SplitByte.fData = stConfigData.fConfig_AreaofCylinderB;
  
  stTx1Message.Data[DATA_BYTE_ZERO] = BIT1_SET ;
  
  /**************************DATA***********************************/
  for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
  {
    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
  }
  /****************************************************************/
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_CYLINDER_SURFACE_AREA |
    DEST_HMI | SOURCE_SCU;
  fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
  printf("\n\rVCM_CYLINDER_AREA2 CONFIG_PARA_10= %x\r\n", stTx1Message.ExtId);
#endif
  
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_CYLINDER_SURFACE_AREA |
    DEST_TABLET | SOURCE_SCU;	
  //	stWifi.ucSendFlag = SET;
  //	stEthernet.ucSendFlag = SET;
  fnXML_FIFO(&stTx1Message);
  /****************************************************************/	
  
  
  /*VCM_CYLINDER_AREA3 CONFIG_PARA_11*/
  stTx1Message.DLC = FIVE_BYTES;
  
  SplitByte.fData = stConfigData.fConfig_AreaofCylinderC;
  
  stTx1Message.Data[DATA_BYTE_ZERO] = BIT2_SET ;
  
  /**************************DATA***********************************/
  for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
  {
    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
  }
  /****************************************************************/
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_CYLINDER_SURFACE_AREA |
    DEST_HMI | SOURCE_SCU;
  fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
  printf("\n\rVCM_CYLINDER_AREA3 CONFIG_PARA_11= %x\r\n", stTx1Message.ExtId);
#endif
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_CYLINDER_SURFACE_AREA |
    DEST_TABLET | SOURCE_SCU;	
  //	stWifi.ucSendFlag = SET;
  //	stEthernet.ucSendFlag = SET;
  fnXML_FIFO(&stTx1Message);
  /****************************************************************/	
  
  
  /*VCM_CYLINDER_COUNT CONFIG_PARA_12*/
  stTx1Message.DLC = ONE_BYTES;
  stTx1Message.Data[DATA_BYTE_ZERO] = stConfigData.ucConfig_CylinderCount;
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_NUMBER_OF_CYLINDER |
    DEST_HMI | SOURCE_SCU;
  fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
  printf("\n\rVCM_CYLINDER_COUNT CONFIG_PARA_12= %x\r\n", stTx1Message.ExtId);
#endif
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_NUMBER_OF_CYLINDER |
    DEST_TABLET | SOURCE_SCU;	
  //	stWifi.ucSendFlag = SET;
  //	stEthernet.ucSendFlag = SET;
  fnXML_FIFO(&stTx1Message);
  /****************************************************************/	
  
  
  /*VCM_PRESSURE_GAUGE1_CONN,
  VCM_PRESSURE_GAUGE2_CONN,
  VCM_PRESSURE_GAUGE3_CONN CONFIG_PARA_13*/
  stTx1Message.DLC = THREE_BYTES;
  stTx1Message.Data[DATA_BYTE_ZERO] = stConfigData.ucConfig_ConnPressureGuage1;
  stTx1Message.Data[DATA_BYTE_ONE]  = stConfigData.ucConfig_ConnPressureGuage2;
  stTx1Message.Data[DATA_BYTE_TWO] 	= stConfigData.ucConfig_ConnPressureGuage3;
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_PGAUGE_REMAP |
    DEST_HMI | SOURCE_SCU;
  fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
  printf("\n\rVCM_PRESSURE_GAUGE1_CONN,VCM_PRESSURE_GAUGE2_CONN,VCM_PRESSURE_GAUGE3_CONN CONFIG_PARA_13= %x\r\n", stTx1Message.ExtId);
#endif
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_PGAUGE_REMAP |
    DEST_TABLET | SOURCE_SCU;	
  //	stWifi.ucSendFlag = SET;
  //	stEthernet.ucSendFlag = SET;
  fnXML_FIFO(&stTx1Message);
  /****************************************************************/	
  
  
  /*VCM_LINEAR_TRANSDUCER_CONN CONFIG_PARA_14*/
  stTx1Message.DLC = ONE_BYTES;
  stTx1Message.Data[DATA_BYTE_ZERO] = stConfigData.ucConfig_ConnLinearTransducer;
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_LT_REMAP |
    DEST_HMI | SOURCE_SCU;
  fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
  printf("\n\rVCM_LINEAR_TRANSDUCER_CONN CONFIG_PARA_14= %x\r\n", stTx1Message.ExtId);
#endif
  
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_LT_REMAP |
    DEST_TABLET | SOURCE_SCU;	
  //	stWifi.ucSendFlag = SET;
  //	stEthernet.ucSendFlag = SET;
  fnXML_FIFO(&stTx1Message);
  /****************************************************************/	
  
  
  /*VCM_EE07_INSIDE_CONN,
  VCM_EE07_OUTSIDE_CONN CONFIG_PARA_15*/
  stTx1Message.DLC = TWO_BYTES;
  stTx1Message.Data[DATA_BYTE_ZERO] = stConfigData.ucConfig_ConnInsideEE07;
  stTx1Message.Data[DATA_BYTE_ONE]  = stConfigData.ucConfig_ConnOutsideEE07;
  
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_EE07_REMAP |
    DEST_HMI | SOURCE_SCU;
  fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
  printf("\n\rVCM_EE07_INSIDE_CONN,	VCM_EE07_OUTSIDE_CONN CONFIG_PARA_15= %x\r\n", stTx1Message.ExtId);
#endif
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_EE07_REMAP |
    DEST_TABLET | SOURCE_SCU;	
  //	stWifi.ucSendFlag = SET;
  //	stEthernet.ucSendFlag = SET;
  fnXML_FIFO(&stTx1Message);
  /****************************************************************/	
  
  
  /*VCM_STALL_VALVE_CONN VCM_RECIRC_VALVE_CONN CONFIG_PARA_16*/
	/*change53*/
  stTx1Message.DLC = TWO_BYTES;
  stTx1Message.Data[DATA_BYTE_ZERO] = stConfigData.ucConfig_ConnStallvalve;
  stTx1Message.Data[DATA_BYTE_ONE] = stConfigData.ucConfig_ConnRecirculationvalve;
  
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_STALL_VALVE_REMAP |
    DEST_HMI | SOURCE_SCU;
  fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
  printf("\n\rVCM_STALL_VALVE_CONN CONFIG_PARA_16= %x\r\n", stTx1Message.ExtId);
#endif
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_STALL_VALVE_REMAP |
    DEST_TABLET | SOURCE_SCU;	
  //	stWifi.ucSendFlag = SET;
  //	stEthernet.ucSendFlag = SET;
  fnXML_FIFO(&stTx1Message);
  /****************************************************************/	
  
  
  /*VCM_BYPASS CONFIG_PARA_17*/
  stTx1Message.DLC = ONE_BYTES;
  stTx1Message.Data[DATA_BYTE_ZERO] = stConfigData.ucConfig_VCMBypass;
  
  
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_VCM_PERIPHRAL_BYPASS |
    DEST_HMI | SOURCE_SCU;
  fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
  printf("\n\rVCM_BYPASS CONFIG_PARA_17= %x\r\n", stTx1Message.ExtId);
#endif
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_VCM_PERIPHRAL_BYPASS |
    DEST_TABLET | SOURCE_SCU;	
  //	stWifi.ucSendFlag = SET;
  //	stEthernet.ucSendFlag = SET;
  fnXML_FIFO(&stTx1Message);
  /****************************************************************/	
  
  
  /*VCM_BEFORE_STALL CONFIG_PARA_18*/
  stTx1Message.DLC = FOUR_BYTES;
  
  SplitByte.fData = stConfigData.fConfig_TimeBeforeStall;
  
  
  /**************************DATA***********************************/
  for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
  {
    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt];
  }
  /****************************************************************/
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_TIME_BEFORE_STALL |
    DEST_HMI | SOURCE_SCU;
  fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
  printf("\n\rVCM_BEFORE_STALL CONFIG_PARA_18= %x\r\n", stTx1Message.ExtId);
#endif
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_TIME_BEFORE_STALL |
    DEST_TABLET | SOURCE_SCU;	
  //	stWifi.ucSendFlag = SET;
  //	stEthernet.ucSendFlag = SET;
  fnXML_FIFO(&stTx1Message);
  /****************************************************************/	
  
  
  /*VCM_COMPARE_STALL CONFIG_PARA_19*/
  stTx1Message.DLC = FOUR_BYTES;
  
  SplitByte.fData = stConfigData.fConfig_TimeCompareStall;
  
  
  /**************************DATA***********************************/
  for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
  {
    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt];
  }
  /****************************************************************/
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_TIME_BEFORE_COMPARE_STALL |
    DEST_HMI | SOURCE_SCU;
  fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
  printf("\n\rVCM_COMPARE_STALL CONFIG_PARA_19= %x\r\n", stTx1Message.ExtId);
#endif
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_TIME_BEFORE_COMPARE_STALL |
    DEST_TABLET | SOURCE_SCU;	
  //	stWifi.ucSendFlag = SET;
  //	stEthernet.ucSendFlag = SET;
  fnXML_FIFO(&stTx1Message);
  /****************************************************************/	
  
#endif 
  
  /*******************************HCM************************************/		
#ifdef HCM
  /*HCM_ANTI_SET_TEMP CONFIG_PARA_20*/
  stTx1Message.DLC = FIVE_BYTES;
  
  SplitByte.fData = stConfigData.fConfig_AntiFrzTempSet;
  
  stTx1Message.Data[DATA_BYTE_ZERO] = BIT0_SET ;
  
  /**************************DATA***********************************/
  for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
  {
    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
  }
  /****************************************************************/
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_BASE_TEMP_SETTING |
    DEST_HMI | SOURCE_SCU;
  fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
  printf("\n\rHCM_ANTI_SET_TEMP CONFIG_PARA_20= %x\r\n", stTx1Message.ExtId);
#endif
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_BASE_TEMP_SETTING |
    DEST_TABLET | SOURCE_SCU;	
  //	stWifi.ucSendFlag = SET;
  //	stEthernet.ucSendFlag = SET;
  fnXML_FIFO(&stTx1Message);
  /****************************************************************/	
  
  
  /*HCM_ANTI_THRESH_TEMP_LOW CONFIG_PARA_21*/
  stTx1Message.DLC = FIVE_BYTES;
  
  SplitByte.fData = stConfigData.fConfig_AntiFrzTempLTH;
  
  stTx1Message.Data[DATA_BYTE_ZERO] = BIT1_SET ;
  
  /**************************DATA***********************************/
  for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
  {
    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
  }
  /****************************************************************/
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_BASE_TEMP_SETTING |
    DEST_HMI | SOURCE_SCU;
  fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
  printf("\n\rHCM_ANTI_THRESH_TEMP_LOW CONFIG_PARA_21= %x\r\n", stTx1Message.ExtId);
#endif
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_BASE_TEMP_SETTING |
    DEST_TABLET | SOURCE_SCU;	
  //	stWifi.ucSendFlag = SET;
  //	stEthernet.ucSendFlag = SET;
  fnXML_FIFO(&stTx1Message);
  /****************************************************************/	
  
  /*HCM_ANTI_THRESH_TEMP_HIGH CONFIG_PARA_22*/
  stTx1Message.DLC = FIVE_BYTES;
  
  SplitByte.fData = stConfigData.fConfig_AntiFrzTempHTH;
  
  stTx1Message.Data[DATA_BYTE_ZERO] = BIT2_SET ;
  
  /**************************DATA***********************************/
  for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
  {
    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
  }
  /****************************************************************/
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_BASE_TEMP_SETTING |
    DEST_HMI | SOURCE_SCU;
  fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
  printf("\n\rHCM_ANTI_THRESH_TEMP_HIGH CONFIG_PARA_22= %x\r\n", stTx1Message.ExtId);
#endif
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_BASE_TEMP_SETTING |
    DEST_TABLET | SOURCE_SCU;	
  //	stWifi.ucSendFlag = SET;
  //	stEthernet.ucSendFlag = SET;
  fnXML_FIFO(&stTx1Message);
  /****************************************************************/	
  
  /*HCM_ANTI_HYST_TEMP_POS CONFIG_PARA_23*/
  stTx1Message.DLC = FIVE_BYTES;
  
  SplitByte.fData = stConfigData.fConfig_HysAntiFrzPos;
  
  stTx1Message.Data[DATA_BYTE_ZERO] = BIT3_SET ;
  
  /**************************DATA***********************************/
  for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
  {
    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
  }
  /****************************************************************/
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_BASE_TEMP_SETTING |
    DEST_HMI | SOURCE_SCU;
  fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
  printf("\n\rHCM_ANTI_HYST_TEMP_POS CONFIG_PARA_23= %x\r\n", stTx1Message.ExtId);
#endif
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_BASE_TEMP_SETTING |
    DEST_TABLET | SOURCE_SCU;	
  //	stWifi.ucSendFlag = SET;
  //	stEthernet.ucSendFlag = SET;
  fnXML_FIFO(&stTx1Message);
  /****************************************************************/	
  
  /*HCM_ANTI_HYST_TEMP_NEG CONFIG_PARA_24*/
  stTx1Message.DLC = FIVE_BYTES;
  
  SplitByte.fData = stConfigData.fConfig_HysAntiFrzNeg;
  
  stTx1Message.Data[DATA_BYTE_ZERO] = BIT4_SET ;
  
  /**************************DATA***********************************/
  for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
  {
    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
  }
  /****************************************************************/
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_BASE_TEMP_SETTING |
    DEST_HMI | SOURCE_SCU;
  fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
  printf("\n\rHCM_ANTI_HYST_TEMP_NEG CONFIG_PARA_24= %x\r\n", stTx1Message.ExtId);
#endif
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_BASE_TEMP_SETTING |
    DEST_TABLET | SOURCE_SCU;	
  //	stWifi.ucSendFlag = SET;
  //	stEthernet.ucSendFlag = SET;
  fnXML_FIFO(&stTx1Message);
  /****************************************************************/	
  
  /*HCM_INLINE_BASE_SET_TEMP CONFIG_PARA_25*/
  stTx1Message.DLC = FIVE_BYTES;
  
  SplitByte.fData = stConfigData.fConfig_InlineBaseTempSet;
  
  stTx1Message.Data[DATA_BYTE_ZERO] = BIT0_SET ;
  
  /**************************DATA***********************************/
  for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
  {
    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
  }
  /****************************************************************/
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_INLINE_BASE_TEMP_SETTING |
    DEST_HMI | SOURCE_SCU;
  fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
  printf("\n\rHCM_INLINE_BASE_SET_TEMP CONFIG_PARA_25= %x\r\n", stTx1Message.ExtId);
#endif
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_INLINE_BASE_TEMP_SETTING |
    DEST_TABLET | SOURCE_SCU;	
  //	stWifi.ucSendFlag = SET;
  //	stEthernet.ucSendFlag = SET;
  fnXML_FIFO(&stTx1Message);
  /****************************************************************/	
  
  /*HCM_INLINE_BASE_THRESH_TEMP_LOW CONFIG_PARA_26*/
  stTx1Message.DLC = FIVE_BYTES;
  
  SplitByte.fData = stConfigData.fConfig_InlineBaseTempLTH;
  
  stTx1Message.Data[DATA_BYTE_ZERO] = BIT1_SET ;
  
  /**************************DATA***********************************/
  for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
  {
    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
  }
  /****************************************************************/
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_INLINE_BASE_TEMP_SETTING |
    DEST_HMI | SOURCE_SCU;
  fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
  printf("\n\rHCM_INLINE_BASE_THRESH_TEMP_LOW CONFIG_PARA_26= %x\r\n", stTx1Message.ExtId);
#endif
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_INLINE_BASE_TEMP_SETTING |
    DEST_TABLET | SOURCE_SCU;	
  //	stWifi.ucSendFlag = SET;
  //	stEthernet.ucSendFlag = SET;
  fnXML_FIFO(&stTx1Message);
  /****************************************************************/	
  
  /*HCM_INLINE_BASE_THRESH_TEMP_HIGH CONFIG_PARA_27*/
  stTx1Message.DLC = FIVE_BYTES;
  
  SplitByte.fData = stConfigData.fConfig_InlineBaseTempHTH;
  
  stTx1Message.Data[DATA_BYTE_ZERO] = BIT2_SET ;
  
  /**************************DATA***********************************/
  for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
  {
    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
  }
  /****************************************************************/
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_INLINE_BASE_TEMP_SETTING |
    DEST_HMI | SOURCE_SCU;
  fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
  printf("\n\rHCM_INLINE_BASE_THRESH_TEMP_HIGH CONFIG_PARA_27= %x\r\n", stTx1Message.ExtId);
#endif
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_INLINE_BASE_TEMP_SETTING |
    DEST_TABLET | SOURCE_SCU;	
  //	stWifi.ucSendFlag = SET;
  //	stEthernet.ucSendFlag = SET;
  fnXML_FIFO(&stTx1Message);
  /****************************************************************/		
  /*HCM_INLINE_BASE_HYST_TEMP_POS CONFIG_PARA_28*/
  stTx1Message.DLC = FIVE_BYTES;
  
  SplitByte.fData = stConfigData.fConfig_HysInlineBasePos;
  
  stTx1Message.Data[DATA_BYTE_ZERO] = BIT3_SET ;
  
  /**************************DATA***********************************/
  for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
  {
    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
  }
  /****************************************************************/
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_INLINE_BASE_TEMP_SETTING |
    DEST_HMI | SOURCE_SCU;
  fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
  printf("\n\rHCM_INLINE_BASE_HYST_TEMP_POS CONFIG_PARA_28= %x\r\n", stTx1Message.ExtId);
#endif
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_INLINE_BASE_TEMP_SETTING |
    DEST_TABLET | SOURCE_SCU;	
  //	stWifi.ucSendFlag = SET;
  //	stEthernet.ucSendFlag = SET;
  fnXML_FIFO(&stTx1Message);
  /****************************************************************/		
  
  
  /*HCM_INLINE_BASE_HYST_TEMP_NEG CONFIG_PARA_29*/
  stTx1Message.DLC = FIVE_BYTES;
  
  SplitByte.fData = stConfigData.fConfig_HysInlineBaseNeg;
  
  stTx1Message.Data[DATA_BYTE_ZERO] = BIT4_SET ;
  
  /**************************DATA***********************************/
  for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
  {
    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
  }
  /****************************************************************/
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_INLINE_BASE_TEMP_SETTING |
    DEST_HMI | SOURCE_SCU;
  fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
  printf("\n\rHCM_INLINE_BASE_HYST_TEMP_NEG CONFIG_PARA_29= %x\r\n", stTx1Message.ExtId);
#endif
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_INLINE_BASE_TEMP_SETTING |
    DEST_TABLET | SOURCE_SCU;	
  //	stWifi.ucSendFlag = SET;
  //	stEthernet.ucSendFlag = SET;
  fnXML_FIFO(&stTx1Message);
  /****************************************************************/		
  
  /*HCM_INLINE_HARD_SET_TEMP CONFIG_PARA_30*/
  stTx1Message.DLC = FIVE_BYTES;
  
  SplitByte.fData = stConfigData.fConfig_InlineHardTempSet;
  
  stTx1Message.Data[DATA_BYTE_ZERO] = BIT0_SET ;
  
  /**************************DATA***********************************/
  for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
  {
    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
  }
  /****************************************************************/
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_INLINE_HARD_TEMP_SETTING |
    DEST_HMI | SOURCE_SCU;
  fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
  printf("\n\rHCM_INLINE_HARD_SET_TEMP CONFIG_PARA_30= %x\r\n", stTx1Message.ExtId);
#endif
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_INLINE_HARD_TEMP_SETTING |
    DEST_TABLET | SOURCE_SCU;	
  //	stWifi.ucSendFlag = SET;
  //	stEthernet.ucSendFlag = SET;
  fnXML_FIFO(&stTx1Message);
  /****************************************************************/		
  
  
  /*HCM_INLINE_HARD_THRESH_TEMP_LOW CONFIG_PARA_31*/
  stTx1Message.DLC = FIVE_BYTES;
  
  SplitByte.fData = stConfigData.fConfig_InlineHardTempLTH;
  
  stTx1Message.Data[DATA_BYTE_ZERO] = BIT1_SET ;
  
  /**************************DATA***********************************/
  for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
  {
    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
  }
  /****************************************************************/
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_INLINE_HARD_TEMP_SETTING |
    DEST_HMI | SOURCE_SCU;
  fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
  printf("\n\rHCM_INLINE_HARD_THRESH_TEMP_LOW CONFIG_PARA_31= %x\r\n", stTx1Message.ExtId);
#endif
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_INLINE_HARD_TEMP_SETTING |
    DEST_TABLET | SOURCE_SCU;	
  //	stWifi.ucSendFlag = SET;
  //	stEthernet.ucSendFlag = SET;
  fnXML_FIFO(&stTx1Message);
  /****************************************************************/		
  
  
  /*HCM_INLINE_HARD_THRESH_TEMP_HIGH CONFIG_PARA_32*/
  stTx1Message.DLC = FIVE_BYTES;
  
  SplitByte.fData = stConfigData.fConfig_InlineHardTempHTH;
  
  stTx1Message.Data[DATA_BYTE_ZERO] = BIT2_SET ;
  
  /**************************DATA***********************************/
  for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
  {
    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
  }
  /****************************************************************/
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_INLINE_HARD_TEMP_SETTING |
    DEST_HMI | SOURCE_SCU;
  fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
  printf("\n\rHCM_INLINE_HARD_THRESH_TEMP_HIGH CONFIG_PARA_32= %x\r\n", stTx1Message.ExtId);
#endif
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_INLINE_HARD_TEMP_SETTING |
    DEST_TABLET | SOURCE_SCU;	
  //	stWifi.ucSendFlag = SET;
  //	stEthernet.ucSendFlag = SET;
  fnXML_FIFO(&stTx1Message);
  /****************************************************************/		
  
  /*HCM_INLINE_HARD_HYST_TEMP_POS CONFIG_PARA_33*/
  stTx1Message.DLC = FIVE_BYTES;
  
  SplitByte.fData = stConfigData.fConfig_InlineHysHardPos;
  
  stTx1Message.Data[DATA_BYTE_ZERO] = BIT3_SET ;
  
  /**************************DATA***********************************/
  for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
  {
    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
  }
  /****************************************************************/
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_INLINE_HARD_TEMP_SETTING |
    DEST_HMI | SOURCE_SCU;
  fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
  printf("\n\rHCM_INLINE_HARD_HYST_TEMP_POS CONFIG_PARA_33= %x\r\n", stTx1Message.ExtId);
#endif
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_INLINE_HARD_TEMP_SETTING |
    DEST_TABLET | SOURCE_SCU;	
  //	stWifi.ucSendFlag = SET;
  //	stEthernet.ucSendFlag = SET;
  fnXML_FIFO(&stTx1Message);
  /****************************************************************/		
  /*HCM_INLINE_HARD_HYST_TEMP_NEG CONFIG_PARA_34*/
  stTx1Message.DLC = FIVE_BYTES;
  
  SplitByte.fData = stConfigData.fConfig_InlineHysHardNeg;
  
  stTx1Message.Data[DATA_BYTE_ZERO] = BIT4_SET ;
  
  /**************************DATA***********************************/
  for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
  {
    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
  }
  /****************************************************************/
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_INLINE_HARD_TEMP_SETTING |
    DEST_HMI | SOURCE_SCU;
  fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
  printf("\n\rHCM_INLINE_HARD_HYST_TEMP_NEG CONFIG_PARA_34= %x\r\n", stTx1Message.ExtId);
#endif
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_INLINE_HARD_TEMP_SETTING |
    DEST_TABLET | SOURCE_SCU;	
  //	stWifi.ucSendFlag = SET;
  //	stEthernet.ucSendFlag = SET;
  fnXML_FIFO(&stTx1Message);
  /****************************************************************/		
  /*HCM_BASE_LIQUID_THRESH_LEVEL_LOW CONFIG_PARA_35*/
  stTx1Message.DLC = FIVE_BYTES;
  
  SplitByte.fData = stConfigData.fConfig_BaseLiquidLevelLTH;
  
  stTx1Message.Data[DATA_BYTE_ZERO] = BIT0_SET ;
  
  /**************************DATA***********************************/
  for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
  {
    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
  }
  /****************************************************************/
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_BASE_LEVEL_SETTING |
    DEST_HMI | SOURCE_SCU;
  fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
  printf("\n\rHCM_BASE_LIQUID_THRESH_LEVEL_LOW CONFIG_PARA_35= %x\r\n", stTx1Message.ExtId);
#endif
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_BASE_LEVEL_SETTING |
    DEST_TABLET | SOURCE_SCU;	
  //	stWifi.ucSendFlag = SET;
  //	stEthernet.ucSendFlag = SET;
  fnXML_FIFO(&stTx1Message);
  /****************************************************************/		
  /*HCM_BASE_LIQUID_THRESH_LEVEL_HIGH CONFIG_PARA_36*/
  stTx1Message.DLC = FIVE_BYTES;
  
  SplitByte.fData = stConfigData.fConfig_BaseLiquidLevelHTH;
  
  stTx1Message.Data[DATA_BYTE_ZERO] = BIT1_SET ;
  
  /**************************DATA***********************************/
  for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
  {
    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
  }
  /****************************************************************/
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_BASE_LEVEL_SETTING |
    DEST_HMI | SOURCE_SCU;
  fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
  printf("\n\rHCM_BASE_LIQUID_THRESH_LEVEL_HIGH CONFIG_PARA_36= %x\r\n", stTx1Message.ExtId);
#endif
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_BASE_LEVEL_SETTING |
    DEST_TABLET | SOURCE_SCU;	
  //	stWifi.ucSendFlag = SET;
  //	stEthernet.ucSendFlag = SET;
  fnXML_FIFO(&stTx1Message);
  /****************************************************************/		
  /*HCM_HARD_LIQUID_THRESH_LEVEL_LOW CONFIG_PARA_37*/
  stTx1Message.DLC = FIVE_BYTES;
  
  SplitByte.fData = stConfigData.fConfig_HardLiquidLevelLTH;
  
  stTx1Message.Data[DATA_BYTE_ZERO] = BIT0_SET ;
  
  /**************************DATA***********************************/
  for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
  {
    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
  }
  /****************************************************************/
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_HARD_LEVEL_SETTING |
    DEST_HMI | SOURCE_SCU;
  fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
  printf("\n\rHCM_HARD_LIQUID_THRESH_LEVEL_LOW CONFIG_PARA_37= %x\r\n", stTx1Message.ExtId);
#endif
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_HARD_LEVEL_SETTING |
    DEST_TABLET | SOURCE_SCU;	
  //	stWifi.ucSendFlag = SET;
  //	stEthernet.ucSendFlag = SET;
  fnXML_FIFO(&stTx1Message);
  /****************************************************************/		
  /*HCM_HARD_LIQUID_THRESH_LEVEL_HIGH CONFIG_PARA_38*/
  stTx1Message.DLC = FIVE_BYTES;
  
  SplitByte.fData = stConfigData.fConfig_HardLiquidLevelHTH;
  
  stTx1Message.Data[DATA_BYTE_ZERO] = BIT1_SET ;
  
  /**************************DATA***********************************/
  for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
  {
    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
  }
  /****************************************************************/
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_HARD_LEVEL_SETTING |
    DEST_HMI | SOURCE_SCU;
  fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
  printf("\n\rHCM_HARD_LIQUID_THRESH_LEVEL_HIGH CONFIG_PARA_38= %x\r\n", stTx1Message.ExtId);
#endif
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_HARD_LEVEL_SETTING |
    DEST_TABLET | SOURCE_SCU;	
  //	stWifi.ucSendFlag = SET;
  //	stEthernet.ucSendFlag = SET;
  fnXML_FIFO(&stTx1Message);
  /****************************************************************/		
  /*HCM_BASE_TANK_HEIGHT CONFIG_PARA_39*/
  stTx1Message.DLC = FIVE_BYTES;
  
  SplitByte.fData = stConfigData.fConfig_BaseTankHeight;
  
  stTx1Message.Data[DATA_BYTE_ZERO] = BIT0_SET ;
  
  /**************************DATA***********************************/
  for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
  {
    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
  }
  /****************************************************************/
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_TANK_HEIGHT |
    DEST_HMI | SOURCE_SCU;
  fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
  printf("\n\rHCM_BASE_TANK_HEIGHT CONFIG_PARA_39= %x\r\n", stTx1Message.ExtId);
#endif
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_TANK_HEIGHT |
    DEST_TABLET | SOURCE_SCU;	
  //	stWifi.ucSendFlag = SET;
  //	stEthernet.ucSendFlag = SET;
  fnXML_FIFO(&stTx1Message);
  /****************************************************************/		
  /*HCM_HARD_TANK_HEIGHT CONFIG_PARA_40*/
  stTx1Message.DLC = FIVE_BYTES;
  
  SplitByte.fData = stConfigData.fConfig_HardTankHeight;
  
  stTx1Message.Data[DATA_BYTE_ZERO] = BIT1_SET ;
  
  /**************************DATA***********************************/
  for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
  {
    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
  }
  /****************************************************************/
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_TANK_HEIGHT |
    DEST_HMI | SOURCE_SCU;
  fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
  printf("\n\rHCM_HARD_TANK_HEIGHT CONFIG_PARA_40= %x\r\n", stTx1Message.ExtId);
#endif
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_TANK_HEIGHT |
    DEST_TABLET | SOURCE_SCU;	
  //	stWifi.ucSendFlag = SET;
  //	stEthernet.ucSendFlag = SET;
  fnXML_FIFO(&stTx1Message);
  /****************************************************************/		
  /*HCM_LIQUID_LEVEL_SAMPLING_RATE CONFIG_PARA_41 change19*/
  stTx1Message.DLC = ONE_BYTES;
  
  SplitByte.uiData = stConfigData.uiConfig_LLSamplingRate;
  
  
  
  /**************************DATA***********************************/
  for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
  {
    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt];
  }
  /****************************************************************/
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_LL_SAMPLE_RATE |
    DEST_HMI | SOURCE_SCU;
  fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
  printf("\n\rHCM_LIQUID_LEVEL_SAMPLING_RATE CONFIG_PARA_41= %x\r\n", stTx1Message.ExtId);
#endif
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_LL_SAMPLE_RATE |
    DEST_TABLET | SOURCE_SCU;	
  //	stWifi.ucSendFlag = SET;
  //	stEthernet.ucSendFlag = SET;
  fnXML_FIFO(&stTx1Message);
  /****************************************************************/		
  /*HCM_INLINE_BASE_THERM_CONN,
  HCM_INLINE_HARD_THERM_CONN,
  HCM_INLINE_ANTIFREEZE_THERM_CONN,
  HCM_RETURN_BASE_THERM_CONN CONFIG_PARA_42*/
  stTx1Message.DLC = FOUR_BYTES;
  
  stTx1Message.Data[DATA_BYTE_ZERO] = stConfigData.ucConfig_ConnBaseinlinether;
  stTx1Message.Data[DATA_BYTE_ONE] = stConfigData.ucConfig_ConnHardinlinether;
  stTx1Message.Data[DATA_BYTE_TWO] = stConfigData.ucConfig_ConnAntifreezether;
  stTx1Message.Data[DATA_BYTE_THREE] = stConfigData.ucConfig_ConnBasereturnlinether;
  
  
  
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_THERMOCOUPLE_REMAP |
    DEST_HMI | SOURCE_SCU;
  fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
  printf("\n\rHCM_THERMOCOUPLE_CONN CONFIG_PARA_42= %x\r\n", stTx1Message.ExtId);
#endif
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_THERMOCOUPLE_REMAP |
    DEST_TABLET | SOURCE_SCU;	
  //	stWifi.ucSendFlag = SET;
  //	stEthernet.ucSendFlag = SET;
  fnXML_FIFO(&stTx1Message);
  /****************************************************************/		
  /*HCM_INLINE_BASE_HEATER_CONN,
  HCM_INLINE_HARD_HEATER_CONN,
  HCM_INLINE_ANTIFREEZE_HEATER_CONN,
  HCM_HARD_HEATER_CONN CONFIG_PARA_43*/
  stTx1Message.DLC = FOUR_BYTES;
  
  stTx1Message.Data[DATA_BYTE_ZERO] 	= stConfigData.ucConfig_ConnBaseinlineheater;
  stTx1Message.Data[DATA_BYTE_ONE] 		= stConfigData.ucConfig_ConnHardinlineheater;
  stTx1Message.Data[DATA_BYTE_TWO] 		= stConfigData.ucConfig_ConnAntifreezeheater;
  stTx1Message.Data[DATA_BYTE_THREE] 	= stConfigData.ucConfig_ConnHardheater;
  
  
  
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_SSR_REMAP |
    DEST_HMI | SOURCE_SCU;
  fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
  printf("\n\rHCM_HEATER_CONN CONFIG_PARA_43= %x\r\n", stTx1Message.ExtId);
#endif
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_SSR_REMAP |
    DEST_TABLET | SOURCE_SCU;	
  //	stWifi.ucSendFlag = SET;
  //	stEthernet.ucSendFlag = SET;
  fnXML_FIFO(&stTx1Message);
  /****************************************************************/		
  /*HCM_BASE_LIQUID_LEVEL_CONN,
  HCM_HARD_LIQUID_LEVEL_CONN CONFIG_PARA_44*/
  stTx1Message.DLC = TWO_BYTES;
  
  stTx1Message.Data[DATA_BYTE_ZERO] 	= stConfigData.ucConfig_ConnBasetankLL;
  stTx1Message.Data[DATA_BYTE_ONE] 		= stConfigData.ucConfig_ConnHardtankLL;
  
  
  
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_LL_SENSOR_REMAP |
    DEST_HMI | SOURCE_SCU;
  fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
  printf("\n\rHCM_LL_CONN CONFIG_PARA_44= %x\r\n", stTx1Message.ExtId);
#endif
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_LL_SENSOR_REMAP |
    DEST_TABLET | SOURCE_SCU;	
  //	stWifi.ucSendFlag = SET;
  //	stEthernet.ucSendFlag = SET;
  fnXML_FIFO(&stTx1Message);
  /****************************************************************/		
  /*HCM_BYPASS CONFIG_PARA_45*/
  stTx1Message.DLC = TWO_BYTES;
  
  SplitByte.iData 	= stConfigData.uiConfig_HCMBypass;
  
  /**************************DATA***********************************/
  for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
  {
    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt];
  }
  /****************************************************************/
  
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_HCM_PERIPHRAL_BYPASS |
    DEST_HMI | SOURCE_SCU;
  fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
  printf("\n\rHCM_BYPASS CONFIG_PARA_45= %x\r\n", stTx1Message.ExtId);
#endif
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_HCM_PERIPHRAL_BYPASS |
    DEST_TABLET | SOURCE_SCU;	
  //	stWifi.ucSendFlag = SET;
  //	stEthernet.ucSendFlag = SET;
  fnXML_FIFO(&stTx1Message);
  /****************************************************************/		
#endif
  /****************************OPMM*********************************/
#ifdef OPMM			
  /*OPMM_SPRAY_COUNT CONFIG_PARA_46*/
  stTx1Message.DLC = ONE_BYTES;
  
  stTx1Message.Data[DATA_BYTE_ZERO]		= stConfigData.ucConfig_SprayCount;
  
  
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_SPRAY_COUNT |
    DEST_HMI | SOURCE_SCU;
  fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
  printf("\n\rOPMM_SPRAY_COUNT CONFIG_PARA_46= %x\r\n", stTx1Message.ExtId);
#endif
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_SPRAY_COUNT |
    DEST_TABLET | SOURCE_SCU;	
  //	stWifi.ucSendFlag = SET;
  //	stEthernet.ucSendFlag = SET;
  fnXML_FIFO(&stTx1Message);
  /****************************************************************/		
  /*OPMM_PRIME_TIME CONFIG_PARA_47*/
  stTx1Message.DLC = FOUR_BYTES;
  
  SplitByte.fData = stConfigData.fConfig_PrimeTime;
  
  
  
  /**************************DATA***********************************/
  for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
  {
    stTx1Message.Data[ucLoopCnt]= SplitByte.rgucData[ucLoopCnt] ;
  }
  /****************************************************************/		
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_PRIME_TIME |
    DEST_HMI | SOURCE_SCU;
  fnCANMsg_Transmit(&stTx1Message, CAN1);
  
#ifdef DBG_CONFIG_SEND_HMI
  printf("\n\rOPMM_PRIME_TIME CONFIG_PARA_47= %x\r\n", stTx1Message.ExtId);
#endif
  
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_PRIME_TIME |
    DEST_TABLET | SOURCE_SCU;	
  //	stWifi.ucSendFlag = SET;
  //	stEthernet.ucSendFlag = SET;
  fnXML_FIFO(&stTx1Message);
  /****************************************************************/		
  /*OPMM_GUN_DELAY_TIME CONFIG_PARA_48*/
  stTx1Message.DLC = FOUR_BYTES;
  
  SplitByte.fData = stConfigData.fConfig_GunDelayTime;
  
  
  
  /**************************DATA***********************************/
  for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
  {
    stTx1Message.Data[ucLoopCnt]= SplitByte.rgucData[ucLoopCnt] ;
  }
  /****************************************************************/		
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_OPMM_GUN_DELAY |
    DEST_HMI | SOURCE_SCU;
  fnCANMsg_Transmit(&stTx1Message, CAN1);
  
#ifdef DBG_CONFIG_SEND_HMI
  printf("\n\rOPMM_GUN_DELAY_TIME CONFIG_PARA_48= %x\r\n", stTx1Message.ExtId);
#endif	
  
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_OPMM_GUN_DELAY |
    DEST_TABLET | SOURCE_SCU;
  //	stWifi.ucSendFlag = SET;
  //	stEthernet.ucSendFlag = SET;
  fnXML_FIFO(&stTx1Message);
  /****************************************************************/		
  /*OPMM_SPRAY_OVERLAPTIME CONFIG_PARA_49*/
  stTx1Message.DLC = FOUR_BYTES;
  
  SplitByte.fData = stConfigData.fConfig_SprayOverlapTime;
  
  
  
  /**************************DATA***********************************/
  for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
  {
    stTx1Message.Data[ucLoopCnt]= SplitByte.rgucData[ucLoopCnt] ;
  }
  /****************************************************************/		
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_OPMM_SPRAY_OVERLAP_TIME |
    DEST_HMI | SOURCE_SCU;
  fnCANMsg_Transmit(&stTx1Message, CAN1);
  
#ifdef DBG_CONFIG_SEND_HMI
  printf("\n\rOPMM_SPRAY_OVERLAPTIME CONFIG_PARA_49= %x\r\n", stTx1Message.ExtId);
#endif						
  
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_OPMM_SPRAY_OVERLAP_TIME |
    DEST_TABLET | SOURCE_SCU;	
  //	stWifi.ucSendFlag = SET;
  //	stEthernet.ucSendFlag = SET;
  fnXML_FIFO(&stTx1Message);
  /****************************************************************/		
  /*OPMM_REVERSE_DELAYTIME CONFIG_PARA_50*/
  stTx1Message.DLC = FOUR_BYTES;
  
  SplitByte.fData = stConfigData.fConfig_ReverseDelayTime;
  
  
  
  /**************************DATA***********************************/
  for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
  {
    stTx1Message.Data[ucLoopCnt]= SplitByte.rgucData[ucLoopCnt] ;
  }
  /****************************************************************/		
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_OPMM_REVERSE_DELAY |
    DEST_HMI | SOURCE_SCU;
  fnCANMsg_Transmit(&stTx1Message, CAN1);
  
#ifdef DBG_CONFIG_SEND_HMI
  printf("\n\rOPMM_REVERSE_DELAYTIME CONFIG_PARA_50= %x\r\n", stTx1Message.ExtId);
#endif						
  
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_OPMM_REVERSE_DELAY |
    DEST_TABLET | SOURCE_SCU;	
  //	stWifi.ucSendFlag = SET;
  //	stEthernet.ucSendFlag = SET;
  fnXML_FIFO(&stTx1Message);
  /****************************************************************/		
  /*OPMM_FLUSH_TIME CONFIG_PARA_51*/
  stTx1Message.DLC = FOUR_BYTES;
  
  SplitByte.fData = stConfigData.fConfig_FlushTime;
  
  
  
  /**************************DATA***********************************/
  for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
  {
    stTx1Message.Data[ucLoopCnt]= SplitByte.rgucData[ucLoopCnt] ;
  }
  /****************************************************************/		
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_OPMM_FLUSH_TIME |
    DEST_HMI | SOURCE_SCU;
  fnCANMsg_Transmit(&stTx1Message, CAN1);
  
#ifdef DBG_CONFIG_SEND_HMI
  printf("\n\rOPMM_FLUSH_TIME CONFIG_PARA_51= %x\r\n", stTx1Message.ExtId);
#endif	
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_OPMM_FLUSH_TIME |
    DEST_TABLET | SOURCE_SCU;	
  //	stWifi.ucSendFlag = SET;
  //	stEthernet.ucSendFlag = SET;
  fnXML_FIFO(&stTx1Message);
  /****************************************************************/		
  /*OPMM_DCLS_HOME_REACH_TIME CONFIG_PARA_52*/
  stTx1Message.DLC = FIVE_BYTES;
  
  SplitByte.fData = stConfigData.fConfig_DCLSTime;
  
  stTx1Message.Data[DATA_BYTE_ZERO] = BIT0_SET ;
  
  /**************************DATA***********************************/
  for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
  {
    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
  }
  /****************************************************************/
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_DCLS_HOME_TIME |
    DEST_HMI | SOURCE_SCU;
  fnCANMsg_Transmit(&stTx1Message, CAN1);
  
#ifdef DBG_CONFIG_SEND_HMI
  printf("\n\rOPMM_DCLS_HOME_REACH_TIME CONFIG_PARA_52= %x\r\n", stTx1Message.ExtId);
#endif			
  
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_DCLS_HOME_TIME |
    DEST_TABLET | SOURCE_SCU;	
  //	stWifi.ucSendFlag = SET;
  //	stEthernet.ucSendFlag = SET;
  fnXML_FIFO(&stTx1Message);
  /****************************************************************/		
  /*OPMM_HOME_DCLS_REACH_TIME CONFIG_PARA_53*/
  stTx1Message.DLC = FIVE_BYTES;
  
  SplitByte.fData = stConfigData.fConfig_HOMETime;
  
  stTx1Message.Data[DATA_BYTE_ZERO] = BIT1_SET ;
  
  /**************************DATA***********************************/
  for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
  {
    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
  }
  /****************************************************************/
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_DCLS_HOME_TIME |
    DEST_HMI | SOURCE_SCU;
  fnCANMsg_Transmit(&stTx1Message, CAN1);
  
#ifdef DBG_CONFIG_SEND_HMI
  printf("\n\rOPMM_HOME_DCLS_REACH_TIME CONFIG_PARA_53= %x\r\n", stTx1Message.ExtId);
#endif
  
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_DCLS_HOME_TIME |
    DEST_TABLET | SOURCE_SCU;	
  //	stWifi.ucSendFlag = SET;
  //	stEthernet.ucSendFlag = SET;
  fnXML_FIFO(&stTx1Message);
  /****************************************************************/		
  /*OPMM_GUN_POSITION_CHANGE_TIME CONFIG_PARA_54*/
  stTx1Message.DLC = FOUR_BYTES;
  
  SplitByte.fData = stConfigData.fConfig_GunPosChangeTime;
  
  
  
  /**************************DATA***********************************/
  for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
  {
    stTx1Message.Data[ucLoopCnt]= SplitByte.rgucData[ucLoopCnt] ;
  }
  /****************************************************************/		
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_GUN_POSITION_CHANGE_TIME |
    DEST_HMI | SOURCE_SCU;
  fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
  printf("\n\rOPMM_GUN_POSITION_CHANGE_TIME CONFIG_PARA_54= %x\r\n", stTx1Message.ExtId);
#endif
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_GUN_POSITION_CHANGE_TIME |
    DEST_TABLET | SOURCE_SCU;	
  //	stWifi.ucSendFlag = SET;
  //	stEthernet.ucSendFlag = SET;
  fnXML_FIFO(&stTx1Message);
  /****************************************************************/		
  /*OPMM_DELAY_SPRAY_VALVE_FLUSH_VBAT_TIME CONFIG_PARA_55*/
  stTx1Message.DLC = FOUR_BYTES;
  
  SplitByte.fData = stConfigData.fConfig_TimebeforeONSprayValve;
  
  
  
  /**************************DATA***********************************/
  for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
  {
    stTx1Message.Data[ucLoopCnt]= SplitByte.rgucData[ucLoopCnt] ;
  }
  /****************************************************************/		
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_SPRAY_VALVE_OPEN_TIME_VBAT |
    DEST_HMI | SOURCE_SCU;
  fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
  printf("\n\rOPMM_DELAY_SPRAY_VALVE_FLUSH_VBAT_TIME CONFIG_PARA_55= %x\r\n", stTx1Message.ExtId);
#endif
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_SPRAY_VALVE_OPEN_TIME_VBAT |
    DEST_TABLET | SOURCE_SCU;	
  //	stWifi.ucSendFlag = SET;
  //	stEthernet.ucSendFlag = SET;
  fnXML_FIFO(&stTx1Message);
  /****************************************************************/		
  /*OPMM_FLUSH_WARNNING_TIME CONFIG_PARA_56*/
  stTx1Message.DLC = FOUR_BYTES;
  
  SplitByte.fData = stConfigData.fConfig_FlushWarnTime;
  
  
  
  /**************************DATA***********************************/
  for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
  {
    stTx1Message.Data[ucLoopCnt]= SplitByte.rgucData[ucLoopCnt] ;
  }
  /****************************************************************/		
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_FLUSH_WARNNING_TIME |
    DEST_HMI | SOURCE_SCU;
  fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
  printf("\n\rOPMM_FLUSH_WARNNING_TIME CONFIG_PARA_56= %x\r\n", stTx1Message.ExtId);
#endif
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_FLUSH_WARNNING_TIME |
    DEST_TABLET | SOURCE_SCU;	
  //	stWifi.ucSendFlag = SET;
  //	stEthernet.ucSendFlag = SET;
  fnXML_FIFO(&stTx1Message);
  /****************************************************************/		
  /*OPMM_SPRAY_VALVE_CONN,
  OPMM_GUNPOS_VALVE_CONN,
  OPMM_AB_VALVE_CONN,
  OPMM_SOLVENT_VALVE_CONN,
  OPMM_FORWARD_VALVE_CONN,
  OPMM_BACKWARD_VALVE_CONN,
  CONFIG_PARA_57*/
  stTx1Message.DLC = SIX_BYTES;
  
  stTx1Message.Data[DATA_BYTE_ZERO] 	= stConfigData.ucConfig_ConnSprayvalve;
  stTx1Message.Data[DATA_BYTE_ONE] 		= stConfigData.ucConfig_ConnGunposition;
  stTx1Message.Data[DATA_BYTE_TWO] 		= stConfigData.ucConfig_ConnABvalve;
  stTx1Message.Data[DATA_BYTE_THREE] 	= stConfigData.ucConfig_ConnSolventvalve;
  stTx1Message.Data[DATA_BYTE_FOUR] 	= stConfigData.ucConfig_ConnForwardvalve;
  stTx1Message.Data[DATA_BYTE_FIVE] 	= stConfigData.ucConfig_ConnBackwardvalve;
  
  
  
  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_OPMM_VALVE_REMAP |
    DEST_HMI | SOURCE_SCU;
  fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
  printf("\n\rOPMM_SPRAY_VALVE_CONN,\
         OPMM_GUNPOS_VALVE_CONN,\
           OPMM_AB_VALVE_CONN,\
             OPMM_SOLVENT_VALVE_CONN,\
               OPMM_FORWARD_VALVE_CONN,\
                 OPMM_BACKWARD_VALVE_CONN,\
                   CONFIG_PARA_57= %x\r\n", stTx1Message.ExtId);
#endif
                     stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_OPMM_VALVE_REMAP |
                       DEST_TABLET | SOURCE_SCU;	
         //	stWifi.ucSendFlag = SET;
         //	stEthernet.ucSendFlag = SET;
         fnXML_FIFO(&stTx1Message);
         /****************************************************************/		
         /* OPMM_NON_CONTACT12_CONN,OPMM_NON_CONTACT3_CONN,OPMM_NON_CONTACT6_CONN
         OPMM_NON_CONTACT9_CONN,
         CONFIG_PARA_89 */
         stTx1Message.DLC = FOUR_BYTES;
         
         stTx1Message.Data[DATA_BYTE_ZERO] 	= stConfigData.ucConfig_ConnNonContactat12;
         stTx1Message.Data[DATA_BYTE_ONE] 		= stConfigData.ucConfig_ConnNonContactat3;
         stTx1Message.Data[DATA_BYTE_TWO] 		= stConfigData.ucConfig_ConnNonContactat6;
         stTx1Message.Data[DATA_BYTE_THREE] 	= stConfigData.ucConfig_ConnNonContactat9;
         
         
         
         stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_OPMM_NON_CONTACT_REMAP |
           DEST_HMI | SOURCE_SCU;
         fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
         printf("\n\rOPMM_NON_CONTACT12_CONN,OPMM_NON_CONTACT3_CONN,OPMM_NON_CONTACT6_CONN\
                OPMM_NON_CONTACT9_CONN,\
                  CONFIG_PARA_89= %x\r\n", stTx1Message.ExtId);
#endif
                    stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_OPMM_NON_CONTACT_REMAP |
                      DEST_TABLET | SOURCE_SCU;	
                //	stWifi.ucSendFlag = SET;
                //	stEthernet.ucSendFlag = SET;
                fnXML_FIFO(&stTx1Message);
                /****************************************************************/		
                
                
                /*OPMM_BYPASS CONFIG_PARA_58*/
                stTx1Message.DLC = TWO_BYTES;
                
                SplitByte.iData 	= stConfigData.uiConfig_OPMMBypass;
                
                /**************************DATA***********************************/
                for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                {
                  stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt];
                }
                /****************************************************************/
                
                
                
                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_OPMM_PERIPHRAL_BYPASS |
                  DEST_HMI | SOURCE_SCU;
                fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
                printf("\n\rOPMM_BYPASS CONFIG_PARA_58= %x\r\n", stTx1Message.ExtId);
#endif
                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_OPMM_PERIPHRAL_BYPASS |
                  DEST_TABLET | SOURCE_SCU;	
                //	stWifi.ucSendFlag = SET;
                //	stEthernet.ucSendFlag = SET;
                fnXML_FIFO(&stTx1Message);
                /****************************************************************/	
#endif
                /*****************************SCU*************************************/	
#ifdef SCU
                
                /*SCU_SPRAY_PRESSURE_VALUE CONFIG_PARA_59*/
                stTx1Message.DLC = FIVE_BYTES;
                
                SplitByte.fData = stConfigData.fConfig_SprayPressure;
                
                stTx1Message.Data[DATA_BYTE_ZERO] = BIT0_SET ;
                
                /**************************DATA***********************************/
                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                {
                  stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
                }
                /****************************************************************/
                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_SPRAY_PRESSURE |
                  DEST_HMI | SOURCE_SCU;
                fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
                printf("\n\rSCU_SPRAY_PRESSURE_VALUE CONFIG_PARA_59= %x\r\n", stTx1Message.ExtId);
#endif
                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_SPRAY_PRESSURE |
                  DEST_TABLET | SOURCE_SCU;	
                //	stWifi.ucSendFlag = SET;
                //	stEthernet.ucSendFlag = SET;
                fnXML_FIFO(&stTx1Message);
                /****************************************************************/		
                /*SCU_SPRAY_PRESSURE_THRESH_LOW CONFIG_PARA_60*/
                stTx1Message.DLC = FIVE_BYTES;
                
                SplitByte.fData = stConfigData.fConfig_SprayPressureLTH;
                
                stTx1Message.Data[DATA_BYTE_ZERO] = BIT1_SET ;
                
                /**************************DATA***********************************/
                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                {
                  stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
                }
                /****************************************************************/
                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_SPRAY_PRESSURE |
                  DEST_HMI | SOURCE_SCU;
                fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
                printf("\n\r SCU_SPRAY_PRESSURE_THRESH_LOW CONFIG_PARA_60= %x\r\n", stTx1Message.ExtId);
#endif
                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_SPRAY_PRESSURE |
                  DEST_TABLET | SOURCE_SCU;	
                //	stWifi.ucSendFlag = SET;
                //	stEthernet.ucSendFlag = SET;
                fnXML_FIFO(&stTx1Message);
                /****************************************************************/		
                /*SCU_SPRAY_PRESSURE_THRESH_HIGH CONFIG_PARA_61*/
                stTx1Message.DLC = FIVE_BYTES;
                
                SplitByte.fData = stConfigData.fConfig_SprayPressureHTH;
                
                stTx1Message.Data[DATA_BYTE_ZERO] = BIT2_SET ;
                
                /**************************DATA***********************************/
                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                {
                  stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
                }
                /****************************************************************/
                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_SPRAY_PRESSURE |
                  DEST_HMI | SOURCE_SCU;
                fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
                printf("\n\r SCU_SPRAY_PRESSURE_THRESH_HIGH CONFIG_PARA_61= %x\r\n", stTx1Message.ExtId);
#endif
                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_SPRAY_PRESSURE |
                  DEST_TABLET | SOURCE_SCU;	
                //	stWifi.ucSendFlag = SET;
                //	stEthernet.ucSendFlag = SET;
                fnXML_FIFO(&stTx1Message);
                /****************************************************************/		
                /*SCU_SPRAY_PRESSURE_HYST_POS CONFIG_PARA_62*/
                stTx1Message.DLC = FIVE_BYTES;
                
                SplitByte.fData = stConfigData.fConfig_HysSprayPresPos;
                
                stTx1Message.Data[DATA_BYTE_ZERO] = BIT3_SET ;
                
                /**************************DATA***********************************/
                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                {
                  stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
                }
                /****************************************************************/
                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_SPRAY_PRESSURE |
                  DEST_HMI | SOURCE_SCU;
                fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
                printf("\n\rSCU_SPRAY_PRESSURE_HYST_POS CONFIG_PARA_62= %x\r\n", stTx1Message.ExtId);
#endif
                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_SPRAY_PRESSURE |
                  DEST_TABLET | SOURCE_SCU;	
                //	stWifi.ucSendFlag = SET;
                //	stEthernet.ucSendFlag = SET;
                fnXML_FIFO(&stTx1Message);
                /****************************************************************/		
                /*SCU_SPRAY_PRESSURE_HYST_NEG CONFIG_PARA_63*/
                stTx1Message.DLC = FIVE_BYTES;
                
                SplitByte.fData = stConfigData.fConfig_HysSprayPresNeg;
                
                stTx1Message.Data[DATA_BYTE_ZERO] = BIT4_SET ;
                
                /**************************DATA***********************************/
                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                {
                  stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
                }
                /****************************************************************/
                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_SPRAY_PRESSURE |
                  DEST_HMI | SOURCE_SCU;
                fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
                printf("\n\r SCU_SPRAY_PRESSURE_HYST_NEG CONFIG_PARA_63= %x\r\n", stTx1Message.ExtId);
#endif
                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_SPRAY_PRESSURE |
                  DEST_TABLET | SOURCE_SCU;	
                //	stWifi.ucSendFlag = SET;
                //	stEthernet.ucSendFlag = SET;
                fnXML_FIFO(&stTx1Message);
                /****************************************************************/		
                /*SCU_RECIRC_PRESSURE_VALUE CONFIG_PARA_64*/
                stTx1Message.DLC = FIVE_BYTES;
                
                SplitByte.fData = stConfigData.fConfig_RecirPressure;
                
                stTx1Message.Data[DATA_BYTE_ZERO] = BIT0_SET ;
                
                /**************************DATA***********************************/
                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                {
                  stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
                }
                /****************************************************************/
                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_RECIRC_PRESSURE |
                  DEST_HMI | SOURCE_SCU;
                fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
                printf("\n\r SCU_RECIRC_PRESSURE_VALUE CONFIG_PARA_64= %x\r\n", stTx1Message.ExtId);
#endif
                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_RECIRC_PRESSURE |
                  DEST_TABLET | SOURCE_SCU;
                //	stWifi.ucSendFlag = SET;
                //	stEthernet.ucSendFlag = SET;
                fnXML_FIFO(&stTx1Message);
                /****************************************************************/		
                /*SCU_RECIRC_PRESSURE_THRESH_LOW CONFIG_PARA_65*/
                stTx1Message.DLC = FIVE_BYTES;
                
                SplitByte.fData = stConfigData.fConfig_RecirPressureLTH;
                
                stTx1Message.Data[DATA_BYTE_ZERO] = BIT1_SET ;
                
                /**************************DATA***********************************/
                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                {
                  stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
                }
                /****************************************************************/
                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_RECIRC_PRESSURE |
                  DEST_HMI | SOURCE_SCU;
                fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
                printf("\n\r SCU_RECIRC_PRESSURE_THRESH_LOW CONFIG_PARA_65= %x\r\n", stTx1Message.ExtId);
#endif
                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_RECIRC_PRESSURE |
                  DEST_TABLET | SOURCE_SCU;
                //	stWifi.ucSendFlag = SET;
                //	stEthernet.ucSendFlag = SET;			
                fnXML_FIFO(&stTx1Message);
                /****************************************************************/		
                /*SCU_RECIRC_PRESSURE_THRESH_HIGH CONFIG_PARA_66*/
                stTx1Message.DLC = FIVE_BYTES;
                
                SplitByte.fData = stConfigData.fConfig_RecirPressureHTH;
                
                stTx1Message.Data[DATA_BYTE_ZERO] = BIT2_SET ;
                
                /**************************DATA***********************************/
                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                {
                  stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
                }
                /****************************************************************/
                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_RECIRC_PRESSURE |
                  DEST_HMI | SOURCE_SCU;
                fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
                printf("\n\r SCU_RECIRC_PRESSURE_THRESH_HIGH CONFIG_PARA_66= %x\r\n", stTx1Message.ExtId);
#endif
                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_RECIRC_PRESSURE |
                  DEST_TABLET | SOURCE_SCU;	
                //	stWifi.ucSendFlag = SET;
                //	stEthernet.ucSendFlag = SET;
                fnXML_FIFO(&stTx1Message);
                /****************************************************************/		
                /*SCU_RECIRC_PRESSURE_HYST_POS CONFIG_PARA_67*/
                stTx1Message.DLC = FIVE_BYTES;
                
                SplitByte.fData = stConfigData.fConfig_HysRecircPresPos;
                
                stTx1Message.Data[DATA_BYTE_ZERO] = BIT3_SET ;
                
                /**************************DATA***********************************/
                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                {
                  stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
                }
                /****************************************************************/
                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_RECIRC_PRESSURE |
                  DEST_HMI | SOURCE_SCU;
                fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
                printf("\n\r SCU_RECIRC_PRESSURE_HYST_POS CONFIG_PARA_67= %x\r\n", stTx1Message.ExtId);
#endif
                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_RECIRC_PRESSURE |
                  DEST_TABLET | SOURCE_SCU;	
                //	stWifi.ucSendFlag = SET;
                //	stEthernet.ucSendFlag = SET;
                fnXML_FIFO(&stTx1Message);
                /****************************************************************/		
                /*SCU_RECIRC_PRESSURE_HYST_NEG CONFIG_PARA_68*/
                stTx1Message.DLC = FIVE_BYTES;
                
                SplitByte.fData = stConfigData.fConfig_HysRecircPresNeg;
                
                stTx1Message.Data[DATA_BYTE_ZERO] = BIT4_SET ;
                
                /**************************DATA***********************************/
                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                {
                  stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
                }
                /****************************************************************/
                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_RECIRC_PRESSURE |
                  DEST_HMI | SOURCE_SCU;
                fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
                printf("\n\r SCU_RECIRC_PRESSURE_HYST_NEG CONFIG_PARA_68= %x\r\n", stTx1Message.ExtId);
#endif
                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_RECIRC_PRESSURE |
                  DEST_TABLET | SOURCE_SCU;	
                //	stWifi.ucSendFlag = SET;
                //	stEthernet.ucSendFlag = SET;
                fnXML_FIFO(&stTx1Message);
                /****************************************************************/		
                /*COUNT_JOINT CONFIG_PARA_69*/
                stTx1Message.DLC = FOUR_BYTES;
                
                SplitByte.fData = stConfigData.fConfig_JointNum;
                
                
                
                /**************************DATA***********************************/
                for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                {
                  stTx1Message.Data[ucLoopCnt]= SplitByte.rgucData[ucLoopCnt] ;
                }
                /****************************************************************/		
                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_JOINT_NUMBER |
                  DEST_HMI | SOURCE_SCU;
                fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
                printf("\n\rCOUNT_JOINT CONFIG_PARA_69= %x\r\n", stTx1Message.ExtId);
#endif
                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_JOINT_NUMBER |
                  DEST_TABLET | SOURCE_SCU;	
                //	stWifi.ucSendFlag = SET;
                //	stEthernet.ucSendFlag = SET;
                fnXML_FIFO(&stTx1Message);
                /****************************************************************/		
                /*TIME_DATA_LOGGING CONFIG_PARA_70*/
                stTx1Message.DLC = FOUR_BYTES;
                
                SplitByte.fData = stConfigData.fConfig_DataLoggingPeriod;
                
                
                
                /**************************DATA***********************************/
                for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                {
                  stTx1Message.Data[ucLoopCnt]= SplitByte.rgucData[ucLoopCnt] ;
                }
                /****************************************************************/		
                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_DATA_LOG_PERIOD |
                  DEST_HMI | SOURCE_SCU;
                fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
                printf("\n\rTIME_DATA_LOGGING CONFIG_PARA_70= %x\r\n", stTx1Message.ExtId);
#endif
                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_DATA_LOG_PERIOD |
                  DEST_TABLET | SOURCE_SCU;	
                //	stWifi.ucSendFlag = SET;
                //	stEthernet.ucSendFlag = SET;
                fnXML_FIFO(&stTx1Message);
                /****************************************************************/		
                /*SYS_TIME CONFIG_PARA_71*/
                //stTx1Message.DLC = SEVEN_BYTES;
                //			
                //				/**************************DATA***********************************/
                //					for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                //						{
                //							stTx1Message.Data[ucLoopCnt] = stConfigData.ucConfig_SystemTime[ucLoopCnt]; 
                //						}
                //					/****************************************************************/		
                //		stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_SYS_DATE_TIME |
                //											 DEST_HMI | SOURCE_SCU;
                //	fnCANMsg_Transmit(&stTx1Message, CAN1);
                //	#ifdef DBG_CONFIG_SEND_HMI
                //	printf("\n\r SYS_TIME CONFIG_PARA_71= %x\r\n", stTx1Message.ExtId);
                //	#endif
                //	stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_SYS_DATE_TIME |
                //											 DEST_TABLET | SOURCE_SCU;
                ////	stWifi.ucSendFlag = SET;
                ////	stEthernet.ucSendFlag = SET;
                //	fnXML_FIFO(&stTx1Message);
                /* Send RTC Reading every second to HMI */
                uiRTCReadCnt = 0;
                
                RTC_GetTime(RTC_Format_BIN, &stRTC_TimeStructure);
                RTC_GetDate(RTC_Format_BIN, &stRTC_DateStructure);
                
                stConfigData.ucConfig_SystemTime[0] = stRTC_TimeStructure.RTC_Hours ;
                stConfigData.ucConfig_SystemTime[1] = stRTC_TimeStructure.RTC_Minutes;
                stConfigData.ucConfig_SystemTime[2] = stRTC_TimeStructure.RTC_Seconds;
				
                stConfigData.ucConfig_SystemTime[3] = stRTC_DateStructure.RTC_Date;
                stConfigData.ucConfig_SystemTime[4] = stRTC_DateStructure.RTC_Month;
                stConfigData.ucConfig_SystemTime[5] = 20;
                stConfigData.ucConfig_SystemTime[6] = stRTC_DateStructure.RTC_Year;
                
#ifdef GENERAL_DBG_RTC
                printf("\n\r%02d/%02d/20%02d %02d:%02d:%02d",
                       stConfigData.ucConfig_SystemTime[3],	stConfigData.ucConfig_SystemTime[4],
                       stConfigData.ucConfig_SystemTime[6], stConfigData.ucConfig_SystemTime[0],
                       stConfigData.ucConfig_SystemTime[1],	stConfigData.ucConfig_SystemTime[2]);
#endif								
                stTx1Message.DLC = SEVEN_BYTES;
                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_SYS_DATE_TIME |
                  DEST_HMI | SOURCE_SCU;
                for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                {
                  stTx1Message.Data[ucLoopCnt] = 
					stConfigData.ucConfig_SystemTime[ucLoopCnt];
                }
                fnCANMsg_Transmit(&stTx1Message, CAN1);
                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_SYS_DATE_TIME |
                  DEST_TABLET | SOURCE_SCU;
                //stWifi.ucSendFlag = SET;
                //stEthernet.ucSendFlag = SET;
                fnXML_FIFO(&stTx1Message);
                /****************************************************************/		
                /*SUPERVISOR_NAME1 CONFIG_PARA_72*/
                stTx1Message.DLC = EIGHT_BYTES;
                stTx1Message.Data[DATA_BYTE_ZERO] = BIT0_SET ;
                
                /**************************DATA***********************************/
                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                {
                  stTx1Message.Data[ucLoopCnt]	 = stConfigData.rgucConfig_SupervisorName1[ucLoopCnt - 1];
                }
                /****************************************************************/
                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_SUPERVISOR_NAME |
                  DEST_HMI | SOURCE_SCU;
                fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
                printf("\n\r SUPERVISOR_NAME1 CONFIG_PARA_72= %x\r\n", stTx1Message.ExtId);
#endif
                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_SUPERVISOR_NAME |
                  DEST_TABLET | SOURCE_SCU;	
                //	stWifi.ucSendFlag = SET;
                //	stEthernet.ucSendFlag = SET;
                fnXML_FIFO(&stTx1Message);
                /****************************************************************/		
                /*SUPERVISOR_NAME2 CONFIG_PARA_73*/
                stTx1Message.DLC = EIGHT_BYTES;
                stTx1Message.Data[DATA_BYTE_ZERO] = BIT1_SET ;
                
                /**************************DATA***********************************/
                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                {
                  stTx1Message.Data[ucLoopCnt]	 = stConfigData.rgucConfig_SupervisorName2[ucLoopCnt - 1];
                }
                /****************************************************************/
                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_SUPERVISOR_NAME |
                  DEST_HMI | SOURCE_SCU;
                fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
                printf("\n\r SUPERVISOR_NAME2 CONFIG_PARA_73= %x\r\n", stTx1Message.ExtId);
#endif
                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_SUPERVISOR_NAME |
                  DEST_TABLET | SOURCE_SCU;
                //	stWifi.ucSendFlag = SET;
                //	stEthernet.ucSendFlag = SET;
                fnXML_FIFO(&stTx1Message);
                /****************************************************************/		
                /*SUPERVISOR_NAME3 CONFIG_PARA_74*/
                stTx1Message.DLC = EIGHT_BYTES;
                stTx1Message.Data[DATA_BYTE_ZERO] = BIT2_SET ;
                
                /**************************DATA***********************************/
                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                {
                  stTx1Message.Data[ucLoopCnt]	 = stConfigData.rgucConfig_SupervisorName3[ucLoopCnt - 1];
                }
                /****************************************************************/
                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_SUPERVISOR_NAME |
                  DEST_HMI | SOURCE_SCU;
                fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
                printf("\n\r SUPERVISOR_NAME3 CONFIG_PARA_74= %x\r\n", stTx1Message.ExtId);
#endif
                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_SUPERVISOR_NAME |
                  DEST_TABLET | SOURCE_SCU;	
                //	stWifi.ucSendFlag = SET;
                //	stEthernet.ucSendFlag = SET;
                fnXML_FIFO(&stTx1Message);
                /****************************************************************/		
                /*COATING_MAT_NAME1 CONFIG_PARA_75*/
                stTx1Message.DLC = EIGHT_BYTES;
                stTx1Message.Data[DATA_BYTE_ZERO] = BIT0_SET ;
                
                /**************************DATA***********************************/
                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                {
                  stTx1Message.Data[ucLoopCnt]	 = stConfigData.rgucConfig_CoatingMatName1[ucLoopCnt - 1];
                }
                /****************************************************************/
                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_COATING_MATERIAL |
                  DEST_HMI | SOURCE_SCU;
                fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
                printf("\n\r COATING_MAT_NAME1 CONFIG_PARA_75= %x\r\n", stTx1Message.ExtId);
#endif
                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_COATING_MATERIAL |
                  DEST_TABLET | SOURCE_SCU;	
                //	stWifi.ucSendFlag = SET;
                //	stEthernet.ucSendFlag = SET;
                fnXML_FIFO(&stTx1Message);
                /****************************************************************/		
                /*COATING_MAT_NAME2 CONFIG_PARA_76*/
                stTx1Message.DLC = EIGHT_BYTES;
                stTx1Message.Data[DATA_BYTE_ZERO] = BIT1_SET ;
                
                /**************************DATA***********************************/
                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                {
                  stTx1Message.Data[ucLoopCnt]	 = stConfigData.rgucConfig_CoatingMatName2[ucLoopCnt - 1];
                }
                /****************************************************************/
                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_COATING_MATERIAL |
                  DEST_HMI | SOURCE_SCU;
                fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
                printf("\n\r COATING_MAT_NAME2 CONFIG_PARA_76= %x\r\n", stTx1Message.ExtId);
#endif
                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_COATING_MATERIAL |
                  DEST_TABLET | SOURCE_SCU;	
                //	stWifi.ucSendFlag = SET;
                //	stEthernet.ucSendFlag = SET;
                fnXML_FIFO(&stTx1Message);
                /****************************************************************/		
                /*COATING_MAT_NAME3 CONFIG_PARA_77*/
                stTx1Message.DLC = EIGHT_BYTES;
                stTx1Message.Data[DATA_BYTE_ZERO] = BIT2_SET ;
                
                /**************************DATA***********************************/
                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                {
                  stTx1Message.Data[ucLoopCnt]	 = stConfigData.rgucConfig_CoatingMatName3[ucLoopCnt - 1];
                }
                /****************************************************************/
                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_COATING_MATERIAL |
                  DEST_HMI | SOURCE_SCU;
                fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
                printf("\n\r COATING_MAT_NAME3 CONFIG_PARA_77= %x\r\n", stTx1Message.ExtId);
#endif
                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_COATING_MATERIAL |
                  DEST_TABLET | SOURCE_SCU;
                //	stWifi.ucSendFlag = SET;
                //	stEthernet.ucSendFlag = SET;			
                fnXML_FIFO(&stTx1Message);
                /****************************************************************/		
                /*BASE_MAT_BATCHNO1 CONFIG_PARA_78*/
                stTx1Message.DLC = EIGHT_BYTES;
                stTx1Message.Data[DATA_BYTE_ZERO] = BIT0_SET ;
                
                /**************************DATA***********************************/
                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                {
                  stTx1Message.Data[ucLoopCnt]	 = stConfigData.rgucConfig_BaseMatBatchNo1[ucLoopCnt - 1];
                }
                /****************************************************************/
                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_BASE_MAT_BATCH_NO |
                  DEST_HMI | SOURCE_SCU;
                fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
                printf("\n\r BASE_MAT_BATCHNO1 CONFIG_PARA_78= %x\r\n", stTx1Message.ExtId);
#endif
                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_BASE_MAT_BATCH_NO |
                  DEST_TABLET | SOURCE_SCU;	
                //	stWifi.ucSendFlag = SET;
                //	stEthernet.ucSendFlag = SET;
                fnXML_FIFO(&stTx1Message);
                /****************************************************************/		
                /*BASE_MAT_BATCHNO2 CONFIG_PARA_79*/
                stTx1Message.DLC = EIGHT_BYTES;
                stTx1Message.Data[DATA_BYTE_ZERO] = BIT1_SET ;
                
                /**************************DATA***********************************/
                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                {
                  stTx1Message.Data[ucLoopCnt]	 = stConfigData.rgucConfig_BaseMatBatchNo2[ucLoopCnt - 1];
                }
                /****************************************************************/
                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_BASE_MAT_BATCH_NO |
                  DEST_HMI | SOURCE_SCU;
                fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
                printf("\n\r BASE_MAT_BATCHNO2 CONFIG_PARA_79= %x\r\n", stTx1Message.ExtId);
#endif
                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_BASE_MAT_BATCH_NO |
                  DEST_TABLET | SOURCE_SCU;	
                //	stWifi.ucSendFlag = SET;
                //	stEthernet.ucSendFlag = SET;
                fnXML_FIFO(&stTx1Message);
                /****************************************************************/		
                /*BASE_MAT_BATCHNO3 CONFIG_PARA_80*/
                stTx1Message.DLC = EIGHT_BYTES;
                stTx1Message.Data[DATA_BYTE_ZERO] = BIT2_SET ;
                
                /**************************DATA***********************************/
                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                {
                  stTx1Message.Data[ucLoopCnt]	 = stConfigData.rgucConfig_BaseMatBatchNo3[ucLoopCnt - 1];
                }
                /****************************************************************/
                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_BASE_MAT_BATCH_NO |
                  DEST_HMI | SOURCE_SCU;
                fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
                printf("\n\r BASE_MAT_BATCHNO3 CONFIG_PARA_80= %x\r\n", stTx1Message.ExtId);
#endif
                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_BASE_MAT_BATCH_NO |
                  DEST_TABLET | SOURCE_SCU;	
                //	stWifi.ucSendFlag = SET;
                //	stEthernet.ucSendFlag = SET;
                fnXML_FIFO(&stTx1Message);
                /****************************************************************/		
                /*HARD_MAT_BATCHNO1 CONFIG_PARA_81*/
                stTx1Message.DLC = EIGHT_BYTES;
                stTx1Message.Data[DATA_BYTE_ZERO] = BIT0_SET ;
                
                /**************************DATA***********************************/
                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                {
                  stTx1Message.Data[ucLoopCnt]	 = stConfigData.rgucConfig_HardMatBatchNo1[ucLoopCnt - 1];
                }
                /****************************************************************/
                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_HARD_MAT_BATCH_NO |
                  DEST_HMI | SOURCE_SCU;
                fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
                printf("\n\r HARD_MAT_BATCHNO1 CONFIG_PARA_81= %x\r\n", stTx1Message.ExtId);
#endif
                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_HARD_MAT_BATCH_NO |
                  DEST_TABLET | SOURCE_SCU;	
                //	stWifi.ucSendFlag = SET;
                //	stEthernet.ucSendFlag = SET;
                fnXML_FIFO(&stTx1Message);
                /****************************************************************/		
                /*HARD_MAT_BATCHNO2 CONFIG_PARA_82*/
                stTx1Message.DLC = EIGHT_BYTES;
                stTx1Message.Data[DATA_BYTE_ZERO] = BIT1_SET ;
                
                /**************************DATA***********************************/
                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                {
                  stTx1Message.Data[ucLoopCnt]	 = stConfigData.rgucConfig_HardMatBatchNo2[ucLoopCnt - 1];
                }
                /****************************************************************/
                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_HARD_MAT_BATCH_NO |
                  DEST_HMI | SOURCE_SCU;
                fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
                printf("\n\r HARD_MAT_BATCHNO2 CONFIG_PARA_82= %x\r\n", stTx1Message.ExtId);
#endif
                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_HARD_MAT_BATCH_NO |
                  DEST_TABLET | SOURCE_SCU;	
                //	stWifi.ucSendFlag = SET;
                //	stEthernet.ucSendFlag = SET;
                fnXML_FIFO(&stTx1Message);
                /****************************************************************/		
                /*HARD_MAT_BATCHNO3 CONFIG_PARA_83*/
                stTx1Message.DLC = EIGHT_BYTES;
                stTx1Message.Data[DATA_BYTE_ZERO] = BIT2_SET ;
                
                /**************************DATA***********************************/
                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                {
                  stTx1Message.Data[ucLoopCnt]	 = stConfigData.rgucConfig_HardMatBatchNo3[ucLoopCnt - 1];
                }
                /****************************************************************/
                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_HARD_MAT_BATCH_NO |
                  DEST_HMI | SOURCE_SCU;
                fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
                printf("\n\r HARD_MAT_BATCHNO3 CONFIG_PARA_83= %x\r\n", stTx1Message.ExtId);
#endif
                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_HARD_MAT_BATCH_NO |
                  DEST_TABLET | SOURCE_SCU;	
                //	stWifi.ucSendFlag = SET;
                //	stEthernet.ucSendFlag = SET;
                fnXML_FIFO(&stTx1Message);
                /****************************************************************/		
                
                /****************************************************************/		
                /*SCU_BYPASS CONFIG_PARA_85*/
                stTx1Message.DLC = ONE_BYTES;
                stTx1Message.Data[DATA_BYTE_ZERO] = stConfigData.ucConfig_SCUBypass  ;
                
                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_SCU_PERIPHRAL_BYPASS |
                  DEST_HMI | SOURCE_SCU;
                fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
                printf("\n\r SCU_BYPASS CONFIG_PARA_85= %x\r\n", stTx1Message.ExtId);
#endif
                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_SCU_PERIPHRAL_BYPASS |
                  DEST_TABLET | SOURCE_SCU;	
                //	stWifi.ucSendFlag = SET;
                //	stEthernet.ucSendFlag = SET;
                fnXML_FIFO(&stTx1Message);
                /****************************************************************/		
                /*OPMM_PIPE_TEMP_THRES_LOW CONFIG_PARA_86*/
                stTx1Message.DLC = FIVE_BYTES;
                
                SplitByte.fData = stConfigData.fConfig_PipeTempLTH;
                
                stTx1Message.Data[DATA_BYTE_ZERO] = BIT0_SET ;
                
                /**************************DATA***********************************/
                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                {
                  stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
                }
                /****************************************************************/
                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_PIPE_TEMP_TH |
                  DEST_HMI | SOURCE_SCU;
                fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
                printf("\n\rOPMM_PIPE_TEMP_THRES_LOW CONFIG_PARA_86= %x\r\n", stTx1Message.ExtId);
#endif
                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_PIPE_TEMP_TH |
                  DEST_TABLET | SOURCE_SCU;	
                //	stWifi.ucSendFlag = SET;
                //	stEthernet.ucSendFlag = SET;
                fnXML_FIFO(&stTx1Message);
                /****************************************************************/		
                /*OPMM_PIPE_TEMP_THRES_HIGH CONFIG_PARA_87*/
                stTx1Message.DLC = FIVE_BYTES;
                
                SplitByte.fData = stConfigData.fConfig_PipeTempHTH;
                
                stTx1Message.Data[DATA_BYTE_ZERO] = BIT1_SET ;
                
                /**************************DATA***********************************/
                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                {
                  stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
                }
                /****************************************************************/
                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_PIPE_TEMP_TH |
                  DEST_HMI | SOURCE_SCU;
                fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
                printf("\n\rOPMM_PIPE_TEMP_THRES_HIGH CONFIG_PARA_87= %x\r\n", stTx1Message.ExtId);
#endif
                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_PIPE_TEMP_TH |
                  DEST_TABLET | SOURCE_SCU;	\
                    //	stWifi.ucSendFlag = SET;
                    //	stEthernet.ucSendFlag = SET;
                    fnXML_FIFO(&stTx1Message);
                    /****************************************************************/	
                    /*CONFIG_TEMP_PRESS_ACHIEVE_BYPASS CONFIG_PARA_88*/
                    stTx1Message.DLC = ONE_BYTES;
                    stTx1Message.Data[DATA_BYTE_ZERO] = stConfigData.ucConfig_Temp_Press_Achieve_Bypass  ;
                    
                    stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_TEMP_PRESS_ACHIEVE_BYPASS |
                      DEST_HMI | SOURCE_SCU;
                    fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
                    printf("\n\r CONFIG_TEMP_PRESS_ACHIEVE_BYPASS CONFIG_PARA_88= %x\r\n", stTx1Message.ExtId);
#endif
                    stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_TEMP_PRESS_ACHIEVE_BYPASS |
                      DEST_TABLET | SOURCE_SCU;	
                    //	stWifi.ucSendFlag = SET;
                    //	stEthernet.ucSendFlag = SET;
                    fnXML_FIFO(&stTx1Message);
                    /****************************************************************/		
                    /*CONFIG_OPMM_BUZZER_VOLUME CONFIG_PARA_94*/
                    stTx1Message.DLC = ONE_BYTES;
                    stTx1Message.Data[DATA_BYTE_ZERO] = stConfigData.ucConfig_BuzzerVolume  ;
                    
                    stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_OPMM_BUZZER_VOLUME |
                      DEST_HMI | SOURCE_SCU;
                    fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
                    printf("\n\r CONFIG_OPMM_BUZZER_VOLUME CONFIG_PARA_94= %x\r\n", stTx1Message.ExtId);
#endif
                    stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_OPMM_BUZZER_VOLUME |
                      DEST_TABLET | SOURCE_SCU;	
                    //	stWifi.ucSendFlag = SET;
                    //	stEthernet.ucSendFlag = SET;
                    fnXML_FIFO(&stTx1Message);
                    /****************************************************************/	
                    /****************************************************************/	
                    /*SCU SIGNATURE BYTE CONFIG_PARA_95*/
                    stTx1Message.DLC = TWO_BYTES;
                    SplitByte.uiData = stConfigData.uiConfig_Signature;
                    /**************************DATA***********************************/
					for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                    {
                      stTx1Message.Data[ucLoopCnt]= SplitByte.rgucData[ucLoopCnt] ;
                    }
                    
                    stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_SIGNATURE_BYTE |
                      DEST_HMI | SOURCE_SCU;
                    fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
                    printf("\n\r SCU SIGNATURE BYTE CONFIG_PARA_95= %x\r\n", stTx1Message.ExtId);
#endif
                    stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_SIGNATURE_BYTE |
                      DEST_TABLET | SOURCE_SCU;	
                    //	stWifi.ucSendFlag = SET;
                    //	stEthernet.ucSendFlag = SET;
                    fnXML_FIFO(&stTx1Message);	
#endif
                    /************************change8*************************************/		
                    /*SCU_TIME_SEND_STALL_COMMAND CONFIG_PARA_96*/
                    stTx1Message.DLC = FOUR_BYTES;
                    
                    SplitByte.fData = stConfigData.fConfig_TimeBeforeSendingStallcommand;
                    
                    
                    
                    /**************************DATA***********************************/
					for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                    {
                      stTx1Message.Data[ucLoopCnt]= SplitByte.rgucData[ucLoopCnt] ;
                    }
					/****************************************************************/		
                    stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_SCU_STALL_COMMAND_TIME |
                      DEST_HMI | SOURCE_SCU;
                    fnCANMsg_Transmit(&stTx1Message, CAN1);
                    
#ifdef DBG_CONFIG_SEND_HMI
                    printf("\n\rSCU_TIME_SEND_STALL_COMMAND CONFIG_PARA_96= %x\r\n", stTx1Message.ExtId);
#endif
                    
                    stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_SCU_STALL_COMMAND_TIME |
                      DEST_TABLET | SOURCE_SCU;	
                    //	stWifi.ucSendFlag = SET;
                    //	stEthernet.ucSendFlag = SET;
                    fnXML_FIFO(&stTx1Message);
                    /****************************************************************/		
                    /****************************************************************/		
                    /*OPMM_STOP_HLS_TIMER CONFIG_PARA_97*/
                    stTx1Message.DLC = FOUR_BYTES;
                    
                    SplitByte.fData = stConfigData.fConfig_TimeStopOPMMcommand;
                    
                    
                    
                    /**************************DATA***********************************/
					for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                    {
                      stTx1Message.Data[ucLoopCnt]= SplitByte.rgucData[ucLoopCnt] ;
                    }
					/****************************************************************/		
                    stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_OPMM_STOP_HLS_TIME |
                      DEST_HMI | SOURCE_SCU;
                    fnCANMsg_Transmit(&stTx1Message, CAN1);
                    
#ifdef DBG_CONFIG_SEND_HMI
                    printf("\n\rOPMM_STOP_HLS_TIMER CONFIG_PARA_97= %x\r\n", stTx1Message.ExtId);
#endif
                    
                    stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_OPMM_STOP_HLS_TIME |
                      DEST_TABLET | SOURCE_SCU;	
                    //	stWifi.ucSendFlag = SET;
                    //	stEthernet.ucSendFlag = SET;
                    fnXML_FIFO(&stTx1Message);
                    /****************************************************************/		
                    /****************************************************************/		
                    /*OPMM_BATT_KNEE_VOLT CONFIG_PARA_98*/
                    stTx1Message.DLC = FOUR_BYTES;
                    
                    SplitByte.fData = stConfigData.fConfig_BatteryKneeVoltage;
                    
                    
                    
                    /**************************DATA***********************************/
					for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                    {
                      stTx1Message.Data[ucLoopCnt]= SplitByte.rgucData[ucLoopCnt] ;
                    }
					/****************************************************************/		
                    stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_OPMM_BATT_KNEE_VOLT |
                      DEST_HMI | SOURCE_SCU;
                    fnCANMsg_Transmit(&stTx1Message, CAN1);
                    
#ifdef DBG_CONFIG_SEND_HMI
                    printf("\n\rOPMM_BATT_KNEE_VOLT CONFIG_PARA_98= %x\r\n", stTx1Message.ExtId);
#endif
                    
                    stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_OPMM_BATT_KNEE_VOLT |
                      DEST_TABLET | SOURCE_SCU;	
                    //	stWifi.ucSendFlag = SET;
                    //	stEthernet.ucSendFlag = SET;
                    fnXML_FIFO(&stTx1Message);
                    /****************************************************************/		
                    /*************************change10***************************************/		
                    /*VCM_STALL_UPSTROKE_UPPER_LIMIT CONFIG_PARA_99*/
                    stTx1Message.DLC = TWO_BYTES;
                    stTx1Message.Data[DATA_BYTE_ZERO] = BIT0_SET ;
                    stTx1Message.Data[DATA_BYTE_ONE] = stConfigData.ucConfig_UpstrokeUpLimit;
                    stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_VCM_STALL_RANGE |
                      DEST_HMI | SOURCE_SCU;
                    fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
                    printf("\n\rVCM_STALL_UPSTROKE_UPPER_LIMIT CONFIG_PARA_99= %x\r\n", stTx1Message.ExtId);
                    printf("\n\rstConfigData.ucConfig_UpstrokeUpLimit = %d\r\n", stConfigData.ucConfig_UpstrokeUpLimit);
#endif
                    stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_VCM_STALL_RANGE |
                      DEST_TABLET | SOURCE_SCU;	
                    //	stWifi.ucSendFlag = SET;
                    //	stEthernet.ucSendFlag = SET;
                    fnXML_FIFO(&stTx1Message);
                    /****************************************************************/	
                    /*VCM_STALL_UPSTROKE_MIN_RANGE CONFIG_PARA_100*/
                    stTx1Message.DLC = TWO_BYTES;
                    stTx1Message.Data[DATA_BYTE_ZERO] = BIT1_SET ;
                    stTx1Message.Data[DATA_BYTE_ONE] = stConfigData.ucConfig_UpstrokeMinRange;
                    stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_VCM_STALL_RANGE |
                      DEST_HMI | SOURCE_SCU;
                    fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
                    printf("\n\rVCM_STALL_UPSTROKE_MIN_RANGE CONFIG_PARA_100= %x\r\n", stTx1Message.ExtId);
                    printf("\n\rstConfigData.ucConfig_UpstrokeMinRange = %d\r\n", stConfigData.ucConfig_UpstrokeMinRange);
#endif
                    stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_VCM_STALL_RANGE |
                      DEST_TABLET | SOURCE_SCU;	
                    //	stWifi.ucSendFlag = SET;
                    //	stEthernet.ucSendFlag = SET;
                    fnXML_FIFO(&stTx1Message);
                    /****************************************************************/	
                    /*VCM_STALL_UPSTROKE_MAX_RANGE CONFIG_PARA_101*/
                    stTx1Message.DLC = TWO_BYTES;
                    stTx1Message.Data[DATA_BYTE_ZERO] = BIT2_SET ;
                    stTx1Message.Data[DATA_BYTE_ONE] = stConfigData.ucConfig_UpstrokeMaxRange;
                    stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_VCM_STALL_RANGE |
                      DEST_HMI | SOURCE_SCU;
                    fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
                    printf("\n\rVCM_STALL_UPSTROKE_MAX_RANGE CONFIG_PARA_101= %x\r\n", stTx1Message.ExtId);
                    printf("\n\rstConfigData.ucConfig_UpstrokeMaxRange = %d\r\n", stConfigData.ucConfig_UpstrokeMaxRange);
#endif
                    stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_VCM_STALL_RANGE |
                      DEST_TABLET | SOURCE_SCU;	
                    //	stWifi.ucSendFlag = SET;
                    //	stEthernet.ucSendFlag = SET;
                    fnXML_FIFO(&stTx1Message);
                    /****************************************************************/	
                    /*VCM_STALL_DOWNSTROKE_UPPER_LIMIT CONFIG_PARA_102*/
                    stTx1Message.DLC = TWO_BYTES;
                    stTx1Message.Data[DATA_BYTE_ZERO] = BIT3_SET ;
                    stTx1Message.Data[DATA_BYTE_ONE] = stConfigData.ucConfig_DownstrokeUpLimit;
                    stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_VCM_STALL_RANGE |
                      DEST_HMI | SOURCE_SCU;
                    fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
                    printf("\n\rVCM_STALL_DOWNSTROKE_UPPER_LIMIT CONFIG_PARA_102= %x\r\n", stTx1Message.ExtId);
                    printf("\n\rstConfigData.ucConfig_DownstrokeUpLimit = %d\r\n", stConfigData.ucConfig_DownstrokeUpLimit);
#endif
                    stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_VCM_STALL_RANGE |
                      DEST_TABLET | SOURCE_SCU;	
                    //	stWifi.ucSendFlag = SET;
                    //	stEthernet.ucSendFlag = SET;
                    fnXML_FIFO(&stTx1Message);
                    /****************************************************************/	
                    /*VCM_STALL_DOWNSTROKE_MIN_RANGE CONFIG_PARA_103*/
                    stTx1Message.DLC = TWO_BYTES;
                    stTx1Message.Data[DATA_BYTE_ZERO] = BIT4_SET ;
                    stTx1Message.Data[DATA_BYTE_ONE] = stConfigData.ucConfig_DownstrokeMinRange;
                    stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_VCM_STALL_RANGE |
                      DEST_HMI | SOURCE_SCU;
                    fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
                    printf("\n\rVCM_STALL_DOWNSTROKE_MIN_RANGE CONFIG_PARA_103= %x\r\n", stTx1Message.ExtId);
                    printf("\n\rstConfigData.ucConfig_DownstrokeMinRange = %d\r\n", stConfigData.ucConfig_DownstrokeMinRange);
#endif
                    stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_VCM_STALL_RANGE |
                      DEST_TABLET | SOURCE_SCU;	
                    //	stWifi.ucSendFlag = SET;
                    //	stEthernet.ucSendFlag = SET;
                    fnXML_FIFO(&stTx1Message);
                    /****************************************************************/	
                    /*VCM_STALL_DOWNSTROKE_MAX_RANGE CONFIG_PARA_104*/
                    stTx1Message.DLC = TWO_BYTES;
                    stTx1Message.Data[DATA_BYTE_ZERO] = BIT5_SET ;
                    stTx1Message.Data[DATA_BYTE_ONE] = stConfigData.ucConfig_DownstrokeMaxRange;
                    stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_VCM_STALL_RANGE |
                      DEST_HMI | SOURCE_SCU;
                    fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
                    printf("\n\rVCM_STALL_DOWNSTROKE_MAX_RANGE CONFIG_PARA_104= %x\r\n", stTx1Message.ExtId);
                    printf("\n\rstConfigData.ucConfig_DownstrokeMaxRange = %d\r\n", stConfigData.ucConfig_DownstrokeMaxRange);
#endif
                    stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_VCM_STALL_RANGE |
                      DEST_TABLET | SOURCE_SCU;	
                    //	stWifi.ucSendFlag = SET;
                    //	stEthernet.ucSendFlag = SET;
                    fnXML_FIFO(&stTx1Message);
                    /****************************************************************/	
                    
                    
                    /***********************change30 message change to Config*************/
                    /*SCU_WIFI_IPADDRESS CONFIG_PARA_105*/
                    stTx1Message.DLC = EIGHT_BYTES;
                    stTx1Message.Data[DATA_BYTE_ZERO] = BIT1_SET ;	//WIFI
                    stTx1Message.Data[DATA_BYTE_ONE] = BIT0_SET ;		//IP
                    
                    strcpy((char *)rgucIPADDRESSBuff,(char *)stWifiIP.rgucIP_Addr);
                    stTx1Message.Data[DATA_BYTE_TWO] = atoi(strtok((char *)rgucIPADDRESSBuff,"."));
                    stTx1Message.Data[DATA_BYTE_THREE] = atoi(strtok(NULL,"."));
                    stTx1Message.Data[DATA_BYTE_FOUR] = atoi(strtok(NULL,"."));
                    stTx1Message.Data[DATA_BYTE_FIVE] = atoi(strtok(NULL,"."));
#ifdef DBG_CONFIG_SEND_HMI	
                    printf("\n\rWIFI IP %d.%d.%d.%d",stTx1Message.Data[DATA_BYTE_TWO],
                           stTx1Message.Data[DATA_BYTE_THREE],stTx1Message.Data[DATA_BYTE_FOUR],stTx1Message.Data[DATA_BYTE_FIVE]);
#endif	
                    
                    stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_ETH_WIFI_IP_SN_GW_PN |
                      DEST_HMI | SOURCE_SCU;
                    fnCANMsg_Transmit(&stTx1Message, CAN1);
                    
#ifdef DBG_CONFIG_SEND_HMI
                    printf("\n\r SCU_WIFI_IPADDRESS CONFIG_PARA_105= %x\r\n", stTx1Message.ExtId);
#endif
                    
                    		stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_ETH_WIFI_IP_SN_GW_PN |
                    										 DEST_TABLET | SOURCE_SCU;	
                    //	stWifi.ucSendFlag = SET;
                    //	stEthernet.ucSendFlag = SET;
                    	fnXML_FIFO(&stTx1Message);
                    
                    /***********************change30 message change to Config*************/
                    /*SCU_WIFI_SMADDRESS CONFIG_PARA_106*/
                    stTx1Message.DLC = EIGHT_BYTES;
                    stTx1Message.Data[DATA_BYTE_ZERO] = BIT1_SET ;	//WIFI
                    stTx1Message.Data[DATA_BYTE_ONE] = BIT1_SET ;		//SUBNET MASK
                    
                    strcpy((char *)rgucIPADDRESSBuff,(char *)stWifiIP.rgucSubnet_Addr);
                    stTx1Message.Data[DATA_BYTE_TWO] = atoi(strtok((char *)rgucIPADDRESSBuff,"."));
                    stTx1Message.Data[DATA_BYTE_THREE] = atoi(strtok(NULL,"."));
                    stTx1Message.Data[DATA_BYTE_FOUR] = atoi(strtok(NULL,"."));
                    stTx1Message.Data[DATA_BYTE_FIVE] = atoi(strtok(NULL,"."));
#ifdef DBG_CONFIG_SEND_HMI	
                    printf("\n\rWIFI SUBNET MASK %d.%d.%d.%d",stTx1Message.Data[DATA_BYTE_TWO],
                           stTx1Message.Data[DATA_BYTE_THREE],stTx1Message.Data[DATA_BYTE_FOUR],stTx1Message.Data[DATA_BYTE_FIVE]);
#endif	
                    
                    stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_ETH_WIFI_IP_SN_GW_PN |
                      DEST_HMI | SOURCE_SCU;
                    fnCANMsg_Transmit(&stTx1Message, CAN1);
                    
#ifdef DBG_CONFIG_SEND_HMI
                    printf("\n\r SCU_WIFI_SMADDRESS CONFIG_PARA_106= %x\r\n", stTx1Message.ExtId);
#endif
                    
                    		stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_ETH_WIFI_IP_SN_GW_PN |
                    										 DEST_TABLET | SOURCE_SCU;	
                    //	stWifi.ucSendFlag = SET;
                    //	stEthernet.ucSendFlag = SET;
                    	fnXML_FIFO(&stTx1Message);
                    
                    /***********************change30 message change to Config*************/
                    /*SCU_WIFI_GWADDRESS CONFIG_PARA_107*/
                    stTx1Message.DLC = EIGHT_BYTES;
                    stTx1Message.Data[DATA_BYTE_ZERO] = BIT1_SET ;	//WIFI
                    stTx1Message.Data[DATA_BYTE_ONE] = BIT2_SET ;		//GATEWAY
                    
                    strcpy((char *)rgucIPADDRESSBuff,(char *)stWifiIP.rgucGateway_Addr);
                    stTx1Message.Data[DATA_BYTE_TWO] = atoi(strtok((char *)rgucIPADDRESSBuff,"."));
                    stTx1Message.Data[DATA_BYTE_THREE] = atoi(strtok(NULL,"."));
                    stTx1Message.Data[DATA_BYTE_FOUR] = atoi(strtok(NULL,"."));
                    stTx1Message.Data[DATA_BYTE_FIVE] = atoi(strtok(NULL,"."));
#ifdef DBG_CONFIG_SEND_HMI	
                    printf("\n\rWIFI GATEWAY %d.%d.%d.%d",stTx1Message.Data[DATA_BYTE_TWO],
                           stTx1Message.Data[DATA_BYTE_THREE],stTx1Message.Data[DATA_BYTE_FOUR],stTx1Message.Data[DATA_BYTE_FIVE]);
#endif	
                    
                    stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_ETH_WIFI_IP_SN_GW_PN |
                      DEST_HMI | SOURCE_SCU;
                    fnCANMsg_Transmit(&stTx1Message, CAN1);
                    
#ifdef DBG_CONFIG_SEND_HMI
                    printf("\n\r SCU_WIFI_GWADDRESS CONFIG_PARA_107= %x\r\n", stTx1Message.ExtId);
#endif
                    
                    		stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_ETH_WIFI_IP_SN_GW_PN |
                    										 DEST_TABLET | SOURCE_SCU;	
                    //	stWifi.ucSendFlag = SET;
                    //	stEthernet.ucSendFlag = SET;
                    	fnXML_FIFO(&stTx1Message);
                    
                    
                    
                    /***********************change30 message change to Config*************/
                    /*SCU_WIFI_PORTADDRESS CONFIG_PARA_108*/
                    stTx1Message.DLC = EIGHT_BYTES;
                    stTx1Message.Data[DATA_BYTE_ZERO] = BIT1_SET ;	//WIFI
                    stTx1Message.Data[DATA_BYTE_ONE] = BIT3_SET ;		//PORT NUMBER
                    SplitByte.iData = stWifiIP.uiPort;
                    stTx1Message.Data[DATA_BYTE_SIX] = SplitByte.rgucData[DATA_BYTE_ZERO];
                    stTx1Message.Data[DATA_BYTE_SEVEN] = SplitByte.rgucData[DATA_BYTE_ONE];
                    
                    
#ifdef DBG_CONFIG_SEND_HMI	
                    printf("\n\rWIFI PORT NUMBER %d",stEthernetIP.uiPort);
#endif	
                    
                    stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_ETH_WIFI_IP_SN_GW_PN |
                      DEST_HMI | SOURCE_SCU;
                    fnCANMsg_Transmit(&stTx1Message, CAN1);
                    
#ifdef DBG_CONFIG_SEND_HMI
                    printf("\n\r SCU_WIFI_PORTADDRESS CONFIG_PARA_108= %x\r\n", stTx1Message.ExtId);
#endif
                    
                    		stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_ETH_WIFI_IP_SN_GW_PN |
                    										 DEST_TABLET | SOURCE_SCU;	
                    //	stWifi.ucSendFlag = SET;
                    //	stEthernet.ucSendFlag = SET;
                    	fnXML_FIFO(&stTx1Message);
                    
                    
                    
                    /****************************************************************/		
                    /*SCU_WIFI_SSIDADDRESS CONFIG_PARA_109*/
                    stTx1Message.DLC = EIGHT_BYTES;
                    stTx1Message.Data[DATA_BYTE_ZERO] = BIT0_SET ;
                    
                    /**************************DATA***********************************/
                    for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                    {
                      stTx1Message.Data[ucLoopCnt]	 = stConfigData.rgucConfig_WifiSSID1[ucLoopCnt - 1];
                    }
                    /****************************************************************/
                    stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_WIFI_SSID |
                      DEST_HMI | SOURCE_SCU;
                    fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
                    printf("\n\r SCU_WIFI_SSIDADDRESS CONFIG_PARA_109= %x\r\n", stTx1Message.ExtId);
#endif
                    	stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_WIFI_SSID |
                    										 DEST_TABLET | SOURCE_SCU;	
                    //	stWifi.ucSendFlag = SET;
                    //	stEthernet.ucSendFlag = SET;
                    	fnXML_FIFO(&stTx1Message);
                    /****************************************************************/		
                    /*SCU_WIFI_SSIDADDRESS CONFIG_PARA_109*/
                    stTx1Message.DLC = EIGHT_BYTES;
                    stTx1Message.Data[DATA_BYTE_ZERO] = BIT1_SET ;
                    
                    /**************************DATA***********************************/
                    for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                    {
                      stTx1Message.Data[ucLoopCnt]	 = stConfigData.rgucConfig_WifiSSID2[ucLoopCnt - 1];
                    }
                    /****************************************************************/
                    stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_WIFI_SSID |
                      DEST_HMI | SOURCE_SCU;
                    fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
                    printf("\n\r SCU_WIFI_SSIDADDRESS CONFIG_PARA_109= %x\r\n", stTx1Message.ExtId);
#endif
                    	stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_WIFI_SSID |
                    											 DEST_TABLET | SOURCE_SCU;	
                    ////	stWifi.ucSendFlag = SET;
                    ////	stEthernet.ucSendFlag = SET;
                   fnXML_FIFO(&stTx1Message);
                    /****************************************************************/		
                    
                    
                    /****************************************************************/		
                    /*SCU_WIFI_PASSADDRESS CONFIG_PARA_110*/
                    stTx1Message.DLC = EIGHT_BYTES;
                    stTx1Message.Data[DATA_BYTE_ZERO] = BIT0_SET ;
                    
                    /**************************DATA***********************************/
                    for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                    {
                      stTx1Message.Data[ucLoopCnt]	 = stConfigData.rgucConfig_WifiPASS1[ucLoopCnt - 1];
                    }
                    /****************************************************************/
                    stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_WIFI_PASS |
                      DEST_HMI | SOURCE_SCU;
                    fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
                    printf("\n\r SCU_WIFI_PASSADDRESS CONFIG_PARA_110= %x\r\n", stTx1Message.ExtId);
#endif
                    stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_WIFI_PASS |
                    											 DEST_TABLET | SOURCE_SCU;	
                    ////	stWifi.ucSendFlag = SET;
                    ////	stEthernet.ucSendFlag = SET;
                    	fnXML_FIFO(&stTx1Message);
                    /****************************************************************/		
                    /*SCU_WIFI_PASSADDRESS CONFIG_PARA_110*/
                    stTx1Message.DLC = EIGHT_BYTES;
                    stTx1Message.Data[DATA_BYTE_ZERO] = BIT1_SET ;
                    
                    /**************************DATA***********************************/
                    for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                    {
                      stTx1Message.Data[ucLoopCnt]	 = stConfigData.rgucConfig_WifiPASS2[ucLoopCnt - 1];
                    }
                    /****************************************************************/
                    stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_WIFI_PASS |
                      DEST_HMI | SOURCE_SCU;
                    fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
                    printf("\n\r SCU_WIFI_PASSADDRESS SCU_WIFI_PASSADDRESS= %x\r\n", stTx1Message.ExtId);
#endif
                    stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_WIFI_PASS |
                    											 DEST_TABLET | SOURCE_SCU;	
                    ////	stWifi.ucSendFlag = SET;
                    ////	stEthernet.ucSendFlag = SET;
                   	fnXML_FIFO(&stTx1Message);
                    /************************change20*************************************/		
                    /***********************change30 message change to Config*************/
                    /*SCU_ETH_IPADDRESS CONFIG_PARA_90*/
                    stTx1Message.DLC = EIGHT_BYTES;
                    stTx1Message.Data[DATA_BYTE_ZERO] = BIT0_SET ;	//ETHERNET
                    stTx1Message.Data[DATA_BYTE_ONE] = BIT0_SET ;		//IP
                    
                    strcpy((char *)rgucIPADDRESSBuff,(char *)stEthernetIP.rgucIP_Addr);
                    stTx1Message.Data[DATA_BYTE_TWO] = atoi(strtok((char *)rgucIPADDRESSBuff,"."));
                    stTx1Message.Data[DATA_BYTE_THREE] = atoi(strtok(NULL,"."));
                    stTx1Message.Data[DATA_BYTE_FOUR] = atoi(strtok(NULL,"."));
                    stTx1Message.Data[DATA_BYTE_FIVE] = atoi(strtok(NULL,"."));
#ifdef DBG_CONFIG_SEND_HMI	
                    printf("\n\rETH IP %d.%d.%d.%d",stTx1Message.Data[DATA_BYTE_TWO],
                           stTx1Message.Data[DATA_BYTE_THREE],stTx1Message.Data[DATA_BYTE_FOUR],stTx1Message.Data[DATA_BYTE_FIVE]);
#endif	
                    
                    stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_ETH_WIFI_IP_SN_GW_PN |
                      DEST_HMI | SOURCE_SCU;
                    fnCANMsg_Transmit(&stTx1Message, CAN1);
                    
#ifdef DBG_CONFIG_SEND_HMI
                    printf("\n\r SCU_WIFI_IPADDRESS CONFIG_PARA_90= %x\r\n", stTx1Message.ExtId);
#endif
                    
                    		stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_ETH_WIFI_IP_SN_GW_PN |
                  										 DEST_TABLET | SOURCE_SCU;	
                    //	stWifi.ucSendFlag = SET;
                    //	stEthernet.ucSendFlag = SET;
                   	fnXML_FIFO(&stTx1Message);
                    
                    /***********************change30 message change to Config*************/
                    /*SCU_ETH_SMADDRESS CONFIG_PARA_91*/
                    stTx1Message.DLC = EIGHT_BYTES;
                    stTx1Message.Data[DATA_BYTE_ZERO] = BIT0_SET ;	//ETHERNET
                    stTx1Message.Data[DATA_BYTE_ONE] = BIT1_SET ;		//SUBNET MASK
                    
                    strcpy((char *)rgucIPADDRESSBuff,(char *)stEthernetIP.rgucSubnet_Addr);
                    stTx1Message.Data[DATA_BYTE_TWO] = atoi(strtok((char *)rgucIPADDRESSBuff,"."));
                    stTx1Message.Data[DATA_BYTE_THREE] = atoi(strtok(NULL,"."));
                    stTx1Message.Data[DATA_BYTE_FOUR] = atoi(strtok(NULL,"."));
                    stTx1Message.Data[DATA_BYTE_FIVE] = atoi(strtok(NULL,"."));
#ifdef DBG_CONFIG_SEND_HMI	
                    printf("\n\rETH SUBNET MASK %d.%d.%d.%d",stTx1Message.Data[DATA_BYTE_TWO],
                           stTx1Message.Data[DATA_BYTE_THREE],stTx1Message.Data[DATA_BYTE_FOUR],stTx1Message.Data[DATA_BYTE_FIVE]);
#endif	
                    
                    stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_ETH_WIFI_IP_SN_GW_PN |
                      DEST_HMI | SOURCE_SCU;
                    fnCANMsg_Transmit(&stTx1Message, CAN1);
                    
#ifdef DBG_CONFIG_SEND_HMI
                    printf("\n\r SCU_ETH_SMADDRESS CONFIG_PARA_91= %x\r\n", stTx1Message.ExtId);
#endif
                    
                    		stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_ETH_WIFI_IP_SN_GW_PN |
                    										 DEST_TABLET | SOURCE_SCU;	
                    //	stWifi.ucSendFlag = SET;
                    //	stEthernet.ucSendFlag = SET;
                    	fnXML_FIFO(&stTx1Message);
                    
                    /***********************change30 message change to Config*************/
                    /*SCU_ETH_GWADDRESS CONFIG_PARA_92*/
                    stTx1Message.DLC = EIGHT_BYTES;
                    stTx1Message.Data[DATA_BYTE_ZERO] = BIT0_SET ;	//ETHERNET
                    stTx1Message.Data[DATA_BYTE_ONE] = BIT2_SET ;		//GATEWAY
                    
                    strcpy((char *)rgucIPADDRESSBuff,(char *)stEthernetIP.rgucGateway_Addr);
                    stTx1Message.Data[DATA_BYTE_TWO] = atoi(strtok((char *)rgucIPADDRESSBuff,"."));
                    stTx1Message.Data[DATA_BYTE_THREE] = atoi(strtok(NULL,"."));
                    stTx1Message.Data[DATA_BYTE_FOUR] = atoi(strtok(NULL,"."));
                    stTx1Message.Data[DATA_BYTE_FIVE] = atoi(strtok(NULL,"."));
#ifdef DBG_CONFIG_SEND_HMI	
                    printf("\n\rETH GATEWAY %d.%d.%d.%d",stTx1Message.Data[DATA_BYTE_TWO],
                           stTx1Message.Data[DATA_BYTE_THREE],stTx1Message.Data[DATA_BYTE_FOUR],stTx1Message.Data[DATA_BYTE_FIVE]);
#endif	
                    
                    stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_ETH_WIFI_IP_SN_GW_PN |
                      DEST_HMI | SOURCE_SCU;
                    fnCANMsg_Transmit(&stTx1Message, CAN1);
                    
#ifdef DBG_CONFIG_SEND_HMI
                    printf("\n\r SCU_ETH_GWADDRESS CONFIG_PARA_92= %x\r\n", stTx1Message.ExtId);
#endif
                    
                    		stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_ETH_WIFI_IP_SN_GW_PN |
                   										 DEST_TABLET | SOURCE_SCU;	
                    //	stWifi.ucSendFlag = SET;
                    //	stEthernet.ucSendFlag = SET;
                  	fnXML_FIFO(&stTx1Message);
                    
                    /***********************change30 message change to Config*************/
                    /*SCU_ETH_PORTADDRESS CONFIG_PARA_93*/
                    stTx1Message.DLC = EIGHT_BYTES;
                    stTx1Message.Data[DATA_BYTE_ZERO] = BIT0_SET ;	//ETHERNET
                    stTx1Message.Data[DATA_BYTE_ONE] = BIT3_SET ;		//PORT NUMBER
                    SplitByte.iData = stEthernetIP.uiPort;
                    stTx1Message.Data[DATA_BYTE_SIX] = SplitByte.rgucData[DATA_BYTE_ZERO];
                    stTx1Message.Data[DATA_BYTE_SEVEN] = SplitByte.rgucData[DATA_BYTE_ONE];
                    
                    
#ifdef DBG_CONFIG_SEND_HMI	
                    printf("\n\rETH PORT NUMBER %d",stEthernetIP.uiPort);
#endif	
                    
                    stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_ETH_WIFI_IP_SN_GW_PN |
                      DEST_HMI | SOURCE_SCU;
                    fnCANMsg_Transmit(&stTx1Message, CAN1);
                    
#ifdef DBG_CONFIG_SEND_HMI
                    printf("\n\r SCU_ETH_PORTADDRESS CONFIG_PARA_93= %x\r\n", stTx1Message.ExtId);
#endif
                    
                   		stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_ETH_WIFI_IP_SN_GW_PN |
                    										 DEST_TABLET | SOURCE_SCU;	
                    //	stWifi.ucSendFlag = SET;
                    //	stEthernet.ucSendFlag = SET;
                    	fnXML_FIFO(&stTx1Message);
                    
                    /*change21*/
                    /****************************************************************/		
                    /* IDLE_DATA_LOGGING CONFIG_PARA_114*/
                    stTx1Message.DLC = FOUR_BYTES;
                    
                    SplitByte.fData = stConfigData.fConfig_IdleTimePeriodicLog;
                    
                    
                    
                    /**************************DATA***********************************/
					for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                    {
                      stTx1Message.Data[ucLoopCnt]= SplitByte.rgucData[ucLoopCnt] ;
                    }
					/****************************************************************/		
                    stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_IDLE_DATA_LOG_PERIOD |
                      DEST_HMI | SOURCE_SCU;
                    fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
                    printf("\n\IDLE_DATA_LOGGING CONFIG_PARA_114= %x\r\n", stTx1Message.ExtId);
#endif
                    stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_IDLE_DATA_LOG_PERIOD |
                      DEST_TABLET | SOURCE_SCU;	
                    //	stWifi.ucSendFlag = SET;
                    //	stEthernet.ucSendFlag = SET;
                    fnXML_FIFO(&stTx1Message);
                    
                    
                    /****************************************************************/		
                    /*JOB_NAME1 CONFIG_PARA_111*/
                    stTx1Message.DLC = EIGHT_BYTES;
                    stTx1Message.Data[DATA_BYTE_ZERO] = BIT0_SET ;
                    
                    /**************************DATA***********************************/
                    for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                    {
                      stTx1Message.Data[ucLoopCnt]	 = stConfigData.rgucConfig_JobName1[ucLoopCnt - 1];
                    }
                    /****************************************************************/
                    stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_JOB_NAME |
                      DEST_HMI | SOURCE_SCU;
                    fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
                    printf("\n\rJOB_NAME1 CONFIG_PARA_111= %x\r\n", stTx1Message.ExtId);
#endif
                    stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_JOB_NAME |
                      DEST_TABLET | SOURCE_SCU;	
                    //	stWifi.ucSendFlag = SET;
                    //	stEthernet.ucSendFlag = SET;
                    fnXML_FIFO(&stTx1Message);
                    /****************************************************************/		
                    /*JOB_NAME2 CONFIG_PARA_112*/
                    stTx1Message.DLC = EIGHT_BYTES;
                    stTx1Message.Data[DATA_BYTE_ZERO] = BIT1_SET ;
                    
                    /**************************DATA***********************************/
                    for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                    {
                      stTx1Message.Data[ucLoopCnt]	 = stConfigData.rgucConfig_JobName2[ucLoopCnt - 1];
                    }
                    /****************************************************************/
                    stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_JOB_NAME |
                      DEST_HMI | SOURCE_SCU;
                    fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
                    printf("\n\r JOB_NAME2 CONFIG_PARA_112= %x\r\n", stTx1Message.ExtId);
#endif
                    stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_JOB_NAME |
                      DEST_TABLET | SOURCE_SCU;
                    //	stWifi.ucSendFlag = SET;
                    //	stEthernet.ucSendFlag = SET;
                    fnXML_FIFO(&stTx1Message);
                    /****************************************************************/		
                    /*JOB_NAME3 CONFIG_PARA_113*/
                    stTx1Message.DLC = EIGHT_BYTES;
                    stTx1Message.Data[DATA_BYTE_ZERO] = BIT2_SET ;
                    
                    /**************************DATA***********************************/
                    for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                    {
                      stTx1Message.Data[ucLoopCnt]	 = stConfigData.rgucConfig_JobName3[ucLoopCnt - 1];
                    }
                    /****************************************************************/
                    stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_JOB_NAME |
                      DEST_HMI | SOURCE_SCU;
                    fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
                    printf("\n\r JOB_NAME3 CONFIG_PARA_113= %x\r\n", stTx1Message.ExtId);
#endif
                    stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_JOB_NAME |
                      DEST_TABLET | SOURCE_SCU;	
                    //	stWifi.ucSendFlag = SET;
                    //	stEthernet.ucSendFlag = SET;
                    fnXML_FIFO(&stTx1Message);
                    
                    /****************************************************************/		
                    /* PIPE_DIAMETER CONFIG_PARA_115*/
                    stTx1Message.DLC = FOUR_BYTES;
                    
                    SplitByte.fData = stConfigData.fConfig_PipeDiameter;
                    
                    
                    
                    /**************************DATA***********************************/
					for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                    {
                      stTx1Message.Data[ucLoopCnt]= SplitByte.rgucData[ucLoopCnt] ;
                    }
					/****************************************************************/		
                    stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_PIPE_DIAMETER |
                      DEST_HMI | SOURCE_SCU;
                    fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
                    printf("\n\PIPE_DIAMETER CONFIG_PARA_115= %x\r\n", stTx1Message.ExtId);
#endif
                    stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_PIPE_DIAMETER |
                      DEST_TABLET | SOURCE_SCU;	
                    //	stWifi.ucSendFlag = SET;
                    //	stEthernet.ucSendFlag = SET;
                    fnXML_FIFO(&stTx1Message);
                    
                    
                    /****************************************************************/	
                    /* change37 */
                    /*HMI_TIME_OFFSET_SIGN CONFIG_PARA_116*/
                    stTx1Message.DLC = FOUR_BYTES;
                    stTx1Message.Data[DATA_BYTE_ZERO] = stConfigData.ucConfig_TimeOffsetSign ;
                    stTx1Message.Data[DATA_BYTE_ONE] = stConfigData.ucConfig_TimeOffsetHour ;
                    stTx1Message.Data[DATA_BYTE_TWO] = stConfigData.ucConfig_TimeOffsetMin ;
                    stTx1Message.Data[DATA_BYTE_THREE] = stConfigData.ucConfig_TimeOffsetSec ;
                    stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_TIME_ZONE_OFFSET |
                      DEST_HMI | SOURCE_SCU;
                    fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
                    printf("\n\r HMI_TIME_OFFSET_SIGN CONFIG_PARA_116 = %x\r\n", stTx1Message.ExtId);
#endif
                    
                    stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_TIME_ZONE_OFFSET |
                      DEST_TABLET | SOURCE_SCU;	
                    //	stWifi.ucSendFlag = SET;
                    //	stEthernet.ucSendFlag = SET;
                    fnXML_FIFO(&stTx1Message);
                    /****************************************************************/	
										
					/**********************change42******************************************/		
                /*JOB_ID1 CONFIG_PARA_117*/
                stTx1Message.DLC = EIGHT_BYTES;
                stTx1Message.Data[DATA_BYTE_ZERO] = BIT0_SET ;
                
                /**************************DATA***********************************/
                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                {
                  stTx1Message.Data[ucLoopCnt]	 = stConfigData.rgucConfig_JobId1[ucLoopCnt - 1];
                }
                /****************************************************************/
                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_JOB_ID |
                  DEST_HMI | SOURCE_SCU;
                fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
                printf("\n\r JOB_ID1 CONFIG_PARA_117= %x\r\n", stTx1Message.ExtId);
#endif
                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_JOB_ID |
                  DEST_TABLET | SOURCE_SCU;	
                //	stWifi.ucSendFlag = SET;
                //	stEthernet.ucSendFlag = SET;
                fnXML_FIFO(&stTx1Message);
                /****************************************************************/		
                /*JOB_ID2 CONFIG_PARA_118*/
                stTx1Message.DLC = EIGHT_BYTES;
                stTx1Message.Data[DATA_BYTE_ZERO] = BIT1_SET ;
                
                /**************************DATA***********************************/
                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                {
                  stTx1Message.Data[ucLoopCnt]	 = stConfigData.rgucConfig_JobId2[ucLoopCnt - 1];
                }
                /****************************************************************/
                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_JOB_ID |
                  DEST_HMI | SOURCE_SCU;
                fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
                printf("\n\r JOB_ID2 CONFIG_PARA_118= %x\r\n", stTx1Message.ExtId);
#endif
                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_JOB_ID |
                  DEST_TABLET | SOURCE_SCU;
                //	stWifi.ucSendFlag = SET;
                //	stEthernet.ucSendFlag = SET;
                fnXML_FIFO(&stTx1Message);
                /****************************************************************/		
                /*JOB_ID3 CONFIG_PARA_119*/
                stTx1Message.DLC = EIGHT_BYTES;
                stTx1Message.Data[DATA_BYTE_ZERO] = BIT2_SET ;
                
                /**************************DATA***********************************/
                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                {
                  stTx1Message.Data[ucLoopCnt]	 = stConfigData.rgucConfig_JobId3[ucLoopCnt - 1];
                }
                /****************************************************************/
                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_JOB_ID |
                  DEST_HMI | SOURCE_SCU;
                fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CONFIG_SEND_HMI
                printf("\n\r JOB_ID3 CONFIG_PARA_119= %x\r\n", stTx1Message.ExtId);
#endif
                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_JOB_ID |
                  DEST_TABLET | SOURCE_SCU;	
                //	stWifi.ucSendFlag = SET;
                //	stEthernet.ucSendFlag = SET;
                fnXML_FIFO(&stTx1Message);
								
							/*change43*/
 /**************************SEND CAN MESSAGE*******************/
              stTx1Message.DLC = FIVE_BYTES;
              SplitByte.fData = stCurrentData.fCurr_BaseMaterialPrimedAccumulate;
              stTx1Message.Data[DATA_BYTE_ZERO] = BIT2_SET ;
              /**************************DATA***********************************/
              for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
              {
                stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
              }
              /****************************************************************/
              stTx1Message.ExtId = MSG_TYPE_DATA | DATA_ACCUMULATED_MATERIAL_USED |
                DEST_HMI | SOURCE_SCU;
              fnCANMsg_Transmit(&stTx1Message, CAN1);
							
						
#ifdef DBG_CAN_PARSER_DATA
              printf("\n\DATA_ACCUMULATED_MATERIAL_USED = %x\r\n", stTx1Message.ExtId);
#endif
              stTx1Message.ExtId = MSG_TYPE_DATA | DATA_ACCUMULATED_MATERIAL_USED |
                DEST_TABLET | SOURCE_SCU;	
              //	stWifi.ucSendFlag = SET;
              //	stEthernet.ucSendFlag = SET;
              fnXML_FIFO(&stTx1Message);
              
              /**************************SEND CAN MESSAGE*******************/
              stTx1Message.DLC = FIVE_BYTES;
              SplitByte.fData = stCurrentData.fCurr_HardMaterialPrimedAccumulate;
              stTx1Message.Data[DATA_BYTE_ZERO] = BIT3_SET ;
              /**************************DATA***********************************/
              for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
              {
                stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
              }
              /****************************************************************/
              stTx1Message.ExtId = MSG_TYPE_DATA | DATA_ACCUMULATED_MATERIAL_USED |
                DEST_HMI | SOURCE_SCU;
              fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CAN_PARSER_DATA
              printf("\n\DATA_ACCUMULATED_MATERIAL_USED = %x\r\n", stTx1Message.ExtId);
#endif
              stTx1Message.ExtId = MSG_TYPE_DATA | DATA_ACCUMULATED_MATERIAL_USED |
                DEST_TABLET | SOURCE_SCU;	
              //	stWifi.ucSendFlag = SET;
              //	stEthernet.ucSendFlag = SET;
              fnXML_FIFO(&stTx1Message);
              /************************************************************/	
  
              stTx1Message.DLC = FIVE_BYTES;
              SplitByte.fData = stCurrentData.fCurr_BaseMaterialAppliedAccumulate;
              stTx1Message.Data[DATA_BYTE_ZERO] = BIT4_SET ;
              /**************************DATA***********************************/
              for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
              {
                stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
              }
              /****************************************************************/
              stTx1Message.ExtId = MSG_TYPE_DATA | DATA_ACCUMULATED_MATERIAL_USED |
                DEST_HMI | SOURCE_SCU;
              fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CAN_PARSER_DATA
              printf("\n\DATA_ACCUMULATED_MATERIAL_USED = %x\r\n", stTx1Message.ExtId);
#endif
              stTx1Message.ExtId = MSG_TYPE_DATA | DATA_ACCUMULATED_MATERIAL_USED |
                DEST_TABLET | SOURCE_SCU;	
              //	stWifi.ucSendFlag = SET;
              //	stEthernet.ucSendFlag = SET;
              fnXML_FIFO(&stTx1Message);
            
              /**************************SEND CAN MESSAGE*******************/
              stTx1Message.DLC = FIVE_BYTES;
              SplitByte.fData = stCurrentData.fCurr_HardMaterialAppliedAccumulate;
              stTx1Message.Data[DATA_BYTE_ZERO] = BIT5_SET ;
              /**************************DATA***********************************/
              for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
              {
                stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
              }
              /****************************************************************/
              stTx1Message.ExtId = MSG_TYPE_DATA | DATA_ACCUMULATED_MATERIAL_USED |
                DEST_HMI | SOURCE_SCU;
              fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CAN_PARSER_DATA
              printf("\n\DATA_ACCUMULATED_MATERIAL_USED = %x\r\n", stTx1Message.ExtId);
#endif
              stTx1Message.ExtId = MSG_TYPE_DATA | DATA_ACCUMULATED_MATERIAL_USED |
                DEST_TABLET | SOURCE_SCU;	
              //	stWifi.ucSendFlag = SET;
              //	stEthernet.ucSendFlag = SET;
              fnXML_FIFO(&stTx1Message);
              /************************************************************/	
            
              /**************************SEND CAN MESSAGE*******************/
              stTx1Message.DLC = FIVE_BYTES;
              SplitByte.fData = stCurrentData.fCurr_BaseTotalMaterialUsedAccumulate;
              stTx1Message.Data[DATA_BYTE_ZERO] = BIT6_SET ;
              /**************************DATA***********************************/
              for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
              {
                stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
              }
              /****************************************************************/
              stTx1Message.ExtId = MSG_TYPE_DATA | DATA_ACCUMULATED_MATERIAL_USED |
                DEST_HMI | SOURCE_SCU;
              fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CAN_PARSER_DATA
              printf("\n\DATA_ACCUMULATED_MATERIAL_USED = %x\r\n", stTx1Message.ExtId);
#endif
              stTx1Message.ExtId = MSG_TYPE_DATA | DATA_ACCUMULATED_MATERIAL_USED |
                DEST_TABLET | SOURCE_SCU;	
              //	stWifi.ucSendFlag = SET;
              //	stEthernet.ucSendFlag = SET;
              fnXML_FIFO(&stTx1Message);
             	
              /**************************SEND CAN MESSAGE*******************/
              stTx1Message.DLC = FIVE_BYTES;
              SplitByte.fData = stCurrentData.fCurr_HardTotalMaterialUsedAccumulate;
              stTx1Message.Data[DATA_BYTE_ZERO] = BIT7_SET ;
              /**************************DATA***********************************/
              for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
              {
                stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
              }
              /****************************************************************/
              stTx1Message.ExtId = MSG_TYPE_DATA | DATA_ACCUMULATED_MATERIAL_USED |
                DEST_HMI | SOURCE_SCU;
              fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CAN_PARSER_DATA
              printf("\n\DATA_ACCUMULATED_MATERIAL_USED = %x\r\n", stTx1Message.ExtId);
#endif
              stTx1Message.ExtId = MSG_TYPE_DATA | DATA_ACCUMULATED_MATERIAL_USED |
                DEST_TABLET | SOURCE_SCU;	
              //	stWifi.ucSendFlag = SET;
              //	stEthernet.ucSendFlag = SET;
              fnXML_FIFO(&stTx1Message);
           
            /**************************SEND CAN MESSAGE*******************/
            stTx1Message.DLC = FIVE_BYTES;
            SplitByte.fData = stCurrentData.fCurr_BaseTotalMaterialUsed;
            stTx1Message.Data[DATA_BYTE_ZERO] = BIT0_SET ;
            /**************************DATA***********************************/
            for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
            }
            /****************************************************************/
            stTx1Message.ExtId = MSG_TYPE_DATA | DATA_ACCUMULATED_MATERIAL_USED |
              DEST_HMI | SOURCE_SCU;
            fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CAN_PARSER_DATA
            printf("\n\DATA_ACCUMULATED_MATERIAL_USED = %x\r\n", stTx1Message.ExtId);
#endif
             stTx1Message.ExtId = MSG_TYPE_DATA | DATA_ACCUMULATED_MATERIAL_USED |
              DEST_TABLET | SOURCE_SCU;	
            //	stWifi.ucSendFlag = SET;
            //	stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
            /************************************************************/		
         
            /**************************SEND CAN MESSAGE*******************/
            stTx1Message.DLC = FIVE_BYTES;
            SplitByte.fData = stCurrentData.fCurr_HardTotalMaterialUsed;
            stTx1Message.Data[DATA_BYTE_ZERO] = BIT1_SET ;
            /**************************DATA***********************************/
            for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
            {
              stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
            }
            /****************************************************************/
            stTx1Message.ExtId = MSG_TYPE_DATA | DATA_ACCUMULATED_MATERIAL_USED |
              DEST_HMI | SOURCE_SCU;
            fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef DBG_CAN_PARSER_DATA
            printf("\n\DATA_ACCUMULATED_MATERIAL_USED = %x\r\n", stTx1Message.ExtId);
#endif
            stTx1Message.ExtId = MSG_TYPE_DATA | DATA_ACCUMULATED_MATERIAL_USED |
              DEST_TABLET | SOURCE_SCU;	
            //	stWifi.ucSendFlag = SET;
            //	stEthernet.ucSendFlag = SET;
            fnXML_FIFO(&stTx1Message);
            /************************************************************/		
            ucDeviceConfigSent |= (1<< HMI);
                    
}
                
                /*****************************************************************************
                **@Function		:		fnSendOPMM_Config
                **@Descriptions: 	This function is implemented for Send configuration parameters to 
                OPMM
                **@parameters	: 	None
                **@return			: 	None
                *****************************************************************************/
                void fnSendOPMM_Config(void)
                {
                  /****************************OPMM*********************************/
                  
                  /*OPMM_SPRAY_COUNT CONFIG_PARA_46*/
                  stTx1Message.DLC = ONE_BYTES;
                  
                  stTx1Message.Data[DATA_BYTE_ZERO]		= stConfigData.ucConfig_SprayCount;
                  
                  
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_SPRAY_COUNT |
                    DEST_OPMM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  
                  
                  /****************************************************************/		
                  /*OPMM_PRIME_TIME CONFIG_PARA_47*/
                  stTx1Message.DLC = FOUR_BYTES;
                  
                  SplitByte.fData = stConfigData.fConfig_PrimeTime;
                  /**************************DATA***********************************/
                  for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                  {
                    stTx1Message.Data[ucLoopCnt]= SplitByte.rgucData[ucLoopCnt] ;
                  }
                  /****************************************************************/		
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_PRIME_TIME |
                    DEST_OPMM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  
                  /****************************************************************/		
                  /*OPMM_GUN_DELAY_TIME CONFIG_PARA_48*/
                  stTx1Message.DLC = FOUR_BYTES;
                  
                  SplitByte.fData = stConfigData.fConfig_GunDelayTime;
                  /**************************DATA***********************************/
                  for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                  {
                    stTx1Message.Data[ucLoopCnt]= SplitByte.rgucData[ucLoopCnt] ;
                  }
                  /****************************************************************/		
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_OPMM_GUN_DELAY |
                    DEST_OPMM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  /****************************************************************/		
                  /*OPMM_SPRAY_OVERLAPTIME CONFIG_PARA_49*/
                  stTx1Message.DLC = FOUR_BYTES;
                  
                  SplitByte.fData = stConfigData.fConfig_SprayOverlapTime;
                  /**************************DATA***********************************/
                  for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                  {
                    stTx1Message.Data[ucLoopCnt]= SplitByte.rgucData[ucLoopCnt] ;
                  }
                  /****************************************************************/		
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_OPMM_SPRAY_OVERLAP_TIME |
                    DEST_OPMM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  /****************************************************************/		
                  /*OPMM_REVERSE_DELAYTIME CONFIG_PARA_50*/
                  stTx1Message.DLC = FOUR_BYTES;
                  
                  SplitByte.fData = stConfigData.fConfig_ReverseDelayTime;
                  /**************************DATA***********************************/
                  for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                  {
                    stTx1Message.Data[ucLoopCnt]= SplitByte.rgucData[ucLoopCnt] ;
                  }
                  /****************************************************************/		
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_OPMM_REVERSE_DELAY |
                    DEST_OPMM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  /****************************************************************/		
                  /*OPMM_FLUSH_TIME CONFIG_PARA_51*/
                  stTx1Message.DLC = FOUR_BYTES;
                  
                  SplitByte.fData = stConfigData.fConfig_FlushTime;
                  /**************************DATA***********************************/
                  for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                  {
                    stTx1Message.Data[ucLoopCnt]= SplitByte.rgucData[ucLoopCnt] ;
                  }
                  /****************************************************************/		
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_OPMM_FLUSH_TIME |
                    DEST_OPMM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  /****************************************************************/		
                  /*OPMM_DCLS_HOME_REACH_TIME CONFIG_PARA_52*/
                  stTx1Message.DLC = FIVE_BYTES;
                  
                  SplitByte.fData = stConfigData.fConfig_DCLSTime;
                  
                  stTx1Message.Data[DATA_BYTE_ZERO] = BIT0_SET ;
                  
                  /**************************DATA***********************************/
                  for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                  {
                    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
                  }
                  /****************************************************************/
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_DCLS_HOME_TIME |
                    DEST_OPMM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  /****************************************************************/		
                  /*OPMM_HOME_DCLS_REACH_TIME CONFIG_PARA_53*/
                  stTx1Message.DLC = FIVE_BYTES;
                  
                  SplitByte.fData = stConfigData.fConfig_HOMETime;
                  
                  stTx1Message.Data[DATA_BYTE_ZERO] = BIT1_SET ;
                  
                  /**************************DATA***********************************/
                  for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                  {
                    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
                  }
                  /****************************************************************/
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_DCLS_HOME_TIME |
                    DEST_OPMM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  /****************************************************************/		
                  /*OPMM_GUN_POSITION_CHANGE_TIME CONFIG_PARA_54*/
                  stTx1Message.DLC = FOUR_BYTES;
                  
                  SplitByte.fData = stConfigData.fConfig_GunPosChangeTime;
                  
                  
                  
                  /**************************DATA***********************************/
                  for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                  {
                    stTx1Message.Data[ucLoopCnt]= SplitByte.rgucData[ucLoopCnt] ;
                  }
                  /****************************************************************/		
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_GUN_POSITION_CHANGE_TIME |
                    DEST_OPMM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  /****************************************************************/		
                  /*OPMM_DELAY_SPRAY_VALVE_FLUSH_VBAT_TIME CONFIG_PARA_55*/
                  stTx1Message.DLC = FOUR_BYTES;
                  
                  SplitByte.fData = stConfigData.fConfig_TimebeforeONSprayValve;
                  
                  
                  
                  /**************************DATA***********************************/
                  for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                  {
                    stTx1Message.Data[ucLoopCnt]= SplitByte.rgucData[ucLoopCnt] ;
                  }
                  /****************************************************************/		
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_SPRAY_VALVE_OPEN_TIME_VBAT |
                    DEST_OPMM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  /****************************************************************/		
                  /*OPMM_FLUSH_WARNNING_TIME CONFIG_PARA_56*/
                  stTx1Message.DLC = FOUR_BYTES;
                  
                  SplitByte.fData = stConfigData.fConfig_FlushWarnTime;
                  
                  
                  
                  /**************************DATA***********************************/
                  for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                  {
                    stTx1Message.Data[ucLoopCnt]= SplitByte.rgucData[ucLoopCnt] ;
                  }
                  /****************************************************************/		
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_FLUSH_WARNNING_TIME |
                    DEST_OPMM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  /****************************************************************/		
                  /*OPMM_SPRAY_VALVE_CONN,
                  OPMM_GUNPOS_VALVE_CONN,
                  OPMM_AB_VALVE_CONN,
                  OPMM_SOLVENT_VALVE_CONN,
                  OPMM_FORWARD_VALVE_CONN,
                  OPMM_BACKWARD_VALVE_CONN,
                  CONFIG_PARA_57*/
                  stTx1Message.DLC = SIX_BYTES;
                  
                  stTx1Message.Data[DATA_BYTE_ZERO] 	= stConfigData.ucConfig_ConnSprayvalve;
                  stTx1Message.Data[DATA_BYTE_ONE] 		= stConfigData.ucConfig_ConnGunposition;
                  stTx1Message.Data[DATA_BYTE_TWO] 		= stConfigData.ucConfig_ConnABvalve;
                  stTx1Message.Data[DATA_BYTE_THREE] 	= stConfigData.ucConfig_ConnSolventvalve;
                  stTx1Message.Data[DATA_BYTE_FOUR] 	= stConfigData.ucConfig_ConnForwardvalve;
                  stTx1Message.Data[DATA_BYTE_FIVE] 	= stConfigData.ucConfig_ConnBackwardvalve;
                  
                  
                  
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_OPMM_VALVE_REMAP |
                    DEST_OPMM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  /****************************************************************/	
                  /* OPMM_NON_CONTACT12_CONN,OPMM_NON_CONTACT3_CONN,OPMM_NON_CONTACT6_CONN
                  OPMM_NON_CONTACT9_CONN,
                  CONFIG_PARA_89 */
                  stTx1Message.DLC = FOUR_BYTES;
                  
                  stTx1Message.Data[DATA_BYTE_ZERO] 	= stConfigData.ucConfig_ConnNonContactat12;
                  stTx1Message.Data[DATA_BYTE_ONE] 		= stConfigData.ucConfig_ConnNonContactat3;
                  stTx1Message.Data[DATA_BYTE_TWO] 		= stConfigData.ucConfig_ConnNonContactat6;
                  stTx1Message.Data[DATA_BYTE_THREE] 	= stConfigData.ucConfig_ConnNonContactat9;
                  
                  
                  
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_OPMM_NON_CONTACT_REMAP |
                    DEST_OPMM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  
                  /****************************************************************/		
                  
                  /*OPMM_BYPASS CONFIG_PARA_58*/
                  stTx1Message.DLC = TWO_BYTES;
                  
                  SplitByte.iData 	= stConfigData.uiConfig_OPMMBypass;
                  
                  /**************************DATA***********************************/
                  for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                  {
                    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt];
                  }
                  /****************************************************************/
                  
                  
                  
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_OPMM_PERIPHRAL_BYPASS |
                    DEST_OPMM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  /****************************************************************/	
                  /*CONFIG_OPMM_BUZZER_VOLUME CONFIG_PARA_94*/
                  stTx1Message.DLC = ONE_BYTES;
                  stTx1Message.Data[DATA_BYTE_ZERO] = stConfigData.ucConfig_BuzzerVolume  ;
                  
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_OPMM_BUZZER_VOLUME |
                    DEST_OPMM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  
                  
                  /****************************************************************/		
                  /***************************change8******************************/		
                  /*OPMM_STOP_HLS_TIMER CONFIG_PARA_97*/
                  stTx1Message.DLC = FOUR_BYTES;
                  
                  SplitByte.fData = stConfigData.fConfig_TimeStopOPMMcommand;
                  
                  
                  
                  /**************************DATA***********************************/
                  for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                  {
                    stTx1Message.Data[ucLoopCnt]= SplitByte.rgucData[ucLoopCnt] ;
                  }
                  /****************************************************************/		
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_OPMM_STOP_HLS_TIME |
                    DEST_OPMM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  
                  /****************************************************************/		
                  /*OPMM_BATT_KNEE_VOLT CONFIG_PARA_98*/
                  stTx1Message.DLC = FOUR_BYTES;
                  
                  SplitByte.fData = stConfigData.fConfig_BatteryKneeVoltage;
                  
                  
                  
                  /**************************DATA***********************************/
                  for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                  {
                    stTx1Message.Data[ucLoopCnt]= SplitByte.rgucData[ucLoopCnt] ;
                  }
                  /****************************************************************/		
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_OPMM_BATT_KNEE_VOLT |
                    DEST_OPMM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  /****************************************************************/		
                  
                  ucDeviceConfigSent |= (1<< OPMM);
                }
                
                /*****************************************************************************
                **@Function		:		fnSendVCM_Config
                **@Descriptions: 	This function is implemented for Send configuration parameters to 
                VCM
                **@parameters	: 	None
                **@return			: 	None
                *****************************************************************************/
                void fnSendVCM_Config(void)
                {
                  /*VCM_UPSTROKE CONFIG_PARA_1*/
                  stTx1Message.DLC = THREE_BYTES;
                  
                  SplitByte.iData = stConfigData.iConfig_UpStrokeTH;
                  
                  stTx1Message.Data[DATA_BYTE_ZERO] = BIT0_SET ;
                  stTx1Message.Data[DATA_BYTE_ONE] = SplitByte.rgucData[DATA_BYTE_ZERO];
                  stTx1Message.Data[DATA_BYTE_TWO] = SplitByte.rgucData[DATA_BYTE_ONE];
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_VCM_STALL_TH |
                    DEST_VCM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  /****************************************************************/	
                  
                  
                  /*VCM_DOWNSTROKE CONFIG_PARA_2*/
                  stTx1Message.DLC = THREE_BYTES;
                  
                  SplitByte.iData = stConfigData.iConfig_DownStrokeTH;
                  
                  stTx1Message.Data[DATA_BYTE_ZERO] = BIT1_SET ;
                  stTx1Message.Data[DATA_BYTE_ONE] = SplitByte.rgucData[DATA_BYTE_ZERO];
                  stTx1Message.Data[DATA_BYTE_TWO] = SplitByte.rgucData[DATA_BYTE_ONE];
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_VCM_STALL_TH |
                    DEST_VCM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  /**********************************************************************/
                  
                  /*VCM_PRESSURE_LOWTH1 CONFIG_PARA_3*/
                  stTx1Message.DLC = FIVE_BYTES;
                  
                  SplitByte.fData = stConfigData.fConfig_PressureGauge1LTH;
                  
                  stTx1Message.Data[DATA_BYTE_ZERO] = BIT0_SET ;
                  
                  /**************************DATA***********************************/
                  for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                  {
                    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
                  }
                  /****************************************************************/
                  
                  
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_VCM_PGAUGE1_TH |
                    DEST_VCM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  /************************************************************/	
                  
                  /*VCM_PRESSURE_HIGHTH1 CONFIG_PARA_4*/
                  stTx1Message.DLC = FIVE_BYTES;
                  
                  SplitByte.fData = stConfigData.fConfig_PressureGauge1HTH;
                  
                  stTx1Message.Data[DATA_BYTE_ZERO] = BIT1_SET ;
                  
                  /**************************DATA***********************************/
                  for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                  {
                    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
                  }
                  /****************************************************************/
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_VCM_PGAUGE1_TH |
                    DEST_VCM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  /****************************************************************/
                  
                  
                  /*VCM_PRESSURE_LOWTH2 CONFIG_PARA_5*/
                  stTx1Message.DLC = FIVE_BYTES;
                  
                  SplitByte.fData = stConfigData.fConfig_PressureGauge2LTH;
                  
                  stTx1Message.Data[DATA_BYTE_ZERO] = BIT0_SET ;
                  
                  /**************************DATA***********************************/
                  for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                  {
                    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
                  }
                  /****************************************************************/
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_VCM_PGAUGE2_TH |
                    DEST_VCM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  /****************************************************************/
                  
                  
                  /*VCM_PRESSURE_HIGHTH2 CONFIG_PARA_6 */
                  stTx1Message.DLC = FIVE_BYTES;
                  
                  SplitByte.fData = stConfigData.fConfig_PressureGauge2HTH;
                  
                  stTx1Message.Data[DATA_BYTE_ZERO] = BIT1_SET ;
                  
                  /**************************DATA***********************************/
                  for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                  {
                    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
                  }
                  /****************************************************************/
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_VCM_PGAUGE2_TH |
                    DEST_VCM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  /****************************************************************/	
                  
                  
                  /*VCM_PRESSURE_LOWTH3 CONFIG_PARA_7*/	
                  stTx1Message.DLC = FIVE_BYTES;
                  
                  SplitByte.fData = stConfigData.fConfig_PressureGauge3LTH;
                  
                  stTx1Message.Data[DATA_BYTE_ZERO] = BIT0_SET ;
                  
                  /**************************DATA***********************************/
                  for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                  {
                    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
                  }
                  /****************************************************************/
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_VCM_PGAUGE3_TH |
                    DEST_VCM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  /****************************************************************/	
                  
                  
                  /*VCM_PRESSURE_HIGHTH3 CONFIG_PARA_8*/
                  stTx1Message.DLC = FIVE_BYTES;
                  
                  SplitByte.fData = stConfigData.fConfig_PressureGauge3HTH;
                  
                  stTx1Message.Data[DATA_BYTE_ZERO] = BIT1_SET ;
                  
                  /**************************DATA***********************************/
                  for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                  {
                    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
                  }
                  /****************************************************************/
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_VCM_PGAUGE3_TH |
                    DEST_VCM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  /****************************************************************/	
                  
                  
                  /*VCM_CYLINDER_AREA1 CONFIG_PARA_9*/
                  stTx1Message.DLC = FIVE_BYTES;
                  
                  SplitByte.fData = stConfigData.fConfig_AreaofCylinderA;
                  
                  stTx1Message.Data[DATA_BYTE_ZERO] = BIT0_SET ;
                  
                  /**************************DATA***********************************/
                  for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                  {
                    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
                  }
                  /****************************************************************/
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_CYLINDER_SURFACE_AREA |
                    DEST_VCM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  /****************************************************************/	
                  
                  
                  /*VCM_CYLINDER_AREA2 CONFIG_PARA_10*/
                  stTx1Message.DLC = FIVE_BYTES;
                  
                  SplitByte.fData = stConfigData.fConfig_AreaofCylinderB;
                  
                  stTx1Message.Data[DATA_BYTE_ZERO] = BIT1_SET ;
                  
                  /**************************DATA***********************************/
                  for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                  {
                    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
                  }
                  /****************************************************************/
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_CYLINDER_SURFACE_AREA |
                    DEST_VCM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  /****************************************************************/	
                  
                  
                  /*VCM_CYLINDER_AREA3 CONFIG_PARA_11*/
                  stTx1Message.DLC = FIVE_BYTES;
                  
                  SplitByte.fData = stConfigData.fConfig_AreaofCylinderC;
                  
                  stTx1Message.Data[DATA_BYTE_ZERO] = BIT2_SET ;
                  
                  /**************************DATA***********************************/
                  for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                  {
                    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
                  }
                  /****************************************************************/
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_CYLINDER_SURFACE_AREA |
                    DEST_VCM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  /****************************************************************/	
                  
                  
                  /*VCM_CYLINDER_COUNT CONFIG_PARA_12*/
                  stTx1Message.DLC = ONE_BYTES;
                  stTx1Message.Data[DATA_BYTE_ZERO] = stConfigData.ucConfig_CylinderCount;
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_NUMBER_OF_CYLINDER |
                    DEST_VCM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  /****************************************************************/	
                  
                  
                  /*VCM_PRESSURE_GAUGE1_CONN,
                  VCM_PRESSURE_GAUGE2_CONN,
                  VCM_PRESSURE_GAUGE3_CONN CONFIG_PARA_13*/
                  stTx1Message.DLC = THREE_BYTES;
                  stTx1Message.Data[DATA_BYTE_ZERO] = stConfigData.ucConfig_ConnPressureGuage1;
                  stTx1Message.Data[DATA_BYTE_ONE]  = stConfigData.ucConfig_ConnPressureGuage2;
                  stTx1Message.Data[DATA_BYTE_TWO] 	= stConfigData.ucConfig_ConnPressureGuage3;
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_PGAUGE_REMAP |
                    DEST_VCM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  /****************************************************************/	
                  
                  
                  /*VCM_LINEAR_TRANSDUCER_CONN CONFIG_PARA_14*/
                  stTx1Message.DLC = ONE_BYTES;
                  stTx1Message.Data[DATA_BYTE_ZERO] = stConfigData.ucConfig_ConnLinearTransducer;
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_LT_REMAP |
                    DEST_VCM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  /****************************************************************/	
                  
                  
                  /*VCM_EE07_INSIDE_CONN,
                  VCM_EE07_OUTSIDE_CONN CONFIG_PARA_15*/
                  stTx1Message.DLC = TWO_BYTES;
                  stTx1Message.Data[DATA_BYTE_ZERO] = stConfigData.ucConfig_ConnInsideEE07;
                  stTx1Message.Data[DATA_BYTE_ONE]  = stConfigData.ucConfig_ConnOutsideEE07;
                  
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_EE07_REMAP |
                    DEST_VCM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  /****************************************************************/	
                  
                  
                  /*VCM_STALL_VALVE_CONN CONFIG_PARA_16*/
									/*change53*/
                  stTx1Message.DLC = TWO_BYTES;
									stTx1Message.Data[DATA_BYTE_ZERO] = stConfigData.ucConfig_ConnStallvalve;
									stTx1Message.Data[DATA_BYTE_ONE] = stConfigData.ucConfig_ConnRecirculationvalve;
                  
                  
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_STALL_VALVE_REMAP |
                    DEST_VCM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  
                  /****************************************************************/	
                  
                  
                  /*VCM_BYPASS CONFIG_PARA_17*/
                  stTx1Message.DLC = ONE_BYTES;
                  stTx1Message.Data[DATA_BYTE_ZERO] = stConfigData.ucConfig_VCMBypass;
                  
                  
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_VCM_PERIPHRAL_BYPASS |
                    DEST_VCM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  /****************************************************************/	
                  
                  
                  /*VCM_BEFORE_STALL CONFIG_PARA_18*/
                  stTx1Message.DLC = FOUR_BYTES;
                  
                  SplitByte.fData = stConfigData.fConfig_TimeBeforeStall;
                  
                  
                  /**************************DATA***********************************/
                  for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                  {
                    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt];
                  }
                  /****************************************************************/
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_TIME_BEFORE_STALL |
                    DEST_VCM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  /****************************************************************/	
                  
                  
                  /*VCM_COMPARE_STALL CONFIG_PARA_19*/
                  stTx1Message.DLC = FOUR_BYTES;
                  
                  SplitByte.fData = stConfigData.fConfig_TimeCompareStall;
                  
                  
                  /**************************DATA***********************************/
                  for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                  {
                    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt];
                  }
                  /****************************************************************/
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_TIME_BEFORE_COMPARE_STALL |
                    DEST_VCM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  /****************************************************************/	
                  
                  /*************************change10***************************************/		
                  /*VCM_STALL_UPSTROKE_UPPER_LIMIT CONFIG_PARA_99*/
                  stTx1Message.DLC = TWO_BYTES;
                  stTx1Message.Data[DATA_BYTE_ZERO] = BIT0_SET ;
                  stTx1Message.Data[DATA_BYTE_ONE] = stConfigData.ucConfig_UpstrokeUpLimit;
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_VCM_STALL_RANGE |
                    DEST_VCM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  /****************************************************************/	
                  /*VCM_STALL_UPSTROKE_MIN_RANGE CONFIG_PARA_100*/
                  stTx1Message.DLC = TWO_BYTES;
                  stTx1Message.Data[DATA_BYTE_ZERO] = BIT1_SET ;
                  stTx1Message.Data[DATA_BYTE_ONE] = stConfigData.ucConfig_UpstrokeMinRange;
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_VCM_STALL_RANGE |
                    DEST_VCM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  /****************************************************************/	
                  /*VCM_STALL_UPSTROKE_MAX_RANGE CONFIG_PARA_101*/
                  stTx1Message.DLC = TWO_BYTES;
                  stTx1Message.Data[DATA_BYTE_ZERO] = BIT2_SET ;
                  stTx1Message.Data[DATA_BYTE_ONE] = stConfigData.ucConfig_UpstrokeMaxRange;
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_VCM_STALL_RANGE |
                    DEST_VCM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  /****************************************************************/	
                  /*VCM_STALL_DOWNSTROKE_UPPER_LIMIT CONFIG_PARA_102*/
                  stTx1Message.DLC = TWO_BYTES;
                  stTx1Message.Data[DATA_BYTE_ZERO] = BIT3_SET ;
                  stTx1Message.Data[DATA_BYTE_ONE] = stConfigData.ucConfig_DownstrokeUpLimit;
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_VCM_STALL_RANGE |
                    DEST_VCM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  /****************************************************************/	
                  /*VCM_STALL_DOWNSTROKE_MIN_RANGE CONFIG_PARA_103*/
                  stTx1Message.DLC = TWO_BYTES;
                  stTx1Message.Data[DATA_BYTE_ZERO] = BIT4_SET ;
                  stTx1Message.Data[DATA_BYTE_ONE] = stConfigData.ucConfig_DownstrokeMinRange;
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_VCM_STALL_RANGE |
                    DEST_VCM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  /****************************************************************/	
                  /*VCM_STALL_DOWNSTROKE_MAX_RANGE CONFIG_PARA_104*/
                  stTx1Message.DLC = TWO_BYTES;
                  stTx1Message.Data[DATA_BYTE_ZERO] = BIT5_SET ;
                  stTx1Message.Data[DATA_BYTE_ONE] = stConfigData.ucConfig_DownstrokeMaxRange;
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_VCM_STALL_RANGE |
                    DEST_VCM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  /****************************************************************/	
                  
                  ucDeviceConfigSent |= (1<< VCM);
                }
                
                /*****************************************************************************
                **@Function		:		fnSendHCM_Config
                **@Descriptions: 	This function is implemented for Send configuration parameters to 
                HCM
                **@parameters	: 	None
                **@return			: 	None
                *****************************************************************************/
                void fnSendHCM_Config(void)
                {
                  /*HCM_ANTI_SET_TEMP CONFIG_PARA_20*/
                  stTx1Message.DLC = FIVE_BYTES;
                  
                  SplitByte.fData = stConfigData.fConfig_AntiFrzTempSet;
                  
                  stTx1Message.Data[DATA_BYTE_ZERO] = BIT0_SET ;
                  
                  /**************************DATA***********************************/
                  for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                  {
                    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
                  }
                  /****************************************************************/
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_BASE_TEMP_SETTING |
                    DEST_HCM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  /****************************************************************/	
                  
                  
                  /*HCM_ANTI_THRESH_TEMP_LOW CONFIG_PARA_21*/
                  stTx1Message.DLC = FIVE_BYTES;
                  
                  SplitByte.fData = stConfigData.fConfig_AntiFrzTempLTH;
                  
                  stTx1Message.Data[DATA_BYTE_ZERO] = BIT1_SET ;
                  
                  /**************************DATA***********************************/
                  for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                  {
                    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
                  }
                  /****************************************************************/
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_BASE_TEMP_SETTING |
                    DEST_HCM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  /****************************************************************/	
                  
                  /*HCM_ANTI_THRESH_TEMP_HIGH CONFIG_PARA_22*/
                  stTx1Message.DLC = FIVE_BYTES;
                  
                  SplitByte.fData = stConfigData.fConfig_AntiFrzTempHTH;
                  
                  stTx1Message.Data[DATA_BYTE_ZERO] = BIT2_SET ;
                  
                  /**************************DATA***********************************/
                  for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                  {
                    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
                  }
                  /****************************************************************/
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_BASE_TEMP_SETTING |
                    DEST_HCM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  /****************************************************************/	
                  
                  /*HCM_ANTI_HYST_TEMP_POS CONFIG_PARA_23*/
                  stTx1Message.DLC = FIVE_BYTES;
                  
                  SplitByte.fData = stConfigData.fConfig_HysAntiFrzPos;
                  
                  stTx1Message.Data[DATA_BYTE_ZERO] = BIT3_SET ;
                  
                  /**************************DATA***********************************/
                  for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                  {
                    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
                  }
                  /****************************************************************/
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_BASE_TEMP_SETTING |
                    DEST_HCM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  /****************************************************************/	
                  
                  /*HCM_ANTI_HYST_TEMP_NEG CONFIG_PARA_24*/
                  stTx1Message.DLC = FIVE_BYTES;
                  
                  SplitByte.fData = stConfigData.fConfig_HysAntiFrzNeg;
                  
                  stTx1Message.Data[DATA_BYTE_ZERO] = BIT4_SET ;
                  
                  /**************************DATA***********************************/
                  for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                  {
                    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
                  }
                  /****************************************************************/
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_BASE_TEMP_SETTING |
                    DEST_HCM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  /****************************************************************/	
                  
                  /*HCM_INLINE_BASE_SET_TEMP CONFIG_PARA_25*/
                  stTx1Message.DLC = FIVE_BYTES;
                  
                  SplitByte.fData = stConfigData.fConfig_InlineBaseTempSet;
                  
                  stTx1Message.Data[DATA_BYTE_ZERO] = BIT0_SET ;
                  
                  /**************************DATA***********************************/
                  for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                  {
                    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
                  }
                  /****************************************************************/
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_INLINE_BASE_TEMP_SETTING |
                    DEST_HCM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  /****************************************************************/	
                  
                  /*HCM_INLINE_BASE_THRESH_TEMP_LOW CONFIG_PARA_26*/
                  stTx1Message.DLC = FIVE_BYTES;
                  
                  SplitByte.fData = stConfigData.fConfig_InlineBaseTempLTH;
                  
                  stTx1Message.Data[DATA_BYTE_ZERO] = BIT1_SET ;
                  
                  /**************************DATA***********************************/
                  for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                  {
                    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
                  }
                  /****************************************************************/
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_INLINE_BASE_TEMP_SETTING |
                    DEST_HCM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  /****************************************************************/	
                  
                  /*HCM_INLINE_BASE_THRESH_TEMP_HIGH CONFIG_PARA_27*/
                  stTx1Message.DLC = FIVE_BYTES;
                  
                  SplitByte.fData = stConfigData.fConfig_InlineBaseTempHTH;
                  
                  stTx1Message.Data[DATA_BYTE_ZERO] = BIT2_SET ;
                  
                  /**************************DATA***********************************/
                  for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                  {
                    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
                  }
                  /****************************************************************/
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_INLINE_BASE_TEMP_SETTING |
                    DEST_HCM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  /****************************************************************/		
                  /*HCM_INLINE_BASE_HYST_TEMP_POS CONFIG_PARA_28*/
                  stTx1Message.DLC = FIVE_BYTES;
                  
                  SplitByte.fData = stConfigData.fConfig_HysInlineBasePos;
                  
                  stTx1Message.Data[DATA_BYTE_ZERO] = BIT3_SET ;
                  
                  /**************************DATA***********************************/
                  for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                  {
                    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
                  }
                  /****************************************************************/
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_INLINE_BASE_TEMP_SETTING |
                    DEST_HCM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  /****************************************************************/		
                  
                  
                  /*HCM_INLINE_BASE_HYST_TEMP_NEG CONFIG_PARA_29*/
                  stTx1Message.DLC = FIVE_BYTES;
                  
                  SplitByte.fData = stConfigData.fConfig_HysInlineBaseNeg;
                  
                  stTx1Message.Data[DATA_BYTE_ZERO] = BIT4_SET ;
                  
                  /**************************DATA***********************************/
                  for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                  {
                    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
                  }
                  /****************************************************************/
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_INLINE_BASE_TEMP_SETTING |
                    DEST_HCM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  /****************************************************************/		
                  
                  /*HCM_INLINE_HARD_SET_TEMP CONFIG_PARA_30*/
                  stTx1Message.DLC = FIVE_BYTES;
                  
                  SplitByte.fData = stConfigData.fConfig_InlineHardTempSet;
                  
                  stTx1Message.Data[DATA_BYTE_ZERO] = BIT0_SET ;
                  
                  /**************************DATA***********************************/
                  for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                  {
                    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
                  }
                  /****************************************************************/
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_INLINE_HARD_TEMP_SETTING |
                    DEST_HCM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  /****************************************************************/		
                  
                  
                  /*HCM_INLINE_HARD_THRESH_TEMP_LOW CONFIG_PARA_31*/
                  stTx1Message.DLC = FIVE_BYTES;
                  
                  SplitByte.fData = stConfigData.fConfig_InlineHardTempLTH;
                  
                  stTx1Message.Data[DATA_BYTE_ZERO] = BIT1_SET ;
                  
                  /**************************DATA***********************************/
                  for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                  {
                    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
                  }
                  /****************************************************************/
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_INLINE_HARD_TEMP_SETTING |
                    DEST_HCM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  /****************************************************************/		
                  
                  
                  /*HCM_INLINE_HARD_THRESH_TEMP_HIGH CONFIG_PARA_32*/
                  stTx1Message.DLC = FIVE_BYTES;
                  
                  SplitByte.fData = stConfigData.fConfig_InlineHardTempHTH;
                  
                  stTx1Message.Data[DATA_BYTE_ZERO] = BIT2_SET ;
                  
                  /**************************DATA***********************************/
                  for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                  {
                    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
                  }
                  /****************************************************************/
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_INLINE_HARD_TEMP_SETTING |
                    DEST_HCM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  /****************************************************************/		
                  
                  /*HCM_INLINE_HARD_HYST_TEMP_POS CONFIG_PARA_33*/
                  stTx1Message.DLC = FIVE_BYTES;
                  
                  SplitByte.fData = stConfigData.fConfig_InlineHysHardPos;
                  
                  stTx1Message.Data[DATA_BYTE_ZERO] = BIT3_SET ;
                  
                  /**************************DATA***********************************/
                  for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                  {
                    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
                  }
                  /****************************************************************/
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_INLINE_HARD_TEMP_SETTING |
                    DEST_HCM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  /****************************************************************/		
                  /*HCM_INLINE_HARD_HYST_TEMP_NEG CONFIG_PARA_34*/
                  stTx1Message.DLC = FIVE_BYTES;
                  
                  SplitByte.fData = stConfigData.fConfig_InlineHysHardNeg;
                  
                  stTx1Message.Data[DATA_BYTE_ZERO] = BIT4_SET ;
                  
                  /**************************DATA***********************************/
                  for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                  {
                    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
                  }
                  /****************************************************************/
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_INLINE_HARD_TEMP_SETTING |
                    DEST_HCM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  /****************************************************************/		
                  /*HCM_BASE_LIQUID_THRESH_LEVEL_LOW CONFIG_PARA_35*/
                  stTx1Message.DLC = FIVE_BYTES;
                  
                  SplitByte.fData = stConfigData.fConfig_BaseLiquidLevelLTH;
                  
                  stTx1Message.Data[DATA_BYTE_ZERO] = BIT0_SET ;
                  
                  /**************************DATA***********************************/
                  for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                  {
                    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
                  }
                  /****************************************************************/
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_BASE_LEVEL_SETTING |
                    DEST_HCM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  /****************************************************************/		
                  /*HCM_BASE_LIQUID_THRESH_LEVEL_HIGH CONFIG_PARA_36*/
                  stTx1Message.DLC = FIVE_BYTES;
                  
                  SplitByte.fData = stConfigData.fConfig_BaseLiquidLevelHTH;
                  
                  stTx1Message.Data[DATA_BYTE_ZERO] = BIT1_SET ;
                  
                  /**************************DATA***********************************/
                  for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                  {
                    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
                  }
                  /****************************************************************/
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_BASE_LEVEL_SETTING |
                    DEST_HCM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  /****************************************************************/		
                  /*HCM_HARD_LIQUID_THRESH_LEVEL_LOW CONFIG_PARA_37*/
                  stTx1Message.DLC = FIVE_BYTES;
                  
                  SplitByte.fData = stConfigData.fConfig_HardLiquidLevelLTH;
                  
                  stTx1Message.Data[DATA_BYTE_ZERO] = BIT0_SET ;
                  
                  /**************************DATA***********************************/
                  for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                  {
                    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
                  }
                  /****************************************************************/
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_HARD_LEVEL_SETTING |
                    DEST_HCM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  /****************************************************************/		
                  /*HCM_HARD_LIQUID_THRESH_LEVEL_HIGH CONFIG_PARA_38*/
                  stTx1Message.DLC = FIVE_BYTES;
                  
                  SplitByte.fData = stConfigData.fConfig_HardLiquidLevelHTH;
                  
                  stTx1Message.Data[DATA_BYTE_ZERO] = BIT1_SET ;
                  
                  /**************************DATA***********************************/
                  for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                  {
                    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
                  }
                  /****************************************************************/
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_HARD_LEVEL_SETTING |
                    DEST_HCM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  /****************************************************************/		
                  /*HCM_BASE_TANK_HEIGHT CONFIG_PARA_39*/
                  stTx1Message.DLC = FIVE_BYTES;
                  
                  SplitByte.fData = stConfigData.fConfig_BaseTankHeight;
                  
                  stTx1Message.Data[DATA_BYTE_ZERO] = BIT0_SET ;
                  
                  /**************************DATA***********************************/
                  for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                  {
                    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
                  }
                  /****************************************************************/
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_TANK_HEIGHT |
                    DEST_HCM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  /****************************************************************/		
                  /*HCM_HARD_TANK_HEIGHT CONFIG_PARA_40*/
                  stTx1Message.DLC = FIVE_BYTES;
                  
                  SplitByte.fData = stConfigData.fConfig_HardTankHeight;
                  
                  stTx1Message.Data[DATA_BYTE_ZERO] = BIT1_SET ;
                  
                  /**************************DATA***********************************/
                  for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                  {
                    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt-1];
                  }
                  /****************************************************************/
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_TANK_HEIGHT |
                    DEST_HCM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  /****************************************************************/		
                  /*HCM_LIQUID_LEVEL_SAMPLING_RATE CONFIG_PARA_41 change19*/
                  stTx1Message.DLC = ONE_BYTES;
                  
                  SplitByte.uiData = stConfigData.uiConfig_LLSamplingRate;
                  
                  
                  
                  /**************************DATA***********************************/
                  for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                  {
                    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt];
                  }
                  /****************************************************************/
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_LL_SAMPLE_RATE |
                    DEST_HCM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  /****************************************************************/		
                  /*HCM_INLINE_BASE_THERM_CONN,
                  HCM_INLINE_HARD_THERM_CONN,
                  HCM_INLINE_ANTIFREEZE_THERM_CONN,
                  HCM_RETURN_BASE_THERM_CONN CONFIG_PARA_42*/
                  stTx1Message.DLC = FOUR_BYTES;
                  
                  stTx1Message.Data[DATA_BYTE_ZERO] = stConfigData.ucConfig_ConnBaseinlinether;
                  stTx1Message.Data[DATA_BYTE_ONE] = stConfigData.ucConfig_ConnHardinlinether;
                  stTx1Message.Data[DATA_BYTE_TWO] = stConfigData.ucConfig_ConnAntifreezether;
                  stTx1Message.Data[DATA_BYTE_THREE] = stConfigData.ucConfig_ConnBasereturnlinether;
                  
                  
                  
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_THERMOCOUPLE_REMAP |
                    DEST_HCM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  /****************************************************************/		
                  /*HCM_INLINE_BASE_HEATER_CONN,
                  HCM_INLINE_HARD_HEATER_CONN,
                  HCM_INLINE_ANTIFREEZE_HEATER_CONN,
                  HCM_HARD_HEATER_CONN CONFIG_PARA_43*/
                  stTx1Message.DLC = FOUR_BYTES;
                  
                  stTx1Message.Data[DATA_BYTE_ZERO] 	= stConfigData.ucConfig_ConnBaseinlineheater;
                  stTx1Message.Data[DATA_BYTE_ONE] 		= stConfigData.ucConfig_ConnHardinlineheater;
                  stTx1Message.Data[DATA_BYTE_TWO] 		= stConfigData.ucConfig_ConnAntifreezeheater;
                  stTx1Message.Data[DATA_BYTE_THREE] 	= stConfigData.ucConfig_ConnHardheater;
                  
                  
                  
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_SSR_REMAP |
                    DEST_HCM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  /****************************************************************/		
                  /*HCM_BASE_LIQUID_LEVEL_CONN,
                  HCM_HARD_LIQUID_LEVEL_CONN CONFIG_PARA_44*/
                  stTx1Message.DLC = TWO_BYTES;
                  
                  stTx1Message.Data[DATA_BYTE_ZERO] 	= stConfigData.ucConfig_ConnBasetankLL;
                  stTx1Message.Data[DATA_BYTE_ONE] 		= stConfigData.ucConfig_ConnHardtankLL;
                  
                  
                  
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_LL_SENSOR_REMAP |
                    DEST_HCM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  /****************************************************************/		
                  /*HCM_BYPASS CONFIG_PARA_45*/
                  stTx1Message.DLC = TWO_BYTES;
                  
                  SplitByte.iData 	= stConfigData.uiConfig_HCMBypass;
                  
                  /**************************DATA***********************************/
                  for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                  {
                    stTx1Message.Data[ucLoopCnt]	 = SplitByte.rgucData[ucLoopCnt];
                  }
                  /****************************************************************/
                  
                  
                  
                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_HCM_PERIPHRAL_BYPASS |
                    DEST_HCM | SOURCE_SCU;
                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                  /****************************************************************/		
                  ucDeviceConfigSent |= (1<< HCM);
                }
                
                
                
                /*****************************************************************************
                **@Function		:	 	fnLED_Indication
                **@Descriptions: 	This function is implemented for To Check Pressure and 
                Temperature are in Range 
                **@parameters	:  	None
                **@return			:  	None
                *****************************************************************************/
                void fnLED_Indication(void)
                {
                  if(RESET == StatusFlag.stStatusFlag.bDefinedState)
                  {
                    if(SET == StatusFlag.stStatusFlag.bCANFaultFlag)
                    {
                      if(uiCANFaultLEDBlinkCnt >  STATUS_LED_BLINK_COUNT)
                      {
                        uiCANFaultLEDBlinkCnt = RESET;
                        ClearBit(uiIOExpData , BOARD_STATUS_BLUE_LED);
                        ClearBit(uiIOExpData , BOARD_STATUS_GREEN_LED);
                        ToggleBit(uiIOExpData , BOARD_STATUS_RED_LED);
                      }
                    }	
                    else if((SET == StatusFlag.stStatusFlag.bErrorFlag)||
                            (SET == StatusFlag.stStatusFlag.bEStop_Flag))
                    {
                      //	printf("ERROR\n\r");
                      ClearBit(uiIOExpData , BOARD_STATUS_BLUE_LED);
                      ClearBit(uiIOExpData , BOARD_STATUS_GREEN_LED);
                      SetBit(uiIOExpData , BOARD_STATUS_RED_LED);
                    }
                    else if(SET == StatusFlag.stStatusFlag.bWarningFlag)
                    {
                      //	printf("Warning\n\r");
                      SetBit(uiIOExpData , BOARD_STATUS_BLUE_LED);
                      ClearBit(uiIOExpData , BOARD_STATUS_GREEN_LED);
                      ClearBit(uiIOExpData , BOARD_STATUS_RED_LED);
                    }
                    else
                    {
                      //	printf("NOTHING\n\r");
                      ClearBit(uiIOExpData , BOARD_STATUS_BLUE_LED);
                      SetBit(uiIOExpData , BOARD_STATUS_GREEN_LED);
                      ClearBit(uiIOExpData , BOARD_STATUS_RED_LED);
                    }
                    if((SET == StatusFlag.stStatusFlag.bCAN1ActiveFlag)&&		
                       (RESET == StatusFlag.stStatusFlag.bCANFaultFlag))
                    {
                      SetBit(uiIOExpData , CAN1_STATUS_GREEN_LED);
                      ClearBit(uiIOExpData , CAN1_STATUS_RED_LED);
                      ClearBit(uiIOExpData , CAN1_STATUS_BLUE_LED);
                    }
                    if((SET == StatusFlag.stStatusFlag.bCAN2ActiveFlag)&&		
                       (RESET == StatusFlag.stStatusFlag.bCANFaultFlag))	
                    {
                      SetBit(uiIOExpData , CAN2_STATUS_GREEN_LED);
                      ClearBit(uiIOExpData , CAN2_STATUS_RED_LED);
                      ClearBit(uiIOExpData , CAN2_STATUS_BLUE_LED);
                    }
                    else if(RESET == StatusFlag.stStatusFlag.bCAN2ActiveFlag)		
                      
                    {
                      ClearBit(uiIOExpData , CAN2_STATUS_GREEN_LED);
                      ClearBit(uiIOExpData , CAN2_STATUS_RED_LED);
                      ClearBit(uiIOExpData , CAN2_STATUS_BLUE_LED);
                    }
                  }	
                  if((uiIOExpData != uiPreIOExpander) && 
                     (RESET == StatusFlag.stStatusFlag.bI2CBusyFlag))
                  {
                    if(!I2C_GetFlagStatus(I2C_IO_EXP, I2C_FLAG_BUSY))
                    {
                      uiI2CWriteData =  ~uiIOExpData;
                      
                      StatusFlag.stStatusFlag.bI2CBusyFlag = SET;
                      ucI2C1Seq = 0;
                      /* Slave Address with Write instruction */
                      ucIOExpSlaveAddr = SLAVE1_ADDRESS & I2C_SLAVE_WRITE; 
                      
                      /* Send I2C1 START condition */
                      I2C_GenerateSTART(I2C_IO_EXP, ENABLE);
                      
                      uiPreIOExpander = uiIOExpData;
#ifdef IOEXP_DBG
                      DEBUG_PRINTF("IOEXP_1_SLAVE_ADDR\r\n");
#endif
                    }
                  }
                  
}



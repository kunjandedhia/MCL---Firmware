/******************************************************************************
**@file     :   MiscFunctions.c
* 
**@brief    :   This file contains the Misceleneous functions  
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
uint8_t ucLoopCnt = 0;
uint8_t ucStalFailureCount = 0;
uint16_t uiAmbiantReadCnt = 0;
uint16_t uiRTCReadCnt = 0;
uint16_t uiRLTempReadCnt = 0;
float fPressureRegRead = 0.0f;
float fPressureRegWrite = 0.0f;
/*Test only*/
uint8_t ucTempCountTEST = 0;

// TypeDefStatus StatusFlag;            /*Operation Flag*/
// uint8_t rgucHBCntr[TOTAL_HB_DEVICE + 1] = {0};
// uint8_t ucHBCntr = 0;
// uint16_t uiHBACKfail = 0;
// uint8_t rgucHBFail[2] = {0}; /*Global Array for Fault Data Logging*/
/* Each bit tells to send device functionality to respective device*/
uint16_t uiDeviceFuncSend = 0;     
/* Each bit tells to send device functionality to respective device*/
uint16_t uiDeviceDiagStatus = 0;    

uint8_t ucIOExp1LowerByte = 0;/*EXP1 lower Port*/
uint8_t ucIOExp1HigherByte = 0;/*EXP1 higher Port*/
uint8_t ucIOExp2LowerByte = 0;/*EXP2 lower Port*/
uint8_t ucIOExp2HigherByte = 0;/*EXP2 Higher Port*/

uint8_t ucMCUMode = MCU_DEFAULT_MODE;
uint16_t uiDiagnosticTimer = 0;
uint16_t uiStartUpDelayCount = 0;
uint16_t uiDeviceConfigSent = 0;
uint16_t uiAnyCANFaultLEDBlinkCnt = 0;
uint16_t uiOperationTimeOut = 0;

uint8_t ucCANLinkFaultCnt = 0;
// CanTxMsg stTx1Message;
// uint16_t uiHBFailSend = 0;
uint16_t uiPressureRegReadCnt = 0;
// uint8_t ucEEPROMCounter = 0;

uint8_t ucI2CRxCnt = 0;
uint8_t ucTempHighByte = 0;
uint8_t ucTempLowByte = 0;
uint8_t rgucI2CRxBuff[2];
uint8_t ucSlave_Add = 0;
uint8_t ucI2CSequence = 0; 

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
**@Function		 	:  fnDefaul_Configuration
**@Descriptions	:  This function is used to configure the default values 
**@parameters		:  None
**@return				:  None
*****************************************************************************/
void fnDefaul_Configuration(void)
{
  /*******************ENABLE CAN****************/
  ucIOExp1HigherByte |=	PRESSURE_REG_STATUS_RED_LED;/*Initially RED ON*/
  ucIOExp1HigherByte &=	(~PRESSURE_REG_STATUS_BLUE_LED);/*Initially BLUE  OFF*/
  ucIOExp1HigherByte &=	(~PRESSURE_REG_STATUS_GREEN_LED);/*Initially GREEN OFF */
  /*Initially OFF CAN LEDs*/
  ucIOExp1LowerByte	&= (~CAN1_STATUS_RED_LED);
  ucIOExp1LowerByte	&= (~CAN1_STATUS_BLUE_LED);
  ucIOExp1LowerByte	&= (~CAN1_STATUS_GREEN_LED);
  /*Initially OFF CAN LEDs*/
  ucIOExp1LowerByte	&= (~CAN2_STATUS_RED_LED);
  ucIOExp1LowerByte	&= (~CAN2_STATUS_BLUE_LED);
  ucIOExp1LowerByte	&= (~CAN2_STATUS_GREEN_LED);
  /*POWER STATUS LED IS NOT USED*/
  ucIOExp1LowerByte	&= (~POWER_STATUS_RED_LED);
  ucIOExp1LowerByte	&= (~POWER_STATUS_BLUE_LED);	
  
  ucIOExp1HigherByte |=	BOARD_STATUS_RED_LED;/*Initially RED ON*/
  ucIOExp1HigherByte &=	(~BOARD_STATUS_BLUE_LED);/*Initially BLUE  OFF*/
  ucIOExp1HigherByte &=	(~BOARD_STATUS_GREEN_LED);/*Initially GREEN OFF */
  /*Initially OFF CAN LEDs*/	
  ucIOExp2HigherByte &=	(~CAN2B_STATUS_RED_LED);
  ucIOExp2HigherByte &=	(~CAN2B_STATUS_BLUE_LED);
  ucIOExp2HigherByte &=	(~CAN2B_STATUS_GREEN_LED);
  
  ucIOExp2LowerByte |= (CAN1_ENABLE_PIN | WATCHDOG_ENABLE | WATCHDOG_INPUT);
  ucIOExp2HigherByte |=  (CAN1_STROBE_PIN	| CAN2_ENABLE_PIN | 
                          CAN2_STROBE_PIN	| CAN2B_ENABLE_PIN	| CAN2B_STROBE_PIN);  
  
  fnIOExpander_Write(SLAVE1_ADDRESS, ucIOExp1LowerByte, ucIOExp1HigherByte);
  fnIOExpander_Write(SLAVE2_ADDRESS, ucIOExp2LowerByte, ucIOExp2HigherByte);
  
  /*Reset the FIFO of all the Modules*/
  fnFifo_Reset(&pstFifo_OP);
  fnFifo_Reset(&pstFifo_OPMM);
  fnFifo_Reset(&pstFifo_MFM);
  fnFifo_Reset(&pstFifo_BHCM);
  fnFifo_Reset(&pstFifo_HHCM);
  fnFifo_Reset(&pstFifo_IHCM1);
  fnFifo_Reset(&pstFifo_IHCM4_HM);
  
  /*RESET all the flags*/
  StatusFlag.uliResetFlag[0] = 0;
  StatusFlag.uliResetFlag[1] = 0;
  StatusFlag.uliResetFlag[2] = 0;
  StatusFlag.uliResetFlag[3] = 0;
  StatusFlag.stStatusFlag.bCAN2ActiveFlag = RESET;
  StatusFlag.stStatusFlag.bCAN1ActiveFlag = SET;
}



/*****************************************************************************
**@Function		:	 	fnDefault_Mode
**@Descriptions: 	This function is implemented for Default Mode of MCU
**@parameters	:  	None
**@return			:  	None
*****************************************************************************/
void fnDefault_Mode(void)
{
  uint16_t uiNVSRAM1;
  uint16_t uiNVSRAM2;
  uint8_t ucTestData = 'T';
  uint8_t ucRdTestData = 0;
  
  if(RESET == StatusFlag.stStatusFlag.bDiagnosticDone)
  {
    /*This flag is use to do diagnostic one time only*/
    StatusFlag.stStatusFlag.bDiagnosticDone = SET;
#ifdef GENERAL_DBG
    DEBUG_PRINTF("fnNVSRAM_Word_Write\r\n");
#endif
    fnNVSRAM1Word_Write( NVSRAM1_START, DUMMY1);
    fnNVSRAM2Word_Write( NVSRAM2_START, DUMMY2);
    
	
    uiNVSRAM1 = fnNVSRAM1Word_Read(NVSRAM1_START);
    uiNVSRAM2 = fnNVSRAM2Word_Read(NVSRAM2_START);
    
    memset(stTx1Message.Data,0,sizeof(stTx1Message.Data));
    
    if((DUMMY1 == uiNVSRAM1) && (DUMMY2 == uiNVSRAM2))
    {
#ifdef GENERAL_DBG
      DEBUG_PRINTF("NVSRAM1 and NVSRAM2 Diagnostic Pass\r\n");
#endif
    }
    else
    {
#ifdef GENERAL_DBG
      DEBUG_PRINTF("NVSRAM1 and NVSRAM2 Diagnostic Fail\r\n");
#endif
      StatusFlag.stStatusFlag.bDiagnosticStatus = SET;
      stTx1Message.Data[1] |= NVSRAM_DIAGNOSTIC_FAIL;
    }
    /********************** EEPROM Diagnostics Test of MCU ********************/
    fnSPI_EEPROM_WriteData(EEPROM_TEST_ADD, TEST_BYTES_COUNT, &ucTestData );
    fnSPI_EEPROM_ReadData(EEPROM_TEST_ADD, TEST_BYTES_COUNT, &ucRdTestData);
    
    if(ucRdTestData == ucTestData)
    {
#ifdef GENERAL_DBG
      DEBUG_PRINTF("EEPROM Diagnostic Pass\r\n");
#endif
    }
    else
    {
      stTx1Message.Data[1] |= EEPROM_DIAGNOSTIC_FAIL;
      StatusFlag.stStatusFlag.bDiagnosticStatus = SET;
#ifdef GENERAL_DBG
      DEBUG_PRINTF("EEPROM Diagnostic Fail\r\n");
#endif
    }
    
    /******************* Pressure Regulator Test of MCU **********************/
    //		fPressureRegRead = fnADCValue_Read(PRESSURE_REG_ADC_CH);
    //		if((PRESSURE_REG_LOWER_READ <= fPressureRegRead) && 
    //	  	 (PRESSURE_REG_HIGHER_READ >= fPressureRegRead))
    //		{
    //			#ifdef GENERAL_DBG
    //				 DEBUG_PRINTF("Pressure Regulator Diagnostic Pass\r\n");
    //			#endif
    //		}
    //		else
    //		{
    //			#ifdef GENERAL_DBG
    //				 DEBUG_PRINTF("Pressure Regulator Diagnostic Fail\r\n");
    //			#endif
    //			StatusFlag.stStatusFlag.bDiagnosticStatus = SET;
    //			stTx1Message.Data[1] |= PRESSURE_REG_DIAGNOSTIC_FAIL; 
    //		}
    
    /**************** Thermocouple Diagnostics Test of MCU *****************/
    /*thermocouple Read and if any error than it set thermocouple flag */
    //		fnReadThermocouple();
    
    //		if(RESET == StatusFlag.stStatusFlag.bThermocoupleFault)
    //		{
    //			#ifdef GENERAL_DBG
    //				 DEBUG_PRINTF("Thermocouple Diagnostics Pass\r\n");
    //			#endif	
    //		}
    //		else
    //		{
    //			StatusFlag.stStatusFlag.bDiagnosticStatus = SET;
    //			stTx1Message.Data[1] |= THERMOCOUPLE_DIAGNOSTIC_FAIL; 
    //			#ifdef GENERAL_DBG
    //				 DEBUG_PRINTF("Thermocouple Diagnostics Fail\r\n");
    //			#endif	
    //		}
    
    
    if(SET == StatusFlag.stStatusFlag.bDiagnosticStatus)
    {
      stTx1Message.Data[0] = DIAGNOSTIC_FAIL;
      ucIOExp1HigherByte |= BOARD_STATUS_RED_LED;
      fnIOExpander_Write(SLAVE1_ADDRESS, ucIOExp1LowerByte,ucIOExp1HigherByte );
#ifdef GENERAL_DBG
      DEBUG_PRINTF("Diagnostics Fail\r\n");
#endif
    }
    else
    {
      stTx1Message.Data[0] = DIAGNOSTIC_PASS;
      
#ifdef GENERAL_DBG
      DEBUG_PRINTF("Diagnostics Pass\r\n");
#endif			
    }
    
  }
  
  /*Proceed further if OP and MCU diagnostics pass*/
  /*Changes for CRC testing*/
  if((uiDeviceDiagStatus & 0x0001)  && (RESET == StatusFlag.stStatusFlag.bTemp9Flag) /*&&
     (RESET == StatusFlag.stStatusFlag.bDiagnosticStatus)*/)
  {
    stTx1Message.DLC = 2;
    stTx1Message.ExtId = STATUS_MCU_DIAGNOSTIC | MSG_TYPE_STATUS |
      DEST_OP | SOURCE_MCU;		
    fnCANMsg_Transmit(&stTx1Message,CAN1);
    fnDatalogMCU_Diag(stTx1Message.Data);/*MCU DIAGNOSTIC STATUS LOG*/
    
#ifdef FINAL_TESTING
    DEBUG_PRINTF("MCU and OP Diagnostic Pass, SEND CONFIG PARA TO OP\r\n");
#endif
    /*send the configuration parameters to OP*/
    fnSendOPConfig();
    StatusFlag.stStatusFlag.bTemp9Flag = SET;
    StatusFlag.stStatusFlag.bStartupDelayFlag = SET;/*Startup Delay Init*/
  }
  /*Start-up Delay*/
  /* Periodic Heartbeat start after the Startup Delay*/
  if(STARTUP_DELAY_MCU_DIAG <= uiStartUpDelayCount)
  {
#ifdef GENERAL_DBG
    DEBUG_PRINTF("StartUp Delay Over Device func: %x\r\n",uiDeviceFuncSend);
#endif
	
    /* Check response from all modules for first boot-up */
    if(ALL_SLAVE_DEVICE <= uiDeviceFuncSend) 
    {
      
#ifdef GENERAL_DBG
      DEBUG_PRINTF("All Device DEVICE FUNCTIONALITY\r\n");
#endif
      uiDeviceFuncSend = 0;  
      /* Start Diagnostic delay timer counter */
      StatusFlag.stStatusFlag.bStartDiagTimer = SET;
    }
    
    /*Wait for Diagnostic Delay*/
    if(DIAGNOSTIC_TIMER_OVER <= uiDiagnosticTimer)
    {
      StatusFlag.stStatusFlag.bStartDiagTimer = RESET;
      
#ifdef GENERAL_DBG
      DEBUG_PRINTF("Diagnostic = 0x%x\r\n",uiDeviceDiagStatus);
#endif
      
      /*Check for diagnostic tests of all slave devices*/
      if(ALL_SLAVE_DEVICE <= uiDeviceDiagStatus) 
      {
#ifdef GENERAL_DBG
        DEBUG_PRINTF("All Device Diagnostic Pass: %x, uiDeviceDiagStatus\
                     : 0x%x\r\n",uiDeviceConfigSent, uiDeviceDiagStatus);
#endif
                       
                       /* Check if device configuration is sent to all modules */
                       if(ALL_SLAVE_DEVICE_CONFIG_SEND  <= uiDeviceConfigSent)
                       {
                         uiDeviceConfigSent = 0;
                         uiDeviceDiagStatus = 0;
#ifdef FINAL_TESTING
                         DEBUG_PRINTF("Configuration parameters to all device sent SWITCHING\r\n");
#endif
                         ucMCUMode = MCU_WARMUP_MODE;
                       }
      }
			else 
			{
              if(RESET == StatusFlag.stStatusFlag.bDeviceNotPresentTx)
              {
                StatusFlag.stStatusFlag.bDeviceNotPresentTx = SET;
                /* Wait for user*/
                /* User will take respective action according to the Diagnostic fail */
                //					StatusFlag.stStatusFlag.bWarnningFlag = SET;
                //					stTx1Message.ExtId = MSG_TYPE_STATUS | STATUS_DEVICE_NOT_PRESENT | DEST_OP | SOURCE_MCU;
                //					stTx1Message.DLC = 2;
                //					stTx1Message.Data[0] = (~((uiDeviceDiagStatus >> 1) & 0x00ff));/*Except OP*/
                //					stTx1Message.Data[1] = (~((uiDeviceDiagStatus >> 8) & 0x00ff));
                //					fnCANMsg_Transmit(&stTx1Message, CAN1);	
                //					#ifdef FINAL_TESTING
                //						DEBUG_PRINTF("All Device Diagnostic Pass Status Not Rcv\r\n");
                //					#endif
              }
			}
    }
  }
}
                     
                     /*****************************************************************************
                     **@Function			:	 	fnDefault_Mode
                     **@Descriptions	: 	This function is implemented for Default Mode of MCU
                     **@parameters		:  	None
                     **@return				:  	None
                     *****************************************************************************/
                     void fnWarmUp_Mode(void)
                     {
#ifdef GENERAL_DBG
                       char rgcTemp[200];
#endif
                       
                       if(RESET == StatusFlag.stStatusFlag.bWarmUpModeCmdFlag)
                       {
                         /*Reset this flag again in Start of Operation Mode*/
                         StatusFlag.stStatusFlag.bWarmUpModeCmdFlag = SET; 
                         
                         /*Open Recirculation Valve*/
                         stTx1Message.ExtId = MSG_TYPE_COMMAND | CMD_VALVE_MFM |
                           DEST_MFM | SOURCE_MCU;
                         stTx1Message.DLC = 0x02;
                         stTx1Message.Data[0] = 0x01;/* Bit 0 switch on the valve*/
                         stTx1Message.Data[1] = 0x01;  
                         fnCANMsg_Transmit(&stTx1Message, CAN1);
                         stTx1Message.ExtId = MSG_TYPE_COMMAND | CMD_VALVE_MFM |
                           DEST_OP | SOURCE_MCU;
                         fnCANMsg_Transmit(&stTx1Message, CAN1);
                         
#ifdef FINAL_TESTING
                         DEBUG_PRINTF("Turn ON Heater of HHCM\r\n");
#endif
                         /* Turn ON Heaters for HHCM */
                         stTx1Message.ExtId = MSG_TYPE_COMMAND | CMD_TURN_ON_OFF_HEATER |
                           DEST_HHCM | SOURCE_MCU;
                         stTx1Message.DLC = 0x01;
                         stTx1Message.Data[0] = 0x01;/* Bit 0 0 switches on the heater*/
                         fnCANMsg_Transmit(&stTx1Message, CAN1);
                         
#ifdef FINAL_TESTING
                         DEBUG_PRINTF("Turn ON Heater Controlling of BHCM\r\n");
#endif
                         /* Turn ON Heaters for BHCM */
                         stTx1Message.ExtId = MSG_TYPE_COMMAND | CMD_TURN_ON_OFF_HEATER| 
                           DEST_BHCM | SOURCE_MCU;
                         fnCANMsg_Transmit(&stTx1Message, CAN1);
                         
#ifdef FINAL_TESTING
                         DEBUG_PRINTF("Turn ON Heater Controlling of IHCM1\r\n");
#endif
                         /* Turn ON Heaters for IHCM1 */
                         stTx1Message.ExtId = MSG_TYPE_COMMAND | CMD_TURN_ON_OFF_HEATER |
                           DEST_IHCM1 | SOURCE_MCU;
                         fnCANMsg_Transmit(&stTx1Message, CAN1);
                         
                         
                         
#ifdef FINAL_TESTING
                         DEBUG_PRINTF("Turn ON Heater Controlling of IHCM4_HM\r\n");
#endif
                         /* Turn ON Heaters for IHCM4_HM */
                         stTx1Message.ExtId = MSG_TYPE_COMMAND | CMD_TURN_ON_OFF_HEATER |
                           DEST_IHCM4_HM | SOURCE_MCU;
                         fnCANMsg_Transmit(&stTx1Message, CAN1);
                       }
                       
                       /*Start monitoring recirculating pressure*/
                       /*Check pressure valve periodically till*/
                       /*it reaches recirculating pressure*/
                       
                       /* Converting Recirculation Pressure BAR to PSI */
                       //	fPressureRegWrite = BAR_TO_PSI(stConfigData.fConfigRecirPressure);
                       //	
                       //	#ifdef GENERAL_DBG
                       //		sprintf(rgcTemp,"CONVERTED BAR TO PSI Config Recir Value : %f \n\r", fPressureRegWrite);
                       //		DEBUG_PRINTF("%s",rgcTemp);
                       //	#endif
                       //	
                       //	/* converting PSI pressure to the equivalent Current Value */
                       //	fPressureRegWrite = (((fPressureRegWrite - LOW_PRESSURE_LEVEL) * PRESSURE_REG_SLOPE_DAC) + LOW_CURRENT_VALUE);
                       //	
                       //	#ifdef GENERAL_DBG
                       //		sprintf(rgcTemp,"CONVERTED mA value SENDING TO DAC :---------------%f \n\r", fPressureRegWrite);
                       //		DEBUG_PRINTF("%s",rgcTemp);
                       //	#endif
                       
                       /* Recirculation Pressure Setting*/
                       //	fnDACValue_Set(fPressureRegWrite);
                       
                       /*Monitoring the Temperature and Pressure*/
                       /*if(((((stConfigData.fConfigBaseTempSet - stConfigData.fConfigHysAntiFrzNeg) <=
                       stCurrentData.fCurrAntifreezeLiquidTemp) && ((stConfigData.fConfigBaseTempSet +
                       stConfigData.fConfigHysAntiFrzPos) >= stCurrentData.fCurrAntifreezeLiquidTemp)) && 
                       (((stConfigData.fConfigInlineBaseTempSet - stConfigData.fConfigHysBaseNeg) <= 
                       stCurrentData.fCurrInlineBaseMatTemp) && ((stConfigData.fConfigInlineBaseTempSet + 
                       stConfigData.fConfigHysBasePos) >= stCurrentData.fCurrInlineBaseMatTemp)) &&
                       (((stConfigData.fConfigInlineHardTempSet - stConfigData.fConfigHysHardNeg) <=
                       stCurrentData.fCurrInlineHardMatTemp) && ((stConfigData.fConfigInlineHardTempSet + 
                       stConfigData.fConfigHysHardPos) >= stCurrentData.fCurrInlineHardMatTemp))) && 
                       ((fPressureRegRead >= (stConfigData.fConfigRecirPressure - 
                       stConfigData.fConfigHysRecircPresNeg)) && (fPressureRegRead <= 
                       (stConfigData.fConfigRecirPressure + stConfigData.fConfigHysRecircPresPos))))*/
                       {
                         if(RESET == StatusFlag.stStatusFlag.bWarmUpOverFlag)
                         {
                           /*Send Statu to OP that MCU is Ready for Operation Mode*/
                           stTx1Message.ExtId = MSG_TYPE_STATUS | STATUS_SYSTEM_MODE | DEST_OP | SOURCE_MCU;
                           stTx1Message.DLC = 0x01;
                           stTx1Message.Data[0] = 0x01;    /* Bit 0 0 System is Ready for Operation Mode*/
                           fnCANMsg_Transmit(&stTx1Message, CAN1);	
                           
                           StatusFlag.stStatusFlag.bWarmUpOverFlag = SET;	
#ifdef PROCESS_DBG
                           DEBUG_PRINTF("In Warm-Up Mode Temperature and Pressure Achieved\r\n");
#endif
                         }
                       }
                       /*else
                       {
                       if in the WARMUP MODE and Temp is out of range then this flag will RESET
                       so that MCU will Again send that ready for OPeration Mode
                       if(SET == StatusFlag.stStatusFlag.bWarmUpOverFlag)
                       {
                       StatusFlag.stStatusFlag.bWarmUpOverFlag = RESET;
                     }
                     }
                       */
                     }
                     
                     
                     
                     /*****************************************************************************
                     **@Function 				: fnOperation_Mode
                     **@Descriptions		: This function have the Operation
                     **@parameters			: None
                     **@return					: None
                     *****************************************************************************/
                     void fnOperation_Mode(void)
                     {
#ifdef PROCESS_DBG
                       char rgcTemp[200];
#endif
                       if(SET == StatusFlag.stStatusFlag.bOp_Mod)/*SET in parser if command rcv*/
                       {
                         /*OP will send the command to MCU and OPMM, after this command 
                         Rcvd do other operations*/
                         if(SET == StatusFlag.stStatusFlag.bOPMMSetOnPipe)/*set in parser*/
                         {
                           /*For Prime Selector Command*/
                           if(SET == StatusFlag.stStatusFlag.bPrimeSelector)
                           {	
                             /*avoid Multiple time sending a command*/
                             if(RESET == StatusFlag.stStatusFlag.bRecirculationValveOFFCmdFlag)
                             {
#ifdef PROCESS_DBG
                               DEBUG_PRINTF("PrimeSelector SEND to OPMM\r\n");
#endif
                               
                               /*Send MFM Command to start Monitoring the Material used*/ 
                               stTx1Message.ExtId = MSG_TYPE_COMMAND | CMD_LINEAR_TRANS_MONITOR |
                                 DEST_MFM | SOURCE_MCU;
                               stTx1Message.DLC = 0x01;
                               stTx1Message.Data[0] = 0x01;/*Bit 0 -> Start Monitoring the Material*/
                               
                               
                               /*OPEN Material Valves*/
                               stTx1Message.ExtId = MSG_TYPE_COMMAND | CMD_VALVE_OPMM |
                                 DEST_OPMM | SOURCE_MCU;
                               stTx1Message.DLC = 0x02;
                               stTx1Message.Data[0] = 0x01;/*Bit 0 switches ON the valves*/
                               stTx1Message.Data[1] = 0x20;/*Material Vale A/B ON */
                               fnCANMsg_Transmit(&stTx1Message, CAN1);
                               
                               
                               /*To Avoid Multiple time sending of these commands*/
                               StatusFlag.stStatusFlag.bRecirculationValveOFFCmdFlag = SET;
                             }
                             
                             
                             
                             /* Converting Spray Pressure BAR to PSI */
                             //				fPressureRegWrite = BAR_TO_PSI(stConfigData.fConfigSprayPressure);
                             //			
                             //				#ifdef PROCESS_DBG
                             //					sprintf(rgcTemp,"CONVERTED BAR TO PSI Config Spray Value : %f \n\r",
                             //										fPressureRegWrite);
                             //					DEBUG_PRINTF("%s",rgcTemp);
                             //				#endif
                             //				
                             //				/*converting PSI pressure to the equivalent Current*/
                             //				fPressureRegWrite = (((fPressureRegWrite - LOW_PRESSURE_LEVEL) * 
                             //																PRESSURE_REG_SLOPE_DAC) + LOW_CURRENT_VALUE);
                             //				
                             //				#ifdef PROCESS_DBG
                             //					sprintf(rgcTemp, "CONVERTED mA value SENDING TO DAC : %f \n\r",
                             //									fPressureRegWrite);
                             //					DEBUG_PRINTF("%s", rgcTemp);
                             //				#endif
                             //				
                             //				/* SPray Pressure Setting*/
                             //				fnDACValue_Set(fPressureRegWrite);
                             
                             /* Check for Spray Pressure*/
                             
                             if(RESET == StatusFlag.stStatusFlag.bPrimePressureAchieve)
                             {
#ifdef PROCESS_DBG
                               DEBUG_PRINTF("Prime pressure Achieved\r\n");
#endif
                               //if material pressure is spary pressure then send the Status to OP
                               stTx1Message.ExtId = MSG_TYPE_STATUS | STATUS_MATERIAL_PRESSURE |
                                 DEST_OP | SOURCE_MCU;
                               stTx1Message.DLC = 0x01;
                               stTx1Message.Data[0] = 0x01;    /*Bit 0 -> spray pressure achieved*/
                               fnCANMsg_Transmit(&stTx1Message, CAN1);
                               
                               
                               stTx1Message.ExtId = MSG_TYPE_COMMAND | CMD_PRIME_SELECTOR |
                                 DEST_OPMM | SOURCE_MCU;
                               stTx1Message.DLC = 0;
                               fnCANMsg_Transmit(&stTx1Message, CAN1);
                               /*Avoid Multiple time Tx of Status and command to OP & OPMM */
                               StatusFlag.stStatusFlag.bPrimePressureAchieve = SET;
                             }
                             
                             if(SET == StatusFlag.stStatusFlag.bPrimePosition)  /*SET in Parser*/
                             {
#ifdef PROCESS_DBG
                               DEBUG_PRINTF("Gun Position at Prime, send Prime Start cmd\r\n");
#endif
                               
                               if(SET == StatusFlag.stStatusFlag.bAutoPrimeStart)
                               {
                                 stTx1Message.Data[0] = 0x01;/*Automatic Prime*/
#ifdef PROCESS_DBG
                                 DEBUG_PRINTF("AUTO PRIME\r\n");
#endif
                               }
                               else if(SET == StatusFlag.stStatusFlag.bManualPrimeStart)
                               {
                                 stTx1Message.Data[0] = 0x02;/*Manual Prime*/
#ifdef PROCESS_DBG
                                 DEBUG_PRINTF("MANUAL PRIME\r\n");
#endif
                               }
                               if((SET == StatusFlag.stStatusFlag.bManualPrimeStart) ||
                                  (SET == StatusFlag.stStatusFlag.bAutoPrimeStart))
                               {
                                 stTx1Message.ExtId = MSG_TYPE_COMMAND | CMD_PRIME_START |
                                   DEST_OPMM | SOURCE_MCU;
                                 stTx1Message.DLC = 1;
                                 fnCANMsg_Transmit(&stTx1Message, CAN1);
                                 
                                 StatusFlag.stStatusFlag.bManualPrimeStart = RESET;
                                 StatusFlag.stStatusFlag.bAutoPrimeStart = RESET;
                               }
                             }
                             
                             if(SET == StatusFlag.stStatusFlag.bPrimeProccesStatus)
                             {
                               /*SET and RESET in in Parser */
                               if(SET == StatusFlag.stStatusFlag.bPrimeProcPass)
                               {
#ifdef PROCESS_DBG
                                 DEBUG_PRINTF("Prime Process Pass\r\n");
#endif
                                 
                               }/*SET and RESET in in Parser */
                               else if(RESET == StatusFlag.stStatusFlag.bPrimeProcPass)
                               {
#ifdef PROCESS_DBG
                                 DEBUG_PRINTF("Prime Process Fail\r\n");
#endif
                                 StatusFlag.stStatusFlag.bPrimePressureAchieve = RESET;
                                 StatusFlag.stStatusFlag.bRecirculationValveOFFCmdFlag = RESET;
                                 StatusFlag.stStatusFlag.bPrimeSelector = RESET;
                               }
                               StatusFlag.stStatusFlag.bPrimeProccesStatus = RESET;
                               StatusFlag.stStatusFlag.bPrimeProcPass = RESET;
                             }				
                           }
                           
                           
                           /*********************Spray Process********************/		
                           /*For Spray Selector Command*/
                           if(SET == StatusFlag.stStatusFlag.bSpraySelector)
                           {
                             /*avoid Multiple time sending a command*/
                             if(RESET == StatusFlag.stStatusFlag.bSprayCmdsFlag)
                             {
#ifdef PROCESS_DBG
                               DEBUG_PRINTF("Spray Selector\r\n");
#endif
                               
                               /*Send MFM Command to start Monitoring the Material used*/
                               stTx1Message.ExtId = MSG_TYPE_COMMAND | CMD_LINEAR_TRANS_MONITOR |
                                 DEST_MFM | SOURCE_MCU;
                               stTx1Message.DLC = 0x01;
                               stTx1Message.Data[0] = 0x01; /*Bit 0 -> Start Monitoring the Material*/
                               fnCANMsg_Transmit(&stTx1Message, CAN1);
                               
                               /*OPEN Material Valves*/
                               stTx1Message.ExtId = MSG_TYPE_COMMAND | CMD_VALVE_OPMM |
                                 DEST_OPMM | SOURCE_MCU;
                               stTx1Message.DLC = 0x02;
                               stTx1Message.Data[0] = 0x01;  /*ON*/
                               stTx1Message.Data[1] = 0x20;  /*Material Vale A/B*/
                               fnCANMsg_Transmit(&stTx1Message, CAN1);
                               
                               /*To Avoid Multiple time sending of these commands*/
                               StatusFlag.stStatusFlag.bSprayCmdsFlag = SET;
                             }	
                             
                             //				/* Converting Spray Pressure BAR to PSI */
                             //				fPressureRegWrite = BAR_TO_PSI(stConfigData.fConfigSprayPressure);
                             //				
                             //				#ifdef PROCESS_DBG
                             //					sprintf(rgcTemp,"CONVERTED BAR TO PSI Config Spray Value : %f \n\r",
                             //									fPressureRegWrite);
                             //					DEBUG_PRINTF("%s",rgcTemp);
                             //				#endif
                             //				
                             //				/*converting PSI pressure to the equivalent Current*/
                             //				fPressureRegWrite = (((fPressureRegWrite - LOW_PRESSURE_LEVEL) *
                             //																PRESSURE_REG_SLOPE_DAC) + LOW_CURRENT_VALUE);
                             //				
                             //				#ifdef PROCESS_DBG
                             //					sprintf(rgcTemp,"CONVERTED mA value SENDING TO DAC : %f \n\r",
                             //											fPressureRegWrite);
                             //					DEBUG_PRINTF("%s",rgcTemp);
                             //				#endif
                             //			
                             //				/* SPray Pressure Setting*/
                             //				fnDACValue_Set(fPressureRegWrite);
                             
                             /*Pressure Achieved to Soray Pressure*/
                             if(RESET == StatusFlag.stStatusFlag.bSprayPressureAchieve)
                             {
#ifdef PROCESS_DBG
                               DEBUG_PRINTF("Spray Selector to OPMM\r\n");
#endif					
                               
                               stTx1Message.ExtId = MSG_TYPE_COMMAND | CMD_SPRAY_SELECTOR |
                                 DEST_OPMM | SOURCE_MCU;
                               stTx1Message.DLC = 1;
                               if(SET == StatusFlag.stStatusFlag.bSingleCoatFlag)
                               {
                                 stTx1Message.Data[0] = 0x02;/*Single Coat*/
                               }
                               else
                               {
                                 stTx1Message.Data[0] = 0x01;/*Automatic full spray*/
                               }
                               fnCANMsg_Transmit(&stTx1Message, CAN1);
                               StatusFlag.stStatusFlag.bSprayPressureAchieve = SET;
                             }
                             
                             /* SET in Parser Msg From OPMM */
                             if(SET == StatusFlag.stStatusFlag.bSprayPosition) 
                             {
                               /* SET in Parser Msg From OP*/
                               if(SET == StatusFlag.stStatusFlag.bSprayStartCmd)
                               {
#ifdef PROCESS_DBG
                                 DEBUG_PRINTF("Gun Position at Spray send Spray \
                                              Start cmd(If Recieved from OP)\r\n");
#endif
                                                /*send command to OPMM to Start spary*/						
                                                stTx1Message.ExtId = MSG_TYPE_COMMAND | CMD_SPRAY_START |
                                                  DEST_OPMM | SOURCE_MCU;
                                              stTx1Message.DLC = 0;
                                              fnCANMsg_Transmit(&stTx1Message, CAN1);
                                              StatusFlag.stStatusFlag.bSprayPosition = RESET;
                                              StatusFlag.stStatusFlag.bSprayStartCmd = RESET;
                               }
                             }
                                              
                                              
                                              if(SET == StatusFlag.stStatusFlag.bSprayProcessStatus)
                                              {
                                                if(SET == StatusFlag.stStatusFlag.bSprayProcPass)
                                                {
#ifdef PROCESS_DBG
                                                  DEBUG_PRINTF("Spray Process Pass\r\n");
#endif
                                                }
                                                else 
                                                {
                                                  /*Wait for User*/
#ifdef PROCESS_DBG
                                                  DEBUG_PRINTF("Spray Process Fail\r\n");
#endif
                                                }
#ifdef PROCESS_DBG
                                                DEBUG_PRINTF("Send Command to Stop Monitoring LT\r\n");
#endif
                                                /*Send MFM Command to Stop Monitoring the Material used*/ 
                                                stTx1Message.ExtId = MSG_TYPE_COMMAND | CMD_LINEAR_TRANS_MONITOR |
                                                  DEST_MFM | SOURCE_MCU;
                                                stTx1Message.DLC = 0x01;
                                                stTx1Message.Data[0] = 0x02;    /*Bit 0 -> Stop Monitoring the Material Used*/	
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                StatusFlag.stStatusFlag.bSpraySelector = RESET;
                                              }
                           }	
                                              /*For Spray Selector Command*/
			else if(RESET == StatusFlag.stStatusFlag.bSpraySelector)
			{
              StatusFlag.stStatusFlag.bSprayCmdsFlag = RESET;
              StatusFlag.stStatusFlag.bSprayPressureAchieve = RESET;
              StatusFlag.stStatusFlag.bSprayProcPass = RESET;
              StatusFlag.stStatusFlag.bSprayProcessStatus = RESET;
			}
                                              
                                              
                                              /*For Flush Selector Command*/
                                              if(SET == StatusFlag.stStatusFlag.bFlushSelector)
                                              {
                                                if(RESET == StatusFlag.stStatusFlag.bTemp3Flag)
                                                {
#ifdef PROCESS_DBG
                                                  DEBUG_PRINTF("Stop Monitoring LT command Tx\r\n");
#endif
                                                  /*Closed Material Valves*/
                                                  stTx1Message.ExtId = MSG_TYPE_COMMAND | CMD_VALVE_OPMM |
                                                    DEST_OPMM | SOURCE_MCU;
                                                  stTx1Message.DLC = 0x02;
                                                  stTx1Message.Data[0] = 0x00; /*Bit 0 switches OFF the valves*/
                                                  stTx1Message.Data[1] = 0x20; /*Material Vale A/B ON */
                                                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                  
                                                  /*To Avoid Multiple time sending of these commands*/
                                                  StatusFlag.stStatusFlag.bTemp3Flag = SET;
                                                }
                                                
                                                
                                                /*Read ADC value for and set to Recirculation Pressure*/
                                                
                                                //				/* Converting Recirculation Pressure BAR to PSI */
                                                //				fPressureRegWrite = BAR_TO_PSI(stConfigData.fConfigRecirPressure);
                                                //			
                                                //				#ifdef PROCESS_DBG
                                                //					sprintf(rgcTemp,"CONVERTED BAR TO PSI Config Spray Value : %f \n\r",
                                                //									fPressureRegWrite);
                                                //					DEBUG_PRINTF("%s",rgcTemp);
                                                //				#endif
                                                //				
                                                //				/* converting PSI pressure to the equivalent Current Value */
                                                //				fPressureRegWrite = (((fPressureRegWrite - LOW_PRESSURE_LEVEL) *
                                                //																PRESSURE_REG_SLOPE_DAC) + LOW_CURRENT_VALUE);
                                                //				
                                                //				#ifdef PROCESS_DBG
                                                //					sprintf(rgcTemp,"CONVERTED mA value SENDING TO DAC : %f \n\r",
                                                //										fPressureRegWrite);
                                                //					DEBUG_PRINTF("%s",rgcTemp);
                                                //				#endif
                                                //				
                                                //				/* Recirculation Pressure Setting*/
                                                //				fnDACValue_Set(fPressureRegWrite);
                                                
                                                /*Check for Recirculation Pressure*/
                                                
                                                if(RESET == StatusFlag.stStatusFlag.bTemp2Flag)
                                                {
#ifdef PROCESS_DBG
                                                  DEBUG_PRINTF("Recirculation Pressure is Achieved\r\n");
#endif
                                                  
                                                  stTx1Message.ExtId = MSG_TYPE_COMMAND | CMD_FLUSH_SELECTOR |
                                                    DEST_OPMM | SOURCE_MCU;
                                                  stTx1Message.DLC = 0;
                                                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                  
                                                  /*Avoid Multiple calling of this block*/
                                                  StatusFlag.stStatusFlag.bTemp2Flag = SET;
                                                }
                                                
                                                if(SET == StatusFlag.stStatusFlag.bPrimePosition) 
                                                {
#ifdef PROCESS_DBG
                                                  DEBUG_PRINTF("Gun at Prime Position\r\n");
#endif
                                                  
                                                  if(SET == StatusFlag.stStatusFlag.bStartFlush_Auto)
                                                  {
                                                    stTx1Message.Data[0] = 0x01;/*Automatic Flush*/
#ifdef PROCESS_DBG
                                                    DEBUG_PRINTF("Auto Flush command\r\n");
#endif
                                                  }
                                                  if(SET == StatusFlag.stStatusFlag.bStartFlush_Manu)
                                                  {
                                                    stTx1Message.Data[0] = 0x02;/*Manual Flush*/
#ifdef PROCESS_DBG
                                                    DEBUG_PRINTF("Manual Flush command\r\n");
#endif
                                                  }
                                                  if((SET == StatusFlag.stStatusFlag.bStartFlush_Manu) ||
                                                     (SET == StatusFlag.stStatusFlag.bStartFlush_Auto))
                                                  {
                                                    stTx1Message.ExtId = MSG_TYPE_COMMAND | CMD_FLUSH_START |
                                                      DEST_OPMM | SOURCE_MCU;
                                                    stTx1Message.DLC = 0x01;
                                                    fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                    StatusFlag.stStatusFlag.bStartFlush_Manu = RESET;
                                                    StatusFlag.stStatusFlag.bStartFlush_Auto = RESET;
                                                  }
                                                }
                                                
                                                if(SET == StatusFlag.stStatusFlag.bFlushProccessStatus)
                                                {
                                                  if(SET == StatusFlag.stStatusFlag.bFlushProcPass)
                                                  {
#ifdef PROCESS_DBG
                                                    DEBUG_PRINTF("Flush Process Pass\r\n");
#endif
                                                  }
                                                  else
                                                  {
                                                    /*Wait for User*/
#ifdef PROCESS_DBG
                                                    DEBUG_PRINTF("Flush Process Fail\r\n");
#endif
                                                    StatusFlag.stStatusFlag.bTemp2Flag = RESET;
                                                    StatusFlag.stStatusFlag.bTemp3Flag = RESET;
                                                    StatusFlag.stStatusFlag.bFlushSelector = RESET;
                                                    
                                                  }
                                                  StatusFlag.stStatusFlag.bFlushProcPass = RESET;
                                                  StatusFlag.stStatusFlag.bFlushProccessStatus = RESET;
                                                }
                                              }		
                                              
                                              /*if Flush Process Success then start automatic Stall Test*/
                                              if(SET == StatusFlag.stStatusFlag.bStallTestAuto)
                                              {
                                                if(RESET == StatusFlag.stStatusFlag.bTemp4Flag)
                                                {
                                                  StatusFlag.stStatusFlag.bTemp4Flag = SET;
                                                  /* START STALL TEST*/
#ifdef PROCESS_DBG
                                                  DEBUG_PRINTF("Auto Stall command After success of flush process\r\n");
#endif
                                                  /*If Auto Pressed then send flush complete command to OPMM*/		
                                                  stTx1Message.ExtId = MSG_TYPE_COMMAND | RESERVED | 
                                                    CMD_MANUAL_PROCESS_DONE | DEST_OPMM | SOURCE_MCU;
                                                  stTx1Message.DLC = 0x01;
                                                  stTx1Message.Data[0] = 0x04;    /* Flush complete Command*/
                                                  fnCANMsg_Transmit(&stTx1Message, CAN1);		
                                                  
                                                  /*Turn off A/B Valve*/
                                                  stTx1Message.ExtId = MSG_TYPE_COMMAND | RESERVED | CMD_VALVE_OPMM |
                                                    DEST_OPMM | SOURCE_MCU;
                                                  stTx1Message.DLC = 0x02;
                                                  stTx1Message.Data[0] = 0x00;    /* OFF*/
                                                  stTx1Message.Data[1] = 0x3F;  	/*ALL VALVE*/
                                                  fnCANMsg_Transmit(&stTx1Message, CAN1);			
                                                  
                                                  stTx1Message.ExtId = MSG_TYPE_COMMAND | CMD_STALL_TEST |
                                                    DEST_MFM | SOURCE_MCU;
                                                  stTx1Message.DLC = 0x01;
                                                  stTx1Message.Data[0] = 0x01;   /*Automatic Stall test*/
                                                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                }
                                                /*Command send for Stall Test and Wait for stall Test result*/
                                                if(SET == StatusFlag.stStatusFlag.bStallTestStatus)
                                                {
                                                  StatusFlag.stStatusFlag.bStallTestStatus = RESET;
                                                  /*Operation Mode over*/
                                                  StatusFlag.stStatusFlag.bOperationModeOver = SET;
                                                  
#ifdef PROCESS_DBG
                                                  DEBUG_PRINTF("Auto Stall Test Result Came\r\n");
#endif
                                                  if((SET == StatusFlag.stStatusFlag.bLAUpstrokeStallTestPass) &&
                                                     (SET == StatusFlag.stStatusFlag.bLBUpstrokeStallTestPass) &&
                                                       (SET == StatusFlag.stStatusFlag.bLCUpstrokeStallTestPass) &&
                                                         (SET == StatusFlag.stStatusFlag.bLADnstrokeStallTestPass) &&
                                                           (SET == StatusFlag.stStatusFlag.bLBDnstrokeStallTestPass) && 
                                                             (SET == StatusFlag.stStatusFlag.bLCDnstrokeStallTestPass))
                                                  {
#ifdef PROCESS_DBG
                                                    DEBUG_PRINTF("Auto Stall Test Pass\r\n");
#endif
                                                    
                                                    StatusFlag.stStatusFlag.bWarmUpModeCmdFlag = RESET;
                                                    StatusFlag.stStatusFlag.bWarmUpOverFlag = RESET;
                                                    StatusFlag.stStatusFlag.bStallTestAuto = RESET;
                                                  }
                                                  else 
                                                  {
#ifdef PROCESS_DBG
                                                    DEBUG_PRINTF("Auto Stall Test Fail\r\n");
#endif
                                                    StatusFlag.stStatusFlag.bWarnningFlag = SET;/*Warnning SET*/
                                                    StatusFlag.stStatusFlag.bStallTestAuto = RESET;
                                                  }
                                                  /*RESET all the flags used in Stall test*/
                                                  StatusFlag.stStatusFlag.bLAUpstrokeStallTestPass = RESET;
                                                  StatusFlag.stStatusFlag.bLBUpstrokeStallTestPass = RESET;
                                                  StatusFlag.stStatusFlag.bLCUpstrokeStallTestPass = RESET;
                                                  StatusFlag.stStatusFlag.bLADnstrokeStallTestPass = RESET;
                                                  StatusFlag.stStatusFlag.bLBDnstrokeStallTestPass = RESET;
                                                  StatusFlag.stStatusFlag.bLCDnstrokeStallTestPass = RESET;
                                                  StatusFlag.stStatusFlag.bTemp4Flag = RESET;
                                                }	
                                              }
                                              
                                              
                                              
                                              if(((stConfigData.fConfigOperationTime * 100) <= uiOperationTimeOut) &&
                                                 (RESET == StatusFlag.stStatusFlag.bTemp8Flag))
                                              {
                                                uiOperationTimeOut = 0;
                                                
#ifdef PROCESS_DBG
                                                DEBUG_PRINTF("Operation Time Out\r\n");
#endif
                                                
                                                /*If Operation timeout over then MCU will switch to Warmup mode*/
                                                StatusFlag.stStatusFlag.bOperationModeOver = SET;
                                                StatusFlag.stStatusFlag.bErrorFlag = SET; /*Error occurs*/
                                                stTx1Message.DLC = 0;
                                                stTx1Message.ExtId = MSG_TYPE_FAULT | FAULT_OPERATION_TIME_OUT |
                                                  DEST_OP | SOURCE_MCU;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);				
                                                StatusFlag.stStatusFlag.bTemp8Flag = SET;
                                                /* FAULT BYTE UPDATED and WRITTEN in NVSRAM */
                                                ucFaultByte0 = (ucFaultByte0 | BIT0_SET); 
                                                fnFault_Log();
                                              }
                                              
                         }
                       }
                                              if(SET == StatusFlag.stStatusFlag.bStStallTestManual)
                                              {
                                                if(RESET == StatusFlag.stStatusFlag.bTemp5Flag)
                                                {
#ifdef PROCESS_DBG
                                                  DEBUG_PRINTF("Manual Stall Test Cmd\r\n");
#endif
                                                  StatusFlag.stStatusFlag.bTemp5Flag = SET;
                                                  
                                                  /*Turn off A/B Valve*/
                                                  stTx1Message.ExtId = MSG_TYPE_COMMAND | RESERVED | CMD_VALVE_OPMM |
                                                    DEST_OPMM | SOURCE_MCU;
                                                  stTx1Message.DLC = 0x02;
                                                  stTx1Message.Data[0] = 0x00;    /* OFF*/
                                                  stTx1Message.Data[1] = 0x20;  	/*Valve A/B*/
                                                  fnCANMsg_Transmit(&stTx1Message, CAN1);		
                                                  
                                                  stTx1Message.ExtId = MSG_TYPE_COMMAND | CMD_STALL_TEST |
                                                    DEST_MFM | SOURCE_MCU;
                                                  stTx1Message.DLC = 0x01;
                                                  stTx1Message.Data[0] = 0x02;    /*Stall Test Manual*/
                                                  fnCANMsg_Transmit(&stTx1Message, CAN1);	
                                                  
                                                }
                                                /*wait for stall test result*/
                                                if(SET == StatusFlag.stStatusFlag.bStallTestStatus)
                                                {
                                                  StatusFlag.stStatusFlag.bStallTestStatus = RESET;
                                                  
#ifdef PROCESS_DBG
                                                  DEBUG_PRINTF("Got Manual Stall Result\r\n");
#endif
                                                  if((SET == StatusFlag.stStatusFlag.bLAUpstrokeStallTestPass) &&
                                                     (SET == StatusFlag.stStatusFlag.bLBUpstrokeStallTestPass) &&
                                                       (SET == StatusFlag.stStatusFlag.bLCUpstrokeStallTestPass) &&	
                                                         (SET == StatusFlag.stStatusFlag.bLADnstrokeStallTestPass) &&
                                                           (SET == StatusFlag.stStatusFlag.bLBDnstrokeStallTestPass) && 
                                                             (SET == StatusFlag.stStatusFlag.bLCDnstrokeStallTestPass))
                                                  {
#ifdef PROCESS_DBG
                                                    DEBUG_PRINTF("Manual Stall Test Pass\r\n");
#endif
                                                    /*Stall test Pass*/
                                                    ucStalFailureCount = 0;
                                                    /*RESET the Warmup Mode Flag*/
                                                    StatusFlag.stStatusFlag.bWarmUpModeCmdFlag = RESET;
                                                    StatusFlag.stStatusFlag.bWarmUpOverFlag = RESET;
                                                    
                                                    StatusFlag.stStatusFlag.bStStallTestManual = RESET; 
                                                  }
                                                  else
                                                  {
                                                    ucStalFailureCount++;
                                                    
#ifdef PROCESS_DBG
                                                    DEBUG_PRINTF("Manual Stall Test Fail %d\r\n",ucStalFailureCount);
#endif
                                                    
                                                    StatusFlag.stStatusFlag.bStStallTestManual = RESET; /*RESET the Manual stall*/
                                                    if(ucStalFailureCount >= 2)
                                                    {
#ifdef PROCESS_DBG
                                                      DEBUG_PRINTF("Manual Stall Test Fail Defective Pump Declaire\r\n",
                                                                   ucStalFailureCount);
#endif
                                                      StatusFlag.stStatusFlag.bErrorFlag = SET; /*Error SET*/
                                                      ucStalFailureCount=0;
                                                      /*message to OP for defective pump*/
                                                      stTx1Message.ExtId = MSG_TYPE_FAULT | FAULT_DEFECTIVE_PUMP |
                                                        DEST_OP | SOURCE_MCU;
                                                      stTx1Message.DLC = 0;
                                                      fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                      StatusFlag.stStatusFlag.bWarmUpModeCmdFlag = RESET;
                                                      StatusFlag.stStatusFlag.bWarmUpOverFlag = RESET;
                                                      ucMCUMode = MCU_WARMUP_MODE;
                                                      /*Defective Pump*/
                                                      /* FAULT BYTE UPDATED and WRITTEN in NVSRAM */
                                                      ucFaultByte2 = (ucFaultByte2 | BIT0_SET);  
                                                      fnFault_Log();
                                                      
                                                    }
                                                    else 
                                                    {
                                                      StatusFlag.stStatusFlag.bWarnningFlag = SET; /*Warnning SET*/
                                                    }
                                                  }
                                                  /*RESET all the flags used in Stall test*/
                                                  StatusFlag.stStatusFlag.bLAUpstrokeStallTestPass = RESET;
                                                  StatusFlag.stStatusFlag.bLBUpstrokeStallTestPass = RESET;
                                                  StatusFlag.stStatusFlag.bLCUpstrokeStallTestPass = RESET;
                                                  StatusFlag.stStatusFlag.bLADnstrokeStallTestPass = RESET;
                                                  StatusFlag.stStatusFlag.bLBDnstrokeStallTestPass = RESET;
                                                  StatusFlag.stStatusFlag.bLCDnstrokeStallTestPass = RESET;
                                                  StatusFlag.stStatusFlag.bTemp5Flag = RESET;
                                                }
                                              }
                                              
                                              /*Send status to OP and RESET the Operation Mode*/
                                              if(SET == StatusFlag.stStatusFlag.bOperationModeOver)
                                              {
#ifdef PROCESS_DBG
                                                DEBUG_PRINTF("Operation Mode over switch from operation to warmup\r\n");
#endif
                                                uiOperationTimeOut = 0; /*Reset the Opertion timeout Conter*/
                                                StatusFlag.stStatusFlag.bOperationModeOver = RESET;
                                                
                                                StatusFlag.stStatusFlag.bOPMMSetOnPipe = RESET;	
                                                StatusFlag.stStatusFlag.bTemp2Flag = RESET;
                                                StatusFlag.stStatusFlag.bTemp3Flag = RESET;
                                                StatusFlag.stStatusFlag.bTemp4Flag = RESET;
                                                StatusFlag.stStatusFlag.bTemp5Flag = RESET;
                                                /*RESET the flag for fault of material temp out of range*/
                                                StatusFlag.stStatusFlag.bTemp6Flag = RESET;
                                                StatusFlag.stStatusFlag.bTemp7Flag = RESET;
                                                StatusFlag.stStatusFlag.bTemp8Flag = RESET;
                                                StatusFlag.stStatusFlag.bWarmUpOverFlag = RESET;
                                                StatusFlag.stStatusFlag.bWarmUpModeCmdFlag = RESET;
                                                StatusFlag.stStatusFlag.bPrimePressureAchieve = RESET;
                                                StatusFlag.stStatusFlag.bRecirculationValveOFFCmdFlag = RESET;
                                                StatusFlag.stStatusFlag.bSprayCmdsFlag = RESET;			
                                                StatusFlag.stStatusFlag.bPrimeProccesStatus = RESET; 
                                                StatusFlag.stStatusFlag.bSprayPressureAchieve = RESET;
                                                StatusFlag.stStatusFlag.bSprayProcPass = RESET;
                                                StatusFlag.stStatusFlag.bSprayProcessStatus = RESET;
                                                StatusFlag.stStatusFlag.bFlushSelector = RESET; 
                                                StatusFlag.stStatusFlag.bFlushProccessStatus = RESET;
                                                StatusFlag.stStatusFlag.bStallTestAuto = RESET; 
                                                
                                                /*Status log after a joint completed*/
                                                fnStatus_Log();
                                              }	
                     }
                                              
                                              /*****************************************************************************
                                              **@Function 			: fnSendOPConfig
                                              **@Descriptions	: Sends configuration msg to OP
                                              **@parameters		: None
                                              **@return				: None
                                              *****************************************************************************/
                                              void fnSendOPConfig(void)
                                              {
                                                uint8_t ucCount;
                                                /*SET variable for configuration send to the OP*/
                                                uiDeviceConfigSent |= (1 << OP);
                                                
                                                
                                                stTx1Message.DLC = ONE_BYTES;
                                                stTx1Message.Data[0] = stConfigData.ucTypeofUnit;
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_TYPE_OF_UNIT |
                                                  DEST_OP | SOURCE_MCU;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                
                                                /*Spray pressure value*/
                                                stTx1Message.DLC = 4;
                                                SplitByte.fData = stConfigData.fConfigSprayPressure;
                                                for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_SPRAY_PRESSURE |
                                                  DEST_OP | SOURCE_MCU;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /************************************************************/	
                                                
                                                stTx1Message.DLC = FOUR_BYTES;
                                                SplitByte.fData = stConfigData.fConfigRecirPressure;
                                                for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_RECIRC_PRESSURE |
                                                  DEST_OP | SOURCE_MCU;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /* Pipe Temperature (Non - contact) HIGH threshold*/
                                                stTx1Message.DLC = FIVE_BYTES;
                                                SplitByte.fData = stConfigData.fConfigPipeTempHTH;
                                                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt - 1];
                                                }
                                                stTx1Message.Data[0] = BYTE_HIGH;
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_PIPE_TEMP_TH |
                                                  DEST_OP | SOURCE_MCU;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /*Pipe Temperature (Non - contact) LOW threshold*/
                                                stTx1Message.DLC = FIVE_BYTES;
                                                SplitByte.fData = stConfigData.fConfigPipeTempLTH;
                                                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt - 1];
                                                }
                                                stTx1Message.Data[0] = BYTE_LOW;
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_PIPE_TEMP_TH |
                                                  DEST_OP | SOURCE_MCU;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /*For OPMM Spraying Count*/
                                                stTx1Message.DLC = ONE_BYTES;
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_SPRAY_COUNT |
                                                  DEST_OP | SOURCE_MCU;
                                                stTx1Message.Data[0] = stConfigData.ucConfigSprayCount;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /*For OPMM Prime Time*/
                                                stTx1Message.DLC = FOUR_BYTES;
                                                SplitByte.fData = stConfigData.fConfigPrimeTime;
                                                for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_PRIME_TIME |
                                                  DEST_OP | SOURCE_MCU;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /*For OPMM Gun Delay Time*/
                                                stTx1Message.DLC = FOUR_BYTES;
                                                SplitByte.fData = stConfigData.fConfigGunDelayTime;
                                                for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_OPMM_GUN_DELAY |
                                                  DEST_OP | SOURCE_MCU;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /*For OPMM Spray Ovelap Time*/
                                                stTx1Message.DLC = FOUR_BYTES;
                                                SplitByte.fData = stConfigData.fConfigSprayOverlapTime;
                                                for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_OPMM_SPRAY_OVERLAP_TIME |
                                                  DEST_OP | SOURCE_MCU;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /*For OPMM Reverse Delay Time*/
                                                stTx1Message.DLC = FOUR_BYTES;
                                                SplitByte.fData = stConfigData.fConfigReverseDelayTime;
                                                for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_OPMM_REVERSE_DELAY |
                                                  DEST_OP | SOURCE_MCU;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /*For OPMM Flush Time*/
                                                stTx1Message.DLC = FOUR_BYTES;
                                                SplitByte.fData = stConfigData.fConfigFlushTime;
                                                for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_OPMM_FLUSH_TIME |
                                                  DEST_OP | SOURCE_MCU;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /* For Stall Threshold in% (Downstroke MFM)*/
                                                stTx1Message.DLC = THREE_BYTES;
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_MFM_STALL_TH |
                                                  DEST_OP | SOURCE_MCU;
                                                stTx1Message.Data[0] = BYTE_LOW;
                                                SplitByte.iData = stConfigData.iConfigDownStrokeTH;
                                                stTx1Message.Data[1] = SplitByte.rgucData[0];
                                                stTx1Message.Data[2] = SplitByte.rgucData[1];
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /*For Stall Threshold in% (Upstroke MFM)*/
                                                stTx1Message.DLC = THREE_BYTES;
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_MFM_STALL_TH |
                                                  DEST_OP | SOURCE_MCU;
                                                stTx1Message.Data[0] = BYTE_HIGH;
                                                SplitByte.iData = stConfigData.iConfigUpStrokeTH;
                                                stTx1Message.Data[1] = SplitByte.rgucData[0];
                                                stTx1Message.Data[2] = SplitByte.rgucData[1];
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /*For Pressure gauge 1  Threshold (Low TH MFM)*/
                                                stTx1Message.DLC = FIVE_BYTES;
                                                SplitByte.fData = stConfigData.fConfigPressureGauge1LTH;
                                                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt - 1];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_MFM_PGAUGE1_TH |
                                                  DEST_OP | SOURCE_MCU;
                                                stTx1Message.Data[0] = BYTE_LOW;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /*For Pressure gauge 1  Threshold (High TH MFM)*/
                                                stTx1Message.DLC = FIVE_BYTES;
                                                SplitByte.fData = stConfigData.fConfigPressureGauge1HTH;
                                                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt - 1];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_MFM_PGAUGE1_TH |
                                                  DEST_OP | SOURCE_MCU;
                                                stTx1Message.Data[0] = BYTE_HIGH;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /* For Pressure gauge 2 Threshold (Low TH MFM)*/
                                                stTx1Message.DLC = FIVE_BYTES;
                                                SplitByte.fData = stConfigData.fConfigPressureGauge2LTH;
                                                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt - 1];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_MFM_PGAUGE2_TH |
                                                  DEST_OP | SOURCE_MCU;
                                                stTx1Message.Data[0] = BYTE_LOW;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                //		fnDelay_ms(1);
                                                
                                                /* For Pressure gauge 2 Threshold (High TH MFM) */
                                                stTx1Message.DLC = FIVE_BYTES;
                                                SplitByte.fData = stConfigData.fConfigPressureGauge2HTH;
                                                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt - 1];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_MFM_PGAUGE2_TH |
                                                  DEST_OP | SOURCE_MCU;
                                                stTx1Message.Data[0] = BYTE_HIGH;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /*For Pressure gauge 3 Threshold (Low TH MFM)*/
                                                stTx1Message.DLC = FIVE_BYTES;
                                                SplitByte.fData = stConfigData.fConfigPressureGauge3LTH;
                                                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt - 1];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_MFM_PGAUGE3_TH |
                                                  DEST_OP | SOURCE_MCU;
                                                stTx1Message.Data[0] = 0;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /*For Pressure gauge 3 Threshold (High TH MFM)*/
                                                stTx1Message.DLC = FIVE_BYTES;
                                                SplitByte.fData = stConfigData.fConfigPressureGauge3HTH;
                                                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt - 1];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_MFM_PGAUGE3_TH |
                                                  DEST_OP | SOURCE_MCU;
                                                stTx1Message.Data[0] = 0x01;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /*Number of cylinder (MFM)*/
                                                stTx1Message.DLC = ONE_BYTES;
                                                stTx1Message.Data[0] = stConfigData.ucConfigCylinderCount;
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_NUMBER_OF_CYLINDER |
                                                  DEST_OP | SOURCE_MCU;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /*For Base tank temperature setting (BHCM)*/
                                                stTx1Message.DLC = FOUR_BYTES;
                                                SplitByte.fData = stConfigData.fConfigBaseTempSet;
                                                for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_BHCM_BTANK_SET_TEMP |
                                                  DEST_OP | SOURCE_MCU;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /*For Base tank temperature threshold (BHCM Low TH)*/
                                                stTx1Message.DLC = FIVE_BYTES;
                                                SplitByte.fData = stConfigData.fConfigBaseTempLTH;
                                                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt - 1];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_BHCM_BTANK_TEMP_TH |
                                                  DEST_OP | SOURCE_MCU;
                                                stTx1Message.Data[0] = BYTE_LOW;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /*For Base tank temperature threshold (BHCM High TH)*/
                                                stTx1Message.DLC = FIVE_BYTES;
                                                SplitByte.fData = stConfigData.fConfigBaseTempHTH;
                                                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt - 1];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_BHCM_BTANK_TEMP_TH |
                                                  DEST_OP | SOURCE_MCU;
                                                stTx1Message.Data[0] = BYTE_HIGH;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                //				fnDelay_ms(1);
                                                
                                                /*For Base tank liquid level threshold (BHCM Low TH)*/
                                                stTx1Message.DLC = FIVE_BYTES;
                                                SplitByte.fData = stConfigData.fConfigBaseLiquidLevelLTH;
                                                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt - 1];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_BHCM_BTANK_LIQUID_TH |
                                                  DEST_OP | SOURCE_MCU;
                                                stTx1Message.Data[0] = BYTE_LOW;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);	
                                                
                                                /*For Base tank liquid level threshold (BHCM High TH)*/
                                                stTx1Message.DLC = FIVE_BYTES;
                                                SplitByte.fData = stConfigData.fConfigBaseLiquidLevelHTH;
                                                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt - 1];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_BHCM_BTANK_LIQUID_TH |
                                                  DEST_OP | SOURCE_MCU;
                                                stTx1Message.Data[0] = BYTE_HIGH;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /*For Base Inline material temperature setting (IHCM base)*/
                                                stTx1Message.DLC = FOUR_BYTES;
                                                SplitByte.fData = stConfigData.fConfigInlineBaseTempSet;
                                                for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_IHCM_BLINE_SET_TEMP |
                                                  DEST_OP | SOURCE_MCU;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /*For Base inline material temperature LOW thresholds (IHCM Base)*/
                                                stTx1Message.DLC = FIVE_BYTES;
                                                SplitByte.fData = stConfigData.fConfigInlineBaseTempLTH;
                                                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt - 1];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_IHCM_BLINE_TEMP_TH |
                                                  DEST_OP | SOURCE_MCU;
                                                stTx1Message.Data[0] = BYTE_LOW;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /*For Base inline material temperature HIGH thresholds (IHCM Base)*/
                                                stTx1Message.DLC = FIVE_BYTES;
                                                SplitByte.fData = stConfigData.fConfigInlineBaseTempHTH;
                                                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt - 1];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_IHCM_BLINE_TEMP_TH |
                                                  DEST_OP | SOURCE_MCU;
                                                stTx1Message.Data[0] = BYTE_HIGH;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /*Hardener Inline temperature setting (IHCM Hardner)*/
                                                stTx1Message.DLC = FOUR_BYTES;
                                                SplitByte.fData = stConfigData.fConfigInlineHardTempSet;
                                                for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_IHCM4_HLINE_SET_TEMP |
                                                  DEST_OP | SOURCE_MCU;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /*Hardener Inline  temperature LOW threshold (IHCM Hardner)*/
                                                stTx1Message.DLC = FIVE_BYTES;
                                                SplitByte.fData = stConfigData.fConfigInlineHardTempLTH;
                                                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt - 1];
                                                } 
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_IHCM4_HLINE_TEMP_TH |
                                                  DEST_OP | SOURCE_MCU;
                                                stTx1Message.Data[0] = BYTE_LOW;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /*Hardener Inline  temperature HIGH threshold (IHCM Hardner)*/
                                                stTx1Message.DLC = FIVE_BYTES;
                                                SplitByte.fData = stConfigData.fConfigInlineHardTempHTH;
                                                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt - 1];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_IHCM4_HLINE_TEMP_TH |
                                                  DEST_OP | SOURCE_MCU;
                                                stTx1Message.Data[0] = BYTE_HIGH;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /*Operation Time*/
                                                stTx1Message.DLC = FOUR_BYTES;
                                                SplitByte.fData = stConfigData.fConfigOperationTime;
                                                for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_OPERATION_TIME |
                                                  DEST_OP | SOURCE_MCU;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                //				fnDelay_ms(1);
                                                
                                                /*Joint Number*/
                                                stTx1Message.DLC = FOUR_BYTES;
                                                SplitByte.fData = stConfigData.fConfigJointNum;
                                                for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_JOINT_NUMBER |
                                                  DEST_OP | SOURCE_MCU;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /*Data Log Period*/
                                                stTx1Message.DLC = FOUR_BYTES;
                                                SplitByte.fData = stConfigData.fConfigDataLoggingPeriod;
                                                for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_DATA_LOG_PERIOD |
                                                  DEST_OP | SOURCE_MCU;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /* Type of 4th IHCM*/
                                                stTx1Message.DLC = ONE_BYTES;
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_TYPE_IHCM4 |
                                                  DEST_OP | SOURCE_MCU;
                                                stTx1Message.Data[0] = stConfigData.ucConfigIHCM4Type;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /* Date and Time Log*/
                                                stTx1Message.DLC = SEVEN_BYTES;
                                                for(ucCount = 0; ucCount < stTx1Message.DLC; ucCount++)
                                                {
                                                  stTx1Message.Data[ucCount] = stConfigData.ucConfigSystemTime[ucCount];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_SYS_DATE_TIME |
                                                  DEST_OP | SOURCE_MCU;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /* Surface area of cylinder (MFM cylinder A) */
                                                stTx1Message.DLC = FIVE_BYTES;
                                                SplitByte.fData = stConfigData.fConfigAreaofCylinderA;
                                                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = 
                                                    SplitByte.rgucData[ucLoopCnt - 1];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_CYLINDER_SURFACE_AREA |
                                                  DEST_OP | SOURCE_MCU;
                                                stTx1Message.Data[0] = CYLINDER_A ;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);	
                                                
                                                /* Surface area of cylinder (MFM cylinder B)*/
                                                stTx1Message.DLC = FIVE_BYTES;
                                                SplitByte.fData = stConfigData.fConfigAreaofCylinderB;
                                                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = 
                                                    SplitByte.rgucData[ucLoopCnt - 1];
                                                } 
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_CYLINDER_SURFACE_AREA |
                                                  DEST_OP | SOURCE_MCU;
                                                stTx1Message.Data[0] = CYLINDER_B;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);	
                                                
                                                /*Surface area of cylinder (MFM cylinder C)*/
                                                stTx1Message.DLC = FIVE_BYTES;
                                                SplitByte.fData = stConfigData.fConfigAreaofCylinderC;
                                                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = 
                                                    SplitByte.rgucData[ucLoopCnt - 1];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_CYLINDER_SURFACE_AREA |
                                                  DEST_OP | SOURCE_MCU;
                                                stTx1Message.Data[0] = CYLINDER_C;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);	
                                                
                                                
                                                /*Supervisor Name1*/
                                                stTx1Message.DLC = EIGHT_BYTES;
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_SUPERVISOR_NAME |
                                                  DEST_OP | SOURCE_MCU;
                                                stTx1Message.Data[0] = FRAME1;
                                                for(ucCount = 1; ucCount < stTx1Message.DLC;  ucCount++)
                                                {
                                                  stTx1Message.Data[ucCount] = 
                                                    stConfigData.rgucConfigSupervisorName1[ucCount - 1];
                                                }
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /* Supervisor Name2*/
                                                stTx1Message.DLC = EIGHT_BYTES;
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_SUPERVISOR_NAME |
                                                  DEST_OP | SOURCE_MCU;
                                                stTx1Message.Data[0] = FRAME2;
                                                for(ucCount = 1; ucCount < stTx1Message.DLC;  ucCount++)
                                                {
                                                  stTx1Message.Data[ucCount] = 
                                                    stConfigData.rgucConfigSupervisorName2[ucCount - 1];
                                                }
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /* Supervisor Name3*/
                                                stTx1Message.DLC = EIGHT_BYTES;
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_SUPERVISOR_NAME |
                                                  DEST_OP | SOURCE_MCU;
                                                stTx1Message.Data[0] = FRAME3;
                                                for(ucCount = 1; ucCount < stTx1Message.DLC;  ucCount++)
                                                {
                                                  stTx1Message.Data[ucCount] = 
                                                    stConfigData.rgucConfigSupervisorName3[ucCount - 1];
                                                }
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /* Coating Material1*/
                                                stTx1Message.DLC = EIGHT_BYTES;
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_COATING_MATERIAL |
                                                  DEST_OP | SOURCE_MCU;
                                                stTx1Message.Data[0] = FRAME1;
                                                for(ucCount = 1; ucCount < stTx1Message.DLC;  ucCount++)
                                                {
                                                  stTx1Message.Data[ucCount] = 
                                                    stConfigData.rgucConfigCoatingMatName1[ucCount - 1];
                                                }
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /*Coating Material2*/
                                                stTx1Message.DLC = EIGHT_BYTES;
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_COATING_MATERIAL |
                                                  DEST_OP | SOURCE_MCU;
                                                stTx1Message.Data[0] = FRAME2;
                                                for(ucCount = 1; ucCount < stTx1Message.DLC;  ucCount++)
                                                {
                                                  stTx1Message.Data[ucCount] = 
                                                    stConfigData.rgucConfigCoatingMatName2[ucCount - 1];
                                                }
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /*Coating Material3*/
                                                stTx1Message.DLC = EIGHT_BYTES;
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_COATING_MATERIAL |
                                                  DEST_OP | SOURCE_MCU;
                                                stTx1Message.Data[0] = FRAME3;
                                                for(ucCount = 1; ucCount < stTx1Message.DLC;  ucCount++)
                                                {
                                                  stTx1Message.Data[ucCount] = 
                                                    stConfigData.rgucConfigCoatingMatName3[ucCount - 1];
                                                }
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /*Base Material Batch No.1*/
                                                stTx1Message.DLC = EIGHT_BYTES;
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_BASE_MAT_BATCH_NO |
                                                  DEST_OP | SOURCE_MCU;
                                                stTx1Message.Data[0] = FRAME1;
                                                for(ucCount = 1; ucCount < stTx1Message.DLC;  ucCount++)
                                                {
                                                  stTx1Message.Data[ucCount] = 
                                                    stConfigData.rgucConfigBaseMatBatchNo1[ucCount - 1];
                                                }
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /* Base Material Batch No.2*/
                                                stTx1Message.DLC = EIGHT_BYTES;
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_BASE_MAT_BATCH_NO |
                                                  DEST_OP | SOURCE_MCU;
                                                stTx1Message.Data[0] = FRAME2;
                                                for(ucCount = 1; ucCount < stTx1Message.DLC;  ucCount++)
                                                {
                                                  stTx1Message.Data[ucCount] = 
                                                    stConfigData.rgucConfigBaseMatBatchNo2[ucCount - 1];
                                                }
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /*Base Material Batch No.3*/
                                                stTx1Message.DLC = EIGHT_BYTES;
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_BASE_MAT_BATCH_NO |
                                                  DEST_OP | SOURCE_MCU;
                                                stTx1Message.Data[0] = FRAME3;
                                                for(ucCount = 1; ucCount < EIGHT_BYTES;  ucCount++)
                                                {
                                                  stTx1Message.Data[ucCount] = 
                                                    stConfigData.rgucConfigBaseMatBatchNo3[ucCount - 1];
                                                }
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /*Hardner Material Batch No.1*/
                                                stTx1Message.DLC = EIGHT_BYTES;
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_HARD_MAT_BATCH_NO |
                                                  DEST_OP | SOURCE_MCU;
                                                stTx1Message.Data[0] = FRAME1;
                                                for(ucCount = 1; ucCount < stTx1Message.DLC ;  ucCount++)
                                                {
                                                  stTx1Message.Data[ucCount] =
                                                    stConfigData.rgucConfigHardMatBatchNo1[ucCount - 1];
                                                }
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /* Hardner Material Batch No.2*/
                                                stTx1Message.DLC = EIGHT_BYTES;
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_HARD_MAT_BATCH_NO |
                                                  DEST_OP | SOURCE_MCU;
                                                stTx1Message.Data[0] = FRAME2;
                                                for(ucCount = 1; ucCount < stTx1Message.DLC;  ucCount++)
                                                {
                                                  stTx1Message.Data[ucCount] = 
                                                    stConfigData.rgucConfigHardMatBatchNo2[ucCount - 1];
                                                }
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /*Hardner Material Batch No.3*/
                                                stTx1Message.DLC = EIGHT_BYTES;
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_HARD_MAT_BATCH_NO | 
                                                  DEST_OP | SOURCE_MCU;
                                                stTx1Message.Data[0] = FRAME3;
                                                for(ucCount = 1; ucCount < stTx1Message.DLC;  ucCount++)
                                                {
                                                  stTx1Message.Data[ucCount] = 
                                                    stConfigData.rgucConfigHardMatBatchNo3[ucCount - 1];
                                                }
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /*Time before stall*/
                                                stTx1Message.DLC = FOUR_BYTES;
                                                SplitByte.fData = stConfigData.fConfigTimeBeforeStall;
                                                for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_TIME_BEFORE_STALL |
                                                  DEST_OP | SOURCE_MCU;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /*Time before comparing stall pressure*/
                                                stTx1Message.DLC = FOUR_BYTES;
                                                SplitByte.fData = stConfigData.fConfigTimeCompareStall;
                                                for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_TIME_BEFORE_COMPARE_STALL |
                                                  DEST_OP | SOURCE_MCU;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /*DCLS reach timer*/
                                                stTx1Message.DLC = FIVE_BYTES;	
                                                SplitByte.fData = stConfigData.fConfigDCLSHomeTime;
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_DCLS_HOME_TIME |
                                                  DEST_OP | SOURCE_MCU;
                                                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = 
                                                    SplitByte.rgucData[ucLoopCnt - 1];
                                                }
                                                stTx1Message.Data[0] = BYTE_DCLS;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /*HOME reach timer*/
                                                stTx1Message.DLC = FIVE_BYTES;	
                                                SplitByte.fData = stConfigData.fConfigHomeDCLSTime;
                                                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = 
                                                    SplitByte.rgucData[ucLoopCnt - 1];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_DCLS_HOME_TIME |
                                                  DEST_OP | SOURCE_MCU;
                                                stTx1Message.Data[0] = BYTE_HLS;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /*Gun position change time*/
                                                stTx1Message.DLC = FOUR_BYTES;
                                                SplitByte.fData = stConfigData.fConfigGunPosChangeTime;
                                                for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_GUN_POSITION_CHANGE_TIME |
                                                  DEST_OP | SOURCE_MCU;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /*Delay time befor turning ON Spray valve(For Prime and Flush)*/
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_SPRAY_VALVE_OPEN_TIME_VBAT |
                                                  DEST_OP | SOURCE_MCU;
                                                stTx1Message.DLC = FOUR_BYTES;
                                                SplitByte.fData = stConfigData.fConfigTimebeforeONSprayValve;
                                                for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt];
                                                }
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                
                                                
                                                /*Temperature Hystesis*/
                                                stTx1Message.DLC = FIVE_BYTES;	
                                                SplitByte.fData = stConfigData.fConfigHysBasePos;
                                                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt - 1];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_TEMP_HYSTERESIS |
                                                  DEST_OP | SOURCE_MCU;
                                                stTx1Message.Data[0] = BIT0_SET;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                stTx1Message.DLC = FIVE_BYTES;	
                                                SplitByte.fData = stConfigData.fConfigHysBaseNeg;
                                                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = 
                                                    SplitByte.rgucData[ucLoopCnt - 1];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_TEMP_HYSTERESIS | 
                                                  DEST_OP | SOURCE_MCU;
                                                stTx1Message.Data[0] = BIT1_SET;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                stTx1Message.DLC = FIVE_BYTES;	
                                                SplitByte.fData = stConfigData.fConfigHysHardPos;
                                                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = 
                                                    SplitByte.rgucData[ucLoopCnt - 1];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_TEMP_HYSTERESIS |
                                                  DEST_OP | SOURCE_MCU;
                                                stTx1Message.Data[0] = BIT2_SET;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                stTx1Message.DLC = FIVE_BYTES;	
                                                SplitByte.fData = stConfigData.fConfigHysHardNeg;
                                                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = 
                                                    SplitByte.rgucData[ucLoopCnt - 1];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_TEMP_HYSTERESIS |
                                                  DEST_OP | SOURCE_MCU;
                                                stTx1Message.Data[0] = BIT3_SET;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                stTx1Message.DLC = FIVE_BYTES;	
                                                SplitByte.fData = stConfigData.fConfigHysAntiFrzPos;
                                                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = 
                                                    SplitByte.rgucData[ucLoopCnt - 1];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_TEMP_HYSTERESIS |
                                                  DEST_OP | SOURCE_MCU;
                                                stTx1Message.Data[0] = BIT4_SET;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                stTx1Message.DLC = FIVE_BYTES;	
                                                SplitByte.fData = stConfigData.fConfigHysAntiFrzPos;
                                                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt - 1];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_TEMP_HYSTERESIS |
                                                  DEST_OP | SOURCE_MCU;
                                                stTx1Message.Data[0] = BIT5_SET;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                
                                                
                                                /*Pressure Hysteresis*/
                                                stTx1Message.DLC = FIVE_BYTES;	
                                                SplitByte.fData = stConfigData.fConfigHysRecircPresPos;
                                                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt - 1];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_PRESSURE_HYSTERESIS |
                                                  DEST_OP | SOURCE_MCU;
                                                stTx1Message.Data[0] = BIT0_SET;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                stTx1Message.DLC = FIVE_BYTES;	
                                                SplitByte.fData = stConfigData.fConfigHysRecircPresNeg;
                                                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt - 1];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_PRESSURE_HYSTERESIS |
                                                  DEST_OP | SOURCE_MCU;
                                                stTx1Message.Data[0] = BIT1_SET;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                stTx1Message.DLC = FIVE_BYTES;	
                                                SplitByte.fData = stConfigData.fConfigHysSprayPresPos;
                                                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt - 1];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_PRESSURE_HYSTERESIS |
                                                  DEST_OP | SOURCE_MCU;
                                                stTx1Message.Data[0] = BIT2_SET;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                stTx1Message.DLC = FIVE_BYTES;	
                                                SplitByte.fData = stConfigData.fConfigHysSprayPresNeg;
                                                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt - 1];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_PRESSURE_HYSTERESIS |
                                                  DEST_OP | SOURCE_MCU;
                                                stTx1Message.Data[0] = BIT3_SET;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /* HHCM liquid Thershold config*/
                                                stTx1Message.DLC = FIVE_BYTES;
                                                SplitByte.fData = stConfigData.fConfigHardLiquidLevelLTH;
                                                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt - 1];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_HHCM_LIQUID_LEVEL_TH |
                                                  DEST_OP | SOURCE_MCU;
                                                stTx1Message.Data[0] = 0x00;  
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                
                                                /* HHCM liquid Thershold config*/
                                                stTx1Message.DLC = FIVE_BYTES;
                                                SplitByte.fData = stConfigData.fConfigHardLiquidLevelHTH;
                                                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt - 1];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_HHCM_LIQUID_LEVEL_TH |
                                                  DEST_OP | SOURCE_MCU;
                                                stTx1Message.Data[0] = 0x01;  
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /*Base Tank Height*/
                                                stTx1Message.DLC = FIVE_BYTES;
                                                SplitByte.fData = stConfigData.fConfigBaseTankHeight;
                                                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt - 1];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_HCM_LIQUID_TANK_HEIGHT | 
                                                  DEST_OP | SOURCE_MCU;
                                                stTx1Message.Data[0] = 0x01;  /*BHCM tank height*/
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /*Hardner Tank Height*/
                                                stTx1Message.DLC = FIVE_BYTES;
                                                SplitByte.fData = stConfigData.fConfigHardTankHeight;
                                                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt - 1];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_HCM_LIQUID_TANK_HEIGHT |
                                                  DEST_OP | SOURCE_MCU;
                                                stTx1Message.Data[0] = 0x02;  /*HHCM tank height*/
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                stTx1Message.DLC = ONE_BYTES;
                                                stTx1Message.Data[0] = stConfigData.ucConfigLLWindowSize;
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_LL_WINDOW_SIZE |
                                                  DEST_OP | SOURCE_MCU;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                
                                                /*Hardner Tank Height*/
                                                stTx1Message.DLC = FOUR_BYTES;
                                                SplitByte.fData = stConfigData.fConfigFlushWarnTime;
                                                for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_FLUSH_WARNNING_TIME |
                                                  DEST_OP | SOURCE_MCU;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                              }
                                              
                                              
                                              /*****************************************************************************
                                              **@Function 			: fnSendOPMMConfig
                                              **@Descriptions	: Sends configuration msg to OPMM
                                              **@parameters		: None
                                              **@return				: None
                                              *****************************************************************************/
                                              void fnSendOPMMConfig(void)
                                              {
                                                
                                                /*SET variable for configuration send to the OP*/
                                                uiDeviceConfigSent |= (1 << OPMM);
                                                
                                                /*For OPMM Spraying Count*/
                                                stTx1Message.DLC = ONE_BYTES;
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_SPRAY_COUNT |
                                                  DEST_OPMM | SOURCE_MCU;
                                                stTx1Message.Data[0] = stConfigData.ucConfigSprayCount;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /*For OPMM Prime Time*/
                                                stTx1Message.DLC = FOUR_BYTES;
                                                SplitByte.fData = stConfigData.fConfigPrimeTime;
                                                for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_PRIME_TIME |
                                                  DEST_OPMM | SOURCE_MCU;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /*For OPMM Gun Delay Time*/
                                                stTx1Message.DLC = FOUR_BYTES;
                                                SplitByte.fData = stConfigData.fConfigGunDelayTime;
                                                for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_OPMM_GUN_DELAY |
                                                  DEST_OPMM | SOURCE_MCU;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /*For OPMM Spray Ovelap Time*/
                                                stTx1Message.DLC = FOUR_BYTES;
                                                SplitByte.fData = stConfigData.fConfigSprayOverlapTime;
                                                for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_OPMM_SPRAY_OVERLAP_TIME |
                                                  DEST_OPMM | SOURCE_MCU;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /*For OPMM Reverse Delay Time*/
                                                stTx1Message.DLC = FOUR_BYTES;
                                                SplitByte.fData = stConfigData.fConfigReverseDelayTime;
                                                for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_OPMM_REVERSE_DELAY |
                                                  DEST_OPMM | SOURCE_MCU;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /*For OPMM Flush Time*/
                                                stTx1Message.DLC = FOUR_BYTES;
                                                SplitByte.fData = stConfigData.fConfigFlushTime;
                                                for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_OPMM_FLUSH_TIME |
                                                  DEST_OPMM | SOURCE_MCU;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /*DCLS reach timer*/
                                                stTx1Message.DLC = FIVE_BYTES;	
                                                SplitByte.fData = stConfigData.fConfigDCLSHomeTime;
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_DCLS_HOME_TIME |
                                                  DEST_OPMM | SOURCE_MCU;
                                                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt - 1];
                                                }
                                                stTx1Message.Data[0] = BYTE_DCLS;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /*HOME reach timer*/
                                                stTx1Message.DLC = FIVE_BYTES;	
                                                SplitByte.fData = stConfigData.fConfigHomeDCLSTime;
                                                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt - 1];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_DCLS_HOME_TIME |
                                                  DEST_OPMM | SOURCE_MCU;
                                                stTx1Message.Data[0] = BYTE_HLS;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /*Gun position change time*/
                                                stTx1Message.DLC = FOUR_BYTES;
                                                SplitByte.fData = stConfigData.fConfigGunPosChangeTime;
                                                for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_GUN_POSITION_CHANGE_TIME |
                                                  DEST_OPMM | SOURCE_MCU;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /*Delay time befor turning ON Spray valve(For Prime and Flush)*/
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_SPRAY_VALVE_OPEN_TIME_VBAT |
                                                  DEST_OPMM | SOURCE_MCU;
                                                stTx1Message.DLC = FOUR_BYTES;
                                                SplitByte.fData = stConfigData.fConfigTimebeforeONSprayValve;
                                                for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt];
                                                }
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                              }
                                              
                                              /************************************************************************
                                              **@Function 			: fnSendMFMConfig
                                              **@Descriptions	: Sends configuration msg to MFM
                                              **@parameters		: None
                                              **@return				: None
                                              *************************************************************************/
                                              void fnSendMFMConfig(void)
                                              {
                                                /*SET variable for configuration send to the OP*/
                                                uiDeviceConfigSent |= (1 << MFM);
                                                
                                                /* For Stall Threshold in% (Downstroke MFM)*/
                                                stTx1Message.DLC = THREE_BYTES;
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_MFM_STALL_TH |
                                                  DEST_MFM | SOURCE_MCU;
                                                stTx1Message.Data[0] = BYTE_LOW;
                                                SplitByte.iData = stConfigData.iConfigDownStrokeTH;
                                                stTx1Message.Data[1] = SplitByte.rgucData[0];
                                                stTx1Message.Data[2] = SplitByte.rgucData[1];
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /*For Stall Threshold in% (Upstroke MFM)*/
                                                stTx1Message.DLC = THREE_BYTES;
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_MFM_STALL_TH |
                                                  DEST_MFM | SOURCE_MCU;
                                                stTx1Message.Data[0] = BYTE_HIGH;
                                                SplitByte.iData = stConfigData.iConfigUpStrokeTH;
                                                stTx1Message.Data[1] = SplitByte.rgucData[0];
                                                stTx1Message.Data[2] = SplitByte.rgucData[1];
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /*For Pressure gauge 1  Threshold (Low TH MFM)*/
                                                stTx1Message.DLC = FIVE_BYTES;
                                                SplitByte.fData = stConfigData.fConfigPressureGauge1LTH;
                                                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt - 1];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_MFM_PGAUGE1_TH |
                                                  DEST_MFM | SOURCE_MCU;
                                                stTx1Message.Data[0] = BYTE_LOW;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /*For Pressure gauge 1  Threshold (High TH MFM)*/
                                                stTx1Message.DLC = FIVE_BYTES;
                                                SplitByte.fData = stConfigData.fConfigPressureGauge1HTH;
                                                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt - 1];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_MFM_PGAUGE1_TH |
                                                  DEST_MFM | SOURCE_MCU;
                                                stTx1Message.Data[0] = BYTE_HIGH;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /* For Pressure gauge 2 Threshold (Low TH MFM)*/
                                                stTx1Message.DLC = FIVE_BYTES;
                                                SplitByte.fData = stConfigData.fConfigPressureGauge2LTH;
                                                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt - 1];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_MFM_PGAUGE2_TH |
                                                  DEST_MFM | SOURCE_MCU;
                                                stTx1Message.Data[0] = BYTE_LOW;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /* For Pressure gauge 2 Threshold (High TH MFM) */
                                                stTx1Message.DLC = FIVE_BYTES;
                                                SplitByte.fData = stConfigData.fConfigPressureGauge2HTH;
                                                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt - 1];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_MFM_PGAUGE2_TH |
                                                  DEST_MFM | SOURCE_MCU;
                                                stTx1Message.Data[0] = BYTE_HIGH;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /*For Pressure gauge 3 Threshold (Low TH MFM)*/
                                                stTx1Message.DLC = FIVE_BYTES;
                                                SplitByte.fData = stConfigData.fConfigPressureGauge3LTH;
                                                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt - 1];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_MFM_PGAUGE3_TH |
                                                  DEST_MFM | SOURCE_MCU;
                                                stTx1Message.Data[0] = BYTE_LOW;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /*For Pressure gauge 3 Threshold (High TH MFM)*/
                                                stTx1Message.DLC = FIVE_BYTES;
                                                SplitByte.fData = stConfigData.fConfigPressureGauge3HTH;
                                                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt - 1];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_MFM_PGAUGE3_TH |
                                                  DEST_MFM | SOURCE_MCU;
                                                stTx1Message.Data[0] = BYTE_HIGH;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /*Number of cylinder (MFM)*/
                                                stTx1Message.DLC = ONE_BYTES;
                                                stTx1Message.Data[0] = stConfigData.ucConfigCylinderCount;
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_NUMBER_OF_CYLINDER |
                                                  DEST_MFM | SOURCE_MCU;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /* Surface area of cylinder (MFM cylinder A) */
                                                stTx1Message.DLC = FIVE_BYTES;
                                                SplitByte.fData = stConfigData.fConfigAreaofCylinderA;
                                                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt - 1];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_CYLINDER_SURFACE_AREA |
                                                  DEST_MFM | SOURCE_MCU;
                                                stTx1Message.Data[0] = CYLINDER_A ;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);	
                                                
                                                /* Surface area of cylinder (MFM cylinder B)*/
                                                stTx1Message.DLC = FIVE_BYTES;
                                                SplitByte.fData = stConfigData.fConfigAreaofCylinderB;
                                                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt - 1];
                                                } 
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_CYLINDER_SURFACE_AREA |
                                                  DEST_MFM | SOURCE_MCU;
                                                stTx1Message.Data[0] = CYLINDER_B;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);	
                                                
                                                /*Surface area of cylinder (MFM cylinder C)*/
                                                stTx1Message.DLC = FIVE_BYTES;
                                                SplitByte.fData = stConfigData.fConfigAreaofCylinderC;
                                                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt - 1];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_CYLINDER_SURFACE_AREA |
                                                  DEST_MFM | SOURCE_MCU;
                                                stTx1Message.Data[0] = CYLINDER_C;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);	
                                                
                                                /* Time before stall*/
                                                stTx1Message.DLC = FOUR_BYTES;
                                                SplitByte.fData = stConfigData.fConfigTimeBeforeStall;
                                                for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_TIME_BEFORE_STALL |
                                                  DEST_MFM | SOURCE_MCU;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /*Time before comparing stall pressure*/
                                                stTx1Message.DLC = FOUR_BYTES;
                                                SplitByte.fData = stConfigData.fConfigTimeCompareStall;
                                                for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_TIME_BEFORE_COMPARE_STALL |
                                                  DEST_MFM | SOURCE_MCU;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                              }
                                              /************************************************************************
                                              **@Function 			: fnSendBHCMConfig
                                              **@Descriptions	: Sends configuration msg to BHCM
                                              **@parameters		: None
                                              **@return				: None
                                              *************************************************************************/
                                              void fnSendBHCMConfig(void)
                                              {
                                                /*SET variable for configuration send to the OP*/
                                                uiDeviceConfigSent |= (1 << BHCM);
                                                
                                                /*For Base tank temperature setting (BHCM)*/
                                                stTx1Message.DLC = FOUR_BYTES;
                                                SplitByte.fData = stConfigData.fConfigBaseTempSet;
                                                for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_BHCM_BTANK_SET_TEMP |
                                                  DEST_BHCM | SOURCE_MCU;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /*For Base tank temperature threshold (BHCM Low TH)*/
                                                stTx1Message.DLC = FIVE_BYTES;
                                                SplitByte.fData = stConfigData.fConfigBaseTempLTH;
                                                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt - 1];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_BHCM_BTANK_TEMP_TH |
                                                  DEST_BHCM | SOURCE_MCU;
                                                stTx1Message.Data[0] = BYTE_LOW;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /*For Base tank temperature threshold (BHCM High TH)*/
                                                stTx1Message.DLC = FIVE_BYTES;
                                                SplitByte.fData = stConfigData.fConfigBaseTempHTH;
                                                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt - 1];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_BHCM_BTANK_TEMP_TH |
                                                  DEST_BHCM | SOURCE_MCU;
                                                stTx1Message.Data[0] = BYTE_HIGH;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /*For Base tank liquid level threshold (BHCM Low TH)*/
                                                stTx1Message.DLC = FIVE_BYTES;
                                                SplitByte.fData = stConfigData.fConfigBaseLiquidLevelLTH;
                                                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt - 1];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_BHCM_BTANK_LIQUID_TH |
                                                  DEST_BHCM | SOURCE_MCU;
                                                stTx1Message.Data[0] = BYTE_LOW;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);	
                                                
                                                /*For Base tank liquid level threshold (BHCM High TH)*/
                                                stTx1Message.DLC = FIVE_BYTES;
                                                SplitByte.fData = stConfigData.fConfigBaseLiquidLevelHTH;
                                                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt - 1];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_BHCM_BTANK_LIQUID_TH |
                                                  DEST_BHCM | SOURCE_MCU;
                                                stTx1Message.Data[0] = BYTE_HIGH;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                stTx1Message.DLC = FIVE_BYTES;
                                                SplitByte.fData = stConfigData.fConfigHysAntiFrzPos;
                                                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt - 1];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_TEMP_HYSTERESIS | 
                                                  DEST_BHCM | SOURCE_MCU;
                                                stTx1Message.Data[0] = ANTIFREEZ_HYS_POS;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                stTx1Message.DLC = FIVE_BYTES;
                                                SplitByte.fData = stConfigData.fConfigHysAntiFrzNeg;
                                                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt - 1];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_TEMP_HYSTERESIS |
                                                  DEST_BHCM | SOURCE_MCU;
                                                stTx1Message.Data[0] = ANTIFREEZ_HYS_NEG;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                stTx1Message.DLC = FIVE_BYTES;
                                                SplitByte.fData = stConfigData.fConfigBaseTankHeight;
                                                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt - 1];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_HCM_LIQUID_TANK_HEIGHT |
                                                  DEST_BHCM | SOURCE_MCU;
                                                stTx1Message.Data[0] = 0x01;  /*BHCM tank height*/
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /*Not now only when its added in MCU*/
                                                stTx1Message.DLC = ONE_BYTES;
                                                stTx1Message.Data[0] = stConfigData.ucConfigLLWindowSize;
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_LL_WINDOW_SIZE | 
                                                  DEST_BHCM | SOURCE_MCU;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                              }
                                              
                                              /*****************************************************************************
                                              **@Function 		 : fnSendHHCMConfig
                                              **@Descriptions : Sends configuration msg to IHCM3
                                              **@parameters	 : None
                                              **@return			 : None
                                              *****************************************************************************/
                                              void fnSendHHCMConfig(void)
                                              {
                                                /*Not now only when its added in MCU*/
                                                stTx1Message.DLC = ONE_BYTES;
                                                stTx1Message.Data[0] = stConfigData.ucConfigLLWindowSize;
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_LL_WINDOW_SIZE |
                                                  DEST_HHCM | SOURCE_MCU;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /*Thershold for HHCM liquid level*/
                                                stTx1Message.DLC = FIVE_BYTES;
                                                SplitByte.fData = stConfigData.fConfigHardLiquidLevelLTH;
                                                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt - 1];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_HHCM_LIQUID_LEVEL_TH |
                                                  DEST_HHCM | SOURCE_MCU;
                                                stTx1Message.Data[0] = 0x00;  /*LOW Thershold*/
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /*Thershold for HHCM liquid level*/
                                                stTx1Message.DLC = FIVE_BYTES;
                                                SplitByte.fData = stConfigData.fConfigHardLiquidLevelHTH;
                                                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt - 1];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_HHCM_LIQUID_LEVEL_TH | 
                                                  DEST_HHCM | SOURCE_MCU;
                                                stTx1Message.Data[0] = 0x01;  /*High Thershold*/
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /*HHCM liquid Tank Height*/
                                                stTx1Message.DLC = FIVE_BYTES;
                                                SplitByte.fData = stConfigData.fConfigHardTankHeight;
                                                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt - 1];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_HCM_LIQUID_TANK_HEIGHT |
                                                  DEST_HHCM | SOURCE_MCU;
                                                stTx1Message.Data[0] = 0x02;  /*HHCM tank height*/
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);	
                                              }
                                              
                                              
                                              
                                              
                                              /*****************************************************************************
                                              **@Function 			: fnSendIHCM1Config
                                              **@Descriptions	: Sends configuration msg to IHCM1
                                              **@parameters		: None
                                              **@return				: None
                                              *****************************************************************************/
                                              void fnSendIHCM1Config(void)
                                              {
                                                /*SET variable for configuration send to the OP*/
                                                uiDeviceConfigSent |= (1 << IHCM1);
                                                
                                                /*For Base Inline material temperature setting (IHCM base)*/
                                                stTx1Message.DLC = FOUR_BYTES;
                                                
                                                SplitByte.fData = stConfigData.fConfigInlineBaseTempSet;
                                                for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_IHCM_BLINE_SET_TEMP |
                                                  DEST_IHCM1 | SOURCE_MCU;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /*For Base inline material temperature LOW thresholds (IHCM Base)*/
                                                stTx1Message.DLC = FIVE_BYTES;
                                                SplitByte.fData = stConfigData.fConfigInlineBaseTempLTH;
                                                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt - 1];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_IHCM_BLINE_TEMP_TH |
                                                  DEST_IHCM1 | SOURCE_MCU;
                                                stTx1Message.Data[0] = BYTE_LOW;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /*For Base inline material temperature HIGH TH(IHCM Base)*/
                                                stTx1Message.DLC = FIVE_BYTES;
                                                SplitByte.fData = stConfigData.fConfigInlineBaseTempHTH;
                                                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt - 1];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_IHCM_BLINE_TEMP_TH |
                                                  DEST_IHCM1 | SOURCE_MCU;
                                                stTx1Message.Data[0] = BYTE_HIGH;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                stTx1Message.DLC = FIVE_BYTES;
                                                SplitByte.fData = stConfigData.fConfigHysBasePos;
                                                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt - 1];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_TEMP_HYSTERESIS |
                                                  DEST_IHCM1 | SOURCE_MCU;
                                                stTx1Message.Data[0] = BASE_HYS_POS;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                stTx1Message.DLC = FIVE_BYTES;
                                                SplitByte.fData = stConfigData.fConfigHysBaseNeg;
                                                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt - 1];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_TEMP_HYSTERESIS |
                                                  DEST_IHCM1 | SOURCE_MCU;
                                                stTx1Message.Data[0] = BASE_HYS_NEG;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                              }
                                              
                                              /*****************************************************************************
                                              **@Function 			: fnSendIHCM4HMConfig
                                              **@Descriptions	: Sends configuration msg to IHCM4BM
                                              **@parameters		: None
                                              **@return				: None
                                              *****************************************************************************/
                                              void fnSendIHCM4HMConfig(void)
                                              {	
                                                /*SET variable for configuration send to the OP*/
                                                uiDeviceConfigSent |= (1 << IHCM4_HM);
                                                
                                                /*Hardener Inline temperature setting (IHCM Hardner)*/
                                                stTx1Message.DLC = FOUR_BYTES;
                                                SplitByte.fData = stConfigData.fConfigInlineHardTempSet;
                                                for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_IHCM4_HLINE_SET_TEMP |
                                                  DEST_IHCM4_HM | SOURCE_MCU;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /*Hardener Inline  temperature LOW threshold (IHCM Hardner)*/
                                                stTx1Message.DLC = FIVE_BYTES;
                                                SplitByte.fData = stConfigData.fConfigInlineHardTempLTH;
                                                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt - 1];
                                                } 
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_IHCM4_HLINE_TEMP_TH |
                                                  DEST_IHCM4_HM | SOURCE_MCU;
                                                stTx1Message.Data[0] = BYTE_LOW;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /*Hardener Inline  temperature HIGH threshold (IHCM Hardner)*/
                                                stTx1Message.DLC = FIVE_BYTES;
                                                SplitByte.fData = stConfigData.fConfigInlineHardTempHTH;
                                                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt - 1];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_IHCM4_HLINE_TEMP_TH |
                                                  DEST_IHCM4_HM | SOURCE_MCU;
                                                stTx1Message.Data[0] = BYTE_HIGH;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                stTx1Message.DLC = FIVE_BYTES;
                                                SplitByte.fData = stConfigData.fConfigHysHardPos;
                                                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt - 1];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_TEMP_HYSTERESIS |
                                                  DEST_IHCM4_HM | SOURCE_MCU;
                                                stTx1Message.Data[0] = HARD_HYS_POS;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                stTx1Message.DLC = FIVE_BYTES;
                                                SplitByte.fData = stConfigData.fConfigHysHardNeg;
                                                for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                {
                                                  stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt - 1];
                                                }
                                                stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_TEMP_HYSTERESIS |
                                                  DEST_IHCM4_HM | SOURCE_MCU;
                                                stTx1Message.Data[0] = HARD_HYS_NEG;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                              }
                                              
                                              
                                              
                                              /*******************************************************************************
                                              **@Function 			: fnPeriodicHeartbeat
                                              **@Descriptions	: This function will send the periodic HB to all the slave
                                              **@parameters		: None
                                              **@return				: None
                                              *******************************************************************************/
                                              //void fnPeriodicHeartbeat(void)
                                              //{
                                              //	CanTxMsg stHBmsg;
                                              //	if(HB_SND_TIME <= uiHBTxCounter)
                                              //	{
                                              //		memset(stHBmsg.Data,0, sizeof(stHBmsg.Data));
                                              //		stHBmsg.IDE = CAN_ID_EXT;
                                              //		stHBmsg.RTR = CAN_RTR_DATA;
                                              //		stHBmsg.DLC = 0;
                                              //		stHBmsg.StdId = 0;
                                              //		stHBmsg.ExtId = MSG_TYPE_BROADCAST | RESERVED |
                                              //										 BCAST_HEARTBEAT | DEST_ALL | SOURCE_MCU;
                                              
                                              //		if(SET == StatusFlag.stStatusFlag.bCAN2ActiveFlag)
                                              //		{
                                              //			/*Transmitt CAN message onto CAN2 bus for OPMM only*/
                                              //			CAN_Transmit(CAN2, &stHBmsg);	
                                              //			#ifdef GENERAL_DBG
                                              //				DEBUG_PRINTF("HB ON CAN2 link\r\n");
                                              //			#endif
                                              //		}
                                              //		
                                              //		/*Transmitt CAN message onto CAN1 bus*/
                                              //		CAN_Transmit(CAN1, &stHBmsg);		
                                              
                                              //		#ifdef DBG_CAN
                                              //			DEBUG_PRINTF("HEARTBEAT SEND COUNT: %d\r\n",++uliTxHbCount);
                                              //		#endif
                                              //		uiHBTxCounter = 0;
                                              //		/* increment heartbeat counter */
                                              //		for(ucHBCntr = 0; ucHBCntr < TOTAL_HB_DEVICE; ucHBCntr++)
                                              //		{					
                                              //			if(rgucHBCntr[ucHBCntr] >= HB_RESND_CNT)
                                              //			{
                                              //				uiHBACKfail |= (1 << ucHBCntr);
                                              //				
                                              //				#ifdef TST_DBG
                                              //					 DEBUG_PRINTF("ucHBCntr = %d \r\n",ucHBCntr);
                                              //				#endif
                                              //			}
                                              //			else
                                              //				rgucHBCntr[ucHBCntr] += 1;
                                              //		}
                                              //		uiHBACKfail &= 0x007F;
                                              //	}
                                              //	
                                              //	if(uiHBACKfail != 0)
                                              //	{
                                              //		rgucHBFail[0] = uiHBACKfail & 0xff;
                                              //		
                                              //		if(uiHBACKfail & 0x02)
                                              //		{	
                                              //			if(RESET == StatusFlag.stStatusFlag.bCAN2ActiveFlag)
                                              //			{
                                              //				StatusFlag.stStatusFlag.bCAN2ActiveFlag = SET;
                                              //			}
                                              //			ucCANLinkFaultCnt++;
                                              //			if(CAN_FAULT_COUNT <= ucCANLinkFaultCnt) 
                                              //			{
                                              //				ucCANLinkFaultCnt = 0;
                                              //				StatusFlag.stStatusFlag.bAnyCANFaultFlag = SET;	
                                              //				StatusFlag.stStatusFlag.bErrorFlag = SET;
                                              //			}
                                              //		}
                                              //		for(ucHBCntr = 0; ucHBCntr < TOTAL_HB_DEVICE; ucHBCntr++)
                                              //		{
                                              //			if((!((uiHBFailSend >> ucHBCntr) & 0x01)) && 
                                              //				 ((uiHBACKfail >> ucHBCntr) & 0x01))
                                              //			{
                                              //				uiHBFailSend |= (1 << ucHBCntr);
                                              //				StatusFlag.stStatusFlag.bHBAckNotRcvFaultSndFlag = SET;	
                                              //			}
                                              //		}
                                              //		if(SET == StatusFlag.stStatusFlag.bHBAckNotRcvFaultSndFlag)
                                              //		{
                                              //			if(0x7F == (uiHBACKfail & 0x7F))
                                              //			{
                                              //				StatusFlag.stStatusFlag.bCAN1ActiveFlag = RESET;
                                              //				StatusFlag.stStatusFlag.bCAN2ActiveFlag = RESET;
                                              //				StatusFlag.stStatusFlag.bAnyCANFaultFlag = SET;	
                                              //				StatusFlag.stStatusFlag.bOPCANFaultFlag  = SET;
                                              //				StatusFlag.stStatusFlag.bErrorFlag = SET;
                                              //			}
                                              //			else 
                                              //			{
                                              //				/*OP HB not Rcv then OP CAN fault Declaired*/
                                              //				if(uiHBACKfail & 0x01)
                                              //				{
                                              //					/*OP CAN FAULT*/
                                              //					StatusFlag.stStatusFlag.bOPCANFaultFlag  = SET;
                                              //					/*ONLY WHEN Both CAN Fault Occurs*/		
                                              //					StatusFlag.stStatusFlag.bErrorFlag = SET;
                                              //				}
                                              //				/*Else any other module CAN Fault Declaire*/
                                              //				if(uiHBACKfail & (~0x01))
                                              //				{
                                              //					/*if any other CAN fault except OP and OPMM*/
                                              //					if(uiHBACKfail & (~0x02))
                                              //					{
                                              //						/* Any Other CAN Fault*/
                                              //						StatusFlag.stStatusFlag.bAnyCANFaultFlag = SET;	
                                              //						/*ONLY WHEN Both CAN Fault Occurs*/	
                                              //						StatusFlag.stStatusFlag.bErrorFlag = SET;
                                              //					}									
                                              //				}
                                              //			}
                                              //			StatusFlag.stStatusFlag.bHBAckNotRcvFaultSndFlag = RESET;
                                              //			fnHBAckFail_Send();	
                                              //			fnFault_Log();/*Fault Log for the heartbeat not received*/
                                              //		}	
                                              //		uiHBACKfail = 0;	
                                              //	}
                                              //}
                                              
                                              
                                              /*****************************************************************************
                                              **@Function 			: fnStatusLED
                                              **@Descriptions	: Gives the Status on the LED's
                                              **@parameters		: None
                                              **@return				: None
                                              *****************************************************************************/
                                              void fnStatusLED(void)
                                              {
                                                /*CAN1 Active*/
                                                if(SET == StatusFlag.stStatusFlag.bCAN1ActiveFlag)
                                                {
                                                  /* ON CAN1 status Led GREEN */
                                                  ucIOExp1LowerByte &=	(~CAN1_STATUS_RED_LED); /*RED OFF*/
                                                  ucIOExp1LowerByte &=	(~CAN1_STATUS_BLUE_LED); /*BLUE OFF*/
                                                  ucIOExp1LowerByte |=	CAN1_STATUS_GREEN_LED;  /*GREEN ON*/
                                                }
                                                else
                                                {
                                                  ucIOExp1LowerByte &=	(~CAN1_STATUS_RED_LED); /*RED OFF*/
                                                  ucIOExp1LowerByte &=	(~CAN1_STATUS_BLUE_LED); /*BLUE OFF*/
                                                  ucIOExp1LowerByte &=	(~CAN1_STATUS_GREEN_LED);  /*GREEN OFF*/
                                                }
                                                
                                                /*CAN2 Active*/
                                                if(SET == StatusFlag.stStatusFlag.bCAN2ActiveFlag)
                                                {
                                                  /* ON CAN2 status Led GREEN */
                                                  ucIOExp1LowerByte |=	CAN2_STATUS_GREEN_LED;   /*GREEN ON*/
                                                  ucIOExp1LowerByte &=	(~CAN2_STATUS_RED_LED);  /*RED OFF*/
                                                  ucIOExp1LowerByte &=	(~CAN2_STATUS_BLUE_LED); /*BLUE OFF*/
                                                }
                                                else
                                                {
                                                  ucIOExp1LowerByte &=	(~CAN2_STATUS_GREEN_LED);   /*GREEN OFF*/
                                                  ucIOExp1LowerByte &=	(~CAN2_STATUS_RED_LED);  /*RED OFF*/
                                                  ucIOExp1LowerByte &=	(~CAN2_STATUS_BLUE_LED); /*BLUE OFF*/
                                                }
                                                
                                                
                                                /*Add this Part only after the Pressure Regulator Part added*/
                                                /*
                                                if((fPressureRegRead >= (stConfigData.fConfigSprayPressure - 
                                                stConfigData.fConfigHysSprayPresNeg)) && (fPressureRegRead <=
                                                (stConfigData.fConfigSprayPressure + stConfigData.fConfigHysSprayPresPos)))
                                                {
                                                ucIOExp1HigherByte |=	PRESSURE_REG_STATUS_GREEN_LED; 
                                                ucIOExp1HigherByte &=	(~PRESSURE_REG_STATUS_RED_LED);
                                                ucIOExp1HigherByte &=	(~PRESSURE_REG_STATUS_BLUE_LED); 
                                              }
	else if((fPressureRegRead >= (stConfigData.fConfigRecirPressure -
                                                stConfigData.fConfigHysRecircPresNeg)) && (fPressureRegRead <=
                                                (stConfigData.fConfigRecirPressure + stConfigData.fConfigHysRecircPresPos)))
                                                {
                                                ucIOExp1HigherByte |=	PRESSURE_REG_STATUS_BLUE_LED; 
                                                ucIOExp1HigherByte &=	(~PRESSURE_REG_STATUS_RED_LED);
                                                ucIOExp1HigherByte &=	(~PRESSURE_REG_STATUS_GREEN_LED);
                                              }
	else
                                                {
                                                ucIOExp1HigherByte |=	PRESSURE_REG_STATUS_RED_LED;
                                                ucIOExp1HigherByte &=	(~PRESSURE_REG_STATUS_BLUE_LED); 
                                                ucIOExp1HigherByte &=	(~PRESSURE_REG_STATUS_GREEN_LED);
                                              }
                                                
                                                */	
                                                
                                                
                                                if(SET == StatusFlag.stStatusFlag.bOPCANFaultFlag)
                                                {
                                                  /* OFF Board status Led GREEN */
                                                  ucIOExp1HigherByte &=	(~BOARD_STATUS_GREEN_LED); /*GREEN OFF*/
                                                  //ucIOExp1HigherByte &=	(~BOARD_STATUS_RED_LED); /*RED OFF*/
                                                  if(uiAnyCANFaultLEDBlinkCnt < STATUS_LED_BLINK_COUNT)
                                                  {
                                                    /* ON Board status BLUE LED*/
                                                    ucIOExp1HigherByte |=	BOARD_STATUS_BLUE_LED; 
                                                  }	
                                                  else if(uiAnyCANFaultLEDBlinkCnt > STATUS_LED_BLINK_COUNT)
                                                  {
                                                    /* OFF Board status BLUE LED*/
                                                    ucIOExp1HigherByte &=	(~BOARD_STATUS_BLUE_LED);
                                                    
                                                    /* RESET The Blinking LED COUNTER if greatere than 200msec */
                                                    if(uiAnyCANFaultLEDBlinkCnt >= (2 * STATUS_LED_BLINK_COUNT))
                                                    {
                                                      uiAnyCANFaultLEDBlinkCnt = 0;
                                                    }
                                                  }
                                                }
                                                
                                                if(SET == StatusFlag.stStatusFlag.bAnyCANFaultFlag)
                                                {
                                                  /* OFF Board status Led GREEN */
                                                  ucIOExp1HigherByte &=	(~BOARD_STATUS_GREEN_LED); /*GREEN OFF*/
                                                  //ucIOExp1HigherByte &=	(~BOARD_STATUS_BLUE_LED); /*BLUE OFF*/
                                                  if(uiAnyCANFaultLEDBlinkCnt < STATUS_LED_BLINK_COUNT)
                                                  {
                                                    /* OFF Board status BLUE LED*/
                                                    ucIOExp1HigherByte &=	(~BOARD_STATUS_RED_LED);
                                                  }	
                                                  else if(uiAnyCANFaultLEDBlinkCnt > STATUS_LED_BLINK_COUNT)
                                                  {
                                                    /* ON Board status BLUE LED*/
                                                    ucIOExp1HigherByte |=	BOARD_STATUS_RED_LED; 
                                                    
                                                    /* RESET The Blinking LED COUNTER if greatere than 200msec */
                                                    if(uiAnyCANFaultLEDBlinkCnt >= (2 * STATUS_LED_BLINK_COUNT))
                                                    {
                                                      uiAnyCANFaultLEDBlinkCnt = 0;
                                                    }
                                                  }
                                                }
                                                
                                                if((RESET == StatusFlag.stStatusFlag.bAnyCANFaultFlag) &&
                                                   (RESET == StatusFlag.stStatusFlag.bOPCANFaultFlag))
                                                {
                                                  ucIOExp1HigherByte &=	(~BOARD_STATUS_RED_LED); /*RED OFF*/
                                                  ucIOExp1HigherByte |=	BOARD_STATUS_GREEN_LED; /*GREEN ON*/
                                                  ucIOExp1HigherByte &=	(~BOARD_STATUS_BLUE_LED); /*BLUE OFF*/
                                                }
                                                fnIOExpander_Write(SLAVE1_ADDRESS, ucIOExp1LowerByte, ucIOExp1HigherByte);
                                              }
                                              
                                              
                                              /*****************************************************************************
                                              **@Function 				: fnMCU_Operation
                                              **@Descriptions		: This function contains the all other funtionality of 
                                              MCU in both mode(operation and warmup)
                                              **@parameters			: None
                                              **@return					: None
                                              *****************************************************************************/
                                              void fnMCU_Operation(void)
                                              {
#ifdef GENERAL_DBG
                                                char rgcTemp[200];
#endif
                                                if(RTC_DATA_SEND_TIME <= uiRTCReadCnt)
                                                {
                                                  uiRTCReadCnt = 0;
                                                  
                                                  RTC_GetTime(RTC_Format_BIN, &stRTC_TimeStructure);
                                                  RTC_GetDate(RTC_Format_BIN, &stRTC_DateStructure);
                                                  
                                                  stConfigData.ucConfigSystemTime[0] = stRTC_TimeStructure.RTC_Hours ;
                                                  stConfigData.ucConfigSystemTime[1] = stRTC_TimeStructure.RTC_Minutes;
                                                  stConfigData.ucConfigSystemTime[2] = stRTC_TimeStructure.RTC_Seconds;
                                                  
                                                  stConfigData.ucConfigSystemTime[3] = stRTC_DateStructure.RTC_Date;
                                                  stConfigData.ucConfigSystemTime[4] = stRTC_DateStructure.RTC_Month;
                                                  stConfigData.ucConfigSystemTime[5] = 20;
                                                  stConfigData.ucConfigSystemTime[6] = stRTC_DateStructure.RTC_Year;
                                                  
#ifdef GENERAL_DBG
                                                  DEBUG_PRINTF("%02d/%02d/20%02d, %02d:%02d:%02d,\r\n ",
                                                               stConfigData.ucConfigSystemTime[3],	stConfigData.ucConfigSystemTime[4],
                                                               stConfigData.ucConfigSystemTime[6], stConfigData.ucConfigSystemTime[0],
                                                               stConfigData.ucConfigSystemTime[1],	stConfigData.ucConfigSystemTime[2]);
#endif								
                                                  stTx1Message.DLC = SEVEN_BYTES;
                                                  stTx1Message.ExtId = MSG_TYPE_CONFIG | CONFIG_SYS_DATE_TIME |
                                                    DEST_OP | SOURCE_MCU;
                                                  for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                  {
                                                    stTx1Message.Data[ucLoopCnt] = 
                                                      stConfigData.ucConfigSystemTime[ucLoopCnt];
                                                  }
                                                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                }
                                                
                                                if((MCU_WARMUP_MODE == ucMCUMode) || (MCU_OPERATION_MODE == ucMCUMode))
                                                {
                                                  /*Reading of the RTC register after 1 second*/
                                                  /* READ The Ambiant Temp and Humidity*/
                                                  //fnAmbiantData_Read();
                                                  
                                                  /*Return Line Temp Read and send to OP*/
                                                  //		if(RETURNLINE_TEMP_READ_TIME <= uiRLTempReadCnt)
                                                  //		{
                                                  //			uiRLTempReadCnt = 0;
                                                  //			stCurrentData.fCurrReturnLineTemp = fnReadThermocouple();
                                                  //			if(!((MAX_READING_RANGE >= fabs(stCurrentData.fCurrReturnLineTemp)) &&
                                                  //					 (MIN_READING_RANGE <= fabs(stCurrentData.fCurrReturnLineTemp))))
                                                  //			{
                                                  //				stCurrentData.fCurrReturnLineTemp = INVALID_FLOAT_READ;
                                                  //			}
                                                  //			
                                                  //			/*impliment heater control logic if return line is for hardner material*/
                                                  //			stTx1Message.DLC = FOUR_BYTES;
                                                  //			SplitByte.fData = stCurrentData.fCurrReturnLineTemp;
                                                  //			for(ucLoopCnt = 0 ; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                                  //			{
                                                  //				 stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt];
                                                  //			}
                                                  //			stTx1Message.ExtId = MSG_TYPE_DATA | DATA_MCU_RETURN_LINE_TEMP |
                                                  //													 DEST_OP | SOURCE_MCU;
                                                  //			fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                  //			
                                                  //			/*DATA Logging of Returnline Temp*/
                                                  //			rgucPeriodicLog[BYTE_THIRTYONE] = SplitByte.rgucData[DATA_BYTE_ZERO];
                                                  //			rgucPeriodicLog[BYTE_THIRTYTWO] = SplitByte.rgucData[DATA_BYTE_ONE];
                                                  //			rgucPeriodicLog[BYTE_THIRTYTHREE] = SplitByte.rgucData[DATA_BYTE_TWO];
                                                  //			rgucPeriodicLog[BYTE_THIRTYFOUR] = SplitByte.rgucData[DATA_BYTE_THREE]; 
                                                  //		}
                                                  //		
                                                  /*Peridic Data Logging*/
                                                  if((stConfigData.fConfigDataLoggingPeriod * 100) <= uiPeriodicDataLogCnt)
                                                  { 
                                                    uiPeriodicDataLogCnt = 0;
                                                    fnPeriodic_Log();
#ifdef GENERAL_DBG
                                                    DEBUG_PRINTF("Periodic Data Log Updated\r\n ");
#endif
                                                  }
                                                  
                                                  /*Pressure Gauge Reading*/
                                                  //		if(PRESSURE_REG_READ_TIME <= uiPressureRegReadCnt)
                                                  //		{
                                                  //			uiPressureRegReadCnt = 0;
                                                  //			fPressureRegRead = fnADCValue_Read(PRESSURE_REG_ADC_CH);
                                                  //			
                                                  /*converting pressure from Voltage to bar and 0.6 voltage
                                                  corresponds to 0.7 psi and 3.0V corresponds to 130.0 psi*/
                                                  //			fPressureRegRead = (((fPressureRegRead - LOW_PRESSURE_V) *
                                                  //														PRESSURE_REG_SLOPE_ADC) + LOW_PRESSURE_LEVEL);
                                                  //			#ifdef GENEARAL_DBG
                                                  //				sprintf(rgcTemp, "READ PSI VALUE : %f \n\r",
                                                  //									fPressureRegRead);	
                                                  //				DEBUG_PRINTF("%s",rgcTemp);
                                                  //			#endif
                                                  //			
                                                  //			/*psi to Bar conversion of pressure regulator value*/
                                                  //			fPressureRegRead = PSI_TO_BAR(fPressureRegRead);
                                                  //			
                                                  //			#ifdef GENERAL_DBG
                                                  //				sprintf(rgcTemp,"CONVERTED PSI TO BAR VALUE : %f \n\r",
                                                  //									fPressureRegRead);
                                                  //				DEBUG_PRINTF("%s",rgcTemp);
                                                  //			#endif
                                                  //			
                                                  //		}
                                                }
                                                
                                                /*Check for the Temperatures of Material in Operation 
                                                Mode if it is not in range then send to OP*/
                                                if(MCU_OPERATION_MODE == ucMCUMode)
                                                {
                                                  /*inline base Material out of range and Send to OP*/
                                                  if((fabs(stCurrentData.fCurrInlineBaseMatTemp) <= 
                                                      fabs(stConfigData.fConfigInlineBaseTempLTH)) && 
                                                     (fabs(stCurrentData.fCurrInlineBaseMatTemp) >=
                                                      fabs(stConfigData.fConfigInlineBaseTempHTH)))
                                                  {
                                                    /* for base*/
                                                    /*use to send only once to OP*/
                                                    if(RESET == StatusFlag.stStatusFlag.bTemp6Flag) 
                                                    {
                                                      StatusFlag.stStatusFlag.bTemp6Flag = SET;
                                                      stTx1Message.DLC = 0;
                                                      stTx1Message.ExtId = MSG_TYPE_DATA | FAULT_MCU_IBASE_TEMP_LOW |
                                                        DEST_OP | SOURCE_MCU;
                                                      fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                      
                                                      /* FAULT BYTE UPDATED and WRITTEN in NVSRAM */
                                                      ucFaultByte0 = (ucFaultByte0 | BIT7_SET); 
                                                      fnFault_Log();
                                                    }
                                                  }
                                                  else 
                                                  {
                                                    StatusFlag.stStatusFlag.bTemp6Flag = RESET;
                                                  }
                                                  /*inline Hardner Material out of range and Send to OP*/
                                                  if((fabs(stCurrentData.fCurrInlineHardMatTemp) < 
                                                      fabs(stConfigData.fConfigInlineHardTempLTH)) && 
                                                     (fabs(stCurrentData.fCurrInlineHardMatTemp) >
                                                      fabs(stConfigData.fConfigInlineHardTempHTH)))
                                                  {
                                                    /* for hardner*/
                                                    if(RESET == StatusFlag.stStatusFlag.bTemp7Flag)
                                                    {
                                                      StatusFlag.stStatusFlag.bTemp7Flag = SET;
                                                      stTx1Message.DLC = 0;
                                                      stTx1Message.ExtId = MSG_TYPE_FAULT | FAULT_MCU_IHARD_TEMP_TH |
                                                        DEST_OP | SOURCE_MCU;
                                                      fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                      /* FAULT BYTE UPDATED and WRITTEN in NVSRAM */
                                                      ucFaultByte1 = (ucFaultByte1 | BIT1_SET); 
                                                      fnFault_Log();
                                                    }
                                                  }
                                                  else
                                                  {
                                                    /* once the fault clear then send the the fault */
                                                    StatusFlag.stStatusFlag.bTemp7Flag = RESET; 
                                                  }
                                                }
                                                
                                                
                                                
                                                
                                                //	/*Warnning Action*/
                                                //	if(SET == StatusFlag.stStatusFlag.bWarnningFlag)
                                                //	{
                                                //	StatusFlag.stStatusFlag.bWarnningFlag = RESET;
                                                //		/*Recirculation Valve ON*/
                                                //		stTx1Message.ExtId = MSG_TYPE_COMMAND | CMD_VALVE_MFM |
                                                //													 DEST_MFM | SOURCE_MCU;
                                                //		stTx1Message.DLC = 0x02;
                                                //		stTx1Message.Data[0] = 0x01;    /* Bit1 = 1, switches on the valve*/
                                                //		stTx1Message.Data[1] = 0x01;
                                                //		fnCANMsg_Transmit(&stTx1Message, CAN1);  
                                                //		
                                                //		if(MCU_OPERATION_MODE == ucMCUMode)
                                                //		{
                                                //			//StatusFlag.stStatusFlag.bOperationModeOver = SET;
                                                //		}
                                                //		
                                                //	}
                                                
                                                /* Error Action*/
                                                if(SET == StatusFlag.stStatusFlag.bErrorFlag)
                                                {		
                                                  if(RESET == StatusFlag.stStatusFlag.bSpraySelector)
                                                  {
                                                    StatusFlag.stStatusFlag.bErrorFlag = RESET;
                                                    
                                                    /*Recirculation Valve ON*/
                                                    stTx1Message.ExtId = MSG_TYPE_COMMAND | CMD_VALVE_MFM |
                                                      DEST_MFM | SOURCE_MCU;
                                                    stTx1Message.DLC = 0x02;
                                                    stTx1Message.Data[0] = 0x01;    /* ON*/
                                                    stTx1Message.Data[1] = 0x01;   /*Recirculation*/
                                                    fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                    stTx1Message.ExtId = MSG_TYPE_COMMAND | CMD_VALVE_MFM |
                                                      DEST_OP | SOURCE_MCU;
                                                    fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                    
                                                    stTx1Message.ExtId = MSG_TYPE_FAULT | FAULT_ERROR_WARNNING |
                                                      DEST_OPMM | SOURCE_MCU;
                                                    stTx1Message.DLC = 0x01;
                                                    stTx1Message.Data[0] = 0x02;    /* Error Bit set*/
                                                    fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                    
                                                    stTx1Message.ExtId = MSG_TYPE_FAULT | FAULT_ERROR_WARNNING |
                                                      DEST_OP | SOURCE_MCU;
                                                    stTx1Message.DLC = 0x01;
                                                    stTx1Message.Data[0] = 0x02;    /* Error Bit set*/
                                                    fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                  }
                                                }
                                              }
                                              
                                              ///*****************************************************************************
                                              // **@Function 		 :  fnEEPROMConfig_Write
                                              // **@Descriptions :	this function write config parameters recieved from the OP
                                              // **@parameters	 :  None
                                              // **@return			 :  None
                                              //*****************************************************************************/
                                              //void fnEEPROMConfig_Write(void)
                                              //{
                                              //	if(0 != ucEEPROMCounter)
                                              //	{
                                              //		#ifdef GENERAL_DBG
                                              //			DEBUG_PRINTF("EEPROM WRITE FROM OP: %d\r\n",ucEEPROMCounter);					
                                              //		#endif
                                              //		switch(ucEEPROMCounter)
                                              //		{
                                              //			case CONFIG_PARA_1:
                                              //			{
                                              //				SplitByte.fData = stConfigData.fConfigSprayPressure;
                                              //				fnSPI_EEPROM_WriteData(PRESSURE_SPRAY_VALUE, 4, SplitByte.rgucData);
                                              //			}
                                              //			break;
                                              //			case CONFIG_PARA_2:
                                              //			{
                                              //					SplitByte.fData = stConfigData.fConfigRecirPressure;
                                              //					fnSPI_EEPROM_WriteData(PRESSURE_RECIR_VALUE, 4, SplitByte.rgucData );
                                              //			}
                                              //			break;
                                              //			case CONFIG_PARA_3:
                                              //			{
                                              //				SplitByte.fData = stConfigData.fConfigPipeTempLTH;
                                              //				fnSPI_EEPROM_WriteData(TEMP_PIPE_LTH, 4, SplitByte.rgucData);
                                              //			}
                                              //			break;
                                              //			case CONFIG_PARA_4:
                                              //			{
                                              //				SplitByte.fData = stConfigData.fConfigPipeTempHTH;
                                              //				fnSPI_EEPROM_WriteData(TEMP_PIPE_HTH, 4, SplitByte.rgucData);
                                              
                                              //			}break;
                                              //			case CONFIG_PARA_5:
                                              //			{
                                              //				fnSPI_EEPROM_WriteData(OPMM_SPRAY_COUNT, 1,
                                              //															 &stConfigData.ucConfigSprayCount);
                                              //				#ifdef GENERAL_DBG
                                              //					DEBUG_PRINTF("Write Spray Count = %d\r\n",
                                              //												stConfigData.ucConfigSprayCount);
                                              //				#endif
                                              //			}
                                              //			break;
                                              //			case CONFIG_PARA_6:
                                              //			{
                                              //				SplitByte.fData = stConfigData.fConfigPrimeTime;
                                              //				fnSPI_EEPROM_WriteData(OPMM_PRIME_TIME, 4, SplitByte.rgucData);
                                              //			}
                                              //			break;
                                              //			case CONFIG_PARA_7:
                                              //			{
                                              //				 SplitByte.fData = stConfigData.fConfigGunDelayTime;
                                              //				fnSPI_EEPROM_WriteData(OPMM_GUN_TIME, 4, SplitByte.rgucData);
                                              //			}
                                              //			break;
                                              //			case CONFIG_PARA_8:
                                              //			{
                                              //				SplitByte.fData = stConfigData.fConfigSprayOverlapTime;
                                              //				fnSPI_EEPROM_WriteData(OPMM_SPRAY_OVERLAPTIME, 4, SplitByte.rgucData);
                                              //			}
                                              //			break;
                                              //			case CONFIG_PARA_9:
                                              //			{
                                              //				SplitByte.fData = stConfigData.fConfigReverseDelayTime;
                                              //				fnSPI_EEPROM_WriteData(OPMM_REVERSE_DELAYTIME, 4, SplitByte.rgucData);
                                              //			
                                              //			}break;
                                              //			case CONFIG_PARA_10:
                                              //			{
                                              //				SplitByte.fData = stConfigData.fConfigFlushTime;
                                              //				fnSPI_EEPROM_WriteData(OPMM_FLUSH_TIME, 4, SplitByte.rgucData);
                                              //			}
                                              //			break;
                                              //			case CONFIG_PARA_11:
                                              //			{
                                              //				SplitByte.iData = stConfigData.iConfigUpStrokeTH;
                                              //				fnSPI_EEPROM_WriteData(MFM_UPSTROKE_ADD, 2, SplitByte.rgucData);
                                              //			}
                                              //			break;
                                              //			case CONFIG_PARA_12:
                                              //			{
                                              //				SplitByte.iData = stConfigData.iConfigDownStrokeTH;
                                              //				fnSPI_EEPROM_WriteData(MFM_DOWNSTROKE_ADD, 1, SplitByte.rgucData);
                                              //			}break;
                                              //			case CONFIG_PARA_13:
                                              //			{
                                              //				SplitByte.fData = stConfigData.fConfigPressureGauge1LTH;
                                              //				fnSPI_EEPROM_WriteData(MFM_PRESSURE_LOWTH1, 4, SplitByte.rgucData); 
                                              //			}
                                              //			break;
                                              //			case CONFIG_PARA_14:
                                              //			{ 
                                              //				SplitByte.fData = stConfigData.fConfigPressureGauge1HTH;
                                              //				fnSPI_EEPROM_WriteData(MFM_PRESSURE_HIGHTH1, 4, SplitByte.rgucData); 
                                              //			}
                                              //			break;
                                              //			case CONFIG_PARA_15:
                                              //			{
                                              //				SplitByte.fData = stConfigData.fConfigPressureGauge2LTH;
                                              //				fnSPI_EEPROM_WriteData(MFM_PRESSURE_LOWTH2, 4, SplitByte.rgucData);
                                              //			}
                                              //			break;
                                              //			case CONFIG_PARA_16:
                                              //			{
                                              //				SplitByte.fData = stConfigData.fConfigPressureGauge2HTH;
                                              //				fnSPI_EEPROM_WriteData(MFM_PRESSURE_HIGHTH2, 4, SplitByte.rgucData);
                                              //			}
                                              //			break;
                                              //			case CONFIG_PARA_17:
                                              //			{
                                              //					SplitByte.fData = stConfigData.fConfigPressureGauge3LTH;
                                              //					fnSPI_EEPROM_WriteData(MFM_PRESSURE_LOWTH3, 4, SplitByte.rgucData);
                                              //			}
                                              //			break;
                                              //			case CONFIG_PARA_18:
                                              //			{
                                              //				SplitByte.fData = stConfigData.fConfigPressureGauge3HTH;
                                              //				fnSPI_EEPROM_WriteData(MFM_PRESSURE_HIGHTH3, 4, SplitByte.rgucData);
                                              //			}
                                              //			break;
                                              //			case CONFIG_PARA_19:
                                              //			{
                                              //				SplitByte.fData = stConfigData.fConfigBaseTempSet;
                                              //				fnSPI_EEPROM_WriteData(BHCM_SET_TEMP, 4, SplitByte.rgucData);
                                              //			}
                                              //			break;
                                              //			case CONFIG_PARA_20:
                                              //			{ 
                                              //				SplitByte.fData = stConfigData.fConfigBaseTempLTH;
                                              //				fnSPI_EEPROM_WriteData(BHCM_TEMPTHRESHOLD_LOW, 4, SplitByte.rgucData);
                                              //			}
                                              //			break;
                                              //			case CONFIG_PARA_21:
                                              //			{ 
                                              //				SplitByte.fData = stConfigData.fConfigBaseTempHTH;
                                              //				fnSPI_EEPROM_WriteData(BHCM_TEMPTHRESHOLD_HIGH, 4, SplitByte.rgucData);
                                              //			}
                                              //			break;
                                              //			case CONFIG_PARA_22:
                                              //			{ 
                                              //				SplitByte.fData = stConfigData.fConfigBaseLiquidLevelLTH;
                                              //				fnSPI_EEPROM_WriteData(BHCM_LIQUIDLEVELTHRESHOLD_LOW, 4,
                                              //															 SplitByte.rgucData);
                                              //			}
                                              //			break;
                                              //			case CONFIG_PARA_23:
                                              //			{ 
                                              //				SplitByte.fData = stConfigData.fConfigBaseLiquidLevelHTH;
                                              //				fnSPI_EEPROM_WriteData(BHCM_LIQUIDLEVELTHRESHOLD_HIGH, 4,
                                              //															 SplitByte.rgucData);
                                              //			}
                                              //			break;
                                              //			case CONFIG_PARA_24:	
                                              //			{
                                              //				SplitByte.fData = stConfigData.fConfigInlineBaseTempSet;
                                              //				fnSPI_EEPROM_WriteData(IHCM_SET_TEMP, 4, SplitByte.rgucData);
                                              //			}
                                              //			break;
                                              //			case CONFIG_PARA_25:
                                              //			{
                                              //				SplitByte.fData = stConfigData.fConfigInlineBaseTempLTH;
                                              //				fnSPI_EEPROM_WriteData(IHCM_LOWTHRESHOLD_TEMP, 4, SplitByte.rgucData);
                                              //			}
                                              //			break;
                                              //			case CONFIG_PARA_26:
                                              //			{ 
                                              //				 SplitByte.fData = stConfigData.fConfigInlineBaseTempHTH;
                                              //				fnSPI_EEPROM_WriteData(IHCM_HIGHTHRESHOLD_TEMP, 4, SplitByte.rgucData);
                                              //			}
                                              //			break;
                                              //			case CONFIG_PARA_27:
                                              //			{
                                              //				 SplitByte.fData = stConfigData.fConfigInlineHardTempSet;
                                              //				fnSPI_EEPROM_WriteData(IHCMHARDNER_SET_TEMP, 4, SplitByte.rgucData);
                                              //			}
                                              //			break;
                                              //			case CONFIG_PARA_28:
                                              //			{
                                              //				SplitByte.fData = stConfigData.fConfigInlineHardTempLTH;
                                              //				fnSPI_EEPROM_WriteData(IHCMHARDNER_LOWTHRESHOLD_TEMP, 4,
                                              //															SplitByte.rgucData);
                                              //			}
                                              //			break;
                                              //			case CONFIG_PARA_29:
                                              //			{ 
                                              //				SplitByte.fData = stConfigData.fConfigInlineHardTempHTH;
                                              //				fnSPI_EEPROM_WriteData(IHCMHARDNER_HIGHTHRESHOLD_TEMP, 4,
                                              //																SplitByte.rgucData);
                                              //			}
                                              //			break;
                                              //			case CONFIG_PARA_30:
                                              //			{
                                              //				SplitByte.fData = stConfigData.fConfigOperationTime;
                                              //				fnSPI_EEPROM_WriteData(TIME_OPERATION, 4, SplitByte.rgucData);
                                              //			}
                                              //			break;
                                              //			case CONFIG_PARA_31:
                                              //			{ 
                                              //				SplitByte.fData = stConfigData.fConfigJointNum;
                                              //				fnSPI_EEPROM_WriteData(COUNT_JOINT, 4, SplitByte.rgucData);
                                              //			}
                                              //			break;
                                              //			case CONFIG_PARA_32:
                                              //			{ 
                                              //				SplitByte.fData = stConfigData.fConfigDataLoggingPeriod;
                                              //				fnSPI_EEPROM_WriteData(TIME_DATA_LOGGING, 4, SplitByte.rgucData);
                                              //			}
                                              //			break;
                                              //			case CONFIG_PARA_34:
                                              //			{ 
                                              //				SplitByte.fData = stConfigData.fConfigAreaofCylinderA;
                                              //				fnSPI_EEPROM_WriteData(MFM_CYLINDER_AREA1, 4, SplitByte.rgucData);
                                              //			}
                                              //			break;
                                              //			case CONFIG_PARA_35:
                                              //			{
                                              //				SplitByte.fData = stConfigData.fConfigAreaofCylinderB;
                                              //				fnSPI_EEPROM_WriteData(MFM_CYLINDER_AREA2, 4, SplitByte.rgucData);
                                              //			}
                                              //			break;
                                              //			case CONFIG_PARA_36:
                                              //			{ 
                                              //				 SplitByte.fData = stConfigData.fConfigAreaofCylinderC;
                                              //				fnSPI_EEPROM_WriteData(MFM_CYLINDER_AREA3, 4, SplitByte.rgucData);
                                              //			}
                                              //			break;
                                              //			case CONFIG_PARA_37:
                                              //			{
                                              //				fnSPI_EEPROM_WriteData(MFM_CYLINDER_COUNT, 1,
                                              //															 &stConfigData.ucConfigCylinderCount);
                                              //			}
                                              //			break;
                                              //			case CONFIG_PARA_38:
                                              //			{ 
                                              //				fnSPI_EEPROM_WriteData(SUPERVISOR_NAME1, 7, 
                                              //																stConfigData.rgucConfigSupervisorName1);
                                              //			}
                                              //			break;
                                              //			case CONFIG_PARA_39:
                                              //			{ 
                                              //				fnSPI_EEPROM_WriteData(SUPERVISOR_NAME2, 7,
                                              //															stConfigData.rgucConfigSupervisorName2);
                                              //			}
                                              //			break;
                                              //			case CONFIG_PARA_40:
                                              //			{
                                              //				fnSPI_EEPROM_WriteData(SUPERVISOR_NAME3, 7,
                                              //															stConfigData.rgucConfigSupervisorName3);
                                              //			}
                                              //			break;
                                              //			case CONFIG_PARA_41:
                                              //			{ 
                                              //				fnSPI_EEPROM_WriteData(COATING_MAT_NAME1, 7,
                                              //														  stConfigData.rgucConfigCoatingMatName1);
                                              //			}
                                              //			break;
                                              //			case CONFIG_PARA_42:
                                              //			{ 
                                              //				fnSPI_EEPROM_WriteData(COATING_MAT_NAME2, 7,
                                              //															 stConfigData.rgucConfigCoatingMatName2);
                                              //			}
                                              //			break;
                                              //			case CONFIG_PARA_43:
                                              //			{ 
                                              //				fnSPI_EEPROM_WriteData(COATING_MAT_NAME3, 7,
                                              //														   stConfigData.rgucConfigCoatingMatName3);
                                              //			}
                                              //			break;
                                              //			case CONFIG_PARA_44:
                                              //			{
                                              //				fnSPI_EEPROM_WriteData(BASE_MAT_BATCHNO1, 7,
                                              //															 stConfigData.rgucConfigBaseMatBatchNo1);
                                              //			}
                                              //			break;
                                              //			case CONFIG_PARA_45:
                                              //			{
                                              //				fnSPI_EEPROM_WriteData(BASE_MAT_BATCHNO2, 7,
                                              //															 stConfigData.rgucConfigBaseMatBatchNo2);
                                              //			}
                                              //			break;
                                              //			case CONFIG_PARA_46:
                                              //			{ 
                                              //				fnSPI_EEPROM_WriteData(BASE_MAT_BATCHNO3, 7,
                                              //																stConfigData.rgucConfigBaseMatBatchNo3);
                                              //			}
                                              //			break;
                                              //			case CONFIG_PARA_47:
                                              //			{ 
                                              //				fnSPI_EEPROM_WriteData(HARD_MAT_BATCHNO1, 7,
                                              //															stConfigData.rgucConfigHardMatBatchNo1);
                                              
                                              //			}
                                              //			break;
                                              //			case CONFIG_PARA_48:
                                              //			{
                                              //				fnSPI_EEPROM_WriteData(HARD_MAT_BATCHNO2, 7,
                                              //														   stConfigData.rgucConfigHardMatBatchNo2);
                                              //			}
                                              //			break;
                                              //			case CONFIG_PARA_49:
                                              //			{
                                              //				fnSPI_EEPROM_WriteData(HARD_MAT_BATCHNO3, 7,
                                              //															 stConfigData.rgucConfigHardMatBatchNo3);
                                              //			}
                                              //			break;
                                              //			case CONFIG_PARA_50:
                                              //			{ 
                                              //				SplitByte.fData = stConfigData.fConfigTimeBeforeStall;
                                              //				fnSPI_EEPROM_WriteData(TIME_BEFORE_STALL, 4, SplitByte.rgucData);
                                              //			}
                                              //			break;
                                              //			case CONFIG_PARA_51:
                                              //			{ 
                                              //				SplitByte.fData = stConfigData.fConfigTimeCompareStall;
                                              //				fnSPI_EEPROM_WriteData(TIME_COMPARE_STALL_PRESSURE, 4, SplitByte.rgucData);
                                              //			}
                                              //			break;
                                              //			case CONFIG_PARA_52:
                                              //			{ 
                                              //				SplitByte.fData = stConfigData.fConfigDCLSHomeTime;
                                              //				fnSPI_EEPROM_WriteData(OPMM_DCLS_HOME_REACH_TIME, 4, SplitByte.rgucData);
                                              //			}
                                              //			break;
                                              //			case CONFIG_PARA_53:
                                              //			{ 
                                              //				SplitByte.fData = stConfigData.fConfigHomeDCLSTime;
                                              //				fnSPI_EEPROM_WriteData(OPMM_HOME_DCLS_REACH_TIME, 4, SplitByte.rgucData);
                                              //			}
                                              //			break;
                                              //			case CONFIG_PARA_54:
                                              //			{ 
                                              //				SplitByte.fData = stConfigData.fConfigGunPosChangeTime;
                                              //				fnSPI_EEPROM_WriteData(OPMM_GUN_POSITION_CHANGE_TIME, 4,
                                              //															 SplitByte.rgucData);
                                              //			}
                                              //			break;
                                              //			case CONFIG_PARA_55:
                                              //			{ 
                                              //				SplitByte.fData = stConfigData.fConfigTimebeforeONSprayValve;
                                              //				fnSPI_EEPROM_WriteData(TIME_BEFORE_ON_SPRAY, 4, SplitByte.rgucData);
                                              //			}
                                              //			break;
                                              //			case CONFIG_PARA_56:
                                              //			{
                                              //				SplitByte.fData = stConfigData.fConfigHysBasePos;
                                              //				fnSPI_EEPROM_WriteData(HYSTERESIS_BASE_POS, 4, SplitByte.rgucData);
                                              //			}
                                              //			break;
                                              //			case CONFIG_PARA_57:
                                              //			{
                                              //				SplitByte.fData = stConfigData.fConfigHysBaseNeg ;
                                              //				fnSPI_EEPROM_WriteData(HYSTERESIS_BASE_NEG, 4, SplitByte.rgucData);
                                              //			}
                                              //			break;
                                              //			case CONFIG_PARA_58:
                                              //			{ 
                                              //				SplitByte.fData = stConfigData.fConfigHysHardPos;
                                              //				fnSPI_EEPROM_WriteData(HYSTERESIS_HARD_POS, 4, SplitByte.rgucData);	
                                              //			}
                                              //			break;
                                              //			case CONFIG_PARA_59:
                                              //			{
                                              //				SplitByte.fData = stConfigData.fConfigHysHardNeg;
                                              //				fnSPI_EEPROM_WriteData(HYSTERESIS_HARD_NEG, 4, SplitByte.rgucData);
                                              //			}
                                              //			break;
                                              //			case CONFIG_PARA_60:
                                              //			{ 
                                              //				SplitByte.fData = stConfigData.fConfigHysAntiFrzPos;
                                              //				fnSPI_EEPROM_WriteData(HYSTERESIS_ANTI_FRZ_POS, 4, SplitByte.rgucData);
                                              //			}
                                              //			break;
                                              //			case CONFIG_PARA_61:
                                              //			{ 
                                              //				SplitByte.fData = stConfigData.fConfigHysAntiFrzNeg;
                                              //				fnSPI_EEPROM_WriteData(HYSTERESIS_ANTI_FRZ_NEG, 4, SplitByte.rgucData);
                                              //			}
                                              //			break;
                                              //			case CONFIG_PARA_62:
                                              //			{ 
                                              //				SplitByte.fData = stConfigData.fConfigHysRecircPresPos;
                                              //				fnSPI_EEPROM_WriteData(HYSTERESIS_RECIRC_PRES_POS, 4, SplitByte.rgucData);
                                              //			}
                                              //			break;
                                              //			case CONFIG_PARA_63:
                                              //			{ 
                                              //				SplitByte.fData = stConfigData.fConfigHysRecircPresNeg;
                                              //				fnSPI_EEPROM_WriteData(HYSTERESIS_RECIRC_PRES_NEG, 4, SplitByte.rgucData);
                                              //			}
                                              //			break;
                                              //			case CONFIG_PARA_64:
                                              //			{
                                              //				SplitByte.fData = stConfigData.fConfigHysSprayPresPos;
                                              //				fnSPI_EEPROM_WriteData(HYSTERESIS_SPRAY_PRES_POS, 4, SplitByte.rgucData);
                                              //			}
                                              //			break;
                                              //			
                                              //			case CONFIG_PARA_65:
                                              //			{ 
                                              //				SplitByte.fData = stConfigData.fConfigHysSprayPresNeg;
                                              //				fnSPI_EEPROM_WriteData(HYSTERESIS_SPRAY_PRES_NEG, 4, SplitByte.rgucData);
                                              //			}
                                              //			break;
                                              //			
                                              //			case CONFIG_PARA_66:
                                              //			{ 
                                              //				SplitByte.fData = stConfigData.fConfigHardLiquidLevelHTH;
                                              //				fnSPI_EEPROM_WriteData(HHCM_LIQUIDLEVELTHRESHOLD_HIGH, 4,
                                              //															 SplitByte.rgucData);
                                              //			}
                                              //			break;
                                              //			
                                              //			case CONFIG_PARA_67:
                                              //			{ 
                                              //				SplitByte.fData = stConfigData.fConfigHardLiquidLevelLTH;
                                              //				fnSPI_EEPROM_WriteData(HHCM_LIQUIDLEVELTHRESHOLD_LOW, 4, 
                                              //															 SplitByte.rgucData);
                                              //			}
                                              //			break;
                                              //			
                                              //			case CONFIG_PARA_68:
                                              //			{ 
                                              //				SplitByte.fData = stConfigData.fConfigBaseTankHeight;
                                              //				fnSPI_EEPROM_WriteData(BHCM_LIQUIDLEVEL_HEIGHT, 4, 
                                              //															 SplitByte.rgucData);
                                              //			}
                                              //			break;
                                              //			
                                              //			case CONFIG_PARA_69:
                                              //			{ 
                                              //				SplitByte.fData = stConfigData.fConfigHardTankHeight;
                                              //				fnSPI_EEPROM_WriteData(HHCM_LIQUIDLEVEL_HEIGHT, 4,
                                              //															 SplitByte.rgucData);
                                              //			}
                                              //			break;
                                              //			
                                              //			case CONFIG_PARA_70:
                                              //			{ 
                                              //				fnSPI_EEPROM_WriteData(LIQUID_LEVEL_WINDOW_SIZE, 1,
                                              //															 &stConfigData.ucConfigLLWindowSize);
                                              //			}
                                              //			break;
                                              //			case CONFIG_PARA_71:
                                              //			{ 
                                              //				fnSPI_EEPROM_WriteData(TYPE_OF_UNIT, 1, &stConfigData.ucTypeofUnit);
                                              //			}
                                              //			break;
                                              //			case CONFIG_PARA_72:
                                              //			{ 
                                              //				SplitByte.fData = stConfigData.fConfigFlushWarnTime;
                                              //				fnSPI_EEPROM_WriteData(FLUSH_WARNNING_TIME, 4, SplitByte.rgucData);
                                              //			}
                                              //			break;
                                              //			default:
                                              //			{
                                              //				
                                              //			}
                                              //		}/*end of switch*/	
                                              //		ucEEPROMCounter = 0;	
                                              //	}/*end of if*/
                                              //}/*end of EEPROM write Function*/
                                              
                                              /*****************************************************************************
                                              **@Function		 	:  fnEstop_Check()
                                              **@Descriptions	:  Estop message check
                                              **@parameters		:  none
                                              **@return				:  none
                                              *****************************************************************************/
                                              void fnEstop_Check(void)
                                              {
                                                if(SET == StatusFlag.stStatusFlag.bEStop_Flag)
                                                {
                                                  StatusFlag.stStatusFlag.bEStop_Flag = RESET;
                                                  
                                                  /*Turn OFF OPMM valves*/
                                                  stTx1Message.ExtId = MSG_TYPE_COMMAND | CMD_VALVE_OPMM | DEST_OPMM | SOURCE_MCU;
                                                  stTx1Message.DLC = 0x02;
                                                  stTx1Message.Data[0] = 0x00;    /* ON */
                                                  stTx1Message.Data[1] = 0x20;   /*Valve A/B*/
                                                  fnCANMsg_Transmit(&stTx1Message, CAN1);		
                                                }
                                              }
                                              
                                              
                                              /******************************************************************************
                                              **@Function		 : fnAllWarnnings_Clear
                                              **@Descriptions : this funtion clear all the Warnning which flags
                                              **@parameters	 : ucData: Devic id of Module to whome MCU will send 
                                              clear warnning command
                                              **@return			 : none
                                              ******************************************************************************/
                                              void fnAllWarnnings_Clear(uint8_t ucData)
                                              {
                                                StatusFlag.stStatusFlag.bWarnningFlag = RESET; /*Error SET*/
                                                switch(ucData)
                                                {
                                                case ID_MCU:
                                                  {
                                                    uint8_t ucHBCntr = 0;
                                                    for(ucHBCntr = 0; ucHBCntr < TOTAL_HB_DEVICE; ucHBCntr++)
                                                    {					
                                                      rgucHBCntr[ucHBCntr] = 0;
                                                    }
                                                  }
                                                  break;
                                                  
                                                case ID_MFM:
                                                  {
                                                    /* Clear faults for MFM */
                                                    stTx1Message.ExtId = MSG_TYPE_COMMAND | CMD_CLEAR_WARNING |
                                                      DEST_MFM | SOURCE_MCU;
                                                    stTx1Message.DLC = 0x01;
                                                    stTx1Message.Data[0] = ucData;    /* Bit 0 0 switches on the heater*/
                                                    fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                  }
                                                  break;
                                                case ID_BHCM:
                                                  {
                                                    /* Clear faults for BHCM */
                                                    stTx1Message.ExtId = MSG_TYPE_COMMAND | CMD_CLEAR_WARNING |
                                                      DEST_BHCM | SOURCE_MCU;
                                                    stTx1Message.DLC = 0x01;
                                                    stTx1Message.Data[0] = ucData;    /* Bit 0 0 switches on the heater*/
                                                    fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                  }
                                                  break;
                                                case ID_IHCM1:
                                                  {
                                                    /* Clear faults for IHCM1 */
                                                    stTx1Message.ExtId = MSG_TYPE_COMMAND | CMD_CLEAR_WARNING |
                                                      DEST_IHCM1 | SOURCE_MCU;
                                                    stTx1Message.DLC = 0x01;
                                                    stTx1Message.Data[0] = ucData;    /* Bit 0 0 switches on the heater*/
                                                    fnCANMsg_Transmit(&stTx1Message, CAN1);	
                                                  }
                                                  break;
                                                case ID_IHCM4_HM:
                                                  {
                                                    /* Clear faults for IHCM_HM */
                                                    stTx1Message.ExtId = MSG_TYPE_COMMAND | CMD_CLEAR_WARNING |
                                                      DEST_IHCM4_HM | SOURCE_MCU;
                                                    stTx1Message.DLC = 0x01;
                                                    stTx1Message.Data[0] = ucData;    /* Bit 0 0 switches on the heater*/
                                                    fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                  }
                                                  break;
                                                case ID_HHCM:
                                                  {
                                                    /* Clear faults for HHCM */
                                                    stTx1Message.ExtId = MSG_TYPE_COMMAND | CMD_CLEAR_WARNING | 
                                                      DEST_HHCM | SOURCE_MCU;
                                                    stTx1Message.DLC = 0x01;
                                                    stTx1Message.Data[0] = ucData;    /* Bit 0 0 switches on the heater*/
                                                    fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                  }
                                                  break;		
                                                }
                                                /* Clear faults for OPMM after clear fault message to any module*/
                                                stTx1Message.ExtId = MSG_TYPE_COMMAND | CMD_CLEAR_WARNING |
                                                  DEST_OPMM | SOURCE_MCU;
                                                stTx1Message.DLC = 0x01;
                                                stTx1Message.Data[0] = ucData;    /* Bit 0 0 switches on the heater*/			
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);	
                                              }
                                              
                                              /*****************************************************************************
                                              **@Function		 :  fnAllFaults_Clear
                                              **@Descriptions :  this function Claer all the faults flags
                                              **@parameters	 :  none
                                              **@return			 :  none
                                              *****************************************************************************/
                                              void fnAllFaults_Clear(void)
                                              {
                                                uint8_t ucHBCntr;
                                                
                                                /* Clear faults for OPMM */
                                                ucDevDead_OP = 0;
                                                ucDevDead_OPMM = 0;
                                                ucDevDead_MFM = 0;
                                                ucDevDead_BHCM = 0;
                                                ucDevDead_HHCM = 0;
                                                ucDevDead_IHCM1 = 0;
                                                ucDevDead_IHCM4_HM = 0;
                                                
                                                ucMsgTxCnt_OP = 0;
                                                ucMsgTxCnt_OPMM = 0;
                                                ucMsgTxCnt_MFM = 0;
                                                ucMsgTxCnt_BHCM = 0;
                                                ucMsgTxCnt_HHCM = 0;
                                                ucMsgTxCnt_IHCM1 = 0;
                                                ucMsgTxCnt_IHCM4_HM = 0;
                                                
                                                uiHBACKfail = 0;
                                                
                                                for(ucHBCntr = 0; ucHBCntr < TOTAL_HB_DEVICE; ucHBCntr++)
                                                {					
                                                  rgucHBCntr[ucHBCntr] = 0;
                                                }
                                                
                                                fnFifo_Reset(&pstFifo_OP);
                                                fnFifo_Reset(&pstFifo_OPMM);
                                                fnFifo_Reset(&pstFifo_MFM);
                                                fnFifo_Reset(&pstFifo_BHCM);
                                                fnFifo_Reset(&pstFifo_HHCM);
                                                fnFifo_Reset(&pstFifo_IHCM1);
                                                fnFifo_Reset(&pstFifo_IHCM4_HM);
                                                
                                                
                                                /* Clear faults for MFM */
                                                stTx1Message.ExtId = MSG_TYPE_COMMAND | CMD_CLEAR_FAULT |
                                                  DEST_MFM | SOURCE_MCU;
                                                stTx1Message.DLC = 0;
                                                ucHBCntr = fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                stTx1Message.ExtId = MSG_TYPE_COMMAND | CMD_CLEAR_FAULT |	
                                                  DEST_OPMM | SOURCE_MCU;
                                                stTx1Message.DLC = 0x00;
                                                
                                                if(SET == StatusFlag.stStatusFlag.bCAN1ActiveFlag )
                                                {
                                                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                }
                                                else
                                                {
                                                  fnCANMsg_Transmit(&stTx1Message, CAN2);
                                                }
                                                
                                                
                                                /* Clear faults for BHCM */
                                                stTx1Message.ExtId = MSG_TYPE_COMMAND | CMD_CLEAR_FAULT |
                                                  DEST_BHCM | SOURCE_MCU;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /* Clear faults for IHCM1 */
                                                stTx1Message.ExtId = MSG_TYPE_COMMAND | CMD_CLEAR_FAULT |
                                                  DEST_IHCM1 | SOURCE_MCU;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);	
                                                
                                                
                                                /* Clear faults for IHCM4_HM */
                                                stTx1Message.ExtId = MSG_TYPE_COMMAND | CMD_CLEAR_FAULT |
                                                  DEST_IHCM4_HM | SOURCE_MCU;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                /* Clear faults for HHCM */
                                                stTx1Message.ExtId = MSG_TYPE_COMMAND | CMD_CLEAR_FAULT |
                                                  DEST_HHCM | SOURCE_MCU;
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                
                                                
                                                uiOperationTimeOut = 0; /*Reset the Opertion timeout Conter*/
                                                
                                                /*Operation Mode over status to OP*/
                                                stTx1Message.ExtId = MSG_TYPE_STATUS | STATUS_SYSTEM_MODE |	
                                                  DEST_OP | SOURCE_MCU;
                                                stTx1Message.DLC = 0x01;
                                                stTx1Message.Data[0] = 0x00;    /*Bit 0 System Operation Mode Over*/
                                                fnCANMsg_Transmit(&stTx1Message, CAN1);
#ifdef PROCESS_DBG
                                                DEBUG_PRINTF("Operation Mode Over\r\n");
#endif
                                                ucMCUMode = MCU_WARMUP_MODE;
                                                
                                                /*RESET all the flags*/
                                                StatusFlag.uliResetFlag[0] = 0;
                                                StatusFlag.uliResetFlag[1] = 0;
                                                StatusFlag.uliResetFlag[2] = 0;
                                                StatusFlag.uliResetFlag[3] = 0;
                                                StatusFlag.stStatusFlag.bCAN2ActiveFlag = RESET;
                                                StatusFlag.stStatusFlag.bCAN1ActiveFlag = SET;
                                              }
                                              
                                              /*****************************************************************************
                                              **@Function		 	: fnDataLogging_UART
                                              **@Descriptions	: This function is used to get data log 
                                              **@parameters		: None
                                              **@return				: None
                                              *****************************************************************************/
                                              void fnDataLogging_UART(void)
                                              {	
                                                char *pTemp, *pTemp1;
                                                
                                                if(((pTemp = strchr((const char *)rgucRecString,'#')) != NULL) && 
                                                   ((pTemp1 = strchr((const char *)rgucRecString, '$')) != NULL))
                                                {
                                                  if((pTemp1 - pTemp) > 2)
                                                  {		
                                                    
                                                    //DEBUG_PRINTF(" Command Got\r\n");
                                                    if('P' == pTemp[2])
                                                    {
                                                      DEBUG_PRINTF("               PERIODIC DATA LOG             \r\n");
                                                      fnPRDLog_Get();
                                                    }
                                                    else if('D' == pTemp[2])
                                                    {
                                                      DEBUG_PRINTF("              DIAGNOSTIC DATA LOG            \r\n");
                                                      fnDiagnosticLog_Get();
                                                    }
                                                    else if('S' == pTemp[2])
                                                    {
                                                      DEBUG_PRINTF("               STATUS DATA LOG                \r\n");
                                                      fnStatusDataLog_Get();
                                                    }
                                                    else if('F' == pTemp[2])
                                                    {
                                                      DEBUG_PRINTF("                FAULT DATA LOG               \r\n");
                                                      fnFaultLog_Get();
                                                    }
                                                    else if('A' == pTemp[2])
                                                    {
                                                      DEBUG_PRINTF("                DIAGNOSTIC DATA LOG           \r\n");
                                                      fnDiagnosticLog_Get();
                                                      
                                                      DEBUG_PRINTF("                FAULT DATA LOG                \r\n");
                                                      fnFaultLog_Get();
                                                      
                                                      DEBUG_PRINTF("                STATUS DATA LOG               \r\n");
                                                      fnStatusDataLog_Get();
                                                      
                                                      DEBUG_PRINTF("                PERIODIC DATA LOG             \r\n");
                                                      fnPRDLog_Get();
                                                    }
                                                    else if(0 == strcmp(pTemp ,"# RESET_LOG $"))
                                                    {
                                                      DEBUG_PRINTF("                   RESET DATA LOG              \r\n");
                                                      fnDelay_ms(50); /*delay before Reset Datalogs base registers*/
                                                      fnDatalogBaseReg_Reset();
                                                      fnDelay_ms(50); /*delay added for Free SPI lines*/
                                                    }
                                                    else 
                                                    {
                                                      DEBUG_PRINTF("         INVALID OPTION       \r\n");
                                                      DEBUG_PRINTF("Enter The Following Command for Data Logging\r\n");
                                                      DEBUG_PRINTF("# S $ For Status Log\r\n");
                                                      DEBUG_PRINTF("# D $ For Diagnostic Log\r\n");
                                                      DEBUG_PRINTF("# F $ For Fault Log\r\n");
                                                      DEBUG_PRINTF("# P $ For Periodic Log\r\n");
                                                      DEBUG_PRINTF("# A $ For All Data Log\r\n");
                                                      DEBUG_PRINTF("# RESET_LOG $ To RESET Data Logging\r\n");
                                                    }		
                                                  }
                                                  memset(rgucRecString, '\0', 20);
                                                  pTemp = 0;
                                                  pTemp1 = 0;
                                                  ucloopRecStr = 0;
                                                }
                                              }
                                              
                                              /*****************************************************************************
                                              **@Function		 	:  fnRuntime_Stop
                                              **@Descriptions	:  This Function performs Stop runtime action
                                              **@parameters		:  none
                                              **@return				:  none
                                              *****************************************************************************/
                                              void fnRuntime_Stop(void)
                                              {
                                                if(SET == StatusFlag.stStatusFlag.bStopRuntimeReceived)
                                                {
                                                  StatusFlag.stStatusFlag.bStopRuntimeReceived = RESET;
                                                  
                                                  /*Turn ON Recirculation valves*/
                                                  stTx1Message.ExtId = MSG_TYPE_COMMAND | CMD_VALVE_MFM | 
                                                    DEST_MFM | SOURCE_MCU;
                                                  stTx1Message.DLC = 0x02;
                                                  stTx1Message.Data[0] = 0x01;    /*ON*/
                                                  stTx1Message.Data[1] = 0x01;   /*Recirculation Valve*/
                                                  fnCANMsg_Transmit(&stTx1Message, CAN1);		
                                                  stTx1Message.ExtId = MSG_TYPE_COMMAND | CMD_VALVE_MFM |
                                                    DEST_OP | SOURCE_MCU;
                                                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                  
                                                  /* Turn OFF Heaters for HHCM */
                                                  stTx1Message.ExtId = MSG_TYPE_COMMAND | CMD_TURN_ON_OFF_HEATER |
                                                    DEST_HHCM | SOURCE_MCU;
                                                  stTx1Message.DLC = 0x01;
                                                  stTx1Message.Data[0] = 0x00;    /* switches ON the heater*/
                                                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                  
                                                  /* Turn OFF Heaters for BHCM */
                                                  stTx1Message.ExtId = MSG_TYPE_COMMAND | CMD_TURN_ON_OFF_HEATER |
                                                    DEST_BHCM | SOURCE_MCU;
                                                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                  
                                                  /* Turn OFF Heaters for BHCM */
                                                  stTx1Message.ExtId = MSG_TYPE_COMMAND | CMD_TURN_ON_OFF_HEATER |
                                                    DEST_IHCM1 | SOURCE_MCU;
                                                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                  
                                                  /* Turn OFF Heaters for IHCM4_HM */
                                                  stTx1Message.ExtId = MSG_TYPE_COMMAND | CMD_TURN_ON_OFF_HEATER |
                                                    DEST_IHCM4_HM | SOURCE_MCU;
                                                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                  
                                                  /*Turn OFF OPMM valves*/
                                                  stTx1Message.ExtId = MSG_TYPE_COMMAND | CMD_VALVE_OPMM | DEST_OPMM |
                                                    SOURCE_MCU;
                                                  stTx1Message.DLC = 0x02;
                                                  stTx1Message.Data[0] = 0x00;    /*switches OFF the valve*/
                                                  stTx1Message.Data[1] = 0xff;  
                                                  fnCANMsg_Transmit(&stTx1Message, CAN1);
                                                  
                                                  ucMCUMode = 0xFF;
                                                }
                                              }
                                              
                                              ///*****************************************************************************
                                              // **@Function		 	:  fnAmbiantData_Read
                                              // **@Descriptions	:  This Function use to read Abient Temp and humidity
                                              // **@parameters		:  none
                                              // **@return				:  none
                                              //*****************************************************************************/
                                              //void fnAmbiantData_Read(void)
                                              //{
                                              //	
                                              //	uint8_t ucEE07StatusInside = 0;
                                              //	uint8_t ucEE07StatusOutside = 0;
                                              //	float fConstA = 17.27f; /*Constant used in the Dew Point calculation*/
                                              //	float fConstB = 237.3f; /*Constant used in the DEDew Point calculation*/
                                              //	float fConstC = 0.0f;
                                              //	#ifdef GENERAL_DBG
                                              //		char rgcTemp[200];
                                              //	#endif
                                              //	
                                              //	
                                              //	if((OUTSIDE_AMBIANT_READ_TIME <= uiAmbiantReadCnt) && 
                                              //		 (RESET == StatusFlag.stStatusFlag.bOutsideReadflag))
                                              //	{
                                              //		/*Reading of Humidity and ambiant temperature after 20 second*/
                                              //		if(RESET == StatusFlag.stStatusFlag.bOutTempReadflag)
                                              //		{
                                              //			stCurrentData.fCurrOutsideAmbientTemp = fnEE07Temp_Read(I2C3);
                                              //		}
                                              //		else if(RESET == StatusFlag.stStatusFlag.bOutHumidReadflag)
                                              //		{
                                              //			stCurrentData.fCurrOutsideAmbientHumidity = fnEE07Humidity_Read(I2C3);
                                              //		}
                                              //		else if(RESET == StatusFlag.stStatusFlag.bOutStatusReadflag)
                                              //		{
                                              //			ucEE07StatusOutside = fnEE07Status_Read(I2C3);
                                              //		}
                                              //		if(((OUTSIDE_AMBIANT_READ_TIME + 2) < uiAmbiantReadCnt) &&
                                              //			 ((RESET == StatusFlag.stStatusFlag.bOutTempReadflag) ||
                                              //			  (RESET == StatusFlag.stStatusFlag.bOutHumidReadflag) ||	
                                              //			  (RESET == StatusFlag.stStatusFlag.bOutStatusReadflag)))
                                              //		{
                                              //			#ifdef GENERAL_DBG
                                              //				DEBUG_PRINTF("Outside Ambiant READ Fail\r\n");
                                              //		  #endif
                                              //			I2C_GenerateSTOP(I2C3, ENABLE); 
                                              //			ucI2CSequence = 0;
                                              //			ucI2CRxCnt=0;
                                              //			StatusFlag.stStatusFlag.bOutTempReadflag = RESET;
                                              //			StatusFlag.stStatusFlag.bOutHumidReadflag = RESET;
                                              //			StatusFlag.stStatusFlag.bOutStatusReadflag = RESET;
                                              //		}	
                                              //		
                                              
                                              //		if(((SET == StatusFlag.stStatusFlag.bOutAmbiantReadDoneflag) && 
                                              //				(0 == (ucEE07StatusOutside & 0x03))) ||
                                              //				(SET == StatusFlag.stStatusFlag.bOutsideErrorflag))
                                              //		{
                                              //			ucI2CSequence = 0;
                                              //			ucI2CRxCnt=0;
                                              //			/*this flag is used to continue reading of inside 
                                              //			sensor and stop reading of outside sensor*/
                                              //			StatusFlag.stStatusFlag.bOutsideReadflag = SET;
                                              //			StatusFlag.stStatusFlag.bOutTempReadflag = RESET;
                                              //			StatusFlag.stStatusFlag.bOutHumidReadflag = RESET;
                                              //			StatusFlag.stStatusFlag.bOutStatusReadflag = RESET;
                                              //		  StatusFlag.stStatusFlag.bOutAmbiantReadDoneflag = RESET;
                                              //			
                                              //			/*Ambiant Temp Log and Send to OP*/
                                              //			stTx1Message.DLC = FOUR_BYTES;
                                              //			if(SET == StatusFlag.stStatusFlag.bOutsideErrorflag)
                                              //			{
                                              //				#ifdef AMBIANT_DBG
                                              //						DEBUG_PRINTF("Outside ambient temp read fail\r\n");
                                              //				#endif
                                              //				stCurrentData.fCurrOutsideAmbientTemp = INVALID_FLOAT_READ;
                                              //			}
                                              //			SplitByte.fData = stCurrentData.fCurrOutsideAmbientTemp;
                                              //			
                                              //			for(ucLoopCnt = 0 ; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                              //			{
                                              //				 stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt];
                                              //			}
                                              //			stTx1Message.ExtId = MSG_TYPE_DATA | DATA_MCU_AMB_TEMP_OUTSIDE |
                                              //													 DEST_OP | SOURCE_MCU;
                                              //			fnCANMsg_Transmit(&stTx1Message, CAN1);
                                              //			 /* Array For DATA LOGGING of STATUS LOG */	
                                              //			rgucStatusLog[BYTE_TWENTYTHREE] = SplitByte.rgucData[0];
                                              //			rgucStatusLog[BYTE_TWENTYFOUR] = SplitByte.rgucData[1]; 
                                              //			rgucStatusLog[BYTE_TWENTYFIVE] = SplitByte.rgucData[2]; 
                                              //			rgucStatusLog[BYTE_TWENTYSIX] = SplitByte.rgucData[3];  
                                              //			
                                              //			/* Array For DATA LOGGING of PERIODIC LOG */	
                                              //			rgucPeriodicLog[BYTE_TWENTYSEVEN] = SplitByte.rgucData[0]; 
                                              //			rgucPeriodicLog[BYTE_TWENTYEIGHT] = SplitByte.rgucData[1]; 
                                              //			rgucPeriodicLog[BYTE_TWENTYNINE] = SplitByte.rgucData[2]; 
                                              //			rgucPeriodicLog[BYTE_THIRTY] = SplitByte.rgucData[3]; 
                                              
                                              //			/*Humidity log and Send to OP*/
                                              //			stTx1Message.DLC = FOUR_BYTES;
                                              //			if(SET == StatusFlag.stStatusFlag.bOutsideErrorflag)
                                              //			{
                                              //				stCurrentData.fCurrOutsideAmbientHumidity = INVALID_FLOAT_READ;
                                              //			}
                                              //			SplitByte.fData = stCurrentData.fCurrOutsideAmbientHumidity;
                                              //			
                                              //			for(ucLoopCnt = 0 ; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                              //			{
                                              //				 stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt];
                                              //			}
                                              //			stTx1Message.ExtId = MSG_TYPE_DATA | DATA_MCU_AMB_HUMIDITY |
                                              //													 DEST_OP | SOURCE_MCU;
                                              //			fnCANMsg_Transmit(&stTx1Message, CAN1);
                                              //			
                                              //			/* Array For DATA LOGGING of STATUS LOG */	
                                              //			rgucStatusLog[BYTE_TWENTYSEVEN] = SplitByte.rgucData[0]; 
                                              //			rgucStatusLog[BYTE_TWENTYEIGHT] = SplitByte.rgucData[1];
                                              //			rgucStatusLog[BYTE_TWENTYNINE] = SplitByte.rgucData[2]; 
                                              //			rgucStatusLog[BYTE_THIRTY] = SplitByte.rgucData[3];  
                                              //			/* Array For DATA LOGGING of PERIODIC LOG */
                                              //			rgucPeriodicLog[BYTE_THIRTYNINE] = SplitByte.rgucData[DATA_BYTE_ZERO]; 
                                              //			rgucPeriodicLog[BYTE_FOURTYONE] = SplitByte.rgucData[DATA_BYTE_ONE]; 
                                              //			rgucPeriodicLog[BYTE_FOURTYTWO] = SplitByte.rgucData[DATA_BYTE_TWO]; 
                                              //			rgucPeriodicLog[BYTE_FOURTYTHREE] = SplitByte.rgucData[DATA_BYTE_THREE];
                                              //			
                                              //			/*Dew Point Calculation and Tx to CAN*/
                                              //			
                                              //			/*Dew point Calculation*/
                                              //			/*Uncertainity in the calculation is +/-0.4 degree Celcius*/
                                              //			fConstC = ((log(stCurrentData.fCurrOutsideAmbientHumidity/100) +
                                              //								((fConstA * stCurrentData.fCurrOutsideAmbientTemp) / 
                                              //								 (fConstB + stCurrentData.fCurrOutsideAmbientTemp)))/fConstA);
                                              //			stCurrentData.fCurrDewPoint = ((fConstB * fConstC) / (1 - fConstC));
                                              //			
                                              //		  #ifdef GENERAL_DBG
                                              //				memset(rgcTemp, NULL , sizeof(rgcTemp));
                                              //				sprintf(rgcTemp,"DEW POINT: %0.2f", stCurrentData.fCurrDewPoint );
                                              //				DEBUG_PRINTF("%s\r\n",rgcTemp);
                                              //			#endif
                                              //			
                                              //			stTx1Message.DLC = FOUR_BYTES;
                                              //			if(SET == StatusFlag.stStatusFlag.bOutsideErrorflag)
                                              //			{
                                              //				stCurrentData.fCurrDewPoint = INVALID_FLOAT_READ;
                                              //			}
                                              //			
                                              //			SplitByte.fData = stCurrentData.fCurrDewPoint;
                                              //			
                                              //			for(ucLoopCnt = 0 ; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                              //			{
                                              //				 stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt];
                                              //			}
                                              //			stTx1Message.ExtId = MSG_TYPE_DATA | DATA_DEW_POINT | DEST_OP | SOURCE_MCU;
                                              //			fnCANMsg_Transmit(&stTx1Message, CAN1);	
                                              //			/* Array For DATA LOGGING of STATUS LOG */	
                                              //			rgucStatusLog[BYTE_THIRTYONE] = SplitByte.rgucData[0]; 
                                              //			rgucStatusLog[BYTE_THIRTYTWO] = SplitByte.rgucData[1];
                                              //			rgucStatusLog[BYTE_THIRTYTHREE] = SplitByte.rgucData[2];
                                              //			rgucStatusLog[BYTE_THIRTYFOUR] = SplitByte.rgucData[3]; 
                                              //			
                                              //			/* Array For DATA LOGGING of PERIODIC LOG */
                                              //			rgucPeriodicLog[BYTE_THIRTYFIVE] = SplitByte.rgucData[DATA_BYTE_ZERO]; 
                                              //			rgucPeriodicLog[BYTE_THIRTYSIX] = SplitByte.rgucData[DATA_BYTE_ONE]; 
                                              //			rgucPeriodicLog[BYTE_THIRTYSEVEN] = SplitByte.rgucData[DATA_BYTE_TWO]; 
                                              //			rgucPeriodicLog[BYTE_THIRTYEIGHT] = SplitByte.rgucData[DATA_BYTE_THREE]; 
                                              //		  /*If any occurs during EE07 read*/
                                              //			StatusFlag.stStatusFlag.bOutsideErrorflag = RESET;
                                              //		}
                                              //	}
                                              //	
                                              //	if(INSIDE_AMBIANT_READ_TIME <= uiAmbiantReadCnt)
                                              //	{
                                              //		/*Reading Ambiant temperature after 30 second*/
                                              //		if(RESET == StatusFlag.stStatusFlag.bInTempReadflag)
                                              //		{
                                              //			stCurrentData.fCurrInsideAmbientTemp = fnEE07Temp_Read(I2C1);
                                              //		}
                                              //		else if(RESET == StatusFlag.stStatusFlag.bInStatusReadflag)
                                              //		{
                                              //			ucEE07StatusInside = fnEE07Status_Read(I2C1);
                                              //		}
                                              //		if(((INSIDE_AMBIANT_READ_TIME + 2) < uiAmbiantReadCnt) && 
                                              //			 ((RESET == StatusFlag.stStatusFlag.bInTempReadflag) ||
                                              //			 (RESET == StatusFlag.stStatusFlag.bInStatusReadflag)))
                                              //		{
                                              //			#ifdef GENERAL_DBG
                                              //				DEBUG_PRINTF("Inside Ambiant READ Fail\r\n");
                                              //		  #endif
                                              //			I2C_GenerateSTOP(I2C1, ENABLE); 
                                              //			uiAmbiantReadCnt = 0;
                                              //			ucI2CSequence = 0;
                                              //			ucI2CRxCnt=0;
                                              //			StatusFlag.stStatusFlag.bInTempReadflag = RESET;
                                              //			StatusFlag.stStatusFlag.bInStatusReadflag = RESET;
                                              //		}
                                              //		
                                              //		
                                              //		if(((SET == StatusFlag.stStatusFlag.bInAmbiantReadDoneflag) && 
                                              //				(0 == (ucEE07StatusInside & 0x02))) || 
                                              //				(SET == StatusFlag.stStatusFlag.bInsideErrorflag))
                                              //		{
                                              //			uiAmbiantReadCnt = 0; /*Not here in the reading of Inside sensor*/
                                              //			ucI2CSequence = 0;
                                              //			ucI2CRxCnt=0;
                                              //			StatusFlag.stStatusFlag.bOutsideReadflag = RESET;
                                              //			StatusFlag.stStatusFlag.bInTempReadflag = RESET;
                                              //			StatusFlag.stStatusFlag.bInStatusReadflag = RESET;
                                              //		  StatusFlag.stStatusFlag.bInAmbiantReadDoneflag = RESET;
                                              //			
                                              //			/*Inside ambiant temp Data log and Send to OP*/
                                              //			stTx1Message.DLC = FOUR_BYTES;
                                              //			if(SET == StatusFlag.stStatusFlag.bInsideErrorflag)
                                              //			{
                                              //				#ifdef AMBIANT_DBG
                                              //						DEBUG_PRINTF("inside Temp read fail\r\n");
                                              //				#endif
                                              //				stCurrentData.fCurrInsideAmbientTemp = INVALID_FLOAT_READ;
                                              //			}
                                              //			SplitByte.fData = stCurrentData.fCurrInsideAmbientTemp;
                                              //		
                                              //			for(ucLoopCnt = 0; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
                                              //			{
                                              //				 stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt];
                                              //			}
                                              //			stTx1Message.ExtId = MSG_TYPE_DATA | DATA_MCU_AMB_TEMP_INSIDE | 
                                              //													 DEST_OP | SOURCE_MCU;
                                              //			fnCANMsg_Transmit(&stTx1Message, CAN1);
                                              //			/* Array For DATA LOGGING of PERIODIC LOG */	
                                              //			rgucPeriodicLog[BYTE_TWENTYTHREE] = SplitByte.rgucData[DATA_BYTE_ZERO]; 
                                              //			rgucPeriodicLog[BYTE_TWENTYFOUR] = SplitByte.rgucData[DATA_BYTE_ONE];
                                              //			rgucPeriodicLog[BYTE_TWENTYFIVE] = SplitByte.rgucData[DATA_BYTE_TWO];
                                              //			rgucPeriodicLog[BYTE_TWENTYSIX] = SplitByte.rgucData[DATA_BYTE_THREE];
                                              //			StatusFlag.stStatusFlag.bInsideErrorflag = RESET;
                                              //		}
                                              //	}
                                              //}
                                              
                                              /*****************************************************************************
                                              **@Function 			:  fnDelay_ms
                                              **@Descriptions	:	 to generate milli sec delay 
                                              **@parameters		:  dly: ms value
                                              **@return				:  None
                                              *****************************************************************************/
                                              void fnDelay_ms(uint32_t ulidly)
                                              {
                                                uint32_t uliclk, ulidly_1ms;
                                                for( ; ulidly > 0; ulidly--)  /*loop to generate user defined delay in ms*/
                                                {
                                                  /*loop to generate 1ms delay*/
                                                  for(ulidly_1ms = 0; ulidly_1ms <= 1000; ulidly_1ms++) 
                                                  {
                                                    for(uliclk = 0; uliclk<= 168; uliclk++);/*loop generate approx 1us delay*/
                                                  }
                                                }
                                              }
                                              
                                              
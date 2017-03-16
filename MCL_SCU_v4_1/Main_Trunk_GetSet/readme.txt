System clock is 180MHZ

on friday 19thfeb search for change1

if((ucPreviousProcess == SPRAYPROCESS) && 
			(SET == StatusFlag.stStatusFlag.bSprayProcessDone) &&
			(SET == StatusFlag.stStatusFlag.bInProgress))

changed to 

if((ucPreviousProcess == SPRAYPROCESS) && 
			(SET == StatusFlag.stStatusFlag.bSprayProcessDone) &&
			(RESET == StatusFlag.stStatusFlag.bInProgress))
/************************************************************************************************/
added  search for change 2
#define FAULT_SPRAY_SEQUENCE_ERROR	((uint32_t)0x000F0000)		/*Spray Sequence Fault change 2*/
/************************************************************************************************/
added  search for change 3
#define STATUS_OPMM_LIMITSW		((uint32_t)0x00120000)		/*Limit switch status change3*/

/************************************************************************************************/
added  search for change 4 on 22nd feb
//	if((ucPreviousProcess != FLUSHPROCESS) && 
	if((ucPreviousProcess == PRIMEPROCESS) && 

/***************************************************************************************************/
#define FAULT_SPRAY_DEBUG_ERROR		((uint32_t)0x00100000)		/*FAULT_SPRAY_DEBUG_ERROR change5*/
/*******************************************************************************************************/

for(uiLoop = 500; uiLoop > 0 ; uiLoop--);	 //change6

/********************************************************************************/	
/************************** Flag Related to STALL TEST change7*******************/
		uint8_t bStart_Stall_Flag:1;
		uint8_t bStall_Timer_Flag:1;
fnSTALLTEST_Commandsend();
/************************** Configuration parameters change8*******************/
float 	fConfig_TimeBeforeSendingStallcommand;  for SCU
float	fConfig_TimeStopOPMMcommand;  for OPMM      
float   fConfig_BatteryKneeVoltage;   for OPMM

/****************************implementation of STOP & Flush stop change9****************************/
	StatusFlag.stStatusFlag.bSprayProcessStarted = RESET;  //change9 

/*********************IMplementation of stall parameters change10***********************************/

/***************change10**************************** */
#define DEFAULT_VCM_STALL_UPSTROKE_UPPER_LIMIT				10
#define DEFAULT_VCM_STALL_UPSTROKE_MIN_RANGE					30
#define DEFAULT_VCM_STALL_UPSTROKE_MAX_RANGE					70
#define DEFAULT_VCM_STALL_DOWNSTROKE_UPPER_LIMIT			120
#define DEFAULT_VCM_STALL_DOWNSTROKE_MIN_RANGE				60
#define DEFAULT_VCM_STALL_DOWNSTROKE_MAX_RANGE				110


/***************************Change11 send auto home rtn command*****************************************/
/**************************************change11**********************************************************/
	if(SET == stProcessInput.stProcessFlag.bReadPipeTempComm)
		{
		/* any operation is in progress */
		/* Send Command */
			if(SET == StatusFlag.stStatusFlag.bStop_Flush_flag)
			{
			StatusFlag.stStatusFlag.bStop_Flush_flag == RESET;
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



/**************************************change11**********************************************************/
	if(SET == stProcessInput.stProcessFlag.bReadPipeTempComm)
		{
		/* any operation is in progress */
		/* Send Command */
			if(SET == StatusFlag.stStatusFlag.bStop_Flush_flag)
			{
			StatusFlag.stStatusFlag.bStop_Flush_flag == RESET;
			stTx1Message.DLC = 0;
			stTx1Message.ExtId = 	MSG_TYPE_COMMAND | COMMAND_RTN_HOME |
														DEST_OPMM | SOURCE_SCU;
			stTx1Message.IDE = CAN_ID_EXT;
			stTx1Message.RTR = CAN_RTR_DATA;
			fnCANMsg_Transmit(&stTx1Message, CAN1);
		}



/**********************************change12*************************************************************/
	//			ucPreviousProcess = FLUSHPROCESS;    //change12   



	/**********************change13 start monitoring of Material************************/	
						/* Send Start Linear Monitoring to VCM */
			stTx1Message.DLC = ONE_BYTES;
			stTx1Message.Data[DATA_BYTE_ZERO] = BIT0_SET  ;
			stTx1Message.ExtId = 	MSG_TYPE_COMMAND | CMD_LINEAR_TRANS_MONITOR |
														DEST_VCM | SOURCE_SCU;
			fnCANMsg_Transmit(&stTx1Message, CAN1);	
						/********************************************************************************/



/****************************************change14*******************************************************************************/
#define STATUS_FIRMWARE_VERSION NUMBER		((uint32_t)0x00130000)		/*Software version Number change14*/

/****************************************change15*******************************************************************************/
/*MAX PACKET SIZE 70=7TIME+52DATA+1Byte(change15 saying idle or spray or stall) (RESERVED 30BYTES CAN BE INCREASED)*/
	#define DL_PRD_PACKET_SIZE 								(0x00000047) 
fnPERIODIC_Log_Process_Based();	
/*change16*/
/*Configuring IO Expander 4 ports To Output as Read Mode change16*/
	fnExp4Data_Write ( DL_IOCONA , 0x28);	/* DL_IOCONA*/
	fnExp4Data_Write ( DL_IODIRA, 0xFF);		/* set port A output*/
	fnExp4Data_Write ( DL_IODIRB, 0xFF);		/* set port B output*/

/*change17*/
change in writing config parameter's for set parameters through wifi


/*change18*/
Adding wifi parameterrs in EEPROM

/*HCM_LIQUID_LEVEL_SAMPLING_RATE CONFIG_PARA_41 change19*/
								stTx1Message.DLC = TWO_BYTES;
changed to

stTx1Message.DLC = TWO_BYTES;

/* change20*/
added can message for data to send ip & port details of ethernet and wifi
also commented two cases 
case DATA_VCM_AIR_FLOW:
case DATA_OPMM_LINK_FAULT_CNT:
#define FIFO_DEPTH  	
(uint8_t)150 //change20 was 100

/*change21*/
addition of configurable idle time for periodic log
addition of configuration parameter Job name
addition of Pipe Diameter

/*change22*/
/*Sending first since HMI requirement change22*/

/*change23*/
/*Added*/
uint8_t ucSupervisorNameID;
uint8_t ucJobNameID;
/*Added EPPROM CHANGES write read*/
#define CURRENT_INDEX_SUPERVISOR_NAME
#define CURRENT_INDEX_JOB_NAME	

/*change24*/
/*change24 ADDED UserFlash.c file*/
/*change24 ADDED UserFlash.h file*/
/*change24 ADDED related variable and functions*/

/*change25*/
/*before updating the name save previous to flash*/
memset(jobname,'\0',strlen((char *)jobname));
strcat((char *)jobname,(char *)stConfigData.rgucConfig_JobName1);
strcat((char *)jobname,(char *)stConfigData.rgucConfig_JobName2);
strcat((char *)jobname,(char *)stConfigData.rgucConfig_JobName3);	
fnStoreJobName(ucJobNameID,&jobname[0]);	

/*before updating the name save previous to flash*/
memset(supervisorname,'\0',strlen((char *)supervisorname));
strcat((char *)supervisorname,(char *)stConfigData.rgucConfig_SupervisorName1);
strcat((char *)supervisorname,(char *)stConfigData.rgucConfig_SupervisorName2);
strcat((char *)supervisorname,(char *)stConfigData.rgucConfig_SupervisorName3);	
fnStoreSupervisorName(ucSupervisorNameID,&supervisorname[0]);	

/*change26*/ 
/* Check for Supervisor ID and Job ID Range to reset Flash*/
fnCheck_ID();	

/*change27 everything for 80% memory warning*/
	#define DL_DIAG_SEG_WARN 								(0x00000333)
	#define DL_STA_SEG_WARN 								(0x00004400)
	#define DL_FLT_SEG_WARN									(0x00007400)
	
	#ifdef 	NVSRAM1MB
	#define DL_PRD_SEG_WARN 								(0x000CE598)
	#endif
	
	#ifdef 	NVSRAM2MB
	#define DL_PRD_SEG_WARN 								(0x0017B265)
	#endif


typedef struct
{
		uint8_t bDiagWarn:1;
		uint8_t bDiagWarnTimerStart:1;
		uint8_t bStatusWarn:1;
		uint8_t bStatusWarnTimerStart:1;
		uint8_t bFaultWarn:1;
		uint8_t bFaultWarnTimerStart:1;
		uint8_t bPeriodicWarn:1;
		uint8_t bPeriodicWarnTimerStart:1;
}TypeDef_Warn_Log_Command;

typedef union
{
	TypeDef_Warn_Log_Command stWarnLogFlag;
	uint8_t ucLOGFlag;
}TypeDef_WarnlogCommand;


typedef struct
{	
uint8_t 	ucWarnLog_DiagTimerCounter;
uint8_t 	ucWarnLog_DiagSendWarnCounter;	
uint8_t 	ucWarnLog_StatusTimerCounter;
uint8_t 	ucWarnLog_StatusSendWarnCounter;
uint8_t 	ucWarnLog_FaultTimerCounter;
uint8_t 	ucWarnLog_FaultSendWarnCounter;
uint8_t 	ucWarnLog_PeriodicTimerCounter;
uint8_t 	ucWarnLog_PeriodicSendWarnCounter;	
	
}TypeDef_WarnLog;


/*Structure Variable for Log Warning*/
extern TypeDef_Warn_Log_Command stWarnLog;
/*Structure Variable for Log Warning Counter*/
extern TypeDef_WarnLog stWarningLog;
void fnWarnlog_Commandsend(void);


	
	if((SET == stWarnLog.stWarnLogFlag.bDiagWarnTimerStart))
	{
		/*counter for sending warnig for 80% full memory*/
		stWarningLog.uiWarnLog_DiagTimerCounter++;
	}
	
	if((SET == stWarnLog.stWarnLogFlag.bStatusWarnTimerStart))
	{
		/*counter for sending warnig for 80% full memory*/
		stWarningLog.uiWarnLog_StatusTimerCounter++;
	}
	
	if((SET == stWarnLog.stWarnLogFlag.bFaultWarnTimerStart))
	{
		/*counter for sending warnig for 80% full memory*/
		stWarningLog.uiWarnLog_FaultTimerCounter++;
	}
	
	if((SET == stWarnLog.stWarnLogFlag.bPeriodicWarnTimerStart))
	{
		/*counter for sending warnig for 80% full memory*/
		stWarningLog.uiWarnLog_PeriodicTimerCounter++;
	}

/*counting and sending of warning logic is done reset of send counter logic remaining and 
sending of warning message over wifi remaining*/
Resetting of sending counter will be on 
1) reset of log
2) wraping of log
3) downloading of log
and ack message remaining also resetting ofthe following on reception of ack.

stWarnLog.stWarnLogFlag.bStatusWarn = RESET;
stWarnLog.stWarnLogFlag.bStatusWarnTimerStart = RESET;
stWarningLog.uiWarnLog_StatusTimerCounter = 0;


/*change28*/
	if((DL_PRD_SEG_WARN < uliPriodicBaseAddr)&&(MAXWARNSEND > stWarningLog.ucWarnLog_PeriodicSendWarnCounter))
	{
		stWarnLog.stWarnLogFlag.bPeriodicWarn = SET;
	} 

	if((DL_FLT_SEG_WARN < uliFaultBaseAddr)&&(MAXWARNSEND > stWarningLog.ucWarnLog_FaultSendWarnCounter))
	{
		stWarnLog.stWarnLogFlag.bFaultWarn = SET;
	}

	if((DL_STA_SEG_WARN < uliStatusBaseAddr)&&(MAXWARNSEND > stWarningLog.ucWarnLog_StatusSendWarnCounter))
	{
		stWarnLog.stWarnLogFlag.bStatusWarn = SET;
	}


	if((DL_DIAG_SEG_WARN < uliBaseAddr)&&(MAXWARNSEND > stWarningLog.ucWarnLog_DiagSendWarnCounter))
	{
		stWarnLog.stWarnLogFlag.bDiagWarn = SET;
	}
  
			stWarningLog.ucWarnLog_DiagSendWarnCounter = RESET;

			stWarningLog.ucWarnLog_StatusSendWarnCounter = RESET;
	stWarningLog.ucWarnLog_FaultSendWarnCounter = RESET;
stWarningLog.ucWarnLog_PeriodicSendWarnCounter = RESET;


/*Tested Warning related implementation*/
By taking logs and seeing them in Hyperterminal 
Result: Warning created properly and message sent is ok  
reset of counter and flags are ok

Remaining: 
Warning generation over wifi command 
Acknowledge for the same & on reception resetting the flags needs to be implemented. 

NVSRAM LOGS working fine tested on 9/Aug/2016


/*change28 Configurable WIFI & ETHERNET IP,SUBNET MASK,GATEWAY,PORT NUMBER*/
/*change29*/ 
SSID PASSWORD FOR WIFI*/
/*change30*/
Sending of ETHERNET AND WIFI PARAMETERS


/*change31*/
fnSPI_EEPROM_ReadData(SCU_WIFI_SSIDADDRESS, 15, stWifiIP.rgucSSID);
stWifiIP.rgucSSID[15] =  NULL;
for(ucStrCount=0;ucStrCount < 7;ucStrCount++)
{
stConfigData.rgucConfig_WifiSSID1[ucStrCount] = stWifiIP.rgucSSID[ucStrCount];
}
for(ucStrCount=0;ucStrCount < 7;ucStrCount++)
{
stConfigData.rgucConfig_WifiSSID2[ucStrCount] = stWifiIP.rgucSSID[ucStrCount+7];
}
#ifdef GENERAL_DBG_EE
printf("96)SCU_WIFI_SSIDADDRESS = %s\r\n", stWifiIP.rgucSSID);
#endif


/*change31*/
fnSPI_EEPROM_ReadData(SCU_WIFI_PASSADDRESS, 15, stWifiIP.rgucPASWD);
stWifiIP.rgucPASWD[15] =  NULL;
for(ucStrCount=0;ucStrCount < 7;ucStrCount++)
{
stConfigData.rgucConfig_WifiPASS1[ucStrCount] = stWifiIP.rgucPASWD[ucStrCount];
}
for(ucStrCount=0;ucStrCount < 7;ucStrCount++)
{
stConfigData.rgucConfig_WifiPASS2[ucStrCount] = stWifiIP.rgucPASWD[ucStrCount+7];
}
#ifdef GENERAL_DBG_EE
printf("96)SCU_WIFI_PASSADDRESS = %s\r\n", stWifiIP.rgucPASWD);
#endif

/*SCU*/
/*change32*/
sending configuration message to HMI from tablet and to Tablet from hmi
CONFIG_SIGNATURE_BYTE
CONFIG_OPMM_BUZZER_VOLUME
CONFIG_TEMP_PRESS_ACHIEVE_BYPASS	
CONFIG_OPMM_NON_CONTACT_REMAP
CONFIG_OPMM_PERIPHRAL_BYPASS
CONFIG_SCU_PERIPHRAL_BYPASS	
CONFIG_HCM_PERIPHRAL_BYPASS	
CONFIG_VCM_PERIPHRAL_BYPASS	

/*  SCU CODE */ 
/* No change only added user flash to SVN*/	
	
/* SCU change33*/
FIFO for status and fault messages for TABLET sandesh changes
Get firmware version command 
Warning part for memory 80% implement
CAN message and ACK part implemented
Reset stWarningLog.ucWarnLog_PeriodicSendWarnCounter when download reset log or on wrapup

Not Tested

/*change34*/
Fault_Log.rgucFaultLogFlag[BYTE_ZERO] 	= RESET;
  Fault_Log.rgucFaultLogFlag[BYTE_ONE] 		= RESET;;
  Fault_Log.rgucFaultLogFlag[BYTE_TWO] 		= RESET;;
  Fault_Log.rgucFaultLogFlag[BYTE_THREE] 	= RESET;;
/*  SCU */
/* change35 refer Datalogging ver1.3*/
added bits for threshold in fault log

completed error and warning clearing

testing remaining

SCU CODE
/* version number change to v1.4 */

/*change36*/
material usage calculations changes and addition of CAN message and Variables .
float			fCurr_BaseMaterialPrimedAccumulate;
float			fCurr_HardMaterialPrimedAccumulate;
float			fCurr_BaseMaterialAppliedAccumulate;
float			fCurr_HardMaterialAppliedAccumulate;
float			fCurr_BaseTotalMaterialUsedAccumulate;
float			fCurr_HardTotalMaterialUsedAccumulate;

Accumulated Material  Usage  CAN Message
#define DATA_ACCUMULATED_MATERIAL_USED				((uint32_t)0x00170000)		/*Volume of material used*/


/*change37*/
/*Added TIME ZONE OFFSET CALCULATION AND CAN MESSAGE*/
/*Saved in EEPROM */
Time Zone Offset

/*change38*/
Add command to reset
Reset Joint Number,Hardener Material,Base Material Screen
#define COMMAND_RESET_MATERIAL		((uint32_t)0x00100000)		/*Pressure Gauge Remapping change38*/	

/*change39*/
if config message than only revert back to HMI
if(MSG_TYPE_CONFIG == (uliCanID & TYPE_MASK))


/*change40*/
WIFI SETTING FROM HMI

/*change41*/
send utc config after diagnostic

/*change42*/
addition of Job ID
/*change43*/
added all material usage values to EEPROM.
/*change44*/
#define STATUS_AIR_CONNECTED	(uint32_t)0x00110000)		/*STATUS_AIR_CONNECTED change44*/

/*change45*/
Added apply wifi settings message

/*change46*/
fault messages are sent without source address change
/*change47*/
added to required command messages to HMI

/*change48*/
addition of Job id in flash

/*change49*/
Addition of Job id index in datalog refer Dataloggihg v1.4

/*change50*/
Added for maintaining pressure
/*change51*/
added to send stall command only when System is not in prime or spray.
/*change52*/
change debouncing period to 100ms

Timer interrupt calculations

			Prescalar * period
timer overflow time =   -------------------
			System clock

			100 *18000
		     =	----------
			180000000

		     =	1/100
                     =  0.01
                     = 10 * 10^-3
		     = 10ms
 

/*change53*/

Version 1.8

Recirculation Valve functionality added.

/*change54*/
/***************************************/
version 1.9 released on 25oct2016
/***************************************/

Added Recirculation Valve Fault in fault log refer DataLogging version1.5
Added Recirculation Valve Connected in Diagnostic log refer DataLogging version1.5
Added Recirculation Valve Status in Diagnostic log refer DataLogging version1.5
Separate Diagnostic For NVSRAM1 & NVSRAM2 added in Diagnostic log refer DataLoggingVersion1.5

/***************************************/
version 2.0 released on 6nov2016

/*change55*/
 /****************************ABK*****************************/
      stTx1Message.ExtId = ((stTx1Message.ExtId & DEST_SOURCE_ADDR_CLEAR_MASK) | 
                            (DEST_TABLET	 | SOURCE_SCU));
      
      ////						stWifi.ucSendFlag = SET;
      ////						stEthernet.ucSendFlag = SET;
      fnXML_FIFO(&stTx1Message);
/*************************************************/
/*change56*/
Flag for flag released
/*************************************************/
/*************************************************/
/*change57*/
if(jobid[0] == 0xFF)
	{
			ucJobID = DEFAULT_CURRENT_INDEX_JOB_ID;
 			fnSPI_EEPROM_WriteData(CURRENT_INDEX_JOB_ID, 1, &ucJobID);
	}

/*change58*/
else
			{
					fnResetName(FLASH_Sector_6);
			}	
/*************************************************/
/***************************************/
SCU
version 2.0 released on 6nov2016
/***************************************/
/*change59*/
uint8_t bABONCmdFlag:1; /*change59*/


/********************************************/
SCU
version 2.1 released on 29/nov/2016
/********************************************/
/*change60*/
Position of IDLE,SPRAY & STALL changed in periodic log refer DataLogging version1.6
/*change61*/
Diagnostic log should be taken once PASS or FAIL

/********************************************/
SCU
version 2.1 released on 29/nov/2016 Bug as periodic log 
for idle was commented 
/********************************************/

/********************************************/
SCU
version 2.2 released on 9/Dec/2016
/********************************************/
/*change62*/
Added Prime time, Flush time & Spray time in Status log refer DataLogging version1.7
/*change63*/
Added Remote Switch Status message for Debugging refer cansheet version1.9.

/********************************************/
SCU
version 2.3 released on 12/Dec/2016 just mailed to kunjan
/********************************************/
/*change64*/
Added Remote Switch command message to do process refer cansheet version2.0.

/********************************************/
SCU
version 2.3 released on 15/Dec/2016
/********************************************/
/*change64*/
Added Remote Switch command message to do process refer cansheet version2.0.

/********************************************/
SCU
version 2.4 released on 19/Dec/2016
/********************************************/
/*change65*/
Added During downloading of datalog no writing of log

/********************************************/
SCU
version 2.5 released on 6/Jan/2017
/********************************************/
Added Debug for prime process time.

/********************************************/
SCU
version 2.6 released on 10/Jan/2017
/********************************************/
/*change66*/
Material usage will now be in litres in log
/*change67*/
stCurrentData.fCurr_PrimeTime = 0;
stCurrentData.fCurr_SprayTime = 0;
stCurrentData.fCurr_FlushTime += SplitByte.fData;

/********************************************/
SCU
version 2.7 released on 12/Jan/2017
/********************************************/
/*change68*/
Total accumulated change

/********************************************/
SCU
version 2.8 released on 13/Jan/2017
/********************************************/
/*change69*/

/********************************************/
SCU
version 2.9 released on 13/Jan/2017
/********************************************/
Bug resolved for Material usage

/********************************************/
SCU
version 3.0 released on 23/Jan/2017
/********************************************/
Reset of all logs erase all joint ids and job name and supervisor name
/********************************************/
SCU
version 3.1 released on 25/Jan/2017
/********************************************/
/*change70*/
During stall test in progress no prime or spray will perform
/*change71*/
If Auto stall test fails then manual log stall test result can be logged

/********************************************/
SCU
version 3.2 released on 25/Jan/2017
/********************************************/
	ucStallTestCount++; /*change72*/

/********************************************/
SCU
version 3.3 released on 30/Jan/2017
/********************************************/
/*change73*/
Change logic of starting stall test not waiting for flush process to complete. 

/********************************************/
SCU
version 3.4 released on 31/Jan/2017
/********************************************/
Status log issue resolved.
During stall no process will occur.

/********************************************/
SCU
version 3.5 released on 2/feb/2017
/********************************************/
/*change74*/
Spray proces failed status treated as error

/********************************************/
SCU
version 3.6 released on 3/feb/2017
/********************************************/
Flush complete implemented
/*change75*/
As soon as flush pressd during sparay or stop press during spray stall should 
happen
if(RESET == StatusFlag.stStatusFlag.bFlushSelector) //change76
(SET == StatusFlag.stStatusFlag.bStall_Cmd_Stop_Flush_flag))  /*change76*/
StatusFlag.stStatusFlag.bAuto_Flush_flag = SET ;  /*change77*/

/********************************************/
SCU
version 3.7 released on 7/feb/2017
/********************************************/
whenever spray estop flush stop set and home button is activated

/********************************************/
SCU
version 3.7_DEBUG released on 9/feb/2017
/********************************************/
Data message for Return To home added for debugging/*change76*/
plese refer cansheet 2.1

/********************************************/
SCU
version 3.8_0 released on 23/feb/2017
/********************************************/
Data message for DCLS AND HLS COUNT and case count added for debugging/*change76*/
plese refer cansheet 2.1

/********************************************/
SCU
version 3.9 released on 2/march/2017
/********************************************/
Change in retrival of index in reverse loop for logging.
and scuversion 3.9_debug is with flag setting after sending status

/********************************************/
SCU
version 4.0 released on 3/march/2017
/********************************************/
REMOTE FUNCTIONALITIES issue /*change77*/

/********************************************/
SCU
version 4.1 released on 6/march/2017
/********************************************/
No repitation of JOB NAME, SUPERVISOR NAME AND JOB ID /*change78*/
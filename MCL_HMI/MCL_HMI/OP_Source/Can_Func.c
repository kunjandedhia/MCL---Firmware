/******************************************************************************
**@file    Can_Func.c
* 
**@brief   This file contains all the functions related to CAN
******************************************************************************
*/    
/* ----------------------------------------------------------------------------
*                           Includes
* ----------------------------------------------------------------------------
*/
#include "System_Header.h"
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
CanTxMsg stTx1Message;  							/*CAN1 Tx msg */
SplitByte_TypeDef SplitByte; 					/* union to split the data as float and char */
SwapExtId_TypeDef SwapExtID; 					/* union to ulter the Received ExtId */
uint8_t ucRec_Byte[8];								/*Received byte 1 from CAN message*/
uint8_t ucDeviceFunc = 0x01;					/*Device functionality*/
uint16_t uiWDTCount = 0;
uint16_t uiAlarm1LEDBlinkCount = 0;
uint8_t ucLEDBlinkCount = 0;					/*variable for period of blinking LED*/
uint16_t uiTxCounter = 0;							/*variable to check resend time time for CAN message*/
uint16_t uiHBTimeCounter = 0;					/*Taken as uint16_t while testing to increase time count to 1 minute*/
DATA_Typedef  stCurrent_Value;
CONFIG_Typedef  stConfig_Value;
Display_DateTime stDisplayDateTime;
DIAGNOSTIC_Typedef stDiagStatus;
TypeDefStatus StatusFlag;            /*Operation Flag*/
CanTxMsg rgstTxCANBuff[TX_CAN_BUF_SIZE] = {0};
uint8_t ucStoreBuffCnt = 0;
uint8_t ucTxBuffCnt = 0;
uint8_t ucCnt=0;
CanRxMsg stCANRx1_Rcvbuff[CAN1_BUF_SIZE];
uint16_t uiStoreCAN1_BuffCnt = 0;
uint16_t uiCAN1_processCnt = 0;
uint16_t uiSCU_DiagnosticStatus = 0x00;
uint32_t uliOPMM_DIagnosticStatus = 0x00;
uint16_t uiVCM_DiagnosticStatus = 0x00;
uint16_t uiHCM_DiagnosticStatus = 0x00;
uint8_t ucSendAckMBNo = 0;
/* ----------------------------------------------------------------------------
*                           EXTERNAL FUNCTIONS
* ----------------------------------------------------------------------------
*/
void fnCANMessage_Send(uint32_t uliMessageId,uint32_t uliMsgType,uint8_t ucMsgTo);
void fnTxBuff_Shift(void);
void fnTxBuff_Store(CanTxMsg *pstTxMsg);
/* ----------------------------------------------------------------------------
*                           FUNCTIONS DECLARATION
* ----------------------------------------------------------------------------
*/

/*****************************************************************************
** @Function name	: fnDefaultConfig
** @Descriptions	: reset all to default state
** @parameters		: none 
** @return				: none
*****************************************************************************/
void fnDefaultConfig(void)
{
	/*Turn OFF all LEDs*/
	//fnLED_TurnONOFF (ALL_PORT_0_LED, FALSE,PORT_0);

	//fnLED_TurnONOFF (ALL_PORT_1_LED, FALSE,PORT_1);

	/* ON Alarm 1 Led GREEN */
	//fnLED_TurnONOFF (ALARM1_STATUS_LED_GREEN_BIT, TRUE, PORT_0);

	/* ON CAN Status Led RED */
	fnLED_TurnONOFF (PORT_LED_COM_RED, Bit_SET, PIN_LED_COM_RED);
	fnLED_TurnONOFF (PORT_LED_COM_BLUE, Bit_RESET, PIN_LED_COM_BLUE);
	fnLED_TurnONOFF (PORT_LED_COM_GREEN, Bit_RESET, PIN_LED_COM_GREEN);
	
	/* ON Board Status Led RED */
	fnLED_TurnONOFF (PORT_LED_ERR_RED, Bit_SET, PIN_LED_ERR_RED);
	fnLED_TurnONOFF (PORT_LED_ERR_BLUE, Bit_RESET, PIN_LED_ERR_BLUE);
	fnLED_TurnONOFF (PORT_LED_ERR_GREEN, Bit_RESET, PIN_LED_ERR_GREEN);
	
	/*CRC Changes*//*by Default All units in @Metric*/
	stConfig_Value.ucTypeofUnit = METRIC;

	/*Initialize with Home Screen as Front Screen*/
	ucScreenPatternNumber = SCREEN_PATTERN_1;
	ucSwitchPressFlag = TRUE;
				
	uiCurrentScreen = SCREEN_HOME;
	uirgStoreCurrentScreen[ucCurrentScreenNumber] = SCREEN_HOME;
	ucrgStoreCurrentScreenPattern[ucPatternNumberCount] = ucScreenPatternNumber;
	
//	stConfig_Value.ucConfigConnSprayvalve = 1;
//	stConfig_Value.ucConfigConnGunposition = 2;
//	stConfig_Value.ucConfigConnABvalve = 3;
//	stConfig_Value.ucConfigConnSolventvalve = 4;
//	stConfig_Value.ucConfigConnForwardvalve = 5;
//	stConfig_Value.ucConfigConnBackwardvalve = 6;
//	
//	stConfig_Value.ucConfigNCTempSensor1 = 1;
//	stConfig_Value.ucConfigNCTempSensor2 = 2;
//	stConfig_Value.ucConfigNCTempSensor3 = 3;
//	stConfig_Value.ucConfigNCTempSensor4 = 4;
			
}

/*****************************************************************************
** @Function name	: fnResetAllFlags
** @Descriptions	: reset all flags of valve status and command
** @parameters		: none 
** @return				: none
*****************************************************************************/
void fnResetAllFlags(void)
{
	StatusFlag.uliFlagData[0] = 0 ;
	StatusFlag.uliFlagData[1] = 0 ;
	StatusFlag.uliFlagData[2] = 0 ;
	StatusFlag.uliFlagData[3] = 0 ;
	StatusFlag.uliFlagData[4] = 0 ;
}
/*****************************************************************************
** @Function name	: fnResetAllData
** @Descriptions	: reset all flags of valve status and command
** @parameters		: none 
** @return				: none
*****************************************************************************/
void fnResetAllData(void)
{
	stCurrent_Value.fCurrReturnLineTemp = 0;
	stCurrent_Value.fCurrInsideAmbientTemp = 0;
	stCurrent_Value.fCurrOutsideAmbientTemp = 0;
	stCurrent_Value.fCurrOutsideAmbientHumidity = 0;
	stCurrent_Value.fCurrPipeTempAvg = 0;
	stCurrent_Value.fCurrPipeTempPos12 = 0;
	stCurrent_Value.fCurrPipeTempPos3 = 0;
	stCurrent_Value.fCurrPipeTempPos6 = 0;
	stCurrent_Value.fCurrPipeTempPos9 = 0;
	stCurrent_Value.fLegAMaterialUsed = 0;
	stCurrent_Value.fLegBMaterialUsed = 0;
	stCurrent_Value.fLegCMaterialUsed = 0;
	stCurrent_Value.fBaseTotalMaterialUsed = 0;
	stCurrent_Value.fHardTotalMaterialUsed = 0;
	stCurrent_Value.fBaseMaterialPrimed = 0;
	stCurrent_Value.fHardMaterialPrimed = 0;
	stCurrent_Value.fBaseMaterialApplied = 0;
	stCurrent_Value.fHardMaterialApplied = 0;
	stCurrent_Value.fCurrPressureGauge1 = 0;
	stCurrent_Value.fCurrPressureGauge2 = 0;
	stCurrent_Value.fCurrPressureGauge3 = 0;
	stCurrent_Value.fCurrLiquidLevelBase = 0;
	stCurrent_Value.fCurrAntifreezeLiquidTemp = 0;
	stCurrent_Value.fCurrInlineBaseMatTemp = 0;
	stCurrent_Value.fCurrInlineHardMatTemp = 0;
	stCurrent_Value.fCurrPrimeTime = 0;
	stCurrent_Value.fCurrSprayTime = 0;
	stCurrent_Value.fCurrFlushTime = 0;
	stCurrent_Value.fCurrDewPoint = 0;
	stCurrent_Value.ucCurrNumberofRotation = 0;
	stCurrent_Value.fCurrBateryStatus = 0;
	
	/*New Additon*/
	stCurrent_Value.fLiquid_Hardner_Level_Curr = 0;
	stCurrent_Value.iCurrPressureLegADown = 0;
	stCurrent_Value.iCurrPressureLegBDown = 0;
	stCurrent_Value.iCurrPressureLegCDown = 0;
	stCurrent_Value.iCurrPressureLegAUp = 0;
	stCurrent_Value.iCurrPressureLegBUp = 0;
	stCurrent_Value.iCurrPressureLegCUp = 0;
	stCurrent_Value.fCurrLinearTransducer = 0;
}
/*****************************************************************************
** @Function name	: fnResetAllData
** @Descriptions	: reset all flags of valve status and command
** @parameters		: none 
** @return				: none
*****************************************************************************/
void fnResetAllConfig(void)
{
	stConfig_Value.ucConfigConnPressureGuage1 = 1;
	stConfig_Value.ucConfigConnPressureGuage2 = 2;
	stConfig_Value.ucConfigConnPressureGuage3 = 3;	
	stConfig_Value.ucConfigConnInsideEE07 = 1;
	stConfig_Value.ucConfigConnOutsideEE07 = 2;	
	stConfig_Value.ucConfigConnStallvalve = 1;	
	stConfig_Value.ucConfigConnRecircvalve = 3;	
	stConfig_Value.ucConfigConnLinearTransducer = 1;
	
	stConfig_Value.ucConfig_ConnBaseinlineHeater = 0;
	stConfig_Value.ucConfig_ConnHardinlineHeater = 0;
	stConfig_Value.ucConfig_ConnAntifreezeHeater = 0;
	stConfig_Value.ucConfig_ConnHardHeater = 0;
	stConfig_Value.ucConfig_ConnBaseInlineTher = 0;
	stConfig_Value.ucConfig_ConnHardInlineTher = 0;
	stConfig_Value.ucConfig_ConnAntifreezeTher = 0;
	stConfig_Value.ucConfig_ConnBasereturnlineTher = 0;
	stConfig_Value.ucConfig_ConnBaseTankLL = 0;
	stConfig_Value.ucConfig_ConnHardTankLL = 0;
	
	stConfig_Value.ucConfigConnSprayvalve = 0;
	stConfig_Value.ucConfigConnGunposition = 0;
	stConfig_Value.ucConfigConnABvalve = 0;
	stConfig_Value.ucConfigConnSolventvalve = 0;
	stConfig_Value.ucConfigConnForwardvalve = 0;
	stConfig_Value.ucConfigConnBackwardvalve = 0;
	stConfig_Value.ucConfigNCTempSensor1 = 0;
	stConfig_Value.ucConfigNCTempSensor2 = 0;
	stConfig_Value.ucConfigNCTempSensor3 = 0;
	stConfig_Value.ucConfigNCTempSensor4 = 0;
	
}
/*****************************************************************************
* @Function name: fnClearErrors
* @Descriptions	: clear error flags.
* @parameters		: none 
* @return				: none
*****************************************************************************/
void fnClearErrors(void)
{
	/* Send Clear FAult Message */
	memset(stTx1Message.Data, 0 , sizeof(stTx1Message.Data));
	stTx1Message.DLC = 0;
	//stTx1Message.Data[0] = SCU_ID;

	fnCANMessage_Send(CMD_CLEAR_ERROR, MSG_TYPE_COMMAND, SCU_ID);
	
	fnAlarmMessage_Clear(SCU_FAULT,SYSTEM_ERROR);
	fnAlarmMessage_Clear(HMI_FAULT,SYSTEM_ERROR);
	fnAlarmMessage_Clear(OPMM_FAULT,SYSTEM_ERROR);
	fnAlarmMessage_Clear(VCM_FAULT,SYSTEM_ERROR);
	fnAlarmMessage_Clear(HCM_FAULT,SYSTEM_ERROR);

	StatusFlag.stStatusFlag.bAwayPipePosition = FALSE;
	StatusFlag.stStatusFlag.bTowardPipePosition = FALSE;

	fnProcessFlags_Clear();
	StatusFlag.stStatusFlag.bSystemErrorFlag = FALSE;

	StatusFlag.stStatusFlag.bSCU_Error = FALSE;
	StatusFlag.stStatusFlag.bOPMM_Error = FALSE;
	StatusFlag.stStatusFlag.bHMI_Error = FALSE;
	StatusFlag.stStatusFlag.bVCM_Error = FALSE;
	StatusFlag.stStatusFlag.bHCM_Error = FALSE;
	
	StatusFlag.stStatusFlag.bHB_Not_Rec = FALSE;

	if (( FALSE == StatusFlag.stStatusFlag.bSCU_Warning )	&&
			( FALSE == StatusFlag.stStatusFlag.bHCM_Warning ) &&
			( FALSE == StatusFlag.stStatusFlag.bHMI_Warning ) &&
			( FALSE == StatusFlag.stStatusFlag.bVCM_Warning ) &&
			( FALSE == StatusFlag.stStatusFlag.bOPMM_Warning) &&	
			( FALSE == StatusFlag.stStatusFlag.bSCU_Error ) 	&&
			( FALSE == StatusFlag.stStatusFlag.bHCM_Error ) 	&&
			( FALSE == StatusFlag.stStatusFlag.bHMI_Error ) 	&&
			( FALSE == StatusFlag.stStatusFlag.bVCM_Error ) 	&&
			( FALSE == StatusFlag.stStatusFlag.bOPMM_Error))
	{
		StatusFlag.stStatusFlag.bAnyFault = FALSE;
			/* ON Alarm 1 Led GREEN */
		fnLED_TurnONOFF (PORT_LED_ERR_GREEN, Bit_SET,PIN_LED_ERR_GREEN);
		fnLED_TurnONOFF (PORT_LED_ERR_RED, Bit_RESET,PIN_LED_ERR_RED);
		fnLED_TurnONOFF (PORT_LED_ERR_BLUE, Bit_RESET,PIN_LED_ERR_BLUE);
		
		fnLED_TurnONOFF (PORT_LED_COM_GREEN, Bit_SET,PIN_LED_COM_GREEN);
		fnLED_TurnONOFF (PORT_LED_COM_RED, Bit_RESET,PIN_LED_COM_RED);
		fnLED_TurnONOFF (PORT_LED_COM_BLUE, Bit_RESET,PIN_LED_COM_BLUE);
	}
}
/*****************************************************************************
* @Function name: fnClearWarnings
* @Descriptions	: clear warnings flags.
* @parameters		: none 
* @return				: none
*****************************************************************************/
void fnClearWarnings(void)
{
	/*Send Clear FAult Message*/
	memset(stTx1Message.Data, 0 , sizeof(stTx1Message.Data));
	stTx1Message.DLC = 1;

	/*SCU*/
	if ( TRUE == StatusFlag.stStatusFlag.bSCU_Warning)
	{
		StatusFlag.stStatusFlag.bSCU_Warning = FALSE;
		fnAlarmMessage_Clear(SCU_FAULT,SYSTEM_WARNING);
		stTx1Message.Data[0] = SCU_ID;
		fnCANMessage_Send(CMD_CLEAR_WARNING, MSG_TYPE_COMMAND, SCU_ID);
	}	
	
	/*HMI*/
	if ( TRUE == StatusFlag.stStatusFlag.bHMI_Warning )
	{
		StatusFlag.stStatusFlag.bHMI_Warning = FALSE;
		fnAlarmMessage_Clear(HMI_FAULT,SYSTEM_WARNING);
		stTx1Message.Data[0] = HMI_ID;
		fnCANMessage_Send(CMD_CLEAR_WARNING, MSG_TYPE_COMMAND, SCU_ID);
	}
	
	/*OPMM*/
	if ( TRUE == StatusFlag.stStatusFlag.bOPMM_Warning )
	{
		StatusFlag.stStatusFlag.bOPMM_Warning = FALSE;	
		fnAlarmMessage_Clear(OPMM_FAULT,SYSTEM_WARNING);
		stTx1Message.Data[0] = OPMM_ID;
		fnCANMessage_Send(CMD_CLEAR_WARNING, MSG_TYPE_COMMAND, SCU_ID);
	}
	
	/*VCM*/
	if ( TRUE == StatusFlag.stStatusFlag.bVCM_Warning )
	{
		StatusFlag.stStatusFlag.bVCM_Warning = FALSE;
		fnAlarmMessage_Clear(VCM_FAULT,SYSTEM_WARNING);
		stTx1Message.Data[0] = VCM_ID;
		fnCANMessage_Send(CMD_CLEAR_WARNING, MSG_TYPE_COMMAND, SCU_ID);
	}
	
	/*HCM*/
	if ( TRUE == StatusFlag.stStatusFlag.bHCM_Warning )
	{
		StatusFlag.stStatusFlag.bHCM_Warning = FALSE;
		fnAlarmMessage_Clear(HCM_FAULT,SYSTEM_WARNING);
		stTx1Message.Data[0] = HCM_ID;
		fnCANMessage_Send(CMD_CLEAR_WARNING, MSG_TYPE_COMMAND, SCU_ID);
	}

	/*If All Faults cleard turn on alarm 1 GREEN Status LED*/
	if (( FALSE == StatusFlag.stStatusFlag.bSCU_Warning )	&& 
			( FALSE == StatusFlag.stStatusFlag.bHCM_Warning ) &&
			( FALSE == StatusFlag.stStatusFlag.bHMI_Warning ) && 
			( FALSE == StatusFlag.stStatusFlag.bVCM_Warning ) &&
			( FALSE == StatusFlag.stStatusFlag.bOPMM_Warning) &&
			( FALSE == StatusFlag.stStatusFlag.bSCU_Error ) && 
			( FALSE == StatusFlag.stStatusFlag.bHCM_Error ) &&
			( FALSE == StatusFlag.stStatusFlag.bHMI_Error ) &&
			( FALSE == StatusFlag.stStatusFlag.bVCM_Error ) &&
			( FALSE == StatusFlag.stStatusFlag.bOPMM_Error))
	{
		StatusFlag.stStatusFlag.bAnyFault = FALSE;
		/* ON Alarm 1 Led GREEN */
		fnLED_TurnONOFF (PORT_LED_ERR_GREEN, Bit_SET,PIN_LED_ERR_GREEN);
		fnLED_TurnONOFF (PORT_LED_ERR_RED, Bit_RESET,PIN_LED_ERR_RED);
		fnLED_TurnONOFF (PORT_LED_ERR_BLUE, Bit_RESET,PIN_LED_ERR_BLUE);
		
		fnLED_TurnONOFF (PORT_LED_COM_GREEN, Bit_SET,PIN_LED_COM_GREEN);
		fnLED_TurnONOFF (PORT_LED_COM_RED, Bit_RESET,PIN_LED_COM_RED);
		fnLED_TurnONOFF (PORT_LED_COM_BLUE, Bit_RESET,PIN_LED_COM_BLUE);
		/*memset(ucrgStoredAlarmMessage[ucAlarmCountLimit],'\0', strlen(ucrgStoredAlarmMessage[ucAlarmCountLimit])+1);
		*/
	}
}
/*****************************************************************************
** @Function name		: fnCAN_Configuration
** @Descriptions		: Configures the CAN bus
** @parameters			: none
** @return					: none
*****************************************************************************/
void fnCAN_Configuration(void)
{
  GPIO_InitTypeDef stGPIO_InitStructure;
  NVIC_InitTypeDef stNVIC_InitStructure;
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APBPERIPH_GPIO, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, DISABLE);
  
  /* CAN1 Periph clock enable */
  RCC_APB1PeriphResetCmd(RCC_APB1Periph_CAN1, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
  
  /* Configure CAN pin: RX */
  stGPIO_InitStructure.GPIO_Pin = CAN_RX_PIN;
  stGPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  stGPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(CAN_PORT, &stGPIO_InitStructure);
  
  /* Configure CAN pin: TX */
  stGPIO_InitStructure.GPIO_Pin = CAN_TX_PIN;
  stGPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  stGPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(CAN_PORT, &stGPIO_InitStructure);
  
	
	GPIO_PinRemapConfig(GPIO_Remap1_CAN1,ENABLE);
 
	stGPIO_InitStructure.GPIO_Pin = CAN_ENABLE_PIN;
  stGPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  stGPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(CAN_ENABLE_PORT, &stGPIO_InitStructure);
	
	stGPIO_InitStructure.GPIO_Pin = CAN_STROBE_PIN;
  stGPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  stGPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(CAN_STROBE_PORT, &stGPIO_InitStructure);

	/* Enable CAN1 RX0 interrupt IRQ channel */
  stNVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
  stNVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  stNVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  stNVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&stNVIC_InitStructure); 
	
	/*Set CAN enable pins to default state*/
	GPIO_WriteBit(CAN_ENABLE_PORT, CAN_ENABLE_PIN ,Bit_SET);
	GPIO_WriteBit(CAN_STROBE_PORT, CAN_STROBE_PIN ,Bit_SET);
}
/*****************************************************************************
** @Function name		: fnCAN_MsgInit
** @Descriptions		: Configures the CAN, transmit and receive using interrupt.
											And CAN Filter Configuration
** @parameters			: none
** @return					: none
*****************************************************************************/
void fnCAN_MsgInit(void)
{
  CAN_InitTypeDef        stCAN_InitStructure;
  CAN_FilterInitTypeDef  stCAN_FilterInitStructure;
  
  /* CAN register init */
  CAN_DeInit(CAN1);
  CAN_StructInit(&stCAN_InitStructure);
  
  /* CAN cell init */
  stCAN_InitStructure.CAN_TTCM = DISABLE;
  stCAN_InitStructure.CAN_ABOM = DISABLE;
  stCAN_InitStructure.CAN_AWUM = DISABLE;
  stCAN_InitStructure.CAN_NART = DISABLE;
  stCAN_InitStructure.CAN_RFLM = ENABLE;
  stCAN_InitStructure.CAN_TXFP = DISABLE;
  stCAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
  
  /*Baudrate setting 125000bps*/
  stCAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
  stCAN_InitStructure.CAN_BS1 = CAN_BS1_10tq;
  stCAN_InitStructure.CAN_BS2 = CAN_BS2_7tq;
  stCAN_InitStructure.CAN_Prescaler = 16;
 
  CAN_Init(CAN1, &stCAN_InitStructure);
  
  /* CAN filter init */ /*For filter no=0*/
  stCAN_FilterInitStructure.CAN_FilterNumber = 0;
  stCAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
  stCAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
  stCAN_FilterInitStructure.CAN_FilterIdHigh = OP_CAN_FILTERIDHIGH_0;
  stCAN_FilterInitStructure.CAN_FilterIdLow = OP_CAN_FILTERIDLOW_0; 
  stCAN_FilterInitStructure.CAN_FilterMaskIdHigh = CAN_FILTERMASKIDHIGH_0;
  stCAN_FilterInitStructure.CAN_FilterMaskIdLow = CAN_FILTERMASKIDLOW_0;
  stCAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FIFO0;
  stCAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
  CAN_FilterInit(&stCAN_FilterInitStructure);
  
  /* CAN filter init */ /*For filter no=1*/
  stCAN_FilterInitStructure.CAN_FilterNumber = 1;
  stCAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
  stCAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
  stCAN_FilterInitStructure.CAN_FilterIdHigh = OP_CAN_FILTERIDHIGH_1;
  stCAN_FilterInitStructure.CAN_FilterIdLow = OP_CAN_FILTERIDLOW_1; 
  stCAN_FilterInitStructure.CAN_FilterMaskIdHigh = CAN_FILTERMASKIDHIGH_1;
  stCAN_FilterInitStructure.CAN_FilterMaskIdLow = CAN_FILTERMASKIDLOW_1;
  stCAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FIFO0;
  stCAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
  CAN_FilterInit(&stCAN_FilterInitStructure);
  
  /* CAN FIFO0 message pending interrupt enable */
  CAN_ITConfig(CAN1,CAN_IT_FMP0, ENABLE);
  
  CAN_ITConfig(CAN1, CAN_IT_FF0, ENABLE);
  CAN_ITConfig(CAN1, CAN_IT_FOV0, ENABLE);
  CAN_ITConfig(CAN1, CAN_IT_ERR, ENABLE);
  CAN_ITConfig(CAN1, CAN_IT_TME, ENABLE);
}

/*****************************************************************************
** @Function name		: fnCAN_Transmit
** @Descriptions		: Sends frame on CAN bus
** @parameters			: pstCANrxmsg: pointer to a structure receive frame which contains CAN Id,
*               			CAN DLC, CAN data and FMI number.
											CANx: where x can be 1 or 2 to select the CAN peripheral.
** @return					: none
*****************************************************************************/
void fnCAN_Transmit(CanTxMsg *pstCANtxmsg,CAN_TypeDef* CANx)
{
  CanTxMsg stTxMsg;
  uint8_t ucCnt;             /*Count variable to fill Transmit Message Frame*/
  uint8_t ucMailBoxNumber;
//  uint8_t ucStatusCAN;
  
  /*Configure CAN Transmitt Message structure*/
  stTxMsg.StdId = pstCANtxmsg->StdId;
  stTxMsg.ExtId = pstCANtxmsg->ExtId;
  stTxMsg.IDE = CAN_ID_EXT;
  stTxMsg.RTR = CAN_RTR_DATA;
  
  /* Reset Data Bufer */
  memset(stTxMsg.Data,0, sizeof(stTxMsg.Data)); 
  
  /*Update Length of CAN Data Frame*/
  stTxMsg.DLC = pstCANtxmsg->DLC;
  
  /*Update CAN Data Frame into Transmitt Structure*/
  for(ucCnt = 0; ucCnt < stTxMsg.DLC; ucCnt++)
  {   
    stTxMsg.Data[ucCnt] = pstCANtxmsg->Data[ucCnt];	
  }
  
	if(CANTXOK == CAN_TransmitStatus(CAN1,ucSendAckMBNo))
	{
		CAN_CancelTransmit(CAN1,ucMailBoxNumber);
	}
  /*Transmitt CAN message onto CAN bus*/
  ucMailBoxNumber = CAN_Transmit(CANx, &stTxMsg); 
  
  StatusFlag.stStatusFlag.bTxFlag = SET;
  	
	#ifdef GENERAL_DBG	
		//DEBUG_PRINTF("stTxMsg.ExtId = 0x%x \r\n",stTxMsg.ExtId);
	#endif		
}

/*****************************************************************************
** @Function name		: fnValidateCANMsg
** @Descriptions		: Validate received can message as per the Device ID
** @parameters			:	pstCANrxmsg: pointer to a structure receive frame which contains CAN Id,
*               			CAN DLC, CAN data and FMI number.
** @return					: none
*****************************************************************************/
void fnValidateCANMsg(CanRxMsg *pstCANrxmsg)
{
  uint32_t uliCanID = pstCANrxmsg->ExtId;
	#ifdef DBG_CAN			 
		DEBUG_PRINTF("In fnProcessCANMsg \n\r");
	#endif	
  
	#ifdef DBG_CAN			 
		DEBUG_PRINTF("ulCanID = 0x%x \n\r", uliCanID);
	#endif
  
  /************Destination Addr*************/
  switch  (uliCanID & DEST_ADDR_MASK)
  {
		case DEST_ALL:
    {
			#ifdef DBG_CAN      
				DEBUG_PRINTF("Broad Cast Message..\r\n");
			#endif	
      /*message is for ALL so precess it*/
      fnProcessCANMsg(pstCANrxmsg); 
    }break;
    
    case DEST_HMI:
		{
			#ifdef DBG_CAN      
				DEBUG_PRINTF("Msg for HMI..\r\n");
			#endif	
			/*message is for HMI so precess it*/
			fnProcessCANMsg(pstCANrxmsg); 
		}break;
		default:
    {     
      break;
    }	
  }
}

/*****************************************************************************
** @Function Name		: fnProcessCANMsg
** @Descriptions		: Process received can message
** @parameters			: pstCANrxmsg: pointer to a structure receive frame which contains CAN Id,
**	               		CAN DLC, CAN data and FMI number.
** @return					: none
*****************************************************************************/
void fnProcessCANMsg(CanRxMsg *pstCANrxmsg)
{
	#ifdef GENERAL_DBG
		uint8_t  rgcTempCAN[200];
	#endif
	
	uint8_t ucCnt = 0;
  uint32_t uliCanID = pstCANrxmsg->ExtId;
	
	/* Reset Data Bufer */
  memset(ucRec_Byte,0, sizeof(ucRec_Byte));
	
	for (ucCnt=0; ucCnt < pstCANrxmsg->DLC; ucCnt++)
	{
		ucRec_Byte[ucCnt] = pstCANrxmsg->Data[ucCnt];
	}
 
	#ifdef DBG_CAN      
		DEBUG_PRINTF("In fnProcessCANMsg..\r\n");
	#endif	
  
	if ( ACKNOWLEDGEMENT != (pstCANrxmsg->ExtId & ACKNOWLEDGEMENT) )
	{
		fnSendACK(pstCANrxmsg); 
	}
	
  /************Type of message*************/
  switch (uliCanID & TYPE_MASK)
		{
		case MSG_TYPE_BROADCAST:		/*Broadcast Message*/
    {
			#ifdef DBG_CAN      
				DEBUG_PRINTF("Msg Type: MSG_TYPE_BROADCAST \r\n");
			#endif
      switch (uliCanID & MSG_MASK) 
      {
				case BCAST_ESTOP:
					{
						#ifdef DBG_CAN      
							DEBUG_PRINTF("Msg ID: BCAST_ESTOP \r\n");
						#endif
					}break;
				case BCAST_HEARTBEAT:
					{
						#ifdef DBG_CAN      
							DEBUG_PRINTF("Msg ID: CMD_HEARTBEAT \r\n");
						#endif								
						StatusFlag.stStatusFlag.bHeartBeatMsg = TRUE;
						uiHBTimeCounter = 0;
						
						StatusFlag.stStatusFlag.bCANFaultMCU = RESET;
						StatusFlag.stStatusFlag.bCANFaultFlag = RESET;
					}break;
				
			}
    }break;
    
		case MSG_TYPE_COMMAND:			/*Command Message*/
    {
			#ifdef DBG_CAN
				DEBUG_PRINTF("Msg Type: MSG_TYPE_COMMAND \r\n");
			#endif
			switch (uliCanID & MSG_MASK)
				{
				case ACKNOWLEDGEMENT:
					{
						#ifdef DBG_CAN
							DEBUG_PRINTF("Msg ID: ACK received\r\n");
						#endif
						StatusFlag.stStatusFlag.bACK_Received = TRUE;
					}break;
				    /*change9*/
					/* Command from HMI to Clear all Error */
				case CMD_CLEAR_ERROR: 
						{
						      
								//#ifdef DBG_CAN_PARSER_COMMAND      
										DEBUG_PRINTF(" CMD_CLEAR_ERROR\r\n");
						//		#endif	
							fnAlarmMessage_Clear(SCU_FAULT,SYSTEM_ERROR);
							fnAlarmMessage_Clear(HMI_FAULT,SYSTEM_ERROR);
							fnAlarmMessage_Clear(OPMM_FAULT,SYSTEM_ERROR);
							fnAlarmMessage_Clear(VCM_FAULT,SYSTEM_ERROR);
							fnAlarmMessage_Clear(HCM_FAULT,SYSTEM_ERROR);

							StatusFlag.stStatusFlag.bAwayPipePosition = FALSE;
							StatusFlag.stStatusFlag.bTowardPipePosition = FALSE;

							fnProcessFlags_Clear();
							StatusFlag.stStatusFlag.bSystemErrorFlag = FALSE;

							StatusFlag.stStatusFlag.bSCU_Error = FALSE;
							StatusFlag.stStatusFlag.bOPMM_Error = FALSE;
							StatusFlag.stStatusFlag.bHMI_Error = FALSE;
							StatusFlag.stStatusFlag.bVCM_Error = FALSE;
							StatusFlag.stStatusFlag.bHCM_Error = FALSE;
	
								StatusFlag.stStatusFlag.bHB_Not_Rec = FALSE;

								if (( FALSE == StatusFlag.stStatusFlag.bSCU_Warning )	&&
										( FALSE == StatusFlag.stStatusFlag.bHCM_Warning ) &&
										( FALSE == StatusFlag.stStatusFlag.bHMI_Warning ) &&
										( FALSE == StatusFlag.stStatusFlag.bVCM_Warning ) &&
										( FALSE == StatusFlag.stStatusFlag.bOPMM_Warning) &&	
										( FALSE == StatusFlag.stStatusFlag.bSCU_Error ) 	&&
										( FALSE == StatusFlag.stStatusFlag.bHCM_Error ) 	&&
										( FALSE == StatusFlag.stStatusFlag.bHMI_Error ) 	&&
										( FALSE == StatusFlag.stStatusFlag.bVCM_Error ) 	&&
										( FALSE == StatusFlag.stStatusFlag.bOPMM_Error))
												{
													StatusFlag.stStatusFlag.bAnyFault = FALSE;
												}
												ucSwitchPressFlag = TRUE;
        }
        break;
				/*change9*/
        /* Command from HMI to Clear all Warning */
      case CMD_CLEAR_WARNING: 
        {
       
						//#ifdef DBG_CAN_PARSER_COMMAND      
									DEBUG_PRINTF(" CMD_CLEAR_WARNING\r\n");
					//		#endif	
					switch (pstCANrxmsg->Data[0])
								{
						case SCU_ID :
									{
										if ( TRUE == StatusFlag.stStatusFlag.bSCU_Warning)
												{
												DEBUG_PRINTF(" CMD_CLEAR_WARNING SCU_ID\r\n");
												StatusFlag.stStatusFlag.bSCU_Warning = FALSE;
												fnAlarmMessage_Clear(SCU_FAULT,SYSTEM_WARNING);
												}	
									}
									break;
								/*HMI*/
						case HMI_ID:
						{
								if ( TRUE == StatusFlag.stStatusFlag.bHMI_Warning )
								{
								DEBUG_PRINTF(" CMD_CLEAR_WARNING HMI_ID\r\n");
									StatusFlag.stStatusFlag.bHMI_Warning = FALSE;
									fnAlarmMessage_Clear(HMI_FAULT,SYSTEM_WARNING);
								}
						}
							break;	
								/*OPMM*/
						case OPMM_ID:
						{
								if ( TRUE == StatusFlag.stStatusFlag.bOPMM_Warning )
								{
								DEBUG_PRINTF(" CMD_CLEAR_WARNING OPMM_ID\r\n");
									StatusFlag.stStatusFlag.bOPMM_Warning = FALSE;	
									fnAlarmMessage_Clear(OPMM_FAULT,SYSTEM_WARNING);
									}
								
								}
								break;
								
								/*VCM*/
								case VCM_ID:
										{
											if ( TRUE == StatusFlag.stStatusFlag.bVCM_Warning )
											{
									DEBUG_PRINTF(" CMD_CLEAR_WARNING VCM_ID\r\n");
												StatusFlag.stStatusFlag.bVCM_Warning = FALSE;
												fnAlarmMessage_Clear(VCM_FAULT,SYSTEM_WARNING);
											}
										}
										break;
								
								/*HCM*/
										case HCM_ID:
										{
											if ( TRUE == StatusFlag.stStatusFlag.bHCM_Warning )
													{
															DEBUG_PRINTF(" CMD_CLEAR_WARNING HCM_ID\r\n");
													StatusFlag.stStatusFlag.bHCM_Warning = FALSE;
													fnAlarmMessage_Clear(HCM_FAULT,SYSTEM_WARNING);
													}
												}
												break;
											}

								/*If All Faults cleard turn on alarm 1 GREEN Status LED*/
								if (( FALSE == StatusFlag.stStatusFlag.bSCU_Warning )	&& 
										( FALSE == StatusFlag.stStatusFlag.bHCM_Warning ) &&
										( FALSE == StatusFlag.stStatusFlag.bHMI_Warning ) && 
										( FALSE == StatusFlag.stStatusFlag.bVCM_Warning ) &&
										( FALSE == StatusFlag.stStatusFlag.bOPMM_Warning) &&
										( FALSE == StatusFlag.stStatusFlag.bSCU_Error ) && 
										( FALSE == StatusFlag.stStatusFlag.bHCM_Error ) &&
										( FALSE == StatusFlag.stStatusFlag.bHMI_Error ) &&
										( FALSE == StatusFlag.stStatusFlag.bVCM_Error ) &&
										( FALSE == StatusFlag.stStatusFlag.bOPMM_Error))
								{
									StatusFlag.stStatusFlag.bAnyFault = FALSE;
									/* ON Alarm 1 Led GREEN */
									fnLED_TurnONOFF (PORT_LED_ERR_GREEN, Bit_SET,PIN_LED_ERR_GREEN);
									fnLED_TurnONOFF (PORT_LED_ERR_RED, Bit_RESET,PIN_LED_ERR_RED);
									
									fnLED_TurnONOFF (PORT_LED_COM_GREEN, Bit_SET,PIN_LED_COM_GREEN);
									fnLED_TurnONOFF (PORT_LED_COM_RED, Bit_RESET,PIN_LED_COM_RED);
									/*memset(ucrgStoredAlarmMessage[ucAlarmCountLimit],'\0', strlen(ucrgStoredAlarmMessage[ucAlarmCountLimit])+1);
									*/
								}

												ucSwitchPressFlag = TRUE;
							
//							uiCurrentScreen = SCREEN_CLEAR_FAULTS;
//							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_CLEAR_FAULTS;
//							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
        }
        break;
				     /* Command recieved from HMI to turn on/off Laser Beam of OPMM  */
      case CMD_JOINT_MATERIAL_RESET: 
        {
          
#ifdef DBG_CAN_PARSER_COMMAND      
          DEBUG_PRINTF(" COMMAND_RESET_MATERIAL\r\n");
#endif	
        	stCurrent_Value.fBaseMatTotalJoint = RESET;
							
						stCurrent_Value.fHardMatTotalJoint = RESET;
					
						stCurrent_Value.fBaseMatPrimeAcc = RESET;
					
						stCurrent_Value.fHardMatPrimeAcc = RESET;
					
						stCurrent_Value.fBaseMatSprayAcc = RESET;
					
						stCurrent_Value.fHardMatSprayAcc = RESET;
					
						stCurrent_Value.fBaseMatTotalAcc = RESET;
					
					
						stCurrent_Value.fHardMatTotalAcc = RESET;
	ucSwitchPressFlag = TRUE;
					
				}
				break;

       default:
        break;
      }
     }break;
 
    
		case MSG_TYPE_FAULT:				/*Fault Message*/
    {
			#ifdef DBG_CAN      
				DEBUG_PRINTF("Msg Type: MSG_TYPE_FAULT \r\n");
			#endif	
			
			if (( SCREEN_ALARMS_1 == uiCurrentScreen  ) || ( SCREEN_ALARMS_2 == uiCurrentScreen  ) ||
					( SCREEN_ALARMS_3 == uiCurrentScreen  ) || ( SCREEN_ALARMS_4 == uiCurrentScreen  ))
				{
					ucSwitchPressFlag = TRUE;
				}

      switch (uliCanID & MSG_MASK)
      {
				/*Error/Warning*/
				case FAULT_OPMM_VALVE:
        {
					#ifdef DBG_CAN      
						DEBUG_PRINTF("FAULT_OPMM_VALVE\r\n");
					#endif		
					
					if ( OPMM_VL_SPRAY_GUN == (ucRec_Byte[0] & OPMM_VL_SPRAY_GUN) )
 					{
						StatusFlag.stStatusFlag.bAnyFault = TRUE;
						StatusFlag.stStatusFlag.bOPMM_Error = TRUE;
						StatusFlag.stStatusFlag.bErrorFlag = TRUE;
						fnAlarmMessage_Store(OPMM_SPRAY_VALVE_FAULT,OPMM_FAULT,SYSTEM_ERROR);									
					}
					if ( OPMM_VL_SPRAY_HEAD == (ucRec_Byte[0] & OPMM_VL_SPRAY_HEAD) )
 					{
						StatusFlag.stStatusFlag.bAnyFault = TRUE;
						StatusFlag.stStatusFlag.bOPMM_Error = TRUE;
						StatusFlag.stStatusFlag.bErrorFlag = TRUE;
						fnAlarmMessage_Store(OPMM_SPRAY_HEAD_VALVE_FAULT,OPMM_FAULT,SYSTEM_ERROR);								
					}
					if	( OPMM_VL_AB == (ucRec_Byte[0] & OPMM_VL_AB) )
 					{
						StatusFlag.stStatusFlag.bAnyFault = TRUE;
						StatusFlag.stStatusFlag.bOPMM_Error = TRUE;
						StatusFlag.stStatusFlag.bErrorFlag = TRUE;
						fnAlarmMessage_Store(OPMM_BASE_MAT_VALVE_FAULT,OPMM_FAULT,SYSTEM_ERROR);					
					}
					if ( OPMM_VL_FLUSH == (ucRec_Byte[0] & OPMM_VL_FLUSH) )
 					{
						StatusFlag.stStatusFlag.bAnyFault = TRUE;
						StatusFlag.stStatusFlag.bOPMM_Error = TRUE;
						StatusFlag.stStatusFlag.bErrorFlag = TRUE;
						fnAlarmMessage_Store(OPMM_FLUSH_VALVE_FAULT,OPMM_FAULT,SYSTEM_ERROR);										
					}
					if ( OPMM_VL_FORWARD == (ucRec_Byte[0] & OPMM_VL_FORWARD) )
 					{
						StatusFlag.stStatusFlag.bOPMM_Warning = TRUE;
						StatusFlag.stStatusFlag.bWarningFlag = TRUE;
						fnLED_TurnONOFF (PORT_LED_ERR_BLUE, Bit_SET,PIN_LED_ERR_BLUE); /*change26*/
						fnAlarmMessage_Store(OPMM_FORWARD_VALVE_FAULT,OPMM_FAULT,SYSTEM_WARNING);			
					}
					if ( OPMM_VL_BACKWARD == (ucRec_Byte[0] & OPMM_VL_BACKWARD) )
 					{
						StatusFlag.stStatusFlag.bOPMM_Warning = TRUE;
						StatusFlag.stStatusFlag.bWarningFlag = TRUE;
						fnLED_TurnONOFF (PORT_LED_ERR_BLUE, Bit_SET,PIN_LED_ERR_BLUE); /*change26*/
						fnAlarmMessage_Store(OPMM_BACKWARD_VALVE_FAULT,OPMM_FAULT,SYSTEM_WARNING);											
					}
        }break;
				
				/*Error*/
				case FAULT_STALL_VALVE:
        {
					
					/*change12*/
					if ( VCM_VL_STALL == (ucRec_Byte[0] & VCM_VL_STALL) )
 					{/*Stall valve fault*/
						StatusFlag.stStatusFlag.bAnyFault = TRUE;
						StatusFlag.stStatusFlag.bVCM_Error = TRUE;
						StatusFlag.stStatusFlag.bErrorFlag = TRUE;
						#ifdef DBG_CAN      
						DEBUG_PRINTF("FAULT_STALL_VALVE\r\n");
					#endif	
						fnAlarmMessage_Store(VCM_STALL_VALVE_FAULT,VCM_FAULT,SYSTEM_ERROR);								
					}
					if ( VCM_VL_RECIRCULATION == (ucRec_Byte[0] & VCM_VL_RECIRCULATION) )
 					{
						StatusFlag.stStatusFlag.bAnyFault = TRUE;
						StatusFlag.stStatusFlag.bVCM_Error = TRUE;
						StatusFlag.stStatusFlag.bErrorFlag = TRUE;
						fnAlarmMessage_Store(VCM_RECIRC_VALVE_FAULT,VCM_FAULT,SYSTEM_ERROR);								
					}

					
        }break;
								
				/*LOW- WARNING*/
				/*High- ERROR*/
				case FAULT_TEMP_BEYOND_TH:
        {
					#ifdef DBG_CAN      
						DEBUG_PRINTF("FAULT_TEMP_BEYOND_TH\r\n");
					#endif	
					if ( BIT_0 == (ucRec_Byte[1] & BIT_0) )															/*LOW TH*/
					{
						/*Low threshold*/
						StatusFlag.stStatusFlag.bWarningFlag = TRUE;										
						StatusFlag.stStatusFlag.bHCM_Warning = TRUE;
						fnLED_TurnONOFF (PORT_LED_ERR_BLUE, Bit_SET,PIN_LED_ERR_BLUE); /*change26*/
						if ( BIT_0 == (ucRec_Byte[0] & BIT_0) )																			/*Inline base*/
						{
							fnAlarmMessage_Store(INL_BASE_MAT_TEMP_BELOW_TH,HCM_FAULT,SYSTEM_WARNING);
						}else if ( BIT_1 == (ucRec_Byte[0] & BIT_1) )																/*Inline hard*/
						{
							fnAlarmMessage_Store(INL_HARD_MAT_TEMP_BELOW_TH,HCM_FAULT,SYSTEM_WARNING);
						}else if ( BIT_2 == (ucRec_Byte[0] & BIT_2) )																/*Antifreez*/
						{
							fnAlarmMessage_Store(ANTIFREEZ_LOW_TH_FAULT,HCM_FAULT,SYSTEM_WARNING);
						}else if ( BIT_3 == (ucRec_Byte[0] & BIT_3) )																/*Base Return Line*/
						{
							fnAlarmMessage_Store(RT_LINE_TEMP_BELOW_TH,HCM_FAULT,SYSTEM_WARNING);
						}			
					}     
					else if ( BIT_1 == (ucRec_Byte[1] & BIT_1) )												/*HIGH TH*/
					{
						/*High threshold*/
						StatusFlag.stStatusFlag.bAnyFault = TRUE;
						StatusFlag.stStatusFlag.bErrorFlag = TRUE;										
						StatusFlag.stStatusFlag.bHCM_Error = TRUE;
						
						if ( BIT_0 == (ucRec_Byte[0] & BIT_0) )																			/*Inline base*/
						{
							fnAlarmMessage_Store(INL_BASE_MAT_TEMP_ABOVE_TH,HCM_FAULT,SYSTEM_ERROR);
						}else if ( BIT_1 == (ucRec_Byte[0] & BIT_1) )																/*Inline hard*/
						{
							fnAlarmMessage_Store(INL_HARD_MAT_TEMP_ABOVE_TH,HCM_FAULT,SYSTEM_ERROR);
						}else if ( BIT_2 == (ucRec_Byte[0] & BIT_2) )																/*Antifreez*/
						{
							fnAlarmMessage_Store(ANTIFREEZ_HIGH_TH_FAULT,HCM_FAULT,SYSTEM_ERROR);
						}else if ( BIT_3 == (ucRec_Byte[0] & BIT_3) )																/*Base Return Line*/
						{
							fnAlarmMessage_Store(RT_LINE_TEMP_ABOVE_TH,HCM_FAULT,SYSTEM_ERROR);
						}
					}     
        }break;
				/*LOW- WARNING*/
				/*High- ERROR*/
				case FAULT_PRESSURE_BEYOND_TH	:
        {
					#ifdef DBG_CAN      
						DEBUG_PRINTF("FAULT_PGAUGE1_BEYOND_TH\r\n");
					#endif	
					
					if ( BIT_0 == (ucRec_Byte[1] & BIT_0) )
					{
						/*Low threshold*/
						StatusFlag.stStatusFlag.bWarningFlag = TRUE;										
						StatusFlag.stStatusFlag.bVCM_Warning = TRUE;
fnLED_TurnONOFF (PORT_LED_ERR_BLUE, Bit_SET,PIN_LED_ERR_BLUE); /*change26*/
						if ( BIT_0 == (ucRec_Byte[0] & BIT_0) )																						/*Pressure Gauge 1*/
						{
							fnAlarmMessage_Store(PRESSURE_GAUGE_1_BELOW_TH,VCM_FAULT,SYSTEM_WARNING);	
						}else if ( BIT_1 == (ucRec_Byte[0] & BIT_1) )																			/*Pressure Gauge 2*/
						{
							fnAlarmMessage_Store(PRESSURE_GAUGE_2_BELOW_TH,VCM_FAULT,SYSTEM_WARNING);	
						}else if ( BIT_2 == (ucRec_Byte[0] & BIT_2) )																			/*Pressure Gauge 3*/
						{
							fnAlarmMessage_Store(PRESSURE_GAUGE_3_BELOW_TH,VCM_FAULT,SYSTEM_WARNING);	
						}
					}
					else if ( BIT_1 == (ucRec_Byte[1] & BIT_1) )
					{
						/*High threshold*/
						StatusFlag.stStatusFlag.bAnyFault = TRUE;
						StatusFlag.stStatusFlag.bErrorFlag = TRUE;					
						StatusFlag.stStatusFlag.bVCM_Error = TRUE;
						
						if ( BIT_0 == (ucRec_Byte[0] & BIT_0) )																						/*Pressure Gauge 1*/
						{
							fnAlarmMessage_Store(PRESSURE_GAUGE_1_ABOVE_TH,VCM_FAULT,SYSTEM_ERROR);	
						}else if ( BIT_1 == (ucRec_Byte[0] & BIT_1) )																			/*Pressure Gauge 2*/
						{
							fnAlarmMessage_Store(PRESSURE_GAUGE_2_ABOVE_TH,VCM_FAULT,SYSTEM_ERROR);	
						}else if ( BIT_2 == (ucRec_Byte[0] & BIT_2) )																			/*Pressure Gauge 3*/
						{
							fnAlarmMessage_Store(PRESSURE_GAUGE_3_ABOVE_TH,VCM_FAULT,SYSTEM_ERROR);	
						}
					}
        }break;
				/*LOW- WARNING*/
				/*High- WARNING*/
				case FAULT_LQ_LV_BEYOND_TH:
        {
					#ifdef DBG_CAN      
						DEBUG_PRINTF("FAULT_LQ_LV_BEYOND_TH\r\n");
					#endif	
					
					StatusFlag.stStatusFlag.bWarningFlag = TRUE;										
					StatusFlag.stStatusFlag.bHCM_Warning = TRUE;
					fnLED_TurnONOFF (PORT_LED_ERR_BLUE, Bit_SET,PIN_LED_ERR_BLUE); /*change26*/
					if ( LOW_TH == (ucRec_Byte[1] & 0x01) ) 														/*LOW TH*/
					{
						/*Low threshold*/
						if ( BIT_0 == (ucRec_Byte[0] & BIT_0) )																						/*Base Tank*/
						{
							fnAlarmMessage_Store(BHCM_BASE_MAT_LEVEL_BELOW_TH,HCM_FAULT,SYSTEM_WARNING);	
						}else if ( BIT_1 == ucRec_Byte[0] )																			/*Hardner Tank*/
						{
							fnAlarmMessage_Store(HHCM_BASE_MAT_LEVEL_BELOW_TH,HCM_FAULT,SYSTEM_WARNING);	
						}
					}
					else if ( HIGH_TH == ucRec_Byte[1] )												/*HIGH TH*/
					{
						/*High threshold*/
						if ( BIT_0 == (ucRec_Byte[0] & BIT_0) )																						/*Base Tank*/
						{
							fnAlarmMessage_Store(BHCM_BASE_MAT_LEVEL_ABOVE_TH,HCM_FAULT,SYSTEM_WARNING);	
						}else if ( BIT_1 == ucRec_Byte[0] )																			/*Hardner Tank*/
						{
							fnAlarmMessage_Store(HHCM_BASE_MAT_LEVEL_ABOVE_TH,HCM_FAULT,SYSTEM_WARNING);	
						}
					}
        }break;
				

				/*High- WARNING*/
				case FAULT_PIPE_TEMP_OUT_OF_RANGE	:
        {
					StatusFlag.stStatusFlag.bWarningFlag = TRUE;
					StatusFlag.stStatusFlag.bWarningMEssageTimer = TRUE;
					
					StatusFlag.stStatusFlag.bOPMM_Warning = TRUE;
					fnLED_TurnONOFF (PORT_LED_ERR_BLUE, Bit_SET,PIN_LED_ERR_BLUE); /*change26*/
					#ifdef DBG_CAN      
						DEBUG_PRINTF("FAULT_PIPE_TEMP_OUT_OF_RANGE\r\n");
					#endif	
					fnAlarmMessage_Store(OPMM_PIPE_TEMP_BEYOND_RANGE,SCU_FAULT,SYSTEM_WARNING);
        }break;
				
				/*LOW- WARNING*/
				case FAULT_HEATER:
        {
					StatusFlag.stStatusFlag.bWarningFlag = TRUE;
					StatusFlag.stStatusFlag.bHCM_Warning = TRUE;
					fnLED_TurnONOFF (PORT_LED_ERR_BLUE, Bit_SET,PIN_LED_ERR_BLUE); /*change26*/
					#ifdef DBG_CAN      
						DEBUG_PRINTF("FAULT_DEFECTIVE_HEATER\r\n");
					#endif	
					
					if ( BIT_0 == ucRec_Byte[0] )							/*Inline Base Heater Fault*/
					{
						fnAlarmMessage_Store(INL_BASE_HEATER_FAULT,HCM_FAULT,SYSTEM_WARNING);
					}
					if ( BIT_1 == ucRec_Byte[0] )							/*Inline Hard Heater Fault*/
					{
						fnAlarmMessage_Store(INL_HARD_HEATER_FAULT,HCM_FAULT,SYSTEM_WARNING);
					}
					if ( BIT_2 == ucRec_Byte[0] )							/*Antifreez Heater Fault*/
					{
						fnAlarmMessage_Store(ANTIFREEZ_HEATER_FAULT,HCM_FAULT,SYSTEM_WARNING);
					}
					if ( BIT_3 == ucRec_Byte[0] )							/*Hard Tank Heater Fault*/
					{
						fnAlarmMessage_Store(HARD_TANK_HEATER_FAULT,HCM_FAULT,SYSTEM_WARNING);
					}
				}break;
				/*LOW- WARNING*/
				case FAULT_SPRAY_GUN_MOTION:
        {
					/*Spray gun motion error*/
					StatusFlag.stStatusFlag.bWarningFlag = TRUE;				
					StatusFlag.stStatusFlag.bOPMM_Warning = TRUE;
					fnLED_TurnONOFF (PORT_LED_ERR_BLUE, Bit_SET,PIN_LED_ERR_BLUE);
					#ifdef DBG_CAN      
						DEBUG_PRINTF("FAULT_SPRAY_GUN_MOTION\r\n");
					#endif	
					fnAlarmMessage_Store(SPRAY_GUN_MOTION_FAULT,OPMM_FAULT,SYSTEM_WARNING);         
        }break;
				/*High- ERROR*/
				case FAULT_LT_OUT_OF_RANGE:
        {
					/*Linear transducer out of range fault*/
					StatusFlag.stStatusFlag.bAnyFault = TRUE;
					StatusFlag.stStatusFlag.bErrorFlag = TRUE;
					ucSwitchPressFlag = TRUE;
					
					StatusFlag.stStatusFlag.bVCM_Error = TRUE;

					fnAlarmMessage_Store(LINEAR_TRANSDUCER_OUT_OF_RANGE,VCM_FAULT,SYSTEM_ERROR);   
					#ifdef DBG_CAN      
						DEBUG_PRINTF("FAULT_LT_OUT_OF_RANGE\r\n");
					#endif					
        }break;
				/*High- ERROR*/
				case FAULT_OPMM_ROTATION:
        {
					StatusFlag.stStatusFlag.bAnyFault = TRUE;
					StatusFlag.stStatusFlag.bErrorFlag = TRUE;				
					StatusFlag.stStatusFlag.bOPMM_Error = TRUE;
					
					#ifdef DBG_CAN      
						DEBUG_PRINTF("FAULT_OPMM_ROTATION_FAULT\r\n");
					#endif					
					if ( DCLS_TIMER == ucRec_Byte[0] )		/*DCLS timer fault*/
					{
						fnAlarmMessage_Store(OPMM_DCLS_TIMER_FAULT,OPMM_FAULT,SYSTEM_ERROR);
						#ifdef DBG_CAN      
							DEBUG_PRINTF("FAULT_OPMM_ROTATION_FAULT for DCLS_TIMER\r\n");
						#endif
					}
					if ( HLS_TIMER == ucRec_Byte[0] )			/*HLS timer fault*/
					{
						fnAlarmMessage_Store(OPMM_HLS_TIMER_FAULT,OPMM_FAULT,SYSTEM_ERROR);
						#ifdef DBG_CAN      
							DEBUG_PRINTF("FAULT_OPMM_ROTATION_FAULT for HLS_TIMER\r\n");
						#endif
					}          
        }break;
				/*High- ERROR*/
				case FAULT_HB_ACK_NOT_RECEIVED:
        {
					StatusFlag.stStatusFlag.bAnyFault = TRUE;
					StatusFlag.stStatusFlag.bErrorFlag = TRUE;								
					#ifdef DBG_CAN      
						DEBUG_PRINTF("FAULT_HB_ACK_NOT_RECEIVED\r\n");
					#endif	
					if ( BIT_0 == (ucRec_Byte[0] & BIT_0) ) /*HMI HeartBeat ACK Not Received*/
					{
						fnAlarmMessage_Store(HB_ACK_NOT_RECEIVED_FROM_OP,HMI_FAULT,SYSTEM_ERROR);
						StatusFlag.stStatusFlag.bHMI_Error = TRUE;
						#ifdef DBG_CAN      
							DEBUG_PRINTF("\r\nHMI HEARTBEAT NOT RECIEVED FAULT \r\n");
						#endif			
					}
          if ( BIT_1 == (ucRec_Byte[0] & BIT_1) ) /*OPMM HeartBeat ACK Not Received*/
					{
						fnAlarmMessage_Store(HB_ACK_NOT_RECEIVED_FROM_OPMM,OPMM_FAULT,SYSTEM_ERROR);
						StatusFlag.stStatusFlag.bOPMM_Error = TRUE;
						#ifdef DBG_CAN      
							DEBUG_PRINTF("\r\nOPMM HEARTBEAT NOT RECIEVED FAULT \r\n");
						#endif			
					}
					if ( BIT_2 == (ucRec_Byte[0] & BIT_2) ) /*VCM HeartBeat ACK Not Received*/
					{
						fnAlarmMessage_Store(HB_ACK_NOT_RECEIVED_FROM_VCM,VCM_FAULT,SYSTEM_ERROR);
						StatusFlag.stStatusFlag.bVCM_Error = TRUE;
						#ifdef DBG_CAN      
							DEBUG_PRINTF("\r\nVCM HEARTBEAT NOT RECIEVED FAULT \r\n");
						#endif			
					}
					if ( BIT_3 == (ucRec_Byte[0] & BIT_3) ) /*HCM HeartBeat ACK Not Received*/
					{
						fnAlarmMessage_Store(HB_ACK_NOT_RECEIVED_FROM_HCM,HCM_FAULT,SYSTEM_ERROR);
						StatusFlag.stStatusFlag.bHCM_Error = TRUE;
						#ifdef DBG_CAN      
							DEBUG_PRINTF("\r\nHCM HEARTBEAT NOT RECIEVED FAULT \r\n");
						#endif			
					}							
        }break;
				
				case FAULT_ERR_WARN_OCCUR:
				{
					#ifdef DBG_CAN
						DEBUG_PRINTF("System Error and Warning Message received\r\n");
					#endif
					if(SYSTEM_WARNING == pstCANrxmsg->Data[0])
					{
					/*System wawning*/
					}
					else	if(SYSTEM_ERROR == pstCANrxmsg->Data[0])
					{
						/*System Error*/
						StatusFlag.stStatusFlag.bAnyFault = TRUE;
						StatusFlag.stStatusFlag.bSystemErrorFlag = TRUE;
						StatusFlag.stStatusFlag.bFlushDone = FALSE;
						StatusFlag.stStatusFlag.bFlushStatusReceived = FALSE;
						StatusFlag.stStatusFlag.bFlushStatus = FALSE;
						StatusFlag.stStatusFlag.bInitFlushProcess = FALSE;							
						StatusFlag.stStatusFlag.bAutoFlush = FALSE;
						StatusFlag.stStatusFlag.bManualFlush = FALSE;
						
						StatusFlag.stStatusFlag.bPrimeDone = FALSE;
						StatusFlag.stStatusFlag.bPrimeStatusReceived = FALSE;
						StatusFlag.stStatusFlag.bPrimeStatus = FALSE;
						StatusFlag.stStatusFlag.bInitPrimeProcess = FALSE;							
						StatusFlag.stStatusFlag.bAutoPrime = FALSE;
						StatusFlag.stStatusFlag.bManualPrime = FALSE;
					
						StatusFlag.stStatusFlag.bFlushWarnCntAchieved = FALSE;
						StatusFlag.stStatusFlag.bFlushWarningFlag = FALSE;
						uliFlushWarningCnt = 0;
					}
				}break;
			 
				/*High- ERROR*/
				case FAULT_HB_NOT_RECEIVED:
        {
					StatusFlag.stStatusFlag.bAnyFault = TRUE;
					StatusFlag.stStatusFlag.bErrorFlag = TRUE;				
					#ifdef DBG_CAN      
						DEBUG_PRINTF("FAULT_HB_NOT_RECEIVED\r\n");
					#endif	
          switch (uliCanID & SOURCE_ADDR_MASK)
					{
						case SOURCE_OPMM:
							fnAlarmMessage_Store(OPMM_NOT_RECEIVED_HEARTBEAT,OPMM_FAULT,SYSTEM_ERROR);
							StatusFlag.stStatusFlag.bOPMM_Error = TRUE;
							#ifdef DBG_CAN      
								DEBUG_PRINTF("\r\nOPMM HEARTBEAT NOT RECIEVED FAULT \r\n");
							#endif			
						break;
						case SOURCE_VCM:
							fnAlarmMessage_Store(MFM_NOT_RECEIVED_HEARTBEAT,VCM_FAULT,SYSTEM_ERROR);
							StatusFlag.stStatusFlag.bVCM_Error = TRUE;
							#ifdef DBG_CAN      
								DEBUG_PRINTF("\r\nMFM HEARTBEAT NOT RECIEVED FAULT \r\n");
							#endif			
						break;
						case SOURCE_HCM:
							fnAlarmMessage_Store(HCM_NOT_RECEIVED_HEARTBEAT,HCM_FAULT,SYSTEM_ERROR);
							StatusFlag.stStatusFlag.bHCM_Error = TRUE;
							#ifdef DBG_CAN      
								DEBUG_PRINTF("\r\nHCM HEARTBEAT NOT RECIEVED FAULT \r\n");
							#endif			
						break;
						default:break;
				}
        }break;
				case FAULT_LSW_SEQ_MISSED:
				{
					#ifdef SPRAY_PROCESS_DEBUG
						if(0x02 == pstCANrxmsg->Data[0])
						{
							fnAlarmMessage_Store(OPMM_SPRAY_EXPECTED_LW4,OPMM_FAULT,SYSTEM_ERROR);
							//fnStatusMessage_Store(OPMM_SPRAY_EXPECTED_LW4);
						}
						else if (0x01 == pstCANrxmsg->Data[0])
						{
							fnAlarmMessage_Store(OPMM_SPRAY_EXPECTED_LW3,OPMM_FAULT,SYSTEM_ERROR);
							//fnStatusMessage_Store(OPMM_SPRAY_EXPECTED_LW3);
						}
						memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
								sprintf(rgcTempCAN,"LIMIT PRESS CNT==: %d", pstCANrxmsg->Data[1] );
						fnAlarmMessage_Store(rgcTempCAN,OPMM_FAULT,SYSTEM_ERROR);
					#endif
				}break;
				
				case FAULT_DEBUG:
				{
					#ifdef SPRAY_PROCESS_DEBUG
						memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
						sprintf(rgcTempCAN,"FAULT DEBUG: %d", pstCANrxmsg->Data[0] );	
								
						
						fnAlarmMessage_Store(rgcTempCAN,OPMM_FAULT,SYSTEM_ERROR);
					#endif
				}break;
       case ACKNOWLEDGEMENT:
        {
					 #ifdef DBG_CAN      
							DEBUG_PRINTF("Msg ID: Fault_MSG_ACK received\r\n");
					 #endif	
           
           StatusFlag.stStatusFlag.bACK_Received = TRUE;
         }break;
				/*change4*/ 
			 case FAULT_LOG_MEMORY_FULL_ERROR:
        {
					StatusFlag.stStatusFlag.bAnyFault = TRUE;
											
					#ifdef DBG_CAN      
						DEBUG_PRINTF("FAULT_LOG_MEMORY_FULL_ERROR\r\n");
					#endif	
					if ( BIT_0 == (ucRec_Byte[0] & BIT_0) ) /*HMI HeartBeat ACK Not Received*/
					{
						fnMemory_Alarm_Message_Store(DIAGNOSTIC_LOG_MEMORY_FULL);
						
						#ifdef DBG_CAN      
							DEBUG_PRINTF("\r\nDIAGNOSTIC_LOG_MEMORY_FULL \r\n");
						#endif			
					}
          if ( BIT_1 == (ucRec_Byte[0] & BIT_1) ) /*OPMM HeartBeat ACK Not Received*/
					{
						fnMemory_Alarm_Message_Store(FAULT_LOG_MEMORY_FULL);
						#ifdef DBG_CAN      
							DEBUG_PRINTF("\r\nFAULT_LOG_MEMORY_FULL\r\n");
						#endif			
					}
					if ( BIT_2 == (ucRec_Byte[0] & BIT_2) ) /*VCM HeartBeat ACK Not Received*/
					{
						fnMemory_Alarm_Message_Store(STATUS_LOG_MEMORY_FULL);
						#ifdef DBG_CAN      
							DEBUG_PRINTF("\r\nSTATUS_LOG_MEMORY_FULL\r\n");
						#endif			
					}
					if ( BIT_3 == (ucRec_Byte[0] & BIT_3) ) /*HCM HeartBeat ACK Not Received*/
					{
						fnMemory_Alarm_Message_Store(PERIODIC_LOG_MEMORY_FULL);
						#ifdef DBG_CAN      
							DEBUG_PRINTF("\r\nPERIODIC_LOG_MEMORY_FULL\r\n");
						#endif			
					}		
					/*change18*/
				if ( ACK_FOR_MEMORY_FULL == (ucRec_Byte[0] & ACK_FOR_MEMORY_FULL) ) /*HCM HeartBeat ACK Not Received*/
					{
									fnMemory_Alarm_Message_Clear();
									if(RESET == ucAlarmCountLimit)
									{
										StatusFlag.stStatusFlag.bAnyFault = FALSE;
										/* ON Alarm 1 Led GREEN */
									fnLED_TurnONOFF (PORT_LED_ERR_GREEN, Bit_SET,PIN_LED_ERR_GREEN);
									fnLED_TurnONOFF (PORT_LED_ERR_RED, Bit_RESET,PIN_LED_ERR_RED);
		
									fnLED_TurnONOFF (PORT_LED_COM_GREEN, Bit_SET,PIN_LED_COM_GREEN);
									fnLED_TurnONOFF (PORT_LED_COM_RED, Bit_RESET,PIN_LED_COM_RED);
									}
									ucSwitchPressFlag = TRUE;
					}
        }break;
        			 
      default :
        break;
      }
    }break;	
    
		case MSG_TYPE_STATUS:				/*Status Message*/
    {
			if (( SCREEN_NOTIFICATION_1 == uiCurrentScreen ) || ( SCREEN_NOTIFICATION_2 == uiCurrentScreen ) ||
					( SCREEN_NOTIFICATION_3 == uiCurrentScreen ) || ( SCREEN_NOTIFICATION_4 == uiCurrentScreen ))
				{
					ucSwitchPressFlag = TRUE;
				}
			#ifdef DBG_CAN      
				DEBUG_PRINTF("Msg Type: MSG_TYPE_STATUS \r\n");
			#endif	
			
      switch (uliCanID & MSG_MASK)
      {
				/*1st- WARNING*/
				/*2nd- Warning*/
				/*3rd- Error*/
				case STATUS_STALL_TEST_RESULT:
        {
				//	StatusFlag.stStatusFlag.bWarningFlag = TRUE;
			//		StatusFlag.stStatusFlag.bWarningMEssageTimer = TRUE;
					
		//			#ifdef DBG_CAN      
						DEBUG_PRINTF("STATUS_STALL_TEST_RESULT\r\n");
		//			#endif		
          
					StatusFlag.stStatusFlag.bStallTestStatusReceived = FALSE;
					if ( LEG_A == (ucRec_Byte[1] & LEG_A) )	/*LEG_A*/
 					{
						if ( UPSTROKE_FAIL == (ucRec_Byte[0] & UPSTROKE_FAIL) ) /*Upstroke Fail*/
						{
							#ifdef DBG_CAN      
								DEBUG_PRINTF("\r\nLEGA UPSTROKE STALL TEST FAIL\r\n");
							#endif			
							fnStatusMessage_Store(UPSTROKE_STALL_FAIL_AT_A);
							ucStatusMessageCounter++;
							StatusFlag.stStatusFlag.bUpStrokeStatus = FALSE;
							StatusFlag.stStatusFlag.bUpstrokeStatusReceived = TRUE;
							
							ucSwitchPressFlag = TRUE;
							StatusFlag.stStatusFlag.bStallTestInProgress = FALSE;
						}
						else if ( UPSTROKE_PASS == (ucRec_Byte[0] & UPSTROKE_PASS) ) /*Upstroke Pass*/
						{
							#ifdef DBG_CAN      
								DEBUG_PRINTF("\r\nLEGA UPSTROKE STALL TEST PASS\r\n");
							#endif			
							fnStatusMessage_Store(UPSTROKE_STALL_PASS_AT_A);
							ucStatusMessageCounter++;
							StatusFlag.stStatusFlag.bUpStrokeStatus = TRUE;
							StatusFlag.stStatusFlag.bUpstrokeStatusReceived = TRUE;
						}
									
						if ( DOWNSTROKE_FAIL == (ucRec_Byte[0] & DOWNSTROKE_FAIL) ) /*Downstroke Pass*/
						{
							#ifdef DBG_CAN      
								DEBUG_PRINTF("\r\nLEGA DOWNSTROKE STALL TEST FAIL\r\n");
							#endif			
							fnStatusMessage_Store(DOWNSTROKE_STALL_FAIL_AT_A);
							ucStatusMessageCounter++;
							StatusFlag.stStatusFlag.bDownStrokeStatus = FALSE;
							StatusFlag.stStatusFlag.bDownstrokeStatusReceived = TRUE;
							//StatusFlag.stStatusFlag.bUpStrokeStatus = FALSE; /*change24*/
							ucSwitchPressFlag = TRUE;
							StatusFlag.stStatusFlag.bStallTestInProgress = FALSE;
						}
						else if ( DOWNSTROKE_PASS == (ucRec_Byte[0] & DOWNSTROKE_PASS) ) /*Downstroke Fail*/
						{
							#ifdef DBG_CAN      
								DEBUG_PRINTF("\r\nLEGA DOWNSTROKE STALL TEST PASS\r\n");
							#endif			
							fnStatusMessage_Store(DOWNSTROKE_STALL_PASS_AT_A);
							ucStatusMessageCounter++;
							StatusFlag.stStatusFlag.bDownStrokeStatus = TRUE;
							StatusFlag.stStatusFlag.bDownstrokeStatusReceived = TRUE;
						}
					}
					
					if ( LEG_B == (ucRec_Byte[1] & LEG_B) )	/*LEG_B*/
 					{
						if ( UPSTROKE_FAIL == (ucRec_Byte[0] & UPSTROKE_FAIL) ) /*Upstroke Fail*/
						{
							#ifdef DBG_CAN      
								DEBUG_PRINTF("\r\nLEGB UPSTROKE STALL TEST FAIL\r\n");
							#endif			
							fnStatusMessage_Store(UPSTROKE_STALL_FAIL_AT_B);
							ucStatusMessageCounter++;
							StatusFlag.stStatusFlag.bUpStrokeStatus = FALSE;
						}
						else if ( UPSTROKE_PASS == (ucRec_Byte[0] & UPSTROKE_PASS) ) /*Upstroke Pass*/
						{
							#ifdef DBG_CAN      
								DEBUG_PRINTF("\r\nLEGB UPSTROKE STALL TEST PASS\r\n");
							#endif			
							fnStatusMessage_Store(UPSTROKE_STALL_PASS_AT_B);
							ucStatusMessageCounter++;
							StatusFlag.stStatusFlag.bUpStrokeStatus = TRUE;
						}
						
						if ( DOWNSTROKE_FAIL == (ucRec_Byte[0] & DOWNSTROKE_FAIL) ) /*Downstroke Pass*/
						{
							#ifdef DBG_CAN      
								DEBUG_PRINTF("\r\nLERGB DOWNSTROKE STALL TEST FAIL\r\n");
							#endif			
							fnStatusMessage_Store(DOWNSTROKE_STALL_FAIL_AT_B);
							ucStatusMessageCounter++;
							StatusFlag.stStatusFlag.bDownStrokeStatus = FALSE;
						}
						else if ( DOWNSTROKE_PASS == (ucRec_Byte[0] & DOWNSTROKE_PASS) ) /*Downstroke Fail*/
						{
							#ifdef DBG_CAN      
								DEBUG_PRINTF("\r\nLEGB DOWNSTROKE STALL TEST PASS\r\n");
							#endif			
							fnStatusMessage_Store(DOWNSTROKE_STALL_PASS_AT_B);
							ucStatusMessageCounter++;
							StatusFlag.stStatusFlag.bDownStrokeStatus = TRUE;
						}
					}
					if ( LEG_C == (ucRec_Byte[1] & LEG_C) )	/*LEG_C*/
 					{
						if ( UPSTROKE_FAIL == (ucRec_Byte[0] & UPSTROKE_FAIL) ) /*Upstroke Fail*/
						{
							#ifdef DBG_CAN      
								DEBUG_PRINTF("\r\nLEGC UPSTROKE STALL TEST FAIL\r\n");
							#endif			
							fnStatusMessage_Store(UPSTROKE_STALL_FAIL_AT_C);
							ucStatusMessageCounter++;
							StatusFlag.stStatusFlag.bUpStrokeStatus = FALSE;
						}
						else if ( UPSTROKE_PASS == (ucRec_Byte[0] & UPSTROKE_PASS) ) /*Upstroke Pass*/
						{
							#ifdef DBG_CAN      
								DEBUG_PRINTF("\r\nLEGC UPSTROKE STALL TEST PASS\r\n");
							#endif			
							fnStatusMessage_Store(UPSTROKE_STALL_PASS_AT_C);
							ucStatusMessageCounter++;
							StatusFlag.stStatusFlag.bUpStrokeStatus = TRUE;
						}
						
						if ( DOWNSTROKE_FAIL == (ucRec_Byte[0] & DOWNSTROKE_FAIL) ) /*Downstroke Pass*/
						{
							#ifdef DBG_CAN      
								DEBUG_PRINTF("\r\nLEGC DOWNSTROKE STALL TEST FAIL\r\n");
							#endif			
							fnStatusMessage_Store(DOWNSTROKE_STALL_FAIL_AT_C);
							ucStatusMessageCounter++;
							StatusFlag.stStatusFlag.bDownStrokeStatus = FALSE;
						}
						else if ( DOWNSTROKE_PASS == (ucRec_Byte[0] & DOWNSTROKE_PASS) ) /*Downstroke Fail*/
						{
							#ifdef DBG_CAN      
								DEBUG_PRINTF("\r\nLEGC DOWNSTROKE STALL TEST PASS\r\n");
							#endif			
							fnStatusMessage_Store(DOWNSTROKE_STALL_PASS_AT_C);
							ucStatusMessageCounter++;
							StatusFlag.stStatusFlag.bDownStrokeStatus = TRUE;
						}
					}
					if (( TRUE == StatusFlag.stStatusFlag.bDownstrokeStatusReceived ) && ( TRUE == StatusFlag.stStatusFlag.bUpstrokeStatusReceived ))
					{
						StatusFlag.stStatusFlag.bStallTestInProgress = FALSE;
						ucSwitchPressFlag = TRUE;
					}
        }break;


				case STATUS_SPRAY_PROCESS:
        {
					#ifdef DBG_CAN      
						DEBUG_PRINTF("STATUS_SPRAY_PROCESS\r\n");
					#endif						
					ucLastProcessDone = SPRAY_PROCESS;
					if ( PROCESS_PASS == (ucRec_Byte[0] & BIT_1) ) /*Spray pass*/
					{
						#ifdef DBG_CAN      
							DEBUG_PRINTF("\r\nSPRAY PROCESS PASS\r\n");
						#endif				
						fnStatusMessage_Store(SPRAY_PROCESS_PASS);
						ucStatusMessageCounter++;
						
				//		stConfig_Value.fConfigJointNum++;
					//	fnOPConfig_Send(CONFIG_JOINT_NUMBER,0);/*change22*/
					}
					else if ( PROCESS_FAIL == (ucRec_Byte[0] & BIT_0) ) /*Spray fail*/
					{
						#ifdef DBG_CAN      
							DEBUG_PRINTF("\r\nSPRAY PROCESS FAIL\r\n");
						DEBUG_PRINTF("\r\nucRec_Byte[1] :- %x\r\n",ucRec_Byte[1]);
						#endif				
						fnStatusMessage_Store(SPRAY_PROCESS_FAIL);
						ucStatusMessageCounter++;
						StatusFlag.stStatusFlag.bAnyFault = TRUE;
						StatusFlag.stStatusFlag.bOPMM_Error = TRUE;
						StatusFlag.stStatusFlag.bErrorFlag = TRUE;
						fnAlarmMessage_Store(SPRAY_PROCESS_FAIL,OPMM_FAULT,SYSTEM_ERROR);
						/*change16*/
						if ( BIT_0 == (ucRec_Byte[1] & BIT_0) )
						{
							#ifdef DBG_CAN      
							DEBUG_PRINTF("\r\nSEQUENCE_ERROR\r\n");
						#endif				
						fnAlarmMessage_Store(SEQUENCE_ERROR,OPMM_FAULT,SYSTEM_ERROR);				
						}
						else if ( BIT_1 == (ucRec_Byte[1] & BIT_1) )
						{
							#ifdef DBG_CAN      
							DEBUG_PRINTF("\r\GUN_POS_SW_TIME_ERROR\r\n");
					#endif				
						fnAlarmMessage_Store(GUN_POS_SW_TIME_ERROR,OPMM_FAULT,SYSTEM_ERROR);					
						}
						else if ( BIT_2 == (ucRec_Byte[1] & BIT_2) )
						{
							#ifdef DBG_CAN      
							DEBUG_PRINTF("\r\TIME_HLS_DCLS_ERROR\r\n");
				#endif				
						fnAlarmMessage_Store(TIME_HLS_DCLS_ERROR,OPMM_FAULT,SYSTEM_ERROR);	
						}
						else if ( BIT_3 == (ucRec_Byte[1] & BIT_3) )
						{
						#ifdef DBG_CAN      
							DEBUG_PRINTF("\r\NO_LIMIT_SW_DETECTED\r\n");
						#endif				
						fnAlarmMessage_Store(NO_LIMIT_SW_DETECTED,OPMM_FAULT,SYSTEM_ERROR);					
						}
						else if ( BIT_4 == (ucRec_Byte[1] & BIT_4) )
						{
							#ifdef DBG_CAN      
							DEBUG_PRINTF("\r\ESTOP_PRESS\r\n");
					#endif				
						fnAlarmMessage_Store(ESTOP_PRESS,OPMM_FAULT,SYSTEM_ERROR);					
						}
						else if ( BIT_5 == (ucRec_Byte[1] & BIT_5) )
						{
							#ifdef DBG_CAN      
							DEBUG_PRINTF("\r\STOP_COMM_RX\r\n");
						#endif				
						fnAlarmMessage_Store(STOP_COMM_RX,OPMM_FAULT,SYSTEM_ERROR);	
						}
						else if ( BIT_6 == (ucRec_Byte[1] & BIT_6) )
						{
							#ifdef DBG_CAN      
							DEBUG_PRINTF("\r\FLUSH_COMM_RX\r\n");
						#endif				
						fnAlarmMessage_Store(FLUSH_COMM_RX,OPMM_FAULT,SYSTEM_ERROR);				
						}
						else if ( BIT_7 == (ucRec_Byte[1] & BIT_7) )
						{
							#ifdef DBG_CAN      
							DEBUG_PRINTF("\r\SYS_ERR_OCCUR\r\n");
						#endif				
						fnAlarmMessage_Store(SYS_ERR_OCCUR,OPMM_FAULT,SYSTEM_ERROR);				
						}
					}
        }break;
				
				case STATUS_PRIME_PROCESS:
        {
					#ifdef DBG_CAN      
						DEBUG_PRINTF("STATUS_PRIME_PROCESS\r\n");
					#endif						
					ucLastProcessDone = PRIME_PROCESS;
          if ( PROCESS_PASS == (ucRec_Byte[0] & BIT_1) ) /*prime pass*/
					{
						#ifdef DBG_CAN      
							DEBUG_PRINTF("\r\nPRIME PROCESS PASS\r\n");
						#endif	
						fnStatusMessage_Store(PRIME_PROCESS_PASS);	
						ucStatusMessageCounter++;										
					}
					else if ( PROCESS_FAIL == (ucRec_Byte[0] & BIT_0) ) /*prime fail*/
					{
						#ifdef DBG_CAN      
							DEBUG_PRINTF("\r\nPRIME PROCESS FAIL\r\n");
						#endif			
						fnStatusMessage_Store(PRIME_PROCESS_FAIL);
						ucStatusMessageCounter++;
					}
        }break;
				
				case STATUS_FLUSH_PROCESS:
        {
					#ifdef DBG_CAN      
						DEBUG_PRINTF("STATUS_FLUSH_PROCESS\r\n");
					#endif	

         if ( PROCESS_PASS == (ucRec_Byte[0] & BIT_1) ) /*flush pass*/
					{
						#ifdef DBG_CAN      
							DEBUG_PRINTF("\r\nFLUSH PROCESS PASS\r\n");
						#endif		
						fnStatusMessage_Store(FLUSH_PROCESS_PASS);
						ucStatusMessageCounter++;
					}
					else if ( PROCESS_FAIL == (ucRec_Byte[0] & BIT_0) ) /*flush fail*/
					{
						#ifdef DBG_CAN      
							DEBUG_PRINTF("\r\nFLUSH PROCESS FAIL\r\n");
						#endif		
						fnStatusMessage_Store(FLUSH_PROCESS_FAIL);
						ucStatusMessageCounter++;
					} 
        }break;
				
				case STATUS_SYSTEM_MODE:
        {
					#ifdef DBG_CAN      
						DEBUG_PRINTF("STATUS_SYSTEM_MODE\r\n");
					#endif	
					
          StatusFlag.stStatusFlag.bModeStatusReceived = TRUE;
					if ( OPERATION_MODE_READY == (ucRec_Byte[0] & BIT_1) ) /*System ready*/
					{
						#ifdef DBG_CAN      
							DEBUG_PRINTF("\r\nSYSTEM READY TO START OERATION MODE\r\n");
						#endif			
						StatusFlag.stStatusFlag.bOperationModeStart = TRUE;
						fnStatusMessage_Store(SYSTEM_READY_FOR_OPERATION);
						ucStatusMessageCounter++;
					}
					else if ( OPERATION_MODE_OVER == (ucRec_Byte[0] & BIT_0) ) /*Warm up mode(configuration mode)*/
					{
						ucSwitchPressFlag = TRUE;
						StatusFlag.stStatusFlag.bOperationModeCmdSent = FALSE;
						
						#ifdef DBG_CAN      
							DEBUG_PRINTF("\r\n OERATION MODE OVER\r\n");
						#endif			
						StatusFlag.stStatusFlag.bOperationModeStart = FALSE;
						fnStatusMessage_Store(OPERATION_MODE_OVER_STRING);
						ucStatusMessageCounter++;
					}
        }break;
				
				
				
				case STATUS_PRIME_TIME_ACHIEVED:
        {
					#ifdef DBG_CAN      
						DEBUG_PRINTF("STATUS_PRIME_TIME_ACHIEVED\r\n");
					#endif	
					if ( TIME_ACHIEVE_FAIL == (ucRec_Byte[1] & TIME_ACHIEVE_FAIL) ) /*Time achieved fail*/
					{
						if ( PRIME == (ucRec_Byte[0] & PRIME) ) /*For Prime*/
						{
							#ifdef DBG_CAN      
								DEBUG_PRINTF("\r\nPRIME PROCESS TIME ACHIEVED FAIL\r\n");
							#endif		
							fnStatusMessage_Store(PRIME_TIME_ACHIEVED_FAIL);
							ucStatusMessageCounter++;
						}
						else if ( FLUSH == (ucRec_Byte[0] & FLUSH) ) /*For Flush*/
						{
							#ifdef DBG_CAN      
								DEBUG_PRINTF("\r\nFLUSH PROCESS TIME ACHIEVED FAIL\r\n");
							#endif		
							fnStatusMessage_Store(FLUSH_TIME_ACHIEVED_FAIL);
							ucStatusMessageCounter++;
						}
					}
					if ( TIME_ACHIEVE_PASS == (ucRec_Byte[1] & TIME_ACHIEVE_PASS) )  /*Time achieved pass*/
					{
            if ( PRIME == (ucRec_Byte[0] & PRIME) ) /*For Prime*/
						{
							#ifdef DBG_CAN      
								DEBUG_PRINTF("\r\nPRIME PROCESS TIME ACHIEVED PASS\r\n");
							#endif	
							fnStatusMessage_Store(PRIME_TIME_ACHIEVED_PASS);
							ucStatusMessageCounter++;
						}
						else if ( FLUSH == (ucRec_Byte[0] & FLUSH) ) /*For Flush*/
						{
							#ifdef DBG_CAN      
								DEBUG_PRINTF("\r\nFLUSH PROCESS TIME ACHIEVED PASS\r\n");
							#endif	
							fnStatusMessage_Store(FLUSH_TIME_ACHIEVED_PASS);
							ucStatusMessageCounter++;
						}
					}
        }break;
			
				case STATUS_MATERIAL_PRESSURE:
        {
					#ifdef DBG_CAN      
						DEBUG_PRINTF("STATUS_MATERIAL_PRESSURE\r\n");
					#endif	
           if ( SPRAY_PRESSURE == (ucRec_Byte[0] & BIT_1) ) 
					{
						#ifdef DBG_CAN      
							DEBUG_PRINTF("\r\nPRESSURE EQUAL TO SPRAY PRESSURE\r\n");
						#endif	
						fnStatusMessage_Store(PRESSURE_EQUAL_TO_SPRAY);
						ucStatusMessageCounter++;
					}
					else if ( RECIRCULATION_PRESSURE == (ucRec_Byte[0] & BIT_0) ) 
					{
						#ifdef DBG_CAN      
							DEBUG_PRINTF("\r\nPRESSURE EQUAL TO RECIRCULATION PRESSURE\r\n");
						#endif	
						fnStatusMessage_Store(PRESSURE_EQUAL_TO_RECIRC);
						ucStatusMessageCounter++;
					}
        }break;				
				
				case STATUS_STALL_VALVE:
				{
					/*change12*/
						stCurrent_Value.ucVCMValveCurState = ucRec_Byte[0];
					stCurrent_Value.ucVCMValveCurStateChange = 
							stCurrent_Value.ucVCMValveCurState ^ stCurrent_Value.ucVCMValveBackUpState;
				/*change12*/
					if ( BIT_0 == (stCurrent_Value.ucVCMValveCurStateChange & BIT_0) )
					{
						/*****************************************************/
						if ( BIT_0 == (ucRec_Byte[0] & BIT_0) ) 			
						{													
						#ifdef DBG_CAN      
							DEBUG_PRINTF("\r\nSTATUS_STALL_VALVE\r\n");
						#endif
						StatusFlag.stStatusFlag.VCM_VL_Stall = TRUE;
						fnStatusMessage_Store(STALL_VALVE_ON);
						ucStatusMessageCounter++;
						}
						else 			/*FORWARD VALVE  OFF*/
						{													
								#ifdef DBG_CAN      
							DEBUG_PRINTF("\r\nSTATUS_STALL_VALVE\r\n");
						#endif
						StatusFlag.stStatusFlag.VCM_VL_Stall = FALSE;
						fnStatusMessage_Store(STALL_VALVE_OFF);
						ucStatusMessageCounter++;
						}
						
						/*****************************************************/
//						#ifdef DBG_CAN      
//							DEBUG_PRINTF("\r\nSTATUS_STALL_VALVE\r\n");
//						#endif
//						StatusFlag.stStatusFlag.VCM_VL_Stall = FALSE;
//						fnStatusMessage_Store(STALL_VALVE_OFF);
//						ucStatusMessageCounter++;
					}
						if ( BIT_1 == (stCurrent_Value.ucVCMValveCurStateChange & BIT_1) )
					{
						/*****************************************************/
						if ( BIT_1 == (ucRec_Byte[0] & BIT_1) ) 			
						{													
							#ifdef DBG_CAN      
							DEBUG_PRINTF("\r\nSTATUS_RECIRC_VALVE\r\n");
						#endif
						StatusFlag.stStatusFlag.VCM_VL_Recirc = TRUE;
						fnStatusMessage_Store(RECIRC_VALVE_ON);
						ucStatusMessageCounter++;
						}
						else 			/*FORWARD VALVE  OFF*/
						{													
							#ifdef DBG_CAN      
							DEBUG_PRINTF("\r\nSTATUS_RECIRC_VALVE\r\n");
						#endif
						StatusFlag.stStatusFlag.VCM_VL_Recirc = FALSE;
						fnStatusMessage_Store(RECIRC_VALVE_OFF);
						ucStatusMessageCounter++;
						}
						
						/*****************************************************/
//						#ifdef DBG_CAN      
//							DEBUG_PRINTF("\r\nSTATUS_STALL_VALVE\r\n");
//						#endif
//						StatusFlag.stStatusFlag.VCM_VL_Stall = FALSE;
//						fnStatusMessage_Store(STALL_VALVE_OFF);
//						ucStatusMessageCounter++;
					}

//					else if ( BIT_0 == (stCurrent_Value.ucVCMValveCurStateChange & BIT_0) )
//					{
//						#ifdef DBG_CAN      
//							DEBUG_PRINTF("\r\nSTATUS_STALL_VALVE\r\n");
//						#endif
//						StatusFlag.stStatusFlag.VCM_VL_Stall = TRUE;
//						fnStatusMessage_Store(STALL_VALVE_ON);
//						ucStatusMessageCounter++;
//					}
					/*change12*/
//					if ( 0x00 == (stCurrent_Value.ucVCMValveCurStateChange & BIT_1) )
//					{
//						#ifdef DBG_CAN      
//							DEBUG_PRINTF("\r\nSTATUS_RECIRC_VALVE\r\n");
//						#endif
//						StatusFlag.stStatusFlag.VCM_VL_Recirc = FALSE;
//						fnStatusMessage_Store(RECIRC_VALVE_OFF);
//						ucStatusMessageCounter++;
//					}
//					else if ( BIT_1 == (stCurrent_Value.ucVCMValveCurStateChange & BIT_1) )
//					{
//						#ifdef DBG_CAN      
//							DEBUG_PRINTF("\r\nSTATUS_RECIRC_VALVE\r\n");
//						#endif
//						StatusFlag.stStatusFlag.VCM_VL_Recirc = TRUE;
//						fnStatusMessage_Store(RECIRC_VALVE_ON);
//						ucStatusMessageCounter++;
//					}
					stCurrent_Value.ucVCMValveBackUpState = stCurrent_Value.ucVCMValveCurState;
				}break;
				case STATUS_OPMM_VALVE:
				{
					ucSwitchPressFlag = TRUE;

					stCurrent_Value.ucOPMMValveCurState = ucRec_Byte[0];
					stCurrent_Value.ucOPMMValveCurStateChange = 
							stCurrent_Value.ucOPMMValveCurState ^ stCurrent_Value.ucOPMMValveBackUpState;
					
					if( BIT_0 == (stCurrent_Value.ucOPMMValveCurStateChange & BIT_0) )
					{
						if ( OPMM_VL_SPRAY_GUN == (ucRec_Byte[0] & OPMM_VL_SPRAY_GUN) ) 	/*SPRAY VALVE ON*/
						{
							StatusFlag.stStatusFlag.OPMM_VL_SprayGun_status = TRUE;
							fnStatusMessage_Store(OPMM_SPRAY_VALVE_ON);
							ucStatusMessageCounter++;
						}
						else 		/*SPRAY VALVE OFF*/
						{
							StatusFlag.stStatusFlag.OPMM_VL_SprayGun_status = FALSE;
							fnStatusMessage_Store(OPMM_SPRAY_VALVE_OFF);
							ucStatusMessageCounter++;
						}
					}
					if( BIT_1 == (stCurrent_Value.ucOPMMValveCurStateChange & BIT_1) )
					{
						if ( OPMM_VL_SPRAY_HEAD == (ucRec_Byte[0] & OPMM_VL_SPRAY_HEAD) ) /*GUN POS VALVE ON*/
						{
							StatusFlag.stStatusFlag.OPMM_VL_SprayHead_status = TRUE;
							fnStatusMessage_Store(OPMM_SPRAY_HEAD_VALVE_ON);
							ucStatusMessageCounter++;
						}
						else  		/*GUN POS VALVE OFF*/
						{
							StatusFlag.stStatusFlag.OPMM_VL_SprayHead_status = FALSE;
							fnStatusMessage_Store(OPMM_SPRAY_HEAD_VALVE_OFF);
							ucStatusMessageCounter++;
						}
					}
					
					if( BIT_2 == (stCurrent_Value.ucOPMMValveCurStateChange & BIT_2) )
					{
						if ( OPMM_VL_AB == (ucRec_Byte[0] & OPMM_VL_AB) ) 								/*AB VALVE ON*/
						{
							StatusFlag.stStatusFlag.OPMM_VL_AB_status = TRUE;
							fnStatusMessage_Store(OPMM_AB_MAT_VALVE_ON);
							ucStatusMessageCounter++;
						}
						else  				/*AB VALVE OFF*/
						{
							StatusFlag.stStatusFlag.OPMM_VL_AB_status = FALSE;
							fnStatusMessage_Store(OPMM_AB_MAT_VALVE_OFF);
							ucStatusMessageCounter++;
						}
					}
					
					if( BIT_3 == (stCurrent_Value.ucOPMMValveCurStateChange & BIT_3) )
					{
						if ( OPMM_VL_FLUSH == (ucRec_Byte[0] & OPMM_VL_FLUSH) ) 					/*SOLVENT/FLUSH VALVE ON*/
						{
							StatusFlag.stStatusFlag.OPMM_VL_Flush_status = TRUE;
							fnStatusMessage_Store(OPMM_FLUSH_VALVE_ON);
							ucStatusMessageCounter++;
						}
						else  			/*SOLVENT/FLUSH VALVE OFF*/
						{	
							StatusFlag.stStatusFlag.OPMM_VL_Flush_status = FALSE;
							fnStatusMessage_Store(OPMM_FLUSH_VALVE_OFF);
							ucStatusMessageCounter++;
						}
					}
					
					if( BIT_4 == (stCurrent_Value.ucOPMMValveCurStateChange & BIT_4) )
					{
						if ( OPMM_VL_FORWARD == (ucRec_Byte[0] & OPMM_VL_FORWARD) ) 			/*FORWARD VALVE ON*/
						{													
							StatusFlag.stStatusFlag.OPMM_VL_Forward_status = TRUE;
							fnStatusMessage_Store(OPMM_FORWARD_VALVE_ON);
							ucStatusMessageCounter++;
						}
						else 			/*FORWARD VALVE  OFF*/
						{													
							StatusFlag.stStatusFlag.OPMM_VL_Forward_status = FALSE;
							fnStatusMessage_Store(OPMM_FORWARD_VALVE_OFF);
							ucStatusMessageCounter++;
						}
					}
					
					if( BIT_5 == (stCurrent_Value.ucOPMMValveCurStateChange & BIT_5) )
					{
						if ( OPMM_VL_BACKWARD == (ucRec_Byte[0] & OPMM_VL_BACKWARD) ) 		/*BACKWARD VALVE ON*/
						{
							StatusFlag.stStatusFlag.OPMM_VL_Backward_status = TRUE;
							fnStatusMessage_Store(OPMM_BACKWARD_VALVE_ON);
							ucStatusMessageCounter++;
						}
						else  		/*BACKWARD VALVE OFF*/
						{
							StatusFlag.stStatusFlag.OPMM_VL_Backward_status = FALSE;
							fnStatusMessage_Store(OPMM_BACKWARD_VALVE_OFF);
							ucStatusMessageCounter++;
						}
					}
					stCurrent_Value.ucOPMMValveBackUpState = stCurrent_Value.ucOPMMValveCurState;
				}break;
				case STATUS_HCM_HEATER:
				{
					stCurrent_Value.ucHCMHeaterCurState = ucRec_Byte[0];
					stCurrent_Value.ucHCMHeaterCurStateChange = 
							stCurrent_Value.ucHCMHeaterCurState ^ stCurrent_Value.ucHCMHeaterBackUpState;
					
					if( BIT_0 == (stCurrent_Value.ucHCMHeaterCurStateChange & BIT_0) )
					{
						if ( HCM_INLINE_BASE_HEATER == (ucRec_Byte[0] & HCM_INLINE_BASE_HEATER) ) 	/*HCM_INLINE_BASE_HEATER*/
						{
							StatusFlag.stStatusFlag.bHCM_Base_Heater_Status = TRUE;
							fnStatusMessage_Store(HCM_BASE_HEATER_ON);
							ucStatusMessageCounter++;
						}
						else  	/*OFF*/
						{
							StatusFlag.stStatusFlag.bHCM_Base_Heater_Status = FALSE;
							fnStatusMessage_Store(HCM_BASE_HEATER_OFF);
							ucStatusMessageCounter++;
						}
					}
					if( BIT_1 == (stCurrent_Value.ucHCMHeaterCurStateChange & BIT_1) )
					{
						if ( HCM_INLINE_HARD_HEATER == (ucRec_Byte[0] & HCM_INLINE_HARD_HEATER) ) 	/*HCM_INLINE_HARD_HEATER*/
						{
							StatusFlag.stStatusFlag.bHCM_Hard_Heater_Status = TRUE;
							fnStatusMessage_Store(HCM_HARD_HEATER_ON);
							ucStatusMessageCounter++;
						}
						else	/*OFF*/
						{
							StatusFlag.stStatusFlag.bHCM_Hard_Heater_Status = FALSE;
							fnStatusMessage_Store(HCM_HARD_HEATER_OFF);
							ucStatusMessageCounter++;
						}
					}
					if( BIT_2 == (stCurrent_Value.ucHCMHeaterCurStateChange & BIT_2) )
					{
						if ( HCM_ANTIFREEZ_HEATER == (ucRec_Byte[0] & HCM_ANTIFREEZ_HEATER) ) 			/*HCM_ANTIFREEZ_HEATER*/
						{
							StatusFlag.stStatusFlag.bHCM_Antfrz_Heater_Status = TRUE;
							fnStatusMessage_Store(HCM_ANTFRZ_HEATER_ON);
							ucStatusMessageCounter++;
						}
						else  			/*OFF*/
						{
							StatusFlag.stStatusFlag.bHCM_Antfrz_Heater_Status = FALSE;
							fnStatusMessage_Store(HCM_ANTFRZ_HEATER_OFF);
							ucStatusMessageCounter++;
						}
					}
					if( BIT_3 == (stCurrent_Value.ucHCMHeaterCurStateChange & BIT_3) )
					{
						if ( HCM_HARD_TANK_HEATER == (ucRec_Byte[0] & HCM_HARD_TANK_HEATER) ) 			/*HCM_HARD_TANK_HEATER*/
						{
							StatusFlag.stStatusFlag.bHCM_Hard_Tank_Heater_Status = TRUE;
							fnStatusMessage_Store(HCM_HARD_TANK_HEATER_ON);
							ucStatusMessageCounter++;
						}
						else 		/*OFF*/
						{
							StatusFlag.stStatusFlag.bHCM_Hard_Tank_Heater_Status = FALSE;
							fnStatusMessage_Store(HCM_HARD_TANK_HEATER_OFF);
							ucStatusMessageCounter++;
						}
					}
					
					stCurrent_Value.ucHCMHeaterBackUpState = stCurrent_Value.ucHCMHeaterCurState;
				}break;
				case STATUS_GUN_POSITION:
				{
					if ( PRIME_POSITION == (ucRec_Byte[0] & PRIME_POSITION) )
					{
						fnStatusMessage_Store(GUN_AWAY_FROM_PIPE);
						ucStatusMessageCounter++;
					}else if ( SPRAY_POSITION == (ucRec_Byte[0] & SPRAY_POSITION) ) 
					{
						fnStatusMessage_Store(GUN_TOWARDS_PIPE);
						ucStatusMessageCounter++;
					}
				}break;
				/*WARNING*/
				case STATUS_VCM_DIAGNOSTIC:
        {
					#ifdef DBG_CAN      
						DEBUG_PRINTF("STATUS_VCM_DIAGNOSTIC\r\n");
					#endif	
					stDiagStatus.bVCM_DiagStatusReceived = TRUE;
					ucSwitchPressFlag = TRUE;
					if ( DIAGNOSTIC_PASS == (ucRec_Byte[0] & DIAGNOSTIC_PASS) ) 				/*PASS*/
					{
						#ifdef DBG_CAN      
							DEBUG_PRINTF("\r\nVCM DIAGNOSTIC PASS\r\n");
						#endif			
						stDiagStatus.bVCM_DiagStatus = TRUE;
						fnStatusMessage_Store(VCM_DIAGNOSTIC_PASS);
						ucStatusMessageCounter++;
					}
					else if ( DIAGNOSTIC_FAIL == (ucRec_Byte[0] & DIAGNOSTIC_FAIL) ) 	 	/*FAIL*/
					{
						StatusFlag.stStatusFlag.bWarningFlag = TRUE;						
						StatusFlag.stStatusFlag.bVCM_Warning = TRUE;
						fnLED_TurnONOFF (PORT_LED_ERR_BLUE, Bit_SET,PIN_LED_ERR_BLUE); /*change26*/
						#ifdef DBG_CAN      
							DEBUG_PRINTF("\r\nVCM DIAGNOSTIC FAIL\r\n");
						#endif			
						stDiagStatus.bVCM_DiagStatus = FALSE;
						fnStatusMessage_Store(VCM_DIAGNOSTIC_FAIL);
						
						fnAlarmMessage_Store(VCM_DIAGNOSTIC_FAIL,VCM_FAULT,SYSTEM_WARNING);
						ucStatusMessageCounter++;
					}
					/*change12*/
					uiVCM_DiagnosticStatus = ucRec_Byte[2];
					uiVCM_DiagnosticStatus = ((uiVCM_DiagnosticStatus<<8)|(ucRec_Byte[1]));
        }break;
				/*WARNING*/
				case STATUS_SCU_DIAGNOSTIC:
        {
					#ifdef DBG_CAN      
						DEBUG_PRINTF("STATUS_MCU_DIAGNOSTIC\r\n");
					#endif	
					stDiagStatus.bSCU_DiagStatusReceived = TRUE;
					ucSwitchPressFlag = TRUE;          
					if ( DIAGNOSTIC_PASS == (ucRec_Byte[0] & DIAGNOSTIC_PASS) ) /*PASS*/
					{
						#ifdef DBG_CAN      
							DEBUG_PRINTF("\r\nMCU DIAGNOSTIC PASS\r\n");
						#endif			
						stDiagStatus.bSCU_DiagStatus = TRUE;
			//			fnStatusMessage_Store(SCU_DIAGNOSTIC_PASS);
						
			//			ucStatusMessageCounter++;
					}
					else  /*FAIL*/
					{
						StatusFlag.stStatusFlag.bWarningFlag = TRUE;						
						StatusFlag.stStatusFlag.bSCU_Warning = TRUE;
						fnLED_TurnONOFF (PORT_LED_ERR_BLUE, Bit_SET,PIN_LED_ERR_BLUE); /*change26*/
						#ifdef DBG_CAN      
							DEBUG_PRINTF("\r\nMCU DIAGNOSTIC FAIL\r\n");
						#endif			
						stDiagStatus.bSCU_DiagStatus = FALSE;
						fnStatusMessage_Store(SCU_DIAGNOSTIC_FAIL);
						
						fnAlarmMessage_Store(SCU_DIAGNOSTIC_FAIL,SCU_FAULT,SYSTEM_WARNING);
						ucStatusMessageCounter++;
					}
					uiSCU_DiagnosticStatus = ucRec_Byte[1];		
        }break;				
				
				/*WARNING*/
				case STATUS_OPMM_DIAGNOSTIC:
        {
					#ifdef DBG_CAN      
						DEBUG_PRINTF("STATUS_OPMM_DIAGNOSTIC\r\n");
					#endif	
					stDiagStatus.bOPMM_DiagStatusReceived = TRUE;
					ucSwitchPressFlag = TRUE; 
					if ( DIAGNOSTIC_PASS == (ucRec_Byte[0] & DIAGNOSTIC_PASS) ) /*PASS*/
					{
						#ifdef DBG_CAN      
							DEBUG_PRINTF("\r\nOPMM DIAGNOSTIC PASS\r\n");
						#endif			
						stDiagStatus.bOPMM_DiagStatus = TRUE;
						fnStatusMessage_Store(OPMM_DIAGNOSTIC_PASS);
						ucStatusMessageCounter++;
					}						
					else  /*FAIL*/
					{
						StatusFlag.stStatusFlag.bWarningFlag = TRUE;
						StatusFlag.stStatusFlag.bOPMM_Warning = TRUE;
fnLED_TurnONOFF (PORT_LED_ERR_BLUE, Bit_SET,PIN_LED_ERR_BLUE); /*change26*/
						#ifdef DBG_CAN      
							DEBUG_PRINTF("\r\nOPMM DIAGNOSTIC FAIL\r\n");
						#endif			
						stDiagStatus.bOPMM_DiagStatus = FALSE;
						fnStatusMessage_Store(OPMM_DIAGNOSTIC_FAIL);
						
						fnAlarmMessage_Store(OPMM_DIAGNOSTIC_FAIL,OPMM_FAULT,SYSTEM_WARNING);
						ucStatusMessageCounter++;
					}
					uliOPMM_DIagnosticStatus = ucRec_Byte[1] | (ucRec_Byte[2] << 8) | (ucRec_Byte[3] << 16);		
        }break;
				
					
				/*WARNING*/
				case STATUS_HCM_DIAGNOSTIC:
        {
					#ifdef DBG_CAN      
						DEBUG_PRINTF("STATUS_HCM_DIAGNOSTIC\r\n");
					#endif	
					stDiagStatus.bHCM_DiagStatusReceived = TRUE;
					ucSwitchPressFlag = TRUE; 
					if ( DIAGNOSTIC_PASS == (ucRec_Byte[0] & DIAGNOSTIC_PASS) ) /*PASS*/
					{
						#ifdef DBG_CAN      
							DEBUG_PRINTF("\r\nHCM DIAGNOSTIC PASS\r\n");
						#endif			
						stDiagStatus.bHCM_DiagStatus = TRUE;
						fnStatusMessage_Store(HCM_DIAGNOSTIC_PASS);
						ucStatusMessageCounter++;
					}
					else  /*FAIL*/
					{
						StatusFlag.stStatusFlag.bWarningFlag = TRUE;
						fnLED_TurnONOFF (PORT_LED_ERR_BLUE, Bit_SET,PIN_LED_ERR_BLUE); /*change26*/
						#ifdef DBG_CAN      
							DEBUG_PRINTF("\r\nHCM DIAGNOSTIC FAIL\r\n");
						#endif			
						stDiagStatus.bHCM_DiagStatus = FALSE;
						fnStatusMessage_Store(HCM_DIAGNOSTIC_FAIL);
						
						fnAlarmMessage_Store(HCM_DIAGNOSTIC_FAIL,HCM_FAULT,SYSTEM_WARNING);
						ucStatusMessageCounter++;
						StatusFlag.stStatusFlag.bHCM_Warning = TRUE;
					}
					uiHCM_DiagnosticStatus = ucRec_Byte[1] | (ucRec_Byte[2] << 8);;
        }break;	
								
				case STATUS_AIR_SENSOR_CONN:
				{
					#ifdef DBG_CAN      
						DEBUG_PRINTF("STATUS_AIR_SENSOR_CONN\r\n");
					#endif
					ucSwitchPressFlag = TRUE;
					if ( AIR_SNR_CONNECTED == ucRec_Byte[0] )
					{
						StatusFlag.stStatusFlag.bVCM_AirSensor_Status = TRUE;
					}
					else if( AIR_SNR_DISCONNECTED == ucRec_Byte[0] )
					{
						StatusFlag.stStatusFlag.bVCM_AirSensor_Status = FALSE;
					}
				}break;
						
				case STATUS_LSW_PRESSED:
				{
					if ( 0x01 == (ucRec_Byte[0] & 0x01))
					{
						fnStatusMessage_Store(LIMIT_SWITCH_1_PRESSED);
						ucStatusMessageCounter++;
					}
					if ( 0x02 == (ucRec_Byte[0] & 0x02))
					{
						fnStatusMessage_Store(LIMIT_SWITCH_2_PRESSED);
						ucStatusMessageCounter++;
					}
					if ( 0x04 == (ucRec_Byte[0] & 0x04))
					{
						fnStatusMessage_Store(LIMIT_SWITCH_3_PRESSED);
						ucStatusMessageCounter++;
					}
					if ( 0x08 == (ucRec_Byte[0] & 0x08))
					{
						fnStatusMessage_Store(LIMIT_SWITCH_4_PRESSED);
						ucStatusMessageCounter++;
					}
				}break;					
				case ACKNOWLEDGEMENT:
        {
					#ifdef DBG_CAN      
						DEBUG_PRINTF("Msg ID: Status_MSG_ACK received\r\n");
					#endif	
					StatusFlag.stStatusFlag.bACK_Received = SET;
        }break;
				
				case STATUS_SOFTWARE_VERSION:
				{
					for(ucCnt = 1; ucCnt < pstCANrxmsg->DLC; ucCnt++)
					{
						SplitByte.ucChar[ucCnt - 1] = pstCANrxmsg->Data[ucCnt];
					}
					
					if ( MODULE_SCU == ucRec_Byte[0] )
					{
							stCurrent_Value.fSCUCodeVersion = SplitByte.fFloat;
					}
					else if ( MODULE_OPMM == ucRec_Byte[0] )
					{
							stCurrent_Value.fOPMMCodeVersion = SplitByte.fFloat;
					}
					else if ( MODULE_HCM == ucRec_Byte[0] )
					{
							stCurrent_Value.fHCMCodeVersion = SplitByte.fFloat;
					}
					else if ( MODULE_VCM == ucRec_Byte[0] )
					{
							stCurrent_Value.fVCMCodeVersion = SplitByte.fFloat;
					}
				}break;
				/*change19*/
				case STATUS_REMOTE_SW:
        {
					#ifdef DBG_CAN      
						DEBUG_PRINTF("STATUS_REMOTE_SW\r\n");
					#endif	

         if ( BIT_0 == (ucRec_Byte[0] & BIT_0) ) /*flush pass*/
					{
						#ifdef DBG_CAN      
							DEBUG_PRINTF("\r\nPRIME_DETECTED\r\n");
						#endif		
						fnStatusMessage_Store(PRIME_DETECTED);
						ucStatusMessageCounter++;
					}
					else if ( BIT_1 == (ucRec_Byte[0] & BIT_1) ) /*flush fail*/
					{
						#ifdef DBG_CAN      
							DEBUG_PRINTF("\r\nFLUSH_DETECTED\r\n");
						#endif		
						fnStatusMessage_Store(FLUSH_DETECTED);
						ucStatusMessageCounter++;
					} 
					else if ( BIT_2 == (ucRec_Byte[0] & BIT_2) ) /*flush fail*/
					{
						#ifdef DBG_CAN      
							DEBUG_PRINTF("\r\nSPRAY_DETECTED\r\n");
						#endif		
						fnStatusMessage_Store(SPRAY_DETECTED);
						ucStatusMessageCounter++;
					} 
					else if ( BIT_3 == (ucRec_Byte[0] & BIT_3) ) /*flush fail*/
					{
						#ifdef DBG_CAN      
							DEBUG_PRINTF("\r\nREAD_PIPE_TEMP_DETECTED\r\n");
						#endif		
						fnStatusMessage_Store(READ_PIPE_TEMP_DETECTED);
						ucStatusMessageCounter++;
					} 
					else if ( BIT_4 == (ucRec_Byte[0] & BIT_4) ) /*flush fail*/
					{
						#ifdef DBG_CAN      
							DEBUG_PRINTF("\r\nSTART/STOP_DETECTED\r\n");
						#endif		
						fnStatusMessage_Store(START_STOP_DETECTED);
						ucStatusMessageCounter++;
					} 
					else if ( BIT_5 == (ucRec_Byte[0] & BIT_5) ) /*flush fail*/
					{
						#ifdef DBG_CAN      
							DEBUG_PRINTF("\r\nFLUSH_COMPLETE_DETECTED\r\n");
						#endif		
						fnStatusMessage_Store(FLUSH_COMPLETE_DETECTED);
						ucStatusMessageCounter++;
					} 
        }break;
				default :
        break;
      }
      if ( 24 < ucStatusMessageCounter )
			{
				ucStatusMessageCounter = 24;
			}
    }break;
    
		case MSG_TYPE_DATA:					/*Data Message*/
    {
			#ifdef DBG_CAN      
				DEBUG_PRINTF("Msg Type: MSG_TYPE_DATA \r\n");
			#endif			
      switch (uliCanID & MSG_MASK)
      {
				case DATA_INLINE_BASE_TEMP:		/*Inline Base material current temp reading*/
        {
					#ifdef DBG_CAN      
						DEBUG_PRINTF("DATA_INLINE_BASE_TEMP\r\n");
					#endif	
          
          for(ucCnt=0; ucCnt < pstCANrxmsg->DLC; ucCnt++)
					{
						SplitByte.ucChar[ucCnt] = pstCANrxmsg->Data[ucCnt];
					}			

					stCurrent_Value.fCurrInlineBaseMatTemp = SplitByte.fFloat;
					#ifdef DBG_CAN     
						memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
						sprintf(rgcTempCAN,"fdInline_Basemat_TEMP_Curr %0.2f",stCurrent_Value.fCurrInlineBaseMatTemp);	
						DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
					#endif
					if (( IMPERIAL == stConfig_Value.ucTypeofUnit ) && ( INVALID_DATA != stCurrent_Value.fCurrInlineBaseMatTemp ))
					{
						stCurrent_Value.fCurrInlineBaseMatTemp = fnMetricToImperial_Conv(stCurrent_Value.fCurrInlineBaseMatTemp, TEMPERATURE_VALUE);
					}
        }break;	
				
				case DATA_INLINE_HARD_TEMP:		/*Inline Hardener material current temp reading*/
        {
					#ifdef DBG_CAN      
						DEBUG_PRINTF("DATA_INLINE_HARD_TEMP\r\n");
					#endif	

          for(ucCnt=0; ucCnt < pstCANrxmsg->DLC; ucCnt++)
					{
						SplitByte.ucChar[ucCnt] = pstCANrxmsg->Data[ucCnt];
					}

					stCurrent_Value.fCurrInlineHardMatTemp = SplitByte.fFloat;
					#ifdef DBG_CAN     
						memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
						sprintf(rgcTempCAN,"fdInline_Hardmat_TEMP_Curr %0.2f",stCurrent_Value.fCurrInlineHardMatTemp);	
						DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
					#endif
					if (( IMPERIAL == stConfig_Value.ucTypeofUnit ) && ( INVALID_DATA != stCurrent_Value.fCurrInlineHardMatTemp ))
					{
						stCurrent_Value.fCurrInlineHardMatTemp = fnMetricToImperial_Conv(stCurrent_Value.fCurrInlineHardMatTemp, TEMPERATURE_VALUE);
					}
        }break;
				case DATA_RETURN_BASE_TEMP:		/*Return line temperature from HCM*/
        {
					#ifdef DBG_CAN      
						DEBUG_PRINTF("Msg ID: DATA_RETURN_BASE_TEMP\r\n");
					#endif	
          for(ucCnt=0; ucCnt < pstCANrxmsg->DLC; ucCnt++)
					{
						SplitByte.ucChar[ucCnt] = pstCANrxmsg->Data[ucCnt];
					}						
          	
					stCurrent_Value.fCurrReturnLineTemp = SplitByte.fFloat;
					#ifdef DBG_CAN     
						memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
						sprintf(rgcTempCAN,"DATA_RETURN_BASE_TEMP: %0.2f", stCurrent_Value.fCurrReturnLineTemp);	
						DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
					#endif		
					
					if (( IMPERIAL == stConfig_Value.ucTypeofUnit ) && ( INVALID_DATA != stCurrent_Value.fCurrReturnLineTemp ))
					{
						stCurrent_Value.fCurrReturnLineTemp = fnMetricToImperial_Conv(stCurrent_Value.fCurrReturnLineTemp, TEMPERATURE_VALUE);
					}
        }break;
				case DATA_ANTIFREEZ_TEMP:			/*Antifreeze liquid Current temp reading*/
        {
					#ifdef DBG_CAN      
						DEBUG_PRINTF("DATA_BHCM_LIQUID_CURR_TEMP\r\n");
					#endif	
          
          for(ucCnt=0; ucCnt < pstCANrxmsg->DLC; ucCnt++)
					{
						SplitByte.ucChar[ucCnt] = pstCANrxmsg->Data[ucCnt];
					}			

					stCurrent_Value.fCurrAntifreezeLiquidTemp = SplitByte.fFloat;
					#ifdef DBG_CAN     
						memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
						sprintf(rgcTempCAN,"fdLiquid_Antifreeze_TEMP_Curr %0.2f",stCurrent_Value.fCurrAntifreezeLiquidTemp);	
						DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
					#endif
					if (( IMPERIAL == stConfig_Value.ucTypeofUnit ) && ( INVALID_DATA != stCurrent_Value.fCurrAntifreezeLiquidTemp ))
					{
						stCurrent_Value.fCurrAntifreezeLiquidTemp = fnMetricToImperial_Conv(stCurrent_Value.fCurrAntifreezeLiquidTemp, TEMPERATURE_VALUE);
					}
        }break;
				case DATA_BASE_MATERIAL_LVL:	/*Base material liquid level reading*/
        {
					#ifdef DBG_CAN      
						DEBUG_PRINTF("DATA_BASE_MATERIAL_LVL\r\n");
					#endif	
          
					for(ucCnt=0; ucCnt < pstCANrxmsg->DLC; ucCnt++)
					{
						SplitByte.ucChar[ucCnt] = pstCANrxmsg->Data[ucCnt];
					}			

					stCurrent_Value.fCurrLiquidLevelBase = SplitByte.fFloat;
				#ifdef DBG_CAN     
						memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
						sprintf(rgcTempCAN,"fdLiquid_Base_Level_Curr %0.2f",stCurrent_Value.fCurrLiquidLevelBase);	
						DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
					#endif		
					if (( IMPERIAL == stConfig_Value.ucTypeofUnit ) && ( INVALID_DATA != stCurrent_Value.fCurrLiquidLevelBase ))
					{
						stCurrent_Value.fCurrLiquidLevelBase = fnMetricToImperial_Conv(stCurrent_Value.fCurrLiquidLevelBase, MM_TO_INCH);
					}					
        }break;	
				
				case DATA_HARD_MATERIAL_LVL:	/*Hardener material liquid level reading*/
				{
					#ifdef DBG_CAN      
						DEBUG_PRINTF("DATA_HARD_MATERIAL_LVL\r\n");
					#endif	

					for(ucCnt=0; ucCnt < pstCANrxmsg->DLC; ucCnt++)
					{
						SplitByte.ucChar[ucCnt] = pstCANrxmsg->Data[ucCnt];
							
					}			

					stCurrent_Value.fLiquid_Hardner_Level_Curr = SplitByte.fFloat;
										
					#ifdef DBG_CAN     
						memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
						sprintf(rgcTempCAN,"fdLiquid_Hard_Level_Curr %0.2f",stCurrent_Value.fLiquid_Hardner_Level_Curr);	
						DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
					#endif
					if (( IMPERIAL == stConfig_Value.ucTypeofUnit ) && ( INVALID_DATA != stCurrent_Value.fCurrLiquidLevelBase ))
					{
						stCurrent_Value.fLiquid_Hardner_Level_Curr = fnMetricToImperial_Conv(stCurrent_Value.fLiquid_Hardner_Level_Curr, MM_TO_INCH);
					}	
				}break;
				case DATA_DEW_POINT:					/*Dew point reading*/
        {
					#ifdef DBG_CAN      
						DEBUG_PRINTF("DATA_DEW_POINT\r\n");
					#endif	
          
          for(ucCnt=0; ucCnt < pstCANrxmsg->DLC; ucCnt++)
						{
							SplitByte.ucChar[ucCnt] = pstCANrxmsg->Data[ucCnt];
						}

						stCurrent_Value.fCurrDewPoint = SplitByte.fFloat;
						#ifdef DBG_CAN     
							memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
							sprintf(rgcTempCAN,"fdDew_Point_Curr : %0.2f", stCurrent_Value.fCurrDewPoint );	
							DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
						#endif		
						
						if (( IMPERIAL == stConfig_Value.ucTypeofUnit ) && ( INVALID_DATA != stCurrent_Value.fCurrInsideAmbientTemp ))
					{
						stCurrent_Value.fCurrInsideAmbientTemp = fnMetricToImperial_Conv(stCurrent_Value.fCurrInsideAmbientTemp, TEMPERATURE_VALUE);
					}
				}break;
				case DATA_AMB_TEMP_INSIDE:		/*Ambient Temperature inside the container*/
        {
					#ifdef DBG_CAN      
						DEBUG_PRINTF("DATA_MCU_AMB_TEMP_INSIDE\r\n");
					#endif	
         
					for(ucCnt=0; ucCnt < pstCANrxmsg->DLC; ucCnt++)
					{
						SplitByte.ucChar[ucCnt] = pstCANrxmsg->Data[ucCnt];
					}						
					
					stCurrent_Value.fCurrInsideAmbientTemp = SplitByte.fFloat;
					#ifdef DBG_CAN     
						memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
						sprintf(rgcTempCAN,"DATA_MCU_AMB_TEMP_INSIDE_CURRENT: %0.2f", stCurrent_Value.fCurrInsideAmbientTemp);	
						DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
					#endif
					if (( IMPERIAL == stConfig_Value.ucTypeofUnit ) && ( INVALID_DATA != stCurrent_Value.fCurrInsideAmbientTemp ))
					{
						stCurrent_Value.fCurrInsideAmbientTemp = fnMetricToImperial_Conv(stCurrent_Value.fCurrInsideAmbientTemp, TEMPERATURE_VALUE);
					}
        }break;
				
				case DATA_AMB_TEMP_OUTSIDE:		/*Ambient Temparature outside the container*/
        {
					#ifdef DBG_CAN      
						DEBUG_PRINTF("DATA_MCU_AMB_TEMP_OUTSIDE\r\n");
					#endif	
          
				 for(ucCnt=0; ucCnt < pstCANrxmsg->DLC; ucCnt++)
					{
						SplitByte.ucChar[ucCnt] = pstCANrxmsg->Data[ucCnt];
					}						
					
					stCurrent_Value.fCurrOutsideAmbientTemp = SplitByte.fFloat;
					#ifdef DBG_CAN     
						memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
						sprintf(rgcTempCAN,"DATA_MCU_AMB_TEMP_OUTSIDE_CURRENT: %0.2f", stCurrent_Value.fCurrOutsideAmbientTemp);	
						DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
					#endif	
					if (( IMPERIAL == stConfig_Value.ucTypeofUnit ) && ( INVALID_DATA != stCurrent_Value.fCurrOutsideAmbientTemp ))
					{
						stCurrent_Value.fCurrOutsideAmbientTemp = fnMetricToImperial_Conv(stCurrent_Value.fCurrOutsideAmbientTemp, TEMPERATURE_VALUE);
					}
        }break;
				
				case DATA_AMB_HUMIDITY:		/*Ambient humidity*/
        {
					#ifdef DBG_CAN      
						DEBUG_PRINTF("DATA_AMB_HUMIDITY\r\n");
					#endif	
          
					for(ucCnt=0; ucCnt < pstCANrxmsg->DLC; ucCnt++)
					{
						SplitByte.ucChar[ucCnt] = pstCANrxmsg->Data[ucCnt];
					}						
					
					stCurrent_Value.fCurrOutsideAmbientHumidity = SplitByte.fFloat;
					#ifdef DBG_CAN     
						memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
						sprintf(rgcTempCAN,"DATA_AMB_HUMIDITY: %0.2f", stCurrent_Value.fCurrOutsideAmbientHumidity);	
						DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
					#endif
					/*if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
					{
						stCurrent_Value.fCurrOutsideAmbientHumidity = fnMetricToImperial_Conv(stCurrent_Value.fCurrOutsideAmbientHumidity, TEMPERATURE_VALUE);
					}*/
        }break;
				case DATA_MATERIAL_USED:			/*Material usage*/
        {
					#ifdef DBG_CAN      
						DEBUG_PRINTF("DATA_MATERIAL_USED\r\n");
					#endif	
                  
					for(ucCnt = 1; ucCnt < pstCANrxmsg->DLC; ucCnt++)
					{
						SplitByte.ucChar[ucCnt - 1] = pstCANrxmsg->Data[ucCnt];
					}

					if ( LEG_A_DATA == pstCANrxmsg->Data[0] )
					{
						ucMaterialDataReceived |= LEG_A_DATA;
						
						stCurrent_Value.fLegAMaterialUsed = SplitByte.fFloat;
						#ifdef DBG_CAN     
							memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
							sprintf(rgcTempCAN,"LEG_A_DATA: %0.2f", stCurrent_Value.fLegAMaterialUsed);	
							DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
						#endif		
					}
					else	if ( LEG_B_DATA == pstCANrxmsg->Data[0] )
					{
						ucMaterialDataReceived |= LEG_B_DATA;
						
						stCurrent_Value.fLegBMaterialUsed = SplitByte.fFloat;
						#ifdef DBG_CAN     
							memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
							sprintf(rgcTempCAN,"LEG_B_DATA: %0.2f", stCurrent_Value.fLegBMaterialUsed);	
							DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
						#endif		
					}
					else if ( LEG_C_DATA == pstCANrxmsg->Data[0] )
					{
						ucMaterialDataReceived |= LEG_C_DATA;
						
						stCurrent_Value.fLegCMaterialUsed = SplitByte.fFloat;
						#ifdef DBG_CAN     
							memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
							sprintf(rgcTempCAN,"LEG_C_DATA: %0.2f", stCurrent_Value.fLegCMaterialUsed);	
							DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
						#endif		
					}
					
					if ( MATERIAL_DATA_RECEIVED == ucMaterialDataReceived )
					{
						ucMaterialDataReceived = 0;
						if ( PRIME_PROCESS == ucLastProcessDone )
						{
							stCurrent_Value.fBaseMaterialPrimed += (stCurrent_Value.fLegAMaterialUsed + stCurrent_Value.fLegCMaterialUsed);
							stCurrent_Value.fHardMaterialPrimed += stCurrent_Value.fLegBMaterialUsed;
						}
						
						if ( SPRAY_PROCESS == ucLastProcessDone )
						{
							stCurrent_Value.fBaseMaterialApplied += (stCurrent_Value.fLegAMaterialUsed + stCurrent_Value.fLegCMaterialUsed);
							stCurrent_Value.fHardMaterialApplied += stCurrent_Value.fLegBMaterialUsed;

							stCurrent_Value.fBaseTotalMaterialUsed = stCurrent_Value.fBaseMaterialApplied + stCurrent_Value.fBaseMaterialPrimed;
							stCurrent_Value.fHardTotalMaterialUsed = stCurrent_Value.fHardMaterialPrimed + stCurrent_Value.fHardMaterialApplied;
						}
					}
        }break;
				case DATA_STALL_PRESSURE_CHANGE:		/*Stall test pressure change reading for 
																							Upstroke and Downstroke*/
        {
					#ifdef DBG_CAN      
						DEBUG_PRINTF("DATA_STALL_PRESSURE_CHANGE\r\n");
					#endif			

					if ( DOWNSTROKE == pstCANrxmsg->Data[0] )
						{
							
							SplitByte.cChar[0] = pstCANrxmsg->Data[1];
							SplitByte.cChar[1] = pstCANrxmsg->Data[2];
							stCurrent_Value.iCurrPressureLegADown = SplitByte.iData;
							
							#ifdef DBG_CAN     
								memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
								sprintf(rgcTempCAN,"fdPressure_LegA_Down_Curr : %d", stCurrent_Value.iCurrPressureLegADown );	
								DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
							#endif		
							
							SplitByte.cChar[0] = pstCANrxmsg->Data[3];
							SplitByte.cChar[1] = pstCANrxmsg->Data[4];
							
							stCurrent_Value.iCurrPressureLegBDown = SplitByte.iData;
							#ifdef DBG_CAN     
								memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
								sprintf(rgcTempCAN,"fdPressure_LegB_Down_Curr : %d", stCurrent_Value.iCurrPressureLegBDown );	
								DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
							#endif		
							
							SplitByte.cChar[0] = pstCANrxmsg->Data[5];
							SplitByte.cChar[1] = pstCANrxmsg->Data[6];
							stCurrent_Value.iCurrPressureLegCDown = SplitByte.iData;
							#ifdef DBG_CAN     
								memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
								sprintf(rgcTempCAN,"fdPressure_LegC_Down_Curr : %d", stCurrent_Value.iCurrPressureLegCDown );	
								DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
							#endif		
						}
						
					if ( UPSTROKE == pstCANrxmsg->Data[0] )
						{
							SplitByte.cChar[0] = pstCANrxmsg->Data[1];
							SplitByte.cChar[1] = pstCANrxmsg->Data[2];
							stCurrent_Value.iCurrPressureLegAUp = SplitByte.iData;
							#ifdef DBG_CAN     
								memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
								sprintf(rgcTempCAN,"fdPressure_LegA_Up_Curr : %d", stCurrent_Value.iCurrPressureLegAUp );	
								DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
							#endif		
							
							SplitByte.cChar[0] = pstCANrxmsg->Data[3];
							SplitByte.cChar[1] = pstCANrxmsg->Data[4];
							stCurrent_Value.iCurrPressureLegBUp = SplitByte.iData;
							#ifdef DBG_CAN     
								memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
								sprintf(rgcTempCAN,"fdPressure_LegB_Up_Curr : %d", stCurrent_Value.iCurrPressureLegBUp );	
								DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
							#endif		
							
							SplitByte.cChar[0] = pstCANrxmsg->Data[5];
							SplitByte.cChar[1] = pstCANrxmsg->Data[6];
							stCurrent_Value.iCurrPressureLegCUp = SplitByte.iData;
							
							#ifdef DBG_CAN     
								memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
								sprintf(rgcTempCAN,"fdPressure_LegC_Up_Curr : %d", stCurrent_Value.iCurrPressureLegCUp );	
								DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
							#endif		
						}
        }break;
				case DATA_PGAUGE1_VALUE:						/*Pressure gauge 1 reading*/
        {
					#ifdef DBG_CAN      
						DEBUG_PRINTF("DATA_PGAUGE1_VALUE\r\n");
					#endif	
          
          //fnSendACK(pstCANrxmsg);
          for(ucCnt=0; ucCnt < pstCANrxmsg->DLC; ucCnt++)
					{
						SplitByte.ucChar[ucCnt] = pstCANrxmsg->Data[ucCnt];
					}			
					stCurrent_Value.fCurrPressureGauge1 = SplitByte.fFloat;
					#ifdef DBG_CAN     
						memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
						sprintf(rgcTempCAN,"Pressure_Gauge1_Curr %0.2f", stCurrent_Value.fCurrPressureGauge1);	
						DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
					#endif
					if (( IMPERIAL == stConfig_Value.ucTypeofUnit ) && ( INVALID_DATA != stCurrent_Value.fCurrPressureGauge1 ))
					{
						stCurrent_Value.fCurrPressureGauge1 = fnMetricToImperial_Conv(stCurrent_Value.fCurrPressureGauge1, PRESSURE_VALUE);
					}
        }break;
				
				case DATA_PGAUGE2_VALUE:						/*Pressure gauge 2 reading*/
        {
					#ifdef DBG_CAN      
						DEBUG_PRINTF("DATA_PGAUGE2_VALUE\r\n");
					#endif	
          
          //fnSendACK(pstCANrxmsg);
          for(ucCnt=0; ucCnt < pstCANrxmsg->DLC; ucCnt++)
					{
						SplitByte.ucChar[ucCnt] = pstCANrxmsg->Data[ucCnt];
					}			

					stCurrent_Value.fCurrPressureGauge2 = SplitByte.fFloat;
					#ifdef DBG_CAN     
						memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
						sprintf(rgcTempCAN,"fdPressure_Gauge2_Curr %0.2f", stCurrent_Value.fCurrPressureGauge2);	
						DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
					#endif
					if (( IMPERIAL == stConfig_Value.ucTypeofUnit ) && ( INVALID_DATA != stCurrent_Value.fCurrPressureGauge2 ))
					{
						stCurrent_Value.fCurrPressureGauge2 = fnMetricToImperial_Conv(stCurrent_Value.fCurrPressureGauge2, PRESSURE_VALUE);
					}
        }break;	
				
				case DATA_PGAUGE3_VALUE:						/*Pressure gauge 3 reading*/
        {
					#ifdef DBG_CAN      
						DEBUG_PRINTF("DATA_PGAUGE3_VALUE\r\n");
					#endif	
          
          for(ucCnt=0; ucCnt < pstCANrxmsg->DLC; ucCnt++)
					{
						SplitByte.ucChar[ucCnt] = pstCANrxmsg->Data[ucCnt];
					}			

					stCurrent_Value.fCurrPressureGauge3 = SplitByte.fFloat;
					#ifdef DBG_CAN     
						memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
						sprintf(rgcTempCAN,"fdPressure_Gauge3_Curr %0.2f",stCurrent_Value.fCurrPressureGauge3);	
						DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
					#endif
					if (( IMPERIAL == stConfig_Value.ucTypeofUnit ) && ( INVALID_DATA != stCurrent_Value.fCurrPressureGauge3 ))
					{
						stCurrent_Value.fCurrPressureGauge3 = fnMetricToImperial_Conv(stCurrent_Value.fCurrPressureGauge3, PRESSURE_VALUE);
					}
        }break;	
				case DATA_LT_READ:									/*Linear tansducer reading*/
				{
						for(ucCnt=0; ucCnt < pstCANrxmsg->DLC; ucCnt++)
						{
							SplitByte.ucChar[ucCnt] = pstCANrxmsg->Data[ucCnt];
						}			

						stCurrent_Value.fCurrLinearTransducer = SplitByte.fFloat;
				}break;
				case DATA_OPMM_PIPE_TEMP:						/*Pepe temperature from OPMM*/
        {
					#ifdef DBG_CAN      
						DEBUG_PRINTF("DATA_OPMM_PIPE_TEMP\r\n");
					#endif	
          
					for(ucCnt=0; ucCnt < pstCANrxmsg->DLC; ucCnt++)
						{
							SplitByte.ucChar[ucCnt] = pstCANrxmsg->Data[ucCnt+1];
						}						

						if ( PIPE_POSITION_12 == pstCANrxmsg->Data[0] )
						{
							stCurrent_Value.fCurrPipeTempPos12 = SplitByte.fFloat;
							#ifdef DBG_CAN     
								memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
								sprintf(rgcTempCAN,"POS12_PIPE_TEMP_CURRENT: %0.2f", stCurrent_Value.fCurrPipeTempPos12);	
								DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
							#endif		
							if (( IMPERIAL == stConfig_Value.ucTypeofUnit ) && ( INVALID_DATA != stCurrent_Value.fCurrPipeTempPos12 ))
							{
								stCurrent_Value.fCurrPipeTempPos12 = fnMetricToImperial_Conv(stCurrent_Value.fCurrPipeTempPos12, TEMPERATURE_VALUE);
							}
						}
						else	if ( PIPE_POSITION_3 == pstCANrxmsg->Data[0] )
						{
							stCurrent_Value.fCurrPipeTempPos3 = SplitByte.fFloat;
							#ifdef DBG_CAN     
								memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
								sprintf(rgcTempCAN,"POS3_PIPE_TEMP_CURRENT: %0.2f", stCurrent_Value.fCurrPipeTempPos3);	
								DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
							#endif
							if (( IMPERIAL == stConfig_Value.ucTypeofUnit ) && ( INVALID_DATA != stCurrent_Value.fCurrPipeTempPos3 ))
							{
								stCurrent_Value.fCurrPipeTempPos3 = fnMetricToImperial_Conv(stCurrent_Value.fCurrPipeTempPos3, TEMPERATURE_VALUE);
							}
						}
						else if ( PIPE_POSITION_6 == pstCANrxmsg->Data[0] )
						{
							stCurrent_Value.fCurrPipeTempPos6 = SplitByte.fFloat;
							#ifdef DBG_CAN     
								memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
								sprintf(rgcTempCAN,"POS6_PIPE_TEMP_CURRENT: %0.2f", stCurrent_Value.fCurrPipeTempPos6);	
								DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
							#endif
							if (( IMPERIAL == stConfig_Value.ucTypeofUnit ) && ( INVALID_DATA != stCurrent_Value.fCurrPipeTempPos6 ))
							{
								stCurrent_Value.fCurrPipeTempPos6 = fnMetricToImperial_Conv(stCurrent_Value.fCurrPipeTempPos6, TEMPERATURE_VALUE);
							}
						}
						else if ( PIPE_POSITION_9 == pstCANrxmsg->Data[0] )
						{
							stCurrent_Value.fCurrPipeTempPos9 = SplitByte.fFloat;
							#ifdef DBG_CAN     
								memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
								sprintf(rgcTempCAN,"POS9_PIPE_TEMP_CURRENT: %0.2f", stCurrent_Value.fCurrPipeTempPos9);	
								DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
							#endif
							if (( IMPERIAL == stConfig_Value.ucTypeofUnit ) && ( INVALID_DATA != stCurrent_Value.fCurrPipeTempPos9 ))
							{
								stCurrent_Value.fCurrPipeTempPos9 = fnMetricToImperial_Conv(stCurrent_Value.fCurrPipeTempPos9, TEMPERATURE_VALUE);
							}
						}
						else if ( AVG_TEMP == pstCANrxmsg->Data[0] )
						{
							stCurrent_Value.fCurrPipeTempAvg = SplitByte.fFloat;
							#ifdef DBG_CAN     
								memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
								sprintf(rgcTempCAN,"AVG_PIPE_TEMP_CURRENT: %0.2f", stCurrent_Value.fCurrPipeTempAvg);	
								DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
							#endif
							if (( IMPERIAL == stConfig_Value.ucTypeofUnit ) && ( INVALID_DATA != stCurrent_Value.fCurrPipeTempAvg ))
							{
								stCurrent_Value.fCurrPipeTempAvg = fnMetricToImperial_Conv(stCurrent_Value.fCurrPipeTempAvg, TEMPERATURE_VALUE);
							}
						}
        }break;
				case DATA_OPMM_BATTERY_STATUS:			/*Battery reading*/
        {
					#ifdef DBG_CAN      
						DEBUG_PRINTF("DATA_OPMM_BATTERY_STATUS\r\n");
					#endif	
          
          for(ucCnt=0; ucCnt < pstCANrxmsg->DLC; ucCnt++)
					{
						SplitByte.ucChar[ucCnt] = pstCANrxmsg->Data[ucCnt];
					}			

					stCurrent_Value.fCurrBateryStatus = SplitByte.fFloat;
					#ifdef DBG_CAN
						memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
						sprintf(rgcTempCAN,"fdBat_Status_Curr: %0.2f", stCurrent_Value.fCurrBateryStatus );
						DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
					#endif
				}break;
				case DATA_PROCESS_TIME:							/*Time taken for Prime,spray and flush 
																							processes*/
        {
					#ifdef DBG_CAN      
						DEBUG_PRINTF("DATA_PROCESS_TIME\r\n");
					#endif	
          
          for(ucCnt=0; ucCnt < pstCANrxmsg->DLC; ucCnt++)
						{
							SplitByte.ucChar[ucCnt] = pstCANrxmsg->Data[ucCnt+1];
						}			

					if ( PRIME_PROCESS == pstCANrxmsg->Data[0] )
						{
						stCurrent_Value.fCurrPrimeTime = SplitByte.fFloat;
						#ifdef DBG_CAN     
							memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
							sprintf(rgcTempCAN,"fdTime_Prime_Curr : %0.2f", stCurrent_Value.fCurrPrimeTime );	
							DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
						#endif		
						}
						
					if ( SPRAY_PROCESS == pstCANrxmsg->Data[0] )
						{
						stCurrent_Value.fCurrSprayTime = SplitByte.fFloat;
						#ifdef DBG_CAN     
							memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
							sprintf(rgcTempCAN,"fdTime_Spray_Curr : %0.2f", stCurrent_Value.fCurrSprayTime );	
							DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
						#endif		
						}
						
					if ( FLUSH_PROCESS == pstCANrxmsg->Data[0] )
						{
						stCurrent_Value.fCurrFlushTime = SplitByte.fFloat;
						#ifdef DBG_CAN     
							memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
							sprintf(rgcTempCAN,"fdTime_Flush_Curr : %0.2f", stCurrent_Value.fCurrFlushTime );	
							DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
						#endif		
						}
				}break;	
				
					
											
				case DATA_ROTATIONS_COMPLETED:			/*Number of current rotations completed*/
        {
					#ifdef DBG_CAN      
						DEBUG_PRINTF("DATA_ROTATIONS_COMPLETED\r\n");
					#endif	
          			
					stCurrent_Value.ucCurrNumberofRotation = pstCANrxmsg->Data[0];
					#ifdef DBG_CAN     
						memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
						sprintf(rgcTempCAN,"ucCurrNumberofRotation: %d", stCurrent_Value.ucCurrNumberofRotation );	
						DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
					#endif
				}break;	
				case DATA_OPMM_BASE_TEMP:
				{
					#ifdef DBG_CAN      
						DEBUG_PRINTF("DATA_OPMM_BASE_TEMP\r\n");
					#endif	
          
          for(ucCnt=0; ucCnt < pstCANrxmsg->DLC; ucCnt++)
					{
						SplitByte.ucChar[ucCnt] = pstCANrxmsg->Data[ucCnt];
					}			

					stCurrent_Value.fCurrOPMMBaseTemp = SplitByte.fFloat;
					#ifdef DBG_CAN     
						memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
						sprintf(rgcTempCAN,"DATA_OPMM_BASE_TEMP %0.2f",stCurrent_Value.fCurrOPMMBaseTemp);	
						DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
					#endif
					if (( IMPERIAL == stConfig_Value.ucTypeofUnit ) && ( INVALID_DATA != stCurrent_Value.fCurrOPMMBaseTemp ))
					{
						stCurrent_Value.fCurrOPMMBaseTemp = fnMetricToImperial_Conv(stCurrent_Value.fCurrOPMMBaseTemp, TEMPERATURE_VALUE);
					}
				}break;
				case DATA_OPMM_HARD_TEMP:
				{
					#ifdef DBG_CAN      
						DEBUG_PRINTF("DATA_OPMM_HARD_TEMP\r\n");
					#endif	
          
          for(ucCnt=0; ucCnt < pstCANrxmsg->DLC; ucCnt++)
					{
						SplitByte.ucChar[ucCnt] = pstCANrxmsg->Data[ucCnt];
					}			

					stCurrent_Value.fCurrOPMMHardTemp = SplitByte.fFloat;
					#ifdef DBG_CAN     
						memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
						sprintf(rgcTempCAN,"DATA_OPMM_HARD_TEMP %0.2f",stCurrent_Value.fCurrOPMMHardTemp);	
						DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
					#endif
					if (( IMPERIAL == stConfig_Value.ucTypeofUnit ) && ( INVALID_DATA != stCurrent_Value.fCurrOPMMHardTemp ))
					{
						stCurrent_Value.fCurrOPMMHardTemp = fnMetricToImperial_Conv(stCurrent_Value.fCurrOPMMHardTemp, TEMPERATURE_VALUE);
					}
				}break;
				
				/*case DATA_ETH_WIFI_IP_DETAILS:
				{
					if( ETHERNET == pstCANrxmsg->Data[0])
					{
						stCurrent_Value.ucEthIPSlot1 = pstCANrxmsg->Data[1];
						stCurrent_Value.ucEthIPSlot2 = pstCANrxmsg->Data[2];
						stCurrent_Value.ucEthIPSlot3 = pstCANrxmsg->Data[3];
						stCurrent_Value.ucEthIPSlot4 = pstCANrxmsg->Data[4];
						
						SplitByte.cChar[0] = pstCANrxmsg->Data[5];
						SplitByte.cChar[1] = pstCANrxmsg->Data[6];
						stCurrent_Value.iEthPort = SplitByte.iData;
					}
					else if( WIFI == pstCANrxmsg->Data[0])
					{
						stCurrent_Value.ucWiFiIPSlot1 = pstCANrxmsg->Data[1];
						stCurrent_Value.ucWiFiIPSlot2 = pstCANrxmsg->Data[2];
						stCurrent_Value.ucWiFiIPSlot3 = pstCANrxmsg->Data[3];
						stCurrent_Value.ucWiFiIPSlot4 = pstCANrxmsg->Data[4];

						SplitByte.cChar[0] = pstCANrxmsg->Data[5];
						SplitByte.cChar[1] = pstCANrxmsg->Data[6];
						stCurrent_Value.iWiFiPort = SplitByte.iData;
					}
				}break;*/
				
				case DATA_ACC_MATERIAL_USAGE:
				{
					for(ucCnt = 1; ucCnt < pstCANrxmsg->DLC; ucCnt++)
					{
						SplitByte.ucChar[ucCnt - 1] = pstCANrxmsg->Data[ucCnt];
					}
					/*change6*//*change21*/
					/*if imperial then in oz*/
											
					
					if( IMPERIAL == stConfig_Value.ucTypeofUnit )
					{
						
						SplitByte.fFloat = (float)(CONVERT_MM3_TO_OZ(SplitByte.fFloat));
					}
					else if( METRIC == stConfig_Value.ucTypeofUnit ) /*if metric then in litres*/
					{
						SplitByte.fFloat = (float)(CONVERT_MM3_TO_L(SplitByte.fFloat));
					}
					
					if( BIT_0 == pstCANrxmsg->Data[0])
					{
						//DEBUG_PRINTF("BIT_0 == pstCANrxmsg->Data[0]\r\n");
						/*change23*/
					/*	if( METRIC == stConfig_Value.ucTypeofUnit )
					{
						SplitByte.fFloat = (float)(CONVERT_L_TO_ML(SplitByte.fFloat));
					}
						*/
						stCurrent_Value.fBaseMatTotalJoint = SplitByte.fFloat;
						
					}
					else if( BIT_1 == pstCANrxmsg->Data[0])
					{
					//	DEBUG_PRINTF("BIT_1 == pstCANrxmsg->Data[0]\r\n");
						/*change23*/
					/*	if( METRIC == stConfig_Value.ucTypeofUnit )
					{
						SplitByte.fFloat = (float)(CONVERT_L_TO_ML(SplitByte.fFloat));
					}
						*/
						
						stCurrent_Value.fHardMatTotalJoint = SplitByte.fFloat;
					}
					else if( BIT_2 == pstCANrxmsg->Data[0])
					{
						//DEBUG_PRINTF("BIT_2 == pstCANrxmsg->Data[0]\r\n");
						
						stCurrent_Value.fBaseMatPrimeAcc = SplitByte.fFloat;
					}
					else if( BIT_3 == pstCANrxmsg->Data[0])
					{
						//DEBUG_PRINTF("BIT_3 == pstCANrxmsg->Data[0]\r\n");
						
					stCurrent_Value.fHardMatPrimeAcc = SplitByte.fFloat;
					}
					else if( BIT_4 == pstCANrxmsg->Data[0])
					{//DEBUG_PRINTF("BIT_4 == pstCANrxmsg->Data[0]\r\n");
						
						stCurrent_Value.fBaseMatSprayAcc = SplitByte.fFloat;
					}
					else if( BIT_5 == pstCANrxmsg->Data[0])
					{//DEBUG_PRINTF("5 == pstCANrxmsg->Data[0]\r\n");
						
						stCurrent_Value.fHardMatSprayAcc = SplitByte.fFloat;
					}
					else if( BIT_6 == pstCANrxmsg->Data[0])
					{//DEBUG_PRINTF("BIT_6 == pstCANrxmsg->Data[0]\r\n");
					
						stCurrent_Value.fBaseMatTotalAcc = SplitByte.fFloat;
					
					}
					else if( BIT_7 == pstCANrxmsg->Data[0])
					{//DEBUG_PRINTF("BIT_7 == pstCANrxmsg->Data[0]\r\n");
						
						stCurrent_Value.fHardMatTotalAcc = SplitByte.fFloat;
					}
				}break;
       case ACKNOWLEDGEMENT:								/*ACK for data messages*/
        {
					 #ifdef DBG_CAN      
							DEBUG_PRINTF("Msg ID: Data_MSG_ACK received\r\n");
					 #endif	
           StatusFlag.stStatusFlag.bACK_Received = TRUE;
        }break;
        
			 default :
       break; 
      }
    }break;
		
		case MSG_TYPE_CONFIG:				/*Configuration message*/
    {
		 #ifdef DBG_CAN      
				DEBUG_PRINTF("Msg Type: MSG_TYPE_CONFIG\r\n");
		 #endif			
		 switch (uliCanID & MSG_MASK)
		 {
			case ACKNOWLEDGEMENT:									/*ACK for Config messages*/
				{
					#ifdef DBG_CAN      
						DEBUG_PRINTF("Msg ID: MSG_TYPE_CONFIG received\r\n");
					#endif	
				 
				 StatusFlag.stStatusFlag.bACK_Received = TRUE;
				 
			 }break;
			 
			case CONFIG_SPRAY_PRESSURE :					/*ACK for data messages*/
				{
					#ifdef DBG_CAN      
						DEBUG_PRINTF("CONFIG_SPRAY_PRESSURE\r\n");
					#endif	        

					for(ucCnt=0; ucCnt < pstCANrxmsg->DLC; ucCnt++)
					{
						SplitByte.ucChar[ucCnt] = pstCANrxmsg->Data[ucCnt+1];
					}
					
					if (BIT_0 == (pstCANrxmsg->Data[0] & BIT_0)) 
					{
						stConfig_Value.fConfigSprayPressure = SplitByte.fFloat;
						if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
						{
							stConfig_Value.fConfigSprayPressure = fnMetricToImperial_Conv(stConfig_Value.fConfigSprayPressure, PRESSURE_VALUE);
						}
						
						#ifdef DBG_CAN     
							memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
							sprintf(rgcTempCAN,"fConfigSprayPressure: %0.2f", stConfig_Value.fConfigSprayPressure );	
							DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
						#endif
					}
					else if (BIT_1 == (pstCANrxmsg->Data[0] & BIT_1))
					{
						stConfig_Value.fConfigSprayPressureLTH = SplitByte.fFloat;
						if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
						{
							stConfig_Value.fConfigSprayPressureLTH= fnMetricToImperial_Conv(stConfig_Value.fConfigSprayPressureLTH, PRESSURE_VALUE);
						}
						#ifdef DBG_CAN     
							memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
							sprintf(rgcTempCAN,"fConfigSprayPressureLTH: %0.2f", stConfig_Value.fConfigSprayPressureLTH );	
							DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
						#endif
					}
					else if (BIT_2 == (pstCANrxmsg->Data[0] & BIT_2))
					{
						stConfig_Value.fConfigSprayPressureHTH = SplitByte.fFloat;
						if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
						{
							stConfig_Value.fConfigSprayPressureHTH= fnMetricToImperial_Conv(stConfig_Value.fConfigSprayPressureHTH, PRESSURE_VALUE);
						}
						#ifdef DBG_CAN     
							memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
							sprintf(rgcTempCAN,"fConfigSprayPressureHTH: %0.2f", stConfig_Value.fConfigSprayPressureHTH );	
							DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
						#endif
					}
					else if (BIT_3 == (pstCANrxmsg->Data[0] & BIT_3))
					{
						stConfig_Value.fConfigHysSprayPresPos = SplitByte.fFloat;
						if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
						{
							stConfig_Value.fConfigHysSprayPresPos= fnMetricToImperial_Conv(stConfig_Value.fConfigHysSprayPresPos, PRESSURE_VALUE);
						}
						#ifdef DBG_CAN     
							memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
							sprintf(rgcTempCAN,"fConfigHysSprayPresPos: %0.2f", stConfig_Value.fConfigHysSprayPresPos );	
							DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
						#endif
					}
					else if (BIT_4 == (pstCANrxmsg->Data[0] & BIT_4))
					{
						stConfig_Value.fConfigHysSprayPresNeg = SplitByte.fFloat;
						if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
						{
							stConfig_Value.fConfigHysSprayPresNeg= fnMetricToImperial_Conv(stConfig_Value.fConfigHysSprayPresNeg, PRESSURE_VALUE);
						}
						#ifdef DBG_CAN     
							memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
							sprintf(rgcTempCAN,"fConfigHysSprayPresNeg: %0.2f", stConfig_Value.fConfigHysSprayPresNeg );	
							DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
						#endif
					}
			 }break;        
			case CONFIG_RECIRC_PRESSURE:					/*Recirculation pressure config value*/
				{
					#ifdef DBG_CAN      
						DEBUG_PRINTF("CONFIG_RECIRC_PRESSURE\r\n");
					#endif	        

					for(ucCnt=0; ucCnt < pstCANrxmsg->DLC; ucCnt++)
					{
						SplitByte.ucChar[ucCnt] = pstCANrxmsg->Data[ucCnt + 1];
					}			

					if (BIT_0 == (pstCANrxmsg->Data[0] & BIT_0)) 
					{
						stConfig_Value.fConfigRecirPressure = SplitByte.fFloat;
						if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
						{
							stConfig_Value.fConfigRecirPressure= fnMetricToImperial_Conv(stConfig_Value.fConfigRecirPressure, PRESSURE_VALUE);
						}
						
						#ifdef DBG_CAN     
							memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
							sprintf(rgcTempCAN,"fConfigRecirPressure: %0.2f", stConfig_Value.fConfigRecirPressure );	
							DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
						#endif
					}
					else if (BIT_1 == (pstCANrxmsg->Data[0] & BIT_1))
					{
						stConfig_Value.fConfigRecirPressureLTH = SplitByte.fFloat;
						if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
						{
							stConfig_Value.fConfigRecirPressureLTH= fnMetricToImperial_Conv(stConfig_Value.fConfigRecirPressureLTH, PRESSURE_VALUE);
						}
						#ifdef DBG_CAN     
							memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
							sprintf(rgcTempCAN,"fConfigRecirPressureLTH: %0.2f", stConfig_Value.fConfigRecirPressureLTH );	
							DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
						#endif
					}
					else if (BIT_2 == (pstCANrxmsg->Data[0] & BIT_2))
					{
						stConfig_Value.fConfigRecirPressureHTH = SplitByte.fFloat;
						if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
						{
							stConfig_Value.fConfigRecirPressureHTH= fnMetricToImperial_Conv(stConfig_Value.fConfigRecirPressureHTH, PRESSURE_VALUE);
						}
						#ifdef DBG_CAN     
							memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
							sprintf(rgcTempCAN,"fConfigRecirPressureHTH: %0.2f", stConfig_Value.fConfigRecirPressureHTH );	
							DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
						#endif
					}
					else if (BIT_3 == (pstCANrxmsg->Data[0] & BIT_3))
					{
						stConfig_Value.fConfigHysRecircPresPos = SplitByte.fFloat;
						if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
						{
							stConfig_Value.fConfigHysRecircPresPos= fnMetricToImperial_Conv(stConfig_Value.fConfigHysRecircPresPos, PRESSURE_VALUE);
						}
						#ifdef DBG_CAN     
							memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
							sprintf(rgcTempCAN,"fConfigHysRecircPresPos: %0.2f", stConfig_Value.fConfigHysRecircPresPos );	
							DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
						#endif
					}
					else if (BIT_4 == (pstCANrxmsg->Data[0] & BIT_4))
					{
						stConfig_Value.fConfigHysRecircPresNeg = SplitByte.fFloat;
						if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
						{
							stConfig_Value.fConfigHysRecircPresNeg= fnMetricToImperial_Conv(stConfig_Value.fConfigHysRecircPresNeg, PRESSURE_VALUE);
						}
						#ifdef DBG_CAN     
							memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
							sprintf(rgcTempCAN,"fConfigHysRecircPresNeg: %0.2f", stConfig_Value.fConfigHysRecircPresNeg );	
							DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
						#endif
					}
			 }break;
			case CONFIG_JOINT_NUMBER:							/*Joint number*/
				{
					#ifdef DBG_CAN      
					 DEBUG_PRINTF("CONFIG_JOINT_NUMBER\r\n");
					#endif	        

					for(ucCnt=0; ucCnt < pstCANrxmsg->DLC; ucCnt++)
					{
						SplitByte.ucChar[ucCnt] = pstCANrxmsg->Data[ucCnt];
					}			

					stConfig_Value.fConfigJointNum= SplitByte.fFloat;
					#ifdef DBG_CAN     
						memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
						sprintf(rgcTempCAN,"CONFIG_JOINT_NUMBER: %0.2f", stConfig_Value.fConfigJointNum);	
						DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
					#endif				           
			 }break;
			case CONFIG_DATA_LOG_PERIOD:					/*Periodic log period*/
				{
					#ifdef DBG_CAN      
					 DEBUG_PRINTF("CONFIG_DATA_LOG_PERIOD\r\n");
					#endif	        

					for(ucCnt=0; ucCnt < pstCANrxmsg->DLC; ucCnt++)
					{
						SplitByte.ucChar[ucCnt] = pstCANrxmsg->Data[ucCnt];
					}			

					stConfig_Value.fConfigDataLoggingPeriod = SplitByte.fFloat;
					#ifdef DBG_CAN     
						memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
						sprintf(rgcTempCAN,"CONFIG_DATA_LOG_PERIOD: %0.2f", stConfig_Value.fConfigDataLoggingPeriod );	
						DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
					#endif				           
			 }break;
			case CONFIG_SYS_DATE_TIME:						/*System date and time*/
				{
					#ifdef DBG_CAN      
					 DEBUG_PRINTF("CONFIG_SYS_DATE_TIME\r\n");
					#endif	        
					if (( SCREEN_TIMER_SETTING_4 != uiCurrentScreen ) && ( SCREEN_KEYPAD != uiCurrentScreen ))
					{
						for(ucCnt=0; ucCnt < pstCANrxmsg->DLC; ucCnt++)
						{
							stConfig_Value.ucConfigSystemTime[ucCnt] = pstCANrxmsg->Data[ucCnt];
						}
						
						sprintf((char *)stDisplayDateTime.ucUTCHR,"%02d",stConfig_Value.ucConfigSystemTime[0]);
						
						
						sprintf((char *)stDisplayDateTime.ucUTCMIN,"%02d",stConfig_Value.ucConfigSystemTime[1]);
					
							
						sprintf((char *)stDisplayDateTime.ucUTCSEC,"%02d",stConfig_Value.ucConfigSystemTime[2]);
						
							
						sprintf((char *)stDisplayDateTime.ucUTCDD,"%02d",stConfig_Value.ucConfigSystemTime[3]);
						
							
						sprintf((char *)stDisplayDateTime.ucUTCMM ,"%02d",stConfig_Value.ucConfigSystemTime[4]);
						
							
						sprintf((char *)stDisplayDateTime.ucUTCYY,"%02d%02d",stConfig_Value.ucConfigSystemTime[5],stConfig_Value.ucConfigSystemTime[6]);

					TimeConvert();				
					#ifdef DBG_CAN 
							DEBUG_PRINTF("Hour :%d\r\n",stConfig_Value.ucConfigSystemTime[0]);
							DEBUG_PRINTF("Minute :%d\r\n",stConfig_Value.ucConfigSystemTime[1]);
							DEBUG_PRINTF("Second :%d\r\n",stConfig_Value.ucConfigSystemTime[2]);
							DEBUG_PRINTF("Date :%d\r\n",stConfig_Value.ucConfigSystemTime[3]);
							DEBUG_PRINTF("Month :%d\r\n",stConfig_Value.ucConfigSystemTime[4]);
							DEBUG_PRINTF("Year :%d%c\r\n",stConfig_Value.ucConfigSystemTime[5],stConfig_Value.ucConfigSystemTime[6]);
						 #endif 
						 /*change5*/
						 if(FALSE == stDiagStatus.bMCU_DiagStatusDisp)
						 {
							 stDiagStatus.bMCU_DiagStatusDisp = TRUE;
						 if(TRUE == stDiagStatus.bSCU_DiagStatus)
						 {
								fnStatusMessage_Store(SCU_DIAGNOSTIC_PASS);
						
									ucStatusMessageCounter++; 
						 }
					
						 else
							 
						 {
							 	fnStatusMessage_Store(SCU_DIAGNOSTIC_FAIL);
						
						fnAlarmMessage_Store(SCU_DIAGNOSTIC_FAIL,SCU_FAULT,SYSTEM_WARNING);
						ucStatusMessageCounter++;
						 }
					 }
						 if(FALSE == stDiagStatus.bHMI_DiagStatusDisp)
						 {
							 stDiagStatus.bHMI_DiagStatusDisp = TRUE;
						 if(TRUE == stDiagStatus.bHMI_DiagStatus)
						 {
						 fnStatusMessage_Store(HMI_DIAGNOSTIC_PASS);
							ucStatusMessageCounter++;
						 }
						  else
							 
						 {
							 	fnStatusMessage_Store(HMI_DIAGNOSTIC_FAIL);
						
				//		fnAlarmMessage_Store(HMI_DIAGNOSTIC_FAIL,SCU_FAULT,SYSTEM_WARNING);
						ucStatusMessageCounter++;
						 }
					 }
				 }					 
			 }break;
			case CONFIG_SUPERVISOR_NAME:					/*supervisor name*/
				{
					#ifdef DBG_CAN      
					 DEBUG_PRINTF("CONFIG_SUPERVISOR_NAME\r\n");
					#endif	        
		 
					if ( FRAME_1 == pstCANrxmsg->Data[0] )
					{
						 for(ucCnt=1; ucCnt < pstCANrxmsg->DLC; ucCnt++)
						{
							stConfig_Value.rgucConfigSupervisorName1[ucCnt - 1] = pstCANrxmsg->Data[ucCnt];
						}
						memset(rgucSupervisorName,'\0',sizeof(rgucSupervisorName));
						stConfig_Value.rgucConfigSupervisorName1[ucCnt - 1] = '\0';
						strcat((char *)rgucSupervisorName,(const char *)stConfig_Value.rgucConfigSupervisorName1);
						
						#ifdef DBG_CAN  
							stConfig_Value.rgucConfigSupervisorName1[ucCnt - 1] = '\0';						
							DEBUG_PRINTF("%s\r\n",stConfig_Value.rgucConfigSupervisorName1);						
						#endif							
					}
					else	if ( FRAME_2 == pstCANrxmsg->Data[0] )
					{
						for(ucCnt=1; ucCnt < pstCANrxmsg->DLC; ucCnt++)
						{
							stConfig_Value.rgucConfigSupervisorName2[ucCnt - 1] = pstCANrxmsg->Data[ucCnt];
						}			
						stConfig_Value.rgucConfigSupervisorName2[ucCnt - 1] = '\0';
						strcat((char *)rgucSupervisorName,(const char *)stConfig_Value.rgucConfigSupervisorName2);

						#ifdef DBG_CAN     
							stConfig_Value.rgucConfigSupervisorName2[ucCnt - 1] = '\0';
							DEBUG_PRINTF("%s\r\n",stConfig_Value.rgucConfigSupervisorName2);						
						#endif
					}
					else	if ( FRAME_3 == pstCANrxmsg->Data[0] )
					{
						for(ucCnt=1; ucCnt < pstCANrxmsg->DLC; ucCnt++)
						{
							stConfig_Value.rgucConfigSupervisorName3[ucCnt - 1] = pstCANrxmsg->Data[ucCnt];
						}		
						stConfig_Value.rgucConfigSupervisorName3[ucCnt - 1] = '\0';
						strcat((char *)rgucSupervisorName,(const char *)stConfig_Value.rgucConfigSupervisorName3);
						
						#ifdef DBG_CAN  
							stConfig_Value.rgucConfigSupervisorName3[ucCnt - 1] = '\0';
							DEBUG_PRINTF("%s\r\n",stConfig_Value.rgucConfigSupervisorName3);						
						#endif
						ucSwitchPressFlag = TRUE;						
					} 
			 }break;
			case CONFIG_COATING_MATERIAL:					/*coating material code*/
				{
					#ifdef DBG_CAN      
						DEBUG_PRINTF("CONFIG_COATING_MATERIAL\r\n");
					#endif	        
				if ( FRAME_1 == pstCANrxmsg->Data[0] )
				{
					for(ucCnt = 1; ucCnt < pstCANrxmsg->DLC; ucCnt++)
					{
						stConfig_Value.rgucConfigCoatingMatName1[ucCnt - 1] = pstCANrxmsg->Data[ucCnt];
					}
					memset(rgucCoatingMaterial,'\0',sizeof(rgucCoatingMaterial));
					stConfig_Value.rgucConfigCoatingMatName1[ucCnt - 1] = '\0';
					strcat((char *)rgucCoatingMaterial,(const char *)stConfig_Value.rgucConfigCoatingMatName1);
				
					#ifdef DBG_CAN     
						DEBUG_PRINTF("%s\r\n",stConfig_Value.rgucConfigCoatingMatName1);						
					#endif					
				}
				else	if ( FRAME_2 == pstCANrxmsg->Data[0] )
				{
					for(ucCnt = 1; ucCnt < pstCANrxmsg->DLC; ucCnt++)
					{
						stConfig_Value.rgucConfigCoatingMatName2[ucCnt - 1] = pstCANrxmsg->Data[ucCnt];
					}			
					stConfig_Value.rgucConfigCoatingMatName2[ucCnt - 1] = '\0';
					strcat((char *)rgucCoatingMaterial,(const char *)stConfig_Value.rgucConfigCoatingMatName2);
					
					#ifdef DBG_CAN     
						DEBUG_PRINTF("%s\r\n",stConfig_Value.rgucConfigCoatingMatName2);						
					#endif						
				}
				else	if ( FRAME_3 == pstCANrxmsg->Data[0] )
				{
					for(ucCnt = 1; ucCnt < pstCANrxmsg->DLC; ucCnt++)
					{
						stConfig_Value.rgucConfigCoatingMatName3[ucCnt - 1] = pstCANrxmsg->Data[ucCnt];
					}			
					stConfig_Value.rgucConfigCoatingMatName3[ucCnt - 1] = '\0';
					strcat((char *)rgucCoatingMaterial,(const char *)stConfig_Value.rgucConfigCoatingMatName3);
					
					#ifdef DBG_CAN     
						DEBUG_PRINTF("%s\r\n",stConfig_Value.rgucConfigCoatingMatName3);						
					#endif	
					ucSwitchPressFlag = TRUE;					
				}

			 }break;
			case CONFIG_BASE_MAT_BATCH_NO:				/*base material batch number*/
				{
					#ifdef DBG_CAN      
						DEBUG_PRINTF("CONFIG_BASE_MAT_BATCH_NO\r\n");
					#endif	        
					if ( FRAME_1 == pstCANrxmsg->Data[0] )
					{
						for(ucCnt = 1; ucCnt < pstCANrxmsg->DLC; ucCnt++)
						{
							stConfig_Value.rgucConfigBaseMatBatchNo1[ucCnt - 1] = pstCANrxmsg->Data[ucCnt];
						}			
						memset(rgucBaseMatBatchNumber,'\0',sizeof(rgucBaseMatBatchNumber));
						stConfig_Value.rgucConfigBaseMatBatchNo1[ucCnt - 1] = '\0';
						strcat((char *)rgucBaseMatBatchNumber,(const char *)stConfig_Value.rgucConfigBaseMatBatchNo1);
						
						#ifdef DBG_CAN     
							DEBUG_PRINTF("%s\r\n",stConfig_Value.rgucConfigBaseMatBatchNo1);						
						#endif							
					}
					else	if ( FRAME_2 == pstCANrxmsg->Data[0] )
					{
						for(ucCnt = 1; ucCnt < pstCANrxmsg->DLC; ucCnt++)
						{
							stConfig_Value.rgucConfigBaseMatBatchNo2[ucCnt - 1] = pstCANrxmsg->Data[ucCnt];
						}			
						stConfig_Value.rgucConfigBaseMatBatchNo2[ucCnt - 1] = '\0';
						strcat((char *)rgucBaseMatBatchNumber,(const char *)stConfig_Value.rgucConfigBaseMatBatchNo2);
						
						#ifdef DBG_CAN     
							DEBUG_PRINTF("%s\r\n",stConfig_Value.rgucConfigBaseMatBatchNo2);						
						#endif						
					}
					else	if ( FRAME_3 == pstCANrxmsg->Data[0] )
					{
						for(ucCnt = 1; ucCnt < pstCANrxmsg->DLC; ucCnt++)
						{
							stConfig_Value.rgucConfigBaseMatBatchNo3[ucCnt - 1] = pstCANrxmsg->Data[ucCnt];
						}			
						stConfig_Value.rgucConfigBaseMatBatchNo3[ucCnt - 1] = '\0';
						strcat((char *)rgucBaseMatBatchNumber,(const char *)stConfig_Value.rgucConfigBaseMatBatchNo3);
						
						#ifdef DBG_CAN     
							DEBUG_PRINTF("%s\r\n",stConfig_Value.rgucConfigBaseMatBatchNo3);						
						#endif	
						ucSwitchPressFlag = TRUE;
					}
			 }break;
			case CONFIG_HARD_MAT_BATCH_NO:				/*Hardener material batch number*/
				{
				#ifdef DBG_CAN      
					DEBUG_PRINTF("CONFIG_HARD_MAT_BATCH_NO\r\n");
				#endif	        
				if ( FRAME_1 == pstCANrxmsg->Data[0] )
				{
					for(ucCnt = 1; ucCnt < pstCANrxmsg->DLC; ucCnt++)
					{
						stConfig_Value.rgucConfigHardMatBatchNo1[ucCnt - 1] = pstCANrxmsg->Data[ucCnt];
					}			
					memset(rgucHardMatBatchNumber,'\0',sizeof(rgucHardMatBatchNumber));
					stConfig_Value.rgucConfigHardMatBatchNo1[ucCnt - 1] = '\0';
					strcat((char *)rgucHardMatBatchNumber,(const char *)stConfig_Value.rgucConfigHardMatBatchNo1);
					
					#ifdef DBG_CAN     
						DEBUG_PRINTF("%s\r\n",stConfig_Value.rgucConfigHardMatBatchNo1);						
					#endif							
				}
				else	if ( FRAME_2 == pstCANrxmsg->Data[0] )
				{
					for(ucCnt = 1; ucCnt < pstCANrxmsg->DLC; ucCnt++)
					{
						stConfig_Value.rgucConfigHardMatBatchNo2[ucCnt - 1] = pstCANrxmsg->Data[ucCnt];
					}		
					stConfig_Value.rgucConfigHardMatBatchNo2[ucCnt - 1] = '\0';
					strcat((char *)rgucHardMatBatchNumber,(const char *)stConfig_Value.rgucConfigHardMatBatchNo2);
					
					#ifdef DBG_CAN     
						DEBUG_PRINTF("%s\r\n",stConfig_Value.rgucConfigHardMatBatchNo2);						
					#endif					
				}
				else if ( FRAME_3 == pstCANrxmsg->Data[0] )
				{
					for(ucCnt = 1; ucCnt < pstCANrxmsg->DLC; ucCnt++)
					{
						stConfig_Value.rgucConfigHardMatBatchNo3[ucCnt - 1] = pstCANrxmsg->Data[ucCnt];
					}		
					stConfig_Value.rgucConfigHardMatBatchNo3[ucCnt - 1] = '\0';
					strcat((char *)rgucHardMatBatchNumber,(const char *)stConfig_Value.rgucConfigHardMatBatchNo3);
					
					#ifdef DBG_CAN     
						DEBUG_PRINTF("%c\r\n",stConfig_Value.rgucConfigHardMatBatchNo3);						
					#endif		
					ucSwitchPressFlag = TRUE;
				} 
			 }break;
			case CONFIG_TYPE_OF_UNIT:							/*Type of unit*/
				{

					stConfig_Value.ucTypeofUnit = pstCANrxmsg->Data[0];
					if(METRIC == stConfig_Value.ucTypeofUnit)
					{
						fnConvertConfigToMatric();
					}else if(IMPERIAL == stConfig_Value.ucTypeofUnit)
					{
						fnConvertConfigToImperial();
					}
					#ifdef DBG_CAN      
						DEBUG_PRINTF("CONFIG_TYPE_OF_UNIT METRIC = 0x01  ---IMPERAIL = 0x02 --- received = %d  \r\n", stConfig_Value.ucTypeofUnit);
					#endif
				
			}break;
			case CONFIG_PIPE_TEMP_TH:							/*Pipe temp threshold*/
				{
					#ifdef DBG_CAN      
						DEBUG_PRINTF("CONFIG_PIPE_TEMP_TH\r\n");
					#endif	        

					for(ucCnt=0; ucCnt < pstCANrxmsg->DLC; ucCnt++)
						{
							SplitByte.ucChar[ucCnt] = pstCANrxmsg->Data[ucCnt+1];
						}			
					if ( LOW_TH == pstCANrxmsg->Data[0] )
					{
						stConfig_Value.fConfigPipeTempLTH = SplitByte.fFloat;
						#ifdef DBG_CAN     
							memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
							sprintf(rgcTempCAN,"CONFIG_PIPE_TEMP_TH LOW: %0.2f", stConfig_Value.fConfigPipeTempLTH);	
							DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
						#endif
						if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
						{
							stConfig_Value.fConfigPipeTempLTH= fnMetricToImperial_Conv(stConfig_Value.fConfigPipeTempLTH, TEMPERATURE_VALUE);
						}
					} 					
					if ( HIGH_TH == pstCANrxmsg->Data[0] )
					{
						stConfig_Value.fConfigPipeTempHTH= SplitByte.fFloat;
						#ifdef DBG_CAN     
							memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
							sprintf(rgcTempCAN,"CONFIG_PIPE_TEMP_TH HIGH: %0.2f", stConfig_Value.fConfigPipeTempHTH);	
							DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
						#endif
						if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
						{
							stConfig_Value.fConfigPipeTempHTH= fnMetricToImperial_Conv(stConfig_Value.fConfigPipeTempHTH, TEMPERATURE_VALUE);
						}
					}          
			 }break;         
			case CONFIG_SPRAY_COUNT:							/*Number of Rotations for Spray*/
				{
					#ifdef DBG_CAN      
						DEBUG_PRINTF("CONFIG_SPRAY_COUNT\r\n");
					#endif	        	

					stConfig_Value.uiConfigSprayCount = pstCANrxmsg->Data[0];
					#ifdef DBG_CAN     
						memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
						sprintf(rgcTempCAN,"CONFIG_SPRAY_COUNT: %d", stConfig_Value.uiConfigSprayCount );	
						DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
					#endif				
			 }break;    
			case CONFIG_PRIME_TIME:								/*Prime process time*/
				{
					#ifdef DBG_CAN      
						DEBUG_PRINTF("CONFIG_PRIME_TIME\r\n");
					#endif	        

					for(ucCnt=0; ucCnt < pstCANrxmsg->DLC; ucCnt++)
						{
							SplitByte.ucChar[ucCnt] = pstCANrxmsg->Data[ucCnt];
						}			

					stConfig_Value.fConfigPrimeTime = SplitByte.fFloat;
					#ifdef DBG_CAN     
						memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
						sprintf(rgcTempCAN,"CONFIG_PRIME_TIME: %0.2f", stConfig_Value.fConfigPrimeTime );	
						DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
					#endif								
			 }break;
			case CONFIG_OPMM_GUN_DELAY:						/*Gun delay time*/
				{
					#ifdef DBG_CAN      
						DEBUG_PRINTF("CONFIG_OPMM_GUN_DELAY\r\n");
					#endif	        

					for(ucCnt=0; ucCnt < pstCANrxmsg->DLC; ucCnt++)
						{
							SplitByte.ucChar[ucCnt] = pstCANrxmsg->Data[ucCnt];
						}			

								 
					
					stConfig_Value.fConfigGunDelayTime = SplitByte.fFloat * 1000;
					#ifdef DBG_CAN     
						memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
						sprintf(rgcTempCAN,"CONFIG_OPMM_GUN_DELAY: %0.2f", stConfig_Value.fConfigGunDelayTime );	
						DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
					#endif           
			 }break;
			case CONFIG_OPMM_SPRAY_OVERLAP_TIME:	/*Spray Overlap time*/
				{
					#ifdef DBG_CAN      
						DEBUG_PRINTF("CONFIG_OPMM_SPRAY_OVERLAP_TIME\r\n");
					#endif	        

					for(ucCnt=0; ucCnt < pstCANrxmsg->DLC; ucCnt++)
						{
							SplitByte.ucChar[ucCnt] = pstCANrxmsg->Data[ucCnt];
						}			

					stConfig_Value.fConfigSprayOverlapTime = SplitByte.fFloat * 1000;
					#ifdef DBG_CAN     
						memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
						sprintf(rgcTempCAN,"CONFIG_OPMM_SPRAY_OVERLAP_TIME: %0.2f", stConfig_Value.fConfigSprayOverlapTime );	
						DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
					#endif
			 }break;
			case CONFIG_OPMM_REVERSE_DELAY:				/*Reverse delay time*/
				{
					#ifdef DBG_CAN      
						DEBUG_PRINTF("CONFIG_OPMM_REVERSE_DELAY\r\n");
					#endif	        

					for(ucCnt=0; ucCnt < pstCANrxmsg->DLC; ucCnt++)
						{
							SplitByte.ucChar[ucCnt] = pstCANrxmsg->Data[ucCnt];
						}			

					stConfig_Value.fConfigReverseDelayTime = SplitByte.fFloat * 1000;
					#ifdef DBG_CAN     
						memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
						sprintf(rgcTempCAN,"CONFIG_OPMM_REVERSE_DELAY: %0.2f", stConfig_Value.fConfigReverseDelayTime);	
						DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
					#endif				      
			 }break;
			case CONFIG_OPMM_FLUSH_TIME:					/*Flush process time*/
				{
					#ifdef DBG_CAN      
						DEBUG_PRINTF("CONFIG_OPMM_FLUSH_TIME\r\n");
					#endif	        

					for(ucCnt=0; ucCnt < pstCANrxmsg->DLC; ucCnt++)
						{
							SplitByte.ucChar[ucCnt] = pstCANrxmsg->Data[ucCnt];
						}			

					stConfig_Value.fConfigFlushTime = SplitByte.fFloat;
					#ifdef DBG_CAN     
						memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
						sprintf(rgcTempCAN,"CONFIG_OPMM_FLUSH_TIME: %0.2f", stConfig_Value.fConfigFlushTime );	
						DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
					#endif				           
			 }break;
			case CONFIG_DCLS_HOME_TIME:						/*DCLS and HLS time*/
				{
				/*splitting the data received from the CAN */
				for(ucCnt = 1; ucCnt < pstCANrxmsg->DLC; ucCnt++)
				{
					SplitByte.ucChar[ucCnt - 1] = pstCANrxmsg->Data[ucCnt];
				}
				
				if(DCLS_TIMER == pstCANrxmsg->Data[0])
				{
					stConfig_Value.fConfigDCLSHomeTime = SplitByte.fFloat;
					#ifdef DBG_CAN      
						DEBUG_PRINTF("CONFIG DCLS-HOME reach time= %d  \r\n", stConfig_Value.fConfigDCLSHomeTime);
					#endif
				}
				if(HLS_TIMER == pstCANrxmsg->Data[0])
				{
					stConfig_Value.fConfigHomeDCLSTime = SplitByte.fFloat;
					#ifdef DBG_CAN      
						DEBUG_PRINTF("CONFIG HOME-DCLS reach time = %d \r\n", stConfig_Value.fConfigHomeDCLSTime);
					#endif
				}
				pstCANrxmsg->Data[0] = 0;
			}break;
			case CONFIG_GUN_POSITION_CHANGE_TIME:	/*gun position switching time*/
				{
				/*splitting the data received from the CAN */
				for(ucCnt=0; ucCnt < pstCANrxmsg->DLC; ucCnt++)
				{
					SplitByte.ucChar[ucCnt] = pstCANrxmsg->Data[ucCnt];
				}
				stConfig_Value.fGunPosSwTime = SplitByte.fFloat;
				pstCANrxmsg->Data[0] = 0;
				#ifdef DBG_CAN      
					DEBUG_PRINTF("Gun Pos switching CONFIG Para = %d  \r\n", stConfig_Value.fGunPosSwTime);
				#endif
			}break;
			case CONFIG_SPRAY_VALVE_OPEN_TIME_VBAT:/*delay to open spray gun valve*/
				{	
				/*splitting the data received from the CAN */
				for(ucCnt = 0; ucCnt < pstCANrxmsg->DLC; ucCnt++)
				{
					SplitByte.ucChar[ucCnt] = pstCANrxmsg->Data[ucCnt];
				}
			
				stConfig_Value.fConfigTimebeforeONSprayValve = SplitByte.fFloat;
				#ifdef DBG_CAN      
						DEBUG_PRINTF("Spray Gun Open Time CONFIG Para = %d  \r\n", stConfig_Value.fConfigTimebeforeONSprayValve);
				#endif
			}break;
			case CONFIG_FLUSH_WARNING_TIME:				/*Flush warning time*/
			{
				#ifdef DBG_CAN      
					DEBUG_PRINTF("CONFIG_FLUSH_WARNING_TIME\r\n");
				#endif	        

				for(ucCnt=0; ucCnt < pstCANrxmsg->DLC; ucCnt++)
				{
					SplitByte.ucChar[ucCnt] = pstCANrxmsg->Data[ucCnt];
				}

				stConfig_Value.fConfigFlushWarnTime = SplitByte.fFloat;
				#ifdef DBG_CAN     
					memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
					sprintf(rgcTempCAN,"CONFIG_FLUSH_WARNING_TIME: %0.2f", stConfig_Value.fConfigFlushWarnTime );	
					DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
				#endif
			}break;
			
			case CONFIG_VCM_STALL_TH	:						/*Stall test threshold for Upstroke and 
																							Downstroke*/
				{
					#ifdef DBG_CAN      
						DEBUG_PRINTF("CONFIG_MFM_STALL_TH\r\n");
					#endif	        
		
					for(ucCnt=1; ucCnt < pstCANrxmsg->DLC; ucCnt++)
					{
						SplitByte.ucChar[ucCnt - 1] = pstCANrxmsg->Data[ucCnt];
					}
						
					if ( DOWNSTROKE_TH == pstCANrxmsg->Data[0] )
					{
					stConfig_Value.iConfigDownStrokeTH = SplitByte.iData;
						#ifdef DBG_CAN     
							memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
							sprintf(rgcTempCAN,"CONFIG_MFM_STALL_TH DOWNSTROKE: %0.2f", stConfig_Value.iConfigDownStrokeTH );	
							DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
						#endif				
					}
					if ( UPSTROKE_TH == pstCANrxmsg->Data[0] )
					{
						stConfig_Value.iConfigUpStrokeTH = SplitByte.iData;
						#ifdef DBG_CAN     
							memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
							sprintf(rgcTempCAN,"CONFIG_MFM_STALL_TH UPSTROKE: %0.2f", stConfig_Value.iConfigUpStrokeTH );	
							DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
						#endif
					}       
			 }break;
			case CONFIG_VCM_PGAUGE1_TH:						/*Pressure gauge 1 High and Low Threshold*/
				{
					#ifdef DBG_CAN      
						DEBUG_PRINTF("CONFIG_MFM_PGAUGE1_TH\r\n");
					#endif	        

					for(ucCnt=0; ucCnt < pstCANrxmsg->DLC; ucCnt++)
					{
						SplitByte.ucChar[ucCnt] = pstCANrxmsg->Data[ucCnt+1];
					}			
							 
					if ( LOW_TH == pstCANrxmsg->Data[0] )
					{
						stConfig_Value.fConfigPressureGauge1LTH = SplitByte.fFloat;
						#ifdef DBG_CAN     
							memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
							sprintf(rgcTempCAN,"CONFIG_MFM_PGAUGE1_TH LOW: %0.2f", stConfig_Value.fConfigPressureGauge1LTH );	
							DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
						#endif
						if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
						{
							stConfig_Value.fConfigPressureGauge1LTH= fnMetricToImperial_Conv(stConfig_Value.fConfigPressureGauge1LTH, PRESSURE_VALUE);
						}
					}
					if ( HIGH_TH == pstCANrxmsg->Data[0] )
					{
						stConfig_Value.fConfigPressureGauge1HTH= SplitByte.fFloat;
						#ifdef DBG_CAN     
							memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
							sprintf(rgcTempCAN,"CONFIG_MFM_PGAUGE1_TH HIGH: %0.2f", stConfig_Value.fConfigPressureGauge1HTH );	
							DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
						#endif
						if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
						{
							stConfig_Value.fConfigPressureGauge1HTH= fnMetricToImperial_Conv(stConfig_Value.fConfigPressureGauge1HTH, PRESSURE_VALUE);
						}
					} 
			 }break;
			case CONFIG_VCM_PGAUGE2_TH:						/*Pressure gauge 2 High and Low Threshold*/			
				{
					#ifdef DBG_CAN      
						DEBUG_PRINTF("CONFIG_MFM_PGAUGE2_TH\r\n");
					#endif	        

					for(ucCnt=0; ucCnt < pstCANrxmsg->DLC; ucCnt++)
					{
						SplitByte.ucChar[ucCnt] = pstCANrxmsg->Data[ucCnt+1];
					}										 
					if ( LOW_TH == pstCANrxmsg->Data[0] )
					{
						stConfig_Value.fConfigPressureGauge2LTH = SplitByte.fFloat;
						#ifdef DBG_CAN     
							memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
							sprintf(rgcTempCAN,"CONFIG_MFM_PGAUGE2_TH LOW: %0.2f", stConfig_Value.fConfigPressureGauge2LTH );	
							DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
						#endif		
						if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
						{
							stConfig_Value.fConfigPressureGauge2LTH= fnMetricToImperial_Conv(stConfig_Value.fConfigPressureGauge2LTH, PRESSURE_VALUE);
						}						
					} 					
					if ( HIGH_TH == pstCANrxmsg->Data[0] )
					{
						stConfig_Value.fConfigPressureGauge2HTH= SplitByte.fFloat;
						#ifdef DBG_CAN     
							memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
							sprintf(rgcTempCAN,"CONFIG_MFM_PGAUGE2_TH HIGH: %0.2f", stConfig_Value.fConfigPressureGauge2HTH );	
							DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
						#endif
						if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
						{
							stConfig_Value.fConfigPressureGauge2HTH= fnMetricToImperial_Conv(stConfig_Value.fConfigPressureGauge2HTH, PRESSURE_VALUE);
						}
					}           
			 }break;
			case CONFIG_VCM_PGAUGE3_TH:						/*Pressure gauge 3 High and Low Threshold*/
				{
				#ifdef DBG_CAN      
				 DEBUG_PRINTF("CONFIG_MFM_PGAUGE3_TH\r\n");
				#endif	        

				for(ucCnt=0; ucCnt < pstCANrxmsg->DLC; ucCnt++)
					{
						SplitByte.ucChar[ucCnt] = pstCANrxmsg->Data[ucCnt+1];
					}			
 
				if ( LOW_TH == pstCANrxmsg->Data[0] )
				{
					stConfig_Value.fConfigPressureGauge3LTH= SplitByte.fFloat;
					#ifdef DBG_CAN     
						memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
						sprintf(rgcTempCAN,"CONFIG_MFM_PGAUGE3_TH LOW: %0.2f", stConfig_Value.fConfigPressureGauge3LTH );	
						DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
					#endif
					if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
					{
						stConfig_Value.fConfigPressureGauge3LTH= fnMetricToImperial_Conv(stConfig_Value.fConfigPressureGauge3LTH, PRESSURE_VALUE);
					}
				} 					
				if ( HIGH_TH == pstCANrxmsg->Data[0] )
				{
					stConfig_Value.fConfigPressureGauge3HTH= SplitByte.fFloat;
					#ifdef DBG_CAN     
						memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
						sprintf(rgcTempCAN,"CONFIG_MFM_PGAUGE3_TH HIGH: %0.2f", stConfig_Value.fConfigPressureGauge3HTH);	
						DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
					#endif
					if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
					{
						stConfig_Value.fConfigPressureGauge3HTH = fnMetricToImperial_Conv(stConfig_Value.fConfigPressureGauge3HTH, PRESSURE_VALUE);
					}					
				}           
			 }break;
			case CONFIG_CYLINDER_SURFACE_AREA:		/*Cylinder surface area*/
				{
					#ifdef DBG_CAN      
					 DEBUG_PRINTF("CONFIG_CYLINDER_AREA\r\n");
					#endif	        

					for(ucCnt=0; ucCnt < pstCANrxmsg->DLC; ucCnt++)
						{
							SplitByte.ucChar[ucCnt] = pstCANrxmsg->Data[ucCnt+1];
						}
	 
					if ( LEG_A == pstCANrxmsg->Data[0] )
					{
						/*change21*/
					stConfig_Value.fConfigAreaofCylinderA = SplitByte.fFloat;
						#ifdef DBG_CAN     
							memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
							sprintf(rgcTempCAN,"CONFIG_CYLINDER_AREA A: %0.2f", stConfig_Value.fConfigAreaofCylinderA );	
							DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
						#endif	
						if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
						{
							stConfig_Value.fConfigAreaofCylinderA = fnMetricToImperial_Conv(stConfig_Value.fConfigAreaofCylinderA, MM2_TO_INCH2);
						}
					}
					if ( LEG_B == pstCANrxmsg->Data[0] )
					{
						/*change21*/
						stConfig_Value.fConfigAreaofCylinderB= SplitByte.fFloat;
						#ifdef DBG_CAN     
							memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
							sprintf(rgcTempCAN,"CONFIG_CYLINDER_AREA B: %0.2f", stConfig_Value.fConfigAreaofCylinderB );	
							DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
						#endif		
						if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
						{
							stConfig_Value.fConfigAreaofCylinderB = fnMetricToImperial_Conv(stConfig_Value.fConfigAreaofCylinderB, MM2_TO_INCH2);
						}
					}
					if ( LEG_C == pstCANrxmsg->Data[0] )
					{/*change21*/
						stConfig_Value.fConfigAreaofCylinderC= SplitByte.fFloat;
						#ifdef DBG_CAN     
							memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
							sprintf(rgcTempCAN,"CONFIG_CYLINDER_AREA C: %0.2f", stConfig_Value.fConfigAreaofCylinderC );	
							DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
						#endif
						if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
						{
							stConfig_Value.fConfigAreaofCylinderC = fnMetricToImperial_Conv(stConfig_Value.fConfigAreaofCylinderC, MM2_TO_INCH2);
						}
					}
			 }break;
			case CONFIG_NUMBER_OF_CYLINDER:				/*NUmber of cylinders*/
				{
					#ifdef DBG_CAN      
					 DEBUG_PRINTF("CONFIG_NUMBER_OF_CYLINDER\r\n");
					#endif	        

					stConfig_Value.uiConfigCylinderCount = pstCANrxmsg->Data[0];

					#ifdef DBG_CAN     
						memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
						sprintf(rgcTempCAN,"CONFIG_NUMBER_OF_CYLINDER: %d", stConfig_Value.uiConfigCylinderCount );	
						DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
					#endif				           
			 }break;
			 case CONFIG_TIME_BEFORE_STALL:				/*delay time to take 1st reading in stall 
																							test*/
				{
				#ifdef DBG_CAN      
					DEBUG_PRINTF("CONFIG_TIME_BEFORE_STALL\r\n");
				#endif	        
				for(ucCnt=0; ucCnt < pstCANrxmsg->DLC; ucCnt++)
				{
					SplitByte.ucChar[ucCnt] = pstCANrxmsg->Data[ucCnt];
				}			

				stConfig_Value.fConfigTimeBeforeStall = SplitByte.fFloat;
				#ifdef DBG_CAN     
					memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
					sprintf(rgcTempCAN,"CONFIG_TIME_BEFORE_STALL: %0.2f", stConfig_Value.fConfigTimeBeforeStall );	
					DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
				#endif				           
			 }break;
			case CONFIG_TIME_BEFORE_COMPARE_STALL:/*delay time to take seconf reading in 
																								stall test*/
				{
					#ifdef DBG_CAN      
						DEBUG_PRINTF("CONFIG_TIME_BEFORE_COMPARE_STALL\r\n");
					#endif	        
					for(ucCnt=0; ucCnt < pstCANrxmsg->DLC; ucCnt++)
					{
						SplitByte.ucChar[ucCnt] = pstCANrxmsg->Data[ucCnt];
					}			

					stConfig_Value.fConfigTimeCompareStall = SplitByte.fFloat;
					#ifdef DBG_CAN     
						memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
						sprintf(rgcTempCAN,"CONFIG_TIME_BEFORE_COMPARE_STALL: %0.2f", stConfig_Value.fConfigTimeCompareStall);	
						DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
					#endif				          
			 }break;
			 
			 
			case CONFIG_BASE_TEMP_SETTING:				/*Base material tank Set temperature(antifreez)*/
				{
					#ifdef DBG_CAN      
					 DEBUG_PRINTF("CONFIG_BASE_TEMP_SETTING(ANTIFREEZ)\r\n");
					#endif	        

					for(ucCnt=0; ucCnt < pstCANrxmsg->DLC; ucCnt++)
					{
						SplitByte.ucChar[ucCnt] = pstCANrxmsg->Data[ucCnt + 1];
					}			

					if (BIT_0 == (pstCANrxmsg->Data[0] & BIT_0))
					{
						stConfig_Value.fConfigAntiFrzTempSet = SplitByte.fFloat;
						
						#ifdef DBG_CAN     
							memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
							sprintf(rgcTempCAN,"fConfigAntiFrzTempSet: %0.2f", stConfig_Value.fConfigAntiFrzTempSet );	
							DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
						#endif
						
						if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
						{
							stConfig_Value.fConfigAntiFrzTempSet = fnMetricToImperial_Conv(stConfig_Value.fConfigAntiFrzTempSet, TEMPERATURE_VALUE);
						}
					}
					if (BIT_1 == (pstCANrxmsg->Data[0] & BIT_1))
					{
						stConfig_Value.fConfigAntiFrzTempLTH = SplitByte.fFloat;
						
						#ifdef DBG_CAN     
							memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
							sprintf(rgcTempCAN,"fConfigAntiFrzTempLTH: %0.2f", stConfig_Value.fConfigAntiFrzTempLTH );	
							DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
						#endif
						
						if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
						{
							stConfig_Value.fConfigAntiFrzTempLTH = fnMetricToImperial_Conv(stConfig_Value.fConfigAntiFrzTempLTH, TEMPERATURE_VALUE);
						}
					}
					if (BIT_2 == (pstCANrxmsg->Data[0] & BIT_2))
					{
						stConfig_Value.fConfigAntiFrzTempHTH = SplitByte.fFloat;
						
						#ifdef DBG_CAN     
							memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
							sprintf(rgcTempCAN,"fConfigAntiFrzTempHTH: %0.2f", stConfig_Value.fConfigAntiFrzTempHTH );	
							DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
						#endif
						
						if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
						{
							stConfig_Value.fConfigAntiFrzTempHTH = fnMetricToImperial_Conv(stConfig_Value.fConfigAntiFrzTempHTH, TEMPERATURE_VALUE);
						}
					}
					if (BIT_3 == (pstCANrxmsg->Data[0] & BIT_3))
					{
						stConfig_Value.fConfigHysAntiFrzPos = SplitByte.fFloat;
						
						#ifdef DBG_CAN     
							memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
							sprintf(rgcTempCAN,"fConfigHysAntiFrzPos: %0.2f", stConfig_Value.fConfigHysAntiFrzPos );	
							DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
						#endif
						
						if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
						{
							stConfig_Value.fConfigHysAntiFrzPos = fnMetricToImperial_Conv(stConfig_Value.fConfigHysAntiFrzPos, TEMPERATURE_VALUE);
						}
					}
					if (BIT_4 == (pstCANrxmsg->Data[0] & BIT_4))
					{
						stConfig_Value.fConfigHysAntiFrzNeg = SplitByte.fFloat;
						
						#ifdef DBG_CAN     
							memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
							sprintf(rgcTempCAN,"fConfigHysAntiFrzNeg: %0.2f", stConfig_Value.fConfigHysAntiFrzNeg );	
							DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
						#endif
						
						if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
						{
							stConfig_Value.fConfigHysAntiFrzNeg = fnMetricToImperial_Conv(stConfig_Value.fConfigHysAntiFrzNeg, TEMPERATURE_VALUE);
						}
					}
			 }break;
			case CONFIG_INLINE_BASE_TEMP_SETTING:			/*Inline base material set temperature*/
				{
					#ifdef DBG_CAN      
					 DEBUG_PRINTF("CONFIG_INLINE_BASE_TEMP_SETTING\r\n");
					#endif	        

					for(ucCnt=0; ucCnt < pstCANrxmsg->DLC; ucCnt++)
					{
						SplitByte.ucChar[ucCnt] = pstCANrxmsg->Data[ucCnt + 1];
					}			

					if (BIT_0 == (pstCANrxmsg->Data[0] & BIT_0))
					{
						stConfig_Value.fConfigInlineBaseTempSet = SplitByte.fFloat;
						
						#ifdef DBG_CAN     
							memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
							sprintf(rgcTempCAN,"fConfigInlineBaseTempSet: %0.2f", stConfig_Value.fConfigInlineBaseTempSet );	
							DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
						#endif
						
						if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
						{
							stConfig_Value.fConfigInlineBaseTempSet = fnMetricToImperial_Conv(stConfig_Value.fConfigInlineBaseTempSet, TEMPERATURE_VALUE);
						}
					}
					if (BIT_1 == (pstCANrxmsg->Data[0] & BIT_1))
					{
						stConfig_Value.fConfigInlineBaseTempLTH = SplitByte.fFloat;
						
						#ifdef DBG_CAN     
							memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
							sprintf(rgcTempCAN,"fConfigInlineBaseTempLTH: %0.2f", stConfig_Value.fConfigInlineBaseTempLTH );	
							DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
						#endif
						
						if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
						{
							stConfig_Value.fConfigInlineBaseTempLTH = fnMetricToImperial_Conv(stConfig_Value.fConfigInlineBaseTempLTH, TEMPERATURE_VALUE);
						}
					}
					if (BIT_2 == (pstCANrxmsg->Data[0] & BIT_2))
					{
						stConfig_Value.fConfigInlineBaseTempHTH = SplitByte.fFloat;
						
						#ifdef DBG_CAN     
							memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
							sprintf(rgcTempCAN,"fConfigInlineBaseTempHTH: %0.2f", stConfig_Value.fConfigInlineBaseTempHTH );	
							DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
						#endif
						
						if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
						{
							stConfig_Value.fConfigInlineBaseTempHTH = fnMetricToImperial_Conv(stConfig_Value.fConfigInlineBaseTempHTH, TEMPERATURE_VALUE);
						}
					}
					if (BIT_3 == (pstCANrxmsg->Data[0] & BIT_3))
					{
						stConfig_Value.fConfigHysInlineBasePos = SplitByte.fFloat;
						
						#ifdef DBG_CAN     
							memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
							sprintf(rgcTempCAN,"fConfigHysInlineBasePos: %0.2f", stConfig_Value.fConfigHysInlineBasePos );	
							DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
						#endif
						
						if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
						{
							stConfig_Value.fConfigHysInlineBasePos = fnMetricToImperial_Conv(stConfig_Value.fConfigHysInlineBasePos, TEMPERATURE_VALUE);
						}
					}
					if (BIT_4 == (pstCANrxmsg->Data[0] & BIT_4))
					{
						stConfig_Value.fConfigHysInlineBaseNeg = SplitByte.fFloat;
						
						#ifdef DBG_CAN     
							memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
							sprintf(rgcTempCAN,"fConfigHysInlineBaseNeg: %0.2f", stConfig_Value.fConfigHysInlineBaseNeg );	
							DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
						#endif
						
						if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
						{
							stConfig_Value.fConfigHysInlineBaseNeg = fnMetricToImperial_Conv(stConfig_Value.fConfigHysInlineBaseNeg, TEMPERATURE_VALUE);
						}
					}
			 }break;
			case CONFIG_INLINE_HARD_TEMP_SETTING:			/*Inline Hardener material set temperature*/
				{
					#ifdef DBG_CAN      
					 DEBUG_PRINTF("CONFIG_INLINE_HARD_TEMP_SETTING\r\n");
					#endif	        

					for(ucCnt=0; ucCnt < pstCANrxmsg->DLC; ucCnt++)
					{
						SplitByte.ucChar[ucCnt] = pstCANrxmsg->Data[ucCnt + 1];
					}			

					if (BIT_0 == (pstCANrxmsg->Data[0] & BIT_0))
					{
						stConfig_Value.fConfigInlineHardTempSet = SplitByte.fFloat;
						
						#ifdef DBG_CAN     
							memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
							sprintf(rgcTempCAN,"fConfigInlineHardTempSet: %0.2f", stConfig_Value.fConfigInlineHardTempSet );	
							DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
						#endif
						
						if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
						{
							stConfig_Value.fConfigInlineHardTempSet = fnMetricToImperial_Conv(stConfig_Value.fConfigInlineHardTempSet, TEMPERATURE_VALUE);
						}
					}
					if (BIT_1 == (pstCANrxmsg->Data[0] & BIT_1))
					{
						stConfig_Value.fConfigInlineHardTempLTH = SplitByte.fFloat;
						
						#ifdef DBG_CAN     
							memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
							sprintf(rgcTempCAN,"fConfigInlineHardTempLTH: %0.2f", stConfig_Value.fConfigInlineHardTempLTH );	
							DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
						#endif
						
						if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
						{
							stConfig_Value.fConfigInlineHardTempLTH = fnMetricToImperial_Conv(stConfig_Value.fConfigInlineHardTempLTH, TEMPERATURE_VALUE);
						}
					}
					if (BIT_2 == (pstCANrxmsg->Data[0] & BIT_2))
					{
						stConfig_Value.fConfigInlineHardTempHTH = SplitByte.fFloat;
						
						#ifdef DBG_CAN     
							memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
							sprintf(rgcTempCAN,"fConfigInlineHardTempHTH: %0.2f", stConfig_Value.fConfigInlineHardTempHTH );	
							DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
						#endif
						
						if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
						{
							stConfig_Value.fConfigInlineHardTempHTH = fnMetricToImperial_Conv(stConfig_Value.fConfigInlineHardTempHTH, TEMPERATURE_VALUE);
						}
					}
					if (BIT_3 == (pstCANrxmsg->Data[0] & BIT_3))
					{
						stConfig_Value.fConfigInlineHysHardPos = SplitByte.fFloat;
						
						#ifdef DBG_CAN     
							memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
							sprintf(rgcTempCAN,"fConfigInlineHysHardPos: %0.2f", stConfig_Value.fConfigInlineHysHardPos );	
							DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
						#endif
						
						if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
						{
							stConfig_Value.fConfigInlineHysHardPos = fnMetricToImperial_Conv(stConfig_Value.fConfigInlineHysHardPos, TEMPERATURE_VALUE);
						}
					}
					if (BIT_4 == (pstCANrxmsg->Data[0] & BIT_4))
					{
						stConfig_Value.fConfigInlineHysHardNeg = SplitByte.fFloat;
						
						#ifdef DBG_CAN     
							memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
							sprintf(rgcTempCAN,"fConfigInlineHysHardNeg: %0.2f", stConfig_Value.fConfigInlineHysHardNeg );	
							DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
						#endif
						
						if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
						{
							stConfig_Value.fConfigInlineHysHardNeg = fnMetricToImperial_Conv(stConfig_Value.fConfigInlineHysHardNeg, TEMPERATURE_VALUE);
						}
					}
			 }break;
			case CONFIG_BASE_LEVEL_SETTING:						/*Base material liquid level threshold*/
				{
					#ifdef DBG_CAN      
					 DEBUG_PRINTF("CONFIG_BHCM_BTANK_LIQUID_TH\r\n");
					#endif	        

					for(ucCnt=0; ucCnt < pstCANrxmsg->DLC; ucCnt++)
						{
							SplitByte.ucChar[ucCnt] = pstCANrxmsg->Data[ucCnt+1];
						}			

					if ( LOW_TH == pstCANrxmsg->Data[0] )
					{
						stConfig_Value.fConfigBaseLiquidLevelLTH= SplitByte.fFloat;
						#ifdef DBG_CAN     
							memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
							sprintf(rgcTempCAN,"CONFIG_BHCM_BTANK_LIQUID_TH LOW: %0.2f", stConfig_Value.fConfigBaseLiquidLevelLTH);	
							DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
						#endif		
						if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
						{
							stConfig_Value.fConfigBaseLiquidLevelLTH = fnMetricToImperial_Conv(stConfig_Value.fConfigBaseLiquidLevelLTH, MM_TO_INCH);
						}						
					}					
					if ( HIGH_TH == pstCANrxmsg->Data[0] )
					{
						stConfig_Value.fConfigBaseLiquidLevelHTH= SplitByte.fFloat;
						#ifdef DBG_CAN     
							memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
							sprintf(rgcTempCAN,"CONFIG_BHCM_BTANK_LIQUID_TH HIGH: %0.2f", stConfig_Value.fConfigBaseLiquidLevelHTH );	
							DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
						#endif
						if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
						{
							stConfig_Value.fConfigBaseLiquidLevelHTH = fnMetricToImperial_Conv(stConfig_Value.fConfigBaseLiquidLevelHTH, MM_TO_INCH);
						}
					}           
			 }break;
			case CONFIG_HARD_LEVEL_SETTING:						/*HHCM liquid level threshold*/
				{
					#ifdef DBG_CAN      
					 DEBUG_PRINTF("CONFIG_HHCM_BTANK_LIQUID_TH\r\n");
					#endif	        

					for(ucCnt=0; ucCnt < pstCANrxmsg->DLC; ucCnt++)
						{
							SplitByte.ucChar[ucCnt] = pstCANrxmsg->Data[ucCnt+1];
						}			

					if ( LOW_TH == pstCANrxmsg->Data[0] )
					{
						stConfig_Value.fConfigHardLiquidLevelLTH= SplitByte.fFloat;
						#ifdef DBG_CAN     
							memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
							sprintf(rgcTempCAN,"CONFIG_BHCM_BTANK_LIQUID_TH LOW: %0.2f", stConfig_Value.fConfigHardLiquidLevelLTH);	
							DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
						#endif				
						if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
						{
							stConfig_Value.fConfigHardLiquidLevelLTH = fnMetricToImperial_Conv(stConfig_Value.fConfigHardLiquidLevelLTH, MM_TO_INCH);
						}
					}					
					if ( HIGH_TH == pstCANrxmsg->Data[0] )
					{
						stConfig_Value.fConfigHardLiquidLevelHTH= SplitByte.fFloat;
						#ifdef DBG_CAN     
							memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
							sprintf(rgcTempCAN,"CONFIG_BHCM_BTANK_LIQUID_TH HIGH: %0.2f", stConfig_Value.fConfigHardLiquidLevelHTH );	
							DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
						#endif
						if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
						{
							stConfig_Value.fConfigHardLiquidLevelHTH = fnMetricToImperial_Conv(stConfig_Value.fConfigHardLiquidLevelHTH, MM_TO_INCH);
						}
					}
				}break;
			case CONFIG_TANK_HEIGHT:									/*Base and hardener material tank height*/
				{
					#ifdef DBG_CAN      
						DEBUG_PRINTF("CONFIG_LIQUID_TANK_HEIGHT\r\n");
					#endif	        

					for(ucCnt=0; ucCnt < pstCANrxmsg->DLC; ucCnt++)
					{
						SplitByte.ucChar[ucCnt] = pstCANrxmsg->Data[ucCnt+1];
					}			

					if ( BASE_TANK == pstCANrxmsg->Data[0] )
					{
						stConfig_Value.fConfigBaseTankHeight = SplitByte.fFloat;
						#ifdef DBG_CAN     
							memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
							sprintf(rgcTempCAN,"CONFIG_LIQUID_TANK_HEIGHT FOR BASE: %0.2f", stConfig_Value.fConfigBaseTankHeight);	
							DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
						#endif
						if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
						{
							stConfig_Value.fConfigBaseTankHeight = fnMetricToImperial_Conv(stConfig_Value.fConfigBaseTankHeight, MM_TO_INCH);
						}
					}					
					if ( HARD_TANK == pstCANrxmsg->Data[0] )
					{
						stConfig_Value.fConfigHardTankHeight= SplitByte.fFloat;
						#ifdef DBG_CAN     
							memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
							sprintf(rgcTempCAN,"CONFIG_LIQUID_TANK_HEIGHT FOR HARDNER: %0.2f", stConfig_Value.fConfigHardTankHeight );	
							DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
						#endif
						if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
						{
							stConfig_Value.fConfigHardTankHeight = fnMetricToImperial_Conv(stConfig_Value.fConfigHardTankHeight, MM_TO_INCH);
						}
					}
				}break;
			case CONFIG_LL_SAMPLE_RATE:								/*liquid level calculation window size*/
				{
					stConfig_Value.uiConfigLLWindowSize = pstCANrxmsg->Data[0] * 100;
					
					#ifdef DBG_CAN     
						memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
						sprintf(rgcTempCAN,"uiConfigLLWindowSize FOR HARDNER: %d", stConfig_Value.uiConfigLLWindowSize );	
						DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
					#endif
				}break;
			case CONFIG_PGAUGE_REMAP:
				{
					#ifdef DBG_CAN      
					 DEBUG_PRINTF("CONFIG_PGAUGE_REMAP\r\n");
					#endif
					
					stConfig_Value.ucConfigConnPressureGuage1 = pstCANrxmsg->Data[0];
					stConfig_Value.ucConfigConnPressureGuage2 = pstCANrxmsg->Data[1];
					stConfig_Value.ucConfigConnPressureGuage3 = pstCANrxmsg->Data[2];
					
					
					stConfig_Value.ucConfigConnPressureGuage1Rev 	= stConfig_Value.ucConfigConnPressureGuage1;
					stConfig_Value.ucConfigConnPressureGuage2Rev 	= stConfig_Value.ucConfigConnPressureGuage2;
					stConfig_Value.ucConfigConnPressureGuage3Rev 	= stConfig_Value.ucConfigConnPressureGuage3;
				}break;
			case CONFIG_LT_REMAP:
				{
					#ifdef DBG_CAN      
					 DEBUG_PRINTF("CONFIG_LT_REMAP\r\n");
					#endif
					
					stConfig_Value.ucConfigConnLinearTransducer = pstCANrxmsg->Data[0];
					
					stConfig_Value.ucConfigConnLinearTransducerRev 	= stConfig_Value.ucConfigConnLinearTransducer;
				}break;
			case CONFIG_EE07_REMAP:
				{
					#ifdef DBG_CAN      
					 DEBUG_PRINTF("CONFIG_EE07_REMAP\r\n");
					#endif
					
					stConfig_Value.ucConfigConnInsideEE07 = pstCANrxmsg->Data[0];
					stConfig_Value.ucConfigConnOutsideEE07 = pstCANrxmsg->Data[1];
					
					stConfig_Value.ucConfigConnInsideEE07Rev 	= stConfig_Value.ucConfigConnInsideEE07;
					stConfig_Value.ucConfigConnOutsideEE07Rev = stConfig_Value.ucConfigConnOutsideEE07;
				}break;
			case CONFIG_STALL_VALVE_REMAP:
				{
					#ifdef DBG_CAN      
					 DEBUG_PRINTF("CONFIG_STALL_VALVE_REMAP\r\n");
					#endif
					
					stConfig_Value.ucConfigConnStallvalve = pstCANrxmsg->Data[0];
					stConfig_Value.ucConfigConnRecircvalve = pstCANrxmsg->Data[1]; /*change12*/
					
					stConfig_Value.ucConfigConnStallvalveRev 	= stConfig_Value.ucConfigConnStallvalve;
					stConfig_Value.ucConfigConnRecircvalveRev 	= stConfig_Value.ucConfigConnRecircvalve; /*change12*/
				}break;
			case CONFIG_THERMOCOUPLE_REMAP:
				{
					#ifdef DBG_CAN      
					 DEBUG_PRINTF("CONFIG_THERMOCOUPLE_REMAP\r\n");
					#endif
					
					stConfig_Value.ucConfig_ConnBaseInlineTher = pstCANrxmsg->Data[0];
					stConfig_Value.ucConfig_ConnHardInlineTher = pstCANrxmsg->Data[1];
					stConfig_Value.ucConfig_ConnAntifreezeTher = pstCANrxmsg->Data[2];
					stConfig_Value.ucConfig_ConnBasereturnlineTher = pstCANrxmsg->Data[3];
					
					stConfig_Value.ucConfig_ConnBaseInlineTherRev 		= stConfig_Value.ucConfig_ConnBaseInlineTher;
					stConfig_Value.ucConfig_ConnHardInlineTherRev 		= stConfig_Value.ucConfig_ConnHardInlineTher;
					stConfig_Value.ucConfig_ConnAntifreezeTherRev 		= stConfig_Value.ucConfig_ConnAntifreezeTher;
					stConfig_Value.ucConfig_ConnBasereturnlineTherRev = stConfig_Value.ucConfig_ConnBasereturnlineTher;
				}break;
			case CONFIG_SSR_REMAP:
				{
					#ifdef DBG_CAN      
					 DEBUG_PRINTF("CONFIG_SSR_REMAP\r\n");
					#endif
					
					stConfig_Value.ucConfig_ConnBaseinlineHeater = pstCANrxmsg->Data[0];
					stConfig_Value.ucConfig_ConnHardinlineHeater = pstCANrxmsg->Data[1];
					stConfig_Value.ucConfig_ConnAntifreezeHeater = pstCANrxmsg->Data[2];
					stConfig_Value.ucConfig_ConnHardHeater = pstCANrxmsg->Data[3];
					
					stConfig_Value.ucConfig_ConnBaseinlineHeaterRev = stConfig_Value.ucConfig_ConnBaseinlineHeater;
					stConfig_Value.ucConfig_ConnHardinlineHeaterRev = stConfig_Value.ucConfig_ConnHardinlineHeater;
					stConfig_Value.ucConfig_ConnAntifreezeHeaterRev = stConfig_Value.ucConfig_ConnAntifreezeHeater;
					stConfig_Value.ucConfig_ConnHardHeaterRev 			= stConfig_Value.ucConfig_ConnHardHeater;
				}break;
			case CONFIG_LL_SENSOR_REMAP:
				{
					#ifdef DBG_CAN      
					 DEBUG_PRINTF("CONFIG_LL_SENSOR_REMAP\r\n");
					#endif
					
					stConfig_Value.ucConfig_ConnBaseTankLL = pstCANrxmsg->Data[0];
					stConfig_Value.ucConfig_ConnHardTankLL = pstCANrxmsg->Data[1];
					
					stConfig_Value.ucConfig_ConnBaseTankLLRev = stConfig_Value.ucConfig_ConnBaseTankLL;
					stConfig_Value.ucConfig_ConnHardTankLLRev = stConfig_Value.ucConfig_ConnHardTankLL;
				}break;
			case CONFIG_OPMM_VALVE_REMAP:
				{
					#ifdef DBG_CAN      
					 DEBUG_PRINTF("CONFIG_OPMM_VALVE_REMAP\r\n");
					#endif
					
					stConfig_Value.ucConfigConnSprayvalve = pstCANrxmsg->Data[0];
					stConfig_Value.ucConfigConnGunposition = pstCANrxmsg->Data[1];
					stConfig_Value.ucConfigConnABvalve = pstCANrxmsg->Data[2];
					stConfig_Value.ucConfigConnSolventvalve = pstCANrxmsg->Data[3];
					stConfig_Value.ucConfigConnForwardvalve = pstCANrxmsg->Data[4];
					stConfig_Value.ucConfigConnBackwardvalve = pstCANrxmsg->Data[5];
					
					stConfig_Value.ucConfigConnSprayvalveRev 		= stConfig_Value.ucConfigConnSprayvalve;
					stConfig_Value.ucConfigConnGunpositionRev 	= stConfig_Value.ucConfigConnGunposition;
					stConfig_Value.ucConfigConnABvalveRev 			= stConfig_Value.ucConfigConnABvalve;
					stConfig_Value.ucConfigConnSolventvalveRev 	= stConfig_Value.ucConfigConnSolventvalve;
					stConfig_Value.ucConfigConnForwardvalveRev 	= stConfig_Value.ucConfigConnForwardvalve;
					stConfig_Value.ucConfigConnBackwardvalveRev = stConfig_Value.ucConfigConnBackwardvalve;
				}break;
			case CONFIG_VCM_PERIPHRAL_BYPASS:
				{
					#ifdef DBG_CAN      
					 DEBUG_PRINTF("CONFIG_VCM_PERIPHRAL_BYPASS\r\n");
					#endif
					
					stConfig_Value.uiConfigVCMBypass = pstCANrxmsg->Data[0];
					
					stConfig_Value.uiConfigVCMBypassRev = stConfig_Value.uiConfigVCMBypass;

				}break;
			case CONFIG_HCM_PERIPHRAL_BYPASS:
				{
					#ifdef DBG_CAN      
					 DEBUG_PRINTF("CONFIG_HCM_PERIPHRAL_BYPASS\r\n");
					#endif
					
					stConfig_Value.uiConfigHCMBypass = (pstCANrxmsg->Data[0])  | (pstCANrxmsg->Data[1] << 8);
					

					stConfig_Value.uiConfigHCMBypassRev = stConfig_Value.uiConfigHCMBypass;

				}break;
			case CONFIG_SCU_PERIPHRAL_BYPASS:
				{
					#ifdef DBG_CAN      
					 DEBUG_PRINTF("CONFIG_SCU_PERIPHRAL_BYPASS\r\n");
					#endif
					
					stConfig_Value.uiConfigSCUBypass = pstCANrxmsg->Data[0];
				}break;
			case CONFIG_OPMM_PERIPHRAL_BYPASS:
				{
					#ifdef DBG_CAN      
					 DEBUG_PRINTF("CONFIG_OPMM_PERIPHRAL_BYPASS\r\n");
					#endif
					
					stConfig_Value.uiConfigOPMMBypass = (pstCANrxmsg->Data[0])  | (pstCANrxmsg->Data[1] << 8);

					stConfig_Value.uiConfigOPMMBypassRev = stConfig_Value.uiConfigOPMMBypass;
				}break;
			case CONFIG_NON_CONTACT_TEMP_REMAP:
				{
					#ifdef DBG_CAN      
					 DEBUG_PRINTF("CONFIG_NON_CONTACT_TEMP_REMAP\r\n");
					#endif
					
					 stConfig_Value.ucConfigNCTempSensor1 = pstCANrxmsg->Data[0];
					 stConfig_Value.ucConfigNCTempSensor2 = pstCANrxmsg->Data[1];
					 stConfig_Value.ucConfigNCTempSensor3 = pstCANrxmsg->Data[2];
					 stConfig_Value.ucConfigNCTempSensor4 = pstCANrxmsg->Data[3];
					
					stConfig_Value.ucConfigNCTempSensor1Rev	= stConfig_Value.ucConfigNCTempSensor1;
					stConfig_Value.ucConfigNCTempSensor2Rev = stConfig_Value.ucConfigNCTempSensor2;
					stConfig_Value.ucConfigNCTempSensor3Rev = stConfig_Value.ucConfigNCTempSensor3;
					stConfig_Value.ucConfigNCTempSensor4Rev = stConfig_Value.ucConfigNCTempSensor4;
				}break;
			case CONFIG_TEMP_PRES_ACHIEVE_BYPASS:
				{
					if( BIT_0 == (pstCANrxmsg->Data[0] & BIT_0))
					{
						StatusFlag.stStatusFlag.bSCU_Temp_Bypass = TRUE;
					}else
					{
						StatusFlag.stStatusFlag.bSCU_Temp_Bypass = FALSE;
					}
					
					if( BIT_1 == (pstCANrxmsg->Data[0] & BIT_1))
					{
						StatusFlag.stStatusFlag.bSCU_Pressure_Bypass = TRUE;
					}else
					{
						StatusFlag.stStatusFlag.bSCU_Pressure_Bypass = FALSE;
					}
				}break;
			case CONFIG_OPMM_BUZZER_VOLUME:
			{
				stConfig_Value.ucBuzzerVolume = pstCANrxmsg->Data[0] / 10;
			}break;
			case CONFIG_SIGNATURE_BYTE:
			{
				SplitByte.cChar[0] = pstCANrxmsg->Data[0];
				SplitByte.cChar[1] = pstCANrxmsg->Data[1];
				stConfig_Value.uiSignature = SplitByte.iData;
			}break;
			/*3rdMarch*/
			case	CONFIG_BAT_KNEE_VOLTAGE:
			{
				#ifdef DBG_CAN      
				 DEBUG_PRINTF("CONFIG_BAT_KNEE_VOLTAGE\r\n");
				#endif	        

				for(ucCnt=0; ucCnt < pstCANrxmsg->DLC; ucCnt++)
				{
					SplitByte.ucChar[ucCnt] = pstCANrxmsg->Data[ucCnt];
				}			
				stConfig_Value.fOPMMBatKneeVoltage = SplitByte.fFloat;
				
				#ifdef DBG_CAN     
					memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
					sprintf(rgcTempCAN,"CONFIG_BAT_KNEE_VOLTAGE: %0.2f", stConfig_Value.fOPMMBatKneeVoltage);	
					DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
				#endif				
			}break;
			
			case CONFIG_TIME_TO_START_STALL:
			{
				#ifdef DBG_CAN      
				 DEBUG_PRINTF("CONFIG_TIME_TO_START_STALL\r\n");
				#endif	        

				for(ucCnt=0; ucCnt < pstCANrxmsg->DLC; ucCnt++)
				{
					SplitByte.ucChar[ucCnt] = pstCANrxmsg->Data[ucCnt];
				}			
				stConfig_Value.fTimerStartStallTest = SplitByte.fFloat;
				
				#ifdef DBG_CAN     
					memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
					sprintf(rgcTempCAN,"CONFIG_TIME_TO_START_STALL: %0.2f", stConfig_Value.fTimerStartStallTest);	
					DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
				#endif
			}break;
		
			case CONFIG_STOP_MOTOR_ROTATION_TIME:
			{
				#ifdef DBG_CAN      
				 DEBUG_PRINTF("CONFIG_STOP_MOTOR_ROTATION_TIME\r\n");
				#endif	        

				for(ucCnt=0; ucCnt < pstCANrxmsg->DLC; ucCnt++)
				{
					SplitByte.ucChar[ucCnt] = pstCANrxmsg->Data[ucCnt];
				}			
				stConfig_Value.fTimerSprayLastRotStop = SplitByte.fFloat;
				
				#ifdef DBG_CAN     
					memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
					sprintf(rgcTempCAN,"CONFIG_STOP_MOTOR_ROTATION_TIME: %0.2f", stConfig_Value.fTimerSprayLastRotStop);	
					DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
				#endif
			}break;
			case CONFIG_STALL_TEST_LT_RANGES:
			{
				#ifdef DBG_CAN      
				 DEBUG_PRINTF("CONFIG_STALL_TEST_LT_RANGES\r\n");
				#endif

				if(BIT_0 == pstCANrxmsg->Data[0])
				{
					stConfig_Value.fUpstrokeLimit = (float)pstCANrxmsg->Data[1];
					if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
					{
						stConfig_Value.fUpstrokeLimit = fnMetricToImperial_Conv( stConfig_Value.fUpstrokeLimit, MM_TO_INCH);
					}
				}
				else if(BIT_1 == pstCANrxmsg->Data[0])
				{
					stConfig_Value.fUpstrokeMin = (float)pstCANrxmsg->Data[1];
					if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
					{
						stConfig_Value.fUpstrokeMin = fnMetricToImperial_Conv( stConfig_Value.fUpstrokeMin, MM_TO_INCH);
					}
				}
				else if(BIT_2 == pstCANrxmsg->Data[0])
				{
					stConfig_Value.fUpstrokeMax = (float)pstCANrxmsg->Data[1];
					if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
					{
						stConfig_Value.fUpstrokeMax = fnMetricToImperial_Conv( stConfig_Value.fUpstrokeMax, MM_TO_INCH);
					}
				}
				else if(BIT_3 == pstCANrxmsg->Data[0])
				{
					stConfig_Value.fDownstrokeLimit = (float)pstCANrxmsg->Data[1];
					if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
					{
						stConfig_Value.fDownstrokeLimit = fnMetricToImperial_Conv( stConfig_Value.fDownstrokeLimit, MM_TO_INCH);
					}
				}
				else if(BIT_4 == pstCANrxmsg->Data[0])
				{
					stConfig_Value.fDownstrokeMin = (float)pstCANrxmsg->Data[1];
					if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
					{
						stConfig_Value.fDownstrokeMin = fnMetricToImperial_Conv( stConfig_Value.fDownstrokeMin, MM_TO_INCH);
					}
				}
				else if(BIT_5 == pstCANrxmsg->Data[0])
				{
					stConfig_Value.fDownstrokeMax = (float)pstCANrxmsg->Data[1];
					if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
					{
						stConfig_Value.fDownstrokeMax = fnMetricToImperial_Conv( stConfig_Value.fDownstrokeMax, MM_TO_INCH);
					}
				}
				
			}break;
			
			case CONFIG_IDEAL_PERIODIC_TIME:
			{
				#ifdef DBG_CAN      
				 DEBUG_PRINTF("CONFIG_IDEAL_PERIODIC_TIME\r\n");
				#endif	        

				for(ucCnt=0; ucCnt < pstCANrxmsg->DLC; ucCnt++)
				{
					SplitByte.ucChar[ucCnt] = pstCANrxmsg->Data[ucCnt];
				}			
				stConfig_Value.fTimerIdealPeriodic = SplitByte.fFloat;
				
				#ifdef DBG_CAN     
					memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
					sprintf(rgcTempCAN,"CONFIG_IDEAL_PERIODIC_TIME: %0.2f", stConfig_Value.fTimerIdealPeriodic);	
					DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
				#endif
			}break;
			
			case CONFIG_JOB_NAME:					/*supervisor name*/
			{
				#ifdef DBG_CAN      
				 DEBUG_PRINTF("CONFIG_JOB_NAME\r\n");
				#endif	        
	 
				if ( FRAME_1 == pstCANrxmsg->Data[0] )
				{
					 for(ucCnt=1; ucCnt < pstCANrxmsg->DLC; ucCnt++)
					{
						stConfig_Value.rgucJobName1[ucCnt - 1] = pstCANrxmsg->Data[ucCnt];
					}
					memset(rgucJobName,'\0',sizeof(rgucJobName));
					stConfig_Value.rgucJobName1[ucCnt - 1] = '\0';
					strcat((char *)rgucJobName,(const char *)stConfig_Value.rgucJobName1);
					
					#ifdef DBG_CAN  
						stConfig_Value.rgucJobName1[ucCnt - 1] = '\0';						
						DEBUG_PRINTF("%s\r\n",stConfig_Value.rgucJobName1);						
					#endif							
				}
				else	if ( FRAME_2 == pstCANrxmsg->Data[0] )
				{
					for(ucCnt=1; ucCnt < pstCANrxmsg->DLC; ucCnt++)
					{
						stConfig_Value.rgucJobName2[ucCnt - 1] = pstCANrxmsg->Data[ucCnt];
					}			
					stConfig_Value.rgucJobName2[ucCnt - 1] = '\0';
					strcat((char *)rgucJobName,(const char *)stConfig_Value.rgucJobName2);

					#ifdef DBG_CAN     
						stConfig_Value.rgucJobName2[ucCnt - 1] = '\0';
						DEBUG_PRINTF("%s\r\n",stConfig_Value.rgucJobName2);						
					#endif
				}
				else	if ( FRAME_3 == pstCANrxmsg->Data[0] )
				{
					for(ucCnt=1; ucCnt < pstCANrxmsg->DLC; ucCnt++)
					{
						stConfig_Value.rgucJobName3[ucCnt - 1] = pstCANrxmsg->Data[ucCnt];
					}		
					stConfig_Value.rgucJobName3[ucCnt - 1] = '\0';
					strcat((char *)rgucJobName,(const char *)stConfig_Value.rgucJobName3);
					
					#ifdef DBG_CAN  
						stConfig_Value.rgucJobName3[ucCnt - 1] = '\0';
						DEBUG_PRINTF("%s\r\n",stConfig_Value.rgucJobName3);						
					#endif			
					ucSwitchPressFlag = TRUE;
				} 
			 }break;
				
			case CONFIG_PIPE_DIAMETER:
			{
				#ifdef DBG_CAN      
				 DEBUG_PRINTF("CONFIG_PIPE_DIAMETER\r\n");
				#endif	        

				for(ucCnt=0; ucCnt < pstCANrxmsg->DLC; ucCnt++)
				{
					SplitByte.ucChar[ucCnt] = pstCANrxmsg->Data[ucCnt];
				}			
				stConfig_Value.fPipeDiameter = SplitByte.fFloat;
				
				#ifdef DBG_CAN     
					memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
					sprintf(rgcTempCAN,"CONFIG_PIPE_DIAMETER: %0.2f", stConfig_Value.fPipeDiameter);	
					DEBUG_PRINTF("%s\r\n",rgcTempCAN);						
				#endif
			}break;
			/*change3*/
			case CONFIG_ETH_WIFI_IP_SN_GW_PN:
			{
					#ifdef DBG_CAN_PARSER_CONFIG      
					DEBUG_PRINTF("CONFIG_ETH_WIFI_IP_SN_GW_PN\r\n");
					#endif
										
					/************************ETHERNET CONFIG PARAMETERS****************************/		
					if(BIT_0 == (pstCANrxmsg->Data[DATA_BYTE_ZERO] & BIT0_MASK))
						{	
						if(BIT_0 == (pstCANrxmsg->Data[DATA_BYTE_ONE] & BIT0_MASK))
							{	
								#ifdef DBG_CAN_PARSER_CONFIG      
								DEBUG_PRINTF("CONFIG_ETH_IP_ADDRESS\r\n");
								#endif
							stCurrent_Value.ucEthIPSlot1 = pstCANrxmsg->Data[DATA_BYTE_TWO];
							stCurrent_Value.ucEthIPSlot2 = pstCANrxmsg->Data[DATA_BYTE_THREE];
							stCurrent_Value.ucEthIPSlot3 = pstCANrxmsg->Data[DATA_BYTE_FOUR];
							stCurrent_Value.ucEthIPSlot4 = pstCANrxmsg->Data[DATA_BYTE_FIVE];								
							}
							else if(BIT_1 == (pstCANrxmsg->Data[DATA_BYTE_ONE] & BIT1_MASK))
							{	
								#ifdef DBG_CAN_PARSER_CONFIG      
								DEBUG_PRINTF("CONFIG_ETH_SUBNET_MASK_ADDRESS\r\n");
								#endif
							
								stCurrent_Value.ucEthSubnetSlot1 = pstCANrxmsg->Data[DATA_BYTE_TWO];
							stCurrent_Value.ucEthSubnetSlot2 = pstCANrxmsg->Data[DATA_BYTE_THREE];
							stCurrent_Value.ucEthSubnetSlot3 = pstCANrxmsg->Data[DATA_BYTE_FOUR];
							stCurrent_Value.ucEthSubnetSlot4 = pstCANrxmsg->Data[DATA_BYTE_FIVE];		
					
					/*****************************/
								}
							else if(BIT_2 == (pstCANrxmsg->Data[DATA_BYTE_ONE] & BIT2_MASK))
							{	
								#ifdef DBG_CAN_PARSER_CONFIG      
								DEBUG_PRINTF("CONFIG_ETH_GATEWAY_ADDRESS\r\n");
								#endif
							stCurrent_Value.ucEthGWSlot1 = pstCANrxmsg->Data[DATA_BYTE_TWO];
							stCurrent_Value.ucEthGWSlot2 = pstCANrxmsg->Data[DATA_BYTE_THREE];
							stCurrent_Value.ucEthGWSlot3 = pstCANrxmsg->Data[DATA_BYTE_FOUR];
							stCurrent_Value.ucEthGWSlot4 = pstCANrxmsg->Data[DATA_BYTE_FIVE];		
							}
							else if(BIT_3 == (pstCANrxmsg->Data[DATA_BYTE_ONE] & BIT3_MASK))
							{	
								#ifdef DBG_CAN_PARSER_CONFIG      
								DEBUG_PRINTF("CONFIG_ETH_PORT_NUMBER\r\n");
								#endif
							SplitByte.ucChar[DATA_BYTE_ZERO]= pstCANrxmsg->Data[DATA_BYTE_SIX];
								SplitByte.ucChar[DATA_BYTE_ONE] = pstCANrxmsg->Data[DATA_BYTE_SEVEN];
								stCurrent_Value.iEthPort = SplitByte.uiData;
					
							}
						}
						/************************WIFI CONFIG PARAMETERS****************************/		
					else if(BIT_1 == (pstCANrxmsg->Data[DATA_BYTE_ZERO] & BIT1_MASK))
						{	
						if(BIT_0 == (pstCANrxmsg->Data[DATA_BYTE_ONE] & BIT0_MASK))
							{	
								#ifdef DBG_CAN_PARSER_CONFIG      
								DEBUG_PRINTF("CONFIG_WIFI_IP_ADDRESS\r\n");
								#endif
							
							stCurrent_Value.ucWiFiIPSlot1 = pstCANrxmsg->Data[DATA_BYTE_TWO];
							stCurrent_Value.ucWiFiIPSlot2 = pstCANrxmsg->Data[DATA_BYTE_THREE];
							stCurrent_Value.ucWiFiIPSlot3 = pstCANrxmsg->Data[DATA_BYTE_FOUR];
							stCurrent_Value.ucWiFiIPSlot4 = pstCANrxmsg->Data[DATA_BYTE_FIVE];	
							
					
							}
							else if(BIT_1 == (pstCANrxmsg->Data[DATA_BYTE_ONE] & BIT1_MASK))
							{	
								#ifdef DBG_CAN_PARSER_CONFIG      
								DEBUG_PRINTF("CONFIG_WIFI_SUBNET_MASK_ADDRESS\r\n");
								#endif
							
							stCurrent_Value.ucWiFiSubnetSlot1 = pstCANrxmsg->Data[DATA_BYTE_TWO];
							stCurrent_Value.ucWiFiSubnetSlot2 = pstCANrxmsg->Data[DATA_BYTE_THREE];
							stCurrent_Value.ucWiFiSubnetSlot3 = pstCANrxmsg->Data[DATA_BYTE_FOUR];
							stCurrent_Value.ucWiFiSubnetSlot4 = pstCANrxmsg->Data[DATA_BYTE_FIVE];	
							
					
							}
							else if(BIT_2== (pstCANrxmsg->Data[DATA_BYTE_ONE] & BIT2_MASK))
							{	
								#ifdef DBG_CAN_PARSER_CONFIG      
								DEBUG_PRINTF("CONFIG_WIFI_GATEWAY_ADDRESS\r\n");
								#endif
							
						
							stCurrent_Value.ucWiFiGWSlot1 = pstCANrxmsg->Data[DATA_BYTE_TWO];
							stCurrent_Value.ucWiFiGWSlot2 = pstCANrxmsg->Data[DATA_BYTE_THREE];
							stCurrent_Value.ucWiFiGWSlot3 = pstCANrxmsg->Data[DATA_BYTE_FOUR];
							stCurrent_Value.ucWiFiGWSlot4 = pstCANrxmsg->Data[DATA_BYTE_FIVE];	
						
							
					
							}
							else if(BIT_3 == (pstCANrxmsg->Data[DATA_BYTE_ONE] & BIT3_MASK))
							{	
								#ifdef DBG_CAN_PARSER_CONFIG      
								DEBUG_PRINTF("CONFIG_WIFI_PORT_NUMBER\r\n");
								#endif
							
								SplitByte.ucChar[DATA_BYTE_ZERO]= pstCANrxmsg->Data[DATA_BYTE_SIX];
								SplitByte.ucChar[DATA_BYTE_ONE] = pstCANrxmsg->Data[DATA_BYTE_SEVEN];
								stCurrent_Value.iWiFiPort = SplitByte.uiData;
					
					/*****************************/
							}
						}		
				
			}break;
			/*change3*/
			case CONFIG_WIFI_SSID:
			{
				#ifdef DBG_CAN      
				 DEBUG_PRINTF("CONFIG_WIFI_SSID\r\n");
				#endif	        
	 
				if ( FRAME_1 == pstCANrxmsg->Data[0] )
				{
					 for(ucCnt=1; ucCnt < pstCANrxmsg->DLC; ucCnt++)
					{
						stConfig_Value.rgucSSID1[ucCnt - 1] = pstCANrxmsg->Data[ucCnt];
					}
					memset(rgucSSID,'\0',sizeof(rgucSSID));
					stConfig_Value.rgucSSID1[ucCnt - 1] = '\0';
					strcat((char *)rgucSSID,(const char *)stConfig_Value.rgucSSID1);
					
					#ifdef DBG_CAN  
						stConfig_Value.rgucSSID1[ucCnt - 1] = '\0';						
						DEBUG_PRINTF("%s\r\n",stConfig_Value.rgucSSID1);						
					#endif							
				}
				else	if ( FRAME_2 == pstCANrxmsg->Data[0] )
				{
					for(ucCnt=1; ucCnt < pstCANrxmsg->DLC; ucCnt++)
					{
						stConfig_Value.rgucSSID2[ucCnt - 1] = pstCANrxmsg->Data[ucCnt];
					}			
					stConfig_Value.rgucSSID2[ucCnt - 1] = '\0';
					strcat((char *)rgucSSID,(const char *)stConfig_Value.rgucSSID2);

					#ifdef DBG_CAN     
						stConfig_Value.rgucSSID2[ucCnt - 1] = '\0';
						DEBUG_PRINTF("%s\r\n",stConfig_Value.rgucSSID2);						
					#endif
					ucSwitchPressFlag = TRUE;
				}
				
			}break;
			/*change3*/
			case CONFIG_WIFI_PASS:
			{
				#ifdef DBG_CAN      
				 DEBUG_PRINTF("CONFIG_WIFI_PASS\r\n");
				#endif	        
	 
				if ( FRAME_1 == pstCANrxmsg->Data[0] )
				{
					 for(ucCnt=1; ucCnt < pstCANrxmsg->DLC; ucCnt++)
					{
						stConfig_Value.rgucPass1[ucCnt - 1] = pstCANrxmsg->Data[ucCnt];
					}
					memset(rgucPass,'\0',sizeof(rgucPass));
					stConfig_Value.rgucPass1[ucCnt - 1] = '\0';
					strcat((char *)rgucPass,(const char *)stConfig_Value.rgucPass1);
					
					#ifdef DBG_CAN  
						stConfig_Value.rgucPass1[ucCnt - 1] = '\0';						
						DEBUG_PRINTF("%s\r\n",stConfig_Value.Pass1);						
					#endif							
				}
				else	if ( FRAME_2 == pstCANrxmsg->Data[0] )
				{
					for(ucCnt=1; ucCnt < pstCANrxmsg->DLC; ucCnt++)
					{
						stConfig_Value.rgucPass2[ucCnt - 1] = pstCANrxmsg->Data[ucCnt];
					}			
					stConfig_Value.rgucPass2[ucCnt - 1] = '\0';
					strcat((char *)rgucPass,(const char *)stConfig_Value.rgucPass2);

					#ifdef DBG_CAN     
						stConfig_Value.rgucPass2[ucCnt - 1] = '\0';
						DEBUG_PRINTF("%s\r\n",stConfig_Value.rgucPass2);						
					#endif
					ucSwitchPressFlag = TRUE;
				}
				
				
			}break;
			/*change1*/
				case CONFIG_UTC_TIME_OFFSET:
			{
				#ifdef DBG_CAN      
				 DEBUG_PRINTF("CONFIG_UTC_TIME_OFFSET\r\n");
				#endif	        
				stConfig_Value.ucConfig_TimeOffsetSign = pstCANrxmsg->Data[DATA_BYTE_ZERO];
				stConfig_Value.ucConfig_TimeOffsetHour = pstCANrxmsg->Data[DATA_BYTE_ONE];
				stConfig_Value.ucConfig_TimeOffsetMin = pstCANrxmsg->Data[DATA_BYTE_TWO];
				stConfig_Value.ucConfig_TimeOffsetSec = pstCANrxmsg->Data[DATA_BYTE_THREE];
				
				#ifdef DBG_CAN     
					memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
					sprintf(rgcTempCAN,"stConfig_Value.ucConfig_TimeOffsetSign: %d", stConfig_Value.ucConfig_TimeOffsetSign);	
					DEBUG_PRINTF("%s\r\n",rgcTempCAN);		
					memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
					sprintf(rgcTempCAN,"stConfig_Value.ucConfig_TimeOffsetHour: %d", stConfig_Value.ucConfig_TimeOffsetHour);	
					DEBUG_PRINTF("%s\r\n",rgcTempCAN);
					memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
					sprintf(rgcTempCAN,"stConfig_Value.ucConfig_TimeOffsetMin: %d", stConfig_Value.ucConfig_TimeOffsetMin);	
					DEBUG_PRINTF("%s\r\n",rgcTempCAN);	
					memset(rgcTempCAN, '\0', sizeof(rgcTempCAN));
					sprintf(rgcTempCAN,"stConfig_Value.ucConfig_TimeOffsetSec: %d", stConfig_Value.ucConfig_TimeOffsetSec);	
					DEBUG_PRINTF("%s\r\n",rgcTempCAN);					
				#endif
			}break;
			case CONFIG_JOB_ID:					/*supervisor name*/
				{
					#ifdef DBG_CAN      
					 DEBUG_PRINTF("CONFIG_JOB_ID\r\n");
					#endif	        
		 
					if ( FRAME_1 == pstCANrxmsg->Data[0] )
					{
						 for(ucCnt=1; ucCnt < pstCANrxmsg->DLC; ucCnt++)
						{
							stConfig_Value.rgucJobId1[ucCnt - 1] = pstCANrxmsg->Data[ucCnt];
						}
						memset(rgucJobId,'\0',sizeof(rgucJobId));
						stConfig_Value.rgucJobId1[ucCnt - 1] = '\0';
						strcat((char *)rgucJobId,(const char *)stConfig_Value.rgucJobId1);
						
						#ifdef DBG_CAN  
							stConfig_Value.rgucJobId1[ucCnt - 1] = '\0';						
							DEBUG_PRINTF("%s\r\n",stConfig_Value.rgucJobId1);						
						#endif							
					}
					else	if ( FRAME_2 == pstCANrxmsg->Data[0] )
					{
						for(ucCnt=1; ucCnt < pstCANrxmsg->DLC; ucCnt++)
						{
							stConfig_Value.rgucJobId2[ucCnt - 1] = pstCANrxmsg->Data[ucCnt];
						}			
						stConfig_Value.rgucJobId2[ucCnt - 1] = '\0';
						strcat((char *)rgucJobId,(const char *)stConfig_Value.rgucJobId2);

						#ifdef DBG_CAN     
							stConfig_Value.rgucJobId2[ucCnt - 1] = '\0';
							DEBUG_PRINTF("%s\r\n",stConfig_Value.rgucJobId2);						
						#endif
					}
					else	if ( FRAME_3 == pstCANrxmsg->Data[0] )
					{
						for(ucCnt=1; ucCnt < pstCANrxmsg->DLC; ucCnt++)
						{
							stConfig_Value.rgucJobId3[ucCnt - 1] = pstCANrxmsg->Data[ucCnt];
						}		
						stConfig_Value.rgucJobId3[ucCnt - 1] = '\0';
						strcat((char *)rgucJobId,(const char *)stConfig_Value.rgucJobId3);
						
						#ifdef DBG_CAN  
							stConfig_Value.rgucJobId3[ucCnt - 1] = '\0';
							DEBUG_PRINTF("%s\r\n",stConfig_Value.rgucJobId3);						
						#endif
						ucSwitchPressFlag = TRUE;						
					} 
			 }break;
			default:
			break;         
			}           
    }break;	
		default:
       break;
  }
	
	if ( TRUE == StatusFlag.stStatusFlag.bErrorFlag )
	{
		StatusFlag.stStatusFlag.bWarningFlag = FALSE;
	}
	/*
	if ( TRUE == StatusFlag.stStatusFlag.bWarningFlag )
	{
		StatusFlag.stStatusFlag.bErrorFlag = FALSE;
	}
	*/
}
/*****************************************************************************
** @Function name		: fnSendACK
** @Descriptions		: Sends acknowledgement frame on CAN bus
** @parameters			: pstCANrxmsg: pointer to a structure receive frame which contains CAN Id,
											CAN DLC, CAN data and FMI number.
** @return					: none
*****************************************************************************/
void fnSendACK(CanRxMsg *pstCANrxmsg)
{
  CanTxMsg stTxMsgACK;
  
  SwapExtID.uliCanID = pstCANrxmsg->ExtId;
  
  /*Configure CAN Transmitt Message tructure*/
  stTxMsgACK.IDE = CAN_ID_EXT;
  stTxMsgACK.RTR = CAN_RTR_DATA;
  
  /* Reset Data Bufer */
  memset(stTxMsgACK.Data,0, sizeof(stTxMsgACK.Data)); 
  
  /*Update Length of CAN Data Frame*/
  stTxMsgACK.DLC = 1;
	
	if(( FALSE == StatusFlag.stStatusFlag.bFirst_HB ) && ( 0x0001FF02 == pstCANrxmsg->ExtId ))
	{
		StatusFlag.stStatusFlag.bFirst_HB = TRUE;
		stTxMsgACK.Data[0] = 0x02;		/* First hearbeat */
	}
	else
	{
		stTxMsgACK.Data[0] = 0x00;		/* Positive ACk */
	}
	
  SwapExtID.ucChar[1] = SwapExtID.ucChar[0];
  SwapExtID.ucChar[0] = SOURCE_HMI;
  SwapExtID.ucChar[2] = 0xE8;	
  stTxMsgACK.ExtId = SwapExtID.uliCanID;
  
  /*Transmitt CAN message onto CAN bus*/
  
  ucSendAckMBNo = CAN_Transmit(CAN1, &stTxMsgACK);
  
}

/*****************************************************************************
** @Function name		: fnACK_Check
** @Descriptions		: Sends acknowledgement frame on CAN bus
** @parameters			: pstCANrxmsg: pointer to a structure receive frame which contains CAN Id,
											CAN DLC, CAN data and FMI number.
** @return					: none
*****************************************************************************/
void fnACK_Check(void)
{
  static uint8_t sucResendCount = 0;
	
	if(TRUE == StatusFlag.stStatusFlag.bTxFlag)
	{
		if(FALSE == StatusFlag.stStatusFlag.bACK_Received)
		{
			/*Resend the Can Message if failed to receive the acknowledge */
			if(CAN_MSG_RESEND_TIME <= uiTxCounter)
			{
				/* resend time check max 3 time */
				if(CAN_MSG_RESEND_ATTEMPT > sucResendCount)
				{
					uiTxCounter = 0;
					sucResendCount++;
					fnCAN_Transmit(&rgstTxCANBuff[ucTxBuffCnt],CAN1);
					
					#ifdef GENERAL_DBG	
						DEBUG_PRINTF("Transmit again\r\n");
					#endif
				}
				else /*3 time send fails*/
				{
					StatusFlag.stStatusFlag.bTxFlag = FALSE;					
					uiTxCounter = 0;
					sucResendCount = 0;
					
					if(DEST_SCU == (rgstTxCANBuff[ucTxBuffCnt].ExtId & DEST_ADDR_MASK))
					{
						StatusFlag.stStatusFlag.bCANFaultMCU = SET;/* MCU CAN FAULT */
						StatusFlag.stStatusFlag.bCANFaultFlag = SET;
					}
//					else if(DEST_OPMM == (rgstTxCANBuff[ucTxBuffCnt].ExtId & DEST_ADDR_MASK))
//					{
//						StatusFlag.stStatusFlag.bCANFaultOPMM = SET;/* OPMM CAN FAULT */
//						StatusFlag.stStatusFlag.bCANFaultFlag = SET;
//					}
					
					if((ucTxBuffCnt != ucStoreBuffCnt) || (TRUE == StatusFlag.stStatusFlag.bDataInCANTx_BuffFlag))
					{
						fnTxBuff_Shift();	
					}
					
					/* OFF CAN Status Led GREEN */
					fnLED_TurnONOFF (PORT_LED_COM_GREEN, Bit_RESET,PIN_LED_COM_GREEN);
					/* OFF Board Status LED */
					fnLED_TurnONOFF (PORT_LED_ERR_GREEN, Bit_RESET,PIN_LED_ERR_GREEN);
					
					#ifdef GENERAL_DBG
						DEBUG_PRINTF("Transmit Fail\r\n");
					#endif
				}
			}
		}
		else
		{
			StatusFlag.stStatusFlag.bTxFlag = FALSE;
			StatusFlag.stStatusFlag.bACK_Received = FALSE;	
			
			uiTxCounter = 0;
			sucResendCount = 0;
			if((ucTxBuffCnt != ucStoreBuffCnt) || (SET == StatusFlag.stStatusFlag.bDataInCANTx_BuffFlag))
			{
				fnTxBuff_Shift();
			}	
			#ifdef GENERAL_DBG	
				DEBUG_PRINTF(" Transmit success\r\n");
			#endif
		}
	}
}

/*****************************************************************************
** @Function name	: fnHeartBeat_Check
** @Descriptions	: check the flag for the heartbeat message and TRUE/FALSE respective flags
** @parameters		: none   
** @return				: none
*****************************************************************************/
void fnHeartBeat_Check(void)
{
  /*Heartbeat Fault check*/
  if ( HEARTBEAT_TIMEOUT <= uiHBTimeCounter )
  {
    uiHBTimeCounter = 0;
    
		#ifdef GENERAL_DBG      
			DEBUG_PRINTF("HEartBEAt NOT received !!!!\r\n");
		#endif

		ucLEDBlinkCount = 0;
    StatusFlag.stStatusFlag.bCANFaultMCU = TRUE;
    
    if ( FALSE == StatusFlag.stStatusFlag.bHB_Not_Rec )
    {
      StatusFlag.stStatusFlag.bHB_Not_Rec = TRUE;
    }
  }
}
/*****************************************************************************
**@Function		 	:  fnTxBuff_Store
**@Descriptions	:  Stores the CAN Structure in the Buffer
**@parameters		:  pstTxMsg: pointer to CAN Transmit structure
**@return				:  none
*****************************************************************************/
void fnTxBuff_Store(CanTxMsg *pstTxMsg)
{	
	#ifdef GENERAL_DBG
		DEBUG_PRINTF("In fnTxStoreBuff = %d \r\nId = 0x%x\r\n", ucStoreBuffCnt, pstTxMsg->ExtId);
	#endif
	memmove(&rgstTxCANBuff[ucStoreBuffCnt++], pstTxMsg, sizeof(CanTxMsg));
	if(TX_CAN_BUF_SIZE == ucStoreBuffCnt)
	{
		ucStoreBuffCnt = 0;
		StatusFlag.stStatusFlag.bDataInCANTx_BuffFlag = TRUE;
	}
}

/*****************************************************************************
**@Function		 	:  fnTxBuff_Shift
**@Descriptions	:  Shift the Buffer when the ack received or the msg is discarded 
									 and transmit the next frame in the buffer
**@parameters		:  none
**@return				:  none
*****************************************************************************/
void fnTxBuff_Shift(void)
{
	memset(&rgstTxCANBuff[ucTxBuffCnt++], 0 , sizeof(CanTxMsg));
	if(TX_CAN_BUF_SIZE <= ucTxBuffCnt)
	{
		ucTxBuffCnt = 0;
		StatusFlag.stStatusFlag.bDataInCANTx_BuffFlag = FALSE;
	}
	#ifdef GENERAL_DBG
		DEBUG_PRINTF("In fnTxBuff_Shift TxCNT = %d and = %d\r\n", ucTxBuffCnt,ucStoreBuffCnt);
	#endif
	if((ucTxBuffCnt != ucStoreBuffCnt) || (TRUE == StatusFlag.stStatusFlag.bDataInCANTx_BuffFlag))/*if buffer is not empty */
	{
		for(; ((ucTxBuffCnt < ucStoreBuffCnt) || (TRUE == StatusFlag.stStatusFlag.bDataInCANTx_BuffFlag)) ; ucTxBuffCnt++)
		{
			/* Empty the CAN Transmit Buffer if the respective Destination CAN Fault occurs*/
			if(((DEST_OPMM == (rgstTxCANBuff[ucTxBuffCnt].ExtId & DEST_ADDR_MASK)) && (TRUE == StatusFlag.stStatusFlag.bCANFaultOPMM)) || 
				 (((DEST_SCU == (rgstTxCANBuff[ucTxBuffCnt].ExtId & DEST_ADDR_MASK)) && (TRUE == StatusFlag.stStatusFlag.bCANFaultMCU))))	
			{
				memset(&rgstTxCANBuff[ucTxBuffCnt], 0 , sizeof(CanTxMsg));
				#ifdef GENERAL_DBG
					DEBUG_PRINTF("in emptying CAN MSG of OP, Txcnt = %d storebuff = %d \r\n", ucTxBuffCnt, ucStoreBuffCnt);
				#endif
			}
			else
			{
				break;
			}
		}
		if((ucTxBuffCnt != ucStoreBuffCnt) || (TRUE == StatusFlag.stStatusFlag.bDataInCANTx_BuffFlag))
		{
			fnCAN_Transmit(&rgstTxCANBuff[ucTxBuffCnt],CAN1);
		}
	}
}

/*****************************************************************************
**@Function 		: fnCANMessage_Send
**@Descriptions	: Sends status message on CAN
**@parameters		: uliMessageId: message id.
									uliMsgType:	Message type.
									ucMsgTo : to state weather the message is for MCU or OP or both.
**@return				: none
*****************************************************************************/
void fnCANMessage_Send(uint32_t uliMessageId,uint32_t uliMsgType,uint8_t ucMsgTo)
{
	uint8_t ucCnt = 0;
	CanTxMsg stSendCANMsg;
	stSendCANMsg.DLC = stTx1Message.DLC;
	for(ucCnt = 0; ucCnt < stSendCANMsg.DLC; ucCnt++)
	{
		stSendCANMsg.Data[ucCnt] = stTx1Message.Data[ucCnt];
	}
	
	/*ESTOP*/
	if(ALL_SLAVE_DEVICE == (ucMsgTo & ALL_SLAVE_DEVICE))
	{
			/*send message to OP*/
			stSendCANMsg.ExtId = (SOURCE_HMI | DEST_ALL | uliMessageId | RESERVED | uliMsgType);
			
			fnCAN_Transmit(&stSendCANMsg,CAN1);
			
			//fnTxBuff_Store(&stSendCANMsg);	
		
	}
	else
	{
		if(SCU_ID == ucMsgTo)
		{
			if(FALSE == StatusFlag.stStatusFlag.bCANFaultMCU)
			{
				/*send message to MCU*/
				
				#ifdef GENERAL_DBG
					DEBUG_PRINTF("In fnTxStoreBuff = %d \r\n ucTxBuffCnt = %d\r\nId = 0x%x\r\n", ucStoreBuffCnt,ucTxBuffCnt);
				#endif
				
				stSendCANMsg.ExtId = (SOURCE_HMI | DEST_SCU | uliMessageId | RESERVED | uliMsgType);
				if((ucTxBuffCnt == ucStoreBuffCnt) && (FALSE == StatusFlag.stStatusFlag.bDataInCANTx_BuffFlag))
				{
					fnCAN_Transmit(&stSendCANMsg,CAN1);
				}
				fnTxBuff_Store(&stSendCANMsg);			
			}
		}
		if(OPMM_ID == ucMsgTo)
		{
			if(FALSE == StatusFlag.stStatusFlag.bCANFaultOPMM)
			{
				/*send message to OPMM*/
				stSendCANMsg.ExtId = (SOURCE_HMI | DEST_OPMM | uliMessageId | RESERVED | uliMsgType);
				
				if((ucTxBuffCnt == ucStoreBuffCnt) && (FALSE == StatusFlag.stStatusFlag.bDataInCANTx_BuffFlag))
				{
					fnCAN_Transmit(&stSendCANMsg,CAN1);
				}
				fnTxBuff_Store(&stSendCANMsg);	
			}
		}

		if(HCM_ID == ucMsgTo)
		{
			/*send message to DEST_IHCM4_HM*/
				stSendCANMsg.ExtId = (SOURCE_HMI | DEST_HCM | uliMessageId | RESERVED | uliMsgType);
				
				if((ucTxBuffCnt == ucStoreBuffCnt) && (FALSE == StatusFlag.stStatusFlag.bDataInCANTx_BuffFlag))
				{
					fnCAN_Transmit(&stSendCANMsg,CAN1);
				}
				fnTxBuff_Store(&stSendCANMsg);
		}
		
		if(VCM_ID == ucMsgTo)
		{
			/*send message to VCM*/
				stSendCANMsg.ExtId = (SOURCE_HMI | DEST_VCM | uliMessageId | RESERVED | uliMsgType);
				
				if((ucTxBuffCnt == ucStoreBuffCnt) && (FALSE == StatusFlag.stStatusFlag.bDataInCANTx_BuffFlag))
				{
					fnCAN_Transmit(&stSendCANMsg,CAN1);
				}
				fnTxBuff_Store(&stSendCANMsg);
		}
	}
}

/*****************************************************************************
**@Function		 	:  fnOPConfig_Send()
**@Descriptions	:	 Send configuration mesages to SCU. All values sent to SCU are sent in Metric form.
**@parameters		:  uliMessageId : configuration message ID
									 ucTypeOfData : type of data i.e low or high threshold etc.
**@return				:  none
*****************************************************************************/
void fnOPConfig_Send(uint32_t uliMessageId,uint8_t ucTypeOfData)
{
	memset(stTx1Message.Data, 0 , sizeof(stTx1Message.Data));
	switch(uliMessageId)
	{
		case CONFIG_SPRAY_PRESSURE:
		{
			stTx1Message.DLC = 5;
			
			if ( (LOW_TH << 1)== ucTypeOfData )
			{
				if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = fnImperialToMetric_Conv(stConfig_Value.fConfigSprayPressureLTH, PRESSURE_VALUE);
				}
				else if ( METRIC == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = stConfig_Value.fConfigSprayPressureLTH;
				}
				stTx1Message.Data[0] = LOW_TH << 1;
			}
			else if ( SET_VALUE == ucTypeOfData )
			{
				if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = fnImperialToMetric_Conv(stConfig_Value.fConfigSprayPressure, PRESSURE_VALUE);
				}
				else if ( METRIC == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = stConfig_Value.fConfigSprayPressure;
				}
				stTx1Message.Data[0] = SET_VALUE;
			}
			else if ( (HIGH_TH << 1) == ucTypeOfData )
			{
				if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = fnImperialToMetric_Conv(stConfig_Value.fConfigSprayPressureHTH, PRESSURE_VALUE);
				}
				else if ( METRIC == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = stConfig_Value.fConfigSprayPressureHTH;
				}
				stTx1Message.Data[0] = HIGH_TH << 1;
			}
			else if ( SPRAY_PRES_HYST_POS == ucTypeOfData )
			{
				if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = fnImperialToMetric_Conv(stConfig_Value.fConfigHysSprayPresPos, PRESSURE_VALUE);
				}
				else if ( METRIC == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = stConfig_Value.fConfigHysSprayPresPos;
				}
				stTx1Message.Data[0] = SPRAY_PRES_HYST_POS;
			}
			else if ( SPRAY_PRES_HYST_NEG == ucTypeOfData )
			{
				if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = fnImperialToMetric_Conv(stConfig_Value.fConfigHysSprayPresNeg, PRESSURE_VALUE);
				}
				else if ( METRIC == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = stConfig_Value.fConfigHysSprayPresNeg;
				}
				stTx1Message.Data[0] = SPRAY_PRES_HYST_NEG;
			}
			
			for(ucCnt = 1; ucCnt < stTx1Message.DLC; ucCnt++)
			{
				stTx1Message.Data[ucCnt] =	SplitByte.ucChar[ucCnt - 1];
			}
			/*Send the CONFIG_SPRAY_PRESSURE to MCU and OP */
			fnCANMessage_Send(CONFIG_SPRAY_PRESSURE, MSG_TYPE_CONFIG, SCU_ID);
		}break;
		case CONFIG_RECIRC_PRESSURE:
		{
			stTx1Message.DLC = 5;
			
			if ( (LOW_TH << 1)== ucTypeOfData )
			{
				if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = fnImperialToMetric_Conv(stConfig_Value.fConfigRecirPressureLTH, PRESSURE_VALUE);
				}
				else if ( METRIC == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = stConfig_Value.fConfigRecirPressureLTH;
				}
				stTx1Message.Data[0] = LOW_TH << 1;
			}
			else if ( SET_VALUE == ucTypeOfData )
			{
				if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = fnImperialToMetric_Conv(stConfig_Value.fConfigRecirPressure, PRESSURE_VALUE);
				}
				else if ( METRIC == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = stConfig_Value.fConfigRecirPressure;
				}
				stTx1Message.Data[0] = SET_VALUE;
			}
			else if ( (HIGH_TH << 1) == ucTypeOfData )
			{
				if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = fnImperialToMetric_Conv(stConfig_Value.fConfigRecirPressureHTH, PRESSURE_VALUE);
				}
				else if ( METRIC == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = stConfig_Value.fConfigRecirPressureHTH;
				}
				stTx1Message.Data[0] = HIGH_TH << 1;
			}
			else if ( RECIRC_PRES_HYST_POS == ucTypeOfData )
			{
				if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat= fnImperialToMetric_Conv(stConfig_Value.fConfigHysRecircPresPos, PRESSURE_VALUE);
				}
				else if ( METRIC == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = stConfig_Value.fConfigHysRecircPresPos;
				}
				stTx1Message.Data[0] = RECIRC_PRES_HYST_POS;
			}
			else if ( RECIRC_PRES_HYST_NEG == ucTypeOfData )
			{
				if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = fnImperialToMetric_Conv(stConfig_Value.fConfigHysRecircPresNeg, PRESSURE_VALUE);
				}
				else if ( METRIC == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = stConfig_Value.fConfigHysRecircPresNeg;
				}
				stTx1Message.Data[0] = RECIRC_PRES_HYST_POS;
			}
			
			for(ucCnt = 1; ucCnt < stTx1Message.DLC; ucCnt++)
			{
				stTx1Message.Data[ucCnt] =	SplitByte.ucChar[ucCnt - 1];
			}
	
			/*Send the CONFIG_RECIRC_PRESSURE to MCU and OP */
			fnCANMessage_Send(CONFIG_RECIRC_PRESSURE, MSG_TYPE_CONFIG, SCU_ID);
		}break;
		case CONFIG_PIPE_TEMP_TH:
		{
			stTx1Message.DLC = 5;

			if ( LOW_TH == ucTypeOfData )
			{
				if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = fnImperialToMetric_Conv(stConfig_Value.fConfigPipeTempLTH, TEMPERATURE_VALUE);
				}
				else if ( METRIC == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = stConfig_Value.fConfigPipeTempLTH;
				}
				stTx1Message.Data[0] = LOW_TH;
			}
			else if ( HIGH_TH == ucTypeOfData )
			{
				if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = fnImperialToMetric_Conv(stConfig_Value.fConfigPipeTempHTH, TEMPERATURE_VALUE);
				}
				else if ( METRIC == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = stConfig_Value.fConfigPipeTempHTH;
				}
				stTx1Message.Data[0] = HIGH_TH;
			}
			
			for(ucCnt = 1; ucCnt < stTx1Message.DLC; ucCnt++)
			{
				stTx1Message.Data[ucCnt] =	SplitByte.ucChar[ucCnt - 1];
			}
			/*Send the CONFIG_PIPE_TEMP_TH to MCU and OP */
			fnCANMessage_Send(CONFIG_PIPE_TEMP_TH, MSG_TYPE_CONFIG, SCU_ID);
		}break;
		case CONFIG_SPRAY_COUNT:
		{
			stTx1Message.DLC = 1;

			stTx1Message.Data[0] =	stConfig_Value.uiConfigSprayCount;
	
			/*Send the CONFIG_SPRAY_COUNT to MCU and OP */
			fnCANMessage_Send(CONFIG_SPRAY_COUNT, MSG_TYPE_CONFIG, SCU_ID);
		}break;
		case CONFIG_PRIME_TIME:
		{
			stTx1Message.DLC = 4;
			
			SplitByte.fFloat = stConfig_Value.fConfigPrimeTime;
			
			for(ucCnt = 0; ucCnt < stTx1Message.DLC; ucCnt++)
			{
				stTx1Message.Data[ucCnt] =	SplitByte.ucChar[ucCnt];
			}
			/*Send the CONFIG_PRIME_TIME to MCU and OP */
			fnCANMessage_Send(CONFIG_PRIME_TIME, MSG_TYPE_CONFIG, SCU_ID);
		}break;
		case CONFIG_OPMM_GUN_DELAY:
		{
			stTx1Message.DLC = 4;
			
			SplitByte.fFloat = stConfig_Value.fConfigGunDelayTime / 1000;
			
			for(ucCnt = 0; ucCnt < stTx1Message.DLC; ucCnt++)
			{
				stTx1Message.Data[ucCnt] =	SplitByte.ucChar[ucCnt];
			}
			/*Send the CONFIG_OPMM_GUN_DELAY to MCU and OP */
			fnCANMessage_Send(CONFIG_OPMM_GUN_DELAY, MSG_TYPE_CONFIG, SCU_ID);
		}break;
		case CONFIG_OPMM_SPRAY_OVERLAP_TIME:
		{
			stTx1Message.DLC = 4;
			
			SplitByte.fFloat = stConfig_Value.fConfigSprayOverlapTime / 1000;
			
			for(ucCnt = 0; ucCnt < stTx1Message.DLC; ucCnt++)
			{
				stTx1Message.Data[ucCnt] =	SplitByte.ucChar[ucCnt];
			}
			/*Send the CONFIG_OPMM_SPRAY_OVERLAP_TIME to MCU and OP */
			fnCANMessage_Send(CONFIG_OPMM_SPRAY_OVERLAP_TIME, MSG_TYPE_CONFIG, SCU_ID);
		}break;
		case CONFIG_OPMM_REVERSE_DELAY:
		{
			stTx1Message.DLC = 4;
			
			SplitByte.fFloat = stConfig_Value.fConfigReverseDelayTime / 1000;
			
			for(ucCnt = 0; ucCnt < stTx1Message.DLC; ucCnt++)
			{
				stTx1Message.Data[ucCnt] =	SplitByte.ucChar[ucCnt];
			}
			/*Send the CONFIG_OPMM_REVERSE_DELAY to MCU and OP */
			fnCANMessage_Send(CONFIG_OPMM_REVERSE_DELAY, MSG_TYPE_CONFIG, SCU_ID);
		}break;
		case CONFIG_OPMM_FLUSH_TIME:
		{
			stTx1Message.DLC = 4;
			
			SplitByte.fFloat = stConfig_Value.fConfigFlushTime;
			
			for(ucCnt = 0; ucCnt < stTx1Message.DLC; ucCnt++)
			{
				stTx1Message.Data[ucCnt] =	SplitByte.ucChar[ucCnt];
			}
			/*Send the CONFIG_OPMM_FLUSH_TIME to MCU and OP */
			fnCANMessage_Send(CONFIG_OPMM_FLUSH_TIME, MSG_TYPE_CONFIG, SCU_ID);
		}break;
		case CONFIG_VCM_STALL_TH:
		{
			stTx1Message.DLC = 3;
			
			if ( DOWNSTROKE_TH == ucTypeOfData )
			{
				SplitByte.iData = stConfig_Value.iConfigDownStrokeTH;
				stTx1Message.Data[0] = DOWNSTROKE_TH;
			}
			else if ( UPSTROKE_TH == ucTypeOfData )
			{
				SplitByte.iData = stConfig_Value.iConfigUpStrokeTH;
				stTx1Message.Data[0] = UPSTROKE_TH;
			}
			
			for(ucCnt = 1; ucCnt < stTx1Message.DLC; ucCnt++)
			{
				stTx1Message.Data[ucCnt] =	SplitByte.ucChar[ucCnt - 1];
			}
			
			/*Send the CONFIG_VCM_STALL_TH to MCU and OP */
			fnCANMessage_Send(CONFIG_VCM_STALL_TH, MSG_TYPE_CONFIG, SCU_ID);
		}break;
		case CONFIG_VCM_PGAUGE1_TH:
		{
			stTx1Message.DLC = 5;

			if ( LOW_TH == ucTypeOfData )
			{
				if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = fnImperialToMetric_Conv(stConfig_Value.fConfigPressureGauge1LTH, PRESSURE_VALUE);
				}
				else if ( METRIC == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = stConfig_Value.fConfigPressureGauge1LTH;
				}
				stTx1Message.Data[0] = LOW_TH;
			}
			else if ( HIGH_TH == ucTypeOfData )
			{
				if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = fnImperialToMetric_Conv(stConfig_Value.fConfigPressureGauge1HTH, PRESSURE_VALUE);
				}
				else if ( METRIC == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = stConfig_Value.fConfigPressureGauge1HTH;
				}
				stTx1Message.Data[0] = HIGH_TH;
			}
			
			for(ucCnt = 1; ucCnt < stTx1Message.DLC; ucCnt++)
			{
				stTx1Message.Data[ucCnt] =	SplitByte.ucChar[ucCnt - 1];
			}
			
			/*Send the CONFIG_VCM_PGAUGE1_TH to MCU and OP */
			fnCANMessage_Send(CONFIG_VCM_PGAUGE1_TH, MSG_TYPE_CONFIG, SCU_ID);
		}break;
		case CONFIG_VCM_PGAUGE2_TH:
		{
			stTx1Message.DLC = 5;

			if ( LOW_TH == ucTypeOfData )
			{
				if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = fnImperialToMetric_Conv(stConfig_Value.fConfigPressureGauge2LTH, PRESSURE_VALUE);
				}
				else if ( METRIC == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = stConfig_Value.fConfigPressureGauge2LTH;
				}
				stTx1Message.Data[0] = LOW_TH;
			}
			else if ( HIGH_TH == ucTypeOfData )
			{
				if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = fnImperialToMetric_Conv(stConfig_Value.fConfigPressureGauge2HTH, PRESSURE_VALUE);
				}
				else if ( METRIC == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = stConfig_Value.fConfigPressureGauge2HTH;
				}
				stTx1Message.Data[0] = HIGH_TH;
			}
			
			for(ucCnt = 1; ucCnt < stTx1Message.DLC; ucCnt++)
			{
				stTx1Message.Data[ucCnt] =	SplitByte.ucChar[ucCnt - 1];
			}
			
			/*Send the CONFIG_VCM_PGAUGE2_TH to MCU and OP */
			fnCANMessage_Send(CONFIG_VCM_PGAUGE2_TH, MSG_TYPE_CONFIG, SCU_ID);
		}break;
		case CONFIG_VCM_PGAUGE3_TH:
		{
			stTx1Message.DLC = 5;

			if ( LOW_TH == ucTypeOfData )
			{
				if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = fnImperialToMetric_Conv(stConfig_Value.fConfigPressureGauge3LTH, PRESSURE_VALUE);
				}
				else if ( METRIC == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = stConfig_Value.fConfigPressureGauge3LTH;
				}
				stTx1Message.Data[0] = LOW_TH;
			}
			else if ( HIGH_TH == ucTypeOfData )
			{
				if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = fnImperialToMetric_Conv(stConfig_Value.fConfigPressureGauge3HTH, PRESSURE_VALUE);
				}
				SplitByte.fFloat = stConfig_Value.fConfigPressureGauge3HTH;
				stTx1Message.Data[0] = HIGH_TH;
			}
			
			for(ucCnt = 1; ucCnt < stTx1Message.DLC; ucCnt++)
			{
				stTx1Message.Data[ucCnt] =	SplitByte.ucChar[ucCnt - 1];
			}
			
			/*Send the CONFIG_VCM_PGAUGE3_TH to MCU and OP */
			fnCANMessage_Send(CONFIG_VCM_PGAUGE3_TH, MSG_TYPE_CONFIG, SCU_ID);
		}break;
		case CONFIG_BASE_TEMP_SETTING:
		{
			stTx1Message.DLC = 5;

			if ( (LOW_TH << 1)== ucTypeOfData )
			{
				if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = fnImperialToMetric_Conv(stConfig_Value.fConfigAntiFrzTempLTH, TEMPERATURE_VALUE);
				}
				else if ( METRIC == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = stConfig_Value.fConfigAntiFrzTempLTH;
				}
				stTx1Message.Data[0] = LOW_TH << 1;
			}
			else if ( SET_VALUE == ucTypeOfData )
			{
				if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = fnImperialToMetric_Conv(stConfig_Value.fConfigAntiFrzTempSet, TEMPERATURE_VALUE);
				}
				else if ( METRIC == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = stConfig_Value.fConfigAntiFrzTempSet;
				}
				stTx1Message.Data[0] = SET_VALUE;
			}
			else if ( (HIGH_TH << 1) == ucTypeOfData )
			{
				if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = fnImperialToMetric_Conv(stConfig_Value.fConfigAntiFrzTempHTH, TEMPERATURE_VALUE);
				}
				else if ( METRIC == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = stConfig_Value.fConfigAntiFrzTempHTH;
				}
				stTx1Message.Data[0] = HIGH_TH << 1;
			}
			else if ( ANTIFRZ_HYST_POS == ucTypeOfData )
			{
				if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = fnImperialToMetric_Conv(stConfig_Value.fConfigHysAntiFrzPos, TEMPERATURE_VALUE);
				}
				else if ( METRIC == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = stConfig_Value.fConfigHysAntiFrzPos;
				}
				stTx1Message.Data[0] = ANTIFRZ_HYST_POS;
			}
			else if ( ANTIFRZ_HYST_NEG == ucTypeOfData )
			{
				if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = fnImperialToMetric_Conv(stConfig_Value.fConfigHysAntiFrzNeg, TEMPERATURE_VALUE);
				}
				else if ( METRIC == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = stConfig_Value.fConfigHysAntiFrzNeg;
				}
				stTx1Message.Data[0] = ANTIFRZ_HYST_NEG;
			}
			
			for(ucCnt = 1; ucCnt < stTx1Message.DLC; ucCnt++)
			{
				stTx1Message.Data[ucCnt] =	SplitByte.ucChar[ucCnt - 1];
			}
			/*Send the CONFIG_BASE_TEMP_SETTING to MCU and OP */
			fnCANMessage_Send(CONFIG_BASE_TEMP_SETTING, MSG_TYPE_CONFIG, SCU_ID);
		}break;

		case CONFIG_BASE_LEVEL_SETTING:
		{
			stTx1Message.DLC = 5;

			if ( LOW_TH == ucTypeOfData )
			{
				if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = fnImperialToMetric_Conv( stConfig_Value.fConfigBaseLiquidLevelLTH, INCH_TO_MM);
				}
				else if ( METRIC == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = stConfig_Value.fConfigBaseLiquidLevelLTH;
				}
				stTx1Message.Data[0] = LOW_TH;
			}
			else if ( HIGH_TH == ucTypeOfData )
			{
				if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = fnImperialToMetric_Conv( stConfig_Value.fConfigBaseLiquidLevelHTH, INCH_TO_MM);
				}
				else if ( METRIC == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = stConfig_Value.fConfigBaseLiquidLevelHTH;
				}
				stTx1Message.Data[0] = HIGH_TH;
			}
			
			for(ucCnt = 1; ucCnt < stTx1Message.DLC; ucCnt++)
			{
				stTx1Message.Data[ucCnt] =	SplitByte.ucChar[ucCnt - 1];
			}
			/*Send the CONFIG_BASE_LEVEL_SETTING to MCU and OP */
			fnCANMessage_Send(CONFIG_BASE_LEVEL_SETTING, MSG_TYPE_CONFIG, SCU_ID);
		}break;
		
		case CONFIG_HARD_LEVEL_SETTING:
		{
			stTx1Message.DLC = 5;

			if ( LOW_TH == ucTypeOfData )
			{
				if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = fnImperialToMetric_Conv( stConfig_Value.fConfigHardLiquidLevelLTH, INCH_TO_MM);
				}
				else if ( METRIC == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = stConfig_Value.fConfigHardLiquidLevelLTH;
				}
				stTx1Message.Data[0] = LOW_TH;
			}
			else if ( HIGH_TH == ucTypeOfData )
			{
				if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = fnImperialToMetric_Conv( stConfig_Value.fConfigHardLiquidLevelHTH, INCH_TO_MM);
				}
				else if ( METRIC == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = stConfig_Value.fConfigHardLiquidLevelHTH;
				}
				stTx1Message.Data[0] = HIGH_TH;
			}
			
			for(ucCnt = 1; ucCnt < stTx1Message.DLC; ucCnt++)
			{
				stTx1Message.Data[ucCnt] =	SplitByte.ucChar[ucCnt - 1];
			}
			
			fnCANMessage_Send(CONFIG_HARD_LEVEL_SETTING, MSG_TYPE_CONFIG, SCU_ID);
		}break;
		
		case CONFIG_INLINE_BASE_TEMP_SETTING:
		{
			stTx1Message.DLC = 5;
			
			if ( (LOW_TH << 1) == ucTypeOfData )
			{
				if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = fnImperialToMetric_Conv(stConfig_Value.fConfigInlineBaseTempLTH, TEMPERATURE_VALUE);
				}
				else if ( METRIC == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = stConfig_Value.fConfigInlineBaseTempLTH;
				}
				stTx1Message.Data[0] = LOW_TH << 1;
			}
			else if ( SET_VALUE == ucTypeOfData )
			{
				if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = fnImperialToMetric_Conv(stConfig_Value.fConfigInlineBaseTempSet, TEMPERATURE_VALUE);
				}
				else if ( METRIC == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = stConfig_Value.fConfigInlineBaseTempSet;
				}
				stTx1Message.Data[0] = SET_VALUE;
			}
			else if ( (HIGH_TH << 1) == ucTypeOfData )
			{
				if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat= fnImperialToMetric_Conv(stConfig_Value.fConfigInlineBaseTempHTH, TEMPERATURE_VALUE);
				}
				else if ( METRIC == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = stConfig_Value.fConfigInlineBaseTempHTH;
				}
				stTx1Message.Data[0] = HIGH_TH << 1;
			}
			else if ( BASE_HYST_POS == ucTypeOfData )
			{
				if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = fnImperialToMetric_Conv(stConfig_Value.fConfigHysInlineBasePos, TEMPERATURE_VALUE);
				}
				else if ( METRIC == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = stConfig_Value.fConfigHysInlineBasePos;
				}
				stTx1Message.Data[0] = BASE_HYST_POS;
			}
			else if ( BASE_HYST_NEG == ucTypeOfData )
			{
				if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = fnImperialToMetric_Conv(stConfig_Value.fConfigHysInlineBaseNeg, TEMPERATURE_VALUE);
				}
				else if ( METRIC == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = stConfig_Value.fConfigHysInlineBaseNeg;
				}
				stTx1Message.Data[0] = BASE_HYST_NEG;
			}
			
			for(ucCnt = 1; ucCnt < stTx1Message.DLC; ucCnt++)
			{
				stTx1Message.Data[ucCnt] =	SplitByte.ucChar[ucCnt - 1];
			}
			/*Send the CONFIG_INLINE_BASE_TEMP_SETTING to MCU*/
			fnCANMessage_Send(CONFIG_INLINE_BASE_TEMP_SETTING, MSG_TYPE_CONFIG, SCU_ID);
		}break;

		case CONFIG_INLINE_HARD_TEMP_SETTING:
		{
			stTx1Message.DLC = 5;
			
			if ( (LOW_TH << 1) == ucTypeOfData )
			{
				if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = fnImperialToMetric_Conv(stConfig_Value.fConfigInlineHardTempLTH, TEMPERATURE_VALUE);
				}
				else if ( METRIC == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = stConfig_Value.fConfigInlineHardTempLTH;
				}
				stTx1Message.Data[0] = LOW_TH << 1;
			}
			else if ( SET_VALUE == ucTypeOfData )
			{
				if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = fnImperialToMetric_Conv(stConfig_Value.fConfigInlineHardTempSet, TEMPERATURE_VALUE);
				}
				else if ( METRIC == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = stConfig_Value.fConfigInlineHardTempSet;
				}
				stTx1Message.Data[0] = SET_VALUE;
			}
			else if ( (HIGH_TH << 1) == ucTypeOfData )
			{
				if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = fnImperialToMetric_Conv(stConfig_Value.fConfigInlineHardTempHTH, TEMPERATURE_VALUE);
				}
				else if ( METRIC == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = stConfig_Value.fConfigInlineHardTempHTH;
				}
				stTx1Message.Data[0] = HIGH_TH << 1;
			}
			else if ( HARD_HYST_POS == ucTypeOfData )
			{
				if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = fnImperialToMetric_Conv(stConfig_Value.fConfigInlineHysHardPos, TEMPERATURE_VALUE);
				}
				else if ( METRIC == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = stConfig_Value.fConfigInlineHysHardPos;
				}
				stTx1Message.Data[0] = HARD_HYST_POS;
			}
			else if ( HARD_HYST_NEG == ucTypeOfData )
			{
				if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = fnImperialToMetric_Conv(stConfig_Value.fConfigInlineHysHardNeg, TEMPERATURE_VALUE);
				}
				else if ( METRIC == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = stConfig_Value.fConfigInlineHysHardNeg;
				}
				stTx1Message.Data[0] = HARD_HYST_NEG;
			}
			
			for(ucCnt = 1; ucCnt < stTx1Message.DLC; ucCnt++)
			{
				stTx1Message.Data[ucCnt] =	SplitByte.ucChar[ucCnt - 1];
			}
			/*Send the CONFIG_INLINE_HARD_TEMP_SETTING to MCU*/
			fnCANMessage_Send(CONFIG_INLINE_HARD_TEMP_SETTING, MSG_TYPE_CONFIG, SCU_ID);
		}break;

		case CONFIG_JOINT_NUMBER:
		{
			stTx1Message.DLC = 4;
			
			SplitByte.fFloat = stConfig_Value.fConfigJointNum;
			
			for(ucCnt = 0; ucCnt < stTx1Message.DLC; ucCnt++)
			{
				stTx1Message.Data[ucCnt] =	SplitByte.ucChar[ucCnt];
			}
			/*Send the CONFIG_JOINT_NUMBER to MCU and OP */
			fnCANMessage_Send(CONFIG_JOINT_NUMBER, MSG_TYPE_CONFIG, SCU_ID);
		}break;
		case CONFIG_DATA_LOG_PERIOD:
		{
			stTx1Message.DLC = 4;
			
			SplitByte.fFloat = stConfig_Value.fConfigDataLoggingPeriod;
			
			for(ucCnt = 0; ucCnt < stTx1Message.DLC; ucCnt++)
			{
				stTx1Message.Data[ucCnt] =	SplitByte.ucChar[ucCnt];
			}
			/*Send the CONFIG_DATA_LOG_PERIOD to MCU and OP */
			fnCANMessage_Send(CONFIG_DATA_LOG_PERIOD, MSG_TYPE_CONFIG, SCU_ID);
		}break;
		case CONFIG_SYS_DATE_TIME:
		{
			stTx1Message.DLC = 7;
		
			stTx1Message.Data[0] = stConfig_Value.ucConfigSystemTime[0];
			stTx1Message.Data[1] = stConfig_Value.ucConfigSystemTime[1];
			stTx1Message.Data[2] = stConfig_Value.ucConfigSystemTime[2];
			stTx1Message.Data[3] = stConfig_Value.ucConfigSystemTime[3];
			stTx1Message.Data[4] = stConfig_Value.ucConfigSystemTime[4];
			stTx1Message.Data[5] = stConfig_Value.ucConfigSystemTime[5];
			stTx1Message.Data[6] = stConfig_Value.ucConfigSystemTime[6];
	
			/*Send the CONFIG_SYS_DATE_TIME to MCU and OP */
			fnCANMessage_Send(CONFIG_SYS_DATE_TIME, MSG_TYPE_CONFIG, SCU_ID);
		}break;
		case CONFIG_CYLINDER_SURFACE_AREA:
		{
			stTx1Message.DLC = 5;

			if ( LEG_A == ucTypeOfData )
			{
				if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = fnImperialToMetric_Conv(stConfig_Value.fConfigAreaofCylinderA, INCH2_TO_MM2);
				}
				else if ( METRIC == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = stConfig_Value.fConfigAreaofCylinderA;
				}
				stTx1Message.Data[0] = LEG_A;
			}
			else if ( LEG_B == ucTypeOfData )
			{
				if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = fnImperialToMetric_Conv(stConfig_Value.fConfigAreaofCylinderB, INCH2_TO_MM2);
				}
				else if ( METRIC == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = stConfig_Value.fConfigAreaofCylinderB;
				}
				stTx1Message.Data[0] = LEG_B;
			}
			else if ( LEG_C == ucTypeOfData )
			{
				if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = fnImperialToMetric_Conv(stConfig_Value.fConfigAreaofCylinderC, INCH2_TO_MM2);
				}
				else if ( METRIC == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = stConfig_Value.fConfigAreaofCylinderC;
				}
				stTx1Message.Data[0] = LEG_C;
			}
			
			for(ucCnt = 1; ucCnt < stTx1Message.DLC; ucCnt++)
			{
				stTx1Message.Data[ucCnt] =	SplitByte.ucChar[ucCnt - 1];
			}
			/*Send the CONFIG_CYLINDER_SURFACE_AREA to MCU and OP */
			fnCANMessage_Send(CONFIG_CYLINDER_SURFACE_AREA, MSG_TYPE_CONFIG, SCU_ID);
		}break;
		case CONFIG_NUMBER_OF_CYLINDER:
		{
			stTx1Message.DLC = 1;			
			stTx1Message.Data[0] =	stConfig_Value.uiConfigCylinderCount;

			/*Send the CONFIG_NUMBER_OF_CYLINDER to MCU and OP */
			fnCANMessage_Send(CONFIG_NUMBER_OF_CYLINDER, MSG_TYPE_CONFIG, SCU_ID);
		}break;
		case CONFIG_SUPERVISOR_NAME:
		{
			stTx1Message.DLC = 8;

			/*FRAME 1*/
				stTx1Message.Data[0] = FRAME_1;
				for(ucCnt = 1; ucCnt < stTx1Message.DLC; ucCnt++)
				{
					stTx1Message.Data[ucCnt] =	stConfig_Value.rgucConfigSupervisorName1[ucCnt - 1];
				}
				/*Send the CONFIG_SUPERVISOR_NAME to MCU and OP */
				fnCANMessage_Send(CONFIG_SUPERVISOR_NAME, MSG_TYPE_CONFIG, SCU_ID);
			
				/*FRAME 2*/
				stTx1Message.Data[0] = FRAME_2;
				
				for(ucCnt = 1; ucCnt < stTx1Message.DLC; ucCnt++)
				{
					stTx1Message.Data[ucCnt] =	stConfig_Value.rgucConfigSupervisorName2[ucCnt - 1];
				}
				/*Send the CONFIG_SUPERVISOR_NAME to MCU and OP */
				fnCANMessage_Send(CONFIG_SUPERVISOR_NAME, MSG_TYPE_CONFIG, SCU_ID);
			
				/*FRAME 3*/
				stTx1Message.Data[0] = FRAME_3;
				
				for(ucCnt = 1; ucCnt < stTx1Message.DLC; ucCnt++)
				{
					stTx1Message.Data[ucCnt] =	stConfig_Value.rgucConfigSupervisorName3[ucCnt - 1];
				}
				/*Send the CONFIG_SUPERVISOR_NAME to MCU and OP */
				fnCANMessage_Send(CONFIG_SUPERVISOR_NAME, MSG_TYPE_CONFIG, SCU_ID);
			
		}break;
		case CONFIG_COATING_MATERIAL:
		{
			stTx1Message.DLC = 8;

			/*FRAME 1*/
				stTx1Message.Data[0] = FRAME_1;
				for(ucCnt = 1; ucCnt < stTx1Message.DLC; ucCnt++)
				{
					stTx1Message.Data[ucCnt] =	stConfig_Value.rgucConfigCoatingMatName1[ucCnt - 1];
				}
				/*Send the CONFIG_COATING_MATERIAL to MCU and OP */
				fnCANMessage_Send(CONFIG_COATING_MATERIAL, MSG_TYPE_CONFIG, SCU_ID);
			
				/*FRAME 2*/
				stTx1Message.Data[0] = FRAME_2;
				
				for(ucCnt = 1; ucCnt < stTx1Message.DLC; ucCnt++)
				{
					stTx1Message.Data[ucCnt] =	stConfig_Value.rgucConfigCoatingMatName2[ucCnt - 1];
				}
				/*Send the CONFIG_COATING_MATERIAL to MCU and OP */
				fnCANMessage_Send(CONFIG_COATING_MATERIAL, MSG_TYPE_CONFIG, SCU_ID);
			
				/*FRAME 3*/
				stTx1Message.Data[0] = FRAME_3;
				
				for(ucCnt = 1; ucCnt < stTx1Message.DLC; ucCnt++)
				{
					stTx1Message.Data[ucCnt] =	stConfig_Value.rgucConfigCoatingMatName3[ucCnt - 1];
				}
				/*Send the CONFIG_COATING_MATERIAL to MCU and OP */
				fnCANMessage_Send(CONFIG_COATING_MATERIAL, MSG_TYPE_CONFIG, SCU_ID);
		}break;
		case CONFIG_BASE_MAT_BATCH_NO:
		{
			stTx1Message.DLC = 8;

			/*FRAME 1*/
				stTx1Message.Data[0] = FRAME_1;
				for(ucCnt = 1; ucCnt < stTx1Message.DLC; ucCnt++)
				{
					stTx1Message.Data[ucCnt] =	stConfig_Value.rgucConfigBaseMatBatchNo1[ucCnt - 1];
				}
				/*Send the CONFIG_BASE_MAT_BATCH_NOto MCU and OP */
				fnCANMessage_Send(CONFIG_BASE_MAT_BATCH_NO, MSG_TYPE_CONFIG, SCU_ID);
			
				/*FRAME 2*/
				stTx1Message.Data[0] = FRAME_2;
				
				for(ucCnt = 1; ucCnt < stTx1Message.DLC; ucCnt++)
				{
					stTx1Message.Data[ucCnt] =	stConfig_Value.rgucConfigBaseMatBatchNo2[ucCnt - 1];
				}
				/*Send the CONFIG_BASE_MAT_BATCH_NO to MCU and OP */
				fnCANMessage_Send(CONFIG_BASE_MAT_BATCH_NO, MSG_TYPE_CONFIG, SCU_ID);
			
				/*FRAME 3*/
				stTx1Message.Data[0] = FRAME_3;
				
				for(ucCnt = 1; ucCnt < stTx1Message.DLC; ucCnt++)
				{
					stTx1Message.Data[ucCnt] =	stConfig_Value.rgucConfigBaseMatBatchNo3[ucCnt - 1];
				}
				/*Send the CONFIG_BASE_MAT_BATCH_NO to MCU and OP */
				fnCANMessage_Send(CONFIG_BASE_MAT_BATCH_NO, MSG_TYPE_CONFIG, SCU_ID);
		}break;
		case CONFIG_HARD_MAT_BATCH_NO:
		{
			stTx1Message.DLC = 8;

			/*FRAME 1*/
				stTx1Message.Data[0] = FRAME_1;
				for(ucCnt = 1; ucCnt < stTx1Message.DLC; ucCnt++)
				{
					stTx1Message.Data[ucCnt] =	stConfig_Value.rgucConfigHardMatBatchNo1[ucCnt - 1];
				}
				/*Send the CONFIG_HARD_MAT_BATCH_NO to MCU and OP */
				fnCANMessage_Send(CONFIG_HARD_MAT_BATCH_NO, MSG_TYPE_CONFIG, SCU_ID);
			
				/*FRAME 2*/
				stTx1Message.Data[0] = FRAME_2;
				
				for(ucCnt = 1; ucCnt < stTx1Message.DLC; ucCnt++)
				{
					stTx1Message.Data[ucCnt] =	stConfig_Value.rgucConfigHardMatBatchNo2[ucCnt - 1];
				}
				/*Send the CONFIG_HARD_MAT_BATCH_NO to MCU and OP */
				fnCANMessage_Send(CONFIG_HARD_MAT_BATCH_NO, MSG_TYPE_CONFIG, SCU_ID);
			
				/*FRAME 3*/
				stTx1Message.Data[0] = FRAME_3;
				
				for(ucCnt = 1; ucCnt < stTx1Message.DLC; ucCnt++)
				{
					stTx1Message.Data[ucCnt] =	stConfig_Value.rgucConfigHardMatBatchNo3[ucCnt - 1];
				}
				/*Send the CONFIG_HARD_MAT_BATCH_NO to MCU and OP */
				fnCANMessage_Send(CONFIG_HARD_MAT_BATCH_NO, MSG_TYPE_CONFIG, SCU_ID);
		}break;
		case CONFIG_TIME_BEFORE_STALL:
		{
			stTx1Message.DLC = 4;
			
			SplitByte.fFloat = stConfig_Value.fConfigTimeBeforeStall;
			
			for(ucCnt = 0; ucCnt < stTx1Message.DLC; ucCnt++)
			{
				stTx1Message.Data[ucCnt] =	SplitByte.ucChar[ucCnt];
			}
			/*Send the CONFIG_TIME_BEFORE_STALL to MCU and OP */
			fnCANMessage_Send(CONFIG_TIME_BEFORE_STALL, MSG_TYPE_CONFIG, SCU_ID);
		}break;
		case CONFIG_TIME_BEFORE_COMPARE_STALL:
		{
			stTx1Message.DLC = 4;
			
			SplitByte.fFloat = stConfig_Value.fConfigTimeCompareStall;
			
			for(ucCnt = 0; ucCnt < stTx1Message.DLC; ucCnt++)
			{
				stTx1Message.Data[ucCnt] =	SplitByte.ucChar[ucCnt];
			}
			/*Send the CONFIG_TIME_BEFORE_COMPARE_STALL to MCU and OP */
			fnCANMessage_Send(CONFIG_TIME_BEFORE_COMPARE_STALL, MSG_TYPE_CONFIG, SCU_ID);
		}break;
		case CONFIG_DCLS_HOME_TIME:
		{
			stTx1Message.DLC = 5;

			if ( DCLS_TIMER == ucTypeOfData )
			{
				SplitByte.fFloat = stConfig_Value.fConfigDCLSHomeTime;
				stTx1Message.Data[0] = DCLS_TIMER;
			}
			else if ( HLS_TIMER == ucTypeOfData )
			{
				SplitByte.fFloat = stConfig_Value.fConfigHomeDCLSTime;
				stTx1Message.Data[0] = HLS_TIMER;
			}
			
			for(ucCnt = 1; ucCnt < stTx1Message.DLC; ucCnt++)
			{
				stTx1Message.Data[ucCnt] =	SplitByte.ucChar[ucCnt - 1];
			}
			/*Send the CONFIG_DCLS_HOME_TIME to MCU and OP */
			fnCANMessage_Send(CONFIG_DCLS_HOME_TIME, MSG_TYPE_CONFIG, SCU_ID);
		}break;
		case CONFIG_GUN_POSITION_CHANGE_TIME:
		{
			stTx1Message.DLC = 4;
			
			SplitByte.fFloat = stConfig_Value.fGunPosSwTime;
			
			for(ucCnt = 0; ucCnt < stTx1Message.DLC; ucCnt++)
			{
				stTx1Message.Data[ucCnt] =	SplitByte.ucChar[ucCnt];
			}
			/*Send the CONFIG_GUN_POSITION_CHANGE_TIME to MCU and OP */
			fnCANMessage_Send(CONFIG_GUN_POSITION_CHANGE_TIME, MSG_TYPE_CONFIG, SCU_ID);
		}break;
		case CONFIG_SPRAY_VALVE_OPEN_TIME_VBAT:
		{
			stTx1Message.DLC = 4;
			
			SplitByte.fFloat = stConfig_Value.fConfigTimebeforeONSprayValve;
			
			for(ucCnt = 0; ucCnt < stTx1Message.DLC; ucCnt++)
			{
				stTx1Message.Data[ucCnt] =	SplitByte.ucChar[ucCnt];
			}
			/*Send the CONFIG_SPRAY_VALVE_OPEN_TIME_VBAT to MCU and OP */
			fnCANMessage_Send(CONFIG_SPRAY_VALVE_OPEN_TIME_VBAT, MSG_TYPE_CONFIG, SCU_ID);
		}break;
		case CONFIG_TYPE_OF_UNIT:
		{
				stTx1Message.DLC = 1;

			if ( METRIC == ucTypeOfData )
			{
				stTx1Message.Data[0] = METRIC;
			}
			else if ( IMPERIAL == ucTypeOfData )
			{
				stTx1Message.Data[0] = IMPERIAL;
			}
		
			/*Send the CONFIG_TYPE_OF_UNIT to MCU and OP */
			fnCANMessage_Send(CONFIG_TYPE_OF_UNIT, MSG_TYPE_CONFIG, SCU_ID);
		}break;			
		case CONFIG_TANK_HEIGHT:
		{
			stTx1Message.DLC = 5;

			if ( BASE_MAT_TANK_HEIGHT == ucTypeOfData )
			{
				if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = fnImperialToMetric_Conv( stConfig_Value.fConfigBaseTankHeight, INCH_TO_MM);
				}
				else if ( METRIC == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = stConfig_Value.fConfigBaseTankHeight;
				}
				stTx1Message.Data[0] = BASE_MAT_TANK_HEIGHT;
			}
			else if ( HARD_MAT_TANK_HEIGHT == ucTypeOfData )
			{
				if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = fnImperialToMetric_Conv( stConfig_Value.fConfigHardTankHeight, INCH_TO_MM);
				}
				else if ( METRIC == stConfig_Value.ucTypeofUnit )
				{
					SplitByte.fFloat = stConfig_Value.fConfigHardTankHeight;
				}
				stTx1Message.Data[0] = HARD_MAT_TANK_HEIGHT;
			}
			
			for(ucCnt = 1; ucCnt < stTx1Message.DLC; ucCnt++)
			{
				stTx1Message.Data[ucCnt] =	SplitByte.ucChar[ucCnt - 1];
			}
			/*Send the CONFIG_LIQUID_TANK_HEIGHT to MCU */
			fnCANMessage_Send(CONFIG_TANK_HEIGHT, MSG_TYPE_CONFIG, SCU_ID);
		}break;
		
		case CONFIG_LL_SAMPLE_RATE:
		{
			stTx1Message.DLC = 1;

			stTx1Message.Data[0] = stConfig_Value.uiConfigLLWindowSize / 100;
		
			fnCANMessage_Send(CONFIG_LL_SAMPLE_RATE, MSG_TYPE_CONFIG, SCU_ID);
		}break;
		
		case CONFIG_FLUSH_WARNING_TIME:
		{
			stTx1Message.DLC = 4;
			
			SplitByte.fFloat = stConfig_Value.fConfigFlushWarnTime;
			
			for(ucCnt = 0; ucCnt < stTx1Message.DLC; ucCnt++)
			{
				stTx1Message.Data[ucCnt] =	SplitByte.ucChar[ucCnt];
			}
			
			fnCANMessage_Send(CONFIG_FLUSH_WARNING_TIME, MSG_TYPE_CONFIG, SCU_ID);
		}break;
		
		case CONFIG_PGAUGE_REMAP:
		{
			stTx1Message.DLC = 3;			

			stTx1Message.Data[0] =	stConfig_Value.ucConfigConnPressureGuage1;
			stTx1Message.Data[1] =	stConfig_Value.ucConfigConnPressureGuage2;
			stTx1Message.Data[2] =	stConfig_Value.ucConfigConnPressureGuage3;
			
			fnCANMessage_Send(CONFIG_PGAUGE_REMAP, MSG_TYPE_CONFIG, SCU_ID);
		}break;
		case CONFIG_LT_REMAP:
		{
			stTx1Message.DLC = 1;			

			stTx1Message.Data[0] =	stConfig_Value.ucConfigConnLinearTransducer;

			fnCANMessage_Send(CONFIG_LT_REMAP, MSG_TYPE_CONFIG, SCU_ID);
		}break;
		case CONFIG_EE07_REMAP:
		{
			stTx1Message.DLC = 2;			

			stTx1Message.Data[0] =	stConfig_Value.ucConfigConnInsideEE07;
			stTx1Message.Data[1] =	stConfig_Value.ucConfigConnOutsideEE07;
			
			fnCANMessage_Send(CONFIG_EE07_REMAP, MSG_TYPE_CONFIG, SCU_ID);
		}break;
		case CONFIG_STALL_VALVE_REMAP:
		{/*change12*/
			stTx1Message.DLC = 2;			

			stTx1Message.Data[0] =	stConfig_Value.ucConfigConnStallvalve;
			stTx1Message.Data[1] =	stConfig_Value.ucConfigConnRecircvalve;
			fnCANMessage_Send(CONFIG_STALL_VALVE_REMAP, MSG_TYPE_CONFIG, SCU_ID);
		}break;
		case CONFIG_THERMOCOUPLE_REMAP:
		{
			stTx1Message.DLC = 4;			

			stTx1Message.Data[0] =	stConfig_Value.ucConfig_ConnBaseInlineTher;
			stTx1Message.Data[1] =	stConfig_Value.ucConfig_ConnHardInlineTher;
			stTx1Message.Data[2] =	stConfig_Value.ucConfig_ConnAntifreezeTher;
			stTx1Message.Data[3] =	stConfig_Value.ucConfig_ConnBasereturnlineTher;

			fnCANMessage_Send(CONFIG_THERMOCOUPLE_REMAP, MSG_TYPE_CONFIG, SCU_ID);
		}break;
		case CONFIG_SSR_REMAP:
		{
			stTx1Message.DLC = 4;			

			stTx1Message.Data[0] =	stConfig_Value.ucConfig_ConnBaseinlineHeater;
			stTx1Message.Data[1] =	stConfig_Value.ucConfig_ConnHardinlineHeater;
			stTx1Message.Data[2] =	stConfig_Value.ucConfig_ConnAntifreezeHeater;
			stTx1Message.Data[3] =	stConfig_Value.ucConfig_ConnHardHeater;

			fnCANMessage_Send(CONFIG_SSR_REMAP, MSG_TYPE_CONFIG, SCU_ID);
		}break;
		case CONFIG_LL_SENSOR_REMAP:
		{
			stTx1Message.DLC = 2;			

			stTx1Message.Data[0] =	stConfig_Value.ucConfig_ConnBaseTankLL;
			stTx1Message.Data[1] =	stConfig_Value.ucConfig_ConnHardTankLL;

			fnCANMessage_Send(CONFIG_LL_SENSOR_REMAP, MSG_TYPE_CONFIG, SCU_ID);
		}break;
		case CONFIG_OPMM_VALVE_REMAP:
		{
			stTx1Message.DLC = 6;			

			stTx1Message.Data[0] =	stConfig_Value.ucConfigConnSprayvalve;
			stTx1Message.Data[1] =	stConfig_Value.ucConfigConnGunposition;
			stTx1Message.Data[2] =	stConfig_Value.ucConfigConnABvalve;
			stTx1Message.Data[3] =	stConfig_Value.ucConfigConnSolventvalve;
			stTx1Message.Data[4] =	stConfig_Value.ucConfigConnForwardvalve;
			stTx1Message.Data[5] =	stConfig_Value.ucConfigConnBackwardvalve;
			
			fnCANMessage_Send(CONFIG_OPMM_VALVE_REMAP, MSG_TYPE_CONFIG, SCU_ID);
		}break;
		case CONFIG_VCM_PERIPHRAL_BYPASS:
		{
			stTx1Message.DLC = 1;			

			stTx1Message.Data[0] =	(stConfig_Value.uiConfigVCMBypass & 0xFF);

			fnCANMessage_Send(CONFIG_VCM_PERIPHRAL_BYPASS, MSG_TYPE_CONFIG, SCU_ID);
		}break;
		case CONFIG_HCM_PERIPHRAL_BYPASS:
		{
			stTx1Message.DLC = 2;			

			stTx1Message.Data[0] =	(stConfig_Value.uiConfigHCMBypass & 0xFF);
			stTx1Message.Data[1] =	((stConfig_Value.uiConfigHCMBypass & 0xFF00) >> 8);

			fnCANMessage_Send(CONFIG_HCM_PERIPHRAL_BYPASS, MSG_TYPE_CONFIG, SCU_ID);
		}break;
		case CONFIG_SCU_PERIPHRAL_BYPASS:
		{
			stTx1Message.DLC = 1;			

			stTx1Message.Data[0] =	(stConfig_Value.uiConfigSCUBypass & 0xFF);

			fnCANMessage_Send(CONFIG_SCU_PERIPHRAL_BYPASS, MSG_TYPE_CONFIG, SCU_ID);
		}break;
		case CONFIG_OPMM_PERIPHRAL_BYPASS:
		{
			stTx1Message.DLC = 2;			

			stTx1Message.Data[0] =	(stConfig_Value.uiConfigOPMMBypass & 0xFF);
			stTx1Message.Data[1] =	((stConfig_Value.uiConfigOPMMBypass & 0xFF00) >> 8);

			fnCANMessage_Send(CONFIG_OPMM_PERIPHRAL_BYPASS, MSG_TYPE_CONFIG, SCU_ID);
		}break;
		case CONFIG_NON_CONTACT_TEMP_REMAP:
		{
			stTx1Message.DLC = 4;			

			stTx1Message.Data[0] =	stConfig_Value.ucConfigNCTempSensor1;
			stTx1Message.Data[1] =	stConfig_Value.ucConfigNCTempSensor2;
			stTx1Message.Data[2] =	stConfig_Value.ucConfigNCTempSensor3;
			stTx1Message.Data[3] =	stConfig_Value.ucConfigNCTempSensor4;

			fnCANMessage_Send(CONFIG_NON_CONTACT_TEMP_REMAP, MSG_TYPE_CONFIG, SCU_ID);
		}break;
		case CONFIG_TEMP_PRES_ACHIEVE_BYPASS:
		{
			stTx1Message.DLC = 1;			

			if(TRUE == StatusFlag.stStatusFlag.bSCU_Temp_Bypass)
			{
				stTx1Message.Data[0] |=	BIT_0;
			}else
			{
				stTx1Message.Data[0] &=	BIT_0;
			}
			
			if(TRUE == StatusFlag.stStatusFlag.bSCU_Pressure_Bypass)
			{
				stTx1Message.Data[0] |=	BIT_1;
			}else
			{
				stTx1Message.Data[0] &=	BIT_1;
			}

			fnCANMessage_Send(CONFIG_TEMP_PRES_ACHIEVE_BYPASS, MSG_TYPE_CONFIG, SCU_ID);
		}break;
		
		case CONFIG_OPMM_BUZZER_VOLUME:
		{
			stTx1Message.DLC = 1;

			stTx1Message.Data[0] =	stConfig_Value.ucBuzzerVolume * 10;

			fnCANMessage_Send(CONFIG_OPMM_BUZZER_VOLUME, MSG_TYPE_CONFIG, SCU_ID);
		}break;
		case CONFIG_SIGNATURE_BYTE:
		{
			stTx1Message.DLC = 2;
			SplitByte.uiData = stConfig_Value.uiSignature;
			
			for(ucCnt = 0; ucCnt < stTx1Message.DLC; ucCnt++)
			{
				stTx1Message.Data[ucCnt] =	SplitByte.ucChar[ucCnt];
			}

			fnCANMessage_Send(CONFIG_SIGNATURE_BYTE, MSG_TYPE_CONFIG, SCU_ID);
		}break;
		
		/*3rdMarch*/
		case	CONFIG_BAT_KNEE_VOLTAGE:
		{
			stTx1Message.DLC = 4;
			
			SplitByte.fFloat = stConfig_Value.fOPMMBatKneeVoltage;
			
			for(ucCnt = 0; ucCnt < stTx1Message.DLC; ucCnt++)
			{
				stTx1Message.Data[ucCnt] =	SplitByte.ucChar[ucCnt];
			}
			
			fnCANMessage_Send(CONFIG_BAT_KNEE_VOLTAGE, MSG_TYPE_CONFIG, SCU_ID);				
		}break;
		
		case CONFIG_TIME_TO_START_STALL:
		{
			stTx1Message.DLC = 4;
			
			SplitByte.fFloat = stConfig_Value.fTimerStartStallTest;
			
			for(ucCnt = 0; ucCnt < stTx1Message.DLC; ucCnt++)
			{
				stTx1Message.Data[ucCnt] =	SplitByte.ucChar[ucCnt];
			}
			
			fnCANMessage_Send(CONFIG_TIME_TO_START_STALL, MSG_TYPE_CONFIG, SCU_ID);
		}break;
	
		case CONFIG_STOP_MOTOR_ROTATION_TIME:
		{
			stTx1Message.DLC = 4;
			
			SplitByte.fFloat = stConfig_Value.fTimerSprayLastRotStop;
			
			for(ucCnt = 0; ucCnt < stTx1Message.DLC; ucCnt++)
			{
				stTx1Message.Data[ucCnt] =	SplitByte.ucChar[ucCnt];
			}
			
			fnCANMessage_Send(CONFIG_STOP_MOTOR_ROTATION_TIME, MSG_TYPE_CONFIG, SCU_ID);
		}break;
		
		case CONFIG_STALL_TEST_LT_RANGES:
		{
			stTx1Message.DLC = 2;
			
			if(UPSTROKE_LIMIT == ucTypeOfData)
			{
				stTx1Message.Data[0] = ucTypeOfData;
				if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
				{
					stTx1Message.Data[1] = (uint8_t)fnImperialToMetric_Conv(stConfig_Value.fUpstrokeLimit, INCH_TO_MM);
				}
				else if ( METRIC == stConfig_Value.ucTypeofUnit )
				{
					stTx1Message.Data[1] = (uint8_t)stConfig_Value.fUpstrokeLimit;
				}
			}
			else if(UPSTROKE_MIN == ucTypeOfData)
			{
				stTx1Message.Data[0] = ucTypeOfData;
				if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
				{
					stTx1Message.Data[1] = (uint8_t)fnImperialToMetric_Conv(stConfig_Value.fUpstrokeMin, INCH_TO_MM);
				}
				else if ( METRIC == stConfig_Value.ucTypeofUnit )
				{
					stTx1Message.Data[1] = (uint8_t)stConfig_Value.fUpstrokeMin;
				}
			}
			else if(UPSTROKE_MAX == ucTypeOfData)
			{
				stTx1Message.Data[0] = ucTypeOfData;
				if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
				{
					stTx1Message.Data[1] = (uint8_t)fnImperialToMetric_Conv(stConfig_Value.fUpstrokeMax, INCH_TO_MM);
				}
				else if ( METRIC == stConfig_Value.ucTypeofUnit )
				{
					stTx1Message.Data[1] = (uint8_t)stConfig_Value.fUpstrokeMax;
				}
			}
			else if(DOWNSTROKE_LIMIT == ucTypeOfData)
			{
				stTx1Message.Data[0] = ucTypeOfData;
				if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
				{
					stTx1Message.Data[1] = (uint8_t)fnImperialToMetric_Conv(stConfig_Value.fDownstrokeLimit, INCH_TO_MM);
				}
				else if ( METRIC == stConfig_Value.ucTypeofUnit )
				{
					stTx1Message.Data[1] = (uint8_t)stConfig_Value.fDownstrokeLimit;
				}
			}
			else if(DOWNSTROKE_MIN == ucTypeOfData)
			{
				stTx1Message.Data[0] = ucTypeOfData;
				if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
				{
					stTx1Message.Data[1] = (uint8_t)fnImperialToMetric_Conv(stConfig_Value.fDownstrokeMin, INCH_TO_MM);
				}
				else if ( METRIC == stConfig_Value.ucTypeofUnit )
				{
					stTx1Message.Data[1] = (uint8_t)stConfig_Value.fDownstrokeMin;
				}
			}
			else if(DOWNSTROKE_MAX == ucTypeOfData)		
			{
				stTx1Message.Data[0] = ucTypeOfData;
				if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
				{
					stTx1Message.Data[1] = (uint8_t)fnImperialToMetric_Conv(stConfig_Value.fDownstrokeMax, INCH_TO_MM);
				}
				else if ( METRIC == stConfig_Value.ucTypeofUnit )
				{
					stTx1Message.Data[1] = (uint8_t)stConfig_Value.fDownstrokeMax;
				}
			}				
			
			fnCANMessage_Send(CONFIG_STALL_TEST_LT_RANGES, MSG_TYPE_CONFIG, SCU_ID);		
		}break;
		
		case CONFIG_IDEAL_PERIODIC_TIME:
		{
			stTx1Message.DLC = 4;
			
			SplitByte.fFloat = stConfig_Value.fTimerIdealPeriodic;
			
			for(ucCnt = 0; ucCnt < stTx1Message.DLC; ucCnt++)
			{
				stTx1Message.Data[ucCnt] =	SplitByte.ucChar[ucCnt];
			}
			
			fnCANMessage_Send(CONFIG_IDEAL_PERIODIC_TIME, MSG_TYPE_CONFIG, SCU_ID);
		}break;
		
		case CONFIG_JOB_NAME:
		{
			stTx1Message.DLC = 8;

			/*FRAME 1*/
				stTx1Message.Data[0] = FRAME_1;
				for(ucCnt = 1; ucCnt < stTx1Message.DLC; ucCnt++)
				{
					stTx1Message.Data[ucCnt] =	stConfig_Value.rgucJobName1[ucCnt - 1];
				}
				/*Send the CONFIG_JOB_NAME to MCU and OP */
				fnCANMessage_Send(CONFIG_JOB_NAME, MSG_TYPE_CONFIG, SCU_ID);
			
				/*FRAME 2*/
				stTx1Message.Data[0] = FRAME_2;
				
				for(ucCnt = 1; ucCnt < stTx1Message.DLC; ucCnt++)
				{
					stTx1Message.Data[ucCnt] =	stConfig_Value.rgucJobName2[ucCnt - 1];
				}
				/*Send the CONFIG_JOB_NAME to MCU and OP */
				fnCANMessage_Send(CONFIG_JOB_NAME, MSG_TYPE_CONFIG, SCU_ID);
			
				/*FRAME 3*/
				stTx1Message.Data[0] = FRAME_3;
				
				for(ucCnt = 1; ucCnt < stTx1Message.DLC; ucCnt++)
				{
					stTx1Message.Data[ucCnt] =	stConfig_Value.rgucJobName3[ucCnt - 1];
				}
				/*Send the CONFIG_JOB_NAME to MCU and OP */
				fnCANMessage_Send(CONFIG_JOB_NAME, MSG_TYPE_CONFIG, SCU_ID);
		}break;
		
		case CONFIG_PIPE_DIAMETER:
		{
			stTx1Message.DLC = 4;
			
			SplitByte.fFloat = stConfig_Value.fPipeDiameter;
			
			for(ucCnt = 0; ucCnt < stTx1Message.DLC; ucCnt++)
			{
				stTx1Message.Data[ucCnt] =	SplitByte.ucChar[ucCnt];
			}
			
			fnCANMessage_Send(CONFIG_PIPE_DIAMETER, MSG_TYPE_CONFIG, SCU_ID);
		}break;
		case CONFIG_UTC_TIME_OFFSET:
		{
			DEBUG_PRINTF("CONFIG_UTC_TIME_OFFSET \r\n");
			stTx1Message.DLC = 4;
			stTx1Message.Data[DATA_BYTE_ZERO]= stConfig_Value.ucConfig_TimeOffsetSign;
			stTx1Message.Data[DATA_BYTE_ONE]= stConfig_Value.ucConfig_TimeOffsetHour;
			stTx1Message.Data[DATA_BYTE_TWO]= stConfig_Value.ucConfig_TimeOffsetMin ;
			stTx1Message.Data[DATA_BYTE_THREE]= stConfig_Value.ucConfig_TimeOffsetSec;
			/*Send the CONFIG_TIME_BEFORE_STALL to MCU and OP */
			fnCANMessage_Send(CONFIG_UTC_TIME_OFFSET, MSG_TYPE_CONFIG, SCU_ID);
		}break;
		/*change3*/
		case CONFIG_ETH_WIFI_IP_SN_GW_PN:
		{
			DEBUG_PRINTF("CONFIG_ETH_WIFI_IP_SN_GW_PN \r\n");
			stTx1Message.DLC = 8;
			if(ETHERNETIP == ucTypeOfData)
			{
			stTx1Message.Data[DATA_BYTE_ZERO]	= BIT_0;
			stTx1Message.Data[DATA_BYTE_ONE] 	= BIT_0;
			stTx1Message.Data[DATA_BYTE_TWO] 	= stCurrent_Value.ucEthIPSlot1;
			stTx1Message.Data[DATA_BYTE_THREE]= stCurrent_Value.ucEthIPSlot2;
			stTx1Message.Data[DATA_BYTE_FOUR]	= stCurrent_Value.ucEthIPSlot3;
			stTx1Message.Data[DATA_BYTE_FIVE]	= stCurrent_Value.ucEthIPSlot4;				
			/*Send the CONFIG_TIME_BEFORE_STALL to MCU and OP */
			fnCANMessage_Send(CONFIG_ETH_WIFI_IP_SN_GW_PN, MSG_TYPE_CONFIG, SCU_ID);
			}
			else if(ETHERNETSUBNET == ucTypeOfData)
			{
			stTx1Message.Data[DATA_BYTE_ZERO]	= BIT_0;
			stTx1Message.Data[DATA_BYTE_ONE] 	= BIT_1;
			stTx1Message.Data[DATA_BYTE_TWO] 	= stCurrent_Value.ucEthSubnetSlot1;
			stTx1Message.Data[DATA_BYTE_THREE]= stCurrent_Value.ucEthSubnetSlot2;
			stTx1Message.Data[DATA_BYTE_FOUR]	= stCurrent_Value.ucEthSubnetSlot3;
			stTx1Message.Data[DATA_BYTE_FIVE]	= stCurrent_Value.ucEthSubnetSlot4;				
			/*Send the CONFIG_TIME_BEFORE_STALL to MCU and OP */
			fnCANMessage_Send(CONFIG_ETH_WIFI_IP_SN_GW_PN, MSG_TYPE_CONFIG, SCU_ID);
			}
			else if(ETHERNETGW == ucTypeOfData)
			{
			stTx1Message.Data[DATA_BYTE_ZERO]	= BIT_0;
			stTx1Message.Data[DATA_BYTE_ONE] 	= BIT_2;
			stTx1Message.Data[DATA_BYTE_TWO] 	= stCurrent_Value.ucEthGWSlot1;
			stTx1Message.Data[DATA_BYTE_THREE]= stCurrent_Value.ucEthGWSlot2;
			stTx1Message.Data[DATA_BYTE_FOUR]	= stCurrent_Value.ucEthGWSlot3;
			stTx1Message.Data[DATA_BYTE_FIVE]	= stCurrent_Value.ucEthGWSlot4;				
			/*Send the CONFIG_TIME_BEFORE_STALL to MCU and OP */
			fnCANMessage_Send(CONFIG_ETH_WIFI_IP_SN_GW_PN, MSG_TYPE_CONFIG, SCU_ID);
			}
			else if(ETHERNETPORTNUMBER == ucTypeOfData)
			{
			stTx1Message.Data[DATA_BYTE_ZERO]	= BIT_0;
			stTx1Message.Data[DATA_BYTE_ONE] 	= BIT_3;
			SplitByte.uiData = stCurrent_Value.iEthPort;							
			stTx1Message.Data[DATA_BYTE_SIX] 	= SplitByte.ucChar[0];
			stTx1Message.Data[DATA_BYTE_SEVEN]=	SplitByte.ucChar[1];
			fnCANMessage_Send(CONFIG_ETH_WIFI_IP_SN_GW_PN, MSG_TYPE_CONFIG, SCU_ID);
			}			
		else if(WIFIIP == ucTypeOfData)
			{
			stTx1Message.Data[DATA_BYTE_ZERO]	= BIT_1;
			stTx1Message.Data[DATA_BYTE_ONE] 	= BIT_0;
			stTx1Message.Data[DATA_BYTE_TWO] 	= stCurrent_Value.ucWiFiIPSlot1;
			stTx1Message.Data[DATA_BYTE_THREE]= stCurrent_Value.ucWiFiIPSlot2;
			stTx1Message.Data[DATA_BYTE_FOUR]	= stCurrent_Value.ucWiFiIPSlot3;
			stTx1Message.Data[DATA_BYTE_FIVE]	= stCurrent_Value.ucWiFiIPSlot4;				
			/*Send the CONFIG_TIME_BEFORE_STALL to MCU and OP */
			fnCANMessage_Send(CONFIG_ETH_WIFI_IP_SN_GW_PN, MSG_TYPE_CONFIG, SCU_ID);
			}
			else if(WIFISUBNET == ucTypeOfData)
			{
			stTx1Message.Data[DATA_BYTE_ZERO]	= BIT_1;
			stTx1Message.Data[DATA_BYTE_ONE] 	= BIT_1;
			stTx1Message.Data[DATA_BYTE_TWO] 	= stCurrent_Value.ucWiFiSubnetSlot1;
			stTx1Message.Data[DATA_BYTE_THREE]= stCurrent_Value.ucWiFiSubnetSlot2;
			stTx1Message.Data[DATA_BYTE_FOUR]	= stCurrent_Value.ucWiFiSubnetSlot3;
			stTx1Message.Data[DATA_BYTE_FIVE]	= stCurrent_Value.ucWiFiSubnetSlot4;				
			/*Send the CONFIG_TIME_BEFORE_STALL to MCU and OP */
			fnCANMessage_Send(CONFIG_ETH_WIFI_IP_SN_GW_PN, MSG_TYPE_CONFIG, SCU_ID);
			}
			else if(WIFIGW == ucTypeOfData)
			{
			stTx1Message.Data[DATA_BYTE_ZERO]	= BIT_1;
			stTx1Message.Data[DATA_BYTE_ONE] 	= BIT_2;
			stTx1Message.Data[DATA_BYTE_TWO] 	= stCurrent_Value.ucWiFiGWSlot1;
			stTx1Message.Data[DATA_BYTE_THREE]= stCurrent_Value.ucWiFiGWSlot2;
			stTx1Message.Data[DATA_BYTE_FOUR]	= stCurrent_Value.ucWiFiGWSlot3;
			stTx1Message.Data[DATA_BYTE_FIVE]	= stCurrent_Value.ucWiFiGWSlot4;				
			/*Send the CONFIG_TIME_BEFORE_STALL to MCU and OP */
			fnCANMessage_Send(CONFIG_ETH_WIFI_IP_SN_GW_PN, MSG_TYPE_CONFIG, SCU_ID);
			}
			else if(WIFIPORTNUMBER == ucTypeOfData)
			{
			stTx1Message.Data[DATA_BYTE_ZERO]	= BIT_1;
			stTx1Message.Data[DATA_BYTE_ONE] 	= BIT_3;
			SplitByte.uiData = stCurrent_Value.iWiFiPort;							
			stTx1Message.Data[DATA_BYTE_SIX] 	= SplitByte.ucChar[0];
			stTx1Message.Data[DATA_BYTE_SEVEN]=	SplitByte.ucChar[1];
			fnCANMessage_Send(CONFIG_ETH_WIFI_IP_SN_GW_PN, MSG_TYPE_CONFIG, SCU_ID);
			}	
		}break;
		
			case CONFIG_WIFI_SSID:
		{
			stTx1Message.DLC = 8;
				
			/*FRAME 1*/
				stTx1Message.Data[0] = FRAME_1;
				for(ucCnt = 1; ucCnt < stTx1Message.DLC; ucCnt++)
				{
					stTx1Message.Data[ucCnt] =	stConfig_Value.rgucSSID1[ucCnt - 1];
				}
				/*Send the CONFIG_SUPERVISOR_NAME to MCU and OP */
			//	DEBUG_PRINTF("%s \r\n",stConfig_Value.rgucSSID1);		
				fnCANMessage_Send(CONFIG_WIFI_SSID, MSG_TYPE_CONFIG, SCU_ID);
			
				/*FRAME 2*/
				stTx1Message.Data[0] = FRAME_2;
				
				for(ucCnt = 1; ucCnt < stTx1Message.DLC; ucCnt++)
				{
					stTx1Message.Data[ucCnt] =	stConfig_Value.rgucSSID2[ucCnt - 1];
				}
			//	DEBUG_PRINTF("%s \r\n",stConfig_Value.rgucSSID2);	
				/*Send the CONFIG_SUPERVISOR_NAME to MCU and OP */
				fnCANMessage_Send(CONFIG_WIFI_SSID, MSG_TYPE_CONFIG, SCU_ID);
			
				
			
		}break;
			case CONFIG_WIFI_PASS:
		{
			stTx1Message.DLC = 8;

			/*FRAME 1*/
				stTx1Message.Data[0] = FRAME_1;
				for(ucCnt = 1; ucCnt < stTx1Message.DLC; ucCnt++)
				{
					stTx1Message.Data[ucCnt] =	stConfig_Value.rgucPass1[ucCnt - 1];
				}
		//		DEBUG_PRINTF("%s \r\n",stConfig_Value.rgucPass1);	
				/*Send the CONFIG_SUPERVISOR_NAME to MCU and OP */
				fnCANMessage_Send(CONFIG_WIFI_PASS, MSG_TYPE_CONFIG, SCU_ID);
			
				/*FRAME 2*/
				stTx1Message.Data[0] = FRAME_2;
				
				for(ucCnt = 1; ucCnt < stTx1Message.DLC; ucCnt++)
				{
					stTx1Message.Data[ucCnt] =	stConfig_Value.rgucPass2[ucCnt - 1];
				}
			//	DEBUG_PRINTF("%s \r\n",stConfig_Value.rgucPass2);	
				/*Send the CONFIG_SUPERVISOR_NAME to MCU and OP */
				fnCANMessage_Send(CONFIG_WIFI_PASS, MSG_TYPE_CONFIG, SCU_ID);
			
				
			
		}break;
		/*change7*/
		case CONFIG_JOB_ID:
		{
			stTx1Message.DLC = 8;

			/*FRAME 1*/
				stTx1Message.Data[0] = FRAME_1;
				for(ucCnt = 1; ucCnt < stTx1Message.DLC; ucCnt++)
				{
					stTx1Message.Data[ucCnt] =	stConfig_Value.rgucJobId1[ucCnt - 1];
				}
				/*Send the CONFIG_SUPERVISOR_NAME to MCU and OP */
				fnCANMessage_Send(CONFIG_JOB_ID, MSG_TYPE_CONFIG, SCU_ID);
			
				/*FRAME 2*/
				stTx1Message.Data[0] = FRAME_2;
				
				for(ucCnt = 1; ucCnt < stTx1Message.DLC; ucCnt++)
				{
					stTx1Message.Data[ucCnt] =	stConfig_Value.rgucJobId2[ucCnt - 1];
				}
				/*Send the CONFIG_SUPERVISOR_NAME to MCU and OP */
				fnCANMessage_Send(CONFIG_JOB_ID, MSG_TYPE_CONFIG, SCU_ID);
			
				/*FRAME 3*/
				stTx1Message.Data[0] = FRAME_3;
				
				for(ucCnt = 1; ucCnt < stTx1Message.DLC; ucCnt++)
				{
					stTx1Message.Data[ucCnt] =	stConfig_Value.rgucJobId3[ucCnt - 1];
				}
				/*Send the CONFIG_SUPERVISOR_NAME to MCU and OP */
				fnCANMessage_Send(CONFIG_JOB_ID, MSG_TYPE_CONFIG, SCU_ID);
			
		}break;
		default:
			break;
	}
}


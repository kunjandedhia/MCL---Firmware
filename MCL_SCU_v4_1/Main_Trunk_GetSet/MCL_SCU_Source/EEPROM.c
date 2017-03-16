/******************************************************************************
**@file     :    EEPROM.c
* 
**@brief    :   this file contain funtions related to Config Parameter(EEPROM)
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
// uint16_t uiSignature; 
uint16_t uiBakSignature;
uint8_t ucEEPROMCounter = 0;
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
void fnsetDefaultvalues(void);
/*****************************************************************************
**@Function 			: fnSPI_ReadWrite
**@Descriptions	: write command to MCP2515.
**@parameters		: SPIx : SPI Peripheral
`								rgucData: command or data to write
**@return				:	data read from MCP2515
*****************************************************************************/
uint8_t fnSPI_ReadWrite(SPI_TypeDef* pstSPIx, uint8_t ucData)   
{    
  uint8_t ucRet = 0;
  SPI_I2S_SendData(pstSPIx, ucData);  
  
  while(SET != SPI_I2S_GetFlagStatus(pstSPIx, SPI_I2S_FLAG_TXE));
  while(SET != SPI_I2S_GetFlagStatus(pstSPIx, SPI_I2S_FLAG_RXNE));
  ucRet = SPI_I2S_ReceiveData(pstSPIx);
  return ucRet;
}  

/*****************************************************************************
**@Function 			: 	fnSPIWrite_enable
**@Descriptions  :   Send Write enable Instruction to EEPROM.
**@parameters    :   None
**@Returnval 		:   None
*****************************************************************************/
void fnSPIWrite_enable(void)
{
  uint16_t uiLoop;
  while (SET == SPI_I2S_GetFlagStatus(SPIX_PERIPH, SPI_I2S_FLAG_BSY));
  /* CS=0*/
  GPIO_WriteBit(EEPROM_CHIP_SELECT_PORT, EEPROM_CHIP_SELECT, Bit_RESET); 
  for(uiLoop = 100; uiLoop > 0 ; uiLoop--);	 //change6	
  /******* Write Enable *******/
  fnSPI_ReadWrite(SPIX_PERIPH, WREN);
  /* CS=1*/
  GPIO_WriteBit(EEPROM_CHIP_SELECT_PORT, EEPROM_CHIP_SELECT, Bit_SET); 
  for(uiLoop = 100; uiLoop > 0 ; uiLoop--);	 //change6	
}

/*****************************************************************************
**@Function 			: 	fnSPIWrite_Disable
**@Descriptions  :   Send Write Disbale Instruction to EEPROM.
**@parameters    :   None
**@Returnval 		:   None
*****************************************************************************/
void fnSPIWrite_Disable(void)
{
  while (SET == SPI_I2S_GetFlagStatus(SPIX_PERIPH, SPI_I2S_FLAG_BSY));
  /* CS = 0*/
  GPIO_WriteBit(EEPROM_CHIP_SELECT_PORT, EEPROM_CHIP_SELECT, Bit_RESET);
  
  /******* Write Enable *******/
  fnSPI_ReadWrite(SPIX_PERIPH, WRDI);
  /* CS = 0*/
  GPIO_WriteBit(EEPROM_CHIP_SELECT_PORT, EEPROM_CHIP_SELECT, Bit_SET); 
}


/*****************************************************************************
**@Function 			:  fnSPI_Read_Status_page
**@Descriptions	:  Reads the Page status of the EEPROM
**@parameters		:  None
**@Return 				:  Page status
*****************************************************************************/
uint8_t fnSPI_Read_Status_page(void)
{
  uint8_t rgucData;
  uint8_t ucDummy;
  /* CS = 0	*/
  GPIO_WriteBit(EEPROM_CHIP_SELECT_PORT, EEPROM_CHIP_SELECT, Bit_RESET); 
  /******* Read Status Register *******/
  fnSPI_ReadWrite(SPIX_PERIPH,RDSR);
  ucDummy = DUMMY_DATA;
  
  /******* Read a Data *******/
  rgucData = fnSPI_ReadWrite(SPIX_PERIPH,ucDummy);
  /* CS = 1*/
  GPIO_WriteBit(EEPROM_CHIP_SELECT_PORT, EEPROM_CHIP_SELECT, Bit_SET); 
  return rgucData;
}


/*****************************************************************************
**@Function 		  : fnWrite_Status_page
**@Descriptions	: Reads the Page status of the EEPROM
**@parameters		: None
**@Return 				: Page status
*****************************************************************************/
uint8_t fnWrite_Status_page(void)
{
  uint16_t uiDummy1;
  /* CS = 0*/
  GPIO_WriteBit(EEPROM_CHIP_SELECT_PORT, EEPROM_CHIP_SELECT, Bit_RESET); 
  
  SPI_I2S_SendData(SPIX_PERIPH, 0x01);
  while(SET != SPI_I2S_GetFlagStatus(SPIX_PERIPH, SPI_I2S_FLAG_TXE));
  while(SET == SPI_I2S_GetFlagStatus(SPIX_PERIPH, SPI_I2S_FLAG_RXNE));
  uiDummy1 = SPI_I2S_ReceiveData(SPIX_PERIPH);
  
  SPI_I2S_SendData(SPIX_PERIPH,0x02);
  while(SET != SPI_I2S_GetFlagStatus(SPIX_PERIPH, SPI_I2S_FLAG_TXE));
  while(SET == SPI_I2S_GetFlagStatus(SPIX_PERIPH, SPI_I2S_FLAG_RXNE));
  uiDummy1 = SPI_I2S_ReceiveData(SPIX_PERIPH);
  /* CS = 1*/
  GPIO_WriteBit(EEPROM_CHIP_SELECT_PORT, EEPROM_CHIP_SELECT, Bit_SET); 
#ifdef GENERAL_DBG_EE
  printf("fnRead_Status_page out!\r\n");
#endif
  return uiDummy1;
}


/**********************************************************************************
**@Function 		: fnSPI_EEPROM_WriteData
**@Descriptions	: This function is write data in EEPROM when write command Recieved
**@parameters		:	uiAddress       start of Memory location
ucLength        Number of bytes write
*pucReadData     pointer to write data bytes
**@return				:	None										 
*************************************************************************************/
void fnSPI_EEPROM_WriteData(uint16_t uiAddress, uint8_t ucLength, uint8_t *pucWriteData)
{
  uint8_t ucData;
  uint8_t uclowerADD = 0, ucupperADD = 0;
  uint16_t uiLoop;
  
  uclowerADD = (uint8_t)(uiAddress & 0x00ff);
  ucupperADD = (uint8_t)((uiAddress >> 8) & 0x00ff);
  
  for(uiLoop = 500; uiLoop > 0 ; uiLoop--);	 //change6	
  /****** Write Enable ******/
  fnSPIWrite_enable();
  /* CS = 0*/
  GPIO_WriteBit(EEPROM_CHIP_SELECT_PORT, EEPROM_CHIP_SELECT, Bit_RESET); 
  
  /******* WRITE Instruction *******/
  fnSPI_ReadWrite(SPIX_PERIPH,WRITE);
  
  /******* Upper Address *******/
  fnSPI_ReadWrite(SPIX_PERIPH,ucupperADD);
  
  /******* Lower Address *******/
  fnSPI_ReadWrite(SPIX_PERIPH,uclowerADD);
  
  while(ucLength > 0)
  {
    ucData = *pucWriteData;
    fnSPI_ReadWrite(SPIX_PERIPH,ucData);
    ucLength--;
    pucWriteData++;	
  }
  
  for(uiLoop = 500; uiLoop > 0 ; uiLoop--);	 //change6	
  /* CS = 1*/
  GPIO_WriteBit(EEPROM_CHIP_SELECT_PORT, EEPROM_CHIP_SELECT, Bit_SET); 
  
  for(uiLoop = 500; uiLoop > 0 ; uiLoop--);	
  
  while(1)
  {  
    ucData = fnSPI_Read_Status_page();
    //	if((fnSPI_Read_Status_page() & BIT_MASK) != BIT_COMPARE)
    if((ucData & BIT_MASK) != BIT_COMPARE)
    {
      break;
    }
    
  }
  for(uiLoop = 500; uiLoop > 0 ; uiLoop--);
}


/*****************************************************************************
**@Function 			:   fnSPI_EEPROM_ReadData
**@Descriptions  :   This function is read the data stored in EEPROM
**@parameters    :		uiAddress       start of Memory location
ucLength        Number of bytes write
*pucReadData     pointer to write data bytes
**@retval				: 		None
*****************************************************************************/
void fnSPI_EEPROM_ReadData(uint16_t uiAddress, uint8_t ucLength, uint8_t *pucReadData)
{
  uint8_t ucDummy;
  uint8_t uclowerADD = 0, ucupperADD = 0;
  uint16_t uiLoop;
  
  uclowerADD = (uint8_t)(uiAddress & 0x00ff);
  ucupperADD = (uint8_t)((uiAddress >> 8) & 0x00ff);
  
  for(uiLoop = 500; uiLoop > 0 ; uiLoop--);	 //change6	
  /* CS = 0 */
  GPIO_WriteBit(EEPROM_CHIP_SELECT_PORT, EEPROM_CHIP_SELECT, Bit_RESET); 
  
  /******* READ Instruction *******/
  fnSPI_ReadWrite(SPIX_PERIPH,READ);
  
  /******* Upper Address *******/
  fnSPI_ReadWrite(SPIX_PERIPH,ucupperADD);
  
  /******* Lower Address *******/
  fnSPI_ReadWrite(SPIX_PERIPH,uclowerADD);
  
  ucDummy = DUMMY_DATA;	
  
  while(ucLength > 0)
  {
    *pucReadData = fnSPI_ReadWrite(SPIX_PERIPH,ucDummy);
    
    pucReadData++;
    ucLength--;
  }
  
  for(uiLoop = 500; uiLoop > 0 ; uiLoop--);  ///change6
  /* CS = 0 */  
  GPIO_WriteBit(EEPROM_CHIP_SELECT_PORT, EEPROM_CHIP_SELECT, Bit_SET); 
}

/*****************************************************************************
**@Function 	    : 	fnConfigData_Read
**@Descriptions	: 	Reads the Configuration Data from the EEPROM and store it to 
Config Structure Variables.
**@parameters		: 	None
**@return				: 	None
*****************************************************************************/
void fnConfigData_Read(void)
{
  uint8_t ucStrCount;
  /* Read Signature Value */	
  fnSPI_EEPROM_ReadData(SIGNATURE_START, SIGNATUREVALUE_SIZE, SplitByte.rgucData);
  stConfigData.uiConfig_Signature = SplitByte.uiData;
  fnSPI_EEPROM_ReadData(SIGNATURE_BACKUP, SIGNATUREVALUE_SIZE, SplitByte.rgucData);
  uiBakSignature = SplitByte.uiData;
#ifdef GENERAL_DBG_EE
  printf("\n\r Signature value is: 0x%x\n\r", stConfigData.uiSignature);
#endif
  
  if(stConfigData.uiConfig_Signature !=  uiBakSignature)
  {
#ifdef GENERAL_DBG_EE
    printf("\n\r First Boot Up Time \n\r");
#endif
    /* Set Default Values.	*/
    fnsetDefaultvalues(); 
    
    uiBakSignature = stConfigData.uiConfig_Signature;
#ifdef GENERAL_DBG_EE
    printf("\n\r NEW Signature value is: 0x%x\n\r", stConfigData.uiConfig_Signature);
#endif
    SplitByte.uiData = uiBakSignature;
    fnSPI_EEPROM_WriteData(SIGNATURE_BACKUP, SIGNATUREVALUE_SIZE, SplitByte.rgucData);		
    SplitByte.uiData = stConfigData.uiConfig_Signature;
    fnSPI_EEPROM_WriteData(SIGNATURE_START, SIGNATUREVALUE_SIZE, SplitByte.rgucData);	
  }
  else
  {
#ifdef GENERAL_DBG_EE
    printf("\n\r Not a First Boot Up Time \n\r");
#endif
#ifdef VCM
    /******************************VCM************************************/
    fnSPI_EEPROM_ReadData(VCM_UPSTROKE_ADD, 2, SplitByte.rgucData);
    stConfigData.iConfig_UpStrokeTH = SplitByte.iData;
#ifdef GENERAL_DBG_EE
    printf("1)VCM_UPSTROKE_ADD = %d\r\n", stConfigData.iConfig_UpStrokeTH);
#endif
    
    fnSPI_EEPROM_ReadData(VCM_DOWNSTROKE_ADD, 2, SplitByte.rgucData);
    stConfigData.iConfig_DownStrokeTH = SplitByte.iData;
#ifdef GENERAL_DBG_EE
    printf("2)VCM_DOWNSTROKE_ADD = %d\r\n", stConfigData.iConfig_DownStrokeTH);
#endif
    
    fnSPI_EEPROM_ReadData(VCM_CYLINDER_COUNT, 1, &stConfigData.ucConfig_CylinderCount);	
#ifdef GENERAL_DBG_EE
    printf("3)VCM_CYLINDER_COUNT = %d\r\n", stConfigData.ucConfig_CylinderCount);
#endif			
    
    SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(VCM_PRESSURE_LOWTH1, 4, SplitByte.rgucData);
    stConfigData.fConfig_PressureGauge1LTH = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("4)VCM_PRESSURE_LOWTH1 = %f\r\n", stConfigData.fConfig_PressureGauge1LTH);
#endif
    
    SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(VCM_PRESSURE_HIGHTH1, 4, SplitByte.rgucData);
    stConfigData.fConfig_PressureGauge1HTH = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("5)VCM_PRESSURE_HIGHTH1 = %f\r\n", stConfigData.fConfig_PressureGauge1HTH);	
#endif
    
    SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(VCM_PRESSURE_LOWTH2, 4, SplitByte.rgucData);
    stConfigData.fConfig_PressureGauge2LTH = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("6)VCM_PRESSURE_LOWTH2 = %f\r\n", stConfigData.fConfig_PressureGauge2LTH);	
#endif
    
    SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(VCM_PRESSURE_HIGHTH2, 4, SplitByte.rgucData);
    stConfigData.fConfig_PressureGauge2HTH = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("7)VCM_PRESSURE_HIGHTH2 = %f\r\n", stConfigData.fConfig_PressureGauge2HTH);	
#endif
    
    SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(VCM_PRESSURE_LOWTH3, 4, SplitByte.rgucData);
    stConfigData.fConfig_PressureGauge3LTH = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("8)VCM_PRESSURE_LOWTH3 = %f\r\n", stConfigData.fConfig_PressureGauge3LTH);	
#endif
    
    SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(VCM_PRESSURE_HIGHTH3, 4, SplitByte.rgucData);
    stConfigData.fConfig_PressureGauge3HTH = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("9)VCM_PRESSURE_HIGHTH3 = %f\r\n", stConfigData.fConfig_PressureGauge3HTH);	
#endif
    
    SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(VCM_CYLINDER_AREA1, 4, SplitByte.rgucData);
    stConfigData.fConfig_AreaofCylinderA = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("10)VCM_CYLINDER_AREA1 = %f\r\n", stConfigData.fConfig_AreaofCylinderA);	
#endif
    
    SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(VCM_CYLINDER_AREA2, 4, SplitByte.rgucData);
    stConfigData.fConfig_AreaofCylinderB = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("11)VCM_CYLINDER_AREA2 = %f\r\n", stConfigData.fConfig_AreaofCylinderB);	
#endif
    
    SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(VCM_CYLINDER_AREA3, 4, SplitByte.rgucData);
    stConfigData.fConfig_AreaofCylinderC = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("12)VCM_CYLINDER_AREA3 = %f\r\n", stConfigData.fConfig_AreaofCylinderC);	
#endif
    
    SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(VCM_BEFORE_STALL, 4, SplitByte.rgucData);
    stConfigData.fConfig_TimeBeforeStall = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("13)VCM_BEFORE_STALL = %f\r\n", stConfigData.fConfig_TimeBeforeStall);	
#endif
    
    
    SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(VCM_COMPARE_STALL, 4, SplitByte.rgucData);
    stConfigData.fConfig_TimeCompareStall = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("14)VCM_COMPARE_STALL = %f\r\n", stConfigData.fConfig_TimeCompareStall);	
#endif
    
    
    fnSPI_EEPROM_ReadData(VCM_PRESSURE_GAUGE1_CONN, 1, &stConfigData.ucConfig_ConnPressureGuage1);	
#ifdef GENERAL_DBG_EE
    printf("15)VCM_PRESSURE_GAUGE1_CONN = %d\r\n", stConfigData.ucConfig_ConnPressureGuage1);
#endif			
    
    fnSPI_EEPROM_ReadData(VCM_PRESSURE_GAUGE2_CONN, 1, &stConfigData.ucConfig_ConnPressureGuage2);	
#ifdef GENERAL_DBG_EE
    printf("16)VCM_PRESSURE_GAUGE2_CONN = %d\r\n", stConfigData.ucConfig_ConnPressureGuage2);
#endif	
    
    fnSPI_EEPROM_ReadData(VCM_PRESSURE_GAUGE3_CONN, 1, &stConfigData.ucConfig_ConnPressureGuage3);	
#ifdef GENERAL_DBG_EE
    printf("17)VCM_PRESSURE_GAUGE3_CONN = %d\r\n", stConfigData.ucConfig_ConnPressureGuage3);
#endif
    
    fnSPI_EEPROM_ReadData(VCM_LINEAR_TRANSDUCER_CONN, 1, &stConfigData.ucConfig_ConnLinearTransducer);	
#ifdef GENERAL_DBG_EE
    printf("18)VCM_LINEAR_TRANSDUCER_CONN = %d\r\n", stConfigData.ucConfig_ConnLinearTransducer);
#endif
    
    fnSPI_EEPROM_ReadData(VCM_EE07_INSIDE_CONN, 1, &stConfigData.ucConfig_ConnInsideEE07);	
#ifdef GENERAL_DBG_EE
    printf("19)VCM_EE07_INSIDE_CONN = %d\r\n", stConfigData.ucConfig_ConnInsideEE07);
#endif
    
    fnSPI_EEPROM_ReadData(VCM_EE07_OUTSIDE_CONN, 1, &stConfigData.ucConfig_ConnOutsideEE07);	
#ifdef GENERAL_DBG_EE
    printf("20)VCM_EE07_OUTSIDE_CONN = %d\r\n", stConfigData.ucConfig_ConnOutsideEE07);
#endif
    
    fnSPI_EEPROM_ReadData(VCM_STALL_VALVE_CONN, 1, &stConfigData.ucConfig_ConnStallvalve);
//		stConfigData.ucConfig_ConnStallvalve = 1;
#ifdef GENERAL_DBG_EE
    printf("21)VCM_STALL_VALVE_CONN = %d\r\n", stConfigData.ucConfig_ConnStallvalve);
#endif
    /*change53*/
		 fnSPI_EEPROM_ReadData(VCM_RECIRC_VALVE_CONN, 1, &stConfigData.ucConfig_ConnRecirculationvalve);	
//stConfigData.ucConfig_ConnRecirculationvalve = 3; 
#ifdef GENERAL_DBG_EE
    printf("21)VCM_RECIRC_VALVE_CONN = %d\r\n", stConfigData.ucConfig_ConnRecirculationvalve);
#endif
		
    fnSPI_EEPROM_ReadData(VCM_BYPASS, 1, &stConfigData.ucConfig_VCMBypass);	
#ifdef GENERAL_DBG_EE
    printf("22)VCM_BYPASS = %d\r\n", stConfigData.ucConfig_VCMBypass);
#endif
    
#endif
#ifdef HCM
    /******************************HCM************************************/
    SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(HCM_ANTI_SET_TEMP, 4, SplitByte.rgucData);
    stConfigData.fConfig_AntiFrzTempSet = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("23)HCM_ANTI_SET_TEMP = %f\r\n", stConfigData.fConfig_AntiFrzTempSet);	
#endif
    
    SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(HCM_ANTI_THRESH_TEMP_LOW, 4, SplitByte.rgucData);
    stConfigData.fConfig_AntiFrzTempLTH = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("24)HCM_ANTI_THRESH_TEMP_LOW = %f\r\n", stConfigData.fConfig_AntiFrzTempLTH);	
#endif
    
    SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(HCM_ANTI_THRESH_TEMP_HIGH, 4, SplitByte.rgucData);
    stConfigData.fConfig_AntiFrzTempHTH = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("25)HCM_ANTI_THRESH_TEMP_HIGH = %f\r\n", stConfigData.fConfig_AntiFrzTempHTH);	
#endif
    
    SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(HCM_ANTI_HYST_TEMP_POS, 4, SplitByte.rgucData);
    stConfigData.fConfig_HysAntiFrzPos = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("26)HCM_ANTI_HYST_TEMP_POS = %f\r\n", stConfigData.fConfig_HysAntiFrzPos);	
#endif
    
    SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(HCM_ANTI_HYST_TEMP_NEG, 4, SplitByte.rgucData);
    stConfigData.fConfig_HysAntiFrzNeg = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("27)HCM_ANTI_HYST_TEMP_NEG = %f\r\n", stConfigData.fConfig_HysAntiFrzNeg);	
#endif
    
    SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(HCM_INLINE_BASE_SET_TEMP, 4, SplitByte.rgucData);
    stConfigData.fConfig_InlineBaseTempSet = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("28)HCM_INLINE_BASE_SET_TEMP = %f\r\n", stConfigData.fConfig_InlineBaseTempSet);	
#endif
    
    SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(HCM_INLINE_BASE_THRESH_TEMP_LOW, 4, SplitByte.rgucData);
    stConfigData.fConfig_InlineBaseTempLTH = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("29)HCM_INLINE_BASE_THRESH_TEMP_LOW = %f\r\n", stConfigData.fConfig_InlineBaseTempLTH);	
#endif
    
    SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(HCM_INLINE_BASE_THRESH_TEMP_HIGH, 4, SplitByte.rgucData);
    stConfigData.fConfig_InlineBaseTempHTH = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("30)HCM_INLINE_BASE_THRESH_TEMP_HIGH = %f\r\n", stConfigData.fConfig_InlineBaseTempHTH);	
#endif		
    
    SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(HCM_INLINE_BASE_HYST_TEMP_POS, 4, SplitByte.rgucData);
    stConfigData.fConfig_HysInlineBasePos = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("31)HCM_INLINE_BASE_HYST_TEMP_POS = %f\r\n", stConfigData.fConfig_HysInlineBasePos);	
#endif
    
    SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(HCM_INLINE_BASE_HYST_TEMP_NEG, 4, SplitByte.rgucData);
    stConfigData.fConfig_HysInlineBaseNeg = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("32)HCM_INLINE_BASE_HYST_TEMP_NEG = %f\r\n", stConfigData.fConfig_HysInlineBaseNeg);	
#endif
    
    SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(HCM_INLINE_HARD_SET_TEMP, 4, SplitByte.rgucData);
    stConfigData.fConfig_InlineHardTempSet = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("33)HCM_INLINE_HARD_SET_TEMP = %f\r\n", stConfigData.fConfig_InlineHardTempSet);	
#endif
    
    SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(HCM_INLINE_HARD_THRESH_TEMP_LOW, 4, SplitByte.rgucData);
    stConfigData.fConfig_InlineHardTempLTH = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("34)HCM_INLINE_HARD_THRESH_TEMP_LOW = %f\r\n", stConfigData.fConfig_InlineHardTempLTH);	
#endif
    
    SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(HCM_INLINE_HARD_THRESH_TEMP_HIGH, 4, SplitByte.rgucData);
    stConfigData.fConfig_InlineHardTempHTH = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("35)HCM_INLINE_HARD_THRESH_TEMP_HIGH = %f\r\n", stConfigData.fConfig_InlineHardTempHTH);	
#endif
    
    SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(HCM_INLINE_HARD_HYST_TEMP_POS, 4, SplitByte.rgucData);
    stConfigData.fConfig_InlineHysHardPos = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("36)HCM_INLINE_HARD_HYST_TEMP_POS = %f\r\n", stConfigData.fConfig_InlineHysHardPos);	
#endif
    
    SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(HCM_INLINE_HARD_HYST_TEMP_NEG, 4, SplitByte.rgucData);
    stConfigData.fConfig_InlineHysHardNeg = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("37)HCM_INLINE_HARD_HYST_TEMP_NEG = %f\r\n", stConfigData.fConfig_InlineHysHardNeg);	
#endif
    
    SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(HCM_BASE_LIQUID_THRESH_LEVEL_LOW, 4, SplitByte.rgucData);
    stConfigData.fConfig_BaseLiquidLevelLTH = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("38)HCM_BASE_LIQUID_THRESH_LEVEL_LOW = %f\r\n", stConfigData.fConfig_BaseLiquidLevelLTH);	
#endif
    
    SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(HCM_BASE_LIQUID_THRESH_LEVEL_HIGH, 4, SplitByte.rgucData);
    stConfigData.fConfig_BaseLiquidLevelHTH = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("39)HCM_BASE_LIQUID_THRESH_LEVEL_HIGH = %f\r\n", stConfigData.fConfig_BaseLiquidLevelHTH);	
#endif
    
    SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(HCM_HARD_LIQUID_THRESH_LEVEL_LOW, 4, SplitByte.rgucData);
    stConfigData.fConfig_HardLiquidLevelLTH = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("40)HCM_HARD_LIQUID_THRESH_LEVEL_LOW = %f\r\n", stConfigData.fConfig_HardLiquidLevelLTH);	
#endif
    
    SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(HCM_HARD_LIQUID_THRESH_LEVEL_HIGH, 4, SplitByte.rgucData);
    stConfigData.fConfig_HardLiquidLevelHTH = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("41)HCM_HARD_LIQUID_THRESH_LEVEL_HIGH = %f\r\n",  stConfigData.fConfig_HardLiquidLevelHTH);	
#endif
    
    SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(HCM_BASE_TANK_HEIGHT, 4, SplitByte.rgucData);
    stConfigData.fConfig_BaseTankHeight = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("42)HCM_BASE_TANK_HEIGHT = %f\r\n", stConfigData.fConfig_BaseTankHeight);	
#endif
    
    SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(HCM_HARD_TANK_HEIGHT, 4, SplitByte.rgucData);
    stConfigData.fConfig_HardTankHeight = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("43)HCM_HARD_TANK_HEIGHT = %f\r\n", stConfigData.fConfig_HardTankHeight);	
#endif
    
    
    fnSPI_EEPROM_ReadData(HCM_LIQUID_LEVEL_SAMPLING_RATE, 2, SplitByte.rgucData);
    stConfigData.uiConfig_LLSamplingRate = SplitByte.iData;
#ifdef GENERAL_DBG_EE
    printf("44)HCM_LIQUID_LEVEL_SAMPLING_RATE = %d\r\n", stConfigData.uiConfig_LLSamplingRate);
#endif
    
    
    fnSPI_EEPROM_ReadData(HCM_INLINE_BASE_THERM_CONN, 1, &stConfigData.ucConfig_ConnBaseinlinether);	
#ifdef GENERAL_DBG_EE
    printf("45)HCM_INLINE_BASE_THERM_CONN = %d\r\n", stConfigData.ucConfig_ConnBaseinlinether);
#endif
    
    fnSPI_EEPROM_ReadData(HCM_INLINE_HARD_THERM_CONN, 1, &stConfigData.ucConfig_ConnHardinlinether);	
#ifdef GENERAL_DBG_EE
    printf("46)HCM_INLINE_HARD_THERM_CONN = %d\r\n", stConfigData.ucConfig_ConnHardinlinether);
#endif
    
    fnSPI_EEPROM_ReadData(HCM_INLINE_ANTIFREEZE_THERM_CONN, 1, &stConfigData.ucConfig_ConnAntifreezether);	
#ifdef GENERAL_DBG_EE
    printf("47)HCM_INLINE_ANTIFREEZE_THERM_CONN = %d\r\n", stConfigData.ucConfig_ConnAntifreezether);
#endif
    
    fnSPI_EEPROM_ReadData(HCM_RETURN_BASE_THERM_CONN, 1, &stConfigData.ucConfig_ConnBasereturnlinether);	
#ifdef GENERAL_DBG_EE
    printf("48)HCM_RETURN_BASE_THERM_CONN = %d\r\n", stConfigData.ucConfig_ConnBasereturnlinether);
#endif
    
    
    
    fnSPI_EEPROM_ReadData(HCM_INLINE_BASE_HEATER_CONN, 1, &stConfigData.ucConfig_ConnBaseinlineheater);	
#ifdef GENERAL_DBG_EE
    printf("49)HCM_INLINE_BASE_HEATER_CONN = %d\r\n", stConfigData.ucConfig_ConnBaseinlineheater);
#endif
    
    fnSPI_EEPROM_ReadData(HCM_INLINE_HARD_HEATER_CONN, 1, &stConfigData.ucConfig_ConnHardinlineheater);	
#ifdef GENERAL_DBG_EE
    printf("50)HCM_INLINE_HARD_HEATER_CONN = %d\r\n", stConfigData.ucConfig_ConnHardinlineheater);
#endif
    
    fnSPI_EEPROM_ReadData(HCM_INLINE_ANTIFREEZE_HEATER_CONN, 1, &stConfigData.ucConfig_ConnAntifreezeheater);	
#ifdef GENERAL_DBG_EE
    printf("51)HCM_INLINE_ANTIFREEZE_HEATER_CONN = %d\r\n", stConfigData.ucConfig_ConnAntifreezeheater);
#endif
    
    fnSPI_EEPROM_ReadData(HCM_HARD_HEATER_CONN, 1, &stConfigData.ucConfig_ConnHardheater);	
#ifdef GENERAL_DBG_EE
    printf("52)HCM_HARD_HEATER_CONN = %d\r\n", stConfigData.ucConfig_ConnHardheater);
#endif
    
    fnSPI_EEPROM_ReadData(HCM_BASE_LIQUID_LEVEL_CONN, 1, &stConfigData.ucConfig_ConnBasetankLL);	
#ifdef GENERAL_DBG_EE
    printf("53)HCM_BASE_LIQUID_LEVEL_CONN = %d\r\n", stConfigData.ucConfig_ConnBasetankLL);
#endif
    
    fnSPI_EEPROM_ReadData(HCM_HARD_LIQUID_LEVEL_CONN, 1, &stConfigData.ucConfig_ConnHardtankLL);	
#ifdef GENERAL_DBG_EE
    printf("54)HCM_HARD_LIQUID_LEVEL_CONN = %d\r\n", stConfigData.ucConfig_ConnHardtankLL);
#endif
    
    fnSPI_EEPROM_ReadData(HCM_BYPASS, 2, SplitByte.rgucData);
    stConfigData.uiConfig_HCMBypass = SplitByte.iData;
#ifdef GENERAL_DBG_EE
    printf("55)HCM_BYPASS = %d\r\n", stConfigData.uiConfig_HCMBypass);
#endif
    
    
#endif
    
    
#ifdef OPMM
    /******************************OPMM************************************/
    fnSPI_EEPROM_ReadData(OPMM_SPRAY_COUNT, 1, &stConfigData.ucConfig_SprayCount);
#ifdef GENERAL_DBG_EE
    printf("56)READ OPMM_SPRAY_COUNT = %d\r\n", stConfigData.ucConfig_SprayCount);
#endif
    
    SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(OPMM_PRIME_TIME, 4, SplitByte.rgucData);
    stConfigData.fConfig_PrimeTime = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("57)OPMM_PRIME_TIME = %f\r\n", stConfigData.fConfig_PrimeTime);	
#endif
    
    SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(OPMM_GUN_DELAY_TIME, 4, SplitByte.rgucData);
    stConfigData.fConfig_GunDelayTime = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("58)OPMM_GUN_DELAY_TIME = %f\r\n", stConfigData.fConfig_GunDelayTime);	
#endif
    
    SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(OPMM_SPRAY_OVERLAPTIME, 4, SplitByte.rgucData);
    stConfigData.fConfig_SprayOverlapTime = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("59)OPMM_SPRAY_OVERLAPTIME = %f\r\n", stConfigData.fConfig_SprayOverlapTime);	
#endif
    
    SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(OPMM_REVERSE_DELAYTIME, 4, SplitByte.rgucData);
    stConfigData.fConfig_ReverseDelayTime = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("60)OPMM_REVERSE_DELAYTIME = %f\r\n", stConfigData.fConfig_ReverseDelayTime);	
#endif
    
    SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(OPMM_FLUSH_TIME, 4, SplitByte.rgucData);
    stConfigData.fConfig_FlushTime = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("61)OPMM_FLUSH_TIME = %f\r\n", stConfigData.fConfig_FlushTime);	
#endif
    
    SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(OPMM_DCLS_HOME_REACH_TIME, 4, SplitByte.rgucData);
    stConfigData.fConfig_DCLSTime = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("62)OPMM_DCLS_HOME_REACH_TIME = %f\r\n", stConfigData.fConfig_DCLSTime);	
#endif
    
    SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(OPMM_HOME_DCLS_REACH_TIME, 4, SplitByte.rgucData);
    stConfigData.fConfig_HOMETime = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("63)OPMM_HOME_DCLS_REACH_TIME = %f\r\n", stConfigData.fConfig_HOMETime);	
#endif
    
    SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(OPMM_GUN_POSITION_CHANGE_TIME, 4, SplitByte.rgucData);
    stConfigData.fConfig_GunPosChangeTime = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("64)OPMM_GUN_POSITION_CHANGE_TIME = %f\r\n", stConfigData.fConfig_GunPosChangeTime);	
#endif
    
    SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(OPMM_DELAY_SPRAY_VALVE_FLUSH_VBAT_TIME, 4, SplitByte.rgucData);
    stConfigData.fConfig_TimebeforeONSprayValve = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("65)OPMM_DELAY_SPRAY_VALVE_FLUSH_VBAT_TIME = %f\r\n", stConfigData.fConfig_TimebeforeONSprayValve);	
#endif
    
    SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(OPMM_FLUSH_WARNNING_TIME, 4, SplitByte.rgucData);
    stConfigData.fConfig_FlushWarnTime = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("66)OPMM_FLUSH_WARNNING_TIME = %f\r\n", stConfigData.fConfig_FlushWarnTime);	
#endif
    
    
    fnSPI_EEPROM_ReadData(OPMM_SPRAY_VALVE_CONN, 1, &stConfigData.ucConfig_ConnSprayvalve);
#ifdef GENERAL_DBG_EE
    printf("67)READ OPMM_SPRAY_VALVE_CONN = %d\r\n", stConfigData.ucConfig_ConnSprayvalve);
#endif
    
    fnSPI_EEPROM_ReadData(OPMM_GUNPOS_VALVE_CONN, 1, &stConfigData.ucConfig_ConnGunposition);
#ifdef GENERAL_DBG_EE
    printf("68)READ OPMM_GUNPOS_VALVE_CONN = %d\r\n", stConfigData.ucConfig_ConnGunposition);
#endif
    
    fnSPI_EEPROM_ReadData(OPMM_AB_VALVE_CONN, 1, &stConfigData.ucConfig_ConnABvalve);
#ifdef GENERAL_DBG_EE
    printf("69)READ OPMM_AB_VALVE_CONN = %d\r\n", stConfigData.ucConfig_ConnABvalve);
#endif
    
    fnSPI_EEPROM_ReadData(OPMM_SOLVENT_VALVE_CONN, 1, &stConfigData.ucConfig_ConnSolventvalve);
#ifdef GENERAL_DBG_EE
    printf("70)READ OPMM_SOLVENT_VALVE_CONN = %d\r\n", stConfigData.ucConfig_ConnSolventvalve);
#endif
    
    fnSPI_EEPROM_ReadData(OPMM_FORWARD_VALVE_CONN, 1, &stConfigData.ucConfig_ConnForwardvalve);
#ifdef GENERAL_DBG_EE
    printf("71)READ OPMM_FORWARD_VALVE_CONN = %d\r\n", stConfigData.ucConfig_ConnForwardvalve);
#endif
    
    fnSPI_EEPROM_ReadData(OPMM_BACKWARD_VALVE_CONN, 1, &stConfigData.ucConfig_ConnBackwardvalve);
#ifdef GENERAL_DBG_EE
    printf("72)READ OPMM_BACKWARD_VALVE_CONN = %d\r\n", stConfigData.ucConfig_ConnBackwardvalve);
#endif
    
    fnSPI_EEPROM_ReadData(OPMM_NON_CONTACT12_CONN, 1, &stConfigData.ucConfig_ConnNonContactat12);
#ifdef GENERAL_DBG_EE
    printf("73)READ OPMM_NON_CONTACT12_CONN = %d\r\n", stConfigData.ucConfig_ConnNonContactat12);
#endif
    
    fnSPI_EEPROM_ReadData(OPMM_NON_CONTACT3_CONN, 1, &stConfigData.ucConfig_ConnNonContactat3);
#ifdef GENERAL_DBG_EE
    printf("74)READ OPMM_NON_CONTACT3_CONN = %d\r\n", stConfigData.ucConfig_ConnNonContactat3);
#endif
    
    fnSPI_EEPROM_ReadData(OPMM_NON_CONTACT6_CONN, 1, &stConfigData.ucConfig_ConnNonContactat6);
#ifdef GENERAL_DBG_EE
    printf("75)READ OPMM_NON_CONTACT6_CONN = %d\r\n", stConfigData.ucConfig_ConnNonContactat6);
#endif
    
    fnSPI_EEPROM_ReadData(OPMM_NON_CONTACT9_CONN, 1, &stConfigData.ucConfig_ConnNonContactat9);
#ifdef GENERAL_DBG_EE
    printf("76)READ OPMM_NON_CONTACT9_CONN = %d\r\n", stConfigData.ucConfig_ConnNonContactat9);
#endif
    
    fnSPI_EEPROM_ReadData(OPMM_BYPASS, 2, SplitByte.rgucData);
    stConfigData.uiConfig_OPMMBypass = SplitByte.iData;
#ifdef GENERAL_DBG_EE
    printf("77)OPMM_BYPASS = %d\r\n", stConfigData.uiConfig_OPMMBypass);
#endif
#endif
    /******************************SCU************************************/
#ifdef SCU
    
    SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(SCU_SPRAY_PRESSURE_VALUE, 4, SplitByte.rgucData);
    stConfigData.fConfig_SprayPressure = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("78)SCU_SPRAY_PRESSURE_VALUE = %f\r\n", stConfigData.fConfig_SprayPressure);	
#endif
    
    SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(SCU_SPRAY_PRESSURE_THRESH_LOW, 4, SplitByte.rgucData);
    stConfigData.fConfig_SprayPressureLTH = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("79)SCU_SPRAY_PRESSURE_THRESH_LOW = %f\r\n", stConfigData.fConfig_SprayPressureLTH);	
#endif
    
    
    SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(SCU_SPRAY_PRESSURE_THRESH_HIGH, 4, SplitByte.rgucData);
    stConfigData.fConfig_SprayPressureHTH = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("80)SCU_SPRAY_PRESSURE_THRESH_HIGH = %f\r\n", stConfigData.fConfig_SprayPressureHTH);	
#endif
    
    
    SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(SCU_SPRAY_PRESSURE_HYST_POS, 4, SplitByte.rgucData);
    stConfigData.fConfig_HysSprayPresPos = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("81)SCU_SPRAY_PRESSURE_HYST_POS = %f\r\n", stConfigData.fConfig_HysSprayPresPos);	
#endif
    
    SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(SCU_SPRAY_PRESSURE_HYST_NEG, 4, SplitByte.rgucData);
    stConfigData.fConfig_HysSprayPresNeg = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("82)SCU_SPRAY_PRESSURE_HYST_NEG = %f\r\n", stConfigData.fConfig_HysSprayPresNeg);	
#endif
    
    SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(SCU_RECIRC_PRESSURE_VALUE, 4, SplitByte.rgucData);
    stConfigData.fConfig_RecirPressure = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("83)SCU_RECIRC_PRESSURE_VALUE = %f\r\n", stConfigData.fConfig_RecirPressure);	
#endif
    
    
    SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(SCU_RECIRC_PRESSURE_THRESH_LOW, 4, SplitByte.rgucData);
    stConfigData.fConfig_RecirPressureLTH = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("84)SCU_RECIRC_PRESSURE_THRESH_LOW = %f\r\n", stConfigData.fConfig_RecirPressureLTH);	
#endif
    
    SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(SCU_RECIRC_PRESSURE_THRESH_HIGH, 4, SplitByte.rgucData);
    stConfigData.fConfig_RecirPressureHTH = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("85)SCU_RECIRC_PRESSURE_THRESH_HIGH = %f\r\n", stConfigData.fConfig_RecirPressureHTH);	
#endif
    
    SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(SCU_RECIRC_PRESSURE_HYST_POS, 4, SplitByte.rgucData);
    stConfigData.fConfig_HysRecircPresPos = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("86)SCU_RECIRC_PRESSURE_HYST_POS = %f\r\n", stConfigData.fConfig_HysRecircPresPos);	
#endif
    
    SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(SCU_RECIRC_PRESSURE_HYST_NEG, 4, SplitByte.rgucData);
    stConfigData.fConfig_HysRecircPresNeg = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("87)SCU_RECIRC_PRESSURE_HYST_NEG = %f\r\n", stConfigData.fConfig_HysRecircPresNeg);	
#endif
    
    
    SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(COUNT_JOINT, 4, SplitByte.rgucData);
    stConfigData.fConfig_JointNum = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("88)COUNT_JOINT = %f\r\n", stConfigData.fConfig_JointNum);	
#endif
    
    
    SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(TIME_DATA_LOGGING, 4, SplitByte.rgucData);
    stConfigData.fConfig_DataLoggingPeriod = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("89)TIME_DATA_LOGGING = %f\r\n",  stConfigData.fConfig_DataLoggingPeriod);	
#endif
    
    fnSPI_EEPROM_ReadData(SYS_TIME, 7, stConfigData.ucConfig_SystemTime);
#ifdef GENERAL_DBG_EE
    printf("90)SYS_TIME = %s\r\n", stConfigData.ucConfig_SystemTime);
#endif	
    
    /******************************************** SUPERVISOR ***************************************/
    fnSPI_EEPROM_ReadData(SUPERVISOR_NAME1, 7, stConfigData.rgucConfig_SupervisorName1);
    stConfigData.rgucConfig_SupervisorName1[7] =  NULL;
#ifdef GENERAL_DBG_EE
    printf("91)SUPERVISOR_NAME1 = %s\r\n", stConfigData.rgucConfig_SupervisorName1);
#endif
    
    /*change23*/
    fnSPI_EEPROM_ReadData(CURRENT_INDEX_SUPERVISOR_NAME, 1, &ucSupervisorNameID);	
		fnSPI_EEPROM_ReadData(LOG_INDEX_SUPERVISOR_NAME, 1, &ucSupervisorNameLog);	
		
#ifdef GENERAL_DBG_EE
    printf("46)CURRENT_INDEX_SUPERVISOR_NAME = %d\r\n", ucSupervisorNameID);
#endif
	
    fnSPI_EEPROM_ReadData(SUPERVISOR_NAME2, 7, stConfigData.rgucConfig_SupervisorName2);
    stConfigData.rgucConfig_SupervisorName2[7] =  NULL;
#ifdef GENERAL_DBG_EE
    printf("92)SUPERVISOR_NAME2 = %s\r\n", stConfigData.rgucConfig_SupervisorName2);
#endif
    
    fnSPI_EEPROM_ReadData(SUPERVISOR_NAME3, 7,stConfigData.rgucConfig_SupervisorName3);
    stConfigData.rgucConfig_SupervisorName3[7] =  NULL;
#ifdef GENERAL_DBG_EE
    printf("93)SUPERVISOR_NAME3 = %s\r\n", stConfigData.rgucConfig_SupervisorName3);
#endif
    
    /********************************************** COATING*************************************/
    fnSPI_EEPROM_ReadData(COATING_MAT_NAME1, 7, stConfigData.rgucConfig_CoatingMatName1);
    stConfigData.rgucConfig_CoatingMatName1[7] =  NULL;
#ifdef GENERAL_DBG_EE
    printf("94)COATING_MAT_NAME1 = %s\r\n", stConfigData.rgucConfig_CoatingMatName1);
#endif
    
    fnSPI_EEPROM_ReadData(COATING_MAT_NAME2, 7, stConfigData.rgucConfig_CoatingMatName2);
    stConfigData.rgucConfig_CoatingMatName2[7] =  NULL;
#ifdef GENERAL_DBG_EE
    printf("95)COATING_MAT_NAME2 = %s\r\n", stConfigData.rgucConfig_CoatingMatName2);
#endif
    
    fnSPI_EEPROM_ReadData(COATING_MAT_NAME3, 7, stConfigData.rgucConfig_CoatingMatName3);
    stConfigData.rgucConfig_CoatingMatName3[7] =  NULL;
#ifdef GENERAL_DBG_EE
    printf("96)COATING_MAT_NAME3 = %s\r\n", stConfigData.rgucConfig_CoatingMatName3);
#endif
    
    
    /********************************************BASE*********************************************/
    fnSPI_EEPROM_ReadData(BASE_MAT_BATCHNO1, 7, stConfigData.rgucConfig_BaseMatBatchNo1);
    stConfigData.rgucConfig_BaseMatBatchNo1[7] =  NULL;
#ifdef GENERAL_DBG_EE
    printf("97)BASE_MAT_BATCHNO1 = %s\r\n", stConfigData.rgucConfig_BaseMatBatchNo1);
#endif
    
    fnSPI_EEPROM_ReadData(BASE_MAT_BATCHNO2, 7, stConfigData.rgucConfig_BaseMatBatchNo2);
    stConfigData.rgucConfig_BaseMatBatchNo2[7] =  NULL;
#ifdef GENERAL_DBG_EE
    printf("98)BASE_MAT_BATCHNO2 = %s\r\n", stConfigData.rgucConfig_BaseMatBatchNo2);
#endif
    
    fnSPI_EEPROM_ReadData(BASE_MAT_BATCHNO3, 7, stConfigData.rgucConfig_BaseMatBatchNo3);
    stConfigData.rgucConfig_BaseMatBatchNo3[7] =  NULL;
#ifdef GENERAL_DBG_EE
    printf("99)BASE_MAT_BATCHNO3 = %s\r\n", stConfigData.rgucConfig_BaseMatBatchNo3);
#endif
    
    /***************************************HARDNER**********************************************/
    fnSPI_EEPROM_ReadData(HARD_MAT_BATCHNO1, 7, stConfigData.rgucConfig_HardMatBatchNo1);
    stConfigData.rgucConfig_HardMatBatchNo1[7] =  NULL;
#ifdef GENERAL_DBG_EE
    printf("100)HARD_MAT_BATCHNO1 = %s\r\n", stConfigData.rgucConfig_HardMatBatchNo1);
#endif
    
    fnSPI_EEPROM_ReadData(HARD_MAT_BATCHNO2, 7, stConfigData.rgucConfig_HardMatBatchNo2);
    stConfigData.rgucConfig_HardMatBatchNo2[7] =  NULL;
#ifdef GENERAL_DBG_EE
    printf("101)HARD_MAT_BATCHNO2 = %s\r\n", stConfigData.rgucConfig_HardMatBatchNo2);
#endif
    
    fnSPI_EEPROM_ReadData(HARD_MAT_BATCHNO3, 7, stConfigData.rgucConfig_HardMatBatchNo3);
    stConfigData.rgucConfig_HardMatBatchNo3[7] =  NULL;		
#ifdef GENERAL_DBG_EE
    printf("102)HARD_MAT_BATCHNO3 = %s\r\n", stConfigData.rgucConfig_HardMatBatchNo3);
#endif
    
    fnSPI_EEPROM_ReadData(SCU_UNIT_TYPE, 1, &stConfigData.ucConfig_TypeofUnit);
#ifdef GENERAL_DBG_EE
    printf("103)SCU_UNIT_TYPE = %d\r\n", stConfigData.ucConfig_TypeofUnit);
#endif	
    
    
    fnSPI_EEPROM_ReadData(SCU_BYPASS, 1, &stConfigData.ucConfig_SCUBypass);
#ifdef GENERAL_DBG_EE
    printf("104)SCU_BYPASS = %d\r\n", stConfigData.ucConfig_SCUBypass);
#endif	
    
    fnSPI_EEPROM_ReadData(TEMP_PRESS_ACHIEVE_BYPASS, 1, &stConfigData.ucConfig_Temp_Press_Achieve_Bypass);
#ifdef GENERAL_DBG_EE
    printf("105)TEMP_PRESS_ACHIEVE_BYPASS = %d\r\n", stConfigData.ucConfig_Temp_Press_Achieve_Bypass);
#endif	
    
    SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(OPMM_PIPE_TEMP_THRES_LOW, 4, SplitByte.rgucData);
    stConfigData.fConfig_PipeTempLTH = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("106)OPMM_PIPE_TEMP_THRES_LOW = %f\r\n", stConfigData.fConfig_PipeTempLTH);	
#endif					
    
    SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(OPMM_PIPE_TEMP_THRES_HIGH, 4, SplitByte.rgucData);
    stConfigData.fConfig_PipeTempHTH = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("107)OPMM_PIPE_TEMP_THRES_HIGH = %f\r\n", stConfigData.fConfig_PipeTempHTH);	
#endif	
    
    /**********************************ETHERNET PARAMETERS****************************/
    fnSPI_EEPROM_ReadData(SCU_ETH_IPADDRESS, 15, stEthernetIP.rgucIP_Addr);
    stEthernetIP.rgucIP_Addr[15] =  NULL;
#ifdef GENERAL_DBG_EE
    printf("108)SCU_ETH_IPADDRESS = %s\r\n", stEthernetIP.rgucIP_Addr);
#endif
    
    fnSPI_EEPROM_ReadData(SCU_ETH_SMADDRESS, 15, stEthernetIP.rgucSubnet_Addr);
    stEthernetIP.rgucSubnet_Addr[15] =  NULL;
#ifdef GENERAL_DBG_EE
    printf("109)SCU_ETH_SMADDRESS = %s\r\n", stEthernetIP.rgucSubnet_Addr);
#endif
    
    fnSPI_EEPROM_ReadData(SCU_ETH_GWADDRESS, 15, stEthernetIP.rgucGateway_Addr);
    stEthernetIP.rgucGateway_Addr[15] =  NULL;
#ifdef GENERAL_DBG_EE
    printf("110)SCU_ETH_GWADDRESS = %s\r\n", stEthernetIP.rgucGateway_Addr);
#endif
    
    fnSPI_EEPROM_ReadData(SCU_ETH_PORTADDRESS, 2, SplitByte.rgucData);
    stEthernetIP.uiPort = SplitByte.uiData;
#ifdef GENERAL_DBG_EE
    printf("111)SCU_ETH_PORTADDRESS = %d\r\n", stEthernetIP.uiPort);
#endif
    
    fnSPI_EEPROM_ReadData(OPMM_BUZZER_VOLUME, 1, &stConfigData.ucConfig_BuzzerVolume);
#ifdef GENERAL_DBG_EE
    printf("112)READ OPMM_BUZZER_VOLUME = %d\r\n", stConfigData.ucConfig_BuzzerVolume);
#endif
    /************************************************************************************/		
    /*********************************change8********************************************/
    SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(SCU_TIME_SEND_STALL_COMMAND, 4, SplitByte.rgucData);
    stConfigData.fConfig_TimeBeforeSendingStallcommand = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("113)SCU_TIME_SEND_STALL_COMMAND = %f\r\n",  stConfigData.fConfig_TimeBeforeSendingStallcommand);	
#endif
    
    SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(OPMM_STOP_HLS_TIMER, 4, SplitByte.rgucData);
    stConfigData.fConfig_TimeStopOPMMcommand = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("114)OPMM_STOP_HLS_TIMER = %f\r\n",  stConfigData.fConfig_TimeStopOPMMcommand);	
#endif
    
    SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(OPMM_BATT_KNEE_VOLT, 4, SplitByte.rgucData);
    stConfigData.fConfig_BatteryKneeVoltage = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("115)OPMM_BATT_KNEE_VOLT = %f\r\n",  stConfigData.fConfig_BatteryKneeVoltage);	
#endif
    
    /************************************************************************************/		
    /*********************************change10********************************************/
    
    fnSPI_EEPROM_ReadData(VCM_STALL_UPSTROKE_UPPER_LIMIT, 1, &stConfigData.ucConfig_UpstrokeUpLimit);	
#ifdef GENERAL_DBG_EE
    printf("116)VCM_STALL_UPSTROKE_UPPER_LIMIT = %d\r\n", stConfigData.ucConfig_UpstrokeUpLimit);
#endif
    
    fnSPI_EEPROM_ReadData(VCM_STALL_UPSTROKE_MIN_RANGE, 1, &stConfigData.ucConfig_UpstrokeMinRange);	
#ifdef GENERAL_DBG_EE
    printf("117)VCM_STALL_UPSTROKE_MIN_RANGE = %d\r\n", stConfigData.ucConfig_UpstrokeMinRange);
#endif
    
    fnSPI_EEPROM_ReadData(VCM_STALL_UPSTROKE_MAX_RANGE, 1, &stConfigData.ucConfig_UpstrokeMaxRange);	
#ifdef GENERAL_DBG_EE
    printf("118)VCM_STALL_UPSTROKE_MAX_RANGE = %d\r\n", stConfigData.ucConfig_UpstrokeMaxRange);
#endif
    
    fnSPI_EEPROM_ReadData(VCM_STALL_DOWNSTROKE_UPPER_LIMIT, 1, &stConfigData.ucConfig_DownstrokeUpLimit);	
#ifdef GENERAL_DBG_EE
    printf("119)VCM_STALL_DOWNSTROKE_UPPER_LIMIT = %d\r\n", stConfigData.ucConfig_DownstrokeUpLimit);
#endif
    
    fnSPI_EEPROM_ReadData(VCM_STALL_DOWNSTROKE_MIN_RANGE, 1, &stConfigData.ucConfig_DownstrokeMinRange);	
#ifdef GENERAL_DBG_EE
    printf("120)VCM_STALL_DOWNSTROKE_MIN_RANGE = %d\r\n", stConfigData.ucConfig_DownstrokeMinRange);
#endif
    
    fnSPI_EEPROM_ReadData(VCM_STALL_DOWNSTROKE_MAX_RANGE, 1, &stConfigData.ucConfig_DownstrokeMaxRange);	
#ifdef GENERAL_DBG_EE
    printf("121)VCM_STALL_DOWNSTROKE_MAX_RANGE = %d\r\n", stConfigData.ucConfig_DownstrokeMaxRange);
#endif
    /**********************************WIFI PARAMETERS change18****************************/
    fnSPI_EEPROM_ReadData(SCU_WIFI_IPADDRESS, 15, stWifiIP.rgucIP_Addr);
    stWifiIP.rgucIP_Addr[15] =  NULL;
#ifdef GENERAL_DBG_EE
    printf("122)SCU_WIFI_IPADDRESS = %s\r\n", stWifiIP.rgucIP_Addr);
#endif
    
    fnSPI_EEPROM_ReadData(SCU_WIFI_SMADDRESS, 15, stWifiIP.rgucSubnet_Addr);
    stWifiIP.rgucSubnet_Addr[15] =  NULL;
#ifdef GENERAL_DBG_EE
    printf("123)SCU_WIFI_SMADDRESS = %s\r\n", stWifiIP.rgucSubnet_Addr);
#endif
    
    fnSPI_EEPROM_ReadData(SCU_WIFI_GWADDRESS, 15, stWifiIP.rgucGateway_Addr);
    stWifiIP.rgucGateway_Addr[15] =  NULL;
#ifdef GENERAL_DBG_EE
    printf("124)SCU_WIFI_GWADDRESS = %s\r\n", stWifiIP.rgucGateway_Addr);
#endif
    
    fnSPI_EEPROM_ReadData(SCU_WIFI_PORTADDRESS, 2, SplitByte.rgucData);
    stWifiIP.uiPort = SplitByte.uiData;
#ifdef GENERAL_DBG_EE
    printf("125)SCU_WIFI_PORTADDRESS = %d\r\n", stWifiIP.uiPort);
#endif
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
    
    /*change21*/		
    SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(IDLE_DATA_LOGGING, 4, SplitByte.rgucData);
    stConfigData.fConfig_IdleTimePeriodicLog = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("97)IDLE_DATA_LOGGING = %f\r\n",  stConfigData.fConfig_IdleTimePeriodicLog);	
#endif
    
    
    /******************************************** JOB NAME ***************************************/
    fnSPI_EEPROM_ReadData(JOB_NAME1, 7, stConfigData.rgucConfig_JobName1);
    stConfigData.rgucConfig_JobName1[7] =  NULL;
#ifdef GENERAL_DBG_EE
    printf("98)JOB_NAME1 = %s\r\n", stConfigData.rgucConfig_JobName1);
#endif
    
    fnSPI_EEPROM_ReadData(JOB_NAME2, 7, stConfigData.rgucConfig_JobName2);
    stConfigData.rgucConfig_JobName2[7] =  NULL;
#ifdef GENERAL_DBG_EE
    printf("99)JOB_NAME2 = %s\r\n", stConfigData.rgucConfig_JobName2);
#endif
    
    fnSPI_EEPROM_ReadData(JOB_NAME3, 7,stConfigData.rgucConfig_JobName3);
    stConfigData.rgucConfig_JobName3[7] =  NULL;
#ifdef GENERAL_DBG_EE
    printf("100)JOB_NAME3 = %s\r\n", stConfigData.rgucConfig_JobName3);
#endif
    /*change23*/
    fnSPI_EEPROM_ReadData(CURRENT_INDEX_JOB_NAME, 1, &ucJobNameID);	
		fnSPI_EEPROM_ReadData(LOG_INDEX_JOB_NAME, 1, &ucJobNameLog); /*change78*/	
		
#ifdef GENERAL_DBG_EE
    printf("CURRENT_INDEX_JOB_NAME = %d\r\n", ucJobNameID);
#endif
    
    SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(PIPE_DIAMETER, 4, SplitByte.rgucData);
    stConfigData.fConfig_PipeDiameter = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("101)PIPE_DIAMETER = %f\r\n",  stConfigData.fConfig_PipeDiameter);	
#endif
    /*change37*/
    fnSPI_EEPROM_ReadData(HMI_TIME_OFFSET_SIGN, 1, &stConfigData.ucConfig_TimeOffsetSign);	
#ifdef GENERAL_DBG_EE
    printf("102)HMI_TIME_OFFSET_SIGN = %d\r\n", stConfigData.ucConfig_TimeOffsetSign);
#endif
    
    fnSPI_EEPROM_ReadData(HMI_TIME_OFFSET_HOUR, 1, &stConfigData.ucConfig_TimeOffsetHour);	
#ifdef GENERAL_DBG_EE
    printf("103)HMI_TIME_OFFSET_HOUR = %d\r\n", stConfigData.ucConfig_TimeOffsetHour);
#endif
    
    fnSPI_EEPROM_ReadData(HMI_TIME_OFFSET_MIN, 1, &stConfigData.ucConfig_TimeOffsetMin);	
#ifdef GENERAL_DBG_EE
    printf("104)HMI_TIME_OFFSET_MIN = %d\r\n", stConfigData.ucConfig_TimeOffsetMin);
#endif
    
    fnSPI_EEPROM_ReadData(HMI_TIME_OFFSET_SEC, 1, &stConfigData.ucConfig_TimeOffsetSec);	
#ifdef GENERAL_DBG_EE
    printf("105)HMI_TIME_OFFSET_SEC = %d\r\n", stConfigData.ucConfig_TimeOffsetSec);
#endif
 
 /*change42*/
   /******************************************** JOB ID ***************************************/
    fnSPI_EEPROM_ReadData(JOB_ID1, 7, stConfigData.rgucConfig_JobId1);
    stConfigData.rgucConfig_JobId1[7] =  NULL;
#ifdef GENERAL_DBG_EE
    printf("91)JOB_ID1 = %s\r\n", stConfigData.rgucConfig_JobId1);
#endif
    
   /*change48*/
    fnSPI_EEPROM_ReadData(CURRENT_INDEX_JOB_ID, 1, &ucJobID);	
		fnSPI_EEPROM_ReadData(LOG_INDEX_JOB_ID, 1, &ucJobLog);	
			
#ifdef GENERAL_DBG_EE
    printf("46)CURRENT_INDEX_JOB_ID = %d\r\n", ucJobID);
#endif
	
    fnSPI_EEPROM_ReadData(JOB_ID2, 7, stConfigData.rgucConfig_JobId2);
    stConfigData.rgucConfig_JobId2[7] =  NULL;
#ifdef GENERAL_DBG_EE
    printf("92)JOB_ID2 = %s\r\n", stConfigData.rgucConfig_JobId2);
#endif
    
    fnSPI_EEPROM_ReadData(JOB_ID3, 7,stConfigData.rgucConfig_JobId3);
    stConfigData.rgucConfig_JobId3[7] =  NULL;
#ifdef GENERAL_DBG_EE
    printf("93)JOB_ID3 = %s\r\n", stConfigData.rgucConfig_JobId3);
#endif
 
/*change43*/
 SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(BASE_TOTAL_MAT_USED, 4, SplitByte.rgucData);
    stCurrentData.fCurr_BaseTotalMaterialUsed = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("106)BASE_TOTAL_MAT_USED = %f\r\n", stCurrentData.fCurr_BaseTotalMaterialUsed);	
#endif	

SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(HARD_TOTAL_MAT_USED, 4, SplitByte.rgucData);
    stCurrentData.fCurr_HardTotalMaterialUsed = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("106)HARD_TOTAL_MAT_USED = %f\r\n", stCurrentData.fCurr_HardTotalMaterialUsed);	
#endif

SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(BASE_PRIME_MAT_ACC, 4, SplitByte.rgucData);
    stCurrentData.fCurr_BaseMaterialPrimedAccumulate = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("106)BASE_PRIME_MAT_ACC = %f\r\n", stCurrentData.fCurr_BaseMaterialPrimedAccumulate);	
#endif

SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(HARD_PRIME_MAT_ACC, 4, SplitByte.rgucData);
    stCurrentData.fCurr_HardMaterialPrimedAccumulate = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("106)HARD_PRIME_MAT_ACC = %f\r\n", stCurrentData.fCurr_HardMaterialPrimedAccumulate);	
#endif

SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(BASE_APPLIED_MAT_ACC, 4, SplitByte.rgucData);
    stCurrentData.fCurr_BaseMaterialAppliedAccumulate = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("106)BASE_APPLIED_MAT_ACC = %f\r\n", stCurrentData.fCurr_BaseMaterialAppliedAccumulate);	
#endif
SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(HARD_APPLIED_MAT_ACC, 4, SplitByte.rgucData);
    stCurrentData.fCurr_HardMaterialAppliedAccumulate = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("106)HARD_APPLIED_MAT_ACC = %f\r\n", stCurrentData.fCurr_HardMaterialAppliedAccumulate);	
#endif
SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(BASE_TOTAL_MAT_USED_ACC, 4, SplitByte.rgucData);
    stCurrentData.fCurr_BaseTotalMaterialUsedAccumulate = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("106)BASE_TOTAL_MAT_USED_ACC = %f\r\n", stCurrentData.fCurr_BaseTotalMaterialUsedAccumulate);	
#endif
SplitByte.uliData = 0;
    fnSPI_EEPROM_ReadData(HARD_TOTAL_MAT_USED_ACC, 4, SplitByte.rgucData);
    stCurrentData.fCurr_HardTotalMaterialUsedAccumulate = SplitByte.fData;
#ifdef GENERAL_DBG_EE
    printf("106)HARD_TOTAL_MAT_USED_ACC = %f\r\n", stCurrentData.fCurr_HardTotalMaterialUsedAccumulate);	
#endif

#endif


  }
}


/*****************************************************************************
**@Function 		  : 	fnWifiApplysSetting
**@Descriptions	: 	Process Received Data through Wifi
**@parameters		: 	None
**@return				: 	None
*****************************************************************************/ 
void fnWifiApplysSetting(void) 
{
	/*change45*/
			uint8_t rgucConfig[100];
			fnWifiCommandMode();
			strcpy((char *)stWifiRxIP.rgucIP_Addr,(char *)stWifiIP.rgucIP_Addr);
			sprintf((char *)rgucConfig,"set ip addr %s",(char *)stWifiIP.rgucIP_Addr);
			fnWifiCommand(rgucConfig);
			
			strcpy((char *)stWifiRxIP.rgucSubnet_Addr,(char *)stWifiIP.rgucSubnet_Addr);
			sprintf((char *)rgucConfig,"set ip net %s",(char *)stWifiIP.rgucSubnet_Addr);
			fnWifiCommand(rgucConfig);
			
			strcpy((char *)stWifiRxIP.rgucGateway_Addr,(char *)stWifiIP.rgucGateway_Addr);
			sprintf((char *)rgucConfig,"set ip gateway %s",(char *)stWifiIP.rgucGateway_Addr);
			fnWifiCommand(rgucConfig);
			
			stWifiRxIP.uiPort=stWifiIP.uiPort;
			sprintf((char *)rgucConfig,"set ip localport %d",stWifiIP.uiPort);
			fnWifiCommand(rgucConfig);
			
			strcpy((char *)stWifiRxIP.rgucSSID,(char *)stWifiIP.rgucSSID);
			sprintf((char *)rgucConfig,"set apmode ssid %s",(char *)stWifiIP.rgucSSID);
			fnWifiCommand(rgucConfig);
			
			strcpy((char *)stWifiRxIP.rgucPASWD,(char *)stWifiIP.rgucPASWD);
			sprintf((char *)rgucConfig,"set apmode passphrase %s",(char *)stWifiIP.rgucPASWD);
			fnWifiCommand(rgucConfig);
			
			fnWifiCommand("save");
			
			fnWifiCommand("reboot");
	
}

/*****************************************************************************
**@Function 	 		: fnsetDefaultvalues
**@Descriptions	: Initialy Set The Default Values To The All The Parameters  
**@parameters		: None
**@return				: None
*****************************************************************************/
void fnsetDefaultvalues(void)
{
  
  /***************************VCM*********************************/
  /* CONFIG_PARA_1 VCM_UPSTROKE_ADD iConfig_UpStrokeTH*/
  {
    stConfigData.iConfig_UpStrokeTH = DEFAULT_VCM_UPSTROKE_TH;
    SplitByte.iData = stConfigData.iConfig_UpStrokeTH;
    fnSPI_EEPROM_WriteData(VCM_UPSTROKE_ADD, 2, SplitByte.rgucData);
  }
  /* CONFIG_PARA_2 VCM_DOWNSTROKE_ADD iConfig_DownStrokeTH*/
  {
    stConfigData.iConfig_DownStrokeTH = DEFAULT_VCM_DOWNSTROKE_TH;
    SplitByte.iData = stConfigData.iConfig_DownStrokeTH;
    fnSPI_EEPROM_WriteData(VCM_DOWNSTROKE_ADD, 2, SplitByte.rgucData);
    
  }
  /* CONFIG_PARA_3 VCM_PRESSURE_LOWTH1 fConfig_PressureGauge1LTH*/
  
  {
    stConfigData.fConfig_PressureGauge1LTH = DEFAULT_VCM_PRESSURE_LOWTH1;
    SplitByte.fData = stConfigData.fConfig_PressureGauge1LTH;
    fnSPI_EEPROM_WriteData(VCM_PRESSURE_LOWTH1, 4, SplitByte.rgucData);
    
  }
  /* CONFIG_PARA_4 VCM_PRESSURE_HIGHTH1 fConfig_PressureGauge1HTH*/
  
  {
    stConfigData.fConfig_PressureGauge1HTH = DEFAULT_VCM_PRESSURE_HIGHTH1;
    SplitByte.fData = stConfigData.fConfig_PressureGauge1HTH;
    fnSPI_EEPROM_WriteData(VCM_PRESSURE_HIGHTH1, 4, SplitByte.rgucData);
    
  }
  /* CONFIG_PARA_5 VCM_PRESSURE_LOWTH2 fConfig_PressureGauge2LTH*/
  
  {
    stConfigData.fConfig_PressureGauge2LTH =DEFAULT_VCM_PRESSURE_LOWTH2;
    SplitByte.fData = stConfigData.fConfig_PressureGauge2LTH;
    fnSPI_EEPROM_WriteData(VCM_PRESSURE_LOWTH2, 4, SplitByte.rgucData);
  }
  /* CONFIG_PARA_6 VCM_PRESSURE_HIGHTH2 fConfig_PressureGauge2HTH*/
  
  {
    stConfigData.fConfig_PressureGauge2HTH = DEFAULT_VCM_PRESSURE_HIGHTH2;
    SplitByte.fData = stConfigData.fConfig_PressureGauge2HTH;
    fnSPI_EEPROM_WriteData(VCM_PRESSURE_HIGHTH2, 4, SplitByte.rgucData);
  }
  /* CONFIG_PARA_7 VCM_PRESSURE_LOWTH3 fConfig_PressureGauge3LTH*/
  
  {
    stConfigData.fConfig_PressureGauge3LTH = DEFAULT_VCM_PRESSURE_LOWTH3;
    SplitByte.fData = stConfigData.fConfig_PressureGauge3LTH ;
    fnSPI_EEPROM_WriteData(VCM_PRESSURE_LOWTH3, 4, SplitByte.rgucData);
  }
  /* CONFIG_PARA_8 VCM_PRESSURE_HIGHTH3 fConfig_PressureGauge3HTH*/
  
  {
    stConfigData.fConfig_PressureGauge3HTH = DEFAULT_VCM_PRESSURE_HIGHTH3;
    SplitByte.fData = stConfigData.fConfig_PressureGauge3HTH;
    fnSPI_EEPROM_WriteData(VCM_PRESSURE_HIGHTH3, 4, SplitByte.rgucData);
    
  }
  /* CONFIG_PARA_9 VCM_CYLINDER_AREA1 fConfig_AreaofCylinderA*/
  
  {
    stConfigData.fConfig_AreaofCylinderA = DEFAULT_VCM_CYLINDER_AREA1;
    SplitByte.fData = stConfigData.fConfig_AreaofCylinderA;
    fnSPI_EEPROM_WriteData(VCM_CYLINDER_AREA1, 4, SplitByte.rgucData);
  }
  /* CONFIG_PARA_10 VCM_CYLINDER_AREA2 fConfig_AreaofCylinderB*/
  
  {
    stConfigData.fConfig_AreaofCylinderB = DEFAULT_VCM_CYLINDER_AREA2;
    SplitByte.fData = stConfigData.fConfig_AreaofCylinderB ;
    fnSPI_EEPROM_WriteData(VCM_CYLINDER_AREA2, 4, SplitByte.rgucData);
  }
  /* CONFIG_PARA_11 VCM_CYLINDER_AREA3 fConfig_AreaofCylinderC*/
  
  {
    stConfigData.fConfig_AreaofCylinderC = DEFAULT_VCM_CYLINDER_AREA3;
    SplitByte.fData = stConfigData.fConfig_AreaofCylinderC;
    fnSPI_EEPROM_WriteData(VCM_CYLINDER_AREA3, 4, SplitByte.rgucData);
  }
  /* CONFIG_PARA_12 VCM_CYLINDER_COUNT ucConfig_CylinderCount*/
  
  {
    stConfigData.ucConfig_CylinderCount = DEFAULT_VCM_CYLINDER_COUNT;
    fnSPI_EEPROM_WriteData(VCM_CYLINDER_COUNT, 1, &stConfigData.ucConfig_CylinderCount);
  }
  /* CONFIG_PARA_13 VCM_PRESSURE_GAUGE1_CONN ucConfig_ConnPressureGuage1*/
  
  {
    stConfigData.ucConfig_ConnPressureGuage1 = DEFAULT_VCM_PRESSURE_GAUGE1_CONN;
    stConfigData.ucConfig_ConnPressureGuage2 = DEFAULT_VCM_PRESSURE_GAUGE2_CONN;
    stConfigData.ucConfig_ConnPressureGuage3 = DEFAULT_VCM_PRESSURE_GAUGE3_CONN;
    fnSPI_EEPROM_WriteData(VCM_PRESSURE_GAUGE1_CONN, 1, &stConfigData.ucConfig_ConnPressureGuage1); 
    fnSPI_EEPROM_WriteData(VCM_PRESSURE_GAUGE2_CONN, 1, &stConfigData.ucConfig_ConnPressureGuage2); 
    fnSPI_EEPROM_WriteData(VCM_PRESSURE_GAUGE3_CONN, 1, &stConfigData.ucConfig_ConnPressureGuage3); 
  }
  /* CONFIG_PARA_14 VCM_LINEAR_TRANSDUCER_CONN ucConfig_ConnLinearTransducer*/
  
  {
    stConfigData.ucConfig_ConnLinearTransducer = DEFAULT_VCM_LINEAR_TRANSDUCER_CONN;
    fnSPI_EEPROM_WriteData(VCM_LINEAR_TRANSDUCER_CONN, 1, &stConfigData.ucConfig_ConnLinearTransducer); 
  }
  /* CONFIG_PARA_15 VCM_EE07_INSIDE_CONN ucConfig_ConnInsideEE07*/
  
  {
    stConfigData.ucConfig_ConnInsideEE07 = DEFAULT_VCM_EE07_INSIDE_CONN;
    stConfigData.ucConfig_ConnOutsideEE07 = DEFAULT_VCM_EE07_OUTSIDE_CONN; 
    fnSPI_EEPROM_WriteData(VCM_EE07_INSIDE_CONN, 1, &stConfigData.ucConfig_ConnInsideEE07); 
    fnSPI_EEPROM_WriteData(VCM_EE07_OUTSIDE_CONN, 1, &stConfigData.ucConfig_ConnOutsideEE07); 
  }
  /* CONFIG_PARA_16 VCM_STALL_VALVE_CONN ucConfig_ConnStallvalve VCM_RECIRC_VALVE_CONN ucConfig_ConnRecirculationvalve*/
  
  {
    stConfigData.ucConfig_ConnStallvalve = DEFAULT_VCM_STALL_VALVE_CONN;
    fnSPI_EEPROM_WriteData(VCM_STALL_VALVE_CONN, 1, &stConfigData.ucConfig_ConnStallvalve); 
		
		/*change53*/
		 stConfigData.ucConfig_ConnRecirculationvalve = DEFAULT_VCM_RECIRC_VALVE_CONN;
    fnSPI_EEPROM_WriteData(VCM_RECIRC_VALVE_CONN, 1, &stConfigData.ucConfig_ConnRecirculationvalve); 
  }
  /* CONFIG_PARA_17 VCM_BYPASS ucConfig_VCMBypass*/
  
  { 
    stConfigData.ucConfig_VCMBypass = DEFAULT_VCM_BYPASS;
    fnSPI_EEPROM_WriteData(VCM_BYPASS, 1, &stConfigData.ucConfig_VCMBypass); 
  }
  /* CONFIG_PARA_18 VCM_BEFORE_STALL fConfig_TimeBeforeStall*/
  
  { 
    stConfigData.fConfig_TimeBeforeStall = DEFAULT_VCM_BEFORE_STALL;
    SplitByte.fData = stConfigData.fConfig_TimeBeforeStall;
    fnSPI_EEPROM_WriteData(VCM_BEFORE_STALL, 4, SplitByte.rgucData);
  }
  /* CONFIG_PARA_19 VCM_COMPARE_STALL fConfig_TimeCompareStall*/
  
  { 
    stConfigData.fConfig_TimeCompareStall = DEFAULT_VCM_COMPARE_STALL;
    SplitByte.fData = stConfigData.fConfig_TimeCompareStall;
    fnSPI_EEPROM_WriteData(VCM_COMPARE_STALL, 4, SplitByte.rgucData);
  }
  
  
  /***************************HCM*********************************/	
  /* CONFIG_PARA_20 HCM_ANTI_SET_TEMP fConfig_AntiFrzTempSet*/
  
  { 
    stConfigData.fConfig_AntiFrzTempSet = DEFAULT_HCM_ANTI_SET_TEMP;
    SplitByte.fData = stConfigData.fConfig_AntiFrzTempSet;
    fnSPI_EEPROM_WriteData(HCM_ANTI_SET_TEMP, 4, SplitByte.rgucData);
  }
  /* CONFIG_PARA_21 HCM_ANTI_THRESH_TEMP_LOW fConfig_AntiFrzTempLTH*/
  
  {
    stConfigData.fConfig_AntiFrzTempLTH = DEFAULT_HCM_ANTI_THRESH_TEMP_LOW;
    SplitByte.fData = stConfigData.fConfig_AntiFrzTempLTH;
    fnSPI_EEPROM_WriteData(HCM_ANTI_THRESH_TEMP_LOW, 4, SplitByte.rgucData);
  }
  /* CONFIG_PARA_22 HCM_ANTI_THRESH_TEMP_HIGH fConfig_AntiFrzTempHTH*/
  
  {
    stConfigData.fConfig_AntiFrzTempHTH = DEFAULT_HCM_ANTI_THRESH_TEMP_HIGH;
    SplitByte.fData = stConfigData.fConfig_AntiFrzTempHTH;
    fnSPI_EEPROM_WriteData(HCM_ANTI_THRESH_TEMP_HIGH, 4, SplitByte.rgucData);
  }
  /* CONFIG_PARA_23 HCM_ANTI_HYST_TEMP_POS fConfig_HysAntiFrzPos*/
  
  { 
    stConfigData.fConfig_HysAntiFrzPos = DEFAULT_HCM_ANTI_HYST_TEMP_POS;
    SplitByte.fData = stConfigData.fConfig_HysAntiFrzPos;
    fnSPI_EEPROM_WriteData(HCM_ANTI_HYST_TEMP_POS, 4, SplitByte.rgucData);
  }
  /* CONFIG_PARA_24 HCM_ANTI_HYST_TEMP_NEG fConfig_HysAntiFrzNeg*/
  
  {
    stConfigData.fConfig_HysAntiFrzNeg = DEFAULT_HCM_ANTI_HYST_TEMP_NEG;
    SplitByte.fData = stConfigData.fConfig_HysAntiFrzNeg;
    fnSPI_EEPROM_WriteData(HCM_ANTI_HYST_TEMP_NEG, 4, SplitByte.rgucData);
  }
  /* CONFIG_PARA_25 HCM_INLINE_BASE_SET_TEMP fConfig_InlineBaseTempSet*/
  
  {
    stConfigData.fConfig_InlineBaseTempSet = DEFAULT_HCM_INLINE_BASE_SET_TEMP;
    SplitByte.fData = stConfigData.fConfig_InlineBaseTempSet;
    fnSPI_EEPROM_WriteData(HCM_INLINE_BASE_SET_TEMP, 4, SplitByte.rgucData);
  }
  /* CONFIG_PARA_26 HCM_INLINE_BASE_THRESH_TEMP_LOW fConfig_InlineBaseTempLTH*/
  
  { stConfigData.fConfig_InlineBaseTempLTH = DEFAULT_HCM_INLINE_BASE_THRESH_TEMP_LOW;
  SplitByte.fData = stConfigData.fConfig_InlineBaseTempLTH;
  fnSPI_EEPROM_WriteData(HCM_INLINE_BASE_THRESH_TEMP_LOW, 4, SplitByte.rgucData);
  }
  /* CONFIG_PARA_27 HCM_INLINE_BASE_THRESH_TEMP_HIGH fConfig_InlineBaseTempHTH*/
  
  {stConfigData.fConfig_InlineBaseTempHTH = DEFAULT_HCM_INLINE_BASE_THRESH_TEMP_HIGH;
  SplitByte.fData = stConfigData.fConfig_InlineBaseTempHTH;
  fnSPI_EEPROM_WriteData(HCM_INLINE_BASE_THRESH_TEMP_HIGH, 4, SplitByte.rgucData);
  }
  /* CONFIG_PARA_28 HCM_INLINE_BASE_HYST_TEMP_POS fConfig_HysInlineBasePos*/
  
  { stConfigData.fConfig_HysInlineBasePos = DEFAULT_HCM_INLINE_BASE_HYST_TEMP_POS;
  SplitByte.fData = stConfigData.fConfig_HysInlineBasePos;
  fnSPI_EEPROM_WriteData(HCM_INLINE_BASE_HYST_TEMP_POS, 4, SplitByte.rgucData);
  }
  /* CONFIG_PARA_29 HCM_INLINE_BASE_HYST_TEMP_NEG fConfig_HysInlineBaseNeg*/
  
  { stConfigData.fConfig_HysInlineBaseNeg = DEFAULT_HCM_INLINE_BASE_HYST_TEMP_NEG;
  SplitByte.fData = stConfigData.fConfig_HysInlineBaseNeg;
  fnSPI_EEPROM_WriteData(HCM_INLINE_BASE_HYST_TEMP_NEG, 4, SplitByte.rgucData);
  }
  /* CONFIG_PARA_30 HCM_INLINE_HARD_SET_TEMP fConfig_InlineHardTempSet*/
  
  { stConfigData.fConfig_InlineHardTempSet = DEFAULT_HCM_INLINE_HARD_SET_TEMP;
  SplitByte.fData = stConfigData.fConfig_InlineHardTempSet;
  fnSPI_EEPROM_WriteData(HCM_INLINE_HARD_SET_TEMP, 4, SplitByte.rgucData);
  }
  /* CONFIG_PARA_31 HCM_INLINE_HARD_THRESH_TEMP_LOW fConfig_InlineHardTempLTH*/
  
  {stConfigData.fConfig_InlineHardTempLTH = DEFAULT_HCM_INLINE_HARD_THRESH_TEMP_LOW;
  SplitByte.fData = stConfigData.fConfig_InlineHardTempLTH;
  fnSPI_EEPROM_WriteData(HCM_INLINE_HARD_THRESH_TEMP_LOW, 4, SplitByte.rgucData);
  }
  /* CONFIG_PARA_32 HCM_INLINE_HARD_THRESH_TEMP_HIGH fConfig_InlineHardTempHTH*/
  
  { stConfigData.fConfig_InlineHardTempHTH = DEFAULT_HCM_INLINE_HARD_THRESH_TEMP_HIGH;
  SplitByte.fData = stConfigData.fConfig_InlineHardTempHTH;
  fnSPI_EEPROM_WriteData(HCM_INLINE_HARD_THRESH_TEMP_HIGH, 4, SplitByte.rgucData);
  }
  /* CONFIG_PARA_33 HCM_INLINE_HARD_HYST_TEMP_POS fConfig_InlineHysHardPos*/
  
  {stConfigData.fConfig_InlineHysHardPos = DEFAULT_HCM_INLINE_HARD_HYST_TEMP_POS;
  SplitByte.fData = stConfigData.fConfig_InlineHysHardPos;
  fnSPI_EEPROM_WriteData(HCM_INLINE_HARD_HYST_TEMP_POS, 4, SplitByte.rgucData);
  }
  /* CONFIG_PARA_34 HCM_INLINE_HARD_HYST_TEMP_NEG fConfig_InlineHysHardNeg*/
  
  { stConfigData.fConfig_InlineHysHardNeg = DEFAULT_HCM_INLINE_HARD_HYST_TEMP_NEG;
  SplitByte.fData = stConfigData.fConfig_InlineHysHardNeg;
  fnSPI_EEPROM_WriteData(HCM_INLINE_HARD_HYST_TEMP_NEG, 4, SplitByte.rgucData);
  }
  /* CONFIG_PARA_35 HCM_BASE_LIQUID_THRESH_LEVEL_LOW fConfig_BaseLiquidLevelLTH*/
  
  { stConfigData.fConfig_BaseLiquidLevelLTH = DEFAULT_HCM_BASE_LIQUID_THRESH_LEVEL_LOW;
  SplitByte.fData = stConfigData.fConfig_BaseLiquidLevelLTH;
  fnSPI_EEPROM_WriteData(HCM_BASE_LIQUID_THRESH_LEVEL_LOW, 4, SplitByte.rgucData);
  }
  /* CONFIG_PARA_36 HCM_BASE_LIQUID_THRESH_LEVEL_HIGH fConfig_BaseLiquidLevelHTH*/
  
  {stConfigData.fConfig_BaseLiquidLevelHTH = DEFAULT_HCM_BASE_LIQUID_THRESH_LEVEL_HIGH;
  SplitByte.fData = stConfigData.fConfig_BaseLiquidLevelHTH;
  fnSPI_EEPROM_WriteData(HCM_BASE_LIQUID_THRESH_LEVEL_HIGH, 4, SplitByte.rgucData);
  }
  /* CONFIG_PARA_37 HCM_HARD_LIQUID_THRESH_LEVEL_LOW fConfig_HardLiquidLevelLTH*/
  
  { stConfigData.fConfig_HardLiquidLevelLTH = DEFAULT_HCM_HARD_LIQUID_THRESH_LEVEL_LOW;
  SplitByte.fData = stConfigData.fConfig_HardLiquidLevelLTH;
  fnSPI_EEPROM_WriteData(HCM_HARD_LIQUID_THRESH_LEVEL_LOW, 4, SplitByte.rgucData);
  }
  /* CONFIG_PARA_38 HCM_HARD_LIQUID_THRESH_LEVEL_HIGH fConfig_HardLiquidLevelHTH*/
  
  { stConfigData.fConfig_HardLiquidLevelHTH = DEFAULT_HCM_HARD_LIQUID_THRESH_LEVEL_HIGH;
  SplitByte.fData = stConfigData.fConfig_HardLiquidLevelHTH;
  fnSPI_EEPROM_WriteData(HCM_HARD_LIQUID_THRESH_LEVEL_HIGH, 4, SplitByte.rgucData);
  }
  /* CONFIG_PARA_39 HCM_BASE_TANK_HEIGHT fConfig_BaseTankHeight*/
  
  { stConfigData.fConfig_BaseTankHeight = DEFAULT_HCM_BASE_TANK_HEIGHT;
  SplitByte.fData = stConfigData.fConfig_BaseTankHeight;
  fnSPI_EEPROM_WriteData(HCM_BASE_TANK_HEIGHT, 4, SplitByte.rgucData);
  }
  /* CONFIG_PARA_40 HCM_HARD_TANK_HEIGHT fConfig_HardTankHeight*/
  
  {stConfigData.fConfig_HardTankHeight = DEFAULT_HCM_HARD_TANK_HEIGHT;
  SplitByte.fData = stConfigData.fConfig_HardTankHeight;
  fnSPI_EEPROM_WriteData(HCM_HARD_TANK_HEIGHT, 4, SplitByte.rgucData);
  }
  /* CONFIG_PARA_41 HCM_LIQUID_LEVEL_SAMPLING_RATE uiConfig_LLSamplingRate*/
  
  {
    stConfigData.uiConfig_LLSamplingRate = DEFAULT_HCM_LIQUID_LEVEL_SAMPLING_RATE;
    SplitByte.iData = stConfigData.uiConfig_LLSamplingRate;
    fnSPI_EEPROM_WriteData(HCM_LIQUID_LEVEL_SAMPLING_RATE, 2, SplitByte.rgucData);
  }
  /* CONFIG_PARA_42 HCM_INLINE_BASE_THERM_CONN ucConfig_ConnBaseinlinether*/
  
  { stConfigData.ucConfig_ConnBaseinlinether = DEFAULT_HCM_INLINE_BASE_THERM_CONN;
  fnSPI_EEPROM_WriteData(HCM_INLINE_BASE_THERM_CONN, 1, &stConfigData.ucConfig_ConnBaseinlinether);
  stConfigData.ucConfig_ConnHardinlinether = DEFAULT_HCM_INLINE_HARD_THERM_CONN;
  fnSPI_EEPROM_WriteData(HCM_INLINE_HARD_THERM_CONN, 1, &stConfigData.ucConfig_ConnHardinlinether);
  stConfigData.ucConfig_ConnAntifreezether = DEFAULT_HCM_INLINE_ANTIFREEZE_THERM_CONN;
  fnSPI_EEPROM_WriteData(HCM_INLINE_ANTIFREEZE_THERM_CONN, 1, &stConfigData.ucConfig_ConnAntifreezether);
  stConfigData.ucConfig_ConnBasereturnlinether = DEFAULT_HCM_RETURN_BASE_THERM_CONN;
  fnSPI_EEPROM_WriteData(HCM_RETURN_BASE_THERM_CONN, 1, &stConfigData.ucConfig_ConnBasereturnlinether);
  }
  /* CONFIG_PARA_43 HCM_INLINE_BASE_HEATER_CONN ucConfig_ConnBaseinlineheater*/
  
  { stConfigData.ucConfig_ConnBaseinlineheater = DEFAULT_HCM_INLINE_BASE_HEATER_CONN;
  fnSPI_EEPROM_WriteData(HCM_INLINE_BASE_HEATER_CONN, 1, &stConfigData.ucConfig_ConnBaseinlineheater);
  stConfigData.ucConfig_ConnHardinlineheater = DEFAULT_HCM_INLINE_HARD_HEATER_CONN;
  fnSPI_EEPROM_WriteData(HCM_INLINE_HARD_HEATER_CONN, 1, &stConfigData.ucConfig_ConnHardinlineheater);
  stConfigData.ucConfig_ConnAntifreezeheater = DEFAULT_HCM_INLINE_ANTIFREEZE_HEATER_CONN;
  fnSPI_EEPROM_WriteData(HCM_INLINE_ANTIFREEZE_HEATER_CONN, 1, &stConfigData.ucConfig_ConnAntifreezeheater);
  stConfigData.ucConfig_ConnHardheater	= DEFAULT_HCM_HARD_HEATER_CONN;
  fnSPI_EEPROM_WriteData(HCM_HARD_HEATER_CONN, 1, &stConfigData.ucConfig_ConnHardheater);
  }
  /* CONFIG_PARA_44 HCM_BASE_LIQUID_LEVEL_CONN ucConfig_ConnBasetankLL*/
  
  { 
    stConfigData.ucConfig_ConnBasetankLL = DEFAULT_HCM_BASE_LIQUID_LEVEL_CONN;
    fnSPI_EEPROM_WriteData(HCM_BASE_LIQUID_LEVEL_CONN, 1, &stConfigData.ucConfig_ConnBasetankLL);
    stConfigData.ucConfig_ConnHardtankLL = DEFAULT_HCM_HARD_LIQUID_LEVEL_CONN;
    fnSPI_EEPROM_WriteData(HCM_HARD_LIQUID_LEVEL_CONN, 1, &stConfigData.ucConfig_ConnHardtankLL);
  }
  /* CONFIG_PARA_45 HCM_BYPASS uiConfig_HCMBypass*/
  
  {
    stConfigData.uiConfig_HCMBypass = DEFAULT_HCM_BYPASS;
    SplitByte.iData = stConfigData.uiConfig_HCMBypass;
    fnSPI_EEPROM_WriteData(HCM_BYPASS, 2, SplitByte.rgucData);
  }
  
  
  /***************************OPMM*********************************/	
  
  /* CONFIG_PARA_46 OPMM_SPRAY_COUNT ucConfig_SprayCount*/
  
  {
    stConfigData.ucConfig_SprayCount = DEFAULT_OPMM_SPRAY_COUNT;
    fnSPI_EEPROM_WriteData(OPMM_SPRAY_COUNT, 1, &stConfigData.ucConfig_SprayCount);
  }
  
  
  /* CONFIG_PARA_47 OPMM_PRIME_TIME fConfig_PrimeTime*/
  
  {stConfigData.fConfig_PrimeTime = DEFAULT_OPMM_PRIME_TIME;
  SplitByte.fData = stConfigData.fConfig_PrimeTime ;
  fnSPI_EEPROM_WriteData(OPMM_PRIME_TIME, 4, SplitByte.rgucData);
  }
  /* CONFIG_PARA_48 OPMM_GUN_DELAY_TIME fConfig_GunDelayTime*/
  
  { stConfigData.fConfig_GunDelayTime = DEFAULT_OPMM_GUN_DELAY_TIME;
  SplitByte.fData = stConfigData.fConfig_GunDelayTime;
  fnSPI_EEPROM_WriteData(OPMM_GUN_DELAY_TIME, 4, SplitByte.rgucData);	
  }
  /* CONFIG_PARA_49 OPMM_SPRAY_OVERLAPTIME fConfig_SprayOverlapTime*/
  
  {
    stConfigData.fConfig_SprayOverlapTime = DEFAULT_OPMM_SPRAY_OVERLAPTIME;
    SplitByte.fData = stConfigData.fConfig_SprayOverlapTime;
    fnSPI_EEPROM_WriteData(OPMM_SPRAY_OVERLAPTIME, 4, SplitByte.rgucData);
  }
  /* CONFIG_PARA_50 OPMM_REVERSE_DELAYTIME fConfig_ReverseDelayTime*/
  
  { stConfigData.fConfig_ReverseDelayTime = DEFAULT_OPMM_REVERSE_DELAYTIME;
  SplitByte.fData = stConfigData.fConfig_ReverseDelayTime;
  fnSPI_EEPROM_WriteData(OPMM_REVERSE_DELAYTIME, 4, SplitByte.rgucData);
  }
  /* CONFIG_PARA_51 OPMM_FLUSH_TIME fConfig_FlushTime*/
  
  { stConfigData.fConfig_FlushTime = DEFAULT_OPMM_FLUSH_TIME;
  SplitByte.fData = stConfigData.fConfig_FlushTime;
  fnSPI_EEPROM_WriteData(OPMM_FLUSH_TIME, 4, SplitByte.rgucData);
  }
  /* CONFIG_PARA_52 OPMM_DCLS_HOME_REACH_TIME fConfig_DCLSTime*/
  
  { stConfigData.fConfig_DCLSTime = DEFAULT_OPMM_DCLS_HOME_REACH_TIME;
  SplitByte.fData = stConfigData.fConfig_DCLSTime;
  fnSPI_EEPROM_WriteData(OPMM_DCLS_HOME_REACH_TIME, 4, SplitByte.rgucData);
  }
  /* CONFIG_PARA_53 OPMM_HOME_DCLS_REACH_TIME fConfig_HOMETime*/
  
  { stConfigData.fConfig_HOMETime = DEFAULT_OPMM_HOME_DCLS_REACH_TIME;
  SplitByte.fData = stConfigData.fConfig_HOMETime;
  fnSPI_EEPROM_WriteData(OPMM_HOME_DCLS_REACH_TIME, 4, SplitByte.rgucData);
  }
  /* CONFIG_PARA_54 OPMM_GUN_POSITION_CHANGE_TIME fConfig_GunPosChangeTime*/
  
  {
    stConfigData.fConfig_GunPosChangeTime = DEFAULT_OPMM_GUN_POSITION_CHANGE_TIME;
    SplitByte.fData = stConfigData.fConfig_GunPosChangeTime;
    fnSPI_EEPROM_WriteData(OPMM_GUN_POSITION_CHANGE_TIME, 4, SplitByte.rgucData);
  }
  /* CONFIG_PARA_55 OPMM_DELAY_SPRAY_VALVE_FLUSH_VBAT_TIME fConfig_TimebeforeONSprayValve*/
  
  
  { stConfigData.fConfig_TimebeforeONSprayValve = DEFAULT_OPMM_DELAY_SPRAY_VALVE_FLUSH_VBAT_TIME;
  SplitByte.fData = stConfigData.fConfig_TimebeforeONSprayValve;
  fnSPI_EEPROM_WriteData(OPMM_DELAY_SPRAY_VALVE_FLUSH_VBAT_TIME, 4, SplitByte.rgucData);
  }
  /* CONFIG_PARA_56 OPMM_FLUSH_WARNNING_TIME fConfig_FlushWarnTime*/
  
  
  { stConfigData.fConfig_FlushWarnTime = DEFAULT_OPMM_FLUSH_WARNNING_TIME;
  SplitByte.fData = stConfigData.fConfig_FlushWarnTime;
  fnSPI_EEPROM_WriteData(OPMM_FLUSH_WARNNING_TIME, 4, SplitByte.rgucData);
  }
  /* CONFIG_PARA_57 OPMM_SPRAY_VALVE_CONN ucConfig_ConnSprayvalve*/
  
  
  { 
    stConfigData.ucConfig_ConnSprayvalve = DEFAULT_OPMM_SPRAY_VALVE_CONN;
    fnSPI_EEPROM_WriteData(OPMM_SPRAY_VALVE_CONN, 1, &stConfigData.ucConfig_ConnSprayvalve);
    stConfigData.ucConfig_ConnGunposition = DEFAULT_OPMM_GUNPOS_VALVE_CONN;
    fnSPI_EEPROM_WriteData(OPMM_GUNPOS_VALVE_CONN, 1, &stConfigData.ucConfig_ConnGunposition);
    stConfigData.ucConfig_ConnABvalve = DEFAULT_OPMM_AB_VALVE_CONN;
    fnSPI_EEPROM_WriteData(OPMM_AB_VALVE_CONN, 1, &stConfigData.ucConfig_ConnABvalve);
    stConfigData.ucConfig_ConnSolventvalve = DEFAULT_OPMM_SOLVENT_VALVE_CONN;
    fnSPI_EEPROM_WriteData(OPMM_SOLVENT_VALVE_CONN, 1, &stConfigData.ucConfig_ConnSolventvalve);
    stConfigData.ucConfig_ConnForwardvalve = DEFAULT_OPMM_FORWARD_VALVE_CONN;
    fnSPI_EEPROM_WriteData(OPMM_FORWARD_VALVE_CONN, 1, &stConfigData.ucConfig_ConnForwardvalve);
    stConfigData.ucConfig_ConnBackwardvalve = DEFAULT_OPMM_BACKWARD_VALVE_CONN;
    fnSPI_EEPROM_WriteData(OPMM_BACKWARD_VALVE_CONN, 1, &stConfigData.ucConfig_ConnBackwardvalve);
  }
  /* CONFIG_PARA_58 OPMM_BYPASS uiConfig_OPMMBypass*/
  
  { stConfigData.uiConfig_OPMMBypass = DEFAULT_OPMM_BYPASS;
  SplitByte.iData = stConfigData.uiConfig_OPMMBypass;
  fnSPI_EEPROM_WriteData(OPMM_BYPASS, 2, SplitByte.rgucData);
  }
  
  
  
  
  /***************************SCU*********************************/	
  /* CONFIG_PARA_59 SCU_SPRAY_PRESSURE_VALUE fConfig_SprayPressure*/
  
  { stConfigData.fConfig_SprayPressure = DEFAULT_SCU_SPRAY_PRESSURE_VALUE;
  SplitByte.fData = stConfigData.fConfig_SprayPressure;
  fnSPI_EEPROM_WriteData(SCU_SPRAY_PRESSURE_VALUE, 4, SplitByte.rgucData);
  }
  /* CONFIG_PARA_60 SCU_SPRAY_PRESSURE_THRESH_LOW fConfig_SprayPressureLTH*/
  
  { stConfigData.fConfig_SprayPressureLTH = DEFAULT_SCU_SPRAY_PRESSURE_THRESH_LOW;
  SplitByte.fData = stConfigData.fConfig_SprayPressureLTH;
  fnSPI_EEPROM_WriteData(SCU_SPRAY_PRESSURE_THRESH_LOW, 4, SplitByte.rgucData);
  }
  /* CONFIG_PARA_61 SCU_SPRAY_PRESSURE_THRESH_HIGH fConfig_SprayPressureHTH*/
  
  
  { stConfigData.fConfig_SprayPressureHTH = DEFAULT_SCU_SPRAY_PRESSURE_THRESH_HIGH;
  SplitByte.fData = stConfigData.fConfig_SprayPressureHTH;
  fnSPI_EEPROM_WriteData(SCU_SPRAY_PRESSURE_THRESH_HIGH, 4, SplitByte.rgucData);
  }
  /* CONFIG_PARA_62 SCU_SPRAY_PRESSURE_HYST_POS fConfig_HysSprayPresPos*/
  
  {  stConfigData.fConfig_HysSprayPresPos = DEFAULT_SCU_SPRAY_PRESSURE_HYST_POS;
  SplitByte.fData = stConfigData.fConfig_HysSprayPresPos;
  fnSPI_EEPROM_WriteData(SCU_SPRAY_PRESSURE_HYST_POS, 4, SplitByte.rgucData);
  }
  /* CONFIG_PARA_63 SCU_SPRAY_PRESSURE_HYST_NEG fConfig_HysSprayPresNeg*/
  
  { stConfigData.fConfig_HysSprayPresNeg = DEFAULT_SCU_SPRAY_PRESSURE_HYST_NEG;
  SplitByte.fData = stConfigData.fConfig_HysSprayPresNeg;
  fnSPI_EEPROM_WriteData(SCU_SPRAY_PRESSURE_HYST_NEG, 4, SplitByte.rgucData);
  }
  /* CONFIG_PARA_64 SCU_RECIRC_PRESSURE_VALUE fConfig_RecirPressure*/
  
  { stConfigData.fConfig_RecirPressure = DEFAULT_SCU_RECIRC_PRESSURE_VALUE;
  SplitByte.fData = stConfigData.fConfig_RecirPressure;
  fnSPI_EEPROM_WriteData(SCU_RECIRC_PRESSURE_VALUE, 4, SplitByte.rgucData);
  }
  /* CONFIG_PARA_65 SCU_RECIRC_PRESSURE_THRESH_LOW fConfig_RecirPressureLTH*/
  
  { stConfigData.fConfig_RecirPressureLTH = DEFAULT_SCU_RECIRC_PRESSURE_THRESH_LOW;
  SplitByte.fData = stConfigData.fConfig_RecirPressureLTH;
  fnSPI_EEPROM_WriteData(SCU_RECIRC_PRESSURE_THRESH_LOW, 4, SplitByte.rgucData);
  }
  /* CONFIG_PARA_66 SCU_RECIRC_PRESSURE_THRESH_HIGH fConfig_RecirPressureHTH*/;
  
  { stConfigData.fConfig_RecirPressureHTH = DEFAULT_SCU_RECIRC_PRESSURE_THRESH_HIGH;
  SplitByte.fData = stConfigData.fConfig_RecirPressureHTH;
  fnSPI_EEPROM_WriteData(SCU_RECIRC_PRESSURE_THRESH_HIGH, 4, SplitByte.rgucData);
  }
  /* CONFIG_PARA_67 SCU_RECIRC_PRESSURE_HYST_POS fConfig_HysRecircPresPos*/
  
  { stConfigData.fConfig_HysRecircPresPos = DEFAULT_SCU_RECIRC_PRESSURE_HYST_POS;
  SplitByte.fData = stConfigData.fConfig_HysRecircPresPos;
  fnSPI_EEPROM_WriteData(SCU_RECIRC_PRESSURE_HYST_POS, 4, SplitByte.rgucData);
  }
  /* CONFIG_PARA_68 SCU_RECIRC_PRESSURE_HYST_NEG fConfig_HysRecircPresNeg*/
  
  { stConfigData.fConfig_HysRecircPresNeg = DEFAULT_SCU_RECIRC_PRESSURE_HYST_NEG;
  SplitByte.fData = stConfigData.fConfig_HysRecircPresNeg;
  fnSPI_EEPROM_WriteData(SCU_RECIRC_PRESSURE_HYST_NEG, 4, SplitByte.rgucData);
  }
  /* CONFIG_PARA_69 COUNT_JOINT fConfig_JointNum*/
  
  { stConfigData.fConfig_JointNum = DEFAULT_COUNT_JOINT;
  SplitByte.fData = stConfigData.fConfig_JointNum;
  fnSPI_EEPROM_WriteData(COUNT_JOINT, 4, SplitByte.rgucData);
  }
  /* CONFIG_PARA_70 TIME_DATA_LOGGING fConfig_DataLoggingPeriod*/
  
  {  stConfigData.fConfig_DataLoggingPeriod = DEFAULT_TIME_DATA_LOGGING;
  SplitByte.fData = stConfigData.fConfig_DataLoggingPeriod;
  fnSPI_EEPROM_WriteData(TIME_DATA_LOGGING, 4, SplitByte.rgucData);
  }
  /* CONFIG_PARA_71 SYS_TIME ucConfig_SystemTime*/
  
  
  { 
    stConfigData.ucConfig_SystemTime[6] = DEFAULT_SYS_TIME_YEARL;
    stConfigData.ucConfig_SystemTime[5] = DEFAULT_SYS_TIME_YEARH;
    stConfigData.ucConfig_SystemTime[4] = DEFAULT_SYS_TIME_MONTH;
    stConfigData.ucConfig_SystemTime[3] = DEFAULT_SYS_TIME_DATE;
    stConfigData.ucConfig_SystemTime[2] = DEFAULT_SYS_TIME_SEC;
    stConfigData.ucConfig_SystemTime[1] = DEFAULT_SYS_TIME_MIN;
    stConfigData.ucConfig_SystemTime[0] = DEFAULT_SYS_TIME_HOUR;
    fnSPI_EEPROM_WriteData(SYS_TIME, 7, stConfigData.ucConfig_SystemTime);
  }
  /* CONFIG_PARA_72 SUPERVISOR_NAME1 rgucConfig_SupervisorName1*/
  
  { 
    strcpy((char *)stConfigData.rgucConfig_SupervisorName1,DEFAULT_SUPERVISOR_NAME1);
    //	*(long long *)stConfigData.rgucConfig_SupervisorName1 = *(long long *)DEFAULT_SUPERVISOR_NAME1;
    fnSPI_EEPROM_WriteData(SUPERVISOR_NAME1, 7, stConfigData.rgucConfig_SupervisorName1);
  }
  /* CONFIG_PARA_73 SUPERVISOR_NAME2 rgucConfig_SupervisorName2*/
  
  { strcpy((char *)stConfigData.rgucConfig_SupervisorName2,DEFAULT_SUPERVISOR_NAME2);
  //		*(long long *)stConfigData.rgucConfig_SupervisorName2 = *(long long *)DEFAULT_SUPERVISOR_NAME2;
  fnSPI_EEPROM_WriteData(SUPERVISOR_NAME2, 7, stConfigData.rgucConfig_SupervisorName2);
  }
  /* CONFIG_PARA_74 SUPERVISOR_NAME3 rgucConfig_SupervisorName3*/
  
  { 
    strcpy((char *)stConfigData.rgucConfig_SupervisorName3 ,DEFAULT_SUPERVISOR_NAME3);
    fnSPI_EEPROM_WriteData(SUPERVISOR_NAME3, 7,stConfigData.rgucConfig_SupervisorName3);
    /*change23*/
    ucSupervisorNameID = DEFAULT_CURRENT_INDEX_SUPERVISOR_NAME;
    ucSupervisorNameLog = DEFAULT_CURRENT_INDEX_SUPERVISOR_NAME;
		 fnSPI_EEPROM_WriteData(CURRENT_INDEX_SUPERVISOR_NAME, 1, &ucSupervisorNameID); /*change78*/
								 fnSPI_EEPROM_WriteData(LOG_INDEX_SUPERVISOR_NAME, 1, &ucSupervisorNameLog); /*change78*/
    /*change24*/
    fnResetName(FLASH_Sector_6);
    
  }
  /* CONFIG_PARA_75 COATING_MAT_NAME1 rgucConfig_CoatingMatName1*/
  
  { strcpy((char *)stConfigData.rgucConfig_CoatingMatName1,DEFAULT_COATING_MAT_NAME1);
  fnSPI_EEPROM_WriteData(COATING_MAT_NAME1, 7, stConfigData.rgucConfig_CoatingMatName1);
  }
  /* CONFIG_PARA_76 COATING_MAT_NAME2 rgucConfig_CoatingMatName2*/
  
  { strcpy((char *)stConfigData.rgucConfig_CoatingMatName2,DEFAULT_COATING_MAT_NAME2);
  fnSPI_EEPROM_WriteData(COATING_MAT_NAME2, 7, stConfigData.rgucConfig_CoatingMatName2);
  }
  /* CONFIG_PARA_77 COATING_MAT_NAME3 rgucConfig_CoatingMatName3*/
  
  { strcpy((char *)stConfigData.rgucConfig_CoatingMatName3,DEFAULT_COATING_MAT_NAME3);
  fnSPI_EEPROM_WriteData(COATING_MAT_NAME3, 7, stConfigData.rgucConfig_CoatingMatName3);
  }
  /* CONFIG_PARA_78 BASE_MAT_BATCHNO1 rgucConfig_BaseMatBatchNo1*/
  
  { strcpy((char *)stConfigData.rgucConfig_BaseMatBatchNo1,DEFAULT_BASE_MAT_BATCHNO1);
  fnSPI_EEPROM_WriteData(BASE_MAT_BATCHNO1, 7, stConfigData.rgucConfig_BaseMatBatchNo1);
  }
  /* CONFIG_PARA_79 BASE_MAT_BATCHNO2 rgucConfig_BaseMatBatchNo2*/
  
  { strcpy((char *)stConfigData.rgucConfig_BaseMatBatchNo2,DEFAULT_BASE_MAT_BATCHNO2);
  fnSPI_EEPROM_WriteData(BASE_MAT_BATCHNO2, 7, stConfigData.rgucConfig_BaseMatBatchNo2);
  }
  /* CONFIG_PARA_80 BASE_MAT_BATCHNO3 rgucConfig_BaseMatBatchNo3*/
  
  { strcpy((char *)stConfigData.rgucConfig_BaseMatBatchNo3,DEFAULT_BASE_MAT_BATCHNO3);
  fnSPI_EEPROM_WriteData(BASE_MAT_BATCHNO3, 7, stConfigData.rgucConfig_BaseMatBatchNo3);
  }
  /* CONFIG_PARA_81 HARD_MAT_BATCHNO1 rgucConfig_HardMatBatchNo1*/
  
  { strcpy((char *)stConfigData.rgucConfig_HardMatBatchNo1,DEFAULT_HARD_MAT_BATCHNO1);
  fnSPI_EEPROM_WriteData(HARD_MAT_BATCHNO1, 7, stConfigData.rgucConfig_HardMatBatchNo1);
  }
  /* CONFIG_PARA_82 HARD_MAT_BATCHNO2 rgucConfig_HardMatBatchNo2*/
  
  { strcpy((char *)stConfigData.rgucConfig_HardMatBatchNo2,DEFAULT_HARD_MAT_BATCHNO2);
  fnSPI_EEPROM_WriteData(HARD_MAT_BATCHNO2, 7, stConfigData.rgucConfig_HardMatBatchNo2);
  }
  /* CONFIG_PARA_83 HARD_MAT_BATCHNO3 rgucConfig_HardMatBatchNo3*/
  
  { strcpy((char *)stConfigData.rgucConfig_HardMatBatchNo3,DEFAULT_HARD_MAT_BATCHNO3);
  fnSPI_EEPROM_WriteData(HARD_MAT_BATCHNO3, 7, stConfigData.rgucConfig_HardMatBatchNo3);
  }
  /* CONFIG_PARA_84 SCU_UNIT_TYPE ucConfig_TypeofUnit*/
  
  { 
    stConfigData.ucConfig_TypeofUnit = DEFAULT_SCU_UNIT_TYPE;
    fnSPI_EEPROM_WriteData(SCU_UNIT_TYPE, 1, &stConfigData.ucConfig_TypeofUnit);
  }
  /* CONFIG_PARA_85 SCU_BYPASS ucConfig_SCUBypass*/
  
  { 
    stConfigData.ucConfig_SCUBypass = DEFAULT_SCU_BYPASS;
    fnSPI_EEPROM_WriteData(SCU_BYPASS, 1, &stConfigData.ucConfig_SCUBypass);
  }
  /* CONFIG_PARA_86 OPMM_PIPE_TEMP_THRES_LOW fConfig_PipeTempLTH*/
  
  { stConfigData.fConfig_PipeTempLTH = DEFAULT_OPMM_PIPE_TEMP_THRES_LOW;
  SplitByte.fData = stConfigData.fConfig_PipeTempLTH ; 
  fnSPI_EEPROM_WriteData(OPMM_PIPE_TEMP_THRES_LOW, 4, SplitByte.rgucData);
  }
  /* CONFIG_PARA_87 OPMM_PIPE_TEMP_THRES_HIGH fConfig_PipeTempHTH*/
  
  { 
    stConfigData.fConfig_PipeTempHTH = DEFAULT_OPMM_PIPE_TEMP_THRES_HIGH;
    SplitByte.fData = stConfigData.fConfig_PipeTempHTH ; 
    fnSPI_EEPROM_WriteData(OPMM_PIPE_TEMP_THRES_HIGH, 4, SplitByte.rgucData);
  }
  /* CONFIG_PARA_88 TEMP_PRESS_ACHIEVE_BYPASS ucConfig_Temp_Press_Achieve_Bypass*/
  
  { 
    stConfigData.ucConfig_Temp_Press_Achieve_Bypass = DEFAULT_TEMP_PRESS_ACHIEVE_BYPASS;
    fnSPI_EEPROM_WriteData(TEMP_PRESS_ACHIEVE_BYPASS, 1, &stConfigData.ucConfig_Temp_Press_Achieve_Bypass);
  }
  /* CONFIG_PARA_89 OPMM_NON_CONTACT12_CONN ucConfig_ConnNonContactat12*/
  
  {
    stConfigData.ucConfig_ConnNonContactat12 = DEFAULT_OPMM_NON_CONTACT12_CONN;
    fnSPI_EEPROM_WriteData(OPMM_NON_CONTACT12_CONN, 1, &stConfigData.ucConfig_ConnNonContactat12);
    stConfigData.ucConfig_ConnNonContactat3 = DEFAULT_OPMM_NON_CONTACT3_CONN;
    fnSPI_EEPROM_WriteData(OPMM_NON_CONTACT3_CONN, 1, &stConfigData.ucConfig_ConnNonContactat3);
    stConfigData.ucConfig_ConnNonContactat6 = DEFAULT_OPMM_NON_CONTACT6_CONN;
    fnSPI_EEPROM_WriteData(OPMM_NON_CONTACT6_CONN, 1, &stConfigData.ucConfig_ConnNonContactat6);
    stConfigData.ucConfig_ConnNonContactat9 = DEFAULT_OPMM_NON_CONTACT9_CONN;
    fnSPI_EEPROM_WriteData(OPMM_NON_CONTACT9_CONN, 1, &stConfigData.ucConfig_ConnNonContactat9);
  }
  /* CONFIG_PARA_94 OPMM_BUZZER_VOLUME ucConfig_BuzzerVolume*/
  
  
  { stConfigData.ucConfig_BuzzerVolume = DEFAULT_OPMM_BUZZER_VOLUME;
  fnSPI_EEPROM_WriteData(OPMM_BUZZER_VOLUME, 1, &stConfigData.ucConfig_BuzzerVolume);
  }
  /*****************************************change8******************************************/	
  /* CONFIG_PARA_96 SCU_TIME_SEND_STALL_COMMAND fConfig_TimeBeforeSendingStallcommand*/
  
  {  stConfigData.fConfig_TimeBeforeSendingStallcommand = DEFAULT_SCU_TIME_SEND_STALL_COMMAND;
  SplitByte.fData = stConfigData.fConfig_TimeBeforeSendingStallcommand;
  fnSPI_EEPROM_WriteData(SCU_TIME_SEND_STALL_COMMAND, 4, SplitByte.rgucData);
  }
  
  /* CONFIG_PARA_97 OPMM_STOP_HLS_TIMER fConfig_TimeStopOPMMcommand*/
  
  {  stConfigData.fConfig_TimeStopOPMMcommand = DEFAULT_OPMM_STOP_HLS_TIMER;
  SplitByte.fData = stConfigData.fConfig_TimeStopOPMMcommand;
  fnSPI_EEPROM_WriteData(OPMM_STOP_HLS_TIMER, 4, SplitByte.rgucData);
  }
  
  /* CONFIG_PARA_98 OPMM_BATT_KNEE_VOLT fConfig_BatteryKneeVoltage*/
  
  {  stConfigData.fConfig_BatteryKneeVoltage = DEFAULT_OPMM_BATT_KNEE_VOLT;
  SplitByte.fData = stConfigData.fConfig_BatteryKneeVoltage;
  fnSPI_EEPROM_WriteData(OPMM_BATT_KNEE_VOLT, 4, SplitByte.rgucData);
  }
  /**************************************change10***********************************************/
  /* CONFIG_PARA_99 VCM_STALL_UPSTROKE_UPPER_LIMIT  ucConfig_UpstrokeUpLimit*/
  {
    stConfigData.ucConfig_UpstrokeUpLimit = DEFAULT_VCM_STALL_UPSTROKE_UPPER_LIMIT;
    fnSPI_EEPROM_WriteData(VCM_STALL_UPSTROKE_UPPER_LIMIT, 1, &stConfigData.ucConfig_UpstrokeUpLimit);
  }
  /*CONFIG_PARA_100 VCM_STALL_UPSTROKE_MIN_RANGE	ucConfig_UpstrokeMinRange*/
  {
    stConfigData.ucConfig_UpstrokeMinRange = DEFAULT_VCM_STALL_UPSTROKE_MIN_RANGE;
    fnSPI_EEPROM_WriteData(VCM_STALL_UPSTROKE_MIN_RANGE, 1, &stConfigData.ucConfig_UpstrokeMinRange);
  }
  /*CONFIG_PARA_101 VCM_STALL_UPSTROKE_MAX_RANGE	ucConfig_UpstrokeMaxRange*/
  {
    stConfigData.ucConfig_UpstrokeMaxRange = DEFAULT_VCM_STALL_UPSTROKE_MAX_RANGE;
    fnSPI_EEPROM_WriteData(VCM_STALL_UPSTROKE_MAX_RANGE, 1, &stConfigData.ucConfig_UpstrokeMaxRange);
  }
  /*CONFIG_PARA_102 VCM_STALL_DOWNSTROKE_UPPER_LIMIT	ucConfig_DownstrokeUpLimit*/
  {
    stConfigData.ucConfig_DownstrokeUpLimit = DEFAULT_VCM_STALL_DOWNSTROKE_UPPER_LIMIT;
    fnSPI_EEPROM_WriteData(VCM_STALL_DOWNSTROKE_UPPER_LIMIT, 1, &stConfigData.ucConfig_DownstrokeUpLimit);
  }
  /*CONFIG_PARA_103 VCM_STALL_DOWNSTROKE_MIN_RANGE	ucConfig_DownstrokeMinRange*/
  {
    stConfigData.ucConfig_DownstrokeMinRange = DEFAULT_VCM_STALL_DOWNSTROKE_MIN_RANGE;
    fnSPI_EEPROM_WriteData(VCM_STALL_DOWNSTROKE_MIN_RANGE, 1, &stConfigData.ucConfig_DownstrokeMinRange);
  }
  /*CONFIG_PARA_104 VCM_STALL_DOWNSTROKE_MAX_RANGE	ucConfig_DownstrokeMaxRange*/
  {
    stConfigData.ucConfig_DownstrokeMaxRange = DEFAULT_VCM_STALL_DOWNSTROKE_MAX_RANGE;
    fnSPI_EEPROM_WriteData(VCM_STALL_DOWNSTROKE_MAX_RANGE, 1, &stConfigData.ucConfig_DownstrokeMaxRange);
  }
  
  
  /********************************************************************************************/
  
  /*ETHERNET PARAMETERS*/
  strcpy((char *)stEthernetIP.rgucIP_Addr,ETH_IP_ADDRESS);
  strcpy((char *)stEthernetIP.rgucSubnet_Addr,ETH_SUBNETMASK);
  strcpy((char *)stEthernetIP.rgucGateway_Addr,ETH_GATEWAY);
  stEthernetIP.uiPort=ETH_PORT;
  
  fnSPI_EEPROM_WriteData(SCU_ETH_IPADDRESS, 15, stEthernetIP.rgucIP_Addr);
  fnSPI_EEPROM_WriteData(SCU_ETH_SMADDRESS, 15, stEthernetIP.rgucSubnet_Addr);
  fnSPI_EEPROM_WriteData(SCU_ETH_GWADDRESS, 15, stEthernetIP.rgucGateway_Addr);
  SplitByte.uiData = stEthernetIP.uiPort;
  fnSPI_EEPROM_WriteData(SCU_ETH_PORTADDRESS, 2, SplitByte.rgucData);
  
  /*WIFI PARAMETERS change18*/
  //	strcpy((char *)stWifiIP.rgucIP_Addr,WIFI_IP_ADDRESS);
  //	strcpy((char *)stWifiIP.rgucSubnet_Addr,WIFI_SUBNETMASK);
  //	strcpy((char *)stWifiIP.rgucGateway_Addr,WIFI_GATEWAY);
  //	strcpy((char *)stWifiIP.rgucSSID,WIFI_SSID);
  //	strcpy((char *)stWifiIP.rgucPASWD,WIFI_PASSWORD);			
  //	stWifiIP.uiPort = WIFI_PORT;
  //	
  //	fnSPI_EEPROM_WriteData(SCU_WIFI_IPADDRESS, 15, stWifiIP.rgucIP_Addr);
  //	fnSPI_EEPROM_WriteData(SCU_WIFI_SMADDRESS, 15, stWifiIP.rgucSubnet_Addr);
  //	fnSPI_EEPROM_WriteData(SCU_WIFI_GWADDRESS, 15, stWifiIP.rgucGateway_Addr);
  //	fnSPI_EEPROM_WriteData(SCU_WIFI_SSIDADDRESS, 15,stWifiIP.rgucSSID);
  //	fnSPI_EEPROM_WriteData(SCU_WIFI_PASSADDRESS, 15,stWifiIP.rgucPASWD);
  //	SplitByte.uiData = stWifiIP.uiPort;
  //	fnSPI_EEPROM_WriteData(SCU_WIFI_PORTADDRESS, 2, SplitByte.rgucData);		
  fnWifiDefaultConfig();
  
  
  /*change21*/	
  /*CONFIG_PARA_114 IDLE_DATA_LOGGING	fConfig_IdleTimePeriodicLog*/	 
  
  {  stConfigData.fConfig_IdleTimePeriodicLog = DEFAULT_IDLE_DATA_LOGGING;
  SplitByte.fData = stConfigData.fConfig_IdleTimePeriodicLog;
  fnSPI_EEPROM_WriteData(IDLE_DATA_LOGGING, 4, SplitByte.rgucData);
  }
  /* CONFIG_PARA_111 JOB_NAME1 rgucConfig_JobName1*/
  { 
    strcpy((char *)stConfigData.rgucConfig_JobName1,DEFAULT_JOB_NAME1);
    //	*(long long *)stConfigData.rgucConfig_SupervisorName1 = *(long long *)DEFAULT_SUPERVISOR_NAME1;
    fnSPI_EEPROM_WriteData(JOB_NAME1, 7, stConfigData.rgucConfig_JobName1);
  }
  /* CONFIG_PARA_112 JOB_NAME2 rgucConfig_JobName2*/
  
  { strcpy((char *)stConfigData.rgucConfig_JobName2,DEFAULT_JOB_NAME2);
  //		*(long long *)stConfigData.rgucConfig_SupervisorName2 = *(long long *)DEFAULT_SUPERVISOR_NAME2;
  fnSPI_EEPROM_WriteData(JOB_NAME2, 7, stConfigData.rgucConfig_JobName2);
  }
  /* CONFIG_PARA_113 JOB_NAME3 rgucConfig_JobName3*/
  
  { strcpy((char *)stConfigData.rgucConfig_JobName3 ,DEFAULT_JOB_NAME3);
  fnSPI_EEPROM_WriteData(JOB_NAME3, 7,stConfigData.rgucConfig_JobName3);
  /*change23*/
  ucJobNameID = DEFAULT_CURRENT_INDEX_JOB_NAME;
  ucJobNameLog = DEFAULT_CURRENT_INDEX_JOB_NAME; /*change78*/
			fnSPI_EEPROM_WriteData(CURRENT_INDEX_JOB_NAME, 1, &ucJobNameID);
			fnSPI_EEPROM_WriteData(LOG_INDEX_JOB_NAME, 1, &ucJobNameLog); /*change78*/
  
  /*change24*/
  fnResetName(FLASH_Sector_7);
  
  }
  /*CONFIG_PARA_115 PIPE_DIAMETER	fConfig_PipeDiameter*/	 
  
  {  stConfigData.fConfig_PipeDiameter = DEFAULT_PIPE_DIAMETER;
  SplitByte.fData = stConfigData.fConfig_PipeDiameter;
  fnSPI_EEPROM_WriteData(PIPE_DIAMETER, 4, SplitByte.rgucData);
  }
  /*change37*/
  /*CONFIG_PARA_116 HMI_TIME_OFFSET_SIGN	ucConfig_TimeOffsetSign*/
  fnSPI_EEPROM_WriteData(HMI_TIME_OFFSET_SIGN, 1, DEFAULT_HMI_TIME_OFFSET_SIGN); 
  
  /*CONFIG_PARA_116 HMI_TIME_OFFSET_HOUR	ucConfig_TimeOffsetHour*/
  fnSPI_EEPROM_WriteData(HMI_TIME_OFFSET_HOUR, 1, DEFAULT_HMI_TIME_OFFSET_HOUR); 
  
  /*CONFIG_PARA_116 HMI_TIME_OFFSET_MIN	ucConfig_TimeOffsetMin*/			
  fnSPI_EEPROM_WriteData(HMI_TIME_OFFSET_MIN, 1, DEFAULT_HMI_TIME_OFFSET_MIN); 
  
  /*CONFIG_PARA_116 HMI_TIME_OFFSET_SEC	ucConfig_TimeOffsetSec*/			
  fnSPI_EEPROM_WriteData(HMI_TIME_OFFSET_SEC, 1, DEFAULT_HMI_TIME_OFFSET_SEC); 
	
	/*change42*/
	 /* CONFIG_PARA_117 JOB_ID1 rgucConfig_JobId1*/
    { 
    strcpy((char *)stConfigData.rgucConfig_JobId1,DEFAULT_JOB_ID1);
     fnSPI_EEPROM_WriteData(JOB_ID1, 7, stConfigData.rgucConfig_JobId1);
  }
  /* CONFIG_PARA_118 JOB_ID2 rgucConfig_JobId2*/
  
  { strcpy((char *)stConfigData.rgucConfig_JobId2,DEFAULT_JOB_ID2);
    fnSPI_EEPROM_WriteData(JOB_ID2, 7, stConfigData.rgucConfig_JobId2);
  }
  /* CONFIG_PARA_119 JOB_ID3 rgucConfig_JobId3*/
  
  { 
    strcpy((char *)stConfigData.rgucConfig_JobId3 ,DEFAULT_JOB_ID3);
    fnSPI_EEPROM_WriteData(JOB_ID3, 7,stConfigData.rgucConfig_JobId3);
//    /*change48*/
    ucJobID = DEFAULT_CURRENT_INDEX_JOB_ID;
    fnSPI_EEPROM_WriteData(DEFAULT_CURRENT_INDEX_JOB_ID, 1, &ucJobID);
//    /*change48*/
    fnResetName(FLASH_Sector_8);
    
  }
	/*change43*/
  /* CONFIG_PARA_120 BASE_TOTAL_MAT_USED fCurr_BaseTotalMaterialUsed*/
  
  {
    stCurrentData.fCurr_BaseTotalMaterialUsed = DEFAULT_BASE_TOTAL_MAT_USED;
    SplitByte.fData = stCurrentData.fCurr_BaseTotalMaterialUsed;
    fnSPI_EEPROM_WriteData(BASE_TOTAL_MAT_USED, 4, SplitByte.rgucData);
  }
	
	/* CONFIG_PARA_121 HARD_TOTAL_MAT_USED fCurr_HardTotalMaterialUsed*/
  
  {
    stCurrentData.fCurr_HardTotalMaterialUsed = DEFAULT_HARD_TOTAL_MAT_USED;
    SplitByte.fData = stCurrentData.fCurr_HardTotalMaterialUsed;
    fnSPI_EEPROM_WriteData(HARD_TOTAL_MAT_USED, 4, SplitByte.rgucData);
  }
	
	 /* CONFIG_PARA_122 BASE_PRIME_MAT_ACC fCurr_BaseMaterialPrimedAccumulate*/
  
  {
    stCurrentData.fCurr_BaseMaterialPrimedAccumulate = DEFAULT_BASE_PRIME_MAT_ACC;
    SplitByte.fData = stCurrentData.fCurr_BaseMaterialPrimedAccumulate;
    fnSPI_EEPROM_WriteData(BASE_PRIME_MAT_ACC, 4, SplitByte.rgucData);
  }
	
	/* CONFIG_PARA_123 HARD_PRIME_MAT_ACC fCurr_HardMaterialPrimedAccumulate*/
  
  {
    stCurrentData.fCurr_HardMaterialPrimedAccumulate = DEFAULT_HARD_PRIME_MAT_ACC;
    SplitByte.fData = stCurrentData.fCurr_HardMaterialPrimedAccumulate;
    fnSPI_EEPROM_WriteData(HARD_PRIME_MAT_ACC, 4, SplitByte.rgucData);
  }

	 /* CONFIG_PARA_124 BASE_APPLIED_MAT_ACC fCurr_BaseMaterialAppliedAccumulate*/
  
  {
    stCurrentData.fCurr_BaseMaterialAppliedAccumulate = DEFAULT_BASE_APPLIED_MAT_ACC;
    SplitByte.fData = stCurrentData.fCurr_BaseMaterialAppliedAccumulate;
    fnSPI_EEPROM_WriteData(BASE_APPLIED_MAT_ACC, 4, SplitByte.rgucData);
  }
	
	/* CONFIG_PARA_125 HARD_APPLIED_MAT_ACC fCurr_HardMaterialAppliedAccumulate*/
  
  {
    stCurrentData.fCurr_HardMaterialAppliedAccumulate = DEFAULT_HARD_APPLIED_MAT_ACC;
    SplitByte.fData = stCurrentData.fCurr_HardMaterialAppliedAccumulate;
    fnSPI_EEPROM_WriteData(HARD_APPLIED_MAT_ACC, 4, SplitByte.rgucData);
  }

	 /* CONFIG_PARA_126 BASE_TOTAL_MAT_USED_ACC fCurr_BaseTotalMaterialUsedAccumulate*/
  
  {
    stCurrentData.fCurr_BaseTotalMaterialUsedAccumulate = DEFAULT_BASE_TOTAL_MAT_USED_ACC;
    SplitByte.fData = stCurrentData.fCurr_BaseTotalMaterialUsedAccumulate;
    fnSPI_EEPROM_WriteData(BASE_TOTAL_MAT_USED_ACC, 4, SplitByte.rgucData);
  }
	
	/* CONFIG_PARA_127 HARD_TOTAL_MAT_USED_ACC fCurr_HardTotalMaterialUsedAccumulate*/
  
  {
    stCurrentData.fCurr_HardTotalMaterialUsedAccumulate = DEFAULT_HARD_TOTAL_MAT_USED_ACC;
    SplitByte.fData = stCurrentData.fCurr_HardTotalMaterialUsedAccumulate;
    fnSPI_EEPROM_WriteData(HARD_TOTAL_MAT_USED_ACC, 4, SplitByte.rgucData);
  }

	
}

/*****************************************************************************
**@Function 		 :  fnEEPROMConfig_Write
**@Descriptions :	this function write config parameters recieved from the OP
**@parameters	 :  None
**@return			 :  None
*****************************************************************************/
void fnEEPROMConfig_Write(void)
{ 
  //ucEEPROMCounter = CONFIG_PARA_98;
  if(0 != ucEEPROMCounter)
  {
#ifdef GENERAL_DBG_EE
    printf("EEPROM WRITE FROM HMI: %d\r\n",ucEEPROMCounter);					
#endif
    switch(ucEEPROMCounter)
    {
      
      /***************************VCM*********************************/
    case CONFIG_PARA_1:
      {
        SplitByte.iData = stConfigData.iConfig_UpStrokeTH;
        fnSPI_EEPROM_WriteData(VCM_UPSTROKE_ADD, 2, SplitByte.rgucData);
      }
      break;
    case CONFIG_PARA_2:
      {
        SplitByte.iData = stConfigData.iConfig_DownStrokeTH;
        fnSPI_EEPROM_WriteData(VCM_DOWNSTROKE_ADD, 2, SplitByte.rgucData);
        
      }
      break;
    case CONFIG_PARA_3:
      {
        SplitByte.fData = stConfigData.fConfig_PressureGauge1LTH;
        fnSPI_EEPROM_WriteData(VCM_PRESSURE_LOWTH1, 4, SplitByte.rgucData);
      }
      break;
    case CONFIG_PARA_4:
      {
        SplitByte.fData = stConfigData.fConfig_PressureGauge1HTH;
        fnSPI_EEPROM_WriteData(VCM_PRESSURE_HIGHTH1, 4, SplitByte.rgucData);
        
      }
      break;
    case CONFIG_PARA_5:
      {
        SplitByte.fData = stConfigData.fConfig_PressureGauge2LTH;
        fnSPI_EEPROM_WriteData(VCM_PRESSURE_LOWTH2, 4, SplitByte.rgucData);
      }
      break;
    case CONFIG_PARA_6:
      {
        SplitByte.fData = stConfigData.fConfig_PressureGauge2HTH;
        fnSPI_EEPROM_WriteData(VCM_PRESSURE_HIGHTH2, 4, SplitByte.rgucData);
      }
      break;
    case CONFIG_PARA_7:
      {
        SplitByte.fData = stConfigData.fConfig_PressureGauge3LTH ;
        fnSPI_EEPROM_WriteData(VCM_PRESSURE_LOWTH3, 4, SplitByte.rgucData);
      }
      break;
    case CONFIG_PARA_8:
      {
        SplitByte.fData = stConfigData.fConfig_PressureGauge3HTH;
        fnSPI_EEPROM_WriteData(VCM_PRESSURE_HIGHTH3, 4, SplitByte.rgucData);
        
      }
      break;
    case CONFIG_PARA_9:
      {
        SplitByte.fData = stConfigData.fConfig_AreaofCylinderA;
        fnSPI_EEPROM_WriteData(VCM_CYLINDER_AREA1, 4, SplitByte.rgucData);
      }
      break;
    case CONFIG_PARA_10:
      {
        SplitByte.fData = stConfigData.fConfig_AreaofCylinderB ;
        fnSPI_EEPROM_WriteData(VCM_CYLINDER_AREA2, 4, SplitByte.rgucData);
      }
      break;
    case CONFIG_PARA_11:
      {
        SplitByte.fData = stConfigData.fConfig_AreaofCylinderC;
        fnSPI_EEPROM_WriteData(VCM_CYLINDER_AREA3, 4, SplitByte.rgucData);
      }
      break;
    case CONFIG_PARA_12:
      {
        
        fnSPI_EEPROM_WriteData(VCM_CYLINDER_COUNT, 1, &stConfigData.ucConfig_CylinderCount);
      }
      break;
    case CONFIG_PARA_13:
      {
        
        fnSPI_EEPROM_WriteData(VCM_PRESSURE_GAUGE1_CONN, 1, &stConfigData.ucConfig_ConnPressureGuage1); 
        fnSPI_EEPROM_WriteData(VCM_PRESSURE_GAUGE2_CONN, 1, &stConfigData.ucConfig_ConnPressureGuage2); 
        fnSPI_EEPROM_WriteData(VCM_PRESSURE_GAUGE3_CONN, 1, &stConfigData.ucConfig_ConnPressureGuage3); 
      }
      break;
    case CONFIG_PARA_14:
      {
        fnSPI_EEPROM_WriteData(VCM_LINEAR_TRANSDUCER_CONN, 1, &stConfigData.ucConfig_ConnLinearTransducer); 
      }
      break;
    case CONFIG_PARA_15:
      {
        fnSPI_EEPROM_WriteData(VCM_EE07_INSIDE_CONN, 1, &stConfigData.ucConfig_ConnInsideEE07); 
        fnSPI_EEPROM_WriteData(VCM_EE07_OUTSIDE_CONN, 1, &stConfigData.ucConfig_ConnOutsideEE07); 
      }
      break;
    case CONFIG_PARA_16:
      {
//				stConfigData.ucConfig_ConnStallvalve =1;
        fnSPI_EEPROM_WriteData(VCM_STALL_VALVE_CONN, 1, &stConfigData.ucConfig_ConnStallvalve); 
				/*change53*/
//				stConfigData.ucConfig_ConnRecirculationvalve =3;
				fnSPI_EEPROM_WriteData(VCM_RECIRC_VALVE_CONN, 1, &stConfigData.ucConfig_ConnRecirculationvalve);
      }
      break;
    case CONFIG_PARA_17:
      { 
        fnSPI_EEPROM_WriteData(VCM_BYPASS, 1, &stConfigData.ucConfig_VCMBypass); 
      }
      break;
    case CONFIG_PARA_18:
      { 
        SplitByte.fData = stConfigData.fConfig_TimeBeforeStall;
        fnSPI_EEPROM_WriteData(VCM_BEFORE_STALL, 4, SplitByte.rgucData);
      }
      break;
    case CONFIG_PARA_19:
      { 
        SplitByte.fData = stConfigData.fConfig_TimeCompareStall;
        fnSPI_EEPROM_WriteData(VCM_COMPARE_STALL, 4, SplitByte.rgucData);
      }
      break;
      
      /***************************HCM*********************************/	
      
    case CONFIG_PARA_20:
      { 
        SplitByte.fData = stConfigData.fConfig_AntiFrzTempSet;
        fnSPI_EEPROM_WriteData(HCM_ANTI_SET_TEMP, 4, SplitByte.rgucData);
      }
      break;
    case CONFIG_PARA_21:	
      {
        SplitByte.fData = stConfigData.fConfig_AntiFrzTempLTH;
        fnSPI_EEPROM_WriteData(HCM_ANTI_THRESH_TEMP_LOW, 4, SplitByte.rgucData);
      }
      break;
    case CONFIG_PARA_22:
      {
        SplitByte.fData = stConfigData.fConfig_AntiFrzTempHTH;
        fnSPI_EEPROM_WriteData(HCM_ANTI_THRESH_TEMP_HIGH, 4, SplitByte.rgucData);
      }
      break;
    case CONFIG_PARA_23:
      { 
        SplitByte.fData = stConfigData.fConfig_HysAntiFrzPos;
        fnSPI_EEPROM_WriteData(HCM_ANTI_HYST_TEMP_POS, 4, SplitByte.rgucData);
      }
      break;
    case CONFIG_PARA_24:
      {
        SplitByte.fData = stConfigData.fConfig_HysAntiFrzNeg;
        fnSPI_EEPROM_WriteData(HCM_ANTI_HYST_TEMP_NEG, 4, SplitByte.rgucData);
      }
      break;
    case CONFIG_PARA_25:
      {
        SplitByte.fData = stConfigData.fConfig_InlineBaseTempSet;
        fnSPI_EEPROM_WriteData(HCM_INLINE_BASE_SET_TEMP, 4, SplitByte.rgucData);
      }
      break;
    case CONFIG_PARA_26:
      { 
        SplitByte.fData = stConfigData.fConfig_InlineBaseTempLTH;
        fnSPI_EEPROM_WriteData(HCM_INLINE_BASE_THRESH_TEMP_LOW, 4, SplitByte.rgucData);
      }
      break;
    case CONFIG_PARA_27:
      {
        SplitByte.fData = stConfigData.fConfig_InlineBaseTempHTH;
        fnSPI_EEPROM_WriteData(HCM_INLINE_BASE_THRESH_TEMP_HIGH, 4, SplitByte.rgucData);
      }
      break;
    case CONFIG_PARA_28:
      { 
        SplitByte.fData = stConfigData.fConfig_HysInlineBasePos;
        fnSPI_EEPROM_WriteData(HCM_INLINE_BASE_HYST_TEMP_POS, 4, SplitByte.rgucData);
      }
      break;
    case CONFIG_PARA_29:
      { 
        SplitByte.fData = stConfigData.fConfig_HysInlineBaseNeg;
        fnSPI_EEPROM_WriteData(HCM_INLINE_BASE_HYST_TEMP_NEG, 4, SplitByte.rgucData);
      }
      break;
    case CONFIG_PARA_30:
      { 
        SplitByte.fData = stConfigData.fConfig_InlineHardTempSet;
        fnSPI_EEPROM_WriteData(HCM_INLINE_HARD_SET_TEMP, 4, SplitByte.rgucData);
      }
      break;
    case CONFIG_PARA_31:
      {
        SplitByte.fData = stConfigData.fConfig_InlineHardTempLTH;
        fnSPI_EEPROM_WriteData(HCM_INLINE_HARD_THRESH_TEMP_LOW, 4, SplitByte.rgucData);
      }
      break;
    case CONFIG_PARA_32:
      { 
        SplitByte.fData = stConfigData.fConfig_InlineHardTempHTH;
        fnSPI_EEPROM_WriteData(HCM_INLINE_HARD_THRESH_TEMP_HIGH, 4, SplitByte.rgucData);
      }
      break;
    case CONFIG_PARA_33:
      {
        SplitByte.fData = stConfigData.fConfig_InlineHysHardPos;
        fnSPI_EEPROM_WriteData(HCM_INLINE_HARD_HYST_TEMP_POS, 4, SplitByte.rgucData);
      }
      break;
    case CONFIG_PARA_34:
      { 
        SplitByte.fData = stConfigData.fConfig_InlineHysHardNeg;
        fnSPI_EEPROM_WriteData(HCM_INLINE_HARD_HYST_TEMP_NEG, 4, SplitByte.rgucData);
      }
      break;
    case CONFIG_PARA_35:
      { 
        SplitByte.fData = stConfigData.fConfig_BaseLiquidLevelLTH;
        fnSPI_EEPROM_WriteData(HCM_BASE_LIQUID_THRESH_LEVEL_LOW, 4, SplitByte.rgucData);
      }
      break;
    case CONFIG_PARA_36:
      {
        SplitByte.fData = stConfigData.fConfig_BaseLiquidLevelHTH;
        fnSPI_EEPROM_WriteData(HCM_BASE_LIQUID_THRESH_LEVEL_HIGH, 4, SplitByte.rgucData);
      }
      break;
    case CONFIG_PARA_37:
      { 
        SplitByte.fData = stConfigData.fConfig_HardLiquidLevelLTH;
        fnSPI_EEPROM_WriteData(HCM_HARD_LIQUID_THRESH_LEVEL_LOW, 4, SplitByte.rgucData);
      }
      break;
    case CONFIG_PARA_38:
      { 
        SplitByte.fData = stConfigData.fConfig_HardLiquidLevelHTH;
        fnSPI_EEPROM_WriteData(HCM_HARD_LIQUID_THRESH_LEVEL_HIGH, 4, SplitByte.rgucData);
      }
      break;
    case CONFIG_PARA_39:
      { 
        SplitByte.fData = stConfigData.fConfig_BaseTankHeight;
        fnSPI_EEPROM_WriteData(HCM_BASE_TANK_HEIGHT, 4, SplitByte.rgucData);
      }
      break;
    case CONFIG_PARA_40:
      {
        SplitByte.fData = stConfigData.fConfig_HardTankHeight;
        fnSPI_EEPROM_WriteData(HCM_HARD_TANK_HEIGHT, 4, SplitByte.rgucData);
      }
      break;
    case CONFIG_PARA_41:
      {
        SplitByte.iData = stConfigData.uiConfig_LLSamplingRate;
        fnSPI_EEPROM_WriteData(HCM_LIQUID_LEVEL_SAMPLING_RATE, 2, SplitByte.rgucData);
      }
      break;
    case CONFIG_PARA_42:
      { 
        fnSPI_EEPROM_WriteData(HCM_INLINE_BASE_THERM_CONN, 1, &stConfigData.ucConfig_ConnBaseinlinether);
        fnSPI_EEPROM_WriteData(HCM_INLINE_HARD_THERM_CONN, 1, &stConfigData.ucConfig_ConnHardinlinether);
        fnSPI_EEPROM_WriteData(HCM_INLINE_ANTIFREEZE_THERM_CONN, 1, &stConfigData.ucConfig_ConnAntifreezether);
        fnSPI_EEPROM_WriteData(HCM_RETURN_BASE_THERM_CONN, 1, &stConfigData.ucConfig_ConnBasereturnlinether);
      }
      break;
    case CONFIG_PARA_43:
      { 
        fnSPI_EEPROM_WriteData(HCM_INLINE_BASE_HEATER_CONN, 1, &stConfigData.ucConfig_ConnBaseinlineheater);
        fnSPI_EEPROM_WriteData(HCM_INLINE_HARD_HEATER_CONN, 1, &stConfigData.ucConfig_ConnHardinlineheater);
        fnSPI_EEPROM_WriteData(HCM_INLINE_ANTIFREEZE_HEATER_CONN, 1, &stConfigData.ucConfig_ConnAntifreezeheater);
        fnSPI_EEPROM_WriteData(HCM_HARD_HEATER_CONN, 1, &stConfigData.ucConfig_ConnHardheater);
      }
      break;
    case CONFIG_PARA_44:
      { 
        
        fnSPI_EEPROM_WriteData(HCM_BASE_LIQUID_LEVEL_CONN, 1, &stConfigData.ucConfig_ConnBasetankLL);
        fnSPI_EEPROM_WriteData(HCM_HARD_LIQUID_LEVEL_CONN, 1, &stConfigData.ucConfig_ConnHardtankLL);
      }
      break;
    case CONFIG_PARA_45:
      {
        SplitByte.iData = stConfigData.uiConfig_HCMBypass;
        fnSPI_EEPROM_WriteData(HCM_BYPASS, 2, SplitByte.rgucData);
      }
      break;
      
      /***************************OPMM*********************************/	
      
      
    case CONFIG_PARA_46:
      {
        fnSPI_EEPROM_WriteData(OPMM_SPRAY_COUNT, 1, &stConfigData.ucConfig_SprayCount);
      }
      break;
      
      
    case CONFIG_PARA_47:
      {
        SplitByte.fData = stConfigData.fConfig_PrimeTime ;
        fnSPI_EEPROM_WriteData(OPMM_PRIME_TIME, 4, SplitByte.rgucData);
      }
      break;
    case CONFIG_PARA_48:
      { 
        SplitByte.fData = stConfigData.fConfig_GunDelayTime;
        fnSPI_EEPROM_WriteData(OPMM_GUN_DELAY_TIME, 4, SplitByte.rgucData);	
      }
      break;
    case CONFIG_PARA_49:
      {
        SplitByte.fData = stConfigData.fConfig_SprayOverlapTime;
        fnSPI_EEPROM_WriteData(OPMM_SPRAY_OVERLAPTIME, 4, SplitByte.rgucData);
      }
      break;
    case CONFIG_PARA_50:
      { 
        SplitByte.fData = stConfigData.fConfig_ReverseDelayTime;
        fnSPI_EEPROM_WriteData(OPMM_REVERSE_DELAYTIME, 4, SplitByte.rgucData);
      }
      break;
    case CONFIG_PARA_51:
      { 
        SplitByte.fData = stConfigData.fConfig_FlushTime;
        fnSPI_EEPROM_WriteData(OPMM_FLUSH_TIME, 4, SplitByte.rgucData);
      }
      break;
    case CONFIG_PARA_52:
      { 
        SplitByte.fData = stConfigData.fConfig_DCLSTime;
        fnSPI_EEPROM_WriteData(OPMM_DCLS_HOME_REACH_TIME, 4, SplitByte.rgucData);
      }
      break;
    case CONFIG_PARA_53:
      { 
        SplitByte.fData = stConfigData.fConfig_HOMETime;
        fnSPI_EEPROM_WriteData(OPMM_HOME_DCLS_REACH_TIME, 4, SplitByte.rgucData);
      }
      break;
    case CONFIG_PARA_54:
      {
        SplitByte.fData = stConfigData.fConfig_GunPosChangeTime;
        fnSPI_EEPROM_WriteData(OPMM_GUN_POSITION_CHANGE_TIME, 4, SplitByte.rgucData);
      }
      break;
      
    case CONFIG_PARA_55:
      { 
        SplitByte.fData = stConfigData.fConfig_TimebeforeONSprayValve;
        fnSPI_EEPROM_WriteData(OPMM_DELAY_SPRAY_VALVE_FLUSH_VBAT_TIME, 4, SplitByte.rgucData);
      }
      break;
      
    case CONFIG_PARA_56:
      { 
        SplitByte.fData = stConfigData.fConfig_FlushWarnTime;
        fnSPI_EEPROM_WriteData(OPMM_FLUSH_WARNNING_TIME, 4, SplitByte.rgucData);
      }
      break;
      
    case CONFIG_PARA_57:
      { 
        
        fnSPI_EEPROM_WriteData(OPMM_SPRAY_VALVE_CONN, 1, &stConfigData.ucConfig_ConnSprayvalve);
        fnSPI_EEPROM_WriteData(OPMM_GUNPOS_VALVE_CONN, 1, &stConfigData.ucConfig_ConnGunposition);
        fnSPI_EEPROM_WriteData(OPMM_AB_VALVE_CONN, 1, &stConfigData.ucConfig_ConnABvalve);
        fnSPI_EEPROM_WriteData(OPMM_SOLVENT_VALVE_CONN, 1, &stConfigData.ucConfig_ConnSolventvalve);
        fnSPI_EEPROM_WriteData(OPMM_FORWARD_VALVE_CONN, 1, &stConfigData.ucConfig_ConnForwardvalve);
        fnSPI_EEPROM_WriteData(OPMM_BACKWARD_VALVE_CONN, 1, &stConfigData.ucConfig_ConnBackwardvalve);
      }
      break;
    case CONFIG_PARA_58:
      { 
        SplitByte.iData = stConfigData.uiConfig_OPMMBypass;
        fnSPI_EEPROM_WriteData(OPMM_BYPASS, 2, SplitByte.rgucData);
      }
      break;
      
      
      
      /***************************SCU*********************************/	
    case CONFIG_PARA_59:
      { 
        SplitByte.fData = stConfigData.fConfig_SprayPressure;
        fnSPI_EEPROM_WriteData(SCU_SPRAY_PRESSURE_VALUE, 4, SplitByte.rgucData);
      }
      break;
    case CONFIG_PARA_60:
      { 
        SplitByte.fData = stConfigData.fConfig_SprayPressureLTH;
        fnSPI_EEPROM_WriteData(SCU_SPRAY_PRESSURE_THRESH_LOW, 4, SplitByte.rgucData);
      }
      break;
      
    case CONFIG_PARA_61:
      { 
        SplitByte.fData = stConfigData.fConfig_SprayPressureHTH;
        fnSPI_EEPROM_WriteData(SCU_SPRAY_PRESSURE_THRESH_HIGH, 4, SplitByte.rgucData);
      }
      break;
    case CONFIG_PARA_62:
      { 
        SplitByte.fData = stConfigData.fConfig_HysSprayPresPos;
        fnSPI_EEPROM_WriteData(SCU_SPRAY_PRESSURE_HYST_POS, 4, SplitByte.rgucData);
      }
      break;
    case CONFIG_PARA_63:
      { 
        SplitByte.fData = stConfigData.fConfig_HysSprayPresNeg;
        fnSPI_EEPROM_WriteData(SCU_SPRAY_PRESSURE_HYST_NEG, 4, SplitByte.rgucData);
      }
      break;
    case CONFIG_PARA_64:
      { 
        SplitByte.fData = stConfigData.fConfig_RecirPressure;
        fnSPI_EEPROM_WriteData(SCU_RECIRC_PRESSURE_VALUE, 4, SplitByte.rgucData);
      }
      break;
    case CONFIG_PARA_65:
      { 
        SplitByte.fData = stConfigData.fConfig_RecirPressureLTH;
        fnSPI_EEPROM_WriteData(SCU_RECIRC_PRESSURE_THRESH_LOW, 4, SplitByte.rgucData);
      }
      break;
    case CONFIG_PARA_66:
      { 
        SplitByte.fData = stConfigData.fConfig_RecirPressureHTH;
        fnSPI_EEPROM_WriteData(SCU_RECIRC_PRESSURE_THRESH_HIGH, 4, SplitByte.rgucData);
      }
      break;
    case CONFIG_PARA_67:
      { 
        SplitByte.fData = stConfigData.fConfig_HysRecircPresPos;
        fnSPI_EEPROM_WriteData(SCU_RECIRC_PRESSURE_HYST_POS, 4, SplitByte.rgucData);
      }
      break;
    case CONFIG_PARA_68:
      { 
        SplitByte.fData = stConfigData.fConfig_HysRecircPresNeg;
        fnSPI_EEPROM_WriteData(SCU_RECIRC_PRESSURE_HYST_NEG, 4, SplitByte.rgucData);
      }
      break;
    case CONFIG_PARA_69:
      { 
        SplitByte.fData = stConfigData.fConfig_JointNum;
        fnSPI_EEPROM_WriteData(COUNT_JOINT, 4, SplitByte.rgucData);
      }
      break;
    case CONFIG_PARA_70:
      { 
        SplitByte.fData = stConfigData.fConfig_DataLoggingPeriod;
        fnSPI_EEPROM_WriteData(TIME_DATA_LOGGING, 4, SplitByte.rgucData);
      }
      break;
      
    case CONFIG_PARA_71:
      { 
        
        fnSPI_EEPROM_WriteData(SYS_TIME, 7, stConfigData.ucConfig_SystemTime);
      }
      break;
    case CONFIG_PARA_72:
      { 
        
        fnSPI_EEPROM_WriteData(SUPERVISOR_NAME1, 7, stConfigData.rgucConfig_SupervisorName1);
        /*change23*/
   //     fnSPI_EEPROM_WriteData(CURRENT_INDEX_SUPERVISOR_NAME, 1, &ucSupervisorNameID);
        
      }
      break;
    case CONFIG_PARA_73:
      { 
        
        fnSPI_EEPROM_WriteData(SUPERVISOR_NAME2, 7, stConfigData.rgucConfig_SupervisorName2);
        
      }
      break;
    case CONFIG_PARA_74:
      { 
        fnSPI_EEPROM_WriteData(SUPERVISOR_NAME3, 7,stConfigData.rgucConfig_SupervisorName3);
        
      }
      break;
    case CONFIG_PARA_75:
      { 
        
        fnSPI_EEPROM_WriteData(COATING_MAT_NAME1, 7, stConfigData.rgucConfig_CoatingMatName1);
      }
      break;
    case CONFIG_PARA_76:
      { 
        
        fnSPI_EEPROM_WriteData(COATING_MAT_NAME2, 7, stConfigData.rgucConfig_CoatingMatName2);
      }
      break;
    case CONFIG_PARA_77:
      { 
        
        fnSPI_EEPROM_WriteData(COATING_MAT_NAME3, 7, stConfigData.rgucConfig_CoatingMatName3);
      }
      break;
    case CONFIG_PARA_78:
      { 
        
        fnSPI_EEPROM_WriteData(BASE_MAT_BATCHNO1, 7, stConfigData.rgucConfig_BaseMatBatchNo1);
      }
      break;
    case CONFIG_PARA_79:
      { 
        
        fnSPI_EEPROM_WriteData(BASE_MAT_BATCHNO2, 7, stConfigData.rgucConfig_BaseMatBatchNo2);
      }
      break;
    case CONFIG_PARA_80:
      { 
        
        fnSPI_EEPROM_WriteData(BASE_MAT_BATCHNO3, 7, stConfigData.rgucConfig_BaseMatBatchNo3);
      }
      break;
    case CONFIG_PARA_81:
      { 
        
        fnSPI_EEPROM_WriteData(HARD_MAT_BATCHNO1, 7, stConfigData.rgucConfig_HardMatBatchNo1);
      }
      break;
    case CONFIG_PARA_82:
      { 
        
        fnSPI_EEPROM_WriteData(HARD_MAT_BATCHNO2, 7, stConfigData.rgucConfig_HardMatBatchNo2);
      }
      break;
    case CONFIG_PARA_83:
      { 
        
        fnSPI_EEPROM_WriteData(HARD_MAT_BATCHNO3, 7, stConfigData.rgucConfig_HardMatBatchNo3);
      }
      break;
    case CONFIG_PARA_84:
      { 
        
        fnSPI_EEPROM_WriteData(SCU_UNIT_TYPE, 1, &stConfigData.ucConfig_TypeofUnit);
      }
      break;
    case CONFIG_PARA_85:
      { 
        
        fnSPI_EEPROM_WriteData(SCU_BYPASS, 1, &stConfigData.ucConfig_SCUBypass);
      }
      break;
    case CONFIG_PARA_86:
      { 
        SplitByte.fData = stConfigData.fConfig_PipeTempLTH ; 
        fnSPI_EEPROM_WriteData(OPMM_PIPE_TEMP_THRES_LOW, 4, SplitByte.rgucData);
      }
      break;
    case CONFIG_PARA_87:
      { 
        
        SplitByte.fData = stConfigData.fConfig_PipeTempHTH ; 
        fnSPI_EEPROM_WriteData(OPMM_PIPE_TEMP_THRES_HIGH, 4, SplitByte.rgucData);
      }
      break;
    case CONFIG_PARA_88:
      { 
        
        fnSPI_EEPROM_WriteData(TEMP_PRESS_ACHIEVE_BYPASS, 1, &stConfigData.ucConfig_Temp_Press_Achieve_Bypass);
      }
      break;
    case CONFIG_PARA_89:
      {
        fnSPI_EEPROM_WriteData(OPMM_NON_CONTACT12_CONN, 1, &stConfigData.ucConfig_ConnNonContactat12);
        fnSPI_EEPROM_WriteData(OPMM_NON_CONTACT3_CONN, 1, &stConfigData.ucConfig_ConnNonContactat3);
        fnSPI_EEPROM_WriteData(OPMM_NON_CONTACT6_CONN, 1, &stConfigData.ucConfig_ConnNonContactat6);
        fnSPI_EEPROM_WriteData(OPMM_NON_CONTACT9_CONN, 1, &stConfigData.ucConfig_ConnNonContactat9);
      }
      break;
      /*ETHERNET PARAMETERS*/
    case CONFIG_PARA_90:
      { 
        
        fnSPI_EEPROM_WriteData(SCU_ETH_IPADDRESS, 15, stEthernetIP.rgucIP_Addr);
      }
      break;
    case CONFIG_PARA_91:
      { 
        
        fnSPI_EEPROM_WriteData(SCU_ETH_SMADDRESS, 15, stEthernetIP.rgucSubnet_Addr);
      }
      break;
    case CONFIG_PARA_92:
      { 
        
        fnSPI_EEPROM_WriteData(SCU_ETH_GWADDRESS, 15, stEthernetIP.rgucGateway_Addr);
      }
      break;
    case CONFIG_PARA_93:
      { 
        SplitByte.uiData = stEthernetIP.uiPort;
        fnSPI_EEPROM_WriteData(SCU_ETH_PORTADDRESS, 2, SplitByte.rgucData);
        
      }
      break;
    case CONFIG_PARA_94:
      { 
        fnSPI_EEPROM_WriteData(OPMM_BUZZER_VOLUME, 1, &stConfigData.ucConfig_BuzzerVolume);
      }
      break;
    case CONFIG_PARA_95:
      {
        SplitByte.uiData = stConfigData.uiConfig_Signature;
        fnSPI_EEPROM_WriteData(SIGNATURE_START, SIGNATUREVALUE_SIZE, SplitByte.rgucData);
      }
      break;
      
      /*****************************************change8******************************************/
    case CONFIG_PARA_96: 
      { 
        SplitByte.fData = stConfigData.fConfig_TimeBeforeSendingStallcommand;
        fnSPI_EEPROM_WriteData(SCU_TIME_SEND_STALL_COMMAND, 4, SplitByte.rgucData);
      }
      break;
    case CONFIG_PARA_97:			
      {  
        SplitByte.fData = stConfigData.fConfig_TimeStopOPMMcommand;
        fnSPI_EEPROM_WriteData(OPMM_STOP_HLS_TIMER, 4, SplitByte.rgucData);
      }
      break;
    case CONFIG_PARA_98:
      { 
        SplitByte.fData = stConfigData.fConfig_BatteryKneeVoltage;
        fnSPI_EEPROM_WriteData(OPMM_BATT_KNEE_VOLT, 4, SplitByte.rgucData);
      }
      break;
      /**************************************change10***********************************************/
    case CONFIG_PARA_99: 
      {
        fnSPI_EEPROM_WriteData(VCM_STALL_UPSTROKE_UPPER_LIMIT, 1, &stConfigData.ucConfig_UpstrokeUpLimit);
      }
      break;
    case CONFIG_PARA_100: 
      {
        fnSPI_EEPROM_WriteData(VCM_STALL_UPSTROKE_MIN_RANGE, 1, &stConfigData.ucConfig_UpstrokeMinRange);
      }
      break;
    case CONFIG_PARA_101:
      {
        
        fnSPI_EEPROM_WriteData(VCM_STALL_UPSTROKE_MAX_RANGE, 1, &stConfigData.ucConfig_UpstrokeMaxRange);
      }
      break;
    case CONFIG_PARA_102: 
      {
        
        fnSPI_EEPROM_WriteData(VCM_STALL_DOWNSTROKE_UPPER_LIMIT, 1, &stConfigData.ucConfig_DownstrokeUpLimit);
      }
      break;
    case CONFIG_PARA_103:
      {
        fnSPI_EEPROM_WriteData(VCM_STALL_DOWNSTROKE_MIN_RANGE, 1, &stConfigData.ucConfig_DownstrokeMinRange);
      }
      break;
    case CONFIG_PARA_104:
      {
        
        fnSPI_EEPROM_WriteData(VCM_STALL_DOWNSTROKE_MAX_RANGE, 1, &stConfigData.ucConfig_DownstrokeMaxRange);
      }
      break;
      /*WIFI PARAMETERS change18*/
    case CONFIG_PARA_105:
      { 
        
        fnSPI_EEPROM_WriteData(SCU_WIFI_IPADDRESS, 15, stWifiIP.rgucIP_Addr);
      }
      break;
    case CONFIG_PARA_106:
      { 
        
        fnSPI_EEPROM_WriteData(SCU_WIFI_SMADDRESS, 15, stWifiIP.rgucSubnet_Addr);
      }
      break;
    case CONFIG_PARA_107:
      { 
        
        fnSPI_EEPROM_WriteData(SCU_WIFI_GWADDRESS, 15, stWifiIP.rgucGateway_Addr);
      }
      break;
    case CONFIG_PARA_108:
      { 
        SplitByte.uiData = stWifiIP.uiPort;
        fnSPI_EEPROM_WriteData(SCU_WIFI_PORTADDRESS, 2, SplitByte.rgucData);
        
      }
      break;
    case CONFIG_PARA_109:
      { 
        fnSPI_EEPROM_WriteData(SCU_WIFI_SSIDADDRESS, 15,stWifiIP.rgucSSID);
        
      }
      break;
    case CONFIG_PARA_110:
      { 
        fnSPI_EEPROM_WriteData(SCU_WIFI_PASSADDRESS, 15,stWifiIP.rgucPASWD);
        
      }
      break;
      
      /*change21*/	
      
    case CONFIG_PARA_111:
      { 
        fnSPI_EEPROM_WriteData(JOB_NAME1, 7, stConfigData.rgucConfig_JobName1);
        /*change23*/
     //   fnSPI_EEPROM_WriteData(CURRENT_INDEX_JOB_NAME, 1, &ucJobNameID); /*change78*/
      }
      break;
    case CONFIG_PARA_112:
      { 
        fnSPI_EEPROM_WriteData(JOB_NAME2, 7, stConfigData.rgucConfig_JobName2);
      }
      break;
    case CONFIG_PARA_113:			
      { 
        fnSPI_EEPROM_WriteData(JOB_NAME3, 7,stConfigData.rgucConfig_JobName3);
        
      }
      break;
    case CONFIG_PARA_114:		
      { 
        SplitByte.fData = stConfigData.fConfig_IdleTimePeriodicLog;
        fnSPI_EEPROM_WriteData(IDLE_DATA_LOGGING, 4, SplitByte.rgucData);
      }
      break;
    case CONFIG_PARA_115: 
      { 
        SplitByte.fData = stConfigData.fConfig_PipeDiameter;
        fnSPI_EEPROM_WriteData(PIPE_DIAMETER, 4, SplitByte.rgucData);
      }
      break;
      /*change37*/
    case CONFIG_PARA_116: 
      { 
        fnSPI_EEPROM_WriteData(HMI_TIME_OFFSET_SIGN, 1, &stConfigData.ucConfig_TimeOffsetSign); 
        fnSPI_EEPROM_WriteData(HMI_TIME_OFFSET_HOUR, 1, &stConfigData.ucConfig_TimeOffsetHour); 
        fnSPI_EEPROM_WriteData(HMI_TIME_OFFSET_MIN, 1, &stConfigData.ucConfig_TimeOffsetMin); 
        fnSPI_EEPROM_WriteData(HMI_TIME_OFFSET_SEC, 1, &stConfigData.ucConfig_TimeOffsetSec); 
      }
      break;
			/*change42*/
		 case CONFIG_PARA_117:
      { 
        
        fnSPI_EEPROM_WriteData(JOB_ID1, 7, stConfigData.rgucConfig_JobId1);
//        /*change48*/
 //       fnSPI_EEPROM_WriteData(CURRENT_INDEX_JOB_ID, 1, &ucJobID); /*change78*/
//        
      }
      break;
    case CONFIG_PARA_118:
      { 
        
        fnSPI_EEPROM_WriteData(JOB_ID2, 7, stConfigData.rgucConfig_JobId2);
        
      }
      break;
    case CONFIG_PARA_119:
      { 
        fnSPI_EEPROM_WriteData(JOB_ID3, 7,stConfigData.rgucConfig_JobId3);
        
      }
      break;
			
			/*change43*/
		case CONFIG_PARA_120:  
			{/*BASE_TOTAL_MAT_USED*/
				SplitByte.fData = stCurrentData.fCurr_BaseTotalMaterialUsed;
				fnSPI_EEPROM_WriteData(BASE_TOTAL_MAT_USED, 4, SplitByte.rgucData);
				/*HARD_TOTAL_MAT_USED*/		
				SplitByte.fData = stCurrentData.fCurr_HardTotalMaterialUsed;
				fnSPI_EEPROM_WriteData(HARD_TOTAL_MAT_USED, 4, SplitByte.rgucData);
				/*BASE_PRIME_MAT_ACC*/
			 	SplitByte.fData = stCurrentData.fCurr_BaseMaterialPrimedAccumulate;
				fnSPI_EEPROM_WriteData(BASE_PRIME_MAT_ACC, 4, SplitByte.rgucData);
				/*HARD_PRIME_MAT_ACC*/
				SplitByte.fData = stCurrentData.fCurr_HardMaterialPrimedAccumulate;
				fnSPI_EEPROM_WriteData(HARD_PRIME_MAT_ACC, 4, SplitByte.rgucData);
				/*BASE_APPLIED_MAT_ACC*/
				SplitByte.fData = stCurrentData.fCurr_BaseMaterialAppliedAccumulate;
				fnSPI_EEPROM_WriteData(BASE_APPLIED_MAT_ACC, 4, SplitByte.rgucData);
				/*HARD_APPLIED_MAT_ACC*/
				SplitByte.fData = stCurrentData.fCurr_HardMaterialAppliedAccumulate;
				fnSPI_EEPROM_WriteData(HARD_APPLIED_MAT_ACC, 4, SplitByte.rgucData);
				/*BASE_TOTAL_MAT_USED_ACC*/
				SplitByte.fData = stCurrentData.fCurr_BaseTotalMaterialUsedAccumulate;
				fnSPI_EEPROM_WriteData(BASE_TOTAL_MAT_USED_ACC, 4, SplitByte.rgucData);
				/*HARD_TOTAL_MAT_USED_ACC*/
				SplitByte.fData = stCurrentData.fCurr_HardTotalMaterialUsedAccumulate;
				fnSPI_EEPROM_WriteData(HARD_TOTAL_MAT_USED_ACC, 4, SplitByte.rgucData);
				}
			break;
    default:
      {
        
      }
    }/*end of switch*/	
    ucEEPROMCounter = 0;	
  }/*end of if*/
}/*end of EEPROM write Function*/




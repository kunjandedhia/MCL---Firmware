/******************************************************************************
**@file     :   Data_Logging.c
*
**@brief    :   This file contains all the functions realated to Data logging
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
uint32_t uliBaseAddr = 0;  /*Local variable for BASE_REGISTER in DIAGNOSTIC*/
uint32_t uliStatusBaseAddr = 0; /*Local variable for BASE_REGISTER in STATUS */
uint32_t uliFaultBaseAddr = 0;  /*Local variable for BASE_REGISTER in FAULT*/
uint32_t uliPriodicBaseAddr = 0;  /*Local variable for BASE_REGISTER in FAULT*/
uint16_t uiPeriodicDataLogCnt = 0; /*Counter for periodic log*/
uint16_t uiJobnameCnt = 0; /*Counter for Jobname change78*/
uint16_t uiJobIDCnt = 0; /*Counter for Jobname change78*/
uint16_t uiSupervisornameCnt = 0; /*Counter for Jobname change78*/
//uint32_t ulitestPRDBase_Local = 0x7C03;
/* DATA_LOGGING*/
uint8_t ucStatusByte; 			/*Status Log Byte contains bitwise logs*/
//uint8_t ucFaultByte0;			  /*FAULT Log Byte Zero contains bitwise logs*/
//uint8_t ucFaultByte1; 			/*FAULT Log Byte One contains bitwise logs*/
//uint8_t ucFaultByte2; 			/*FAULT Log Byte Two contains bitwise logs*/
//uint8_t ucFaultByte3; 			/*FAULT Log Byte Three contains bitwise logs*/
uint8_t rgucStatusLog[DL_STA_PACKET_SIZE];  /*Global Array For DATA LOGGING of STATUS LOG*/
uint8_t rgucPeriodicLog[DL_PRD_PACKET_SIZE];/*Global Array For DATA LOGGING of PERIODIC LOG*/
TypeDef_Diaglog Diag_Log;
TypeDef_Faultlog Fault_Log;
//DATALOG_t stDatalog;
uint8_t ucSupervisorNameID;/*change23*/
uint8_t ucJobNameID;/*change23*/
/*change48*/
uint8_t ucJobID;
/*change78*/
uint8_t ucJobNameLog;
uint8_t ucJobLog;
uint8_t ucSupervisorNameLog;

/* ----------------------------------------------------------------------------
*                               STRUCTURES
* ----------------------------------------------------------------------------
*/	
/*change27*/
TypeDef_WarnlogCommand stWarnLog;
/*Structure Variable for Log Warning Counter*/
TypeDef_WarnLog stWarningLog;
DATALOG_TX_t stWifiLog;
DATALOG_TX_t stEthernetLog;
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
//void fnUserDetail_Disp(void);
//void fnStatusLog_Detail(void);
//void fnFaultLog_Detail(void);
//void fnPRDLog_Detail(void);
/*****************************************************************************
**@Function 			: 	fnExp3Data_Write
**@Descriptions	:   write data to IO Expander3 over SPI bus
**@Parameters		: 	ucAddress:  address of Expander location
rgucData:  8 bit data to write in Expander
**@Return				:   None  
*****************************************************************************/
void fnExp3Data_Write(uint8_t ucAddress, uint8_t rgucData)
{		
  int16_t iCount;
  GPIO_ResetBits(IOEXP3_CS_PORT, IOEXP3_CS_PIN); /* CS asserted low*/
  
  /* select expander 3 to write*/
  while(!SPI_I2S_GetFlagStatus(SPIX_PERIPH, SPI_I2S_FLAG_TXE));
  SPI_I2S_SendData(SPIX_PERIPH, SPI_EXP3_WRITE);
  /* select DL_IODIRA of expander 3*/
  while(!SPI_I2S_GetFlagStatus(SPIX_PERIPH, SPI_I2S_FLAG_TXE));
  SPI_I2S_SendData(SPIX_PERIPH, ucAddress);
  
  /* write data to DL_IODIRA of expander 3*/
  while(!SPI_I2S_GetFlagStatus(SPIX_PERIPH, SPI_I2S_FLAG_TXE)); 
  SPI_I2S_SendData(SPIX_PERIPH, rgucData);
  
  for (iCount = 0; iCount < 50; iCount++); /* delay to check the output*/
  
  GPIO_SetBits(IOEXP3_CS_PORT, IOEXP3_CS_PIN); /* CS deasserted High*/
}

/*****************************************************************************
**@Function 			: fnExp4Data_Write
**@Descriptions	: write data to IO Expander4 over SPI bus
**@Parameters		: ucAddress:  address of Expander location
rgucData:  8 bit data to write in Expander    
**@Return				: None 
*****************************************************************************/
void fnExp4Data_Write(uint8_t ucAddress, uint8_t rgucData)
{		
  int16_t iCount;
  GPIO_ResetBits(IOEXP4_CS_PORT, IOEXP4_CS_PIN); /* CS asserted low */
  
  /* select expander 4 to write*/
  while(!SPI_I2S_GetFlagStatus(SPIX_PERIPH, SPI_I2S_FLAG_TXE)); 
  SPI_I2S_SendData(SPIX_PERIPH, SPI_EXP4_WRITE);
  
  /* select DL_IODIRA of expander 4*/
  while(!SPI_I2S_GetFlagStatus(SPIX_PERIPH, SPI_I2S_FLAG_TXE)); 
  SPI_I2S_SendData(SPIX_PERIPH, ucAddress);
  
  /* write data to DL_IODIRA of expander 4*/
  while(!SPI_I2S_GetFlagStatus(SPIX_PERIPH, SPI_I2S_FLAG_TXE));
  SPI_I2S_SendData(SPIX_PERIPH, rgucData);
  
  for (iCount = 0; iCount < 50; iCount++); /* delay to check the output*/
  
  GPIO_SetBits(IOEXP4_CS_PORT, IOEXP4_CS_PIN); /* CS deasserted High*/
}


/*****************************************************************************
**@Function 			:  fnExp3Data_Read
**@Descriptions	:  Read data from IO Expander3 over SPI bus
**@Parameters		:  ucAddress:  address of Expander location  
**@Return				:  rgucData:  8 bit data read from Expander      
*****************************************************************************/
uint16_t fnExp3Data_Read(uint8_t ucAddress)
{
  int16_t iCount;
  GPIO_ResetBits(IOEXP3_CS_PORT, IOEXP3_CS_PIN);
  
  /* select expander 3 to read*/
  while(!SPI_I2S_GetFlagStatus(SPIX_PERIPH, SPI_I2S_FLAG_TXE)); 
  SPI_I2S_SendData(SPIX_PERIPH, SPI_EXP3_READ);
  
  /* select DL_IODIRA of expander 3 */
  while(!SPI_I2S_GetFlagStatus(SPIX_PERIPH, SPI_I2S_FLAG_TXE)); 
  SPI_I2S_SendData(SPIX_PERIPH, ucAddress);
  
  for (iCount = 0; iCount < 50; iCount++); /* delay to check the output*/
  
  /* Receieve data from DL_IODIRA  */
  while(!SPI_I2S_GetFlagStatus(SPIX_PERIPH, SPI_I2S_FLAG_RXNE)); 
  SPI_I2S_ReceiveData(SPIX_PERIPH);
  
  while(!SPI_I2S_GetFlagStatus(SPIX_PERIPH, SPI_I2S_FLAG_TXE)); 
  SPI_I2S_SendData(SPIX_PERIPH, DUMMY); /* Dummy byte to generate clock */
  
  for (iCount = 0; iCount < 50; iCount++); /*delay to check the output*/
  
  GPIO_SetBits(IOEXP3_CS_PORT, IOEXP3_CS_PIN);
  
  /* Receieve data from DL_IODIRA   */
  while(!SPI_I2S_GetFlagStatus(SPIX_PERIPH, SPI_I2S_FLAG_RXNE)); 
  return(SPI_I2S_ReceiveData(SPIX_PERIPH));
}

/*****************************************************************************
**@Function 			:  fnExp4Data_Read
**@Descriptions	:  Read data from IO Expander4 over SPI bus
**@Parameters		:  ucAddress:  address of Expander location  
**@Return				:  rgucData:  8 bit data read from Expander      
*****************************************************************************/
uint16_t fnExp4Data_Read (uint8_t ucAddress)
{
  int16_t iCount;
  GPIO_ResetBits(IOEXP4_CS_PORT, IOEXP4_CS_PIN);
  
  /* select expander 4 to read*/
  while(!SPI_I2S_GetFlagStatus(SPIX_PERIPH, SPI_I2S_FLAG_TXE)); 
  SPI_I2S_SendData(SPIX_PERIPH, SPI_EXP4_READ);
  
  /* select DL_IODIRA of expander 4 */
  while(!SPI_I2S_GetFlagStatus(SPIX_PERIPH, SPI_I2S_FLAG_TXE)); 
  SPI_I2S_SendData(SPIX_PERIPH, ucAddress);
  
  for (iCount = 0; iCount < 500; iCount++); /* delay to check the output*/
  /* Receieve data from DL_IODIRA  */
  while(!SPI_I2S_GetFlagStatus(SPIX_PERIPH, SPI_I2S_FLAG_RXNE));
  SPI_I2S_ReceiveData(SPIX_PERIPH);
  
  while(!SPI_I2S_GetFlagStatus(SPIX_PERIPH, SPI_I2S_FLAG_TXE)); 
  SPI_I2S_SendData(SPIX_PERIPH, DUMMY); /*Dummy byte to generate clock*/
  
  for (iCount = 0; iCount < 500; iCount++); /*delay to check the output*/
  
  GPIO_SetBits(IOEXP4_CS_PORT, IOEXP4_CS_PIN);
  
  /* Receieve data from DL_IODIRA  */
  while(!SPI_I2S_GetFlagStatus(SPIX_PERIPH, SPI_I2S_FLAG_RXNE)); 
  return(SPI_I2S_ReceiveData(SPIX_PERIPH));
}

/*****************************************************************************
**@Function 			: fnNVSRAM1Word_Write
**@Descriptions	: This function assumes that the address is 19 bit and 
the access is a WORD Address  
**@Parameters		: uliAddr: 32 bit word location address 
uiData: 16 bit data to write  
**@Return				: None      
*******************************************************************************/
void fnNVSRAM1Word_Write(uint32_t uliAddr, uint16_t uiData )
{ 
  /*Configuring IO Expander 3 ports To Output as it is connected to address bus*/
  fnExp3Data_Write ( DL_IOCONA , 0x28); /* DL_IOCONA*/
  fnExp3Data_Write ( DL_IODIRA, 0x00);  /* set port A output */
  fnExp3Data_Write ( DL_IODIRB, 0x00);  /* set port B output	*/
  
  /*Configuring IO Expander 4 ports To Output as WRITE Mode	*/
  fnExp4Data_Write ( DL_IOCONA , 0x28); /* DL_IOCONA */
  fnExp4Data_Write ( DL_IODIRA, 0x00);	 /* set port A output*/
  fnExp4Data_Write ( DL_IODIRB, 0x00);	 /* set port B output */
  
  
  /* Loading the address on Address Lines*/
  uint8_t ucLower = uliAddr & 0xff;
  fnExp3Data_Write ( DL_OLATA, ucLower ); /* Write lower byte address */
  //	printf("ucLower= 0x%x\n\r",ucLower);
  uint8_t ucUpper = (uliAddr >> 8)  & 0xff;
  fnExp3Data_Write ( DL_OLATB, ucUpper ); /* Write higher byte address */
  //	printf("ucUpper= 0x%x\n\r",ucUpper);
  
  if ( uliAddr & ADD16 )  /* Set or Reset A16 pin*/
  {
    GPIO_SetBits(ADD16_PORT, ADD16_PIN);
  }
  else		
  {
    GPIO_ResetBits(ADD16_PORT, ADD16_PIN);
  }
  
  if ( uliAddr & ADD17 )  /* Set or Reset A17 pin*/
  {
    GPIO_SetBits(ADD17_PORT, ADD17_PIN);
  }
  else		
  {
    GPIO_ResetBits(ADD17_PORT, ADD17_PIN);
  }
  
  if ( uliAddr & ADD18 )  /* Set or Reset A18 pin*/
  {
    GPIO_SetBits(ADD18_PORT, ADD18_PIN);
  }
  else		
  {
    GPIO_ResetBits(ADD18_PORT, ADD18_PIN);
  }
  if ( uliAddr & ADD19 )  /* Set or Reset A18 pin*/
  {
    GPIO_SetBits(ADD19_PORT, ADD19_PIN);
  }
  else		
  {
    GPIO_ResetBits(ADD19_PORT, ADD19_PIN);
  }
  /*Configuring RAM1 and RAM2 Disabled*/
  GPIO_SetBits(NVSRAM2_EN_PORT, NVSRAM2_EN_PIN); /* CE2 is SET*/
  GPIO_SetBits(WE_COMMAN_PORT, WE_COMMAN_PIN);  /* WRITE(W) is SET*/
  
  /*Reset Chip Enable*/
  GPIO_ResetBits(NVSRAM1_EN_PORT, NVSRAM1_EN_PIN); /* CE1 is RESET*/
  
  GPIO_ResetBits(WE_COMMAN_PORT, WE_COMMAN_PIN); /* WRITE(W) is RESET*/
  
  /*RESET all the control pins*/
  GPIO_ResetBits(BLE_PORT, BLE_PIN); /* Byte Low Enable (BLE) is SET	*/
  GPIO_SetBits(BHE_PORT, BHE_PIN); /* Byte High Enable (BHE) is SET	*/
  
  
  /*Read the DATA from Data Lines*/
  uint8_t ucLowerdata = uiData & 0xff;
  fnExp4Data_Write ( DL_OLATA, ucLowerdata ); /*Write lower byte data*/
  
  
  /*RESET all the control pins*/
  GPIO_SetBits(BLE_PORT, BLE_PIN); /* Byte Low Enable (BLE) is SET	*/
  GPIO_ResetBits(BHE_PORT, BHE_PIN); /* Byte High Enable (BHE) is SET	*/
  uint8_t ucUpperdata = (uiData >> 8)  & 0xff;
  fnExp4Data_Write ( DL_OLATB, ucUpperdata ); /* Write higher byte data*/
  //printf("ucLowerdata write= 0x%x\n\r",ucLowerdata);
  //printf("ucUpperdata write= 0x%x\n\r",ucUpperdata);
  
  /* Set all control pins */
  GPIO_SetBits(WE_COMMAN_PORT, WE_COMMAN_PIN); /* WRITE(W) is SET */
  GPIO_SetBits(BLE_PORT, BLE_PIN); /* Byte Low Enable (BLE) is SET*/
  GPIO_SetBits(BHE_PORT, BHE_PIN); /* Byte High Enable (BHE) is SET*/
  GPIO_SetBits(NVSRAM1_EN_PORT, NVSRAM1_EN_PIN); /* CE1 is SET*/
}


/****************************************************************************
**@Function 		: fnNVSRAM2Word_Write
**@Descriptions: This function assumes that the address is 19 bit
and the access is a WORD Address  
**@Parameters	: uliAddr: 32 bit word location address 
uiData: 16 bit data to write   
**@Return			: None     
******************************************************************************/
void fnNVSRAM2Word_Write(uint32_t uliAddr, uint16_t uiData )
{ 
  /*Configuring IO Expander 3 ports To Output as it is connected to address bus*/
  fnExp3Data_Write ( DL_IOCONA , 0x28); /* DL_IOCONA */
  fnExp3Data_Write ( DL_IODIRA, 0x00);  /* set port A output */
  fnExp3Data_Write ( DL_IODIRB, 0x00);  /* set port B output	*/
  
  /*Configuring IO Expander 4 ports To Output as WRITE Mode	*/
  fnExp4Data_Write ( DL_IOCONA , 0x28);  /* DL_IOCONA*/
  fnExp4Data_Write ( DL_IODIRA, 0x00);   /* set port A output*/
  fnExp4Data_Write ( DL_IODIRB, 0x00); 	/* set port B output*/
  
  /*Loading the address on Address Lines*/
  uint8_t ucLower = uliAddr & 0xff;
  fnExp3Data_Write ( DL_OLATA, ucLower ); /*Write lower byte address*/
  uint8_t ucUpper = (uliAddr >> 8)  & 0xff;
  fnExp3Data_Write ( DL_OLATB, ucUpper ); /*Write higher byte address*/
  //printf("ucLower write= 0x%x\n\r",ucLower);
  //printf("ucUpper write= 0x%x\n\r",ucUpper);
  if ( uliAddr & ADD16 )  /* Set or Reset A16 pin*/
  {
    GPIO_SetBits(ADD16_PORT, ADD16_PIN);
  }
  else		
  {
    GPIO_ResetBits(ADD16_PORT, ADD16_PIN);
  }
  
  if ( uliAddr & ADD17 )  /* Set or Reset A17 pin*/
  {
    GPIO_SetBits(ADD17_PORT, ADD17_PIN);
  }
  else		
  {
    GPIO_ResetBits(ADD17_PORT, ADD17_PIN);
  }
  
  if ( uliAddr & ADD18 )  /* Set or Reset A18 pin*/
  {
    GPIO_SetBits(ADD18_PORT, ADD18_PIN);
  }
  else		
  {
    GPIO_ResetBits(ADD18_PORT, ADD18_PIN);
  }
  if ( uliAddr & ADD19 )  /* Set or Reset A18 pin*/
  {
    GPIO_SetBits(ADD19_PORT, ADD19_PIN);
  }
  else		
  {
    GPIO_ResetBits(ADD19_PORT, ADD19_PIN);
  }
  /*Configuring RAM1 and RAM2 Disabled*/
  GPIO_SetBits(NVSRAM2_EN_PORT, NVSRAM2_EN_PIN);
  GPIO_SetBits(WE_COMMAN_PORT, WE_COMMAN_PIN); /* WRITE(W) is SET*/
  
  /*Reset Chip Enable*/
  GPIO_ResetBits(NVSRAM2_EN_PORT, NVSRAM2_EN_PIN); /* CE1 is RESET*/
  
  GPIO_ResetBits(WE_COMMAN_PORT, WE_COMMAN_PIN); /* WRITE(W) is RESET */
  
  /*RESET all the control pins*/
  GPIO_ResetBits(BLE_PORT, BLE_PIN); /* Byte Low Enable (BLE) is SET	*/
  GPIO_SetBits(BHE_PORT, BHE_PIN); /* Byte High Enable (BHE) is SET	*/
  
  
  /*Read the DATA from Data Lines*/
  uint8_t ucLowerdata = uiData & 0xff;
  fnExp4Data_Write ( DL_OLATA, ucLowerdata ); /*Write lower byte data*/
  
  /*RESET all the control pins*/
  GPIO_SetBits(BLE_PORT, BLE_PIN); /* Byte Low Enable (BLE) is SET	*/
  GPIO_ResetBits(BHE_PORT, BHE_PIN); /* Byte High Enable (BHE) is SET	*/
  uint8_t ucUpperdata = (uiData >> 8)  & 0xff;
  fnExp4Data_Write ( DL_OLATB, ucUpperdata ); /* Write higher byte data*/
  //printf("ucLowerdata write= 0x%x\n\r",ucLowerdata);
  //printf("ucUpperdata write= 0x%x\n\r",ucUpperdata);
  
  /* Set all control pins*/
  GPIO_SetBits(WE_COMMAN_PORT, WE_COMMAN_PIN);  /* WRITE(W) is SET*/
  GPIO_SetBits(BLE_PORT, BLE_PIN);  /* Byte Low Enable (BLE) is SET	*/
  GPIO_SetBits(BHE_PORT, BHE_PIN);  /* Byte High Enable (BHE) is SET	*/
  GPIO_SetBits(NVSRAM2_EN_PORT, NVSRAM2_EN_PIN); /* CE2 is SET*/
  
}

/********************************************************************************
**@Function 				: 	fnNVSRAM1Byte_Write
**@Descriptions		:		This function assumes that the address is 20 bit
and the access is a BYTE Address  
**@Parameters			: 	uliAddr: 32 bit byte location address 
rgucData: 8 bit data to write   
**@Return					:   None   
*********************************************************************************/
void fnNVSRAM1Byte_Write(uint32_t uliAddr, uint8_t ucData )
{ 
  /*Configuring IO Expander 3 ports To Output as it is connected to address bus*/
  fnExp3Data_Write ( DL_IOCONA , 0x28);  /* DL_IOCONA */
  fnExp3Data_Write ( DL_IODIRA, 0x00);   /* set port A output */
  fnExp3Data_Write ( DL_IODIRB, 0x00);   /* set port B output	*/
  /*Configuring IO Expander 4 ports To Output as WRITE Mode	*/
  fnExp4Data_Write ( DL_IOCONA , 0x28);   /* DL_IOCONA*/
  fnExp4Data_Write ( DL_IODIRA, 0x00);    /* set port A output*/
  fnExp4Data_Write ( DL_IODIRB, 0x00);    /* set port B output*/
  
  /* converting the 20 bit Address to 19 bit Address*/
  uint32_t uNewAddr = (uint32_t)((uliAddr & 0x0007ffff) >> 1);	
  
  /*Loading the address on Address Lines*/
  uint8_t ucLower = uNewAddr & 0xff;
  
  fnExp3Data_Write ( DL_OLATA, ucLower ); /*Write lower byte address*/
  uint8_t ucUpper = (uNewAddr >> 8)  & 0xff;
  
  fnExp3Data_Write ( DL_OLATB, ucUpper ); /*Write higher byte address*/
  
  if ( uNewAddr & ADD16 )  /*Set or Reset A16 pin*/
  {
    GPIO_SetBits(ADD16_PORT, ADD16_PIN);
  }
  else		
  {
    GPIO_ResetBits(ADD16_PORT, ADD16_PIN);
  }
  
  if ( uNewAddr & ADD17 )  /* Set or Reset A17 pin*/
  {
    GPIO_SetBits(ADD17_PORT, ADD17_PIN);
  }
  else		
  {
    GPIO_ResetBits(ADD17_PORT, ADD17_PIN);
  }
  
  if ( uNewAddr & ADD18 )  /* Set or Reset A18 pin*/
  {
    GPIO_SetBits(ADD18_PORT, ADD18_PIN);
  }
  else		
  {
    GPIO_ResetBits(ADD18_PORT, ADD18_PIN);
  }
  if ( uliAddr & ADD19 )  /* Set or Reset A18 pin*/
  {
    GPIO_SetBits(ADD19_PORT, ADD19_PIN);
  }
  else		
  {
    GPIO_ResetBits(ADD19_PORT, ADD19_PIN);
  }
  /*Configuring RAM1 and RAM2 Disabled*/
  GPIO_SetBits(NVSRAM1_EN_PORT, NVSRAM1_EN_PIN);  	/* NVSRAM1 CE Disable*/
  GPIO_SetBits(NVSRAM2_EN_PORT, NVSRAM2_EN_PIN);   /* NVSRAM2 CE Disable*/
  GPIO_SetBits(WE_COMMAN_PORT, WE_COMMAN_PIN); 		/* WRITE(W) is SET*/
  
  /*Reset Chip Enable*/
  GPIO_ResetBits(NVSRAM1_EN_PORT, NVSRAM1_EN_PIN);  /* NVSRAM1 CE RESET*/
  GPIO_ResetBits(WE_COMMAN_PORT, WE_COMMAN_PIN);  /* WRITE(W) is RESET*/
  
  if(uliAddr % 2 == 0)	/* If the Address is EVEN*/
  {	 
    
    /*Configuring RAM1 and RAM2 Disabled*/
    /* Byte High Enable (BHE) is SET	since this is an even access*/
    GPIO_SetBits(BHE_PORT, BHE_PIN);   
    /* Byte Low Enable (BLE) is RESET since this is an even access*/
    GPIO_ResetBits(BLE_PORT, BLE_PIN); 
    
    /* Read the DATA from Data Lines*/
    fnExp4Data_Write ( DL_OLATA, ucData ); /* Write lower byte data*/
    
  }
  else
  {	
    
    /* RESET all the control pins */ 
    GPIO_ResetBits(BHE_PORT, BHE_PIN);   /* Byte High Enable (BHE) is SET	*/
    GPIO_SetBits(BLE_PORT, BLE_PIN);     /* Byte Low Enable (BLE) is RESET*/
    /* Read the DATA from Data Lines */
    fnExp4Data_Write ( DL_OLATB, ucData ); /*Write upper byte data*/
    
  }	
  /* Set all control pins */
  GPIO_SetBits(WE_COMMAN_PORT, WE_COMMAN_PIN); /* WRITE(W) is SET*/
  GPIO_SetBits(BLE_PORT, BLE_PIN); /* Byte Low Enable (BLE) is SET	*/
  GPIO_SetBits(BHE_PORT, BHE_PIN); /* Byte High Enable (BHE) is SET	*/
  GPIO_SetBits(NVSRAM1_EN_PORT, NVSRAM1_EN_PIN); /* CE1 is SET*/
}		

/*******************************************************************************
**@Function 		 : 	fnNVSRAM2Byte_Write
**@Descriptions : 	This function assumes that the address is 20 bit 
and the access is a BYTE Address  
**@Parameters	 : 	uliAddr: 32 bit byte location address 
ucData: 8 bit data to write   
**@Return			 :   None
********************************************************************************/
void fnNVSRAM2Byte_Write(uint32_t uliAddr, uint8_t ucData)
{ 
  /*Configuring IO Expander 3 ports To Output as it is connected to address bus*/
  fnExp3Data_Write ( DL_IOCONA , 0x28);  /* DL_IOCONA*/
  fnExp3Data_Write ( DL_IODIRA, 0x00);   /* set port A output*/
  fnExp3Data_Write ( DL_IODIRB, 0x00);		/* set port B output*/
  
  /*Configuring IO Expander 4 ports To Output as WRITE Mode	*/
  fnExp4Data_Write ( DL_IOCONA , 0x28);	/* DL_IOCONA*/
  fnExp4Data_Write ( DL_IODIRA, 0x00); 	/*set port A output*/
  fnExp4Data_Write ( DL_IODIRB, 0x00);   /*set port B output*/
  
  /*converting the 20 bit Address to 19 bit Address*/
  uint32_t uNewAddr = (uint32_t)((uliAddr & 0x000fffff) >> 1);	  
  
  /* Loading the address on Address Lines*/
  uint8_t ucLower = uNewAddr & 0xff;
  fnExp3Data_Write ( DL_OLATA, ucLower ); 	/*Write lower byte address*/
  uint8_t ucUpper = (uNewAddr >> 8)  & 0xff;
  fnExp3Data_Write ( DL_OLATB, ucUpper );	 /*Write higher byte address */
  
  if ( uNewAddr & ADD16 )  /* Set or Reset A16 pin*/
  {
    GPIO_SetBits(ADD16_PORT, ADD16_PIN);
  }
  else		
  {
    GPIO_ResetBits(ADD16_PORT, ADD16_PIN);
  }
  
  if ( uNewAddr & ADD17 )  /* Set or Reset A17 pin*/
  {
    GPIO_SetBits(ADD17_PORT, ADD17_PIN);
  }
  else		
  {
    GPIO_ResetBits(ADD17_PORT, ADD17_PIN);
  }
  
  if ( uNewAddr & ADD18 )  /* Set or Reset A18 pin*/
  {
    GPIO_SetBits(ADD18_PORT, ADD18_PIN);
  }
  else		
  {
    GPIO_ResetBits(ADD18_PORT, ADD18_PIN);
  }
  if ( uliAddr & ADD19 )  /* Set or Reset A18 pin*/
  {
    GPIO_SetBits(ADD19_PORT, ADD19_PIN);
  }
  else		
  {
    GPIO_ResetBits(ADD19_PORT, ADD19_PIN);
  }
  /*Configuring RAM1 and RAM2 Disabled*/
  GPIO_SetBits(NVSRAM1_EN_PORT, NVSRAM1_EN_PIN);   /* NVSRAM1 CE Disable*/
  GPIO_SetBits(NVSRAM2_EN_PORT, NVSRAM2_EN_PIN);  /* NVSRAM2 CE */
  GPIO_SetBits(WE_COMMAN_PORT, WE_COMMAN_PIN);   /* WRITE(W) is SET*/
  
  /*Reset Chip Enable*/
  GPIO_ResetBits(NVSRAM2_EN_PORT, NVSRAM2_EN_PIN); /* NVSRAM2 CE*/
  GPIO_ResetBits(WE_COMMAN_PORT, WE_COMMAN_PIN); /* WRITE(W) is RESET*/
  
  if(uliAddr % 2 == 0)		/* If the Address is EVEN*/
  {		 
    /*Configuring RAM1 and RAM2 Disabled*/
    /* Byte High Enable (BHE) is SET	since this is an even access*/
    GPIO_SetBits(BHE_PORT, BHE_PIN);  
    /* Byte Low Enable (BLE) is RESET since this is an even access*/
    GPIO_ResetBits(BLE_PORT, BLE_PIN); 
    
    /* Read the DATA from Data Lines*/
    fnExp4Data_Write ( DL_OLATA, ucData ); /* Write lower byte data*/
  }
  else
  {
    /* RESET all the control pins*/
    GPIO_ResetBits(BHE_PORT, BHE_PIN);   /* Byte High Enable (BHE) is SET	*/
    GPIO_SetBits(BLE_PORT, BLE_PIN); 		 /* Byte Low Enable (BLE) is RESET*/
    
    /*Read the DATA from Data Lines*/
    fnExp4Data_Write ( DL_OLATB, ucData ); /*Write upper byte data*/
  }	
  
  /*Set all control pins*/
  GPIO_SetBits(WE_COMMAN_PORT, WE_COMMAN_PIN);  /* WRITE(W) is SET*/
  GPIO_SetBits(BLE_PORT, BLE_PIN);  /* Byte Low Enable (BLE) is SET	*/
  GPIO_SetBits(BHE_PORT, BHE_PIN);  /* Byte High Enable (BHE) is SET	*/
  GPIO_SetBits(NVSRAM2_EN_PORT, NVSRAM2_EN_PIN); /* CE2 is SET*/
}		

/*****************************************************************************
**@Function 			:  fnNVSRAM1Word_Read
**@Descriptions	:  Considering the address to be read is 19 bit
and reading a WORD data
**@Parameters		:  uliAddr: 32 bit word location address   
**@Return				:  16 bit read word      
*****************************************************************************/
uint16_t  fnNVSRAM1Word_Read(uint32_t uliAddr)
{
  uint16_t uiRvalue;
  
  /* Configuring IO Expander 3 ports To Output as it is connected to address bus*/
  fnExp3Data_Write ( DL_IOCONA , 0x28);  /* DL_IOCONA*/
  fnExp3Data_Write ( DL_IODIRA, 0x00);		/* set port A output*/
  fnExp3Data_Write ( DL_IODIRB, 0x00);		/* set port B output	*/
  /*Configuring IO Expander 4 ports To Output as Read Mode change16*/
  fnExp4Data_Write ( DL_IOCONA , 0x28);	/* DL_IOCONA*/
  fnExp4Data_Write ( DL_IODIRA, 0xFF);		/* set port A output*/
  fnExp4Data_Write ( DL_IODIRB, 0xFF);		/* set port B output*/
  
  /*Loading the address on Address Lines*/
  uint8_t ucLower = uliAddr & 0xff;
  fnExp3Data_Write ( DL_OLATA, ucLower ); /*Write lower byte address*/
  uint8_t ucUpper = (uliAddr >> 8)  & 0xff;
  fnExp3Data_Write ( DL_OLATB, ucUpper ); /*Write higher byte address*/
  
  if ( uliAddr & ADD16 )  /* Set or Reset A16 pin*/
  {
    GPIO_SetBits(ADD16_PORT, ADD16_PIN);
  }
  else		
  {
    GPIO_ResetBits(ADD16_PORT, ADD16_PIN);
  }
  
  if ( uliAddr & ADD17 )  /* Set or Reset A17 pin*/
  {
    GPIO_SetBits(ADD17_PORT, ADD17_PIN);
  }
  else		
  {
    GPIO_ResetBits(ADD17_PORT, ADD17_PIN);
  }
  
  if ( uliAddr & ADD18 )  /* Set or Reset A18 pin*/
  {
    GPIO_SetBits(ADD18_PORT, ADD18_PIN);
  }
  else		
  {
    GPIO_ResetBits(ADD18_PORT, ADD18_PIN);
  }
  if ( uliAddr & ADD19 )  /* Set or Reset A18 pin*/
  {
    GPIO_SetBits(ADD19_PORT, ADD19_PIN);
  }
  else		
  {
    GPIO_ResetBits(ADD19_PORT, ADD19_PIN);
  }
  /*Configuring RAM1 and RAM2 Disabled*/
  GPIO_SetBits(NVSRAM2_EN_PORT, NVSRAM2_EN_PIN);  /* CE2 is SET*/
  GPIO_SetBits(WE_COMMAN_PORT, WE_COMMAN_PIN);  /* Write pin is made HIGH*/
  
  /*Reset Chip Enable*/
  GPIO_ResetBits(NVSRAM1_EN_PORT, NVSRAM1_EN_PIN); /* CE1 is RESET*/
  
  /*Reset NVSRAM control pins*/
  GPIO_ResetBits(OE_COMMAN_PORT, OE_COMMAN_PIN); /* Output Enable(OE) is RESET*/
  GPIO_ResetBits(BLE_PORT, BLE_PIN); /* Byte Low Enable (BLE) is RESET	*/
  GPIO_ResetBits(BHE_PORT, BHE_PIN); /* Byte High Enable (BHE) is RESET	*/
  
  /*Read the DATA from Data Lines*/
  uint16_t uiLowerdata = fnExp4Data_Read ( DL_GPIOA );  /* Reading the Lower Byte of data*/
  uint16_t uiUpperdata = fnExp4Data_Read ( DL_GPIOB );  /* Reading the Upper Byte of data*/
  uiRvalue = (uiUpperdata << 8) | uiLowerdata; 
  //printf("uiLowerdata read= 0x%x\n\r",uiLowerdata);
  //printf("uiUpperdata read= 0x%x\n\r",uiUpperdata);
  
  /*SET all the control pins*/
  GPIO_SetBits(NVSRAM1_EN_PORT, NVSRAM1_EN_PIN); /* CE1 is SET*/
  GPIO_SetBits(OE_COMMAN_PORT, OE_COMMAN_PIN); /* Output Enable(OE) is SET*/
  GPIO_SetBits(BLE_PORT, BLE_PIN); /* Byte Low Enable (BLE) is SET	*/
  GPIO_SetBits(BHE_PORT, BHE_PIN); /* Byte High Enable (BHE) is SET	*/
  
  return uiRvalue; /* Returning the 16 bit read value*/
}


/*****************************************************************************
**@Function 			: fnNVSRAM2Word_Read
**@Descriptions	: Considering the address to be read is 19 bit 
and reading a WORD data
**@Parameters		: uliAddr: 32 bit word location address   
**@Return				: 16 bit read word     
*****************************************************************************/
uint16_t  fnNVSRAM2Word_Read(uint32_t uliAddr)
{
  uint16_t uiRvalue;
  
  /*Configuring IO Expander 3 ports To Output as it is connected to address bus*/
  fnExp3Data_Write ( DL_IOCONA , 0x28);		/* DL_IOCONA*/
  fnExp3Data_Write ( DL_IODIRA, 0x00);			/* set port A output*/
  fnExp3Data_Write ( DL_IODIRB, 0x00);			/* set port B output	*/
  /*Configuring IO Expander 4 ports To Output as Read Mode change16*/
  fnExp4Data_Write ( DL_IOCONA , 0x28);		/* DL_IOCONA*/
  fnExp4Data_Write ( DL_IODIRA, 0xFF);			/* set port A output*/
  fnExp4Data_Write ( DL_IODIRB, 0xFF);			/* set port B output*/
  
  /*Loading the address on Address Lines*/
  uint8_t ucLower = uliAddr & 0xff;
  fnExp3Data_Write ( DL_OLATA, ucLower ); /*Write lower byte address*/
  uint8_t ucUpper = (uliAddr >> 8)  & 0xff;
  fnExp3Data_Write ( DL_OLATB, ucUpper ); /*Write higher byte address*/
  
  if ( uliAddr & ADD16 )  /* Set or Reset A16 pin*/
  {
    GPIO_SetBits(ADD16_PORT, ADD16_PIN);
  }
  else		
  {
    GPIO_ResetBits(ADD16_PORT, ADD16_PIN);
  }
  
  if ( uliAddr & ADD17 )  /* Set or Reset A17 pin*/
  {
    GPIO_SetBits(ADD17_PORT, ADD17_PIN);
  }
  else		
  {
    GPIO_ResetBits(ADD17_PORT, ADD17_PIN);
  }
  
  if ( uliAddr & ADD18 )  /* Set or Reset A18 pin*/
  {
    GPIO_SetBits(ADD18_PORT, ADD18_PIN);
  }
  else		
  {
    GPIO_ResetBits(ADD18_PORT, ADD18_PIN);
  }
  if ( uliAddr & ADD19 )  /* Set or Reset A18 pin*/
  {
    GPIO_SetBits(ADD19_PORT, ADD19_PIN);
  }
  else		
  {
    GPIO_ResetBits(ADD19_PORT, ADD19_PIN);
  }
  /*Configuring RAM1 and RAM2 Disabled*/
  GPIO_SetBits(NVSRAM1_EN_PORT, NVSRAM1_EN_PIN);
  GPIO_SetBits(WE_COMMAN_PORT, WE_COMMAN_PIN);  /* Write pin is made HIGH*/
  
  /*Reset Chip Enable*/
  GPIO_ResetBits(NVSRAM2_EN_PORT, NVSRAM2_EN_PIN); /* CE2 is RESET*/
  
  /*Reset NVSRAM control pins*/
  GPIO_ResetBits(OE_COMMAN_PORT, OE_COMMAN_PIN); /* Output Enable(OE) is RESET*/
  GPIO_ResetBits(BLE_PORT, BLE_PIN); /* Byte Low Enable (BLE) is RESET	*/
  GPIO_ResetBits(BHE_PORT, BHE_PIN); /* Byte High Enable (BHE) is RESET	*/
  
  /*Read the DATA from Data Lines*/
  /* Reading the Lower Byte of data*/
  uint16_t uiLowerdata = fnExp4Data_Read ( DL_GPIOA ); 
  /* Reading the Upper Byte of data*/
  uint16_t uiUpperdata = fnExp4Data_Read ( DL_GPIOB ); 
  uiRvalue = (uiUpperdata << 8) | uiLowerdata; 
  //printf("uiLowerdata Read= 0x%x\n\r",uiLowerdata);
  //	printf("uiUpperdata Read= 0x%x\n\r",uiUpperdata);
  
  /*SET all the control pins*/
  GPIO_SetBits(NVSRAM2_EN_PORT, NVSRAM2_EN_PIN); /* CE1 is SET*/
  GPIO_SetBits(OE_COMMAN_PORT, OE_COMMAN_PIN);  /* Output Enable(OE) is SET*/
  GPIO_SetBits(BLE_PORT, BLE_PIN);  /* Byte Low Enable (BLE) is SET	*/
  GPIO_SetBits(BHE_PORT, BHE_PIN);  /* Byte High Enable (BHE) is SET	*/
  
  return uiRvalue; /* Returning the 16 bit read value*/
}

/*****************************************************************************
**@Function 			: fnNVSRAM1Byte_Read
**@Descriptions	: Considering the address to be read is 20 bit
and reading a BYTE data
**@Parameters		: uliAddr: 32 bit word location address   
**@Return				: 8 bit read byte       
*****************************************************************************/
uint8_t  fnNVSRAM1Byte_Read(uint32_t uliAddr)
{
  uint8_t ucRvalue;
  
  
  /*Configuring IO Expander 3 ports To Output as it is connected to address bus*/
  fnExp3Data_Write ( DL_IOCONA , 0x28);	/* DL_IOCONA*/
  fnExp3Data_Write ( DL_IOCONB , 0x28);	/* DL_IOCONB*/
  fnExp3Data_Write ( DL_GPPUA , 0xFF);	/* ENABLE PULL UP'S*/
  fnExp3Data_Write ( DL_GPPUB , 0xFF);	/* ENABLE PULL UP'S*/
  fnExp3Data_Write ( DL_IODIRA, 0x00);		/* set port A output*/
  fnExp3Data_Write ( DL_IODIRB, 0x00);		/* set port B output	*/
  //Configuring IO Expander 4 ports To Output as Read Mode change16*/
  fnExp4Data_Write ( DL_IOCONA , 0x28);	/* DL_IOCONA*/
  fnExp4Data_Write ( DL_IOCONB , 0x28);	/* DL_IOCONA*/
  fnExp3Data_Write ( DL_GPPUA , 0xFF);	/* ENABLE PULL UP'S*/
  fnExp4Data_Write ( DL_GPPUB , 0xFF);	/* ENABLE PULL UP'S*/
  //	fnExp4Data_Write ( DL_IODIRA, 0x00);		/* set port A output*/
  //	fnExp4Data_Write ( DL_IODIRB, 0x00);		/* set port B output*/
  //	fnExp4Data_Write ( DL_GPIOA, 0x00);		/* GPIOA*/
  //	fnExp4Data_Write ( DL_GPIOB, 0x00);		/* GPIOB*/
  fnExp4Data_Write ( DL_IODIRA, 0xFF);		/* set port A input*/
  fnExp4Data_Write ( DL_IODIRB, 0xFF);		/* set port B input*/
  
  /*Configuring RAM1 and RAM2 Disabled*/
  GPIO_SetBits(NVSRAM2_EN_PORT, NVSRAM2_EN_PIN);  /* CE2 is SET*/
  GPIO_SetBits(WE_COMMAN_PORT, WE_COMMAN_PIN);   /* Write pin is made HIGH*/
  
  /* Reset Chip Enable*/
  GPIO_ResetBits(NVSRAM1_EN_PORT, NVSRAM1_EN_PIN); /* CE1 is RESET*/
  GPIO_ResetBits(OE_COMMAN_PORT, OE_COMMAN_PIN); /* Output Enable(OE) is RESET*/
  
  
  /* converting the 20 bit Address to 19 bit Address*/
  
  uint32_t uNewAddr = (uliAddr >> 1);	
  
  /*Loading the address on Address Lines*/
  uint8_t ucLower = uNewAddr & 0xff;
  fnExp3Data_Write ( DL_OLATA, ucLower ); /*Write lower byte address*/
  
  uint8_t ucUpper = (uNewAddr >> 8)  & 0xff;
  fnExp3Data_Write ( DL_OLATB, ucUpper ); /*Write higher byte address*/
  
  if ( uNewAddr & ADD16 )  /* Set or Reset A16 pin*/
  {
    GPIO_SetBits(ADD16_PORT, ADD16_PIN);
  }
  else		
  {
    GPIO_ResetBits(ADD16_PORT, ADD16_PIN);
  }
  
  if ( uNewAddr & ADD17 )  /* Set or Reset A17 pin*/
  {
    GPIO_SetBits(ADD17_PORT, ADD17_PIN);
  }
  else		
  {
    GPIO_ResetBits(ADD17_PORT, ADD17_PIN);
  }
  
  if ( uNewAddr & ADD18 )  /* Set or Reset A18 pin*/
  {
    GPIO_SetBits(ADD18_PORT, ADD18_PIN);
  }
  else		
  {
    GPIO_ResetBits(ADD18_PORT, ADD18_PIN);
  }
  if ( uNewAddr & ADD19 )  /* Set or Reset A18 pin*/
  {
    GPIO_SetBits(ADD19_PORT, ADD19_PIN);
  }
  else		
  {
    GPIO_ResetBits(ADD19_PORT, ADD19_PIN);
  }
  
  
  
  if(uliAddr%2 == 0)		/* If the Address is EVEN*/
  {	
    /*Reset NVSRAM control pins*/
    GPIO_ResetBits(BLE_PORT, BLE_PIN);  /* Byte Low Enable (BLE) is RESET	*/
    GPIO_SetBits(BHE_PORT, BHE_PIN); 		/* Byte High Enable (BHE) is SET	*/
    
    
    /*Read the DATA from Data Lines*/
    ucRvalue = fnExp4Data_Read ( DL_GPIOA );  /*Reading the Lower Byte of data*/
    
    
    
  }	
  else
  {
    /*Reset NVSRAM control pins*/
    GPIO_SetBits(BLE_PORT, BLE_PIN); 		/* Byte Low Enable (BLE) is SET	*/
    GPIO_ResetBits(BHE_PORT, BHE_PIN);  /* Byte High Enable (BHE) is RESET */
    
    /*Read the DATA from Data Lines*/
    ucRvalue = fnExp4Data_Read ( DL_GPIOB );  /* Reading the Upper Byte of data */
    
    
  }
  
  /*SET all the control pins*/
  
  GPIO_SetBits(BLE_PORT, BLE_PIN); /* Byte Low Enable (BLE) is SET */
  GPIO_SetBits(BHE_PORT, BHE_PIN); /* Byte High Enable (BHE) is SET	*/
  
  GPIO_SetBits(NVSRAM1_EN_PORT, NVSRAM1_EN_PIN); /* CE1 is SET*/
  GPIO_SetBits(OE_COMMAN_PORT, OE_COMMAN_PIN); /* Output Enable(OE) is SET*/
  
  return ucRvalue; /* Returning the 8 bit read value	*/
}

/*****************************************************************************
**@Function 			: fnNVSRAM2Byte_Read
**@Descriptions	: Considering the address to be read is 20 bit 
and reading a BYTE data
**@Parameters		: uliAddr: 32 bit word location address   
**@Return				: 8 bit read byte     
*****************************************************************************/
uint8_t  fnNVSRAM2Byte_Read(uint32_t uliAddr)
{
  uint8_t ucRvalue;
  
  /*Configuring IO Expander 3 ports To Output as it is connected to address bus*/
  fnExp3Data_Write ( DL_IOCONA , 0x28);	/* DL_IOCONA*/
  fnExp3Data_Write ( DL_IODIRA, 0x00);		/* set port A output*/
  fnExp3Data_Write ( DL_IODIRB, 0x00);		/* set port B output*/
  /*Configuring IO Expander 4 ports To Output as Read Mode change16*/
  fnExp4Data_Write ( DL_IOCONA , 0x28);	/* DL_IOCONA*/
  fnExp4Data_Write ( DL_IODIRA, 0xFF);		/* set port A output*/
  fnExp4Data_Write ( DL_IODIRB, 0xFF);		/* set port B output*/
  
  /* converting the 20 bit Address to 19 bit Address*/
  uint32_t uNewAddr = (uliAddr >> 1);	  
  
  /*Loading the address on Address Lines*/
  uint8_t ucLower = uNewAddr & 0xff;
  fnExp3Data_Write ( DL_OLATA, ucLower ); /*Write lower byte address*/
  uint8_t ucUpper = (uNewAddr >> 8)  & 0xff;
  fnExp3Data_Write ( DL_OLATB, ucUpper ); /*Write higher byte address*/
  
  if ( uNewAddr & ADD16 )  /* Set or Reset A16 pin*/
  {
    GPIO_SetBits(ADD16_PORT, ADD16_PIN);
  }
  else		
  {
    GPIO_ResetBits(ADD16_PORT, ADD16_PIN);
  }
  
  if ( uNewAddr & ADD17 )  /* Set or Reset A17 pin*/
  {
    GPIO_SetBits(ADD17_PORT, ADD17_PIN);
  }
  else		
  {
    GPIO_ResetBits(ADD17_PORT, ADD17_PIN);
  }
  
  if ( uNewAddr & ADD18 )  /* Set or Reset A18 pin*/
  {
    GPIO_SetBits(ADD18_PORT, ADD18_PIN);
  }
  else		
  {
    GPIO_ResetBits(ADD18_PORT, ADD18_PIN);
  }
  if ( uliAddr & ADD19 )  /* Set or Reset A18 pin*/
  {
    GPIO_SetBits(ADD19_PORT, ADD19_PIN);
  }
  else		
  {
    GPIO_ResetBits(ADD19_PORT, ADD19_PIN);
  }	
  /*Configuring RAM1 and RAM2 Disabled*/
  GPIO_SetBits(NVSRAM1_EN_PORT, NVSRAM1_EN_PIN);
  GPIO_SetBits(WE_COMMAN_PORT, WE_COMMAN_PIN);  /* Write pin is made HIGH*/
  
  /*Reset Chip Enable*/
  GPIO_ResetBits(NVSRAM2_EN_PORT, NVSRAM2_EN_PIN);  /* CE2 is RESET*/
  GPIO_ResetBits(OE_COMMAN_PORT, OE_COMMAN_PIN); 	 /* Output Enable(OE) is RESET*/
  
  
  if(uliAddr % 2 == 0)	/* If the Address is EVEN */
  {	
    /*Reset NVSRAM control pins*/
    GPIO_ResetBits(BLE_PORT, BLE_PIN); /* Byte Low Enable (BLE) is RESET*/	
    GPIO_SetBits(BHE_PORT, BHE_PIN);   /* Byte High Enable (BHE) is SET	*/
    
    /*Read the DATA from Data Lines*/
    ucRvalue = fnExp4Data_Read ( DL_GPIOA );  /* Reading the Lower Byte of data*/
  }	
  else
  {
    /*Reset NVSRAM control pins*/
    GPIO_SetBits(BLE_PORT, BLE_PIN); 		/* Byte Low Enable (BLE) is SET */
    GPIO_ResetBits(BHE_PORT, BHE_PIN); 	/* Byte High Enable (BHE) is RESET	*/
    
    /*Read the DATA from Data Lines*/
    ucRvalue = fnExp4Data_Read ( DL_GPIOB );  /* Reading the Upper Byte of data */
  }
  
  /* SET all the control pins*/
  GPIO_SetBits(NVSRAM2_EN_PORT, NVSRAM2_EN_PIN); /* CE1 is SET */
  GPIO_SetBits(OE_COMMAN_PORT, OE_COMMAN_PIN);  /* Output Enable(OE) is SET*/
  GPIO_SetBits(BLE_PORT, BLE_PIN);  /* Byte Low Enable (BLE) is SET	*/
  GPIO_SetBits(BHE_PORT, BHE_PIN);  /* Byte High Enable (BHE) is SET	*/
  
  return ucRvalue; /* Returning the 8 bit read value	*/
}




/*****************************************************************************
**@Function 			: fnNVSRAM1MultiData_Write
**@Descriptions	: Function to Write to NVSRAM1 with Input ADDRESS as
"BYTE" address.
Zeroth array element of Argument Array will go to 
the First Memory Location
**@Parameters		: ucCount: The number of bytes to write
uliAddr:	32 bit Starting address of data to be written
rgucData_Bytes []: array of data to be written
**@Return				:	None       
*******************************************************************************/
void fnNVSRAM1MultiData_Write( uint8_t ucCount, uint32_t uliAddr, 
                              uint8_t rgucData_Bytes [])
{ 
  int ucLoop = 0;	
  uint16_t uiData;
  
  /*Masking the incoming Address*/
  uliAddr = (uliAddr & ADDRMASK);
  
  /*if Only one byte is to be written*/
  if ( ucCount == 1 )  /* If ucCount = 1 only ONE BYTE WRITE*/
  {
    fnNVSRAM1Byte_Write(uliAddr, rgucData_Bytes [0] );
    return;	
  }
  
  /*if Multiple bytes are to be written*/
  if(uliAddr % 2 == 0)  /*If the address is EVEN*/
  {
    
    if(ucCount % 2 == 0) /*If the DATA COUNT is EVEN*/
    {
      /*ONLY WORD WRITE OPERATIONS ARE PERFORMED as ADDR is EVEN & DATA is EVEN*/
      while( ucCount >= 2) 
      { 
        uiData = (rgucData_Bytes[ucLoop+1] << 8) | rgucData_Bytes[ucLoop];
        /*Changing the BYTE address to WORD address*/
        fnNVSRAM1Word_Write( (uliAddr >>1 ), uiData); 
        ucLoop = ucLoop+2;
        ucCount = ucCount - 2;
        uliAddr = uliAddr + 0x00000002;
      }	
    }
    else /*If the DATA COUNT is ODD*/
    {
      
      /*All WORD WRITE and LAST BYTE WRITE as ADDR is EVEN but DATA is ODD*/
      while( ucCount > 1)  /*WORD WRITE*/
      {
        
        uiData = (rgucData_Bytes[ucLoop+1] << 8) | rgucData_Bytes[ucLoop];
        
        /*Changing the BYTE address to WORD address*/
        fnNVSRAM1Word_Write( (uliAddr >> 1), uiData); 
        ucLoop = ucLoop+2;
        ucCount = ucCount-2;
        uliAddr = uliAddr + 0x00000002;
        
      }
      if ( ucCount == 1 )  /*BYTE WRITE*/
      {	
        
        
        fnNVSRAM1Byte_Write( uliAddr , rgucData_Bytes[ucLoop]);
        ucCount = ucCount-1;
        ucLoop++;
        uliAddr = uliAddr + 0x00000001;
      }	
    }			
  }	
  else  /*If the address is ODD*/
  {
    
    if(ucCount % 2 == 0) /*If the DATA COUNT is EVEN*/
    {			
      /*ONE BYTE WRITE, All WORD WRITE and LAST ONE BYTE WRITE as
      ADDR is ODD but DATA is EVEN*/			
      fnNVSRAM1Byte_Write( uliAddr, rgucData_Bytes[ucLoop]);/*FIRST BYTE WRITE*/
      ucCount = ucCount - 1;
      ucLoop++;
      uliAddr = uliAddr + 0x00000001;
      while( ucCount >= 2) /*ALL WORD WRITE*/
      {
        uiData = (rgucData_Bytes[ucLoop+1] << 8) | rgucData_Bytes[ucLoop];
        /*Changing the BYTE address to WORD address*/
        fnNVSRAM1Word_Write( (uliAddr >> 1) , uiData); 
        ucLoop = ucLoop+2;
        ucCount = ucCount-2;
        uliAddr = uliAddr + 0x00000002;
      }
      if ( ucCount == 1 )  /*LAST BYTE WRITE*/
      {	
        fnNVSRAM1Byte_Write( uliAddr , rgucData_Bytes[ucLoop]);
        ucCount = ucCount-1;
        ucLoop++;
        uliAddr = uliAddr + 0x00000001;				
      }					
    }
    else /*If the DATA is ODD*/
    {
      /*One BYTE WRITE and All WORD WRITE as ADDR is ODD and DATA is ODD*/
      /*FIRST BYTE WRITE*/
      fnNVSRAM1Byte_Write( uliAddr, rgucData_Bytes[ucLoop]); 
      ucCount = ucCount-1;
      ucLoop++;
      uliAddr = uliAddr + 0x00000001;
      while( ucCount >= 2) /*ALL WORD WRITE*/
      {
        uiData = (rgucData_Bytes[ucLoop+1] << 8) | rgucData_Bytes[ucLoop];
        /*Changing the BYTE address to WORD address*/
        fnNVSRAM1Word_Write( (uliAddr >> 1), uiData); 
        ucLoop = ucLoop+2;
        ucCount = ucCount-2;
        uliAddr = uliAddr + 0x00000002;
      }
    }			
  } 			
}	

/*****************************************************************************
**@Function 			: fnNVSRAM2MultiData_Write
**@Descriptions	: Function to Write to NVSRAM2 with Input ADDR as "BYTE" addr.
**@Parameters		: ucCount: The number of bytes to write
uliAddr:	32 bit Starting address of data to be written
rgucData_Bytes []: array of data to be written 
**@Return				:	None       

*****************************************************************************/
void fnNVSRAM2MultiData_Write( uint8_t ucCount, uint32_t uliAddr, 
                              uint8_t rgucData_Bytes[])
{ 
  int ucLoop = 0;	
  uint16_t uiData;
  if ( ucCount == 1 )  /* If ucCount = 1 only ONE BYTE WRITE*/
  {
    fnNVSRAM2Byte_Write(uliAddr, rgucData_Bytes [0] );
    return;	
  }
  
  if(uliAddr % 2 == 0)  /*If the address is EVEN*/
  {
    if(ucCount % 2 == 0) /*If the DATA COUNT is EVEN*/
    {       
      /*ONLY WORD WRITE OPERATIONS ARE PERFORMED as ADDR is EVEN & DATA is EVEN*/
      while( ucCount >= 2) 
      { 
        uiData = (rgucData_Bytes[ucLoop+1] << 8) | rgucData_Bytes[ucLoop];
        /*Changing the BYTE address to WORD address*/
        fnNVSRAM2Word_Write( (uliAddr >>1 ), uiData); 
        ucLoop = ucLoop+2;
        ucCount = ucCount-2;
        uliAddr = uliAddr + 0x00000002;
      }	
    }
    else /*If the DATA COUNT is ODD*/
    {
      /*All WORD WRITE and LAST ONE BYTE WRITE as ADDR is EVEN but DATA is ODD*/
      while( ucCount > 1)  /* WORD WRITE */
      {
        uiData = (uint16_t)((rgucData_Bytes[ucLoop+1] << 8) | rgucData_Bytes[ucLoop]);
        /*Changing the BYTE address to WORD address*/
        fnNVSRAM2Word_Write((uliAddr >> 1), uiData); 
        ucLoop = ucLoop + 2;
        ucCount = ucCount - 2;
        uliAddr = uliAddr + 0x00000002;
      }
      if ( ucCount == 1 )  /*BYTE WRITE*/
      {	
        fnNVSRAM2Byte_Write( uliAddr , rgucData_Bytes[ucLoop]);
        ucCount = ucCount-1;
        ucLoop++;
        uliAddr = uliAddr + 0x00000001;
      }	
    }			
  }	
  else  /* If the address is ODD */
  {
    if(ucCount%2 == 0) /*If the DATA COUNT is EVEN*/
    {			 
      /*ONE BYTE WRITE, All WORD WRITE and LAST ONE BYTE WRITE as 
      ADDR is ODD but DATA is EVEN*/
      /*FIRST BYTE WRITE	*/
      fnNVSRAM2Byte_Write( uliAddr, rgucData_Bytes[ucLoop]); 
      ucCount = ucCount-1;
      ucLoop++;
      uliAddr = uliAddr + 0x00000001;
      while( ucCount >= 2) /* ALL WORD WRITE*/
      {
        uiData = (rgucData_Bytes[ucLoop+1] << 8) | rgucData_Bytes[ucLoop];
        /*Changing the BYTE address to WORD address*/
        fnNVSRAM2Word_Write( (uliAddr >> 1) , uiData); 
        ucLoop = ucLoop+2;
        ucCount = ucCount-2;
        uliAddr = uliAddr + 0x00000002;
      }
      if ( ucCount == 1 )  /*LAST BYTE WRITE*/
      {	
        fnNVSRAM2Byte_Write( uliAddr , rgucData_Bytes[ucLoop]);
        ucCount = ucCount - 1;	
        ucLoop++;
        uliAddr = uliAddr + 0x00000001;
      }					
    }
    else /*If the DATA count is ODD*/
    {    
      /*One BYTE WRITE and All WORD WRITE as ADDR is ODD and DATA is ODD*/
      fnNVSRAM2Byte_Write( uliAddr, rgucData_Bytes[ucLoop]); /*FIRST BYTE WRITE*/
      ucCount = ucCount - 1;
      ucLoop++;
      uliAddr = uliAddr + 0x00000001;
      while( ucCount >= 2) /*ALL WORD WRITE*/
      {
        uiData = (rgucData_Bytes[ucLoop + 1] << 8) | rgucData_Bytes[ucLoop];
        /*Changing the BYTE address to WORD address*/
        fnNVSRAM2Word_Write( (uliAddr >> 1), uiData); 
        ucLoop = ucLoop + 2;
        ucCount = ucCount - 2;
        uliAddr = uliAddr + 0x00000002;
      }
    }			
  } 
}	

/*****************************************************************************
**@Function 		: fnDatalog_Diag
**@Descriptions	: Function Reads Base Register of DIAG_LOG, Writes Time Stamp
and DIAG to memory and updates the Base Register 
**@Parameters		: rgucData_Bytes[6]:Array of DIAG_DATABYTES   
**@Return				: None      
*****************************************************************************/
void fnDatalog_Diag(uint8_t rgucData_Bytes[DL_DIAG_DATABYTE_SIZE])
{
  uint8_t rgucWriteArr[DL_DIAG_PACKET_SIZE];
  // uint8_t rgucTimeStamp[6];
  //	uint8_t rgucTempData[100];
  
  /*Copying the BASE_REGISTER into Local variable uliStatusBaseAddr*/
  uint8_t ucLowerdata = fnNVSRAM1Byte_Read(DL_DIAG_WR_BASE_L);
  uint8_t ucMiddledata= fnNVSRAM1Byte_Read(DL_DIAG_WR_BASE_M);
  uint8_t ucUpperdata = fnNVSRAM1Byte_Read(DL_DIAG_WR_BASE_H);
  uliBaseAddr = (ucUpperdata << 16)| (ucMiddledata << 8)| (ucLowerdata);
  /* Resetting the UNWANTED BITS by Masking with 0x3FF*/
  uliBaseAddr = ( uliBaseAddr & DIAG_MASK );  
  
#ifdef DBG_DIAG_WARN_LOGIC
  static uint16_t DIAGCounter;
  DIAGCounter++;
  printf("\n\rDIAGCOUNT= %d\n\r",DIAGCounter);
  printf("\n\r uliBaseAddr = 0x%x ",uliBaseAddr);
#endif
  
  /*change28*/
  if((DL_DIAG_SEG_WARN < uliBaseAddr)&&(MAXWARNSEND > stWarningLog.ucWarnLog_DiagSendWarnCounter))
  {
   // printf("\n\r DIAGNOSTIC WARNING FLAG SET");
    stWarnLog.stWarnLogFlag.bDiagWarn = SET;
	
  }
  
  /*updatint the Date and Time from RTC registers*/
  RTC_GetTime(RTC_Format_BIN, &stRTC_TimeStructure);
  RTC_GetDate(RTC_Format_BIN, &stRTC_DateStructure);
  
  /*Generating an Array to Write*/
  rgucWriteArr[0] 	= stRTC_DateStructure.RTC_Year;
  rgucWriteArr[1] 	= stRTC_DateStructure.RTC_Month;
  rgucWriteArr[2] 	= stRTC_DateStructure.RTC_Date;
  rgucWriteArr[3] 	= stRTC_TimeStructure.RTC_Hours;
  rgucWriteArr[4] 	= stRTC_TimeStructure.RTC_Minutes;
  rgucWriteArr[5] 	= stRTC_TimeStructure.RTC_Seconds;
//  rgucWriteArr[6] 	=	ucJobNameID ;/*change23*/
//  rgucWriteArr[7] 	=	ucSupervisorNameID;/*change23*/
//	rgucWriteArr[8] 	=	ucJobID;/*change49*/
	rgucWriteArr[6] 	= ucJobNameLog ;/*change78*/
 	rgucWriteArr[7] 	=	ucSupervisorNameLog;/*change78*/
	rgucWriteArr[8] 	=	ucJobLog;/*change78*/
  rgucWriteArr[9] 	= rgucData_Bytes[0];  /* DIAGNOSTIC LOG BYTE 0 */
  rgucWriteArr[10] 	= rgucData_Bytes[1]; 	/* DIAGNOSTIC LOG BYTE 1 */
  rgucWriteArr[11] 	= rgucData_Bytes[2]; 	/* DIAGNOSTIC LOG BYTE 2 */
  rgucWriteArr[12] 	= rgucData_Bytes[3];  /* DIAGNOSTIC LOG BYTE 3 */
  rgucWriteArr[13] 	= rgucData_Bytes[4]; 	/* DIAGNOSTIC LOG BYTE 4 */
  rgucWriteArr[14] 	= rgucData_Bytes[5]; 	/* DIAGNOSTIC LOG BYTE 5 */
	rgucWriteArr[15] 	= rgucData_Bytes[6]; 	/* DIAGNOSTIC LOG BYTE 6 */
  
#ifdef DBG_DIAG_LOG_WRITE
  printf("20%02d/%02d/%02d, %02d:%02d:%02d,\r\n ",
         rgucWriteArr[BYTE_ZERO],rgucWriteArr[BYTE_ONE],
         rgucWriteArr[BYTE_TWO], rgucWriteArr[BYTE_THREE],
         rgucWriteArr[BYTE_FOUR], rgucWriteArr[BYTE_FIVE]);
#endif		
  
  /*Write to NVSRAM1*/
  /*MCU data is written at Addr Base (without offset)*/
  
  fnNVSRAM1MultiData_Write( DL_DIAG_PACKET_SIZE, uliBaseAddr, rgucWriteArr);	
  
  
  
  /*Check if this Log can fit in the Memory, if NOT then Wrap Around*/
  if(uliBaseAddr < DL_DIAG_WRAP)
  {
    uint32_t uliBase_Local;  /*new Local Variable to UPDATE the BASE_REGISTER*/
    uliBase_Local = uliBaseAddr + DL_DIAG_PACKET_SIZE;
    uint8_t ucLower = uliBase_Local & 0xff;
    uint8_t ucMiddle = (uliBase_Local >> 8)  & 0xff;
    uint8_t ucUpper = (uliBase_Local >> 16)  & 0xff;
    uint8_t rguBase_Reset[] = {ucLower, ucMiddle, ucUpper};
    fnNVSRAM1MultiData_Write( 3, DL_DIAG_WR_BASE_L, rguBase_Reset);
  }
  /*WRAP AROUND if BASE_REGISTER is greater or equal to DL_DIAG_END*/
  else if(uliBaseAddr >= DL_DIAG_WRAP) 
  {
    /* Zeroth array element of Argument Array*/
    uint8_t rguBase_Reset[] = {DIAGBASE_LOW,DIAGBASE_MID,DIAGBASE_HIGH};  
    /* will go to the First Memory Location*/
    fnNVSRAM1MultiData_Write( 3, DL_DIAG_WR_BASE_L, rguBase_Reset);
    /*change28*/
    stWarningLog.ucWarnLog_DiagSendWarnCounter = RESET;
#ifdef DBG_DIAG_WARN_LOGIC
    DIAGCounter=0;
#endif
  }
}

/*****************************************************************************
**@Function 			:  fnStatus_Log
**@Descriptions	:  Function to Write STATUS Log to NVSRAM1.
**@Parameters		:  None  
**@Return				:  None 
*****************************************************************************/
void fnStatus_Log(void)
{
#ifdef DBG_STAT_LOG_WRITE
  uint8_t rgucTimeStamp[6];
  static uint16_t STACounter;
  STACounter++;
  printf("\n\rSTACOUNT= %d\n\r",STACounter);
  uint8_t Datacount = 6;
  rgucStatusLog[Datacount++]=0x1F;rgucStatusLog[Datacount++]=0x85;rgucStatusLog[Datacount++]=0x50;rgucStatusLog[Datacount++]=0x42;							
  rgucStatusLog[Datacount++]=0x1F;rgucStatusLog[Datacount++]=0x85;rgucStatusLog[Datacount++]=0x50;rgucStatusLog[Datacount++]=0x42;							
  rgucStatusLog[Datacount++]=0x1F;rgucStatusLog[Datacount++]=0x85;rgucStatusLog[Datacount++]=0x50;rgucStatusLog[Datacount++]=0x42;							
  rgucStatusLog[Datacount++]=0x1F;rgucStatusLog[Datacount++]=0x85;rgucStatusLog[Datacount++]=0x50;rgucStatusLog[Datacount++]=0x42;							
  rgucStatusLog[Datacount++]=0x1F;rgucStatusLog[Datacount++]=0x85;rgucStatusLog[Datacount++]=0x50;rgucStatusLog[Datacount++]=0x42;							
  rgucStatusLog[Datacount++]=0x1F;rgucStatusLog[Datacount++]=0x85;rgucStatusLog[Datacount++]=0x50;rgucStatusLog[Datacount++]=0x42;							
  rgucStatusLog[Datacount++]=0x1F;rgucStatusLog[Datacount++]=0x85;rgucStatusLog[Datacount++]=0x50;rgucStatusLog[Datacount++]=0x42;							
  rgucStatusLog[Datacount++]=0x1F;rgucStatusLog[Datacount++]=0x85;rgucStatusLog[Datacount++]=0x50;rgucStatusLog[Datacount++]=0x42;							
  rgucStatusLog[Datacount++]=0x1F;rgucStatusLog[Datacount++]=0x85;rgucStatusLog[Datacount++]=0x50;rgucStatusLog[Datacount++]=0x42;							
  rgucStatusLog[Datacount++]=0x1F;rgucStatusLog[Datacount++]=0x85;rgucStatusLog[Datacount++]=0x50;rgucStatusLog[Datacount++]=0x42;							
  rgucStatusLog[Datacount++]=0x09;rgucStatusLog[Datacount++]=0x08;
  rgucStatusLog[Datacount++]=0x1F;rgucStatusLog[Datacount++]=0x85;rgucStatusLog[Datacount++]=0x50;rgucStatusLog[Datacount++]=0x42;							
  rgucStatusLog[Datacount++]=0x1F;rgucStatusLog[Datacount++]=0x85;rgucStatusLog[Datacount++]=0x50;rgucStatusLog[Datacount++]=0x42;							
  rgucStatusLog[Datacount++]=0x1F;rgucStatusLog[Datacount++]=0x85;rgucStatusLog[Datacount++]=0x50;rgucStatusLog[Datacount++]=0x42;							
  //		fnNVSRAM1_MultiDataRead(3, DL_PRD_WR_BASE_L, rgucTimeStamp);	
  //	printf("\n\rNVSRAM DATA at DL_PRD_WR_BASE_L =%x ,%x ,%x \r\n",
  //				rgucTimeStamp[0],rgucTimeStamp[1],rgucTimeStamp[2]);
  
#endif
  
  /*Copying the BASE_REGISTER into Local variable uliStatusBaseAddr*/
  uint8_t ucLowerdata = fnNVSRAM1Byte_Read(DL_STA_WR_BASE_L);
  uint8_t ucMiddledata = fnNVSRAM1Byte_Read(DL_STA_WR_BASE_M);
  uint8_t ucUpperdata = fnNVSRAM1Byte_Read(DL_STA_WR_BASE_H);
  uliStatusBaseAddr = (ucUpperdata << 16) | (ucMiddledata << 8) | (ucLowerdata);
  /* Resetting the UNWANTED BITS by Masking with 0x7FFF*/
  uliStatusBaseAddr = ( uliStatusBaseAddr & STA_MASK );  
  
#ifdef DBG_STATUS_WARN_LOGIC
  static uint16_t STATUSCounter;
  STATUSCounter++;
  printf("\n\rSTATUSCounter= %d\n\r",STATUSCounter);
  printf("\n\r uliStatusBaseAddr = 0x%x ",uliStatusBaseAddr);
#endif
  /*change28*/
  if((DL_STA_SEG_WARN < uliStatusBaseAddr)&&(MAXWARNSEND > stWarningLog.ucWarnLog_StatusSendWarnCounter))
  {
//    printf("\n\r STATUS WARNING FLAG SET");
    stWarnLog.stWarnLogFlag.bStatusWarn = SET;
  }
  /* updating the Date and Time from RTC registers*/
  RTC_GetTime(RTC_Format_BIN, &stRTC_TimeStructure);
  RTC_GetDate(RTC_Format_BIN, &stRTC_DateStructure);
  
  /* UPDATING array WITH DATE and TIME*/
  rgucStatusLog[BYTE_ZERO] 	= stRTC_DateStructure.RTC_Year;
  rgucStatusLog[BYTE_ONE] 	= stRTC_DateStructure.RTC_Month;
  rgucStatusLog[BYTE_TWO] 	= stRTC_DateStructure.RTC_Date;
  rgucStatusLog[BYTE_THREE] = stRTC_TimeStructure.RTC_Hours;
  rgucStatusLog[BYTE_FOUR] 	=	stRTC_TimeStructure.RTC_Minutes;
  rgucStatusLog[BYTE_FIVE] 	= stRTC_TimeStructure.RTC_Seconds;
  
  
#ifdef GENERAL_DBG
  printf("Status\r\n 20%02d/%02d/%02d, %02d:%02d:%02d,\r\n ",
         rgucStatusLog[BYTE_ZERO],rgucStatusLog[BYTE_ONE],
         rgucStatusLog[BYTE_TWO], rgucStatusLog[BYTE_THREE],
         rgucStatusLog[BYTE_FOUR], rgucStatusLog[BYTE_FIVE]);
#endif		
//  rgucStatusLog[BYTE_SIX] 		=	ucJobNameID ;/*change23*/
//  rgucStatusLog[BYTE_SEVEN] 	=	ucSupervisorNameID;/*change23*/
//	rgucStatusLog[BYTE_EIGHT] 	=	ucJobID;/*change49*/
	
	rgucStatusLog[BYTE_SIX] 		= ucJobNameLog ;/*change78*/
 	rgucStatusLog[BYTE_SEVEN] 	=	ucSupervisorNameLog;/*change78*/
	rgucStatusLog[BYTE_EIGHT] 	=	ucJobLog;/*change78*/
  /*Write to NVSRAM1*/
  /*WRITING THE STATUS_LOG_ARRAY TO MEMORY */
  fnNVSRAM1MultiData_Write( DL_STA_PACKET_SIZE, uliStatusBaseAddr, rgucStatusLog);	
  
  /*Read the data Written and send on UART */
#ifdef DBG_PRIO_LOG_READ
  fnMemory_Read(uliStatusBaseAddr, DL_STA_PACKET_SIZE); 
#endif
  /*Check if this Log can fit in the Available Memory, if NOT then Wrap Around*/
  if(uliStatusBaseAddr < DL_STA_WRAP)
  {
    uint32_t uliSTABase_Local; /* new Local Variable to UPDATE the BASE_REGISTER*/
    uliSTABase_Local = uliStatusBaseAddr + DL_STA_PACKET_SIZE;
    uint8_t ucLower = uliSTABase_Local & 0xff;
    uint8_t ucMiddle = (uliSTABase_Local >> 8)  & 0xff;
    uint8_t ucUpper = (uliSTABase_Local >> 16)  & 0xff;
    uint8_t rgucSTABase_Reset[] = {ucLower, ucMiddle, ucUpper};
    fnNVSRAM1MultiData_Write( 3, DL_STA_WR_BASE_L, rgucSTABase_Reset);
  }
  /* WRAP AROUND if BASE_REGISTER is greater or equal to DL_DIAG_END*/
  else if(uliStatusBaseAddr >= DL_STA_WRAP) 
  {
    /*Zeroth array element of Argument Array will go to the First Memory Location*/
    uint8_t rguSTABase_Reset[] = {STABASE_LOW,STABASE_MID,STABASE_HIGH};                                         
    fnNVSRAM1MultiData_Write( 3, DL_STA_WR_BASE_L, rguSTABase_Reset);
#ifdef DBG_STATUS_WARN_LOGIC
    STATUSCounter=0;
#endif
    /*change28*/
    stWarningLog.ucWarnLog_StatusSendWarnCounter = RESET;
  }
  /*RESET the Status byte*/
  ucStatusByte = 0;
}

/*****************************************************************************
**@Function 			: fnFault_Log
**@Descriptions	: Function to Write to STA_DIAGNOSTICS data to NVSRAM1.
**@Parameters		: None
**@Return				:	None     
*****************************************************************************/
void fnFault_Log(void)
{
  uint8_t rgucWriteArr[DL_FLT_PACKET_SIZE];
  
  /*Copying the BASE_REGISTER into Local variable uliStatusBaseAddr*/
  uint8_t ucLowerdata = fnNVSRAM1Byte_Read(DL_FLT_WR_BASE_L);
  uint8_t ucMiddledata = fnNVSRAM1Byte_Read(DL_FLT_WR_BASE_M);
  uint8_t ucUpperdata = fnNVSRAM1Byte_Read(DL_FLT_WR_BASE_H);
  uliFaultBaseAddr = (ucUpperdata << 16) | (ucMiddledata << 8) | (ucLowerdata);
  /* Resetting the UNWANTED BITS by Masking with 0x7FFF*/
  uliFaultBaseAddr = ( uliFaultBaseAddr & FLT_MASK );  
#ifdef DBG_FAULT_WARN_LOGIC
  static uint16_t FAULTCounter;
  FAULTCounter++;
  printf("\n\rFAULTCounter= %d\n\r",FAULTCounter);
  printf("\n\r uliFaultBaseAddr = 0x%x ",uliFaultBaseAddr);
#endif
  /*change28*/
  if((DL_FLT_SEG_WARN < uliFaultBaseAddr)&&(MAXWARNSEND > stWarningLog.ucWarnLog_FaultSendWarnCounter))
  {
    //printf("\n\r FAULT WARNING FLAG SET");
    stWarnLog.stWarnLogFlag.bFaultWarn = SET;
  }
  
  /*updatint the Date and Time from RTC registers*/
  RTC_GetTime(RTC_Format_BIN, &stRTC_TimeStructure);
  RTC_GetDate(RTC_Format_BIN, &stRTC_DateStructure);
  
  /*Generating an Array to Write*/
  rgucWriteArr[BYTE_ZERO] 	= stRTC_DateStructure.RTC_Year;
  rgucWriteArr[BYTE_ONE] 		= stRTC_DateStructure.RTC_Month;
  rgucWriteArr[BYTE_TWO] 		= stRTC_DateStructure.RTC_Date;
  rgucWriteArr[BYTE_THREE] 	= stRTC_TimeStructure.RTC_Hours;
  rgucWriteArr[BYTE_FOUR] 	= stRTC_TimeStructure.RTC_Minutes;
  rgucWriteArr[BYTE_FIVE] 	= stRTC_TimeStructure.RTC_Seconds;
 // rgucWriteArr[BYTE_SIX] 		=	ucJobNameID ;/*change23*/
	rgucWriteArr[BYTE_SIX] 		= ucJobNameLog ;/*change78*/
 // rgucWriteArr[BYTE_SEVEN] 	=	ucSupervisorNameID;/*change23*/
	rgucWriteArr[BYTE_SEVEN] 	=	ucSupervisorNameLog;/*change78*/
//	rgucWriteArr[BYTE_EIGHT] 	=	ucJobID;/*change49*/
	rgucWriteArr[BYTE_EIGHT] 	=	ucJobLog;/*change78*/
  rgucWriteArr[BYTE_NINE] 	= Fault_Log.rgucFaultLogFlag[BYTE_ZERO];
  rgucWriteArr[BYTE_TEN] 		= Fault_Log.rgucFaultLogFlag[BYTE_ONE];
  rgucWriteArr[BYTE_ELEVEN] = Fault_Log.rgucFaultLogFlag[BYTE_TWO];
  rgucWriteArr[BYTE_TWELVE] = Fault_Log.rgucFaultLogFlag[BYTE_THREE];
  rgucWriteArr[BYTE_THIRTEEN] = Fault_Log.rgucFaultLogFlag[BYTE_FOUR]; /*change35*/
  
#ifdef DBG_FAULT_LOG_WRITE
  printf("FAULT\r\n20%02d/%02d/%02d, %02d:%02d:%02d,\r\n ",
         rgucWriteArr[BYTE_ZERO],rgucWriteArr[BYTE_ONE],
         rgucWriteArr[BYTE_TWO], rgucWriteArr[BYTE_THREE],
         rgucWriteArr[BYTE_FOUR], rgucWriteArr[BYTE_FIVE]);
#endif		
#ifdef DBG_FAULT_LOG_WRITE
  rgucWriteArr[BYTE_SIX] 		= 0x03 ;
  rgucWriteArr[BYTE_SEVEN] 	= 0x05;
  rgucWriteArr[BYTE_EIGHT] 	= 0x0F;
  rgucWriteArr[BYTE_NINE] 	= 0xF0;
#endif		
  /*Write to NVSRAM1	*/
  /*MCU data is written at Addr Base (without offset)	*/
  fnNVSRAM1MultiData_Write( DL_FLT_PACKET_SIZE, uliFaultBaseAddr, rgucWriteArr);	
  
  
  /*Read the data Written and send on UART */
#ifdef DBG_PRIO_LOG_READ
  fnMemory_Read ( uliFaultBaseAddr, DL_FLT_PACKET_SIZE); 
#endif
  /*Check if this Log can fit in the Memory, if NOT then Wrap Around*/
  if(uliFaultBaseAddr < DL_FLT_WRAP)
  {
    uint32_t uliFLTBase_Local;/*new Local Variable to UPDATE the BASE_REGISTER*/
    uliFLTBase_Local = uliFaultBaseAddr + DL_FLT_PACKET_SIZE;
    uint8_t ucLower = uliFLTBase_Local & 0xff;
    uint8_t ucMiddle = (uliFLTBase_Local >> 8)  & 0xff;
    uint8_t ucUpper = (uliFLTBase_Local >> 16)  & 0xff;
    uint8_t rguFLTBase_Reset[] = {ucLower, ucMiddle, ucUpper};
    fnNVSRAM1MultiData_Write( 3, DL_FLT_WR_BASE_L, rguFLTBase_Reset);
  }
  
  /*WRAP AROUND if BASE_REGISTER is greater or equal to DL_DIAG_END*/
  else if(uliFaultBaseAddr >= DL_FLT_WRAP) 
  {
    /* Zeroth array element of Argument Array
    will go to the First Memory Location*/
    uint8_t rguFLTBase_Reset[] = {FLTBASE_LOW,FLTBASE_MID,FLTBASE_HIGH};  
    fnNVSRAM1MultiData_Write( 3, DL_FLT_WR_BASE_L, rguFLTBase_Reset);
#ifdef DBG_FAULT_WARN_LOGIC
    FAULTCounter=0;
#endif
    
    /*change28*/
    stWarningLog.ucWarnLog_FaultSendWarnCounter = RESET;
  }
  //	ucFaultByte0 = 0;
  //	ucFaultByte1 = 0;
  //	ucFaultByte2 = 0;
  //	ucFaultByte3 = 0;
  rgucHBFail[0] = 0;
  rgucHBFail[1] = 0; 
}

/***********************************************************************************
**@Function 			: fnPeriodic_Log
**@Descriptions	: Data Logging of the Periodic Log
This views 2 NVSRAMs as a contiguous memory with addr as follows
NVSRAM1 0MB to 1MB = 0x00000000 to 0x0007FFFF
NVSRAM2 1MB to 2MB = 0x00080000 to 0x000FFFFF
The following function uses addresses 0x00007C00 to 0x000FFFFF
The first 0x00007BFF addr are used by other data logging functions
**@Parameters		: None
**@Return				: None
*************************************************************************************/
void fnPeriodic_Log(void)
{
#ifdef DBG_PRIO_LOG_WRITE
  uint8_t rgucTimeStamp[6];
  
  //Counter++;
  //	printf("\n\rPRDCOUNT= %d \n\r",Counter);
  uint8_t Datacount = 6;
  rgucPeriodicLog[Datacount++]=0x1F;rgucPeriodicLog[Datacount++]=0x85;rgucPeriodicLog[Datacount++]=0x50;rgucPeriodicLog[Datacount++]=0x42;							
  rgucPeriodicLog[Datacount++]=0x1F;rgucPeriodicLog[Datacount++]=0x85;rgucPeriodicLog[Datacount++]=0x50;rgucPeriodicLog[Datacount++]=0x42;							
  rgucPeriodicLog[Datacount++]=0x1F;rgucPeriodicLog[Datacount++]=0x85;rgucPeriodicLog[Datacount++]=0x50;rgucPeriodicLog[Datacount++]=0x42;							
  rgucPeriodicLog[Datacount++]=0x1F;rgucPeriodicLog[Datacount++]=0x85;rgucPeriodicLog[Datacount++]=0x50;rgucPeriodicLog[Datacount++]=0x42;							
  rgucPeriodicLog[Datacount++]=0x1F;rgucPeriodicLog[Datacount++]=0x85;rgucPeriodicLog[Datacount++]=0x50;rgucPeriodicLog[Datacount++]=0x42;							
  rgucPeriodicLog[Datacount++]=0x1F;rgucPeriodicLog[Datacount++]=0x85;rgucPeriodicLog[Datacount++]=0x50;rgucPeriodicLog[Datacount++]=0x42;							
  rgucPeriodicLog[Datacount++]=0x1F;rgucPeriodicLog[Datacount++]=0x85;rgucPeriodicLog[Datacount++]=0x50;rgucPeriodicLog[Datacount++]=0x42;							
  rgucPeriodicLog[Datacount++]=0x1F;rgucPeriodicLog[Datacount++]=0x85;rgucPeriodicLog[Datacount++]=0x50;rgucPeriodicLog[Datacount++]=0x42;							
  rgucPeriodicLog[Datacount++]=0x1F;rgucPeriodicLog[Datacount++]=0x85;rgucPeriodicLog[Datacount++]=0x50;rgucPeriodicLog[Datacount++]=0x42;							
  rgucPeriodicLog[Datacount++]=0x1F;rgucPeriodicLog[Datacount++]=0x85;rgucPeriodicLog[Datacount++]=0x50;rgucPeriodicLog[Datacount++]=0x42;							
  rgucPeriodicLog[Datacount++]=0x1F;rgucPeriodicLog[Datacount++]=0x85;rgucPeriodicLog[Datacount++]=0x50;rgucPeriodicLog[Datacount++]=0x42;							
  rgucPeriodicLog[Datacount++]=0x1F;rgucPeriodicLog[Datacount++]=0x85;rgucPeriodicLog[Datacount++]=0x50;rgucPeriodicLog[Datacount++]=0x42;							
  rgucPeriodicLog[Datacount++]=0x1F;rgucPeriodicLog[Datacount++]=0x85;rgucPeriodicLog[Datacount++]=0x50;rgucPeriodicLog[Datacount++]=0x42;							
  rgucPeriodicLog[Datacount++]=0x1F;rgucPeriodicLog[Datacount++]=0x85;rgucPeriodicLog[Datacount++]=0x50;rgucPeriodicLog[Datacount++]=0x42;							
  rgucPeriodicLog[Datacount++]=0x1F;rgucPeriodicLog[Datacount++]=0x85;rgucPeriodicLog[Datacount++]=0x50;rgucPeriodicLog[Datacount++]=0x42;							
  rgucPeriodicLog[Datacount++]=0x1F;rgucPeriodicLog[Datacount++]=0x85;rgucPeriodicLog[Datacount++]=0x50;rgucPeriodicLog[Datacount++]=0x42;							
  
  //		fnNVSRAM1_MultiDataRead(3, DL_PRD_WR_BASE_L, rgucTimeStamp);	
  //	printf("\n\rNVSRAM DATA at DL_PRD_WR_BASE_L =%x ,%x ,%x \r\n",
  //				rgucTimeStamp[0],rgucTimeStamp[1],rgucTimeStamp[2]);
  
#endif
  
  
  
  uint32_t uliPRDBase_Local;  /* new Local Variable to UPDATE the BASE_REGISTER*/
  /*Copying the BASE_REGISTER into Local variable uliStatusBaseAddr*/
  uint8_t ucLowerdata 	= fnNVSRAM1Byte_Read(DL_PRD_WR_BASE_L);
  
#ifdef DBG_PRIO_LOG_WRITE
  //printf("fnPeriodic_Log ucLowerdata= %x\n\r" , ucLowerdata);
#endif
  
  uint8_t ucMiddledata 	= fnNVSRAM1Byte_Read(DL_PRD_WR_BASE_M);
  
#ifdef DBG_PRIO_LOG_WRITE
  //	printf("fnPeriodic_Log ucMiddledata= %x\n\r" , ucMiddledata);
#endif
  
  uint8_t ucUpperdata 	= fnNVSRAM1Byte_Read(DL_PRD_WR_BASE_H);
  
#ifdef DBG_PRIO_LOG_WRITE
  //	printf("fnPeriodic_Log ucUpperdata= %x\n\r" , ucUpperdata);
#endif
  
  uliPriodicBaseAddr 		= (uint32_t)((uint32_t)(ucUpperdata << 16) |
                                         (uint32_t)(ucMiddledata << 8)| (uint32_t)(ucLowerdata));	
#ifdef DBG_PERIODIC_WARN_LOGIC
  static uint16_t PERIODICCounter;
  PERIODICCounter++;
  printf("\n\r PERIODICCounter= %d\n\r",PERIODICCounter);
  printf("\n\r uliPriodicBaseAddr = 0x%x ",uliPriodicBaseAddr);
#endif
  /*change28*/
  if((DL_PRD_SEG_WARN < uliPriodicBaseAddr)&&(MAXWARNSEND > stWarningLog.ucWarnLog_PeriodicSendWarnCounter))
  {
//    printf("\n\r PERIODIC WARNING FLAG SET");
    stWarnLog.stWarnLogFlag.bPeriodicWarn = SET;
  }
  
#ifdef DBG_PRIO_LOG_WRITE
  //printf("fnPeriodic_Log uliPriodicBaseAddr= %x\n\r" , uliPriodicBaseAddr);
#endif
  /* Resetting the UNWANTED BITS by Masking with 0xFFFFF*/
  uliPriodicBaseAddr = (uliPriodicBaseAddr & PRD_MASK2 );  
  
  /*updatint the Date and Time from RTC registers	*/
  RTC_GetTime(RTC_Format_BIN, &stRTC_TimeStructure);	
  RTC_GetDate(RTC_Format_BIN, &stRTC_DateStructure);
  
  /*Updating the TimeStamp to the GLOBAL ARRAY*/
  rgucPeriodicLog[BYTE_ZERO] = stRTC_DateStructure.RTC_Year;
  rgucPeriodicLog[BYTE_ONE] = stRTC_DateStructure.RTC_Month;
  rgucPeriodicLog[BYTE_TWO] = stRTC_DateStructure.RTC_Date;
  rgucPeriodicLog[BYTE_THREE] = stRTC_TimeStructure.RTC_Hours;
  rgucPeriodicLog[BYTE_FOUR] = stRTC_TimeStructure.RTC_Minutes ;
  rgucPeriodicLog[BYTE_FIVE] = stRTC_TimeStructure.RTC_Seconds;
  
  
#ifdef DBG_PRIO_LOG_WRITE
  printf("PERIODIC Time\r\n20%02d/%02d/%02d, %02d:%02d:%02d,\r\n ",
         rgucPeriodicLog[BYTE_ZERO],rgucPeriodicLog[BYTE_ONE],
         rgucPeriodicLog[BYTE_TWO], rgucPeriodicLog[BYTE_THREE],
         rgucPeriodicLog[BYTE_FOUR], rgucPeriodicLog[BYTE_FIVE]);
#endif		
//  rgucPeriodicLog[BYTE_SIX] 		=	ucJobNameID ;/*change23*/
//  rgucPeriodicLog[BYTE_SEVEN] 	=	ucSupervisorNameID;/*change23*/
//	rgucPeriodicLog[BYTE_EIGHT] 	=	ucJobID;/*change49*/
	rgucPeriodicLog[BYTE_SIX] 		= ucJobNameLog ;/*change78*/
 	rgucPeriodicLog[BYTE_SEVEN] 	=	ucSupervisorNameLog;/*change78*/
	rgucPeriodicLog[BYTE_EIGHT] 	=	ucJobLog;/*change78*/
	
	//printf("ucJobNameID:%d \n\r ucSupervisorNameID:%d \n\r ucJobID:%d \n\r  ",ucJobNameID,ucSupervisorNameID,ucJobID);
        
  /*checking if to write in NVSRAM1 or NVSRAM2*/
  if ( uliPriodicBaseAddr & PRDADDR_MASK) /*Check if the address is for NVSRAM2*/
  {
    
    fnNVSRAM2MultiData_Write(DL_PRD_PACKET_SIZE, uliPriodicBaseAddr, rgucPeriodicLog);
    
    /*Read the data Written and send on UART */
#ifdef DBG_PRIO_LOG_READ
    fnMemory_Read ( uliPriodicBaseAddr, DL_PRD_PACKET_SIZE); 
#endif
  }
  else /*The address is for NVSRAM1*/
  {
    
#ifdef DBG_PRIO_LOG_WRITE
    printf(" The address is for NVSRAM1 uliPriodicBaseAddr= %d \n\r" , uliPriodicBaseAddr);
#endif
    fnNVSRAM1MultiData_Write( DL_PRD_PACKET_SIZE, uliPriodicBaseAddr, rgucPeriodicLog);
    
    /*Read the data Written and send on UART */
#ifdef DBG_PRIO_LOG_READ
    fnMemory_Read (uliPriodicBaseAddr, DL_PRD_PACKET_SIZE); 
#endif
  }
  
  /*updating the BASE REGISTER*/
  if (uliPriodicBaseAddr & PRDADDR_MASK)  /*THE ADDRESS IS FOR NVSRAM2*/
  {
    
    if(uliPriodicBaseAddr < DL_PRD_WRAP)
    {
      uliPRDBase_Local = uliPriodicBaseAddr + DL_PRD_PACKET_SIZE;
      
      uint8_t ucLower = (uint8_t)(uliPRDBase_Local & 0xff);
      
      uint8_t ucMiddle = (uint8_t)(uliPRDBase_Local >> 8)  & 0xff;
      
      uint8_t ucUpper = (uint8_t)(uliPRDBase_Local >> 16)  & 0xff;
      
      uint8_t rgucPRDBase_Reset[] = {ucLower, ucMiddle, ucUpper};
      fnNVSRAM1MultiData_Write( 3, DL_PRD_WR_BASE_L, rgucPRDBase_Reset);
#ifdef DBG_PRIO_LOG_READ
      printf("RAM2WRITE ");
#endif
    }
    
    /*WRAP AROUND if BASE_REGISTER is greater or equal to DL_DIAG_WRAP*/
    else if(uliPriodicBaseAddr >= DL_PRD_WRAP) 
    {
      
      /* Zeroth array element of Argument Array*/
      /* will go to the First Memory Location*/
      uint8_t rguPRDBase_Reset[] = {PRDBASE_LOW2,PRDBASE_MID2,PRDBASE_HIGH2}; 
      fnNVSRAM1MultiData_Write( 3, DL_PRD_WR_BASE_L, rguPRDBase_Reset);
#ifdef DBG_PRIO_LOG_READ
      printf("\nBASE REG = NVSRAM1 STARTS to WRITE at %x %x %x\n",
             rguPRDBase_Reset[2],rguPRDBase_Reset[1],rguPRDBase_Reset[0]);			
#endif
      /*change28*/
      stWarningLog.ucWarnLog_PeriodicSendWarnCounter = RESET;
#ifdef DBG_PERIODIC_WARN_LOGIC
      PERIODICCounter=0;
#endif
      
      
    }
  }
  else  /*THE ADDRESS IS FOR NVSRAM1*/
  {
    
    if(uliPriodicBaseAddr < DL_PRD_BOUNDARY)
    {
      uliPRDBase_Local = uliPriodicBaseAddr + DL_PRD_PACKET_SIZE;
      
      uint8_t ucLower = uliPRDBase_Local & 0xff;
      uint8_t ucMiddle = (uliPRDBase_Local >> 8)  & 0xff;
      uint8_t ucUpper = (uliPRDBase_Local >> 16)  & 0xff;
      uint8_t rgucPRDBase_Reset[] = {ucLower, ucMiddle, ucUpper};
      fnNVSRAM1MultiData_Write( 3, DL_PRD_WR_BASE_L, rgucPRDBase_Reset);
    }
    /*IF the NVSRAM1 Can't accomodate the next data then reset 
    to zeroth location of NVSRAM2*/
    else if(uliPriodicBaseAddr >= DL_PRD_BOUNDARY) 
    {
      
      /* Zeroth array element of Argument Array*/
      /* will go to the First Memory Location*/
      uint8_t rguPRDBase_Reset[] = {PRDBASE_LOW1,PRDBASE_MID1,PRDBASE_HIGH1}; 
      fnNVSRAM1MultiData_Write( 3, DL_PRD_WR_BASE_L, rguPRDBase_Reset);
    }
  }
}


/*****************************************************************************
**@Function     :  fnDatalogBaseReg_Reset
**@Descriptions :  RESET the BASE REGISTER of NVSRAM1 to 0x03
**@Parameters   :  none
**@Return       :  none 
*****************************************************************************/
void fnDatalogBaseReg_Reset(void)
{
  /*DIAGNOSTIC LOG*/
  // Zeroth array element of Argument Array*/
  /* will go to the First Memory Location*/
  //printf("\n\r RESET LOG");
  uint8_t rgucBase_Reset[] = {DIAGBASE_LOW,DIAGBASE_MID,DIAGBASE_HIGH};    
  fnNVSRAM1MultiData_Write( BYTE_THREE, DL_DIAG_WR_BASE_L, rgucBase_Reset);	
  /*change28*/
  stWarningLog.ucWarnLog_DiagSendWarnCounter = RESET;
  /*STATUS LOG*/
  /* Zeroth array element of Argument Array
  will go to the First Memory Location accessed*/
  uint8_t rgucSTABase_Reset[] = {STABASE_LOW, STABASE_MID, STABASE_HIGH};   
  fnNVSRAM1MultiData_Write( BYTE_THREE, DL_STA_WR_BASE_L, rgucSTABase_Reset);	
  /*change28*/
  stWarningLog.ucWarnLog_StatusSendWarnCounter = RESET;
  /*FAULT LOG*/
  /* Zeroth array element of Argument Array
  will go to the First Memory Location accessed*/
  uint8_t rgucFLTBase_Reset[] = {FLTBASE_LOW, FLTBASE_MID, FLTBASE_HIGH};    
  fnNVSRAM1MultiData_Write( BYTE_THREE, DL_FLT_WR_BASE_L, rgucFLTBase_Reset);	
  /*change28*/
  stWarningLog.ucWarnLog_FaultSendWarnCounter = RESET;
  /*PERIODIC LOG*/
  /* Zeroth array element of Argument Array
  will go to the First Memory Location accessed*/
  uint8_t rgucPRDBase_Reset[] = {PRDBASE_LOW2, PRDBASE_MID2, PRDBASE_HIGH2};  
  fnNVSRAM1MultiData_Write( BYTE_THREE, DL_PRD_WR_BASE_L, rgucPRDBase_Reset);
  
  /*change28*/
  stWarningLog.ucWarnLog_PeriodicSendWarnCounter = RESET;
  
  /*change26*/
  //	ucSupervisorNameID = DEFAULT_CURRENT_INDEX_SUPERVISOR_NAME;
  //	ucSupervisorNameLog = DEFAULT_CURRENT_INDEX_SUPERVISOR_NAME;
	//	 fnSPI_EEPROM_WriteData(CURRENT_INDEX_SUPERVISOR_NAME, 1, &ucSupervisorNameID); /*change78*/
	//							 fnSPI_EEPROM_WriteData(LOG_INDEX_SUPERVISOR_NAME, 1, &ucSupervisorNameLog); /*change78*/
  //	fnResetName(FLASH_Sector_6);
  //	ucJobNameID = DEFAULT_CURRENT_INDEX_JOB_NAME;
  //	fnSPI_EEPROM_WriteData(CURRENT_INDEX_JOB_NAME, 1, &ucJobNameID);
  //	fnResetName(FLASH_Sector_7);		
}

/*****************************************************************************
**@Function     : fnMemory_Read
**@Descriptions : Read the memory contents.
**@Parameters   : ulisStartAddress: 32 bit start location of address to read
uliPacketSize: 32 bit maximum packet size
**@Return       : None      
*****************************************************************************/
#ifdef DBG_PRIO_LOG_READ
void fnMemory_Read (uint32_t ulisStartAddress, uint32_t uliPacketSize)
{
  uint8_t ucCount = 0;
  uint32_t uliStopAddress = (ulisStartAddress + uliPacketSize);
  
  while(ulisStartAddress < uliStopAddress)
  {
    ucCount++;
    
    /* checking if to write in NVSRAM1 or NVSRAM2*/
    if ( ulisStartAddress & PRDADDR_MASK) /*Check if the address is for NVSRAM2*/
    {	
      uint8_t ucRead =  fnNVSRAM2Byte_Read(ulisStartAddress);
#ifdef DBG_PRIO_LOG_READ
      if(ucCount <= 6 )
      {
        printf("%x, ",ucRead);
      }else
      {
        printf("0x%02x, ",ucRead);
      }
#endif
      ulisStartAddress++;
    }
    else /*The address is for NVSRAM1*/
    {
      uint8_t ucRead = fnNVSRAM1Byte_Read(ulisStartAddress);
#ifdef DBG_PRIO_LOG_READ
      printf("%x, ",ucRead);
#endif
      
      ulisStartAddress++;
    }
  }
#ifdef DBG_PRIO_LOG_READ
  printf("\r\n");
#endif
}
#endif

/*****************************************************************************
**@Function 			: fnReserveArray_Reset
**@Descriptions	: RESET the reserved array
**@Parameters		: None  
**@Return				: None     
*****************************************************************************/	
void fnReserveArray_Reset (void)
{
  /* UPDATING THE STATUS RESERVED BYTES TO ZERO*/
  //	rgucStatusLog[BYTE_FIFTYTWO] = DUMMY;
  //	rgucStatusLog[BYTE_FIFTYTHREE] = DUMMY;
  //	rgucStatusLog[BYTE_FIFTYFOUR] = DUMMY;
  //	rgucStatusLog[BYTE_FIFTYFIVE] = DUMMY;
  //	rgucStatusLog[BYTE_FIFTYSIX] = DUMMY;
  //	rgucStatusLog[BYTE_FIFTYSEVEN] = DUMMY;
  //	rgucStatusLog[BYTE_FIFTYEIGHT] = DUMMY;
  //	rgucStatusLog[BYTE_FIFTYNINE] = DUMMY;
  /*UPDATING THE STATUS RESERVED BYTES TO ZERO*/
  //	rgucStatusLog[BYTE_SIXTY] = DUMMY;
  //	rgucStatusLog[BYTE_SIXTYONE] = DUMMY;
  //	rgucStatusLog[BYTE_SIXTYTWO] = DUMMY;
  //	rgucStatusLog[BYTE_SIXTYTHREE] = DUMMY;
  //	rgucStatusLog[BYTE_SIXTYFOUR] = DUMMY;
  //	rgucStatusLog[BYTE_SIXTYFIVE] = DUMMY;
  //	rgucStatusLog[BYTE_SIXTYSIX] = DUMMY;
  //	rgucStatusLog[BYTE_SIXTYSEVEN] = DUMMY;
  //	rgucStatusLog[BYTE_SIXTYEIGHT] = DUMMY;
  //	rgucStatusLog[BYTE_SIXTYNINE] = DUMMY;
  //	rgucStatusLog[BYTE_SEVENTY] = DUMMY;
  
  //	/*UPDATINGING THE PERIODIC RESERVED BYTES TO ZERO	*/
  //	rgucPeriodicLog[BYTE_FIFTYNINE] = DUMMY;
  //	rgucPeriodicLog[BYTE_SIXTY] = DUMMY;
  //	rgucPeriodicLog[BYTE_SIXTYONE] = DUMMY;
  //	rgucPeriodicLog[BYTE_SIXTYTWO] = DUMMY;
  //	rgucPeriodicLog[BYTE_SIXTYTHREE] = DUMMY;
  //	rgucPeriodicLog[BYTE_SIXTYFOUR] = DUMMY;
  //	rgucPeriodicLog[BYTE_SIXTYFIVE] = DUMMY;
  //	rgucPeriodicLog[BYTE_SIXTYSIX] = DUMMY;
  //	rgucPeriodicLog[BYTE_SIXTYSEVEN] = DUMMY;
  //	rgucPeriodicLog[BYTE_SIXTYEIGHT] = DUMMY;
  //	rgucPeriodicLog[BYTE_SIXTYNINE] = DUMMY;
  //	rgucPeriodicLog[BYTE_SEVENTY] = DUMMY;
  //	/*UPDATINGING THE PERIODIC RESERVED BYTES TO ZERO	*/
  //	rgucPeriodicLog[BYTE_SEVENTYONE] = DUMMY;
  //	rgucPeriodicLog[BYTE_SEVENTYTWO] = DUMMY;
  //	rgucPeriodicLog[BYTE_SEVENTYTHREE] = DUMMY;
  //	rgucPeriodicLog[BYTE_SEVENTYFOUR] = DUMMY;
  //	rgucPeriodicLog[BYTE_SEVENTYFIVE] = DUMMY;
  //	rgucPeriodicLog[BYTE_SEVENTYSIX] = DUMMY;
  //	rgucPeriodicLog[BYTE_SEVENTYSEVEN] = DUMMY;
  //	rgucPeriodicLog[BYTE_SEVENTYEIGHT] = DUMMY;
  //	rgucPeriodicLog[BYTE_SEVENTYNINE] = DUMMY;
  //	rgucPeriodicLog[BYTE_EIGHTY] = DUMMY;
  //	/*UPDATINGING THE PERIODIC RESERVED BYTES TO ZERO	*/
  //	rgucPeriodicLog[BYTE_EIGHTYONE] = DUMMY;
  //	rgucPeriodicLog[BYTE_EIGHTYTWO] = DUMMY;
  //	rgucPeriodicLog[BYTE_EIGHTYTHREE] = DUMMY;
  //	rgucPeriodicLog[BYTE_EIGHTYFOUR] = DUMMY;
  //	rgucPeriodicLog[BYTE_EIGHTYFIVE] = DUMMY;
  //	rgucPeriodicLog[BYTE_EIGHTYSIX] = DUMMY;
  //	rgucPeriodicLog[BYTE_EIGHTYSEVEN] = DUMMY;
  //	rgucPeriodicLog[BYTE_EIGHTYEIGHT] = DUMMY;
}
/*****************************************************************************
**@Function 			: fnDataLogging_Init
**@Descriptions	: Configures the Initialization of SPI bus and GPIO extender
**@Parameters		: None   
**@Return				: None      
*****************************************************************************/
void fnDataLogging_Init(void)
{
  uint16_t uiLoop = 0;
  
  GPIO_InitTypeDef stGPIO_InitTypeDefStruct;
  
  
  /*Initializing Port A pins*/
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB |
                         RCC_AHB1Periph_GPIOG | RCC_AHB1Periph_GPIOH |
                           RCC_AHB1Periph_GPIOI,
                           ENABLE);
  
  /*Configuring IO Expander Chip-Select pin*/
  /* Configuring CHIP_SELECT for IO_EXPANDER_3*/	
  stGPIO_InitTypeDefStruct.GPIO_Pin 	= IOEXP3_CS_PIN;
  stGPIO_InitTypeDefStruct.GPIO_OType = GPIO_OType_PP;
  stGPIO_InitTypeDefStruct.GPIO_Mode 	= GPIO_Mode_OUT;
  stGPIO_InitTypeDefStruct.GPIO_PuPd 	= GPIO_PuPd_NOPULL;
  stGPIO_InitTypeDefStruct.GPIO_Speed = GPIO_Speed_50MHz;
  
  GPIO_Init(IOEXP3_CS_PORT, &stGPIO_InitTypeDefStruct);
  GPIO_SetBits(IOEXP3_CS_PORT, IOEXP3_CS_PIN);  //Set to inactive value
  
  /*Configuring CHIP_SELECT for IO_EXPANDER_4*/	
  stGPIO_InitTypeDefStruct.GPIO_Pin = IOEXP4_CS_PIN;
  stGPIO_InitTypeDefStruct.GPIO_OType = GPIO_OType_PP;
  stGPIO_InitTypeDefStruct.GPIO_Mode = GPIO_Mode_OUT;
  stGPIO_InitTypeDefStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  stGPIO_InitTypeDefStruct.GPIO_Speed = GPIO_Speed_50MHz;
  
  GPIO_Init(IOEXP4_CS_PORT, &stGPIO_InitTypeDefStruct);
  GPIO_SetBits(IOEXP4_CS_PORT, IOEXP4_CS_PIN);  /*Set to inactive value*/
  
  /******************* NVSRAM Pins Initialization ****************/
  /*Chip Enable for NVSRAM_1*/	
  stGPIO_InitTypeDefStruct.GPIO_Pin = NVSRAM1_EN_PIN;
  stGPIO_InitTypeDefStruct.GPIO_OType = GPIO_OType_PP;
  stGPIO_InitTypeDefStruct.GPIO_Mode = GPIO_Mode_OUT;
  stGPIO_InitTypeDefStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  stGPIO_InitTypeDefStruct.GPIO_Speed = GPIO_Speed_50MHz;
  
  GPIO_Init(NVSRAM1_EN_PORT, &stGPIO_InitTypeDefStruct);
  GPIO_SetBits(NVSRAM1_EN_PORT, NVSRAM1_EN_PIN);  //Set to inactive value
  
  /*Chip Enable for NVSRAM_2*/	
  stGPIO_InitTypeDefStruct.GPIO_Pin = NVSRAM2_EN_PIN;
  stGPIO_InitTypeDefStruct.GPIO_OType = GPIO_OType_PP;
  stGPIO_InitTypeDefStruct.GPIO_Mode = GPIO_Mode_OUT;
  stGPIO_InitTypeDefStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  stGPIO_InitTypeDefStruct.GPIO_Speed = GPIO_Speed_50MHz;
  
  GPIO_Init(NVSRAM2_EN_PORT, &stGPIO_InitTypeDefStruct);
  GPIO_SetBits(NVSRAM2_EN_PORT, NVSRAM2_EN_PIN);  /*Set to inactive value*/
  
  /*Common Signal LOWER_BYTE_WRITE*/	
  stGPIO_InitTypeDefStruct.GPIO_Pin = BLE_PIN;
  stGPIO_InitTypeDefStruct.GPIO_OType = GPIO_OType_PP;
  stGPIO_InitTypeDefStruct.GPIO_Mode = GPIO_Mode_OUT;
  stGPIO_InitTypeDefStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  stGPIO_InitTypeDefStruct.GPIO_Speed = GPIO_Speed_50MHz;
  
  GPIO_Init(BLE_PORT, &stGPIO_InitTypeDefStruct);
  GPIO_SetBits(BLE_PORT, BLE_PIN);  /*Set to inactive value*/
  
  /*Common Signal UPPER_BYTE_WRITE*/
  stGPIO_InitTypeDefStruct.GPIO_Pin = BHE_PIN;
  stGPIO_InitTypeDefStruct.GPIO_OType = GPIO_OType_PP;
  stGPIO_InitTypeDefStruct.GPIO_Mode = GPIO_Mode_OUT;
  stGPIO_InitTypeDefStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  stGPIO_InitTypeDefStruct.GPIO_Speed = GPIO_Speed_50MHz;
  
  GPIO_Init(BHE_PORT, &stGPIO_InitTypeDefStruct);
  GPIO_SetBits(BHE_PORT, BHE_PIN);  /*Set to inactive value*/
  
  /*Common Signal WRITE_ENABLE*/	
  stGPIO_InitTypeDefStruct.GPIO_Pin = WE_COMMAN_PIN;/*write enable Pin*/
  stGPIO_InitTypeDefStruct.GPIO_OType = GPIO_OType_PP;
  stGPIO_InitTypeDefStruct.GPIO_Mode = GPIO_Mode_OUT;
  stGPIO_InitTypeDefStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  stGPIO_InitTypeDefStruct.GPIO_Speed = GPIO_Speed_50MHz;
  
  GPIO_Init(WE_COMMAN_PORT, &stGPIO_InitTypeDefStruct);
  GPIO_SetBits(WE_COMMAN_PORT, WE_COMMAN_PIN);  /*Set to inactive value*/
  
  /*Common Signal OUTPUT_ENABLE*/	
  stGPIO_InitTypeDefStruct.GPIO_Pin = OE_COMMAN_PIN;
  stGPIO_InitTypeDefStruct.GPIO_OType = GPIO_OType_PP;
  stGPIO_InitTypeDefStruct.GPIO_Mode = GPIO_Mode_OUT;
  stGPIO_InitTypeDefStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  stGPIO_InitTypeDefStruct.GPIO_Speed = GPIO_Speed_50MHz;
  
  GPIO_Init(OE_COMMAN_PORT, &stGPIO_InitTypeDefStruct);
  GPIO_SetBits(OE_COMMAN_PORT, OE_COMMAN_PIN);  /*Set to inactive value*/
  
  /*Common Signal ADDRESS_LINE_A16*/	
  stGPIO_InitTypeDefStruct.GPIO_Pin = ADD16_PIN;
  stGPIO_InitTypeDefStruct.GPIO_OType = GPIO_OType_PP;
  stGPIO_InitTypeDefStruct.GPIO_Mode = GPIO_Mode_OUT;
  stGPIO_InitTypeDefStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  stGPIO_InitTypeDefStruct.GPIO_Speed = GPIO_Speed_50MHz;
  
  GPIO_Init(ADD16_PORT, &stGPIO_InitTypeDefStruct);
  GPIO_SetBits(ADD16_PORT, ADD16_PIN);  /*Set to inactive value*/
  
  /*Common Signal ADDRESS_LINE_A17*/	
  stGPIO_InitTypeDefStruct.GPIO_Pin = ADD17_PIN;
  stGPIO_InitTypeDefStruct.GPIO_OType = GPIO_OType_PP;
  stGPIO_InitTypeDefStruct.GPIO_Mode = GPIO_Mode_OUT;
  stGPIO_InitTypeDefStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  stGPIO_InitTypeDefStruct.GPIO_Speed = GPIO_Speed_50MHz;
  
  GPIO_Init(ADD17_PORT, &stGPIO_InitTypeDefStruct);
  GPIO_SetBits(ADD17_PORT, ADD17_PIN);  /*Set to inactive value*/
  
  /*Common Signal ADDRESS_LINE_A18*/	
  stGPIO_InitTypeDefStruct.GPIO_Pin = ADD18_PIN;
  stGPIO_InitTypeDefStruct.GPIO_OType = GPIO_OType_PP;
  stGPIO_InitTypeDefStruct.GPIO_Mode = GPIO_Mode_OUT;
  stGPIO_InitTypeDefStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  stGPIO_InitTypeDefStruct.GPIO_Speed = GPIO_Speed_50MHz;
  
  GPIO_Init(ADD18_PORT, &stGPIO_InitTypeDefStruct);
  GPIO_SetBits(ADD18_PORT, ADD18_PIN);  /*Set to inactive value*/
  
  /*Common Signal ADDRESS_LINE_A19*/	
  stGPIO_InitTypeDefStruct.GPIO_Pin = ADD19_PIN;
  stGPIO_InitTypeDefStruct.GPIO_OType = GPIO_OType_PP;
  stGPIO_InitTypeDefStruct.GPIO_Mode = GPIO_Mode_OUT;
  stGPIO_InitTypeDefStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  stGPIO_InitTypeDefStruct.GPIO_Speed = GPIO_Speed_50MHz;
  
  GPIO_Init(ADD19_PORT, &stGPIO_InitTypeDefStruct);
  GPIO_SetBits(ADD19_PORT, ADD19_PIN);  /*Set to inactive value*/
  
  
  /*********************** IO_EXPANDER_3 Pins Initialization**************/
  /* INTERRUPT_A_EXPANDER3*/	
  stGPIO_InitTypeDefStruct.GPIO_Pin = EXP3_INTR_A_PIN; 
  stGPIO_InitTypeDefStruct.GPIO_OType = GPIO_OType_PP;
  stGPIO_InitTypeDefStruct.GPIO_Mode = GPIO_Mode_IN;
  stGPIO_InitTypeDefStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  stGPIO_InitTypeDefStruct.GPIO_Speed = GPIO_Speed_50MHz;
  
  GPIO_Init(EXP3_INTR_A_PORT, &stGPIO_InitTypeDefStruct);
  
  /* INTERRUPT_B_EXPANDER3*/
  stGPIO_InitTypeDefStruct.GPIO_Pin = EXP3_INTR_B_PIN;
  stGPIO_InitTypeDefStruct.GPIO_OType = GPIO_OType_PP;
  stGPIO_InitTypeDefStruct.GPIO_Mode = GPIO_Mode_IN;
  stGPIO_InitTypeDefStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  stGPIO_InitTypeDefStruct.GPIO_Speed = GPIO_Speed_50MHz;
  
  GPIO_Init(EXP3_INTR_B_PORT, &stGPIO_InitTypeDefStruct);	
  
  /* RESETB_EXPANDER3*/	
  stGPIO_InitTypeDefStruct.GPIO_Pin = EXP3_RESET_PIN;
  stGPIO_InitTypeDefStruct.GPIO_OType = GPIO_OType_PP;
  stGPIO_InitTypeDefStruct.GPIO_Mode = GPIO_Mode_IN;
  stGPIO_InitTypeDefStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  stGPIO_InitTypeDefStruct.GPIO_Speed = GPIO_Speed_50MHz;
  
  GPIO_Init(EXP3_RESET_PORT, &stGPIO_InitTypeDefStruct);
  
  
  /**********************IO_EXPANDER_4 Pins Initialization*********/
  /* INTERRUPT_A_EXPANDER4*/	
  stGPIO_InitTypeDefStruct.GPIO_Pin = EXP4_INTR_A_PIN;
  stGPIO_InitTypeDefStruct.GPIO_OType = GPIO_OType_PP;
  stGPIO_InitTypeDefStruct.GPIO_Mode = GPIO_Mode_IN;
  stGPIO_InitTypeDefStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  stGPIO_InitTypeDefStruct.GPIO_Speed = GPIO_Speed_50MHz;
  
  GPIO_Init(EXP4_INTR_A_PORT, &stGPIO_InitTypeDefStruct);
  
  /*INTERRUPT_B_EXPANDER4*/
  stGPIO_InitTypeDefStruct.GPIO_Pin = EXP4_INTR_B_PIN;
  stGPIO_InitTypeDefStruct.GPIO_OType = GPIO_OType_PP;
  stGPIO_InitTypeDefStruct.GPIO_Mode = GPIO_Mode_IN;
  stGPIO_InitTypeDefStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  stGPIO_InitTypeDefStruct.GPIO_Speed = GPIO_Speed_50MHz;
  
  GPIO_Init(EXP4_INTR_B_PORT, &stGPIO_InitTypeDefStruct);	
  
  /* RESETB_EXPANDER4*/
  stGPIO_InitTypeDefStruct.GPIO_Pin = EXP4_RESET_PIN;
  stGPIO_InitTypeDefStruct.GPIO_OType = GPIO_OType_PP;
  stGPIO_InitTypeDefStruct.GPIO_Mode = GPIO_Mode_IN;
  stGPIO_InitTypeDefStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  stGPIO_InitTypeDefStruct.GPIO_Speed = GPIO_Speed_50MHz;
  
  GPIO_Init(EXP4_RESET_PORT, &stGPIO_InitTypeDefStruct);
  
  /*Resetting the RESET of both IO_EXPNADERS*/
  GPIO_SetBits(EXP4_RESET_PORT, EXP4_RESET_PIN); 
  GPIO_SetBits(EXP3_RESET_PORT, EXP3_RESET_PIN);
  for (uiLoop = 0; uiLoop < 500; uiLoop++);
  
  GPIO_ResetBits(EXP4_RESET_PORT, EXP4_RESET_PIN); 
  GPIO_ResetBits(EXP3_RESET_PORT, EXP3_RESET_PIN);
  for(uiLoop = 0; uiLoop < 500; uiLoop++);
}

/*************************************************************************
**@Function			: fnNVSRAM1_MultiDataRead
**@Descriptions	: Testing of the GPIO and the extended GPIO pins.
**@parameters		: ucCount: No. of Bytes to read 
uliAddr: Memory Address
pucReadTo: Pointer to Array for read Data
**@return				: None      
**************************************************************************/
void fnNVSRAM1_MultiDataRead(uint8_t ucCount, uint32_t uliAddr, uint8_t *pucReadTo)
{
  uint8_t ucLoop = 0;	
  for(ucLoop = 0; ucLoop < ucCount; ucLoop++)
  {
    pucReadTo[ucLoop] = fnNVSRAM1Byte_Read(uliAddr + ucLoop); 
    
  }
}



/*****************************************************************************
**@Function			: fnNVSRAM2_MultiDataRead
**@Descriptions	: Testing of the GPIO and the extended GPIO pins.
**@parameters		: ucCount: No. of Bytes to read 
uliAddr: Memory Address
pucReadTo: Pointer to Array for read Data
**@return				: None      
*******************************************************************************/
void fnNVSRAM2_MultiDataRead(uint8_t ucCount, uint32_t uliAddr, uint8_t *pucReadTo)
{
  int ucLoop = 0;	
  for(ucLoop = 0; ucLoop < ucCount; ucLoop++)
  {
    pucReadTo[ucLoop] = fnNVSRAM2Byte_Read(uliAddr + ucLoop); 
  }
}
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
      memset(rgcFiledata,'\0',100);
      strncpy(rgcFiledata,(pTemp1+strlen("<Start>")),(pTemp2-(pTemp1+strlen("<Start>"))));
      if(strstr(rgcFiledata,"at Time"))
      {
        if((pTemp1 = strstr((char *)XMLstring,"<Start_Time>")) != NULL)
        {
          if((pTemp2 = strstr(pTemp1,"</Start_Time>")) != NULL)
          {
            memset(rgcFiledata,'\0',100);
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
      else if(strstr(rgcFiledata,"Beginning"))
      {
				#ifdef DBG_DATALOG
        printf("\n\rat Beginning of Log");
				#endif
        pstLog->stStartLogDateTime.uliTotalSec = 0;
      }
    }
  }
  if((pTemp1 = strstr((char *)XMLstring,"<Stop>")) != NULL)
  {
    if((pTemp2 = strstr(pTemp1,"</Stop>")) != NULL)
    {
      memset(rgcFiledata,'\0',100);
      strncpy(rgcFiledata,(pTemp1+strlen("<Stop>")),(pTemp2-(pTemp1+strlen("<Stop>"))));
      if(strstr(rgcFiledata,"at Time"))
      {
        if((pTemp1 = strstr((char *)XMLstring,"<Stop_Time>")) != NULL)
        {
          if((pTemp2 = strstr(pTemp1,"</Stop_Time>")) != NULL)
          {
            memset(rgcFiledata,'\0',100);
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
      else if(strstr(rgcFiledata,"End"))
      {
        
        pstLog->stEndLogDateTime.uliTotalSec = 0xFFFFFFFF;
				#ifdef DBG_DATALOG
        printf("\n\rat End of Log");
				#endif
      }
    }
  }
  if((pTemp1 = strstr((char *)XMLstring,"<JobName>")) != NULL)
  {
    if((pTemp2 = strstr(pTemp1,"</JobName>")) != NULL)
    {
      memset(rgcFiledata,'\0',100);
      strncpy(rgcFiledata,(pTemp1+strlen("<JobName>")),(pTemp2-(pTemp1+strlen("<JobName>"))));
			strcpy(pstLog->cJobName,rgcFiledata);
			#ifdef DBG_DATALOG
			printf("\n\rJobName : %s",pstLog->cJobName);
			#endif

    }
  }
  if((pTemp1 = strstr((char *)XMLstring,"<JobID>")) != NULL)
  {
    if((pTemp2 = strstr(pTemp1,"</JobID>")) != NULL)
    {
      memset(rgcFiledata,'\0',100);
      strncpy(rgcFiledata,(pTemp1+strlen("<JobID>")),(pTemp2-(pTemp1+strlen("<JobID>"))));
			strcpy(pstLog->cJobID,rgcFiledata);
			#ifdef DBG_DATALOG
			printf("\n\rJobID : %s",pstLog->cJobID);
			#endif
    }
  }
	else
	{
		#ifdef DBG_DATALOG
		printf("\n\rJobID Not Present");
		#endif
		pstLog->cJobID[0] = '\0';
	}
	if((pTemp1 = strstr((char *)XMLstring,"<Process>")) != NULL)
  {
    if((pTemp2 = strstr(pTemp1,"</Process>")) != NULL)
    {
      memset(rgcFiledata,'\0',100);
      strncpy(rgcFiledata,(pTemp1+strlen("<Process>")),(pTemp2-(pTemp1+strlen("<Process>"))));
			strcpy(pstLog->cProcess,rgcFiledata);
			#ifdef DBG_DATALOG
			printf("\n\rProcess : %s",pstLog->cProcess);
			#endif
    }
  }
	else
	{
		#ifdef DBG_DATALOG
		printf("\n\rProcess Not Present");
		#endif
		pstLog->cProcess[0] = '\0';
	}
}
/*****************************************************************************
**@Function 		: fnDatalogVersionFrame
**@Descriptions	: Function write datalog Version in XML format
**@Parameters		: None   
**@Return				: None      
*****************************************************************************/
void fnDatalogVersionFrame(void)
{
	uint8_t rgucTempData[100];
	fnWriteStartElement("Record internalName=\"Version\"");
  
  /*SCU Firmware Version*/
  memset(rgucTempData,'\0',100);
  sprintf((char *)rgucTempData,"%.2f ",SCU_FIRMWARE_VERSION);
  fnWriteData(rgucTempData);
  
  /*HCM Firmware Version*/
  memset(rgucTempData,'\0',100);
  sprintf((char *)rgucTempData,"%.2f ",stCurrentData.fCurr_HCM_Firmware_Ver);
  fnWriteData(rgucTempData);
  
  /*VCM Firmware Version*/
  memset(rgucTempData,'\0',100);
  sprintf((char *)rgucTempData,"%.2f ",stCurrentData.fCurr_VCM_Firmware_Ver);
  fnWriteData(rgucTempData);
  
  /*HMI Firmware Version*/
  memset(rgucTempData,'\0',100);
  sprintf((char *)rgucTempData,"%.2f ",stCurrentData.fCurr_HMI_Firmware_Ver);
  fnWriteData(rgucTempData);
  
  /*OPMM Firmware Version*/
  memset(rgucTempData,'\0',100);
  sprintf((char *)rgucTempData,"%.2f ",stCurrentData.fCurr_OPMM_Firmware_Ver);
  fnWriteData(rgucTempData);
  
  fnWriteEndElement("Record");
}
/*****************************************************************************
**@Function 		: fnDatalogStartFrame
**@Descriptions	: Function write datalog start time stamp in XML format
**@Parameters		: None   
**@Return				: None      
*****************************************************************************/
void fnDatalogStartFrame(DATALOG_TX_t *pstLog)
{
  uint8_t rgucTempData[100];
  //	uint8_t rgucDetailData[22];
  /*updatint the Date and Time from RTC registers*/
  RTC_GetTime(RTC_Format_BIN, &stRTC_TimeStructure);
  RTC_GetDate(RTC_Format_BIN, &stRTC_DateStructure);
  
  
  fnWriteStartElement("Record internalName=\"Start\"");
  sprintf((char *)rgucTempData,"20%02d/%02d/%02d %02d:%02d:%02d ", stRTC_DateStructure.RTC_Year, 
          stRTC_DateStructure.RTC_Month, stRTC_DateStructure.RTC_Date, stRTC_TimeStructure.RTC_Hours, 
          stRTC_TimeStructure.RTC_Minutes, stRTC_TimeStructure.RTC_Seconds);
  fnWriteData(rgucTempData);
  
	/*Send Job Name*/
	fnRetriveJobName(pstLog->ucStoredJobName);	
	memset(rgucTempData,'\0',100);
	sprintf((char *)rgucTempData,"%s ",(char *)jobname);
	fnWriteData(rgucTempData);

	if(STATUS_LOG == pstLog->ucLogType)
	{
		if(255 != pstLog->ucGetJobID)
		{
			/*Send Job ID*/
			fnRetriveJobID(pstLog->ucGetJobID);	
			memset(rgucTempData,'\0',100);
			sprintf((char *)rgucTempData,"%s ",(char *)jobid);
			fnWriteData(rgucTempData);
		}
	}
	
	/*Send Coating Machine Name*/
	memset(rgucTempData,'\0',100);
	sprintf((char *)rgucTempData,"%s ",(char *)stWifiIP.rgucSSID);
	fnWriteData(rgucTempData);
	
		/*Send LogType*/
	memset(rgucTempData,'\0',100);
	switch(pstLog->ucLogType)
	{
		case PERIODIC_LOG:
			sprintf((char *)rgucTempData,"%s ","Periodic_Log");
			fnWriteData(rgucTempData);
		break;
		case DIAGNOSTIC_LOG:
			sprintf((char *)rgucTempData,"%s ","Diagnostic_Log");
			fnWriteData(rgucTempData);
		break;
		case FAULT_LOG:
			sprintf((char *)rgucTempData,"%s ","Fault_Log");
			fnWriteData(rgucTempData);
		break;
		case STATUS_LOG:
			sprintf((char *)rgucTempData,"%s ","Status_Log");
			fnWriteData(rgucTempData);
		break;
	}
  
  fnWriteEndElement("Record");
				
	fnDatalogVersionFrame();
}
/*****************************************************************************
**@Function 		: fnDatalogStartFrame
**@Descriptions	: Function write datalog start time stamp in XML format
**@Parameters		: None   
**@Return				: None      
*****************************************************************************/
void fnDatalogPRDStartFrame(DATALOG_TX_t *pstLog)
{
  uint8_t rgucTempData[100];
  //	uint8_t rgucDetailData[22];
  /*updatint the Date and Time from RTC registers*/
  RTC_GetTime(RTC_Format_BIN, &stRTC_TimeStructure);
  RTC_GetDate(RTC_Format_BIN, &stRTC_DateStructure);
  
  
  fnWriteStartElement("Record internalName=\"Start\"");
  sprintf((char *)rgucTempData,"20%02d/%02d/%02d %02d:%02d:%02d ", stRTC_DateStructure.RTC_Year, 
          stRTC_DateStructure.RTC_Month, stRTC_DateStructure.RTC_Date, stRTC_TimeStructure.RTC_Hours, 
          stRTC_TimeStructure.RTC_Minutes, stRTC_TimeStructure.RTC_Seconds);
  fnWriteData(rgucTempData);
  
	/*Send Job Name*/
	fnRetriveJobName(pstLog->ucGetJobName);	
	memset(rgucTempData,'\0',100);
	sprintf((char *)rgucTempData,"%s ",(char *)jobname);
	fnWriteData(rgucTempData);

	if(255 != pstLog->ucGetJobID)
	{
		/*Send Job ID*/
	fnRetriveJobID(pstLog->ucGetJobID);	
	memset(rgucTempData,'\0',100);
	sprintf((char *)rgucTempData,"%s ",(char *)jobid);
	fnWriteData(rgucTempData);
	}
	
	
	if(255 != pstLog->ucGetProcessID)
	{
		/*Send Process ID*/
    if(pstLog->ucGetProcessID & 0x01)
    {
      fnWriteData("SPRAY ");
    }
    else if(pstLog->ucGetProcessID & 0x02)
    {
      fnWriteData("STALL ");
    }
    else
    {
      fnWriteData("IDLE ");
    }
	}
	
	/*Send Coating Machine Name*/
	memset(rgucTempData,'\0',100);
	sprintf((char *)rgucTempData,"%s ",(char *)stWifiIP.rgucSSID);
	fnWriteData(rgucTempData);
	
		/*Send LogType*/
	memset(rgucTempData,'\0',100);
	sprintf((char *)rgucTempData,"%s ","Periodic_Log");
	fnWriteData(rgucTempData);
  
  fnWriteEndElement("Record");
				
	fnDatalogVersionFrame();
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
  switch(*ucNextData)
  {
  case 1:
    {
      
      fnWriteStartElement("CRC-Evans_Data");
      fnWriteStartElement("File_Identity displayName=\"Diagnostic Log\"");
      fnWriteData("Diagnostic Log");
      fnWriteEndElement("File_Identity");
      fnWriteStartElement("View");
      
      fnWriteStartElement("Record internalName=\"Start\" displayName=\"Start\"");
      fnWriteStartElement("Variable internalName=\"StartDate\" displayName=\"Date\" type=\"Date\" /");
      fnWriteStartElement("Variable internalName=\"StartTime\" displayName=\"Time\" type=\"Time\" /");
			fnWriteStartElement("Variable internalName=\"JobName\" displayName=\"Job Name\" type=\"string\" /");
      //fnWriteStartElement("Variable internalName=\"JointID\" displayName=\"JointID\" type=\"string\" /");//ADDED S1711
			fnWriteStartElement("Variable internalName=\"CoatingMachineName\" displayName=\"CoatingMachineName\" type=\"string\" /");
			fnWriteStartElement("Variable internalName=\"LogType\" displayName=\"LogType\" type=\"string\" /");
      fnWriteEndElement("Record");
			fnWriteStartElement("Record internalName=\"Version\" displayName=\"Firmware Version\"");
      fnWriteStartElement("Variable internalName=\"SCU_Firmware_Version\" displayName=\"SCU Firmware Version\" type=\"string\"/");
      fnWriteStartElement("Variable internalName=\"HCM_Firmware_Version\" displayName=\"HCM Firmware Version\" type=\"string\"/");
      fnWriteStartElement("Variable internalName=\"VCM_Firmware_Version\" displayName=\"VCM Firmware Version\" type=\"string\"/");
      fnWriteStartElement("Variable internalName=\"HMI_Firmware_Version\" displayName=\"HMI Firmware Version\" type=\"string\"/");
      fnWriteStartElement("Variable internalName=\"OPMM_Firmware_Version\" displayName=\"OPMM Firmware Version\" type=\"string\"/");
      fnWriteEndElement("Record");
      fnWriteStartElement("Record internalName=\"Measurement\" displayName=\"Measurement\"");
      *ucNextData = *ucNextData + 1;
    }
    break;
  case 2:
    {
      fnWriteStartElement("Variable internalName=\"MeasurementStartDate\" displayName=\"Date\" type=\"Date\" /");
      fnWriteStartElement("Variable internalName=\"MeasurementStartTime\" displayName=\"Time\" type=\"Time\" /");
			fnWriteStartElement("Variable internalName=\"SupervisorName\" displayName=\"Supervisor Name\" type=\"string\" /");
			fnWriteStartElement("Variable internalName=\"JointID\" displayName=\"Joint ID\" type=\"string\" /");//ADDED S1711
      fnWriteStartElement("Variable internalName=\"SOMDiag\" displayName=\"SOM Diag\" type=\"string\" /");
      fnWriteStartElement("Variable internalName=\"SCU_EEPROM\" displayName=\"SCU EEPROM\" type=\"string\" /");
      fnWriteStartElement("Variable internalName=\"SCU_NVSRAM1\" displayName=\"SCU NVSRAM 1\" type=\"string\" /");/*Change 54*/
			fnWriteStartElement("Variable internalName=\"SCU_NVSRAM2\" displayName=\"SCU NVSRAM 2\" type=\"string\" /");/*Change 54*/
      fnWriteStartElement("Variable internalName=\"PressureRegulator\" displayName=\"Pressure Regulator\" type=\"string\" /");
      *ucNextData = *ucNextData + 1;
    }
    break;
  case 3:
    {
      fnWriteStartElement("Variable internalName=\"HMIDiag\" displayName=\"HMI Diag\" type=\"string\" /");
      fnWriteStartElement("Variable internalName=\"HMI_NVSRAM\" displayName=\"HMI NVSRAM\" type=\"string\" /");
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
      fnWriteStartElement("Variable internalName=\"StallValveConn\" displayName=\"Stall Valve Connected\" type=\"string\" /");
      fnWriteStartElement("Variable internalName=\"StallValve\" displayName=\"Stall Valve\" type=\"string\" /");
			fnWriteStartElement("Variable internalName=\"RecirculationValveConn\" displayName=\"Recirculation Valve Connected\" type=\"string\" /");/*Change 54*/
      fnWriteStartElement("Variable internalName=\"RecirculationValve\" displayName=\"Recirculation Valve\" type=\"string\" /");/*Change 54*/
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
  
  fnWriteStartElement("Record internalName=\"Measurement\"");
  memset(rgucTimeStamp,'\0',6); 
  memset(rgucTempData,'\0',100); 
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
	fnRetriveSupervisorName(ucReadData);
	sprintf((char *)rgucTempData,"%s ",supervisorname);
	fnWriteData(rgucTempData);
	
#ifdef DBG_DIAG_WARN_LOGIC
  sprintf((char *)rgucTempData,"%d ",ucReadData);
  fnWriteData(rgucTempData);
#endif
  
  /*ucJobID; change49*/
  ucReadData = fnNVSRAM1Byte_Read(uliStartAddres++);
	/*Send Job ID*///ADDED S1711
	fnRetriveJobID(ucReadData);	
	memset(rgucTempData,'\0',100);
	sprintf((char *)rgucTempData,"%s ",(char *)jobid);
	fnWriteData(rgucTempData);
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
	/*change54*/
  ((ucReadData >> 6) & 0x01)? fnWriteData("FAIL ") : fnWriteData("PASS ");
  ((ucReadData >> 7) & 0x01)? fnWriteData("FAIL ") : fnWriteData("PASS ");
  
		ucReadData = fnNVSRAM1Byte_Read(uliStartAddres++);
  ((ucReadData >> 0) & 0x01)? fnWriteData("FAIL ") : fnWriteData("PASS ");
	
	
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
  memset(rgucTimeStamp,'\0',6); 
  DateTime_t stLogDateTime;
  /*Time Stamp read */
  fnNVSRAM1_MultiDataRead(6, pstLog->uliStartAddress, rgucTimeStamp);		
  
  stLogDateTime.ucYear = rgucTimeStamp[BYTE_ZERO];
  stLogDateTime.ucMonth = rgucTimeStamp[BYTE_ONE];
  stLogDateTime.ucDate = rgucTimeStamp[BYTE_TWO];
  stLogDateTime.ucHour = rgucTimeStamp[BYTE_THREE];
  stLogDateTime.ucMin = rgucTimeStamp[BYTE_FOUR];
  stLogDateTime.ucSec = rgucTimeStamp[BYTE_FIVE];
	
	pstLog->uliStartAddress += 6;
      
  /*ucStoredJobName change23*/
  pstLog->ucStoredJobName  = fnNVSRAM1Byte_Read(pstLog->uliStartAddress++);
  pstLog->uliStartAddress++;
  /*ucStoredJobID; change23*/
  pstLog->ucStoredJobID  = fnNVSRAM1Byte_Read(pstLog->uliStartAddress++);
  pstLog->uliStartAddress -= 9;
	
  if((pstLog->stStartLogDateTime.uliTotalSec <= fnGetDateTimeInSec(stLogDateTime)) &&
     (pstLog->stEndLogDateTime.uliTotalSec >= fnGetDateTimeInSec(stLogDateTime)) &&
		 (pstLog->ucStoredJobName == pstLog->ucGetJobName))
  {
    if(RESET == pstLog->ucStartLogTimeFlag)
    {
      fnDatalogStartFrame(pstLog);
      pstLog->ucStartLogTimeFlag = SET;
			pstLog->ucStopLogTimeFlag = SET;
			fnDiagLog_Read(pstLog->uliStartAddress);
      pstLog->uliStartAddress += DL_DIAG_PACKET_SIZE;
    }
    else
    {
        fnDiagLog_Read(pstLog->uliStartAddress);
        pstLog->uliStartAddress += DL_DIAG_PACKET_SIZE;
    }
  }
  else
  {
    pstLog->uliStartAddress += DL_DIAG_PACKET_SIZE;
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
      fnDiagLog_Detail(&pstLog->ucDetailFlag);
      pstLog->ucStartLogTimeFlag = RESET;
    }
    else if(pstLog->uliStartAddress < pstLog->uliStopAddress)
    {
      fnDiagnosticLogScan(pstLog);
    }
    else
    {
      if(pstLog->ucStopLogTimeFlag == SET)
			{
				pstLog->ucStopLogTimeFlag = RESET;
				fnDatalogEndFrame();
			}
      fnWriteEndElement("Data");
      fnWriteEndElement("CRC-Evans_Data");

			char rgucTempBuff[100]={'\0'};
      memset(rgucTempBuff,'\0',100);
      sprintf((char *)rgucTempBuff,"%02x%02x%02x%02x%02x%02x%02x%02x",0x00,
              0x00,0x00,0x00,0x00,0x00,0x00,0x00);
      fnWriteStartElement("CRC-Evans_MCL");
      fnWriteStartElement("Ext_ID");
      fnWriteMessageID((uint32_t)(0x16000000 | 0x00E80000 | DEST_TABLET | SOURCE_SCU));
      fnWriteEndElement("Ext_ID");
      fnWriteStartElement("DLC");
      fnWriteLength((uint8_t)(1));
      fnWriteEndElement("DLC");
      fnWriteStartElement("Data");			
      fnWriteData((uint8_t *)rgucTempBuff);
      fnWriteEndElement("Data");
      fnWriteEndElement("CRC-Evans_MCL");
			
      pstLog->ucLogType = RESET;
      pstLog->ucSendFlag = RESET;
      /*change33*/
      stWarningLog.ucWarnLog_DiagSendWarnCounter = RESET;
    }
  }
}
/*****************************************************************************
**@Function			: fnPRDLog_Detail
**@Descriptions	: Append Periodic log info in XML format 
**@parameters		: None
**@return				: None      
*******************************************************************************/
void fnPRDLog_Detail(DATALOG_TX_t *pstLog)
{
  switch(pstLog->ucDetailFlag)
  {
  case 1:
    {
      
      fnWriteStartElement("CRC-Evans_Data");
      fnWriteStartElement("File_Identity displayName=\"Periodic Log\"");
      fnWriteData("Periodic Log");
      fnWriteEndElement("File_Identity");
      fnWriteStartElement("View");	

      
      fnWriteStartElement("Record internalName=\"Start\" displayName=\"Start\"");
      fnWriteStartElement("Variable internalName=\"StartDate\" displayName=\"Date\" type=\"Date\" /");
      fnWriteStartElement("Variable internalName=\"StartTime\" displayName=\"Time\" type=\"Time\" /");
      pstLog->ucDetailFlag = pstLog->ucDetailFlag + 1;
    }
    break;
  case 2:
    {
			fnWriteStartElement("Variable internalName=\"JobName\" displayName=\"Job Name\" type=\"string\" /");
			if(255 != pstLog->ucGetJobID)
				fnWriteStartElement("Variable internalName=\"JointID\" displayName=\"JointID\" type=\"string\" /");
			if(255 != pstLog->ucGetProcessID)
				fnWriteStartElement("Variable internalName=\"Process\" displayName=\"Process\" type=\"string\" /");
			fnWriteStartElement("Variable internalName=\"CoatingMachineName\" displayName=\"CoatingMachineName\" type=\"string\" /");
			fnWriteStartElement("Variable internalName=\"LogType\" displayName=\"LogType\" type=\"string\" /");
      fnWriteEndElement("Record");
			      
      fnWriteStartElement("Record internalName=\"Version\" displayName=\"Firmware Version\"");
      fnWriteStartElement("Variable internalName=\"SCU_Firmware_Version\" displayName=\"SCU Firmware Version\" type=\"string\"/");
      fnWriteStartElement("Variable internalName=\"HCM_Firmware_Version\" displayName=\"HCM Firmware Version\" type=\"string\"/");
      fnWriteStartElement("Variable internalName=\"VCM_Firmware_Version\" displayName=\"VCM Firmware Version\" type=\"string\"/");
      fnWriteStartElement("Variable internalName=\"HMI_Firmware_Version\" displayName=\"HMI Firmware Version\" type=\"string\"/");
      fnWriteStartElement("Variable internalName=\"OPMM_Firmware_Version\" displayName=\"OPMM Firmware Version\" type=\"string\"/");
      fnWriteEndElement("Record");
      pstLog->ucDetailFlag = pstLog->ucDetailFlag + 1;
    }
    break;
  case 3:
    {
      fnWriteStartElement("Record internalName=\"Measurement\" displayName=\"Measurement\"");
      fnWriteStartElement("Variable internalName=\"MeasurementStartDate\" displayName=\"Date\" type=\"Date\" /");
      fnWriteStartElement("Variable internalName=\"MeasurementStartTime\" displayName=\"Time\" type=\"Time\" /");
			fnWriteStartElement("Variable internalName=\"SupervisorName\" displayName=\"Supervisor Name\" type=\"string\" /");
			if(255 == pstLog->ucGetJobID)
				fnWriteStartElement("Variable internalName=\"JointID\" displayName=\"JointID\" type=\"string\" /");
			if(255 == pstLog->ucGetProcessID)
				fnWriteStartElement("Variable internalName=\"Process\" displayName=\"Process\" type=\"string\" /");
      //fnWriteStartElement("Variable internalName=\"JointNum\" displayName=\"Joint Number\" type=\"decimal\" /");/*Change 54*/
      fnWriteStartElement("Variable internalName=\"AntifreezTemp\" displayName=\"Antifreez Temp\" type=\"float\" units=\"celsius\" /");
      pstLog->ucDetailFlag = pstLog->ucDetailFlag + 1;
    }
    break;
  case 4:
    {
      fnWriteStartElement("Variable internalName=\"BaseTemp\" displayName=\"Base Temp\" type=\"float\" units=\"celsius\" /");
      fnWriteStartElement("Variable internalName=\"HardTemp\" displayName=\"Hard Temp\" type=\"float\" units=\"celsius\" /");
      fnWriteStartElement("Variable internalName=\"OPMMBaseTemp\" displayName=\"Base Temp\" type=\"float\" units=\"celsius\" /");
      fnWriteStartElement("Variable internalName=\"OPMMHardTemp\" displayName=\"Hard Temp\" type=\"float\" units=\"celsius\" /");
      pstLog->ucDetailFlag = pstLog->ucDetailFlag + 1;
    }
    break;
  case 5:
    {
      fnWriteStartElement("Variable internalName=\"AmbientTempInContainer\" displayName=\"Ambient Temp Inside container\" type=\"float\" units=\"celsius\" /");
      fnWriteStartElement("Variable internalName=\"AmbientTempOutContainer\" displayName=\"Ambient Temp Outside container\" type=\"float\" units=\"celsius\" /");
      fnWriteStartElement("Variable internalName=\"ReturnLineTemp\" displayName=\"Return line temperature (MCU)\" type=\"float\" units=\"celcius\" /");
      fnWriteStartElement("Variable internalName=\"Humidity\" displayName=\"Humidity\" type=\"float\" units=\"Percent\" /");
      pstLog->ucDetailFlag = pstLog->ucDetailFlag + 1;
    }
    break;
  case 6:
    {
      fnWriteStartElement("Variable internalName=\"DewPoint\" displayName=\"Dew Point\" type=\"float\" units=\"celsius\" /");
      fnWriteStartElement("Variable internalName=\"BaseMaterialFillLevel\" displayName=\"Base material  Fill level\" type=\"float\" units=\"mm\" /");
      fnWriteStartElement("Variable internalName=\"HardMaterialFillLevel\" displayName=\"Hard material  Fill level\" type=\"float\" units=\"mm\" /");	
      fnWriteStartElement("Variable internalName=\"PressureGauge1\" displayName=\"Pressure gauge 1\" type=\"float\" units=\"b\" /");
      pstLog->ucDetailFlag = pstLog->ucDetailFlag + 1;
    }
    break;
  case 7:
    {
      fnWriteStartElement("Variable internalName=\"PressureGauge2\" displayName=\"Pressure gauge 2\" type=\"float\" units=\"b\" /");
      fnWriteStartElement("Variable internalName=\"PressureGauge3\" displayName=\"Pressure gauge 3\" type=\"float\" units=\"b\" /");
      fnWriteEndElement("Record");
      fnWriteStartElement("Record internalName=\"End\" displayName=\"End\"");
      fnWriteStartElement("Variable internalName=\"EndDate\" displayName=\"Date\" type=\"Date\" /");
      fnWriteStartElement("Variable internalName=\"EndTime\" displayName=\"Time\" type=\"Time\" /");
      fnWriteEndElement("Record");
      fnWriteEndElement("View");
      fnWriteStartElement("Data");
      pstLog->ucDetailFlag = pstLog->ucDetailFlag + 1;
    }
    break;
  default:
    {
      pstLog->ucDetailFlag = BYTE_ZERO;
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
void fnPRDLog_Read(DATALOG_TX_t *pstLog)
{
  uint8_t rgucTimeStamp[20];
  //char rgucTempData[150];
  uint8_t rgucTempData[100];
  uint8_t ucReadData ;  //change15
  
  fnWriteStartElement("Record internalName=\"Measurement\"");
  
  if(pstLog->uliStartAddress & PRDADDR_MASK)/*NVSRAM2 Address*/
  {
		//printf("\n\rNVSRAM 2");
    /*Time Stamp read and Display on UART*/
    fnNVSRAM2_MultiDataRead(6, pstLog->uliStartAddress, rgucTimeStamp);		
    
    sprintf((char *)rgucTempData,"20%02d/%02d/%02d %02d:%02d:%02d ", rgucTimeStamp[BYTE_ZERO],
            rgucTimeStamp[BYTE_ONE], rgucTimeStamp[BYTE_TWO], rgucTimeStamp[BYTE_THREE],
            rgucTimeStamp[BYTE_FOUR], rgucTimeStamp[BYTE_FIVE]);
    pstLog->uliStartAddress += 6;
    fnWriteData(rgucTempData);
		
    /*change23*/
    ucReadData = fnNVSRAM2Byte_Read(pstLog->uliStartAddress++);
#ifdef DBG_PERIODIC_WARN_LOGIC
//    sprintf((char *)rgucTempData,"%d ",ucReadData);
//    fnWriteData(rgucTempData);
#endif
	fnDelay_ms(100);
    /*change23*/
    ucReadData = fnNVSRAM2Byte_Read(pstLog->uliStartAddress++);
    fnRetriveSupervisorName(ucReadData);
		sprintf((char *)rgucTempData,"%s ",supervisorname);
		fnWriteData(rgucTempData);
#ifdef DBG_PERIODIC_WARN_LOGIC
//    sprintf((char *)rgucTempData,"%d ",ucReadData);
//    fnWriteData(rgucTempData);
#endif

     /*change49*/
    ucReadData = fnNVSRAM2Byte_Read(pstLog->uliStartAddress++);
		//printf("Joint ID = %d\n\r",ucReadData);
    if(255 == pstLog->ucGetJobID)
		{
				/*Send Job ID*/
				fnRetriveJobID(pstLog->ucStoredJobID);	
				memset(rgucTempData,'\0',100);
				sprintf((char *)rgucTempData,"%s ",(char *)jobid);
				fnWriteData(rgucTempData);	
		}
		
				
#ifdef DBG_PERIODIC_WARN_LOGIC
    sprintf((char *)rgucTempData,"%d ",ucReadData);
    fnWriteData(rgucTempData);
#endif
 
 /*change60*/
    /*SPRAY OR STALL OR IDLE Data Byte 1 change15*/
    ucReadData = fnNVSRAM2Byte_Read(pstLog->uliStartAddress++);
		//printf("Process ID = %d\n\r",ucReadData);
				if(255 == pstLog->ucGetProcessID)
				{
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
    /*Joint Number Read*/
    fnNVSRAM2_MultiDataRead (4, pstLog->uliStartAddress, SplitByte.rgucData);
#ifdef DBG_PRD_READ
    printf(" Joint Number Read = %f \n\r", SplitByte.fData);
#endif
//    sprintf((char *)rgucTempData,"%0.0f ", SplitByte.fData);/*Change 54*/
//    fnWriteData(rgucTempData);/*Change 54*/
    pstLog->uliStartAddress += 4;
    
    /*Antifreez Liquid Temp Read*/
    fnNVSRAM2_MultiDataRead (4, pstLog->uliStartAddress, SplitByte.rgucData);
#ifdef DBG_PRD_READ
    printf(" Antifreez Liquid Temp Read = %f \n\r", SplitByte.fData);
#endif
    if((INVALID_FLOAT_READ == SplitByte.fData) || (NaN == SplitByte.uliData))
    {
      fnWriteData("ERROR ");
    }
    else
    {
      sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
      fnWriteData(rgucTempData);
    }
    pstLog->uliStartAddress += 4;
    
    /*Base Material Temp Read*/
    fnNVSRAM2_MultiDataRead (4, pstLog->uliStartAddress, SplitByte.rgucData);
#ifdef DBG_PRD_READ
    printf(" Base Material Temp Read= %f \n\r", SplitByte.fData);
#endif
    if((INVALID_FLOAT_READ == SplitByte.fData) || (NaN == SplitByte.uliData))
    {
      fnWriteData("ERROR ");
    }
    else
    {
      sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
      fnWriteData(rgucTempData);
    }
    pstLog->uliStartAddress += 4;
    
    /*Hardner Material Temp Read*/
    fnNVSRAM2_MultiDataRead (4, pstLog->uliStartAddress, SplitByte.rgucData);
#ifdef DBG_PRD_READ
    printf(" Hardner Material Temp Read = %f \n\r", SplitByte.fData);
#endif
    if((INVALID_FLOAT_READ == SplitByte.fData) || (NaN == SplitByte.uliData))
    {
      fnWriteData("ERROR ");
    }
    else
    {
      sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
      fnWriteData(rgucTempData);
    }
    pstLog->uliStartAddress += 4;
    
    /*OPMM Base Temp Read*/
    fnNVSRAM2_MultiDataRead (4, pstLog->uliStartAddress, SplitByte.rgucData);
#ifdef DBG_PRD_READ
    printf(" OPMM Base Temp Read = %.2f \n\r", SplitByte.fData);
#endif
    if((INVALID_FLOAT_READ == SplitByte.fData) || (NaN == SplitByte.uliData))
    {
      fnWriteData("ERROR ");
    }
    else
    {
      sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
      fnWriteData(rgucTempData);
    }
    pstLog->uliStartAddress += 4;
    
    /*OPMM Hard Temp Read*/
    fnNVSRAM2_MultiDataRead (4, pstLog->uliStartAddress, SplitByte.rgucData);
#ifdef DBG_PRD_READ
    printf(" OPMM Hard Temp Read = %.2f \n\r", SplitByte.fData);
#endif
    if((INVALID_FLOAT_READ == SplitByte.fData) || (NaN == SplitByte.uliData))
    {
      fnWriteData("ERROR ");
    }
    else
    {
      sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
      fnWriteData(rgucTempData);
    }
    pstLog->uliStartAddress += 4;
    
    /*Inside container Temp Read*/
    fnNVSRAM2_MultiDataRead (4, pstLog->uliStartAddress, SplitByte.rgucData);
#ifdef DBG_PRD_READ
    printf(" Inside container Temp Read = %f \n\r", SplitByte.fData);
#endif
    if((INVALID_FLOAT_READ == SplitByte.fData) || (NaN == SplitByte.uliData))
    {
      fnWriteData("ERROR ");
    }
    else
    {
      sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
      fnWriteData(rgucTempData);
    }
    pstLog->uliStartAddress += 4;
    
    /*Outside container Temp Read*/
    fnNVSRAM2_MultiDataRead (4, pstLog->uliStartAddress, SplitByte.rgucData);
#ifdef DBG_PRD_READ
    printf(" Outside container Temp Read = %f \n\r", SplitByte.fData);
#endif
    if((INVALID_FLOAT_READ == SplitByte.fData) || (NaN == SplitByte.uliData))
    {
      fnWriteData("ERROR ");
    }
    else
    {
      sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
      fnWriteData(rgucTempData);
    }
    pstLog->uliStartAddress += 4;
    
    /*Base Returnline Temp Read SCU*/
    fnNVSRAM2_MultiDataRead (4, pstLog->uliStartAddress, SplitByte.rgucData);
#ifdef DBG_PRD_READ
    printf(" Base Returnline Temp Read SCU = %f \n\r", SplitByte.fData);
#endif
    if((INVALID_FLOAT_READ == SplitByte.fData) || (NaN == SplitByte.uliData))
    {
      fnWriteData("ERROR ");
    }
    else
    {
      sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
      fnWriteData(rgucTempData);
    }
    pstLog->uliStartAddress += 4;
    
    /*Humidity Read*/
    fnNVSRAM2_MultiDataRead (4, pstLog->uliStartAddress, SplitByte.rgucData);
#ifdef DBG_PRD_READ
    printf(" Humidity Read = %f \n\r", SplitByte.fData);
#endif
    if((INVALID_FLOAT_READ == SplitByte.fData) || (NaN == SplitByte.uliData))
    {
      fnWriteData("ERROR ");
    }
    else
    {
      sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
      fnWriteData(rgucTempData);
    }
    pstLog->uliStartAddress += 4;
    
    /*Dew Point Read*/
    fnNVSRAM2_MultiDataRead (4, pstLog->uliStartAddress, SplitByte.rgucData);
#ifdef DBG_PRD_READ
    printf(" Dew Point Read = %f \n\r", SplitByte.fData);
#endif
    if((INVALID_FLOAT_READ == SplitByte.fData) || (NaN == SplitByte.uliData))
    {
      fnWriteData("ERROR ");
    }
    else
    {
      sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
      fnWriteData(rgucTempData);
    }
    pstLog->uliStartAddress += 4;
    
    /*Base Material Fill Level Read*/
    fnNVSRAM2_MultiDataRead (4, pstLog->uliStartAddress, SplitByte.rgucData);
#ifdef DBG_PRD_READ
    printf(" Base Material Fill Level Read = %f \n\r", SplitByte.fData);
#endif
    if((INVALID_FLOAT_READ == SplitByte.fData) || (NaN == SplitByte.uliData))
    {
      fnWriteData("ERROR ");
    }
    else
    {
      sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
      fnWriteData(rgucTempData);
    }
    pstLog->uliStartAddress += 4;
    
    
    /*Hard Material Fill Level Read*/
    fnNVSRAM2_MultiDataRead (4, pstLog->uliStartAddress, SplitByte.rgucData);
#ifdef DBG_PRD_READ
    printf(" HHard Material Fill Level Read = %f \n\r", SplitByte.fData);
#endif
    if((INVALID_FLOAT_READ == SplitByte.fData) || (NaN == SplitByte.uliData))
    {
      fnWriteData("ERROR ");
    }
    else
    {
      sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
      fnWriteData(rgucTempData);
    }
    pstLog->uliStartAddress += 4;
    
    /*PG1 Read*/
    fnNVSRAM2_MultiDataRead (4, pstLog->uliStartAddress, SplitByte.rgucData);
#ifdef DBG_PRD_READ
    printf(" PG1 Read = %f \n\r", SplitByte.fData);
#endif
    if((INVALID_FLOAT_READ == SplitByte.fData) || (NaN == SplitByte.uliData))
    {
      fnWriteData("ERROR ");
    }
    else
    {
      sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
      fnWriteData(rgucTempData);
    }
    pstLog->uliStartAddress += 4;
    
    /*PG2 Read*/
    fnNVSRAM2_MultiDataRead (4, pstLog->uliStartAddress, SplitByte.rgucData);
#ifdef DBG_PRD_READ
    printf(" PG2 Read = %f \n\r", SplitByte.fData);
#endif
    if((INVALID_FLOAT_READ == SplitByte.fData) || (NaN == SplitByte.uliData))
    {
      fnWriteData("ERROR ");
    }
    else
    {
      sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
      fnWriteData(rgucTempData);
    }
    pstLog->uliStartAddress += 4;
    
    /*PG3 Read*/
    fnNVSRAM2_MultiDataRead (4, pstLog->uliStartAddress, SplitByte.rgucData);
#ifdef DBG_PRD_READ
    printf(" PG3 Read= %f \n\r", SplitByte.fData);
#endif
    if((INVALID_FLOAT_READ == SplitByte.fData) || (NaN == SplitByte.uliData))
    {
      fnWriteData("ERROR ");
    }
    else
    {
      sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
      fnWriteData(rgucTempData);
    }

    pstLog->uliStartAddress += 4;	

    
  }		
  else /* NVSRAM1 Address*/
  {
		//printf("\n\rNVSRAM 1");
    /*Time Stamp read and Display on UART*/
    fnNVSRAM1_MultiDataRead(6, pstLog->uliStartAddress, rgucTimeStamp);		
    
    sprintf((char *)rgucTempData,"20%02d/%02d/%02d %02d:%02d:%02d ", rgucTimeStamp[BYTE_ZERO], 
            rgucTimeStamp[BYTE_ONE], rgucTimeStamp[BYTE_TWO], rgucTimeStamp[BYTE_THREE],
            rgucTimeStamp[BYTE_FOUR], rgucTimeStamp[BYTE_FIVE]);
    pstLog->uliStartAddress += 6;
    fnWriteData(rgucTempData);
		
    /*change23*/
    ucReadData = fnNVSRAM1Byte_Read(pstLog->uliStartAddress++);
#ifdef DBG_PERIODIC_WARN_LOGIC
//    sprintf((char *)rgucTempData,"%d ",ucReadData);
//    fnWriteData(rgucTempData);
#endif
		
    /*change23*/
    ucReadData = fnNVSRAM1Byte_Read(pstLog->uliStartAddress++);
  	fnRetriveSupervisorName(ucReadData);
	  sprintf((char *)rgucTempData,"%s ",supervisorname);
	  fnWriteData(rgucTempData);
#ifdef DBG_PERIODIC_WARN_LOGIC
//    sprintf((char *)rgucTempData,"%d ",ucReadData);
//    fnWriteData(rgucTempData);
#endif
   
   /*change49*/
    ucReadData = fnNVSRAM1Byte_Read(pstLog->uliStartAddress++);
		//printf("Joint ID = %d\n\r",ucReadData);
        if(255 == pstLog->ucGetJobID)
		{
				/*Send Job ID*/
				fnRetriveJobID(pstLog->ucStoredJobID);	
				memset(rgucTempData,'\0',100);
				sprintf((char *)rgucTempData,"%s ",(char *)jobid);
				fnWriteData(rgucTempData);	
		}
		
				
#ifdef DBG_PERIODIC_WARN_LOGIC
    sprintf((char *)rgucTempData,"%d ",ucReadData);
    fnWriteData(rgucTempData);
#endif

 /*change60*/
    /*SPRAY OR STALL OR IDLE Data Byte 1 change15*/
    ucReadData = fnNVSRAM1Byte_Read(pstLog->uliStartAddress++);
		//printf("Process ID = %d\n\r",ucReadData);
				if(255 == pstLog->ucGetProcessID)
				{
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

    /*Joint Number Read*/
    fnNVSRAM1_MultiDataRead (4, pstLog->uliStartAddress, SplitByte.rgucData);
#ifdef DBG_PRD_READ
    printf(" Joint Number Read = %f \n\r", SplitByte.fData );
#endif
//    sprintf((char *)rgucTempData,"%0.0f ", SplitByte.fData);
//    fnWriteData(rgucTempData);
    pstLog->uliStartAddress += 4;
    
    /*Antifreez Liquid Temp Read*/
    fnNVSRAM1_MultiDataRead (4, pstLog->uliStartAddress, SplitByte.rgucData);
#ifdef DBG_PRD_READ
    printf(" Antifreez Liquid Temp Read = %f \n\r", SplitByte.fData);
#endif
    if((INVALID_FLOAT_READ == SplitByte.fData) || (NaN == SplitByte.uliData))
    {
      fnWriteData("ERROR ");
    }
    else
    {
      sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
      fnWriteData(rgucTempData);
    }
    pstLog->uliStartAddress += 4;
    
    /*Base Material Temp Read*/
    fnNVSRAM1_MultiDataRead (4, pstLog->uliStartAddress, SplitByte.rgucData);
#ifdef DBG_PRD_READ
    printf(" Base Material Temp Read = %f \n\r", SplitByte.fData);
#endif
    if((INVALID_FLOAT_READ == SplitByte.fData) || (NaN == SplitByte.uliData))
    {
      fnWriteData("ERROR ");
    }
    else
    {
      sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
      fnWriteData(rgucTempData);
    }
    pstLog->uliStartAddress += 4;
    
    /*Hardner Material Temp Read*/
    fnNVSRAM1_MultiDataRead (4, pstLog->uliStartAddress, SplitByte.rgucData);
#ifdef DBG_PRD_READ
    printf("Hardner Material Temp Read = %f \n\r", SplitByte.fData);
#endif
    if((INVALID_FLOAT_READ == SplitByte.fData) || (NaN == SplitByte.uliData))
    {
      fnWriteData("ERROR ");
    }
    else
    {
      sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
      fnWriteData(rgucTempData);
    }
    pstLog->uliStartAddress += 4;
    
    /*OPMM Base Temp Read*/
    fnNVSRAM1_MultiDataRead (4, pstLog->uliStartAddress, SplitByte.rgucData);
#ifdef DBG_PRD_READ
    printf(" OPMM Base Temp Read = %X\n\r", SplitByte.uliData);
#endif
    if((INVALID_FLOAT_READ == SplitByte.fData) || (NaN == SplitByte.uliData))
    {
      fnWriteData("ERROR ");
    }
    else
    {
      sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
      fnWriteData(rgucTempData);
    }
    pstLog->uliStartAddress += 4;
    
    /*OPMM Hard Temp Read*/
    fnNVSRAM1_MultiDataRead (4, pstLog->uliStartAddress, SplitByte.rgucData);
#ifdef DBG_PRD_READ
    printf(" OPMM Hard Temp Read = %X \n\r", SplitByte.uliData);
#endif
    if((INVALID_FLOAT_READ == SplitByte.fData) || (NaN == SplitByte.uliData))
    {
      fnWriteData("ERROR ");
    }
    else
    {
      sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
      fnWriteData(rgucTempData);
    }
    pstLog->uliStartAddress += 4;
    
    /*Inside container Temp Read*/
    fnNVSRAM1_MultiDataRead (4, pstLog->uliStartAddress, SplitByte.rgucData);
#ifdef DBG_PRD_READ
    printf(" Inside container Temp Read = %f \n\r", SplitByte.fData);
#endif
    if((INVALID_FLOAT_READ == SplitByte.fData) || (NaN == SplitByte.uliData))
    {
      fnWriteData("ERROR ");
    }
    else
    {
      sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
      fnWriteData(rgucTempData);
    }
    pstLog->uliStartAddress += 4;
    
    /*Outside container Temp Read*/
    fnNVSRAM1_MultiDataRead (4, pstLog->uliStartAddress, SplitByte.rgucData);
#ifdef DBG_PRD_READ
    printf(" Outside container Temp Read = %f \n\r", SplitByte.fData);
#endif
    if((INVALID_FLOAT_READ == SplitByte.fData) || (NaN == SplitByte.uliData))
    {
      fnWriteData("ERROR ");
    }
    else
    {
      sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
      fnWriteData(rgucTempData);
    }
    pstLog->uliStartAddress += 4;
    
    /*Base Returnline Temp Read SCU*/
    fnNVSRAM1_MultiDataRead (4, pstLog->uliStartAddress, SplitByte.rgucData);
#ifdef DBG_PRD_READ
    printf(" Base Returnline Temp Read SCU = %f \n\r", SplitByte.fData);
#endif
    if((INVALID_FLOAT_READ == SplitByte.fData) || (NaN == SplitByte.uliData))
    {
      fnWriteData("ERROR ");
    }
    else
    {
      sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
      fnWriteData(rgucTempData);
    }
    pstLog->uliStartAddress += 4;
    
    /*Humidity Read*/
    fnNVSRAM1_MultiDataRead (4, pstLog->uliStartAddress, SplitByte.rgucData);
#ifdef DBG_PRD_READ
    printf(" Humidity Read = %f \n\r", SplitByte.fData);
#endif
    if((INVALID_FLOAT_READ == SplitByte.fData) || (NaN == SplitByte.uliData))
    {
      fnWriteData("ERROR ");
    }
    else
    {
      sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
      fnWriteData(rgucTempData);
    }
    pstLog->uliStartAddress += 4;
    
    /*Dew Point Read*/
    fnNVSRAM1_MultiDataRead (4, pstLog->uliStartAddress, SplitByte.rgucData);
#ifdef DBG_PRD_READ
    printf(" Dew Point Read = %f \n\r", SplitByte.fData);
#endif
    if((INVALID_FLOAT_READ == SplitByte.fData) || (NaN == SplitByte.uliData))
    {
      fnWriteData("ERROR ");
    }
    else
    {
      sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
      fnWriteData(rgucTempData);
    }
    pstLog->uliStartAddress += 4;
    
    /*Base Material Fill Level Read*/
    fnNVSRAM1_MultiDataRead (4, pstLog->uliStartAddress, SplitByte.rgucData);
#ifdef DBG_PRD_READ
    printf(" Base Material Fill Level Read = %f \n\r", SplitByte.fData);
#endif
    if((INVALID_FLOAT_READ == SplitByte.fData) || (NaN == SplitByte.uliData))
    {
      fnWriteData("ERROR ");
    }
    else
    {
      sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
      fnWriteData(rgucTempData);
    }
    pstLog->uliStartAddress += 4;
    
    
    /*Hard Material Fill Level Read*/
    fnNVSRAM1_MultiDataRead (4, pstLog->uliStartAddress, SplitByte.rgucData);
#ifdef DBG_PRD_READ
    printf(" Hard Material Fill Level Read = %f \n\r", SplitByte.fData);
#endif
    if((INVALID_FLOAT_READ == SplitByte.fData) || (NaN == SplitByte.uliData))
    {
      fnWriteData("ERROR ");
    }
    else
    {
      sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
      fnWriteData(rgucTempData);
    }
    pstLog->uliStartAddress += 4;
    
    /*PG1 Read*/
    fnNVSRAM1_MultiDataRead (4, pstLog->uliStartAddress, SplitByte.rgucData);
#ifdef DBG_PRD_READ
    printf(" PG1 Read = %f \n\r", SplitByte.fData);
#endif
    if((INVALID_FLOAT_READ == SplitByte.fData) || (NaN == SplitByte.uliData))
    {
      fnWriteData("ERROR ");
    }
    else
    {
      sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
      fnWriteData(rgucTempData);
    }
    pstLog->uliStartAddress += 4;
    
    /*PG2 Read*/
    fnNVSRAM1_MultiDataRead (4, pstLog->uliStartAddress, SplitByte.rgucData);
#ifdef DBG_PRD_READ
    printf(" PG2 Read = %f \n\r", SplitByte.fData);
#endif
    if((INVALID_FLOAT_READ == SplitByte.fData) || (NaN == SplitByte.uliData))
    {
      fnWriteData("ERROR ");
    }
    else
    {
      sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
      fnWriteData(rgucTempData);
    }
    pstLog->uliStartAddress += 4;
    
    /*PG3 Read*/
    fnNVSRAM1_MultiDataRead (4, pstLog->uliStartAddress, SplitByte.rgucData);
#ifdef DBG_PRD_READ
    printf(" PG3 Read = %f \n\r", SplitByte.fData);
#endif
    if((INVALID_FLOAT_READ == SplitByte.fData) || (NaN == SplitByte.uliData))
    {
      fnWriteData("ERROR ");
    }
    else
    {
      sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
      fnWriteData(rgucTempData);
    }
    pstLog->uliStartAddress += 4;	
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
	  memset(rgucTimeStamp,'\0',6); 
  DateTime_t stLogDateTime;
  uint32_t uliPRDBase_Local; 
		uliPRDBase_Local = pstLog->uliStartAddress ; 
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
	
	pstLog->uliStartAddress += 6;
	if(pstLog->uliStartAddress & PRDADDR_MASK)/*NVSRAM2 Address*/
  {
		/*ucJobNameID change23*/
    pstLog->ucStoredJobName = fnNVSRAM2Byte_Read(pstLog->uliStartAddress++);
    pstLog->uliStartAddress++;
    /*ucSupervisorNameID; change23*/
    pstLog->ucStoredJobID = fnNVSRAM2Byte_Read(pstLog->uliStartAddress++);
		
		pstLog->ucStoredProcessID = fnNVSRAM2Byte_Read(pstLog->uliStartAddress++);
	}
  else
	{
	   /*ucJobNameID change23*/
     pstLog->ucStoredJobName = fnNVSRAM1Byte_Read(pstLog->uliStartAddress++);
			pstLog->uliStartAddress++;
     /*ucSupervisorNameID; change23*/
     pstLog->ucStoredJobID = fnNVSRAM1Byte_Read(pstLog->uliStartAddress++);
		pstLog->ucStoredProcessID = fnNVSRAM1Byte_Read(pstLog->uliStartAddress++);
  }
	pstLog->uliStartAddress -= 10;
  if((pstLog->stStartLogDateTime.uliTotalSec <= fnGetDateTimeInSec(stLogDateTime)) &&
     (pstLog->stEndLogDateTime.uliTotalSec >= fnGetDateTimeInSec(stLogDateTime)) &&
				(255 == pstLog->ucGetProcessID) && (255 == pstLog->ucGetJobID) && 
	   (pstLog->ucStoredJobName == pstLog->ucGetJobName))
  {
    if(RESET == pstLog->ucStartLogTimeFlag)
    {
      fnDatalogPRDStartFrame(pstLog);
      pstLog->ucStartLogTimeFlag = SET;
			pstLog->ucStopLogTimeFlag = SET;
      fnPRDLog_Read(pstLog);
			pstLog->uliStartAddress = uliPRDBase_Local ; 
      pstLog->uliStartAddress += DL_PRD_PACKET_SIZE;
    }
    else
    {
      fnPRDLog_Read(pstLog);
			pstLog->uliStartAddress = uliPRDBase_Local ; 
      pstLog->uliStartAddress += DL_PRD_PACKET_SIZE;
    }
  }
  else if((pstLog->stStartLogDateTime.uliTotalSec <= fnGetDateTimeInSec(stLogDateTime)) &&
     (pstLog->stEndLogDateTime.uliTotalSec >= fnGetDateTimeInSec(stLogDateTime)) &&
	(255 == pstLog->ucGetProcessID) && (255 != pstLog->ucGetJobID)
	&& (pstLog->ucStoredJobID == pstLog->ucGetJobID) && 
	   (pstLog->ucStoredJobName == pstLog->ucGetJobName))
  {
    if(RESET == pstLog->ucStartLogTimeFlag)
    {
      fnDatalogPRDStartFrame(pstLog);
      pstLog->ucStartLogTimeFlag = SET;
			pstLog->ucStopLogTimeFlag = SET;
      fnPRDLog_Read(pstLog);
			pstLog->uliStartAddress = uliPRDBase_Local ; 
      pstLog->uliStartAddress += DL_PRD_PACKET_SIZE;
     }
    else
    {
      fnPRDLog_Read(pstLog);
			pstLog->uliStartAddress = uliPRDBase_Local ; 
      pstLog->uliStartAddress += DL_PRD_PACKET_SIZE;
    }
  }
  else if((pstLog->stStartLogDateTime.uliTotalSec <= fnGetDateTimeInSec(stLogDateTime)) &&
     (pstLog->stEndLogDateTime.uliTotalSec >= fnGetDateTimeInSec(stLogDateTime)) &&
	(255 != pstLog->ucGetProcessID) && (255 == pstLog->ucGetJobID)
	&& (pstLog->ucStoredProcessID == pstLog->ucGetProcessID) && 
	   (pstLog->ucStoredJobName == pstLog->ucGetJobName))
  {
    if(RESET == pstLog->ucStartLogTimeFlag)
    {
      fnDatalogPRDStartFrame(pstLog);
      pstLog->ucStartLogTimeFlag = SET;
			pstLog->ucStopLogTimeFlag = SET;
      fnPRDLog_Read(pstLog);
			pstLog->uliStartAddress = uliPRDBase_Local ; 
      pstLog->uliStartAddress += DL_PRD_PACKET_SIZE;
    }
    else
    {
      fnPRDLog_Read(pstLog);
			pstLog->uliStartAddress = uliPRDBase_Local ; 
      pstLog->uliStartAddress += DL_PRD_PACKET_SIZE;
    }
  }
  else if((pstLog->stStartLogDateTime.uliTotalSec <= fnGetDateTimeInSec(stLogDateTime)) &&
     (pstLog->stEndLogDateTime.uliTotalSec >= fnGetDateTimeInSec(stLogDateTime)) &&
	(255 != pstLog->ucGetProcessID) && (255 != pstLog->ucGetJobID)
	&& (pstLog->ucStoredProcessID == pstLog->ucGetProcessID)
	&& (pstLog->ucStoredJobID == pstLog->ucGetJobID) && 
	   (pstLog->ucStoredJobName == pstLog->ucGetJobName))
  {
    if(RESET == pstLog->ucStartLogTimeFlag)
    {
      fnDatalogPRDStartFrame(pstLog);
      pstLog->ucStartLogTimeFlag = SET;
			pstLog->ucStopLogTimeFlag = SET;
      fnPRDLog_Read(pstLog);
			pstLog->uliStartAddress = uliPRDBase_Local ; 
      pstLog->uliStartAddress += DL_PRD_PACKET_SIZE;
    }
    else
    {
      fnPRDLog_Read(pstLog);
			pstLog->uliStartAddress = uliPRDBase_Local ; 
      pstLog->uliStartAddress += DL_PRD_PACKET_SIZE;
    }
  }
  else
  {
		pstLog->uliStartAddress = uliPRDBase_Local ; 
	pstLog->uliStartAddress += DL_PRD_PACKET_SIZE;
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
      fnPRDLog_Detail(pstLog);
      pstLog->ucStartLogTimeFlag = RESET;
    }
    else if(pstLog->uliStartAddress < pstLog->uliStopAddress)
    {
			fnPeriodicLogScan(pstLog);
     if((0 == pstLog->ucAddrFlag) && (pstLog->uliStartAddress >= DL_PRD_BOUNDARY))
      {
        pstLog->ucAddrFlag = 1;
        pstLog->uliStartAddress = BASE_ADDR_NVSRAM2;
      }

    }
    else
    {
      if(pstLog->ucStopLogTimeFlag == SET)
			{
				pstLog->ucStopLogTimeFlag = RESET;
				fnDatalogEndFrame();
			}
      fnWriteEndElement("Data");
      fnWriteEndElement("CRC-Evans_Data");
			char rgucTempBuff[100]={'\0'};
      memset(rgucTempBuff,'\0',100);
      sprintf((char *)rgucTempBuff,"%02x%02x%02x%02x%02x%02x%02x%02x",0x00,
              0x00,0x00,0x00,0x00,0x00,0x00,0x00);
      fnWriteStartElement("CRC-Evans_MCL");
      fnWriteStartElement("Ext_ID");
      fnWriteMessageID((uint32_t)(0x16000000 | 0x00E80000 | DEST_TABLET | SOURCE_SCU));
      fnWriteEndElement("Ext_ID");
      fnWriteStartElement("DLC");
      fnWriteLength((uint8_t)(1));
      fnWriteEndElement("DLC");
      fnWriteStartElement("Data");			
      fnWriteData((uint8_t *)rgucTempBuff);
      fnWriteEndElement("Data");
      fnWriteEndElement("CRC-Evans_MCL");
      //fnWifiSendData(rgucXMLtxBuffer);
      pstLog->ucLogType = RESET;
      pstLog->ucSendFlag = RESET;
      /*change33*/
      stWarningLog.ucWarnLog_PeriodicSendWarnCounter = RESET;
    }
  }
}

/*****************************************************************************
**@Function			: fnStatusLog_Detail
**@Descriptions	: Append staus log inf in XML format 
**@parameters		: None
**@return				: None      
*******************************************************************************/
void fnStatusLog_Detail(DATALOG_TX_t *pstLog)
{
  switch(pstLog->ucDetailFlag)
  {
  case 1:
    {
      
      fnWriteStartElement("CRC-Evans_Data");
      fnWriteStartElement("File_Identity displayName=\"Status Log\"");
      fnWriteData("Status Log Per Joint");
      fnWriteEndElement("File_Identity");
      fnWriteStartElement("View");

      fnWriteStartElement("Record internalName=\"Start\" displayName=\"Start\"");
      fnWriteStartElement("Variable internalName=\"StartDate\" displayName=\"Date\" type=\"Date\" /");
      fnWriteStartElement("Variable internalName=\"StartTime\" displayName=\"Time\" type=\"Time\" /");
			fnWriteStartElement("Variable internalName=\"JobName\" displayName=\"Job Name\" type=\"string\" /");
			if(255 != pstLog->ucGetJobID)
				fnWriteStartElement("Variable internalName=\"JointID\" displayName=\"JointID\" type=\"string\" /");
			fnWriteStartElement("Variable internalName=\"CoatingMachineName\" displayName=\"CoatingMachineName\" type=\"string\" /");
			fnWriteStartElement("Variable internalName=\"LogType\" displayName=\"LogType\" type=\"string\" /");
      pstLog->ucDetailFlag = pstLog->ucDetailFlag + 1;
    }
    break;
  case 2:
    {
      fnWriteEndElement("Record");
			
			fnWriteStartElement("Record internalName=\"Version\" displayName=\"Firmware Version\"");
      fnWriteStartElement("Variable internalName=\"SCU_Firmware_Version\" displayName=\"SCU Firmware Version\" type=\"string\"/");
      fnWriteStartElement("Variable internalName=\"HCM_Firmware_Version\" displayName=\"HCM Firmware Version\" type=\"string\"/");
      fnWriteStartElement("Variable internalName=\"VCM_Firmware_Version\" displayName=\"VCM Firmware Version\" type=\"string\"/");
      fnWriteStartElement("Variable internalName=\"HMI_Firmware_Version\" displayName=\"HMI Firmware Version\" type=\"string\"/");
      fnWriteStartElement("Variable internalName=\"OPMM_Firmware_Version\" displayName=\"OPMM Firmware Version\" type=\"string\"/");
      fnWriteEndElement("Record");
			
      fnWriteStartElement("Record internalName=\"Measurement\" displayName=\"Measurement\"");
      fnWriteStartElement("Variable internalName=\"MeasurementStartDate\" displayName=\"Date\" type=\"Date\" /");
      fnWriteStartElement("Variable internalName=\"MeasurementStartTime\" displayName=\"Time\" type=\"Time\" /");
			fnWriteStartElement("Variable internalName=\"SupervisorName\" displayName=\"Supervisor Name\" type=\"string\" /");
      //fnWriteStartElement("Variable internalName=\"JointNumber\" displayName=\"Joint Number\" type=\"decimal\" /");/*Change 54*/
      if(255 == pstLog->ucGetJobID)
				fnWriteStartElement("Variable internalName=\"JointID\" displayName=\"JointID\" type=\"string\" /");
			fnWriteStartElement("Variable internalName=\"AntifreezTemp\" displayName=\"Antifreez Temp\" type=\"float\" units=\"celsius\" /");
      pstLog->ucDetailFlag = pstLog->ucDetailFlag + 1;
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
      pstLog->ucDetailFlag = pstLog->ucDetailFlag + 1;
    }
    break;
  case 4:
    {
      fnWriteStartElement("Variable internalName=\"Humidity\" displayName=\"Humidity\" type=\"float\" units=\"Percent\" /");
      fnWriteStartElement("Variable internalName=\"DewPoint\" displayName=\"Dew Point\" type=\"float\" units=\"celsius\" /");
      fnWriteStartElement("Variable internalName=\"UpStrokeStall\" displayName=\"Up Stroke Stall Test\" type=\"string\" /");
      fnWriteStartElement("Variable internalName=\"DownStrokeStall\" displayName=\"Down Stroke Stall Test\" type=\"string\" /");
      fnWriteStartElement("Variable internalName=\"BaseMaterial\" displayName=\"Base Material\" type=\"float\" units=\"Litre\" /");
      
      pstLog->ucDetailFlag = pstLog->ucDetailFlag + 1;
    }
    break;
  case 5:
    {
      fnWriteStartElement("Variable internalName=\"HardMaterial\" displayName=\"Hardner Material\" type=\"float\" units=\"Litre\" /");
      fnWriteStartElement("Variable internalName=\"MaterialConsumed\" displayName=\"Material Consumed\" type=\"float\" units=\"Litre\" /");
			fnWriteStartElement("Variable internalName=\"PrimeTime\" displayName=\"Prime Time\" type=\"float\" units=\"Sec\" /");
			fnWriteStartElement("Variable internalName=\"FlushTime\" displayName=\"Flush Time\" type=\"float\" units=\"Sec\" /");
			fnWriteStartElement("Variable internalName=\"SprayTime\" displayName=\"Spray Time\" type=\"float\" units=\"Sec\" /");
			fnWriteEndElement("Record");
      fnWriteStartElement("Record internalName=\"End\" displayName=\"End\"");
      fnWriteStartElement("Variable internalName=\"EndDate\" displayName=\"Date\" type=\"Date\" /");
      fnWriteStartElement("Variable internalName=\"EndTime\" displayName=\"Time\" type=\"Time\" /");
      fnWriteEndElement("Record");
      fnWriteEndElement("View");
      fnWriteStartElement("Data");
      pstLog->ucDetailFlag = pstLog->ucDetailFlag + 1;
    }
    break;
  default:
    {
      pstLog->ucDetailFlag = BYTE_ZERO;
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
void fnStatusLog_Read(DATALOG_TX_t *pstLog)
{
  uint8_t rgucTimeStamp[6];
  uint8_t rgucTempData[100];
  uint8_t ucReadData = 0;
  uint32_t uliStatusStartAddr = pstLog->uliStartAddress;
	
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
  fnRetriveSupervisorName(ucReadData);
	sprintf((char *)rgucTempData,"%s ",supervisorname);
	fnWriteData(rgucTempData);
#ifdef DBG_STATUS_WARN_LOGIC
  sprintf((char *)rgucTempData,"%d ",ucReadData);
  fnWriteData(rgucTempData);
#endif
	
	 /*change49*/
  ucReadData = fnNVSRAM1Byte_Read(uliStatusStartAddr++);
  if(255 == pstLog->ucGetJobID)
	{
			/*Send Job ID*/
			fnRetriveJobID(pstLog->ucStoredJobID);	
			memset(rgucTempData,'\0',100);
			sprintf((char *)rgucTempData,"%s ",(char *)jobid);
			fnWriteData(rgucTempData);	
	}
#ifdef DBG_STATUS_WARN_LOGIC
  sprintf((char *)rgucTempData,"%d ",ucReadData);
  fnWriteData(rgucTempData);
#endif

  /*Joint Number Read*/
  fnNVSRAM1_MultiDataRead (4, uliStatusStartAddr, SplitByte.rgucData);
//  sprintf((char *)rgucTempData,"%0.0f ", SplitByte.fData);/*Change 54*/
//  fnWriteData(rgucTempData);/*Change 54*/
  
  uliStatusStartAddr += 4;
  
  /*AntiFreez Temp Read*/
  fnNVSRAM1_MultiDataRead (4, uliStatusStartAddr, SplitByte.rgucData);
  if((INVALID_FLOAT_READ == SplitByte.fData) || (NaN == SplitByte.uliData))
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
  if((INVALID_FLOAT_READ == SplitByte.fData) || (NaN == SplitByte.uliData))
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
  if((INVALID_FLOAT_READ == SplitByte.fData) || (NaN == SplitByte.uliData))
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
  if((INVALID_FLOAT_READ == SplitByte.fData) || (NaN == SplitByte.uliData))
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
  if((INVALID_FLOAT_READ == SplitByte.fData) || (NaN == SplitByte.uliData))
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
  if((INVALID_FLOAT_READ == SplitByte.fData) || (NaN == SplitByte.uliData))
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
  if((INVALID_FLOAT_READ == SplitByte.fData) || (NaN == SplitByte.uliData))
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
  if((INVALID_FLOAT_READ == SplitByte.fData) || (NaN == SplitByte.uliData))
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
  if((INVALID_FLOAT_READ == SplitByte.fData) || (NaN == SplitByte.uliData))
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
  ((ucReadData & 0x01)? fnWriteData("PASS "):fnWriteData("FAIL "));
  /*Downstroke Stall test Result Data Byte 1*/
  ucReadData = fnNVSRAM1Byte_Read(uliStatusStartAddr++);
  ((ucReadData & 0x01)? fnWriteData("PASS "):fnWriteData("FAIL "));
  
  /* Base material consumed per joint */
  fnNVSRAM1_MultiDataRead (4, uliStatusStartAddr, SplitByte.rgucData);
  if((INVALID_FLOAT_READ == SplitByte.fData) || (NaN == SplitByte.uliData))
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
  if((INVALID_FLOAT_READ == SplitByte.fData) || (NaN == SplitByte.uliData))
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
  if((INVALID_FLOAT_READ == SplitByte.fData) || (NaN == SplitByte.uliData))
  {
    fnWriteData("ERROR ");
  }
  else
  {
    sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
    fnWriteData(rgucTempData);
  }
  
  uliStatusStartAddr += 4;
  
	  /*Prime Process Time *//*change62*/
  fnNVSRAM1_MultiDataRead (4, uliStatusStartAddr, SplitByte.rgucData);
  if((INVALID_FLOAT_READ == SplitByte.fData) || (NaN == SplitByte.uliData))
  {
    fnWriteData("ERROR ");
  }
  else
  {
    sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
    fnWriteData(rgucTempData);
  }
  
  uliStatusStartAddr += 4;
	
	 /*Flush Process Time *//*change62*/
  fnNVSRAM1_MultiDataRead (4, uliStatusStartAddr, SplitByte.rgucData);
  if((INVALID_FLOAT_READ == SplitByte.fData) || (NaN == SplitByte.uliData))
  {
    fnWriteData("ERROR ");
  }
  else
  {
    sprintf((char *)rgucTempData,"%0.2f ", SplitByte.fData);
    fnWriteData(rgucTempData);
  }
  
  uliStatusStartAddr += 4;
	
	 /*Spray Process Time *//*change62*/
  fnNVSRAM1_MultiDataRead (4, uliStatusStartAddr, SplitByte.rgucData);
  if((INVALID_FLOAT_READ == SplitByte.fData) || (NaN == SplitByte.uliData))
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
  memset(rgucTimeStamp,'\0',6); 
  DateTime_t stLogDateTime;
  /*Time Stamp read */
  fnNVSRAM1_MultiDataRead(6, pstLog->uliStartAddress, rgucTimeStamp);		
  
  stLogDateTime.ucYear = rgucTimeStamp[BYTE_ZERO];
  stLogDateTime.ucMonth = rgucTimeStamp[BYTE_ONE];
  stLogDateTime.ucDate = rgucTimeStamp[BYTE_TWO];
  stLogDateTime.ucHour = rgucTimeStamp[BYTE_THREE];
  stLogDateTime.ucMin = rgucTimeStamp[BYTE_FOUR];
  stLogDateTime.ucSec = rgucTimeStamp[BYTE_FIVE];
  pstLog->uliStartAddress += 6;
      
  /*ucStoredJobName change23*/
  pstLog->ucStoredJobName  = fnNVSRAM1Byte_Read(pstLog->uliStartAddress++);
  pstLog->uliStartAddress++;
  /*ucStoredJobID; change23*/
  pstLog->ucStoredJobID  = fnNVSRAM1Byte_Read(pstLog->uliStartAddress++);
  pstLog->uliStartAddress -= 9;
	
  if((pstLog->stStartLogDateTime.uliTotalSec <= fnGetDateTimeInSec(stLogDateTime)) &&
     (pstLog->stEndLogDateTime.uliTotalSec >= fnGetDateTimeInSec(stLogDateTime)) &&
		 (pstLog->ucStoredJobID == pstLog->ucGetJobID) && 
	   (pstLog->ucStoredJobName == pstLog->ucGetJobName))
  {
    if(RESET == pstLog->ucStartLogTimeFlag)
    {
      fnDatalogStartFrame(pstLog);
      pstLog->ucStartLogTimeFlag = SET;
			pstLog->ucStopLogTimeFlag = SET;
      fnStatusLog_Read(pstLog);
      pstLog->uliStartAddress += DL_STA_PACKET_SIZE;
    }
    else
    {
      fnStatusLog_Read(pstLog);
      pstLog->uliStartAddress += DL_STA_PACKET_SIZE;
    }
  }
	else if((pstLog->stStartLogDateTime.uliTotalSec <= fnGetDateTimeInSec(stLogDateTime)) &&
     (pstLog->stEndLogDateTime.uliTotalSec >= fnGetDateTimeInSec(stLogDateTime)) &&
		 (255 == pstLog->ucGetJobID) && 
	   (pstLog->ucStoredJobName == pstLog->ucGetJobName))
  {
    if(RESET == pstLog->ucStartLogTimeFlag)
    {
      fnDatalogStartFrame(pstLog);
      pstLog->ucStartLogTimeFlag = SET;
			pstLog->ucStopLogTimeFlag = SET;
      fnStatusLog_Read(pstLog);
      pstLog->uliStartAddress += DL_STA_PACKET_SIZE;
    }
    else
    {
      fnStatusLog_Read(pstLog);
      pstLog->uliStartAddress += DL_STA_PACKET_SIZE;
    }
  }
  else
  {
    pstLog->uliStartAddress += DL_STA_PACKET_SIZE;
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
      fnStatusLog_Detail(pstLog);
      pstLog->ucStartLogTimeFlag = RESET;
    }
    else if(pstLog->uliStartAddress < pstLog->uliStopAddress)
    {
      fnStatusLogScan(pstLog);
    }
    else
    {
      if(pstLog->ucStopLogTimeFlag == SET)
			{
				pstLog->ucStopLogTimeFlag = RESET;
				fnDatalogEndFrame();
			}
      fnWriteEndElement("Data");
      fnWriteEndElement("CRC-Evans_Data");
			char rgucTempBuff[100]={'\0'};
      memset(rgucTempBuff,'\0',100);
      sprintf((char *)rgucTempBuff,"%02x%02x%02x%02x%02x%02x%02x%02x",0x00,
              0x00,0x00,0x00,0x00,0x00,0x00,0x00);
      fnWriteStartElement("CRC-Evans_MCL");
      fnWriteStartElement("Ext_ID");
      fnWriteMessageID((uint32_t)(0x16000000 | 0x00E80000 | DEST_TABLET | SOURCE_SCU));
      fnWriteEndElement("Ext_ID");
      fnWriteStartElement("DLC");
      fnWriteLength((uint8_t)(1));
      fnWriteEndElement("DLC");
      fnWriteStartElement("Data");			
      fnWriteData((uint8_t *)rgucTempBuff);
      fnWriteEndElement("Data");
      fnWriteEndElement("CRC-Evans_MCL");
      //fnWifiSendData(rgucXMLtxBuffer);
      pstLog->ucLogType = RESET;
      pstLog->ucSendFlag = RESET;
      /*change33*/
      stWarningLog.ucWarnLog_PeriodicSendWarnCounter = RESET;
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
  switch(*ucNextData)
  {
  case 1:
    {
      fnWriteStartElement("CRC-Evans_Data");
      fnWriteStartElement("File_Identity displayName=\"Fault Log\"");
      fnWriteData("Fault Log");
      fnWriteEndElement("File_Identity");
      fnWriteStartElement("View");

      
      fnWriteStartElement("Record internalName=\"Start\" displayName=\"Start\"");
      fnWriteStartElement("Variable internalName=\"StartDate\" displayName=\"Date\" type=\"Date\" /");
      fnWriteStartElement("Variable internalName=\"StartTime\" displayName=\"Time\" type=\"Time\" /");
			fnWriteStartElement("Variable internalName=\"JobName\" displayName=\"Job Name\" type=\"string\" /");
      //fnWriteStartElement("Variable internalName=\"JointID\" displayName=\"JointID\" type=\"string\" /");//ADDED S1711
			fnWriteStartElement("Variable internalName=\"CoatingMachineName\" displayName=\"CoatingMachineName\" type=\"string\" /");
			fnWriteStartElement("Variable internalName=\"LogType\" displayName=\"LogType\" type=\"string\" /");
      fnWriteEndElement("Record");
			      
      fnWriteStartElement("Record internalName=\"Version\" displayName=\"Firmware Version\"");
      fnWriteStartElement("Variable internalName=\"SCU_Firmware_Version\" displayName=\"SCU Firmware Version\" type=\"string\"/");
      fnWriteStartElement("Variable internalName=\"HCM_Firmware_Version\" displayName=\"HCM Firmware Version\" type=\"string\"/");
      fnWriteStartElement("Variable internalName=\"VCM_Firmware_Version\" displayName=\"VCM Firmware Version\" type=\"string\"/");
      fnWriteStartElement("Variable internalName=\"HMI_Firmware_Version\" displayName=\"HMI Firmware Version\" type=\"string\"/");
      fnWriteStartElement("Variable internalName=\"OPMM_Firmware_Version\" displayName=\"OPMM Firmware Version\" type=\"string\"/");
      fnWriteEndElement("Record");
      fnWriteStartElement("Record internalName=\"Measurement\" displayName=\"Measurement\"");
      *ucNextData = *ucNextData + 1;
    }
    break;
  case 2:
    {
      fnWriteStartElement("Variable internalName=\"MeasurementStartDate\" displayName=\"Date\" type=\"Date\" /");
      fnWriteStartElement("Variable internalName=\"MeasurementStartTime\" displayName=\"Time\" type=\"Time\" /");
			fnWriteStartElement("Variable internalName=\"SupervisorName\" displayName=\"Supervisor Name\" type=\"string\" /");
			fnWriteStartElement("Variable internalName=\"JointID\" displayName=\"Joint ID\" type=\"string\" /");//ADDED S1711
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
			fnWriteStartElement("Variable internalName=\"RecirculationValve\" displayName=\"Recirculation Valve\" type=\"string\" /");	/*Change 54*/
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
	fnRetriveSupervisorName(ucReadData);
	sprintf((char *)rgucTempData,"%s ",supervisorname);
	fnWriteData(rgucTempData);
#ifdef DBG_FAULT_WARN_LOGIC
  sprintf((char *)rgucTempData,"%d ",ucReadData);
  fnWriteData(rgucTempData);
#endif
  
	/*ucJobID; change49*/
  ucReadData = fnNVSRAM1Byte_Read(uliFaultStartAddr++);
			/*Send Job ID*///ADDED S1711
	fnRetriveJobID(ucReadData);	
	memset(rgucTempData,'\0',100);
	sprintf((char *)rgucTempData,"%s ",(char *)jobid);
	fnWriteData(rgucTempData);
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
  ((ucReadData >> 6) & 0x01)? fnWriteData("FAIL ") : fnWriteData("- "); /*change54*/
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
  memset(rgucTimeStamp,'\0',6); 
  DateTime_t stLogDateTime;
  /*Time Stamp read */
  fnNVSRAM1_MultiDataRead(6, pstLog->uliStartAddress, rgucTimeStamp);		
  
  stLogDateTime.ucYear = rgucTimeStamp[BYTE_ZERO];
  stLogDateTime.ucMonth = rgucTimeStamp[BYTE_ONE];
  stLogDateTime.ucDate = rgucTimeStamp[BYTE_TWO];
  stLogDateTime.ucHour = rgucTimeStamp[BYTE_THREE];
  stLogDateTime.ucMin = rgucTimeStamp[BYTE_FOUR];
  stLogDateTime.ucSec = rgucTimeStamp[BYTE_FIVE];
	pstLog->uliStartAddress += 6;
    /*ucStoredJobName change23*/
  pstLog->ucStoredJobName  = fnNVSRAM1Byte_Read(pstLog->uliStartAddress++);
  pstLog->uliStartAddress++;
  /*ucStoredJobID; change23*/
  pstLog->ucStoredJobID  = fnNVSRAM1Byte_Read(pstLog->uliStartAddress++);
  pstLog->uliStartAddress -= 9;
	
  if((pstLog->stStartLogDateTime.uliTotalSec <= fnGetDateTimeInSec(stLogDateTime)) &&
     (pstLog->stEndLogDateTime.uliTotalSec >= fnGetDateTimeInSec(stLogDateTime)) &&
		 (pstLog->ucStoredJobName == pstLog->ucGetJobName))
  {
    if(RESET == pstLog->ucStartLogTimeFlag)
    {
      fnDatalogStartFrame(pstLog);
      pstLog->ucStartLogTimeFlag = SET;
			pstLog->ucStopLogTimeFlag = SET;
			fnFaultLog_Read(pstLog->uliStartAddress);
      pstLog->uliStartAddress += DL_FLT_PACKET_SIZE;
    }
    else
    {
      fnFaultLog_Read(pstLog->uliStartAddress);
      pstLog->uliStartAddress += DL_FLT_PACKET_SIZE;
    }
  }
  else
  {
    pstLog->uliStartAddress += DL_FLT_PACKET_SIZE;
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
      fnFaultLog_Detail(&pstLog->ucDetailFlag);
      pstLog->ucStartLogTimeFlag = RESET;
    }
    else if(pstLog->uliStartAddress < pstLog->uliStopAddress)
    {
      fnFaultLogScan(pstLog);
    }
    else
    {
			if(pstLog->ucStopLogTimeFlag == SET)
			{
				pstLog->ucStopLogTimeFlag = RESET;
				fnDatalogEndFrame();
			}
      fnWriteEndElement("Data");
      fnWriteEndElement("CRC-Evans_Data");
			char rgucTempBuff[100]={'\0'};
      memset(rgucTempBuff,'\0',100);
      sprintf((char *)rgucTempBuff,"%02x%02x%02x%02x%02x%02x%02x%02x",0x00,
              0x00,0x00,0x00,0x00,0x00,0x00,0x00);
      fnWriteStartElement("CRC-Evans_MCL");
      fnWriteStartElement("Ext_ID");
      fnWriteMessageID((uint32_t)(0x16000000 | 0x00E80000 | DEST_TABLET | SOURCE_SCU));
      fnWriteEndElement("Ext_ID");
      fnWriteStartElement("DLC");
      fnWriteLength((uint8_t)(1));
      fnWriteEndElement("DLC");
      fnWriteStartElement("Data");			
      fnWriteData((uint8_t *)rgucTempBuff);
      fnWriteEndElement("Data");
      fnWriteEndElement("CRC-Evans_MCL");
      //fnWifiSendData(rgucXMLtxBuffer);
      pstLog->ucLogType = RESET;
      pstLog->ucSendFlag = RESET;
      /*change33*/
      stWarningLog.ucWarnLog_PeriodicSendWarnCounter = RESET;
    }
  }
}
/*****************************************************************************
**@Function			: fnGetJobNameIndex
**@Descriptions	: Send Datalog 
**@parameters		: None
**@return				: None      
*******************************************************************************/
uint8_t fnGetJobNameIndex(char* pcJobName)
{
	int16_t ucIndex=0;
	/*Send Job Index*/
	for(ucIndex=0;ucIndex <= ucJobNameID;ucIndex++)
	//for(ucIndex=ucJobNameID;ucIndex >= 0;ucIndex--)
	{
		fnRetriveJobName(ucIndex);	
		//printf("\n\r%s  %s",jobname,pcJobName);
		if(RESET == strcmp(jobname,pcJobName))
		{
			#ifdef DBG_DATALOG
			printf("\n\rIndex JobName %s,%d",jobname,ucIndex);
			#endif
			return ucIndex;
		}
	}
	return 255;
}
/*****************************************************************************
**@Function			: fnGetJobIDIndex
**@Descriptions	: Send Datalog 
**@parameters		: None
**@return				: None      
*******************************************************************************/
uint8_t fnGetJobIDIndex(char* pcJobID)
{
	int16_t ucIndex=0;
	/*Send Job Index*/
	for(ucIndex=0;ucIndex <= ucJobID;ucIndex++)
	//for(ucIndex=ucJobID;ucIndex >= 0;ucIndex--)
	{
		fnRetriveJobID(ucIndex);	
		//printf("\n\r%s  %s",jobid,pcJobID);
		if(RESET == strcmp(jobid,pcJobID))
		{
			#ifdef DBG_DATALOG
			printf("\n\rIndex JobID %d,%s",ucIndex,jobid);
			#endif
			return ucIndex;
		}
	}
	return 255;
}
/*****************************************************************************
**@Function			: fnGetProcessIndex
**@Descriptions	: Send Datalog 
**@parameters		: None
**@return				: None      
*******************************************************************************/
uint8_t fnGetProcessIndex(char* pcProcessID)
{
	if((strstr(pcProcessID,"IDLE")) != NULL)
	{
		#ifdef DBG_DATALOG
		printf("\n\rIdle Process");
		#endif
		return 0;
	}
	else if((strstr(pcProcessID,"SPRAY")) != NULL)
	{
		#ifdef DBG_DATALOG
		printf("\n\rSPRAY Process");
		#endif
		return 0x01;
	}
	else if((strstr(pcProcessID,"STALL")) != NULL)
	{
		#ifdef DBG_DATALOG
		printf("\n\rSTALL Process");
		#endif
		return 0x02;
	}
	return 255;
}
/*****************************************************************************
**@Function			: fnCheckLogPresent
**@Descriptions	: Send Datalog 
**@parameters		: None
**@return				: None      
*******************************************************************************/
uint8_t fnCheckLogPresent(DATALOG_TX_t *pstLog,uint32_t uliLogSize)
{
  uint8_t rgucTimeStamp[6];
  DateTime_t stLogDateTime;
	pstLog->ucGetJobName = fnGetJobNameIndex(pstLog->cJobName);
	if(255 == pstLog->ucGetJobName)
	{
		#ifdef DBG_DATALOG
		printf("\n\rJob Name Not Found");
		#endif
    return 0;
	}
	if(('\0' != pstLog->cJobID[0]))
	{
		pstLog->ucGetJobID = fnGetJobIDIndex(pstLog->cJobID);
		if(255 == pstLog->ucGetJobID)
		{
			#ifdef DBG_DATALOG
			printf("\n\rJob ID Not Found");
			#endif
			return 0;
		}
	}
	else
	{
		pstLog->ucGetJobID = 255;
	}
	if(('\0' != pstLog->cProcess[0]))
	{
		pstLog->ucGetProcessID = fnGetProcessIndex(pstLog->cProcess);
		if(255 == pstLog->ucGetProcessID)
		{
			#ifdef DBG_DATALOG
			printf("\n\rProcess ID Not Found");
			#endif
			return 0;
		}
	}
	else
	{
		pstLog->ucGetProcessID = 255;
	}
		
		
  while(pstLog->uliStartAddress < pstLog->uliStopAddress)
  {
    memset(rgucTimeStamp,'\0',6); 
    if(PERIODIC_LOG == pstLog->ucLogType)
    {
	    /*Time Stamp read */
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
    }
    else
    {
      fnNVSRAM1_MultiDataRead(6, pstLog->uliStartAddress, rgucTimeStamp);		
    }
    
    stLogDateTime.ucYear = rgucTimeStamp[BYTE_ZERO];
    stLogDateTime.ucMonth = rgucTimeStamp[BYTE_ONE];
    stLogDateTime.ucDate = rgucTimeStamp[BYTE_TWO];
    stLogDateTime.ucHour = rgucTimeStamp[BYTE_THREE];
    stLogDateTime.ucMin = rgucTimeStamp[BYTE_FOUR];
    stLogDateTime.ucSec = rgucTimeStamp[BYTE_FIVE];
		#ifdef DBG_DATALOG
   // printf("\n\rLog Sec %X",fnGetDateTimeInSec(stLogDateTime));
   // printf("\n\rStart Sec %X",pstLog->stStartLogDateTime.uliTotalSec);
    //printf("\n\rEnd Sec %X",pstLog->stEndLogDateTime.uliTotalSec);
    #endif
    if((pstLog->stStartLogDateTime.uliTotalSec <= fnGetDateTimeInSec(stLogDateTime)) &&
       (pstLog->stEndLogDateTime.uliTotalSec >= fnGetDateTimeInSec(stLogDateTime)))
    {
			pstLog->uliStartAddress += 6;
			    if(PERIODIC_LOG == pstLog->ucLogType)
    {
				  if(pstLog->uliStartAddress & PRDADDR_MASK)/*NVSRAM2 Address*/
      {
				      /*ucStoredJobName change23*/
      pstLog->ucStoredJobName  = fnNVSRAM2Byte_Read(pstLog->uliStartAddress++);
		  pstLog->uliStartAddress++;
		  /*ucStoredJobID change23*/
       pstLog->ucStoredJobID  = fnNVSRAM2Byte_Read(pstLog->uliStartAddress++);
				pstLog->ucStoredProcessID = fnNVSRAM2Byte_Read(pstLog->uliStartAddress++);
				pstLog->uliStartAddress--;
		  }
      else
	  {
		  /*ucStoredJobName change23*/
      pstLog->ucStoredJobName  = fnNVSRAM1Byte_Read(pstLog->uliStartAddress++);
		 pstLog->uliStartAddress++;
		   /*ucStoredJobID; change23*/
       pstLog->ucStoredJobID  = fnNVSRAM1Byte_Read(pstLog->uliStartAddress++);
				pstLog->ucStoredProcessID = fnNVSRAM1Byte_Read(pstLog->uliStartAddress++);
			pstLog->uliStartAddress--;
		  }
		}
		else
		{
			      /*ucStoredJobName change23*/
      pstLog->ucStoredJobName  = fnNVSRAM1Byte_Read(pstLog->uliStartAddress++);

      pstLog->uliStartAddress++;
      /*ucStoredJobID; change23*/
       pstLog->ucStoredJobID  = fnNVSRAM1Byte_Read(pstLog->uliStartAddress++);
		}
		#ifdef DBG_DATALOG
		printf("\n\rJobName %d, JointId %d",pstLog->ucStoredJobName,pstLog->ucStoredJobID);
		#endif
		if(pstLog->ucStoredJobName == pstLog->ucGetJobName)
		{
			if(PERIODIC_LOG == pstLog->ucLogType)
			{
				if((255 == pstLog->ucGetProcessID) && (255 == pstLog->ucGetJobID))
				{
					#ifdef DBG_DATALOG
					printf("\n\rLog Present, Joint and Process not there");
					#endif
					return 1;
				}
				else if((255 == pstLog->ucGetProcessID) && (255 != pstLog->ucGetJobID)
					&& (pstLog->ucStoredJobID == pstLog->ucGetJobID))
				{
					#ifdef DBG_DATALOG
					printf("\n\rLog Present Joint there, Process not there");
					#endif
					return 1;
				}
				else if((255 != pstLog->ucGetProcessID) && (255 == pstLog->ucGetJobID)
					&& (pstLog->ucStoredProcessID == pstLog->ucGetProcessID))
				{
					#ifdef DBG_DATALOG
					printf("\n\rLog Present Process there Joint  not there");
					#endif
					return 1;
				}
				else if((255 != pstLog->ucGetProcessID) && (255 != pstLog->ucGetJobID)
					&& (pstLog->ucStoredProcessID == pstLog->ucGetProcessID)
					&& (pstLog->ucStoredJobID == pstLog->ucGetJobID))
				{
					#ifdef DBG_DATALOG
					printf("\n\rLog Present Process and joint present");
					#endif
					return 1;
				}
				
			}
			else if(STATUS_LOG == pstLog->ucLogType)
			{
				if(255 == pstLog->ucGetJobID)
				{
					#ifdef DBG_DATALOG
						printf("\n\rLog Present Status for no JobID");
					#endif
					return 1;
				}
				else if(pstLog->ucStoredJobID == pstLog->ucGetJobID)
				{
					#ifdef DBG_DATALOG
						printf("\n\rLog Present Status");
					#endif
					return 1;
				}
				
			}
			else if((DIAGNOSTIC_LOG == pstLog->ucLogType) || (FAULT_LOG == pstLog->ucLogType))
			{
				#ifdef DBG_DATALOG
				printf("\n\rLog Present Dia or Fault");
				#endif
				return 1;
			}
		}
			pstLog->uliStartAddress -= 9;
    }
      pstLog->uliStartAddress += uliLogSize;
      if(PERIODIC_LOG == pstLog->ucLogType)
      {
        if((0 == pstLog->ucAddrFlag) && (pstLog->uliStartAddress >= DL_PRD_BOUNDARY))
        {
          pstLog->ucAddrFlag = 1;
          pstLog->uliStartAddress = BASE_ADDR_NVSRAM2;
        }
      }
  }
	#ifdef DBG_DATALOG
  printf("\n\rNo Log Present");
	#endif
  return 0;
}
/*****************************************************************************
**@Function		 	: fnDataLoggingWifi
**@Descriptions	: This function is used to get data log 
**@parameters		: None
**@return				: None
*****************************************************************************/
void fnDataLogging(uint8_t *pucString,DATALOG_TX_t *pstLog)
{
  if(strstr((char *)pucString,"Get_Periodic_Log"))
  {
    if(RESET == pstLog->ucSendFlag)
    {
      /*Set flag if command recieved from WIFI*/
      if(PERIODIC_LOG != pstLog->ucLogType)
      {	
				pstLog->ucAddrFlag = 0;
        memset(pstLog,'\0',sizeof(DATALOG_TX_t));
        pstLog->ucLogType = PERIODIC_LOG;
        fnGetLogStartStop(pucString,pstLog);
				#ifdef DBG_DATALOG
        printf("\r\nPERIODIC DATA LOG\r\n");
				#endif
        uint8_t ucLowerdata = fnNVSRAM1Byte_Read(DL_PRD_WR_BASE_L);
        uint8_t ucMiddledata = fnNVSRAM1Byte_Read(DL_PRD_WR_BASE_M);
        uint8_t ucUpperdata = fnNVSRAM1Byte_Read(DL_PRD_WR_BASE_H);
        pstLog->uliStopAddress = (uint32_t)((uint32_t)(ucUpperdata << 16) | 
                                            (uint32_t)(ucMiddledata << 8) | (uint32_t)(ucLowerdata));
        /* Resetting the UNWANTED BITS by Masking with 0xFFFFF*/
        pstLog->uliStopAddress = ( pstLog->uliStopAddress & PRD_MASK2 ); 
        pstLog->uliStartAddress = DL_PRD_SEG_START; /*For NVSRAM1*/
				//printf("\n\r Stop 2 %x, Start 2 %x",pstLog->uliStopAddress,pstLog->uliStartAddress);
        if(fnCheckLogPresent(pstLog,DL_PRD_PACKET_SIZE))
        {
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
					 pstLog->ucAddrFlag = 0;
        }
        else
        {
          memset(pstLog,'\0',sizeof(DATALOG_TX_t));
          /*Send NACK for no datalog*/
          stTx1Message.ExtId = 0x16000000 | 0x00E70000 |
            DEST_TABLET | SOURCE_SCU;	
          /*Update Length of CAN Data Frame*/
          stTx1Message.DLC = 1;
          stTx1Message.Data[0] = 0x00;		/* Positive ACk */
          fnXML_FIFO(&stTx1Message);
					pstLog->ucAddrFlag = 0;
					pstLog->ucDetailFlag = RESET;
          pstLog->ucSendFlag = RESET;
					pstLog->ucLogType = RESET;
        }
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
        pstLog->ucLogType = DIAGNOSTIC_LOG;
        fnGetLogStartStop(pucString,pstLog);
				#ifdef DBG_DATALOG
        printf("\r\nDIAGNOSTIC DATA LOG");
				#endif
        uint8_t ucLowerdata = fnNVSRAM1Byte_Read(DL_DIAG_WR_BASE_L);
        uint8_t ucMiddledata = fnNVSRAM1Byte_Read(DL_DIAG_WR_BASE_M);
        uint8_t ucUpperdata = fnNVSRAM1Byte_Read(DL_DIAG_WR_BASE_H);
        pstLog->uliStopAddress = (uint32_t)((ucUpperdata << 16)| (ucMiddledata << 8)| (ucLowerdata));
        /* Resetting the UNWANTED BITS by Masking with 0x7FFF */
        pstLog->uliStopAddress = ( pstLog->uliStopAddress & DIAG_MASK ); 
        pstLog->uliStartAddress = DL_DIAG_SEG_START;  
        if(fnCheckLogPresent(pstLog,DL_DIAG_PACKET_SIZE))
        {
					uint8_t ucLowerdata = fnNVSRAM1Byte_Read(DL_DIAG_WR_BASE_L);
					uint8_t ucMiddledata = fnNVSRAM1Byte_Read(DL_DIAG_WR_BASE_M);
					uint8_t ucUpperdata = fnNVSRAM1Byte_Read(DL_DIAG_WR_BASE_H);
					pstLog->uliStopAddress = (uint32_t)((ucUpperdata << 16)| (ucMiddledata << 8)| (ucLowerdata));
					/* Resetting the UNWANTED BITS by Masking with 0x7FFF */
					pstLog->uliStopAddress = ( pstLog->uliStopAddress & DIAG_MASK ); 
					pstLog->uliStartAddress = DL_DIAG_SEG_START;  
          pstLog->ucDetailFlag = SET;
          pstLog->ucSendFlag = SET;
        }
        else
        {
          memset(pstLog,'\0',sizeof(DATALOG_TX_t));
          /*Send NACK for no datalog*/
          stTx1Message.ExtId = 0x16000000 | 0x00E70000 |
            DEST_TABLET | SOURCE_SCU;	
          /*Update Length of CAN Data Frame*/
          stTx1Message.DLC = 1;
          stTx1Message.Data[0] = 0x00;		/* Positive ACk */
          fnXML_FIFO(&stTx1Message);
					pstLog->ucDetailFlag = RESET;
          pstLog->ucSendFlag = RESET;
					pstLog->ucLogType = RESET;
        }
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
        memset(pstLog,'\0',sizeof(DATALOG_TX_t));
        pstLog->ucLogType = STATUS_LOG;
        fnGetLogStartStop(pucString,pstLog);
				#ifdef DBG_DATALOG
        printf("\r\nSTATUS DATA LOG");
				#endif
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
        if(fnCheckLogPresent(pstLog,DL_STA_PACKET_SIZE))
        {
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
        else
        {
          memset(pstLog,'\0',sizeof(DATALOG_TX_t));
          /*Send NACK for no datalog*/
          stTx1Message.ExtId = 0x16000000 | 0x00E70000 |
            DEST_TABLET | SOURCE_SCU;	
          /*Update Length of CAN Data Frame*/
          stTx1Message.DLC = 1;
          stTx1Message.Data[0] = 0x00;		/* Positive ACk */
          fnXML_FIFO(&stTx1Message);
					pstLog->ucDetailFlag = RESET;
          pstLog->ucSendFlag = RESET;
					pstLog->ucLogType = RESET;
        }
      }
    }
  }
  else if(strstr((char *)pucString,"Get_Fault_Log"))
  {
    if(RESET == pstLog->ucSendFlag)
    {
      /*Set flag if command recieved from Wifi*/
      memset(pstLog,'\0',sizeof(DATALOG_TX_t));
      if(FAULT_LOG != pstLog->ucLogType)
      {
        pstLog->ucLogType = FAULT_LOG;
        fnGetLogStartStop(pucString,pstLog);
				#ifdef DBG_DATALOG
        printf("\r\nFAULT DATA LOG");
				#endif
        /*Copying the BASE_REGISTER into Local variable uliStatusBaseAddr */
        uint8_t ucLowerdata = fnNVSRAM1Byte_Read(DL_FLT_WR_BASE_L);
        uint8_t ucMiddledata = fnNVSRAM1Byte_Read(DL_FLT_WR_BASE_M);
        uint8_t ucUpperdata = fnNVSRAM1Byte_Read(DL_FLT_WR_BASE_H);
        pstLog->uliStopAddress = (uint32_t)((ucUpperdata << 16)| (ucMiddledata << 8)| (ucLowerdata));
        /* Resetting the UNWANTED BITS by Masking with 0x7FFF*/
        pstLog->uliStopAddress = ( pstLog->uliStopAddress & FLT_MASK );  
        
        pstLog->uliStartAddress = DL_FLT_SEG_START;  
        if(fnCheckLogPresent(pstLog,DL_FLT_PACKET_SIZE))
        {
					/*Copying the BASE_REGISTER into Local variable uliStatusBaseAddr */
					uint8_t ucLowerdata = fnNVSRAM1Byte_Read(DL_FLT_WR_BASE_L);
					uint8_t ucMiddledata = fnNVSRAM1Byte_Read(DL_FLT_WR_BASE_M);
					uint8_t ucUpperdata = fnNVSRAM1Byte_Read(DL_FLT_WR_BASE_H);
					pstLog->uliStopAddress = (uint32_t)((ucUpperdata << 16)| (ucMiddledata << 8)| (ucLowerdata));
					/* Resetting the UNWANTED BITS by Masking with 0x7FFF*/
					pstLog->uliStopAddress = ( pstLog->uliStopAddress & FLT_MASK );  
					pstLog->uliStartAddress = DL_FLT_SEG_START;  
          pstLog->ucDetailFlag = SET;
          pstLog->ucSendFlag = SET;
        }
        else
        {
          memset(pstLog,'\0',sizeof(DATALOG_TX_t));
          /*Send NACK for no datalog*/
          stTx1Message.ExtId = 0x16000000 | 0x00E70000 |
            DEST_TABLET | SOURCE_SCU;	
          /*Update Length of CAN Data Frame*/
          stTx1Message.DLC = 1;
          stTx1Message.Data[0] = 0x00;		/* Positive ACk */
          fnXML_FIFO(&stTx1Message);
					pstLog->ucDetailFlag = RESET;
          pstLog->ucSendFlag = RESET;
					pstLog->ucLogType = RESET;
        }
      }
    }
  }
  else if(strstr((char *)pucString,"Get_CAN_Config_Parameters"))
  {
    if(RESET == pstLog->ucSendFlag)
    {
      memset(pstLog,'\0',sizeof(DATALOG_TX_t));
      pstLog->ucSendFlag = RESET;
			#ifdef DBG_DATALOG
      printf("\r\nConfiguration Parameter");
			#endif
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
      memset(pstLog,'\0',sizeof(DATALOG_TX_t));
      pstLog->ucSendFlag = RESET;
			#ifdef DBG_DATALOG
      printf("\r\nGet_Firmware_Version");
			#endif
      
      /*SCU Firmware Version*/
      stTx1Message.DLC = FIVE_BYTES;
      stTx1Message.ExtId = STATUS_FIRMWARE_VERSION_NUMBER | MSG_TYPE_STATUS |
        DEST_TABLET | SOURCE_SCU;	
      stTx1Message.Data[0] = ID_SCU; 
      SplitByte.fData = SCU_FIRMWARE_VERSION;
      /**************************DATA***********************************/
      for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
      {
				stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt-1];
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
				stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt-1];
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
				stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt-1];
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
				stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt-1];
      }
      fnXML_FIFO(&stTx1Message);
      
      /*HMI Firmware Version*/
      stTx1Message.DLC = FIVE_BYTES;
      stTx1Message.ExtId = STATUS_FIRMWARE_VERSION_NUMBER | MSG_TYPE_STATUS |
        DEST_TABLET | SOURCE_SCU;	
      stTx1Message.Data[0] = ID_HMI; 
      SplitByte.fData = stCurrentData.fCurr_HMI_Firmware_Ver;
      /**************************DATA***********************************/
      for(ucLoopCnt = 1; ucLoopCnt < stTx1Message.DLC; ucLoopCnt++)
      {
				stTx1Message.Data[ucLoopCnt] = SplitByte.rgucData[ucLoopCnt-1];
      }
      fnXML_FIFO(&stTx1Message);
      
      stTx1Message.ExtId = MSG_TYPE_STATUS | ACKNOWLEDGEMENT |
        DEST_TABLET | SOURCE_SCU;	
      /*Update Length of CAN Data Frame*/
      stTx1Message.DLC = 1;
      stTx1Message.Data[0] = 0x00;		/* Positive ACk */
      fnXML_FIFO(&stTx1Message);
      
    }
    
  }
	else if(strstr((char *)pucString,"Get_JointID"))
  {
    if(RESET == pstLog->ucSendFlag)
    {
			uint8_t ucIndex=0;
      memset(pstLog,'\0',sizeof(DATALOG_TX_t));
      pstLog->ucSendFlag = RESET;
			#ifdef DBG_DATALOG
      printf("\r\nGet_JoinID");
			#endif
			memset(rgucXMLtxBuffer,'\0',TRANSMIT_BUFFER);
			/*if(ucJobID == 0)
			{
				printf("\n\rNo Joint ID present");
				char rgucTempBuff[100]={'\0'};
				memset(rgucTempBuff,'\0',100);
				sprintf((char *)rgucTempBuff,"%02x%02x%02x%02x%02x%02x%02x%02x",0x00,
								0x00,0x00,0x00,0x00,0x00,0x00,0x00);
				fnWriteStartElement("CRC-Evans_MCL");
				fnWriteStartElement("Ext_ID");
				fnWriteMessageID((uint32_t)(0x17000000 | 0x00E70000 | DEST_TABLET | SOURCE_SCU));
				fnWriteEndElement("Ext_ID");
				fnWriteStartElement("DLC");
				fnWriteLength((uint8_t)(1));
				fnWriteEndElement("DLC");
				fnWriteStartElement("Data");			
				fnWriteData((uint8_t *)rgucTempBuff);
				fnWriteEndElement("Data");
				fnWriteEndElement("CRC-Evans_MCL");
				fnWifiSendData(rgucXMLtxBuffer);
			}
			else*/
			{
				fnWriteStartElement("CRC-Evans_Data");
				/*Send Joint ID*/
				for(ucIndex=0;ucIndex <= ucJobID;ucIndex++)
				{
					fnRetriveJobID(ucIndex);
					fnWriteStartElement("JointID");
					fnWriteData((uint8_t*)jobid);
					#ifdef DBG_DATALOG
					printf("\n\rJoint ID - %s",jobid);
					#endif
					fnWriteEndElement("JointID");
				}
				fnWriteEndElement("CRC-Evans_Data");
				
				char rgucTempBuff[100]={'\0'};
				memset(rgucTempBuff,'\0',100);
				sprintf((char *)rgucTempBuff,"%02x%02x%02x%02x%02x%02x%02x%02x",0x00,
								0x00,0x00,0x00,0x00,0x00,0x00,0x00);
				fnWriteStartElement("CRC-Evans_MCL");
				fnWriteStartElement("Ext_ID");
				fnWriteMessageID((uint32_t)(0x17000000 | 0x00E80000 | DEST_TABLET | SOURCE_SCU));
				fnWriteEndElement("Ext_ID");
				fnWriteStartElement("DLC");
				fnWriteLength((uint8_t)(1));
				fnWriteEndElement("DLC");
				fnWriteStartElement("Data");			
				fnWriteData((uint8_t *)rgucTempBuff);
				fnWriteEndElement("Data");
				fnWriteEndElement("CRC-Evans_MCL");
				fnWifiSendData(rgucXMLtxBuffer);
			}
		}
	}
	else if(strstr((char *)pucString,"Reset_Periodic_Log"))
  {
    if(RESET == pstLog->ucSendFlag)
    {
      memset(pstLog,'\0',sizeof(DATALOG_TX_t));
      pstLog->ucSendFlag = RESET;
		  /*PERIODIC LOG*/
			#ifdef DBG_DATALOG
			printf("\n\rRESET Periodic LOG");
			#endif
			/* Zeroth array element of Argument Array
			will go to the First Memory Location accessed*/
			uint8_t rgucPRDBase_Reset[] = {PRDBASE_LOW2, PRDBASE_MID2, PRDBASE_HIGH2};  
			fnNVSRAM1MultiData_Write( BYTE_THREE, DL_PRD_WR_BASE_L, rgucPRDBase_Reset);
  
			/*change28*/
			stWarningLog.ucWarnLog_PeriodicSendWarnCounter = RESET;
			
			stTx1Message.ExtId = 0x18000000 | ACKNOWLEDGEMENT |
        DEST_TABLET | SOURCE_SCU;	
      /*Update Length of CAN Data Frame*/
      stTx1Message.DLC = 1;
      stTx1Message.Data[0] = 0x00;		/* Positive ACk */
      fnXML_FIFO(&stTx1Message);
		}
	}
	else if(strstr((char *)pucString,"Reset_Fault_Log"))
  {
    if(RESET == pstLog->ucSendFlag)
    {
      memset(pstLog,'\0',sizeof(DATALOG_TX_t));
      pstLog->ucSendFlag = RESET;
		  /*FAULT LOG*/
			#ifdef DBG_DATALOG
			printf("\n\rRESET Fault LOG");
			#endif
			/* Zeroth array element of Argument Array
			will go to the First Memory Location accessed*/
			uint8_t rgucFLTBase_Reset[] = {FLTBASE_LOW, FLTBASE_MID, FLTBASE_HIGH};    
			fnNVSRAM1MultiData_Write( BYTE_THREE, DL_FLT_WR_BASE_L, rgucFLTBase_Reset);	
			/*change28*/
			stWarningLog.ucWarnLog_FaultSendWarnCounter = RESET;
			stTx1Message.ExtId = 0x18000000 | ACKNOWLEDGEMENT |
        DEST_TABLET | SOURCE_SCU;	
      /*Update Length of CAN Data Frame*/
      stTx1Message.DLC = 1;
      stTx1Message.Data[0] = 0x00;		/* Positive ACk */
      fnXML_FIFO(&stTx1Message);
		}
	}
	else if(strstr((char *)pucString,"Reset_Status_Log"))
  {
    if(RESET == pstLog->ucSendFlag)
    {
      memset(pstLog,'\0',sizeof(DATALOG_TX_t));
      pstLog->ucSendFlag = RESET;
		  /*STATUS LOG*/
			#ifdef DBG_DATALOG
			printf("\n\rRESET Status LOG");
			#endif
			/* Zeroth array element of Argument Array
			will go to the First Memory Location accessed*/
			uint8_t rgucSTABase_Reset[] = {STABASE_LOW, STABASE_MID, STABASE_HIGH};   
			fnNVSRAM1MultiData_Write( BYTE_THREE, DL_STA_WR_BASE_L, rgucSTABase_Reset);	
			/*change28*/
			stWarningLog.ucWarnLog_StatusSendWarnCounter = RESET;
			stTx1Message.ExtId = 0x18000000 | ACKNOWLEDGEMENT |
        DEST_TABLET | SOURCE_SCU;	
      /*Update Length of CAN Data Frame*/
      stTx1Message.DLC = 1;
      stTx1Message.Data[0] = 0x00;		/* Positive ACk */
      fnXML_FIFO(&stTx1Message);
		}
	}
	else if(strstr((char *)pucString,"Reset_Diagnostic_Log"))
  {
    if(RESET == pstLog->ucSendFlag)
    {
      memset(pstLog,'\0',sizeof(DATALOG_TX_t));
      pstLog->ucSendFlag = RESET;
		  /*DIAGNOSTIC LOG*/
			#ifdef DBG_DATALOG
			printf("\n\rRESET Diagnostic LOG");
			#endif
			// Zeroth array element of Argument Array*/
			/* will go to the First Memory Location*/
			uint8_t rgucBase_Reset[] = {DIAGBASE_LOW,DIAGBASE_MID,DIAGBASE_HIGH};    
			fnNVSRAM1MultiData_Write( BYTE_THREE, DL_DIAG_WR_BASE_L, rgucBase_Reset);	
			/*change28*/
			stWarningLog.ucWarnLog_DiagSendWarnCounter = RESET;
			stTx1Message.ExtId = 0x18000000 | ACKNOWLEDGEMENT |
        DEST_TABLET | SOURCE_SCU;	
      /*Update Length of CAN Data Frame*/
      stTx1Message.DLC = 1;
      stTx1Message.Data[0] = 0x00;		/* Positive ACk */
      fnXML_FIFO(&stTx1Message);
		}
	}
  else if(strstr((char *)pucString,"Reset_All_Log"))
  {
		#ifdef DBG_DATALOG
    printf("\n\rRESET DATA LOG");
		#endif
    fnDatalogBaseReg_Reset();
					stTx1Message.ExtId = 0x18000000 | ACKNOWLEDGEMENT |
        DEST_TABLET | SOURCE_SCU;	
      /*Update Length of CAN Data Frame*/
      stTx1Message.DLC = 1;
      stTx1Message.Data[0] = 0x00;		/* Positive ACk */
      fnXML_FIFO(&stTx1Message);
	
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
/*change48*/				
				 				ucJobID = DEFAULT_CURRENT_INDEX_JOB_ID;
 				ucJobLog = DEFAULT_CURRENT_INDEX_JOB_ID; /*change78*/
			fnSPI_EEPROM_WriteData(CURRENT_INDEX_JOB_ID, 1, &ucJobID); /*change78*/
								 fnSPI_EEPROM_WriteData(LOG_INDEX_JOB_ID, 1, &ucJobLog); /*change78*/
 				/*change24*/
 				fnResetName(FLASH_Sector_8);	

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
  if((SET == stEthernetLog.ucSendFlag) && (stEthernetLog.ucNextLogFlag == SET)
     && (SET == stEthernet.ucConnectedStatus))
  {
		#ifdef DBG_DATALOG
    //printf("\n\rEthernet Datalog");
		#endif
    stEthernetLog.ucNextLogFlag = RESET;
    memset(rgucXMLtxBuffer,'\0',TRANSMIT_BUFFER);
    fnDiagnosticLog(&stEthernetLog);
    fnPeriodicLog(&stEthernetLog);
    fnStatusLog(&stEthernetLog);
    fnFaultLog(&stEthernetLog);
    fnEthernetSend(rgucXMLtxBuffer);
  }
  if((SET == stWifiLog.ucSendFlag) && (SET == stWifi.ucConnectedStatus))
  {
		#ifdef DBG_DATALOG
    //printf("\n\rWifi Datalog");
		#endif
    memset(rgucXMLtxBuffer,'\0',TRANSMIT_BUFFER);
    fnDiagnosticLog(&stWifiLog);
    fnPeriodicLog(&stWifiLog);
    fnStatusLog(&stWifiLog);
    fnFaultLog(&stWifiLog);
    fnWifiSendData(rgucXMLtxBuffer);
		/*Wifi Received Timer Reset*/
    stWifi.uiReceiveTimer = RESET;
  }
}







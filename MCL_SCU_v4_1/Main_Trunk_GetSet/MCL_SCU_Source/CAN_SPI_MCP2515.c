
/*******************************************************************************
**@file    CAN_SPI_MCP2515.c
* 
**@brief   This file contains the function used for MCP2515 CAN2 link.
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
uint8_t ucLastTxBuff = 0;
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
static uint8_t fnMCP2515TxRxStatus_Read(uint8_t);

/*****************************************************************************
**@Function      : fnMCPSPI_Config
**@Descriptions	: Provide the initiation of the SPI port to be use for the 
MCP2515 transmission. 
**@parameters		: None
**@return				: None
*****************************************************************************/
void fnMCPSPI_Config(void)
{
  NVIC_InitTypeDef stNVIC_InitStructure;
  SPI_InitTypeDef  stSPI_InitStructure;
  GPIO_InitTypeDef stGPIO_InitStructure;
  /* Peripheral Clock Enable -------------------------------------------------*/
  /* Enable the SPI clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI5, ENABLE);
  
#ifdef DBG      
  ////DEBUG_PRINTF("SPI clk \n\r");
#endif	
  /* Enable GPIO clocks */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF , ENABLE);
  
#ifdef DBG    	
  ////DEBUG_PRINTF("GPIO cllk\n\r");
#endif	
  /* SPI GPIO Configuration --------------------------------------------------*/
  
  /* Connect SPI pins to AF5 */  
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource6, GPIO_AF_SPI5);
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource7, GPIO_AF_SPI5);    
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource8, GPIO_AF_SPI5);
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource9, GPIO_AF_SPI5);
  
  /* SPI CS, SCK, MISO and MOSI pin configuration */
  stGPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  stGPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  stGPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  stGPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
  stGPIO_InitStructure.GPIO_Pin = MCP_SPI_CS | MCP_SPI_SCK | MCP_SPI_MISO |
    MCP_SPI_MOSI;
  GPIO_Init(MCP_GPIO_PORT, &stGPIO_InitStructure);
  
  
#ifdef DBG    	
  DEBUG_PRINTF("GPIO Init\n\r");
#endif
  
  /* SPI configuration -------------------------------------------------*/
  SPI_I2S_DeInit(SPI5);
  stSPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  stSPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  stSPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  stSPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  stSPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  stSPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;
  stSPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  stSPI_InitStructure.SPI_CRCPolynomial = 7;
  
  /* Configure the Priority Group to 1 bit */                
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
  /* Configure the SPI interrupt priority */
  stNVIC_InitStructure.NVIC_IRQChannel = SPI5_IRQn;
  stNVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  stNVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  stNVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&stNVIC_InitStructure);   
  stSPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_Init(SPI5, &stSPI_InitStructure);
  
#ifdef DBG    	
  DEBUG_PRINTF("SPI Init\n\r");
#endif	
  
  /* Enable the SPI peripheral */
  SPI_Cmd(SPI5, ENABLE);
#ifdef DBG	
  DEBUG_PRINTF("SPI Enable\n\r");
#endif	
  
  /* Disable the Rx buffer not empty interrupt */
  SPI_I2S_ITConfig(SPI5, SPI_I2S_IT_RXNE, DISABLE);
  
  /* Disable the Tx empty interrupt */
  SPI_I2S_ITConfig(SPI5, SPI_I2S_IT_TXE, DISABLE);
}

/*****************************************************************************
**@Function 			: fnMCP2515_Init
**@Descriptions	: Configure and initialize externalCAN controller MCP2515.
**@parameters  	: None
**@return				: ucRet : Pass or fail status
*****************************************************************************/
uint8_t fnMCP2515_Init(void)   
{   
  uint8_t ucRet;   
  
#ifdef GENERAL_DBG    
  DEBUG_PRINTF("in mcp2515 init \r\n");
#endif	
  
  MCP2515_UNSELECT();   
  
  fnMCP2515_Reset(); 
  
  ucRet = fnMCP2515CANCTRLMode_Set(MCP2515_MODE_CONFIG);   
  
  if(MCP2515_FAIL == ucRet) 
    return ucRet;  /* function exit on error */   
  /*Set the MCP2515 CAN2 Baudrate*/
  fnMCP2515_ConfigRate();   
  
  fnMCP2515CANBuffer_Init();			
  return ucRet;   
}   
/*****************************************************************************
**@Function 		  : fnMCP2515_Reset
**@Descriptions	: Resets the MCP2515 by sending commad.
**@parameters		: None
**@return				: None
*****************************************************************************/
void fnMCP2515_Reset(void)   
{      
  MCP2515_SELECT();   
  fnSPI_ReadWrite(SPI5, MCP2515_RESET);   
  MCP2515_UNSELECT();  
  
#ifdef GENERAL_DBG
  DEBUG_PRINTF("Reset done Delay start\r\n");	
#endif
  
  /*roughly  >128 MCP clock-cycles */  
  fnDelay_ms(10); 
  
}

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
**@Function 			: fnMCP2515CANCTRLMode_Set
**@Descriptions	: set can control mode of MCP2515
**@parameters		: ucNewMode: mode to be set
**@return				: staus as OK or FAIL
*****************************************************************************/
uint8_t fnMCP2515CANCTRLMode_Set(uint8_t ucNewMode)   
{   
  uint8_t ucRcvData = 0;   
  
  fnMCP2515_ModifyRegister(MCP2515_CANCTRL, MCP2515_MODE_MASK, ucNewMode);   
  
  ucRcvData = fnMCP2515Register_Read(MCP2515_CANCTRL);   
  ucRcvData &= MCP2515_MODE_MASK;   
  
  if (ucRcvData == ucNewMode) 
  {   
    return MCP2515_OK;    
  }   
  else 
  {   
    return MCP2515_FAIL;   
  }
}   

/***************************************************************************
**@Function 			: fnMCP2515_ModifyRegister
**@Descriptions	: modify the MCP2515 register
**@parameters		: ucAddress : The register address of the MCP2515.
Mask		: Mask bits
Data		: command or data to write
**@return				: None
****************************************************************************/
void fnMCP2515_ModifyRegister(uint8_t ucAddress, uint8_t ucMask, uint8_t rgucData)   
{   
  MCP2515_SELECT();  
  
  fnSPI_ReadWrite(SPI5, MCP2515_BIT_MODIFY);   
  fnSPI_ReadWrite(SPI5, ucAddress);   
  fnSPI_ReadWrite(SPI5, ucMask);   
  fnSPI_ReadWrite(SPI5, rgucData); 
  
  MCP2515_UNSELECT();   
}   

/***************************************************************************
**@Function 			: fnMCP2515_ConfigRate
**@Descriptions	: configure CAN speed for MCP2515
**@parameters		:	None
**@return				: None
***************************************************************************/
void fnMCP2515_ConfigRate(void)   
{   
  /*config for 125kbps..... crystal freq @24Mhz*/   
  fnMCP2515Register_Set(MCP2515_CNF1, BAUD_CFG1);   
  fnMCP2515Register_Set(MCP2515_CNF2, BAUD_CFG2);   
  fnMCP2515Register_Set(MCP2515_CNF3, BAUD_CFG3);   
}

/*****************************************************************************
**@Function 			: fnMCP2515Register_Read
**@Descriptions	: Send the Read command over SPI bus. It can read any register
byte accordingly of the address provided.
**@parameters		: ucAddress: is the register address of the MCP2515.
**@return				: data read from register
******************************************************************************/
uint8_t fnMCP2515Register_Read(uint8_t ucAddress)   
{   
  uint8_t ucRet;   
  MCP2515_SELECT();   
  fnSPI_ReadWrite(SPI5, MCP2515_READ);   
  fnSPI_ReadWrite(SPI5, ucAddress);   
  ucRet = fnSPI_ReadWrite(SPI5, 0x00);   
  MCP2515_UNSELECT();   
  
  return ucRet;   
}   

/*****************************************************************************
**@Function 			: fnMCP2515Register_Set
**@Descriptions	: Write data to the MCP2515 register
**@parameters		: ucAddress : is the register address of the MCP2515.
ucValue : value to be write on given address
**@return				: None
******************************************************************************/
void fnMCP2515Register_Set( uint8_t ucAddress,  uint8_t ucValue)   
{   
  MCP2515_SELECT(); 
  
  fnSPI_ReadWrite(SPI5, MCP2515_WRITE);   
  fnSPI_ReadWrite(SPI5, ucAddress);   
  fnSPI_ReadWrite(SPI5, ucValue); 
  
  MCP2515_UNSELECT();   
}   

/*****************************************************************************
**@Function 		  : fnMCP2515CANBuffer_Init
**@Descriptions	: Initialize MCP2515 CAN buffers
**@parameters		: None  
**@return				: None
******************************************************************************/
void fnMCP2515CANBuffer_Init(void)   
{   
  /* enable both receive-buffers to receive messages  */ 
  /* with std. and ext. identifiers   */
  /* and enable rollover   */
  /* Enable filter RXf0 and RXF2 */
  fnMCP2515_ModifyRegister(MCP2515_RXB0CTRL,    
                           MCP_ID_MODE_MASK | (RXB0F_BUKT_MASK) | (RXB0F_FILHIT0_MASK),    
                           MCP_ID_MODE_BITS | (RXB0F_BUKT_BIT) | (RXB0F_FILHIT0_BIT) );   
  fnMCP2515_ModifyRegister(MCP2515_RXB1CTRL, MCP_ID_MODE_MASK | MCP_FILTER_MASK_RXB1,    
                           MCP_ID_MODE_BITS | (RXB1F_FILHIT2_BIT));   
  
  /* Filter Configuration for OPMM (0x03)and 	All i.e 0xFF */
  /* Filter Id 0 Configuration */
  fnMCP2515_ModifyRegister(MCP2515_RXF0SIDH, FILTER_MASK_SIDH, FILTER_ID_8BIT_0);
  fnMCP2515_ModifyRegister(MCP2515_RXF0SIDL, FILTER_MASK_SIDL, FILTER_EXTID_BIT_SET);
  fnMCP2515_ModifyRegister(MCP2515_RXF0EID8, FILTER_MASK_EID8, FILTER_ID_DEST_OPMM);
  fnMCP2515_ModifyRegister(MCP2515_RXF0EID0, FILTER_MASK_EID0, FILTER_ID_8BIT_0);
  
  /* Mask 0 Initialization*/
  fnMCP2515_ModifyRegister(MCP2515_RXM0SIDH, FILTER_MASK_8BIT_0, FILTER_MASK_8BIT_0);
  fnMCP2515_ModifyRegister(MCP2515_RXM0SIDL, FILTER_MASK_8BIT_0, FILTER_MASK_8BIT_0);
  fnMCP2515_ModifyRegister(MCP2515_RXM0EID8, FILTER_MASK_8BIT_FF, FILTER_MASK_8BIT_FF);
  fnMCP2515_ModifyRegister(MCP2515_RXM0EID0, FILTER_MASK_8BIT_0, FILTER_MASK_8BIT_0);
  
  /* Filter Id 2 Configuration */
  fnMCP2515_ModifyRegister(MCP2515_RXF2SIDH, FILTER_MASK_8BIT_FF, FILTER_ID_8BIT_0);
  fnMCP2515_ModifyRegister(MCP2515_RXF2SIDL, FILTER_MASK_SIDL, FILTER_EXTID_BIT_SET);
  fnMCP2515_ModifyRegister(MCP2515_RXF2EID8, FILTER_MASK_8BIT_FF, FILTER_ID_DEST_ALL);
  fnMCP2515_ModifyRegister(MCP2515_RXF2EID0, FILTER_MASK_8BIT_FF, FILTER_ID_8BIT_0);
  
  /*Mask 0 Initialization*/
  fnMCP2515_ModifyRegister(MCP2515_RXM1SIDH, FILTER_MASK_8BIT_0, FILTER_MASK_8BIT_0);
  fnMCP2515_ModifyRegister(MCP2515_RXM1SIDL, FILTER_MASK_8BIT_0, FILTER_MASK_8BIT_0);
  fnMCP2515_ModifyRegister(MCP2515_RXM1EID8, FILTER_MASK_8BIT_FF, FILTER_MASK_8BIT_FF);
  fnMCP2515_ModifyRegister(MCP2515_RXM1EID0, FILTER_MASK_8BIT_0, FILTER_MASK_8BIT_0);
  
  /*Enable both Rx buff interrupts*/
  fnMCP2515_ModifyRegister(MCP2515_CANINTE, MCP2515_INTE_MASK, MCP2515_CANINTF_RX0IF |
                           MCP2515_CANINTF_RX1IF);
}   

/*****************************************************************************
**@Function 			: fnMCP2515CANId_Write
**@Descriptions	: Write CAN Id for MCP2515
**@parameters		: ucMCP_Addr: register address of MCP2515
ucExt 		: Extended or standard id
uliCan_Id : CAN id
**@return				: None
******************************************************************************/
void fnMCP2515CANId_Write(uint8_t ucMCP_Addr, uint8_t ucExt, uint32_t uliCan_Id )   
{   
  uint16_t uiCanID;   
  uint8_t rgucTBuffData[4];   
  
  uiCanID = (uint16_t)(uliCan_Id & MCP_CAN_ID_L16_BIT);   
  
  if(1 == ucExt) 
  {   
    rgucTBuffData[MCP_EID0] = (uint8_t) (uiCanID & MASK_8BIT_FF);   
    rgucTBuffData[MCP_EID8] = (uint8_t) (uiCanID / MCP_EID8_DIV );   
    uiCanID = (uint16_t)(uliCan_Id / CAN_ID_HIGH_16_BIT_DIV );   
    rgucTBuffData[MCP_SIDL] = (uint8_t) (uiCanID & MCP_SIDL_EID16_17);   
    rgucTBuffData[MCP_SIDL] += (uint8_t) ((uiCanID & MCP_SIDL_1C ) * 
                                          MCP_SIDL_RIGHT_SHIFT_4);   
    rgucTBuffData[MCP_SIDL] |= MCP_TXB_EXIDE_M;
    rgucTBuffData[MCP_SIDH] = (uint8_t)(uiCanID / MCP_5TH_BIT);   
  }   
  else
  {   
    rgucTBuffData[MCP_SIDH] = (uint8_t) (uiCanID / MCP_SIDH_DIV );   
    rgucTBuffData[MCP_SIDL] = (uint8_t) ((uiCanID & MCP_SIDL_LOW_3BIT ) * MCP_5TH_BIT);   
    rgucTBuffData[MCP_EID0] = 0;   
    rgucTBuffData[MCP_EID8] = 0;   
  }   
  fnMCP2515Register_SetS(ucMCP_Addr, rgucTBuffData, 4);   
}   
/**********************************************************************************
**@Function 			: fnMCP2515Register_SetS
**@Descriptions	: write to the multiple registers of MCP2515
**@parameters		: ucAddress 	:	register address of MCP2515  
rgucValues[]:	Array to data to write to the Registers
ucLen 			: length of data to write in the registers
**@return				: None
***********************************************************************************/
void fnMCP2515Register_SetS(uint8_t ucAddress, uint8_t rgucValues[], uint8_t ucLen)   
{   	
  MCP2515_SELECT(); 
  
  fnSPI_ReadWrite(SPI5, MCP2515_WRITE);   
  fnSPI_ReadWrite(SPI5, ucAddress);   
  /* mcp2515 has auto-increment of address-pointer */   
  for (ucLoopCnt = 0; ucLoopCnt < ucLen; ucLoopCnt++)
  {   
    fnSPI_ReadWrite(SPI5, rgucValues[ucLoopCnt]);   
  }   
  MCP2515_UNSELECT();   
}   

/*****************************************************************************
**@Function 			: fnMCP2515Register_ReadS
**@Descriptions	: read multiple registers of MCP2515
**@parameters		: ucAddress : address of the register 
rgucValues[]: Read data store in this array
ucLen     : No. of Byte to read
**@return				: None
******************************************************************************/
void fnMCP2515Register_ReadS(uint8_t ucAddress,    
                             uint8_t rgucValues[], uint8_t ucLen)   
{   	
  MCP2515_SELECT(); 
  
  fnSPI_ReadWrite(SPI5, MCP2515_READ);   
  fnSPI_ReadWrite(SPI5, ucAddress);   
  /* mcp2515 has auto-increment of address-pointer */   
  for (ucLoopCnt = 0; ucLoopCnt < ucLen; ucLoopCnt++)
  {   
    rgucValues[ucLoopCnt] = fnSPI_ReadWrite(SPI5, 0x00);   
  }   
  MCP2515_UNSELECT();   
}

/*****************************************************************************
**@Function 			: fnMCPCANMsgStruct_Init
**@Descriptions	: Initialize CAN message structure
**@parameters		: pstMCPMsg : pointer to MCP CAN message 
**@return				: None
******************************************************************************/
void fnMCPCANMsgStruct_Init(MCPCANMessage_TypeDef* pstMCPMsg)   
{   
  memset(pstMCPMsg, 0, sizeof(MCPCANMessage_TypeDef));   
}   

/*****************************************************************************
**@Function 			: fnMCPCANMsg_Send
**@Descriptions	: Send CAN message
**@parameters		: pstMCPMsg : pointer to MCP CAN message 
**@return				: returns status CAN_OK
******************************************************************************/
uint8_t fnMCPCANMsg_Send( MCPCANMessage_TypeDef* pstMCPMsg)   
{   
  uint8_t ucTxBuff;   
  
  fnMCP2515NextFreeTXBuff_Get(&ucTxBuff); 
  
  /* Clear the TXBuff if the last Transmit message is pending*/
  if(0 != ucLastTxBuff)
  {		
    if((fnMCP2515Register_Read(ucLastTxBuff) & TXREQ) == TXREQ)
    {
      fnMCP2515_ModifyRegister(ucLastTxBuff, TXREQ, 0);
    }
  }
  
  fnMCP2515CANMsg_Write( ucTxBuff, pstMCPMsg);   
  fnMCP2515Start_Transmit(ucTxBuff ); 
  ucLastTxBuff = ucTxBuff - 1; /* TXnCNTRL Buff(Reg) Address*/		
  return CAN_OK;     
}   

/*****************************************************************************
**@Function 			: fnMCPCANRcv_Check
**@Descriptions	: Check received message status
**@parameters		: None  
**@return				: return status of availability of CAn message
******************************************************************************/
uint8_t fnMCPCANRcv_Check(void)   
{   
  uint8_t ucRet;   
  
  ucRet = fnMCP2515Status_Read(); /*RXnIF in Bit 1 and 0*/   
  if (ucRet & MCP2515_RXF0EID0) 
  {   
    return CAN_MSGAVAIL;   
  }   
  else 
  {   
    return CAN_NOMSG;   
  }   
}  

/*****************************************************************************
**@Function 			: fnMCP2515Status_Read
**@Descriptions	: read status of MCP2515
**@parameters		: None  
**@return				: Transmit/ receive status
******************************************************************************/
uint8_t fnMCP2515Status_Read(void)   
{   
  return fnMCP2515TxRxStatus_Read(MCP2515_READ_RXTX_STATUS);   
} 

/*****************************************************************************
**@Function 			:	fnMCP2515TxRxStatus_Read
**@Descriptions	: readXX status helper
**@parameters		: ucCmd :  command for address to be read
**@return				: returns rgucData
******************************************************************************/
static uint8_t fnMCP2515TxRxStatus_Read( uint8_t ucCmd)   
{   
  uint8_t rgucData;   
  
  MCP2515_SELECT();   
  fnSPI_ReadWrite(SPI5, ucCmd);   
  rgucData = fnSPI_ReadWrite(SPI5, 0x00);   
  MCP2515_UNSELECT();   
  
  return rgucData;   
}   

/*****************************************************************************
**@Function 			: fnMCP2515NextFreeTXBuff_Get
**@Descriptions	:	Get next free TX buffer 
**@parameters		: pucTxBuff :  pointer to message buffer number
**@return				:	uint8_t: it will return the status of Tx Buff empty or not
******************************************************************************/
uint8_t fnMCP2515NextFreeTXBuff_Get(uint8_t *pucTxBuff)   
{   
  uint8_t  ucLoopCnt, ucCtrlVal;   
  uint8_t rgucCtrlRegs[MCP_N_TXBUFFERS] = { MCP2515_TXB0CTRL, 
  MCP2515_TXB1CTRL, MCP2515_TXB2CTRL };   
  
  /*ucRet = MCP_ALLTXBUSY;*/   
  *pucTxBuff = 0x00;   
  
  /*check all 3 TX-Buffers*/   
  for(ucLoopCnt = 0; ucLoopCnt < MCP_N_TXBUFFERS; ucLoopCnt++) 
  {   
    ucCtrlVal = fnMCP2515Register_Read(rgucCtrlRegs[ucLoopCnt]);   
    if ((ucCtrlVal & MCP2515_CANSTAT_TXB1_INT) == 0) 
    {   		
      *pucTxBuff = rgucCtrlRegs[ucLoopCnt] + 1; /*return SIDH-address of Buffer*/   
      /*ucRet = MCP2515_OK; */  
      return (uint8_t)MCP2515_OK; /* ! function exit */   
    }   
  }   
  return (uint8_t)MCP_ALLTXBUSY;   
}   

/*****************************************************************************
**@Function 			: fnMCP2515Start_Transmit
**@Descriptions	: start transmitting for MCP2515
**@parameters		: ucBuffer :  number of transmit buffer
**@return				: None
******************************************************************************/
void fnMCP2515Start_Transmit(uint8_t ucBuffer)   
{   
  fnMCP2515_ModifyRegister(ucBuffer - 1 , MCP2515_MODE_ONESHOT,    
                           MCP2515_MODE_ONESHOT);   
}  

/*****************************************************************************
**@Function 			: fnMCP2515CANMsg_Write
**@Descriptions	: write CAN messsage for MCP2515
**@parameters		: ucBuffer_Sidh_Addr: buffer address of MCP2515
*pstMCPMsg : pointer of message to be write in MCP2515
to send over CAN
**@return				: None
******************************************************************************/
void fnMCP2515CANMsg_Write(uint8_t ucBuffer_Sidh_Addr,    
                           MCPCANMessage_TypeDef* pstMCPMsg)   
{   
  uint8_t ucMCP_Addr, ucDlc;   
  
  ucMCP_Addr = ucBuffer_Sidh_Addr;   
  ucDlc = pstMCPMsg->ucDlc;   
  
  fnMCP2515Register_SetS(ucMCP_Addr + 5, &(pstMCPMsg->rgucData[0]), ucDlc); 
  fnMCP2515CANId_Write(ucMCP_Addr, pstMCPMsg->ucExtended_Identifier,   
                       pstMCPMsg->uliIdentifier);  /*write CAN id*/  
  
  if (1 ==  pstMCPMsg->ucRtr)  /*if RTR  bit is set in a byte*/   
  {
    ucDlc |= MCP_RTR_MASK;  
  }
  fnMCP2515Register_Set((ucMCP_Addr + 4), ucDlc);  /*write the RTR and DLC*/   
}   

/*****************************************************************************
**@Function 			: fnMCPCANMsg_Process
**@Descriptions	:	process CAn message 
**@parameters		: None 
**@return				: None
******************************************************************************/
void fnMCPCANMsg_Process(void)   
{
  MCPCANMessage_TypeDef stMCPMsg;
#ifdef GENERAL_DBG
  DEBUG_PRINTF("In fnMCPCANMsg_Process \r\n");
#endif
  
  fnMCPCANMsg_Read(&stMCPMsg); 
  
  /*copy received CAN message to the Main CAN structure */
  rgstRxCANBuff[uiStoreRxCANBuffCnt].ExtId = stMCPMsg.uliIdentifier;
  rgstRxCANBuff[uiStoreRxCANBuffCnt].IDE = stMCPMsg.ucExtended_Identifier;
  rgstRxCANBuff[uiStoreRxCANBuffCnt].RTR = stMCPMsg.ucRtr;
  rgstRxCANBuff[uiStoreRxCANBuffCnt].DLC = stMCPMsg.ucDlc;
  
  memset(rgstRxCANBuff[uiStoreRxCANBuffCnt].Data, 0, 
         sizeof(rgstRxCANBuff[uiStoreRxCANBuffCnt].Data)); 
  for(ucLoopCnt = 0; ucLoopCnt < rgstRxCANBuff[uiStoreRxCANBuffCnt].DLC ;
  ucLoopCnt++)
  {
    rgstRxCANBuff[uiStoreRxCANBuffCnt].Data[ucLoopCnt] =  
      stMCPMsg.rgucData[ucLoopCnt];	
  }
  uiStoreRxCANBuffCnt++;
  if(RX_CAN_BUF_SIZE <= uiStoreRxCANBuffCnt)
  {
    uiStoreRxCANBuffCnt = 0;
    StatusFlag.stStatusFlag.bDataInCANRx_BuffFlag =  SET;
  }
#ifdef GENERAL_DBG    
  DEBUG_PRINTF("EXT id = %x \r\n ", stMCPMsg.ucExtended_Identifier);
  DEBUG_PRINTF("id = %x\r\n ", stMCPMsg.uliIdentifier);
#endif				
}

/*****************************************************************************
**@Function 			: fnMCPCANMsg_Read
**@Descriptions	: Read CAN message
**@parameters		: pstMCPMsg :  read message pointer
**@return				: OK or NO message
******************************************************************************/
uint8_t fnMCPCANMsg_Read(MCPCANMessage_TypeDef *pstMCPMsg)   
{   
  uint8_t ucStat, ucRet;   
  
  ucStat = fnMCP2515Status_Read();   
  
  if (ucStat & MCP_STAT_RX0IF) 
  {   
    /*Msg in Buffer 0*/   
    fnMCP2515CANMsg_Read( MCP2515_RXB0SIDH, pstMCPMsg);   
    fnMCP2515_ModifyRegister(MCP_CANINTF, MCP2515_CANINTF_RX0IF, 0);   
    ucRet = CAN_OK;   
  }   
  else if(ucStat & MCP_STAT_RX1IF)
  {   
    /*Msg in Buffer 1*/   
    fnMCP2515CANMsg_Read( MCP2515_RXB1SIDH, pstMCPMsg);   
    fnMCP2515_ModifyRegister(MCP2515_CANINTF, MCP2515_CANINTF_RX1IF, 0);   
    ucRet = CAN_OK;   
  }   
  else 
  {   
    ucRet = CAN_NOMSG;   
  }
  return ucRet;   
}   

/*****************************************************************************
**@Function 			: fnMCP2515CANMsg_Read
**@Descriptions	: read CAN message from MCP2515
**@parameters		: ucBuffer_Sidh_Addr : Buffer register address of MCP2515
pstMCPMsg					 :  read message pointer 
**@return				: None
******************************************************************************/
void fnMCP2515CANMsg_Read(uint8_t ucBuffer_Sidh_Addr,   
                          MCPCANMessage_TypeDef* pstMCPMsg)   
{     
  uint8_t ucMCP_Addr, ucCtrl;   
  
  ucMCP_Addr = ucBuffer_Sidh_Addr;   
  
  fnMCP2515CANId_Read( ucMCP_Addr, &(pstMCPMsg->ucExtended_Identifier),    
                      &(pstMCPMsg->uliIdentifier) );   
  
  ucCtrl = fnMCP2515Register_Read( ucMCP_Addr-1 );   
  pstMCPMsg->ucDlc = fnMCP2515Register_Read( ucMCP_Addr+4 );   
  
  if (ucCtrl & 0x08) 
  {   
    pstMCPMsg->ucRtr = 1;   
  } 
  else 
  {   
    pstMCPMsg->ucRtr = 0;   
  }   
  
  pstMCPMsg->ucDlc &= MCP_DLC_MASK;   
  fnMCP2515Register_ReadS(ucMCP_Addr+5, &(pstMCPMsg->rgucData[0]), pstMCPMsg->ucDlc);   
} 

/*******************************************************************************
**@Function 			: fnMCP2515CANId_Read
**@Descriptions	:	Read CAN Id for MCP2515 
**@parameters		: ucMCP_Addr : MCP2515 register address
pucExt 		 : for extended can ID
pulCAN_Id	 : CAN ID
**@return				: None
**********************************************************************************/
void fnMCP2515CANId_Read(uint8_t ucMCP_Addr, uint8_t* pucExt, uint32_t* puliCAN_Id)   
{   
  uint8_t rgucTBuffData[4];   
  
  *pucExt = 0;   
  *puliCAN_Id = 0;   
  
  fnMCP2515Register_ReadS(ucMCP_Addr, rgucTBuffData, 4);   
  
  *puliCAN_Id = (rgucTBuffData[MCP_SIDH] << 3) + (rgucTBuffData[MCP_SIDL] >> 5);   
  
  if(MCP_TXB_EXIDE_M == (rgucTBuffData[MCP_SIDL] & MCP_TXB_EXIDE_M))
  { 
    /*Extended Id*/   
    *puliCAN_Id = (*puliCAN_Id << 2) + (rgucTBuffData[MCP_SIDL] & 0x03);   
    *puliCAN_Id <<= 16;   
    *puliCAN_Id = *puliCAN_Id +(rgucTBuffData[MCP_EID8]<<8) + rgucTBuffData[MCP_EID0];   
    *pucExt = 1;   
  }
}   

/*****************************************************************************
**@Function 			: fnMCPCAN2_Transmit
**@Descriptions	: this function will tx the can msg using CAN controller
MCP2515(SPI)
**@parameters		: pstCANTxMsg : pointer to CAN Trasmit message structure
**@return				: None
*****************************************************************************/
void fnMCPCAN2_Transmit(CanTxMsg *pstCANTxMsg)
{
  MCPCANMessage_TypeDef stMCPMsg;
  
  fnMCPCANMsgStruct_Init(&stMCPMsg); 
  
  stMCPMsg.uliIdentifier = pstCANTxMsg->ExtId;   
  stMCPMsg.ucExtended_Identifier = CAN_EXTID;  
  stMCPMsg.ucDlc = pstCANTxMsg->DLC;
  
  for(ucLoopCnt = 0; ucLoopCnt < stMCPMsg.ucDlc; ucLoopCnt++)
  {	
    stMCPMsg.rgucData[ucLoopCnt] = pstCANTxMsg->Data[ucLoopCnt];
  }
  fnMCPCANMsg_Send(&stMCPMsg);   
}


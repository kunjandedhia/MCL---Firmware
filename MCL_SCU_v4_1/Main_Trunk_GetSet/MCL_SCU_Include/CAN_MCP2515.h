

#ifndef MCP2515_H_
#define MCP2515_H_

/* ----------------------------------------------------------------------------
 *                           Includes
 * ----------------------------------------------------------------------------
 */
#include <stdint.h>
/* ----------------------------------------------------------------------------
 *                           MACROS
 * ----------------------------------------------------------------------------
 */

#define MCP_CAN_MAX_CHAR_IN_MESSAGE (8) 
#define MCP_SPI_CS 														GPIO_Pin_6
#define MCP_SPI_SCK														GPIO_Pin_7
#define MCP_SPI_MISO 													GPIO_Pin_8
#define MCP_SPI_MOSI 													GPIO_Pin_9
#define MCP_GPIO_PORT													GPIOF
/* ----------------------------------------------------------------------------
 *                           CONSTANTS
 * ----------------------------------------------------------------------------
 */

/* ----------------------------------------------------------------------------
 *                           GLOBAL VARIABLES
 * ----------------------------------------------------------------------------
 */
  /*Structure of a CANbus frame*/
 
 


typedef struct 
{ 
	/* identifier CAN_xxxID */
	uint8_t  ucExtended_Identifier;  
	/* either extended (the 29 LSB) or standard (the 11 LSB) */
	uint32_t uliIdentifier;  
	/* data length: */
	uint8_t  ucDlc; 
	uint8_t  rgucData[MCP_CAN_MAX_CHAR_IN_MESSAGE]; 
	 
	/* used for receive only: 
	 * Received Remote Transfer Bit 
	 * (0=no... 1=remote transfer request received) 
	 */
	uint8_t  ucRtr;   
	/* Acceptence Filter that enabled the reception */
	uint8_t  ucFilhit; 
} MCPCANMessage_TypeDef; 



/* ----------------------------------------------------------------------------
 *                           EXTERNAL VARIABLES
 * ----------------------------------------------------------------------------
 */

/* ----------------------------------------------------------------------------
 *                           EXTERNAL FUNCTIONS
 * ----------------------------------------------------------------------------
 */


extern uint8_t fnMCP2515_Init(void);  
extern void fnMCP2515_ConfigRate(void);
extern void  fnMCP2515_ModifyRegister( uint8_t, uint8_t, uint8_t);
extern void fnMCP2515_Reset(void);
extern uint8_t fnMCP2515CANCTRLMode_Set(uint8_t);
extern  void fnSPI_Configuration(void);
extern uint8_t fnSPI_ReadWrite(SPI_TypeDef*, uint8_t);
extern uint8_t fnMCP2515Register_Read( uint8_t);
extern void fnMCP2515Register_Set(uint8_t, uint8_t);
extern void fnMCP2515CANBuffer_Init(void);
extern void  fnMCP2515CANId_Write( uint8_t, uint8_t, uint32_t);
extern void fnMCP2515Register_SetS( uint8_t, uint8_t[],  uint8_t);
extern void fnMCPCANMsgStruct_Init(MCPCANMessage_TypeDef*);
extern uint8_t fnMCPCANMsg_Send( MCPCANMessage_TypeDef*);
extern uint8_t fnMCPCANRcv_Check(void);
extern uint8_t fnMCP2515Status_Read(void);
extern uint8_t fnMCP2515NextFreeTXBuff_Get(uint8_t*);
extern void fnMCP2515CANMsg_Write(uint8_t, MCPCANMessage_TypeDef*);
extern void fnMCP2515Start_Transmit(uint8_t);
extern void fnMCPCANMsg_Process(void);
extern uint8_t fnMCPCANMsg_Read(MCPCANMessage_TypeDef*);
extern void fnMCP2515CANMsg_Read(uint8_t, MCPCANMessage_TypeDef*);
extern void fnMCP2515CANId_Read(uint8_t, uint8_t*, uint32_t*);
extern void fnMCP2515Register_ReadS( uint8_t, uint8_t[], uint8_t);
extern void fnMCPCAN2_Transmit(CanTxMsg*);




/* -----------------------------------------------------------------------------
 *                           FUNCTIONS DECLARATION
 * ----------------------------------------------------------------------------
 */


#endif /*MCP2515_H_*/


/******************************************************************************
  **@file     :    CAN.h
  * 
  * @brief  	:		 This file contains included files, Macros, Global Variables,
									 External Variables, External Functions
  ******************************************************************************
  */	
	#ifndef __Can_h
	#define __Can_h
/* ----------------------------------------------------------------------------
*                           Includes
* ----------------------------------------------------------------------------
*/
/* ----------------------------------------------------------------------------
*                           MACROS
* ----------------------------------------------------------------------------
*/
	/******CAN initialization Pins and Ports*******************/
	#define CAN_TX_PIN									GPIO_Pin_12
	#define CAN_RX_PIN									GPIO_Pin_11

	#define CAN_PORT										GPIOA
	#define RCC_APBPERIPH_GPIO					RCC_APB2Periph_GPIOA
 
	#define CAN_ENABLE_PIN							GPIO_Pin_10
	#define CAN_STROBE_PIN							GPIO_Pin_11	
	#define CAN_ERROR_PIN								GPIO_Pin_12	
	#define	CAN_ENABLE_PORT							GPIOG
	#define	CAN_STROBE_PORT							GPIOG
	#define	CAN_ERROR_PORT							GPIOG
	
	/****************CAN filter********************/
	/*Filter 0*/
	#define VCM_CAN_FILTER_0_ID					    (uint32_t)((((uint32_t)0x0500) << 3 ) | 4)

	#define VCM_CAN_FILTERIDHIGH_0					(uint16_t)((VCM_CAN_FILTER_0_ID >> 16) & 0xFFFF)
	#define VCM_CAN_FILTERIDLOW_0						(uint16_t)(VCM_CAN_FILTER_0_ID  & 0xFFFF) 

	#define CAN_FILTERMASKIDHIGH_0					(uint16_t)0x0000
	#define CAN_FILTERMASKIDLOW_0						(uint16_t)0xFF00

	/*Filter 1*/
	#define VCM_CAN_FILTER_1_ID					(uint32_t)((((uint32_t)0xFF00) << 3 ) | 4)

	#define VCM_CAN_FILTERIDHIGH_1					(uint16_t)((VCM_CAN_FILTER_1_ID >> 16) & 0xFFFF)
	#define VCM_CAN_FILTERIDLOW_1						(uint16_t)(VCM_CAN_FILTER_1_ID  & 0xFFFF) 

	#define CAN_FILTERMASKIDHIGH_1					(uint16_t)0x0000
	#define CAN_FILTERMASKIDLOW_1						(uint16_t)0xFF00

	/****************************************/
	#define CAN_MSG_RESEND_TIME					(uint16_t)25  	/*Message resend time = 500 * 10ms = 5 seconds*/
	#define CAN_MSG_RESEND_ATTEMPT			(uint8_t)2 			/*CAN message resend attempt count*/
	#define HEARTBEAT_TIMEOUT						(uint16_t)75	/*HeartBeat Timeout = 3000 * 10ms = 30 seconds*/
	
	/********* Config para Recieved Macros ********************/
	#define CONFIG_PARA_STALL_TH_DOWN 						(uint32_t)0x00000001
	#define CONFIG_PARA_STALL_TH_UP		 						(uint32_t)0x00000002
	#define CONFIG_PARA_PG1_LTH 									(uint32_t)0x00000004
	#define CONFIG_PARA_PG1_HTH 									(uint32_t)0x00000008
	#define CONFIG_PARA_PG2_LTH 									(uint32_t)0x00000010
	#define CONFIG_PARA_PG2_HTH 									(uint32_t)0x00000020
	#define CONFIG_PARA_PG3_LTH 									(uint32_t)0x00000040
	#define CONFIG_PARA_PG3_HTH 									(uint32_t)0x00000080
	#define CONFIG_PARA_CSA_A 										(uint32_t)0x00000100
  #define CONFIG_PARA_CSA_B 										(uint32_t)0x00000200
	#define CONFIG_PARA_CSA_C		 									(uint32_t)0x00000400
	#define CONFIG_PARA_NBM_CYLINDERS							(uint32_t)0x00000800
	#define CONFIG_PARA_TB_STALL									(uint32_t)0x00001000
	#define CONFIG_PARA_TB_COMPARE_STALL  				(uint32_t)0x00002000
	#define CONFIG_PARA_PGAUGE_REMAP							(uint32_t)0x00004000
	#define CONFIG_PARA_LT_REMAP		 							(uint32_t)0x00008000
	#define CONFIG_PARA_EE07_REMAP		 						(uint32_t)0x00010000
	#define CONFIG_PARA_STALL_VALVE_REMAP		 			(uint32_t)0x00020000
	#define CONFIG_PARA_VCM_PERIPH_BYPASS		 			(uint32_t)0x00040000
	/*****************************************************************/
	#define CONFIG_PARA_BYTE_VCM									(uint32_t)0x0007FFFF	
	
	#define TX_CAN_BUF_SIZE												(uint8_t)100 
	#define RX_CAN_BUF_SIZE												(uint8_t)100 
	
	#define HB_MSG																(uint32_t)(BCAST_HEARTBEAT | RESERVED | \
																								MSG_TYPE_BROADCAST | DEST_ALL | \
																								SOURCE_SCU)
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
	extern CanTxMsg rgstTxCANBuff[TX_CAN_BUF_SIZE];
	extern CanTxMsg stTxMsg;
	extern uint8_t ucStoreBuffCnt;
	extern uint8_t ucTxBuffCnt;
	extern uint32_t uliFaultId;
	extern uint32_t uliConfigParaByte;
	extern CanRxMsg rgstRxCANBuff[RX_CAN_BUF_SIZE];
	extern uint8_t ucCAN1ProcessCnt ;
	extern uint8_t ucStoreRxCANBuffCnt;
/* ----------------------------------------------------------------------------
*                           EXTERNAL FUNCTIONS
* ----------------------------------------------------------------------------
*/
	
	extern void CAN_Config(void);
	extern void CANMsg_Init(void);
	extern void fnData_Send(uint32_t);
	extern void fnAck_Send(CanRxMsg *);
	extern void fnStallData_Send(void);
	extern void fnCheck_ACK(void);
	extern void fnCheck_HeartBeat(void);
	extern void fnCAN_Transmit(CanTxMsg*);
	extern void fnBuffer_Store(CanTxMsg*);
	extern void fnValveFault_Send(void);
	extern void fnFault_Send(uint32_t);
	extern void fnStatus_Send(uint32_t);
/* ----------------------------------------------------------------------------
*                           FUNCTIONS DECLARATION
* ----------------------------------------------------------------------------
*/
	#endif



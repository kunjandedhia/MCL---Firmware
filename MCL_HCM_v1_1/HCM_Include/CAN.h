
/******************************************************************************
**@file    CAN.h
* 
**@brief   This Header File contains declaration of all the external function ,
					 Macros and External variables related to CAN Communication.
******************************************************************************
*/    
#ifndef __CAN_h
#define __CAN_h

/* ----------------------------------------------------------------------------
*                           Includes
* ----------------------------------------------------------------------------
*/
/* ----------------------------------------------------------------------------
*                           MACROS
* ----------------------------------------------------------------------------
*/
#define CAN_RX																GPIO_Pin_11
#define CAN_TX 																GPIO_Pin_12
#define CAN_GPIO															GPIOA
#define RCC_APB2PERIPH_GPIO										RCC_APB2Periph_GPIOA

//#define CAN_FILTERMASKIDHIGH		              (uint16_t)0x0000
//#define CAN_FILTERMASKIDLOW		     				 		(uint16_t)0xFF00

//#define HCB_CAN_FILTERIDHIGH									(uint16_t)0x0000
//#define HCB_CAN_FILTERIDBROADCAST 						(uint16_t)0xFF00
//#define HCB_CAN_FILTERIDHCM										(uint16_t)0x0400;


	/****************CAN filter********************/
	/*Filter 0*/
	#define HCM_CAN_FILTER_0_ID							(uint32_t)((((uint32_t)0x0400) << 3 ) | 4)

	#define HCM_CAN_FILTERIDHIGH_0					(uint16_t)((HCM_CAN_FILTER_0_ID >> 16) & 0xFFFF)
	#define HCM_CAN_FILTERIDLOW_0						(uint16_t)(HCM_CAN_FILTER_0_ID  & 0xFFFF) 

	#define CAN_FILTERMASKIDHIGH_0					(uint16_t)0x0000
	#define CAN_FILTERMASKIDLOW_0						(uint16_t)0xFF00

	/*Filter 1*/
	#define HCM_CAN_FILTER_1_ID							(uint32_t)((((uint32_t)0xFF00) << 3 ) | 4)

	#define HCM_CAN_FILTERIDHIGH_1					(uint16_t)((HCM_CAN_FILTER_1_ID >> 16) & 0xFFFF)
	#define HCM_CAN_FILTERIDLOW_1						(uint16_t)(HCM_CAN_FILTER_1_ID  & 0xFFFF) 

	#define CAN_FILTERMASKIDHIGH_1					(uint16_t)0x0000
	#define CAN_FILTERMASKIDLOW_1						(uint16_t)0xFF00




#define TX_CAN_BUF_SIZE												(uint8_t)50
#define RX_CAN_BUF_SIZE												(uint8_t)50

#define CAN_ENABLE_GPIO_PIN									GPIO_Pin_10
#define CAN_STROBE_GPIO_PIN									GPIO_Pin_11
#define CAN_ERROR_GPIO_PIN									GPIO_Pin_12
#define CAN_ENABLE_GPIO_PORT								GPIOG
#define CAN_STROBE_GPIO_PORT								GPIOG
#define CAN_ERROR_GPIO_PORT									GPIOG


/********** MACROS For Configuration Parameter Received ***********/
#define CONFIG_BTEMP_SET											(uint32_t)0x00000001
#define CONFIG_BTEMP_LOW_TH										(uint32_t)0x00000002
#define CONFIG_BTEMP_HIGH_TH									(uint32_t)0x00000004
#define CONFIG_BTEMP_HYS_POS									(uint32_t)0x00000008
#define CONFIG_BTEMP_HYS_NEG									(uint32_t)0x00000010
	
#define CONFIG_ILBTEMP_SET										(uint32_t)0x00000020
#define CONFIG_ILBTEMP_LOW_TH									(uint32_t)0x00000040
#define CONFIG_ILBTEMP_HIGH_TH								(uint32_t)0x00000080
#define CONFIG_ILBTEMP_HYS_POS								(uint32_t)0x00000100
#define CONFIG_ILBTEMP_HYS_NEG								(uint32_t)0x00000200

#define CONFIG_ILHTEMP_SET										(uint32_t)0x00000400
#define CONFIG_ILHTEMP_LOW_TH									(uint32_t)0x00000800
#define CONFIG_ILHTEMP_HIGH_TH								(uint32_t)0x00001000
#define CONFIG_ILHTEMP_HYS_POS								(uint32_t)0x00002000
#define CONFIG_ILHTEMP_HYS_NEG								(uint32_t)0x00004000

#define CONFIG_LL_BASE_LOW_TH									(uint32_t)0x00008000
#define CONFIG_LL_BASE_HIGH_TH								(uint32_t)0x00010000

#define CONFIG_LL_HARD_LOW_TH									(uint32_t)0x00020000
#define CONFIG_LL_HARD_HIGH_TH								(uint32_t)0x00040000		

#define CONFIG_BASE_TANK_HEIGHT								(uint32_t)0x00080000
#define CONFIG_HARD_TANK_HEIGHT								(uint32_t)0x00100000

#define CONFIG_LL_SAMPLING_RATE								(uint32_t)0x00200000

#define CONFIG_SSR_REMAP_RCV							  	(uint32_t)0x00400000
#define CONFIG_TC_REMAP_RCV									  (uint32_t)0x00800000
#define CONFIG_LL_REMAP_RCV									  (uint32_t)0x01000000
#define CONFIG_BYPASS_RCV											(uint32_t)0x02000000

#define CONFIG_PARA_BYTE_HCM                	(uint32_t)0x03FFFFFF     /* Total number of parameters in HCM (each bit per parameter)*/

#define HEARTBEAT_TIMEOUT						 					(uint16_t)75			/*Message resend time = 75 * 10ms = 750 miliSecond*/
#define CAN_MSG_RESEND_TIME										(uint16_t)25			/*Message resend time = 25 * 10ms = 250 miliseconds*/
#define CAN_MSG_RESEND_ATTEMPT								(uint8_t)2 


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
extern CanRxMsg rgstRxCANBuff[RX_CAN_BUF_SIZE];
extern uint16_t uiTxCounter;
extern uint8_t ucTxBuffCnt;
extern uint8_t ucStoreBuffCnt;
extern CanTxMsg stTxMsg;  /*CAN Tx msg */
extern uint16_t uiHBTimeCounter;
extern uint32_t uliConfigParaByte;
extern uint8_t ucStoreRxCANBuffCnt;
extern uint8_t ucRxCANProcessCnt;
#ifdef TEST_DBG
extern uint32_t uliCANTxCount;
extern uint32_t uliCANRxCount;
#endif
/* ----------------------------------------------------------------------------
*                           EXTERNAL FUNCTIONS
* ----------------------------------------------------------------------------
*/
/* ----------------------------------------------------------------------------
*                           FUNCTIONS DECLARATION
* ----------------------------------------------------------------------------
*/

/* ----------------------------------------------------------------------------
 *                           EXTERNAL VARIABLES
 * ----------------------------------------------------------------------------
 */
extern void fnCAN_Transmit(CanTxMsg*,CAN_TypeDef*);
extern void fnDataMsg_Tx(float,uint8_t,uint8_t);
extern void fnFaultMsg_Tx(uint8_t,uint8_t,uint8_t);
extern void fnACK_Check(void);
extern void fnBuffer_Store(CanTxMsg*);
extern void fnBuffer_Shift(void);
extern void fnCAN_Config(void);
extern void fnCAN_MsgInit(void);
extern void fnACK_Send(CanRxMsg*);
extern void fnCANMessage_Send(CanTxMsg*);

/* ----------------------------------------------------------------------------
 *                           EXTERNAL FUNCTIONS
 * ----------------------------------------------------------------------------
 */
#endif


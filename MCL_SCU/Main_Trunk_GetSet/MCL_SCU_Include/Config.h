/******************************************************************************
  **@file     :    Config.h
  * 
  * @brief   This file contains all include files,configuration parameters, flags and extern functions 
  ******************************************************************************
  */
/* ----------------------------------------------------------------------------
 *                           Includes
 * ----------------------------------------------------------------------------
 */

#include "Includes.h"
/* ----------------------------------------------------------------------------
 *                           MACROS
 * ----------------------------------------------------------------------------
 */
/*Macro to include print debug statements on Hyperterminal**/ 
#define DBG 
//#define DBG_XML 
//#define DBG_WIFI 
//#define DBG_ETH 
#define DBG_DATALOG
//#define FACTORY_RESET
/*DEBUGGUNG OF CAN ABK*/
//#define DBG_CAN
//#define DBG_CAN_PARSER
//#define DBG_CAN_PARSER_BROADCAST
//#define DBG_CAN_PARSER_COMMAND
//#define DBG_CAN_PARSER_DATA
//#define DBG_CAN_PARSER_FAULT 
//#define DBG_CAN_PARSER_STATUS
//#define DBG_CAN_PARSER_CONFIG
//#define DBG_CAN_HB
//#define FIFO_DBG

/*DEBUG USED FOR RTC ABK*/
//#define GENERAL_DBG_RTC

/*DEBUG USED FOR EEPROM ABK*/
//#define GENERAL_DBG_EE

/* DEBUG SENT CONFIG PARAMETERS ABK */
//#define DBG_CONFIG_SEND_HMI

/* DEBUG FOR DEFAULT MODE ABK*/
//#define DBG_DEFAULT_MODE
//#define DBG_DIAGNOSTIC
//#define DBG_HANDSHAKE

/* Use for Debugging NVSRAM  */
//#define DBG_CAN_EE
//#define GENERAL_DBG

//#define STATU_LOG_TEST
//#define PROCESS_DBG



/* ABK */
//#define 	DBG_PRESSURE_REGULATOR
//#define DBG_EEPROM
//#define DBG_EEPROM_READ
//#define DBG_EEPROM_WRITE
//#define DBG_NVSRAM
//#define DBG_NVSRAM_READ
//#define DBG_NVSRAM_WRITE
//#define DBG_WIFI
//#define DBG_ETHERNET
//#define DBG_RECIEVER
//#define DBG_ERROR_WARNING
//#define DBG_HOOTER
//#define DBG_LED
//#define DBG_RECIEVER
#define DBG_PROCESS

/* DATALOG DEBUG */
//#define DBG_PRIO_LOG_WRITE
//#define DBG_PRIO_LOG_READ
//#define DBG_PRD_READ
//#define DBG_DIAG_LOG_WRITE
/* Enabling this macro will write default value in LOG */
/* USED FOR TESTING */
//#define DBG_STAT_LOG_WRITE
/* Enabling this macro will write default value in LOG */
/* USED FOR TESTING */
//#define DBG_FAULT_LOG_WRITE
//#define DBG_DIAG_LOG_READ
//#define DBG_STAT_LOG_READ

//#define DBG_FAULT_LOG_READ

/*WARNING DEBUG*/
//#define DBG_DIAG_WARN_LOGIC
//#define DBG_STATUS_WARN_LOGIC
//#define DBG_FAULT_WARN_LOGIC
//#define DBG_PERIODIC_WARN_LOGIC

/* code version changes with addition of features in datalogging v1.1 */
#define SCU_FIRMWARE_VERSION			4.1f  //change14 

/* For USART6 is used for Debugging of SCU */
#define USART6_Init


#define CHECK_BIT(value,position) 				(value&(1<<position))
#define SetBit(value,position)   					(value|=(1<<position))
#define ClearBit(value,position) 					(value&=(~(1<<position)))
#define ToggleBit(value,position)   			(value^=(1<<position))

/****INITIALIZATION OF PERIPHERALS MACROS****/
/* CAN GPIO */
#define CAN1_ERROR_PIN												GPIO_Pin_9
#define CAN1_ERROR_PORT									  		GPIOC
#define CAN1_STROBE_PIN												GPIO_Pin_6
#define CAN1_STROBE_PORT									  	GPIOD
#define CAN1_ENABLE_PIN												GPIO_Pin_12
#define CAN1_ENABLE_PORT									  	GPIOG

#define CAN2_ERROR_PIN												GPIO_Pin_2
#define CAN2_ERROR_PORT											  GPIOE
#define CAN2_STROBE_PIN												GPIO_Pin_10
#define CAN2_STROBE_PORT									  	GPIOF
#define CAN2_ENABLE_PIN												GPIO_Pin_7
#define CAN2_ENABLE_PORT									  	GPIOD

/* RECIEVER INPUT GPIO */
#define PRIME_SELECT_PIN											GPIO_Pin_15
#define PRIME_SELECT_PORT											GPIOH
#define FLUSH_SELECT_PIN											GPIO_Pin_8
#define FLUSH_SELECT_PORT											GPIOH
#define SPRAY_SELECT_PIN											GPIO_Pin_11
#define SPRAY_SELECT_PORT											GPIOH
#define READ_PIPE_TEMP_SELECT_PIN							GPIO_Pin_14
#define READ_PIPE_TEMP_SELECT_PORT						GPIOH
#define START_SELECT_PIN											GPIO_Pin_6
#define START_SELECT_PORT											GPIOI
#define FLUSH_COMPLETE_SELECT_PIN							GPIO_Pin_7
#define FLUSH_COMPLETE_SELECT_PORT						GPIOI
#define ESTOP_SIGNAL_SELECT_PIN								GPIO_Pin_4
#define ESTOP_SIGNAL_SELECT_PORT							GPIOH
#define START_SIGNAL_SELECT_PIN								GPIO_Pin_0
#define START_SIGNAL_SELECT_PORT							GPIOI


/* EPPROM CHIP SELECT GPIO */
#define EEPROM_CHIP_SELECT										GPIO_Pin_2	
#define EEPROM_CHIP_SELECT_PORT 							GPIOH

/* WIFI RESET GPIO */
#define WIFI_RESET_PIN												GPIO_Pin_8	
#define WIFI_RESET_PORT 											GPIOF

#define WIFI_FORCE_ON_PIN												GPIO_Pin_7	
#define WIFI_FORCE_ON_PORT 											GPIOG

/* HOOTER SELECT GPIO */
#define HOOTER_SELECT													GPIO_Pin_1	
#define HOOTER_SELECT_PORT 										GPIOB

/* UPS FEEDBACK & OUTPUT GPIO */
#define UPS_GPIO_OUT_PIN											GPIO_Pin_12
#define UPS_GPIO_OUT_PORT											GPIOC
#define UPS_GPIO_IN_PIN												GPIO_Pin_2
#define UPS_GPIO_IN_PORT											GPIOD

/*SPI2 USED FOR NVSRAM AND EEPROM*/ 
#define SPIX_PERIPH                    	SPI2
#define SPIx_CLK                       	RCC_APB1Periph_SPI2
#define SPIx_CLK_INIT                  	RCC_APB1PeriphClockCmd
#define SPIx_IRQn                      	SPI2_IRQn
#define SPIx_IRQHANDLER                	SPI2_IRQHandler

#define SPIx_SCK_PIN                   	GPIO_Pin_3
#define SPIx_SCK_GPIO_PORT             	GPIOD
#define SPIx_SCK_GPIO_CLK              	RCC_AHB1Periph_GPIOD
#define SPIx_SCK_SOURCE                	GPIO_PinSource3
#define SPIx_SCK_AF                    	GPIO_AF_SPI2

#define SPIx_MISO_PIN                  	GPIO_Pin_2
#define SPIx_MISO_GPIO_PORT            	GPIOI
#define SPIx_MISO_GPIO_CLK             	RCC_AHB1Periph_GPIOI
#define SPIx_MISO_SOURCE               	GPIO_PinSource2
#define SPIx_MISO_AF                   	GPIO_AF_SPI2

#define SPIx_MOSI_PIN                 GPIO_Pin_15
#define SPIx_MOSI_GPIO_PORT           GPIOB
#define SPIx_MOSI_GPIO_CLK            RCC_AHB1Periph_GPIOB
#define SPIx_MOSI_SOURCE              GPIO_PinSource15
#define SPIx_MOSI_AF                  GPIO_AF_SPI2

/* PRESSURE REGULATOR */

#define ADC_PERIPHERAL								ADC3	
#define PR_ADC_PIN										GPIO_Pin_0
#define PR_ADC_PORT										GPIOA

#define ADC_GPIO_CLOCK								RCC_AHB1Periph_GPIOA
#define ADC_CLOCK											RCC_APB2Periph_ADC3

#define PRESSURE_REG_ADC_CH						ADC_Channel_0		
#define LOW_PRESSURE_LEVEL						0.7f   			 /* Pressure value in psi*/
#define HIGH_PRESSURE_LEVEL						130.0f
#define LOW_PRESSURE_V								0.6f	/* Voltage corresponds to Low pressure value*/
#define HIGH_PRESSURE_V								3.0f	/* Voltage corresponds to High pressure value*/
#define PRESSURE_REG_SLOPE_ADC 				((HIGH_PRESSURE_LEVEL - LOW_PRESSURE_LEVEL)/\
																			 (HIGH_PRESSURE_V - LOW_PRESSURE_V))

#define PSI_TO_BAR(CURRENT_PRESSURE)	(float)(CURRENT_PRESSURE * 0.068947f)	
#define BAR_TO_PSI(CURRENT_PRESSURE)	(float)(CURRENT_PRESSURE * 14.5038f)  

#define LOW_CURRENT_VALUE							4.0f
#define HIGH_CURRENT_VALUE						20.0f
#define PRESSURE_REG_SLOPE_DAC 				((HIGH_CURRENT_VALUE - LOW_CURRENT_VALUE)/\
																			(HIGH_PRESSURE_LEVEL - LOW_PRESSURE_LEVEL))

#define ACK_FOR_MEMORY_FULL							(uint8_t)0xF0
/* Process Related Macros */
//#define DEBOUNCINGCNT   				(uint8_t)35
/*change52*/
#define DEBOUNCINGCNT   				(uint8_t)10
#define PRIMEPROCESS   					(uint8_t)0x01
#define SPRAYPROCESS   					(uint8_t)0x02
#define FLUSHPROCESS   					(uint8_t)0x03
#define MATERIAL_DATA_RECEIVED	(uint8_t)0x07
#define LEG_A_DATA							(uint8_t)0x01
#define LEG_B_DATA							(uint8_t)0x02
#define LEG_C_DATA							(uint8_t)0x04

/********************************************/
/* In while loop at a time message are processed */ 
#define MSG_PROCESS_CNT								(uint8_t)15
/* Bit position used for heartbeat ack fail and message ack fail*/ 
#define HMI														(uint8_t)0
#define OPMM													(uint8_t)1
#define VCM				  									(uint8_t)2
#define HCM														(uint8_t)3
#define SCU 													(uint8_t)4
#define ALL_SLAVE_DEVICE    					 ((uint8_t)((1 << HMI) | (1 << OPMM) | (1 << VCM) | (1 << HCM)))
																			 
#define ALL_SLAVE_DEVICE_CONFIG_SEND   ((uint8_t)((1 << HMI) | (1 << OPMM) | (1 << VCM) | (1 << HCM)))

/* Total number of Devices in system in which HB communication occurs*/
#define TOTAL_HB_DEVICE  								(uint8_t)4
/*Total 3 times heartbeat is send befaore declaring fault*/
#define HB_RESND_CNT										(uint8_t)2

#define ONE_BYTES												(uint8_t)1
#define TWO_BYTES												(uint8_t)2
#define THREE_BYTES											(uint8_t)3
#define FOUR_BYTES											(uint8_t)4
#define FIVE_BYTES											(uint8_t)5
#define SIX_BYTES												(uint8_t)6
#define SEVEN_BYTES											(uint8_t)7
#define EIGHT_BYTES											(uint8_t)8

#define DATA_BYTE_ZERO 											(uint8_t)0
#define DATA_BYTE_ONE  											(uint8_t)1
#define DATA_BYTE_TWO  											(uint8_t)2
#define DATA_BYTE_THREE 										(uint8_t)3
#define DATA_BYTE_FOUR  										(uint8_t)4
#define DATA_BYTE_FIVE  										(uint8_t)5
#define DATA_BYTE_SIX	  										(uint8_t)6
#define DATA_BYTE_SEVEN	  									(uint8_t)7

#define BIT0_SET                     			 	(uint8_t)0x01
#define BIT0_RESET                    			(uint8_t)0x00   

#define BIT1_SET                      			(uint8_t)0x02
#define BIT1_RESET                    			(uint8_t)0x00   

#define BIT2_SET                      			(uint8_t)0x04
#define BIT2_RESET                      		(uint8_t)0x00   

#define BIT3_SET                      			(uint8_t)0x08
#define BIT3_RESET                    			(uint8_t)0x00   

#define BIT4_SET                      			(uint8_t)0x10
#define BIT4_RESET                    			(uint8_t)0x00   

#define BIT5_SET                      			(uint8_t)0x20
#define BIT5_RESET                    			(uint8_t)0x00   

#define BIT6_SET                      			(uint8_t)0x40
#define BIT6_RESET                    			(uint8_t)0x00   

#define BIT7_SET                      			(uint8_t)0x80
#define BIT7_RESET                    			(uint8_t)0x00   
#define BIT8_SET                     			 	(uint16_t)0x0100
#define BIT8_RESET                    			(uint16_t)0x0000   

#define BIT9_SET                      			(uint16_t)0x0200
#define BIT9_RESET                    			(uint16_t)0x0000   

#define BIT10_SET                      			(uint16_t)0x0400
#define BIT10_RESET                      		(uint16_t)0x0000   

#define BIT11_SET                      			(uint16_t)0x0800
#define BIT11_RESET                    			(uint16_t)0x0000   

#define BIT12_SET                      			(uint16_t)0x1000
#define BIT12_RESET                    			(uint16_t)0x0000   

#define BIT13_SET                      			(uint16_t)0x2000
#define BIT13_RESET                    			(uint16_t)0x0000   

#define BIT14_SET                      			(uint16_t)0x4000
#define BIT14_RESET                    			(uint16_t)0x0000   

#define BIT15_SET                      			(uint16_t)0x8000
#define BIT15_RESET                    			(uint16_t)0x0000   

#define BIT0_MASK              				      (uint8_t)0x01
#define BIT1_MASK               		  	    (uint8_t)0x02
#define BIT2_MASK             			        (uint8_t)0x04
#define BIT3_MASK                  				  (uint8_t)0x08
#define BIT4_MASK                 			    (uint8_t)0x10
#define BIT5_MASK                 			    (uint8_t)0x20
#define BIT6_MASK                 		      (uint8_t)0x40
#define BIT7_MASK                				    (uint8_t)0x80

#define BIT8_MASK              				      (uint16_t)0x0100
#define BIT9_MASK               		  	    (uint16_t)0x0200
#define BIT10_MASK             			        (uint16_t)0x0400
#define BIT11_MASK                  				(uint16_t)0x0800
#define BIT12_MASK                 			    (uint16_t)0x1000
#define BIT13_MASK                 			    (uint16_t)0x2000
#define BIT14_MASK                 		      (uint16_t)0x4000
#define BIT15_MASK                				  (uint16_t)0x8000

#define BYTE_LOW 					0x00
#define BYTE_HIGH 				0x01

#define BYTE_DCLS					0x01
#define BYTE_HLS					0x02

#define FRAME1						0x01
#define FRAME2						0x02
#define FRAME3						0x03

#define CYLINDER_A        0x01
#define CYLINDER_B        0x02
#define CYLINDER_C        0x04

#define BASE_HYS_POS										(uint8_t)0x01
#define BASE_HYS_NEG										(uint8_t)0x02
#define HARD_HYS_POS										(uint8_t)0x04
#define HARD_HYS_NEG										(uint8_t)0x08
#define ANTIFREEZ_HYS_POS								(uint8_t)0x10
#define ANTIFREEZ_HYS_NEG								(uint8_t)0x20

/*Interval to send HB its 250 ms */
#define HB_SND_TIME											25   /*with 10ms timer, 10* 25 = 250msec*/


/* Use to set timer for 10 ms*/
#define TIM1_PRESCALER								(uint16_t)18000
#define TIM1_PERIOD									  (uint16_t)100
#define TIM2_PRESCALER								(uint16_t)9000
#define TIM2_PERIOD									  (uint16_t)100 
 


#define PROCESS_CAN_MSG			 					(uint8_t)0x00
#define LEGA													(uint8_t)0x01
#define LEGB													(uint8_t)0x02
#define LEGC													(uint8_t)0x04
#define STALL_RESULT_RECIEVED					(uint8_t)0x07 /*change71*/						


/*ideally this delay should be there - 250ms*/
#define ACK_TIMEOUT_DELAY                     50



#define DUMMY_DATA_8BIT												(uint8_t)0xFF
#define SCU_DEFAULT_MODE                      (uint8_t)0x00
#define SCU_OPERATION_MODE                    (uint8_t)0x01


#define DUMMY1																(uint16_t)0x1234
#define DUMMY2																(uint16_t)0x5678
#define NVSRAM1_START												  (DL_STA_SEG_END - 3)/* in the NVSRAM1 free space*/
#define NVSRAM2_START												  (DL_PRD_RAM2_END - 3)/*At the end of NVSRAM2*/
#define EEPROM_TEST_ADD												(PAGE0_START + 6)
#define TEST_BYTES_COUNT											(uint8_t)1


#define EEPROM_DIAGNOSTIC_FAIL								(uint8_t)0x01
#define NVSRAM_DIAGNOSTIC_FAIL								(uint8_t)0x02
#define PRESSURE_REG_DIAGNOSTIC_FAIL					(uint8_t)0x04
#define THERMOCOUPLE_DIAGNOSTIC_FAIL	 				(uint8_t)0x08
#define TEMP_HUMI_DIAGNOSTIC_FAIL	 						(uint8_t)0x10
#define PRESSURE_REG_LOWER_READ								0.6f
#define PRESSURE_REG_HIGHER_READ							3.0f


#define DIAGNOSTIC_FAIL												(uint8_t)0x01
#define DIAGNOSTIC_PASS												(uint8_t)0x02

/*Start up delay approx 5 sec for testing*/
#define STARTUP_DELAY_SCU								(uint8_t)25	
#define STARTUP_DELAY_SCU1								(uint8_t)50		
/*DIAGNOSTIC_TIMER_OVER approx 3 sec for testing*/
#define DIAGNOSTIC_TIMER_OVER									(uint16_t)300			

#define STATUS_LED_BLINK_COUNT								(uint8_t)30
#define RTC_DATA_SEND_TIME										(uint16_t)100
#define RETURNLINE_TEMP_READ_TIME							(uint16_t)100


#define PRESSURE_REG_READ_TIME 								(uint16_t)100


#define INVALID_FLOAT_READ										2999.0f
#define NaN																		0x7FC00000

#define CAN_FAULT_COUNT												(uint8_t)4
#define WRONG_HUMIDITY												INVALID_FLOAT_READ
#define WRONG_AMBIENT_TEMP										INVALID_FLOAT_READ


#define INSIDE_AMBIANT_READ_TIME							(uint16_t)8000	/*80 sec Delay*/
#define OUTSIDE_AMBIANT_READ_TIME							(uint16_t)4000	/*80 sec Delay */

#define CONVR_NUMERATOR            	 					3.3f 
#define CONVR_DENOMINATOR          						4096.0f 

#define ID_HMI 																(uint8_t)0x01  
#define ID_SCU																(uint8_t)0x02
#define ID_OPMM																(uint8_t)0x03
#define ID_HCM																(uint8_t)0x04
#define ID_VCM																(uint8_t)0x05



#define MULTIPLICATION_VALUE 			720   /* at 4mA we Get a 720 Value(0 - 4096)*/
#define DIVISION_VALUE						4			/* This is a 4mA.*/

/*Macros for the CAN msg Transmit Status*/
#define DEVICE_DEAD												(uint8_t)0
#define MSG_STORE_IN_FIFO									(uint8_t)1
#define FIFO_FULL_MSG_DISCARDED						(uint8_t)2
#define MSG_TRANSMITTING									(uint8_t)3
#define WRONG_DESTINATION									(uint8_t)4

#define LOW_TH												(uint8_t)0x00
#define HIGH_TH												(uint8_t)0x01

#define BHCM_TANK_HEIHGT							(uint8_t)0x01
#define HHCM_TANK_HEIHGT							(uint8_t)0x02


#define DOWNSTROKE										(uint8_t)0x01
#define UPSTROKE											(uint8_t)0x02
#define DOWNSTROKE_TH									(uint8_t)0x00
#define UPSTROKE_TH										(uint8_t)0x01


#define PIPE_POSITION_12							(uint8_t)0x01
#define PIPE_POSITION_3								(uint8_t)0x02
#define PIPE_POSITION_6								(uint8_t)0x04
#define PIPE_POSITION_9								(uint8_t)0x08
#define AVG_TEMP											(uint8_t)0x10

#define PRIME_PROCESS									(uint8_t)0x01
#define SPRAY_PROCESS									(uint8_t)0x02
#define FLUSH_PROCESS									(uint8_t)0x04


#define FRAME_1												(uint8_t)0x01
#define FRAME_2												(uint8_t)0x02
#define FRAME_3												(uint8_t)0x04

#define DCLS_TO_HOME_TIME							(uint8_t)0x01
#define HOME_TO_DCLS_TIME							(uint8_t)0x02

/*Data byte meaning in ACK frame */
#define POSITIVE_ACK								(uint8_t)0x00
#define NEGATIVE_ACK								(uint8_t)0x01	
#define FIRST_BOOTUP_ACK						(uint8_t)0x02

#define RX_CAN_BUF_SIZE							(uint16_t)500 	

/* Warm up mode & operation mode */
#define CAN_MSG_WARM_UP_MODE								(uint8_t)0x01
#define CAN_MSG_OPERATION_MODE     					(uint8_t)0x02	


#define CMD_PAUSE_OPERATION         				(uint8_t)0x00
#define CMD_START_OPERATION         				(uint8_t)0x01

#define FIFO_DEPTH  												(uint8_t)150 //change20 was 100

/*Used for received data frame 0-7 total 8 bytes*/
#define MAX_DATA_LENGTH 										(uint8_t)8



#define AUTO_PROCESS                 				(uint8_t)0x00
#define MANUAL_PROCESS               				(uint8_t)0x02

/*Used to update the byte ucStatusLog*/
#define UPSTROKE_STALL_PASS									(uint8_t)0x01
#define UPSTROKE_STALL_FAIL									(uint8_t)0xFE

#define DOWNSTROKE_STALL_PASS								(uint8_t)0x02
#define DOWNSTROKE_STALL_FAIL								(uint8_t)0xFD

#define SYSTEM_PRIMED												(uint8_t)0x04
#define SYSTEM_FLUSH												(uint8_t)0x08


#define DCLS_TIMER_FAULT										((uint8_t)0x01)		//OPMM DCLS timer fault
#define HLS_TIMER_FAULT											((uint8_t)0x02)		//OPMM HLS timer fault

#define MIN_READING_RANGE										-45.0f					
#define MAX_READING_RANGE										-250.0f					

#define FLUSH_COMPLETE											(uint8_t)0x04
#define PRDSPRAY														(uint8_t)0x01 //change15
#define PRDSTALL														(uint8_t)0x02 //change15
#define IDLEPERIOD													(uint8_t)30 //change15

/*change66*/
#define MM3_TO_OZ_FACTOR										(float)0.000033814f
#define MM3_TO_L_FACTOR											(float)0.000001f
#define CONVERT_MM3_TO_OZ(CURR_VALUE)				(float)(CURR_VALUE * MM3_TO_OZ_FACTOR)
#define CONVERT_MM3_TO_L(CURR_VALUE)				(float)(CURR_VALUE * MM3_TO_L_FACTOR)
	

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




/* Structure storing msg frame and CAN device info */
typedef struct
{
 CanTxMsg rgFifo[FIFO_DEPTH];/*Array of CanTxMSg*/
}stTxMsg;

/* Tx FIFO used in ACK protocol definition */ 
typedef struct
{
  stTxMsg stMsg;
	uint8_t bFifoFull:1;
	uint8_t bFifoEmpty:1;
	uint8_t bFiFullFlt:1;
	uint8_t bFiEmpFlt:1;
	uint8_t ucWriteCnt;
	uint8_t ucReadCnt;
 }stTxMsgFlags;

 
/* structure storing current msg frame and id read from fifo */

typedef struct
{
 CanTxMsg CanMsgRd;     /*MsgTrans - current message under transmission*/
 CAN_TypeDef* CanDevRd; /*CAN bus to which current msg is to be tranmitte*/
}stCANTempStr;

/*******************************************************************************/
extern CanRxMsg rgstRxCANBuff[RX_CAN_BUF_SIZE];
extern uint16_t uiStoreRxCANBuffCnt;
extern uint16_t uiCANProcessCnt;


extern  CanTxMsg stTx1Message;

/*OP Device related Flags */
extern uint8_t ucMsgTxCnt_HMI;      /*msg transmit counter*/
extern uint32_t uliTimeoutCnt_HMI;  /*250ms timeout counter*/
extern uint8_t ucTimerEn_HMI;       /*Timeout Counter enable*/
extern uint8_t ucDevDead_HMI;       /*Device dead signal*/
extern stTxMsgFlags stFifo_HMI;    /*FIFO Structure*/
extern CanTxMsg stCurMsgTrans_HMI; /*MsgTrans - current message under transmission*/

/*OPMM Device related Flags */
extern uint8_t ucMsgTxCnt_OPMM;      /*msg transmit counter*/
extern uint32_t uliTimeoutCnt_OPMM;  /*250ms timeout counter*/
extern uint8_t ucTimerEn_OPMM;       /*Timeout Counter enable*/
extern uint8_t ucDevDead_OPMM;       /*Device dead signal*/
extern stTxMsgFlags stFifo_OPMM;    /*FIFO Structure*/
extern CanTxMsg stCurMsgTrans_OPMM; /*MsgTrans - current message under transmission*/

/*MFM Device related Flags */
extern uint8_t ucMsgTxCnt_VCM;      /*msg transmit counter*/
extern uint32_t uliTimeoutCnt_VCM;  /*250ms timeout counter*/
extern uint8_t ucTimerEn_VCM;       /*Timeout Counter enable*/
extern uint8_t ucDevDead_VCM;       /*Device dead signal*/
extern stTxMsgFlags stFifo_VCM;    /*FIFO Structure*/
extern CanTxMsg stCurMsgTrans_VCM; /*MsgTrans - current message under transmission*/


/*BHCM Device related Flags */
extern uint8_t ucMsgTxCnt_HCM;      /*msg transmit counter*/
extern uint32_t uliTimeoutCnt_HCM;  /*250ms timeout counter*/
extern uint8_t ucTimerEn_HCM;       /*Timeout Counter enable*/
extern uint8_t ucDevDead_HCM;       /*Device dead signal*/
extern stTxMsgFlags stFifo_HCM;    /*FIFO Structure*/
extern CanTxMsg stCurMsgTrans_HCM; /*MsgTrans - current message under transmission*/




/* Structure for Current Data ABK*/
typedef struct
{	
/***********************************************/
/*        OPMM RELATED DATA VARIABLES   */
float 		fCurr_PipeTempPos12;
float 		fCurr_PipeTempPos3;
float 		fCurr_PipeTempPos6;
float 		fCurr_PipeTempPos9;
float 		fCurr_PipeTempAvg;
float 		fCurr_BatteryStatus;
float 		fCurr_PrimeTime;
float 		fCurr_SprayTime;
float 		fCurr_FlushTime;
uint8_t 	ucCurr_NumberofRotation;
float 		fCurr_OPMMBaseTemp;
float 		fCurr_OPMMHardTemp;
float 		fCurr_CAN1ALinkFault;
float 		fCurr_CAN1BLinkFault;
float 		fCurr_AIRFlow;	

/***********************************************/
/*        VCM RELATED DATA VARIABLES   */
float 		fCurr_DewPoint;
float 		fCurr_InsideAmbientTemp;
float 		fCurr_OutsideAmbientTemp;
float 		fCurr_OutsideAmbientHumidity;
float 		fCurr_LegAMaterialUsed;
float 		fCurr_LegBMaterialUsed;
float 		fCurr_LegCMaterialUsed;
int16_t 	iCurr_PressureLegADown;
int16_t 	iCurr_PressureLegBDown;
int16_t 	iCurr_PressureLegCDown;
int16_t 	iCurr_PressureLegAUp;
int16_t 	iCurr_PressureLegBUp;
int16_t 	iCurr_PressureLegCUp;
float 		fCurr_PressureGauge1;
float 		fCurr_PressureGauge2;
float 		fCurr_PressureGauge3;
float 		fCurr_LinearTransducer;
/***********************************************/
/*        HCM RELATED DATA VARIABLES   */

float 		fCurr_InlineBaseMatTemp;
float 		fCurr_InlineHardMatTemp;
float 		fCurr_AntifreezeLiquidTemp;
float 		fCurr_ReturnLineTemp;
float 		fCurr_LiquidLevelBase;
float 		fCurr_LiquidLevelHard;

/***********************************************/
/*        Material Usage RELATED DATA VARIABLES   */
float			fCurr_BaseMaterialPrimed;
float			fCurr_HardMaterialPrimed;
float			fCurr_BaseMaterialApplied;
float			fCurr_HardMaterialApplied;
float			fCurr_BaseTotalMaterialUsed;
float			fCurr_HardTotalMaterialUsed;

/*change36*/
float			fCurr_BaseMaterialPrimedAccumulate;
float			fCurr_HardMaterialPrimedAccumulate;
float			fCurr_BaseMaterialAppliedAccumulate;
float			fCurr_HardMaterialAppliedAccumulate;
float			fCurr_BaseTotalMaterialUsedAccumulate;
float			fCurr_HardTotalMaterialUsedAccumulate;
/*change14*/
float			fCurr_HCM_Firmware_Ver;
float			fCurr_VCM_Firmware_Ver;
float			fCurr_HMI_Firmware_Ver;
float			fCurr_OPMM_Firmware_Ver;

float    fCurr_PressureRegRead;


}Typedef_CurrData;


/* Structure for Configuration Parameters*/
typedef struct
{	
/*        SCU RELATED CONFIG VARIABLES   */
float 		fConfig_SprayPressure; 
float 		fConfig_SprayPressureLTH;
float 		fConfig_SprayPressureHTH;
float 		fConfig_HysSprayPresPos;
float 		fConfig_HysSprayPresNeg;	
float 		fConfig_RecirPressure;	
float 		fConfig_RecirPressureLTH;
float 		fConfig_RecirPressureHTH;
float 		fConfig_HysRecircPresPos;
float 		fConfig_HysRecircPresNeg;	
float 		fConfig_JointNum;	
float 		fConfig_DataLoggingPeriod;
uint8_t 	ucConfig_SystemTime[7];
uint8_t 	rgucConfig_SupervisorName1[8]; 
uint8_t 	rgucConfig_SupervisorName2[8]; 
uint8_t 	rgucConfig_SupervisorName3[8]; 
uint8_t 	rgucConfig_CoatingMatName1[8]; 
uint8_t 	rgucConfig_CoatingMatName2[8]; 
uint8_t 	rgucConfig_CoatingMatName3[8]; 
uint8_t 	rgucConfig_BaseMatBatchNo1[8]; 
uint8_t 	rgucConfig_BaseMatBatchNo2[8]; 
uint8_t 	rgucConfig_BaseMatBatchNo3[8]; 
uint8_t 	rgucConfig_HardMatBatchNo1[8]; 
uint8_t 	rgucConfig_HardMatBatchNo2[8]; 
uint8_t 	rgucConfig_HardMatBatchNo3[8]; 
uint8_t		ucConfig_TypeofUnit;
uint8_t		ucConfig_SCUBypass;
uint8_t		ucConfig_Temp_Press_Achieve_Bypass;
float 		fConfig_TimeBeforeSendingStallcommand;  // change7 change8
uint16_t  uiConfig_Signature;

float   	fConfig_IdleTimePeriodicLog;  //change21
float   	fConfig_PipeDiameter;    //change21
uint8_t 	rgucConfig_JobName1[8]; 	//change21
uint8_t 	rgucConfig_JobName2[8]; 		//change21
uint8_t 	rgucConfig_JobName3[8]; 		//change21

uint8_t 	rgucConfig_JobId1[8];  //change42
uint8_t 	rgucConfig_JobId2[8];   //change42
uint8_t 	rgucConfig_JobId3[8];   //change42
/***********************************************/
/*        OPMM RELATED CONFIG VARIABLES   */

float 		fConfig_PipeTempLTH;				
float 		fConfig_PipeTempHTH;				
uint8_t 	ucConfig_SprayCount;				
float 		fConfig_PrimeTime;					
float 		fConfig_GunDelayTime;				
float 		fConfig_SprayOverlapTime;		
float 		fConfig_ReverseDelayTime;		
float 		fConfig_FlushTime;	
float 		fConfig_DCLSTime;
float 		fConfig_HOMETime;
float 		fConfig_GunPosChangeTime;	
float 		fConfig_TimebeforeONSprayValve;
float 		fConfig_FlushWarnTime;
uint8_t 	ucConfig_ConnSprayvalve;
uint8_t 	ucConfig_ConnGunposition;
uint8_t 	ucConfig_ConnABvalve;
uint8_t 	ucConfig_ConnSolventvalve;
uint8_t 	ucConfig_ConnForwardvalve;
uint8_t 	ucConfig_ConnBackwardvalve;
uint8_t 	ucConfig_ConnNonContactat12;
uint8_t 	ucConfig_ConnNonContactat3;
uint8_t 	ucConfig_ConnNonContactat6;
uint8_t 	ucConfig_ConnNonContactat9;
uint8_t 	ucConfig_BuzzerVolume;
uint16_t 	uiConfig_OPMMBypass;
float			fConfig_TimeStopOPMMcommand;   //change8    
float   	fConfig_BatteryKneeVoltage; //change8
/***********************************************/
/*        VCM RELATED CONFIG VARIABLES   */
int16_t 	iConfig_UpStrokeTH;
int16_t 	iConfig_DownStrokeTH;
float 		fConfig_PressureGauge1LTH;
float 		fConfig_PressureGauge1HTH;
float 		fConfig_PressureGauge2LTH;
float 		fConfig_PressureGauge2HTH;
float 		fConfig_PressureGauge3LTH;
float 		fConfig_PressureGauge3HTH;
float 		fConfig_AreaofCylinderA;
float 		fConfig_AreaofCylinderB;
float 		fConfig_AreaofCylinderC;
uint8_t 	ucConfig_CylinderCount;
float 		fConfig_TimeBeforeStall;
float 		fConfig_TimeCompareStall;
uint8_t 	ucConfig_ConnPressureGuage1;
uint8_t 	ucConfig_ConnPressureGuage2;
uint8_t 	ucConfig_ConnPressureGuage3;
uint8_t 	ucConfig_ConnLinearTransducer;
uint8_t 	ucConfig_ConnInsideEE07;
uint8_t 	ucConfig_ConnOutsideEE07;
uint8_t 	ucConfig_ConnStallvalve;
uint8_t 	ucConfig_ConnRecirculationvalve;/*change53*/
uint8_t 	ucConfig_VCMBypass;
uint8_t		ucConfig_UpstrokeUpLimit;  	// change10
uint8_t		ucConfig_UpstrokeMaxRange;  // change10
uint8_t		ucConfig_UpstrokeMinRange;  	// change10
uint8_t		ucConfig_DownstrokeUpLimit;  // change10
uint8_t		ucConfig_DownstrokeMaxRange;  // change10
uint8_t		ucConfig_DownstrokeMinRange;  // change10
/***********************************************/
/*        HCM RELATED CONFIG VARIABLES   */

float 		fConfig_AntiFrzTempSet;
float 		fConfig_AntiFrzTempLTH;
float 		fConfig_AntiFrzTempHTH;
float 		fConfig_HysAntiFrzPos;
float 		fConfig_HysAntiFrzNeg;
float 		fConfig_InlineBaseTempSet;
float 		fConfig_InlineBaseTempLTH;
float 		fConfig_InlineBaseTempHTH;
float 		fConfig_HysInlineBasePos;
float 		fConfig_HysInlineBaseNeg;
float 		fConfig_InlineHardTempSet;
float 		fConfig_InlineHardTempLTH;
float 		fConfig_InlineHardTempHTH;
float 		fConfig_InlineHysHardPos;
float 		fConfig_InlineHysHardNeg;
float 		fConfig_BaseLiquidLevelLTH;
float 		fConfig_BaseLiquidLevelHTH;
float 		fConfig_HardLiquidLevelLTH;
float 		fConfig_HardLiquidLevelHTH;
float 		fConfig_BaseTankHeight;
float 		fConfig_HardTankHeight;
uint16_t 	uiConfig_LLSamplingRate;


uint8_t 	ucConfig_ConnBaseinlinether;
uint8_t 	ucConfig_ConnHardinlinether;
uint8_t 	ucConfig_ConnAntifreezether;
uint8_t 	ucConfig_ConnBasereturnlinether;
uint8_t 	ucConfig_ConnBaseinlineheater;
uint8_t 	ucConfig_ConnHardinlineheater;
uint8_t 	ucConfig_ConnAntifreezeheater;
uint8_t 	ucConfig_ConnHardheater;
uint8_t 	ucConfig_ConnBasetankLL;
uint8_t 	ucConfig_ConnHardtankLL;
uint16_t 	uiConfig_HCMBypass;
/*change29*/
uint8_t 	rgucConfig_WifiSSID1[8]; 
uint8_t 	rgucConfig_WifiSSID2[8]; 
uint8_t 	rgucConfig_WifiPASS1[8]; 
uint8_t 	rgucConfig_WifiPASS2[8]; 
/*change37*/
uint8_t 	ucConfig_TimeOffsetSign;	
uint8_t 	ucConfig_TimeOffsetHour;
uint8_t 	ucConfig_TimeOffsetMin;
uint8_t 	ucConfig_TimeOffsetSec;
}TypeDef_Config;




/* Structure for Configuration Parameters*/
typedef struct
{	
	uint8_t bRecievedPrimeSelector:1;
	uint8_t bRecievedSpraySelector:1;
	uint8_t bRecievedFlushSelector:1;
	uint8_t bRecievedFlushComplete:1;
	uint8_t bRecievedStart:1;
	uint8_t bRecievedReadPipeTemp:1;
	uint8_t bRecievedEstop:1;
}TypeDef_Reciever;

typedef union
{
	TypeDef_Reciever stRecieverFlag;
	uint8_t ucRecieveFlag;
	
}TypeDef_RecievedCommand;

typedef struct
{
		uint8_t bPrimeSelect:1;
		uint8_t bSpraySelect:1;
		uint8_t bFlushSelect:1;
		uint8_t bFlushComplete:1;
		uint8_t bStartProcess:1;
		uint8_t bReadPipeTempComm:1;
		uint8_t bReadPipeTempRecieveData:1;
		uint8_t bEstopSignal:1;
}TypeDef_ProcessCommand;

typedef union
{
	TypeDef_ProcessCommand stProcessFlag;
	uint8_t ucProcessFlag;
	
}TypeDef_ProcessedCommand;

/* Union for Split the Float and unt32_t data */
typedef union
{
	float fData;
	uint8_t ucData;
	uint16_t uiData;
	int16_t iData;
	uint32_t uliData;
	uint8_t rgucData[4];
}SplitByte_TypeDef;

/*change27 warning for 80% full memory*/
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

/* change27 Structure for Log Warning Counters*/
typedef struct
{	
uint16_t 	uiWarnLog_DiagTimerCounter;
uint8_t 	ucWarnLog_DiagSendWarnCounter;	
uint16_t 	uiWarnLog_StatusTimerCounter;
uint8_t 	ucWarnLog_StatusSendWarnCounter;
uint16_t 	uiWarnLog_FaultTimerCounter;
uint8_t 	ucWarnLog_FaultSendWarnCounter;
uint16_t 	uiWarnLog_PeriodicTimerCounter;
uint8_t 	ucWarnLog_PeriodicSendWarnCounter;	
	
}TypeDef_WarnLog;


/*Structure Variable for Log Warning*/
extern TypeDef_WarnlogCommand stWarnLog;
/*Structure Variable for Log Warning Counter*/
extern TypeDef_WarnLog stWarningLog;


/* union Variable */
extern SplitByte_TypeDef SplitByte; 			

/*Structure Variable for Current Data*/
extern Typedef_CurrData stCurrentData;

/*Structure Variable for configuration Parameter*/
extern TypeDef_Config stConfigData;
/*Structure Variable for Reciever Inputs*/
extern TypeDef_RecievedCommand stRecieverInput;
/*Structure Variable for Process Inputs*/
extern TypeDef_ProcessedCommand stProcessInput;

typedef struct OperationFlags
{	
	/*Flag used in Default Mode */
	uint8_t bDiagnosticDone:1;
	uint8_t bSCUDiagnosticstatus:1;
	uint8_t bHandshakeDone:1;
	uint8_t bHMIConfigDone:1;
	uint8_t bStartPeriodicHB:1;
	uint8_t bStartDiagTimer:1;  
	
	/*Flag used in Operation Mode */
		uint8_t bHCMCommandSent:1;
		uint8_t bTempAchieveBypass:1;
		uint8_t bPressAchieveBypass:1;
		uint8_t bTempAchieved:1;
		uint8_t bPressAchieved:1;
	uint8_t bCanRemote:1; /*change77*/
	/*Flag used in Scanning Reciever */
		uint8_t bDebouncing:1;
		uint8_t bInputReleased:1;
	/*Flag used in PROCESS */
		uint8_t bStartKeyPressed:1;
		uint8_t bInProgress:1;	
	/* Flag related to Prime Process */
		uint8_t bPrimeSelector:1;
		uint8_t bPrimePosition :1;							/* Gun prime position*/
		uint8_t bPrimeProcessStarted :1;				/* Prime Process Started */
		uint8_t bPrimeFirstStart:1;							/* this flag will tell prime command recieve for 1st time */
		uint8_t bPrimeTimeAchieve:1;  					/* Prime time achieved */
		uint8_t bPrimeProcessPassed :1;
	/* Flag related to Spray Process */	
		uint8_t bAutoCoatSelector:1;
		uint8_t bSpraySelector:1;
		uint8_t bSprayPosition :1;							/* Gun spray position*/
		uint8_t bSprayProcessStarted :1;				/* Prime Process Started */
		uint8_t bSprayTimeAchieve:1;  					/* Prime time achieved */
		uint8_t bSprayProcessPassed :1;
		uint8_t bSprayProcessDone :1;
		uint8_t bStatusLogTake:1;
	/* Flag related to Flush Process */	
		uint8_t bFlushSelector:1;
		uint8_t bFlushPosition :1;							/* Gun prime position*/
		uint8_t bFlushProcessStarted :1;				/* Prime Process Started */
		uint8_t bFlushFirstStart:1;							/* this flag will tell prime command recieve for 1st time */
		uint8_t bFlushTimeAchieve:1;  					/* Prime time achieved */
		uint8_t bFlushProcessPassed :1;
	
		/* Flag Related to CAN */
		uint8_t bCANFaultFlag:1;
		uint8_t bDataInCANRx_BuffFlag:1 ;
		uint8_t bFifoAckTimeoutCheckFlag:1;
		uint8_t bCAN1ActiveFlag:1;
		uint8_t bCAN2ActiveFlag:1;	
		uint8_t bHBAckNotRcvFaultSndFlag:1;
		/* Flag Related to Error & Warning */
		uint8_t bEStop_Flag:1;
		uint8_t bEStop_Sent_Flag:1;
		uint8_t bWarningFlag:1;
		uint8_t bErrorFlag:1;	
		uint8_t bErrorSentFlag:1;
		uint8_t bClearFault :1 ;  /*Clear the Fault and start the process*/
		uint8_t bClearWarning :1 ;/*Clear the Warning and start the process*/	
		uint8_t bStartSignal :1 ;/*Clear the Warning and start the process*/	
		uint8_t bStartupFault :1 ;  /*Clear the Fault and start the process*/
		uint8_t bDefinedState :1 ;  /*Clear the Fault and start the process*/
/* Flag Related to STALL TEST change7 */
		uint8_t bStart_Stall_Flag:1;
		uint8_t bStall_Timer_Flag:1;
		uint8_t bStall_In_Progress:1; /*change70*/
		uint8_t bAuto_Stall_Fail:1; /*change71*/
		uint8_t bStop_Flush_flag:1;
		uint8_t bStall_Cmd_Stop_Flush_flag:1; /*change76*/
		uint8_t bAuto_Flush_flag:1; /*change77*/
		/*  General flags */
		uint8_t bUartFlag:1;
		/* Flag to indicate data availiblity in rgstRxCANBuff buffer	*/
		uint8_t bI2CBusyFlag:1;
	/*Flag indicate either in Periodic Log during Spray or Idle*/
		  /*change56*/
			uint8_t bSendEtopClear:1;
//	uint8_t bThermocoupleFault:1;     
//	    
//	uint8_t bDiagnosticStatus:1;
//	
//	uint8_t bStartupDelayFlag:1;
//	
//	uint8_t bWarmUpModeCmdFlag:1;
//	uint8_t bWarmUpOverFlag:1;
//	
	uint8_t bRecirculationValveONCmdFlag:1;
	uint8_t bABONCmdFlag:1; /*change59*/
	
	uint8_t bConfigJobName:1; /*change78*/
	uint8_t bChangeJobName:1; /*change78*/
	
	uint8_t bConfigJobID:1; /*change78*/
	uint8_t bChangeJobID:1; /*change78*/
	
	uint8_t bConfigSupervisorName:1; /*change78*/
	uint8_t bChangeSupervisorName:1; /*change78*/
	
//	uint8_t bSprayCmdsFlag:1;
//	
//	uint8_t bOperationModeOver:1;
//	
//	uint8_t bPrimePressureAchieve:1;
//	uint8_t bSprayPressureAchieve:1;
//	uint8_t bFlushPressureAchieve:1;
//	
//	uint8_t bInAmbiantReadDoneflag:1;
//	
//	uint8_t bOutAmbiantReadDoneflag:1;
//	uint8_t bInStatusReadflag:1;
//	uint8_t bOutStatusReadflag:1;
//	uint8_t bInTempReadflag:1;
//	
//	uint8_t bOutTempReadflag:1;
//	uint8_t bOutHumidReadflag:1;
//	uint8_t bInHumidReadflag:1;
//	uint8_t bOutsideReadflag:1;
//	
//	uint8_t bOutsideErrorflag:1;
//	uint8_t bInsideErrorflag:1;	
//	
//	
//	uint8_t bStopRuntimeReceived:1;
// 	uint8_t bOPCANFaultFlag:1;

//	uint8_t bSingleCoatFlag:1;
//	
//	uint8_t bWarm_Mod:1  ; /*In system command, Warm up mode is set*/
//  uint8_t bOp_Mod:1	;		 /*When set, Operation mode is set when bit is reset it is Warm up mode*/
//	uint8_t bPause_Mod:1	; /*In system command, pause mode is set*/
//	uint8_t bStart:1 ;   /* Start command Flag*/
//		
//		uint8_t bAutoPrimeStart:1 ; /*Prime process start automatic*/
//	uint8_t bManualPrimeStart:1 ;/*Prime process start Manual*/
//	
//	uint8_t bStartFlush_Auto:1 ; /*To start the Auto flush process*/
//	uint8_t bStartFlush_Manu:1 ; /*To start the Manual flush process*/
//	uint8_t bStallTestAuto:1 ;   /*To start automatic Stall test*/
//	
//	uint8_t bStStallTestManual:1 ;/*To start Manual Stall test*/
//	
//	uint8_t bLAUpstrokeStallTestPass :1 ;/*LEG A Upstroke stall test pass/fail*/
//	
//	uint8_t bLADnstrokeStallTestPass :1 ;/*LEG A Down stroke stall test pass/fail*/
//	uint8_t bLBUpstrokeStallTestPass :1 ;/*LEG B Upstroke stall test pass/fail*/
//	uint8_t bLBDnstrokeStallTestPass :1 ;/*LEG B Down stroke stall test pass/fail*/
//	uint8_t bLCUpstrokeStallTestPass :1 ;/*LEG C Upstroke stall test pass/fail*/
//	
//	uint8_t bLCDnstrokeStallTestPass :1 ;/*LEG C Down stroke stall test pass/fail*/
//		
//	uint8_t bOPMMSetOnPipe :1;		/*OPMM is set on pipe*/

//	uint8_t bSprayStartCmd:1;
//	uint8_t bTemp2Flag:1;
//	
//	uint8_t bTemp3Flag:1;
//	uint8_t bTemp4Flag:1;
//	uint8_t bTemp5Flag:1;
//	uint8_t bTemp6Flag:1;
//	
//	uint8_t bTemp7Flag:1;
//	uint8_t bTemp8Flag:1;
//	uint8_t bTemp9Flag:1;
//	uint8_t bStallTestStatus:1;
//	

//	uint8_t bDeviceNotPresentTx:1;
//	uint8_t bOperationInProcess:1;/*for Operation timeout Counter check*/
//	
}FLAGS_TypeDef;

typedef union
{
	FLAGS_TypeDef stStatusFlag;
	uint32_t uliResetFlag[4];
}TypeDef_Status;

extern TypeDef_Status StatusFlag;
//extern uint16_t uiSignature;
extern uint16_t uiBakSignature;
extern uint16_t uiAmbiantReadCnt;
extern uint16_t uiRTCReadCnt;
extern uint16_t uiRLTempReadCnt;
extern uint16_t uiOperationTimeOut;
/*Reciever Related Variables */
extern uint8_t ucDebCounter;
extern uint8_t ucReleaseStartCounter;
extern uint8_t  ucBackUpReciever;
extern uint8_t ucPreviousProcess;
extern uint8_t ucCurrentProcess;


#ifdef CAN2B_MCP2515_LINK
/*External CAN*/
 extern MCPCANMessage_TypeDef stMCPCANRxmsg; 
 extern MCPCANMessage_TypeDef stMCPCANTxmsg; 
#endif 

 extern uint8_t rgucHBCntr[TOTAL_HB_DEVICE];
 extern uint8_t ucHBCntr;
 extern uint16_t uiHBACKfail;
 extern uint16_t uiHBBAkACKfail;
 extern uint8_t rgucHBFail[2]; /*Array for Fault Data Logging*/
 
 /*change61*/
/* Each bit tells to Diagnostic status recieved from respective device*/
extern uint8_t ucDeviceDiagRx ;  
 extern uint8_t ucDeviceDiagStatus;
 extern uint8_t ucDeviceConfigSent;
 extern uint8_t ucDeviceFirstBootUp;
 
 
 extern uint8_t 	ucSCUMode;
 extern uint16_t 	uiDiagnosticTimer;
 extern uint16_t 	uiStartUpDelayCount;
 
 
 extern uint16_t uiHBTxCounter;
 extern uint16_t uiCANFaultLEDBlinkCnt;
 extern uint16_t uiDefinedStateCnt;
 extern uint8_t ucStalFailureCount;
 extern uint8_t ucCANLinkFaultCnt;

 
 extern uint16_t uiHBFailSend;
 extern uint16_t uiPressureRegReadCnt;
 extern uint8_t ucEEPROMCounter;
// extern float stCurrentData.fCurr_PressureRegRead;
 extern float fPressureRegWrite;
 
 extern uint8_t ucI2CRxCnt;
 extern uint8_t ucTempHighByte;
 extern  uint8_t ucTempLowByte;
 extern uint8_t rgucI2CRxBuff[2];
 extern uint8_t ucSlave_Add;
 extern uint8_t ucI2CSequence;
 extern uint8_t rgucRecString[100];
 extern uint8_t ucloopRecStr;
 extern uint8_t ucLoopCnt;
 extern uint8_t 	ucUpstrokeStallTestResult;
 extern uint8_t 	ucDownstrokeStallTestResult;
 extern uint8_t 	ucMaterialDataReceived;
 
extern uint8_t ucIOExp1LowerByte ;/*EXP1 lower Port*/
extern uint8_t ucIOExp1HigherByte;/*EXP1 higher Port*/
extern uint8_t ucIOExp2LowerByte ;/*EXP2 lower Port*/
extern uint8_t ucIOExp2HigherByte ;/*EXP2 Higher Port*/
extern uint8_t ucPRDSpray; /*status to take periodic log change15*/
extern uint8_t rgucIPADDRESSBuff[15];/*change20*/
extern uint8_t ucSupervisorNameID;/*change23*/
extern uint8_t ucJobNameID;/*change23*/
/*change48*/
extern uint8_t ucJobID;
/*Flash Testing */
/*change24*/
extern uint8_t ucData[FLASH_RX_TX_BUFFER_SIZE];
extern char supervisorname[FLASH_RX_TX_BUFFER_SIZE];
extern char jobname[FLASH_RX_TX_BUFFER_SIZE];
extern char jobid[FLASH_RX_TX_BUFFER_SIZE];/*change42*/
extern uint8_t 	ucStallTestCount;  /*change72*/

/*change78*/
extern char jobnamepre[FLASH_RX_TX_BUFFER_SIZE];
extern char jobnamebac[FLASH_RX_TX_BUFFER_SIZE];
extern uint8_t ucJobNameLog;


extern char supervisornamepre[FLASH_RX_TX_BUFFER_SIZE];
extern char supervisornamebac[FLASH_RX_TX_BUFFER_SIZE];
extern uint8_t ucSupervisorNameLog;

extern char jobidpre[FLASH_RX_TX_BUFFER_SIZE];
extern char jobidbac[FLASH_RX_TX_BUFFER_SIZE];
extern uint8_t ucJobLog;
/* ----------------------------------------------------------------------------
 *                           EXTERNAL FUNCTIONS
 * ----------------------------------------------------------------------------
 */
 
 /* System Init File */
	extern void fnSystem_Init(void);
  /*SPI functions*/
	extern void  fnSPI_Config(void);
 /*USART Functions*/
	extern void fnUSART_Config(void);
 /*Timer Functions*/
	extern void fnTIM_Configuration(void);

	extern void fnPressure_Regulator_Config(void);
	extern void fnWifi_Config(void);
	extern void fnCAN_INIT(void);
	extern void fnSPI_Config(void);
	extern void fnNVSRAM_Config(void);
	extern void fnEEPROM_Config(void);
	extern void fnReciever_Config(void);
	extern void fnHooter_Config(void);
 
 /* Functions defined in Miscellaneous.c */
	extern void fnDefault_Mode(void);
	extern void fnOperation_Mode(void);
	extern void fnReset_Configuration(void);
	extern void fnSCU_Diagnostic(void);
	extern void fnSCU_Handshake(void);
	extern void fnSCU_HCM_Command(void);
	extern void fnSCU_Press_Temp_Check(void);
	extern void fnSCU_Perform_Process(void);
	extern void fnSCU_Operation(void);
  extern void fnSendOPMM_Config(void);
	extern void fnSendHMI_Config(void);
	extern void fnSendVCM_Config(void); 
	extern void fnSendHCM_Config(void);
	extern void fnLED_Indication(void);
	extern void fnEstop_Check(void);
	extern void fnAllWarnnings_Clear(uint8_t , uint8_t);
	extern void fnAllERROR_Clear(uint8_t);
	extern void fnSCU_HCM_Command(void);
	extern void fnSCU_Press_Temp_Check(void);
	extern void fnSCU_Perform_Process(void);
	extern void fnPressure_Regulation(void);
	extern void fnRTC_Read(void);
	extern void fnERROR_WARN_Action(void);
	extern void fnReciever_Scanner(void);
	extern void fnStalltest_Commandsend(void);/*change7*/
 /*General*/
	extern void fnDelay_ms(uint32_t dly);
 
 /* Functions defined in EPPROM.c */ 
	extern void fnConfigData_Read(void);
	extern void fnEEPROMConfig_Write(void);
	/* Functions defined in PressureRegulator.c */ 
	extern float fnADCValue_Read(uint8_t);
	extern uint16_t fnADC3ChannelValue_Get(uint8_t);
	extern void fnDACValue_Set(uint8_t);
	extern void fnADC_Config(void);
	extern void fnDAC_Config(void);

/* Functions defined in Data_Logging.c */ 
	extern void fnFaultLog_Read(uint32_t);
	extern void fnFaultLog_Get(void);	
	extern void fnStatusLog_Read(DATALOG_TX_t *pstLog);
	extern void fnStatusDataLog_Get(void);
	extern void fnDiagLog_Read(uint32_t );
	extern void fnDiagnosticLog_Get(void);
	extern void fnPRDLog_Get(void);
	extern void fnPRDLog_Read(DATALOG_TX_t *pstLog);
	extern void fnNVSRAM2_MultiDataRead(uint8_t, uint32_t, uint8_t*);
	extern void fnNVSRAM1_MultiDataRead(uint8_t, uint32_t, uint8_t*);
	extern void fnDataLogging_UART(void);
	/*change15*/ 
	extern void fnPeriodic_Log_Process_Based(void);
	/*change78*/ 
	extern void fnCheck_Repeat_Index(void);


	/*CAN functions*/
	extern void fnCAN_Config(CAN_TypeDef*);
	extern void fnCANMsg_Process(CanRxMsg*);
	extern void fnCANMsg_Validate(CanRxMsg*);
	extern void fnHBAckFail_Send(uint16_t);
	extern void fnPeriodicHeartbeat(void);
	extern void fnFifo_Reset(stTxMsgFlags *);
	extern uint8_t fnFifo_Write(CanTxMsg*, stTxMsgFlags*);
	extern void fnFifo_Read(stTxMsgFlags *, CanTxMsg *);
	extern uint8_t fnCANMsg_Transmit(CanTxMsg *, CAN_TypeDef* );
	extern uint8_t fnCANPost_Ack(CanRxMsg *pstCANRxmsg, CAN_TypeDef* );
	extern void fnCANPost_TimeOut(uint32_t);
	extern void fnSendACK(CanRxMsg*);
	extern void fnFifoAckTimeout_Check(void);
	
	/*change24*/
	extern void fnFlashWrite(uint32_t uliAddress, char *pucData);
	extern uint8_t* fnFlashRead(uint32_t uliAddress);
	extern void fnStoreSupervisorName(uint8_t INDEX_NUMBER,char *pucData);/*change25*/
	extern void fnRetriveSupervisorName(uint8_t INDEX_NUMBER);/*change25*/
	extern void fnStoreJobName(uint8_t INDEX_NUMBER,char *pucData);/*change25*/
	extern void fnRetriveJobName(uint8_t INDEX_NUMBER);/*change25*/
	extern void fnResetName(uint16_t Sector_No);/*change25*/
	extern void fnCheck_ID(void);/*change26*/
	/*change27*/
	extern void fnWarnlog_Commandsend(void);
	/*change48*/
	extern void fnRetriveJobID(uint8_t INDEX_NUMBER);
	extern void fnStoreJobID(uint8_t INDEX_NUMBER,char *pucData);


/* ----------------------------------------------------------------------------
 *                           FUNCTIONS DECLARITION
 * ----------------------------------------------------------------------------
 */
 




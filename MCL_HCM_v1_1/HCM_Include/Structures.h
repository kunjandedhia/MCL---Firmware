/******************************************************************************
**@file			Structures.h
* 
**@brief    This Header File contains declaration of all Structures
******************************************************************************
*/    
#ifndef __Structures_h
#define __Structures_h

/* ----------------------------------------------------------------------------
*                           Includes
* ----------------------------------------------------------------------------
*/
#include "Macros.h"

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
typedef struct
{
	uint8_t bWindowOverflow:1;					/*This will indicate that window overflows*/
	uint8_t ucThermoPosition;						/*for Position of the thermocouple connector*/
	float fCurrentTemp;									/*Current Temp data*/	
	float rgfTempBuff[TC_WINDOW_SIZE];  /*current Temp Buffer*/	
	uint8_t ucTCFault;									/*Counter for fault detection of the thermocouple*/
	uint8_t ucTraverse;									/*variable for traversing through the window*/
	float fSum;													/*summation of all temp values stored in the window*/
}TC_Read_t;

typedef struct
{
	uint8_t bTempInRangeFlag:1; 				/*Temp in Range indication flag*/
	uint8_t bTempHighTHFaultFlag:1; 		/*High thershold fault indication flag*/
	uint8_t bTempLowTHFaultFlag:1;			/*Low  thershold fault indication flag*/
	uint8_t bTCFaultFlag:1;

	uint8_t bHeaterStatusFlag:1;				/*for Current heater status Indication, SET = ON RESET = OFF*/
	uint8_t bPreHeaterStatusFlag:1; 		/*use to send the heater status whenever heater state is changed*/
	uint8_t bHeaterFaultFlag:1;					/*for heater fault declaration and LED indication*/

	uint8_t ucLowTHFaultCnt;						/*counter for counting low thershold fault occurance continuously*/
	uint8_t ucHighTHFaultCnt; 					/*counter for counting High thershold fault occurance continuously*/
	uint8_t ucNormalTempCnt; 					 	/*counter for counting Normal temp after any error occurance continuously*/

	uint8_t ucSSRPosition;  						/*Used for the position of SSR's Connected on the board*/
	float fTempHighTH;									/*temperature High thershold value*/
	float fTempLowTH;										/*temperature Low thershold value*/
	float fSetTemp;											/*Set Temperature*/
	float fTempHystPos;									/*Hysterysis Positive value*/
	float fTempHystNeg;									/*Hysterysis Negative value*/
}TC_Process_t;

typedef struct
{
	TC_Read_t stRead[NO_OF_TC_SENSOR];
	TC_Process_t stProcess[NO_OF_TC_SENSOR - 1];
}TC_t;

typedef struct
{
	uint8_t bSlideWindowFull:1;					/*indication for window full or not*/
	uint8_t bLLSensorError:1;						/*indication for faulty/disconnected sensor*/
	uint8_t bLLSensorErrorSend:1;				/*to send the error Message to SCU and HMI only Once*/
	uint8_t bLLLowTHFaultFlag:1;				/*Indication for Low thershold fault*/
	uint8_t bLLHighTHFaultFlag:1;				/*Indication for High thershold fault*/
	uint8_t bLLInRangeFlag:1;						/*Indication for In Range*/
	
	uint8_t ucWindowtraverse;						/*Variable for traversing in window*/
	uint8_t	ucADCChannel;								/*ADC Channel Number*/
	uint8_t	ucLLLowFaultCnt;						/*Counter for fault declaration*/
	uint8_t ucLLHighFaultCnt;						/*Counter for fault declaration*/
	uint8_t ucLLInRangeCnt;							/*Counter for No Fault declaration*/
	
	uint16_t uiTimCountLLSample;				/*Counter for the Delay between reading the LL*/
	float fSumOfLL;											/*Summation of all the LL Data*/
	float fLiquidLevel;									/*Current Liquid Level*/
	float fLiquidLevelTankHeight;				/*Height of the Tank*/
	float fLiquidLevelLowTH;						/*Low thershold*/
	float fLiquidLevelHighTH;						/*High thershold*/
	float rgfLiquidLevelBuff[LL_WINDOW_SIZE];/*Buffer for storing liquid level data*/
}LL_t;

typedef struct 
{
	uint8_t bTCProcessed:1;
	uint8_t bACKRxFlag:1;
	uint8_t bTxFlag:1 ;
	uint8_t bHeartBeatMsgFlag:1;
	uint8_t bFirstHrtBtMsgFlag:1;
	uint8_t bCANFaultSCU:1;
	uint8_t bCANFaultHMI:1;
	uint8_t bDiagnosticsPass:1;
	
	uint8_t bEStopFlag:1;
	uint8_t bHBNotRcvFlag:1;
	uint8_t bDiagnosticsFlag:1;
/*uint8_t bCANFaultFlag:1; *//*CAN fault not used in HCM, SCU will indicate the CAN Fault */
	uint8_t bDataInCANRxBuffFlag:1;
	uint8_t bDataInCANTxBuffFlag:1;
	uint8_t bFirstBootup:1;
	uint8_t bWarnningFlag:1; 
	
	uint8_t bErrorFlag:1;
	uint8_t bHardPreHeaterStatusFlag:1;/*use to send the hardner heater status whenever heater state is changed*/
	uint8_t bHardHeaterStatusFlag:1; /*hardner Current heater status Indication, SET = ON RESET = OFF*/
	uint8_t bHardHeaterFaultFlag:1; /*hardner heater Fault flag for LED ON/OFF*/
	uint8_t bI2CBusyFlag:1;					/*indication for I2C is being reading/Writing*/
	#ifdef TEST_DBG
	  uint8_t bUartFlag:1;
	#endif
}FLAGS_t;

typedef union
{
	FLAGS_t stStatusFlag;
	uint32_t uliStatusFlagReset;
}StatusFlags_t;

typedef union
{
	float fData;
	uint8_t rgucData[4];
	uint16_t uiData;
	uint32_t uliData;
}SplitByte_t;

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

#endif


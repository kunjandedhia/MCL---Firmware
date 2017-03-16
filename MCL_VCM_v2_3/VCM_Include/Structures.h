/******************************************************************************
  **@file     :    Structures.h
  * 
  * @brief  	:		 This file contains structures
  ******************************************************************************
  */	
	#ifndef __Structures_h
	#define __Structures_h
/* ----------------------------------------------------------------------------
*                           Includes
* ----------------------------------------------------------------------------
*/
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
	typedef struct OperationFlags
	{	
		uint8_t bDataInCANRxBuffFlag:1;	
		uint8_t bDataInCANTxBuffFlag:1;
		uint8_t bACK_Received:1;
		uint8_t bTransmittingCAN:1;	
		uint8_t bCANFaultSCU:1;
		uint8_t bCANFaultHMI:1;	
		uint8_t bHBNotRecv:1;
		uint8_t bDiagnosticsPass:1;
		
		uint8_t bDiagnosticsFlag:1;
		uint8_t bStartMonitorLT:1;
		uint8_t bStopMonitorLT:1;
		uint8_t bLTFindLimit:1;
		uint8_t bStallTestStart:1;
		uint8_t bHeartBeatMsgFlag:1;
		uint8_t bFirstHrtBtMsgFlag:1;
		
		uint8_t bEstop:1;	
		uint8_t bReadValue1:1;
		uint8_t bReadValue2:1;	
		uint8_t bLTRangeCheck:1;
		uint8_t bValveReadFlag:1;
		uint8_t bLTFirstLength:1;
		uint8_t bLTLastLength:1;
		
		uint8_t bI2CReadDone:1;			/*Indicate that current I2C Read Cycle Completed*/
		uint8_t bAmbiantReadDone:1; /*indicates that Both EE07 Read successfully*/
		uint8_t bI2CBusyFlag:1;
		uint8_t bFirstBootUp:1;
		
		uint8_t bExtInt:1; //change2
	}FLAGS_t;

	typedef union
	{
		FLAGS_t stStatusFlag;
		uint32_t uliFlagReset;
	}FLAGS_Union_t;
	
	/*********Union for the Rx an Tx the float Value from the Can*****/
	typedef union
	{
		uint16_t uiData;
		int16_t iData;
		uint8_t rgucData[4];
		int8_t rgcData[4];
		uint32_t uliData;
		float fData;
	}SplitByte_t;
	
	typedef struct StallStatusFlags
	 {	
		uint8_t bTBeforeStall:1;
		uint8_t bTBeforeStallPresCompr:1;		
		uint8_t bPumpOnDownStroke:1;
		uint8_t bPumpOnUpStroke:1;
		uint8_t bCompareLegs:1;
		uint8_t bDownStrokeDone:1;
		uint8_t bUpStrokeDone:1;
		uint8_t bStartDownStroke:1;
		uint8_t bStartUpStroke:1;
		uint8_t bUpStrokeLegCmpr:1;
		uint8_t bDownStrokeLegCmpr:1;
		
		uint8_t bUpStrokePass:1;
		uint8_t bDownStrokePass:1;
	 }StallTest_t;
	 
	typedef struct
	{
		uint8_t bValvePrevCurrState:1;		/*Valve Previous status Flag*/
		uint8_t bValveCurrState:1;				/*valve current status Flag*/
		uint8_t bFaultFlag:1;							/*Fault Indication flag*/
		uint8_t ucPosition;								/*Position of Valve connected on the Board */
	}ValveState_t;
	
	typedef struct
	{
//		volatile float fCurrentPG;						/*Current reading of Pressure Gauge*/
//		volatile float fPreviousPG;					/*Previous reading of Pressure Gauge*/
//		volatile float fPGLowTH;							/*Pressure Gauge Low Thershold */
//		volatile float fPGHighTH; 						/*Pressure Gauge High Thershold*/	
		
		uint8_t bLegStatus:1;				/*Leg Status Flag*/
		uint8_t bLTHFaultFlag:1;		/*Indication for Low thershold fualt*/
		uint8_t bHTHFaultFlag:1;		/*Indication for High thershold fualt*/
		
		uint8_t ucLTHFaultCnt;  		/*Counter to Declare Low thershold fault*/
		uint8_t ucHTHFaultCnt;  		/*Counter to Declare High thershold fault*/
		//uint8_t ucPosition;		  		/*Connected position on the Board connector*/
		
		int16_t iPercentage;				/*Change in Pressure*/ 
	}PG_t;
	
	
/* ----------------------------------------------------------------------------
*                           EXTERNAL FUNCTIONS
* ----------------------------------------------------------------------------
*/
/* ----------------------------------------------------------------------------
*                           FUNCTIONS DECLARATION
* ----------------------------------------------------------------------------
*/

#endif


/******************************************************************************
**@file    CAN_MACROS1.h
* 
**@brief   This file contains 
******************************************************************************
*/    
/* ----------------------------------------------------------------------------
*                           Includes
* ----------------------------------------------------------------------------
*/
#include<stdint.h>
/* ----------------------------------------------------------------------------
*                           MACROS
* ----------------------------------------------------------------------------
*/
/******CAN initialization Pins and Ports*******************/
#define CAN_TX_PIN												GPIO_Pin_9
#define CAN_RX_PIN												GPIO_Pin_8

#define CAN_PORT													GPIOB
#define RCC_APBPERIPH_GPIO								RCC_APB2Periph_GPIOB
	
#define CAN_ENABLE_PIN										GPIO_Pin_10
#define CAN_STROBE_PIN										GPIO_Pin_11	
#define	CAN_ENABLE_PORT										GPIOB
#define	CAN_STROBE_PORT										GPIOB

/****************CAN filter********************/
/*Filter 0*/
	#define OP_CAN_FILTERIDHIGH_0						(uint16_t)0x0000
	#define OP_CAN_FILTERIDLOW_0						(uint16_t)0x0100;

	#define CAN_FILTERMASKIDHIGH_0					(uint16_t)0x0000
	#define CAN_FILTERMASKIDLOW_0						(uint16_t)0xFF00

/*Filter 1*/
	#define OP_CAN_FILTERIDHIGH_1						(uint16_t)0x0000
	#define OP_CAN_FILTERIDLOW_1						(uint16_t)0xFF00

	#define CAN_FILTERMASKIDHIGH_1					(uint16_t)0x0000
	#define CAN_FILTERMASKIDLOW_1						(uint16_t)0xFF00

/****************************************/

	#define CAN_MSG_RESEND_TIME							(uint16_t)250  	/*Message resend time = 250 * 1ms = 250 ms*/
	#define CAN_MSG_RESEND_ATTEMPT					(uint8_t)2 			/*CAN message resend attempt count*/
	#define HEARTBEAT_TIMEOUT								(uint16_t)750		/*HeartBeat Timeout = 3000 * 1ms = 3 seconds*/
	
	#define CAN1_BUF_SIZE										(uint8_t)500 	

	/*OPMM Valves*/
	#define OPMM_VL_SPRAY_GUN								(uint8_t)0x01
	#define OPMM_VL_SPRAY_HEAD							(uint8_t)0x02
	#define OPMM_VL_AB											(uint8_t)0x04
	#define OPMM_VL_FLUSH										(uint8_t)0x08
	#define OPMM_VL_FORWARD									(uint8_t)0x10
	#define OPMM_VL_BACKWARD								(uint8_t)0x20
	
	/*HCM Heaters*/
	#define HCM_INLINE_BASE_HEATER					(uint8_t)0x01
	#define HCM_INLINE_HARD_HEATER					(uint8_t)0x02
	#define HCM_ANTIFREEZ_HEATER						(uint8_t)0x04
	#define HCM_HARD_TANK_HEATER						(uint8_t)0x08

	/*VCM Valves*/
	#define VCM_VL_STALL										(uint8_t)0x01  /*change12*/
	#define VCM_VL_RECIRCULATION						(uint8_t)0x02  /*change12*/ 

	/*Threshold*/\
	#define SET_VALUE												(uint8_t)0x01
	#define LOW_TH													(uint8_t)0x01
	#define HIGH_TH													(uint8_t)0x02
	
	#define BASE_TANK												(uint8_t)0x01
	#define HARD_TANK												(uint8_t)0x02

	#define BASE_HYST_POS										(uint8_t)0x08
	#define BASE_HYST_NEG										(uint8_t)0x10
	#define HARD_HYST_POS										(uint8_t)0x08
	#define HARD_HYST_NEG										(uint8_t)0x10
	#define ANTIFRZ_HYST_POS								(uint8_t)0x08
	#define ANTIFRZ_HYST_NEG								(uint8_t)0x10

	#define RECIRC_PRES_HYST_POS						(uint8_t)0x08
	#define RECIRC_PRES_HYST_NEG						(uint8_t)0x10
	#define SPRAY_PRES_HYST_POS							(uint8_t)0x08
	#define SPRAY_PRES_HYST_NEG							(uint8_t)0x10

	#define BASE_MAT_TANK_HEIGHT						(uint8_t)0x01
	#define HARD_MAT_TANK_HEIGHT						(uint8_t)0x02

	/*Devices*/
	#define OPMM														(uint8_t)0x01   
	#define MFM															(uint8_t)0x02   
	#define BHCM														(uint8_t)0x08  
	#define HHCM														(uint8_t)0x10  
	#define IHCM1														(uint8_t)0x20 
	#define IHCM4														(uint8_t)0x04 

	/*Device Id*/
	#define HMI_ID														(uint8_t)0x01		//Operator module Device Id
	#define SCU_ID													(uint8_t)0x02		//SCU Device Id 
	#define OPMM_ID													(uint8_t)0x03		//OPMM Device Id 
	#define VCM_ID													(uint8_t)0x05		//MFM Device Id 
	#define HCM_ID													(uint8_t)0x04		//BHCM Device Id 

	/*Stall*/	
	#define UPSTROKE_FAIL		 								(uint8_t)0x01
	#define UPSTROKE_PASS										(uint8_t)0x02
	#define DOWNSTROKE_FAIL									(uint8_t)0x04
	#define DOWNSTROKE_PASS									(uint8_t)0x08

	#define	LEG_A														(uint8_t)0x01
	#define	LEG_B														(uint8_t)0x02
	#define	LEG_C														(uint8_t)0x04
/*change11*/
	#define UPSTROKE												(uint8_t)0x01
	#define DOWNSTROKE											(uint8_t)0x02
	/*change11*/ 	
	#define UPSTROKE_TH											(uint8_t)0x01
	#define DOWNSTROKE_TH											(uint8_t)0x02

	/*4th March2016*/
	#define UPSTROKE_LIMIT									(uint8_t)0x01
	#define UPSTROKE_MIN										(uint8_t)0x02
	#define UPSTROKE_MAX										(uint8_t)0x04
	#define DOWNSTROKE_LIMIT								(uint8_t)0x08
	#define DOWNSTROKE_MIN									(uint8_t)0x10
	#define DOWNSTROKE_MAX									(uint8_t)0x20
	
	#define PROCESS_FAIL										(uint8_t)0x01
	#define PROCESS_PASS										(uint8_t)0x02

	#define OPERATION_MODE_OVER							(uint8_t)0x01
	#define OPERATION_MODE_READY						(uint8_t)0x02

	#define OPMM_NOT_SET_PIPE								(uint8_t)0x00
	#define OPMM_SET_PIPE										(uint8_t)0x01

	#define PRIME														(uint8_t)0x01
	#define FLUSH														(uint8_t)0x02
	#define TIME_ACHIEVE_FAIL								(uint8_t)0x01
	#define TIME_ACHIEVE_PASS								(uint8_t)0x02

	#define RECIRCULATION_PRESSURE					(uint8_t)0x00
	#define SPRAY_PRESSURE									(uint8_t)0x01

	/*Diagnostic*/
	#define DIAGNOSTIC_PASS									(uint8_t)0x02
	#define DIAGNOSTIC_FAIL									(uint8_t)0x01

	/*MCU*/
	#define MCU_EEPROM											(uint8_t)0x01
	/*change13*/
	#define MCU_NVSRAM1											(uint8_t)0x02
	#define MCU_NVSRAM2											(uint8_t)0x04
	#define MCU_PRESSURE_REGULATOR					(uint8_t)0x08

	/*VCM*/
	#define AIR_SNR_CONNECTED								(uint8_t)0x02
	#define AIR_SNR_DISCONNECTED						(uint8_t)0x01
	
	#define MFM_LINEAR_TRANS								(uint8_t)0x02
	#define MFM_PRESSURE_G1									(uint8_t)0x04
	#define MFM_PRESSURE_G2									(uint8_t)0x08
	#define MFM_PRESSURE_G3									(uint8_t)0x10
	#define MFM_PRESSIDENT_STATUS						(uint8_t)0x20
	#define MFM_KING_STATUS									(uint8_t)0x40
	#define MFM_PRESSIDENT_CONNECTED				(uint8_t)0x01
	#define MFM_KING_CONNECTED							(uint8_t)0x02

	/*BHCM*/
	#define HEATER													(uint8_t)0x02
	#define THERMOCOUPLE										(uint8_t)0x04
	#define BHCM_LIQUID_LEVEL_SENSOR				(uint8_t)0x08

	#define PRIME_POSITION									(uint8_t)0x01
	#define SPRAY_POSITION									(uint8_t)0x02

	#define PIPE_POSITION_12								(uint8_t)0x01
	#define PIPE_POSITION_3									(uint8_t)0x02
	#define PIPE_POSITION_6									(uint8_t)0x04
	#define PIPE_POSITION_9									(uint8_t)0x08
	#define AVG_TEMP												(uint8_t)0x10

	#define PRIME_PROCESS										(uint8_t)0x01
	#define SPRAY_PROCESS										(uint8_t)0x02
	#define FLUSH_PROCESS										(uint8_t)0x04

	#define LEG_A_DATA											(uint8_t)0x01
	#define LEG_B_DATA											(uint8_t)0x02
	#define LEG_C_DATA											(uint8_t)0x04
	#define MATERIAL_DATA_RECEIVED					(uint8_t)0x07

	#define PRIME_DONE											(uint8_t)0x01
	#define FLUSH_DONE											(uint8_t)0x02
	#define FLUSH_COMPLETED									(uint8_t)0x04

	#define BASE_MATERIAL										(uint8_t)0x01
	#define HARD_MATERIAL										(uint8_t)0x02

	#define IHCM4_HM												(uint8_t)0x01
	#define IHCM4_BM												(uint8_t)0x02

	#define FRAME_1													(uint8_t)0x01
	#define FRAME_2													(uint8_t)0x02
	#define FRAME_3													(uint8_t)0x04

	#define TIME_CONVERT_FACTOR							(uint8_t)100
	#define DCLS_TIMER											(uint8_t)0x01
	#define HLS_TIMER												(uint8_t)0x02

	#define TX_CAN_BUF_SIZE									(uint8_t)50
	#define RX_CAN_BUF_SIZE									(uint8_t)50 

	#define AUTOMATIC_PROCESS								(uint8_t)0x01
	#define MANUAL_PROCESS									(uint8_t)0x02
	#define EXTRA_COAT											(uint8_t)0x02


	#define ON_COMMAND											(uint8_t)0x02
	#define OFF_COMMAND											(uint8_t)0x01

	#define WARM_UP_MODE										(uint8_t)0x00
	#define OPERATION_UP_MODE								(uint8_t)0x01
	#define PAUSE_UP_MODE										(uint8_t)0x02
	/*change4*/
	#define ACK_FOR_MEMORY_FULL							(uint8_t)0xF0
	#define ALL_SLAVE_DEVICE  							(uint16_t)(	OPMM_ID		| VCM_ID | HCM_ID )
		
	#define BIT_0														(uint8_t)0x01
	#define BIT_1														(uint8_t)0x02
	#define BIT_2														(uint8_t)0x04
	#define BIT_3														(uint8_t)0x08
	#define BIT_4														(uint8_t)0x10
	#define BIT_5														(uint8_t)0x20
	#define BIT_6														(uint8_t)0x40
	#define BIT_7														(uint8_t)0x80
	
	#define BIT_8														(uint16_t)0x0100
	#define BIT_9														(uint16_t)0x0200
	#define BIT_10													(uint16_t)0x0400
	#define BIT_11													(uint16_t)0x0800
	#define BIT_12													(uint16_t)0x1000
	#define BIT_13													(uint16_t)0x2000
	#define BIT_14													(uint16_t)0x4000
	#define BIT_15													(uint16_t)0x8000
	/*change3*/
  

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

	#define DATA_BYTE_ZERO 											(uint8_t)0
	#define DATA_BYTE_ONE  											(uint8_t)1
	#define DATA_BYTE_TWO  											(uint8_t)2
	#define DATA_BYTE_THREE 										(uint8_t)3
	#define DATA_BYTE_FOUR  										(uint8_t)4
	#define DATA_BYTE_FIVE  										(uint8_t)5
	#define DATA_BYTE_SIX	  										(uint8_t)6
	#define DATA_BYTE_SEVEN	  									(uint8_t)7
	
	#define PRIME_POSITION									(uint8_t)0x01
	#define SPRAY_POSITION									(uint8_t)0x02
	
	#define ETHERNET 												(uint8_t)0x01
	#define WIFI		 												(uint8_t)0x02
	/*change3*/
	#define ETHERNETIP											(uint8_t)0x11		
	#define ETHERNETSUBNET									(uint8_t)0x21		
	#define ETHERNETGW											(uint8_t)0x41		
	#define ETHERNETPORTNUMBER							(uint8_t)0x81		
	#define WIFIIP													(uint8_t)0x12		
	#define WIFISUBNET											(uint8_t)0x22		
	#define WIFIGW													(uint8_t)0x42		
	#define WIFIPORTNUMBER									(uint8_t)0x82	
	
	
/*----------------------------------------------------------------------------
*               STRUCTURES
* ----------------------------------------------------------------------------
*/

/* Structure for Current Data*/
typedef struct CurrentValues
{	
	float fCurrInlineBaseMatTemp;
	float fCurrInlineHardMatTemp;
	float fCurrReturnLineTemp;
	float fCurrAntifreezeLiquidTemp;
	
	float fCurrOPMMBaseTemp;
	float fCurrOPMMHardTemp;
	
	uint8_t fCurrCAN1ALinkFault;
	
	float fCurrInsideAmbientTemp;
	float fCurrOutsideAmbientTemp;
	float fCurrOutsideAmbientHumidity;
	float fCurrPipeTempAvg;
	float fCurrPipeTempPos12;
	float fCurrPipeTempPos3;
	float fCurrPipeTempPos6;
	float fCurrPipeTempPos9;
	float fLegAMaterialUsed;
	float fLegBMaterialUsed;
	float fLegCMaterialUsed;
	float fBaseTotalMaterialUsed;
	float fHardTotalMaterialUsed;
	float fBaseMaterialPrimed;
	float fHardMaterialPrimed;
	float fBaseMaterialApplied;
	float fHardMaterialApplied;
	float fCurrPressureGauge1;
	float fCurrPressureGauge2;
	float fCurrPressureGauge3;
	float fCurrLiquidLevelBase;

	float fCurrPrimeTime;
	float fCurrSprayTime;
	float fCurrFlushTime;
	float fCurrDewPoint;
	uint8_t ucCurrNumberofRotation;
	float fCurrBateryStatus;
	
	/*New Additon*/
	float fLiquid_Hardner_Level_Curr;
	int16_t iCurrPressureLegADown;
	int16_t iCurrPressureLegBDown;
	int16_t iCurrPressureLegCDown;
	int16_t iCurrPressureLegAUp;
	int16_t iCurrPressureLegBUp;
	int16_t iCurrPressureLegCUp;
	float fCurrLinearTransducer;
	
	uint8_t ucOPMMValveBackUpState;
	uint8_t ucOPMMValveCurState;
	uint8_t ucOPMMValveCurStateChange;
	/*change12*/
	uint8_t ucVCMValveBackUpState;
	uint8_t ucVCMValveCurState;
	uint8_t ucVCMValveCurStateChange;
	
	uint8_t ucHCMHeaterBackUpState;
	uint8_t ucHCMHeaterCurState;
	uint8_t ucHCMHeaterCurStateChange;
	
	float fSCUCodeVersion;
	float fOPMMCodeVersion;
	float fHCMCodeVersion;
	float fVCMCodeVersion;
	
	/*13thJuly2016*/
	uint8_t ucWiFiIPSlot1;
	uint8_t ucWiFiIPSlot2;
	uint8_t ucWiFiIPSlot3;
	uint8_t ucWiFiIPSlot4;
	/*change3*/
	uint8_t ucWiFiSubnetSlot1;
	uint8_t ucWiFiSubnetSlot2;
	uint8_t ucWiFiSubnetSlot3;
	uint8_t ucWiFiSubnetSlot4;
	uint8_t ucWiFiGWSlot1;
	uint8_t ucWiFiGWSlot2;
	uint8_t ucWiFiGWSlot3;
	uint8_t ucWiFiGWSlot4;
	
	uint8_t ucEthIPSlot1;
	uint8_t ucEthIPSlot2;
	uint8_t ucEthIPSlot3;
	uint8_t ucEthIPSlot4;
	/*change3*/
	uint8_t ucEthSubnetSlot1;
	uint8_t ucEthSubnetSlot2;
	uint8_t ucEthSubnetSlot3;
	uint8_t ucEthSubnetSlot4;
	uint8_t ucEthGWSlot1;
	uint8_t ucEthGWSlot2;
	uint8_t ucEthGWSlot3;
	uint8_t ucEthGWSlot4;
	
	
	uint16_t iWiFiPort;
	uint16_t iEthPort;
	
	float fBaseMatTotalJoint;
	float fHardMatTotalJoint;
	float fBaseMatPrimeAcc;
	float fHardMatPrimeAcc;
	float fBaseMatSprayAcc;
	float fHardMatSprayAcc;
	float fBaseMatTotalAcc;
	float fHardMatTotalAcc;
	
	
}DATA_Typedef;

/* Structure for Configuration Parameters */
typedef struct ConfigValues
{
	float fConfigPipeTempLTH;
	float fConfigPipeTempHTH;
	float fConfigPrimeTime;
	float fConfigGunDelayTime;
	float fConfigSprayOverlapTime;
	float fConfigReverseDelayTime;
	float fConfigFlushTime;	
	float fConfigPressureGauge1LTH;
	float fConfigPressureGauge1HTH;                                                                            
	float fConfigPressureGauge2LTH;
	float fConfigPressureGauge2HTH;
	float fConfigPressureGauge3LTH;
	float fConfigPressureGauge3HTH;
	float fConfigJointNum;
	float fConfigDataLoggingPeriod;
	float fConfigAreaofCylinderA;
	float fConfigAreaofCylinderB;
	float fConfigAreaofCylinderC;
	float fConfigTimeBeforeStall;
	float fConfigTimeCompareStall;
	float fConfigDCLSHomeTime;
	float fConfigHomeDCLSTime;
	float fGunPosSwTime;
	float fConfigTimebeforeONSprayValve;
	float fConfigBaseLiquidLevelLTH;
	float fConfigBaseLiquidLevelHTH;
	float fConfigHardLiquidLevelLTH;
	float fConfigHardLiquidLevelHTH;
	float fConfigBaseTankHeight;
	float fConfigHardTankHeight;
	float fConfigFlushWarnTime;
	
	uint16_t uiConfigSprayCount;
	uint16_t uiConfigCylinderCount;
	uint8_t ucTypeofUnit;	
	uint8_t rgucConfigSupervisorName1[8]; 
	uint8_t rgucConfigSupervisorName2[8]; 
	uint8_t rgucConfigSupervisorName3[8]; 
	uint8_t rgucConfigCoatingMatName1[8]; 
	uint8_t rgucConfigCoatingMatName2[8]; 
	uint8_t rgucConfigCoatingMatName3[8]; 
	uint8_t rgucConfigBaseMatBatchNo1[8]; 
	uint8_t rgucConfigBaseMatBatchNo2[8]; 
	uint8_t rgucConfigBaseMatBatchNo3[8]; 
	uint8_t rgucConfigHardMatBatchNo1[8]; 
	uint8_t rgucConfigHardMatBatchNo2[8]; 
	uint8_t rgucConfigHardMatBatchNo3[8]; 
	uint8_t ucConfigSystemTime[7];
	
	uint8_t rgucJobName1[8];
	uint8_t rgucJobName2[8];
	uint8_t rgucJobName3[8];
	
	/*change7*/
	uint8_t rgucJobId1[8];
	uint8_t rgucJobId2[8];
	uint8_t rgucJobId3[8];
	
	/*change3*/
	uint8_t rgucSSID1[8];
	uint8_t rgucSSID2[8];
	
	uint8_t rgucPass1[8];
	uint8_t rgucPass2[8];

	uint16_t uiConfigLLWindowSize;
	int16_t iConfigDownStrokeTH;
	int16_t iConfigUpStrokeTH;
	
	
	/**************/
	float fConfigSprayPressure; 
	float	fConfigSprayPressureLTH;
	float	fConfigSprayPressureHTH;
	float	fConfigHysSprayPresPos;
	float	fConfigHysSprayPresNeg;
	
	float	fConfigRecirPressure;	
	float	fConfigRecirPressureLTH;
	float	fConfigRecirPressureHTH;
	float	fConfigHysRecircPresPos;
	float	fConfigHysRecircPresNeg;
	
	float	fConfigAntiFrzTempSet;
	float	fConfigAntiFrzTempLTH;
	float	fConfigAntiFrzTempHTH;
	float	fConfigHysAntiFrzPos;
	float	fConfigHysAntiFrzNeg;

	float	fConfigInlineBaseTempSet;
	float	fConfigInlineBaseTempLTH;
	float	fConfigInlineBaseTempHTH;
	float	fConfigHysInlineBasePos;
	float	fConfigHysInlineBaseNeg;

	float	fConfigInlineHardTempSet;
	float	fConfigInlineHardTempLTH;
	float	fConfigInlineHardTempHTH;
	float	fConfigInlineHysHardPos;
	float	fConfigInlineHysHardNeg;
	
	/*Remapping config para*/
	/*VCM*/
	uint8_t	ucConfigConnPressureGuage1;
	uint8_t	ucConfigConnPressureGuage2;
	uint8_t	ucConfigConnPressureGuage3;
	uint8_t ucConfigConnLinearTransducer;
	uint8_t	ucConfigConnInsideEE07;
	uint8_t	ucConfigConnOutsideEE07;
	uint8_t ucConfigConnStallvalve;
	uint8_t ucConfigConnRecircvalve;  /*change12*/
	/*revert*/
	uint8_t	ucConfigConnPressureGuage1Rev;
	uint8_t	ucConfigConnPressureGuage2Rev;
	uint8_t	ucConfigConnPressureGuage3Rev;
	uint8_t ucConfigConnLinearTransducerRev;
	uint8_t	ucConfigConnInsideEE07Rev;
	uint8_t	ucConfigConnOutsideEE07Rev;
	uint8_t ucConfigConnStallvalveRev;
	uint8_t ucConfigConnRecircvalveRev; /*change12*/
	
	/*HCM*/
	uint8_t	ucConfig_ConnBaseInlineTher;
	uint8_t	ucConfig_ConnHardInlineTher;
	uint8_t	ucConfig_ConnAntifreezeTher;
	uint8_t	ucConfig_ConnBasereturnlineTher;
	uint8_t	ucConfig_ConnBaseinlineHeater;
	uint8_t	ucConfig_ConnHardinlineHeater;
	uint8_t	ucConfig_ConnAntifreezeHeater;
	uint8_t	ucConfig_ConnHardHeater;
	uint8_t	ucConfig_ConnBaseTankLL;
	uint8_t	ucConfig_ConnHardTankLL;
	/*revert*/
	uint8_t	ucConfig_ConnBaseInlineTherRev;
	uint8_t	ucConfig_ConnHardInlineTherRev;
	uint8_t	ucConfig_ConnAntifreezeTherRev;
	uint8_t	ucConfig_ConnBasereturnlineTherRev;
	uint8_t	ucConfig_ConnBaseinlineHeaterRev;
	uint8_t	ucConfig_ConnHardinlineHeaterRev;
	uint8_t	ucConfig_ConnAntifreezeHeaterRev;
	uint8_t	ucConfig_ConnHardHeaterRev;
	uint8_t	ucConfig_ConnBaseTankLLRev;
	uint8_t	ucConfig_ConnHardTankLLRev;
	
	/*OPMM*/
	uint8_t	ucConfigConnSprayvalve;
	uint8_t	ucConfigConnGunposition;
	uint8_t	ucConfigConnABvalve;
	uint8_t	ucConfigConnSolventvalve;
	uint8_t	ucConfigConnForwardvalve;
	uint8_t	ucConfigConnBackwardvalve;
	uint8_t ucConfigNCTempSensor1;
	uint8_t ucConfigNCTempSensor2;
	uint8_t ucConfigNCTempSensor3;
	uint8_t ucConfigNCTempSensor4;
	/*revert*/
	uint8_t	ucConfigConnSprayvalveRev;
	uint8_t	ucConfigConnGunpositionRev;
	uint8_t	ucConfigConnABvalveRev;
	uint8_t	ucConfigConnSolventvalveRev;
	uint8_t	ucConfigConnForwardvalveRev;
	uint8_t	ucConfigConnBackwardvalveRev;
	uint8_t ucConfigNCTempSensor1Rev;
	uint8_t ucConfigNCTempSensor2Rev;
	uint8_t ucConfigNCTempSensor3Rev;
	uint8_t ucConfigNCTempSensor4Rev;
	
	/*BYPASS config variables*/
	uint16_t uiConfigVCMBypass;
	uint16_t uiConfigHCMBypass;
	uint16_t uiConfigSCUBypass;
	uint16_t uiConfigOPMMBypass;
	
	/*REVERT BYPASS config variables*/
	uint16_t uiConfigVCMBypassRev;
	uint16_t uiConfigHCMBypassRev;
	uint16_t uiConfigSCUBypassRev;
	uint16_t uiConfigOPMMBypassRev;
	
	uint8_t ucBuzzerVolume;
	uint16_t uiSignature;
	
	float fOPMMBatKneeVoltage;
	float fTimerStartStallTest;
	float fTimerSprayLastRotStop;
	
	float fUpstrokeLimit;
	float fUpstrokeMin;
	float fUpstrokeMax;
	float fDownstrokeLimit;
	float fDownstrokeMin;
	float fDownstrokeMax;
	
	float fTimerIdealPeriodic;
	float fPipeDiameter;
	
	/*change1*/
uint8_t 	ucConfig_TimeOffsetSign;	
uint8_t 	ucConfig_TimeOffsetHour;
uint8_t 	ucConfig_TimeOffsetMin;
uint8_t 	ucConfig_TimeOffsetSec;
}CONFIG_Typedef;

typedef struct OperationFlags
{	
	uint8_t bACK_Received:1;
	uint8_t bCANFaultMCU:1;
	uint8_t bCANFaultOPMM:1;	
	uint8_t bHB_Not_Rec:1;
	uint8_t bTxFlag:1;
	uint8_t bCANFaultFlag:1;
	uint8_t bAnyFault:1;
	uint8_t bLaserBeamStatus:1;
	uint8_t bDiagnostic_Status:1;
	uint8_t bOperationModeStart:1;	
	uint8_t bOperationModeCmdSent:1;	
	uint8_t bModeStatusReceived:1;
	uint8_t bHeartBeatMsg:1;
	uint8_t bFirst_HB:1;
	uint8_t bDataInCAN1_BuffFlag:1;	
	uint8_t bOPMMSetOnPipe:1;
	uint8_t bAwayPipePosition:1;
	uint8_t bTowardPipePosition:1;	
	
	uint8_t bInitPrimeProcess:1;
	uint8_t bInitSprayProcess:1;
	uint8_t bInitFlushProcess:1;	
	uint8_t bManualProcessStarted:1;		
	
	uint8_t bPrimeStatusReceived:1;
	uint8_t bFlushStatusReceived:1;
	uint8_t bSprayStatusReceived:1;
	
	/*Prime*/
	uint8_t bPrimeDone:1;
	uint8_t bPrimeStatus:1;
	uint8_t bAutoPrime:1;
	uint8_t bManualPrime:1;
	
	/*Flush*/
	uint8_t bFlushDone:1;
	uint8_t bFlushStatus:1;
	uint8_t bAutoFlush:1;
	uint8_t bManualFlush:1;
	
	/*Spray*/
	uint8_t bSprayDone:1;
	uint8_t bSprayStatus:1;
	uint8_t bAutoSpray:1;
	
	/*OPMM Valve Status Flags*/
	uint8_t OPMM_VL_Forward_status:1;
	uint8_t OPMM_VL_Backward_status:1;
	uint8_t OPMM_VL_Flush_status:1;
	uint8_t OPMM_VL_SprayGun_status:1;
	uint8_t OPMM_VL_SprayHead_status:1;
	uint8_t OPMM_VL_AB_status:1;
	
	/*VCM Valve status Flags*/
	uint8_t VCM_VL_Stall:1;
	uint8_t VCM_VL_Recirc:1; /*change12*/
	
	/*Heaters*/
	uint8_t	bHCM_Base_Heater_Status:1;
	uint8_t	bHCM_Hard_Heater_Status:1;
	uint8_t	bHCM_Antfrz_Heater_Status:1;
	uint8_t	bHCM_Hard_Tank_Heater_Status:1;

	uint8_t bDataInCANTx_BuffFlag:1;
	uint8_t bParameterSet:1;
	uint8_t bUpStrokeStatus:1;
	uint8_t bDownStrokeStatus:1;
	uint8_t bUpstrokeStatusReceived:1;
	uint8_t bDownstrokeStatusReceived:1;
	uint8_t bStallTestStatusReceived:1;
	uint8_t bEstopPressed:1;
	uint8_t ucI2CInterruptFlag:1;
	/*CRC Changes*/
	uint8_t bErrorFlag:1;
	uint8_t bWarningFlag:1;
	uint8_t bWarningMEssageTimer:1;
	uint8_t bSystemErrorFlag:1;
	uint8_t bFlushWarningFlag:1;
	uint8_t bFlushWarnCntAchieved:1;
	uint8_t bStallTestInProgress:1;
	/*Fault Flags*/
	uint8_t bSCU_Error:1;
	uint8_t bOPMM_Error:1;
	uint8_t bHMI_Error:1;
	uint8_t bVCM_Error:1;
	uint8_t bHCM_Error:1;
	
	uint8_t bSCU_Warning:1;
	uint8_t bOPMM_Warning:1;
	uint8_t bHMI_Warning:1;
	uint8_t bVCM_Warning:1;
	uint8_t bHCM_Warning:1;

	uint8_t bSCU_Temp_Bypass:1;
	uint8_t bSCU_Pressure_Bypass:1;
	uint8_t bVCM_AirSensor_Status:1;
	uint8_t bBuzzerVolDecrease:1;
	
	uint8_t bVCMPortSetFail:1;
	uint8_t bHCMPortSetFail:1;
	uint8_t bOPMMPortSetFail:1;
	uint8_t bPortValuesSet:1;
	
	/*change2*/
	/*HMI Diagnostic screen added*/
	uint8_t bDiagnostic_SW_Status:1;
	uint8_t bDiagnostic_NVSRAM_Status:1;

}FLAGS_Typedef;

typedef struct DiagnosticStructure
{
	uint8_t bSCU_DiagStatusReceived:1;
	uint8_t bOPMM_DiagStatusReceived:1;
	uint8_t bVCM_DiagStatusReceived:1;
	uint8_t bHCM_DiagStatusReceived:1;
	
	uint8_t bHMI_DiagStatusSent:1;
	
	uint8_t bSCU_DiagStatus:1;
	uint8_t bOPMM_DiagStatus:1;
	uint8_t bHMI_DiagStatus:1;
	uint8_t bVCM_DiagStatus:1;
	uint8_t bHCM_DiagStatus:1;
	uint8_t bHMI_DiagStatusDisp:1;
	uint8_t bMCU_DiagStatusDisp:1;
}DIAGNOSTIC_Typedef;

typedef union
{
	uint32_t uliCanID;
	uint8_t ucChar[4];
}SwapExtId_TypeDef;

/*********Union for the Rx an Tx the float Value from the Can*****/
typedef union
{
	float fFloat;
	uint16_t uiData;
	int16_t	iData;
	uint8_t ucChar[4];
	int8_t cChar[4];
}SplitByte_TypeDef;

typedef struct DisplayDateTime
{	
	uint8_t ucDD[3];
	uint8_t ucMM[3];
	uint8_t ucYY[5];
	uint8_t ucHR[3];
	uint8_t ucMIN[3];
	uint8_t ucSEC[3];
	/*change1*/

	uint8_t ucOFFHR[3];
	uint8_t ucOFFMIN[3];
	uint8_t ucOFFSEC[3];
	uint8_t ucUTCDD[3];
	uint8_t ucUTCMM[3];
	uint8_t ucUTCYY[5];
	uint8_t ucUTCHR[3];
	uint8_t ucUTCMIN[3];
	uint8_t ucUTCSEC[3];
}Display_DateTime;

typedef union
{
	FLAGS_Typedef stStatusFlag;
	uint32_t uliFlagData[5];
}TypeDefStatus;


/* ----------------------------------------------------------------------------
*                           CONSTANTS
* ----------------------------------------------------------------------------
*/
/* ----------------------------------------------------------------------------
*                           GLOBAL VARIABLES
* ----------------------------------------------------------------------------
*/
extern CanTxMsg stTx1Message;  						/*CAN1 Tx msg */
extern SplitByte_TypeDef SplitByte; 			/* union to split the data as float and char */
extern SwapExtId_TypeDef SwapExtID; 			/* union to ulter the Received ExtId */
extern uint8_t ucRec_Byte[8];							/*Received byte 1 from CAN message*/
extern uint8_t ucDeviceFunc ;							/*Device functionality*/
extern 	uint16_t uiWDTCount;
extern	uint16_t uiAlarm1LEDBlinkCount;		/*variable for period of blinking LED for Larm 1*/
extern	uint8_t ucLEDBlinkCount;					/*variable for period of blinking LED*/
extern	uint16_t uiTxCounter;							/*variable to check resend time time for CAN message*/
extern	uint16_t uiHBTimeCounter;					/*Taken as uint16_t while testing to increase time count to 1 minute*/
extern TypeDefStatus StatusFlag;
extern DATA_Typedef  stCurrent_Value;
extern CONFIG_Typedef  stConfig_Value;
extern DIAGNOSTIC_Typedef stDiagStatus;
extern Display_DateTime stDisplayDateTime;
extern uint8_t ucStoreBuffCnt;
extern uint8_t ucTxBuffCnt;
extern uint8_t ucCnt;
extern CanRxMsg stCANRx1_Rcvbuff[CAN1_BUF_SIZE];
extern uint16_t uiStoreCAN1_BuffCnt;
extern uint16_t uiCAN1_processCnt ;
extern uint32_t uliFlashTimeCounter;
extern uint32_t uliFlushWarningCnt;
extern uint16_t uiSCU_DiagnosticStatus ;
extern uint16_t uiVCM_DiagnosticStatus ;
extern uint16_t uiHCM_DiagnosticStatus ;
extern uint32_t uliOPMM_DIagnosticStatus ;

extern uint32_t uliHmScreenRefreshCnt;

extern uint16_t uiPostLedBlinkCnt;
/* ----------------------------------------------------------------------------
*                           EXTERNAL VARIABLES
* ----------------------------------------------------------------------------
*/
/* ----------------------------------------------------------------------------
*                           EXTERNAL FUNCTIONS
* ----------------------------------------------------------------------------
*/
extern void fnDefaultConfig(void);
extern void fnResetAllFlags(void);
extern void fnDeviceFunctionality_Check(void);
extern void fnClearErrors(void);
extern void fnClearWarnings(void);
extern void fnCAN_Configuration(void);
extern void fnCAN_MsgInit(void);
extern void fnCAN_Transmit(CanTxMsg *,CAN_TypeDef* );
extern void fnProcessCANMsg(CanRxMsg *);
extern void fnValidateCANMsg(CanRxMsg *);
extern void fnSendACK(CanRxMsg *);
extern void fnACK_Check(void);
extern void fnHeartBeat_Check(void);
extern void fnResetAllData(void);
extern void fnResetAllConfig(void);
/* ----------------------------------------------------------------------------
*                           FUNCTIONS DECLARATION
* ----------------------------------------------------------------------------
*/

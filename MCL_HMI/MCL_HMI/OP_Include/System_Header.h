/******************************************************************************
**@file    System_Header.h
* 
**@brief   This file contains 
******************************************************************************
*/    
/* ----------------------------------------------------------------------------
*                           Includes
* ----------------------------------------------------------------------------
*/
#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "usart.h"
#include "stdint.h"
#include <string.h>
#include <stdlib.h>
#include "Can.h"

#include "stm32f10x_usart.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_i2c.h"
#include <stdio.h>

#include "LCD_MACROS.h"
#include "USART_MACROS.h"
#include "SW_MACROS.h"
#include "JOYSTICK_MACROS.h"
#include "CAN_MACROS1.h"
#include "interr.h"
#include "I2C_MACROS.h"

#include <math.h>
/* ----------------------------------------------------------------------------
*                           MACROS
* ----------------------------------------------------------------------------
*/
	/*WDT Pins And Ports*/
	#define WDT_ENABLE_PIN															GPIO_Pin_7
	#define WDTI_PIN																		GPIO_Pin_7
	#define	WDT_ENABLE_PORT															GPIOD
	#define WDTI_PORT																		GPIOA
/******************************************************************/
	/*COM LED PINS*/
	#define PIN_LED_COM_GREEN														GPIO_Pin_1
	#define PIN_LED_COM_RED															GPIO_Pin_5
	#define PIN_LED_COM_BLUE														GPIO_Pin_6 
	/*COM LED PORT*/
	#define PORT_LED_COM_GREEN													GPIOB
	#define PORT_LED_COM_RED														GPIOC
	#define PORT_LED_COM_BLUE														GPIOC
/******************************************************************/	
	/*ERR LED PINS*/
	#define PIN_LED_ERR_GREEN														GPIO_Pin_2
	#define PIN_LED_ERR_RED															GPIO_Pin_3
	#define PIN_LED_ERR_BLUE														GPIO_Pin_0	
	/*ERR LED PORT*/
	#define PORT_LED_ERR_GREEN												  GPIOC
	#define PORT_LED_ERR_RED													  GPIOC
	#define PORT_LED_ERR_BLUE													  GPIOB
/******************************************************************/	
	
	
	
	/*Left top LED*/
	#define PIN_LED_LEFT_TOP_GREEN											GPIO_Pin_6
	#define PIN_LED_LEFT_TOP_RED												GPIO_Pin_7
	#define PIN_LED_LEFT_TOP_BLUE												GPIO_Pin_8
	
	/*left top LED PORT*/
	#define PORT_LED_LEFT_TOP														GPIOF

	
	/*Left bottom LED*/
	#define PIN_LED_LEFT_BOTTOM_GREEN										GPIO_Pin_9
	#define PIN_LED_LEFT_BOTTOM_RED											GPIO_Pin_0
	#define PIN_LED_LEFT_BOTTOM_BLUE										GPIO_Pin_1
	/*left bottom LED PORT*/
	#define PORT_LED_LEFT_BOTTOM_GREEN									GPIOF
	#define PORT_LED_LEFT_BOTTOM_RED										GPIOC
	#define PORT_LED_LEFT_BOTTOM_BLUE										GPIOC
	
	/**************USART*******************/
	#ifdef GENERAL_DBG
		#define RCC_APB2PERIPH_GPIO												RCC_APB2Periph_GPIOA
		#define RCC_APBPERIPH_USART												RCC_APB2Periph_USART1
		#define USART_TX_PIN															GPIO_Pin_9
		#define USART_RX_PIN															GPIO_Pin_10
		#define USART_GPIO_PORT														GPIOA
		#define USART_BAUD_RATE														(uint32_t)115200
		#define USART																			USART1
		#define USART_IRQN																USART1_IRQn
	#endif

	#define SRAM_BASE_ADDR          										(uint32_t)0x68000000
	#define SRAM_SIZE_BYTES        											(uint32_t)0x00080000

	#define WDT_TIME_CHECK															(uint16_t)100
	#define ALL_SW_AT_DEFAULT 													(uint8_t)0
	#define NVSRAM_DIAGNOSTIC_FAIL											(uint8_t)0x01
	#define JS_DIAGNOSTIC_FAIL													(uint8_t)0x02
	#define SW_DIAGNOSTIC_FAIL													(uint8_t)0x04

	#define NVSRAM_TEST_ADDRESS  												(uint32_t)0x00000001	
	#define NVSRAM_BYTE_COUNT														(uint32_t)0x00000001
	#define NVSRAM_TEST_DATA  													(uint8_t)0x3F
	#define NVSRAM_TEST_BYTE														(uint8_t)1

	#define PORT_0																			(uint8_t)0x00
	#define PORT_1																			(uint8_t)0x01

	/*MACROS FOR LED*/
	#define BOARD_STATUS_LED_RED_BIT										(uint8_t)0x01
	#define BOARD_STATUS_LED_BLUE_BIT										(uint8_t)0x02
	#define BOARD_STATUS_LED_GREEN_BIT									(uint8_t)0x04

	#define ALARM1_STATUS_LED_RED_BIT										(uint8_t)0x08
	#define ALARM1_STATUS_LED_BLUE_BIT									(uint8_t)0x10
	#define ALARM1_STATUS_LED_GREEN_BIT									(uint8_t)0x20

	#define ALARM2_STATUS_LED_RED_BIT										(uint8_t)0x40
	#define ALARM2_STATUS_LED_BLUE_BIT									(uint8_t)0x80
	#define ALARM2_STATUS_LED_GREEN_BIT									(uint8_t)0x01

	#define CAN1_STATUS_LED_BIT													(uint8_t)0x02

	#define ALL_PORT_0_LED															(uint8_t)0xFF
	#define ALL_PORT_1_LED															(uint8_t)0x03

	#define TEMPERATURE_VALUE														(uint8_t)0x01
	#define PRESSURE_VALUE															(uint8_t)0x02
	#define MM_TO_INCH																	(uint8_t)0x03
	#define CM_TO_INCH																	(uint8_t)0x04
	
	#define INCH_TO_MM																	(uint8_t)0x05
	#define INCH_TO_CM																	(uint8_t)0x06
	
	/*change6*/
	#define ML_TO_L																			(uint8_t)0x07	
	#define ML_TO_OZ																		(uint8_t)0x08	
	
	#define L_TO_ML																			(uint8_t)0x09	
	#define OZ_TO_ML																		(uint8_t)0x0A																				
  /*change21*/
	#define MM2_TO_INCH2																	(uint8_t)0x0B
	#define INCH2_TO_MM2																	(uint8_t)0x0C
	#define MM3_TO_L																			(uint8_t)0x0D	
	#define MM3_TO_OZ																			(uint8_t)0x0E	
	
	
	#define TRANSMITTER            											(uint8_t)0x00

	#define SYSTEM_WARNING															(uint8_t)0x01
	#define SYSTEM_ERROR																(uint8_t)0x02

	#define PRIME_SELECTOR															(uint8_t)0x01
	#define SPRAY_SELECTOR															(uint8_t)0x02
	#define FLUSH_SELECTOR															(uint8_t)0x04

	/*MACROS for CONFIG Parameters Send*/
	#define ONE_BYTES																		(uint8_t)1
	#define TWO_BYTES																		(uint8_t)2
	#define FOUR_BYTES																	(uint8_t)4
	#define FIVE_BYTES																	(uint8_t)5
	#define SEVEN_BYTES																	(uint8_t)7
	#define EIGHT_BYTES																	(uint8_t)8

	#define INVALID_DATA																(float)2999.0f
		
	/**************From File Task.c******************/
	#define ESTOP_PRESSED																(uint8_t)0x00
	#define ESTOP_RELEASED															(uint8_t)0x03

	#define	ROW1																				(uint8_t)0
	#define	ROW2																				(uint8_t)1
	#define	ROW3																				(uint8_t)2

	#define COLUMN1																			(uint8_t)0
	#define COLUMN2																			(uint8_t)1
	#define COLUMN3																			(uint8_t)2
	#define COLUMN4																			(uint8_t)3
	#define COLUMN5																			(uint8_t)4
	#define COLUMN6																			(uint8_t)5
	#define COLUMN7																			(uint8_t)6
	#define COLUMN8																			(uint8_t)7
	#define COLUMN9																			(uint8_t)8
	#define COLUMN10																		(uint8_t)9
	#define COLUMN11																		(uint8_t)10
	#define COLUMN12																		(uint8_t)11
	#define COLUMN13																		(uint8_t)12

	#define LETTER_WIDTH																(uint8_t)16
	#define DISP_LETTER_START_POS												(uint16_t)340

	#define LETTER_POS_X(POSITIONX)											(	POSITIONX == 149 ? ROW1 : \
																												POSITIONX == 179 ? ROW2 : ROW3)
																						
	#define LETTER_POS_Y(POSITIONY)											(	POSITIONY == 366 ? COLUMN1 : \
																												POSITIONY == 336 ? COLUMN2 : \
																												POSITIONY == 306 ? COLUMN3 : \
																												POSITIONY == 276 ? COLUMN4 : \
																												POSITIONY == 246 ? COLUMN5 : \
																												POSITIONY == 216 ? COLUMN6 : \
																												POSITIONY == 186 ? COLUMN7 : \
																												POSITIONY == 156 ? COLUMN8 : \
																												POSITIONY == 126 ? COLUMN9 : \
																												POSITIONY == 96 ? COLUMN10 : \
																												POSITIONY == 66  ? COLUMN11 : \
																												POSITIONY == 36  ? COLUMN12 : COLUMN13)
																												
	#define DIGIT_5																			(float)5.0f
	#define DIGIT_9																			(float)9.0f
	#define DIGIT_32																		(float)32.0f
		
	#define MM_TO_INCH_FACTOR														(float)0.03937f
	#define CM_TO_INCH_FACTOR														(float)0.3937f
		
	#define INCH_TO_MM_FACTOR														(float)25.4f
	#define INCH_TO_CM_FACTOR														(float)2.54f
	
	/*change6*/
	#define ML_TO_OZ_FACTOR															(float)0.033814f
	#define OZ_TO_ML_FACTOR															(float)29.5735f	
	#define ML_TO_L_FACTOR															(float)0.001f
	#define L_TO_ML_FACTOR															(float)1000.0f
	
	#define CONVERT_MM_TO_INCH(CURR_VALUE)							(float)(CURR_VALUE * MM_TO_INCH_FACTOR)
	#define CONVERT_INCH_TO_MM(CURR_VALUE)							(float)(CURR_VALUE * INCH_TO_MM_FACTOR)
			
	#define CONVERT_CM_TO_INCH(CURR_VALUE)							(float)(CURR_VALUE * CM_TO_INCH_FACTOR)
	#define CONVERT_INCH_TO_CM(CURR_VALUE)							(float)(CURR_VALUE * INCH_TO_CM_FACTOR)
	
	/*change21*/
	#define MM2_TO_INCH2_FACTOR															(float)0.00155f
	#define INCH2_TO_MM2_FACTOR															(float)645.16f
	#define CONVERT_MM2_TO_INCH2(CURR_VALUE)							(float)(CURR_VALUE * MM2_TO_INCH2_FACTOR)
	#define CONVERT_INCH2_TO_MM2(CURR_VALUE)							(float)(CURR_VALUE * INCH2_TO_MM2_FACTOR)
	#define MM3_TO_OZ_FACTOR															(float)0.000033814f
	#define MM3_TO_L_FACTOR															(float)0.000001f
	#define CONVERT_MM3_TO_OZ(CURR_VALUE)							(float)(CURR_VALUE * MM3_TO_OZ_FACTOR)
	#define CONVERT_MM3_TO_L(CURR_VALUE)							(float)(CURR_VALUE * MM3_TO_L_FACTOR)
	
	
	#define PSI_TO_BAR(CURRENT_PRESSURE)								(float)(CURRENT_PRESSURE * 0.068947)
	#define BAR_TO_PSI(CURRENT_PRESSURE)								(float)(CURRENT_PRESSURE * 14.5038)

	#define CELSIUS_TO_FAHRENHEIT(CUR_TEMP)							(float)((CUR_TEMP * (DIGIT_9/DIGIT_5))+DIGIT_32)
	#define FAHRENHEIT_TO_CELSIUS(CUR_TEMP)							(float)((CUR_TEMP - DIGIT_32)*(DIGIT_5/DIGIT_9))
	
	/*change6*/
	#define CONVERT_ML_TO_OZ(CURR_MAT_VOLUME_TO_OZ)			(float)(CURR_MAT_VOLUME_TO_OZ * ML_TO_OZ_FACTOR)
	#define CONVERT_OZ_TO_ML(CURR_MAT_VOLUME_TO_ML)			(float)(CURR_MAT_VOLUME_TO_ML * OZ_TO_ML_FACTOR)	
	#define CONVERT_ML_TO_L(CURR_MAT_VOLUME_TO_L)				(float)(CURR_MAT_VOLUME_TO_L * ML_TO_L_FACTOR)
	#define CONVERT_L_TO_ML(CURR_MAT_VOLUME_TO_ML)			(float)(CURR_MAT_VOLUME_TO_ML * L_TO_ML_FACTOR)
	
	
	#define STATUS_LED_BLINK_COUNT											(uint16_t)150

	
	#define PRESSURE_LOW_LIMIT													(float)0.0f
	#define PRESSURE_HIGH_LIMIT													(float)250.0f
		
	#define PRESSURE_LOW_LIMIT_VALIDATION								(float)( stConfig_Value.ucTypeofUnit == METRIC ? \
																																		PRESSURE_LOW_LIMIT : PRESSURE_LOW_LIMIT) 
	#define PRESSURE_HIGH_LIMIT_VALIDATION							(float)( stConfig_Value.ucTypeofUnit == METRIC ? \
																																		PRESSURE_HIGH_LIMIT : BAR_TO_PSI(PRESSURE_HIGH_LIMIT))
		
	#define TEMPERATURE_LOW_LIMIT												(float)0.0f
	#define TEMPERATURE_HIGH_LIMIT											(float)100.0f
		
	#define TEMPERATURE_LOW_LIMIT_VALIDATION						(float)( stConfig_Value.ucTypeofUnit == METRIC ? \
																																		TEMPERATURE_LOW_LIMIT : CELSIUS_TO_FAHRENHEIT(TEMPERATURE_LOW_LIMIT))
		
	#define TEMPERATURE_HIGH_LIMIT_VALIDATION						(float)( stConfig_Value.ucTypeofUnit == METRIC ? \
																																		TEMPERATURE_HIGH_LIMIT : CELSIUS_TO_FAHRENHEIT(TEMPERATURE_HIGH_LIMIT))
		
	#define STALL_NEGATIVE_TH_VALIDATION_MIN						(int16_t)0
	#define STALL_NEGATIVE_TH_VALIDATION_MAX						(int16_t)100

	#define STALL_POSITIVE_TH_VALIDATION_MIN						(int16_t)0
	#define STALL_POSITIVE_TH_VALIDATION_MAX						(int16_t)500
	
	#define TIMER_LOW_LIMIT_VALIDATION									(int16_t)0
	#define TIMER_HIGH_LIMIT_VALIDATION									(int16_t)9999
	
	#define LL_LOW_LIMIT																(int16_t)0
	#define LL_HIGH_LIMIT																(int16_t)9999
	
	#define LL_LOW_LIMIT_VALIDATION											(float)( stConfig_Value.ucTypeofUnit == METRIC ? \
																																		LL_LOW_LIMIT : LL_LOW_LIMIT)
	
	#define LL_HIGH_LIMIT_VALIDATION										(float)( stConfig_Value.ucTypeofUnit == METRIC ? \
																																		LL_HIGH_LIMIT : BAR_TO_PSI(LL_HIGH_LIMIT))
	
	#define MODULE_SCU																	(uint8_t)0x02
	#define MODULE_OPMM																	(uint8_t)0x03
	#define MODULE_HCM																	(uint8_t)0x04
	#define MODULE_VCM																	(uint8_t)0x05
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
	extern uint8_t ucIOExpader1Low;
	extern uint8_t ucIOExpader1High;
	extern uint8_t ucrgStoredStatusMessage[24][75];
	extern uint8_t ucStatusCountLimit;
	extern uint8_t ucrgStoredAlarmMessage[24][75];
	extern uint8_t rgucModule[24];
	extern uint8_t rgucAlarmTpe[24];
	extern uint8_t ucAlarmCountLimit;
	extern uint8_t rgucSupervisorName[22];
	extern uint8_t rgucCoatingMaterial[22];
	extern uint8_t rgucBaseMatBatchNumber[22];
	extern uint8_t rgucHardMatBatchNumber[22];
	
	extern uint8_t rgucJobName[22];
	
	/*change7*/
	extern uint8_t rgucJobId[22];
	/*change3*/
	extern uint8_t rgucSSID[22];
	extern uint8_t rgucPass[22];
	extern uint8_t ucLastProcessDone;
	extern uint8_t ucMaterialDataReceived;
	extern uint8_t ucpDisplayLetter;
	extern uint8_t *ucAlphaNum[3][13];
	extern uint16_t uiLetter;
	extern uint16_t uiLetterStartPos;
	extern uint8_t ulirgStoreLetter[30];
	extern uint8_t ucLetterPos;
	extern uint8_t ucResetVariableCounter;
	extern uint8_t ucOtherSetVariableCounter;
	extern uint8_t ucDiagModuleVariable;
	extern uint8_t ucLiquidLSetVariableCounter1;
	extern uint8_t ucPresSetVariableCounter;
	extern uint8_t ucTempSetVariableCounter;
	extern uint8_t ucPumpSetVariableCounter;
	extern uint8_t ucJobSetVariableCounter;
	extern uint8_t ucTimerSetVariableCounter;
	extern uint8_t ucStallSetVariableCounter;
	extern uint8_t ucHystSetVariableCounter;
	
	extern uint8_t ucVCMPeripheralSetCounter;
	extern uint8_t ucHCMPeripheralSetCounter;
	extern uint8_t ucOPMMPeripheralSetCounter;
	extern uint8_t ucSCUPeripheralSetCounter;	
	/*change3*/
	extern uint8_t ucETHIPSetVariableCounter;
	extern uint8_t ucWIFIIPSetVariableCounter;
	extern uint8_t ucRedColor;
	extern uint8_t ucGreenColor;
	extern uint8_t ucBlueColor;
	
	/*change4*/
	extern uint8_t ucrgStoredMemoryAlarmMessage[24][75];
	extern uint8_t ucMemoryAlarmCountLimit;
	extern uint8_t ucMemoryAlarmMessageCounter;
/* ----------------------------------------------------------------------------
*                           EXTERNAL FUNCTIONS
* ----------------------------------------------------------------------------
*/
	extern void fnAlarmMessage_Store(uint8_t ucrgStr[50],uint8_t ucModuleNum,uint8_t ucAlarmtype);
	extern void fnStatusMessage_Store(uint8_t ucrgStr[50]);
	extern void fnCANMessage_Send(uint32_t ,uint32_t ,uint8_t );
	extern void fnTxBuff_Store(CanTxMsg*);
	extern void fnTxBuff_Shift(void);
	extern void fnSelfDiagnostics_Test(void);
	extern bool NV_Write(uint32_t uliAddress, void *pData, uint32_t uliCount);
	extern bool NV_Read(uint32_t uliAddress, void *pData, uint32_t uliCount);
	extern void fnLED_TurnONOFF (GPIO_TypeDef *GPIOx,BitAction bLEDStatus,uint16_t bPinNumber);
	extern void fnUSART_Configuration(void);
	extern void Dly1us(void);
	extern float fnMetricToImperial_Conv(float fCurrentData,uint8_t ucTypeOfData);
	extern float fnImperialToMetric_Conv(float fCurrentData,uint8_t ucTypeOfData);
	extern void fnOPConfig_Send(uint32_t uliMessageId,uint8_t ucTypeOfData);
	extern void fnProcessFlags_Clear(void);
	extern void fnCheckJoystick_Press(uint8_t ucCurrentJoystickNum);
	extern void fnCurrentScreen_Display(uint8_t uiCurrentScreen);
	extern void fnFlashMessage_Display(uint8_t *pucStr);
	extern void fnStringColor_Set(uint8_t ucFlagStatus);
	extern void fnAlarmMessage_Clear(uint8_t ucModuleNum,uint8_t ucAlarmType);
	extern void fnEStop_Check(void);
	extern void fnBoardStatusRedLed_Blink(void);
	extern void fnAlarm1StatusRedLed_Blink(void);
	extern void fnWDT_Check(void);
	extern void fnWDT_Initialize(void);
	extern void fnSystem_Init(void);
	extern void fnSwitchPress_Check(uint8_t ucCurrentSwitchNum);
	extern void fnCursorPosition_Setting(void);
	extern void fnCurrentScreen_Pattern(void);
	extern void fnFlushWarning_Check(void);
	
	extern void fnConvertConfigToMatric(void);
	extern void fnConvertConfigToImperial(void);
	
	extern void TimeConvert(void);
	/*change4*/
	extern void fnMemory_Alarm_Message_Store(uint8_t ucrgStr[50]);
	/*change4*/
	extern void fnMemory_Alarm_Message_Clear(void);
/* ----------------------------------------------------------------------------
*                           FUNCTIONS DECLARATION
* ----------------------------------------------------------------------------
*/

/******************************************************************************
  * @file    I2C.h
  * 
  * @brief   This file contains included files, Macros, Global Variables,
						 External Variables, External Functions
  ****************************************************************************
  */
	#ifndef __I2C_h
	#define __I2C_h
/* ----------------------------------------------------------------------------
 *                           Includes
 * ---------------------------------------------------------------------------
 */
/* ----------------------------------------------------------------------------
 *                           MACROS
 * ---------------------------------------------------------------------------
 */
  /*EE07 Read Commands*/
  #define TEMP_LOW_BYTE_COMMAND						(uint8_t)0xA1
	#define TEMP_HIGH_BYTE_COMMAND					(uint8_t)0xB1
	#define HUM_LOW_BYTE_COMMAND						(uint8_t)0x81
	#define HUM_HIGH_BYTE_COMMAND						(uint8_t)0x91
	#define STATUS_BYTE_COMMAND							(uint8_t)0x71
 	/*Pins for I2C*/
	/*I2C for IOExpander for LEDs*/
	#define I2C1_SCL												GPIO_Pin_6					
	#define I2C1_SDA												GPIO_Pin_7					
	#define I2C1_PORT												GPIOB
	#define I2C_IO_EXP_SLOCK_SPEED					(uint32_t)100000	/*100kHz*/
	#define I2C_IO_EXP											I2C1
	
	/*I2C for interfacing with Temp and Humidity Sensor(EE07)*/
	#define I2C2_SCL												GPIO_Pin_10 	//J3.17==>SCL				
	#define I2C2_SDA												GPIO_Pin_11 	//J3.16==>SDA								
	#define I2C2_PORT												GPIOB
	#define I2C_EE07_SLOCK_SPEED						(uint32_t)5000	/*5kHz*/
	#define I2C_EE07												I2C2
	#define EE07_I2C_EV_IRQn								I2C2_EV_IRQn
	#define EE07_I2C_ER_IRQn								I2C2_ER_IRQn	
	
	
	#define I2C_SLAVE_WRITE									(uint8_t)0xFE		
	#define I2C_SLAVE_READ									(uint8_t)0x01

	#define IOEXP_1_SLAVE_ADDR							(uint8_t)0x42					
	#define IOEXP_2_SLAVE_ADDR							(uint8_t)0x44							

	#define MUX_ADDR												(uint8_t)0xE0
	#define I2CMUX_CHANNEL0									(uint8_t)0x04
	#define I2CMUX_CHANNEL1									(uint8_t)0x05
	#define I2CMUX_CHANNEL2									(uint8_t)0x06
	#define I2CMUX_CHANNEL3									(uint8_t)0x07
	#define EE07_READ_TIME									(uint8_t)10							/*50 * 10 = 500 msec*/
	
	#define CHANNEL_OUT											(uint8_t)0				
	#define	READ_OUT_TEMP_LOW_BYTE 					(uint8_t)1
	#define	READ_OUT_TEMP_HIGH_BYTE 				(uint8_t)2 
	#define	READ_OUT_HUMID_LOW_BYTE 				(uint8_t)3 
	#define	READ_OUT_HUMID_HIGH_BYTE 				(uint8_t)4 
	#define	READ_OUT_STATUS  								(uint8_t)5
	#define CHANNEL_IN											(uint8_t)6				
	#define	READ_IN_TEMP_LOW_BYTE 					(uint8_t)7
	#define	READ_IN_TEMP_HIGH_BYTE 					(uint8_t)8 
	#define	READ_IN_STATUS 									(uint8_t)9 
	
	/****** Expander 1 ******* SLAVE 1 *****/
	#define STALL_VALVE_RED_LED						(uint16_t)0x0001
	#define STALL_VALVE_BLUE_LED					(uint16_t)0x0002
	#define STALL_VALVE_GREEN_LED					(uint16_t)0x0004

	#define LT_RED_LED										(uint16_t)0x0008
	#define LT_BLUE_LED										(uint16_t)0x0010
	#define LT_GREEN_LED									(uint16_t)0x0020

	#define UPS_RED_LED										(uint16_t)0x0040
	#define UPS_BLUE_LED									(uint16_t)0x0080
	#define UPS_GREEN_LED									(uint16_t)0x0100

	#define PG1_RED_LED										(uint16_t)0x0200
	#define PG1_BLUE_LED									(uint16_t)0x0400
	#define PG1_GREEN_LED									(uint16_t)0x0800
	
	#define PG2_RED_LED									 	(uint16_t)0x1000
	#define PG2_BLUE_LED						  		(uint16_t)0x2000
	#define PG2_GREEN_LED									(uint16_t)0x4000

	/****** Expander 2 ******* SLAVE 2 *****/
	#define PG3_RED_LED									 	(uint16_t)0x0001
	#define PG3_BLUE_LED						  		(uint16_t)0x0002
	#define PG3_GREEN_LED									(uint16_t)0x0004

	#define APS_RED_LED									 	(uint16_t)0x0008
	#define APS_BLUE_LED						  		(uint16_t)0x0010
	#define APS_GREEN_LED									(uint16_t)0x0020

										

	#define IO1_ALL_RED_LED								(uint16_t)0x1249
	#define IO2_ALL_RED_LED								(uint16_t)0x1249
	#define IO1_ALL_BLUE_LED							(uint16_t)0x2492
	#define IO2_ALL_BLUE_LED							(uint16_t)0x2492
	#define IO1_ALL_GREEN_LED							(uint16_t)0xC924
	#define IO2_ALL_GREEN_LED							(uint16_t)0xC924
	
	#define POST_LED_BLINK_COUNT					(uint8_t)20


	#define RED														(uint16_t)1	
	#define BLUE													(uint16_t)2
	#define GREEN													(uint16_t)3

	#define STALL_VALVE_LED_VALUE(x)		  ((RED == x)?  STALL_VALVE_RED_LED : (BLUE == x)? \
																					STALL_VALVE_BLUE_LED : STALL_VALVE_GREEN_LED)
		
	#define LT_LED_VALUE(x)								((RED == x)?  LT_RED_LED: (BLUE == x)? LT_BLUE_LED: LT_GREEN_LED)
		
	#define UPS_LED_VALUE(x)							((RED == x)?  UPS_RED_LED: (BLUE == x)? UPS_BLUE_LED: UPS_GREEN_LED)

	#define PG_LED_VALUE(x , y)						((0 == x)? ((RED == y)? PG1_RED_LED: (BLUE == y)? PG1_BLUE_LED: PG1_GREEN_LED): \
																				 (1 == x)? ((RED == y)? PG2_RED_LED: (BLUE == y)? PG2_BLUE_LED: PG2_GREEN_LED): \
																									 ((RED == y)? PG3_RED_LED: (BLUE == y)? PG3_BLUE_LED: PG3_GREEN_LED))

	#define APS_LED_VALUE(x)							((RED == x)?  APS_RED_LED: (BLUE == x)? APS_BLUE_LED: APS_GREEN_LED)
	
	#define AMBIANT_TEMP_OUT_CALC 							READ_OUT_HUMID_LOW_BYTE//(uint8_t)2 
	#define AMBIANT_HUMIDITY_OUT_CALC 					READ_OUT_STATUS//(uint8_t)4 
	#define AMBIANT_STATUS_OUT_CALC 						READ_OUT_STATUS + 1//(uint8_t)5 
	#define AMBIANT_TEMP_IN_CALC 								READ_IN_STATUS//(uint8_t)7 
	#define AMBIANT_STATUS_IN_CALC 							READ_IN_STATUS + 1 //(uint8_t)8 
	#define GET_EE07_CHANNEL(x)									((1 == x)? I2CMUX_CHANNEL3: \
																							(2 == x)? I2CMUX_CHANNEL2: I2CMUX_CHANNEL1)
																							
	#define CONSTANT_A												17.27f
	#define CONSTANT_B												237.3f		
/* ----------------------------------------------------------------------------
 *                           CONSTANTS
 * ---------------------------------------------------------------------------
 */
/* ----------------------------------------------------------------------------
 *                           GLOBAL VARIABLES
 * ---------------------------------------------------------------------------
 */
/* ----------------------------------------------------------------------------
 *                           EXTERNAL VARIABLES
 * ---------------------------------------------------------------------------
 */
	extern uint8_t ucI2CRxCnt;
	extern uint8_t ucTempHighByte;
	extern uint8_t ucTempLowByte;
	extern uint8_t rgucI2CRxBuff[2];
	extern uint8_t ucEE07SlaveAddr;
	extern uint8_t ucI2CSequence; 
	extern uint8_t ucInsideStatus; 
	extern uint8_t ucOutsideStatus; 
	extern float fInsideTemp;
	extern float fOutsideTemp;
	extern float fOutsideHumidity;
	extern float fDewPoint;
	
	extern uint16_t uiI2CWriteData;
	extern uint8_t ucI2C1Seq;
	extern uint8_t ucIOExpSlaveAddr;

 /* ---------------------------------------------------------------------------
 *                           EXTERNAL FUNCTIONS
 * ---------------------------------------------------------------------------
 */
	extern void fnI2C_Config(void);
	extern void fnI2C_Write(I2C_TypeDef*, uint8_t );
	extern uint8_t fnI2C_ReadNack(I2C_TypeDef*);
	extern uint8_t fnI2C_ReadAck(I2C_TypeDef*);
	extern void fnI2C_Stop(I2C_TypeDef*);
	extern void fnI2C_Start(I2C_TypeDef*, uint8_t, uint8_t);
	extern void fnIOExpander_Write(uint8_t, uint16_t);
	extern uint16_t fnGPIOExpander_Read(uint8_t);
	extern void fnI2CMUX_Write(uint8_t ucSlaveAddr, uint8_t ucControlRegData);

	extern void fnAmbiant_Read(uint8_t);
	extern void fnEE07_Process(void);
	extern void fnEE07_Calculation(void);
	extern void fnDelay_ms(unsigned int uiDly);
/* ----------------------------------------------------------------------------
 *                           FUNCTIONS DECLARITION
 * ---------------------------------------------------------------------------
 */
	#endif
	

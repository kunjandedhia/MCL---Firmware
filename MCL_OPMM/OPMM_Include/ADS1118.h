/******************************************************************************
** @file  :  	ADS1118.h
** 
** @brief :		This Header File contains all the function declaration, Macros and
							global variables related to thermocouple.
******************************************************************************
*/    
/* ----------------------------------------------------------------------------
*                           Includes
* ----------------------------------------------------------------------------
*/
/* ----------------------------------------------------------------------------
*                           MACROS
* ----------------------------------------------------------------------------
*/

/****************************  REGISTER CONFIG *******************************/


#define SS_MODE															(uint16_t)0x8000

#define MUX_MASK														(uint16_t)0x7000

#define MUX_AIN0_AIN1												(uint16_t)0x0000
#define MUX_AIN0_AIN3												(uint16_t)0x1000
#define MUX_AIN1_AIN3												(uint16_t)0x2000
#define MUX_AIN2_AIN3												(uint16_t)0x3000
#define MUX_AIN0_GND												(uint16_t)0x4000
#define MUX_AIN1_GND												(uint16_t)0x5000
#define MUX_AIN2_GND												(uint16_t)0x6000
#define MUX_AIN3_GND												(uint16_t)0x7000

#define PGA_MASK														(uint16_t)0x0E00

#define PGA_6144V														(uint16_t)0x0000
#define PGA_4096V														(uint16_t)0x0200
#define PGA_2048V														(uint16_t)0x0400
#define PGA_1024V														(uint16_t)0x0600
#define PGA_0512V														(uint16_t)0x0800
#define PGA_0256V														(uint16_t)0x0A00

#define MODE								 								(uint16_t)0x0100

#define DATA_RATE_MASK											(uint16_t)0x00E0

#define DR_8_SPS														(uint16_t)0x0000
#define DR_16_SPS														(uint16_t)0x0020
#define DR_32_SPS														(uint16_t)0x0040
#define DR_64_SPS														(uint16_t)0x0060
#define DR_128_SPS													(uint16_t)0x0080
#define DR_250_SPS													(uint16_t)0x00A0
#define DR_475_SPS													(uint16_t)0x00B0
#define DR_860_SPS													(uint16_t)0x00E0

#define TS_MODE															(uint16_t)0x0010

#define PULL_UP_EN													(uint16_t)0x0008

#define INVALID_CONFIG_DATA									(uint16_t)0x0006
#define VALID_CONFIG_DATA										(uint16_t)0x0002

#define NOT_USE															(uint16_t)0x0001

#define DEFAULT_SETTING											(MUX_AIN2_AIN3 | PGA_1024V |  DR_128_SPS | \
																						TS_MODE | PULL_UP_EN | VALID_CONFIG_DATA | NOT_USE)

//#define MUX_CHANNEL_CONFIG(x)							(((x == 1) || (x == 3) || (x == 5))? MUX_AIN0_AIN1 : MUX_AIN2_AIN3)

//#define MUX_CHANNEL_CONFIG(x)								(((x == 3) || (x == 5))? MUX_AIN0_AIN1 : MUX_AIN2_AIN3)

/*************************  REGISTER CONFIG ENDS  ****************************/
#define TEMP_COLUMN        					  			(uint8_t)0
#define VOLT_COLUMN         								(uint8_t)1
																 																	 
#define RES_NUM												  		1.024f//0.256f//0.512f//0.256f//0.128f//0.128f
#define RES_DEN															32768.0f///*16777216*/8388608.0f

#define COLD_JUCNTION										 		(uint8_t)2
#define THERMOCOUPLE										 		(uint8_t)1

#define COLDJ_RESOLUTION										0.03125f





/******** SPI for temperature Sensor TC(thermocouple) *********/
	#define TC_SCK 													GPIO_Pin_13
	#define TC_MISO 												GPIO_Pin_14
	#define TC_MOSI 														GPIO_Pin_15
	#define TC_PORT															GPIOB
	#define TC_CS1_PORT												 GPIOB	
	#define TC_CS2_PORT												 GPIOG	
	#define TC_CS1_PIN												 GPIO_Pin_12		
	#define TC_CS2_PIN												 GPIO_Pin_8	
	
	#define TC_CS_1_PIN												GPIO_Pin_12
	#define TC_CS_2_PIN												GPIO_Pin_8



#define TC_SPI_NUMBER												SPI2


#define ADS1118_SELECT(x)    							 	((x == 0)? GPIO_WriteBit(TC_CS1_PORT, TC_CS_1_PIN, Bit_RESET) : \
																											 GPIO_WriteBit(TC_CS2_PORT, TC_CS_2_PIN, Bit_RESET))
																				
#define ADS1118_UNSELECT(x)  								((x == 0)? GPIO_WriteBit(TC_CS1_PORT, TC_CS_1_PIN, Bit_SET) : \
																											 GPIO_WriteBit(TC_CS2_PORT, TC_CS_2_PIN, Bit_SET))

#define NO_OF_TC_SENSOR											(uint8_t)2
#define INVALID_FLOAT_VALUE									2999.0

#define TC_WINDOW_SIZE									 		(uint8_t)10

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
typedef struct
{
	uint8_t bWindowOverflow:1;					/*This will indicate that window overflows*/
	uint8_t ucThermoPosition;						/*for Position of the thermocouple connector*/
	float fCurrentTemp;									/*Current Temp data*/	
	float rgfTempBuff[TC_WINDOW_SIZE];  /*current Temp Buffer*/	
	uint8_t ucTCFault;									/*Counter for fault detection of the thermocouple*/
	uint8_t ucTraverse;									/*variable for traversing throw the window*/
	float fSum;													/*summation of all temp values stored in the window*/
}TypeDef_TC_Read;

extern TypeDef_TC_Read stTCRead[2];
/* ----------------------------------------------------------------------------
*                           EXTERNAL FUNCTIONS
* ----------------------------------------------------------------------------
*/
extern uint16_t fnADS1118_ReadData(uint16_t);
extern void fnReadTemp(void);
extern float fnTempToVoltage(float);
extern float fnVoltageToTemp(float);
extern uint16_t fnTCSPI_ReadWrite(uint16_t);
/* ----------------------------------------------------------------------------
*                           FUNCTIONS DECLARATION
* ----------------------------------------------------------------------------
*/

/******************************************************************************
**@file     :   I2C_Driver.h
* 
* @brief  This File Contain all the MACROS and extern and global Funtion and Variable Decleration 
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
#define SLAVE1_ADDRESS									(uint8_t)0x42
#define I2C_IO_EXP													I2C1
#define I2C_SLAVE_WRITE									(uint8_t)0xFE
///*IO EXPANDER 1 ==> Address 0x42 PORT 0 */
//#define PRESSUREREGULATOR_RED_LED							(uint8_t)0x01   	 	/* P00 */
//#define PRESSUREREGULATOR_BLUE_LED						(uint8_t)0x02				/* P01 */
//#define PRESSUREREGULATOR_GREEN_LED						(uint8_t)0x04				/* P02 */
//#define CAN1_STATUS_RED_LED										(uint8_t)0x08				/* P03 */
//#define CAN1_STATUS_BLUE_LED									(uint8_t)0x10				/* P04 */
//#define CAN1_STATUS_GREEN_LED									(uint8_t)0x20				/* P05 */
//#define CAN2_STATUS_RED_LED										(uint8_t)0x40				/* P06 */
//#define CAN2_STATUS_BLUE_LED									(uint8_t)0x80				/* P07 */

///*IO EXPANDER 1 ==> Address 0x42 PORT 1 */
//#define CAN2_STATUS_GREEN_LED									(uint16_t)(0x01 << 8)    		/* P10 */
//#define BOARD_STATUS_RED_LED									(uint16_t)(0x02	<< 8)			/* P11 */
//#define BOARD_STATUS_BLUE_LED									(uint16_t)(0x04	<< 8)			/* P12 */
//#define BOARD_STATUS_GREEN_LED								(uint16_t)(0x08	<< 8)			/* P13 */
//#define ESTOP_RED_LED													(uint16_t)(0x10	<< 8)			/* P14 */
//#define ESTOP_BLUE_LED												(uint16_t)(0x20	<< 8)			/* P15 */
//#define ESTOP_GREEN_LED												(uint16_t)(0x40	<< 8)			/* P16 */

/*IO EXPANDER 1 ==> Address 0x42 PORT 0 */
#define PRESSUREREGULATOR_RED_LED							(uint8_t)0   	 	/* P00 */
#define PRESSUREREGULATOR_BLUE_LED						(uint8_t)1				/* P01 */
#define PRESSUREREGULATOR_GREEN_LED						(uint8_t)2				/* P02 */
#define CAN1_STATUS_RED_LED										(uint8_t)3				/* P03 */
#define CAN1_STATUS_BLUE_LED									(uint8_t)4				/* P04 */
#define CAN1_STATUS_GREEN_LED									(uint8_t)5				/* P05 */
#define CAN2_STATUS_RED_LED										(uint8_t)6				/* P06 */
#define CAN2_STATUS_BLUE_LED									(uint8_t)7				/* P07 */

/*IO EXPANDER 1 ==> Address 0x42 PORT 1 */
#define CAN2_STATUS_GREEN_LED									(uint16_t)8    		/* P10 */
#define BOARD_STATUS_RED_LED									(uint16_t)9			/* P11 */
#define BOARD_STATUS_BLUE_LED									(uint16_t)10			/* P12 */
#define BOARD_STATUS_GREEN_LED								(uint16_t)11			/* P13 */
#define ESTOP_RED_LED													(uint16_t)12			/* P14 */
#define ESTOP_BLUE_LED												(uint16_t)13			/* P15 */
#define ESTOP_GREEN_LED												(uint16_t)14			/* P16 */
#define RESET_CONFIG_RED											(uint16_t)0x1249	/* P16 */
#define RESET_CONFIG_BLUE											(uint16_t)0x2492	/* P16 */

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
extern uint16_t uiIOExpData;
extern uint8_t ucIOExpSlaveAddr;
extern uint8_t	ucI2C1Seq;
extern uint16_t uiI2CWriteData;
extern uint16_t uiPreIOExpander;
/* ----------------------------------------------------------------------------
*                           EXTERNAL FUNCTIONS
* ----------------------------------------------------------------------------
*/
extern void fnI2C_Configuration(void);
extern void fnI2C_Write(I2C_TypeDef*, uint8_t );
extern uint8_t fnI2C_ReadNack(I2C_TypeDef*);
extern uint8_t fnI2C_ReadAck(I2C_TypeDef*);
extern void fnI2C_Stop(I2C_TypeDef*);
extern void fnI2C_Start(I2C_TypeDef*, uint8_t, uint8_t);
extern float fnEE07Temp_Read(I2C_TypeDef*);
extern float fnEE07Humidity_Read(I2C_TypeDef*);
extern uint8_t fnEE07Status_Read(I2C_TypeDef*);
extern void fnIOExpander_Write(uint8_t, uint8_t, uint8_t);



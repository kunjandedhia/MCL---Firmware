/******************************************************************************
**@file    JOYSTICK_MACROS.h
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
#define MAX_COMMAND_ARGUMENTS                         (uint8_t)9
/***********joystick**************************/
#define SERVICE_INTERVAL_TICKS_J      								(uint8_t)50
#define UPDATE_TOLERANCE            									(uint16_t)256
#define JSCAL_HISTORY_LENGTH        									(uint8_t)64
#define JSCAL_HISTORY_MARGIN        									(uint8_t)16
#define MAX_EXCURSION               									(uint16_t)1900
#define DEAD_ZONE                   									(uint8_t)50

/* Multiplexer A0 */
#define MUX_A0_GPIO_J     														GPIOC
#define MUX_A0_PIN_J      														GPIO_Pin_9

/* Multiplexer A1 */
#define MUX_A1_GPIO_J     														GPIOC
#define MUX_A1_PIN_J      														GPIO_Pin_10

/* DN input pin */
#define DN_GPIO_J         														GPIOC
#define DN_PIN_J          														GPIO_Pin_11

/* UP input pin */
#define UP_GPIO_J         														GPIOC
#define UP_PIN_J          														GPIO_Pin_12

#define JOYSTICK_MANDREL            									(uint8_t)0
#define JOYSTICK_STIFFBACK          									(uint8_t)1
#define JOYSTICK_PIPETRAVEL         									(uint8_t)2
#define JOYSTICK_PINUP              									(uint8_t)3

#define NUMBER_OF_JOYSTICKS         									(uint8_t)4
#define OFF_CENTER(n)    															(brgSwState[(n) * 2] || brgSwState[(n) * 2 + 1])
/*********************ADC**********************/
#define ADC_BITS             													(uint8_t)12

#define SERVICE_INTERVAL_TICKS_A      								(uint8_t)5
#define AVG_HISTORY_LENGTH          									(uint8_t)16

/* ADC_A0 (multiplexer select) is connected to PB5 */
#define MUX_A0_GPIO_A     														GPIOB
#define MUX_A0_PIN_A      														GPIO_Pin_5

/* SPI1 CS is PC13 */
#define CS_GPIO_A         														GPIOC
#define CS_PIN_A          														GPIO_Pin_13

#define JOYSTICK_UP_1																	(uint8_t)1
#define JOYSTICK_DOWN_1																(uint8_t)2
#define JOYSTICK_LEFT_2																(uint8_t)3
#define JOYSTICK_RIGHT_2															(uint8_t)4
#define JOYSTICK_LEFT_3																(uint8_t)5
#define JOYSTICK_RIGHT_3															(uint8_t)6
#define JOYSTICK_UP_4																	(uint8_t)7
#define JOYSTICK_DOWN_4																(uint8_t)8
#define JOYSTICK_CENTER_4															(uint8_t)9
#define JOYSTICK_CENTER_1															(uint8_t)10
/* ----------------------------------------------------------------------------
*                           CONSTANTS
* ----------------------------------------------------------------------------
*/
/* ----------------------------------------------------------------------------
*                           GLOBAL VARIABLES
* ----------------------------------------------------------------------------
*/
typedef struct JoystickFlags
{	
	uint8_t bMandrel_Left_Pos:1;   //MANDREL LEFT POSITION	
	uint8_t bMandrel_Right_Pos:1;   //MANDREL RIGHT POSITION	
	uint8_t bMandrel_Center_Pos:1;	//MANDREL CENTER POSITION
	uint8_t bStiff_Up_Pos:1;        //STIFF UP POSITION
	uint8_t bStiff_Down_Pos:1;			//STIFF DOWN POSITION
	uint8_t bStiff_Center_Pos:1;		//STIFF CENTER POSITION
	uint8_t bPipe_Left_Pos:1;   //PIPE TRAVEL LEFT POSITION	
	uint8_t bPipe_Right_Pos:1;   //PIPE TRAVEL RIGHT POSITION	
	uint8_t bPipe_Center_Pos:1;	//PIPE TRAVEL CENTER POSITION	
	uint8_t bPin_Up_Pos:1;			//PIN UP POSITION
	uint8_t bPin_Down_Pos:1;			//PIN DOWN POSITION
	uint8_t bPin_Center_Pos:1;		//PIN CENTER POSITION
}JOYS_TypeDef;
/* ----------------------------------------------------------------------------
*                           EXTERNAL VARIABLES
* ----------------------------------------------------------------------------
*/

extern JOYS_TypeDef stJoy_Flag;
extern  bool bInit_Complete_Jo ;
extern  uint32_t uliTick_Counter_Jo;
extern  uint32_t uliLast_Service_Tick_Jo;
extern  bool bService_Flag_Jo;
extern  uint32_t rguliJs_Current[NUMBER_OF_JOYSTICKS];
extern  uint32_t rguliJs_Reported[NUMBER_OF_JOYSTICKS];
extern  uint32_t rguliPrev_Js_Reported[NUMBER_OF_JOYSTICKS];
extern  uint32_t rguliSamples_Since_Last_Offcenter[NUMBER_OF_JOYSTICKS];
extern  uint32_t rguliCal_Center[NUMBER_OF_JOYSTICKS];
extern  uint32_t rguliCal_Accum[NUMBER_OF_JOYSTICKS];
extern  uint32_t rguliCal_Count[NUMBER_OF_JOYSTICKS];
extern  bool bInit_Complete_Ad ;
extern  uint32_t uliTick_Counter_Ad;
extern  uint32_t uliLast_Service_Tick_Ad;
extern  bool bService_Flag_Ad;
extern  uint16_t rguiAvg_Buffer[4][AVG_HISTORY_LENGTH];
extern  uint32_t uliAvg_Buf_Index;
extern uint8_t ucCurrentJoystick ;
/* ----------------------------------------------------------------------------
*                           EXTERNAL FUNCTIONS
* ----------------------------------------------------------------------------
*/
extern void fnJS_Initialize(void);
extern void fnJS_Tick(void);
extern void fnJS_Foreground_Service(void);
extern void fnJS_Get_Report(uint16_t *);
extern  void fnJS_Read_Switch_States(bool *);
extern void fnADC_Initialize(void);
extern void fnADC_Read(uint16_t *);
extern void fnADC_Tick(void);
extern void fnADC_Foreground_Service(void);
extern void fnADC_Get_Avg_Readings(uint16_t *, uint32_t);
/* ----------------------------------------------------------------------------
*                           FUNCTIONS DECLARATION
* ----------------------------------------------------------------------------
*/













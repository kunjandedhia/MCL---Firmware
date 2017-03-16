//
/******************************************************************************
**@file    SW_MACROS.h
* 
**@brief   This file contains 
******************************************************************************
*/    
/* ----------------------------------------------------------------------------
*                           Includes
* ----------------------------------------------------------------------------
*/
#include "stdint.h"
/* ----------------------------------------------------------------------------
*                           MACROS
* ----------------------------------------------------------------------------
*/
#define NUMBER_OF_SWITCHES          		(uint8_t)14
#define SERVICE_INTERVAL_TICKS_SW    		(uint8_t)50

#define SWITCH1													(uint8_t)1
#define SWITCH2													(uint8_t)2
#define SWITCH3													(uint8_t)3
#define SWITCH4													(uint8_t)4
#define SWITCH5													(uint8_t)5
#define SWITCH6													(uint8_t)6
#define SWITCH7													(uint8_t)7
#define SWITCH8													(uint8_t)8
#define SWITCH9													(uint8_t)9
#define SWITCH10												(uint8_t)10
#define SWITCH11												(uint8_t)11			/*JS UP*/
#define SWITCH12												(uint8_t)12			/*JS DOWN*/
#define SWITCH13												(uint8_t)13			/*JS LEFT*/
#define SWITCH14												(uint8_t)14			/*JS RIGHT*/

#define SPECIAL_PATTERN_RESET       		0x1860    			/*binary 0001 1000 0110 0000 = 
																													left 2 + left 3 + right 2 + right 3*/
/* ----------------------------------------------------------------------------
*                           CONSTANTS
* ----------------------------------------------------------------------------
*/
/* ----------------------------------------------------------------------------
*                           GLOBAL VARIABLES
* ----------------------------------------------------------------------------
*/

// Switches are connected to PA0, PA1, PA2, PA3, PA4, PG15, PG14, PG13, PG11, PF11, PG8, PG7, PG6, PC8
static struct
{
	GPIO_TypeDef* GPIOx;
	uint16_t GPIO_Pin;
}
switch_map[NUMBER_OF_SWITCHES] =
{
	//               must be in this order
	//                          |
	//                          v
	{GPIOA, GPIO_Pin_0 },    // left 1 (top)
	{GPIOA, GPIO_Pin_1 },    // left 2
	{GPIOA, GPIO_Pin_2 },    // left 3
	{GPIOG, GPIO_Pin_14},    // left 4 (bottom outside)
	{GPIOG, GPIO_Pin_13},    // left 5 (bottom inside)
	{GPIOF, GPIO_Pin_11},    // right 5 (bottom inside)
	{GPIOG, GPIO_Pin_11},    // right 4 (bottom outside)
	{GPIOG, GPIO_Pin_15},    // right 3
	{GPIOA, GPIO_Pin_4 },    // right 2
	{GPIOA, GPIO_Pin_3 },    // right 1 (top)
	{GPIOG, GPIO_Pin_7 },    // 4-way top
	{GPIOG, GPIO_Pin_8 },    // 4-way bottom
	{GPIOC, GPIO_Pin_8 },    // 4-way left
	{GPIOG, GPIO_Pin_6 },    // 4-way right
};

/* ----------------------------------------------------------------------------
*                           EXTERNAL VARIABLES
* ----------------------------------------------------------------------------
*/
extern bool sbInit_Complete_Sw ;
extern uint32_t suliTick_Counter_Sw;
extern uint32_t suliLast_Service_Tick_Sw;
extern bool sbService_Flag_Sw;
extern uint16_t suiCurrent_Switch_State;
extern uint32_t suliCurrent_Tick ;
extern uint8_t ucCurrentSwitch;
/* ----------------------------------------------------------------------------
*                           EXTERNAL FUNCTIONS
* ----------------------------------------------------------------------------
*/
extern void fnTIM1_Configuration(void);
extern void fnTIM2_Configuration(void);
extern void Timer_1_Update_IRQ(void);
extern uint32_t fnTimer_GetTick(void);
extern void fnSw_Initialize(void);
extern uint16_t fnSw_Read_All(void);
extern void fnSw_Tick(void);
extern void fnSw_Foreground_Service(void);
/* ----------------------------------------------------------------------------
*                           FUNCTIONS DECLARATION
* ----------------------------------------------------------------------------
*/


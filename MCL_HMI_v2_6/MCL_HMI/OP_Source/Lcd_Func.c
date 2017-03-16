/******************************************************************************
* @file    lcd_func.c
* 
* @brief   This file contain all the function related to LCD
******************************************************************************
*/    
/* ----------------------------------------------------------------------------
*                           Includes
* ----------------------------------------------------------------------------
*/
//#include "Config.h"
#include "System_Header.h"
#include "Logos.h"
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
	uint32_t uliCurrentOrientation = 0;
	uint32_t uliDisplayHeight = 0;
	uint32_t uliDisplayWidth = 0;

	uint16_t uiCurrentScreen = 0;
	uint8_t	ucCurrentScreenNumber = 0;

	uint16_t uirgStoreCurrentScreen[100];
	uint8_t ucrgStoreCurrentScreenPattern[100];

	
	FONT *font_list[] =
	{
		//&font_franklin16,
		&font_franklin20,
		//&font_franklin22,
		&font_franklin24,
		//&font_franklin28,
		//&font_franklin36,
		//&font_times13,
	//	&font_times16,
	//	&font_times20,
	//	&font_times24,
	//	&font_times28,
	};

	FONT *current_font = NULL;
	uint32_t uliFont_Scaling_Factor;

	uint16_t uiCursorXPos = 0;/*149;*/
	uint16_t uiCursorYPos = 0;/*216;*/

	uint16_t uiCursorXPosKP = 149;
	uint16_t uiCursorYPosKP = 96;

/*****************************************************************************
 **@Function		 	:  fnDly100us
 **@Descriptions	:	 this function generate delay in multiple of 100 microsecond
 **@parameters		:  pArg: void pointer 
 **@return				:  none
*****************************************************************************/
void fnDly100us(void *pArg)
{
	volatile uint32_t vuliTime;
	uint32_t uliDelay = (uint32_t)pArg;

	while(uliDelay--)
	{
		for ( vuliTime = LOOP_DLY_100US ; vuliTime ; vuliTime-- );
	}
}
/*****************************************************************************
 **@Function		 	:  fnDelayMsec
 **@Descriptions	:	 this function generate delay in multiple of mili seconds
 **@parameters		:  uliMsecs: mili second count 
 **@return				:  none
*****************************************************************************/
void fnDelayMsec(uint32_t uliMsecs)
{
	volatile uint32_t vuliTime;

	for ( vuliTime = 0 ; vuliTime < uliMsecs ; vuliTime++ )
		fnDly100us((void *)10);
}
/*****************************************************************************
**@Function		 	:  fnGr_Initialize
**@Descriptions	:	 this use to initialize the display orientation
**@parameters		:  none
**@return				:  none
*****************************************************************************/
void fnGr_Initialize(void)
{
	uliFont_Scaling_Factor = 1;
	fnDisplay_Set_Orientation(DEFAULT_ORIENTATION);
	/*Gr_TestScreen();*/
}
/*****************************************************************************
**@Function		 	:  fnGr_FillRectangle
**@Descriptions	:	 this function is uset to fill color in the given rectangle
**@parameters		:  uliXAxis: X-axis
									 uliYAxis: Y-axis
									 uliPxAxis: Rectangle length in x direction 
									 uliPyAxis: Rectangle breadth in y direction
									 uliColor:	color value
**@return				:  none
*****************************************************************************/
void fnGr_FillRectangle(uint32_t uliXAxis, uint32_t uliYAxis, uint32_t uliPxAxis,
												uint32_t uliPyAxis, uint32_t uliColor)
{
	uint16_t uiClr;
	uint32_t uliXTemp, uliYTemp;

	uiClr = REPACK_RGB(uliColor);
	for ( uliYTemp = uliYAxis ; uliYTemp < (uliYAxis + uliPyAxis) ; uliYTemp++ )
	{
		fnDisplay_Set_RAM_Address(uliXAxis, uliYTemp);
		for ( uliXTemp = 0 ; uliXTemp < uliPxAxis ; uliXTemp++ )
			fnDisplay_Data_Write(uiClr);
	}
}
/*****************************************************************************
**@Function		 	:  fnGrVertical_Line
**@Descriptions	:	 This function is used to draw verticle line
**@parameters		:  uliXAxis: X-axis
									 uliYAxis: Y-axis
									 uliPxAxis: Rectangle length in x direction 
									 uliColor:	color value
**@return				:  none
*****************************************************************************/
void fnGrVertical_Line(uint32_t uliXAxis, uint32_t uliYAxis, uint32_t uliPxAxis,
												uint32_t uliColor)
{
	uint16_t uiClr;
	uint32_t uliXTemp;

	uiClr = REPACK_RGB(uliColor);
	fnDisplay_Set_RAM_Address(uliXAxis, uliYAxis);
	for ( uliXTemp = 0 ; uliXTemp < uliPxAxis ; uliXTemp++ )
		fnDisplay_Data_Write(uiClr);
}
/*****************************************************************************
**@Function		 	:  fnGrHorizontal_Line
**@Descriptions	:	 This function is used to draw verticle line
**@parameters		:  uliXAxis: X-axis
									 uliYAxis: Y-axis
									 uliPxAxis: Rectangle length in y direction 
									 uliColor:	color value
**@return				:  none
*****************************************************************************/
void fnGrHorizontal_Line(uint32_t uliXAxis, uint32_t uliYAxis, uint32_t uliPyAxis,
												 uint32_t uliColor)
{
	uint16_t uiClr;
	uint32_t uliYTemp;
	uint32_t uliOrientation;

	uiClr = REPACK_RGB(uliColor);
	fnDisplay_Set_RAM_Address(uliXAxis, uliYAxis);
	fnDisplay_Get_Geometry(NULL, NULL, &uliOrientation);
	fnDisplay_Set_Orientation((uliOrientation + 3) % 4);
	for ( uliYTemp = 0 ; uliYTemp < uliPyAxis ; uliYTemp++ )
		fnDisplay_Data_Write(uiClr);
	fnDisplay_Set_Orientation(uliOrientation);
}
/*****************************************************************************
**@Function		 	:  fnGr_Rectangle
**@Descriptions	:	 this function is uset to Draw the rectangle
**@parameters		:  uliXAxis: X-axis
									 uliYAxis: Y-axis
									 uliPxAxis: Rectangle length in x direction 
									 uliPyAxis: Rectangle breadth in y direction
									 uliColor:	color value
**@return				:  none
*****************************************************************************/
void fnGr_Rectangle(uint32_t uliXAxis, uint32_t uliYAxis, uint32_t uliPxAxis,
										uint32_t uliPyAxis, uint32_t uliColor)
{
	fnGrVertical_Line(uliXAxis, uliYAxis, uliPxAxis, uliColor);
	fnGrVertical_Line(uliXAxis, uliYAxis + uliPyAxis - 1, uliPxAxis, uliColor);
	fnGrHorizontal_Line(uliXAxis, uliYAxis, uliPyAxis, uliColor);
	fnGrHorizontal_Line(uliXAxis + uliPxAxis - 1, uliYAxis, uliPyAxis, uliColor);
}
/*****************************************************************************
**@Function		 	:  fnGr_ThickRectangle
**@Descriptions	:	 this function is uset to Draw thick rectangle
**@parameters		:  uliXAxis: X-axis
									 uliYAxis: Y-axis
									 uliPxAxis: Rectangle length in x direction 
									 uliPyAxis: Rectangle breadth in y direction
									 uliColor:	color value
									 uliThickness: thickness of Rectangle line
**@return				:  none
*****************************************************************************/
void fnGr_ThickRectangle(uint32_t uliXAxis, uint32_t uliYAxis, uint32_t uliPxAxis,
													uint32_t uliPyAxis, uint32_t uliColor, uint32_t uliThickness)
{
	uint32_t uliThick, uliLoop;
	uliThick = uliThickness;
	
	if ( uliThick > (uliPxAxis / 2) )
		uliThick = uliPxAxis / 2;
	if ( uliThick > (uliPyAxis / 2) )
		uliThick = uliPyAxis / 2;
	for ( uliLoop = 0 ; uliLoop < uliThick ; uliLoop++ )
		fnGr_Rectangle(uliXAxis + uliLoop, uliYAxis + uliLoop, uliPxAxis - (2 * uliLoop),
									uliPyAxis - (2 * uliLoop), uliColor);
}
/*****************************************************************************
**@Function		 	:  fnGr_SelectFont
**@Descriptions	:	 this function is used to select font
**@parameters		:  pucName:	pointer to font name
									 uliScaling_Factor: size of the character
**@return				:  bool: true or false
*****************************************************************************/
bool fnGr_SelectFont(uint8_t *pucName, uint32_t uliScaling_Factor)
{
	uint32_t uliLoop;
	if ( NULL == pucName )
	{
		current_font = NULL;
		return TRUE;
	}
	if ( 0 == strlen((char *)pucName) )
	{
		current_font = NULL;
		return TRUE;
	}
	for ( uliLoop = 0 ; uliLoop < NUMBER_OF_FONTS ; uliLoop++ )
	{
		if ( 0 == strcmp((const char *)pucName, (const char *)font_list[uliLoop]->ucName) )
		{
			current_font = font_list[uliLoop];
			uliFont_Scaling_Factor = uliScaling_Factor;
			return TRUE;
		}
	}
	return FALSE;
}
/*****************************************************************************
**@Function		 	:  fnGr_DoesFontExist
**@Descriptions	:	 this function is used to check that the given font exist or not
**@parameters		:  pucName: pointer to unsigned char used for font 
**@return				:  bool: True or false
*****************************************************************************/
bool fnGr_DoesFontExist(uint8_t *pucName)
{
	uint32_t uliLoop;
	if ( NULL == pucName )
		return FALSE;
	if ( 0 == strlen((const char *)pucName) )
		return FALSE;
	for ( uliLoop = 0 ; uliLoop < NUMBER_OF_FONTS ; uliLoop++ )
	{
		if ( 0 == strcmp((const char *)pucName, (const char *)font_list[uliLoop]->ucName) )
			return TRUE;
	}
	return FALSE;
}
/*****************************************************************************
**@Function		 	:  fnGr_GetStringWidth
**@Descriptions	:	 use to get the width of the string
**@parameters		:  pucStr: pointer to string
**@return				:  32 bit width data
*****************************************************************************/
uint32_t fnGr_GetStringWidth(uint8_t *pucStr)
{
	uint32_t uliCc;
	uint32_t uliLen;
	uint32_t uliTotal;
	const FSYMBOL *fsym;
	
	if ( NULL == current_font )
		return 0;
	if ( NULL == pucStr )
		return 0;
	uliTotal = 0;
	uliLen = strlen((const char *)pucStr);
	for ( uliCc = 0 ; uliCc < uliLen ; uliCc++ )
	{
		fsym = &(current_font->symtable[pucStr[uliCc] - current_font->ucCode_Start_Offset]);
		uliTotal += fsym->symhdr.ucWidth;
	}
	return uliTotal * uliFont_Scaling_Factor;
}
/*****************************************************************************
**@Function		 	:  fnGr_GetFontHeight
**@Descriptions	:	 use to get the height of the string
**@parameters		:  none
**@return				:  32 bit height data
*****************************************************************************/
uint32_t fnGr_GetFontHeight(void)
{
	if ( NULL == current_font )
		return 0;
	return current_font->uliHeight * uliFont_Scaling_Factor;
}
/*****************************************************************************
**@Function		 	:  fnGr_PutString
**@Descriptions	:	 uliXAxis: X-axis
									 uliYAxis: Y-axis
									 pucStr: pointer to string
									 uliForeGrnd_color: foreground color value
									 uliBackGrnd_color: background color value
									 uliRelative_Orientation:	relative orientation
**@parameters		:  
**@return				:  
*****************************************************************************/
bool fnGr_PutString(uint32_t uliXAxis, uint32_t uliYAxis, uint8_t *pucStr,
										uint32_t uliForeGrnd_color, uint32_t uliBackGrnd_color,
										uint32_t uliRelative_Orientation, uint8_t ucStrType)
{
	uint32_t uliHght, uliWdth;
	uint32_t uliCc;
	uint32_t uliLen;
	const FSYMBOL *fsym;
	uint32_t uliRl;
	uint32_t uliPix;
	uint16_t uiFg_Clr, uiBg_Clr;
	uint16_t uiClr;
	const uint8_t *ucpBData;
	uint32_t uliWdthb;
	uint32_t uliMx;
	uint32_t uliMy;
	uint32_t uliOrientation;
	uint8_t ucYShifting = 0;
//	uint8_t rgucStrStore[10];
	
	
//	strcat(pucStrStore,pucStr);
	
	if(ucStrType == 1)
	{
		if(strlen((const char *)pucStr) == 6)
		{
			ucYShifting = 12 *0;
		}
		else if(strlen((const char *)pucStr) == 5)
		{
			ucYShifting = 12 *1;
		}
		else if(strlen((const char *)pucStr) == 4)
		{
			ucYShifting = 12 *2;
		}
		else if(strlen((const char *)pucStr) == 3)
		{
			ucYShifting = 12 *3;
		}
		else if(strlen((const char *)pucStr) == 2)
		{
			ucYShifting = 12 *4;
		}
		else if(strlen((const char *)pucStr) == 1)
		{
			ucYShifting = 12 *5;
		}
		
		uliYAxis = uliYAxis - ucYShifting;
	}
	if ( NULL == current_font )
		return FALSE;

	uliHght = current_font->uliHeight;
	uliLen = strlen((const char *)pucStr);
	uiFg_Clr = REPACK_RGB(uliForeGrnd_color);
	uiBg_Clr = REPACK_RGB(uliBackGrnd_color);
/*	wb = (current_font->max_width + 7) / 8;*/

	if ( uliRelative_Orientation != ORIENTATION_0 )
	{
		fnDisplay_Get_Geometry(NULL, NULL, &uliOrientation);
		fnDisplay_RAM_Address_Oriented_to_Physical(&uliXAxis, &uliYAxis);
		fnDisplay_Set_Orientation((uliOrientation + uliRelative_Orientation) % 4);
		fnDisplay_RAM_Address_Physical_to_Oriented(&uliXAxis, &uliYAxis);
	}

	for ( uliRl = 0 ; uliRl < uliHght ; uliRl++ )
	{
		for ( uliMy = 0 ; uliMy < uliFont_Scaling_Factor ; uliMy++ )
		{
			fnDisplay_Set_RAM_Address(uliXAxis, uliYAxis +
															(uliRl * uliFont_Scaling_Factor) + uliMy);
			for ( uliCc = 0 ; uliCc < uliLen ; uliCc++ )
			{
				fsym = &(current_font->symtable[pucStr[uliCc] - 
								current_font->ucCode_Start_Offset]);
				uliWdth = fsym->symhdr.ucWidth;
				uliWdthb = (uliWdth + 7) / 8;
				ucpBData = &(fsym->bitdata[uliRl * uliWdthb]);
				for ( uliPix = 0 ; uliPix < uliWdth ; uliPix++ )
				{
					if ( (*ucpBData) & (1 << (7 - (uliPix & 7))) )
						uiClr = uiFg_Clr;
					else
						uiClr = uiBg_Clr;
					for ( uliMx = 0 ; uliMx < uliFont_Scaling_Factor ; uliMx++ )
						fnDisplay_Data_Write(uiClr);
					if ( 7 == (uliPix & 7) )
						ucpBData++;
				}
			}
		}
	}

	if ( ORIENTATION_0 != uliRelative_Orientation )
		fnDisplay_Set_Orientation(uliOrientation);

	return TRUE;
}
/*****************************************************************************
**@Function		 	:  Dly1us
**@Descriptions	:	 this function is use to generate delay of 1 micro second
**@parameters		:  none 
**@return				:  none
*****************************************************************************/
void Dly1us(void)
{
	volatile uint32_t vuliTime;

	for ( vuliTime = LOOP_DLY_1US ; vuliTime ; vuliTime-- );
}
/*****************************************************************************
**@Function		 	:  fnDisplay_Set_Orientation
**@Descriptions	:	 this function is used to set the orientation of display
**@parameters		:  uliOrientation:	orientation of the display
**@return				:  none
*****************************************************************************/
void fnDisplay_Set_Orientation(uint32_t uliOrientation)
{
	fnDelayMsec(1);
	uliCurrentOrientation = uliOrientation;
	switch ( uliOrientation )
	{
	case ORIENTATION_0:
		fnDisplay_Reg_Write(0x003, ORIENTATION_0_R003);
		uliDisplayWidth = DISPLAY_WIDTH_0_180;
		uliDisplayHeight = DISPLAY_HEIGHT_0_180;
		break;
	case ORIENTATION_90:
		fnDisplay_Reg_Write(0x003, ORIENTATION_90_R003);
		uliDisplayWidth = DISPLAY_WIDTH_90_270;
		uliDisplayHeight = DISPLAY_HEIGHT_90_270;
		break;
	case ORIENTATION_180:
		fnDisplay_Reg_Write(0x003, ORIENTATION_180_R003);
		uliDisplayWidth = DISPLAY_WIDTH_0_180;
		uliDisplayHeight = DISPLAY_HEIGHT_0_180;
		break;
	case ORIENTATION_270:
		fnDisplay_Reg_Write(0x003, ORIENTATION_270_R003);
		uliDisplayWidth = DISPLAY_WIDTH_90_270;
		uliDisplayHeight = DISPLAY_HEIGHT_90_270;
		break;
	}
	fnDelayMsec(1); 
	fnDisplay_Index_Reg_Set(0x202);        /*Index register for RAM write*/
}
/*****************************************************************************
**@Function		 	:  fnDisplay_Get_Geometry
**@Descriptions	:	 this function is used to get the geometry of display
**@parameters		:  puliWidthPixels: pointer to pixel width
									 puliHeightPixels: pointer to pixel height
									 puliOrientation:	pointer to the orientation of the display
**@return				:  none
*****************************************************************************/
void fnDisplay_Get_Geometry(uint32_t *puliWidthPixels, uint32_t *puliHeightPixels,
														uint32_t *puliOrientation)
{
	if ( NULL != puliHeightPixels )
		*puliHeightPixels = uliDisplayHeight;
	if ( NULL != puliWidthPixels )
		*puliWidthPixels = uliDisplayWidth;
	if ( NULL != puliOrientation )
		*puliOrientation = uliCurrentOrientation;
}
/*****************************************************************************
**@Function		 	:  fnDisplay_Index_Reg_Set
**@Descriptions	:	 this function is used set the index of memory address
**@parameters		:  uiAddr: address of the memory
**@return				:  none
*****************************************************************************/
static void fnDisplay_Index_Reg_Set(uint16_t uiAddr)
{
	*( uint16_t*)INDEX_REG_MEMADDR = uiAddr;
}
/*****************************************************************************
**@Function		 	:  fnDisplay_Reg_Write
**@Descriptions	:	 this function is used to write the data to memory register
**@parameters		:  uiAddr: Address of register
									 uiData: data to be written
**@return				:  
*****************************************************************************/
void fnDisplay_Reg_Write(uint16_t uiAddr, uint16_t uiData)
{
	*(uint16_t *)INDEX_REG_MEMADDR = uiAddr;
	*(uint16_t *)DATA_REG_MEMADDR = uiData;
}
/*****************************************************************************
**@Function		 	:  fnDisplay_Data_Write
**@Descriptions	:	 this function is used to write the data on the display
**@parameters		:  uiData: data to display
**@return				:  none
*****************************************************************************/
void fnDisplay_Data_Write(uint16_t uiData)
{
	*(uint16_t *)DATA_REG_MEMADDR = uiData;
}
/*****************************************************************************
**@Function		 	:  fnDisplay_Set_All_Registers
**@Descriptions	:	 this function is used to set the display registers
**@parameters		:  none
**@return				:  none
*****************************************************************************/
void fnDisplay_Set_All_Registers(void) 
{
	fnDelayMsec(100);

	fnDisplay_Index_Reg_Set(0x0000); 
	fnDisplay_Index_Reg_Set(0x0000); 
	fnDisplay_Index_Reg_Set(0x0000); 
	fnDisplay_Index_Reg_Set(0x0000); 

	fnDisplay_Reg_Write(0x001, 0x0100);   /*Driver Output; SM=1, SS=0 */
	/*LCD Driving Wave Control; B/C=1, NW=0 ;N Line inversion */
	fnDisplay_Reg_Write(0x002, 0x0100);   

	fnDisplay_Reg_Write(0x008, 0x0204);   /*Display Control2 */
	fnDisplay_Reg_Write(0x00B, 0x0010);   /*Low Power Control2 */
	fnDisplay_Reg_Write(0x00F, 0x001B);   /*External Display Interface Control2*/ 
	fnDisplay_Reg_Write(0x010, 0x001F);   /*Panel Interface Control1 */
	fnDisplay_Reg_Write(0x011, 0x0000);   /*Panel Interface Control2 */
	fnDisplay_Reg_Write(0x012, 0x0000);   /*Panel Interface Control3 */
	fnDisplay_Reg_Write(0x020, 0x021E);   /*Panel Interface Control4 */
	fnDisplay_Reg_Write(0x021, 0x0000);   /*Panel Interface Control5 */
	fnDisplay_Reg_Write(0x022, 0x0500);   /*Panel Interface Control6 */
	fnDisplay_Reg_Write(0x210, 0x0000);   /*HSA */
	fnDisplay_Reg_Write(0x211, 0x00EF);   /*HEA 400 */
	fnDisplay_Reg_Write(0x212, 0x0000);   /*VSA */
	fnDisplay_Reg_Write(0x213, 0x018F);   /*VEA 240 */

	/* Gamma value setting */
	fnDisplay_Reg_Write(0x300, 0x0707); 
	fnDisplay_Reg_Write(0x301, 0x0607); 
	fnDisplay_Reg_Write(0x302, 0x0504); 
	fnDisplay_Reg_Write(0x303, 0x0303); 
	fnDisplay_Reg_Write(0x304, 0x0002); 
	fnDisplay_Reg_Write(0x305, 0x0207); 
	fnDisplay_Reg_Write(0x306, 0x0202); 
	fnDisplay_Reg_Write(0x307, 0x0707); 
	fnDisplay_Reg_Write(0x308, 0x0607); 
	fnDisplay_Reg_Write(0x309, 0x0504); 
	fnDisplay_Reg_Write(0x30A, 0x0303); 
	fnDisplay_Reg_Write(0x30B, 0x0002); 
	fnDisplay_Reg_Write(0x30C, 0x0207); 
	fnDisplay_Reg_Write(0x30D, 0x1F1F); 

	/* Base Image Setting */
	fnDisplay_Reg_Write(0x400, 0x3100);   /*Base Image Number of Line*/ 
	fnDisplay_Reg_Write(0x401, 0x0001);   /*Base Image Display Control */
	fnDisplay_Reg_Write(0x404, 0x0000);   /*Base Image Vertical Scroll Control */

	/* POWER ON */
	fnDisplay_Reg_Write(0x007, 0x0001);   /*Display Control1 */
	fnDisplay_Reg_Write(0x110, 0x0001);   /*Power Control6       PSE=1 */
	fnDisplay_Reg_Write(0x012, 0x0060);   /*Panel Interface Control3 */
	fnDisplay_Reg_Write(0x100, 0x17B0);   /*Power Control1       Booster OFF */
	fnDisplay_Reg_Write(0x101, 0x0014);   /*Power Control2 */
	fnDisplay_Reg_Write(0x102, 0x01A8);   /*Power Control3       PSON=0 */

	fnDelayMsec(1); 

	fnDisplay_Reg_Write(0x103, 0x3000);   /*Power Control4 */
	fnDisplay_Reg_Write(0x281, 0x0008);   /*Vcom High Voltage*/ 
	fnDisplay_Reg_Write(0x102, 0x01B8);   /*Power Control3       PSON=1 */

	fnDelayMsec(150); 

	/* DISPLAY_ON */
	fnDisplay_Reg_Write(0x007, 0x0021);   /*Display Control1 */

	fnDelayMsec(1); 

	fnDisplay_Reg_Write(0x100, 0x1690);   
	fnDisplay_Reg_Write(0x101, 0x0001);   /*Power Control2 */
	fnDisplay_Reg_Write(0x007, 0x0061);   /*Display Control1 */

	fnDelayMsec(40); 

	fnDisplay_Reg_Write(0x007, 0x0173);   /*Display Control1 */

	/*Internal Clock Display Mode*/ 
	fnDelayMsec(1); 
	fnDisplay_Reg_Write(0x003, 0x5030);   /*Entry Mode */
	fnDelayMsec(1); 
	fnDisplay_Reg_Write(0x200, 0x0000);   /*RAM Address Set */
	fnDisplay_Reg_Write(0x201, 0x0000);   /*RAM Address Set */
	 /*External Display Interface Control1   was 0x0110*/
	fnDisplay_Reg_Write(0x00C, 0x0000);  

	fnDisplay_Index_Reg_Set(0x202);           /*Index Register Set*/ 

	fnDelayMsec(20);
}
/*****************************************************************************
**@Function		 	:  fnDisplay_Initialize
**@Descriptions	:	 to initialize the display
**@parameters		:  none
**@return				:  none
*****************************************************************************/
void fnDisplay_Initialize(void)
{
	GPIO_InitTypeDef stGPIO_InitStructure_GR;

	uliDisplayHeight = DISPLAY_HEIGHT_0_180;
	uliDisplayWidth = DISPLAY_WIDTH_0_180;
	
	/* set up backlight pin*/
	memset(&stGPIO_InitStructure_GR, 0, sizeof(stGPIO_InitStructure_GR));
 
	stGPIO_InitStructure_GR.GPIO_Speed = GPIO_Speed_50MHz;
	stGPIO_InitStructure_GR.GPIO_Mode = GPIO_Mode_Out_PP;
	stGPIO_InitStructure_GR.GPIO_Pin = GPIO_Pin_8;
	GPIO_Init(GPIOA, &stGPIO_InitStructure_GR);
	GPIO_WriteBit(GPIOA, GPIO_Pin_8, Bit_SET);    /* backlight on*/

	fnDisplay_Set_All_Registers();	

}
/*****************************************************************************
**@Function		 	:  fnDisplay_Set_RAM_Address
**@Descriptions	:	 this function is used to set the RAM address
**@parameters		:  uliXAxis: X-Position
									 uliYAxis: Y-Position
**@return				:  none
*****************************************************************************/
void fnDisplay_Set_RAM_Address(uint32_t uliXAxis, uint32_t uliYAxis)
{
	switch ( uliCurrentOrientation )
	{
		case ORIENTATION_0:
			fnDisplay_Reg_Write(0x200, (uint16_t)uliXAxis);   /*RAM address X*/
			fnDisplay_Reg_Write(0x201, (uint16_t)uliYAxis);   /*RAM address Y*/
		break;
		case ORIENTATION_90:
			fnDisplay_Reg_Write(0x200, (uint16_t)uliYAxis);   /*RAM address X*/
		  /*RAM address Y*/
			fnDisplay_Reg_Write(0x201, (uint16_t)(uliDisplayWidth - 1 - uliXAxis));
		break;
		case ORIENTATION_180:
			/*RAM address X*/
			fnDisplay_Reg_Write(0x200, (uint16_t)(uliDisplayWidth - 1 - uliXAxis)); 
			/*RAM address Y*/		
			fnDisplay_Reg_Write(0x201, (uint16_t)(uliDisplayHeight - 1 - uliYAxis));  
		break;
		case ORIENTATION_270:
			fnDisplay_Reg_Write(0x200, (uint16_t)(uliDisplayHeight - 1 - uliYAxis));   
			fnDisplay_Reg_Write(0x201, (uint16_t)uliXAxis);  
		break;
	}
	/*Index register for RAM write*/
	fnDisplay_Index_Reg_Set(0x202);      
}
/*****************************************************************************
**@Function		 	:  fnDisplay_RAM_Address_Oriented_to_Physical
**@Descriptions	:	 this function is used to Display RAM Address Oriented
									 to Physical memory
**@parameters		:  puliPxAxis: pointer to position of x axis
									 puliPyAxis: pointer to position of y axis
**@return				:  none
*****************************************************************************/
void fnDisplay_RAM_Address_Oriented_to_Physical(uint32_t *puliPxAxis, 
																								uint32_t *puliPyAxis)
{
	uint32_t uliXTemp, uliYTemp;

	uliXTemp = *puliPxAxis;
	uliYTemp = *puliPyAxis;
	switch ( uliCurrentOrientation )
	{
		case ORIENTATION_0:
			*puliPxAxis = uliXTemp;
			*puliPyAxis = uliYTemp;
		break;
		case ORIENTATION_90:
			*puliPxAxis = uliYTemp;
			*puliPyAxis = uliDisplayWidth - 1 - uliXTemp;
		break;
		case ORIENTATION_180:
			*puliPxAxis = uliDisplayWidth - 1 - uliXTemp;
			*puliPyAxis = uliDisplayHeight - 1 - uliYTemp;
		break;
		case ORIENTATION_270:
			*puliPxAxis = uliDisplayHeight - 1 - uliYTemp;
			*puliPyAxis = uliXTemp;
		break;
	}
}
/*****************************************************************************
**@Function		 	:  fnDisplay_RAM_Address_Physical_to_Oriented
**@Descriptions	:	 this function is used to fnDisplay RAM Address 
									 Physical to Orientation
**@parameters		:  puliPxAxis: pointer to position of x axis
									 puliPyAxis: pointer to position of y axis
**@return				:  none
*****************************************************************************/
void fnDisplay_RAM_Address_Physical_to_Oriented(uint32_t *puliPxAxis, uint32_t *puliPyAxis)
{
	uint32_t uliXTemp, uliYTemp;

	uliXTemp = *puliPxAxis;
	uliYTemp = *puliPyAxis;
	switch ( uliCurrentOrientation )
	{
		case ORIENTATION_0:
			*puliPxAxis = uliXTemp;
			*puliPyAxis = uliYTemp;
		break;
		case ORIENTATION_90:
			*puliPxAxis = uliDisplayWidth - 1 - uliYTemp;
			*puliPyAxis = uliXTemp;
		break;
		case ORIENTATION_180:
			*puliPxAxis = uliDisplayWidth - 1 - uliXTemp;
			*puliPyAxis = uliDisplayHeight - 1 - uliYTemp;
		break;
		case ORIENTATION_270:
			*puliPxAxis = uliYTemp;
			*puliPyAxis = uliDisplayHeight - 1 - uliXTemp;
		break;
	}
}
/*****************************************************************************
**@Function		 	:  fnDisplay_Clear_Screen
**@Descriptions	:	 this is function is used to clear the screen 
**@parameters		:  uliColor: color of the screen
**@return				:  none
*****************************************************************************/
void fnDisplay_Clear_Screen(uint32_t uliColor)
{
	uint32_t	 uliLoop;

	fnDisplay_Set_RAM_Address(0, 0);
	for ( uliLoop = 0 ; uliLoop < uliDisplayHeight * uliDisplayWidth ; uliLoop++ )
		fnDisplay_Data_Write(REPACK_RGB(uliColor));
}
/*****************************************************************************
**@Function		 	:  fnGR_GPIO_Configuration
**@Descriptions	:	 GPIO Configurations
**@parameters		:  none
**@return				:  none
*****************************************************************************/
void fnGR_GPIO_Configuration(void)
{
	GPIO_InitTypeDef stGPIO_InitStructure_GR;

	/* Configure all unused GPIO port pins in Analog Input mode (floating input
	trigger OFF), this will reduce the power consumption and increase the device
	immunity against EMI/EMC */
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | /*RCC_APB2Periph_GPIOB |*/
	                       RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD |
	                       RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOF |
	                       RCC_APB2Periph_GPIOG, ENABLE);
	stGPIO_InitStructure_GR.GPIO_Pin = GPIO_Pin_All;
	stGPIO_InitStructure_GR.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &stGPIO_InitStructure_GR);
	GPIO_Init(GPIOC, &stGPIO_InitStructure_GR);
	GPIO_Init(GPIOD, &stGPIO_InitStructure_GR);
	GPIO_Init(GPIOE, &stGPIO_InitStructure_GR);
	GPIO_Init(GPIOF, &stGPIO_InitStructure_GR);
	GPIO_Init(GPIOG, &stGPIO_InitStructure_GR);
}
/*****************************************************************************
**@Function		 	:  fnGR_Memory_Configuration
**@Descriptions	:	 Graphical memory configuration
**@parameters		:  none
**@return				:  none
*****************************************************************************/
void fnGR_Memory_Configuration(void)
{
	GPIO_InitTypeDef stGPIO_InitStructure_GR;
	FSMC_NORSRAMInitTypeDef stFSMC_InitStructure;
	FSMC_NORSRAMTimingInitTypeDef stFSMC_ReadWriteTimingStruct;
	FSMC_NORSRAMTimingInitTypeDef stFSMC_WriteTimingStruct;

	/* enable FSMC */
	
	RCC->AHBENR |= RCC_AHBENR_FSMCEN;
	// enable clocks for all relevant ports
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOF | RCC_APB2Periph_GPIOG, ENABLE);

	/* set up GPIO pins

	 SRAM_CS is on PG10/FSMC_NCE4_1/FSMC_NE3 and uses NBL0 and NBL1
	 FSMC_NE3 address range = 0x68000000 to 0x6BFFFFFF
	 alternate function pins needed: A0 thru A18, D0 thru D15, FSMC_NOE, FSMC_NWE, PG10, PE0, PE1

	 LCD_CS is on PG12/FSMC_NE4
	 FSMC_NE4 address range = 0x6C000000 to 0x6FFFFFFF
	 alternate function pins needed: A1, D0 thru D15, FSMC_NOE, FSMC_NWE, PG12

	 AF pin map:
	  D                                               E
	  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
	  x  x        x  x        x  x  x  x  x  x  x  x  x  x                 x  x  x  x  x  x  x  x  x

	  F                                               G																									
	  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15		
	  x  x  x  x  x  x                    x  x  x  x  x  x  x  x  x  x              x     x							*/

	stGPIO_InitStructure_GR.GPIO_Speed = GPIO_Speed_50MHz;
	stGPIO_InitStructure_GR.GPIO_Mode = GPIO_Mode_AF_PP;
	/* Port D */
	stGPIO_InitStructure_GR.GPIO_Pin =
			GPIO_Pin_0 |
			GPIO_Pin_1 |
			/*GPIO_Pin_2 |*/
			/*GPIO_Pin_3 |*/
			GPIO_Pin_4 |
			GPIO_Pin_5 |
			/*GPIO_Pin_6 |*/
			/*GPIO_Pin_7 |*/
			GPIO_Pin_8 |
			GPIO_Pin_9 |
			GPIO_Pin_10 |
			GPIO_Pin_11 |
			GPIO_Pin_12 |
			GPIO_Pin_13 |
			GPIO_Pin_14 |
			GPIO_Pin_15 |
			0;
	GPIO_Init(GPIOD, &stGPIO_InitStructure_GR);
	/* Port E */
	stGPIO_InitStructure_GR.GPIO_Pin =
			GPIO_Pin_0 |
			GPIO_Pin_1 |
			/*GPIO_Pin_2 |*/
			/*GPIO_Pin_3 |*/
			GPIO_Pin_4 |		/*Buzzer Init*/
			/*GPIO_Pin_5 |*/
			/*GPIO_Pin_6 |*/
			GPIO_Pin_7 |
			GPIO_Pin_8 |
			GPIO_Pin_9 |
			GPIO_Pin_10 |
			GPIO_Pin_11 |
			GPIO_Pin_12 |
			GPIO_Pin_13 |
			GPIO_Pin_14 |
			GPIO_Pin_15 |
			0;
	GPIO_Init(GPIOE, &stGPIO_InitStructure_GR);
	/*  Port F*/
	stGPIO_InitStructure_GR.GPIO_Pin =
			GPIO_Pin_0 |
			GPIO_Pin_1 |
			GPIO_Pin_2 |
			GPIO_Pin_3 |
			GPIO_Pin_4 |
			GPIO_Pin_5 |
			GPIO_Pin_6 |  
			GPIO_Pin_7 |   
			/*GPIO_Pin_8 |
			GPIO_Pin_9 |
			GPIO_Pin_10 |
			GPIO_Pin_11 |*/
			GPIO_Pin_12 |
			GPIO_Pin_13 |
			GPIO_Pin_14 |
			GPIO_Pin_15 |
			0;
	GPIO_Init(GPIOF, &stGPIO_InitStructure_GR); 
	/* Port G */
	stGPIO_InitStructure_GR.GPIO_Pin =
			GPIO_Pin_0 |
			GPIO_Pin_1 |
			GPIO_Pin_2 |
			GPIO_Pin_3 |
			GPIO_Pin_4 |
			GPIO_Pin_5 |
			/*GPIO_Pin_6 |
			GPIO_Pin_7 |
			GPIO_Pin_8 |
			GPIO_Pin_9 |*/
			/*GPIO_Pin_10 |*/
			/*GPIO_Pin_11 |*/
			GPIO_Pin_12 |
			/*GPIO_Pin_13 |
			GPIO_Pin_14 |
			GPIO_Pin_15 |*/
			0;
	GPIO_Init(GPIOG, &stGPIO_InitStructure_GR);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);
  stGPIO_InitStructure_GR.GPIO_Pin = GPIO_Pin_6;
  stGPIO_InitStructure_GR.GPIO_Speed = GPIO_Speed_50MHz;
  stGPIO_InitStructure_GR.GPIO_Mode =  GPIO_Mode_IN_FLOATING ;
	GPIO_Init(GPIOF, &stGPIO_InitStructure_GR);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);
  stGPIO_InitStructure_GR.GPIO_Pin = GPIO_Pin_7;
  stGPIO_InitStructure_GR.GPIO_Speed = GPIO_Speed_50MHz;
  stGPIO_InitStructure_GR.GPIO_Mode =  GPIO_Mode_IN_FLOATING ;
	GPIO_Init(GPIOF, &stGPIO_InitStructure_GR);
	
	/* configure FSMC for SRAM3 */
	FSMC_NORSRAMDeInit(FSMC_Bank1_NORSRAM3);
	stFSMC_InitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM3;
	stFSMC_InitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
	stFSMC_InitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
	stFSMC_InitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
	stFSMC_InitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
	stFSMC_InitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	stFSMC_InitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
	stFSMC_InitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
	stFSMC_InitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
	stFSMC_InitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
	stFSMC_InitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
	stFSMC_InitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
	stFSMC_InitStructure.FSMC_ReadWriteTimingStruct = &stFSMC_ReadWriteTimingStruct;
	stFSMC_InitStructure.FSMC_ReadWriteTimingStruct = &stFSMC_ReadWriteTimingStruct;
	stFSMC_ReadWriteTimingStruct.FSMC_AddressSetupTime = 0x1;
	stFSMC_ReadWriteTimingStruct.FSMC_AddressHoldTime = 0x1;
	stFSMC_ReadWriteTimingStruct.FSMC_DataSetupTime = 0x2; 
	stFSMC_ReadWriteTimingStruct.FSMC_BusTurnAroundDuration = 0x1;
	stFSMC_ReadWriteTimingStruct.FSMC_CLKDivision = 0x2;
	stFSMC_ReadWriteTimingStruct.FSMC_DataLatency = 0x1;
	stFSMC_ReadWriteTimingStruct.FSMC_AccessMode = FSMC_AccessMode_A;
	memcpy(&stFSMC_WriteTimingStruct, &stFSMC_ReadWriteTimingStruct, sizeof(stFSMC_WriteTimingStruct));
	FSMC_NORSRAMInit(&stFSMC_InitStructure);
	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM3, ENABLE);

	/* configure FSMC for SRAM4 */
	stFSMC_ReadWriteTimingStruct.FSMC_AddressSetupTime = 0x1;
	stFSMC_ReadWriteTimingStruct.FSMC_AddressHoldTime = 0x1;
	stFSMC_ReadWriteTimingStruct.FSMC_DataSetupTime = 0x2;  
	stFSMC_ReadWriteTimingStruct.FSMC_BusTurnAroundDuration = 0x1;
	stFSMC_ReadWriteTimingStruct.FSMC_CLKDivision = 0x2;
	stFSMC_ReadWriteTimingStruct.FSMC_DataLatency = 0x1;
	stFSMC_ReadWriteTimingStruct.FSMC_AccessMode = FSMC_AccessMode_A;
	FSMC_NORSRAMDeInit(FSMC_Bank1_NORSRAM4);
	stFSMC_InitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4;
	stFSMC_InitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
	stFSMC_InitStructure.FSMC_MemoryType = FSMC_MemoryType_PSRAM;
	stFSMC_InitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
	stFSMC_InitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
	stFSMC_InitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	stFSMC_InitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
	stFSMC_InitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
	stFSMC_InitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
	stFSMC_InitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
	stFSMC_InitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
	stFSMC_InitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
	stFSMC_InitStructure.FSMC_ReadWriteTimingStruct = &stFSMC_ReadWriteTimingStruct;
	stFSMC_InitStructure.FSMC_WriteTimingStruct = &stFSMC_ReadWriteTimingStruct;
	
	memcpy(&stFSMC_WriteTimingStruct, &stFSMC_ReadWriteTimingStruct, sizeof(stFSMC_WriteTimingStruct));
	FSMC_NORSRAMInit(&stFSMC_InitStructure);
	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);
	
	
	/*Led*/ 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  stGPIO_InitStructure_GR.GPIO_Pin = 	PIN_LED_ERR_RED | PIN_LED_ERR_GREEN | 
																			PIN_LED_COM_RED | PIN_LED_COM_BLUE;
  stGPIO_InitStructure_GR.GPIO_Speed = GPIO_Speed_50MHz;
  stGPIO_InitStructure_GR.GPIO_Mode =  GPIO_Mode_Out_PP ;
	GPIO_Init(GPIOC, &stGPIO_InitStructure_GR);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  stGPIO_InitStructure_GR.GPIO_Pin = PIN_LED_COM_GREEN | PIN_LED_ERR_BLUE;
  stGPIO_InitStructure_GR.GPIO_Speed = GPIO_Speed_50MHz;
  stGPIO_InitStructure_GR.GPIO_Mode =  GPIO_Mode_Out_PP ;
	GPIO_Init(GPIOB, &stGPIO_InitStructure_GR);
	
	/*Not Used LED*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  stGPIO_InitStructure_GR.GPIO_Pin = 	PIN_LED_LEFT_BOTTOM_RED | PIN_LED_LEFT_BOTTOM_BLUE;
  stGPIO_InitStructure_GR.GPIO_Speed = GPIO_Speed_50MHz;
  stGPIO_InitStructure_GR.GPIO_Mode =  GPIO_Mode_Out_PP ;
	GPIO_Init(GPIOC, &stGPIO_InitStructure_GR);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  stGPIO_InitStructure_GR.GPIO_Pin = 	PIN_LED_LEFT_BOTTOM_GREEN | 
																			PIN_LED_LEFT_TOP_RED |
																			PIN_LED_LEFT_TOP_BLUE |
																			PIN_LED_LEFT_TOP_GREEN;
  stGPIO_InitStructure_GR.GPIO_Speed = GPIO_Speed_50MHz;
  stGPIO_InitStructure_GR.GPIO_Mode =  GPIO_Mode_Out_PP ;
	GPIO_Init(GPIOF, &stGPIO_InitStructure_GR);
	
	GPIO_WriteBit(PORT_LED_ERR_RED, PIN_LED_LEFT_BOTTOM_RED | PIN_LED_LEFT_BOTTOM_BLUE, Bit_RESET);
	
	GPIO_WriteBit(PORT_LED_LEFT_TOP, 	PIN_LED_LEFT_BOTTOM_GREEN | 
																		PIN_LED_LEFT_TOP_RED |
																		PIN_LED_LEFT_TOP_BLUE |
																		PIN_LED_LEFT_TOP_GREEN, Bit_RESET);
	/*********/
}
/*****************************************************************************
**@Function		 	:  fnGR_SCREEN1
**@Descriptions	:	 Screen graphical representation
**@parameters		:  ucScreen_No : screen pattern number
**@return				:  none
*****************************************************************************/
void fnGR_SCREEN1(uint8_t ucScreen_No)
{
	fnGr_ThickRectangle(1, 1, 238, 398, RGB(0,0,0), 2);       /* outer rectangle */

	fnLCDbmp(40,3,38,38,ucrgHomeLogo);
	if(	(uiCurrentScreen == SCREEN_ALARMS_1) || (uiCurrentScreen == SCREEN_ALARMS_2) || 
			(uiCurrentScreen == SCREEN_ALARMS_3) || (uiCurrentScreen == SCREEN_ALARMS_4) )
	{
		fnLCDbmp(40,359,38,38,ucrgDelete);
	}
	else
	{
		fnLCDbmp(40,359,38,38,ucrgMenuButton);
	}
	
	fnGr_FillRectangle(3, 50, 36,299,BLUE_CLR);       				/* Fill Header Rectangle */
	
	fnGrHorizontal_Line( 39 ,44 , 311, RGB(0,0,0));					  /* Line Below Header Rectangle*/
	
	
	if(SCREEN_PATTERN_13 == ucScreen_No )
	{
		fnGr_ThickRectangle(100, 85, 40, 228, RGB(0,0,0), 1);
		fnGr_ThickRectangle(1, 1, 60, 80, RGB(0,0,0), 2);
		
		/*Add Checks for priority for Error nad Warning*/
		if(TRUE == StatusFlag.stStatusFlag.bErrorFlag )
		{
			fnGr_FillRectangle(101, 86, 38,226,RGB(255,21,15));
			fnGr_FillRectangle(2, 2, 58,78,RGB(255,21,15));
		}
		else	if(TRUE == StatusFlag.stStatusFlag.bWarningFlag )
		{
			fnGr_FillRectangle(101, 86, 38,226,RGB(45,45,255));
			fnGr_FillRectangle(2, 2, 58,78,RGB(45,45,255));
		}
	}

	fnGr_SelectFont("Franklin24", 1);	

	if(	(SCREEN_PATTERN_1 == ucScreen_No) || (SCREEN_PATTERN_2 == ucScreen_No) || (SCREEN_PATTERN_3 == ucScreen_No) ||
			(SCREEN_PATTERN_6 == ucScreen_No) || (SCREEN_PATTERN_7 == ucScreen_No) ||	(SCREEN_PATTERN_8 == ucScreen_No) ||
			(SCREEN_PATTERN_12 == ucScreen_No) )
	{
//		if(SCREEN_JOB_SETTING_1 != uiCurrentScreen )
//		{
//			fnGr_ThickRectangle(183, 172, 54, 55, RGB(0,0,0), 2);	
//			fnGr_ThickRectangle(183, 79, 54, 95, RGB(0,0,0), 2);
//			fnGr_ThickRectangle(183, 225, 54, 95, RGB(0,0,0), 2);		
//		}	
	}
	
	switch(ucScreen_No)
	{
		case SCREEN_PATTERN_1:
		{
			/*HOME*/
			if( SCREEN_HOME == uiCurrentScreen )
			{
				fnLCDbmp(110,5,50,50,ucrgWarning);
				fnLCDbmp(170,5,50,50,ucrgError);
				/*change4*/
				/*80% log memory full alarm*/
				fnLCDbmp(230,5,50,50,ucrgError);
			}
			
		}break;
		case SCREEN_PATTERN_2:
		{
			/*MENU BUTTONS*/
			
			fnGr_ThickRectangle(45, 8, 60, 184, DARK_GREY_CLR, 3);
			fnGr_ThickRectangle(45, 208, 60, 184, DARK_GREY_CLR, 3);
			fnGr_ThickRectangle(110, 8, 60, 184, DARK_GREY_CLR, 3);									
			fnGr_ThickRectangle(110, 208, 60, 184, DARK_GREY_CLR, 3);										
			fnGr_ThickRectangle(175, 8, 60, 184, DARK_GREY_CLR, 3);										
			fnGr_ThickRectangle(175, 208, 60, 184, DARK_GREY_CLR, 3);
			
			if(( uiCurrentScreen != SCREEN_MANAGE_PERIPHERAL_1 ) &&
				 ( uiCurrentScreen != SCREEN_MANAGE_PERIPHERAL_2 )&&
				 ( uiCurrentScreen != SCREEN_MANAGE_PERIPHERAL_3 ))
			{
				fnGr_FillRectangle(47, 10, 56,180,LIGHT_GREY_CLR);
				fnGr_FillRectangle(47, 210, 56,180,LIGHT_GREY_CLR);
				fnGr_FillRectangle(112, 10, 56,180,LIGHT_GREY_CLR);
				fnGr_FillRectangle(112, 210, 56,180,LIGHT_GREY_CLR);
				fnGr_FillRectangle(177, 10, 56,180,LIGHT_GREY_CLR);
				fnGr_FillRectangle(177, 210, 56,180,LIGHT_GREY_CLR);
			}
		}break;
		case SCREEN_PATTERN_3:
		{
//			fnLCDbmp(68,35,30,26,ucrgArrow);
//			fnLCDbmp(101,35,30,26,ucrgArrow);
//			fnLCDbmp(134,35,30,26,ucrgArrow);
//			fnLCDbmp(167,35,30,26,ucrgArrow);
//			fnLCDbmp(200,35,30,26,ucrgArrow);
//			fnLCDbmp(233,35,30,26,ucrgArrow);
		}break;
		case SCREEN_PATTERN_4:
		{
			fnGr_ThickRectangle(106, 3, 33, 237, LIGHT_GREY_CLR, 1);
			fnGr_ThickRectangle(106, 240, 33, 157, LIGHT_GREY_CLR, 1); 
			
			fnGr_ThickRectangle(70, 10, 33, 100, LIGHT_GREY_CLR, 1);		/*Manual Button*/
			fnGr_FillRectangle(71, 11, 31,98,DARK_GREY_CLR);
		}break;
		case SCREEN_PATTERN_5:
		{
			fnGrVertical_Line( 40 ,222 , 197, LIGHT_GREY_CLR);
			fnGrVertical_Line( 40 ,160 , 197, LIGHT_GREY_CLR);
			fnGrVertical_Line( 40 ,72 , 197, LIGHT_GREY_CLR);
			
			fnLCDbmp(100,5,24,24,ucrgCheck);
			fnLCDbmp(165,5,24,24,ucrgUncheck);
		}break;
		case SCREEN_PATTERN_6:
		{
			fnGr_FillRectangle(53, 385, 5,5,BLACK_CLR);
			fnGr_FillRectangle(86, 385, 5,5,BLACK_CLR);
			fnGr_FillRectangle(119, 385, 5,5,BLACK_CLR);
			fnGr_FillRectangle(152, 385, 5,5,BLACK_CLR);
			fnGr_FillRectangle(188, 385, 5,5,BLACK_CLR);
			fnGr_FillRectangle(221, 385, 5,5,BLACK_CLR);

		}break;
		case SCREEN_PATTERN_7:
		{
			
			fnGr_ThickRectangle(125, 8, 45, 184, DARK_GREY_CLR, 3);
			fnGr_FillRectangle(127, 10, 41,180,LIGHT_GREY_CLR);
			
			fnGr_ThickRectangle(190, 8, 45, 184, DARK_GREY_CLR, 3);
			fnGr_FillRectangle(192, 10, 41,180,LIGHT_GREY_CLR);
		}break;
		case SCREEN_PATTERN_8: 
		{
			fnGrHorizontal_Line( 63 ,80 , 238, RGB(0,0,0));				
			fnGrHorizontal_Line( 87 ,80 , 238, RGB(0,0,0));				
			fnGrHorizontal_Line( 111 ,80 , 238, RGB(0,0,0));
			
			if(FALSE == StatusFlag.stStatusFlag.bOPMMSetOnPipe ) 	
			{
					fnGrVertical_Line( 87 ,199 , 25, RGB(0,0,0));
			}
			else
			{
				if((uiCurrentScreen == SCREEN_PRIME) || (uiCurrentScreen == SCREEN_FLUSH) || 
						(uiCurrentScreen == SCREEN_SPRAY))
				{
					fnGrVertical_Line( 63 ,170 , 48, RGB(0,0,0));
				}
				else
				{
					fnGrHorizontal_Line( 135 ,80 , 238, RGB(0,0,0));
					fnGrHorizontal_Line( 159 ,80 , 238, RGB(0,0,0));
				
					fnGrVertical_Line( 27 ,170 , 156, RGB(0,0,0));		
				}
			}
		}break;
		case SCREEN_PATTERN_9: 
		{
			fnGrHorizontal_Line( 84 ,80 , 238, RGB(0,0,0));
			fnGrHorizontal_Line( 119 ,80 , 238, RGB(0,0,0));
			fnGrHorizontal_Line( 154 ,80 , 238, RGB(0,0,0));	
			fnGrVertical_Line( 119 ,199 , 35, RGB(0,0,0));
			
			/*OPMM Set on Pipe*/
			fnGr_PutString(98, 258, OPMM_SET, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			
			/*YES / NO option*/
			fnGr_PutString(130, 268 , YES, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			fnGr_PutString(130, 158 , NO, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
		}break;
		case SCREEN_PATTERN_11:			/*keypad screen*/
		{
			fnGr_ThickRectangle(70, 2, 50, 398, RGB(0,0,0), 2);
			
			fnGr_ThickRectangle(145, 2, 93, 396, RGB(0,0,0), 3); 
			
			fnGrHorizontal_Line( 146 ,2 , 396, RGB(0,0,0));				
			fnGrHorizontal_Line( 176 ,2 , 396, RGB(0,0,0));
			fnGrHorizontal_Line( 206 ,2 , 396, RGB(0,0,0));
			
			fnGrVertical_Line( 146 ,34 , 90, RGB(0,0,0));
			fnGrVertical_Line( 146 ,64 , 90, RGB(0,0,0));
			fnGrVertical_Line( 146 ,94 , 90, RGB(0,0,0));
			fnGrVertical_Line( 146 ,124 , 90, RGB(0,0,0));
			fnGrVertical_Line( 146 ,154 , 90, RGB(0,0,0));
			fnGrVertical_Line( 146 ,184 , 90, RGB(0,0,0));
			fnGrVertical_Line( 146 ,214 , 90, RGB(0,0,0));
			fnGrVertical_Line( 146 ,244 , 90, RGB(0,0,0));
			fnGrVertical_Line( 146 ,274 , 90, RGB(0,0,0));
			fnGrVertical_Line( 146 ,304 , 90, RGB(0,0,0));
			fnGrVertical_Line( 146 ,334 , 90, RGB(0,0,0));
			fnGrVertical_Line( 146 ,364 , 90, RGB(0,0,0));
		}break;
		case SCREEN_PATTERN_12: 
		{
			fnGrVertical_Line( 27 ,158 , 52, RGB(0,0,0));
				
			fnGrHorizontal_Line( 53 ,80 , 238, RGB(0,0,0));				
			fnGrHorizontal_Line( 79 ,80 , 238, RGB(0,0,0));				
			fnGrHorizontal_Line( 105 ,80 , 238, RGB(0,0,0));			
			fnGrHorizontal_Line( 131 ,80 , 238, RGB(0,0,0));			
			fnGrHorizontal_Line( 157 ,80 , 238, RGB(0,0,0));				
			
			fnGrVertical_Line( 105 ,199 , 26, RGB(0,0,0));
			fnGrVertical_Line( 157 ,199 , 26, RGB(0,0,0));
		}break;
		default:
			break;
	}
}
/*****************************************************************************
**@Function		 	:  NV_Write
**@Descriptions	:	 NVSRAM write function
**@parameters		:  uliAddress : address locaion
									 pData : data to be write
									 uliCount : number of data bytes 
**@return				:  status of write cycle
*****************************************************************************/
bool NV_Write(uint32_t uliAddress, void *pData, uint32_t uliCount)
{
	if ( (uliAddress > SRAM_SIZE_BYTES) || ((uliAddress + uliCount) > SRAM_SIZE_BYTES) )
		return FALSE;
	if ( uliCount == 0 )
		return TRUE;
	
	GPIO_WriteBit(GPIOG, GPIO_Pin_12, Bit_SET);	/*CE of LCD*/	
	GPIO_WriteBit(GPIOD, GPIO_Pin_5, Bit_RESET); /* WE */
	GPIO_WriteBit(GPIOD, GPIO_Pin_4, Bit_SET); /* OE */
  GPIO_WriteBit(GPIOG, GPIO_Pin_10, Bit_RESET); /* CE */
	GPIO_WriteBit(GPIOE, GPIO_Pin_0, Bit_RESET); /* BLE */
	GPIO_WriteBit(GPIOE, GPIO_Pin_1, Bit_RESET); /* BHE */
			
	memcpy((uint8_t *)(SRAM_BASE_ADDR + uliAddress), pData, uliCount);
			
	GPIO_WriteBit(GPIOD, GPIO_Pin_5, Bit_SET); /* WE */
	GPIO_WriteBit(GPIOD, GPIO_Pin_4, Bit_SET); /* OE */
	GPIO_WriteBit(GPIOG, GPIO_Pin_10, Bit_SET); /* CE */
	GPIO_WriteBit(GPIOE, GPIO_Pin_0, Bit_SET); /* BLE */
	GPIO_WriteBit(GPIOE, GPIO_Pin_1, Bit_SET); /* BHE*/
	GPIO_WriteBit(GPIOG, GPIO_Pin_12, Bit_RESET);		

	return TRUE;
}
/*****************************************************************************
**@Function		 	:  NV_Read
**@Descriptions	:	 NVSRAM read function
**@parameters		:  uliAddress : address locaion
									 pData : data to read
									 uliCount : number of data bytes 
**@return				:  status of read cycle
*****************************************************************************/
bool NV_Read(uint32_t uliAddress, void *pData, uint32_t uliCount)
{
	if ( (uliAddress > SRAM_SIZE_BYTES) || ((uliAddress + uliCount) > SRAM_SIZE_BYTES) )
		return FALSE;
	if ( uliCount == 0 )
		return TRUE;
				
	GPIO_WriteBit(GPIOD, GPIO_Pin_5, Bit_SET); /* WE */
	GPIO_WriteBit(GPIOG, GPIO_Pin_10, Bit_RESET); /* CE of NVSRAM */
	GPIO_WriteBit(GPIOG, GPIO_Pin_12, Bit_RESET); /* CE of LCD*/
	GPIO_WriteBit(GPIOD, GPIO_Pin_4, Bit_RESET); /* OE */
	GPIO_WriteBit(GPIOE, GPIO_Pin_0, Bit_RESET); /* BLE */
	GPIO_WriteBit(GPIOE, GPIO_Pin_1, Bit_RESET); /* BHE */

	memcpy((char*)pData,(uint8_t *)(SRAM_BASE_ADDR + uliAddress), uliCount);
		
	GPIO_WriteBit(GPIOG, GPIO_Pin_10, Bit_SET); /* CE */
	GPIO_WriteBit(GPIOD, GPIO_Pin_4, Bit_SET); /* OE */
	GPIO_WriteBit(GPIOE, GPIO_Pin_0, Bit_SET); /*BLE */
	GPIO_WriteBit(GPIOE, GPIO_Pin_1, Bit_SET); /*BHE */
	GPIO_WriteBit(GPIOD, GPIO_Pin_5, Bit_RESET); 
	GPIO_WriteBit(GPIOG, GPIO_Pin_12, Bit_RESET);

	return TRUE;
}
/*****************************************************************************
**@Function		 	:  fnCurrentScreen_Display()
**@Descriptions	:	 Function to show current Screen
**@parameters		:  uiCurrentScreen : Current Screen number
**@return				:  none
*****************************************************************************/
void fnCurrentScreen_Display(uint8_t uiCurrentScreenNum)
{
	uint8_t rgcTemp[50];
//	float fTemp = 0.0f;
	uint16_t uiXPos = 0;
	uint16_t uiYPos = 0;
	uint8_t ucColumn = 0;
	uint8_t ucRow = 0;
	
	fnGr_SelectFont("Franklin24", 1);
	switch(uiCurrentScreenNum)
	{
		case SCREEN_HOME :
		{
			if( 9 >= ucStatusCountLimit )
			{
				sprintf((char *)rgcTemp,"%d",ucStatusCountLimit);
				fnGr_PutString(80, 35, rgcTemp, BLACK_CLR,
												RGB(255,255,0), ORIENTATION_90,0);
			}
			else
			{
				fnGr_SelectFont("Franklin20", 1);
				sprintf((char *)rgcTemp,"%d",ucStatusCountLimit);
				fnGr_PutString(83, 38, rgcTemp, BLACK_CLR,
												RGB(255,255,0), ORIENTATION_90,0);
			}
			
			fnGr_SelectFont("Franklin24", 1);
			if( 9 >= ucAlarmCountLimit )
			{
				sprintf((char *)rgcTemp,"%d",ucAlarmCountLimit);
				fnGr_PutString(140, 35, rgcTemp, BLACK_CLR,
												RGB(255,74,74), ORIENTATION_90,0);
			}
			else
			{
				fnGr_SelectFont("Franklin20", 1);
				sprintf((char *)rgcTemp,"%d",ucAlarmCountLimit);
				fnGr_PutString(143, 38, rgcTemp, BLACK_CLR,
												RGB(255,74,74), ORIENTATION_90,0);
			}
			
			fnGr_SelectFont("Franklin24", 1);
			/*change4*/
			/*80% log memory full alarm*/
			
			if( 9 >= ucMemoryAlarmCountLimit )
			{
				sprintf((char *)rgcTemp,"%d",ucMemoryAlarmCountLimit);
				fnGr_PutString(200, 35, rgcTemp, BLACK_CLR,
												RGB(255,74,74), ORIENTATION_90,0);
			}
			else
			{
				fnGr_SelectFont("Franklin20", 1);
				sprintf((char *)rgcTemp,"%d",ucMemoryAlarmCountLimit);
				fnGr_PutString(203, 38, rgcTemp, BLACK_CLR,
												RGB(255,74,74), ORIENTATION_90,0);
			}
			
			fnGr_SelectFont("Franklin24", 1);
			if ( TRUE == StatusFlag.stStatusFlag.bModeStatusReceived )
			{
				if ( TRUE == StatusFlag.stStatusFlag.bOperationModeStart )
				{
					fnGr_PutString(9, 270, HOME, RGB(0,0,0), RGB(255,255,0), ORIENTATION_90,0);
					fnGr_PutString(9, 232, OPERATION_MODE_STRING, RGB(0,0,0), 
												RGB(255,255,0), ORIENTATION_90,0);
				}
				else if ( FALSE == StatusFlag.stStatusFlag.bOperationModeStart )
				{
					//fnGr_PutString(9, 270, HOME, RGB(0,0,0), RGB(255,255,0), ORIENTATION_90,0);
					//fnGr_PutString(9, 232, WARMUP, RGB(0,0,0), RGB(255,255,0), ORIENTATION_90,0);
				}
			}
			else
			{
				fnGr_PutString(9, 215, HOME, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			}

			if(uliHmScreenRefreshCnt < 5000)
			{
				/*LEG 1*/
				fnGr_PutString(43, 380, LEG_1, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
				sprintf((char *)rgcTemp,"%0.1f",stCurrent_Value.fCurrPressureGauge1);
				if ( INVALID_DATA == stCurrent_Value.fCurrPressureGauge1 )
				{
					fnGr_PutString(43, 180, ERR_STRING, RED_CLR, WHITE_CLR, ORIENTATION_90,0);
				}
				else
				{
					if ( METRIC == stConfig_Value.ucTypeofUnit )
					{
						fnGr_PutString(43, 130, ALPHA_b, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
					}
					else
					{
						fnGr_PutString(43, 130, ALPHA_p, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
					}
					fnGr_PutString(43, 205, rgcTemp, BLACK_CLR, WHITE_CLR, ORIENTATION_90,1);
				}
				
				/*LEG 2*/
				fnGr_PutString(76, 380, LEG_2, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
				sprintf((char *)rgcTemp,"%0.1f",stCurrent_Value.fCurrPressureGauge2);
				if ( INVALID_DATA == stCurrent_Value.fCurrPressureGauge2 )
				{
					fnGr_PutString(76, 180, ERR_STRING, RED_CLR, WHITE_CLR, ORIENTATION_90,0);
				}
				else
				{
					if ( METRIC == stConfig_Value.ucTypeofUnit )
					{
						fnGr_PutString(76, 130, ALPHA_b, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
					}
					else
					{
						fnGr_PutString(76, 130, ALPHA_p, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
					}
					fnGr_PutString(76, 205, rgcTemp, BLACK_CLR, WHITE_CLR, ORIENTATION_90,1);
				}
				
				/*LEG 3*/
				fnGr_PutString(109, 380, LEG_3, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
				sprintf((char *)rgcTemp,"%0.1f",stCurrent_Value.fCurrPressureGauge3);
				if ( INVALID_DATA == stCurrent_Value.fCurrPressureGauge3 )
				{
					fnGr_PutString(109, 180, ERR_STRING, RED_CLR, WHITE_CLR, ORIENTATION_90,0);
				}
				else
				{
					if ( METRIC == stConfig_Value.ucTypeofUnit )
					{
						fnGr_PutString(109, 130, ALPHA_b, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
					}
					else
					{
						fnGr_PutString(109, 130, ALPHA_p, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
					}
					fnGr_PutString(109, 205, rgcTemp, BLACK_CLR, WHITE_CLR, ORIENTATION_90,1);			
				}
				
				/*base Temp*/
				fnGr_PutString(142, 380, BASE_TEMP, RGB(0,0,0), 
												RGB(255,255,255), ORIENTATION_90,0);
				if ( stCurrent_Value.fCurrInlineBaseMatTemp == INVALID_DATA )
				{
						fnGr_PutString(142, 180, ERR_STRING, RGB(255,0,0), RGB(255,255,255), ORIENTATION_90,0);
				}
				else
				{
					if ( METRIC == stConfig_Value.ucTypeofUnit )
					{
						fnGr_PutString(142, 130, ALPHA_C, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
					}
					else
					{
						fnGr_PutString(142, 130, ALPHA_F, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
					}
					sprintf((char *)rgcTemp,"%0.1f",stCurrent_Value.fCurrInlineBaseMatTemp);
					fnGr_PutString(142, 205, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
				}
				
				/*base return Temp*/
				fnGr_PutString(178, 380, BASE_RTE_TEMP, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
				if ( INVALID_DATA == stCurrent_Value.fCurrReturnLineTemp )
				{
					fnGr_PutString(178, 180, ERR_STRING, RGB(255,0,0), RGB(255,255,255), ORIENTATION_90,0);
				}
				else
				{
					if ( METRIC == stConfig_Value.ucTypeofUnit )
					{
						fnGr_PutString(178, 130, ALPHA_C, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
					}
					else
					{
						fnGr_PutString(178, 130, ALPHA_F, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
					}
					sprintf((char *)rgcTemp,"%0.1f",stCurrent_Value.fCurrReturnLineTemp);
					fnGr_PutString(178, 205, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
				}
				
				/*antifreez Temp*/
				fnGr_PutString(211, 380, ANTIFREEZ_TEMP, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
				if ( INVALID_DATA == stCurrent_Value.fCurrAntifreezeLiquidTemp )
				{
					fnGr_PutString(211, 180, ERR_STRING, RGB(255,0,0), RGB(255,255,255), ORIENTATION_90,0);
				}
				else
				{
					if ( METRIC == stConfig_Value.ucTypeofUnit )
					{
						fnGr_PutString(211, 130, ALPHA_C, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
					}
					else
					{
						fnGr_PutString(211, 130, ALPHA_F, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
					}
					sprintf((char *)rgcTemp,"%0.1f",stCurrent_Value.fCurrAntifreezeLiquidTemp);
					fnGr_PutString(211, 205, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
				}
			}
			else if(uliHmScreenRefreshCnt < 10000)
			{
				/*hard Temp*/
				fnGr_PutString(43, 380, HARDNER_TEMP, RGB(0,0,0), 
												RGB(255,255,255), ORIENTATION_90,0);
				if ( stCurrent_Value.fCurrInlineHardMatTemp == INVALID_DATA )
				{
					fnGr_PutString(43, 180, ERR_STRING, RGB(255,0,0), RGB(255,255,255), ORIENTATION_90,0);
				}
				else
				{
					if ( METRIC == stConfig_Value.ucTypeofUnit )
					{
						fnGr_PutString(43, 130, ALPHA_C, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
					}
					else
					{
						fnGr_PutString(43, 130, ALPHA_F, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
					}
					sprintf((char *)rgcTemp,"%0.1f",stCurrent_Value.fCurrInlineHardMatTemp);
					fnGr_PutString(43, 205, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
				}
				
				/*ambient Temp OC Container*/
				fnGr_PutString(76, 380, AMBIENT_OC, RGB(0,0,0), 
												RGB(255,255,255), ORIENTATION_90,0);
				if ( INVALID_DATA == stCurrent_Value.fCurrOutsideAmbientTemp )
				{
					fnGr_PutString(76, 180, ERR_STRING, RGB(255,0,0), RGB(255,255,255), ORIENTATION_90,0);
				}
				else
				{
					if ( METRIC == stConfig_Value.ucTypeofUnit )
					{
						fnGr_PutString(76, 130, ALPHA_C, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
					}
					else
					{
						fnGr_PutString(76, 130, ALPHA_F, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
					}
					sprintf((char *)rgcTemp,"%0.1f",stCurrent_Value.fCurrOutsideAmbientTemp);
					fnGr_PutString(76, 205, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
				}
				
				/*Dew*/
				fnGr_PutString(109, 380, DEW_POINT, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
				if ( INVALID_DATA == stCurrent_Value.fCurrDewPoint )
				{
					fnGr_PutString(109, 180, ERR_STRING, RGB(255,0,0), RGB(255,255,255), ORIENTATION_90,0);
				}
				else
				{
					if ( METRIC == stConfig_Value.ucTypeofUnit )
					{
						fnGr_PutString(109, 130, ALPHA_C, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
					}
					else
					{
						fnGr_PutString(109, 130, ALPHA_F, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
					}
					sprintf((char *)rgcTemp,"%0.1f",stCurrent_Value.fCurrDewPoint);
					fnGr_PutString(109, 205, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
				}
				
				/*3rdMarch*/
				/*Hardner Material Liquid level*/
				fnGr_PutString(142, 380, HARDNER_MATERIAL_LIQUID_LEVEL, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
				if ( INVALID_DATA == stCurrent_Value.fLiquid_Hardner_Level_Curr )
				{
					fnGr_PutString(142, 180, ERR_STRING, RGB(255,0,0), RGB(255,255,255), ORIENTATION_90,0);
				}
				else
				{/*change10*/
					fnGr_PutString(142, 120, PERCENT, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
//					sprintf((char *)rgcTemp,"%0.1f",(((stCurrent_Value.fLiquid_Hardner_Level_Curr / 10) / 
//										stConfig_Value.fConfigHardTankHeight) * 100.0f));
					sprintf((char *)rgcTemp,"%0.1f",((stCurrent_Value.fLiquid_Hardner_Level_Curr / 
										stConfig_Value.fConfigHardTankHeight) * 100.0f));
					fnGr_PutString(142, 205, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
				}
				
				/*OPMM Battery Status*/
				fnGr_PutString(178, 380, OPMM_BATTERY_STATUS, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
				sprintf((char *)rgcTemp,"%0.1f",stCurrent_Value.fCurrBateryStatus);
				fnGr_PutString(178, 205, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
				fnGr_PutString(178, 120, VOLT, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
				
				/*System Time And Date*/		
				if ( METRIC == stConfig_Value.ucTypeofUnit )
				{
					fnGr_PutString(211, 315, stDisplayDateTime.ucDD, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
					fnGr_PutString(211, 279, stDisplayDateTime.ucMM, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
				}
				else
				{
					fnGr_PutString(211, 279, stDisplayDateTime.ucDD, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
					fnGr_PutString(211, 315, stDisplayDateTime.ucMM, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
				}
				fnGr_PutString(211, 289, SLASH, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
				fnGr_PutString(211, 253, SLASH, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
				fnGr_PutString(211, 243, stDisplayDateTime.ucYY, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
				
				fnGr_PutString(211, 170, stDisplayDateTime.ucHR, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
				fnGr_PutString(211, 140, COLON, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
				fnGr_PutString(211, 130, stDisplayDateTime.ucMIN, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
				fnGr_PutString(211, 100, COLON, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
				fnGr_PutString(211, 90, stDisplayDateTime.ucSEC, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			
						
			}
			else if(uliHmScreenRefreshCnt > 10000)
			{
				uliHmScreenRefreshCnt = 0;
				ucSwitchPressFlag = TRUE;
			}
		}break;
		
		case SCREEN_MENU_1:
		{
			fnGr_PutString(9, 230, MENU, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(50, 380, PARAMETERS, RGB(0,0,0), 
														LIGHT_GREY_CLR, ORIENTATION_90,0);
			fnGr_PutString(75, 380, SETTINGS, RGB(0,0,0), 
														LIGHT_GREY_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(60, 185, STATUS, RGB(0,0,0), 
														LIGHT_GREY_CLR, ORIENTATION_90,0);
			
														
			fnGr_PutString(115, 380, CURRENT, RGB(0,0,0), 
											LIGHT_GREY_CLR, ORIENTATION_90,0);
			fnGr_PutString(140, 380, PARAMETERS, RGB(0,0,0), 
											LIGHT_GREY_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(125, 185, ALARMS, RGB(0,0,0), 
														LIGHT_GREY_CLR, ORIENTATION_90,0);
														
		
			
			fnGr_PutString(190, 380, ADVANCED, RGB(0,0,0), 
														LIGHT_GREY_CLR, ORIENTATION_90,0);
														
			fnGr_PutString(190, 185, NOTIFICATIONS, RGB(0,0,0), 
														LIGHT_GREY_CLR, ORIENTATION_90,0);
														
		}break;
		
		case SCREEN_MENU_2:
		{
			fnGr_PutString(9, 230, MENU, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(60, 380, STALL_TEST, RGB(0,0,0), 
														LIGHT_GREY_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(50, 185, PERIPHERAL, RGB(0,0,0), 
											LIGHT_GREY_CLR, ORIENTATION_90,0);
			fnGr_PutString(75, 185, SETTINGS, RGB(0,0,0), 
											LIGHT_GREY_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(115, 380, FIRMWARE, RGB(0,0,0), 
											LIGHT_GREY_CLR, ORIENTATION_90,0);
			fnGr_PutString(140, 380, DETAILS, RGB(0,0,0), 
											LIGHT_GREY_CLR, ORIENTATION_90,0);
			/*change3*/
			/*Ethernet and wifi config screen*/
			fnGr_PutString(115, 185, ETHDISPLAY, RGB(0,0,0), 
														LIGHT_GREY_CLR, ORIENTATION_90,0);
			fnGr_PutString(140, 185, IP_DETAILS, RGB(0,0,0), 
														LIGHT_GREY_CLR, ORIENTATION_90,0);
			fnGr_PutString(180, 380, WIFIDISPLAY, RGB(0,0,0), 
														LIGHT_GREY_CLR, ORIENTATION_90,0);
			fnGr_PutString(205, 380, IP_DETAILS, RGB(0,0,0), 
														LIGHT_GREY_CLR, ORIENTATION_90,0);											
														
			
		}break;
		
		case SCREEN_STALL:
		{
			fnGr_PutString(9, 240, STALL_TEST, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(76, 100, MANUAL, WHITE_CLR, DARK_GREY_CLR, ORIENTATION_90,0);
			
			/*Upstroke Stall*/
			fnGr_PutString(43, 380, UPSTROKE_STRING, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			if ( FALSE == StatusFlag.stStatusFlag.bStallTestStatusReceived )
			{
				if ( TRUE == StatusFlag.stStatusFlag.bUpstrokeStatusReceived )
				{
					if ( TRUE == StatusFlag.stStatusFlag.bUpStrokeStatus )
					{
						fnGr_PutString(43, 170, STRING_PASS, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
					}
					else if ( FALSE == StatusFlag.stStatusFlag.bUpStrokeStatus )
					{
						fnGr_PutString(43, 170, STRING_FAIL, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
					}
				}
				else
				{
					fnGr_PutString(43, 170, SPAC, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
				}
				
				if ( TRUE == StatusFlag.stStatusFlag.bDownstrokeStatusReceived )
				{
					if ( TRUE == StatusFlag.stStatusFlag.bDownStrokeStatus )
					{
						fnGr_PutString(76, 170, STRING_PASS, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
					}
					else if ( FALSE == StatusFlag.stStatusFlag.bDownStrokeStatus )
					{
						fnGr_PutString(76, 170, STRING_FAIL, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
					}
				}
				else
				{
					fnGr_PutString(76, 170, SPAC, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
				}
			}
			else
			{
				fnGr_PutString(76, 170, SPAC, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
				fnGr_PutString(43, 170, SPAC, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			}
			
			/*Downstroke Stall*/
			fnGr_PutString(76, 380, DOWNSTROKE_STRING, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			
			/*Linear Transducer Reading*/
			fnGr_PutString(109, 380, LT, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			sprintf((char *)rgcTemp,"%0.0f",stCurrent_Value.fCurrLinearTransducer);
			//if ( 2 >= strlen((const char *)rgcTemp) )
				if( 99.0f > stCurrent_Value.fCurrLinearTransducer )
			{
				//strcat((char *)rgcTemp,"  ");
				fnGr_PutString(109, 341, "  ", RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			}
			fnGr_PutString(109, 380, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			fnGr_PutString(109, 295, MILIMETER, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			
			/*UP*/
			fnGr_PutString(109, 205, UP, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			/*DOWN*/
			fnGr_PutString(109, 130, DOWN, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			
			/*Leg 1*/
			fnGr_PutString(142, 360, LEG_1, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			sprintf((char *)rgcTemp,"%d",stCurrent_Value.iCurrPressureLegAUp);
			fnGr_PutString(142, 230, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			fnGr_PutString(142, 185, PERCENT, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			
			sprintf((char *)rgcTemp,"%d",stCurrent_Value.iCurrPressureLegADown);
			fnGr_PutString(142, 135, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			fnGr_PutString(142, 90, PERCENT, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			
			/*Leg 2*/
			fnGr_PutString(178, 360, LEG_2, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			sprintf((char *)rgcTemp,"%d",stCurrent_Value.iCurrPressureLegBUp);
			fnGr_PutString(178, 230, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			fnGr_PutString(178, 185, PERCENT, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			
			sprintf((char *)rgcTemp,"%d",stCurrent_Value.iCurrPressureLegBDown);
			fnGr_PutString(178, 135, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			fnGr_PutString(178, 90, PERCENT, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
		
			/*Leg 3*/
			fnGr_PutString(211, 360, LEG_3, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			sprintf((char *)rgcTemp,"%d",stCurrent_Value.iCurrPressureLegCUp);
			fnGr_PutString(211, 230, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			fnGr_PutString(211, 185, PERCENT, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			
			sprintf((char *)rgcTemp,"%d",stCurrent_Value.iCurrPressureLegCDown);
			fnGr_PutString(211, 135, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			fnGr_PutString(211, 90, PERCENT, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
		}break;
		case SCREEN_PARAMETER_SET_1:
		{
			fnGr_PutString(9, 280, PARAMETER_SETTING, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(60, 380, LIQUID_LEVEL, RGB(0,0,0), 
														LIGHT_GREY_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(60, 185, TEMPERATURE, RGB(0,0,0), 
														LIGHT_GREY_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(125, 380, TIMERS, RGB(0,0,0), 
														LIGHT_GREY_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(125, 185, PUMP, RGB(0,0,0), 
														LIGHT_GREY_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(190, 380, OTHERS, RGB(0,0,0), 
														LIGHT_GREY_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(190, 185, HYSTERESIS, RGB(0,0,0), 
														LIGHT_GREY_CLR, ORIENTATION_90,0);
		}break;
		
		case SCREEN_LIQUID_LEVEL_1:
		{
			fnLCDbmp(uiCursorXPos, uiCursorYPos,30,26,ucrgArrow);
			
			/*Screen Title*/
			fnGr_PutString(9, 250, LIQUID_LEVEL, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			
			/*Base Tank height*/
			fnGr_PutString(43, 380, BASE_TANK_HEIGHT, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			sprintf((char *)rgcTemp,"%0.1f",stConfig_Value.fConfigBaseTankHeight);
			fnGr_PutString(43, 165, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			/*Hardner tank height*/
			fnGr_PutString(76, 380, HARDNER_TANK_HEIGHT, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			sprintf((char *)rgcTemp,"%0.1f",stConfig_Value.fConfigHardTankHeight);
			fnGr_PutString(76, 165, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			/*Hardner Liq High TH*/
			fnGr_PutString(109, 380, HARDNER_LIQ_HIGH_TH, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			sprintf((char *)rgcTemp,"%0.1f",stConfig_Value.fConfigHardLiquidLevelHTH);
			fnGr_PutString(109, 165, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
					
			/*Hardner Liq Low TH*/
			fnGr_PutString(142, 380, HARDNER_LIQ_LOW_TH, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			sprintf((char *)rgcTemp,"%0.1f",stConfig_Value.fConfigHardLiquidLevelLTH);
			fnGr_PutString(142, 165, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
					
			/*Base Liq Lev High TH*/
			fnGr_PutString(178, 380, BASE_LIQ_LEV_HIGH_TH, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			sprintf((char *)rgcTemp,"%0.1f",stConfig_Value.fConfigBaseLiquidLevelHTH);
			fnGr_PutString(178, 165, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
					
			/*Base Liq Lev Low TH*/
			fnGr_PutString(211, 380, BASE_LIQ_LEV_LOW_TH, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			sprintf((char *)rgcTemp,"%0.1f",stConfig_Value.fConfigBaseLiquidLevelLTH);
			fnGr_PutString(211, 165, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			if ( METRIC == stConfig_Value.ucTypeofUnit )
			{
				fnGr_PutString(43, 90, MILIMETER, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
				fnGr_PutString(76, 90, MILIMETER, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
				fnGr_PutString(109, 90 , MILIMETER, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
				fnGr_PutString(142, 90 , MILIMETER, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
				fnGr_PutString(178, 90 , MILIMETER, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
				fnGr_PutString(211, 90 , MILIMETER, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			}
			else if( IMPERIAL == stConfig_Value.ucTypeofUnit )
			{
				fnGr_PutString(43, 90, INCH, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
				fnGr_PutString(76, 90, INCH, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
				fnGr_PutString(109, 90 , INCH, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
				fnGr_PutString(142, 90 , INCH, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
				fnGr_PutString(178, 90 , INCH, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
				fnGr_PutString(211, 90 , INCH, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			}
		}break;

		case SCREEN_LIQUID_LEVEL_2:
		{
			fnLCDbmp(uiCursorXPos, uiCursorYPos,30,26,ucrgArrow);
			
			/*Screen Title*/
			fnGr_PutString(9, 250, LIQUID_LEVEL, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			
			/*Liq Lv Sampling rate*/
			fnGr_PutString(43, 380, LIQ_LV_SAMPLING_RATE, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			sprintf((char *)rgcTemp,"%d",stConfig_Value.uiConfigLLWindowSize);
			fnGr_PutString(43, 165, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			fnGr_PutString(43, 90 , MILISECOND, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
		}break;
			
		case SCREEN_TEMP_SETTING_1:
		{
			fnLCDbmp(uiCursorXPos, uiCursorYPos,30,26,ucrgArrow);
			
			/*Screen Title*/
			fnGr_PutString(9, 260, TEMPERATURE, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			
			/*Unit*/
			if ( METRIC == stConfig_Value.ucTypeofUnit )
			{
				fnGr_PutString(9, 140, ALPHA_C_BRACKET, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			}
			else
			{
				fnGr_PutString(9, 140, ALPHA_F_BRACKET, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			}
			
			/*Antifreez Liq*/
			fnGr_PutString(43, 380, ANTIFREEZ_LIQ, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			fnGr_PutString(43, 230, MIN, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			fnGr_PutString(76, 230, SET_STRING, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			fnGr_PutString(109, 230, MAX, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			/*MIN*/
			sprintf((char *)rgcTemp,"%0.1f",stConfig_Value.fConfigAntiFrzTempLTH);
			fnGr_PutString(43, 150, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			/*SET*/
			sprintf((char *)rgcTemp,"%0.1f",stConfig_Value.fConfigAntiFrzTempSet);
			fnGr_PutString(76, 150, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			/*MAX*/
			sprintf((char *)rgcTemp,"%0.1f",stConfig_Value.fConfigAntiFrzTempHTH);
			fnGr_PutString(109, 150, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
					
			
			/*Base Material*/
			fnGr_PutString(142, 380, BASE_MATERIAL_STRING, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			fnGr_PutString(142, 230, MIN, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			fnGr_PutString(178, 230, SET_STRING, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			fnGr_PutString(211, 230, MAX, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			/*Min*/
			sprintf((char *)rgcTemp,"%03.1f",stConfig_Value.fConfigInlineBaseTempLTH);
			fnGr_PutString(142, 150, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			/*SET*/
			sprintf((char *)rgcTemp,"%03.1f",stConfig_Value.fConfigInlineBaseTempSet);
			fnGr_PutString(178, 150, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			/*MAX*/
			sprintf((char *)rgcTemp,"%03.1f",stConfig_Value.fConfigInlineBaseTempHTH);
			fnGr_PutString(211, 150, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
		}break;
		
		case SCREEN_TEMP_SETTING_2:
		{
			fnLCDbmp(uiCursorXPos, uiCursorYPos,30,26,ucrgArrow);
			
			/*Screen Title*/
			fnGr_PutString(9, 260, TEMPERATURE, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			
			/*Unit*/
			if ( METRIC == stConfig_Value.ucTypeofUnit )
			{
				fnGr_PutString(9, 140, ALPHA_C_BRACKET, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			}
			else
			{
			fnGr_PutString(9, 140, ALPHA_F_BRACKET, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			}
			
			/*Hard Material*/
			fnGr_PutString(43, 380, HARD_MATERIAL_STRING, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			fnGr_PutString(43, 230, MIN, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			fnGr_PutString(76, 230, SET_STRING, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			fnGr_PutString(109, 230, MAX, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			/*Min*/
			sprintf((char *)rgcTemp,"%0.1f",stConfig_Value.fConfigInlineHardTempLTH);
			fnGr_PutString(43, 150, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			/*SET*/
			sprintf((char *)rgcTemp,"%0.1f",stConfig_Value.fConfigInlineHardTempSet);
			fnGr_PutString(76, 150, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			/*Max*/
			sprintf((char *)rgcTemp,"%0.1f",stConfig_Value.fConfigInlineHardTempHTH);
			fnGr_PutString(109, 150, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
					
			
			/*Pipe Temp*/
			fnGr_PutString(142, 380, PIPE_TEMP, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			fnGr_PutString(142, 230, MIN, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			fnGr_PutString(178, 230, MAX, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			/*Min*/
			sprintf((char *)rgcTemp,"%0.1f",stConfig_Value.fConfigPipeTempLTH);
			fnGr_PutString(142, 150, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			/*Max*/
			sprintf((char *)rgcTemp,"%0.1f",stConfig_Value.fConfigPipeTempHTH);
			fnGr_PutString(178,150, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
		}break;
		
		case SCREEN_TIMER_SETTING_1:
		{
			fnLCDbmp(uiCursorXPos, uiCursorYPos,30,26,ucrgArrow);
			
			/*Screen Title*/
			fnGr_PutString(9, 230, TIMERS, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			
			/*Unit*/
			//fnGr_PutString(9, 185, SEC, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			
			/*Prime Time*/
			fnGr_PutString(43, 380, PRIME_TIME, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			sprintf((char *)rgcTemp,"%0.1f",stConfig_Value.fConfigPrimeTime);
			fnGr_PutString(43, 165, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			
			/*Flush Time*/
			fnGr_PutString(76, 380, FLUSH_TIME, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			sprintf((char *)rgcTemp,"%0.1f",stConfig_Value.fConfigFlushTime);
			fnGr_PutString(76, 165, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			
			/*Gun Delay Time*/
			fnGr_PutString(109, 380, GUN_DELAY_TIME, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			sprintf((char *)rgcTemp,"%0.0f",stConfig_Value.fConfigGunDelayTime);
			fnGr_PutString(109, 171, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			
			/*Reverse Delay Time*/
			fnGr_PutString(142, 380, REVERSE_DELAY_TIME, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			sprintf((char *)rgcTemp,"%0.0f",stConfig_Value.fConfigReverseDelayTime);
			fnGr_PutString(142, 171, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			
			/*Spray Overlap Time*/
			fnGr_PutString(178, 380, SPRAY_OVERLAP_TIME, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			sprintf((char *)rgcTemp,"%0.0f",stConfig_Value.fConfigSprayOverlapTime);
			fnGr_PutString(178, 171, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			
			/*Data Logging Time*/
			fnGr_PutString(211, 380, DATA_LOGGING_PERIOD, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			sprintf((char *)rgcTemp,"%0.1f",stConfig_Value.fConfigDataLoggingPeriod);
			fnGr_PutString(211, 165, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			fnGr_PutString(43, 90, SEC_WB, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			fnGr_PutString(76, 90, SEC_WB, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			fnGr_PutString(109, 90, MS_WB, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			fnGr_PutString(142, 90, MS_WB, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			fnGr_PutString(178, 90, MS_WB, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			fnGr_PutString(211, 90, SEC_WB, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
		}break;
		
		case SCREEN_TIMER_SETTING_2:
		{
			fnLCDbmp(uiCursorXPos, uiCursorYPos,30,26,ucrgArrow);
			
			/*Screen Title*/
			fnGr_PutString(9, 230, TIMERS, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			
			/*Unit*/
			//fnGr_PutString(9, 185, SEC, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			
			/*Time Before Stall*/
			fnGr_PutString(43, 380, TIME_BEFORE_STALL, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			sprintf((char *)rgcTemp,"%0.1f",stConfig_Value.fConfigTimeBeforeStall);
			fnGr_PutString(43, 165, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			/*Time Before comparing Stall*/
			fnGr_PutString(76, 380, TIME_BEFORE_COMPARING_STALL, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			sprintf((char *)rgcTemp,"%0.1f",stConfig_Value.fConfigTimeCompareStall);
			fnGr_PutString(76, 165, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			/*DCLS Time*/
			fnGr_PutString(109, 380, DCLS_TIME, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			sprintf((char *)rgcTemp,"%0.1f",stConfig_Value.fConfigDCLSHomeTime);
			fnGr_PutString(109, 165, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			/*HLS Time*/
			fnGr_PutString(142, 380,HLS_TIME, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			sprintf((char *)rgcTemp,"%0.1f",stConfig_Value.fConfigHomeDCLSTime);
			fnGr_PutString(142, 165, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			/*Gun position switching time*/
			fnGr_PutString(178, 380, GUN_SWITCHING_TIME, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			sprintf((char *)rgcTemp,"%0.1f",stConfig_Value.fGunPosSwTime);
			fnGr_PutString(178, 165, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			/*Delay time before turning ON spray valve*/
			fnGr_PutString(211, 380, SPRAY_GUN_DELAY, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			sprintf((char *)rgcTemp,"%0.1f",stConfig_Value.fConfigTimebeforeONSprayValve);
			fnGr_PutString(211, 165, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			fnGr_PutString(43, 90, SEC_WB, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			fnGr_PutString(76, 90, SEC_WB, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			fnGr_PutString(109, 90, SEC_WB, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			fnGr_PutString(142, 90, SEC_WB, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			fnGr_PutString(178, 90, SEC_WB, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			fnGr_PutString(211, 90, SEC_WB, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
		}break;
		
		case SCREEN_TIMER_SETTING_3:
		{
			fnLCDbmp(uiCursorXPos, uiCursorYPos,30,26,ucrgArrow);
			
			/*Screen Title*/
			fnGr_PutString(9, 230, TIMERS, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			
			/*Unit*/
			//fnGr_PutString(9, 185, SEC, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			
			/*Flush Warning period*/
			fnGr_PutString(43, 380, FLUSH_WARNING_TIME, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			sprintf((char *)rgcTemp,"%0.1f",stConfig_Value.fConfigFlushWarnTime);
			fnGr_PutString(43, 165, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			/*3rdMarch*/
			/*CONFIG_TIME_TO_START_STALL*/
			fnGr_PutString(76, 380, TIME_START_STALL_TEST, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			sprintf((char *)rgcTemp,"%0.1f",stConfig_Value.fTimerStartStallTest);
			fnGr_PutString(76, 165, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			/*CONFIG_STOP_MOTOR_ROTATION_TIME*/
			fnGr_PutString(109, 380, TIME_SPRAY_ROTATION_OFF_TIME, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			sprintf((char *)rgcTemp,"%0.1f",stConfig_Value.fTimerSprayLastRotStop);
			fnGr_PutString(109, 165, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			/*20thJuly2016*/
			/*CONFIG_IDEAL_PERIODIC_TIME*/
			fnGr_PutString(142, 380, TIME_IDLE_PERIODIC_TIME, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			sprintf((char *)rgcTemp,"%0.1f",stConfig_Value.fTimerIdealPeriodic);
			fnGr_PutString(142, 165, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
		
		/*CONFIG_UTC_TIME_OFFSET change1*/
			fnGr_PutString(175, 380, UTC_TIME_OFFSET, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
		//	sprintf((char *)rgcTemp,"%0.1f",stConfig_Value.fTimerIdealPeriodic);
		//	fnGr_PutString(175, 165, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
				
				
				sprintf((char *)stDisplayDateTime.ucOFFHR,"%02d",stConfig_Value.ucConfig_TimeOffsetHour);
				
				sprintf((char *)stDisplayDateTime.ucOFFMIN,"%02d",stConfig_Value.ucConfig_TimeOffsetMin);
				
				sprintf((char *)stDisplayDateTime.ucOFFSEC,"%02d",stConfig_Value.ucConfig_TimeOffsetSec);
				
			/*change1*/
//			stConfig_Value.ucConfig_TimeOffsetSign = 0x01;
			if(0x01 == stConfig_Value.ucConfig_TimeOffsetSign)
			{
		//	DEBUG_PRINTF("IN NEGATIVE \r\n");	
			fnGr_PutString(175, 160, NEGATIVE, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			}
			else
			{
			//	fnGr_PutString(175, 145, POSITIVE, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			}				
		
//			fnGr_PutString(175, 178, stDisplayDateTime.ucOFFHR, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
//			fnGr_PutString(175, 104, COLON, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
//			fnGr_PutString(175, 144, stDisplayDateTime.ucOFFMIN, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
//			
			fnGr_PutString(175, 203, stDisplayDateTime.ucOFFHR, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			fnGr_PutString(175, 130, COLON, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			fnGr_PutString(175, 173, stDisplayDateTime.ucOFFMIN, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
		
			fnGr_PutString(43, 90, SEC_WB, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			fnGr_PutString(76, 90, SEC_WB, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			fnGr_PutString(109, 90, SEC_WB, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			fnGr_PutString(142, 90, SEC_WB, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
		}break;
		
		case SCREEN_TIMER_SETTING_4:
		{
			fnLCDbmp(uiCursorXPos, uiCursorYPos,30,26,ucrgArrow);
			
			/*Screen Title*/
			fnGr_PutString(9, 230, TIMERS, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			
			/*System date*/
			fnGr_PutString(43, 380, UTC_DATE, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);

			/*DD*/
			fnGr_PutString(43, 250, DD, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			fnGr_PutString(43, 165, stDisplayDateTime.ucUTCDD, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			/*MM*/
			fnGr_PutString(76, 250, MM, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			fnGr_PutString(76, 165, stDisplayDateTime.ucUTCMM, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			/*YY*/
			fnGr_PutString(109, 250, YY, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			fnGr_PutString(109, 165, stDisplayDateTime.ucUTCYY, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			/*System Time*/
			fnGr_PutString(142, 380, UTC_TIME, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			
			/*HR*/
			fnGr_PutString(142, 250, HR, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			fnGr_PutString(142, 165, stDisplayDateTime.ucUTCHR, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			/*MIN*/
			fnGr_PutString(178, 250, MIN_TIME, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			fnGr_PutString(178, 165, stDisplayDateTime.ucUTCMIN, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			/*SEC*/
			fnGr_PutString(211, 250, SEC, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			fnGr_PutString(211, 165, stDisplayDateTime.ucUTCSEC, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			fnGr_PutString(211, 33, SET_BUTTON, WHITE_CLR , 
			DARK_GREY_CLR, ORIENTATION_90,0);
		}break;
		
		case SCREEN_PUMP_SETTING:
		{
			
			fnLCDbmp(uiCursorXPos, uiCursorYPos,30,26,ucrgArrow);
			
			/*Screen Title*/
			fnGr_PutString(9, 250, PUMP_SETTING, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			
			/*Cylinder count*/
			fnGr_PutString(43, 380,  CYLINDER_COUNT, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			sprintf((char *)rgcTemp,"%d",stConfig_Value.uiConfigCylinderCount);
			fnGr_PutString(43, 170, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			/*Leg 1*/
			fnGr_PutString(76, 310, AREA, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			fnGr_PutString(76, 380, LEG_1, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);						
			/*Area*/
			sprintf((char *)rgcTemp,"%0.1f",stConfig_Value.fConfigAreaofCylinderA);
			fnGr_PutString(76, 165, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
				
			/*Leg 2*/
			fnGr_PutString(109, 380, LEG_2, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			/*Area*/
			sprintf((char *)rgcTemp,"%0.1f",stConfig_Value.fConfigAreaofCylinderB);
			fnGr_PutString(109, 165, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			
			/*Leg 3*/
			fnGr_PutString(142, 380, LEG_3, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			/*Area*/
			sprintf((char *)rgcTemp,"%0.1f",stConfig_Value.fConfigAreaofCylinderC);
			fnGr_PutString(142, 165, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			if ( METRIC == stConfig_Value.ucTypeofUnit )
			{
				fnGr_PutString(76, 90, MM2, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);	
				fnGr_PutString(109, 90, MM2, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);	
				fnGr_PutString(142, 90, MM2, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);	
			}
			else
			{
				fnGr_PutString(76, 90, INCH2, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
				fnGr_PutString(109, 90, INCH2, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
				fnGr_PutString(142, 90, INCH2, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			}
			
		}break;
		
		case SCREEN_OTHER_SETTING:
		{
			fnLCDbmp(uiCursorXPos, uiCursorYPos,30,26,ucrgArrow);
			
			/*Screen Title*/
			fnGr_PutString(9, 230, OTHERS, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			
			/*Current Joint no*/
			fnGr_PutString(43, 380,  CURRENT_KP_NO, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			sprintf((char *)rgcTemp,"%0.0f",stConfig_Value.fConfigJointNum);
			fnGr_PutString(43, 165, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			/*Spraying Count*/
			fnGr_PutString(76, 380, SPRAYING_COUNT, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);						
			sprintf((char *)rgcTemp,"%d",stConfig_Value.uiConfigSprayCount);
			fnGr_PutString(76, 165, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			/*Type Of Unit*/
			fnGr_PutString(109, 380, TYPE_OF_UNIT, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			
			if ( 0 == stConfig_Value.ucTypeofUnit )
			{
				fnGr_PutString(109, 150, METRIC_STRING, LIGHT_GREY_CLR, RGB(255,255,255), ORIENTATION_90,0);
				fnGr_PutString(142, 165, IMPERIAL_STRING, LIGHT_GREY_CLR, RGB(255,255,255), ORIENTATION_90,0);
			}
			else	if ( METRIC == stConfig_Value.ucTypeofUnit )
			{
				fnGr_PutString(109, 150, METRIC_STRING, RED_CLR, RGB(255,255,255), ORIENTATION_90,0);
				fnGr_PutString(142, 165, IMPERIAL_STRING, LIGHT_GREY_CLR, RGB(255,255,255), ORIENTATION_90,0);
			}
			else	if ( IMPERIAL == stConfig_Value.ucTypeofUnit )
			{
				fnGr_PutString(109, 150, METRIC_STRING, LIGHT_GREY_CLR, RGB(255,255,255), ORIENTATION_90,0);
				fnGr_PutString(142, 165, IMPERIAL_STRING, RED_CLR, RGB(255,255,255), ORIENTATION_90,0);
			}	
			
			/*Buzzer Volume*/
			fnGr_PutString(178, 380, BUZZER_VOLUME, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			
			/*Delay time before turning ON spray valve*/
			fnGr_PutString(211, 373, NEGATIVE, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			fnGr_PutString(211, 35, POSITIVE, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
						
			fnGr_BuzzerVolume();
		}break;
		case SCREEN_OTHER_SETTING_1:
		{
			fnGr_PutString(9, 240, OTHERS, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			
			fnLCDbmp(uiCursorXPos, uiCursorYPos,30,26,ucrgArrow);
			
			/*Signature Value*/
			fnGr_PutString(43, 380,  SIGNATURE_VALUE, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			sprintf((char *)rgcTemp,"%d",stConfig_Value.uiSignature);
			fnGr_PutString(43, 165, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			/*OPMM Baterry Knee Voltage*/
			fnGr_PutString(76, 380,  BATTERY_KNEE_VOLTAGE, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			sprintf((char *)rgcTemp,"%0.1f",stConfig_Value.fOPMMBatKneeVoltage);
			fnGr_PutString(76, 160, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			fnGr_PutString(76, 80, VOLT, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			
			/*CONFIG_PIPE_DIAMETER*/
			fnGr_PutString(109, 380,  PIPE_DIAMETER, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			sprintf((char *)rgcTemp,"%0.1f",stConfig_Value.fPipeDiameter);
			fnGr_PutString(109, 160, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			if ( METRIC == stConfig_Value.ucTypeofUnit )
			{
				fnGr_PutString(109, 80 , MILIMETER, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			}
			else if( IMPERIAL == stConfig_Value.ucTypeofUnit )
			{
				fnGr_PutString(109, 80, INCH, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			}
		}break;
		case SCREEN_HYSTERESIS_1:
		{
			fnLCDbmp(uiCursorXPos, uiCursorYPos,30,26,ucrgArrow);
			
			/*Screen Title*/
			fnGr_PutString(9, 250, HYSTERESIS, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			
			/*BASE_HYSTERESIS_POS*/
			fnGr_PutString(43, 380, BASE_HYSTERESIS_POS, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);						
			sprintf((char *)rgcTemp,"%0.1f",stConfig_Value.fConfigHysInlineBasePos);
			fnGr_PutString(43, 165, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			/*BASE_HYSTERESIS_NEG*/
			fnGr_PutString(76, 380, BASE_HYSTERESIS_NEG, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);						
			sprintf((char *)rgcTemp,"%0.1f",stConfig_Value.fConfigHysInlineBaseNeg);
			fnGr_PutString(76, 165, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			/*HARD_HYSTERESIS_POS*/
			fnGr_PutString(109, 380, HARD_HYSTERESIS_POS, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);						
			sprintf((char *)rgcTemp,"%0.1f",stConfig_Value.fConfigInlineHysHardPos);
			fnGr_PutString(109, 165, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			/*HARD_HYSTERESIS_NEG*/
			fnGr_PutString(142, 380, HARD_HYSTERESIS_NEG, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);						
			sprintf((char *)rgcTemp,"%0.1f",stConfig_Value.fConfigInlineHysHardNeg);
			fnGr_PutString(142, 165, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			/*ANTIFRZ_HYSTERESIS_POS*/
			fnGr_PutString(178, 380, ANTIFRZ_HYSTERESIS_POS, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);						
			sprintf((char *)rgcTemp,"%0.1f",stConfig_Value.fConfigHysAntiFrzPos);
			fnGr_PutString(178, 165, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			/*ANTIFRZ_HYSTERESIS_NEG*/
			fnGr_PutString(211, 380, ANTIFRZ_HYSTERESIS_NEG, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);						
			sprintf((char *)rgcTemp,"%0.1f",stConfig_Value.fConfigHysAntiFrzNeg);
			fnGr_PutString(211, 165, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
		}break;
		case SCREEN_HYSTERESIS_2:
		{
			fnLCDbmp(uiCursorXPos, uiCursorYPos,30,26,ucrgArrow);
			
			/*Screen Title*/
			fnGr_PutString(9, 250, HYSTERESIS, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			
			/*RECIRC_PRESSURE_HYSTERESIS_POS*/
			fnGr_PutString(43, 380, RECIRC_PRESSURE_HYSTERESIS_POS, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);						
			sprintf((char *)rgcTemp,"%0.1f",stConfig_Value.fConfigHysRecircPresPos);
			fnGr_PutString(43, 165, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			/*RECIRC_PRESSURE_HYSTERESIS_NEG*/
			fnGr_PutString(76, 380, RECIRC_PRESSURE_HYSTERESIS_NEG, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);						
			sprintf((char *)rgcTemp,"%0.1f",stConfig_Value.fConfigHysRecircPresNeg);
			fnGr_PutString(76, 165, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			/*SPRAY_PRESSURE_HYSTERESIS_POS*/
			fnGr_PutString(109, 380, SPRAY_PRESSURE_HYSTERESIS_POS, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);						
			sprintf((char *)rgcTemp,"%0.1f",stConfig_Value.fConfigHysSprayPresPos);
			fnGr_PutString(109, 165, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			/*SPRAY_PRESSURE_HYSTERESIS_NEG*/
			fnGr_PutString(142, 380, SPRAY_PRESSURE_HYSTERESIS_NEG, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);						
			sprintf((char *)rgcTemp,"%0.1f",stConfig_Value.fConfigHysSprayPresNeg);
			fnGr_PutString(142, 165, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
		}break;
		
		case SCREEN_JOB_SETTING_1:
		{
			fnLCDbmp(uiCursorXPos, uiCursorYPos,30,26,ucrgArrow);
			
			/*Screen Title*/
			fnGr_PutString(9, 250, JOB_SETTING, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			
			/*Supervisor*/
			fnGr_PutString(43, 380, SUPERVISOR, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			fnGr_PutString(76, 350, rgucSupervisorName, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			
			/*Coating Material*/
			fnGr_PutString(109, 380, COATING_MATERIAL, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			fnGr_PutString(142, 350, rgucCoatingMaterial, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			
			/*Base Batch number*/
			fnGr_PutString(178, 380, BASE_BATCH_NUMBER, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			fnGr_PutString(211, 350, rgucBaseMatBatchNumber, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
		}break;
		case SCREEN_JOB_SETTING_2:
		{
			fnLCDbmp(uiCursorXPos, uiCursorYPos,30,26,ucrgArrow);
			
			/*Screen Title*/
			fnGr_PutString(9, 250, JOB_SETTING, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);

			/*Hardner batch Number*/
			fnGr_PutString(43, 380, HARDNER_BATCH_NUMBER, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			fnGr_PutString(76, 350, rgucHardMatBatchNumber, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			
			/*Job batch Number*/
			fnGr_PutString(109, 380, JOB_NAME, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			fnGr_PutString(142, 350, rgucJobName, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			
			/*change7*/
			/*JOB ID number*/
			fnGr_PutString(178, 380, JOINT_ID, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			fnGr_PutString(211, 350, rgucJobId, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
		}break;
		case SCREEN_STALL_SETTING:
		{
			fnLCDbmp(uiCursorXPos, uiCursorYPos,30,26,ucrgArrow);
			
			/*Screen Title*/
			fnGr_PutString(9, 250, STALL_SETTING, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			
			/*Stall Threshold*/
			fnGr_PutString(43, 380, STALL_THRESHOLD_LOW, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			fnGr_PutString(43, 130, NEGATIVE, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			fnGr_PutString(76, 230, "(high)", BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			
			/*downstroke*//*Negative Threshold*/
			sprintf((char *)rgcTemp,"%d",stConfig_Value.iConfigDownStrokeTH);
			fnGr_PutString(43, 165, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
									
			/*upstroke*//*positive Threshold*/
			sprintf((char *)rgcTemp,"%d",stConfig_Value.iConfigUpStrokeTH);
			fnGr_PutString(76, 165, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);

			/*4thMarch*/
			/*Upstroke Limit*/
			fnGr_PutString(109, 380, UPSTROKE_LIMIT_STR, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);						
			sprintf((char *)rgcTemp,"%0.1f",stConfig_Value.fUpstrokeLimit);
			fnGr_PutString(109, 165, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			/*Upstroke min*/
			fnGr_PutString(142, 380, UPSTROKE_MIN_STR, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);						
			sprintf((char *)rgcTemp,"%0.1f",stConfig_Value.fUpstrokeMin);
			fnGr_PutString(142, 165, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			/*Upstroke Max*/
			fnGr_PutString(178, 380, UPSTROKE_MAX_STR, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);						
			sprintf((char *)rgcTemp,"%0.1f",stConfig_Value.fUpstrokeMax);
			fnGr_PutString(178, 165, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			/*Downstroke limit*/
			fnGr_PutString(211, 380, DOWNSTROKE_LIMIT_STR, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);						
			sprintf((char *)rgcTemp,"%0.1f",stConfig_Value.fDownstrokeLimit);
			fnGr_PutString(211, 165, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			if ( METRIC == stConfig_Value.ucTypeofUnit )
			{
				fnGr_PutString(109, 85 , MILIMETER, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
				fnGr_PutString(142, 85 , MILIMETER, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
				fnGr_PutString(178, 85 , MILIMETER, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
				fnGr_PutString(211, 85 , MILIMETER, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			}
			else if( IMPERIAL == stConfig_Value.ucTypeofUnit )
			{
				fnGr_PutString(109, 85 , INCH, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
				fnGr_PutString(142, 85 , INCH, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
				fnGr_PutString(178, 85 , INCH, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
				fnGr_PutString(211, 85 , INCH, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			}
		}break;
		
		case SCREEN_STALL_SETTING_1:
		{
			/*4thMarch2016*/
			fnLCDbmp(uiCursorXPos, uiCursorYPos,30,26,ucrgArrow);
			
			/*Screen Title*/
			fnGr_PutString(9, 250, STALL_SETTING, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			
			/*Downstroke Min*/
			fnGr_PutString(43, 380, DOWNSTROKE_MIN_STR, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);						
			sprintf((char *)rgcTemp,"%0.1f",stConfig_Value.fDownstrokeMin);
			fnGr_PutString(43, 165, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			/*Downstroke max*/
			fnGr_PutString(76, 380, DOWNSTROKE_MAX_STR, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);						
			sprintf((char *)rgcTemp,"%0.1f",stConfig_Value.fDownstrokeMax);
			fnGr_PutString(76, 165, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			if ( METRIC == stConfig_Value.ucTypeofUnit )
			{
				fnGr_PutString(43, 85, MILIMETER, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
				fnGr_PutString(76, 85, MILIMETER, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			}
			else if( IMPERIAL == stConfig_Value.ucTypeofUnit )
			{
				fnGr_PutString(43, 85, INCH, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
				fnGr_PutString(76, 85, INCH, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			}
		}break;
		case SCREEN_PRESSURE_SETTING_1:
		{
			fnLCDbmp(uiCursorXPos, uiCursorYPos,30,26,ucrgArrow);
			
			/*Screen Title*/
			fnGr_PutString(9, 300, PRESSURE_SETTING, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			
			/*Unit*/
			if ( METRIC == stConfig_Value.ucTypeofUnit )
			{
				fnGr_PutString(9, 140, BAR, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			}
			else
			{
			fnGr_PutString(9, 140, PSI, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			}
			
			/*Gauge 1(low th)*/
			fnGr_PutString(43, 380, GAUGE1_LOW_TH, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);						
			sprintf((char *)rgcTemp,"%0.1f",stConfig_Value.fConfigPressureGauge1LTH);
			fnGr_PutString(43, 165, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			/*(high th)*/
			fnGr_PutString(76, 300, HIGH_TH_STRING, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);						
			sprintf((char *)rgcTemp,"%0.1f",stConfig_Value.fConfigPressureGauge1HTH);
			fnGr_PutString(76, 165, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			/*Gauge 2(low th)*/
			fnGr_PutString(109, 380, GAUGE2_LOW_TH, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);						
			sprintf((char *)rgcTemp,"%0.1f",stConfig_Value.fConfigPressureGauge2LTH);
			fnGr_PutString(109, 165, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			/*(high th)*/
			fnGr_PutString(142, 300, HIGH_TH_STRING, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);						
			sprintf((char *)rgcTemp,"%0.1f",stConfig_Value.fConfigPressureGauge2HTH);
			fnGr_PutString(142, 165, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			/*Gauge 3(low th)*/
			fnGr_PutString(178, 380, GAUGE3_LOW_TH, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);						
			sprintf((char *)rgcTemp,"%0.1f",stConfig_Value.fConfigPressureGauge3LTH);
			fnGr_PutString(178, 165, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			/*(high th)*/
			fnGr_PutString(211, 300, HIGH_TH_STRING, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);						
			sprintf((char *)rgcTemp,"%0.1f",stConfig_Value.fConfigPressureGauge3HTH);
			fnGr_PutString(211, 165, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
		}break;
		
		case SCREEN_PRESSURE_SETTING_2:
		{
			fnLCDbmp(uiCursorXPos, uiCursorYPos,30,26,ucrgArrow);
			
			/*Screen Title*/
			fnGr_PutString(9, 300, PRESSURE_SETTING, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			
			/*Unit*/
			if ( METRIC == stConfig_Value.ucTypeofUnit )
			{
				fnGr_PutString(9, 140, BAR, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			}
			else
			{
			fnGr_PutString(9, 140, PSI, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			}
			
			/*Spray*/
			fnGr_PutString(43, 380, SPRAY, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			fnGr_PutString(43, 230, MIN, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			fnGr_PutString(76, 230, SET_STRING, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			fnGr_PutString(109, 230, MAX, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
							
			sprintf((char *)rgcTemp,"%0.1f",stConfig_Value.fConfigSprayPressureLTH);
			fnGr_PutString(43, 165, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
							
			sprintf((char *)rgcTemp,"%0.1f",stConfig_Value.fConfigSprayPressure);
			fnGr_PutString(76, 165, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
								
			sprintf((char *)rgcTemp,"%0.1f",stConfig_Value.fConfigSprayPressureHTH);
			fnGr_PutString(109, 165, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			/*Recirculation*/
			fnGr_PutString(142, 380, RECIRCULATION, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			fnGr_PutString(142, 230, MIN, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			fnGr_PutString(178, 230, SET_STRING, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			fnGr_PutString(211, 230, MAX, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
															
			sprintf((char *)rgcTemp,"%0.1f",stConfig_Value.fConfigRecirPressureLTH);
			fnGr_PutString(142, 165, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			sprintf((char *)rgcTemp,"%0.1f",stConfig_Value.fConfigRecirPressure);
			fnGr_PutString(178, 165, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			sprintf((char *)rgcTemp,"%0.1f",stConfig_Value.fConfigRecirPressureHTH);
			fnGr_PutString(211, 165, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
		}break;
		
		case SCREEN_PARAMETER_SET_2:
		{
			fnGr_PutString(9, 280, PARAMETER_SETTING, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(60, 380, JOB_SETTING, RGB(0,0,0), 
														LIGHT_GREY_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(60, 185, STALL_SETTING, RGB(0,0,0), 
														LIGHT_GREY_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(125, 380, PRESSURE_SETTING, RGB(0,0,0), 
														LIGHT_GREY_CLR, ORIENTATION_90,0);
		}break;
		
		case SCREEN_STATUS:
		{
			fnGr_PutString(9, 230, STATUS, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(60, 380, MATERIAL_USAGE, RGB(0,0,0), 
														LIGHT_GREY_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(60, 185, VALVE_STATUS, RGB(0,0,0), 
														LIGHT_GREY_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(125, 380, CYCLE_STATUS, RGB(0,0,0), 
														LIGHT_GREY_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(115, 185, DIAGNOSTIC, RGB(0,0,0), 
														LIGHT_GREY_CLR, ORIENTATION_90,0);
			fnGr_PutString(140, 185, STATUS, RGB(0,0,0), 
														LIGHT_GREY_CLR, ORIENTATION_90,0);
		}break;
		
		case SCREEN_MATERIAL_USAGE:
		{
			fnGr_PutString(9, 280, MATERIAL_USAGE, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			/*change6*/
			if( METRIC == stConfig_Value.ucTypeofUnit )
					{
			fnGr_PutString(9, 130, LITRE, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
					}
						else
						{
						fnGr_PutString(9, 130, OZ, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
						}							
			/*Hardner*/
			fnGr_PutString(43, 240, HARDNER, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			/*Base*/
			fnGr_PutString(43, 140, BASE, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			
			/*Last/Current Joint*/
						/*change23*/
//						if( METRIC == stConfig_Value.ucTypeofUnit )
//					{
//					fnGr_PutString(76, 380, CURRENT_JOINT(ml), BLACK_CLR, 
//												WHITE_CLR, ORIENTATION_90,0);
//					}
				//	else
					{fnGr_PutString(76, 380, CURENT_JOINT(oz), BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
					}
				
						
				/*hardner*/
				/*fTemp = ((stCurrent_Value.fLegAMaterialUsed + stCurrent_Value.fLegCMaterialUsed)/1000000);*/
				/*change6*/
//			if( IMPERIAL == stConfig_Value.ucTypeofUnit )
//					{      SplitByte.fFloat =stCurrent_Value.fHardMatTotalJoint;
//						SplitByte.fFloat = fnMetricToImperial_Conv(SplitByte.fFloat, ML_TO_OZ);
//						stCurrent_Value.fHardMatTotalJoint = SplitByte.fFloat;
//						
//SplitByte.fFloat =stCurrent_Value.fBaseMatTotalJoint;
//						SplitByte.fFloat = fnMetricToImperial_Conv(SplitByte.fFloat, ML_TO_OZ);
//						stCurrent_Value.fBaseMatTotalJoint = SplitByte.fFloat;
//SplitByte.fFloat =stCurrent_Value.fHardMatPrimeAcc;
//						SplitByte.fFloat = fnMetricToImperial_Conv(SplitByte.fFloat, ML_TO_OZ);
//						stCurrent_Value.fHardMatPrimeAcc = SplitByte.fFloat;
//SplitByte.fFloat =stCurrent_Value.fBaseMatPrimeAcc;
//						SplitByte.fFloat = fnMetricToImperial_Conv(SplitByte.fFloat, ML_TO_OZ);
//						stCurrent_Value.fBaseMatPrimeAcc = SplitByte.fFloat;
//SplitByte.fFloat =stCurrent_Value.fHardMatSprayAcc;
//						SplitByte.fFloat = fnMetricToImperial_Conv(SplitByte.fFloat, ML_TO_OZ);
//						stCurrent_Value.fHardMatSprayAcc = SplitByte.fFloat;
//SplitByte.fFloat =stCurrent_Value.fBaseMatSprayAcc;
//						SplitByte.fFloat = fnMetricToImperial_Conv(SplitByte.fFloat, ML_TO_OZ);
//						stCurrent_Value.fHardMatTotalJoint = SplitByte.fFloat;
//SplitByte.fFloat =stCurrent_Value.fHardMatTotalAcc;
//						SplitByte.fFloat = fnMetricToImperial_Conv(SplitByte.fFloat, ML_TO_OZ);
//						stCurrent_Value.fHardMatTotalAcc = SplitByte.fFloat;
//SplitByte.fFloat =stCurrent_Value.fBaseMatTotalAcc;
//						SplitByte.fFloat = fnMetricToImperial_Conv(SplitByte.fFloat, ML_TO_OZ);
//						stCurrent_Value.fBaseMatTotalAcc = SplitByte.fFloat;	

//					}
//					else/*if metric then in litres*/
//					{
//						
//  
//SplitByte.fFloat =stCurrent_Value.fHardMatPrimeAcc;
//						SplitByte.fFloat = (float)(CONVERT_ML_TO_L(SplitByte.fFloat));
//						stCurrent_Value.fHardMatPrimeAcc = SplitByte.fFloat;
//SplitByte.fFloat =stCurrent_Value.fBaseMatPrimeAcc;
//						SplitByte.fFloat = (float)(CONVERT_ML_TO_L(SplitByte.fFloat));
//						stCurrent_Value.fBaseMatPrimeAcc = SplitByte.fFloat;
//SplitByte.fFloat =stCurrent_Value.fHardMatSprayAcc;
//						SplitByte.fFloat = (float)(CONVERT_ML_TO_L(SplitByte.fFloat));
//						stCurrent_Value.fHardMatSprayAcc = SplitByte.fFloat;
//SplitByte.fFloat =stCurrent_Value.fBaseMatSprayAcc;
//						SplitByte.fFloat = (float)(CONVERT_ML_TO_L(SplitByte.fFloat));
//						stCurrent_Value.fHardMatTotalJoint = SplitByte.fFloat;
//SplitByte.fFloat =stCurrent_Value.fHardMatTotalAcc;
//						SplitByte.fFloat = (float)(CONVERT_ML_TO_L(SplitByte.fFloat));
//						stCurrent_Value.fHardMatTotalAcc = SplitByte.fFloat;
//SplitByte.fFloat =stCurrent_Value.fBaseMatTotalAcc;
//						SplitByte.fFloat = (float)(CONVERT_ML_TO_L(SplitByte.fFloat));
//						stCurrent_Value.fBaseMatTotalAcc = SplitByte.fFloat;	


//					}
	
						/***************Hardener*********************************/				
						sprintf((char *)rgcTemp,"%0.2f",stCurrent_Value.fHardMatTotalJoint );
						fnGr_PutString(76, 240, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);

					
											/*base*/
				sprintf((char *)rgcTemp,"%0.2f",stCurrent_Value.fBaseMatTotalJoint);
				fnGr_PutString(76, 150, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);

					
						/*Primed*/
			fnGr_PutString(109, 380, PRIMED, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
				/*hardner*/
				sprintf((char *)rgcTemp,"%0.2f",stCurrent_Value.fHardMatPrimeAcc );
				fnGr_PutString(109, 240, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
				
				
					/*Base*/
				sprintf((char *)rgcTemp,"%0.2f",stCurrent_Value.fBaseMatPrimeAcc );
				fnGr_PutString(109, 150, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
					
				
							
			/*Applied*/
			fnGr_PutString(142, 380, APPLIED, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
					/*hardner*/
					sprintf((char *)rgcTemp,"%0.2f",stCurrent_Value.fHardMatSprayAcc );
					fnGr_PutString(142, 240, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
	
						
								/*Base*/
					sprintf((char *)rgcTemp,"%0.2f",stCurrent_Value.fBaseMatSprayAcc );
					fnGr_PutString(142, 150, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
	
					
						/*Total*/
			fnGr_PutString(178, 380, TOTAL_ACC, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
					/*hardner*/
					sprintf((char *)rgcTemp,"%0.2f",stCurrent_Value.fHardMatTotalAcc );
					fnGr_PutString(178, 240, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);

	
						
						/*Base*/
					sprintf((char *)rgcTemp,"%0.2f",stCurrent_Value.fBaseMatTotalAcc );
					fnGr_PutString(178, 150, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
	
						
				
					
								}break;
		
		case SCREEN_CYCLE_STATUS:
		{
			/*Screen title*/
			fnGr_PutString(9, 240, CYCLE_STATUS, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
							
			/*Ready*/
			fnGr_PutString(43, 225, READY, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			
			/*Prime*/
			fnGr_PutString(76, 380, PRIME_STRING, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			if ( TRUE == StatusFlag.stStatusFlag.bOperationModeStart )
			{
				fnGr_PutString(76, 225, YES, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
				fnGr_PutString(109, 225, YES, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
				fnGr_PutString(142, 225, YES, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			}
			else
			{
				fnGr_PutString(76, 225, NO, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
				fnGr_PutString(109, 225, NO, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
				fnGr_PutString(142, 225, NO, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			}
			
			if (( TRUE == StatusFlag.stStatusFlag.bInitPrimeProcess ) && 
					(FALSE == StatusFlag.stStatusFlag.bPrimeDone ))
			{
				fnGr_PutString(76, 225, PROGRESS, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			}
			else if ( TRUE == StatusFlag.stStatusFlag.bPrimeDone )
			{
				fnGr_PutString(76, 225, DONE, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			}
			
			/*Spray*/
			fnGr_PutString(109, 380, SPRAY_STRING, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			if (( TRUE == StatusFlag.stStatusFlag.bInitSprayProcess ) && 
					( FALSE == StatusFlag.stStatusFlag.bSprayDone ))
			{
				fnGr_PutString(109, 225, PROGRESS, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			}
			else if( TRUE == StatusFlag.stStatusFlag.bSprayDone )
			{
				fnGr_PutString(109, 225, DONE, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			}
			
			/*Flush*/
			fnGr_PutString(142, 380, FLUSH_STRING, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			if (( TRUE == StatusFlag.stStatusFlag.bInitFlushProcess ) && 
					( FALSE == StatusFlag.stStatusFlag.bFlushDone ))
			{
				fnGr_PutString(142, 225, PROGRESS, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			}
			else if ( TRUE == StatusFlag.stStatusFlag.bFlushDone )
			{
				fnGr_PutString(142, 225, DONE, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			}
		}break;
		
		case SCREEN_VALVE_STATUS_1:
		{
			/*Screen title*/
			fnGr_PutString(9, 260, VALVE_STATUS, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			
			/*Head to Prime/Spray*/
			fnGr_PutString(43, 380, HEAD_TO_PRIME_SPRAY, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			if ( TRUE == StatusFlag.stStatusFlag.OPMM_VL_SprayHead_status )
				{
					fnGr_PutString(43, 145, ON, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
				}
				else
				{
					fnGr_PutString(43, 145, OFF, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
				}
			/*valve A/B*/
			fnGr_PutString(76, 380, A_B_VALVE, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			if ( TRUE == StatusFlag.stStatusFlag.OPMM_VL_AB_status )
				{
					fnGr_PutString(76, 145, ON, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
				}
				else
				{
					fnGr_PutString(76, 145, OFF, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
				}
				
			/*Forward valve*/
			fnGr_PutString(109, 380, FORWARD_VALVE, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			if ( TRUE == StatusFlag.stStatusFlag.OPMM_VL_Forward_status )
				{
					fnGr_PutString(109, 145, ON, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
				}
				else
				{
					fnGr_PutString(109, 145, OFF, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
				}
				
			/*Backward Valve*/
			fnGr_PutString(142, 380, BACKWARD_VALVE, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			if ( TRUE == StatusFlag.stStatusFlag.OPMM_VL_Backward_status )
				{
					fnGr_PutString(142, 145, ON, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
				}
				else
				{
					fnGr_PutString(142, 145, OFF, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
				}
				
			/*Spray Valve*/
			fnGr_PutString(178, 380, SPRAY_VALVE, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			if ( TRUE == StatusFlag.stStatusFlag.OPMM_VL_SprayGun_status )
				{
					fnGr_PutString(178, 145, ON, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
				}
				else
				{
					fnGr_PutString(178, 145, OFF, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
				}
				
			/*Solvent Valve*/
			fnGr_PutString(211, 380, SOLVENT_VALVE, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			if ( TRUE == StatusFlag.stStatusFlag.OPMM_VL_Flush_status )
				{
					fnGr_PutString(211, 145, ON, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
				}
				else
				{
					fnGr_PutString(211, 145, OFF, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
				}
		}break;
		
		case SCREEN_VALVE_STATUS_2:
		{
			/*Screen title*/
			fnGr_PutString(9, 260, VALVE_STATUS, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
		
			/*Stall Valve*/
			fnGr_PutString(43, 380, STALL_VALVE, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			if ( TRUE == StatusFlag.stStatusFlag.VCM_VL_Stall )
				{
					fnGr_PutString(43, 145, ON, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
				}
				else
				{
					fnGr_PutString(43, 145, OFF, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
				}
		
				/*Recirc Valve change13*/
				fnGr_PutString(76, 380, RECIRC_VALVE, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			if ( TRUE == StatusFlag.stStatusFlag.VCM_VL_Recirc )
				{
					fnGr_PutString(76, 145, ON, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
				}
				else
				{
					fnGr_PutString(76, 145, OFF, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
				}
		}break;
		case SCREEN_CURRENT_PARA:
		{
			fnGr_PutString(9, 250, PARAMETERS, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(60, 380, TEMPERATURE, RGB(0,0,0), 
														LIGHT_GREY_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(60, 185, PRESSURE, RGB(0,0,0), 
														LIGHT_GREY_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(125, 380, PROCESS_TIME, RGB(0,0,0), 
														LIGHT_GREY_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(125, 185, OTHERS, RGB(0,0,0), 
														LIGHT_GREY_CLR, ORIENTATION_90,0);
		}break;
		case SCREEN_ADVANCED:
		{
			fnGr_PutString(9, 250, ADVANCED, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(60, 380, AUTOCOAT_STATUS, RGB(0,0,0), 
														LIGHT_GREY_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(60, 185, RESET_STRING, RGB(0,0,0), 
														LIGHT_GREY_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(115, 380, PERIPHERALS , RGB(0,0,0), 
														LIGHT_GREY_CLR, ORIENTATION_90,0);
			fnGr_PutString(140, 380,  STATUS, RGB(0,0,0), 
														LIGHT_GREY_CLR, ORIENTATION_90,0);
			
			
			fnGr_PutString(125, 185, REMOTE, RGB(0,0,0), 
														LIGHT_GREY_CLR, ORIENTATION_90,0);
		}break;
		
		case SCREEN_ADVANCED_AUTOCOAT:
		{
			fnGr_PutString(9, 260, AUTOCOAT_STATUS, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			
			/*Passes required*/
			fnGr_PutString(43, 380, PASSES_REQUIRED, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			sprintf((char *)rgcTemp,"%d",stConfig_Value.uiConfigSprayCount);
			fnGr_PutString(43, 190, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			/*Passes completed*/
			fnGr_PutString(76, 380, PASSES_COMPLETED, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			sprintf((char *)rgcTemp,"%d",stCurrent_Value.ucCurrNumberofRotation);
			fnGr_PutString(76, 190, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			/*Gun timer delay*/
			fnGr_PutString(109, 380, GUN_TIMER_DELAY, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			sprintf((char *)rgcTemp,"%0.0f",stConfig_Value.fConfigGunDelayTime);
			fnGr_PutString(109, 190, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			/*over run delay or spray overlap delay*/
			fnGr_PutString(142, 380, OVER_RUN_DELAY, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			sprintf((char *)rgcTemp,"%0.0f",stConfig_Value.fConfigSprayOverlapTime);
			fnGr_PutString(142, 190, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			/*min prime/purge time*/
			fnGr_PutString(178, 380, MINIMUM_PURGE_TIME, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			sprintf((char *)rgcTemp,"%0.1f",stConfig_Value.fConfigPrimeTime);
			fnGr_PutString(178, 185, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			/*min flush time*/
			fnGr_PutString(211, 380, MINIMUM_FLUSH_TIME, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			sprintf((char *)rgcTemp,"%0.1f",stConfig_Value.fConfigFlushTime);
			fnGr_PutString(211, 185, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			
			fnGr_PutString(109, 110, MS_WB, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			fnGr_PutString(142, 110, MS_WB, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			fnGr_PutString(178, 110, SEC_WB, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			fnGr_PutString(211, 110, SEC_WB, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
		}break;
		case SCREEN_MANAGE_PERIPHERAL_1:
		{
			fnGr_PutString(9, 280, MANAGE_PERIPHERAL, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			
			/*Head to Prime/Spray*/
			
			if ( TRUE == StatusFlag.stStatusFlag.OPMM_VL_SprayHead_status )
			{
				fnGr_PutString(50, 380, HEAD_TO_PRIME, GREEN_CLR, WHITE_CLR, ORIENTATION_90,0);
				fnGr_PutString(75, 380, SPRAY, GREEN_CLR, WHITE_CLR, ORIENTATION_90,0);
			}
			else
			{
				fnGr_PutString(50, 380, HEAD_TO_PRIME, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
				fnGr_PutString(75, 380, SPRAY, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			}
				
			/*valve A/B*/
			if ( TRUE == StatusFlag.stStatusFlag.OPMM_VL_AB_status )
			{
				fnGr_PutString(60, 185, A_B_VALVE, GREEN_CLR, WHITE_CLR, ORIENTATION_90,0);
			}
			else
			{
				fnGr_PutString(60, 185, A_B_VALVE, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			}
				
			/*Forward valve*/
			if ( TRUE == StatusFlag.stStatusFlag.OPMM_VL_Forward_status )
			{
				fnGr_PutString(125, 380, FORWARD_VALVE, GREEN_CLR, WHITE_CLR, ORIENTATION_90,0);
			}
			else
			{
				fnGr_PutString(125, 380, FORWARD_VALVE, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			}
				
			/*Backward Valve*/
			if ( TRUE == StatusFlag.stStatusFlag.OPMM_VL_Backward_status )
			{
				fnGr_PutString(125, 185, BACKWARD_VALVE, GREEN_CLR, WHITE_CLR, ORIENTATION_90,0);
			}
			else
			{
				fnGr_PutString(125, 185, BACKWARD_VALVE, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			}
				
			/*Spray Valve*/
			if ( TRUE == StatusFlag.stStatusFlag.OPMM_VL_SprayGun_status )
			{
				fnGr_PutString(190, 380, SPRAY_VALVE, GREEN_CLR, WHITE_CLR, ORIENTATION_90,0);
			}
			else
			{
				fnGr_PutString(190, 380, SPRAY_VALVE, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			}
				
			/*Solvent Valve*/
			if ( TRUE == StatusFlag.stStatusFlag.OPMM_VL_Flush_status )
			{
				fnGr_PutString(190, 185, SOLVENT_VALVE, GREEN_CLR, WHITE_CLR, ORIENTATION_90,0);
			}
			else
			{
				fnGr_PutString(190, 185, SOLVENT_VALVE, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			}
			
		}break;
		
		case SCREEN_MANAGE_PERIPHERAL_2:
		{
			fnGr_PutString(9, 280, MANAGE_PERIPHERAL, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			
			/*Stall Valve*/
			if ( TRUE == StatusFlag.stStatusFlag.VCM_VL_Stall )
			{
				fnGr_PutString(60, 380, STALL_VALVE, GREEN_CLR, WHITE_CLR, ORIENTATION_90,0);
			}
			else
			{
				fnGr_PutString(60, 380, STALL_VALVE, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			}
			
			/*Laser Pointer*/
			if ( TRUE == StatusFlag.stStatusFlag.bLaserBeamStatus )
			{
				fnGr_PutString(60, 185, LASER_POINTER, GREEN_CLR, WHITE_CLR, ORIENTATION_90,0);
			}
			else
			{
				fnGr_PutString(60, 185, LASER_POINTER, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			}
			
			/*Base Heater*/
			if ( TRUE == StatusFlag.stStatusFlag.bHCM_Antfrz_Heater_Status )
			{
				fnGr_PutString(125, 380, BASE_HEATER, GREEN_CLR, WHITE_CLR, ORIENTATION_90,0);
			}
			else
			{
				fnGr_PutString(125, 380, BASE_HEATER, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			}
				
			/*Base Inl Heater*/
			if ( TRUE == StatusFlag.stStatusFlag.bHCM_Base_Heater_Status )
			{
				fnGr_PutString(125, 185, BASE_INL_HEATER, GREEN_CLR, WHITE_CLR, ORIENTATION_90,0);
			}
			else
			{
				fnGr_PutString(125, 185, BASE_INL_HEATER, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			}
				
			/*Hard Inl Heater*/
			if ( TRUE == StatusFlag.stStatusFlag.bHCM_Hard_Heater_Status )
			{
				fnGr_PutString(190, 380, HARD_INL_HEATER, GREEN_CLR, WHITE_CLR, ORIENTATION_90,0);
			}
			else
			{
				fnGr_PutString(190, 380, HARD_INL_HEATER, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			}
				
			/*Hard Tank Heater*/
			if ( TRUE == StatusFlag.stStatusFlag.bHCM_Hard_Tank_Heater_Status )
			{
				fnGr_PutString(190, 185, HARDNER_HEATER, GREEN_CLR, WHITE_CLR, ORIENTATION_90,0);
			}
			else
			{
				fnGr_PutString(190, 185, HARDNER_HEATER, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			}	
		}break;
		/*change13*/
		case SCREEN_MANAGE_PERIPHERAL_3:
		{
			fnGr_PutString(9, 280, MANAGE_PERIPHERAL, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			
			/*Recirc Valve*/
			if ( TRUE == StatusFlag.stStatusFlag.VCM_VL_Recirc )
			{
				fnGr_PutString(60, 380, RECIRC_VALVE, GREEN_CLR, WHITE_CLR, ORIENTATION_90,0);
			}
			else
			{
				fnGr_PutString(60, 380, RECIRC_VALVE, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			}
			
		}break;
	
		case SCREEN_ADVANCED_RESET:
		{
			//fnLCDbmp(uiCursorXPos, uiCursorYPos,30,26,ucrgArrow);
			
			fnGr_PutString(9, 235, RESET_STRING, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			
			/*Joints*/
			fnGr_PutString(43, 380, JOINTS, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			
			/*Base Volume*/
			fnGr_PutString(76, 380, BASE_VOLUME, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			
			/*Hardner Volume*/
			fnGr_PutString(109, 380, HARD_VOLUME, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			
			fnGr_PutString(211, 33, SET_BUTTON, WHITE_CLR , 
			DARK_GREY_CLR, ORIENTATION_90,0);
		}break;
		case SCREEN_PARAM_TEMP_1:
		{
			fnGr_PutString(9, 260, TEMPERATURE, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			
			/*Unit*/
			if ( METRIC == stConfig_Value.ucTypeofUnit )
			{
				fnGr_PutString(9, 140, ALPHA_C_BRACKET, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			}
			else
			{
			fnGr_PutString(9, 140, ALPHA_F_BRACKET, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			}
					
			/*Return Line*/
			fnGr_PutString(43, 380, RETURN_LINE, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			if ( INVALID_DATA == stCurrent_Value.fCurrReturnLineTemp )
			{
				fnGr_PutString(43, 160, ERR_STRING, RED_CLR, WHITE_CLR, ORIENTATION_90,0);
			}
			else
			{
				sprintf((char *)rgcTemp,"%0.1f",stCurrent_Value.fCurrReturnLineTemp);
				fnGr_PutString(43, 175, rgcTemp, BLACK_CLR, WHITE_CLR, ORIENTATION_90,1);
			}
			
			/*Ambient in container*/
			fnGr_PutString(76, 380, "Ambient In Container" , BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			if ( INVALID_DATA == stCurrent_Value.fCurrInsideAmbientTemp )
			{
				fnGr_PutString(76, 160, ERR_STRING, RED_CLR, WHITE_CLR, ORIENTATION_90,0);
			}
			else
			{
				sprintf((char *)rgcTemp,"%0.1f",stCurrent_Value.fCurrInsideAmbientTemp);
				fnGr_PutString(76, 175, rgcTemp, BLACK_CLR, WHITE_CLR, ORIENTATION_90,1);
			}
			
			/*ambient out container*/
			fnGr_PutString(109, 380, "Ambient Out Container" , BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			if ( INVALID_DATA == stCurrent_Value.fCurrOutsideAmbientTemp )
			{
				fnGr_PutString(109, 160, ERR_STRING, RED_CLR, WHITE_CLR, ORIENTATION_90,0);
			}
			else
			{
				sprintf((char *)rgcTemp,"%0.1f",stCurrent_Value.fCurrOutsideAmbientTemp);
				fnGr_PutString(109, 175, rgcTemp, BLACK_CLR, WHITE_CLR, ORIENTATION_90,1);
			}
			
			/*antifreez liquid*/
			fnGr_PutString(142, 380, ANTIFREEZ_LIQUID , BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			if ( INVALID_DATA == stCurrent_Value.fCurrAntifreezeLiquidTemp )
			{
				fnGr_PutString(142, 160, ERR_STRING, RED_CLR, WHITE_CLR, ORIENTATION_90,0);
			}
			else
			{
				sprintf((char *)rgcTemp,"%0.1f",stCurrent_Value.fCurrAntifreezeLiquidTemp);
				fnGr_PutString(142, 175, rgcTemp, BLACK_CLR, WHITE_CLR, ORIENTATION_90,1);
			}
			
			/*base material*/
			fnGr_PutString(178, 380, "Base Material" , BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			if ( INVALID_DATA == stCurrent_Value.fCurrInlineBaseMatTemp )
			{
				fnGr_PutString(178, 160, ERR_STRING, RED_CLR, WHITE_CLR, ORIENTATION_90,0);
			}
			else
			{
				sprintf((char *)rgcTemp,"%0.1f",stCurrent_Value.fCurrInlineBaseMatTemp);
				fnGr_PutString(178, 175, rgcTemp, BLACK_CLR, WHITE_CLR, ORIENTATION_90,1);
			}
			
			/*hardner material*/
			fnGr_PutString(211, 380, "Hardner Material" , BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			if ( INVALID_DATA == stCurrent_Value.fCurrInlineHardMatTemp )
			{
				fnGr_PutString(211, 160, ERR_STRING, RED_CLR, WHITE_CLR, ORIENTATION_90,0);
			}
			else
			{
				sprintf((char *)rgcTemp,"%0.1f",stCurrent_Value.fCurrInlineHardMatTemp);
				fnGr_PutString(211, 175, rgcTemp, BLACK_CLR, WHITE_CLR, ORIENTATION_90,1);
			}
			
		}break;
		
		case SCREEN_PARAM_TEMP_2:
		{
			fnGr_PutString(9, 260, TEMPERATURE, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			
			/*Unit*/
			if ( METRIC == stConfig_Value.ucTypeofUnit )
			{
				fnGr_PutString(9, 140, ALPHA_C_BRACKET, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			}
			else
			{
			fnGr_PutString(9, 140, ALPHA_F_BRACKET, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			}
					
			/*pipe position 12*/
			fnGr_PutString(43, 380, PIPE_POSITION12, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			if ( INVALID_DATA == stCurrent_Value.fCurrPipeTempPos12 )
			{
				fnGr_PutString(43, 160, ERR_STRING, RED_CLR, WHITE_CLR, ORIENTATION_90,0);
			}
			else
			{
				sprintf((char *)rgcTemp,"%0.1f",stCurrent_Value.fCurrPipeTempPos12);
				fnGr_PutString(43, 175, rgcTemp, BLACK_CLR, WHITE_CLR, ORIENTATION_90,1);
			}
			
			/*pipe position 3*/
			fnGr_PutString(76, 380, PIPE_POSITION3 , BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			if ( INVALID_DATA == stCurrent_Value.fCurrPipeTempPos3 )
			{
				fnGr_PutString(76, 160, ERR_STRING, RED_CLR, WHITE_CLR, ORIENTATION_90,0);
			}
			else
			{
				sprintf((char *)rgcTemp,"%0.1f",stCurrent_Value.fCurrPipeTempPos3);
				fnGr_PutString(76, 175, rgcTemp, BLACK_CLR, WHITE_CLR, ORIENTATION_90,1);
			}
			
			/*pipe position 6*/
			fnGr_PutString(109, 380, PIPE_POSITION6 , BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			if ( INVALID_DATA == stCurrent_Value.fCurrPipeTempPos6 )
			{
				fnGr_PutString(109, 160, ERR_STRING, RED_CLR, WHITE_CLR, ORIENTATION_90,0);
			}
			else
			{
				sprintf((char *)rgcTemp,"%0.1f",stCurrent_Value.fCurrPipeTempPos6);
				fnGr_PutString(109, 175, rgcTemp, BLACK_CLR, WHITE_CLR, ORIENTATION_90,1);
			}
			
			/*pipe position 9*/
			fnGr_PutString(142, 380, PIPE_POSITION9 , BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			if ( INVALID_DATA == stCurrent_Value.fCurrPipeTempPos9 )
			{
				fnGr_PutString(142, 160, ERR_STRING, RED_CLR, WHITE_CLR, ORIENTATION_90,0);
			}
			else
			{
				sprintf((char *)rgcTemp,"%0.1f",stCurrent_Value.fCurrPipeTempPos9);
				fnGr_PutString(142, 175, rgcTemp, BLACK_CLR, WHITE_CLR, ORIENTATION_90,1);
			}
			
			/*average pipe temperature*/
			fnGr_PutString(178, 380, AVERAGE_PIPE_TEMP , BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			if ( INVALID_DATA == stCurrent_Value.fCurrPipeTempAvg )
			{
				fnGr_PutString(178, 160, ERR_STRING, RED_CLR, WHITE_CLR, ORIENTATION_90,0);
			}
			else
			{
				sprintf((char *)rgcTemp,"%0.1f",stCurrent_Value.fCurrPipeTempAvg);
				fnGr_PutString(178, 175, rgcTemp, BLACK_CLR, WHITE_CLR, ORIENTATION_90,1);
			}
			
			/*OPMM Base*/
			fnGr_PutString(211, 380, "OPMM Base" , BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			if ( INVALID_DATA == stCurrent_Value.fCurrOPMMBaseTemp )
			{
				fnGr_PutString(211, 160, ERR_STRING, RED_CLR, WHITE_CLR, ORIENTATION_90,0);
			}
			else
			{
				sprintf((char *)rgcTemp,"%0.1f",stCurrent_Value.fCurrOPMMBaseTemp);
				fnGr_PutString(211, 175, rgcTemp, BLACK_CLR, WHITE_CLR, ORIENTATION_90,1);
			}
			
		}break;
		
		case SCREEN_PARAM_TEMP_3:
		{
			fnGr_PutString(9, 260, TEMPERATURE, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			
			/*Unit*/
			if ( METRIC == stConfig_Value.ucTypeofUnit )
			{
				fnGr_PutString(9, 140, ALPHA_C_BRACKET, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			}
			else
			{
			fnGr_PutString(9, 140, ALPHA_F_BRACKET, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			}
					
			/*OPMM HArd*/
			fnGr_PutString(43, 380, "OPMM Hardner", BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			if (  INVALID_DATA == stCurrent_Value.fCurrOPMMHardTemp )
			{
				fnGr_PutString(43, 160, ERR_STRING, RED_CLR, WHITE_CLR, ORIENTATION_90,0);
			}
			else
			{
				sprintf((char *)rgcTemp,"%0.1f",stCurrent_Value.fCurrOPMMHardTemp);
				fnGr_PutString(43, 175, rgcTemp, BLACK_CLR, WHITE_CLR, ORIENTATION_90,1);
			}
		}break;
		
		case SCREEN_PARAM_PRESSURE:
		{
			fnGr_PutString(9, 250, PRESSURE, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			
			/*Unit*/
			if ( METRIC == stConfig_Value.ucTypeofUnit )
			{
				fnGr_PutString(9, 160, BAR, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			}
			else
			{
			fnGr_PutString(9, 160, PSI, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			}
			
			/*LEG 1*/
				fnGr_PutString(43, 380, LEG_1, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
				sprintf((char *)rgcTemp,"%0.1f",stCurrent_Value.fCurrPressureGauge1);
//				if ( 2 <= strlen((const char *)rgcTemp) )
//				{
//					strcat((char *)rgcTemp,"   ");
//				}
				if ( INVALID_DATA == stCurrent_Value.fCurrPressureGauge1 )
				{
					fnGr_PutString(43, 190, ERR_STRING, RED_CLR, WHITE_CLR, ORIENTATION_90,0);
				}
				else
				{
					fnGr_PutString(43, 205, rgcTemp, BLACK_CLR, WHITE_CLR, ORIENTATION_90,1);
				}
				
				/*LEG 2*/
				fnGr_PutString(76, 380, LEG_2, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
				sprintf((char *)rgcTemp,"%0.1f",stCurrent_Value.fCurrPressureGauge2);
//				if ( 2 <= strlen((const char *)rgcTemp) )
//				{
//					strcat((char *)rgcTemp,"   ");
//				}
				if ( INVALID_DATA == stCurrent_Value.fCurrPressureGauge2 )
				{
					fnGr_PutString(76, 190, ERR_STRING, RED_CLR, WHITE_CLR, ORIENTATION_90,0);
				}
				else
				{
					fnGr_PutString(76, 205, rgcTemp, BLACK_CLR, WHITE_CLR, ORIENTATION_90,1);
				}
				
	/*LEG 3*/
				fnGr_PutString(109, 380, LEG_3, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
				sprintf((char *)rgcTemp,"%0.1f",stCurrent_Value.fCurrPressureGauge3);
//				if ( 2<= strlen((const char *)rgcTemp) )
//				{
//					strcat((char *)rgcTemp,"   ");
//				}
				if ( INVALID_DATA == stCurrent_Value.fCurrPressureGauge3 )
				{
					fnGr_PutString(109, 190, ERR_STRING, RED_CLR, WHITE_CLR, ORIENTATION_90,0);
				}
				else
				{
					fnGr_PutString(109, 205, rgcTemp, BLACK_CLR, WHITE_CLR, ORIENTATION_90,1);			
				}
				
		/*Air Sensor*/
				fnGr_PutString(142, 380, AIR_SENSOR, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
				if( TRUE == StatusFlag.stStatusFlag.bVCM_AirSensor_Status )
				{
					fnGr_PutString(142, 190, CONNECTED, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
				}
				else
				{
					fnGr_PutString(142, 190, DISCONNECTED, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
				}
		}break;
		
		case SCREEN_PARAM_TIME:
		{
			fnGr_PutString(9, 300, PROCESS_TIME, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
					
			/*Unit*/
			fnGr_PutString(9, 165, SEC, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			
			/*Prime Process*/
			fnGr_PutString(43, 380, PRIME_PROCESS_PARAM, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			sprintf((char *)rgcTemp,"%0.1f",stCurrent_Value.fCurrPrimeTime);
			fnGr_PutString(43, 175, rgcTemp, BLACK_CLR, WHITE_CLR, ORIENTATION_90,1);
			
			/*Spray Process*/
			fnGr_PutString(76, 380, SPRAY_PROCESS_PARAM, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			sprintf((char *)rgcTemp,"%0.1f",stCurrent_Value.fCurrSprayTime);
			fnGr_PutString(76, 175, rgcTemp, BLACK_CLR, WHITE_CLR, ORIENTATION_90,1);
			
			/*Flush Process*/
			fnGr_PutString(109, 380, FLUSH_PROCESS_PARAM, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			sprintf((char *)rgcTemp,"%0.1f",stCurrent_Value.fCurrFlushTime);
			fnGr_PutString(109, 175, rgcTemp, BLACK_CLR, WHITE_CLR, ORIENTATION_90,1);
			
		}break;
		
		case SCREEN_PARAM_OTHERS:
		{
			fnGr_PutString(9, 240, OTHERS, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			
			/*Dew Point*/
			fnGr_PutString(43, 380, DEW_POINT, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			if ( INVALID_DATA == stCurrent_Value.fCurrDewPoint )
			{
				fnGr_PutString(43, 165, ERR_STRING, RGB(255,0,0), RGB(255,255,255), ORIENTATION_90,0);
			}
			else
			{
				fnGr_PutString(43, 100, ALPHA_C_BRACKET, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
				sprintf((char *)rgcTemp,"%0.1f",stCurrent_Value.fCurrDewPoint);
				fnGr_PutString(43, 185, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			}
			
			/*Humidity*/
			fnGr_PutString(76, 380, HUMIDITY, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			sprintf((char *)rgcTemp,"%0.1f",stCurrent_Value.fCurrOutsideAmbientHumidity);
			if ( INVALID_DATA == stCurrent_Value.fCurrOutsideAmbientHumidity )
			{
				fnGr_PutString(76, 165, ERR_STRING, RGB(255,0,0), RGB(255,255,255), ORIENTATION_90,0);
			}
			else
			{
				fnGr_PutString(76, 100, PERCENT, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
				fnGr_PutString(76, 185, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			}
			
			/*Base Material Liquid level*/
			fnGr_PutString(109, 380, BASE_MATERIAL_LIQUID_LEVEL, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			if ( INVALID_DATA == stCurrent_Value.fCurrLiquidLevelBase )
			{
				fnGr_PutString(109, 165, ERR_STRING, RGB(255,0,0), RGB(255,255,255), ORIENTATION_90,0);
			}
			else
			{/*change10*/
				fnGr_PutString(109, 100, PERCENT, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
//				sprintf((char *)rgcTemp,"%0.1f",(((stCurrent_Value.fCurrLiquidLevelBase / 10) / 
//								stConfig_Value.fConfigBaseTankHeight) * 100.0f));
				sprintf((char *)rgcTemp,"%0.1f",((stCurrent_Value.fCurrLiquidLevelBase  / 
								stConfig_Value.fConfigBaseTankHeight) * 100.0f));
				fnGr_PutString(109, 185, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			}
			
			/*Hardner Material Liquid level*/
			fnGr_PutString(142, 380, HARDNER_MATERIAL_LIQUID_LEVEL, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			if ( INVALID_DATA == stCurrent_Value.fLiquid_Hardner_Level_Curr )
			{
				fnGr_PutString(142, 165, ERR_STRING, RGB(255,0,0), RGB(255,255,255), ORIENTATION_90,0);
			}
			else
			{/*change10*/
				fnGr_PutString(142, 100, PERCENT, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
//				sprintf((char *)rgcTemp,"%0.1f",(((stCurrent_Value.fLiquid_Hardner_Level_Curr / 10) / 
//									stConfig_Value.fConfigHardTankHeight) * 100.0f));
				sprintf((char *)rgcTemp,"%0.1f",((stCurrent_Value.fLiquid_Hardner_Level_Curr / 
									stConfig_Value.fConfigHardTankHeight) * 100.0f));
				fnGr_PutString(142, 185, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			}
			
			/*OPMM Battery Status*/
			fnGr_PutString(178, 380, OPMM_BATTERY_STATUS, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			sprintf((char *)rgcTemp,"%0.1f",stCurrent_Value.fCurrBateryStatus);
			fnGr_PutString(178, 185, rgcTemp, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,1);
			fnGr_PutString(178, 100, VOLT, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			
			/*System Time And Date*/		
			if ( METRIC == stConfig_Value.ucTypeofUnit )
			{
				fnGr_PutString(211, 315, stDisplayDateTime.ucDD, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
				fnGr_PutString(211, 279, stDisplayDateTime.ucMM, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			}
			else
			{
				fnGr_PutString(211, 279, stDisplayDateTime.ucDD, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
				fnGr_PutString(211, 315, stDisplayDateTime.ucMM, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			}
			fnGr_PutString(211, 289, SLASH, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			fnGr_PutString(211, 253, SLASH, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			fnGr_PutString(211, 243, stDisplayDateTime.ucYY, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			
			fnGr_PutString(211, 170, stDisplayDateTime.ucHR, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			fnGr_PutString(211, 140, COLON, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			fnGr_PutString(211, 130, stDisplayDateTime.ucMIN, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			fnGr_PutString(211, 100, COLON, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			fnGr_PutString(211, 90, stDisplayDateTime.ucSEC, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
				
		}break;
		case SCREEN_DIAGNOSTIC_STATUS:
		{
			fnGr_PutString(9, 310, DIAGNOSTIC_STATUS, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(60, 380, SCU_STRING, RGB(0,0,0), 
														LIGHT_GREY_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(60, 185, OPMM_STRING, RGB(0,0,0), 
														LIGHT_GREY_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(125, 380, VCM_STRING, RGB(0,0,0), 
														LIGHT_GREY_CLR, ORIENTATION_90,0);
					
			fnGr_PutString(125, 185, HCM_STRING, RGB(0,0,0), 
														LIGHT_GREY_CLR, ORIENTATION_90,0);
			
			/*change2*/
			/*HMI Diagnostic screen added*/
			fnGr_PutString(190, 380, HMI_STRING, RGB(0,0,0), 
														LIGHT_GREY_CLR, ORIENTATION_90,0);
		}break;
		
		case SCREEN_SCU_DIAGNOSTIC:
		{
			/*Screen title*/
			fnGr_PutString(9, 280, "SCU DIAGNOSTIC", WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
						
			if ( TRUE == stDiagStatus.bSCU_DiagStatusReceived )
			{
				/*EEPROM*/
				fnStringColor_Set(uiSCU_DiagnosticStatus & MCU_EEPROM);
				fnGr_PutString(43, 380, EEPROM, RGB(ucRedColor,ucGreenColor,ucBlueColor),
												RGB(255,255,255), ORIENTATION_90,0);
				
				/*NVSRAM1 change13*/
				fnStringColor_Set(uiSCU_DiagnosticStatus & MCU_NVSRAM1);
				fnGr_PutString(76, 380, NVSRAM1, RGB(ucRedColor,ucGreenColor,ucBlueColor), 
												RGB(255,255,255), ORIENTATION_90,0);
				/*NVSRAM2*/
				fnStringColor_Set(uiSCU_DiagnosticStatus & MCU_NVSRAM2);
				fnGr_PutString(109, 380, NVSRAM2, RGB(ucRedColor,ucGreenColor,ucBlueColor), 
												RGB(255,255,255), ORIENTATION_90,0);
				
				/*PRESSURE_REGULATOR*/
				fnStringColor_Set(uiSCU_DiagnosticStatus & MCU_PRESSURE_REGULATOR);
				fnGr_PutString(142, 380, PRESSURE_REGULATOR, RGB(ucRedColor,ucGreenColor,ucBlueColor), 
												RGB(255,255,255), ORIENTATION_90,0);
			}else
			{
				fnGr_PutString(109, 280, "Diagnostic Status" , RED_CLR, 
												RGB(255,255,255), ORIENTATION_90,0);
				fnGr_PutString(142, 270, "Not Available!!!!", RED_CLR, 
												RGB(255,255,255), ORIENTATION_90,0);
			}
		}break;
		
		case SCREEN_OPMM_DIAGNOSTIC_1:
		{
			
			/*Screen title*/
			fnGr_PutString(9, 300, "OPMM DIAGNOSTIC", WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			
			if ( TRUE == stDiagStatus.bOPMM_DiagStatusReceived  )
			{
				/*Head to Prime/Spray*/
				if( BIT_1 ==  (uliOPMM_DIagnosticStatus & BIT_1) )
				{
					fnStringColor_Set( DEFAULT );
				}
				else 
				{
					fnStringColor_Set( (uliOPMM_DIagnosticStatus >> 9) & 0x01 );
				}
				fnGr_PutString(43, 380, HEAD_TO_PRIME_SPRAY, RGB(ucRedColor,ucGreenColor,ucBlueColor), 
												RGB(255,255,255), ORIENTATION_90,0);
				/*NOt Connected - GREY*/
				/*Pass 					- Green/Blue*/
				/*Fail 					- Red*/
				
				/*valve A/B*/
				if( BIT_2 ==  (uliOPMM_DIagnosticStatus & BIT_2) )
				{
					fnStringColor_Set( DEFAULT );
				}
				else 
				{
					fnStringColor_Set( (uliOPMM_DIagnosticStatus >> 10) & 0x01 );
				}
				fnGr_PutString(76, 380, A_B_VALVE, RGB(ucRedColor,ucGreenColor,ucBlueColor), 
												RGB(255,255,255), ORIENTATION_90,0);
				
					
				/*Forward valve*/
				if( BIT_4 ==  (uliOPMM_DIagnosticStatus & BIT_4) )
				{
					fnStringColor_Set( DEFAULT );
				}
				else 
				{
					fnStringColor_Set( (uliOPMM_DIagnosticStatus >> 12) & 0x01 );
				}
				fnGr_PutString(109, 380, FORWARD_VALVE, RGB(ucRedColor,ucGreenColor,ucBlueColor), 
												RGB(255,255,255), ORIENTATION_90,0);
				
					
				/*Backward Valve*/
				if( BIT_5 ==  (uliOPMM_DIagnosticStatus & BIT_5) )
				{
					fnStringColor_Set( DEFAULT );
				}
				else 
				{
					fnStringColor_Set( (uliOPMM_DIagnosticStatus >> 13) & 0x01 );
				}
				fnGr_PutString(142, 380, BACKWARD_VALVE, RGB(ucRedColor,ucGreenColor,ucBlueColor), 
												RGB(255,255,255), ORIENTATION_90,0);
				
					
				/*Spray Valve*/
				if( BIT_0 ==  (uliOPMM_DIagnosticStatus & BIT_0) )
				{
					fnStringColor_Set( DEFAULT );
				}
				else 
				{
					fnStringColor_Set( (uliOPMM_DIagnosticStatus >> 8) & 0x01 );
				}
				fnGr_PutString(178, 380, SPRAY_VALVE, RGB(ucRedColor,ucGreenColor,ucBlueColor), 
												RGB(255,255,255), ORIENTATION_90,0);

				/*Solvent Valve*/
				if( BIT_3 ==  (uliOPMM_DIagnosticStatus & BIT_3) )
				{
					fnStringColor_Set( DEFAULT );
				}
				else 
				{
					fnStringColor_Set( (uliOPMM_DIagnosticStatus >> 11) & 0x01 );
				}
				fnGr_PutString(211, 380, SOLVENT_VALVE, RGB(ucRedColor,ucGreenColor,ucBlueColor), 
												RGB(255,255,255), ORIENTATION_90,0);
				
				/*NCTS 1*/
				fnStringColor_Set((uliOPMM_DIagnosticStatus >> 16)& 0x01);
				fnGr_PutString(43, 170, NCTS_1, RGB(ucRedColor,ucGreenColor,ucBlueColor), 
												RGB(255,255,255), ORIENTATION_90,0);
												
				/*NCTS 2*/
				fnStringColor_Set((uliOPMM_DIagnosticStatus >> 17)& 0x01);
				fnGr_PutString(76, 170, NCTS_2, RGB(ucRedColor,ucGreenColor,ucBlueColor), 
												RGB(255,255,255), ORIENTATION_90,0);
				
					
				/*NCTS 3*/
				fnStringColor_Set((uliOPMM_DIagnosticStatus >> 18)& 0x01);
				fnGr_PutString(109, 170, NCTS_3, RGB(ucRedColor,ucGreenColor,ucBlueColor), 
												RGB(255,255,255), ORIENTATION_90,0);
				
					
				/*NCTS 4*/
				fnStringColor_Set((uliOPMM_DIagnosticStatus >> 19)& 0x01);
				fnGr_PutString(142, 170, NCTS_4, RGB(ucRedColor,ucGreenColor,ucBlueColor), 
												RGB(255,255,255), ORIENTATION_90,0);
												
				/*Base Temp Sns*/
				fnStringColor_Set((uliOPMM_DIagnosticStatus >> 20)& 0x01);
				fnGr_PutString(178, 170, "Base Temp Sns", RGB(ucRedColor,ucGreenColor,ucBlueColor), 
												RGB(255,255,255), ORIENTATION_90,0);
				
				/*Hard Temp Sns*/
				fnStringColor_Set((uliOPMM_DIagnosticStatus >> 21)& 0x01);
				fnGr_PutString(211, 170, "Hard Temp Sns", RGB(ucRedColor,ucGreenColor,ucBlueColor), 
												RGB(255,255,255), ORIENTATION_90,0);
			}else
			{
				fnGr_PutString(109, 280, "Diagnostic Status" , RED_CLR, 
												RGB(255,255,255), ORIENTATION_90,0);
				fnGr_PutString(142, 270, "Not Available!!!!", RED_CLR, 
												RGB(255,255,255), ORIENTATION_90,0);
			}
		}break;
		
		case SCREEN_VCM_DIAGNOSTIC:
		{
			/*Screen title*/
			fnGr_PutString(9, 280, "VCM DIAGNOSTIC", WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			
			if ( TRUE == stDiagStatus.bVCM_DiagStatusReceived )
			{
				/*Stall Valve*/
				if( BIT_0 ==  (uiVCM_DiagnosticStatus & BIT_0) )
				{
					fnStringColor_Set( DEFAULT );
				}
				else 
				{
					fnStringColor_Set((uiVCM_DiagnosticStatus >> 1)& 0x01);
				}
				
				/*NOt Connected - GREY*/
				/*Pass 					- Green/Blue*/
				/*Fail 					- Red*/
				fnGr_PutString(43, 380, "Stall Valve", RGB(ucRedColor,ucGreenColor,ucBlueColor), 
												RGB(255,255,255), ORIENTATION_90,0);
				
				/*LINEAR_TRANSDUCER*/
				fnStringColor_Set((uiVCM_DiagnosticStatus >> 5)& 0x01);
				fnGr_PutString(76, 380, LINEAR_TRANSDUCER, RGB(ucRedColor,ucGreenColor,ucBlueColor), 
												RGB(255,255,255), ORIENTATION_90,0);
				
				/*GAUGE1*/
				fnStringColor_Set((uiVCM_DiagnosticStatus >> 2)& 0x01);
				fnGr_PutString(109, 380, GAUGE1, RGB(ucRedColor,ucGreenColor,ucBlueColor), 
												RGB(255,255,255), ORIENTATION_90,0);
				
				/*GAUGE2*/
				fnStringColor_Set((uiVCM_DiagnosticStatus >> 3)& 0x01);
				fnGr_PutString(142, 380, GAUGE2, RGB(ucRedColor,ucGreenColor,ucBlueColor), 
												RGB(255,255,255), ORIENTATION_90,0);
				
				/*GAUGE3*/
				fnStringColor_Set((uiVCM_DiagnosticStatus >> 4)& 0x01);
				fnGr_PutString(178, 380, GAUGE3, RGB(ucRedColor,ucGreenColor,ucBlueColor), 
												RGB(255,255,255), ORIENTATION_90,0);
				
				/*EE07-1*/
				fnStringColor_Set((uiVCM_DiagnosticStatus >> 6)& 0x01);
				fnGr_PutString(211, 380, "EE07-1", RGB(ucRedColor,ucGreenColor,ucBlueColor), 
												RGB(255,255,255), ORIENTATION_90,0);
				
				/*EE07-2*/
				fnStringColor_Set((uiVCM_DiagnosticStatus >> 7)& 0x01);
				fnGr_PutString(43, 185, "EE07-2", RGB(ucRedColor,ucGreenColor,ucBlueColor), 
												RGB(255,255,255), ORIENTATION_90,0);	
				
				/*Recirc Valve change13*/
				if( BIT_8 ==  (uiVCM_DiagnosticStatus & BIT_8) )
				{
					fnStringColor_Set( DEFAULT );
				}
				else 
				{
					fnStringColor_Set((uiVCM_DiagnosticStatus >> 9)& 0x01);
				}
				
				/*NOt Connected - GREY*/
				/*Pass 					- Green/Blue*/
				/*Fail 					- Red*/
				fnGr_PutString(76, 185, "Recirc Valve", RGB(ucRedColor,ucGreenColor,ucBlueColor), 
												RGB(255,255,255), ORIENTATION_90,0);												
												
			}else
			{
				fnGr_PutString(109, 280, "Diagnostic Status" , RED_CLR, 
												RGB(255,255,255), ORIENTATION_90,0);
				fnGr_PutString(142, 270, "Not Available!!!!", RED_CLR, 
												RGB(255,255,255), ORIENTATION_90,0);
			}
		}break;
		
		case SCREEN_HCM_DIAGNOSTIC:
		{
			/*Screen title*/
			fnGr_PutString(9, 280, "HCM DIAGNOSTIC", WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			
			if ( TRUE == stDiagStatus.bHCM_DiagStatusReceived )
			{
				/*HEATER*/
				fnStringColor_Set((uiHCM_DiagnosticStatus >> 2)& 0x01);
				fnGr_PutString(43, 380, BASE_HEATER, RGB(ucRedColor,ucGreenColor,ucBlueColor), 
												RGB(255,255,255), ORIENTATION_90,0);
				
				fnStringColor_Set((uiHCM_DiagnosticStatus >> 3)& 0x01);
				fnGr_PutString(76, 380, HARDNER_HEATER, RGB(ucRedColor,ucGreenColor,ucBlueColor), 
												RGB(255,255,255), ORIENTATION_90,0);
				
				fnStringColor_Set( uiHCM_DiagnosticStatus & 0x01 );
				fnGr_PutString(109, 380, BASE_INL_HEATER, RGB(ucRedColor,ucGreenColor,ucBlueColor), 
												RGB(255,255,255), ORIENTATION_90,0);
				
				fnStringColor_Set((uiHCM_DiagnosticStatus >> 1)& 0x01);
				fnGr_PutString(142, 380, HARD_INL_HEATER, RGB(ucRedColor,ucGreenColor,ucBlueColor), 
												RGB(255,255,255), ORIENTATION_90,0);
				
				/*LIQUID_LEVEL_SENSOR*/
				fnStringColor_Set((uiHCM_DiagnosticStatus >> 8)& 0x01);
				fnGr_PutString(178, 380, "Base Liq lv", RGB(ucRedColor,ucGreenColor,ucBlueColor), 
												RGB(255,255,255), ORIENTATION_90,0);
												
				fnStringColor_Set((uiHCM_DiagnosticStatus >> 9)& 0x01);
				fnGr_PutString(211, 380, "Hard Liq lv", RGB(ucRedColor,ucGreenColor,ucBlueColor), 
												RGB(255,255,255), ORIENTATION_90,0);
				
				/*THERMOCOUPLE*/
				fnStringColor_Set((uiHCM_DiagnosticStatus >> 6) & 0x01);
				fnGr_PutString(43, 185, "Antifreez TS", RGB(ucRedColor,ucGreenColor,ucBlueColor), 
												RGB(255,255,255), ORIENTATION_90,0);
												
				fnStringColor_Set((uiHCM_DiagnosticStatus >> 4)& 0x01);
				fnGr_PutString(76, 185, "Base Inl TS", RGB(ucRedColor,ucGreenColor,ucBlueColor), 
												RGB(255,255,255), ORIENTATION_90,0);
				
				fnStringColor_Set((uiHCM_DiagnosticStatus >> 5)& 0x01);
				fnGr_PutString(109, 185, "Hard Inl TS", RGB(ucRedColor,ucGreenColor,ucBlueColor), 
												RGB(255,255,255), ORIENTATION_90,0);
												
				fnStringColor_Set((uiHCM_DiagnosticStatus >> 7)& 0x01);
				fnGr_PutString(142, 185, "Return Line TS", RGB(ucRedColor,ucGreenColor,ucBlueColor), 
												RGB(255,255,255), ORIENTATION_90,0);
			}else
			{
				fnGr_PutString(109, 280, "Diagnostic Status" , RED_CLR, 
												RGB(255,255,255), ORIENTATION_90,0);
				fnGr_PutString(142, 270, "Not Available!!!!", RED_CLR, 
												RGB(255,255,255), ORIENTATION_90,0);
			}			
		}break;
		case SCREEN_HMI_DIAGNOSTIC:
		{
			/*Screen title*/
			fnGr_PutString(9, 280, "HMI DIAGNOSTIC", WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
						
				/*EEPROM*/
				fnStringColor_Set(StatusFlag.stStatusFlag.bDiagnostic_SW_Status);
				fnGr_PutString(43, 380, PUSH_ON_SWITCH, RGB(ucRedColor,ucGreenColor,ucBlueColor),
												RGB(255,255,255), ORIENTATION_90,0);
				
				/*NVSRAM*/
				fnStringColor_Set(StatusFlag.stStatusFlag.bDiagnostic_NVSRAM_Status);
				fnGr_PutString(76, 380, NVSRAM, RGB(ucRedColor,ucGreenColor,ucBlueColor), 
												RGB(255,255,255), ORIENTATION_90,0);
		}break;
		case SCREEN_PERIPHERAL_SETTING:
		{
			/*Screen title*/
			fnGr_PutString(9, 280, "Peripheral Setting", WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(60, 380, VCM_STRING, RGB(0,0,0), 
														LIGHT_GREY_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(60, 185, HCM_STRING, RGB(0,0,0), 
														LIGHT_GREY_CLR, ORIENTATION_90,0);
			
														
			fnGr_PutString(125, 380, OPMM_STRING, RGB(0,0,0), 
											LIGHT_GREY_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(125, 185, SCU_STRING, RGB(0,0,0), 
														LIGHT_GREY_CLR, ORIENTATION_90,0);
		}break;
		
		case SCREEN_VCM_PP_SETTING_1:
		{
			fnLCDbmp(uiCursorXPos, uiCursorYPos,30,26,ucrgArrow);
			
			/*Screen title*/
			fnGr_PutString(9, 300, "VCM Peripheral Setting", WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(43, 220, "PORT", BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			fnGr_PutString(43, 158, BYPASS, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(76, 390, "Pressure G1", BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			if( 0 == stConfig_Value.ucConfigConnPressureGuage1 )
			{
				strcpy((char *)rgcTemp,"-");
			}
			else
			{
				sprintf((char *)rgcTemp,"%d",stConfig_Value.ucConfigConnPressureGuage1);
			}		
				fnGr_PutString(76, 200, rgcTemp, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(109, 390, "Pressure G2", BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			if( 0 == stConfig_Value.ucConfigConnPressureGuage2 )
			{
				strcpy((char *)rgcTemp,"-");
			}
			else
			{
				sprintf((char *)rgcTemp,"%d",stConfig_Value.ucConfigConnPressureGuage2);
			}
			fnGr_PutString(109, 200, rgcTemp, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(142, 390, "Pressure G3", BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			if( 0 == stConfig_Value.ucConfigConnPressureGuage3 )
			{
				strcpy((char *)rgcTemp,"-");
			}
			else
			{
				sprintf((char *)rgcTemp,"%d",stConfig_Value.ucConfigConnPressureGuage3);
			}
				fnGr_PutString(142, 200, rgcTemp, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(178, 390, "Ambient snr IN", BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			if( 0 == stConfig_Value.ucConfigConnInsideEE07 )
			{
				strcpy((char *)rgcTemp,"-");
			}
			else
			{
				sprintf((char *)rgcTemp,"%d",stConfig_Value.ucConfigConnInsideEE07);
			}
			fnGr_PutString(178, 200, rgcTemp, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(211, 390, "Ambient snr OUT", BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			if( 0 == stConfig_Value.ucConfigConnOutsideEE07 )
			{
				strcpy((char *)rgcTemp,"-");
			}
			else
			{
				sprintf((char *)rgcTemp,"%d",stConfig_Value.ucConfigConnOutsideEE07);
			}
				fnGr_PutString(211, 200, rgcTemp, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			
			if( FALSE == StatusFlag.stStatusFlag.bVCMPortSetFail)
			{
				fnGr_PutString(211, 33, SET_BUTTON, WHITE_CLR , 
				DARK_GREY_CLR, ORIENTATION_90,0);
			}
			/*pressure guage 1*/
			if( BIT_1 == (stConfig_Value.uiConfigVCMBypass & BIT_1) )
			{
				fnLCDbmp(100,105,24,24,ucrgUncheck);
			}else
			{
				fnLCDbmp(100,105,24,24,ucrgCheck);
			}
			/*pressure guage 2*/
			if( BIT_2 == (stConfig_Value.uiConfigVCMBypass & BIT_2) )
			{
				fnLCDbmp(132,105,24,24,ucrgUncheck);
			}else
			{
				fnLCDbmp(132,105,24,24,ucrgCheck);
			}
			/*pressure guage 3*/
			if( BIT_3 == (stConfig_Value.uiConfigVCMBypass & BIT_3) )
			{
				fnLCDbmp(165,105,24,24,ucrgUncheck);
			}else
			{
				fnLCDbmp(165,105,24,24,ucrgCheck);
			}
			/*Ambient Sensor In*/
			if( BIT_5 == (stConfig_Value.uiConfigVCMBypass & BIT_5) )
			{
				fnLCDbmp(199,105,24,24,ucrgUncheck);
			}else
			{
				fnLCDbmp(199,105,24,24,ucrgCheck);
			}
			/*Ambient Sensor Out*/
			if( BIT_6 == (stConfig_Value.uiConfigVCMBypass & BIT_6) )
			{
				fnLCDbmp(234,105,24,24,ucrgUncheck);
			}else
			{
				fnLCDbmp(234,105,24,24,ucrgCheck);
			}
		}break;
		
		case SCREEN_VCM_PP_SETTING_2:
		{
			fnLCDbmp(uiCursorXPos, uiCursorYPos,30,26,ucrgArrow);
			
			/*Screen title*/
			fnGr_PutString(9, 300, "VCM Peripheral Setting", WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(43, 220, "PORT", BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			fnGr_PutString(43, 158, BYPASS, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			/**********************************************************/
			fnGr_PutString(76, 390, "Stall Valve", BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			if( 0 == stConfig_Value.ucConfigConnStallvalve )
			{
				strcpy((char *)rgcTemp,"-");
			}
			else
			{
				sprintf((char *)rgcTemp,"%d",stConfig_Value.ucConfigConnStallvalve);
			}
			fnGr_PutString(76, 200, rgcTemp, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			/*******************************************************************/
			fnGr_PutString(109, 390, "Linear Td", BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			if( 0 == stConfig_Value.ucConfigConnLinearTransducer )
			{
				strcpy((char *)rgcTemp,"-");
			}
			else
			{
				sprintf((char *)rgcTemp,"%d",stConfig_Value.ucConfigConnLinearTransducer);
			}
			fnGr_PutString(109, 200, rgcTemp, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(142, 390, "Air Sensor", BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			fnGr_PutString(142, 205, NA, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			
			/**********************change15************************************/
			fnGr_PutString(175, 390, "Recirc Valve", BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			if( 0 == stConfig_Value.ucConfigConnRecircvalve )
			{
				strcpy((char *)rgcTemp,"-");
			}
			else
			{
				sprintf((char *)rgcTemp,"%d",stConfig_Value.ucConfigConnRecircvalve);
			}
			fnGr_PutString(175, 200, rgcTemp, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			/*******************************************************************/
			
			if( FALSE == StatusFlag.stStatusFlag.bVCMPortSetFail)
			{
				fnGr_PutString(211, 33, SET_BUTTON, WHITE_CLR , 
				DARK_GREY_CLR, ORIENTATION_90,0);
			}
			
			if( BIT_4 == (stConfig_Value.uiConfigVCMBypass & BIT_4) )
			{
				fnLCDbmp(100,105,24,24,ucrgUncheck);
			}else
			{
				fnLCDbmp(100,105,24,24,ucrgCheck);
			}
			
			if( BIT_0 == (stConfig_Value.uiConfigVCMBypass & BIT_0) )
			{
				fnLCDbmp(132,105,24,24,ucrgUncheck);
			}else
			{
				fnLCDbmp(132,105,24,24,ucrgCheck);
			}
			fnLCDbmp(165,105,24,24,ucrgCheck);
			if( BIT_7 == (stConfig_Value.uiConfigVCMBypass & BIT_7) )
			{
				fnLCDbmp(198,105,24,24,ucrgUncheck);
			}
			else
			{
				fnLCDbmp(198,105,24,24,ucrgCheck);
			}
		}break;
		
		case SCREEN_HCM_PP_SETTING_1:
		{
			fnLCDbmp(uiCursorXPos, uiCursorYPos,30,26,ucrgArrow);
			
			/*Screen title*/
			fnGr_PutString(9, 300, "HCM Peripheral Setting", WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(43, 220, "PORT", BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			fnGr_PutString(43, 158, BYPASS, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(76, 390, BASE_INL_HEATER, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			if( 0 == stConfig_Value.ucConfig_ConnBaseinlineHeater )
			{
				strcpy((char *)rgcTemp,"-");
			}
			else
			{
				sprintf((char *)rgcTemp,"%d",stConfig_Value.ucConfig_ConnBaseinlineHeater);
			}
			fnGr_PutString(76, 200, rgcTemp, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(109, 390, HARD_INL_HEATER, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			if( 0 == stConfig_Value.ucConfig_ConnHardinlineHeater )
			{
				strcpy((char *)rgcTemp,"-");
			}
			else
			{
				sprintf((char *)rgcTemp,"%d",stConfig_Value.ucConfig_ConnHardinlineHeater);
			}
			fnGr_PutString(109, 200, rgcTemp, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(142, 390, BASE_HEATER, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			if( 0 == stConfig_Value.ucConfig_ConnAntifreezeHeater )
			{
				strcpy((char *)rgcTemp,"-");
			}
			else
			{
				sprintf((char *)rgcTemp,"%d",stConfig_Value.ucConfig_ConnAntifreezeHeater);
			}
			fnGr_PutString(142, 200, rgcTemp, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(178, 390, HARDNER_HEATER, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			if( 0 == stConfig_Value.ucConfig_ConnHardHeater )
			{
				strcpy((char *)rgcTemp,"-");
			}
			else
			{
				sprintf((char *)rgcTemp,"%d",stConfig_Value.ucConfig_ConnHardHeater);
			}
			fnGr_PutString(178, 200, rgcTemp, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(211, 390, BASE_INL_TC, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			if( 0 == stConfig_Value.ucConfig_ConnBaseInlineTher )
			{
				strcpy((char *)rgcTemp,"-");
			}
			else
			{
				sprintf((char *)rgcTemp,"%d",stConfig_Value.ucConfig_ConnBaseInlineTher);
			}
			fnGr_PutString(211, 200, rgcTemp, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			
			if( FALSE == StatusFlag.stStatusFlag.bHCMPortSetFail)
			{
				fnGr_PutString(211, 33, SET_BUTTON, WHITE_CLR , 
				DARK_GREY_CLR, ORIENTATION_90,0);
			}
			
			if( BIT_0 == (stConfig_Value.uiConfigHCMBypass & BIT_0) )
			{
				fnLCDbmp(100,105,24,24,ucrgUncheck);
			}else
			{
				fnLCDbmp(100,105,24,24,ucrgCheck);
			}
			
			if( BIT_1 == (stConfig_Value.uiConfigHCMBypass & BIT_1) )
			{
				fnLCDbmp(132,105,24,24,ucrgUncheck);
			}else
			{
				fnLCDbmp(132,105,24,24,ucrgCheck);
			}
			
			if( BIT_2 == (stConfig_Value.uiConfigHCMBypass & BIT_2) )
			{
				fnLCDbmp(165,105,24,24,ucrgUncheck);
			}else
			{
				fnLCDbmp(165,105,24,24,ucrgCheck);
			}
			
			if( BIT_3 == (stConfig_Value.uiConfigHCMBypass & BIT_3) )
			{
				fnLCDbmp(199,105,24,24,ucrgUncheck);
			}else
			{
				fnLCDbmp(199,105,24,24,ucrgCheck);
			}
			
			if( BIT_4 == (stConfig_Value.uiConfigHCMBypass & BIT_4) )
			{
				fnLCDbmp(234,105,24,24,ucrgUncheck);
			}else
			{
				fnLCDbmp(234,105,24,24,ucrgCheck);
			}
			
		}break;
		
		case SCREEN_HCM_PP_SETTING_2:
		{
			fnLCDbmp(uiCursorXPos, uiCursorYPos,30,26,ucrgArrow);
			
			/*Screen title*/
			fnGr_PutString(9, 300, "HCM Peripheral Setting", WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(43, 220, "PORT", BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			fnGr_PutString(43, 158, BYPASS, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(76, 390, HARD_INL_TC, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			if( 0 == stConfig_Value.ucConfig_ConnHardInlineTher )
			{
				strcpy((char *)rgcTemp,"-");
			}
			else
			{
				sprintf((char *)rgcTemp,"%d",stConfig_Value.ucConfig_ConnHardInlineTher);
			}
			fnGr_PutString(76, 200, rgcTemp, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(109, 390, BASE_TC, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			if( 0 == stConfig_Value.ucConfig_ConnAntifreezeTher )
			{
				strcpy((char *)rgcTemp,"-");
			}
			else
			{
				sprintf((char *)rgcTemp,"%d",stConfig_Value.ucConfig_ConnAntifreezeTher);
			}
			fnGr_PutString(109, 200, rgcTemp, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(142, 390, RETURN_LINE_TC, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			if( 0 == stConfig_Value.ucConfig_ConnBasereturnlineTher )
			{
				strcpy((char *)rgcTemp,"-");
			}
			else
			{
				sprintf((char *)rgcTemp,"%d",stConfig_Value.ucConfig_ConnBasereturnlineTher);
			}
			fnGr_PutString(142, 200, rgcTemp, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(178, 390, "Base Tank LLS", BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			if( 0 == stConfig_Value.ucConfig_ConnBaseTankLL )
			{
				strcpy((char *)rgcTemp,"-");
			}
			else
			{
				sprintf((char *)rgcTemp,"%d",stConfig_Value.ucConfig_ConnBaseTankLL);
			}
			fnGr_PutString(178, 200, rgcTemp, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(211, 390, "Hard Tank LLS", BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			if( 0 == stConfig_Value.ucConfig_ConnHardTankLL )
			{
				strcpy((char *)rgcTemp,"-");
			}
			else
			{
				sprintf((char *)rgcTemp,"%d",stConfig_Value.ucConfig_ConnHardTankLL);
			}
			fnGr_PutString(211, 200, rgcTemp, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			
			if( FALSE == StatusFlag.stStatusFlag.bHCMPortSetFail)
			{
				fnGr_PutString(211, 33, SET_BUTTON, WHITE_CLR , 
				DARK_GREY_CLR, ORIENTATION_90,0);
			}
			
			if( BIT_5 == (stConfig_Value.uiConfigHCMBypass & BIT_5) )
			{
				fnLCDbmp(100,105,24,24,ucrgUncheck);
			}else
			{
				fnLCDbmp(100,105,24,24,ucrgCheck);
			}
			
			if( BIT_6 == (stConfig_Value.uiConfigHCMBypass & BIT_6) )
			{
				fnLCDbmp(132,105,24,24,ucrgUncheck);
			}else
			{
				fnLCDbmp(132,105,24,24,ucrgCheck);
			}
			
			if( BIT_7 == (stConfig_Value.uiConfigHCMBypass & BIT_7) )
			{
				fnLCDbmp(165,105,24,24,ucrgUncheck);
			}else
			{
				fnLCDbmp(165,105,24,24,ucrgCheck);
			}
			
			if( BIT_8 == (stConfig_Value.uiConfigHCMBypass & BIT_8) )
			{
				fnLCDbmp(199,105,24,24,ucrgUncheck);
			}else
			{
				fnLCDbmp(199,105,24,24,ucrgCheck);
			}
			
			if( BIT_9 == (stConfig_Value.uiConfigHCMBypass & BIT_9) )
			{
				fnLCDbmp(234,105,24,24,ucrgUncheck);
			}else
			{
				fnLCDbmp(234,105,24,24,ucrgCheck);
			}
		}break;
		
		case SCREEN_OPMM_PP_SETTING_1:
		{
			fnLCDbmp(uiCursorXPos, uiCursorYPos,30,26,ucrgArrow);
			
			/*Screen title*/
			fnGr_PutString(9, 300, "OPMM Peripheral Setting", WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(43, 220, "PORT", BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			fnGr_PutString(43, 158, BYPASS, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(76, 390, SPRAY_VALVE, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			sprintf((char *)rgcTemp,"%d",stConfig_Value.ucConfigConnSprayvalve);
			fnGr_PutString(76, 200, rgcTemp, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(109, 390, GUN_POSITION, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			sprintf((char *)rgcTemp,"%d",stConfig_Value.ucConfigConnGunposition);
			fnGr_PutString(109, 200, rgcTemp, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(142, 390, A_B_VALVE, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			sprintf((char *)rgcTemp,"%d",stConfig_Value.ucConfigConnABvalve);
			fnGr_PutString(142, 200, rgcTemp, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(178, 390, SOLVENT_VALVE, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			sprintf((char *)rgcTemp,"%d",stConfig_Value.ucConfigConnSolventvalve);
			fnGr_PutString(178, 200, rgcTemp, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(211, 390, FORWARD_VALVE, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			sprintf((char *)rgcTemp,"%d",stConfig_Value.ucConfigConnForwardvalve);
			fnGr_PutString(211, 200, rgcTemp, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(211, 33, SET_BUTTON, WHITE_CLR , 
			DARK_GREY_CLR, ORIENTATION_90,0);
			
			if( BIT_0 == (stConfig_Value.uiConfigOPMMBypass & BIT_0) )
			{
				fnLCDbmp(100,105,24,24,ucrgUncheck);
			}else
			{
				fnLCDbmp(100,105,24,24,ucrgCheck);
			}
			
			if( BIT_1 == (stConfig_Value.uiConfigOPMMBypass & BIT_1) )
			{
				fnLCDbmp(132,105,24,24,ucrgUncheck);
			}else
			{
				fnLCDbmp(132,105,24,24,ucrgCheck);
			}
			
			if( BIT_2 == (stConfig_Value.uiConfigOPMMBypass & BIT_2) )
			{
				fnLCDbmp(165,105,24,24,ucrgUncheck);
			}else
			{
				fnLCDbmp(165,105,24,24,ucrgCheck);
			}
			
			if( BIT_3 == (stConfig_Value.uiConfigOPMMBypass & BIT_3) )
			{
				fnLCDbmp(199,105,24,24,ucrgUncheck);
			}else
			{
				fnLCDbmp(199,105,24,24,ucrgCheck);
			}
			
			if( BIT_4 == (stConfig_Value.uiConfigOPMMBypass & BIT_4) )
			{
				fnLCDbmp(234,105,24,24,ucrgUncheck);
			}else
			{
				fnLCDbmp(234,105,24,24,ucrgCheck);
			}
		}break;
		
		case SCREEN_OPMM_PP_SETTING_2:
		{
			fnLCDbmp(uiCursorXPos, uiCursorYPos,30,26,ucrgArrow);
			
			/*Screen title*/
			fnGr_PutString(9, 300, "OPMM Peripheral Setting", WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(43, 220, "PORT", BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			fnGr_PutString(43, 158, BYPASS, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(76, 390, BACKWARD_VALVE, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			sprintf((char *)rgcTemp,"%d",stConfig_Value.ucConfigConnBackwardvalve);
			fnGr_PutString(76, 200, rgcTemp, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(109, 390, NCTS_1, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			sprintf((char *)rgcTemp,"%d",stConfig_Value.ucConfigNCTempSensor1);
			fnGr_PutString(109, 200, rgcTemp, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(142, 390, NCTS_2, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			sprintf((char *)rgcTemp,"%d",stConfig_Value.ucConfigNCTempSensor2);
			fnGr_PutString(142, 200, rgcTemp, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(178, 390, NCTS_3, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			sprintf((char *)rgcTemp,"%d",stConfig_Value.ucConfigNCTempSensor3);
			fnGr_PutString(178, 200, rgcTemp, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(211, 390, NCTS_4, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			sprintf((char *)rgcTemp,"%d",stConfig_Value.ucConfigNCTempSensor4);
			fnGr_PutString(211, 200, rgcTemp, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(211, 33, SET_BUTTON, WHITE_CLR , 
			DARK_GREY_CLR, ORIENTATION_90,0);
			
			if( BIT_5 == (stConfig_Value.uiConfigOPMMBypass & BIT_5) )
			{
				fnLCDbmp(100,105,24,24,ucrgUncheck);
			}else
			{
				fnLCDbmp(100,105,24,24,ucrgCheck);
			}
			
			if( BIT_8 == (stConfig_Value.uiConfigOPMMBypass & BIT_8) )
			{
				fnLCDbmp(132,105,24,24,ucrgUncheck);
			}else
			{
				fnLCDbmp(132,105,24,24,ucrgCheck);
			}
			
			if( BIT_9 == (stConfig_Value.uiConfigOPMMBypass & BIT_9) )
			{
				fnLCDbmp(165,105,24,24,ucrgUncheck);
			}else
			{
				fnLCDbmp(165,105,24,24,ucrgCheck);
			}
			
			if( BIT_10 == (stConfig_Value.uiConfigOPMMBypass & BIT_10) )
			{
				fnLCDbmp(199,105,24,24,ucrgUncheck);
			}else
			{
				fnLCDbmp(199,105,24,24,ucrgCheck);
			}
			
			if( BIT_11 == (stConfig_Value.uiConfigOPMMBypass & BIT_11) )
			{
				fnLCDbmp(234,105,24,24,ucrgUncheck);
			}else
			{
				fnLCDbmp(234,105,24,24,ucrgCheck);
			}
		}break;
		
		case SCREEN_OPMM_PP_SETTING_3:
		{
			fnLCDbmp(uiCursorXPos, uiCursorYPos,30,26,ucrgArrow);
			
			/*Screen title*/
			fnGr_PutString(9, 300, "OPMM Peripheral Setting", WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(43, 220, "PORT", BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			fnGr_PutString(43, 158, BYPASS, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(76, 390, "Base temp snr", BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			fnGr_PutString(76, 205, NA, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(109, 390, "Hard temp snr", BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			fnGr_PutString(109, 205, NA, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(211, 33, SET_BUTTON, WHITE_CLR , 
			DARK_GREY_CLR, ORIENTATION_90,0);
			
			if( BIT_12 == (stConfig_Value.uiConfigOPMMBypass & BIT_12) )
			{
				fnLCDbmp(100,105,24,24,ucrgUncheck);
			}else
			{
				fnLCDbmp(100,105,24,24,ucrgCheck);
			}
			
			if( BIT_13 == (stConfig_Value.uiConfigOPMMBypass & BIT_13) )
			{
				fnLCDbmp(132,105,24,24,ucrgUncheck);
			}else
			{
				fnLCDbmp(132,105,24,24,ucrgCheck);
			}
		}break;
		
		case SCREEN_SCU_PP_SETTING:
		{
			fnLCDbmp(uiCursorXPos, uiCursorYPos,30,26,ucrgArrow);
			
			/*Screen title*/
			fnGr_PutString(9, 300, "SCU Peripheral Setting", WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(43, 158, BYPASS, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(76, 390, EEPROM, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			fnGr_PutString(76, 205, NA, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(109, 390, NVSRAM, BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			fnGr_PutString(109, 205, NA, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(142, 390, "Press Regulator", BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			fnGr_PutString(142, 205, NA, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(178, 390, "Temp Achieve", BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			fnGr_PutString(178, 205, NA, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(211, 390, "Pressure Achieve", BLACK_CLR, 
												WHITE_CLR, ORIENTATION_90,0);
			fnGr_PutString(211, 205, NA, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(211, 33, SET_BUTTON, WHITE_CLR , 
			DARK_GREY_CLR, ORIENTATION_90,0);
			
			if( BIT_0 == (stConfig_Value.uiConfigSCUBypass & BIT_0) )
			{
				fnLCDbmp(100,105,24,24,ucrgUncheck);
			}else
			{
				fnLCDbmp(100,105,24,24,ucrgCheck);
			}
			
			if( BIT_1 == (stConfig_Value.uiConfigSCUBypass & BIT_1) )
			{
				fnLCDbmp(132,105,24,24,ucrgUncheck);
			}else
			{
				fnLCDbmp(132,105,24,24,ucrgCheck);
			}
			
			if( BIT_2 == (stConfig_Value.uiConfigSCUBypass & BIT_2) )
			{
				fnLCDbmp(165,105,24,24,ucrgUncheck);
			}else
			{
				fnLCDbmp(165,105,24,24,ucrgCheck);
			}
			
			if( TRUE == StatusFlag.stStatusFlag.bSCU_Temp_Bypass )
			{
				fnLCDbmp(199,105,24,24,ucrgUncheck);
			}else
			{
				fnLCDbmp(199,105,24,24,ucrgCheck);
			}
			
			if( TRUE == StatusFlag.stStatusFlag.bSCU_Pressure_Bypass )
			{
				fnLCDbmp(234,105,24,24,ucrgUncheck);
			}else
			{
				fnLCDbmp(234,105,24,24,ucrgCheck);
			}
			
		}break;

		case SCREEN_NOTIFICATION_1:
		{
			/*Screen title*/
			fnGr_PutString(9, 250, NOTIFICATION_1, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			
			fnGr_SelectFont("Franklin20", 1);/*change5*/
			/*1st notification*/
			fnGr_PutString(43, 380, ucrgStoredStatusMessage[0], RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			
			/*2nd notification*/
			fnGr_PutString(76, 380, ucrgStoredStatusMessage[1], RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			
			/*3rd notification*/
			fnGr_PutString(109, 380, ucrgStoredStatusMessage[2], RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			
			/*4th notification*/
			fnGr_PutString(142, 380, ucrgStoredStatusMessage[3], RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			
			/*5th notification*/
			fnGr_PutString(178, 380, ucrgStoredStatusMessage[4], RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			
			/*6th notification*/
			fnGr_PutString(211, 380, ucrgStoredStatusMessage[5], RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
		}break;
		
		case SCREEN_NOTIFICATION_2:
		{
			/*Screen title*/
			fnGr_PutString(9, 250, NOTIFICATION_2, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			fnGr_SelectFont("Franklin20", 1);/*change5*/
			/*1st notification*/
			fnGr_PutString(43, 380, ucrgStoredStatusMessage[6], RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			
			/*2nd notification*/
			fnGr_PutString(76, 380, ucrgStoredStatusMessage[7], RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			
			/*3rd notification*/
			fnGr_PutString(109, 380, ucrgStoredStatusMessage[8], RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			
			/*4th notification*/
			fnGr_PutString(142, 380, ucrgStoredStatusMessage[9], RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			
			/*5th notification*/
			fnGr_PutString(178, 380, ucrgStoredStatusMessage[10], RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			
			/*6th notification*/
			fnGr_PutString(211, 380, ucrgStoredStatusMessage[11], RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);	
		}break;
		
		case SCREEN_NOTIFICATION_3:
		{
			/*Screen title*/
			fnGr_PutString(9, 250, NOTIFICATION_3, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
		fnGr_SelectFont("Franklin20", 1);/*change5*/
			/*1st notification*/
			fnGr_PutString(43, 380, ucrgStoredStatusMessage[12], RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			
			/*2nd notification*/
			fnGr_PutString(76, 380, ucrgStoredStatusMessage[13], RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			
			/*3rd notification*/
			fnGr_PutString(109, 380, ucrgStoredStatusMessage[14], RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			
			/*4th notification*/
			fnGr_PutString(142, 380, ucrgStoredStatusMessage[15], RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			
			/*5th notification*/
			fnGr_PutString(178, 380, ucrgStoredStatusMessage[16], RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			
			/*6th notification*/
			fnGr_PutString(211, 380, ucrgStoredStatusMessage[17], RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
		}break;
		case SCREEN_NOTIFICATION_4:
		{
			/*Screen title*/
			fnGr_PutString(9, 250, NOTIFICATION_4, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			fnGr_SelectFont("Franklin20", 1);/*change5*/
			/*1st notification*/
			fnGr_PutString(43, 380, ucrgStoredStatusMessage[18], RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			
			/*2nd notification*/
			fnGr_PutString(76, 380, ucrgStoredStatusMessage[19], RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			
			/*3rd notification*/
			fnGr_PutString(109, 380, ucrgStoredStatusMessage[20], RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			
			/*4th notification*/
			fnGr_PutString(142, 380, ucrgStoredStatusMessage[21], RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			
			/*5th notification*/
			fnGr_PutString(178, 380, ucrgStoredStatusMessage[22], RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			
			/*6th notification*/
			fnGr_PutString(211, 380, ucrgStoredStatusMessage[23], RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
		}break;
		
		case SCREEN_ALARMS_1:
		{
			/*Screen title*/
			fnGr_PutString(9, 235, ALARMS_1, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			fnGr_SelectFont("Franklin20", 1);/*change5*/
			/*1st notification*/
			if ( SYSTEM_ERROR == rgucAlarmTpe[0] )
			{
				/*RED*/
				ucRedColor = 255;
				ucGreenColor = 0;
				ucBlueColor = 0;
			}
			else
			{
				/*BLUE*/
				ucRedColor = 0;
				ucGreenColor = 0;
				ucBlueColor = 255;
			}
			fnGr_PutString(43, 380, ucrgStoredAlarmMessage[0], RGB(ucRedColor,ucGreenColor,ucBlueColor), 
											RGB(255,255,255), ORIENTATION_90,0);
			
			/*2nd notification*/
			if ( SYSTEM_ERROR == rgucAlarmTpe[1] )
			{
				/*RED*/
				ucRedColor = 255;
				ucGreenColor = 0;
				ucBlueColor = 0;
			}
			else
			{
				/*BLUE*/
				ucRedColor = 0;
				ucGreenColor = 0;
				ucBlueColor = 255;
			}
			fnGr_PutString(76, 380, ucrgStoredAlarmMessage[1], RGB(ucRedColor,ucGreenColor,ucBlueColor), 
											RGB(255,255,255), ORIENTATION_90,0);
			
			/*3rd notification*/
			if ( SYSTEM_ERROR == rgucAlarmTpe[2] )
			{
				/*RED*/
				ucRedColor = 255;
				ucGreenColor = 0;
				ucBlueColor = 0;
			}
			else
			{
				/*BLUE*/
				ucRedColor = 0;
				ucGreenColor = 0;
				ucBlueColor = 255;
			}
			fnGr_PutString(109, 380, ucrgStoredAlarmMessage[2],  RGB(ucRedColor,ucGreenColor,ucBlueColor),
											RGB(255,255,255), ORIENTATION_90,0);
			
			/*4th notification*/
			if ( SYSTEM_ERROR == rgucAlarmTpe[3] )
			{
				/*RED*/
				ucRedColor = 255;
				ucGreenColor = 0;
				ucBlueColor = 0;
			}
			else
			{
				/*BLUE*/
				ucRedColor = 0;
				ucGreenColor = 0;
				ucBlueColor = 255;
			}
			fnGr_PutString(142, 380, ucrgStoredAlarmMessage[3],  RGB(ucRedColor,ucGreenColor,ucBlueColor), 
											RGB(255,255,255), ORIENTATION_90,0);
			
			/*5th notification*/
			if ( SYSTEM_ERROR == rgucAlarmTpe[4] )
			{
				/*RED*/
				ucRedColor = 255;
				ucGreenColor = 0;
				ucBlueColor = 0;
			}
			else
			{
				/*BLUE*/
				ucRedColor = 0;
				ucGreenColor = 0;
				ucBlueColor = 255;
			}
			fnGr_PutString(178, 380, ucrgStoredAlarmMessage[4],  RGB(ucRedColor,ucGreenColor,ucBlueColor),
											RGB(255,255,255), ORIENTATION_90,0);
			
			/*6th notification*/
			if ( SYSTEM_ERROR == rgucAlarmTpe[5] )
			{
				/*RED*/
				ucRedColor = 255;
				ucGreenColor = 0;
				ucBlueColor = 0;
			}
			else
			{
				/*BLUE*/
				ucRedColor = 0;
				ucGreenColor = 0;
				ucBlueColor = 255;
			}
			fnGr_PutString(211, 380, ucrgStoredAlarmMessage[5],  RGB(ucRedColor,ucGreenColor,ucBlueColor),
											RGB(255,255,255), ORIENTATION_90,0);
		}break;
		case SCREEN_ALARMS_2:
		{
			/*Screen title*/
			fnGr_PutString(9, 235, ALARMS_2, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
				fnGr_SelectFont("Franklin20", 1);/*change5*/		
			/*1st notification*/
			if ( SYSTEM_ERROR == rgucAlarmTpe[6] )
			{
				/*RED*/
				ucRedColor = 255;
				ucGreenColor = 0;
				ucBlueColor = 0;
			}
			else
			{
				/*BLUE*/
				ucRedColor = 0;
				ucGreenColor = 0;
				ucBlueColor = 255;
			}
			fnGr_PutString(43, 380, ucrgStoredAlarmMessage[6], RGB(ucRedColor,ucGreenColor,ucBlueColor), 
											RGB(255,255,255), ORIENTATION_90,0);
			
			/*2nd notification*/
			if ( SYSTEM_ERROR == rgucAlarmTpe[7] )
			{
				/*RED*/
				ucRedColor = 255;
				ucGreenColor = 0;
				ucBlueColor = 0;
			}
			else
			{
				/*BLUE*/
				ucRedColor = 0;
				ucGreenColor = 0;
				ucBlueColor = 255;
			}
			fnGr_PutString(76, 380, ucrgStoredAlarmMessage[7], RGB(ucRedColor,ucGreenColor,ucBlueColor), 
											RGB(255,255,255), ORIENTATION_90,0);
			
			/*3rd notification*/
			if ( SYSTEM_ERROR == rgucAlarmTpe[8] )
			{
				/*RED*/
				ucRedColor = 255;
				ucGreenColor = 0;
				ucBlueColor = 0;
			}
			else
			{
				/*BLUE*/
				ucRedColor = 0;
				ucGreenColor = 0;
				ucBlueColor = 255;
			}
			fnGr_PutString(109, 380, ucrgStoredAlarmMessage[8], RGB(ucRedColor,ucGreenColor,ucBlueColor), 
											RGB(255,255,255), ORIENTATION_90,0);
			
			/*4th notification*/
			if ( SYSTEM_ERROR == rgucAlarmTpe[9] )
			{
				/*RED*/
				ucRedColor = 255;
				ucGreenColor = 0;
				ucBlueColor = 0;
			}
			else
			{
				/*BLUE*/
				ucRedColor = 0;
				ucGreenColor = 0;
				ucBlueColor = 255;
			}
			fnGr_PutString(142, 380, ucrgStoredAlarmMessage[9], RGB(ucRedColor,ucGreenColor,ucBlueColor),
											RGB(255,255,255), ORIENTATION_90,0);
			
			/*5th notification*/
			if ( SYSTEM_ERROR == rgucAlarmTpe[10] )
			{
				/*RED*/
				ucRedColor = 255;
				ucGreenColor = 0;
				ucBlueColor = 0;
			}
			else
			{
				/*BLUE*/
				ucRedColor = 0;
				ucGreenColor = 0;
				ucBlueColor = 255;
			}
			fnGr_PutString(178, 380, ucrgStoredAlarmMessage[10], RGB(ucRedColor,ucGreenColor,ucBlueColor),
											RGB(255,255,255), ORIENTATION_90,0);
			
			/*6th notification*/
			if ( SYSTEM_ERROR == rgucAlarmTpe[11] )
			{
				/*RED*/
				ucRedColor = 255;
				ucGreenColor = 0;
				ucBlueColor = 0;
			}
			else
			{
				/*BLUE*/
				ucRedColor = 0;
				ucGreenColor = 0;
				ucBlueColor = 255;
			}
			fnGr_PutString(211, 380, ucrgStoredAlarmMessage[11], RGB(ucRedColor,ucGreenColor,ucBlueColor), 
											RGB(255,255,255), ORIENTATION_90,0);
		}break;
		case SCREEN_ALARMS_3:
		{
			/*Screen title*/
			fnGr_PutString(9, 235, ALARMS_3, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
		fnGr_SelectFont("Franklin20", 1);/*change5*/
			/*1st notification*/
			if ( SYSTEM_ERROR == rgucAlarmTpe[12] )
			{
				/*RED*/
				ucRedColor = 255;
				ucGreenColor = 0;
				ucBlueColor = 0;
			}
			else
			{
				/*BLUE*/
				ucRedColor = 0;
				ucGreenColor = 0;
				ucBlueColor = 255;
			}
			fnGr_PutString(43, 380, ucrgStoredAlarmMessage[12], RGB(ucRedColor,ucGreenColor,ucBlueColor), 
											RGB(255,255,255), ORIENTATION_90,0);
			
			/*2nd notification*/
			if ( SYSTEM_ERROR == rgucAlarmTpe[13] )
			{
				/*RED*/
				ucRedColor = 255;
				ucGreenColor = 0;
				ucBlueColor = 0;
			}
			else
			{
				/*BLUE*/
				ucRedColor = 0;
				ucGreenColor = 0;
				ucBlueColor = 255;
			}
			fnGr_PutString(76, 380, ucrgStoredAlarmMessage[13], RGB(ucRedColor,ucGreenColor,ucBlueColor), 
											RGB(255,255,255), ORIENTATION_90,0);
			
			/*3rd notification*/
			if ( SYSTEM_ERROR == rgucAlarmTpe[14] )
			{
				/*RED*/
				ucRedColor = 255;
				ucGreenColor = 0;
				ucBlueColor = 0;
			}
			else
			{
				/*BLUE*/
				ucRedColor = 0;
				ucGreenColor = 0;
				ucBlueColor = 255;
			}
			fnGr_PutString(109, 380, ucrgStoredAlarmMessage[14], RGB(ucRedColor,ucGreenColor,ucBlueColor), 
											RGB(255,255,255), ORIENTATION_90,0);
			
			/*4th notification*/
			if ( SYSTEM_ERROR == rgucAlarmTpe[15] )
			{
				/*RED*/
				ucRedColor = 255;
				ucGreenColor = 0;
				ucBlueColor = 0;
			}
			else
			{
				/*BLUE*/
				ucRedColor = 0;
				ucGreenColor = 0;
				ucBlueColor = 255;
			}
			fnGr_PutString(142, 380, ucrgStoredAlarmMessage[15], RGB(ucRedColor,ucGreenColor,ucBlueColor), 
											RGB(255,255,255), ORIENTATION_90,0);
			
			/*5th notification*/
			if ( SYSTEM_ERROR == rgucAlarmTpe[16] )
			{
				/*RED*/
				ucRedColor = 255;
				ucGreenColor = 0;
				ucBlueColor = 0;
			}
			else
			{
				/*BLUE*/
				ucRedColor = 0;
				ucGreenColor = 0;
				ucBlueColor = 255;
			}
			fnGr_PutString(178, 380, ucrgStoredAlarmMessage[16], RGB(ucRedColor,ucGreenColor,ucBlueColor), 
											RGB(255,255,255), ORIENTATION_90,0);
			
			/*6th notification*/
			if ( SYSTEM_ERROR == rgucAlarmTpe[17] )
			{
				/*RED*/
				ucRedColor = 255;
				ucGreenColor = 0;
				ucBlueColor = 0;
			}
			else
			{
				/*BLUE*/
				ucRedColor = 0;
				ucGreenColor = 0;
				ucBlueColor = 255;
			}
			fnGr_PutString(211, 380, ucrgStoredAlarmMessage[17], RGB(ucRedColor,ucGreenColor,ucBlueColor), 
											RGB(255,255,255), ORIENTATION_90,0);
		}break;
		case SCREEN_ALARMS_4:
		{
			/*Screen title*/
			fnGr_PutString(9, 235, ALARMS_4, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			fnGr_SelectFont("Franklin20", 1);/*change5*/
			/*1st notification*/
			if ( SYSTEM_ERROR == rgucAlarmTpe[18] )
			{
				/*RED*/
				ucRedColor = 255;
				ucGreenColor = 0;
				ucBlueColor = 0;
			}
			else
			{
				/*BLUE*/
				ucRedColor = 0;
				ucGreenColor = 0;
				ucBlueColor = 255;
			}
			fnGr_PutString(43, 380, ucrgStoredAlarmMessage[18], RGB(ucRedColor,ucGreenColor,ucBlueColor), 
											RGB(255,255,255), ORIENTATION_90,0);
			
			/*2nd notification*/
			if ( SYSTEM_ERROR == rgucAlarmTpe[19] )
			{
				/*RED*/
				ucRedColor = 255;
				ucGreenColor = 0;
				ucBlueColor = 0;
			}
			else
			{
				/*BLUE*/
				ucRedColor = 0;
				ucGreenColor = 0;
				ucBlueColor = 255;
			}
			fnGr_PutString(76, 380, ucrgStoredAlarmMessage[19], RGB(ucRedColor,ucGreenColor,ucBlueColor), 
											RGB(255,255,255), ORIENTATION_90,0);
			
			/*3rd notification*/
			if ( SYSTEM_ERROR == rgucAlarmTpe[20] )
			{
				/*RED*/
				ucRedColor = 255;
				ucGreenColor = 0;
				ucBlueColor = 0;
			}
			else
			{
				/*BLUE*/
				ucRedColor = 0;
				ucGreenColor = 0;
				ucBlueColor = 255;
			}
			fnGr_PutString(109, 380, ucrgStoredAlarmMessage[20], RGB(ucRedColor,ucGreenColor,ucBlueColor), 
											RGB(255,255,255), ORIENTATION_90,0);
			
			/*4th notification*/
			if ( SYSTEM_ERROR == rgucAlarmTpe[21] )
			{
				/*RED*/
				ucRedColor = 255;
				ucGreenColor = 0;
				ucBlueColor = 0;
			}
			else
			{
				/*BLUE*/
				ucRedColor = 0;
				ucGreenColor = 0;
				ucBlueColor = 255;
			}
			fnGr_PutString(142, 380, ucrgStoredAlarmMessage[21], RGB(ucRedColor,ucGreenColor,ucBlueColor), 
											RGB(255,255,255), ORIENTATION_90,0);
			
			/*5th notification*/
			if ( SYSTEM_ERROR == rgucAlarmTpe[22] )
			{
				/*RED*/
				ucRedColor = 255;
				ucGreenColor = 0;
				ucBlueColor = 0;
			}
			else
			{
				/*BLUE*/
				ucRedColor = 0;
				ucGreenColor = 0;
				ucBlueColor = 255;
			}
			fnGr_PutString(178, 380, ucrgStoredAlarmMessage[22], RGB(ucRedColor,ucGreenColor,ucBlueColor), 
											RGB(255,255,255), ORIENTATION_90,0);
			
			/*6th notification*/
			if ( SYSTEM_ERROR == rgucAlarmTpe[23] )
			{
				/*RED*/
				ucRedColor = 255;
				ucGreenColor = 0;
				ucBlueColor = 0;
			}
			else
			{
				/*BLUE*/
				ucRedColor = 0;
				ucGreenColor = 0;
				ucBlueColor = 255;
			}
			fnGr_PutString(211, 380, ucrgStoredAlarmMessage[23], RGB(ucRedColor,ucGreenColor,ucBlueColor), 
											RGB(255,255,255), ORIENTATION_90,0);
		}break;
		
		case SCREEN_CLEAR_FAULTS:
		{
			/*Screen title*/
			fnGr_PutString(9, 275, "CLEAR ALARMS", WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);

			/*Clear Errors*/
			fnGr_PutString(135, 175, "Clear Errors", RGB(0,0,0), LIGHT_GREY_CLR, ORIENTATION_90,0);

			/*Clear Wwarnings*/
			fnGr_PutString(200, 175, "Clear Warnings", RGB(0,0,0), LIGHT_GREY_CLR, ORIENTATION_90,0);

					/*OP*//*Nedd to implemet the OP Diagnostic*/
					fnStringColor_Set(StatusFlag.stStatusFlag.bHMI_Error);
					fnGr_PutString(43, 380, HMI_STRING, RGB(ucRedColor,ucGreenColor,ucBlueColor), 
													RGB(255,255,255), ORIENTATION_90,0);
					
					/*MCU*/
					if ( TRUE == StatusFlag.stStatusFlag.bSCU_Error )
					{
						fnStringColor_Set(StatusFlag.stStatusFlag.bSCU_Error);
					}
					else if ( TRUE == StatusFlag.stStatusFlag.bSCU_Warning )
					{
						ucRedColor = 0;
						ucGreenColor = 0;
						ucBlueColor = 255;
					}
					else
					{
						ucRedColor = 13;
						ucGreenColor = 185;
						ucBlueColor = 56;
					}
					fnGr_PutString(76, 380, SCU_STRING, RGB(ucRedColor,ucGreenColor,ucBlueColor), 
													RGB(255,255,255), ORIENTATION_90,0);
					
					/*OPMM*/
					if ( TRUE == StatusFlag.stStatusFlag.bOPMM_Error )
					{
						fnStringColor_Set(StatusFlag.stStatusFlag.bOPMM_Error);						
					}
					else if ( TRUE == StatusFlag.stStatusFlag.bOPMM_Warning )
					{
						ucRedColor = 0;
						ucGreenColor = 0;
						ucBlueColor = 255;
					}
					else
					{
						ucRedColor = 13;
						ucGreenColor = 185;
						ucBlueColor = 56;
					}
					fnGr_PutString(109, 380, OPMM_STRING, RGB(ucRedColor,ucGreenColor,ucBlueColor), 
													RGB(255,255,255), ORIENTATION_90,0);
					
					/*MFM*/
					if ( TRUE == StatusFlag.stStatusFlag.bVCM_Error )
					{
						fnStringColor_Set(StatusFlag.stStatusFlag.bVCM_Error);							
					}
					else if ( TRUE == StatusFlag.stStatusFlag.bVCM_Warning )
					{
						ucRedColor = 0;
						ucGreenColor = 0;
						ucBlueColor = 255;
					}
					else
					{
						ucRedColor = 13;
						ucGreenColor = 185;
						ucBlueColor = 56;
					}
					fnGr_PutString(142, 380, VCM_STRING, RGB(ucRedColor,ucGreenColor,ucBlueColor), 
													RGB(255,255,255), ORIENTATION_90,0);						
					
					/*HCM*/
					if ( TRUE == StatusFlag.stStatusFlag.bHCM_Error )
					{
						fnStringColor_Set(StatusFlag.stStatusFlag.bHCM_Error);						
					}
					else if ( TRUE == StatusFlag.stStatusFlag.bHCM_Warning )
					{
						ucRedColor = 0;
						ucGreenColor = 0;
						ucBlueColor = 255;
					}
					else
					{
						ucRedColor = 13;
						ucGreenColor = 185;
						ucBlueColor = 56;
					}
					fnGr_PutString(178, 380, HCM_STRING, RGB(ucRedColor,ucGreenColor,ucBlueColor), 
													RGB(255,255,255), ORIENTATION_90,0);
		}break;
		case SCREEN_KEYPAD :
		{
			uiXPos = 150;
			uiYPos = 386;
			for(ucRow = 1; ucRow <= 3; ucRow++)
			{
				for(ucColumn = 1; ucColumn <= 13 ; ucColumn++)
				{
					fnGr_PutString(uiXPos, uiYPos, ucAlphaNum[ucRow-1][ucColumn-1], RGB(0,0,0), 
													RGB(255,255,255), ORIENTATION_90,0);

					uiYPos -= 30; 
				}
				uiXPos += 30;
				uiYPos = 386;
			}
			
		}break;
		
		case SCREEN_ERROR_WARNING:
		{
			fnGr_SelectFont("Franklin24", 2);

			/*Add Checks for priority for Error nad Warning*/
			if ( TRUE == StatusFlag.stStatusFlag.bErrorFlag )
			{
				fnGr_PutString(110, 250, ERROR_STRING, RGB(255,255,255), RGB(255,21,15), ORIENTATION_90,0);
			}
			else	if ( TRUE == StatusFlag.stStatusFlag.bWarningFlag )
			{
				fnGr_PutString(110, 270, WARNING, RGB(255,255,255), RGB(45,45,255), ORIENTATION_90,0);
			}
			
			fnGr_SelectFont("Franklin24", 1);
			
			if ( TRUE == StatusFlag.stStatusFlag.bErrorFlag )
			{
				fnGr_PutString(20, 60, ALARMS, RGB(255,255,255), RGB(255,21,15), ORIENTATION_90,0);
			}
			if ( TRUE == StatusFlag.stStatusFlag.bWarningFlag )
			{
				fnGr_PutString(20, 60, ALARMS, RGB(255,255,255), RGB(45,45,255), ORIENTATION_90,0);
			}
			//fnGr_SelectFont("Times13", 1);
		}break;
		
		case SCREEN_REMOTE_REPLICA:
		{
			fnGr_PutString(9, 290, "REMOTE CONTROL", WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(60, 380, PRIME_STRING, RGB(0,0,0), 
														LIGHT_GREY_CLR, ORIENTATION_90,0);
			
			
			fnGr_PutString(60, 185, FLUSH_STRING , RGB(0,0,0), 
														LIGHT_GREY_CLR, ORIENTATION_90,0);
			
														
			fnGr_PutString(125, 380, SPRAY_STRING , RGB(0,0,0), 
											LIGHT_GREY_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(125, 185, PIPE_TEMP , RGB(0,0,0), 
														LIGHT_GREY_CLR, ORIENTATION_90,0);
														
		
			
			fnGr_PutString(190, 380, START , RGB(0,0,0), 
														LIGHT_GREY_CLR, ORIENTATION_90,0);
														
			fnGr_PutString(190, 185, STOP , RGB(0,0,0), 
														LIGHT_GREY_CLR, ORIENTATION_90,0);
		}break;
		
		case SCREEN_FIRMWARE_DETAILS:
		{
			fnGr_PutString(9, 300, "FIRMWARE DETAILS", WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(43, 380, "MODULE", BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			fnGr_PutString(43, 165, "VERSION", BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			
			/*SCU*/
			fnGr_PutString(76, 380, SCU_STRING, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			sprintf((char *)rgcTemp,"%0.1f",stCurrent_Value.fSCUCodeVersion);
			fnGr_PutString(76, 175, rgcTemp, BLACK_CLR, WHITE_CLR, ORIENTATION_90,1);
			
			/*OPMM*/
			fnGr_PutString(109, 380, OPMM_STRING, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			sprintf((char *)rgcTemp,"%0.1f",stCurrent_Value.fOPMMCodeVersion);
			fnGr_PutString(109, 175, rgcTemp, BLACK_CLR, WHITE_CLR, ORIENTATION_90,1);
			
			/*HCM*/
			fnGr_PutString(142, 380, HCM_STRING, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			sprintf((char *)rgcTemp,"%0.1f",stCurrent_Value.fHCMCodeVersion);
			fnGr_PutString(142, 175, rgcTemp, BLACK_CLR, WHITE_CLR, ORIENTATION_90,1);
			
			/*VCM*/
			fnGr_PutString(178, 380, VCM_STRING, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			sprintf((char *)rgcTemp,"%0.1f",stCurrent_Value.fVCMCodeVersion);
			fnGr_PutString(178, 175, rgcTemp, BLACK_CLR, WHITE_CLR, ORIENTATION_90,1);
			
			/*HMI*/
			fnGr_PutString(211, 380, HMI_STRING, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			sprintf((char *)rgcTemp,"%0.1f",CODE_VERSION);
			fnGr_PutString(211, 175, rgcTemp, BLACK_CLR, WHITE_CLR, ORIENTATION_90,1);
		}break;
		
		/*change3*/
		case SCREEN_ETH_IP_DETAILS:
		{
			
			fnLCDbmp(uiCursorXPos, uiCursorYPos,30,26,ucrgArrow);
			fnGr_PutString(9, 300, "ETHERNET DETAILS", WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			
			/*ETHERNET IP 160*/
			fnGr_PutString(43, 380, IP, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			sprintf((char *)rgcTemp,"%d.%d.%d.%d",stCurrent_Value.ucEthIPSlot1,stCurrent_Value.ucEthIPSlot2,
																			stCurrent_Value.ucEthIPSlot3,stCurrent_Value.ucEthIPSlot4);
			fnGr_PutString(43, 220, rgcTemp, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			
			/*ETHERNET Subnet Mask*/
			fnGr_PutString(76, 380, SUBNET, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			sprintf((char *)rgcTemp,"%d.%d.%d.%d",stCurrent_Value.ucEthSubnetSlot1,stCurrent_Value.ucEthSubnetSlot2,
																			stCurrent_Value.ucEthSubnetSlot3,stCurrent_Value.ucEthSubnetSlot4);
			fnGr_PutString(76, 220, rgcTemp, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			
				/*ETHERNET Gateway*/
			fnGr_PutString(109, 380, GATEWAY, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			sprintf((char *)rgcTemp,"%d.%d.%d.%d",stCurrent_Value.ucEthGWSlot1,stCurrent_Value.ucEthGWSlot2,
																			stCurrent_Value.ucEthGWSlot3,stCurrent_Value.ucEthGWSlot4);
			fnGr_PutString(109, 220, rgcTemp, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			
			/*ETHERNET Port*/
			fnGr_PutString(142, 380, PORT, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			sprintf((char *)rgcTemp,"%d",stCurrent_Value.iEthPort);
			fnGr_PutString(142, 220, rgcTemp, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			
			
			
		}break;
		/*change3*/
		case SCREEN_WIFI_IP_DETAILS:
		{
			fnLCDbmp(uiCursorXPos, uiCursorYPos,30,26,ucrgArrow);
			fnGr_PutString(9, 275, "Wi-Fi DETAILS", WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			
			
			/*WIFI IP*/
			fnGr_PutString(43, 380, IP, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			sprintf((char *)rgcTemp,"%d.%d.%d.%d",stCurrent_Value.ucWiFiIPSlot1,stCurrent_Value.ucWiFiIPSlot2,
																			stCurrent_Value.ucWiFiIPSlot3,stCurrent_Value.ucWiFiIPSlot4);
			fnGr_PutString(43, 220, rgcTemp, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			
			/*WIFI Subnet Mask*/
			fnGr_PutString(76, 380, SUBNET, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			sprintf((char *)rgcTemp,"%d.%d.%d.%d",stCurrent_Value.ucWiFiSubnetSlot1,stCurrent_Value.ucWiFiSubnetSlot2,
																			stCurrent_Value.ucWiFiSubnetSlot3,stCurrent_Value.ucWiFiSubnetSlot4);
			fnGr_PutString(76, 220, rgcTemp, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			
				/*WIFI Gateway*/
			fnGr_PutString(109, 380, GATEWAY, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			sprintf((char *)rgcTemp,"%d.%d.%d.%d",stCurrent_Value.ucWiFiGWSlot1,stCurrent_Value.ucWiFiGWSlot2,
																			stCurrent_Value.ucWiFiGWSlot3,stCurrent_Value.ucWiFiGWSlot4);
			fnGr_PutString(109, 220, rgcTemp, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			
			/*WIFI Port*/
			fnGr_PutString(142, 380, PORT, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			sprintf((char *)rgcTemp,"%d",stCurrent_Value.iWiFiPort);
			fnGr_PutString(142, 220, rgcTemp, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			
			/*WIFI SSID*/
			fnGr_PutString(175, 380, SSID, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			fnGr_PutString(175, 220, rgucSSID, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			/*WIFI PASSWORD*/
			fnGr_PutString(208, 380, PASSWORD, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			fnGr_PutString(208, 220, rgucPass, BLACK_CLR, WHITE_CLR, ORIENTATION_90,0);
			
			fnGr_PutString(208, 50, APPLY_BUTTON, WHITE_CLR , 
			DARK_GREY_CLR, ORIENTATION_90,0);
		}break;
/*change4*/
		case SCREEN_MEMORY_ALARM_1:
		{
			/*Screen title*/
			fnGr_PutString(9, 250, MEMORY_ALARMS_1, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			
			fnGr_SelectFont("Franklin20", 1);/*change5*/
			/*1st notification*/
			fnGr_PutString(43, 380, ucrgStoredMemoryAlarmMessage[0], RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			
			/*2nd notification*/
			fnGr_PutString(76, 380, ucrgStoredMemoryAlarmMessage[1], RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			
			/*3rd notification*/
			fnGr_PutString(109, 380, ucrgStoredMemoryAlarmMessage[2], RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			
			/*4th notification*/
			fnGr_PutString(142, 380, ucrgStoredMemoryAlarmMessage[3], RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			
			/*5th notification*/
			fnGr_PutString(178, 380, ucrgStoredMemoryAlarmMessage[4], RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			
			/*6th notification*/
			fnGr_PutString(211, 380, ucrgStoredMemoryAlarmMessage[5], RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			fnGr_PutString(211, 33, ACK_BUTTON, WHITE_CLR , 
			DARK_GREY_CLR, ORIENTATION_90,0);
		}break;
				case SCREEN_MEMORY_ALARM_2:
		{
			/*Screen title*/
			fnGr_PutString(9, 250, MEMORY_ALARMS_2, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			
			fnGr_SelectFont("Franklin20", 1);/*change5*/
			/*1st notification*/
			fnGr_PutString(43, 380, ucrgStoredMemoryAlarmMessage[6], RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			
			/*2nd notification*/
			fnGr_PutString(76, 380, ucrgStoredMemoryAlarmMessage[7], RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			
			/*3rd notification*/
			fnGr_PutString(109, 380, ucrgStoredMemoryAlarmMessage[8], RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			
			/*4th notification*/
			fnGr_PutString(142, 380, ucrgStoredMemoryAlarmMessage[9], RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			
			/*5th notification*/
			fnGr_PutString(178, 380, ucrgStoredMemoryAlarmMessage[10], RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			
			/*6th notification*/
			fnGr_PutString(211, 380, ucrgStoredMemoryAlarmMessage[11], RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			fnGr_PutString(211, 33, ACK_BUTTON, WHITE_CLR , 
			DARK_GREY_CLR, ORIENTATION_90,0);
		}break;
				case SCREEN_MEMORY_ALARM_3:
		{
			/*Screen title*/
			fnGr_PutString(9, 250, MEMORY_ALARMS_3, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			
			fnGr_SelectFont("Franklin20", 1);/*change5*/
			/*1st notification*/
			fnGr_PutString(43, 380, ucrgStoredMemoryAlarmMessage[12], RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			
			/*2nd notification*/
			fnGr_PutString(76, 380, ucrgStoredMemoryAlarmMessage[13], RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			
			/*3rd notification*/
			fnGr_PutString(109, 380, ucrgStoredMemoryAlarmMessage[14], RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			
			/*4th notification*/
			fnGr_PutString(142, 380, ucrgStoredMemoryAlarmMessage[15], RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			
			/*5th notification*/
			fnGr_PutString(178, 380, ucrgStoredMemoryAlarmMessage[16], RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			
			/*6th notification*/
			fnGr_PutString(211, 380, ucrgStoredMemoryAlarmMessage[17], RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			fnGr_PutString(211, 33, ACK_BUTTON, WHITE_CLR , 
			DARK_GREY_CLR, ORIENTATION_90,0);
		}break;
				case SCREEN_MEMORY_ALARM_4:
		{
			/*Screen title*/
			fnGr_PutString(9, 250, MEMORY_ALARMS_4, WHITE_CLR, BLUE_CLR, ORIENTATION_90,0);
			
			fnGr_SelectFont("Franklin20", 1);/*change5*/
			/*1st notification*/
			fnGr_PutString(43, 380, ucrgStoredMemoryAlarmMessage[18], RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			
			/*2nd notification*/
			fnGr_PutString(76, 380, ucrgStoredMemoryAlarmMessage[19], RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			
			/*3rd notification*/
			fnGr_PutString(109, 380, ucrgStoredMemoryAlarmMessage[20], RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			
			/*4th notification*/
			fnGr_PutString(142, 380, ucrgStoredMemoryAlarmMessage[21], RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			
			/*5th notification*/
			fnGr_PutString(178, 380, ucrgStoredStatusMessage[22], RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			
			/*6th notification*/
			fnGr_PutString(211, 380, ucrgStoredMemoryAlarmMessage[23], RGB(0,0,0), RGB(255,255,255), ORIENTATION_90,0);
			fnGr_PutString(211, 33, ACK_BUTTON, WHITE_CLR , 
			DARK_GREY_CLR, ORIENTATION_90,0);
		}break;
	}
	memset(rgcTemp,'\0', sizeof(rgcTemp));
}

/*****************************************************************************
**@Function		 	:  fnFlashMessage_Display()
**@Descriptions	:  Function to Display Flash Message
**@parameters		:  pucStr : Flash message
**@return				:  none
*****************************************************************************/
void fnFlashMessage_Display(uint8_t *pucStr)
{
	if ( 1000 > uliFlashTimeCounter )
	{
		fnGr_ThickRectangle(130, 85, 40, 228, RGB(0,0,0), 1);
		fnGr_FillRectangle(131, 86, 38,226,RGB(255,140,140));
		fnGr_PutString(140, 270, pucStr, RGB(0,0,0), RGB(255,140,140), ORIENTATION_90,0);
	}
	else if ( 300 < uliFlashTimeCounter )
	{
		if ( 1300 < uliFlashTimeCounter )
		{
			uliFlashTimeCounter = 0;
		}
		fnGr_ThickRectangle(130, 85, 40, 228, RGB(255,255,255), 1);
		fnGr_FillRectangle(131, 86, 38,226,RGB(255,255,255));
	}
}
/*****************************************************************************
**@Function		 	:  fnStringColor_Set()
**@Descriptions	:  Function to set string color for Diagnostic Status
**@parameters		:  ucFlagStatus : Diagnostic Status
**@return				:  none
*****************************************************************************/
void fnStringColor_Set(uint8_t ucFlagStatus)
{
	if ( FALSE == ucFlagStatus)
	{
		/*GREEN*/
		ucRedColor = 13;
		ucGreenColor = 185;
		ucBlueColor = 56;
	}
	else if( TRUE == ucFlagStatus)
	{
		/*RED*/
		ucRedColor = 255;
		ucGreenColor = 0;
		ucBlueColor = 0;
	}else
	{
		/*GREY*/
		ucRedColor = 211;
		ucGreenColor = 211;
		ucBlueColor = 211;
	}
}
/*****************************************************************************
**@Function		 	:  fnCurrentScreen_Pattern()
**@Descriptions	:  Current screen pattern
**@parameters		:  none
**@return				:  none
*****************************************************************************/
void fnCurrentScreen_Pattern(void)
{
	if(TRUE == ucSwitchPressFlag)
	{
		ucSwitchPressFlag = FALSE;
		
		fnDisplay_Reg_Write(0x007, 0x0000);
		fnDisplay_Clear_Screen(RGB(255,255,255));
		fnGR_SCREEN1(ucScreenPatternNumber);
		fnDisplay_Reg_Write(0x007, 0x0173);
		
		if ( SCREEN_KEYPAD == uiCurrentScreen )		
		{
			fnGr_FillRectangle(uiCursorXPosKP, uiCursorYPosKP, 26,28,RGB(106,255,102));
		}	
	}
}

/*****************************************************************************
**@Function		 	:  fnLCDbmp()
**@Descriptions	:  To draw bitmap image on screen
**@parameters		:  	uliXAxis	:	x-position
										uliYAxis	:	y-position
										uliWidth	:	width of image
										uliHeight	:	height of image
										ucrgImage	:	image buffer
**@return				:  none
*****************************************************************************/
void fnLCDbmp(uint32_t uliXAxis,uint32_t uliYAxis,uint32_t uliWidth,uint32_t uliHeight,uint8_t *ucrgImage)
{
	uint32_t uliHLoop, uliWLoop, uliILoop=0;
	uint32_t uliOrientation = 0;

	fnDisplay_Get_Geometry(NULL, NULL, &uliOrientation);
	fnDisplay_RAM_Address_Oriented_to_Physical(&uliXAxis, &uliYAxis);
	fnDisplay_Set_Orientation((uliOrientation + 3) % 4);
	fnDisplay_RAM_Address_Physical_to_Oriented(&uliXAxis, &uliYAxis);


	for(uliHLoop=0;uliHLoop<uliHeight;uliHLoop++)
	{
		for(uliWLoop=0;uliWLoop<uliWidth;uliWLoop++)
		{
				fnLCDDrawPixel(uliWLoop+uliXAxis, uliHLoop+uliYAxis, RGB(ucrgImage[uliILoop], ucrgImage[uliILoop+1], ucrgImage[uliILoop+2]));
				uliILoop = uliILoop + 2;
		}
	}
	fnDisplay_Set_Orientation(uliOrientation);
}

/*****************************************************************************
**@Function		 	:  fnLCDDrawPixel()
**@Descriptions	:  To draw pixel on screen
**@parameters		:  	uliXAxis	:	x-position
										uliYAxis	:	y-position
										uicolor		:	pixel color
**@return				:  none
*****************************************************************************/
void fnLCDDrawPixel(uint32_t uliXAxis, uint32_t uliYAxis, uint16_t uiColor)
{
	fnDisplay_Set_RAM_Address(uliXAxis, uliYAxis);
  fnDisplay_Data_Write(uiColor);
}

/*****************************************************************************
**@Function		 	:  fnGrBuzzerVolume()
**@Descriptions	:  Buzzer Volume Graphics
**@parameters		:  none
**@return				:  none
*****************************************************************************/
void fnGr_BuzzerVolume()
{
	uint8_t ucLoop = 0;
	fnGr_Rectangle( 211,  48,  25, 306,  BLACK_CLR);
	if( TRUE == StatusFlag.stStatusFlag.bBuzzerVolDecrease )
	{
		StatusFlag.stStatusFlag.bBuzzerVolDecrease = FALSE;
		fnGr_FillRectangle(214, (323 - ( stConfig_Value.ucBuzzerVolume * 30 )), 
												19, 26, WHITE_CLR);
	}
	else
	{
		for(ucLoop = 0; ucLoop < stConfig_Value.ucBuzzerVolume ; ucLoop++)
		{
			fnGr_FillRectangle(214, (323 - ( ucLoop * 30 )), 19, 26, RGB(106,255,102));
		}
	}
}

//for(ucLoop = 9; ucLoop >= 0 ; ucLoop--)
//	{
//		fnGr_FillRectangle(214, (53 + ( i * 30 )), 19,26,RGB(106,255,102));
//	}



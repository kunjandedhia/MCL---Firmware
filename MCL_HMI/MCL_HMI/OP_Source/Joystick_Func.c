/******************************************************************************
**@file    I2C_Func.c
* 
**@brief   This file contains all the functions related to GPIO expander and I2C
******************************************************************************
*/    
/* ----------------------------------------------------------------------------
*                           Includes
* ----------------------------------------------------------------------------
*/
#include "System_Header.h"
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
uint8_t ucCurrentJoystick = 0x00;
JOYS_TypeDef stJoy_Flag;
bool bInit_Complete_Jo = FALSE;
uint32_t uliTick_Counter_Jo;
uint32_t uliLast_Service_Tick_Jo;
bool bService_Flag_Jo;
bool bJoyFlag = FALSE;
uint32_t rguliJs_Current[NUMBER_OF_JOYSTICKS];
uint32_t rguliJs_Reported[NUMBER_OF_JOYSTICKS];
uint32_t rguliPrev_Js_Reported[NUMBER_OF_JOYSTICKS];
uint32_t rguliSamples_Since_Last_Offcenter[NUMBER_OF_JOYSTICKS];
uint32_t rguliCal_Center[NUMBER_OF_JOYSTICKS];
uint32_t rguliCal_Accum[NUMBER_OF_JOYSTICKS];
uint32_t rguliCal_Count[NUMBER_OF_JOYSTICKS];
uint8_t  ucJ_Bouncingcounter=0;
uint8_t  ucJ_DeBouncingcounter=0;
uint8_t  ucCurrJoystickStatus=0;
uint8_t  ucJ_Backup=0;
bool bInit_Complete_Ad = FALSE;
uint32_t uliTick_Counter_Ad;
uint32_t uliLast_Service_Tick_Ad;
bool bService_Flag_Ad;
uint16_t rguiAvg_Buffer[4][AVG_HISTORY_LENGTH];
uint32_t uliAvg_Buf_Index;
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

/*****************************************************************************
**@Function		 	:  fnJS_Initialize
**@Descriptions	:	 Initialize the Joystick Pins
**@parameters		:  none
**@return				:  none
*****************************************************************************/
void fnJS_Initialize(void)
{
	GPIO_InitTypeDef stGPIO_InitStructure_JOY;

	uliTick_Counter_Jo = 0;
	uliLast_Service_Tick_Jo = 0;
	bService_Flag_Jo = FALSE;
	memset(rguliJs_Current, 0, sizeof(rguliJs_Current));
	memset(rguliJs_Reported, 0, sizeof(rguliJs_Reported));
	memset(rguliPrev_Js_Reported, 0, sizeof(rguliPrev_Js_Reported));
	memset(rguliSamples_Since_Last_Offcenter, 0, sizeof(rguliSamples_Since_Last_Offcenter));
	memset(rguliCal_Center, 0, sizeof(rguliCal_Center));
	memset(rguliCal_Accum, 0, sizeof(rguliCal_Accum));
	memset(rguliCal_Count, 0, sizeof(rguliCal_Count));

	/* set up Mux A0 pin */
	stGPIO_InitStructure_JOY.GPIO_Speed = GPIO_Speed_50MHz;
	stGPIO_InitStructure_JOY.GPIO_Mode = GPIO_Mode_Out_PP;
	stGPIO_InitStructure_JOY.GPIO_Pin = MUX_A0_PIN_J;
	GPIO_Init(MUX_A0_GPIO_J, &stGPIO_InitStructure_JOY);

	/* set up Mux A1 pin */
	stGPIO_InitStructure_JOY.GPIO_Speed = GPIO_Speed_50MHz;
	stGPIO_InitStructure_JOY.GPIO_Mode = GPIO_Mode_Out_PP;
	stGPIO_InitStructure_JOY.GPIO_Pin = MUX_A1_PIN_J;
	GPIO_Init(MUX_A1_GPIO_J, &stGPIO_InitStructure_JOY);

	/* set up DN pin */
	stGPIO_InitStructure_JOY.GPIO_Speed = GPIO_Speed_50MHz;
	stGPIO_InitStructure_JOY.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	stGPIO_InitStructure_JOY.GPIO_Pin = DN_PIN_J;
	GPIO_Init(DN_GPIO_J, &stGPIO_InitStructure_JOY);

	/* set up UP pin */
	stGPIO_InitStructure_JOY.GPIO_Speed = GPIO_Speed_50MHz;
	stGPIO_InitStructure_JOY.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	stGPIO_InitStructure_JOY.GPIO_Pin = UP_PIN_J;
	GPIO_Init(UP_GPIO_J, &stGPIO_InitStructure_JOY);

	bInit_Complete_Jo = TRUE;
}
/*****************************************************************************
**@Function		 	:  fnJS_Read_Switch_States
**@Descriptions	:	 Reading switch states of joystick
**@parameters		:  bState : switch state
**@return				:  none
*****************************************************************************/
void fnJS_Read_Switch_States(bool *bState)
{
	uint32_t uliNofJ;
	for ( uliNofJ = 0 ; uliNofJ < NUMBER_OF_JOYSTICKS ; uliNofJ++ )
	{
		GPIO_WriteBit(MUX_A0_GPIO_J, MUX_A0_PIN_J, (uliNofJ & 1) ? Bit_SET : Bit_RESET);
		GPIO_WriteBit(MUX_A1_GPIO_J, MUX_A1_PIN_J, ((uliNofJ >> 1) & 1) ? Bit_SET : Bit_RESET);
		Dly1us();
		Dly1us();
		bState[uliNofJ * 2] = (bool)GPIO_ReadInputDataBit(DN_GPIO_J, DN_PIN_J);
		bState[uliNofJ * 2 + 1] = (bool)GPIO_ReadInputDataBit(UP_GPIO_J, UP_PIN_J);
	}
}		
/*****************************************************************************
**@Function		 	:  fnJS_Foreground_Service
**@Descriptions	:	 Scanning joystick position
**@parameters		:  none
**@return				:  none
*****************************************************************************/
void fnJS_Foreground_Service(void)
{
	uint16_t uirgAdcVal[NUMBER_OF_JOYSTICKS];
	bool brgSwState[NUMBER_OF_JOYSTICKS * 2];
	uint32_t uliNofJs;

	fnJS_Read_Switch_States(brgSwState);
	fnADC_Get_Avg_Readings(uirgAdcVal, 0);
	/*Requires Optimization for this checking*/
	for ( uliNofJs = 0 ; uliNofJs < NUMBER_OF_JOYSTICKS ; uliNofJs++ )
	{
		if ( OFF_CENTER(uliNofJs) )
		{
			rguliSamples_Since_Last_Offcenter[uliNofJs] = 0;
		}
		else
		{
			if ( JSCAL_HISTORY_LENGTH > rguliSamples_Since_Last_Offcenter[uliNofJs] )
				rguliSamples_Since_Last_Offcenter[uliNofJs]++;
		if ( JSCAL_HISTORY_MARGIN <= rguliSamples_Since_Last_Offcenter[uliNofJs] )
			{
			if ( 32767 > rguliCal_Count[uliNofJs] )
				{
				rguliCal_Count[uliNofJs]++;
					rguliCal_Accum[uliNofJs] += uirgAdcVal[uliNofJs];
					rguliCal_Center[uliNofJs] = rguliCal_Accum[uliNofJs] / rguliCal_Count[uliNofJs];
				}
			}
		}
		rguliJs_Current[uliNofJs] = uirgAdcVal[uliNofJs] - rguliCal_Center[uliNofJs];
	}

	for ( uliNofJs = 0 ; uliNofJs < NUMBER_OF_JOYSTICKS ; uliNofJs++ )
	{
		if ( ( 0 == uliNofJs ) && (( ( 1500 < rguliJs_Current[uliNofJs] ) ) && (( 2200 > rguliJs_Current[uliNofJs]) )))
		{
			ucCurrJoystickStatus = JOYSTICK_RIGHT_2;
			break;			
		}
		else if ( ( 0 == uliNofJs ) && ((( 63600 < rguliJs_Current[uliNofJs]) ) && (( 64600 > rguliJs_Current[uliNofJs]) )))
		{
					ucCurrJoystickStatus = JOYSTICK_LEFT_2;
			break;
		}
				
		else if ( ( 1 == uliNofJs ) && (( ( 1500 < rguliJs_Current[uliNofJs]) ) && (( 2500 > rguliJs_Current[uliNofJs]) )))
		{
			ucCurrJoystickStatus = JOYSTICK_UP_1;
			break;
		}
		else if ( ( 1 == uliNofJs ) && ((( 63616 < rguliJs_Current[uliNofJs])) && (( 65035 > rguliJs_Current[uliNofJs]) )))
		{
			ucCurrJoystickStatus = JOYSTICK_DOWN_1;
			break;
		}
		else if ( ( 1 == uliNofJs ) && (((( 5 < rguliJs_Current[uliNofJs]) )&&(( 100 > rguliJs_Current[uliNofJs]) )) || 
							((( 65035 < rguliJs_Current[uliNofJs]) ) && (( 65540 > rguliJs_Current[uliNofJs]) )))&& 
							( TRUE == StatusFlag.stStatusFlag.bJS1_CenterDetected ))
		{	
			ucCurrentJoystick = JOYSTICK_CENTER_1;
			break;
		}
		else if ( ( 2 == uliNofJs ) && ((( 500 < rguliJs_Current[uliNofJs]) ) && (( 2000 > rguliJs_Current[uliNofJs]) )))
		{
			ucCurrJoystickStatus = JOYSTICK_RIGHT_3;
			break;
		}
		else if ( ( 2 == uliNofJs ) && ((( 63524 < rguliJs_Current[uliNofJs]) ) && (( 65254 > rguliJs_Current[uliNofJs]) )))
		{
			ucCurrJoystickStatus = JOYSTICK_LEFT_3;
			break;
		}
		else 	if ( ( 3 == uliNofJs ) && ((( 500 < rguliJs_Current[uliNofJs]) ) && (( 3000 > rguliJs_Current[uliNofJs]) )))
		{
			ucCurrJoystickStatus = JOYSTICK_UP_4;
   	break;
		}
		else if ( (3 == uliNofJs ) && ((( 63524 < rguliJs_Current[uliNofJs]) ) && (( 65026 > rguliJs_Current[uliNofJs]) )))
		{
			ucCurrJoystickStatus = JOYSTICK_DOWN_4;
   	break;
		}
		else if ( ( 3 == uliNofJs ) && (((( 65026 < rguliJs_Current[uliNofJs]) ) && 
							(( 65535 > rguliJs_Current[uliNofJs]) )) || ((( 1 <= rguliJs_Current[uliNofJs]) ) &&
							(( 100 > rguliJs_Current[uliNofJs]) ))) &&  ( TRUE == StatusFlag.stStatusFlag.bJS4_CenterDetected ))
		{
			ucCurrentJoystick = JOYSTICK_CENTER_4;			
			break;
		}
		else
		{
			ucCurrJoystickStatus = 0x00;
		}
	}

	if(0 != ucCurrJoystickStatus)
	{
		if( 0 == ucJ_Bouncingcounter )
		{
			ucJ_Bouncingcounter++;
			ucJ_Backup = ucCurrJoystickStatus;
		}
		else if(FALSE == bJoyFlag)
		{
			ucJ_Bouncingcounter++;
			if( 30 <= ucJ_Bouncingcounter )
			{
				if ( ucJ_Backup == ucCurrJoystickStatus )
				{
					#ifdef JOYSTICK_DBG
						DEBUG_PRINTF("\nIn ucJ_Backup == ucCurrJoystickStatus=%d\r\n",ucCurrJoystickStatus);
					#endif
					if( JOYSTICK_UP_1 == ucCurrJoystickStatus )
					{
						#ifdef JOYSTICK_DBG
							DEBUG_PRINTF("STIFF UP POSITION\r\n");
						#endif
						ucCurrentJoystick = JOYSTICK_UP_1;
					}
					else if ( JOYSTICK_DOWN_1 == ucCurrJoystickStatus )
					{
						#ifdef JOYSTICK_DBG
							DEBUG_PRINTF("STIFF DOWN POSITION\r\n");
						#endif
						ucCurrentJoystick = JOYSTICK_DOWN_1;
					}
					else if ( JOYSTICK_RIGHT_3 == ucCurrJoystickStatus )
					{
						#ifdef JOYSTICK_DBG
							DEBUG_PRINTF("PIPE TRAVEL RIGHT POSITION\r\n");
						#endif
						ucCurrentJoystick = JOYSTICK_RIGHT_3;
					}
					else if  (JOYSTICK_LEFT_3 == ucCurrJoystickStatus )
					{
						#ifdef JOYSTICK_DBG
							DEBUG_PRINTF("PIPE TRAVEL LEFT POSITION\r\n");
						#endif
						ucCurrentJoystick = JOYSTICK_LEFT_3;
					}
					else if ( JOYSTICK_LEFT_2 == ucCurrJoystickStatus )
					{
						#ifdef JOYSTICK_DBG
							DEBUG_PRINTF("MANDREL LEFT POSITION\r\n");
						#endif
						ucCurrentJoystick = JOYSTICK_LEFT_2;
					}
					else if ( JOYSTICK_RIGHT_2 == ucCurrJoystickStatus )
					{
						ucCurrentJoystick = JOYSTICK_RIGHT_2;
						#ifdef JOYSTICK_DBG
							DEBUG_PRINTF("MANDREL RIGHT POSITION\r\n");
						#endif
					}
					else if ( JOYSTICK_UP_4 == ucCurrJoystickStatus )
					{
						#ifdef JOYSTICK_DBG
							DEBUG_PRINTF("PIN UP POSITION\r\n");
						#endif
						ucCurrentJoystick = JOYSTICK_UP_4;
					}
					else if ( JOYSTICK_DOWN_4 == ucCurrJoystickStatus )
					{
						#ifdef JOYSTICK_DBG
							DEBUG_PRINTF("PIN DOWN POSITION\r\n");
						#endif
						ucCurrentJoystick = JOYSTICK_DOWN_4;
					}
					else if ( JOYSTICK_CENTER_4 == ucCurrJoystickStatus )
					{
						#ifdef JOYSTICK_DBG
							DEBUG_PRINTF("PIN CENTER POSITION\r\n");
						#endif
						ucCurrentJoystick = JOYSTICK_CENTER_4;
					}
					ucJ_Bouncingcounter = 0;
					bJoyFlag= TRUE;
					ucJ_DeBouncingcounter = 1;
				}
			}
		}
	}
	else
	{
		if ( TRUE == bJoyFlag )
		{
			ucJ_DeBouncingcounter++;
			if(	30 <= ucJ_DeBouncingcounter )
			{
				ucJ_DeBouncingcounter=0;
				bJoyFlag = FALSE;
				ucCurrJoystickStatus=0x00;
				ucJ_Bouncingcounter = 0;
			}	
		}
		else
		{
				ucJ_DeBouncingcounter=0;
				bJoyFlag= FALSE;
				ucJ_Bouncingcounter = 0;
		}
	}
}
/*****************************************************************************
**@Function		 	:  fnADC_Initialize
**@Descriptions	:	 initialize ADC
**@parameters		:  none
**@return				:  none
*****************************************************************************/
void fnADC_Initialize(void)
{
	GPIO_InitTypeDef stGPIO_InitStructure_ADC;
	SPI_InitTypeDef stSPI_InitStructure_ADC;
	volatile uint16_t vuiDummy;

	uliTick_Counter_Ad = 0;
	uliLast_Service_Tick_Ad = 0;
	bService_Flag_Ad = FALSE;
	memset(rguiAvg_Buffer, 0, sizeof(rguiAvg_Buffer));
	uliAvg_Buf_Index = 0;

	/* set clock and reset states */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1, DISABLE);

	/* set up MISO pin */
	stGPIO_InitStructure_ADC.GPIO_Speed = GPIO_Speed_50MHz;
	stGPIO_InitStructure_ADC.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	stGPIO_InitStructure_ADC.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOA, &stGPIO_InitStructure_ADC);

	/* set up SCLK pin */
	stGPIO_InitStructure_ADC.GPIO_Speed = GPIO_Speed_50MHz;
	stGPIO_InitStructure_ADC.GPIO_Mode = GPIO_Mode_AF_PP;
	stGPIO_InitStructure_ADC.GPIO_Pin = GPIO_Pin_5;
	GPIO_Init(GPIOA, &stGPIO_InitStructure_ADC);

	/* set up CS pin */
	stGPIO_InitStructure_ADC.GPIO_Speed = GPIO_Speed_50MHz;
	stGPIO_InitStructure_ADC.GPIO_Mode = GPIO_Mode_Out_PP;
	stGPIO_InitStructure_ADC.GPIO_Pin = CS_PIN_A;
	GPIO_Init(CS_GPIO_A, &stGPIO_InitStructure_ADC);

	/* set up A0 pin */
	stGPIO_InitStructure_ADC.GPIO_Speed = GPIO_Speed_50MHz;
	stGPIO_InitStructure_ADC.GPIO_Mode = GPIO_Mode_Out_PP;
	stGPIO_InitStructure_ADC.GPIO_Pin = MUX_A0_PIN_A;
	GPIO_Init(MUX_A0_GPIO_A, &stGPIO_InitStructure_ADC);

	/* set remapping (not remapped) */
	GPIO_PinRemapConfig(GPIO_Remap_SPI1, DISABLE);

	/* initialize SPI machine */
	stSPI_InitStructure_ADC.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	stSPI_InitStructure_ADC.SPI_Mode = SPI_Mode_Master;
	stSPI_InitStructure_ADC.SPI_DataSize = SPI_DataSize_16b;
	stSPI_InitStructure_ADC.SPI_CPOL = SPI_CPOL_High;
	stSPI_InitStructure_ADC.SPI_CPHA = SPI_CPHA_1Edge;
	stSPI_InitStructure_ADC.SPI_NSS = SPI_NSS_Soft;
	stSPI_InitStructure_ADC.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
	stSPI_InitStructure_ADC.SPI_FirstBit = SPI_FirstBit_MSB;
	stSPI_InitStructure_ADC.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1, &stSPI_InitStructure_ADC);

	/* enable SPI machine */
	SPI_Cmd(SPI1, ENABLE);

	/* clear out receiver */
	while ( SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) )
	{
		vuiDummy = SPI_I2S_ReceiveData(SPI1);
	}

	bInit_Complete_Ad = TRUE;
}
/*****************************************************************************
**@Function		 	:  fnADC_Read
**@Descriptions	:	 ADC read
**@parameters		:  puiResult : pointer to store ADC value
**@return				:  none
*****************************************************************************/
void fnADC_Read(uint16_t *puiResult)
{
	volatile uint16_t vuiDummy;
	uint32_t uinoj;

	/* clear out receiver (avoid possible loss of sync) */
	while ( SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) )
	{
		vuiDummy = SPI_I2S_ReceiveData(SPI1);
	}

	/* A0 high */
	GPIO_WriteBit(MUX_A0_GPIO_A, MUX_A0_PIN_A, Bit_SET);
	/* CS low */
	GPIO_WriteBit(CS_GPIO_A, CS_PIN_A, Bit_RESET);
 
	/* first result word */
	/* send dummy word (necessary to operate SCLK) */
	SPI_I2S_SendData(SPI1, 0);
	/* wait for word in receiver */
	while ( !SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) ) ;
	// get received word
	puiResult[0] = (uint16_t)SPI_I2S_ReceiveData(SPI1);

	/* second result word */
	/* send dummy word (necessary to operate SCLK)*/
	SPI_I2S_SendData(SPI1, 0);
	/* wait for word in receiver */
	while ( !SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) ) ;
	/* get received word */
	puiResult[2] = (uint16_t)SPI_I2S_ReceiveData(SPI1);

	/* CS high */
	GPIO_WriteBit(CS_GPIO_A, CS_PIN_A, Bit_SET);

	/* A0 low */
	GPIO_WriteBit(MUX_A0_GPIO_A, MUX_A0_PIN_A, Bit_RESET);
	/* CS low */
	GPIO_WriteBit(CS_GPIO_A, CS_PIN_A, Bit_RESET);

	/* third result word */
	/* send dummy word (necessary to operate SCLK) */
	SPI_I2S_SendData(SPI1, 0);
	/* wait for word in receiver */
	while ( !SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) ) ;
	/* get received word */
	puiResult[1] = (uint16_t)SPI_I2S_ReceiveData(SPI1);

	/* fourth result word */
	/* send dummy word (necessary to operate SCLK)*/
	SPI_I2S_SendData(SPI1, 0);
	/* wait for word in receiver */
	while ( !SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) ) ;
	/* get received word */
	puiResult[3] = (uint16_t)SPI_I2S_ReceiveData(SPI1);

	/* CS high */
	GPIO_WriteBit(CS_GPIO_A, CS_PIN_A, Bit_SET);

	for ( uinoj = 0 ; uinoj < 4 ; uinoj++ )
	{
		puiResult[uinoj] &= (uint16_t)((1 << ADC_BITS) - 1);    /* mask out all but analog value */
		if ( puiResult[uinoj] & (uint16_t)(1 << (ADC_BITS - 1)) )    /* if negative */
			puiResult[uinoj] = puiResult[uinoj] - (uint16_t)(1 << ADC_BITS);
	}
}
/*****************************************************************************
**@Function		 	:  fnADC_Foreground_Service
**@Descriptions	:	 ADC scanning
**@parameters		:  none
**@return				:  none
*****************************************************************************/
void fnADC_Foreground_Service(void)
{
	uint16_t rguiAdcVal[4];
	uint32_t uliNofJ;

	fnADC_Read(rguiAdcVal);
	for ( uliNofJ = 0 ; uliNofJ < 4 ; uliNofJ++ )
	{
		rguiAvg_Buffer[uliNofJ][uliAvg_Buf_Index] = rguiAdcVal[uliNofJ];
	}
	uliAvg_Buf_Index = (uliAvg_Buf_Index + 1) % AVG_HISTORY_LENGTH;
}
/*****************************************************************************
**@Function		 	:  fnADC_Get_Avg_Readings
**@Descriptions	:	 Average reading 
**@parameters		:  puiResult : pointer to store ADC value
									 uiAvg_count : average count
**@return				:  none
*****************************************************************************/
void fnADC_Get_Avg_Readings(uint16_t *puiResult, uint32_t uiAvg_count)
{
	uint32_t uliCh;
	uint32_t uliNofJ;
	uint32_t rgulirgSum[4];
	uint32_t uliIndex;

	if (( AVG_HISTORY_LENGTH < uiAvg_count ) || ( 0 == uiAvg_count ))
			uiAvg_count = AVG_HISTORY_LENGTH;
	
	uliIndex = (uliAvg_Buf_Index + AVG_HISTORY_LENGTH - uiAvg_count) % AVG_HISTORY_LENGTH;
	memset(rgulirgSum, 0, sizeof(rgulirgSum));
	for ( uliNofJ = 0 ; uliNofJ < uiAvg_count ; uliNofJ++ )
	{
		for ( uliCh = 0 ; uliCh < 4 ; uliCh++ )
			rgulirgSum[uliCh] += (uint32_t)rguiAvg_Buffer[uliCh][uliIndex];
		uliIndex = (uliIndex + 1) % AVG_HISTORY_LENGTH;
	}
	for ( uliCh = 0 ; uliCh < 4 ; uliCh++ )
		puiResult[uliCh] = (uint16_t)(rgulirgSum[uliCh] / uiAvg_count);
}

/*****************************************************************************
**@Function		 	:  fnCheckJoystick_Press()
**@Descriptions	:	 Function to detect Current Joystick pressed
**@parameters		:  uiCurrentScreen : Current Screen number
**@return				:  none
*****************************************************************************/
void fnCheckJoystick_Press(uint8_t ucCurrentJoystickNum)
{
	if( 0 != ucCurrentJoystickNum)
	{
		switch(ucCurrentJoystickNum)
		{
			/*Enter*/
			case JOYSTICK_RIGHT_3:
			{
				switch(uiCurrentScreen)
				{
					case	SCREEN_KEYPAD:
					{
					ucCurrentJoystickNum = 0;
					ucpDisplayLetter = ucAlphaNum[LETTER_POS_X(uiCursorXPosKP)][LETTER_POS_Y(uiCursorYPosKP)][0];
						if ( ucpDisplayLetter == ucAlphaNum[2][7][0] )
						{
							uiLetter -= LETTER_WIDTH;
							
							uiLetterStartPos = DISP_LETTER_START_POS - uiLetter;
							if ( uiLetterStartPos > DISP_LETTER_START_POS )
							{
								uiLetter = 0;
								uiLetterStartPos = DISP_LETTER_START_POS;
							}
							ulirgStoreLetter[--ucLetterPos] = '\0';
							fnGr_PutString(85,uiLetterStartPos , "  ", RGB(0,0,0), RGB(255,255,255), ORIENTATION_90);		
							#ifdef JOYSTICK_DBG
								DEBUG_PRINTF("Current Stored String After Deleting :%s \r\n",ulirgStoreLetter);								
							#endif
						}
						else
						{
							uiLetterStartPos = DISP_LETTER_START_POS - uiLetter;
							fnGr_PutString(85,uiLetterStartPos , &ucpDisplayLetter, RGB(0,0,0), RGB(255,255,255), ORIENTATION_90);
							uiLetter += LETTER_WIDTH;
								
							ulirgStoreLetter[ucLetterPos++] = ucpDisplayLetter;
							#ifdef JOYSTICK_DBG
								DEBUG_PRINTF("Current Stored String :%s \r\n",ulirgStoreLetter);
							#endif
						}
						}break;
				
					case SCREEN_PRESSURE_SETTING:
					case SCREEN_TEMP_SETTING_1:
					case SCREEN_TEMP_SETTING_2:
					case SCREEN_PUMP_SETTING:
					case SCREEN_JOB_SETTING:
					case SCREEN_TIMER_SETTING_1:
					case SCREEN_TIMER_SETTING_2:
					case SCREEN_TIMER_SETTING_3:
					case SCREEN_TIMER_SETTING_4:
					case SCREEN_STALL_SETTING:
					case SCREEN_OTHER_SETTING_2:
					case SCREEN_OTHER_SETTING_3:
					case SCREEN_LIQUID_LEVEL_SETTING:
					{
						StatusFlag.stStatusFlag.bParameterSet = TRUE;
						
						ucCurrentJoystickNum = 0;

						ucScreenPatternNumber = SCREEN_PATTERN_11;
						ucSwitchPressFlag = TRUE;
						
						uiCurrentScreen = SCREEN_KEYPAD;
							
						if ( SCREEN_KEYPAD != uirgStoreCurrentScreen[ucCurrentScreenNumber - 1] )
						{
							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_KEYPAD;
							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
						}
					}break;
					
					case SCREEN_OPERATION:
					{
						ucCurrentJoystickNum = 0;		
						if(FALSE == StatusFlag.stStatusFlag.bStallTestInProgress)				
						{
							if ( 1 == ucPipeSetVariable )
							{
								ucSwitchPressFlag = TRUE;
								StatusFlag.stStatusFlag.bOPMMSetOnPipe = TRUE;
								
								/*send OMM set on Pipe command*/
								memset(stTx1Message.Data, 0 , sizeof(stTx1Message.Data));
								stTx1Message.DLC = 1;
								stTx1Message.Data[0] = 0x01;

								fnCANMessage_Send(STATUS_OPMM_ON_PIPE, MSG_TYPE_STATUS,  MCU_ID);

								/*fnCANMessage_Send(STATUS_OPMM_ON_PIPE, MSG_TYPE_STATUS, OPMM_ID);*/
							}
							else
							{
								StatusFlag.stStatusFlag.bOPMMSetOnPipe = FALSE;
								
								/*send not OPMM set on Pipe command*/
								memset(stTx1Message.Data, 0 , sizeof(stTx1Message.Data));
								stTx1Message.DLC = 1;
								stTx1Message.Data[0] = 0x00;

								fnCANMessage_Send(STATUS_OPMM_ON_PIPE, MSG_TYPE_STATUS, MCU_ID);
								
								/*fnCANMessage_Send(STATUS_OPMM_ON_PIPE, MSG_TYPE_STATUS, OPMM_ID);*/
							}
						}
					}break;
					
					case SCREEN_ADVANCED_RESET:
					{
						ucCurrentJoystickNum = 0;	
						if( 1 == ucResetVariableCounter )
						{
							/*Reset no of joints*/
							#ifdef JOYSTICK_DBG
								DEBUG_PRINTF("Reset Number of joints \n\r");
							#endif
							
							stConfig_Value.fConfigJointNum = 0;
							fnOPConfig_Send(CONFIG_JOINT_NUMBER,0);
						}
						else if ( 2 == ucResetVariableCounter )
						{
							/*reset base volume*/
							#ifdef JOYSTICK_DBG
								DEBUG_PRINTF("Reset Base Volume \n\r");
							#endif
							stCurrent_Value.fBaseTotalMaterialUsed = 0.0f;
							
							stCurrent_Value.fBaseMaterialPrimed = 0.0f;
							
							stCurrent_Value.fBaseMaterialApplied = 0.0f;
							
						}
						else if ( 3 == ucResetVariableCounter )
						{
							/*reset hardner volume*/
							#ifdef JOYSTICK_DBG
								DEBUG_PRINTF("Reset Hardner Volume \n\r");
							#endif
							stCurrent_Value.fHardTotalMaterialUsed = 0;
							
							stCurrent_Value.fHardMaterialPrimed = 0.0f;
							stCurrent_Value.fHardMaterialApplied = 0.0f;
						}
					}break;
					case SCREEN_OTHER_SETTING:
					{
						ucCurrentJoystickNum = 0;
						if ( (1 != ucOtherSetVariableCounter ) && ( 2 != ucOtherSetVariableCounter ) )
						{
							if ( 3 == ucOtherSetVariableCounter )
							{
								stConfig_Value.ucConfigIHCM4Type = BASE_MATERIAL_TYPE;
								fnOPConfig_Send(CONFIG_TYPE_IHCM4,BASE_MATERIAL_TYPE);
							}
							else if ( 4 == ucOtherSetVariableCounter )
							{
								stConfig_Value.ucConfigIHCM4Type = HARDNER_MATERIAL_TYPE;
								fnOPConfig_Send(CONFIG_TYPE_IHCM4,BASE_MATERIAL_TYPE);
							}
							else if ( 5 == ucOtherSetVariableCounter )
							{
								stConfig_Value.ucTypeofUnit = METRIC;
								fnOPConfig_Send(CONFIG_TYPE_OF_UNIT,METRIC);
							}
							else if(ucOtherSetVariableCounter == 6)
							{
								stConfig_Value.ucTypeofUnit = IMPERIAL;
								fnOPConfig_Send(CONFIG_TYPE_OF_UNIT,IMPERIAL);
							}
							break;
						}
						
						if ( ( 1 == ucOtherSetVariableCounter ) || ( 2 == ucOtherSetVariableCounter ) )
						{
							ucScreenPatternNumber = SCREEN_PATTERN_11;
							ucSwitchPressFlag = TRUE;
							StatusFlag.stStatusFlag.bParameterSet = TRUE;
							
							uiCurrentScreen = SCREEN_KEYPAD;
								
							if ( SCREEN_KEYPAD != uirgStoreCurrentScreen[ucCurrentScreenNumber - 1] )
							{
								uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_KEYPAD;
								ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
							}
						}
					}break;
					
					case	SCREEN_DIAGNOSTIC_STATUS:
					{
						ucCurrentJoystickNum = 0;
						
						switch(ucDiagModuleVariable)
						{
							case OP_STATUS:
							{
								ucScreenPatternNumber = SCREEN_PATTERN_1;
								ucSwitchPressFlag = TRUE;
							
								uiCurrentScreen = SCREEN_OP_DIAGNOSTIC;
								uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_OP_DIAGNOSTIC;
								ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
							}break;
							case MCU_STATUS:
							{
								ucScreenPatternNumber = SCREEN_PATTERN_1;
								ucSwitchPressFlag = TRUE;
							
								uiCurrentScreen = SCREEN_MCU_DIAGNOSTIC;
								uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_MCU_DIAGNOSTIC;
								ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
							}break;
							case OPMM_STATUS:
							{
								ucScreenPatternNumber = SCREEN_PATTERN_1;
								ucSwitchPressFlag = TRUE;
							
								uiCurrentScreen = SCREEN_OPMM_DIAGNOSTIC_1;
								uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_OPMM_DIAGNOSTIC_1;
								ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
							}break;
							case MFM_STATUS:
							{
								ucScreenPatternNumber = SCREEN_PATTERN_1;
								ucSwitchPressFlag = TRUE;
							
								uiCurrentScreen = SCREEN_MFM_DIAGNOSTIC;
								uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_MFM_DIAGNOSTIC;
								ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
							}break;
							case BHCM_STATUS:
							{
								ucScreenPatternNumber = SCREEN_PATTERN_1;
								ucSwitchPressFlag = TRUE;
							
								uiCurrentScreen = SCREEN_BHCM_DIAGNOSTIC;
								uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_BHCM_DIAGNOSTIC;
								ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
							}break;
							case HHCM_STATUS:
							{
								ucScreenPatternNumber = SCREEN_PATTERN_1;
								ucSwitchPressFlag = TRUE;
							
								uiCurrentScreen = SCREEN_HHCM_DIAGNOSTIC;
								uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_HHCM_DIAGNOSTIC;
								ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
							}break;
							case IHCM1_STATUS:
							{
								ucScreenPatternNumber = SCREEN_PATTERN_1;
								ucSwitchPressFlag = TRUE;
							
								uiCurrentScreen = SCREEN_IHCM1_DIAGNOSTIC;
								uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_IHCM1_DIAGNOSTIC;
								ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
							}break;
							case IHCM4_HM_STATUS:
							{
								ucScreenPatternNumber = SCREEN_PATTERN_1;
								ucSwitchPressFlag = TRUE;
							
								uiCurrentScreen = SCREEN_IHCM4_HM_DIAGNOSTIC;
								uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_IHCM4_HM_DIAGNOSTIC;
								ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
							}break;
						}						
					}break;
					default:
					{
						ucCurrentJoystickNum = 0;
					}break;
				}					
			}break;
			
			/*Back*/
			case JOYSTICK_LEFT_3:
			{
				ucCurrentJoystickNum = 0;
				ucSwitchPressFlag = TRUE;
				if( ( 0 < ucCurrentScreenNumber ) && ( 0 < ucPatternNumberCount ) )
				{
					if ( 0 != ucCurrentScreenNumber )
					{
						uiCurrentScreen = uirgStoreCurrentScreen[--ucCurrentScreenNumber];
						ucScreenPatternNumber = ucrgStoreCurrentScreenPattern[--ucPatternNumberCount];
					}
				}
			}break;
			
			/*Prime  Selector and Prime Start Joystick*/
			case JOYSTICK_UP_1:
			{
				ucCurrentJoystickNum = 0;

				switch(uiCurrentScreen)
					{
						case	SCREEN_OPERATION:
						case	SCREEN_PRIME:
						case	SCREEN_FLUSH:
						case 	SCREEN_SPRAY:
						{
							if (( FALSE == StatusFlag.stStatusFlag.bSystemErrorFlag ) && ( FALSE == StatusFlag.stStatusFlag.bEstopPressed ))
							{
								if (((( TRUE == StatusFlag.stStatusFlag.bPrimeDone ) && ( TRUE == StatusFlag.stStatusFlag.bPrimeStatusReceived )) ||
										(( TRUE == StatusFlag.stStatusFlag.bFlushDone ) && ( TRUE == StatusFlag.stStatusFlag.bFlushStatusReceived )) 	||
										(( TRUE == StatusFlag.stStatusFlag.bSprayDone ) && ( TRUE == StatusFlag.stStatusFlag.bSprayStatusReceived ))) && 
										( PRIME_SELECTOR != StatusFlag.stStatusFlag.bCUrrentProcessSelector ))
								{
									if ( SPRAY_SELECTOR != StatusFlag.stStatusFlag.bCUrrentProcessSelector )
									{
										StatusFlag.stStatusFlag.bCUrrentProcessSelector = PRIME_SELECTOR;
										fnProcessFlags_Clear();
									}
								}
								
								if( ( TRUE == StatusFlag.stStatusFlag.bManualPrime ) && ( PRIME_SELECTOR == StatusFlag.stStatusFlag.bCUrrentProcessSelector ) )
								{
									ucScreenPatternNumber = SCREEN_PATTERN_8;
									ucSwitchPressFlag = TRUE;
								
									uiCurrentScreen = SCREEN_PRIME;
									uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_PRIME;
									ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
									
									if (( TRUE == StatusFlag.stStatusFlag.bAwayPipePosition ) && ( PRIME_SELECTOR == StatusFlag.stStatusFlag.bCUrrentProcessSelector ) &&
											( FALSE == StatusFlag.stStatusFlag.bManualProcessStarted ))
									{
										#ifdef JOYSTICK_DBG
											DEBUG_PRINTF("\r\nMANUAL PRIME STARTS FROM JOSTICK\r\n");
										#endif
										
										StatusFlag.stStatusFlag.bJS1_CenterDetected = TRUE;
										
										StatusFlag.stStatusFlag.bManualProcessStarted = TRUE;
										ucSwitchPressFlag = TRUE;
										StatusFlag.stStatusFlag.bInitPrimeProcess = TRUE;
										
										/*Send CAN Message to Start the Automatic Prime Process*//*Send CAN command for Prime Selector*/								
										memset(stTx1Message.Data, 0 , sizeof(stTx1Message.Data));
										stTx1Message.DLC = 1;
										stTx1Message.Data[0] = MANUAL_PROCESS;

										fnCANMessage_Send(CMD_PRIME_START, MSG_TYPE_COMMAND, MCU_ID);
										
										StatusFlag.stStatusFlag.bFlushWarnCntAchieved = FALSE;
										StatusFlag.stStatusFlag.bFlushWarningFlag = FALSE;
										uliFlushWarningCnt = 0;
										GPIO_WriteBit(GPIOE, GPIO_Pin_4, Bit_RESET);
										
										StatusFlag.stStatusFlag.bPrimeDone = FALSE;
										StatusFlag.stStatusFlag.bPrimeStatusReceived = FALSE;
										StatusFlag.stStatusFlag.bPrimeStatus = FALSE;
									}
									break;
								}
								
								if (( TRUE == StatusFlag.stStatusFlag.bOPMMSetOnPipe ) && ( FALSE == StatusFlag.stStatusFlag.bManualPrime ) &&
										( FALSE == StatusFlag.stStatusFlag.bManualFlush )	&& ( FALSE == StatusFlag.stStatusFlag.bAutoSpray ))
								{
									StatusFlag.stStatusFlag.bManualPrime = TRUE;
									
									StatusFlag.stStatusFlag.bJS1_CenterDetected = TRUE;
									
									ucScreenPatternNumber = SCREEN_PATTERN_8;
									ucSwitchPressFlag = TRUE;
																	
									StatusFlag.stStatusFlag.bFlushWarningFlag = FALSE;
									uliFlushWarningCnt = 0;
									GPIO_WriteBit(GPIOE, GPIO_Pin_4, Bit_RESET);
								
									uiCurrentScreen = SCREEN_PRIME;
									uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_PRIME;
									ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
									
									/*Send CAN command for Prime Selector*/
									memset(stTx1Message.Data, 0 , sizeof(stTx1Message.Data));
									stTx1Message.DLC = 0;

									fnCANMessage_Send(CMD_PRIME_SELECTOR, MSG_TYPE_COMMAND, MCU_ID);
									
									StatusFlag.stStatusFlag.bCUrrentProcessSelector = PRIME_SELECTOR;
									
									if ( TRUE == StatusFlag.stStatusFlag.bAwayPipePosition )
									{
										/*Manual prime starts if gun already away*/
										StatusFlag.stStatusFlag.bManualProcessStarted = TRUE;
										ucSwitchPressFlag = TRUE;
										StatusFlag.stStatusFlag.bInitPrimeProcess = TRUE;
										
										/*Send CAN Message to Start the Manual Prime Process*//*Send CAN command for Prime Selector*/								
										memset(stTx1Message.Data, 0 , sizeof(stTx1Message.Data));
										stTx1Message.DLC = 1;
										stTx1Message.Data[0] = MANUAL_PROCESS;

										fnCANMessage_Send(CMD_PRIME_START, MSG_TYPE_COMMAND, MCU_ID);
										
										StatusFlag.stStatusFlag.bPrimeDone = FALSE;
										StatusFlag.stStatusFlag.bPrimeStatusReceived = FALSE;
										StatusFlag.stStatusFlag.bPrimeStatus = FALSE;
									}
								}
							}
						}break;
					}
			}break;
			
			/*Prime Stop Joystick*/
			case JOYSTICK_CENTER_1:
			{
				if ( TRUE == StatusFlag.stStatusFlag.bManualProcessStarted )
							{
								StatusFlag.stStatusFlag.bJS1_CenterDetected = FALSE;	
								GPIO_WriteBit(GPIOE, GPIO_Pin_4, Bit_RESET);
								
								StatusFlag.stStatusFlag.bManualProcessStarted = FALSE;
								#ifdef JOYSTICK_DBG
									DEBUG_PRINTF("CENTRE DETECTED AND SEND PRIME STOP Command send to OPMM\r\n");
								#endif
								
								ucSwitchPressFlag = TRUE;
								/*StatusFlag.stStatusFlag.bInitPrimeProcess = FALSE;
								StatusFlag.stStatusFlag.bPrimeDone = TRUE;*/
								/*Send CAN Message to Stop the MANUAL Prime Process*/
								memset(stTx1Message.Data, 0 , sizeof(stTx1Message.Data));
								stTx1Message.DLC = 1;
								stTx1Message.Data[0] = PRIME_DONE;

								fnCANMessage_Send(CMD_MANUAL_PROCESS_DONE, MSG_TYPE_COMMAND, MCU_ID);
								
								StatusFlag.stStatusFlag.bFlushWarningFlag = TRUE;
								uliFlushWarningCnt = 0;
								break;
							}
				
			}break;
			/*SprayProcess Jostick*/
			case JOYSTICK_LEFT_2:
			{
				ucCurrentJoystickNum = 0;
					if (( FALSE == StatusFlag.stStatusFlag.bSystemErrorFlag ) && ( FALSE == StatusFlag.stStatusFlag.bEstopPressed ))
					{
						ucCurrentJoystickNum = 0;
						if ((( TRUE == StatusFlag.stStatusFlag.bPrimeDone ) && ( TRUE == StatusFlag.stStatusFlag.bPrimeStatusReceived )) ||
								(( TRUE == StatusFlag.stStatusFlag.bFlushDone ) && ( TRUE == StatusFlag.stStatusFlag.bFlushStatusReceived )) ||
								(( TRUE == StatusFlag.stStatusFlag.bSprayDone ) && ( TRUE == StatusFlag.stStatusFlag.bSprayStatusReceived ))
							)
						{
							if (( PRIME_SELECTOR == StatusFlag.stStatusFlag.bCUrrentProcessSelector ) || 
									( SPRAY_SELECTOR == StatusFlag.stStatusFlag.bCUrrentProcessSelector ))
							{
								#ifdef JOYSTICK_DBG
									DEBUG_PRINTF("\r\nCUrrentProcessSelector-%d\r\n",StatusFlag.stStatusFlag.bCUrrentProcessSelector);
								#endif
								StatusFlag.stStatusFlag.bCUrrentProcessSelector = SPRAY_SELECTOR;
							
								fnProcessFlags_Clear();
							}
						}						
						if ( TRUE == StatusFlag.stStatusFlag.bAutoSpray )
						{
							ucScreenPatternNumber = SCREEN_PATTERN_8;
							ucSwitchPressFlag = TRUE;
						
							uiCurrentScreen = SCREEN_SPRAY;
							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_SPRAY;
							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
							break;
						}
						if (( TRUE == StatusFlag.stStatusFlag.bOPMMSetOnPipe ) && ( FALSE == StatusFlag.stStatusFlag.bAutoSpray ) &&
								( FALSE == StatusFlag.stStatusFlag.bManualFlush ) 	&& ( FALSE == StatusFlag.stStatusFlag.bAutoFlush ) &&
								( FALSE == StatusFlag.stStatusFlag.bManualPrime )  && ( FALSE == StatusFlag.stStatusFlag.bAutoPrime ) &&
								( SPRAY_SELECTOR == StatusFlag.stStatusFlag.bCUrrentProcessSelector ))
						{
							StatusFlag.stStatusFlag.bAutoSpray = TRUE;
							
							ucScreenPatternNumber = SCREEN_PATTERN_8;
							ucSwitchPressFlag = TRUE;
						
							uiCurrentScreen = SCREEN_SPRAY;
							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_SPRAY;
							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
							
							/*Send Command for Spray selector*/
							memset(stTx1Message.Data, 0 , sizeof(stTx1Message.Data));
							stTx1Message.DLC = 1;
							stTx1Message.Data[0] = AUTOMATIC_PROCESS;

							fnCANMessage_Send(CMD_SPRAY_SELECTOR, MSG_TYPE_COMMAND, MCU_ID);
							
							StatusFlag.stStatusFlag.bFlushWarnCntAchieved = FALSE;
							StatusFlag.stStatusFlag.bFlushWarningFlag = FALSE;
							uliFlushWarningCnt = 0;
							GPIO_WriteBit(GPIOE, GPIO_Pin_4, Bit_RESET);
						}
					}
				}break;
			/*Spray Extra Coat*/
			case JOYSTICK_RIGHT_2:
			{
				ucCurrentJoystickNum = 0;

				switch(uiCurrentScreen)
				{
					case	SCREEN_OPERATION:
					case	SCREEN_PRIME:
					case	SCREEN_FLUSH:
					case 	SCREEN_SPRAY:
					{
						if (( FALSE == StatusFlag.stStatusFlag.bSystemErrorFlag ) && ( FALSE == StatusFlag.stStatusFlag.bEstopPressed ))
						{
							ucCurrentJoystickNum = 0;
							if ((( TRUE == StatusFlag.stStatusFlag.bPrimeDone ) && ( TRUE == StatusFlag.stStatusFlag.bPrimeStatusReceived )) ||
									(( TRUE == StatusFlag.stStatusFlag.bFlushDone ) && ( TRUE == StatusFlag.stStatusFlag.bFlushStatusReceived )) ||
									(( TRUE == StatusFlag.stStatusFlag.bSprayDone ) && ( TRUE == StatusFlag.stStatusFlag.bSprayStatusReceived )))
							{
								if (( PRIME_SELECTOR == StatusFlag.stStatusFlag.bCUrrentProcessSelector ) || 
										( SPRAY_SELECTOR == StatusFlag.stStatusFlag.bCUrrentProcessSelector ))
								{
									StatusFlag.stStatusFlag.bCUrrentProcessSelector = SPRAY_SELECTOR;
									fnProcessFlags_Clear();
								}
							}
							if ( TRUE == StatusFlag.stStatusFlag.bAutoSpray  )
							{
								ucScreenPatternNumber = SCREEN_PATTERN_8;
								ucSwitchPressFlag = TRUE;
							
								uiCurrentScreen = SCREEN_SPRAY;
								uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_SPRAY;
								ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
								break;
							}
							if	(( TRUE == StatusFlag.stStatusFlag.bOPMMSetOnPipe )	&& ( FALSE == StatusFlag.stStatusFlag.bAutoSpray ) &&
									( FALSE == StatusFlag.stStatusFlag.bManualFlush )		&& ( FALSE == StatusFlag.stStatusFlag.bAutoFlush ) &&
									( FALSE == StatusFlag.stStatusFlag.bManualPrime ) 		&& ( FALSE == StatusFlag.stStatusFlag.bAutoPrime ) &&
									( SPRAY_SELECTOR == StatusFlag.stStatusFlag.bCUrrentProcessSelector ))
							{
								StatusFlag.stStatusFlag.bAutoSpray = TRUE;
								
								ucScreenPatternNumber = SCREEN_PATTERN_8;
								ucSwitchPressFlag = TRUE;
							
								uiCurrentScreen = SCREEN_SPRAY;
								uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_SPRAY;
								ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
								
								/*Send Command for Spray selector*/
								memset(stTx1Message.Data, 0 , sizeof(stTx1Message.Data));
								stTx1Message.DLC = 1;
								stTx1Message.Data[0] = EXTRA_COAT;

								fnCANMessage_Send(CMD_SPRAY_SELECTOR, MSG_TYPE_COMMAND, MCU_ID);
								
								StatusFlag.stStatusFlag.bFlushWarnCntAchieved = FALSE;
								StatusFlag.stStatusFlag.bFlushWarningFlag = FALSE;
								uliFlushWarningCnt = 0;
								GPIO_WriteBit(GPIOE, GPIO_Pin_4, Bit_RESET);
							}
						}
					}break;
				}
			}break;
			/*Not In Use*/
			case JOYSTICK_DOWN_1:
			{
			}break;
			
			/*Flush  Selector and Flush Start Joystick*/
			case JOYSTICK_UP_4:
			{
				ucCurrentJoystickNum = 0;

				switch(uiCurrentScreen)
					{
						case	SCREEN_OPERATION:
						case	SCREEN_PRIME:
						case	SCREEN_FLUSH:
						case 	SCREEN_SPRAY:
						{
								if (((( TRUE == StatusFlag.stStatusFlag.bPrimeDone == TRUE)	&& ( TRUE == StatusFlag.stStatusFlag.bPrimeStatusReceived == TRUE)) ||
										(( TRUE == StatusFlag.stStatusFlag.bFlushDone == TRUE) 	&& ( TRUE == StatusFlag.stStatusFlag.bFlushStatusReceived == TRUE)) ||
										(( TRUE == StatusFlag.stStatusFlag.bSprayDone == TRUE)		&& ( TRUE == StatusFlag.stStatusFlag.bSprayStatusReceived == TRUE))) && 
										(StatusFlag.stStatusFlag.bCUrrentProcessSelector != FLUSH_SELECTOR))
								{
									#ifdef JOYSTICK_DBG
										DEBUG_PRINTF("\r\nALL Process Flags RESET in FLUSH\r\n");
									#endif
									StatusFlag.stStatusFlag.bCUrrentProcessSelector = FLUSH_SELECTOR;
								
									StatusFlag.stStatusFlag.bFlushWarnCntAchieved = FALSE;
									StatusFlag.stStatusFlag.bFlushWarningFlag = FALSE;
									uliFlushWarningCnt = 0;
								
									fnProcessFlags_Clear();
								}
								
								if ( TRUE == StatusFlag.stStatusFlag.bManualFlush )
								{
									ucScreenPatternNumber = SCREEN_PATTERN_8;
									ucSwitchPressFlag = TRUE;
								
									uiCurrentScreen = SCREEN_FLUSH;
									uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_FLUSH;
									ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
									
									if (( TRUE == StatusFlag.stStatusFlag.bAwayPipePosition ) && ( FLUSH_SELECTOR == StatusFlag.stStatusFlag.bCUrrentProcessSelector ) &&
											( FALSE == StatusFlag.stStatusFlag.bManualProcessStarted ))
									{
										#ifdef JOYSTICK_DBG
											DEBUG_PRINTF("\r\nMANUAL FLUSH STARTS FROM JOSTICK\r\n");
										#endif
										
										StatusFlag.stStatusFlag.bJS4_CenterDetected = TRUE;
										
										StatusFlag.stStatusFlag.bManualProcessStarted = TRUE;
										ucSwitchPressFlag = TRUE;
										StatusFlag.stStatusFlag.bInitFlushProcess = TRUE;
										
										/*Send CAN Message to Start the Automatic Prime Process*//*Send CAN command for Prime Selector*/								
										memset(stTx1Message.Data, 0 , sizeof(stTx1Message.Data));
										stTx1Message.DLC = 1;
										stTx1Message.Data[0] = MANUAL_PROCESS;

										fnCANMessage_Send(CMD_FLUSH_START, MSG_TYPE_COMMAND, MCU_ID);
										
										StatusFlag.stStatusFlag.bFlushDone = FALSE;
										StatusFlag.stStatusFlag.bFlushStatusReceived = FALSE;
										StatusFlag.stStatusFlag.bFlushStatus = FALSE;
										
										StatusFlag.stStatusFlag.bFlushDoneOnce = SET;
										
										GPIO_WriteBit(GPIOE, GPIO_Pin_4, Bit_RESET);
									}
									break;
								}
								
								if	(( TRUE == StatusFlag.stStatusFlag.bOPMMSetOnPipe )	&& ( FALSE == StatusFlag.stStatusFlag.bManualPrime ) &&
										( FALSE == StatusFlag.stStatusFlag.bManualFlush )		&& ( FALSE == StatusFlag.stStatusFlag.bAutoSpray ))
								{
									StatusFlag.stStatusFlag.bManualFlush = TRUE;
									
									StatusFlag.stStatusFlag.bJS4_CenterDetected = TRUE;
									
									ucScreenPatternNumber = SCREEN_PATTERN_8;
									ucSwitchPressFlag = TRUE;
																
									GPIO_WriteBit(GPIOE, GPIO_Pin_4, Bit_RESET);
								
									uiCurrentScreen = SCREEN_FLUSH;
									uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_FLUSH;
									ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
									
									/*Send CAN command for Prime Selector*/
									memset(stTx1Message.Data, 0 , sizeof(stTx1Message.Data));
									stTx1Message.DLC = 0;

									fnCANMessage_Send(CMD_FLUSH_SELECTOR, MSG_TYPE_COMMAND, MCU_ID);
									
									StatusFlag.stStatusFlag.bCUrrentProcessSelector = FLUSH_SELECTOR;
									
									if ( TRUE == StatusFlag.stStatusFlag.bAwayPipePosition )
									{
										#ifdef JOYSTICK_DBG
											DEBUG_PRINTF("\r\nMANUAL FLUSH STARTS IF GUN Alread Away\r\n");
										#endif
								
										StatusFlag.stStatusFlag.bManualProcessStarted = TRUE;
										ucSwitchPressFlag = TRUE;
										StatusFlag.stStatusFlag.bInitFlushProcess = TRUE;
										
										/*Send CAN Message to Start the Manual Prime Process*//*Send CAN command for Prime Selector*/								
										memset(stTx1Message.Data, 0 , sizeof(stTx1Message.Data));
										stTx1Message.DLC = 1;
										stTx1Message.Data[0] = MANUAL_PROCESS;

										fnCANMessage_Send(CMD_FLUSH_START, MSG_TYPE_COMMAND, MCU_ID);
										
										StatusFlag.stStatusFlag.bFlushDone = FALSE;
										StatusFlag.stStatusFlag.bFlushStatusReceived = FALSE;
										StatusFlag.stStatusFlag.bFlushStatus = FALSE;
										
										StatusFlag.stStatusFlag.bFlushDoneOnce = SET;
									}
								}
						}break;
					}
			}break;
			
			/*Flush Stop Joystick*/
			case JOYSTICK_CENTER_4:
			{
				ucCurrentJoystickNum = 0;
				if ( TRUE == StatusFlag.stStatusFlag.bManualProcessStarted )
				{
					StatusFlag.stStatusFlag.bJS4_CenterDetected = FALSE;
					
					GPIO_WriteBit(GPIOE, GPIO_Pin_4, Bit_RESET);
					
					StatusFlag.stStatusFlag.bManualProcessStarted = FALSE;
					#ifdef JOYSTICK_DBG
						DEBUG_PRINTF("CENTRE DETECTED AND SEND FLUSH STOP Command send to OPMM\r\n");
					#endif
					
					ucSwitchPressFlag = TRUE;
					/*Send CAN Message to Stop the MANUAL Prime Process*/
					memset(stTx1Message.Data, 0 , sizeof(stTx1Message.Data));
					stTx1Message.DLC = 1;
					stTx1Message.Data[0] = FLUSH_DONE;

					fnCANMessage_Send(CMD_MANUAL_PROCESS_DONE, MSG_TYPE_COMMAND, MCU_ID);
					break;
				}
			}break;
			
			/*Stall Test Jostick*/
			case JOYSTICK_DOWN_4:
			{
				switch(uiCurrentScreen)
				{
					case	SCREEN_STALL:
					{
						if (( FALSE == StatusFlag.stStatusFlag.bInitPrimeProcess )	&& ( FALSE == StatusFlag.stStatusFlag.bInitSprayProcess ) &&
							 ( FALSE == StatusFlag.stStatusFlag.bInitFlushProcess ))
						{
							fnProcessFlags_Clear();
							
							/*Again Ask user For Gun Positions*/
							StatusFlag.stStatusFlag.bOPMMSetOnPipe = FALSE;
							
							ucScreenPatternNumber = SCREEN_PATTERN_3;
							ucSwitchPressFlag = TRUE;
							
							uiCurrentScreen = SCREEN_STALL;
							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_STALL;
							ucrgStoreCurrentScreenPattern[++ucPatternNumberCount] = ucScreenPatternNumber;
							
							StatusFlag.stStatusFlag.bStallTestStatusReceived = FALSE;
							StatusFlag.stStatusFlag.bDownstrokeStatusReceived = FALSE;
							StatusFlag.stStatusFlag.bUpstrokeStatusReceived = FALSE;
							stCurrent_Value.iCurrPressureLegADown = 0;
							stCurrent_Value.iCurrPressureLegBDown = 0;
							stCurrent_Value.iCurrPressureLegCDown = 0;
							stCurrent_Value.ucCurrPressureLegAUp = 0;
							stCurrent_Value.ucCurrPressureLegBUp = 0;
							stCurrent_Value.ucCurrPressureLegCUp = 0;
							
							/*Send Command for Manual Stall test*/
							memset(stTx1Message.Data, 0 , sizeof(stTx1Message.Data));
							stTx1Message.DLC = 1;
							stTx1Message.Data[0] = MANUAL_PROCESS;

							fnCANMessage_Send(CMD_STALL_TEST, MSG_TYPE_COMMAND, MCU_ID);
							
							StatusFlag.stStatusFlag.bStallTestInProgress = TRUE;
						}
					}break;

					case SCREEN_FLUSH:
					{
						if (( FALSE == StatusFlag.stStatusFlag.bInitPrimeProcess )	&& ( FALSE == StatusFlag.stStatusFlag.bInitSprayProcess ) &&
							 ( TRUE == StatusFlag.stStatusFlag.bFlushDone ) && ( TRUE == StatusFlag.stStatusFlag.bFlushStatusReceived ))
						{
							fnProcessFlags_Clear();
							
							/*Again Ask user For Gun Positions*/
							StatusFlag.stStatusFlag.bOPMMSetOnPipe = FALSE;
							
							ucScreenPatternNumber = SCREEN_PATTERN_8;
							ucSwitchPressFlag = TRUE;
							
							/*cursor position*/
							uiCursorXPos = 108;
							uiCursorYPos = 245;
													
							ucPipeSetVariable = 1;						
							uiCurrentScreen = SCREEN_OPERATION;
							uirgStoreCurrentScreen[++ucCurrentScreenNumber] = SCREEN_OPERATION;
							
							StatusFlag.stStatusFlag.bStallTestStatusReceived = FALSE;
							StatusFlag.stStatusFlag.bDownstrokeStatusReceived = FALSE;
							StatusFlag.stStatusFlag.bUpstrokeStatusReceived = FALSE;
							stCurrent_Value.iCurrPressureLegADown = 0;
							stCurrent_Value.iCurrPressureLegBDown = 0;
							stCurrent_Value.iCurrPressureLegCDown = 0;
							stCurrent_Value.ucCurrPressureLegAUp = 0;
							stCurrent_Value.ucCurrPressureLegBUp = 0;
							stCurrent_Value.ucCurrPressureLegCUp = 0;
							
							/*Send Command for Manual Stall test*/
							memset(stTx1Message.Data, 0 , sizeof(stTx1Message.Data));
							stTx1Message.DLC = 1;
							stTx1Message.Data[0] = AUTOMATIC_PROCESS;

							fnCANMessage_Send(CMD_STALL_TEST, MSG_TYPE_COMMAND, MCU_ID);
							StatusFlag.stStatusFlag.bStallTestInProgress = TRUE;
						}
					}break;
			}
		}break;
		}
	}
	ucCurrentJoystick = 0;
}

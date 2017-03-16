/******************************************************************************
* @file    ADS1118.c
* 
* @brief   This file contains the funtions related to Thercouple.
******************************************************************************
*/    
/* ----------------------------------------------------------------------------
*                           Includes
* ----------------------------------------------------------------------------
*/
#include "Config.h"
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
uint8_t ucCReading = 0; /*for currently reading thermocouple temp*/
uint8_t ucSwitchRd = 0; /*Use for switching between TS and ADC reading of ADS1118*/
uint16_t uiConfigWr = DEFAULT_SETTING; /*Configuration Register of the ADS1118*/
uint16_t uiConfigRd = 0;	/*Use for Read ADC data From the ADS1118*/

float fTcj = 0.0f;		/*Read Cold Junction Temp Value from the ADS1118*/
float fVcj = 0.0f;		/*Converted voltage Value from fTcj using NIST table*/
float fVtc = 0.0f;		/*Read Thermocouple Voltage Value from the ADS1118*/
float fVtotal = 0.0f; /*Added voltage values of Thermocouple and Cold Junction*/
//float fTtotal = 0.0f; /*Converted Temperature Value from fVtotal using NIST table*/


/*Look Up table according to NIST table for K-type Thermocouple*/
/*Coloumn 1 ==> Temp and Coloumn(in °C) 2 ==> volts in mV*/
float rgfLookUp[][2] = { {-160.0f, -5.141f},
												 {-120.0f, -4.138f},	
												 {-100.0f, -3.554f},	
												 {-90.0f, -3.243f},
												 {-80.0f, -2.920f},
												 {-70.0f, -2.587f},	
												 {-60.0f, -2.243f},	
												 {-50.0f, -1.889f},
												 {-40.0f, -1.527f},
												 {-30.0f, -1.156f},	
												 {-20.0f, -0.778f},	
												 {-10.0f, -0.392f},	
												 {0.0f, 0.0f},	
												 {10.0f, 0.397f},	
											   {20.0f, 0.798f},	
												 {30.0f, 1.203f},	
												 {40.0f, 1.612f},	
											 	 {50.0f, 2.023f},	
												 {60.0f, 2.436f},	
												 {70.0f, 2.851f},	
												 {80.0f, 3.267f},	
												 {90.0f, 3.682f},	
												 {100.0f, 4.096f},	
												 {110.0f, 4.509f},	
												 {120.0f, 4.920f},	
												 {130.0f, 5.328f},	
												 {140.0f, 5.735f},	
												 {150.0f, 6.138f},	
												 {180.0f, 7.340f},	
												 {200.0f, 8.138f},	
												 {220.0f, 8.940f},	
												 {250.0f, 10.153f},	
												 {280.0f, 10.153f},	
												 {300.0f, 12.209f},	
												 {350.0f, 14.293f},	
												 {400.0f, 16.397f},	
												 {450.0f, 18.516f},	
												 {500.0f, 20.644f},	
												 {550.0f, 22.776f},	
												 {600.0f, 24.905f},	
												 {650.0f, 27.025f},	
											 	 {700.0f, 29.129f},	
												 {800.0f, 33.275f},	
												 {900.0f, 37.326f},	
												 {1000.0f, 41.276f},
												 {1050.0f, 43.211f},
												 {1100.0f, 45.119f},
												 {1200.0f, 48.838f},
												 {1300.0f, 52.410f},
												 {1350.0f, 54.138f},
												 {1372.0f, 54.886f},
											 };
TypeDef_TC_Read stTCRead[2];
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
/***************************************************************************
 **@function	 : 	fnSPI_Init()
 **@Description: 	Iinitialization of SPI for thermocouple 
 **@para 			 :  none 
 **@retval 		 :  none
*****************************************************************************/
void fnSPI_Init(void)
{
//	uint8_t ucLoop = 0;
	SPI_InitTypeDef stSPI_Config;
  GPIO_InitTypeDef   stGPIO_InitStructure;
 	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOG, ENABLE);
	
	

	stGPIO_InitStructure.GPIO_Pin = TC_CS1_PIN;
  stGPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	stGPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_Init(TC_CS1_PORT, &stGPIO_InitStructure);
 	
	stGPIO_InitStructure.GPIO_Pin = TC_CS2_PIN;
  stGPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	stGPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_Init(TC_CS2_PORT, &stGPIO_InitStructure);
	
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

  stGPIO_InitStructure.GPIO_Pin = TC_SCK | TC_MOSI;  /*** clk and MOSI ***/
  stGPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  stGPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(TC_PORT, &stGPIO_InitStructure);
	
	stGPIO_InitStructure.GPIO_Pin = TC_MISO; /** MISO ***/
  stGPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  stGPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  /* Initialize GPIOA */ 
  GPIO_Init(TC_PORT, &stGPIO_InitStructure);  
  
  stSPI_Config.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  stSPI_Config.SPI_Mode = SPI_Mode_Master;
  stSPI_Config.SPI_DataSize = SPI_DataSize_16b;
  stSPI_Config.SPI_CPOL = SPI_CPOL_Low;
  stSPI_Config.SPI_CPHA = SPI_CPHA_2Edge;//SPI_CPHA_1Edge;
  stSPI_Config.SPI_NSS = SPI_NSS_Hard;
  stSPI_Config.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8 ;
  stSPI_Config.SPI_FirstBit = SPI_FirstBit_MSB;
  stSPI_Config.SPI_CRCPolynomial = 7;

  SPI_Init(TC_SPI_NUMBER , &stSPI_Config);

  SPI_SSOutputCmd(TC_SPI_NUMBER, ENABLE);

  SPI_Cmd(TC_SPI_NUMBER , ENABLE);
	
	GPIO_WriteBit(TC_CS1_PORT, TC_CS1_PIN, Bit_SET);
	GPIO_WriteBit(TC_CS2_PORT, TC_CS2_PIN, Bit_SET);



}

/**************************************************************************
 **@Function 		: fnReadTemp
 **@Description	: This function is used to read the Temp value from ADS1118
 **@Parameters	: none
 **@Return			: none
 *************************************************************************/
void fnReadTemp(void)
{
	#ifdef TC_READ
		uint8_t rgucTemp[150] = {0};
	#endif
	if(0 == ucSwitchRd)
	{ 
		ucSwitchRd = 1;	/*Use for switching between TS and ADC reading of ADS1118*/
		
		uiConfigWr &= ~TS_MODE;  /* Setting  TS = 0 for ADC read in next cycle */
		uiConfigRd = fnADS1118_ReadData(uiConfigWr);//COLD_JUCNTION

		uiConfigRd = ((uiConfigRd >> 2) & 0x3FFF);

		/*voltage calculation*/
		if(uiConfigRd & 0x2000)
		{
			uiConfigRd = ((~(uiConfigRd ) + 1) & 0x3FFF);
			fTcj = -(float)uiConfigRd * COLDJ_RESOLUTION;
		}
		else
		{
			fTcj = (float)uiConfigRd * COLDJ_RESOLUTION;
		}				
				
	}
	else
	{
		/*Voltage prints*/		
		#ifdef TC_READ		
			sprintf((char *)rgucTemp, "\nCONG Reg : 0x%04x\nTcj = %0.04f C  \r\n", uiConfigWr, fTcj);
			DEBUG_PRINTF((const char *)rgucTemp);
		#endif
		
		fVcj = fnTempToVoltage(fTcj);
		
		#ifdef TC_READ
			sprintf((char *)rgucTemp, "Vcj = %0.04f mV\r\n", fVcj);
			DEBUG_PRINTF((const char *)rgucTemp);		
		#endif		
		ucSwitchRd = 0; /*Use for switching between TS and ADC reading of ADS1118*/

		uiConfigWr |= TS_MODE; //Setting  TS = 1 for Cold Junction Temp
		
		uiConfigRd = fnADS1118_ReadData(uiConfigWr);//Delta V ADC
		
		if(uiConfigRd & 0x8000)
		{
			uiConfigRd = (~(uiConfigRd) + 1);
			fVtc = -(((float)uiConfigRd * RES_NUM) / RES_DEN ) * 1000.0f;
		}
		else
		{
			fVtc = (((float)uiConfigRd * RES_NUM) / RES_DEN ) * 1000.0f;
			/*Voltage prints*/
		}
				
		/*Voltage prints*/
		#ifdef TC_READ
			sprintf((char *)rgucTemp, "Vtc = %0.04f mV\r\n", fVtc);
			DEBUG_PRINTF((const char *)rgucTemp);
		#endif
		/*addition of both voltages*/
		fVtotal = fVtc + fVcj;
		#ifdef TC_READ
			sprintf((char *)rgucTemp, "Voltage = %0.04f mV\r\n", fVtotal);
			DEBUG_PRINTF((const char *)rgucTemp);
		#endif
		/*Convert voltage to Temp usin NIST and Current Temp stored in structure*/
		stTCRead[ucCReading].fCurrentTemp = (float)fnVoltageToTemp(fVtotal);
		#ifdef TC_READ	
			sprintf((char *)rgucTemp, "Temperature %d = %0.04f C \r\n", ucCReading, stTCRead[ucCReading].fCurrentTemp);
			DEBUG_PRINTF((const char *)rgucTemp);
		#endif
		/************************* Temp reading completed *********************************/
	
		

		/*This will set thermocouple fault*/
		if(fVtotal >= 55.0f)
		{
			stTCRead[ucCReading].fCurrentTemp = INVALID_FLOAT_VALUE;
			#ifdef TC_READ
			DEBUG_PRINTF("Fault Detected\r\n");
			#endif
		}
		
		ucCReading++;
		if(NO_OF_TC_SENSOR <= ucCReading)
		{
			ucCReading = 0;
			StatusFlag.stStatusFlag.bTCProcessed = SET;
		}
	}		
}





/*****************************************************************************
 **@Function 			: fnADS1118_ReadData
 **@Descriptions	: write command to MCP2515.
 **@parameters		: uiData: 
 **@return				:	uint16_t data read from MCP2515
 *****************************************************************************/
uint16_t fnADS1118_ReadData(uint16_t uiData)   
{ 
	uint16_t uiReadData = 0;
	
	ADS1118_SELECT(ucCReading); /*CS asserts low*/
	
	uiReadData = fnTCSPI_ReadWrite(uiData);	
	
	ADS1118_UNSELECT(ucCReading);  /*CS High*/
		
	return 	uiReadData;
}


/*****************************************************************************
**@Function 		: fnTCSPI_ReadWrite
**@Descriptions	: write command to ADS1118 .
**@parameters		: uiData: command or data to write
**@return				:	data read from ADS1118
*****************************************************************************/
uint16_t fnTCSPI_ReadWrite(uint16_t uiData)   
{    
    SPI_I2S_SendData(SPI2,uiData);  

		while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) != SET);
    while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) != SET);
    
    return (uint16_t)(SPI_I2S_ReceiveData(SPI2)); 
} 
/*****************************************************************************
 **@Function 			: fnTempToVoltage 
 **@Descriptions	: This function is used to convert Temp(°C) to Voltage(mV)
 **@parameters		: fTempValue: Temp value to convert into Voltage according 
										to NIST equation
 **@return				:	converted voltage 
 *****************************************************************************/
float fnTempToVoltage(float fTempValue)
{
	float fRetVoltage = 0.0f;
	uint8_t ucRow = 0;
	int uiLookUpTableSize = (sizeof(rgfLookUp)/sizeof(rgfLookUp[0]));
	for(ucRow = 0; ucRow < uiLookUpTableSize; ucRow++)
	{
		if(rgfLookUp[ucRow][TEMP_COLUMN] == fTempValue)
		{
			fRetVoltage = rgfLookUp[ucRow][VOLT_COLUMN];
		}
		else if((rgfLookUp[ucRow][TEMP_COLUMN] < fTempValue) && (rgfLookUp[ucRow + 1][TEMP_COLUMN] > fTempValue))
		{
			//here n = ucRow +1 and n-1 = ucRow
			fRetVoltage = (rgfLookUp[ucRow][VOLT_COLUMN] + /*Vn-1*/
										((rgfLookUp[ucRow + 1][VOLT_COLUMN] - rgfLookUp[ucRow][VOLT_COLUMN]) * /*Vn - Vn-1*/
										  ((fTempValue - rgfLookUp[ucRow][TEMP_COLUMN]) / /*Tin - Tn-1*/
											(rgfLookUp[ucRow + 1][TEMP_COLUMN] - rgfLookUp[ucRow][TEMP_COLUMN]))));/*Tn - Tn-1*/
		}
	}
	return (fRetVoltage);/*Converting mili vots to volts*/
}

/*****************************************************************************
 **@Function 			: fnVoltageToTemp
 **@Descriptions	: This Function is used to conver voltage(mV) to Temp(°C)
 **@parameters		: fTempValue: voltage value to convert into Tempreture 
										according to NIST equation
 **@return				:	converted temp in float 
 *****************************************************************************/
float fnVoltageToTemp(float fVoltage)
{
	float fRetTempValue = 0.0f;
	uint8_t ucRow = 0;
	
	int uiLookUpTableSize = (sizeof(rgfLookUp)/sizeof(rgfLookUp[0]));
//	fVoltage = fVoltage * 1000.0f; /*converting volt to mili volts*/
	
	for(ucRow = 0; ucRow < uiLookUpTableSize; ucRow++)
	{
		if(rgfLookUp[ucRow][VOLT_COLUMN] == fVoltage)
		{
			fRetTempValue = rgfLookUp[ucRow][TEMP_COLUMN];
		}
		else if((rgfLookUp[ucRow][VOLT_COLUMN] < fVoltage) && (rgfLookUp[ucRow + 1][VOLT_COLUMN] > fVoltage))
		{
			//here n = ucRow +1 and n-1 = ucRow
			fRetTempValue = (rgfLookUp[ucRow][TEMP_COLUMN] + /*Tn-1*/
										((rgfLookUp[ucRow + 1][TEMP_COLUMN] - rgfLookUp[ucRow][TEMP_COLUMN]) * /*Tn - Tn-1*/
										  ((fVoltage - rgfLookUp[ucRow][VOLT_COLUMN]) / /*Vin - Vn-1*/
											(rgfLookUp[ucRow + 1][VOLT_COLUMN] - rgfLookUp[ucRow][VOLT_COLUMN]))));/*Vn - Vn-1*/
		}
	}
	return fRetTempValue;
}

/*****************************************************************************
 **@Function 	 	: fnTemprature_Process
 **@Description	: This funtion will read the TC and will take the action
 **@Parameters	: ucTCNum: processing TC no. between 0 to 3 only
 **@Return			: None
 ***************************************************************************/
void fnTemprature_Process(uint8_t ucTCNum)
{
//	uint8_t ucLoop;
#ifdef GENERAL_DBG
	char rgcTemp[150];
#endif
	
	/*subtract the previous value from the sum and add new temp to it*/
	if((fabs(stTCRead[ucTCNum].fCurrentTemp) <= MAX_READING_RANGE) && 
		 (fabs(stTCRead[ucTCNum].fCurrentTemp) >= MIN_READING_RANGE))
	{
		/*Remove Last value of the Window from the some*/
		stTCRead[ucTCNum].fSum -= 
										stTCRead[ucTCNum].rgfTempBuff[stTCRead[ucTCNum].ucTraverse];
		/*Store the newly Read data to the Window at the First*/
		stTCRead[ucTCNum].rgfTempBuff[stTCRead[ucTCNum].ucTraverse] =
																								stTCRead[ucTCNum].fCurrentTemp;
		/*Add Newly Read Data to the Sum of The Temperatures*/
		stTCRead[ucTCNum].fSum += stTCRead[ucTCNum].fCurrentTemp;
		stTCRead[ucTCNum].ucTraverse++;/*Increase the Pointer*/
		
		if(TC_WINDOW_SIZE <= stTCRead[ucTCNum].ucTraverse)
		{
			stTCRead[ucTCNum].ucTraverse = 0;
			stTCRead[ucTCNum].bWindowOverflow = SET;
		}
		
		stTCRead[ucTCNum].ucTCFault = 0;	
	}
	else /* Thermocouple fault */
	{
		if(5 > stTCRead[ucTCNum].ucTCFault)
		{
			stTCRead[ucTCNum].ucTCFault++;
		}
	}
		
	
	/*Averaging*/
	/*if Window Overflow then take the average of TC_WINDOW_SIZE else 
		the Number of Data Read(i.e Traverse)*/
	if(SET == stTCRead[ucTCNum].bWindowOverflow)
	{
		stTCRead[ucTCNum].fCurrentTemp = stTCRead[ucTCNum].fSum / TC_WINDOW_SIZE;
	}
	else
	{
		stTCRead[ucTCNum].fCurrentTemp = stTCRead[ucTCNum].fSum / 
																						stTCRead[ucTCNum].ucTraverse;	
	}
	#ifdef GENERAL_DBG
		sprintf(rgcTemp, "Current Temp[%d] Send = %0.4f Sum = %0.04f",
						 ucTCNum, stTCRead[ucTCNum].fCurrentTemp,stTCRead[ucTCNum].fSum); 
		DEBUG_PRINTF("%s\r\n", rgcTemp); 
	#endif
	if((RESET == StatusFlag.stStatusFlag.bMCPCAN2ActiveFlag) || 
		 ((RESET == StatusFlag.stStatusFlag.bInitPrimeProcess) && 
	    (RESET == StatusFlag.stStatusFlag.bInitFlushProcess) &&
		  (RESET == StatusFlag.stStatusFlag.bInitSprayProcess)))
	{
		/*Transmit data over CAN*/
		if(5 <= stTCRead[ucTCNum].ucTCFault)
		{
			/* Invalid data to HMI and SCU*/
			fnDataMsg_Tx(INVALID_FLOAT_VALUE, ucTCNum);/*Also send the channel no.*/
			
			/*RESET Window again for New data*/
			stTCRead[ucTCNum].ucTraverse = 0;
			stTCRead[ucTCNum].fSum = 0.0f;
			memset(stTCRead[ucTCNum].rgfTempBuff, 0, sizeof(stTCRead[ucTCNum].rgfTempBuff));
			stTCRead[ucTCNum].bWindowOverflow = RESET;
			stTCRead[ucTCNum].ucTraverse = 0;
		}
		else 
		{		
			/* Tx Current temp to the Respective Module*/
			fnDataMsg_Tx(stTCRead[ucTCNum].fCurrentTemp, ucTCNum);
		}
	}
}





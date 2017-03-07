/******************************************************************************
 **@file    I2C_GPIO_Expander.c
 * 
 **@brief   This file contains all functions related to GPIO expander and I2C
 *****************************************************************************
 */    
/* ----------------------------------------------------------------------------
*                           Includes
* ----------------------------------------------------------------------------
*/
	#include "Includes.h"

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
 uint8_t ucI2CRxCnt 				= 0;
 uint8_t ucTempHighByte 		= 0;
 uint8_t ucTempLowByte 			= 0;
 uint8_t ucEE07SlaveAddr 		= 0;
 uint8_t ucI2CDirection 		= 0;
 uint8_t ucMuxChannelNum 		= 0;
 uint8_t ucI2CSequence 			= 0; 
 uint8_t ucInsideStatus 		= 0; 
 uint8_t ucOutsideStatus 		= 0;  
 float fInsideTemp 					= 0.0f;
 float fOutsideTemp 				= 0.0f;
 float fOutsideHumidity 		= 0.0f;
 float fDewPoint 						= 0.0f;
 uint16_t uiI2CWriteData   	= 0;
 uint8_t  ucI2C1Seq					=	0;
 uint8_t  ucIOExpSlaveAddr	=	0;
 uint16_t uiAmbiantReadCnt 	= 0;
 uint8_t rgucI2CRxBuff[2]		=	{0};

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
/******************************************************************************
 **@Function 			: fnI2C_Config
 **@Descriptions	: This Funtion Initializes the I2Cs
 **@Parameters		:	None
 **@Return				: None
 ****************************************************************************/
void fnI2C_Config(void)
{
	GPIO_InitTypeDef stGPIOinit;
	I2C_InitTypeDef stI2Cinit;
	NVIC_InitTypeDef stNVIC_InitStructure;
	
	/* enable APB1 peripheral clock for I2C1, I2C2 and I2C3 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
	
	/* enable clock for SCL and SDA pins */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  
	/*Deinit and reset the I2C to avoid it locking up*/
	I2C_DeInit(I2C_IO_EXP);  
	I2C_SoftwareResetCmd(I2C_IO_EXP, ENABLE);
	I2C_SoftwareResetCmd(I2C_IO_EXP, DISABLE);
	
	/*Deinit and reset the I2C to avoid it locking up*/
	I2C_DeInit(I2C_EE07); 
	I2C_SoftwareResetCmd(I2C_EE07, ENABLE);
	I2C_SoftwareResetCmd(I2C_EE07, DISABLE);

	/* setup SCL and SDA pins PB 6 and PB 7 */
	stGPIOinit.GPIO_Pin 	= I2C1_SCL | I2C1_SDA; 	/* we are going to use SCL and SDA*/
	stGPIOinit.GPIO_Mode 	= GPIO_Mode_AF_OD;			/* set pins to alternate function open drain */
	stGPIOinit.GPIO_Speed = GPIO_Speed_50MHz;			/* set GPIO speed */
	GPIO_Init(I2C1_PORT, &stGPIOinit);		
	
	/* setup SCL and SDA pins PB10 and PB11*/
	stGPIOinit.GPIO_Pin 	= I2C2_SCL | I2C2_SDA; 	/* we are going to use SCL and SDA*/
	stGPIOinit.GPIO_Mode 	= GPIO_Mode_AF_OD;			/* set pins to alternate function open drain */
	stGPIOinit.GPIO_Speed = GPIO_Speed_50MHz;			/* set GPIO speed */
	GPIO_Init(I2C2_PORT, &stGPIOinit);
		
	/*configure I2C1 */
	stI2Cinit.I2C_ClockSpeed 					= I2C_IO_EXP_SLOCK_SPEED; 
	stI2Cinit.I2C_Mode 								= I2C_Mode_I2C;					/* I2C mode */
	stI2Cinit.I2C_DutyCycle 					= I2C_DutyCycle_2;			/* 50% duty cycle --> standard */
	/* own address, not relevant in master mode */
	stI2Cinit.I2C_OwnAddress1 				= 0;
	/* disable acknowledge when reading */
	stI2Cinit.I2C_Ack 								= I2C_Ack_Disable;
	/* set address length to 7 bit addresses */
	stI2Cinit.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; 
	I2C_Init(I2C_IO_EXP, &stI2Cinit);													/* initialise I2C2 */	
	I2C_Cmd(I2C_IO_EXP, ENABLE);
	 
	stI2Cinit.I2C_ClockSpeed 					= I2C_EE07_SLOCK_SPEED; 					/*SCLK	< 5kHz */
	stI2Cinit.I2C_Mode 								= I2C_Mode_I2C;				/* I2C mode */
	stI2Cinit.I2C_DutyCycle 					= I2C_DutyCycle_2;/* 50% duty cycle --> standard */
	/* own address, not relevant in master mode */
	stI2Cinit.I2C_OwnAddress1 				= 0;			
	stI2Cinit.I2C_Ack = I2C_Ack_Enable;
	stI2Cinit.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; 
	I2C_Init(I2C_EE07, &stI2Cinit);
  I2C_Cmd(I2C_EE07, ENABLE);
	
		/* Configure the I2C event priority */
	stNVIC_InitStructure.NVIC_IRQChannel                   = I2C1_EV_IRQn;
	stNVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	stNVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0x01;
	stNVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
	NVIC_Init(&stNVIC_InitStructure);
	
	/* Configure the I2C error condition */
	stNVIC_InitStructure.NVIC_IRQChannel                   = I2C1_ER_IRQn;
	stNVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	stNVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0x03;
	stNVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
	NVIC_Init(&stNVIC_InitStructure);
	
	I2C_ITConfig(I2C_IO_EXP , I2C_IT_EVT | I2C_IT_BUF | I2C_IT_ERR, ENABLE);
	I2C_Cmd(I2C_IO_EXP, ENABLE);

	/* Configure the I2C event priority */
	stNVIC_InitStructure.NVIC_IRQChannel                   = EE07_I2C_EV_IRQn;
	stNVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	stNVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0x04;
	stNVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
	NVIC_Init(&stNVIC_InitStructure);
	
	/* Configure the I2C error condition */
	stNVIC_InitStructure.NVIC_IRQChannel                   = EE07_I2C_ER_IRQn;
	stNVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	stNVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0x05;
	stNVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
	NVIC_Init(&stNVIC_InitStructure);
	
	I2C_ITConfig(I2C_EE07 , I2C_IT_EVT | I2C_IT_BUF | I2C_IT_ERR, ENABLE);
	I2C_Cmd(I2C_EE07, ENABLE);
}

/******************************************************************************
 **@Function 			: fnAmbiant_Read	
 **@Descriptions	: This funtion will Read the Ambiant data 
 **@Parameters		: None
 **@Return				: None
 ****************************************************************************/
void fnAmbiant_Read(uint8_t ucSeqNum)
{
	#ifdef EE07_DBG
		DEBUG_PRINTF("fnAmbiant_Read %d\r\n", ucSeqNum);
	#endif
	switch(ucSeqNum)
	{
		/*Start of Outside Ambiant Temp read (Low Byte) */
		case READ_OUT_TEMP_LOW_BYTE:
		{
			#ifdef EE07_DBG
				DEBUG_PRINTF("Outside Temp Read Low Byte init\r\n");
			#endif
			rgucI2CRxBuff[1] 	= 0;
			rgucI2CRxBuff[0] 	= 0;
			ucI2CRxCnt 				= 0;
			ucEE07SlaveAddr 	= TEMP_LOW_BYTE_COMMAND;
			ucI2CDirection 		= I2C_Direction_Receiver;
			I2C_AcknowledgeConfig(I2C_EE07, ENABLE);
			I2C_GenerateSTART(I2C_EE07, ENABLE);
		}
		break;
		/*Start of Outside Ambiant Temp read (High Byte) */
		case READ_OUT_TEMP_HIGH_BYTE:
		{
			if(((TEMP_LOW_BYTE_COMMAND + rgucI2CRxBuff[0]) % 256) == rgucI2CRxBuff[1])
			{
				#ifdef EE07_DBG
					DEBUG_PRINTF("Outside Temp Read High Byte Init \r\n");
				#endif
				ucTempLowByte = rgucI2CRxBuff[0];
				
				ucI2CRxCnt 				= 0;
				ucI2CDirection 		= I2C_Direction_Receiver;
				rgucI2CRxBuff[1] 	= rgucI2CRxBuff[0] = 0;
				ucEE07SlaveAddr 	= TEMP_HIGH_BYTE_COMMAND;
				I2C_AcknowledgeConfig(I2C_EE07, ENABLE);
				I2C_GenerateSTART(I2C_EE07, ENABLE);	
			} 
		}
		break;
		/*Start of Outside Humidity read (Low Byte) */
		case READ_OUT_HUMID_LOW_BYTE:
		{
			#ifdef EE07_DBG1
				DEBUG_PRINTF("Outside Humidity Read Low Byte Init\r\n");
			#endif
			ucI2CRxCnt = 0;
			rgucI2CRxBuff[1] 	= 0;
			rgucI2CRxBuff[0] 	= 0;
			ucEE07SlaveAddr 	= HUM_LOW_BYTE_COMMAND;
			ucI2CDirection 		= I2C_Direction_Receiver;
			I2C_AcknowledgeConfig(I2C_EE07, ENABLE);
			I2C_GenerateSTART(I2C_EE07, ENABLE);
		}
		break;
		/*Start of Outside Humidity read (High Byte) */
		case READ_OUT_HUMID_HIGH_BYTE: 
		{
			if(((HUM_LOW_BYTE_COMMAND + rgucI2CRxBuff[0]) % 256) == rgucI2CRxBuff[1])
			{
				#ifdef EE07_DBG1
					DEBUG_PRINTF("Outside Humidity Read High Byte Init\r\n");
				#endif
				ucTempLowByte = rgucI2CRxBuff[0];
				ucI2CRxCnt 				= 0;
				rgucI2CRxBuff[1] 	= 0;
				rgucI2CRxBuff[0] 	= 0;
				ucEE07SlaveAddr 	= HUM_HIGH_BYTE_COMMAND;
				ucI2CDirection 		= I2C_Direction_Receiver;
				I2C_AcknowledgeConfig(I2C_EE07, ENABLE);
				I2C_GenerateSTART(I2C_EE07, ENABLE);	
			} 
		}
		break;
		case READ_OUT_STATUS:/*Start of Outside Status Reading */
		{			
			#ifdef EE07_DBG1
				DEBUG_PRINTF("Outside Status Read Init\r\n");
			#endif
			ucI2CRxCnt = 0;
			rgucI2CRxBuff[1] 	= 0;
			rgucI2CRxBuff[0] 	= 0;
			ucEE07SlaveAddr 	= STATUS_BYTE_COMMAND;
			ucI2CDirection 		= I2C_Direction_Receiver;
			I2C_AcknowledgeConfig(I2C_EE07, ENABLE);
			I2C_GenerateSTART(I2C_EE07, ENABLE);
		}
		break;
		/*Start of Inside Ambiant Temp read (Low Byte) */
		case READ_IN_TEMP_LOW_BYTE:
		{
			#ifdef EE07_DBG1
				DEBUG_PRINTF("Inside Temp Read Low Byte Init\r\n");
			#endif
			rgucI2CRxBuff[1] 	= 0;
			rgucI2CRxBuff[0] 	= 0;
			ucI2CRxCnt 				= 0;
			ucEE07SlaveAddr 	= TEMP_LOW_BYTE_COMMAND;
			ucI2CDirection 		= I2C_Direction_Receiver;
			I2C_AcknowledgeConfig(I2C_EE07, ENABLE);
			I2C_GenerateSTART(I2C_EE07, ENABLE);
			
		}
		break;
		/*Start of Insside Ambiant Temp read (High Byte) */
		case READ_IN_TEMP_HIGH_BYTE:
		{
			if(((TEMP_LOW_BYTE_COMMAND + rgucI2CRxBuff[0]) % 256) == rgucI2CRxBuff[1])
			{
				#ifdef EE07_DBG1
					DEBUG_PRINTF("Inside Temp Read High Byte Init\r\n");
				#endif
				ucTempLowByte = rgucI2CRxBuff[0];
				
				ucI2CRxCnt = 0;
				rgucI2CRxBuff[1] 	= 0;
				rgucI2CRxBuff[0] 	= 0;
				ucEE07SlaveAddr 	= TEMP_HIGH_BYTE_COMMAND;
				ucI2CDirection 		= I2C_Direction_Receiver;
				I2C_AcknowledgeConfig(I2C_EE07, ENABLE);
				I2C_GenerateSTART(I2C_EE07, ENABLE);	
			} 
		}
		break;
		case READ_IN_STATUS:/*Start of Outside Status Reading */
		{			
			#ifdef EE07_DBG1
				DEBUG_PRINTF("Inside Status Read Init\r\n");
			#endif
			ucI2CRxCnt = 0;
			rgucI2CRxBuff[1] 	= 0;
			rgucI2CRxBuff[0] 	= 0;
			ucEE07SlaveAddr 	= STATUS_BYTE_COMMAND;
			ucI2CDirection 		= I2C_Direction_Receiver;
			I2C_AcknowledgeConfig(I2C_EE07, ENABLE);
			I2C_GenerateSTART(I2C_EE07, ENABLE);
		}
		break;
		default:
		{
			#ifdef EE07_DBG1
				DEBUG_PRINTF("Invalid Case EE07/I2C read\r\n");
			#endif
		}
	}
}
/******************************************************************************
 **@Function 			: fnEE07_Calculation	
 **@Descriptions	: This funtion will Calculate and send the Ambiant Data
 **@Parameters		: None
 **@Return				: None
 ****************************************************************************/
void fnEE07_Calculation(void)
{
	#ifdef EE07_DATA_READ_DBG
		char rgcTemp[150];
	#endif
	
	uint16_t uiTemperature 	= 0;
	uint16_t uiHimidity 		= 0;
	uint8_t ucLoop 					= 0;
	float fConstA 					= CONSTANT_A; /*Constant used in the Dew Point calculation*/
	float fConstB 					= CONSTANT_B; /*Constant used in the DEDew Point calculation*/
	float fConstC 					= 0.0f;
	
	/*check if succesfully read and then calculate the actual values*/
	/*Outside Ambiant Temp calculation*/
	if(AMBIANT_TEMP_OUT_CALC == ucI2CSequence)
	{
		/*Start of Outside Ambiant Temp Calculation) */
		if(((TEMP_HIGH_BYTE_COMMAND + rgucI2CRxBuff[0]) % 256) == rgucI2CRxBuff[1])
		{
			ucTempHighByte 	= rgucI2CRxBuff[0];
			ucI2CRxCnt			=	0;
			uiTemperature 	= (ucTempLowByte + (256 * ucTempHighByte)); 
			fOutsideTemp 		= ((float)uiTemperature / 100) - 273.15f;
			#ifdef EE07_DATA_READ_DBG
				sprintf(rgcTemp,"OAT: %0.2f\r\n", fOutsideTemp);	
				DEBUG_PRINTF("%s", rgcTemp );
			#endif
		}
	}
	/*Outside Ambiant Humidity calculation*/
	else if(AMBIANT_HUMIDITY_OUT_CALC == ucI2CSequence)
	{
		/*Start of Outside Humidity Calculation*/
		if(((HUM_HIGH_BYTE_COMMAND + rgucI2CRxBuff[0]) % 256) == rgucI2CRxBuff[1]) 
		{
			ucTempHighByte 		= rgucI2CRxBuff[0];
			ucI2CRxCnt				=	0;
			uiHimidity 				= (ucTempLowByte + (256 * ucTempHighByte)); 
			fOutsideHumidity 	= ((float)uiHimidity / 100);
			#ifdef EE07_DATA_READ_DBG
				sprintf(rgcTemp,"OH:  %0.2f\r\n", fOutsideHumidity);	
				DEBUG_PRINTF("%s", rgcTemp );
			#endif
		}
	}
	/*Outside Status calculation*/
	else if(AMBIANT_STATUS_OUT_CALC == ucI2CSequence)
	{
		/*Oustside status read*/
		if(((STATUS_BYTE_COMMAND + rgucI2CRxBuff[0]) % 256) == rgucI2CRxBuff[1])
		{
			ucI2CRxCnt = 0;
			ucOutsideStatus = rgucI2CRxBuff[0];
			#ifdef EE07_DATA_READ_DBG
				DEBUG_PRINTF("OS: %d\r\n", ucOutsideStatus);
			#endif
			if(0 == (ucOutsideStatus & 0x03))/*check temp and humidity bits*/
			{
				/*Send Outside Ambiant temp to SCU and HMI*/
				stTxMsg.DLC = 4;
				SplitByte.fData = fOutsideTemp;
				for(ucLoop = 0; ucLoop < stTxMsg.DLC; ucLoop++)
				{
					stTxMsg.Data[ucLoop] = SplitByte.rgcData[ucLoop];
				}
				fnData_Send(DATA_AMB_TEMP_OUTSIDE);
				
				/*Send Outside Humidity to SCU and HMI*/
				SplitByte.fData = fOutsideHumidity;
				for(ucLoop = 0; ucLoop < stTxMsg.DLC; ucLoop++)
				{
					stTxMsg.Data[ucLoop] = SplitByte.rgcData[ucLoop];
				}
				fnData_Send(DATA_AMB_HUMIDITY);
				
				/*Dew point Calculation*/
				/*Uncertainity in the calculation is +/-0.4 degree Celcius*/
				fConstC = ((log(fOutsideHumidity / 100) +	((fConstA * fOutsideTemp) /
									 (fConstB + fOutsideTemp))) / fConstA);
					
				fDewPoint = ((fConstB * fConstC) / (1 - fConstC));
				SplitByte.fData = fDewPoint;
				for(ucLoop = 0; ucLoop < stTxMsg.DLC; ucLoop++)
				{
					stTxMsg.Data[ucLoop] = SplitByte.rgcData[ucLoop];
				}
				stTxMsg.ExtId = MSG_TYPE_DATA | DATA_DEW_POINT;
				fnData_Send(DATA_DEW_POINT);
				#ifdef EE07_DATA_READ_DBG
					sprintf(rgcTemp,"DP:  %0.2f\r\n", fDewPoint);	
					DEBUG_PRINTF("%s", rgcTemp );
				#endif
			}
		}
	}
	/*Inside Ambiant Temp calculation*/
	else if(AMBIANT_TEMP_IN_CALC == ucI2CSequence) 
	{
		/*Start of Outside Ambiant Temp Calculation) */
		if(((TEMP_HIGH_BYTE_COMMAND + rgucI2CRxBuff[0]) % 256) == rgucI2CRxBuff[1])
		{
			ucTempHighByte 	= rgucI2CRxBuff[0];
			ucI2CRxCnt			=0;
			uiTemperature 	= (ucTempLowByte + (256 * ucTempHighByte)); 
			fInsideTemp 		= ((float)uiTemperature / 100) - 273.15f;
			#ifdef EE07_DATA_READ_DBG
				sprintf(rgcTemp,"IAT: %0.2f\r\n", fInsideTemp);	
				DEBUG_PRINTF("%s", rgcTemp );
			#endif
		}
	}
	/*Inside Status calculation*/
	else if(AMBIANT_STATUS_IN_CALC == ucI2CSequence)
	{
		if(((STATUS_BYTE_COMMAND + rgucI2CRxBuff[0]) % 256) == rgucI2CRxBuff[1])
		{
			ucI2CRxCnt = 0;
			ucInsideStatus = rgucI2CRxBuff[0];
			#ifdef EE07_DATA_READ_DBG
				DEBUG_PRINTF("IS: %d\r\n",ucInsideStatus);
			#endif
			if(0 == (ucOutsideStatus & 0x01))/*check Ambiant temp  bit*/
			{
				stTxMsg.DLC = 4;
				SplitByte.fData = fInsideTemp;
				for(ucLoop = 0; ucLoop < stTxMsg.DLC; ucLoop++)
				{
					stTxMsg.Data[ucLoop] = SplitByte.rgcData[ucLoop];
				}
				fnData_Send(DATA_AMB_TEMP_INSIDE);
			}
		}
		StatusFlag.stStatusFlag.bAmbiantReadDone = SET;
		ucI2CSequence = 0;
	}
}
/******************************************************************************
 **@Function 			: fnEE07_Process	
 **@Descriptions	: This funtion will Read and Calculate Ambiant data
 **@Parameters		: None
 **@Return				: None
 ****************************************************************************/
void fnEE07_Process(void)
{		
	#ifdef EE07_DBG
		DEBUG_PRINTF("ucI2CSequence: %d\r\n",ucI2CSequence);
	#endif
	if(EE07_READ_TIME <= uiAmbiantReadCnt)
	{
		#ifdef EE07_DBG
		  DEBUG_PRINTF("ucI2CSequence: %d\r\n",ucI2CSequence);
		#endif
		if(CHANNEL_OUT == ucI2CSequence)
		{
			#ifdef EE07_DBG
				DEBUG_PRINTF("switch to Outside\r\n");
			#endif
			/*Channel should be selected according to Remap of Outside sensor*/
			ucEE07SlaveAddr = MUX_ADDR;
			ucI2CDirection 	= I2C_Direction_Transmitter;
			ucMuxChannelNum = GET_EE07_CHANNEL(ucOutsideRemap);;
			/* Send I2C1 START condition */
			I2C_GenerateSTART(I2C_EE07, ENABLE);
		}
		else if(CHANNEL_IN == ucI2CSequence)
		{
			#ifdef EE07_DBG
				DEBUG_PRINTF("switch to Inside\r\n");
			#endif
			/*Channel should be selected according to Remap of Inside sensor*/
			ucEE07SlaveAddr = MUX_ADDR ;
			ucI2CDirection 	= I2C_Direction_Transmitter;
			ucMuxChannelNum = GET_EE07_CHANNEL(ucInsideRemap); 
			/* Send I2C1 START condition */
			I2C_GenerateSTART(I2C_EE07, ENABLE);
		}
		else if(READ_IN_STATUS  >= ucI2CSequence)
		{
			fnAmbiant_Read(ucI2CSequence);
		}	
		uiAmbiantReadCnt = 0;
		ucI2CSequence++;
	}
	if(SET == StatusFlag.stStatusFlag.bI2CReadDone)
	{
		#ifdef EE07_DBG
			DEBUG_PRINTF("EE07 Calc I2CReadDone rgc1\r\n");
		#endif
		fnEE07_Calculation();
		/*This flag SET in the I2C EV handler when read successfully*/
		StatusFlag.stStatusFlag.bI2CReadDone = RESET; 
	}
}


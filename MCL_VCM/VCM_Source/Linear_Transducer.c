/******************************************************************************
  * @file    Linear_Transducer.h
  * 
  * @brief   
  ****************************************************************************
  */
/* ----------------------------------------------------------------------------
 *                           Includes
 * ---------------------------------------------------------------------------
 */
	#include "Includes.h"
/* ----------------------------------------------------------------------------
 *                           MACROS
 * ---------------------------------------------------------------------------
 */
/* ----------------------------------------------------------------------------
 *                           CONSTANTS
 * ---------------------------------------------------------------------------
 */
/* ----------------------------------------------------------------------------
 *                           GLOBAL VARIABLES
 * ---------------------------------------------------------------------------
 */
	float fLinearTransMin 			= 75.0f;
	float fLinearTransMax 			= 75.0f;
	float fLinearTransCurrRead 	= 0.0f;
	float fLTTotalLength 				= 0.0f;
	float fLTFirstRead 					= 0.0f;
	float fLTLastRead 					= 0.0f;
	float fLTFirstLength 				= 0.0f;
	float fLTLastLength 				= 0.0f;
	float fMaterialUsedA 				= 0.0f;
	float fMaterialUsedB 				= 0.0f;
	float fMaterialUsedC 				= 0.0f;
	float fLinearTrns 					= 0.0f;				/*Linear Transducer reading*/
	float fCylinderAreaA 				= 0.0f;				/*Area of Cylinder A*/
	float fCylinderAreaB 				= 0.0f;				/*Area of Cylinder B*/
	float fCylinderAreaC 				= 0.0f;				/*Area of Cylinder C*/
	
	uint8_t ucLTPostion 				= 0;
	uint8_t ucStrokeOfType 			= NONE;
/* ----------------------------------------------------------------------------
 *                           EXTERNAL VARIABLES
 * ---------------------------------------------------------------------------
 */
 /* ---------------------------------------------------------------------------
 *                           EXTERNAL FUNCTIONS
 * ---------------------------------------------------------------------------
 */
/* ----------------------------------------------------------------------------
 *                           FUNCTIONS DECLARITION
 * ---------------------------------------------------------------------------
 */
/******************************************************************************
 **@Function 		: fnLinearTransLimit
 **@Descriptions: Function to get linear transducer limits Starting and ending.
 **@Parameters	: None 
 **@Return			: None
 ****************************************************************************/
void fnLinearTransLimit(void)
{
	#ifdef LT_LIMIT_DBG
		char rgcTemp[200];
	#endif
	fLinearTransCurrRead = fnLinearPosition();
	if((HIGH_LT_LEVEL >= fLinearTransCurrRead) &&
		 (LOW_LT_LEVEL < fLinearTransCurrRead))
	{
		/*change4*/
		if((fLinearTransMin > fLinearTransCurrRead) && ( 1 < fLinearTransCurrRead))
		{
			fLinearTransMin = fLinearTransCurrRead;
	#ifdef LT_LIMIT_DBG
				sprintf(rgcTemp,"fLinearTransMin--- = %0.2f\r\n",	fLinearTransMin);	
				DEBUG_PRINTF("%s",rgcTemp ); 
		#endif
		}
		if(fLinearTransMax < fLinearTransCurrRead)
		{
			fLinearTransMax = fLinearTransCurrRead;
			#ifdef LT_LIMIT_DBG
				sprintf(rgcTemp,"fLinearTransMax--- = %0.2f\r\n\n\n", fLinearTransMax);	
				DEBUG_PRINTF("%s",rgcTemp );
			#endif
		}
	}
	fLinearTransCurrRead = 0;
}

/******************************************************************************
 **@Function 		  : fnLTFirstLastLength_Read
 **@Descriptions	: Will give the 1st and last length of linear transducer 
 **@Parameters		: fLTRead : 1st or Last reading 
										ucTypeOfStroke : Type of Stroke(upstroke or downstroke)
 **@Return				: None
 ****************************************************************************/
float fnLTFirstLastLength_Read(float fLTRead, uint8_t ucTypeOfStroke, uint8_t ucLType)
{
	float fLTLength = 0.0f;
	if(DOWNSTROKE == ucTypeOfStroke)
	{
		if(FIRST_LENGTH == ucLType)
		{				
			if( LT_EXT_INT_VALUE < fLTRead)
			{
				//(- sign) for reducing from the total length
				//that means current count will be increamented and 
				//this much length is extra and this should be remove from the total length
				return (fLTRead - fLinearTransMax); 
			}
			else
			{
				//(+ sign) for Add to the total length
				//that means extra pumping i.e length to be added, 
				//as the count for the current will not be increamented anymore
				return (fLTRead - fLinearTransMin); 
			}
		}
		else if(LAST_LENGTH == ucLType)
		{
			if( LT_EXT_INT_VALUE < fLTRead)
			{
				//(+ sign) for Add to the total length
				//that means extra pumping i.e to be added in total length as  count is not increamented yet
				return (fLinearTransMax - fLTRead);
			}
			else
			{
				//(- sign) for reducing from the total length
        //that means left to complete the current count total values
				return (fLinearTransMin - fLTRead); 
			}
		}
	}
	else if(UPSTROKE == ucTypeOfStroke)
	{
		if(FIRST_LENGTH == ucLType)
		{				
			if( LT_EXT_INT_VALUE < fLTRead) /*Greater than INTR*/
			{
				//(sign)  for Add to the total length
				//that means extra pumping i.e length to be added, 
				//as the count for the current will not be increamented anymore
				
				return (fLinearTransMax - fLTRead); 
			}
			else
			{
				//(- sign) for reducing from the total length
				//that means current count will be increamented and 
				//this much length is extra and this should be remove from the total length
				return (fLinearTransMin - fLTRead); 
			}
		}
		else if(LAST_LENGTH == ucLType)
		{
			if( LT_EXT_INT_VALUE < fLTRead)
			{
				//(- sign) for reducing from the total length
				//that means left to complete the current count total values
				return (fLTRead - fLinearTransMax);
			}
			else
			{
				//(+ sign) for Add to the total length
        //that means extra pumping i.e to be added in total length as 
				//count is not increamented yet
				return (fLTRead - fLinearTransMin); 
			}
		}
	}
	return fLTLength;
}

/******************************************************************************
 **@Function 			: fnStrokeType
 **@Descriptions	: function to find the type of stroke
 **@Parameters		: None
 **@Return				: type of stroke i.e Upstroke or downstroke.Until stroke 
										type is not recognized it will return NONE.
 ****************************************************************************/
uint8_t fnStrokeType(void)
{
	if(RESET == StatusFlag.stStatusFlag.bReadValue1)
	{
		fValue1 = fnLinearPosition();
		StatusFlag.stStatusFlag.bReadValue1 = SET;
	}
	if((STALL_COUNTER < uiStallCounter) && (RESET == StatusFlag.stStatusFlag.bReadValue2))
	{
		uiStallCounter = 0;
		fValue2 = fnLinearPosition();
		StatusFlag.stStatusFlag.bReadValue2 = SET;
	}
	if(SET == StatusFlag.stStatusFlag.bReadValue2)
	{
		if(fValue1 < fValue2)
		{
			/*Upstroke*/
				ucStrokeOfType = UPSTROKE;
		}
		if(fValue1 > fValue2)
		{
			/*Downstroke*/
			ucStrokeOfType = DOWNSTROKE;
		}
		StatusFlag.stStatusFlag.bReadValue1 = RESET;
		StatusFlag.stStatusFlag.bReadValue2 = RESET;
		return ucStrokeOfType;
	}
	return NONE;
}

/******************************************************************************
 **@Function		 	: fnLT_Process
 **@Descriptions	: This Function is used to Process the Linerar Transducer, 
										Reads Posiition and sends Position of LT, material 
										calculation and Faults(if any) to SCU and HMI
 **@Parameters		: None
 **@Return				: None
 ****************************************************************************/
void fnLT_Process(void)
{
	uint8_t ucLoop = 0;
	#ifdef LT_DBG  
	 uint8_t	rgcTemp[100];	
	#endif
	fnLinearPosition();
	if((LT_CHECKCOUNTER < uiLTCheckCounter) &&
	   (RESET == StatusFlag.stStatusFlag.bLTRangeCheck ))
	{
		uiLTCheckCounter = 0;
		
		stTxMsg.DLC = 4;
		SplitByte.fData = fLinearTrns;
		
		for(ucLoop = 0; ucLoop < stTxMsg.DLC; ucLoop++)
		{
			stTxMsg.Data[ucLoop] = SplitByte.rgucData[ucLoop];
		}
		
		fnData_Send(DATA_LT_READ);
		
		if(HIGH_LT_LEVEL < fLinearTrns )
		{
			StatusFlag.stStatusFlag.bLTRangeCheck = SET;
			stTxMsg.DLC = 0;
			fnFault_Send(FAULT_LT_OUT_OF_RANGE);

			uiIOExpander1 |= LT_LED_VALUE(RED);/*FAULT/RED LED ON*/
			uiIOExpander1 &= ~LT_LED_VALUE(GREEN);/*FINE/GREEN LED OFF*/
		}
		else
		{
			uiIOExpander1 &= ~LT_LED_VALUE(RED);/*FAULT/RED LED OFF*/
			uiIOExpander1 |= LT_LED_VALUE(GREEN);/*FINE/GREEN LED ON*/
		}
	}
	/*
	if((StatusFlag.stStatusFlag.bStartMonitorLT == SET) && 
		 (uiMaterialUsageCnt >= MATERIAL_USAGE_CHECKCOUNTER))
	{
		uiMaterialUsageCnt = 0;
		fnMaterial_Used();
	}
	*/
	/*change2*/
/*if(((LT_EXT_INT_VALUE + 5)  < fLinearTrns) || ((LT_EXT_INT_VALUE - 5)  > fLinearTrns))
{
	StatusFlag.stStatusFlag.bExtInt	= RESET;
}*/  
	if(((75	< fLinearTrns) && (90	> fLinearTrns)) || (12  > fLinearTrns))
	{
		StatusFlag.stStatusFlag.bExtInt	= RESET;
	}  
	/*Finding Limits of Linear Transducer*/
	//if((10 > uiPumpCount) &&  (1 <= uiLinearTransCnt) &&
	//	 (RESET == StatusFlag.stStatusFlag.bLTFindLimit))
		/*Finding Limits of Linear Transducer change3*/
	if((1 <= uiLinearTransCnt) &&
		 ((RESET ==StatusFlag.stStatusFlag.bStartMonitorLT )))
	{
		uiLinearTransCnt = 0;
		fnLinearTransLimit();
	}
	
	if(SET == StatusFlag.stStatusFlag.bStartMonitorLT )
	{
		/*fLinearTransCurrRead = fnLinearPosition();*/
		/*For 1st Length Read*/
		if(RESET == StatusFlag.stStatusFlag.bLTFirstLength)
		{
			ucStrokeOfType = fnStrokeType();
			if(NONE != ucStrokeOfType)
			{
				fLTFirstLength = fnLTFirstLastLength_Read(fLTFirstRead,
																									ucStrokeOfType, FIRST_LENGTH);
				StatusFlag.stStatusFlag.bLTFirstLength = SET;
				
				#ifdef LT_DBG  
					sprintf(rgcTemp,"fLTFirstLength = %0.04f Read = %0.04f\r\n", fLTFirstLength, fLTFirstRead );	
					DEBUG_PRINTF("%s",rgcTemp );
				#endif
			}
		}
	}
	if(SET == StatusFlag.stStatusFlag.bStopMonitorLT)
	{
		/*For Last Length Read*/
		if(RESET == StatusFlag.stStatusFlag.bLTLastLength )
		{
			ucStrokeOfType = fnStrokeType();
			if( NONE != ucStrokeOfType )
			{
				fLTLastLength = fnLTFirstLastLength_Read(fLTLastRead,
																								 ucStrokeOfType, LAST_LENGTH);
				#ifdef LT_DBG
					sprintf(rgcTemp,"fLTLastLengt-  = %0.04f Read = %0.04f\r\n",
									fLTLastLength, fLTLastRead);	
					DEBUG_PRINTF("%s",rgcTemp );
				#endif
				StatusFlag.stStatusFlag.bLTLastLength = RESET;
				StatusFlag.stStatusFlag.bLTFirstLength = RESET;
				StatusFlag.stStatusFlag.bStopMonitorLT = RESET;
				
				fnMaterial_Used();
			}
		}
	}
}

/******************************************************************************
 **@Function 			: fnMaterial_Used
 **@Descriptions	: measure the material used.
 **@parameters		: None 
 **@return				: None
 ****************************************************************************/
void fnMaterial_Used(void)
{
	uint8_t ucLoop;
	float fTotalLength = 0.0f;
	#ifdef MAT_CAL_DBG
		char rgcTemp[300];
	#endif
	fTotalLength =  (((uiPumpCount) * (fLinearTransMax - fLinearTransMin)) +
										fLTFirstLength + fLTLastLength);
	#ifdef MAT_CAL_DBG
		sprintf(rgcTemp,"fTotalLength : %0.2f \r\n",fTotalLength);
		DEBUG_PRINTF("%s\r\n",rgcTemp);
		sprintf(rgcTemp,"uiPumpCount : %0.2f \r\n",(float)uiPumpCount);
		DEBUG_PRINTF("%s\r\n",rgcTemp);
		sprintf(rgcTemp,"fLinearTransMin : %0.2f \r\n",fLinearTransMin);
		DEBUG_PRINTF("%s\r\n",rgcTemp);
		sprintf(rgcTemp,"fLinearTransMax : %0.2f \r\n",fLinearTransMax);
		DEBUG_PRINTF("%s\r\n",rgcTemp);
		sprintf(rgcTemp,"fLTFirstLength : %0.2f \r\n",fLTFirstLength);
		DEBUG_PRINTF("%s\r\n",rgcTemp);
		sprintf(rgcTemp,"fLTLastLength : %0.2f \r\n",fLTLastLength);
		DEBUG_PRINTF("%s\r\n",rgcTemp);
	#endif
	/*Cylinder A*/
	fMaterialUsedA = (fCylinderAreaA * ucCylinderCount * fTotalLength);
//	fMaterialUsedA = 100000;
	#ifdef MAT_CAL_DBG
		sprintf(rgcTemp,"Area A = %0.2f MAterial Used by A : %0.2f \r\n",fCylinderAreaA, fMaterialUsedA);
		DEBUG_PRINTF("%s\r\n",rgcTemp);
	#endif
	
	stTxMsg.DLC 		= 5;
	stTxMsg.Data[0] = CYLINDER_A;
	SplitByte.fData = fMaterialUsedA;
	for(ucLoop = 1; ucLoop < stTxMsg.DLC; ucLoop++)
	{
		stTxMsg.Data[ucLoop] = SplitByte.rgucData[ucLoop - 1];
	}
	fnData_Send(DATA_MATERIAL_USED);
	
	/*Cylinder B*/
	fMaterialUsedB = (fCylinderAreaB * ucCylinderCount * fTotalLength);
//	fMaterialUsedB = 100000;
	#ifdef MAT_CAL_DBG
		sprintf(rgcTemp,"Area B = %0.2f MAterial Used by B : %0.2f \r\n",fCylinderAreaB, fMaterialUsedB);
		DEBUG_PRINTF("%s\r\n",rgcTemp);
	#endif
	
	stTxMsg.DLC = 5;
	stTxMsg.Data[0] = CYLINDER_B;
	SplitByte.fData = fMaterialUsedB;
	for(ucLoop = 1; ucLoop < stTxMsg.DLC; ucLoop++)
	{
		stTxMsg.Data[ucLoop] = SplitByte.rgucData[ucLoop - 1];
	}
	fnData_Send(DATA_MATERIAL_USED);
	
	/*Cylinder C*/
	fMaterialUsedC = (fCylinderAreaC * ucCylinderCount * fTotalLength);
//	fMaterialUsedC = 100000;
	#ifdef MAT_CAL_DBG
		sprintf(rgcTemp,"Area C = %0.2f MAterial Used by C : %0.2f \r\n",fCylinderAreaC, fMaterialUsedC);
		DEBUG_PRINTF("%s\r\n",rgcTemp);
	#endif
	stTxMsg.DLC 		= 5;
	stTxMsg.Data[0] = CYLINDER_C;
	SplitByte.fData = fMaterialUsedC;
	for(ucLoop = 1; ucLoop < stTxMsg.DLC; ucLoop++)
	{
		stTxMsg.Data[ucLoop] = SplitByte.rgucData[ucLoop - 1];
	}
	fnData_Send(DATA_MATERIAL_USED);
	
	uiPumpCount 				= 0;
	uiMaterialUsageCnt 	= 0;
	fMaterialUsedA 			= 0;
	fMaterialUsedB 			= 0;
	fMaterialUsedC 			= 0;
}
/******************************************************************************  
 **@Function 		 : fnLinearPosition  
 **@Description  : This function Converts the value on given configured channel
									 to postion of linear transducer
 **@Parameters   : channel : chanel number for the  selected ADC  
 **@Return       : Linear transducer value in mm(millimeter)  
 ****************************************************************************/  
float fnLinearPosition(void)
{
	#ifdef GENERAL_DBG
		char rgcTemp[150];
	#endif
	
	/*Value from the Linear transducer*/
	fLinearTrns = fnADC_value(LINEAR_TRANSDUCER_CH(ucLTPostion));
	#ifdef GENERAL_DBG
		sprintf(rgcTemp,"LT Read voltage  = %0.2f\r\n", fLinearTrns);	
		DEBUG_PRINTF("%s", rgcTemp ); 
	#endif
	/*converting liquid level from Voltage to mm and 0.6 voltage corresponds 
	  to 0 mm and 3.0V corresponds to 150 mm*/
  fLinearTrns = (((fLinearTrns - LOW_LT_V) * LT_SLOPE) + LOW_LT_LEVEL);
	#ifdef GENERAL_DBG
		sprintf(rgcTemp, "Linear Transducer in value  = %0.1f mm\r",
					  fLinearTrns);	
		DEBUG_PRINTF("%s",rgcTemp ); 
	#endif 		
	return fLinearTrns; 
}

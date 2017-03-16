/******************************************************************************
  * @file    Pressure_Gauge.c
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
 PG_t stPG[NUM_OF_PG];
 
 float fCurrentPG1 = 0.0f;
 float fCurrentPG2 = 0.0f;
 float fCurrentPG3 = 0.0f;
 
 float fPG1_LowTH = 0.0f;							/*Pressure Gauge1 Low Thershold */
 float fPG1_HighTH = 0.0f;						/*Pressure Gauge1 High Thershold*/
 
 float fPG2_LowTH = 0.0f;							/*Pressure Gauge2 Low Thershold */
 float fPG2_HighTH = 0.0f;						/*Pressure Gauge2 High Thershold*/
 
 float fPG3_LowTH = 0.0f;							/*Pressure Gauge3 Low Thershold */
 float fPG3_HighTH = 0.0f;						/*Pressure Gauge3 High Thershold*/
 
 uint8_t ucPositionPG1 = 1;		  						/*Connected position on the Board connector*/
 uint8_t ucPositionPG2 = 2;		  						/*Connected position on the Board connector*/
 uint8_t ucPositionPG3 = 3;		  						/*Connected position on the Board connector*/
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
 **@Function 			: fnAllFlags_Reset
 **@Descriptions	: Clear all the flags to RESET Condition
 **@Parameters		: None 
 **@Return				: None
 ****************************************************************************/
void fnPGSensor_Read(uint8_t ucCallFrom)
{	
	uint8_t ucLoop 	= 0;
	uint8_t ucPGNum = 0;
	float fADCRead = 0.0f;
	#ifdef PG_DBG
		char rgcTemp[250];
	#endif
	for(ucPGNum = 0; ucPGNum < NUM_OF_PG; ucPGNum++)
	{
		fADCRead = fnADC_value(PG_ADC_CHANNLE(PG_POSITION(ucPGNum)));

		fADCRead = (((fADCRead - LOW_PRESSURE_V) * 
											PRESSURE_GAUGE_SLOPE) + LOW_PRESSURE_LEVEL);
		if(0 == ucPGNum)
		{
			fCurrentPG1 = fADCRead;
		}
		else if(1 == ucPGNum)
		{
			fCurrentPG2 = fADCRead;
		}
		else if(2 == ucPGNum)
		{
			fCurrentPG3 = fADCRead;
		}
	}
	DEBUG_PRINTF("Read & SenD!!!!!\r\n");
	#ifdef PG_DBG
		DEBUG_PRINTF("Sending pressure readings!!!!!\r\n");
	
	sprintf(rgcTemp," pressure readings 1---%f\r\n",	fCurrentPG1);	
				DEBUG_PRINTF("%s",rgcTemp ); 
	
	sprintf(rgcTemp," pressure readings 2---%f\r\n",	fCurrentPG2);	
				DEBUG_PRINTF("%s",rgcTemp );
	
	sprintf(rgcTemp," pressure readings 3---%f\r\n",	fCurrentPG3);	
				DEBUG_PRINTF("%s",rgcTemp );	
	#endif
	if(PG_ROCESS == ucCallFrom)
	{
		stTxMsg.DLC = 4;
		for(ucPGNum = 0; ucPGNum < NUM_OF_PG; ucPGNum++)
		{
			if((CURRENT_PG_DATA(ucPGNum) > MIN_PRESSURE) &&
				 (CURRENT_PG_DATA(ucPGNum) < MAX_PRESSURE))
			{
				SplitByte.fData = CURRENT_PG_DATA(ucPGNum);
			}
			else
			{
				SplitByte.fData = INVALID_DATA;/*Faulty Reading*/
			}
			for(ucLoop = 0; ucLoop < stTxMsg.DLC; ucLoop++)
			{
				stTxMsg.Data[ucLoop] = SplitByte.rgucData[ucLoop];
			}
			fnData_Send(PG_CAN_MSG_ID(ucPGNum));
		}
	}
}

/******************************************************************************
 **@Function		 	: fnPG_Process
 **@Descriptions	: This Function is used to Process the Pressure Gauge, Read
										PG and Send the data and fault(if any) to SCU And HMI.
 **@Parameters		: None
 **@Return				: None
 ****************************************************************************/
void fnPG_Process(void)
{
	uint8_t ucPGNum = 0;
	#ifdef PG_DBG
		char rgcTemp[500];
	#endif
	uiPressureReadCounter = 0;
	/*Get values for Pressure Gauge*/
	fnPGSensor_Read(PG_ROCESS);
	

	/* Check for any faults */
	for(ucPGNum = 0; ucPGNum < NUM_OF_PG; ucPGNum++)
	{		
	#ifdef PG_DBG	
		sprintf(rgcTemp," PG %d LW---%f\r\n",(ucPGNum+1),	PG_LOW_TH(ucPGNum));	
				DEBUG_PRINTF("%s",rgcTemp ); 
		sprintf(rgcTemp," PG %d HI---%f\r\n",(ucPGNum+1),	PG_HIGH_TH(ucPGNum));	
				DEBUG_PRINTF("%s",rgcTemp );
 #endif
		if((CURRENT_PG_DATA(ucPGNum) > PG_LOW_TH(ucPGNum)) &&
			 (CURRENT_PG_DATA(ucPGNum) < PG_HIGH_TH(ucPGNum)))
		{
			/*Turn ON green LED*/
			/*Turn OFF Red LED*/
			/*Turn OFF Blue LED*/	
			if(PG3 == ucPGNum)
			{
				uiIOExpander2 &= ~PG_LED_VALUE(ucPGNum, RED);
				uiIOExpander2 &= ~PG_LED_VALUE(ucPGNum, BLUE);
				uiIOExpander2 |= PG_LED_VALUE(ucPGNum, GREEN);
			}
			else
			{
				uiIOExpander1 &= ~PG_LED_VALUE(ucPGNum, RED);
				uiIOExpander1 &= ~PG_LED_VALUE(ucPGNum, BLUE);
				uiIOExpander1 |= PG_LED_VALUE(ucPGNum, GREEN);
			}
			stPG[ucPGNum].ucLTHFaultCnt = 0;
			stPG[ucPGNum].ucHTHFaultCnt = 0;
		}
		else																										
		{
			#ifdef	PG_DBG
						DEBUG_PRINTF("Pressure gauge Fault Processing %d\r\n",ucPGNum);
			#endif
			
			/*Low Thershold Fault*/
			if(CURRENT_PG_DATA(ucPGNum) < PG_LOW_TH(ucPGNum))
			{
				if(FAULT_COUNT >= stPG[ucPGNum].ucLTHFaultCnt)
				{
					/*Increment the Low threshold Fault counter for the
						respective pressure gauge*/							
					stPG[ucPGNum].ucLTHFaultCnt++;
					stPG[ucPGNum].ucHTHFaultCnt = 0;
					
					if(stPG[ucPGNum].ucLTHFaultCnt == FAULT_COUNT)
					{
						#ifdef PG_DBG
							DEBUG_PRINTF("send low thershold fault PG%d\r\n", ucPGNum);
						#endif
						stPG[ucPGNum].bLTHFaultFlag = SET;
						stTxMsg.DLC = 2;
						stTxMsg.Data[0] = 0x01 << ucPGNum;/*specify which PG in Fault*/
						stTxMsg.Data[1] = 0x01;/*Low thershold Fault indication*/
						fnFault_Send(FAULT_PRESSURE_BEYOND_TH);
						/*Turn OFF green LED*/
						/*Turn OFF red LED*/
						/*Turn ON Blue LED*/
						if(PG3 == ucPGNum)
						{
							uiIOExpander2 &= ~PG_LED_VALUE(ucPGNum, RED);
							uiIOExpander2 |= PG_LED_VALUE(ucPGNum, BLUE);
							uiIOExpander2 &= ~PG_LED_VALUE(ucPGNum, GREEN);
						}
						else
						{
							uiIOExpander1 &= ~PG_LED_VALUE(ucPGNum, RED);
							uiIOExpander1 |= PG_LED_VALUE(ucPGNum, BLUE);
							uiIOExpander1 &= ~PG_LED_VALUE(ucPGNum, GREEN);
						}
					}
				}
			}
			/*High Thershold Fault*/
			if(CURRENT_PG_DATA(ucPGNum) > PG_HIGH_TH(ucPGNum))
			{
				if(FAULT_COUNT >= stPG[ucPGNum].ucHTHFaultCnt)
				{
					/*Increment the High threshold Fault counter for the 
						respective pressure gauge*/							
					stPG[ucPGNum].ucLTHFaultCnt = 0;
					stPG[ucPGNum].ucHTHFaultCnt++;
											
					if(FAULT_COUNT == stPG[ucPGNum].ucHTHFaultCnt)
					{
						#ifdef	PG_DBG
							DEBUG_PRINTF("send high TH fault PG%d\r\n", ucPGNum);
						#endif
						
						stPG[ucPGNum].bHTHFaultFlag = SET;
						stTxMsg.DLC = 2;
						stTxMsg.Data[0] = 0x01 << ucPGNum;/*specify which PG Fault*/
						stTxMsg.Data[1] = 0x02;/*High Thershold Fault Indication*/
						fnFault_Send(FAULT_PRESSURE_BEYOND_TH);				
						/*Turn OFF green LED*/
						/*Turn ON red LED*/
						/*Turn OFF Blue LED*/
						if(PG3 == ucPGNum)
						{
							uiIOExpander2 |= PG_LED_VALUE(ucPGNum, RED);
							uiIOExpander2 &= ~PG_LED_VALUE(ucPGNum, BLUE);
							uiIOExpander2 &= ~PG_LED_VALUE(ucPGNum, GREEN);
						}
						else
						{
							uiIOExpander1 |= PG_LED_VALUE(ucPGNum, RED);
							uiIOExpander1 &= ~PG_LED_VALUE(ucPGNum, BLUE);
							uiIOExpander1 &= ~PG_LED_VALUE(ucPGNum, GREEN);
						}
					}	
				}							
			}
		}
  }/*end of for loop*/
}


SplitByte.uliData = 0;
		fnSPI_EEPROM_ReadData(SCU_SPRAY_PRESSURE_VALUE, 4, SplitByte.rgucData);
		stConfigData.fConfig_SprayPressure = SplitByte.fData;
		#ifdef GENERAL_DBG_EE
			memset(rgcRDData, 0, READ_BUFFER_SIZE);
			sprintf(rgcRDData,"%f", stConfigData.fConfig_SprayPressure);
			DEBUG_PRINTF("SCU_SPRAY_PRESSURE_VALUE = %s\r\n", rgcRDData);	
		#endif

		SplitByte.uliData = 0;
		fnSPI_EEPROM_ReadData(SCU_SPRAY_PRESSURE_THRESH_LOW, 4, SplitByte.rgucData);
		stConfigData.fConfig_SprayPressureLTH = SplitByte.fData;
		#ifdef GENERAL_DBG_EE
			memset(rgcRDData, 0, READ_BUFFER_SIZE);
			sprintf(rgcRDData,"%f", stConfigData.fConfig_SprayPressureLTH);
			DEBUG_PRINTF("SCU_SPRAY_PRESSURE_THRESH_LOW = %s\r\n", rgcRDData);	
		#endif


		SplitByte.uliData = 0;
		fnSPI_EEPROM_ReadData(SCU_SPRAY_PRESSURE_THRESH_HIGH, 4, SplitByte.rgucData);
		stConfigData.fConfig_SprayPressureHTH = SplitByte.fData;
		#ifdef GENERAL_DBG_EE
			memset(rgcRDData, 0, READ_BUFFER_SIZE);
			sprintf(rgcRDData,"%f", stConfigData.fConfig_SprayPressureHTH);
			DEBUG_PRINTF("SCU_SPRAY_PRESSURE_THRESH_HIGH = %s\r\n", rgcRDData);	
		#endif


SplitByte.uliData = 0;
		fnSPI_EEPROM_ReadData(SCU_SPRAY_PRESSURE_HYST_POS, 4, SplitByte.rgucData);
		stConfigData.fConfig_HysSprayPresPos = SplitByte.fData;
		#ifdef GENERAL_DBG_EE
			memset(rgcRDData, 0, READ_BUFFER_SIZE);
			sprintf(rgcRDData,"%f", stConfigData.fConfig_HysSprayPresPos);
			DEBUG_PRINTF("SCU_SPRAY_PRESSURE_HYST_POS = %s\r\n", rgcRDData);	
		#endif
		
		SplitByte.uliData = 0;
		fnSPI_EEPROM_ReadData(SCU_SPRAY_PRESSURE_HYST_NEG, 4, SplitByte.rgucData);
		stConfigData.fConfig_HysSprayPresNeg = SplitByte.fData;
		#ifdef GENERAL_DBG_EE
			memset(rgcRDData, 0, READ_BUFFER_SIZE);
			sprintf(rgcRDData,"%f", stConfigData.fConfig_HysSprayPresNeg);
			DEBUG_PRINTF("SCU_SPRAY_PRESSURE_HYST_NEG = %s\r\n", rgcRDData);	
		#endif
		
		SplitByte.uliData = 0;
		fnSPI_EEPROM_ReadData(SCU_RECIRC_PRESSURE_VALUE, 4, SplitByte.rgucData);
		stConfigData.fConfig_RecirPressure = SplitByte.fData;
		#ifdef GENERAL_DBG_EE
			memset(rgcRDData, 0, READ_BUFFER_SIZE);
			sprintf(rgcRDData,"%f", stConfigData.fConfig_RecirPressure);
			DEBUG_PRINTF("SCU_RECIRC_PRESSURE_VALUE = %s\r\n", rgcRDData);	
		#endif


		SplitByte.uliData = 0;
		fnSPI_EEPROM_ReadData(SCU_RECIRC_PRESSURE_THRESH_LOW, 4, SplitByte.rgucData);
		stConfigData.fConfig_RecirPressureLTH = SplitByte.fData;
		#ifdef GENERAL_DBG_EE
			memset(rgcRDData, 0, READ_BUFFER_SIZE);
			sprintf(rgcRDData,"%f", stConfigData.fConfig_RecirPressureLTH);
			DEBUG_PRINTF("SCU_RECIRC_PRESSURE_THRESH_LOW = %s\r\n", rgcRDData);	
		#endif
		
		SplitByte.uliData = 0;
		fnSPI_EEPROM_ReadData(SCU_RECIRC_PRESSURE_THRESH_HIGH, 4, SplitByte.rgucData);
		stConfigData.fConfig_RecirPressureHTH = SplitByte.fData;
		#ifdef GENERAL_DBG_EE
			memset(rgcRDData, 0, READ_BUFFER_SIZE);
			sprintf(rgcRDData,"%f", stConfigData.fConfig_RecirPressureHTH);
			DEBUG_PRINTF("SCU_RECIRC_PRESSURE_THRESH_HIGH = %s\r\n", rgcRDData);	
		#endif

SplitByte.uliData = 0;
		fnSPI_EEPROM_ReadData(SCU_RECIRC_PRESSURE_HYST_POS, 4, SplitByte.rgucData);
		stConfigData.fConfig_HysRecircPresPos = SplitByte.fData;
		#ifdef GENERAL_DBG_EE
			memset(rgcRDData, 0, READ_BUFFER_SIZE);
			sprintf(rgcRDData,"%f", stConfigData.fConfig_HysRecircPresPos);
			DEBUG_PRINTF("SCU_RECIRC_PRESSURE_HYST_POS = %s\r\n", rgcRDData);	
		#endif
		
		SplitByte.uliData = 0;
		fnSPI_EEPROM_ReadData(SCU_RECIRC_PRESSURE_HYST_NEG, 4, SplitByte.rgucData);
		stConfigData.fConfig_HysRecircPresNeg = SplitByte.fData;
		#ifdef GENERAL_DBG_EE
			memset(rgcRDData, 0, READ_BUFFER_SIZE);
			sprintf(rgcRDData,"%f", stConfigData.fConfig_HysRecircPresNeg);
			DEBUG_PRINTF("SCU_RECIRC_PRESSURE_HYST_NEG = %s\r\n", rgcRDData);	
		#endif


		SplitByte.uliData = 0;
		fnSPI_EEPROM_ReadData(COUNT_JOINT, 4, SplitByte.rgucData);
		stConfigData.fConfig_JointNum = SplitByte.fData;
		#ifdef GENERAL_DBG_EE
			memset(rgcRDData, 0, READ_BUFFER_SIZE);
			sprintf(rgcRDData,"%f", stConfigData.fConfig_JointNum);
			DEBUG_PRINTF("COUNT_JOINT = %s\r\n", rgcRDData);	
		#endif


		SplitByte.uliData = 0;
		fnSPI_EEPROM_ReadData(TIME_DATA_LOGGING, 4, SplitByte.rgucData);
		stConfigData.fConfig_DataLoggingPeriod = SplitByte.fData;
		#ifdef GENERAL_DBG_EE
			memset(rgcRDData, 0, READ_BUFFER_SIZE);
			sprintf(rgcRDData,"%f", stConfigData.fConfig_DataLoggingPeriod);
			DEBUG_PRINTF("TIME_DATA_LOGGING = %s\r\n", rgcRDData);	
		#endif

		fnSPI_EEPROM_ReadData(SYS_TIME, 7, stConfigData.ucConfig_SystemTime);
		#ifdef GENERAL_DBG_EE
			DEBUG_PRINTF("SYS_TIME = %s\r\n", stConfigData.ucConfig_SystemTime);
		#endif	

/******************************************** SUPERVISOR ***************************************/
		fnSPI_EEPROM_ReadData(SUPERVISOR_NAME1, 7, stConfigData.rgucConfig_SupervisorName1);
		stConfigData.rgucConfig_SupervisorName1[7] =  NULL;
		#ifdef GENERAL_DBG_EE
			DEBUG_PRINTF("SUPERVISOR_NAME1 = %s\r\n", stConfigData.rgucConfig_SupervisorName1);
		#endif

		fnSPI_EEPROM_ReadData(SUPERVISOR_NAME2, 7, stConfigData.rgucConfig_SupervisorName2);
		stConfigData.rgucConfig_SupervisorName2[7] =  NULL;
		#ifdef GENERAL_DBG_EE
			EBUG_PRINTF("SUPERVISOR_NAME2 = %s\r\n", stConfigData.rgucConfig_SupervisorName2);
		#endif

		fnSPI_EEPROM_ReadData(SUPERVISOR_NAME3, 7,stConfigData.rgucConfig_SupervisorName3);
		stConfigData.rgucConfig_SupervisorName3[7] =  NULL;
		#ifdef GENERAL_DBG_EE
			DEBUG_PRINTF("SUPERVISOR_NAME3 = %s\r\n", stConfigData.rgucConfig_SupervisorName3);
		#endif

/********************************************** COATING*************************************/
		fnSPI_EEPROM_ReadData(COATING_MAT_NAME1, 7, stConfigData.rgucConfig_CoatingMatName1);
		stConfigData.rgucConfig_CoatingMatName1[7] =  NULL;
		#ifdef GENERAL_DBG_EE
			DEBUG_PRINTF("COATING_MAT_NAME1 = %s\r\n", stConfigData.rgucConfig_CoatingMatName1);
		#endif

		fnSPI_EEPROM_ReadData(COATING_MAT_NAME2, 7, stConfigData.rgucConfig_CoatingMatName2);
		stConfigData.rgucConfig_CoatingMatName2[7] =  NULL;
		#ifdef GENERAL_DBG_EE
			DEBUG_PRINTF("COATING_MAT_NAME2 = %s\r\n", stConfigData.rgucConfig_CoatingMatName2);
		#endif

		fnSPI_EEPROM_ReadData(COATING_MAT_NAME3, 7, stConfigData.rgucConfig_CoatingMatName3);
		stConfigData.rgucConfig_CoatingMatName3[7] =  NULL;
		#ifdef GENERAL_DBG_EE
			DEBUG_PRINTF("COATING_MAT_NAME3 = %s\r\n", stConfigData.rgucConfig_CoatingMatName3);
		#endif


/********************************************BASE*********************************************/
		fnSPI_EEPROM_ReadData(BASE_MAT_BATCHNO1, 7, stConfigData.rgucConfig_BaseMatBatchNo1);
		stConfigData.rgucConfig_BaseMatBatchNo1[7] =  NULL;
		#ifdef GENERAL_DBG_EE
			DEBUG_PRINTF("BASE_MAT_BATCHNO1 = %s\r\n", stConfigData.rgucConfig_BaseMatBatchNo1);
		#endif

		fnSPI_EEPROM_ReadData(BASE_MAT_BATCHNO2, 7, stConfigData.rgucConfig_BaseMatBatchNo2);
		stConfigData.rgucConfig_BaseMatBatchNo2[7] =  NULL;
		#ifdef GENERAL_DBG_EE
			DEBUG_PRINTF("BASE_MAT_BATCHNO2 = %s\r\n", stConfigData.rgucConfig_BaseMatBatchNo2);
		#endif

		fnSPI_EEPROM_ReadData(BASE_MAT_BATCHNO3, 7, stConfigData.rgucConfig_BaseMatBatchNo3);
		stConfigData.rgucConfig_BaseMatBatchNo3[7] =  NULL;
		#ifdef GENERAL_DBG_EE
			DEBUG_PRINTF("BASE_MAT_BATCHNO3 = %s\r\n", stConfigData.rgucConfig_BaseMatBatchNo3);
		#endif

/***************************************HARDNER**********************************************/
		fnSPI_EEPROM_ReadData(HARD_MAT_BATCHNO1, 7, stConfigData.rgucConfig_HardMatBatchNo1);
		stConfigData.rgucConfig_HardMatBatchNo1[7] =  NULL;
		#ifdef GENERAL_DBG_EE
			DEBUG_PRINTF("HARD_MAT_BATCHNO1 = %s\r\n", stConfigData.rgucConfig_HardMatBatchNo1);
		#endif
		
		fnSPI_EEPROM_ReadData(HARD_MAT_BATCHNO2, 7, stConfigData.rgucConfig_HardMatBatchNo2);
		stConfigData.rgucConfig_HardMatBatchNo2[7] =  NULL;
		#ifdef GENERAL_DBG_EE
			DEBUG_PRINTF("HARD_MAT_BATCHNO2 = %s\r\n", stConfigData.rgucConfig_HardMatBatchNo2);
		#endif

		fnSPI_EEPROM_ReadData(HARD_MAT_BATCHNO3, 7, stConfigData.rgucConfig_HardMatBatchNo3);
		stConfigData.rgucConfig_HardMatBatchNo3[7] =  NULL;		
		#ifdef GENERAL_DBG_EE
			DEBUG_PRINTF("HARD_MAT_BATCHNO3 = %s\r\n", stConfigData.rgucConfig_HardMatBatchNo3);
		#endif

		fnSPI_EEPROM_ReadData(SCU_UNIT_TYPE, 1, &stConfigData.ucConfig_TypeofUnit);
		#ifdef GENERAL_DBG_EE
			DEBUG_PRINTF("SCU_UNIT_TYPE = %d\r\n", stConfigData.ucConfig_TypeofUnit);
		#endif	


		fnSPI_EEPROM_ReadData(SCU_BYPASS, 1, &stConfigData.ucConfig_SCUBypass);
		#ifdef GENERAL_DBG_EE
			DEBUG_PRINTF("SCU_BYPASS = %d\r\n", stConfigData.ucConfig_SCUBypass);
		#endif	


		SplitByte.uliData = 0;
		fnSPI_EEPROM_ReadData(OPMM_PIPE_TEMP_THRES_LOW, 4, SplitByte.rgucData);
		stConfigData.fConfig_PipeTempLTH = SplitByte.fData;
		#ifdef GENERAL_DBG_EE
			memset(rgcRDData, 0, READ_BUFFER_SIZE);
			sprintf(rgcRDData,"%f", stConfigData.fConfig_PipeTempLTH);
			DEBUG_PRINTF("OPMM_PIPE_TEMP_THRES_LOW = %s\r\n", rgcRDData);	
		#endif					
		
		SplitByte.uliData = 0;
		fnSPI_EEPROM_ReadData(OPMM_PIPE_TEMP_THRES_HIGH, 4, SplitByte.rgucData);
		stConfigData.fConfig_PipeTempHTH = SplitByte.fData;
		#ifdef GENERAL_DBG_EE
			memset(rgcRDData, 0, READ_BUFFER_SIZE);
			sprintf(rgcRDData,"%f", stConfigData.fConfig_PipeTempHTH);
			DEBUG_PRINTF("OPMM_PIPE_TEMP_THRES_HIGH = %s\r\n", rgcRDData);	
		#endif	

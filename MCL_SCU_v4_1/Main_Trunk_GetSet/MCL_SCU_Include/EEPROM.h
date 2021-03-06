/******************************************************************************
**@file     :   EEPROM.h
* 
* @brief  	:		This File Contains All The MACROS Of EEPROM and
*				  			flags and extern functions, PAGE size is 16 Byte. 
******************************************************************************/

/* ----------------------------------------------------------------------------
 *          MACROS FOR THE EEPROM PAGE ADDRESSES OF ALL THE MODULES
 * ----------------------------------------------------------------------------
 */
 
#define EEPROM_SPI_CS 														GPIO_Pin_6
#define EEPROM_SPI_SCK														GPIO_Pin_7
#define EEPROM_SPI_MISO 													GPIO_Pin_8
#define EEPROM_SPI_MOSI 													GPIO_Pin_9
#define EEPROM_GPIO_PORT													GPIOF
 
#define SIGNATURE_VALUE 											(uint16_t)0xABAB

#define SIGNATURE_START 											(uint16_t)0x0000

#define PAGE0_START 													SIGNATURE_START
#define SIGNATURE_BACKUP												SIGNATURE_START+2
#define PAGE1_START 													(uint16_t)0x0010
#define PAGE2_START 													(uint16_t)0x0020
#define PAGE3_START 													(uint16_t)0x0030
#define PAGE4_START 													(uint16_t)0x0040
#define PAGE5_START 													(uint16_t)0x0050
#define PAGE6_START 													(uint16_t)0x0060
#define PAGE7_START 													(uint16_t)0x0070
#define PAGE8_START 													(uint16_t)0x0080
#define PAGE9_START 													(uint16_t)0x0090
#define PAGE10_START 													(uint16_t)0x00A0
#define PAGE11_START 													(uint16_t)0x00B0
#define PAGE12_START 													(uint16_t)0x00C0
#define PAGE13_START 													(uint16_t)0x00D0
#define PAGE14_START 													(uint16_t)0x00E0
#define PAGE15_START 													(uint16_t)0x00F0
#define PAGE16_START													(uint16_t)0x0100
#define PAGE17_START													(uint16_t)0x0110
#define PAGE18_START													(uint16_t)0x0120
#define PAGE19_START													(uint16_t)0x0130
#define PAGE20_START													(uint16_t)0x0140
#define PAGE21_START													(uint16_t)0x0150
#define PAGE22_START													(uint16_t)0x0160
#define PAGE23_START													(uint16_t)0x0170
#define PAGE24_START													(uint16_t)0x0180
#define PAGE25_START													(uint16_t)0x0190
#define PAGE26_START													(uint16_t)0x01A0
#define PAGE27_START													(uint16_t)0x01B0
#define PAGE28_START													(uint16_t)0x01C0
#define PAGE29_START													(uint16_t)0x01D0
#define PAGE30_START													(uint16_t)0x01E0
#define PAGE31_START													(uint16_t)0x01F0
#define PAGE32_START													(uint16_t)0x0200
#define PAGE33_START													(uint16_t)0x0210
#define PAGE34_START													(uint16_t)0x0220
#define PAGE35_START													(uint16_t)0x0230
#define PAGE36_START													(uint16_t)0x0240
#define PAGE37_START													(uint16_t)0x0250
#define PAGE38_START													(uint16_t)0x0260
#define PAGE39_START													(uint16_t)0x0270
#define PAGE40_START													(uint16_t)0x0280
#define PAGE41_START													(uint16_t)0x0290
#define PAGE42_START													(uint16_t)0x02A0
#define PAGE43_START													(uint16_t)0x02B0
#define PAGE44_START													(uint16_t)0x02C0
#define PAGE45_START													(uint16_t)0x02D0
#define PAGE46_START													(uint16_t)0x02E0
#define PAGE47_START													(uint16_t)0x02F0
#define PAGE48_START													(uint16_t)0x0300
#define PAGE49_START													(uint16_t)0x0310
#define PAGE50_START													(uint16_t)0x0320
#define PAGE51_START													(uint16_t)0x0330
#define PAGE52_START													(uint16_t)0x0340
#define PAGE53_START													(uint16_t)0x0350
#define PAGE54_START													(uint16_t)0x0360
#define PAGE55_START													(uint16_t)0x0370
#define PAGE56_START													(uint16_t)0x0380
#define PAGE57_START													(uint16_t)0x0390
#define PAGE58_START													(uint16_t)0x03A0
#define PAGE59_START													(uint16_t)0x03B0
#define PAGE60_START													(uint16_t)0x03C0
#define PAGE61_START													(uint16_t)0x03D0
#define PAGE62_START													(uint16_t)0x03E0
#define PAGE63_START													(uint16_t)0x03F0
#define PAGE64_START													(uint16_t)0x0400


/* OTHER MACROS ----------------------------------------------------------*/

#define BIT_MASK															(uint8_t)0x01
#define BIT_COMPARE														(uint8_t)0x01

#define DUMMY_DATA														(uint8_t)0xAA
#define WREN              										(uint8_t)0x06/*Write enble*/
#define WRDI 			  													(uint8_t)0x04/*Write disable*/
#define RDSR  	          										(uint8_t)0x05/*Read sts regi*/
#define WRSR  			  												(uint8_t)0x01/*Write sts regi*/
#define READ			  													(uint8_t)0x03/*Read Data Bytes*/
#define WRITE             										(uint8_t)0x02/*Write Data Bytes*/


/*************************************VCM**************************************/

#define VCM_STARTADDRESS										PAGE1_START 
 
#define VCM_ADD1	 													VCM_STARTADDRESS  
#define VCM_ADD2														PAGE2_START
#define VCM_ADD3														PAGE3_START
#define VCM_ADD4														PAGE4_START

#define VCM_UPSTROKE_ADD										VCM_ADD1 
#define VCM_DOWNSTROKE_ADD									VCM_UPSTROKE_ADD 					+ 2 	
#define VCM_PRESSURE_LOWTH1									VCM_DOWNSTROKE_ADD 				+ 2
#define VCM_PRESSURE_HIGHTH1								VCM_PRESSURE_LOWTH1 			+ 4
#define VCM_PRESSURE_LOWTH2									VCM_PRESSURE_HIGHTH1 			+ 4

#define VCM_PRESSURE_HIGHTH2								VCM_ADD2
#define VCM_PRESSURE_LOWTH3									VCM_PRESSURE_HIGHTH2 			+ 4
#define VCM_PRESSURE_HIGHTH3								VCM_PRESSURE_LOWTH3 			+ 4
#define VCM_CYLINDER_AREA1									VCM_PRESSURE_HIGHTH3 			+ 4
#define VCM_CYLINDER_AREA2									VCM_ADD3 
#define VCM_CYLINDER_AREA3									VCM_CYLINDER_AREA2 				+ 4
#define VCM_CYLINDER_COUNT									VCM_CYLINDER_AREA3 				+ 4
#define VCM_PRESSURE_GAUGE1_CONN						VCM_CYLINDER_COUNT				+ 1
#define VCM_PRESSURE_GAUGE2_CONN						VCM_PRESSURE_GAUGE1_CONN	+ 1
#define VCM_PRESSURE_GAUGE3_CONN						VCM_PRESSURE_GAUGE2_CONN	+ 1

#define VCM_LINEAR_TRANSDUCER_CONN					VCM_ADD4	
#define VCM_EE07_INSIDE_CONN								VCM_LINEAR_TRANSDUCER_CONN+1
#define VCM_EE07_OUTSIDE_CONN								VCM_EE07_INSIDE_CONN	+ 1
#define VCM_STALL_VALVE_CONN								VCM_EE07_OUTSIDE_CONN	+ 1
#define VCM_BYPASS													VCM_STALL_VALVE_CONN	+ 1
#define VCM_BEFORE_STALL										VCM_BYPASS	+ 1
#define VCM_COMPARE_STALL										VCM_BEFORE_STALL	+ 4
/*change53*/
#define VCM_RECIRC_VALVE_CONN								VCM_COMPARE_STALL	+ 4



/************************ HCM ****************************-*/

#define HCM_STARTADDRESS 										PAGE5_START

#define HCM_ADD1														HCM_STARTADDRESS
#define HCM_ADD2														PAGE6_START
#define HCM_ADD3														PAGE7_START
#define HCM_ADD4														PAGE8_START
#define HCM_ADD5														PAGE9_START
#define HCM_ADD6														PAGE10_START
#define HCM_ADD7														PAGE11_START


#define HCM_ANTI_SET_TEMP										HCM_ADD1
#define HCM_ANTI_THRESH_TEMP_LOW						HCM_ANTI_SET_TEMP + 4	
#define HCM_ANTI_THRESH_TEMP_HIGH						HCM_ANTI_THRESH_TEMP_LOW +4
#define HCM_ANTI_HYST_TEMP_POS							HCM_ANTI_THRESH_TEMP_HIGH +4

#define HCM_ANTI_HYST_TEMP_NEG							HCM_ADD2
#define HCM_INLINE_BASE_SET_TEMP						HCM_ANTI_HYST_TEMP_NEG +4
#define HCM_INLINE_BASE_THRESH_TEMP_LOW			HCM_INLINE_BASE_SET_TEMP +4				
#define HCM_INLINE_BASE_THRESH_TEMP_HIGH		HCM_INLINE_BASE_THRESH_TEMP_LOW +4

#define HCM_INLINE_BASE_HYST_TEMP_POS				HCM_ADD3
#define HCM_INLINE_BASE_HYST_TEMP_NEG				HCM_INLINE_BASE_HYST_TEMP_POS + 4
#define HCM_INLINE_HARD_SET_TEMP						HCM_INLINE_BASE_HYST_TEMP_NEG  + 4
#define HCM_INLINE_HARD_THRESH_TEMP_LOW			HCM_INLINE_HARD_SET_TEMP+ 4	

#define HCM_INLINE_HARD_THRESH_TEMP_HIGH		HCM_ADD4
#define HCM_INLINE_HARD_HYST_TEMP_POS				HCM_INLINE_HARD_THRESH_TEMP_HIGH +4
#define HCM_INLINE_HARD_HYST_TEMP_NEG				HCM_INLINE_HARD_HYST_TEMP_POS +4
#define HCM_BASE_LIQUID_THRESH_LEVEL_LOW		HCM_INLINE_HARD_HYST_TEMP_NEG + 4		

#define HCM_BASE_LIQUID_THRESH_LEVEL_HIGH		HCM_ADD5		
#define HCM_HARD_LIQUID_THRESH_LEVEL_LOW		HCM_BASE_LIQUID_THRESH_LEVEL_HIGH +4		
#define HCM_HARD_LIQUID_THRESH_LEVEL_HIGH		HCM_HARD_LIQUID_THRESH_LEVEL_LOW + 4		
#define HCM_BASE_TANK_HEIGHT								HCM_HARD_LIQUID_THRESH_LEVEL_HIGH +4

#define HCM_HARD_TANK_HEIGHT								HCM_ADD6
#define HCM_LIQUID_LEVEL_SAMPLING_RATE			HCM_HARD_TANK_HEIGHT + 4
#define HCM_INLINE_BASE_THERM_CONN					HCM_LIQUID_LEVEL_SAMPLING_RATE + 2
#define HCM_INLINE_HARD_THERM_CONN					HCM_INLINE_BASE_THERM_CONN +1
#define HCM_INLINE_ANTIFREEZE_THERM_CONN		HCM_INLINE_HARD_THERM_CONN +1
#define HCM_RETURN_BASE_THERM_CONN					HCM_INLINE_ANTIFREEZE_THERM_CONN +1
#define HCM_INLINE_BASE_HEATER_CONN					HCM_RETURN_BASE_THERM_CONN +1
#define HCM_INLINE_HARD_HEATER_CONN					HCM_INLINE_BASE_HEATER_CONN +1
#define HCM_INLINE_ANTIFREEZE_HEATER_CONN		HCM_INLINE_HARD_HEATER_CONN	+1
#define HCM_HARD_HEATER_CONN								HCM_INLINE_ANTIFREEZE_HEATER_CONN+1

#define HCM_BASE_LIQUID_LEVEL_CONN					HCM_ADD7
#define HCM_HARD_LIQUID_LEVEL_CONN					HCM_BASE_LIQUID_LEVEL_CONN+1
#define HCM_BYPASS													HCM_HARD_LIQUID_LEVEL_CONN +1

/************************ OPMM ************************/

#define OPMM_STARTADDRESS										PAGE12_START

#define OPMM_ADD1														OPMM_STARTADDRESS
#define OPMM_ADD2														PAGE13_START
#define OPMM_ADD3														PAGE14_START
#define OPMM_ADD4														PAGE15_START


						
#define OPMM_SPRAY_COUNT												OPMM_ADD1
#define OPMM_PRIME_TIME										  		OPMM_SPRAY_COUNT+1
#define OPMM_GUN_DELAY_TIME											OPMM_PRIME_TIME+4			
#define OPMM_SPRAY_OVERLAPTIME									OPMM_ADD2
#define OPMM_REVERSE_DELAYTIME									OPMM_SPRAY_OVERLAPTIME+4
#define OPMM_FLUSH_TIME													OPMM_REVERSE_DELAYTIME+4
#define OPMM_DCLS_HOME_REACH_TIME								OPMM_FLUSH_TIME+4
#define OPMM_HOME_DCLS_REACH_TIME								OPMM_ADD3
#define OPMM_GUN_POSITION_CHANGE_TIME						OPMM_HOME_DCLS_REACH_TIME+4
#define OPMM_DELAY_SPRAY_VALVE_FLUSH_VBAT_TIME	OPMM_GUN_POSITION_CHANGE_TIME+4	  
#define OPMM_FLUSH_WARNNING_TIME								OPMM_DELAY_SPRAY_VALVE_FLUSH_VBAT_TIME+4
#define OPMM_SPRAY_VALVE_CONN										OPMM_ADD4
#define OPMM_GUNPOS_VALVE_CONN									OPMM_SPRAY_VALVE_CONN+1
#define OPMM_AB_VALVE_CONN											OPMM_GUNPOS_VALVE_CONN+1
#define OPMM_SOLVENT_VALVE_CONN									OPMM_AB_VALVE_CONN+1
#define OPMM_FORWARD_VALVE_CONN									OPMM_SOLVENT_VALVE_CONN+1
#define OPMM_BACKWARD_VALVE_CONN								OPMM_FORWARD_VALVE_CONN+1
#define OPMM_BYPASS															OPMM_BACKWARD_VALVE_CONN+1
#define OPMM_NON_CONTACT12_CONN									OPMM_BYPASS+2
#define OPMM_NON_CONTACT3_CONN									OPMM_NON_CONTACT12_CONN+1
#define OPMM_NON_CONTACT6_CONN									OPMM_NON_CONTACT3_CONN+1
#define OPMM_NON_CONTACT9_CONN									OPMM_NON_CONTACT6_CONN+1
/************************ SCU ************************/

#define SCU_STARTADDRESS										PAGE16_START
#define SCU_ADD1														SCU_STARTADDRESS
#define SCU_ADD2														PAGE17_START
#define SCU_ADD3														PAGE18_START
#define SCU_ADD4														PAGE19_START
#define SCU_ADD5														PAGE20_START
#define SCU_ADD6														PAGE21_START
#define SCU_ADD7														PAGE22_START
#define SCU_ADD8														PAGE23_START
#define SCU_ADD9														PAGE24_START
#define SCU_ADD10														PAGE25_START
#define SCU_ADD11														PAGE26_START
#define SCU_ADD12														PAGE27_START
#define SCU_ADD13														PAGE28_START
#define SCU_ADD14														PAGE29_START




#define SCU_SPRAY_PRESSURE_VALUE								SCU_ADD1
#define SCU_SPRAY_PRESSURE_THRESH_LOW						SCU_SPRAY_PRESSURE_VALUE+4
#define SCU_SPRAY_PRESSURE_THRESH_HIGH					SCU_SPRAY_PRESSURE_THRESH_LOW+4
#define SCU_SPRAY_PRESSURE_HYST_POS							SCU_SPRAY_PRESSURE_THRESH_HIGH+4

#define SCU_SPRAY_PRESSURE_HYST_NEG							SCU_ADD2
#define SCU_RECIRC_PRESSURE_VALUE								SCU_SPRAY_PRESSURE_HYST_NEG+4
#define SCU_RECIRC_PRESSURE_THRESH_LOW					SCU_RECIRC_PRESSURE_VALUE+4
#define SCU_RECIRC_PRESSURE_THRESH_HIGH					SCU_RECIRC_PRESSURE_THRESH_LOW+4

#define SCU_RECIRC_PRESSURE_HYST_POS						SCU_ADD3
#define SCU_RECIRC_PRESSURE_HYST_NEG						SCU_RECIRC_PRESSURE_HYST_POS+4

#define COUNT_JOINT															SCU_RECIRC_PRESSURE_HYST_NEG+4	
#define TIME_DATA_LOGGING												COUNT_JOINT+4

#define SYS_TIME																SCU_ADD4
/************************* Job Settings ************************/
#define SUPERVISOR_NAME1										SCU_ADD5 
#define SUPERVISOR_NAME2										SCU_ADD5 + 7
#define SUPERVISOR_NAME3										SCU_ADD6 
#define COATING_MAT_NAME1										SCU_ADD6 + 7
#define COATING_MAT_NAME2										SCU_ADD7 
#define COATING_MAT_NAME3										SCU_ADD7 + 7
#define BASE_MAT_BATCHNO1										SCU_ADD8 
#define BASE_MAT_BATCHNO2										SCU_ADD8 + 7
#define BASE_MAT_BATCHNO3										SCU_ADD9 
#define HARD_MAT_BATCHNO1										SCU_ADD9 + 7
#define HARD_MAT_BATCHNO2										SCU_ADD10 
#define HARD_MAT_BATCHNO3										SCU_ADD10 + 7
#define SCU_UNIT_TYPE												SCU_ADD11 
#define SCU_BYPASS													SCU_UNIT_TYPE+1
#define TEMP_PRESS_ACHIEVE_BYPASS						SCU_BYPASS+1
#define OPMM_PIPE_TEMP_THRES_LOW						SCU_ADD14			
#define OPMM_PIPE_TEMP_THRES_HIGH						OPMM_PIPE_TEMP_THRES_LOW + 4
#define OPMM_BUZZER_VOLUME									OPMM_PIPE_TEMP_THRES_HIGH + 4

/*************************************ETHERNET**************************************/
#define SCU_ETH_IPADDRESS										PAGE30_START 
#define SCU_ETH_SMADDRESS										PAGE31_START 
#define SCU_ETH_GWADDRESS										PAGE32_START 
#define SCU_ETH_PORTADDRESS									PAGE33_START 
/**********************************************************************************/
/***********************************change8****************************************/
#define SCU_TIME_SEND_STALL_COMMAND					PAGE34_START
#define	OPMM_STOP_HLS_TIMER									SCU_TIME_SEND_STALL_COMMAND+4
#define	OPMM_BATT_KNEE_VOLT									OPMM_STOP_HLS_TIMER+4
/**********************************************************************************/
/***********************************change10****************************************/
#define VCM_STALL_UPSTROKE_UPPER_LIMIT				PAGE35_START
#define VCM_STALL_UPSTROKE_MIN_RANGE					VCM_STALL_UPSTROKE_UPPER_LIMIT+1
#define VCM_STALL_UPSTROKE_MAX_RANGE					VCM_STALL_UPSTROKE_MIN_RANGE+1
#define VCM_STALL_DOWNSTROKE_UPPER_LIMIT			VCM_STALL_UPSTROKE_MAX_RANGE+1
#define VCM_STALL_DOWNSTROKE_MIN_RANGE				VCM_STALL_DOWNSTROKE_UPPER_LIMIT+1
#define VCM_STALL_DOWNSTROKE_MAX_RANGE				VCM_STALL_DOWNSTROKE_MIN_RANGE+1

/*************************************WIFI change18**************************************/
#define SCU_WIFI_IPADDRESS										PAGE36_START 
#define SCU_WIFI_SMADDRESS										PAGE37_START 
#define SCU_WIFI_GWADDRESS										PAGE38_START 
#define SCU_WIFI_PORTADDRESS									PAGE39_START 
#define SCU_WIFI_SSIDADDRESS									PAGE40_START 
#define SCU_WIFI_PASSADDRESS									PAGE41_START 

/*******************************change21***************************************************/
#define JOB_NAME1										PAGE42_START 
#define JOB_NAME2										PAGE42_START + 7
#define JOB_NAME3										PAGE43_START 
#define IDLE_DATA_LOGGING						PAGE43_START+7
#define PIPE_DIAMETER   						IDLE_DATA_LOGGING+4

/***********************************change23****************************************/
#define CURRENT_INDEX_SUPERVISOR_NAME				PAGE44_START  //0
#define CURRENT_INDEX_JOB_NAME							CURRENT_INDEX_SUPERVISOR_NAME+1  //1
/***********************************change37****************************************/
#define HMI_TIME_OFFSET_SIGN								CURRENT_INDEX_JOB_NAME+1  //2
#define HMI_TIME_OFFSET_HOUR								HMI_TIME_OFFSET_SIGN+1		//3
#define HMI_TIME_OFFSET_MIN									HMI_TIME_OFFSET_HOUR+1		//4
#define HMI_TIME_OFFSET_SEC									HMI_TIME_OFFSET_MIN+1			//5
/*change43*/
#define BASE_TOTAL_MAT_USED									HMI_TIME_OFFSET_SEC+1			//6
#define HARD_TOTAL_MAT_USED									BASE_TOTAL_MAT_USED+4			//A
#define CURRENT_INDEX_JOB_ID								HARD_TOTAL_MAT_USED+4			//e
/************************* Job Id change42 ************************/
#define JOB_ID1															PAGE45_START    //0
#define JOB_ID2															PAGE45_START + 7		//7
#define JOB_ID3															PAGE46_START 		//0
/*change43*/
#define BASE_PRIME_MAT_ACC									PAGE46_START+7  //7
#define HARD_PRIME_MAT_ACC									BASE_PRIME_MAT_ACC+4//
#define BASE_APPLIED_MAT_ACC								PAGE47_START  //0
#define HARD_APPLIED_MAT_ACC								BASE_APPLIED_MAT_ACC+4 //4
#define BASE_TOTAL_MAT_USED_ACC							HARD_APPLIED_MAT_ACC+4 //8
#define HARD_TOTAL_MAT_USED_ACC							BASE_TOTAL_MAT_USED_ACC+4//12
/*change78*/
#define LOG_INDEX_JOB_NAME								PAGE48_START  //0
#define LOG_INDEX_JOB_ID									LOG_INDEX_JOB_NAME + 1  //0
#define LOG_INDEX_SUPERVISOR_NAME					LOG_INDEX_JOB_ID + 1  //0

#define SIGNATUREVALUE_SIZE							2
#define READ_BUFFER_SIZE								150
#define WRITE_BUFFER_SIZE								10


/***************************VCM*********************************/
/* VCM_UPSTROKE   CONFIG_VCM_STALL_TH */
#define CONFIG_PARA_1													(uint8_t)1  	
/* VCM_DOWNSTROKE_ADD  CONFIG_VCM_STALL_TH */
#define CONFIG_PARA_2													(uint8_t)2  	
/* VCM_PRESSURE_LOWTH1 CONFIG_VCM_PGAUGE1_TH*/
#define CONFIG_PARA_3													(uint8_t)3		
/* VCM_PRESSURE_HIGHTH1 CONFIG_VCM_PGAUGE1_TH*/
#define CONFIG_PARA_4													(uint8_t)4		
/* VCM_PRESSURE_LOWTH2 CONFIG_VCM_PGAUGE2_TH*/
#define CONFIG_PARA_5													(uint8_t)5		
/* VCM_PRESSURE_HIGHTH2 CONFIG_VCM_PGAUGE2_TH*/
#define CONFIG_PARA_6													(uint8_t)6		
/* VCM_PRESSURE_LOWTH3 CONFIG_VCM_PGAUGE3_TH*/
#define CONFIG_PARA_7													(uint8_t)7		
/* VCM_PRESSURE_HIGHTH3 CONFIG_VCM_PGAUGE3_TH*/
#define CONFIG_PARA_8													(uint8_t)8		
/* VCM_CYLINDER_AREA1 CONFIG_CYLINDER_SURFACE_AREA*/
#define CONFIG_PARA_9													(uint8_t)9		
/* VCM_CYLINDER_AREA2 CONFIG_CYLINDER_SURFACE_AREA*/
#define CONFIG_PARA_10												(uint8_t)10		 
/* VCM_CYLINDER_AREA3 CONFIG_CYLINDER_SURFACE_AREA*/
#define CONFIG_PARA_11												(uint8_t)11		
/* VCM_CYLINDER_COUNT CONFIG_NUMBER_OF_CYLINDER*/
#define CONFIG_PARA_12												(uint8_t)12		
/* VCM_PRESSURE_GAUGE1_CONN,VCM_PRESSURE_GAUGE2_CONN,VCM_PRESSURE_GAUGE3_CONN
   CONFIG_PGAUGE_REMAP*/
#define CONFIG_PARA_13												(uint8_t)13		
/* VCM_LINEAR_TRANSDUCER_CONN CONFIG_LT_REMAP*/
#define CONFIG_PARA_14												(uint8_t)14		
/* VCM_EE07_INSIDE_CONN,VCM_EE07_OUTSIDE_CONN CONFIG_EE07_REMAP*/
#define CONFIG_PARA_15												(uint8_t)15		
/* VCM_STALL_VALVE_CONN CONFIG_STALL_VALVE_REMAP */
#define CONFIG_PARA_16												(uint8_t)16		
/* VCM_BYPASS CONFIG_VCM_PERIPHRAL_BYPASS*/
#define CONFIG_PARA_17												(uint8_t)17		
/* VCM_BEFORE_STALL CONFIG_TIME_BEFORE_STALL */
#define CONFIG_PARA_18												(uint8_t)18		
/* VCM_COMPARE_STALL CONFIG_TIME_BEFORE_COMPARE_STALL */
#define CONFIG_PARA_19												(uint8_t)19		
/***************************HCM*********************************/
/* HCM_ANTI_SET_TEMP CONFIG_BASE_TEMP_SETTING  */
#define CONFIG_PARA_20												(uint8_t)20		
/* HCM_ANTI_THRESH_TEMP_LOW CONFIG_BASE_TEMP_SETTING */
#define CONFIG_PARA_21												(uint8_t)21		
/* HCM_ANTI_THRESH_TEMP_HIGH  CONFIG_BASE_TEMP_SETTING */
#define CONFIG_PARA_22												(uint8_t)22		
/* HCM_ANTI_HYST_TEMP_POS CONFIG_BASE_TEMP_SETTING */
#define CONFIG_PARA_23												(uint8_t)23		
/* HCM_ANTI_HYST_TEMP_NEG CONFIG_BASE_TEMP_SETTING*/
#define CONFIG_PARA_24												(uint8_t)24		
/* HCM_INLINE_BASE_SET_TEMP CONFIG_INLINE_BASE_TEMP_SETTING*/
#define CONFIG_PARA_25												(uint8_t)25		
/* HCM_INLINE_BASE_THRESH_TEMP_LOW CONFIG_INLINE_BASE_TEMP_SETTING*/
#define CONFIG_PARA_26												(uint8_t)26		
/* HCM_INLINE_BASE_THRESH_TEMP_HIGH CONFIG_INLINE_BASE_TEMP_SETTING */
#define CONFIG_PARA_27												(uint8_t)27		
/* HCM_INLINE_BASE_HYST_TEMP_POS CONFIG_INLINE_BASE_TEMP_SETTING*/
#define CONFIG_PARA_28												(uint8_t)28		
/* HCM_INLINE_BASE_HYST_TEMP_NEG CONFIG_INLINE_BASE_TEMP_SETTING*/
#define CONFIG_PARA_29												(uint8_t)29		
/* HCM_INLINE_HARD_SET_TEMP CONFIG_INLINE_HARD_TEMP_SETTING */
#define CONFIG_PARA_30												(uint8_t)30		
/* HCM_INLINE_HARD_THRESH_TEMP_LOW CONFIG_INLINE_HARD_TEMP_SETTING */
#define CONFIG_PARA_31												(uint8_t)31		
/* HCM_INLINE_HARD_THRESH_TEMP_HIGH CONFIG_INLINE_HARD_TEMP_SETTING*/
#define CONFIG_PARA_32												(uint8_t)32		
/* HCM_INLINE_HARD_HYST_TEMP_POS CONFIG_INLINE_HARD_TEMP_SETTING*/
#define CONFIG_PARA_33												(uint8_t)33		
/* HCM_INLINE_HARD_HYST_TEMP_NEG CONFIG_INLINE_HARD_TEMP_SETTING*/
#define CONFIG_PARA_34												(uint8_t)34		
/* HCM_BASE_LIQUID_THRESH_LEVEL_LOW CONFIG_BASE_LEVEL_SETTING*/
#define CONFIG_PARA_35												(uint8_t)35		
/* HCM_BASE_LIQUID_THRESH_LEVEL_HIGH CONFIG_BASE_LEVEL_SETTING*/
#define CONFIG_PARA_36												(uint8_t)36		
/* HCM_HARD_LIQUID_THRESH_LEVEL_LOW CONFIG_HARD_LEVEL_SETTING*/
#define CONFIG_PARA_37												(uint8_t)37		
/* HCM_HARD_LIQUID_THRESH_LEVEL_HIGH CONFIG_HARD_LEVEL_SETTING*/
#define CONFIG_PARA_38												(uint8_t)38		
/* HCM_BASE_TANK_HEIGHT CONFIG_TANK_HEIGHT */
#define CONFIG_PARA_39												(uint8_t)39		
/* HCM_HARD_TANK_HEIGHT CONFIG_TANK_HEIGHT*/
#define CONFIG_PARA_40												(uint8_t)40		
/* HCM_LIQUID_LEVEL_SAMPLING_RATE CONFIG_LL_SAMPLE_RATE*/
#define CONFIG_PARA_41												(uint8_t)41		
/* HCM_INLINE_BASE_THERM_CONN,HCM_INLINE_HARD_THERM_CONN,
HCM_INLINE_ANTIFREEZE_THERM_CONN,HCM_RETURN_BASE_THERM_CONN 
CONFIG_THERMOCOUPLE_REMAP */
#define CONFIG_PARA_42												(uint8_t)42		
/*HCM_INLINE_BASE_HEATER_CONN,HCM_INLINE_HARD_HEATER_CONN
  HCM_INLINE_ANTIFREEZE_HEATER_CONN,HCM_HARD_HEATER_CONN
	CONFIG_SSR_REMAP */
#define CONFIG_PARA_43												(uint8_t)43		
/* HCM_BASE_LIQUID_LEVEL_CONN,HCM_HARD_LIQUID_LEVEL_CONN
 CONFIG_LL_SENSOR_REMAP*/
#define CONFIG_PARA_44												(uint8_t)44		
/*HCM_BYPASS  CONFIG_HCM_PERIPHRAL_BYPASS */
#define CONFIG_PARA_45												(uint8_t)45		
/***************************OPMM*********************************/
/* OPMM_SPRAY_COUNT  CONFIG_SPRAY_COUNT */
#define CONFIG_PARA_46												(uint8_t)46		
/* OPMM_PRIME_TIME CONFIG_PRIME_TIME*/
#define CONFIG_PARA_47												(uint8_t)47		
/* OPMM_GUN_DELAY_TIME CONFIG_OPMM_GUN_DELAY*/
#define CONFIG_PARA_48												(uint8_t)48		
/* OPMM_SPRAY_OVERLAPTIME CONFIG_OPMM_SPRAY_OVERLAP_TIME */
#define CONFIG_PARA_49												(uint8_t)49		
/* OPMM_REVERSE_DELAYTIME CONFIG_OPMM_REVERSE_DELAY */
#define CONFIG_PARA_50												(uint8_t)50		
/* OPMM_FLUSH_TIME CONFIG_OPMM_FLUSH_TIME */
#define CONFIG_PARA_51												(uint8_t)51		
/* OPMM_DCLS_HOME_REACH_TIME CONFIG_DCLS_HOME_TIME */
#define CONFIG_PARA_52												(uint8_t)52		
/* OPMM_HOME_DCLS_REACH_TIME CONFIG_DCLS_HOME_TIME */
#define CONFIG_PARA_53												(uint8_t)53		
/* OPMM_GUN_POSITION_CHANGE_TIME CONFIG_GUN_POSITION_CHANGE_TIME */
#define CONFIG_PARA_54												(uint8_t)54		
/* OPMM_DELAY_SPRAY_VALVE_FLUSH_VBAT_TIME CONFIG_SPRAY_VALVE_OPEN_TIME_VBAT */
#define CONFIG_PARA_55												(uint8_t)55		
/* OPMM_FLUSH_WARNNING_TIME CONFIG_FLUSH_WARNNING_TIME */
#define CONFIG_PARA_56												(uint8_t)56		
/* OPMM_SPRAY_VALVE_CONN,OPMM_GUNPOS_VALVE_CONN,OPMM_AB_VALVE_CONN
OPMM_SOLVENT_VALVE_CONN,OPMM_FORWARD_VALVE_CONN,OPMM_BACKWARD_VALVE_CONN
CONFIG_OPMM_VALVE_REMAP */
#define CONFIG_PARA_57												(uint8_t)57		
/* OPMM_BYPASS CONFIG_OPMM_PERIPHRAL_BYPASS */
#define CONFIG_PARA_58												(uint8_t)58		
/***************************SCU*********************************/
/* SCU_SPRAY_PRESSURE_VALUE CONFIG_SPRAY_PRESSURE */
#define CONFIG_PARA_59												(uint8_t)59		
/* SCU_SPRAY_PRESSURE_THRESH_LOW CONFIG_SPRAY_PRESSURE */
#define CONFIG_PARA_60												(uint8_t)60		
/* SCU_SPRAY_PRESSURE_THRESH_HIGH CONFIG_SPRAY_PRESSURE */
#define CONFIG_PARA_61												(uint8_t)61		
/* SCU_SPRAY_PRESSURE_HYST_POS CONFIG_SPRAY_PRESSURE */
#define CONFIG_PARA_62												(uint8_t)62		
/* SCU_SPRAY_PRESSURE_HYST_NEG CONFIG_SPRAY_PRESSURE */
#define CONFIG_PARA_63												(uint8_t)63		
/* SCU_RECIRC_PRESSURE_VALUE CONFIG_RECIRC_PRESSURE */
#define CONFIG_PARA_64												(uint8_t)64		
/* SCU_RECIRC_PRESSURE_THRESH_LOW CONFIG_RECIRC_PRESSURE */
#define CONFIG_PARA_65												(uint8_t)65		
/* SCU_RECIRC_PRESSURE_THRESH_HIGH CONFIG_RECIRC_PRESSURE */
#define CONFIG_PARA_66												(uint8_t)66		
/* SCU_RECIRC_PRESSURE_HYST_POS CONFIG_RECIRC_PRESSURE */
#define CONFIG_PARA_67												(uint8_t)67		
/* SCU_RECIRC_PRESSURE_HYST_NEG CONFIG_RECIRC_PRESSURE */
#define CONFIG_PARA_68												(uint8_t)68		
/* COUNT_JOINT CONFIG_JOINT_NUMBER */
#define CONFIG_PARA_69												(uint8_t)69		
/* TIME_DATA_LOGGING CONFIG_DATA_LOG_PERIOD */
#define CONFIG_PARA_70												(uint8_t)70		
/* SYS_TIME CONFIG_SYS_DATE_TIME */
#define CONFIG_PARA_71												(uint8_t)71		
/* SUPERVISOR_NAME1 CONFIG_SUPERVISOR_NAME */
#define CONFIG_PARA_72												(uint8_t)72		
/* SUPERVISOR_NAME2 CONFIG_SUPERVISOR_NAME */
#define CONFIG_PARA_73												(uint8_t)73		
/* SUPERVISOR_NAME3 CONFIG_SUPERVISOR_NAME */
#define CONFIG_PARA_74												(uint8_t)74		
/* COATING_MAT_NAME1 CONFIG_COATING_MATERIAL */
#define CONFIG_PARA_75												(uint8_t)75		
/* COATING_MAT_NAME2 CONFIG_COATING_MATERIAL */
#define CONFIG_PARA_76												(uint8_t)76		
/* COATING_MAT_NAME3 CONFIG_COATING_MATERIAL */
#define CONFIG_PARA_77												(uint8_t)77		
/* BASE_MAT_BATCHNO1 CONFIG_BASE_MAT_BATCH_NO */
#define CONFIG_PARA_78												(uint8_t)78		
/* BASE_MAT_BATCHNO2 CONFIG_BASE_MAT_BATCH_NO */
#define CONFIG_PARA_79												(uint8_t)79		
/* BASE_MAT_BATCHNO3 CONFIG_BASE_MAT_BATCH_NO */
#define CONFIG_PARA_80												(uint8_t)80		
/* HARD_MAT_BATCHNO1 CONFIG_HARD_MAT_BATCH_NO */
#define CONFIG_PARA_81												(uint8_t)81		
/* HARD_MAT_BATCHNO2 CONFIG_HARD_MAT_BATCH_NO */
#define CONFIG_PARA_82												(uint8_t)82		
/* HARD_MAT_BATCHNO3 CONFIG_HARD_MAT_BATCH_NO */
#define CONFIG_PARA_83												(uint8_t)83		
/* SCU_UNIT_TYPE CONFIG_TYPE_OF_UNIT */
#define CONFIG_PARA_84												(uint8_t)84		
/* SCU_BYPASS CONFIG_SCU_PERIPHRAL_BYPASS */
#define CONFIG_PARA_85												(uint8_t)85		
/* OPMM_PIPE_TEMP_THRES_LOW CONFIG_PIPE_TEMP_TH */
#define CONFIG_PARA_86												(uint8_t)86		
/* OPMM_PIPE_TEMP_THRES_HIGH CONFIG_PIPE_TEMP_TH */
#define CONFIG_PARA_87												(uint8_t)87		
/* TEMP_PRESS_ACHIEVE_BYPASS CONFIG_TEMP_PRESS_ACHIEVE_BYPASS */
#define CONFIG_PARA_88												(uint8_t)88		
/* OPMM_NON_CONTACT12_CONN,OPMM_NON_CONTACT3_CONN,OPMM_NON_CONTACT6_CONN
OPMM_NON_CONTACT9_CONN,
CONFIG_OPMM_NON_CONTACT_REMAP */
#define CONFIG_PARA_89												(uint8_t)89		
/* ETHERNET IP ADDRESS */
#define CONFIG_PARA_90												(uint8_t)90		
/* ETHERNET SUBNET MASK ADDRESS */
#define CONFIG_PARA_91												(uint8_t)91		
/* ETHERNET GATEWAY */
#define CONFIG_PARA_92												(uint8_t)92		
/* ETHERNET PORT NUMBER */
#define CONFIG_PARA_93												(uint8_t)93		
/* OPMM BUZZER VOLUME */
#define CONFIG_PARA_94												(uint8_t)94		
/* SCU SIGNATURE BYTE */
#define CONFIG_PARA_95												(uint8_t)95		
/*****************change8**********************************/
/*SCU_TIME_SEND_STALL_COMMAND  CONFIG_SCU_STALL_COMMAND_TIME*/
#define CONFIG_PARA_96												(uint8_t)96		
/*OPMM_STOP_HLS_TIMER CONFIG_OPMM_STOP_HLS_TIME*/
#define CONFIG_PARA_97												(uint8_t)97		
/* OPMM_BATT_KNEE_VOLT CONFIG_OPMM_BATT_KNEE_VOLT*/
#define CONFIG_PARA_98												(uint8_t)98	
/***********************************change10****************************************/
/*VCM_STALL_UPSTROKE_UPPER_LIMIT CONFIG_VCM_STALL_RANGE*/
#define CONFIG_PARA_99												(uint8_t)99				
/*VCM_STALL_UPSTROKE_MIN_RANGE	CONFIG_VCM_STALL_RANGE*/
#define CONFIG_PARA_100												(uint8_t)100		
/*VCM_STALL_UPSTROKE_MAX_RANGE	CONFIG_VCM_STALL_RANGE*/
#define CONFIG_PARA_101												(uint8_t)101					
/*VCM_STALL_DOWNSTROKE_UPPER_LIMIT	CONFIG_VCM_STALL_RANGE*/
#define CONFIG_PARA_102												(uint8_t)102				
/*VCM_STALL_DOWNSTROKE_MIN_RANGE	CONFIG_VCM_STALL_RANGE*/
#define CONFIG_PARA_103												(uint8_t)103					
/*VCM_STALL_DOWNSTROKE_MAX_RANGE	CONFIG_VCM_STALL_RANGE*/
#define CONFIG_PARA_104												(uint8_t)104	
/*change18*/
/* WIFI IP ADDRESS */
#define CONFIG_PARA_105												(uint8_t)105		
/* WIFI SUBNET MASK ADDRESS */
#define CONFIG_PARA_106												(uint8_t)106		
/* WIFI GATEWAY */
#define CONFIG_PARA_107												(uint8_t)107		
/* WIFI PORT NUMBER */
#define CONFIG_PARA_108												(uint8_t)108	
/* WIFI SSID */
#define CONFIG_PARA_109												(uint8_t)109	
/* WIFI PASSWORD */
#define CONFIG_PARA_110												(uint8_t)110	
/*change21*/
/* JOB_NAME1 CONFIG_JOB_NAME */
#define CONFIG_PARA_111												(uint8_t)111		
/* JOB_NAME2 CONFIG_JOB_NAME */
#define CONFIG_PARA_112												(uint8_t)112		
/* JOB_NAME3 CONFIG_JOB_NAME */
#define CONFIG_PARA_113												(uint8_t)113		
/* IDLE_DATA_LOGGING CONFIG_IDLE_DATA_LOG_PERIOD */
#define CONFIG_PARA_114												(uint8_t)114		
/* PIPE_DIAMETER CONFIG_PIPE_DIAMETER */
#define CONFIG_PARA_115												(uint8_t)115		
/* HMI_TIME_OFFSET_SIGN CONFIG_TIME_ZONE_OFFSET */
#define CONFIG_PARA_116												(uint8_t)116		
/* JOB_ID1  CONFIG_JOB_ID */
#define CONFIG_PARA_117												(uint8_t)117	
/* JOB_ID2  CONFIG_JOB_ID */
#define CONFIG_PARA_118												(uint8_t)118	
/* JOB_ID3  CONFIG_JOB_ID */
#define CONFIG_PARA_119												(uint8_t)119	
/* BASE_TOTAL_MAT_USED,HARD_TOTAL_MAT_USED,
	BASE_PRIME_MAT_ACC, HARD_PRIME_MAT_ACC ,
	BASE_APPLIED_MAT_ACC , HARD_APPLIED_MAT_ACC ,
	BASE_TOTAL_MAT_USED_ACC, HARD_TOTAL_MAT_USED_ACC
DATA_ACCUMULATED_MATERIAL_USED */
#define CONFIG_PARA_120												(uint8_t)120	



/* DEFAULT VALUES */
/************************ VCM ****************************-*/
#define DEFAULT_VCM_UPSTROKE_TH								(uint16_t)100							
#define DEFAULT_VCM_DOWNSTROKE_TH							(uint16_t)100								
#define DEFAULT_VCM_PRESSURE_LOWTH1						 1.0f			
#define DEFAULT_VCM_PRESSURE_HIGHTH1					 40.0f						
#define DEFAULT_VCM_PRESSURE_LOWTH2						 1.0f												

#define DEFAULT_VCM_PRESSURE_HIGHTH2					 40.0f											
#define DEFAULT_VCM_PRESSURE_LOWTH3						 1.0f			
#define DEFAULT_VCM_PRESSURE_HIGHTH3					 40.0f	
#define DEFAULT_VCM_CYLINDER_AREA1						 150.0f			
#define DEFAULT_VCM_CYLINDER_AREA2						 150.0f								
#define DEFAULT_VCM_CYLINDER_AREA3						 150.0f									
#define DEFAULT_VCM_CYLINDER_COUNT						 (uint8_t)1									
#define DEFAULT_VCM_PRESSURE_GAUGE1_CONN			 (uint8_t)1								
#define DEFAULT_VCM_PRESSURE_GAUGE2_CONN			 (uint8_t)2								
#define DEFAULT_VCM_PRESSURE_GAUGE3_CONN			 (uint8_t)3								

#define DEFAULT_VCM_LINEAR_TRANSDUCER_CONN			(uint8_t)1							
#define DEFAULT_VCM_EE07_INSIDE_CONN					  (uint8_t)1										
#define DEFAULT_VCM_EE07_OUTSIDE_CONN			 			(uint8_t)2										
#define DEFAULT_VCM_STALL_VALVE_CONN				 		(uint8_t)1		
#define DEFAULT_VCM_RECIRC_VALVE_CONN				 		(uint8_t)3
#define DEFAULT_VCM_BYPASS											(uint8_t)0															
#define DEFAULT_VCM_BEFORE_STALL								2.0f
#define DEFAULT_VCM_COMPARE_STALL								3.0f

/************************ HCM ****************************-*/
#define DEFAULT_HCM_ANTI_SET_TEMP								35.0f
#define DEFAULT_HCM_ANTI_THRESH_TEMP_LOW				30.0f
#define DEFAULT_HCM_ANTI_THRESH_TEMP_HIGH				45.0f
#define DEFAULT_HCM_ANTI_HYST_TEMP_POS					1.0f
#define DEFAULT_HCM_ANTI_HYST_TEMP_NEG				  1.0f

#define DEFAULT_HCM_INLINE_BASE_SET_TEMP				35.0f		
#define DEFAULT_HCM_INLINE_BASE_THRESH_TEMP_LOW	30.0f		
#define DEFAULT_HCM_INLINE_BASE_THRESH_TEMP_HIGH 45.0f		
#define DEFAULT_HCM_INLINE_BASE_HYST_TEMP_POS				1.0f
#define DEFAULT_HCM_INLINE_BASE_HYST_TEMP_NEG				1.0f

#define DEFAULT_HCM_INLINE_HARD_SET_TEMP					35.0f	
#define DEFAULT_HCM_INLINE_HARD_THRESH_TEMP_LOW		30.0f		

#define DEFAULT_HCM_INLINE_HARD_THRESH_TEMP_HIGH		45.0f
#define DEFAULT_HCM_INLINE_HARD_HYST_TEMP_POS				 1.0f
#define DEFAULT_HCM_INLINE_HARD_HYST_TEMP_NEG				 1.0f

#define DEFAULT_HCM_BASE_LIQUID_THRESH_LEVEL_LOW		 		30.0f
#define DEFAULT_HCM_BASE_LIQUID_THRESH_LEVEL_HIGH				180.0f
#define DEFAULT_HCM_HARD_LIQUID_THRESH_LEVEL_LOW				30.0f
#define DEFAULT_HCM_HARD_LIQUID_THRESH_LEVEL_HIGH				180.0f
#define DEFAULT_HCM_BASE_TANK_HEIGHT										200.0f	

#define DEFAULT_HCM_HARD_TANK_HEIGHT								    200.0f
#define DEFAULT_HCM_LIQUID_LEVEL_SAMPLING_RATE			    (uint16_t)100
#define DEFAULT_HCM_INLINE_BASE_THERM_CONN					 		(uint8_t)1
#define DEFAULT_HCM_INLINE_HARD_THERM_CONN							(uint8_t)2
#define DEFAULT_HCM_INLINE_ANTIFREEZE_THERM_CONN				(uint8_t)3
#define DEFAULT_HCM_RETURN_BASE_THERM_CONN							(uint8_t)4
#define DEFAULT_HCM_INLINE_BASE_HEATER_CONN							(uint8_t)1
#define DEFAULT_HCM_INLINE_HARD_HEATER_CONN							(uint8_t)2
#define DEFAULT_HCM_INLINE_ANTIFREEZE_HEATER_CONN				(uint8_t)3
#define DEFAULT_HCM_HARD_HEATER_CONN										(uint8_t)4

#define DEFAULT_HCM_BASE_LIQUID_LEVEL_CONN							(uint8_t)1				
#define DEFAULT_HCM_HARD_LIQUID_LEVEL_CONN							(uint8_t)2
#define DEFAULT_HCM_BYPASS															(uint16_t)0											

/************************ OPMM ************************/
						
#define DEFAULT_OPMM_SPRAY_COUNT												(uint8_t)2										
#define DEFAULT_OPMM_PRIME_TIME													5.0f								  		
#define DEFAULT_OPMM_GUN_DELAY_TIME											1.0f			
#define DEFAULT_OPMM_SPRAY_OVERLAPTIME									1.0f
#define DEFAULT_OPMM_REVERSE_DELAYTIME									2.0
#define DEFAULT_OPMM_FLUSH_TIME													5.0f
#define DEFAULT_OPMM_DCLS_HOME_REACH_TIME								150.0f
#define DEFAULT_OPMM_HOME_DCLS_REACH_TIME								150.0f
#define DEFAULT_OPMM_GUN_POSITION_CHANGE_TIME						20.0f
#define DEFAULT_OPMM_DELAY_SPRAY_VALVE_FLUSH_VBAT_TIME  5.0f		  
#define DEFAULT_OPMM_FLUSH_WARNNING_TIME								180.0f
#define DEFAULT_OPMM_SPRAY_VALVE_CONN										(uint8_t)1
#define DEFAULT_OPMM_GUNPOS_VALVE_CONN									(uint8_t)2
#define DEFAULT_OPMM_AB_VALVE_CONN											(uint8_t)3
#define DEFAULT_OPMM_SOLVENT_VALVE_CONN									(uint8_t)4
#define DEFAULT_OPMM_FORWARD_VALVE_CONN									(uint8_t)5
#define DEFAULT_OPMM_BACKWARD_VALVE_CONN								(uint8_t)6
#define DEFAULT_OPMM_BYPASS															(uint16_t)0
#define DEFAULT_OPMM_NON_CONTACT12_CONN									(uint8_t)1
#define DEFAULT_OPMM_NON_CONTACT3_CONN									(uint8_t)2
#define DEFAULT_OPMM_NON_CONTACT6_CONN									(uint8_t)3
#define DEFAULT_OPMM_NON_CONTACT9_CONN									(uint8_t)4
/************************ SCU ************************/
#define DEFAULT_SCU_SPRAY_PRESSURE_VALUE								5.0f						
#define DEFAULT_SCU_SPRAY_PRESSURE_THRESH_LOW						3.0f
#define DEFAULT_SCU_SPRAY_PRESSURE_THRESH_HIGH					7.0f
#define DEFAULT_SCU_SPRAY_PRESSURE_HYST_POS							1.0f

#define DEFAULT_SCU_SPRAY_PRESSURE_HYST_NEG							1.0f
#define DEFAULT_SCU_RECIRC_PRESSURE_VALUE								4.0f
#define DEFAULT_SCU_RECIRC_PRESSURE_THRESH_LOW					2.0f
#define DEFAULT_SCU_RECIRC_PRESSURE_THRESH_HIGH					6.0f

#define DEFAULT_SCU_RECIRC_PRESSURE_HYST_POS						1.0f
#define DEFAULT_SCU_RECIRC_PRESSURE_HYST_NEG						1.0f

#define DEFAULT_COUNT_JOINT															1.0f	
#define DEFAULT_TIME_DATA_LOGGING												1.0f

#define DEFAULT_SYS_TIME_YEARH													(uint8_t)20
#define DEFAULT_SYS_TIME_YEARL													(uint8_t)15	
#define DEFAULT_SYS_TIME_MONTH													(uint8_t)12	
#define DEFAULT_SYS_TIME_DATE														(uint8_t)3
#define DEFAULT_SYS_TIME_SEC														(uint8_t)30
#define DEFAULT_SYS_TIME_MIN														(uint8_t)15
#define DEFAULT_SYS_TIME_HOUR														(uint8_t)1

/************************* Job Settings ************************/
#define DEFAULT_SUPERVISOR_NAME1										 		"CRC"
#define DEFAULT_SUPERVISOR_NAME2												"EVANS"
#define DEFAULT_SUPERVISOR_NAME3												"SBD"
#define DEFAULT_COATING_MAT_NAME1												"COAT1"
#define DEFAULT_COATING_MAT_NAME2												"COAT2"
#define DEFAULT_COATING_MAT_NAME3												"COAT3"
#define DEFAULT_BASE_MAT_BATCHNO1												"BASE1"
#define DEFAULT_BASE_MAT_BATCHNO2												"BASE2"
#define DEFAULT_BASE_MAT_BATCHNO3												"BASE3"
#define DEFAULT_HARD_MAT_BATCHNO1												"HARD1"
#define DEFAULT_HARD_MAT_BATCHNO2												"HARD2"
#define DEFAULT_HARD_MAT_BATCHNO3												"HARD3"
#define DEFAULT_SCU_UNIT_TYPE														(uint8_t)1
#define DEFAULT_SCU_BYPASS															(uint8_t)0
#define DEFAULT_TEMP_PRESS_ACHIEVE_BYPASS								(uint8_t)0
#define DEFAULT_OPMM_PIPE_TEMP_THRES_LOW								30.0f	
#define DEFAULT_OPMM_PIPE_TEMP_THRES_HIGH								40.0f	
#define DEFAULT_OPMM_BUZZER_VOLUME											(uint8_t)0
/***************change8**************************** */
#define DEFAULT_SCU_TIME_SEND_STALL_COMMAND							5.0f		
#define DEFAULT_OPMM_STOP_HLS_TIMER											0.5f		
#define DEFAULT_OPMM_BATT_KNEE_VOLT											19.5f	

/***************change10**************************** */
#define DEFAULT_VCM_STALL_UPSTROKE_UPPER_LIMIT				10
#define DEFAULT_VCM_STALL_UPSTROKE_MIN_RANGE					30
#define DEFAULT_VCM_STALL_UPSTROKE_MAX_RANGE					70
#define DEFAULT_VCM_STALL_DOWNSTROKE_UPPER_LIMIT			120
#define DEFAULT_VCM_STALL_DOWNSTROKE_MIN_RANGE				60
#define DEFAULT_VCM_STALL_DOWNSTROKE_MAX_RANGE				110

/***************change21**************************** */
#define DEFAULT_JOB_NAME1										 			"CRC1"
#define DEFAULT_JOB_NAME2													"EVANS1"
#define DEFAULT_JOB_NAME3													"SBD1"

/*  CONFIG_IDLE_DATA_LOG_PERIOD */
#define DEFAULT_IDLE_DATA_LOGGING										30.0f		
/* PIPE_DIAMETER CONFIG_PIPE_DIAMETER */
#define DEFAULT_PIPE_DIAMETER												10.0f	
/*change23*/
#define DEFAULT_CURRENT_INDEX_SUPERVISOR_NAME				0
#define DEFAULT_CURRENT_INDEX_JOB_NAME							0
#define DEFAULT_CURRENT_INDEX_JOB_ID								0  /*change48*/
/***********************************change37****************************************/
#define DEFAULT_HMI_TIME_OFFSET_SIGN								0
#define DEFAULT_HMI_TIME_OFFSET_HOUR								0
#define DEFAULT_HMI_TIME_OFFSET_MIN									0
#define DEFAULT_HMI_TIME_OFFSET_SEC									0

/***************change42**************************** */
#define DEFAULT_JOB_ID1										 			"CRC1"
#define DEFAULT_JOB_ID2													"EVANS1"
#define DEFAULT_JOB_ID3													"SBD1"


/*change43*/
#define DEFAULT_BASE_TOTAL_MAT_USED									0
#define DEFAULT_HARD_TOTAL_MAT_USED									0
/*change43*/
#define DEFAULT_BASE_PRIME_MAT_ACC									0
#define DEFAULT_HARD_PRIME_MAT_ACC									0
#define DEFAULT_BASE_APPLIED_MAT_ACC								0
#define DEFAULT_HARD_APPLIED_MAT_ACC								0
#define DEFAULT_BASE_TOTAL_MAT_USED_ACC							0
#define DEFAULT_HARD_TOTAL_MAT_USED_ACC							0









/* ----------------------------------------------------------------------------
 *                           External Functions
 * ----------------------------------------------------------------------------
 */
extern void fnSPI_EEPROM_WriteData(uint16_t, uint8_t, uint8_t *);
extern void fnSPI_EEPROM_ReadData(uint16_t, uint8_t, uint8_t *);
extern uint8_t fnWrite_Status_page(void);
extern uint8_t fnSPI_Read_Status_page(void);
extern void fnSPIWrite_enable(void);
extern void fnWrite_disable(void);
extern uint8_t fnTestEEPROM(void);





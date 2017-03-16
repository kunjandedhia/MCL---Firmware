/******************************************************************************
  **@file     :    Can.h
  * 
  * @brief  	:		 Macros defined for MCL CAN MESSAGE PROTOCOL.
  ******************************************************************************
  */

#define TYPE_MASK															((uint32_t)0x1C000000)
#define MSG_MASK															((uint32_t)0x00FF0000)	
#define DEST_ADDR_MASK												((uint32_t)0x0000FF00)
#define SOURCE_ADDR_MASK											((uint32_t)0x000000FF)

#define DEST_ADDR_CLEAR_MASK									((uint32_t)0xffff00ff)
/*****source address*****/
#define SOURCE_OP															((uint32_t)0x00000001)		/*Operator module source address*/
#define SOURCE_MCU														((uint32_t)0x00000002)		/*MCU source address*/
#define SOURCE_OPMM														((uint32_t)0x00000003)		/*OPMM source address*/
#define SOURCE_MFM														((uint32_t)0x00000010)		/*MFM source address*/
#define SOURCE_BHCM														((uint32_t)0x00000030)		/*BHCM source address*/
#define SOURCE_HHCM														((uint32_t)0x00000031)		/*HHCM source address*/
#define SOURCE_IHCM1													((uint32_t)0x00000032)		/*IHCM1 source address*/
#define SOURCE_IHCM4_HM												((uint32_t)0x00000036)		/*IHCM4 source address for hardner material*/

/*****DESTINATION ADDRESS*****/
#define DEST_OP																((uint32_t)0x00000100)		/*Operator module destination address*/
#define DEST_MCU															((uint32_t)0x00000200)		/*MCU destination address*/
#define DEST_OPMM															((uint32_t)0x00000300)		/*OPMM destination address*/
#define DEST_MFM															((uint32_t)0x00001000)		/*MFM destination address*/
#define DEST_BHCM															((uint32_t)0x00003000)		/*BHCM destination address*/
#define DEST_HHCM															((uint32_t)0x00003100)		/*HHCM destination address*/
#define DEST_IHCM1														((uint32_t)0x00003200)		/*IHCM1 destination address*/
#define DEST_IHCM4_HM													((uint32_t)0x00003600)		/*IHCM4 destination address(Hardner Material)*/

#define DEST_ALL															((uint32_t)0x0000FF00)		/*Broadcast message destination mask*/
/*****RESERVED ADDRESS*****/
#define RESERVED															((uint32_t)0x00000000)		/*ADDRESS FOR RESERVED FIELD*/

/*****MESSAGE TYPE*****/
#define MSG_TYPE_BROADCAST										((uint32_t)0x00000000)		/*BROADCAST MESSAGE ADDRESS*/
#define MSG_TYPE_COMMAND											((uint32_t)0x04000000)		/*COMMAND MESSAGE ADDRESS*/
#define MSG_TYPE_FAULT												((uint32_t)0x08000000)		/*FAULT MESSAGE ADDRESS*/
#define MSG_TYPE_STATUS												((uint32_t)0x0C000000)		/*STATUS MESSAGE ADDRESS*/
#define MSG_TYPE_DATA													((uint32_t)0x10000000)		/*DATA MESSAGE ADDRESS*/
#define MSG_TYPE_CONFIG												((uint32_t)0x14000000)		/*CONFIGURATION MESSAGE ADDRESS*/

/*****MESSAGE ID----FOR BROADCAST COMMAND*****/
#define BCAST_ESTOP	 														((uint32_t)0x00000000)	/*ESTOP */							
#define BCAST_HEARTBEAT													((uint32_t)0x00010000)	/*Broadcast message*/

/*****MESSAGE ID----FOR COMMANDS*****/
#define CMD_SET_SYSTEM_MODE										((uint32_t)0x00010000)		/*SET SYSTEM MODE*/
#define CMD_START_PAUSE												((uint32_t)0x00020000)		/*START / PAUSE*/
#define CMD_SPRAY_SELECTOR										((uint32_t)0x00030000)		/*Spray Selector*/
#define CMD_SPRAY_START												((uint32_t)0x00040000)		/*Spray process start*/
#define CMD_PRIME_SELECTOR										((uint32_t)0x00050000)		/*Prime Selector*/
#define CMD_PRIME_START												((uint32_t)0x00060000)		/*Prime process start*/
#define CMD_FLUSH_SELECTOR										((uint32_t)0x00070000)		/*Flush Selector*/
#define CMD_FLUSH_START												((uint32_t)0x00080000)		/*Flush process start*/
#define CMD_STALL_TEST												((uint32_t)0x00090000)		/*Start Stall test*/	
#define CMD_LASER_BEAM												((uint32_t)0x000A0000)		/*Laser beam ON/OFF(OPMM)*/		
#define CMD_VALVE_OPMM												((uint32_t)0x000B0000)		/*Valve (OPMM)*/
#define CMD_VALVE_MFM													((uint32_t)0x000C0000)		/*Valve (MFM)*/
#define CMD_LINEAR_TRANS_MONITOR							((uint32_t)0x000D0000)		/*Linear Transducer Monitoring (MFM) */
#define CMD_OPMM_CONFIG												((uint32_t)0x000F0000)		/*OPMM Configuration*/
#define CMD_MFM_CONFIG												((uint32_t)0x00100000)		/*MFM Configuration*/	
#define CMD_BHCM_CONFIG												((uint32_t)0x00120000)		/*BHCM Configuration*/		
#define CMD_IHCM_CONFIG												((uint32_t)0x00130000)		/*IHCM1 Configuration*/				
#define CMD_HHCM_CONFIG												((uint32_t)0x00140000)		/*IHCM2 Configuration*/					
#define CMD_DEVICE_FUNCT											((uint32_t)0x00150000)		/*Device Functionality*/
#define CMD_CLEAR_FAULT												((uint32_t)0x00160000)		/*Clear Fault		*/
#define CMD_CLEAR_WARNING											((uint32_t)0x00170000)		/*Clear Warning*/
#define CMD_TURN_ON_OFF_HEATER								((uint32_t)0x00180000)		/*Turn ON Heater(BHCM, HHCM, IHCM1, IHCM4)*/
#define CMD_MANUAL_PROCESS_DONE								((uint32_t)0x00190000)		/*Manual prime/Flush process done*/
#define CMD_STOP_RUNTIME											((uint32_t)0x001B0000)		/*Stop Runtime Command*/

/*****MESSAGE ID----FOR FAULTS*****/
#define FAULT_OPERATION_TIME_OUT							((uint32_t)0x00000000)		/*Operation Time Out Error*/																																			
#define FAULT_OPMM_VALVE											((uint32_t)0x00010000)		/*Valve error (OPMM)*/																															
#define FAULT_MFM_VALVE												((uint32_t)0x00020000)		/*Valve error (MFM)*/																										
#define FAULT_BHCM_TEMP_TH										((uint32_t)0x00040000)		/*Temperature beyond threshold (BHCM)	*/																		
#define FAULT_BHCM_LIQUID_LEVEL   						((uint32_t)0x00050000)		/*Liquid Level below threshold (BHCM)	*/																			
#define FAULT_IHCM1_IBASE_TEMP_TH							((uint32_t)0x00060000)		/*Inline Base material temperature beyond threshold (IHCM1)	*/							
#define FAULT_MCU_IBASE_TEMP_LOW							((uint32_t)0x00070000)		/*Inline Base material temperature not in range*/											
#define FAULT_IHCM4_IHARD_TEMP_TH							((uint32_t)0x00080000)		/*Inline Hardener material temperature beyond threshold (IHCM4)*/							
#define FAULT_MCU_IHARD_TEMP_TH								((uint32_t)0x00090000)		/*Inline Hardener material temperature not in range*/ 									
#define FAULT_PGAUGE1_ABOVE_TH								((uint32_t)0x000A0000)		/*Pressure gauge 1 beyond threshold(MFM)	*/																	
#define FAULT_PGAUGE2_ABOVE_TH								((uint32_t)0x000B0000)		/*Pressure gauge 2 beyond threshold(MFM)	*/																	
#define FAULT_PGAUGE3_ABOVE_TH								((uint32_t)0x000C0000)		/*Pressure gauge 3 beyond threshold(MFM)*/																		
#define FAULT_PIPE_TEMP_OUT_OF_RANGE					((uint32_t)0x000D0000)		/*Pipe temperature is not in range*/	
#define FAULT_HB_ACK_NOT_RECEIVED							((uint32_t)0x000E0000)		/*HeartBeat Ack not Received*/
#define FAULT_HB_NOT_RECEIVED									((uint32_t)0x000F0000)		/*HeartBeat Not Received*/
#define FAULT_DEFECTIVE_PUMP									((uint32_t)0x00100000)		/*Defective pump*/
#define FAULT_DEFECTIVE_HEATER								((uint32_t)0x00110000)		/*Defective Heater*/
#define FAULT_ROTARY_SWITCH										((uint32_t)0x00120000)		/*Spray gun motion fault */
#define FAULT_SPRAY_GUN_MOTION								((uint32_t)0x00130000)		/*Spray gun motion fault*/
#define FAULT_LT_OUT_OF_RANGE					  			((uint32_t)0x00140000)		/*Linear transducer out of range*/
#define FAULT_OPMM_ROTATION							    	((uint32_t)0x00150000)		/*OPMM rotation fault*/
#define FAULT_HHCM_LIQUID_LEVEL   						((uint32_t)0x00160000)		/*Liquid Level below threshold (BHCM)		*/																		
#define FAULT_ERROR_WARNNING					      	((uint32_t)0x00170000)		/*FAULT_ERROR_WARNNING*/

/*****MESSAGE ID----FOR STATUS*****/
#define STATUS_STALL_TEST_RESULT							((uint32_t)0x00000000)		/*Stall test result*/
#define STATUS_SPRAY_PROCESS									((uint32_t)0x00010000)		/*Spray process status*/
#define STATUS_PRIME_PROCESS									((uint32_t)0x00020000)		/*Prime process status*/
#define STATUS_FLUSH_PROCESS									((uint32_t)0x00030000)		/*Flush process status*/
#define STATUS_SYSTEM_MODE										((uint32_t)0x00040000)		/*System mode status*/
#define STATUS_OPMM_ON_PIPE										((uint32_t)0x00050000)		/*OPMM is on pipe	*/																		
#define STATUS_PRIME_TIME_ACHIEVED						((uint32_t)0x00060000)		/*Prime time achieved	*/																
#define STATUS_MATERIAL_PRESSURE							((uint32_t)0x00070000)		/*Material pressure status*/
#define STATUS_MCU_DIAGNOSTIC									((uint32_t)0x00080000)		/*MCU Diagnostic*/
#define STATUS_OP_DIAGNOSTIC									((uint32_t)0x00090000)		/*OP Diagnostic*/
#define STATUS_OPMM_DIAGNOSTIC								((uint32_t)0x000A0000)		/*OPMM Diagnostic*/
#define STATUS_MFM_DIAGNOSTIC									((uint32_t)0x000B0000)		/*MFM Diagnostic*/
#define STATUS_BHCM_DIAGNOSTIC								((uint32_t)0x000D0000)		/*BHCM Diagnostic*/
#define STATUS_HHCM_DIAGNOSTIC								((uint32_t)0x000E0000)		/*HHCM Diagnostic*/
#define STATUS_IHCM1_DIAGNOSTIC								((uint32_t)0x000F0000)		/*IHCM1 Diagnostic*/
#define STATUS_IHCM4_DIAGNOSTIC_HM						((uint32_t)0x00130000)		/*IHCM4 Diagnostic(hardner Material)*/
#define STATUS_DEVICE_NOT_PRESENT							((uint32_t)0x00140000)		/*Device not Present*/
#define STATUS_GUN_POSITION						      	((uint32_t)0x00150000)		/*Gun position status*/
#define STATUS_VALVES_ON_OFF									((uint32_t)0x00160000)		/*Valve ON OFF status*/


/*****MESSAGE ID----FOR DATA*****/
#define DATA_MCU_RETURN_LINE_TEMP							((uint32_t)0x00000000)		/*Return line temperature(MCU)*/
#define DATA_MCU_AMB_TEMP_INSIDE							((uint32_t)0x00010000)		/*Ambient temperature of inside the container*/
#define DATA_MCU_AMB_TEMP_OUTSIDE							((uint32_t)0x00020000)		/*Ambient temperature of out side the container*/
#define DATA_MCU_AMB_HUMIDITY									((uint32_t)0x00030000)		/*Ambient Humidity of out side the container*/
#define DATA_OPMM_PIPE_TEMP										((uint32_t)0x00040000)		/*Pipe Temperature (Non - contact)(OPMM)*/
#define DATA_MFM_MATERIAL_USED								((uint32_t)0x00050000)		/*Volume of material used*/
#define DATA_PGAUGE1_VALUE										((uint32_t)0x00060000)		/*Value of pressure gauge 1(MFM)*/
#define DATA_PGAUGE2_VALUE										((uint32_t)0x00070000)		/*Value of pressure gauge 2(MFM)*/
#define DATA_PGAUGE3_VALUE										((uint32_t)0x00080000)		/*Value of pressure gauge 3(MFM)*/
#define DATA_BHCM_LIQUID_LEVEL								((uint32_t)0x00090000)		/*Liquid level of base material(BHCM)*/
#define DATA_BHCM_LIQUID_CURR_TEMP						((uint32_t)0x000A0000)		/*Current temperature of antifreeze liquid(BHCM)*/
#define DATA_IHCM1_MATERIAL_CURR_TEMP					((uint32_t)0x000B0000)		/*Current temperature of inline base material(IHCM1)*/
#define DATA_IHCM4_MATERIAL_CURR_TEMP					((uint32_t)0x000C0000)		/*Current temperature of inline hardner material(IHCM4)*/
#define DATA_STALL_PRESSURE_CHANGE						((uint32_t)0x000D0000)		/*Stall test pressure change (in %)*/
#define DATA_PROCESS_TIME                     ((uint32_t)0x000E0000)    /*Prime/Spray/Flush process time*/
#define DATA_DEW_POINT                        ((uint32_t)0x000F0000)		/* DEW POINT*/
#define DATA_ROTATIONS_COMPLETED              ((uint32_t)0x00100000)    /*OPMM no of rotations completed*/
#define DATA_OPMM_BATTERY_STATUS              ((uint32_t)0x00110000)    /*OPMM battery status in % */


/*****MESSAGE ID----FOR CONFIGURATION*****/
#define CONFIG_SPRAY_PRESSURE									((uint32_t)0x00000000)		/*Spray pressure value	*/																											
#define CONFIG_RECIRC_PRESSURE								((uint32_t)0x00010000)		/*Recirculation pressure value*/
#define CONFIG_PIPE_TEMP_TH										((uint32_t)0x00020000)		/*Pipe temperature(non-contact) threshold(OPMM)*/
#define CONFIG_SPRAY_COUNT										((uint32_t)0x00030000)		/*Spraying count(OPMM)*/
#define CONFIG_PRIME_TIME											((uint32_t)0x00040000)		/*Prime time(OPMM)*/
#define CONFIG_OPMM_GUN_DELAY									((uint32_t)0x00050000)		/*Gun delay time(OPMM)*/
#define CONFIG_OPMM_SPRAY_OVERLAP_TIME				((uint32_t)0x00060000)		/*Spray overlap time(OPMM)*/
#define CONFIG_OPMM_REVERSE_DELAY							((uint32_t)0x00070000)		/*Reverse delay time(OPMM)*/
#define CONFIG_OPMM_FLUSH_TIME								((uint32_t)0x00080000)		/*Flush Time(OPMM)*/
#define CONFIG_MFM_STALL_TH										((uint32_t)0x00090000)		/*Stall threshold in %(MFM)*/
#define CONFIG_MFM_PGAUGE1_TH									((uint32_t)0x000A0000)		/*Pressure Gauge 1 High threshold(MFM)*/																							
#define CONFIG_MFM_PGAUGE2_TH									((uint32_t)0x000B0000)		/*Pressure Gauge 2 High threshold(MFM)*/																					
#define CONFIG_MFM_PGAUGE3_TH									((uint32_t)0x000C0000)		/*Pressure Gauge 3 High threshold(MFM)*/																			
#define CONFIG_BHCM_BTANK_SET_TEMP						((uint32_t)0x000D0000)		/*Base tank temperature setting (BHCM)*/																						
#define CONFIG_BHCM_BTANK_TEMP_TH			      	((uint32_t)0x000E0000)		/*Base tank high temperature threshold (BHCM)*/									
#define CONFIG_BHCM_BTANK_LIQUID_TH				    ((uint32_t)0x000F0000)		/*Base tank liquid low level threshold (BHCM)*/						
#define CONFIG_IHCM_BLINE_SET_TEMP						((uint32_t)0x00100000)		/*Base inline material temperature setting(IHCM1)*/		
#define CONFIG_IHCM_BLINE_TEMP_TH							((uint32_t)0x00110000)		/*Base inline material high temperature threshold(IHCM1)*/
#define CONFIG_IHCM4_HLINE_SET_TEMP						((uint32_t)0x00120000)		/*Hardener Inline heater temperature setting (IHCM4)*/				
#define CONFIG_IHCM4_HLINE_TEMP_TH						((uint32_t)0x00130000)		/*Hardener Inline high temperature threshold (IHCM4)*/
#define CONFIG_OPERATION_TIME									((uint32_t)0x00140000)		/*Operation time*/																							
#define CONFIG_JOINT_NUMBER										((uint32_t)0x00150000)		/*Joint number*/
#define CONFIG_DATA_LOG_PERIOD								((uint32_t)0x00160000)		/*Data Logging Period*/
#define CONFIG_TYPE_IHCM4											((uint32_t)0x00170000)		/*Type of 4th IHCM*/
#define CONFIG_SYS_DATE_TIME									((uint32_t)0x00180000)		/*System Date abnd time*/
#define CONFIG_CYLINDER_SURFACE_AREA   				((uint32_t)0x001A0000)		/*Dew Point*/
#define CONFIG_NUMBER_OF_CYLINDER 	          ((uint32_t)0x001B0000)    /*Number of cylinder*/
#define CONFIG_SUPERVISOR_NAME                ((uint32_t)0x001C0000)    /*Supervisor name*/
#define CONFIG_COATING_MATERIAL               ((uint32_t)0x001D0000)    /*coating mterial*/
#define CONFIG_BASE_MAT_BATCH_NO              ((uint32_t)0x001E0000)    /*base material batch no*/
#define CONFIG_HARD_MAT_BATCH_NO	            ((uint32_t)0x001F0000)    /*Hardener material batch no*/
#define CONFIG_TIME_BEFORE_STALL              ((uint32_t)0x00200000)    /*Time before stall*/
#define CONFIG_TIME_BEFORE_COMPARE_STALL    	((uint32_t)0x00210000)    /*Time before comparing stall pressure*/
#define CONFIG_DCLS_HOME_TIME	                ((uint32_t)0x00220000)    /*DCLS-HOME reach timer*/
#define CONFIG_GUN_POSITION_CHANGE_TIME       ((uint32_t)0x00230000)    /*Gun position change time*/
#define CONFIG_SPRAY_VALVE_OPEN_TIME_VBAT     ((uint32_t)0x00240000)    /*Spray Valve Open time*/
#define CONFIG_TYPE_OF_UNIT							      ((uint32_t)0x00250000)    /*Type of Unit used to display the parameter*/
#define CONFIG_TEMP_HYSTERESIS								((uint32_t)0x00260000)		/*Hysteresis for Temperature*/
#define CONFIG_PRESSURE_HYSTERESIS						((uint32_t)0x00270000)		/*Hysteresis for Pressure*/
#define CONFIG_HHCM_LIQUID_LEVEL_TH				    ((uint32_t)0x00280000)		/*Hard tank liquid  level threshold (HHCM)*/
#define CONFIG_HCM_LIQUID_TANK_HEIGHT				  ((uint32_t)0x00290000)		/*Liquid tank Height */							
#define CONFIG_LL_WINDOW_SIZE 							  ((uint32_t)0x002A0000)		/*Liquid tank Height */							
#define CONFIG_FLUSH_WARNNING_TIME						((uint32_t)0x002B0000)		/*Liquid tank Height */

/*********Acknowledgement*********/
#define ACKNOWLEDGEMENT												((uint32_t)0x00E80000)		/*acknowledgement for each message*/

/*******************
TH								---Threshold
TEMP							---Tetmperature
CURR							---Current
HLINE							---Hardner Inline
BLINE							---Base Inline
BTANK							---Base Tank
HTANK							---Hardner Tank
********************/




/* ----------------------------------------------------------------------------
 *                           EXTERNAL VARIABLES
 * ----------------------------------------------------------------------------
 */

/* ----------------------------------------------------------------------------
 *                           EXTERNAL FUNCTIONS
 * ----------------------------------------------------------------------------
 */





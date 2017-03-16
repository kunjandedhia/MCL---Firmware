/******************************************************************************
**@file    LCD_MACROS.h
* 
**@brief   This file contains 
******************************************************************************
*/    
/* ----------------------------------------------------------------------------
*                           Includes
* ----------------------------------------------------------------------------
*/
/* ----------------------------------------------------------------------------
*                           MACROS
* ----------------------------------------------------------------------------
*/
#define INDEX_REG_MEMADDR       					0x6C000000     	//ADDRESS LOCATION for index register
#define DATA_REG_MEMADDR        					0x6C000004    	// Address Location for data register

#define DISPLAY_WIDTH_0_180     					240							//screen size 
#define DISPLAY_HEIGHT_0_180    					400       
#define DISPLAY_WIDTH_90_270    					400
#define DISPLAY_HEIGHT_90_270   					240


#define ORIENTATION_0_R003      					0x5030       		// input value in Register 3 for orientation 0
#define ORIENTATION_90_R003    						0x5018					// input value in Register 3 for orientation 90
#define ORIENTATION_180_R003    					0x5000					// input value in Register 3 for orientation 180
#define ORIENTATION_270_R003    					0x5028        	// input value in Register 3 for orientation 270

/*****************************for Colour******************************************/
#define RGB(r,g,b)             ( (uint32_t)( (b) | ((g) << 8) | ((r) << 16) ) )     
#define PACK_RGB(r,g,b)        ( (uint16_t)( (((b) >> 3) & 0x001F) | (((g) << 3) & 0x07E0) | (((r) << 8) & 0xF800) ) )
#define REPACK_RGB(c)          PACK_RGB( ((c) >> 16) & 0x000000FF, ((c) >> 8) & 0x000000FF, (c) & 0x000000FF )
#define UNPACK_RGB(w)          ( ((((uint32_t)(w)) & 0x0000001F) << 3) | ((((uint32_t)(w)) & 0x000007E0) << 5) | ((((uint32_t)(w)) & 0x0000F800) << 8) )

#define BLACK_CLR    					RGB(0,0,0)
#define WHITE_CLR    					RGB(255,255,255)
#define LIGHT_GREY_CLR		 		RGB(211,211,211)
#define DARK_GREY_CLR		 			RGB(121,121,121)
#define BLUE_CLR		 					RGB(0,162,255)
#define RED_CLR								RGB(255,0,0)
#define GREEN_CLR							RGB(0,255,0)
/******************************ERROR SCREEN COLOUR********************************/
#define ERROR_SCREEN_BACKGROUND     			RGB(192,0,0)
#define ERROR_SCREEN_FOREGROUND     			RGB(255,255,255)
#define INFO_SCREEN_BACKGROUND      			RGB(255,255,255)
#define INFO_SCREEN_FOREGROUND      			RGB(0,0,0)
#define NUMBER_OF_FONTS         					(sizeof(font_list) / sizeof(int *))

#define ORIENTATION_0         			 			0
#define ORIENTATION_90        			 			1
#define ORIENTATION_180       						2
#define ORIENTATION_270       			 			3
#define DEFAULT_ORIENTATION         			ORIENTATION_90

#define MAX_FONT_HEIGHT         					47
#define MAX_FONT_WIDTH          					43
#define MAX_FONT_WIDTH_BYTES    					((MAX_FONT_WIDTH + 7) / 8)


/*********************************************************************************/
#define SPAC     													" "
#define ALPHA_F  													"F"
#define SLASH															"/"
#define PERCENT														"%"
#define ALPHA_A	  												"A"
#define ALPHA_B  													"B"
#define ALPHA_C														"°C"
#define ALPHA_p	  												"p"
#define ALPHA_b														"b"
#define COLON															":"
#define POSITIVE													"+"
#define NEGATIVE													"-"
#define VOLT															"V"
#define ERR_STRING												"ERR"
#define ALPHA_C_BRACKET										"(°C)"
#define ALPHA_F_BRACKET										"(F)"
#define PSI_BRACKET												"(psi)"
#define BAR_BRACKET												"(bar)"
#define CENTIMETER												"cm"
#define MILIMETER													"mm"
#define SECONDS_BRACKET										"(seconds)"
/*#define dot  	    											"."		*/
#define MILISECOND												"ms"
#define LITRE															"(litre)"
#define OZ																"(oz)"
#define STARTUP														"Start up"
#define SCREEN    												"Screen"
#define STOP_OPTION      									"Stop"
#define RUNTIME														"Runtime"
#define AUTOCOAT													"Autocoat"
#define HOME															"Home"

#define JOINT															"Joint"
#define CURRENT														"Current"
#define ALARMS														"Alarms"
#define ENGINEERING												"Engineering"
#define CYCLE															"Cycle"
#define VALVE															"Valve"
#define MATERIAL													"Material"
#define USAGE															"usage"
#define GAL																"Gal"
#define BAR																"(bar)"
#define PSI																"(psi)"
#define DEW																"Dew"
#define LEG																"Leg"
#define HARDNER														"Hardner"
#define HARD															"Hard"
#define AMBIENT														"Ambient"
#define WATER															"Water"
#define LAST															"Last"
#define PRIMED														"Primed Acc"
#define APPLIED														"Applied Acc"
#define TOTAL_ACC													"Total Acc"
/*#define LEFT														"% Left"*/
#define OFPASSES REQUIRED									"of passes required"
#define OFPASSESCOMPLETE									"of passes complete"
#define GUNTIMERDELAY											"Gun timer delay(ms)"
#define OVERRUNDELAY											"Over run delay(ms)"
#define MINIMUMPURGETIME									"Minimum purge time"
#define MINIMUMFLUSHTIME									"Minimum flush time"

//#define WARMUP														"(Warm Up Mode)"
//#define MACHINECOATING										"machine coating"
#define READY															"Ready"
#define PIPE															"Pipe"
#define TEMP															"Temp"
#define GUN																"Gun"
#define DELAY															"Delay"
#define REVERSE														"Reverse"
#define WARNINGS													"Warnings"
#define YES																"Yes"
#define NO																"No"
#define CLOSED														"closed"
#define OPEN															"open"
#define PRIMING														"Priming"
#define SETTINGS													"Settings"
#define MIN																"(min)"
#define MAX																"(max)"
#define SET_STRING												"(set)"
#define SET_BUTTON												"set"
#define APPLY_BUTTON												"apply"
#define NA																"NA"
#define SOLVENT														"Solvent"
#define HARDENER													"Hardener"
#define SURFACE														"Surface"
#define BASE															"Base"


#define HARDNER_TANK_HEIGHT								"Hardner tank height"

#define LIQUID_LV_SAMPLING_RATE						"Liq lv Sampling rate"



#define JOB																"Job"
#define SUPERVISOR												"Supervisor"
#define COATING_MATERIAL									"Coating Material"
#define BATCHNO														"Batch No."
#define PUMP															"Pump"
#define STALL															"Stall"
#define THRESHOLD													"Threshold %"
#define DOWNSTROKE_STRING									"Downstroke"
#define UPSTROKE_STRING										"Upstroke"
#define RECORD														"Record"
#define PRESSURE													"Pressure"
#define ALLOWED														"Allowed %"
#define S																	"(s)"
#define BYPASS														"Bypass"
#define TRANSDUCER												"Transducer"
#define MANUAL														"(Manual)"
#define MANUAL_STRING											"Manual"
#define AUTOMATIC_STRING									"(Automatic)"
#define TEST															"Test"
#define LOG																"Log"
#define RESET_STRING											"Reset"
#define BASE_VOLUME												"Base Volume"
#define HARD_VOLUME												"Hardner Volume"
#define MACHINE														"Machine"
#define TOTAL															"Total"
#define HEADTO														"Head to"
#define PRIME_STRING											"Prime"
#define SPRAY_STRING											"Spray"
#define FLUSH_STRING											"Flush"
#define COMPLETE													"Complete"
#define OVERLAP														"Overlap"
#define ON																"On"
#define OFF																"Off"
#define AT																"at"
#define DOWN															"Down"
#define UP																"Up"
#define SOL																"Sol"
#define OPERATION													"Operation"
#define OPERATION_MODE_STRING							"(Operation Mode)"
#define CANCLE														"Cancle"
#define PROCESS														"Process"
#define TIME															"Time"
#define DATA_LOGGING											"Data Logging"
#define PERIOD														"Period"
#define IN																"in"
#define OUT																"out"
#define TO																"to"
#define CONTAINER													"Container"
#define CYCLE															"Cycle"
#define HEAD															"Head"
#define VALVE															"Valve"
#define ROTATION													"Rotation"
#define HOME															"Home"
#define END																"End"
#define LIQUID														"Liquid"
#define LV																"lv."
#define POSITION													"Position"
#define MODE															"Mode"
#define OPMM_SET													"OPMM Set on Pipe?"
#define NOTIFICATION_1										"Notifcation 1"
#define NOTIFICATION_2										"Notifcation 2"
#define NOTIFICATION_3										"Notifcation 3"
#define NOTIFICATION_4										"Notifcation 4"
#define PROCESS_TIME											"Process Time"

#define FORWARD														"Forward"
#define BACKWARD													"Backward"
#define FLUSH_WARNING_TIME								"Flush Warning Time"

#define ALARMS_1													"Alarms 1"
#define ALARMS_2													"Alarms 2"
#define ALARMS_3													"Alarms 3"
#define ALARMS_4													"Alarms 4"

#define CLEAR															"Clear"
#define SECONDS														"(Seconds)"
#define FAULTS														"Faults"

#define TURN_ON														"Turn ON  "
#define TURN_OFF													"Turn OFF"
#define LASER															"Laser"

#define DONE															"Done"
#define PROGRESS													"Progress"

#define STRING_PASS												"Pass"
#define STRING_FAIL												"Fail"


#define ENABLE_STRING											"Enable"
#define DISABLE_STRING										"Disable"
#define BUZZER														"Buzzer"

#define WARNING														"WARNING..!!!"
#define ERROR_STRING											"ERROR..!!!"

#define MANAGE														"Manage"
#define PERIPHERALS												"Peripherals"

#define VALVE_1														"Valve 1"
#define VALVE_2														"Valve 2"
#define VALVE_3														"Valve 3"

#define VALVE_7														"Valve 7"
#define VALVE_8														"Valve 8"

#define WARM_UP														"Warm-Up"

#define UPPER_LIMIT												"Upper Limit"
#define LOWER_LIMIT												"Lower Limit"

#define REMOTE														"Remote Control"
#define START															"Start"
#define STOP															"Stop"
/********HOME Screen*******/
#define LEG_1															"LEG A"
#define LEG_2															"LEG B"
#define LEG_3															"LEG C"
#define OUTSIDE_CONDITION									"oc"
#define RETURN														"Rte"
#define ANTFR															"Anfr"
#define LT																"LT :"
	
/******Parameters******/
#define OPMM_BATTERY_STATUS								"Battery Status"
#define BASE_MATERIAL_LIQUID_LEVEL				"Base Mat Level"
#define HARDNER_MATERIAL_LIQUID_LEVEL			"Hardner Mat Level"
#define HUMIDITY													"Humidity"

#define RETURN_LINE												"Return Line"
#define ANTIFREEZ_LIQUID									"Antifreez Liquid"
#define SCROLL_DOWN												"Scroll Down"
#define SCROLL_UP													"Scroll Up"
#define PIPE_POSITION3										"Pipe Position 3"
#define PIPE_POSITION6										"Pipe Position 6"
#define PIPE_POSITION9										"Pipe Position 9"
#define PIPE_POSITION12										"Pipe Position 12"
#define AVERAGE_PIPE_TEMP									"Avg Pipe Temp"
	
#define PRESSURE_GAUGE_1									"Pressure gauge 1"
#define PRESSURE_GAUGE_2									"Pressure gauge 2"
#define PRESSURE_GAUGE_3									"Pressure gauge 3"

#define PRIME_PROCESS_PARAM								"Prime Process"
#define SPRAY_PROCESS_PARAM								"Spray Process"
#define FLUSH_PROCESS_PARAM								"Flush Process"

#define	HMI_STRING												"HMI"
#define	SCU_STRING												"SCU"
#define	OPMM_STRING												"OPMM"
#define	VCM_STRING												"VCM"
#define	HCM_STRING												"HCM"

#define AIR_SENSOR												"Air Sensor"
#define CONNECTED													"Connected"
#define DISCONNECTED											"Disconnected"
/****Engineering Settings*****/
#define GAUGE1														"Gauge 1"
#define GAUGE2														"Gauge 2"
#define GAUGE3														"Gauge 3"
#define ANTIFREEZ													"Antifreez"	
#define SCROLL														"Scroll"
#define AREA															"(Area)"
#define BASE_BATCH_NUMBER									"Base Batch no."
#define HARDNER_BATCH_NUMBER							"Hardner Batch no."

#define TIME_BEFORE_STALL									"Time Before Stall"
#define TIME_BEFORE_COMPARING_STALL				"Time Bfr cmpr Stall"

#define TIME_START_STALL_TEST							"Start Stall Test Time"
#define TIME_SPRAY_ROTATION_OFF_TIME			"Spray Rotn Stop Time"

/*20thJuly2016*/
#define TIME_IDLE_PERIODIC_TIME					"Idle Periodic Time"
#define UTC_TIME_OFFSET										"UTC Time Offset"
#define JOB_NAME													"Job Name"
/*change7*//*change17*/
#define JOINT_ID														"Joint Id"

#define SYSTEM_DATE												"System Date"
#define UTC_DATE												  "UTC Date"
#define SYSTEM_TIME												"System Time"
#define UTC_TIME													"UTC Time"
#define DD																"(DD)"
#define MM																"(MM)"
#define YY																"(YY)"
#define HR																"(HR)"
#define MIN_TIME													"(Min)"
#define SEC																"(sec)"

#define SEC_WB														"sec"
#define MS_WB															"ms"
#define MM2																"mm2"
#define INCH2															"in2"
#define INCH															"in"

#define DCLS															"DCLS"
#define HLS																"HLS"
#define GUN_SWITICHING										"Gun Switching"
#define SPRAY_GUN_DELAY										"VBAT Spray delay Time"
#define SPRAYING_COUNT										"Spraying Count"
#define TYPE_OF_4TH_IHCM									"Type of 4th IHCM"
#define TYPE_OF_UNITS											"Type of Units"
#define METRIC_STRING											"Metric"
#define IMPERIAL_STRING										"Imperial"
#define JOINT_NUMBER											"Joint Number"	
#define DIAGNOSTIC												"Diagnostic"
#define CYCLE															"Cycle"
#define SPRAY_GUN													"Spray Gun"
#define VALVE_A														"Valve A"
#define VALVE_B														"Valve B"
#define RECIRC_HARD												"Recirc (Hard)"
#define RECIRC_BASE												"Recirc (Base)"
#define MOTOR_FORWARD											"Motor Forward"
#define MOTOR_BACKWARD										"Motor Backward"
#define SPRAY_HEAD												"Spray Head"
#define SOLVENT_PUMP											"Solvent Pump"
#define ANTIFREEZ_PUMP										"Antifreez Pump"
#define HARDNER_PUMP											"Hardner Pump"
#define PRESIDENT_PUMP										"President Pump"
#define	KING_PUMP													"King Pump"
#define SPARE_VALVE												"Spare Valve"

#define JOINTS														"Joints"
#define TOTAL_VOLUME											"Total vol."


#define PASSES_REQUIRED										"Passes Required"
#define PASSES_COMPLETED									"Passes Completed"
#define GUN_TIMER_DELAY										"Gun timer Delay"
#define OVER_RUN_DELAY										"Over run Delay"
#define MINIMUM_PURGE_TIME								"min Purge Time"
#define MINIMUM_FLUSH_TIME								"min Flush Time"

#define OPMM_SET_ON_PIPE_Q								"OPMM Set On Pipe?"
#define START_STRING											"Start"
#define STOP_STRING												"Stop"
#define GUN_POSITION											"Gun Position"

/*OP*/
#define NVSRAM														"NVSRAM"
#define NVSRAM1														"NVSRAM1"
#define NVSRAM2														"NVSRAM2"
#define JOYSTICK													"Joystick"
#define PUSH_ON_SWITCH										"Push Sw"

/*MCU*/
#define EEPROM														"EEPROM"
#define PRESSURE_REGULATOR								"Pres Reg"
#define THERMOCOUPLE_STRING								"Temp Snr"
#define	TEMP_HUMIDITY_SENSOR							"Temp-Humd"

/*OPMM*/
#define BACKWARD_VALVE_CONNECTED					"BW vl Cnt"
#define FORWARD_VALVE_CONNECTED						"FW vl Cnt"
#define FLUSH_VALVE_CONNECTED							"Flush Cnt"
#define SPRAY_VALVE_CONNECTED							"Spray Cnt"
#define SPRAY_HEAD_VALVE_CONNECTED				"Spry H Cnt"
#define BASE_MAT_VALVE_CONNECTED					"B Mat Cnt"
#define HARD_MAT_VALVE_CONNECTED					"H Mat Cnt"
#define SPARE_VALVE_CONNECTED							"Spare Cnt"

#define BACKWARD_VALVE_STATUS							"BW vl Sts"
#define FORWARD_VALVE_STATUS							"FW vl Sts"
#define FLUSH_VALVE_STATUS								"Flush Sts"
#define SPRAY_VALVE_STATUS								"Spray Sts"
#define SPRAY_HEAD_VALVE_STATUS						"Spry H Sts"
#define BASE_MAT_VALVE_STATUS							"B Mat Sts"
#define HARD_MAT_VALVE_STATUS							"H Mat Sts"
#define SPARE_VALVE_STATUS								"Spare Sts"

#define TEMP_AT_3													"Temp at 3"
#define TEMP_AT_6													"Temp at 6"
#define TEMP_AT_9													"Temp at 9"
#define TEMP_AT_12												"Temp at 12"

#define BINARY_SWITCH											"Bin SW"
#define LINEAR_TRANSDUCER									"Lin Trans"
#define VALVE1_CONNECTED									"Val1 Cnt"
#define VALVE2_CONNECTED									"Val2 Cnt"
#define VALVE3_CONNECTED									"Val3 Cnt"
#define VALVE4_CONNECTED									"Val4 Cnt"
#define VALVE5_CONNECTED									"Val5 Cnt"

#define VALVE1_STATUS											"Val1 Sts"
#define VALVE2_STATUS											"Val2 Sts"
#define VALVE3_STATUS											"Val3 Sts"
#define VALVE4_STATUS											"Val4 Sts"
#define VALVE5_STATUS											"Val5 Sts"

#define HEATER_STRING											"Heater"
#define LIQUID_LEVEL_SENSOR								"Liq Lvl Snr"

#define BASE_HYSTERESIS_POS								"Base Hysteresis +ve"
#define BASE_HYSTERESIS_NEG								"Base Hysteresis -ve"
#define HARD_HYSTERESIS_POS								"Hard Hysteresis +ve"
#define HARD_HYSTERESIS_NEG								"Hard Hysteresis -ve"
#define ANTIFRZ_HYSTERESIS_POS						"AntiFreez Hyst +ve"
#define ANTIFRZ_HYSTERESIS_NEG						"AntiFreez Hyst -ve"

#define RECIRC_PRESSURE_HYSTERESIS_POS		"Recirc Pres Hyst +ve"
#define RECIRC_PRESSURE_HYSTERESIS_NEG		"Recirc Pres Hyst -ve"
#define SPRAY_PRESSURE_HYSTERESIS_POS			"Spray Pres Hyst +ve"
#define SPRAY_PRESSURE_HYSTERESIS_NEG			"Spray Pres Hyst -ve"

#define BHCM_LIQUID_LEVEL_HTH							"Base Liq High TH"
#define BHCM_LIQUID_LEVEL_LTH							"Base Liq Low TH"

#define HHCM_LIQUID_LEVEL_HTH							"Hardner Liq High TH"
#define HHCM_LIQUID_LEVEL_LTH							"Hardner Liq Low TH"

/*4th March2016*/
#define UPSTROKE_LIMIT_STR								"Upstroke Limit"
#define UPSTROKE_MIN_STR									"Upstroke Min"
#define UPSTROKE_MAX_STR									"Upstroke Max"
#define DOWNSTROKE_LIMIT_STR							"Downstroke Limit"
#define DOWNSTROKE_MIN_STR								"Downstroke Min"
#define DOWNSTROKE_MAX_STR								"Downstroke Max"
	
/*11thMarch*/
#define FIRMWARE													"Firmware"
#define DETAILS														"Details"

/*13thJuly2016*/
#define ETHDISPLAY												"Ethernet"
#define WIFIDISPLAY												"Wi-Fi"
#define IP_DETAILS												"IP Details"
/*change3*/
#define IP																"IP"
#define SUBNET														"Subnet"
#define GATEWAY														"Gateway"
#define PORT															"Port"
#define SSID															"SSID"
#define PASSWORD													"Password"
/*****************FLASH MESSAGES************************/
#define	WAIT_FOR_GUN_POSITION							"Wait for Gun position"
#define GUN_AWAY_FROM_PIPE								"Gun Away From Pipe"
#define GUN_TOWARDS_PIPE									"Gun Towards Pipe"
#define PRIME_GOING_ON										"Prime in Progress"
#define PRIME_PROCESS_DONE								"Prime Completed"
/*#define PRIME_PROCESS_FAIL							"Prime Process Fail"*/

#define FLUSH_GOING_ON										"Flush in Progress"
#define FLUSH_PROCESS_DONE								"Flush Completed"
#define FLUSH_PROCESS_FAIL								"Flush Process Fail"

#define SPRAY_GOING_ON										"Spray in Progress"
#define SPRAY_PROCESS_DONE								"Spray Completed"
#define SPRAY_PROCESS_FAIL								"Spray Process Fail"

/*change16*/
#define SEQUENCE_ERROR										"Sequence Error."
#define GUN_POS_SW_TIME_ERROR							"Gun Pos Sw Time Error."
#define TIME_HLS_DCLS_ERROR								"Time HLS/DCLS  Error."
#define NO_LIMIT_SW_DETECTED							"No Limit Sw Detected."
#define ESTOP_PRESS												"ESTOP Press."
#define STOP_COMM_RX											"STOP Comm Rx."
#define FLUSH_COMM_RX											"FLUSH STOP Rx."
#define SYS_ERR_OCCUR											"Sys Err Occur"
/**********************/
#define FLUSH_NOW													"!!!Flush Now!!!"
#define STALL_TEST_IN_PROGRESS						"Stall Test In Progress"
/***************Status Messages***************/
#define UPSTROKE_STALL_FAIL_AT_A					"Upstroke Stall Fail At 'A'"
#define UPSTROKE_STALL_FAIL_AT_B					"Upstroke Stall Fail At 'B'"
#define UPSTROKE_STALL_FAIL_AT_C					"Upstroke Stall Fail At 'C'"
#define UPSTROKE_STALL_PASS_AT_A					"Upstroke Stall Pass At 'A'"
#define UPSTROKE_STALL_PASS_AT_B					"Upstroke Stall Pass At 'B'"
#define UPSTROKE_STALL_PASS_AT_C					"Upstroke Stall Pass At 'C'"

#define DOWNSTROKE_STALL_FAIL_AT_A				"Downstroke Stall Fail At 'A'"
#define DOWNSTROKE_STALL_FAIL_AT_B				"Downstroke Stall Fail At 'B'"
#define DOWNSTROKE_STALL_FAIL_AT_C				"Downstroke Stall Fail At 'C'"
#define DOWNSTROKE_STALL_PASS_AT_A				"Downstroke Stall Pass At 'A'"
#define DOWNSTROKE_STALL_PASS_AT_B				"Downstroke Stall Pass At 'B'"
#define DOWNSTROKE_STALL_PASS_AT_C				"Downstroke Stall Pass At 'C'"

#define SPRAY_PROCESS_PASS								"Spray Process Pass"
#define SPRAY_PROCESS_FAIL								"Spray Process Fail"

#define PRIME_PROCESS_PASS								"Prime Process Pass"
#define PRIME_PROCESS_FAIL								"Prime Process Fail"

#define FLUSH_PROCESS_PASS								"Flush Process Pass"
#define FLUSH_PROCESS_FAIL								"Flush Process Fail"

#define SYSTEM_READY_FOR_OPERATION				"System Ready For Operation"
#define OPERATION_MODE_OVER_STRING				"Operation Mode Over"

#define OPMM_SET_ON_PIPE									"OPMM Set on Pipe"
#define OPMM_NOT_SET_ON_PIPE							"OPMM Not Set on Pipe"

#define FLUSH_TIME_ACHIEVED_PASS					"Flush Time Achieved Pass"
#define FLUSH_TIME_ACHIEVED_FAIL					"Flush Time Achieved Fail"
#define PRIME_TIME_ACHIEVED_PASS					"Prime Time Achieved Pass"
#define PRIME_TIME_ACHIEVED_FAIL					"Prime Time Achieved Fail"

#define STALL_VALVE_ON										"Stall Valve ON"
#define STALL_VALVE_OFF										"Stall Valve OFF"

#define RECIRC_VALVE_ON										"Recirc Valve ON"
#define RECIRC_VALVE_OFF									"Recirc Valve OFF"

#define	PRESSURE_EQUAL_TO_RECIRC					"Recirculation Pressure Achieve"
#define	PRESSURE_EQUAL_TO_SPRAY						"Pressure Equal to Spray"

#define	SCU_DIAGNOSTIC_PASS								"SCU Diagnostic Pass"
#define	SCU_DIAGNOSTIC_FAIL								"SCU Diagnostic Fail"
#define	HMI_DIAGNOSTIC_PASS								"HMI Diagnostic Pass"
#define	HMI_DIAGNOSTIC_FAIL								"HMI Diagnostic Fail"
#define	OPMM_DIAGNOSTIC_PASS							"OPMM Diagnostic Pass"
#define	OPMM_DIAGNOSTIC_FAIL							"OPMM Diagnostic Fail"

#define	VCM_DIAGNOSTIC_PASS								"VCM Diagnostic Pass"
#define	VCM_DIAGNOSTIC_FAIL								"VCM Diagnostic Fail"
#define	HCM_DIAGNOSTIC_PASS								"HCM Diagnostic Pass"
#define	HCM_DIAGNOSTIC_FAIL								"HCM Diagnostic Fail"



#define OPMM_SPRAY_VALVE_ON								"Spray Valve ON"
#define OPMM_SPRAY_HEAD_VALVE_ON					"Spray Head Valve ON"
#define OPMM_AB_MAT_VALVE_ON							"AB material Valve ON"
#define OPMM_FLUSH_VALVE_ON								"Flush Valve ON"
#define OPMM_FORWARD_VALVE_ON							"Forward Rotation Valve ON"
#define OPMM_BACKWARD_VALVE_ON						"Backward Rotation Valve ON"

#define OPMM_SPRAY_VALVE_OFF							"Spray Valve OFF"
#define OPMM_SPRAY_HEAD_VALVE_OFF					"Spray Head Valve OFF"
#define OPMM_AB_MAT_VALVE_OFF							"AB material Valve OFF"
#define OPMM_FLUSH_VALVE_OFF							"Flush Valve OFF"
#define OPMM_FORWARD_VALVE_OFF						"Forward Rotation Valve OFF"
#define OPMM_BACKWARD_VALVE_OFF						"Backward Rotation Valve OFF"


#define HCM_BASE_HEATER_ON								"Base Heater ON"
#define HCM_HARD_HEATER_ON								"Hardner Heater ON"
#define HCM_ANTFRZ_HEATER_ON							"Antifreez Heater ON"
#define HCM_HARD_TANK_HEATER_ON						"Hard Tank Heater ON"

#define HCM_BASE_HEATER_OFF								"Base Heater OFF"
#define HCM_HARD_HEATER_OFF								"Hardner Heater OFF"
#define HCM_ANTFRZ_HEATER_OFF							"Antifreez Heater OFF"
#define HCM_HARD_TANK_HEATER_OFF					"Hard Tank Heater OFF"


#define ESTOP_PRESSED_STRING							"ESTOP Pressed!!!"
#define ESTOP_RELEASED_STRING							"ESTOP Released!!!"


#define LIMIT_SWITCH_1_PRESSED						"Limit switch 1 pressed"
#define LIMIT_SWITCH_2_PRESSED						"Limit switch 2 pressed"
#define LIMIT_SWITCH_3_PRESSED						"Limit switch 3 pressed"
#define LIMIT_SWITCH_4_PRESSED						"Limit switch 4 pressed"
/***************Alarm Messages for Faults***************/

#define OPMM_FORWARD_VALVE_FAULT					"Forward Val Fault"
#define OPMM_BACKWARD_VALVE_FAULT					"Backward Val Fault"
#define OPMM_FLUSH_VALVE_FAULT						"Flush Val Fault"
#define OPMM_SPRAY_VALVE_FAULT						"Spray Val Fault"
#define OPMM_SPRAY_HEAD_VALVE_FAULT				"Spray Head Rotation Val Fault"
#define OPMM_BASE_MAT_VALVE_FAULT					"AB Mat Val Fault"

#define VCM_STALL_VALVE_FAULT							"VCM Stall Valve Fault"
#define VCM_RECIRC_VALVE_FAULT							"VCM Recirc Valve Fault" /*change12*/

#define ANTIFREEZ_HIGH_TH_FAULT						"Antifrz Liq temp Above Th Fault"
#define ANTIFREEZ_LOW_TH_FAULT						"Antifrz Liq temp Below Th Fault"
#define INL_BASE_MAT_TEMP_ABOVE_TH				"Inl Base Mat temp Above Th"
#define INL_BASE_MAT_TEMP_BELOW_TH				"Inl Base Mat temp Below Th"
#define INL_HARD_MAT_TEMP_ABOVE_TH				"Inl Hard Mat temp Above Th"
#define INL_HARD_MAT_TEMP_BELOW_TH				"Inl Hard Mat temp Below Th"
#define RT_LINE_TEMP_ABOVE_TH							"Rt Line temp Above Th"
#define RT_LINE_TEMP_BELOW_TH							"Rt Line temp Below Th"

#define BHCM_BASE_MAT_LEVEL_ABOVE_TH			"Base Mat Lvl Above Th Fault"
#define BHCM_BASE_MAT_LEVEL_BELOW_TH			"Base Mat Lvl Below Th Fault"
#define HHCM_BASE_MAT_LEVEL_ABOVE_TH			"Hard Mat Lvl Above Th Fault"
#define HHCM_BASE_MAT_LEVEL_BELOW_TH			"Hard Mat Lvl Below Th Fault"

#define PRESSURE_GAUGE_1_ABOVE_TH					"Pressure Gauge 1 Above Th"
#define PRESSURE_GAUGE_1_BELOW_TH					"Pressure Gauge 1 Below Th"
#define PRESSURE_GAUGE_2_ABOVE_TH					"Pressure Gauge 2 Above Th"
#define PRESSURE_GAUGE_2_BELOW_TH					"Pressure Gauge 2 Below Th"
#define PRESSURE_GAUGE_3_ABOVE_TH					"Pressure Gauge 3 Above Th"
#define PRESSURE_GAUGE_3_BELOW_TH					"Pressure Gauge 3 Below Th"

#define OPMM_PIPE_TEMP_BEYOND_RANGE				"Pipe Temp Not In Range"

#define INL_BASE_HEATER_FAULT							"Inl Base Heater Fault"
#define INL_HARD_HEATER_FAULT							"Inl Hard Heater Fault"
#define ANTIFREEZ_HEATER_FAULT						"Antifreez Heater Fault"
#define HARD_TANK_HEATER_FAULT						"Hard Tank Heater Fault"

#define SPRAY_GUN_MOTION_FAULT						"Spray Gun Motion Error"

#define LINEAR_TRANSDUCER_OUT_OF_RANGE		"Linear Transd Out Of Range"

#define OPMM_DCLS_TIMER_FAULT							"OPMM DCLS Timer Fault"
#define OPMM_HLS_TIMER_FAULT							"OPMM HLS Timer Fault"

#define HB_ACK_NOT_RECEIVED_FROM_OP			  "HB Ack Not RX From OP"
#define HB_ACK_NOT_RECEIVED_FROM_OPMM			"HB Ack Not RX From OPMM"
#define HB_ACK_NOT_RECEIVED_FROM_VCM			"HB Ack Not RX From VCM"
#define HB_ACK_NOT_RECEIVED_FROM_HCM			"HB Ack Not RX From HCM"

#define OPMM_NOT_RECEIVED_HEARTBEAT				"OPMM Not Received Heartbeat"
#define MFM_NOT_RECEIVED_HEARTBEAT				"MFM Not Received Heartbeat"
#define VCM_NOT_RECEIVED_HEARTBEAT				"VCM Not Received Heartbeat"
#define HCM_NOT_RECEIVED_HEARTBEAT				"HCM Not Received Heartbeat"

#define OPMM_SPRAY_EXPECTED_LW3						"SPRAY-Expected LW3,Triggered LW4"
#define OPMM_SPRAY_EXPECTED_LW4						"SPRAY-Expected LW4,Triggered LW3"

/*change4*/
#define ACK_BUTTON    										"Ack"
#define MEMORY_ALARMS_1										"Memory Alarms 1"
#define MEMORY_ALARMS_2										"Memory Alarms 2"
#define MEMORY_ALARMS_3										"Memory Alarms 3"
#define MEMORY_ALARMS_4										"Memory Alarms 4"

#define DIAGNOSTIC_LOG_MEMORY_FULL				"Diagnostic Log Mem 80% Full"
#define STATUS_LOG_MEMORY_FULL						"Status Log Mem 80% Full"
#define FAULT_LOG_MEMORY_FULL							"Fault Log Mem 80% Full"
#define PERIODIC_LOG_MEMORY_FULL					"Periodic Log Mem 80% Full"
/*change19*/
#define PRIME_DETECTED										"Prime Detected"
#define FLUSH_DETECTED										"Flush Detected"
#define SPRAY_DETECTED										"Spray Detected"
#define READ_PIPE_TEMP_DETECTED						"Read Pipe temp Detected"
#define START_STOP_DETECTED								"Start/Stop Detected"
#define FLUSH_COMPLETE_DETECTED						"Flush Complete Detected"
/*********************************************************************************/
#define HARDNER_MATERIAL_TYPE							(uint8_t)0x01
#define BASE_MATERIAL_TYPE								(uint8_t)0x02
#define METRIC														(uint8_t)0x01
#define IMPERIAL													(uint8_t)0x02



/*********************SCREENS MACROS********************************/
#define SCREEN_HOME												(uint8_t)1
#define SCREEN_ENGINEERING								(uint8_t)2
#define SCREEN_PRESSURE_SETTING						(uint8_t)3
#define SCREEN_KEYPAD											(uint8_t)4
#define SCREEN_ALARMS_1										(uint8_t)5
#define SCREEN_PARAM											(uint8_t)6
#define SCREEN_NOTIFICATION_1							(uint8_t)7
#define SCREEN_ADVANCED_MAN_CYCLE					(uint8_t)8
#define SCREEN_VALVE_STATUS_2							(uint8_t)9
#define SCREEN_VALVE_STATUS_3							(uint8_t)10
#define SCREEN_OPERATION									(uint8_t)11
#define SCREEN_PRIME											(uint8_t)12
#define SCREEN_FLUSH											(uint8_t)13
#define SCREEN_SPRAY											(uint8_t)14
#define SCREEN_NOTIFICATION_2							(uint8_t)15
#define SCREEN_NOTIFICATION_3							(uint8_t)16
#define SCREEN_NOTIFICATION_4							(uint8_t)17
#define SCREEN_ALARMS_2										(uint8_t)18
#define SCREEN_ALARMS_3										(uint8_t)19
#define SCREEN_ALARMS_4										(uint8_t)20
#define SCREEN_CLEAR_FAULTS								(uint8_t)21
#define SCREEN_OTHER_SETTING_2						(uint8_t)22
#define SCREEN_OTHER_SETTING_3						(uint8_t)23
/*New Screen*/
#define SCREEN_ERROR_WARNING							(uint8_t)24
#define SCREEN_MANAGE_PERIPHERALS					(uint8_t)25
#define SCREEN_LIQUID_LEVEL_SETTING				(uint8_t)26
/**************PHASE_2***************/
#define SCREEN_MENU_1											(uint8_t)27
#define SCREEN_MENU_2											(uint8_t)28
#define SCREEN_STALL											(uint8_t)29
#define SCREEN_PARAMETER_SET_1						(uint8_t)30
#define SCREEN_LIQUID_LEVEL_1							(uint8_t)31
#define SCREEN_LIQUID_LEVEL_2							(uint8_t)32
#define SCREEN_TEMP_SETTING_1							(uint8_t)33
#define SCREEN_TEMP_SETTING_2							(uint8_t)34
#define SCREEN_TIMER_SETTING_1						(uint8_t)35
#define SCREEN_TIMER_SETTING_2						(uint8_t)36
#define SCREEN_TIMER_SETTING_3						(uint8_t)37
#define SCREEN_TIMER_SETTING_4						(uint8_t)38
#define SCREEN_PUMP_SETTING								(uint8_t)39
#define SCREEN_OTHER_SETTING							(uint8_t)40
#define SCREEN_OTHER_SETTING_1						(uint8_t)41
#define SCREEN_HYSTERESIS_1								(uint8_t)42
#define SCREEN_HYSTERESIS_2								(uint8_t)43
#define SCREEN_STALL_SETTING							(uint8_t)44
#define SCREEN_JOB_SETTING_1							(uint8_t)45
#define SCREEN_JOB_SETTING_2							(uint8_t)46
#define SCREEN_PRESSURE_SETTING_1					(uint8_t)47
#define SCREEN_PRESSURE_SETTING_2					(uint8_t)48
#define SCREEN_PARAMETER_SET_2						(uint8_t)49
#define SCREEN_CURRENT_PARA								(uint8_t)50
#define SCREEN_PARAM_TEMP_1								(uint8_t)51
#define SCREEN_PARAM_TEMP_2								(uint8_t)52
#define SCREEN_PARAM_TEMP_3								(uint8_t)53
#define SCREEN_PARAM_PRESSURE							(uint8_t)54
#define SCREEN_PARAM_TIME									(uint8_t)55
#define SCREEN_PARAM_OTHERS								(uint8_t)56
#define SCREEN_ADVANCED										(uint8_t)57
#define SCREEN_ADVANCED_AUTOCOAT					(uint8_t)58
#define SCREEN_ADVANCED_RESET							(uint8_t)59
#define SCREEN_MANAGE_PERIPHERAL_1				(uint8_t)60
#define SCREEN_MANAGE_PERIPHERAL_2				(uint8_t)61
#define SCREEN_STATUS											(uint8_t)62
#define SCREEN_MATERIAL_USAGE							(uint8_t)63
#define SCREEN_VALVE_STATUS_1							(uint8_t)64
#define SCREEN_MANAGE_PERIPHERAL_3				(uint8_t)65   /* change13*/
#define SCREEN_CYCLE_STATUS								(uint8_t)66
#define SCREEN_DIAGNOSTIC_STATUS					(uint8_t)67
#define SCREEN_HMI_DIAGNOSTIC							(uint8_t)68
#define SCREEN_SCU_DIAGNOSTIC							(uint8_t)69
#define SCREEN_OPMM_DIAGNOSTIC_1					(uint8_t)70
#define SCREEN_OPMM_DIAGNOSTIC_2					(uint8_t)71
#define SCREEN_MFM_DIAGNOSTIC							(uint8_t)72
#define SCREEN_VCM_DIAGNOSTIC							(uint8_t)73
#define SCREEN_HCM_DIAGNOSTIC							(uint8_t)74
#define SCREEN_HHCM_DIAGNOSTIC						(uint8_t)75
#define SCREEN_IHCM1_DIAGNOSTIC						(uint8_t)76
#define SCREEN_IHCM2_DIAGNOSTIC						(uint8_t)77
#define SCREEN_IHCM3_DIAGNOSTIC						(uint8_t)78
#define SCREEN_IHCM4_BM_DIAGNOSTIC				(uint8_t)79
#define SCREEN_IHCM4_HM_DIAGNOSTIC				(uint8_t)80
#define SCREEN_PERIPHERAL_SETTING					(uint8_t)81
#define SCREEN_VCM_PP_SETTING_1						(uint8_t)82
#define SCREEN_VCM_PP_SETTING_2						(uint8_t)83
#define SCREEN_HCM_PP_SETTING_1						(uint8_t)84
#define SCREEN_HCM_PP_SETTING_2						(uint8_t)85
#define SCREEN_OPMM_PP_SETTING_1					(uint8_t)86
#define SCREEN_OPMM_PP_SETTING_2					(uint8_t)87
#define SCREEN_OPMM_PP_SETTING_3					(uint8_t)88
#define SCREEN_SCU_PP_SETTING							(uint8_t)89
	/*March2016*/
#define	SCREEN_REMOTE_REPLICA							(uint8_t)90
#define SCREEN_STALL_SETTING_1						(uint8_t)91
#define SCREEN_FIRMWARE_DETAILS						(uint8_t)92
/*13thJuly2016*/
/*change3*/
#define SCREEN_ETH_IP_DETAILS							(uint8_t)93
#define SCREEN_WIFI_IP_DETAILS						(uint8_t)94

#define SCREEN_MEMORY_ALARM_1							(uint8_t)95
#define SCREEN_MEMORY_ALARM_2							(uint8_t)96
#define SCREEN_MEMORY_ALARM_3							(uint8_t)97
#define SCREEN_MEMORY_ALARM_4							(uint8_t)98

/*****************************************************/

#define OP_STATUS													(uint8_t)1
#define MCU_STATUS												(uint8_t)2
#define OPMM_STATUS												(uint8_t)3
#define MFM_STATUS												(uint8_t)4
#define VCM_STATUS												(uint8_t)5
#define BHCM_STATUS												(uint8_t)6
#define HHCM_STATUS												(uint8_t)7
#define IHCM1_STATUS											(uint8_t)8
#define IHCM2_STATUS											(uint8_t)9
#define IHCM3_STATUS											(uint8_t)10
#define IHCM4_BM_STATUS										(uint8_t)11
#define IHCM4_HM_STATUS										(uint8_t)12

#define HMI_FAULT													(uint8_t)1
#define SCU_FAULT													(uint8_t)2
#define OPMM_FAULT												(uint8_t)3
#define VCM_FAULT													(uint8_t)4
#define HCM_FAULT													(uint8_t)6

/***************************************************/

#define SCREEN_PATTERN_1									(uint8_t)1		/*HOME*/
#define SCREEN_PATTERN_2									(uint8_t)2		/*MENU*/
#define SCREEN_PATTERN_3									(uint8_t)3		/*CONFIG*/
#define SCREEN_PATTERN_4									(uint8_t)4		/*Stall test Screen*/
#define SCREEN_PATTERN_5									(uint8_t)5		/*peripheral remap and bypass*/
#define SCREEN_PATTERN_6									(uint8_t)6		/*Alarm & Warning screen pattern*/
#define SCREEN_PATTERN_7									(uint8_t)7		/*Clear faults screen*/



#define SCREEN_PATTERN_8									(uint8_t)8
#define SCREEN_PATTERN_9									(uint8_t)9
#define SCREEN_PATTERN_10									(uint8_t)10
#define SCREEN_PATTERN_11									(uint8_t)11
#define SCREEN_PATTERN_12									(uint8_t)12
#define SCREEN_PATTERN_13									(uint8_t)13
#define SCREEN_PATTERN_14									(uint8_t)14
#define SCREEN_PATTERN_15									(uint8_t)15
#define SCREEN_PATTERN_16									(uint8_t)16
#define SCREEN_PATTERN_17									(uint8_t)17
#define SCREEN_PATTERN_18									(uint8_t)18
#define SCREEN_PATTERN_19									(uint8_t)19
#define SCREEN_PATTERN_20									(uint8_t)20

#define DEFAULT								(uint8_t)0x03
/* ----------------------------------------------------------------------------
*                           CONSTANTS
* ----------------------------------------------------------------------------
*/
/* ----------------------------------------------------------------------------
*                           GLOBAL VARIABLES
* ----------------------------------------------------------------------------
*/
typedef enum {FALSE = 0, TRUE = !FALSE} bool;
typedef struct fsymbol_hdr
{
	uint8_t ucWidth;
	uint8_t ucHeight;
}FSYMBOL_HDR;

typedef struct fsymbol
{
	const FSYMBOL_HDR symhdr;
	const uint8_t bitdata[MAX_FONT_HEIGHT * MAX_FONT_WIDTH_BYTES];
}FSYMBOL;

typedef struct font
{
	uint8_t ucName[32];
	uint32_t uliMax_Width;
	uint32_t uliHeight;
	uint8_t ucCode_Start_Offset;
	const FSYMBOL *symtable;
}FONT;

/* ----------------------------------------------------------------------------
*                           EXTERNAL VARIABLES
* ----------------------------------------------------------------------------
*/

extern FONT font_franklin20;
/*extern FONT font_franklin22;
extern FONT font_franklin16;
*/
extern FONT font_franklin24;
/*
extern FONT font_franklin28;
extern FONT font_franklin36;
extern FONT font_times13;

extern FONT font_times16;
extern FONT font_times20;
extern FONT font_times24;
extern FONT font_times28;
*/
extern uint32_t uliDisplayHeight;
extern uint32_t uliDisplayWidth;
extern uint32_t uliCurrentOrientation;
extern uint16_t uiCurrentScreen;
extern uint16_t uiCursorXPos;
extern uint16_t uiCursorYPos;
extern uint16_t uiCursorXPosKP;
extern uint16_t uiCursorYPosKP;
extern uint8_t ucSwitchPressFlag ;
extern uint8_t ucScreenPatternNumber;
extern uint8_t ucPatternNumberCount;
extern uint8_t ucCurrentScreenNumber;
extern uint16_t uirgStoreCurrentScreen[100];
extern uint8_t ucrgStoreCurrentScreenPattern[100];
extern uint8_t ucStatusMessageCounter;

extern uint8_t ucrgCheck[];
extern uint8_t ucrgUncheck[];

/* ----------------------------------------------------------------------------
*                           EXTERNAL FUNCTIONS
* ----------------------------------------------------------------------------
*/
extern void fnGR_SCREEN1(uint8_t ucscreen_no);
extern void fnGR_GPIO_Configuration(void);
extern void fnGR_Memory_Configuration(void);
extern void fnGr_Initialize(void);
extern void fnGr_FillRectangle(uint32_t ,uint32_t ,uint32_t ,uint32_t ,uint32_t);
extern void fnGrVertical_Line(uint32_t ,uint32_t ,uint32_t ,uint32_t);
extern void fnGrHorizontal_Line(uint32_t ,uint32_t ,uint32_t ,uint32_t);
extern void fnGr_Rectangle(uint32_t ,uint32_t ,uint32_t ,uint32_t ,uint32_t);
extern void fnGr_ThickRectangle(uint32_t ,uint32_t ,uint32_t ,uint32_t ,uint32_t ,uint32_t);
extern bool fnGr_SelectFont(uint8_t*, uint32_t );
extern bool fnGr_PutString(uint32_t ,uint32_t ,uint8_t* ,uint32_t ,uint32_t ,uint32_t,uint8_t);
extern uint32_t fnGr_GetStringWidth(uint8_t *);
extern uint32_t fnGr_GetFontHeight(void);
extern void fnDisplay_Initialize(void);
extern void fnDisplay_Clear_Screen(uint32_t);
extern void fnDisplay_Set_Orientation(uint32_t);
extern void fnDisplay_Get_Geometry(uint32_t*, uint32_t* ,uint32_t*);
extern void fnDisplay_RAM_Address_Oriented_to_Physical(uint32_t* ,uint32_t*);
extern void fnDisplay_RAM_Address_Physical_to_Oriented(uint32_t* ,uint32_t*);
extern void fnDisplay_Set_RAM_Address(uint32_t ,uint32_t);
extern void fnDisplay_Data_Write(uint16_t);
extern bool fnGr_DoesFontExist(uint8_t*);
extern void fnDisplay_Index_Reg_Set(uint16_t);
extern void fnDisplay_Reg_Write(uint16_t usiaddr ,uint16_t);
extern void fnDisplay_Set_All_Registers(void);
extern void fnDelayMsec(uint32_t);

extern void fnLCDbmp(uint32_t ,uint32_t ,uint32_t ,uint32_t ,uint8_t*);
extern void fnLCDDrawPixel(uint32_t , uint32_t , uint16_t );
extern void fnGr_BuzzerVolume(void);
/* ----------------------------------------------------------------------------
*                           FUNCTIONS DECLARATION
* ----------------------------------------------------------------------------
*/


/*****Strings On Screen*****/
/*HOME*/
#define BASE_TEMP										"Base Temp"
#define HARDNER_TEMP								"Hardner Temp"
#define AMBIENT_OC									"Ambient OC"
#define BASE_RTE_TEMP								"Base Rte Temp"
#define ANTIFREEZ_TEMP							"Antifreez Temp"
#define DEW_POINT										"Dew point"

/*MENU*/
#define MENU												"MENU"
#define NOTIFICATIONS								"Notification"
#define STALL_TEST									"Stall Test"
#define PERIPHERAL									"Peripheral"
#define PARAMETERS									"Parameters"

/*PARAMETER SETTING-1*/
#define PARAMETER_SETTING						"Parameter Setting"
#define LIQUID_LEVEL								"Liquid Level"
#define HYSTERESIS									"Hysteresis"

/*Liquid level setting*/
#define BASE_TANK_HEIGHT						"Base Tank height"
#define HARDNER_TANK_HEIGHT					"Hardner tank height"
#define HARDNER_LIQ_HIGH_TH		 			"Hardner Liq High TH"
#define HARDNER_LIQ_LOW_TH					"Hardner Liq Low TH"
#define BASE_LIQ_LEV_HIGH_TH				"Base Liq Lev High TH"
#define BASE_LIQ_LEV_LOW_TH					"Base Liq Lev Low TH"
#define LIQ_LV_SAMPLING_RATE				"Liq lv Sampling Rate"

/*Temperature Setting*/
#define TEMPERATURE									"Temperature"
#define ANTIFREEZ_LIQ								"Antifreez Liq"
#define BASE_MATERIAL_STRING				"Base Material"
#define HARD_MATERIAL_STRING				"Hard Material"
#define PIPE_TEMP										"Pipe Temp"

/*Timers Setting*/
#define TIMERS											"Timers"
#define PRIME_TIME									"Prime Time"
#define FLUSH_TIME									"Flush Time"
#define GUN_DELAY_TIME							"Gun Delay Time"
#define REVERSE_DELAY_TIME					"Reverse Delay Time"
#define SPRAY_OVERLAP_TIME					"Spray Overlap Time"
#define DATA_LOGGING_PERIOD					"Data Logging Period"
#define DCLS_TIME										"DCLS Time"
#define HLS_TIME										"HLS Time"
#define GUN_SWITCHING_TIME					"Gun Switching Time"

/*Pump Setting*/
#define PUMP_SETTING								"Pump Setting"
#define CYLINDER_COUNT							"Cylinder Count"

/*others setting*/
#define OTHERS											"Others"
#define CURRENT_KP_NO								"Current KP"  /*change17*/
#define SPRAYING_COUNT							"Spraying Count"
#define TYPE_OF_UNIT								"Type Of Unit"

#define SIGNATURE_VALUE							"Signature Value"
#define BATTERY_KNEE_VOLTAGE				"Battery Knee Voltage"
/*Job Setting*/
#define JOB_SETTING									"Job Setting"
#define PIPE_DIAMETER								"Pipe Diameter"
/*Stall Setting*/
#define STALL_SETTING								"Stall Setting"
#define STALL_THRESHOLD_LOW					"Stall Threshold%(low)"

/*Pressure Setting*/
#define PRESSURE_SETTING						"Pressure Setting"
#define GAUGE1_LOW_TH								"Gauge 1(Low Th)"
#define GAUGE2_LOW_TH								"Gauge 2(Low Th)"
#define GAUGE3_LOW_TH								"Gauge 3(Low Th)"
#define HIGH_TH_STRING							"(High Th)"
#define SPRAY												"Spray"
#define RECIRCULATION								"Recirculation"

/*parameter setting 2*/
#define JOB_SETTING									"Job Setting" 
#define STALL_SETTINGS							"Stall Setting"
#define STATUS											"Status"

/*Status screen*/
#define MATERIAL_USAGE							"Material Usage"
#define VALVE_STATUS								"Valve Status"
#define CYCLE_STATUS								"Cycle Status"

/*Material usage*/
#define CURRENT_JOINT(ml)								"Curr Joint(ml)"
#define CURENT_JOINT(oz)								"Curr Joint"
/*valve status screen*/
#define HEAD_TO_PRIME_SPRAY					"Head to Prime/Spray"
#define A_B_VALVE										"A/B Valve"
#define FORWARD_VALVE								"Forward Valve"
#define BACKWARD_VALVE							"Backward Valve"
#define SPRAY_VALVE									"Spray Valve"
#define SOLVENT_VALVE								"Solvent Valve"

#define GUN_POSITION								"Gun Position"
#define HEAD_TO_PRIME								"Head to Prime"


#define STALL_VALVE									"Stall Valve"
#define RECIRC_VALVE								"Recirc Valve"
#define PROCESS_TIME								"Process Time"

/*Advanced screen*/
#define ADVANCED										"Advanced"
#define AUTOCOAT_STATUS							"Autocoat Status"

/*Manage peripherals*/
#define MANAGE_PERIPHERAL						"Manage Peripheral"


/*SSR's*/
#define	BASE_INL_HEATER							"Base Inl Heater"
#define	HARD_INL_HEATER							"Hard Inl Heater"
#define	BASE_HEATER									"Base Heater"
#define	HARDNER_HEATER							"Hardner Heater"

/*Thermocouples*/
#define	BASE_INL_TC									"Base Inl TC"
#define	HARD_INL_TC									"Hard Inl TC"
#define	BASE_TC											"Base TC"
#define	RETURN_LINE_TC							"Return Line TC"


/*OPMM NCTS*/
#define NCTS_1												"NCTS 1"
#define NCTS_2												"NCTS 2"
#define NCTS_3												"NCTS 3"
#define NCTS_4												"NCTS 4"

/**/
#define LASER_POINTER									"Laser Pointer"
#define BUZZER_VOLUME									"Buzzer Volume"
#define DIAGNOSTIC_STATUS							"Diagnostic Status"



#include <string.h>
//#define SW_1_HIGH	mPORTDSetBits(0x0100)	
//#define SW_2_HIGH	mPORTDSetBits(0x0200)
#define SW_3_HIGH	mPORTBSetBits(0x0020)
#define SW_1_DETECT PORTEbits.RE8
#define SW_2_DETECT PORTEbits.RE9
#define SW_3_DETECT PORTBbits.RB5

#define MIN_LITRE 0.1

#define FIRST_FRAME_DELAY   	1500
#define SECOND_FRAME_DELAY  	2000
#define KEY_DEBOUNCE_DELAY		40		//40 ms
#define UP_DETECT		1
#define DOWN_DETECT		2
#define ENTER_DETECT	3
#define MENU_DETECT		4
#define NO_DETECT		5		    //1.0.1


// -----------------    Menu option S ------------------------------   //
#define FRANCHISEE_START_POS    ADD_BALANCE_MENU           //1  //1.0.1
#define FRANCHISEE_LAST_POS     EXIT_FRANCHISEE_MENU      //13  //1.0.1  //v3.1.1.D
#define ADMIN_START_POS         OUTFLOW_CALIBRATION_MENU  //14  //1.0.1  //v3.1.1.D
#define ADMIN_LAST_POS          EXIT_ADMIN_MENU           //17  //1.0.1  //v3.1.1.D

#define ADD_BALANCE_MENU             1   //3.1.2
#define ADD_CARD_MENU                2
#define CARD_RECH_BAL_MENU           3
#define REMOVE_CARD_MENU             4
#define DAILY_DISP_MENU              5
#define DUMP_MODE_MENU               6
#define REFILL_MODE_MENU             7
#define SCHOOL_MODE_MENU             8
#define CORPORATE_MODE_MENU          9
#define REQ_MAINTANANCE_MENU         10
#define DEVICE_INFO_MENU             11
#define CALIBRATION_MENU             12
#define FLOW_ERROR_REMOVE_MENU       13
#define EXIT_FRANCHISEE_MENU         14
#define OUTFLOW_CALIBRATION_MENU     15
#define TDS_CALIBRATION_MENU         16
#define RTC_MANUAL_SET_MENU          17
#define EXIT_ADMIN_MENU              18 
// -----------------    Menu option E ------------------------------   //

#define LED1_ON		mPORTFSetBits(0x0001)
#define LED1_OFF	mPORTFClearBits(0x0001)
#define LED2_ON 	mPORTFSetBits(0x0002)
#define LED2_OFF	mPORTFClearBits(0x0002)
#define LED4_ON 	mPORTGSetBits(0x0001)
#define LED4_OFF 	mPORTGClearBits(0x0001)
#define LED3_ON 	mPORTGSetBits(0x0002)
#define LED3_OFF 	mPORTGClearBits(0x0002)

#define TANK_LOW_1   1
#define TANK_LOW_2   2
#define TANK_HIGH_1  3
#define TANK_HIGH_2  4
//#define TANK_LOW     5
//#define TANK_HIGH    6

// sync_type =1 by_modem 2= time_delay 3= rtc halt
#define BY_MODEM                 1
#define BY_TIME_DELAY            2 
#define BY_RTC_HALT              3
#define BY_EEPROM_TIME           4
#define BY_MANUALLY_SET_TIME     5

#define SEARCH_PEN               1
#define SEARCH_ERR               2

#define DISPLAY_SET              0

#define FILE_FIND_RETURN_FLAG    1
#define FILE_FIND_DELETE_REMAKE  2

#define COIN_BOX_ENABLE          1
#define COIN_BOX_DISABLE         2 
#define RECEIVE_COIN             3 
#define COIN_BOX_STATUS          4 
#define COIN_CHANEL_ENBL         5 

#define FILE_SEND_EN  1
#define FILE_SEND_DIS  0

#define DATE_CHANGE_FLAG_ON  1
#define DATE_CHANGE_FLAG_OFF 0


#define MAX_DATA_LENGTH 			1024		//115
#define MAX_SMS_LENGTH				200
#define MAX_SERVER_STRING 			128
#define AVG_READING_TDS				10		//20
#define AVG_READING_TEMP 			3      //10  v3.1.1.F
#define AVG_READING_SV_CURRENT 		5
#define AVG_READING_BATTERY			10.0
#define FIRMWARE_VERSION 			"3.1.C" //07/04/17
#define TOTAL_ERRORS 16
#define SENSOR_DETECT_TIME	1		//trip identify if sensor status stable for x(SENSOR_DETECT_TIME) sec

#define MAX_RX_BUFFER_LIMIT 	256
#define MAX_TX_BUFFER_LIMIT 	256
#define MAX_TEMPERATURE		1250  	//78	//(1600*0.0625 = 100)  //v3.1.1.F
#define TEMPERATURE_OFFSET  10

#define IDEAL_COIN_TIME 		5		//v2.2.9  //1.0.2
#define COIN_AMOUNT_OFFSET 	10		//v2.2.9     //1.0.2

//#define USER_BALANCE_FACTOR 50		//v3.0.0
#define MAX_BALANCE_LIMIT		999		//v3.0.0
#define DISPENSE_LEAST_COUNT   0.1

//---Major Mode
#define IDLE_MODE		            0
#define ERROR_MODE  	            1
#define CONSUMER_MODE	            2
#define REFILL_MODE		            3
#define DUMP_MODE		            4
#define DEBUG_MODE		            5
#define DIAGNOSTIC_MODE	            6
#define SCHOOL_MODE		            7
#define ANTENNA_MODE    			8
#define SLEEP_MODE					9
#define GSM_MODE					10
#define FRANCHISEE_MODE				11  //v2.2.5
#define CORPORATE_MODE  			12  //v2.2.5
#define ULTRA_SUPER_ADMIN_MODE 		13 //1.0.0
#define CALIBRATION_MODE            14  //v3.1.1.D

#define MAX_OPMODE                  14 //v3.1.1.F


//---Sub-Mode
#define CON_AUTH_PROG	           1
#define CON_AUTH_OK		           2
#define CON_DISPENSE	           3
#define CON_HALT		           4
#define CON_COMPLETE	           5

#define DUMP_AUTH_PROG	           1
#define DUMP_AUTH_OK	           2
#define DUMP_DISPENSE	           3
#define DUMP_COMPLETE	           5

#define REFILL_WAIT		           0
#define REFILL_REFILLING	       1
#define REFILL_COMPLETE	           2

#define ST_IDLE_MSG			    	1
#define ST_CARD_DETECTED			2
#define ST_BUTTON_PRESSED	    	3
#define ST_PAG_ERROR		    	4
#define ST_SERVER_CHECK	     		5
#define ST_LOW_BATTERY	    		6
#define ST_SMS_CHECK	       		7
#define ST_PP_INTERRUPT	     		8	
#define ST_WATER_LEAKAGE			9	
#define ST_UNAUTH_WATER_INPUT		10	
#define ST_DEBUG_STATE	    		11
#define ST_DIAGNOSTIC_STATE 		12
#define ST_TDS_CHECK	      		13
#define ST_ERROR_CHECK	    		14
#define ST_OP_MODE		    		15
#define ST_MENU			    		16
#define ST_ULTRA_SUPER_ADMIN_MODE 	17
#define ST_DATE_CHANGE 	            18
#define ST_ANALOG_SENSOR_UPDATE     19
#define ST_RTC_TIME_CHECK           20
#define ST_SCHOOL_MODE              21
#define ST_MEMORY_CARD_STATE        22              //v3.1.1.H
#define ST_COIN_FUNCTION_DETECTED   23


#define ST_MAX				    	24		//v2.3.2 18 to 22//1.0.1  (22 to 23//v3.1.1.H  ) //(23 to 24//v3.1.1.H  )

//---RFID Card Type
#define INVALID_CARD	            0
#define CONSUMER_CARD	            1
#define FRANCHISEE_CARD	            2
#define UNAUTHORISED_CARD	        3
#define LOW_BAL_CARD                4
#define NOT_PROPER_SCAN             5
#define REFILL_CARD	      	        7
#define DUMP_CARD		            8

//---Franchisee Options---> v2.2.5


//---Server Responses Type
#define SERVER_ACK_OK	        	1
#define SERVER_RESPONSE_OK		    2
#define SERVER_DATA_ERROR		    3

//----Server connection state
#define SERVER_CONN_OK    	        1
#define SERVER_DOWN		            2 
#define NETWORK_DOWN	            3
//---#define GPRS_DOWN	4

//#define BUTTON_HALT	 1
//#define LONG_PRESS                  1  //v3.1.1.F
//#define DOUBLE_CLICK                2  //v3.1.1.F
//#define SHORT_PRESS	              3  //v3.1.1.F

//----------Demo 2.0 Board
//#define DISPENSE_SW	PORTDbits.RD0
#define PAG_ER_PIN	0		//PORTEbits.RE2
#define LLS_PIN_CHECK	PORTCbits.RC2  //v3.1.1.F
#define HLS_PIN_CHECK	PORTCbits.RC1

#define MODEM_ON mPORTDClearBits(0x4000)
#define MODEM_OFF mPORTDSetBits(0x4000)

#define SV_VALVE_ON {mPORTASetBits(0x0002);peripheral_flags.sv_on=TRUE;}
#define SV_VALVE_OFF {mPORTAClearBits(0x0002);peripheral_flags.sv_on=FALSE;}

#define RFID_RESET_ON	mPORTDClearBits(0x2000)
#define RFID_RESET_OFF	mPORTDSetBits(0x2000)

#define CHILLER_HEATER_ON  mPORTCSetBits(0x4000);               //v3.1.3
#define CHILLER_HEATER_OFF mPORTCClearBits(0x4000)             //v3.1.3

#define HIGH                 1
#define LOW                  0
#define INVALID              0

//------Buzzer state
#define OFF	                 0
#define ERR01	             1
#define ERR02	             2 
#define AUT01	             3
#define AUT02	             4
#define AUT03	             5
#define DISP	             6
#define HALT	             7

//-------Error States
//-system_flags.system_error(4bits)
#define ERROR_FREE			0
//#define LOW_BATT_ER		1
#define WATER_LEAK_ER		2
#define HIGH_TDS_ER			3
#define PAG_ERROR			4
#define LLS_TRIG_ER			5
#define NETWORK_FAIL_ER		6
#define SERVER_FAIL_ER		7
#define MODEM_HANG_ER		8
//---#define SIM_FAIL_ER	9
#define TANK_EMPTY_ER		9	//v2.2.2
#define SV_ER				10
#define FLOW_SENS_ER		11
#define UV_FAIL_ER			12
#define LOW_TDS_ER			13
//#define HLS_TRIG_ER		14
#define TEMP_SENSOR_FAIL_ER	14
#define DEV_DISABLED		15

//------Network State
//---network_state
#define NOT_REGISTERED	'0'
#define REGISTERED		'1'
#define NETWORK_SEARCH	'2'
#define REG_DENIED		'3'
#define UNKNOWN			'4'
#define REG_ROAMING		'5'
#define INITIAL			0xFF

//---------------------- Server Response Messages -------------------
#define DEV_TXN_LIMIT_LOW_1                1
#define DEV_TXN_LIMIT_HIGH_1               3
#define DEV_TXN_LIMIT_LOW_2                121
#define DEV_TXN_LIMIT_HIGH_2               130

#define CONFIG_PARAMETER_LIMIT_LOW_1       4             
#define CONFIG_PARAMETER_LIMIT_HIGH_1      69
#define CONFIG_PARAMETER_LIMIT_LOW_2       131             
#define CONFIG_PARAMETER_LIMIT_HIGH_2      150
    
#define READ_SETTINGS_LIMIT_LOW_1          70
#define READ_SETTINGS_LIMIT_HIGH_1         75
#define READ_SETTINGS_LIMIT_LOW_2          151
#define READ_SETTINGS_LIMIT_HIGH_2         160

#define MISC_MSG_LIMIT_LOW_1               76       
#define MISC_MSG_LIMIT_HIGH_1              79
#define MISC_MSG_LIMIT_LOW_2               161       
#define MISC_MSG_LIMIT_HIGH_2              170
  
#define DATA_ALERT_MSG_LIMIT_LOW_1          80   //v3.1.2.G     //ALERT_MSG_LIMIT_LOW_1
#define DATA_ALERT_MSG_LIMIT_HIGH_1         98   //v3.1.2.G     //ALERT_MSG_LIMIT_HIGH_1
#define DATA_ALERT_MSG_LIMIT_LOW_2          171  //v3.1.2.G     //ALERT_MSG_LIMIT_LOW_2
#define DATA_ALERT_MSG_LIMIT_HIGH_2         180  //v3.1.2.G     //ALERT_MSG_LIMIT_HIGH_2

#define ONLY_ALERT_MSG_LIMIT_LOW_1         99
#define ONLY_ALERT_MSG_LIMIT_HIGH_1        109
#define ONLY_ALERT_MSG_LIMIT_LOW_2         181
#define ONLY_ALERT_MSG_LIMIT_HIGH_2        190

//--------------------           Transaction Msg         -------------------------------------------  //
#define DEV_AUTHENTICATE			   	1       //RV01
#define DEV_AUTHENTICATE_W_PARA			2       //RV02
#define DEV_BALANCE_DEDUCT				3       //RB03
#define DEV_USER_ADD                	121     //RV03
#define DEV_USER_REMOVE             	122     //RV04
#define DEV_BAL_ADD					    123     //RB04   
//--------------------           Configure Parameters   -----------------------------------------------------------  //	
#define DEV_HB_MSG						4       //RA05  
#define DEV_USER_TIMEOUT				5       //RA01  
#define DEV_DISP_TIMEOUT				6       //RA02  
#define DEV_DUMP_TIMEOUT				7       //RA03
#define DEV_REFILL_TIMEOUT				8       //RA04
#define DEV_CLEANING_TIMEOUT			9       //RA22
#define DEV_SCHOOL_DISPENSE_AMT			10      //RA06
    #define DEV_ER_DISPLAY_TIMER			11      //RA07 
#define DEV_DISPENSE_LT_LC				12      //RA08
#define DEV_LOGOUT_IDLE_TIMEOUT			13      //RA09
#define DEV_LT_EMPTY_TANK				14      //RA10
#define DEV_OUT_FLOW_CALIB				15      //RA11
#define DEV_IN_FLOW_CALIB				16      //RA12
#define DEV_TDS_CALIB					17      //RA13
#define DEV_DEVICE_EN_DIS				18	    //RA14
#define DEV_TDS_EN_DIS					19      //RA15
#define DEV_LLS_EN_DIS					20      //RA16
#define DEV_HLS_EN_DIS					21      //RA17
#define DEV_TDS_MIN						22      //RA18
#define DEV_TDS_MAX						23      //RA19
#define DEV_NEW_PASSWORD_SET			24      //RA20
//#define DEV_LOW_BATT_ALERT_LEVEL 		25      //RA21
#define DEV_LEAK_TIMER					26      //RA30
#define DEV_DEVICE_IC_CHANGED			27      //RA23
#define DEV_DEBUG_MODE					28      //RA24
#define DEV_N_W_SIGNAL_STRENGTH			29      //RA25
#define DEV_SOFT_RESET					30      //RA26
#define DEV_BUZZER_CTRL					31      //RA27
//#define DEV_BATT_CHARGE_STATE			32      //RA28
#define DEV_DIAGNOSTIC_TEST				33      //RA29
//#define DEV_CRITICAL_LOW_BATT_LVL		34      //RA31
#define DEV_SCHOOL_MODE					35      //RA33
#define DEV_DFLT_PARA					36      //RA34
#define DEV_SMS_PERMIT					37      //RA32
//#define DEV_FULL40_VALUE				38      //RA35
#define DEV_SCHOOL_MODE_ACC_ID			39      //RA36
#define DEV_MASTER_CONT_NO				40      //RA37
#define DEV_DATA_ER_AUTO_CORRECT	    41      //RA38
#define DEV_HYST_COUNT				    42      //RA39
//#define DEV_DS_COMMAND				43      //RA40
#define DEV_ANTENNA_MODE				44      //RA41    
//#define DEV_SV_CURRENT_ERR_COUNT		45      //RA42
#define DEV_GSM_MODE					46      //RA43
#define DEV_TEMPERATURE_CALIB			47      //RA44
#define DEV_WATER_TEMPERATURE_LEVEL		48      //RA45  //v2.3.6        //DEV_COLD_TEMPERATURE_LEVEL
#define DEV_TEMPERATURE_EN_DIS			49      //RA46
#define DEV_OUT_FLOW_FREQ_LIMIT			50      //RA47
#define DEV_IN_FLOW_FREQ_LIMIT			51      //RA48
#define DEV_SLEEP_MODE_MINUTES			52      //RA49
#define DEV_AUTO_REFILL_FLAG			53      //RA50
#define DEV_FRANCHISEE_RFID             54      //RA51  //v2.2.5
#define DEV_FRANCHISEE_RFID_VERIFIED    55      //RA52  //v2.2.5
#define DEV_CORPORATE_MODE              56      //RA53  //v2.2.5
#define DEV_CORPORATE_MODE_ACC_ID       57      //RA54  //v2.2.5
#define DEV_CHILLER_HEATER_EN_DIS       58      //RA55 	//v2.2.6  //v2.3.6
#define DEV_TDS_VERSION					59      //RA56	//v2.2.9
#define DEV_TEMP_COMPEN_EN_DIS			60      //RA57	//v2.2.9

#define DEV_COIN_EN_DIS				    61      //RA58	//v2.2.9
#define DEV_COIN_CHN_STATUS		    	62      //RA59	//v2.2.9
#define DEV_TOTAL_COIN_AMOUNT		    63      //RA60	//v2.2.9
#define DEV_TOTAL_COIN_COUNT	    	64      //RA61	//v2.2.9
#define DEV_COIN_WATER_TARIFF		    65      //RA62	//v2.2.9
#define DEV_COIN_COLLECT_CAPACITY   	66      //RA63	//v2.2.9
#define DEV_COIN_CHN_EN_DIS		    	67      //RA64	//v2.2.9
#define DEV_COIN_ID				        68      //RA65	//v2.2.9
#define DEV_RESET_COIN_AMOUNT		    69      //RA66	//v2.2.9

#define DEV_TIME_SYNC_BY_MODEM    		131     //RA67
#define DEV_OFFLINE_WATER_TARIFF    	132     //RA68
#define DEV_MENU_PASSWORD_SET           133     //RA69
#define DEV_FORMAT_SD_CARD          	134     //RA70
#define DEV_FETCH_RTC_TIME          	135     //RA71 //v3.1.1.D
#define DEV_AUTO_SYNC_EN_DIS        	136     //RA72 //v3.1.1.F

//--------------------           Read Settings   -----------------------------------------------------------  //
#define DEV_SYSTEM_SETTINGS_1			70      //RR01
#define DEV_SYSTEM_SETTINGS_2			71      //RR02
#define DEV_SYSTEM_SETTINGS_3			72      //RR03
#define DEV_SYSTEM_SETTINGS_4			73      //RR04
#define DEV_SYSTEM_SETTINGS_5			74      //RR05
#define DEV_SYSTEM_SETTINGS_6			75      //RR06

//--------------------           Misc Msg   ---------------------------------------------------------------------  //
#define DEV_MODEM_COMMAND				76      //RM01
#define DEV_SMS_RX						77      //RM02
#define DEV_GENERAL_COMMAND				78      //RM03
#define DEV_VALIDATION_ERROR			79      //RM04

//--------------------           data alert msg     -------------------------------------------------------------//
#define DEV_TDS_HIGH					80       //RL01
#define DEV_TDS_LOW						81       //RL02
#define DEV_UNAUTHORISED_WATER_INPUT	82       //RL03
#define DEV_UNAUTHORISED_WATER_DISPENSE 83       //RL04
#define DEV_WRONG_PASSWORD  			84       //RL05
#define DEV_DUMP_COMPLETE   			85       //RL06
#define DEV_COMMAND_RX					86       //RL07
#define DEV_REFILL_TANK					87       //RL08 
//#define DEV_LOW_BATT					88       //RL09
#define DEV_CONNECTED					89       //RL17
#define DEV_SLEEP_MODE					90       //RL20
#define DEV_AGE_IN_MIN					91       //RL21
#define DEV_CURRENT_MODE				92       //RL22
#define DEV_GSM_ACTIVATED				93       //RL25
#define DEV_TEMP_SENSOR_FAILURE			94       //RL26
#define DEV_FRANCHISEE_MAINTENANCE_RQ	95       //RL27
#define DEV_TRAP_ERROR					96       //RL28
#define DEV_COIN_COLLECTOR_ERROR		97       //RL29
#define DEV_COIN_COMMUNICATION_ERROR 	98       //RL30  
#define DEV_DAILY_WATER_DISPENSE		171      //RL31
#define DEV_SERVER_SYNC					172      //RL32	//v3.0.0  114 ERROR TYPE2 missing
#define DEV_ERROR_TYPE2                 173      //RL33  //1.0.1
#define DEV_ULTRA_SUPER_ADMIN_MODE  	174      //RL34  //v3.1.1.F
#define DEV_SEND_ER_FILE_NAME           175      //RL35  //v3.1.1.F
#define DEV_FAULT_ERROR                 176      //RL36  //v3.1.2.H 

//--------------------           only alert msg---   -----------------------------------------------------------  //
#define DEV_DUMP_START			    	99        //RL10
#define DEV_LLS_TRIGGER			    	100       //RL11
#define DEV_HLS_TRIGGER			    	101       //RL12
#define DEV_FLOW_SENSOR_ERROR		    102       //RL13
#define DEV_SOLENOID_VALVE_ERROR 	    103       //RL14
#define DEV_PAG_ERROR				    104       //RL15
#define DEV_TANK_EMPTY				    105       //RL16
#define DEV_SOLAR_DETECTED			    106       //RL18
#define DEV_AC_MAINS_DETECTED		    107       //RL19
#define DEV_SERVER_LOST				    108       //RL23
#define DEV_NETWORK_LOST			    109       //RL24

#define DEV_SENT_VIA_SMS		     	200

//--------------------           Server Response Type -------------------
#define TXN_MSG		                	1
#define CONFIG_MSG		                2
#define SETTING_MSG		                3
#define MISC_MSG	                    4
#define DATA_ALERT_MSG	                5
#define ALERT_MSG		                6

//--------------------           SMS Codes ---------------------------
#define SER_SIGNATURE_BYTE		        0
#define SER_TDS_CALIB			        1
#define SER_OUT_FLOW_CALIB		        2
#define SER_IN_FLOW_CALIB		        3
#define SER_SERVER_IP			        4
#define SER_SERVER_PORT			        5
#define SER_SERVER_APN              	6
#define SER_TDS_MAX		          		7
#define SER_TDS_MIN			        	8
#define SER_USER_TIMEOUT	         	9
#define SER_DISP_TIMEOUT	        	10
//#define SER_LLS_DETECT_STATE	        11
#define SER_SLEEP_MODE_FLAG		        11	//v2.2.4
#define SER_DEVICE_EN_DIS	        	12
#define SER_TDS_EN_DIS		        	13
#define SER_LLS_HLS_EN_DIS	        	14
#define SER_MASS_DISP_EN_DIS	        15
#define SER_SCHOOL_DISPENSE_AMT	        16
#define SER_ER_DISPLAY_TIMER	        17
#define SER_DISPENSE_LT_LC	        	18
#define SER_NEW_PASSWORD_SET	        19
#define SER_LOGOUT_IDLE_TIMEOUT	        20
#define SER_LT_EMPTY_TANK		        21
#define SER_WATER_EXPIRED		        22
#define SER_FACT_PASSWORD_RESET	        23
#define SER_RESET_OOO 			        24
#define SER_REMOTE_DUMP			        25
#define SER_RQ_SYSTEM_SETTINGS	        26
#define SER_IDLE_MSG_FREQ		        27
#define SER_RECONNECTION		        28
#define SER_RESET_ALL_OOO	         	29
#define SER_DUMP_TIMEOUT		        30
#define SER_REFILL_TIMEOUT		        31
#define SER_CLEANING_TIMEOUT		    32
//#define SER_BATT_LEVEL			 	33
//#define SER_LOW_BATT_ALERT_LEVEL  	34
#define SER_CLEAN_TANK		     		35
#define SER_MODEM_COMMAND	    		36
#define SER_GENERAL_COMMAND	     		37
#define SER_EEPROM_READ		     		38 

#define SER_DEBUG_MODE		     		39
#define SER_CHANGE_DEV_ID	     		40
#define SER_LEAK_TIMER		     		41
#define SER_BUZZER_CTRL		     		42
#define SER_DEV_SOFT_RESET		    	43
#define SER_NETWORK_SIGNAL_STRENGTH     44
//#define SER_BATTERY_CHARGE_STATE   	45
#define SER_DIAGNOSTIC_TEST		    	46
//#define SER_CRITICAL_LOW_BATT_LVL	    47
#define SER_SEND_PARAMETER_VIA_SMS	    48
#define SER_SMS_PERMIT			     	49
#define SER_DEL_ALL_SMS			    	50
#define SER_CURRENT_MODE	     		51
#define SER_SCHOOL_MODE				    52
//#define SER_FULL40_VALUE		    	53
#define SER_AGE_IN_MIN			    	54
#define SER_DS_INIT				    	55
#define SER_MASTER_CONT_NO		    	56
#define SER_DATA_ER_AUTO_CORRECT    	57
#define SER_HYST_COUNT			     	58
#define SER_DS_COMMAND				    59
#define SER_ANTENNA_MODE			    60
//#define SER_SV_CURRENT_ERR_COUNT	    61  

#define SER_TEMPERATURE_CALIB		    63
#define SER_WATER_TEMPERATURE_LEVEL	    64  //SER_COLD_TEMPERATURE_LEVEL //v2.3.6
#define SER_TEMPERATURE_EN_DIS		    65
#define SER_OUT_FLOW_FREQ_LIMIT		    66
#define SER_IN_FLOW_FREQ_LIMIT		    67
#define SER_SLEEP_MODE_MINUTES		    68
#define SER_AUTO_REFILL_FLAG		    69
#define SER_FRANCHISEE_RFID        	    70   //v2.2.5
#define SER_CORPORATE_MODE         	    71   //V2.2.5
#define SER_CORPORATE_MODE_ACC_ID  	    72   //V2.2.5
#define SER_CHILLER_HEATER_EN_DIS 	    73   //V2.2.6   //v2.3.6

#define SER_COIN_EN_DIS				    74	//v2.2.9
#define SER_COIN_CHN_STATUS			    75	//v2.2.9
#define SER_TOTAL_COIN_AMOUNT		    76	//v2.2.9
#define SER_TOTAL_COIN_COUNT		    77	//v2.2.9
#define SER_COIN_WATER_TARIFF		    78	//v2.2.9
#define SER_COIN_COLLECT_CAPACITY	    79	//v2.2.9
#define SER_COIN_CHN_EN_DIS			    80	//v2.2.9
#define SER_COIN_ID					    81	//v2.2.9
#define SER_COIN_ID_STATUS			    82	//v2.2.9
#define SER_RESET_COIN_AMOUNT_COUNT     83	//v2.2.9  //1.0.2

#define SER_REQ_DAILY_DISPENSE		    84	//v3.0.0   No eeprom  No data_validation
#define SER_SERVER_SYNC			    	85	//v3.0.0   No eeprom  No data_validation
#define SER_TIME_SYNC_BY_MODEM          86  //         No eeprom  No data_validation
#define SER_OFFLINE_WATER_TARIFF        87  //         offline_water_tariff 
#define SER_MENU_PASSWORD_SET           88  //         menu_franchisee_password
#define SER_SYNC_DATE				    89  //         No eeprom but data_validation required
//#define SER_USER_ADD                  90  //1.0.1
#define SER_COIN_DISABLE_BY_FS_ERROR    90 //v6
#define SER_TDS_VERSION				    91	//v2.2.9 'RM91' Super Admin sms RM
#define SER_TEMP_COMPEN_EN_DIS 		    92	//v2.2.9

#define SER_AUTO_SYNC_EN_DIS		    93  //v3.1.1.F   system_flags.auto_sync_flag 
#define SER_SYNC_ERR_FILE               94  //v3.1.1.F   No eeprom  No data_validation
#define SER_SYNC_FILE_UPDATE            95  //v3.1.1.F   No eeprom  No data_validation
#define SER_MODEM_CHECK_ROUTINE         96  //v3.1.2.J   

#define SER_ERASE_SIGNATURE			 	99     //write default paramter
#define SER_OP_MODE					 	100   //
#define SER_ERROR_STACK				    101
#define SER_SCHOOL_MODE_ACC_ID		 	102
#define SER_FLOW_OFFSET				 	103
#define SER_T_SMS_READ				 	104
#define SER_SV_ON_I					 	105
#define SER_SV_OFF_I			     	106
#define SER_ER_STATE				    107
#define SER_TANK_LOW_LEVEL_STATUS	    108
#define SER_COIN_COLLECTOR_ERROR	    109  //1.0.2 //v3.1.1.F
//#define SER_SERVER_LIVE_CHECK_EN_DIS	110	//v2.3.5	2303
//#define SER_CONNECTION_CHECK_FREQ		111	//v2.3.5.a

#define SER_CARD_INIT_BAL            	112    //v3.1.1.F
#define SER_PREV_DATE				 	113    //v3.1.1.F    
#define SER_PREV_MONTH				 	114    //v3.1.1.F
#define SER_PREV_YEAR				 	115    //v3.1.1.F
#define SER_ULTRA_SUPER_ADMIN_MODE   	116    //v3.1.1.F

#define SER_FETCH_RTC_TIME              151  //v3.1.1.F   No eeprom  No data_validation
#define SER_FORMAT_SD_CARD              150  //v3.1.1.F   No eeprom  No data_validation

#define SER_TOTAL_PARAMETER             116
#define size_sd_read  		    		830   // 830
//--------------------           EEPROM Defines ---------------------
#define EE_RD_BIT		1
#define EE_WR_BIT		0

#define EEPROM_TOTAL_PARAMETERS		    98   //v2.2.9 TDS Header Version //3.1.0

#define EEPROM_READ		                2
#define EEPROM_WRITE	                3
#define EEPROM_DATABASE_CHECK	        4
#define DATA_ER_LIMIT
#define DATA_ER_EEPROM_READ_LIMIT
#define DATA_ER_DEFAULT_RESTORED
#define NO_DATA_ERROR

//--------------------           EEPROM Para Address ---------------------
#define EE_TDS_CALIB 				    0x00  //0 //0x00		//2 bytes___________
#define EE_OUT_FLOW_CALIB               0x03  //3 //0x04//0x02		//2 bytes___________
#define EE_IN_FLOW_CALIB                0x06  //6 //0x08//0x04		//2 bytes___________
#define EE_SERVER_IP                    0x09  //9 //0x0C//20 bytes__________
#define EE_SERVER_PORT                  0x1E  //30  //0x22//0x1A		//5 bytes___________
#define EE_SERVER_APN                   0x24  //36  //0x29//0x1F		//25 bytes__________ //v3.2.0.A
#define EE_TDS_MAX                      0x3E  //62     //0x3F//0x33		//2 bytes___________
#define EE_TDS_MIN                      0x41  //65     //0x43//0x35		//2 bytes___________
#define EE_USER_TIMEOUT 				0x44  //68     //0x47//0x37		//2 bytes___________
#define EE_DISP_TIMEOUT 				0x47  //71	  //0x4B//0x39		//2 bytes___________
//#define EE_LLS_DETECT                 0x4F  		//0x3B		//1 bytes___________
#define EE_SLEEP_MODE_FLAG              0x4F  //79	//0x4F				//1 bytes___________
#define EE_DEVICE_EN_DIS                0x52  //82 //0x52 //0x3C		//1 bytes___________
#define EE_TDS_EN_DIS                   0x55  //85  //0x55 //0x3D		//1 bytes___________
#define EE_LLS_HLS_EN_DIS               0x58  //88  //0x58 //0x3E		//1 bytes___________
#define EE_MASS_DISP_EN_DIS             0x5B  //91  //0x5B//0x3F 		//1 bytes___________
#define EE_SCHOOL_DISPENSE_AMT          0x5E  //94 // 0x5E  //0x40		//2 bytes___________
#define EE_ER_DISPLAY_TIMER             0x62      //87   0x62 //	//0x42  //2 bytes___________
#define EE_DISPENSE_LT_LC               0x66  //90		//0x44	  	//2 bytes___________
#define EE_NEW_PASSWORD_SET             0x6A		//0x46  	//5 bytes___________
#define EE_LOGOUT_IDLE_TIMEOUT          0x71		//0x4B  	//2 bytes___________
#define EE_LT_EMPTY_TANK                0x75		//0x4D  	//2 bytes___________
#define EE_WATER_EXPIRED                0x79		//0x4F 	  	//1 bytes___________
#define EE_FACT_PASSWORD_RESET          0x7C		//0x50  	//5 bytes___________
#define EE_IDLE_MSG_FREQ                0x83		//0x55 	    //1 bytes___________
#define EE_DUMP_TIMEOUT                 0x86		//0x57 	    //2 bytes___________
#define EE_REFILL_TIMEOUT               0x8A		//0x59 	    //2 bytes___________
#define EE_CLEANING_TIMEOUT             0x8E		//0x5B 	    //2 bytes___________
//------#define EE_CONFIG_LOW_BAT_ALRT 		0x92		//0x5D 	    //1 bytes___________
#define EE_CONFIG_LOW_BAT_ALRT          0x93		//0x5D 	    //1 bytes___________	//v2.2.5
#define EE_DEBUG_MODE                   0x95		//0x5E 	    //1 bytes___________
#define EE_CHANGE_DEVICE_ID             0x98		//0x5F		//11 bytes__________
#define EE_LEAK_DETECT_TIMER            0xA5		//0x6A 	    //2 bytes___________
#define EE_BUZZER_CONTROL               0xA9		//0x6C 	    //1 byte____________
#define EE_RUN_DIAG_TEST                0xAC		//0x6D 	    //1 byte____________
//#define EE_CRITICAL_LOW_BATT_LVL  	0xAF		//0x6E		//1 byte____________
#define EE_SMS_PERMISSION               0xB2		//0x6F		//1 byte____________
#define EE_SCHOOL_MODE                  0xB5		//0x70		//1 byte____________
//#define EE_FULL40_VAL               	0xB8		//0x71		//1 byte
#define EE_SCHOOL_MODE_ACC_ID           0xBB		//0x72		//13 bytes
#define EE_ERROR_STATE                  0xCA		//0x7E		//17 bytes
#define EE_OP_MODE                      0xDD		//0x8E		//1 Byte
#define EE_MASTER_CONT_NO               0xE0					//14 Bytes
#define EE_DATA_ER_AUTO_CORRECT         0xF0					//1 Byte
#define EE_HYST_COUNT                   0xF2		//0xF3		//2 Bytes
#define EE_SIGNATURE_BYTE               0xF5 	  //  //8 bytes
#define EE_SV_CURRENT_ERR_COUNT         0x101		//2 Bytes
#define EE_TEMPERATURE_CALIB            0x105		//2 Bytes
#define EE_WATER_TEMPERATURE_LEVEL   	0x109		//1 Byte   // EE_COLD_TEMPERATURE_LEVEL  //v2.3.6
#define EE_TEMPERATURE_EN_DIS           0x10C		//1 Byte
#define EE_TANK_LOW_LEVEL_STATUS        0x10F		//2 Byte
#define EE_IN_FLOW_FREQ_LIMIT           0x113		//2 Byte
#define EE_OUT_FLOW_FREQ_LIMIT          0x117		//2 Byte
#define EE_AUTO_REFILL_FLAG             0x11B		//1 Byte
#define EE_FRANCHISEE_RFID              0X11E		//13 Bytes  //v2.2.5
#define EE_CORPORATE_MODE               0x12C       //1 bytes  //v2.2.5
#define EE_CORPORATE_MODE_ACC_ID        0x12F       //13 byte    //v2.2.5
#define EE_CHILLER_HEATER_EN_DIS        0x13E       //1 byte 	//v2.2.6
#define EE_TDS_VERSION                  0x141		//1 byte 	v2.2.9
#define EE_TEMP_COMPEN_EN_DIS   		0x144		//1 byte 	v2.2.9


#define EE_COIN_EN_DIS          		0x147		//1 Byte	//v2.2.9
#define EE_COIN_CHN_EN_DIS          	0x14A		//1 Byte	//v2.2.9
#define EE_COIN_ID_CHN1         		0x150		//12 Bytes	//v2.2.9 12+2 + 2 Extra to maintin formet 
#define EE_COIN_ID_CHN2         		0x160		//12 Bytes	//v2.2.9
#define EE_COIN_ID_CHN3             	0x170		//12 Bytes	//v2.2.9
#define EE_COIN_ID_CHN4                	0x180		//12 Bytes	//v2.2.9
#define EE_COIN_ID_CHN5             	0x190		//12 Bytes	//v2.2.9
#define EE_COIN_ID_CHN6             	0x1A0		//12 Bytes	//v2.2.9
#define EE_COIN_ID_CHN7                 0x1B0		//12 Bytes	//v2.2.9
#define EE_COIN_ID_CHN8                 0x1C0		//12 Bytes	//v2.2.9		//177-185
#define EE_COIN_WATER_TARIFF            0x1D0		//2 Bytes	//v2.2.9
#define EE_TOTAL_COIN_AMOUNT            0x1D4		//2 Bytes	//v2.2.9
#define EE_TOTAL_COIN_COUNT             0x1D8		//2 Bytes	//v2.2.9
#define EE_COIN_COLLECT_CAPACITY        0x1DC		//2 Bytes	//v2.2.9
#define EE_COIN_COLLECTOR_ERROR         0x1E0		//1 BYte	//v2.2.9
//address #define EE_SERVER_LIVE_CHECK_EN_DIS		0x1E3	//1 Byte  //1.0.2

#define EE_PREV_DATE			        0x1E3		//1 Byte	//3.1.0
#define EE_PREV_MONTH			       	0x1E6		//1 byte   //3.1.0
#define EE_PREV_YEAR			       	0x1E9		//1 byte   //3.1.0
#define EE_ULTRA_SUPER_ADMIN_MODE	   	0x1EC		//1 byte   //3.1.0
#define EE_OFFLINE_WATER_TARIFF			0x1F0     // 2byte
#define EE_MENU_PASSWORD_SET            0x1F4      // 5 byte
#define EE_AUTO_SYNC_EN_DIS             0x1FA      // 1 byte    //3.1.1 
#define EE_CARD_INIT_BAL            	0x1FD      // 2 byteS    //3.1.1
#define EE_COIN_DISABLE_BY_FS_ERROR     0X200      // 1 byte
#define EE_MODEM_CHECK_ROUTINE          0x203      //1 byte  //v3.1.2.J

//#define EE_SV_CURRENT_ERR_COUNT		0xF5					//2 Bytes
//#define EE_SIGNATURE_BYTE 			0xF8 	    //8 bytes


//--------------------- EEPROM Function Prototypes ----------------------
void io_mapping(void);
void hardware_init(void);
void display_error_mode(void);
void check_system_stability(unsigned char);
void system_error_check(void);
void variable_init(void);
void peripheral_handler(void);
void mcu_reset_source(void);
void unauthorized_water_input_check(void);
void pag_error_check(void);
unsigned char verify_card_id(unsigned char rfid_card_check[]);
unsigned char data_validation(unsigned int variable);
void unknown_access_alert(void);


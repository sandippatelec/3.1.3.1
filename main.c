#define USE_AND_OR 	/* To enable AND_OR mask setting */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include<math.h>
#include "mcu_timer.h"
#include "ext_lcd.h"
#include "main.h"
//#include "variable.h"
#include "FSIO.h"
#include "p24fxxxx.h"
#include "PPS.h"
#include "uart.h"
#include<ports.h>
#include "PwrMgnt.h"
#include "wdt.h"
#include "1wire_temperature.h"
#include "serial_uart.h"
#include "FSconfig.h"
#include "DS1307.h"
#include "mcu_adc.h"
#include "math.h"
#include "coin_uca2.h"  //1.0.2  

#include "serial_uart.h"
#include "ext_eeprom.h"
#include "mcu_adc.h"
#include "peripherial.h"
#include "mode.h"
#include "modem.h"
#include "communicate.h"
#include "YHY502.h"
#include "server.h"


#if defined (__PIC24FJ128GA010__) || defined (__PIC24FJ256DA210__)
#elif defined(__PIC24FJ256GB110__)
_CONFIG1(WDTPS_PS8192 & FWPSA_PR128 & FWDTEN_ON & ICS_PGx2 & JTAGEN_OFF & GCP_ON); // Watch Dog and Code Protection enabled
_CONFIG2(IESO_ON & FNOSC_PRI & FCKSM_CSECME & POSCMOD_HS & PLLDIV_DIV4 & IOL1WAY_OFF)
_CONFIG3(WPFP_WPFP511 & WPDIS_WPDIS & WPCFG_WPCFGDIS & WPEND_WPSTARTMEM); //do_finalize  //set finally WPDIS_WPEN
#endif

//#define OLD_BOARD
//#define DEVICE_INIT
//----------------Global Master Variable-----------------//
signed char MACHINE_ID[11] = {"1111111111"}; //signed char MACHINE_ID[11]={"1000000005"};//
unsigned char master_contact[15] = {"918128252501"};//v3.2.0.B
RESET_SOURCE reset_source;
const char signature[9] = {"SARVAJAL"};
char *ptr_1, *ptr_2;

//----------- Modem and Server -----------------//
//volatile unsigned char Txdata[MAX_TX_BUFFER_LIMIT]={0};
unsigned int TOTAL_SMS_READ = 5;
volatile unsigned int sms_scan_no = 1;
volatile unsigned char Tx_Buffer_Head = 0, Tx_Buffer_Tail = 0;
volatile unsigned char Rx_Buffer_Head = 0, Rx_Buffer_Tail = 0;
volatile unsigned char modem_reset_count = 0;
volatile unsigned char modem_reset_tolerance = 0;
volatile char sms_string[160] = {0};
volatile char Rxdata[MAX_RX_BUFFER_LIMIT] = {0};
volatile char sms_buffer[MAX_SMS_LENGTH];
volatile char modem_cmd_response[40] = {0};
volatile unsigned char network_service_provider[25] = {0}; //v3.2.0.A //15 to 25
volatile unsigned char sms_sender_no[15] = {0};   //v3.2.0.A
volatile unsigned char sms_txn[5] = {0};
volatile unsigned char network_signal_strength[3] = {0};
volatile char factory_password[5] = "WA20"; //"WA20" //1.0.1  "2016" //v3.1.1.C
volatile char current_password[5] = "1234";
volatile char menu_current_password[5] = "0000";
volatile char menu_franchisee_password[5] = "1234";

//-----------         Server        -----------------//
volatile char temp_data[16] = {0}; // change in memory mapping of variable due rfid card getting null value //v2.2.6
unsigned char SERVER_RESENT_COUNT = 3;
volatile unsigned int rfid_to_server_msg = 0;
volatile unsigned char server_send_message_type = 0;
volatile unsigned char network_state = 0, server_reconnect_count = 0;
volatile char transmitData[MAX_DATA_LENGTH] = {0};
volatile char server_response_string[MAX_SERVER_STRING] = {0};
volatile unsigned char local_ip[20] = {0};
volatile unsigned char server_ip[20] = {0}; //"115.112.184.207";	//"122.170.012.160";
volatile unsigned char server_command[5] = {0};
volatile unsigned char server_port[6] = {0}; //"9091";	//"9092";
volatile unsigned char apn[25] = {0}; //"airtelgprs.com"; //"www"; //v3.2.0  20 to 25
volatile char txn_type[5] = {0};
volatile char msg_data[15] = {0};

volatile unsigned int random_no = 0;
volatile char random_no_array[6] = {0};
unsigned int idle_msg_freq = 60,modem_status_check_min=240; //3.1.2 //3.1.2.J //(60*modem_status_check_hour = 240 min (i.e modem_status_check_hour=1 ) )
volatile unsigned char mcu_auto_reset_counter = 0,modem_status_check_hour =1;

//----------------------FLOW SENSOR      -----------------//
volatile unsigned int output_flow_count = 0, input_flow_count = 0;
volatile unsigned int output_flow_tick = 0, input_flow_tick = 0; //v2.2.3
volatile unsigned int output_flow_freq_limit = 10, input_flow_freq_limit = 10; //v2.2.3

//volatile float DISPENSE_LEAST_COUNT = 0.1;  //v3.1.1.E
volatile float balance_deduct = 0, max_dispense_litre = 20.0, unauth_dispense_litre = 0, unauth_input_litre = 0;
volatile float dispense_litre = 0, total_dispense_litre = 0, refill_litre = 0, acc_dispense_capacity = 0;
volatile char disp_buffer[10] = {0};

//--volatile unsigned int RAAC_val,RSAC_val,FULL_val,AE_val,SE_val;  
volatile unsigned char RARC_val = 0; //,RSRC_val=0,STATUS_reg=0; //ask

volatile char acc_credit_array[10] = "00000.000"; //---volatile unsigned int acc_credit=0;//---,dummy_acc_credit=0;
volatile char previous_bal_array[10] = {0};


//----------- TDS and adc  -----------------//
unsigned char TDS_OFFSET = 10;
volatile double adc_volt = 0;
volatile unsigned char analog_sense = 0; //TDS_CHANNEL
volatile unsigned int tds_array[AVG_READING_TDS] = {0}, avg_tds_count = 0;
volatile unsigned int tds_reading = 0;
volatile unsigned int min_tds = 20, max_tds = 150, tds_calib_fact = 100;
volatile unsigned char adc_reading_counter = 0;
volatile char tds_value[5] = "0000";
volatile char tds_header_version = 'C'; //v2.2.9 //ask
volatile unsigned char tds_difference_exceed = 0;

//******Temp. Comp. Testing***********//	//v2.2.9
volatile float tds_at_25 = 0, error_per_degree = 0, total_error_correction = 0, temperature_difference = 0;
volatile float temperature_error_factor = 2.1;
//************************************//

//----------- Temperature  -----------------//
//volatile unsigned long temperature_array[AVG_READING_TEMP]={0},avg_temp_count=0; //1.0.1
volatile unsigned long avg_temp_count = 0;
volatile unsigned int min_temp = 10, max_temp = 40, temp_calib_fact = 100;
volatile unsigned char water_temperature_level = 20, temperature_reading_counter = 0, temperature_failure_count = 0, temperature_success_count = 0; //v3.1.1.F //temp_reading_counter v3.1.1.F
volatile float temperature_reading = 0;
volatile char temp_value[6] = "00000";
volatile double temp_adc_volt = 0;
volatile unsigned char DS_temperature_array[3] = {0};
volatile unsigned int DS_sensor_reading = 0, avg_temperature_array[AVG_READING_TEMP] = {0}; //v3.1.1.F
float DS_temperature = 0.0;
////--------------------------    battery        ------------------------------//
volatile unsigned char error_stack[TOTAL_ERRORS] = {0};
volatile char error_state[5] = "0000";
//--------------------------    timer       ------------------------------//
volatile unsigned int timer2_tick = 0, sec_tick = 0; //
volatile unsigned long int min_tick = 0, battery_verify_time_stamp = 0, sleep_time_stamp = 0, timer1_tick = 0, chiller_heater_time_stamp = 0; //v2.2.6  //v3.1.3
volatile unsigned int button_time_stamp = 0, out_leak_time_stamp = 0, in_leak_time_stamp = 0, button_tick = 0;  // flow_count_timestamp = 0; //,diagnosis_time_stamp=0; //v3.1.2.G
volatile unsigned int lls_time_stamp = 0, hls_time_stamp = 0, pag_err_time_stamp = 0, led_time_stamp = 0, solar_time_stamp = 0; //v2.2.6
volatile unsigned int active_awake_time_stamp = 0;
volatile unsigned int sch_time_stamp = 0, refill_time_stamp = 0; //v2.2.6
volatile unsigned int button_timeout = 0, display_toggle_time_stamp = 0;

volatile unsigned int user_timeout = 30, dispense_timeout = 15, leak_timeout = 10;
volatile unsigned int dump_timeout = 20, refill_timeout = 45, cleaning_timeout = 20;
volatile unsigned int error_display_time = 3, logout_idle_time = 5, lit_empty_tank = 50, tank_low_level_litre = 50;
volatile unsigned int out_flow_calibration = 250, in_flow_calibration = 250; //250 change to 20
volatile unsigned int led_board_timeout = 300;
volatile unsigned char unknown_access_count = 0; //v2.2.2
volatile unsigned int sleep_mode_timeout = 0; //v2.2.4

volatile float flow_offset = 0.1/*in_flow_calibration=250.0*/;

//--volatile unsigned char fresh_eeprom=0;
volatile unsigned long templong = 0;
volatile unsigned char signature_byte[8] = {0};
//--------------------------    LCD        ------------------------------//
unsigned char lcd_data_reg = 0;
unsigned char lcd_rs_pin = 0, dispense_button_count = 0;
char lcd_data[17] = {""};
//---char lcd_data_freeze[10][16]={""};	//10 line storage for LCD data
char lcd_line1_data_freeze[17] = {""};
char lcd_line2_data_freeze[17] = {""};

volatile unsigned char device_reset_state = 0; //,byte_variable=0;		//,int_variable=0,lcd_freeze_counter=0;
volatile unsigned int byte_variable = 0; //v2.2.9
volatile unsigned char rfid_card_type = 0, system_error = 0, litre_transcation = 0, function_state = 0;
volatile unsigned int unauth_count = 0, previous_adc_channel = 0;
volatile unsigned char dispense_trail = 0; //no_dispense_error_count //v6
volatile float temp_unauth_input = 0.0;

volatile int school_disp_amount = 500;
volatile unsigned char school_mode_en = 0, sch_mode_acc_id[9] = {0}; // = "NULL_SCHL_ACC";		// Latch byte for school mode
volatile unsigned char sch_disp_send_server = 0;

volatile unsigned char corporate_mode_en = 0, corp_mode_acc_id[9] = {0}; // = "NULL_CORP_ACC";		// Latch byte for corporate mode // v2.2.5
volatile unsigned char server_failure_counter = 0, network_failure_counter = 0, validation_error_counter = 0, new_sms_location = 0;
volatile unsigned int data_error_validation_code = 0, data_error_correct_value = 0;
volatile unsigned char eeprom_access = 0;
volatile unsigned char data_error_buffer[20] = {0}, validate_para_cnt = 0;
volatile unsigned char temp_counter = 0, temp_cach = 0; //data_error_occurance=0,
signed char temp_cntr_1 = 0;
float sch_disp_fraction = 0.0;

/****************************************************************************************************/
//-----------------------------  SD CARD S---------------------------------------//
//volatile unsigned char cur_dir[20] ={0};  //v3.1.1.B
unsigned int general_time_stamp_logout = 0;
volatile unsigned char file_name[20] = {0};
unsigned int sd_year = 0;
volatile unsigned char sd_month = 0, sd_date = 0, sd_hour = 0, sd_min = 0, sd_sec = 0, sd_yr = 0, P_month = 0, P_date = 0, P_yr = 0, last_year_backup = 0, last_month_backup = 0, last_date_backup = 0;
volatile unsigned char temp_sd_bytes[20] = {0};
//FSFILE *pointer;
FSFILE *sd_pointer;
volatile unsigned char SD_Buffer[MAX_DATA_LENGTH] = {0}; //1024
volatile unsigned char server_sync_status = 0; //v3.0.0
volatile unsigned char offline_rfid_card[9] = {0}; //v3.0.0
unsigned long int SD_lastbyte_Pointer = 0;

volatile char str_test[1000] = "";
volatile char * pch;
volatile unsigned int char_location = 0;
volatile unsigned int cntr_SD = 0;
volatile unsigned int Protocol_No = 1;
unsigned int sd_data_count = 0, err_pen_file_counter = 0;


float offline_water_tariff = 50.0;
//volatile unsigned char curnt_user_bal_ary[7]={0};   //2.3.B
volatile unsigned long int curnt_user_bal_paisa = 0, previous_balance = 0, Rfid_user_bal_paisa = 0;
//volatile unsigned int curnt_user_bal_rs=0;
volatile double curnt_user_bal_rs = 0.0;
volatile double Rfid_user_bal_rs = 0.0;
volatile unsigned char user_balance_factor = 50;
volatile double daily_total_dispense = 0.0;
volatile unsigned long int daily_total_lit = 0;
volatile unsigned char daily_total_lit_ary[10] = {0};
//volatile unsigned char user_bal_file[25]={0};  //v3.1.1.B
volatile unsigned char sd_delay = 2;
volatile unsigned char file_response = 0;
char sd_sync_buffer[20] = {0};
volatile unsigned long int sync_file_lastbyte_Pointer = 0;
volatile unsigned char single_date_sync = 0;
volatile unsigned char card_presense_fail_cntr = 0;

//-----------------------------  SD CARD E---------------------------------------// 

//-------------------------------------RFID YHY502 S-------------------------------------//
volatile unsigned char rfid_card[9] = {0};
volatile unsigned char scanned_rfid_card[25] = {0}; //v2.3.1
volatile unsigned char rfid_reader_counter = 0;
volatile unsigned char franchisee_rfid_tag[9] = {0}; //"2800757384AA";   //v3.1.1.B  franchisee_rfid_tag[13]
volatile unsigned char Rfid_remove_type = 0;
volatile unsigned char Rfid_addition_type = 0;
unsigned char command_array[35] = {0};
//unsigned char rfid_fetch_data[30]={0};  //v3.1.1.B
unsigned char rfid_element = 0;
unsigned int Rfid_time_stamp = 0;
unsigned char Rfid_loop = 0;
unsigned int card_init_balance = 0;
unsigned char rfid_read_fail_cntr = 0; //3.1.2.J
unsigned char cmd_response_count = 0; //3.1.3.B

/*******************************************************Coin UCA2**********************************/

volatile unsigned int coin_amount_paisa = 0;
volatile float coin_water_tariff = 50.0;
volatile unsigned char coin_amount = 0;

volatile unsigned char coinbox_data[20] = {0}; 

//volatile unsigned char enable_UCA_cmd[6]  = {0x90,0x05,0x01,0x03,0x99,0x5B};
//volatile unsigned char disable_UCA_cmd[6] = {0x90,0x05,0x02,0x03,0x9A,0x5B};
//volatile unsigned char UCA_status_check_cmd[6] = {0x90,0x05,0x11,0x03,0xA9,0x5B};

//volatile unsigned char en_dis_UCA_channel_cmd[7] = {0x90,0x06,0x90,0x20,0x03,0x49,0x5B};
//volatile unsigned char disable_channels_UCA_cmd[7] = {0x90,0x06,0x90,0x00,0x03,0x29,0x5B};

volatile unsigned int coin_time_stamp = 0, total_coin_amount = 0, coin_ideal_time_stamp = 0, total_coin_count = 0;
volatile unsigned int coin_collector_capacity = 300;
volatile unsigned char coinbox_counter = 0, eeprom_channel_read = 3;
volatile unsigned char coin_channel_no = 0, coin_id_channel_no = 0, coin_channel_En_Dis = 1, cmd_checksum = 0, channel_cmd = 0;
volatile unsigned char bit_extract_factor = 0x80;
volatile unsigned char temp_coin_id[13] = {0};
volatile unsigned char coin_rs_status[14] = {0};
unsigned char response_count = 0;
volatile unsigned char accepted_coin_id[13] = {""};
unsigned char coin_init_try = 0;
unsigned char data_valid_coin = 0;

unsigned char coin_send_data_arr[20] = {0}; 
unsigned char calibration_mode_en = 0;

volatile struct card_id
{
    unsigned char channel_1[9]; //  in all channel_1to8[13] to [9]  --2.3.A
    unsigned char channel_2[9];
    unsigned char channel_3[9];
    unsigned char channel_4[9];
    unsigned char channel_5[9];
    unsigned char channel_6[9];
    unsigned char channel_7[9];
    unsigned char channel_8[9];
} coin_id;

volatile struct flags_group_9
{
    unsigned coinselect                    : 1; //1
    unsigned coinprocess                   : 1; //2
    unsigned coinenable                    : 1; //3
    unsigned coinidle                      : 1; //4	
    unsigned coin_receive_process          : 1; //5	
    unsigned coin_checksum                 : 1; //6	
    unsigned coin_total_amount_write       : 1; //7	
    unsigned coin_debug                    : 1; //8	
//    unsigned coin_disable_by_error         : 1; //9
    unsigned coin_collector_alert          : 1; //10	  
    unsigned coin_collector_error          : 1; //11
    unsigned coin_channel_status           : 1; //12
    unsigned coin_disable_by_fs_error      : 1;	//13		//v2.3.6
    unsigned coin_FS_en_dis_auto_log_of:1;	//14		//v2.3.6
    unsigned coin_disable_fault_alert      : 1;	//15		//v2.3.6
    
} coin_flags;
/****************************************************************************************************/
//-------------------------------------BUTTON  AND MENU-------------------------------------///
volatile unsigned int button_detect_timestamp = 0;
volatile unsigned char button_detect = 0;
volatile unsigned int sensor_detect_timestamp    = 0; //1.0.1
volatile unsigned int Rfid_detect_timestamp = 0; //3.1.2
//volatile unsigned char sensor_detect=1;				//keep 1 to firstly check the status of all senors
volatile unsigned char error_type2 = 0;
//volatile unsigned long temp_adc_result_1 =0;


volatile struct flags_group_1
{
    unsigned send_idle_msg                   : 1; //1
    unsigned check_server_connection         : 1; //2
    unsigned local_ip                        : 1; //3
    unsigned send_ip_port                    : 1; //4
    unsigned send_apn                        : 1; //5
    unsigned force_reconnect                 : 1; //6
    unsigned system_stability                : 1; //7
    unsigned server_response                 : 2; //8,9
    unsigned server_connection               : 2; //10,11
    unsigned send_sys_setting                : 2; //12,13
    unsigned disp_led_state                  : 3; //14,15,16
    unsigned server_chiller_heater_status    : 3; //17,18,19  //v3.1.3
    unsigned battery_auto_correct            : 1; //19	v2.2.7
    unsigned super_admin                     : 1; //20	v2.2.9
    unsigned ultra_super_admin               : 1;
    unsigned ultra_super_admin_state         : 1;
    unsigned auto_sync_flag                  : 1;
} system_flags;

volatile struct flags_group_8
{
    unsigned sms_sender_ok                   : 1; //1
    unsigned solar_ack_sent                  : 1; //2
    unsigned sms_permit                      : 1; //3
    unsigned sim_card_check                  : 1; //4
    unsigned fact_password_ok                : 1; //5
    unsigned is_sms_from_server              : 2; //6,7
    unsigned scan_sms                        : 1; //8
    unsigned ac_ack_sent                     : 1; //9
    unsigned temp_sensor_failure_sent        : 1; //10
    unsigned msg_get                         : 1; //11  //v3.1.1.B
    unsigned signal_strength_check           : 1; //12  //v3.1.1.D
} gsm_flags;

volatile struct flags_group_2
{
    unsigned run_diagnostic                  : 1; //1
    unsigned diagnostic_test_toggle          : 1; //2
    //unsigned soft_mcu_reset:1;			//3
    unsigned op_mode                         : 5; //3,4,5,6,7  //v2.2.5
    unsigned op_sub_mode                     : 3; //8,9,10
    unsigned prev_op_mode                    : 5; //11,12,13,14
    unsigned forced_function_state           : 1; //15
    unsigned device_active_awake             : 1; //16
    unsigned validation_error                : 1; //17
    unsigned op_coin_mode                    : 1; //18 v2.2.9
} mode_flags;

volatile struct flags_group_3
{
    unsigned rfid_read                       : 1; //1
    unsigned dispense                        : 1; //2
    //@	unsigned dispense_type:2;			//3
    unsigned dispense_button                 : 1; //4
    unsigned acc_cummulative                 : 1; //6
    unsigned acc_switch_detect               : 1; //7
    unsigned freeze_display                  : 1; //8
    unsigned output_flow_sensed              : 1; //9
    unsigned coin_read                       : 1; //10 v2.2.9
    unsigned card_detect                     : 1;
    unsigned card_detect_first_time          : 1; //3.1.2
    unsigned dispense_error                  : 1;
    unsigned dispense_complete               : 1;
} consumer_flags;

volatile struct flags_group_4
{
    unsigned input_flow_sensed               : 1; //1
    unsigned remote_water_dump               : 1; //2
} dump_flags;

volatile struct flags_group_5
{
    unsigned input_flow_sensed               : 1; //1
    unsigned refill_sub_mode                 : 2; //2
    unsigned auto_refill_flag                : 1; //3

} refill_flags;

volatile struct flags_group_6
{
    unsigned dev_en_dis                      : 1; //1
    unsigned tds_en_dis                      : 1; //2
    unsigned lls_processing                  : 1; //3
    unsigned lls_trigger                     : 1; //4
    unsigned hls_processing                  : 1; //5
    unsigned hls_trigger                     : 1; //6
    unsigned lls_pin                         : 1; //7
    unsigned hls_pin                         : 1; //8
    unsigned lls_hls_en_dis                  : 2; //9,10

    unsigned tds_update                      : 1; //11
    unsigned pag_buzzer_control              : 1; //12
    unsigned peripheral_interrupt            : 1; //13
    unsigned solar_sense                     : 1; //15
    unsigned sv_on                           : 1; //17
    unsigned cold_water                      : 1; //19
    unsigned temp_en_dis                     : 1; //20
    unsigned sleep_mode_flag                 : 1; //21
    unsigned chiller_status_flag             : 1; //22
    unsigned heater_status_flag              : 1; //	//v3.1.3
    unsigned warm_water                      : 1; //34	//v3.1.3
    unsigned franchisee_tag_check            : 1; //23         //v2.2.5
    unsigned chiller_heater_start_flag       : 1; //24         //v3.1.3    
    unsigned chiller_heater_en_dis           : 2; //25,26      //v3.1.3
    unsigned solar_processing                : 1; //26   //v2.2.6
    unsigned solar_pin                       : 1; //27
    unsigned chiller_heater_lls_trigger      : 1; //28                      //v3.1.3
    unsigned lcd_refresh                     : 1; //29	//v2.2.7
    unsigned temp_compen_en_dis              : 1; //30	//v2.2.9
    unsigned coin_en_dis                     : 1; //31	//v2.2.9
    unsigned user_enter_password             : 1;
    unsigned head_key_get                    : 1;
    unsigned lls_last_first_state            : 3;
    unsigned lls_last_second_state           : 3;
} peripheral_flags;

volatile struct flags_group_7
{
    unsigned pag_error                       : 1; //1
    unsigned pag_error_sent                  : 1; //2
    unsigned apn_error                       : 1; //3
    unsigned low_batt_alert_sent             : 1; //5
    unsigned tds_high                        : 1; //6
    unsigned tds_low                         : 1; //7 
    //unsigned reset_all_ooo:1;			//9
    unsigned modem_sw_reset                  : 1; //9
    unsigned water_expired                   : 1; //10
    unsigned modem_response_error            : 2; //11,12
    unsigned tds_error_sent                  : 1; //13
    unsigned data_error_auto_correct         : 1; //14
    unsigned tank_empty                      : 1; //15
    unsigned coin_error_sent                 : 1; //16
    unsigned chiller_heater_relay_reset      : 1;	//17	//v3.1.2.H
  //  unsigned battery_critically_low:1;	//17	//v3.1.2.H
} error_flags;

volatile struct flags_group_10
{
    unsigned sms_check                       : 1; //1
    unsigned chiller_heater_control          : 1; //2
    unsigned coin_communication_check        : 1; //3
    unsigned coin_communication_retry        : 1; //4
    unsigned analog_sensor_update            : 1; //5
    unsigned system_error_flag               : 1; //6
    unsigned mode_refresh_check              : 1; //7
    unsigned check_temperature               : 1; //1.0.1
    unsigned modem_status_check              : 1; //v3.1.2.J
    unsigned modem_status_check_en_dis               : 1; //v3.1.2.J
} timer_flags;

volatile struct flags_group_12
{
    unsigned display_toggle                  : 2; //1,2
    unsigned display_mode                    : 1; //3
    unsigned freeze_display                  : 1; //4
} display_flags;

volatile struct flags_group_13
{
    //	unsigned newfile_create	:1;
    unsigned card_initialize                 : 1;
    unsigned card_detect                     : 1;
    unsigned sd_card_format                  : 1;
    unsigned Err_Pen_search                  : 1;
} sd_flags;

volatile struct flags_group_14
{
    unsigned exit                            : 1;
    unsigned timeout_menu_options            : 1;
    unsigned function_access                 : 1;
    //   unsigned dispense_button : 1;
    //@	unsigned dispense_type: 1;
    unsigned admin_access                    : 1;
    unsigned any_button_press                : 1;
} menu_flags;

volatile struct flags_group_15
{
    unsigned sync_by_modem                   : 1;
    unsigned time_halt                       : 1;
    unsigned sync_type                       : 3;
    unsigned sync_by_time                    : 2;
    unsigned halt_time_check                 : 1;
    unsigned date_change                     : 1;
    unsigned month_change                    : 1;
} rtc_flag;
//***********************************Coin Box UCA2 UART 3*****************************************************

void __attribute__((interrupt, no_auto_psv))_U3RXInterrupt(void) //v2.2.9       //2.3.H 
{
    U3RX_Clear_Intr_Status_Bit; //clear interrupt status of UART3 RX   


    while (!DataRdyUART3()); //wait for data reception on RX
    coinbox_data[coinbox_counter] = ReadUART3();

    if ((consumer_flags.coin_read == LOW) && (coinbox_counter >= 3)) //3.1.2
    {
        consumer_flags.coin_read = HIGH;
//        LCD_BACKLITE_ON;   //3.1.2.H
        LED3_ON;
        if (mode_flags.forced_function_state == FALSE)
        {
            mode_flags.forced_function_state = TRUE;
            function_state = ST_COIN_FUNCTION_DETECTED;
        }
    }
    else{}

    coinbox_counter++;

}
//*************** Interrupt Service routine for UART2 reception (Modem)*************************************

void __attribute__((interrupt, no_auto_psv))_U2RXInterrupt(void)
{
    unsigned char tmphead;
    unsigned char data;

    U2RX_Clear_Intr_Status_Bit; //clear the interrupt status of UART2 RX     

    while (!DataRdyUART2()); //wait for data reception on RX
    data = ReadUART2();

    tmphead = (Rx_Buffer_Head + 1) & MAX_RX_BUFFER_MASK;
    Rx_Buffer_Head = tmphead;
    Rxdata[Rx_Buffer_Head] = data;
    gsm_flags.msg_get = 1;
}

//*************** Interrupt Service routine for UART1 reception (RFID Reader)*************************************

void __attribute__((interrupt, no_auto_psv))_U1RXInterrupt(void)
{
    U1RX_Clear_Intr_Status_Bit; //clear interrupt status of UART1 RX        
    while (!DataRdyUART1()); //wait for data reception on RX
    scanned_rfid_card[rfid_reader_counter] = ReadUART1(); //v2.3.1
    rfid_reader_counter++;

    if (rfid_reader_counter >= 5)
    {
        //consumer_flags.rfid_read = HIGH;			
        LCD_BACKLITE_ON; //v2.2.7
    }
}
//************************ Interrupt service routine for INT2 (SWITCH 4) ****************************

void __attribute__((interrupt, no_auto_psv)) _INT2Interrupt(void)
{
    Int2_Clear_Intr_Status_Bit; //Clear interrupt status bit
    button_detect = MENU_DETECT;

    if (timer2_tick - button_tick >= 1000) //v2.2.3  //3.1.2  this if condition check before mode
    {

        if ((mode_flags.op_mode == SCHOOL_MODE) || (mode_flags.op_mode == CALIBRATION_MODE) || (mode_flags.op_mode == CORPORATE_MODE) || (mode_flags.op_mode == DUMP_MODE) || (mode_flags.op_mode == REFILL_MODE) || (mode_flags.op_mode == ANTENNA_MODE)) //v3.1.1.D
        {
            //	LCD_BACKLITE_ON;	//v2.2.7
            consumer_flags.dispense_button = 1;
            button_detect = MENU_DETECT;
            mode_flags.forced_function_state = TRUE;
            function_state = ST_BUTTON_PRESSED;
            dispense_button_count++;
            if (((mode_flags.op_mode == SCHOOL_MODE) || (mode_flags.op_mode == CALIBRATION_MODE))&&(dispense_button_count < 2))
            {
                sch_time_stamp = sec_tick; //sch_time_stamp = timer2_tick; 1.0.1
                SV_VALVE_ON;
                //LCD_BACKLITE_ON;  //sec_tick
                output_flow_count = 0;
                dispense_trail = 1;
            }
            else{}         
        }
        else
        {
            if (mode_flags.op_coin_mode)  //v3.1.2.G
            {
                consumer_flags.dispense_button = 1;
            } else{}
            if (mode_flags.op_mode == DEBUG_MODE) //v3.1.2.G
            {
            consumer_flags.dispense_button = 1;
            }else{}
        }       
    }
    else{}

    button_tick = timer2_tick; //v2.2.3
    //flow_count_timestamp = sec_tick;  //3.1.2.G

   
}
//************************ Interrupt service routine for INT1 (Output Flow Sensor) ****************************

void __attribute__((interrupt, no_auto_psv))_INT1Interrupt(void)
{
    Int1_Clear_Intr_Status_Bit; //Clear interrupt status bit
    if (timer2_tick - output_flow_tick >= output_flow_freq_limit) //v2.2.3
    {
        output_flow_count++;
        consumer_flags.output_flow_sensed = HIGH;

        if ((mode_flags.op_mode != CONSUMER_MODE)&&(mode_flags.op_mode != DUMP_MODE)&&(mode_flags.op_mode != SCHOOL_MODE)&&(mode_flags.op_mode != CALIBRATION_MODE)&&(mode_flags.op_mode != DIAGNOSTIC_MODE))
        {
            mode_flags.forced_function_state = TRUE;
            function_state = ST_WATER_LEAKAGE;
        }
        else{}

        if (((mode_flags.op_mode == SCHOOL_MODE) || (mode_flags.op_mode == CALIBRATION_MODE))&&((consumer_flags.dispense_button == 1) || (menu_flags.any_button_press == 1))) //2.3.C  //v3.1.1.D
        {
            //if(output_flow_count>=((int)(out_flow_calibration*((((float)school_disp_amount)/1000.0)))))
            if (output_flow_count >= ((int) (out_flow_calibration * sch_disp_fraction)))
            {
                SV_VALVE_OFF;
                dispense_button_count = 0;
                //LCD_BACKLITE_OFF;
                total_dispense_litre += (output_flow_count / ((float) (out_flow_calibration)));
                output_flow_count = 0;
                consumer_flags.output_flow_sensed = 0;
                menu_flags.any_button_press = 0;
                consumer_flags.dispense_button = 0;
            }
            else{}

        }
        else{}

    }
    else{}

    output_flow_tick = timer2_tick; //v2.2.3		
}
//************************ Interrupt service routine for INT3 (Input Flow Sensor) ****************************

void __attribute__((interrupt, no_auto_psv)) _INT3Interrupt(void)
{
    IFS3bits.INT3IF = 0; // Clearing interrupt flag
    if (timer2_tick - input_flow_tick >= input_flow_freq_limit) //v2.2.3
    {
        input_flow_count++;
        refill_flags.input_flow_sensed = HIGH;
        if ((mode_flags.op_mode != REFILL_MODE)&&(refill_flags.auto_refill_flag == FALSE)) //v2.2.4
        {
            mode_flags.forced_function_state = TRUE;
            function_state = ST_UNAUTH_WATER_INPUT;
        }
    }
    else{}

    input_flow_tick = timer2_tick; //v2.2.3	
    if (refill_flags.auto_refill_flag == TRUE)
        refill_time_stamp = sec_tick; //v2.2.4
    else{}
   
}

//************************ Interrupt service routine for INT0 (RFID DETECT) ****************************

void __attribute__((interrupt, no_auto_psv)) _INT0Interrupt(void)
{
    Int0_Clear_Intr_Status_Bit; //Clear interrupt status bit

    consumer_flags.card_detect_first_time = 1;
    Rfid_detect_timestamp = timer2_tick;
    //    if (mode_flags.op_mode != ULTRA_SUPER_ADMIN_MODE)
    //    {
    //	mode_flags.forced_function_state = TRUE; //0.0.1
    //	function_state = ST_CARD_DETECTED; //0.0.1
    //    }
    //    consumer_flags.card_detect = HIGH;
    //    general_time_stamp_logout = sec_tick;
}
//************************ Interrupt service routine for CN ****************************

void __attribute__((interrupt, no_auto_psv)) _CNInterrupt(void)
{
    InputChange_Clear_Intr_Status_Bit;
    //   if ((mode_flags.op_mode == SCHOOL_MODE) || (mode_flags.op_mode == CORPORATE_MODE) || (mode_flags.op_mode == CALIBRATION_MODE) || (mode_flags.op_mode == REFILL_MODE) || (mode_flags.op_mode == DUMP_MODE))  //v3.1.1.H

    if ((timer2_tick - sensor_detect_timestamp) >= 500)
    {
        //	sensor_detect = 1;		
        if ((!PORTEbits.RE8) || (!PORTEbits.RE9) || (!PORTBbits.RB5))
        {
            if ((mode_flags.op_mode == SCHOOL_MODE) || (mode_flags.op_mode == CORPORATE_MODE) || (mode_flags.op_mode == CALIBRATION_MODE) || (mode_flags.op_mode == REFILL_MODE) || (mode_flags.op_mode == DUMP_MODE))
            {
                peripheral_flags.peripheral_interrupt = TRUE;
                mode_flags.forced_function_state = TRUE;
                function_state = ST_BUTTON_PRESSED;
                menu_flags.any_button_press = 1;
                //LCD_BACKLITE_ON;
                output_flow_count = 0;
                dispense_trail = 1;
            }
           else{}
        }
        else{}
        if (PORTAbits.RA4 == 1) //v3.1.1.H
        {
            #ifdef OLD_BOARD 
               
                sd_flags.card_detect = 1;
            #else
                mode_flags.forced_function_state = TRUE;
                function_state = ST_MEMORY_CARD_STATE;
                sd_flags.card_detect = 0; //SD card not detect 
                
            #endif
        }
        else
        { //v3.1.1.H
            sd_flags.card_detect = 1; //SD card detect
        }

        if ((mode_flags.op_mode == SCHOOL_MODE) || (mode_flags.op_mode == CALIBRATION_MODE))
        {
            sch_time_stamp = sec_tick; //sch_time_stamp = timer2_tick; 1.0.1
            SV_VALVE_ON;
        }
        else{}

    }
    else
    {
        sensor_detect_timestamp = timer2_tick;
    } //1.0.1


}

int main(void)
{
    //---------------initilization system  S ----------------------------//
    
    unsigned char eeprom_erase_flag = 0;
    mcu_reset_source(); //Get cause of MCU reset
    io_mapping();
    hardware_init();
    SV_VALVE_OFF;
    CHILLER_HEATER_OFF;    //v3.1.3
    UART1_init();
    UART2_init();
    UART3_init(); //v2.2.9
    timer2_init();
    init_i2c();
    i2c_bus_recovery(); //i2c for eeproom
    adc_init();
    variable_init();
    DS1307_config();
    lcd_init();

    //---------------initilization system E ----------------------------// 
    LED1_ON;
    LED2_ON;
    LED3_ON;
    LED4_ON;
    WriteUART2((unsigned int) 26); //Clear pending data from modem if any
    serial_string_send("AT+CIPSHUT\r\n"); //Disconnect from server	

    strcpy(lcd_data, "   Powered by"); //v2.2.7
    lcd_display(1);
    strcpy(lcd_data, "PIRAMAL SARVAJAL");
    lcd_display(2);
    timer2_sec_delay(error_display_time);
    buzzer(HALT); //SYSTEM START BUZZER FOR 2.2.5

    strcpy(lcd_data, " SWAD: SARVAJAL "); //v2.2.5
    lcd_display(1);
    strcpy(lcd_data, "WATER ATM DEVICE");
    lcd_display(2);
    timer2_sec_delay(error_display_time);
    LED1_OFF;
    LED2_OFF;
    LED3_OFF;
    LED4_OFF;

    coinbox_disable(); //2.3.H   
    //@    peripheral_handler();  //v3.1.2.c
    MODEM_ON;
    SV_VALVE_OFF;
    init_temperature_sensor();

    //--------------------- EEPROM Read OR write  S-----------------------------------------------// 
    for (temp_counter = 0; temp_counter < 3; temp_counter++)
    {
        read_eeprom_parameter(SER_SIGNATURE_BYTE);
        ptr_1 = &signature;
        ptr_2 = &signature_byte;

        temp_cntr_1 = strcmp(ptr_1, ptr_2);
        if (temp_cntr_1 == 0)
            break;
        else
        {
        }
    }

    //    temp_cntr_1 = 1;
    if (temp_cntr_1)
    {
        lcd_init();
        strcpy(lcd_data, "INITIALIZING...");
        lcd_display(1);
        strcpy(lcd_data, "MEMORY.........");
        lcd_display(2);
        erase_ext_eeprom();

        for (temp_cntr_1 = 0; temp_cntr_1 < TOTAL_ERRORS; temp_cntr_1++)
            error_stack[temp_cntr_1] = 0;

        for (temp_cntr_1 = 1; temp_cntr_1 <= EEPROM_TOTAL_PARAMETERS; temp_cntr_1++)
        {
           
            eeprom_write(temp_cntr_1);
             if(temp_cntr_1==SER_COIN_CHN_EN_DIS)   //v3.1.2.D  
            {
                temp_cntr_1++;  //to skip SER_COIN_ID 81 number
            }
        }
        eeprom_write(0);
        eeprom_write(SER_TANK_LOW_LEVEL_STATUS); //v2.2.4
        coin_id_eeprom_write(); //v2.2.9 for write Coin ID on EEPROM Init
        eeprom_write(SER_SCHOOL_MODE_ACC_ID); //v3.1.1.C
        eeprom_write(SER_PREV_DATE);
        eeprom_write(SER_PREV_MONTH);
        eeprom_write(SER_PREV_YEAR);
        eeprom_write(SER_CARD_INIT_BAL);
        eeprom_write(SER_ULTRA_SUPER_ADMIN_MODE);
        eeprom_erase_flag = 1;
        //	configure_sms_mode(); ///set modem -> ECHO off
        //	rtc_flag.sync_by_modem = 1;
        //	rtc_flag.sync_type = BY_MODEM;
        //	get_modem_local_time(); //SET_TIME_BY_MODEM		
    }
    else
    {
        for (temp_cntr_1 = 0; temp_cntr_1 <= EEPROM_TOTAL_PARAMETERS; temp_cntr_1++)
        {
            read_eeprom_parameter(temp_cntr_1);
        }
        read_eeprom_parameter(SER_OP_MODE);
        read_eeprom_parameter(SER_ERROR_STACK);
        read_eeprom_parameter(SER_SCHOOL_MODE_ACC_ID); //SER_FLOW_OFFSET //SER_T_SMS_READ //SER_ER_STATE	
        read_eeprom_parameter(SER_TANK_LOW_LEVEL_STATUS); //v2.2.4 //  SER_COIN_COLLECTOR_ERROR	    
        read_eeprom_parameter(SER_COIN_COLLECTOR_ERROR); //v2.2.9 //1.0.2
        read_eeprom_parameter(SER_CORPORATE_MODE_ACC_ID);

        read_eeprom_parameter(SER_CARD_INIT_BAL); //v3.1.1.F
        read_eeprom_parameter(SER_PREV_DATE); //v3.1.1.F
        read_eeprom_parameter(SER_PREV_MONTH); //v3.1.1.F
        read_eeprom_parameter(SER_PREV_YEAR); //v3.1.1.F	
        read_eeprom_parameter(SER_ULTRA_SUPER_ADMIN_MODE); //v3.1.1.F     

        for (eeprom_channel_read = 1; eeprom_channel_read < 9; eeprom_channel_read++) //v2.2.9 Coin Channel ID of all channel
            read_eeprom_parameter(SER_COIN_ID);
    }
    peripheral_handler(); //v3.1.2.c
    //----------------------- EEPROM Read OR write  E  -----------------------------------------------//

#ifdef DEVICE_INIT
      eeprom_write(SER_ERASE_SIGNATURE);
#endif
    //----------------------- System status on SW pressed S  -----------------------------------------------// 
    if (button_detect == MENU_DETECT)
    {
        general_time_stamp_logout = sec_tick;
        file_response = 0;
        while ((sec_tick - general_time_stamp_logout) < 5)
        {
            file_response = FSInit();
            if (file_response == 1)
            {
                file_create();
                sd_flags.card_initialize = 1;
                clear_lcd();
                strcpy(lcd_data, "MEMORY DETECTED"); //v3.1.1.F
                lcd_display(1);
                timer2_sec_delay(error_display_time);
                break;
            }
            else if (file_response == 0)
            {
                sd_flags.card_initialize = 0;
                buzzer(ERR02);
                clear_lcd();
                strcpy(lcd_data, "MEMORY FAULT    ");
                lcd_display(1); //v3.1.1.D
                strcpy(lcd_data, "CONTACT SARVAJAL");
                lcd_display(2); //v3.1.1.D
                timer2_sec_delay(error_display_time); //v3.1.1.D

            }
            else
            {
            }
        }
        strcpy(lcd_data, "MCU RST FLAG");
        lcd_display(1);
        sprintf(lcd_data, "%u", device_reset_state);
        lcd_display(2);
        timer2_sec_delay(error_display_time);
        ds_temp_id();
        timer2_sec_delay(error_display_time);
        strcpy(lcd_data, "READ TIME"); //v2.2.5
        lcd_display(1);
        strcpy(lcd_data, "PLS WAIT....");
        lcd_display(2);
        get_local_time();
        sprintf(lcd_data, "%.2d/%.2d/%.2d [%.2d/%.2d]", sd_date, sd_month, sd_yr, P_date, P_month);
        lcd_display(1);
        sprintf(lcd_data, "%.2d:%.2d:%.2d [%.2d]", sd_hour, sd_min, sd_sec, P_yr);
        lcd_display(2);
        timer2_sec_delay(error_display_time);
        //	serial_string_send("AT+CMGDA=\"DEL ALL\""); //Delete all sms on SIM card //check if used or not
        //	serial_string_send("\r\n");
        //	timer2_sec_delay(error_display_time);
    } //----------------------- System status on SW pressed E  -----------------------------------------// 
    else
    {
        general_time_stamp_logout = sec_tick;
        file_response = 0;
        while ((sec_tick - general_time_stamp_logout) < 5)
        {
            file_response = FSInit();
            if (file_response == 1)
            {
                file_create();
                sd_flags.card_initialize = 1;
                break;
            }
            else if (file_response == 0)
            {
                sd_flags.card_initialize = 0;
                buzzer(ERR02);
                clear_lcd();
                strcpy(lcd_data, "MEMORY FAULT    ");
                lcd_display(1); //v3.1.1.D
                strcpy(lcd_data, "CONTACT SARVAJAL");
                lcd_display(2); //v3.1.1.D
                timer2_sec_delay(error_display_time); //v3.1.1.D
            }
            else{}
        }
    }
    get_local_time(); //get local time when start the system    
    button_detect = 0;
    configure_sms_mode(); ///set modem -> ECHO off
    lcd_init();
    strcpy(lcd_data, "DEVICE ID");
    lcd_display(1);
    strcpy(lcd_data, MACHINE_ID);
    lcd_display(2);
    timer2_sec_delay(1);
    if(peripheral_flags.temp_en_dis==1){read_temperature();} //v3.1.2.C   
    timer2_sec_delay(1);
    if(peripheral_flags.temp_en_dis==1){read_temperature();}//v3.1.2.C
    timer2_sec_delay(1);
    if(peripheral_flags.temp_en_dis==1){read_temperature();} //v3.1.2.C
    //timer2_sec_delay(error_display_time);    //v3.1.2.C
    clear_lcd();
    strcpy(lcd_data, "VERSION");
    lcd_display(1);
    strcpy(lcd_data, FIRMWARE_VERSION);
    lcd_display(2);
    timer2_sec_delay(1);
    if(peripheral_flags.temp_en_dis==1){read_temperature();} //v3.1.2.C
    timer2_sec_delay(1);
    if(peripheral_flags.temp_en_dis==1){read_temperature();} //v3.1.2.C
    timer2_sec_delay(1);
    if(peripheral_flags.temp_en_dis==1){read_temperature();} //v3.1.2.C        
    //        timer2_sec_delay(error_display_time);      //v3.1.2.C
    consumer_flags.card_detect = 0;
    //MODEM_ON;
    //SV_VALVE_OFF;
    
   
    //------------------------------- MODEM Check & SERVER Connect S----------------------------------------//	
    temp_cntr_1 = 0;
    network_state = INITIAL;
    while ((network_state != REGISTERED) && (network_state != REG_ROAMING)&&(temp_cntr_1 < 2))
    {
        network_check();
        temp_cntr_1++;
    }
    temp_cntr_1 = 0;
    timer2_sec_delay(1);
    if(peripheral_flags.temp_en_dis==1){read_temperature();}
    
   

    //------------------------------- MODEM Check & SERVER Connect E   ----------------------------------//	

    //------------------------------- RTC Halt CHeck S   ------------------------------------------------//	
    if (eeprom_erase_flag == 1)
    {
        rtc_flag.sync_by_modem = 1;
        rtc_flag.sync_type = BY_MODEM;
        get_modem_local_time(); //SET_TIME_BY_MODEM
    }
    else
    {
        rtc_flag.sync_by_modem = 0;
        rtc_flag.sync_by_time = 1;
        rtc_flag.halt_time_check = 1;
        get_modem_local_time(); //Read modem time  and compare with RTC time and update
    }
    send_server_response(DEV_CONNECTED); //v3.1.1.G


    //------------------------------- RTC Halt CHeck E ---------------------------------------------------//	

    //------------------------------- GET AVG OF TEMP & TDS ----------------------------------------------//	 
    temp_cntr_1 = 0;
    for (temp_cntr_1 = 0; temp_cntr_1 < AVG_READING_TDS; temp_cntr_1++) //v3.1.1.F get avg before modem check   //3.1.1 shift after modem connect
    {
        update_analog_peripheral(TDS_CHANNEL);
        timer2_ms_delay(50); //v3.1.2.B
        //  update_analog_peripheral(TEMPERATURE_CHANNEL);        //v3.1.2.B
        // timer2_ms_delay(50); //v3.1.2.B
    }

    //------------------------------- GET AVG OF TEMP & TDS ----------------------------------------------//	

    //------------------------------- MODE SET BEFORE STORE ----------------------------------------------//	
    if (mode_flags.op_mode == SCHOOL_MODE)
    {
        read_eeprom_parameter(SER_SCHOOL_MODE_ACC_ID);
    }
    else
    {
    }

    if (mode_flags.op_mode == DIAGNOSTIC_MODE)
    {
        mode_flags.run_diagnostic = TRUE; //Run diagnostic test
        SV_VALVE_ON;
        timer2_sec_delay(error_display_time);
        SV_VALVE_OFF;
    }
    if (mode_flags.op_mode == DEBUG_MODE)
    {
        mode_flags.forced_function_state = TRUE;
        function_state = ST_DEBUG_STATE;
    }
    else if (mode_flags.op_mode == DIAGNOSTIC_MODE)
    {
        mode_flags.forced_function_state = TRUE;
        function_state = ST_DIAGNOSTIC_STATE;
    }
    else if (mode_flags.op_mode == ANTENNA_MODE)
    {
        mode_flags.forced_function_state = TRUE;
        function_state = ST_OP_MODE;
    }
    else if (mode_flags.op_mode == ULTRA_SUPER_ADMIN_MODE)
    {
        mode_flags.forced_function_state = TRUE;
        function_state = ST_ULTRA_SUPER_ADMIN_MODE;
    }
    else if (mode_flags.op_mode == ERROR_MODE)
        system_error_check();
    else
        function_state = 0;
    //------------------------------- CHECK COIN MODULE EN OR DIS ---------------------------------------//	

    mode_flags.forced_function_state = FALSE; //v2.3.5 to switch off Backlight //
    if ((peripheral_flags.coin_en_dis)&&(system_error == ERROR_FREE)&&(coin_flags.coin_collector_error == 0)&& (coin_flags.coin_disable_by_fs_error == 0)&&(mode_flags.op_mode != SCHOOL_MODE)&&(mode_flags.op_mode != ST_ULTRA_SUPER_ADMIN_MODE) &&(mode_flags.op_mode != CORPORATE_MODE)&&(mode_flags.op_mode != DEBUG_MODE)&&(mode_flags.op_mode != DIAGNOSTIC_MODE)&&(mode_flags.op_mode != ANTENNA_MODE)&&(mode_flags.op_mode != ERROR_MODE))  //3.1.2.I
    { //v2.3.4
        coinbox_init();
        //  new_ch_check_en_dis();
        strcpy(lcd_data, "COIN BOX");
        lcd_display(1);
        if (coin_flags.coinenable == TRUE)
        { //v2.3.3
            strcpy(lcd_data, "ENABLE");
            lcd_display(2);
        }
        else
        {
            strcpy(lcd_data, "ENABLE FAILED");
            lcd_display(2);
        }
    }
    else
    {
        coinbox_disable();
        if(peripheral_flags.coin_en_dis){
        strcpy(lcd_data, "COIN BOX");
        lcd_display(1);
        strcpy(lcd_data, "DISABLE");
        lcd_display(2);
        }
        else{
            memset(coinbox_data, 0, sizeof (coinbox_data)); //v3.1.1.D
            coinbox_counter = 0;
            consumer_flags.coin_read = LOW;
            DisableIntU3RX ;
        }
            
    }
    timer2_sec_delay(1);
    if(peripheral_flags.temp_en_dis==1){read_temperature();} //3.1.2.C
    timer2_sec_delay(1);
    if(peripheral_flags.temp_en_dis==1){read_temperature();} //3.1.2.C
    timer2_sec_delay(1);
    if(peripheral_flags.temp_en_dis==1){read_temperature();} //3.1.2.C
    //        timer2_sec_delay(error_display_time);
    //------------------------------- CHECK COIN MODULE EN OR DIS ---------------------------=-----------//	
    system_error_check();

    //------------------------------- All Variable Nullify when Start Prog -------------------------------//	

    peripheral_flags.lcd_refresh = TRUE; //v2.2.7

    rtc_flag.sync_by_time = 0;
    timer_flags.check_temperature = 0;
    menu_flags.any_button_press = 0;
    button_detect = 0;

    last_date_backup = 0;
    last_month_backup = 0;
    last_year_backup = 0;
    FSfclose(sd_pointer); //File close when any open   
    coin_flags.coin_receive_process = FALSE; //clear data from start
    clear_UCA2_data(); //v3.1.1.H
    peripheral_handler(); //v3.1.2.c  //v3.1.2.G

    if (tank_low_level_litre < 5) //V3.1.2.c
    {
        peripheral_flags.chiller_heater_lls_trigger = TRUE;
    }
    else
    {
        peripheral_flags.chiller_heater_lls_trigger = FALSE;
    }
     	//     ----------   v3.1.2.J   Modem run time routine     --------------------------------------------------------
    read_eeprom_parameter( SER_MODEM_CHECK_ROUTINE );  //3.1.2.J
    if( ( modem_status_check_hour >=0 ) && (modem_status_check_hour <=24)  )
    {       
			if (modem_status_check_hour == 0 )
					{ modem_status_check_min = 0;   } 
			else if (modem_status_check_hour == 1 )
					{ modem_status_check_min = 240 ; }
			else 
					{ modem_status_check_min  = (modem_status_check_hour * 60);  }
    }
    else
    {
            modem_status_check_hour =0;	
            modem_status_check_min  = 0;
    }
	if (modem_status_check_min==0)
            { timer_flags.modem_status_check_en_dis  = 0; }
	else    
            {timer_flags.modem_status_check_en_dis  = 1;}
    //     ----------   v3.1.2.J   Modem run time routine  ------------------------------------------------------------
  
    
    // ----------------------------------	   SYSTEM   START    ---------------------------------------------//  
     
    while (1)
    {
        if ((gsm_flags.msg_get == 1) && (timer_flags.sms_check == 1)&& (consumer_flags.card_detect == LOW)&&(consumer_flags.coin_read == LOW)  && (mode_flags.forced_function_state==0) ) //v3.1.1.D
        {            
            gsm_flags.msg_get = 0;
            timer_flags.sms_check = 0;
#ifdef TEST_ON
            strcpy(lcd_data, "MSG RECIEVED 1"); //v2.2.7
            lcd_display(1);
#endif
            timer2_sec_delay(1);
            if (scan_sms_memory())
            {
                check_sms();
            }
            else{}
        }
        else{}

        if ((sch_disp_send_server == TRUE)&&(consumer_flags.card_detect == LOW)&&(consumer_flags.coin_read == LOW)  && (mode_flags.forced_function_state==0) ) //v1.0.0
        {
            if (total_dispense_litre != 0.0)
            {
                total_water_dispense_store(total_dispense_litre); //1.0.1
                send_server_response(DEV_BALANCE_DEDUCT);
                sch_disp_send_server = 0;

                tank_low_level_update(total_dispense_litre); //v2.2.3
                total_dispense_litre = 0.0;
            }
            else
                sch_disp_send_server = FALSE;
        }
        else{}

        if ((timer_flags.system_error_flag == 1) &&(consumer_flags.card_detect == LOW) && (consumer_flags.coin_read == LOW)&&(mode_flags.op_mode != CONSUMER_MODE)&&(mode_flags.forced_function_state == FALSE)  && (mode_flags.forced_function_state==0) )//v1.0.0
        {
            timer_flags.system_error_flag = 0;
            system_error_check();
            system_error = ERROR_FREE; //3.1.0
            if ((system_error == ERROR_FREE) && (mode_flags.op_mode == ERROR_MODE))
            { //3.1.0                                
                mode_flags.op_mode = mode_flags.prev_op_mode;
                memset(error_stack, 0, sizeof (error_stack)); //1.0.1
                //		eeprom_write(SER_ERROR_STACK);	         //1.0.1
            }
            else{}
        }
        else{}

        if ((coin_flags.coin_total_amount_write == 1) && ((sec_tick - coin_ideal_time_stamp) >= IDEAL_COIN_TIME) && (consumer_flags.card_detect == LOW) && (consumer_flags.coin_read == LOW)  && (mode_flags.forced_function_state==0) )
        {//v2.3.5 //1.0.2
            coin_flags.coin_total_amount_write = 0;
            //		coin_ideal_time_stamp = 0;
            eeprom_write(SER_TOTAL_COIN_AMOUNT);
            eeprom_write(SER_TOTAL_COIN_COUNT);
        }
        else{}
        if ((coin_init_try > 0) && (timer_flags.coin_communication_retry == 1) && (consumer_flags.card_detect == LOW) && (consumer_flags.coin_read == LOW)  && (mode_flags.forced_function_state==0)  ) //v2.3.4 (condition updated) 11 Sec
        {
            timer_flags.coin_communication_retry = 0;
            coin_check_on_error();
        }
        else{}
        if ((timer_flags.coin_communication_check == 1) && (coin_init_try == 0) && (consumer_flags.card_detect == LOW) && (consumer_flags.coin_read == LOW)   && (mode_flags.forced_function_state==0) ) //v2.3.4 30 min
        {
            timer_flags.coin_communication_check = 0; //v2.3.4
            coin_check_on_interval(); //3.1.2
        }
        else{}
        // ----------------------------------------  chiller/heater control	 ---------------------------------------------------
        if ((timer_flags.chiller_heater_control == 1)&&(consumer_flags.rfid_read == LOW)&&(consumer_flags.coin_read == LOW)&&(consumer_flags.dispense_button == LOW)  && (mode_flags.forced_function_state==0) ) //v2.3.4 12 Sec
        {
            timer_flags.chiller_heater_control = 0;
            CHILLER_HEATER_CONTROL(); //v2.3.6
        }
        else{}
        // ----------------------------------------  chiller/heater control	 ---------------------------------------------------
        
        // ---------------------------------------   modem_check_runtime         ---------------------------------------------------
        if( (timer_flags.modem_status_check == 1) &&(consumer_flags.card_detect == LOW) && (consumer_flags.coin_read == LOW)&& (mode_flags.forced_function_state == FALSE)  )//v3.1.2.J
        {           
            timer_flags.modem_status_check = 0;
            send_server_response(DEV_N_W_SIGNAL_STRENGTH);
            modem_state_check_runtime();
            if(modem_reset_count >= 2)  //modem_reset_count
            {  
                temp_cntr_1 = 0;
                network_state = INITIAL;
                error_flags.modem_sw_reset = TRUE;
                //check netwoer three times when continues command reponse fail.
                while ((network_state != REGISTERED) && (network_state != REG_ROAMING)&&(temp_cntr_1 < 4))
                {
//                    sprintf(lcd_data,"TP3 = %c-%d-%d",network_state,temp_cntr_1,modem_reset_count);    timer2_sec_delay(1); //delay
                    network_check();
                    temp_cntr_1++;
//                    sprintf(lcd_data,"TP4 = %c-%d-%d    %d",network_state,temp_cntr_1,modem_reset_count,mcu_auto_reset_counter);
//                    clear_lcd();
//                    lcd_display(2);    
                    timer2_sec_delay(1); //delay
                }
                temp_cntr_1 = 0;
                modem_reset_count =0;
            }
        }else{}
       // ----------------------------------------   modem_check_runtime         ---------------------------------------------------
        //   * *************************  
        if (((timer_flags.mode_refresh_check == 1) || (peripheral_flags.lcd_refresh == TRUE)) && (consumer_flags.card_detect == LOW)&&(consumer_flags.coin_read == LOW)&&(mode_flags.op_mode != CONSUMER_MODE)&&(system_flags.ultra_super_admin_state == 0)   && (mode_flags.forced_function_state==0) ) //v1.0.0
        {
            timer_flags.mode_refresh_check = 0;
            peripheral_flags.lcd_refresh = FALSE;
            system_flags.check_server_connection = TRUE;
            //lcd_init();  //3.1.0
            
            if(rfid_read_fail_cntr>3)  //3.1.2.J
            {
                rfid_read_fail_cntr = 0;
                system_flags.system_stability = FALSE;
            }
            else{}

            if (mode_flags.op_mode == SCHOOL_MODE)
            {
                sch_disp_fraction = ((float) school_disp_amount) / 1000.0;
                sprintf(disp_buffer, "%2.1f", sch_disp_fraction);
                lcd_init();
                sprintf(lcd_data, "DISPENSING:%s L", disp_buffer);
                lcd_display(1);
                sprintf(lcd_data, "TDS:%.4s ppm  ", tds_value);
                lcd_display(2);
                //LCD_BACKLITE_OFF;
                timer2_ms_delay(550);
                LED1_OFF; //3.1.0 lde3 off  when sensor error free  //v3.1.2.G
            }
            else if (mode_flags.op_mode == CORPORATE_MODE) //v2.2.5
            {
                if ((error_flags.water_expired != TRUE) && (error_flags.tank_empty != TRUE))
                {
                    strcpy(lcd_data, "PRESS ANY BUTTON"); //v2.2.7
                    lcd_display(1);
                    if (peripheral_flags.cold_water == TRUE)      //chiller
                        strcpy(lcd_data, "FOR COLD WATER  ");
                    else if (peripheral_flags.warm_water == TRUE)  	//v2.3.6
                        strcpy(lcd_data, "FOR WARM WATER  ");
                    else
                        strcpy(lcd_data, "FOR CLEAN WATER "); //v2.2.3

                    lcd_display(2);
                    timer2_ms_delay(550);         //3.1.3

                    LED1_OFF; //3.1.0 lde3 off  when sensor error free  //v3.1.2.G
                }
                else
                {
                    LCD_BACKLITE_OFF; // if found tank empty error, then we will not show long push button msg. //v2.2.6
                    clear_lcd();
                }

            }
            else if (mode_flags.op_mode == CALIBRATION_MODE)
            {
                //lcd_init();
                clear_lcd();
                
                if (peripheral_flags.temp_en_dis == 1)
                    sprintf(lcd_data, "OC:%.4d WT:%.4s", out_flow_calibration, temp_value);
                else
                    sprintf(lcd_data, "OC:%.4d IC:%.4d", out_flow_calibration, in_flow_calibration);
                    lcd_display(1);
                    
                    sprintf(lcd_data, "TC:%.3d TDS:%.4s", tds_calib_fact, tds_value);          
                    lcd_display(2);
                    LED1_OFF; //3.1.0 lde3 off  when sensor error free  //v3.1.2.G
            }
            else if (mode_flags.op_mode == ERROR_MODE) //3.1.0 //ask  ////3.1.0  led3 on if any error comes and still 
            {
                LED1_ON; //v3.1.1.D
            }
            else //v2.2.7
            {
                if ((mode_flags.op_mode != DIAGNOSTIC_MODE)&&(mode_flags.op_mode != ERROR_MODE) && (mode_flags.op_mode != FRANCHISEE_MODE)) //1.0.1
                {  
                    strcpy(lcd_data, "   Powered by"); //v2.2.7                
                    lcd_display(3);
                    strcpy(lcd_data, "PIRAMAL SARVAJAL");
                    lcd_display(4);                    
//                    LCD_BACKLITE_OFF;     //3.1.2.I             
                    if(mode_flags.forced_function_state==FALSE){ LCD_BACKLITE_OFF;}  //v2.3.5  //3.1.2.I     //do_finalize
                    else{}		
                    
                    timer2_ms_delay(550);
                }
                else{}
                LED1_OFF; //3.1.0 lde3 off  when sensor error free  //v3.1.1.D
            }
            timer2_ms_delay(400);
        }
        else{}

        if ((school_mode_en == TRUE)&&(dispense_trail == 'E')) //v2.2.3
        {
            if (total_dispense_litre != 0.0)
            {
                total_water_dispense_store(total_dispense_litre); //This command fire when if flow sense error comes and water dispense more than  0.0 litre.
                send_server_response(DEV_BALANCE_DEDUCT);
            }
            else{}
            dispense_trail = 0;
            eeprom_write(SER_ERROR_STACK);
            system_error_check();
            display_error_mode();
            buzzer(ERR01);

            //		if(error_stack[SV_ER]==TRUE) //v1.0.1
            //		{
            //			send_server_response(DEV_SOLENOID_VALVE_ERROR);				
            //		}
            if (error_stack[FLOW_SENS_ER] == TRUE) //v1.0.1
            {
                if ((peripheral_flags.lls_hls_en_dis >= 2)&&(peripheral_flags.lls_trigger == TRUE)&&(tank_low_level_litre <= 1))
                {
                    system_error = TANK_EMPTY_ER;
                    error_stack[TANK_EMPTY_ER] = TRUE;
                    error_flags.tank_empty = TRUE;
                    send_server_response(DEV_TANK_EMPTY);
                }
                else
                {
                    error_stack[FLOW_SENS_ER] = TRUE;
                    send_server_response(DEV_FLOW_SENSOR_ERROR);
                }
            }
            else{}
            send_server_response(DEV_HB_MSG);
        }
        else{}

        if (mode_flags.forced_function_state == FALSE)
        {
            //---function_state++;	
            if (function_state <= (ST_MAX - 1))
                function_state++;
            else
                function_state = 1;
        }
        else
        {
            mode_flags.forced_function_state = FALSE;
        }
        //------------------------------start swicth-------------------------------
        switch (function_state)
        {

        case ST_RTC_TIME_CHECK:
            if ((rtc_flag.sync_by_time == 1)&&(consumer_flags.dispense_button == 0)&&(menu_flags.any_button_press == 0)&&(consumer_flags.card_detect == LOW) && (consumer_flags.coin_read == LOW))
            {
                rtc_flag.sync_by_modem = 0;
                rtc_flag.halt_time_check = 1;
                get_modem_local_time();
            }
            else{}
            break;

        case ST_IDLE_MSG: //1
            if ((system_flags.send_idle_msg == TRUE) &&(mode_flags.op_mode != DIAGNOSTIC_MODE)&&(consumer_flags.card_detect == LOW) && (consumer_flags.coin_read == LOW))
            {
                timer2_ms_delay(200); //delay to avoid simultaneous protocol sending on server
                send_server_response(DEV_HB_MSG);
                system_flags.send_idle_msg = FALSE;

                if ((peripheral_flags.lls_hls_en_dis >= 2)&&(peripheral_flags.lls_trigger == TRUE) && (LLS_PIN_CHECK == LOW)) //v2.2.3 //3.1.2.D
                    display_tank_low_alert();
                else{}
            }
            else{}
            break;

        case ST_CARD_DETECTED: //2

            if ((consumer_flags.card_detect == HIGH)&&(system_flags.ultra_super_admin_state == 0))
            {
                if ((sd_flags.card_initialize == 0) || (sd_flags.card_detect == 0)) //v3.1.1.D  //v3.1.1.H
                {
                    LED2_ON;
                    buzzer(ERR02);
                    strcpy(lcd_data, "MEMORY FAULT    "); //v3.1.1.D
                    lcd_display(1);
                    strcpy(lcd_data, "CONTACT SARVAJAL"); //3.1.1.D
                    lcd_display(2);
                    timer2_sec_delay(error_display_time); //v3.1.1.D
                }
                else
                {
                    if (card_Serial_number() == 1)
                    {
                        consumer_flags.rfid_read = HIGH;
                    }
                    else
                    {
                        consumer_flags.rfid_read = LOW;
                        consumer_flags.card_detect = LOW;
                    }
                }
            }
            else
            {
                consumer_flags.rfid_read = LOW;
                consumer_flags.card_detect = LOW;
            }
            if ((consumer_flags.rfid_read == HIGH)&&(system_flags.ultra_super_admin_state == 0))
            {
                consumer_flags.rfid_read = LOW;
                mode_flags.device_active_awake = TRUE;
                if ((mode_flags.op_mode != DIAGNOSTIC_MODE)&&(peripheral_flags.dev_en_dis == TRUE))
                {                   		
                    card_detection();                  		
                }
                else
                {
                    display_error_mode();
                    memset(rfid_card, 0, sizeof (rfid_card)); //v3.1.1.B
                    memset(scanned_rfid_card, 0, sizeof (scanned_rfid_card)); //v3.1.1.B                
                }
                //consumer_flags.rfid_read = LOW;
                //consumer_flags.card_detect = LOW;		// to operate SV in consumer mode	
            }
            else
            {
                consumer_flags.rfid_read = LOW;
                consumer_flags.card_detect = LOW;
            }
            break;

        case ST_BUTTON_PRESSED: //3
            if ((consumer_flags.dispense_button == 1) || (menu_flags.any_button_press == 1)&&(consumer_flags.card_detect == LOW) && (consumer_flags.coin_read == LOW))
            {
                peripheral_flags.lcd_refresh = TRUE; //v2.2.7
                //---unsigned int sch_time_stamp = 0;
                if ((mode_flags.op_mode != SCHOOL_MODE) && (mode_flags.op_mode != CORPORATE_MODE)&& (mode_flags.op_mode != CALIBRATION_MODE))
                {
                    //---lcd_init();	// v2.2.7
                    button_detect = 0;
                    consumer_flags.dispense_button = 0; //3.1.0
                    menu_flags.any_button_press = 0;
                }
                else{}
                mode_flags.device_active_awake = TRUE;

                if ((mode_flags.op_mode == ERROR_MODE) || (mode_flags.op_mode == DIAGNOSTIC_MODE))
                {
                    display_error_mode();
                }
                else
                {


                    //                                if((mode_flags.op_mode!=SCHOOL_MODE) && (mode_flags.op_mode!=CORPORATE_MODE))
                    //                                {						
                    //                                    timer2_sec_delay(error_display_time/2);
                    //                                    lcd_init();
                    //                                    strcpy(lcd_data,"PLEASE SCAN CARD");
                    //                                    lcd_display(1);				
                    //                                    if(coin_flags.coin_collector_error){
                    //                                        strcpy(lcd_data,"COIN CLECTOR FULL");
                    //                                    }
                    //                                    else if(peripheral_flags.cold_water==TRUE)	//chiller
                    //                                        strcpy(lcd_data,"FOR COLD WATER  ");
                    //                                    else if(peripheral_flags.coin_en_dis){
                    //                                        sprintf(lcd_data,"%.5s%.10s","COIN:",coin_rs_status);
                    //                                    }
                    //                                    else
                    //                                        strcpy(lcd_data,"FOR CLEAN WATER ");	//v2.2.3
                    //                                    lcd_display(2);
                    //                                    button_detect = 0; 
                    //                                    consumer_flags.dispense_button=0;
                    //                                    menu_flags.any_button_press=0;
                    //                                    timer2_sec_delay(error_display_time);
                    //                                    LCD_BACKLITE_OFF;
                    //                                    clear_lcd();			
                    //                                }
                    //                                //					else{}

                    if ((mode_flags.op_mode == CORPORATE_MODE)&&(corporate_mode_en == 1)) //v2.2.5
                    {
                        if ((error_flags.water_expired != TRUE) && (error_flags.tank_empty != TRUE)) //v2.2.6
                        {
                            corporate_mode();
                        }
                        else
                        {
                            display_error_mode();
                        }
                        consumer_flags.dispense_button = 0;
                        menu_flags.any_button_press = 0; //consumer_flags.dispense_button = 0;
                    }
                    else{}
                }
            }
            break;

        case ST_SERVER_CHECK: //5
            //if((consumer_flags.card_detect==LOW) && (consumer_flags.dispense_button==LOW)&&(menu_flags.any_button_press==0)&&(mode_flags.op_mode==DIAGNOSTIC_MODE))
            if ((consumer_flags.card_detect == LOW)&& (consumer_flags.coin_read == LOW) && (consumer_flags.dispense_button == LOW)&&(menu_flags.any_button_press == 0)&&(mode_flags.op_mode == DIAGNOSTIC_MODE))
            {
                if (system_flags.check_server_connection == TRUE)
                {
                    system_flags.check_server_connection = FALSE;
                    check_server_connection();
                    {
                        while (system_flags.server_connection != SERVER_CONN_OK)
                        {
                            connect_to_server();
                            check_server_connection();
                        }
                        if ((mode_flags.op_mode != DIAGNOSTIC_MODE)&&(mode_flags.op_mode != SCHOOL_MODE)&&(mode_flags.op_mode != CORPORATE_MODE) &&(mode_flags.op_mode != CALIBRATION_MODE))
                        {
                            if ((consumer_flags.card_detect == LOW)&&(consumer_flags.coin_read == LOW)&&(consumer_flags.dispense_button == LOW)) //v2.3.0
                            {
                                LCD_BACKLITE_OFF;
                            }
                            else{}
                        }
                        else{}
                    }
                }
                else{}
            }
            else{}
            break;

        case ST_SMS_CHECK: //7

            //   if((timer_flags.sms_check==1) && (consumer_flags.card_detect==LOW)) 	//v1.0.0 //v2.3.4	- 5 Sec
            //	    if ((timer_flags.sms_check == 1) &&(consumer_flags.coin_read == LOW) && (consumer_flags.card_detect == LOW)) //v1.0.0 //v2.3.4	- 5 Sec
            //	    {
            //		timer_flags.sms_check = 0;
            //		//                if(scan_sms_memory()){	              
            //		//                check_sms();
            //		//                timer2_ms_delay(550);		                			
            //		//                }
            //		//                else{}
            //	    } else
            //	    {
            if ((consumer_flags.card_detect == LOW)&&(consumer_flags.coin_read == LOW) && (consumer_flags.dispense_button == 0)&&(menu_flags.any_button_press == 0)) //v2.3.2
            {
                if (gsm_flags.scan_sms == TRUE)
                {
                    temp_cach = data_validation(validate_para_cnt);
#ifdef TEST_ON
                    strcpy(lcd_data, "CHECK MSG");
                    lcd_display(2);
#endif
                    check_sms();
                    gsm_flags.scan_sms = 0;
                }
                else{}
                if (gsm_flags.signal_strength_check == 1) //v3.1.1.D
                {
                    gsm_flags.signal_strength_check = 0;
                    if ((consumer_flags.card_detect == LOW)&&(consumer_flags.coin_read == LOW)) //offline
                        check_network_strength(FALSE);
                    else{}
#ifdef TEST_ON
                    strcpy(lcd_data, "SIG. STR. CHECK");
                    lcd_display(2);
#endif
                    system_flags.check_server_connection = TRUE;
                }
                else{}
            }
            else{}
            //	    }
            break;

        case ST_PP_INTERRUPT: //8
            if ((consumer_flags.card_detect == LOW)&& (consumer_flags.coin_read == LOW) && (consumer_flags.dispense_button == 0)&&(menu_flags.any_button_press == 0)) //v2.3.2
            {
                if (peripheral_flags.peripheral_interrupt == TRUE)
                    peripheral_handler();
                else{}
            }
            else{}
            break;

        case ST_WATER_LEAKAGE: //9
            if ((consumer_flags.output_flow_sensed == HIGH)&&(consumer_flags.dispense_button == 0)&&(menu_flags.any_button_press == 0))
            {
                if ((timer2_tick - out_leak_time_stamp) >= (leak_timeout * 1000)) //&& (output_flow_count>=25))
                {
                    if (output_flow_count >= ((int) (3 * (out_flow_calibration * DISPENSE_LEAST_COUNT))))
                    {
                        buzzer(ERR02);
                        system_error = WATER_LEAK_ER;
                        display_error_mode();
                        unauth_dispense_litre = ((output_flow_count) / ((float) (out_flow_calibration)));
                        output_flow_count = 0;
                        consumer_flags.output_flow_sensed = LOW;
                        total_water_dispense_store(unauth_dispense_litre);
                        send_server_response(DEV_UNAUTHORISED_WATER_DISPENSE);
                        send_server_response(DEV_HB_MSG);
                        unauth_dispense_litre = 0;
                    }
                    else
                    {
                        consumer_flags.output_flow_sensed = LOW;
                        output_flow_count = 0;
                    }
                }
                else{}
            }
            else
            {
                out_leak_time_stamp = timer2_tick;
            }
            break;
        case ST_UNAUTH_WATER_INPUT: //10
            if ((consumer_flags.card_detect == LOW)&& (consumer_flags.coin_read == LOW)&& (consumer_flags.dispense_button == 0)&&(menu_flags.any_button_press == 0)) //v2.3.2
                unauthorized_water_input_check();
            else{}
            break;

        case ST_DEBUG_STATE: //11
            if (mode_flags.op_mode == DEBUG_MODE)
                debug_mode();
            else{}
            break;

        case ST_DIAGNOSTIC_STATE: //12
            if ((mode_flags.run_diagnostic == TRUE) && (mode_flags.op_mode == DIAGNOSTIC_MODE))
                diagnostic_mode();
            else{}
            break;

        case ST_TDS_CHECK: //13
            if ((consumer_flags.card_detect == LOW)&& (consumer_flags.coin_read == LOW)&&(system_flags.ultra_super_admin_state == 0)&&(consumer_flags.dispense_button == 0)&&(menu_flags.any_button_press == 0)) //v1.0.0
                tds_check();
            else{}
            break;

        case ST_ERROR_CHECK: //14

            if ((consumer_flags.card_detect == LOW) && (consumer_flags.coin_read == LOW)&&(system_flags.ultra_super_admin_state == 0)&&(consumer_flags.dispense_button == 0)&&(menu_flags.any_button_press == 0)) //v1.0.0
            {
                if ((mode_flags.op_mode == ERROR_MODE) || (mode_flags.op_mode == DEBUG_MODE))
                {
                    system_flags.system_stability = FALSE;
                    LCD_BACKLITE_OFF;
                    SV_VALVE_OFF;
                    temp_cach = data_validation(validate_para_cnt);
                }
                else
                {
                    if (system_flags.system_stability == FALSE)
                        check_system_stability(1);
                    else{}
                    if ((mode_flags.op_mode != DIAGNOSTIC_MODE)&&(mode_flags.op_mode != SCHOOL_MODE)&&(mode_flags.op_mode != CORPORATE_MODE) &&(mode_flags.op_mode != CALIBRATION_MODE)) //v3.1.1.D
                    {
                        if ((!school_mode_en) && (!corporate_mode_en) && (!calibration_mode_en))
                        {
                            mode_flags.op_mode = IDLE_MODE;
                        }
                        else{}

                        if ((consumer_flags.card_detect == LOW)&&(consumer_flags.dispense_button == LOW))
                        {
                            //		LCD_BACKLITE_OFF;  //1.0.1 //ask
                        }
                        else{}
                        SV_VALVE_OFF;
                    }
                    else{}
                }
            }
            else{}
            break;

        case ST_OP_MODE: //15
            if ((system_flags.ultra_super_admin_state == 0))
            {
                if (mode_flags.op_mode == CONSUMER_MODE)
                {
                    consumer_mode();
                }
                else if (mode_flags.op_mode == REFILL_MODE)
                    refill_mode();
                else if (mode_flags.op_mode == DUMP_MODE)
                    dump_mode();
                else if (mode_flags.op_mode == ANTENNA_MODE)
                    antenna_test_mode();
                else if (mode_flags.op_mode == SLEEP_MODE)
                    sleep_mode();
                else if (mode_flags.op_mode == FRANCHISEE_MODE)
                    new_franchise_menu();
                else{}
            }
            else{}
            break;

        case ST_SCHOOL_MODE:
            break;

        case ST_ULTRA_SUPER_ADMIN_MODE: //17
            if (mode_flags.op_mode == ULTRA_SUPER_ADMIN_MODE)
                ultra_super_admin_mode();
            else{}
            break;
        case ST_DATE_CHANGE:
            if ((rtc_flag.date_change == 1)&&(consumer_flags.card_detect == LOW) && (consumer_flags.coin_read == LOW))
            {
                rtc_flag.date_change = 0;
                dispense_water_data_backup();
                read_eeprom_parameter(SER_AUTO_SYNC_EN_DIS);
                if (system_flags.auto_sync_flag == 1)
                {
                    Server_Sync_2(SEARCH_PEN, FILE_SEND_EN);
                }
                else{}

                if (rtc_flag.month_change == 1)
                {
                    err_pen_file_counter = 0;
                    Server_Sync_2(SEARCH_PEN, FILE_SEND_DIS); //err_pen_file_counter+ = count no of PEN file                    
                    Server_Sync_2(SEARCH_ERR, FILE_SEND_DIS); //err_pen_file_counter+=  Count no of ERR file
                    if (err_pen_file_counter <= 0)
                    {
                        if (search_file("SYNC.TXT", FILE_FIND_DELETE_REMAKE)) //find file and delete, after make new SYNC.txt 
                        {
                            server_sync_status = 5;
                            send_server_response(DEV_SERVER_SYNC);
                        }
                        else{}
                    }
                    else{}
                    rtc_flag.month_change = 0;
                }
                else{}
            }
            else{}
            break;

        case ST_ANALOG_SENSOR_UPDATE:
            if ((timer_flags.analog_sensor_update == 1)&&(consumer_flags.card_detect == LOW) && (consumer_flags.coin_read == LOW)) ////v2.3.4 - For TDS Update in real time
            {
                timer_flags.analog_sensor_update = 0;
                analog_sensor_update();
            }
            else{}
            break;

        case ST_MEMORY_CARD_STATE:
            if (sd_flags.card_detect == 0)
            {
                card_presense_fail_cntr++;
                if (card_presense_fail_cntr > 100)
                {
                    card_presense_fail_cntr = 2;
                }
                else{}
                LED2_ON;
                buzzer(ERR02);
                strcpy(lcd_data, "MEMORY NOT FIND"); //v3.1.1.D
                lcd_display(1);
                strcpy(lcd_data, "CONTACT SARVAJAL"); //3.  1.D
                lcd_display(2);
                timer2_sec_delay(error_display_time); //v3.1.1.D
            }
            else
            {
                if (card_presense_fail_cntr > 1)
                {
                    strcpy(lcd_data, "MEMORY INIT"); //v3.1.1.D
                    lcd_display(1);
                    strcpy(lcd_data, "PLS WAIT..."); //v3.1.1.D
                    lcd_display(2);
                    timer2_sec_delay(error_display_time); //v3.1.1.D
                    card_presense_fail_cntr = 0;
                    file_response = FSInit();
                    if (file_response == 1)
                    {
                        file_create();
                        sd_flags.card_initialize = 1;
                        LED2_OFF;
                    }
                    else if (file_response == 0)
                    {
                        sd_flags.card_initialize = 0;
                        LED2_ON;
                        buzzer(ERR02);
                        clear_lcd();
                        strcpy(lcd_data, "MEMORY FAULT    ");
                        lcd_display(1); //v3.1.1.D
                        strcpy(lcd_data, "CONTACT SARVAJAL");
                        lcd_display(2); //v3.1.1.D
                        timer2_sec_delay(error_display_time); //v3.1.1.D
                    }
                    else{}
                }
                else{}
            }

            break;

        case ST_COIN_FUNCTION_DETECTED:
            if ((sd_flags.card_initialize == 0) || (sd_flags.card_detect == 0)) //v3.1.1.D  //v3.1.1.H
            {
                LED2_ON;
                buzzer(ERR02);
                strcpy(lcd_data, "MEMORY FAULT    "); //v3.1.1.D
                lcd_display(1);
                strcpy(lcd_data, "CONTACT SARVAJAL"); //3.  1.D
                lcd_display(2);
                timer2_sec_delay(error_display_time); //v3.1.1.D
            }
            else
            {
                //    if ((consumer_flags.coin_read == HIGH)&&(sd_flags.card_initialize == 1)) //v2.2.9


                if (consumer_flags.coin_read == HIGH) //2.3.H
                {                    
                     ptr_1 = strchr(coinbox_data, 0x90);

                    if (ptr_1 != 0)
                    {
#ifdef COIN_TEST_EN
                        clear_lcd();
                        sprintf(lcd_data, "R= %d %d", total_coin_count, total_coin_amount);
                        lcd_display(1);
                        sprintf(lcd_data, "S= %d", coin_collector_capacity);
                        lcd_display(2);
                        timer2_sec_delay(4);
#endif
                        LCD_BACKLITE_ON;  //3.1.2.H
                        mode_flags.device_active_awake = TRUE;
                        if ((coin_flags.coinenable == TRUE)&&(peripheral_flags.dev_en_dis == TRUE))
                        {
                            coin_amount = 0;
                            coin_response_check(RECEIVE_COIN);
#ifdef COIN_TEST_EN
                            sprintf(lcd_data, "COIN = %d ", coin_amount);
                            lcd_display(2); //timer2_sec_delay(2);
#endif

                            if (coin_flags.coinselect == TRUE)
                            {
                                buzzer(AUT01); //v2.3.5  
                                clear_lcd();
                                strcpy(lcd_data, "COIN ACCEPTED");
                                lcd_display(1);
                                coinbox_disable();
                                buzzer(AUT01); //v2.3.5
                                dispense_water_via_coin(); //To initialize variables and flag before going into dispense function                          
                            }
                            else
                            {
                                clear_UCA2_data(); //flag disable when coin operation COMPLETE
                                coin_flags.coinselect = FALSE;
                                if (coin_flags.coin_collector_error == 0)
                                    coinbox_init();
                                else{}

                                strcpy(lcd_data, "COIN SENSING");
                                lcd_display(1);
                                strcpy(lcd_data, "ERROR...!!!");
                                lcd_display(2);
                                timer2_sec_delay(error_display_time);
                                consumer_flags.coin_read = LOW;
                            }

                        }
                        else
                        {
                            consumer_flags.coin_read = LOW;
                            clear_lcd();
                            sprintf(lcd_data, "COIN ERROR", coin_amount);
                            lcd_display(2);
                            timer2_sec_delay(5);
                            display_error_mode();
                            clear_UCA2_data();
                            if (coin_flags.coin_collector_error == 0)
                            {
                                coin_flags.coin_debug  = 1;  //v3.1.2.I
                                coinbox_init();
                            }
                            else{}
                        }
                        //				       consumer_flags.coin_read = LOW;    //v2.3.2
                        //			        	UART1_init();
                        //                     consumer_flags.coin_read = LOW;        //3.1.2
                        //                     clear_UCA2_data();                     //3.1.2 

                    }
                    else
                    {
//                        strcpy(lcd_data, "DATA CORRUPT");
//                        lcd_display(1);
//                        timer2_sec_delay(2);
                        consumer_flags.coin_read = 0;
                    }
                    memset(coinbox_data, 0, sizeof (coinbox_data)); 
                    coinbox_counter = 0;
                    LED3_OFF;                    
                }
                else{}
            }
            break;
        default:
            break;
        }
    }
}

void hardware_init(void)
{
    //---RFID detect pin 
    ConfigINT0(INT_ENABLE | FALLING_EDGE_INT | INT_PRI_1); //Enable INT0 interrupt on falling edge with priority 1   //v1.0.0
    Int0_Clear_Intr_Status_Bit; //Clear interrupt status bit

    //--outflow sensor
    ConfigINT1(INT_ENABLE | FALLING_EDGE_INT | INT_PRI_4); //Enable INT1 interrupt on falling edge with priority 1   //v1.0.0
    Int1_Clear_Intr_Status_Bit; //Clear interrupt status bit

    //---switch 4
    ConfigINT2(INT_ENABLE | FALLING_EDGE_INT | INT_PRI_6); //Enable INT2 interrupt on falling edge with priority 1   //v1.0.0
    Int2_Clear_Intr_Status_Bit; //Clear interrupt status bit

    //-------Input Flow Sensor
    INTCON2bits.INT3EP = 0; // Interrupt 3 is configured on rising edge interrupt //v2.2.8
    IPC13 |= 0x0070; //level7 =0x0070 level5= 0x0050	// Interrupt 3 priority set to Level 5	//v2.2.8
    IFS3bits.INT3IF = 0; // Clearing interrupt flag
    IEC3bits.INT3IE = 1; // Interrupt 3 request enable


    mPORTEInputConfig(0x0300); //I/p as RE8 = SW1, RE9 = SW2
    mPORTBInputConfig(0x0030); //I/p as RB5 = SW3	 RB4 = SW4
    //mPORTAInputConfig(0x0010); //I/p as RA4 = SD_Card_Detect  //v3.1.1.H
    TRISAbits.TRISA4 = 1;
    //PORTAbits.RA7=1;

    mPORTFOutputConfig(0x0003); //o/p as RF0 = LED1,  RF1= LED2
    mPORTGOutputConfig(0x0003); //o/p as RG0 = LED3,  RG1= LED4

    mPORTAOutputConfig(0x0002); //RA1 as o/p = solenoid control

    mPORTCInputConfig(0x0018); //RC4 as I/P - Output Flow Sensor, RC3 as I/P - Input Flow Sensor
    mPORTDInputConfig(0x0001); //RD0 as I/p = RFid card detct

    mPORTCInputConfig(0x0006); //RC1 as I/p = HLS ,RC2 as I/p = LLS

    mPORTDInputConfig(0x0002); //RD1 as I/P - Buzzer Pin

    mPORTDOutputConfig(0x6000); //RD13 as o/P - RFID reset Pin, RD14 as O/P - Modem Reset Pin
    mPORTDSetBits(0x2000);

    mPORTCOutputConfig(0x4000); //Chiller_heater_control_pin RC14 as o/p //v3.1.3


    ConfigIntCN(INT_ENABLE | INT_PRI_5);

    EnableCN66; //RE8
    EnableCN67; //RE9
    EnableCN7; //RB5
    EnableCN37; //RA4  //v3.1.1.H

    mPORTFOutputConfig(0x0100); //TP10 RF8 as output
    mPORTDOutputConfig(0x0100); //TP11 RD8as output

    memset(previous_bal_array, '0', sizeof (previous_bal_array)); //start the program array will fill with '0' ascii zeroes  	
    EnableIntInputChange;

}

void mcu_reset_source(void)
{
    reset_source = PwrMgnt_ResetSource();

    device_reset_state = 0;

    if ((reset_source) == POWER_ON_Reset) //1
    {
        //reset source is POR
        device_reset_state |= 0x01;
        mPWRMGNT_Clear_PORbit();
    }
    else
    {
    }

    if (reset_source == BURN_OUT_Reset) //2
    {
        //reset source is brown out
        device_reset_state |= 0x02;
        mPWRMGNT_Clear_BORbit();
    }
    else
    {
    }
    if (reset_source == WATCHDOG_Reset) //4
    {
        //reset source is watchdog timer time out
        device_reset_state |= 0x04;
        mPWRMGNT_Clear_WDTObit();
    }
    else
    {
    }
    if (reset_source == SOFTWARE_Reset) //8
    {
        //reset source is software reset
        device_reset_state |= 0x08;
        mPWRMGNT_Clear_SWRbit();
    }
    else
    {
    }

    if (reset_source == EXTERNAL_Reset) //16
    {
        //reset source is external reset (MCLR)
        device_reset_state |= 0x10;
        mPWRMGNT_Clear_EXTRbit();
    }
    else
    {
    }

    if (reset_source == CFG_WORD_MISMATCH_Reset) //32
    {
        //reset source is due to configuration word mismatch
        device_reset_state |= 0x20;
        mPWRMGNT_Clear_CMbit();
    }
    else
    {
    }
    if (reset_source == ILLEGAL_INSTR_Reset) //64
    {
        //reset source is due to illegal instruction execution
        device_reset_state |= 0x40;
        mPWRMGNT_Clear_IOPUWRbit();
    }
    else
    {
    }

    if (reset_source == TRAP_Reset) //128
    {
        //reset source is due to trap
        device_reset_state |= 0x80;
        mPWRMGNT_Clear_TRAPRbit();
    }
    else{}
}

void variable_init(void)
{
    unsigned int counter = 0;

    for (counter = 0; counter<sizeof (server_ip); counter++)
        server_ip[counter] = 0;

    for (counter = 0; counter<sizeof (server_port); counter++)
        server_port[counter] = 0;

    for (counter = 0; counter<sizeof (apn); counter++)
        apn[counter] = 0;  

    /* TDS,LLS/HLS,DEV EN/DIS,TEMP EN/DIS */
    peripheral_flags.tds_update = TRUE;
    peripheral_flags.tds_en_dis = TRUE;
    peripheral_flags.lls_hls_en_dis = 3; //Both LLS & HLS enabled
    peripheral_flags.lls_trigger = FALSE;
    peripheral_flags.lls_processing = FALSE;
    peripheral_flags.dev_en_dis = TRUE;
    peripheral_flags.temp_en_dis = FALSE;
    error_flags.tank_empty = FALSE;

    /*MODEM,ERROR FLAGS,SOLAR,SYSTEM */
    gsm_flags.sms_permit = TRUE;
    error_flags.modem_sw_reset = FALSE;
    error_flags.tds_error_sent = FALSE;
    error_flags.water_expired = FALSE;
    //mode_flags.soft_mcu_reset = FALSE;
    mode_flags.device_active_awake = TRUE;
    gsm_flags.solar_ack_sent = FALSE;
    gsm_flags.ac_ack_sent = FALSE;
    peripheral_flags.solar_sense = FALSE;
    peripheral_flags.solar_processing = FALSE;
    system_flags.system_stability = FALSE;
    modem_reset_tolerance = 5;

    /*DISPENSE VARIABLES,DATA VALIDATION REFILL, SLEEP MODE */
    total_dispense_litre = 0;
    data_error_validation_code = 0;
    error_flags.data_error_auto_correct = TRUE;
    unknown_access_count = 0; //v2.2.2
    tank_low_level_litre = lit_empty_tank; //v2.2.3
    peripheral_flags.sleep_mode_flag = FALSE; //v2.2.4
    refill_flags.auto_refill_flag = FALSE; //v2.2.4
    school_disp_amount = 500; //v3.1.1.E 500 to 1000
    system_flags.battery_auto_correct = FALSE; //v2.2.7

    /*CHILLER,ACC ID,FR. TAG,BUZZER */
    peripheral_flags.chiller_status_flag = FALSE; //chiller //v2.2.6
    peripheral_flags.heater_status_flag = FALSE; //heater //v2.3.6
    peripheral_flags.chiller_heater_start_flag = TRUE; //v2.3.6
    peripheral_flags.chiller_heater_en_dis = FALSE; //v2.2.6		//v2.3.6
    
    peripheral_flags.chiller_heater_lls_trigger = FALSE; //V2.2.6		//v2.3.6
    peripheral_flags.cold_water = FALSE;
    peripheral_flags.warm_water = FALSE; //v2.3.6
    error_flags.chiller_heater_relay_reset = FALSE;		//v2.3.6
    peripheral_flags.pag_buzzer_control = TRUE;


    system_flags.super_admin = 0; //v2.2.9
    system_flags.ultra_super_admin_state = 0;
    system_flags.ultra_super_admin = 0; //1.0.0
    consumer_flags.rfid_read = 0; //1.0.0
    consumer_flags.card_detect = 0; //1.0.0
    P_month = 0;
    P_date = 0, P_yr = 17; //1.0.1
    system_flags.auto_sync_flag = 1; //1.0.1
    peripheral_flags.temp_compen_en_dis = 0; //v3.1.1.F

    if (PORTAbits.RA4 == 1) //SD_CD pin check if card not presense then pin valt =3.3v(1) otherwise = 0v (0)            //v3.1.1.H     
    {    
        #ifdef OLD_BOARD
                sd_flags.card_detect = 1;
        #else 
                sd_flags.card_detect = 0;
        #endif
    }
    else
    { //v3.1.1.H
        sd_flags.card_detect = 1;
    }

    strcpy(sch_mode_acc_id, "NULL_ACC"); //v2.2.5 //v3.1.1.A
    strcpy(corp_mode_acc_id, "NULL_ACC"); //v2.2.5             //v3.1.1.A
    sch_mode_acc_id[8] = 0; //last byte null character           //v3.1.1.A
    corp_mode_acc_id[8] = 0; //last byte null character      //v3.1.1.A
    //strcpy(franchisee_rfid_tag,"2800757384AA");		//v2.2.5
    strcpy(franchisee_rfid_tag, "721234FB"); //v0.0.1 offline  //3.1.2

    /* FULL 40, IP & PORT, APN, PASSWORDS */
    //	strcpy(server_ip,"115.112.184.207");	//Tata test Server
    //	strcpy(server_port,"9094");				//Tata test Server

    //@	strcpy(server_ip,"46.137.213.51");	//v3.0.0
    //@	strcpy(server_port,"9089");				//v3.0.0

    strcpy(server_ip, "35.154.253.231"); //v3.0.0115.112.184.207
    strcpy(server_port, "9094"); //v3.0.0  


    //strcpy(server_ip,"122.170.012.160");	//Local Server
    //strcpy(server_port,"9091");			//Local Server

    strcpy(apn, "airtelgprs.com"); // Airtel APN
    //strcpy(apn,"www");					// Vodafone APN
    strcpy(current_password, "1234");
    strcpy(factory_password, "WA20"); //WA20 //1.0.1  2016 //v3.1.1.C
    strcpy(menu_franchisee_password, "1234"); //franchisee password

    for (counter = 0; counter<sizeof (transmitData); counter++)
        transmitData[counter] = 0;
    for (counter = 0; counter<sizeof (sms_buffer); counter++)
        sms_buffer[counter] = 0;
    for (counter = 0; counter < AVG_READING_TDS; counter++)
        tds_array[counter] = 0;
    for (counter = 0; counter < MAX_SERVER_STRING; counter++)
        server_response_string[counter] = 0;
    for (counter = 0; counter < TOTAL_ERRORS; counter++)
        error_stack[counter] = 0;
    //***********************************************************////v2.2.9
    //DIR_74HC245_TX;		//v2.2.9 //
    //DIR_74HC245_RX;

    coin_flags.coinidle = FALSE;
    coinbox_counter = 0;
    coin_amount = 0;
    coin_water_tariff = 50.0;
    coin_collector_capacity = 300;
    coin_init_try = 0;

    coin_flags.coinselect = FALSE; //For coinbox
    //	coin_flags.coinenable=FALSE;
    coin_flags.coinidle = FALSE;
    coin_flags.coin_receive_process = FALSE;    
    coin_flags.coin_collector_alert = 0;
    coin_flags.coin_collector_error = 0;
    error_flags.coin_error_sent = FALSE;
    coin_flags.coin_disable_by_fs_error = 0;	//v2.3.6
    coin_flags.coin_disable_fault_alert = 0;	//v2.3.6

    strcpy(coin_id.channel_1, "COIN0001"); //2.3.A
    strcpy(coin_id.channel_2, "COIN0005"); //2.3.A
    strcpy(coin_id.channel_3, "COIN0001"); //2.3.A
    strcpy(coin_id.channel_4, "COIN0002"); //2.3.A
    strcpy(coin_id.channel_5, "COIN0005"); //2.3.A
    strcpy(coin_id.channel_6, "COIN0005"); //2.3.A
    strcpy(coin_id.channel_7, "COIN0010"); //2.3.A
    strcpy(coin_id.channel_8, "COIN00NA");

    //	strcpy(coin_id.channel_1,"CWDCOIN00001");
    //	strcpy(coin_id.channel_2,"CWDCOIN00005");
    //	strcpy(coin_id.channel_3,"CWDCOIN00001");
    //	strcpy(coin_id.channel_4,"CWDCOIN00002");
    //	strcpy(coin_id.channel_5,"CWDCOIN00005");
    //	strcpy(coin_id.channel_6,"CWDCOIN00005");
    //	strcpy(coin_id.channel_7,"CWDCOIN00010");//COIN0010
    //	strcpy(coin_id.channel_8,"CWDCOIN00001");

    consumer_flags.card_detect_first_time = 0; //3.1.2
    peripheral_flags.lls_last_first_state = 0; //v3.1.2.c
    peripheral_flags.lls_last_second_state = 0; //v3.1.2.c
    
    timer_flags.modem_status_check  = 0;  //v3.1.2.J
    timer_flags.modem_status_check_en_dis  = 0;   //v3.1.2.J
    modem_status_check_hour =1; //v3.1.2.J
    modem_status_check_min =0;  //v3.1.2.J
    rfid_read_fail_cntr=0 ; //v3.1.2.J
}

void io_mapping(void)
{
    PPSUnLock;
    //*************SPI for SD CArd 
    PPSInput(PPS_SDI1, PPS_RP16); //TP4
    PPSOutput(PPS_RP30, PPS_SCK1OUT); //TP3
    PPSOutput(PPS_RP17, PPS_SDO1); //TP2
    PPSOutput(PPS_RP10, PPS_SS1OUT); //TP1

    //********************** Assign UART 2 signals (Modem)***********************************

    //----------Demo 2.0 Board
    iPPSOutput(OUT_PIN_PPS_RP29, OUT_FN_PPS_U2TX); //Assign U2TX to pin RP5
    iPPSInput(IN_FN_PPS_U2RX, IN_PIN_PPS_RP5); //Assign U2RX to pin RP29

    //********************** Assign UART 1 signals(RFID Reader) ***********************************

//    ----------Demo 2.0 Board
//    	iPPSOutput(OUT_PIN_PPS_RP20,OUT_FN_PPS_U1TX);	//Assign U1TX to pin RP25
//    	iPPSInput(IN_FN_PPS_U1RX,IN_PIN_PPS_RP25);	//Assign U1RX to pin RP20

    iPPSOutput(OUT_PIN_PPS_RP20, OUT_FN_PPS_U1TX); //Assign U1TX to pin RP25
    iPPSInput(IN_FN_PPS_U1RX, IN_PIN_PPS_RP25); //Assign U1RX to pin RP20

//    ********************** Assign UART 3 signals (Coin Box UCA2 )******************************///v2.2.9
//        iPPSOutput(OUT_PIN_PPS_RP23, OUT_FN_PPS_U3TX); //Assign U3TX to pin RP23
//        iPPSInput(IN_FN_PPS_U3RX, IN_PIN_PPS_RP22); //Assign U3RX to pin RP22

    iPPSOutput(OUT_PIN_PPS_RP22, OUT_FN_PPS_U3TX); //Assign U3TX to pin RP23
    iPPSInput(IN_FN_PPS_U3RX, IN_PIN_PPS_RP23); //Assign U3RX to pin RP22

    //---------------------------- Assign  INT1 (Output Flow Sensor) -----------------------

    iPPSInput(IN_FN_PPS_INT1, IN_PIN_PPS_RPI41); //Assign INT1 on RPI43 pin 

    //---------------------------- Assign  INT2 (Switch 4 ) -----------------------
    iPPSInput(IN_FN_PPS_INT2, IN_PIN_PPS_RP28); //Assign INT2 on RP29 pin 

    //---------------------------- A Assign  INT3 (Input Flow Sensor) -----------------------
    iPPSInput(IN_FN_PPS_INT3, IN_PIN_PPS_RPI40); //Assign INT3 on RPI38 pin 

    //---------------------------- Assign Buzzer Pin (Pin 71, RPI2) -----------------------
    iPPSOutput(OUT_PIN_PPS_RP24, OUT_FN_PPS_OC1); //Rev-A - Assign OC1 to pin RP16

    PPSLock;
}

void check_system_stability(unsigned char display_ctrl)
{
    unsigned char temp = 0;

    lcd_init();

    if (display_ctrl != 0)
    {
        strcpy(lcd_data, "SYSTEM STABILITY");
        lcd_display(1);
        strcpy(lcd_data, "CHECK, PLS. WAIT");
        lcd_display(2);
        timer2_sec_delay(error_display_time);
    }
    else{}

    consumer_flags.freeze_display = FALSE;
//    for (temp = 0; temp < (sizeof (lcd_line1_data_freeze)); temp++)
//    {
//        lcd_line1_data_freeze[temp] = 0;
//    }
     memset(lcd_line1_data_freeze, 0, sizeof (lcd_line1_data_freeze)); //v3.1.1.D

    if ((mode_flags.op_mode != CONSUMER_MODE)&&(mode_flags.op_mode != ERROR_MODE)) //----v2.3.1
    {
        memset(rfid_card, 0, sizeof (rfid_card)); //v3.1.1.B
        memset(scanned_rfid_card, 0, sizeof (scanned_rfid_card)); //v3.1.1.B
        rfid_reader_counter = 0;
        consumer_flags.rfid_read = LOW;
        consumer_flags.card_detect = LOW; //v1.0.0
    }
    else{}

    memset(coinbox_data, 0, sizeof (coinbox_data)); //v3.1.1.D
    coinbox_counter = 0;
    consumer_flags.coin_read = LOW;

    system_flags.system_stability = TRUE;
    if (mode_flags.op_mode == SCHOOL_MODE)
    {
        dispense_button_count = 0;
    }
    else{}
    consumer_flags.dispense_button = 0;
    mcu_auto_reset_counter = 0;

    UART1_init();     //rfid_uart init
  
    if(peripheral_flags.coin_en_dis!=0)
    {
    UART3_init(); //v2.2.9//1.0.2  //coin_uart init 
    }
    if ((coin_flags.coin_disable_by_fs_error==0)&&(peripheral_flags.coin_en_dis)&&(coin_flags.coin_collector_error == 0)&&(peripheral_flags.dev_en_dis == TRUE)&&(mode_flags.op_mode != SCHOOL_MODE)&&(mode_flags.op_mode != CORPORATE_MODE)&&(mode_flags.op_mode != DEBUG_MODE)&&(mode_flags.op_mode != DIAGNOSTIC_MODE)&&(mode_flags.op_mode != ANTENNA_MODE)&&(mode_flags.op_mode != ERROR_MODE)) //v6
    { //v2.3.4
        coinbox_enable();
    }else{}
    
    if (mode_flags.forced_function_state == FALSE)  //v2.3.5 //do_finalize
    { 
        clear_lcd(); //v2.3.5
        LCD_BACKLITE_OFF; //v2.3.5
    }else{}

    peripheral_flags.lcd_refresh = TRUE;
    button_detect = 0; //3.1.0 
    consumer_flags.dispense_button = 0; //3.1.0
}

void system_error_check(void)
{
    unsigned char cnt = 0;

    system_error = ERROR_FREE;

    for (cnt = (TOTAL_ERRORS - 1); cnt > 0; cnt--)
    {
        if (cnt == TANK_EMPTY_ER) //v2.2.2	//Do not consider error mode when tank empty
            cnt--;
        else{}

        //if(error_stack[cnt]==TRUE)
        if (error_stack[cnt] >= 1)
        {
            if (mode_flags.op_mode != ERROR_MODE)
            {
                mode_flags.prev_op_mode = mode_flags.op_mode;
                mode_flags.op_mode = ERROR_MODE;
                //	if(peripheral_flags.coin_en_dis){coinbox_enable();} else{}
            }
            else{}
            system_error = cnt;
        }
        else{}
    }

    if ((system_error == ERROR_FREE) && (mode_flags.op_mode == ERROR_MODE))
    {
        mode_flags.op_mode = mode_flags.prev_op_mode;
        //	if(peripheral_flags.coin_en_dis){coinbox_disable();} else{}
    }
    else{}

       	if(error_stack[TANK_EMPTY_ER]>=1)	//v2.2.2   //3.1.2.H
    		system_error = TANK_EMPTY_ER;
    	else{}

    switch (system_error)
    {
    case ERROR_FREE:
        strcpy(error_state, "0000");
        break;

    case WATER_LEAK_ER:
        strcpy(error_state, "RL04");
        break;

    case HIGH_TDS_ER:
        strcpy(error_state, "RL01");
        break;

    case PAG_ERROR:
        //@			strcpy(error_state,"RL15");
        break;

    case LLS_TRIG_ER:
        strcpy(error_state, "RL11");
        break;

    case NETWORK_FAIL_ER:
        strcpy(error_state, "NWER");
        break;

    case SERVER_FAIL_ER:
        strcpy(error_state, "SFER");
        break;

    case MODEM_HANG_ER:
        strcpy(error_state, "MDER");
        break;

        //---case SIM_FAIL_ER:
    case TANK_EMPTY_ER: //v2.2.2
        strcpy(error_state, "RL16");
        break;

    case SV_ER:
        strcpy(error_state, "RL14");
        break;

    case FLOW_SENS_ER:
        strcpy(error_state, "RL13");
        break;

    case UV_FAIL_ER:
        //strcpy(error_state,"RLUV");
        sprintf(error_state, "TR0%u", error_stack[UV_FAIL_ER]);
        send_server_response(DEV_TRAP_ERROR);
        send_server_response(DEV_HB_MSG);

        timer2_ms_delay(100);
        lcd_init();
        sprintf(lcd_data, "TRAP ERROR..!!  ");
        lcd_display(1);
        sprintf(lcd_data, "SYSTEM RESTART  ");
        lcd_display(2);
        buzzer(ERR02);
        timer2_sec_delay(error_display_time);
        abort(); //reset uC
        break;

    case LOW_TDS_ER:
        strcpy(error_state, "RL02");
        break;

    case TEMP_SENSOR_FAIL_ER:
        strcpy(error_state, "RL26");
        break;

    case DEV_DISABLED:
        strcpy(error_state, "RA14");
        break;

    default:
        break;
    }

    //	if((mode_flags.op_mode == ERROR_MODE) && ((peripheral_flags.chiller_status_flag==TRUE)))
    //	{
    //			strcpy(lcd_data,"CHILLER OFF ER.");
    //			lcd_display(1);
    //			timer2_sec_delay(error_display_time);
    //			CHILLER_HEATER_OFF;
    //			peripheral_flags.chiller_status_flag=FALSE;
    //	}

    //@		system_error = ERROR_FREE; //3.1.0
    //		if((system_error==ERROR_FREE) && (mode_flags.op_mode==ERROR_MODE)){  //3.1.0
    //			mode_flags.op_mode = mode_flags.prev_op_mode;
    //				memset(error_stack,0,sizeof(error_stack));  //3.1.0
    //				eeprom_write(SER_ERROR_STACK);	         //3.1.0		
    //		}
    //@		else{}	


}

void display_error_mode(void)
{
    lcd_init();
    LED1_ON;
    switch (system_error)
    {
    case FLOW_SENS_ER:
        //strcpy(lcd_data,"TRY IN 5 MIN #FS");
        strcpy(lcd_data, "FLOW SENSE ERROR");    
        system_error = ERROR_FREE; //v3.0.0
        error_stack[FLOW_SENS_ER] = FALSE;
        break;

    case WATER_LEAK_ER:
        strcpy(lcd_data, "WATER LEAK");
        system_error = ERROR_FREE; //v3.0.0
        break;

    case HIGH_TDS_ER:
        strcpy(lcd_data, "HIGH TDS");
        system_error = ERROR_FREE; //v3.0.0
        error_stack[HIGH_TDS_ER] = FALSE;   
        break;

    case LOW_TDS_ER:
        strcpy(lcd_data, "LOW TDS");
        system_error = ERROR_FREE; //v3.0.0
        error_stack[LOW_TDS_ER] = FALSE;  
        break;

    case LLS_TRIG_ER:
        strcpy(lcd_data, "LOW WATER LEVEL");
        system_error = ERROR_FREE; //v3.0.0
        
        break;

    case TEMP_SENSOR_FAIL_ER:
        strcpy(lcd_data, "TEMP SENSOR FAIL");
        system_error = ERROR_FREE; //v3.0.0
        break;


    case SERVER_FAIL_ER:
        strcpy(lcd_data, "SERVER FAILURE");
        system_error = ERROR_FREE; //v3.0.0
        break;

    case NETWORK_FAIL_ER:
        strcpy(lcd_data, "NETWORK FAILURE");
        system_error = ERROR_FREE; //v3.0.0
        break;

    case MODEM_HANG_ER:
        strcpy(lcd_data, "MODEM FAILURE");
        system_error = ERROR_FREE; //v3.0.0
        break;

    case UV_FAIL_ER:
        sprintf(lcd_data, "TRAP ERROR: %u  ", error_stack[UV_FAIL_ER]);
        system_error = ERROR_FREE; //v3.0.0
        break;

        //---case SIM_FAIL_ER:
        //strcpy(lcd_data,"SIM FAILURE     ");
    case TANK_EMPTY_ER: //v2.2.2
        strcpy(lcd_data, "TANK EMPTY");
        system_error = ERROR_FREE; //v3.0.0
        error_stack[TANK_EMPTY_ER] = FALSE;  //v3.1.2.H
        break;

    case PAG_ERROR:
        //	strcpy(lcd_data,"PAG FAILURE");
        //	system_error = ERROR_FREE;		//v3.0.0
        break;

    case DEV_DISABLED:
        if (error_flags.water_expired == TRUE)
            strcpy(lcd_data, "WATER EXPIRED");
        else
            strcpy(lcd_data, "DEVICE DISABLED");
        system_error = ERROR_FREE; //v3.0.0
        break;

    case SV_ER:
        //	if((peripheral_flags.lls_hls_en_dis>=2)&&(peripheral_flags.lls_trigger==TRUE))
        //		strcpy(lcd_data,"TANK EMPTY   #SV");
        //	else
        //	{
        //		//strcpy(lcd_data,"SOLENOID FAILURE");
        //		strcpy(lcd_data,"TRY IN 5 MIN #SV");
        //	}
        system_error = ERROR_FREE; //v3.0.0
        error_stack[SV_ER] = FALSE;
        break;

    default:
        //strcpy(lcd_data,"ERROR: PLS.");
        //@			sprintf(lcd_data,"ERROR: PLS.:%u",system_error);   //3.1.0
        system_error = ERROR_FREE; //v3.0.0
        break;
    }

    if (mode_flags.op_mode == DIAGNOSTIC_MODE)
    {
        strcpy(lcd_data, "DIAGNOSTIC MODE ON");
    }
    else{}
    lcd_display(1);
    timer2_sec_delay(error_display_time);

    eeprom_write(SER_ERROR_STACK); //v3.0.0
    if (system_error == ERROR_FREE) //v3.0.0
        system_error_check(); //v3.0.0
}

void unauthorized_water_input_check(void)
{
    if (refill_flags.auto_refill_flag == FALSE)
    {
        if (refill_flags.input_flow_sensed == HIGH)
        {
            if ((timer2_tick - in_leak_time_stamp) >= (leak_timeout * 1000))
            {
                if (input_flow_count >= ((int) (3 * (in_flow_calibration * DISPENSE_LEAST_COUNT))))
                {
                    buzzer(ERR02);
                    lcd_init();
                    strcpy(lcd_data, "UNAUTHORIZED");
                    lcd_display(1);
                    strcpy(lcd_data, "WATER INPUT");
                    lcd_display(2);

                    unauth_input_litre = 0;
                    unauth_input_litre += ((input_flow_count) / ((float) (in_flow_calibration)));
                    input_flow_count = 0;
                    refill_flags.input_flow_sensed = 0;

                    send_server_response(DEV_UNAUTHORISED_WATER_INPUT);
                    unauth_input_litre = 0;
                    unknown_access_count++;
                    if (unknown_access_count >= leak_timeout)
                        unknown_access_alert();
                    else{}
                    timer2_sec_delay(error_display_time); //3.1.0
                }
                else
                {
                    refill_flags.input_flow_sensed = LOW;
                    input_flow_count = 0;
                }
            }
            else{}
        }
        else
        {
            in_leak_time_stamp = timer2_tick;
        }
    }
    else
    {
        if (refill_flags.input_flow_sensed)
        {
            refill_flags.input_flow_sensed = LOW;
            if (input_flow_count >= ((int) (in_flow_calibration * DISPENSE_LEAST_COUNT)))
            {
                refill_litre += ((input_flow_count) / ((float) (in_flow_calibration)));
                input_flow_count = 0;
            }
            else{}
            //-------debug purpose
            /*lcd_init();
            strcpy(lcd_data,"REFILLING:");
            lcd_display(1);
            sprintf(lcd_data,"%3.1f L",((double)refill_litre));
            lcd_display(2);
            timer2_sec_delay(error_display_time);	*/

            //-------debug purpose
        }
        else{}
        if (((sec_tick - refill_time_stamp) >= refill_timeout)&&(refill_litre >= 10))
        {
            //call refill_mode() with specific var. init.
            rfid_card_type = INVALID_CARD;
            lcd_init();
            strcpy(lcd_data, "PLS. WAIT");
            lcd_display(1);
            strcpy(lcd_data, "PROCESSING...");
            lcd_display(2);
            timer2_sec_delay(error_display_time);
            refill_mode();
        }
        else{}
    }
}

void unknown_access_alert(void) //v2.2.2
{
    lcd_init();
    unauth_input_litre = 0.0;
    in_leak_time_stamp = timer2_tick;
    while ((timer2_tick - in_leak_time_stamp) <= (leak_timeout * 1000))
    {
        if (refill_flags.input_flow_sensed == HIGH)
        {
            refill_flags.input_flow_sensed = LOW;
            if (input_flow_count >= ((int) (in_flow_calibration * DISPENSE_LEAST_COUNT)))
            {
                unauth_input_litre += ((input_flow_count) / ((float) (in_flow_calibration)));
                input_flow_count = 0;
            }
            else{}
            buzzer(AUT03);
            sprintf(disp_buffer, "%4.1f", ((double) unauth_input_litre));
            strcpy(lcd_data, "STOP REFILLING");
            lcd_display(1);
            sprintf(lcd_data, "INPUT:%sL", disp_buffer);
            lcd_display(2);
            in_leak_time_stamp = timer2_tick;

            if (system_flags.send_idle_msg == TRUE)
            {
                strcpy(lcd_data, "PLS. WAIT...    ");
                lcd_display(1);
                send_server_response(DEV_HB_MSG);
                system_flags.send_idle_msg = FALSE;
            }
            else{}
        }
        else{}

    }

    if (unauth_input_litre > 30)
    {
        error_flags.water_expired = TRUE;
        eeprom_write(SER_WATER_EXPIRED);
        timer2_ms_delay(100);

        buzzer(ERR02);
        lcd_init();
        strcpy(lcd_data, "WATER EXPIRED   ");
        lcd_display(1);
        strcpy(lcd_data, "DEVICE DISABLED ");
        lcd_display(2);
        timer2_sec_delay(error_display_time);
    }
    else{}

    //@	check_server_connection();
    //	while(system_flags.server_connection!=SERVER_CONN_OK)
    //	{
    //		system_flags.disp_led_state = DISPENSE_LED_STATE_4;
    //		connect_to_server();
    //		check_server_connection();
    //@	}
    send_server_response(DEV_UNAUTHORISED_WATER_INPUT);
    unauth_input_litre = 0;
    unknown_access_count = 0;
    send_server_response(DEV_HB_MSG);
}

unsigned char data_validation(unsigned int variable)
{
    unsigned char data_error = 1, loop = 0; //val_loop=0; //v3.1.1.C
    float data_valid_float = 0.0;
    unsigned int data_valid_int = 0;

    eeprom_access = EEPROM_DATABASE_CHECK;
    for (loop = 0; loop<sizeof (data_error_buffer); loop++)
        data_error_buffer[loop] = 0;

    data_error_validation_code = 0;
    data_error_correct_value = 0;

    validate_para_cnt = variable;
    switch (variable)
    {
    case SER_TDS_CALIB:                                                    //1
        while (data_error <= 5)
        {
            if ((tds_calib_fact < 1) || (tds_calib_fact > 500))
            {
                data_error_validation_code = SER_TDS_CALIB;
                data_error_correct_value = DEV_TDS_CALIB;

                if (data_error == 1)
                    sprintf(data_error_buffer, "%.5u", tds_calib_fact);
                else{}

                if (error_flags.data_error_auto_correct == TRUE)
                {
                    if ((eeprom_access != EEPROM_READ)&&(data_error <= 4))
                    {
                        read_eeprom_parameter(SER_TDS_CALIB);
                        data_error++;
                    }
                    else
                    {
                        tds_calib_fact = 8;
                        data_error = 0xAA; //to display error at function end
                        eeprom_write(SER_TDS_CALIB);
                    }
                }
                else
                {
                    data_error = 0xAA; //to display error at function end
                }
            }
            else
            {
                //tds_calib_fact = tds_calib_fact;
                data_error = 10;
            }
        }
        break;

    case SER_OUT_FLOW_CALIB:                                               //2
        while (data_error <= 5)
        {
            if ((out_flow_calibration < 25) || (out_flow_calibration > 5000))
            {
                data_error_validation_code = SER_OUT_FLOW_CALIB;
                data_error_correct_value = DEV_OUT_FLOW_CALIB;

                if (data_error == 1)
                    sprintf(data_error_buffer, "%.5u", out_flow_calibration);
                else{}
                if (error_flags.data_error_auto_correct == TRUE)
                {
                    if ((eeprom_access != EEPROM_READ)&&(data_error <= 4))
                    {
                        read_eeprom_parameter(SER_OUT_FLOW_CALIB);
                        data_error++;
                    }
                    else
                    {
                        out_flow_calibration = 250;
                        data_error = 0xAA; //to display error at function end
                        eeprom_write(SER_OUT_FLOW_CALIB);
                    }
                }
                else
                {
                    data_error = 0xAA; //to display error at function end
                }
            }
            else
            {
                //out_flow_calibration = out_flow_calibration;
                data_error = 10;
            }
        }
        break;
    case SER_IN_FLOW_CALIB:                                              //3
        while (data_error <= 5)
        {
            if ((in_flow_calibration < 25) || (in_flow_calibration > 5000))
            {
                data_error_validation_code = SER_IN_FLOW_CALIB;
                data_error_correct_value = DEV_IN_FLOW_CALIB;

                if (data_error == 1)
                    sprintf(data_error_buffer, "%.5u", in_flow_calibration);
                else{}
                if (error_flags.data_error_auto_correct == TRUE)
                {
                    if ((eeprom_access != EEPROM_READ)&&(data_error <= 4))
                    {
                        read_eeprom_parameter(SER_IN_FLOW_CALIB);
                        data_error++;
                    }
                    else
                    {
                        in_flow_calibration = 250;

                        data_error = 0xAA; //to display error at function end
                        eeprom_write(SER_IN_FLOW_CALIB);
                    }
                }
                else
                {
                    data_error = 0xAA; //to display error at function end
                }
            }
            else
            {
                //in_flow_calibration = in_flow_calibration;
                data_error = 10;
            }
        }
        break;
    case SER_SERVER_IP:                                                   //4
        while (data_error <= 5)
        {
            loop = strlen(server_ip);
            if ((loop <= 6) || (loop >= 16))
                data_error = 2;
            else
            {
                loop = 0;
                data_error = 1;
                for (loop = 0; loop <= ((strlen(server_ip)) - 1); loop++)
                {
                    if (!((server_ip[loop] >= '.') && (server_ip[loop] <= '9')))
                        data_error = 2;
                    else{}
                }
            }

            if (data_error > 1)
            {
                data_error_validation_code = SER_SERVER_IP;
                data_error_correct_value = DEV_SYSTEM_SETTINGS_3;

                if (data_error == 2)
                    sprintf(data_error_buffer, "%.20s", server_ip);
                else{}

                if (error_flags.data_error_auto_correct == TRUE)
                {
                    if ((eeprom_access != EEPROM_READ)&&(data_error <= 4))
                    {
                        read_eeprom_parameter(SER_SERVER_IP);
                        data_error++;
                    }
                    else
                    {
                        strcpy(server_ip, "122.170.012.160");

                        data_error = 0xAA; //to display error at function end
                        eeprom_write(SER_SERVER_IP);
                    }
                }
                else
                {
                    data_error = 0xAA; //to display error at function end
                }
            }
            else
            {
                data_error = 10;
            }
        }
        break;

    case SER_SERVER_PORT:                                                  //5
        while (data_error <= 5)
        {
            loop = strlen(server_port);
            if ((loop < 1) || (loop >= 6))
                data_error = 2;
            else
            {
                loop = 0;
                data_error = 1;
                unsigned char length = (strlen(server_port));
                ;
                for (loop = 0; loop < length; loop++)
                {
                    if (!((server_port[loop] >= '0') && (server_port[loop] <= '9')))
                    {
                        data_error = 2;
                    }
                    else{}
                }
            }
            if (data_error > 1)
            {
                data_error_validation_code = SER_SERVER_PORT;
                data_error_correct_value = DEV_SYSTEM_SETTINGS_3;

                if (data_error == 1)
                    sprintf(data_error_buffer, "%.6s", server_port);
                else{}

                if (error_flags.data_error_auto_correct == TRUE)
                {

                    if ((eeprom_access != EEPROM_READ)&&(data_error <= 3))
                    {
                        read_eeprom_parameter(SER_SERVER_PORT);
                        data_error++;
                    }
                    else
                    {
                        strcpy(server_port, "9094");

                        data_error = 0xAA; //to display error at function end
                        eeprom_write(SER_SERVER_PORT);
                    }
                }
                else
                {
                    data_error = 0xAA; //to display error at function end
                }
            }
            else
            {
                data_error = 10;
            }
        }
        break;
    case SER_SERVER_APN:                                                   //6
        while (data_error <= 5)
        {
            if ((strlen(apn) < 1) || ((strlen(apn))>(sizeof (apn))))
            {
                data_error_validation_code = SER_SERVER_APN;
                data_error_correct_value = DEV_SYSTEM_SETTINGS_3;

                if (data_error == 1)
                    sprintf(data_error_buffer, "%.25s", apn);  ////v3.2.0  20 to 25
                else{}

                if (error_flags.data_error_auto_correct == TRUE)
                {
                    if ((eeprom_access != EEPROM_READ)&&(data_error <= 3))
                    {
                        read_eeprom_parameter(SER_SERVER_APN);
                        data_error++;
                    }
                    else
                    {
                        strcpy(apn, "airtelgprs.com");

                        data_error = 0xAA; //to display error at function end
                        eeprom_write(SER_SERVER_APN);
                    }
                }
                else
                {
                    data_error = 0xAA; //to display error at function end
                }
            }
            else
            {
                data_error = 10;
            }
        }
        break;
    case SER_TDS_MAX:                                                    //7
        while (data_error <= 5)
        {
            if ((max_tds < 50) || (max_tds > 5000))
            {
                data_error_validation_code = SER_TDS_MAX;
                data_error_correct_value = DEV_TDS_MAX;

                if (data_error == 1)
                    sprintf(data_error_buffer, "%.5u", max_tds);
                else{}

                if (error_flags.data_error_auto_correct == TRUE)
                {
                    if ((eeprom_access != EEPROM_READ)&&(data_error <= 3))
                    {
                        read_eeprom_parameter(SER_TDS_MAX);
                        data_error++;
                    }
                    else
                    {
                        max_tds = 150;

                        data_error = 0xAA; //to display error at function end
                        eeprom_write(SER_TDS_MAX);
                    }
                }
                else
                {
                    data_error = 0xAA; //to display error at function end
                }
            }
            else
            {
                //max_tds = max_tds;
                data_error = 10;
            }
        }
        break;

    case SER_TDS_MIN:                                                       //8
        while (data_error <= 5)
        {
            if ((min_tds <= 0) || (min_tds > 1000))
            {
                data_error_validation_code = SER_TDS_MIN;
                data_error_correct_value = DEV_TDS_MIN;

                if (data_error == 1)
                    sprintf(data_error_buffer, "%.5u", min_tds);
                else{}

                if (error_flags.data_error_auto_correct == TRUE)
                {
                    if ((eeprom_access != EEPROM_READ)&&(data_error <= 3))
                    {
                        read_eeprom_parameter(SER_TDS_MIN);
                        data_error++;
                    }
                    else
                    {
                        min_tds = 20;

                        data_error = 0xAA; //to display error at function end
                        eeprom_write(SER_TDS_MIN);
                    }
                }
                else
                {
                    data_error = 0xAA; //to display error at function end
                }
            }
            else
            {
                //min_tds = min_tds;
                data_error = 10;
            }
        }
        break;
    case SER_USER_TIMEOUT:                                                  //9
        while (data_error <= 5)
        {
            if ((user_timeout < 5) || (user_timeout > 1000))
            {
                data_error_validation_code = SER_USER_TIMEOUT;
                data_error_correct_value = DEV_USER_TIMEOUT;

                if (data_error == 1)
                    sprintf(data_error_buffer, "%.5u", user_timeout);
                else{}

                if (error_flags.data_error_auto_correct == TRUE)
                {
                    if ((eeprom_access != EEPROM_READ)&&(data_error <= 3))
                    {
                        read_eeprom_parameter(SER_USER_TIMEOUT);
                        data_error++;
                    }
                    else
                    {
                        user_timeout = 30;

                        data_error = 0xAA; //to display error at function end
                        eeprom_write(SER_USER_TIMEOUT);
                    }
                }
                else
                {
                    data_error = 0xAA; //to display error at function end
                }
            }
            else
            {
                //user_timeout = user_timeout;
                data_error = 10;
            }
        }
        break;

    case SER_DISP_TIMEOUT:                                                  //10
        while (data_error <= 5)
        {
            if ((dispense_timeout < 5) || (dispense_timeout > 1000))
            {
                data_error_validation_code = SER_DISP_TIMEOUT;
                data_error_correct_value = DEV_DISP_TIMEOUT;

                if (data_error == 1)
                    sprintf(data_error_buffer, "%.5u", dispense_timeout);
                else{}

                if (error_flags.data_error_auto_correct == TRUE)
                {
                    if ((eeprom_access != EEPROM_READ)&&(data_error <= 3))
                    {
                        read_eeprom_parameter(SER_DISP_TIMEOUT);
                        data_error++;
                    }
                    else
                    {
                        dispense_timeout = 15;

                        data_error = 0xAA; //to display error at function end
                        eeprom_write(SER_DISP_TIMEOUT);
                    }
                }
                else
                {
                    data_error = 0xAA; //to display error at function end
                }
            }
            else
            {
                //dispense_timeout = dispense_timeout;
                data_error = 10;
            }
        }
        break;

//    case SER_SLEEP_MODE_FLAG:                                               //11
//        break;
//
//    case SER_DEVICE_EN_DIS:                                                 //12
//        break;
//
//    case SER_TDS_EN_DIS: //Enable/Disable TDS Sensor                        //13
//        break;

    case SER_LLS_HLS_EN_DIS: //Enable/Disable LLS		              //14
        while (data_error <= 5)
        {
            if ((peripheral_flags.lls_hls_en_dis < 0) || (peripheral_flags.lls_hls_en_dis > 3))
            {
                data_error_validation_code = SER_LLS_HLS_EN_DIS;
                data_error_correct_value = DEV_LLS_EN_DIS;

                if (data_error == 1)
                    sprintf(data_error_buffer, "%u", peripheral_flags.lls_hls_en_dis);
                else{}

                if (error_flags.data_error_auto_correct == TRUE)
                {
                    if ((eeprom_access != EEPROM_READ)&&(data_error <= 3))
                    {
                        read_eeprom_parameter(SER_LLS_HLS_EN_DIS);
                        data_error++;
                    }
                    else
                    {
                        peripheral_flags.lls_hls_en_dis = 3;

                        data_error = 0xAA; //to display error at function end
                        eeprom_write(SER_LLS_HLS_EN_DIS);
                    }
                }
                else
                {
                    data_error = 0xAA; //to display error at function end
                }
            }
            else
            {
                data_error = 10;
            }
        }
        break;

   
    case SER_SCHOOL_DISPENSE_AMT: //16
        while (data_error <= 5)
        {
            if ((school_disp_amount < 200) || (school_disp_amount > 5000))
            {
                data_error_validation_code = SER_SCHOOL_DISPENSE_AMT;
                data_error_correct_value = DEV_SCHOOL_DISPENSE_AMT;

                if (data_error == 1)
                    sprintf(data_error_buffer, "%.5u", school_disp_amount);
                else{}

                if (error_flags.data_error_auto_correct == TRUE)
                {
                    if ((eeprom_access != EEPROM_READ)&&(data_error <= 3))
                    {
                        read_eeprom_parameter(SER_SCHOOL_DISPENSE_AMT);
                        data_error++;
                    }
                    else
                    {
                        school_disp_amount = 500;

                        data_error = 0xAA; //to display error at function end
                        eeprom_write(SER_SCHOOL_DISPENSE_AMT);
                    }
                }
                else
                {
                    data_error = 0xAA; //to display error at function end
                }
            }
            else
            {
                //school_disp_amount = school_disp_amount;
                data_error = 10;
            }
        }
        break;

    case SER_ER_DISPLAY_TIMER:                                            //17
        while (data_error <= 5)
        {
            if ((error_display_time < 1) || (error_display_time > 10))
            {
                data_error_validation_code = SER_ER_DISPLAY_TIMER;
                data_error_correct_value = DEV_ER_DISPLAY_TIMER;

                if (data_error == 1)
                    sprintf(data_error_buffer, "%.5u", error_display_time);
                else{}
                if (error_flags.data_error_auto_correct == TRUE)
                {
                    if ((eeprom_access != EEPROM_READ)&&(data_error <= 3))
                    {
                        read_eeprom_parameter(SER_ER_DISPLAY_TIMER);
                        data_error++;
                    }
                    else
                    {
                        error_display_time = 3;

                        data_error = 0xAA; //to display error at function end
                        eeprom_write(SER_ER_DISPLAY_TIMER);
                    }
                }
                else
                {
                    data_error = 0xAA; //to display error at function end
                }
            }
            else
            {
                //error_display_time = error_display_time;
                data_error = 10;
            }
        }
        break;

//    case SER_DISPENSE_LT_LC:                                               //18
//        	while (data_error <= 5)  //v3.1.1.E
//        	{
//        	    if ((DISPENSE_LEAST_COUNT < 0.1) || (DISPENSE_LEAST_COUNT > 1.0))
//        	    {
//        		data_error_validation_code = SER_DISPENSE_LT_LC;
//        		data_error_correct_value = DEV_DISPENSE_LT_LC;
//        
//        		if (data_error == 1)
//        		    sprintf(data_error_buffer, "%f", DISPENSE_LEAST_COUNT);
//        		else{}
//        
//        		if (error_flags.data_error_auto_correct == TRUE)
//        		{
//        		    if ((eeprom_access != EEPROM_READ)&&(data_error <= 3))
//        		    {
//        			read_eeprom_parameter(SER_DISPENSE_LT_LC);
//        			data_error++;
//        		    } else
//        		    {
//        			DISPENSE_LEAST_COUNT = 0.1;
//        
//        			data_error = 0xAA; //to display error at function end
//        			eeprom_write(SER_DISPENSE_LT_LC);
//        		    }
//        		} else
//        		{
//        		    data_error = 0xAA; //to display error at function end
//        		}
//        	    } else
//        	    {
//        		//DISPENSE_LEAST_COUNT = DISPENSE_LEAST_COUNT;
//        		data_error = 10;
//        	    }
//        	}     //v3.1.1.E
//        break;

    case SER_NEW_PASSWORD_SET: //Change SMS Password-19                    //19
        while (data_error <= 5)
        {
            if (((strlen(current_password)) <= 2) || ((strlen(current_password)) > 4))
            {
                data_error_validation_code = SER_NEW_PASSWORD_SET;
                data_error_correct_value = DEV_NEW_PASSWORD_SET;

                if (data_error == 1)
                    sprintf(data_error_buffer, "%.5s", current_password);
                else{}

                if (error_flags.data_error_auto_correct == TRUE)
                {
                    if ((eeprom_access != EEPROM_READ)&&(data_error <= 3))
                    {
                        read_eeprom_parameter(SER_NEW_PASSWORD_SET);
                        data_error++;
                    }
                    else
                    {
                        strcpy(current_password, "1234");

                        data_error = 0xAA; //to display error at function end
                        eeprom_write(SER_NEW_PASSWORD_SET);
                    }
                }
                else
                {
                    data_error = 0xAA; //to display error at function end
                }
            }
            else
            {
                data_error = 10;
            }
        }
        break;

    case SER_LOGOUT_IDLE_TIMEOUT:                                          //20
        while (data_error <= 5)
        {
            if ((logout_idle_time < 1) || (logout_idle_time > 10))
            {
                data_error_validation_code = SER_LOGOUT_IDLE_TIMEOUT;
                data_error_correct_value = DEV_LOGOUT_IDLE_TIMEOUT;

                if (data_error == 1)
                    sprintf(data_error_buffer, "%.5u", logout_idle_time);
                else{}

                if (error_flags.data_error_auto_correct == TRUE)
                {
                    if ((eeprom_access != EEPROM_READ)&&(data_error <= 3))
                    {
                        read_eeprom_parameter(SER_LOGOUT_IDLE_TIMEOUT);
                        data_error++;
                    }
                    else
                    {
                        logout_idle_time = 3;
                        data_error = 0xAA; //to display error at function end
                        eeprom_write(SER_LOGOUT_IDLE_TIMEOUT);
                    }
                }
                else
                {
                    data_error = 0xAA; //to display error at function end
                }
            }
            else
            {
                //logout_idle_time = logout_idle_time;
                data_error = 10;
            }
        }
        break;

    case SER_LT_EMPTY_TANK:                                                 //21
        if (!((peripheral_flags.lls_hls_en_dis >= 2)&&(peripheral_flags.lls_trigger == TRUE)))
        {
            while (data_error <= 5)
            {
                if ((lit_empty_tank < 1) || (lit_empty_tank > 1000))
                {
                    data_error_validation_code = SER_LT_EMPTY_TANK;
                    data_error_correct_value = DEV_LT_EMPTY_TANK;

                    if (data_error == 1)
                        sprintf(data_error_buffer, "%.5u", lit_empty_tank);
                    else{}

                    if (error_flags.data_error_auto_correct == TRUE)
                    {
                        if ((eeprom_access != EEPROM_READ)&&(data_error <= 3))
                        {
                            read_eeprom_parameter(SER_LT_EMPTY_TANK);
                            data_error++;
                        }
                        else
                        {
                            lit_empty_tank = 50;

                            data_error = 0xAA; //to display error at function end
                            eeprom_write(SER_LT_EMPTY_TANK);
                        }
                    }
                    else
                    {
                        data_error = 0xAA; //to display error at function end
                    }
                }
                else
                {
                    //lit_empty_tank = lit_empty_tank;
                    data_error = 10;
                }
            }
        }
        break; 

    case SER_FACT_PASSWORD_RESET: //Factory Password Reset                 //23     //v3.1.1.C 
        while (data_error <= 5)
        {
            if (!((factory_password[0] == 'W')&&(factory_password[1] == 'A')&&(factory_password[2] == '2')&&(factory_password[3] == '0')))
            {
                data_error_validation_code = SER_FACT_PASSWORD_RESET;
                data_error_correct_value = DEV_NEW_PASSWORD_SET;

                if (data_error == 1)
                    sprintf(data_error_buffer, "%.5s", factory_password);
                else{}

                if ((eeprom_access != EEPROM_READ)&&(data_error <= 3))
                {
                    read_eeprom_parameter(SER_FACT_PASSWORD_RESET);
                    data_error++;
                }
                else
                {
                    strcpy(factory_password, "WA20");
                    data_error = 0xAA; //to display error at function end
                    eeprom_write(SER_FACT_PASSWORD_RESET);
                }
            }
            else
            {
                data_error = 10;
            }
        }
        break;

    case SER_IDLE_MSG_FREQ: //Idle Message Frequency	                  //27
        while (data_error <= 5)
        {
            if ((idle_msg_freq < 1) || (idle_msg_freq > 1440))
            {
                data_error_validation_code = SER_IDLE_MSG_FREQ;
                data_error_correct_value = DEV_HB_MSG;

                if (data_error == 1)
                    sprintf(data_error_buffer, "%.5u", idle_msg_freq);
                else{}

                if (error_flags.data_error_auto_correct == TRUE)
                {
                    if ((eeprom_access != EEPROM_READ)&&(data_error <= 3))
                    {
                        read_eeprom_parameter(SER_IDLE_MSG_FREQ);
                        data_error++;
                    }
                    else
                    {
                        idle_msg_freq = 20;
                        data_error = 0xAA; //to display error at function end
                        eeprom_write(SER_IDLE_MSG_FREQ);
                    }
                }
                else
                {
                    data_error = 0xAA; //to display error at function end
                }
            }
            else
            {
                //idle_msg_freq = idle_msg_freq;
                data_error = 10;
            }
        }
        break;

    case SER_DUMP_TIMEOUT:                                              //30
        while (data_error <= 5)
        {
            if ((dump_timeout < 10) || (dump_timeout > 120))
            {
                data_error_validation_code = SER_DUMP_TIMEOUT;
                data_error_correct_value = DEV_DUMP_TIMEOUT;

                if (data_error == 1)
                    sprintf(data_error_buffer, "%.5u", dump_timeout);
                else{}

                if (error_flags.data_error_auto_correct == TRUE)
                {
                    if ((eeprom_access != EEPROM_READ)&&(data_error <= 3))
                    {
                        read_eeprom_parameter(SER_DUMP_TIMEOUT);
                        data_error++;
                    }
                    else
                    {
                        dump_timeout = 20;
                        data_error = 0xAA; //to display error at function end
                        eeprom_write(SER_DUMP_TIMEOUT);
                    }
                }
                else
                {
                    data_error = 0xAA; //to display error at function end
                }
            }
            else
            {
                //dump_timeout = dump_timeout;
                data_error = 10;
            }
        }
        break;

    case SER_REFILL_TIMEOUT:                                            //31
        while (data_error <= 5)
        {
            if ((refill_timeout < 10) || (refill_timeout > 900))
            {
                data_error_validation_code = SER_REFILL_TIMEOUT;
                data_error_correct_value = DEV_REFILL_TIMEOUT;

                if (data_error == 1)
                    sprintf(data_error_buffer, "%.5u", refill_timeout);
                else{}

                if (error_flags.data_error_auto_correct == TRUE)
                {
                    if ((eeprom_access != EEPROM_READ)&&(data_error <= 3))
                    {
                        read_eeprom_parameter(SER_REFILL_TIMEOUT);
                        data_error++;
                    }
                    else
                    {
                        refill_timeout = 45;
                        data_error = 0xAA; //to display error at function end
                        eeprom_write(SER_REFILL_TIMEOUT);
                    }
                }
                else
                {
                    data_error = 0xAA; //to display error at function end
                }
            }
            else
            {
                //refill_timeout = refill_timeout;
                data_error = 10;
            }
        }
        break;

    case SER_CLEANING_TIMEOUT: //32
        while (data_error <= 5)
        {
            if ((cleaning_timeout < 10) || (cleaning_timeout > 900))
            {
                data_error_validation_code = SER_CLEANING_TIMEOUT;
                data_error_correct_value = DEV_CLEANING_TIMEOUT;

                if (data_error == 1)
                    sprintf(data_error_buffer, "%.5u", cleaning_timeout);
               else{}

                if (error_flags.data_error_auto_correct == TRUE)
                {
                    if ((eeprom_access != EEPROM_READ)&&(data_error <= 3))
                    {
                        read_eeprom_parameter(SER_CLEANING_TIMEOUT);
                        data_error++;
                    }
                    else
                    {
                        cleaning_timeout = 20;
                        data_error = 0xAA; //to display error at function end
                        eeprom_write(SER_CLEANING_TIMEOUT);
                    }
                }
                else
                {
                    data_error = 0xAA; //to display error at function end
                }
            }
            else
            {
                //cleaning_timeout = cleaning_timeout;
                data_error = 10;
            }
        }
        break;
        
    case SER_CHANGE_DEV_ID:                                                //40
        while (data_error <= 5)
        {
            if (((strlen(MACHINE_ID)) < 2) || ((strlen(MACHINE_ID)) > 10))
            {
                data_error_validation_code = SER_CHANGE_DEV_ID;
                data_error_correct_value = DEV_DEVICE_IC_CHANGED;

                if (data_error == 1)
                    sprintf(data_error_buffer, "%.11s", MACHINE_ID);
                else{}

                if (error_flags.data_error_auto_correct == TRUE)
                {
                    if ((eeprom_access != EEPROM_READ)&&(data_error <= 3))
                    {
                        read_eeprom_parameter(SER_CHANGE_DEV_ID);
                        data_error++;
                    }
                    else
                    {
                        strcpy(MACHINE_ID, "1111111111");
                        data_error = 0xAA; //to display error at function end
                        eeprom_write(SER_CHANGE_DEV_ID);
                    }
                }
                else
                {
                    data_error = 0xAA; //to display error at function end
                }
            }
            else
            {
                data_error = 10;
            }
        }
        break;

    case SER_LEAK_TIMER:                                                    //41
        while (data_error <= 5)
        {
            if ((leak_timeout < 5) || (leak_timeout > 300))
            {
                data_error_validation_code = SER_LEAK_TIMER;
                data_error_correct_value = DEV_LEAK_TIMER;

                if (data_error == 1)
                    sprintf(data_error_buffer, "%.5u", leak_timeout);
                else{}

                if (error_flags.data_error_auto_correct == TRUE)
                {
                    if ((eeprom_access != EEPROM_READ)&&(data_error <= 3))
                    {
                        read_eeprom_parameter(SER_LEAK_TIMER);
                        data_error++;
                    }
                    else
                    {
                        leak_timeout = 10;
                        data_error = 0xAA; //to display error at function end
                        eeprom_write(SER_LEAK_TIMER);
                    }
                }
                else
                {
                    data_error = 0xAA; //to display error at function end
                }
            }
            else
            {
                //leak_timeout = leak_timeout;
                data_error = 10;
            }
        }
        break;

    case SER_SCHOOL_MODE:                                               //52
        
                while (data_error <= 5)
                {
                    if (school_mode_en > 1)
                    {
                        data_error_validation_code = SER_SCHOOL_MODE;
                        data_error_correct_value = DEV_HB_MSG;

                        if (data_error == 1)
                            sprintf(data_error_buffer, "%.3u", school_mode_en);
                        else{}

                        if (error_flags.data_error_auto_correct == TRUE)
                        {
                            if ((eeprom_access != EEPROM_READ)&&(data_error <= 3))
                            {
                                read_eeprom_parameter(SER_SCHOOL_MODE);
                                data_error++;
                            } else
                            {
                                school_mode_en = 0;
                                eeprom_write(SER_SCHOOL_MODE);
                            }
                        } else
                        {
                            data_error = 0xAA; //to display error at function end
                        }
                    } else
                    {
                        //school_mode_en = school_mode_en;
                        data_error = 10;
                    }
                }
         
        break;    
        
    case SER_MASTER_CONT_NO: //56
        while (data_error <= 5)
        {
            //if (((strlen(master_contact)) < 10) || ((strlen(master_contact)) > 12))  //v3.1.2
              if (((strlen(master_contact)) < 10) || ((strlen(master_contact)) > 15))  //v3.2.0.B
            {
                data_error_validation_code = SER_MASTER_CONT_NO;
                data_error_correct_value = DEV_MASTER_CONT_NO;

                if (data_error == 1)
                    sprintf(data_error_buffer, "%s", master_contact);
                else{}

                if (error_flags.data_error_auto_correct == TRUE)
                {
                    if ((eeprom_access != EEPROM_READ)&&(data_error <= 3))
                    {
                        read_eeprom_parameter(SER_MASTER_CONT_NO);
                        data_error++;
                    }
                    else
                    {
                        strcpy(master_contact, "918128252501"); //918128660427
                        data_error = 0xAA; //to display error at function end
                        eeprom_write(SER_MASTER_CONT_NO);
                    }
                }
                else
                {
                    data_error = 0xAA; //to display error at function end
                }
            }
            else
            {
                data_error = 10;
            }
        }
        break;

    case SER_TEMPERATURE_CALIB:                                              //63
        while (data_error <= 5)
        {
            if ((temp_calib_fact < 1) || (temp_calib_fact > 5000))
            {
                data_error_validation_code = SER_TEMPERATURE_CALIB;
                data_error_correct_value = DEV_TEMPERATURE_CALIB;

                if (data_error == 1)
                    sprintf(data_error_buffer, "%.4u", temp_calib_fact);
                else{}

                if (error_flags.data_error_auto_correct == TRUE)
                {
                    if ((eeprom_access != EEPROM_READ)&&(data_error <= 4))
                    {
                        read_eeprom_parameter(SER_TEMPERATURE_CALIB);
                        data_error++;
                    }
                    else
                    {
                        temp_calib_fact = 100;
                        data_error = 0xAA; //to display error at function end
                        eeprom_write(SER_TEMPERATURE_CALIB);
                    }
                }
                else
                {
                    data_error = 0xAA; //to display error at function end
                }
            }
            else
            {
                data_error = 10;
            }
        }
        break;

    case SER_WATER_TEMPERATURE_LEVEL: //v2.3.6                            //64
        while (data_error <= 5)
        {
            if (peripheral_flags.chiller_heater_en_dis == 1)
            {
                if ((water_temperature_level < 11) || (water_temperature_level >= 30)) //v2.3.6 - limit form 1 to 50 to 10 to 70
                {
                    data_error_validation_code = SER_WATER_TEMPERATURE_LEVEL; //v2.3.6
                    data_error_correct_value = DEV_WATER_TEMPERATURE_LEVEL; //v2.3.6

                    if (data_error == 1)
                        sprintf(data_error_buffer, "%.3u", water_temperature_level); //v2.3.6
                    else{}

                    if (error_flags.data_error_auto_correct == TRUE)
                    {
                        if ((eeprom_access != EEPROM_READ)&&(data_error <= 4))
                        {
                            read_eeprom_parameter(SER_WATER_TEMPERATURE_LEVEL); //v2.3.6
                            data_error++;
                        }
                        else
                        {
                            water_temperature_level = 20; //v2.3.6
                            data_error = 0xAA; //to display error at function end
                            eeprom_write(SER_WATER_TEMPERATURE_LEVEL); //v2.3.6
                        }
                    }
                    else
                    {
                        data_error = 0xAA; //to display error at function end
                    }
                }
                else
                {
                    data_error = 10;
                }
            }
            else if (peripheral_flags.chiller_heater_en_dis == 2)
            { //v2.3.6
                if ((water_temperature_level <= 30) || (water_temperature_level > 55)) //v2.3.6 - limit form 1 to 50 to 10 to 70
                {
                    data_error_validation_code = SER_WATER_TEMPERATURE_LEVEL; //v2.3.6
                    data_error_correct_value = DEV_WATER_TEMPERATURE_LEVEL; //v2.3.6

                    if (data_error == 1)
                        sprintf(data_error_buffer, "%.3u", water_temperature_level); //v2.3.6
                    else{}

                    if (error_flags.data_error_auto_correct == TRUE)
                    {
                        if ((eeprom_access != EEPROM_READ)&&(data_error <= 4))
                        {
                            read_eeprom_parameter(SER_WATER_TEMPERATURE_LEVEL); //v2.3.6
                            data_error++;
                        }
                        else
                        {
                            water_temperature_level = 45; //v2.3.6
                            data_error = 0xAA; //to display error at function end
                            eeprom_write(SER_WATER_TEMPERATURE_LEVEL); //v2.3.6
                        }
                    }
                    else
                    {
                        data_error = 0xAA; //to display error at function end
                    }
                }
                else
                {
                    data_error = 10;
                }
            }
            else
            {
                data_error = 10;
            }
        }
        break;

    case SER_OUT_FLOW_FREQ_LIMIT:                                       //66
        while (data_error <= 5)
        {
            //---if((output_flow_freq_limit<1) || (output_flow_freq_limit>500))
            if ((output_flow_freq_limit < 0) || (output_flow_freq_limit > 500)) //v2.2.5
            {
                data_error_validation_code = SER_OUT_FLOW_FREQ_LIMIT;
                data_error_correct_value = DEV_OUT_FLOW_FREQ_LIMIT;

                if (data_error == 1)
                    sprintf(data_error_buffer, "%.3u", output_flow_freq_limit);
                else{}

                if (error_flags.data_error_auto_correct == TRUE)
                {
                    if ((eeprom_access != EEPROM_READ)&&(data_error <= 4))
                    {
                        read_eeprom_parameter(SER_OUT_FLOW_FREQ_LIMIT);
                        data_error++;
                    }
                    else
                    {
                        output_flow_freq_limit = 10;
                        data_error = 0xAA; //to display error at function end
                        eeprom_write(SER_OUT_FLOW_FREQ_LIMIT);
                    }
                }
                else
                {
                    data_error = 0xAA; //to display error at function end
                }
            }
            else
            {
                data_error = 10;
            }
        }
        break;

    case SER_IN_FLOW_FREQ_LIMIT:                                        //67
        while (data_error <= 5)
        {
            //---if((input_flow_freq_limit<1) || (input_flow_freq_limit>500))
            if ((input_flow_freq_limit < 0) || (input_flow_freq_limit > 500)) //v2.2.5
            {
                data_error_validation_code = SER_IN_FLOW_FREQ_LIMIT;
                data_error_correct_value = DEV_IN_FLOW_FREQ_LIMIT;

                if (data_error == 1)
                    sprintf(data_error_buffer, "%.3u", input_flow_freq_limit);
                else{}

                if (error_flags.data_error_auto_correct == TRUE)
                {
                    if ((eeprom_access != EEPROM_READ)&&(data_error <= 4))
                    {
                        read_eeprom_parameter(SER_IN_FLOW_FREQ_LIMIT);
                        data_error++;
                    }
                    else
                    {
                        input_flow_freq_limit = 10;
                        data_error = 0xAA; //to display error at function end
                        eeprom_write(SER_IN_FLOW_FREQ_LIMIT);
                    }
                }
                else
                {
                    data_error = 0xAA; //to display error at function end
                }
            }
            else
            {
                data_error = 10;
            }
        }
        break;     

    case SER_FRANCHISEE_RFID: // for verifying sms rfid tag no :- v2.2.5        //70
        while (data_error <= 5)
        {
            //					if(!(verify_card_id(franchisee_rfid_tag)))       
            if (((strlen(franchisee_rfid_tag)) < 8) || ((strlen(franchisee_rfid_tag)) > 8)) 
            {
                data_error_validation_code = SER_FRANCHISEE_RFID;
                data_error_correct_value = DEV_FRANCHISEE_RFID;
                lcd_init();
                strcpy(lcd_data, "IMPROPER RFID NO.");
                lcd_display(1);
                strcpy(lcd_data, "PLS. SMS AGAIN");
                lcd_display(2);
                buzzer(ERR01);
                timer2_sec_delay(error_display_time);
                if (data_error == 1)
                    sprintf(data_error_buffer, "%s", franchisee_rfid_tag); //.8s to %s //v3.1.1.C
                else{}

                if (error_flags.data_error_auto_correct == TRUE)
                {
                    if ((eeprom_access != EEPROM_READ)&&(data_error <= 3))
                    {
                        read_eeprom_parameter(SER_FRANCHISEE_RFID);
                        data_error++;
                    }
                    else
                    {
                        strcpy(franchisee_rfid_tag, "721234FB");
                        franchisee_rfid_tag[8] = 0; //last byte null character

                        data_error = 0xAA; //to display error at function end
                        eeprom_write(SER_FRANCHISEE_RFID);
                    }
                }
                else
                    data_error = 0xAA;
            }
            else
                data_error = 10;
        }
        break;

    case SER_CORPORATE_MODE: //v2.2.5			                      	//71 
        while (data_error <= 5)
        {
            if (corporate_mode_en > 1)
            {
                data_error_validation_code = SER_CORPORATE_MODE;
                data_error_correct_value = DEV_HB_MSG;

                if (data_error == 1)
                    sprintf(data_error_buffer, "%.3u", corporate_mode_en);
                else{}

                if (error_flags.data_error_auto_correct == TRUE)
                {
                    if ((eeprom_access != EEPROM_READ)&&(data_error <= 3))
                    {
                        read_eeprom_parameter(SER_CORPORATE_MODE);
                        data_error++;
                    }
                    else
                    {
                        corporate_mode_en = 0;
                        eeprom_write(SER_CORPORATE_MODE);
                    }
                }
                else
                {
                    data_error = 0xAA; //to display error at function end
                }
            }
            else
            {
                data_error = 10;
            }
        }
        break;

    case SER_CORPORATE_MODE_ACC_ID: //v2.2.5                                    //72

        while (data_error <= 5)
        {
            if (((strlen(corp_mode_acc_id)) < 2) || ((strlen(corp_mode_acc_id)) > 8)) //3.1.0
            {
                data_error_validation_code = SER_CORPORATE_MODE_ACC_ID;
                data_error_correct_value = DEV_CORPORATE_MODE_ACC_ID;
                corporate_mode_en =0; //v3.1.2.G

                if (data_error == 1)
                    sprintf(data_error_buffer, "%.8s", corp_mode_acc_id);
                else{}

                if (error_flags.data_error_auto_correct == TRUE)
                {
                    if ((eeprom_access != EEPROM_READ)&&(data_error <= 3))
                    {
                        read_eeprom_parameter(SER_CORPORATE_MODE_ACC_ID);
                        data_error++;
                    }
                    else
                    {
                        //strcpy(corp_mode_acc_id,"NULL_ACC"); //3.1.0 
                        strcpy(corp_mode_acc_id, "EMPTY_ID"); //v3.1.1.C
                        corp_mode_acc_id[8] = 0; //last byte null character

                        data_error = 0xAA; //to display error at function end
                        eeprom_write(SER_CORPORATE_MODE_ACC_ID);
                    }
                }
                else
                {
                    data_error = 0xAA; //to display error at function end
                }
            }
            else if (mode_flags.op_mode == CORPORATE_MODE)
            {
                char* local_ptr;
                local_ptr = 0;
                local_ptr = strstr(corp_mode_acc_id, "EMPTY");
                if (local_ptr != 0)
                {
                    data_error_validation_code = SER_CORPORATE_MODE_ACC_ID;
                    data_error_correct_value = DEV_CORPORATE_MODE_ACC_ID;

                    if (data_error == 1)
                        sprintf(data_error_buffer, "%.8s", corp_mode_acc_id); //3.1.0
                    else{}

                    //if(error_flags.data_error_auto_correct==TRUE)
                    {
                        if ((eeprom_access != EEPROM_READ)&&(data_error <= 3))
                        {
                            read_eeprom_parameter(SER_CORPORATE_MODE_ACC_ID);
                            data_error++;
                        }
                        else
                        {
                            strcpy(corp_mode_acc_id, "00000000");
                            corp_mode_acc_id[8] = 0; //last byte null character

                            data_error = 0xAA; //to display error at function end
                            eeprom_write(SER_CORPORATE_MODE_ACC_ID);
                        }
                    }
                }
                else
                    data_error = 10;
            }
            else
            {
                data_error = 10;
            }
        }
        break;

    case SER_COIN_WATER_TARIFF: //v2.3.5	                          //78
        while (data_error <= 5)
        {
            data_valid_float = coin_water_tariff;
            if ((data_valid_float < 10) || (data_valid_float > 500))
            {
                data_error_validation_code = SER_COIN_WATER_TARIFF;
                data_error_correct_value = DEV_COIN_WATER_TARIFF;

                if (data_error == 1)
                    sprintf(data_error_buffer, "%4.0f", data_valid_float);
                else{}

                if (error_flags.data_error_auto_correct == TRUE)
                {
                    if ((eeprom_access != EEPROM_READ)&&(data_error <= 4))
                    {
                        read_eeprom_parameter(SER_COIN_WATER_TARIFF);
                        data_error++;
                    }
                    else
                    {
                        //read_eeprom_parameter(SER_COIN_WATER_TARIFF);  ?
                        coin_water_tariff = 50.0;
                        data_error = 0xAA; //to display error at function end
                        eeprom_write(SER_COIN_WATER_TARIFF);
                    }
                }
                else
                {
                    data_error = 0xAA; //to display error at function end
                }
            }
            else
            {
                data_error = 10;
            }
        }
        break;

    case SER_COIN_COLLECT_CAPACITY: //v2.3.5	                          //79
        while (data_error <= 5)
        {
            data_valid_int = coin_collector_capacity;
            if ((data_valid_int < 50) || (data_valid_int > 2500))
            {
                data_error_validation_code = SER_COIN_COLLECT_CAPACITY;
                data_error_correct_value = DEV_COIN_COLLECT_CAPACITY;

                if (data_error == 1)
                    sprintf(data_error_buffer, "%.4d", data_valid_int);
                else{}

                if (error_flags.data_error_auto_correct == TRUE)
                {
                    if ((eeprom_access != EEPROM_READ)&&(data_error <= 4))
                    {
                        read_eeprom_parameter(SER_COIN_COLLECT_CAPACITY);
                        data_error++;
                    }
                    else
                    {
                        coin_collector_capacity = 300;
                        data_error = 0xAA; //to display error at function end
                        eeprom_write(SER_COIN_COLLECT_CAPACITY);
                    }
                }
                else
                {
                    data_error = 0xAA; //to display error at function end
                }
            }
            else
            {
                data_error = 10;
            }
        }
        break;

    case SER_COIN_CHN_EN_DIS: //v2.2.9		 //v3.1.1.C		    //80
        while (data_error <= 5)
        {
            if ((coin_channel_no < 1) || (coin_channel_no > 8))
            {
                data_error_validation_code = SER_COIN_CHN_EN_DIS;
                data_error_correct_value = DEV_COIN_CHN_EN_DIS;

                if (data_error == 1)
                    sprintf(data_error_buffer, "%.1u", coin_channel_no);
                else{}

                if (error_flags.data_error_auto_correct == TRUE)
                {
                    if ((eeprom_access != EEPROM_READ)&&(data_error <= 4))
                    {
                        read_eeprom_parameter(SER_COIN_CHN_EN_DIS);
                        data_error++;
                    }
                    else
                    {
                        //	coin_channel_no = 3;
                        //	coin_channel_En_Dis = 1;
                        data_error = 0xAA; //to display error at function end
                    }
                }
                else
                {
                    data_error = 0xAA; //to display error at function end
                }
            }
            else
            {
                data_error = 10;
            }
        }
        break;
    case SER_COIN_ID: //v2.2.9			                	//81
        while (data_error <= 5)
        {
            if (((strlen(temp_coin_id)) < 2) || ((strlen(temp_coin_id)) > 12))
            {
                data_error_validation_code = SER_COIN_ID;
                data_error_correct_value = DEV_COIN_ID;

                if (data_error == 1)
                    sprintf(data_error_buffer, "%.12s", temp_coin_id);
                else{}

                if (error_flags.data_error_auto_correct == TRUE)
                {
                    if ((eeprom_access != EEPROM_READ)&&(data_error <= 3))
                    {
                        read_eeprom_parameter(SER_COIN_ID);
                        data_error++;
                    }
                    else
                    {
                        strcpy(temp_coin_id, "EMPTY_COINID");
                        temp_coin_id[12] = 0; //last byte null character

                        data_error = 0xAA; //to display error at function end
                        eeprom_write(SER_COIN_ID);
                    }
                }
                else
                {
                    data_error = 0xAA; //to display error at function end
                }
            }
            else
            {
                data_error = 10;
            }
        }
        break;

    case SER_OFFLINE_WATER_TARIFF: //v2.3.5	                         //87
        while (data_error <= 5)
        {
            //data_valid_float = offline_water_tariff;
            if ((offline_water_tariff < 10) || (offline_water_tariff > 500))
            {
                data_error_validation_code = SER_OFFLINE_WATER_TARIFF;
                data_error_correct_value = DEV_OFFLINE_WATER_TARIFF;

                if (data_error == 1)
                    sprintf(data_error_buffer, "%4.0f", offline_water_tariff);
                else{}

                if (error_flags.data_error_auto_correct == TRUE)
                {
                    if ((eeprom_access != EEPROM_READ)&&(data_error <= 4))
                    {
                        read_eeprom_parameter(SER_OFFLINE_WATER_TARIFF);
                        data_error++;
                    }
                    else
                    {
                        //read_eeprom_parameter(SER_OFFLINE_WATER_TARIFF);  
                        offline_water_tariff = 50.0;
                        data_error = 0xAA; //to display error at function end
                        eeprom_write(SER_OFFLINE_WATER_TARIFF);
                    }
                }
                else
                {
                    data_error = 0xAA; //to display error at function end
                }
            }
            else
            {
                data_error = 10;
            }
        }
        break;
    case SER_MENU_PASSWORD_SET:                                           //88
        while (data_error <= 5)
        {
            if (((strlen(menu_franchisee_password)) <= 3) || ((strlen(menu_franchisee_password)) > 4))
            {
                data_error_validation_code = SER_MENU_PASSWORD_SET;
                data_error_correct_value = DEV_MENU_PASSWORD_SET;

                if (data_error == 1)
                    sprintf(data_error_buffer, "%.5s", menu_franchisee_password);
                else{}

                if (error_flags.data_error_auto_correct == TRUE)
                {
                    if ((eeprom_access != EEPROM_READ)&&(data_error <= 3))
                    {
                        read_eeprom_parameter(SER_MENU_PASSWORD_SET);
                        data_error++;
                    }
                    else
                    {
                        strcpy(menu_franchisee_password, "1234");

                        data_error = 0xAA; //to display error at function end
                        eeprom_write(SER_MENU_PASSWORD_SET);
                    }
                }
                else
                {
                    data_error = 0xAA; //to display error at function end
                }
            }
            else
            {
                data_error = 10;
            }
        }
        break;
  
    case SER_TDS_VERSION: //v2.2.9                                              //91
        while (data_error <= 5)
        {
            if (((tds_header_version != 'B')&&(tds_header_version != 'b')) && ((tds_header_version != 'C')&&(tds_header_version != 'c')))
            {
                data_error_validation_code = SER_TDS_VERSION;
                data_error_correct_value = DEV_TDS_VERSION;

                if (data_error == 1)
                    sprintf(data_error_buffer, "%c", tds_header_version);
                else{}

                if (error_flags.data_error_auto_correct == TRUE)
                {
                    if ((eeprom_access != EEPROM_READ)&&(data_error <= 3))
                    {
                        read_eeprom_parameter(SER_TDS_VERSION);
                        data_error++;
                    }
                    else
                    {
                        tds_header_version = 'C'; //v3.1.1.F 'B' to 'C'
                        data_error = 0xAA; //to display error at function end
                        eeprom_write(SER_TDS_VERSION);
                    }
                }
                else
                {
                    data_error = 0xAA; //to display error at function end
                }
            }
            else
            {
                data_error = 10;
            }
        }
        break;
 
    case SER_OP_MODE:                                                     //100
        while (data_error <= 5)
        {
            //if((mode_flags.op_mode <0) || (mode_flags.op_mode >7))
            if ((mode_flags.op_mode < 0) || (mode_flags.op_mode > MAX_OPMODE)) //v3.1.1.F
            {
                data_error_validation_code = SER_OP_MODE;
                data_error_correct_value = DEV_HB_MSG;

                if (data_error == 1)
                    sprintf(data_error_buffer, "%u", mode_flags.op_mode);
                else{}

                if (error_flags.data_error_auto_correct == TRUE)
                {
                    if ((eeprom_access != EEPROM_READ)&&(data_error <= 3))
                    {
                        read_eeprom_parameter(SER_OP_MODE);
                        data_error++;
                    }
                    else
                    {
                        mode_flags.op_mode = 0;
                        data_error = 0xAA; //to display error at function end
                        eeprom_write(SER_OP_MODE);
                    }
                }
                else
                {
                    data_error = 0xAA; //to display error at function end
                }
            }
            else
            {
                //---mode_flags.op_mode = mode_flags.op_mode;
                data_error = 10;
            }
        }
        break;

    case SER_ERROR_STACK:                                                 //101
        while (data_error <= 5)
        {
            for (loop = 0; loop < TOTAL_ERRORS; loop++)
            {
                if (error_stack[loop] > 1)
                    data_error = 2;
                else
                {
                }
            }
            if (data_error > 1)
            {
                data_error_validation_code = SER_ERROR_STACK;
                data_error_correct_value = DEV_HB_MSG;

                if (data_error == 1)
                {
                    for (loop = 0; loop < TOTAL_ERRORS; loop++)
                        sprintf(data_error_buffer[loop], "%u", error_stack[loop]);
                }
                else{}

                if (error_flags.data_error_auto_correct == TRUE)
                {
                    if ((eeprom_access != EEPROM_READ)&&(data_error <= 3))
                    {
                        read_eeprom_parameter(SER_ERROR_STACK);
                        data_error++;
                    }
                    else
                    {
                        for (loop = 0; loop < TOTAL_ERRORS; loop++)
                            error_stack[loop] = 0;

                        data_error = 0xAA; //to display error at function end
                        eeprom_write(SER_ERROR_STACK);
                    }
                }
                else
                {
                    data_error = 0xAA; //to display error at function end
                }
                system_error_check();
            }
            else
            {
                data_error = 10;
            }
        }
        break;

    case SER_SCHOOL_MODE_ACC_ID:                                               //102
        while (data_error <= 5)
        {
            if (((strlen(sch_mode_acc_id)) < 2) || ((strlen(sch_mode_acc_id)) > 8)) //3.1.0
            {
                data_error_validation_code = SER_SCHOOL_MODE_ACC_ID;
                data_error_correct_value = DEV_SCHOOL_MODE_ACC_ID;

                if (data_error == 1)
                    sprintf(data_error_buffer, "%.8s", sch_mode_acc_id);
                else{}

                if (error_flags.data_error_auto_correct == TRUE)
                {
                    if ((eeprom_access != EEPROM_READ)&&(data_error <= 3))
                    {
                        read_eeprom_parameter(SER_SCHOOL_MODE_ACC_ID);
                        data_error++;
                    }
                    else
                    {
                        strcpy(sch_mode_acc_id, "EMPTY_ID"); //3.1.0
                        sch_mode_acc_id[8] = 0; //last byte null character//3.1.0

                        data_error = 0xAA; //to display error at function end
                        eeprom_write(SER_SCHOOL_MODE_ACC_ID);
                    }
                }
                else
                {
                    data_error = 0xAA; //to display error at function end
                }
            }
            else if (mode_flags.op_mode == SCHOOL_MODE)
            {
                char* local_ptr;
                local_ptr = 0;
                local_ptr = strstr(sch_mode_acc_id, "EMPTY");
                if (local_ptr != 0)
                {
                    data_error_validation_code = SER_SCHOOL_MODE_ACC_ID;
                    data_error_correct_value = DEV_SCHOOL_MODE_ACC_ID;

                    if (data_error == 1)
                        sprintf(data_error_buffer, "%.8s", sch_mode_acc_id); //3.1.0
                    else{}

                    //if(error_flags.data_error_auto_correct==TRUE)
                    {
                        if ((eeprom_access != EEPROM_READ)&&(data_error <= 3))
                        {
                            read_eeprom_parameter(SER_SCHOOL_MODE_ACC_ID);
                            data_error++;
                        }
                        else
                        {
                            strcpy(sch_mode_acc_id, "00000000"); //3.1.0
                            sch_mode_acc_id[8] = 0; //last byte null character  //3.1.0

                            data_error = 0xAA; //to display error at function end
                            eeprom_write(SER_SCHOOL_MODE_ACC_ID);
                        }
                    }
                }
                else
                    data_error = 10;
            }
            else
            {
                data_error = 10;
            }
        }
        break;

    case SER_FLOW_OFFSET:                                                  //103
        while (data_error <= 5)
        {
            if (flow_offset != 0.1)
            {
                data_error_validation_code = SER_FLOW_OFFSET;
                data_error_correct_value = 0;

                if (data_error == 1)
                {
                    sprintf(data_error_buffer, "%f", flow_offset);
                    send_server_response(DEV_VALIDATION_ERROR);
                }
                else{}

                if ((eeprom_access != EEPROM_READ)&&(data_error <= 3))
                {
                    flow_offset = 0.1;
                    data_error++;
                }
                else
                {
                    flow_offset = 0.1;
                    data_error = 0xAA; //to display error at function end
                }
            }
            else
            {
                data_error = 10;
            }
        }
        break;

    case SER_T_SMS_READ:                                                //104
        while (data_error <= 5)
        {
            if ((TOTAL_SMS_READ < 3) || (TOTAL_SMS_READ > 50))
            {
                data_error_validation_code = SER_T_SMS_READ;
                data_error_correct_value = 0;

                if (data_error == 1)
                {
                    sprintf(data_error_buffer, "%.5u", TOTAL_SMS_READ);
                    send_server_response(DEV_VALIDATION_ERROR);
                }
                else{}

                if ((eeprom_access != EEPROM_READ)&&(data_error <= 3))
                {
                    TOTAL_SMS_READ = 5;
                    data_error++;
                }
                else
                {
                    TOTAL_SMS_READ = 5;
                    data_error = 0xAA; //to display error at function end
                }
            }
            else
            {
                data_error = 10;
            }
        }
        break;

    case SER_ER_STATE:                                                  //107
        while (data_error <= 5)
        {
            loop = strlen(error_state);
            if (loop != 4)
            {
                data_error_validation_code = SER_ER_STATE;
                data_error_correct_value = DEV_HB_MSG;

                if (data_error == 1)
                    sprintf(data_error_buffer, "%.12s", error_state);
                else{}

                if (data_error <= 3)
                {
                    system_error_check();
                    data_error++;
                }
                else
                {
                    strcpy(error_state, "0000");

                    data_error = 0xAA; //to display error at function end
                }
            }
            else
            {
                data_error = 10;
            }
        }
        break;

    case SER_TANK_LOW_LEVEL_STATUS: //v2.2.3                             //108
        if (!((peripheral_flags.lls_hls_en_dis >= 2)&&(peripheral_flags.lls_trigger == TRUE)))
        {
            while (data_error <= 5)
            {
                if ((tank_low_level_litre < 1) || (tank_low_level_litre > 1000))
                {
                    data_error_validation_code = SER_TANK_LOW_LEVEL_STATUS;
                    data_error_correct_value = DEV_LT_EMPTY_TANK;

                    if (data_error == 1)
                        sprintf(data_error_buffer, "%.5u", tank_low_level_litre);
                    else{}
                    if (error_flags.data_error_auto_correct == TRUE)
                    {
                        if ((eeprom_access != EEPROM_READ)&&(data_error <= 3))
                        {
                            read_eeprom_parameter(SER_TANK_LOW_LEVEL_STATUS);
                            data_error++;
                        }
                        else
                        {
                            tank_low_level_litre = lit_empty_tank;

                            data_error = 0xAA; //to display error at function end
                            eeprom_write(SER_TANK_LOW_LEVEL_STATUS);
                        }
                    }
                    else
                    {
                        data_error = 0xAA; //to display error at function end
                    }
                }
                else
                {
                    //lit_empty_tank = lit_empty_tank;
                    data_error = 10;
                }
            }
        }
        break;

    default:
        break;
    }

    validate_para_cnt++;
    
    if ((validate_para_cnt>32) && (validate_para_cnt<40))
    {
       validate_para_cnt = 40;
    }
    else if ((validate_para_cnt>41) && (validate_para_cnt<52))
    {
       validate_para_cnt = 52;
    }
    else if ((validate_para_cnt>56) && (validate_para_cnt<63))
    {
       validate_para_cnt = 63;
    }
    else if ((validate_para_cnt>72) && (validate_para_cnt<78))
    {
       validate_para_cnt = 78;
    }
    else if ((validate_para_cnt>79) && (validate_para_cnt<87))
    {
       validate_para_cnt = 87;
    }
   
    else if ((validate_para_cnt > 91) && (validate_para_cnt < 102)) //v1.0.1
    {        validate_para_cnt = 102;
    }

    else if (validate_para_cnt > 103)
    {   validate_para_cnt = 0;
    }
    else{}
    if (data_error_validation_code != 0)
    {
        validation_error_counter++;
        lcd_init();
        sprintf(lcd_data, "DE:%u", data_error_validation_code);
        lcd_display(1);
        sprintf(lcd_data, "%s", data_error_buffer);
        lcd_display(2);
        buzzer(ERR01);
        send_server_response(DEV_VALIDATION_ERROR); //RM04
        //send_server_response(DEV_VALIDATION_ERROR+DEV_SENT_VIA_SMS);
        send_server_response(data_error_correct_value); //
        send_server_response(DEV_DATA_ER_AUTO_CORRECT); //RA38
        timer2_sec_delay(error_display_time);
        if (validation_error_counter > 5) // 3 TO 5 //v3.1.1.G
        {
            sprintf(lcd_data, "DE EXCEEDED..!! ");
            lcd_display(1);
            sprintf(lcd_data, "SYSTEM RESTART  ");
            lcd_display(2);
            buzzer(ERR02);
            timer2_sec_delay(error_display_time);
            abort(); //reset uC
        }
        else{}

        return data_error;
    }
    else{}
    return 0;
}
//    *******************************************************************************************************************************************************  //


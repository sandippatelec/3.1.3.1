#include "main.h"
 
//#include "ff.h"			/* Declarations of FatFs API */
//#include "diskio.h"		/* Declarations of disk I/O functions */
#include "FSIO.h"
#include "DS1307.h"


//----------- Modem and Server -----------------//
extern unsigned int TOTAL_SMS_READ;
extern unsigned char SERVER_RESENT_COUNT;

//----------- TDS -----------------//
extern unsigned char TDS_OFFSET;
extern unsigned char tds_difference_exceed;

//----------------Global Master Variable-----------------
extern signed char MACHINE_ID[11];
extern unsigned char master_contact[15]; //v3.2.0.B

//----------------Global Master Variable-----------------
extern char Rxdata[MAX_RX_BUFFER_LIMIT];
extern char modem_cmd_response[40];
extern char sms_string[160];

extern unsigned char Txdata[MAX_TX_BUFFER_LIMIT];
extern unsigned char Tx_Buffer_Head,Tx_Buffer_Tail;
extern unsigned char Rx_Buffer_Head,Rx_Buffer_Tail;
extern unsigned char sms_sender_no[15];   //v3.2.0.A
extern unsigned char modem_reset_count;
extern unsigned char sms_txn[5];
extern unsigned char modem_reset_tolerance;
extern unsigned char network_service_provider[25]; //v3.2.0.A //15 to 25
extern unsigned char network_signal_strength[3];
extern char sms_buffer[MAX_SMS_LENGTH];
extern char current_password[5];
extern char factory_password[5];
extern char menu_current_password[5];
extern char menu_franchisee_password[5];
extern unsigned int sms_scan_no;

extern unsigned int rfid_to_server_msg;
extern unsigned char local_ip[20];
extern unsigned char server_ip[20];
extern unsigned char server_port[6];
extern unsigned char apn[25];                         //v3.2.0  20 to 25
extern unsigned char server_send_message_type;
extern unsigned char server_command[5];
//extern unsigned char rfid_to_server_msg;

extern unsigned char network_state,server_reconnect_count;
extern char transmitData[MAX_DATA_LENGTH];
extern char txn_type[5];
extern char msg_data[15];
extern char server_response_string[MAX_SERVER_STRING];
extern char temp_data[16];

extern unsigned char rfid_card[9];  //v3.1.1.B 13
extern unsigned char scanned_rfid_card[25];		//v2.3.1

//	for testing
//extern unsigned char user1[13];
//extern unsigned char user2[13];
//extern unsigned char user3[13];
//extern unsigned int user1_balance;
//extern unsigned int user2_balance;
//extern unsigned int user3_balance;
//extern unsigned char authorization;
//extern unsigned char user_add_count;
//  

extern unsigned char rfid_reader_counter;
extern unsigned char franchisee_rfid_tag[9];

extern unsigned int output_flow_count,input_flow_count;
extern unsigned int output_flow_tick,input_flow_tick;		//v2.2.3
extern unsigned int output_flow_freq_limit,input_flow_freq_limit;		//v2.2.3
extern char disp_buffer[10];


//---extern unsigned int RAAC_val,RSAC_val,FULL_val,AE_val,SE_val;
extern unsigned char RARC_val;	//,RSRC_val,STATUS_reg;

//extern float DISPENSE_LEAST_COUNT;  //v3.1.1.E
extern float balance_deduct,max_dispense_litre,unauth_dispense_litre,unauth_input_litre;

extern char acc_credit_array[10];
extern char previous_bal_array[10];
//---extern unsigned int acc_credit;		//---,dummy_acc_credit;
//---extern unsigned int acc_rate_1,acc_rate_2,acc_switch_point;
extern float dispense_litre,total_dispense_litre, refill_litre,acc_dispense_capacity;

extern unsigned int random_no;
extern char random_no_array[6];


extern double adc_volt;
extern unsigned char analog_sense;
extern unsigned int idle_msg_freq,modem_status_check_min;  //v3.1.2.J
extern unsigned char mcu_auto_reset_counter,modem_status_check_hour;
//---extern unsigned int copy_idle_msg_freq;

extern unsigned int tds_array[AVG_READING_TDS],avg_tds_count;
extern unsigned int tds_reading;
extern unsigned int min_tds,max_tds,tds_calib_fact;
extern unsigned char adc_reading_counter;
extern char tds_value[5];
extern char tds_header_version;		//v2.2.9
//******Temp. Comp. Testing***********//		//v2.2.9
extern float tds_at_25,error_per_degree,total_error_correction,temperature_difference;
extern float temperature_error_factor;
//************************************//


//extern unsigned long temperature_array[AVG_READING_TEMP],avg_temp_count;
extern unsigned int min_temp,max_temp,temp_calib_fact;
extern unsigned char water_temperature_level,temperature_reading_counter,temperature_failure_count,temperature_success_count; //v3.1.1.F //temp_reading_counter v3.1.1.F
extern unsigned long avg_temp_count; //v3.1.1.F
extern float temperature_reading;
extern char temp_value[6];
extern double temp_adc_volt;


extern unsigned char DS_temperature_array[3];
extern unsigned int DS_sensor_reading,avg_temperature_array[AVG_READING_TEMP]; //v3.1.1.F
extern float DS_temperature;


extern unsigned char error_stack[TOTAL_ERRORS];
extern char error_state[5];

extern unsigned int timer2_tick,sec_tick;	//timer1_tick,
extern unsigned long int min_tick,battery_verify_time_stamp,sleep_time_stamp,timer1_tick,chiller_heater_time_stamp;   //v3.1.3
extern unsigned int button_time_stamp,out_leak_time_stamp,in_leak_time_stamp,button_tick;  //flow_count_timestamp;		//diagnosis_time_stamp; //v3.1.2.G
extern unsigned int lls_time_stamp,hls_time_stamp,pag_err_time_stamp,led_time_stamp,solar_time_stamp;
extern unsigned int active_awake_time_stamp;
extern unsigned int sch_time_stamp,refill_time_stamp;
extern unsigned int button_timeout,display_toggle_time_stamp;

extern unsigned int user_timeout,dispense_timeout,leak_timeout;
extern unsigned int dump_timeout,refill_timeout,cleaning_timeout;
extern unsigned int error_display_time,logout_idle_time,lit_empty_tank,tank_low_level_litre;
extern unsigned int out_flow_calibration,in_flow_calibration;
extern unsigned int led_board_timeout;
extern unsigned char unknown_access_count;	//v2.2.2
extern unsigned int sleep_mode_timeout;		//v2.2.4

extern float flow_offset/*, in_flow_calibration*/;

//--extern unsigned char fresh_eeprom;
extern unsigned long templong;
extern unsigned char signature_byte[8];

extern unsigned char lcd_data_reg;
extern unsigned char lcd_rs_pin,dispense_button_count;

extern char lcd_data[17];
//---extern char lcd_data_freeze[10][16];	//10 line storage for LCD data
extern char lcd_line1_data_freeze[17];
extern char lcd_line2_data_freeze[17];
extern unsigned char device_reset_state;//,byte_variable;		//,int_variable,lcd_freeze_counter;
extern unsigned int byte_variable;	//v2.2.9
extern unsigned char rfid_card_type,system_error,litre_transcation,function_state;
extern unsigned int unauth_count,previous_adc_channel ;
extern unsigned char dispense_trail; //no_dispense_error_count //V6
extern float temp_unauth_input;
extern unsigned int school_disp_amount;
extern unsigned char school_mode_en, sch_mode_acc_id[9];//3.1.0		// Latch byte for school mode
extern unsigned char sch_disp_send_server;
extern unsigned char corporate_mode_en,corp_mode_acc_id[9]; //3.1.0 //3.1.2
extern unsigned char server_failure_counter,network_failure_counter,validation_error_counter,new_sms_location;
extern unsigned int data_error_validation_code,data_error_correct_value;
extern unsigned char eeprom_access;
extern unsigned char data_error_buffer[20], validate_para_cnt;
extern unsigned char temp_counter,temp_cach;	//data_error_occurance,
extern float sch_disp_fraction;

/*******************************************************Coin UCA2******************************/
extern unsigned int coin_amount_paisa;
extern float coin_water_tariff;
extern unsigned char coin_amount;

extern unsigned int coin_time_stamp,total_coin_amount,coin_ideal_time_stamp,total_coin_count; 
extern unsigned int coin_collector_capacity;
extern unsigned char coinbox_counter,eeprom_channel_read;        
extern unsigned char coin_channel_no,coin_id_channel_no,coin_channel_En_Dis,channel_cmd;
extern unsigned char bit_extract_factor;
extern unsigned char coinbox_data[7];
//extern unsigned char enable_UCA_cmd[6],disable_UCA_cmd[6],UCA_status_check_cmd[6];
//extern unsigned char en_dis_UCA_channel_cmd[7];
//extern unsigned char disable_channels_UCA_cmd[7];
extern unsigned char coin_rs_status[14];
		extern unsigned char temp_coin_id[13];
		extern unsigned char accepted_coin_id[13];
               
extern unsigned char coin_send_data_arr[20];
extern struct card_id{
    unsigned char channel_1[9]; //  in all channel_1to8[13] to [9]  --2.3.A
    unsigned char channel_2[9];
    unsigned char channel_3[9];
    unsigned char channel_4[9];
    unsigned char channel_5[9];
    unsigned char channel_6[9];
    unsigned char channel_7[9];
    unsigned char channel_8[9];
}coin_id;
extern unsigned char coin_init_try; 

extern struct flags_group_9
{
	unsigned coinselect						:1;					//1
	unsigned coinprocess				    :1;					//2
	unsigned coinenable						:1;					//3
	unsigned coinidle						:1;					//4	
	unsigned coin_receive_process			:1;		//5	
	unsigned coin_checksum                  :1;				//6	
	unsigned coin_total_amount_write        :1;		//7	
	unsigned coin_debug                     :1;	    //8	
//	unsigned coin_disable_by_error          :1;		//9
	unsigned coin_collector_alert           :1;		//10	
	unsigned coin_collector_error           :1;		//11
	unsigned coin_channel_status            :1;			//12
    unsigned coin_disable_by_fs_error       :1;	//13		//v2.3.6
    unsigned coin_FS_en_dis_auto_log_of     :1;	//14		//v2.3.6
    unsigned coin_disable_fault_alert       :1;		//15		//v2.3.6
}coin_flags;
/*******************************************************Coin UCA2 E******************************/

//-----------------------------  RFID CARD S---------------------------------------//
extern unsigned char command_array[35]; //v3.1.1.B 30

//extern  char Rfid_Response_array[30];
//extern unsigned char rfid_fetch_data[30];  //v3.1.1.B
extern unsigned char rfid_element;
extern unsigned int Rfid_time_stamp;
extern unsigned char Rfid_loop;
extern unsigned char offline_rfid_card[9];		//v3.0.0
extern unsigned char Rfid_addition_type;
extern unsigned char Rfid_remove_type;
extern unsigned int card_init_balance;
extern unsigned char rfid_read_fail_cntr; //3.1.2.J
extern unsigned char cmd_response_count ; //3.1.3.B

//-----------------------------  RFID CARD E---------------------------------------//

//-----------------------------  SD CARD S---------------------------------------//
/*extern unsigned char transaction_dir[20];
extern unsigned char user_dir[20];
extern unsigned char total_dir[8];*/ 
//extern unsigned char cur_dir[20];  //v3.1.1.B

extern unsigned int general_time_stamp_logout;
extern unsigned char file_name[20];
extern unsigned int sd_year;
extern unsigned char sd_month,sd_date,sd_hour,sd_min,sd_sec,sd_yr,P_month,P_date,P_yr,last_year_backup,last_month_backup,last_date_backup;
extern unsigned char temp_sd_bytes[20];
//extern FSFILE *pointer;
extern FSFILE *sd_pointer;
extern volatile unsigned char SD_Buffer[MAX_DATA_LENGTH];  //1024
extern unsigned char server_sync_status;			//v3.0.0
extern unsigned long int SD_lastbyte_Pointer;
extern char str_test[1000];
extern char * pch;
extern unsigned int char_location;
//extern char strd[];
extern unsigned int cntr_SD;
extern unsigned int Protocol_No;
extern unsigned int sd_data_count,err_pen_file_counter;

extern double daily_total_dispense;
extern unsigned char sd_delay;
//------------offline---
extern char receiveBuffer[50];
extern float offline_water_tariff;
extern unsigned char curnt_user_bal_ary[7];  //2.3.B
extern unsigned long int curnt_user_bal_paisa,previous_balance,Rfid_user_bal_paisa;
//extern unsigned int curnt_user_bal_rs;
extern double curnt_user_bal_rs;   
extern double Rfid_user_bal_rs;   
extern unsigned char user_balance_factor;

extern unsigned long int daily_total_lit;
extern unsigned char daily_total_lit_ary[10];
//extern unsigned char user_bal_file[25];  //v3.1.1.B
extern unsigned char file_response;
extern char sd_sync_buffer[20];  //v3.1.1.B  19
extern unsigned long int sync_file_lastbyte_Pointer;
extern unsigned char single_date_sync;
extern unsigned char card_presense_fail_cntr;


//-----------------------------  SD CARD E---------------------------------------//

//-------------------------------------BUTTON AND MENU-------------------------------------///
extern unsigned int button_detect_timestamp;
extern unsigned char button_detect;
extern unsigned int sensor_detect_timestamp;  //1.0.1
extern unsigned int Rfid_detect_timestamp; //3.1.2
//extern unsigned char sensor_detect;
extern unsigned char error_type2;
extern unsigned char heater_temperature_disp;
//extern unsigned long temp_adc_result_1;
extern unsigned char calibration_mode_en;


extern struct flags_group_1
{
	unsigned send_idle_msg                  :1;		//1
	unsigned check_server_connection        :1;     //2
	unsigned local_ip                       :1;		//3
	unsigned send_ip_port                   :1;		//4
	unsigned send_apn                       :1;		//5
	unsigned force_reconnect                :1;		//6
	unsigned system_stability               :1;		//7
	unsigned server_response                :2;     //8,9
	unsigned server_connection              :2;		//10,11
	unsigned send_sys_setting               :2;		//12,13
	unsigned disp_led_state                 :3;		//14,15,16
	unsigned server_chiller_heater_status   :3;     //17,18,19
	unsigned battery_auto_correct           :1;	    //19	v2.2.7
	unsigned super_admin                    :1;		//20	v2.2.9
	unsigned ultra_super_admin              :1;     //21
	unsigned ultra_super_admin_state        :1;     //22
	unsigned auto_sync_flag                 :1;	    //23
}system_flags;


extern struct flags_group_8
{
	unsigned sms_sender_ok	                :1;	     //1
	unsigned solar_ack_sent                 :1;      //2
	unsigned sms_permit                     :1;      //3
	unsigned sim_card_check                 :1;      //4
	unsigned fact_password_ok               :1;      //5
	unsigned is_sms_from_server             :2;      //6,7
	unsigned scan_sms                       :1;	     //8
	unsigned ac_ack_sent                    :1;   	 //9
	unsigned temp_sensor_failure_sent       :1;      //10
    unsigned msg_get                        :1;    	 //11   
    unsigned signal_strength_check          :1;      //12  //v3.1.1.D    
}gsm_flags;

extern struct flags_group_2
{
	unsigned run_diagnostic                 :1;		//1
	unsigned diagnostic_test_toggle         :1;	    //2
	//unsigned soft_mcu_reset     :1;//3
	unsigned op_mode                        :5;   	//3,4,5,6,7
	unsigned op_sub_mode                    :3;   	//8,9,10
	unsigned prev_op_mode                   :5;		//11,12,13,14
	unsigned forced_function_state          :1;  	//15
	unsigned device_active_awake            :1;		//16
	unsigned validation_error               :1;		//17
//	unsigned franchisee_mode:1;
	unsigned op_coin_mode                   :1;	 //18 v2.2.9
}mode_flags;

	
extern struct flags_group_3
{
	unsigned rfid_read                     :1;     //1
	unsigned dispense                      :1;	   //2
//	unsigned dispense_type:2;			//3
	unsigned dispense_button               :1;     //4
	unsigned acc_cummulative               :1;     //6
	unsigned acc_switch_detect             :1;     //7
	unsigned freeze_display                :1;	   //8
	unsigned output_flow_sensed            :1;	   //9
	unsigned coin_read                     :1;	   //10 v2.2.9
    unsigned card_detect                   :1;
    unsigned card_detect_first_time        :1;   //3.1.2
	unsigned dispense_error                :1;
    unsigned dispense_complete             :1;
}consumer_flags;

extern struct flags_group_4
{
	unsigned input_flow_sensed            :1;		//80
	unsigned remote_water_dump            :1;		//21
}dump_flags;

extern struct flags_group_5
{
	unsigned input_flow_sensed            :1;	 	//1
	unsigned refill_sub_mode              :2;	    //2
	unsigned auto_refill_flag             :1;		//3

}refill_flags;

extern struct flags_group_6
{
/*	unsigned dev_en_dis :1;				//1
	unsigned tds_en_dis :1;				//2
	unsigned lls_processing :1;			//3
	unsigned lls_trigger :1;			//4
	unsigned hls_processing :1;			//5
	unsigned hls_trigger :1;			//6
	unsigned lls_pin :1;				//7
	unsigned hls_pin :1;				//8
	unsigned lls_hls_en_dis :2;			//9,10
	unsigned tds_update:1;				//11
	unsigned pag_buzzer_control:1;		//12
	unsigned peripheral_interrupt:1;	//13
	unsigned solar_sense:1;				//15
	unsigned sv_on:1;					//17
	unsigned cold_water:1;				//19
	unsigned temp_en_dis:1;				//20
	unsigned sleep_mode_flag:1;			//21
	unsigned chiller_status_flag:1;		//22
	unsigned franchisee_tag_check:1;    //23	//v2.2.5
	unsigned chiller_en_dis:1;          //24v2.2.6
	unsigned chiller_start_flag:1;      //25
    unsigned solar_processing:1;        //26   //v2.2.6
	unsigned solar_pin:1;	            //27
	unsigned chiller_lls_trigger:1;     //28 //v2.2.6
	unsigned lcd_refresh:1;				//29	//v2.2.7
	unsigned temp_compen_en_dis:1;		//30	//v2.2.9
	unsigned coin_en_dis:1;				//31	//v2.2.9
	unsigned user_enter_password:1;
    unsigned head_key_get             :1;*/
    
    unsigned dev_en_dis                  :1;       //1
    unsigned tds_en_dis                  :1;       //2

    unsigned lls_processing              :1;       //3
    unsigned lls_trigger                 :1;       //4
    unsigned hls_processing              :1;       //5
    unsigned hls_trigger                 :1;       //6
    unsigned lls_pin                     :1;       //7
    unsigned hls_pin                     :1;       //8
    unsigned lls_hls_en_dis              :2;       //9,10

    unsigned tds_update                  :1;       //11
    unsigned pag_buzzer_control          :1;       //12
    unsigned peripheral_interrupt        :1;       //13
    unsigned solar_sense                 :1;       //15
    unsigned sv_on                       :1;       //17
    unsigned cold_water                  :1;       //19
    unsigned temp_en_dis                 :1;       //20
    unsigned sleep_mode_flag             :1;       //21
    unsigned chiller_status_flag         :1;       //22
    unsigned heater_status_flag          :1;	   //23	//v3.1.3
	unsigned warm_water                  :1;	   //34	//v3.1.3
    unsigned franchisee_tag_check        :1;       //23         //v2.2.5
    unsigned chiller_heater_start_flag   :1;       //24         //v3.1.3    
    unsigned chiller_heater_en_dis       :2;       //25,26      //v3.1.3
    unsigned solar_processing            :1;       //26   //v2.2.6
    unsigned solar_pin                   :1;       //27
    unsigned chiller_heater_lls_trigger  :1;       //28                      //v3.1.3
    unsigned lcd_refresh                 :1;       //29	//v2.2.7
    unsigned temp_compen_en_dis          :1;       //30	//v2.2.9
    unsigned coin_en_dis                 :1;      //31	//v2.2.9
    unsigned user_enter_password         :1;
    unsigned head_key_get                :1;
    unsigned lls_last_first_state        :3;
    unsigned lls_last_second_state       :3;
}peripheral_flags;
 
extern struct flags_group_7
{
	unsigned pag_error                   :1;        //1
	unsigned pag_error_sent              :1;	    //2
	unsigned apn_error                   :1;        //3
	unsigned low_batt_alert_sent         :1;	    //5
	unsigned tds_high                    :1;	    //6
	unsigned tds_low                     :1;	 	//7
	//unsigned reset_all_ooo:1;			//9
	unsigned modem_sw_reset              :1;		//9
	unsigned water_expired               :1;		//10
	unsigned modem_response_error        :2;    	//11,12
	unsigned tds_error_sent              :1;		//13
	unsigned data_error_auto_correct     :1;	    //14
	unsigned tank_empty                  :1;    	//15
	unsigned coin_error_sent             :1;	    //16
    unsigned chiller_heater_relay_reset  :1;    	//17	//v3.1.2.H
//     unsigned battery_critically_low:1;	//17	//v3.1.2.H
}error_flags;

extern struct flags_group_12
{
	unsigned display_toggle	            :2;		//1,2
	unsigned display_mode 	            :1;		//3
	unsigned freeze_display	            :1;		//4
}display_flags;
extern struct flags_group_13
{
//	unsigned newfile_create	:1;	
	unsigned card_initialize            :1;
    unsigned card_detect                :1;
	unsigned sd_card_format             :1;	
    unsigned Err_Pen_search             :1;    
}sd_flags;

extern struct flags_group_14
{
	unsigned exit                       :1;
	unsigned timeout_menu_options       :1;
	unsigned function_access            :1;
//	unsigned dispense_button: 1;
//@	unsigned dispense_type: 1;
	unsigned admin_access               :1;
	unsigned any_button_press           :1;
}menu_flags;


extern struct flags_group_10
{	
	unsigned sms_check                  :1;   	//1
	unsigned chiller_heater_control     :1;  	//2
	unsigned coin_communication_check   :1;  	//3
	unsigned coin_communication_retry   :1;  	//4
	unsigned analog_sensor_update       :1;		//5
	unsigned system_error_flag          :1;		//1
	unsigned mode_refresh_check         :1;  	//1
	unsigned check_temperature          :1;     //1.0.1
    unsigned modem_status_check         :1;     //v3.1.2.J
    unsigned modem_status_check_en_dis  :1;     //v3.1.2.J
}timer_flags; 
extern struct flags_group_15
{
unsigned sync_by_modem                  :1;
unsigned time_halt                      :1;
unsigned sync_type                      :3;
unsigned sync_by_time                   :2;
unsigned halt_time_check                :1;
unsigned date_change                    :1;
unsigned month_change                   :1;
}rtc_flag;  


extern unsigned char daily_dispense_sms_send_status;	//v3.0.0
extern unsigned char server_sync_status;			//v3.0.0




//
//#define OPT_DUMP_TANK          		1
//#define OPT_REFIL_TANK         		2
//#define OPT_SCHOOL_MODE_EN_DIS		3
//#define OPT_CORPORATE_MODE_EN_DIS	4
////#define OPT_CHILLER_ON_OFF		5   // v2.2.6
//#define OPT_MAINTAINANCE_REQ   		5
//#define OPT_ADD_NEW_CARD   			6	//v3.0.0
//#define OPT_REMOVE_CARD   			7	//v3.0.0
//#define OPT_ADD_BALANCE   			8	//v3.0.0
//#define OPT_SYSTEM_INFO      		9
//#define OPT_EXIT_MENU          		10

//main.c file


    //	while((system_flags.server_connection!=SERVER_CONN_OK)&&(temp_cntr_1<2))
    //	{
    //		connect_to_server();
    //		check_server_connection();	//v2.2.7
    //		temp_cntr_1++;	
    //	}
    //	temp_cntr_1 = 0;
    //	if(system_flags.server_connection==SERVER_CONN_OK)
    //	{
    //		send_server_response(DEV_CONNECTED);
    //		clear_lcd();
    //		LCD_BACKLITE_OFF;		//disable for v2.2.9	
    //	}

//    network_state = INITIAL;
//    while ((network_state != REGISTERED) && (network_state != REG_ROAMING)&&(temp_cntr_1 < 2))
//    {
//        network_check();
//        temp_cntr_1++;
//    }
//    temp_cntr_1 = 0;   

    //@	if(system_flags.server_connection==SERVER_CONN_OK){
    //		send_server_response(DEV_CONNECTED);   //3.1.0
    //		clear_lcd();
    //		LCD_BACKLITE_OFF;		//disable for v2.2.9
    //@	}
    //------------------------------- MODEM Check & SERVER Connect E   ----------------------------------//	

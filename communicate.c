#include <stdio.h>
#include <stdlib.h>

#include "communicate.h"
#include "main.h"
#include "serial_uart.h"
#include "variable.h"
#include "modem.h"
#include "ext_lcd.h"
#include <string.h>
#include "FSconfig.h"
#include "FSIO.h"
#include "DS1307.h"
#include "mode.h"
#include "server.h"
#include "mcu_timer.h"
#include "peripherial.h"

void send_server_response(unsigned int response_type) { //v2.2.9
    //unsigned char data = 0;
    unsigned int data = 0; //v3.0.0
    //	unsigned int cntr_1=0,cntr_SD = 0;	//v3.1.1.C

    if (response_type > DEV_SENT_VIA_SMS)
    {
	data = response_type;
	data -= DEV_SENT_VIA_SMS;
    } else
    {
	data = response_type;
    }
    construct_response_data(data);
    construct_response_message();
    calculate_check_sum();

    if (response_type > DEV_SENT_VIA_SMS)
    {
	lcd_init();
	strcpy(lcd_data, "SENDING SMS"); //v2.2.7
	lcd_display(1);
	strcpy(lcd_data, "PLS WAIT...");
	lcd_display(2);
	send_sms();
    } else
    {
	if (response_type != DEV_SERVER_SYNC)
	{
	    if (response_type == DEV_DAILY_WATER_DISPENSE)
	    {
		all_log_store_sdcard(DATE_CHANGE_FLAG_ON);
	    } else
	    {
		all_log_store_sdcard(DATE_CHANGE_FLAG_OFF);    
                 
	    }

	} else
	{
	    send_to_server();
	}
    }
    memset(transmitData, 0, sizeof (transmitData)); //1.0.1

    rfid_to_server_msg = 0;
    server_send_message_type = 0;


}
//void construct_response_data(unsigned char data)

void construct_response_data(unsigned int data) //v2.2.9
{
    //unsigned char count=0;
    unsigned int count = 0; //v2.2.9
    //	unsigned int cntr_SD = 0;	//v3.0.0  //v3.1.1.C
    //	unsigned char temp_array[6]="";  //v3.1.1.C

    for (count = 0; count<sizeof (txn_type); count++)
	txn_type[count] = 0;

    for (count = 0; count<sizeof (temp_data); count++)
	temp_data[count] = 0;

    rfid_to_server_msg = data; //v2.2.9 -> rfid_to_server_msg changed form unsigned char to unsigned int

    //	if((rfid_to_server_msg!=DEV_DEVICE_IC_CHANGED)&&(rfid_to_server_msg!=DEV_DS_COMMAND))  //ask //3.0.2 18/11/16
    if (rfid_to_server_msg != DEV_DEVICE_IC_CHANGED)
    {
	for (count = 0; count<sizeof (msg_data); count++)
	    msg_data[count] = 0;
    } else{}
   
    //-----set server send message type 
    if (((rfid_to_server_msg >= DEV_TXN_LIMIT_LOW_1)&&(rfid_to_server_msg <= DEV_TXN_LIMIT_HIGH_1)) || ((rfid_to_server_msg >= DEV_TXN_LIMIT_LOW_2)&&(rfid_to_server_msg <= DEV_TXN_LIMIT_HIGH_2)))
	server_send_message_type = TXN_MSG;

    else if (((rfid_to_server_msg >= CONFIG_PARAMETER_LIMIT_LOW_1)&&(rfid_to_server_msg <= CONFIG_PARAMETER_LIMIT_HIGH_1)) || ((rfid_to_server_msg >= CONFIG_PARAMETER_LIMIT_LOW_2)&&(rfid_to_server_msg <= CONFIG_PARAMETER_LIMIT_HIGH_2)))
	server_send_message_type = CONFIG_MSG;

    else if (((rfid_to_server_msg >= READ_SETTINGS_LIMIT_LOW_1)&&(rfid_to_server_msg <= READ_SETTINGS_LIMIT_HIGH_1)) || ((rfid_to_server_msg >= READ_SETTINGS_LIMIT_LOW_2)&&(rfid_to_server_msg <= READ_SETTINGS_LIMIT_HIGH_2)))
	server_send_message_type = SETTING_MSG;

    else if (((rfid_to_server_msg >= MISC_MSG_LIMIT_LOW_1)&&(rfid_to_server_msg <= MISC_MSG_LIMIT_HIGH_1)) || ((rfid_to_server_msg >= MISC_MSG_LIMIT_LOW_2)&&(rfid_to_server_msg <= MISC_MSG_LIMIT_HIGH_2)))
	server_send_message_type = MISC_MSG;

    else if (((rfid_to_server_msg >= DATA_ALERT_MSG_LIMIT_LOW_1)&&(rfid_to_server_msg <= DATA_ALERT_MSG_LIMIT_HIGH_1)) || ((rfid_to_server_msg >= DATA_ALERT_MSG_LIMIT_LOW_2)&&(rfid_to_server_msg <= DATA_ALERT_MSG_LIMIT_HIGH_2)))
	server_send_message_type = DATA_ALERT_MSG;

    else
	server_send_message_type = ALERT_MSG;

    //-------set specific code for server send message	
switch (rfid_to_server_msg)
{
//--------------------           Transaction Msg         -------------------------------------------  //

//  case DEV_AUTHENTICATE:                                            //1			
//	    sprintf(txn_type,"RV02");
//	    break;

    case DEV_BALANCE_DEDUCT:                                                    //3
	sprintf(txn_type, "RB03");

	if (balance_deduct >= (acc_dispense_capacity + 3))
	    balance_deduct = acc_dispense_capacity + 3;
	sprintf(msg_data, "%2.1f", ((double) balance_deduct));

	break;
    case DEV_USER_ADD:                                                          //121
	sprintf(txn_type, "RV03");
	break;

    case DEV_USER_REMOVE:                                                       //122
	sprintf(txn_type, "RV04");
	break;

    case DEV_BAL_ADD:                                                           //123
	sprintf(txn_type, "RB04");
	break;
	//--------------------           Configure Parameters   -----------------------------------------------------------  //	

    case DEV_HB_MSG:                                                            //4		
	sprintf(txn_type, "RA05");
	sprintf(msg_data, "%u", idle_msg_freq);

	//Dev,LLS,HLS,TDS,WE,Charging,AC,Solar
	sprintf(temp_data, "0,0,0,0,0,0,0,0");
	if (peripheral_flags.dev_en_dis)
	    temp_data[0] = '1';
	else{}
	
	if (peripheral_flags.lls_hls_en_dis < 2) //LLS disabled
	    temp_data[2] = 'D';
	else if (peripheral_flags.lls_trigger)
	    temp_data[2] = '1';
	else{}


	if ((peripheral_flags.lls_hls_en_dis == 0) || (peripheral_flags.lls_hls_en_dis == 2)) //HLS disabled
	    temp_data[4] = 'D';
	else if (peripheral_flags.hls_trigger)
	    temp_data[4] = '1';
	else{}
	

	if (peripheral_flags.tds_en_dis)
	    temp_data[6] = '1';
	else{}
	

	if (error_flags.water_expired)
	    temp_data[8] = '1';
	else{}
	

//	else if(peripheral_flags.chiller_status_flag==TRUE)		//chiller
//		temp_data[8] = 'C';
//	else{}

//@	if(peripheral_flags.battery_charging)
//		temp_data[10] = '1';
//	else{}			
//
//	if(peripheral_flags.ac_mains_sense)
//		temp_data[12] = '1';
//@	else{}
	
	temp_data[10] = '0'; //peripheral_flags.battery_charging
	temp_data[12] = '1'; //peripheral_flags.ac_mains_sense
	if (peripheral_flags.solar_sense)
	    temp_data[14] = '1';
	else{}
	break;

    case DEV_USER_TIMEOUT:							//5
        sprintf(txn_type, "RA01");
        sprintf(msg_data, "%.4d", user_timeout);
	break;

    case DEV_DISP_TIMEOUT:							//6
	sprintf(txn_type, "RA02");
	sprintf(msg_data, "%.4d", dispense_timeout);
	break;

    case DEV_DUMP_TIMEOUT:							//7
	sprintf(txn_type, "RA03");
	sprintf(msg_data, "%.4d", dump_timeout);
	break;

    case DEV_REFILL_TIMEOUT:							//8
	sprintf(txn_type, "RA04");
	sprintf(msg_data, "%.4d", refill_timeout);
	break;

    case DEV_CLEANING_TIMEOUT:							//9
	sprintf(txn_type, "RA22");
	sprintf(msg_data, "%.4d", cleaning_timeout);
	break;

    case DEV_SCHOOL_DISPENSE_AMT:						//10
	sprintf(txn_type, "RA06");
	sprintf(msg_data, "%u", school_disp_amount);
	break;

    case DEV_ER_DISPLAY_TIMER:							//11
	sprintf(txn_type, "RA07");
	sprintf(msg_data, "%.4d", error_display_time);
	break;

    case DEV_DISPENSE_LT_LC:							//12
	sprintf(txn_type, "RA08");
	sprintf(msg_data, "%.2f", ((double) DISPENSE_LEAST_COUNT));
	break;

    case DEV_LOGOUT_IDLE_TIMEOUT:						//13
	sprintf(txn_type, "RA09");
	sprintf(msg_data, "%.4d", logout_idle_time);
	break;

    case DEV_LT_EMPTY_TANK:							//14
	sprintf(txn_type, "RA10");
	sprintf(msg_data, "%.4d", lit_empty_tank); 
	break;

    case DEV_OUT_FLOW_CALIB:							//15
	sprintf(txn_type, "RA11");
	sprintf(msg_data, "%.4d", out_flow_calibration);
	break;

    case DEV_IN_FLOW_CALIB:							//16
	sprintf(txn_type, "RA12");
	sprintf(msg_data, "%.4d", in_flow_calibration);
	break;

    case DEV_TDS_CALIB:                                                         //17
	sprintf(txn_type, "RA13");
	sprintf(msg_data, "%.4d", tds_calib_fact);
	break;

    case DEV_DEVICE_EN_DIS:                                                     //18
	sprintf(txn_type, "RA14");
	if (peripheral_flags.dev_en_dis == HIGH)
	    sprintf(msg_data, "1");
	else if (peripheral_flags.dev_en_dis == LOW)
	    sprintf(msg_data, "0");
	else{}
	break;

    case DEV_TDS_EN_DIS:                                                        //19
	sprintf(txn_type, "RA15");
	if (peripheral_flags.tds_en_dis == HIGH)
	    sprintf(msg_data, "1");
	else if (peripheral_flags.tds_en_dis == LOW)
	    sprintf(msg_data, "0");
	else{}	
	break;

    case DEV_LLS_EN_DIS:                                                        //20
	sprintf(txn_type, "RA16");
	if (peripheral_flags.lls_hls_en_dis >= 2)
	    sprintf(msg_data, "1");
	else if (peripheral_flags.lls_hls_en_dis < 2)
	    sprintf(msg_data, "0");
	else{}	
	break;

    case DEV_HLS_EN_DIS:  //21
        
        sprintf(txn_type, "RA17");
	if ((peripheral_flags.lls_hls_en_dis == 1) || (peripheral_flags.lls_hls_en_dis == 3))
	    sprintf(msg_data, "1");
	else if ((peripheral_flags.lls_hls_en_dis == 0) || (peripheral_flags.lls_hls_en_dis == 2))
	    sprintf(msg_data, "0");
	else{}
	break;

    case DEV_TDS_MIN:                                                           //22
	sprintf(txn_type, "RA18");
	sprintf(msg_data, "%.4d", min_tds);
	break;

    case DEV_TDS_MAX:								//23
	sprintf(txn_type, "RA19");
	sprintf(msg_data, "%.4d", max_tds);
	break;

    case DEV_NEW_PASSWORD_SET:                                                  //24
	sprintf(txn_type, "RA20");
	sprintf(msg_data, "%.4s", current_password); 
	break;

    case DEV_LEAK_TIMER:                                                        //26
	sprintf(txn_type, "RA30");
	sprintf(msg_data, "%.4d", leak_timeout);
	break;

    case DEV_DEVICE_IC_CHANGED:                                                 //27
	sprintf(txn_type, "RA23");
	//Old device ID copied to "msg_data" during sms processing
	break;

    case DEV_DEBUG_MODE:                                                        //28
	sprintf(txn_type, "RA24");
	if (mode_flags.op_mode == DEBUG_MODE)
	    sprintf(msg_data, "1");
	else
	    sprintf(msg_data, "0");
	break;

    case DEV_N_W_SIGNAL_STRENGTH:                                               //29
	sprintf(txn_type, "RA25");
	sprintf(msg_data, "%s", network_signal_strength);
	break;

    case DEV_SOFT_RESET:                                                        //30
	sprintf(txn_type, "RA26");
	sprintf(msg_data, "0"); //Indicate soft reset
	break;

    case DEV_BUZZER_CTRL:                                                       //31
	sprintf(txn_type, "RA27");
	sprintf(msg_data, "%u", peripheral_flags.pag_buzzer_control);
	break;

    case DEV_DIAGNOSTIC_TEST:                                                   //32
	sprintf(txn_type, "RA29");
	if (mode_flags.op_mode == DIAGNOSTIC_MODE)
	    sprintf(msg_data, "1");
	else
	    sprintf(msg_data, "0");
	break;
	
    case DEV_SCHOOL_MODE:						        //33
	sprintf(txn_type, "RA33");
	if (mode_flags.op_mode == SCHOOL_MODE)
	    sprintf(msg_data, "1");
	else
	    sprintf(msg_data, "0");
	break;

    case DEV_SMS_PERMIT:							//37
	sprintf(txn_type, "RA32");
	sprintf(msg_data, "%u", gsm_flags.sms_permit);
	break;

    case DEV_DFLT_PARA:								//36
	sprintf(txn_type, "RA34");
	sprintf(msg_data, "1"); // Assuming the EEPROM is written correctly
	break;

//    case DEV_FULL40_VALUE:							//38
//	sprintf(txn_type, "RA35");
//	sprintf(msg_data, "%u", full40_val); // Assuming the EEPROM is written correctly
//	break;

    case DEV_SCHOOL_MODE_ACC_ID:						//39
	sprintf(txn_type, "RA36");
	strcpy(msg_data, sch_mode_acc_id); // Assuming the EEPROM is written correctly
	break;

    case DEV_MASTER_CONT_NO:							//40
	sprintf(txn_type, "RA37");
	strcpy(msg_data, master_contact); // Assuming the EEPROM is written correctly
	break;

    case DEV_DATA_ER_AUTO_CORRECT:						//41
	sprintf(txn_type, "RA38");
	sprintf(msg_data, "%u", error_flags.data_error_auto_correct);
	if (strlen(sms_sender_no) < 3)
	    strcpy(sms_sender_no, "0000000000");
	else{}
	break;

    case DEV_HYST_COUNT:							//42
	sprintf(txn_type, "RA39");
	sprintf(msg_data, "%u", 0); // Assuming the EEPROM is written correctly  hyst_count =0
	break;

    case DEV_ANTENNA_MODE:							//44
	sprintf(txn_type, "RA41");
	if (mode_flags.op_mode == ANTENNA_MODE)
	    sprintf(msg_data, "1");
	else
	    sprintf(msg_data, "0");
	break;

    case DEV_GSM_MODE:								//46
	sprintf(txn_type, "RA43");
	if (mode_flags.op_mode == GSM_MODE)
	    sprintf(msg_data, "1");
	else
	    sprintf(msg_data, "0");
	break;

    case DEV_TEMPERATURE_CALIB:							//47
	sprintf(txn_type, "RA44");
	sprintf(msg_data, "%.4d", temp_calib_fact);
	break;

    case DEV_WATER_TEMPERATURE_LEVEL:			//v2.3.6
        sprintf(txn_type,"RA45");
        sprintf(msg_data,"%.3d",water_temperature_level);			//v2.3.6
	break;

    case DEV_TEMPERATURE_EN_DIS:						//49
	sprintf(txn_type, "RA46");
	sprintf(msg_data, "%u", peripheral_flags.temp_en_dis);
	break;

    case DEV_OUT_FLOW_FREQ_LIMIT:						//50
	sprintf(txn_type, "RA47");
	sprintf(msg_data, "%u", output_flow_freq_limit);
	break;

    case DEV_IN_FLOW_FREQ_LIMIT:						//51
	sprintf(txn_type, "RA48");
	sprintf(msg_data, "%u", input_flow_freq_limit);
	break;

    case DEV_SLEEP_MODE_MINUTES:						//52
	sprintf(txn_type, "RA49");
	sprintf(msg_data, "%u", sleep_mode_timeout);
	break;

    case DEV_AUTO_REFILL_FLAG:							//53
	sprintf(txn_type, "RA50");
	sprintf(msg_data, "%u", refill_flags.auto_refill_flag);
	break;

    case DEV_FRANCHISEE_RFID: //v2.2.5						//54
	sprintf(txn_type, "RA51 ");
	//	sprintf(msg_data,"%.12s",franchisee_rfid_tag);	// Assuming the EEPROM is written correctly  //ask
	sprintf(msg_data, "%.8s", franchisee_rfid_tag); // Assuming the EEPROM is written correctly
	break;

    case DEV_FRANCHISEE_RFID_VERIFIED: //v2.2.5					//55
	sprintf(txn_type, "RA52 ");
	if (peripheral_flags.franchisee_tag_check == 1)
	    strcpy(msg_data, "1");
	else
	    strcpy(msg_data, "0");
	break;

    case DEV_CORPORATE_MODE: //v2.2.5						//56
	sprintf(txn_type, "RA53");
	if (mode_flags.op_mode == CORPORATE_MODE)
	    sprintf(msg_data, "1");
	else
	    sprintf(msg_data, "0");
	break;

    case DEV_CORPORATE_MODE_ACC_ID: //v2.2.5					//57
	sprintf(txn_type, "RA54");
	strcpy(msg_data, corp_mode_acc_id); // Assuming the EEPROM is written correctly
	break;
    
    case DEV_CHILLER_HEATER_EN_DIS:		//v2.2.6	//v2.3.6   //58
        sprintf(txn_type,"RA55");
        if(peripheral_flags.chiller_heater_en_dis == 1)			//v2.3.6
            sprintf(msg_data,"1");				// Assuming the EEPROM is written correctly
        else if(peripheral_flags.chiller_heater_en_dis == 2)	//v2.3.6
            sprintf(msg_data,"2");				
        else
            sprintf(msg_data,"0");
    break;

    case DEV_TDS_VERSION: //v2.2.9						//59
	sprintf(txn_type, "RA56");
	sprintf(msg_data, "%c", tds_header_version);
	break;

    case DEV_TEMP_COMPEN_EN_DIS: //v2.2.9					//60
	sprintf(txn_type, "RA57");
	if (peripheral_flags.temp_compen_en_dis == TRUE)
	    sprintf(msg_data, "1");
	else
	    sprintf(msg_data, "0");
	break;

	//****************************************************************************************//

  case DEV_COIN_EN_DIS:							//61
	sprintf(txn_type, "RA58");
	sprintf(msg_data, "%u", peripheral_flags.coin_en_dis);
	break;

  case DEV_COIN_CHN_STATUS:							//62
	sprintf(txn_type, "RA59");
	sprintf(msg_data, "%.3d", channel_cmd);
	break;

  case DEV_TOTAL_COIN_AMOUNT:							//63
	sprintf(txn_type, "RA60");
	if (total_coin_amount == 55555)
	    sprintf(msg_data, "%s", "ERROR");
	else
	    sprintf(msg_data, "%.5d", total_coin_amount);
	break;

  case DEV_TOTAL_COIN_COUNT:							//64	
	sprintf(txn_type, "RA61");
	if (total_coin_count == 55555)
	    sprintf(msg_data, "%s", "ERROR");
	else
	    sprintf(msg_data, "%.5d", total_coin_count);
	break;

  case DEV_COIN_WATER_TARIFF:							//65
	sprintf(txn_type, "RA62");
	sprintf(msg_data, "%2.1f", coin_water_tariff);
	break;

  case DEV_COIN_COLLECT_CAPACITY:						//66
	sprintf(txn_type, "RA63");
	sprintf(msg_data, "%.5d", coin_collector_capacity);
	break;

  case DEV_COIN_CHN_EN_DIS:							//67
	sprintf(txn_type, "RA64");
	sprintf(msg_data, "%.1d,%.1d", coin_channel_no, coin_channel_En_Dis);
	break;

  case DEV_COIN_ID:								//68
	sprintf(txn_type, "RA65");
	sprintf(msg_data, "%.1d,%.12s", coin_id_channel_no, temp_coin_id);
	break;

  case DEV_RESET_COIN_AMOUNT:							//69
	sprintf(txn_type, "RA66");
	sprintf(msg_data, "%.5d,%.5d", total_coin_amount, total_coin_count);
	break;

    case DEV_TIME_SYNC_BY_MODEM:						//131
	sprintf(txn_type, "RA67");
	sprintf(msg_data, "%.4d%.2d%.2d%.2d%.2d%.2d", sd_year, sd_month, sd_date, sd_hour, sd_min, sd_sec);
	break;
	
    case DEV_OFFLINE_WATER_TARIFF:						//132
	sprintf(txn_type, "RA68");
	sprintf(msg_data, "%2.1f", offline_water_tariff);
	break;

    case DEV_MENU_PASSWORD_SET:							//133
	sprintf(txn_type, "RA69");
	memcpy(msg_data, menu_franchisee_password, 4);
	break;

    case DEV_FORMAT_SD_CARD:							//134
	sprintf(txn_type, "RA70");
	if (sd_flags.sd_card_format == TRUE)
	    sprintf(msg_data, "1");
	else
	    sprintf(msg_data, "0");
	break;

    case DEV_FETCH_RTC_TIME:							//135
	sprintf(txn_type, "RA71");
	sprintf(msg_data, "%.4d%.2d%.2d%.2d%.2d%.2d", sd_year, sd_month, sd_date, sd_hour, sd_min, sd_sec);
	break;
	
    case DEV_AUTO_SYNC_EN_DIS:							//136
	sprintf(txn_type, "RA72");
	sprintf(msg_data, "%u", system_flags.auto_sync_flag);
	break;   

//--------------------           Read Settings   -----------------------------------------------------------  //
    case DEV_SYSTEM_SETTINGS_1:							//70
	sprintf(txn_type, "RR01");
	break;

    case DEV_SYSTEM_SETTINGS_2:							//71
	sprintf(txn_type, "RR02");
	break;

    case DEV_SYSTEM_SETTINGS_3:							//72
	sprintf(txn_type, "RR03");
	sprintf(temp_data, "0,0,0,0,0");
	if (peripheral_flags.dev_en_dis)
	    temp_data[0] = '1';
	else{}	

	//---if(peripheral_flags.lls_trigger==TRUE)
	if (peripheral_flags.lls_hls_en_dis >= 2) //LLS enable
	    temp_data[2] = '1';
	else{}

	//---if(peripheral_flags.hls_trigger==TRUE)
	if ((peripheral_flags.lls_hls_en_dis == 1) || (peripheral_flags.lls_hls_en_dis == 3)) //HLS enable
	    temp_data[4] = '1';
	else{}
	if (peripheral_flags.tds_en_dis)
	    temp_data[6] = '1';
	else{}	

	if (error_flags.water_expired)
	    temp_data[8] = '1';
	else{}
	break;

    case DEV_SYSTEM_SETTINGS_4:							//73
	sprintf(txn_type, "RR04");
	break;

    case DEV_SYSTEM_SETTINGS_5:							//74
	sprintf(txn_type, "RR05");
	break;

    case DEV_SYSTEM_SETTINGS_6: //v2.2.5					//75
	sprintf(txn_type, "RR06");
	break;
//--------------------           Misc Msg   ---------------------------------------------------------------------  //
	
    case DEV_MODEM_COMMAND:							//76
	sprintf(txn_type, "RM01");
	break;

    case DEV_SMS_RX:								//77
	sprintf(txn_type, "RM02");
	sprintf(msg_data, "%.4s", sms_txn);
	break;

    case DEV_GENERAL_COMMAND:							//78
	sprintf(txn_type, "RM03");
	if (strlen(modem_cmd_response) < 5)
	    //sprintf(modem_cmd_response,"%.5u,%ld,%u,%1.3f,%s,%s",AcCurrent,avg_tds_count,tds_calib_fact,adc_volt,tds_value,factory_password);
	    sprintf(modem_cmd_response, "%.5u,%.3d,%ld,%u,%1.3f,%s,%.4u,%s", 0, 0, avg_tds_count, tds_calib_fact, adc_volt, tds_value, 0, factory_password);
	else{}
	break;

    case DEV_VALIDATION_ERROR:							//79
        sprintf(txn_type, "RM04");
        sprintf(msg_data, "%u", data_error_validation_code);
	break;
//--------------------           data alert msg     -------------------------------------------------------------//
	
    case DEV_TDS_HIGH:								//80
        sprintf(txn_type, "RL01");
        sprintf(msg_data, "%.4d", max_tds);
	break;

    case DEV_TDS_LOW:								//81
        sprintf(txn_type, "RL02");
        sprintf(msg_data, "%.4d", min_tds);
	break;

    case DEV_UNAUTHORISED_WATER_INPUT:						//82
        sprintf(txn_type, "RL03");
        sprintf(msg_data, "%2.1f", ((double) unauth_input_litre)); //disable for v3.0.0 testing
	break;

    case DEV_UNAUTHORISED_WATER_DISPENSE:					//83
        sprintf(txn_type, "RL04");
        sprintf(msg_data, "%2.1f", ((double) unauth_dispense_litre));
	break;

    case DEV_WRONG_PASSWORD:							//84
        sprintf(txn_type, "RL05");
        sprintf(msg_data, "%.4s", current_password); 
	break;

    case DEV_DUMP_COMPLETE:							//85
        sprintf(txn_type, "RL06");
        sprintf(msg_data, "%3.1f", ((double) dispense_litre));
	break;

    case DEV_COMMAND_RX:							//86
        sprintf(txn_type, "RL07");
        sprintf(msg_data, "%.4s", server_command);
	break;

    case DEV_REFILL_TANK:							//87
        sprintf(txn_type, "RL08");
        sprintf(msg_data, "%3.1f", ((double) refill_litre));
	break;

    case DEV_CONNECTED:								//89
        sprintf(txn_type, "RL17");
        sprintf(msg_data, "%u", device_reset_state);
	break;

    case DEV_SLEEP_MODE:							//90
	break;

    case DEV_AGE_IN_MIN:
        sprintf(txn_type, "RL21");
        sprintf(msg_data, "%ld", min_tick);
	break;

    case DEV_CURRENT_MODE:							//91
        sprintf(txn_type, "RL22");
        sprintf(msg_data, "%u", mode_flags.op_mode);
	break;

    case DEV_GSM_ACTIVATED:							//93
        sprintf(txn_type, "RL25");
        //sprintf(msg_data, "%.13s", network_service_provider);
        sprintf(msg_data, "%.25s", network_service_provider);
	break;

    case DEV_TEMP_SENSOR_FAILURE:						//94
	sprintf(txn_type, "RL26");
	sprintf(msg_data, "%.5s", temp_value);
	break;

    case DEV_FRANCHISEE_MAINTENANCE_RQ:						//95
	sprintf(txn_type, "RL27");
	if (mode_flags.op_mode == FRANCHISEE_MODE)
	    sprintf(msg_data, "%.8s", franchisee_rfid_tag); //@  change .12s to .8s
	else
	    sprintf(msg_data, "%.8s", rfid_card); //@  change .12s to .8s
	break;

    case DEV_TRAP_ERROR:							//96
        sprintf(txn_type, "RL28");
        strcpy(msg_data, error_state);
	break;

    case DEV_COIN_COLLECTOR_ERROR:		//v2.2.9			//97
	
       sprintf(txn_type,"RL29");
       if(total_coin_amount == 55555)
       sprintf(msg_data,"%s","ERROR");
       else
       sprintf(msg_data,"%.5d",total_coin_count);
	
	break;

	case DEV_COIN_COMMUNICATION_ERROR:					//98	    //v2.3.2	    
        sprintf(txn_type,"RL30");
        sprintf(msg_data,"%.4d",1);
	break;

    case DEV_DAILY_WATER_DISPENSE: //v3.0.0					//171
        sprintf(txn_type, "RL31");
        sprintf(msg_data, "%3.2f", ((double) daily_total_dispense));
	break;
	
    case DEV_SERVER_SYNC: //v3.0.0						//172
	sprintf(txn_type, "RL32");
	sprintf(msg_data, "%.1d", server_sync_status);
	break;
	
    case DEV_ERROR_TYPE2: //1.0.1						//173
        sprintf(txn_type, "RL33");
        sprintf(msg_data, "%d", error_type2);
	break;
	
    case DEV_SEND_ER_FILE_NAME:							//175
        sprintf(txn_type, "RL35");
        strcpy(msg_data, file_name);
	break;
	
    case DEV_ULTRA_SUPER_ADMIN_MODE:						//174
        sprintf(txn_type, "RL34");
        if (system_flags.ultra_super_admin_state == TRUE)
            sprintf(msg_data, "1");
        else
            sprintf(msg_data, "0");
	break;
    
    case DEV_FAULT_ERROR: //v2.3.6	- Heater / Chiller Relay failure
        sprintf(txn_type,"RL36");
        sprintf(msg_data,"%.5s",temp_value);
        break;    

	//--------------------           only alert msg---   -----------------------------------------------------------  //

    case DEV_DUMP_START:							//99
	sprintf(txn_type, "RL10");
	break;

    case DEV_LLS_TRIGGER:							//100
	sprintf(txn_type, "RL11");
	break;

    case DEV_HLS_TRIGGER:							//101
	sprintf(txn_type, "RL12");
	break;

    case DEV_FLOW_SENSOR_ERROR:							//102
	sprintf(txn_type, "RL13");
	break;

    case DEV_SOLENOID_VALVE_ERROR:						//103
	sprintf(txn_type, "RL14");
	break;

    case DEV_PAG_ERROR:								//104
	sprintf(txn_type, "RL15");
	break;

    case DEV_TANK_EMPTY:							//105
	sprintf(txn_type, "RL16");
	break;

    case DEV_SOLAR_DETECTED:							//106
	sprintf(txn_type, "RL18");
	break;

    case DEV_AC_MAINS_DETECTED:							//107
	sprintf(txn_type, "RL19");
	break;

    case DEV_SERVER_LOST:							//108
	sprintf(txn_type, "RL23");
	break;

    case DEV_NETWORK_LOST:							//109
	sprintf(txn_type, "RL24");
	break;
     
    default:
	sprintf(txn_type, "ERR");						//default
	break;
}
    //Last byte of both array made 'null' to support string
    txn_type[4] = 0;
    msg_data[14] = 0; //v2.2.9 ->	msg_data[13]=0; to 	msg_data[14]=0;
}

void construct_response_message(void) 
{
    unsigned char cntr_1 = 0;
    float voltage_value = 0.0, current_value = 0.0;

    random_no = rand();
    for (cntr_1 = 0; cntr_1 < (sizeof (random_no_array)); cntr_1++)
	random_no_array[cntr_1] = 0;

    //---utoa(random_no_array,random_no,10);
    sprintf(random_no_array, "%.4u", random_no);
    random_no_array[5] = 0; //make last byte 'null'(zero)
    memset(transmitData, '\0', sizeof (transmitData)); //3.1.0

    switch (server_send_message_type)
    {
	case TXN_MSG:
	if (rfid_to_server_msg == DEV_AUTHENTICATE)
	{
	    //sprintf(transmitData,"%.4s,d,%.10s,%.5s,%.12s,%s,$",txn_type,MACHINE_ID,FIRMWARE_VERSION,rfid_card,random_no_array);
	    sprintf(transmitData, "%.4s,d,%.10s,%.5s,%.8s,%.4s,D,D,D,D,D,%s,$", txn_type, MACHINE_ID, FIRMWARE_VERSION, rfid_card, temp_value, random_no_array);
	} 
	else if (rfid_to_server_msg == DEV_BALANCE_DEDUCT)
	{
	    if ((school_mode_en) && (mode_flags.op_mode == SCHOOL_MODE))
	    {
		//strcpy(rfid_card, sch_mode_acc_id);  //v3.1.2.G 
		memcpy(rfid_card, sch_mode_acc_id,8);  //v3.1.2.G
		sprintf(msg_data, "%2.1f", ((double) total_dispense_litre));
	    } else if ((corporate_mode_en) && (mode_flags.op_mode == CORPORATE_MODE)) //v2.2.5
	    {
		//strcpy(rfid_card, corp_mode_acc_id); //v3.1.2.G
		memcpy(rfid_card, corp_mode_acc_id,8); //v3.1.2.G
	    } else if ((peripheral_flags.coin_en_dis)&&(mode_flags.op_coin_mode == 1)) //v2.2.9
	    {
//		strcpy(rfid_card, accepted_coin_id); //v3.1.2.G
		memcpy(rfid_card, accepted_coin_id,8); //v3.1.2.G
	    } else{}	  
	    if (((school_mode_en) || (corporate_mode_en))&&(mode_flags.op_mode != IDLE_MODE)) //2.2.5
	    {
		for (cntr_1 = 0; cntr_1 < (sizeof (acc_credit_array)); cntr_1++)
		    acc_credit_array[cntr_1] = 0;
		acc_credit_array[0] = '0';
		acc_credit_array[1] = '.';
		acc_credit_array[2] = '0';
	    }

	    sprintf(transmitData, "%.4s,d,%.10s,%.5s,%.8s,%.4s,%.6s,%.4s,%.4s,%.4s,D,D,D,D,D,%s,$", txn_type, MACHINE_ID, FIRMWARE_VERSION, rfid_card, msg_data, acc_credit_array, tds_value, "100", temp_value, random_no_array);
	} 
	else if (rfid_to_server_msg == DEV_USER_ADD)
	{
	    sprintf(transmitData, "%.4s,d,%.10s,%.5s,%.8s,%.6s,%.1d,%.4d%.2d%.2d%.2d%.2d%.2d,%s,$", txn_type, MACHINE_ID, FIRMWARE_VERSION, rfid_card, acc_credit_array, Rfid_addition_type, sd_year, sd_month, sd_date, sd_hour, sd_min, sd_sec, random_no_array);
	}
	else if (rfid_to_server_msg == DEV_USER_REMOVE)
	{
	    sprintf(transmitData, "%.4s,d,%.10s,%.5s,%.8s,%.6s,%.1d,%.4d%.2d%.2d%.2d%.2d%.2d,%s,$", txn_type, MACHINE_ID, FIRMWARE_VERSION, rfid_card, acc_credit_array, Rfid_remove_type, sd_year, sd_month, sd_date, sd_hour, sd_min, sd_sec, random_no_array);
	} 
	else if (rfid_to_server_msg == DEV_BAL_ADD)
	{
	    sprintf(transmitData, "%.4s,d,%.10s,%.5s,%.8s,%.4s,%.6s,%.4d%.2d%.2d%.2d%.2d%.2d,%s,$", txn_type, MACHINE_ID, FIRMWARE_VERSION, rfid_card, previous_bal_array, acc_credit_array, sd_year, sd_month, sd_date, sd_hour, sd_min, sd_sec, random_no_array);
	}
	else
	    sprintf(transmitData, "%.4s,d,%.10s,%.5s,CODE_ERROR1:$", txn_type, MACHINE_ID, FIRMWARE_VERSION);
	break;

    case CONFIG_MSG:
	if (rfid_to_server_msg == DEV_HB_MSG)
	{
            if(system_flags.server_connection==TRUE)        //v2.2.7
            {	
                if(peripheral_flags.chiller_status_flag==TRUE)
                    system_flags.server_chiller_heater_status=3;
                else if(peripheral_flags.heater_status_flag==TRUE)		//v2.3.6 - Server connected - Heater ON
                    system_flags.server_chiller_heater_status=5;
                else
                    system_flags.server_chiller_heater_status=1;		//v2.3.6 - Server connected - Heater,Chiller OFF
            }
            else
            {
                if(peripheral_flags.chiller_status_flag==TRUE)
                    system_flags.server_chiller_heater_status=2;
                else if(peripheral_flags.heater_status_flag==TRUE)		//v2.3.6 - Server disconnected - Heater ON
                    system_flags.server_chiller_heater_status=4;
                else
                    system_flags.server_chiller_heater_status=0;		//v2.3.6 - Server disconnected - Heater,Chiller OFF
            }
                sprintf(transmitData, "%.4s,d,%.10s,%.5s,%s,%.4s,%.3d,%2.2f,%1.3f,%1.2f,%u,%.2s,%.5s,%.4s,%ld,%u,%s,%s,$", txn_type, MACHINE_ID, FIRMWARE_VERSION, msg_data, tds_value, 100, voltage_value, current_value, 0.0, system_flags.server_chiller_heater_status, network_signal_strength, temp_value/*total_dispense_litre*/, error_state, min_tick, mode_flags.op_mode, temp_data, random_no_array);   
              //sprintf(transmitData,"%.4s,d,%.10s,%.5s,%s,%.4s,%.3d,%2.2f,%1.3f,%1.2f,%u,%.2s,%2.1f,%.4s,%u,%u,%s,%s,$",txn_type,MACHINE_ID,FIRMWARE_VERSION,msg_data,tds_value,"100",voltage_value,current_value,sv_on_amp,system_flags.server_connection,network_signal_strength,total_dispense_litre,error_state,AcCurrent,mode_flags.op_mode,temp_data,random_no_array);
                if (!((school_mode_en) && (mode_flags.op_mode == SCHOOL_MODE)))
                {
                    total_dispense_litre = 0;
                } else{}
	
	} 
	else
	{
	    if (strlen(sms_sender_no) < 3)
            {
		//strcpy(sms_sender_no, "0000000000");

	    //sprintf(transmitData, "%.4s,d,%.10s,%.5s,%s,%.12s,%.4s,%.4s,%s,$", txn_type, MACHINE_ID, FIRMWARE_VERSION, msg_data, sms_sender_no, tds_value, "100", random_no_array); //v3.1.1.E %s to .12s sms_sendor_no
	      sprintf(transmitData, "%.4s,d,%.10s,%.5s,%s,%.15s,%.4s,%.4s,%s,$", txn_type, MACHINE_ID, FIRMWARE_VERSION, msg_data, "0000000000", tds_value, "100", random_no_array); //v3.2.0.A %.12s to .13s
            }
            else
            {
                   sprintf(transmitData, "%.4s,d,%.10s,%.5s,%s,%.15s,%.4s,%.4s,%s,$", txn_type, MACHINE_ID, FIRMWARE_VERSION, msg_data, sms_sender_no, tds_value, "100", random_no_array); //v3.2.0.A %.12s to .13s
            }
	}
	break;

    case SETTING_MSG:
	    switch (rfid_to_server_msg)
	    {
		case DEV_SYSTEM_SETTINGS_1:
		    sprintf(transmitData, "%.4s,d,%.10s,%.5s,%.4s,%.4s,%.4d,%.2f,%.4d,%.4d,%.4d,%.4d,%.4d,%s,$", txn_type, MACHINE_ID, FIRMWARE_VERSION, "100", error_state, lit_empty_tank, DISPENSE_LEAST_COUNT, idle_msg_freq, in_flow_calibration, out_flow_calibration, error_display_time, logout_idle_time, random_no_array);
		    break;

		case DEV_SYSTEM_SETTINGS_2:
		    sprintf(transmitData, "%.4s,d,%.10s,%.5s,%.4d,%.4d,%.4d,%.4d,%.4d,%.4d,%.4d,%.4d,%.4s,%s,$", txn_type, MACHINE_ID, FIRMWARE_VERSION, user_timeout, dispense_timeout, dump_timeout, refill_timeout, cleaning_timeout, tds_calib_fact, max_tds, min_tds, tds_value, random_no_array);
		    break;
            
		case DEV_SYSTEM_SETTINGS_3:
		    sprintf(transmitData, "%.4s,d,%.10s,%.5s,%s,%s,%s,%s,%s,$", txn_type, MACHINE_ID, FIRMWARE_VERSION, server_port, server_ip, apn, temp_data, random_no_array);
                break;
            
		case DEV_SYSTEM_SETTINGS_4:
		    sprintf(transmitData, "%.4s,d,%.10s,%.5s,%.2s,%.4d,%u,%u,%.3d,%.3d,%u,%.1d,%.12s,%s,$", txn_type, MACHINE_ID, FIRMWARE_VERSION, network_signal_strength, leak_timeout, peripheral_flags.pag_buzzer_control, 0, 20, 10, 0, gsm_flags.sms_permit, sch_mode_acc_id, random_no_array);
		    break;
            
		case DEV_SYSTEM_SETTINGS_5:
//		    sprintf(transmitData, "%.4s,d,%.10s,%.5s,%.12s,%.4u,%.4u,%u,%.2u,%.15s,%.4u,%.4u,%.3u,%.1u,%u,%u,%.1u,%s,$", txn_type, MACHINE_ID, FIRMWARE_VERSION, master_contact, 0, school_disp_amount, system_flags.server_connection, TOTAL_SMS_READ, network_service_provider, 0, temp_calib_fact, water_temperature_level, peripheral_flags.temp_en_dis, input_flow_freq_limit, output_flow_freq_limit, refill_flags.auto_refill_flag, random_no_array); //v3.2.0.A
		    sprintf(transmitData, "%.4s,d,%.10s,%.5s,%.15s,%.4u,%.4u,%u,%.2u,%.25s,%.4u,%.4u,%.3u,%.1u,%u,%u,%.1u,%s,$", txn_type, MACHINE_ID, FIRMWARE_VERSION, master_contact, 0, school_disp_amount, system_flags.server_connection, TOTAL_SMS_READ, network_service_provider, 0, temp_calib_fact, water_temperature_level, peripheral_flags.temp_en_dis, input_flow_freq_limit, output_flow_freq_limit, refill_flags.auto_refill_flag, random_no_array); //v3.2.0.A .12s to .15s change in master contact 
		    break;                                                                                                                                                                                                                                                                                                                                                                                                                                             //network_service_provider changed to .25s
		case DEV_SYSTEM_SETTINGS_6: //v2.2.9
             sprintf(transmitData,"%.4s,d,%.10s,%.5s,%.12s,%.12s,%u,%c,%u,%u,%.3d,%.5d,%.5d,%2.1f,%.5d,%s,$",  txn_type,MACHINE_ID, FIRMWARE_VERSION, franchisee_rfid_tag,corp_mode_acc_id,peripheral_flags.chiller_heater_en_dis, tds_header_version,peripheral_flags.temp_compen_en_dis,peripheral_flags.coin_en_dis,channel_cmd,total_coin_amount,total_coin_count,coin_water_tariff,coin_collector_capacity,random_no_array);  //v3.1.2.G                 
     //@     sprintf(transmitData,"%.4s,d,%.10s,%.5s,%.8s,%.12s,%u,%c,%u,%u,%s,$",txn_type,MACHINE_ID,FIRMWARE_VERSION,franchisee_rfid_tag,corp_mode_acc_id,peripheral_flags.chiller_en_dis,tds_header_version,peripheral_flags.temp_compen_en_dis,random_no_array);//offline
	    	break;
	    	default:
		 break;
	    }
	break;

    case MISC_MSG:
	if ((rfid_to_server_msg == DEV_MODEM_COMMAND) || (rfid_to_server_msg == DEV_GENERAL_COMMAND))
	//  sprintf(transmitData, "%.4s,d,%.10s,%.5s,%s,%.12s,%s,$", txn_type, MACHINE_ID, FIRMWARE_VERSION, modem_cmd_response, sms_sender_no, random_no_array); //v3.1.1.D .10s to .12a sms_sendor_no
	    sprintf(transmitData, "%.4s,d,%.10s,%.5s,%s,%.15s,%s,$", txn_type, MACHINE_ID, FIRMWARE_VERSION, modem_cmd_response, sms_sender_no, random_no_array); //v3.2.0.A .12s to .15s sms_sendor_no
	else if (rfid_to_server_msg == DEV_SMS_RX)
	    //sprintf(transmitData, "%.4s,d,%.10s,%.5s,%.4s,%.12s,%.4s,%.4s,%s,$", txn_type, MACHINE_ID, FIRMWARE_VERSION, msg_data, sms_sender_no, tds_value, "100", random_no_array); //v3.1.1.D  .10s to .12a sms_sendor_no
	      sprintf(transmitData, "%.4s,d,%.10s,%.5s,%.4s,%.15s,%.4s,%.4s,%s,$", txn_type, MACHINE_ID, FIRMWARE_VERSION, msg_data, sms_sender_no, tds_value, "100", random_no_array); //v3.2.0.A .12s to .15s sms_sendor_no
	else if (rfid_to_server_msg == DEV_VALIDATION_ERROR)
	    sprintf(transmitData, "%.4s,d,%.10s,%.5s,%s,%s,%s,$", txn_type, MACHINE_ID, FIRMWARE_VERSION, msg_data, data_error_buffer, random_no_array);
	else
	    sprintf(transmitData, "%.4s,d,%.10s,%.5s,CODE_ERROR3:$", txn_type, MACHINE_ID, FIRMWARE_VERSION);
	break;

    case DATA_ALERT_MSG:
	if (rfid_to_server_msg == DEV_ULTRA_SUPER_ADMIN_MODE)
	{
	 // sprintf(transmitData, "%.4s,d,%.10s,%.5s,%.12s,%.2s,%.4d%.2d%.2d%.2d%.2d%.2d,%s,$", txn_type, MACHINE_ID, FIRMWARE_VERSION, sms_sender_no, msg_data, sd_year, sd_month, sd_date, sd_hour, sd_min, sd_sec, random_no_array); //3.1.2
	    sprintf(transmitData, "%.4s,d,%.10s,%.5s,%.15s,%.2s,%.4d%.2d%.2d%.2d%.2d%.2d,%s,$", txn_type, MACHINE_ID, FIRMWARE_VERSION, sms_sender_no, msg_data, sd_year, sd_month, sd_date, sd_hour, sd_min, sd_sec, random_no_array); //v3.2.0.A
	} 
	else
	{
	    sprintf(transmitData, "%.4s,d,%.10s,%.5s,%.13s,%.4s,%.4s,%s,$", txn_type, MACHINE_ID, FIRMWARE_VERSION, msg_data, tds_value, "100", random_no_array);
	}
	if ((rfid_to_server_msg == DEV_TDS_HIGH) || (rfid_to_server_msg == DEV_TEMP_SENSOR_FAILURE))
	    strcpy(error_state, txn_type);
	else{}	
	break;

    case ALERT_MSG:
	sprintf(transmitData, "%.4s,d,%.10s,%.5s,%s,$", txn_type, MACHINE_ID, FIRMWARE_VERSION, random_no_array);

	if ((rfid_to_server_msg == DEV_LLS_TRIGGER) || (rfid_to_server_msg == DEV_FLOW_SENSOR_ERROR) || (rfid_to_server_msg == DEV_SOLENOID_VALVE_ERROR) || (rfid_to_server_msg == DEV_PAG_ERROR))
	    strcpy(error_state, txn_type);
	else{}
	break;

    default:
	sprintf(transmitData, "%.4s,d,%.10s,%.5s,CODE_ERROR2:$", txn_type, MACHINE_ID, FIRMWARE_VERSION); ///TIME SYNC DATA
	break;
    }
}

void calculate_check_sum(void)
{
    unsigned int loc_counter_1 = 0, loc_counter_2 = 0;
    unsigned char string_length = 0;
    char cs_buffer[6];
    unsigned int sum = 0;

    string_length = strlen(transmitData);

    if (string_length <= 0)
	return;
    else{}

    //Calculate check sum
    for (loc_counter_1 = 0; loc_counter_1<sizeof (transmitData); loc_counter_1++)
    {
	if (transmitData[loc_counter_1] == '$')
	{
	    transmitData[loc_counter_1] = 0;
	    break;
	} else{}
	
	sum += ((unsigned int) (transmitData[loc_counter_1]));
    }

    //---utoa(cs_buffer,sum,10);
    sprintf(cs_buffer, "%.4u", sum);
    cs_buffer[5] = 0;

    //Append check sum
    //---for(loc_counter_2=0; loc_counter_2<5; loc_counter_2++)
    for (loc_counter_2 = 0; loc_counter_2 < (sizeof (cs_buffer)); loc_counter_2++)
    {
	if (cs_buffer[loc_counter_2] == 0)
	    break;
	else{}

	transmitData[loc_counter_1] = cs_buffer[loc_counter_2];
	loc_counter_1++;
    }

    if (loc_counter_1 < (MAX_DATA_LENGTH - 3))
    {
	transmitData[loc_counter_1] = '#'; //EOM
	transmitData[++loc_counter_1] = '$'; //EOM
	transmitData[++loc_counter_1] = '%'; //EOM
    } else{}
  
}

//--------------------SD Card------------------------------//

void calculate_check_sum_SD_TEST(void) {
    unsigned int loc_counter_1 = 0, loc_counter_2 = 0;
    unsigned int string_length = 0;
    unsigned char cs_buffer[7];
    unsigned long int sum = 0;

    string_length = strlen(transmitData);

    if (string_length <= 0)
	return;
    else{} 

    //Calculate check sum
    for (loc_counter_1 = 0; loc_counter_1 < MAX_DATA_LENGTH; loc_counter_1++)
    {
	if (transmitData[loc_counter_1] == '*')
	{
	    transmitData[loc_counter_1] = 0;
	    break;
	} else{}
	sum += ((unsigned int) (transmitData[loc_counter_1]));
    }

    //---utoa(cs_buffer,sum,10);
    sprintf(cs_buffer, "%ld", sum);
    //@	sprintf(lcd_data,"CS =%ld",sum); lcd_display(1); timer2_ms_delay(2000);  //3.1.0
    cs_buffer[7] = 0; //3.1.0

    //Append check sum
    //---for(loc_counter_2=0; loc_counter_2<5; loc_counter_2++)
    for (loc_counter_2 = 0; loc_counter_2 < (sizeof (cs_buffer)); loc_counter_2++)
    {
	if (cs_buffer[loc_counter_2] == 0)
	    break;
	else{}
	transmitData[loc_counter_1] = cs_buffer[loc_counter_2];
	loc_counter_1++;
    }
    //	transmitData[loc_counter_1] = '%';	//EOM
    if (loc_counter_1 < (MAX_DATA_LENGTH - 3))
    {
	transmitData[loc_counter_1] = '*'; //EOM
	transmitData[++loc_counter_1] = '\0'; //EOM
	//	transmitData[++loc_counter_1] = '%';	//EOM
    } else{}
}

//*********************************

void calculate_alert_check_sum(void) 
{
    unsigned int loc_counter_1 = 0, loc_counter_2 = 0;
    unsigned char string_length = 0;
    char cs_buffer[6];
    unsigned int sum = 0;

    string_length = strlen(transmitData);

    if (string_length <= 0)
	return;
    else{}

    //Calculate check sum
    for (loc_counter_1 = 0; loc_counter_1 < MAX_DATA_LENGTH; loc_counter_1++)
    {
	if (transmitData[loc_counter_1] == '$')
	{
	    transmitData[loc_counter_1] = 0;
	    break;
	} else{}

	sum += ((unsigned int) (transmitData[loc_counter_1]));
    }

    //---utoa(cs_buffer,sum,10);
    sprintf(cs_buffer, "%.4u", sum);
    cs_buffer[5] = 0;

    //Append check sum
    //---for(loc_counter_2=0; loc_counter_2<5; loc_counter_2++)
    for (loc_counter_2 = 0; loc_counter_2 < (sizeof (cs_buffer)); loc_counter_2++)
    {
	if (cs_buffer[loc_counter_2] == 0)
	    break;
	else{}
	transmitData[loc_counter_1] = cs_buffer[loc_counter_2];
	loc_counter_1++;
    }

    if (loc_counter_1 < (MAX_DATA_LENGTH - 3))
    {
	transmitData[loc_counter_1] = '#'; //EOM
	transmitData[++loc_counter_1] = '$'; //EOM
	transmitData[++loc_counter_1] = '%'; //EOM
	transmitData[++loc_counter_1] = ','; //EOM
	transmitData[++loc_counter_1] = '1'; //EOM
    } else{}
}

void Serve_daily_dispense() {
}

void send_server_sd_response(void) {

}

void Server_Sync_2(unsigned char file_search, unsigned char data_send_to_server)  //char *file_names
{
    if ((sd_flags.card_initialize) && (sd_flags.card_detect))//v3.1.1.D  //v3.1.1.H
    {
        char *str_ptr_2 = 0;
        unsigned char loc_temp_var = 0, temp = 0, ack_response_error = 0, file_open_var = 0, Sync_process_comp = 0; // if this flag true then function complete, 1) log send 2) current date come 3) error comes;
        unsigned char ser_response[10] = "", file_name_str_find = 0, file_send_to_server = 0, last_file_sync = 0;
        unsigned int shift_sync_file_pointer = 0;
        unsigned long int sd_read_byte = 0, sd_read_byte_temp = 0;
        unsigned char folder_name[9] = {0}, file_status_1[5] = {""}, file_status_2[5] = {0}, sync_file_name[15] = {0}, compare_date[19] = {0};

        if (data_send_to_server) {
            system_flags.server_connection = 0;
            while (system_flags.server_connection != SERVER_CONN_OK) {
                system_flags.server_connection = 0;
                connect_to_server(); //v3.0.0	
                check_server_connection(); //v3.0.0
                loc_temp_var++;
                if (loc_temp_var > 5) {
                    server_sync_status = 0;
                    send_server_response(DEV_SERVER_SYNC + 200);
                    break;
                }
            }
        } else {
        }
        if (loc_temp_var > 5) {
            Sync_process_comp = 1;
            strcpy(lcd_data, "SIM MODULE ERROR");
            lcd_display(2);
            timer2_sec_delay(error_display_time);
        } else {
            FSchdir("\\");
            FSchdir("Trans");
            get_local_time();
            clear_lcd();
            sprintf(compare_date, "#%.2d_%.2d_%.2d.txt;PEN", sd_yr, sd_month, sd_date);
            strncpy(lcd_data, compare_date, 15);
            lcd_display(2);
            timer2_sec_delay(error_display_time);
            //	sd_pointer = FSfopen("SYNC.txt",FS_READPLUS);	
            Sync_process_comp = 0;
        }
        if (file_search == 1) {
            strcpy(file_status_1, "PEN");
        }
        else if (file_search == 2) {
            strcpy(file_status_1, "ERR");
        } else {
            strcpy(file_status_1, "PEN");
        }

        sync_file_lastbyte_Pointer = 0;
        loc_temp_var = 0; //reset conter after sever connectivity
        while (!Sync_process_comp) {
            if (single_date_sync == 1) {
                file_name_str_find = 1;

                temp = 0;
                memcpy(file_name, sd_sync_buffer, 12);
                memset(sync_file_name, '\0', sizeof (sync_file_name));
                for (loc_temp_var = 0; loc_temp_var < 9; loc_temp_var++) {
                    if (sd_sync_buffer[loc_temp_var] == '.')
                        break;
                    if (sd_sync_buffer[loc_temp_var] != '_')
                        sync_file_name[temp++] = sd_sync_buffer[loc_temp_var];

                }
                Protocol_No = 1;
            } else {
                file_name_str_find = 0;
                FSchdir("\\");
                FSchdir("Trans");
                FSfclose(sd_pointer);
                sd_pointer = FSfopen("SYNC.txt", FS_READPLUS);
            }
            while (!file_name_str_find) {
                if (FSfeof(sd_pointer)) {
                    strcpy(lcd_data, "PROCESS END     ");
                    lcd_display(1);
                    strcpy(lcd_data, "PLS WAIT...     ");
                    lcd_display(2);
                    Sync_process_comp = 1;
                    file_name_str_find = 0;
                    file_send_to_server = 0;
                    break;
                }
                else {
                    if (!FSfseek(sd_pointer, sync_file_lastbyte_Pointer, 0)) // Move file pointer to the specified value
                    {
                        //SD_lastbyte_Pointer = FSftell(sd_pointer);  //1.0.1
                    } else {
                    }

                    memset(sd_sync_buffer, '\0', sizeof (sd_sync_buffer));
                    //sd_read_byte_temp = FSfread(sd_sync_buffer, 1, (sizeof (sd_sync_buffer)), sd_pointer); //v3.1.1.F
                    sd_read_byte_temp = FSfread(sd_sync_buffer, 1, 19, sd_pointer); //v3.1.1.F
                    if (strncmp(sd_sync_buffer, compare_date, 9) == 0) //change
                    {
                        file_name_str_find = 0;
                        Sync_process_comp = 1;
                        file_send_to_server = 0;
                        clear_lcd();
                        strcpy(lcd_data, "SYNC COMPLETED");
                        lcd_display(1);
                        //strcpy(lcd_data,"DATE COMPLETED");lcd_display(2);					
                        timer2_sec_delay(1);
                        break;
                    } else {
                    }

                    pch = strstr(sd_sync_buffer, file_status_1);

                    if (pch) {
                        memset(file_name, '\0', sizeof (file_name)); //if PEN file find then copy into file_name and sync_file_name with diffrent format
                        memset(sync_file_name, '\0', sizeof (sync_file_name));
                        loc_temp_var = 0;
                        sync_file_lastbyte_Pointer = FSftell(sd_pointer); //store the address

                        str_ptr_2 = strchr(sd_sync_buffer, '#');

                        if (str_ptr_2 != 0) {
                            ++str_ptr_2;
                            for (loc_temp_var = 0; loc_temp_var < 15; loc_temp_var++) //from (#17_02_21.txt;PEN) to (#17_02_21.txt) if ; get then break loop
                            {
                                file_name[loc_temp_var] = *str_ptr_2;
                                if (*++str_ptr_2 == ';')
                                    break;
                                else {
                                }
                            }
                            temp = 0;
                            for (loc_temp_var = 0; loc_temp_var < 9; loc_temp_var++) //from (#17_02_21.txt) to (170221 = YYMMDD) if . get then break loop
                            {
                                if (file_name[loc_temp_var] == '.')
                                    break;
                                if (file_name[loc_temp_var] != '_')
                                    sync_file_name[temp++] = file_name[loc_temp_var];
                                else {
                                }
                            }
                        } else {
                        }
                        err_pen_file_counter++;
                        file_name_str_find = 1;
                        Protocol_No = 1;
                        break;
                    }
                    else {
                        sync_file_lastbyte_Pointer = FSftell(sd_pointer);
                        file_name_str_find = 0;
                    }
                }
            }
            FSfclose(sd_pointer); //close sync file 
            if ((file_name_str_find)&&(data_send_to_server)) {
                err_pen_file_counter = 0; //if data_send_to_server=1 means we not count ERR and PEN file in sync.txt
                file_name_str_find = 0;
                file_send_to_server = 1;
                FSfclose(sd_pointer);
                memset(folder_name, '\0', sizeof (folder_name));
                sprintf(folder_name, "20%.5s", file_name); //sd_sync_buffer to file_name 
                FSchdir(folder_name);
                sd_data_count = 0;
                ack_response_error = 0;
                loc_temp_var = 0;
                file_open_var = 0;
                sprintf(lcd_data, "%s", file_name);
                lcd_display(1);
                sd_pointer = FSfopen(file_name, FS_READPLUS); //fr = f_open(&fil,file_name, FA_READ);										
                if (sd_pointer != NULL) {
                    file_open_var = 1;
                    strcpy(lcd_data, "SYNCING START..");
                    lcd_display(1);
                    strcpy(lcd_data, "FILE OPEN");
                    lcd_display(2);
                    timer2_sec_delay(1);
                }
                else {
                    file_open_var = search_file(file_name, FILE_FIND_RETURN_FLAG);
                }
                //strcpy(lcd_data,"SYNCING START..");	lcd_display(1);
                //strcpy(lcd_data,"FILE OPEN");lcd_display(2);								

                while (file_open_var) {
                    if (FSfeof(sd_pointer)) {
                        FSfclose(sd_pointer); //timer2_ms_delay(sd_delay);
                        strcpy(lcd_data, "FILE END.      1");
                        lcd_display(1);
                        strcpy(lcd_data, "DATA SYNCED.    ");
                        lcd_display(2);
                        timer2_sec_delay(1);
                        file_send_to_server = 1;
                        break;
                    }
                    sd_data_count++;
                    memset(SD_Buffer, 0, sizeof (SD_Buffer));
                    sd_read_byte = 0;
                    sd_read_byte = FSfread(SD_Buffer, 1, size_sd_read, sd_pointer);
                    if (sd_read_byte < 10) {
                        FSfclose(sd_pointer);
                        strcpy(lcd_data, "FILE END..     2");
                        lcd_display(1);
                        strcpy(lcd_data, "DATA SYNCED..   ");
                        lcd_display(2);
                        timer2_sec_delay(error_display_time);
                        file_send_to_server = 1;
                        break;
                    } else {
                    }

                    memset(str_test, '\0', sizeof (str_test));
                    memcpy(str_test, SD_Buffer, sd_read_byte); // copy data to str_test
                    pch = strrchr(str_test, '%'); // Find last location pointer of character in string 								
                    char_location = pch - str_test + 1; // Location of character in arrary
                    SD_Buffer[char_location + 2] = '\0';
                    SD_lastbyte_Pointer = FSftell(sd_pointer); // Current read/write pointer in file

                    FSfseek(sd_pointer, (SD_lastbyte_Pointer - (sd_read_byte - char_location - 2)), SEEK_SET); //f_lseek(&fil,SD_Pointer-(br-char_location-2));	// Move file pointer to the specified value

                    serial_string_send("AT+CIPSEND\r\n");
                    timer2_ms_delay(200);
                    sprintf(transmitData, "SD20%s%.1d,%.10s,%s&*", sync_file_name, Protocol_No, MACHINE_ID, SD_Buffer);
                    calculate_check_sum_SD_TEST();

                    serial_string_send(transmitData);
                    clear_lcd();
                    sprintf(lcd_data, "DATA SEND %d", char_location);
                    lcd_display(1);
                    sprintf(lcd_data, "SD20%s %.1d", sync_file_name, Protocol_No);
                    lcd_display(2);

                    while (BusyUART2()); //wait till the UART is busy
                    WriteUART2((unsigned int) 26);
                    memset(ser_response, '\0', sizeof (ser_response));
                    sprintf(ser_response, "SD20%s%.1d", sync_file_name, Protocol_No); 
                    if (check_modem_reply(ser_response, 3, 5000)) //	//check for string	//v2.2.8
                    {
                        Protocol_No++;
                        ack_response_error = 0; //if data send proper then response error =0
                    }
                    else {
                        ack_response_error++;
                        sprintf(lcd_data, "SD20%s%.1d", sync_file_name, Protocol_No);
                        lcd_display(1);
                        sprintf(lcd_data, "RESPONSE ERROR %d", ack_response_error);
                        lcd_display(2);

                        check_server_connection();
                        if (system_flags.server_connection != SERVER_CONN_OK) {
                            loc_temp_var = 0;
                            while (system_flags.server_connection != SERVER_CONN_OK) {
                                system_flags.server_connection = 0;
                                connect_to_server(); //v3.0.0	
                                check_server_connection(); //v3.0.0
                                loc_temp_var++;
                                if (loc_temp_var > 5) {
                                    clear_lcd();
                                    strcpy(lcd_data, "NETWORK PROB_1"); //v3.1.1.F
                                    lcd_display(1);
                                    timer2_sec_delay(error_display_time);
                                    server_sync_status = 0;
                                    send_server_response(DEV_SERVER_SYNC + 200);
                                    file_send_to_server = 0;
                                    file_name_str_find = 0;
                                    Sync_process_comp = 1; //1.0.1	
                                    break;
                                }
                            }
                        } else {
                        }

                        if (ack_response_error > 4) {
                            clear_lcd();
                            strcpy(lcd_data, "NETWORK PROB_2"); //v3.1.1.F
                            lcd_display(1);
                            timer2_sec_delay(error_display_time);
                            server_sync_status = 2;
                            send_server_response(DEV_SERVER_SYNC + 200);
                            file_send_to_server = 0;
                            file_name_str_find = 0;
                            Sync_process_comp = 1; //1.0.1	
                            break;
                        } else {
                        }
                        SD_lastbyte_Pointer = FSftell(sd_pointer);
                        //if(fr == FR_OK)
                        //	data= 5;
                        if (SD_lastbyte_Pointer > (size_sd_read)) //size_sd_read
                            FSfseek(sd_pointer, ((SD_lastbyte_Pointer - sd_read_byte)+(sd_read_byte - char_location - 2)), SEEK_SET); //	f_lseek(&fil,(SD_Pointer-br)+(br-char_location-2));
                        else
                            FSfseek(sd_pointer, ((sd_read_byte - SD_lastbyte_Pointer)-(sd_read_byte - char_location - 2)), SEEK_SET); //f_lseek(&fil,(br-SD_Pointer)-(br-char_location-2));
                    }
                } //end of while EOF	
            } else {
            }

            //@	sprintf(lcd_data,"%d %d %d %d %d" ,file_open_var,ack_response_error,loc_temp_var,file_send_to_server,single_date_sync);	lcd_display(1);	timer2_sec_delay(error_display_time);	
            if ((file_send_to_server == 1)&&(single_date_sync == 0)) {
                file_send_to_server = 0;
                clear_lcd();

                if ((file_open_var == 0) && (strncmp(file_status_1, "ERR", 3) == 0)) //||(ack_response_error>4)||(loc_temp_var > 5))
                {
                    strcpy(lcd_data, "WRITE DATA 1"); //FILE WRITE CHK v3.1.1.F
                    lcd_display(1);
                    strcpy(file_status_2, "CHK");
                }
                else if ((file_open_var == 0) && (strncmp(file_status_1, "PEN", 3) == 0)) {
                    strcpy(lcd_data, "WRITE DATA 2"); //FILE WRITE ERR  v3.1.1.F
                    lcd_display(1);
                    strcpy(file_status_2, "ERR");
                } else if ((file_open_var == 1) && (strncmp(file_status_1, "PEN", 3) == 0)) {
                    strcpy(lcd_data, "WRITE DATA 3"); //FILE WRITE SYNC v3.1.1.F
                    lcd_display(1);
                    strcpy(file_status_2, "SYN");
                } else if ((file_open_var == 1) && (strncmp(file_status_1, "ERR", 3) == 0)) //neeedtocheck10
                {
                    strcpy(lcd_data, "WRITE DATA 4"); //FILE WRITE SYNC v3.1.1.F
                    lcd_display(1);
                    strcpy(file_status_2, "SYN");
                } else {
                }

                FSchdir("\\");
                FSfclose(sd_pointer);
                FSchdir("Trans"); //timer2_ms_delay(sd_delay);
                sd_pointer = FSfopen("SYNC.txt", FS_READPLUS);
                last_file_sync = 0;
                while (!last_file_sync) {
                    //	sd_read_byte_temp= FSfread (sd_sync_buffer,1,(sizeof(sd_sync_buffer)),sd_pointer);
                    pch = strstr(sd_sync_buffer, file_status_1);
                    if (pch) {
                        char_location = pch - sd_sync_buffer; // Location of character in arrary						
                        shift_sync_file_pointer = (sync_file_lastbyte_Pointer - (sd_read_byte_temp - char_location));
                        if (!FSfseek(sd_pointer, shift_sync_file_pointer, 0)) // Move file pointer to the specified value
                        {
                            SD_lastbyte_Pointer = FSftell(sd_pointer);
                        } else {
                        }

                        FSfprintf(sd_pointer, "%s", file_status_2);
                        strcpy(lcd_data, "SUCCESSFULLY    ");
                        lcd_display(2);
                        timer2_sec_delay(1);
                        last_file_sync = 1;
                        FSfclose(sd_pointer);
                        if (strncmp(file_status_2, "SYN", 3) == 0) {
                            server_sync_status = 1;
                            send_server_response(DEV_SERVER_SYNC); //server send sync status when single date file will be send   
                        }
                        else if (strncmp(file_status_2, "ERR", 3) == 0) //change
                        {
                            send_server_response(DEV_SEND_ER_FILE_NAME);
                        }
                        else if (strncmp(file_status_2, "CHK", 3) == 0) //change
                        {
                            server_sync_status = 3;
                            send_server_response(DEV_SERVER_SYNC);
                        }
                    }
                    else {
                        strcpy(lcd_data, "NOT DONE");
                        lcd_display(2);
                        last_file_sync = 0;

                        timer2_sec_delay(error_display_time);
                        //SD_lastbyte_Pointer = FSftell(sd_pointer);	                                   
                    }
                    //                                    if(FSfeof(sd_pointer)){
                    //                                        Sync_process_comp=1;
                    //                                        FSfclose(sd_pointer);
                    //                                        //send_server_response(DEV_SERVER_SYNC);  //1.0.1						
                    //                                        break;
                    //                                    }
                    //                                else{
                    //                                        FSfclose(sd_pointer);
                    //                                      //  send_server_response(DEV_SERVER_SYNC);//server send sync status when single date file will be send   
                    //                                    }										
                }
            }
            else if ((file_send_to_server == 1)&&(single_date_sync == 1)) {
                FSfclose(sd_pointer);
                if(file_open_var==0)
                {
                    server_sync_status = 3;
                }
                else{
                server_sync_status = 1;
                }
                strcpy(lcd_data, "WRITE DATA 5"); //SINGLE FILE SYNC
                lcd_display(1);
                strcpy(lcd_data, "SUCCESSFULLY");
                lcd_display(2);
                send_server_response(DEV_SERVER_SYNC);
                break;

            } else {
            }
            FSfclose(sd_pointer);
            file_send_to_server = 0;
        } //end of Sync_process_comp
        FSfclose(sd_pointer);
        FSchdir("\\");
        single_date_sync = 0;
    }
    else {
        buzzer(ERR02);
        clear_lcd();
        strcpy(lcd_data, "MEMORY FAULT    ");
        lcd_display(1); //v3.1.1.D
        strcpy(lcd_data, "CONTACT SARVAJAL");
        lcd_display(2); //v3.1.1.D
        timer2_sec_delay(error_display_time); //v3.1.1.D
    }
}


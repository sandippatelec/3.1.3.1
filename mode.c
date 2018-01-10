#define USE_AND_OR /* To enable AND_OR mask setting */

#include<ports.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<adc.h>
#include<timer.h>
#include<math.h>

#include "PwrMgnt.h"	//v2.2.3
#include "wdt.h"
#include "p24fxxxx.h"

#include "math.h"
#include "mode.h"
#include "variable.h"
#include "peripherial.h"
#include "serial_uart.h"
#include "mcu_adc.h"
#include "mcu_timer.h"
#include "communicate.h"
#include "ext_lcd.h"
#include "server.h"
#include "modem.h"
//#include "DS2781.h"
#include "ext_eeprom.h"
#include "coin_uca2.h"  //1.0.2
#include "FSconfig.h"
#include "FSIO.h"
#include "DS1307.h"
#include "YHY502.h"
#include "ext_lcd.h"
#include "1wire_temperature.h"
#include "main.h"

//#define TEST_LCD_ON
#define DEBUG_TEST_ON

void card_detection(void)
{
    //	unsigned char loop=0;  //v3.1.1.C
    memset(offline_rfid_card, '\0', sizeof (offline_rfid_card));
    //	memset(user_bal_file,'\0',sizeof(user_bal_file));	  //v3.1.1.B
    sprintf(offline_rfid_card, "%.8s", rfid_card);

    //@	consumer_flags.dispense_type = 0;
    //	consumer_flags.rfid_read = LOW;  //1.0.1
    rfid_reader_counter = 0;
    SV_VALVE_OFF;
    rfid_card_type = 0;

    //	timer2_ms_delay(10);	//v2.2.7

    read_eeprom_parameter(SER_FRANCHISEE_RFID); //v2.2.5  Compares eeprom string with the scaned rfid card 

    if (strcmp(franchisee_rfid_tag, rfid_card) == 0) //v2.2.5  
    {
    #ifdef TEST_LCD_ON

        strcpy(lcd_data,"FRACHINSES CARD "); lcd_display(1); timer2_sec_delay(3);
    #endif
	buzzer(AUT01);
	mode_flags.op_mode = FRANCHISEE_MODE; //Franchisee mode is only for Drop Down menu screen
	rfid_card_type = FRANCHISEE_CARD;
	mode_flags.forced_function_state = TRUE;
	function_state = ST_OP_MODE;
	consumer_flags.dispense_button = 0;
        if(peripheral_flags.coin_en_dis){coinbox_disable();} else {}	//v3.1.2 //3.1.2.I
    }
    else if (strncmp("0000", rfid_card, 4) == 0) //v1.0.1
    {
	rfid_card_type = UNAUTHORISED_CARD;
	consumer_flags.card_detect = LOW;
    }
    else
    {
	Rfid_user_bal_paisa = 0;
	curnt_user_bal_paisa = 0;

	if ((rfid_read_purse(0x00, 0x08))&&(mode_flags.op_mode != SCHOOL_MODE) && (mode_flags.op_mode != CORPORATE_MODE) &&(mode_flags.op_mode != CALIBRATION_MODE)) //v2.2.5  //v3.1.1.D
	{
	    buzzer(AUT01); 
	    if (curnt_user_bal_paisa >= ((unsigned int) (offline_water_tariff * 2.0))) //49 //3.1.0
	    {
                acc_dispense_capacity = (((float) curnt_user_bal_paisa) / offline_water_tariff); //v3.1.1.B
                if (acc_dispense_capacity >= 20.0) //v3.1.0
                {
                    acc_dispense_capacity = 20.0;
                }
                else{}		
                Rfid_user_bal_paisa = (unsigned long int) (acc_dispense_capacity * offline_water_tariff);
                #if DISPLAY_SET	
                sprintf(lcd_data,"T2 %.6ld",Rfid_user_bal_paisa); lcd_display(1);  timer2_sec_delay(3); 
                #endif
                if (rfid_Epurse_decrement(0x00, 0x08, Rfid_user_bal_paisa))
                {
                    rfid_card_type = CONSUMER_CARD;
                    curnt_user_bal_rs = (((double) curnt_user_bal_paisa) / 100.0); //convert into rs to display
                    memset(acc_credit_array, '\0', sizeof (acc_credit_array));
                    sprintf(acc_credit_array, "%3.2f", (double) curnt_user_bal_rs); //(double)curnt_user_bal_rs);		  
                    litre_transcation = 1; // in liter
                    curnt_user_bal_paisa = curnt_user_bal_paisa - Rfid_user_bal_paisa;		
                }
                else
                {
                    rfid_card_type = NOT_PROPER_SCAN;
                    curnt_user_bal_paisa = 0;
                }
	    }//end of curnt_user_bal_paisa >=
	    else
	    {
		rfid_card_type = LOW_BAL_CARD;
	    }
	}
	else
	{
	    rfid_card_type = UNAUTHORISED_CARD;
	}

	system_flags.server_response = SERVER_RESPONSE_OK;

	switch (rfid_card_type)
	{
	case CONSUMER_CARD:                                                     //1
	    if ((mode_flags.op_mode != SCHOOL_MODE) && (mode_flags.op_mode != CORPORATE_MODE) &&(mode_flags.op_mode != CALIBRATION_MODE)) //v2.2.5 //1.0.1 //v3.1.1.D
	    {
		mode_flags.op_mode = CONSUMER_MODE;
		mode_flags.forced_function_state = TRUE;
		function_state = ST_OP_MODE;
                if(peripheral_flags.coin_en_dis){coinbox_disable();} else {}	//3.1.2.I
                rfid_read_fail_cntr = 0;  //3.1.2.J
	    }	
	else{  //v3.1.1.G
//		lcd_init();    
//		strcpy(lcd_data,"PRESS ANY BUTTON");
//		lcd_display(1);
//		strcpy(lcd_data,"FOR CLEAN WATER");
//		lcd_display(2);
//		for(loop=0; loop<(sizeof(rfid_card)); loop++)		//added v2.2.2
//		rfid_card[loop] = 0;	
//		timer2_sec_delay(error_display_time);
	    }	
	    break;
	case UNAUTHORISED_CARD:                                                 //3
	    if ((mode_flags.op_mode != SCHOOL_MODE) && (mode_flags.op_mode != CORPORATE_MODE) &&(mode_flags.op_mode != CALIBRATION_MODE)) //v3.1.1.D
	    {
		strcpy(lcd_data, "INVALID USER");
		lcd_display(1);
		sprintf(lcd_data, "CONTACT SARVAJAL");
		lcd_display(2);
		timer2_sec_delay(1);
		buzzer(ERR01); //v3.1.1.D
		consumer_flags.rfid_read = LOW;
                rfid_read_fail_cntr++;
	    }
	    else
	    {
		lcd_init();
		strcpy(lcd_data, "PRESS ANY BUTTON");
		lcd_display(1);
		strcpy(lcd_data, "FOR CLEAN WATER");
		lcd_display(2);
		//for(loop=0; loop<(sizeof(rfid_card)); loop++)		//added v2.2.2  //1.0.1
		//rfid_card[loop] = 0;	
		consumer_flags.rfid_read = LOW;
	    }
	    consumer_flags.card_detect = LOW; 

	    break;
	case LOW_BAL_CARD:                                                      //4
	    strcpy(lcd_data, "BALANCE LOW  ");
	    lcd_display(1);
	    strcpy(lcd_data, "PLS. RECHARGE");
	    lcd_display(2);
	    timer2_sec_delay(1);
	    consumer_flags.card_detect = LOW; 
	    break;

	case NOT_PROPER_SCAN:                                                   //5
	    strcpy(lcd_data, "NOT PROPER SCAN");
	    lcd_display(1);
	    strcpy(lcd_data, "PLS. TRY AGAIN");
	    lcd_display(2);
	    timer2_sec_delay(1);
	    consumer_flags.card_detect = LOW; 
            rfid_read_fail_cntr++;
	    break;
	default:
	    //	lcd_init();
	    if (error_flags.water_expired == TRUE)
	    {
		strcpy(lcd_data, "WATER EXPIRED");
		lcd_display(1);
		sprintf(lcd_data, "CONTACT OWNER");
		lcd_display(2);
	    }
	    else if (error_flags.tank_empty == TRUE)
	    {
		strcpy(lcd_data, "TANK EMPTY");
		lcd_display(1);
		sprintf(lcd_data, "CONTACT OWNER");
		lcd_display(2);
	    }		
	    else
	    {
		strcpy(lcd_data, "PLS. TRY AGAIN");
		lcd_display(1);
		//sprintf(lcd_data, "TYPE:%d", rfid_card_type);
	    }
	    
	    memset(rfid_card, 0, sizeof (rfid_card)); //v3.1.1.B  //remove for loop
	    memset(scanned_rfid_card, 0, sizeof (scanned_rfid_card)); //v3.1.1.B	//remove for loop
	    timer2_sec_delay(error_display_time);
	    consumer_flags.card_detect = LOW; 
	    peripheral_flags.lcd_refresh = TRUE; //v2.2.7
	    break;
	}
    }
}

/*---------------------------------------------------------------------------------------------------
Function Name:	void Franchisee_menu_screen(void)
Arguments:		None
Return Val:		None
Description:	This function gives Drop Down menu option to the Franchisee card holder. All the functions are described in
				different cases.Long Press & Short Press use of the Button is done for traversing in the menu 
				and Selection of the options.Autologout from main menu is 30 sec. toggling of the screen occrs at
				7sec and 4sec.For device info. short press for traversing across menu and long press to exit
                 
---------------------------------------------------------------------------------------------------*/

void diagnostic_mode(void)
{
    //	unsigned char var_1=0;   //v3.1.1.C
    mode_flags.run_diagnostic = FALSE;
    if (mode_flags.diagnostic_test_toggle)
    {
	mode_flags.diagnostic_test_toggle = FALSE;
	SV_VALVE_OFF;
    }
    else
    {
	mode_flags.diagnostic_test_toggle = TRUE;
	SV_VALVE_ON;
    }
    lcd_init();
    //---sprintf(lcd_data,"SV%d,C=%.3d,N%.2s",mode_flags.diagnostic_test_toggle,sv_current,network_signal_strength);

    sprintf(lcd_data, "SV%d,C%1.2f,N%.2s", mode_flags.diagnostic_test_toggle, 0.0, network_signal_strength);
    lcd_display(1);

    strcpy(lcd_data, "                ");
    sprintf(lcd_data, "A%d,S%d,Ch%d,B%d", 1, peripheral_flags.solar_sense, 0, 100);
    //   sprintf(lcd_data,"TEMP.:%.5s",temp_value);
    lcd_display(2);
    timer2_sec_delay(2);

    send_server_response(DEV_HB_MSG);

}

void antenna_test_mode(void)
{
  	CHILLER_HEATER_OFF;                                 //v3.1.3
	peripheral_flags.chiller_status_flag=FALSE;         //v3.1.3
    temp_counter = 0; 
    SV_VALVE_OFF;
    lcd_init();
    strcpy(lcd_data, "ANTENNA TEST");
    lcd_display(1);
    strcpy(lcd_data, "INITIATING...");
    lcd_display(2);
    buzzer(HALT);
    timer2_sec_delay(error_display_time);
    clear_lcd();
    system_flags.system_stability = FALSE;

    network_state = INITIAL;
    while ((network_state != REGISTERED) && (network_state != REG_ROAMING))
    {
	network_check();
    }

    while (mode_flags.op_mode == ANTENNA_MODE)
    {
	if (consumer_flags.dispense_button == 1) //if(consumer_flags.dispense_button==1)  //3.1.0
	{
	    LED4_ON;
	    buzzer(AUT01);
	    consumer_flags.dispense_button = 0; //consumer_flags.dispense_button = 0;	
	    network_check();
	    LED4_OFF;
	}
	else{}

	if (scan_sms_memory())
	    check_sms();
	else{}
	
	if ((sec_tick % 30) == 0)
	{
	    lcd_init();
	    strcpy(lcd_data, "PLS. WAIT...");
	    lcd_display(1);
	    check_sms();
	    strcpy(lcd_data, "SIGNAL QUALITY");
	    lcd_display(1);
	}
	else{}
	if (check_network_strength(0))
	{
	    strcpy(lcd_data, "                ");
	    strcpy(lcd_data, network_signal_strength);
	}
	else
	{   strcpy(lcd_data, "ERROR");}
	lcd_display(2);
	timer2_ms_delay(200); //200ms delay
    }
}

void debug_mode(void)
{
	CHILLER_HEATER_OFF;                       //v2.2.6  //v3.1.3
	peripheral_flags.chiller_status_flag=FALSE;         //v3.1.3
    temp_counter = 0;
    SV_VALVE_OFF;
    lcd_init();
    strcpy(lcd_data, "DEBUG MODE");
    lcd_display(1);
    strcpy(lcd_data, "INITIATING...");
    lcd_display(2);
    buzzer(HALT);
    timer2_sec_delay(error_display_time);
    clear_lcd();
    //	ds_id_presence();
    clear_lcd();
    strcpy(lcd_data, "DEBUG MODE ON");
    lcd_display(1);
    system_flags.system_stability = FALSE;
    while (mode_flags.op_mode == DEBUG_MODE)
    {
	if ((system_flags.send_idle_msg == TRUE) &&(mode_flags.op_mode != DIAGNOSTIC_MODE))
	{
	    //read_battery_status();
//            strcpy(lcd_data, "TP 1 DEBUG");            lcd_display(1); timer2_sec_delay(1);
	    peripheral_handler();
	    //			pag_error_check();
	    //LED_updates();
	    temp_cach = data_validation(validate_para_cnt);

	    send_server_response(DEV_HB_MSG);
	    system_flags.send_idle_msg = FALSE;
	    timer2_ms_delay(20);
	}
	else{}	
	if (scan_sms_memory())
	    check_sms();
	else{}

	if ((sec_tick % 10) == 0)
	{
	    check_sms();
//            strcpy(lcd_data, "TP 2 DEBUG");            lcd_display(1); timer2_sec_delay(1);
	    check_server_connection();
//            strcpy(lcd_data, "TP 3 DEBUG");            lcd_display(1); timer2_sec_delay(1);
	    while (system_flags.server_connection != SERVER_CONN_OK)
	    {
		connect_to_server();
		check_server_connection();
	    }
//            strcpy(lcd_data, "TP 5 DEBUG");            lcd_display(1); timer2_sec_delay(1);
	}
	else{}	
	if (consumer_flags.dispense_button == 1)
	{
	    consumer_flags.dispense_button = 0;
	    lcd_init();
	    strcpy(lcd_data, "DEBUG MODE ON");
	    lcd_display(1);
	    buzzer(HALT);
	    timer2_sec_delay(error_display_time);
	}
	else{}	
    }

}

void sleep_mode(void)
{
}

void refill_mode(void)
{
	CHILLER_HEATER_OFF;                       //v2.2.6   //v3.1.3
	peripheral_flags.chiller_status_flag=FALSE;          //v3.1.3
    unsigned int lit_count = 0, display_time_stamp = 0; // bkup_sec_tick = 0, HLS_count,HLS_time_stamp;	//logoff_time_stamp,refill_time_stamp=0,	//v2.2.4
    unsigned char lcd_scan = 0, toggle = 0; // dot_counter=1, HLS_flag = 0,lit_count=0;
    unsigned int temp_compare_flow_count_3=0,dummy_count_3=0; //v3.1.1.F

    refill_time_stamp = 0; //v2.2.4
    temp_compare_flow_count_3 = (int) (in_flow_calibration * DISPENSE_LEAST_COUNT); //v3.1.1.F
    dummy_count_3=1;            //v3.1.1.F
    input_flow_count=0;        //v3.1.1.F
    consumer_flags.dispense_button = 0;
    menu_flags.any_button_press = 0;
    dispense_button_count = 0;
    if ((mode_flags.op_mode == REFILL_MODE) && ((rfid_card_type == REFILL_CARD) || (rfid_card_type == FRANCHISEE_CARD)))
    {
	lcd_init();	
	strcpy(lcd_data, "REFILL MODE");
	lcd_display(1);
	strcpy(lcd_data, "PLS. WAIT");
	lcd_display(2);
	refill_time_stamp = sec_tick; //timer2_tick;
	refill_flags.refill_sub_mode = REFILL_WAIT;
	refill_litre = 0.0;
	sprintf(disp_buffer, "%3.1f", ((double) refill_litre));
	refill_time_stamp = sec_tick;
	buzzer(AUT01);
	timer2_sec_delay(error_display_time);
	
	lcd_init();
	if (peripheral_flags.hls_trigger == TRUE)
	{
	    strcpy(lcd_data, "TANK LEVEL:FULL ");
	    lcd_display(1);
	    buzzer(ERR02);
	    timer2_sec_delay(error_display_time);
	}
	else if (peripheral_flags.lls_trigger == TRUE)
	{
	    strcpy(lcd_data, "TANK LEVEL:LOW");
	    lcd_display(1);
	    strcpy(lcd_data, "START REFILLING");
	    lcd_display(2);
	}
	else
	{
	    strcpy(lcd_data, "TANK LEVEL:MED");
	    lcd_display(1);
	    strcpy(lcd_data, "START REFILLING");
	    lcd_display(2);
	}

	display_time_stamp = timer2_tick;

	while (mode_flags.op_mode == REFILL_MODE)
	{
	    scan_switch();

	    if (system_flags.send_idle_msg == TRUE)
	    {
		strcpy(lcd_data, "PLS. WAIT...1");
		lcd_display(1);
		lcd_scan = TRUE;
		send_server_response(DEV_HB_MSG);
		system_flags.send_idle_msg = FALSE;

	    }
	    else{} 

	    if ((sec_tick % 60) == 0)
	    {
		strcpy(lcd_data, "PLS. WAIT...2");
		lcd_display(1);
		lcd_scan = TRUE;
		check_sms();
	    }
	    else{}
	    //if(((sec_tick%10)==0)&&(lcd_scan==FALSE))
	    if (((timer2_tick - display_time_stamp) >= 10000)&&(lcd_scan == FALSE))
		lcd_scan = TRUE;
	    else{}	 

	    //if((timer2_tick%10000)==0)
	    if (lcd_scan == TRUE)
	    {
		lcd_scan = FALSE;
		display_time_stamp = timer2_tick;
		lcd_init();
		if (peripheral_flags.hls_trigger == TRUE)
		{
		    strcpy(lcd_data, "TANK LEVEL:FULL");
		    lcd_display(1);
		    buzzer(ERR02);
		    //timer2_sec_delay(error_display_time);
		}
		else if (peripheral_flags.lls_trigger == TRUE)
		{
		    strcpy(lcd_data, "TANK LEVEL:LOW");
		    lcd_display(1);
		}
		else
		{
		    strcpy(lcd_data, "TANK LEVEL:MED");
		    lcd_display(1);
		}
		sprintf(lcd_data, "REFILLING:%s L", disp_buffer);
		lcd_display(2);
	    }
	    else{}
	    
	    if ((refill_flags.input_flow_sensed) && ((sec_tick - refill_time_stamp) < (refill_timeout)))
	    {
		refill_flags.input_flow_sensed = LOW;
		refill_flags.refill_sub_mode = REFILL_REFILLING;
		refill_time_stamp = sec_tick;
		consumer_flags.dispense = TRUE;
		//if (input_flow_count >= ((int) (in_flow_calibration * DISPENSE_LEAST_COUNT)))
		if (input_flow_count >= ((temp_compare_flow_count_3)*(dummy_count_3))) //v3.1.1.F
		{
		 //   refill_litre += ((input_flow_count) / ((float) (in_flow_calibration)));
		 //   input_flow_count = 0;        //v3.1.1.F	
		    refill_litre += 0.1;           //v3.1.1.F	
		    dummy_count_3++;              //v3.1.1.F
		    lit_count = (unsigned int) (refill_litre * 10);
		    if ((lit_count % 100) == 0)
			buzzer(DISP);
		    else{}
		 
		    sprintf(disp_buffer, "%3.1f", ((double) refill_litre));
		    sprintf(lcd_data, "REFILLING:%s L", disp_buffer);
		    lcd_display(2);
		}
		else{}
	    }
		//---else if(((consumer_flags.dispense_type==LONG_PRESS)&&((sec_tick-refill_time_stamp)>10)) || ((sec_tick - refill_time_stamp) > (refill_timeout)))  //((consumer_flags.dispense_button) || (menu_flags.any_button_press))
		//	else if(((consumer_flags.dispense_type==LONG_PRESS)&&((sec_tick-refill_time_stamp)>5)) || ((sec_tick - refill_time_stamp) > (refill_timeout)))	//v2.2.7 - LP delay reduce to finish refilling
	    else if ((((consumer_flags.dispense_button == 1) || (menu_flags.any_button_press == 1))&&((sec_tick - refill_time_stamp) > 5)) || ((sec_tick - refill_time_stamp) > (refill_timeout))) //v2.2.7 - LP delay reduce to finish refilling
	    {
		if (school_mode_en)
		    mode_flags.op_mode = SCHOOL_MODE;
		else if (corporate_mode_en) //v2.2.5
		    mode_flags.op_mode = CORPORATE_MODE;
		else
		    mode_flags.op_mode = IDLE_MODE;

		consumer_flags.dispense_button = 0;
		menu_flags.any_button_press=0; //v3.1.1.G
		refill_flags.refill_sub_mode = REFILL_COMPLETE;
		//clear_lcd();
		lcd_init();
		strcpy(lcd_data, "REFILL COMPLETE");
		lcd_display(1);
		sprintf(disp_buffer, "%3.1f", ((double) refill_litre));
		sprintf(lcd_data, "REFILLED:%s L", disp_buffer);
		lcd_display(2);
		buzzer(AUT03);
		timer2_sec_delay(logout_idle_time);
	    }
	    else
	    {
//@		if(consumer_flags.dispense_type==LONG_PRESS)
//		{
//			consumer_flags.dispense_button=0;
//		consumer_flags.dispense_type = 0;
//		}	
//@		else{}
	    }

	    //if((timer2_tick%3000)==0)
	    //{
	    if (toggle == TRUE)
	    {
		hls_sense_processing();
		toggle = FALSE;
	    }
	    else
	    {
		lls_sense_processing();
		toggle = TRUE;
	    }
	    //}//else{}
	}
    }
    else{}   
    lcd_init();
    if ((refill_flags.refill_sub_mode == REFILL_WAIT) && (input_flow_count < 5) && (refill_litre == 0.0))
    {
	strcpy(lcd_data, "AUTO LOG OFF");
	lcd_display(1);
	strcpy(lcd_data, "REFILL CARD");
	lcd_display(2);
    }
    else
    {
	strcpy(lcd_data, "   Powered by"); //v2.2.7
	lcd_display(1);
	strcpy(lcd_data, "PIRAMAL SARVAJAL");
	lcd_display(2);

	if (refill_litre > 2)
	{
	    read_eeprom_parameter(SER_LT_EMPTY_TANK); //v2.2.3

	    if (refill_litre >= lit_empty_tank)
	    {
		if (error_flags.tank_empty == TRUE) //v2.2.2
		{
		    //system_error = TANK_EMPTY_ER;
		    error_stack[TANK_EMPTY_ER] = FALSE;
		    error_flags.tank_empty = FALSE;
		    //read_eeprom_parameter(SER_LT_EMPTY_TANK);
		}
		else{}
		//read_eeprom_parameter(SER_LT_EMPTY_TANK);
		if ((data_validation(SER_LT_EMPTY_TANK)) == 0)
		{
		    tank_low_level_litre = lit_empty_tank; //v2.2.4
		    eeprom_write(SER_TANK_LOW_LEVEL_STATUS);
		}
		else{}		
	    }
	    else
	    {
		read_eeprom_parameter(SER_TANK_LOW_LEVEL_STATUS);
	    }
	}
	else
	{
	}

	//		check_server_connection();
	//		while(system_flags.server_connection!=SERVER_CONN_OK)	//v2.2.2
	//		{
	//			system_flags.disp_led_state = DISPENSE_LED_STATE_4;
	//			connect_to_server();
	//			check_server_connection();
	//		}

	send_server_response(DEV_REFILL_TANK);
    }
    buzzer(AUT03);
    memset(rfid_card, 0, sizeof (rfid_card)); //v3.1.1.B
    memset(scanned_rfid_card, 0, sizeof (scanned_rfid_card)); //v3.1.1.B
   
    timer2_sec_delay(error_display_time); //v2.2.7

    if (school_mode_en)
	mode_flags.op_mode = SCHOOL_MODE;
    else if (corporate_mode_en) //v2.2.5
	mode_flags.op_mode = CORPORATE_MODE;
    else
    {
	mode_flags.op_mode = IDLE_MODE;
	//		if(peripheral_flags.coin_en_dis)		//v2.3.4	-> Re-enable Coin Module on Exit of refill Mode
	//			Coin_Module_En_Dis(1);
	//		else{}
    }

    	if((system_error==ERROR_FREE)&&(peripheral_flags.coin_en_dis==1)&&(peripheral_flags.dev_en_dis==TRUE)&&(coin_flags.coin_collector_error==0)&&(mode_flags.op_mode!=DEBUG_MODE)&&(mode_flags.op_mode!=DIAGNOSTIC_MODE)&&(mode_flags.op_mode!=ANTENNA_MODE)&&(mode_flags.op_mode!=SCHOOL_MODE)&&(mode_flags.op_mode!=CORPORATE_MODE))	//v2.3.4 //v6
    		coinbox_enable();
    	else{}

    rfid_card_type = INVALID_CARD;
    dispense_button_count = 0;
    consumer_flags.dispense_button = LOW;
    menu_flags.any_button_press = 0; //1.0.1
    refill_litre = 0;
    input_flow_count = 0;
    peripheral_flags.lcd_refresh = TRUE; //v2.2.7  
    temp_compare_flow_count_3 = 0;      //v3.1.1.F
    dummy_count_3 = 0;                 //v3.1.1.F
    input_flow_count = 0;            //v3.1.1.F
    LCD_BACKLITE_OFF;                //v2.2.7
    clear_lcd();	             //v3.1.1.F
    
}

/*----------------------------- Dump Card Functionality ----------------------------------
 * Description:	Dump Card Function:
 * -> Scan card if the card is dump card, it will wait for the button long press for 30 seconds.
 * -> If button not pressed within time-out, it will log off from dump mode.
 * -> If button pressed, sv will open and allow water to dispense till the LLS triggered or User button long press.
 * -> As soon as the user long press sensed for water dispense, it will send the response(Alert message RL10) to server for "Dump Start".
 * -> If the LLS triggered, it will consider that the tank is empty, and will close the SV, log off from dump mode and send the dump complete (Alery msg RL06) to server along with the amount of water dumped.
 * -> During the dumping process it will show the live amount of water being dispensed along with the beep sound at every litre of water.
 * -> If the water is not dispensed within flow sensor time out, it will declare either SV error or Flow sensor error and send the Alert message RL14 or RL13 repectively.
 * -> In any case, may it be auto logoff, user log off, tank empty, SV error or flow sensor error, the device will send the dump complete msg (RL06 with amount of dumped water).
-----------------------------------------------------------------------------------------*/
void dump_mode(void)
{
  	CHILLER_HEATER_OFF;                       //v2.2.6   //v3.1.3
	peripheral_flags.chiller_status_flag=FALSE;          //v3.1.3

    unsigned int dump_time_stamp = 0, lit_count = 0, display_time_stamp = 0;
    unsigned int temp_compare_flow_count_2=0,dummy_count_2=0; //v3.1.1.F
    unsigned char lcd_scan = 0; //temp_counter=0;  //v3.1.1.C
    //float lit_count=0.0;

    //@	menu_flags.dispense_type =0; //consumer_flags.dispense_type = 0;
    consumer_flags.dispense_button = 0;
    menu_flags.any_button_press = 0;

    mode_flags.op_sub_mode = DUMP_AUTH_OK;
    temp_compare_flow_count_2 = (int) (out_flow_calibration * DISPENSE_LEAST_COUNT); //v3.1.1.F
    dispense_button_count = 0;
    dummy_count_2=1; //v3.1.1.F
    output_flow_count = 0;
    dispense_litre = 0.0;
    dump_time_stamp = timer2_tick;
    dispense_trail = 0;

//    timer2_sec_delay(error_display_time);   //v3.1.1.F
    lcd_init();
    if (peripheral_flags.lls_trigger == TRUE)
    {
	strcpy(lcd_data, "TANK LEVEL:LOW");
    }
    else if (peripheral_flags.hls_trigger == TRUE)
    {

	strcpy(lcd_data, "TANK LEVEL:FULL");
    }
    else
    {

	strcpy(lcd_data, "TANK LEVEL:MED");
    }
    lcd_display(1);
    timer2_sec_delay(error_display_time);

    if ((dump_flags.remote_water_dump == TRUE))
    {
	consumer_flags.dispense_button = 1; // consumer_flags.dispense_button = 1;
	//@		menu_flags.dispense_type =0; //consumer_flags.dispense_type = 0;
	system_flags.server_response = SERVER_RESPONSE_OK;
	rfid_card_type = DUMP_CARD;
    }
    else{}
  
    while (((timer2_tick - dump_time_stamp)<(dump_timeout * 1000)) && (mode_flags.op_mode == DUMP_MODE)&& (consumer_flags.dispense_button == 0) &&(menu_flags.any_button_press == 0) && ((rfid_card_type == DUMP_CARD) || (rfid_card_type = FRANCHISEE_CARD)) && (system_flags.server_response == SERVER_RESPONSE_OK))
    {
	scan_switch();
	strcpy(lcd_data, "PRESS ANY BUTTON");
	lcd_display(1);
	strcpy(lcd_data, "START WATER DUMP");
	lcd_display(2);
	if (dump_flags.remote_water_dump == TRUE)
	{
	    //consumer_flags.dispense_button = 1;
	    consumer_flags.dispense_button = 1;
	    //consumer_flags.dispense_type = LONG_PRESS;
	}
	else{}	
    }

    if ((mode_flags.op_mode) && ((consumer_flags.dispense_button) || (menu_flags.any_button_press)) && ((rfid_card_type == DUMP_CARD) || (rfid_card_type == FRANCHISEE_CARD))) //v2.2.5
    {
	//@		  consumer_flags.dispense_type = 0;
	consumer_flags.dispense_button = 0;
	menu_flags.any_button_press = 0;
	//consumer_flags.dispense_button = 0; v2.2.5
	lcd_init();
	strcpy(lcd_data, "DUMP MODE");
	lcd_display(1);
	sprintf(lcd_data, "PLS. WAIT...");
	lcd_display(2);
	send_server_response(DEV_DUMP_START);
	clear_lcd();

	if (peripheral_flags.lls_trigger == TRUE)
	{
	    strcpy(lcd_data, "TANK LEVEL:LOW");
	}
	else if (peripheral_flags.hls_trigger == TRUE)
	{
	    strcpy(lcd_data, "TANK LEVEL:FULL");
	}
	else
	{
	    strcpy(lcd_data, "TANK LEVEL:MED");
	}
	lcd_display(1);
	dump_time_stamp = timer2_tick;
	display_time_stamp = timer2_tick;
	mode_flags.op_sub_mode = DUMP_DISPENSE;
	buzzer(DISP);
	timer2_sec_delay(error_display_time); //ask  add to debounce remove
	sprintf(disp_buffer, "%3.1f", ((double) dispense_litre));  //v3.1.1.F
	sprintf(lcd_data, "DUMPING:%s L", disp_buffer);
	lcd_display(2);
	consumer_flags.dispense_button = 0;
	menu_flags.any_button_press = 0;
	output_flow_count=0;   //v3.1.1.F
	while (((timer2_tick - dump_time_stamp)<(dump_timeout * 1000)) && (mode_flags.op_sub_mode != DUMP_COMPLETE))
	{
	    SV_VALVE_ON;
	    mode_flags.op_sub_mode = DUMP_DISPENSE;

	    lls_sense_processing();

	    if (system_flags.send_idle_msg == TRUE)
	    {
		strcpy(lcd_data, "PLS. WAIT.......");
		lcd_display(1);
		lcd_scan = TRUE;
		send_server_response(DEV_HB_MSG);
		system_flags.send_idle_msg = FALSE;
		//timer2_ms_delay(20);
	    }
	    else{}
	    //if(((sec_tick%10)==0)&&(lcd_scan==FALSE))
	    if (((timer2_tick - display_time_stamp) >= 10000)&&(lcd_scan == FALSE))
		lcd_scan = TRUE;
	    else{}
	    //if((timer2_tick%10000)==0)
	    if (lcd_scan == TRUE)
	    {
		lcd_scan = FALSE;
		display_time_stamp = timer2_tick;
		lcd_init();
		if (peripheral_flags.lls_trigger == TRUE)
		{
		    strcpy(lcd_data, "TANK LEVEL:LOW");
		}
		else if (peripheral_flags.hls_trigger == TRUE)
		{
		    strcpy(lcd_data, "TANK LEVEL:FULL");
		}
		else
		{
		    strcpy(lcd_data, "TANK LEVEL:MED");
		}
		lcd_display(1);
		sprintf(lcd_data, "DUMPING:%s L", disp_buffer);
		lcd_display(2);
	    }
	    else{}

	    if ((consumer_flags.output_flow_sensed) && (mode_flags.op_mode == DUMP_MODE))
	    {
		dump_time_stamp = timer2_tick;
		consumer_flags.output_flow_sensed = LOW;
		if (consumer_flags.dispense == 0)
		{
		    consumer_flags.dispense = TRUE;
		    dispense_litre += DISPENSE_LEAST_COUNT;
		}
		else{}
	
		consumer_flags.output_flow_sensed = LOW;
		//if (output_flow_count >= ((int) (out_flow_calibration * DISPENSE_LEAST_COUNT))) //3.1F
	        if (output_flow_count >= ((temp_compare_flow_count_2)*(dummy_count_2))) //v3.1.1.F
		{		   
		    //dispense_litre += ((output_flow_count) / ((float) (out_flow_calibration)));
		    // output_flow_count = 0;      //v3.1.1.F
		    dispense_litre += 0.1;         //v3.1.1.F	
		    dummy_count_2++;               //v3.1.1.F	
		   
		    lit_count = (unsigned int) (dispense_litre * 10);
		    if ((lit_count % 100) == 0)
			buzzer(DISP);
		    else{}	
		    //dispense_litre = (((unsigned dispense_litreint) (dispense_litre * 10)) / 10.0); //v3.1.1.F //unded 100ml complete than display 0.1 increment
		    sprintf(disp_buffer, "%3.1f", dispense_litre);
		    sprintf(lcd_data, "DUMPING:%s L", disp_buffer);
		    lcd_display(2);
		}
		else{}	   
	    }
	    else
	    {
		// Checking for LLS and Flow sensor error simultaneously
		if ((peripheral_flags.lls_trigger == TRUE) && (mode_flags.op_sub_mode == DUMP_DISPENSE)&&((timer2_tick - dump_time_stamp) > 5000))
		{
		    if (LLS_PIN_CHECK == 0)
		    {
			SV_VALVE_OFF;
			mode_flags.op_sub_mode = DUMP_COMPLETE;
			clear_lcd();
			strcpy(lcd_data, "DUMP COMPLETE");
			lcd_display(1);
			sprintf(disp_buffer, "%3.1f", ((double) dispense_litre));
			sprintf(lcd_data, "DUMPED:%s L", disp_buffer);
			lcd_display(2);
			buzzer(AUT03);
		    }
		    else{}		 
		}

		    // Check if there is a Flow sensor error or not
		    // If there is no LLS triggered with dumping is on but flow sensor error is there then it is actually the flow sensor error
		    //else if(((timer2_tick-dump_time_stamp)>10000) && (mode_flags.op_sub_mode==DUMP_DISPENSE) && (peripheral_flags.lls_trigger==FALSE))
		else if (((timer2_tick - dump_time_stamp) > 8000) && (mode_flags.op_sub_mode == DUMP_DISPENSE) && (peripheral_flags.lls_trigger == FALSE) && (peripheral_flags.lls_hls_en_dis >= 2))
		{
		    water_dispense_error();
		    dump_time_stamp = timer2_tick;
		    if (dispense_trail == 'E')
			mode_flags.op_sub_mode = DUMP_COMPLETE;
		    else
			continue;

		}
		else if ((peripheral_flags.lls_hls_en_dis < 2)&& (mode_flags.op_sub_mode == DUMP_DISPENSE)&&((timer2_tick - dump_time_stamp) > 5000))
		{//LLS disabled
		    SV_VALVE_OFF;
		    mode_flags.op_sub_mode = DUMP_COMPLETE;
		    clear_lcd();
		    strcpy(lcd_data, "DUMP COMPLETE");
		    lcd_display(1);
		    sprintf(disp_buffer, "%3.1f", ((double) dispense_litre));
		    sprintf(lcd_data, "DUMPED:%s L", disp_buffer);
		    lcd_display(2);
		    buzzer(AUT03);
		}
		else{}
	
	    }

	    scan_switch();
	    //if((consumer_flags.dispense_type == LONG_PRESS)&&(dump_flags.remote_water_dump==FALSE))
	    if (((consumer_flags.dispense_button == 1) || (menu_flags.any_button_press == 1))&&(dump_flags.remote_water_dump == FALSE)) //LONG_PRESS =1
	    {
		//consumer_flags.dispense_type = 0;

		if (mode_flags.op_sub_mode == DUMP_DISPENSE)
		{
		    SV_VALVE_OFF;
		    mode_flags.op_sub_mode = DUMP_COMPLETE;
		    clear_lcd();
		    strcpy(lcd_data, "DUMP COMPLETE");
		    lcd_display(1);
		    sprintf(disp_buffer, "%3.1f", ((double) dispense_litre));
		    sprintf(lcd_data, "DUMPED:%s L", disp_buffer);
		    lcd_display(2);
		    buzzer(AUT03);
		}
		else{}		
		dispense_button_count = 0;
		peripheral_flags.tds_update = FALSE;
		consumer_flags.dispense_button = 0;
		menu_flags.any_button_press = 0;
	    }
	    else{}
	    if ((sec_tick % 60) == 0)
	    {
		strcpy(lcd_data, "PLS. WAIT.......");
		lcd_display(1);
		lcd_scan = TRUE;
		check_sms();
	    }
	    else{}	  
	}
    }
    else{}

    SV_VALVE_OFF;
    timer2_sec_delay(logout_idle_time);

    lcd_init();
    if (mode_flags.op_mode != ERROR_MODE)
    {
	if ((mode_flags.op_sub_mode == DUMP_AUTH_OK) && (dispense_litre == 0.0) && (rfid_card_type != INVALID) && (output_flow_count < 5) && (system_flags.server_response == SERVER_RESPONSE_OK))
	{
	    strcpy(lcd_data, "AUTO LOG OFF");
	    lcd_display(1);
	    strcpy(lcd_data, "RESCAN CARD");
	    lcd_display(2);
	}
	else
	{
	    strcpy(lcd_data, "   Powered by"); //v2.2.7
	    lcd_display(1);
	    strcpy(lcd_data, "PIRAMAL SARVAJAL");
	    lcd_display(2);
	    battery_verify_time_stamp = min_tick;

	    if (error_flags.water_expired == TRUE)
	    {
		error_flags.water_expired = FALSE;
		eeprom_write(SER_WATER_EXPIRED);
	    }
	    else{}	  
	    /*if(error_flags.tank_empty==TRUE)	//v2.2.2
	    {
		    //system_error = TANK_EMPTY_ER;
		    error_stack[TANK_EMPTY_ER] = FALSE;
		    error_flags.tank_empty = FALSE;
		    read_eeprom_parameter(SER_LT_EMPTY_TANK);
	    }else{}*/
	}
	buzzer(AUT03);
	timer2_sec_delay(logout_idle_time);

	if (school_mode_en)
	    mode_flags.op_mode = SCHOOL_MODE;
	else if (corporate_mode_en) //v2.2.5
	    mode_flags.op_mode = CORPORATE_MODE;
	else
	{
	    mode_flags.op_mode = IDLE_MODE;
	    //			if(peripheral_flags.coin_en_dis)		//v2.3.4	-> Re-enable Coin Module on Exit of dump Mode
	    //				Coin_Module_En_Dis(1);
	    //			else{}
	}
    }
    else{}
    
    //	check_server_connection();  //3.1.0
    //		temp_counter = 0;
    //		while((system_flags.server_connection!=SERVER_CONN_OK)&&(temp_counter<2)){
    //		connect_to_server();
    //		check_server_connection();	//v2.2.7
    //		temp_counter++;
    //	}
    //	temp_counter = 0;
    send_server_response(DEV_DUMP_COMPLETE);
    dispense_litre = 0;
    memset(rfid_card, 0, sizeof (rfid_card)); //v3.1.1.B  //remove for loop
    memset(scanned_rfid_card, 0, sizeof (scanned_rfid_card)); //v3.1.1.B //remove for loop

    if((system_error==ERROR_FREE)&&(peripheral_flags.coin_en_dis==1)&&(peripheral_flags.dev_en_dis==TRUE)&&(coin_flags.coin_collector_error==0)&&(mode_flags.op_mode!=DEBUG_MODE)&&(mode_flags.op_mode!=DIAGNOSTIC_MODE)&&(mode_flags.op_mode!=ANTENNA_MODE)&&(mode_flags.op_mode!=SCHOOL_MODE)&&(mode_flags.op_mode!=CORPORATE_MODE))	//v2.3.4  //v6
        coinbox_enable();
    else{}
  
    if (dump_flags.remote_water_dump == TRUE)
	dump_flags.remote_water_dump = FALSE;
    else{}

    consumer_flags.dispense_button = 0;
    menu_flags.any_button_press = 0;
    //@	consumer_flags.dispense_type = 0;
    consumer_flags.rfid_read = LOW;
    rfid_reader_counter = 0;
    dispense_litre = 0;
    peripheral_flags.tds_update = TRUE;
    system_flags.server_response = FALSE;
    peripheral_flags.lcd_refresh = TRUE; //v2.2.7
    button_detect = 0;
    temp_compare_flow_count_2 = 0;      //v3.1.1.F
    dummy_count_2 = 0;                  //v3.1.1.F
    output_flow_count = 0;              //v3.1.1.F
    LCD_BACKLITE_OFF;	                //v3.1.1.F //change order   
    clear_lcd();	                //v3.1.1.F //change order   
}

void consumer_mode(void)
{
    unsigned int cs_time_stamp = 0, temp_int = 0; //,temp=0;
    unsigned long int updated_user_bal_paisa = 0;
    unsigned char lit_count = 0; //loop=0,decimal[1]={0};
    unsigned char transcation_mode[3] = {0};
    //signed long Minus_Bal_Verify=0;  //v3.1.1.C
    unsigned int temp_compare_flow_count = 0, dummy_count = 0; //round_of_value =0, //v3.1.1.C
    //float lit_count=0.0;
    consumer_flags.dispense_error = 0;

    mode_flags.op_mode = CONSUMER_MODE;
    consumer_flags.acc_switch_detect = FALSE;
    LED4_ON;

    if (litre_transcation == 1)
	strcpy(transcation_mode, "Rs");
    else if (litre_transcation == 2)
	strcpy(transcation_mode, "L");
	//else{}
    else
	strcpy(transcation_mode, "_");

    mode_flags.op_sub_mode = CON_AUTH_OK;

    dispense_button_count = 0;
    cs_time_stamp = timer2_tick;
    display_toggle_time_stamp = timer2_tick;
    peripheral_flags.tds_update = 0;
    dispense_trail = 0;

    if (peripheral_flags.temp_en_dis == 0)
    {
	DS_temperature = 0;
	memset(temp_value, '\0', sizeof (temp_value));
	temp_value[0] = 'O';
	temp_value[1] = 'F', temp_value[2] = 'F';
	temp_value[3] = '\0';
    }
    else{}  
//    if ((peripheral_flags.lls_hls_en_dis >= 2)&&(peripheral_flags.lls_trigger == TRUE))
//    {
//        lcd_init();
//        strcpy(lcd_data, "TANK WATER");
//        lcd_display(1);
//        strcpy(lcd_data, "LEVEL LOW");
//        lcd_display(2);
//        buzzer(ERR01);
//        timer2_sec_delay(error_display_time);
//    }
//    else{}
    if ((mode_flags.op_mode == CONSUMER_MODE)&&(rfid_card_type == CONSUMER_CARD))
    {
        clear_lcd();
        output_flow_count = 0;
        temp_compare_flow_count = (int) (out_flow_calibration * DISPENSE_LEAST_COUNT); //v3.1.1.B
        dummy_count = 1;
        dispense_litre = 0.0;
        lit_count = 0.0;
        consumer_flags.dispense_button = 0; //v2.2.2
        consumer_flags.card_detect = 0; //v3.1.0
        consumer_flags.rfid_read = 0; //1.0.1
        sprintf(disp_buffer, "%2.1f", ((double) dispense_litre)); 
        cs_time_stamp = timer2_tick;
        //no_dispense_state = 0;
        if (acc_dispense_capacity <= (DISPENSE_LEAST_COUNT))
        {
            strcpy(lcd_data, "BALANCE OVER");
            lcd_display(1);
            strcpy(lcd_data, "RECHARGE CARD");
            lcd_display(2);

            buzzer(ERR01);
            timer2_sec_delay(error_display_time);

            SV_VALVE_OFF;
            //@		consumer_flags.dispense_type = 0;
            consumer_flags.dispense_button = 0;
            consumer_flags.dispense = 0;
            mode_flags.op_sub_mode = CON_COMPLETE;
    	}
        else
        {          
                sprintf(lcd_data, "BL:%.6s%.2s T:%.2s", acc_credit_array, transcation_mode, temp_value);
                lcd_display(1);
                sprintf(lcd_data, "TDS:%.4s D:%sL", tds_value, disp_buffer);
                lcd_display(2);
             if ((peripheral_flags.lls_hls_en_dis >= 2)&&(peripheral_flags.lls_trigger == TRUE))
            {
                timer2_sec_delay(2);
                lcd_init();
                strcpy(lcd_data, "TANK WATER");
                lcd_display(1);
                strcpy(lcd_data, "LEVEL LOW");
                lcd_display(2);
                buzzer(ERR01);
                timer2_sec_delay(2);
                sprintf(lcd_data, "BL:%.6s%.2s T:%.2s", acc_credit_array, transcation_mode, temp_value);
                lcd_display(1);
                sprintf(lcd_data, "TDS:%.4s D:%sL", tds_value, disp_buffer);
                lcd_display(2);
            }
            else{}

             general_time_stamp_logout = sec_tick;  //v3.1.1.G
             balance_deduct =0;
        while (((timer2_tick - cs_time_stamp)<(dispense_timeout * 1000)) && (mode_flags.op_sub_mode != CON_COMPLETE))
        {
    
		if ((sec_tick - general_time_stamp_logout)>3) //v3.1.1.G
		{
		    general_time_stamp_logout = sec_tick;		   
		    lcd_init();
		    sprintf(lcd_data, "BL:%.6s%.2s T:%.2s", acc_credit_array, transcation_mode, temp_value);
		    lcd_display(1);
		    sprintf(lcd_data, "TDS:%.4s D:%sL", tds_value, disp_buffer);
      	    lcd_display(2);
		}
		else{}
		
            if ((mode_flags.op_sub_mode == CON_HALT) && ((timer2_tick - cs_time_stamp) > 2000)) //v3.1.1.B
            {
                consumer_flags.dispense = 0;
                mode_flags.op_sub_mode = CON_COMPLETE;
                consumer_flags.dispense_button = 0; //v2.2.2
            }
            else{}	
		
            if ((consumer_flags.output_flow_sensed) && (mode_flags.op_mode == CONSUMER_MODE))
            {
                //			if(consumer_flags.dispense==0)  //v3.1.1.D
                //			{
                //				consumer_flags.dispense = TRUE;
                //				//----dispense_litre += DISPENSE_LEAST_COUNT;		//v2.2.8 (to count 100ml only when complete 100ml dispensed)
                //			}
                //			else{}


                if (mode_flags.op_sub_mode == CON_DISPENSE)
                cs_time_stamp = timer2_tick;
                else{}

                //dispense_trail = 0;		
                consumer_flags.output_flow_sensed = LOW;
                //if(output_flow_count>=((int)(out_flow_calibration*DISPENSE_LEAST_COUNT)))
                if (output_flow_count >= ((temp_compare_flow_count)*(dummy_count))) //v3.1.1.B
                {
    //			dispense_litre += ((output_flow_count)/((float)(out_flow_calibration)));
    //			output_flow_count = 0;
    //			dispense_litre = ((output_flow_count)/((float)(out_flow_calibration)));  //v3.1.1.B //v3.1.1.D
    //			dispense_litre =(((unsigned int)(dispense_litre*10))/10.0);  // { v3.1.1.B  //v3.1.1.D

    //			dispense_litre = dispense_litre +MIN_LITRE;
    //			dispense_litre = (((unsigned int) (dispense_litre * 10)) / 10.0); //v3.1.1.F to discard second decimal point

                    dispense_litre = (double)(dummy_count/10.0);     //3.1.1
                    dummy_count++; //v3.1.1.B			    

                    dispense_trail = 0;
                    sprintf(disp_buffer, "%2.1f", dispense_litre); //v3.1.1.D
                    sprintf(lcd_data, "TDS:%.4s D:%sL", tds_value, disp_buffer);
                    lcd_display(2); // v3.1.1.B  } 
                    //---lit_count++;
                    //---if((lit_count%10)==0)
                    //---if((dispense_litre>=(acc_dispense_capacity-flow_offset))&&(mode_flags.op_sub_mode==CON_DISPENSE))
                     if ((dispense_litre >= acc_dispense_capacity)&&(mode_flags.op_sub_mode == CON_DISPENSE)) //v2.2.8
                    {
                        SV_VALVE_OFF;
                        //--mode_flags.op_sub_mode = CON_COMPLETE;
                        mode_flags.op_sub_mode = CON_HALT;
                        //@			consumer_flags.dispense_type = 0;
                        consumer_flags.dispense = 0;

                        strcpy(lcd_data, "MAXIMUM DISPENSE");
                        lcd_display(1);
                        strcpy(lcd_data, "LIMIT REACHED");
                        lcd_display(2);
                        timer2_sec_delay(error_display_time);
                        cs_time_stamp = timer2_tick;
                        //consumer_flags.dispense_error = 1;
                        //if (coin_flags.coinselect == 1)
                        //{
                    //	mode_flags.op_sub_mode = CON_COMPLETE;
                       // }
                        break;
                    } else{}			

                    if ((dispense_litre - lit_count) >= 1)
                    {
                        buzzer(DISP);
                        sprintf(lcd_data,"BL:%.6s%.2s T:%.2s",acc_credit_array,transcation_mode,temp_value);  //v3.1.1.H
                        lcd_display(1);                                                                       //v3.1.1.H
                        lit_count = dispense_litre;
                        if (timer_flags.check_temperature == 1) //1.0.1
                        {
                        timer_flags.check_temperature = 0;
                        read_temperature();
                        }else{}			    
                    }else{}
                }else{}
            }
            else
            {
                if (((timer2_tick - cs_time_stamp) > 2000) && (mode_flags.op_sub_mode == CON_DISPENSE))
                {
                    water_dispense_error();
                    if (dispense_trail == 'E')
                    {
                        mode_flags.op_sub_mode = CON_HALT;
                        consumer_flags.dispense_error = 1;
                    }
                    else                  
                    {cs_time_stamp = timer2_tick;}                    		
                }
                else
                {
                    if ((mode_flags.op_sub_mode == CON_HALT)&&(dispense_trail >= 1))
                    {
                        dispense_trail = 'E';
                        consumer_flags.dispense_error = 1;
                        water_dispense_error();	
                        
                        if(coin_flags.coinselect==1)
                        {
                            mode_flags.op_sub_mode == CON_COMPLETE;
                            break;
                        }else{}
                    }
                    else{}			
                }
            }

    //		//---if((dispense_litre>=(acc_dispense_capacity-flow_offset))&&(mode_flags.op_sub_mode==CON_DISPENSE))
    //		if ((dispense_litre >= acc_dispense_capacity)&&(mode_flags.op_sub_mode == CON_DISPENSE)) //v2.2.8
    //		{
    //		    SV_VALVE_OFF;
    //		    //--mode_flags.op_sub_mode = CON_COMPLETE;
    //		    mode_flags.op_sub_mode = CON_HALT;
    //		    //@			consumer_flags.dispense_type = 0;
    //		    consumer_flags.dispense = 0;
    //
    //		    strcpy(lcd_data, "MAXIMUM DISPENSE");
    //		    lcd_display(1);
    //		    strcpy(lcd_data, "LIMIT REACHED");
    //		    lcd_display(2);
    //		    timer2_sec_delay(error_display_time);
    //		    cs_time_stamp = timer2_tick;
    //		    //   consumer_flags.dispense_error = 1;  //2.3.G
    //		    if (coin_flags.coinselect == 1)
    //		    {
    //			mode_flags.op_sub_mode = CON_COMPLETE;
    //		    }
    //		    break;
    //		}
    //		else
    //		{
    //		}
                if ((consumer_flags.card_detect == 0) && (consumer_flags.dispense_error == 0)&& (mode_flags.op_sub_mode != CON_HALT)) //v2.2.2
                {
                    SV_VALVE_ON;
                    mode_flags.op_sub_mode = CON_DISPENSE;
                    dispense_button_count = 0;
                    peripheral_flags.tds_update = 0;
        //		    sprintf(lcd_data, "BL:%.6s%.2s T:%.2s", acc_credit_array, transcation_mode, temp_value);
        //		    lcd_display(1);
        //		    sprintf(lcd_data, "TDS:%.4s D:%sL", tds_value, disp_buffer);
        //		    lcd_display(2);
                }
                else
                {
                //else if (((consumer_flags.card_detect == 1) || (consumer_flags.dispense_error == 1))&&(coin_flags.coinselect == 0))  //3.1.2 //
                //{
                    consumer_flags.dispense_complete = 0;
                    if ((consumer_flags.dispense_error == 1)&&(coin_flags.coinselect==0))
                    {
//                    consumer_flags.dispense_error = 0;
                        SV_VALVE_OFF;
                        strcpy(lcd_data, "PLS. PUT CARD   ");
                        lcd_display(1);
                        strcpy(lcd_data, "TO UPDATE BAL.  ");
                        lcd_display(2);
                        consumer_flags.card_detect = LOW;
                        consumer_flags.rfid_read = LOW;
                       
                        button_detect = 0;
                        cs_time_stamp = timer2_tick;
                        while ((consumer_flags.card_detect != HIGH)&&((timer2_tick - cs_time_stamp)<(user_timeout * 500)))
                        {
                            if (button_detect == MENU_DETECT)
                            break;
                        }
                        if (card_Serial_number() == 1) //mode_flags.op_sub_mode==CON_DISPENSE
                        {
                            if (!strcmp(offline_rfid_card, rfid_card))
                            {
                                consumer_flags.dispense_complete = 1;
                                strcpy(lcd_data, "PLS. HOLD CARD  ");
                                lcd_display(1);
                                strcpy(lcd_data, "ON SCAN AREA    ");
                                lcd_display(2);
                            }
                            else
                            {
                                strcpy(lcd_data, "PLS. SCAN    ");
                                lcd_display(1);
                                strcpy(lcd_data, "SAME CARD    ");
                                lcd_display(2);
                                buzzer(ERR01);
                                timer2_sec_delay(1);
                            }
                        }
                        else
                        { 
                            clear_lcd();  //v3.1.2.H
                            if(consumer_flags.card_detect== 0)       //v3.1.2.H
                            {
                                strcpy(lcd_data, "USER TIME OUT");   //v3.1.2.H
                                lcd_display(1);
                                strcpy(lcd_data, "PLS. WAIT");
                                lcd_display(2);
                            }
                            else{
                                strcpy(lcd_data, "NOT PROPER SCAN");  
                                lcd_display(1);
                                strcpy(lcd_data, "PLS. TRY AGAIN");
                                lcd_display(2);
                            }
                            
                            buzzer(ERR01);
                            timer2_sec_delay(1);
                        }
                    }
                    else if ((consumer_flags.card_detect == 1)&&(coin_flags.coinselect==0))
                    {
                        consumer_flags.card_detect = 0; //1.0.1
                        if (card_Serial_number() == 1) //mode_flags.op_sub_mode==CON_DISPENSE
                        {
                            if (!strcmp(offline_rfid_card, rfid_card))
                            {
                                consumer_flags.dispense_complete = 1;
                                strcpy(lcd_data, "PLS. HOLD CARD  ");
                                lcd_display(1);
                                strcpy(lcd_data, "ON SCAN AREA    ");
                                lcd_display(2);
                            }
                            else
                            {
                                strcpy(lcd_data, "PLS. SCAN    ");
                                lcd_display(1);
                                strcpy(lcd_data, "SAME CARD    ");
                                lcd_display(2);
                                buzzer(ERR01);
                                timer2_sec_delay(1);
                            }
                        }
                        else
                        {
                            strcpy(lcd_data, "NOT PROPER SCAN");
                            lcd_display(1);
                            strcpy(lcd_data, "PLS. TRY AGAIN");
                            lcd_display(2);
                            buzzer(ERR01);
                            timer2_sec_delay(1);
                        }			
                    }
                    else{}		 

                    if ((consumer_flags.dispense_complete == 1)&&(coin_flags.coinselect==0))
                    {
                        balance_deduct = dispense_litre;

                        updated_user_bal_paisa = 0; //v3.1.1.B
                            if (balance_deduct > acc_dispense_capacity)
                            {
                                updated_user_bal_paisa = (((unsigned long int) (balance_deduct * offline_water_tariff)) - Rfid_user_bal_paisa); //v3.1.1.B

                                if (updated_user_bal_paisa > curnt_user_bal_paisa)
                                {
                                    updated_user_bal_paisa = 0;
                                    curnt_user_bal_paisa = updated_user_bal_paisa;
                                }
                                else
                                {
                                    curnt_user_bal_paisa = curnt_user_bal_paisa - updated_user_bal_paisa;
                                }

                                if (rfid_Epurse_decrement(0x00, 0x08, updated_user_bal_paisa))
                                {
                                    SV_VALVE_OFF;
                                    //rfid_read_purse(0x00,0x08);  
                                    curnt_user_bal_paisa = curnt_user_bal_paisa - updated_user_bal_paisa;
                                    curnt_user_bal_rs = (((double) (curnt_user_bal_paisa)) / 100.0); //convert into rs to display //v3.1.1.B
                                    sprintf(acc_credit_array, "%3.2f", curnt_user_bal_rs); //(double)curnt_user_bal_rs);  //v3.1.1.B				
                                    consumer_flags.dispense_error = 0;
                                    consumer_flags.dispense = 0;
                                    mode_flags.op_sub_mode = CON_COMPLETE;
                                    timer2_ms_delay(1500);
                                    buzzer(AUT01);
                                }
                                else
                                {
                                    
                                                        mode_flags.op_sub_mode = CON_DISPENSE;
                                                        strcpy(lcd_data, "NOT PROPER SCAN");  //3.1.2
                                                        lcd_display(1);
                                                        strcpy(lcd_data, "PLS. TRY AGAIN");
                                                        lcd_display(2);
                                                        buzzer(ERR01);                                                        
                                }
                            }
                            else
                            {
                                             
                                updated_user_bal_paisa = (Rfid_user_bal_paisa - ((unsigned long int) (balance_deduct * offline_water_tariff))); //v3.1.1.B	

                                if (rfid_Epurse_increment(0x00, 0x08, updated_user_bal_paisa))
                                {
                                SV_VALVE_OFF;
                                //rfid_read_purse(0x00,0x08);   
                                curnt_user_bal_paisa = curnt_user_bal_paisa + updated_user_bal_paisa;
                                curnt_user_bal_rs = (((double) curnt_user_bal_paisa) / 100.0); //convert into rs to display  //v3.1.1.B
                                sprintf(acc_credit_array, "%3.2f", curnt_user_bal_rs); //(double)curnt_user_bal_rs);		  //v3.1.1.B		
                                consumer_flags.dispense_error = 0;
                                consumer_flags.dispense = 0;
                                mode_flags.op_sub_mode = CON_COMPLETE;
                                timer2_ms_delay(500);
                                buzzer(AUT01); //3.1.0	 
                                }
                                else
                                {
                                                        mode_flags.op_sub_mode = CON_DISPENSE;
                                                        strcpy(lcd_data, "NOT PROPER SCAN");  //3.1.2
                                                        lcd_display(1);
                                                        strcpy(lcd_data, "PLS. TRY AGAIN");
                                                        lcd_display(2);
                                                        buzzer(ERR01);
                                                       
                                                        
                                }
                            }
                        consumer_flags.card_detect = 0;
                        sprintf(lcd_data, "BL:%.6s%.2s T:%.2s", acc_credit_array, transcation_mode, temp_value);
                        lcd_display(1);
                        sprintf(lcd_data, "TDS:%.4s D:%sL", tds_value, disp_buffer);
                        lcd_display(2);
                        cs_time_stamp = timer2_tick;
                    }
                    else{} //end of card_Serial_number

                    consumer_flags.card_detect = 0;
//                    consumer_flags.dispense_error = 0;  //3.1.2
                }
//                else{}
                if(consumer_flags.dispense_button==1)
                {
                    SV_VALVE_OFF;    
                    consumer_flags.dispense_error = 0;
                    break;
                }
                 else{}
            if (mode_flags.op_sub_mode == CON_COMPLETE) //3.1.0 //19/05/17
            {
                break;
            }
            //if((dispense_litre-lit_count) < 1.0) &&((sec_tick-water_time_stamp)>5))
        }// end of while()
	}
	balance_deduct = dispense_litre;

	send_server_response(DEV_BALANCE_DEDUCT); //v3.0.0

	total_dispense_litre += balance_deduct;
	tank_low_level_update(balance_deduct); //v2.2.3

	FSchdir("\\");
	FSchdir("TOTAL");
	memset(file_name, '\0', sizeof (file_name));
	//sprintf(file_name,"%.5s.txt","TOTAL");
	sprintf(file_name, "%.8s.txt", "Cur_date"); //1.0.1	//v3.1.1.D
	FSfclose(sd_pointer); //@ f_close(&fil);
	sd_pointer = FSfopen(file_name, FS_READPLUS);
	if (FSfread(daily_total_lit_ary, 1, 10, sd_pointer) != 10)
	{}	
	else{}	
	FSfclose(sd_pointer);
	daily_total_lit = atol(daily_total_lit_ary);
	daily_total_dispense = (((double) daily_total_lit) / 100.0); //(double)daily_total_lit/100.0;

	daily_total_dispense += balance_deduct;
	daily_total_lit = (unsigned long int) (daily_total_dispense * 100);
        memset(daily_total_lit_ary, '\0', sizeof (daily_total_lit_ary)); //v3.1.2.G
	sprintf(daily_total_lit_ary, "%.10ld", daily_total_lit);

	memset(file_name, '\0', sizeof (file_name));
	sprintf(file_name, "%.8s.txt", "Cur_date"); //1.0.1	 //v3.1.1.D
	FSfclose(sd_pointer);
	sd_pointer = FSfopen(file_name, FS_READPLUS); //@	f_open(&fil,file_name,FA_READ | FA_WRITE );

	if (FSfwrite(daily_total_lit_ary, 1, 10, sd_pointer) != 10)
	{ //@	f_write (&fil,daily_total_lit_ary,10,&br);
	}
	else{}
	
	FSfclose(sd_pointer);
	FSchdir("\\");

	//	lcd_init();    

	if (mode_flags.op_coin_mode == 1)
	{ //v2.2.9_coin //sndp-> to not update balance deduct //1.0.2
	    //mode_flags.op_coin_mode = 0;
	    coin_flags.coin_total_amount_write = 1;
	    coin_ideal_time_stamp = sec_tick;
	}
	else
	{
	    curnt_user_bal_rs = (((double) (curnt_user_bal_paisa)) / 100.0); //convert into rs to display  //v3.1.1.B
	    sprintf(acc_credit_array, "%3.2f", curnt_user_bal_rs); //(double)curnt_user_bal_rs);	  //v3.1.1.B
	    sprintf(lcd_data, "BL:%.6s%.2s T:%.2s", acc_credit_array, transcation_mode, temp_value);
	    //sprintf(lcd_data,"BL:%.9s %.2s",acc_credit_array,transcation_mode);
	    lcd_display(1);
	    sprintf(lcd_data, "TDS:%.4s D:%sL", tds_value, disp_buffer);
	    lcd_display(2);
	}
	buzzer(AUT03);
	timer2_sec_delay(logout_idle_time); //1.0.2

    }
    else{}
   
    if (mode_flags.op_coin_mode)
    { //v2.2.9  //1.0.2
        coin_flags.coinselect = FALSE; //30/05 //do_finalize                    
        mode_flags.op_coin_mode = 0;
        if (total_coin_count >= coin_collector_capacity)
        {
            if (coin_flags.coin_collector_alert == 0)
            {
                coin_flags.coin_collector_alert = 1;
                send_server_response(DEV_COIN_COLLECTOR_ERROR);
            }
            else if (coin_flags.coin_collector_alert == 1)
            {
                temp_int = (int) (coin_collector_capacity / 10); //10% of total coin count
                if ((total_coin_count >= (coin_collector_capacity + temp_int)))
                {
                    coin_flags.coin_collector_error = 1;
                    //peripheral_flags.coin_en_dis = 0;
                    coinbox_disable();
                    if (coin_flags.coinenable == FALSE) //v2.3.2
                    {
                    peripheral_flags.coin_en_dis = 0; //v2.3.2
                    }
                    eeprom_write(SER_COIN_EN_DIS);
                    eeprom_write(SER_COIN_COLLECTOR_ERROR);
                    send_server_response(DEV_COIN_COLLECTOR_ERROR);
                    strcpy(sms_sender_no, "AUTODISABL");
                    send_server_response(DEV_COIN_EN_DIS);
                }
            }
            else{}           
        }
    }
    else{}
    
    
    if((coin_flags.coin_disable_by_fs_error==TRUE) && (dispense_trail != 'E') && (dispense_litre>=0.1))      //v3.1.2.H
    {
        #ifdef DEBUG_TEST_ON              
           //         strcpy(lcd_data, "PLS WAIT..........");lcd_display(1);strcpy(lcd_data, "COIN ERR REMOVE");lcd_display(2);timer2_ms_delay(2000);
        #endif
       coin_flags.coin_disable_by_fs_error=0;  //v6  //remove sense       
               
    }else{}
        if ((peripheral_flags.coin_en_dis) && (system_error == ERROR_FREE)&&(coin_flags.coin_collector_error == 0)&&(coin_flags.coin_disable_by_fs_error==0)) //v2.2.9_coin&RFID coin enable, transtion by either coin or RFID		//add 4 coin box//1.0.2   //v6
        {             
            coinbox_enable();
        }else{
//               coin_flags.coin_disable_by_fs_error =1;
//               if((peripheral_flags.coin_en_dis==1)&& (coin_flags.coin_disable_by_fs_error==0))  //V6
//               {        
//                        coinbox_disable();
//                        if (coin_flags.coinenable == FALSE) //v2.3.2
//                        {        
//                        coin_flags.coin_disable_by_fs_error =1;
//                        }
                     
//               }else{}
        }

   

    SV_VALVE_OFF;
    if ((mode_flags.op_mode != ERROR_MODE) && (!school_mode_en) && (!corporate_mode_en))
	mode_flags.op_mode = IDLE_MODE;
    else{}
   
    memset(rfid_card, '\0', sizeof (rfid_card)); 
    memset(scanned_rfid_card, '\0', sizeof (scanned_rfid_card));
    memset(acc_credit_array, '\0', sizeof (acc_credit_array));

    acc_dispense_capacity = 0; //v2.2.4
    litre_transcation = 0; //v2.2.4
    Rfid_user_bal_paisa = 0; 
    curnt_user_bal_paisa = 0;
    curnt_user_bal_rs = 0;
    //for(temp_counter=0; temp_counter<(sizeof(curnt_user_bal_ary)); temp_counter++)  //2.3.B
    //	curnt_user_bal_ary[temp_counter] = 0;

    //---------v2.3.2

    consumer_flags.dispense_button = 0;
    consumer_flags.rfid_read = LOW;
    consumer_flags.card_detect = LOW; //v1.0.0
    rfid_card_type = 0;
    rfid_reader_counter = 0;
    dispense_litre = 0;
    dispense_trail = 0;
    peripheral_flags.tds_update = TRUE;
    system_flags.server_response = FALSE;
    peripheral_flags.lcd_refresh = TRUE; //v2.2.7

    if (consumer_flags.coin_read == HIGH)
    { //v2.3.2	- Coin Process complete after transaction over. //1.0.2
	consumer_flags.coin_read = LOW;
	UART1_init();
	//---timer2_ms_delay(10);
    }
    else{}  
       
  
    clear_lcd();
    LCD_BACKLITE_OFF;
    button_detect = 0;
    mode_flags.forced_function_state = FALSE;
    function_state = 0;
    consumer_flags.dispense_button = 0;
    LED4_OFF;
    peripheral_flags.lcd_refresh = TRUE;
    consumer_flags.dispense_error = 0;
    output_flow_count = 0; //v3.1.1.B
    temp_compare_flow_count = 0; //v3.1.1.B
    dummy_count = 0; //v3.1.1.B

}

void water_dispense_error(void)
{
    if (dispense_trail >= 2)
    {
        strcpy(lcd_data, "WATER DISPENSE");
        lcd_display(1);
        strcpy(lcd_data, "ISSUE");
        lcd_display(2);
        SV_VALVE_OFF;
        dispense_trail = 'E';
        buzzer(ERR01); 
        timer2_sec_delay(1); //3.1.2
    }
    else
    {
        dispense_trail++;
        SV_VALVE_OFF;       
        timer2_ms_delay(1500);
        SV_VALVE_ON;
        timer2_ms_delay(100);
    }

    if (dispense_trail > 3)
    {
        system_error = FLOW_SENS_ER;
        SV_VALVE_OFF;        
        if (system_error == FLOW_SENS_ER)
        {
            if ((peripheral_flags.lls_hls_en_dis >= 2)&&(peripheral_flags.lls_trigger == TRUE)&&(tank_low_level_litre <= 1))
            {
                system_error = TANK_EMPTY_ER;
                error_stack[TANK_EMPTY_ER] = TRUE;
                //---error_stack[FLOW_SENS_ER] = FALSE;
                error_flags.tank_empty = TRUE;
                send_server_response(DEV_TANK_EMPTY);
            }
                else
                {
                    error_stack[FLOW_SENS_ER] = TRUE;
                    send_server_response(DEV_FLOW_SENSOR_ERROR);
                }
	    //---send_server_response(DEV_HB_MSG);
        }
        else{}	

        eeprom_write(SER_ERROR_STACK);
        system_error_check();
        display_error_mode();
        buzzer(ERR01);
        send_server_response(DEV_HB_MSG); //v2.2.2
//        no_dispense_error_count = 0;  //v3.1.2
        dispense_trail = 'E'; 
        coin_flags.coin_disable_by_fs_error=1;  //v3.1.2.H
    }
    else{}   
    //return dispense_trail;
    peripheral_flags.lcd_refresh = TRUE;
}

void daily_dispense_data()
{
    double pre_total_dispense = 0;
    FSchdir("\\");
    FSchdir("TOTAL");
    memset(file_name, '\0', sizeof (file_name));
    memset(daily_total_lit_ary, '\0', sizeof (daily_total_lit_ary));
    sprintf(file_name, "%.8s.txt", "Cur_date"); //     //v3.1.1.D
    FSfclose(sd_pointer); //@ f_close(&fil);
    sd_pointer = FSfopen(file_name, FS_READPLUS);
    if (FSfread(daily_total_lit_ary, 1, 10, sd_pointer) != 10)
    {}
    else{}  
    FSfclose(sd_pointer);
    daily_total_lit = atol(daily_total_lit_ary);
    daily_total_dispense = (double) daily_total_lit / 100.0;

    sprintf(lcd_data, "CUR =%4.1f Lt.", daily_total_dispense);
    lcd_display(1);

    memset(daily_total_lit_ary, '\0', sizeof (daily_total_lit_ary));
    memset(file_name, '\0', sizeof (file_name));
    sprintf(file_name, "%.8s.txt", "pre_date"); //v3.1.1.D
    FSfclose(sd_pointer); //@ f_close(&fil);
    sd_pointer = FSfopen(file_name, FS_READPLUS);
    if (FSfread(daily_total_lit_ary, 1, 10, sd_pointer) != 10)
    {}
    else{}
    FSfclose(sd_pointer);
    FSchdir("\\");
    daily_total_lit = atol(daily_total_lit_ary);
    pre_total_dispense = (double) daily_total_lit / 100.0;
    sprintf(lcd_data, "PREV=%4.1f Lt.", pre_total_dispense);
    lcd_display(2);   
    timer2_sec_delay(5);
}

void file_create()
{
    FSmkdir(".\\Trans"); 	
    FSchdir("Trans");
    sd_pointer = FSfopen("SYNC.txt", FS_APPENDPLUS);
    FSfclose(sd_pointer); 
    FSchdir("\\");
    FSmkdir(".\\TOTAL"); 			
    FSchdir("\\"); 		
    FSchdir("TOTAL"); 		 
    sd_pointer = FSfopen("Cur_date.txt", FS_APPENDPLUS); 		
    FSfclose(sd_pointer);
    sd_pointer = FSfopen("pre_date.txt", FS_APPENDPLUS); 	
    FSfclose(sd_pointer);
    FSchdir("\\");
}

void all_log_store_sdcard(unsigned char change_date_flag)
{
    if ((sd_flags.card_initialize) && (sd_flags.card_detect))//v3.1.1.D  //v3.1.1.H
    {
	get_local_time();
	//		lcd_init();		
	//		sprintf(lcd_data,"%.2d/%.2d/%.2d [%.2d/%.2d]",sd_date,sd_month,sd_yr,P_date,P_month);
	//		lcd_display(1);	
	//		sprintf(lcd_data,"%.2d:%.2d:%.2d [%.2d]",sd_hour,sd_min,sd_sec,P_yr);
	//		lcd_display(2);
	//		timer2_ms_delay(2000);	
	//	    read_eeprom_parameter(SER_PREV_MONTH);
	//	    read_eeprom_parameter(SER_PREV_YEAR);
	read_eeprom_parameter(SER_PREV_DATE);
	FSchdir("\\");
	FSchdir("Trans");

	if (sd_date != P_date)
	{
	    read_eeprom_parameter(SER_PREV_MONTH);
	    read_eeprom_parameter(SER_PREV_YEAR);
	    last_date_backup = P_date; //update3
	    last_month_backup = P_month; //update3
	    last_year_backup = P_yr; //update3		
	    P_date = sd_date;
	    eeprom_write(SER_PREV_DATE);
	    FSfclose(sd_pointer);
	    sd_pointer = FSfopen("SYNC.txt", FS_APPEND);
	    FSfseek(sd_pointer, 1, SEEK_END);
	    memset(file_name, '\0', sizeof (file_name));
	    sprintf(file_name, "#%.2d_%.2d_%.2d.txt;PEN", sd_yr, sd_month, sd_date);
	    FSfprintf(sd_pointer, "%s\r\n^", file_name);
	    FSfclose(sd_pointer);
	    if ((last_date_backup != 0) || (last_month_backup != 0))
	    {
		rtc_flag.date_change = 1;
		mode_flags.forced_function_state = TRUE; //0.0.1
		function_state = ST_DATE_CHANGE; //0.0.1
	    } else{}
	 
	    //dispense_water_data_backup();
	    //FSchdir("\\");
	    //FSchdir ("Trans");

	    if (sd_month != P_month)
	    {
		last_month_backup = P_month; //update3			
		P_month = sd_month;
		eeprom_write(SER_PREV_MONTH);
		read_eeprom_parameter(SER_PREV_YEAR);
		if (last_month_backup != 0)
		{
		    rtc_flag.month_change = 1;
		} //month change indication flat for remove Sync file
		if (sd_yr != P_yr)
		{
		    P_yr = sd_yr; //update3	
		    eeprom_write(SER_PREV_YEAR);
		}
		else{}
		
		memset(file_name, '\0', sizeof (file_name));
		sprintf(file_name, "20%.2d_%.2d", sd_yr, sd_month);
		FSmkdir(file_name);
		FSchdir(file_name);
	    }
	    else
	    {
		memset(file_name, '\0', sizeof (file_name));
		sprintf(file_name, "20%.2d_%.2d", sd_yr, sd_month);
		if (!FSchdir(file_name))
		{
		}
		else
		{
		    strcpy(lcd_data, "FILE NOT GET   1"); //MONTH NOT FIND v3.1.1.F
		    lcd_display(1);
		    timer2_sec_delay(error_display_time);
		    FSmkdir(file_name);
		    FSchdir(file_name);
		}
	    }
	}
	else
	{
	    memset(file_name, '\0', sizeof (file_name));
	    sprintf(file_name, "20%.2d_%.2d", sd_yr, sd_month);
	    if (!FSchdir(file_name))
	    {
	    }
	    else
	    {
		strcpy(lcd_data, "FILE NOT GET   2");
		lcd_display(1);
		timer2_sec_delay(error_display_time);
		FSmkdir(file_name);
		FSchdir(file_name);
	    }
	}
	memset(SD_Buffer, '\0', sizeof (SD_Buffer));
	memset(file_name, '\0', sizeof (file_name));

	if (change_date_flag == 1)
	{
	    if (last_month_backup != sd_month)
	    {
		FSchdir("\\");
		FSchdir("Trans");
		sprintf(file_name, "20%.2d_%.2d", last_year_backup, last_month_backup);
		if (!FSchdir(file_name))
		{
//			strcpy(lcd_data,"CHNG MONTH 3 Ok");lcd_display(1);	timer2_sec_delay(error_display_time);
		}
		else
		{
		    FSmkdir(file_name);
		    FSchdir(file_name);
		    strcpy(lcd_data, "P. FILE NOT GET"); //P. MONTH NOT GET v3.1.1.F
		    lcd_display(1);
		    timer2_sec_delay(error_display_time);
		}
	    }
	    else{}
	  
	    memset(file_name, '\0', sizeof (file_name));
	    sprintf(file_name, "%.2d_%.2d_%.2d.txt", last_year_backup, last_month_backup, last_date_backup);
	    sprintf(SD_Buffer, "%.4d-%.2d-%.2d %.2d:%.2d:%.2d/%s", (2000 + last_year_backup), last_month_backup, last_date_backup, 23, 59, 59, transmitData); //23,59,50 //v3.1.1.F
	    //strcpy(lcd_data,file_name);lcd_display(1);
	}
	else
	{
	    memset(file_name, '\0', sizeof (file_name));
	    sprintf(file_name, "%.2d_%.2d_%.2d.txt", sd_yr, sd_month, sd_date);
	    sprintf(SD_Buffer, "%.4d-%.2d-%.2d %.2d:%.2d:%.2d/%s", sd_year, sd_month, sd_date, sd_hour, sd_min, sd_sec, transmitData);
	    //strcpy(lcd_data,file_name);lcd_display(2);
	}
	FSfclose(sd_pointer);
	sd_pointer = FSfopen(file_name, FS_APPEND);
	FSfseek(sd_pointer, 1, SEEK_END);
	FSfprintf(sd_pointer, "%s\r\n^", SD_Buffer);
	FSfclose(sd_pointer);
	FSchdir("\\");
	timer2_ms_delay(10);
	memset(SD_Buffer, '\0', sizeof (SD_Buffer)); //v3.1.1.B	
	//for(cntr_SD=0; cntr_SD<sizeof(SD_Buffer); cntr_SD++)  //v3.1.1.B
	//	SD_Buffer[cntr_SD] = 0;
    }
    else
    {
	LED2_ON;
	buzzer(ERR02);
	strcpy(lcd_data, "MEMORY FAULT    "); //v3.1.1.D
	lcd_display(1);
	strcpy(lcd_data, "CONTACT SARVAJAL"); //v3.1.1.D
	lcd_display(2);
	timer2_sec_delay(error_display_time); //v3.1.1.D
    }

}

void ultra_super_admin_mode()
{
    if (consumer_flags.card_detect == HIGH)
    {
	buzzer(AUT01);
	strcpy(lcd_data, "SUPER ADMIN MODE");
	lcd_display(1);
	strcpy(lcd_data, "ENABLE..........");
	lcd_display(2);
	timer2_ms_delay(2000);
	strcpy(lcd_data, "PLEASE TRY AFTER");
	lcd_display(1);
	strcpy(lcd_data, "SOME TIME.......");
	lcd_display(2);
	timer2_ms_delay(2000);
	//	clear_lcd();
	LCD_BACKLITE_OFF;

	consumer_flags.card_detect = LOW;
    }
    else{}
   
    if (scan_sms_memory())
    {	
	check_sms();
	timer2_ms_delay(550);
    }
    else{}   
    if ((sec_tick % 60) == 0)
    {	
	strcpy(lcd_data, "PLS. WAIT...");
	lcd_display(1);
	check_sms();	
    }
    else{}
}

void new_franchise_menu()
{
    //lcd_init();			
    button_detect = 0;
    button_timeout = sec_tick;
    display_flags.display_toggle = 0; //v3.1.1.D
    display_toggle_time_stamp = 0; //v3.1.1.D
    while ((button_detect != ENTER_DETECT)&&(button_detect != MENU_DETECT)&&((sec_tick - button_timeout) < 20))
    {
	scan_switch();
	if (((timer2_tick - display_toggle_time_stamp) >= FIRST_FRAME_DELAY) && (display_flags.display_toggle == 0))
	{
	    display_flags.display_toggle = 1;
	    display_flags.display_mode = 1;
	    display_toggle_time_stamp = timer2_tick;
	}
	else{}
	
	if (((timer2_tick - display_toggle_time_stamp) >= SECOND_FRAME_DELAY) && (display_flags.display_toggle == 1))
	{

	    display_flags.display_toggle = 0;
	    display_flags.display_mode = 1;
	    display_toggle_time_stamp = timer2_tick;
	}
	else{}
	
	if ((display_flags.display_toggle == 1) && (display_flags.display_mode))
	{
	    clear_lcd();
	    strcpy(lcd_data, "WELCOME TO      ");
	    lcd_display(1);
	    strcpy(lcd_data, "MASTER MENU  "); //3.1.3.A
	    lcd_display(2);
	    //	LCD_BACKLITE_OFF;
	    display_flags.display_mode = 0;
	}
	else if ((display_flags.display_toggle == 0) && (display_flags.display_mode))
	{
	    clear_lcd();
	    strcpy(lcd_data, "PRESS ENTER TO  ");
	    lcd_display(1);
	    strcpy(lcd_data, "PROCEED      ");
	    lcd_display(2);
	    display_flags.display_mode = 0;
	}
	else{}	
    }
    if (button_detect == ENTER_DETECT)
    {
	button_detect = 0;
	//	LCD_BACKLITE_OFF;
	timer2_ms_delay(500);
	if (password() == 1)
	{
	    menu_scrolling_function();
	    button_detect = 0;
	}
	else
	{
	    if (school_mode_en)
		mode_flags.op_mode = SCHOOL_MODE;
	    else if (corporate_mode_en) //v2.2.5
		mode_flags.op_mode = CORPORATE_MODE;
	    else
		mode_flags.op_mode = IDLE_MODE;
	}
	//LCD_BACKLITE_OFF;
    }
    else
    {
	if (school_mode_en)
	    mode_flags.op_mode = SCHOOL_MODE;
	else if (corporate_mode_en) //v2.2.5
	    mode_flags.op_mode = CORPORATE_MODE;
	else
	    mode_flags.op_mode = IDLE_MODE;

	strcpy(lcd_data, "MENU EXIT       ");
	lcd_display(1);
	strcpy(lcd_data, "PLEASE WAIT....."); /// strcpy(lcd_data,"PIRAMAL SARVAJAL");
	lcd_display(2);
	buzzer(AUT03);
	timer2_sec_delay(1);
	//	LCD_BACKLITE_OFF;  //1.0.1
	//	clear_lcd();     //1.0.1
    }
    button_detect = 0;

    peripheral_flags.lcd_refresh = TRUE;
    //function_state=0;
}

unsigned char password()
{
    lcd_init();
    strcpy(lcd_data, "ENTER PASSWORD");
    lcd_display(1);
    strcpy(lcd_data, "0000");
    lcd_display(2);
    memset(menu_current_password, '0', sizeof (menu_current_password)); //null si required to send in function argument
    button_detect = 0;
    menu_flags.admin_access = 0;
    //	peripheral_flags.user_enter_password=0;
    //	while(!peripheral_flags.user_enter_password){
    while (button_detect != MENU_DETECT)
    {
	menu_current_password[0] = user_value(2, 1, menu_current_password[0], 9);
	if (button_detect == MENU_DETECT)
	{
	    break;
	}
	else if (button_detect == NO_DETECT)
	{
	    break;
	}
	else
	{
	    button_detect = 0;
	}
	menu_current_password[1] = user_value(2, 2, menu_current_password[1], 9);
	if (button_detect == MENU_DETECT)
	{
	    break;
	}
	else if (button_detect == NO_DETECT)
	{
	    break;
	}
	else
	{
	    button_detect = 0;
	}
	menu_current_password[2] = user_value(2, 3, menu_current_password[2], 9);
	if (button_detect == MENU_DETECT)
	{
	    break;
	}
	else if (button_detect == NO_DETECT)
	{
	    break;
	}
	else
	{
	    button_detect = 0;
	}
	menu_current_password[3] = user_value(2, 4, menu_current_password[3], 9);
	if (button_detect == MENU_DETECT)
	{
	    break;
	}
	else if (button_detect == NO_DETECT)
	{
	    break;
	}
	else
	{
	    button_detect = 0;
	}
    }
    menu_current_password[4] = '\0';
    if (button_detect == NO_DETECT)
    {
	strcpy(lcd_data, "USER ENTER      ");
	lcd_display(1);
	strcpy(lcd_data, "TIMEOUT         ");
	lcd_display(2);
	button_detect = 0;
	timer2_sec_delay(error_display_time);
	return 0;
    }
    else{}
   
    button_detect = 0;   
    strcpy(lcd_data, "VALIDATING");
    lcd_display(1);
    strcpy(lcd_data, "PLS WAIT......");
    lcd_display(2);
    if (strncmp(menu_current_password, menu_franchisee_password, 4) == 0)
    {	timer2_sec_delay(error_display_time);
	clear_lcd();
	menu_flags.admin_access = 0;
	return 1;
    }
    else{}
   
    if (strncmp(menu_current_password, "2016", 4) == 0)
    {
	//	strcpy(lcd_data,"ADMIN");lcd_display(1);
	//	strcpy(lcd_data,"PLS WAIT......");lcd_display(2);timer2_sec_delay(error_display_time);clear_lcd();
	timer2_sec_delay(error_display_time);
	clear_lcd();
	menu_flags.admin_access = 1;
	return 1;
    }
    else
    {
	strcpy(lcd_data, "INVALID PASSWORD");
	lcd_display(1);
	strcpy(lcd_data, "TRY AGAIN.......");
	lcd_display(2);
	timer2_ms_delay(2000); //clear_lcd();
	return 0;
    }
}

unsigned char user_value(unsigned lcd_line, unsigned char lcd_location, unsigned char previous_value, unsigned char user_max_value)
{

    unsigned char cntr = 0, pre_cntr = 1;
    cntr = (previous_value - 0x30);
    lcd_byte_display(lcd_line, lcd_location, cntr + 0x30);
    button_timeout = sec_tick;
    while ((button_detect != ENTER_DETECT)&&(button_detect != MENU_DETECT))
    {
	scan_switch();
	if (button_detect == DOWN_DETECT)
	{
	    timer2_ms_delay(5);
	    button_timeout = sec_tick; //1.0.1 
	    //pre_cntr = cntr;
	    button_detect = 0;
	    if (cntr == 0)
		cntr = user_max_value;
	    else
		cntr--;
	}
	else if (button_detect == UP_DETECT)
	{
	    timer2_ms_delay(5);
	    button_timeout = sec_tick; //1.0.1
	    //pre_cntr = cntr;
	    button_detect = 0;
	    if (cntr == user_max_value)
		cntr = 0;
	    else
		cntr++;
	}
	else{}
	if (pre_cntr != cntr)
	{
	    pre_cntr = cntr;
	    lcd_byte_display(lcd_line, lcd_location, cntr + 0x30);
	}
	else{}
	
	if ((sec_tick - button_timeout) > 30)
	{
	    button_detect = NO_DETECT;
	    break;
	}
	else{}	
    }
    return (cntr + 0x30);
}

void menu_scrolling_function()
{
    unsigned char menu_pos = 1, prev_menu_pos = 0;

    if (menu_flags.admin_access == 1)
	menu_pos = ADMIN_START_POS;
    else
	menu_pos = FRANCHISEE_START_POS;
    menu_flags.exit = 0;
    menu_flags.function_access = 0;  //v3.1.1.D
    button_detect = 0;
    while (!menu_flags.exit)
    {
	scan_switch();
	if (button_detect == ENTER_DETECT)
	{
	    button_detect = 0;
        
        // ----------- 
        switch(menu_pos)
        {
            case ADD_BALANCE_MENU  :
                  add_balance();      
            break;  
            
            case ADD_CARD_MENU  :
                  add_new_card(); 
            break;   
            
            case CARD_RECH_BAL_MENU :
                  sensor_calibration_menu("CARD INIT BAL   ", SER_CARD_INIT_BAL);      //BAL. to BAL            
            break;  
            
            case REMOVE_CARD_MENU   : 
                  remove_card();                  
            break;
            
            case DAILY_DISP_MENU :    
                  daily_dispense_data();            
            break;
            
            case DUMP_MODE_MENU  :   
                 dump_mode_setting();                     
            break;
            
            case REFILL_MODE_MENU :       
                 refill_mode_setting();              
            break;
            
            case SCHOOL_MODE_MENU :      
                  school_mode_setting();       
            break;
            
            case CORPORATE_MODE_MENU :   
                    corporate_mode_setting();   
            break;
            
            case REQ_MAINTANANCE_MENU : 
                    maintenance_request();                       
            break;
            
            case DEVICE_INFO_MENU  :     
                    device_information();                      
            break;
            
            case CALIBRATION_MENU  : 
                calibration_mode();       
            break;
            
            case FLOW_ERROR_REMOVE_MENU  :  
                flow_error_remove();
                    break;
            case EXIT_FRANCHISEE_MENU : 
                    if (exit_menu("EXIT MENU       "))
                    {
                        menu_flags.exit = 1;
                        clear_lcd();
                    } //exit from menu  //3.1.0
                    else
                    {
                        menu_pos = FRANCHISEE_START_POS;
                        prev_menu_pos = 0;
                    } //retunr to home positon		
                    break;
            case OUTFLOW_CALIBRATION_MENU  : 
                    sensor_calibration_menu("OUTFLOW CALI.   ", SER_OUT_FLOW_CALIB);                
            break;
            
            case TDS_CALIBRATION_MENU  :       
                   sensor_calibration_menu("TDS CALI.       ", SER_TDS_CALIB);         
            break;
            
            case RTC_MANUAL_SET_MENU   : 
                    manual_RTC_set_time();               
            break;
            
            case EXIT_ADMIN_MENU   :        
        
                if (exit_menu("EXIT MENU       "))
                {
                    menu_flags.exit = 1;
                    clear_lcd();
                } //exit from menu  //3.1.0
                else
                {
                    menu_pos = ADMIN_START_POS;
                    prev_menu_pos = 0;
                } //retunr to home positon	
                break;
            defailt: 
            break;
        } 
	    button_detect = 0;
	    prev_menu_pos = 0;

	}
	else if (button_detect == UP_DETECT)
	{
	    button_detect = 0;
	    menu_pos--;
	    if ((menu_pos < ADMIN_START_POS)&&(menu_flags.admin_access == 1))
	    {
            menu_pos = ADMIN_LAST_POS;
	    } //change here if add another list
	    else if ((menu_pos < FRANCHISEE_START_POS)&&(menu_flags.admin_access == 0))
	    {
            menu_pos = FRANCHISEE_LAST_POS;
	    } //change here if add another list
	    else{}
	    
	}
	else if (button_detect == DOWN_DETECT)
	{
	    button_detect = 0;
	    menu_pos++;
	    if ((menu_pos > ADMIN_LAST_POS)&&(menu_flags.admin_access == 1))
	    {
		menu_pos = ADMIN_START_POS;
	    } //change here if add another list
	    else if ((menu_pos > FRANCHISEE_LAST_POS)&&(menu_flags.admin_access == 0))
	    {
		menu_pos = FRANCHISEE_START_POS;
	    } //change here if add another list
	    else{}
	    
	}
	else{}
	
	if ((menu_pos != prev_menu_pos) &&(menu_flags.exit == 0))
	{
	    general_time_stamp_logout = sec_tick; //1.0.1
	    menu_flags.timeout_menu_options = FALSE; //1.0.1
	    switch (menu_pos)
	    {
	    case ADD_BALANCE_MENU:  
            strcpy(lcd_data, "1.ADD BALANCE   ");
            lcd_display(1);
            strcpy(lcd_data, "2.ADD CARD      ");
            lcd_display(2);
            prev_menu_pos = menu_pos;
		break;
        
	    case ADD_CARD_MENU:
            strcpy(lcd_data, "2.ADD CARD      ");
            lcd_display(1);
            strcpy(lcd_data, "3.CARD INIT BAL "); //RECH TO INIT //3.1.3.B
            lcd_display(2);
            prev_menu_pos = menu_pos;
		break;
        
	    case CARD_RECH_BAL_MENU:
            strcpy(lcd_data, "3.CARD INIT BAL "); //RECH TO INIT //3.1.3.B
            lcd_display(1);
            strcpy(lcd_data, "4.REMOVE CARD   ");
            lcd_display(2);
            prev_menu_pos = menu_pos;
		break;
        
	    case REMOVE_CARD_MENU:
            strcpy(lcd_data, "4.REMOVE CARD   ");
            lcd_display(1);
            strcpy(lcd_data, "5.DAILY DISP.   ");
            lcd_display(2);
            prev_menu_pos = menu_pos;
		break;
        
	    case DAILY_DISP_MENU:
            strcpy(lcd_data, "5.DAILY DISP.   ");
            lcd_display(1);
            strcpy(lcd_data, "6.DUMP MODE     ");
            lcd_display(2);
            prev_menu_pos = menu_pos;
		break;
        
	    case DUMP_MODE_MENU:
            strcpy(lcd_data, "6.DUMP MODE     ");
            lcd_display(1);
            strcpy(lcd_data, "7.REFILL MODE   ");
            lcd_display(2);
            prev_menu_pos = menu_pos;
		break;
        
	    case REFILL_MODE_MENU:
            strcpy(lcd_data, "7.REFILL MODE   ");
            lcd_display(1);
            strcpy(lcd_data, "8.SCHOOL MODE   ");
            lcd_display(2);
            prev_menu_pos = menu_pos;
		break;
        
	    case SCHOOL_MODE_MENU:
            strcpy(lcd_data, "8.SCHOOL MODE   ");
            lcd_display(1);
            strcpy(lcd_data, "9.CORPORATE MODE");
            lcd_display(2);
            prev_menu_pos = menu_pos;
		break;
        
	    case CORPORATE_MODE_MENU:
            strcpy(lcd_data, "9.CORPORATE MODE");
            lcd_display(1);
            strcpy(lcd_data, "10.REQ. MAINT.   ");
            lcd_display(2);
            prev_menu_pos = menu_pos;
		break;
        
	    case REQ_MAINTANANCE_MENU:
            strcpy(lcd_data, "10.REQ. MAINT.   ");
            lcd_display(1);
            strcpy(lcd_data, "11.DEVICE INFO. ");
            lcd_display(2);
            prev_menu_pos = menu_pos;
		break;
        
	    case DEVICE_INFO_MENU:
            strcpy(lcd_data, "11.DEVICE INFO. ");
            lcd_display(1);
            strcpy(lcd_data, "12.CALI MODE    ");
            lcd_display(2);
            prev_menu_pos = menu_pos;
		break;
        
	    case CALIBRATION_MENU:
            strcpy(lcd_data, "12.CALI MODE    ");
            lcd_display(1);
            strcpy(lcd_data, "13.FLOW ERR. REM");
            lcd_display(2);
            prev_menu_pos = menu_pos;
		break;
        
        case FLOW_ERROR_REMOVE_MENU:
            strcpy(lcd_data, "13.FLOW ERR. REM");
            lcd_display(1);
            strcpy(lcd_data, "14.EXIT         ");
            lcd_display(2);
            prev_menu_pos = menu_pos;
		break;
        
	    case EXIT_FRANCHISEE_MENU: ////change here if add another list	
            strcpy(lcd_data, "14.EXIT         ");
            lcd_display(1);
            strcpy(lcd_data, "1.ADD BALANCE   ");
            lcd_display(2);
            prev_menu_pos = menu_pos;
		break;
        
	    case OUTFLOW_CALIBRATION_MENU: ////change here if add another list
            prev_menu_pos = menu_pos;
            if (menu_flags.admin_access == 1)
            {
                strcpy(lcd_data, "1.OUTFLOW CALI. ");
                lcd_display(1);
                strcpy(lcd_data, "2.TDS CALI.     ");
                lcd_display(2);
            }
            else{}
		break;
        
	    case TDS_CALIBRATION_MENU: ////change here if add another list	
            prev_menu_pos = menu_pos;
            if (menu_flags.admin_access == 1)
            {
                strcpy(lcd_data, "2.TDS CALI.   ");
                lcd_display(1);
                strcpy(lcd_data, "3.RTC SET TIME ");
                lcd_display(2);
            }
            else{}		
		break;
        
	    case RTC_MANUAL_SET_MENU: ////change here if add another list	
            prev_menu_pos = menu_pos;
            if (menu_flags.admin_access == 1)
            {
                strcpy(lcd_data, "3.RTC SET TIME ");
                lcd_display(1);
                strcpy(lcd_data, "4.EXIT         ");
                lcd_display(2);
            }
            else{}		
		break;

	    case EXIT_ADMIN_MENU: ////change here if add another list	
            prev_menu_pos = menu_pos;
            if (menu_flags.admin_access == 1)
            {
                strcpy(lcd_data, "4.EXIT         ");
                lcd_display(1);
                strcpy(lcd_data, "1.OUTFLOW CALI. ");
                lcd_display(2);
            }
            else{}
		break;

	    default:
		break;

	    }
	    lcd_set_cursor(1, 1, 15);
	}
	else
	{
	}

	if ((sec_tick - general_time_stamp_logout)>(user_timeout * 1)) //timeout condition: 30sec * 1=30sec
	{
	    menu_flags.timeout_menu_options = TRUE;
	    menu_flags.exit = TRUE;
	    break;
	}
    } //while close	
    button_detect = 0;

    if (menu_flags.timeout_menu_options == TRUE)
    {
	lcd_init();
	strcpy(lcd_data, "AUTO LOG OFF");
	lcd_display(1);
	strcpy(lcd_data, "RESCAN CARD");
	lcd_display(2);
	buzzer(AUT03);
	timer2_sec_delay(error_display_time);

	if (school_mode_en)
	    mode_flags.op_mode = SCHOOL_MODE;
	else if (corporate_mode_en) //v2.2.5
	    mode_flags.op_mode = CORPORATE_MODE;
	else if (calibration_mode_en)
	    mode_flags.op_mode = CALIBRATION_MODE;
	else
	    mode_flags.op_mode = IDLE_MODE;
    }
    else{}
    if (menu_flags.function_access != TRUE)
    {
	menu_flags.function_access = 0;
	if (school_mode_en)
	    mode_flags.op_mode = SCHOOL_MODE;
	else if (corporate_mode_en) //v2.2.5
	    mode_flags.op_mode = CORPORATE_MODE;
	else
	    mode_flags.op_mode = IDLE_MODE;
    }
    else{}
    if ((mode_flags.op_mode == DUMP_MODE) || (mode_flags.op_mode == REFILL_MODE) || (menu_flags.timeout_menu_options == TRUE))
    {
    }
    else
    {        
	lcd_init();
	strcpy(lcd_data, "MENU EXIT       "); //v2.2.7  	strcpy(lcd_data,"   Powered by");		//v2.2.7
	lcd_display(1);
	strcpy(lcd_data, "PLEASE WAIT....."); /// strcpy(lcd_data,"PIRAMAL SARVAJAL");
	lcd_display(2);
	buzzer(AUT03);
        if((system_error==ERROR_FREE)&&(peripheral_flags.coin_en_dis==1)&&(peripheral_flags.dev_en_dis==TRUE)&&(coin_flags.coin_collector_error==0)&&(mode_flags.op_mode!=DEBUG_MODE)&&(mode_flags.op_mode!=DIAGNOSTIC_MODE)&&(mode_flags.op_mode!=ANTENNA_MODE)&&(mode_flags.op_mode!=SCHOOL_MODE)&&(mode_flags.op_mode!=CALIBRATION_MODE)&&(mode_flags.op_mode!=CORPORATE_MODE)&&(mode_flags.op_mode!=DUMP_MODE)&&(mode_flags.op_mode!=REFILL_MODE))	//v2.3.4
            coinbox_enable();
            else{}
	timer2_sec_delay(logout_idle_time);
	//LCD_BACKLITE_OFF;
    }
    peripheral_flags.lcd_refresh = TRUE;
}

unsigned char exit_menu(unsigned char str_1[])
{
    unsigned char process_decision = 0;
    button_detect = 0;
    strncpy(lcd_data, str_1, 16);
    lcd_display(1);
    strcpy(lcd_data, "PROCEED: Y / N ");
    lcd_display(2);
    lcd_byte_display(2, 10, 'Y'); //by defualt it will be position on yes and return 1
    process_decision = 1;
    button_timeout = sec_tick;
    while ((button_detect != ENTER_DETECT))
    {
	if ((sec_tick - button_timeout) > 30)
	{
	    process_decision = 0;
	    break;
	} //1.0.1  //if timeout then retun 0
	else{}
	scan_switch();
	if (button_detect == DOWN_DETECT)
	{
	    process_decision = 1;
	    strncpy(lcd_data, str_1, 16);
	    lcd_display(1);
	    strcpy(lcd_data, "PROCEED: Y / N ");
	    lcd_display(2);
	    lcd_byte_display(2, 10, 'Y');
	    button_detect = 0;
	}
	else if (button_detect == UP_DETECT)
	{
	    process_decision = 0;
	    strncpy(lcd_data, str_1, 16);
	    lcd_display(1);
	    strcpy(lcd_data, "PROCEED: Y / N ");
	    lcd_display(2);
	    lcd_byte_display(2, 14, 'N');
	    button_detect = 0;
	}
	else{}
    }
    return process_decision;
}

void dump_mode_setting()
{
    lcd_init();
    strcpy(lcd_data, "DUMP MODE");
    lcd_display(1);
    sprintf(lcd_data, "ENABLE PLS. WAIT");
    lcd_display(2);
    timer2_sec_delay(error_display_time);
    mode_flags.op_mode = DUMP_MODE;
    mode_flags.forced_function_state = TRUE;
    function_state = ST_OP_MODE;
    system_flags.server_response = SERVER_RESPONSE_OK;
    menu_flags.exit = TRUE;
    menu_flags.function_access = TRUE;
    menu_flags.any_button_press = 0;
    consumer_flags.dispense_button = 0;
}

void refill_mode_setting()
{
    lcd_init();
    strcpy(lcd_data, "REFILL MODE");
    lcd_display(1);
    sprintf(lcd_data, "ENABLE PLS. WAIT");
    lcd_display(2);
    timer2_sec_delay(error_display_time);
    mode_flags.op_mode = REFILL_MODE;
    mode_flags.forced_function_state = TRUE;
    function_state = ST_OP_MODE;
    menu_flags.exit = TRUE;
    menu_flags.function_access = TRUE;
    menu_flags.any_button_press = 0;
    consumer_flags.dispense_button = 0;

}

void school_mode_setting()
{
    char* temp_ptr_1;
    char* temp_ptr_2;
    read_eeprom_parameter(SER_SCHOOL_MODE_ACC_ID);

    if (school_mode_en == 1)
    {
	strcpy(lcd_data, "SCHOOL MODE ON   ");
	lcd_display(1);
	strcpy(lcd_data, "DOWN=OFF / UP=ON");
	lcd_display(2);
	lcd_byte_display(2, 15, 'O');
    }
    else
    {
	strcpy(lcd_data, "SCHOOL MODE OFF ");
	lcd_display(1);
	strcpy(lcd_data, "DOWN=OFF / UP=ON");
	lcd_display(2);
	lcd_byte_display(2, 6, 'O');
    }
    //timer2_sec_delay(1);	

    temp_ptr_1 = 0;
    temp_ptr_2 = 0;

    temp_ptr_1 = strstr(sch_mode_acc_id, "NULL"); //
//    temp_ptr_2 = strstr(sch_mode_acc_id, "0000");  //v3.1.1
    temp_ptr_2 = strstr(sch_mode_acc_id, "00000000");   //v3.1.2.G

    if ((temp_ptr_1 != 0) || (temp_ptr_2 != 0))
    {
	lcd_init();
	strcpy(lcd_data, "NO ACCOUNT ID");
	lcd_display(1);
	strcpy(lcd_data, "CONTACT SARVAJAL");
	lcd_display(2);

	buzzer(ERR01);
	timer2_sec_delay(error_display_time);
    }
    else
    {
	buzzer(AUT01);
	general_time_stamp_logout = sec_tick; //1.0.1			
	button_detect = 0;
	while (button_detect != ENTER_DETECT)
	{
	    scan_switch();
	    if (button_detect == UP_DETECT)
	    {
		school_mode_en = 1;
		corporate_mode_en = 0;
		//@				mode_flags.op_mode = SCHOOL_MODE;			
		strcpy(lcd_data, "SCHOOL MODE ON");
		lcd_display(1);
		strcpy(lcd_data, "DOWN=OFF / UP=ON");
		lcd_display(2);
		lcd_byte_display(2, 15, 'O');
		button_detect = 0;
		//break;
	    }
	    else if (button_detect == DOWN_DETECT)
	    {

		school_mode_en = 0;		
		strcpy(lcd_data, "SCHOOL MODE OFF");
		lcd_display(1);
		strcpy(lcd_data, "DOWN=OFF / UP=ON");
		lcd_display(2);
		lcd_byte_display(2, 6, 'O');
		button_detect = 0;
		//break;
	    }
	    else if (button_detect == ENTER_DETECT)
	    {
		//strcpy(lcd_data,"SYSTEM EXIT ");  //17/05/17
		//lcd_display(1);
		break;
	    }
	    else{}
	   
	    if ((sec_tick - general_time_stamp_logout)>(user_timeout * 1))
	    {
		menu_flags.timeout_menu_options = TRUE;
		menu_flags.exit = TRUE;
		break;
	    }
	    else{}	  
	}
	buzzer(AUT01);
	if (school_mode_en)
        {
	    read_eeprom_parameter(SER_SCHOOL_DISPENSE_AMT);  //3.1.2.I
            mode_flags.op_mode = SCHOOL_MODE;            
        }
	else if (corporate_mode_en) //v2.2.5
	    mode_flags.op_mode = CORPORATE_MODE;
	else
	    mode_flags.op_mode = IDLE_MODE;

	strcpy(lcd_data, "PLEASE WAIT...");
	lcd_display(2);
	eeprom_write(SER_SCHOOL_MODE);
	eeprom_write(SER_CORPORATE_MODE);
	eeprom_write(SER_OP_MODE);

	send_server_response(DEV_CORPORATE_MODE);
	send_server_response(DEV_SCHOOL_MODE);
	send_server_response(DEV_SCHOOL_MODE_ACC_ID);
	buzzer(AUT03);
	send_server_response(DEV_FRANCHISEE_RFID);
	strcpy(lcd_data, "SUCCESSFULLY...");
	lcd_display(2);
	timer2_sec_delay(error_display_time);
    }

    menu_flags.exit = TRUE;
    menu_flags.function_access = TRUE;

}

void corporate_mode_setting()
{
    char* temp_ptr_3;
    char* temp_ptr_4;

    read_eeprom_parameter(SER_CORPORATE_MODE_ACC_ID);
    if (corporate_mode_en == 1)
    {
	strcpy(lcd_data, "CORP. MODE ON");
	lcd_display(1);
	strcpy(lcd_data, "DOWN=OFF / UP=ON");
	lcd_display(2);
	lcd_byte_display(2, 15, 'O');
    }
    else
    {
	strcpy(lcd_data, "CORP. MODE OFF");
	lcd_display(1);
	strcpy(lcd_data, "DOWN=OFF / UP=ON");
	lcd_display(2);
	lcd_byte_display(2, 6, 'O');
    }
    temp_ptr_3 = 0;
    temp_ptr_4 = 0;
    temp_ptr_3 = strstr(corp_mode_acc_id, "NULL");
//    temp_ptr_4 = strstr(corp_mode_acc_id, "0000"); //3.1.1
    temp_ptr_4 = strstr(corp_mode_acc_id, "00000000"); //3.1.2.G

    if ((temp_ptr_3 != 0) || (temp_ptr_4 != 0))
    {
	lcd_init();
	strcpy(lcd_data, "NO ACCOUNT ID");
	lcd_display(1);
	strcpy(lcd_data, "CONTACT SARVAJAL");
	lcd_display(2);
	buzzer(ERR01);
	timer2_sec_delay(error_display_time);
    }

    else
    {
	buzzer(AUT01);
	general_time_stamp_logout = sec_tick;
	button_detect = 0;
	while (button_detect != ENTER_DETECT)
	{
	    scan_switch();
	    if (button_detect == UP_DETECT)
	    {
		corporate_mode_en = 1;
		school_mode_en = 0;
		//				mode_flags.op_mode = CORPORATE_MODE;
		strcpy(lcd_data, "CORP. MODE ON");
		lcd_display(1);
		strcpy(lcd_data, "DOWN=OFF / UP=ON");
		lcd_display(2);
		button_detect = 0;
		lcd_byte_display(2, 15, 'O');
	    }
	    else if (button_detect == DOWN_DETECT)
	    {
		corporate_mode_en = 0;
		strcpy(lcd_data, "CORP. MODE OFF");
		lcd_display(1);
		strcpy(lcd_data, "DOWN=OFF / UP=ON");
		lcd_display(2);
		button_detect = 0;
		lcd_byte_display(2, 6, 'O');
	    }
	    else if (button_detect == ENTER_DETECT)
	    {
		//strcpy(lcd_data,"SYSTEM EXIT "); //17/05/17
		//lcd_display(1);  
		break;
	    }
	    else{}	  
	    if ((sec_tick - general_time_stamp_logout)>(user_timeout * 1))
	    {
		menu_flags.timeout_menu_options = TRUE;
		menu_flags.exit = TRUE;
		break;
	    }
	    else{}	   
	}
	buzzer(AUT01);
	if (school_mode_en)
	    mode_flags.op_mode = SCHOOL_MODE;
	else if (corporate_mode_en) //v2.2.5
	    mode_flags.op_mode = CORPORATE_MODE;
	else
	    mode_flags.op_mode = IDLE_MODE;

	strcpy(lcd_data, "PLEASE WAIT.. ");
	lcd_display(2);
	eeprom_write(SER_CORPORATE_MODE);
	eeprom_write(SER_SCHOOL_MODE);
	eeprom_write(SER_OP_MODE);

	send_server_response(DEV_SCHOOL_MODE);
	send_server_response(DEV_CORPORATE_MODE);
	send_server_response(DEV_CORPORATE_MODE_ACC_ID);
	buzzer(AUT03);
	send_server_response(DEV_FRANCHISEE_RFID);
	strcpy(lcd_data, "SUCCESSFULLY...");
	lcd_display(2);
	timer2_sec_delay(error_display_time);
    }
    menu_flags.exit = TRUE;
    menu_flags.function_access = TRUE;

}

void maintenance_request() //maintenance_request
{

    general_time_stamp_logout = sec_tick; //1.0.1
    if (exit_menu("PROCEED         "))
    {
	rfid_card_type = FRANCHISEE_CARD;
	strcpy(lcd_data, "SENDING...      ");
	lcd_display(1);
	strcpy(lcd_data, "MAINTENANCE REQ.");
	lcd_display(2);
	buzzer(HALT);
	send_server_response(DEV_FRANCHISEE_MAINTENANCE_RQ + 200);
	timer2_sec_delay(error_display_time);
	buzzer(AUT03);
	lcd_init();
	strcpy(lcd_data, "SENT SUCCESSFUL ");
	lcd_display(1);
	strcpy(lcd_data, "THANK YOU...!!  ");
	lcd_display(2);
	timer2_sec_delay(error_display_time);
	rfid_card_type = 0;
    }
    else
    {
    }
    button_detect = 0;
    menu_flags.any_button_press = 0;
    consumer_flags.dispense_button = 0;
    clear_lcd();
}

void device_information()
{
    unsigned char sub_menu_info = 1;
    unsigned int time_stamp_menu_logout = 0;
    unsigned char loop = 0;
    lcd_init();
    read_eeprom_parameter(SER_OUT_FLOW_CALIB);
    strcpy(lcd_data, "SYSTEM LOAD     ");
    lcd_display(1);
    strcpy(lcd_data, "PLEASE WAIT.....");
    lcd_display(2);
    buzzer(AUT01);
    timer2_sec_delay(logout_idle_time);
    timer2_ms_delay(500); // This delay is to just wait for the user to release the key after Long press so any other detection can't take place

    button_detect = 0;
    general_time_stamp_logout = sec_tick; //1.0.1
    menu_flags.timeout_menu_options = FALSE; //1.0.1
    time_stamp_menu_logout = timer2_tick; //FOR AUTO LOG-OFF TIME
    //	while((consumer_flags.dispense_type == 0) && (local_flags.sub_menu_option==0)) 
    while (button_detect != MENU_DETECT)
    {
	scan_switch();
	if (button_detect == DOWN_DETECT)
	{
	    sub_menu_info--;
	    if (sub_menu_info < 1) sub_menu_info = 4;
	    else{}	  
	    time_stamp_menu_logout = timer2_tick;
	    button_detect = 0;
	}
	else if (button_detect == UP_DETECT)
	{
	    sub_menu_info++;
	    if (sub_menu_info > 4) sub_menu_info = 1;
	    else{}
	    time_stamp_menu_logout = timer2_tick;
	    button_detect = 0;
	}
	else if (button_detect == MENU_DETECT)
	{
	    buzzer(AUT01); //INDICATION TO EXIT
	    break;
	}
	else
	{
	}

	if (sub_menu_info == 1) //FOR SCREEN-1
	{
	    if ((peripheral_flags.lls_hls_en_dis >= 2) && (peripheral_flags.lls_trigger == TRUE))
		strcpy(lcd_data, "TANK LEVEL:LOW");
	    else if ((peripheral_flags.lls_hls_en_dis >= 2) && (peripheral_flags.hls_trigger == TRUE))
		strcpy(lcd_data, "TANK LEVEL:FULL");
	    else if ((peripheral_flags.lls_hls_en_dis >= 2) && ((peripheral_flags.hls_trigger == 0) && (peripheral_flags.lls_trigger == 0)))
		strcpy(lcd_data, "TANK LEVEL:MED");
	    else
		strcpy(lcd_data, "TANK LEVEL:OFF");
	    lcd_display(1);

	    loop = atoi(network_signal_strength);
	    if ((loop <= 31) && (loop >= 25))
		strcpy(lcd_data, "NETWORK: GOOD");
	    else if ((loop <= 25) && (loop >= 17))
		strcpy(lcd_data, "NETWORK: AVERAGE");
	    else if ((loop <= 17) && (loop >= 1))
		strcpy(lcd_data, "NETWORK: POOR");
	    lcd_display(2);

	}
	else if (sub_menu_info == 2) //FOR SCREEN-2
	{
	    if (peripheral_flags.temp_en_dis == TRUE)
		sprintf(lcd_data, "TEMP:ON T:%.5s", temp_value);
	    else if (peripheral_flags.temp_en_dis == FALSE)
		sprintf(lcd_data, "TEMP:OFF        ");
	    else{}
	    lcd_display(1);

	    if (peripheral_flags.tds_en_dis == TRUE)
		sprintf(lcd_data, "TDS:ON  T:%.5s  ", tds_value);
	    else if (peripheral_flags.tds_en_dis == FALSE)
		strcpy(lcd_data, "TDS:OFF ");
	    else{}
	    lcd_display(2);
	}
	else if (sub_menu_info == 3) //FOR SCREEN-3  
	{
	    sprintf(lcd_data, "OUTFLOW CALI:%.3d", out_flow_calibration);
	    lcd_display(1);
	    if (peripheral_flags.tds_en_dis == TRUE)
		sprintf(lcd_data, "TDS CALI: %u", tds_calib_fact);
	    else if (peripheral_flags.tds_en_dis == FALSE)
		strcpy(lcd_data, "TDS CALI: OFF"); //17/05/17
	    else{}
	    lcd_display(2);
	}
        else if(sub_menu_info == 4)			//FOR SCREEN-4 //v2.2.6
        {
                if(peripheral_flags.chiller_heater_en_dis==1)		//v2.3.6
                        strcpy(lcd_data,"CHILLER ENABLE");
                else if(peripheral_flags.chiller_heater_en_dis==2)	//v2.3.6
                        strcpy(lcd_data,"HEATER ENABLE");
                else
                        strcpy(lcd_data,"CHILER/HEATR DIS");	//v2.3.6

                lcd_display(1);
                if(peripheral_flags.chiller_status_flag==TRUE)		//v2.3.6
                        strcpy(lcd_data,"CHILLER ON");
                else if(peripheral_flags.heater_status_flag==TRUE)	//v2.3.6
                        strcpy(lcd_data,"HEATER ON");
                else
                        strcpy(lcd_data,"CHILER/HEATR OFF");	//v2.3.6
                lcd_display(2);

        }
							
	else{}
	if ((timer2_tick - time_stamp_menu_logout)>(user_timeout * 2000)) //SUB_MENU_TIMEOUT= 3*2000=60 SECONDS
	{
	    break;
	}
	else{}
    }
    button_detect = 0;
    function_state = 0;
}

unsigned char sensor_calibration_menu(unsigned char string_data_1[], unsigned char sensor)
{
    unsigned char temp_count = 0;
    lcd_init();
    strcpy(lcd_data, string_data_1);
    lcd_display(1);
    if (sensor == SER_OUT_FLOW_CALIB)
    {
	read_eeprom_parameter(SER_OUT_FLOW_CALIB);
	sprintf(lcd_data, "000      %.3u", out_flow_calibration);
    }
    else if (sensor == SER_TDS_CALIB)
    {
	read_eeprom_parameter(SER_TDS_CALIB);
	sprintf(lcd_data, "000     %.3u", tds_calib_fact);
    }
    else if (sensor == SER_CARD_INIT_BAL)
    {
	read_eeprom_parameter(SER_CARD_INIT_BAL);
	sprintf(lcd_data, "000     %.3u", card_init_balance);
    }
    lcd_display(2);
    memset(menu_current_password, '0', sizeof (menu_current_password)); //null si required to send in function argument
    button_detect = 0;

    while (button_detect != MENU_DETECT)
    {
	menu_current_password[0] = user_value(2, 1, menu_current_password[0], 9);
	if (button_detect == MENU_DETECT)
	{
	    break;
	}
	else if (button_detect == NO_DETECT)
	{
	    break;
	}
	else
	{
	    button_detect = 0;
	}
	menu_current_password[1] = user_value(2, 2, menu_current_password[1], 9);
	if (button_detect == MENU_DETECT)
	{
	    break;
	}
	else if (button_detect == NO_DETECT)
	{
	    break;
	}
	else
	{
	    button_detect = 0;
	}
	menu_current_password[2] = user_value(2, 3, menu_current_password[2], 9);
	if (button_detect == MENU_DETECT)
	{
	    break;
	}
	else if (button_detect == NO_DETECT)
	{
	    break;
	}
	else
	{
	    button_detect = 0;
	}
	//	menu_current_password[3]=user_value(2,4,menu_current_password[3],9);if(button_detect==MENU_DETECT){break;} else{button_detect =0;}
    }
    menu_current_password[3] = '\0';
    menu_current_password[4] = '\0';
    buzzer(AUT01);
    if (button_detect == NO_DETECT)
    {
	strcpy(lcd_data, "USER ENTER      ");
	lcd_display(1);
	strcpy(lcd_data, "TIMEOUT         ");
	lcd_display(2);
	button_detect = 0;
	timer2_sec_delay(error_display_time);
	return 0;
    }
    else
    {
    }
    if (exit_menu("SET VALUE       "))
    {

        if (sensor == SER_OUT_FLOW_CALIB) //   SER_TDS_CALIB
        {
            out_flow_calibration = atoi(menu_current_password);
            temp_count = 0;
            temp_count = data_validation(SER_OUT_FLOW_CALIB);
            if (temp_count == 0)
            {
                strcpy(lcd_data, "OUT FLOW CALI.");
                lcd_display(1);
                sprintf(lcd_data, "SET VALUE= %u", out_flow_calibration);
                lcd_display(2);
                eeprom_write(SER_OUT_FLOW_CALIB);
                send_server_response(DEV_OUT_FLOW_CALIB);
                timer2_sec_delay(error_display_time);
            }
            else
            {
                strcpy(lcd_data, "OUT FLOW CALI.");
                lcd_display(1);
                sprintf(lcd_data, "PREV. VALUE= %u", out_flow_calibration);
                lcd_display(2);
                timer2_ms_delay(4000);
            }

        }
        else if (sensor == SER_TDS_CALIB)
        { //   SER_TDS_CALIB		
            tds_calib_fact = atoi(menu_current_password);
            temp_count = 0;
            temp_count = data_validation(SER_TDS_CALIB);
            if (temp_count == 0)
            {
                strcpy(lcd_data, "TDS CALI.");
                lcd_display(1);
                sprintf(lcd_data, "SET VALUE= %u", tds_calib_fact);
                lcd_display(2);
                eeprom_write(SER_TDS_CALIB);
                send_server_response(DEV_TDS_CALIB);
                timer2_sec_delay(error_display_time);
            }
            else
            {
                strcpy(lcd_data, "TDS CALI.");
                lcd_display(1);
                sprintf(lcd_data, "PREV. VALUE= %u", tds_calib_fact);
                lcd_display(2);
                timer2_ms_delay(4000);
            }
        }
        else if (sensor == SER_CARD_INIT_BAL)
        {
            card_init_balance = atoi(menu_current_password);
            temp_count = 0;
            temp_count = data_validation(SER_CARD_INIT_BAL); //v3.1.1.C
            if (temp_count == 0)
            {
            strcpy(lcd_data, "CARD INIT BAL");  //BAL. to BAL //3.1.3.A
            lcd_display(1);
            sprintf(lcd_data, "SET VALUE= %u", card_init_balance);
            lcd_display(2);
            eeprom_write(SER_CARD_INIT_BAL);
            //send_server_response(DEV_TDS_CALIB);				
            timer2_sec_delay(error_display_time);
            }
            else
            {
            //strcpy(lcd_data,"TDS CALI.");lcd_display(1);
            //sprintf(lcd_data,"PREV. VALUE= %u",tds_calib_fact);lcd_display(2);timer2_ms_delay(4000);
            }

        }
        else{}	
    }
    else{}
    button_detect = 0;
    menu_flags.any_button_press = 0;
    consumer_flags.dispense_button = 0;
    clear_lcd();
}

void manual_RTC_set_time()
{
    unsigned char local_date_stamp[20] = {0}, next_value = 0;
    clear_lcd();
    get_local_time();

    //sprintf(lcd_data,"%.2d/%.2d/%.4d%.2d",sd_date,sd_month,sd_year,sd_hour);
    //lcd_display(1);	
    sprintf(lcd_data, "DD/MM/YYYY HH/MM");
    lcd_display(1);
    //lcd_set_cursor(1,2,15);
    memset(local_date_stamp, '0', sizeof (local_date_stamp)); //null si required to send in function argument	
    sprintf(local_date_stamp, "%.2d/%.2d/%.4d %.2d/%.2d", sd_date, sd_month, sd_year, sd_hour, sd_min);
    strncpy(lcd_data, local_date_stamp, 16);
    lcd_display(2);
    button_detect = 0;
    while (button_detect != MENU_DETECT)
    {

	local_date_stamp[0] = user_value(2, 1, local_date_stamp[0], 3); //DD			
	if ((local_date_stamp[0] - '0') > 2)
	{
	    if ((local_date_stamp[1] - '0') > 2)
	    {
		local_date_stamp[1] = '0';
	    }
	    else{}
	}
	else
	{
	    next_value = 9;
	}
	if (button_detect == MENU_DETECT)
	{
	    break;
	}
	else
	{
	    button_detect = 0;
	}
	local_date_stamp[1] = user_value(2, 2, local_date_stamp[1], next_value);
	if (button_detect == MENU_DETECT)
	{
	    break;
	}
	else
	{
	    button_detect = 0;
	}

	local_date_stamp[3] = user_value(2, 4, local_date_stamp[3], 1); //MM				
	if ((local_date_stamp[3] - '0') > 0)
	{
	    next_value = 2;
	    if ((local_date_stamp[4] - '0') > 2)
	    {
		local_date_stamp[4] = '0';
	    }
	    else{}
	}
	else
	{
	    next_value = 9;
	}
	if (button_detect == MENU_DETECT)
	{
	    break;
	}
	else
	{
	    button_detect = 0;
	}
	local_date_stamp[4] = user_value(2, 5, local_date_stamp[4], next_value);
	if (button_detect == MENU_DETECT)
	{
	    break;
	}
	else
	{
	    button_detect = 0;
	}

	local_date_stamp[8] = user_value(2, 9, local_date_stamp[8], 2);
	if (button_detect == MENU_DETECT)
	{
	    break;
	}
	else
	{
	    button_detect = 0;
	} //YY			
	local_date_stamp[9] = user_value(2, 10, local_date_stamp[9], 9);
	if (button_detect == MENU_DETECT)
	{
	    break;
	}
	else
	{
	    button_detect = 0;
	}

	local_date_stamp[11] = user_value(2, 12, local_date_stamp[11], 2); //HH
	if ((local_date_stamp[11] - '0') > 1)
	{
	    next_value = 3;
	    if ((local_date_stamp[12] - '0') > 3)
	    {
		local_date_stamp[12] = '0';
	    }
	    else{}
	}
	else
	{
	    next_value = 9;
	}
	if (button_detect == MENU_DETECT)
	{
	    break;
	}
	else
	{
	    button_detect = 0;
	}
	local_date_stamp[12] = user_value(2, 13, local_date_stamp[12], next_value);
	if (button_detect == MENU_DETECT)
	{
	    break;
	}
	else
	{
	    button_detect = 0;
	}

	local_date_stamp[14] = user_value(2, 15, local_date_stamp[14], 5);
	if (button_detect == MENU_DETECT)
	{
	    break;
	}
	else
	{
	    button_detect = 0;
	} //MM	
	local_date_stamp[15] = user_value(2, 16, local_date_stamp[15], 9);
	if (button_detect == MENU_DETECT)
	{
	    break;
	}
	else
	{
	    button_detect = 0;
	}
    }
    //strncpy(lcd_data,local_date_stamp,16);
    if (exit_menu(local_date_stamp))
    {
	buzzer(AUT01);
	sd_date = ((local_date_stamp[0] & 0x0f) << 4)+ (local_date_stamp[1]& 0x0f);
	sd_month = ((local_date_stamp[3] & 0x0f) << 4)+ (local_date_stamp[4]& 0x0f);
	sd_yr = ((local_date_stamp[8] & 0x0f) << 4)+ (local_date_stamp[9]& 0x0f);
	sd_hour = ((local_date_stamp[11] & 0x0f) << 4)+ (local_date_stamp[12]& 0x0f);
	sd_min = ((local_date_stamp[14] & 0x0f) << 4)+ (local_date_stamp[15]& 0x0f);
	sd_sec = 0x00;
	if (((sd_date >= 0x01)&&(sd_date <= 0x31))&&((sd_month >= 0x01)&&(sd_month <= 0x12))&&((sd_yr >= 0x16)&&(sd_yr <= 0x99))&&((sd_hour >= 0x00)&&(sd_hour <= 0x23))&&((sd_min >= 0)&&(sd_min <= 0x59)))
	{
	    set_time_DS1307();
	    timer2_ms_delay(100);
	    get_local_time();
	    sprintf(lcd_data, "%.2d/%.2d/%.2d [%.2d/%.2d]", sd_date, sd_month, sd_yr, P_date, P_month);
	    lcd_display(1);
	    sprintf(lcd_data, "%.2d:%.2d:%.2d [%.2d]", sd_hour, sd_min, sd_sec, P_yr);
	    lcd_display(2);
	    rtc_flag.sync_type = BY_MANUALLY_SET_TIME;
	    error_type2 = rtc_flag.sync_type;
	    send_server_response(DEV_TIME_SYNC_BY_MODEM);
	    send_server_response(DEV_ERROR_TYPE2);
	    timer2_sec_delay(error_display_time);
	}
	else
	{
	    strcpy(lcd_data, "TIME SET NOT   ");
	    lcd_display(1);
	    strcpy(lcd_data, "PROPERLY       ");
	    lcd_display(2);
	    timer2_sec_delay(error_display_time);
	}
    }
    else
    {
	strcpy(lcd_data, "TIME SET ");
	lcd_display(1);
	strcpy(lcd_data, "CANCEL    ");
	lcd_display(2);
	timer2_sec_delay(error_display_time);
    }
    button_detect = 0;
}

void dispense_water_data_backup()
{
    clear_lcd();
    strcpy(lcd_data, "DAILY DISP. SEND");
    lcd_display(1);
    timer2_sec_delay(error_display_time);
    FSchdir("\\");
    FSchdir("TOTAL");
    memset(file_name, '\0', sizeof (file_name));
    memset(daily_total_lit_ary, '\0', sizeof (daily_total_lit_ary));
    sprintf(file_name, "%.8s.txt", "Cur_date"); //v3.1.1.D
    FSfclose(sd_pointer); //@ f_close(&fil);
    sd_pointer = FSfopen(file_name, FS_READPLUS);
    if (FSfread(daily_total_lit_ary, 1, 10, sd_pointer) >= 10)
    {}
    else
    {}
    FSfclose(sd_pointer);
    memset(file_name, '\0', sizeof (file_name));
    sprintf(file_name, "%.8s.txt", "pre_date"); //v3.1.1.D
    FSfclose(sd_pointer);
    sd_pointer = FSfopen(file_name, FS_READPLUS);
    if (FSfwrite(daily_total_lit_ary, 1, 10, sd_pointer) >= 10)
    { } //@	f_write (&fil,daily_total_lit_ary,10,&br);	
    else{}
    FSfclose(sd_pointer);
    memset(file_name, '\0', sizeof (file_name));
    sd_pointer = FSfopen("Cur_date.txt", FS_READPLUS);
    FSfwrite("0000000000", 1, 10, sd_pointer);
    FSfclose(sd_pointer);
    FSchdir("\\");
    daily_total_lit = atol(daily_total_lit_ary);
    daily_total_dispense = (double) daily_total_lit / 100.0; //	timer2_sec_delay(error_display_time);
    //	sprintf(lcd_data,"Disp.=%4.2f",((double)daily_total_dispense));lcd_display(1);timer2_ms_delay(2000);	
    send_server_response(DEV_DAILY_WATER_DISPENSE); //v3.0.0					
}

void total_water_dispense_store(float water_dispense_var)
{
    FSchdir("\\");
    FSchdir("TOTAL");
    memset(file_name, '\0', sizeof (file_name));              //v3.1.2.F
//    sprintf(file_name, "%.8s.txt", "Cur_date"); //1.0.1       //v3.1.1.D	

    memset(daily_total_lit_ary, '\0', sizeof (daily_total_lit_ary));
    FSfclose(sd_pointer);
    sd_pointer = FSfopen("Cur_date.txt", FS_READPLUS);
    if (FSfread(daily_total_lit_ary, 1, 10, sd_pointer) != 10)
    {}
    else{}
    FSfclose(sd_pointer);
    daily_total_lit = atol(daily_total_lit_ary);
    daily_total_dispense = (double) daily_total_lit / 100.0;
    //................. 1.0.1
    daily_total_dispense += water_dispense_var;
    daily_total_lit = (unsigned long int) (daily_total_dispense * 100);
    sprintf(daily_total_lit_ary, "%.10ld", daily_total_lit);

//  memset(file_name, '\0', sizeof (file_name));            //v3.1.2.F
//  sprintf(file_name, "%.8s.txt", "Cur_date"); //1.0.1      //v3.1.1.D	 //v3.1.2.F
    FSfclose(sd_pointer);
    sd_pointer = FSfopen("Cur_date.txt", FS_READPLUS); //@	f_open(&fil,file_name,FA_READ | FA_WRITE );//v3.1.2.F

    if (FSfwrite(daily_total_lit_ary, 1, 10, sd_pointer) != 10)
    {}   //@	f_write (&fil,daily_total_lit_ary,10,&br);   
    else{}  
    FSfclose(sd_pointer);
    FSchdir("\\");
}


void add_new_card()
{
    unsigned char temp_var = 1, user_add_error_flag = 0;
    unsigned int cs_time_stamp = 0;
    unsigned char rfid_read1[9] = {0};
    unsigned char rfid_read2[9] = {0};
    //unsigned char temp_user_balance_array[7]={0};  //v3.1.1.C
    unsigned char default_key[8] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    unsigned char new_key[8] = {"SRVJAL"};
    //unsigned long int temp_user_add_balance=0;

    Rfid_addition_type = 0; //manually add card
    temp_var = 1;
    while (temp_var < 3)
    {
	lcd_init();
	if (temp_var == 1)
	{
	    strcpy(lcd_data, "PUT CARD ON     ");
	    lcd_display(1);
	    strcpy(lcd_data, "SCAN AREA       ");
	    lcd_display(2);
	}
	else
	{
	    strcpy(lcd_data, "PUT & HOLD CARD ");
	    lcd_display(1);
	    strcpy(lcd_data, "ON SCAN AREA    ");
	    lcd_display(2);
	}
	general_time_stamp_logout = sec_tick; //1.0.1
	consumer_flags.card_detect = LOW;
	consumer_flags.rfid_read = LOW;
	cs_time_stamp = timer2_tick;
	while ((consumer_flags.card_detect != HIGH)&&((timer2_tick - cs_time_stamp) < user_timeout * 500));

	cs_time_stamp = timer2_tick;
	if ((consumer_flags.card_detect == HIGH)&&(card_Serial_number() == 1)    )
	{
	    consumer_flags.rfid_read = HIGH;
	    general_time_stamp_logout = sec_tick;

	    if (temp_var == 1)
	    {
		strcpy(rfid_read1, rfid_card);
		rfid_read1[8] = '\0';
		clear_lcd();
		sprintf(lcd_data, "SCAN %d OK", temp_var);
		lcd_display(1);
		buzzer(AUT01);
		read_eeprom_parameter(SER_CARD_INIT_BAL);
		timer2_sec_delay(error_display_time);
	    }
	    else if (temp_var == 2)
	    {
		strcpy(rfid_read2, rfid_card);
		rfid_read2[8] = '\0';
		buzzer(AUT01);

		if (!strcmp(rfid_read1, rfid_read2))
                {
                    menu_flags.timeout_menu_options = FALSE;
                    menu_flags.exit = FALSE;
                    strcpy(lcd_data, "PLS. HOLD CARD  ");
                    lcd_display(1);
                    strcpy(lcd_data, "ON SCAN AREA    ");
                    lcd_display(2);
                    if (rfid_change_key(0x00, 0x02, default_key, new_key))
                    {
                        timer2_ms_delay(500);
                        Rfid_user_bal_paisa = (unsigned long int) card_init_balance; //v3.1.1.A
                        if (rfid_Epurse_init(0x00, 0x08, (Rfid_user_bal_paisa * 100))) //convert to paisa  //v3.1.1.A
                        {
                            
                            // --------------  //3.1.3.B
                            get_local_time();
                            memset(acc_credit_array, '\0', sizeof (acc_credit_array));
                            sprintf(acc_credit_array, "%2.1f", 0);
                            send_server_response(DEV_USER_ADD);
                            
                            memset(acc_credit_array, '\0', sizeof (acc_credit_array));
			    memset(previous_bal_array, '\0', sizeof (previous_bal_array));
			    sprintf(acc_credit_array, "%2.1f", (double) card_init_balance);
			    sprintf(previous_bal_array, "%.3d", 0);  //v3.1.1.F
			    send_server_response(DEV_BAL_ADD);
                            
                            // --------------  //3.1.3.B
                            
                            
//                            sprintf(acc_credit_array, "%2.1f", (double) card_init_balance);  //3.1.3.B
//                            get_local_time();
//                            send_server_response(DEV_USER_ADD);
                            timer2_sec_delay(error_display_time);
                            lcd_init();
                            strcpy(lcd_data, "USER ADD        ");
                            lcd_display(1);
                            strcpy(lcd_data, "SUCCESSFULLY    ");
                            lcd_display(2);
                            buzzer(AUT03);
                            user_add_error_flag = 0;
                            break;
                        }
                        else
                        {                           
                            get_local_time();
                            send_server_response(DEV_USER_ADD);
                            strcpy(lcd_data, "USER ADD        ");
                            lcd_display(1);
                            strcpy(lcd_data, "INCOMPLETE      ");
                            lcd_display(2);
                            buzzer(ERR01);
                            user_add_error_flag = 4;
                            break;
                        }
                        timer2_sec_delay(error_display_time);
                    }
                    else
                    {
                        if (rfid_read_purse(0x00, 0x08))
                        {
                            if (curnt_user_bal_paisa > 0)
                            {
                                timer2_sec_delay(error_display_time);
                                strcpy(lcd_data, "USER ALREADY    "); //1.0.1
                                lcd_display(1);
                                strcpy(lcd_data, "ADDED           ");
                                lcd_display(2);
                                buzzer(ERR01);
                                user_add_error_flag = 4;
                                break;
                            }
                            else
                            {
                                                               
                                Rfid_user_bal_paisa = (unsigned long int) card_init_balance; //v3.1.1.A
                                if (rfid_Epurse_init(0x00, 0x08, (Rfid_user_bal_paisa * 100))) //convert to paisa  ///3.1.3.B
                                {                  
                              //  if (rfid_Epurse_init(0x00, 0x08, 0))  //3.1.3.A
                               // {
                                    memset(acc_credit_array, '\0', sizeof (acc_credit_array));
                                    memset(previous_bal_array, '\0', sizeof (previous_bal_array));
                                    sprintf(acc_credit_array, "%2.1f", (double) card_init_balance);  //3.1.3.B
//                                    sprintf(acc_credit_array, "%2.1f", 0);  c
                                    sprintf(previous_bal_array, "%.3d", 0);  //v3.1.1.F
                                    
                                  //  sprintf(acc_credit_array, "%2.1f", (double) card_init_balance);
                                //    sprintf(previous_bal_array, "%1.1f", 0);  //v3.1.1.F
                                    get_local_time();
                                    send_server_response(DEV_BAL_ADD);
                                    
                                    
//                                    sprintf(acc_credit_array, "%2.1f", 0);
//                                    get_local_time();
//                                    send_server_response(DEV_USER_ADD);
                                    timer2_sec_delay(error_display_time);
                                    lcd_init();
                                    strcpy(lcd_data, "USER ADD.       ");
                                    lcd_display(1);
                                    strcpy(lcd_data, "SUCCESSFULLY    ");
                                    lcd_display(2);
                                    buzzer(AUT03);
                                    user_add_error_flag = 0;
                                    break;
                                }
                                else{}
                            }
                        }
                        else
                        {
                            timer2_sec_delay(error_display_time);
                            strcpy(lcd_data, "USER            ");
                            lcd_display(1);
                            strcpy(lcd_data, "UNAUTHORISED    ");
                            lcd_display(2);
                            buzzer(ERR01);
                            user_add_error_flag = 4;
                            break;
                        }
                        timer2_sec_delay(error_display_time);
                    }
                }
                else
                {
                    lcd_init();
                    strcpy(lcd_data, "IMPROPER SCAN   ");
                    lcd_display(1);
                    strcpy(lcd_data, "PLS. TRY AGAIN. ");
                    lcd_display(2);
                    buzzer(ERR01);
                    timer2_sec_delay(error_display_time);
                    user_add_error_flag = 4;
                    break;
                }
            }
            else
            {
            }
            temp_var++;
        }
        else
        {
            lcd_init();
            strcpy(lcd_data, "IMPROPER SCAN   ");
            lcd_display(1);
            strcpy(lcd_data, "PLS. TRY AGAIN  ");
            lcd_display(2);
            buzzer(ERR01);
            timer2_sec_delay(error_display_time);
            consumer_flags.rfid_read = LOW;
            user_add_error_flag++;
        }
        consumer_flags.card_detect = LOW;
        if (user_add_error_flag >= 3)
        {
            break;
        }
    }
    button_detect = 0;
    consumer_flags.rfid_read = LOW; //v1.0.0
    consumer_flags.card_detect = LOW;
    Rfid_user_bal_paisa = 0; //v3.1.1.A
    curnt_user_bal_paisa = 0; //v3.1.1.A
    curnt_user_bal_rs = 0; //v3.1.1.A
    timer2_sec_delay(error_display_time);
}

void add_balance()
{
    unsigned char temp_var = 1, user_bal_add_error_flag = 0;
    unsigned long int card_scan_time_stamp = 0;
    unsigned char rfid_read1[9] = {0};
    unsigned char rfid_read2[9] = {0};
    //	unsigned char temp_user_balance_array[7]={0};
    //	unsigned long int temp_user_balance=0;
    float temp_user_previous_bal_rs = 0;
    unsigned char temp_value_1[4] = {0}; //temp_value_2[4]={0};  //v3.1.1.C
    unsigned char temp_str1[17] = {0};
    unsigned long int temp_user_enter_val = 0; //v3.1.1.A
    //	unsigned int recharge_balance= 0;
    temp_var = 1;
    user_bal_add_error_flag = 0;
    button_detect = 0;
    lcd_init();
    //    strcpy(lcd_data,"PUT CARD ON     ");
    //    lcd_display(1);
    //    strcpy(lcd_data,"SCAN AREA       ");
    //    lcd_display(2);
    while (temp_var < 3)
    {
	general_time_stamp_logout = sec_tick; //1.0.1
	consumer_flags.card_detect = LOW;
	consumer_flags.rfid_read = LOW;
	card_scan_time_stamp = timer2_tick;
	button_detect = 0;
	if (temp_var == 1)
	{
	    strcpy(lcd_data, "PUT CARD ON     ");
	    lcd_display(1);
	    strcpy(lcd_data, "SCAN AREA       ");
	    lcd_display(2);
	}
	else
	{
	    strcpy(lcd_data, "PUT & HOLD CARD ");
	    lcd_display(1);
	    strcpy(lcd_data, "ON SCAN AREA    ");
	    lcd_display(2);
	}
	while ((consumer_flags.card_detect != HIGH)&&((timer2_tick - card_scan_time_stamp) < user_timeout * 400))
	{
	    if (button_detect == MENU_DETECT)
	    {
		user_bal_add_error_flag = 4;
		break;
	    }
	    else{}
	}
	if ((consumer_flags.card_detect == HIGH)&&(card_Serial_number() == 1))
	{
	    button_detect = 0;
	    general_time_stamp_logout = sec_tick;
	    if (temp_var == 1)
	    {
		strcpy(rfid_read1, rfid_card);
		rfid_read1[8] = '\0';

		if (rfid_read_purse(0x00, 0x08))
		{
		    lcd_init();
		    memset(temp_value_1, '0', sizeof (temp_value_1)); //null is required to send in function argument
		    curnt_user_bal_rs = (double) curnt_user_bal_paisa / 100.0;
		    temp_user_previous_bal_rs = curnt_user_bal_rs;
		    sprintf(lcd_data, "CARD BAL:%4.2f", curnt_user_bal_rs);
		    lcd_display(1);
		    strncpy(temp_value_1, previous_bal_array, 3);
		    sprintf(lcd_data, "RECH BAL:%.3s", temp_value_1);
		    lcd_display(2);
		    buzzer(AUT01);
		    button_detect = 0;
		    while (button_detect != MENU_DETECT)
		    {
			temp_value_1[0] = user_value(2, 10, temp_value_1[0], 9);
			if (button_detect == MENU_DETECT)
			{
			    break;
			}
			else if (button_detect == NO_DETECT)
			{
			    break;
			}
			else
			{
			    button_detect = 0;
			}
			temp_value_1[1] = user_value(2, 11, temp_value_1[1], 9);
			if (button_detect == MENU_DETECT)
			{
			    break;
			}
			else if (button_detect == NO_DETECT)
			{
			    break;
			}
			else
			{
			    button_detect = 0;
			}
			temp_value_1[2] = user_value(2, 12, temp_value_1[2], 9);
			if (button_detect == MENU_DETECT)
			{
			    break;
			}
			else if (button_detect == NO_DETECT)
			{
			    break;
			}
			else
			{
			    button_detect = 0;
			}
		    }
		    if (button_detect == NO_DETECT)
		    {
			strcpy(lcd_data, "BALANCE ADD     ");
			lcd_display(1);
			strcpy(lcd_data, "TIMEOUT         ");
			lcd_display(2);
			button_detect = 0;
			user_bal_add_error_flag = 4;
			timer2_sec_delay(error_display_time);
			break;
		    }
		    else
		    {
		    }
		    button_detect = 0;
		    sprintf(temp_str1, "ADD BAL: %.3s", temp_value_1);
		    lcd_display(1);
		    //sprintf(previous_bal_array,"%.3s",temp_value_1);
		    //strncpy(temp_value_1,previous_bal_array,3);

		    if (exit_menu(temp_str1))
		    {
			peripheral_flags.user_enter_password = 1; //exit from menu and save the data
			clear_lcd();
			temp_user_enter_val = (((temp_value_1[0] - 0x30)*100)+((temp_value_1[1] - 0x30)*10)+(temp_value_1[2] - 0x30)); //recharge balance	
			if ((temp_user_enter_val + temp_user_previous_bal_rs) >= MAX_BALANCE_LIMIT)
			{
			    strcpy(lcd_data, "MAXIMUM BALANCE ");
			    lcd_display(1);
			    strcpy(lcd_data, "REACHED         ");
			    lcd_display(2);
			    buzzer(ERR01);
			    user_bal_add_error_flag = 4;
			    timer2_sec_delay(error_display_time);
			    //break;
			}
			else{}
		    }
		    else
		    {
			peripheral_flags.user_enter_password = 0;
			strcpy(lcd_data, "BALANCE ADD     ");
			lcd_display(1);
			strcpy(lcd_data, "CANCEL          ");
			lcd_display(2);
			buzzer(ERR01);
			timer2_sec_delay(error_display_time);
			break;
		    } //exit without save and balance discarded								
		}
		else
		{
		    clear_lcd();
		    strcpy(lcd_data, "USER            ");
		    lcd_display(1);
		    strcpy(lcd_data, "UNAUTHORISED    ");
		    lcd_display(2);
		    buzzer(ERR01);
		    user_bal_add_error_flag = 4;
		    timer2_sec_delay(error_display_time);
		    break;
		}
	    }
	    else if (temp_var == 2)
	    {
		strcpy(rfid_read2, rfid_card);
		rfid_read2[8] = '\0';

		if (!strcmp(rfid_read1, rfid_read2))
		{
		    menu_flags.timeout_menu_options = FALSE;
		    menu_flags.exit = FALSE;
		    strcpy(lcd_data, "PLS. HOLD CARD  ");
		    lcd_display(1);
		    strcpy(lcd_data, "ON SCAN AREA    ");
		    lcd_display(2);
		    buzzer(AUT01);

		    Rfid_user_bal_paisa = (unsigned long int) (temp_user_enter_val * 100);

		    if (rfid_Epurse_increment(0x00, 0x08, Rfid_user_bal_paisa))
		    {
			timer2_ms_delay(500); //v3.1.1.A  50 to 500				
			if (rfid_read_purse(0x00, 0x08))
			{
			    curnt_user_bal_rs = (double) (curnt_user_bal_paisa / 100.00); //convert into rs to display
			    //curnt_user_bal_rs = curnt_user_bal_paisa/100.0 ;							
			    memset(acc_credit_array, '\0', sizeof (acc_credit_array));
			    memset(previous_bal_array, '\0', sizeof (previous_bal_array));
			    sprintf(acc_credit_array, "%3.2f", (double) curnt_user_bal_rs); //final updated balance								
			    sprintf(previous_bal_array, "%.3ld", temp_user_enter_val);  //v3.1.1.F																		
			    get_local_time();
			    send_server_response(DEV_BAL_ADD);
			    timer2_sec_delay(error_display_time);
			    strcpy(lcd_data, "BAL. ADD SUCCESS");
			    lcd_display(1);
			    sprintf(lcd_data, "CARD BAL:%.6s", acc_credit_array);
			    lcd_display(2);
			    buzzer(AUT03);
			    user_bal_add_error_flag = 0;
			    break;
			}
			else
			{
			    strcpy(lcd_data, "BAL. ADD SUCCESS");
			    lcd_display(1);
			    user_bal_add_error_flag = 0;
			    break;
			}
		    }
		    else
		    {
			timer2_sec_delay(error_display_time);
			clear_lcd();
			strcpy(lcd_data, "IMPROPER SCAN   ");
			lcd_display(1);
			strcpy(lcd_data, "PLS. TRY AGAIN  ");
			lcd_display(2);
			timer2_sec_delay(error_display_time);
			temp_var = 1;
			user_bal_add_error_flag++;
		    }
		}
		else
		{
		    lcd_init();
		    strcpy(lcd_data, "IMPROPER SCAN   ");
		    lcd_display(1);
		    strcpy(lcd_data, "PLS. TRY AGAIN. ");
		    lcd_display(2);
		    buzzer(ERR01);
		    timer2_sec_delay(error_display_time);
		    break;
		}
	    }
	    else{}
	    temp_var++;
	}
	else
	{
	    lcd_init();
	    strcpy(lcd_data, "IMPROPER SCAN   ");
	    lcd_display(1);
	    strcpy(lcd_data, "PLS. TRY AGAIN  ");
	    lcd_display(2);
	    buzzer(ERR01);
	    timer2_sec_delay(error_display_time);
	    consumer_flags.rfid_read = LOW;
	    user_bal_add_error_flag++;
	}
	consumer_flags.card_detect = LOW;
	if (user_bal_add_error_flag >= 3)
	{
	    lcd_init();
	    strcpy(lcd_data, "BALANCE ADD     ");
	    lcd_display(1);
	    strcpy(lcd_data, "PROCESS FAILED  ");
	    lcd_display(2);
	    break;
	}
    }
    button_detect = 0;
    consumer_flags.rfid_read = LOW; //v1.0.0
    consumer_flags.card_detect = LOW;
    temp_user_enter_val = 0; //v3.1.1.A
    Rfid_user_bal_paisa = 0; //v3.1.1.A
    curnt_user_bal_paisa = 0; //v3.1.1.A
    curnt_user_bal_rs = 0; //v3.1.1.A
    timer2_sec_delay(error_display_time);
}

void remove_card()
{
    unsigned char temp_var = 1, user_remove_error_flag = 0; //temp_counter=0 //v3.1.1.C
    unsigned long int card_scan_time_stamp = 0;

    unsigned char rfid_read1[13] = {0}; //this array is usef after add .txt to searching of file
    unsigned char rfid_read2[9] = {0};
    //unsigned char temp_user_balance_array[7]={0};  //v3.1.1.C

    unsigned char default_key[8] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    unsigned char new_key[8] = {"SRVJAL"};

    memset(acc_credit_array, '\0', sizeof (acc_credit_array));
    general_time_stamp_logout = sec_tick; //1.0.1

    while (temp_var < 3)
    {
	consumer_flags.card_detect = LOW;
	consumer_flags.rfid_read = LOW;
	card_scan_time_stamp = timer2_tick;
	lcd_init();
	if (temp_var == 1)
	{
	    strcpy(lcd_data, "PUT CARD ON     ");
	    lcd_display(1);
	    strcpy(lcd_data, "SCAN AREA       ");
	    lcd_display(2);
	}
	else
	{
	    strcpy(lcd_data, "PUT & HOLD CARD ");
	    lcd_display(1);
	    strcpy(lcd_data, "ON SCAN AREA    ");
	    lcd_display(2);
	}
	while ((consumer_flags.card_detect != HIGH)&&((timer2_tick - card_scan_time_stamp) < user_timeout * 400));
	card_scan_time_stamp = timer2_tick;
	if ((consumer_flags.card_detect == HIGH)&&(card_Serial_number() == 1))
	{
	    //consumer_flags.rfid_read=HIGH;
	    general_time_stamp_logout = sec_tick;
	    if (temp_var == 1)
	    {
		strcpy(rfid_read1, rfid_card);
		rfid_read1[8] = '\0';
		buzzer(AUT01);
		clear_lcd();
		sprintf(lcd_data, "SCAN %d OK", temp_var);
		lcd_display(1);
		timer2_sec_delay(error_display_time);
	    }
	    else if (temp_var == 2)
	    {
		strcpy(rfid_read2, rfid_card);
		rfid_read2[8] = '\0';
		buzzer(AUT01);

		if (!strcmp(rfid_read1, rfid_read2))
		{
		    menu_flags.timeout_menu_options = FALSE;
		    menu_flags.exit = FALSE;
		    strcpy(lcd_data, "PLS. HOLD CARD  ");
		    lcd_display(1);
		    strcpy(lcd_data, "ON SCAN AREA    ");
		    lcd_display(2);
		    if (rfid_read_purse(0x00, 0x08))
		    {
                curnt_user_bal_rs = (double) (curnt_user_bal_paisa / 100.00); //convert into rs to display
                sprintf(acc_credit_array, "%3.2f", (double) curnt_user_bal_rs);
                if (rfid_Epurse_init(0x00, 0x08, 0))
                {
                    if (rfid_change_key(0x00, 0x02, new_key, default_key))
                    {
                    clear_lcd();
                    sprintf(lcd_data, "CARD BAL.:%3.2f", (double) curnt_user_bal_rs);
                    lcd_display(1);
                    timer2_sec_delay(error_display_time);
                    get_local_time();
                    Rfid_remove_type = 0;
                    send_server_response(DEV_USER_REMOVE);
                    clear_lcd();
                    strcpy(lcd_data, "USER REMOVE     ");
                    lcd_display(1);
                    strcpy(lcd_data, "SUCCESSFULLY    ");
                    lcd_display(2);
                    buzzer(AUT03);
                    user_remove_error_flag = 0;
                    break;
                    }
                    else
                    {
                    clear_lcd();
                    strcpy(lcd_data, "USER REMOVE     ");
                    lcd_display(1);
                    strcpy(lcd_data, "PROCESS FAILED  ");
                    lcd_display(2);
                    buzzer(ERR01);
                    user_remove_error_flag = 4; //user_process_fail	 			
                    timer2_sec_delay(error_display_time);
                    break;
                    }
                }
                else
                {
                    clear_lcd();
                    strcpy(lcd_data, "USER REMOVE     ");
                    lcd_display(1);
                    strcpy(lcd_data, "PROCESS FAILED..");
                    lcd_display(2);
                    buzzer(ERR01);
                    user_remove_error_flag = 4; //user_process_fail
                    break;
                }
		    }
		    else
		    {
			clear_lcd();
			strcpy(lcd_data, "USER            ");
			lcd_display(1);
			strcpy(lcd_data, "UNAUTHORISED    ");
			lcd_display(2);
			buzzer(ERR01);
			user_remove_error_flag = 4; //user_process_fail				
			timer2_sec_delay(error_display_time);
			break;
		    }
		}
		else
		{
		    lcd_init();
		    strcpy(lcd_data, "IMPROPER SCAN   ");
		    lcd_display(1);
		    strcpy(lcd_data, "PLS. TRY AGAIN  ");
		    lcd_display(2);
		    buzzer(ERR01);
		    timer2_sec_delay(error_display_time);
		    break;
		}
	    }
	    else{}
	    temp_var++;
	} //end of 	if((consumer_flags.card_detect==HIGH)&&(card_Serial_number()==1))
	else
	{
	    lcd_init();
	    strcpy(lcd_data, "IMPROPER SCAN   ");
	    lcd_display(1);
	    strcpy(lcd_data, "PLS. TRY AGAIN  ");
	    lcd_display(2);
	    buzzer(ERR01);
	    timer2_sec_delay(error_display_time);
	    consumer_flags.rfid_read = LOW;
	    user_remove_error_flag++;
	}

	if (user_remove_error_flag >= 3)
	    break;
    } //end of while	
    button_detect = 0;
    consumer_flags.rfid_read = LOW; //v1.0.0
    consumer_flags.card_detect = LOW;
    Rfid_user_bal_paisa = 0; //v3.1.1.A
    curnt_user_bal_paisa = 0; //v3.1.1.A
    curnt_user_bal_rs = 0; //v3.1.1.A
    timer2_sec_delay(error_display_time);
}

unsigned char search_file(unsigned char file_data[], unsigned char file_search)
{
    SearchRec sd_file_properties;
    unsigned char attributes;
    attributes = ATTR_ARCHIVE | ATTR_READ_ONLY | ATTR_HIDDEN;
    FSchdir("\\");
    FSchdir("Trans"); //dirctroy root to Trans and search file

    if (file_search == 1)
    {
	if (!FindFirst(file_data, attributes, &sd_file_properties))
	{
	    strcpy(lcd_data, sd_file_properties.filename); // sd_file.filename
	    lcd_display(1);
	    strcpy(lcd_data, "FILE 1 GET"); // sd_file.filename
	    lcd_display(2);
	    FSfclose(sd_pointer);
	    sd_pointer = FSfopen(file_name, FS_READPLUS); //fr = f_open(&fil,file_name, FA_READ);			
	    return 1;

	}
	else
	{
	    strcpy(lcd_data, "FILE 1 NOT GET");
	    lcd_display(2); // sd_file.filename
	}
	timer2_sec_delay(2);
	FSchdir(".."); //bac to trans and goto root directory and find file 
	if (!FindFirst(file_data, attributes, &sd_file_properties))
	{
	    strcpy(lcd_data, sd_file_properties.filename); // sd_file.filename
	    lcd_display(1);
	    strcpy(lcd_data, "FILE 2 GET"); // sd_file.filename
	    lcd_display(2);
	    FSfclose(sd_pointer);
	    sd_pointer = FSfopen(file_name, FS_READPLUS);
	    return 1;
	}
	else
	{
	    strcpy(lcd_data, "FILE 2 NOT GET");
	    lcd_display(2); // sd_file.filename
	    return 0;
	}
	timer2_sec_delay(2);
    }
    else if (file_search == 2)
    {
	if (!FindFirst(file_data, attributes, &sd_file_properties))
	{
	    if (!FSremove(sd_file_properties.filename))
	    {
		strcpy(lcd_data, "FILE REMOVED");
		lcd_display(2);
		timer2_sec_delay(2);
		FSfclose(sd_pointer);
		get_local_time();
		FSchdir("\\");
		FSchdir("Trans");
		sd_pointer = FSfopen("SYNC.txt", FS_APPEND);
		// FSfseek(sd_pointer,1, SEEK_END);
		memset(file_name, '\0', sizeof (file_name));
		sprintf(file_name, "#%.2d_%.2d_%.2d.txt;PEN", sd_yr, sd_month, sd_date);
		FSfprintf(sd_pointer, "%s\r\n^", file_name);
		FSfclose(sd_pointer);
		FSchdir("\\");
		return 1;
	    }
	    else
	    {
		strcpy(lcd_data, "FILE NOT REMOVE");
		lcd_display(2);
		timer2_sec_delay(2);
	    }
	}
	else
	{
	    strcpy(lcd_data, "FILE NOT FOUND");
	    lcd_display(2);
	    timer2_sec_delay(2);
	    return 0;
	}
    }
}
void calibration_mode()
{
    sprintf(lcd_data, "CALIBRATION MODE");
    lcd_display(1);
    sprintf(lcd_data, "START PLS. WAIT ");
    lcd_display(2);
    button_detect = 0;  
    sch_disp_fraction = 1.0;  //v3.1.2.I
    timer2_sec_delay(error_display_time);
    mode_flags.op_mode = CALIBRATION_MODE;
    calibration_mode_en = 1;
    menu_flags.exit = TRUE;
    menu_flags.function_access = TRUE;

}

void flow_error_remove()
{          
             if (exit_menu("CLEAR FLOW ERROR"))
             {  
                clear_lcd();
                strcpy(lcd_data, "PLS WAIT....... ");
                lcd_display(1);
                
                coin_flags.coin_disable_by_fs_error=0;  //v6  //remove sense                
                if ((peripheral_flags.coin_en_dis)&&(coin_flags.coin_collector_error == 0)) //3.1.2//v6
                {
                    coinbox_enable();
                }else{}
                timer2_sec_delay(2);
             }
             else{}
               button_detect = 0;
               menu_flags.any_button_press = 0;
               consumer_flags.dispense_button = 0;
}

void corporate_mode(void) //v2.2.5
{
    unsigned int cr_time_stamp = 0;
    unsigned int temp_compare_flow_count = 0, dummy_count = 0;
    unsigned char lit_count = 0;
    LED4_ON;

    //if ((consumer_flags.dispense_button == 1) || (menu_flags.any_button_press == 1))//if(consumer_flags.dispense_type==LONG_PRESS)
    if (((consumer_flags.dispense_button == 1) || (menu_flags.any_button_press == 1))&&(mode_flags.op_mode == CORPORATE_MODE))
    {        
        if (peripheral_flags.temp_en_dis == 0)
        {
            DS_temperature = 0;
            memset(temp_value, '\0', sizeof (temp_value));
            temp_value[0] = 'O';
            temp_value[1] = 'F',
            temp_value[2] = 'F';
            temp_value[3] = '\0';
        }
        else{}
        
        if ((peripheral_flags.lls_hls_en_dis >= 2)&&(peripheral_flags.lls_trigger == TRUE))
        {
            lcd_init();
            strcpy(lcd_data, "TANK WATER");
            lcd_display(1);
            strcpy(lcd_data, "LEVEL LOW");
            lcd_display(2);
            buzzer(ERR01);
            //---timer2_sec_delay(error_display_time);
            timer2_sec_delay((error_display_time) / 2); //v2.2.6
        }
        else{}
        acc_dispense_capacity = 5.0;
        temp_compare_flow_count = (int) (out_flow_calibration * DISPENSE_LEAST_COUNT); //3.1.B
        dummy_count = 1;

        mode_flags.op_sub_mode = CON_AUTH_OK;
        cr_time_stamp = timer2_tick;

        consumer_flags.dispense_button = 0; //consumer_flags.dispense_button = 0;
        menu_flags.any_button_press = 0;

        output_flow_count = 0;
        dispense_litre = 0.0;
        balance_deduct = 0;
        lit_count = 0.0;
        peripheral_flags.tds_update = 0;
        dispense_trail = 0;
        dispense_button_count = 0;
        
        lcd_init();
        sprintf(lcd_data, "TDS:%.4s ppm", tds_value);
        lcd_display(1);
        sprintf(disp_buffer, "%2.1f", dispense_litre);  
        sprintf(lcd_data, "D:%sL, T:%.2s", disp_buffer, temp_value);
        lcd_display(2);

        cr_time_stamp = timer2_tick;
        general_time_stamp_logout = sec_tick; //3.1.G

        while (((timer2_tick - cr_time_stamp)<(dispense_timeout * 1000)) && (mode_flags.op_sub_mode != CON_COMPLETE))
        {
            if ((sec_tick - general_time_stamp_logout) > 3) //3.1.2.G
            {
                general_time_stamp_logout = sec_tick;
                lcd_init();
                sprintf(lcd_data, "TDS:%.4s ppm", tds_value);
                lcd_display(1);
                sprintf(lcd_data, "D:%sL, T:%.2s", disp_buffer, temp_value);
                lcd_display(2);
            }
            else{}
         
            if (((consumer_flags.dispense_button == 0)&&(menu_flags.any_button_press == 0))&&(mode_flags.op_sub_mode != CON_HALT))
            {
                SV_VALVE_ON;
                mode_flags.op_sub_mode = CON_DISPENSE;
                dispense_button_count = 0;
                peripheral_flags.tds_update = 0;
                /* sprintf(lcd_data, "TDS:%.4s ppm", tds_value);//v3.1.2.G
                lcd_display(1);
                sprintf(lcd_data, "D:%.3sL, T:%.2s", disp_buffer, temp_value);
                lcd_display(2); */
            }
            else
            {
                if (mode_flags.op_sub_mode == CON_DISPENSE)
                {
                    SV_VALVE_OFF;
                    mode_flags.op_sub_mode = CON_HALT;
                    //@	     			consumer_flags.dispense_type = 0;
                    consumer_flags.dispense_button = 0; //consumer_flags.dispense_button = 0;
                    menu_flags.any_button_press = 0;

                    sprintf(lcd_data, "TDS:%.4s ppm", tds_value);
                    lcd_display(1);
                    sprintf(lcd_data, "D:%sL, T:%.2s", disp_buffer, temp_value);
                    lcd_display(2);
                    cr_time_stamp = timer2_tick;
                }
                else{}               
            }

            if (((timer2_tick - cr_time_stamp) > 2000) && (mode_flags.op_sub_mode == CON_HALT))
            {
                consumer_flags.dispense = 0;
                mode_flags.op_sub_mode = CON_COMPLETE;
                //@			consumer_flags.dispense_button = 0;
                consumer_flags.dispense_button = 0;
                menu_flags.any_button_press = 0;
            }
            else
            {
            }

            if ((consumer_flags.output_flow_sensed) && (mode_flags.op_mode == CORPORATE_MODE))
            {
                // if (consumer_flags.dispense == 0)
                // {
                // consumer_flags.dispense = TRUE;
                // dispense_litre += DISPENSE_LEAST_COUNT;
                // }
                // else{}

                //---no_dispense_error_count = 0;
                if (mode_flags.op_sub_mode == CON_DISPENSE)
                    cr_time_stamp = timer2_tick;
                else{}             

                //@	dispense_trail = 0;  //move to below beacause when SV on, flow sense =1 &  dispense_trail =0
                consumer_flags.output_flow_sensed = LOW;
                // if (output_flow_count >= ((int) (out_flow_calibration * dummy_count)))
                if (output_flow_count >= ((temp_compare_flow_count)*(dummy_count))) //3.1.B
                {
                    dispense_litre = (double) (dummy_count / 10.0); //3.1.1
                    dummy_count++; //3.1.B	
                    dispense_trail = 0;
                    sprintf(disp_buffer, "%2.1f", dispense_litre);
                    sprintf(lcd_data, "D:%sL, T:%.2s", disp_buffer, temp_value);
                    lcd_display(2);
                    if ((dispense_litre >= acc_dispense_capacity)&&(mode_flags.op_sub_mode == CON_DISPENSE)) //v2.2.8
                    {
                        SV_VALVE_OFF;
                        //--mode_flags.op_sub_mode = CON_COMPLETE;
                        mode_flags.op_sub_mode = CON_HALT;
                        //@	consumer_flags.dispense_type = 0;
                        consumer_flags.dispense = 0;

                        strcpy(lcd_data, "MAXIMUM DISPENSE");
                        lcd_display(1);
                        strcpy(lcd_data, "LIMIT REACHED");
                        lcd_display(2);
                        timer2_sec_delay(error_display_time);
                        cr_time_stamp = timer2_tick;
                        break ;                                  //v3.1.2.G
                    }
                    else{}                   
                    if ((dispense_litre - lit_count) >= 1)
                    {
                        buzzer(DISP);
                        lit_count = dispense_litre;
                        sprintf(lcd_data, "TDS:%.4s ppm", tds_value);
                        lcd_display(1);
                        if (timer_flags.check_temperature == 1) //1.0.1
                        {
                            timer_flags.check_temperature = 0;
                            read_temperature();
                        }
                        else{}                      
                    }
                    else{}                   
                }
                else{}
            }
            else
            {
                if (((timer2_tick - cr_time_stamp) > 2000) && (mode_flags.op_sub_mode == CON_DISPENSE))
                {
                    water_dispense_error();
                    if (dispense_trail == 'E')
                        mode_flags.op_sub_mode = CON_HALT;
                    else
                        cr_time_stamp = timer2_tick;
                }
                else
                {
                    if ((mode_flags.op_sub_mode == CON_HALT)&&(dispense_trail >= 2))
                    {
                        dispense_trail = 'E';
                        water_dispense_error();
                    }
                }
            }
        }// end of while()

        SV_VALVE_OFF;
        output_flow_count=0;
        consumer_flags.dispense_button = 0; //@	consumer_flags.dispense_button = 0;
        menu_flags.any_button_press = 0;
        if (mode_flags.op_mode != ERROR_MODE)
        {
            strcpy(lcd_data, "   Powered by"); //v2.2.7
            lcd_display(1);
            strcpy(lcd_data, "PIRAMAL SARVAJAL");
            lcd_display(2);
            //timer2_sec_delay(logout_idle_time);
        }
        else{}       
        balance_deduct = dispense_litre;
        send_server_response(DEV_BALANCE_DEDUCT);
        //total_dispense_litre += balance_deduct;
        tank_low_level_update(balance_deduct);
        total_water_dispense_store(balance_deduct);

        SV_VALVE_OFF;
        acc_dispense_capacity = 0;
        litre_transcation = 0;

        //---clear_lcd();
        //---LCD_BACKLITE_OFF;
        consumer_flags.dispense_button = 0;
        menu_flags.any_button_press = 0;
        //			consumer_flags.dispense_type = 0;
        consumer_flags.rfid_read = LOW;
        consumer_flags.card_detect = LOW; //v1.0.0
        rfid_card_type = 0;
        rfid_reader_counter = 0;
        dispense_litre = 0;
        dispense_trail = 0;
        button_detect = 0;
        peripheral_flags.tds_update = TRUE;
        system_flags.server_response = FALSE;
    }//----LP
    timer2_sec_delay(1);
    if (mode_flags.op_mode != ERROR_MODE)
    {
        lcd_init();
        strcpy(lcd_data, "PRESS ANY BUTTON"); //v2.2.7
        lcd_display(1);
        if (peripheral_flags.cold_water == TRUE) //chiller
            strcpy(lcd_data, "FOR COLD WATER  ");
        else if (peripheral_flags.warm_water == TRUE) //heater	//v2.3.6
            strcpy(lcd_data, "FOR WARM WATER  ");
        else
            strcpy(lcd_data, "FOR CLEAN WATER "); //v2.2.3
        lcd_display(2);
    }
    else{}   
    output_flow_count=0;
    peripheral_flags.lcd_refresh = TRUE;
    LED4_OFF;
    
}



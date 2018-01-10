#define USE_AND_OR /* To enable AND_OR mask setting */

#include<outcompare.h>
#include<timer.h>
#include<ports.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include<math.h> 
#include "serial_uart.h"
#include "variable.h"
#include "main.h"
#include "peripherial.h"
#include "mcu_timer.h"
#include "ext_lcd.h"
//#include "MCP23017.h"
#include "communicate.h"
//#include "DS2781.h"
#include "ext_eeprom.h"

void __attribute__ ((interrupt,no_auto_psv)) _OC1Interrupt(void)
{
   OC1_Clear_Intr_Status_Bit;
}

void peripheral_handler(void) //GIT testing 
{
	peripheral_flags.peripheral_interrupt = FALSE;
//--------LLS Sense
	lls_sense_processing();
//--------HLS Sense
	hls_sense_processing();
	if(((min_tick-sleep_time_stamp)>=300)&&(peripheral_flags.sleep_mode_flag==TRUE))	//check after 7 hrs(420 mins)
	{
		peripheral_flags.sleep_mode_flag = FALSE;
		eeprom_write(SER_SLEEP_MODE_FLAG);
		sleep_time_stamp = 0;
	}else{}
}

void lls_sense_processing(void){    //V2.3.1.c  // GIT testing not confirmed
	peripheral_flags.lls_pin = LLS_PIN_CHECK;
	timer2_ms_delay(50);
	if((peripheral_flags.lls_pin==LOW)&&(peripheral_flags.lls_processing==FALSE) && ((peripheral_flags.lls_last_second_state != TANK_LOW_2))  )    //&&(peripheral_flags.lls_trigger==FALSE))
	{                
                lls_time_stamp = sec_tick;  //timer2_tick; //v3.1.1.F		
                peripheral_flags.lls_last_first_state = TANK_LOW_1;//v3.1.2.C
                peripheral_flags.lls_processing = TRUE;	
	}
	else if((peripheral_flags.lls_pin==LOW)&&(peripheral_flags.lls_processing==TRUE)&&((sec_tick-lls_time_stamp)>15) )  //&&(peripheral_flags.lls_trigger==FALSE))  //v3.1.1.F
	{		
		peripheral_flags.lls_trigger = TRUE;
		lls_time_stamp = 0;               
                peripheral_flags.lls_processing = FALSE;                //v3.1.2.C
               
		if (peripheral_flags.lls_hls_en_dis>=2)   // && (peripheral_flags.lls_last_second_state == TANK_HIGH_2)  )  // (peripheral_flags.lls_last_second_state==0)
		{			
                    lcd_init();
                    strcpy(lcd_data,"TANK WATER");
                    lcd_display(1);
                    strcpy(lcd_data,"LEVEL LOW");
                    lcd_display(2);
                    buzzer(ERR01);
                    send_server_response(DEV_LLS_TRIGGER);						
                    send_server_response(DEV_HB_MSG);
                    timer2_sec_delay(error_display_time);
		}else{}   
                 peripheral_flags.lls_last_second_state = TANK_LOW_2;           //v3.1.2.C
                 peripheral_flags.lls_last_first_state = 0;                     //v3.1.2.C
                
	}else{}

	if((peripheral_flags.lls_pin==HIGH)&&(peripheral_flags.lls_processing==FALSE) && ((peripheral_flags.lls_last_second_state != TANK_HIGH_2))  )//&&(peripheral_flags.lls_trigger==TRUE))
	{               
                lls_time_stamp = sec_tick;   //v3.1.1.F		
                peripheral_flags.lls_last_first_state = TANK_HIGH_1;  //v3.1.2.C
                peripheral_flags.lls_processing = TRUE;
              
	}
	else if((peripheral_flags.lls_pin==HIGH)&&(peripheral_flags.lls_processing==TRUE)&&((sec_tick-lls_time_stamp)>15) )      //&&(peripheral_flags.lls_trigger==TRUE))  //v3.1.1.F
	{
		peripheral_flags.lls_trigger = FALSE;
		lls_time_stamp = 0;
		peripheral_flags.chiller_heater_lls_trigger=FALSE;  //v2.2.6	//v3.1.2.H              
                peripheral_flags.lls_processing = FALSE;             //v3.1.2.C
		
		if(tank_low_level_litre<lit_empty_tank)		//v2.2.4 - To re-init "tank_low_level_litre" when water rises above LLS
		{                   
			if(error_flags.tank_empty==TRUE)
			{
				error_stack[TANK_EMPTY_ER] = FALSE;
				error_flags.tank_empty = FALSE;
			}else{}
	
			if((data_validation(SER_LT_EMPTY_TANK))==0)
			{
				tank_low_level_litre = lit_empty_tank;
				eeprom_write(SER_TANK_LOW_LEVEL_STATUS);
			}
			else{}
		}else{}
                peripheral_flags.lls_last_second_state = TANK_HIGH_2;     //v3.1.2.C
                peripheral_flags.lls_last_first_state = 0;                //v3.1.2.C 
	}else{}
}

void hls_sense_processing(void){
	peripheral_flags.hls_pin = HLS_PIN_CHECK;
	timer2_ms_delay(50);  //v3.1.1.F
	if((peripheral_flags.hls_pin==LOW)&&(peripheral_flags.hls_processing==FALSE)&&(peripheral_flags.hls_trigger==FALSE))
	{
		hls_time_stamp = sec_tick;//  timer2_tick  //v3.1.1.F
		peripheral_flags.hls_processing = TRUE;
		
	}
	else if((peripheral_flags.hls_pin==LOW)&&(peripheral_flags.hls_processing==TRUE)&&((sec_tick-hls_time_stamp)>15)&&(peripheral_flags.hls_trigger==FALSE))
	{
		peripheral_flags.hls_trigger = TRUE;		
		hls_time_stamp = 0;
		
		//&&(mode_flags.op_mode!=REFILL_MODE)&&(mode_flags.op_mode!=DUMP_MODE))
		if((peripheral_flags.lls_hls_en_dis==1) || (peripheral_flags.lls_hls_en_dis==3))
		{
			//send_server_response(DEV_LLS_TRIGGER);						
			//send_server_response(DEV_HB_MSG);
			
			lcd_init();
			strcpy(lcd_data,"TANK WATER");
			lcd_display(1);
			strcpy(lcd_data,"LEVEL HIGH..!!");
			lcd_display(2);
			buzzer(ERR01);
			timer2_sec_delay(error_display_time);
		}else{}

	}else{}

	if((peripheral_flags.hls_pin==HIGH)&&(peripheral_flags.hls_processing==FALSE)&&(peripheral_flags.hls_trigger==TRUE))
	{
		hls_time_stamp = sec_tick;  //timer2_tick  //v3.1.1.F
		peripheral_flags.hls_processing = TRUE;
	}
	else if((peripheral_flags.hls_pin==HIGH)&&(peripheral_flags.hls_processing==TRUE)&&((sec_tick-hls_time_stamp)>15)&&(peripheral_flags.hls_trigger==TRUE))
	{
		peripheral_flags.hls_trigger = FALSE;
		hls_time_stamp = 0;
	}else{}
}


void buzzer(unsigned char buzzer_state){
	unsigned int delay=0;
	unsigned char loop=0;
	UINT config1,config2;
    UINT32 value1,value2;

	mPORTASetBits(0x0080);	//Enable PWM Opto

	//---OpenTimer3(T3_ON|T3_PS_1_1|T3_SOURCE_INT,0x0FFF); //Timer is configured PWM	
	OpenTimer3(T3_ON|T3_PS_1_1|T3_SOURCE_INT|T3_IDLE_STOP,0x0FFF); //Timer is configured PWM	//v2.2.4 (T3_IDLE_STOP  added)

	config1 = OC_IDLE_CON | OC_TIMER3_SRC | OC_FAULT0_IN_DISABLE | OC_PWM_CENTRE_ALIGN;
	config2 = OC_SYNC_TRIG_IN_TMR3;//OC_SYNC_TRIG_IN_CURR_OC;	//0x0000;	//
	value1 = 0x09FF;	//PWM duty cycle to be stored in OCxRS
    value2 = 0x0000;	//PWM initial duty cycle OCxR

	switch(buzzer_state)
	{
		case OFF:	//0
				value1 = 0;	//PWM duty cycle to be stored in OCxRS
				delay = 0;
				loop = 0;
			break;

		case ERR01:	//1
				delay = 500;
				loop = 2;
			break;		

		case ERR02:	//2
				delay = 200;
				loop = 4;
			break;		

		case AUT01:	//3
				//delay = 500;
				delay = 300;
				loop = 1;
			break;		

		case AUT02:	//4
				delay = 200;
				loop = 4;
			break;		

		case AUT03:	//5
				delay = 100;
				loop = 4;
			break;		

		case DISP:	//6
				delay = 100;	
				//---loop = 2;
				loop = 1;	//v2.2.8
			break;		

		case HALT:	//7
				delay = 1000;
				loop = 1;
			break;		

		default:
			break;
	}
	
	if(peripheral_flags.pag_buzzer_control==TRUE)
	{
		for(; loop>0; loop--)
		{
			OpenOC1(config1,config2,value1,value2);
			timer2_ms_delay(delay);
			CloseOC1();	
			timer2_ms_delay(delay);
		}
	}
	else{}

	mPORTAClearBits(0x0080);	//disable PWM Opto
}
void scan_switch(void){

	if(!PORTEbits.RE8){
		button_detect_timestamp = timer2_tick;
		while((!PORTEbits.RE8));			//Key Debounce
		if((timer2_tick-button_detect_timestamp)>KEY_DEBOUNCE_DELAY)
			button_detect = ENTER_DETECT;
	}
	else if(!PORTEbits.RE9){
		button_detect_timestamp = timer2_tick;
		while(!PORTEbits.RE9);			//Key Debounce
		if((timer2_tick-button_detect_timestamp)>KEY_DEBOUNCE_DELAY)
			button_detect = DOWN_DETECT;
	}
	else if(!PORTBbits.RB5){
		button_detect_timestamp = timer2_tick;
		while(!PORTBbits.RB5);			//Key Debounce
		if((timer2_tick-button_detect_timestamp)>KEY_DEBOUNCE_DELAY)
			button_detect = UP_DETECT;
	}
	
//	else if(!PORTDbits.RD0){
//		button_detect_timestamp = timer2_tick;
//		while(!PORTDbits.RD0);			//Key Debounce
//		consumer_flags.dispense_button = 1;
//				//if((timer2_tick-button_detect_timestamp)>KEY_DEBOUNCE_DELAY)			
//	}
	else{
		//button_detect = 0;			
	}


}

void tds_check(void)
{
	//if((temperature_reading<3.0)&&(gsm_flags.temp_sensor_failure_sent==FALSE))
/*	if((DS_sensor_reading>65534)&&(gsm_flags.temp_sensor_failure_sent==FALSE)) //v3.1.1.F
	{
		DS_temperature = 0; 	//v2.2.7
		strcpy(temp_value,"     ");
		strcpy(temp_value,"ERR");
		temp_value[5]=0;
		gsm_flags.temp_sensor_failure_sent = TRUE;
		
		lcd_init();
		strcpy(lcd_data,"TEMP SENSOR FAIL");
		lcd_display(1);
		strcpy(lcd_data,"CONTACT SARVAJAL");
		lcd_display(2);
		buzzer(ERR01);
		timer2_sec_delay(error_display_time);
	
		//send_server_response(DEV_TEMP_SENSOR_FAILURE);
		//send_server_response(DEV_HB_MSG);		
		
	}
	else{}
*/

	if(peripheral_flags.tds_en_dis)
	{
		if(tds_reading>=max_tds)
		{
			error_flags.tds_high = TRUE;
			error_stack[HIGH_TDS_ER] = TRUE;
		}
		else
		{
			if((error_flags.tds_high==TRUE) && (tds_reading<=(max_tds-TDS_OFFSET)))
			{
				error_flags.tds_high = FALSE;
				error_stack[HIGH_TDS_ER] = FALSE;
				error_flags.tds_error_sent = FALSE;
			}
			else{}		
		}
	
		if(tds_reading<=min_tds)
		{
			error_flags.tds_low = TRUE;
			error_stack[LOW_TDS_ER] = TRUE;
		}
		else
		{
			if((error_flags.tds_low==TRUE) && (tds_reading>=(min_tds+TDS_OFFSET)))
			{
				error_flags.tds_low = FALSE;
				error_stack[LOW_TDS_ER] = FALSE;
				error_flags.tds_error_sent = FALSE;
			}
			else{}
		}		
	}
	else
	{
		error_flags.tds_low = FALSE;
		error_stack[LOW_TDS_ER] = FALSE;
		error_flags.tds_high = FALSE;
		error_stack[HIGH_TDS_ER] = FALSE;
		error_flags.tds_error_sent = FALSE;
	}

	if((error_flags.tds_low==TRUE) && (peripheral_flags.tds_en_dis==TRUE) && (error_flags.tds_error_sent==FALSE))
	{
		system_error = LOW_TDS_ER;
		//error_stack[LOW_TDS_ER] = TRUE;
		display_error_mode();
		buzzer(ERR01);
		system_error_check();
		send_server_response(DEV_TDS_LOW);
		send_server_response(DEV_HB_MSG);
		error_flags.tds_error_sent = TRUE;
	}
	else{}

	if((error_flags.tds_high==TRUE) && (peripheral_flags.tds_en_dis==TRUE) && (error_flags.tds_error_sent==FALSE))
	{
		system_error = HIGH_TDS_ER;  //3.1.0
	//	error_stack[HIGH_TDS_ER] = TRUE;
		display_error_mode();
		buzzer(ERR01);
		system_error_check();
		send_server_response(DEV_TDS_HIGH);
		send_server_response(DEV_HB_MSG);
		error_flags.tds_error_sent = TRUE;
	}
	else{}
}
void display_tank_low_alert(void)		//v2.2.3
{
	lcd_init();
	strcpy(lcd_data,"TANK LEVEL LOW");
	lcd_display(1);
	if(tank_low_level_litre>1)
		sprintf(lcd_data,"%d L",tank_low_level_litre);
	else
		strcpy(lcd_data,"CONTACT SARVAJAL");

	lcd_display(2);
	buzzer(ERR01);
	timer2_sec_delay(error_display_time);	
}

void tank_low_level_update(float litre)		//v2.2.3
{
    unsigned int disp_lit = 0;
    if ((peripheral_flags.lls_hls_en_dis >= 2)&&(peripheral_flags.lls_trigger == TRUE))
    {
        disp_lit = ceilf(litre);
        if (tank_low_level_litre > disp_lit)
        {
            tank_low_level_litre -= disp_lit;

            if (tank_low_level_litre < 1)
                tank_low_level_litre = 1;
            else{}           
            if ((data_validation(SER_LT_EMPTY_TANK)) == 0)
                eeprom_write(SER_TANK_LOW_LEVEL_STATUS);
            else{}           
        }
        else
        {
            tank_low_level_litre = 1; //TANK EMPTY

            if (peripheral_flags.chiller_heater_lls_trigger == FALSE)
            {
                if ((data_validation(SER_LT_EMPTY_TANK)) == 0)
                    eeprom_write(SER_TANK_LOW_LEVEL_STATUS);
                else{}              
            }
            peripheral_flags.chiller_heater_lls_trigger = TRUE; //v2.2.6  // For making the chiller off  after tank low level lit. becomes 1	//v3.1.2.H
        }
    }
    else{}   
}

void temperature_failure(){			//v3.1.1.F
	DS_temperature = 0; 	//v2.2.7
	strcpy(temp_value,"     ");
	strcpy(temp_value,"ERR");
	temp_value[5]=0;
	avg_temperature_array[0]='O';avg_temperature_array[1]='F';avg_temperature_array[2]='F';	//v2.3.5
	temperature_reading_counter=0;

	lcd_init();
	strcpy(lcd_data,"TEMP SENSOR FAIL");
	lcd_display(1);
	strcpy(lcd_data,"CONTACT SARVAJAL");
	lcd_display(2);
	buzzer(ERR01);
	timer2_sec_delay(error_display_time);

	gsm_flags.temp_sensor_failure_sent = TRUE;
	send_server_response(DEV_TEMP_SENSOR_FAILURE);
	send_server_response(DEV_HB_MSG);	
}

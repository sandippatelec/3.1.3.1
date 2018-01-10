#define USE_AND_OR /* To enable AND_OR mask setting */

#include "serial_uart.h"
#include "variable.h"
#include "mcu_timer.h"
#include "mcu_adc.h"
#include "peripherial.h"
#include "ext_lcd.h"
#include "1wire_temperature.h"
#include<ports.h>
#include<timer.h>

void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void)
{
    T1_Clear_Intr_Status_Bit; //interrupt every 1 sec
    //ClrWdt();
    //	timer1_tick++;
    //	sec_tick++;
    //	if((sec_tick%60)==0)
    //		min_tick++;
    //	else{}
}

void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void)
{
    T2_Clear_Intr_Status_Bit; //Timer2 is configured for 1 msec
    ClrWdt();

    timer2_tick++;
    if (peripheral_flags.sv_on == TRUE)
    {
        if ((peripheral_flags.temp_en_dis == 1) &&((timer2_tick % 900) == 0)) //750ms conversion time		//v1.0.1	
        {
            timer_flags.check_temperature = 1;
        }
        else{}       
    }
    if( (consumer_flags.card_detect_first_time==1) && (timer2_tick -Rfid_detect_timestamp>100) )  //3.1.2  //do_finalize //50 3.1.3.B
    {
        if(PORTDbits.RD0==0)
        {
              consumer_flags.card_detect = HIGH;
               general_time_stamp_logout = sec_tick;
            if (mode_flags.op_mode != ULTRA_SUPER_ADMIN_MODE)
            {
            mode_flags.forced_function_state = TRUE; //0.0.1
            function_state = ST_CARD_DETECTED; //0.0.1
            }
        }
        consumer_flags.card_detect_first_time=0;                     
    }else{}

    if ((timer2_tick % 1000) == 0)
    {
        sec_tick++;

        if ((sec_tick % 5) == 0)                        //v2.3.4
        { 
            timer_flags.sms_check = 1;
        }else{}
        
        if ((sec_tick % 7) == 0)                        //1.0.1
        { 
            timer_flags.system_error_flag = 1;
        }else{}
        
        if ((sec_tick % 9) == 0)                       //1.0.1 sec_tick%9
        {
            timer_flags.mode_refresh_check = 1;
            timer_flags.analog_sensor_update = 1;	    
        }else{}
        
        if ((sec_tick % 11) == 0)
        { //v2.3.4            
            timer_flags.coin_communication_retry = 1; //v2.3.4
        }else{}
        
        if ((sec_tick % 12) == 0)  //v3.1.2
        { 
            timer_flags.chiller_heater_control = 1;           

        } else{}
     
        if ((sec_tick % 15) == 0)  //v3.1.1.D  //
        {
          gsm_flags.scan_sms = 1;  
        } else{}

        if ((sec_tick % 60) == 0) //v3.1.1.E
        {
            gsm_flags.signal_strength_check=1;
        }else{}    
   


	if (((mode_flags.op_mode == SCHOOL_MODE) || (mode_flags.op_mode == CALIBRATION_MODE))&&((menu_flags.any_button_press == 1) || (consumer_flags.dispense_button == 1) || (peripheral_flags.sv_on == TRUE)))
	{
            // if((sec_tick - sch_time_stamp)>=(dispense_timeout*1000))		//v2.2.3  //timer2_tick -sch_time_stamp
	    if ((sec_tick - sch_time_stamp) >= ((int) ((dispense_timeout * 2) * sch_disp_fraction)))		
	    {
            SV_VALVE_OFF;
            menu_flags.any_button_press = 0;
            consumer_flags.dispense_button = 0;
            dispense_button_count = 0;
            total_dispense_litre += (output_flow_count / ((float) (out_flow_calibration))); //v2.2.3
            output_flow_count = 0;
            consumer_flags.output_flow_sensed = 0;
	    }
	    else if (((sec_tick - sch_time_stamp) >= 2)&&(consumer_flags.output_flow_sensed == FALSE)) //timer2_tick -sch_time_stamp >=1500  //1.0.1
	    {
            switch (dispense_trail)
            {
                case 1:
                case 3:
                    sch_time_stamp = sec_tick; //sch_time_stamp = timer2_tick; 1.0.1
                    SV_VALVE_OFF;
                    dispense_trail++;
                    break;

                case 2:
                case 4:
                    sch_time_stamp = sec_tick; //sch_time_stamp = timer2_tick; 1.0.1
                    SV_VALVE_ON;
                    dispense_trail++;
                    break;

                case 5:
                    // v3.0.0
                    //@						if(sv_on_current<sv_current_error_count)		//785  //1.0.1
                    //						{
                    //							system_error = SV_ER;
                    //							error_stack[SV_ER] = TRUE;
                    //						}
                    //						else
                    //						{
                    //							system_error = FLOW_SENS_ER;
                    //							error_stack[FLOW_SENS_ER] = TRUE;
                    //@						}                                                            //1.0.1
                    system_error = FLOW_SENS_ER;
                    error_stack[FLOW_SENS_ER] = TRUE;
                    SV_VALVE_OFF;
                    button_detect = 0; //consumer_flags.dispense_button = 0;
                    menu_flags.any_button_press = 0;
                    dispense_button_count = 0;
                    output_flow_count = 0;
                    consumer_flags.output_flow_sensed = 0;
                    mode_flags.forced_function_state = TRUE;
                    function_state = ST_BUTTON_PRESSED;
                    //dispense_trail++;
                    dispense_trail = 'E'; //v2.2.3

                    break;

                default:
                    break;
            }
	    }
	    else{}

	    consumer_flags.output_flow_sensed = FALSE;
	}
	else{}	

        if (peripheral_flags.lls_processing == TRUE)  //V2.3.1.c
        {
            //peripheral_flags.lls_pin = LLS_PIN_CHECK;
//            if ((peripheral_flags.lls_trigger == FALSE)&&(peripheral_flags.lls_pin == HIGH))
//            peripheral_flags.lls_processing = FALSE;
//
//            else if ((peripheral_flags.lls_trigger == TRUE)&&(peripheral_flags.lls_pin == LOW))
//            peripheral_flags.lls_processing = FALSE;
//            else{}	

            if ((peripheral_flags.lls_last_first_state  == TANK_LOW_1)&&(LLS_PIN_CHECK == HIGH))
                peripheral_flags.lls_processing = FALSE;   
            
            else if ((peripheral_flags.lls_last_first_state  == TANK_HIGH_1)&&(LLS_PIN_CHECK == LOW))
                peripheral_flags.lls_processing = FALSE;  
            else{}
        }
        else{}	

        if (peripheral_flags.hls_processing == TRUE)
        {
            peripheral_flags.hls_pin = HLS_PIN_CHECK;
            if ((peripheral_flags.hls_trigger == FALSE)&&(peripheral_flags.hls_pin == HIGH))
            peripheral_flags.hls_processing = FALSE;

            else if ((peripheral_flags.hls_trigger == TRUE)&&(peripheral_flags.hls_pin == LOW))
            peripheral_flags.hls_processing = FALSE;
            else{}	 
        }
        else{}	

        if ((sec_tick % 60) == 0)
        {
            min_tick++;
            peripheral_flags.peripheral_interrupt = TRUE; 

            if ((min_tick % idle_msg_freq) == 0) //v2.2.4
            system_flags.send_idle_msg = TRUE;
            else{}	  

            if ((min_tick % 30) == 0) //v2.3.5	//2.3.5.A
            {
                timer_flags.coin_communication_check = 1;
                system_flags.system_stability =FALSE; //v3.1.2.J  //needtodelete and rewrite on every 30min
            }
            else{}

            if ((min_tick % 60) == 0)
            {
                validation_error_counter = 0;
                unknown_access_count = 0;
            }
            else{}

            if ((min_tick % 195) == 0)
            { // //3 hour 15Minute = 195 min   // 3 hour= 10800 sec //180min 
                rtc_flag.sync_by_time = 1;
            }
            else{}
            
            if( ( (min_tick % modem_status_check_min) == 0) && (timer_flags.modem_status_check_en_dis==1 ) )//v3.1.2.J        
            {
                timer_flags.modem_status_check =1;  //3.1.2.J
            } else{}
        }
        else{}

        if ((school_mode_en) && (mode_flags.op_mode == SCHOOL_MODE))
        {
            if ((sec_tick % (idle_msg_freq * 30)) == 0)
            sch_disp_send_server = TRUE;
            else{}	   
        }
        else{}

        if (((sec_tick % 30) == 0) && (mode_flags.op_mode == DIAGNOSTIC_MODE) && (mode_flags.run_diagnostic == FALSE))
            mode_flags.run_diagnostic = TRUE;
        else{}
	
    }
    else{}
  
}

void timer1_init(void)
{
    ConfigIntTimer1(T1_INT_ON | T1_INT_PRIOR_1); //Enable Interrupt
    //-----16 Mhz Clk	
    OpenTimer1(T1_ON | T1_PS_1_256, 0x7A12); //2sec=F424, 1sec=0x7A12

    //Timer is configured for 100 usec = 0x320

    //-----8 Mhz Clk	
    timer1_tick = 0;
}

void timer2_init(void)
{
    ConfigIntTimer2(T2_INT_ON | T2_INT_PRIOR_2); /*Enable Interrupt*/
    //-----16 Mhz Clk	
    OpenTimer2(T2_ON, 0x1F40); //Timer is configured for 1 msec

    //-----8 Mhz Clk
    timer2_tick = 0;
}

/*----void timer3_init(void)
{
//-----16 Mhz Clk	
	OpenTimer3(T3_ON|T3_PS_1_1|T3_SOURCE_INT,0x7FFF); //Timer is configured PWM
}---*/

/*void timer45_init(void)		//v2.2.4
{
	ConfigIntTimer45(T45_INT_PRIOR_3|T45_INT_ON);

	OpenTimer45(T45_ON|T45_IDLE_CON|T45_PS_1_1|T45_SOURCE_INT,0xFFFFFFFF);
}*/


void timer2_sec_delay(unsigned int sec) //multiple of 1 sec
{
    timer2_ms_delay((sec * 1000));
}

void timer1_sec_delay(unsigned int sec) //multiple of 1 sec
{
    unsigned int time_ms_stamp = 0;

    time_ms_stamp = timer1_tick;
    while ((timer1_tick - time_ms_stamp) < sec);
}

void timer2_ms_delay(unsigned int count) //multiple of 1 msec
{
    unsigned int time_ms_stamp = 0;

    time_ms_stamp = timer2_tick;
    while ((timer2_tick - time_ms_stamp) < count);
}




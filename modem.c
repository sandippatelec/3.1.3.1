#define USE_AND_OR /* To enable AND_OR mask setting */

#include<ports.h>
#include<timer.h>
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#include "main.h"
#include "modem.h"
#include "variable.h"
#include "serial_uart.h"
#include "server.h"
#include "mcu_timer.h"
#include "communicate.h"
#include "ext_lcd.h"
#include "peripherial.h"
#include "ext_eeprom.h"
#include "coin_uca2.h"	//v2.2.9 
#include "DS1307.h"
#include "ext_eeprom.h"
#include "mode.h"
#include "1wire_temperature.h" //3.1.2.V13

// Modem code 
// Developed by : Sandip Patel
// Tested OK

void network_check(void)  // test git cmd
{
    unsigned char loop = 0; //,count=0;
    char string_buffer[20] = {0};
    char* local_ptr = 0;

    while (BusyUART2());
    WriteUART2((unsigned int) 26); //Clear pending data in modem if any

    for (loop = 0; loop<sizeof (string_buffer); loop++)
        string_buffer[loop] = 0;

    lcd_init();
    strcpy(lcd_data, "SIM CARD CHECK");
    lcd_display(1);

    serial_string_send("AT+CSMINS?\r\n");
    if (check_modem_reply("CSMINS:", 7, 2000)) //Actual response "+CSMINS: 0,1"
    {
        strcpy(string_buffer, "CSMINS:");
        local_ptr = strstr(Rxdata, string_buffer);
        if (local_ptr != 0) //string matched
        {
            loop = 0;
            while ((*local_ptr++ != ',') && (loop <= 15))
            {
                loop++;
            }

            if (*local_ptr == '1')
            {
                gsm_flags.sim_card_check = TRUE;
                strcpy(lcd_data, "PRESENT");
            }
            else if (*local_ptr == '0')
            {
                gsm_flags.sim_card_check = FALSE;
                strcpy(lcd_data, "ABSENT");
            }
            else
            {
                gsm_flags.sim_card_check = FALSE;
                strcpy(lcd_data, "COMM. ERROR");
            }
        }
        else
        {
        }

    }
    else
    {
        gsm_flags.sim_card_check = FALSE;
        //---strcpy(lcd_data,"SIM ERROR");
        strcpy(lcd_data, "MODEM ERROR     "); //v2.3.1
    }
    lcd_display(2);
    //timer2_sec_delay(error_display_time);

    WriteUART2((unsigned int) 26); //clear any pending data to server
    timer2_ms_delay(1000); //delay
    network_state = INITIAL;

    serial_string_send("AT+CREG?\r\n");
    check_modem_reply("CREG:", 5, 2000); //Response "+CREG: 0,1"
    timer2_sec_delay(1);

    /*-----------------------------------------
            AT+CREG?
            ---check SIM card registration 0-5
            0-not reg.
            1-reg.
            2-not reg. but searching
            3-reg. denied
            4-unknown
            5-reg. roaming
    ------------------------------------------*/

    strcpy(string_buffer, "CREG:");
    local_ptr = strstr(Rxdata, string_buffer);

    lcd_init();
    strcpy(lcd_data, "NETWORK CHECK");
    lcd_display(1);

    if (local_ptr != 0) //string matched
    {
        loop = 0;
        while ((*local_ptr++ != ',') && (loop <= 10))
        {
            loop++;
        }
        network_state = *local_ptr;
    }
    else
    {
        strcpy(string_buffer, "ERROR");
        local_ptr = strstr(server_response_string, string_buffer);
        if (local_ptr != 0) //string matched
        {
            strcpy(lcd_data, "MODEM ERROR");
            modem_reset();
        }
        else
        {
        }

        strcpy(lcd_data, "NO RESPONSE");
        lcd_display(2);
        timer2_sec_delay(error_display_time);
    }
    strcpy(lcd_data, "            ");
    lcd_display(2);

    switch (network_state)
    {

    case NOT_REGISTERED:
        modem_reset_count++;
        strcpy(lcd_data, "NOT REGISTERED");
        lcd_display(2);
        break;

    case REGISTERED:
    case REG_ROAMING:
        //modem_reset_count = 0;
        check_network(TRUE);

        break;

    case NETWORK_SEARCH:
        modem_reset_count++;
        strcpy(lcd_data, "SEARCHING...");
        break;

    case REG_DENIED:
        modem_reset_count++;
        strcpy(lcd_data, "REG. DENIED");
        lcd_display(1);
        timer2_sec_delay(error_display_time); //delay

        strcpy(lcd_data, "SIM CARD ERROR");
        lcd_display(1);
        strcpy(lcd_data, "INVALID SIM");
        lcd_display(2);
        break;

    case INITIAL:
        modem_reset_count++;
        strcpy(lcd_data, "CONNECTION ERROR");
        break;

    default:
        modem_reset_count++;
        strcpy(lcd_data, "UN ERROR");
        break;
    }

    if (modem_reset_count != 0)
    {
        lcd_display(2);
        if (modem_reset_count >= modem_reset_tolerance)
            modem_reset();
        else
            timer2_sec_delay(error_display_time); //delay
    }
    else
    {
    }

    //LED_updates();	//debug LED update

}

unsigned char check_network(unsigned char display)
{
    unsigned char loop_1 = 0, loop_2 = 0, result = 0;
    char data_buffer[10] = {0};
    char* local_ptr = 0;

    //-----Check Service Provider
    serial_string_send("AT+CSPN?\r\n");
    check_modem_reply("+CSPN", 5, 1000);

    strcpy(data_buffer, "+CSPN:");
    local_ptr = strstr(Rxdata, data_buffer);
    if (local_ptr != 0) //string matched
    {
        *local_ptr += 5;

        for (loop_1 = 0; loop_1 < (sizeof (network_service_provider)); loop_1++)
            network_service_provider[loop_1] = 0;

        for (loop_1 = 0; loop_1 < 10; loop_1++)
        {
            if (*++local_ptr == '"')
                break;
            else
            {
            }
        }
        ++local_ptr;
        loop_2 = 0;
        for (loop_1 = 0; loop_1 < 25; loop_1++) //3.2.0 //10 to 25
        {
            network_service_provider[loop_2++] = *local_ptr;
            if (*++local_ptr == '"')
                break;
            else{}
        }
        result = TRUE;
    }
    else
        result = FALSE;

    if (display == TRUE)
    {
        strcpy(lcd_data, "                ");
        if (result == TRUE)
        {
            strcpy(lcd_data, network_service_provider);
            if (network_state == REG_ROAMING)
                lcd_data[15] = 'R';
            else
            {
            }
        }
        else
            strcpy(lcd_data, "ERROR");

        lcd_display(2);
        timer2_sec_delay(error_display_time);
        clear_lcd();
    }
    else
    {
    }
    //-----Check Signal Strength

    //if(check_network_strength(TRUE))
    if (check_network_strength(display))
        result = TRUE;
    else
        result = FALSE;

    return result;
}

unsigned char check_network_strength(unsigned char display)
{
    unsigned char loop = 0, cnt = 0, response = 0;
    char string_buffer[10] = {0};
    char* local_pointer = 0;

    if (display == TRUE)
    {
        //clear_lcd();
        lcd_init();
        strcpy(lcd_data, "SIGNAL QUALITY  ");
        lcd_display(1);
    }
    else
    {
    }


    for (loop = 0; loop<sizeof (network_signal_strength); loop++)
        network_signal_strength[loop] = 0;

    timer2_sec_delay(1); //delay
    serial_string_send("AT+CSQ\r\n");
    check_modem_reply("+CSQ:", 5, 1000);

    timer2_ms_delay(200); //200ms delay
    strcpy(string_buffer, "CSQ:");
    local_pointer = strstr(Rxdata, string_buffer);
    if (local_pointer != 0) //string matched
    {
        cnt = 0;
        for (loop = 0; loop < 10; loop++)
        {
            if (*++local_pointer == ',')
                break;
            else
            {
            }

            if ((*local_pointer >= '0') && (*local_pointer <= '9'))
                network_signal_strength[cnt++] = *local_pointer;
            else
            {
            }
        }
        response = TRUE;
    }
    else
        response = FALSE;

    if (display == TRUE)
    {
        strcpy(lcd_data, "                ");
        if (response == TRUE)
            strcpy(lcd_data, network_signal_strength);
        else
            strcpy(lcd_data, "ERROR");

        lcd_display(2);
        timer2_sec_delay(error_display_time);
    }
    else
    {
    }

    return response;
}

void modem_reset(void)
{
    //unsigned char temp_array[15]={0};

    modem_reset_count = 0;

    if ((mcu_auto_reset_counter >= 3) && (mcu_auto_reset_counter <= 4)) //added on 8/11/12
    {
        read_eeprom_parameter(SER_SERVER_IP);
        timer2_ms_delay(100); //delay
        read_eeprom_parameter(SER_SERVER_PORT);
        timer2_ms_delay(100); //delay
        read_eeprom_parameter(SER_SERVER_APN);
        timer2_ms_delay(100); //delay
        mcu_auto_reset_counter++;
    }//added on 23/8/12
    else if (mcu_auto_reset_counter >= 6)
    {
        mcu_auto_reset_counter = 0;
        abort(); //reset uC
    }
    else
        mcu_auto_reset_counter++;

    lcd_init();
    strcpy(lcd_data, "RESTARTING MODEM");
    lcd_display(1);
    strcpy(lcd_data, "PLS. WAIT");
    lcd_display(2);

    UART2_init(); //serial UART initialized
    timer2_ms_delay(200); //delay

    //-------------------------		v2.2.3	//setting phone functionality to default
    modem_powerdown(0);

    /*strcpy(temp_array,"AT+IPR=0\r\n");			
    serial_string_send(temp_array);
    check_modem_reply("OK",2,2000);

    timer2_sec_delay(error_display_time);

    strcpy(temp_array,"AT+CFUN=1\r\n");			
    serial_string_send(temp_array);
    check_modem_reply("OK",2,2000);*/
    //-------------------------

    if (error_flags.modem_sw_reset == FALSE)
    {
        error_flags.modem_sw_reset = TRUE;
        serial_string_send("AT+CPOWD=1\r\n");
        timer2_ms_delay(1000); //delay
        //---UART2_init();	//serial UART initialized
        timer2_ms_delay(5000); //delay

        check_modem_reply("CPIN: READY", 11, 25000);
    }
    else
    {
        error_flags.modem_sw_reset = FALSE;
        MODEM_OFF;
        //timer2_ms_delay(5000);  //delay  //3.1.2.J
        timer2_ms_delay(6000);   //delay  //3.1.2.J
        MODEM_ON;
    }
  // timer2_ms_delay(1000); //delay  //3.1.2.J
     timer2_ms_delay(5000); //delay  //3.1.2.J

    serial_string_send("AT\r\n");        if( check_modem_reply("OK", 2, 500)) { }else{/* strcpy(lcd_data, "AT FAIL")  ;  lcd_display(2); timer2_sec_delay(2); */}    timer2_ms_delay(100); //Check Communication        
    serial_string_send("ATE0\r\n");      if( check_modem_reply("OK", 2, 1000)){ }else{/* strcpy(lcd_data, "ATE0 FAIL");  lcd_display(2); timer2_sec_delay(2); */}    timer2_ms_delay(100); //Echo off        
    serial_string_send("AT+CMGF=1\r\n"); if( check_modem_reply("OK", 2, 1000)){ }else{/* strcpy(lcd_data, "CMGF FAIL");  lcd_display(2); timer2_sec_delay(2); */}    timer2_ms_delay(100); //set text mode for sms
    serial_string_send("AT+CLTS=1\r\n"); if( check_modem_reply("OK", 2, 3000)){ }else{/* strcpy(lcd_data, "CLTS FAIL");  lcd_display(2); timer2_sec_delay(2); */}    timer2_ms_delay(100);    
}

void modem_powerdown(unsigned char status)
{
    unsigned char temp_buffer[20] = {0};

    if (status == TRUE)
        strcpy(temp_buffer, "AT+IPR=9600\r\n ");
    else
        strcpy(temp_buffer, "AT+IPR=0\r\n ");

    serial_string_send(temp_buffer);
    check_modem_reply("OK", 2, 2000);

    timer2_sec_delay(2);

    if (status == TRUE)
        strcpy(temp_buffer, "AT+CFUN=4\r\n   "); //4=disable Rx & Tx of phone(stored in flash)
    else //0=minimum functionality but not stored in flash				
        strcpy(temp_buffer, "AT+CFUN=1\r\n   ");

    serial_string_send(temp_buffer);
    check_modem_reply("OK", 2, 2000);

    timer2_sec_delay(2);
}

void check_sms(void)
{
    unsigned char status = 0;
    char command[20];

    //@    serial_string_send("ATE0\r\n"); //Echo off  
    //    check_modem_reply("OK", 2, 1000);
    //
    //    if ((consumer_flags.card_detect == LOW)&&(consumer_flags.coin_read == LOW)&&(consumer_flags.dispense_button == LOW)&&(menu_flags.any_button_press == 0))
    //    {
    //	serial_string_send("AT+CMGF=1\r\n"); //set text mode for sms
    //	check_modem_reply("OK", 2, 1000);
    //@    } else{}

    if ((consumer_flags.card_detect == LOW)&&(consumer_flags.coin_read == LOW)&&(consumer_flags.dispense_button == LOW)&&(menu_flags.any_button_press == 0)) //v1.0.0
    {
        if (new_sms_location)
        {
            sms_scan_no = new_sms_location - 0x30; //ask
            new_sms_location = 0;
        }
        else if (sms_scan_no >= (TOTAL_SMS_READ + 1))
            sms_scan_no = 1;
        else
        {
        }

        status = read_sms(sms_scan_no);

        switch (status)
        {
        case 4:
            LED3_ON;
            if ((system_error == ERROR_FREE)&&(peripheral_flags.coin_en_dis == 1)&&(peripheral_flags.dev_en_dis == TRUE)) //v2.2.9
                coinbox_disable();
            else
            {
            }
            send_server_response(DEV_SMS_RX);
            process_sms();
            if ((system_error == ERROR_FREE)&&(peripheral_flags.coin_en_dis == 1)&&(peripheral_flags.dev_en_dis == TRUE)&&(coin_flags.coin_collector_error == 0)&&(mode_flags.op_mode != SCHOOL_MODE)&&(mode_flags.op_mode != CORPORATE_MODE)&&(mode_flags.op_mode != DEBUG_MODE)&&(mode_flags.op_mode != DIAGNOSTIC_MODE)&&(mode_flags.op_mode != ANTENNA_MODE)&&(mode_flags.op_mode != DUMP_MODE)&&(mode_flags.op_mode != REFILL_MODE)) //V6////v2.2.9 //v2.3.4 -> if School,Corporate,Debug/diagnostic/antenna mode then no need to re-enable coin module it will enable while exit by R29 in Ideal mode
            {
                coinbox_enable();
            }
            else{}
            LED3_OFF;
            break;

        default:
            break;
        }
        while (BusyUART2()); //wait till the UART is busy	
        WriteUART2((unsigned int) 26);

        timer2_ms_delay(200);
        sprintf(command, "AT+CMGD=%u\r\n", sms_scan_no);
        serial_string_send(command);
        check_modem_reply("OK", 2, 2000);
        sms_scan_no++;

    }
    else
    {
    }

}

void send_sms(void)
{
    unsigned char cntr_1 = 0, len = 0;
    unsigned int cntr_2 = 0;

    serial_string_send("AT\r\n"); //v3.0.0
    check_modem_reply("OK", 2, 500);

    if (gsm_flags.sms_permit == TRUE)
    {
        len = strlen(sms_sender_no);
        //		sprintf(lcd_data,"Length = %d",len);lcd_display(1); timer2_sec_delay(3);
        serial_string_send("AT+CMGS=\"+");

        if (len > 3) //send to master cdfontact if not requested by sms
        {
            putsUART2((unsigned int *) (sms_sender_no));
        }
        else
            putsUART2((unsigned int *) (master_contact));
        putsUART2((unsigned int *) ("")); //do_finalize
        serial_string_send("\",145\r\n");
        timer2_ms_delay(300);
        serial_string_send(transmitData);
        while (BusyUART2()); //wait till the UART is busy	
        WriteUART2((unsigned int) 26);


        for (cntr_2 = 0; cntr_2<sizeof (transmitData); cntr_2++)
            transmitData[cntr_2] = 0;

        cntr_1 = 0; //v3.0.0
        do //wait till modem is free from any progressing
        {
            serial_string_send("AT+CPIN?\r\n");
            timer2_ms_delay(1000); //delay
            cntr_1++;
        }
        while ((!(check_modem_reply("CPIN: READY", 11, 5000)))&&(cntr_1 < 3));
        timer2_ms_delay(100);
    }
    else
    {
    }
}

void process_sms(void)
{

    unsigned int server_code = 0;
    unsigned char cntr_1 = 0, cntr_2 = 0, finish = 0, sms_code_count = 0, temp_count = 0, dummy_master_contact[15] = {0}; //v3.2.0 //12 to 15
    char data_buffer[16];
    char* pointer;
    char* copy_ptr;
    char* sms_data_start_ptr;
    unsigned char server_data_req = 0; //v3.0.0


    //char** endptr;
    for (cntr_1 = 0; cntr_1<sizeof (data_buffer); cntr_1++) //reset buffer
        data_buffer[cntr_1] = 0;

    if (gsm_flags.fact_password_ok == 0)
    {
        pointer = strstr(sms_string, current_password); //check for "password" in the "sms_string" sms data
        //it return 0 if 2nd string is not found in 1st string 
        //it returns pointer of the location where 2nd string starts in 1st string 
    }
    else
    {
        pointer = strstr(sms_string, factory_password); //factory_password	//check for "factory_password" in the "sms_string" 
        //it return 0 if 2nd string is not found in 1st string 
        //it returns pointer of the location where 2nd string starts in 1st string 
    }
    if (pointer != 0) //Process only is password authenticated...
    {
        for (cntr_1 = 0; cntr_1 < 6; cntr_1++) //set the pointer to start of desired msg(@ the end of password)
        {
            ++pointer;
            if (*pointer == ',')
                break;
            else
            {
            }

            if ((*pointer == ';') || (*pointer == '#'))
            {
                pointer--;
                break;
            }
            else
            {
            }
        }

        copy_ptr = pointer;

        while ((finish == FALSE) || (sms_code_count <= 30))
        {
            for (cntr_1 = 0; cntr_1<sizeof (data_buffer); cntr_1++) //reset buffer
                data_buffer[cntr_1] = 0;

            if (sms_code_count == 0)
            {
                for (cntr_1 = 0; cntr_1 < 5; cntr_1++) //cntr_1<4
                {
                    if (sms_string[cntr_1] == 'R')
                    {
                        if (sms_string[cntr_1 + 1] == 'M')
                        {
                            cntr_1 += 1;
                            system_flags.super_admin = 1;
                        }
                        else
                            system_flags.super_admin = 0;
                        break;
                    }
                    else if (sms_string[cntr_1] == 'O')
                    {
                        if (sms_string[cntr_1 + 1] == 'C')
                        {
                            cntr_1 += 1;
                            system_flags.ultra_super_admin = 1;
                        }
                        else
                            system_flags.ultra_super_admin = 0;
                        break;
                    }
                    else
                    {
                    }

                }
                for (cntr_2 = 0; cntr_2 < 6; cntr_2++) //cntr_2<5
                {
                    if (sms_string[++cntr_1] == ',')
                        break;
                    else
                    {
                    }

                    data_buffer[cntr_2] = sms_string[cntr_1]; //get the code from sms (R1,R24...), excluding 'R'			
                } //get RM super admin Msg //
                cntr_2 = 0;
            }
            else
            {
                cntr_2 = 0;
                for (cntr_1 = 0; cntr_1 < 5; cntr_1++)
                {
                    ++pointer;
                    if (*pointer == ',')
                        break;
                    else
                    {
                    }

                    if (*pointer == ';')
                    {
                        pointer--;
                        break;
                    }
                    else
                    {
                    }

                    if ((*pointer >= '0') && (*pointer <= '9')) //check for only no. data
                        data_buffer[cntr_2++] = *pointer;
                        /*else if((*pointer=='M')){					//v2.2.9
                                data_buffer[cntr_2] = *pointer;			//v2.2.9	
                        }*/ //v2.2.9										
                    else
                    {
                    }
                }
            }
            server_code = atoi(data_buffer); //convert the code to integer
            //-----Debug
            lcd_init();
            strcpy(lcd_data, "PROCESSING...");
            lcd_display(1);
            strcpy(lcd_data, "PLS. WAIT...");
            lcd_display(2);
            timer2_ms_delay(500);
            clear_lcd();
            sprintf(lcd_data, "Code:%s", data_buffer);
            lcd_display(1);

            //-----Debug
            if (server_code == 0)
            {
                finish = TRUE;
                sms_code_count = 31;
                break;
            }
            else
            {
            }

            sms_data_start_ptr = pointer;

            for (cntr_1 = 0; cntr_1<sizeof (data_buffer); cntr_1++) //reset buffer
                data_buffer[cntr_1] = 0;

            for (cntr_1 = 0; cntr_1<sizeof (data_buffer);)
            {
                if (*++pointer == ';')
                    break;
                else
                {
                }

                if (*pointer == '#')
                {
                    finish = TRUE;
                    sms_code_count = 31;
                    break;
                }
                else
                {
                }

                if (*pointer != ',')
                    data_buffer[cntr_1++] = *pointer;
                else
                {
                }
            }
            sms_code_count++;
            //-----Debug
            if (mode_flags.op_mode == DEBUG_MODE) //v2.2.4
            {
                strcpy(lcd_data, data_buffer);
                lcd_display(2);
            }
            else
            {
            }
            timer2_ms_delay(1000);
            //-----Debug
            if (!system_flags.ultra_super_admin)
            {
                switch (server_code) //process according to the code 
                {
                case SER_TDS_CALIB: //Calibrate TDS-1	                //1
                    tds_calib_fact = atoi(data_buffer);
                    temp_count = 0;
                    temp_count = data_validation(SER_TDS_CALIB);
                    if (temp_count == 0)
                        eeprom_write(SER_TDS_CALIB); //store in eeprom
                    else
                    {
                    }
                    send_server_response(DEV_TDS_CALIB);
                    break;

                case SER_OUT_FLOW_CALIB: //2
                    out_flow_calibration = atoi(data_buffer);
                    temp_count = 0;
                    temp_count = data_validation(SER_OUT_FLOW_CALIB);
                    if (temp_count == 0)
                        eeprom_write(SER_OUT_FLOW_CALIB);
                    else
                    {
                    }
                    send_server_response(DEV_OUT_FLOW_CALIB);
                    break;

                case SER_IN_FLOW_CALIB: //3
                    in_flow_calibration = atoi(data_buffer);
                    temp_count = 0;
                    temp_count = data_validation(SER_IN_FLOW_CALIB);
                    if (temp_count == 0)
                        eeprom_write(SER_IN_FLOW_CALIB);
                    else
                    {
                    }
                    send_server_response(DEV_IN_FLOW_CALIB);
                    break;

                case SER_SERVER_IP: //Change destination IP-4           //4
                    for (temp_count = 0; temp_count<sizeof (server_ip); temp_count++)
                        server_ip[temp_count] = 0;

                    strcpy(server_ip, data_buffer);
                    temp_count = 0;
                    temp_count = data_validation(SER_SERVER_IP);
                    if (temp_count == 0)
                        eeprom_write(SER_SERVER_IP);
                    else
                    {
                    }
                    send_server_response(DEV_SYSTEM_SETTINGS_3);
                    break;

                case SER_SERVER_PORT: //Change destination Port             //5
                    for (temp_count = 0; temp_count<sizeof (server_port); temp_count++)
                        server_port[temp_count] = 0;

                    strcpy(server_port, data_buffer);
                    temp_count = 0;
                    temp_count = data_validation(SER_SERVER_PORT);
                    if (temp_count == 0)
                        eeprom_write(SER_SERVER_PORT);
                    else
                    {
                    }
                    send_server_response(DEV_SYSTEM_SETTINGS_3);
                    break;

                case SER_SERVER_APN: //Change APN Name                      //6
                    for (cntr_1 = 0; cntr_1 < (sizeof (apn)); cntr_1++) //reset APN previous value
                        apn[cntr_1] = 0;
                    for (temp_count = 0; temp_count<sizeof (apn); temp_count++)
                        apn[temp_count] = 0;

                    strcpy(apn, data_buffer);
                    temp_count = 0;
                    temp_count = data_validation(SER_SERVER_APN);
                    if (temp_count == 0)
                        eeprom_write(SER_SERVER_APN);
                    else
                    {
                    }
                    send_server_response(DEV_SYSTEM_SETTINGS_3);
                    break;

                case SER_TDS_MAX: //Max TDS Set Point                       //7
                    max_tds = atoi(data_buffer);
                    temp_count = 0;
                    temp_count = data_validation(SER_TDS_MAX);
                    if (temp_count == 0)
                        eeprom_write(SER_TDS_MAX);
                    else
                    {
                    }
                    send_server_response(DEV_TDS_MAX);
                    break;

                case SER_TDS_MIN: //Min TDS Set Point                       //8
                    min_tds = atoi(data_buffer);
                    temp_count = 0;
                    temp_count = data_validation(SER_TDS_MIN);
                    if (temp_count == 0)
                        eeprom_write(SER_TDS_MIN);
                    else
                    {
                    }
                    send_server_response(DEV_TDS_MIN);
                    break;

                case SER_USER_TIMEOUT: //9
                    user_timeout = atoi(data_buffer);
                    temp_count = 0;
                    temp_count = data_validation(SER_USER_TIMEOUT); //v3.1.1.C
                    if (temp_count == 0)
                        eeprom_write(SER_USER_TIMEOUT);
                    else
                    {
                    }

                    send_server_response(DEV_USER_TIMEOUT);
                    break;

                case SER_DISP_TIMEOUT: //10
                    dispense_timeout = atoi(data_buffer);
                    temp_count = 0;
                    temp_count = data_validation(SER_DISP_TIMEOUT);
                    if (temp_count == 0)
                        eeprom_write(SER_DISP_TIMEOUT);
                    else
                    {
                    }
                    send_server_response(DEV_DISP_TIMEOUT);
                    break;

                    //case SER_LLS_DETECT_STATE:	                //11
                case SER_SLEEP_MODE_FLAG: //11
                    break;

                case SER_DEVICE_EN_DIS: //Enable/Disable Device             //12
                    coin_flags.coin_debug  = 1;  //v3.1.2.I
                    if (data_buffer[0] == '1')
                    {
                        peripheral_flags.dev_en_dis = TRUE;
                        error_stack[DEV_DISABLED] = FALSE;
                        read_eeprom_parameter(SER_COIN_EN_DIS); //v2.2.9
                        timer2_ms_delay(50); //v2.2.9
                        if ((peripheral_flags.coin_en_dis)&&(coin_flags.coin_collector_error == 0)) //v2.2.9
                        {                              
                            coinbox_init();
                        }
                        else
                        {
                        }
                    }
                    else if (data_buffer[0] == '0')
                    {
                        peripheral_flags.dev_en_dis = FALSE;
                        error_stack[DEV_DISABLED] = TRUE;
                        coinbox_disable(); //v2.2.9
                        //peripheral_flags.coin_en_dis = 0;				
                        //eeprom_write(SER_COIN_EN_DIS);
                    }
                    else
                    {
                    }
                    coin_flags.coin_debug  = 0;  //3.1.2.I
                    eeprom_write(SER_ERROR_STACK);
                    timer2_ms_delay(100);
                    eeprom_write(SER_DEVICE_EN_DIS);
                    system_error_check();
                    send_server_response(DEV_DEVICE_EN_DIS);
                    break;

                case SER_TDS_EN_DIS: //Enable/Disable TDS Sensor            //13
                    if (data_buffer[0] == '1')
                        peripheral_flags.tds_en_dis = TRUE;
                    else if (data_buffer[0] == '0')
                        peripheral_flags.tds_en_dis = FALSE;
                    else
                    {
                    }
                    eeprom_write(SER_TDS_EN_DIS);
                    send_server_response(DEV_TDS_EN_DIS);
                    break;

                case SER_LLS_HLS_EN_DIS: //Enable/Disable LLS               //14
                    peripheral_flags.lls_hls_en_dis = atoi(data_buffer);
                    temp_count = 0;
                    temp_count = data_validation(SER_LLS_HLS_EN_DIS);
                    if (temp_count == 0)
                        eeprom_write(SER_LLS_HLS_EN_DIS);
                    else{}                    
                    send_server_response(DEV_LLS_EN_DIS);
                    
                    if (peripheral_flags.lls_hls_en_dis < 2)
                    {
                        peripheral_flags.chiller_heater_en_dis = 0;
                        eeprom_write(SER_CHILLER_HEATER_EN_DIS); //v2.3.6
                        send_server_response(DEV_CHILLER_HEATER_EN_DIS); //v2.3.6
                    }
                    break;

                case SER_MASS_DISP_EN_DIS: //Enable/Disable Mass Dispense   //15
                    break;

                case SER_SCHOOL_DISPENSE_AMT: //-16	//Protocol deleted      //16
                    school_disp_amount = atoi(data_buffer);
                    temp_count = 0;
                    temp_count = data_validation(SER_SCHOOL_DISPENSE_AMT);
                    if (temp_count == 0)
                        eeprom_write(SER_SCHOOL_DISPENSE_AMT);
                    else
                    {
                    }

                    sch_disp_fraction = ((float) school_disp_amount) / 1000.0;
                    send_server_response(DEV_SCHOOL_DISPENSE_AMT);
                    break;

                case SER_ER_DISPLAY_TIMER: //17
                    error_display_time = atoi(data_buffer);
                    temp_count = 0;
                    temp_count = data_validation(SER_ER_DISPLAY_TIMER);
                    if (temp_count == 0)
                        eeprom_write(SER_ER_DISPLAY_TIMER);
                    else
                    {
                    }

                    send_server_response(DEV_ER_DISPLAY_TIMER);
                    break;

                case SER_DISPENSE_LT_LC: //18
                    //		    DISPENSE_LEAST_COUNT = atof(data_buffer);  //v3.1.1.E
                    //		    temp_count = 0;
                    //		    temp_count = data_validation(SER_DISPENSE_LT_LC);
                    //		    if (temp_count == 0)
                    //			eeprom_write(SER_DISPENSE_LT_LC);
                    //		    else
                    //		    {
                    //		    }
                    //
                    //		    send_server_response(DEV_DISPENSE_LT_LC);
                    break;

                case SER_NEW_PASSWORD_SET: //Change SMS Password            //19
                    strcpy(current_password, data_buffer);
                    temp_count = 0;
                    temp_count = data_validation(SER_NEW_PASSWORD_SET);
                    if (temp_count == 0)
                        eeprom_write(SER_NEW_PASSWORD_SET);
                    else
                    {
                    }
                    send_server_response(DEV_NEW_PASSWORD_SET);
                    break;


                case SER_LOGOUT_IDLE_TIMEOUT: //20
                    logout_idle_time = atoi(data_buffer);
                    temp_count = 0;
                    temp_count = data_validation(SER_LOGOUT_IDLE_TIMEOUT);
                    if (temp_count == 0)
                        eeprom_write(SER_LOGOUT_IDLE_TIMEOUT);
                    else
                    {
                    }
                    send_server_response(DEV_LOGOUT_IDLE_TIMEOUT);
                    break;

                case SER_LT_EMPTY_TANK:                        //21
                    lit_empty_tank = atoi(data_buffer);
                    temp_count = 0;
                    temp_count = data_validation(SER_LT_EMPTY_TANK);
                    if (temp_count == 0)
                    {
                        eeprom_write(SER_LT_EMPTY_TANK);
                        tank_low_level_litre = lit_empty_tank;
                        eeprom_write(SER_TANK_LOW_LEVEL_STATUS);
                    }
                    else
                    {
                    }
                    send_server_response(DEV_LT_EMPTY_TANK);
                    break;

                case SER_WATER_EXPIRED: //Water Expired                     //22
                    error_flags.water_expired = TRUE;

                    eeprom_write(SER_WATER_EXPIRED);
                    timer2_ms_delay(100);
                    send_server_response(DEV_HB_MSG);
                    break;

                case SER_FACT_PASSWORD_RESET: //Factory Password Reset      //23  //v3.1.1.C

                    pointer = strstr(sms_string, factory_password); //check for "factory_password" in the "sms_string" 
                    //it return 0 if 2nd string is not found in 1st string 
                    //it returns pointer of the location where 2nd string starts in 1st string 
                    pointer = copy_ptr;
                    if (pointer != 0)
                    {
                        for (cntr_1 = 0; cntr_1 < (sizeof (current_password)); cntr_1++)
                        {
                            if (*++pointer == '#')
                                break;
                            else
                            {
                            }
                            current_password[cntr_1] = *pointer;
                        }
                    }
                    else
                    {
                    }

                    temp_count = 0;
                    temp_count = data_validation(SER_FACT_PASSWORD_RESET);
                    if (temp_count == 0)
                        eeprom_write(SER_NEW_PASSWORD_SET);
                    else
                    {
                    }
                    send_server_response(DEV_NEW_PASSWORD_SET);
                    gsm_flags.fact_password_ok = 0;
                    break;

                case SER_RESET_OOO: //Reset Out of Order                    //24
                    system_error = ERROR_FREE;
                    error_stack[FLOW_SENS_ER] = FALSE;
                    error_stack[SV_ER] = FALSE;

                    eeprom_write(SER_ERROR_STACK);

                    if (school_mode_en)
                        mode_flags.op_mode = SCHOOL_MODE;
                    else if (corporate_mode_en) //v2.2.5
                        mode_flags.op_mode = CORPORATE_MODE;
                    else
                        mode_flags.op_mode = IDLE_MODE;

                    eeprom_write(SER_OP_MODE);
                    strcpy(error_state, "0000");
                    system_error_check(); //v2.2.4
                    send_server_response(DEV_HB_MSG);
                    break;

                case SER_REMOTE_DUMP: //Remote Dump Water                   //25
                    dump_flags.remote_water_dump = TRUE;
                    mode_flags.op_mode = DUMP_MODE;

                    error_flags.water_expired = FALSE;
                    peripheral_flags.dev_en_dis = TRUE;
                    error_stack[DEV_DISABLED] = FALSE;
                    eeprom_write(SER_ERROR_STACK);

                    mode_flags.forced_function_state = TRUE;
                    function_state = ST_OP_MODE;
                    break;

                case SER_RQ_SYSTEM_SETTINGS: //Request System Settings      //26
                    system_flags.send_sys_setting = TRUE;
                    send_server_response(DEV_SYSTEM_SETTINGS_1);
                    timer2_ms_delay(100);
                    send_server_response(DEV_SYSTEM_SETTINGS_2);
                    timer2_ms_delay(100);
                    send_server_response(DEV_SYSTEM_SETTINGS_3);
                    timer2_ms_delay(100);
                    send_server_response(DEV_SYSTEM_SETTINGS_4);
                    timer2_ms_delay(100);
                    send_server_response(DEV_SYSTEM_SETTINGS_5);
                    timer2_ms_delay(100);
                    send_server_response(DEV_SYSTEM_SETTINGS_6);
                    timer2_ms_delay(100);                             //v3.1.2.G
                    if (peripheral_flags.coin_en_dis) //v2.2.9
                        send_server_coin_id();
                    else{}
                    break;

                case SER_IDLE_MSG_FREQ: //Idle Message Frequency            //27
                    idle_msg_freq = atoi(data_buffer);
                    if ((idle_msg_freq > 2000) || (idle_msg_freq == 0))
                        idle_msg_freq = 60; //change 5 to 20 //v3.1.1.F
                    else
                    {
                    }
                    //copy_idle_msg_freq = idle_msg_freq;
                    temp_count = 0;
                    temp_count = data_validation(SER_IDLE_MSG_FREQ);
                    if (temp_count == 0)
                        eeprom_write(SER_IDLE_MSG_FREQ);
                    else
                    {
                    }
                    send_server_response(DEV_HB_MSG);
                    break;

                case SER_RECONNECTION: //force fully Request Reconnection   //28
                    system_flags.force_reconnect = TRUE;
                    timer2_ms_delay(200);
                    sprintf(data_buffer, "AT+CMGD=%u\r\n", sms_scan_no); //del sms before soft reset			
                    serial_string_send(data_buffer);
                    check_modem_reply("OK", 2, 2000);
                    connect_to_server();
                    break;

                case SER_RESET_ALL_OOO: //force fully clear all error       //29
                    //error_flags.reset_all_ooo = TRUE;
                    if (error_flags.water_expired == TRUE)
                    {
                        error_flags.water_expired = FALSE;
                        eeprom_write(SER_WATER_EXPIRED);
                    }
                    else{}
                    peripheral_flags.dev_en_dis = TRUE;
                    system_error = ERROR_FREE;

                    if (error_flags.tank_empty == TRUE)
                    {
                        error_flags.tank_empty = FALSE;
                        read_eeprom_parameter(SER_LT_EMPTY_TANK);
                    }
                    else{}
                    if (school_mode_en)
                        mode_flags.op_mode = SCHOOL_MODE;
                    else if (corporate_mode_en) //v2.2.5
                        mode_flags.op_mode = CORPORATE_MODE;
                    else
                        mode_flags.op_mode = IDLE_MODE;

                    //@if((peripheral_flags.solar_sense!=TRUE)&&(peripheral_flags.ac_mains_sense!=TRUE))  //
                    if (peripheral_flags.solar_sense != TRUE) //3.0.2 18/11/16
                        battery_verify_time_stamp = min_tick;

                    for (cntr_1 = 0; cntr_1 < TOTAL_ERRORS; cntr_1++)
                        error_stack[cntr_1] = FALSE;
                    
                    coin_flags.coin_disable_by_fs_error = 0;	//v3.1.2.H

                    eeprom_write(SER_ERROR_STACK);
                    eeprom_write(SER_DEVICE_EN_DIS);
                    eeprom_write(SER_OP_MODE);
                    strcpy(error_state, "0000");
                    eeprom_write(SER_DEBUG_MODE);
                    eeprom_write(SER_DIAGNOSTIC_TEST);
                    send_server_response(DEV_HB_MSG);
                    consumer_flags.dispense_button = 0; //v2.2.5    //IMP because when button Long Pressed to Dis. the mode it jumps to switch case in main.c and water dispense starts whenever corporate mode is ON
                    system_flags.ultra_super_admin_state = 0;
                    eeprom_write(SER_ULTRA_SUPER_ADMIN_MODE);
                    break;

                case SER_DUMP_TIMEOUT: //30
                    dump_timeout = atoi(data_buffer);
                    temp_count = 0;
                    temp_count = data_validation(SER_DUMP_TIMEOUT);
                    if (temp_count == 0)
                        eeprom_write(SER_DUMP_TIMEOUT);
                    else
                    {
                    }
                    send_server_response(DEV_DUMP_TIMEOUT);
                    break;

                case SER_REFILL_TIMEOUT: //31
                    refill_timeout = atoi(data_buffer);
                    temp_count = 0;
                    temp_count = data_validation(SER_REFILL_TIMEOUT);
                    if (temp_count == 0)
                        eeprom_write(SER_REFILL_TIMEOUT);
                    else
                    {
                    }
                    send_server_response(DEV_REFILL_TIMEOUT);
                    break;

                case SER_CLEANING_TIMEOUT: //32
                    cleaning_timeout = atoi(data_buffer);
                    temp_count = 0;
                    temp_count = data_validation(SER_CLEANING_TIMEOUT);
                    if (temp_count == 0)
                        eeprom_write(SER_CLEANING_TIMEOUT);
                    else
                    {
                    }
                    send_server_response(DEV_CLEANING_TIMEOUT);
                    break;

                case SER_CLEAN_TANK: //35
                    break;

                case SER_MODEM_COMMAND: //36
                    //-------------------------v2.2.2
                    timer2_ms_delay(200);
                    sprintf(data_buffer, "AT+CMGD=%u\r\n", sms_scan_no); //delete sms before command process			
                    serial_string_send(data_buffer);
                    check_modem_reply("OK", 2, 2000);
                    //-------------------------v2.2.2
                    cntr_1 = 0;
                    cntr_2 = 0;
                    for (cntr_1 = 0; cntr_1 < (sizeof (modem_cmd_response)); cntr_1++)
                        modem_cmd_response[cntr_1] = 0;

                    while ((cntr_1 == FALSE) && (cntr_2 <= 3))
                    {
                        serial_string_send("AT\r\n");
                        cntr_1 = check_modem_reply("OK", 2, 1000);
                        cntr_2++;
                    }
                    cntr_1 = 0;
                    serial_string_send(data_buffer);
                    serial_string_send("\r\n");
                    //timer2_ms_delay(1000);		//delay
                    timer2_ms_delay(10000); //delay		//v2.2.3
                    for (cntr_1 = 0; cntr_1 < (sizeof (modem_cmd_response)); cntr_1++) //capture full sms data in "sms_buffer" without any checking
                        modem_cmd_response[cntr_1] = serial_byte_receive();

                    send_server_response(DEV_MODEM_COMMAND);
                    for (cntr_1 = 0; cntr_1 < (sizeof (modem_cmd_response)); cntr_1++)
                        modem_cmd_response[cntr_1] = 0;
                    break;

                case SER_GENERAL_COMMAND: //37
                    for (cntr_1 = 0; cntr_1 < (sizeof (modem_cmd_response)); cntr_1++)
                        modem_cmd_response[cntr_1] = 0;
                    if (mode_flags.op_mode == DEBUG_MODE)
                    {
                        cntr_2 = data_buffer[0];
                        switch (cntr_2)
                        {
                        case '1':
                            sprintf(modem_cmd_response, "%.5u,%.3d,%ld,%u,%1.3f,%s,%.4u,%s", 0, 0, avg_tds_count, tds_calib_fact, adc_volt, tds_value, 0, factory_password);
                            break;

                        default:
                            sprintf(modem_cmd_response, "%.5u,%.3d,%ld,%u,%1.3f,%s,%.4u,%s", 0, 0, avg_tds_count, tds_calib_fact, adc_volt, tds_value, 0, factory_password);
                            break;
                        }
                        send_server_response(DEV_GENERAL_COMMAND);
                        for (cntr_1 = 0; cntr_1 < (sizeof (modem_cmd_response)); cntr_1++)
                            modem_cmd_response[cntr_1] = 0;

                        timer2_ms_delay(500);
                        send_server_response(DEV_HB_MSG);
                    }
                    else
                    { //v2.3.5
                        lcd_init();
                        strcpy(lcd_data, "INSUFFICIENT");
                        lcd_display(1);
                        strcpy(lcd_data, "COMMAND CODE");
                        lcd_display(2);
                        timer2_sec_delay(error_display_time);
                    }
                    break;

                case SER_EEPROM_READ: //38
                    timer2_ms_delay(300);
                    lcd_init();
                    strcpy(lcd_data, "MEM. PROCESSING");
                    lcd_display(1);
                    strcpy(lcd_data, "PLS. WAIT...");
                    lcd_display(2);
                    for (temp_count = 0; temp_count <= EEPROM_TOTAL_PARAMETERS; temp_count++)
                        read_eeprom_parameter(temp_count);

                    timer2_ms_delay(100);
                    send_server_response(DEV_SYSTEM_SETTINGS_1);
                    timer2_ms_delay(100);
                    send_server_response(DEV_SYSTEM_SETTINGS_2);
                    timer2_ms_delay(100);
                    send_server_response(DEV_SYSTEM_SETTINGS_3);
                    timer2_ms_delay(100);
                    send_server_response(DEV_SYSTEM_SETTINGS_4);
                    timer2_ms_delay(100);
                    send_server_response(DEV_SYSTEM_SETTINGS_5);
                    timer2_ms_delay(100);
                    send_server_response(DEV_SYSTEM_SETTINGS_6);
                    if (peripheral_flags.coin_en_dis) //v2.2.9		
                        send_server_coin_id();
                    else
                    {
                    }
                    break;

                case SER_DEBUG_MODE: //39
                    mode_flags.op_mode = DEBUG_MODE;
                    peripheral_flags.dev_en_dis = FALSE;
                    //debug_mode = 1;
                    eeprom_write(SER_DEBUG_MODE);
                    eeprom_write(SER_OP_MODE);
                    send_server_response(DEV_DEBUG_MODE);
                    break;

                case SER_CHANGE_DEV_ID: //40
                    if (mode_flags.op_mode == DEBUG_MODE)
                    {
                        for (temp_count = 0; temp_count<sizeof (msg_data); temp_count++)
                            msg_data[temp_count] = 0;

                        strcpy(msg_data, MACHINE_ID); //copy old device ID for server acknowledgment 
                        strcpy(MACHINE_ID, data_buffer);
                        temp_count = 0;
                        temp_count = data_validation(SER_CHANGE_DEV_ID);
                        if (temp_count == 0)
                            eeprom_write(SER_CHANGE_DEV_ID);
                        else
                        {
                        }

                        send_server_response(DEV_DEVICE_IC_CHANGED);
                    }
                    else
                    {
                        lcd_init();
                        strcpy(lcd_data, "INSUFFICIENT");
                        lcd_display(1);
                        strcpy(lcd_data, "COMMAND CODE");
                        lcd_display(2);
                        timer2_sec_delay(error_display_time);
                    }
                    break;

                case SER_LEAK_TIMER: //41
                    leak_timeout = atoi(data_buffer);
                    temp_count = 0;
                    temp_count = data_validation(SER_LEAK_TIMER);
                    if (temp_count == 0)
                        eeprom_write(SER_LEAK_TIMER);
                    else
                    {
                    }
                    send_server_response(DEV_LEAK_TIMER);
                    break;

                case SER_BUZZER_CTRL: //42
                    if (data_buffer[0] == '1')
                        peripheral_flags.pag_buzzer_control = 1;
                    else if (data_buffer[0] == '0')
                        peripheral_flags.pag_buzzer_control = 0;
                    else
                    {
                    }
                    eeprom_write(SER_BUZZER_CTRL);
                    send_server_response(DEV_BUZZER_CTRL);
                    break;

                case SER_DEV_SOFT_RESET: //43
                    send_server_response(DEV_SOFT_RESET);
                    //mode_flags.soft_mcu_reset = TRUE;

                    timer2_ms_delay(200);
                    sprintf(data_buffer, "AT+CMGD=%u\r\n", sms_scan_no); //del sms before soft reset			
                    serial_string_send(data_buffer);
                    check_modem_reply("OK", 2, 2000);

                    if ((coin_flags.coin_total_amount_write == 1))
                    {//v2.2.9
                        coin_flags.coin_total_amount_write = 0;
                        eeprom_write(SER_TOTAL_COIN_AMOUNT);
                        eeprom_write(SER_TOTAL_COIN_COUNT);
                    }
                    else
                    {
                    }
                    abort(); //soft reset MCU
                    break;

                case SER_NETWORK_SIGNAL_STRENGTH: //44
                    send_server_response(DEV_N_W_SIGNAL_STRENGTH);
                    break;

                case SER_DIAGNOSTIC_TEST: //46
                    if (mode_flags.op_mode == DEBUG_MODE)
                    {
                        mode_flags.op_mode = DIAGNOSTIC_MODE;
                        peripheral_flags.dev_en_dis = TRUE;
                        eeprom_write(SER_DIAGNOSTIC_TEST);
                        eeprom_write(SER_OP_MODE);
                        send_server_response(DEV_DIAGNOSTIC_TEST);
                    }
                    else
                    {
                        lcd_init();
                        strcpy(lcd_data, "INSUFFICIENT");
                        lcd_display(1);
                        strcpy(lcd_data, "COMMAND CODE");
                        lcd_display(2);
                        timer2_sec_delay(error_display_time);
                    }

                    break;

                case SER_SEND_PARAMETER_VIA_SMS: //48
                    byte_variable = 0;
                    byte_variable = atoi(data_buffer);
                    send_server_response(byte_variable);
                    break;

                case SER_SMS_PERMIT: //49
                    if (data_buffer[0] == '1')
                        gsm_flags.sms_permit = 1;
                    else if (data_buffer[0] == '0')
                        gsm_flags.sms_permit = 0;
                    else
                    {
                    }
                    eeprom_write(SER_SMS_PERMIT);
                    send_server_response(DEV_SMS_PERMIT);
                    break;

                case SER_DEL_ALL_SMS: //50
                    timer2_ms_delay(500);
                    serial_string_send("AT+CMGDA=\"DEL ALL\""); //Delete all sms on SIM card
                    serial_string_send("\r\n");
                    if (check_modem_reply("OK", 2, 3000))
                    {
                        strcpy(lcd_data, "CMD SUCCESS");
                        lcd_display(1);
                        timer2_sec_delay(error_display_time);
                    }
                    else
                    {
                        strcpy(lcd_data, "CMD FAILED");
                        lcd_display(1);
                        timer2_sec_delay(error_display_time);
                    }
                    break;

                case SER_CURRENT_MODE: //51
                    send_server_response(DEV_CURRENT_MODE);
                    break;

                case SER_SCHOOL_MODE: //52

                    pointer = copy_ptr;
                    if (pointer != 0)
                    {
                        for (cntr_1 = 0; cntr_1 < ((sizeof (sch_mode_acc_id) + 2)); cntr_1++)
                        {
                            if (*++pointer == ',')
                                break;
                            else{}  
                        }
                        pointer++;
                        school_mode_en = (*pointer - 48);
                        pointer = copy_ptr;
                        
                        temp_count = 0;
                        temp_count = data_validation(SER_SCHOOL_MODE);
                        
                        if ( (temp_count==0) && ( (school_mode_en==1) || (school_mode_en==0) )   )  //V3.1.2.H
                        {
                            corporate_mode_en = 0;
                            memset(sch_mode_acc_id, '\0', sizeof (sch_mode_acc_id)); //v3.1.1.A
                            for (cntr_1 = 0; cntr_1 < ((sizeof (sch_mode_acc_id) + 2)); cntr_1++)
                            {
                                if (*++pointer == ',')
                                    break;
                                else{}
                               
                                if (cntr_1 <= 8) //v3.1.1.A
                                    sch_mode_acc_id[cntr_1] = *pointer;
                                else{}                              
                            }
                                temp_count = 0;
                                temp_count = data_validation(SER_SCHOOL_MODE_ACC_ID); //v3.1.1.E
                                if (temp_count == 0)
                                {
                                    if (school_mode_en)
                                        mode_flags.op_mode = SCHOOL_MODE;
                                    else if (corporate_mode_en) //v2.2.5
                                        mode_flags.op_mode = CORPORATE_MODE;
                                    else
                                    {
                                        mode_flags.op_mode = IDLE_MODE;
                                        battery_verify_time_stamp = min_tick;
                                    }
                                    eeprom_write(SER_SCHOOL_MODE);
                                    eeprom_write(SER_CORPORATE_MODE);
                                    eeprom_write(SER_OP_MODE); //v2.2.1
                                    eeprom_write(SER_SCHOOL_MODE_ACC_ID);
                                    read_eeprom_parameter(SER_SCHOOL_DISPENSE_AMT); //3.1.2.I
                                }
                                else{
                                school_mode_en =0;
                                }
                        }
                        else{}                      
                                send_server_response(DEV_CORPORATE_MODE);
                                send_server_response(DEV_SCHOOL_MODE);
                                send_server_response(DEV_SCHOOL_MODE_ACC_ID);
                    }
                    else{}
 
                    break;

                case SER_AGE_IN_MIN: //54
                    send_server_response(DEV_AGE_IN_MIN);
                    break;

                case SER_DS_INIT: //55
                    //				init_DS2781();
                    //				send_server_response(DEV_HB_MSG);
                    break;

                case SER_MASTER_CONT_NO: //56
                    if (mode_flags.op_mode == DEBUG_MODE)
                    {
                        strcpy(master_contact, data_buffer);
                        strcpy(lcd_data, master_contact);
                        lcd_display(2);
                        temp_count = 0;
                        temp_count = data_validation(SER_MASTER_CONT_NO);
                        if (temp_count == 0)
                            eeprom_write(SER_MASTER_CONT_NO);
                        else
                        {
                        }
                        send_server_response(DEV_MASTER_CONT_NO);
                    }
                    else
                    {
                        lcd_init();
                        strcpy(lcd_data, "INSUFFICIENT");
                        lcd_display(1);
                        strcpy(lcd_data, "COMMAND CODE");
                        lcd_display(2);
                        timer2_sec_delay(error_display_time);
                    }
                    break;

                case SER_DATA_ER_AUTO_CORRECT: //57
                    if (data_buffer[0] == '1')
                        error_flags.data_error_auto_correct = 1;
                    else if (data_buffer[0] == '0')
                        error_flags.data_error_auto_correct = 0;
                    else
                    {
                    }
                    eeprom_write(SER_DATA_ER_AUTO_CORRECT);
                    send_server_response(DEV_DATA_ER_AUTO_CORRECT);
                    break;

                case SER_HYST_COUNT: //v3.1.1.F                           //58			
                    break;

                case SER_ANTENNA_MODE: //60
                    if (mode_flags.op_mode == DEBUG_MODE)
                    {
                        mode_flags.op_mode = ANTENNA_MODE;
                        peripheral_flags.dev_en_dis = TRUE;
                        eeprom_write(SER_OP_MODE);
                        send_server_response(DEV_ANTENNA_MODE);

                        mode_flags.forced_function_state = TRUE;
                        function_state = ST_OP_MODE;
                    }
                    else
                    {
                        lcd_init();
                        strcpy(lcd_data, "INSUFFICIENT");
                        lcd_display(1);
                        strcpy(lcd_data, "COMMAND CODE");
                        lcd_display(2);
                        timer2_sec_delay(error_display_time);
                    }
                    break;

                case SER_TEMPERATURE_CALIB: //63
                    temp_calib_fact = atoi(data_buffer);
                    temp_count = 0;
                    temp_count = data_validation(SER_TEMPERATURE_CALIB);
                    if (temp_count == 0)
                        eeprom_write(SER_TEMPERATURE_CALIB);
                    else
                    {
                    }
                    send_server_response(DEV_TEMPERATURE_CALIB);
                    break;

                case SER_WATER_TEMPERATURE_LEVEL: //v2.3.6  //64
                    if (peripheral_flags.chiller_heater_en_dis != 0)
                    {

                        water_temperature_level = atoi(data_buffer); //v2.3.6
                        temp_count = 0;
                        temp_count = data_validation(SER_WATER_TEMPERATURE_LEVEL); //v2.3.6
                        if (temp_count == 0)
                            eeprom_write(SER_WATER_TEMPERATURE_LEVEL); //v2.3.6
                        else{}
                        send_server_response(DEV_WATER_TEMPERATURE_LEVEL); //v2.3.6
                    }
                    else
                    { //v2.3.6
                        lcd_init();
                        strcpy(lcd_data, "PLS ENABLE FIRST");
                        lcd_display(1);
                        strcpy(lcd_data, "CHILLER / HEATER");
                        lcd_display(2);
                        send_server_response(DEV_CHILLER_HEATER_EN_DIS); //v2.3.6
                        buzzer(ERR01);
                        timer2_sec_delay(error_display_time);
                    }
                    break;

                case SER_TEMPERATURE_EN_DIS: //65
                    if (data_buffer[0] == '1')
                    {
                        peripheral_flags.temp_en_dis = 1;                        
                        read_temperature(); //v3.1.2.G
                        timer2_sec_delay(1);
                        read_temperature(); //v3.1.2.G
                        timer2_sec_delay(1);
                        read_temperature(); //v3.1.2.G
                    }
                    else if (data_buffer[0] == '0')
                        peripheral_flags.temp_en_dis = 0;
                    else{}
                    eeprom_write(SER_TEMPERATURE_EN_DIS);
                    send_server_response(DEV_TEMPERATURE_EN_DIS);
                    
                    //----------3.1.2.H-------//
                    if(peripheral_flags.temp_en_dis==0){
                            peripheral_flags.chiller_heater_en_dis = 0;				
                            eeprom_write(SER_CHILLER_HEATER_EN_DIS);				
                            send_server_response(DEV_CHILLER_HEATER_EN_DIS);
                    }
                    else{}
                    //----------3.1.2.H-------//
                    break;

                case SER_OUT_FLOW_FREQ_LIMIT: //v2.2.3                      //66
                    output_flow_freq_limit = atoi(data_buffer);
                    temp_count = 0;
                    temp_count = data_validation(SER_OUT_FLOW_FREQ_LIMIT);
                    if (temp_count == 0)
                        eeprom_write(SER_OUT_FLOW_FREQ_LIMIT);
                    else
                    {
                    }
                    send_server_response(DEV_OUT_FLOW_FREQ_LIMIT);
                    break;

                case SER_IN_FLOW_FREQ_LIMIT: //v2.2.3                       //67
                    input_flow_freq_limit = atoi(data_buffer);
                    temp_count = 0;
                    temp_count = data_validation(SER_IN_FLOW_FREQ_LIMIT);
                    if (temp_count == 0)
                        eeprom_write(SER_IN_FLOW_FREQ_LIMIT);
                    else
                    {
                    }
                    send_server_response(DEV_IN_FLOW_FREQ_LIMIT);
                    break;

                case SER_SLEEP_MODE_MINUTES: //v2.2.4                       //68 //v3.1.1.F		
                    break;

                case SER_AUTO_REFILL_FLAG: //69  //v2.2.4
                    if (data_buffer[0] == '1')
                        refill_flags.auto_refill_flag = 1;
                    else if (data_buffer[0] == '0')
                        refill_flags.auto_refill_flag = 0;
                    else
                    {
                    }
                    eeprom_write(SER_AUTO_REFILL_FLAG);
                    send_server_response(DEV_AUTO_REFILL_FLAG);
                    break;


                case SER_FRANCHISEE_RFID: //v2.2.5				//70
                    pointer = copy_ptr;
                    if (pointer != 0)
                    {
                        for (cntr_1 = 0; cntr_1 < ((sizeof (franchisee_rfid_tag) + 2)); cntr_1++)
                        {
                            if (*++pointer == ',')
                                break;
                            else
                            {
                            }
                            franchisee_rfid_tag[cntr_1] = *pointer;
                        }
                        pointer++;
                    }
                    else
                    {
                    }
                    franchisee_rfid_tag[8] = 0;
                    lcd_init();
                    strcpy(lcd_data, "FR. RFID NO.");
                    lcd_display(1);
                    sprintf(lcd_data, "%.8s", franchisee_rfid_tag);
                    lcd_display(2);

                    timer2_sec_delay(2); //This delay is for viewing the entered rfid tag through sms

                    temp_count = 0;
                    temp_count = data_validation(SER_FRANCHISEE_RFID);
                    if (temp_count == 0)
                    {
                        peripheral_flags.franchisee_tag_check = 1;
                        eeprom_write(SER_FRANCHISEE_RFID); // Version - 2.2.5
                        send_server_response(DEV_FRANCHISEE_RFID);
                        buzzer(AUT01);
                        send_server_response(DEV_FRANCHISEE_RFID_VERIFIED);
                        lcd_init();
                        strcpy(lcd_data, "CORRECT RFID NO.");
                        lcd_display(1);
                    }
                    else
                    {
                        peripheral_flags.franchisee_tag_check = 0;
                        send_server_response(DEV_FRANCHISEE_RFID);
                        buzzer(AUT01);
                        send_server_response(DEV_FRANCHISEE_RFID_VERIFIED);
                        //strcpy(franchisee_rfid_tag,"2800757384AA");//ask
                        strcpy(franchisee_rfid_tag, "22A63EFB");
                        franchisee_rfid_tag[8] = 0; //last byte null character
                        eeprom_write(SER_FRANCHISEE_RFID);
                    }
                    break;


                case SER_CORPORATE_MODE: //v2.2.5	                        //71

                    pointer = copy_ptr;
                    if (pointer != 0)
                    {
                        for (cntr_1 = 0; cntr_1 < ((sizeof (corp_mode_acc_id) + 2)); cntr_1++)
                        {
                            if (*++pointer == ',')
                                break;
                            else{}                                                 
                        }
                        pointer++;
                        corporate_mode_en = (*pointer - 48);
                        pointer = copy_ptr;
                        
                        temp_count = 0;
                        temp_count = data_validation(SER_CORPORATE_MODE);
                        if( (temp_count==0) && ((corporate_mode_en == 1)  || (corporate_mode_en==0)) )
                        {
                            school_mode_en = 0;
                            memset(corp_mode_acc_id, '\0', sizeof (corp_mode_acc_id)); //v3.1.1.A

                            for (cntr_1 = 0; cntr_1 < ((sizeof (corp_mode_acc_id) + 2)); cntr_1++)
                            {
                                if (*++pointer == ',')
                                    break;
                                else{}                               
                                if (cntr_1 <= 8) //v3.1.1.E
                                    corp_mode_acc_id[cntr_1] = *pointer;
                                else{}
                            }
                                    temp_count = 0;
                                    temp_count = data_validation(SER_CORPORATE_MODE_ACC_ID);
                                    if (temp_count == 0)
                                    {
                                        if (corporate_mode_en) //v2.2.5
                                            mode_flags.op_mode = CORPORATE_MODE;
                                        else if (school_mode_en)
                                            mode_flags.op_mode = SCHOOL_MODE;
                                        else
                                        {
                                            mode_flags.op_mode = IDLE_MODE;
                                            battery_verify_time_stamp = min_tick;
                                        }
                                        eeprom_write(SER_CORPORATE_MODE);
                                        eeprom_write(SER_SCHOOL_MODE);
                                        eeprom_write(SER_OP_MODE); //v2.2.1
                                        eeprom_write(SER_CORPORATE_MODE_ACC_ID);
                                    }
                                    else{
                                    corporate_mode_en =0;
                                    }
                        }
                        else{}
                      
                                    send_server_response(DEV_SCHOOL_MODE);
                                    send_server_response(DEV_CORPORATE_MODE);
                                    send_server_response(DEV_CORPORATE_MODE_ACC_ID);
                    }
                    else{}
                  
                    //		    corp_mode_acc_id[8] = 0; //last byte null character  //3.1.0
                    //
                    //		    if (corporate_mode_en) //v2.2.5
                    //			mode_flags.op_mode = CORPORATE_MODE;
                    //		    else if (school_mode_en)
                    //			mode_flags.op_mode = SCHOOL_MODE;
                    //		    else
                    //		    {
                    //			mode_flags.op_mode = IDLE_MODE;
                    //			battery_verify_time_stamp = min_tick;
                    //		    }


                    //		    if(!corporate_mode_en)	//v2.2.5
                    //		    {
                    //			    mode_flags.op_mode = IDLE_MODE;
                    //			    corporate_mode_en = 0;
                    //			    battery_verify_time_stamp = min_tick;
                    //		    }
                    //		    else
                    //		    {
                    //			    mode_flags.op_mode = CORPORATE_MODE;
                    //			    corporate_mode_en = 1;
                    //		    }

//                    temp_count = 0;
//                    temp_count = data_validation(SER_CORPORATE_MODE);
//                    if (temp_count == 0)
//                    {
//                        temp_count = 0;
//                        temp_count = data_validation(SER_CORPORATE_MODE_ACC_ID);
//                        if (temp_count == 0)
//                        {
//                            if (corporate_mode_en) //v2.2.5
//                                mode_flags.op_mode = CORPORATE_MODE;
//                            else if (school_mode_en)
//                                mode_flags.op_mode = SCHOOL_MODE;
//                            else
//                            {
//                                mode_flags.op_mode = IDLE_MODE;
//                                battery_verify_time_stamp = min_tick;
//                            }
//                            eeprom_write(SER_CORPORATE_MODE);
//                            eeprom_write(SER_OP_MODE); //v2.2.1
//                            eeprom_write(SER_CORPORATE_MODE_ACC_ID);
//                        }
//                        else
//                        {
//                        }
//                    }
//                    else
//                    {
//                    }

//                    send_server_response(DEV_SCHOOL_MODE);
//                    send_server_response(DEV_CORPORATE_MODE);
//                    send_server_response(DEV_CORPORATE_MODE_ACC_ID);

                    break;

                case SER_CHILLER_HEATER_EN_DIS: //V2.2.6            //v2.3.6                //73
// ----  NOTE  ------------
// To enable Chiller / Heater User have to first enable temperature sensor & Product LLS first 
// To Switch / Enable / Disable Chiller / Heater, Device initial conditon must be both disable, If any one of them enable by software & user try to 
//switch form chiller to heatre & vice-versa user must have to disable first(whatevr previously on)

                    if ((peripheral_flags.temp_en_dis == 1) && (peripheral_flags.lls_hls_en_dis >= 2) &&(peripheral_flags.chiller_heater_lls_trigger == FALSE))
                    {
                        
                        if( (data_buffer[0] == '1')&&(peripheral_flags.chiller_heater_en_dis==0) )
                        {
                            lcd_init();
                            strcpy(lcd_data, "CHILLER ENABLE  "); //v2.3.6 - EN to ENABLE
                            lcd_display(1);
                            peripheral_flags.chiller_heater_en_dis = 1; //v2.3.6
                            peripheral_flags.warm_water = 0;     //v3.1.2.d
                            water_temperature_level = 20;					//v2.3.6
			    eeprom_write(SER_WATER_TEMPERATURE_LEVEL);		//v2.3.6
                           
                        }
                        else if( (data_buffer[0] == '2')&&(peripheral_flags.chiller_heater_en_dis==0) ) //v2.3.6
                        {
                            lcd_init();
                            strcpy(lcd_data, "HEATER ENABLE   ");
                            lcd_display(1);
                            peripheral_flags.chiller_heater_en_dis = 2; //v2.3.6
                            peripheral_flags.cold_water = 0;     //v3.1.2.d
                            water_temperature_level = 45;					//v2.3.6
                            eeprom_write(SER_WATER_TEMPERATURE_LEVEL);		//v2.3.6
                        }
                        else if (data_buffer[0] == '0')
                        {
                            lcd_init();
                            strcpy(lcd_data, "CHILER/HEATR DIS"); //v2.3.6
                            lcd_display(1);
                            peripheral_flags.chiller_heater_en_dis = 0; //v2.3.6
                        }
                        else
                        {
                            lcd_init();
                            if(peripheral_flags.chiller_heater_en_dis){
                                    strcpy(lcd_data,"PLEASE DISABLE  ");
                                    lcd_display(1);
                                    if(peripheral_flags.chiller_heater_en_dis==1)			//v2.3.6
                                            strcpy(lcd_data,"CHILLER FIRST..!!");
                                    else if(peripheral_flags.chiller_heater_en_dis==2)
                                            strcpy(lcd_data,"HEATER FIRST..!! ");
                                    else{}
                                    lcd_display(2);
                            }
                        }
                        timer2_sec_delay(error_display_time);
                        eeprom_write(SER_CHILLER_HEATER_EN_DIS); //v2.3.6
                        send_server_response(DEV_CHILLER_HEATER_EN_DIS); //v2.3.6
                        
                    }
                    else //V2.2.7
                    {
                        lcd_init();
                        if (peripheral_flags.temp_en_dis == 0) //3.1.2.C 
                        {
                            strcpy(lcd_data, "EN. TEMP SENSOR");
                            send_server_response(DEV_TEMPERATURE_EN_DIS);
                        }
                        else if (peripheral_flags.lls_hls_en_dis < 2)//3.1.2.C
                        {
                            strcpy(lcd_data, "EN. LLS SENSOR");
                            send_server_response(DEV_LLS_EN_DIS);
                        }
                        else
                        {
                        strcpy(lcd_data, "FILL TANK WATER"); //3.1.2.C    
                        }
                        lcd_display(1);
                        strcpy(lcd_data, "FIRST..!!");
                        lcd_display(2);
                        buzzer(ERR01);
                        timer2_sec_delay(error_display_time);
                    }
                    if ((peripheral_flags.temp_en_dis == 1)&&(peripheral_flags.chiller_heater_en_dis != 0))
                    { //Chiller / Heater enable get exact temperature to operate chiller/heater at actual temperature - average
                        read_temperature();                timer2_sec_delay(1);
                        read_temperature();                timer2_sec_delay(1);
                        read_temperature();                timer2_sec_delay(1);
                    } //v2.3.6
                    else
                    {
                        timer2_sec_delay(2);
                    }
                    break;

                case SER_REQ_DAILY_DISPENSE: //v3.0.0                           //84
                    server_data_req = (data_buffer[0] - 0x30);
                    if (server_data_req == 1)
                    {
                        strcpy(lcd_data, "DATA SENT       ");
                        lcd_display(2); //1.0.1
                        strcpy(lcd_data, "PLS WAIT....    ");
                        lcd_display(1);
                        FSchdir("\\");
                        FSchdir("TOTAL");
                        memset(file_name, '\0', sizeof (file_name));
                        sprintf(file_name, "%.8s.txt", "Cur_date"); //1.0.1  //v3.1.1.D													
                        //sprintf(file_name,"%.5s.txt","TOTAL"); 
                        FSfclose(sd_pointer); //@ f_close(&fil);
                        sd_pointer = FSfopen(file_name, FS_READPLUS);
                        if (FSfread(daily_total_lit_ary, 1, 10, sd_pointer) != 10)
                        {
                        }
                        else
                        {
                        }
                        FSfclose(sd_pointer);
                        FSchdir("\\");
                        daily_total_lit = atol(daily_total_lit_ary);
                        daily_total_dispense = (double) daily_total_lit / 100.0;
                        timer2_sec_delay(error_display_time);

                        send_server_response(DEV_DAILY_WATER_DISPENSE + 200); //v3.0.0
                        clear_lcd();
                        sprintf(lcd_data, "DISP.=%4.1f", ((double) daily_total_dispense));
                        lcd_display(1);
                        timer2_ms_delay(2000);

                        for (cntr_1 = 2; cntr_1<sizeof (data_buffer); cntr_1++) //v3.0.2-27/07/16
                            dummy_master_contact[cntr_1 - 2] = master_contact[cntr_1];

                        temp_count = strstr(sms_sender_no, dummy_master_contact);
                        if (temp_count != 0)
                        {
                            FSchdir("\\");
                            FSchdir("TOTAL"); //f_chdir("/TOTAL");
                            memset(file_name, '\0', sizeof (file_name));
                            sprintf(file_name, "%.8s.txt", "Cur_date"); //1.0.1 //v3.1.1.D
                            //sprintf(file_name,"%.5s.txt","TOTAL");  //v3.1.1.D
                            strcpy(daily_total_lit_ary, "0000000000");
                            FSfclose(sd_pointer);
                            sd_pointer = FSfopen(file_name, FS_READPLUS);
                            if (FSfwrite(daily_total_lit_ary, 1, 10, sd_pointer) != 10)
                            {
                            }
                            else
                            {
                            }
                            FSfclose(sd_pointer);
                            FSchdir("\\");
                        }
                        else
                        {
                        }

                    }
                    else if (server_data_req == 0)
                    {
                        //Serve_daily_dispense();
                    }
                    else
                    {
                    }
                    //send_server_response(DEV_DAILY_WATER_DISPENSE);		//v3.0.0
                    //timer2_ms_delay(50);
                    //send_server_response(DEV_COIN_CHN_STATUS);
                    break;

                case SER_SERVER_SYNC: //v3.0.0				//85
                    if (delete_msg_goto_normal_mode() == 1)
                    {
                        consumer_flags.dispense_button = 0; //v2.2.5    //IMP because when button Long Pressed to Dis. the mode it jumps to switch case in main.c and water dispense starts whenever corporate mode is ON		
                        Server_Sync_2(SEARCH_PEN, FILE_SEND_EN);
                        consumer_flags.dispense_button = 0;
                    }
                    else
                    {
                    }
                    break;

                case SER_TIME_SYNC_BY_MODEM: //86
                    rtc_flag.sync_by_modem = 1;
                    rtc_flag.sync_type = BY_MODEM;
                    get_modem_local_time(); //SET_TIME_BY_MODEM				
                    break;

                case SER_OFFLINE_WATER_TARIFF: //v2.2.9                     //87			
                    offline_water_tariff = atoi(data_buffer);
                    temp_count = 0;
                    temp_count = data_validation(SER_OFFLINE_WATER_TARIFF);
                    if (temp_count == 0)
                        eeprom_write(SER_OFFLINE_WATER_TARIFF);
                    else
                    {
                    }
                    send_server_response(DEV_OFFLINE_WATER_TARIFF);
                    break;

                case SER_MENU_PASSWORD_SET: //Change SMS Password-19        //88
                    strcpy(menu_franchisee_password, data_buffer);
                    temp_count = 0;
                    temp_count = data_validation(SER_MENU_PASSWORD_SET);
                    if (temp_count == 0)
                        eeprom_write(SER_MENU_PASSWORD_SET);
                    else
                    {
                    }
                    send_server_response(DEV_MENU_PASSWORD_SET);
                    break;

                case SER_SYNC_DATE: //89
                    if (delete_msg_goto_normal_mode() == 1)
                    {
                        memset(sd_sync_buffer, '\0', sizeof (sd_sync_buffer));
                        //sprintf(sd_sync_buffer,"%.8s.txt",data_buffer);  //data copy into sd_sync bbuffer	
                        sprintf(sd_sync_buffer, "%.8s", data_buffer); //data copy into sd_sync bbuffer	
                        sprintf(sd_sync_buffer, "%s.txt", sd_sync_buffer); //data copy into sd_sync bbuffer	
                        clear_lcd();
                        if (strlen(sd_sync_buffer) <= 12) //v3.1.1.D
                        {
                            if ((sd_sync_buffer[2] == '_') && (sd_sync_buffer[5] == '_') && (sd_sync_buffer[8] == '.')&& (sd_sync_buffer[9] == 't')&& (sd_sync_buffer[10] == 'x')&&(sd_sync_buffer[11] == 't'))
                            {
                                strcpy(lcd_data, "CMD SUCCESS"); //3.1.2.J
                                lcd_display(1);
                                timer2_sec_delay(3);
                                single_date_sync = 1;
                                Server_Sync_2(SEARCH_PEN, FILE_SEND_EN);
                            }
                            else
                            {
                                strcpy(lcd_data, "CMD FAILED");
                                lcd_display(1);
                                timer2_sec_delay(3);
                            }
                        }
                        else
                        {
                            strcpy(lcd_data, "CMD FAILED..");
                            lcd_display(1);
                            timer2_sec_delay(3);
                        }
                    }
                    else
                    {
                    }
                    memset(sd_sync_buffer, '\0', sizeof (sd_sync_buffer));
                    break;

                    //		    case SER_USER_ADD:		//v2.2.9                        //90													
                    //			 break;

                case SER_TDS_VERSION: //v2.2.9                              //91
                    if (system_flags.super_admin == 1)
                    {
                        system_flags.super_admin = 0;
                        tds_header_version = data_buffer[0];
                        temp_count = 0;
                        temp_count = data_validation(SER_TDS_VERSION);
                        if (temp_count == 0)
                            eeprom_write(SER_TDS_VERSION);
                        else
                        {
                        }
                        send_server_response(DEV_TDS_VERSION);
                    }
                    else
                    {
                        lcd_init();
                        strcpy(lcd_data, "INSUFFICIENT");
                        lcd_display(1);
                        strcpy(lcd_data, "COMMAND CODE");
                        lcd_display(2);
                        timer2_sec_delay(error_display_time);
                    }
                    break;

                case SER_TEMP_COMPEN_EN_DIS: //v2.2.9			//92
                    if (system_flags.super_admin == 1)
                    {
                        system_flags.super_admin = 0;
                        if (data_buffer[0] == '1')
                            peripheral_flags.temp_compen_en_dis = TRUE;
                        else if (data_buffer[0] == '0')
                            peripheral_flags.temp_compen_en_dis = FALSE;
                        else
                        {
                        }
                        eeprom_write(SER_TEMP_COMPEN_EN_DIS);
                        send_server_response(DEV_TEMP_COMPEN_EN_DIS);
                    }
                    else

                    {
                        lcd_init();
                        strcpy(lcd_data, "INSUFFICIENT");
                        lcd_display(1);
                        strcpy(lcd_data, "COMMAND CODE");
                        lcd_display(2);
                        timer2_sec_delay(error_display_time);
                    }
                    break;

                case SER_AUTO_SYNC_EN_DIS: //Enable/Disable                 //93	
                    if (data_buffer[0] == '1')
                        system_flags.auto_sync_flag = TRUE;
                    else if (data_buffer[0] == '0')
                        system_flags.auto_sync_flag = FALSE;
                    else
                    {
                    }
                    eeprom_write(SER_AUTO_SYNC_EN_DIS);
                    send_server_response(DEV_AUTO_SYNC_EN_DIS);
                    break;

                case SER_SYNC_ERR_FILE: //94			
                    if (delete_msg_goto_normal_mode() == 1)
                    {
                        consumer_flags.dispense_button = 0; //v2.2.5    //IMP because when button Long Pressed to Dis. the mode it jumps to switch case in main.c and water dispense starts whenever corporate mode is ON	
                        Server_Sync_2(SEARCH_ERR, FILE_SEND_EN);
                        consumer_flags.dispense_button = 0;
                    }
                    else
                    {
                    }
                    break;

                case SER_SYNC_FILE_UPDATE: //95
                    if (delete_msg_goto_normal_mode() == 1)
                    {
                        err_pen_file_counter = 0;
                        Server_Sync_2(SEARCH_PEN, FILE_SEND_DIS); //err_pen_file_counter+ = count no of PEN file                    
                        Server_Sync_2(SEARCH_ERR, FILE_SEND_DIS); //err_pen_file_counter+=  Count no of ERR file
                        if (err_pen_file_counter <= 0)
                        {
                            if (search_file("SYNC.TXT", FILE_FIND_DELETE_REMAKE))
                            {
                                server_sync_status = 5;
                                send_server_response(DEV_SERVER_SYNC);
                            }
                            else
                            {
                            }
                        }
                        else
                        {
                        }
                    }
                    else
                    {
                    }
                    break;
                    //     ----------   v3.1.2.J   --------------
                case SER_MODEM_CHECK_ROUTINE:   //v3.1.2.J    //96
                    
                    modem_status_check_hour = atoi(data_buffer);
                    if( ( modem_status_check_hour >=0 ) && (modem_status_check_hour <=24)  )
                    {                                                
                        eeprom_write(SER_MODEM_CHECK_ROUTINE);
                        timer_flags.modem_status_check =0 ;
                        if (modem_status_check_hour == 0 )
                                { modem_status_check_min = 0;   } 
                        else if (modem_status_check_hour == 1 )
                                { modem_status_check_min = 240 ; }
                        else 
                                { modem_status_check_min  = (modem_status_check_hour * 60);  }
                    }
                    else
                    {
                        lcd_init();                        
                        strcpy(lcd_data, "INSUFFICIENT");
                        lcd_display(1);
                        strcpy(lcd_data, "COMMAND CODE");
                        lcd_display(2);
                        timer2_sec_delay(error_display_time);
                        modem_status_check_hour =0;
                        read_eeprom_parameter( SER_MODEM_CHECK_ROUTINE );
                        modem_status_check_min  = (modem_status_check_hour * 60);
                    }
                    if (modem_status_check_min==0)
                    { 
                          timer_flags.modem_status_check_en_dis  = 0; 
                    }
                    else{ timer_flags.modem_status_check_en_dis  = 1;}
                    
                    //     ----------   v3.1.2.J   --------------
                    break;

                case SER_ERASE_SIGNATURE: //99
                    if (mode_flags.op_mode == DEBUG_MODE)
                    {
                        eeprom_write(SER_ERASE_SIGNATURE);
                        send_server_response(DEV_DFLT_PARA);
                    }
                    else
                    {
                        lcd_init();
                        strcpy(lcd_data, "INSUFFICIENT");
                        lcd_display(1);
                        strcpy(lcd_data, "COMMAND CODE");
                        lcd_display(2);
                        timer2_sec_delay(error_display_time);
                    }
                    break;

                case SER_FORMAT_SD_CARD: //150                                   
                    if (system_flags.ultra_super_admin_state == TRUE)
                    {
                        strcpy(lcd_data, "DATA FORMAT");
                        lcd_display(1);
                        strcpy(lcd_data, "PROCESS RUNNING.");
                        lcd_display(2);
                        if (!FSformat(0, 0x12345678, "Mycard"))
                        {
                            P_month = 0;
                            P_date = 0;
                            eeprom_write(SER_PREV_DATE);
                            eeprom_write(SER_PREV_MONTH);
                            sd_flags.sd_card_format = 1;
                            file_create();
                            clear_lcd();
                            strcpy(lcd_data, "DATA FORMAT");
                            lcd_display(1);
                            strcpy(lcd_data, "COMPLETED");
                            lcd_display(2);
                        }
                        else
                        {
                            clear_lcd();
                            strcpy(lcd_data, "DATA FORMAT");
                            lcd_display(1);
                            strcpy(lcd_data, "FAILED...");
                            lcd_display(2);
                            sd_flags.sd_card_format = 0;
                        }
                        timer2_ms_delay(error_display_time);
                        send_server_response(DEV_FORMAT_SD_CARD);
                    }
                    else
                    {
                    }
                    break;

                case SER_FETCH_RTC_TIME: //151
                    get_local_time();
                    sprintf(lcd_data, "%.2d/%.2d/%.2d [%.2d/%.2d]", sd_date, sd_month, sd_yr, P_date, P_month);
                    lcd_display(1);
                    sprintf(lcd_data, "%.2d:%.2d:%.2d [%.2d]", sd_hour, sd_min, sd_sec, P_yr);
                    lcd_display(2);
                    timer2_sec_delay(error_display_time);
                    send_server_response(DEV_FETCH_RTC_TIME + 200);
                    break;

                    // ---------------------------------------   COIN MODULE MSG s     -------------------------------------------------  //  2.2.9
                case SER_COIN_EN_DIS: //v2.2.9                                  //74
                    coin_flags.coin_debug  = 1;  //3.1.2.I
                    //if ((system_error == ERROR_FREE)&&(mode_flags.op_mode != DEBUG_MODE)&&(mode_flags.op_mode != DIAGNOSTIC_MODE)&&(mode_flags.op_mode != ANTENNA_MODE)&&(mode_flags.op_mode != SCHOOL_MODE)&&(mode_flags.op_mode != CORPORATE_MODE)&&(mode_flags.op_mode != DUMP_MODE)&&(mode_flags.op_mode != REFILL_MODE)) //v2.3.5.b
                    UART3_init();         //v3.1.2.J
                    timer2_sec_delay(1);  //v3.1.2.J
                    if((coin_flags.coin_disable_fault_alert==0)  && (system_error == ERROR_FREE)&&(mode_flags.op_mode != DEBUG_MODE)&&(mode_flags.op_mode != DIAGNOSTIC_MODE)&&(mode_flags.op_mode != ANTENNA_MODE)&&(mode_flags.op_mode != SCHOOL_MODE)&&(mode_flags.op_mode != CORPORATE_MODE)&&(mode_flags.op_mode != DUMP_MODE)&&(mode_flags.op_mode != REFILL_MODE)) //v2.3.5.b
                    {
                        if ((data_buffer[0] == '1')&&(peripheral_flags.dev_en_dis)&&(coin_flags.coin_collector_error == 0))
                        {
                            //peripheral_flags.coin_en_dis = 1;                        
                            coinbox_init();
                            if (coin_flags.coinenable == TRUE) //v2.3.2
                                peripheral_flags.coin_en_dis = 1; //v2.3.2
                            else
                            {
                            }

                            read_eeprom_parameter(SER_COIN_WATER_TARIFF);
                            eeprom_write(SER_COIN_EN_DIS);
                            send_server_response(DEV_COIN_EN_DIS);
                        }
                        else if ((data_buffer[0] == '0')&&(peripheral_flags.dev_en_dis)&&(coin_flags.coin_collector_error == 0))
                        {
                            //peripheral_flags.coin_en_dis = 0;
                            coinbox_disable();
                            if (coin_flags.coinenable == FALSE) //v2.3.2
                                peripheral_flags.coin_en_dis = 0; //v2.3.2
                            else{}
                            eeprom_write(SER_COIN_EN_DIS);
                            send_server_response(DEV_COIN_EN_DIS);
                        }
                        else
                        {
                            coinbox_disable(); //v2.3.5  //V6
                            if (coin_flags.coinenable == FALSE) //v2.3.5
                            {
                                peripheral_flags.coin_en_dis = 0; //v2.3.5
                                DisableIntU3RX ;  //v3.1.2.J
                            }
                            else{}

                            if (coin_flags.coin_collector_error)
                            {
                                send_server_response(DEV_COIN_COLLECTOR_ERROR);
                                strcpy(lcd_data, "COIN COLLECTOR");
                                lcd_display(1);
                                strcpy(lcd_data, "FULL...!!!");
                                lcd_display(2);
                                timer2_sec_delay(error_display_time);
                            }
                            else
                            {
                                lcd_init();
                                strcpy(lcd_data, "WRONG COMMAND   ");
                                lcd_display(1);
                                strcpy(lcd_data, "CONTACT SARVAJAL");
                                lcd_display(2);
                                timer2_sec_delay(error_display_time);
                            }

                        }
                        //				    if (peripheral_flags.dev_en_dis)   //V6
                        //				    {
                        //					eeprom_write(SER_COIN_EN_DIS);
                        //					timer2_ms_delay(50);
                        //					send_server_response(DEV_COIN_EN_DIS);
                        //				    } else{}
                    }
                    else //v2.3.5.b
                    {
                        lcd_init();
                        strcpy(lcd_data, "CHECK DEVICE MOD");
                        lcd_display(1);
                        strcpy(lcd_data, "CONTACT SARVAJAL");
                        lcd_display(2);
                        timer2_sec_delay(error_display_time);
                    }
                    coin_flags.coin_debug  = 0;  //3.1.2.I
                    break;

                case SER_COIN_CHN_STATUS: //v2.2.9				//75

                    read_eeprom_parameter(SER_COIN_CHN_EN_DIS);
                    timer2_ms_delay(50);
                    send_server_response(DEV_COIN_CHN_STATUS);
                    break;

                case SER_TOTAL_COIN_AMOUNT: //v2.2.9			    	//76

                    if ((total_coin_amount >= 0)&&(total_coin_amount <= 25000))
                        send_server_response(DEV_TOTAL_COIN_AMOUNT);
                    else
                    {
                        read_eeprom_parameter(SER_TOTAL_COIN_AMOUNT);
                        if ((total_coin_amount >= 0)&&(total_coin_amount <= 25000))
                            send_server_response(DEV_TOTAL_COIN_AMOUNT);
                        else
                        {
                            total_coin_amount = 55555;
                            send_server_response(DEV_TOTAL_COIN_AMOUNT);
                            total_coin_amount = 0;
                            coin_flags.coin_collector_error = 1;
                            //peripheral_flags.coin_en_dis = 0;
                            coinbox_disable();
                            if (coin_flags.coinenable == FALSE) //v2.3.2
                                peripheral_flags.coin_en_dis = 0; //v2.3.2
                            else
                            {
                            }
                            eeprom_write(SER_COIN_EN_DIS);
                            eeprom_write(SER_COIN_COLLECTOR_ERROR);
                            send_server_response(DEV_COIN_COLLECTOR_ERROR);
                            strcpy(sms_sender_no, "ERR_DISABL");
                            send_server_response(DEV_COIN_EN_DIS);
                        }
                    }

                    break;

                case SER_TOTAL_COIN_COUNT: //v2.2.9	    			//77

                    if ((total_coin_count >= 0)&&(total_coin_count <= 10000))
                        send_server_response(DEV_TOTAL_COIN_COUNT);
                    else
                    {
                        read_eeprom_parameter(SER_TOTAL_COIN_COUNT);
                        if ((total_coin_count >= 0)&&(total_coin_count <= 10000))
                            send_server_response(DEV_TOTAL_COIN_AMOUNT);
                        else
                        {
                            total_coin_count = 55555;
                            send_server_response(DEV_TOTAL_COIN_COUNT);
                            total_coin_count = 0;
                            coin_flags.coin_collector_error = 1;
                            //peripheral_flags.coin_en_dis = 0;
                            coinbox_disable();
                            if (coin_flags.coinenable == FALSE) //v2.3.2
                                peripheral_flags.coin_en_dis = 0; //v2.3.2
                            else
                            {
                            }

                            eeprom_write(SER_COIN_EN_DIS);
                            eeprom_write(SER_COIN_COLLECTOR_ERROR);
                            send_server_response(DEV_COIN_COLLECTOR_ERROR);
                            strcpy(sms_sender_no, "ERR_DISABL");
                            send_server_response(DEV_COIN_EN_DIS);
                        }
                    }

                    break;

                case SER_COIN_WATER_TARIFF: //v2.2.9				//78

                    coin_water_tariff = atoi(data_buffer);
                    //					coin_water_tariff = atof(data_buffer);

                    temp_count = 0; //v2.3.5
                    temp_count = data_validation(SER_COIN_WATER_TARIFF); //v2.3.5
                    if (temp_count == 0) //v2.3.5
                        eeprom_write(SER_COIN_WATER_TARIFF); //v2.3.5
                    else
                    {
                    } //v2.3.5

                    send_server_response(DEV_COIN_WATER_TARIFF);

                    break;

                case SER_COIN_COLLECT_CAPACITY: //v2.2.9			//79

                    coin_collector_capacity = atoi(data_buffer);
                    //				coin_water_tariff = atof(data_buffer);

                    temp_count = 0; //v2.3.5
                    temp_count = data_validation(SER_COIN_COLLECT_CAPACITY); //v2.3.5 
                    if (temp_count == 0) //v2.3.5
                        eeprom_write(SER_COIN_COLLECT_CAPACITY); //v2.3.5
                    else
                    {
                    } //v2.3.5

                    send_server_response(DEV_COIN_COLLECT_CAPACITY);

                    break;

                case SER_COIN_CHN_EN_DIS: //v2.2.9        			//80
                    coin_flags.coin_debug  = 1;  //3.1.2.I
                    coin_channel_no = (data_buffer[0] - 0x30);
                    coin_channel_En_Dis = (data_buffer[1] - 0x30);
                    temp_count = 0;
                    temp_count = data_validation(SER_COIN_CHN_EN_DIS);
                    if (temp_count == 0)
                    {
                        coin_channel_en_dis_cmd();
                        //					if(coin_flags.coin_channel_status == TRUE){
                        //						eeprom_write(SER_COIN_CHN_EN_DIS);
                        //						timer2_ms_delay(50);
                        //						coin_flags.coin_channel_status = FALSE;
                        //					}
                        //					else{
                        //						coin_channel_En_Dis = 0;
                        //					}
                    }
                    else
                    {
                    }
                    send_server_response(DEV_COIN_CHN_EN_DIS);
                    coin_flags.coin_debug  = 0;  //3.1.2.I
                    break;

                case SER_COIN_ID: //v2.2.9					//81

                    coin_id_channel_no = (data_buffer[0] - 0x30);
                    for (cntr_1 = 0; cntr_1 < 13; cntr_1++)
                    {
                        temp_coin_id[cntr_1] = data_buffer[cntr_1 + 1];
                    }
                    temp_count = 0;
                    temp_count = data_validation(SER_COIN_ID);
                    if (temp_count == 0)
                    {
                        eeprom_write(SER_COIN_ID);
                        timer2_ms_delay(50);
                    }
                    else
                    {
                    }
                    send_server_response(DEV_COIN_ID);

                    break;

                case SER_COIN_ID_STATUS: //v2.2.9          			//82	

                    read_eeprom_parameter(SER_COIN_CHN_EN_DIS);
                    timer2_ms_delay(50);
                    bit_extract_factor = 0x80;
                    for (cntr_1 = 1; cntr_1 < 9; cntr_1++)
                    {
                        if ((channel_cmd & bit_extract_factor) != 0)
                        {
                            eeprom_channel_read = cntr_1;
                            read_eeprom_parameter(SER_COIN_ID);
                            timer2_ms_delay(50);
                            coin_id_channel_no = eeprom_channel_read;
                            //send_server_response(DEV_COIN_ID_STATUS);
                            send_server_response(DEV_COIN_ID);
                        }
                        else
                        {
                        }
                        bit_extract_factor = bit_extract_factor >> 1;
                    }

                    break;

                case SER_RESET_COIN_AMOUNT_COUNT: //v2.2.9		        //83

                    if (mode_flags.op_mode == DEBUG_MODE)
                    {
                        send_server_response(DEV_RESET_COIN_AMOUNT);
                        total_coin_amount = 0;
                        total_coin_count = 0;
                        eeprom_write(SER_TOTAL_COIN_AMOUNT);
                        eeprom_write(SER_TOTAL_COIN_COUNT);
                        //send_server_response(DEV_RESET_COIN_AMOUNT);
                        coin_flags.coin_collector_alert = 0;
                        coin_flags.coin_collector_error = 0;
                        //peripheral_flags.coin_en_dis = 1;			// Auto Coin Enable on Reset coin counter
                        peripheral_flags.dev_en_dis = TRUE;

                        if (system_error == ERROR_FREE)
                        {
                            coinbox_init();
                            if (coin_flags.coinenable == TRUE) //v2.3.2
                                peripheral_flags.coin_en_dis = 1; //v2.3.2
                            else
                            {
                            }
                        }
                        else
                        {
                        }


                        if (school_mode_en)
                            mode_flags.op_mode = SCHOOL_MODE;
                        else if (corporate_mode_en) //v2.2.5
                            mode_flags.op_mode = CORPORATE_MODE;
                        else
                            mode_flags.op_mode = IDLE_MODE;

                        eeprom_write(SER_COIN_COLLECTOR_ERROR);
                        eeprom_write(SER_COIN_EN_DIS);
                        eeprom_write(SER_OP_MODE);
                        eeprom_write(SER_DEVICE_EN_DIS);
                        send_server_response(DEV_COIN_EN_DIS);
                        //eeprom_write(SER_DEBUG_MODE);
                        //eeprom_write(SER_DIAGNOSTIC_TEST);					
                    }
                    else
                    {
                        lcd_init();
                        strcpy(lcd_data, "INSUFFICIENT");
                        lcd_display(1);
                        strcpy(lcd_data, "COMMAND CODE");
                        lcd_display(2);
                        timer2_sec_delay(error_display_time);
                    }

                    break;
                    // ---------------------------------------   COIN MODULE MSG E     -------------------------------------------------  //

                    //*****************************************************************************************/
                default:
                    break;
                }//switch case
            }//super admin
            else if (system_flags.ultra_super_admin == 1)
            {
                sprintf(lcd_data, "SUPER ADMIN=%d", server_code);
                lcd_display(1);
                timer2_sec_delay(error_display_time);

                mode_flags.op_mode = ULTRA_SUPER_ADMIN_MODE;
                eeprom_write(SER_ULTRA_SUPER_ADMIN_MODE);
                eeprom_write(SER_OP_MODE);

                mode_flags.forced_function_state = TRUE; //0.0.1
                function_state = ST_ULTRA_SUPER_ADMIN_MODE; //0.0.1
                system_flags.ultra_super_admin = 0;
                system_flags.ultra_super_admin_state = 1;
                send_server_response(DEV_ULTRA_SUPER_ADMIN_MODE);
                if(peripheral_flags.coin_en_dis){coinbox_disable();} else {}	//v3.1.2.I
            }
            else
            {
            }
        }//while close
    }//if end(pointer!=0)
    else
    {
    }

    for (cntr_1 = 0; cntr_1<sizeof (sms_sender_no); cntr_1++) //reset string
        sms_sender_no[cntr_1] = 0;
    clear_lcd();
    LCD_BACKLITE_OFF;
}

unsigned char read_sms(unsigned int no)
{
    char ss_buffer[20] = {0};
    unsigned char char_data = 0, result = 0;
    unsigned char counter_1 = 0, counter_2 = 0;
    char* ptr;

    gsm_flags.is_sms_from_server = 0;

    for (counter_1 = 0; counter_1<sizeof (sms_sender_no); counter_1++) //reset string
        sms_sender_no[counter_1] = 0;

    for (counter_1 = 0; counter_1<sizeof (sms_string); counter_1++) //reset string	
        sms_string[counter_1] = 0;

    for (counter_1 = 0; counter_1<sizeof (sms_buffer); counter_1++) //reset string	
        sms_buffer[counter_1] = 0;

    for (counter_1 = 0; counter_1<sizeof (sms_txn); counter_1++) //reset string	
        sms_txn[counter_1] = 0;

    sprintf(ss_buffer, "AT+CMGR=%u\r\n", no); //send sms read command to modem	
    serial_string_send(ss_buffer);
    timer2_ms_delay(10);
    counter_2 = 0;

    /*---------------------------------SMS format followed----------------------------------

    +CMGR: "REC READ","<mobile no>","","<date & time>"<sms data>

    eg:+CMGR: "REC READ","+918128660427","","11/09/27,11:37:46+22"@R4,1234,122.170.012.160#

    ----------------------------------------------------------------------------------------*/

    if (check_modem_reply("+CMGR:", 6, 2000)) //check for string "+CMGR: (indicate sms read)
    {
               timer2_ms_delay(2000);
        for (counter_1 = 0; counter_1 < MAX_SMS_LENGTH; counter_1++) //capture full sms data in "sms_buffer" without any checking
        {
            char_data = serial_byte_receive();
            sms_buffer[counter_1] = char_data;
        }
    }
    else
    {
        gsm_flags.is_sms_from_server = 0;
        result = 0;
        return result; //no sms
    }

    ptr = strstr(sms_buffer, "R23"); //check for factory password msg

    if (ptr == 0)
        gsm_flags.fact_password_ok = 0; //No factory password detected
    else
    {
        ptr = strstr(sms_buffer, factory_password); //3.1.0	"WA20"//v3.1.1.C
        if (ptr == 0)
            gsm_flags.fact_password_ok = 0; //No factory password detected
        else
        {
            gsm_flags.fact_password_ok = 1; //Factory password detected
            gsm_flags.is_sms_from_server = 1; //sms authentication passed(password validated)
            for (counter_1 = 0; counter_1 < (sizeof (ss_buffer)); counter_1++) //reset buffer	
                ss_buffer[counter_1] = 0;
        }
    }

    if (gsm_flags.fact_password_ok == 0) //check current password only if factory password not found
    {
        ptr = strstr(sms_buffer, current_password); //check for "password" in the "sms_buffer" sms data
        //it return 0 if 2nd string is not found in 1st string 
        //it returns pointer of the location where 2nd string starts in 1st string 
        if (ptr == 0)
        {
            gsm_flags.is_sms_from_server = 0;
            result = 1; //sms authentication failed
            return result;
        }
        else
        {
            gsm_flags.is_sms_from_server = 1; //sms authentication passed(password validated)
            for (counter_1 = 0; counter_1 < (sizeof (ss_buffer)); counter_1++) //reset buffer	
                ss_buffer[counter_1] = 0;
        }
    }
    else
    {
    }



    if (gsm_flags.is_sms_from_server == 1)
    {
        strcpy(ss_buffer, "READ");
        ptr = strstr(sms_buffer, ss_buffer); //check the "READ" string, ie.location just before the sender no.(refer sms format above)

        if (ptr == 0)
        {
            gsm_flags.is_sms_from_server = 0;
            result = 2; //sms authentication failed
            return result;
        }
        else
        {
            counter_2 = 0;
            // ------------- v3.2.0.A ---------------  //
            while(   (*++ptr != '"')&& (counter_2<8)  )
            {
                counter_2++;
            }
            
            while(   (*++ptr != '"')&& (counter_2<8)  )
            {
                counter_2++;
            }
            *ptr++;
              // ------------- v3.2.0.A ---------------  //
            counter_2 =0;
            //for (counter_1 = 0; counter_1 < 20; counter_1++) //capture the sms sender no. from sms buffer //v3.1.2
            for (counter_1 = 0; counter_1 < 15; counter_1++) //capture the sms sender no. from sms buffer  //v3.2.0.A 20 to 25
            {
                if ((*ptr >= '0') && (*ptr <= '9')) //check for only no. data
                {
                    sms_sender_no[counter_2] = *ptr;
                    counter_2++;
                }
                else{}   
                ptr++;
                if (counter_2>sizeof (sms_sender_no)) //13	//break if limit over
                    break;
                else{}
            }
        }

        strcpy(ss_buffer, "@R");
        ptr = strstr(sms_buffer, ss_buffer); //check the string "@R",ie start of desired msg
        if (ptr == 0)
        {
            strcpy(ss_buffer, "@O");
            ptr = strstr(sms_buffer, ss_buffer); //check the string "@R",ie start of desired msg
        }
        else{}
      
        if (ptr == 0)
        {
            gsm_flags.is_sms_from_server = 0;
            result = 3;
            return result; //sms authentication failed
        }
        else
        {
            for (counter_1 = 0; counter_1<sizeof (sms_string); counter_1++) //capture the sms desired data to be processed
            {
                sms_string[counter_1] = *++ptr;

                if ((sms_string[counter_1] != ',') && (counter_1 < 4)) //v2.2.9 <4 instead of <3 for RM91
                    sms_txn[counter_1] = sms_string[counter_1]; //store the sms type
                else{}

                if (*ptr == '#') //break if end of data '#' encountered
                {
                    result = 4; //sms validated to be from server
                    break;
                }
                else{}                
            }
        }
    }
    else{}
    return result;
}

unsigned char scan_sms_memory(void)
{

    //+CMTI: "SM",1 //when SMS received, "1" is location of sms stored
    unsigned char loop = 0;
    char* temp_ptr = 0;

    new_sms_location = 0;

    temp_ptr = strstr(Rxdata, "+CMTI:");

    if (temp_ptr != 0)
    {
        for (loop = 0; loop < 20; loop++)
        {
            if (*++temp_ptr == ',')
                break;
        }
        new_sms_location = *++temp_ptr;
    }
    if (new_sms_location)
        return 1;
    else
        return 0;
}

unsigned char get_modem_local_time()
{
    unsigned char local_time_stamp[21] = {0}, modem_get_time_fail = 0;
    unsigned char loop_1 = 0, result_get = 0, count_err = 0;
    unsigned char modem_sd_yr = 0, modem_sd_month = 0, modem_sd_date = 0, modem_sd_hour = 0, modem_sd_min = 0, modem_sd_sec = 0;
    char data_buffer[10] = {0};
    char* local_ptr = 0;
    unsigned int rtc_sec_time_stamp = 0, rtc_min_time_stamp = 0;
    rtc_flag.time_halt = 0;
    modem_get_time_fail = 0;
    if ((rtc_flag.sync_by_modem == 0) && (rtc_flag.halt_time_check == 1))
    {
        get_local_time();
        rtc_sec_time_stamp = sd_sec;
        rtc_min_time_stamp = sd_min;
        clear_lcd();
        strcpy(lcd_data, "TIME CHECK    ");
        lcd_display(1);
        strcpy(lcd_data, "PLS WAIT..... ");
        lcd_display(2);
        timer2_sec_delay(3);
        get_local_time();
        if (((sd_sec == rtc_sec_time_stamp)&&(rtc_min_time_stamp == sd_min)) || ((sd_date == 1)&&(sd_month == 1) && (sd_yr != 17)))
        {
            //strcpy(lcd_data,"NOT WORKING");lcd_display(2);
            rtc_flag.time_halt = 1;
            rtc_flag.sync_type = BY_RTC_HALT;
        }
        else
        {
            //strcpy(lcd_data,"WORKING GOOD");lcd_display(2);
            rtc_flag.time_halt = 0;
        }
        timer2_sec_delay(2);
    }
    else{}

    if ((rtc_flag.sync_by_modem == 1) || (rtc_flag.time_halt == 1) || (rtc_flag.sync_by_time == 1))
    {
        memset(local_time_stamp, 0, sizeof (local_time_stamp)); 
        result_get = FALSE;
        while ((count_err < 3)&&(result_get == FALSE)) //1.0.1 - 21/11
        {
            //	     strcpy(lcd_data,"FETCH MODEM TIME");lcd_display(1);
            //	     timer2_sec_delay(1);
            serial_string_send("AT+CCLK?\r\n");
            if (check_modem_reply("+CCLK:", 6, 3000))
            {
                //	     strcpy(lcd_data,"RTC FETCH TIME");lcd_display(1);
                timer2_sec_delay(1);
            }
            strcpy(data_buffer, "+CCLK:");
            local_ptr = strstr(Rxdata, data_buffer);
            if (local_ptr != 0) //string matched
            {
                local_ptr += 8;
                for (loop_1 = 0; loop_1 < 20; loop_1++)
                {
                    local_time_stamp[loop_1] = *local_ptr;
                    if (*++local_ptr == '"')
                        break;
                    else
                    {
                    }
                }
                if ((local_time_stamp[0] == '0')&&(local_time_stamp[1] == '0'))
                {
                    result_get = FALSE;
                    count_err++;
                    lcd_init();
                    strcpy(lcd_data, "RESTARTING MODEM");
                    lcd_display(1);
                    strcpy(lcd_data, "PLS. WAIT");
                    lcd_display(2);

                    MODEM_OFF;
                    timer2_ms_delay(5000); //delay
                    MODEM_ON;
                    timer2_sec_delay(10);
                    configure_sms_mode(); ///set modem -> ECHO off    
                }
                else
                {
                    result_get = TRUE;
                    break;
                }

            }
            else
            {
                count_err++;
                result_get = FALSE;
                serial_string_send("AT+CLTS=1\r\n");
                if (check_modem_reply("OK", 2, 3000))
                {
                    //		    strcpy(lcd_data,"MODEM RTC ON  2");lcd_display(1);
                }
                timer2_sec_delay(1);
            }
        }
        if (((local_time_stamp[0] == '0')&&(local_time_stamp[1] == '0')) || ((local_time_stamp[0] == 0x00)&&(local_time_stamp[1] == 0x00))) //v3.0.2 21/07/16  //3.0.3 - 17/11
        {
            read_eeprom_parameter(SER_PREV_YEAR);
            if ((P_yr >= 16) && (P_yr <= 99))
            {
                modem_sd_yr = P_yr;
                sd_year = modem_sd_yr + 2000;
            }
            else
            {
                modem_sd_yr = 17;
                sd_year = modem_sd_yr + 2000;
            }
            modem_get_time_fail = 1;
        }
        else
        {
            modem_sd_yr = (((local_time_stamp[0] - 0x30)*10) + (local_time_stamp[1] - 0x30));
            sd_year = 2000 + modem_sd_yr;
            P_yr = modem_sd_yr;
            //eeprom_write(SER_PREV_YEAR);						
        }
        if ((modem_get_time_fail == 1) || ((local_time_stamp[3] == '0')&&(local_time_stamp[4] == '0')) || ((local_time_stamp[3] == 0x00)&&(local_time_stamp[4] == 0x00))) //v3.0.2 21/07/16  //3.0.3 - 17/11
        {
            read_eeprom_parameter(SER_PREV_MONTH);
            modem_sd_month = P_month; //modem_sd_month= 1;read_eeprom_parameter(SER_PREV_DATE);	P_date = sd_date;P_month = sd_month;
        }
        else
        {
            modem_sd_month = (((local_time_stamp[3] - 0x30)*10) + (local_time_stamp[4] - 0x30));
            //eeprom_write(SER_PREV_MONTH);	
        }
        if ((modem_get_time_fail == 1) || ((local_time_stamp[6] == '0')&&(local_time_stamp[7] == '0')) || ((local_time_stamp[6] == 0x00)&&(local_time_stamp[7] == 0x00))) //v3.0.2 21/07/16  //3.0.3 - 17/11
        {
            read_eeprom_parameter(SER_PREV_DATE);
            modem_sd_date = P_date; //modem_sd_date = 1;
        }
        else
        {
            modem_sd_date = (((local_time_stamp[6] - 0x30)*10) + (local_time_stamp[7] - 0x30));
            //eeprom_write(SER_PREV_DATE);
        }
        if (modem_get_time_fail == 1)
        {
            modem_sd_hour = 0;
            modem_sd_min = 0;
            modem_sd_sec = 0;
            rtc_flag.sync_type = BY_EEPROM_TIME;
        }
        else
        {
            modem_sd_hour = (((local_time_stamp[9] - 0x30)*10) + (local_time_stamp[10] - 0x30));
            modem_sd_min = (((local_time_stamp[12] - 0x30)*10) + (local_time_stamp[13] - 0x30));
            modem_sd_sec = (((local_time_stamp[15] - 0x30)*10) + (local_time_stamp[16] - 0x30));
        }
    }
    else{}

    if ((rtc_flag.sync_by_time == 1)&&(rtc_flag.time_halt == 0))
    {
        get_local_time();
        if ((modem_get_time_fail == 0) && ((sd_hour >= 13) &&(sd_hour <= 17)) &&(modem_sd_hour == sd_hour)&&(modem_sd_yr == sd_yr)&&(modem_sd_month == sd_month)&&(modem_sd_date == sd_date)&&((modem_sd_min - sd_min) > 5)) 
        //if ((modem_get_time_fail == 0) && (modem_sd_hour == sd_hour)&&(modem_sd_yr == sd_yr)&&(modem_sd_month == sd_month)&&(modem_sd_date == sd_date)&&((modem_sd_min - sd_min) > 5))
        {
            rtc_flag.sync_by_time = 1;
            rtc_flag.sync_type = BY_TIME_DELAY;
        }
        else
        {
            rtc_flag.sync_by_time = 0;
        }
    }
    else{}

    if ((rtc_flag.sync_by_modem == 1) || (rtc_flag.time_halt == 1) || (rtc_flag.sync_by_time == 1))
    {
        // **********    CONVERT DECIMAL TO HEX i.e year 16 = year 0x16 (decimal 22)
        sd_yr = (((modem_sd_yr / 10) & 0x0f) << 4) + ((modem_sd_yr % 10) & 0x0f);
        sd_month = (((modem_sd_month / 10) & 0x0f) << 4) + ((modem_sd_month % 10) & 0x0f);
        sd_date = (((modem_sd_date / 10) & 0x0f) << 4) + ((modem_sd_date % 10) & 0x0f);
        sd_hour = (((modem_sd_hour / 10) & 0x0f) << 4) + ((modem_sd_hour % 10) & 0x0f);
        sd_min = (((modem_sd_min / 10) & 0x0f) << 4) + ((modem_sd_min % 10) & 0x0f);
        sd_sec = (((modem_sd_sec / 10) & 0x0f) << 4) + ((modem_sd_sec % 10) & 0x0f);
        sd_sec &= 0x7f;
        set_time_DS1307();
        timer2_ms_delay(100);
        get_local_time();
        sprintf(lcd_data, "%.2d/%.2d/%.2d [%.2d/%.2d]", sd_date, sd_month, sd_yr, P_date, P_month);
        lcd_display(1);
        sprintf(lcd_data, "%.2d:%.2d:%.2d [%.2d]", sd_hour, sd_min, sd_sec, P_yr);
        lcd_display(2);
        timer2_sec_delay(error_display_time);

        error_type2 = rtc_flag.sync_type;
        send_server_response(DEV_TIME_SYNC_BY_MODEM);
        send_server_response(DEV_ERROR_TYPE2);

    }//end of (rtc_flag.sync_by_modem==1) || (rtc_flag.time_halt ==1))
    else{}

    rtc_flag.halt_time_check = 0;
    rtc_flag.sync_by_modem = 0;
    rtc_flag.time_halt = 0;
    rtc_flag.sync_by_time = 0;
    rtc_flag.sync_type = 0;
    return 0; //v3.1.1.C
}

void configure_sms_mode()
{
    serial_string_send("ATE0\r\n");        check_modem_reply("OK", 2, 1000);    timer2_ms_delay(100);
    serial_string_send("AT+CMGF=1\r\n");   check_modem_reply("OK", 2, 1000);    timer2_ms_delay(100);//set text mode for sms   
    serial_string_send("AT+CLTS=1\r\n");   check_modem_reply("OK", 2, 3000);    timer2_ms_delay(100);
}

unsigned char delete_msg_goto_normal_mode()
{
    memset(file_name, '\0', sizeof (file_name));
    sprintf(file_name, "AT+CMGD=%u\r\n", sms_scan_no);
    while (BusyUART2()); //wait till the UART is busy	
    WriteUART2((unsigned int) 26);
    timer2_ms_delay(200);
    serial_string_send(file_name);
    if (check_modem_reply("OK", 2, 2000))
        memset(file_name, '\0', sizeof (file_name));

    if (mode_flags.op_mode == DEBUG_MODE)
    {
        if (school_mode_en)
            mode_flags.op_mode = SCHOOL_MODE;
        else if (corporate_mode_en) //v2.2.5
            mode_flags.op_mode = CORPORATE_MODE;
        else
            mode_flags.op_mode = IDLE_MODE;

        peripheral_flags.dev_en_dis = TRUE;
        system_error = ERROR_FREE;
        memset(error_stack, FALSE, sizeof (error_stack));
        //for(cntr_1=0; cntr_1<TOTAL_ERRORS; cntr_1++)
        //error_stack[cntr_1] = FALSE;

        eeprom_write(SER_ERROR_STACK);
        eeprom_write(SER_OP_MODE);
        eeprom_write(SER_DEVICE_EN_DIS);
        strcpy(error_state, "0000");
        return 1;
    }
    else
    {
        lcd_init();
        strcpy(lcd_data, "INSUFFICIENT");
        lcd_display(1);
        strcpy(lcd_data, "COMMAND CODE");
        lcd_display(2);
        timer2_sec_delay(error_display_time);
        return 0;
    }

}

void modem_state_check_runtime()
{
  //  general_time_stamp_logout = sec_tick;       
    //  testing
    strcpy(lcd_data, "PLS WAIT        ");  lcd_display(1);       
    strcpy(lcd_data, "PROCESSING......");  lcd_display(2);    
     //  testing    
    unsigned char loop_1=0;
    unsigned char *local_ptr_1=0;
    WriteUART2((unsigned int) 26); //clear any pending data to server
    timer2_ms_delay(1000); //delay
    network_state = INITIAL;

    serial_string_send("AT+CREG?\r\n");
    check_modem_reply("CREG:", 5, 2000); //Response "+CREG: 0,1"
    timer2_sec_delay(1);
//    strcpy(string_buffer, "CREG:");
    local_ptr_1 = strstr(Rxdata,"CREG:");

    if (local_ptr_1 != 0) //string matched
    {
        loop_1 = 0;
        while ((*local_ptr_1++ != ',') && (loop_1 <= 10))
        {
            loop_1++;
        }
        network_state = *local_ptr_1;
    }
    else
    {
       modem_reset_count++;
    } 
    if( (network_state != '1') && (network_state != '5') )
    {
        modem_reset_count++;
    }   
//    sprintf(lcd_data,"TP2 = %c-%d-%d",network_state,(sec_tick - general_time_stamp_logout),modem_reset_count);
//    sprintf(lcd_data,"TP2 = %c -- %d",network_state,modem_reset_count);
//    clear_lcd();
//    lcd_display(1); 
//    sprintf(lcd_data,"%d %d %d %d",timer_flags.modem_status_check_en_dis,timer_flags.modem_status_check,modem_status_check_hour,modem_status_check_min);
//    lcd_display(2); 
//    timer2_sec_delay(3); //delay
     //  testing
    
}



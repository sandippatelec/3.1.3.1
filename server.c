#define USE_AND_OR /* To enable AND_OR mask setting */

#include<timer.h>
#include<string.h>
#include<stdio.h>
#include<stdlib.h>

#include "main.h"
#include "serial_uart.h"
#include "variable.h"
#include "server.h"
#include "uart.h"
#include "mcu_timer.h"
#include "ext_lcd.h"
#include "modem.h"
#include "peripherial.h"
#include "communicate.h"
#include "DS1307.h"
//#include "DS2781.h"
#include "coin_uca2.h"  

void connect_to_server(void){
	unsigned char temp_cntr=0;	//v3.0.0

	WriteUART2((unsigned int)26);			//Clear pending data from modem if any
	serial_string_send("AT+CIPSHUT\r\n");	//Disconnect from server
	check_modem_reply("SHUT OK",7,2000);

	//lcd_init();
	network_state = INITIAL;
	while((network_state!=REGISTERED) && (network_state!=REG_ROAMING) && (temp_cntr<2)){	
		network_check();
		temp_cntr++;
	}
	temp_cntr=0;

	if(system_flags.server_connection <= SERVER_CONN_OK)
	{
		lcd_init();
		strcpy(lcd_data,"CONNECTING TO");
		lcd_display(1);
		strcpy(lcd_data,"SERVER");
		lcd_display(2);
	}
	else{}

	serial_string_send("\r\n");    // clear all previous command
	timer2_ms_delay(250);

	serial_string_send("AT\r\n"); 
	check_modem_reply("OK",2,500);

	serial_string_send("ATE0\r\n");
	check_modem_reply("OK",2,1000);
	
	serial_string_send("AT+CFUN=1\r\n");
	check_modem_reply("OK",2,1000);
		
	serial_string_send("AT+CIPSHUT\r\n");
	check_modem_reply("SHUT OK",7,2000);
	
	serial_string_send("AT+CGATT=1\r\n");
//	check_modem_reply("OK",2,2000);
	check_modem_reply("OK",2,5000);   //3.1.2.H 
	
	serial_string_send("AT+CGDCONT=1,\"IP\",\"");
	putsUART2((unsigned int *)(apn));
	serial_string_send("\"\r\n");
	check_modem_reply("OK",2,2000);

//----------------	
	serial_string_send("AT+CSTT=\"");
	putsUART2((unsigned int *)(apn));
	serial_string_send("\",\"\",\"\"\r\n");
	check_modem_reply("OK",2,2000);
	
	serial_string_send("AT+CIICR\r\n");		//DELAYED Response
	
	if(check_modem_reply("DEACT",5,5000))
	{
		lcd_init();
		strcpy(lcd_data,"APN ERROR");
		//strcpy(lcd_data_freeze[lcd_freeze_counter++],"APN ERROR ");lcd_line1_data_freeze
		strcpy(lcd_line1_data_freeze,lcd_data);
		lcd_display(1);
		strcpy(lcd_data,"CONTACT SARVAJAL");
		//strcpy(lcd_data_freeze[lcd_freeze_counter++],"CONTACT SARVAJAL");
		strcpy(lcd_line2_data_freeze,lcd_data);
		lcd_display(2);
		system_flags.server_connection = FALSE;
		error_flags.apn_error =TRUE;
		consumer_flags.freeze_display = TRUE;
		
		serial_string_send("AT+CIPSHUT\r\n");
		check_modem_reply("SHUT OK",7,2000);

		timer2_sec_delay(error_display_time);	//delay
	}
	else if(check_modem_reply("OK",2,5000))
	{
		error_flags.apn_error = FALSE;
		consumer_flags.freeze_display = FALSE;
	}
	else
	{
		error_flags.apn_error = FALSE;
	}
	
	if(error_flags.apn_error == FALSE)
	{
		system_flags.local_ip = 1;		//alert to store local IP
		serial_string_send("AT+CIFSR\r\n");
		check_modem_reply("xxx.xxx.xxx.xxx",15,4000);
		system_flags.local_ip = 0;		//disable alert to store local IP
			

		serial_string_send("AT+CIPSTART=\"TCP\",\""); 	
		putsUART2((unsigned int *)(server_ip));
		serial_string_send("\",\"");
		putsUART2((unsigned int *)(server_port));
		serial_string_send("\"\r\n");
		if(check_modem_reply("CONNECT OK",10,8000))	//("CONNECT OK",10,1000)
		{
			system_flags.server_connection = SERVER_CONN_OK;
			server_reconnect_count = 0; 
		}
		else
		{
			system_flags.server_connection = FALSE;
			server_reconnect_count++;
		}
	}
	else{}

	if(system_flags.server_connection==TRUE)
	{
		consumer_flags.freeze_display = FALSE;

		strcpy(lcd_data,"CONNECTION");
		lcd_display(1);
		strcpy(lcd_data,"SUCCESSFUL...");
		lcd_display(2);
		timer2_sec_delay(error_display_time) ;	

//-----v2.3.1		
		serial_string_send("\r\n");    // clear all previous command
		timer2_ms_delay(250);
	
		serial_string_send("AT\r\n"); 
		check_modem_reply("OK",2,500);
//-----v2.3.1

		//---if(network_failure_counter==0xAA)
		if(network_failure_counter>=2)	//v2.2.7
			send_server_response(DEV_NETWORK_LOST);
		else{}

		//---if(server_failure_counter==0xAA)
		if(server_failure_counter>=2)	//v2.2.7
			send_server_response(DEV_SERVER_LOST);
		else{}

		server_failure_counter = 0;
		network_failure_counter = 0;
		error_flags.modem_sw_reset = FALSE;
		check_system_stability(1);
	}
	else
	{
		if(error_flags.apn_error == FALSE)
		{
			connection_fail_check();

			serial_string_send("AT+CIPSEND\r\n");
			timer2_ms_delay(100);
			serial_string_send("Rx problem");
			while(BusyUART2());								//wait till the UART is busy	
			WriteUART2((unsigned int)26);
			strcpy(lcd_data,"SERVER FAILURE");
			lcd_display(1);
			strcpy(lcd_data,"PLS. WAIT");
			lcd_display(2);
			timer2_sec_delay(error_display_time);
			
			if(server_reconnect_count>=2)
			{
				if(consumer_flags.dispense_button==TRUE)
				{
					lcd_init();
					strcpy(lcd_data,server_ip);
					lcd_display(1);
					strcpy(lcd_data,server_port);
					lcd_display(2);
					timer2_sec_delay(error_display_time);
					consumer_flags.dispense_button = 0;
				}
				else{}
				data_validation(SER_SERVER_IP);
				data_validation(SER_SERVER_PORT);
				data_validation(SER_SERVER_APN);
			}
			else{}
		}
		else{}		

		error_flags.modem_response_error = FALSE;

		if(server_reconnect_count>=3)
		{
			server_reconnect_count = 0;
			modem_reset_count = modem_reset_tolerance;
			network_state = INITIAL;
		}
		else{}
	
		check_sms();
	}	
	//LED_updates();	//debug LED update
}

void connection_fail_check(void)
{
	char local_buffer[12] = {0};
	char* pointer_1;
	
    memset(local_buffer,'\0',sizeof(local_buffer)); 
	strcpy(local_buffer,"TCP CLOSED");
	pointer_1 = strstr(Rxdata,local_buffer);
	if(pointer_1!=0)	//string matched
	{
		lcd_init();
		//---strcpy(lcd_data,"SERVER FAILURE");
		strcpy(lcd_data,"SERVER CUT-OFF  ");	//v2.3.1
		lcd_display(1);
		strcpy(lcd_data,"CONTACT SARVAJAL");
		lcd_display(2);
		if(peripheral_flags.coin_en_dis){coinbox_disable();} else {}	//v3.1.2
		timer2_sec_delay(error_display_time);
	}
	else{}

	strcpy(local_buffer,"PDP DEACT");
	pointer_1 = strstr(Rxdata,local_buffer);
	if(pointer_1!=0)	//string matched
	{
		lcd_init();
		strcpy(lcd_data,"NETWORK FAILURE");
		lcd_display(1);
		strcpy(lcd_data,"CONTACT SARVAJAL");
		lcd_display(2);
		if(peripheral_flags.coin_en_dis){coinbox_disable();} else {}	//v2.2.9	coin box disable //3.1.2
		timer2_sec_delay(error_display_time);
	}
	else{}

}

unsigned char check_server_connection(void)
{
	char local_buffer[10] = {0};
	char* pointer_1;

	system_flags.server_connection = FALSE;		//v2.3.1

	serial_string_send("AT+CIPSTATUS\r\n");
	if(check_modem_reply("CONNECT OK",10,4000)){
		system_flags.server_connection = SERVER_CONN_OK;
/*		if((peripheral_flags.coin_en_dis)&&(system_error == ERROR_FREE)&&(coin_flags.coin_collector_error==0)){	//v2.2.9  //3.1.2 
	//		clear_UCA2_data();
			coinbox_enable();
			//coinbox_status_check();
	//		clear_UCA2_data();
		}
		else{}	//v2.2.9*/
	}
	else{
		if(peripheral_flags.coin_en_dis){coinbox_disable();} else {}	//v2.2.9	coin box disable //3.1.2
		system_flags.server_connection = FALSE;
		strcpy(local_buffer,"TCP CLOSED");
		pointer_1 = strstr(Rxdata,local_buffer);
		if(pointer_1!=0)	//string matched
		{
			lcd_init();
			//---strcpy(lcd_data,"SERVER FAILURE  ");
			strcpy(lcd_data,"SERVER CUT-OFF  ");	//v2.3.1
			//---strcpy(lcd_data_freeze[lcd_freeze_counter++],"SERVER FAILURE");
			strcpy(lcd_line1_data_freeze,lcd_data);
			lcd_display(1);
			strcpy(lcd_data,"CONTACT SARVAJAL");
			//---strcpy(lcd_data_freeze[lcd_freeze_counter++],"CONTACT SARVAJAL");
			strcpy(lcd_line2_data_freeze,lcd_data);
			lcd_display(2);
			consumer_flags.freeze_display = TRUE;

			if(server_failure_counter!=0xAA)
				server_failure_counter++;
			else{}

			//---if((server_failure_counter>=4) && (server_failure_counter<=20) && (min_tick>30))
			if((server_failure_counter>=4) && (server_failure_counter<=20))		//v2.2.7
			{
				//---if(min_tick>30)		//If no solar/AC since last 125 mins & if server failure detected, do not send SMS alert - it is server restart
				//---if(!(((min_tick-battery_verify_time_stamp)>=125)&&(peripheral_flags.solar_sense==FALSE)&&(peripheral_flags.ac_mains_sense==FALSE)))	//v2.2.7
				if(!(((min_tick-battery_verify_time_stamp)>=125)&&(peripheral_flags.solar_sense==FALSE)))		//v2.2.8
					send_server_response((DEV_SERVER_LOST+DEV_SENT_VIA_SMS));
				else{}
				server_failure_counter = 0xAA;
			}
			else{}

			system_flags.server_connection = SERVER_DOWN;
			timer2_sec_delay(error_display_time);
			return system_flags.server_connection;
		}	
		else{}

		strcpy(local_buffer,"PDP DEACT");
		pointer_1 = strstr(Rxdata,local_buffer);
		if(pointer_1!=0)	//string matched
		{
			lcd_init();
			strcpy(lcd_data,"NETWORK FAILURE ");
			//---strcpy(lcd_data_freeze[lcd_freeze_counter++],"NETWORK FAILURE");
			strcpy(lcd_line1_data_freeze,lcd_data);
			lcd_display(1);
			strcpy(lcd_data,"CONTACT SARVAJAL");
			//---strcpy(lcd_data_freeze[lcd_freeze_counter++],"CONTACT SARVAJAL");
			strcpy(lcd_line2_data_freeze,lcd_data);
			lcd_display(2);
			
			consumer_flags.freeze_display = TRUE;
			if(network_failure_counter!=0xAA)
				network_failure_counter++;
			else{}

			//---if((network_failure_counter>=4) && (network_failure_counter<=20) && (min_tick>30))
			if((network_failure_counter>=4) && (network_failure_counter<=20))	//v2.2.7
			{
				if(min_tick>30)		//v2.2.7
					send_server_response((DEV_NETWORK_LOST+DEV_SENT_VIA_SMS));
				else{}
				network_failure_counter = 0xAA;
			}
			else{}

			system_flags.server_connection = NETWORK_DOWN;
			timer2_sec_delay(error_display_time);
			return system_flags.server_connection;
		}
		else{}

		strcpy(local_buffer,"IP INITIAL");
		pointer_1 = strstr(Rxdata,local_buffer);
		if(pointer_1!=0)	//string matched
		{
			lcd_init();
			strcpy(lcd_data,"SERVER LOST");
			lcd_display(1);
			strcpy(lcd_data,"PLS. WAIT");
			lcd_display(2);
			system_flags.server_connection = NETWORK_DOWN;
			timer2_sec_delay(error_display_time);
			return system_flags.server_connection;
		}
		else{}
	}

//	system_flags.server_connection = SERVER_CONN_OK;	//v3.0.0
//	system_flags.server_response=SERVER_RESPONSE_OK;	//v3.0.0
	return system_flags.server_connection;
}

//---time_limit in multiple of 100msec
//unsigned char check_modem_reply(char *reponse_string,unsigned char length,unsigned int time_limit)
unsigned char check_modem_reply(char *reponse_string,unsigned char store,unsigned int time_limit)
{

	unsigned int reply_time_stamp = 0,buzzer_time_stamp=0;
	unsigned char loop_1=0,length=0;	//char_data=0,loop_2=0,
	char* local_pointer;
	char check_buffer[20]= {0};

	local_pointer = 0;
	//server_buffer_flush();
	for(loop_1=0; loop_1<sizeof(check_buffer); loop_1++)
		check_buffer[loop_1] = 0;

	length = strlen(reponse_string);
	strcpy(check_buffer,reponse_string);
	
	if(time_limit<999)
		time_limit = 1000;
	else{}

	time_limit /= 1000;
	reply_time_stamp = sec_tick;	//note time stamp marking the start of process
	buzzer_time_stamp = timer2_tick;

	while((sec_tick - reply_time_stamp) <= time_limit)		//exit the function after "time_limit"
	{
		
		if((rfid_to_server_msg==DEV_AUTHENTICATE) && ((timer2_tick-buzzer_time_stamp)<=1000))
			buzzer(AUT01);		
		else{}

		local_pointer = strstr(Rxdata,check_buffer);
		if(local_pointer!=0)
		{	
			timer2_ms_delay(500);		//delay

			return TRUE;
		}
		else{}
	}

	timer2_ms_delay(100);
	return FALSE;
}

void send_to_server(void)
{
	unsigned char server_response = 0,sent_count=0;
	unsigned int cntr_2=0;
	
	while((server_response==0) && (sent_count<SERVER_RESENT_COUNT))
	{
		if(sent_count!=0)
		{
			lcd_init();
			//---strcpy(lcd_data,"SERVER SLOW");	
			strcpy(lcd_data,"NETWORK SLOW");		//v2.3.1
			lcd_display(1);
		}
		else{}

//-----v2.3.1		
		serial_string_send("\r\n");    // clear all previous command
		timer2_ms_delay(250);
//-----v2.3.1

		serial_string_send("AT+CIPSEND\r\n");
		timer2_ms_delay(200);
		serial_string_send(transmitData);
		timer2_ms_delay(200);
		while(BusyUART2());								//wait till the UART is busy	
		WriteUART2((unsigned int)26);
		sent_count++;
		server_response = check_server_response();
	}
	
	if(server_response==FALSE)
	{
		lcd_init();
		//---strcpy(lcd_data,"SERVER FAILURE");	
		strcpy(lcd_data,"NETWORK FAILURE ");		//v2.3.1
		lcd_display(1);
		strcpy(lcd_data,"PLS TRY AGAIN   ");	
		lcd_display(2);
		timer2_sec_delay(error_display_time);
	}
	else{}

	for(cntr_2=0; cntr_2<sizeof(transmitData); cntr_2++)
		transmitData[cntr_2] = 0;	
	
	rfid_to_server_msg = 0;
	server_send_message_type = 0;
}

unsigned char check_server_response(void)
{
	unsigned char data=0,counter=0,result=FALSE,debug=0,separator=0;
	unsigned char temp_data=0;
	char temp_buffer[10] = {0};
	char* pointer;
	char **endptr;
	double temp_dou_data;

	for(counter=0; counter<sizeof(temp_buffer); counter++)
		temp_buffer[counter] = 0;
	
	counter = 0;

	//---if(check_modem_reply("@ACK,",5,8000))	//check for string

	if(check_modem_reply("@ACK,",5,15000))	//check for string	//v2.2.8
	{
		if(debug==1)
		{
			strcpy(lcd_data,"ACK OK");	
			lcd_display(1);
		}
		else{}

		if(!((mode_flags.op_mode==CORPORATE_MODE)&&(rfid_to_server_msg==DEV_BALANCE_DEDUCT)))	//v2.2.5
		{
			timer2_ms_delay(500);		//delay
			timer2_ms_delay(500);		//delay
			for(counter=0; counter<MAX_SERVER_STRING; counter++)		//capture full sms data in "sms_buffer" without any checking
				server_response_string[counter] = serial_byte_receive();
	
			pointer = strstr(server_response_string,txn_type);	//check the txn_tpye string, ptr to starting of that string
				
			if(pointer!=0)
			{
				system_flags.server_response = SERVER_ACK_OK;
				if(debug==1)
				{
					strcpy(lcd_data,"CMD RX");	
					lcd_display(1);
				}
				else{}
			}
			else
			{
				strcpy(lcd_data,"PLS. WAIT...");	
				lcd_display(1);
				strcpy(lcd_data,"CMD FAILED");	
				lcd_display(2);	
				timer2_ms_delay(2000);		//delay		
				return FALSE;
			}
		}else{}

	}
	else if(mode_flags.op_mode != FRANCHISEE_MODE)    //2.2.5 when franchisee mode server doesn't respond so
	{
		lcd_init();
		strcpy(lcd_data,"PLS. WAIT...");	
		lcd_display(1);
		strcpy(lcd_data,"PROCESSING__");	
		lcd_display(2);
		timer2_ms_delay(1000);		//delay	
	}
	else{}

	result = check_server_error();

	if((mode_flags.op_mode==CORPORATE_MODE)&&(rfid_to_server_msg==DEV_BALANCE_DEDUCT))	//v2.2.5
		return result;

	if(debug==1)
	{
		strcpy(lcd_data,"Error checked");	
		lcd_display(2);	
	}
	else{}

	counter = 0;
	pointer = 0;

 if((system_flags.server_response==SERVER_ACK_OK) && (server_send_message_type==TXN_MSG) && (result==TRUE)){
	switch(rfid_to_server_msg)
	{
		case DEV_AUTHENTICATE:				//Example string: " @RV01,1,3234.66,2,20#$% "
			strcpy(temp_buffer,"@RV02");
			pointer = strstr(server_response_string,temp_buffer);	//check the "RV01" string, ptr to starting of that string
			
			if(pointer!=0)
				separator = 0;
			else
				break;	//exit loop

			while(*pointer!='$')
			{
				data = *pointer++;	
				
				if((data==',') || (data=='#'))
				{
					temp_data = 0;
					switch(separator)
					{
						case 0:
							break;

						case 1: //Card Type
							//---temp_data = atoi(temp_buffer);
							//---temp_data = strtoul(temp_buffer,endptr,10);
							temp_data = (temp_buffer[0]-0x30);
							rfid_card_type = temp_data;
							if(debug==1)
							{
								strcpy(lcd_data,"Card Type");	
								lcd_display(1);
								strcpy(lcd_data,temp_buffer);	
								lcd_display(2);	
								timer2_ms_delay(1000);		//delay
							}
							else{}
							break;

						case 2:	//Balance
							strcpy(acc_credit_array,temp_buffer);
							if(debug==1)
							{
								strcpy(lcd_data,"Credits");	
								lcd_display(1);
								strcpy(lcd_data,temp_buffer);	
								lcd_display(2);	
								timer2_ms_delay(1000);		//delay
							}
							else{}
							break;
						
						case 3:	//Rupee / Litre
							//---temp_data = atoi(temp_buffer);
							//---temp_data = strtoul(temp_buffer,endptr,10);
							temp_data = (temp_buffer[0]-0x30);
							litre_transcation = temp_data;
							if(debug==1)
							{
								strcpy(lcd_data,"Re / Lit");	
								lcd_display(1);
								strcpy(lcd_data,temp_buffer);	
								lcd_display(2);	
								timer2_ms_delay(1000);		//delay
							}
							else{}
							break;

						case 4:	//Total dispense capacity
							//---acc_dispense_capacity = atof(temp_buffer);
							temp_dou_data = strtod(temp_buffer,endptr);
							acc_dispense_capacity = (float)temp_dou_data;
							if(debug==1)
							{
								strcpy(lcd_data,"Total Capacity");	
								lcd_display(1);
								strcpy(lcd_data,temp_buffer);	
								lcd_display(2);	
								timer2_ms_delay(1000);		//delay
							}
							else{}

							system_flags.server_response=SERVER_RESPONSE_OK;
							result = TRUE;
							break;

						default:
							break;
					}
				
					for(counter=0; counter<sizeof(temp_buffer); counter++)
						temp_buffer[counter] = 0;
					counter = 0;	
					separator++;
				}
				else{}
				
				if((data>='.') && (data<='9'))
					temp_buffer[counter++] = data;
				else{}

				if((*pointer=='$')||(*pointer=='%'))
					break;
				else{}
			}
			break;
		
		case DEV_BALANCE_DEDUCT://
			
			strcpy(temp_buffer,"@RB03");
			pointer = strstr(server_response_string,temp_buffer);	//check the "RV01" string, ptr to starting of that string
			
			if(pointer!=0)
				separator = 0;
			else
				break;	//exit loop

			while(*pointer!='$')
			{
				data = *pointer++;	
				
				if((data==',') || (data=='#'))
				{
					temp_data = 0;
					switch(separator)
					{
						case 0:
							break;

						case 1: //Card Type
							//---temp_data = atoi(temp_buffer);
							//---temp_data = strtoul(temp_buffer,endptr,10);
							temp_data = (temp_buffer[0]-0x30);
							rfid_card_type = temp_data;
							if(debug==1)
							{
								strcpy(lcd_data,"Card Type");	
								lcd_display(1);
								strcpy(lcd_data,temp_buffer);	
								lcd_display(2);	
								timer2_ms_delay(1000);		//delay
							}
							else{}
							break;

						case 2:	//Balance
							strcpy(acc_credit_array,temp_buffer);
							if(debug==1)
							{
								strcpy(lcd_data,"Credits");	
								lcd_display(1);
								strcpy(lcd_data,temp_buffer);	
								lcd_display(2);	
								timer2_ms_delay(1000);		//delay
							}
							else{}

							result = TRUE;
							break;

						default:
							break;
					}
				
					for(counter=0; counter<sizeof(temp_buffer); counter++)
						temp_buffer[counter] = 0;
					counter = 0;	
					separator++;
				}
				else{}
				
				if((data>='.') && (data<='9'))
					temp_buffer[counter++] = data;	
				else{}

				if((*pointer=='$')||(*pointer=='%'))
					break;
				else{}
			}
			break;		
		
		default:		
			break;
	}
 }	//end of if((system_flags.server_response==SERVER_ACK_OK) && (server_send_message_type==TXN_MSG))
 else	
	return result;

if(system_flags.server_response==SERVER_DATA_ERROR)
{
	lcd_init();
	strcpy(lcd_data,"SERVER RESPONSE");	
	lcd_display(1);
	strcpy(lcd_data,"ERROR");	
	lcd_display(2);
	timer2_ms_delay(1000);
}
else{}

return result;

}

unsigned char check_server_error(void)
{
	char error_buffer[5] = {0};
	char* er_pointer;
	unsigned char error = 0;

	strcpy(error_buffer,"@EV");
	er_pointer = strstr(server_response_string,error_buffer);	//check the "@EV" string, ptr to starting of that string
			
	if(er_pointer!=0)
	{
		buzzer(ERR01);
		rfid_card_type = 'E';
		//clear_lcd();
		error = *(er_pointer+3);
		lcd_init();
		switch(error)
		{
			case '1':	//Invalid Account
				strcpy(lcd_data,"INVALID ACCOUNT");	
				lcd_display(1);	
				strcpy(lcd_data,"SERVER ERROR:EV1");	
				break;

			case '2':	//Incorrect Card
				strcpy(lcd_data,"INCORRECT CARD");
				lcd_display(1);
				strcpy(lcd_data,"SERVER ERROR:EV2");	
				//---lcd_display(2);
				break;

			case '3':	//Insufficient Customer
				strcpy(lcd_data,"LOW BALANCE");	
				lcd_display(1);
				strcpy(lcd_data,"SERVER ERROR:EV3");
				break;

			case '4':	//MAX LIMIT REACHED		//V2.2.7
				strcpy(lcd_data,"DAILY MAX WATER");	
				lcd_display(1);
				strcpy(lcd_data,"LIMIT REACHED");	
				break;

			case '5':	//Unknown data
				strcpy(lcd_data,"UNKNOWN DATA");	
				lcd_display(1);
				strcpy(lcd_data,"SERVER ERROR:EV5");	
				break;

			case '6':	//Undefined String
				strcpy(lcd_data,"UNDEFINED STRING");	
				lcd_display(1);
				strcpy(lcd_data,"SERVER ERROR:EV6");	
				break;
			
			case '7':	//Checksum Error
				strcpy(lcd_data,"CHECKSUM ERROR");	
				lcd_display(1);
				strcpy(lcd_data,"SERVER ERROR:EV7");	
				//send_server_response(rfid_to_server_msg);
				break;

			case '8':	//Checksum Error
				strcpy(lcd_data,"CARD ON HOLD PLS");	
				lcd_display(1);
				strcpy(lcd_data,"SPEAK TO VENDOR");	
				break;

			case '9':	//Credit mismatched
				strcpy(lcd_data,"CREDIT SYS ERROR");	
				lcd_display(1);
				strcpy(lcd_data,"SERVER ERROR:EV9");	
				break;

			default:
				strcpy(lcd_data,"UNKNOWN RESPONSE");	
				lcd_display(1);
				strcpy(lcd_data,"SERVER ERROR:EVx");	
				break;
		}
		lcd_display(2);		//v2.2.7
		timer2_sec_delay(error_display_time);
		return error;
	}
	else
	{
		return TRUE;//no error
	}
}


void server_buffer_flush(void)
{
    unsigned char cnt = 0;
//    for (cnt = 0; cnt<sizeof (server_response_string); cnt++)  //v3.1.2.G
//    {
//        server_response_string[cnt] = 0;
//    }
    memset(server_response_string, 0, sizeof (server_response_string));  //v3.1.2.G
}

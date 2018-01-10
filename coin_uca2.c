/************************************************For coin box UCA2*************************************/
#define USE_AND_OR
#include<ports.h>
#include<timer.h>
#include<string.h>
#include<stdio.h>
#include<stdlib.h>

#include "math.h"
#include "p24fxxxx.h"
#include "main.h"
#include "coin_uca2.h"
#include "modem.h"
#include "variable.h"
#include "serial_uart.h"
#include "server.h"
#include "mcu_timer.h"
#include "communicate.h"
#include "ext_lcd.h"
#include "peripherial.h"
#include "ext_eeprom.h"
#include "serial_uart.h"

//#define COIN_TEST_EN
//#define COIN_TEST_EN_2

/**************************************************************************************************/
void clear_UCA2_data(void)                //To claer all receive data & appropriate flags.
{ 

//    if(consumer_flags.coin_read==FALSE){  
    	memset(coinbox_data,0,sizeof(coinbox_data));   
        memset(coin_send_data_arr, '\0', sizeof (coin_send_data_arr)); //3.1.2 //do_finalize
        coinbox_counter=0;	
        coin_flags.coin_receive_process=FALSE;	
        coin_flags.coin_checksum=FALSE;
        consumer_flags.coin_read=LOW;   //do_finalize
//    }

}
void coinbox_init(void ) //Initialize COIN BOX-2506
{
    unsigned char count;  
                                                                                #ifdef COIN_TEST_EN_2
                                                                                    strcpy(lcd_data, "COIN INIT START ");
                                                                                    lcd_display(1);
                                                                                    timer2_sec_delay(1);
                                                                                #endif
    if(coin_flags.coin_disable_by_fs_error==0)
    {
        coinbox_enable(); 

        read_eeprom_parameter(SER_COIN_CHN_EN_DIS);
        timer2_ms_delay(50);
        coinbox_channel_en_dis();
        bit_extract_factor = 0x80;
        for (count = 1; count < 9; count++)
        {
            if ((channel_cmd & bit_extract_factor) != 0)
            {
                eeprom_channel_read = count;
                read_eeprom_parameter(SER_COIN_ID);
                timer2_ms_delay(50);
            }
            else{}   
            bit_extract_factor = bit_extract_factor >> 1;
        }
//        coin_channels_status();
        coin_channels_status_new();
        //read_eeprom_parameter(SER_COIN_WATER_TARIFF);
        timer2_ms_delay(50);
        coinbox_status_check();
                                                                                    #ifdef COIN_TEST_EN_2
                                                                                        strcpy(lcd_data, "END COIN INIT   ");
                                                                                        lcd_display(1);
                                                                                        timer2_sec_delay(1); 
                                                                                    #endif
                                                                                    
    }                                                                                        
}

//v2.3.2

/*
void send_command_UCA2(unsigned char *command)
{
unsigned int count;
for(count=0;count<7;count++)
{
if(*command!=0x5B)
{
  while(BusyUART3());
  WriteUART3(*command);
 }
 *command++;
}
}
 */


void coinbox_disable(void) // To disable coin box-1010
{
                                                                              
    memset(coin_send_data_arr, '\0', sizeof (coin_send_data_arr));
    sprintf(coin_send_data_arr, "%c%c%c%c%c%c", 0x90, 0x05, 0x02, 0x03, 0x9A, 0x5b);  //3.1.2
    serial_string_send_coin(coin_send_data_arr);  
    coin_time_stamp = timer2_tick;
    while ((consumer_flags.coin_read == LOW)&&((timer2_tick - coin_time_stamp) < 500));  //V6
    coin_response_check(COIN_BOX_DISABLE);      	
    coinbox_status_check();

}

void coinbox_enable(void) // To enable coin box-1010
{    
    if(coin_flags.coin_disable_by_fs_error==0)
    {  
        memset(coin_send_data_arr, '\0', sizeof (coin_send_data_arr));
        sprintf(coin_send_data_arr, "%c%c%c%c%c%c", 0x90, 0x05, 0x01, 0x03, 0x99,0x5b); //3.1.2
        serial_string_send_coin(coin_send_data_arr);
        coin_time_stamp = timer2_tick;
         while ((consumer_flags.coin_read == LOW)&&((timer2_tick - coin_time_stamp) < 500));  //V6
        coin_response_check(COIN_BOX_ENABLE);   
        coinbox_status_check();
//        coin_channels_status(); //v2.3.4
    }

}

void coinbox_status_check(void) // To check status of coinbox		//505 ms
{
    memset(coin_send_data_arr, '\0', sizeof (coin_send_data_arr));
    sprintf(coin_send_data_arr, "%c%c%c%c%c%c", 0x90, 0x05, 0x11, 0x03, 0xA9, 0x5b);       //3.1.2
    serial_string_send_coin(coin_send_data_arr);
    coin_time_stamp = timer2_tick;
     while ((consumer_flags.coin_read == LOW)&&((timer2_tick - coin_time_stamp) < 500));  //V6
    coin_response_check(COIN_BOX_STATUS);
    //coin_data_receive_process(UCA_status_check);


}

void coinbox_channel_en_dis(void) // To enable/disable induividual channel of coin box
{
   unsigned char coin_checksum = 0;
    memset(coin_send_data_arr, '\0', sizeof (coin_send_data_arr));
    coin_checksum = 0x90 + 0x06 + 0x90 + channel_cmd + 0x03;
    sprintf(coin_send_data_arr, "%c%c%c%c%c%c%c", 0x90, 0x06, 0x90,channel_cmd, 0x03, coin_checksum,0x5b); //3.1.2
    //coin_command_send(Enable_channels);
    coin_checksum =0;
    serial_string_send_coin(coin_send_data_arr);
    coin_time_stamp = timer2_tick;
     while ((consumer_flags.coin_read == LOW)&&((timer2_tick - coin_time_stamp) < 500));  //V6
    coin_response_check(COIN_CHANEL_ENBL);
}

void coin_channel_en_dis_cmd(void)
{  
	switch(coin_channel_no){
		case 1:
				if(coin_channel_En_Dis){channel_cmd |= 0x80;}
				else{channel_cmd &= 0x7f;}
				break;
		case 2:
				if(coin_channel_En_Dis){channel_cmd |= 0x40;}
				else{channel_cmd &= 0xbf;}
				break;
		case 3:
				if(coin_channel_En_Dis){channel_cmd |= 0x20;}
				else{channel_cmd &= 0xdf;}
				break;
		case 4:
				if(coin_channel_En_Dis){channel_cmd |= 0x10;}
				else{channel_cmd &= 0xef;}
				break;
		case 5:
				if(coin_channel_En_Dis){channel_cmd |= 0x08;}
				else{channel_cmd &= 0xf7;}
				break;
		case 6:
				if(coin_channel_En_Dis){channel_cmd |= 0x04;}
				else{channel_cmd &= 0xfb;}
				break;
		case 7:
				if(coin_channel_En_Dis){channel_cmd |= 0x02;}
				else{channel_cmd &= 0xfd;}
				break;
		case 8:
				if(coin_channel_En_Dis){channel_cmd |= 0x01;}
				else{channel_cmd &= 0xfe;}
				break;
		default:
				break;
    }
        if (coin_flags.coin_debug)
        {

            lcd_init();
            strcpy(lcd_data, "CHANNEL NO:");
            lcd_display(1);
            if (coin_channel_En_Dis)
                sprintf(lcd_data, "%d,%s", coin_channel_no, " ENABLE");
            else
                sprintf(lcd_data, "%d,%s", coin_channel_no, " DISABLE");
            lcd_display(2);
        }
        if (coin_channel_En_Dis)
        {
            eeprom_channel_read = coin_channel_no;
            read_eeprom_parameter(SER_COIN_ID);
        }

            timer2_ms_delay(1000);
            coinbox_channel_en_dis();
            if (coin_flags.coin_channel_status == TRUE)
            {
                eeprom_write(SER_COIN_CHN_EN_DIS);
                timer2_ms_delay(50);
                coin_flags.coin_channel_status = FALSE;
            }
                else
                {
                    coin_channel_En_Dis = 0;
                    read_eeprom_parameter(SER_COIN_CHN_EN_DIS); //3.1.2 if channel enable cmd failed, then re update --> channel_cmd variable
                }
//            coin_channels_status(); // To update which COIN en/dis;
            coin_channels_status_new();
     
}

void coin_channels_status(void)
{
	
//    unsigned char temp_count,temp_count1=0,temp_channel_dis;
//        read_eeprom_parameter(SER_COIN_CHN_EN_DIS);
//		temp_channel_dis = channel_cmd;
//		bit_extract_factor = 0x80;
//		if(!temp_channel_dis){strcpy(coin_rs_status,"DISABLE ");}
//		else{
//			if(((temp_channel_dis&0x80)!=0)&&((temp_channel_dis&0x20)!=0)){ temp_channel_dis &= 0xDf;} else{}
//			if(((temp_channel_dis&0x08)!=0)&&((temp_channel_dis&0x04)!=0)){ temp_channel_dis &= 0xBB;} else{}
//			//if(((temp_channel_dis&0x40)!=0)&&((temp_channel_dis&0x08)!=0)&&((temp_channel_dis&0x04)!=0)){ temp_channel_dis &= 0xBB;} else{}
//			//if(((temp_channel_dis&0x40)!=0)&&((temp_channel_dis&0x08)!=0)){ temp_channel_dis &= 0xBB;} else{}
//			//if(((temp_channel_dis&0x40)!=0)&&((temp_channel_dis&0x04)!=0)){ temp_channel_dis &= 0xBB;} else{}
//			//if(((temp_channel_dis&0x08)!=0)&&((temp_channel_dis&0x04)!=0)){ temp_channel_dis &= 0xBB;} else{}
//			if(((temp_channel_dis&0x40)!=0)){ temp_channel_dis &= 0xBf;temp_channel_dis |= 0x08;} else{}
//			for(temp_count=1;temp_count<9;temp_count++){
//				if((temp_channel_dis&bit_extract_factor) != 0){
//					switch(temp_count){
//						case 1:
//							coin_rs_status[temp_count1] = '1';coin_rs_status[temp_count1+1] = ',';temp_count1 +=2; break;
//						case 2:
//							coin_rs_status[temp_count1] = '5';coin_rs_status[temp_count1+1] = ',';temp_count1 +=2; break;
//						case 3:
//							coin_rs_status[temp_count1] = '1';coin_rs_status[temp_count1+1] = ',';temp_count1 +=2; break;
//						case 4:
//							coin_rs_status[temp_count1] = '2';coin_rs_status[temp_count1+1] = ',';temp_count1 +=2; break;
//						case 5:
//							coin_rs_status[temp_count1] = '5';coin_rs_status[temp_count1+1] = ',';temp_count1 +=2; break;
//						case 6:
//							coin_rs_status[temp_count1] = '5';coin_rs_status[temp_count1+1] = ',';temp_count1 +=2; break;
//						case 7:
//							coin_rs_status[temp_count1] = '1';coin_rs_status[temp_count1+1] = '0';coin_rs_status[temp_count1+2] = ',';temp_count1 +=3; break;
//	//					case 8:
//	//						coin_rs_status[temp_count1] = '1';coin_rs_status[temp_count1+1] = ',';temp_count1 +=2; break;
//						default:
//							break;
//						}
//					}
//			else{}
//			bit_extract_factor = bit_extract_factor>>1;
//			}
//			coin_rs_status[temp_count1-1]=0;
//			temp_count1 = 0;
//		}

}
void coin_channels_status_new(void)
{

    	unsigned char temp_count,temp_count1=0,new_temp_channel_dis,coin_rs_status_new[23]={0};
        read_eeprom_parameter(SER_COIN_CHN_EN_DIS);
		new_temp_channel_dis = channel_cmd;		
        
        if(new_temp_channel_dis & 0x80)  //1a
        {
        coin_rs_status_new[temp_count1++] = '1'; coin_rs_status_new[temp_count1++] = 'a';coin_rs_status_new[temp_count1++] = ',';
        }else{}
        if(new_temp_channel_dis & 0x20)  //1b
        {
        coin_rs_status_new[temp_count1++] = '1'; coin_rs_status_new[temp_count1++] = 'b';coin_rs_status_new[temp_count1++] = ',';
        }else{}
         if(new_temp_channel_dis & 0x10)  //2a
        {
        coin_rs_status_new[temp_count1++] = '2'; coin_rs_status_new[temp_count1++] = 'a';coin_rs_status_new[temp_count1++] = ',';
        }else{}        
        if(new_temp_channel_dis & 0x40)  //5a
        {
        coin_rs_status_new[temp_count1++] = '5'; coin_rs_status_new[temp_count1++] = 'a';coin_rs_status_new[temp_count1++] = ',';
        }else{}       
        if(new_temp_channel_dis & 0x08)  //5b
        {
        coin_rs_status_new[temp_count1++] = '5'; coin_rs_status_new[temp_count1++] = 'b';coin_rs_status_new[temp_count1++] = ',';
        }else{}
        if(new_temp_channel_dis & 0x04)  //5c
        {
        coin_rs_status_new[temp_count1++] = '5'; coin_rs_status_new[temp_count1++] = 'c';coin_rs_status_new[temp_count1++] = ',';
        }else{}
        if(new_temp_channel_dis & 0x02)  //10a
        {
        coin_rs_status_new[temp_count1++] = '1'; coin_rs_status_new[temp_count1++] = '0';coin_rs_status_new[temp_count1++] = 'a';coin_rs_status_new[temp_count1++] = ',';
        }else{}    
        memcpy(lcd_data,&coin_rs_status_new[0],9);lcd_display(1);
        memcpy(lcd_data,&coin_rs_status_new[9],13);lcd_display(2);
        temp_count =temp_count1;

 }
void dispense_water_via_coin(void)				//Variable initialize after coin select
{

   unsigned char loop=0;
   for(loop=0; loop<sizeof(acc_credit_array); loop++)
		acc_credit_array[loop] = 0;

  	system_flags.server_response=SERVER_RESPONSE_OK; 		// for water dispense in consumer mode. noneed of 
									// serer response in COIN so force fully RESPONSE_OK
	rfid_card_type=CONSUMER_CARD;
	mode_flags.op_coin_mode = 1;							// Consumer mode execute by coin
	sprintf(acc_credit_array,"%.2u",coin_amount);			// To dispense water accordingl coin amount.
     litre_transcation=1;									// To disply amount of coin

	coin_amount_paisa = coin_amount*100;
    	acc_dispense_capacity =(float)(coin_amount_paisa/coin_water_tariff);
	//acc_dispense_capacity = 2.0;

// Dispensing not working as per Water terrif then must send SMS first for Water terrif it reads EEPROM which is 
// blank first so it'll never dispense acording water terrif value untill you first write in EEPROM
		//		sprintf(lcd_data,"%s,%d,%d,%f",acc_credit_array,coin_amount,coin_amount_paisa,coin_water_tariff);
		//		lcd_display(1);
		//		sprintf(lcd_data,"%2.1f",acc_dispense_capacity);
		//		lcd_display(2);
		//		timer2_sec_delay(10);

	mode_flags.op_mode = CONSUMER_MODE;
	mode_flags.forced_function_state = TRUE;
	function_state = ST_OP_MODE;
        consumer_flags.coin_read =HIGH; //3.1.2 
    //lcd_init();

}

void coin_id_eeprom_write(void){

	for(coin_id_channel_no=1;coin_id_channel_no<9;coin_id_channel_no++){
		switch(coin_id_channel_no){
			case 1:
				strcpy(temp_coin_id,"OCCOIN01");
				break;
			case 2:
				strcpy(temp_coin_id,"OCCOIN05");
				break;
			case 3:
				strcpy(temp_coin_id,"OCCOIN01");
				break;
			case 4:
				strcpy(temp_coin_id,"OCCOIN02");
				break;
			case 5:
				strcpy(temp_coin_id,"OCCOIN05");
				break;
			case 6:
				strcpy(temp_coin_id,"OCCOIN05");
				break;
			case 7:
				strcpy(temp_coin_id,"OCCOIN10");
				break;
			case 8:
				strcpy(temp_coin_id,"OCCOIN10");
				break;
			default:
				strcpy(temp_coin_id,"OCCOIN01");
				break;
	 	}   
	    eeprom_write(SER_COIN_ID);
	}

}

void send_server_coin_id(void){

	unsigned char cntr_1=0;
	read_eeprom_parameter(SER_COIN_CHN_EN_DIS);
	timer2_ms_delay(50);
	bit_extract_factor = 0x80;
	for(cntr_1=1;cntr_1<9;cntr_1++){
	if((channel_cmd & bit_extract_factor) != 0){
		eeprom_channel_read = cntr_1;
		read_eeprom_parameter(SER_COIN_ID);	
		timer2_ms_delay(50);
		coin_id_channel_no = eeprom_channel_read;
		//send_server_response(DEV_COIN_ID_STATUS);
		send_server_response(DEV_COIN_ID);
		}
	else{}
	bit_extract_factor = bit_extract_factor>>1;
	}

}
void coin_comm_check(void){

	UART3_init();
	timer2_sec_delay(1);	
	coinbox_init();			//v2.3.3
	coinbox_status_check();	//v2.3.3

}



/*					sprintf(lcd_data,"%u   %u",coin_flags.coinenable,coin_flags.coin_receive_process);	
					lcd_display(1);
					//sprintf(lcd_data,"TDS:%.4s ppm",tds_value);	
					sprintf(lcd_data,"%u  ",coin_flags.coinselect);	
					lcd_display(2);
					timer2_sec_delay(2);
*/
//enable_UCA_cmd[6]  = {0x90,0x05,0x01,0x03,0x99};
//disable_UCA_cmd[6] = {0x90,0x05,0x02,0x03,0x9A};
//UCA_status_check_cmd[6] = {0x90,0x05,0x11,0x03,0xA9};
//
//en_dis_UCA_channel_cmd[7] = {0x90,0x06,0x90,0x20,0x03,0x49};
//disable_channels_UCA_cmd[7] = {0x90,0x06,0x90,0x00,0x03,0x29};

//RS  1  0x01,0x03
//RS  2  0x04
//RS  5  0x02,0x05,0x06
//RS 10  0x07
void coin_response_check(unsigned char coin_cmd)
{
    unsigned char temp_count_1=0,temp_count_2=0,coin_process_complete=0; //coin_process_fail=0,temp_count_3=0;  //v3.1.1.C
//    unsigned char new_coin_data[20]={0};  //v3.1.1.C
//    char coin_lcd_data[17]={0};//v2.3.2   //v3.1.1.C
    //sprintf(lcd_data,"TP5 ");	lcd_display(1); 	timer2_sec_delay(error_display_time);
//    unsigned char Response_en_coin_Success[7]      = {0x90,0x05,0x50,0x03,0xE8};   //ACK
//    unsigned char Response_en_coin_Fail[7]         = {0x90,0x05,0x4B,0x03,0xE3};  //NACK
//    unsigned char Response_status_coin_idle[7]     = {0x90,0x05,0x11,0x03,0xA9};  //IDLE
//    unsigned char Response_status_coin_disable[7]  = {0x90,0x05,0x14,0x03,0xAC};  //DISABLE
//  //unsigned char Response_status_coin_Sensor_Problem[7]  = {0x90,0x06,0x16,0x01,0x03,0xB0};  //SENSOR PROB
//    unsigned char Response_status_coin_Fishing[7]  = {0x90,0x05,0x17,0x03,0xA9};  //FISHING
//    unsigned char Response_status_coin_Checksum_error[7]  = {0x90,0x05,0x18,0x03,0xA9};  //CHEKSUM ERROR
//    unsigned char Response_status_coin_ch_En_Success[7]  = {0x90,0x05,0x50,0x03,0xE8};  //CHEKSUM ERROR
//    unsigned char Response_status_coin_ch_En_Fail[7]  = {0x90,0x05,0x4B,0x03,0xE3};  //CHEKSUM ERROR

    timer2_ms_delay(200);
    coin_process_complete =0;    //2.3.H
    consumer_flags.coin_read=LOW; //v3.1.1.H  //V6
    temp_count_1 =0;             //2.3.H
    while(!coin_process_complete)
    {
            if(coinbox_data[temp_count_1]==0x90)
            {                   
               // timer2_ms_delay(500);
                coin_process_complete=1;
               // memcpy(new_coin_data,&coinbox_data[temp_count_1],7);
                temp_count_1++; 
                
                if(coinbox_data[temp_count_1]==0x05)
                {    
                   
                    temp_count_1++;  
                    switch(coinbox_data[temp_count_1])
                    {                       
                        case 0x50: 
                            
                            if(coin_cmd==COIN_BOX_ENABLE)
                            {
                                coin_flags.coin_receive_process=TRUE;     
                                coin_flags.coinenable=TRUE;

                                if(coin_flags.coin_debug){
                                    clear_lcd();
                                    strcpy(lcd_data,"COIN BOX ENABLE"); lcd_display(1);  //v2.3.2
                                    strcpy(lcd_data,"SUCCESS");         lcd_display(2);
                                }else{}
                            }
                            else if(coin_cmd==COIN_BOX_DISABLE)
                            {
                                coin_flags.coin_receive_process=TRUE;     
                                coin_flags.coinenable=FALSE;
                                if(coin_flags.coin_debug){ 
                                    clear_lcd();
                                    strcpy(lcd_data,"COIN BOX DISABLE"); lcd_display(1);  //v2.3.2
                                    strcpy(lcd_data,"SUCCESS");          lcd_display(2);
                                }else{}
                            }
                            else if(coin_cmd==COIN_CHANEL_ENBL)
                            {
                                coin_flags.coin_receive_process=TRUE;
                                coin_flags.coin_channel_status = TRUE;
                                if(coin_flags.coin_debug){  
                                    clear_lcd();
                                    strcpy(lcd_data,"CH EN/DIS CMD"); lcd_display(1);  //v2.3.2
                                    strcpy(lcd_data,"SUCCESS");       lcd_display(2);
                                }else{}
                            }else{}
                            break;

                        case 0x4B: 
                            
                            if(coin_cmd==COIN_BOX_ENABLE)
                            {
                                coin_flags.coin_receive_process=FALSE; 
                                if(coin_flags.coin_debug){  
                                    clear_lcd();
                                    strcpy(lcd_data,"COIN BOX ENABLE"); lcd_display(1);  //v2.3.2
                                    strcpy(lcd_data,"FAILED");          lcd_display(2);
                                }else{}
                            } 
                            else if(coin_cmd==COIN_BOX_DISABLE)
                            {
                                coin_flags.coin_receive_process=FALSE;    
                                if(coin_flags.coin_debug){ 
                                    clear_lcd();
                                    strcpy(lcd_data,"COIN BOX DISABLE"); lcd_display(1);  //v2.3.2
                                    strcpy(lcd_data,"FAILED");           lcd_display(2);
                                }else{}
                            }
                            else if(coin_cmd==COIN_CHANEL_ENBL)
                            {
                                coin_flags.coin_receive_process=FALSE; 
                                coin_flags.coin_channel_status =FALSE;
                                if(coin_flags.coin_debug){  
                                    clear_lcd();
                                    strcpy(lcd_data,"CH EN/DIS CMD"); lcd_display(1);  //v2.3.2
                                    strcpy(lcd_data,"FAILED");           lcd_display(2);
                                }else{}
                            }
                            else{}
                            break;
                            
                        case 0x11: 
                            
                            if(coin_cmd==COIN_BOX_STATUS)
                            {
                                coin_flags.coin_receive_process=TRUE;
                                if (coin_flags.coinenable==TRUE)
                                {
                                    coin_flags.coinidle=TRUE;
                                }else{}                     

                                if(coin_flags.coin_debug)
                                { 
                                    clear_lcd();
                                    strcpy(lcd_data,"COIN BOX STATUS"); lcd_display(1); 
                                    strcpy(lcd_data,"IDLE");       lcd_display(2);                         
                                }else{} //v2.3.2
                            }
                            break;
                        case 0x14: 
                            if(coin_cmd==COIN_BOX_STATUS)
                            {
                               coin_flags.coin_receive_process=FALSE;
                               if(coin_flags.coin_debug)
                               { 
                                   clear_lcd();
                                   strcpy(lcd_data,"COIN BOX STATUS"); lcd_display(1); 
                                   strcpy(lcd_data,"DISABLE");         lcd_display(2);                         
                               }else{} //v2.3.2
                            }
                            break;
                            
                        default :
                            coin_flags.coin_receive_process=FALSE;
                            if(coin_flags.coin_debug)
                            { 
                                clear_lcd();
                                strcpy(lcd_data,"COIN SYSTEM"); lcd_display(1); 
                                strcpy(lcd_data,"ERROR");       lcd_display(2);                         
                            }else{} //v2.3.2 
                            break;
                        } 
                          if(coin_flags.coin_debug){ timer2_ms_delay(500);}else{}
                }
                else if(coinbox_data[temp_count_1]==0x06)
                {
                    temp_count_1++;      
                   if(coinbox_data[temp_count_1]==0x12)
                   {
                         temp_count_1++;      
                        switch(coinbox_data[temp_count_1])
                        {
                            case 0x01:
                                coin_flags.coin_receive_process=TRUE;
                                coin_flags.coinselect=TRUE;
                                coin_amount = 0x01;
                                strcpy(accepted_coin_id,coin_id.channel_1);
                                //strcpy(rfid_card,"CWDCOIN00001");		//testing remove it
                                break;

                            case 0x02:
                                coin_flags.coin_receive_process=TRUE;
                                coin_flags.coinselect=TRUE;
                                coin_amount = 0x05;
                                strcpy(accepted_coin_id,coin_id.channel_2);
                                //strcpy(rfid_card,"CWDCOIN00001");		//testing remove it
                                break;

                            case 0x03:
                                coin_flags.coin_receive_process=TRUE;
                                coin_flags.coinselect=TRUE;
                                coin_amount = 0x01;
                                strcpy(accepted_coin_id,coin_id.channel_3); 
                                //strcpy(rfid_card,"CWDCOIN00001");		//testng remove it
                                break;

                            case 0x04:
                                coin_flags.coin_receive_process=TRUE;
                                coin_flags.coinselect=TRUE;
                                coin_amount = 0x02;
                                strcpy(accepted_coin_id,coin_id.channel_4);
                                //strcpy(rfid_card,"CWDCOIN00002");		//testng remove it
                                break;

                            case 0x05:
                                coin_flags.coin_receive_process=TRUE;
                                coin_flags.coinselect=TRUE;
                                coin_amount = 0x05;
                                strcpy(accepted_coin_id,coin_id.channel_5);
                                //strcpy(rfid_card,"CWDCOIN00005");		//testng remove it
                                break;

                            case 0x06:
                                coin_flags.coin_receive_process=TRUE;
                                coin_flags.coinselect=TRUE;
                                coin_amount = 0x05;
                                strcpy(accepted_coin_id,coin_id.channel_6);
                                //strcpy(rfid_card,"CWDCOIN00005");		//testng remove it
                                break;

                            case 0x07:
                                coin_flags.coin_receive_process=TRUE;
                                coin_flags.coinselect=TRUE;
                                coin_amount = 0x0A;
                                strcpy(accepted_coin_id,coin_id.channel_7);
                                //strcpy(rfid_card,"CWDCOIN00010");		//testng remove it
                                break;

                            default:
                                coin_flags.coin_receive_process=FALSE;
                                coin_flags.coinselect=FALSE;
                                coin_amount = 0;
                                break;
                        }
                        total_coin_amount += coin_amount; 
                        total_coin_count++;
                   }else{coin_flags.coin_receive_process=FALSE;}  //0x12  
                                   
                }
                else
                {
                  coin_flags.coin_receive_process=FALSE;  
                }            
                
            }else{}  //0x90  
            
        temp_count_1++;
        
        if(temp_count_1>=15)
        {  
             coin_process_complete=1;                            
              break;        
        }else{}
                            if(coin_flags.coin_receive_process==FALSE )
                            { 
                                if(coin_cmd==COIN_BOX_STATUS)coin_flags.coinidle=FALSE;else{};
                                if(coin_cmd==COIN_BOX_ENABLE)coin_flags.coinenable=FALSE; else{};
                                
                                //sprintf(lcd_data,"COIN CMD= %d",coin_cmd); lcd_display(1); 
                                //strcpy(lcd_data,"ERROR2");       lcd_display(2);                         
                            }else{} //v2.3.2 
    } //while close
    clear_UCA2_data(); //do_finalize
    timer2_sec_delay(1);
    temp_count_2 =0;
    LED3_OFF;

}
void serial_string_send_coin(unsigned char *string)
{  
        memset(coinbox_data,'\0',sizeof(coinbox_data));   	
        coinbox_counter=0;	
        coin_flags.coin_receive_process=FALSE;	
        coin_flags.coin_checksum=FALSE;
        consumer_flags.coin_read=LOW;

  
    while(*string != 0x5b)
    {
		while(BusyUART3());								//wait till the UART is busy	
        WriteUART3(*string);
        string++;
    } 

}

void Coin_Module_En_Dis(unsigned char En_Dis){

	if(En_Dis == 1){
		if((peripheral_flags.coin_en_dis)&&(system_error == ERROR_FREE)&&(coin_flags.coin_collector_error==0)&&(coin_flags.coin_disable_by_fs_error==0) ){//v2.2.9_coin&RFID coin enable, transtion by either coin or RFID		//add 4 coin box  //V6
			 coinbox_enable();
		}
		else{}
	}
	else if(En_Dis == 0){
		coinbox_disable();
	}
	else{}
}

void new_ch_check_en_dis(void)
{

//                    unsigned char chan_num =1;
//                    for(chan_num=1;chan_num<=7;chan_num++)
//                    {
//                          coin_channel_no = chan_num;           //(data_buffer[0] - 0x30);
//                          coin_channel_En_Dis = 1;					
//                              coin_channel_en_dis_cmd();
//          //					if(coin_flags.coin_channel_status == TRUE){
//          //						eeprom_write(SER_COIN_CHN_EN_DIS);
//          //						timer2_ms_delay(50);
//          //						coin_flags.coin_channel_status = FALSE;
//          //					}
//          //					else{
//          //						coin_channel_En_Dis = 0;
//          //					} 
//                               coin_channels_status_new();
//                                  send_server_response(DEV_COIN_CHN_EN_DIS);
//          }

}

void coin_check_on_error(void)
{
#ifdef COIN_TEST_EN

                    sprintf(lcd_data,"COIN COMM2, %d",coin_init_try);
					lcd_display(1);
					strcpy(lcd_data,"CHECK PLS. WAIT."); 
                    timer2_sec_delay(2);
#endif 
		
		gsm_flags.scan_sms = TRUE;
//		if((coin_init_try>0)&&(peripheral_flags.coin_en_dis)&&(peripheral_flags.dev_en_dis==TRUE)&&(system_error == ERROR_FREE)&&(coin_flags.coin_collector_error==0)){	//v2.3.3
		if((coin_init_try>0)&&(peripheral_flags.coin_en_dis)&&  (peripheral_flags.dev_en_dis==TRUE)&&(system_error == ERROR_FREE)&&(coin_flags.coin_collector_error==0)&& (coin_flags.coin_disable_by_fs_error==0)&&(mode_flags.op_mode == IDLE_MODE)){	//v2.3.4
			coinbox_status_check();
			if((coin_flags.coinidle==FALSE)||(coin_flags.coinenable==FALSE)){
				UART3_init();
				timer2_sec_delay(1);
				coinbox_init();			//v2.3.3
				coinbox_status_check();	//v2.3.3
				if((coin_flags.coinidle==TRUE)&&(coin_flags.coinenable==TRUE)){
					error_flags.coin_error_sent = FALSE;
					coin_init_try=0;	
				}
				else{
					coin_init_try++;	//v2.3.3
				}
				if(coin_init_try>3){	//v2.3.3
					lcd_init();
					strcpy(lcd_data,"COIN ERROR");
					lcd_display(1);
					strcpy(lcd_data,"PLS. WAIT...");
					//sprintf(lcd_data,"PLS. WAIT...%d",coin_init_try);
					lcd_display(2);
				}
				else{}
			}
			if((coin_init_try>=5)&&(error_flags.coin_error_sent==FALSE)){	//v2.3.3
				//coin_init_try=0;		//v2.3.5	//v2.3.5.B
				//if(min_tick>30){		//v2.3.5	//2.3.5.B
					send_server_response(DEV_COIN_COMMUNICATION_ERROR);
					send_server_response(DEV_HB_MSG);
					error_flags.coin_error_sent = TRUE;
					coin_init_try=0;	
					lcd_init();
					sprintf(lcd_data,"COIN FAILED..!! ");
					lcd_display(1);
					sprintf(lcd_data,"SYSTEM RESTART  ");
					lcd_display(2);
					buzzer(ERR02);
					timer2_sec_delay(error_display_time);
					abort();
				//}				
				//else{}		
			}
			else{}
		}
		else{}
}

 void coin_check_on_interval(void)
 {  
#ifdef COIN_TEST_EN

                    sprintf(lcd_data,"COIN COMM1, %d",coin_init_try);
					lcd_display(1);
					strcpy(lcd_data,"CHECK PLS. WAIT."); 
                    timer2_sec_delay(2);
#endif                    
    if((peripheral_flags.coin_en_dis)&&(peripheral_flags.dev_en_dis==TRUE)&&(system_error == ERROR_FREE)&&(coin_flags.coin_collector_error==0)&& (coin_flags.coin_disable_by_fs_error==0) &&(mode_flags.op_mode == IDLE_MODE))	//v2.3.4
    {
        coinbox_status_check();	 										//1194-1209:505/4516
        if((coin_flags.coinidle==FALSE)||(coin_flags.coinenable==FALSE)){
            coin_comm_check();												//2.3.5.B
            if((coin_flags.coinidle==TRUE)&&(coin_flags.coinenable==TRUE)){
                error_flags.coin_error_sent = FALSE;
                coin_init_try=0;	
            }
            else{
                coin_init_try++;
            }
        }
        else{}
    }
    else{}     
}

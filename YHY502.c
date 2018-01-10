#define USE_AND_OR

#include "YHY502.h"
#include "p24fxxxx.h"
#include "serial_uart.h"
#include "math.h"
#include "variable.h"
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include "mcu_timer.h" //v3.1.1.C
unsigned char rfid_Epurse_init(unsigned char key,unsigned char Block_num,unsigned long bal) //v3.1.1.D unsigned int bal
{
	rfid_element=0;rfid_reader_counter=0;
	memset(command_array,'0',sizeof(command_array)); 
	memset(scanned_rfid_card,'\0',sizeof(scanned_rfid_card)); 
	
	unsigned char Ex_or_var=0; 
	command_array[rfid_element++] = 0xAA;
	command_array[rfid_element++] = 0xBB;
	command_array[rfid_element++] = 0x0E;
	command_array[rfid_element++] = 0x23;
	command_array[rfid_element++] = key;
	command_array[rfid_element++] = Block_num;	
	command_array[rfid_element++] = 'S';
	command_array[rfid_element++] = 'R';
	command_array[rfid_element++] = 'V';
	command_array[rfid_element++] = 'J';
	command_array[rfid_element++] = 'A';
	command_array[rfid_element++] = 'L';
	command_array[rfid_element++] = (unsigned char)(bal&0xFF);
	if(command_array[rfid_element-1]==0xAA)	{ command_array[rfid_element++] = 0x00;}else{}     //v3.1.1.E
	command_array[rfid_element++] = (unsigned char)((bal>>8)&0xFF);
	if(command_array[rfid_element-1]==0xAA)	{ command_array[rfid_element++] = 0x00;}else{}     //v3.1.1.E
	command_array[rfid_element++] = (unsigned char)((bal>>16)&0xFF);
	if(command_array[rfid_element-1]==0xAA)	{ command_array[rfid_element++] = 0x00;}else{}     //v3.1.1.E
	command_array[rfid_element++] = (unsigned char)((bal>>24)&0xFF);
	if(command_array[rfid_element-1]==0xAA)	{ command_array[rfid_element++] = 0x00;}else{}    //v3.1.1.E

	Ex_or_var=command_array[2];
		for(Rfid_loop=3;Rfid_loop<rfid_element;Rfid_loop++)	{
		Ex_or_var ^= command_array[Rfid_loop]; 
		}
	command_array[rfid_element++] = Ex_or_var;

	for(Rfid_loop=0;Rfid_loop<rfid_element;Rfid_loop++)	
	serial_byte_send_rfid(command_array[Rfid_loop]);
	if(check_Rfid_reply(0x23,0xDC,1000,"Epurse_init_P","Epurse_init_F"))
	{
		//serial_string_send("\nBlock_data= ");
		//serial_string_send(scanned_rfid_card);
		return 1;		
	}else{}
    return 0; //v3.1.1.C
}
unsigned char rfid_Epurse_increment(unsigned char key,unsigned char Block_num,unsigned long bal_val)
{
	rfid_element=0;rfid_reader_counter=0;
	memset(command_array,'0',sizeof(command_array)); 
	memset(scanned_rfid_card,'\0',sizeof(scanned_rfid_card)); 
	
	unsigned char Ex_or_var=0; 
	command_array[rfid_element++] = 0xAA;
	command_array[rfid_element++] = 0xBB;
	command_array[rfid_element++] = 0x0E;
	command_array[rfid_element++] = 0x25;
	command_array[rfid_element++] = key;
	command_array[rfid_element++] = Block_num;
	command_array[rfid_element++] = 'S';
	command_array[rfid_element++] = 'R';
	command_array[rfid_element++] = 'V';
	command_array[rfid_element++] = 'J';
	command_array[rfid_element++] = 'A';
	command_array[rfid_element++] = 'L';
	command_array[rfid_element++] = (unsigned char)(bal_val&0xFF);
	if(command_array[rfid_element-1]==0xAA)	{ command_array[rfid_element++] = 0x00;}else{}  //v3.1.1.E
	command_array[rfid_element++] = (unsigned char)((bal_val>>8)&0xFF);
	if(command_array[rfid_element-1]==0xAA)	{ command_array[rfid_element++] = 0x00;}else{}  //v3.1.1.E
	command_array[rfid_element++] = (unsigned char)((bal_val>>16)&0xFF);
	if(command_array[rfid_element-1]==0xAA)	{ command_array[rfid_element++] = 0x00;}else{}  //v3.1.1.E
	command_array[rfid_element++] = (unsigned char)((bal_val>>24)&0xFF);
	if(command_array[rfid_element-1]==0xAA)	{ command_array[rfid_element++] = 0x00;}else{}  //v3.1.1.E

	Ex_or_var=command_array[2];
		for(Rfid_loop=3;Rfid_loop<rfid_element;Rfid_loop++)	{
		Ex_or_var ^= command_array[Rfid_loop]; 
		}
	command_array[rfid_element++] = Ex_or_var;

	for(Rfid_loop=0;Rfid_loop<rfid_element;Rfid_loop++)	
	serial_byte_send_rfid(command_array[Rfid_loop]);
	if(check_Rfid_reply(0x25,0xDA,1000,"Epurse_+++","E+++fail"))
	{
		return 1;
	}else{}
    return 0; //v3.1.1.C
}
unsigned char rfid_Epurse_decrement(unsigned char key,unsigned char Block_num,unsigned long bal_dec)
{
	rfid_element=0;rfid_reader_counter=0;
	memset(command_array,'0',sizeof(command_array)); 
	memset(scanned_rfid_card,'\0',sizeof(scanned_rfid_card)); 
	
	unsigned char Ex_or_var=0; 

	command_array[rfid_element++] = 0xAA;
	command_array[rfid_element++] = 0xBB;
	command_array[rfid_element++] = 0x0E;
	command_array[rfid_element++] = 0x26;
	command_array[rfid_element++] = key;
	command_array[rfid_element++] = Block_num;
	command_array[rfid_element++] = 'S';
	command_array[rfid_element++] = 'R';
	command_array[rfid_element++] = 'V';
	command_array[rfid_element++] = 'J';
	command_array[rfid_element++] = 'A';
	command_array[rfid_element++] = 'L';	
	command_array[rfid_element++] = (unsigned char)(bal_dec&0xFF);	
	if(command_array[rfid_element-1]==0xAA)	{ command_array[rfid_element++] =0x00;}else{}  //v3.1.1.E
	command_array[rfid_element++] = (unsigned char)((bal_dec>>8)&0xFF);
	if(command_array[rfid_element-1]==0xAA)	{ command_array[rfid_element++] =0x00;}else{}  //v3.1.1.E
	command_array[rfid_element++] = (unsigned char)((bal_dec>>16)&0xFF);
	if(command_array[rfid_element-1]==0xAA)	{ command_array[rfid_element++] =0x00;}else{}  //v3.1.1.E
	command_array[rfid_element++] = (unsigned char)((bal_dec>>24)&0xFF);
	if(command_array[rfid_element-1]==0xAA)	{ command_array[rfid_element++] =0x00;}else{}  //v3.1.1.E

	Ex_or_var=command_array[2];
		for(Rfid_loop=3;Rfid_loop<rfid_element;Rfid_loop++)	{
		Ex_or_var ^= command_array[Rfid_loop]; 
		}
	command_array[rfid_element++] = Ex_or_var;

	for(Rfid_loop=0;Rfid_loop<rfid_element;Rfid_loop++)	
	serial_byte_send_rfid(command_array[Rfid_loop]);
	if(check_Rfid_reply(0x26,0xD9,1000,"Epurse_---","E---fail"))
	{
		return 1;		
	}else{}
    return 0; //v3.1.1.C
}
//unsigned char rfid_read_purse(unsigned char key,unsigned char Block_num)
//{
//	rfid_element=0;rfid_reader_counter=0;
//	memset(command_array,'0',sizeof(command_array)); 
//	memset(scanned_rfid_card,'\0',sizeof(scanned_rfid_card)); 
//	unsigned char Ex_or_var=0;  //temp_loop_1=0 ,temp_rfid_card[20]={0}; signed curnt_neg_bal_paisa=0;
//	                            
//	//unsigned long int temp_user_bal_paisa_1=0,decimalNumber_1=0; ////
//    unsigned char temp_decimal_array[15]={0};    
//    memset(temp_decimal_array,'\0',sizeof(temp_decimal_array)); 
//    
//	command_array[rfid_element++] = 0xAA;              //command string send
//	command_array[rfid_element++] = 0xBB;
//	command_array[rfid_element++] = 0x0A;
//	command_array[rfid_element++] = 0x24;
//	command_array[rfid_element++] = key;            //key 
//	command_array[rfid_element++] = Block_num;     //particular block number read
//	command_array[rfid_element++] = 'S';          // our signature key
//	command_array[rfid_element++] = 'R';
//	command_array[rfid_element++] = 'V';
//	command_array[rfid_element++] = 'J';
//	command_array[rfid_element++] = 'A';
//	command_array[rfid_element++] = 'L';
//		
//		Ex_or_var=command_array[2];
//		for(Rfid_loop=3;Rfid_loop<rfid_element;Rfid_loop++)	{
//		Ex_or_var ^= command_array[Rfid_loop]; 
//		}	
//		command_array[rfid_element++] = Ex_or_var;    
//       
//		for(Rfid_loop=0;Rfid_loop<rfid_element;Rfid_loop++)	
//		serial_byte_send_rfid(command_array[Rfid_loop]);	
//		
//		if(check_Rfid_reply(0x24,0xDB,3000,"Read_Epurse","No_Epurse"))  //1000
//		{	
//			curnt_user_bal_paisa=0;			
//			if((scanned_rfid_card[7] & 0x01)!= 0x01)
//				{   
//                    temp_decimal_array[0] = (scanned_rfid_card[4]&0x0F); //
//                    temp_decimal_array[1] = ((scanned_rfid_card[4]&0xF0)>>4);
//                    temp_decimal_array[2] = (scanned_rfid_card[5]&0x0F);
//                    temp_decimal_array[3] = ((scanned_rfid_card[5]&0xF0)>>4);
//                    temp_decimal_array[4] = (scanned_rfid_card[6]&0x0F);
//                    temp_decimal_array[5] = ((scanned_rfid_card[6]&0xF0)>>4);
//                    curnt_user_bal_paisa =0;
//                    curnt_user_bal_paisa += temp_decimal_array[0]*pow(16, 0);
//                    curnt_user_bal_paisa += temp_decimal_array[1]*pow(16, 1);
//                    curnt_user_bal_paisa += temp_decimal_array[2]*pow(16, 2);
//                    curnt_user_bal_paisa += temp_decimal_array[3]*pow(16, 3);
//                    curnt_user_bal_paisa += temp_decimal_array[4]*pow(16, 4);
//                    curnt_user_bal_paisa += temp_decimal_array[5]*pow(16, 5);
//                  //  curnt_user_bal_paisa += temp_decimal_array[6]*pow(16, 6);
//                     return 1;					
//				}
//				else
//				{	
//						curnt_user_bal_paisa =0;										
//				  		return 1;	
//				}				
//		}
//		else{
//		    return 0;
//		}   
//}
unsigned char card_Serial_number()
{
	rfid_element=0;rfid_reader_counter=0;
	memset(command_array,'0',sizeof(command_array)); 
	memset(scanned_rfid_card,'\0',sizeof(scanned_rfid_card)); 
	command_array[rfid_element++] = 0xAA;
	command_array[rfid_element++] = 0xBB;
	command_array[rfid_element++] = 0x02;
	command_array[rfid_element++] = 0x20;
	command_array[rfid_element++] = 0x22;
	for(Rfid_loop=0;Rfid_loop<rfid_element;Rfid_loop++)	
	serial_byte_send_rfid(command_array[Rfid_loop]);
	if(check_Rfid_reply(0x20,0xDF,1000,"CARD DETECT","No_Card_Serial"))
	{
		hex_to_ascii(scanned_rfid_card,4,7);
		return 1;	
 	}
 	else{return 0;}   
}

void hex_to_ascii(unsigned char buffer1[],unsigned int start_pos, unsigned char end_pos) //0x48 convert into 0x04  0x08
{
	unsigned char temp_rfid=0,temp_rfid_loop=0;
	memset(rfid_card,'\0',sizeof(rfid_card));
	
	for(Rfid_loop=start_pos;Rfid_loop<=end_pos;Rfid_loop++){	
	temp_rfid = ((buffer1[Rfid_loop]&0xF0)>>4);
				if(temp_rfid<=0x09)
					temp_rfid += 0x30;
				else
					temp_rfid += 0x37;
				rfid_card[temp_rfid_loop++] = temp_rfid;
	temp_rfid = (buffer1[Rfid_loop]&0x0F);
				if(temp_rfid<=0x09)
					temp_rfid += 0x30;
				else
					temp_rfid += 0x37;
				rfid_card[temp_rfid_loop++] = temp_rfid;
				//rfid_card[temp_rfid_loop] = 0;
	}
	timer2_ms_delay(10);
//	sprintf(lcd_data,"%s",rfid_card);lcd_display(2);
		
}


unsigned char check_Rfid_reply(unsigned char reponse_data_1,unsigned char reponse_data_2,unsigned int time_limit,char *string1,char *string2)
{
	Rfid_time_stamp = timer2_tick;	//note time stamp marking the start of process
	
	//timer2_ms_delay(20);
	while((timer2_tick - Rfid_time_stamp) < time_limit)		//exit the function after "time_limit"				
	{
	    for(Rfid_loop=0;Rfid_loop<=20;Rfid_loop++)
	    {
		if(scanned_rfid_card[Rfid_loop]==reponse_data_1)
		{
//     		    clear_lcd();sprintf(lcd_data,"%s",string1);lcd_display(1);timer2_sec_delay(2);
                    cmd_response_count  = Rfid_loop ;
		timer2_ms_delay(50);
		return 1;
		}		
	    else if(scanned_rfid_card[Rfid_loop]==reponse_data_2)
	    {
//		    clear_lcd();sprintf(lcd_data,"%s",string2);lcd_display(1);timer2_sec_delay(2);
				return 0;
				}
			else{}
		}
	}
	//clear_lcd();sprintf(lcd_data,"%s",string2);lcd_display(1);
    return 0; //v3.1.1.C
}

//unsigned char  rfid_Read_Block(unsigned char key,unsigned char Block_num)
//{
//	rfid_element=0;rfid_reader_counter=0;
//	memset(command_array,'0',sizeof(command_array)); 
//	memset(scanned_rfid_card,'\0',sizeof(scanned_rfid_card)); 
//	
//	unsigned char Ex_or_var=0; 
//	command_array[rfid_element++] = 0xAA;
//	command_array[rfid_element++] = 0xBB;
//	command_array[rfid_element++] = 0x0A;
//	command_array[rfid_element++] = 0x21;
//	command_array[rfid_element++] = key;
//	command_array[rfid_element++] = Block_num;
//	command_array[rfid_element++] = 'S';
//	command_array[rfid_element++] = 'R';
//	command_array[rfid_element++] = 'V';
//	command_array[rfid_element++] = 'J';
//	command_array[rfid_element++] = 'A';
//	command_array[rfid_element++] = 'L';
//		Ex_or_var=command_array[2];
//			for(Rfid_loop=3;Rfid_loop<rfid_element;Rfid_loop++)	{
//			Ex_or_var ^= command_array[Rfid_loop]; 
//			}
//		command_array[rfid_element++] = Ex_or_var;
//	
//		for(Rfid_loop=0;Rfid_loop<rfid_element;Rfid_loop++)	
//		serial_byte_send_rfid(command_array[Rfid_loop]);
//		if(check_Rfid_reply(0x21,0xDE,1000,"Read_Block","No_Read_block"))
//		{	
//			return 1;
//		}
//		else{ return 0;}
//}

unsigned char  rfid_Read_Block_2(unsigned char key,unsigned char Block_num)
{
//rfid_element=0;rfid_reader_counter=0;
//memset(command_array,'0',sizeof(command_array)); 
//memset(scanned_rfid_card,'\0',sizeof(scanned_rfid_card)); 
//
//unsigned char Ex_or_var=0; 
//command_array[rfid_element++] = 0xAA;
//command_array[rfid_element++] = 0xBB;
//command_array[rfid_element++] = 0x0A;
//command_array[rfid_element++] = 0x21;
//command_array[rfid_element++] = key;
//command_array[rfid_element++] = Block_num;
//command_array[rfid_element++] = 0xFF;
//command_array[rfid_element++] = 0xFF;
//command_array[rfid_element++] = 0xFF;
//command_array[rfid_element++] = 0xFF;
//command_array[rfid_element++] = 0xFF;
//command_array[rfid_element++] = 0xFF;
//
//	Ex_or_var=command_array[2];
//		for(Rfid_loop=3;Rfid_loop<rfid_element;Rfid_loop++)	{
//		Ex_or_var ^= command_array[Rfid_loop]; 
//		}
//	command_array[rfid_element++] = Ex_or_var;
//
//	for(Rfid_loop=0;Rfid_loop<rfid_element;Rfid_loop++)	
//	serial_byte_send_rfid(command_array[Rfid_loop]);
//	if(check_Rfid_reply(0x21,0xDE,1000,"Read_Block_2","No_Read_block_2"))
//	{	
//		return 1;
//	}
//	else{ return 0;}
    return 0; //v3.1.1.C
}

//unsigned char rfid_Write_Block(unsigned char key,unsigned char Block_num,unsigned char data_bal_array[])
//{
//	rfid_element=0;rfid_reader_counter=0;
//
//	memset(command_array,'0',sizeof(command_array)); 
//	memset(scanned_rfid_card,'\0',sizeof(scanned_rfid_card)); 
//	
//	unsigned char Ex_or_var=0; 
//	command_array[rfid_element++] = 0xAA;
//	command_array[rfid_element++] = 0xBB;
//	command_array[rfid_element++] = 0x1A;
//	command_array[rfid_element++] = 0x22;
//	command_array[rfid_element++] = key;
//	command_array[rfid_element++] = Block_num;
//	command_array[rfid_element++] = 'S';
//	command_array[rfid_element++] = 'R';
//	command_array[rfid_element++] = 'V';
//	command_array[rfid_element++] = 'J';
//	command_array[rfid_element++] = 'A';
//	command_array[rfid_element++] = 'L';
//
//	//memcpy(command_array, &command_array[rfid_element], 16);
//		for(Rfid_loop=0;Rfid_loop<16;Rfid_loop++)	{
//		command_array[rfid_element++] =data_bal_array[Rfid_loop];
//		}	
//		Ex_or_var=command_array[2];
//			for(Rfid_loop=3;Rfid_loop<rfid_element;Rfid_loop++)	{
//			Ex_or_var ^= command_array[Rfid_loop]; 
//			}
//		command_array[rfid_element++] = Ex_or_var;
//	
//		for(Rfid_loop=0;Rfid_loop<rfid_element;Rfid_loop++)	
//		serial_byte_send_rfid(command_array[Rfid_loop]);
//		if(check_Rfid_reply(0x22,0xDD,1000,"get_write","No_write"))
//		{
//			return 1;		
//		}else{}
//}

//unsigned char rfid_Write_Block_2(unsigned char key,unsigned char Block_num,unsigned char data_bal_array[])
//{
//	rfid_element=0;rfid_reader_counter=0;
//
//	memset(command_array,'0',sizeof(command_array)); 
//	memset(scanned_rfid_card,'\0',sizeof(scanned_rfid_card)); 
//	
//	unsigned char Ex_or_var=0; 
//	command_array[rfid_element++] = 0xAA;
//	command_array[rfid_element++] = 0xBB;
//	command_array[rfid_element++] = 0x1A;
//	command_array[rfid_element++] = 0x22;
//	command_array[rfid_element++] = key;
//	command_array[rfid_element++] = Block_num;
//	command_array[rfid_element++] = 0xFF;
//	command_array[rfid_element++] = 0xFF;
//	command_array[rfid_element++] = 0xFF;
//	command_array[rfid_element++] = 0xFF;
//	command_array[rfid_element++] = 0xFF;
//	command_array[rfid_element++] = 0xFF;
//
//
//	memcpy(command_array, &command_array[rfid_element], 16);
//		for(Rfid_loop=0;Rfid_loop<16;Rfid_loop++)	{
//		command_array[rfid_element++] =data_bal_array[Rfid_loop];
//		}	
//		Ex_or_var=command_array[2];
//			for(Rfid_loop=3;Rfid_loop<rfid_element;Rfid_loop++)	{
//			Ex_or_var ^= command_array[Rfid_loop]; 
//			}
//		command_array[rfid_element++] = Ex_or_var;
//	
//		for(Rfid_loop=0;Rfid_loop<rfid_element;Rfid_loop++)	
//		serial_byte_send_rfid(command_array[Rfid_loop]);
//		if(check_Rfid_reply(0x22,0xDD,1000,"get_write_2","No_write_2"))
//		{
//			return 1;		
//		}else{}
//}

unsigned char rfid_change_key(unsigned char key,unsigned char sector_num,unsigned char old_key[],unsigned char new_key[])
{
	rfid_element=0;rfid_reader_counter=0;
	memset(command_array,'0',sizeof(command_array)); 
	memset(scanned_rfid_card,'\0',sizeof(scanned_rfid_card)); 
	
	unsigned char Ex_or_var=0; 
	command_array[rfid_element++] = 0xAA;
	command_array[rfid_element++] = 0xBB;
	command_array[rfid_element++] = 0x1A;
	command_array[rfid_element++] = 0x06;
	command_array[rfid_element++] = key; //KEYA - 0x00 , KEYB- 0x01
	command_array[rfid_element++] = sector_num; //sector
	
	command_array[rfid_element++] = old_key[0];   //  old key
	command_array[rfid_element++] = old_key[1];   //  old key
	command_array[rfid_element++] = old_key[2];   //  old key
	command_array[rfid_element++] = old_key[3];   //  old key
	command_array[rfid_element++] = old_key[4];   //  old key
	command_array[rfid_element++] = old_key[5];   //  old key
	
	command_array[rfid_element++] = new_key[0];   //  new key
	command_array[rfid_element++] = new_key[1];   //  new key
	command_array[rfid_element++] = new_key[2];   //  new key
	command_array[rfid_element++] = new_key[3];   //  new key
	command_array[rfid_element++] = new_key[4];   //  new key
	command_array[rfid_element++] = new_key[5];   //  new key
	
	command_array[rfid_element++] = 0xFF;  //access bit
	command_array[rfid_element++] = 0x07;
	command_array[rfid_element++] = 0x80;
	command_array[rfid_element++] = 0x69;   //10
	
	command_array[rfid_element++] = 0xFF;
	command_array[rfid_element++] = 0xFF;
	command_array[rfid_element++] = 0xFF;
	command_array[rfid_element++] = 0xFF;
	command_array[rfid_element++] = 0xFF;
	command_array[rfid_element++] = 0xFF;   //16

		Ex_or_var=command_array[2];
			for(Rfid_loop=3;Rfid_loop<rfid_element;Rfid_loop++)	{
			Ex_or_var ^= command_array[Rfid_loop]; 
			}
		command_array[rfid_element++] = Ex_or_var;
	
		for(Rfid_loop=0;Rfid_loop<rfid_element;Rfid_loop++)	
		serial_byte_send_rfid(command_array[Rfid_loop]);
		if(check_Rfid_reply(0x06,0xF9,1000,"Change key","No_key change"))
		{
			return 1;		
		}
		else{return 0;}
}


/*
sending
	curnt_user_bal_paisa = (unsigned long int) (curnt_user_bal_paisa - (unsigned int)(balance_deduct*coin_water_tariff));
	sprintf(curnt_user_bal_ary,"%.6ld",curnt_user_bal_paisa);
	curnt_user_bal_rs = (double)(curnt_user_bal_paisa/100.00);	//convert into rs to display
	sprintf(acc_credit_array,"%3.2f",(double)curnt_user_bal_rs);
	
receivng
	curnt_user_bal_paisa = atol(curnt_user_bal_ary);
	curnt_user_bal_rs = (double)(curnt_user_bal_paisa/100.00);	//convert into rs to display
		sprintf(acc_credit_array,"%3.2f",(double)curnt_user_bal_rs);
			sprintf(lcd_data,"BL:%.9s %.2s",acc_credit_array,transcation_mode);
		lcd_display(1);
*/

void serial_string_send_rfid(unsigned char *string)
{
//	serial_buffer_flush_rfid();
//	server_buffer_flush();
		
	while(*string)
		serial_byte_send_rfid(*string++);
}

void serial_byte_send_rfid(unsigned char data)
{
    while(BusyUART1());								//wait till the UART is busy	
	WriteUART1(data);
}

//
//void encryption(unsigned char array1[])
//{
//	unsigned char pos_1;
//	memset(user_bal_file,'\0',sizeof(user_bal_file));
//	random_no = ((rand()%10)+1)+131;
//	for(pos_1=0;pos_1<=5;pos_1++)  //total 6 byte
//	{	
//	array1[pos_1] = array1[pos_1]+random_no;
//	}
//	pos_1 =0;
//	sprintf(user_bal_file,"%.6s,%.3u,%c",array1,random_no,0xF9);
//	
//	calculate_check_sum_user_bal();
//
//	
//	//memcpy(array1 + 5, b, 5 * sizeof(int));		
//}
//unsigned char decryption(unsigned char read_user_bal[])
//{	
//	unsigned char pos_2=0,pos_3=0;
//	unsigned int sum_check =0,sum_check_2=0;
//	unsigned char receive_checksum[6]={0};
//	unsigned char data_problem_flag =0;
//   	char *ret;
//	memset(curnt_user_bal_ary,'\0',sizeof(curnt_user_bal_ary));
//	memset(random_no_array,'\0',sizeof(random_no_array));		
//	ret = strchr(read_user_bal,',');
//   
//  //  sprintf("String after |%c| is - |%s|\n", ch, ret);
//    
//    if(ret != '\0')
//    {
//		while(read_user_bal[pos_2] != ',')//user_bal_file
//		{
//		if(pos_2>=6){break;}
//		curnt_user_bal_ary[pos_2]= read_user_bal[pos_2];
//		sum_check += read_user_bal[pos_2];
//		pos_2++;		
//		}
//			sum_check += read_user_bal[pos_2];
//			pos_2++;
//			pos_3=0;
//				while(read_user_bal[pos_2] != ',')
//				{
//					if(pos_3>=6){data_problem_flag=1;break;}
//					random_no_array[pos_3++]= read_user_bal[pos_2];
//					sum_check += read_user_bal[pos_2];
//					pos_2++;
//				}
//					sum_check += read_user_bal[pos_2];
//					pos_2++;
//					pos_3=0;
//						while(read_user_bal[pos_2] != '$')
//						{
//							if(pos_3>=6){data_problem_flag=1;break;}
//							receive_checksum[pos_3++]= read_user_bal[pos_2];		
//							pos_2++;
//						}
//							sum_check_2 = atoi(receive_checksum);
//						if(sum_check_2 ==sum_check)
//						{
//					//		strcpy(lcd_data,"DATA MATCH      ");	lcd_display(1);		timer2_sec_delay(error_display_time);
//							data_problem_flag=0;
//						}
//						else
//						{
//					//		   strcpy(lcd_data,"DATA NOT MATCH  ");	lcd_display(1);		timer2_sec_delay(error_display_time);
//							data_problem_flag=1;
//						}
//			
//		if(!data_problem_flag)
//		{
//			random_no =atoi(random_no_array);
//			for(pos_2=0;pos_2<=5;pos_2++)  //total 6 byte
//			curnt_user_bal_ary[pos_2] = curnt_user_bal_ary[pos_2]-random_no;
//			return 1;
//		}
//		else{
//			rfid_card_type=UNAUTHORISED_CARD;
//			memset(curnt_user_bal_ary,'\0',sizeof(curnt_user_bal_ary));	
//			return 0;	
//		}	
//	 
//	}
//	else{
//		rfid_card_type=UNAUTHORISED_CARD;	
//		strcpy(lcd_data,"File Not write");	lcd_display(1);		timer2_sec_delay(error_display_time);
//		data_problem_flag =1;
//		memset(curnt_user_bal_ary,'\0',sizeof(curnt_user_bal_ary));
//		return 0;
//	}	
//	
//
//	
//}
//
//void calculate_check_sum_user_bal()
//{
//	unsigned int loc_counter_3=0,loc_counter_4=0;
//	unsigned char str_len =0;
//	unsigned char checksum_buf[6];
//	unsigned int final_sum = 0;
//	
//	str_len= strlen(user_bal_file);
//	
//	if(str_len<=0)
//		return;
//	else{}
//
//	for(loc_counter_3=0; loc_counter_3<sizeof(user_bal_file); loc_counter_3++)
//	{
//		if(user_bal_file[loc_counter_3]==0xF9)
//		{
//			user_bal_file[loc_counter_3]=0;
//			break;
//		}
//		else{}
//		final_sum += ((unsigned int)(user_bal_file[loc_counter_3]));
//	}
//	sprintf(checksum_buf,"%.3u",final_sum);
//	checksum_buf[4] = 0;
//
//	for(loc_counter_4=0; loc_counter_4<(sizeof(checksum_buf)); loc_counter_4++)
//	{
//		if(checksum_buf[loc_counter_4]==0)
//			break;
//		else{}
//
//		user_bal_file[loc_counter_3] = checksum_buf[loc_counter_4];
//		loc_counter_3++;
//	}
//		user_bal_file[loc_counter_3] = '$';	//# to $ //EOM
//}

//
//void rfid_read_write_block()
//{
//	unsigned char x_1=0,signature_array[17]={0};
//	
//	curnt_user_bal_paisa = 12000;
//	sprintf(curnt_user_bal_ary,"%.6ld",curnt_user_bal_paisa);																				
//	encryption(curnt_user_bal_ary);
//	if(rfid_Write_Block(0x00,0x01,user_bal_file))
//	{
//		strcpy(lcd_data,"WRTIE OK")	;lcd_display(1);timer2_sec_delay(2);
//		buzzer(AUT01);	
//		x_1=1;								
//	}	
//	else
//	{
//	strcpy(lcd_data,"NOT WRTIE")	;lcd_display(1);timer2_sec_delay(2);
//	x_1=2;
//	}	
//	if(rfid_Read_Block(0x00,0x01))
//	{
//		strcpy(lcd_data,"READ OK"); lcd_display(1);timer2_sec_delay(2);
//		x_1=3;
//			memset(rfid_fetch_data,'\0',sizeof(rfid_fetch_data)); 
//			memcpy(rfid_fetch_data,&scanned_rfid_card[4],16);
//		decryption(rfid_fetch_data);
//		curnt_user_bal_paisa = atol(curnt_user_bal_ary);
//		x_1=4;
//		sprintf(curnt_user_bal_ary,"%.6ld",curnt_user_bal_paisa);	
//		strcpy(lcd_data,curnt_user_bal_ary)	;lcd_display(1);timer2_sec_delay(2);
//	}
//	else{
//		strcpy(lcd_data,"PROBLEM READ"); lcd_display(1);timer2_sec_delay(2);	
//	}
//	if(card_Serial_number()==1){
//		
//		sprintf(signature_array,"SWAD,%.8s%$#",rfid_card);
//				if(rfid_Write_Block(0x00,0x02,signature_array))
//				{
//					strcpy(lcd_data,"WRTIE OK 2")	;lcd_display(1);timer2_sec_delay(2);
//					buzzer(AUT01);	
//					x_1=1;								
//				}	
//				else
//				{
//					strcpy(lcd_data,"NOT WRTIE")	;lcd_display(1);timer2_sec_delay(2);
//					x_1=2;
//				}
//				if(rfid_Read_Block(0x00,0x02))
//				{
//					strcpy(lcd_data,"READ OK 2"); lcd_display(1);timer2_sec_delay(2);
//				}
//					else{
//						strcpy(lcd_data,"PROBLEM READ"); lcd_display(1);timer2_sec_delay(2);	
//						}		
//	}	
//
//}

unsigned char rfid_read_keys(unsigned char sector)
{
	rfid_element=0;rfid_reader_counter=0;
	memset(command_array,'0',sizeof(command_array)); 
	memset(scanned_rfid_card,'\0',sizeof(scanned_rfid_card)); 
	
	unsigned char Ex_or_var=0; 

	command_array[rfid_element++] = 0xAA;
	command_array[rfid_element++] = 0xBB;
	command_array[rfid_element++] = 0x09;
	command_array[rfid_element++] = 0x05;
	command_array[rfid_element++] = sector;

	command_array[rfid_element++] = 0xFF;
	command_array[rfid_element++] = 0xFF;
	command_array[rfid_element++] = 0xFF;
	command_array[rfid_element++] = 0xFF;
	command_array[rfid_element++] = 0xFF;
	command_array[rfid_element++] = 0xFF;


	Ex_or_var=command_array[2];
		for(Rfid_loop=3;Rfid_loop<rfid_element;Rfid_loop++)	{
		Ex_or_var ^= command_array[Rfid_loop]; 
		}
	command_array[rfid_element++] = Ex_or_var;

	for(Rfid_loop=0;Rfid_loop<rfid_element;Rfid_loop++)	
	serial_byte_send_rfid(command_array[Rfid_loop]);
	if(check_Rfid_reply(0x05,0xFA,1000,"key_get","key_no"))
	{
		//serial_string_send("\nBlock_data= ");
		//serial_string_send(scanned_rfid_card);
		return 1;		
	}
    return 0; //v3.1.1.C
}
unsigned char rfid_read_keys_2(unsigned char sector)
{
	rfid_element=0;rfid_reader_counter=0;
	memset(command_array,'0',sizeof(command_array)); 
	memset(scanned_rfid_card,'\0',sizeof(scanned_rfid_card)); 
	
	unsigned char Ex_or_var=0; 

	command_array[rfid_element++] = 0xAA;
	command_array[rfid_element++] = 0xBB;
	command_array[rfid_element++] = 0x09;
	command_array[rfid_element++] = 0x05;
	command_array[rfid_element++] = sector;

	command_array[rfid_element++] = 0xFF;
	command_array[rfid_element++] = 0xFF;
	command_array[rfid_element++] = 0xFF;
	command_array[rfid_element++] = 0xFF;
	command_array[rfid_element++] = 0xFF;
	command_array[rfid_element++] = 0xFF;


	Ex_or_var=command_array[2];
		for(Rfid_loop=3;Rfid_loop<rfid_element;Rfid_loop++)	{
		Ex_or_var ^= command_array[Rfid_loop]; 
		}
	command_array[rfid_element++] = Ex_or_var;

	for(Rfid_loop=0;Rfid_loop<rfid_element;Rfid_loop++)	
	serial_byte_send_rfid(command_array[Rfid_loop]);
	if(check_Rfid_reply(0x05,0xFA,1000,"key_get","key_no"))
	{
		//serial_string_send("\nBlock_data= ");
		//serial_string_send(scanned_rfid_card);
		return 1;		
	}
    return 0; //v3.1.1.C
}

void read_module_type()
{
/*	rfid_element=0;rfid_reader_counter=0;
	memset(command_array,'0',sizeof(command_array)); 
	memset(scanned_rfid_card,'\0',sizeof(scanned_rfid_card)); 
	command_array[rfid_element++] = 0xAA;
	command_array[rfid_element++] = 0xBB;
	command_array[rfid_element++] = 0x02;
	command_array[rfid_element++] = 0x01;
	command_array[rfid_element++] = 0x03;
	for(Rfid_loop=0;Rfid_loop<rfid_element;Rfid_loop++)	
	serial_byte_send_rfid(command_array[Rfid_loop]);

	if(check_Rfid_reply(0x01,0xFE,1000,"GET_module","No_module"))
	{
		//serial_string_send("\ndata= ");
		//////serial_string_send(scanned_rfid_card);
		hex_to_ascii(scanned_rfid_card,4,7);		
	}*/
}

void module_serial_number()
{
/*	rfid_element=0;rfid_reader_counter=0;
	memset(command_array,'0',sizeof(command_array)); 
	memset(scanned_rfid_card,'\0',sizeof(scanned_rfid_card)); 
	command_array[rfid_element++] = 0xAA;
	command_array[rfid_element++] = 0xBB;
	command_array[rfid_element++] = 0x02;
	command_array[rfid_element++] = 0x02;
	command_array[rfid_element++] = 0x00;
	for(Rfid_loop=0;Rfid_loop<rfid_element;Rfid_loop++)	
	serial_byte_send_rfid(command_array[Rfid_loop]);
	if(check_Rfid_reply(0x02,0xFD,1000,"GET_Serial","No_Serial"))
	{
		//serial_string_send("\nSerial_Number= ");
		//serial_string_send(scanned_rfid_card);
		hex_to_ascii(scanned_rfid_card,4,7);			
	}
*/
}
void module_Card_seek(unsigned char seek)
{
/*	rfid_element=0;rfid_reader_counter=0;
	memset(command_array,'0',sizeof(command_array)); 
	memset(scanned_rfid_card,'\0',sizeof(scanned_rfid_card));
	
	unsigned char Ex_or_var=0; 
	command_array[rfid_element++] = 0xAA;
	command_array[rfid_element++] = 0xBB;
	command_array[rfid_element++] = 0x03;
	command_array[rfid_element++] = 0x13;
	command_array[rfid_element++] = seek;
	Ex_or_var=command_array[2];
		for(Rfid_loop=3;Rfid_loop<rfid_element;Rfid_loop++)	{
		Ex_or_var ^= command_array[Rfid_loop]; 
		}
	command_array[rfid_element++] = Ex_or_var;
	
	for(Rfid_loop=0;Rfid_loop<rfid_element;Rfid_loop++)	{
	serial_byte_send_rfid(command_array[Rfid_loop]);
	serial_byte_send(command_array[Rfid_loop]);
	}
	if(check_Rfid_reply(0x13,0xEC,1000,"GET_Seek","No_Seek"))
	{
		//serial_string_send("\nSeek_response= ");
		//serial_string_send(scanned_rfid_card);
		hex_to_ascii(scanned_rfid_card,4,7);		
	}
*/
}
void set_module_output_1(unsigned char output)
{
/*	rfid_element=0;rfid_reader_counter=0;
	memset(command_array,'0',sizeof(command_array)); 
	memset(scanned_rfid_card,'\0',sizeof(scanned_rfid_card));
	
	unsigned char Ex_or_var=0; 
	command_array[rfid_element++] = 0xAA;
	command_array[rfid_element++] = 0xBB;
	command_array[rfid_element++] = 0x03;
	command_array[rfid_element++] = 0x16;
	command_array[rfid_element++] = output;
	Ex_or_var=command_array[2];
		for(Rfid_loop=3;Rfid_loop<rfid_element;Rfid_loop++)	{
		Ex_or_var ^= command_array[Rfid_loop]; 
		}
	command_array[rfid_element++] = Ex_or_var;
	
	for(Rfid_loop=0;Rfid_loop<rfid_element;Rfid_loop++)	{
	serial_byte_send_rfid(command_array[Rfid_loop]);
	serial_byte_send(command_array[Rfid_loop]);
	}
	if(check_Rfid_reply(0x16,0xE9,1000,"set_op_1","fail_set_op_1"))
	{
		//serial_string_send("\nOUTPUTDATA= ");
		//serial_string_send(scanned_rfid_card);	
		hex_to_ascii(scanned_rfid_card,4,7);	
	}
*/
}

void card_type()
{
/*	rfid_element=0;rfid_reader_counter=0;
	memset(command_array,'0',sizeof(command_array)); 
	memset(scanned_rfid_card,'\0',sizeof(scanned_rfid_card)); 
	command_array[rfid_element++] = 0xAA;
	command_array[rfid_element++] = 0xBB;
	command_array[rfid_element++] = 0x02;
	command_array[rfid_element++] = 0x19;
	command_array[rfid_element++] = 0x1B;
	for(Rfid_loop=0;Rfid_loop<rfid_element;Rfid_loop++)	
	serial_byte_send_rfid(command_array[Rfid_loop]);
	if(check_Rfid_reply(0x19,0xE6,1000,"GET_Card_type","No_Card_type"))
	{
		//serial_string_send("\ncard_type_Number= ");
		//serial_string_send(scanned_rfid_card);
		hex_to_ascii(scanned_rfid_card,4,7);		
	}
*/
}

void hex_to_decimal()
{
 /*  long long decimalNumber=0;
   // unssigned char hexDigits[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8',
  //    '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    unsigned char hexDigits[16] = {0x00,0x01,0xA4,0xA0};
   
    int i, j, power=0;//, digit;     
    //memcpy(curnt_user_bal_ary,&scanned_rfid_card[4],4); 
     
    /// Converting hexadecimal number to decimal number 
    for(i=7; i >= 4; i--) {
       // /search currect character in hexDigits array 
        for(j=0; j<16; j++){
            if(scanned_rfid_card[i] == hexDigits[j]){
                decimalNumber += j*pow(16, power);
            }
        }
        power++;
    }  
  
    printf("Decimal Number : %ld", decimalNumber);  	
*/	
}
void rfid_power_down()
{
/*	rfid_element=0;rfid_reader_counter=0;
	memset(command_array,'0',sizeof(command_array)); 
	memset(scanned_rfid_card,'\0',sizeof(scanned_rfid_card)); 
	command_array[rfid_element++] = 0xAA;
	command_array[rfid_element++] = 0xBB;
	command_array[rfid_element++] = 0x02;
	command_array[rfid_element++] = 0x03;
	command_array[rfid_element++] = 0x01;
		for(Rfid_loop=0;Rfid_loop<rfid_element;Rfid_loop++)	
		serial_byte_send_rfid(command_array[Rfid_loop]);
		if(check_Rfid_reply(0x03,0xFC,1000,"Powr_down","No_power_down"))
		{}	
*/
}
void rfid_firmware_version()
{
/*	rfid_element=0;rfid_reader_counter=0;
	memset(command_array,'0',sizeof(command_array)); 
	memset(scanned_rfid_card,'\0',sizeof(scanned_rfid_card)); 
	command_array[rfid_element++] = 0xAA;
	command_array[rfid_element++] = 0xBB;
	command_array[rfid_element++] = 0x02;
	command_array[rfid_element++] = 0x10;
	command_array[rfid_element++] = 0x12;
		for(Rfid_loop=0;Rfid_loop<rfid_element;Rfid_loop++)	
		serial_byte_send_rfid(command_array[Rfid_loop]);
		if(check_Rfid_reply(0x10,0xEF,1000,"Firmwar_get","No_Firmwar_get"))
		{
			//serial_string_send("\nSerial_Number= ");
			//serial_string_send(scanned_rfid_card);		
		}
*/
}
unsigned char rfid_read_purse(unsigned char key,unsigned char Block_num)
{
    rfid_element = 0;
    rfid_reader_counter = 0;
    memset(command_array, '0', sizeof (command_array));
    memset(scanned_rfid_card, '\0', sizeof (scanned_rfid_card));
    unsigned char Ex_or_var = 0, temp_var_1 = 0, temp_var_2 = 0, bal_temp_buff[7] = {0}; //temp_loop_1=0 ,temp_rfid_card[20]={0}; signed curnt_neg_bal_paisa=0;
//bal_temp_buff[6 ]  //3.1.3.B
    unsigned char temp_decimal_array[15] = {0};
    memset(temp_decimal_array,'\0',sizeof(temp_decimal_array)); 
    cmd_response_count =0;
	command_array[rfid_element++] = 0xAA;              //command string send
	command_array[rfid_element++] = 0xBB;
	command_array[rfid_element++] = 0x0A;
	command_array[rfid_element++] = 0x24;
	command_array[rfid_element++] = key;            //key 
	command_array[rfid_element++] = Block_num;     //particular block number read
	command_array[rfid_element++] = 'S';          // our signature key
	command_array[rfid_element++] = 'R';
	command_array[rfid_element++] = 'V';
	command_array[rfid_element++] = 'J';
	command_array[rfid_element++] = 'A';
	command_array[rfid_element++] = 'L';
		
		Ex_or_var=command_array[2];
		for(Rfid_loop=3;Rfid_loop<rfid_element;Rfid_loop++)	
		{
		Ex_or_var ^= command_array[Rfid_loop]; 
		}	
		command_array[rfid_element++] = Ex_or_var;    
       
		for(Rfid_loop=0;Rfid_loop<rfid_element;Rfid_loop++)	
		{
		serial_byte_send_rfid(command_array[Rfid_loop]);	
		}
		if(check_Rfid_reply(0x24,0xDB,3000,"Read_Epurse","No_Epurse"))  //1000
		{	
		    curnt_user_bal_paisa=0;
		    peripheral_flags.head_key_get=0;
		    temp_var_1=0;temp_var_2=0;		
                    cmd_response_count++;
		    for(temp_var_1=cmd_response_count;temp_var_1<=(cmd_response_count+5);temp_var_1++)
		    {
			if(scanned_rfid_card[temp_var_1] == 0xAA)
			{
			    if(scanned_rfid_card[temp_var_1+1]==0x00)
			    {
				bal_temp_buff[temp_var_2++] =  scanned_rfid_card[temp_var_1];
				temp_var_1++;
				peripheral_flags.head_key_get=1;
			    }
			    else{}
			}
			else
			{
			    bal_temp_buff[temp_var_2++] =  scanned_rfid_card[temp_var_1];
			    if(peripheral_flags.head_key_get==1)
			    {
				peripheral_flags.head_key_get=0;
			    }
			    else{}
			}
		    }	
			if((bal_temp_buff[3] & 0x01)!= 0x01)
			{   
			    temp_decimal_array[0] = (bal_temp_buff[0]&0x0F); 
			    temp_decimal_array[1] = ((bal_temp_buff[0]&0xF0)>>4);
			    temp_decimal_array[2] = (bal_temp_buff[1]&0x0F);
			    temp_decimal_array[3] = ((bal_temp_buff[1]&0xF0)>>4);
			    temp_decimal_array[4] = (bal_temp_buff[2]&0x0F);
			    temp_decimal_array[5] = ((bal_temp_buff[2]&0xF0)>>4);
			    curnt_user_bal_paisa =0;
			    curnt_user_bal_paisa += temp_decimal_array[0]*pow(16, 0);   //16
			    curnt_user_bal_paisa += temp_decimal_array[1]*pow(16, 1);   //256
			    curnt_user_bal_paisa += temp_decimal_array[2]*pow(16, 2);   //4096
			    curnt_user_bal_paisa += temp_decimal_array[3]*pow(16, 3);   //65536
			    curnt_user_bal_paisa += temp_decimal_array[4]*pow(16, 4);   //1048576
			    curnt_user_bal_paisa += temp_decimal_array[5]*pow(16, 5);    //16777216    
			     return 1;					
			}
			else
			{	
					curnt_user_bal_paisa =0;										
					return 1;	
			}				
		}
		else{
		    return 0;
		}   
}


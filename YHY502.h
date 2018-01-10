#ifndef _YHY502_H_
#define _YHY502_H_

unsigned char check_Rfid_reply(unsigned char reponse_data_1,unsigned char reponse_data_2,unsigned int time_limit,char *string1,char *string2);
void read_module_type();
void module_serial_number();
void module_Card_seek(unsigned char seek);
void set_module_output_1(unsigned char output);
void card_type();
unsigned char card_Serial_number();
void hex_to_ascii(unsigned char buffer1[],unsigned int start_pos, unsigned char end_pos);
void rfid_power_down();
void rfid_firmware_version();
//unsigned char check_Rfid_reply(unsigned char reponse_data_1,unsigned char reponse_data_2,unsigned int time_limit,unsigned char *string1,unsigned char *string2);
//unsigned char rfid_Read_Block(unsigned char key,unsigned char Block_num);
//unsigned char  rfid_Read_Block_2(unsigned char key,unsigned char Block_num);
//unsigned char  rfid_Read_Block(unsigned char key,unsigned char Block_num,unsigned char read_key[]);
//unsigned char rfid_Write_Block(unsigned char key,unsigned char Block_num,unsigned char data_bal_array[]);
//unsigned char rfid_Write_Block_2(unsigned char key,unsigned char Block_num,unsigned char data_bal_array[]);
void serial_string_send_rfid(unsigned char *string);
void serial_byte_send_rfid(unsigned char data);
unsigned char rfid_read_purse(unsigned char key,unsigned char Block_num);
unsigned char rfid_Epurse_init(unsigned char key,unsigned char Block_num,unsigned long bal);  //v3.1.1.D unsigned int bal
unsigned char rfid_Epurse_increment(unsigned char key,unsigned char Block_num,unsigned long bal);
unsigned char rfid_Epurse_decrement(unsigned char key,unsigned char Block_num,unsigned long bal);
unsigned char rfid_change_key(unsigned char key,unsigned char sector_num,unsigned char old_key[],unsigned char new_key[]);
//void calculate_check_sum_user_bal();
//void encryption(unsigned char array1[]);
//unsigned char decryption(unsigned char read_user_bal[]);
/*void read_module_type();
void module_serial_number();
void rfid_power_down();
void rfid_firmware_version();*/
unsigned char rfid_read_keys(unsigned char sector);
unsigned char rfid_read_keys_2(unsigned char sector);
//void E_purse_funcheck();

#endif



    //    ----------   teting ----------------------
//unsigned char call_fun_1(unsigned char a);
//    unsigned char var_check=30;
//    var_check = call_fun_1(10);
//    if(call_fun_1(50)==FALSE)
//    {
//        strcpy(lcd_data,"Ok");		//v2.2.5
//        lcd_display(1);
//    }
//    else
//    {
//        strcpy(lcd_data," Not Ok");		//v2.2.5
//        lcd_display(1);
//    }
//        
//     var_check = call_fun_1(20);
//      //    ----------   teting ----------------------

//unsigned char call_fun_1(unsigned char a)
//{
//    
//    if(a==10)
//        return 1;
//    
//    if(a==20)
//        return 0;  
//    
//    return 0;
//}

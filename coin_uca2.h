//#define DIR_74HC245_TX {mPORTAOutputConfig(0x0040);mPORTASetBits(0x0040);}
//#define DIR_74HC245_RX {mPORTAOutputConfig(0x0080);mPORTAClearBits(0x0080);}

#define Enable_UCA2 			1 
#define Disable_UCA2 			2
#define Enable_channels 		3
#define Disable_channels 		4
#define UCA_status_check 		5	
#define Coin_Accept 			6

#define COIN_cmd_byte1			0x90
#define COIN_cmd_byte2			0x06
#define COIN_cmd_byte3			0x90
#define COIN_cmd_byte5			0x03

void coin_response_check(unsigned char coin_cmd);
void serial_string_send_coin(unsigned char *string);
void Coin_Module_En_Dis(unsigned char En_Dis);

void clear_UCA2_data(void);
void coin_id_eeprom_write(void);
void send_server_coin_id(void);
void new_ch_check_en_dis();
void coin_comm_check(void);
void coinbox_init(void);
void coinbox_disable(void);
void coinbox_enable(void);
void coinbox_status_check(void);
void coinbox_channel_en_dis(void);
void coin_channel_en_dis_cmd(void);
void coin_channels_status(void);
void coin_channels_status_new(void);
void dispense_water_via_coin(void);
void coin_check_on_interval();
void coin_check_on_error();



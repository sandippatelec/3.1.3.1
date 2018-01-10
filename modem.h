void check_sms(void);
void process_sms(void);
unsigned char read_sms(unsigned int no);
void network_check(void);
void modem_reset(void);
unsigned char check_network(unsigned char display);
unsigned char check_network_strength(unsigned char display);
void send_sms(void);
unsigned char scan_sms_memory (void);
void modem_powerdown(unsigned char status);
unsigned char get_modem_local_time(void);
unsigned char delete_msg_goto_normal_mode();
void configure_sms_mode();
void modem_state_check_runtime();

//---void send_sms_to_server(unsigned char sms_type);


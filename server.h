
void connect_to_server(void);
unsigned char check_modem_reply(char *reponse_string,unsigned char store,unsigned int time_limit);
void send_to_server(void);
unsigned char check_server_connection(void);
unsigned char check_server_response(void);
void server_buffer_flush (void);
unsigned char check_server_error(void);
void connection_fail_check(void);


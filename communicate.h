
//void send_server_response(unsigned char);

//void construct_response_data(unsigned char);

void send_server_response(unsigned int);	//v2.2.9

void construct_response_data(unsigned int);	//v2.2.9

void construct_response_message(void);

void calculate_check_sum(void);
void calculate_check_sum_SD_TEST(void);
void Server_Sync_2(unsigned char file_search,unsigned char data_send_to_server);
void Server_Sync(); //char *file_name

void send_to_server();




//#ifndef DS1307_H_INCLUDED
//#define DS1307_H_INCLUDED

#define USE_AND_OR

void DS1307_config(void);
void set_time_DS1307();
void DS1307_byte_write(unsigned char addr_1);
void start_time();
//void rtc_read1307(unsigned char *ee_bytes, unsigned char address_rtc, unsigned char no_of_bytes);
void rtc_read1307(unsigned char address_rtc, unsigned char no_of_bytes);
void get_local_time();



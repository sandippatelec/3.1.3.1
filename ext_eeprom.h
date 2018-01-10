/*----------------------------------------------------------------------------------
* 
* File        : ext_eeprom.h
* Author      : Vishal Prajapati
* Description : This file defines function prototypes for used in the configuration 
*               and operation of 24C512
*
*---------------------------------------------------------------------------------*/
#include "main.h"


void init_eeprom(void);
void init_i2c(void);
void test_i2c(void);
void i2c_wait(unsigned int cnt);
void eeprom_write(unsigned int data);
void eeprom_config(unsigned int address, unsigned char rw_bit);
void write_ext_eeprom_byte(unsigned char);
void write_eeprom_int(unsigned int);
//void write_eeprom_multiple_bytes(unsigned char *, unsigned int);
void write_eeprom_multiple_bytes(unsigned char *ee_bytes, unsigned int ee_address,unsigned char ee_length);
void read_eeprom_parameter(unsigned char);
void read_eeprom_multiple_bytes(unsigned char *, unsigned int, unsigned char);
void i2c_bus_recovery(void);
void erase_ext_eeprom(void);

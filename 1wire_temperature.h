#include "p24fxxxx.h"

#define READ_ROM_DS18B20    	0x33         // Command to read the 64-bit serial number from 1-wire slave device.
#define SKIP_ROM 				0xCC		// Skip ROM address command
#define MATCH_ROM				0x55		// Match ROM command
#define SEARCH_ROM				0xF0		// Search ROM command
#define ALRM_SEARCH_			0xEC		// Alarm Search command
#define READ_SCRATCHPAD			0xBE		// Read Scratchpad command
#define WRITE_SCRATCHPAD		0x4E		// Write Scratchpad command
#define COPY_SCRATCHPAD		    0x48		// Copy Scratchpad command
#define CONVERT_TEMPERATURE		0x44		// Temperature Address
#define RECALL_E2				0xB8		// Recall values from Non volatile memory
#define READ_POWER_SUPPLY		0xB4		// Signals the mode of power supply

void drive_OW_temp_low(void);
void drive_OW_temp_high(void);
unsigned char read_temp_OW(void);
void OW_temp_write_bit (unsigned char write_data);
unsigned char OW_temp_read_bit (void);
unsigned char OW_temp_reset_pulse(void);
void OW_temp_write_byte (unsigned char write_data);
unsigned char OW_temp_read_byte (void);
void init_temperature_sensor(void);
void ds_temp_id(void);
void read_temperature(void);
void CHILLER_HEATER_CONTROL(void);
void device_fault();



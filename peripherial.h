void scan_switch(void);
void buzzer(unsigned char buzzer_state);
//void LED_updates(void);	
void lls_sense_processing(void);
void hls_sense_processing(void);
void tds_check(void);
void display_low_battery_alert(void);
void display_tank_low_alert(void);
void tank_low_level_update(float litre);
void solar_sense_processing(void); 
void temperature_failure();

#define BUZZER_ON mPORTDSetBits(0x0002)			//{mPORTASetBits(0x0080);mPORTDSetBits(0x0800);}
#define BUZZER_OFF mPORTDClearBits(0x0002)		//{mPORTAClearBits(0x0080);mPORTDClearBits(0x0800);}

#define SW_LED_RED 0		//{mPORTEOutputConfig(0x0003);mPORTESetBits(0x0001);mPORTEClearBits(0x0002);}
#define SW_LED_BLUE 0		//{mPORTEOutputConfig(0x0003);mPORTESetBits(0x0002);mPORTEClearBits(0x0001);}
//----#define SW_LED_OFF {mPORTEInputConfig(0x0003);}
#define SW_LED_OFF  0 		//{mPORTEClearBits(0x0002);mPORTEClearBits(0x0001);mPORTEInputConfig(0x0003);}		//v2.2.4

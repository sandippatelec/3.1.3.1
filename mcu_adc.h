#define AVCC	3.3
#define ADC_MAX_COUNT	1024.0

#define AN_TDS_PIN	 		AD1PCFGbits.PCFG0			//temp input on AN0
#define AN_SV_CURRENT_PIN 	AD1PCFGbits.PCFG1			//temp input on AN1
//#define AN_TEMPERATURE_PIN 	0//AD1PCFGbits.PCFG5			//temp input on AN5
#define TDS_CHANNEL 0
//#define SV_CURRENT_CHANNEL 1
#define TEMPERATURE_CHANNEL 5

void adc_init(void);
unsigned long read_adc(void);
void update_tds(void);
void update_sv_current(void);
void update_temperature(void);
void update_analog_peripheral(unsigned char analog_parameter);
void analog_sensor_update(); //v3.1.1.C


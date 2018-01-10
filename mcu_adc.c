#define USE_AND_OR /* To enable AND_OR mask setting */
#include<adc.h>
#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "mcu_adc.h"
#include "variable.h"
#include "1wire_temperature.h"
#include "mcu_timer.h"

void adc_init(void)
{
	unsigned int config1,config2,config3,configportl,configporth=0,configscanl=0,configscanh=0;
	//---config1 = ADC_MODULE_OFF | ADC_CLK_AUTO | ADC_AUTO_SAMPLING_ON ;	
	config1 = 0x00E4;
	config2 = 0;///*ADC_SCAN_ON |*/ ADC_INTR_16_CONV ;
	//---config3 = ADC_SAMPLE_TIME_17 | ADC_CONV_CLK_254Tcy;
	config3 = ADC_SAMPLE_TIME_17 | ADC_CONV_CLK_64Tcy;
	//---configportl = 0xFFFC;
	configportl = 0xFFF8;	//0xFF00
	configscanl = ADC_SCAN_AN0;	
	OpenADC10(config1,config2,config3,configportl,configporth,configscanl,configscanh);
	EnableADC1;    												//turn on the ADC							
	SetChanADC10(ADC_CH0_POS_SAMPLEA_AN0);    					
}

void analog_sensor_update(){		//v1.0.0	//v2.3.4
	if((analog_sense==TDS_CHANNEL)&&(peripheral_flags.temp_en_dis==1))	//v2.2.9
	{
		analog_sense = TEMPERATURE_CHANNEL;
//		temp_reading_counter = 0;  //v3.1.1.F
	}
	else if(analog_sense==TEMPERATURE_CHANNEL)
	{
		analog_sense = TDS_CHANNEL;
		//adc_reading_counter = 0;   //3.1.0
	}
	else
		analog_sense = TDS_CHANNEL;
		
		update_analog_peripheral(analog_sense);		//v2.2.9
}

void update_analog_peripheral(unsigned char analog_parameter)
{
	unsigned long adc_result=0,difference=0,previous_result=0;

	switch(analog_parameter)
	{
		case TDS_CHANNEL:
			if(peripheral_flags.tds_en_dis)
			{
				//------Scan TDS
				SetChanADC10(ADC_CH0_POS_SAMPLEA_AN0); timer2_ms_delay(10);  //3.1.1
				adc_result = read_adc();
	
			// v2.2.9 
				if(adc_reading_counter>0)
					previous_result = tds_array[adc_reading_counter-1];
				else if((adc_reading_counter==0)&&(tds_array[AVG_READING_TDS-1]!=0)) //AVG_READING_TDS change to AVG_READING_TDS-1 //1.0.1
					previous_result = tds_array[AVG_READING_TDS-1]; //1.0.1
				else
					previous_result = adc_result;

				if(previous_result>adc_result){
					difference = previous_result - adc_result;
				}
				else{
					difference = adc_result - previous_result;
				}
				if(difference < 30) //3.1.0
					tds_array[adc_reading_counter++] = adc_result;
				else{
				tds_difference_exceed++;
			    }//3.1.0
                 if(tds_difference_exceed>=10)
                {
                    tds_difference_exceed=0;
                //    memset(tds_array,'\0',sizeof(tds_array));   //3.1.1.
                    previous_result = adc_result;
                //    adc_reading_counter=0;                    //3.1.1.
                    tds_array[adc_reading_counter++] = adc_result;
                }
			
				//tds_array[adc_reading_counter++] = adc_result;
			//	lcd_init();
			//	sprintf(lcd_data,"%.4ld",adc_result);
			//	lcd_display(1);
				update_tds();
			}
			else{ //1.0.1
				memset(tds_value,'\0',sizeof(tds_value));	
				tds_value[0]='O';tds_value[1]='F';tds_value[2]='F',tds_value[3]='\0';
			}		
			break;

	case TEMPERATURE_CHANNEL:

//                        timer2_sec_delay(1);  //must be required delay between two transition
			read_temperature();
			break; 

		default:
			break;
	}
}

unsigned long read_adc(void)
{
	unsigned long adc_data=0;
	
//----------Demo 2.0 Board
	while(!AD1CON1bits.DONE);          
    adc_data = (long) ADC1BUF0;
	
	return adc_data;	
}

void update_tds(void)
{
	unsigned char loop=0;
	float efective_calib=0;
	//if(adc_reading_counter>=(AVG_READING_TDS+1))
	if(peripheral_flags.tds_en_dis==1)
	{
		if(adc_reading_counter>=AVG_READING_TDS)
		{
			adc_reading_counter = 0;
		}
		else{}
	
		avg_tds_count = 0;
		//for(loop=0; loop<AVG_READING_TDS; loop++)
		for(loop=1; loop<AVG_READING_TDS; loop++)
		{
			 avg_tds_count += tds_array[loop];
		}	
		avg_tds_count = (avg_tds_count/(AVG_READING_TDS-1));
	
		if((tds_header_version=='C')||(tds_header_version=='c')){	//v2.2.9 If version B absolute, Modify Equation for calibration factor 0 to 20 rather then 8 to 20
				if( avg_tds_count < 45)				//Less then 50 TDS water
				{
					tds_reading = 50;
				}	
				else if((avg_tds_count > 45)&&(avg_tds_count < 58)){
					tds_reading = (int)((avg_tds_count-31)/0.26);
					/*if(tds_calib_fact>8)
						tds_reading += (tds_calib_fact-8);
					else{}*/
					//tds_reading = (int)(tds_reading*(float)(tds_calib_fact/100))
				}
				else if (avg_tds_count > 62){
					//tds_reading = (int)((avg_tds_count+15+tds_calib_fact)/0.847);
					tds_reading = (int)((avg_tds_count+23)/0.847);
				}
				else{
					tds_reading = 101;
				}
			efective_calib = (float)(tds_calib_fact/100.0);		//v2.2.9
			tds_reading = (int)(tds_reading*efective_calib);	// v2.2.9 Calibration Effect 
			//tds_reading = (int)((avg_tds_count+10+tds_calib_fact)/0.830);
		}
		else{
			adc_volt = (avg_tds_count*AVCC)/ADC_MAX_COUNT;
			tds_reading = (int)(adc_volt*10*tds_calib_fact);
		}


		if( (peripheral_flags.temp_en_dis==1)&&(peripheral_flags.temp_compen_en_dis==1)&&(DS_temperature>1.0)&&(DS_temperature<60.0)){		//v2.2.9 Temperature Compensation // temp limit 1 to 60 coz if sensor error then DS_temperature will be 0.	
			tds_at_25 = (float)(((int)(tds_reading*25))/DS_temperature);
			error_per_degree = (float)(((float)(tds_at_25*temperature_error_factor))/100.0);
			temperature_difference = (float)(DS_temperature-25);
			total_error_correction = (float)(temperature_difference*error_per_degree);
	
			tds_reading = (unsigned int)(tds_at_25 + total_error_correction);
		//	sprintf(tds_value1,"%.3d",tds_reading1);
		}
		else{}


		//tds_reading = (int)(adc_volt*10*8);
	
		/*adc_volt *= 1000.0;
		tds_reading = (int)(adc_volt/tds_calib_fact);
		adc_volt = (avg_tds_count*AVCC)/ADC_MAX_COUNT;*/
	
		if(peripheral_flags.tds_update==TRUE)
			sprintf(tds_value,"%.3d",tds_reading);
		else{}
	}
	else{
		memset(tds_value,'\0',sizeof(tds_value));	
		tds_value[0]='O';tds_value[1]='F';tds_value[2]='F',tds_value[3]='\0';
	}		
}


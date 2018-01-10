#define USE_AND_OR
#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<ports.h>

#include "Config.h"
#include "main.h"
#include "1wire_temperature.h"
#include "ext_lcd.h"
#include "variable.h"
#include "mcu_timer.h"
#include "mcu_adc.h"
#include "peripherial.h"
#include "communicate.h"
#include "coin_uca2.h"	//v2.2.9 
#include "ext_eeprom.h" //3.1.2.V13
#include "modem.h"   //3.1.2.V13


/**********************************************************************
* Function:        void drive_OW_low (void)
* PreCondition:    None
* Input:		   None	
* Output:		   None	
* Overview:		   Configure the OW_PIN as Output and drive the OW_PIN LOW.
* Time:			   0.000000375 S
***********************************************************************/
void drive_OW_temp_low (void)
{
	OW_TEMP_PIN_DIR = OUTPUT;
	OW_TEMP_WRITE_PIN=LOW;
}

/**********************************************************************
* Function:        void drive_OW_high (void)
* PreCondition:    None
* Input:		   None	
* Output:		   None	
* Overview:		   Configure the OW_PIN as Output and drive the OW_PIN HIGH.
* Time:			   0.000000375 S	
***********************************************************************/
void drive_OW_temp_high (void)
{
	OW_TEMP_PIN_DIR = OUTPUT;
	OW_TEMP_WRITE_PIN = HIGH;	
}

/**********************************************************************
* Function:        unsigned char read_OW (void)
* PreCondition:    None
* Input:		   None	
* Output:		   Return the status of OW pin.	
* Overview:		   Configure as Input pin and Read the status of OW_PIN 
* Time:			   0.000000875 S
***********************************************************************/
unsigned char read_temp_OW (void)
{
	unsigned char read_data=0;
	
	OW_TEMP_WRITE_PIN = INPUT;
	
	 if (OW_TEMP_READ_PIN==HIGH)
	 	read_data = SET;
	 else 	
		read_data = CLEAR;
		
	return read_data;
}

/**********************************************************************
* Function:        unsigned char OW_reset_pulse(void)
* PreCondition:    None
* Input:		   None	
* Output:		   Return the Presense Pulse from the slave.	
* Overview:		   Initialization sequence start with reset pulse.
*				   This code generates reset sequence as per the protocol
* Time:			   0.0010028 S
***********************************************************************/
unsigned char OW_temp_reset_pulse(void)
{
	unsigned char presence_detect=0;
	
  	drive_OW_temp_low(); 				// Drive the bus low
 	
 	asm( "REPEAT #3840\n\t" 
 	"NOP");	  						// delay 480 microsecond (us)
	
 	drive_OW_temp_high ();  				// Release the bus
	
	asm( "REPEAT #560\n\t"
	"NOP");							// delay 70 microsecond (us)
	
	presence_detect = read_temp_OW();	//Sample for presence pulse from slave

 	asm( "REPEAT #3280\n\t"
	"NOP");	  						// delay 410 microsecond (us)
	
	drive_OW_temp_high ();		    	// Release the bus
	
	return presence_detect;
}	

/**********************************************************************
* Function:        void OW_write_bit (unsigned char write_data)
* PreCondition:    None
* Input:		   Write a bit to 1-wire slave device.
* Output:		   None
* Overview:		   This function used to transmit a single bit to slave device.
*				   
***********************************************************************/

void OW_temp_write_bit (unsigned char write_bit)
{
	if (write_bit)
	{
		//writing a bit '1'
		drive_OW_temp_low(); 				// Drive the bus low
		asm( "REPEAT #48\n\t"
		"NOP");							// delay 6 microsecond (us)
		drive_OW_temp_high ();  				// Release the bus
		asm( "REPEAT #512\n\t"
		"NOP");							// delay 64 microsecond (us)
	}
	else
	{
		//writing a bit '0'
		drive_OW_temp_low(); 				// Drive the bus low
		asm( "REPEAT #480\n\t"
		"NOP");							// delay 60 microsecond (us)
		drive_OW_temp_high ();  				// Release the bus
		asm( "REPEAT #80\n\t"
		"NOP");							// delay 10 microsecond for recovery (us)
	}
}	


/**********************************************************************
* Function:        unsigned char OW_read_bit (void)
* PreCondition:    None
* Input:		   None
* Output:		   Return the status of the OW PIN
* Overview:		   This function used to read a single bit from the slave device.
*				   
***********************************************************************/

unsigned char OW_temp_read_bit (void)
{
	unsigned char read_data; 
	//reading a bit 
	drive_OW_temp_low(); 						// Drive the bus low
	asm( "REPEAT #48\n\t"
	"NOP");								// delay 6 microsecond (us)
	drive_OW_temp_high ();  						// Release the bus
	asm( "REPEAT #72\n\t"
	"NOP");								// delay 9 microsecond (us)

	read_data = read_temp_OW();					//Read the status of OW_PIN

	asm( "REPEAT #440\n\t"
	"NOP");								// delay 55 microsecond (us)	
	return read_data;
}

/**********************************************************************
* Function:        void OW_write_byte (unsigned char write_data)
* PreCondition:    None
* Input:		   Send byte to 1-wire slave device
* Output:		   None
* Overview:		   This function used to transmit a complete byte to slave device.
* Time:			   0.0006365 S
***********************************************************************/
void OW_temp_write_byte (unsigned char write_data)
{
	unsigned char loop;
	
	for (loop = 0; loop < 8; loop++)
	{
		OW_temp_write_bit(write_data & 0x01); 	//Sending LS-bit first
		write_data >>= 1;					// shift the data byte for the next bit to send
	}	
}	

/**********************************************************************
* Function:        unsigned char OW_read_byte (void)
* PreCondition:    None
* Input:		   None
* Output:		   Return the read byte from slave device
* Overview:		   This function used to read a complete byte from the slave device.
* Time:			   0.0006615 S
***********************************************************************/

unsigned char OW_temp_read_byte (void)
{
	unsigned char loop, result=0;
	
	for (loop = 0; loop < 8; loop++)
	{
		
		result >>= 1; 				// shift the result to get it ready for the next bit to receive
		if (OW_temp_read_bit())
			result |= 0x80;				// if result is one, then set MS-bit
		else{}
	}
	return result;					
}	

void init_temperature_sensor(void)
{
	OW_temp_reset_pulse();					// Issuing Reset Pulse
   	OW_temp_write_byte(SKIP_ROM);			// Issuing Skip NET Adress command (CCh)
	OW_temp_write_byte(WRITE_SCRATCHPAD);
	OW_temp_write_byte(0xFE);
	OW_temp_write_byte(0x01);
	OW_temp_write_byte(0x7F);
	OW_temp_reset_pulse();

	timer2_ms_delay(200);	//delay

	OW_temp_reset_pulse();
	OW_temp_write_byte(SKIP_ROM);			// Issuing Skip NET Adress command (CCh)
	OW_temp_write_byte(COPY_SCRATCHPAD);
	timer2_ms_delay(20);	//delay
	OW_temp_reset_pulse();

	OW_temp_reset_pulse();	
	OW_temp_write_byte(SKIP_ROM);			// Issuing Skip NET Adress command (CCh)
	OW_temp_write_byte(CONVERT_TEMPERATURE);		//Convert Temperature Command
	drive_OW_temp_high();
}

void ds_temp_id(void)
{
	unsigned char presence_pulse=0,cnt_1=0,cnt_2=0,temp=0;
	char data_buffer[17]={0};
	unsigned char temp_serial_no[8]={0};
	
	presence_pulse = OW_temp_reset_pulse();
	
	// Detecting Presence of the slave device
	lcd_init();
	if(presence_pulse)
	{
		strcpy(lcd_data,"DS TEMP ID-");
		lcd_display(1);
	}
	else
	{
		strcpy(lcd_data,"DS TEMP ID+");
		lcd_display(1);
	}
	
	// Reading Serial No.
	OW_temp_write_byte (READ_ROM_DS18B20);		// Issuing Read ROM command (33h)
	
	for(cnt_1=0;cnt_1<8;cnt_1++)
   	{
   		temp_serial_no[cnt_1] = OW_temp_read_byte();	// Read 64-bit registration (48-bit serial number) number from 1-wire Slave Device
	}

	cnt_2=0;
	for(cnt_1=0;cnt_1<8;cnt_1++)
   	{
   		temp = ((temp_serial_no[cnt_1] & 0xF0)>>4);
		if (temp <= 0x09)
			data_buffer[cnt_2++] = (temp+'0');
		else
			data_buffer[cnt_2++] = (temp+'0'+0x07);
	
		temp = temp_serial_no[cnt_1] & 0x0F;
		if (temp <= 0x09)
			data_buffer[cnt_2++] = (temp+'0');
		else
			data_buffer[cnt_2++] = (temp+'0'+0x07);
	} 
	
	strcpy(lcd_data,data_buffer);
	lcd_display(2);
	timer2_sec_delay(error_display_time);
}


void read_temperature(void)
{
    unsigned char cnt_1 = 0, cnt_2 = 0;
    if (peripheral_flags.temp_en_dis == 1)
    {
        for (cnt_2 = 0; cnt_2 < 3; cnt_2++)
        {
            //memset(DS_temperature_array, 0, sizeof (DS_temperature_array)); //v3.1.2.B
            OW_temp_reset_pulse(); // Issuing Reset Pulse
            OW_temp_write_byte(SKIP_ROM); // Issuing Skip NET Adress command (CCh)
            OW_temp_write_byte(READ_SCRATCHPAD); // Issuing Recall Scratch Pad command (BE)
            for (cnt_1 = 0; cnt_1 < 2; cnt_1++)
                DS_temperature_array[cnt_1] = OW_temp_read_byte(); // Read 64-bit registration (48-bit serial number) number from 1-wire Slave Device

            OW_temp_reset_pulse();
            DS_sensor_reading = 0;
            DS_sensor_reading = ((DS_temperature_array[1] << 8) & 0xFF00);
            DS_sensor_reading |= (DS_temperature_array[0] & 0x00FF);
            if ((DS_sensor_reading > 10)&&(DS_sensor_reading < 65534))
                cnt_2 = 3;
        }

        OW_temp_reset_pulse();
        OW_temp_write_byte(SKIP_ROM); // Issuing Skip NET Adress command (CCh)
        OW_temp_write_byte(CONVERT_TEMPERATURE); //Convert Temperature Command
        drive_OW_temp_high();

        /*sprintf(lcd_data,"Temp cnt=%u",DS_sensor_reading);
        lcd_display(1);
        DS_temperature = (((float)DS_sensor_reading) * 0.0625);
        sprintf(lcd_data,"Temp=%2.3f deg C",DS_temperature);
        lcd_display(2);*/

        //timer2_sec_delay(1);	// 1 sec delay
        //timer2_ms_delay(800);	//delay
        //drive_OW_temp_low();

        //------------v2.3.5----------//

//        if ((DS_sensor_reading > 80)&&(DS_sensor_reading < MAX_TEMPERATURE))   //3.1.1   5 to 78°C - 1600
        if ((DS_sensor_reading > 8)&&(DS_sensor_reading < MAX_TEMPERATURE))     //3.1.2.H  0.5 to 78°C - 1600
        { // - 5 to 78°C - 1600 //v3.1.1.F

            avg_temperature_array[temperature_reading_counter++] = DS_sensor_reading;
            temperature_failure_count = 0;

            if (temperature_reading_counter >= AVG_READING_TEMP)
            {
                temperature_reading_counter = 0;
            }
            else{}

            avg_temp_count = 0;
            if ((avg_temperature_array[0] == 'O') || (avg_temperature_array[1] == 'F') || (avg_temperature_array[2] == 'F'))
            {
                avg_temperature_array[1] = avg_temperature_array[0];
                avg_temperature_array[2] = avg_temperature_array[0];
            }
            else{}
            for (cnt_2 = 0; cnt_2 < AVG_READING_TEMP; cnt_2++)
            {
                avg_temp_count += avg_temperature_array[cnt_2];
            }
            avg_temp_count = (avg_temp_count / (AVG_READING_TEMP));
            //------------v2.3.5----------//


            //		if((DS_sensor_reading>10)&&(DS_sensor_reading<65534))
            //DS_temperature = (((float)DS_sensor_reading) * 0.0625);		//v2.3.5
            DS_temperature = (((float) avg_temp_count) * 0.0625);

            if (temp_calib_fact != 100)
                DS_temperature = ((temp_calib_fact * DS_temperature) / 100.0);

            sprintf(temp_value, "%2.2f", DS_temperature);
            temp_value[5] = 0;

            if (gsm_flags.temp_sensor_failure_sent == 1)
                gsm_flags.temp_sensor_failure_sent = 0;

            if (peripheral_flags.chiller_heater_en_dis == 1)
            { //v2.3.6
                //if(water_temperature_level >= ((int) DS_temperature))
                if (((int) DS_temperature) < water_temperature_level) //v3.1.2.B
                {
                    peripheral_flags.cold_water = 1; //chiller_off
//                    peripheral_flags.warm_water = 1; //v3.1.2.c
                }

                    //else if (water_temperature_level <= ((int) (DS_temperature - 4.0))) //v2.2.2  //v2.2.6 added hysterisis value	//		//v2.3.6
                else if (((int) DS_temperature) >= (water_temperature_level + 4)) //v2.2.2  //v2.2.6 added hysterisis value	//		//v3.1.2.B
                {
                    peripheral_flags.cold_water = 0;     //chiller_on
//                    peripheral_flags.warm_water = 1;     //v3.1.2.c
                }
                else{}
            }
            else if (peripheral_flags.chiller_heater_en_dis == 2)
            { //v2.3.6
                if (((int) DS_temperature) >= water_temperature_level)
                {
                    peripheral_flags.warm_water = 1;    //heater_off
//                    peripheral_flags.cold_water = 1;     //v3.1.2.c
                }
                else if (((int) DS_temperature) <= (int) (water_temperature_level - 4.0)) //v2.2.2  //v2.2.6 added hysterisis value	//		//v2.3.6
                {
                    peripheral_flags.warm_water = 0;    //heater_on
//                    peripheral_flags.cold_water = 1;     //v3.1.2.c
                }
                else{}
            }
            else
            {
            }
        }
        else
        {
            if (temperature_failure_count <= 5) //v2.3.5.B to stop Overflow
                temperature_failure_count++; //v2.3.5
            else{}
//            if (((temperature_failure_count > 5))&&(gsm_flags.temp_sensor_failure_sent == FALSE)) //3.1.2.A
            if (((temperature_failure_count > 5))&&(gsm_flags.temp_sensor_failure_sent==FALSE)&&(sec_tick>120))  //3.1.2.H
            { //v2.3.5
                peripheral_flags.cold_water = 0;
                peripheral_flags.warm_water = 0; //3.1.2.d
                DS_temperature = 0; //v2.2.7
                temperature_failure(); //v2.3.5
            }
            else{}
        }
    }
    else
    {
        DS_temperature = 0;
        for (cnt_1 = 0; cnt_1<sizeof (temp_value); cnt_1++)
            temp_value[cnt_1] = 0;
        temp_value[0] = 'O';
        temp_value[1] = 'F';
        temp_value[2] = 'F';

    }
}
/*---------------------------------------------------------------------------------------------------
Function Name:	void CHILLER_HEATER_CONTROL(void)
Arguments:		None
Return Val:		None
Description:	This Function controll starting and stop of the Chiller sensing the cold temprature level
				with hysterisis values. It also has chiller timeout condition using min_tick of timer
                to safely turned chiller off. Sleep time: 30 min & chiller timeout:120 min.           
---------------------------------------------------------------------------------------------------*/

void CHILLER_HEATER_CONTROL(void)    //v2.2.6	//v2.3.6
{
	if((peripheral_flags.chiller_heater_en_dis==1) && (DS_temperature>0.0) && (mode_flags.op_mode != ERROR_MODE) && (mode_flags.op_mode != DUMP_MODE) && (mode_flags.op_mode != REFILL_MODE) && (mode_flags.op_mode != DIAGNOSTIC_MODE) && (mode_flags.op_mode != DEBUG_MODE))   //v2.2.7 //v2.3.6
	{
		if(((peripheral_flags.cold_water == 1) || (peripheral_flags.chiller_heater_lls_trigger==TRUE)) && (peripheral_flags.chiller_status_flag==TRUE))     //FOR TESTING PURPOSE //v2.3.6
		{
			CHILLER_HEATER_OFF;

			lcd_init();
			strcpy(lcd_data,"CHILLER OFF");
			lcd_display(1);
			sprintf(lcd_data,"TEMP.:%.5s",temp_value);
			lcd_display(2);         
			buzzer(AUT01);
			timer2_sec_delay(error_display_time);
			
			peripheral_flags.chiller_status_flag=FALSE;
                        peripheral_flags.heater_status_flag=FALSE;  //v3.1.2.c  
			chiller_heater_time_stamp = 0;                       //clearing timeout count	//v2.3.6
		}
                else if((peripheral_flags.cold_water == 0) && (peripheral_flags.chiller_heater_lls_trigger==FALSE) && (peripheral_flags.chiller_status_flag==FALSE) && (peripheral_flags.chiller_heater_start_flag==TRUE))  //v2.3.6
		{
			CHILLER_HEATER_ON;

			lcd_init();
			strcpy(lcd_data,"CHILLER ON");
			lcd_display(1);
			sprintf(lcd_data,"TEMP.:%.5s",temp_value);
			lcd_display(2);
			buzzer(AUT01);
			timer2_sec_delay(error_display_time);
		
			peripheral_flags.chiller_status_flag=TRUE;
                        peripheral_flags.heater_status_flag=FALSE;  //v3.1.2.c  
//			timer1_init();
			chiller_heater_time_stamp = min_tick;	      // starting min count for timeout period	//v2.3.6
			error_flags.chiller_heater_relay_reset = FALSE;	//v3.1.2.I
		}
		else{}
                if((((int)DS_temperature) <= (unsigned char)(water_temperature_level-(unsigned char)(TEMPERATURE_OFFSET/2)))&&(error_flags.chiller_heater_relay_reset==FALSE)){		//v2.3.6
			CHILLER_HEATER_ON;	timer2_sec_delay(2);
			CHILLER_HEATER_OFF;	timer2_sec_delay(2);
			CHILLER_HEATER_ON;	timer2_sec_delay(2);
			CHILLER_HEATER_OFF;	timer2_sec_delay(2);
			CHILLER_HEATER_OFF;	timer2_sec_delay(2);
			error_flags.chiller_heater_relay_reset = TRUE;
		}
		else if((int)DS_temperature>65){
			error_flags.chiller_heater_relay_reset = TRUE;
		}
		else{}
                //		if((((int)DS_temperature) <= (unsigned char)(water_temperature_level-TEMPERATURE_OFFSET))&&(error_flags.chiller_heater_relay_reset==TRUE)){		//v2.3.6
		if(((((int)DS_temperature) <= (unsigned char)(water_temperature_level-TEMPERATURE_OFFSET))||((int)DS_temperature)>65)&&(error_flags.chiller_heater_relay_reset==TRUE))		//v2.3.6
                {
                    if(((int)DS_temperature)>65){
				peripheral_flags.chiller_heater_en_dis = 0;				//v2.3.6
				eeprom_write(SER_CHILLER_HEATER_EN_DIS);				//v2.3.6
				send_server_response(DEV_CHILLER_HEATER_EN_DIS);		//v2.3.6
			}
			else{}
			device_fault();
		}
		else{}
	}

//v2.3.6 -- HEATER control --	//
	else if((peripheral_flags.chiller_heater_en_dis==2) && (DS_temperature>0.0) && (mode_flags.op_mode != ERROR_MODE) && (mode_flags.op_mode != DUMP_MODE) && (mode_flags.op_mode != REFILL_MODE) && (mode_flags.op_mode != DIAGNOSTIC_MODE) && (mode_flags.op_mode != DEBUG_MODE))   //v2.2.7 //v2.3.6
	{
		if(((peripheral_flags.warm_water == 1) || (peripheral_flags.chiller_heater_lls_trigger==TRUE)) && (peripheral_flags.heater_status_flag==TRUE))     //FOR TESTING PURPOSE //v2.3.6
		{
			CHILLER_HEATER_OFF;

			lcd_init();
			strcpy(lcd_data,"HEATER OFF");
			lcd_display(1);
			sprintf(lcd_data,"TEMP.:%.5s",temp_value);
			lcd_display(2);
			buzzer(AUT01);
			timer2_sec_delay(error_display_time);
			
			peripheral_flags.heater_status_flag=FALSE;
                        peripheral_flags.chiller_status_flag=FALSE;  //v3.1.2.c  
			chiller_heater_time_stamp = 0;                       //clearing timeout count	//v2.3.6
		}
                else if((peripheral_flags.warm_water == 0) && (peripheral_flags.chiller_heater_lls_trigger==FALSE) && (peripheral_flags.heater_status_flag==FALSE) && (peripheral_flags.chiller_heater_start_flag==TRUE))  //v2.3.6
		{
			CHILLER_HEATER_ON;

			lcd_init();
			strcpy(lcd_data,"HEATER ON");
			lcd_display(1);
			sprintf(lcd_data,"TEMP.:%.5s",temp_value);
			lcd_display(2);
			buzzer(AUT01);
			timer2_sec_delay(error_display_time);
		
			peripheral_flags.heater_status_flag=TRUE;
                        peripheral_flags.chiller_status_flag=FALSE;  //v3.1.2.c 
//			timer1_init();
			chiller_heater_time_stamp = min_tick;	      // starting min count for timeout period	//v2.3.6
		}
		else{}
                if ((((int) DS_temperature) >= (unsigned char) (water_temperature_level + (unsigned char) (TEMPERATURE_OFFSET / 2)))&&(error_flags.chiller_heater_relay_reset == FALSE))
                { //v2.3.6
                    CHILLER_HEATER_ON;        timer2_sec_delay(2);
                    CHILLER_HEATER_OFF;       timer2_sec_delay(2);
                    CHILLER_HEATER_ON;        timer2_sec_delay(2);
                    CHILLER_HEATER_OFF;       timer2_sec_delay(2);
                    CHILLER_HEATER_OFF;       timer2_sec_delay(2);
                    error_flags.chiller_heater_relay_reset = TRUE;
                }
                else if ((int) DS_temperature < 2)
                {
                    error_flags.chiller_heater_relay_reset = TRUE;
                }
                else{}
                
//		if(((int)DS_temperature) > (unsigned char)(water_temperature_level+TEMPERATURE_OFFSET)&&(error_flags.chiller_heater_relay_reset==TRUE)){		//v2.3.6
		if(((((int)DS_temperature) > (unsigned char)(water_temperature_level+TEMPERATURE_OFFSET))||( (int)DS_temperature<2) )&&(error_flags.chiller_heater_relay_reset==TRUE))		//v2.3.6
                { 
                    if(((int)DS_temperature)<2){
				peripheral_flags.chiller_heater_en_dis = 0;				//v2.3.6
				eeprom_write(SER_CHILLER_HEATER_EN_DIS);				//v2.3.6
				send_server_response(DEV_CHILLER_HEATER_EN_DIS);		//v2.3.6
			}
                    else{}
			device_fault();
		}
		else{}                
	}
	else
	{
		CHILLER_HEATER_OFF;
		peripheral_flags.chiller_status_flag=FALSE;
		peripheral_flags.heater_status_flag=FALSE;	//v2.3.6
		chiller_heater_time_stamp=0;				//v2.3.6
                peripheral_flags.warm_water = 0;			//v2.3.6
		peripheral_flags.cold_water = 0;			//v2.3.6
	}
//************CHILLER SLEEP TIMEOUT CONTROL*********************
		if((min_tick-chiller_heater_time_stamp>=120) && ((peripheral_flags.chiller_status_flag==TRUE)||(peripheral_flags.heater_status_flag==TRUE)))  //true only when chiller is ON & ON for timeout period	//v2.3.6
		{
		
			lcd_init();

			if(peripheral_flags.chiller_heater_en_dis==1)	//v2.3.6
				strcpy(lcd_data,"CHILLER SLEEP");
			else if (peripheral_flags.chiller_heater_en_dis==2)
				strcpy(lcd_data,"HEATER SLEEP");
			else{}

			lcd_display(1);
			sprintf(lcd_data,"TEMP: %.5s",temp_value);
			lcd_display(2);
                        buzzer(ERR01);
			timer2_sec_delay(error_display_time);
//			send_server_response(DEV_CHILLER_EN_DIS);

			chiller_heater_time_stamp=0;		//v2.3.6
			peripheral_flags.chiller_heater_start_flag=FALSE;	//v2.3.6

			buzzer(AUT01);
			CHILLER_HEATER_OFF;
			peripheral_flags.chiller_status_flag=FALSE;
			peripheral_flags.heater_status_flag=FALSE;		//v2.3.6
			chiller_heater_time_stamp = min_tick;	//v2.3.6
		}	
		else{}

		if(	peripheral_flags.chiller_heater_start_flag == FALSE)	//v2.3.6
		{
			if(min_tick-chiller_heater_time_stamp>=30)		//v2.3.6  
			{
				peripheral_flags.chiller_heater_start_flag=TRUE;
				chiller_heater_time_stamp=0;		//v2.3.6
			}
			else{}
		}
		else{}
//******************************************************
}

void device_fault()
{
    CHILLER_HEATER_OFF;
    chiller_heater_time_stamp = min_tick;
    lcd_init();
    if (peripheral_flags.chiller_heater_en_dis == 1)
        strcpy(lcd_data, "CHILLER FAULT   ");
    else if (peripheral_flags.chiller_heater_en_dis == 2)
        strcpy(lcd_data, "HEATER FAULT    ");
    else
        strcpy(lcd_data, "DEVICE FAULT    ");
    lcd_display(1);
    strcpy(lcd_data, "CONTACT SARVAJAL");
    lcd_display(2);
    buzzer(AUT01);

    coinbox_disable(); //v2.3.6
    if (coin_flags.coinenable == FALSE) //v2.3.6
        peripheral_flags.coin_en_dis = 0; //v2.3.6
    else
    {
    }
    coin_flags.coin_disable_fault_alert = 1; //v2.3.6

    send_server_response(DEV_FAULT_ERROR);
    timer2_sec_delay(2);
    memset(sms_sender_no,'\0',sizeof(sms_sender_no)); //1.0.1	 //3.1.2.I //do_finalize
    send_server_response(DEV_FAULT_ERROR + 200);
    timer2_sec_delay(2);
    //		peripheral_flags.chiller_heater_en_dis = 0;				//v2.3.6
    //		eeprom_write(SER_CHILLER_HEATER_EN_DIS);				//v2.3.6
    //		send_server_response(DEV_CHILLER_HEATER_EN_DIS);		//v2.3.6
    //		timer2_sec_delay(1);

    while (1)
    {
        consumer_flags.rfid_read = LOW;
        consumer_flags.coin_read = LOW;
        consumer_flags.dispense_button = LOW;
        check_sms();

        if ((system_flags.send_idle_msg == TRUE) &&(mode_flags.op_mode != DIAGNOSTIC_MODE))
        {
            strcpy(error_state, "RL36");
            timer2_ms_delay(200);
            send_server_response(DEV_HB_MSG);
            system_flags.send_idle_msg = FALSE;
        }
        else
        {
        }

        if (min_tick - chiller_heater_time_stamp >= 1)
        {
            lcd_init();
            if (peripheral_flags.chiller_heater_en_dis == 1)
                strcpy(lcd_data, "CHILLER FAULT   ");
            else if (peripheral_flags.chiller_heater_en_dis == 2)
                strcpy(lcd_data, "HEATER FAULT    ");
            else
                strcpy(lcd_data, "DEVICE FAULT    ");
            lcd_display(1);
            strcpy(lcd_data, "CONTACT SARVAJAL");
            lcd_display(2);
            chiller_heater_time_stamp = min_tick;
        }
        else
        {
        }
    }
}
#define USE_AND_OR

#include "DS1307.h"
#include "ext_lcd.h"
#include "variable.h"
#include "mcu_timer.h"
#include "FSIO.h"
#include <stdlib.h>
#include <ports.h>
#include <i2c.h>
#include "ext_eeprom.h"

void DS1307_config(void) {
    unsigned int config1 = 0;
    unsigned int config2 = 0;

    /* Turn off I2C modules */
    CloseI2C1(); //Disbale I2C1 mdolue if enabled previously

    //************ I2C interrupt configuration ******************************************************
    ConfigIntI2C1(MI2C_INT_OFF); //Disable I2C interrupt
    //***************** I2C1 configuration **********************************************************
    /**********************************************************************************************
     *
     *        I2C1 enabled
     *        continue I2C module in Idle mode
     *        IPMI mode not enabled
     *        I2CADD is 7-bit address
     *        Disable Slew Rate Control for 100KHz
     *        Enable SM bus specification
     *        Disable General call address
     *        Baud @ 16MHz = 78 into I2CxBRG
     **********************************************************************************************/
    config1 = (I2C_ON | I2C_7BIT_ADD);
    //	config1 = (I2C_ON  | I2C_7BIT_ADD|I2C_IDLE_STOP);	//v2.2.4
    config2 = 78; // Actual FSCL = 100KHz
    OpenI2C1(config1, config2); //configure I2C1

    //	   	start_time();
}

void set_time_DS1307() {
    IdleI2C1();
    StartI2C1();
    while (I2C1CONbits.SEN); //Wait till Start sequence is completed
    MI2C1_Clear_Intr_Status_Bit; //Clear interrupt flag	
    DS1307_byte_write(0xD0);
    DS1307_byte_write(0x00);
    DS1307_byte_write((unsigned char) sd_sec); // second
    DS1307_byte_write((unsigned char) sd_min); //min	
    DS1307_byte_write((unsigned char) sd_hour); //hour
    DS1307_byte_write(0x01); //day
    DS1307_byte_write((unsigned char) sd_date); // date
    DS1307_byte_write((unsigned char) sd_month); //month
    DS1307_byte_write((unsigned char) sd_yr); //year



    IdleI2C1(); //wait for the I2C to be Idle
    StopI2C1(); //Terminate communication protocol with stop signal
    while (I2C1CONbits.PEN); //Wait till stop sequence is completed	
    strcpy(lcd_data, "TIME SET");
    lcd_display(1);
    timer2_ms_delay(1000);
    //	start_time(); 
    //	strcpy(lcd_data,"TIME START");lcd_display(1); 	
    //	timer2_ms_delay(3000);
}

void DS1307_byte_write(unsigned char addr_1) {
    MasterWriteI2C1(addr_1); //Disable Sequencial Address
    while (I2C1STATbits.TBF); //Wait till address is transmitted
    while (!IFS1bits.MI2C1IF); //Wait for ninth clock cycle
    MI2C1_Clear_Intr_Status_Bit; //Clear interrupt flag	
    while (I2C1STATbits.ACKSTAT);
    //	 while(I2C1STATbits.TRSTAT);

}

void start_time() {

    IdleI2C1();
    StartI2C1();
    while (I2C1CONbits.SEN); //Wait till Start sequence is completed
    MI2C1_Clear_Intr_Status_Bit; //Clear interrupt flag	
    DS1307_byte_write(0xD0);
    DS1307_byte_write(0x00);
    DS1307_byte_write(0x00); // second	
    IdleI2C1(); //wait for the I2C to be Idle
    StopI2C1(); //Terminate communication protocol with stop signal
    while (I2C1CONbits.PEN); //Wait till stop sequence is completed	

}

void get_local_time() {
    memset(temp_sd_bytes, '\0', sizeof (temp_sd_bytes));
    rtc_read1307(0x00, 1);

    sd_sec = ((((temp_sd_bytes[0] >> 4)&0x07)*10)+(temp_sd_bytes[0] & 0x0F));
    //	lcd_display_line1();	BCD_lcd_data(temp_sd_bytes[0]);timer2_ms_delay(1);

    memset(temp_sd_bytes, '\0', sizeof (temp_sd_bytes));
    rtc_read1307(0x01, 1);
    sd_min = ((((temp_sd_bytes[0] >> 4)&0x0F)*10)+(temp_sd_bytes[0] & 0x0F));
    //	BCD_lcd_data(temp_sd_bytes[0]);timer2_ms_delay(1);

    memset(temp_sd_bytes, '\0', sizeof (temp_sd_bytes));
    rtc_read1307(0x02, 1);
    sd_hour = ((((temp_sd_bytes[0] >> 4)&0x0F)*10)+(temp_sd_bytes[0] & 0x0F));
    //	BCD_lcd_data(temp_sd_bytes[0]);timer2_ms_delay(1);


    memset(temp_sd_bytes, '\0', sizeof (temp_sd_bytes));
    rtc_read1307(0x04, 1);
    sd_date = ((((temp_sd_bytes[0] >> 4)&0x0F)*10)+(temp_sd_bytes[0] & 0x0F));
    //	lcd_display_line2();	BCD_lcd_data(temp_sd_bytes[0]);timer2_ms_delay(1);

    memset(temp_sd_bytes, '\0', sizeof (temp_sd_bytes));
    rtc_read1307(0x05, 1);
    sd_month = ((((temp_sd_bytes[0] >> 4)&0x0F)*10)+(temp_sd_bytes[0] & 0x0F));
    //BCD_lcd_data(temp_sd_bytes[0]);timer2_ms_delay(1);

    memset(temp_sd_bytes, '\0', sizeof (temp_sd_bytes));
    rtc_read1307(0x06, 1);
    sd_yr = (((temp_sd_bytes[0] >> 4)&0x0F)*10)+(temp_sd_bytes[0] & 0x0F);
    sd_year = 2000 + sd_yr;
    //BCD_lcd_data(temp_sd_bytes[0]);timer2_ms_delay(1);
    if (sd_yr == 0)
    {
	//		get_modem_local_time();
	read_eeprom_parameter(SER_PREV_YEAR);
	sd_yr = P_yr;
	sd_year = sd_yr + 2000;
    } else
    {
    }

}

void rtc_read1307(unsigned char address_rtc, unsigned char no_of_bytes) {
    //	unsigned char l=0;
    IdleI2C1();
    StartI2C1();
    while (I2C1CONbits.SEN); //Wait till Start sequence is completed
    MI2C1_Clear_Intr_Status_Bit; //Clear interrupt flag	
    DS1307_byte_write(0xD0);
    DS1307_byte_write(address_rtc);
    IdleI2C1(); //wait for the I2C to be Idle
    RestartI2C1();
    while (I2C1CONbits.RSEN); //Wait till Restart sequence is completed
    //	  for(l=0;l<1000;l++);
    Nop();
    MI2C1_Clear_Intr_Status_Bit;
    MasterWriteI2C1(0xD1);
    while (!IFS1bits.MI2C1IF); //Wait for ninth clock cycle
    while (I2C1STATbits.ACKSTAT); //check for ACK from slave
    MI2C1_Clear_Intr_Status_Bit;
    MastergetsI2C1(no_of_bytes, temp_sd_bytes, 1000); //(no_of_bytes,ee_bytes,1000);
    //	check_data = MasterReadI2C1();
    IdleI2C1(); //wait for the I2C to be Idle
    StopI2C1(); //Terminate communication protocol with stop signal
    while (I2C1CONbits.PEN); //Wait till stop sequence is completed	
    timer2_ms_delay(10);
}

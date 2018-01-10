#define USE_AND_OR /* To enable AND_OR mask setting */

#include "ext_lcd.h"
#include "mcu_timer.h"
#include "p24FJ256GB110.h"
#include "variable.h"
#include<ports.h>


void lcd_init(void)
{
	unsigned temp_count=0;

	mPORTGOutputConfig(0xD000);	//PG0-1 & 12-15 as O/P
	mPORTEOutputConfig(0x001E);		//PD9 as O/P - LCD Backlight

	send_lcd_command();

	for(temp_count=0; temp_count<sizeof(lcd_data); temp_count++)	//v2.2.4
		lcd_data[temp_count] = 0;

	//clear_lcd();
	if(consumer_flags.freeze_display==TRUE)
	{
		LCD_BACKLITE_ON;
		consumer_flags.freeze_display = FALSE;
		clear_lcd();
		//---strcpy(lcd_data,lcd_data_freeze[--lcd_freeze_counter]);
		strcpy(lcd_data,lcd_line1_data_freeze);
		lcd_display(1);
		
		//---strcpy(lcd_data,lcd_data_freeze[--lcd_freeze_counter]);
		strcpy(lcd_data,lcd_line2_data_freeze);
		lcd_display(2);	
		//lcd_freeze_counter+=2;
		consumer_flags.freeze_display = TRUE;
	}
	else
		LCD_BACKLITE_OFF;
}

void lcd_display(unsigned char line_no)
{
	unsigned char counter=0;
		
	if(line_no<=2)		//v2.2.7  (display LCD with Backlit OFF)
	{
		LCD_BACKLITE_ON;
	}
	else
		line_no -= 2;
	
if(consumer_flags.freeze_display==FALSE)	//Do not change display content is flag is TRUE
{
	if(line_no==1)
		lcd_display_line1();
	else if(line_no==2)
		lcd_display_line2();
	else
		{}

	lcd_rs_pin=LCD_DATA;
	for(counter=0; counter<sizeof(lcd_data); counter++)
	{
		lcd_data_reg = lcd_data[counter];
		if(((lcd_data_reg>=' ') && (lcd_data_reg<='~')) || (lcd_data_reg==0))
		{
			if(lcd_data_reg!=0)
				send_lcd_byte(lcd_data_reg);
			else
				send_lcd_byte(' ');
		}
		else{}
		lcd_data[counter] = 0;
	}
	lcd_rs_pin=FALSE;
}
else{}

}

void send_lcd_byte(unsigned char byte)
{
	unsigned char temp=0;	

	temp = byte;
	temp = (temp & (0xF0));
	temp = temp >> 4;
	send_lcd_nibble(temp);
	byte = (byte & (0x0F));
	send_lcd_nibble(byte);
}

void BCD_lcd_data(unsigned char data_3)
{
//	lcd_display_line1();
	lcd_rs_pin=LCD_DATA;
	lcd_rs_pin=LCD_DATA;
	send_lcd_byte((data_3>>4)|0x30);
	send_lcd_byte((data_3 & 0x0F)|0x30);	
	send_lcd_byte(':');	
	lcd_rs_pin=FALSE;
}



void send_lcd_nibble(unsigned char data)
{
	unsigned int temp=0;

	if(lcd_rs_pin==LCD_DATA)
	{	LCD_RS_DATA_EN;}
	else
	{	LCD_RS_CMD_EN;}
		
	temp=(unsigned int)data;
	temp = temp << 1;
	temp &= (0x001E);	//Mask all bits except LCD data pins
	//temp = (temp |(0x0F));			
	LCD_DATA_PORT |= temp;
	
	LCD_EN_ON;				// E=1
	//timer1_100us_delay(1);
	timer2_ms_delay(1);			// 1 ms
	LCD_EN_OFF;					// E=0
		
	temp = 0xFFE1;
	LCD_DATA_PORT &= temp;
}


	

void send_lcd_command(void)
{
	lcd_rs_pin = LCD_CMD;

	timer2_ms_delay(15);			//15 ms

	lcd_data_reg = 0x03;			// send 03		
	send_lcd_nibble(lcd_data_reg);		
	
	timer2_ms_delay(10);			// 10 ms
	lcd_data_reg = 0x03;			// send 03	    
	send_lcd_nibble(lcd_data_reg);		

	timer2_ms_delay(2);
	lcd_data_reg = 0x03;			// send 03		
	send_lcd_nibble(lcd_data_reg);		

	timer2_ms_delay(2);
	lcd_data_reg = 0x02;			// send 02		
	send_lcd_nibble(lcd_data_reg);		//to set interface length
						
	timer2_ms_delay(2);
	lcd_data_reg = 0x28;			// send 28      
	send_lcd_byte(lcd_data_reg);	//Send 28 to set No of Lines 2 font
		
	timer2_ms_delay(2);
	lcd_data_reg = 0x0C;			//	send 0C
	send_lcd_byte(lcd_data_reg);	//Set Display on, Cursor ON ,Blink OFF

	timer2_ms_delay(2);
	lcd_data_reg = 0x14;			//	send 14			
	send_lcd_byte(lcd_data_reg);	//Set Auto-inc 

	timer2_ms_delay(2);
	clear_lcd();
	lcd_rs_pin = FALSE;

}

void clear_lcd(void)
{
	if(consumer_flags.freeze_display==FALSE)	//Do not change display content is flag is TRUE
	{
		lcd_rs_pin = LCD_CMD;

		lcd_data_reg = 0x01;							
		send_lcd_byte(lcd_data_reg);	//Clear Display		

		timer2_ms_delay(1);

		lcd_data_reg = 0x02;			//Return Cursor					
		send_lcd_byte(lcd_data_reg);			//on 1st line

		timer2_ms_delay(1);				//1 ms
		lcd_rs_pin = FALSE;
	}
	else{}
}

void lcd_display_line1(void)
{
	lcd_rs_pin = LCD_CMD;

	lcd_data_reg = 0x02;			//Return Cursor
	send_lcd_byte(lcd_data_reg);	//on 1st line
		
	timer2_ms_delay(1);				//1 ms		
	lcd_rs_pin = FALSE;
}
		
void lcd_display_line2(void)
{
	lcd_rs_pin = LCD_CMD;

	lcd_data_reg = 0xC0;			//Display on nxt line
	send_lcd_byte(lcd_data_reg);	
	timer2_ms_delay(1);				//1 ms	
	lcd_rs_pin = FALSE;	
}
void lcd_set_cursor(unsigned char symbol,unsigned char line ,unsigned char location){

//	Send cousor to location	//
	lcd_rs_pin = LCD_CMD;
	if(line == 1) lcd_data_reg = (0x80+(location-1));
	else lcd_data_reg = (0xC0+(location-1));
	send_lcd_byte(lcd_data_reg);	
	timer2_ms_delay(1);				//1 ms	
	lcd_rs_pin = FALSE;	
//	To Print Arrow	//
	if(symbol==1){
		lcd_rs_pin = LCD_DATA;
		lcd_data_reg = 0x7F;			//
		send_lcd_byte(lcd_data_reg);	//on 1st line
		timer2_ms_delay(1);				//1 ms		
		lcd_rs_pin = FALSE;
	
		lcd_rs_pin = LCD_DATA;
		lcd_data_reg = '-';			//
		send_lcd_byte(lcd_data_reg);	//on 1st line
		timer2_ms_delay(1);				//1 ms		
		lcd_rs_pin = FALSE;
	
		lcd_rs_pin = LCD_CMD;
		if(line == 1) lcd_data_reg = (0x80);
		else lcd_data_reg = (0xC0);
		send_lcd_byte(lcd_data_reg);	
		timer2_ms_delay(1);				//1 ms	
		lcd_rs_pin = FALSE;	
//		lcd_rs_pin = LCD_CMD;
//		lcd_data_reg = 0x0D;			//cursor off cursor blink
//		send_lcd_byte(lcd_data_reg);	//on 1st line
//		timer2_ms_delay(1);				//1 ms		
//		lcd_rs_pin = FALSE;
	}
	else{
		lcd_rs_pin = LCD_CMD;
		if(line == 1) lcd_data_reg = (0x80+(location-1));
		else lcd_data_reg = (0xC0+(location-1));
		send_lcd_byte(lcd_data_reg);	
		timer2_ms_delay(1);				//1 ms	
		lcd_rs_pin = FALSE;	
	}
//	cursor off cursor blink	//
	lcd_rs_pin = LCD_CMD;
	lcd_data_reg = 0x0D;			//cursor off cursor blink
	send_lcd_byte(lcd_data_reg);	//on 1st line
	timer2_ms_delay(1);				//1 ms		
	lcd_rs_pin = FALSE;
}
void lcd_byte_display(unsigned char line ,unsigned char location, unsigned char data){


	lcd_rs_pin = LCD_CMD;
	if(line == 1) lcd_data_reg = (0x80+(location-1));
	else lcd_data_reg = (0xC0+(location-1));
	send_lcd_byte(lcd_data_reg);	
	timer2_ms_delay(1);				
	lcd_rs_pin = FALSE;	

	lcd_rs_pin = LCD_CMD;
	lcd_data_reg = 0x0E;			//cursor on cursor blink
	send_lcd_byte(lcd_data_reg);	//on 1st line
	timer2_ms_delay(1);				//1 ms		
	lcd_rs_pin = FALSE;

	lcd_rs_pin = LCD_DATA;
	lcd_data_reg = data;			
	send_lcd_byte(lcd_data_reg);	
	timer2_ms_delay(1);				
	lcd_rs_pin = FALSE;

	lcd_rs_pin = LCD_CMD;
	if(line == 1) lcd_data_reg = (0x80+(location-1));
	else lcd_data_reg = (0xC0+(location-1));
	send_lcd_byte(lcd_data_reg);	
	timer2_ms_delay(1);				
	lcd_rs_pin = FALSE;	
}







#define USE_AND_OR

#define LCD_DATA 1
#define LCD_CMD 0

#define LCD_RS_DATA_EN mPORTGSetBits(0x4000)
#define LCD_RS_CMD_EN mPORTGClearBits(0x4000)

#define LCD_EN_ON mPORTGSetBits(0x1000)
#define LCD_EN_OFF mPORTGClearBits(0x1000)

#define LCD_BACKLITE_ON mPORTGSetBits(0x8000)
//#define LCD_BACKLITE_ON {if(mode_flags.op_mode!=DEBUG_MODE)mPORTDSetBits(0x0200)}
#define LCD_BACKLITE_OFF mPORTGClearBits(0x8000)

#define LCD_DATA_PORT LATE //PORTG 

void lcd_display(unsigned char line_no);
void send_lcd_byte(unsigned char byte);
void send_lcd_nibble(unsigned char data);
void send_lcd_command(void);
void clear_lcd(void);
void lcd_display_line1(void);
void lcd_display_line2(void);
void lcd_init(void);
void BCD_lcd_data(unsigned char data_3);
void lcd_set_cursor(unsigned char symbol,unsigned char line ,unsigned char location);
void lcd_byte_display(unsigned char line ,unsigned char location, unsigned char data);








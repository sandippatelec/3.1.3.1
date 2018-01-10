#include "serial_uart.h"
#include "variable.h"
#include "server.h"

void UART1_init(void)		//(RFID Reader)
{

/*********************** UART 1 Configuration (RFID Reader) ****************************************************
*		9600 baudrate
*		Low baud rate
*		8 bit transmission/reception
*		No parity bit
*		1 stop bit
************************************************************************************************/	
    /*Enable UART interrupt*/	    
	ConfigIntUART1(UART_RX_INT_EN|UART_RX_INT_PR6);	
    
	//---OpenUART1(UART_EN,UART_TX_ENABLE,51);			//configure UART and enable it
	OpenUART1(UART_EN|UART_IDLE_STOP,UART_TX_ENABLE,25);			//v2.2.4   configure UART and enable it

	EnableIntU1RX;

	U1RX_Clear_Intr_Status_Bit;
}

void UART2_init(void)	//(Modem)
{

Tx_Buffer_Head = 0;
Tx_Buffer_Tail = 0;
Rx_Buffer_Head = 0;
Rx_Buffer_Tail = 0;

/*********************** UART 2 Configuration (Modem Communication) ****************************************************
*		9600 baudrate
*		Low baud rate
*		8 bit transmission/reception
*		No parity bit
*		1 stop bit
************************************************************************************************/	
    /*Enable UART interrupts*/	    
	ConfigIntUART2(UART_RX_INT_EN |UART_RX_INT_PR7 );	//| UART_TX_INT_EN |UART_TX_INT_PR6);

	//---OpenUART2(UART_EN, UART_TX_ENABLE,51);			//configure UART and enable it
	OpenUART2(UART_EN|UART_IDLE_STOP, UART_TX_ENABLE,51);			//v2.2.4  configure UART and enable it
}
void UART3_init(void)		//For Coin box
{
/*********************** UART 3 Configuration (Coin box) ****************************************************
*		9600 baudrate
*		Low baud rate
*		8 bit transmission/reception
*		No parity bit
*		1 stop bit
************************************************************************************************/	
    /*Enable UART interrupt*/
    
    ConfigIntUART3(UART_RX_INT_EN|UART_RX_INT_PR5);    
	OpenUART3(UART_EN,UART_TX_ENABLE,51);			//configure UART and enable it
	EnableIntU3RX;
	U3RX_Clear_Intr_Status_Bit;
//	IEC5bits.U3RXIE = 1;
//	IPC20  = 0x0700;
}


void serial_string_send(const char *string)
{
	serial_buffer_flush();
	server_buffer_flush();
		
	while(*string)
		serial_byte_send(*string++);
}

void serial_byte_send(unsigned char data)
{
    while(BusyUART2());								//wait till the UART is busy	
	WriteUART2(data);
}

unsigned char serial_byte_receive(void)
{
    unsigned char tmptail;
    unsigned char data;

    tmptail = (Rx_Buffer_Tail + 1) & MAX_RX_BUFFER_MASK;
    Rx_Buffer_Tail = tmptail; 
    data = Rxdata[tmptail];

    return data;
}

void serial_buffer_flush(void)
{
	unsigned int i=0;
	
	Rx_Buffer_Head = 0;
	Rx_Buffer_Tail = 0;
//	for(i=0; i<MAX_RX_BUFFER_LIMIT; i++)	
//		Rxdata[i] = 0;
        memset(Rxdata, 0, sizeof (Rxdata));  //v3.1.2.G
	
	Rxdata[0] = 0xFF;
}





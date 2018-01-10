
#define USE_AND_OR
#include "uart.h"
#include "main.h"

#define MAX_RX_BUFFER_MASK ( MAX_RX_BUFFER_LIMIT - 1)
#define MAX_TX_BUFFER_MASK ( MAX_TX_BUFFER_LIMIT - 1)

#if(MAX_RX_BUFFER_LIMIT & MAX_RX_BUFFER_MASK)
#error RX buffer size is not a power of 2
#endif

#if(MAX_TX_BUFFER_LIMIT & MAX_TX_BUFFER_MASK)
#error TX buffer size is not a power of 2
#endif

void UART1_init(void);
void UART2_init(void);
void UART3_init(void);		//v2.3.2
void serial_string_send(const char *string);
void serial_byte_send(unsigned char data);
unsigned char serial_byte_receive(void);
void serial_buffer_flush(void);


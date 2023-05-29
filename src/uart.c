#include "headers.h"
#include "uart.h"

static FILE mystdout = FDEV_SETUP_STREAM(uart_putc_printf, NULL, _FDEV_SETUP_WRITE);

void uart_init(void)
{
    stdout = &mystdout;
    PORTB.DIRSET |= PIN2_bm;                      // Enable PB2 as output (USART0 TXD)
    USART0.BAUD = 1389;                           // 9600 baud @ 3.3 MHz
    USART0.CTRLB = USART_RXEN_bm | USART_TXEN_bm; // Enable Tx/Rx
}

uint8_t uart_getc(void)
{
    while (!(USART0.STATUS & USART_RXCIF_bm))
        ; // Wait for data
    return USART0.RXDATAL;
}

void uart_putc(uint8_t c)
{
    while (!(USART0.STATUS & USART_DREIF_bm))
        ; // Wait for TXDATA empty
    USART0.TXDATAL = c;
}

int uart_putc_printf(char c, FILE *stream)
{
    uart_putc(c);
    return 0;
}

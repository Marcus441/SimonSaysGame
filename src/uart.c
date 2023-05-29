#include "headers.h"
#include "uart.h"

static FILE mystdout = FDEV_SETUP_STREAM(uart_putc_printf, NULL, _FDEV_SETUP_WRITE);

int uart_putc_printf(char c, FILE *stream)
{
    uart_putc(c);
    return 0;
}

void uart_init(void) {
    stdout = &mystdout;
    PORTB.DIRSET |= PIN2_bm; // Enable PB2 as output (USART0 TXD)
    USART0.BAUD = 1389;     // 9600 baud @ 3.3 MHz
    USART0.CTRLB = USART_RXEN_bm | USART_TXEN_bm;   // Enable Tx/Rx

}


uint8_t uart_getc(void) {
    while (!(USART0.STATUS & USART_RXCIF_bm));  // Wait for data
    return USART0.RXDATAL;
}

void uart_putc(uint8_t c) {
    while (!(USART0.STATUS & USART_DREIF_bm));  // Wait for TXDATA empty
    USART0.TXDATAL = c;
}

void uart_puts(char* string) {
    while(*string != '\0'){
        uart_putc(*string);
        string++;
    }
}

void println(char* string) {
    printf(string);
    printf("\n");
}

// void print(char* format, ...) {
//     char* character;
//     uint16_t i;
//     char* s;
//     for (character = format; *character != '\0'; character++)
//     {   
//         while (*character != '%')
//         {
//             uart_putc(*character);
//             character++;
//         }
//         character++;

//         switch (*character)
//         {
//         case 'c':
//             uart_putc(i);
//             break;
        
//         default:
//             break;
//         }
//     }
// }

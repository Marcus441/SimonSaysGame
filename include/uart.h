#include "headers.h"

void uart_init(void);
uint8_t uart_getc(void);
void uart_putc(uint8_t c);
int uart_putc_printf(char c, FILE *stream);
uint8_t hex_to_int(char c);

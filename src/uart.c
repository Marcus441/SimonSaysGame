#include "headers.h"
#include "uart.h"
#include "sequence.h"

#include "buzzer.h"
#include "timer.h"
#include "sequence.h"
#include "states.h"

extern volatile char name[20];

volatile SERIAL_STATE serial_state = Command_Wait;
volatile uint8_t chars_received = 0;

static FILE mystdout = FDEV_SETUP_STREAM(uart_putc_printf, NULL, _FDEV_SETUP_WRITE);

void uart_init(void)
{

    PORTB.DIRSET |= PIN2_bm; // Enable PB2 as output (USART0 TXD)
    USART0.CTRLA = USART_RXCIE_bm;
    USART0.BAUD = 1389;                           // 9600 baud @ 3.3 MHz 1389
    USART0.CTRLB = USART_RXEN_bm | USART_TXEN_bm; // Enable Tx/Rx
    stdout = &mystdout;
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

uint8_t hex_to_int(char c)
{
    if ('0' <= c && c <= '9')
    {
        return c - '0';
    }
    else if ('a' <= c && c <= 'f')
    {
        return 10 + c - 'a';
    }
    else
    {
        return 16; // invalid input
    }
}

ISR(USART0_RXC_vect)
{

    static uint16_t payload = 0;
    static bool payload_valid = true;

    uint8_t rx_data = USART0.RXDATAL;

    switch (serial_state)
    {
    case Command_Wait:
        // uart_putc(rx_data);
        switch (rx_data)
        {
        case '1':
        case 'q':
            if (pb == (UserInput | PB1))
            {
                pb = PB1;
                uart_control = true;
                pb_released = true;
            }
            break;
        case '2':
        case 'w':
            if (pb == UserInput)
            {
                pb = PB2;
                uart_control = true;
                pb_released = true;
            }
            break;
        case '3':
        case 'e':
            if (pb == UserInput)
            {
                pb = PB3;
                uart_control = true;
                pb_released = true;
            }
            break;
        case '4':
        case 'r':
            if (pb == UserInput)
            {
                pb = PB4;
                uart_control = true;
                pb_released = true;
            }
            break;
        case ',':
        case 'k':

            inc();

            break;
        case '.':
        case 'l':

            dec();

            break;
        case '0':
        case 'p':
            if (temp_seed)
                seed = temp_seed;

            // seed = init_seed;
            pb = Reset;
            break;
        case '9':
        case 'o':
            payload_valid = true;
            chars_received = 0;
            payload = 0;
            serial_state = Payload_Wait;
            break;
        default:
            break;
        }
        break;
    case Payload_Wait:
    {
        uint8_t toint = hex_to_int((char)rx_data);
        if (toint != 16)
            payload = (payload << 4) | toint;
        else
            payload_valid = false;

        if (++chars_received == 8)
        {
            if (payload_valid)
                temp_seed = payload;
            chars_received = 0;
            serial_state = Command_Wait;
        }
        break;
    }
    case uart_GetName:
        if (rx_data == '\n' || rx_data == '\r')
        {
            state = SetName;
            serial_state = Command_Wait;
        }
        else
        {
            name[chars_received] = rx_data;
            chars_received++;
            elapsed_time = 0;
        }
        break;
    default:
        break;
    }
}

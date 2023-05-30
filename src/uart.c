#include "headers.h"
#include "buzzer.h"
#include "states.h"

int uart_putc_printf(char c, FILE *stream);
extern volatile int8_t octave;
extern volatile GAMESTATES pb;
extern volatile bool pb_released;
extern volatile uint32_t seed;


void uart_init(void)
{
    static FILE mystdout = FDEV_SETUP_STREAM(uart_putc_printf, NULL, _FDEV_SETUP_WRITE);
    stdout = &mystdout;

    cli();
    PORTB.DIRSET |= PIN2_bm;                      // Enable PB2 as output (USART0 TXD)
    USART0.BAUD = 1389;                           // 9600 baud @ 3.3 MHz
    USART0.CTRLB = USART_RXEN_bm | USART_TXEN_bm; // Enable Tx/Rx
    sei();
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
    static SERIAL_STATE serial_state = Command_Wait;

    static uint8_t chars_received = 0;
    static uint16_t payload = 0;
    static bool payload_valid = true;

    uint8_t rx_data = USART0.RXDATAL;

    switch (serial_state)
    {
    case Command_Wait:
        switch (rx_data)
        {
        case '1':
        case 'q':
            if (pb == UserInput)
            {
                pb = PB1;
                pb_released = true;
            }
            break;
        case '2':
        case 'w':
            if (pb == UserInput)
            {
                pb = PB2;
                pb_released = true;
            }
            break;
        case '3':
        case 'e':
            if (pb == UserInput)
            {
                pb = PB3;
                pb_released = true;
            }
            break;
        case '4':
        case 'r':
            if (pb == UserInput)
            {
                pb = PB4;
                pb_released = true;
            }
            break;
        case ',':
        case 'k':
            if (octave < 3)
                octave++;
            break;
        case '.':
        case 'l':
            if (octave > -2)
                octave--;
            break;
        case '0':
        case 'p':
            octave = 0;
            seed = SID;
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
        uint8_t parsed_result = hex_to_int((char)rx_data);
        if (parsed_result != 16)
            payload = (payload << 4) | parsed_result;
        else
            payload_valid = 0;

        if (++chars_received == 8)
        {
            seed = payload_valid ? payload : seed;
            serial_state = Command_Wait;
        }
        break;
    }
    default:
        break;
    }
}





#include "headers.h"
#include "uart.h"
#include "sequence.h"

#include "buzzer.h"
#include "timer.h"
#include "sequence.h"
#include "states.h"

extern volatile char name[20]; // declare name variable from main.c

volatile SERIAL_STATE serial_state = Command_Wait; // declare serial state variable
volatile uint8_t chars_received = 0;               // declare chars received variable

static FILE mystdout = FDEV_SETUP_STREAM(uart_putc_printf, NULL, _FDEV_SETUP_WRITE); // declare mystdout variable to configure printf

void uart_init(void)
{

    PORTB.DIRSET |= PIN2_bm;                      // Enable PB2 as output (USART0 TXD)
    USART0.CTRLA = USART_RXCIE_bm;                // Enable RX interrupt
    USART0.BAUD = 1389;                           // 9600 baud @ 3.3 MHz 1389
    USART0.CTRLB = USART_RXEN_bm | USART_TXEN_bm; // Enable Tx/Rx
    stdout = &mystdout;                           // configure printf
}

uint8_t uart_getc(void)
{
    while (!(USART0.STATUS & USART_RXCIF_bm))
        ;                  // Wait for data
    return USART0.RXDATAL; // Read received character
}

void uart_putc(uint8_t c)
{
    while (!(USART0.STATUS & USART_DREIF_bm))
        ;               // Wait for TXDATA empty
    USART0.TXDATAL = c; // Send character
}

int uart_putc_printf(char c, FILE *stream)
{
    uart_putc(c); // send character
    return 0;     // return 0
}

uint8_t hex_to_int(char c) // convert hex to int
{
    if ('0' <= c && c <= '9') // if c is between 0 and 9
    {
        return c - '0'; // return c - 0
    }
    else if ('a' <= c && c <= 'f') // if c is between a and f
    {
        return 10 + c - 'a'; // return 10 + c - a
    }
    else // else
    {
        return 16; // invalid input
    }
}

ISR(USART0_RXC_vect)
{

    static uint16_t payload = 0;      // declare payload variable
    static bool payload_valid = true; // declare payload valid variable

    uint8_t rx_data = USART0.RXDATAL; // read received data

    switch (serial_state) // begin serial state state machine
    {
    case Command_Wait:
        switch (rx_data) // begin command state machine
        {
        // set up keys 1 - 4 and q, w, e, r to control the game
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
            if (pb == (UserInput | PB2))
            {
                pb = PB2;
                uart_control = true;
                pb_released = true;
            }
            break;
        case '3':
        case 'e':
            if (pb == (UserInput | PB3))
            {
                pb = PB3;
                uart_control = true;
                pb_released = true;
            }
            break;
        case '4':
        case 'r':
            if (pb == (UserInput | PB4))
            {
                pb = PB4;
                uart_control = true;
                pb_released = true;
            }
            break;
        // set up keys , and k to increase the octaves
        case ',':
        case 'k':
            inc(); // call inc function to increment octaves
            break;
        // set up keys . and l to decrease the octaves
        case '.':
        case 'l':
            dec(); // call dec function to decrement octaves
            break;
        // set up keys 0 and p to reset the game
        case '0':
        case 'p':
            if (temp_seed)        // if temp seed is not 0
                seed = temp_seed; // set seed to temp seed
            pb = Reset;           // set pb to reset
            break;
        // set up keys 9 and o to set a new seed
        case '9':
        case 'o':
            payload_valid = true;        // set payload valid to true
            chars_received = 0;          // set chars received to 0
            payload = 0;                 // set payload to 0
            serial_state = Payload_Wait; // set serial state to payload wait
            break;
        default:
            break;
        }
        break;
    case Payload_Wait:
    {
        uint8_t toint = hex_to_int((char)rx_data); // convert user input as hex to int
        if (toint != 16)                           // if toint is not an invalid input
            payload = (payload << 4) | toint;      // shift payload left 4 bits and or it with toint
        else
            payload_valid = false; // else payload is invalid

        if (++chars_received == 8) // if chars received is 8
        {
            if (payload_valid)           // if payload is valid
                temp_seed = payload;     // set temp seed to payload
            chars_received = 0;          // set chars received to 0
            serial_state = Command_Wait; // set serial state to command wait
        }
        break;
    }
    case uart_GetName:
        if (rx_data == '\n' || rx_data == '\r') // if rx data is a new line or carriage return
        {
            state = SetName;             // set state to set name
            serial_state = Command_Wait; // set serial state to command wait
        }
        else
        {
            name[chars_received] = rx_data; // set name at chars received to rx data
            chars_received++;               // increment chars received
            elapsed_time = 0;               // reset elapsed time
        }
        break;
    default:
        break;
    }
}

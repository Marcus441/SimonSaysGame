#include "uart.h"
#include "sequence.h"
#include "spi.h"
#include "buttons.h"

typedef enum{
    sequence_start,
    failed,
    success,
    users_turn

} GAMESTATE;
uint16_t sequence_length;

int main(void) {
    uart_init();
    spi_init();
    buttons_init();

    GAMESTATE state = sequence_start;
    while(1) {
        switch (state){
            case sequence_start:
                generate_sequence(sequence_length);
                state = users_turn;
                break;
            case users_turn:
                
                break;
            case success:
                sequence_length++;
                state = sequence_start;
                break;
            case failed:
                sequence_length = 0;
                state = sequence_start;
                break;
            // if statement
        }
    }

}


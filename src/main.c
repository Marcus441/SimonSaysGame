#include "uart.h"
#include "sequence.h"
#include "spi.h"
#include "buttons.h"
#include "buzzer.h"

typedef enum{
    sequence_start,
    failed,
    success,
    users_turn

} GAMESTATE;

int main(void) {
    uart_init();
    spi_init();
    buttons_init();
    //buzzer_init();

    GAMESTATE state = sequence_start;
    uint8_t outcome;
    uint16_t sequence_length = 0;

    while(1) {
        switch (state){
            case sequence_start:
                generate_sequence(sequence_length);
                state = users_turn;
                break;
            case users_turn:
                outcome = runSequence(sequence_length);
                if (outcome == 1)
                {
                    state = success;
                } else if(outcome == 0){
                    state = failed;
                }
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


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
    buzzer_init();

    printf("Game Start\n");
    GAMESTATE state = sequence_start;
    
    bool outcome;
    uint16_t sequence_length = 0;

    char stringarr[100];

    while(1) {
        switch (state){
            case sequence_start:
                
                sprintf(stringarr, "Sequence of length %d", sequence_length);
                println(stringarr);
                
                generate_sequence(sequence_length);
                state = users_turn;
                println("Users Turn");
                break;
            case users_turn:
                outcome = runSequence(sequence_length);
                if (outcome)
                {
                    state = success;
                    println("State = Success");
                } else if(outcome == false){
                    state = failed;
                    println("State = Failed");
                }
                break;
            case success:
                sequence_length++;
                state = sequence_start;
                println("Success");
                break;
            case failed:
                sequence_length = 0;
                state = sequence_start;
                println("Failed");
                break;
        }
    }

}


#include "uart.h"
#include "sequence.h"
#include "spi.h"
#include "buttons.h"
#include "buzzer.h"

typedef enum
{
    sequence_start,
    failed,
    success,
    users_turn

} GAMESTATE;

int main(void)
{
    uart_init();
    spi_init();
    buttons_init();
    buzzer_init();
    
    printf("Game Start\n");
    GAMESTATE state = sequence_start;

    bool outcome;
    uint16_t sequence_length = 0;

    while (1)
    {
        switch (state)
        {
        case sequence_start:
            generate_sequence(sequence_length);
            state = users_turn;
            printf("Users Turn\n");
            break;
        case users_turn:
            outcome = runSequence(sequence_length);
            if (outcome)
            {
                state = success;
                printf("State = Success\n");
            }
            else if (outcome == false)
            {
                state = failed;
                printf("State = Failed\n");
            }
            break;
        case success:
            sequence_length++;
            state = sequence_start;
            printf("Success\n");
            break;
        case failed:
            sequence_length = 0;
            state = sequence_start;
            printf("Failed\n");
            break;
        }
    }
}

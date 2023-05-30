#include "uart.h"
#include "sequence.h"
#include "spi.h"
#include "buttons.h"
#include "buzzer.h"
#include "adc.h"
#include "delay.h"
#include "states.h"

int main(void)
{
    uart_init();
    spi_init();
    buttons_init();
    buzzer_init();
    adc_init();
    delay_init();

    // printf("Game Start\n");
    GAMESTATES state = sequence_start;

    bool outcome;
    uint16_t sequence_length = 1;

    while (1)
    {
        switch (state)
        {
        case sequence_start:
            generate_sequence(sequence_length);
            state = UserInput;
            // printf("Users Turn\n");
            break;
        case UserInput:
            outcome = runSequence(sequence_length);
            if (outcome)
            {
                state = Success;
                // printf("State = Success\n");
            }
            else if (outcome == false)
            {
                state = Fail;
                // printf("State = Failed\n");
            }
            break;
        case Success:
            sequence_length++;
            state = sequence_start;
            // printf("Success\n");
            break;
        case Fail:
            sequence_length = 1;
            state = sequence_start;
            // printf("Failed\n");
            break;
        default:
            break;
        }
    }
}
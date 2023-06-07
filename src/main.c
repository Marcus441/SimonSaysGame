#include "headers.h"
#include "states.h"

#include "uart.h"
#include "sequence.h"
#include "spi.h"
#include "buttons.h"
#include "buzzer.h"
#include "adc.h"
#include "delay.h"
#include "timer.h"

void update_high_scores(uint16_t score);
void display_high_scores();

volatile GAMESTATES state; // declare state variable

volatile char name[20]; // declare name variable

typedef struct // declare player struct
{
    char name[20];      // declare name variable
    uint16_t HighScore; // declare high score variable
} player_t;             // declare player struct

player_t highScores[5];            // declare high score array of structs with 5 elements
volatile uint16_t sequence_length; // declare sequence length variable

int main(void)
{
    cli();          // clear global interrupts
    uart_init();    // initialize UART
    spi_init();     // initialize SPI
    buttons_init(); // initialize buttons
    buzzer_init();  // initialize buzzer
    adc_init();     // initialize ADC
    delay_init();   // initialize delay
    sei();          // enable global interrupts

    state = sequence_start; // set initial state

    bool outcome;                 // = false;
    uint16_t sequence_length = 1; // set initial sequence length

    while (1) // game loop
    {
        switch (state) // begin state machine
        {
        case sequence_start:
            generate_sequence(sequence_length); // generate sequence on spi
            state = UserInput;                  // set state to user input
            break;
        case UserInput:
            outcome = runSequence(sequence_length); // user performs sequence, returns true if correct
            if (outcome)                            // if correct
            {
                state = Success; // move to success state
            }
            else if (outcome == false) // if incorrect
            {
                state = Fail; // move to fail state
            }
            break;
        case Success:
            sequence_length++;      // increment sequence length on success
            state = sequence_start; // move to sequence start state to begin new sequence
            break;
        case Fail:
            for (uint8_t i = 0; i < 5; i++) // check if score is high score
            {
                if (sequence_length > highScores[i].HighScore) // if score is high score
                {
                    printf("Enter name: ");
                    serial_state = uart_GetName; // set serial state to get name
                    state = GetName;             // set state to get name
                    break;
                }
            }
            if (state != GetName) // if score is not high score
            {
                display_high_scores();  // display high scores
                state = sequence_start; // move to sequence start state to begin new sequence
            }

            if (temp_seed) // if temp_seed is not 0
            {
                seed = temp_seed; // set seed to temp_seed
                temp_seed = 0;    // set temp_seed to 0
            }
            break;
        case GetName:
            if (elapsed_time > 5000) // if 5 seconds have passed
            {
                serial_state = Command_Wait; // set serial state to command wait
                state = SetName;             // set state to set name
            }
            break;
        case SetName:
            name[chars_received] = '\0';         // set last character of name to null terminator
            update_high_scores(sequence_length); // update high scores table
            display_high_scores();               // display high scores table in uart

            chars_received = 0;     // reset chars_received
            sequence_length = 1;    // reset sequence_length
            state = sequence_start; // begin new sequence
            break;
        default:
            break;
        } // end state machine
    }     // end game loop
}

void update_high_scores(uint16_t score)
{
    for (uint8_t i = 0; i < 5; i++) // iterate through high scores
    {
        if (score > highScores[i].HighScore) // if score is greater than current high score
        {
            for (uint8_t j = 4; j > i; j--) // shift high scores down
                highScores[j] = highScores[j - 1];

            for (uint8_t j = 0; j < 20; j++) // copy name to high scores
                highScores[i].name[j] = name[j];
            highScores[i].HighScore = score; // set high score
            break;                           // break out of loop
        }
    }
}

void display_high_scores()
{
    for (uint8_t i = 0; i < 5; i++) // iterate through high scores
    {
        if (highScores[i].HighScore == 0) // if high score is 0
            break;                        // break out of loop

        printf("%s %d\n", highScores[i].name, highScores[i].HighScore); // print high score
    }
}
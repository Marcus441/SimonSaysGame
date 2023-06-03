#include "uart.h"
#include "sequence.h"
#include "spi.h"
#include "buttons.h"
#include "buzzer.h"
#include "adc.h"
#include "delay.h"
#include "states.h"
#include "timer.h"

void update_high_scores(uint16_t score);
void display_high_scores();

volatile GAMESTATES state;

volatile char name[20];
extern volatile uint8_t chars_received;

high_score_t highScores[5];
extern volatile SERIAL_STATE serial_state;

volatile uint32_t init_seed = SID;
extern volatile uint32_t seed;
extern volatile uint32_t temp_seed;
volatile uint16_t sequence_length;

int main(void)
{
    cli();
    uart_init();
    spi_init();
    buttons_init();
    buzzer_init();
    adc_init();
    delay_init();
    sei();    

    state = sequence_start;

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
            for (uint8_t i = 0; i < 5; i++)
            {
                if (sequence_length > highScores[i].HighScore)
                {
                    printf("Enter name: ");
                    serial_state = uart_GetName;
                    state = GetName;
                    break;
                }
            }
            if (state != GetName)
            {
                display_high_scores();
                state = sequence_start;
            }

            if (temp_seed)
            {
                seed = temp_seed;
                temp_seed = 0;
            }
            // printf("Failed\n");
            break;
        case GetName:
            if (elapsed_time > 5000)
            {
                serial_state = Command_Wait;
                state = SetName;
            }
            break;
        case SetName:
            name[chars_received] = '\0';
            update_high_scores(sequence_length);
            display_high_scores();

            chars_received = 0;
            sequence_length = 1;
            state = sequence_start;
            break;
        default:
            break;
        }
    }
}

void update_high_scores(uint16_t score)
{
    for (uint8_t i = 0; i < 5; i++)
    {
        if (score > highScores[i].HighScore)
        {
            for (uint8_t j = 4; j > i; j--)
                highScores[j] = highScores[j - 1];

            for (uint8_t j = 0; j < 20; j++)
                highScores[i].name[j] = name[j];
            highScores[i].HighScore = score;
            break;
        }
    }
}

void display_high_scores()
{
    for (uint8_t i = 0; i < 5; i++)
    {
        if (highScores[i].HighScore == 0)
            break;

        printf("%s %d\n", highScores[i].name, highScores[i].HighScore);
    }
}
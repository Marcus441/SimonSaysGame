#include "headers.h"
#include "uart.h"
#include "spi.h"
#include "buzzer.h"
#include "buttons.h"
#include "timer.h"
#include "delay.h"

typedef enum
{
    PB1,
    PB2,
    PB3,
    PB4,
    Success,
    Fail,
    UserInput
} GAMESTATES;

uint32_t seed = SID;

extern volatile uint8_t pb_debounced;
extern uint8_t digits[];
volatile uint8_t segs[] = {Spi_Off, Spi_Off};

uint8_t generate_step(uint32_t *state)
{

    uint8_t bit = *state & 1;
    *state >>= 1;

    if (bit == 1)
    {
        *state ^= MASK;
    }
    return *state & 0b11;
}

void display_score(uint16_t score)
{
    segs[0] = Spi_Off;
    if (score > 9)
        segs[0] = digits[score % 100 / 10];
    segs[1] = digits[score % 10];
}

bool runSequence(uint16_t sequenceLength)
{
    uint32_t lfsr_state = seed;
    uint8_t step = generate_step(&lfsr_state);
    uint16_t count = 0;

    GAMESTATES pb = UserInput;

    uint8_t pb_sample = 0xFF;
    uint8_t pb_sample_r = 0xFF;
    uint8_t pb_changed, pb_rising, pb_falling;

    allow_updating_playback_delay = true;
    while (1)
    {
        pb_sample_r = pb_sample;
        pb_sample = pb_debounced;
        pb_changed = pb_sample ^ pb_sample_r;

        pb_falling = pb_changed & pb_sample_r;
        pb_rising = pb_changed & pb_sample;

        switch (pb)
        {
        case UserInput:

            elapsed_time = 0;
            allow_updating_playback_delay = false;

            tone_stop();

            segs[0] = Spi_Off;
            segs[1] = Spi_Off;

            if (pb_falling & PIN4_bm)
            {
                pb = PB1;
                // printf("PB1 Pressed\n");
            }
            else if (pb_falling & PIN5_bm)
            {
                pb = PB2;
                // printf("PB2 Pressed\n");
            }
            else if (pb_falling & PIN6_bm)
            {
                pb = PB3;
                // printf("PB3 Pressed\n");
            }
            else if (pb_falling & PIN7_bm)
            {
                pb = PB4;
                // printf("PB4 Pressed\n");
            }
            break;
        case PB1:
            play_tone(0);
            segs[0] = SegLeft;
            if (pb_rising & PIN4_bm)
            {
                tone_stop();
                if (step == 0)
                {
                    pb = Success;
                    // printf("PB1 Correct\n");
                }
                else
                {
                    pb = Fail;
                    // printf("PB1 Incorrect\n");
                }
            }
            break;
        case PB2:
            play_tone(1);
            segs[0] = SegRight;
            if (pb_rising & PIN5_bm)
            {
                tone_stop();
                if (step == 1)
                {
                    pb = Success;
                    // printf("PB2 Correct\n");
                }
                else
                {
                    pb = Fail;
                    // printf("PB2 Incorrect\n");
                }
            }
            break;
        case PB3:
            play_tone(2);
            segs[1] = SegLeft;
            if (pb_rising & PIN6_bm)
            {
                tone_stop();
                if (step == 2)
                {
                    pb = Success;
                    // printf("PB3 Correct\n");
                }
                else
                {
                    pb = Fail;
                    // printf("PB3 Incorrect\n");
                }
            }
            break;
        case PB4:
            play_tone(3);
            segs[1] = SegRight;
            if (pb_rising & PIN7_bm)
            {
                tone_stop();
                if (step == 3)
                {
                    pb = Success;
                    // printf("PB4 Correct\n");
                }
                else
                {
                    pb = Fail;
                    // printf("PB4 Incorrect\n");
                }
            }
            break;
        case Success:
            count++;
            if (sequenceLength == count)
            {
                printf("SUCCESS\n");
                printf("%d\n", sequenceLength);

                display_score(sequenceLength);
                delay(false);
                segs[0] = Spi_On;
                segs[1] = Spi_On;
                delay(false);
                segs[0] = Spi_Off;
                segs[1] = Spi_Off;
                // printf("success state\n");
                return true;
            }
            else
            {
                step = generate_step(&lfsr_state);
                pb = UserInput;
                // printf("moving into delay state\n");
                break;
            }
        case Fail:
            printf("GAME OVER\n");
            printf("%d\n", sequenceLength);

            display_score(sequenceLength - 1);
            delay(false);
            segs[0] = Spi_Fail;
            segs[1] = Spi_Fail;
            delay(false);
            segs[0] = Spi_Off;
            segs[1] = Spi_Off;

            count++;
            for (; count < sequenceLength; count++)
                generate_step(&lfsr_state);
            seed = lfsr_state;

            return false;

        default:
            pb = UserInput;
            break;
        }
    }
    return true;
}

void generate_sequence(uint16_t sequenceLength)
{
    uint32_t lfsr_state = seed;
    for (uint16_t i = 0; i < sequenceLength; i++)
    {
        uint8_t step = generate_step(&lfsr_state);
        play_tone(step);

        switch (step)
        {
        case 0:
            segs[0] = SEG1;
            break;
        case 1:
            segs[0] = SEG2;
            break;
        case 2:
            segs[1] = SEG3;
            break;
        case 3:
            segs[1] = SEG4;
            break;
        }

        delay(true);

        tone_stop();
        segs[0] = Spi_Off;
        segs[1] = Spi_Off;
        delay(true);
    }
}

#include "headers.h"
#include "uart.h"
#include "spi.h"
#include "buzzer.h"
#include "buttons.h"
#include "timer.h"
#include "delay.h"
#include "states.h"

extern volatile uint8_t pb_debounced;
extern uint8_t digits[];

volatile uint32_t seed = SID;
volatile bool pb_released = false;
volatile uint8_t segs[] = {Spi_Off, Spi_Off};
volatile GAMESTATES pb = Paused;

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
    pb = UserInput;
    uint32_t lfsr_state = seed;
    uint8_t step = generate_step(&lfsr_state);
    uint16_t count = 0;

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
        case Paused:
            pb_released = false;
            break;
        case UserInput:
            pb_released = false;
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
            allow_updating_playback_delay = false;
        case PB1:
            play_tone(0);
            segs[0] = SegLeft;
            if (!pb_released)
            {
                if (pb_rising & PIN4_bm)
                {
                    pb_released = true;
                }
            }
            else if (elapsed_time >= playback_time)
            {
                tone_stop();
                // allow_updating_playback_delay = true;
                if (step == 0)
                {
                    pb = Success;
                }
                else
                {
                    pb = Fail;
                }
            }

            break;
        case PB2:
            play_tone(1);
            segs[0] = SegRight;
            if (!pb_released)
            {
                if (pb_rising & PIN5_bm)
                {
                    pb_released = true;
                }
            }
            else if (elapsed_time >= playback_time)
            {
                tone_stop();
                // allow_updating_playback_delay = true;
                if (step == 1)
                {
                    pb = Success;
                }
                else
                {
                    pb = Fail;
                }
            }

            break;
        case PB3:
            play_tone(2);
            segs[1] = SegLeft;
            if (!pb_released)
            {
                if (pb_rising & PIN6_bm)
                {
                    pb_released = true;
                }
            }
            else if (elapsed_time >= playback_time)
            {
                tone_stop();
                // allow_updating_playback_delay = true;
                if (step == 2)
                {
                    pb = Success;
                }
                else
                {
                    pb = Fail;
                }
            }

            break;
        case PB4:
            play_tone(3);
            segs[1] = SegRight;
            if (!pb_released)
            {
                if (pb_rising & PIN7_bm)
                {
                    pb_released = true;
                }
            }
            else if (elapsed_time >= playback_time)
            {
                tone_stop();
                // allow_updating_playback_delay = true;
                if (step == 3)
                {
                    pb = Success;
                }
                else
                {
                    pb = Fail;
                }
            }

            break;
        case Success:
            allow_updating_playback_delay = true;
            count++;
            if (sequenceLength == count)
            {
                // printf("SUCCESS\n");
                // printf("%d\n", sequenceLength);

                display_score(sequenceLength);
                delay(false);
                segs[0] = Spi_On;
                segs[1] = Spi_On;
                delay(false);
                segs[0] = Spi_Off;
                segs[1] = Spi_Off;
                // printf("success state\n");
                pb = Paused;
                return true;
            }
            else
            {
                step = generate_step(&lfsr_state);
                pb = UserInput;
                // printf("moving into UserInput\n");
            }
            break;
        case Fail:
            allow_updating_playback_delay = true;
            // printf("GAME OVER\n");
            // printf("%d\n", sequenceLength);

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
            pb = Paused;

            return false;
        case Reset:
            tone_stop();

            segs[0] = Spi_Off;
            segs[1] = Spi_Off;
            
            pb = Paused;
            return false;
            break;
        default:
            pb = Paused;
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

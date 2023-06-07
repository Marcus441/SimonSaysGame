#include "headers.h"
#include "sequence.h"

#include "uart.h"
#include "spi.h"
#include "buzzer.h"
#include "buttons.h"
#include "timer.h"
#include "delay.h"
#include "states.h"

volatile uint32_t seed = SID;                 // declare seed variable equal to Student ID
volatile uint32_t temp_seed;                  // declare temp seed variable
volatile uint32_t init_seed = SID;            // declare initial seed variable equal to Student ID
volatile uint8_t segs[] = {Spi_Off, Spi_Off}; // declare segs array starting with both segments off
volatile GAMESTATES pb = Paused;              // declare pushbutton variable

volatile bool uart_control = false; // declare uart control variable
volatile bool pb_released = false;  // declare pushbutton released variable

uint8_t generate_step(uint32_t *state) // generate step function
{

    uint8_t bit = *state & 1; // get the LSB
    *state >>= 1;             // shift the register right by 1

    if (bit == 1) // if the LSB is 1
    {
        *state ^= MASK; // XOR the register with the mask
    }
    return *state & 0b11; // return the last 2 bits
}

void display_score(uint16_t score)
{
    if (score > 9) // if score is greater than 9
    {
        uint8_t mod = score % 100; // get the remainder of score divided by 100
        uint8_t quotient = 0;      // declare quotient variable

        while (mod > 9) // while mod is greater than 9
        {
            mod -= 10;  // subtract 10 from mod
            quotient++; // increment quotient
        }
        segs[0] = digits[quotient]; // set segs[0] to quotient
    }
    else // if score is less than 10
    {
        segs[0] = Spi_Off; // set left hand seg to off
    }

    segs[1] = digits[score % 10]; // set right hand seg to remainder of score divided by 10
}

bool runSequence(uint16_t sequenceLength)
{
    pb = UserInput;                            // set pushbutton to user input
    uint32_t lfsr_state = seed;                // declare lfsr_state variable equal to seed
    uint8_t step = generate_step(&lfsr_state); // declare step variable equal to generate step function with lfsr_state as parameter
    uint16_t count = 0;                        // declare count variable for scoring

    uint8_t pb_sample = 0xFF;                  // declare pushbutton sample variable
    uint8_t pb_sample_r = 0xFF;                // declare pushbutton sample_r variable
    uint8_t pb_changed, pb_rising, pb_falling; // declare pushbutton changed, rising, and falling variables

    while (1)
    {
        pb_sample_r = pb_sample;              // set pushbutton sample_r to pushbutton sample
        pb_sample = pb_debounced;             // set pushbutton sample to debounced pushbutton
        pb_changed = pb_sample ^ pb_sample_r; // set pushbutton changed to pushbutton sample XOR pushbutton sample_r

        pb_falling = pb_changed & pb_sample_r; // set pushbutton falling to pushbutton changed AND pushbutton sample_r
        pb_rising = pb_changed & pb_sample;    // set pushbutton rising to pushbutton changed AND pushbutton sample
        allow_updating_playback_delay = true;  // allow updating playback delay

        switch (pb) // begin switch statement for pushbutton state machine
        {
        case Paused:              // if paused
            segs[0] = Spi_Off;    // set left hand seg to off
            segs[1] = Spi_Off;    // set right hand seg to off
            uart_control = false; // set uart control to false
            pb_released = false;  // set pushbutton released to false
            break;
        case UserInput:                            // if user input
            elapsed_time = 0;                      // set elapsed time to 0
            allow_updating_playback_delay = false; // disallow updating playback delay
            pb_released = false;                   // set pushbutton released to false

            tone_stop(); // stop tones

            segs[0] = Spi_Off; // set left hand seg to off
            segs[1] = Spi_Off; // set right hand seg to off

            if (pb_falling & PIN4_bm)      // if pushbutton falling AND pin 4
                pb = PB1;                  // set pushbutton to PB1
            else if (pb_falling & PIN5_bm) // if pushbutton falling AND pin 5
                pb = PB2;                  // set pushbutton to PB2
            else if (pb_falling & PIN6_bm) // if pushbutton falling AND pin 6
                pb = PB3;                  // set pushbutton to PB3
            else if (pb_falling & PIN7_bm) // if pushbutton falling AND pin 7
                pb = PB4;                  // set pushbutton to PB4

            break; // break
        case PB1:
            play_tone(0);      // play tone 0
            segs[0] = SegLeft; // set left hand seg to left

            if ((uart_control == false) & (!pb_released)) // if pressed by user
            {
                if ((pb_rising & PIN4_bm)) // if rising edge (released)
                {
                    pb_released = true; // pb released = true
                }
            }
            else if ((elapsed_time >= playback_time)) // if elapsed time is greater than playback time
            {

                tone_stop();                          // stop tones
                allow_updating_playback_delay = true; // allow updating playback delay
                pb = step == 0 ? Success : Fail;      // if step is correct, set pushbutton to success, else set pushbutton to fail
                uart_control = false;                 // set uart control to false
            }
            break;
        case PB2:
            play_tone(1);
            segs[0] = SegRight;
            if ((uart_control == false) & (!pb_released)) // if pressed
            {
                if ((pb_rising & PIN5_bm)) // if rising edge (released)
                {
                    pb_released = true; // pb released = true
                }
            }
            else if ((elapsed_time >= playback_time))
            {

                tone_stop();
                allow_updating_playback_delay = true;
                pb = step == 1 ? Success : Fail;
                uart_control = false;
            }

            break;
        case PB3:
            play_tone(2);
            segs[1] = SegLeft;
            if ((uart_control == false) & (!pb_released)) // if pressed
            {
                if ((pb_rising & PIN6_bm)) // if rising edge (released)
                {
                    pb_released = true; // pb released = true
                }
            }
            else if ((elapsed_time >= playback_time))
            {

                tone_stop();
                allow_updating_playback_delay = true;
                pb = step == 2 ? Success : Fail;
                uart_control = false;
            }

            break;
        case PB4:
            play_tone(3);
            segs[1] = SegRight;
            if ((uart_control == false) & (!pb_released)) // if pressed
            {
                if ((pb_rising & PIN7_bm)) // if rising edge (released)
                {
                    pb_released = true; // pb released = true
                }
            }
            else if ((elapsed_time >= playback_time))
            {

                tone_stop();
                allow_updating_playback_delay = true;
                pb = step == 3 ? Success : Fail;
                uart_control = false;
            }
            break;
        case Success:
            allow_updating_playback_delay = true; // allow updating playback delay
            count++;                              // increment count
            if (sequenceLength == count)          // if sequence is completed
            {
                printf("SUCCESS\n"); // print success
                printf("%d\n", sequenceLength);

                // display success pattern for duration of playback delay
                segs[0] = Spi_On;
                segs[1] = Spi_On;
                delay(false); // delay not div 2

                display_score(sequenceLength); // display score for half the dration of playback delay
                delay(true);                   // delay div 2

                // segments off for half the playback delay
                segs[0] = Spi_Off;
                segs[1] = Spi_Off;
                delay(true); // delay div 2

                pb = Paused; // set pushbutton to paused
                return true; // return true for bool outcome in main
            }
            else
            {
                step = generate_step(&lfsr_state); // generate step to check against next pb press
                pb = UserInput;                    // set pushbutton to user input to check next pb press
            }
            break;
        case Fail:
            allow_updating_playback_delay = true; // allow updating playback delay
            printf("GAME OVER\n");                // print game over
            printf("%d\n", sequenceLength);       // print score

            // display fail pattern for duration of playback delay
            segs[0] = Spi_Fail;
            segs[1] = Spi_Fail;
            delay(false); // delay not div 2

            display_score(sequenceLength); // display score for half the dration of playback delay
            delay(true);                   // delay div 2

            // segments off for half the playback delay
            segs[0] = Spi_Off;
            segs[1] = Spi_Off;
            delay(true); // delay div 2

            count++;                                // increment count
            for (; count < sequenceLength; count++) // for the rest of the sequence
                generate_step(&lfsr_state);         // generate step so that the sequence is played out from where the user failed
            seed = lfsr_state;                      // set seed to lfsr state
            pb = Paused;                            // set pushbutton to paused

            return false; // return false for bool outcome in main
        case Reset:
            tone_stop();  // stop tones
            tone_reset(); // reset tones

            // turn of segments
            segs[0] = Spi_Off;
            segs[1] = Spi_Off;

            pb = Paused;  // set pushbutton state to paused
            return false; // return false for bool outcome in main
            break;
        default:
            pb = Paused; // set state to paused
            break;
        } // end switch
    }     // end while

    return true; // return true for bool outcome in main
}

void generate_sequence(uint16_t sequenceLength)
{
    uint32_t lfsr_state = seed;                   // set lfsr state to seed
    for (uint16_t i = 0; i < sequenceLength; i++) // for the length of the sequence
    {
        uint8_t step = generate_step(&lfsr_state); // generate step

        switch (step) // switch on step
        {
        case 0: // play tone and light segment for corresponding pushbutton
            play_tone(0);
            segs[0] = SegLeft;
            break;
        case 1: // play tone and light segment for corresponding pushbutton
            play_tone(1);
            segs[0] = SegRight;
            break;
        case 2: // play tone and light segment for corresponding pushbutton
            play_tone(2);
            segs[1] = SegLeft;
            break;
        case 3: // play tone and light segment for corresponding pushbutton
            play_tone(3);
            segs[1] = SegRight;
            break;
        }
        // display segment for half the playback delay
        delay(true);

        // turn off segment for half the playback delay
        tone_stop();
        segs[0] = Spi_Off;
        segs[1] = Spi_Off;
        delay(true);
    }
}

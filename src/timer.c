#include "timer.h"

#include "sequence.h"
#include "headers.h"
#include "buttons.h"
#include "headers.h"
#include "spi.h"
#include "delay.h"

volatile uint16_t elapsed_time = 0;                  // declare elapsed time variable
volatile uint16_t playback_time;                     // declare playback time variable
volatile uint16_t new_playback_time;                 // declare new playback time variable
volatile bool allow_updating_playback_delay = false; // declare allow updating playback delay variable

// Timer ISR; samples pushbuttons
ISR(TCB0_INT_vect)
{

    pb_debounce(); // sample pushbuttons

    static int digit = 0; // digit counter
    if (digit)            // if digit is 1
    {
        spi_write(segs[0] | (0x01 << 7)); // write to spi
    }
    else
    {
        spi_write(segs[1]); // write to spi
    }
    digit = !digit; // toggle digit

    TCB0.INTFLAGS = TCB_CAPT_bm; // Acknowledge interrupt
}

ISR(TCB1_INT_vect)
{
    elapsed_time++;                    // increment elapsed time
    if (allow_updating_playback_delay) // if allow updating playback delay is true
    {
        playback_time = get_duration();        // get duration
        allow_updating_playback_delay = false; // set allow updating playback delay to false
    }
    TCB1.INTFLAGS = TCB_CAPT_bm; // Acknowledge interrupt
}
#include "timer.h"

#include "sequence.h"
#include "headers.h"
#include "buttons.h"
#include "headers.h"
#include "spi.h"
#include "delay.h"

volatile uint16_t elapsed_time = 0;
volatile uint16_t playback_time;
volatile uint16_t new_playback_time;
volatile bool allow_updating_playback_delay = false;


// Timer ISR; samples pushbuttons
ISR(TCB0_INT_vect)
{

    pb_debounce();

    static int digit = 0;
    if (digit)
    {
        spi_write(segs[0] | (0x01 << 7));
    }
    else
    {
        spi_write(segs[1]);
    }
    digit = !digit;

    TCB0.INTFLAGS = TCB_CAPT_bm; // Acknowledge interrupt
}

ISR(TCB1_INT_vect)
{
    elapsed_time++;
    if (allow_updating_playback_delay)
    {
        playback_time = get_duration();
        allow_updating_playback_delay = false;
    }
    // new_time =
    TCB1.INTFLAGS = TCB_CAPT_bm;
}
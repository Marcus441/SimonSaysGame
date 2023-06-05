#include "buttons.h"
#include "timer.h"

#include "spi.h"
#include "delay.h"

volatile uint16_t elapsed_time = 0;
volatile uint16_t playback_time = 250;
volatile uint16_t new_playback_time = 250;
volatile bool allow_updating_playback_delay = false;

extern volatile uint8_t pb_debounced;
extern uint8_t segs[];
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
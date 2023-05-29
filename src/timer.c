#include "buttons.h"
#include "spi.h"



extern volatile uint8_t pb_debounced;
extern uint8_t segs[];
// Timer ISR; samples pushbuttons
ISR(TCB0_INT_vect)
{

    static uint8_t vcount0 = 0, vcount1 = 0; // vertical counter bits

    uint8_t pb_sample = PORTA.IN;

    uint8_t pb_changed = (pb_sample ^ pb_debounced);

    // vertical counter update
    vcount1 = (vcount1 ^ vcount0) & pb_changed; // vcount1 (bit 1 of vertical counter)
    vcount0 = ~vcount0 & pb_changed;            // vcount0 (bit 0 of vertical counter)

    pb_debounced ^= (vcount0 & vcount1); // toggle pb_debounced

    static int digit = 0;
    if (digit) {
        spi_write(segs[0] | (0x01 << 7));
    } else {
        spi_write(segs[1]);
    }
    digit = !digit;
    TCB0.INTFLAGS = TCB_CAPT_bm;         // Acknowledge interrupt

}


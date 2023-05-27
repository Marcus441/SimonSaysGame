#include "headers.h"


volatile uint8_t pb_state = 0xFF;

void buttons_init() {
    // Enable pull-up resistors for PBs
    PORTA.PIN4CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN5CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN6CTRL = PORT_PULLUPEN_bm;
    PORTA.PIN7CTRL = PORT_PULLUPEN_bm;

    // Configure timer for PB sampling
    cli();
    TCB0.CTRLB = TCB_CNTMODE_INT_gc;    // Configure TCB0 in periodic interrupt mode
    TCB0.CCMP = 33333;                  // Set interval for 1ms (3333 clocks @ 3.3 MHz)
    TCB0.INTCTRL = TCB_CAPT_bm;         // CAPT interrupt enable
    TCB0.CTRLA = TCB_ENABLE_bm;        // Enable
    sei(); 
}

// Timer ISR; samples pushbuttons
ISR(TCB0_INT_vect) {
    
    static uint8_t count0 = 0;
    static uint8_t count1 = 0;
    uint8_t pb_sample = PORTA.IN; // Sample PB state
    uint8_t pb_changed = pb_sample ^ pb_state; // Detect change to PB state
    // Increment if PB state changed, reset otherwise
    count1 = (count1 ^ count0) & pb_changed;
    count0 = ~count0 & pb_changed;
    // Update PB state immediately on falling edge or if PB high for three samples
    pb_state ^= (count1 & count0) | (pb_changed & pb_state); //
    uint8_t pb_state_r = pb_state;

    uint8_t pb_falling = pb_changed & ~pb_state;
    
    TCB0.INTFLAGS = TCB_CAPT_bm;        // Acknowledge interrupt
}
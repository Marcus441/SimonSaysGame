#include "headers.h"

extern volatile int8_t octave;    // Octave offset
extern volatile uint32_t tones[]; // Array of tones

void buzzer_init(void);        // initialize buzzer
void play_tone(uint8_t Index); // play tone
void tone_stop(void);          // stop tone
void inc(void);                // increment octave
void dec(void);                // decrement octave
void tone_reset(void);         // reset octave

#include "headers.h"

extern volatile int8_t octave;
extern volatile uint32_t tones[];

void buzzer_init(void);
void play_tone(uint8_t Index);
void tone_stop(void);
void inc(void);
void dec(void);
void tone_reset(void);

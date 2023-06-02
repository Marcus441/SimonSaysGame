#include "headers.h"

volatile int8_t octave = 0;
volatile uint32_t tones[] = {
    T1, T2, T3, T4};

void buzzer_init(void)
{
    cli();
    PORTB.DIRSET |= PIN0_bm;
    TCA0.SINGLE.CTRLA |= TCA_SINGLE_CLKSEL_DIV1_gc;                              // p274  | TCA_SINGLE_ENABLE_bm
    TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_SINGLESLOPE_gc | TCA_SINGLE_CMP0EN_bm; // p210
    TCA0.SINGLE.CTRLA = TCA_SINGLE_ENABLE_bm;
    sei(); // Enable interrupts
}

void play_tone(uint8_t Index)
{
    TCA0.SINGLE.PERBUF = tones[Index]; // 2120 Hz, Period: 3,333,333/2120
    TCA0.SINGLE.CMP0BUF = TCA0.SINGLE.PERBUF >> 1;
}
void tone_stop(void)
{
    TCA0.SINGLE.CMP0BUF = 0;
}

void inc(void)
{
    for (size_t i = 0; i <= 3; i++)
    {
        tones[i] >>= 1;
    }
}

void dec(void)
{
    for (size_t i = 0; i <= 3; i++)
    {
        tones[i] <<= 1;
    }
}

void tone_reset(void)
{
    octave = 0;
    tones[0] = T1;
    tones[1] = T2;
    tones[2] = T3;
    tones[3] = T4;
}
#include "headers.h"
#include "buzzer.h"

volatile int8_t octave = 0;
volatile uint32_t tones[] = {
    T1, T2, T3, T4};

void buzzer_init(void)
{
    PORTB.DIRSET |= PIN0_bm;                                                     // Enable PB0 as output (TCA0 WO0)
    TCA0.SINGLE.CTRLA |= TCA_SINGLE_CLKSEL_DIV1_gc;                              // Set clock source to CLK_PER
    TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_SINGLESLOPE_gc | TCA_SINGLE_CMP0EN_bm; // Set waveform generation mode to single slope PWM
    TCA0.SINGLE.CTRLA = TCA_SINGLE_ENABLE_bm;                                    // Enable TCA0
}

void play_tone(uint8_t Index)
{
    TCA0.SINGLE.PERBUF = tones[Index];             // Set period
    TCA0.SINGLE.CMP0BUF = TCA0.SINGLE.PERBUF >> 1; // Set duty cycle
}
void tone_stop(void)
{
    TCA0.SINGLE.CMP0BUF = 0; // Set duty cycle to 0 to disable tone
}

void inc(void)
{
    for (size_t i = 0; i <= 3; i++) // Shift all tones one octave up
    {
        tones[i] >>= 1; // Divide by 2
    }
}

void dec(void)
{
    for (size_t i = 0; i <= 3; i++) // Shift all tones one octave down
    {
        tones[i] <<= 1; // Multiply by 2
    }
}

void tone_reset(void)
{
    octave = 0; // Reset octave
    // reset tones
    tones[0] = T1;
    tones[1] = T2;
    tones[2] = T3;
    tones[3] = T4;
}
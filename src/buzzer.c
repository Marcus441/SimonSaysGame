#include "headers.h"
int tones [] = {
    496*(2^(-5/12)), 496*(2^(-8/12)), 496, 496*(2^(-17/12))
};

void buzzer_init(void) {
    cli(); // Disables interrupts 
    PORTB.DIRSET = PIN0_bm; // enable buzzer output
    TCB0.CTRLB = TCB_CNTMODE_INT_gc; // periodic interupt mode
    TCB0.CCMP = 3333; //1ms
    TCB0.INTCTRL = TCB_CAPT_bm; // Enable CAPT interrupt source
    TCB0.CTRLA = TCB_CLKSEL_DIV1_gc | TCB_ENABLE_bm; // no prescaler
    TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_SINGLESLOPE_gc | TCA_SINGLE_CMP0EN_bm; // select waveform

    sei(); // Enable interrupts 
}

void play_tone(uint8_t Index){
    TCA0.SINGLE.PERBUF = tones[Index]; // 2120 Hz, Period: 3,333,333/2120
    TCA0.SINGLE.CMP0BUF = TCA0.SINGLE.PERBUF >> 1;
    TCA0.SINGLE.CTRLB |= TCA_SINGLE_CMP0EN_bm;
}
void tone_stop(void){
    TCA0.SINGLE.CTRLB |= TCA_SINGLE_CMP0EN_bm;
    TCA0.SINGLE.CTRLB &= ~TCA_SINGLE_CMP0EN_bm;
}

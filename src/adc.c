#include "headers.h"
#include "adc.h"

void adc_init(void)
{
    ADC0.CTRLA = ADC_ENABLE_bm;                                      // Enable ADC
    ADC0.CTRLB = ADC_PRESC_DIV2_gc;                                  // set ADC prescaler to 2
    ADC0.CTRLC = (4 << ADC_TIMEBASE_gp) | ADC_REFSEL_VDD_gc;         // set ADC timebase to 4 and reference to VDD
    ADC0.CTRLE = 64;                                                 // set ADC sampling time to 64
    ADC0.CTRLF = ADC_FREERUN_bm | ADC_LEFTADJ_bm;                    // set ADC to freerun and left adjust
    ADC0.MUXPOS = ADC_MUXPOS_AIN2_gc;                                // set ADC to read from AIN2
    ADC0.COMMAND = ADC_MODE_SINGLE_8BIT_gc | ADC_START_IMMEDIATE_gc; // set ADC to single 8 bit mode and start conversion
}
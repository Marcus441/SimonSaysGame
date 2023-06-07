#include "headers.h"
#include "delay.h"

#include "timer.h"

#define ADC8bit 6.8359375  // 1750 / 256 (frequency range / resolution)
#define ADC_RESOLUTION 256 // 8-bit resolution
#define MIN_DURATION 250   // 250ms
#define MAX_DURATION 2000  // 2000ms

void delay_init(void)
{
    TCB1.CCMP = 3333;           // Set interval for 1ms (3333 clocks @ 3.3 MHz)
    TCB1.INTCTRL = TCB_CAPT_bm; // CAPT interrupt enable
    TCB1.CTRLA = TCB_ENABLE_bm; // enable TCB1
}

uint16_t get_duration(void)
{
    while (!(ADC0.INTFLAGS & ADC_RESRDY_bm)) // wait for ADC conversion to finish
        ;
    uint32_t result = ADC0.RESULT;                         // read ADC result
    uint16_t duration = (result * ADC8bit) + MIN_DURATION; // convert ADC result to duration
    return duration;                                       // return duration
}

void delay(bool div)
{
    uint32_t ms = get_duration(); // get duration
    if (div)                      // if div is true
        ms >>= 1;                 // divide duration by 2
    elapsed_time = 0;             // reset elapsed time
    while (elapsed_time < ms)     // wait for elapsed time to reach duration
        ;
}

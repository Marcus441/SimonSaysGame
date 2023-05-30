#include "headers.h"
#include "timer.h"

void delay_init(void) 
{
    cli();
    TCB1.CCMP = 3333;
    TCB1.INTCTRL = TCB_CAPT_bm;
    TCB1.CTRLA = TCB_ENABLE_bm;
    sei();
}

uint16_t get_duration(void)
{
    uint16_t result = ADC0.RESULT;
    uint16_t duration = result / 255.0 * 1750 + 250;
    return duration;
}

void delay_ms(uint16_t ms)
{
    elapsed_time = 0;
    while (elapsed_time < ms)
        ;
}

void delay(bool div)
{
    uint16_t ms = get_duration();
    if(div) ms >>= 1;
    delay_ms(ms);
}

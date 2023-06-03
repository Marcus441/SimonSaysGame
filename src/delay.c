#include "headers.h"
#include "delay.h"

#include "timer.h"

void delay_init(void)
{
    TCB1.CCMP = 3333;
    TCB1.INTCTRL = TCB_CAPT_bm;
    TCB1.CTRLA = TCB_ENABLE_bm;
    // get_duration();
}

uint16_t get_duration(void)
{
    uint32_t result = ADC0.RESULT;
    uint16_t duration = (result * 6.84) + ((int)(result * 6.84) >> 8) + 250;;
    // uint16_t duration = (result * 7) + ((result * 7)  >> 8);
    return duration;
}

void delay(bool div)
{
    uint32_t ms = playback_time;
    if (div)
        ms >>= 1;
    elapsed_time = 0;
    while (elapsed_time < ms)
        ;
}

#include "headers.h"
#include "delay.h"

#include "timer.h"
#define ADC8bit 6.8359375 // 1750/256
#define ADC_RESOLUTION 256
#define MIN_DURATION 250
#define MAX_DURATION 2000

void delay_init(void)
{
    TCB1.CCMP = 3333;
    TCB1.INTCTRL = TCB_CAPT_bm;
    TCB1.CTRLA = TCB_ENABLE_bm;
    get_duration();
}

uint16_t get_duration(void)
{
    uint16_t result = ADC0.RESULT;
    // uint16_t duration = (result * ADC8bit) + ((int16_t)(result * ADC8bit) >> 8) + 250;
    uint16_t duration = (result * 7) + MIN_DURATION;
    //printf("result: %d, duration: %d\n", result, duration);
    return duration;
}

void delay(bool div)
{
    uint32_t ms = get_duration();
    if (div)
        ms >>= 1;
    elapsed_time = 0;
    while (elapsed_time < ms)
        ;
}

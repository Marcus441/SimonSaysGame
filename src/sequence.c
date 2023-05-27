#include "headers.h"
#include "uart.h"
#include "spi.h"
#include "buzzer.h"
#include "buttons.h"

typedef enum 
{
    PB1,
    PB2,
    PB3,
    PB4,
    Success,
    Fail,
    Delay
} GAMESTATES;

uint32_t seed = SID;
uint8_t step;
extern volatile uint8_t pb_debounced;

uint8_t generate_step(uint32_t *state){
    
    uint8_t bit = *state & 1;
    *state >>= 1;

    if (bit == 1){
        *state ^= MASK;
    }
    return *state & 0b11;
}

uint8_t runSequence(uint16_t sequenceLength){
    uint32_t lfsr_state = seed;
    step = generate_step(&lfsr_state);

    GAMESTATES pb= Delay;

    uint8_t pb_sample = 0xFF;
    uint8_t pb_sample_r = 0xFF;
    uint8_t pb_changed, pb_rising, pb_falling;

    while (1)
    {
        pb_sample_r = pb_sample;
        pb_sample = pb_debounced;
        pb_changed = pb_sample ^ pb_sample_r;

        pb_falling = pb_changed & pb_sample_r;
        pb_rising = pb_changed & pb_sample;

        switch(pb){
            case Delay:
                spi_write(0xFF);
                if (pb_falling & PIN4_bm)
                {
                    pb = PB1;
                    //uart_puts("pb1");
                } 
                else if (pb_falling & PIN5_bm)
                {
                    pb = PB2;
                    //uart_puts("pb2");                    
                }
                else if (pb_falling & PIN6_bm)
                {
                    pb = PB3;
                    //uart_puts("pb3");
                }  
                else if (pb_falling & PIN7_bm)
                {
                    pb = PB4;
                    //uart_puts("pb4");
                }
                break;
            case PB1:
                spi_write(segs[0]);
                if (pb_rising & PIN4_bm){
                    if(step == 0){
                        pb = Success;
                    }
                }
                break;
            case PB2:
                spi_write(segs[1]);
                if (pb_rising & PIN5_bm){
                    if(step == 1){
                        pb = Success;
                    }
                }
                break;
            case PB3:
                spi_write(segs[2]);
                if (pb_rising & PIN6_bm){
                    if(step == 2){
                        pb = Success;
                    }
                }
                break;
            case PB4:
                spi_write(segs[3]);
                if (pb_rising & PIN7_bm){
                    if(step == 3){
                        pb = Success;
                    }
                }
                break;
            case Success:
                sequenceLength--;
                if(sequenceLength == 0){
                    return 1;
                }else
                {
                    step = generate_step(&lfsr_state);
                    pb = Delay;
                }
            case Fail:

                seed = lfsr_state;
                return 0;

            default: 
            pb = Delay;
            
            break;
        }

    }
    return 1;
}

void generate_sequence(uint16_t sequenceLength){
    uint32_t lfsr_state = seed;
    for(uint16_t i=0; i <= sequenceLength; i++){
        step = generate_step(&lfsr_state);
        // if(i!=0){uart_puts("\n");}
        // uart_putc(step+48);
        //play_tone(step);
        _delay_ms(500);
        spi_write(segs[step]);
        _delay_ms(1000);
        spi_write(0xFF);
    }
    // tone_stop();
}
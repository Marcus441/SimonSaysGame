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
    uint8_t step = generate_step(&lfsr_state);
    uint16_t count=0;

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
                    play_tone(0);
                    println("PB1 Pressed");
                } 
                else if (pb_falling & PIN5_bm)
                {
                    pb = PB2;
                    play_tone(1);
                    println("PB2 Pressed");;                    
                }
                else if (pb_falling & PIN6_bm)
                {
                    pb = PB3;
                    play_tone(2);
                    println("PB3 Pressed");
                }  
                else if (pb_falling & PIN7_bm)
                {
                    pb = PB4;
                    play_tone(3);
                    println("PB4 Pressed");
                }
                break;
            case PB1:
                spi_write(segs[0]);
                if (pb_rising & PIN4_bm){
                    tone_stop();
                    if(step == 0){
                        pb = Success;
                        println("PB1 Correct");
                    } else {
                        pb = Fail;
                        println("PB1 Incorrect");
                    }
                }
                break;
            case PB2:
                spi_write(segs[1]);
                if (pb_rising & PIN5_bm){
                    tone_stop();
                    if(step == 1){
                        pb = Success;
                        println("PB2 Correct");
                    } else {
                        pb = Fail;
                        println("PB2 Incorrect");
                    }
                }
                break;
            case PB3:
                spi_write(segs[2]);
                if (pb_rising & PIN6_bm){
                    tone_stop();
                    if(step == 2){
                        pb = Success;
                        println("PB3 Correct");
                    } else {
                        pb = Fail;
                        println("PB3 Incorrect");
                    }
                }
                break;
            case PB4:
                spi_write(segs[3]);
                if (pb_rising & PIN7_bm){
                    tone_stop();
                    if(step == 3){
                        pb = Success;
                        println("PB4 Correct");
                    } else {
                        pb = Fail;
                        println("PB4 Incorrect");
                    }
                }
                break;
            case Success:
                if(sequenceLength == count){
                    spi_write(0x00);
                    _delay_ms(500);
                    spi_write(0xFF);
                    println("success state");
                    return 1;
                }else
                {
                    count++;
                    step = generate_step(&lfsr_state);
                    pb = Delay;
                    println("moving into delay state");
                    break;
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
        uint8_t step = generate_step(&lfsr_state);
        // if(i!=0){uart_puts("\n");}
        // uart_putc(step+48);
        // play_tone(step);
        _delay_ms(250);
        play_tone(step);
        spi_write(segs[step]);
        _delay_ms(500);
        spi_write(0xFF);
        tone_stop();
    }
}
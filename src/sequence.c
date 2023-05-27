#include "headers.h"
#include "uart.h"
#include "spi.h"
volatile uint8_t segs [] = {
    0xBE, 0xEB, 0x3E, 0x6B
};
uint32_t state = SID;
uint8_t generate_step(uint32_t *state){
    
    uint8_t bit = *state & 1;
    *state >>= 1;

    if (bit == 1){
        *state ^= MASK;
    }
    return *state & 0b11;
}

void runSequence(uint16_t sequenceLength){
    uint32_t state = SID;
    for (uint16_t i = 0; i <sequenceLength; i++){
        uint8_t step = generate_step(&state);
        while(1){
            if (VPORTA.IN & (PIN4_bm == step)){
                break;
            }
            if (VPORTA.IN & (PIN5_bm == step)){
                break;
            }
            if (VPORTA.IN & (PIN6_bm == step)){
                break;
            }
            if (VPORTA.IN & (PIN7_bm == step)){
                break;
            }
        }

    }
}

void generate_sequence(uint16_t sequenceLength){
    uint8_t step;
    for(uint16_t i=0; i <= sequenceLength; i++){
        step = generate_step(&state);
        // if(i!=0){uart_puts("\n");}
        // uart_putc(step+48);

        spi_write(segs[step]);
        _delay_ms(1000);
        spi_write(0xFF);
    }
    
}
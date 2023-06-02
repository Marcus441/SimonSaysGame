#include "headers.h"
#include "spi.h"


uint8_t digits[] = {
    0x08,0x6B,0x44,0x41,0x23,0x11,0x10,0x4B,0x00,0x01
};

void spi_init(void)
{
    cli();
    PORTMUX.SPIROUTEA = PORTMUX_SPI0_ALT1_gc; // SPI pins on PC0-3
    PORTC.DIRSET |= (PIN0_bm | PIN2_bm);      // SCK (PC0) and MOSI (PC2) output
    PORTA.OUTSET |= PIN1_bm;                  // DISP_LATCH initiall high
    PORTA.DIRSET |= PIN1_bm;

    SPI0.CTRLA = SPI_MASTER_bm;  // Master, /4 prescaler, MSB first
    SPI0.CTRLB = SPI_SSD_bm;     // Mode 0, client select disable, unbuffered
    SPI0.INTCTRL |= SPI_IE_bm;   // Interrupt enable
    SPI0.CTRLA |= SPI_ENABLE_bm; // Enable
    sei();
}

void spi_write(uint8_t data)
{
    SPI0.DATA = data; // Note DATA register used for both Tx and Rx
}

ISR(SPI0_INT_vect)
{
    // rising edge on DISP_LATCH
    PORTA.OUTCLR |= PIN1_bm;
    PORTA.OUTSET |= PIN1_bm;
    SPI0.INTFLAGS = SPI_IF_bm;
}

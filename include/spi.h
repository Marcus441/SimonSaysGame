#include "headers.h"

extern uint8_t digits[]; // Array of digits

void spi_init(void);          // initialize SPI
void spi_write(uint8_t data); // write data to SPI

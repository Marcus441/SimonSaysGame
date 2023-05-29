#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdio.h>
#include <util/delay.h>
#include <stdbool.h>

#define SID 0x11240296
#define MASK 0xE2023CAB

#define SEG1 0xBE
#define SEG2 0xEB
#define SEG3 0x3E
#define SEG4 0x6B

#define SEGS_EF 0b00111110
#define SEGS_BC 0b01101011
#define Spi_Fail 0b01110111
#define Spi_On 0x00
#define Spi_Off 0x7F

#define T1 8971
#define T2 10668
#define T3 6720
#define T4 17941
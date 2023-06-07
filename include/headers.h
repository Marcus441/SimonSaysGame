#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#define SID 0x11240296  // Student ID
#define MASK 0xE2023CAB // Mask

#define SegLeft 0x3E  // 0b00111110
#define SegRight 0x6B // 0b01101011
#define Spi_Fail 0x77 // 0b01110111
#define Spi_On 0x00   // 0b00000000
#define Spi_Off 0x7F  // 0b01111111

#define T1 8971  // E high converted to period
#define T2 10668 // C # high converted to period
#define T3 6720  // A high converted to period
#define T4 17941 // E low converted to period

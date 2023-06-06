#include "headers.h"

uint8_t generate_step(uint32_t *state);
void generate_sequence(uint16_t sequenceLength);
bool runSequence(uint16_t sequenceLength);
void display_score(uint16_t score);


extern volatile uint32_t temp_seed;
extern volatile uint32_t seed;
extern volatile uint32_t init_seed;
extern volatile uint8_t segs[];

extern volatile bool uart_control;
extern volatile bool pb_released;
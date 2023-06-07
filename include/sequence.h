#include "headers.h"

uint8_t generate_step(uint32_t *state);          // generate step
void generate_sequence(uint16_t sequenceLength); // generate sequence
bool runSequence(uint16_t sequenceLength);       // run sequence
void display_score(uint16_t score);              // display score

extern volatile uint32_t temp_seed; // declare temp seed variable
extern volatile uint32_t seed;      // declare seed variable
extern volatile uint32_t init_seed; // declare init seed variable
extern volatile uint8_t segs[];     // declare segs array

extern volatile bool uart_control; // declare uart control variable
extern volatile bool pb_released;  // declare pushbutton released variable
#include "headers.h"

extern volatile uint8_t pb_debounced; // debounced pushbutton state

void buttons_init(void); // initialize pushbuttons
void pb_debounce(void);  // debounce pushbuttons
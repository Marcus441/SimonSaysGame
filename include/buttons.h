#include "headers.h"

// extern volatile uint8_t pb;
extern volatile uint8_t pb_debounced;

void buttons_init(void);
void pb_debounce(void);
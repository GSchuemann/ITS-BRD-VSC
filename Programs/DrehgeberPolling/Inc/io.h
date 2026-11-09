#include <stdint.h>

#ifndef io_h
#define io_h
void read_buttons(void);
uint32_t get_buttons(void);
void clr_e_led(uint32_t leds);
void clr_d_led(uint32_t leds);

void set_e_led(uint32_t leds);
void set_d_led(uint32_t leds);

#endif
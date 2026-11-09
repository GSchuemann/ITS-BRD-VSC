#include "stm32f429xx.h"
#include <stdint.h>
#include "io.h"
#define  USED_LEDS 0b111 << 5
static uint32_t buttons;
void read_buttons() {
    buttons = GPIOF->IDR; }

uint32_t get_buttons() {
    return buttons;
}

void clr_e_led(uint32_t leds) {
    GPIOE -> BSRR = leds << 16;
}
void clr_d_led(uint32_t leds) {
    GPIOD -> BSRR = leds << 16;
}

void set_e_led(uint32_t leds) {
    GPIOE -> BSRR = leds;
} 


void set_d_led(uint32_t leds) {
    GPIOD -> BSRR = leds;
} 
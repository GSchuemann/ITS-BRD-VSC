#include "stm32f429xx.h"
#include <stdint.h>
#include "io.h"
static uint32_t buttons;
void read_buttons() {
    buttons = GPIOF->IDR; }

uint32_t get_buttons() {
    return buttons;
}

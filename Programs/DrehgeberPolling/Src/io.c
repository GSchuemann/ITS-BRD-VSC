/**
 ******************************************************************************
 * @file    io.c
 * @author  Xuan Hoang Duy Trinh, Georg Schümann
 * @brief   Lesen von Knöpfen und Setzen von LEDs
 ******************************************************************************
 */
/* Includes ------------------------------------------------------------------*/
#include "io.h"
#include "stm32f429xx.h"
#include <stdint.h>
#define USED_LEDS 0b111 << 5
#define OFF_OFFSET 16
static uint32_t buttons;
void read_buttons() { buttons = GPIOF->IDR; }

uint32_t get_buttons() { return buttons; }

void clr_e_led(uint32_t leds) { GPIOE->BSRR = leds << OFF_OFFSET; }
void clr_d_led(uint32_t leds) { GPIOD->BSRR = leds << OFF_OFFSET; }

void set_e_led(uint32_t leds) { GPIOE->BSRR = leds; }

void set_d_led(uint32_t leds) { GPIOD->BSRR = leds; }
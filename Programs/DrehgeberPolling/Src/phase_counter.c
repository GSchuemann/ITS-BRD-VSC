#include "phase_counter.h"
#include <stdint.h>
#include <stdio.h>
#include "timing.h"
#include "stdlib.h"
#include "io.h"
#define PHASE_SHIFT_CONST (0.3)
static uint32_t counter;
static double degree_counter;
static uint64_t timestamps[2];


void reset_degree() {degree_counter = 0;}
void add_degree() { 
    degree_counter += PHASE_SHIFT_CONST; 
    timestamps[counter++ & 1] = get_time();
    set_d_led(counter & 0xFF);
}
void sub_degree() { 
    degree_counter -= PHASE_SHIFT_CONST; 
    timestamps[counter++ & 1] = get_time();
    set_d_led(counter & 0xFF);

}
void print_degree() {
    printf("%f\r\n",degree_counter);
}
void print_speed() {
    printf("%f\n",(double) 1 /(((double)abs((int32_t)(timestamps[0] - timestamps[1])) / (double)TICKS_PER_SECOND )* 3));
}
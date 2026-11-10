#include "phase_counter.h"
#include <stdint.h>
#include <stdio.h>
#include "timing.h"
#include "io.h"
#define PHASE_SHIFT_CONST (0.3)
#define THIRD_OF_SECOND (3.333)
static uint32_t counter;
static double degree_counter;
static uint64_t last_timestamp = 0;
static uint32_t speed_diff = 0;

void reset_degree() {degree_counter = 0;}
void add_degree() { 
    degree_counter += PHASE_SHIFT_CONST; 
    speed_diff = get_time() -last_timestamp;
    last_timestamp = get_time();
    set_d_led(counter & 0xFF);
}
void sub_degree() { 
    degree_counter -= PHASE_SHIFT_CONST; 
    speed_diff = get_time() -last_timestamp;
    last_timestamp = get_time();
    set_d_led(counter & 0xFF);

}

double get_degree() {
    return degree_counter;
}
double get_speed() {
    return (double) 1 /((( (double) speed_diff / (double)TICKS_PER_SECOND )* THIRD_OF_SECOND));
}
void print_degree() {
    printf("%f\r\n",degree_counter);
}
void print_speed() {
    printf("%f\n",(double) 1 /((( (double) speed_diff / (double)TICKS_PER_SECOND )* THIRD_OF_SECOND)));
}
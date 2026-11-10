#include "phase_counter.h"
#include <stdint.h>
#include <stdio.h>
#include "timing.h"
#include "io.h"
#define PHASE_SHIFT_CONST (0.3)
#define THIRD_OF_SECOND (3.333)
#define EIGHT_BIT (0xFF)

static int32_t counter;
static double degree_counter;
static uint64_t last_timestamp = 0;
static uint32_t speed_diff = 0;

void reset_degree() {
    degree_counter = 0.0;
    counter = 0;
}
void add_degree() { 
    degree_counter += PHASE_SHIFT_CONST; 
    speed_diff = get_time() -last_timestamp;
    last_timestamp = get_time();
    set_d_led(++counter & EIGHT_BIT);
    printf("%d\r\n",counter);
}
void sub_degree() { 
    degree_counter -= PHASE_SHIFT_CONST; 
    speed_diff = get_time() - last_timestamp;
    last_timestamp = get_time();
    clr_d_led(~(--counter));
    // set_d_led(--counter & EIGHT_BIT);

}
void reset_velocity(void) {
    speed_diff = 0;
}
double get_degree() {
    return degree_counter;
}
double get_speed() {
    if(get_time() - last_timestamp > WINDOW_3S){
        return 0.00;
    }
    double fraction = (( (double) speed_diff / (double)TICKS_PER_SECOND) * THIRD_OF_SECOND);
    return fraction == 0? 0.0: (double) 1 /(fraction);
}
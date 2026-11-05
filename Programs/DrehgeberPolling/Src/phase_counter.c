#include <stdint.h>
#include <stdio.h>

#define PHASE_SHIFT_CONST (0.3)
static uint32_t phase_counter;
static double degree_counter;


void add_degree() { degree_counter += PHASE_SHIFT_CONST; }
void sub_degree() { degree_counter -= PHASE_SHIFT_CONST; }
void print_degree() {
    printf("%f\r\n",degree_counter);
}
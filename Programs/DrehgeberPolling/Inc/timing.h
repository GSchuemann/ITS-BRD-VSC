#ifndef TIMING_H
#define TIMING_H

#include <stdint.h>

void update_time(void);
uint64_t get_time(void);
int update_velocity(int32_t counter, uint64_t counter_time);
double get_velocity(void);
void reset_velocity(void);

#endif
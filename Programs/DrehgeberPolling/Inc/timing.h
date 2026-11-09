#ifndef TIMING_H
#define TIMING_H

#include <stdint.h>
#define TICKS_PER_SECOND 90000000U

void update_time(void);
uint64_t get_time(void);
int update_velocity(int32_t counter, uint64_t counter_time);
double get_velocity(void);
void reset_velocity(void);

#endif
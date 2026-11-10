#ifndef TIMING_H
#define TIMING_H

#include <stdint.h>
#define TICKS_PER_SECOND 90000000U
#define WINDOW_300MS (((TICKS_PER_SECOND / 1000) * 300))
#define WINDOW_3S (TICKS_PER_SECOND * 3ULL)
/*
 ****************************************************************************************
 *  @brief      Reads the timer and saves it
 * *
 *  @return     void
 ****************************************************************************************/
void update_time(void);
/*
 ****************************************************************************************
 *  @brief      Returns the saved time for the loop
 * *
 *  @return     time
 ****************************************************************************************/
uint64_t get_time(void);

#endif
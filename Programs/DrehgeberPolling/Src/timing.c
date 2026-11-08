#include "timing.h"
#include "stm32f4xx.h"

#define TIMER_CNT (TIM2->CNT)
#define TICKS_PER_SECOND 90000000ULL
#define WINDOW_300MS (TICKS_PER_SECOND * 3 / 10)
#define WINDOW_3S (TICKS_PER_SECOND * 3)
#define DEGREE_PER_PHASE 0.3

static uint32_t last_cnt = 0;
static uint64_t total_time = 0;

static int32_t ref_counter = 0;
static uint64_t ref_time = 0;
static uint64_t last_calc = 0;
static double velocity = 0.0;

void update_time(void) {
   uint32_t now = TIMER_CNT;
   total_time = total_time + (uint32_t)(now - last_cnt);
   last_cnt = now;
}

uint64_t get_time(void) {
   return total_time;
}

int update_velocity(int32_t counter, uint64_t counter_time) {
   uint64_t since_calc = total_time - last_calc;

   if (since_calc < WINDOW_300MS) {
      return 0;
   }

   if (counter != ref_counter) {
      double degrees = (double)(counter - ref_counter) * DEGREE_PER_PHASE;
      double seconds = (double)(counter_time - ref_time) / (double)TICKS_PER_SECOND;
      velocity = degrees / seconds;
      ref_counter = counter;
      ref_time = counter_time;
      last_calc = total_time;
      return 1;
   }

   if (since_calc >= WINDOW_3S) {
      velocity = 0.0;
      last_calc = total_time;
      return 1;
   }

   return 0;
}

double get_velocity(void) {
   return velocity;
}

void reset_velocity(void) {
   ref_counter = 0;
   ref_time = total_time;
   last_calc = total_time;
   velocity = 0.0;
}
#include "timing.h"
#include "stm32f4xx.h"

#define TIMER_CNT (TIM2->CNT)
#define DEGREE_PER_PHASE 0.3

static uint32_t last_cnt = 0;
static uint64_t total_time = 0;


void update_time(void) {
   uint32_t now = TIMER_CNT;
   total_time = total_time + (now - last_cnt);
   last_cnt = now;
}

uint64_t get_time(void) {
   return total_time;
}

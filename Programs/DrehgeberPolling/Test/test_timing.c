#include "stm32f4xx.h"
#include "timing.h"
#include <stdint.h>
#include <stdio.h>

TIM_TypeDef test_tim2;

static void set_time(uint64_t ticks) {
   test_tim2.CNT = (uint32_t)ticks;
   update_time();
}

int main(void) {
   uint64_t t;

   for (t = 0; t <= 10000000000ULL; t = t + 100000000ULL) {
      set_time(t);
   }
   printf("%s  Zeit stimmt nach mehreren Timer-Ueberlaeufen\n",
          get_time() == 10000000000ULL ? "OK    " : "FEHLER");
   return 0;
}
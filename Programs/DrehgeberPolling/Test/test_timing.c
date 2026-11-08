#include "stm32f4xx.h"
#include "timing.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>

#define MS 90000ULL

TIM_TypeDef test_tim2;

static void set_time(uint64_t ticks) {
   test_tim2.CNT = (uint32_t)ticks;
   update_time();
}

static void check(int ok, const char *text) {
   printf("%s  %s\n", ok ? "OK    " : "FEHLER", text);
}

int main(void) {
   uint64_t t;
   uint64_t base;
   int k;
   int first;

   for (t = 0; t <= 10000000000ULL; t = t + 100000000ULL) {
      set_time(t);
   }
   check(get_time() == 10000000000ULL, "Zeit stimmt nach mehreren Timer-Ueberlaeufen");

   base = get_time();
   reset_velocity();
   first = 0;
   for (k = 1; k <= 1000 && first == 0; k++) {
      set_time(base + k * MS);
      if (update_velocity(k, base + k * MS)) {
         first = k;
      }
   }
   check(first == 300, "erster Wert nach 300 ms");
   check(fabs(get_velocity() - 300.0) < 0.001, "Geschwindigkeit ist 300.0");

   first = 0;
   for (k = 301; k <= 5000 && first == 0; k++) {
      set_time(base + k * MS);
      if (update_velocity(300, base + 300 * MS)) {
         first = k;
      }
   }
   check(first == 3300, "Stillstand: neuer Wert erst nach 3 s");
   check(get_velocity() == 0.0, "Stillstand: Geschwindigkeit ist 0.0");

   base = get_time();
   reset_velocity();
   set_time(base + 350 * MS);
   update_velocity(300, base + 300 * MS);
   check(fabs(get_velocity() - 300.0) < 0.001, "Zeitstempel zaehlt, nicht der Aufrufzeitpunkt");

   return 0;
}
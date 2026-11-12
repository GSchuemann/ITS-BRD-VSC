/**
 ******************************************************************************
 * @file    phase_counter.c
 * @author  Xuan Hoang Duy Trinh, Georg Schümann
 * @brief   Zählen von Phasen und Berechnung des Winkels so wie der
 *Geschwindigkeit
 ******************************************************************************
 */
/* Includes ------------------------------------------------------------------*/
#include "phase_counter.h"
#include "io.h"
#include "timing.h"
#include <stdint.h>
#define PHASE_SHIFT_CONST (0.3)
#define THIRD_OF_SECOND (3.333)
#define EIGHT_BIT (0xFF)

static int32_t counter;
static double degree_counter;
static uint64_t last_timestamp = 0;
static uint32_t velocity_diff = 0;
static int direction = 0;

void reset_degree() {
   degree_counter = 0.0;
   counter = 0;
   direction = 0;
}
void add_degree() {
   degree_counter += PHASE_SHIFT_CONST;
   velocity_diff = get_time() - last_timestamp;
   last_timestamp = get_time();
   direction = 1;
   counter++;
   if(counter > 0) {
      set_d_led((uint32_t)counter & EIGHT_BIT);
   }
}
void sub_degree() {
   degree_counter -= PHASE_SHIFT_CONST;
   velocity_diff = get_time() - last_timestamp;
   last_timestamp = get_time();
   direction = -1;
   counter--;
   if(counter >= 0){
      clr_d_led(~(uint32_t)counter);
      set_d_led((uint32_t)counter & EIGHT_BIT);
   }
}
void reset_velocity(void) { velocity_diff = 0; }
double get_degree() { return degree_counter; }
double get_velocity() {
   if (get_time() - last_timestamp > WINDOW_3S) {
      return 0.00;
   }
   if (velocity_diff == 0) {
      return 0.00;
   }
   double fraction = (double)velocity_diff / (double)TICKS_PER_SECOND;
   return direction * PHASE_SHIFT_CONST / fraction;
}
/**
 ******************************************************************************
 * @file    main.c
 * @author  Franz Korf
 * @brief   Kleines Testprogramm fuer neu erstelle Fonts.
 ******************************************************************************
 */
/* Includes ------------------------------------------------------------------*/

#include "main.h"
#include "LCD_GUI.h"
#include "LCD_Touch.h"
#include "display.h"
#include "init.h"
#include "phase_counter.h"
#include "phases.h"
#include <stdint.h>
#include "io.h"
#include "stm32f4xx.h"
#include "timer.h"

#define COUNTER_LED_MASK 0xFF

static char state[4] = {'A','B','C','D'};


    int main(void) {
   initITSboard(); // Initialisierung des ITS Boards

   GUI_init(DEFAULT_BRIGHTNESS); // Initialisierung des LCD Boards mit Touch
   TP_Init(false); // Initialisierung des LCD Boards mit Touch
   initTimer();
   enum NextPhase current_phase = INIT;
   // Test in Endlosschleife
   while (1) {
      read_buttons();
      update_time();
      current_phase = phase_ops[current_phase]();

      if (current_phase == ERROR) {
         set_err_led(1);
      } else {
         set_err_led(0);
         set_counter_leds((uint8_t)(get_counter() & COUNTER_LED_MASK));
         set_direction_leds(get_counter());
         print_degree();
      }
   }
}

// EOF

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
#include <stdio.h>
#include "io.h"
#include "stm32f4xx.h"
#include "timer.h"
#include "timing.h"

#define COUNTER_LED_MASK 0xFF


   int main(void) {
   initITSboard(); // Initialisierung des ITS Boards

   GUI_init(DEFAULT_BRIGHTNESS); // Initialisierung des LCD Boards mit Touch
   TP_Init(false); // Initialisierung des LCD Boards mit Touch
   initTimer();
   init_display();
   enum NextPhase current_phase = INIT;
   // Test in Endlosschleife
   while (1) {
      read_buttons();
      update_time();
      current_phase = phase_ops[current_phase]();
   }
}
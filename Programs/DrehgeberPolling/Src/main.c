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
#include "init.h"
#include "phase_counter.h"
#include "phases.h"
#include <stdint.h>
#include "io.h"

static char state[4] = {'A','B','C','D'};


    int main(void) {
   initITSboard(); // Initialisierung des ITS Boards

   GUI_init(DEFAULT_BRIGHTNESS); // Initialisierung des LCD Boards mit Touch
   TP_Init(false); // Initialisierung des LCD Boards mit Touch
   enum NextPhase current_phase = INIT;
   // Test in Endlosschleife
   while (1) {
      read_buttons();
      current_phase = phase_ops[current_phase]();
      print_degree();
   }
}

// EOF

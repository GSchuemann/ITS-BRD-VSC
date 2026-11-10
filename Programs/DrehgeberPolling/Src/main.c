/**
 ******************************************************************************
 * @file    main.c
 * @author  Xuan Hoang Duy Trinh, Georg Schümann
 * @brief   Programm was den angeschlossenen Drehgeber ausliest und Winkel sowie
 Geschwindigkeit anzeigt. Dafür läuft eine Superloop, die am Anfange Knöpfe und
 Zeit liest und danach in einen State reingeht.
 ******************************************************************************
 */
/* Includes ------------------------------------------------------------------*/

#include "main.h"
#include "LCD_GUI.h"
#include "LCD_Touch.h"
#include "display.h"
#include "init.h"
#include "io.h"
#include "phases.h"
#include "timer.h"
#include "timing.h"

int main(void) {
   initITSboard(); // Initialisierung des ITS Boards

   GUI_init(DEFAULT_BRIGHTNESS); // Initialisierung des LCD Boards mit Touch
   TP_Init(false);               // Initialisierung des LCD Boards mit Touch
   initTimer();
   init_display();
   enum NextPhase current_phase = INIT;
   while (1) {
      read_buttons();
      update_time();
      current_phase = phase_ops[current_phase]();
   }
}
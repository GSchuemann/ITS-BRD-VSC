/**
 ******************************************************************************
 * @file    main.c
 * @author  Franz Korf
 * @brief   Kleines Testprogramm fuer neu erstelle Fonts.
 ******************************************************************************
 */
/* Includes ------------------------------------------------------------------*/

#include "LCD_GUI.h"
#include "LCD_Touch.h"
#include "init.h"
#include "lcd.h"
#include "stm32f4xx_hal.h"
#include "../Inc/display.h"
#include "../Inc/scanner.h"


int main(void) {
   initITSboard(); // Initialisierung des ITS Boards

   init_display();
   T_token tok;
   while (1) {
      tok = next_token();
   }
}

// EOF

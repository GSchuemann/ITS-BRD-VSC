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
#include "stm32f429xx.h"
#include "stm32f4xx_hal.h"
#include <stdint.h>
#include "main.h"


static char state[4] = {'A','B','C','D'};

int main(void) {
   initITSboard(); // Initialisierung des ITS Boards

   GUI_init(DEFAULT_BRIGHTNESS); // Initialisierung des LCD Boards mit Touch
   TP_Init(false); // Initialisierung des LCD Boards mit Touch

   // Test in Endlosschleife
   uint16_t buttons = read_buttons();
      char q_state = state[buttons & 0b11];
   while (1) { 
      buttons = read_buttons();
   }
}
uint32_t read_buttons() {
   return GPIOF->IDR;
}

// EOF

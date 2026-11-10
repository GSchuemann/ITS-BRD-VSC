/**
 ******************************************************************************
 * @file    display.c
 * @author  Xuan Hoang Duy Trinh, Georg Schümann
 * @brief   Einfache Display Ausgabe von Geschwindigtkeit und Grad
 ******************************************************************************
 */
/* Includes ------------------------------------------------------------------*/

#include "display.h"
#include "LCD_GUI.h"
#include "LCD_general.h"
#include "fonts.h"
#include "lcd.h"
#include "phase_counter.h"
#include "terminal.h"
#include "timing.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#define BG_COLOR WHITE
#define FG_COLOR BLACK
#define FONT (&Font16)

#define LEFT_X 10
#define LINE1_Y 20
#define LINE2_Y 60
#define LABEL_CHARS 18
#define VALUE_CHARS 10
#define UNIT_CHARS 8
#define VALUE_X (LEFT_X + (LABEL_CHARS + 1) * FONT->Width)
#define UNIT_X (VALUE_X + VALUE_CHARS * FONT->Width)
#define Y_VALUE_velocity 4
#define Y_VALUE_DEGREE 1
#define X_VALUE 19
static uint64_t last_refresh = 0;
static char velocity_buff[VALUE_CHARS + 1];
static char degree_buff[VALUE_CHARS + 1];
static char velocity_display_buff[VALUE_CHARS + 1];
static char degree_display_buff[VALUE_CHARS + 1];
static uint32_t pos_deg = VALUE_CHARS;
static uint32_t velocity_pos = VALUE_CHARS;

typedef struct Coordinates {
   uint16_t x;
   uint16_t y;
} Coordinates;

static Terminal make_term(Coordinates coordinates, uint16_t chars) {
   Coordinate top_left;
   Coordinate bottom_right;

   top_left.x = coordinates.x;
   top_left.y = coordinates.y;
   bottom_right.x = coordinates.x + (chars * FONT->Width);
   bottom_right.y = coordinates.y + FONT->Height;
   return genTerm(top_left, bottom_right, BG_COLOR, FG_COLOR, FONT);
}

void init_display(void) {
   Terminal label_term;

   GUI_clear(BG_COLOR);

   label_term = make_term((Coordinates){LEFT_X, LINE1_Y}, LABEL_CHARS);
   printTerm("Winkel          :", &label_term);
   label_term = make_term((Coordinates){LEFT_X, LINE2_Y}, LABEL_CHARS);
   printTerm("Geschwindigkeit :", &label_term);

   label_term = make_term((Coordinates){UNIT_X, LINE1_Y}, UNIT_CHARS);
   printTerm(" Grad", &label_term);
   label_term = make_term((Coordinates){UNIT_X, LINE2_Y}, UNIT_CHARS);
   printTerm(" Grad/s", &label_term);
}

static int validate_printable_ascii(char character) {
   return character >= '!' && character <= '~';
}
// BEGIN_NOLINT
static void print_char(char char_to_print, Coordinates coordinate) {
   // END_NOLINT
   lcdGotoXY(coordinate.x, coordinate.y);
   if (validate_printable_ascii(char_to_print)) {
      lcdPrintC(char_to_print);
   } else {
      lcdPrintC(' ');
   }
}
void print_values() {
   char buf[VALUE_CHARS + 1];
   if ((get_time() - last_refresh) >= (uint64_t)WINDOW_300MS) {
      last_refresh = get_time();
      snprintf(buf, sizeof(buf), "%10.1f", get_degree());
      if (strcmp(buf, degree_buff) != 0) {
         strlcpy(degree_buff, buf, VALUE_CHARS + 1);
         pos_deg = VALUE_CHARS;
      }
      snprintf(buf, sizeof(buf), "%10.1f", get_velocity());
      if (strcmp(buf, velocity_buff) != 0) {
         strlcpy(velocity_buff, buf, VALUE_CHARS + 1);
         velocity_pos = VALUE_CHARS;
      }
   }
   if (pos_deg != 0) {
      if (degree_buff[pos_deg] != degree_display_buff[pos_deg]) {
         print_char(degree_buff[pos_deg],
                    (Coordinates){X_VALUE + pos_deg, Y_VALUE_DEGREE});
         degree_display_buff[pos_deg] = degree_buff[pos_deg];
      }
      --pos_deg;
   }
   if (velocity_pos != 0 &&
       pos_deg == 0) { // only 1 char per Loop. Make sure to not print velocity
                       // when Position is printed!
      if (velocity_buff[velocity_pos] != velocity_display_buff[velocity_pos]) {
         print_char(velocity_buff[velocity_pos],
                    (Coordinates){X_VALUE + velocity_pos, Y_VALUE_velocity});
         velocity_display_buff[velocity_pos] = velocity_buff[velocity_pos];
      }
      --velocity_pos;
   }
}
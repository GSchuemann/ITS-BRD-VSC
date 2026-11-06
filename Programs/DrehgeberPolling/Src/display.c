#include "display.h"
#include "LCD_GUI.h"
#include "fonts.h"
#include "terminal.h"
#include <stdint.h>
#include <stdio.h>

#define BG_COLOR WHITE
#define FG_COLOR BLACK
#define FONT (&Font16)

#define LEFT_X 10
#define LINE1_Y 20
#define LINE2_Y 60
#define LABEL_CHARS 18
#define VALUE_CHARS 18
#define VALUE_X (LEFT_X + (LABEL_CHARS + 1) * FONT->Width)

static Terminal angle_term = INVALID_TERM;
static Terminal velocity_term = INVALID_TERM;

static Terminal make_term(uint16_t x, uint16_t y, uint16_t chars) {
   Coordinate top_left;
   Coordinate bottom_right;

   top_left.x = x;
   top_left.y = y;
   bottom_right.x = x + chars * FONT->Width;
   bottom_right.y = y + FONT->Height;
   return genTerm(top_left, bottom_right, BG_COLOR, FG_COLOR, FONT);
}

void init_display(void) {
   Terminal label_term;

   GUI_clear(BG_COLOR);

   label_term = make_term(LEFT_X, LINE1_Y, LABEL_CHARS);
   printTerm("Winkel          :", &label_term);
   label_term = make_term(LEFT_X, LINE2_Y, LABEL_CHARS);
   printTerm("Geschwindigkeit :", &label_term);

   angle_term = make_term(VALUE_X, LINE1_Y, VALUE_CHARS);
   velocity_term = make_term(VALUE_X, LINE2_Y, VALUE_CHARS);
}

void print_values(double angle, double velocity) {
   char buf[VALUE_CHARS + 1];

   snprintf(buf, sizeof(buf), "%10.1f Grad", angle);
   clearTerm(&angle_term);
   printTerm(buf, &angle_term);

   snprintf(buf, sizeof(buf), "%10.1f Grad/s", velocity);
   clearTerm(&velocity_term);
   printTerm(buf, &velocity_term);
}
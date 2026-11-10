#include "display.h"
#include "LCD_GUI.h"
#include "fonts.h"
#include "terminal.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "lcd.h"
#include "timer.h"
#include "timing.h"

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
static uint64_t last_refresh = 0;
static char speed_buff[VALUE_CHARS + 1];
static char degree_buff[VALUE_CHARS + 1];
static char speed_display_buff[VALUE_CHARS + 1];
static char degree_display_buff[VALUE_CHARS + 1];
static uint32_t pos_deg = VALUE_CHARS;
static uint32_t speed_deg = VALUE_CHARS;


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
   
   label_term = make_term(UNIT_X, LINE1_Y, UNIT_CHARS);
   printTerm(" Grad", &label_term);
   label_term = make_term(UNIT_X, LINE2_Y, UNIT_CHARS);
   printTerm(" Grad/s", &label_term);

   angle_term = make_term(VALUE_X, LINE1_Y, VALUE_CHARS);
   velocity_term = make_term(VALUE_X, LINE2_Y, VALUE_CHARS);
}

// static void print_value(Terminal *term, double value) {
//    clearTerm(term);
//    printTerm(buf, term);
// }
static int validate_printable_ascii(char character){
   return character >= '!' && character <= '~';
}
void print_values(double angle, double velocity) {
   char buf[VALUE_CHARS + 1];
   if((get_time() - last_refresh) >= WINDOW_300MS);
   last_refresh = get_time();
   snprintf(buf, sizeof(buf), "%10.1f", angle);
   if(strcmp(buf,degree_buff) != 0) {
      strcpy(degree_buff, buf);
      pos_deg = VALUE_CHARS;
   }
   snprintf(buf, sizeof(buf), "%10.1f", velocity);
   if(strcmp(buf,speed_buff) != 0) {
      strcpy(speed_buff, buf);
      speed_deg = VALUE_CHARS;
   }
   if(pos_deg != 0) {
      lcdGotoXY(19 + pos_deg, 1 );
      if(validate_printable_ascii(degree_buff[pos_deg]) && degree_buff[pos_deg] != degree_display_buff[pos_deg]){
         lcdPrintC(degree_buff[pos_deg]);
         degree_display_buff[pos_deg] = degree_buff[pos_deg];
      }
      --pos_deg;
      if(speed_deg != 0) {
      lcdGotoXY(19 + speed_deg, 4 );
      if(validate_printable_ascii(speed_buff[speed_deg]) && speed_buff[speed_deg] != speed_display_buff[speed_deg]){
         lcdPrintC(speed_buff[speed_deg]);
         speed_display_buff[speed_deg] = speed_buff[speed_deg];
      }
      --speed_deg;
   }
   }

   // print_value(&angle_term, angle);
   // print_value(&velocity_term, velocity);
}
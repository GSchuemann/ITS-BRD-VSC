#include "display.h"
#include "LCD_GUI.h"
#include "LCD_general.h"
#include "fonts.h"
#include "terminal.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "lcd.h"
#include "timing.h"
#include "phase_counter.h"
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
#define Y_VALUE_SPEED 4
#define Y_VALUE_DEGREE 1
#define X_VALUE 19
static uint64_t last_refresh = 0;
static char speed_buff[VALUE_CHARS + 1];
static char degree_buff[VALUE_CHARS + 1];
static char speed_display_buff[VALUE_CHARS + 1];
static char degree_display_buff[VALUE_CHARS + 1];
static uint32_t pos_deg = VALUE_CHARS;
static uint32_t speed_pos = VALUE_CHARS;


typedef struct Coordinates{
   uint16_t x;
   uint16_t y;
} Coordinates ;

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
   label_term = make_term((Coordinates) {LEFT_X, LINE2_Y}, LABEL_CHARS);
   printTerm("Geschwindigkeit :", &label_term);
   
   label_term = make_term((Coordinates){UNIT_X, LINE1_Y}, UNIT_CHARS);
   printTerm(" Grad", &label_term);
   label_term = make_term((Coordinates){UNIT_X, LINE2_Y}, UNIT_CHARS);
   printTerm(" Grad/s", &label_term);

}


static int validate_printable_ascii(char character){
   return character >= '!' && character <= '~';
}
//BEGIN_NOLINT
static void print_char(char char_to_print,Coordinates coordinate ){
//END_NOLINT
lcdGotoXY(coordinate.x, coordinate.y);
if(validate_printable_ascii(char_to_print)){
   lcdPrintC(char_to_print);
} else {
   lcdPrintC(' ');
}
}
void print_values() {
   char buf[VALUE_CHARS + 1];
   if((get_time() - last_refresh) >= WINDOW_300MS){
      last_refresh = get_time();
      snprintf(buf, sizeof(buf), "%10.1f", get_degree());
      if(strcmp(buf,degree_buff) != 0) {
         strcpy(degree_buff, buf);
         pos_deg = VALUE_CHARS;
      }
      snprintf(buf, sizeof(buf), "%10.1f", get_speed());
      if(strcmp(buf,speed_buff) != 0) {
         strcpy(speed_buff, buf);
         speed_pos = VALUE_CHARS;
      }
   }
   if(pos_deg != 0) {
      if(degree_buff[pos_deg] != degree_display_buff[pos_deg]){
         print_char(degree_buff[pos_deg] ,(Coordinates){ X_VALUE + pos_deg,Y_VALUE_DEGREE});
         degree_display_buff[pos_deg] = degree_buff[pos_deg];
      }
      --pos_deg;
   }
   if(speed_pos != 0 && pos_deg == 0) { //only 1 char per Loop. Make sure to not print Speed when Position is printed!
      lcdGotoXY(19 + speed_pos, 4 );
      if(speed_buff[speed_pos] != speed_display_buff[speed_pos]){
         print_char(speed_buff[speed_pos] , (Coordinates){X_VALUE + speed_pos, Y_VALUE_SPEED});
         speed_display_buff[speed_pos] = speed_buff[speed_pos];
      }
      --speed_pos;
   }

}
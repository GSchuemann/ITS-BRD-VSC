#include "../Inc/print.h"
#include "../Inc/display.h"
#include "../Inc/status.h"

#define ASCII_OFFSET_FOR_NUM '0'
#define BASE 10
#define MAX_INT_ASCII_SIZE 11
static void print_err_helper(char *msg) {
   set_err_mode();
   print_stdout(msg);
};

void print_err(enum Status err) {
   switch (err) {
   case CALC_STATUS_DIV_BY_ZERO:
      print_err_helper("Kann nicht durch 0 teilen!");
      break;
   case CALC_STATUS_OVERFLOW:
      print_err_helper("Es gab einen Overflow!");
      break;
   case CALC_STATUS_STACK_FULL:
      print_err_helper(
          "Es wurden die maximalen Elemente auf dem Stack ueberschritten!");
      break;
   case CALC_STATUS_STACK_EMPTY:
      print_err_helper("Es gibt zu wenige Elemente auf dem Stack!");
      break;
   case CALC_STATUS_INVALID_INPUT:
      print_err_helper("Diese Funktion wurde entweder noch nicht Implementiert "
                       "oder es ist ein anderer Fehler aufgetreten!");
      break;
   default:
      print_err_helper("Es ist ein unbekannter Fehlercode aufgetreten!");
   }
};

void print_num(int32_t num) {
   char digits[MAX_INT_ASCII_SIZE];
   // NOLINTBEGIN
   uint32_t n = 0;
   // NOLINTEND
   bool negative = (num < 0);
   uint32_t abs_value = (uint32_t)num;
   if (negative) {
      abs_value = 0U - abs_value;
   }
   if (abs_value == 0U) {
      digits[n++] = '0';
   } else {
      while (abs_value > 0U) {
         digits[n++] = (char)((abs_value % BASE) + ASCII_OFFSET_FOR_NUM);
         abs_value /= BASE;
      }
   }
   if (negative) {
      digits[n++] = '-';
   }
   while (n > 0) {
      print_to_echo_line(digits[--n]);
   }
   print_to_echo_line(' ');
};
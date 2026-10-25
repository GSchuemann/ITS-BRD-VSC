#include "../Inc/print.h"
#include "../Inc/status.h"
#include "../Inc/display.h"

#define ASCII_OFFSET_FOR_NUM '0'
#define BASE 10
#define MAX_INT_ASCII_SIZE 6
static void print_err_helper(char *msg) {
   set_err_mode();
   print_stdout(msg);
   };

void print_err(enum Status err) {
   switch (err) {
   case CALC_STATUS_DIV_BY_ZERO:
      print_err_helper("Kann nicht durch 0 teilen!");
   case CALC_STATUS_OVERFLOW:
      print_err_helper("Es gab einen Overflow!");
   case CALC_STATUS_STACK_FULL:
      print_err_helper("Es wurden die maximalen Elemente auf dem Stack überschritten!");
   case CALC_STATUS_STACK_EMPTY:
      print_err_helper("Es gibt zu wenige Elemente auf dem Stack!");
      break;
   case CALC_STATUS_INVALID_INPUT:
      print_err_helper("Diese Funktion wurde entweder noch nicht Implementiert oder es ist ein anderer Fehler aufgetreten!");
      break;
      default: print_err_helper("Es ist ein unbekannter Fehlercode aufgetreten!"); 
   }
};

void print_num_with_clear(int32_t num) {
   clear_echo_term();
   print_num(num);
};
void print_num(int32_t num) {
   char digits[MAX_INT_ASCII_SIZE]; // enough for binary digits of an unsigned integer
   uint32_t n = 0;
   if (num < 0) {
      digits[n++] = '-';
      num *= -1;
   }
   if (num == 0) {
      digits[n++] = '0';
   } else {
      while (num > 0) {
         digits[n++] = (char)((num % BASE) + ASCII_OFFSET_FOR_NUM);
         num /= BASE;
      }
   }

   while (n > 0) {
      print_to_echo_line(digits[--n]);
   }
};
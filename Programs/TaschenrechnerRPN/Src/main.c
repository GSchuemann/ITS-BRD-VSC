/**
 ******************************************************************************
 * @file    main.c
 * @author  Franz Korf
 * @brief   Kleines Testprogramm fuer neu erstelle Fonts.
 ******************************************************************************
 */
/* Includes ------------------------------------------------------------------*/

// #include "LCD_GUI.h"
// #include "LCD_Touch.h"
#include "init.h"
// #include "lcd.h"
#include "stack_functions.h"
#include "status.h"
#include "stm32f4xx_hal.h"
#include "../Inc/display.h"
#include "../Inc/scanner.h"
#include "../Inc/print.h"
#include "../Inc/stack.h"
#include "../Inc/math_calc.h"
#include "../Inc/stack_functions.h"
//#include <stdint.h>
#include <stdio.h>
#include "../Inc/test.h"
#include "token.h"
//#include "../Inc/stack.h"


enum Status math_helper(T_token tok) {
   enum Status status = CALC_STATUS_OK;

   int32_t result;
   switch (tok.tok) {
      case PLUS:
         status = math_add(&result);
         break;
      case MINUS:
         status = math_sub(&result);
         break;
      case MULT:
         status = math_mul(&result);
         break;
      case DIV:
         status = math_div(&result);
         break;
      }
      if (status < CALC_STATUS_OK) {
         return status;
      }
      status = add_to_stack(result);
      if (status < CALC_STATUS_OK) {
         return status;
      }
      return status;
   }

int main(void) {
   // test();
   initITSboard(); // Initialisierung des ITS Boards
   init_display();
   enum Status status = CALC_STATUS_OK;
   int clear = 1; 
   while (1) {
      T_token tok = next_token(clear);
      clear = 1;
      switch (tok.tok) {
      case CLEAR:
         clearAll();
         break;
      case OVERFLOW:
         status = CALC_STATUS_OVERFLOW;
         break;
      case NUMBER:
            status = add_to_stack(tok.val);
            break;
         case PLUS: //Fall through to code in div, math is handled in a helper function
         case MINUS:
         case MULT:
         case DIV:
            status = math_helper(tok);
            break;
         case PRT:
            status = print_from_stack(1);
            clear = 0;
            break;
         case SWAP:
            status = swap_top_two();
            break;
         case PRT_ALL:
            status = print_from_stack(3);
            clear = 0;
            break;
         case DOUBLE:
            status = duplicate_top();
            break;
         case UNEXPECTED: //same as default
         default:
            status = CALC_STATUS_INVALID_INPUT;
         }
         if (status < CALC_STATUS_OK) {
            print_err(status);
         }
   }
   return 0;
}

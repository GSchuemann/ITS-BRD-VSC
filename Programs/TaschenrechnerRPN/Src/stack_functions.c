#include "../Inc/stack_functions.h"
#include "../Inc/display.h"
#include "../Inc/print.h"
#include "../Inc/stack.h"
#include "../Inc/status.h"
#include <stdint.h>
#include <stdio.h>

#define PRINT_BUFFER_SIZE 16
#define PRINT_COUNT 3

enum Status print_from_stack(uint8_t n) {
   int32_t values[n];
   uint8_t popped = 0;
   for (uint8_t i = 0; i < n; i++) {
      int32_t status = pop_from_stack(&values[i]);
      if (status < CALC_STATUS_OK) {
         break;
      }
      popped++;
   }
   if (popped == 0) {
      return CALC_STATUS_STACK_EMPTY;
   }
   for (uint8_t i = 0; i < popped; i++) {
      print_num(values[i]);
   }
   for (uint8_t i = popped; i > 0; i--) {
      int32_t status = add_to_stack(values[i - 1]);
      if (status < CALC_STATUS_OK) {
         return status;
      }
   }
   return CALC_STATUS_OK;
}

void clear_all(void) {
   clear_stack();
   set_normal_mode();
}

enum Status duplicate_top(void) {
   int32_t value = 0;
   int32_t status = pop_from_stack(&value);
   if (status < CALC_STATUS_OK) {
      return status;
   }
   status = add_to_stack(value);
   if (status < CALC_STATUS_OK) {
      return status;
   }
   return add_to_stack(value);
}

enum Status swap_top_two(void) {
   int32_t top = 0;
   int32_t below = 0;
   enum Status status = pop_two(&top, &below);
   if (status < CALC_STATUS_OK) {
      return status;
   }
   status = add_to_stack(top);
   if (status < CALC_STATUS_OK) {
      return status;
   }
   return add_to_stack(below);
}
#include "../Inc/stack.h"
#include "../Inc/status.h"
#include "stack_functions.h"
#define MIN_STACK_POINTER (-1)
#define STACK_SIZE 100

static int32_t stack_pointer = MIN_STACK_POINTER;
static int32_t stack[STACK_SIZE];
bool is_stack_empty() { return stack_pointer == MIN_STACK_POINTER; };

int32_t add_to_stack(int32_t number) {
   if (stack_pointer >= (STACK_SIZE - 1)) {
      return CALC_STATUS_STACK_FULL;
   }
   stack_pointer++;
   stack[stack_pointer] = number;
   return CALC_STATUS_OK;
}

int32_t pop_from_stack(int32_t *ret) {
   if (stack_pointer < 0) {
      return CALC_STATUS_STACK_EMPTY;
   }
   *ret = stack[stack_pointer];
   stack_pointer--;
return CALC_STATUS_OK;
}

void clear_stack() {
   stack_pointer = MIN_STACK_POINTER;
}

int32_t get_last_stack_pos() { return stack_pointer; }
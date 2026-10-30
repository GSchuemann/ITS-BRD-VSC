#include "../Inc/stack.h"
#include "../Inc/status.h"
#define MIN_STACK_POINTER (-1)
#define STACK_SIZE 100

static int32_t stack_pointer = MIN_STACK_POINTER;
static int32_t stack[STACK_SIZE];

enum Status add_to_stack(int32_t number) {
   if (stack_pointer >= (STACK_SIZE - 1)) {
      return CALC_STATUS_STACK_FULL;
   }
   stack_pointer++;
   stack[stack_pointer] = number;
   return CALC_STATUS_OK;
}

enum Status pop_from_stack(int32_t *ret) {
   if (stack_pointer < 0) {
      return CALC_STATUS_STACK_EMPTY;
   }
   *ret = stack[stack_pointer];
   stack_pointer--;
   return CALC_STATUS_OK;
}

enum Status pop_two(int32_t *top, int32_t *below) {
   enum Status status = pop_from_stack(top);
   if (status < CALC_STATUS_OK) {
      return status;
   }
   status = pop_from_stack(below);
   if (status < CALC_STATUS_OK) {
      (void)add_to_stack(*top);
   }
   return status;
}

void clear_stack(void) { stack_pointer = MIN_STACK_POINTER; }

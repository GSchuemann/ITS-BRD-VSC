#include "../Inc/math_calc.h"
#include "../Inc/stack.h"
#include "../Inc/status.h"
#include <limits.h>
#include <stdint.h>
#define MINUS_ONE (-1)

static enum Status pop_two(int32_t *top, int32_t *below) {
   int32_t status = pop_from_stack(top);
   if (status < CALC_STATUS_OK) {
      return status;
   }
   return pop_from_stack(below);
}

enum Status math_add(int32_t *result) {
   int32_t num1 = 0;
   int32_t num2 = 0;
   int32_t status = pop_two(&num1, &num2);
   if (status < CALC_STATUS_OK) {
      return status;
   }
   if (((num2 > 0) && (num1 > (INT_MAX - num2))) ||
       ((num2 < 0) && (num1 < (INT_MIN - num2)))) {
      return CALC_STATUS_OVERFLOW;
   }
   *result = num1 + num2;
   return CALC_STATUS_OK;
}

enum Status math_sub(int32_t *result) {
   int32_t num1 = 0;
   int32_t num2 = 0;
   int32_t status = pop_two(&num1, &num2);
   if (status < CALC_STATUS_OK) {
      return status;
   }
   if (((num1 < 0) && (num2 > (INT_MAX + num1))) ||
       ((num1 > 0) && (num2 < (INT_MIN + num1)))) {
      return CALC_STATUS_OVERFLOW;
   }
   *result = num2 - num1;
   return CALC_STATUS_OK;
}

enum Status math_mul(int32_t *result) {
   int32_t num1 = 0;
   int32_t num2 = 0;
   int32_t status = pop_two(&num1, &num2);
   if (status < CALC_STATUS_OK) {
      return status;
   }
   if (((num1 > 0) && (num2 > 0) && (num1 > (INT_MAX / num2))) ||
       ((num1 > 0) && (num2 < 0) && (num2 < (INT_MIN / num1))) ||
       ((num1 < 0) && (num2 > 0) && (num1 < (INT_MIN / num2))) ||
       ((num1 < 0) && (num2 < 0) && (num1 < (INT_MAX / num2)))) {
      return CALC_STATUS_OVERFLOW;
   }
   *result = num1 * num2;
   return CALC_STATUS_OK;
}

enum Status math_div(int32_t *result) {
   int32_t num1 = 0;
   int32_t num2 = 0;
   int32_t status = pop_two(&num1, &num2);
   if (status < CALC_STATUS_OK) {
      return status;
   }
   if (num1 == 0) {
      return CALC_STATUS_DIV_BY_ZERO;
   }
   if ((num2 == INT_MIN) && (num1 == MINUS_ONE)) {
      return CALC_STATUS_OVERFLOW;
   }
   *result = num2 / num1;
   return CALC_STATUS_OK;
}
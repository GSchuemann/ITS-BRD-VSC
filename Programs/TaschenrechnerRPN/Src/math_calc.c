#include <limits.h>
#include "../Inc/status.h"
#include "../Inc/stack.h"
#include "../Inc/math_calc.h"
#include <stdint.h>
#include <stdio.h>

#define MINUS_ONE (-1)

int32_t math_add(int32_t* result)
{
    int32_t a = 0;
    int32_t b = 0;

    int32_t status = pop_from_stack(&a);
    if (status < CALC_STATUS_OK) {
        return status;
    }
    status = pop_from_stack(&b);
    if (status < CALC_STATUS_OK) {
        return status;
    }
    if ((b > 0) && (a > (INT_MAX - b))) {
        return CALC_STATUS_OVERFLOW;
    }
    if ((b < 0) && (a < (INT_MIN - b))) {
        return CALC_STATUS_OVERFLOW;
    }
    *result = a + b;
    return CALC_STATUS_OK;
}

int32_t math_sub(int32_t* result)
{
    int32_t a = 0;
    int32_t b = 0;

    int32_t status = pop_from_stack(&a);
    if (status < CALC_STATUS_OK) {
        return status;
    }
    status = pop_from_stack(&b);
    if (status < CALC_STATUS_OK) {
        return status;
    }

    if ((a < 0) && (b > (INT_MAX + a))) {
        return CALC_STATUS_OVERFLOW;
    }
    if ((a > 0) && (b < (INT_MIN + a))) {
        return CALC_STATUS_OVERFLOW;
    }

    *result = b - a;
    return CALC_STATUS_OK;
}

int32_t math_mul(int32_t* result)
{
    int32_t a = 0;
    int32_t b = 0;

    int32_t status = pop_from_stack(&a);
    if (status < CALC_STATUS_OK) {
        return status;
    }
    status = pop_from_stack(&b);
    if (status < CALC_STATUS_OK) {
        return status;
    }

    if ((a == 0) || (b == 0)) {
        *result = 0;
        return CALC_STATUS_OK;
    }
    if ((a > 0) && (b > 0) && (a > (INT_MAX / b))) {
        return CALC_STATUS_OVERFLOW;
    }
    if ((a > 0) && (b < 0) && (b < (INT_MIN / a))) {
        return CALC_STATUS_OVERFLOW;
    }
    if ((a < 0) && (b > 0) && (a < (INT_MIN / b))) {
        return CALC_STATUS_OVERFLOW;
    }
    if ((a < 0) && (b < 0) && (a < (INT_MAX / b))) {
        return CALC_STATUS_OVERFLOW;
    }
    *result = a * b;
    return CALC_STATUS_OK;
}

int32_t math_div(int32_t* result)
{
    int32_t a = 0;
    int32_t b = 0;

    int32_t status = pop_from_stack(&a);
    if (status < CALC_STATUS_OK) {
        return status;
    }
    status = pop_from_stack(&b);
    if (status < CALC_STATUS_OK) {
        return status;
    }

    if (a == 0) {
        return CALC_STATUS_DIV_BY_ZERO;
    }
    if ((b == INT_MIN) && (a == MINUS_ONE)) {
        return CALC_STATUS_OVERFLOW;
    }

    *result = b / a;
    return CALC_STATUS_OK;
}
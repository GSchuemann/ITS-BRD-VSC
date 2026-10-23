#include <limits.h>
#include <stdint.h>
#include "../Inc/status.h"
#include "../Inc/stack.h"
#include "../Inc/math_calc.h"
#define MINUS_ONE (-1)

static int32_t pop_two(int32_t* top, int32_t* below)
{
    int32_t status = pop_from_stack(top);
    if (status < CALC_STATUS_OK) {
        return status;
    }
    return pop_from_stack(below);
}

int32_t math_add(int32_t* result)
{
    int32_t a = 0;
    int32_t b = 0;
    int32_t status = pop_two(&a, &b);
    if (status < CALC_STATUS_OK) {
        return status;
    }
    if (((b > 0) && (a > (INT_MAX - b))) || 
        ((b < 0) && (a < (INT_MIN - b)))) {
        return CALC_STATUS_OVERFLOW;
    }
    *result = a + b;
    return CALC_STATUS_OK;
}

int32_t math_sub(int32_t* result)
{
    int32_t a = 0;
    int32_t b = 0;
    int32_t status = pop_two(&a, &b);
    if (status < CALC_STATUS_OK) {
        return status;
    }
    if (((a < 0) && (b > (INT_MAX + a))) || 
        ((a > 0) && (b < (INT_MIN + a)))) {
        return CALC_STATUS_OVERFLOW;
    }
    *result = b - a;
    return CALC_STATUS_OK;
}

int32_t math_mul(int32_t* result)
{
    int32_t a = 0;
    int32_t b = 0;
    int32_t status = pop_two(&a, &b);
    if (status < CALC_STATUS_OK) {
        return status;
    }
    if (((a > 0) && (b > 0) && (a > (INT_MAX / b))) ||
        ((a > 0) && (b < 0) && (b < (INT_MIN / a))) ||
        ((a < 0) && (b > 0) && (a < (INT_MIN / b))) ||
        ((a < 0) && (b < 0) && (a < (INT_MAX / b)))) {
        return CALC_STATUS_OVERFLOW;
    }
    *result = a * b;
    return CALC_STATUS_OK;
}

int32_t math_div(int32_t* result)
{
    int32_t a = 0;
    int32_t b = 0;
    int32_t status = pop_two(&a, &b);
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
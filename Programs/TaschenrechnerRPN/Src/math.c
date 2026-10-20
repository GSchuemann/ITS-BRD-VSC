#include <limits.h>

#include "../Inc/status.h"
#include "../Inc/stack.h"
#include "../Inc/math_calc.h"
#include <stdint.h>
#include <stdio.h>
#define MINUS_ONE (-1)

int math_add(int* result)
{
    int32_t a = 0;
    int32_t b = 0;

    int status = pop_from_stack(&a);
    if(status < CALC_STATUS_OK){
        return status;
    }
    status = pop_from_stack(&b);
    if(status < CALC_STATUS_OK) {
        return status;
    }
    // if ((b > 0) && (a > (INT_MAX - b))) {
    //     return CALC_STATUS_ARITH_OVERFLOW;
    // }
    // if ((b < 0) && (a < (INT_MIN - b))) {
    //     return CALC_STATUS_OVERFLOW;
    // }
    printf("a: %d b: %d\n", a,b);
    *result = a + b;
    return CALC_STATUS_OK;

}
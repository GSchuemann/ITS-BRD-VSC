#include <limits.h>
#include "../Inc/status.h"
#include "../Inc/stack.h"
#include "../Inc/math_calc.h"
#include "../Inc/numbers.h"

#include <stdint.h>
#include <stdio.h>

#define MINUS_ONE (-1)

int32_t math_add(bigInt* result)
{
    bigInt a = {};
    bigInt b = {};

    int32_t status = pop_from_stack(&a);
    if (status < CALC_STATUS_OK) {
        return status;
    }
    status = pop_from_stack(&b);
    if (status < CALC_STATUS_OK) {
        return status;
    }
    uint8_t carry = 0;
    uint8_t size = 0;
    if(a.size > b.size){
        size = a.size;
    } else {
        size = b.size;
    }
    for(int32_t i =0;i<size;i++){
        uint32_t xpsr;
        uint32_t sum;
        uint32_t a_num = (i <= a.size)? a.number[i]:0;
        uint32_t b_num = (i <= b.size)? b.number[i]:0;
    __asm__ volatile (
        "adds %[sum], %[a], %[b]\n\t"
        "mrs  %[psr], xpsr"
        : [sum] "=&r" (sum),
            [psr] "=&r" (xpsr)
        : [a] "r" (a_num),
            [b] "r" (b_num)
        : "cc"
    );
    push_part_numb(result,(sum+carry));
    carry = (xpsr >> 29) & 1u;
}
    if(carry > 0){
        push_part_numb(result,(carry));
    }
    free_big_int(&a);
    free_big_int(&b);

    return CALC_STATUS_OK;
}

// int32_t math_sub(int32_t* result)
// {
//     int32_t a = 0;
//     int32_t b = 0;

//     int32_t status = pop_from_stack(&a);
//     if (status < CALC_STATUS_OK) {
//         return status;
//     }
//     status = pop_from_stack(&b);
//     if (status < CALC_STATUS_OK) {
//         return status;
//     }

//     if ((a < 0) && (b > (INT_MAX + a))) {
//         return CALC_STATUS_OVERFLOW;
//     }
//     if ((a > 0) && (b < (INT_MIN + a))) {
//         return CALC_STATUS_OVERFLOW;
//     }

//     *result = b - a;
//     return CALC_STATUS_OK;
// }

// int32_t math_mul(int32_t* result)
// {
//     int32_t a = 0;
//     int32_t b = 0;

//     int32_t status = pop_from_stack(&a);
//     if (status < CALC_STATUS_OK) {
//         return status;
//     }
//     status = pop_from_stack(&b);
//     if (status < CALC_STATUS_OK) {
//         return status;
//     }

//     if ((a == 0) || (b == 0)) {
//         *result = 0;
//         return CALC_STATUS_OK;
//     }
//     if ((a > 0) && (b > 0) && (a > (INT_MAX / b))) {
//         return CALC_STATUS_OVERFLOW;
//     }
//     if ((a > 0) && (b < 0) && (b < (INT_MIN / a))) {
//         return CALC_STATUS_OVERFLOW;
//     }
//     if ((a < 0) && (b > 0) && (a < (INT_MIN / b))) {
//         return CALC_STATUS_OVERFLOW;
//     }
//     if ((a < 0) && (b < 0) && (a < (INT_MAX / b))) {
//         return CALC_STATUS_OVERFLOW;
//     }
//     *result = a * b;
//     return CALC_STATUS_OK;
// }

// int32_t math_div(int32_t* result)
// {
//     int32_t a = 0;
//     int32_t b = 0;

//     int32_t status = pop_from_stack(&a);
//     if (status < CALC_STATUS_OK) {
//         return status;
//     }
//     status = pop_from_stack(&b);
//     if (status < CALC_STATUS_OK) {
//         return status;
//     }

//     if (a == 0) {
//         return CALC_STATUS_DIV_BY_ZERO;
//     }
//     if ((b == INT_MIN) && (a == MINUS_ONE)) {
//         return CALC_STATUS_OVERFLOW;
//     }

//     *result = b / a;
//     return CALC_STATUS_OK;
// }
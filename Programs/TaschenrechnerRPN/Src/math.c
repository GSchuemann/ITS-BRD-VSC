#include "math.h"

#include <limits.h>

#include "status.h"

#define MINUS_ONE (-1)

int math_add(int a, int b, int result)
{
    if ((b > 0) && (a > (INT_MAX - b))) {
        return STATUS_ARITH_OVERFLOW;
    }
    if ((b < 0) && (a < (INT_MIN - b))) {
        return STATUS_OVERFLOW;
    }
    result = a + b;
    return STATUS_OK;
}
#include "../Inc/stack.h"
#include "../Inc/math_calc.h"
#include <stdio.h>
#include <limits.h>
#include "../Inc/test.h"

int test(){

    int result = 0;
    int status =0;

    printf("\n===== Tests(+-*/) =====\n\n"); 

    /* Test: 5 + 10 = 15 */
    printf("Test(+): (5 +10)\n");
    add_to_stack(5);
    add_to_stack(10);
    status = math_add(&result);
    printf("result %d\nstatus %d\n", result, status);
    printf("status: %s\n\n", (result == 15 && status == 0) ? "pass" : "fail");

    /* Test: 10 - 5 = 5 */
    printf("Test(-): (10 - 5)\n");
    add_to_stack(10);
    add_to_stack(5);
    status = math_sub(&result);
    printf("result %d\nstatus %d\n", result, status);
    printf("status: %s\n\n", (result == 5 && status == 0) ? "pass" : "fail");

    /* Test: 5 * 10 =50 */
    printf("Test(*): (5 * 10)\n");
    add_to_stack(10);
    add_to_stack(5);
    status = math_mul(&result);
    printf("result %d\nstatus %d\n", result, status);
    printf("status: %s\n\n", (result == 50 && status == 0) ? "pass" : "fail");

    /* Test: 10 / 5 = 2 */
    printf("Test(/): (10 / 5)\n");
    add_to_stack(10);
    add_to_stack(5);
    status = math_div(&result);
    printf("result %d\nstatus %d\n", result, status);
    printf("status: %s\n\n", (result == 2 && status == 0) ? "pass" : "fail");

    /* Test: Division by Zero */
    printf("Test: Division by Zero (10 / 0)\n");
    add_to_stack(10);
    add_to_stack(0);
    status = math_div(&result);
    printf("Status: %d (expected: -1)\n", status);
    printf("Status: %s\n\n", (status < 0) ? "pass" : "fail");

    /* Test: Overflow Multiplication */
    printf("Test: Overflow Multiplication (INT_MAX * 2)\n");
    add_to_stack(2);
    add_to_stack(INT_MAX);
    status = math_mul(&result);
    printf("Status: %d (expected: negative)\n", status);
    printf("Status: %s\n\n", (status < 0) ? "pass" : "fail");

    test_stack_functions();

    return 0;
}






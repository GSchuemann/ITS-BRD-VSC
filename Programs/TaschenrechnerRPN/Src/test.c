#include "../Inc/stack.h"
#include "../Inc/math_calc.h"
#include "../Inc/status.h"
#include <stdio.h>
// #include "../Inc/status.h"
// #include <limits.h>

void big_int_create() {
    
};
int test(){
    add_to_stack(5);
    add_to_stack(10);
    int result = 0;
    int status = math_add(&result);
    printf("status %d\nergebnis %d\n", status ,result);
    status = math_add(&result);
    printf("status < 0 %d\n", status);
    return CALC_STATUS_OK;
}

// void big_int_create(void);


#include "../Inc/stack.h"
#include "../Inc/math_calc.h"
#include <stdio.h>
#include <limits.h>
int test(){
    add_to_stack(5);
    add_to_stack(10);
    int result = 0;
    int status = math_add(&result);
    printf("status %d\nergebnis %d\n", status ,result);
    status = math_add(&result);
    printf("status < 0 %d\n", status); 
}




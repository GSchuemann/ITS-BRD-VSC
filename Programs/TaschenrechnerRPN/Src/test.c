#include "../Inc/stack.h"
#include "../Inc/math_calc.h"
#include "../Inc/status.h"
#include "../Inc/numbers.h"
#include <stdio.h>
// #include "../Inc/status.h"
// #include <limits.h>

void big_int_create() {
    
};


int test(){
    bigInt num = { .size=0, .capacity=0, .number = NULL, .sign=0};
    add_to_stack(num);
    uint32_t stack_pos = get_last_stack_pos();
    void* stack_addr = get_stack_start_addr();
    
    void* int_ptr = (void *) stack_addr + (stack_pos  * sizeof(bigInt)); //pointer arithmetic, should be done cleaner for testing!
    int32_t status = push_part_numb(int_ptr,1LL << 31);
    printf("status, should be 0: %d\n",status);
    status = push_part_numb(int_ptr,1LL << 3);
    pop_from_stack(&num);
    printf("Capacitiy should be 2: %d\n", num.capacity);
    add_to_stack(num);
    add_to_stack(num);

    bigInt result = { .size=0, .capacity=0, .number = NULL, .sign=0};
    status = math_add(&result);
    printf("status %d\nergebnis %ld\n", status ,*result.number);
    // add_to_stack(5);
    // add_to_stack(10);
    // int result = 0;
    // int status = math_add(&result);
    // printf("status %d\nergebnis %d\n", status ,result);
    // status = math_add(&result);
    // printf("status < 0 %d\n", status);
    return CALC_STATUS_OK;
}


// void big_int_create(void);


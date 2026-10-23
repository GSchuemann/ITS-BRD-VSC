#include "../Inc/stack.h"
#include "../Inc/status.h"
#include "../Inc/numbers.h"
#define MIN_STACK_POINTER (-1)
#define STACK_SIZE  100


static int32_t stack_pointer = MIN_STACK_POINTER;
static bigInt stack[STACK_SIZE];
bool is_stack_empty(){
    return stack_pointer == MIN_STACK_POINTER;
};

int32_t add_to_stack(bigInt number) {
    stack_pointer++;
    if(stack_pointer > 100){
        return CALC_STATUS_STACK_FULL;
    }
    stack[stack_pointer] = number;
    return CALC_STATUS_OK;
}

int32_t pop_from_stack(bigInt* ret){
    if(stack_pointer < 0) {
        return CALC_STATUS_STACK_EMPTY;
    }
    *ret = stack[stack_pointer];
    stack_pointer--;
    return CALC_STATUS_OK;
}

int32_t get_last_stack_pos(){
    return stack_pointer;
}
void* get_stack_start_addr(){
    return &stack;
}
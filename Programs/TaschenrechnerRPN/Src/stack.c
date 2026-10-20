#include "../Inc/stack.h"
#include "../Inc/statuses.h"
#define MIN_STACK_POINTER (-1);
#define STACK_SIZE  100;


static int32_t stack_pointer = MIN_STACK_POINTER;
static int32_t stack[100] = {};
bool is_stack_empty(){
    return stack_pointer == MIN_STACK_POINTER;
};

int32_t add_to_stack(int32_t number) {
    stack_pointer++;
    if(stack_pointer > 100){
        return CALC_INDEX_OUT_OF_BOUNDS;
    }
    stack[stack_pointer] = number;
    return CALC_STATUS_OK;
}

int32_t pop_from_stack(int32_t* ret){
    stack_pointer--;
    if(stack_pointer < 0) {
        return CALC_INDEX_OUT_OF_BOUNDS;
    }
    *ret = stack[stack_pointer];
    return CALC_STATUS_OK;
}

int32_t get_last_stack_pos(){
    return stack_pointer;
}
#include "../Inc/stack_functions.h"
#include "../Inc/display.h"
#include "../Inc/stack.h"
#include "../Inc/status.h"
#include <stdio.h>

#define PRINT_BUFFER_SIZE 16
#define PRINT_COUNT 3

static void print_number(int32_t value)
{
    char buffer[PRINT_BUFFER_SIZE] = {0};
    (void)snprintf(buffer, sizeof(buffer), "%d\n", (int)value);
    print_stdout(buffer);
}

int32_t print_top(void)
{
    int32_t value = 0;
    int32_t status = pop_from_stack(&value);
    if (status < CALC_STATUS_OK) {
        return status;
    }
    print_number(value);
    return add_to_stack(value);
}

int32_t print_top_three(void)
{
    int32_t values[PRINT_COUNT] = {0};
    int32_t count = 0;
    int32_t status = CALC_STATUS_OK;

    while ((count < PRINT_COUNT) && (status == CALC_STATUS_OK)) {
        status = pop_from_stack(&values[count]);
        if (status == CALC_STATUS_OK) {
            count++;
        }
    }
    if (count == 0) {
        return status;
    }
    for (int32_t index = 0; index < count; index++) {
        print_number(values[index]);
    }
    for (int32_t index = count - 1; index >= 0; index--) {
        (void)add_to_stack(values[index]);
    }
    return CALC_STATUS_OK;
}

void clear_stack(void)
{
    int32_t dummy = 0;
    while (!is_stack_empty()) {
        (void)pop_from_stack(&dummy);
    }
}

int32_t duplicate_top(void)
{
    int32_t value = 0;
    int32_t status = pop_from_stack(&value);
    if (status < CALC_STATUS_OK) {
        return status;
    }
    status = add_to_stack(value);
    if (status < CALC_STATUS_OK) {
        return status;
    }
    return add_to_stack(value);
}

int32_t swap_top_two(void)
{
    int32_t top = 0;
    int32_t below = 0;
    int32_t status = pop_from_stack(&top);
    if (status < CALC_STATUS_OK) {
        return status;
    }
    status = pop_from_stack(&below);
    if (status < CALC_STATUS_OK) {
        (void)add_to_stack(top);
        return status;
    }
    status = add_to_stack(top);
    if (status < CALC_STATUS_OK) {
        return status;
    }
    return add_to_stack(below);
}
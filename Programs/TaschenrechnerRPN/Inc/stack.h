#ifndef STACK_H
#define STACK_H
#include "status.h"
#include "stdint.h"
#include <stdbool.h>

/*
 ****************************************************************************************
 * @brief : adds an int32 on top of the stack
 *
 * @return Returns Status for adding
 ****************************************************************************************/
enum Status add_to_stack(int32_t number);

/*
 ****************************************************************************************
 * @brief : adds an writes last element on the stack to input pointer,
    and deletes last element from stack
 *
 * @return Returns Status for Popping
 ****************************************************************************************/
enum Status pop_from_stack(int32_t *ret);

/*
 ****************************************************************************************
 * @brief : Pops the two top entries of the stack.
 *
 * @param top   Receives the top entry. Must not be NULL.
 * @param below Receives the entry below the top. Must not be NULL.
 *
 * @retval CALC_STATUS_OK Both entries were popped.
 * @retval CALC_STATUS_STACK_EMPTY Fewer than two entries on the stack. The
 *         stack is unchanged.
 ****************************************************************************************/
enum Status pop_two(int32_t *top, int32_t *below);

/*
 ****************************************************************************************
 * @brief : sets position of stack_pointer to -1
 *
 * @return Returns nothing
 ****************************************************************************************/
void clear_stack(void);
#endif /* STACK_H */
       // EOF
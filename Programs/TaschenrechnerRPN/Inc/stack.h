#ifndef STACK_H
#define STACK_H
#include "stdint.h"
#include <stdbool.h>

/*
 ****************************************************************************************
 * @brief : checks if the stack is empty
 *
 * @return Returns true if empty and false if not empty
 ****************************************************************************************/
bool is_stack_empty(void);

/*
 ****************************************************************************************
 * @brief : adds an int32 on top of the stack
 *
 * @return Returns Status for adding
 ****************************************************************************************/
int32_t add_to_stack(int32_t);

/*
 ****************************************************************************************
 * @brief : adds an writes last element on the stack to input pointer,
    and deletes last element from stack
 *
 * @return Returns Status for Popping
 ****************************************************************************************/
int32_t pop_from_stack(int32_t*);

/*
 ****************************************************************************************
 * @brief : gets the element of the stack_pointer
 *
 * @return Returns element of the stack_pointer
 ****************************************************************************************/
int32_t get_last_stack_pos(void);

#endif /* SCANNER_H */
// EOF

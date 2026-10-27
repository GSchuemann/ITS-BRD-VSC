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
enum Status add_to_stack(int32_t);

/*
 ****************************************************************************************
 * @brief : adds an writes last element on the stack to input pointer,
    and deletes last element from stack
 *
 * @return Returns Status for Popping
 ****************************************************************************************/
enum Status pop_from_stack(int32_t *);

/*
 ****************************************************************************************
 * @brief : sets position of stack_pointer to -1
 *
 * @return Returns nothing
 ****************************************************************************************/
void clear_stack(void);
#endif /* SCANNER_H */
// EOF

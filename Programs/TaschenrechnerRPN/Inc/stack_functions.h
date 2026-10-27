#ifndef STACK_FUNCTIONS_H
#define STACK_FUNCTIONS_H

#include "status.h"
#include <stdint.h>

/**
 *****************************************************************************
 * @brief  Prints the n top entries, top entry first (commands p and P).
 *         The stack is not changed.
 *
 * @param  n Number of entries to print (1 for command p, 3 for command P).
 * @retval CALC_STATUS_OK Entries were printed. If the stack holds fewer than
 *         n entries, only the existing entries are printed.
 * @retval CALC_STATUS_STACK_EMPTY Stack is empty, nothing was printed.
 *****************************************************************************/
enum Status print_from_stack(uint8_t n);

/**
 *****************************************************************************
 * @brief  Removes all entries from the stack (command C).
 *
 * @retval void
 *****************************************************************************/
void clear_all(void);

/**
 *****************************************************************************
 * @brief  Duplicates the top entry (command d).
 *
 * @retval CALC_STATUS_OK Top entry was copied.
 * @retval CALC_STATUS_STACK_EMPTY Stack is empty.
 * @retval CALC_STATUS_STACK_FULL No space for the copy, the stack is
 *         unchanged.
 *****************************************************************************/
enum Status duplicate_top(void);

/**
 *****************************************************************************
 * @brief  Swaps the two top entries (command r).
 *
 * @retval CALC_STATUS_OK Entries were swapped.
 * @retval CALC_STATUS_STACK_EMPTY Fewer than two entries, the stack is
 *         unchanged.
 *****************************************************************************/
enum Status swap_top_two(void);

#endif /* STACK_FUNCTIONS_H */
		 // EOF
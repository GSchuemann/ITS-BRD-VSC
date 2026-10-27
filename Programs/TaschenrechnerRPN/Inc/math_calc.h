#ifndef MATH_CALC_H
#define MATH_CALC_H
#include <stdint.h>

/**
 *****************************************************************************
 * @brief  Adds the two top entries: result = below + top.
 *
 * @param  result Pointer to the variable that receives the result. Must not be
 *                 NULL.
 * @retval CALC_STATUS_OK Result was written to result.
 * @retval CALC_STATUS_OVERFLOW Sum is outside the int32 range.
 * @retval CALC_STATUS_STACK_EMPTY Fewer than two entries on the stack.
 * @note   Both operands are removed from the stack, even if an error occurs.
 *         The result is NOT pushed on the stack, the caller (main) does this.
 *****************************************************************************/
enum Status math_add(int32_t *result);

/**
 *****************************************************************************
 * @brief  Subtracts the top entry from the entry below: result = below - top.
 *
 * @param  result Pointer to the variable that receives the result. Must not be
 *                 NULL.
 * @retval CALC_STATUS_OK Result was written to result.
 * @retval CALC_STATUS_OVERFLOW Difference is outside the int32 range.
 * @retval CALC_STATUS_STACK_EMPTY Fewer than two entries on the stack.
 * @note   Both operands are removed from the stack, even if an error occurs.
 *         The result is NOT pushed on the stack, the caller (main) does this.
 *****************************************************************************/
enum Status math_sub(int32_t *result);

/**
 *****************************************************************************
 * @brief  Multiplies the two top entries: result = below * top.
 *
 * @param  result Pointer to the variable that receives the result. Must not be
 *                 NULL.
 * @retval CALC_STATUS_OK Result was written to result.
 * @retval CALC_STATUS_OVERFLOW Product is outside the int32 range.
 * @retval CALC_STATUS_STACK_EMPTY Fewer than two entries on the stack.
 * @note   Both operands are removed from the stack, even if an error occurs.
 *         The result is NOT pushed on the stack, the caller (main) does this.
 *****************************************************************************/
enum Status math_mul(int32_t *result);

/**
 *****************************************************************************
 * @brief  Divides the entry below by the top entry (integer division): result =
 *         below / top.
 *
 * @param  result Pointer to the variable that receives the result. Must not be
 *                 NULL.
 * @retval CALC_STATUS_OK Result was written to result.
 * @retval CALC_STATUS_DIV_BY_ZERO Top entry is 0.
 * @retval CALC_STATUS_OVERFLOW INT_MIN divided by -1 does not fit into int32.
 * @retval CALC_STATUS_STACK_EMPTY Fewer than two entries on the stack.
 * @note   Both operands are removed from the stack, even if an error occurs.
 *         The result is NOT pushed on the stack, the caller (main) does this.
 *****************************************************************************/
enum Status math_div(int32_t *result);

#endif /* MATH_CALC_H */
		 // EOF
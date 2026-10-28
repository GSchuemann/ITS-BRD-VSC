#ifndef STATUS_H
#define STATUS_H

enum Status {
   CALC_STATUS_OK = 0,              /**< Operation was successful. */
   CALC_STATUS_DIV_BY_ZERO = -1,    /**< Division by zero. */
   CALC_STATUS_OVERFLOW = -2,       /**< Result out of int32 range. */
   CALC_STATUS_STACK_FULL = -3,     /**< Push on a full stack. */
   CALC_STATUS_STACK_EMPTY = -4,    /**< Pop on an empty stack. */
   CALC_STATUS_INVALID_INPUT = -10, /**< Invalid operator or input format.*/
};

#endif /* STATUS_H */
// EOF

#ifndef STATUS_H
#define STATUS_H

<<<<<<< HEAD
#define CALC_STATUS_OK 0
#define CALC_STATUS_DIV_BY_ZERO (-1)
#define CALC_STATUS_OVERFLOW (-2)
#define CALC_STATUS_STACK_FULL (-3)
#define CALC_STATUS_STACK_EMPTY (-4)
=======
enum Status {
    CALC_STATUS_OK = 0,
    CALC_STATUS_ARITH_OVERFLOW = 1,
    CALC_STATUS_OVERFLOW = 2,
    CALC_INDEX_OUT_OF_BOUNDS = -1
};
>>>>>>> 4e72229 (before merge)

#endif /* STATUS_H */
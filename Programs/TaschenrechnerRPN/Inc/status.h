#ifndef STATUS_H
#define STATUS_H

enum Status {
    CALC_STATUS_OK = 0,
    CALC_STATUS_DIV_BY_ZERO = -1,
    CALC_STATUS_OVERFLOW = -2,
    CALC_STATUS_STACK_FULL = -3,
    CALC_STATUS_STACK_EMPTY = -4,
};

#endif /* STATUS_H */
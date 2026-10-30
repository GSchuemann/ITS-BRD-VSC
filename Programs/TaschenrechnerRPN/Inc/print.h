#ifndef PRINT_H
#define PRINT_H
#include "status.h"
#include <stdint.h>
/*
 ****************************************************************************************
 * @brief : Takes an error code and prints the corresponding Error Message
 *
 * @return Returns nothing
 ****************************************************************************************/
void print_err(enum Status err);
/*
 ****************************************************************************************
 * @brief : Get's a number and prints it
 *
 * @return Returns nothing
 ****************************************************************************************/
void print_num(int32_t num);
#endif
// EOF

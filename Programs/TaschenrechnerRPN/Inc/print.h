#ifndef PRINT_H
#define PRINT_H
#include <stdint.h>
#include "status.h"
/*
 ****************************************************************************************
 * @brief : Takes an error code and prints the corresponding Error Message
 *
 * @return Returns nothing
 ****************************************************************************************/
void print_err(enum Status);
/*
 ****************************************************************************************
 * @brief : Get's a number and prints it
 *
 * @return Returns nothing
 ****************************************************************************************/
void print_num(int32_t);
/*
 ****************************************************************************************
 * @brief : Get's a number and prints it, also clears stdozut before printing
 *
 * @return Returns nothing
 ****************************************************************************************/
void print_num_with_clear(int32_t);
#endif 
// EOF

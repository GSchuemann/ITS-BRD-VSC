/**
 * @file numbers.h
 * @author Xuan Hoang Duy, Georg Schümann
 * @date Okt 2026
 * @brief Header file of input module.
 */

#ifndef NUMBERS_H
#define NUMBERS_H


#include <stdint.h>
/**
 * Struct for Big Integers. Integers are stored with least significant 4bytes first. 4 Bytes are later described as partNum. The size Elements of the Number are limited to uint_8 (255).

The Sign byte is used to indicate if the number is negative or positive!
 */

typedef struct {
uint8_t size;
uint8_t capacity;
uint32_t *number;
uint8_t sign; 
} bigInt;

/*
 ****************************************************************************************
 * @brief : This function allocats memory (up to 255 * 4 bytes) for the bigInt1
 *
 * @return status, either succes or if there is enough memory allocated, a different success message, or error if the allocation failed
 ****************************************************************************************/
uint32_t alloc_big_int(bigInt *n, uint8_t new_capacity);
/*
 ****************************************************************************************
 * @brief : This functions frees the allocated memeory of a bigInt and zeros it's fields. The BigInt itself persistens and can be reused without reallocation
 *
 * @return void
 ****************************************************************************************/
void free_big_int(bigInt *n);
/*
 ****************************************************************************************
 * @brief : This function adds a partNum to the bigInt
 *
 * @return Status
 ****************************************************************************************/
uint32_t push_part_numb(bigInt *n, uint32_t part_num);
/*
 ****************************************************************************************
 * @brief : This function prrints an infinite Precision Integer
 *
 * @return void
 ****************************************************************************************/
void big_int_print(bigInt *n);
// T_token next_token(void);

#endif /* SCANNER_H */
// EOF

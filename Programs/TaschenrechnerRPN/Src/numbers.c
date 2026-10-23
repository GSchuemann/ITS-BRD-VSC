#include "../Inc/numbers.h"
#include "../Inc/status.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
uint32_t alloc_big_int(bigInt *n, uint8_t new_capacity){
    if(n->capacity > new_capacity){
        return CALC_STATUS_NO_ACTION;
    }
    if(new_capacity > UINT8_MAX){
        return CALC_STATUS_OVERFLOW;
    }
    uint32_t *ptr = realloc(n->number, new_capacity *  sizeof(n->number)); 
    if(ptr == NULL){
        return CALC_STATUS_OVERFLOW;
    }
    n->number = ptr;
    n->capacity = new_capacity;

    return CALC_STATUS_OK;
};
uint32_t push_part_numb(bigInt *n, uint32_t part_num){
    
    if(n->size == n->capacity){
        uint32_t status = alloc_big_int(n, n->capacity+1);
        if(status < CALC_STATUS_OK){
            return status;
        }
        n->number[n->size++] = part_num; //Be aware of the ++ after size. It needs to be assigned and then incremented.
    }
    
    return CALC_STATUS_OK;
}
void free_big_int(bigInt *n)
{
    free(n->number);
    n->number = NULL;
    n->size = 0;
    n->capacity = 0;
}
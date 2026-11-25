#ifndef PICTURE_OUT_H
#define PICTURE_OUT_H
#include <stdint.h>

extern void picture_out_pixel(int x, int y, uint16_t color);
void picture_out_row(int y, uint16_t row[] );
#endif
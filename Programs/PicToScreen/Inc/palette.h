#ifndef PALETTE_H
#define PALETTE_H

#include "BMP_types.h"

extern int palette_entry_count(const BITMAPINFOHEADER *info_hdr_p);
extern int palette_read(const BITMAPINFOHEADER *info_hdr_p);
extern int palette_get_color(uint8_t index, uint16_t *color_p);

#endif
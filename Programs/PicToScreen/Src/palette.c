#include "palette.h"
#define BITS_PER_PIXEL_24 24

int palette_entry_count(const BITMAPINFOHEADER *info_hdr_p) {
   if (BITS_PER_PIXEL_24 == info_hdr_p->biBitCount) {
      return 0;
   }
   if (0 == info_hdr_p->biClrUsed) {
      return MAX_COLOR_TABLE_SIZE;
   }
   return (int)info_hdr_p->biClrUsed;
}
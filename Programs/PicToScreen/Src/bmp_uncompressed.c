#include "bmp_uncompressed.h"
#include "errorhandler.h"
#include "picture_out.h"
#include "input.h"
#include "palette.h"
#include <stdio.h>

#define BITS_PER_PIXEL 8
#define ROW_ALIGN_BITS 32
#define ROW_ALIGN_BYTES 4

int bmp_uncompressed_draw(const BITMAPINFOHEADER *info_hdr_p) {
   CHECK_OR_RETURN((BITS_PER_PIXEL == info_hdr_p->biBitCount) && (BI_RGB == info_hdr_p->biCompression),
                   UNSUPPORTED_FORMAT_ERR);

   int width = (int)info_hdr_p->biWidth;
   int height = (int)info_hdr_p->biHeight;
   int row_bytes = ((width * BITS_PER_PIXEL + (ROW_ALIGN_BITS - 1)) / ROW_ALIGN_BITS) * ROW_ALIGN_BYTES;
   int padding = row_bytes - width;
   uint16_t color;

   for (int row = 0; row < height; row++) {
      for (int x = 0; x < width; x++) {
         int index = nxt_chr();
         CHECK_OR_RETURN(EOF != index, READ_ERR);
         RETURN_ON_ERROR(palette_get_color((uint8_t)index, &color));
         picture_out_pixel(x, height - 1 - row, color);
      }
      for (int p = 0; p < padding; p++) {
         CHECK_OR_RETURN(EOF != nxt_chr(), READ_ERR);
      }
   }
   return EOK;
}
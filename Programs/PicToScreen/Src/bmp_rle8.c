#include "bmp_rle8.h"
#include "errorhandler.h"
#include "input.h"
#include "palette.h"
#include "picture_out.h"
#include <stdio.h>

#define BITS_PER_PIXEL 8
#define ESC_END_OF_LINE 0
#define ESC_END_OF_BITMAP 1
#define ESC_DELTA 2
#define ODD_RUN_PADDING 2

static int read_byte(int *value_p) {
   int value = nxt_chr();

   CHECK_OR_RETURN(EOF != value, READ_ERR);
   *value_p = value;
   return EOK;
}

static int put_pixel(int x, int row, int index, int height) {
   uint16_t color;

   RETURN_ON_ERROR(palette_get_color((uint8_t)index, &color));
   picture_out_pixel(x, height - 1 - row, color);
   return EOK;
}

static int draw_run(int x, int row, int count, int index, const BITMAPINFOHEADER *info_hdr_p) {
   int width = (int)info_hdr_p->biWidth;
   int height = (int)info_hdr_p->biHeight;

   CHECK_OR_RETURN((row < height) && (x + count <= width), FORMAT_ERR);
   for (int i = 0; i < count; i++) {
      RETURN_ON_ERROR(put_pixel(x + i, row, index, height));
   }
   return EOK;
}

static int draw_absolute(int x, int row, int count, const BITMAPINFOHEADER *info_hdr_p) {
   int width = (int)info_hdr_p->biWidth;
   int height = (int)info_hdr_p->biHeight;
   int index;

   CHECK_OR_RETURN((row < height) && (x + count <= width), FORMAT_ERR);
   for (int i = 0; i < count; i++) {
      RETURN_ON_ERROR(read_byte(&index));
      RETURN_ON_ERROR(put_pixel(x + i, row, index, height));
   }
   if (0 != (count % ODD_RUN_PADDING)) {
      RETURN_ON_ERROR(read_byte(&index));
   }
   return EOK;
}

int bmp_rle8_draw(const BITMAPINFOHEADER *info_hdr_p) {
   CHECK_OR_RETURN((BITS_PER_PIXEL == info_hdr_p->biBitCount) && (BI_RLE8 == info_hdr_p->biCompression),
                   UNSUPPORTED_FORMAT_ERR);

   int width = (int)info_hdr_p->biWidth;
   int x = 0;
   int row = 0;
   int count;
   int value;
   int delta_x;
   int delta_y;

   for (;;) {
      RETURN_ON_ERROR(read_byte(&count));
      RETURN_ON_ERROR(read_byte(&value));
      if (0 != count) {
         RETURN_ON_ERROR(draw_run(x, row, count, value, info_hdr_p));
         x += count;
      } else if (ESC_END_OF_LINE == value) {
         CHECK_OR_RETURN(x == width, FORMAT_ERR);
         x = 0;
         row++;
      } else if (ESC_END_OF_BITMAP == value) {
         return EOK;
      } else if (ESC_DELTA == value) {
         RETURN_ON_ERROR(read_byte(&delta_x));
         RETURN_ON_ERROR(read_byte(&delta_y));
         x += delta_x;
         row += delta_y;
      } else {
         RETURN_ON_ERROR(draw_absolute(x, row, value, info_hdr_p));
         x += value;
      }
   }
}
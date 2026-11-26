#include "bmp_rle8.h"
#include "errorhandler.h"
#include "input.h"
#include "palette.h"
#include "picture_out.h"
#include "stm32f4xx.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define BITS_PER_PIXEL 8
#define ESC_END_OF_LINE 0
#define ESC_END_OF_BITMAP 1
#define ESC_DELTA 2
#define ODD_RUN_PADDING 2
#define WIDTH 480
#define HEIGHT 320

static int read_byte(int *value_p) {
   int value = nxt_chr();

   CHECK_OR_RETURN(EOF != value, READ_ERR);
   *value_p = value;
   return EOK;
}

static uint16_t pixel_row[WIDTH] = {};
static uint16_t uncompressed_pixel_row[10000] = {};
static uint16_t uncompressed_pixel_row_blurred[10000] = {};

static int put_pixel(int x, int index,int compressed) {
   uint16_t color;

   RETURN_ON_ERROR(palette_get_color((uint8_t)index, &color));
   if(compressed){
      uncompressed_pixel_row[x] = color;
   } else if(x < WIDTH){
      pixel_row[x] = color;
   }
   return EOK;
}
static int put_pixel_color(int x, uint16_t pixel) {
   if(x < WIDTH) {
         pixel_row[x] = pixel;

   }
   return  EOK;
}

static int draw_run(int x, int row, int count, int index, const BITMAPINFOHEADER *info_hdr_p, int compressed) {
   int width = (int)info_hdr_p->biWidth;
   int height = (int)info_hdr_p->biHeight;

   CHECK_OR_RETURN( (x + count <= width), FORMAT_ERR);
   for (int i = 0; i < count; i++) {
      RETURN_ON_ERROR(put_pixel(x + i,  index,compressed));
   }
   return EOK;
}
uint16_t get_color(int x, int length){
   if(x < length) {
      return uncompressed_pixel_row[0];
   } 
   if(x >= length) {
      return uncompressed_pixel_row[length -1];
   }
      return uncompressed_pixel_row[x];
   
}
void compress_row( int radius, int length) {
   uint16_t buffer[radius];
   const int row_start = 0;
   int dif = 0;
   int sum = 0;
   for(int x = row_start -2 * radius ;x < length; x++) {
      sum += dif;
      dif += get_color(x+radius, length);
      if(x >= row_start) {
         dif += buffer[x%radius];
         uncompressed_pixel_row_blurred[x] = sum/(radius * radius);
         uint16_t p = get_color( x, length);
         buffer[x%radius] = p;
         dif -= 2*p;
      }
   }
   int i = 0;
   for(int x = 0; x < length; x++) {
         if(x%radius == 0)  {
            put_pixel_color(i++, uncompressed_pixel_row[x]);
         }
   }
   for(;i <WIDTH;i++){
      put_pixel_color(i, 0x0000);
   }
}
static int draw_absolute(int x, int row, int count, const BITMAPINFOHEADER *info_hdr_p,int compressed) {
   int width = (int)info_hdr_p->biWidth;
   int height = (int)info_hdr_p->biHeight;
   int index;

   CHECK_OR_RETURN( (x + count <= width), FORMAT_ERR);
   for (int i = 0; i < count; i++) {
      RETURN_ON_ERROR(read_byte(&index));
      RETURN_ON_ERROR(put_pixel(x + i,  index,compressed));
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
   int height = (int)info_hdr_p->biHeight;

   int x = 0;
   int row = 320;
   int count;
   int value;
   int delta_x;
   int delta_y;
   int diff_x = (int)round(((double)width/ (double)WIDTH) + 0.46);
   int diff_y = (int)round(((double)height / (double)HEIGHT) +0.46);
   int compression_faktor = diff_x > diff_y ? diff_x : diff_y;
   if(compression_faktor % 2 == 0) {
      compression_faktor += 1;
   }

   for (;;) {
      RETURN_ON_ERROR(read_byte(&count));
      RETURN_ON_ERROR(read_byte(&value));
      if (0 != count) {
         RETURN_ON_ERROR(draw_run(x, row, count, value, info_hdr_p,compression_faktor != 1 ? 1 : 0));
         x += count;
      } else {
         switch (value) {
         case ESC_END_OF_LINE:
            CHECK_OR_RETURN(x == width, FORMAT_ERR);
            x = 0;
            if(compression_faktor != 1){
               compress_row(compression_faktor, width);
            }
            picture_out_row(row, pixel_row);
            row--;
            break;
         case  ESC_END_OF_BITMAP:
            return EOK;
         case ESC_DELTA:
            RETURN_ON_ERROR(read_byte(&delta_x));
            RETURN_ON_ERROR(read_byte(&delta_y));
            x += delta_x;
            if(compression_faktor != 1){
               compress_row(compression_faktor, width);
            }
            picture_out_row(row, pixel_row);
            row -= delta_y;
            break;
         default:
            RETURN_ON_ERROR(draw_absolute(x, row, value, info_hdr_p,compression_faktor != 1 ? 1 : 0));
            x += value;
         }
      }
   }

}
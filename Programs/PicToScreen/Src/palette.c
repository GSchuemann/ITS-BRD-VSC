#include "palette.h"
#include "errorhandler.h"
#include "input.h"
#include "rgb565.h"

#define BITS_PER_PIXEL_24 24

static uint16_t palette[MAX_COLOR_TABLE_SIZE];
static int entry_count = 0;

int palette_entry_count(const BITMAPINFOHEADER *info_hdr_p) {
   if (BITS_PER_PIXEL_24 == info_hdr_p->biBitCount) {
      return 0;
   }
   if (0 == info_hdr_p->biClrUsed) {
      return MAX_COLOR_TABLE_SIZE;
   }
   return (int)info_hdr_p->biClrUsed;
}

int palette_read(const BITMAPINFOHEADER *info_hdr_p) {
   RGBQUAD entry;
   int count = palette_entry_count(info_hdr_p);

   entry_count = 0;
   for (int i = 0; i < count; i++) {
      CHECK_OR_RETURN(1 == read_file((char *)&entry, sizeof(entry), 1),
                      READ_ERR);
      palette[i] = rgb_to_rgb565(entry.rgbRed, entry.rgbGreen, entry.rgbBlue);
   }
   entry_count = count;
   return EOK;
}

int palette_get_color(uint8_t index, uint16_t *color_p) {
   CHECK_OR_RETURN(index < entry_count, INDEX_ERR);
   *color_p = palette[index];
   return EOK;
}
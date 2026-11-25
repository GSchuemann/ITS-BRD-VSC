#include "picture_out.h"
#include "LCD_GUI.h"
#include "LCD_general.h"

void picture_out_pixel(int x, int y, uint16_t color) {
   Coordinate crd;

   if ((x < 0) || (x >= LCD_WIDTH) || (y < 0) || (y >= LCD_HEIGHT)) {
      return;
   }
   crd.x = (uint16_t)x;
   crd.y = (uint16_t)y;
   GUI_drawPoint(crd, color, DOT_PIXEL_1X1, DOT_FILL_AROUND);
}
void picture_out_row(int y, uint16_t row[] ) {
   Coordinate crd;
   crd.x = 0;
   crd.y = y;
   GUI_WriteLine(crd,480,row);
}
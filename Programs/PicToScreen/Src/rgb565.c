#include "rgb565.h"
#define RED_SHRINK 3
#define GREEN_SHRINK 2
#define BLUE_SHRINK 3
#define RED_SHIFT 11
#define GREEN_SHIFT 5

uint16_t rgb_to_rgb565(uint8_t red, uint8_t green, uint8_t blue) {
   uint16_t red_val = (uint16_t)((red >> RED_SHRINK) << RED_SHIFT);
   uint16_t green_val = (uint16_t)((green >> GREEN_SHRINK) << GREEN_SHIFT);
   uint16_t blue_val = (uint16_t)(blue >> BLUE_SHRINK);

   return (uint16_t)(red_val | green_val | blue_val);
}
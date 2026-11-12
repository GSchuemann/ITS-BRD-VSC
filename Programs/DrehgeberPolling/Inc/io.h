#include <stdint.h>

#ifndef io_h
#define io_h
/*
 ****************************************************************************************
 *  @brief      This functions reads all butons and saves them on a local
 *variable.
 * *
 *  @return     void
 ****************************************************************************************/
uint32_t read_buttons(void);
/*
 ****************************************************************************************
 *  @brief      This function returns the set buttons from the local variable.
 * *
 *  @return     buttons (uint32t_t)
 ****************************************************************************************/

void clr_e_led(uint32_t leds);
/*
 ****************************************************************************************
 *  @brief      This clears the D LEDs
 *
 *  @param      leds to be cleared
 *
 *  @return     void
 ****************************************************************************************/
void clr_d_led(uint32_t leds);
/*
 ****************************************************************************************
 *  @brief      This sets the E LEDs
 *
 *  @param      leds to be set
 *
 *  @return     void
 ****************************************************************************************/
void set_e_led(uint32_t leds);
/*
 ****************************************************************************************
 *  @brief      This sets the D LEDs
 *
 *  @param      leds to be set
 *
 *  @return     void
 ****************************************************************************************/
void set_d_led(uint32_t leds);

#endif
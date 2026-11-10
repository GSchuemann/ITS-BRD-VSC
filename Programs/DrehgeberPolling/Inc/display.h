#ifndef DISPLAY_H
#define DISPLAY_H
/*
 ****************************************************************************************
 *  @brief      This function sets up the screen with the already known content
 * *
 *  @return     void
 ****************************************************************************************/
void init_display(void);
/*
 ****************************************************************************************
 *  @brief      This function prints the velocity and degrees to the screen. The
 *refresh is max every 300ms. The refresh is done incremental, on char at a time
 * *
 *  @return     void
 ****************************************************************************************/
void print_values(void);

#endif
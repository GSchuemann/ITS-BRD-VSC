#ifndef phase_counter
#define phase_counter
/*
 ****************************************************************************************
 *  @brief      This functions sets the internal degree variable to 0
 * *
 *  @return     void
 ****************************************************************************************/
void reset_degree(void);
/*
 ****************************************************************************************
 *  @brief      This functions sets the internal velocity variable to 0.0
 * *
 *  @return     void
 ****************************************************************************************/
void reset_velocity(void);
/*
 ****************************************************************************************
 *  @brief      This function increments the degree by 0.3. And also set's the D
 *LED to phase counter-
 * *
 *  @return     void
 ****************************************************************************************/
void add_degree(void);
/*
 ****************************************************************************************
 *  @brief      This function decrements the degree by 0.3. And also clears the
 *D LED, so that only phase_counter is set
 * *
 *  @return     void
 ****************************************************************************************/
void sub_degree(void);
/*
 ****************************************************************************************
 *  @brief      This function returns the degrees, that were rotated.
 * *
 *  @return     void
 ****************************************************************************************/
double get_degree(void);
/*
 ****************************************************************************************
 *  @brief      This function returns the velocity, which was used for the last
 *rotation.
 * *
 *  @return     void
 ****************************************************************************************/
double get_velocity(void);
#endif
/**
 * @file scanner.h
 * @author Xuan Hoang Duy, Georg Schümann
 * @date Okt 2026
 * @brief Header file of input module.
 */

#ifndef SCANNER_H
#define SCANNER_H

#include "token.h"

/*
 ****************************************************************************************
 * @brief : This function returns the next token, defined in token.h
 *
 * @return Next token will be returned. In case of an error the UNEXPTECTED
 * token will be returned.
 ****************************************************************************************/
T_token next_token(void);

#endif /* SCANNER_H */
// EOF

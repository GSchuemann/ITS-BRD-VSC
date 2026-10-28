/**
 ******************************************************************************
 * @file    main.c
 * @author  Franz Korf
 * @brief   Kleines Testprogramm fuer neu erstelle Fonts.
 ******************************************************************************
 */
/* Includes ------------------------------------------------------------------*/

// #include "LCD_GUI.h"
// #include "LCD_Touch.h"
#include "init.h"
// #include "lcd.h"
#include "../Inc/display.h"
#include "../Inc/math_calc.h"
#include "../Inc/print.h"
#include "../Inc/scanner.h"
#include "../Inc/stack.h"
#include "../Inc/stack_functions.h"
#include "LCD_GUI.h"
#include "display.h"
#include "lcd.h"
#include "stack_functions.h"
#include "status.h"
// #include <stdint.h>
#include "../Inc/test.h"
#include "token.h"
// #include "../Inc/stack.h"
#include "LCD_Touch.h"
#define Y_CENTER 9
#define X_CENTER 8
#define Y_BOTTOM 18
#define Y_BOTTOM_RIGHT 25
#define BUTTON_S0_PIN 1U      // Pin 1 for S0 (Skip)
#define BUTTON_S7_PIN 1U << 7 // Pin 8 for S7 (Continue)

/*
 ****************************************************************************************
 * @brief : math Helper function, takes a token and calls the corresponding
 *function
 *
 * @return Returns Status
 ****************************************************************************************/
enum Status math_helper(T_token tok) {
	enum Status status = CALC_STATUS_OK;

	int32_t result;
	switch (tok.tok) {
	case PLUS:
		status = math_add(&result);
		break;
	case MINUS:
		status = math_sub(&result);
		break;
	case MULT:
		status = math_mul(&result);
		break;
	case DIV:
		status = math_div(&result);
		break;
	default:
		return CALC_STATUS_INVALID_INPUT;
	}
	if (status < CALC_STATUS_OK) {
		return status;
	}
	status = add_to_stack(result);
	if (status < CALC_STATUS_OK) {
		return status;
	}
	return status;
}
static uint8_t read_all_buttons(void) {
#define GPIOF_BASE 0x40021400
#define BASE_OFFSET 0x10
	volatile uint32_t *gpiof_in =
		 (volatile uint32_t *)(GPIOF_BASE + BASE_OFFSET);

	return *(uint8_t *)gpiof_in; // Niedrigste 8 Bits zurückgeben
}
int main(void) {
	initITSboard();               // Initialisierung des ITS Board
	GUI_init(DEFAULT_BRIGHTNESS); // Initialisierung des LCD Boards mit Touch
	TP_Init(false);               // Initialisierung des LCD Boards mit Touch
#ifdef DEBUG                     // only IN DEBUG BUILD
	lcdGotoXY(X_CENTER, Y_CENTER);
	lcdPrintlnS("Tests laufen lassen?");
	lcdGotoXY(0, Y_BOTTOM);
	lcdPrintlnS("S7 Weiter");
	lcdGotoXY(Y_BOTTOM_RIGHT, Y_BOTTOM);
	lcdPrintlnS("S0 Ueberpsringen");
	while (1) {
		uint8_t buttons =
			 read_all_buttons(); // Do this in Main loop, in other cases the button
		                        // reads could go onto the stack.
		uint8_t s0pressed = (buttons & (BUTTON_S0_PIN)) == 0;
		uint8_t s7pressed = (buttons & (BUTTON_S7_PIN)) == 0;
		if (s0pressed) {
			break; // continue Normal
		}
		if (s7pressed) {
			test(); // Run the tests, the output is sent via UART, on linux screen
			        // /dev/ttyACM0 115200 can be used to read //TODO nice
			        // formatting in Tests
			break;
			;
		}
	}
#endif // DEBUG
	init_display();
	enum Status status = CALC_STATUS_OK;
	int clear = 1;
	while (1) {
		T_token tok = next_token(
			 clear); // Switch case to handle the different possible tokens
		clear = 1;
		switch (tok.tok) {
		case CLEAR:
			clear_all();
			status = CALC_STATUS_OK;
			break;
		case OVERFLOW:
			status = CALC_STATUS_OVERFLOW;
			break;
		case NUMBER:
			status = add_to_stack(tok.val);
			break;
		case PLUS: // Fall through to code in div, math is handled in a helper
		           // function
		case MINUS:
		case MULT:
		case DIV:
			status = math_helper(tok);
			break;
		case PRT:
			status = print_from_stack(1);
			clear = 0;
			break;
		case SWAP:
			status = swap_top_two();
			break;
		case PRT_ALL:
			status = print_from_stack(3);
			clear = 0;
			break;
		case DOUBLE:
			status = duplicate_top();
			break;
		case UNEXPECTED: // same as default
		default:
			status = CALC_STATUS_INVALID_INPUT;
		}
		if (status < CALC_STATUS_OK) {
			print_err(status);
		}
	}
	return 0;
}

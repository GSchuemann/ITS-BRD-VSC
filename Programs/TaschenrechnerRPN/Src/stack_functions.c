#include "../Inc/stack_functions.h"
#include "../Inc/display.h"
#include "../Inc/print.h"
#include "../Inc/stack.h"
#include "../Inc/status.h"
#include <stdint.h>
#include <stdio.h>

#define PRINT_BUFFER_SIZE 16
#define PRINT_COUNT 3

enum Status print_from_stack(uint8_t n) {
	int32_t values[n];
	for (uint8_t i = 0; i < n; i++) {
		int32_t status = pop_from_stack(&values[i]);
		if (status < CALC_STATUS_OK) {
			return status;
		}
	}
	for (uint8_t i = 0; i < n; i++) {
		print_num(values[i]);
		int32_t status = add_to_stack(values[i]);
		if (status < CALC_STATUS_OK) {
			return status;
		}
	}
	return CALC_STATUS_OK;
}

void clear_all(void) {
	clear_stack();
	set_normal_mode();
}

enum Status duplicate_top(void) {
	int32_t value = 0;
	int32_t status = pop_from_stack(&value);
	if (status < CALC_STATUS_OK) {
		return status;
	}
	status = add_to_stack(value);
	if (status < CALC_STATUS_OK) {
		return status;
	}
	return add_to_stack(value);
}

enum Status swap_top_two(void) {
	int32_t top = 0;
	int32_t below = 0;
	int32_t status = pop_from_stack(&top);
	if (status < CALC_STATUS_OK) {
		return status;
	}
	status = pop_from_stack(&below);
	if (status < CALC_STATUS_OK) {
		(void)add_to_stack(top);
		return status;
	}
	status = add_to_stack(top);
	if (status < CALC_STATUS_OK) {
		return status;
	}
	return add_to_stack(below);
}
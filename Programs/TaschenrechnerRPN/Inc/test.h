#ifndef TEST_H
#define TEST_H

/**
 *****************************************************************************
 * @brief  Tests the operations + - * / including error cases and prints pass or
 *         fail for each test.
 *
 * @return 0 after all tests were executed.
 *****************************************************************************/
int test(void);

/**
 *****************************************************************************
 * @brief  Tests the stack commands p, P, C, d and r including error cases and
 *         prints pass or fail for each test.
 *
 * @return 0 after all tests were executed.
 * @note   Uses print_stdout. On the PC compile with -DPC_TEST, which provides a
 *         replacement for the display module.
 *****************************************************************************/
int test_stack_functions(void);

#endif /* TEST_H */
       // EOF
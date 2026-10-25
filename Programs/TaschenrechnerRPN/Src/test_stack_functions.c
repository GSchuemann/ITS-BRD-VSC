#include "../Inc/display.h"
#include "../Inc/math_calc.h"
#include "../Inc/stack.h"
#include "../Inc/stack_functions.h"
#include "../Inc/status.h"
#include "../Inc/test.h"
#include <stdio.h>

#ifdef PC_TEST
void print_stdout(char *msg) { printf("%s", msg); }
#endif

int test_stack_functions(void) {

   int result = 0;
   int second = 0;
   int status = 0;

   printf("\n===== Tests(p P C d r) =====\n\n");
   clear_stack();

   /* Test: 5 d -> 5 5 */
   printf("Test(d): (5 d)\n");
   add_to_stack(5);
   status = duplicate_top();
   pop_from_stack(&result);
   pop_from_stack(&second);
   printf("result %d\nsecond %d\nstatus %d\n", result, second, status);
   printf("status: %s\n\n",
          (result == 5 && second == 5 && status == 0 && is_stack_empty())
              ? "pass"
              : "fail");

   /* Test: 1 2 r -> 2 1 (oben 1) */
   printf("Test(r): (1 2 r)\n");
   add_to_stack(1);
   add_to_stack(2);
   status = swap_top_two();
   pop_from_stack(&result);
   pop_from_stack(&second);
   printf("result %d\nsecond %d\nstatus %d\n", result, second, status);
   printf("status: %s\n\n",
       (result == 1 && second == 2 && status == 0 && is_stack_empty())
              ? "pass"
              : "fail");

   /* Test: 1 2 3 C -> Stack leer */
   printf("Test(C): (1 2 3 C)\n");
   add_to_stack(1);
   add_to_stack(2);
   add_to_stack(3);
   clear_stack();
   printf("empty %d\n", is_stack_empty());
   printf("status: %s\n\n", is_stack_empty() ? "pass" : "fail");

   /* Test: 7 p -> gibt 7 aus, Stack unveraendert */
   printf("Test(p): (7 p) (expected output: 7)\n");
   add_to_stack(7);
   status = print_from_stack(1);
   pop_from_stack(&result);
   printf("result %d\nstatus %d\n", result, status);
   printf("status: %s\n\n",
          (result == 7 && status == 0 && is_stack_empty()) ? "pass" : "fail");

   /* Test: 1 2 3 4 P -> gibt 4 3 2 aus, Stack unveraendert */
   printf("Test(P): (1 2 3 4 P) (expected output: 4 3 2)\n");
   add_to_stack(1);
   add_to_stack(2);
   add_to_stack(3);
   add_to_stack(4);
   status = print_from_stack(3);
   pop_from_stack(&result);
   pop_from_stack(&second);
   printf("result %d\nsecond %d\nstatus %d\n", result, second, status);
   printf("status: %s\n\n",
          (result == 4 && second == 3 && status == 0) ? "pass" : "fail");
   clear_stack();

   /* Test: p auf leerem Stack */
   printf("Test(p): empty stack\n");
   status = print_from_stack(1);
   printf("Status: %d (expected: -4)\n", status);
   printf("Status: %s\n\n",
          (status == CALC_STATUS_STACK_EMPTY) ? "pass" : "fail");

   /* Test: P auf leerem Stack */
   printf("Test(P): empty stack\n");
   status = print_from_stack(3);
   printf("Status: %d (expected: -4)\n", status);
   printf("Status: %s\n\n",
          (status == CALC_STATUS_STACK_EMPTY) ? "pass" : "fail");

   /* Test: d auf leerem Stack */
   printf("Test(d): empty stack\n");
   status = duplicate_top();
   printf("Status: %d (expected: -4)\n", status);
   printf("Status: %s\n\n",
          (status == CALC_STATUS_STACK_EMPTY) ? "pass" : "fail");

   /* Test: r auf leerem Stack */
   printf("Test(r): empty stack\n");
   status = swap_top_two();
   printf("Status: %d (expected: -4)\n", status);
   printf("Status: %s\n\n",
          (status == CALC_STATUS_STACK_EMPTY) ? "pass" : "fail");

   /* Test: r mit nur einem Wert, der Wert bleibt erhalten */
   printf("Test(r): only one value (5 r)\n");
   add_to_stack(5);
   status = swap_top_two();
   pop_from_stack(&result);
   printf("Status: %d (expected: -4)\nresult %d (expected: 5)\n", status,
          result);
   printf("Status: %s\n\n",
          (status == CALC_STATUS_STACK_EMPTY && result == 5) ? "pass" : "fail");

   /* Test: d auf vollem Stack (100 Werte) */
   printf("Test(d): full stack\n");
   for (int index = 0; index < 100; index++) {
      add_to_stack(index);
   }
   status = duplicate_top();
   pop_from_stack(&result);
   printf("Status: %d (expected: -3)\nresult %d (expected: 99)\n", status,
          result);
   printf("Status: %s\n\n",
          (status == CALC_STATUS_STACK_FULL && result == 99) ? "pass" : "fail");
   clear_stack();

   /* Test: Beispiel aus dem Aufgabenblatt: 25 8 67 + r / p -> 3 */
   printf("Test: 25 8 67 + r / p (expected output: 3)\n");
   add_to_stack(25);
   add_to_stack(8);
   add_to_stack(67);
   math_add(&result);
   add_to_stack(result);
   swap_top_two();
   math_div(&result);
   add_to_stack(result);
   status = print_from_stack(1);
   pop_from_stack(&result);
   printf("result %d\nstatus %d\n", result, status);
   printf("status: %s\n\n", (result == 3 && status == 0) ? "pass" : "fail");
   clear_stack();

   return 0;
}
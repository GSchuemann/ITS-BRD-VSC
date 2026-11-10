#include <stdint.h>

#include "io.h"
#include <phases.h>
#include "phase_counter.h"
#include "display.h"
#include "timing.h"
#define BUTTON_S6 6
#define LED_FORWARD 1U << 7
#define LED_BACKWARD 1U << 6
#define LED_ERROR 1U << 5

phase_fn phase_ops[] = {
    phase_a, phase_b, phase_c, phase_d, a_to_b, a_to_d, b_to_a,
    b_to_c,  c_to_b,  c_to_d,  d_to_c,  d_to_a, error,  init,
};
struct Phase {
    uint8_t direction;
    uint32_t counter;
    uint64_t timestamp;
};
static void forward(){
   add_degree();
   clr_e_led(LED_BACKWARD);
   set_e_led(LED_FORWARD);
}
static void backward(){
   sub_degree();
   clr_e_led(LED_FORWARD);
   set_e_led(LED_BACKWARD);
}
static void output_vals(){
   print_values(get_degree(),get_speed());
}
enum NextPhase phase_a() {
   output_vals();
   switch (get_buttons() &3) {
        case 0:
            return  A;
        case 1:
            return ATOB;
        case 2:
            return ATOD;
        default: return ERRORPHASE;
    }
}
enum NextPhase phase_b() {
   output_vals();
   switch (get_buttons() & 3) {
   case 1:
      return B;
   case 3:
      return BTOC;
   case 0:
      return BTOA;
   default:
      return ERRORPHASE;
   }
}
enum NextPhase phase_c() {
   output_vals();
   switch (get_buttons() & 3) {
   case 3:
      return C;
   case 2:
      return CTOD;
   case 1:
      return CTOB;
   default:
      return ERRORPHASE;
   }
}

enum NextPhase phase_d() {
   output_vals();
   switch (get_buttons() & 3) {
   case 2:
      return D;
   case 0:
      return DTOA;
   case 3:
      return DTOC;
   default:
      return ERRORPHASE;
   }
}
enum NextPhase a_to_b() {
   forward();
    return B;
}
enum NextPhase a_to_d() {
    backward();
    return D;
}
enum NextPhase b_to_c() {
    forward();
    return C;
}
enum NextPhase b_to_a() {
    backward();
    return A;
}
enum NextPhase c_to_b() {
   backward();
    return B;
}
enum NextPhase c_to_d() {
    forward();
    return D;
}
enum NextPhase d_to_a() {
    forward();
    return A;
}
enum NextPhase d_to_c() {
    backward();
    return C;
}

enum NextPhase error() {
   clr_e_led((LED_FORWARD | LED_BACKWARD));
   set_e_led(LED_ERROR);
   switch ((get_buttons() >> BUTTON_S6) & 1) {
   case 1:
      return ERRORPHASE;
   default:
      return INIT;
   }
}

enum NextPhase init() {
   clr_e_led(LED_ERROR);
   clr_d_led(0xFF);
   reset_degree();
   reset_velocity();
   output_vals();
    switch (get_buttons() & 3) {
    case 0:
       return A;
    case 1:
       return B;
    case 3:
       return C;
    case 2:
       return D;
    default:
       return ERRORPHASE;
    }
}
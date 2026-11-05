#include <stdint.h>

#include "io.h"
#include <phases.h>
#include "phase_counter.h"
#define BUTTON_S6 6

phase_fn phase_ops[] = {
    phase_a, phase_b, phase_c, phase_d, a_to_b, a_to_d, b_to_a,
    b_to_c,  c_to_b,  c_to_d,  d_to_c,  d_to_a, error,  init,
};
struct Phase {
    uint8_t direction;
    uint32_t counter;
    uint64_t timestamp;
};

enum NextPhase phase_a() {
    switch (get_buttons() &3) {
        case 0:
            return  A;
        case 1:
            return ATOB;
        case 2:
            return ATOD;
        default: return ERROR;
    }
}
enum NextPhase phase_b() {
   switch (get_buttons() & 3) {
   case 1:
      return B;
   case 3:
      return BTOC;
   case 0:
      return BTOA;
   default:
      return ERROR;
   }
}
enum NextPhase phase_c() {
   switch (get_buttons() & 3) {
   case 3:
      return C;
   case 2:
      return CTOD;
   case 1:
      return CTOB;
   default:
      return ERROR;
   }
}

enum NextPhase phase_d() {
   switch (get_buttons() & 3) {
   case 2:
      return D;
   case 0:
      return DTOA;
   case 3:
      return DTOC;
   default:
      return ERROR;
   }
}
enum NextPhase a_to_b() {
    add_degree();
    return B;
}
enum NextPhase a_to_d() {
    sub_degree();
    return D;
}
enum NextPhase b_to_c() {
    add_degree();
    return C;
}
enum NextPhase b_to_a() {
    sub_degree();
    return A;
}
enum NextPhase c_to_b() {
   sub_degree();
    return B;
}
enum NextPhase c_to_d() {
    add_degree();
    return D;
}
enum NextPhase d_to_a() {
    add_degree();
    return A;
}
enum NextPhase d_to_c() {
    sub_degree();
    return C;
}

enum NextPhase error() {
   switch ((get_buttons() >> BUTTON_S6) & 1) {
   case 1:
      return INIT;
   default:
      return ERROR;
   }
}

enum NextPhase init() {
    //TODO Clear display
    //TODO Reset other stuff to 0
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
       return ERROR;
    }
}
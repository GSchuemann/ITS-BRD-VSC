#include <stdint.h>

#define BUTTON_S6 6
enum NextPhase {
    A,B,C,D,ATOB,ATOD,BTOA,BTOC,CTOB,CTOD,DTOC,DTOA,ERROR,INIT
};


struct Phase {
     uint8_t direction;
     uint32_t counter;
     uint64_t timestamp;
};

enum NextPhase phase_a(uint32_t buttons) {
    switch (buttons &3) {
        case 0:
            return  A;
        case 1:
            return ATOB;
        case 2:
            return ATOD;
        default: return ERROR;
    }
}
enum NextPhase phase_b(uint32_t buttons) {
    switch (buttons &3) {
        case 1:
            return  B;
        case 3:
            return BTOC;
        case 0:
            return BTOA;
        default: return ERROR;
    }
}
enum NextPhase phase_c(uint32_t buttons) {
    switch (buttons &3) {
        case 3:
            return  C;
        case 2: 
            return CTOD;
        case 1:
            return CTOB;
        default: return ERROR;
    }
}

enum NextPhase phase_d(uint32_t buttons) {
    switch (buttons &3) {
        case 2:
            return  D;
        case 0:
            return DTOA;
        case 3:
            return DTOC;
        default: return ERROR;
    }
}
enum NextPhase a_to_b() {
    return B;
}
enum NextPhase a_to_d() {
    return D;
}
enum NextPhase b_to_c() {
    return C;
}
enum NextPhase b_to_a() {
    return A;
}
enum NextPhase c_to_b() {
    return B;
}
enum NextPhase c_to_d() {
    return D;
}
enum NextPhase d_to_a() {
    return A;
}
enum NextPhase d_to_c() {
    return C;
}

enum NextPhase error(uint32_t buttons) {
    switch ((buttons >> BUTTON_S6) &1) {
        case 1:
            return INIT;
        default:
            return ERROR;
    }
}

enum NextPhase init(uint32_t buttons) {
    //TODO Clear display
    //TODO Reset other stuff to 0
    switch (buttons&3) {
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
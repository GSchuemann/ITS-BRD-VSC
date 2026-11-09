#include <stdint.h>
#ifndef phases
#define  phases
enum NextPhase phase_a(void);
enum NextPhase phase_b(void);
enum NextPhase phase_c(void);
enum NextPhase phase_d(void);
enum NextPhase a_to_b(void);
enum NextPhase a_to_d(void);
enum NextPhase b_to_c(void);
enum NextPhase b_to_a(void);
enum NextPhase c_to_b(void);
enum NextPhase c_to_d(void);
enum NextPhase d_to_a(void);
enum NextPhase d_to_c(void);
enum NextPhase error(void);
enum NextPhase init(void);

enum NextPhase {
    A,
    B,
    C,
    D,
    ATOB,
    ATOD,
    BTOA,
    BTOC,
    CTOB,
    CTOD,
    DTOC,
    DTOA,
    ERRORPHASE,
    INIT,
};
typedef  enum NextPhase (*phase_fn)(void);
extern phase_fn phase_ops[];
#endif
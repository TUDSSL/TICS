#ifndef JUMP_H__
#define JUMP_H__

#include "config.h"
#include "arch.h"

#include "checkpoint.h"
#include "workingstack.h"
#include "checkpoint_registers.h"

typedef struct {
    reg_t registers[N_REGISTERS];
    unsigned int workingstack;
    volatile int return_value;
} visp_jmp_buf;

void visp_longjmp(visp_jmp_buf *environment, int value);

__attribute__((always_inline))
static inline int visp_setjmp(visp_jmp_buf *environment)
{
    /* Setjmp always returns 0 */
    environment->return_value = 0;

    /* Save the active workingstack */
    environment->workingstack = (unsigned int)workingstack_get_idx();

    /* Save the registers */
    // Configure register checkpoint buffer
    checkpoint_registers_set_dst(environment->registers);
    checkpoint_registers();

    return environment->return_value;
}

#endif /* JUMP_H__ */

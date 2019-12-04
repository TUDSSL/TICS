#ifndef CHECKPOINT_H__
#define CHECKPOINT_H__

#include <stdlib.h>
#include <stdint.h>

#include "config.h"
#include "arch.h"
#include "stackarray.h"
#include "critical.h"

typedef struct checkpoint_ctrl {
    reg_t register_checkpoint[N_REGISTERS];
    char stack_checkpoint[STACK_BLOCK_SIZE];
    size_t stack_checkpoint_size;
} checkpoint_ctrl_t;

/*
 * Mimic fork system call
 *  return
 *      negative    -   checkpoint unsuccessful
 *      zero        -   checkpoint succeeded
 *      positive    -   checkpoint restored
 */
int checkpoint(void);
int checkpoint_custom_range(reg_t *register_checkpoint, char *sp);
void checkpoint_restore(void);
void checkpoint_init(void);

static inline int checkpoint_safe(void)
{
    int ret;

    CRITICAL_START();
    ret = checkpoint();
    CRITICAL_END();

    return ret;
}

#endif /* CHECKPOINT_H__ */

#ifndef WORKINGSTACK_H__
#define WORKINGSTACK_H__

#include "config.h"
#include "stackarray.h"
#include "stack.h"

/*
 * Workingstack Control (double buffered)
 */
typedef struct workingstack_ctrl {
    stackarray_idx_t workingstack_idx;
    stackarray_idx_t current_checkpoint_block_idx;
} workingstack_ctrl_t;

#define WORKINGSTACK_INITIAL_CTRL {                     \
    .workingstack_idx = N_STACK_BLOCKS-1,               \
    .current_checkpoint_block_idx = N_STACK_BLOCKS-1,   \
}

void workingstack_set_ctrl_a(void);
void workingstack_set_ctrl_b(void);
stackarray_idx_t workingstack_get_idx(void);


uint16_t workingstack_shrink(stack_block_t *shrink_to);
void workingstack_register_checkpoint(void);
void workingstack_restore_state(void);
void workingstack_configure_virtualmem_check(void);


#endif /* WORKINGSTACK_H__ */

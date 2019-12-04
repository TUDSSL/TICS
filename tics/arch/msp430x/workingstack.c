//#define DEBUG_PRINT
#define DEBUG_MODULE "workingstack"
#include "util/debug.h"
#undef DEBUG_PRINT

#define WS_CNT

#include <stdint.h>

#include "config.h"

#include "workingstack.h"
#include "nvm.h"
#include "checkpoint.h"
#include "stack.h"
#include "checkpoint_stack.h"
#include "stackarray.h"
#include "virtualmem.h"
#include "checkpoint_timer.h"

char *__workingstack_limit;
char *__workingstack_start;

NVM workingstack_ctrl_t WorkingstackCtrl[2] = {WORKINGSTACK_INITIAL_CTRL, WORKINGSTACK_INITIAL_CTRL};
#define WORKINGSTACK_CTRL_A WorkingstackCtrl[0]
#define WORKINGSTACK_CTRL_B WorkingstackCtrl[1]
NVM workingstack_ctrl_t *ActiveWorkingstackCtrl     = &WORKINGSTACK_CTRL_A;
NVM workingstack_ctrl_t *InactiveWorkingstackCtrl   = &WORKINGSTACK_CTRL_B;

static inline void workingstack_restore_ctrl(void)
{
    *ActiveWorkingstackCtrl = *InactiveWorkingstackCtrl;
}

void workingstack_set_ctrl_a(void)
{
    ActiveWorkingstackCtrl      = &WORKINGSTACK_CTRL_A;
    InactiveWorkingstackCtrl    = &WORKINGSTACK_CTRL_B;
    workingstack_restore_ctrl();
}

void workingstack_set_ctrl_b(void)
{
    ActiveWorkingstackCtrl      = &WORKINGSTACK_CTRL_B;
    InactiveWorkingstackCtrl    = &WORKINGSTACK_CTRL_A;
    workingstack_restore_ctrl();
}

stackarray_idx_t workingstack_get_idx(void)
{
    return ActiveWorkingstackCtrl->workingstack_idx;
}

#ifdef WS_CNT
NVM uint16_t __workingstack_grow_cnt = 0;
NVM uint16_t __workingstack_shrink_cnt = 0;
NVM uint16_t __workingstack_checkpoint_cnt = 0;
#endif /* WS_CNT */

stack_block_t *workingstack_grow(void)
{
    stack_block_t *new_workingstack;
    stackarray_idx_t new_workingstack_idx;

    new_workingstack_idx = stackarray_get_next_block_idx(ActiveWorkingstackCtrl->workingstack_idx);
    new_workingstack = stackarray_get_block(new_workingstack_idx);

    dbgprintf("grow to block: "DBG_PTR_PRINT" (idx=%d)\n",
            DBG_PTR(new_workingstack), new_workingstack_idx);

    // Update the double buffered workingstack control
    ActiveWorkingstackCtrl->workingstack_idx = new_workingstack_idx;

    // Update the workingstack limit (for assembly)
    __workingstack_limit = new_workingstack;

    // Update the workingstack start (for assembly)
    __workingstack_start = &new_workingstack[STACK_BLOCK_SIZE]; // Stack starts at the high address

    // Move the actual check address TODO make nice
    __workingstack_limit = &__workingstack_limit[STACK_BLOCK_SIZE - WORKINGSTACK_SIZE];


    //checkpoint_timer_reset();

#ifdef WS_CNT
    ++__workingstack_grow_cnt;
#endif /* WS_CNT */

    return new_workingstack;
}

uint16_t workingstack_shrink(stack_block_t *shrink_to)
{
    uint16_t do_checkpoint;
    stackarray_idx_t current_cp_idx, shrink_to_idx;

    current_cp_idx = ActiveWorkingstackCtrl->current_checkpoint_block_idx;
    shrink_to_idx = stackarray_ptr2idx(shrink_to);

#ifdef DEBUG_PRINT
    stack_block_t current_cp_block = stackarray_get_block(current_cp_idx);
    dbgprintf("current checkpoint block: "DBG_PTR_PRINT" (idx=%d)\n",
            DBG_PTR(current_cp_block), current_cp_idx);
#endif /* DEBUG_PRINT */

    dbgprintf("shrink to block: "DBG_PTR_PRINT" (idx=%d)\n",
            DBG_PTR(shrink_to), shrink_to_idx);

    /* If we shrink past the current checkpoint block we checkpoint the block
     * we shrink to
     */
    if (shrink_to_idx > current_cp_idx) {

        dbgprintf("shrink past checkpoint, mark for checkpoint\n");

#ifdef WS_CNT
        ++__workingstack_checkpoint_cnt;
#endif /* WS_CNT */

        do_checkpoint = 1;
    } else {
        do_checkpoint = 0;
    }

    //checkpoint_timer_reset();

    // Update the double buffered workingstack control
    ActiveWorkingstackCtrl->workingstack_idx = shrink_to_idx;
    workingstack_restore_state();

#ifdef WS_CNT
        ++__workingstack_shrink_cnt;
#endif /* WS_CNT */

    return do_checkpoint;
}

void workingstack_register_checkpoint(void)
{
    ActiveWorkingstackCtrl->current_checkpoint_block_idx = ActiveWorkingstackCtrl->workingstack_idx;
}

/*
 * Set global variables used in the workingstack assembly code
 */
void workingstack_restore_state(void)
{
    // Update the workingstack limit (for assembly)
    __workingstack_limit = stackarray_get_block(ActiveWorkingstackCtrl->workingstack_idx);

    // Update the workingstack start (for assembly)
    __workingstack_start = &__workingstack_limit[STACK_BLOCK_SIZE]; // Stack starts at the high address

    // Move the actual check address TODO: Make nice
    __workingstack_limit = &__workingstack_limit[STACK_BLOCK_SIZE - WORKINGSTACK_SIZE];

}

void workingstack_configure_virtualmem_check(void)
{
    // Set virtualmem check address to all stack blocks previous to the checkpointed stack block
    stackarray_idx_t vmem_check_idx = ActiveWorkingstackCtrl->current_checkpoint_block_idx;

    stack_block_t *vmem_check_block = stackarray_get_block(vmem_check_idx);
    /* The check address is all previous blocks. The stack grows down, so idx + 1 minus 1 element*/
    virtualmem_set_check_addr((uintptr_t)&vmem_check_block[STACK_BLOCK_SIZE-1]);
}

#if 0
stack_block_t * workingstack_grow(void)
{
    stack_block_t *sb = stackpool_alloc_block();
    sb->stack_size = 0;
    checkpoint_stack_set_dst(sb->stack, &sb->stack_size);
    dbgprintf("backup to block: "DBG_PTR_PRINT"\n", DBG_PTR(sb));
    checkpoint_stack();

    virtualaddr_map_block(virtualindex_get(), sb);
    virtualindex_incr();

#ifdef WS_CNT
    ++ws_cnt;
#endif /* WS_CNT */

    checkpoint_timer_reset();

    return sb;
}

stack_block_t * workingstack_shrink(stack_block_t *working_stack, stack_block_t *shrink_to)
{
    checkpoint_stack_set_src(sb->stack, &sb->stack_size);
    dbgprintf("restore from block: "DBG_PTR_PRINT"\n", DBG_PTR(sb));
    checkpoint_restore_stack();

    virtualindex_decr();

    checkpoint_timer_reset();

#ifdef WS_CNT
        ++ws_restore_cnt;
#endif /* WS_CNT */

    return sb;
}
#endif


/*
 * Variables used in assembly
 */
NVM uint32_t __wss_save_r5 = 0;
NVM uint32_t __wss_save_r6 = 0;
NVM uint32_t __wss_old_sp = 0;
NVM uint32_t __wss_call = 0;

/* Arguments need to be saved twice, have two copies to avoid WAR violation */
NVM uint32_t __wss_save_r11_a = 0;
NVM uint32_t __wss_save_r12_a = 0;
NVM uint32_t __wss_save_r13_a = 0;
NVM uint32_t __wss_save_r14_a = 0;
NVM uint32_t __wss_save_r15_a = 0;

NVM uint32_t __wss_save_r11_b = 0;
NVM uint32_t __wss_save_r12_b = 0;
NVM uint32_t __wss_save_r13_b = 0;
NVM uint32_t __wss_save_r14_b = 0;
NVM uint32_t __wss_save_r15_b = 0;

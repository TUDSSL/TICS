//#define DEBUG_PRINT
#define DEBUG_MODULE "checkpoint"
#include "util/debug.h"
#undef DEBUG_PRINT

/* Use a defined stack, bss and data buffer */
//#define BASIC_CHECKPOINT

#include <stdlib.h>
#include <stdbool.h>

#include "config.h"

#include "nvm.h"
#include "checkpoint.h"
#include "arch.h"
#include "checkpoint_arch.h"

#include "stackarray.h"
#include "workingstack.h"
#include "virtualmem.h"

/* 0 buffer a, 1 buffer b */
#define CHECKPOINT_FLAG_BUF         (1<<0)
#define CHECKPOINT_FLAG_BUF_MASK    CHECKPOINT_FLAG_BUF

#define RESTORE_POINT(flag_, restore_state_) do {   \
    if (cp_get_atomic_flag() == flag_) {            \
        /* Restored */                              \
        restore_state_ = 1;                         \
    } else {                                        \
        /* Normal operation */                      \
        restore_state_ = 0;                         \
        /* set atomic flag bouble buffer */         \
        cp_set_atomic_flag(flag_);                  \
    }                                               \
} while (0);

NVM volatile atomic_flag_t cp_buffer_flag = 0;
NVM volatile int reboot = 0;


/*
 * Checkpoint Control Backup (double buffered)
 */
NVM checkpoint_ctrl_t CheckpointCtrl[2];
#define CHECKPOINT_CTRL_A CheckpointCtrl[0]
#define CHECKPOINT_CTRL_B CheckpointCtrl[1]
checkpoint_ctrl_t *ActiveCheckpointCtrl     = &CHECKPOINT_CTRL_A;
checkpoint_ctrl_t *InactiveCheckpointCtrl   = &CHECKPOINT_CTRL_B;

static inline void checkpoint_set_ctrl_a(void)
{
    ActiveCheckpointCtrl    = &CHECKPOINT_CTRL_A;
    InactiveCheckpointCtrl  = &CHECKPOINT_CTRL_B;
}

static inline void checkpoint_set_ctrl_b(void)
{
    ActiveCheckpointCtrl    = &CHECKPOINT_CTRL_B;
    InactiveCheckpointCtrl  = &CHECKPOINT_CTRL_A;
}


/*
 * Atomic flag manipulation
 */
static inline atomic_flag_t cp_get_atomic_flag(void)
{
    return cp_buffer_flag;
}

static inline void cp_set_atomic_flag(atomic_flag_t flag)
{
    cp_buffer_flag = flag;
}

static inline bool checkpoint_buf_a(atomic_flag_t flag)
{
    return ((flag & CHECKPOINT_FLAG_BUF) == 0);
}

static inline bool checkpoint_buf_b(atomic_flag_t flag)
{
    return ((flag & CHECKPOINT_FLAG_BUF) == 1);
}

static inline atomic_flag_t checkpoint_buf_other(atomic_flag_t flag)
{
    flag ^= CHECKPOINT_FLAG_BUF_MASK;
    return flag;
}


/*
 * Checkpoint functions
 */
static inline void checkpoint_setup_checkpoint_arguments(void)
{
    // Configure register checkpoint buffer
    checkpoint_registers_set_dst(ActiveCheckpointCtrl->register_checkpoint);

    // Configure stack checkpoint buffer
    checkpoint_stack_set_dst(ActiveCheckpointCtrl->stack_checkpoint,
            &ActiveCheckpointCtrl->stack_checkpoint_size);
}

static inline void checkpoint_setup_checkpoint_restore_arguments(void)
{
    // Configure register checkpoint buffer
    checkpoint_registers_set_src(InactiveCheckpointCtrl->register_checkpoint);

    // Configure stack checkpoint buffer
    checkpoint_stack_set_src(InactiveCheckpointCtrl->stack_checkpoint,
            &InactiveCheckpointCtrl->stack_checkpoint_size);
}

static void checkpoint_setup_active_buffers(atomic_flag_t new_flag)
{
    // Configure stack checkpoint control
    if (checkpoint_buf_a(new_flag)) {
        checkpoint_set_ctrl_a();
        workingstack_set_ctrl_a();
        virtualmem_set_ctrl_a();
    } else {
        checkpoint_set_ctrl_b();
        workingstack_set_ctrl_b();
        virtualmem_set_ctrl_b();
    }
}


static inline void checkpoint_prepare(volatile atomic_flag_t *new_flag)
{
    //dbgprintf("Checkpoint prepare\n");

    *new_flag = cp_get_atomic_flag();
    *new_flag = checkpoint_buf_other(*new_flag);
}

static inline void checkpoint_prepare_next(void)
{
    // Setup the buffers for this restore
    checkpoint_setup_active_buffers(cp_get_atomic_flag());

    // Make sure the new active virtualmem is empty
    virtualmem_free();
}

int checkpoint(void)
{
    /* Must be volatile as this function is also the restore point
     * and the flag is used to check if it is a restore or not
     */

    volatile atomic_flag_t new_flag;
    int restore_state;

    dbgprintf("checkpoint using buffer: "DBG_PTR_PRINT"\n", DBG_PTR(ActiveCheckpointCtrl->stack_checkpoint));

    // Set the new current_checkpoint_block_idx corresponding to this checkpoint
    workingstack_register_checkpoint();

    // Prepare for the next checkpoint
    checkpoint_prepare(&new_flag);

    // Perform checkpoint
    checkpoint_setup_checkpoint_arguments(); // Arguments for checkpoint
    checkpoint_arch();

    // NB: restore point
    RESTORE_POINT(new_flag, restore_state);

    if (restore_state == 0) {
        /* Checkpoint already prepared in restore */
        checkpoint_prepare_next();
    }

    workingstack_configure_virtualmem_check();

#ifdef DEBUG_PRINT
    char *stack_cp = InactiveCheckpointCtrl->stack_checkpoint;
    if (restore_state) {
        dbgprintf("POST restore using buffer: "DBG_PTR_PRINT"\n", DBG_PTR(stack_cp));
    } else {
        dbgprintf("POST checkpoint using buffer: "DBG_PTR_PRINT"\n", DBG_PTR(stack_cp));
    }
#endif

    return restore_state;
}

void checkpoint_restore(void)
{
    atomic_flag_t flag;

    flag = cp_get_atomic_flag();

    // Configure stack checkpoint source buffer
    checkpoint_setup_active_buffers(flag);

    // Restore virtual memory
    virtualmem_writeback();

    // Restore the workingstack state
    workingstack_restore_state();

    dbgprintf("PRE restore using buffer: "DBG_PTR_PRINT"\n", DBG_PTR(InactiveCheckpointCtrl->stack_checkpoint));
    checkpoint_setup_checkpoint_restore_arguments();
    checkpoint_arch_restore_full();
}

/*
 * Checkpoint using provided register buffers
 */
int checkpoint_custom_range(reg_t *register_checkpoint, char *sp)
{
    /* Must be volatile as this function is also the restore point
     * and the flag is used to check if it is a restore or not
     */

    volatile atomic_flag_t new_flag;
    int restore_state;

    dbgprintf("Range checkpoint using buffer: "DBG_PTR_PRINT"\n", DBG_PTR(ActiveCheckpointCtrl->stack_checkpoint));

    // Set the new current_checkpoint_block_idx corresponding to this checkpoint
    workingstack_register_checkpoint();

    // Prepare for the next checkpoint
    checkpoint_prepare(&new_flag);

    // Perform checkpoint
    checkpoint_setup_checkpoint_arguments(); // Arguments for checkpoint
    //checkpoint_stack_set_stack_start(stack_start);
    checkpoint_stack_set_sp(sp);

    // Alternative to checkpoint_arch
    checkpoint_stack_custom_range();
    for (int i=0; i<N_REGISTERS; i++) {
        ActiveCheckpointCtrl->register_checkpoint[i] = register_checkpoint[i];
    }

    // NB: restore point
    RESTORE_POINT(new_flag, restore_state);

    if (restore_state == 0) {
        /* Checkpoint already prepared in restore */
        checkpoint_prepare_next();
    }

    workingstack_configure_virtualmem_check();

#ifdef DEBUG_PRINT
    char *stack_cp = InactiveCheckpointCtrl->stack_checkpoint;
    if (restore_state) {
        dbgprintf("POST restore range using buffer: "DBG_PTR_PRINT"\n", DBG_PTR(stack_cp));
    } else {
        dbgprintf("POST ragnge checkpoint using buffer: "DBG_PTR_PRINT"\n", DBG_PTR(stack_cp));
    }
#endif

    return restore_state;
}


extern int application_main(void);

__attribute__((noinline))
static void checkpoint_application_start(void)
{
    checkpoint();
    reboot = 1;
    application_main();
}

__attribute__((noinline))
static void checkpoint_application_continue(void)
{
    checkpoint_restore();
}

void checkpoint_init(void)
{
    if (reboot == 0) {
        checkpoint_setup_active_buffers(cp_get_atomic_flag());
        workingstack_restore_state();

        /* Set the correct stackpointer and checkpoint, then call main */

        stackarray_idx_t ws_idx = workingstack_get_idx();
        stack_block_t *ws = stackarray_get_block(ws_idx);

        // NB. Can't use stack after this
        checkpoint_set_sp(&ws[STACK_BLOCK_SIZE]); // Stack starts high

        checkpoint_application_start();
    } else {
        checkpoint_application_continue();
    }
}

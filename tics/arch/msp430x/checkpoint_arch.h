#ifndef ARCH_CHECKPOINT_H__
#define ARCH_CHECKPOINT_H__

#include <stdlib.h>

#include "checkpoint_ctrl.h"
#include "checkpoint_registers.h"
#include "checkpoint_stack.h"
#include "checkpoint_data.h"

static void checkpoint_arch(void)
{
#ifdef CHECKPOINT_DATA_SECTION
    checkpoint_data();
#endif /* CHECKPOINT_DATA_SECTION */

    checkpoint_stack();
    checkpoint_registers();
}

// Can be optimized by passing sp to stack restore
// but we must guarantee that it works correctly
// as SP can not be on the old stack
static void checkpoint_arch_restore_full(void)
{
    char *sp;
    size_t size = *StackCtrl.stack_size;

    sp = (char *)&StackCtrl.stack_start[-size];
    checkpoint_set_sp(sp);

#ifdef CHECKPOINT_DATA_SECTION
    checkpoint_restore_data();
#endif /* CHECKPOINT_DATA_SECTION */

    checkpoint_restore_stack();
    checkpoint_restore_registers();
}

#endif /* ARCH_CHECKPOINT_H__ */

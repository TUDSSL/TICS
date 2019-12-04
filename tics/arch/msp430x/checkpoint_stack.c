#include <stdlib.h>

#include "config.h"

#include "nvm.h"
#include "stack.h"
#include "stackarray.h"
#include "checkpoint_ctrl.h"
#include "checkpoint_stack.h"

#ifdef CHECKPOINT_DMA
#include "dma.h"
#define CP_COPY   dma_copy
#pragma message "#Checkpoint Stack: Using DMA"
#else /* !CHECKPOINT_DMA */
#define CP_COPY   memcpy
#endif /* CHECKPOINT_DMA */

NVM struct cpctrl_stack StackCtrl = {
    .dst = NULL,
    .sp = NULL,
    .stack_start = NULL,
    .stack_size = NULL
};

void checkpoint_stack(void)
{
    char *sp, *dst;
    size_t size;

    sp = checkpoint_get_sp();

#if 0 // Is this still a thing now?
    size = StackCtrl.stack_start - sp;
    *StackCtrl.stack_size += size; // NB. Update the total size, this can be larger than the copied stack
    total_size = *StackCtrl.stack_size;
#endif
    size = StackCtrl.stack_start - sp;
    *StackCtrl.stack_size = size; // NB. Update the total size, this can be larger than the copied stack

    dst = &StackCtrl.dst[STACKBLOCK_REVERSE_IDX(size-1)];
    CP_COPY(dst, sp, size);
}

void checkpoint_stack_custom_range(void)
{
    char *dst;
    size_t size;

    size = StackCtrl.stack_start - StackCtrl.sp;
    *StackCtrl.stack_size = size; // NB. Update the total size, this can be larger than the copied stack

    dst = &StackCtrl.dst[STACKBLOCK_REVERSE_IDX(size-1)];
    CP_COPY(dst, StackCtrl.sp, size);
}

void checkpoint_restore_stack(void)
{
    char *sp, *src;
    size_t size = *StackCtrl.stack_size;

    sp = (char *)&StackCtrl.stack_start[-size];

    src = &StackCtrl.dst[STACKBLOCK_REVERSE_IDX(size-1)];
    CP_COPY(sp, src, size);
}

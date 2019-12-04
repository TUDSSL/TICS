#ifndef CHECKPOINT_STACK_H__
#define CHECKPOINT_STACK_H__

#include <stdint.h>
#include <string.h>

#include "config.h"

#include "arch.h"
#include "stack.h"
#include "checkpoint_ctrl.h"
#include "workingstack.h"

extern struct cpctrl_stack StackCtrl;
extern char *__workingstack_start; // workingstack.c

static inline void checkpoint_stack_set_dst(char *dst, size_t *size)
{
    StackCtrl.dst = dst;
    StackCtrl.stack_size = size;
    StackCtrl.stack_start = __workingstack_start;
}

static inline void checkpoint_stack_set_stack_start(char *stack_start)
{
    StackCtrl.stack_start = stack_start;
}

static inline void checkpoint_stack_set_sp(char *sp)
{
    StackCtrl.sp = sp;
}

#define checkpoint_stack_set_src checkpoint_stack_set_dst

//#define CHECKPOINT_STACK_SIZE(sp_) (size_t)((intptr_t)(&__LD_STACK_MARKER) - (intptr_t)(sp_))

/*
 * The following functions must be inlined due to stack issues
 * They change the stack pointer and therefore break the return address
 */
static inline char *checkpoint_get_sp(void) __attribute__((always_inline));
static inline char *checkpoint_set_sp(char *sp) __attribute__((always_inline));

/*
 * The following functions may not be inlined due to stack issues
 * They change the stack in a way that will break the checkpointing routine
 * if inlined.
 */
void checkpoint_stack(void) __attribute__((noinline));
void checkpoint_stack_custom_range(void) __attribute__((noinline));
void checkpoint_restore_stack(void) __attribute__((noinline));

static inline char * checkpoint_get_sp(void)
{
    char *sp;

    __asm__ volatile (
            MSP_MOV_ADDR_INSTR" r1, %[stack_ptr] \n\t"
            :   [stack_ptr] "=m" (sp)/* output */
            : /* input */
            : "memory" /* clobber */
    );

    return sp;
}

static inline char * checkpoint_set_sp(char *sp)
{
    __asm__ volatile (
            MSP_MOV_ADDR_INSTR" %[stack_ptr], r1\n\t"
            : /* output */
            : [stack_ptr] "g" (sp) /* input */
            : /* clobber */
    );

    return sp;
}


#endif /* CHECKPOINT_STACK_H__ */

#ifndef CHECKPOINT_CTRL_H__
#define CHECKPOINT_CTRL_H__

#include <stdint.h>

#include "arch.h"

typedef struct cpctrl_registers {
    reg_t *dst;

    union {
        reg_t *pc_save;
        intptr_t pc_save_addr;
    };

    union {
        reg_t *sp_save;
        intptr_t sp_save_addr;
    };
} cpctrl_registers_t;

typedef struct cpctrl_stack {
    char *dst;
    char *sp;
    char *stack_start;
    size_t *stack_size;
} cpctrl_stack_t;

typedef struct cpctrl_data {
    char *dst_data;
    char *data_start;
    size_t data_size;

    char *dst_bss;
    char *bss_start;
    size_t bss_size;
} cpctrl_data_t;

#endif /* CHECKPOINT_CTRL_H__ */

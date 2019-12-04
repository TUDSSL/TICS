#ifndef CHECKPOINT_REGISTERS_H__
#define CHECKPOINT_REGISTERS_H__

#include "arch.h"
#include "checkpoint_ctrl.h"

//extern volatile struct cpctrl_registers RegisterCtrl;

extern volatile reg_t *RegisterCtrl_dst;
extern volatile reg_t *RegisterCtrl_pc_save;
extern volatile reg_t *RegisterCtrl_sp_save;

static inline void checkpoint_registers(void) __attribute__((always_inline));

static inline void checkpoint_registers_set_dst(reg_t *dst)
{
    RegisterCtrl_dst        = &dst[N_REGISTERS-1];
    RegisterCtrl_pc_save    = &dst[0];
    RegisterCtrl_sp_save    = &dst[1];
}

static inline void checkpoint_registers_set_src(reg_t *dst)
{
    RegisterCtrl_dst            = &dst[N_REGISTERS-1-12];
    RegisterCtrl_pc_save   = (reg_t *)dst[0];
    RegisterCtrl_sp_save   = (reg_t *)dst[1];
}

// save stack -> set dts as stack -> pusha registers to stack -> restore stack
// cycles: 4 + 4 + 2+2*16 + 4 = 46 cycles
// cycles with all mova = 16*4 = 64 cycles
// NB. PSUHM is high to low
static inline void checkpoint_registers(void)
{
    __asm__ volatile (
        MSP_MOV_ADDR_INSTR" r1, %[sp_save] \n\t"  /* save the SP [2+2*1=4] */
        MSP_MOV_ADDR_INSTR" %[new_sp], r1\n\t"    /* set the dst as the new SP [4] */
        MSP_PUSHM_ADDR_INSTR" #12, r15 \n\t"      /* push all registers r4-r15 to the stack [2+2*16=34] */
        MSP_MOV_ADDR_INSTR" %[sp_save], r1 \n\t"  /* restore the old SP */
        MSP_MOV_ADDR_INSTR" r0, %[pc_save] \n\t"  /* save the PC [2+2*1=4] */
        : [new_sp] "+m" (RegisterCtrl_dst),     /* output */
          [pc_save] "=m" (*RegisterCtrl_pc_save),
          [sp_save] "+m" (*RegisterCtrl_sp_save)
        : /* input */
        : "memory" /* clobber */
    );
}

/*
 * LLVM does not convert RegisterCtrl.x to constants.
 * But this is needed for the register manipulation
 * If we partially use LLVM (Clang) compile the function seperately using GCC
 * This in currently not an issue as GCC is required for a lot more steps...
 */
#ifdef __clang__
void checkpoint_restore_registers(void);
#else
static inline void checkpoint_restore_registers(void)
{
    __asm__ volatile (
        MSP_MOV_ADDR_INSTR" %[new_sp], r1 \n\t"
        MSP_POPM_ADDR_INSTR" #12, r15 \n\t"
        MSP_MOV_ADDR_INSTR" %[sp_save], r1 \n\t"
        MSP_BRANCH_INSTR" %[pc_save] \n\t"
        : /* output */
        : [new_sp] "m" (RegisterCtrl_dst),     /* input */
          [pc_save] "m" (RegisterCtrl_pc_save),
          [sp_save] "m" (RegisterCtrl_sp_save)
        : /* clobber */
    );
}
#endif

#endif /* CHECKPOINT_REGISTERS_H__ */

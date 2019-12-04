#include <stdlib.h>

#include "arch.h"
#include "checkpoint_ctrl.h"

extern volatile reg_t *RegisterCtrl_dst;
extern volatile reg_t *RegisterCtrl_pc_save;
extern volatile reg_t *RegisterCtrl_sp_save;
/*
 * See checkpoint_registers.h
 */
void checkpoint_restore_registers(void)
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

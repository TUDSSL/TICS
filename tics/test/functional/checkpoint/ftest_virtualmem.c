#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "driverlib.h"

#include "nvm.h"
#include "arch.h"
#include "checkpoint.h"
#include "stackpool.h"
#include "virtualaddr.h"
#include "virtualmem.h"

#include "debug_arch.h"


// Clear all the volatile memory
// Currently only the working stack + scratch stack (see linkerscript)
// Exclude the memory currently in use
#include "checkpoint_stack.h"
extern char __stack;
#define CLEAR_RAM_LENGTH  (0x0FCE + 0x0032)
#define CLEAR_RAM_START   0x1C00
void clear_volatile_mem(void)
{
    char *sp, *ram;
    uintptr_t sp_addr, ram_addr;
    ptrdiff_t clear_size;
    uint32_t i;

    ram = (char *)CLEAR_RAM_START;
    sp = checkpoint_get_sp();

    ram_addr = CLEAR_RAM_START;
    sp_addr = (uintptr_t)&sp;
    clear_size = sp_addr - ram_addr;

    // Don't use memset to avoid extra stack usage
    // Stack required for the for loop should have been allocated at the start
    // of the function.
    for (i=0; i<clear_size; i++) {
        ram[i] = 0;
    }
}

void reset_mcu(void)
{
    WDTCTL = 0;
}

NVM uint16_t reboot = 0;

NVM int global_var = 42;

struct test_struct {
    char str[10];
    int var;
};

#define A2V_vidx(vidx_, ptr_)    ((__typeof__(ptr_)){virtualaddr_addr2virt(vidx_, ptr_)})
#define V2A_vidx(cvidx_, ptr_)   ((__typeof__(ptr_)){virtualaddr_virt2addr(cvidx_, ptr_)})

#define A2V(ptr_)       ((__typeof__(ptr_)){virtualaddr_addr2virt(virtualindex_get(), ptr_)})
#define V2A(ptr_)       ((__typeof__(ptr_)){virtualaddr_virt2addr(virtualindex_get(), ptr_)})

extern void workingstack_swap(void);

__attribute__((noinline))
void workingstack_modify_stack_ptr(int *stack_ptr)
{
    printf("WS: Stack pointer virtual address: "DBG_PTR_PRINT"\n", DBG_PTR(stack_ptr));
    printf("WS: Stack pointer physical address: "DBG_PTR_PRINT"\n", DBG_PTR(V2A(stack_ptr)));
    VMEM_WR(*VMEM_WR(V2A(stack_ptr))) = *(V2A(stack_ptr)) + 10;
}

static volatile int *stack_ptr_tmp;

__attribute__((noinline))
void workingstack_func(void)
{
    workingstack_swap();
    workingstack_modify_stack_ptr(stack_ptr_tmp);
}


int main(void)
{
    int stack_var = 42;
    struct test_struct ts = {
        .var = 60
    };
    strcpy(ts.str, "hello");

    WDTCTL = WDTPW | WDTHOLD; // Stop WDT

    if (reboot == 0) {
        checkpoint_full();
        reboot = 1;
    } else {
        // Clear the working stack and scratch stack
        clear_volatile_mem();
        checkpoint_restore();
    }

    // stack_var should be 42
    int *stack_ptr;

    VMEM_WR(stack_ptr) = A2V(&stack_var);

    VMEM_WR(*VMEM_WR(V2A(stack_ptr))) = 43;
    // stack_var should be 43

    printf("Stack pointer virtual address: "DBG_PTR_PRINT"\n", DBG_PTR(stack_ptr));
    printf("Stack pointer physical address: "DBG_PTR_PRINT"\n", DBG_PTR(V2A(stack_ptr)));

    stack_ptr_tmp = stack_ptr;
    workingstack_func();

    // stack_var should be 53

    virtualmem_writeback();
    // stack_var should be 42


#if 0
    int *global_var_ptr, *global_var_vptr;
    int *stack_var_ptr, *stack_var_vptr;
    int *block_var_ptr, *block_var_vptr;
    struct test_struct *ts_ptr, *ts_vptr;

    int vidx_old = 2;
    int vidx = 3;

    global_var = 1;

    VMEM_WR(global_var) = 42;
    global_var_ptr = A2V(vidx, &global_var);

    if (global_var != 42) {
        printf("Write error\n");
    }

    VMEM_WR(*VMEM_WR(V2A(vidx, global_var_ptr))) = 43;
    //VMEM_WR(*VMEM_WR(global_var_ptr)) = 43;

    if (global_var != 43) {
        printf("Write error\n");
    }

    virtualmem_writeback();

    if (global_var != 1) {
        printf("Restore error\n");
    }


    /* Virtual address of CURRENT stack struct */
    printf("\nCurrent Stack struct\n");

    ts.var = 2;
    //ts_ptr = &ts;
    VMEM_WR(ts_vptr) = A2V(vidx, &ts); // ts_vptr = A2V(vidx, &ts);
    //ts_vptr = A2V(vidx, &ts); // ts_vptr = A2V(vidx, &ts);
    VMEM_WR(VMEM_WR(V2A(vidx, ts_vptr))->var) = 32;

    if (ts.var != 32) {
        printf("Write error\n");
    }

    // Writes back nothing, as it is in the current stack
    virtualmem_writeback();

    if (ts.var != 32) {
        printf("Restore error\n");
    }


    /* Virtual address of OLD stack struct */
    printf("\nOld Stack struct\n");

    ts.var = 2;
    VMEM_WR(ts_vptr) = A2V(vidx_old, &ts); // ts_vptr = A2V(vidx, &ts);
    VMEM_WR(VMEM_WR(V2A(vidx, ts_vptr))->var) = 12;

    if ((V2A(vidx, ts_vptr))->var != 12) {
        printf("Write error\n");
    }

    // Writes back nothing, as it is in the current stack
    virtualmem_writeback();

    if ((V2A(vidx, ts_vptr))->var != 60) {
        printf("Restore error\n");
    }
#endif

#if 0
    /* Get a stack block for testing */
    stack_block_t *sb_old = stackpool_alloc_block();
    stack_block_t *sb_current = stackpool_alloc_block();

    /* Save the stack to the sb_old */
    sb_old->stack_size = 0;
    checkpoint_stack_set_dst(sb_old->stack, &sb_old->stack_size);
    dbgprintf("backup to block: "DBG_PTR_PRINT"\n", DBG_PTR(sb_old));
    checkpoint_stack();

    printf("Old stackblock: "DBG_PTR_PRINT"\n", DBG_PTR(sb_old));
    printf("Current stackblock: "DBG_PTR_PRINT"\n", DBG_PTR(sb_current));

    int vidx_old = 2;
    int vidx = 3;

    /* Map the stack block to the virtual index */
    virtualaddr_map_block(vidx_old, sb_old);
    virtualaddr_map_block(vidx, sb_current);


    /* Virtual address of global variable */
    printf("\nGlobal variable\n");
    printf("Expect same address\n");

    global_var_ptr = &global_var;
    global_var_vptr = A2V(vidx, global_var_ptr);

    printf("paddr: "DBG_PTR_PRINT"\n", DBG_PTR(global_var_ptr));
    printf("vaddr: "DBG_PTR_PRINT"\n", DBG_PTR(global_var_vptr));

    printf("Not a virual address, expect same address\n");
    global_var_ptr = V2A(vidx, global_var_vptr);
    printf("paddr: "DBG_PTR_PRINT"\n", DBG_PTR(global_var_ptr));


    /* Virtual address of stackblock (NVM) variable */
    printf("\nGlobal variable\n");
    printf("Expect same address, should normally not happen\n");

    block_var_ptr = (int *)sb_old->stack;
    block_var_vptr = A2V(vidx_old, block_var_ptr);

    printf("paddr: "DBG_PTR_PRINT"\n", DBG_PTR(block_var_ptr));
    printf("vaddr: "DBG_PTR_PRINT"\n", DBG_PTR(block_var_vptr));


    /* Virtual address of stack variable */
    printf("\nStack variable\n");
    printf("Expect virtual address\n");

    stack_var_ptr = &stack_var;
    stack_var_vptr = A2V(vidx, stack_var_ptr);

    printf("paddr: "DBG_PTR_PRINT"\n", DBG_PTR(stack_var_ptr));
    printf("vaddr: "DBG_PTR_PRINT"\n", DBG_PTR(stack_var_vptr));

    printf("Virual address, current vidx, expect physical address SRAM\n");
    stack_var_ptr = V2A(vidx, stack_var_vptr);
    printf("paddr: "DBG_PTR_PRINT"\n", DBG_PTR(stack_var_ptr));


    /* Virtual address of OLD stack variable */
    printf("\nOLD Stack variable\n");
    stack_var_ptr = &stack_var;
    stack_var_vptr = A2V(vidx_old, stack_var_ptr);

    printf("paddr: "DBG_PTR_PRINT"\n", DBG_PTR(stack_var_ptr));
    printf("vaddr: "DBG_PTR_PRINT"\n", DBG_PTR(stack_var_vptr));

    printf("Virual address, old vidx, expect physical address in stackblock\n");
    stack_var_ptr = V2A(vidx, stack_var_vptr);
    printf("paddr: "DBG_PTR_PRINT"\n", DBG_PTR(stack_var_ptr));

    int val_block_addr = *((int *)stack_var_ptr);
    printf("Value at block address: %d\n", val_block_addr);


    /* Virtual address of OLD stack struct */
    printf("\nOLD Stack struct\n");
    ts_ptr = &ts;
    ts_vptr = A2V(vidx_old, ts_ptr);

    printf("paddr: "DBG_PTR_PRINT"\n", DBG_PTR(ts_ptr));
    printf("vaddr: "DBG_PTR_PRINT"\n", DBG_PTR(ts_vptr));

    printf("Virual address, old vidx, expect physical address in stackblock\n");
    ts_ptr = V2A(vidx, ts_vptr);
    printf("paddr: "DBG_PTR_PRINT"\n", DBG_PTR(ts_ptr));

    printf("Str: %s, Val:%d at block address\n", ts_ptr->str, ts_ptr->var);


    /* Free stack block */
    stackpool_free_block(sb_old);
    stackpool_free_block(sb_current);

#endif

    printf("\nStackpool report\n");
    printf("Blocks left: %d/%d\n", ActiveStackPool->n_free_blocks, N_STACK_BLOCKS);

    while(1) {
        printf("Done!\n");
    }
}

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "driverlib.h"

#include "nvm.h"
#include "arch.h"
#include "checkpoint.h"
#include "stackarray.h"
#include "virtualmem.h"

////////////////////////////////////////////////////////////////////////////////
// checkpoint.c
//extern volatile atomic_flag_t cp_buffer_flag;
//extern reg_t RegisterCheckpoint[2][N_REGISTERS];
//extern size_t StackCheckpointSize[N_STACKS];
//extern char StackCheckpoint[N_STACKS][STACK_SIZE];
//extern char DataCheckpoint[2][0xFFF];
//extern char BssCheckpoint[2][0xFFF];
////////////////////////////////////////////////////////////////////////////////

extern int checkpoint_full(void);
extern int checkpoint_minimal(stack_block_t *restore_stack, char *sp_start);

/*
    Stackpool usage:
        [0] checkpoint_full()
        [1] workingstack_backup (workingstack_swap)
        [2] checkpoint_full() (workingstack_swap) // TODO free [2]
        [1] checkpoint_minimal() (workingstack_swap after return)
*/

// Workingstack
extern void workingstack_swap(void);

NVM int test_data[200] = {0};

#define WSS __attribute__((segstack))

WSS int dummy(void)
{
    return 42;
}

WSS uint32_t dummy_arg(uint32_t reg_a)
{
    uint32_t ret;

    ret = 42 + reg_a;
    return ret;
}

WSS uint32_t dummy_stack_arg(uint32_t reg_a, uint32_t reg_b, uint32_t stack_a, uint8_t stack_b)
{
    uint32_t ret = reg_a + reg_b + stack_a + stack_b;
    return ret;
}

/*
 * With checkpoint
 */

WSS int dummy_cp(void)
{
    checkpoint();
    return 42;
}

WSS uint32_t dummy_arg_cp(uint32_t reg_a)
{
    uint32_t ret;

    ret = 42 + reg_a;

    checkpoint();
    return ret;
}

WSS uint32_t dummy_stack_arg_cp(uint32_t reg_a, uint32_t reg_b, uint32_t stack_a, uint8_t stack_b)
{
    uint32_t ret = reg_a + reg_b + stack_a + stack_b;

    checkpoint();
    return ret;
}


/*
 * Functions with virtual memory
 */
NVM int GlobalTestVar = 42;

/*
 * Directly write to a global variable
 * ACCESS REQUIRES LOG
 */
WSS int dummy_vmem_global_direct(void)
{
    VMEM_WR(GlobalTestVar) = 10;
    return GlobalTestVar;
}

/*
 * Directly write to a local variable through a pointer
 * ACCESS WITHOUT LOG
 */
WSS int dummy_vmem_local_ptr_direct(void)
{
    int variable;
    int *var_ptr = &variable;

    VMEM_WR(*var_ptr) = 11;

    return *var_ptr;
}

/*
 * Pointer access to data in another workingstack, but no checkpoint
 * ACCESS WITHOUT LOG
 */
WSS int dummy_vmem_other_wss_ptr_direct(int *var)
{
    VMEM_WR(*var) = 12;

    return *var;
}

/*
 * Pointer access to data in another workingstack that IS ceckpointed
 * ACCESS REQUIRES LOG
 */
WSS int dummy_vmem_local_ptr_checkpointed(int *var)
{
    checkpoint();
    VMEM_WR(*var) = 13;

    return *var;
}


#if 0
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
#endif

//NVM uint16_t reboot = 0;

NVM int vmem_var = 0;
char test_buf[10];
volatile int retval;

void mspconsole_init(void);
void main_application(void);

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD; // Stop WDT

    /* init FRAM */
    FRCTL0_H |= (FWPW) >> 8;

    PM5CTL0 &= ~LOCKLPM5;

    // UART
    mspconsole_init();
    __enable_interrupt();

    checkpoint_init();
}

int application_main(void)
{


#if 0
    WDTCTL = WDTPW | WDTHOLD; // Stop WDT

    /* init FRAM */
    FRCTL0_H |= (FWPW) >> 8;

    PM5CTL0 &= ~LOCKLPM5;

    // UART
    mspconsole_init();
    __enable_interrupt();

    if (reboot == 0) {
        memset(test_data, 0, sizeof(test_data));

        checkpoint_init();
        checkpoint();
        reboot = 1;
    } else {
        // Clear the working stack and scratch stack
        checkpoint_restore();
    }
#endif

    checkpoint();

    /* Functions that do not cross the checkpoint return boundary */
    retval = dummy(); // 42
    retval = dummy_arg(12); // 54
    retval = dummy_stack_arg(10, 20, 40, 50); // = 120

    /* Functions that cross the checkpoint return boundary */
    retval = dummy_cp(); // 42
    retval = dummy_arg_cp(12); // 54
    retval = dummy_stack_arg_cp(10, 20, 40, 50); // = 120

    /* Functions that use virtual memory */
    checkpoint();
    int var = 43;
    retval = dummy_vmem_global_direct();
    retval = dummy_vmem_local_ptr_direct();
    retval = dummy_vmem_other_wss_ptr_direct(&var);
    retval = dummy_vmem_local_ptr_checkpointed(&var);


    printf("Return value: %d\n", retval);

#if 0
    if (reboot == 0) {
        checkpoint_full();
        reboot = 1;
        memset(test_data, 0, sizeof(test_data));
    } else {
        // Clear the working stack and scratch stack
        clear_volatile_mem();
        checkpoint_restore();
    }

#if 1
    if (checkpoint_full() == 0)
        reset_mcu();

    for (int i=0; i<3; i++) {
        test_data[i] += i;
        if (checkpoint_full() == 0)
            reset_mcu();
    }

    if (checkpoint_full() == 0)
        reset_mcu();

    retval = dummy_do();

    if (checkpoint_full() == 0)
        reset_mcu();

    for (int i=3; i<6; i++) {
        test_data[i] += i;
        if (checkpoint_full() == 0)
            reset_mcu();
    }
#else
    VMEM_WR(vmem_var) = 42;
    VMEM_WR(vmem_var) = 12;
    checkpoint_noret();

#endif

#endif

    printf("\nStackpool report\n");
    //printf("Blocks left: %d/%d\n", ActiveStackPool->n_free_blocks, N_STACK_BLOCKS);

    while(1) {
        printf("Done!\n");
    }

    checkpoint_restore();
}

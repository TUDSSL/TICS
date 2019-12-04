#define DEBUG_PRINT

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


extern int checkpoint_full(void);
extern int checkpoint_minimal(stack_block_t *restore_stack, char *sp_start);


NVM int test_data[200] = {0};

__attribute__((segstack))
uint32_t dummy_arg(uint32_t reg_a);

__attribute__ ((segstack))
int dummy_do(void)
{
    for (int i=42; i<44; i++) {
        test_data[i] = i;
    }
    return test_data[42];
}

__attribute__((segstack))
uint32_t dummy_stack_arg(uint32_t reg_a, uint32_t reg_b, uint32_t stack_a, uint8_t stack_b)
{
    uint32_t ret = reg_a + reg_b + stack_a + stack_b;
    ret = dummy_arg(ret);
    return ret;
}

__attribute__((segstack))
uint32_t dummy_arg(uint32_t reg_a)
{
    uint32_t ret;

    ret = dummy_do() + reg_a;
    return ret;
}

__attribute__((segstack))
int ws_arg_ptr(int *int_ptr)
{
    VMEM_WR(*V2A(int_ptr)) = VMEM_RD(*V2A(int_ptr)) + 42;
    return 42;
}


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

NVM int vmem_var = 0;

int main(void)
{
    int retval;

    WDTCTL = WDTPW | WDTHOLD; // Stop WDT

    if (reboot == 0) {
        checkpoint_full();
        reboot = 1;
        memset(test_data, 0, sizeof(test_data));
    } else {
        // Clear the working stack and scratch stack
        clear_volatile_mem();
        checkpoint_restore();
    }

#if 0
    retval = dummy_do();
    retval = dummy_stack_arg(10, 20, 40, 50); // = 120
#endif

    int orig_int = 12;
    retval = ws_arg_ptr(A2V(&orig_int));

    printf("Return value: %d\n", retval);
    printf("orig_int value: %d\n", orig_int); // 12 + 42 = 54

    VMEM_WR(*V2A(A2V(&orig_int))) = 6;
    printf("orig_int value (local ptr change): %d\n", orig_int); // 12 + 42 = 54

    printf("\nStackpool report\n");
    printf("Blocks left: %d/%d\n", ActiveStackPool->n_free_blocks, N_STACK_BLOCKS);

    while(1) {
        printf("Done!\n");
    }

    checkpoint_restore();
}

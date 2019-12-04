#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "driverlib.h"

#include "nvm.h"
#include "arch.h"
#include "checkpoint.h"
#include "stackarray.h"
#include "virtualmem.h"

#define WSS __attribute__((segstack))

WSS int co_ws_grow_no_arg(void)
{
    return 42;
}


WSS uint32_t co_ws_grow_8Bstack_arg(uint32_t reg_a, uint32_t reg_b, uint32_t stack_a, uint32_t stack_b)
{
    uint32_t ret = reg_a + reg_b + stack_a + stack_b;
    return ret;
}

/*
 * With checkpoint
 */

#define ADDITIONAL_STACK_BYTES 14
WSS int co_64B_checkpoint(void)
{
    volatile char local_64B[64-ADDITIONAL_STACK_BYTES];
    for (int i=0; i<(64-ADDITIONAL_STACK_BYTES); i++) {
        local_64B[i] = 42;
    }
    checkpoint();

    // Restore here
    return local_64B[1];
}

WSS int co_256B_checkpoint(void)
{
    volatile char local_256B[256-ADDITIONAL_STACK_BYTES];
    for (int i=0; i<(256-ADDITIONAL_STACK_BYTES); i++) {
        local_256B[i] = 42;
    }
    checkpoint();

    // Restore here
    return local_256B[1];
}

WSS void co_other_stack_write(uint32_t *ptr)
{
    checkpoint();

    VMEM_WR(*ptr) = 42;
}

volatile int retval;

void mspconsole_init(void);

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
/*
    Workingstack grow check
    Workingstack swap grow (no func arg on stack)
    Workingstack swap grow (8B func arg on stack)
    Wrokingstack shrink
    Checkpoint logic (w/o copy)
    Checkpoint 64B stack
    Checkpoint 256B stack
    Restore logic (w/o copy)
    Restore 64B stack
    Restore 256B stack
    Check virtualmem
    Log virtualmem 4B
    Log virtualmem 64B
    Writeback virtualmem 4B
    Writeback virtualmem 8B
*/

NVM uint32_t GlobalVar = 10;

int application_main(void)
{

    uint32_t local_var = 10;
    uint32_t *local_ptr;

    local_ptr = &local_var;

    checkpoint();

    // Workingstack grow check
    // Workingstack grow (no func arg on stack)
    // Wrokingstack shrink
    retval = co_ws_grow_no_arg();

    // Workingstack grow (8B func arg on stack)
    retval = co_ws_grow_8Bstack_arg(*local_ptr, 20, 30, 40);

    // Checkpoint logic (w/o copy)
    checkpoint();

    // Checkpoint 64B stack
    // Restore 64B stack
    retval = co_64B_checkpoint();

    // Checkpoint 256B stack
    // Restore 256B stack
    retval = co_256B_checkpoint();

    // Check virtualmem local write
    VMEM_WR(*local_ptr) = 42;

    // Check virtualmem global write (64B strategy write)
    VMEM_WR(GlobalVar) = 42;

    // Check virtualmem write to other stack (4B no strategy write)
    co_other_stack_write(local_ptr);

    printf("Return value: %d\n", retval);

    while(1);
}

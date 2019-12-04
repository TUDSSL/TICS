// Defined here
// void workingstack_swap(void)
    .global __workingstack_swap

// Procedure TODO
//  * Backup stack (same as checkpoint stack, but dont setup restore)
//  * save the old sp
//  * reset the stack (sp = start of stack)
//  * push the current nvm backup pointer to the stack
//  * push the old sp to the stack
//  * push the arguments to the start of the stack
//  * checkpoint();
//  * call/return magic (to old sp)
//
//  <<<< return point >>>>
//
//  ^ get backup pointer from the stack [stack_base[offset]]
//  ^ write return (if required) directly to the backup stack [stack_base[offset]]
//  ^ copy nvm backup to the stack
//  ^ set nvm backup as the checkpoint stack restore (instead of checkpoint_stack();
//  ^ checkpoint_registers()
//  ^ return
//

// stack_block_t *stackpool_alloc_block(void)
    .extern stackpool_alloc_block

// stack_block_t *workingstack_backup(void)
    .global workingstack_backup

// stack_block_t * workingstack_restore(stack_block_t *sb)
    .global workingstack_restore

// int checkpoint_full(void)
    .global checkpoint_full

// int checkpoint_minimal(stack_block_t *restore_stack)
    .global checkpoint_minimal

// stack start global (linker script)
    //.global _stack
    .global __stack_ws_restore
    .global __checkpoint_stack_start

// scratch stack start global (linker script)
    .global __scratchstack


// Temp memory locations (can't use stack because we change it)
// defined in workingstack.c
    .global __wss_save_r5
    .global __wss_save_r6
    .global __wss_call

    .global __wss_save_r11_a
    .global __wss_save_r12_a
    .global __wss_save_r13_a
    .global __wss_save_r14_a
    .global __wss_save_r15_a

    .global __wss_save_r11_b
    .global __wss_save_r12_b
    .global __wss_save_r13_b
    .global __wss_save_r14_b
    .global __wss_save_r15_b

__workingstack_swap:

    // Disable interrupts
    dint
    nop

    // Save used registers
    movx.a r5,&__wss_save_r5

    // Save arg registers
    movx.a r11, &__wss_save_r11_a
    movx.a r12, &__wss_save_r12_a
    movx.a r13, &__wss_save_r13_a
    movx.a r14, &__wss_save_r14_a
    movx.a r15, &__wss_save_r15_a

    // backup the stack into a new stackpool stack_block, R12 holds block
    pushm.a #1, r13
    calla #workingstack_backup
    popm.a #1, r13

    // Save the old stack pointer
    mova sp,r5

    // Read the return location before the stack gets overwritten
    movx.a @r5,&__wss_call

    // Reset the stack
    //mova #__stack_ws_restore, sp
    movx.a #__checkpoint_stack_start, sp
    movx.a @sp, sp
    movx.a @sp, sp

    // Push the stack_block address to the stack (link back)
    pushm.a #1, r12

    // Push the old stack pointer to the stack
    pushm.a #1, r5

    // Make sure the stack is word aligned after argument copy
    // So make R13 even, if the arg size is not even we copy junk, but
    // that's ok
    bit.w #1, r13
    jnc .L_WS_ALIGNED
    add.w #1, r13
.L_WS_ALIGNED:

    // Copy args to new stack
    adda r13, r5
    adda #15, r5

    suba #1, sp
.L_WS_SWAP_ARG:
    cmpa #0, r13
    jeq .L_WS_SWAP_ARG_END
    mov.b @r5, 0(sp)
    suba #1, r5
    suba #1, r13
    suba #1, sp
    jmp .L_WS_SWAP_ARG
.L_WS_SWAP_ARG_END:
    // Copy R12 R13
    suba #7, sp
    suba #11, r5 // 4 for call, 1 for word offset (.w = 0(r5) and -1(r5))
    mov.w 6(r5), 6(sp)
    mov.w 4(r5), 4(sp)
    mov.w 2(r5), 2(sp)
    mov.w 0(r5), 0(sp)

    // Perform a checkpoint (with the new almost empty stack)
    // TODO integrate virtualmem
    calla #checkpoint_full

    // restore used registers
    movx.a &__wss_save_r5,r5

    // restore argument registers
    movx.a &__wss_save_r11_a, r11
    movx.a &__wss_save_r12_a, r12
    movx.a &__wss_save_r13_a, r13
    movx.a &__wss_save_r14_a, r14
    movx.a &__wss_save_r15_a, r15

    // Enable interrupts
    nop
    eint
    nop

    // Return/call magic to after the function that initiated the grow
    calla &__wss_call

    // Disable interrupts
    dint
    nop

    // Remove R12 and R13 from the stack
    //adda #8, sp
    // We don't know the arg size, but we know how much metadata we pushed
    //mov.a #__stack_ws_restore, sp
    movx.a #__checkpoint_stack_start, sp
    movx.a @sp, sp
    movx.a @sp, sp

    suba #8, sp

    //// Srink return, put the old stack back ////
    // TODO save the return, checkpoint_minimal will overwrite it
    movx.a r11,&__wss_save_r11_b
    movx.a r12,&__wss_save_r12_b
    movx.a r13,&__wss_save_r13_b
    movx.a r14,&__wss_save_r14_b
    movx.a r15,&__wss_save_r15_b

    // save r6
    movx.a r6,&__wss_save_r6

    // Get the old stack pointer
    popm.a #1, r6

    movx.a r6,&__wss_old_sp

    // restore r6
    movx.a &__wss_save_r6,r6

    // Get the old block
    popm.a #1, r12

    mova #__scratchstack, sp

    // Restore the old stack block, block is in r12
    calla #workingstack_restore

    // restore the old stack pointer
    movx.a &__wss_old_sp, sp

    // Second argument to checkpoint minimal is the old sp
    mova sp, r13

    // Call checkpoint and set old block as stack restore
    calla #checkpoint_minimal

    // Restore return value
    movx.a &__wss_save_r11_b,r11
    movx.a &__wss_save_r12_b,r12
    movx.a &__wss_save_r13_b,r13
    movx.a &__wss_save_r14_b,r14
    movx.a &__wss_save_r15_b,r15

    // Return PC+1 instruction (skip the original call)
    add.w #12, sp

    // Enable interrupts
    nop
    eint
    nop

    reta


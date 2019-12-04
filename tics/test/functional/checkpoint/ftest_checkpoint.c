#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "driverlib.h"

#include "nvm.h"
#include "arch.h"
#include "checkpoint.h"

#include "virtualmem.h"

/*
 * Functional Test
 *  Test the working of the checkpoint and checkpoint_restore functions
 *
 *  Makes a checkpoint, saves the flag, creates a second checkpoint.
 *  The goal of this test is to check if the checkpoint remains the same
 *  every time the checkpoint is restored and the same sequence of code is
 *  executed again. To correcly compare the stack, we must compare the copy
 *  of the stack after a restore (otherwise the result variable on the stack
 *  is different). So the execution is:
 *      [1] prepare data
 *      [2] checkpoint #1
 *      [3] do stuff on stack/data/bss/registers (what happens here is compared)
 *      [4] checkpoint #2
 *      [5] backup checkpoint #2 to compare later on
 *      [6] if 3rd time goto [cmp]
 *      [7] goto checkpoint #1
 *      [cmp] compare the copy with the content of checkpoint
 *          Test succeeds if:
 *              test_data_ok == 0;
 *              test_bss_ok == 0;
 *              test_stack_ok == 0;
 *              cp_reg_ok == 0;
 *              cp_stack_size_ok == 0;
 *              cp_stack_ok == 0;
 *              cp_data_ok == 0;
 *              cp_bss_ok == 0;
 *
 * Running the test
 *  The test can be run by starting the program, and when it enters the
 *  infinite loop and test_all_ok == 1.
 *
 * The compare variables are in NVM to not spoil the data to be checkpointed.
 */

////////////////////////////////////////////////////////////////////////////////
// checkpoint.c
extern volatile atomic_flag_t cp_buffer_flag;
extern reg_t RegisterCheckpoint[2][N_REGISTERS];
extern size_t StackCheckpointSize[N_STACKS];
extern char StackCheckpoint[N_STACKS][STACK_SIZE];
extern char DataCheckpoint[2][0xFFF];
extern char BssCheckpoint[2][0xFFF];
////////////////////////////////////////////////////////////////////////////////

/*
 * NB. This is the index of the second checkpoint destination.
 * This is a bit of a hack, if checkpoint.c changes this should also be changed
 */
#define CIDX 0

NVM volatile atomic_flag_t flag_backup = 0;

NVM volatile int again = 0;

#define TEST_DATA_SIZE  16
#define TEST_BSS_SIZE   16
#define TEST_STACK_SIZE 16

volatile int test_data[TEST_DATA_SIZE] = {42,12};
volatile int test_bss[TEST_BSS_SIZE];

NVM volatile int test_data_cmp[TEST_DATA_SIZE] = {0};
NVM volatile int test_bss_cmp[TEST_BSS_SIZE] = {0};
NVM volatile int test_stack_cmp[TEST_STACK_SIZE] = {0};

NVM volatile int test_data_ok = -1;
NVM volatile int test_bss_ok = -1;
NVM volatile int test_stack_ok = -1;

NVM volatile int cp_reg_ok = -1;
NVM volatile int cp_stack_size_ok = -1;
NVM volatile int cp_stack_ok = -1;
NVM volatile int cp_data_ok = -1;
NVM volatile int cp_bss_ok = -1;

NVM volatile int test_all_ok = -1;

/* Compare stuff */
NVM reg_t RegisterCheckpointCpy[N_REGISTERS] = {0};
NVM size_t StackCheckpointSizeCpy = 0;
NVM char StackCheckpointCpy[STACK_SIZE] = {0};
//NVM char DataCheckpointCpy[0xFFF] = {0};
//NVM char BssCheckpointCpy[0xFFF] = {0};

void copy_checkpoint(void)
{
    StackCheckpointSizeCpy = StackCheckpointSize[CIDX];
    memcpy(RegisterCheckpointCpy,   &RegisterCheckpoint[CIDX][0],   N_REGISTERS * sizeof(reg_t));
    memcpy(StackCheckpointCpy,      &StackCheckpoint[CIDX][0],      StackCheckpointSize[1]);
    //memcpy(DataCheckpointCpy,       &DataCheckpoint[CIDX][0],       0xFFF);
    //memcpy(BssCheckpointCpy,        &BssCheckpoint[CIDX][0],        0xFFF);
}

void cmp_checkpoint(void)
{

    cp_reg_ok = memcmp((const char *)RegisterCheckpointCpy,
            (const char *)&RegisterCheckpoint[CIDX][0],
            N_REGISTERS * sizeof(reg_t));

    if (StackCheckpointSizeCpy == StackCheckpointSize[CIDX]) {
        cp_stack_size_ok = 0;
    }

    cp_stack_ok = memcmp((const char *)StackCheckpointCpy,
            (const char *)&StackCheckpoint[CIDX][0],
            StackCheckpointSizeCpy);

    /*
    cp_data_ok = memcmp((const char *)DataCheckpointCpy,
            (const char *)&DataCheckpoint[CIDX][0],
            0xFFF * sizeof(char)); // TODO

    cp_bss_ok = memcmp((const char *)BssCheckpointCpy,
            (const char *)&BssCheckpoint[CIDX][0],
            0xFFF * sizeof(char)); // TODO
    */
}

int main(void)
{
    int test_stack[TEST_STACK_SIZE] = {4, 5};

    WDTCTL = WDTPW | WDTHOLD; // Stop WDT

    /* Setup compare data
     */
    // fill data
    for (int i=0; i<TEST_DATA_SIZE; i++) {
        test_data_cmp[i] = i;
    }

    for (int i=0; i<TEST_BSS_SIZE; i++) {
        test_bss_cmp[i] = TEST_BSS_SIZE - i;
    }

    for (int i=0; i<TEST_STACK_SIZE; i++) {
        test_stack_cmp[i] = 100+i;
    }

    // checkpoint [1]
    checkpoint();
    flag_backup = cp_buffer_flag; // backup flag to restore other checkpoint

    // fill data
    for (int i=0; i<TEST_DATA_SIZE; i++) {
        VMEM_WR(test_data[i]) = i;
    }

    for (int i=0; i<TEST_BSS_SIZE; i++) {
        VMEM_WR(test_bss[i]) = TEST_BSS_SIZE - i;
    }

    for (int i=0; i<TEST_STACK_SIZE; i++) {
        test_stack[i] = 100+i;
    }

    // checkpoint [0]
    checkpoint();

    // Set fake flag to restore to checkpoint [1]
    cp_buffer_flag = flag_backup;

    // If second time, compare checkpoint [1] and [2]
    if (again == 2) {
        // compare checkpoints
        cmp_checkpoint();
    } else {
        copy_checkpoint();
        cmp_checkpoint();

        // Compare output
        test_data_ok    = memcmp((const char *)test_data,     (const char *)test_data_cmp, TEST_DATA_SIZE);
        test_bss_ok     = memcmp((const char *)test_bss,      (const char *)test_bss_cmp, TEST_BSS_SIZE);
        test_stack_ok   = memcmp((const char *)test_stack,    (const char *)test_stack_cmp, TEST_STACK_SIZE);

        // Done with the test
        while (1) {
            if (    test_data_ok == 0 &&
                    test_bss_ok == 0 &&
                    test_stack_ok == 0 &&
                    cp_reg_ok == 0 &&
                    cp_stack_size_ok == 0 &&
                    cp_stack_ok == 0
               ) {
                test_all_ok = 1;
                printf("Tests all OK\n");
            } else {
                test_all_ok = 0;
                printf("Tests FAILED\n");
            }
            while(1);
        }
    }
    again++;

    // restore to checkpoint [1]
    checkpoint_restore();
}

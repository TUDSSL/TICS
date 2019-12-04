#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "driverlib.h"

#include "nvm.h"
#include "arch.h"
#include "checkpoint.h"
#include "stackpool.h"
#include "virtualmem.h"

// Workingstack
extern void workingstack_swap(void);

__attribute__ ((noinline))
void checkpoint_noret(void)
{
    workingstack_swap();
    virtualmem_writeback();
}


// Checkpoint timer
void cp_timer_init(void)
{
  TA0CCTL0 = CCIE;                             // CCR0 interrupt enabled
  TA0CTL = TASSEL__SMCLK | MC__UP | ID__8;      // SMCLK/8, upmode
  /* 8MHz/8 = 1MHz. time = CCR0 * (1/1MHz)*/
  //TA0CCR0 =  10000; // = 0.01s = 10ms
  TA0CCR0 =  50000; // = 0.05s = 50ms
}

uint32_t isr_count = 0;

__attribute__((interrupt(TIMER0_A0_VECTOR)))
void Timer_A_ISR (void)
{
    ++isr_count;
    //_DINT();
    //checkpoint_noret();
    //_EINT();
}


const int arr[] = {3,1,4,6,9,5,10,8,16,20,19,40,16,17,2,41,80,100,5,89,66,77,8,3,32,55,8,11,99,
    65,25,89,3,22,25,121,11,90,74,1,12,39,54,20,22,43,45,90,81,40,
    3,1,4,6,9,5,10,8,16,20,19,40,16,17,2,41,80,100,5,89,66,77,8,3,32,55,8,11,99,
    65,25,89,3,22,25,121,11,90,74,1,12,39,54,20,22,43,45,90,81,40};

int arr_sort[sizeof(arr)/sizeof(int)];
int arr_len = 100;

void copy_arr(void)
{
    memcpy(arr_sort, arr, sizeof(arr));
}


// The algorithm
// 69636 cycles for arr, at 8Mhz = 8.7ms
// 73000 cycles for volatile array
__attribute__((noinline))
void sortAlgo(volatile int arr[], int arrLen){
    for (int i=0; i< arrLen-1;i++){
        for(int j=i+1; j < arrLen; j++){
            if(arr[i] >  arr[j]) { //ascending order
                int temp = arr[j];
                arr[j] = arr[i];
                arr[i] = temp;
            }
        }
    }
}

#ifdef COALA_TASKS

// inner = 1
// outer = 2
// finish = 3
//
// starts at inner

/////  TASKS
unsigned int in_i, in_j, arr_i, arr_j;
void task_inner_loop()
{

#if TSK_SIZ
    cp_reset();
#endif

    arr_i = RP( arr[RP(i)] );
    arr_j = RP( arr[RP(j)] );

    if( arr_i  > arr_j )
    {
        unsigned int temp = arr_j;
        arr_j =  arr_i;
        arr_i =  temp;
    }

    if( RP(j) < (arr_len-1) ) os_jump(0); // same task


    WP( arr[RP(i)] ) = arr_i;
    WP( arr[RP(j)] ) = arr_j;
    WP(j)++;
}


void task_outer_loop()
{

#if TSK_SIZ
    cp_reset();
#endif

    WP(i)++;

    if(RP(i) < arr_len)   os_jump(2); // outer

    WP(j)=  RP(i)+1;
}



void task_finish()
{
#if TSK_SIZ
    cp_reset();
#endif

    unsigned cct;
    for(cct=0; cct< arr_len; cct++)
    {
        arr[cct] =  arr2[cct];
    }


    WP(i) = 0 ;
    WP(j) = 1;


    while(1);
}

#endif


// Checkpoint system that checkpoints at the task boundaries
// 2290438 cycles w/o writeback
// writeback 9219 cycles
__attribute__((noinline))
void sortAlgoVmemNaive(void){

    int *arr = arr_sort;
    int arrLen = arr_len;

    int i, j;

    for (i=0; i< arrLen-1;i++){
        for(j=i+1; j < arrLen; j++){
            if(VMEM_RD(arr[i]) >  VMEM_RD(arr[j])) { //ascending order
                int temp = VMEM_RD(arr[j]);
                VMEM_WR(arr[j]) = VMEM_RD(arr[i]);
                VMEM_WR(arr[i]) = temp;
            }
        }
    }
}

// 1847000 cycles w/o writeback
// 230ms at 8MHz
__attribute__((noinline))
void sortAlgoVmemAware(void){

    int *arr = arr_sort;
    int arrLen = arr_len;

    int i, j;

    // Trigger page load on first array element
    (void)VMEM_WR(arr[0]);

    for (i=0; i< arrLen-1;i++){
        for(j=i+1; j < arrLen; j++){
            int tmp_arr_i = VMEM_RD(arr[i]);
            int tmp_arr_j = VMEM_RD(arr[j]);
            if(tmp_arr_i > tmp_arr_j) { //ascending order
                VMEM_WR(arr[j]) = tmp_arr_i;
                VMEM_WR(arr[i]) = tmp_arr_j;
            }
        }
    }
}

#if 0
NVM int test_data[200] = {0};

int dummy_do(void)
{
    for (int i=42; i<44; i++) {
        test_data[i] = i;
    }
    return test_data[42];
}

__attribute__ ((noinline))
int dummy_call(void)
{
    workingstack_swap();
    return dummy_do();
}
#endif

void reset_mcu(void)
{
    WDTCTL = 0;
}


NVM uint16_t reboot = 0;
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD; // Stop WDT

    CSCTL0_H = CSKEY >> 8;                // Unlock CS registers
    CSCTL1 = DCOFSEL_6;                   // Set DCO to 8MHz
    //CSCTL2 = SELM__DCOCLK;               // MCLK = DCO
    CSCTL2 = SELA__VLOCLK | SELS__DCOCLK | SELM__DCOCLK;  // Set SMCLK = MCLK = DCO
    //CSCTL3 = DIVM__1;                     // divide the DCO frequency by 1
    CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1;     // Set all dividers to 1
    CSCTL0_H = 0;

    _DINT();
#if 0
    cp_timer_init();

    //_EINT();

    if (reboot == 0) {
        checkpoint_full();
        reboot = 1;
    } else {
        // Clear the working stack and scratch stack
        //clear_volatile_mem();
        checkpoint_restore();
    }

    for (long int i=0; i<4096; i++)
    {
        vm_arr[i] = i%64;
    }
#endif

    // setup sort array
    copy_arr();
    printf("Start original code reference\n");
    sortAlgo(arr_sort, arr_len);
    printf("End original code reference\n");

    copy_arr();
    printf("Start vmem naive code reference\n");
    sortAlgoVmemNaive();
    virtualmem_writeback();
    printf("End vmem naive code reference\n");

    copy_arr();
    printf("Start vmem aware code reference\n");
    sortAlgoVmemAware();
    virtualmem_writeback();
    printf("End vmem aware code reference\n");

    printf("\nStackpool report\n");
    printf("Blocks left: %d/%d\n", ActiveStackPool->n_free_blocks, N_STACK_BLOCKS);

    while(1) {
        printf("Done!\n");
    }
}

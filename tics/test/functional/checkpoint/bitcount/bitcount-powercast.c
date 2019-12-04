#define PRINTF_PRINT

#define CHECKPOINT_WORKINGSTACK
#define CHECKPOINT_ON_TASK_BOUNDARY
//#define CHECKPOINT_TIMER

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <msp430.h>
#include "driverlib.h"
#include "nvm.h"
#include "arch.h"
#include "checkpoint.h"
#include "stackarray.h"
#include "virtualmem.h"
#include "checkpoint_timer.h"

#include "ftest_util.h"
void mspconsole_init();

#ifdef LOG_PRINT
#define LOG(...) do {       \
    printf("Log: ");        \
    printf(__VA_ARGS__);    \
} while(0)
#else /* !LOG_PRINT */
#define LOG(...)
#endif /* LOG_PRINT */

#ifdef PRINTF_PRINT
#define PRINTF printf
#else
#define PRINTF(...)
#endif /* PRINTF_PRINT */

/* VISP */
extern volatile uint16_t __isr_checkpoint_cnt;
extern uint16_t __workingstack_grow_cnt;
extern uint16_t __workingstack_shrink_cnt;
extern uint16_t __workingstack_checkpoint_cnt;

NVM uint16_t __task_checkpoint_cnt = 0;
#ifdef CHECKPOINT_ON_TASK_BOUNDARY
#define TASK_CHECKPOINT() checkpoint(); ++__task_checkpoint_cnt;
//#define TASK_CHECKPOINT() checkpoint_and_reset(); ++__task_checkpoint_cnt;
#else /* !CHECKPOINT_ON_TASK_BOUNDARY */
#define TASK_CHECKPOINT(...)
#endif /* CHECKPOINT_ON_TASK_BOUNDARY */
/* VISP END */

NVM static uint16_t __software_checkpoint_cnt = 0;
#define SOFTWARE_CHECKPOINT() checkpoint(); ++__software_checkpoint_cnt;

#ifdef CHECKPOINT_WORKINGSTACK
#define CPWS __attribute__((segstack))
#else
#define CPWS
#endif


#define SEED 4L
#define ITER 100
#define CHAR_BIT 8


NVM static char bits[256] =
{
    0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,  /* 0   - 15  */
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,  /* 16  - 31  */
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,  /* 32  - 47  */
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,  /* 48  - 63  */
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,  /* 64  - 79  */
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,  /* 80  - 95  */
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,  /* 96  - 111 */
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,  /* 112 - 127 */
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,  /* 128 - 143 */
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,  /* 144 - 159 */
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,  /* 160 - 175 */
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,  /* 176 - 191 */
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,  /* 192 - 207 */
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,  /* 208 - 223 */
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,  /* 224 - 239 */
    4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8   /* 240 - 255 */
};


/* Prototypes */
CPWS int btbl_bitcnt(uint32_t x);
CPWS int bit_count(uint32_t x);
CPWS int bitcount(uint32_t i);
CPWS int ntbl_bitcount(uint32_t x);
CPWS int BW_btbl_bitcount(uint32_t x);
CPWS int AR_btbl_bitcount(uint32_t x);
CPWS int ntbl_bitcnt(uint32_t x);
CPWS int bit_shifter(uint32_t x);


int btbl_bitcnt(uint32_t x)
{
    int cnt = bits[ ((char *)&x)[0] & 0xFF ];

    if (0L != (x >>= 8))
        cnt += btbl_bitcnt(x);
    return cnt;
}

int bit_count(uint32_t x)
{
    int n = 0;
    /*
     ** The loop will execute once for each bit of x set, this is in average
     ** twice as fast as the shift/test method.
     */
    if (x) do {
        n++;
    } while (0 != (x = x&(x-1))) ;
    return(n);
}

int bitcount(uint32_t i)
{
    i = ((i & 0xAAAAAAAAL) >>  1) + (i & 0x55555555L);
    i = ((i & 0xCCCCCCCCL) >>  2) + (i & 0x33333333L);
    i = ((i & 0xF0F0F0F0L) >>  4) + (i & 0x0F0F0F0FL);
    i = ((i & 0xFF00FF00L) >>  8) + (i & 0x00FF00FFL);
    i = ((i & 0xFFFF0000L) >> 16) + (i & 0x0000FFFFL);
    return (int)i;
}

int ntbl_bitcount(uint32_t x)
{
    return
        bits[ (int) (x & 0x0000000FUL)       ] +
        bits[ (int)((x & 0x000000F0UL) >> 4) ] +
        bits[ (int)((x & 0x00000F00UL) >> 8) ] +
        bits[ (int)((x & 0x0000F000UL) >> 12)] +
        bits[ (int)((x & 0x000F0000UL) >> 16)] +
        bits[ (int)((x & 0x00F00000UL) >> 20)] +
        bits[ (int)((x & 0x0F000000UL) >> 24)] +
        bits[ (int)((x & 0xF0000000UL) >> 28)];
}

int BW_btbl_bitcount(uint32_t x)
{
    union
    {
        unsigned char ch[4];
        long y;
    } U;

    U.y = x;

    return bits[ U.ch[0] ] + bits[ U.ch[1] ] +
        bits[ U.ch[3] ] + bits[ U.ch[2] ];
}

/* x does not have to be instrumented as a pointer because it's never passed to another function */
int AR_btbl_bitcount(uint32_t x)
{
    unsigned char * Ptr = (unsigned char *) &x ;
    int Accu ;

    Accu  = bits[ *Ptr++ ];
    Accu += bits[ *Ptr++ ];
    Accu += bits[ *Ptr++ ];
    Accu += bits[ *Ptr ];
    return Accu;
}

int ntbl_bitcnt(uint32_t x)
{
    int cnt = bits[(int)(x & 0x0000000FL)];

    if (0L != (x >>= 4))
        cnt += ntbl_bitcnt(x);

    return cnt;
}

int bit_shifter(uint32_t x)
{
    int i, n;
    for (i = n = 0; x && (i < (sizeof(uint32_t) * CHAR_BIT)); ++i, x >>= 1)
        n += (int)(x & 1L);
    return n;
}

NVM volatile uint16_t benchmark_loop_cnt = 0;
static inline void benchmark_start(void)
{
    led_on();
    ++benchmark_loop_cnt;
}

static inline void benchmark_stop(void)
{
    led_off();
}

static inline void benchmark_output_correct(void)
{
    GPIO_setOutputLowOnPin(
            GPIO_PORT_P4,
            GPIO_PIN6
            );
}

static inline void benchmark_output_incorrect(void)
{
    GPIO_setOutputHighOnPin(
            GPIO_PORT_P4,
            GPIO_PIN6
            );
}

volatile NVM unsigned int n_0, n_1, n_2, n_3, n_4, n_5, n_6;

static inline benchmark_output_check(void)
{
    if (n_0 == 502 && n_1 == 502 && n_2 == 502 && n_3 == 502 && n_4 == 502 && n_5 == 502 && n_6 == 502) {
        benchmark_output_correct();
    } else {
        benchmark_output_incorrect();
    }
}

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD; // Stop WDT

    // Disable FRAM wait cycles to allow clock operation over 8MHz
    FRCTL0 = 0xA500 | ((1) << 4); // FRCTLPW | NWAITS_1;
    //__delay_cycles(3);

    /* init FRAM */
    FRCTL0_H |= (FWPW) >> 8;

    PM5CTL0 &= ~LOCKLPM5;

    // UART
    mspconsole_init();
    __enable_interrupt();

    // init timer
#ifdef CHECKPOINT_TIMER
    checkpoint_timer_init();
#endif

    init_led();

    GPIO_setAsOutputPin(
            GPIO_PORT_P4,
            GPIO_PIN6
            );

    checkpoint_init();
}


volatile int nop_var = 2;
int application_main(void)
{
    //	while(true){
    uint32_t seed;
    unsigned iter;
    unsigned func;


    while (1) {
        benchmark_start();
        SOFTWARE_CHECKPOINT();

        VMEM_WR(n_0) = 0;
        VMEM_WR(n_1) = 0;
        VMEM_WR(n_2) = 0;
        VMEM_WR(n_3) = 0;
        VMEM_WR(n_4) = 0;
        VMEM_WR(n_5) = 0;
        VMEM_WR(n_6) = 0;

        for (func = 0; func < 7; func++) {
            TASK_CHECKPOINT();

            seed = (uint32_t)SEED;
            if(func == 0){
                for(iter = 0; iter < ITER; ++iter, seed += 13){
                    TASK_CHECKPOINT();
                    VMEM_WR(n_0) = n_0 + bit_count(seed);
                }
            }
            else if(func == 1){
                for(iter = 0; iter < ITER; ++iter, seed += 13){
                    TASK_CHECKPOINT();
                    VMEM_WR(n_1) = n_1 + bitcount(seed);
                }
            }
            else if(func == 2){
                for(iter = 0; iter < ITER; ++iter, seed += 13){
                    TASK_CHECKPOINT();
                    VMEM_WR(n_2) = n_2 + ntbl_bitcnt(seed);
                }
            }
            else if(func == 3){
                for(iter = 0; iter < ITER; ++iter, seed += 13){
                    TASK_CHECKPOINT();
                    VMEM_WR(n_3) = n_3 + ntbl_bitcount(seed);
                }
            }
            else if(func == 4){
                for(iter = 0; iter < ITER; ++iter, seed += 13){
                    TASK_CHECKPOINT();
                    VMEM_WR(n_4) = n_4 + BW_btbl_bitcount(seed);
                }
            }
            else if(func == 5){
                for(iter = 0; iter < ITER; ++iter, seed += 13){
                    TASK_CHECKPOINT();
                    VMEM_WR(n_5) = n_5 + AR_btbl_bitcount(seed);
                }
            }
            else if(func == 6){
                for(iter = 0; iter < ITER; ++iter, seed += 13){
                    TASK_CHECKPOINT();
                    VMEM_WR(n_6) = n_6 + bit_shifter(seed);
                }
            }
        }
        TASK_CHECKPOINT();
        benchmark_output_check();
        benchmark_stop();
    }

#if 0
    checkpoint_timer_disable();
    nop_var = 21;
    printf("%u\r\n", n_0);
    printf("%u\r\n", n_1);
    printf("%u\r\n", n_2);
    printf("%u\r\n", n_3);
    printf("%u\r\n", n_4);
    printf("%u\r\n", n_5);
    printf("%u\r\n", n_6);

    printf("This is the end of the Bitcount benchmark\n");
    printf("Task checkpoint cnt: %d\n", __task_checkpoint_cnt);
    printf("WS grow: %d, WS shrink cnt: %d\n", __workingstack_grow_cnt, __workingstack_shrink_cnt);
    printf("WS checkpoint count: %d\n", __workingstack_checkpoint_cnt);
    printf("ISR cnt: %d\n", __isr_checkpoint_cnt);
    // name, Working stack size, vmem size, checkpoints, task checkpoints, isr checkpoints, workingstack checkpoints, workingstack grow, workingstack shrink
    printf("Bitcount, %d, %d, %d, %d, %d, %d, %d, %d\n",
            WORKINGSTACK_SIZE,
            VIRTUALMEM_STRATEGY_SIZE,
            __software_checkpoint_cnt,
            __task_checkpoint_cnt,
            __isr_checkpoint_cnt,
            __workingstack_checkpoint_cnt,
            __workingstack_grow_cnt,
            __workingstack_shrink_cnt);

    while (1);

    return 0;
#endif
}

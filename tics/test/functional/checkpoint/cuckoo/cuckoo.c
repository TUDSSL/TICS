#define EXIT_DONE
//#define LOG_PRINT
//#define PRINTF_PRINT
//#define BLOCK_PRINT
//#define BLOCK_LOG_PRINT

//#define DEBUG_PRINT

//#define OPTIMIZED_HASH_PTR
//#define OPTIMIZED_HASH_IDX_KEY

//#define CHECKPOINT_ON_TASK_BOUNDARY
#define CHECKPOINT_WORKINGSTACK
//#define CHECKPOINT_TIMER

#include "builtins.h"

#include "driverlib.h"
#include "nvm.h"
#include "arch.h"
#include "checkpoint.h"
#include "stackarray.h"
#include "virtualmem.h"
#include "checkpoint_timer.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef CONFIG_EDB
#include <libedb/edb.h>
#else
#define ENERGY_GUARD_BEGIN()
#define ENERGY_GUARD_END()
#endif

#include "cuckoo.h"

#include "ftest_util.h"
void mspconsole_init();

// #define SHOW_PROGRESS_ON_LED

#define NUM_KEYS (NUM_BUCKETS / 4) // shoot for 25% occupancy
#define INIT_KEY 0x1

#ifdef LOG_PRINT

#define LOG(...) do {       \
    printf("Log: ");        \
    printf(__VA_ARGS__);    \
} while(0)

#else

#define LOG(...)

#endif

#ifdef PRINTF_PRINT
#define PRINTF printf
#else
#define PRINTF(...)
#endif

#define __nv

#ifdef BLOCK_PRINT

#define BLOCK_PRINTF_BEGIN()    printf("BLOCK START\n")
#define BLOCK_PRINTF_END()      printf("BLOCK END\n")
#define BLOCK_PRINTF            printf

#else

#define BLOCK_PRINTF_BEGIN(...)
#define BLOCK_PRINTF_END(...)
#define BLOCK_PRINTF(...)

#endif

#ifdef BLOCK_LOG_PRINT

#define BLOCK_LOG_BEGIN()       printf("BLOCK LOG START\n")
#define BLOCK_LOG_END()         printf("BLOCK LOG END\n")
#define BLOCK_LOG               printf

#else

#define BLOCK_LOG_BEGIN(...)
#define BLOCK_LOG_END(...)
#define BLOCK_LOG(...)

#endif

NVM static uint16_t __task_checkpoint_cnt = 0;
#ifdef CHECKPOINT_ON_TASK_BOUNDARY
#define TASK_CHECKPOINT() checkpoint(); ++__task_checkpoint_cnt;
//#define TASK_CHECKPOINT() checkpoint_and_reset(); ++__task_checkpoint_cnt;
#else /* !CHECKPOINT_ON_TASK_BOUNDARY */
#define TASK_CHECKPOINT(...)
#endif /* CHECKPOINT_ON_TASK_BOUNDARY */

NVM static uint16_t __software_checkpoint_cnt = 0;
#define SOFTWARE_CHECKPOINT() checkpoint(); ++__software_checkpoint_cnt;


#ifdef CHECKPOINT_WORKINGSTACK
#define CPWS __attribute__((segstack))
#else
#define CPWS
#endif

CPWS static bool insert(fingerprint_t *filter, value_t key);
CPWS static bool lookup(fingerprint_t *filter, value_t key);

CPWS static hash_t djb_hash(uint8_t* data, unsigned len);
CPWS static index_t hash_fp_to_index(fingerprint_t fp);
CPWS static index_t hash_key_to_index(value_t fp);
CPWS static fingerprint_t hash_to_fingerprint(value_t key);
CPWS static value_t generate_key(value_t prev_key);

static NVM fingerprint_t fp_compare[NUM_BUCKETS] = {
    0x8500, 0x8b01, 0x8502, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x73fa, 0x0000, 0x0000, 0x8202, 0x0000, 0x0000, 0x860e, 0x0000,
    0x8610, 0x0000, 0x0000, 0x8913, 0x0000, 0x0000, 0x0000, 0x6d17,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x7d1e, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x7124, 0x0000, 0x8179, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x722d, 0x782e, 0x0000,
    0x8530, 0x0000, 0x0000, 0x7c33, 0x0000, 0x8835, 0x0000, 0x7837,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x7148, 0x0000, 0x0000, 0x0000, 0x0000, 0x724d, 0x0000, 0x724f,
    0x0000, 0x0000, 0x0000, 0x7e53, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x725c, 0x765d, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x8167,
    0x73ff, 0x6f69, 0x886a, 0x0000, 0x0000, 0x8a6d, 0x6e6e, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x6f75, 0x0000, 0x0000,
    0x0000, 0x8079, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x827f,
    0x7d80, 0x7981, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x738e, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x868e, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x7b96, 0x88f5,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x7a35, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x6ea7,
    0x79a8, 0x0000, 0x0000, 0x0000, 0x72ac, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x6eb6, 0x0000,
    0x0000, 0x81b9, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x7cc3, 0x0000, 0x0000, 0x8110, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x8b08, 0x0000, 0x0000, 0x0000, 0x84d4, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x72dc, 0x7add, 0x0000, 0x7ddf,
    0x0000, 0x0000, 0x6ee2, 0x0000, 0x0000, 0x6ce5, 0x0000, 0x0000,
    0x76e8, 0x0000, 0x0000, 0x0000, 0x7fa7, 0x0000, 0x78ee, 0x0000,
    0x0000, 0x6add, 0x0000, 0x0000, 0x0000, 0x78f5, 0x6af6, 0x0000,
    0x86f8, 0x0000, 0x74fa, 0x0000, 0x7efc, 0x0000, 0x0000, 0x7602,
};

void filter_not_equal(void)
{
    printf("Filter WRONG\n ");
}
void filter_equal(void)
{
    printf("Filter CORRECT\n\n");
}
__attribute__((noinline))
void check_filter(fingerprint_t *filter)
{
    int i;

    for (i=0; i<NUM_BUCKETS; i++) {
        if (filter[i] != fp_compare[i]) {
            filter_not_equal();
            return;
        }
    }
    filter_equal();
}

void print_filter(fingerprint_t *filter)
{
    unsigned i;
    BLOCK_PRINTF_BEGIN();
    for (i = 0; i < NUM_BUCKETS; ++i) {
        BLOCK_PRINTF("%04x ", filter[i]);
        if (i > 0 && (i + 1) % 8 == 0)
            BLOCK_PRINTF("\r\n");
    }
    BLOCK_PRINTF_END();
}

void log_filter(fingerprint_t *filter)
{
    unsigned i;
    BLOCK_LOG_BEGIN();
    for (i = 0; i < NUM_BUCKETS; ++i) {
        BLOCK_LOG("%04x ", filter[i]);
        if (i > 0 && (i + 1) % 8 == 0)
            BLOCK_LOG("\r\n");
    }
    BLOCK_LOG_END();
}

// TODO: to avoid having to wrap every thing printf macro (to prevent
// a mementos checkpoint in the middle of it, which could be in the
// middle of an EDB energy guard), make printf functions in libio
// and exclude libio from Mementos passes
void print_stats(unsigned inserts, unsigned members, unsigned total)
{
    PRINTF("stats: inserts %u members %u total %u\r\n",
           inserts, members, total);
}

volatile uint8_t *data_ptr;
static hash_t djb_hash(uint8_t* data, unsigned len)
{
   uint32_t hash = 5381;
   unsigned int i;

   data_ptr = data;
   for(i = 0; i < len; data++, i++)
      hash = ((hash << 5) + hash) + *data;

   return hash & 0xFFFF;
}

static index_t hash_fp_to_index(fingerprint_t fp)
{
    hash_t hash = djb_hash((uint8_t *)&fp, sizeof(fingerprint_t));
    return hash & (NUM_BUCKETS - 1); // NUM_BUCKETS must be power of 2
}

static index_t hash_key_to_index(value_t fp)
{
    hash_t hash = djb_hash((uint8_t *)&fp, sizeof(value_t));
    return hash & (NUM_BUCKETS - 1); // NUM_BUCKETS must be power of 2
}

static fingerprint_t hash_to_fingerprint(value_t key)
{
    return djb_hash((uint8_t *)&key, sizeof(value_t));
}

static value_t generate_key(value_t prev_key)
{
    /* Task Boundary */
    TASK_CHECKPOINT();

    // insert pseufo-random integers, for testing
    // If we use consecutive ints, they hash to consecutive DJB hashes...
    // NOTE: we are not using rand(), to have the sequence available to verify
    // that that are no false negatives (and avoid having to save the values).
    return (prev_key + 1) * 17;
}

static bool insert(fingerprint_t *filter, value_t key)
{
    fingerprint_t fp1, fp2, fp_victim, fp_next_victim;
    index_t index_victim, fp_hash_victim;
    unsigned relocation_count = 0;

    /* Task Boundary */
    TASK_CHECKPOINT();

    fingerprint_t fp = hash_to_fingerprint(key);

    /* Task Boundary */
    TASK_CHECKPOINT();

    index_t index1 = hash_key_to_index(key);

    /* Task Boundary */
    TASK_CHECKPOINT();

    index_t fp_hash = hash_fp_to_index(fp);
    index_t index2 = index1 ^ fp_hash;

    /* Task Boundary */
    TASK_CHECKPOINT();

    LOG("insert: key %04x fp %04x h %04x i1 %u i2 %u\r\n",
        key, fp, fp_hash, index1, index2);

    fp1 = filter[index1];
    LOG("insert: fp1 %04x\r\n", fp1);
    if (!fp1) { // slot 1 is free
        VMEM_WR(filter[index1]) = fp;
    } else {
        fp2 = filter[index2];
        LOG("insert: fp2 %04x\r\n", fp2);
        if (!fp2) { // slot 2 is free
            VMEM_WR(filter[index2]) = fp;
        } else { // both slots occupied, evict
            if (rand() & 0x80) { // don't use lsb because it's systematic
                index_victim = index1;
                fp_victim = fp1;
            } else {
                index_victim = index2;
                fp_victim = fp2;
            }

            LOG("insert: evict [%u] = %04x\r\n", index_victim, fp_victim);
            VMEM_WR(filter[index_victim]) = fp; // evict victim

            do { // relocate victim(s)
                /* Task Boundary */
                TASK_CHECKPOINT();

                fp_hash_victim = hash_fp_to_index(fp_victim);
                index_victim = index_victim ^ fp_hash_victim;

                /* Task Boundary */
                TASK_CHECKPOINT();

                fp_next_victim = filter[index_victim];
                VMEM_WR(filter[index_victim]) = fp_victim;

                LOG("insert: moved %04x to %u; next victim %04x\r\n",
                    fp_victim, index_victim, fp_next_victim);

                fp_victim = fp_next_victim;
            } while (fp_victim && ++relocation_count < MAX_RELOCATIONS);

            if (fp_victim) {
                PRINTF("insert: lost fp %04x\r\n", fp_victim);
                return false;
            }
        }
    }

    return true;
}

static bool lookup(fingerprint_t *filter, value_t key)
{
    /* Task Boundary */
    TASK_CHECKPOINT();

    fingerprint_t fp = hash_to_fingerprint(key);

    /* Task Boundary */
    TASK_CHECKPOINT();

    index_t index1 = hash_key_to_index(key);

    /* Task Boundary */
    TASK_CHECKPOINT();

    index_t fp_hash = hash_fp_to_index(fp);
    index_t index2 = index1 ^ fp_hash;

    /* Task Boundary */
    TASK_CHECKPOINT();

    LOG("lookup: key %04x fp %04x h %04x i1 %u i2 %u\r\n",
        key, fp, fp_hash, index1, index2);

    return (filter[index1] == fp || filter[index2] == fp);
}

void init()
{
#if 0
    WISP_init();

#ifdef CONFIG_EDB
    debug_setup();
#endif

    INIT_CONSOLE();

    __enable_interrupt();

    GPIO(PORT_LED_1, DIR) |= BIT(PIN_LED_1);
    GPIO(PORT_LED_2, DIR) |= BIT(PIN_LED_2);
#if defined(PORT_LED_3)
    GPIO(PORT_LED_3, DIR) |= BIT(PIN_LED_3);
#endif

#if defined(PORT_LED_3) // when available, this LED indicates power-on
    GPIO(PORT_LED_3, OUT) |= BIT(PIN_LED_3);
#endif

#ifdef SHOW_PROGRESS_ON_LED
    blink(1, SEC_TO_CYCLES * 5, LED1 | LED2);
#endif
#endif

}

extern volatile uint16_t __isr_checkpoint_cnt;

extern uint16_t __workingstack_grow_cnt;
extern uint16_t __workingstack_shrink_cnt;
extern uint16_t __workingstack_checkpoint_cnt;

//NVM static int reboot = 0;
volatile int nop_var = 2;

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD; // Stop WDT

    // Disable FRAM wait cycles to allow clock operation over 8MHz
    FRCTL0 = 0xA500 | ((1) << 4); // FRCTLPW | NWAITS_1;

    /* init FRAM */
    FRCTL0_H |= (FWPW) >> 8;

    PM5CTL0 &= ~LOCKLPM5;

    // UART
    mspconsole_init();
    __enable_interrupt();

    init_led();

#ifdef CHECKPOINT_TIMER
    checkpoint_timer_init();
    checkpoint_timer_disable();
#endif

    srand(42);
    checkpoint_init();
}

int application_main(void)
{
    unsigned i;
    value_t key;

    // Mementos can't handle globals: it restores them to .data, when they are
    // in .bss... So, for now, just keep all data on stack.
#ifdef MEMENTOS

#ifdef MEMENTOS_NONVOLATILE
    static __nv fingerprint_t filter[NUM_BUCKETS];
#else
    fingerprint_t filter[NUM_BUCKETS];
#endif

    // Can't use C initializer because it gets converted into
    // memset, but the memset linked in by GCC is of the wrong calling
    // convention, but we can't override with our own memset because C runtime
    // calls memset with GCC's calling convention. Catch 22.
    for (i = 0; i < NUM_BUCKETS; ++i)
        filter[i] = 0;
#else // !MEMENTOS
    static NVM fingerprint_t filter[NUM_BUCKETS] = {0};
    for (i = 0; i < NUM_BUCKETS; ++i)
        filter[i] = 0;
#endif // !MEMENTOS


#ifndef MEMENTOS
    init();
#endif

    /* NOP to break on */
    nop_var = 42;
    SOFTWARE_CHECKPOINT();
    checkpoint_timer_enable();

    while (1) {
        /* Task Boundary */
        TASK_CHECKPOINT();

        key = INIT_KEY;
        unsigned inserts = 0;
        for (i = 0; i < NUM_KEYS; ++i) {
            key = generate_key(key);
            bool success = insert(filter, key);
            LOG("insert: key %04x success %u\r\n", key, success);
            if (!success)
                PRINTF("insert: key %04x failed\r\n", key);
            log_filter(filter);

            inserts += success;

#ifdef CONT_POWER
            volatile uint32_t delay = 0x8ffff;
            while (delay--);
#endif
        }
        LOG("inserts/total: %u/%u\r\n", inserts, NUM_KEYS);

        key = INIT_KEY;
        unsigned members = 0;
        for (i = 0; i < NUM_KEYS; ++i) {
            key = generate_key(key);
            bool member = lookup(filter, key);
            LOG("lookup: key %04x member %u\r\n", key, member);
            if (!member) {
#ifdef PRINTF_PRINT
                fingerprint_t fp = hash_to_fingerprint(key);
                PRINTF("lookup: key %04x fp %04x not member\r\n", key, fp);
#endif
            }
            members += member;
        }
        LOG("members/total: %u/%u\r\n", members, NUM_KEYS);

        /* Task Boundary */
        TASK_CHECKPOINT();

        //print_filter(filter);
        //print_stats(inserts, members, NUM_KEYS);

#ifdef CONT_POWER
        volatile uint32_t delay = 0x8ffff;
        while (delay--);
#endif

        checkpoint_timer_disable();
        check_filter(filter);
        printf("ISR count: %d\n", __isr_checkpoint_cnt);
        printf("Task Checkpoint count: %d\n", __task_checkpoint_cnt);
        printf("Workingstack checkpoint count: %d\n", __workingstack_checkpoint_cnt);
        printf("Workingstack count: %d\n", __workingstack_grow_cnt);
        printf("Workingstack restore count: %d\n", __workingstack_shrink_cnt);
        /* Copy line */
        // name, Working stack size, vmem size, checkpoints, task checkpoints, isr checkpoints, workingstack checkpoints, workingstack grow, workingstack shrink
        printf("Cuckoo, %d, %d, %d, %d, %d, %d, %d, %d\n",
                WORKINGSTACK_SIZE,
                VIRTUALMEM_STRATEGY_SIZE,
                __software_checkpoint_cnt,
                __task_checkpoint_cnt,
                __isr_checkpoint_cnt,
                __workingstack_checkpoint_cnt,
                __workingstack_grow_cnt,
                __workingstack_shrink_cnt);

#ifdef EXIT_DONE
        return 0;
#else
        while (1);
#endif
    }

    return 0;
}

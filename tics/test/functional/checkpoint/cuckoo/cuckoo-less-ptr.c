#define EXIT_DONE
//#define LOG_PRINT
#define PRINTF_PRINT
#define BLOCK_PRINT
//#define BLOCK_LOG_PRINT

#ifdef MSP430
#include <msp430.h>
#include "pins.h"
#endif

#include "driverlib.h"
#include "nvm.h"
#include "arch.h"
#include "checkpoint.h"
#include "stackpool.h"
#include "virtualaddr.h"
#include "virtualmem.h"

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

static hash_t djb_hash(uint8_t* data, unsigned len)
{
   uint32_t hash = 5381;
   unsigned int i;

   //for(i = 0; i < len; data++, i++)
   for(i = 0; i < len; data++, i++)
      hash = ((hash << 5) + hash) + (*data);

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

    fingerprint_t fp = hash_to_fingerprint(key);

    /* Task Boundary */

    index_t index1 = hash_key_to_index(key);

    /* Task Boundary */

    index_t fp_hash = hash_fp_to_index(fp);
    index_t index2 = index1 ^ fp_hash;

    /* Task Boundary */

    LOG("insert: key %04x fp %04x h %04x i1 %u i2 %u\r\n",
        key, fp, fp_hash, index1, index2);

    fp1 = (V2A(filter))[index1];
    LOG("insert: fp1 %04x\r\n", fp1);
    if (!fp1) { // slot 1 is free
        VMEM_WR((V2A(filter))[index1]) = fp;
    } else {
        fp2 = (V2A(filter))[index2];
        LOG("insert: fp2 %04x\r\n", fp2);
        if (!fp2) { // slot 2 is free
            VMEM_WR((V2A(filter))[index2]) = fp;
        } else { // both slots occupied, evict
            if (rand() & 0x80) { // don't use lsb because it's systematic
                index_victim = index1;
                fp_victim = fp1;
            } else {
                index_victim = index2;
                fp_victim = fp2;
            }

            LOG("insert: evict [%u] = %04x\r\n", index_victim, fp_victim);
            VMEM_WR((V2A(filter))[index_victim]) = fp; // evict victim

            do { // relocate victim(s)
                /* Task Boundary */

                fp_hash_victim = hash_fp_to_index(fp_victim);
                index_victim = index_victim ^ fp_hash_victim;

                /* Task Boundary */

                fp_next_victim = (V2A(filter))[index_victim];
                VMEM_WR((V2A(filter))[index_victim]) = fp_victim;

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

    fingerprint_t fp = hash_to_fingerprint(key);

    /* Task Boundary */

    index_t index1 = hash_key_to_index(key);

    /* Task Boundary */

    index_t fp_hash = hash_fp_to_index(fp);
    index_t index2 = index1 ^ fp_hash;

    /* Task Boundary */

    LOG("lookup: key %04x fp %04x h %04x i1 %u i2 %u\r\n",
        key, fp, fp_hash, index1, index2);

    return (V2A(filter))[index1] == fp || (V2A(filter))[index2] == fp;
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

NVM int reboot = 0;

int main()
{
    WDTCTL = WDTPW | WDTHOLD; // Stop WDT

    if (reboot == 0) {
        checkpoint_full();
        reboot = 1;
    } else {
        checkpoint_restore();
    }

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
    static __nv fingerprint_t filter[NUM_BUCKETS];
#endif // !MEMENTOS


#ifndef MEMENTOS
    init();
#endif

    while (1) {
        /* Task Boundary */

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
                fingerprint_t fp = hash_to_fingerprint(key);
                PRINTF("lookup: key %04x fp %04x not member\r\n", key, fp);
            }
            members += member;
        }
        LOG("members/total: %u/%u\r\n", members, NUM_KEYS);

        /* Task Boundary */

        print_filter(filter);
        print_stats(inserts, members, NUM_KEYS);

#ifdef CONT_POWER
        volatile uint32_t delay = 0x8ffff;
        while (delay--);
#endif

#ifdef EXIT_DONE
        return 0;
#else
        while (1);
#endif
    }

    return 0;
}

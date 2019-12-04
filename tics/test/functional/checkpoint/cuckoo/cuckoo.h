#ifndef CUCKOO_H
#define CUCKOO_H

#include <stdint.h>

#define NUM_BUCKETS 256 // must be a power of 2
#define MAX_RELOCATIONS 8

typedef uint16_t value_t;
typedef uint16_t hash_t;
typedef uint16_t fingerprint_t;
typedef uint16_t index_t; // bucket index

#endif // CUCKOO_H

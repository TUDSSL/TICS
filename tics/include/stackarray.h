#ifndef STACKARRAY_H__
#define STACKARRAY_H__

#include <stdint.h>
#include <stddef.h>

typedef uint16_t stackarray_idx_t;
typedef char stack_block_t;

#define STACKBLOCK_REVERSE_IDX(idx_) ((STACK_BLOCK_SIZE-1) - (idx_))
extern stack_block_t stackarray[N_STACK_BLOCKS][STACK_BLOCK_SIZE];

static inline stack_block_t *stackarray_get_block(stackarray_idx_t block_idx)
{
    return &stackarray[block_idx][0];
}

static inline stackarray_idx_t stackarray_ptr2idx(stack_block_t *block)
{
    ptrdiff_t diff;
    stackarray_idx_t idx;

    // Assume block is valid and therefore has a higher address
    diff = block - (char *)stackarray;

    idx = diff / STACK_BLOCK_SIZE;

    return idx;
}

// Can be positive or negative
static inline char *stackarray_relative_block_index(stack_block_t *block, int16_t ridx)
{
    int16_t offset;
    stack_block_t *relative_block;

    offset = ridx * STACK_BLOCK_SIZE;
    relative_block = &block[offset];

    return relative_block;
}

static inline stackarray_idx_t stackarray_get_next_block_idx(stackarray_idx_t idx)
{
    stackarray_idx_t next_block_idx;

    next_block_idx = idx - 1; // The next block is 1 down (stack grows down in memory, so also the index)

    return next_block_idx;
}

static inline int stackarray_addr_in_stackarray(char *addr)
{
    if (addr >= &stackarray[0][0] && addr < &stackarray[N_STACK_BLOCKS][0]) {
        return 1;
    }
    return 0;
}

#endif /* STACKARRAY_H__ */

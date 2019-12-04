//#define DEBUG_PRINT
#define DEBUG_MODULE "virtualmem"
#include "util/debug.h"

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "config.h"

#include "nvm.h"
#include "virtualmem.h"
#include "critical.h"
#include "stackarray.h"

// Intervaltree "template"
#define INTERVAL_TYPE       intptr_t
#define INTERVAL_ALLOC      virtualmem_alloc
#include "intervaltree.h"

typedef INTERVAL_TYPE vmem_addr_t;

#ifdef VIRTUALMEM_DMA
#include "dma.h"
#define VMEM_COPY   dma_copy
#pragma message "#VirtualMem: Using DMA"
#else /* !VIRTUALMEM_DMA */
#define VMEM_COPY   memcpy
#pragma message "#VirtualMem: Using memcpy"
#endif /* VIRTUALMEM_DMA */

NVM uintptr_t vmem_check_addr; // NB. Keeps changing depending on WS checkpoint
NVM uintptr_t vmem_check_addr_low = (uintptr_t)&stackarray[0][0]; // Constant

NVM vmem_entry_t *vmem_root[2] = {NULL, NULL};
vmem_entry_t **ActiveVmemRoot = &vmem_root[0];
vmem_entry_t **InactiveVmemRoot = &vmem_root[1];

NVM char VirtualMemPool[VIRTUALMEM_SIZE] = {0};
NVM size_t VirtualMemPoolIdx = 0;

void virtualmem_set_ctrl_a(void)
{
    ActiveVmemRoot = &vmem_root[0];
    InactiveVmemRoot = &vmem_root[1];
}

void virtualmem_set_ctrl_b(void)
{
    ActiveVmemRoot = &vmem_root[1];
    InactiveVmemRoot = &vmem_root[0];
}

void virtualmem_set_check_addr(uintptr_t addr)
{
    vmem_check_addr = addr;
}


#if VIRTUALMEM_STRATEGY_SIZE > 0
static void _vmem_size_strategy(vmem_addr_t *dst, vmem_addr_t *end, size_t *size)
{
    ptrdiff_t diff = *end - *dst + 1;

    /* Don't apply to stack blocks
     * If this is desired, we must make a test to see whether the variable
     * expansion grows across a stack block boundary
     */
    if (stackarray_addr_in_stackarray((char *)*dst)) {
        return;
    }

    if (diff >= VIRTUALMEM_STRATEGY_SIZE) {
        return;
    }

    *end = *end + (VIRTUALMEM_STRATEGY_SIZE - diff);
    *size = VIRTUALMEM_STRATEGY_SIZE;
}

#define vmem_size_strategy _vmem_size_strategy

#else /* !VIRTUALMEM_STRATEGY_SIZE */

// No size strategy, use the size of the variable to be buffered
#define vmem_size_strategy(dst_, end_, size_)

#endif /* VIRTUALMEM_STRATEGY_SIZE */

/*
 * TODO
 *  The VirtalMemory pool can be in NVM, the VirtualMemPoolIdx has to be
 *  correctly maintained. It can be in volatile memory and should then be double
 *  buffered at a checkpoint.
 *  At every checkpoint the alloc should start empty, when a checkpoint is made
 *  the data should be written back to the original location in NVM. (This IS
 *  the double buffer of the NVM data).
 */
void *virtualmem_alloc(size_t size)
{
    void *res;
    size_t new_idx = VirtualMemPoolIdx + size;

    // Find the next empty spot, NULL if no room left
    if (new_idx >= VIRTUALMEM_SIZE) {
        // Not enough room
        return NULL;
    }

    res = &VirtualMemPool[VirtualMemPoolIdx];
    VirtualMemPoolIdx = new_idx;

    return res;
}

// After a writeback all memory is freed
void virtualmem_free(void)
{
    *ActiveVmemRoot = NULL;
    VirtualMemPoolIdx = 0;
}

static void virtualmem_writeback_callback(char *data, INTERVAL_TYPE low, INTERVAL_TYPE high, INTERVAL_TYPE max, uint16_t valid_node)
{
    if (valid_node) {
        dbgprintf("Writeback ["DBG_PTR_PRINT", "DBG_PTR_PRINT"]\n", DBG_PTR(low), DBG_PTR(high));
        size_t size = high-low+1;
        char *dst_ptr = (char *)low;
        VMEM_COPY(dst_ptr, data, size);
    } else {
        dbgprintf("Registered node not valid ["DBG_PTR_PRINT", "DBG_PTR_PRINT"]\n", DBG_PTR(low), DBG_PTR(high));
    }
}

void virtualmem_writeback(void)
{
    if (*ActiveVmemRoot != NULL) {
        dbgprintf("Writeback virtual memory\n");
        vmem_inorder(*ActiveVmemRoot, &virtualmem_writeback_callback);
        virtualmem_free();
    }
}

#if 0
static void virtualmem_workingstack_writeback_callback(char *data, INTERVAL_TYPE low, INTERVAL_TYPE high, INTERVAL_TYPE max)
{
    size_t size = high-low+1;
    char *dst_ptr = (char *)low;
    char *dst_ptr_stack;

    vidx_t vidx;

    vidx = virtualindex_get();
    char *ws_block = virtualaddr_sb_tlb_lookup(vidx);

    //if (dst_ptr >= ws_block && dst_ptr < (ws_block+STACK_BLOCK_SIZE)) {
    if ( dst_ptr >= (ws_block-STACK_BLOCK_SIZE) && dst_ptr < ws_block) {

        dst_ptr_stack = vidx2stack(vidx) - (ws_block - dst_ptr);

        dbgprintf("Writeback Stack ["DBG_PTR_PRINT", "DBG_PTR_PRINT"] -> ["DBG_PTR_PRINT"]\n",
                DBG_PTR(low), DBG_PTR(high), DBG_PTR(dst_ptr_stack));
        VMEM_COPY(dst_ptr_stack, dst_ptr, size);
    }
}

void virtualmem_workingstack_writeback(void)
{
    if (*ActiveVmemRoot != NULL) {
        dbgprintf("Writeback workingstack changes\n");
        vmem_inorder(*ActiveVmemRoot, &virtualmem_workingstack_writeback_callback);
    }
}
#endif

static void __virtualmem_error(void)
{
    dbgprintf("Virtual memory addressing error!!\n");
    while (1) {
        dbgprintf(".");
    }
}

// TODO size must be > 0
#include <stdio.h>
void *virtualmem_translate_addr(char *dst_ptr, size_t size, bool write)
{
    /*
     *  Test if the pointer range is already in the virtual memory
     *      [Y] if the range is inside the search result -> overwrite vmen data
     *      [N] if range is partially outsize search result -> writeback vmem data
     *  If not in range -> Add memory to vmem
     */
    vmem_addr_t dst = (vmem_addr_t)dst_ptr;
    vmem_addr_t end = dst + size - 1;
    vmem_entry_t *vmem_entry;

    dbgprintf("Translate req: ["DBG_PTR_PRINT" - "DBG_PTR_PRINT"] size=%d\n",
            DBG_PTR(dst), DBG_PTR(end), (int)size);

    CRITICAL_START();
    vmem_entry = vmem_overlap_search(*ActiveVmemRoot, dst, end);

    if (vmem_entry != NULL) {
        /* Address interval already within virtual memory */
        if (dst >= vmem_entry->interval.low && end <= vmem_entry->interval.high) {
            /* Data in address interval is already in virual memory */
#ifdef DEBUG_PRINT
            size_t data_idx = dst - vmem_entry->interval.low;
            dbgprintf("Already in VM, at data addr "DBG_PTR_PRINT"\n",
                    DBG_PTR(&vmem_entry->data[data_idx]));
#endif
            //return &vmem_entry->data[data_idx];
            CRITICAL_END();
            return dst_ptr;
        } else {
            /* Interval is partially outside of an interval in virtual memory */
            // TODO writeback + checkpoint etc, or add a new interval with the rest?
            dbgprintf("Overlap out of range (has "
                    "["DBG_PTR_PRINT", "DBG_PTR_PRINT"] req ["DBG_PTR_PRINT", "DBG_PTR_PRINT"])\n",
                    DBG_PTR(vmem_entry->interval.low),
                    DBG_PTR(vmem_entry->interval.high),
                    DBG_PTR(dst), DBG_PTR(end));
            //virtualmem_writeback();
            virtualmem_free();
        }
    }

    /* If no write is required, return the original address */
    if (!write) {
        dbgprintf("No write, returning original pointer: "DBG_PTR_PRINT"\n",
                DBG_PTR(dst_ptr));
        CRITICAL_END();
        return dst_ptr;
    }

    /* Decide the size of the data to cache (can be used to implement paging) */
    vmem_size_strategy(&dst, &end, &size);

    /* Address interval not in virtual memory, add it if enough room */
    *ActiveVmemRoot = vmem_insert(&vmem_entry, *ActiveVmemRoot, dst, end);
    if (vmem_entry == NULL) {
        /* could not allocate space */
        // TODO writeback + checkpoint etc
        dbgprintf("Requested entry too large (req ["DBG_PTR_PRINT", "DBG_PTR_PRINT"])\n",
                DBG_PTR(dst), DBG_PTR(end));
        printf("Virtualmem full!!\n"); // TODO handle nicely
        //virtualmem_writeback();
        virtualmem_free();
        CRITICAL_END();
        return virtualmem_translate_addr(dst_ptr, size, write); // retry with empty virtual memory
    } else {
        dbgprintf("Add entry, copied data at "DBG_PTR_PRINT" [n=%d], to addr: "DBG_PTR_PRINT"\n",
                DBG_PTR(dst_ptr),
                (int)size,
                DBG_PTR(vmem_entry->data));
        VMEM_COPY(vmem_entry->data, dst_ptr, size); // Copy the original content
        vmem_entry->valid_node = 1;
        CRITICAL_END();
        return dst_ptr;
    }
    __virtualmem_error();
    CRITICAL_END();
}


#define INTERVALTREE_DEBUG
#ifdef INTERVALTREE_DEBUG
#include <stdio.h>
static void intervaltree_print_callback(char *data, INTERVAL_TYPE low, INTERVAL_TYPE high, INTERVAL_TYPE max, uint16_t valid_node)
{
    (void)data;
    dbgprintf("[valid=%d "DBG_PTR_PRINT", "DBG_PTR_PRINT"] max="DBG_PTR_PRINT"\n",
            valid_node, DBG_PTR(low), DBG_PTR(high), DBG_PTR(max));
}
void intervaltree_print(void)
{
    intervaltree_inorder(*ActiveVmemRoot, &intervaltree_print_callback);
}
#endif


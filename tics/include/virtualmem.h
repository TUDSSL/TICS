#ifndef VIRTUALMEM_H__
#define VIRTUALMEM_H__

#ifndef DEBUG_MODULE
#define DEBUG_MODULE "virtualmem"
#include "util/debug.h"
#endif

#include <stdbool.h>
#include "config.h"
#include "nvm.h"

#define vmem_overlap_search intervaltree_overlap_search
#define vmem_insert         intervaltree_insert
#define vmem_inorder        intervaltree_inorder
#define vmem_entry_t        interval_node_t

extern uintptr_t vmem_check_addr;
extern uintptr_t vmem_check_addr_low;

#define virtualmem_get_check_addr() vmem_check_addr
#define virtualmem_get_check_addr_low() vmem_check_addr_low
void virtualmem_set_check_addr(uintptr_t addr);


extern char VirtualMemPool[VIRTUALMEM_SIZE];
extern size_t VirtualMemPoolIdx;

void virtualmem_set_ctrl_a(void);
void virtualmem_set_ctrl_b(void);

void *virtualmem_alloc(size_t size);
void *virtualmem_translate_addr(char *dst_ptr, size_t size, bool write);
void virtualmem_writeback(void);
void virtualmem_free(void);

void virtualmem_workingstack_writeback(void);

/*
 * Log if the virtualmem is outside of:
 *  virtualmem_get_check_addr_low() and virtualmem_get_check_addr()
 */
static inline void * virtualmem_translate(char *ref, size_t size, bool write)
{
    intptr_t ref_addr = (intptr_t)ref;
    if (ref_addr < virtualmem_get_check_addr_low() || ref_addr > virtualmem_get_check_addr()) {
#if 0
#ifdef DEBUG_PRINT
        if (ref_addr >= (intptr_t)&VirtualMemPool[0] &&
           (ref_addr+size-1) <= (intptr_t)&VirtualMemPool[VIRTUALMEM_SIZE-1]) {
            dbgprintf("Access to variable in buffer (0x%lx)\n", (long int)ref_addr);
        }
#endif /* DEBUG_PRINT */
#endif
        //ref = vm_translate_addr(ref, size);
        dbgprintf("Addr: "DBG_PTR_PRINT" Size: %d\n", DBG_PTR(ref), (int)size);
        virtualmem_translate_addr(ref, size, write);
    }
    return ref;
}


/* Access buffered variables */
#define VMEM_RD(var_)   (*((__typeof__(var_) *)virtualmem_translate((char *)&(var_), sizeof(var_), false)))
#define VMEM_WR(var_)   (*((__typeof__(var_) *)virtualmem_translate((char *)&(var_), sizeof(var_), true)))

#endif /* VIRTUALMEM_H__ */

#ifndef ARCH_CONFIG_H__
#define ARCH_CONFIG_H__

#include <stdint.h>
#include "config.h"

/* Architecture specific configuration */

#define N_REGISTERS 16

typedef uint16_t reg_t;
typedef int16_t atomic_flag_t;

/*
 * Assembly instructions
 */
#ifdef CODE_MODEL_SMALL

#define MSP_BRANCH_INSTR "br"
#define MSP_MOV_ADDR_INSTR "mov.w"
#define MSP_POPM_ADDR_INSTR "popm.w"
#define MSP_PUSHM_ADDR_INSTR "pushm.w"

#else /* !CODE_MODEL_SMALL */

#define MSP_BRANCH_INSTR "bra"
#define MSP_MOV_ADDR_INSTR "mova"
#define MSP_POPM_ADDR_INSTR "popm.a"
#define MSP_PUSHM_ADDR_INSTR "pushm.a"

#endif /* CODE_MODEL_SMALL */

/*
 * Virtual address layout
 */
#ifdef CODE_MODEL_SMALL

// Pointer is 16 bits on the msp430
#define VADDR_OFFSET_SHIFT  0
#define VADDR_OFFSET_MASK   (2048-1)    // 2^11 - 1, 11 bits used as offset

#define VADDR_VIDX_SHIFT    11
#define VADDR_VIDX_MASK     (16UL-1) << VADDR_VIDX_SHIFT    // 2^4 - 1, 4 bits used for idx, max 15 blocks

#define VADDR_IDENT_SHIFT   15UL  // 16th bit 1 means it's a virtual address
#define VADDR_IDENT_MASK    (1UL << VADDR_IDENT_SHIFT) // only 1 bit

#else /* CODE_MODEL_LARGE */

// Pointer is 20 bits on the msp430x
#define VADDR_OFFSET_SHIFT  0
#define VADDR_OFFSET_MASK   (2048-1)    // 2^11 - 1, 11 bits used as offset

#define VADDR_VIDX_SHIFT    11
#define VADDR_VIDX_MASK     (256UL-1) << VADDR_VIDX_SHIFT    // 2^8 - 1, 8 bits used for idx, max 256 blocks

#define VADDR_IDENT_SHIFT   19UL  // 20st bit 1 means it's a virtual address
#define VADDR_IDENT_MASK    (1UL << VADDR_IDENT_SHIFT) // only 1 bit

#endif /* CODE_MODEL_SMALL */


#endif /* ARCH_CONFIG_H__ */

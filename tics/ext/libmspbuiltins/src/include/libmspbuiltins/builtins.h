#ifndef _MSP_BUILTINS_H
#define _MSP_BUILTINS_H

#include <stdint.h>

#ifdef __clang__

#define __delay_cycles(n) \
    __asm__ volatile ( \
      "pushm.a #1, r13\n" \
      "mov     %[count], r13\n" \
      "dec     r13\n" \
      "jnz     $-2\n" \
      "popm.a  #1, r13\n" \
      "nop\n" \
      : : [count] "i" ((n) / 3 - 3) \
    )

#define __bic_SR_register_on_exit(bits) \
    __asm__ volatile ( \
        "bic     %[b],    24(r1)\n" \
        : : [b] "i" (bits) \
    )

#endif // __clang__

// These are fake "built-ins" for Clang: implemented in C in a trivial way, and
// compiled with gcc. We do this because it is easier than implementing actual
// builtins Clang calls that come from compiler-rt (few are implemented there).
uint32_t mspbuiltins_mult32(uint32_t a, uint32_t b);
uint16_t mspbuiltins_div16(uint16_t a, uint16_t b);

#endif // _MSP_BUILTINS_H

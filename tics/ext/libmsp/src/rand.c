// A simple LCG pseudo-random-number generator with state persistance across
// reboots. Currently, the seed is static (could seed from libwispbase random
// API based on ADC). This exists primarily because libc's srand/rand do
// not seem to produce random bits even if seeded from ADC on each reboot.

#include "rand.h"

#include "mem.h"

#define RAND_LCG_A 1103515245ULL
#define RAND_LCG_C 12345

__nv static volatile unsigned seed = 42;

unsigned msp_rand()
{
  seed = ((unsigned)RAND_LCG_A) * seed + RAND_LCG_C;
  return seed >> 8; // do not use low-order bits
}

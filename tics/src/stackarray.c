#include "config.h"

#include "stackarray.h"

#define STACK_SECTION __attribute__((section(".stackarray")))
STACK_SECTION char stackarray[N_STACK_BLOCKS][STACK_BLOCK_SIZE] __attribute__((aligned (4)));

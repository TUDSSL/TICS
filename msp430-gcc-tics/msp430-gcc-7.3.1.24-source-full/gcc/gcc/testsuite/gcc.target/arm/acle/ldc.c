/* Test the ldc ACLE intrinsic.  */

/* { dg-do assemble } */
/* { dg-options "-save-temps" } */
/* { dg-require-effective-target arm_coproc1_ok } */

#include "arm_acle.h"

extern void * p;

void test_ldc (void)
{
  __arm_ldc (10, 1, p + 4);
  __arm_ldc (11, 1, p + 1024);
}

/* { dg-final { scan-assembler "ldc\tp10, CR1, \[r\[0-9\]+" } } */
/* { dg-final { scan-assembler "ldc\tp11, CR1, \[r\[0-9\]+\]\n" } } */

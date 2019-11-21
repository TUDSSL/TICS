/* { dg-do link } */
/* { dg-skip-if "" { "*-*-*" } { "-mhwmult=*" } { "" } } */
/* { dg-options "-I. -mhwmult=f5series -mcpu=msp430xv2 -mmcu=msp430_20" } */
/* { dg-excess-errors "does not have hardware multiply support" } */

#include "main.c"

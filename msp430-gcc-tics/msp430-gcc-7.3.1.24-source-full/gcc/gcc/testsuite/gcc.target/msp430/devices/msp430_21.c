/* { dg-do link } */
/* { dg-skip-if "" { *-*-* } { "-mhwmult=*" "-mcpu=msp430" } { "" } } */
/* { dg-options "-I. -mhwmult=16bit -mcpu=msp430xv2 -mmcu=msp430_21" } */

#include "main.c"

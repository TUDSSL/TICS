/* { dg-do link } */
/* { dg-skip-if "" { *-*-* } { "-mhwmult=*" "-mcpu=msp430" } { "" } } */
/* { dg-options "-I. -mhwmult=32bit -mcpu=msp430xv2 -mmcu=msp430_24" } */

#include "main.c"

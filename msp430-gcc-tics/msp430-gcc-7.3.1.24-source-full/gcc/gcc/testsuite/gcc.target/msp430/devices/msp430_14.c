/* { dg-do link } */
/* { dg-skip-if "" { *-*-* } { "-mhwmult=*" "-mcpu=msp430" } { "" } } */
/* { dg-options "-I. -mhwmult=32bit -mcpu=msp430x -mmcu=msp430_14" } */

#include "main.c"

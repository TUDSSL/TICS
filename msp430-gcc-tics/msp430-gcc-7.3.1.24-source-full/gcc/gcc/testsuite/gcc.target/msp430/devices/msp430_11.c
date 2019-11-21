/* { dg-do link } */
/* { dg-skip-if "" { *-*-* } { "-mhwmult=*" "-mcpu=msp430" } { "" } } */
/* { dg-options "-I. -mhwmult=16bit -mcpu=msp430x -mmcu=msp430_11" } */

#include "main.c"

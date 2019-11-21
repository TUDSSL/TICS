/* { dg-do link } */
/* { dg-skip-if "" { *-*-* } { "-mhwmult=*" "-mlarge" "-mcpu=msp430x" "-mcpu=msp430xv2" } { "" } } */
/* { dg-options "-I. -mhwmult=f5series -mcpu=msp430 -mmcu=msp430_08" } */

#include "main.c"

/* { dg-do link } */
/* { dg-skip-if "" { *-*-* } { "-mhwmult=*" "-mlarge" "-mcpu=msp430x" "-mcpu=msp430xv2" } { "" } } */
/* { dg-options "-I. -mhwmult=16bit -mcpu=msp430 -mmcu=msp430_02" } */

#include "main.c"

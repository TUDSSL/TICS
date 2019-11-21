/* { dg-do link } */
/* { dg-skip-if "" { *-*-* } { "-mhwmult=*" "-mlarge" "-mcpu=msp430x" "-mcpu=msp430xv2" } { "" } } */
/* { dg-options "-I. -mhwmult=none -mcpu=msp430 -mmcu=msp430_00" } */

#include "main.c"

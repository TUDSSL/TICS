/* { dg-do link } */
/* { dg-skip-if "" { *-*-* } { "-mhwmult=*" "-mlarge" "-mcpu=msp430x" "-mcpu=msp430xv2" } { "" } } */
/* { dg-options "-I. -mhwmult=32bit -mcpu=msp430 -mmcu=msp430_04" } */

#include "main.c"

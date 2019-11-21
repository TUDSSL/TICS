/* { dg-do link } */
/* { dg-skip-if "" { "*-*-*" } { "-mhwmult=*" } { "" } } */
/* { dg-options "-I. -mhwmult=16bit -mcpu=msp430xv2 -mmcu=msp430_28" } */
/* { dg-excess-errors "supports 32-bit (5xx) hardware multiply" } */

#include "main.c"

/* { dg-do link } */
/* { dg-skip-if "" { *-*-* } { "-mcpu=msp430x" "-mcpu=msp430xv2" "-mhwmult=*" } { "-mhwmult=none" "-mhwmult=32bit" } } */
/* { dg-options "-mmcu=msp430f4783 -mdisable-device-warnings" } */

int main (void)
{
  while (1);
  return 0;
}

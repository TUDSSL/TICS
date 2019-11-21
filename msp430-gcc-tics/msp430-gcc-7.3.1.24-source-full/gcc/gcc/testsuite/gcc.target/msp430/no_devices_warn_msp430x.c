/* { dg-do link } */
/* { dg-skip-if "" { *-*-* } { "-mcpu=msp430" "-mhwmult=*" } { "-mhwmult=none" "-mhwmult=f5series" } } */
/* { dg-options "-mmcu=msp430fr5969 -mdisable-device-warnings" } */

int main (void)
{
  while (1);
  return 0;
}

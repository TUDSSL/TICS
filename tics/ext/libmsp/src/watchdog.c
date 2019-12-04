#include <msp430.h>

#include "watchdog.h"

static uint8_t watchdog_bits;

void msp_watchdog_enable(uint8_t bits)
{
    WDTCTL = WDTPW | WDTCNTCL | bits;
    watchdog_bits = bits;
}

void msp_watchdog_disable()
{
     WDTCTL = WDTPW + WDTHOLD;
}

void msp_watchdog_kick()
{
    WDTCTL = WDTPW | WDTCNTCL | watchdog_bits;
}

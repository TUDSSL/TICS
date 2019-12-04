#include <stdio.h>
#include <stdint.h>
#include "checkpoint.h"

#include "driverlib.h"

volatile int result;

volatile int test_data[16] = {42};
volatile int test_bss[16];

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD; // Stop WDT

    result = checkpoint();
    if (result == 0) {
        checkpoint_restore();
    }

    test_data[1] = 12;
    test_bss[1] = 13;

    checkpoint_restore();

    result = checkpoint();
    if (result == 0) {
        checkpoint_restore();
    }
}

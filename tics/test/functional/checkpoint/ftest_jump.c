#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "driverlib.h"

#include "nvm.h"
#include "arch.h"
#include "checkpoint.h"
#include "stackarray.h"
#include "virtualmem.h"
#include "jump.h"

#define WSS __attribute__((segstack))

NVM visp_jmp_buf buf;

int dummy(void)
{
    visp_longjmp(&buf, 10);
    return 42;
}

WSS int dummy_ws(void)
{
    visp_longjmp(&buf, 20);
    return 42;
}

WSS int dummy_ws_cp(void)
{
    checkpoint();
    visp_longjmp(&buf, 30);
    return 42;
}


volatile int retval;

void mspconsole_init(void);
void main_application(void);


int main(void)
{
    WDTCTL = WDTPW | WDTHOLD; // Stop WDT

    /* init FRAM */
    FRCTL0_H |= (FWPW) >> 8;

    PM5CTL0 &= ~LOCKLPM5;

    // UART
    mspconsole_init();
    __enable_interrupt();

    checkpoint_init();
}

int application_main(void)
{
    int r;

    checkpoint();

    /* Basic jump functionality */
    r = visp_setjmp(&buf);
    printf("r = %d\n", r);
    if (r != 42) {
        visp_longjmp(&buf, 42);
    }

    /* Jump from a function back */
    r = visp_setjmp(&buf);
    printf("r = %d\n", r);
    if (r != 10) {
        dummy();
    }

    /* Jump from a different workingstack */
    r = visp_setjmp(&buf);
    printf("r = %d\n", r);
    if (r != 20) {
        dummy_ws();
    }

    /* Jump from a different workingstack after a checkpoint */
    r = visp_setjmp(&buf);
    printf("r = %d\n", r);
    if (r != 30) {
        dummy_ws_cp();
    }

#if 0
    /* Functions that do not cross the checkpoint return boundary */
    retval = dummy(); // 42

    /* Functions that cross the checkpoint return boundary */
    retval = dummy_cp(); // 42

    printf("Return value: %d\n", retval);
#endif

    while(1) {
        printf("Done!\n");
    }
}

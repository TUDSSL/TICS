#ifndef LIBMSP_WATCHDOG_H
#define LIBMSP_WATCHDOG_H

#include <stdint.h>

void msp_watchdog_enable(uint8_t bits);
void msp_watchdog_disable();
void msp_watchdog_kick();

#endif // LIBMSP_WATCHDOG_H

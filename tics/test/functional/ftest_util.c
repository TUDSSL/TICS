#include "msp430.h"
#include "config.h"
#include "nvm.h"
#include "ftest_util.h"

#include "checkpoint.h"
// 1 == restore, 0 == checkpoint
NVM static int last_checkpoint_return_restore = 1;
void checkpoint_and_restore(void) {
    if (last_checkpoint_return_restore) {
        // Last operation was a restore, now checkpoint
        last_checkpoint_return_restore = checkpoint();
    } else {
        // Last operation was a checkpoint, now restore
        checkpoint_restore();
    }
}

void reset_mcu(void)
{
    WDTCTL = 0;
}


NVM static int last_checkpoint_return_reset = 1;
void checkpoint_and_reset(void) {
    if (last_checkpoint_return_reset) {
        // Last operation was a restore, now checkpoint
        last_checkpoint_return_reset = checkpoint();
    } else {
        // Last operation was a checkpoint, now reset
        reset_mcu();
    }
}
void checkpoint_and_reset_safe(void) {
    if (last_checkpoint_return_reset) {
        // Last operation was a restore, now checkpoint
        last_checkpoint_return_reset = checkpoint_safe();
    } else {
        // Last operation was a checkpoint, now reset
        reset_mcu();
    }
}


#include "checkpoint_stack.h"
extern char __stack;
#define CLEAR_RAM_START   0x1C00
void clear_volatile_mem(void)
{
    char *sp, *ram;
    uintptr_t sp_addr, ram_addr;
    ptrdiff_t clear_size;
    uint32_t i;

    ram = (char *)CLEAR_RAM_START;
    sp = checkpoint_get_sp();

    ram_addr = CLEAR_RAM_START;
    sp_addr = (uintptr_t)&sp;
    clear_size = sp_addr - ram_addr;

    // Don't use memset to avoid extra stack usage
    // Stack required for the for loop should have been allocated at the start
    // of the function.
    for (i=0; i<clear_size; i++) {
        ram[i] = 0;
    }
}


#include "gpio.h"
void PMM_unlockLPM5 (void)
{
	//Direct register access to avoid compiler warning - #10420-D
	//For FRAM devices, at start up, the GPO power-on default
	//high-impedance mode needs to be disabled to activate previously
	//configured port settings. This can be done by clearing the LOCKLPM5
	//bit in PM5CTL0 register

	PM5CTL0 &= ~LOCKLPM5;
}
static const uint16_t GPIO_PORT_TO_BASE[] = {
    0x00,
#if defined(__MSP430_HAS_PORT1_R__)
    __MSP430_BASEADDRESS_PORT1_R__,
#elif defined(__MSP430_HAS_PORT1__)
    __MSP430_BASEADDRESS_PORT1__,
#else
    0xFFFF,
#endif
#if defined(__MSP430_HAS_PORT2_R__)
    __MSP430_BASEADDRESS_PORT2_R__,
#elif defined(__MSP430_HAS_PORT2__)
    __MSP430_BASEADDRESS_PORT2__,
#else
    0xFFFF,
#endif
#if defined(__MSP430_HAS_PORT3_R__)
    __MSP430_BASEADDRESS_PORT3_R__,
#elif defined(__MSP430_HAS_PORT3__)
    __MSP430_BASEADDRESS_PORT3__,
#else
    0xFFFF,
#endif
#if defined(__MSP430_HAS_PORT4_R__)
    __MSP430_BASEADDRESS_PORT4_R__,
#elif defined(__MSP430_HAS_PORT4__)
    __MSP430_BASEADDRESS_PORT4__,
#else
    0xFFFF,
#endif
#if defined(__MSP430_HAS_PORT5_R__)
    __MSP430_BASEADDRESS_PORT5_R__,
#elif defined(__MSP430_HAS_PORT5__)
    __MSP430_BASEADDRESS_PORT5__,
#else
    0xFFFF,
#endif
#if defined(__MSP430_HAS_PORT6_R__)
    __MSP430_BASEADDRESS_PORT6_R__,
#elif defined(__MSP430_HAS_PORT6__)
    __MSP430_BASEADDRESS_PORT6__,
#else
    0xFFFF,
#endif
#if defined(__MSP430_HAS_PORT7_R__)
    __MSP430_BASEADDRESS_PORT7_R__,
#elif defined(__MSP430_HAS_PORT7__)
    __MSP430_BASEADDRESS_PORT7__,
#else
    0xFFFF,
#endif
#if defined(__MSP430_HAS_PORT8_R__)
    __MSP430_BASEADDRESS_PORT8_R__,
#elif defined(__MSP430_HAS_PORT8__)
    __MSP430_BASEADDRESS_PORT8__,
#else
    0xFFFF,
#endif
#if defined(__MSP430_HAS_PORT9_R__)
    __MSP430_BASEADDRESS_PORT9_R__,
#elif defined(__MSP430_HAS_PORT9__)
    __MSP430_BASEADDRESS_PORT9__,
#else
    0xFFFF,
#endif
#if defined(__MSP430_HAS_PORT10_R__)
    __MSP430_BASEADDRESS_PORT10_R__,
#elif defined(__MSP430_HAS_PORT10__)
    __MSP430_BASEADDRESS_PORT10__,
#else
    0xFFFF,
#endif
#if defined(__MSP430_HAS_PORT11_R__)
    __MSP430_BASEADDRESS_PORT11_R__,
#elif defined(__MSP430_HAS_PORT11__)
    __MSP430_BASEADDRESS_PORT11__,
#else
    0xFFFF,
#endif
    0xFFFF,
#if defined(__MSP430_HAS_PORTJ_R__)
    __MSP430_BASEADDRESS_PORTJ_R__
#elif defined(__MSP430_HAS_PORTJ__)
    __MSP430_BASEADDRESS_PORTJ__
#else
    0xFFFF
#endif
};
void GPIO_setAsOutputPin(uint8_t selectedPort, uint16_t selectedPins) {

    uint16_t baseAddress = GPIO_PORT_TO_BASE[selectedPort];

    #ifndef NDEBUG
    if(baseAddress == 0xFFFF) {
        return;
    }
    #endif

    // Shift by 8 if port is even (upper 8-bits)
    if((selectedPort & 1) ^ 1) {
        selectedPins <<= 8;
    }

    HWREG16(baseAddress + OFS_PASEL0) &= ~selectedPins;
    HWREG16(baseAddress + OFS_PASEL1) &= ~selectedPins;
    HWREG16(baseAddress + OFS_PADIR) |= selectedPins;

    return;
}

void GPIO_setOutputHighOnPin (uint8_t selectedPort,
                                   uint16_t selectedPins) {

    uint16_t baseAddress = GPIO_PORT_TO_BASE[selectedPort];

    #ifndef NDEBUG
    if(baseAddress == 0xFFFF) {
        return;
    }
    #endif

    // Shift by 8 if port is even (upper 8-bits)
    if((selectedPort & 1) ^ 1) {
        selectedPins <<= 8;
    }

    HWREG16(baseAddress + OFS_PAOUT) |= selectedPins;
}

void GPIO_setOutputLowOnPin (uint8_t selectedPort, uint16_t selectedPins) {

    uint16_t baseAddress = GPIO_PORT_TO_BASE[selectedPort];

    #ifndef NDEBUG
    if(baseAddress == 0xFFFF) {
        return;
    }
    #endif

    // Shift by 8 if port is even (upper 8-bits)
    if((selectedPort & 1) ^ 1) {
        selectedPins <<= 8;
    }

    HWREG16(baseAddress + OFS_PAOUT) &= ~selectedPins;
}

void GPIO_toggleOutputOnPin (uint8_t selectedPort, uint16_t selectedPins) {

    uint16_t baseAddress = GPIO_PORT_TO_BASE[selectedPort];

    #ifndef NDEBUG
    if(baseAddress == 0xFFFF) {
        return;
    }
    #endif

    // Shift by 8 if port is even (upper 8-bits)
    if((selectedPort & 1) ^ 1) {
        selectedPins <<= 8;
    }

    HWREG16(baseAddress + OFS_PAOUT) ^= selectedPins;
}


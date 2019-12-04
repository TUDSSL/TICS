#include <msp430.h>

#include "checkpoint_timer.h"
#include "timer_a.h"
#include "config.h"

//#define TIMER_CCR0 1250  // 1MHz/8/1250 = 100Hz (every 10ms)
#define TIMER_CCR0 (MCLK/8/CHECKPOINT_TIMER_FREQ) // !MUST FIT IN THE TIMER REGISTER

void checkpoint_timer_enable(void)
{
#if defined(__MSP430FR5994__)
    HWREG16(TA0_BASE + OFS_TAxCCTL0)  = TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE;
#elif defined(__MSP430FR5969__)
    HWREG16(TIMER_A0_BASE + OFS_TAxCCTL0)  = TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE;
#else
    #error "Device not supported, specify timer"
#endif
}

void checkpoint_timer_disable(void)
{
#if defined(__MSP430FR5994__)
    HWREG16(TA0_BASE + OFS_TAxCCTL0)  = TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE;
#elif defined(__MSP430FR5969__)
    HWREG16(TIMER_A0_BASE + OFS_TAxCCTL0)  = TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE;
#else
    #error "Device not supported, specify timer"
#endif
}

void checkpoint_timer_init(void)
{
    // initialize Timer0_A
    TA0CCR0 = TIMER_CCR0;
    TA0CTL = TASSEL_2 + ID_3 + MC_1; // configure and start timer (ID_3 = DIV8; MC_1 = count UP)

    checkpoint_timer_enable();
    checkpoint_timer_start();
}

void checkpoint_timer_reset(void)
{
    TA0R = 0;
}

#include "checkpoint.h"
#include "nvm.h"
volatile NVM uint16_t __isr_checkpoint_cnt = 0;
void __attribute__ ((interrupt(TIMER0_A0_VECTOR))) checkpoint_timer_isr (void)
{
    ++__isr_checkpoint_cnt;
    checkpoint_safe();
}

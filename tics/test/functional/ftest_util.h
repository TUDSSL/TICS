#ifndef FTEST_UTIL_H__
#define FTEST_UTIL_H__

#include "driverlib.h"
void GPIO_setAsOutputPin(uint8_t selectedPort, uint16_t selectedPins);
void GPIO_setOutputHighOnPin (uint8_t selectedPort, uint16_t selectedPins);
void GPIO_setOutputLowOnPin (uint8_t selectedPort, uint16_t selectedPins);
void GPIO_toggleOutputOnPin (uint8_t selectedPort, uint16_t selectedPins);
void PMM_unlockLPM5 (void);

void clear_volatile_mem(void);
void reset_mcu(void);
void checkpoint_and_restore(void);
void checkpoint_and_reset(void);
void checkpoint_and_reset_safe(void);

static inline void init_led(void)
{
    // Set P1.0 to output direction
    GPIO_setAsOutputPin(
            GPIO_PORT_P1,
            GPIO_PIN0
            );
    PMM_unlockLPM5();
}

static inline void led_toggle(void)
{
    GPIO_toggleOutputOnPin(
            GPIO_PORT_P1,
            GPIO_PIN0
            );
}

static inline void led_on(void)
{
    GPIO_setOutputHighOnPin(
            GPIO_PORT_P1,
            GPIO_PIN0
            );
}

static inline void led_off(void)
{
    GPIO_setOutputLowOnPin(
            GPIO_PORT_P1,
            GPIO_PIN0
            );
}

#endif /* FTEST_UTIL_H__ */

#ifndef LIBMSP_GPIO_H
#define LIBMSP_GPIO_H

#if defined(__MSP430FR5969__) || defined(__MSP430FR6989__) || defined(__MSP430FR5949__)
#define msp_gpio_unlock() PM5CTL0 &= ~LOCKLPM5
#elif defined(__MSP430F5340__)
#define msp_gpio_unlock() // no unlocking on this model
#else // MCU model
#error MCU not supported
#endif // MCU model

#endif // LIBMSP_GPIO_H

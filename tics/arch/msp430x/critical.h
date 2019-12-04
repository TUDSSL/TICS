#ifndef CRITICAL_H__
#define CRITICAL_H__

#include "msp430.h"
#include "checkpoint_timer.h"

//#define CRITICAL_START  __disable_interrupt
//#define CRITICAL_END    __enable_interrupt

#define CRITICAL_START checkpoint_timer_disable
#define CRITICAL_END checkpoint_timer_enable

#endif /* CRITICAL_H__ */

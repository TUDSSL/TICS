/**
 * @file wisp-base.h
 *
 * The interface for the standard WISP library, including all definitions
 *  required to communicate with an RFID reader and use onboard peripherals.
 *
 * @author Aaron Parks, Saman Naderiparizi
 */

#ifndef WISP_BASE_H_
#define WISP_BASE_H_

//#include <msp430.h>
#include "libwispbase/globals.h" // Get these outta here (breaks encapsulation barrier)
#ifdef LIBWISPBASE_INCLUDE_SPI
#include "libwispbase/spi.h"
#endif // LIBWISPBASE_INCLUDE_SPI
#include "libwispbase/uart.h"
#ifdef LIBWISPBASE_INCLUDE_ACCEL
#include "libwispbase/accel.h"
#endif // LIBWISPBASE_INCLUDE_ACCEL
#include "libwispbase/fram.h"
#include "libwispbase/rfid.h"
#include "libwispbase/wispGuts.h"
#include "libwispbase/timer.h"
#include "libwispbase/rand.h"

void WISP_init(void);
void WISP_getDataBuffers(WISP_dataStructInterface_t* clientStruct);

#endif /* WISP_BASE_H_ */

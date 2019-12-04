#ifndef LIBIO_LOG_H
#define LIBIO_LOG_H

/* Configuration:
 *   * Verbosity level: VERBOSE=<level>
 *
 * Usage:
 *   * LOG<level>()
 *   * BLOCK_LOG_BEGIN(),BLOCK_LOG(),BLOCK_LOG_END()
 */

#include "printf.h"

#if VERBOSE > 0

#define BLOCK_LOG_BEGIN() BLOCK_PRINTF_BEGIN()
#define BLOCK_LOG(...)    BLOCK_PRINTF(__VA_ARGS__)
#define BLOCK_LOG_END()   BLOCK_PRINTF_END()

#if VERBOSE > 0
#define LOG PRINTF
#else
#define LOG(...)
#endif

#if VERBOSE >= 2
#define LOG2 PRINTF
#else // VERBOSE < 2
#define LOG2(...)
#endif // VERBOSE < 2

#else // !VERBOSE*

#define LOG(...)
#define LOG2(...)

#define BLOCK_LOG_BEGIN()
#define BLOCK_LOG(...)
#define BLOCK_LOG_END()

#endif // !VERBOSE*

#endif // LIBIO_LOG_H

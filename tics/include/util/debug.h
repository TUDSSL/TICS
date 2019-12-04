#ifndef DEBUG_H__
#define DEBUG_H__


#ifdef DEBUG_PRINT

#include <stdio.h>
#include <stdarg.h>

#include "debug_arch.h"

#ifndef DEBUG_MODULE
#define DEBUG_MODULE "unregistered"
#endif

// LLVM varg issues //TODO could be solved after chaning call-conv
#ifdef __clang__
int debug_print(const char *format, ...);
#define dbgprintf(...) do { \
    _DBG_PRINTF_CALL("# %s: ", DEBUG_MODULE); \
    _DBG_PRINTF_CALL(__VA_ARGS__); \
} while (0)


#else /* GCC */
static int debug_print(const char *format, ...)
{
    int res;
    va_list args;

    _DBG_PRINTF_CALL("# %s: ", DEBUG_MODULE);
    va_start(args, format);
    res = _DBG_VPRINTF_CALL(format, args);
    va_end(args);
    return res;
}
#define dbgprintf debug_print

#endif /* __clang__ */

#else

#define dbgprintf(...)

#endif /* DEBUG_PRINT */

#endif /* DEBUG_H__ */

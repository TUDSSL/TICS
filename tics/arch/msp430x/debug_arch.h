#ifndef DEBUG_ARG_H__
#define DEBUG_ARG_H__

#include <stdint.h>

#include "config.h"
#include "printf.h"

#define _DBG_PRINTF_CALL    printf
#define _DBG_VPRINTF_CALL   vprintf

#ifdef CODE_MODEL_SMALL
#define DBG_PTR(ptr_)   ((uint16_t)((uintptr_t)(ptr_)))
#define DBG_PTR_PRINT   "0x%x"
#else /* CODE_MODEL_LARGE */
#define DBG_PTR(ptr_)   ((uint32_t)((uintptr_t)(ptr_)))
#define DBG_PTR_PRINT   "0x%lx"
#endif /* CODE_MODEL_SMALL */

#endif /* DEBUG_ARG_H__ */

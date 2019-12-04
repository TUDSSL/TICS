#ifndef LIBIO_PRINTF_H
#define LIBIO_PRINTF_H

/* Configuration:
 *
 *  Console lib selection:
 *
 *   * CONFIG_LIBEDB_PRINTF_<type>
 *       where <type> specifies the implementation for the PRINTF macro (the
 *       default) is either EIF or BARE
 *
 *   * CONFIG_LIBMSPCONSOLE_PRINTF: output channel via a HW UART module
 *
 *   * CONFIG_LIBMSPSOFTUART_PRINTF: output channel via a SW UART on GPIO pins
 *
 * Usage:
 *
 *   * use the PRINTF and BLOCK_PRINTF_BEGIN,BLOCK_PRINTF,BLOCK_PRINTF_END macros
 *   * (uncommon) use EIF_PRINTF or BARE_PRINTF specifically
 */

#include <stdio.h>

#if defined(CONFIG_LIBEDB_PRINTF_EIF) || defined(CONFIG_LIBEDB_PRINTF_BARE)

#include <libedb/edb.h>
#include <libedb/printf.h>

// Initialization call
#if defined(CONFIG_LIBEDB_PRINTF_BARE)
#define INIT_CONSOLE() BARE_PRINTF_ENABLE()
#else // CONFIG_LIBEDB_PRINTF_EIF
#define INIT_CONSOLE()
#endif

// The multi-statement printf, is...
#define BLOCK_PRINTF_BEGIN() ENERGY_GUARD_BEGIN()
#define BLOCK_PRINTF(...) BARE_PRINTF(__VA_ARGS__)
#define BLOCK_PRINTF_END() ENERGY_GUARD_END()

// By default, printf is...
#if defined(CONFIG_LIBEDB_PRINTF_EIF)
#define PRINTF(...) EIF_PRINTF(__VA_ARGS__)
#elif defined(CONFIG_LIBEDB_PRINTF_BARE)
#define PRINTF(...) BARE_PRINTF(__VA_ARGS__)
#endif

#elif defined(CONFIG_LIBMSPCONSOLE_PRINTF)

#include <libmspconsole/printf.h>

#define INIT_CONSOLE() mspconsole_init()
//#define INIT_CONSOLE() UART_init() //kwmaeng. changed to fix uart

// All special printfs fall back to the regular printf
#define BLOCK_PRINTF_BEGIN()
#define BLOCK_PRINTF(...) printf(__VA_ARGS__)
#define BLOCK_PRINTF_END()

#define EIF_PRINTF(...)  printf(__VA_ARGS__)
#define BARE_PRINTF(...) printf(__VA_ARGS__)

// Default printf also falls back to regular printf
#define PRINTF(...) printf(__VA_ARGS__)

#elif defined(CONFIG_LIBMSPSOFTUART_PRINTF)

#include <libmspsoftuart/printf.h>

#define INIT_CONSOLE() mspsoftuart_init()

// All special printfs fall back to the regular printf
#define BLOCK_PRINTF_BEGIN()
#define BLOCK_PRINTF(...) printf(__VA_ARGS__)
#define BLOCK_PRINTF_END()

#define EIF_PRINTF(...)  printf(__VA_ARGS__)
#define BARE_PRINTF(...) printf(__VA_ARGS__)

// Default printf also falls back to regular printf
#define PRINTF(...) printf(__VA_ARGS__)

#else // no printf

#define INIT_CONSOLE()

// All printfs fall back to nop
#define BLOCK_PRINTF_BEGIN()
#define BLOCK_PRINTF(...)
#define BLOCK_PRINTF_END()

#define PRINTF(...)

#define EIF_PRINTF(...)
#define BARE_PRINTF(...)

#endif // no printf

#endif // LIBIO_PRINTF_H

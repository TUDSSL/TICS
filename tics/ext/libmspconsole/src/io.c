#include <stdint.h>
#include <libwispbase/wisp-base.h>

#include <stdlib.h>

void mspconsole_init()
{
    UART_init();
}

int io_putchar(int c)
/*{
	unsigned r12;
        __asm__ volatile ("mov r12, %0\n"
			"mov r15, r12\n"
			"call #io_putchar_c\n"
			"mov %0, r12\n"
			: "=r" (r12)
			);
}

int io_putchar_c(int c)
*/{
    uint8_t ch = c;
    UART_send(&ch, 1);
    return c;
}

int io_puts_no_newline(const char *ptr)
{
    unsigned len = 0;
    const char *p = ptr;

    while (*p++ != '\0')
        len++;

    UART_send((uint8_t *)ptr, len);
    return len;
}

int io_puts(const char *ptr)
{
    unsigned len;

    len = io_puts_no_newline(ptr);

    // Semantics of puts are annoying...
    io_putchar('\n');

    return len;
}

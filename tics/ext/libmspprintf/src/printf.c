// Tiny printf implementation by oPossum from here:
// http://forum.43oh.com/topic/1289-tiny-printf-c-version/#entry10652
//
// NOTE: We are not using the libc printf because it's huge:
// https://e2e.ti.com/support/development_tools/compiler/f/343/t/442632

#include <stdlib.h>
#include <stdarg.h>

// The I/O backend library provides these
int io_putchar(int c);
int io_puts_no_newline(const char *);

#define PUTC(c) io_putchar(c)

static const unsigned long dv[] = {
//  4294967296      // 32 bit unsigned max
    1000000000,     // +0
     100000000,     // +1
      10000000,     // +2
       1000000,     // +3
        100000,     // +4
//       65535      // 16 bit unsigned max
         10000,     // +5
          1000,     // +6
           100,     // +7
            10,     // +8
             1,     // +9
};

static void xtoa(unsigned long x, const unsigned long *dp)
{
    char c;
    unsigned long d;
    if(x) {
        while(x < *dp) ++dp;
        do {
            d = *dp++;
            c = '0';
            while(x >= d) ++c, x -= d;
            PUTC(c);
        } while(!(d & 1));
    } else {
        PUTC('0');
    }
}

static void puth(unsigned n)
{
    static const char hex[16] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
    char c = hex[n & 15];
    PUTC(c);
}

int puts(const char *str){
	while(*str != 0){
		PUTC(*str++);
	}
	return 0;
}

int vprintf(const char *format, va_list a)
{
    char c;
    int i;
    long n;
    int fill_zeros;
    unsigned d;

    while((c = *format++)) {
        if(c == '%') {
            fill_zeros = 0;
parse_fmt_char:
            switch(c = *format++) {
                case 's':                       // String
                    io_puts_no_newline(va_arg(a, char*));
                    break;
                case 'c':                       // Char
                    PUTC(va_arg(a, int)); // TODO: 'char' generated a warning
                    break;
                case 'i':                       // 16 bit Integer
                case 'd':                       // 16 bit Integer
                case 'u':                       // 16 bit Unsigned
                    i = va_arg(a, int);
                    if(c == 'i' && i < 0) i = -i, PUTC('-');
                    xtoa((unsigned)i, dv + 5);
                    break;
                case 'l':                       // 32 bit Long
                case 'n':                       // 32 bit uNsigned loNg
                    n = va_arg(a, long);
                    if(c == 'l' &&  n < 0) n = -n, PUTC('-');
                    xtoa((unsigned long)n, dv);
                    break;
                case 'x':                       // 16 bit heXadecimal
                    i = va_arg(a, int);
                    d = i >> 12;
                    if (d > 0 || fill_zeros >= 4)
                        puth(d);
                    d = i >> 8;
                    if (d > 0 || fill_zeros >= 3)
                        puth(d);
                    d = i >> 4;
                    if (d > 0 || fill_zeros >= 2)
                        puth(d);
                    puth(i);
                    break;
                case '0':
                    c = *format++;
                    fill_zeros = c - '0';
                    goto parse_fmt_char;
                case 0: return 0;
                default: goto bad_fmt;
            }
        } else
bad_fmt:    PUTC(c);
    }
    return 0; // TODO: return number of chars printed
}


int printf(const char *format, ...)
{
    va_list a;
    int r;

    va_start(a, format);
    r = vprintf(format, a);
    va_end(a);

    return r;
}

#if 0
int printf(const char *format, ...)
{
    char c;
    int i;
    long n;
    int fill_zeros;
    unsigned d;

    va_list a;
    va_start(a, format);
    while((c = *format++)) {
        if(c == '%') {
            fill_zeros = 0;
parse_fmt_char:
            switch(c = *format++) {
                case 's':                       // String
                    io_puts_no_newline(va_arg(a, char*));
                    break;
                case 'c':                       // Char
                    PUTC(va_arg(a, int)); // TODO: 'char' generated a warning
                    break;
                case 'i':                       // 16 bit Integer
                case 'u':                       // 16 bit Unsigned
                    i = va_arg(a, int);
                    if(c == 'i' && i < 0) i = -i, PUTC('-');
                    xtoa((unsigned)i, dv + 5);
                    break;
                case 'l':                       // 32 bit Long
                case 'n':                       // 32 bit uNsigned loNg
                    n = va_arg(a, long);
                    if(c == 'l' &&  n < 0) n = -n, PUTC('-');
                    xtoa((unsigned long)n, dv);
                    break;
                case 'x':                       // 16 bit heXadecimal
                    i = va_arg(a, int);
                    d = i >> 12;
                    if (d > 0 || fill_zeros >= 4)
                        puth(d);
                    d = i >> 8;
                    if (d > 0 || fill_zeros >= 3)
                        puth(d);
                    d = i >> 4;
                    if (d > 0 || fill_zeros >= 2)
                        puth(d);
                    puth(i);
                    break;
                case '0':
                    c = *format++;
                    fill_zeros = c - '0';
                    goto parse_fmt_char;
                case 0: return 0;
                default: goto bad_fmt;
            }
        } else
bad_fmt:    PUTC(c);
    }
    va_end(a);
    return 0; // TODO: return number of chars printed
}
#endif

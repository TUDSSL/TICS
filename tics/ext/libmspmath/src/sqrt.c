#include <stdint.h>

#include "msp-math.h"

/* Square root by Newton's method */
uint16_t sqrt16(uint32_t x)
{
    uint16_t hi = 0xffff;
    uint16_t lo = 0;
    uint16_t mid = ((uint32_t)hi + (uint32_t)lo) >> 1;
    uint32_t s = 0;

    while (s != x && hi - lo > 1) {
        mid = ((uint32_t)hi + (uint32_t)lo) >> 1;
        s = mult16(mid, mid);
        if (s < x)
            lo = mid;
        else
            hi = mid;
    }

    return mid;
}

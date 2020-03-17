#include "timely.h"
#include <stdio.h>

#define WINDOW_SIZE 10

expires_meta_t _temperature_meta = {.expires_after_ms=1000}; /* Data expires in 1 second */
int temperature[WINDOW_SIZE];

int read_sensor(void) {
    return 42;
}

int main(void) {
    int max = 100;
    TIMELY_DISABLE_CHECKPOINTS();
    _temperature_meta.expires = TIMELY_GET_FUTURE_TIME(_temperature_meta.expires_after_ms);
    temperature[0] = read_sensor(); // Assign with timestamp
    TIMELY_ENABLE_CHECKPOINTS();

    /* Catch expiration */
    TIMELY_CHECKPOINT();
    TIMELY_DISABLE_CHECKPOINTS();
    _temperature_meta.last_ms_remaining = TIMELY_MS_REMAINING(_temperature_meta.expires);
    if (_temperature_meta.last_ms_remaining) {
        TIMELY_START_EXPIRE_TIMER_MS(_temperature_meta.last_ms_remaining);
        if (temperature[0] > max) {
            max = temperature[0];
        }
        TIMELY_STOP_EXPIRE_TIMER_MS();
        TIMELY_ENABLE_CHECKPOINTS();
    }
    else {
        TIMELY_ENABLE_CHECKPOINTS();
        printf("Temperature expired\r\n");
        temperature[0] = 0;
    }

    while (1) {
        /* Branch in time */
        expires_meta_t _timely_0_meta = {.expires_after_ms = 100};
        TIMELY_CHECKPOINT();
        TIMELY_DISABLE_CHECKPOINTS();
        _timely_0_meta.last_ms_remaining = TIMELY_MS_REMAINING(_timely_0_meta.expires_after_ms);
        if (_timely_0_meta.last_ms_remaining)
        {
            TIMELY_START_EXPIRE_TIMER_MS(_timely_0_meta.last_ms_remaining);
            if (temperature[0] > max) {
                max = temperature[0];
            }
            TIMELY_STOP_EXPIRE_TIMER_MS();
        }
        TIMELY_ENABLE_CHECKPOINTS();
        break;
    }
}

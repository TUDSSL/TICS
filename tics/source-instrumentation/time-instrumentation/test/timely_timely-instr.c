#include "timely.h"
#include <stdio.h>

#define WINDOW_SIZE 10

int temperature[WINDOW_SIZE];

int read_sensor(void) {
    return 42;
}

int main(void) {
    temperature[0] = read_sensor();

    expires_meta_t _timely_0_meta = {.expires_after_ms = 200};
    TIMELY_CHECKPOINT();
    TIMELY_DISABLE_CHECKPOINTS();
    _timely_0_meta.last_ms_remaining = TIMELY_MS_REMAINING(_timely_0_meta.expires_after_ms);
    if (_timely_0_meta.last_ms_remaining) {
        TIMELY_START_EXPIRE_TIMER_MS(_timely_0_meta.last_ms_remaining);
        temperature[0] += 10;
        TIMELY_STOP_EXPIRE_TIMER_MS();
        TIMELY_ENABLE_CHECKPOINTS();
    } else {
        TIMELY_ENABLE_CHECKPOINTS();
        printf("Temperature expired\r\n");
    }

    expires_meta_t _timely_1_meta = {.expires_after_ms = 1000};
    TIMELY_CHECKPOINT();
    TIMELY_DISABLE_CHECKPOINTS();
    _timely_1_meta.last_ms_remaining = TIMELY_MS_REMAINING(_timely_1_meta.expires_after_ms);
    if (_timely_1_meta.last_ms_remaining)
    {
        TIMELY_START_EXPIRE_TIMER_MS(_timely_1_meta.last_ms_remaining);
        temperature[0] += 10;
        TIMELY_STOP_EXPIRE_TIMER_MS();
    }
    TIMELY_ENABLE_CHECKPOINTS();
    printf("Temperature[0] = %d\r\n", temperature[0]);
}
